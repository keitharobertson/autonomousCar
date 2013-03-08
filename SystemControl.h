#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <mqueue.h>
#include <stdio.h>
#include <string>
#include "MQ_PARAMS.h"
#include "Subsystem.h"

#define NUM_SUBSYSTEMS	1

class SystemControl {
	public:
		SystemControl();
		~SystemControl();
		void init();
		void recieve_sys_messages();
	private:
		struct mq_attr attr, old_attr; //queue attributes
		struct sigevent sigevent;// For notification
		char buf[MSG_SIZE];// A good-sized buffer
		unsigned int prio;// Priority
		mqd_t sys_mq; //system mq
		mqd_t subsys_mq[NUM_SUBSYSTEMS];//subsys mq
		Subsystem* subsys[NUM_SUBSYSTEMS];
		int iret_mq_receiver;
		pthread_t tMQReceiver;
		
};

#endif
