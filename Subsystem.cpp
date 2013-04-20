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
	// disable subsystem by default
	enabled = 0;
	//set message queue priority default to 10
	prio = 10;
	// Open message queue
	sys_mq = mq_open((char*)MQ_SYSTEM, O_RDWR | O_CREAT, 0664, &attr);
	if(sys_mq == ERROR){
		perror("failed to open sys mq");
		exit(-1);
	}
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
