/**
 * @defgroup mission
 *
 * @brief Spacecraft Mission Module.
 *
 * @details Spacecraft Mission Module.
 *
 * @ingroup mission
 */

#ifndef __MISSION_H__
#define __MISSION_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

/* Stored in EEPROM */
typedef struct mission_state_data_t {
	/* 32b word */
	bool mission_inprogress:1;
	bool antenna_deployed:1;
	/* 32b word */
	uint16_t obc_reboots;
	uint16_t configuration_crcfail;
} mission_state_data_t;

typedef struct mission_state_t {
	mission_state_data_t data;
	uint16_t crc;
} mission_state_t;

/* Stored in RAM (volatile) */
typedef struct subsystems_ok_t {
	/* 32b word */
	bool eeprom:1;
	bool fram:1;
	bool camera:1;
	bool gps:1;
	bool imu:1;
	bool transmitter:1;
	bool receiver:1;
	bool eps:1;
} subsystems_ok_t;


mission_state_t mission_state;

subsystems_ok_t subsystems_ok;



// Custom structs
typedef struct task_t {
	 /* Rate at which the task should tick, and hence its priority */
   uint64_t period;

	 /* Function to call for task's tick which takes and returns the state */
   int8_t (*TickFct)(int8_t);

} task_t;
//task opmode_tasks[]; // you put in opmode_tasks[<mode whose tasks I want>] and you get a pointer to that array of tasks

// #DEFINE < ENUM because automatic numbering in latter (obv)
typedef enum mode_n {
   FBU,//first boot-up
   AD, //antenna-deployment
   NF, //nominal-functionality
   LP, //low-power
   SM, //safe-mode
   CFU,//configuration file update
   PT, //picture taking
   DL  //data downlinking
} mode_n;

//Can be assigned to general tasks in mission code, each task doesn't have to be mode specfic

typedef enum FBU_task{
	//LISTEN_FOR_GS,
	SAVE_MORSE_TELEMETRY,
	EXIT_FBU
} FBU_task;

typedef enum AD_task{
	AD_SAVE_MORSE_TELEMETRY,
	//AD_CHECK_HEALTH,
	TRANSMIT_MORSE_TELEMETRY,
	//AD_CHECK_VOLTAGE,
	//WAIT_FOR_RECHARGE, 
	ANTENNA_DEPLOY_ATTEMPT, //may be able to remove and do in either of previous tasks
	EXIT_AD // To NF or LP
} AD_task;

typedef enum NF_task {
	SAVE_EPS_HEALTH,
	TRANSMIT,
	SAVE_ATTITUDE,
	SAVE_GPS_POS,
	PROCESS_GS_COMMAND,
	CHECK_HEALTH
} NF_task;

typedef enum LP_task{
	//All data gathering suspended by changing mode unless already added to scheduler, need to halt this?
	LP_POWER_DOWN,
	LP_LISTEN_FOR_GS, //Config update from this task
	LP_CHECK_HEALTH,
	EXIT_LP
} LP_task;

typedef enum SM_task{
	//All data gathering suspended by changing mode unless already added to scheduler, need to halt this?
	SM_POWER_DOWN,
	SM_LISTEN_FOR_GS, //return to NF, got to CFU or reboot through this 
	SM_CHEK_HEALTH, //Periodically read, save and transmit health packets
	EXIT_SM //May be achieved through SM_LISTEN_FOR_GS
} SM_task;

typedef enum CFU_task{
	//All data gathering suspended by changing mode unless already added to scheduler, need to halt this?
	READ_CF, //Also check values are in range
	EXIT_CFU //return to previous mode
} CFU_task;

typedef enum PT_task{
	SAVE_IMAGE,
	EXIT_PT
} PT_task;

typedef enum DL_task{
	DOWNLINK,
	TAKE_PICTURE,
	EXIT_DL
} DL_task;


typedef struct opmode_t {
   void (*Mode_startup)(void); // Points to start-up code for this mode
   task_t* opmode_tasks;
   uint8_t num_tasks;
} opmode_t;


typedef enum GS_command {
	IMAGE_REQUEST,
	ACK_REC_PACKETS,
	DL_REQUEST,
	UPDATE_CONFIG,
	ENTER_SAFE_MODE,
	MANUAL_OVERRIDE
} GS_command;

// Main functions
void Mission_init(void);
void Mission_loop(void);
void Mission_SEU(void);

// Start-up
void Mode_init(int8_t type);


//Timers

void timer0_isr();
void timer1_isr();
void timer2_isr();
void timer3_isr();
void timer4_isr();
void timer5_isr();

// Tasks
int8_t save_eps_health_data(int8_t r);
int8_t transmit_next_telemetry(int8_t t);

#endif /*  __MISSION_H__ */
