#include <iostream>

#include "../../SUBSYS_COMMANDS.h"

/**
 * \brief Motor Basic Test
 * 
 * Flips between forward and reverse every second
* 
 */
int main() {	
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC ,&t);
	t.tv_sec += 2;
	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
	
	std::cout << "en_subsys " << SUBSYS_MOTOR << std::endl;
	int u;
	for(int i=0; i<20; i++){
		if(i%2){
			u=14000;
		}else{
			u=16000;
		}
		std::cout << "subsys " << 1 << " " << SUBSYS_MOTOR << " " << MOT_SET_SPEED << " " << u << std::endl;
		//wait 1 second
		clock_gettime(CLOCK_MONOTONIC ,&t);
		t.tv_sec += 1;
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
	}
	
	std::cout << "subsys 0 " << SUBSYS_MOTOR << " " << MOT_STOP << std::endl;
	
	std::cout << "exit" << std::endl;
	
}
