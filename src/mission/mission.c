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

#include "../board/memory_map.h"

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

int8_t timer_to_task[6]; // timer_id -> task_id (reset look-up table)

// Error detection
uint8_t flash_errors;
uint8_t ram_errors;

// State variables
uint16_t data_packets_pending;
uint8_t rx_noisefloor;

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

uint32_t ulPeriod;

void set_timer_for_task(uint8_t id, uint8_t task_id, uint32_t period, void (*timer_isr)() ){
  TimerLoadSet(ALL_TIMER_BASES[id], TIMER_A, period-1); // prime it (-1 is unnecessery but keep it just to avoid sysclock conflict?)

  TimerIntRegister(ALL_TIMER_BASES[id], TIMER_A, timer_isr); // this is for dynamic interrupt compilation

  TimerEnable(ALL_TIMER_BASES[id], TIMER_A); 	 // start timer

  timer_to_task[id] = task_id;
}

void stop_timer_for_task(uint8_t id){
  TimerDisable(ALL_TIMER_BASES[id], TIMER_A);
  timer_to_task[id] = -1; //free timer
}

int8_t get_available_timer(){
  int8_t id = -1;

  for (uint8_t i=0; i<6; i++){
    if (timer_to_task[i] == -1) {
      id = i;
      break;
    }
  }

  return id;
}

void Mission_init(void)
{
	LED_on(LED_B);

	/* Leeeeeeeeeeerooooy Jenkinnnnns! */
  Board_init();
  WDT_kick();
	EEPROM_init();
	I2C_init(0);
  UART_init(UART_INTERFACE, 9600);

	int16_t temp = Temperature_read_tmp100();
	char output[100];
	sprintf(output,"%d\r\n", temp);
	UART_puts(UART_INTERFACE, output);

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

  ulPeriod = (SysCtlClockGet()) ; // 1Hz update rate
   // third will miss sometimes if processor busy

  Mode_init(NF);
}

void queue_task(int8_t task_id){
  task_q[no_of_tasks_queued] = task_id;
  no_of_tasks_queued++;

  int8_t timer_id = get_available_timer();
  set_timer_for_task(timer_id, task_id, ulPeriod, &timer0_isr);
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

      // for (uint8_t i=0; i<tasks; i++)
        // queue_task(tasks[i]);
      queue_task(SAVE_EPS_HEALTH);

      // task_q[no_of_tasks_queued] = TRANSMIT;
      // no_of_tasks_queued++;

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

uint8_t telemetry[104]; //832 bits (104 bytes) available for each payload
uint8_t buffer_tracker;

void start_telemetry(){
  buffer_tracker = 0;
}

void add_telemetry(uint8_t* value, uint8_t bytes){
  uint8_t index = 0;
  while (bytes-->0){
    telemetry[buffer_tracker++] = value[index++];
  }
}

uint16_t consume_available_location(){
	uint8_t fram_availability[609];
	FRAM_read(FRAM_TABLE_OFFSET, fram_availability, 609);
	uint16_t available_slot = 0;

	// Cycle through all bits/slots to find an available one
	for (uint8_t i=0; i<609; i++)
		for (uint8_t bit=0; i<8; i++)
			// If the current bit shows availability
			if (0b11111110 & (fram_availability[i] >> bit) == FRAM_AVAILABLE) {
				// Mark as consumed
				fram_availability[i] |= 1 << bit;
				FRAM_write(FRAM_TABLE_OFFSET + i, fram_availability[i], 1);

				return FRAM_PAYLOAD_OFFSET + (available_slot * FRAM_PAYLOAD_SIZE);
			} else
				available_slot++;
}

void end_telemetry(){
	// Find available location
	uint16_t location = consume_available_location();

  // Save telemetry to FRAM
	FRAM_write(location, telemetry, FRAM_PAYLOAD_SIZE);
}

uint16_t perform_subsystem_check(){
	uint16_t status = 0;

	for (int i=0; i<14; i++)
		status |= 0; // insert system check here

	return status;
}

int8_t save_eps_health_data(int8_t t){
  uint8_t i; // loop counter
	uint32_t eeprom_read;
  start_telemetry();

  // Generate dataset id
  add_telemetry((uint16_t)0, 2);

  // Get timestamp
  add_telemetry((uint32_t)0, 4);

  // Get TOBC values
  add_telemetry(get_temp(TEMP_OBC), 1);
  add_telemetry(get_temp(TEMP_RX), 1);
  add_telemetry(get_temp(TEMP_TX), 1);
  add_telemetry(get_temp(TEMP_PA), 1);

  // read EEPROM for reboot_count
	EEPROM_read(EE_REBOOT_COUNT, &eeprom_read, 1);
	add_telemetry((uint8_t)eeprom_read, 1);

  // [CLARIFY] data_packets_pending
	add_telemetry((uint16_t)data_packets_pending, 2);

  // [CLARIFY] rx_noisefloor
	add_telemetry((uint8_t)rx_noisefloor, 1);

	// Flash and RAM errors in same byte
  add_telemetry( (flash_errors << 4) + ram_errors, 1);

  // Get 45 EPS values (16-bit fields)
  for (i=0; i<45; i++){
    uint16_t eps_field;
    EPS_getBatteryInfo(&eps_field, i);
    add_telemetry(eps_field, 2);
  }

  // Check subsystems and record status of 14
	uint16_t subsystem_status = perform_subsystem_check();
	// read antenna switch state (and ask where this can be attached)
	subsystem_status |= (uint8_t)Antenna_read_deployment_switch() << 14;

	add_telemetry(subsystem_status, 2);

  end_telemetry();

  UART_puts(UART_INTERFACE, "[TASK #001] Finished saving EPS health data.\r\n");
}

int8_t transmit_next_telemetry(int8_t t){
  uint8_t data_read[TELEMETRY_SIZE];

  // Read lookup table to find where to look in FRAM


  // Load packet from FRAM
  // get next packet to read
  //FRAM_read(0x00000, data_read, TELEMETRY_SIZE);

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
