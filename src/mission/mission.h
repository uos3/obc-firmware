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
   //Dynamic
   uint8_t running;      /* 1 indicates task is running */
   int8_t state;            /* Current state of state machine */
   //Fixed
   uint32_t period;      /* Rate at which the task should tick */
   uint32_t elapsed_time; /* Time since task's previous tick */
   int8_t (*TickFct)(int8_t);  /* Function to call for task's tick which takes and returns the state */
} task_t;
//task opmode_tasks[]; // you put in opmode_tasks[<mode whose tasks I want>] and you get a pointer to that array of tasks

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

typedef struct opmode_t {
   void (*Mode_startup)(void); // Points to start-up code for this mode
   task_t* opmode_tasks;
   uint8_t num_tasks;
} opmode_t;


// Main functions
void Mission_init(void);
void Mission_loop(void);
void Mission_SEU(void);

// Start-up
void Modes_init(void);
task_t* FBU_tasks_init(void);
task_t* AD_tasks_init(void);

// First-Boot Up Mode
void Mode_startup_FBU(void);
int8_t Enter_AD_tickFct(int8_t);
int8_t Save_telem_tickFct(int8_t);

// Antenna-Deployment Mode
void Mode_startup_AD(void);
int8_t Transmit_morse_tickFct(int8_t);
int8_t Save_telem_2_tickFct(int8_t);
int8_t Deploy_antenna_tickFct(int8_t);
void Notify_successful_deploy(void); // interrupt

// Scheduler
void Switch_task(uint8_t new_mode);

// Utility


#endif /*  __MISSION_H__ */
