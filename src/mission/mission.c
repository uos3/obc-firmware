/**
 * @ingroup mission
 *
 * @file mission.c
 *
 * @{
 */

#include "../firmware.h"

// Array of modes which can be accessed via enum indexing for simplicity (instead of random nums)
opmode_t modes[2]; // debug no., should be 8 on final
uint8_t current_mode;

// Tasks associated with every mode
task_t FBU_tasks[2], AD_tasks[3];

void Mission_init(void)
{
  /* Leeeeeeeeeeerooooy Jenkinnnnns! */
  Board_init();
  WDT_kick();
  
  Modes_init();
}

void Mission_loop(void)
{
  LED_set(LED_A, 0);

  /* On period */
  Delay_ms(100);
  WDT_kick();

  LED_set(LED_B, 0);

  /* Off period */
  Delay_ms(100);
  WDT_kick();
}

void Mission_SEU(void)
{
  /* TODO: Power Reset / Reboot */
	
}

// Initialisation of scheduler
void Modes_init(void) {
  modes[FBU].Mode_startup = &Mode_startup_FBU;
  modes[FBU].num_tasks = 2;
  modes[FBU].opmode_tasks = FBU_tasks_init(); // get address of the list of tasks in this mode

  modes[AD].Mode_startup = &Mode_startup_AD;
  modes[AD].num_tasks = 3;
  modes[AD].opmode_tasks = AD_tasks_init();
}

task_t* FBU_tasks_init(void){
  int8_t tasksNum = -1;

  FBU_tasks[++tasksNum].state = -1;
  FBU_tasks[tasksNum].period = 1800;
  FBU_tasks[tasksNum].elapsed_time = FBU_tasks[tasksNum].period;
  FBU_tasks[tasksNum].running = 0;
  FBU_tasks[tasksNum].TickFct = &Enter_AD_tickFct;

  FBU_tasks[++tasksNum].state = -1;
  FBU_tasks[tasksNum].period = 300;
  FBU_tasks[tasksNum].elapsed_time = FBU_tasks[tasksNum].period;
  FBU_tasks[tasksNum].running = 0;
  FBU_tasks[tasksNum].TickFct = &Save_telem_tickFct;

  return FBU_tasks;
}

task_t* AD_tasks_init(void){
  int8_t tasksNum = -1;

  AD_tasks[++tasksNum].state = -1;
  AD_tasks[tasksNum].period = 60;
  AD_tasks[tasksNum].elapsed_time = AD_tasks[tasksNum].period;
  AD_tasks[tasksNum].running = 0;
  AD_tasks[tasksNum].TickFct = &Transmit_morse_tickFct;

  AD_tasks[++tasksNum].state = -1;
  AD_tasks[tasksNum].period = 300;
  AD_tasks[tasksNum].elapsed_time = AD_tasks[tasksNum].period;
  AD_tasks[tasksNum].running = 0;
  AD_tasks[tasksNum].TickFct = &Save_telem_2_tickFct;

  AD_tasks[++tasksNum].state = -1;
  AD_tasks[tasksNum].period = 16200;
  AD_tasks[tasksNum].elapsed_time = AD_tasks[tasksNum].period;
  AD_tasks[tasksNum].running = 0;
  AD_tasks[tasksNum].TickFct = &Deploy_antenna_tickFct;

  return AD_tasks;
}

// First-Boot Up Mode
void Mode_startup_FBU(void){
  // there is no startup code for this mode
}

int8_t Enter_AD_tickFct(int8_t state){
  return state;
}

int8_t Save_telem_tickFct(int8_t state){
  return state;
}

// Antenna-Deployment Mode
void Mode_startup_AD(void){
  // there is no startup code for this mode
}

int8_t Transmit_morse_tickFct(int8_t state){
  return state;
}

int8_t Save_telem_2_tickFct(int8_t state){
  return state;
}

int8_t Deploy_antenna_tickFct(int8_t state){
  return state;
}

void Notify_successful_deploy(void){

}

// Tasks
void Switch_task(uint8_t new_mode){
  
  // run start-up code of tasks
  
  // change pointer of current_mode
  
}

// Utility

/**
 * @}
 */
