/**
 * @ingroup mission
 *
 * @file mission.c
 *
 * @{
 */

#include "../firmware.h"

#include <stdio.h>

opmode_t modes[8];

// Run-time
#define MAX_TASKS 10
volatile int8_t task_q[MAX_TASKS]; // Task queue directly accessed by interrupts
Node* task_pq[MAX_TASKS]; // Task priority queue constructed from previous q indirectly

// Properties
task_t* current_tasks;
uint8_t current_mode; // for deciding which enums to use

void Mission_init(void)
{
  /* Leeeeeeeeeeerooooy Jenkinnnnns! */
  Board_init();
  WDT_kick();

  for (uint8_t i=0; i<MAX_TASKS; i++){
    task_q[i] = -1;
  }
  Mode_init(NF);
}

void Mode_init(int8_t type){
  switch(type){
    case NF:{
      task_t* tasks = (task_t *) malloc (sizeof(task_t)*6);

      tasks[SAVE_EPS_HEALTH].period = 300;
      tasks[SAVE_EPS_HEALTH].TickFct = &save_eps_health_data;

      tasks[TRANSMIT].period = 60;
      tasks[TRANSMIT].TickFct = &transmit_next_telemetry;

      current_mode = NF;
    } break;
  }
}

void Mission_loop(void)
{
  // Sort list of pending tasks by their priorities
  for (uint8_t i=0; i<MAX_TASKS; i++){
    if (task_q[i] != -1)
      push(&task_pq, task_q[i], current_tasks[task_q[i]].period);
  }

  // peek
  task_t todo_task;
  peek(&todo_task);

  // execute
  todo_task.TickFct;

  // pop
  pop(&todo_task);

  // Sleep
  LED_toggle(LED_B);
  Delay_ms(100);
  WDT_kick();
}

void Mission_SEU(void)
{
  /* TODO: Power Reset / Reboot */

}

void save_eps_health_data(){

}

void transmit_next_telemetry(){

}

// Utility

/**
 * @}
 */
