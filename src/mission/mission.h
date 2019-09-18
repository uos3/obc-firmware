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

typedef struct timer_properties_t {
	uint32_t Timer_period[6];
} timer_properties_t;


/* Task structure */
typedef struct task_t {
/* Rate at which the task should tick, and hence its priority */
   uint32_t period;

/* Function to call for task's tick which takes and returns the state */
   int8_t (*TickFct)(int8_t);

} task_t;
//task opmode_tasks[]; // you put in opmode_tasks[<mode whose tasks I want>] and you get a pointer to that array of tasks

/* Modes of sattelite operation */
typedef enum mode_n {
   FBU,				//first boot-up
   AD, 				//antenna-deployment
   NF, 				//nominal-functionality
   LP, 				//low-power
   SM, 				//safe-mode
   CFU,				//configuration file update
   PT, 				//picture taking
   DL  				//data downlinking
} mode_n;

//Can be assigned to general tasks in mission code, each task doesn't have to be mode specfic

typedef enum FBU_task{
	FBU_SAVE_EPS_HEALTH,
	EXIT_FBU
} FBU_task;

typedef enum AD_task{
	AD_SAVE_EPS_HEALTH,
	TRANSMIT_MORSE_TELEMETRY,
	ANTENNA_DEPLOY_ATTEMPT,
	AD_PROCESS_GS_COMMAND = 6	//this one has specified value because it needs to be mode-independent (the same in every mode)
								// - so the receiver pin interrupt will be always referring to correct task
} AD_task;

typedef enum NF_task {
	NF_SAVE_EPS_HEALTH,
	NF_SAVE_GPS_POS,
	NF_SAVE_ATTITUDE,
	NF_TRANSMIT_TELEMETRY,
	NF_CHECK_HEALTH, 		//WHAT IS GATHERED IN CHECK HEALTH?
	NF_TAKE_PICTURE,
	NF_PROCESS_GS_COMMAND = 6
} NF_task;

typedef enum LP_task{
//All data gathering suspended by changing mode unless already added to scheduler, need to halt this?
	LP_SAVE_EPS_HEALTH,
	LP_CHECK_HEALTH,
	LP_TRANSMIT_TELEMETRY,
	LP_PROCESS_GS_COMMAND = 6
} LP_task;

typedef enum SM_task{
	//All data gathering suspended by changing mode unless already added to scheduler, need to halt this?
	SM_SAVE_EPS_HEALTH,
	SM_CHECK_HEALTH,
	SM_TRANSMIT_TELEMETRY,
	SM_REBOOT_CHECK,
	SM_PROCESS_GS_COMMAND = 6
} SM_task;

/* not needed - CFU and PT modes are just functions - not a typical mode of operation
typedef enum CFU_task{
	//All data gathering suspended by changing mode unless already added to scheduler, need to halt this?
	READ_CF, //Also check values are in range
	EXIT_CFU //return to previous mode
} CFU_task;
typedef enum PT_task{
	PT_TAKE_PICTURE,
	EXIT_PT
} PT_task;
*/
typedef enum DL_task{
	DL_SAVE_EPS_HEALTH,
	DL_SAVE_GPS_POSITION,
	DL_SAVE_ATTITUDE,
	DL_POLL_TRANSMITTER,
	DL_CHECK_HEALTH,
	DL_TAKE_PICTURE
} DL_task;

/* Operational mode structure */
typedef struct opmode_t {
   bool (*Mode_startup)(void);	//Points to start-up code for this mode
   task_t* opmode_tasks;
   uint8_t num_tasks;
} opmode_t;

/* This structure represents three types of uplink transmission - basing on this different actions should be taken */
/* For the values of each type - refer to the TMTC file, Type of Packet field */
typedef enum GS_command {
	ACK_REC_PACKETS = 10,
	UPDATE_CONFIG = 11,
	GREETINGS_MESSAGE = 12,
	TC_TELECOMMAND = 13
} GS_command;


// Main functions
void Mission_init(void);
void Mission_loop(void);
void Mission_SEU(void);

// Start-up
void Mode_init(int8_t type);

//Timers
void timer0_isr(void);
void timer1_isr(void);
void timer2_isr(void);
void timer3_isr(void);
void timer4_isr(void);
void timer5_isr(void);

// Tasks
//TRANSFER FROM .C FILE

#endif /*  __MISSION_H__ */