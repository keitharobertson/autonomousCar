#include <iostream>
#include <stdlib.h>
#include <string.h>
#include "Subsystem.h"
#include "shirtt.h"

#define SHUTDOWN_DEBUG

static void* timing_send_task(void* c) {
	setup_rt_task(30);
	Subsystem* s = (Subsystem*)c;
	s->timing_mq_send();
}

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
	timing_mq = mq_open((char*)MQ_TIMING, O_RDWR | O_CREAT | O_NONBLOCK, 0664, &timing_attr);
	if(timing_mq == ERROR){
		perror("failed to open timing mq");
		exit(-1);
	}
	
	/*if(sem_init(&timer_msg_send_sem, 0, 0) != 0){
		perror("Failed to init the timer message send sem \n");
	}*/
	
	num_timing_messages = 0;
	num_timing_messages_sent = 0;
	/*if(pthread_create( &tMQTimingSend, NULL, &timing_send_task, (void *)(this)) != 0) {
		perror("Failed to create timing message sending task. Will exit now... " );
		exit(-1);
	}*/
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
	
	timing_message[num_timing_messages % TIMING_ANALYSIS_BUFFER_SIZE].from = subsys_num;
	timing_message[num_timing_messages % TIMING_ANALYSIS_BUFFER_SIZE].to = task_num;
	timing_message[num_timing_messages % TIMING_ANALYSIS_BUFFER_SIZE].command = (is_release) ? TIM_RELEASE_TIME : TIM_END_TIME;
	timing_message[num_timing_messages % TIMING_ANALYSIS_BUFFER_SIZE].data = (void*)(&timing_analysis[num_timing_messages % TIMING_ANALYSIS_BUFFER_SIZE]);
	char mess[4];
	MESSAGE* tmp_mess = &timing_message[num_timing_messages % TIMING_ANALYSIS_BUFFER_SIZE];
		memcpy(mess, &tmp_mess, sizeof(MESSAGE*));
		if(mq_send (timing_mq, mess, MSG_SIZE, prio) == ERROR) {
			perror("Timing analysis message failed to send!");
			exit(-1);
		}
		num_timing_messages_sent++;
		num_timing_messages++;
	
	//sem_post(&timer_msg_send_sem);
}

void Subsystem::timing_mq_send() {
	while(1){
		sem_wait(&timer_msg_send_sem);
	}
}
