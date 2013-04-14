#include <iostream>
#include <stdlib.h>
#include <string.h>
#include "Subsystem.h"
#include "shirtt.h"

#define SHUTDOWN_DEBUG

/*
static void* subsys_receive_task(void* c) {
	setup_rt_task(10);
	Subsystem* s = (Subsystem*)c;
	s->receive_subsys_messages();
}
* */

Subsystem::Subsystem(){
	 // setup message queue attributes
	attr.mq_maxmsg = MAX_MSG;
	attr.mq_msgsize = MSG_SIZE;
	attr.mq_flags = 0;
	// disable subsystem by default
	enabled = 0;
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
	//iret_mq_receiver = pthread_create( &tMQReceiver, NULL, &subsys_receive_task, (void *)(this));
}

Subsystem::~Subsystem(){
	#ifdef SHUTDOWN_DEBUG
		std::cout << "Subsystem destructor" << std::endl;
	#endif
	//pthread_cancel(tMQReceiver);
	mq_close(subsys_mq);
	//mq_unlink((char*)((std::string("/aMQ_").append(subsys_name)).c_str()));
	mq_unlink((char*)((std::string("/MQ_").append(subsys_name)).c_str()));
}

/*
void Subsystem::receive_subsys_messages() {
	char message[sizeof(char*)];
	unsigned int priority;
	ssize_t size;
	while(1){
		if((size = mq_receive(subsys_mq, message, MSG_SIZE, &priority)) == ERROR){
			perror("Subsystem Recieve Failed!");
		}else{
			handle_message(*((char**)message));
			delete *((char**)message);
		}
	}
}
* */

void Subsystem::send_message(MESSAGE* message){
	if(mq_send (subsys_mq, (char*)message, MSG_SIZE, prio) == ERROR) {
		perror("Subsystem message failed to send!\n");
		std::cout << "--Message:--" << std::endl;
		std::cout << "To: " << subsys_name << std::endl;
		std::cout << "From: " << message->from << std::endl;
		std::cout << "Command: " << message->command << std::endl;
		std::cout << "Data: " << message->data << std::endl;
		exit(-1);
	}
}

void Subsystem::send_sys_message(MESSAGE* message){
	char* mess = new char[4];
	memcpy(mess, &message, sizeof(MESSAGE*));
	if(mq_send (sys_mq, mess, MSG_SIZE, prio) == ERROR) {
		perror("System message failed to send!");
		exit(-1);
	}
}
