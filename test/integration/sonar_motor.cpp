#include <iostream>

#include "../../SUBSYS_COMMANDS.h"

/**
 * \brief Sonar Motor Integration Test
 * 
 * Will output distances from sonar for 20 seconds.  Will control motor speed according to object proximity.
 */
int main() {
	
	std::cout << "en_subsys " << SUBSYS_SONAR << std::endl; //enable sonar subsystem
	std::cout << "en_subsys " << SUBSYS_MOTOR << std::endl; //enable steering subsystem
	std::cout << "subsys " << 0 << " " << SUBSYS_SONAR << " " << SNR_PRINT_DATA << " " << 1 << std::endl; //enable printing of data
	
	//run for 20 seconds
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC ,&t);
	t.tv_sec += 30;
	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
	
	
	//shutdown and exit
	std::cout << "exit" << std::endl;
	
}
