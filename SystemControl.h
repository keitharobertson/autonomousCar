#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <mqueue.h>
#include <stdio.h>
#include <string>
#include "MQ_PARAMS.h"
#include "Subsystem.h"

#define NUM_SUBSYSTEMS	6

/**
 * \class SystemControl
 * \brief System control module
 * 
 * Initializes subsystems, manages subsytem state, manages
 * inter-subsystem communication
 */
class SystemControl {
	public:
		/**
		 * \brief SystemControl default constructor
		 * 
		 * sets up system message queue and set up system message receiver task
		 */
		SystemControl();
		
		/**
		 * \brief SystemControl destructor
		 * 
		 * cancels system receiver task, closes message queue, and unlinks message queue
		 */
		~SystemControl();
		
		/**
		 * \brief System initialization
		 * 
		 * initializes all the subsystems
		 */
		void init(); //initialize the subsystems
		
		/**
		 * \brief System message receiver task
		 * 
		 * receivers and handles system messages
		 */
		void recieve_sys_messages();//recieve task for system mq
	private:
		struct mq_attr attr; //queue attributes
		//struct sigevent sigevent;// For notification
		char buf[MSG_SIZE];// A good-sized buffer
		unsigned int prio;// Priority
		mqd_t sys_mq; //system mq
		mqd_t subsys_mq[NUM_SUBSYSTEMS];//subsys mq
		Subsystem* subsys[NUM_SUBSYSTEMS]; //array of subsystems
		int iret_mq_receiver;
		pthread_t tMQReceiver; //sys mq receiver task
		
};

#endif
