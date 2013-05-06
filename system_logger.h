#ifndef _LOGGER_
#define	_LOGGER_

#include <mqueue.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "shirtt.h"
#include "MQ_PARAMS.h"

class system_logger {
	public:
		system_logger();
		
		~system_logger();

		void log_message(char* message);
	
		void receive_log_messages();
	private:
		/** Message queue attributes */
		struct mq_attr attr; 
		
		/** message queue descriptor */
		mqd_t logger_mq;
		
		/** message queue priority */
		unsigned int prio;
		
		/** Logger message queue receiver task */
		pthread_t tMQLoggerReceiver;
		
};

#endif
