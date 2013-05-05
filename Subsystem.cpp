#include <iostream>
#include <stdlib.h>
#include <string.h>
#include "Subsystem.h"
#include "shirtt.h"

#define SHUTDOWN_DEBUG

Subsystem::Subsystem(){
	 // setup message queue attributes
	attr.mq_maxmsg = MAX_MSG;
	attr.mq_msgsize = MSG_SIZE;
	attr.mq_flags = 0;
	// setup timin message queue attributes
	timing_attr.mq_maxmsg = MAX_MSG;
	timing_attr.mq_msgsize = MSG_SIZE;
	timing_attr.mq_flags = 0;
	// disable subsystem by default
	enabled = 0;
	//set system message queue priority default to 10
	prio = 10;
	//set timing message queue priority default to 90
	timing_prio = 90;
	// Open message queue
	sys_mq = mq_open((char*)MQ_SYSTEM, O_RDWR | O_CREAT, 0664, &attr);
	if(sys_mq == ERROR){
		perror("failed to open sys mq");
		exit(-1);
	}
	// Open timing message queue
	timing_mq = mq_open((char*)MQ_TIMING, O_RDWR | O_CREAT, 0664, &timing_attr);
	if(timing_mq == ERROR){
		perror("failed to open timing mq");
		exit(-1);
	}
	
	num_timing_messages = 0;
}

Subsystem::~Subsystem(){
	#ifdef SHUTDOWN_DEBUG
		std::cout << "Subsystem destructor" << std::endl;
	#endif
	//close message queue
	mq_close(sys_mq);
}

void Subsystem::send_sys_message(MESSAGE* message){
	char* mess = new char[4];
	memcpy(mess, &message, sizeof(MESSAGE*));
	if(mq_send (sys_mq, mess, MSG_SIZE, prio) == ERROR) {
		perror("System message failed to send!");
		exit(-1);
	}
}

void Subsystem::log_release_time(struct timespec* release_time, int task_num){
	send_timing_message(release_time, task_num, true);
}

void Subsystem::log_end_time(struct timespec* end_time, int task_num){
	send_timing_message(end_time, task_num, false);
}

void Subsystem::send_timing_message(struct timespec* time, int task_num, bool is_release){
	timing_analysis[num_timing_messages % TIMING_ANALYSIS_BUFFER_SIZE].tv_sec = time->tv_sec;
	timing_analysis[num_timing_messages % TIMING_ANALYSIS_BUFFER_SIZE].tv_nsec = time->tv_nsec;
	MESSAGE* message = new MESSAGE(subsys_num, task_num, TIM_RELEASE_TIME, (void*)(&timing_analysis[num_timing_messages % TIMING_ANALYSIS_BUFFER_SIZE]));
	num_timing_messages++;
	char* mess = new char[4];
	memcpy(mess, &message, sizeof(MESSAGE*));
	if(mq_send (timing_mq, mess, MSG_SIZE, prio) == ERROR) {
		perror("Timing analysis message failed to send!");
		exit(-1);
	}
}
