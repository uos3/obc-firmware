#include "../utility/debug.h"
#include "../mission/task_handling.h"
#include "../driver/delay.h"
#include "../driver/timers.h"

void task_0_fn(){
	debug_print("task 0 executing");
}


void task_1_fn(){
	debug_print("task 1 executing");
}


void mode_init_function(){
	debug_print("initialising the mode");
}


int main(){
	debug_init();
	debug_print("=== Task handling demo ===");
	timers_init();
	

	task_t task_0;
	task_0.function = &task_0_fn;
	task_0.priority=1;
	task_0.period_s=2;

	task_t task_1;
	task_1.function = &task_1_fn;
	task_1.priority=2;
	task_1.period_s=2;

	mode_t test_mode;
	test_mode.available_tasks[0] = task_0;
	test_mode.available_tasks[1] = task_1;
	test_mode.number_of_tasks = 3;
	test_mode.init_fn = mode_init_function;

	int task_no_in_queue;
	task_t highest_priority_task;

	debug_marker("switching to test mode");
	task_handling_switch_to_mode(&test_mode);
	debug_done();

	while(1){
		if (get_no_tasks_in_queue()>1){
			debug_marker("sorting tasks in the queue");
			task_handling_sort_task_queue();
			debug_done();
			highest_priority_task = task_handling_pop_next_task();
			highest_priority_task.function();
		}
		Delay_ms(1000);
	}

	debug_print("=== demo end ===");
	debug_end();
}
