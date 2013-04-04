#include <iostream>
#include <time.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

#include "Motor.h"

#define PWM_IOCTL_SET_FREQ         (1)  

Motor::Motor(){
	subsys_name = MOTOR;
	subsys_num = SUBSYS_MOTOR;
}

Motor::~Motor() {
	close(motor_fd);
}
		
void Motor::init_device(){
	sprintf(motor_filepath,"/dev/pwm10");
	if ((motor_fd = open(motor_filepath,O_RDWR)) < 0) {
		perror("Failed to open the bus for compass read.\n");
	}
	
	ioctl(motor_fd, PWM_IOCTL_SET_FREQ, 256);

	char command[1];
	
	command[0] = 50;
	write(motor_fd,command,1);
}

void Motor::mech_command(char value){
	write(motor_fd, &value, 1);
}

void Motor::mech_control(){
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC ,&t);
	while(1){
		std::cout << "Motor flip" << std::endl;
		t.tv_sec++;
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
		mech_command(100);
		t.tv_sec++;
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
		mech_command(0);
	}
}

void Motor::handle_message(char* message){
	
}
