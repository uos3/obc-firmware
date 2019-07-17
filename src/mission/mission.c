/**
 * @ingroup mission
 *
 * @file mission.c
 *
 * -------------------_IMPORTANT---------------------------------
 * LIST ALL "TO DO" NEXT TO THE FUNCTION PROTOTYPE
 * OR HERE, ABOVE THE LIBRARIES, IF THEY DON'T MATCH ANY FUNCTION
 * 
 * ------for debugging, uncomment lines in the wdt.c with the WatchdogStallEnable
 * 
 * @{
 */

#include "../firmware.h"
#include "mission.h"
#include <string.h>

#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "inc/hw_memmap.h"
 
#include "../board/memory_map.h"
#include "../board/radio.h"

//#define UART_INTERFACE UART_GNSS

#define TELEMETRY_SIZE 107 // 104 (tel) + 2 (timestamp) + 1 (id)

#define BATTERY_VOLTAGE 6 //COMMENT WHEN USING EPS!!!

#define BATTERY_THRESHOLD 7.5
//--------------------------------PROTOTYPES OF FUNCTIONS---------------------------------
//Main functions
void Mission_init(void);
void Mission_loop(void);
void Mission_SEU(void);

//modes initialisation
void Mode_init(int8_t type);

//System tasks functions
int8_t save_eps_health_data(int8_t t);      //TODO: revision of checked/saved parameters
int8_t save_imu_data(int8_t t);             //no definition!!!
int8_t save_gps_data(int8_t t);             //completed
int8_t check_health_status(int8_t t);       //TODO: write body of this function
int8_t save_image_data(void);               //TODO: finish camera driver
int8_t ad_deploy_attempt(int8_t t);         
int8_t save_morse_telemetry(int8_t t);      //TODO: revise the information/data we want to inlude here
int8_t transmit_morse_telemetry(int8_t t);  //TODO: write the body of the function
int8_t transmit_next_telemetry(int8_t t);   //TODO: revise what is written and complete the body
int8_t send_morse_telemetry (int8_t t);     //TODO: revise, look for uprgades, data we want to include
int8_t exit_fbu (int8_t t);                 //completed
int8_t exit_ad (int8_t t);                  //TODO: why any argument?
int8_t save_attitude (int8_t t);            //completed
int8_t sm_reboot(int8_t t);                 //TODO: write a body of function
int8_t process_gs_command(int8_t t);        //TODO: revise what is done
int8_t poll_transmitter(int8_t t);          //TODO: revise what is done, what is missing
int8_t take_picture(int8_t);
uint16_t perform_subsystem_check();         //TODO: write the body of function
void update_radio_parameters();             //TODO: revise what need to be done 

//creating Morse signals
static void cw_tone_on(void);
static void cw_tone_off(void);

//queue implementation and transition functions
void suspend_all_tasks();
void mode_switch(uint8_t mode);
void queue_task(int8_t task_id, uint64_t task_period);

//Mode initialisation functions
bool fbu_init(void);
bool ad_init(void);
bool nf_init(void);
bool lp_init(void);
bool sm_init(void);
bool cfu_init(void);
bool pt_init(void);
bool dl_init(void);

//Timer functions
int8_t get_available_timer(void);
int8_t free_timers(void);
void timer_init(void);
//void Timer_Update(uint8_t);
void stop_timer_for_task(uint8_t id);
void set_timer_for_task(uint8_t id, uint8_t task_id, uint64_t period, void (*timer_isr)() );

//interrupt handlers/functions
void timer0_isr(void);
void timer1_isr(void);
void timer2_isr(void);
void timer3_isr(void);
void timer4_isr(void);
void timer5_isr(void);

//data splitting functions
void data_split_32(int32_t data, uint8_t *split);
void data_split_u32(uint32_t data, uint8_t *split);
void data_split_16(int16_t data, uint8_t  *split);
void data_split_u16(uint16_t data, uint8_t  *split);

//FRAM packet creation function
uint8_t place_data_in_packet(uint8_t position, uint8_t *data_bytes, uint8_t *data_packet);

//----------------------------------VARIABLES---------------------------------------------

//structures to organise modes, tasks and timers -> definition in header
opmode_t modes[8];
timer_properties_t Timer_Properties;

// [ RUN-TIME ]
// Total number of tasks that can be run concurrently (limited by number of avail. timers)
#define MAX_TASKS 6

volatile int8_t task_q[MAX_TASKS];  // Task queue directly accessed by interrupts
Node*           task_pq;            // Task priority queue constructed from previous q indirectly
volatile uint8_t no_of_tasks_queued;

// Properties
task_t* current_tasks;              //current task structure
uint8_t current_mode;               //for deciding which enums to use -> enums declaration in header
uint8_t previous_mode;              //for returning to previous mode from certain modes such as PT

//specific for FBU
bool FBU_exit_switch = false;

//specific for ADM 
uint32_t ADM_status;
char ADM_char;
uint32_t deploy_attempts;

//specific for Camera
bool image_trigger;
uint32_t picture_wait_period =1;

// Error detection
uint8_t flash_errors;
uint8_t ram_errors;

// State variables
uint16_t data_packets_pending;
uint8_t rx_noisefloor;

//------------------------------TRANSMITTER SETTINGS------------------------------------------------
radio_config_t radio_config;

static double freq = 145.5;
static double pwr = 10.0;

//------------------------------TIMERS VARIABLES AND FUNCTIONS---------------------------------------
//can be moved (except variables) to separate source file, but lot of them are operating on the this file variables

uint32_t ulPeriod;                  //stores the clock frequency of MCU, therefore represents the count value required for 1Hz timeout
int8_t timer_to_task[MAX_TASKS];    //for mapping timers to tasks: timer_id -> task_id (reset look-up table); 

//list of all the timers used for scheduling tasks
//"SYSCTL_PERIPH_TIMERn" - needed for enabling timers operations -> driverlib/sysctl.h and sysctl.c
uint32_t ALL_TIMER_PERIPHERALS[6] =
  { SYSCTL_PERIPH_WTIMER0, SYSCTL_PERIPH_WTIMER1, SYSCTL_PERIPH_WTIMER2,
    SYSCTL_PERIPH_WTIMER3, SYSCTL_PERIPH_WTIMER4, SYSCTL_PERIPH_WTIMER5 };
