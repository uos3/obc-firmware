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
   uint32_t period;

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



typedef enum NF_task {
	SAVE_EPS_HEALTH,
	TRANSMIT,
	SAVE_ATTITUDE,
	SAVE_GPS_POS,
	PROCESS_GS_COMMAND,
	CHECK_HEALTH
} NF_task;

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

// Tasks
int8_t save_eps_health_data(int8_t r);
int8_t transmit_next_telemetry(int8_t t);

#endif /*  __MISSION_H__ */
