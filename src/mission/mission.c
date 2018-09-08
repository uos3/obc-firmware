/**
 * @ingroup mission
 *
 * @file mission.c
 *
 * @{
 */

#include "../firmware.h"
#include "mission.h"

#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "inc/hw_memmap.h"

#define UART_INTERFACE UART_GNSS

#define TELEMETRY_SIZE 107 // 104 (tel) + 2 (timestamp) + 1 (id)

opmode_t modes[8];

// Run-time
#define MAX_TASKS 10
volatile int8_t task_q[MAX_TASKS];  // Task queue directly accessed by interrupts
Node*           task_pq;            // Task priority queue constructed from previous q indirectly
volatile uint8_t no_of_tasks_queued;

// Properties
task_t* current_tasks;
uint8_t current_mode; // for deciding which enums to use

uint8_t timer_to_task[6]; // timer_id -> task_id (reset look-up table)

void timer0_isr(){
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
  UART_puts(UART_INTERFACE, "**timer0_interrupt!**\r\n");
}

void timer1_isr(){
	TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
  UART_puts(UART_INTERFACE, "**timer1_interrupt!**\r\n");

  // get task id and etc..

}

uint32_t ALL_TIMER_PERIPHERALS[6] =
  { SYSCTL_PERIPH_TIMER0, SYSCTL_PERIPH_TIMER1, SYSCTL_PERIPH_TIMER2,
    SYSCTL_PERIPH_TIMER3, SYSCTL_PERIPH_TIMER4, SYSCTL_PERIPH_TIMER5 };

uint32_t ALL_TIMER_BASES[6] =
  {TIMER0_BASE, TIMER1_BASE, TIMER2_BASE, TIMER3_BASE, TIMER4_BASE, TIMER5_BASE};

void set_timer_for_task(uint8_t id, uint32_t period, void (*timer_isr)() ){
  TimerLoadSet(ALL_TIMER_BASES[id], TIMER_A, period-1); // prime it (-1 is unnecessery but keep it just to avoid sysclock conflict?)

  TimerIntRegister(ALL_TIMER_BASES[id], TIMER_A, timer_isr); // this is for dynamic interrupt compilation

  TimerEnable(ALL_TIMER_BASES[id], TIMER_A); 	 // start timer
}

void stop_timer_for_task(uint8_t id){
  TimerDisable(ALL_TIMER_BASES[id], TIMER_A);
}

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



  for (uint8_t i=0; i<6; i++) SysCtlPeripheralEnable(ALL_TIMER_PERIPHERALS[i]);
  for (uint8_t i=0; i<6; i++) while(!SysCtlPeripheralReady(ALL_TIMER_PERIPHERALS[i]));
  for (uint8_t i=0; i<6; i++){
    TimerConfigure(ALL_TIMER_BASES[i], TIMER_CFG_PERIODIC); // set it to periodically trigger interrupt
    TimerIntEnable(ALL_TIMER_BASES[i], TIMER_TIMA_TIMEOUT); // enable interrupt
  }

  uint32_t ulPeriod = (SysCtlClockGet()) ; // 1Hz update rate
   // third will miss sometimes if processor busy

  // -- get available timer

  set_timer_for_task(0, ulPeriod, &timer0_isr);
  set_timer_for_task(1, ulPeriod*2, &timer1_isr);

  Delay_ms(3500);

  stop_timer_for_task(0);
  stop_timer_for_task(1);

  Delay_ms(3500);

  set_timer_for_task(0, ulPeriod/2, &timer0_isr);
  set_timer_for_task(1, ulPeriod, &timer1_isr);

  Delay_ms(3500);

  stop_timer_for_task(0);
  stop_timer_for_task(1);

  Delay_ms(3500);

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

      task_q[no_of_tasks_queued] = TRANSMIT;
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
    // char output[100];
    // sprintf(output,"%+06d\r\n", todo_task_index);
    // UART_puts(UART_INTERFACE, output);

    // execute
    UART_puts(UART_INTERFACE, "**executing**\r\n");

    current_tasks[todo_task_index].TickFct(NULL);

    // reset timer for that tasks


    // pop
    circ_pop(&task_pq);
  }

  // Sleep
  UART_puts(UART_INTERFACE, "sleeping...\r\n");
  LED_toggle(LED_B);
  Delay_ms(1000);
  WDT_kick();
}

void Mission_SEU(void)
{
	/* TODO: Power Reset / Reboot */

}



int8_t save_eps_health_data(int8_t t){
  char telemetry[104]; //832 bits (104 bytes) available for each payload
  uint8_t i; // loop counter

  // Generate dataset id
  telemetry[0] = (uint32_t) 0;

  // Get timestamp
  telemetry[2] = (uint16_t) 0;


  uint8_t pos = 3; // current byte / 104 in payload

  // Get TOBC values
  for (i=0; i<10; i++)
    telemetry[pos+i] = 0;
  pos+=10;

  // Get 45 EPS values
  for (i=0; i<45; i++)
    telemetry[pos+i] = 0;
  pos+=45;

  // Check subsystems and record status of 14
  for (i=0; i<14; i++)
    telemetry[pos+i] = 0;
  pos+=14;

  // Save telemetry to FRAM

  // Insert new entry into FRAM lookup table (packet_id -> FRAM pos)
  // ..do this by finding next available pos

  UART_puts(UART_INTERFACE, "[TASK #001] Finished saving EPS health data.\r\n");
}

int8_t transmit_next_telemetry(int8_t t){
  uint8_t data_read[TELEMETRY_SIZE];

  // Read lookup table to find where to look in FRAM


  // Load packet from FRAM
  // get next packet to read
  // FRAM_read(0x00000, data_read, TELEMETRY_SIZE);

  // add authentication hash

  // add crc


  // Transfer to TX transceiver


  // Transmit


  UART_puts(UART_INTERFACE, "[TASK #002] Finished transmit next telemetry\r\n");
}

// Utility

/**
 * @}
 */
