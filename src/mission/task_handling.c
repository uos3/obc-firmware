#include "../driver/timers.h"
#include "../utility/debug.h"

#include "task_handling.h"

mode_t* current_mode;
task_t task_queue[max_queue_length];
int tasks_in_queue = 0;


void add_task_to_queue(int task_number){
	if (task_number >= current_mode->number_of_tasks){
		// can't add a task that doesn't exist
		return;
	}
	if (tasks_in_queue == max_queue_length){
		// can't add to full queue
		return;
	}
	task_queue[tasks_in_queue] = current_mode->available_tasks[task_number];
	tasks_in_queue++;
	debug_printf("task_handling.c: Added task %d to queue", task_number);
}


void add_task0_isr(){
	add_task_to_queue(0);
}

void add_task1_isr(){
	add_task_to_queue(1);
}

void add_task2_isr(){
	add_task_to_queue(2);
}

void add_task3_isr(){
	add_task_to_queue(3);
}

void add_task4_isr(){
	add_task_to_queue(4);
}

void add_task5_isr(){
	add_task_to_queue(5);
}

void (*add_task_isr_list[])(void) = {
	add_task0_isr,
	add_task1_isr,
	add_task2_isr,
	add_task3_isr,
	add_task4_isr,
	add_task5_isr
};


void task_handling_switch_to_mode(mode_t* new_mode){
	int timer_no;
	uint64_t period;
	void (*function)(void);
	// clean timers from previous mode
	timers_stop_all();
	// re-assign and enable the timers
	current_mode = new_mode;
	for (int task_no = 0; task_no < current_mode->number_of_tasks; task_no++){
		timer_no = task_no;
		period = current_mode->available_tasks[task_no].period_s;
		function = add_task_isr_list[task_no];
		timers_setup_timer(task_no, period, function);
	}
	// run the init function
	current_mode->init_fn();
}

void task_handling_sort_task_queue(){
	task_t tmp;
	// a sort. Not efficient, but works.
	for (int i = 0; i<tasks_in_queue; i++){
		for (int j = 1; j<tasks_in_queue; j++){
			if (task_queue[j].priority < task_queue[j-1].priority){
				tmp = task_queue[j-1];
				task_queue[j-1] = task_queue[j];
				task_queue[j] = tmp;
			}
		}
	}
}

task_t task_handling_pop_next_task(){
	task_t top_task;
	top_task = task_queue[0];
	for (int i = 1; i<tasks_in_queue; i++){
		task_queue[i-1] = task_queue[i];
	}
	tasks_in_queue--;
	return top_task;
}

int get_no_tasks_in_queue(){
	return tasks_in_queue;
}