//Timer bases needed to manipulate timers - turn ON/OFF, load value etc.
uint32_t ALL_TIMER_BASES[6] =
  {WTIMER0_BASE, WTIMER1_BASE, WTIMER2_BASE, WTIMER3_BASE, WTIMER4_BASE, WTIMER5_BASE};

//Definitions of the interrupt handlers/funtions for the scheduling timers
void timer0_isr(){
	TimerIntClear(WTIMER0_BASE, TIMER_TIMA_TIMEOUT);
  task_q[0] = 0;
  no_of_tasks_queued++;
  }
void timer1_isr(){
	TimerIntClear(WTIMER1_BASE, TIMER_TIMA_TIMEOUT);
  task_q[1] = 1;
  no_of_tasks_queued++;
  }
void timer2_isr(){
	TimerIntClear(WTIMER2_BASE, TIMER_TIMA_TIMEOUT);
  task_q[2] = 2;
  no_of_tasks_queued++;
  }
void timer3_isr(){
	TimerIntClear(WTIMER3_BASE, TIMER_TIMA_TIMEOUT);
  task_q[3] = 3;
  no_of_tasks_queued++;
  }
void timer4_isr(){
	TimerIntClear(WTIMER4_BASE, TIMER_TIMA_TIMEOUT);
  task_q[4] = 4;
  no_of_tasks_queued++;
  }
void timer5_isr(){
	TimerIntClear(WTIMER5_BASE, TIMER_TIMA_TIMEOUT);
  task_q[5] = 5;
  no_of_tasks_queued++;
  }

//table for mapping interrupt funtions for the timers; its initialisation below;
typedef void (*isr_map)(void);
isr_map timer_isr_map[MAX_TASKS] = {&timer0_isr, &timer1_isr, &timer2_isr,
                                    &timer3_isr, &timer4_isr, &timer5_isr};

//timer_isr_map[0] = &timer0_isr;
//timer_isr_map[1] = &timer1_isr;
//timer_isr_map[2] = &timer2_isr;
//timer_isr_map[3] = &timer3_isr;
//timer_isr_map[4] = &timer4_isr;
//timer_isr_map[5] = &timer5_isr;

//funtion for assigning and setting the timer to task
void set_timer_for_task(uint8_t id, uint8_t task_id, uint64_t period, void (*timer_isr)() ){
  //load the timer count register with the value corresponding to period; prime it (-1 is unnecessery but keep it just to avoid sysclock conflict?)
  TimerLoadSet64(ALL_TIMER_BASES[id], period-1);
  //assign the interrupt handler to the timer
  TimerIntRegister(ALL_TIMER_BASES[id], TIMER_A, timer_isr);
  //start the timer
  TimerEnable(ALL_TIMER_BASES[id], TIMER_A);
  //map used timer to assigned task
  timer_to_task[id] = task_id;
}

//function for stopping ONE (specified) timer and freeing it
void stop_timer_for_task(uint8_t id){
  TimerDisable(ALL_TIMER_BASES[id], TIMER_A);     //stop the timer
  timer_to_task[id] = -1;                         //free timer
}

//function for freeing ALL the timers that has been used in mode
int8_t free_timers(){
    for(uint8_t ids = 0; ids <6; ids++){
      stop_timer_for_task(ids);                   //go through all the timers and stop them
    }
    return 0;
}

//function which get the available timer and return its "id"
int8_t get_available_timer(){
  int8_t id = -1;
  for (uint8_t i=0; i<6; i++){
    if (timer_to_task[i] == -1) {
      id = i;                                     //check all timers, break if found one which is  free
      break;
    }
  }
  return id;
}

//Initialise timers
void timer_init(){
  for (uint8_t i=0; i<6; i++) SysCtlPeripheralEnable(ALL_TIMER_PERIPHERALS[i]); //enable all the timers
  for (uint8_t i=0; i<6; i++) {
    while(!SysCtlPeripheralReady(ALL_TIMER_PERIPHERALS[i]));                    //wait for them to be ready
  }
  for (uint8_t i=0; i<6; i++){
    TimerConfigure(ALL_TIMER_BASES[i], TIMER_CFG_PERIODIC); // set it to periodically trigger interrupt
    TimerIntEnable(ALL_TIMER_BASES[i], TIMER_TIMA_TIMEOUT); // enable interrupt
  }
ulPeriod = SysCtlClockGet();                                //get MCU clock rate - 1Hz update rate
}

//for the old solution for the timing, not needed of the wider timers work properly
/*void Timer_Update(uint8_t Timer_id){
    char Interrupt_string[50];
    if(Timer_Properties.Timer_division[Timer_id] > 1){
      if(Timer_Properties.Timer_count[Timer_id] == 0){
        sprintf(Interrupt_string, "**timer%d_interrupt!**\r\n", Timer_id);
        UART_puts(UART_INTERFACE, Interrupt_string);
        // Schedule task
        task_q[Timer_id] = Timer_id;
        TimerDisable(ALL_TIMER_BASES[Timer_id], TIMER_A);
        TimerLoadSet(ALL_TIMER_BASES[Timer_id], TIMER_A, 100*ulPeriod-1); // prime it (-1 is unnecessery but keep it just to avoid sysclock conflict?)
        TimerIntRegister(ALL_TIMER_BASES[Timer_id], TIMER_A, timer_isr_map[Timer_id]); // this is for dynamic interrupt compilation
        TimerEnable(ALL_TIMER_BASES[Timer_id], TIMER_A); 	 // start timer
        Timer_Properties.Timer_count[Timer_id] = Timer_Properties.Initial_Timer_count[Timer_id];
        no_of_tasks_queued++;
        return;
      } 
      if (Timer_Properties.Timer_count[Timer_id] == 1){
        TimerDisable(ALL_TIMER_BASES[Timer_id], TIMER_A);
        TimerLoadSet(ALL_TIMER_BASES[Timer_id], TIMER_A, Timer_Properties.Timer_mod[Timer_id]*ulPeriod-1); // prime it (-1 is unnecessery but keep it just to avoid sysclock conflict?)
        TimerIntRegister(ALL_TIMER_BASES[Timer_id], TIMER_A, timer_isr_map[Timer_id]); // this is for dynamic interrupt compilation
        TimerEnable(ALL_TIMER_BASES[Timer_id], TIMER_A); 	 // start timer
        Timer_Properties.Timer_count[Timer_id]--;
        return;
      }
      if (Timer_Properties.Timer_count[Timer_id] > 1){
        Timer_Properties.Timer_count[Timer_id]--;
        return;
      }
    } 
  else{
  // Schedule task
    sprintf(Interrupt_string, "**timer%d_interrupt!**\r\n", Timer_id);
    UART_puts(UART_INTERFACE, Interrupt_string);
    task_q[Timer_id] = Timer_id;
    no_of_tasks_queued++;
  }
}*/

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//-------------------------------------FUNCTIONS FOR THE MORSE TELEMTRY-----------------------------------------------
static void cw_tone_on(void)
{
  uint8_t pwr_reg;

  radio_reset_config(SPI_RADIO_TX, preferredSettings_cw, sizeof(preferredSettings_cw)/sizeof(registerSetting_t));
  manualCalibration(SPI_RADIO_TX);

  radio_set_freq_f(SPI_RADIO_TX, &freq);

  radio_set_pwr_f(SPI_RADIO_TX, &pwr, &pwr_reg);

  SPI_cmd(SPI_RADIO_TX, CC112X_STX);

  LED_on(LED_B);
}

