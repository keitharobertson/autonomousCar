/**
 * \file Subsystem.h
 * \brief Subsystem class definition
 */

#ifndef 	SUBSYSTEM_H_
#define 	SUBSYSTEM_H_

#include <mqueue.h>
#include <stdio.h>
#include <string>
#include "MQ_PARAMS.h"
#include "SUBSYS_COMMANDS.h"

/**
 * \class Subsystem.h
 * \brief Subsystem class defines the basic properties shared by all subsystems.
 * 
 * Includes message send (system and subsystem) functionality and subsytem receiver task.
 */
class Subsystem{
	public:
	
		/**
		 * \brief Subsystem default constructor
		 * 
		 * Sets up subsystem and system message queues and receiver task
		 */
		Subsystem();
		
		/**
		 * \brief Subsystem destructor
		 * 
		 * cancels receiver thread and closes and unlinks message queues
		 */
		~Subsystem();
		
		/**
		 * \brief initializes subsystem tasks
		 * 
		 * Virtual function to be defined at subsystem type level.
		 */
		virtual void init() = 0;
		
		/**
		 * \brief send message to subsystem
		 * 
		 * sends a message using subsystem heap message queue
		 */
		void send_message(MESSAGE* message);
		
		/**
		 * \brief send message to system
		 * 
		 * sends a message to the system using system heap message queue
		 */
		void send_sys_message(char* message);
		
		/**
		 * \brief Shutdown subsystem
		 * 
		 * Virtual function to be defined at subsystem type level
		 */
		virtual void shutdown() = 0;
		
		/**
		 * \brief subsystem message queue receiver task 
		 * 
		 * subsystem receiver task. Uses a blocking mq_receive and waits 
		 * subsystem messages. Sends messages on to subsystem
		 * handle_message.
		 */
		void receive_subsys_messages();
		
		std::string subsys_name;//subsystem name
		int subsys_num;//subsystem number;
		
	protected:
	
		/**
		 * \brief handles messages sent to the subsystem
		 * 
		 * virtual function defined at the specific subsystem level
		 */
		virtual void handle_message(char* message) = 0;
		
		struct mq_attr attr; //queue attributes
		mqd_t subsys_mq; //message queue descriptor
		mqd_t sys_mq; //message queue descriptor
		unsigned int prio;// Priority 
		int iret_mq_receiver;
		pthread_t tMQReceiver;
};

#endif
