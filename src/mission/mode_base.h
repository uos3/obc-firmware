#ifndef MODE_BASE_H
#define MODE_BASE_H

#include <stdint.h>
#define MAX_TASKS 7
#define max_queue_length 10

typedef struct task_t{
	// the function that performs the task
	void (*function)(void);
	// the priority of the task relative to other tasks in mode (lower number is higher)
	uint8_t priority;
	// how often the timer that adds this task should run for
	uint64_t period_s;
}task_t;

typedef struct mode_t{
	// list of repeatable tasks
	task_t available_tasks[MAX_TASKS];
	// number of tasks in list
	uint8_t number_of_tasks;
	// the init function executed when starting the mode
	void (*init_fn)(void);
}mode_t;


void mode_switch_to_mode(mode_t* new_mode);

void mode_sort_task_queue();

task_t mode_pop_top_task();

int get_no_tasks_in_queue();


#endif