static void cw_tone_off(void)
{
  radio_reset_config(SPI_RADIO_TX, preferredSettings_cw, sizeof(preferredSettings_cw)/sizeof(registerSetting_t));

  LED_off(LED_B);
}
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//-----------------------------------MISSION INITIALISATION FUNCTION------------------------------------------

void Mission_init(void)
{
  LED_on(LED_B);

  Board_init();
  update_watchdog_timestamp();
  enable_watchdog_kick();
  setup_internal_watchdogs();
  EEPROM_init();
  I2C_init(0);
  UART_init(UART_INTERFACE, 9600);
  GNSS_Init();
  Buffer_init();
  Configuration_Init();
  timer_init();
  //radio_config.frequency 
  
  //Radio_rx_receive(radio_config_t *radio_config, void *process_gs_command);
  
  int16_t temp = Temperature_read_tmp100();
  UART_puts(UART_INTERFACE, "\r\n\n**BOARD & DRIVER INITIALISED**\r\n");
//Initialise the task queue as empty
  for (uint8_t i=0; i<MAX_TASKS; i++){
    task_q[i] = -1;
    timer_to_task[i] = -1;
    }
  no_of_tasks_queued = 0;
  image_trigger = false;

  //PRINTS ULPERIOD - TO REMOVE
  char output3[100];
  sprintf(output3,"ULPERIOD: %" PRIu32 "\r\n", ulPeriod);
	UART_puts(UART_INTERFACE, output3);
  //Delay_ms(5000);
  EEPROM_read(EEPROM_DEPLOY_STATUS, &ADM_status, 4);
  EEPROM_read(EEPROM_ADM_DEPLOY_ATTEMPTS, &deploy_attempts, 4);
  // third will miss sometimes if processor busy
  mode_switch(FBU);
}
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//--------------------------------------MISSION LOOP IMPLEMENTATION----------------------------------

//Main function in this file!
//implements mission loop: sorts tasks by the priority and executes first one
void Mission_loop(void)
{
  // Sort list of pending tasks by their priorities
  for (uint8_t i=0; i<MAX_TASKS; i++){
    if (task_q[i] != -1){
      circ_push(&task_pq, task_q[i], current_tasks[task_q[i]].period); 

      // Remove task from standard queue
      task_q[i] = -1;
      no_of_tasks_queued--;}
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

    UART_puts(UART_INTERFACE, "**executed!**\r\n");
    // reset timer for that tasks------------------------------------!!!!!

    // pop
    if(!circ_isEmpty(&task_pq)) circ_pop(&task_pq);
  }

  // Sleep
  UART_puts(UART_INTERFACE, "sleeping...\r\n");
  LED_toggle(LED_B);
  Delay_ms(1000);   //why delay?
  update_watchdog_timestamp();
}

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//--------------------------QUEUE IMPLEMENTATION AND MODE TRANSITION------------------------------------

//function for queing new task
void queue_task(int8_t task_id, uint64_t task_period){

  //If uncommented tasks fire as soon as mode initialised
  //task_q[no_of_tasks_queued] = task_id;
  //no_of_tasks_queued++;

  int8_t timer_id = get_available_timer();

  char output[100];
  sprintf(output,"set timer: : %+06d\r\n", timer_id);
  UART_puts(UART_INTERFACE, output);
  set_timer_for_task(timer_id, task_id, ulPeriod*current_tasks[task_id].period, timer_isr_map[task_id]);
}

//Function for suspending all the queued tasks
void suspend_all_tasks(){
  while(!circ_isEmpty(&task_pq)){
    circ_pop(&task_pq);
  }
}

//Function for transition between the modes
void mode_switch(uint8_t mode){
  free_timers();
  previous_mode = current_mode;
  Mode_init(mode);
}
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//-------------------------------------MODES DECLARATIONS AND DESCRIPTIONS----------------------------------

