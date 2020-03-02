#include <string.h>

#include "../driver/uart.h"
#include "../driver/rtc.h"
#include "../driver/delay.h"

#include "../component/antenna.h"

#include "../utility/debug.h"

#define BURN_TIME 7500-312
#define COOLDOWN 5*60*1000
// #define COOLDOWN 10

#define output_size 80

char do_test_command[] = "burn";
char finish_command[] = "done";
char do_n_command[] = "do ";

void demo_adm(){
	uint64_t burn_start_time;
	uint64_t burn_stop_time;
	double burn_elapsed_time;

	debug_printf("burning now...");
	debug_flash(3);
	RTC_getTime_ms(&burn_start_time);
	Antenna_deploy(BURN_TIME);
	RTC_getTime_ms(&burn_stop_time);
	debug_flash(3);
	burn_elapsed_time = ((double) burn_stop_time - (double) burn_start_time)/1000.0;
	debug_printf("burn lasted for %4.3f", burn_elapsed_time);
}

int main(){
	int test_num = 1;
	int n_times = 0;
	int n_characters = 0;
	char output[output_size];
	debug_init();
	RTC_init();
	debug_printf("=== ADM repeat test ===");

	while (1)
	{
		debug_printf("\r\n--- ADM test number %d ---", test_num);
		debug_get_command(output,output_size);
		if (strcmp(do_test_command, output)==0){

			demo_adm();
			test_num++;
		}
		else if (strcmp(finish_command, output)==0){
			debug_printf("ending test, total tests: %d", test_num);
			break;
		}
		else if (memcmp(output, do_n_command, 3)==0){
			n_times = debug_parse_int(output, 3);
			debug_printf("performing %d tests:", n_times);
			for (int i=0; i<n_times; i++){
				debug_printf("\r\n--- ADM test number %d ---", test_num);
				demo_adm();
				test_num++;
				debug_printf("cooling down for %d ms", COOLDOWN);
				Delay_ms(COOLDOWN);
			}
			// test_num += n_times;
		}
		else{
			debug_printf("invalid option '%s'.\r\navailable commands are: burn, done", output);
		}

	}
	debug_end();
}
