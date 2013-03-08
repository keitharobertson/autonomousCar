//Some Helpfully Implemented RT Tasks
#include <stdlib.h>
#include <sched.h>
#include <stdio.h>
#include <sys/mman.h>
#include <string.h>

#include "shirtt.h"

void set_priority(int priority) {
	struct sched_param param;
	 param.sched_priority = priority;
	if(sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
		perror("sched_setscheduler failed");
		exit(-1);
	}
}

void stack_prefault(void) {
	unsigned char dummy[MAX_SAFE_STACK];

	memset(dummy, 0, MAX_SAFE_STACK);
	return;
}

void stack_setup() {
	 /* Lock memory */
	if(mlockall(MCL_CURRENT|MCL_FUTURE) == -1) {
		perror("mlockall failed");
		exit(-2);
	}
	/* Pre-fault our stack */
	stack_prefault();
}

void setup_rt_task(int priority) {
	set_priority(priority);
	stack_setup();
	return;
}
