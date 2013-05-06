#include "system_logger.h"

static void* logger_receive_task(void* c) {
	setup_rt_task(90);
	system_logger* s = (system_logger*)c;
	s->receive_log_messages();
}

system_logger::system_logger(){
	// setup message queue attributes
	attr.mq_maxmsg = MAX_MSG;
	attr.mq_msgsize = MSG_SIZE;
	attr.mq_flags = 0;
	//set message queue priority default to 90
	prio = 90;
	// Open message queue
	logger_mq = mq_open((char*)MQ_LOGGER, O_RDWR | O_CREAT, 0664, &attr);
	if(logger_mq == ERROR){
		perror("failed to open timing mq");
		exit(-1);
	}
	
	if(pthread_create( &tMQLoggerReceiver, NULL, &logger_receive_task, (void *)(this)) != 0) {
		perror("Failed to create logger message receiver task. Will exit now... " );
		exit(-1);
	}
}

system_logger::~system_logger() {
	pthread_cancel(tMQLoggerReceiver);
	mq_close(logger_mq);
	mq_unlink(MQ_LOGGER);
}

void system_logger::log_message(char* message){
	std::cout << "Log Message: " << std::endl << message << std::endl;
}
	
void system_logger::receive_log_messages(){
	char message[4];
	unsigned int priority;
	ssize_t size;
	while(1){
		if((size = mq_receive(logger_mq, message, MSG_SIZE, &priority)) == ERROR){
			perror("System Recieve Failed!");
		}else{
			log_message(*((char**)message));
		}
	}
}
