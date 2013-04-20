#include <iostream>

#include "../../SUBSYS_COMMANDS.h"

/**
 * \brief Basic Sonar Test
 * 
 * Will output distances from sonar for 20 seconds
 */
int main() {
	
	std::cout << "en_subsys " << SUBSYS_SONAR << std::endl; //enable sonar subsystem
	std::cout << "subsys " << 0 << " " << SUBSYS_SONAR << " " << SNR_PRINT_DATA << " " << 1 << std::endl; //enable printing of data
	
	//run for 20 seconds
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC ,&t);
	t.tv_sec += 20;
	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
	
	
	//shutdown and exit
	std::cout << "exit" << std::endl;
	
}
