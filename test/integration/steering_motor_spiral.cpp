#include <iostream>

#include "../../SUBSYS_COMMANDS.h"

/**
 * \brief Steering/Motor Integration Spiral Test
 * 
 * Will turn the steering all the way to the left and slowly increase the motor speed. Should spiral outwards.
 */
int main() {
	
	std::cout << "en_subsys " << SUBSYS_STEERING << std::endl; //enable steering subsystem
	std::cout << "en_subsys " << SUBSYS_MOTOR << std::endl; //enable motor subsystem
	
	std::cout << "subsys " << 0 << " " << SUBSYS_STEERING << " " << STR_HARD_LEFT << std::endl;
	
	struct timespec t;
	int u;
	
	for(int i=0; i<10; i++){
		u = 15000 - i*500;
		std::cout << "subsys " << 1 << " " << SUBSYS_MOTOR << " " << MOT_SET_SPEED << " " << u << std::endl;
		//wait 1 second
		clock_gettime(CLOCK_MONOTONIC ,&t);
		t.tv_sec += 1;
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
	}
	
	std::cout << "subsys 0 " << SUBSYS_MOTOR << " " << MOT_STOP << std::endl;
	
	//shutdown and exit
	std::cout << "exit" << std::endl;
	
}
