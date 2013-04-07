#include <iostream>
#include <time.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
 #include <sys/mman.h>

#include "Motor.h"

#define PWM_IOCTL_SET_FREQ         (1) 
									
#define REG_BASE				0x48000000
#define GPTIMER10_OFFSET		0x86000
#define TIMER_LOAD_REG			0x02c


Motor::Motor(){
	subsys_name = MOTOR;
	subsys_num = SUBSYS_MOTOR;
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
	
	sprintf(motor_filepath,"/dev/pwm11");
	if ((motor_fd = open(motor_filepath,O_RDWR)) < 0) {
		perror("Failed to open the bus for compass read.\n");
	}
	
	//ioctl(motor_fd, PWM_IOCTL_SET_FREQ, 256);

	char command[2];
	sprintf(command,"50");
	write(motor_fd,command,2);
}

void Motor::mech_command(char *value){
	write(motor_fd, value, 2);
}

void Motor::mech_control(){
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC ,&t);
	char value1[2];
	sprintf(value1,"90");
	char value2[2];
	sprintf(value2,"60");
	while(1){
		//std::cout << "Motor flip" << std::endl;
		t.tv_sec++;
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
		mech_command(value1);
		t.tv_sec++;
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
		mech_command(value2);
	}
}

void Motor::handle_message(MESSAGE* message){
	
}
