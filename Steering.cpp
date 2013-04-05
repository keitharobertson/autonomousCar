#include <iostream>
#include <time.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
 #include <sys/mman.h>

#include "Steering.h"
									
#define REG_BASE				0x48000000
#define GPTIMER10_OFFSET		0x86000
#define TIMER_LOAD_REG			0x02c


Steering::Steering(){
	subsys_name = STEERING;
	subsys_num = SUBSYS_STEERING;
}

Steering::~Steering() {
	close(steering_fd);
}
		
void Steering::init_device(){
	
	/*int mem_fd;
	char mem_filepath[40];
	sprintf(mem_filepath,"/dev/mem");
	if ((mem_fd = open(mem_filepath,O_RDWR | O_SYNC)) < 0) {
		perror("Failed to open the bus for mem.\n");
	}
	std::cout << "mem opened" << std::endl;
	
	if( (gptimer_reg = (volatile uint32_t*)mmap(NULL, 0x100000, PROT_WRITE  | PROT_READ, MAP_SHARED, mem_fd,REG_BASE)) == MAP_FAILED ){
		perror("mmap failed");
	}
	std::cout << "mmap done" << std::endl;
	std::cout << "gptimer value: " << gptimer_reg[GPTIMER10_OFFSET+TIMER_LOAD_REG] << std::endl;
	gptimer_reg[GPTIMER10_OFFSET+TIMER_LOAD_REG] = 0xcccc;
	std::cout << "gptimer value: " << gptimer_reg[GPTIMER10_OFFSET+TIMER_LOAD_REG] << std::endl;
	
	close(mem_fd);*/
	
	sprintf(steering_filepath,"/dev/pwm10");
	if ((steering_fd = open(steering_filepath,O_RDWR)) < 0) {
		perror("Failed to open the bus for steering read.\n");
	}
	
	//ioctl(motor_fd, PWM_IOCTL_SET_FREQ, 256);

	char command[1];
	
	command[0] = 50;
	write(steering_fd,command,1);
}

void Steering::mech_command(char *value){
	write(steering_fd, value, 2);
}

void Steering::mech_control(){
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC ,&t);
	char value1[2];
	sprintf(value1,"90");
	char value2[2];
	sprintf(value2,"60");
	while(1){
		std::cout << "Motor flip" << std::endl;
		t.tv_sec++;
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
		mech_command(value1);
		t.tv_sec++;
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
		mech_command(value2);
	}
}

void Steering::handle_message(MESSAGE* message){
	switch(message->command){
		case STR_HARD_LEFT:
			std::cout << "hard left!" << std::endl;
			break;
		case STR_HARD_RIGHT:
			std::cout << "hard right!" << std::endl;
			break;
		case STR_SLIGHT_LEFT:
			std::cout << "slight left!" << std::endl;
			break;
		case STR_SLIGHT_RIGHT:
			std::cout << "slight right!" << std::endl;
			break;
		case STR_INCR_LEFT:
			std::cout << "inc left!" << std::endl;
			break;
		case STR_INCR_RIGHT:
			std::cout << "inc right!" << std::endl;
			break;
		case STR_STRAIGHT:
			std::cout << "inc straight!" << std::endl;
			break;
		case STR_SET_STEERING:
			break;
		case STR_DISABLE:
			break;
		case STR_ENABLE:
			break;
		case STR_SET_MIN_PRIO:
			break;
		default:
			std::cout << "Unknown command passed to steering subsystem! Command was : " << message->command << std::endl;
			break;
	}
}
