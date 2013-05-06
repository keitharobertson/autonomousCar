#ifndef _TIMING_ANALYSIS_
#define _TIMING_ANALYSIS_

#include <mqueue.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "MQ_PARAMS.h"
#include "SUBSYS_COMMANDS.h"
#include "shirtt.h"
#include "subsys_timing.h"
#include "TASK_NUMBERS.h"

#include "system_logger.h"


class timing_analysis : system_logger {
	public:
	
		timing_analysis();
		
		~timing_analysis();
		
		void receive_timing_messages();
		
		void log_timing(char* message);
		
	private:
	
		void clear_queue();
	
		/** Message queue attributes */
		struct mq_attr attr; 
		
		/** message queue descriptor */
		mqd_t timing_mq;
		
		/** message queue priority */
		unsigned int prio;
		
		/** Logger message queue receiver task */
		pthread_t tMQTimingReceiver;
		
		subsys_timing timing_info[NUM_SUBSYSTEMS][NUM_TASKS];
		int timing_count;
		
};

#endif