void Mode_init(int8_t type){

  //TODO: alter all times to correct times for mission and for test
  Node* task_pq = newNode(0, 0);
  switch(type){
    case FBU:{
      task_t* tasks_FBU = (task_t *) malloc (sizeof(task_t)*2);

      modes[FBU].Mode_startup = &fbu_init;
      modes[FBU].opmode_tasks = tasks_FBU;
      modes[FBU].num_tasks = 2;


      tasks_FBU[SAVE_MORSE_TELEMETRY].period = 300;     //save morse telemetry with 300s period
      tasks_FBU[SAVE_MORSE_TELEMETRY].TickFct = &save_morse_telemetry;

      tasks_FBU[EXIT_FBU].period = 1800;                //30min * 60s = 2700s, wait 30min to enter AD mode
      tasks_FBU[EXIT_FBU].TickFct = &exit_fbu;

      current_mode = FBU;
      current_tasks = tasks_FBU;
    
    } break;
    case AD:{
      task_t* tasks_AD = (task_t *) malloc (sizeof(task_t)*4);

      modes[AD].Mode_startup = &ad_init;
      modes[AD].opmode_tasks = tasks_AD;
      modes[AD].num_tasks = 3;

      //must have shorter period than transmit morse to give higher priority
      tasks_AD[AD_SAVE_MORSE_TELEMETRY].period = 30;
      tasks_AD[AD_SAVE_MORSE_TELEMETRY].TickFct = &save_morse_telemetry;

      tasks_AD[TRANSMIT_MORSE_TELEMETRY].period = 60;
      tasks_AD[TRANSMIT_MORSE_TELEMETRY].TickFct = &transmit_morse_telemetry;

      tasks_AD[ANTENNA_DEPLOY_ATTEMPT].period = 70;
      tasks_AD[ANTENNA_DEPLOY_ATTEMPT].TickFct = &ad_deploy_attempt;


      current_mode = AD;
      current_tasks = tasks_AD;
    
    }break;
    case NF:{
      //Difference between data downlink mode and transitting telemetry? Is telemetry just for camera data?
      task_t* tasks_NF = (task_t *) malloc (sizeof(task_t)*6);

      modes[NF].Mode_startup = &nf_init;
      modes[NF].opmode_tasks = tasks_NF;
      modes[NF].num_tasks = 5;  //Don't want image to be captured until after a command is sent


      tasks_NF[NF_SAVE_EPS_HEALTH].period =30; 
      tasks_NF[NF_SAVE_EPS_HEALTH].TickFct = &save_eps_health_data;

      tasks_NF[NF_SAVE_GPS_POS].period = spacecraft_configuration.data.gps_acquisition_interval;
      tasks_NF[NF_SAVE_GPS_POS].TickFct = &save_gps_data;

			tasks_NF[NF_SAVE_ATTITUDE].period = spacecraft_configuration.data.imu_acquisition_interval;
			tasks_NF[NF_SAVE_ATTITUDE].TickFct = &save_attitude;

      tasks_NF[NF_TRANSMIT_TELEMETRY].period = spacecraft_configuration.data.tx_interval;
      tasks_NF[NF_TRANSMIT_TELEMETRY].TickFct = &transmit_next_telemetry;

			tasks_NF[NF_CHECK_HEALTH].period = spacecraft_configuration.data.health_acquisition_interval;
			tasks_NF[NF_CHECK_HEALTH].TickFct = &check_health_status;

      tasks_NF[NF_TAKE_PICTURE].period = spacecraft_configuration.data.image_acquisition_time;
			tasks_NF[NF_TAKE_PICTURE].TickFct = &take_picture; 
      

      current_mode = NF;
      current_tasks = tasks_NF;


      if(image_trigger == true){
        //queue_task(NF_TAKE_PICTURE);
      } 
      
    } break;
    case LP:{
      //CAN'T COMMAND OUT OF THIS MODE SO NO EXIT FUNCTION NEEDED, JUST USES CHECK HEALTH
      task_t* tasks_LP = (task_t *) malloc (sizeof(task_t)*3);

      modes[LP].Mode_startup = &lp_init;
      modes[LP].opmode_tasks = tasks_LP;
      modes[LP].num_tasks = 3;


      tasks_LP[LP_SAVE_EPS_HEALTH].period =30; //300
      tasks_LP[LP_SAVE_EPS_HEALTH].TickFct = &save_eps_health_data;

      tasks_LP[LP_CHECK_HEALTH].period = spacecraft_configuration.data.health_acquisition_interval; //300
      tasks_LP[LP_CHECK_HEALTH].TickFct = &check_health_status;

			tasks_LP[LP_TRANSMIT_TELEMETRY].period = spacecraft_configuration.data.tx_interval;
			tasks_LP[LP_TRANSMIT_TELEMETRY].TickFct = &transmit_next_telemetry;

      current_mode = LP;
      current_tasks = tasks_LP; 

    }break;
    case SM:{
      task_t* tasks_SM = (task_t *) malloc (sizeof(task_t)*4);

      modes[SM].Mode_startup = &sm_init;
      modes[SM].opmode_tasks = tasks_SM;
      modes[SM].num_tasks = 4;

      tasks_SM[SM_SAVE_EPS_HEALTH].period =30; //300
      tasks_SM[SM_SAVE_EPS_HEALTH].TickFct = &save_eps_health_data;

      tasks_SM[SM_CHECK_HEALTH].period = 30; //300
      tasks_SM[SM_CHECK_HEALTH].TickFct = check_health_status;

			tasks_SM[SM_TRANSMIT_TELEMETRY].period = 60;
			tasks_SM[SM_TRANSMIT_TELEMETRY].TickFct = &transmit_next_telemetry;

      tasks_SM[REBOOT_CHECK].period = 5;
			tasks_SM[REBOOT_CHECK].TickFct = &sm_reboot;  //this funtion is just the print statement, should there be anything more?


      current_mode = SM;
      current_tasks = tasks_SM; 

    }break;
  case CFU:{
      task_t* tasks_CFU = (task_t *) malloc (sizeof(task_t));

      modes[CFU].Mode_startup = &cfu_init;
      modes[CFU].opmode_tasks = tasks_CFU;
      modes[CFU].num_tasks = 1;

      //NO ACTUAL RECURRING TASKS, CAN THE TASKS JUST BE LEFT AS EMPTY?
      current_mode = CFU;
      current_tasks = tasks_CFU; 

    }break;
    case PT:{
      task_t* tasks_PT = (task_t *) malloc (sizeof(task_t));

      modes[PT].Mode_startup = &pt_init;
      modes[PT].opmode_tasks = tasks_PT;
      modes[PT].num_tasks = 1;


      //NO ACTUAL RECURRING TASKS, CAN THE TASKS JUST BE LEFT AS EMPTY?
      //ALL TIMING OCCURS IN NF MODE
      current_mode = PT;
      current_tasks = tasks_PT; 

    }break;
    case DL:{
      task_t* tasks_DL = (task_t *) malloc (sizeof(task_t)*6);

      modes[DL].Mode_startup = &dl_init;
      modes[DL].opmode_tasks = tasks_DL;
      modes[DL].num_tasks = 6;

      tasks_DL[DL_SAVE_EPS_HEALTH].period =20; //300
      tasks_DL[DL_SAVE_EPS_HEALTH].TickFct = &save_eps_health_data;

      tasks_DL[DL_SAVE_GPS_POSITION].period = spacecraft_configuration.data.gps_acquisition_interval; //300
      tasks_DL[DL_SAVE_GPS_POSITION].TickFct = save_gps_data;

			tasks_DL[DL_SAVE_ATTITUDE].period = spacecraft_configuration.data.imu_acquisition_interval;
			tasks_DL[DL_SAVE_ATTITUDE].TickFct = &save_attitude;

      tasks_DL[DL_TAKE_PICTURE].period = spacecraft_configuration.data.image_acquisition_time;
			tasks_DL[DL_TAKE_PICTURE].TickFct = &take_picture;

      tasks_DL[DL_POLL_TRANSMITTER].period = 1; //Should be 1ms but 1s should be sufficient?
			tasks_DL[DL_POLL_TRANSMITTER].TickFct = &poll_transmitter; //To get 1ms a lot has to be changed in the code

      tasks_DL[DL_CHECK_HEALTH].period = 5;
			tasks_DL[DL_CHECK_HEALTH].TickFct = &check_health_status;

      current_mode = DL;
      current_tasks = tasks_DL; 

    }break;
  }

  if(modes[current_mode].Mode_startup() == false){return;}

  if(modes[type].num_tasks != 1){
    for(int task_counter = 0 ;task_counter < modes[type].num_tasks; task_counter++){
      Timer_Properties.Timer_period[task_counter] = modes[type].opmode_tasks[task_counter].period;
      queue_task(task_counter, modes[type].opmode_tasks[task_counter].period);
      }
    }
}
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//-----------------------------------FUNCTIONS FOR SYSTEM ACTIONS------------------------------
//like taking measurements, checking health, making picture etc.

