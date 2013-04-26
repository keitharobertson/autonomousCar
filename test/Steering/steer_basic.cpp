#include <iostream>
#include <stdio.h>

#include "../../SUBSYS_COMMANDS.h"

/**
 * \brief Basic steering Test
 * 
 * Will turn the wheels to the left and right (saturating) flipping direction every second
 */
int main() {
	std::cout << "en_subsys " << SUBSYS_STEERING << std::endl; //enable steering subsystem
	
	struct timespec t;
	int u;
	//run for 30 seconds
	for(int i=0; i<30; i++){
		//turn the wheels
		if(i%2){
			u=10000;
		}else{
			u=20000;
		}
		std::cout << "subsys " << 1 << " " << SUBSYS_STEERING << " " << STR_SET_STEERING << " " << u << std::endl;
		//wait 1 second.
		clock_gettime(CLOCK_MONOTONIC ,&t);
		t.tv_sec += 1;
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
	}
	
	u=15000;
	std::cout << "subsys " << 1 << " " << SUBSYS_STEERING << " " << STR_SET_STEERING << " " << u << std::endl;
	
	//shutdown and exit
	std::cout << "exit" << std::endl;
	
}
