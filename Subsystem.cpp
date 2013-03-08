#include <iostream>
#include <stdlib.h>
#include "Subsystem.h"
#include "shirtt.h"

static void* subsys_receive_task(void* c) {
	setup_rt_task(10);
	Subsystem* s = (Subsystem*)c;
	s->recieve_subsys_messages();
}

Subsystem::Subsystem(){
	 // setup message queue attributes
	attr.mq_maxmsg = MAX_MSG;
	attr.mq_msgsize = MSG_SIZE;
	attr.mq_flags = 0;
	// Open message queues
	subsys_mq = mq_open( (char*)((std::string("/MQ_").append(subsys_name)).c_str()), O_RDWR | O_CREAT, 0664, &attr);
	if(subsys_mq == ERROR){
		perror("failed to open subsys mq");
		exit(-1);
	}
	sys_mq = mq_open((char*)MQ_SYSTEM, O_RDWR | O_CREAT, 0664, &attr);
	if(sys_mq == ERROR){
		perror("failed to open sys mq");
		exit(-1);
	}
	iret_mq_receiver = pthread_create( &tMQReceiver, NULL, &subsys_receive_task, (void *)(this));
}

Subsystem::~Subsystem(){
	pthread_cancel(tMQReceiver);
	mq_close(subsys_mq);
}

void Subsystem::recieve_subsys_messages() {
	char message[MSG_SIZE];
	unsigned int priority;
	ssize_t size;
	while(1){
		if((size = mq_receive(subsys_mq, message, MSG_SIZE, &priority)) == ERROR){
			perror("Recieve Failed!");
		}else{
			handle_message(message);
		}
	}
}

void Subsystem::send_message(char* message){
	message[MSG_SIZE] = '\0';
	if(mq_send (subsys_mq, message, MSG_SIZE, prio) == ERROR) {
		perror("Subsystem message failed to send!");
		exit(-1);
	}
}

void Subsystem::send_sys_message(char* message){
	message[MSG_SIZE-1] = '\0';
	message[MSG_SIZE] = (char)subsys_num;
	if(mq_send (sys_mq, message, MSG_SIZE, prio) == ERROR) {
		perror("System message failed to send!");
		exit(-1);
	}
}