void Mission_SEU(void)
{
	/* Uncomment when working EPS board available
    Turns off mcu power rail and the eps logic turns it back on automatically 
    as the eps mcu will continually check to see if the tobc mcu is on (not yet implemented)*/
  //EPS_setPowerRail(EPS_PWR_MCU, 0);

}

uint16_t perform_subsystem_check(){
	uint16_t status = 0;

	for (int i=0; i<14; i++)
		status |= 0; // insert system check here

	return status;
}

int8_t save_eps_health_data(int8_t t){
  //Uncomment code when EPS board being used
  /*/data packet for FRAM
  uint8_t data_packet_for_fram[BUFFER_SLOT_SIZE/8];
  uint8_t data_count = 0;
  uint32_t time_of_data_acquisition;
  uint8_t i; // loop counter
	uint32_t eeprom_read;
  uint8_t data_byte32[4];
  uint8_t data_byte16[2];

  // TODO: Generate dataset id

  // TODO: Get timestamp
  RTC_getTime(&time_of_data_acquisition);
  data_split_u32(time_of_data_acquisition, data_byte32);
  data_count = place_data_in_packet(data_count, data_byte32, data_packet_for_fram);
  //Get TOBC values
  //obc_temperature - read, split and place in the packet
  data_split_16(get_temp(TEMP_OBC), &data_byte16);
  data_count = place_data_in_packet(data_count, data_byte16, data_packet_for_fram);
  //rx_temperature - read, split and place in the packet
  data_split_16(get_temp(TEMP_RX), &data_byte16);
  data_count = place_data_in_packet(data_count, data_byte16, data_packet_for_fram);
  //tx_temperature...
  data_split_16(get_temp(TEMP_TX), &data_byte16);
  data_count = place_data_in_packet(data_count, data_byte16, data_packet_for_fram);
``//pa_temperature...
  data_split_16(get_temp(TEMP_PA), &data_byte16);
  data_count = place_data_in_packet(data_count, data_byte16, data_packet_for_fram);

  //read EEPROM for reboot_count
	EEPROM_read(EEPROM_REBOOT_COUNT, &eeprom_read, 1);
	data_count = place_data_in_packet(data_count, (uint8_t)eeprom_read, data_packet_for_fram);
  
  // [CLARIFY] data_packets_pending
  data_split_u16(data_packets_pending, &data_byte16);
	data_count = place_data_in_packet(data_count, data_byte16, data_packet_for_fram);

  // [CLARIFY] rx_noisefloor
	data_count = place_data_in_packet(data_count, rx_noisefloor, data_packet_for_fram);
  
	// Flash and RAM errors in same byte
  data_count = place_data_in_packet(data_count, (flash_errors << 4) + ram_errors, data_packet_for_fram);
  //Buffer_store_new_data( (flash_errors << 4) + ram_errors);

  // Get 45 EPS values (16-bit fields)
  for (i=0; i<45; i++){
    uint16_t eps_field;
    EPS_getBatteryInfo(&eps_field, i);
    data_split_u16(eps_field, &data_byte16);
    data_count = place_data_in_packet(data_count, data_byte16, data_packet_for_fram);
  }
	uint16_t subsystem_status = perform_subsystem_check();
	// read antenna switch state (and ask where this can be attached)
	subsystem_status |= (uint8_t)Antenna_read_deployment_switch() << 14;

  data_split_u16(subsystem_status, &data_byte16);
	data_count = place_data_in_packet(data_count, data_byte16, data_packet_for_fram);

  uint16_t batt_volt = 0;
  if(current_mode != SM){
      if(EPS_getBatteryInfo(&batt_volt,  EPS_REG_BAT_V/*EPS_REG_SW_ON*//*) > BATTERY_THRESHOLD){
      mode_switch(LP);
    }
  }
  if(current_mode == LP){
    if(EPS_getBatteryInfo(&batt_volt,  EPS_REG_BAT_V/*EPS_REG_SW_ON*//*) > BATTERY_THRESHOLD){
      mode_switch(NF);
    }
  }

  UART_puts(UART_INTERFACE, "[TASK] Finished saving EPS health data.\r\n");
*/
  return 0;
}

