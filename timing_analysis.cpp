#include "timing_analysis.h"

#define REPORT_COUNT	100

static void* timing_receive_task(void* c) {
	setup_rt_task(90);
	timing_analysis* s = (timing_analysis*)c;
	s->receive_timing_messages();
}

timing_analysis::timing_analysis() {
	// setup message queue attributes
	attr.mq_maxmsg = MAX_MSG;
	attr.mq_msgsize = MSG_SIZE;
	attr.mq_flags = 0;
	//set message queue priority default to 90
	prio = 90;
	// Open message queue
	timing_mq = mq_open((char*)MQ_TIMING, O_RDWR | O_CREAT, 0664, &attr);
	if(timing_mq == ERROR){
		perror("failed to open timing mq");
		exit(-1);
	}
	
	if(pthread_create( &tMQTimingReceiver, NULL, &timing_receive_task, (void *)(this)) != 0) {
		perror("Failed to create system message receiver task. Will exit now... " );
		exit(-1);
	}
	
	timing_count = 0;
}

timing_analysis::~timing_analysis() {
	pthread_cancel(tMQTimingReceiver);
	mq_close(timing_mq);
	mq_unlink(MQ_TIMING);
}

void timing_analysis::receive_timing_messages(){
	char message[4];
	unsigned int priority;
	ssize_t size;
	MESSAGE* mess;
	while(1){
		if((size = mq_receive(timing_mq, message, MSG_SIZE, &priority)) == ERROR){
			perror("System Recieve Failed!");
		}else{
			unsigned int subsys_num = 0;
			mess = ( (MESSAGE*)(*((unsigned int*)message)) );
			switch(mess->command){
				case TIM_RELEASE_TIME:
					timing_info[mess->from][mess->to].add_release_time((struct timespec*)mess->data);
					break;
				case TIM_END_TIME:
					timing_info[mess->from][mess->to].add_release_time((struct timespec*)mess->data);
					break;
				default:
					break;
			}
			if(timing_count == REPORT_COUNT){
				timing_count == 0;
				std::cout << "Timing Information" << std::endl;
				std::cout << "------------------" << std::endl;
				for(int i=0;i<NUM_SUBSYSTEMS; i++) {
					std::cout << "Subsystem: " << i << std::endl;
					for(int j=0;j<NUM_TASKS; j++) {
						if(timing_info[mess->from][mess->to].get_max_period() > 0.0){
							std::cout << "Task: " << j << std::endl;
							std::cout << "Period (avg, min, max): " << timing_info[i][j].get_average_period() << ", " << timing_info[i][j].get_min_period() << ", " << timing_info[i][j].get_max_period() << std::endl;
							std::cout << "Execution (avg, min, max): " << timing_info[i][j].get_average_execution() << ", " << timing_info[i][j].get_min_execution() << ", " << timing_info[i][j].get_max_execution() << std::endl;
						}
					}
				}
			}
			timing_count++;
		}
	}
}

void timing_analysis::log_timing(char* message){
	
}
