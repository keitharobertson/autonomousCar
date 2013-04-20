#ifndef 	SUBSYSTEM_H_
#define 	SUBSYSTEM_H_

#include <mqueue.h>
#include <stdio.h>
#include <string>
#include "MQ_PARAMS.h"
#include "SUBSYS_COMMANDS.h"

/**
 * \class Subsystem
 * \brief The Subsystem class defines the basic properties shared by all subsystems.
 * 
 * Includes message send (system and subsystem) functionality and subsytem receiver task.
 */
class Subsystem{
	public:
	
		/**
		 * \brief Subsystem default constructor
		 * 
		 * Initializes the subsystem variables and sets up the message queue for system communication.
		 */
		Subsystem();
		
		/**
		 * \brief Subsystem destructor
		 * 
		 * closes the message queue
		 */
		~Subsystem();
		
		/**
		 * \brief Initializes the subsystem tasks particular to that subsystem.
		 * 
		 * Virtual function to be defined at subsystem type level. 
		 */
		virtual void init() = 0;
		
		/**
		 * \brief send message to system
		 * 
		 * sends a message to the system using system heap message queue.  This is used to send inter-subsystem messages.  
		 * The system processes the messages and delivers them to the appropriate subsystem.
		 */
		void send_sys_message(MESSAGE* message);
		
		/**
		 * \brief Shutdown subsystem
		 * 
		 * Virtual function to be defined at subsystem type level.  Responsible for deleting all tasks and 
		 * freeing any dynamic memory before subsystem is shut down.
		 */
		virtual void shutdown() = 0;
		
		/**
		 * \brief Reads in data from message to the actuator.
		 * 
		 * Virtual function implemented at the subsystem level.
		 * Will read data into appropriate data type and then cast to a void* and return.  
		 * The type of data received depends on the message command.  
		 * For example if trying to set the motor pwm duty cycle, a three byte char* is expected. 
		 * This function will read the data in from standard in (using cin) into an appropriate datatype 
		 * for the given command and will return the data to the system message handler for further processing.
		 */
		virtual void* read_data(int command) = 0;
		
		
		/**
		 * \brief Handles message sent to the subsystem
		 * 
		 * Virtual function defined at the specific subsystem level. Will handle the messages sent to the Subsystem
		 *  from the system controller.  Messages could be from command line interface or from another subsystem. 
		 */
		virtual void handle_message(MESSAGE* message) = 0;
		
		/** Subsystem name */
		std::string subsys_name;
		
		/** Subsystem number (assigned in SUBSYS_COMMANDS.h) */
		int subsys_num;
		
		/** if subsystem is enabled (0=disabled, 1=enabled) */
		int enabled;
		
		
	protected:
		
		/** Message queue attributes */
		struct mq_attr attr; 
		
		/** message queue descriptor */
		mqd_t sys_mq;
		
		/** message queue priority */
		unsigned int prio;

};

#endif
