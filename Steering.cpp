#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
 #include <sys/mman.h>
 #include <string.h>

#include "Steering.h"
									
#define REG_BASE				0x48000000
#define GPTIMER10_OFFSET		0x86000
#define TIMER_LOAD_REG			0x02c

#define HARD_LEFT		"90"
#define HARD_RIGHT		"10"
#define SLIGHT_LEFT		"75"
#define SLIGHT_RIGHT	"25"
#define FINE_LEFT		"60"
#define FINE_RIGHT		"40"
#define	STRAIGHT		"50"


Steering::Steering(){
	subsys_name = STEERING;
	subsys_num = SUBSYS_STEERING;
	if(sem_init(&steer_cmd_ctrl, 0, 0) != 0){
		perror("Failed to init the steering subsys command / mech control sync sem \n");
	}
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

	const char* command = STRAIGHT;
	write(steering_fd,command,2);
}

void Steering::mech_command(char *value){
	write(steering_fd, value, 2);
}

void Steering::mech_control(){
	while(1){
		sem_wait(&steer_cmd_ctrl);
		if(enabled){
			mech_command(steering_duty_cycle);
		}
	}
}

void Steering::set_new_pwm_duty_cycle(const char* value){
	memcpy(steering_duty_cycle,value,2);
	sem_post(&steer_cmd_ctrl);
}

void Steering::handle_message(MESSAGE* message){
	switch(message->command){
		case STR_HARD_LEFT:
			set_new_pwm_duty_cycle(HARD_LEFT);
			#ifdef STR_DEBUG
				std::cout << "hard left!" << std::endl;
			#endif
			break;
		case STR_HARD_RIGHT:
			set_new_pwm_duty_cycle(HARD_RIGHT);
			#ifdef STR_DEBUG
				std::cout << "hard right!" << std::endl;
			#endif
			break;
		case STR_SLIGHT_LEFT:
			set_new_pwm_duty_cycle(SLIGHT_LEFT);
			#ifdef STR_DEBUG
				std::cout << "slight left!" << std::endl;
			#endif
			break;
		case STR_SLIGHT_RIGHT:
			set_new_pwm_duty_cycle(SLIGHT_RIGHT);
			#ifdef STR_DEBUG
				std::cout << "slight right!" << std::endl;
			#endif
			break;
		case STR_FINE_LEFT:
			set_new_pwm_duty_cycle(FINE_LEFT);
			#ifdef STR_DEBUG
				std::cout << "fine left!" << std::endl;
			#endif
			break;
		case STR_FINE_RIGHT:
			set_new_pwm_duty_cycle(FINE_RIGHT);
			#ifdef STR_DEBUG
				std::cout << "fine right!" << std::endl;
			#endif
			break;
		case STR_STRAIGHT:
			set_new_pwm_duty_cycle(STRAIGHT);
			#ifdef STR_DEBUG
				std::cout << "straight!" << std::endl;
			#endif
			break;
		case STR_SET_STEERING:
			set_new_pwm_duty_cycle((const char*)message->data);
			break;
		case STR_DISABLE:
			set_new_pwm_duty_cycle(STRAIGHT);
			enabled = 0;
			break;
		case STR_ENABLE:
			enabled = 1;
			break;
		case STR_SET_MIN_PRIO:
			min_priority = (int)message->data;
			break;
		default:
			std::cout << "Unknown command passed to steering subsystem! Command was : " << message->command << std::endl;
			break;
	}
}