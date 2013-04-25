#include <iostream>

#include "../../SUBSYS_COMMANDS.h"

/**
 * \brief motor steering compass
 * 
 * will go north for 5 seconds 
 */
int main() {
	
	std::cout << "en_subsys " << SUBSYS_MOTOR << std::endl; //enable motor subsystem
	std::cout << "en_subsys " << SUBSYS_COMPASS << std::endl; //enable COMPASS subsystem
	std::cout << "en_subsys " << SUBSYS_STEERING << std::endl; //enable steering subsystem
	std::cout << "subsys " << 0 << " " << SUBSYS_MOTOR << " " << MOT_SLOW << std::endl; //make motor go slow
	
	//run for 20 seconds
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC ,&t);
	t.tv_sec += 20;
	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
	
	std::cout << "subsys " << 0 << " " << SUBSYS_MOTOR << " " << MOT_STOP << std::endl; //make motor go slow
	
	//shutdown and exit
	std::cout << "exit" << std::endl;
	
}