int8_t save_gps_data(int8_t t){
  //Driver needs to be updated
  UART_puts(UART_INTERFACE, "[TASK] Saving GPS payload data...\r\n");
  // TODO: Generate dataset id  - ID is an index from FRAM! need to add it to the data packet in downlink
	// Take multiple samples
    uint8_t data_packet_for_fram[BUFFER_SLOT_SIZE/8]; //for storing data packet
    uint8_t data_count = 0;                           //secondary variable for arranging the data in the packet
    uint16_t week_num;                                //GPS week number
    //varabiables for the readings and measurements
  	int32_t longitude;
    int32_t latitude;
    int32_t altitude;
    uint8_t long_sd;
    uint8_t lat_sd;
    uint8_t alt_sd;
    uint32_t week_seconds;
    uint32_t time_of_data_acquisition;
    uint8_t data_byte32[4];                            //temporary variables for splitting operations
    uint8_t data_byte16[2];                            //temporary variables for splitting operations
    uint8_t data_byte8[3];                             //temporary variable for saving 8bit datas

  RTC_getTime(&time_of_data_acquisition);              //get the time of measurement, just before GPS function
  //placing time of data acquisition in the packet
  data_split_u32(time_of_data_acquisition, data_byte32);                                //split the time data
  data_count = place_data_in_packet(data_count, data_byte32, data_packet_for_fram);  //place in the packet

  //getting gnss data and placing it in the packet
  //better to place immediately in the packet to save memory
	for (uint8_t i=0; i<spacecraft_configuration.data.gps_sample_count; i++){
    //get GNSS data
    GNSS_getData(&longitude, &latitude, &altitude, &long_sd, &lat_sd, &alt_sd, &week_num, &week_seconds);
    //GPS week number - save it only once in the beginning
    if(i == 0){
      data_split_u16(week_num, data_byte16);                                              //split week no. data
      data_count = place_data_in_packet(data_count, data_byte16, data_packet_for_fram);//place in the packet
    }
    //gps time - time since beginning of the current week from GPS
    data_split_u32(week_seconds, data_byte32);                                            //split GPS_time sample
    data_count = place_data_in_packet(data_count, data_byte32, data_packet_for_fram);  //place sample in the packet
    //lattitude data
    data_split_32(latitude, data_byte32);                                                 //split latitude sample
    data_count = place_data_in_packet(data_count, data_byte32, data_packet_for_fram);  //place sample in the packet
    //longitude data
    data_split_32(longitude, data_byte32);                                                //split longitude sample
    data_count = place_data_in_packet(data_count, data_byte32, data_packet_for_fram);  //place in the packet
    //altitude data
    data_split_32(altitude, data_byte32);                                                 //split altitude sample
    data_count = place_data_in_packet(data_count, data_byte32, data_packet_for_fram);  //place sample in the packet
    //lat_sd data
    data_count = place_data_in_packet(data_count, lat_sd, data_packet_for_fram);
    //long_sd data
    data_count = place_data_in_packet(data_count, long_sd, data_packet_for_fram);
    //alt_sd data
    data_count = place_data_in_packet(data_count, alt_sd, data_packet_for_fram);

    Delay_ms(spacecraft_configuration.data.gps_sample_interval*1000);//delay for the GPS to be ready for next measurement
	}

  Buffer_store_new_data(data_packet_for_fram);  //store the packet in FRAM
  UART_puts(UART_INTERFACE, "[TASK] GPS payload data saved.\r\n");
  return 0;
}

int8_t send_morse_telemetry (int8_t t){
  //TODO: test and change string to fully match TMTC specification
  //char morse_string[30];

  //UART_puts(UART_INTERFACE, "[FBU][TASK] Sending morse telemetry...\r\n");
  
  //char batt_str[20];
  //uint16_t batt_volt = 5;
  //**UNCOMMENT WHEN YOU HAVE ACCESS TO WORKING EPS BOARD
  //EPS_getBatteryInfo(&batt_volt,  EPS_REG_BAT_V/*EPS_REG_SW_ON*/); //May need to change to 4, given in eps header
  //if(Antenna_read_deployment_switch()){
    //ADM_status = 'Y';
  //}
  //sprintf(batt_str, "%" PRId16, batt_volt);

  //sprintf(morse_string, "U O S 3   %" PRId16 " V   %c   %" PRId16 "  K\0", batt_volt, ADM_status, deploy_attempts);
  //Packet_cw_transmit_buffer(morse_string, strlen(morse_string), cw_tone_on, cw_tone_off); ***UNCOMMENT WHEN TX IS WORKING
  UART_puts(UART_INTERFACE, "[TASK] Morse telemetry sent\r\n");
  return 0;

}

int8_t exit_fbu(int8_t t){
  UART_puts(UART_INTERFACE, "[TASK] Wait Period Finished... Entering ADM Mode.\r\n");
  mode_switch(AD);
  return 0;
}

int8_t ad_deploy_attempt(int8_t t){
  //ATTEMPT TO DEPLOY ANTENNA
  uint16_t batt_volt = BATTERY_VOLTAGE;
  UART_puts(UART_INTERFACE, "[TASK] Attempting To Deploy Antenna...\r\n");
  //EPS_getBatteryInfo(&batt_volt,  EPS_REG_BAT_V/*EPS_REG_SW_ON*/); //May need to change to 4, given in eps header)
  if(batt_volt >= BATTERY_THRESHOLD){
    LED_on(LED_B);  //debugging only
    Antenna_deploy();
    LED_off(LED_B); //debugging only
    ADM_status = 1;
    EEPROM_write(EEPROM_DEPLOY_STATUS, &ADM_status, 4);
    deploy_attempts++;
    EEPROM_write(EEPROM_ADM_DEPLOY_ATTEMPTS, &deploy_attempts, 4);
    mode_switch(NF);  //This is temporary, when radio driver is complete you only exit adm via telecommand (to LP or NF depending on batt status)
    UART_puts(UART_INTERFACE, "[TASK] Antenna Deployed, Switched to NF Mode.\r\n");
  }
  else{
    UART_puts(UART_INTERFACE, "[TASK] Insufficient Battery Voltage, Waiting.\r\n");
    deploy_attempts++;
    EEPROM_write(EEPROM_ADM_DEPLOY_ATTEMPTS, &deploy_attempts, 4);

  }
  return 0;
}

int8_t save_morse_telemetry(int8_t t){
  //SAVE MORSE TELEMETRY
  UART_puts(UART_INTERFACE, "[TASK] Saving Morse Telemetry...\r\n");
  char morse_string[30];
  
  //char batt_str[20];
  uint16_t batt_volt = 5;
  //**UNCOMMENT WHEN YOU HAVE ACCESS TO WORKING EPS BOARD
  //EPS_getBatteryInfo(&batt_volt,  EPS_REG_BAT_V/*EPS_REG_SW_ON*/); //May need to change to 4, given in eps header
  if(ADM_status == 1){
    ADM_char = 'Y';
  }
  else{
    ADM_char = 'N';
  }
  //sprintf(batt_str, "%" PRId16, batt_volt);

  sprintf(morse_string, "U O S 3   %" PRId16 " V   %c   %" PRId32 "  K\0", batt_volt, ADM_char, deploy_attempts);
  Buffer_store_new_data(&morse_string);
  UART_puts(UART_INTERFACE, "[TASK] Morse Telemetry Saved.\r\n");
  return 0;
}

