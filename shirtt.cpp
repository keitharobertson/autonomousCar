//Some Helpfully Implemented Real Time Tasks
#include <stdlib.h>
#include <sched.h>
#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
#include <time.h>
#include <iostream>

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

float ms_time_diff(struct timespec* t1, struct timespec* t2) {
	long ns_diff = t2->tv_nsec - t1->tv_nsec;
	int sec_diff = t2->tv_sec - t1->tv_sec;
	if(ns_diff < 0) {
		ns_diff += 1000000000;
		sec_diff -= 1;
	}
	return (float)(sec_diff*1000) + ((float)(ns_diff/10000))/100.0;
}
