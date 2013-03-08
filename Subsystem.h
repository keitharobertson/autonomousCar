#ifndef 	SUBSYSTEM_H_
#define 	SUBSYSTEM_H_

#include <mqueue.h>
#include <stdio.h>
#include <string>
#include "MQ_PARAMS.h"

class Subsystem{
	public:
		Subsystem();
		~Subsystem();
		virtual void init() = 0;
		void send_message(char* message);
		void send_sys_message(char* message);
		virtual void shutdown() = 0;
		void recieve_subsys_messages();
		
		std::string subsys_name;//subsystem name
		int subsys_num;//subsystem number;
		
	protected:
		virtual void handle_message(char* message) = 0;
		
		struct mq_attr attr, old_attr; //queue attributes
		struct sigevent sigevent;// For notification
		char buf[MSG_SIZE];// A good-sized buffer
		mqd_t subsys_mq; //message queue descriptor
		mqd_t sys_mq; //message queue descriptor
		unsigned int prio;// Priority 
		int iret_mq_receiver;
		pthread_t tMQReceiver;
		char sys_message_buffer[MSG_SIZE];
		char subsys_message_buffer[MSG_SIZE];
};

#endif