int8_t transmit_morse_telemetry(int8_t t){
  //TRANSMIT MORSE TELEMETRY
  UART_puts(UART_INTERFACE, "[TASK] Transmit morse telemetry\r\n");
  return 0;
}

int8_t exit_ad(int8_t t){
  //EXIT AD MODE
  UART_puts(UART_INTERFACE, "[AD][TASK] Exit AD mode\r\n");
  mode_switch(NF);
  return 0;
}

int8_t sm_reboot(int8_t t){
  UART_puts(UART_INTERFACE, "[SM][TASK?] No contact from ground, rebooting...\r\n");
  return 0;
}

int8_t save_attitude(int8_t t){
  UART_puts(UART_INTERFACE, "[TASK] Saving IMU payload data...\r\n");
  //variables for creating the FRAM data packet
  // TODO: Generate dataset id - dataset ID is the index fo the slot from FRAM memory; need to be read in the downlinking mode
  uint8_t data_packet_for_fram[BUFFER_SLOT_SIZE/8];   //for storing data packet
  uint8_t data_count = 0;                             //secondary variable for arranging the data in the packet
  //variables for the readings and measurements
  uint32_t time_of_data_acquisition;
  int16_t mems_temp;                                  //temperature of mems. sensor
  int16_t mag_x;
  int16_t mag_y;
  int16_t mag_z;
  int16_t gyro_x;
  int16_t gyro_y;
  int16_t gyro_z;
  //variables for splitting the data
  uint8_t data_byte32[4];
  uint8_t data_byte16[2];
  
  RTC_getTime(&time_of_data_acquisition);                                             //get time of acquisition
  data_split_u32(time_of_data_acquisition, data_byte32);                              //split it
  data_count = place_data_in_packet(data_count, data_byte32, data_packet_for_fram);//place in the FRAM packet
	// Get IMU temperature based on Ed's driver
  IMU_read_temp(&mems_temp);
  data_split_16(mems_temp, &data_byte16);                                              //split it
  data_count = place_data_in_packet(data_count, data_byte16, data_packet_for_fram);//place in the FRAM packet
	
  // Take multiple samples
	for (uint8_t i=0; i<spacecraft_configuration.data.imu_sample_count; i++){
		IMU_read_accel(mag_x, mag_y, mag_z);          //read mag. measurements
		IMU_read_gyro(gyro_x, gyro_y, gyro_z);        //read gyro measurements

    //placing measurements in the packet
    //mag_x measurement - splitting and placing in the FRAM packet 
    data_split_16(mag_x, data_byte16);
    data_count = place_data_in_packet(data_count, data_byte16, data_packet_for_fram);
    //mag_y measurement - splitting and placing in the FRAM packet
    data_split_16(mag_y, data_byte16);
    data_count = place_data_in_packet(data_count, data_byte16, data_packet_for_fram);
    //mag_z measurement - ...
    data_split_16(mag_z, data_byte16);
    data_count = place_data_in_packet(data_count, data_byte16, data_packet_for_fram);
    //gyro_x measurement - ...
    data_split_16(gyro_x, data_byte16);
    data_count = place_data_in_packet(data_count, data_byte16, data_packet_for_fram);
    //gyro_y measurement - ...
    data_split_16(gyro_y, data_byte16);
    data_count = place_data_in_packet(data_count, data_byte16, data_packet_for_fram);
    //gyro_z measurement - ...
    data_split_16(gyro_z, data_byte16);
    data_count = place_data_in_packet(data_count, data_byte16, data_packet_for_fram);
    
		Delay_ms(spacecraft_configuration.data.imu_sample_interval*1000); // TODO: update time once it has been determined
	}
  
  Buffer_store_new_data(data_packet_for_fram);
  UART_puts(UART_INTERFACE, "[TASK] Finished saving IMU payload data.\r\n");
}

int8_t save_image_data(void){
  UART_puts(UART_INTERFACE, "[TASK] Saving Image data...\r\n");
  //switch to new driver function that stores data straight into buffer
  //Camera_capture(5000, image_length_add); 
  
  return 0;
}

void update_radio_parameters(){
	// TODO: read these from the config
	// radio_set_freq_f
	// radio_set_pwr_f

	// TODO: investigate whether any of the following are neccessary
	// -set sym_dev
	// -set packets length
	// -get packet wait
	// -manual calibration
}
/*THIS FUNCTION TRANSMITS THE CURRENT IMU AND GPS DATA IN A BEACON, DL MODE DOWNLINKS ALL DATA STORED IN THE BUFFER WHEN
OVER THE UOS3 GROUNDSTATION*/

int8_t transmit_next_telemetry(int8_t t){
  uint8_t data_packet;

  //TODO: only cycle through 'most recent data':
  while(Buffer_get_next_data(&data_packet) != 1){

		// TODO: add Matt's weird error detection & check code

		// Set-up radio
		EPS_setPowerRail(EPS_PWR_TX, 1);
		radio_reset_config(SPI_RADIO_TX, preferredSettings_cw, sizeof(preferredSettings_cw)/sizeof(registerSetting_t));
		update_radio_parameters();

		// Transfer buffer to TX transceiver buffer
		cc112xSpiWriteTxFifo(SPI_RADIO_TX, &data_packet, FRAM_PAYLOAD_SIZE);

  }
		// Transmit
		SPI_cmd(SPI_RADIO_TX, CC112X_STX);

		// wait for the packet to send
		while( cc1125_pollGPIO(GPIO0_RADIO_TX)) {} ;

		EPS_setPowerRail(EPS_PWR_TX, 0);
    
	  UART_puts(UART_INTERFACE, "[TASK] Finished transmit next telemetry\r\n");
		return 0;
}

int8_t check_health_status(int8_t t){

	// load allowed voltage of battery

	// Get current voltage of battery from EPS board

	// Calculate checksums for all payload

  UART_puts(UART_INTERFACE, "[TASK] Finished checking health\r\n");
  return 0;
}

int8_t poll_transmitter(int8_t t){
  UART_puts(UART_INTERFACE, "[TASK] Finished polling transmitter\r\n");
  return 0;
}

int8_t take_picture(int8_t t){
  if(image_trigger == true){
    UART_puts(UART_INTERFACE, "[TASK] Taking picture\r\n");
    mode_switch(PT);
    image_trigger = false;
  }
  return 0;
}

