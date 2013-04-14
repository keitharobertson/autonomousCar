#include <iostream>

#include "../SUBSYS_COMMANDS.h"

int main() {
	//forward full speed for 5 seconds
	
	std::cout << "en_subsys " << SUBSYS_MOTOR << std::endl;
	std::cout << "subsys " << 0 << " " << SUBSYS_MOTOR << " " << MOT_FAST << std::endl;
	
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC ,&t);
	t.tv_sec += 5;
	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
	
	std::cout << "subsys 0 " << SUBSYS_MOTOR << " " << MOT_STOP << std::endl;
	
	std::cout << "exit" << std::endl;
	
}
