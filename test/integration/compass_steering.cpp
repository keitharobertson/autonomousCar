#include <iostream>

#include "../../SUBSYS_COMMANDS.h"

/**
 * \brief Steering/Compass Integration Test
 * 
 * Will set the desired heading to be north and control the wheels accordingly. If the system is rotated, 
 * the front wheels should turn such a way that if the car was moving would correct the direction.
 * Runs for 30 seconds
 */
int main() {
	
	std::cout << "en_subsys " << SUBSYS_STEERING << std::endl; //enable steering subsystem
	std::cout << "en_subsys " << SUBSYS_COMPASS << std::endl; //enable compass subsystem
	
	std::cout << "subsys " << 1 << " " << SUBSYS_COMPASS << " " << CPS_SET_HEADING << " " << 0 << std::endl; //set compass heading north
	
	//wait 30 seconds
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC ,&t);
	t.tv_sec += 30;
	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
	
	std::cout << "subsys 0 " << SUBSYS_STEERING << " " << STR_STRAIGHT << std::endl; //return steering to straight before shutdown
	
	//shutdown and exit
	std::cout << "exit" << std::endl;
	
}