int8_t process_gs_command(int8_t t){
  //Reciever interupt has not been finsished so not completely sure what the parameters of this function will look like
	switch(t){
		case IMAGE_REQUEST:
    image_trigger = true;
    if (current_mode == NF){
      Timer_Properties.Timer_period[5] = modes[NF].opmode_tasks[5].period;
      queue_task(5, modes[NF].opmode_tasks[5].period);
    }
    if (current_mode == DL){
      Timer_Properties.Timer_period[5] = modes[DL].opmode_tasks[5].period;
      queue_task(5, modes[DL].opmode_tasks[5].period);
    }
    UART_puts(UART_INTERFACE, "[TASK] Image Scheduled.\r\n");
		break;
		case ACK_REC_PACKETS:
		// update entries for packets in meta-table stored in FRAM
		break;
		case DL_REQUEST:
    //Dowlinking timing handled inside DL mode, basically an augmented version of NF mode
      mode_switch(DL);
		break;
		case UPDATE_CONFIG:
      mode_switch(CFU);
      UART_puts(UART_INTERFACE, "[TASK] Switched to config update mode.\r\n");
		//generate config bytes and save to EEPROM
		// in two separate locations
		break;
		case ENTER_SAFE_MODE:
      mode_switch(SM);
      UART_puts(UART_INTERFACE, "[TASK] Switched to safe mode.\r\n");
		break;
		case MANUAL_OVERRIDE:
		//reboot or switch-off subsystem
		break;
	}
  UART_puts(UART_INTERFACE, "[TASK] Finished processing gs command.\r\n");
}

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//-------------------------------------MODE INITIALISATION FUNCTIONS------------------------------

bool fbu_init(){
  uint16_t batt_volt = BATTERY_VOLTAGE;
  UART_puts(UART_INTERFACE, "[TASK] FBU INITIALISED\r\n");
  char adm_stat[30];
  sprintf(adm_stat, "ADM status: %d\r\n", ADM_status);
  UART_puts(UART_INTERFACE, adm_stat);
  //Exit fbu immediately if antenna is not already deployed, reset flag with reset_adm_flag.c
  if(ADM_status == 1){
    //EPS_getBatteryInfo(&batt_volt,  EPS_REG_BAT_V/*EPS_REG_SW_ON*/); //May need to change to 4, given in eps header)
    if(batt_volt>BATTERY_THRESHOLD){
      mode_switch(NF);
      return false;
    }
    else{
      mode_switch(LP);
      return false;
    }
  }
  else{
    return true;
  }
}

bool ad_init(){
  //Not (yet?) needed
  UART_puts(UART_INTERFACE, "[TASK] AD INITIALISED\r\n");
  return true;
}

bool nf_init(){
  //Not (yet?) needed
  UART_puts(UART_INTERFACE, "[TASK] NF INITIALISED\r\n");
  return true;
}

bool lp_init(){

  //Suspend all activities in queue from previous mode
  suspend_all_tasks();

  //UNCOMMENT WHEN WORKING EPS BOARD AVAILABLE
  /*EPS_setPowerRail(EPS_PWR_CAM, 0);
  EPS_setPowerRail(EPS_PWR_GPS, 0);
  EPS_setPowerRail(EPS_PWR_GPSLNA, 0);*/

  UART_puts(UART_INTERFACE, "[TASK] LP INITIALISED\r\n");
  return true;
}

bool sm_init(){

  //Suspend all activities in queue from previous mode
  suspend_all_tasks();

  //UNCOMMENT WHEN WORKING EPS BOARD AVAILABLE
  /*EPS_setPowerRail(EPS_PWR_CAM, 0);
  EPS_setPowerRail(EPS_PWR_GPS, 0);
  EPS_setPowerRail(EPS_PWR_GPSLNA, 0);*/

  UART_puts(UART_INTERFACE, "[Task] SM INITIALISED\r\n");
  return true;
}

bool cfu_init(){
  //suspend data gathering, picture taking and downlinking
  suspend_all_tasks();
  //Open recieved file and verify values (assuming the data has already been stored
  //in the spacecraft_configuration struct from the data uplink)

  //Configuration_save_to_eeprom();
  
  //Overwrite config file

  //Return beacon with configuration state

  //Return to previous mode
  mode_switch(NF);
  return true;

}

bool pt_init(){

  //Take picture and write to FRAM buffer

  save_image_data();  //need to be implemented  

  //Return to previous mode
  if (previous_mode == DL){
    mode_switch(DL);
  }
  else{
    mode_switch(NF);
  }
  return true;
}

bool dl_init(){
  //Transfer packets to cc1125 buffer
  return true;
}

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//-------------------------------------DATA SPLITTING FUNCTIONS--------------------------------
//for storing larger data in the FRAM (FRAM data is defined as uint8_t)

void data_split_32(int32_t data, uint8_t *split){
  split[0] = ((uint32_t) data & 0x000000ff);
  split[1] = ((uint32_t) data & 0x0000ff00) >> 8;
  split[2] = ((uint32_t) data & 0x00ff0000) >> 16;
  split[3] = ((uint32_t) data & 0xff000000) >> 24;
}

void data_split_u32(uint32_t data, uint8_t *split){
  split[0] = (data & 0x000000ff);
  split[1] = (data & 0x0000ff00) >> 8;
  split[2] = (data & 0x00ff0000) >> 16;
  split[3] = (data & 0xff000000) >> 24;
}

void data_split_u16(uint16_t data, uint8_t  *split){
  split[0] = (data & 0x000000ff);
  split[1] = (data & 0x0000ff00) >> 8;
}

void data_split_16(int16_t data, uint8_t  *split){
  split[0] = ((uint16_t) data & 0x000000ff);
  split[1] = ((uint16_t) data & 0x0000ff00) >> 8;
}
//--------------------------------------FUNCTION FOR PLACING SEPARATE DATA IN ONE FRAM PACKET
uint8_t place_data_in_packet(/*int size,*/uint8_t position, uint8_t *data_bytes, uint8_t *data_packet){
  int size = sizeof(data_bytes);  //can i replace with this first argument?
  for(int i=0; i<size; i++){
    data_packet[position] = data_bytes[i];
    position++;
  }
  return position;
}

// Utility

/**
 * @}
 */

