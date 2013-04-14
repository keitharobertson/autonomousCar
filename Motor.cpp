#include <iostream>
#include <time.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>

#include "Motor.h"

#define MOTOR_DEBUG

#define PWM_IOCTL_SET_FREQ         (1) 
									
#define REG_BASE				0x48000000
#define GPTIMER10_OFFSET		0x86000
#define TIMER_LOAD_REG			0x02c

#define FORWARD_FAST	"90"
#define FORWARD_SLOW	"55"
#define FORWARD_MID		"75"
#define BACKWARD_MID	"25"
#define BACKWARD_FAST	"5"
#define BACKWARD_SLOW	"45"
#define SPEED_STOP		"50"

#define DIR_FORWARD		1
#define DIR_BACKWARD	0


Motor::Motor(){
	subsys_name = MOTOR;
	subsys_num = SUBSYS_MOTOR;
	if(sem_init(&motor_cmd_ctrl, 0, 0) != 0){
		perror("Failed to init the motor subsys command / mech control sync sem \n");
	}
}

Motor::~Motor() {
	close(motor_fd);
}
		
void Motor::init_device(){
	
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
	
	sprintf(motor_filepath,"/dev/pwm9");
	if ((motor_fd = open(motor_filepath,O_RDWR)) < 0) {
		perror("Failed to open the bus for motor.\n");
	}
	
	//ioctl(motor_fd, PWM_IOCTL_SET_FREQ, 256);

	/*char command[2];
	sprintf(command,"50");
	write(motor_fd,command,2);*/
	direction = 1;
	set_new_pwm_duty_cycle(SPEED_STOP);
}

void Motor::mech_command(char *value){
	#ifdef MOTOR_DEBUG
		std::cout << "mech command. Duty cycle: " << value[0] << value[1] << std::endl;
	#endif
	write(motor_fd, value, 2);
}

void Motor::mech_control(){
	while(1){
		sem_wait(&motor_cmd_ctrl);
		#ifdef MOTOR_DEBUG
			std::cout << "issuing motor command. Duty cycle: " << motor_duty_cycle[0] << motor_duty_cycle[1] << std::endl;
		#endif
		if(enabled){
			mech_command(motor_duty_cycle);
		}
	}
}

void Motor::set_new_pwm_duty_cycle(const char* value){
	memcpy(motor_duty_cycle,value,2);
	sem_post(&motor_cmd_ctrl);
}

void Motor::handle_message(MESSAGE* message){
	switch(message->command) {
		case MOT_FAST:
			#ifdef MOTOR_DEBUG
				std::cout << "Setting motor fast!" << std::endl;
			#endif
			set_new_pwm_duty_cycle((direction==1) ? FORWARD_FAST : BACKWARD_FAST);
			break;
		case MOT_SLOW:
			set_new_pwm_duty_cycle((direction==1) ? FORWARD_SLOW : BACKWARD_SLOW);
			break;
		case MOT_STOP:
			set_new_pwm_duty_cycle(SPEED_STOP);
			break;
		case MOT_MID:
			set_new_pwm_duty_cycle((direction==1) ? FORWARD_MID : BACKWARD_MID);
			break;
		case MOT_DIRECTION:
		
			break;
		case MOT_SET_SPEED:
			
			break;
		case MOT_DISABLE:
			enabled = 0;
			break;
		case MOT_ENABLE:
			enabled = 1;
			break;
		case MOT_SET_MIN_PRIO:
			break;
		default:
			break;
	}
}
