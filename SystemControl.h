#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <mqueue.h>
#include <stdio.h>
#include <string>
#include "MQ_PARAMS.h"
#include "Subsystem.h"

/**
 * \brief System control is the central controller.
 * 
 * Initializes subsystems, manages subsytem state, and manages
 * inter-subsystem communication.
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
		 * calls shutdown
		 */
		~SystemControl();
		
		/**
		 * \brief Shuts the system down
		 * 
		 * deletes the subsystems, cancels the system receiver task, closes the system message queue, and unlinks message queue.
		 */
		void shutdown();
		
		/**
		 * \brief System initialization
		 * 
		 * Initializes all the subsystems and stores their references into the subsys array.  
		 * Each subsystem's index in the array is given by its subsystem number assigned as in SUBSYS_COMMANDS.h 
		 * (see \ref subsys_nums "Subsystem Numbers").
		 */
		void init();
		
		/**
		 * \brief Read in data from the command interface
		 * 
		 * Calls the read_data specific to the subsystem being command and returns the result.
		 */
		void* read_data(int subsys_num, int command);
		
		/**
		 * \brief System message receiver task
		 * 
		 * Receivers and handles system messages.  Relay's inter-subsystem messages to their target destination.
		 */
		void recieve_sys_messages();
		
		/** An array of references to the subsystems.  Each subsystem's index in the array is given 
		 * by its subsystem number assigned as in SUBSYS_COMMANDS.h 
		 * (see \ref subsys_nums "Subsystem Numbers").
		 */
		Subsystem* subsys[NUM_SUBSYSTEMS];
		
		/** The system message queue */
		mqd_t sys_mq;
		
		/** struct to store adc data across subsystems */
		ADC_DATA adc_data;
		
	private:
		/** Message queue attributes */
		struct mq_attr attr;
		/** Message queue Buffer */
		char buf[MSG_SIZE];
		/** message priority */
		unsigned int prio;
		/** System message queue receiver task */
		pthread_t tMQReceiver;
};

#endif
