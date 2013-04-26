#include <iostream>

#include "../../SUBSYS_COMMANDS.h"

/**
 * \brief motor steering compass
 * 
 * will go in square
 */
int main() {
	
	struct timespec t;
	//wait for 10 seconds
	clock_gettime(CLOCK_MONOTONIC ,&t);
	t.tv_sec += 10;
	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
	
	
	std::cout << "en_subsys " << SUBSYS_MOTOR << std::endl; //enable motor subsystem
	std::cout << "en_subsys " << SUBSYS_COMPASS << std::endl; //enable COMPASS subsystem
	std::cout << "en_subsys " << SUBSYS_STEERING << std::endl; //enable steering subsystem
	std::cout << "subsys " << 0 << " " << SUBSYS_MOTOR << " " << MOT_SLOW << std::endl; //make motor go slow
	
	//north for 10 seconds
	clock_gettime(CLOCK_MONOTONIC ,&t);
	t.tv_sec += 4;
	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
	
	std::cout << "subsys " << 0 << " " << SUBSYS_COMPASS << " " << CPS_LEFT_90 << std::endl; //make motor go slow
	
	//west for 5 seconds
	clock_gettime(CLOCK_MONOTONIC ,&t);
	t.tv_sec += 4;
	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
	
	std::cout << "subsys " << 0 << " " << SUBSYS_COMPASS << " " << CPS_LEFT_90 << std::endl; //make motor go slow
	
	//south for 5 seconds
	clock_gettime(CLOCK_MONOTONIC ,&t);
	t.tv_sec += 4;
	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
	
	std::cout << "subsys " << 0 << " " << SUBSYS_COMPASS << " " << CPS_LEFT_90 << std::endl; //make motor go slow
	
	//east for 5 seconds
	clock_gettime(CLOCK_MONOTONIC ,&t);
	t.tv_sec += 4;
	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
	
	std::cout << "subsys " << 0 << " " << SUBSYS_MOTOR << " " << MOT_STOP << std::endl; //make motor go slow
	
	//shutdown and exit
	std::cout << "exit" << std::endl;
	
}
