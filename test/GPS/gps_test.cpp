#include <iostream>

#include "../../SUBSYS_COMMANDS.h"

int main() {
	//forward full speed for 5 seconds
	
	std::cout << "en_subsys " << SUBSYS_GPS << std::endl;
	std::cout << "subsys 0 " << SUBSYS_GPS << " " << GPS_DISPLAY << std::endl;
	
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC ,&t);
	t.tv_sec += 20;
	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
	
	//std::cout << "subsys 0 " << SUBSYS_MOTOR << " " << MOT_STOP << std::endl;
	
	std::cout << "exit" << std::endl;
	
}
