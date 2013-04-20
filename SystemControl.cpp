#include <iostream>
#include <stdlib.h>

#define SHUTDOWN_DEBUG

#include "SystemControl.h"
#include "Compass.h"
#include "shirtt.h"
#include "Motor.h"
#include "Steering.h"
#include "Sonar.h"
#include "GPS.h"
#include "SUBSYS_COMMANDS.h"
 
static void* sys_receive_task(void* c) {
	setup_rt_task(5);
	SystemControl* s = (SystemControl*)c;
	s->recieve_sys_messages();
}

SystemControl::SystemControl() {
	 // setup message queue attributes
	attr.mq_maxmsg = MAX_MSG;
	attr.mq_msgsize = MSG_SIZE;
	attr.mq_flags = 0;
	sys_mq = mq_open(MQ_SYSTEM, O_RDWR | O_CREAT, 0664, &attr);
	if(sys_mq == ERROR){
		perror("failed to open sys mq from system");
		exit(-1);
	}
	if(pthread_create( &tMQReceiver, NULL, &sys_receive_task, (void *)(this)) != 0) {
		perror("Failed to create system message receiver task. Will exit now... " );
		exit(-1);
	}
}

SystemControl::~SystemControl() {
	shutdown();
}

void SystemControl::shutdown() {
	for(int i=0;i<NUM_SUBSYSTEMS;i++){
		#ifdef SHUTDOWN_DEBUG
			std::cout << "shutting down subsystem " << i << std::endl;
		#endif
		subsys[i]->shutdown();
		delete subsys[i];
		#ifdef SHUTDOWN_DEBUG
			std::cout << "subsystem " << i << " successfully shut down" << std::endl;
		#endif
	}
	#ifdef SHUTDOWN_DEBUG
		std::cout << "cancelling system mq receive function " << std::endl;
	#endif
	pthread_cancel(tMQReceiver);
	#ifdef SHUTDOWN_DEBUG
		std::cout << "closing system message queue" << std::endl;
	#endif
	mq_close(sys_mq);
	#ifdef SHUTDOWN_DEBUG
		std::cout << "deleting system message queue" << std::endl;
	#endif
	mq_unlink(MQ_SYSTEM);
}

void SystemControl::init() {
	
	subsys[SUBSYS_MOTOR] = new Motor();
	subsys[SUBSYS_MOTOR]->init();
	
	subsys[SUBSYS_STEERING] = new Steering();
	subsys[SUBSYS_STEERING]->init();
	
	subsys[SUBSYS_COMPASS] = new Compass();
	subsys[SUBSYS_COMPASS]->init();

	subsys[SUBSYS_SONAR] = new Sonar();
	subsys[SUBSYS_SONAR]->init();
	
	subsys[SUBSYS_GPS] = new GPS();
	subsys[SUBSYS_GPS]->init();
}

void* SystemControl::read_data(int subsys_num, int command){
	return subsys[subsys_num]->read_data(command);
}

void SystemControl::handle_sys_message(MESSAGE* mess) {
	switch(mess->command) {
		case 
		default:
			std::cout << "Message with unknown command sent to System Control! Ignoring..." << std::endl;
			break;
	}
}

void SystemControl::recieve_sys_messages() {
	char message[4];
	unsigned int priority;
	ssize_t size;
	MESSAGE* mess;
	while(1){
		if((size = mq_receive(sys_mq, message, MSG_SIZE, &priority)) == ERROR){
			perror("System Recieve Failed!");
		}else{
			unsigned int subsys_num = 0;
			mess = ( (MESSAGE*)(*((unsigned int*)message)) );
			#ifdef DEBUG_SYS_MESSAGES
				std::cout << "Sys message received from " << subsys[mess->from]->subsys_name<< " (" << mess->from << "):" << std::endl;
				std::cout << "to: " << mess->to << std::endl;
				std::cout << "size of subsys array=" << sizeof(subsys)/sizeof(Subsystem*) << std::endl;
				std::cout << "subsys enabled: " << subsys[mess->to]->enabled << std::endl;
				std::cout << "Message to: " << subsys[mess->to]->subsys_name<< " (" << mess->to << ")" << std::endl;
				std::cout << "Command: " << mess->command << std::endl;
				std::cout << "Data: " << mess->data << std::endl;
				std::cout << std::endl;
			#endif
			//send message to recipient subsystem
			subsys[mess->to]->handle_message(mess);
		}
	}
}

