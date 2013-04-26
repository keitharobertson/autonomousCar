#include <iostream>

#include "../../SUBSYS_COMMANDS.h"

/**
 * \brief Straight line with no compass test
 * 
 * Will travel in a straight line without compass guidance for 10 seconds.
 */
int main() {
	
	std::cout << "en_subsys " << SUBSYS_STEERING << std::endl; //enable steering subsystem
	std::cout << "en_subsys " << SUBSYS_MOTOR << std::endl; //enable motor subsystem
	
	std::cout << "subsys " << 0 << " " << SUBSYS_STEERING << " " << STR_STRAIGHT << std::endl;
	std::cout << "subsys " << 0 << " " << SUBSYS_MOTOR << " " << MOT_SLOW << std::endl;
	
	//wait 10 seconds
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC ,&t);
	t.tv_sec += 10;
	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
	
	std::cout << "subsys 0 " << SUBSYS_MOTOR << " " << MOT_STOP << std::endl;
	
	//shutdown and exit
	std::cout << "exit" << std::endl;
	
}
