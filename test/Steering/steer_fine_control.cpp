#include <iostream>
#include <stdio.h>

#include "../../SUBSYS_COMMANDS.h"

/**
 * \brief steering fine control test
 * 
 * The result of the fine control test should be that the front wheels start pointing all
 * the way to the left and slowly and evenly without large jumps turn all the way to the
 * right. The process should take 10 seconds to complete.
 */
int main() {
	std::cout << "en_subsys " << SUBSYS_STEERING << std::endl; //enable steering subsystem
	
	struct timespec t;
	int u;
	//run for 10 seconds
	for(int i=0; i<10; i++){
		//turn the wheels
		u = 10000+i*1000;
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
