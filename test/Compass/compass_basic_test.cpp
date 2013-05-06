#include <iostream>

#include "../../SUBSYS_COMMANDS.h"

/**
 * \brief Basic Compass Test
 * 
 * Will output compass readings
 */
int main() {
	
	std::cout << "en_subsys " << SUBSYS_COMPASS << std::endl; //enable compass subsystem
	
	struct timespec t;
	//run for 30 seconds
	for(int i=0; i<30; i++){
		//read out compass heading
		//std::cout << "subsys " << 0 << " " << SUBSYS_COMPASS << " " << CPS_GET_READING << std::endl;
		//wait 1 second.
		clock_gettime(CLOCK_MONOTONIC ,&t);
		t.tv_sec += 1;
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
	}
	
	//shutdown and exit
	std::cout << "exit" << std::endl;
	
}
