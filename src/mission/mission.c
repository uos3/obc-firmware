/**
 * @ingroup mission
 *
 * @file mission.c
 *
 * @{
 */

#include "../firmware.h"
#include "mission.h"

#define UART_INTERFACE UART_GNSS

opmode_t modes[8];

// Run-time
#define MAX_TASKS 10
volatile int8_t task_q[MAX_TASKS];  // Task queue directly accessed by interrupts
Node*           task_pq;            // Task priority queue constructed from previous q indirectly
volatile uint8_t no_of_tasks_queued;

// Properties
task_t* current_tasks;
uint8_t current_mode; // for deciding which enums to use

void Mission_init(void)
{
  /* Leeeeeeeeeeerooooy Jenkinnnnns! */
  Board_init();
  WDT_kick();

  UART_init(UART_INTERFACE, 9600);
  UART_puts(UART_INTERFACE, "\r\n\n**BOARD & DRIVER INITIALISED**\r\n");

  // Initialise the task queue as empty
  for (uint8_t i=0; i<MAX_TASKS; i++){
    task_q[i] = -1;
  }
  no_of_tasks_queued = 0;

  Mode_init(NF);
}

void Mode_init(int8_t type){
  switch(type){
    case NF:{
      Node* task_pq = newNode(0, 0);

      //task_t* tasks = (task_t *) malloc (sizeof(task_t)*6);

      task_t* tasks = (task_t *) malloc (sizeof(task_t)*2);

      tasks[SAVE_EPS_HEALTH].period = 300;
      tasks[SAVE_EPS_HEALTH].TickFct = &save_eps_health_data;

      tasks[TRANSMIT].period = 60;
      tasks[TRANSMIT].TickFct = &transmit_next_telemetry;

      current_mode = NF;
      current_tasks = tasks;

      task_q[no_of_tasks_queued] = SAVE_EPS_HEALTH;
      no_of_tasks_queued++;

    } break;
  }
}

void Mission_loop(void)
{
  // Sort list of pending tasks by their priorities
  for (uint8_t i=0; i<MAX_TASKS; i++){
    if (task_q[i] != -1)
      circ_push(&task_pq, task_q[i], current_tasks[task_q[i]].period);

      // Remove task from standard queue
      task_q[i] = -1;
      no_of_tasks_queued--;
  }

  // If there is a task to execute
  if (!circ_isEmpty(&task_pq)){
    // peek
    uint8_t todo_task_index = circ_peek(&task_pq);
    char output[100];
    sprintf(output,"%+06d\r\n", todo_task_index);
    UART_puts(UART_INTERFACE, output);

    // execute
    UART_puts(UART_INTERFACE, "**executing**\r\n");

    current_tasks[todo_task_index].TickFct(NULL);

    // pop
    circ_pop(&task_pq);
  }

  // Sleep
  UART_puts(UART_INTERFACE, "sleeping...\r\n");
  LED_toggle(LED_B);
  Delay_ms(100);
  WDT_kick();
}

void Mission_SEU(void)
{
	/* TODO: Power Reset / Reboot */

}

int8_t save_eps_health_data(int8_t t){
  UART_puts(UART_INTERFACE, "[TASK] Saving EPS health data\r\n");
}

int8_t transmit_next_telemetry(int8_t t){
  UART_puts(UART_INTERFACE, "[TASK] Transmit next telemetry\r\n");
}

// Utility

/**
 * @}
 */
