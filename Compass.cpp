#include <iostream>
#include <time.h>

#include "Compass.h"

//Compass class

Compass::Compass() {
	subsys_name = COMPASS;
	subsys_num = SUBSYS_COMPASS;
}

void Compass::data_grab(){
	
}

void Compass::collector(){
	struct timespec t;
	send_sys_message((char *)std::string("hello from compass collector").c_str());
	clock_gettime(CLOCK_MONOTONIC ,&t);
	t.tv_sec++;
	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
	send_sys_message((char *)std::string("hello from compass collector 1 second later").c_str());
}

void Compass::analysis(){
	struct timespec t;
	send_sys_message((char *)std::string("hello from compass analysis").c_str());
	clock_gettime(CLOCK_MONOTONIC ,&t);
	t.tv_sec++;
	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
	send_sys_message((char *)std::string("hello from compass analysis 1 second later").c_str());
}

void Compass::handle_message(char* message){
	std::cout << "Compass Recieved Message:" << std::endl;
	std::cout << message <<std::endl << std::endl;
}
