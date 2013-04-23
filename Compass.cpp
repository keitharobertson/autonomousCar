#include <iostream>
#include <time.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "Compass.h"


#define	COMPASS_ADDR	0x21

#define NS_PER_MS	1000000
#define NS_PER_S	1000000000
#define DATA_COL_PERIOD_MS	50

#define	HARD_TURN_THRESH	45
#define	SLIGHT_TURN_THRESH	20
#define STRAIGHT_THRESH		5

#define K_PROP_CONTROL		500
#define CONTROL_OFFSET		15000
#define STR_CMD_LEN			6

#define SAT_LIMIT			10
#define	CONTROL_LOWER_LIMIT	10000
#define	CONTROL_UPPER_LIMIT	20000

//Compass class

Compass::Compass() {
	subsys_name = COMPASS;
	subsys_num = SUBSYS_COMPASS;
	desired_heading = 0;
	if(sem_init(&collect_analysis_sync, 0, 0) != 0){
		perror("Failed to init the compass collector/analysis sync sem \n");
	}
}

void Compass::init_sensor() {
	sprintf(compass_filepath,"/dev/i2c-3");
	if ((compass_fd = open(compass_filepath,O_RDWR)) < 0) {
		perror("Failed to open the bus for compass read.\n");
	}
	if (ioctl(compass_fd,I2C_SLAVE,COMPASS_ADDR) < 0) {
		perror("Failed to acquire bus access and/or talk to slave.\n");
	}
	char command[3];
	
	command[0] = 'r';
	command[1] = 0x08;
	write(compass_fd,command,2);
	
	read(compass_fd,command,1);
	std::cout << "op byte: " << (int)command[0] << std::endl;
	
	command[0] = 'w';
	command[1] = 0x08;
	command[2] = 0b01110010;
/*				   ||||||||- continuous mode lsb
				   |||||||-- continuous mode msb
				   ||||||--- 0
				   |||||---- 0
				   ||||----- periodic set/reset on
				   |||------ continuous mode measurement rate msb
				   ||------- continuous mode measurement rate lsb
				   |-------- 0
*/
	if(write(compass_fd,command,3) != 3){
		perror("Failed to set operational byte");
	}
}

float Compass::data_grab(){
	char buff[2];
	if(read(compass_fd,buff,2) != 2) {
		perror("Failed to read data from compass");
	}
	return ((float)(((int)buff[0] << 8) + (int)buff[1]))/10.0;
}

void Compass::collector(){
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC ,&t);
	while(1){
		t.tv_nsec+= DATA_COL_PERIOD_MS*NS_PER_MS;
		while(t.tv_nsec > NS_PER_S){
			t.tv_sec++;
			t.tv_nsec -= NS_PER_S;
		}
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
		if(enabled){
			meas_heading = data_grab();
			#ifdef COMPASS_DEBUG
				std::cout << "Compass Heading: " << meas_heading << std::endl;
			#endif
			sem_post(&collect_analysis_sync);
		}
	}
}

void Compass::analysis(){
	
	hard_right = MESSAGE(subsys_num, SUBSYS_STEERING, STR_HARD_RIGHT);
	slight_right = MESSAGE(subsys_num, SUBSYS_STEERING, STR_SLIGHT_RIGHT);
	
	straight = MESSAGE(subsys_num, SUBSYS_STEERING, STR_STRAIGHT);
	
	hard_left = MESSAGE(subsys_num, SUBSYS_STEERING, STR_HARD_LEFT);
	slight_left = MESSAGE(subsys_num, SUBSYS_STEERING, STR_SLIGHT_LEFT);
	
	MESSAGE steer = MESSAGE(subsys_num, SUBSYS_STEERING, STR_SET_STEERING);
	
	char u[STR_CMD_LEN];
	
	while(1) {
		//wait for data
		sem_wait(&collect_analysis_sync);
		//analyze data
		int diff = (int)meas_heading - (int)desired_heading;
		if(diff > 0) {
			diff = (diff>180) ? diff-360 : diff;
		}else{
			diff = (diff<-180) ? diff+360 : diff;
		}
		
		if(diff > SAT_LIMIT){
			diff = SAT_LIMIT;
		}else if(diff < -1*SAT_LIMIT) {
			diff = -1*SAT_LIMIT;
		}
		int command_u = (int)((diff) * K_PROP_CONTROL) + CONTROL_OFFSET;
		if(command_u > CONTROL_UPPER_LIMIT){
			command_u = CONTROL_UPPER_LIMIT;
		}else if(command_u < CONTROL_LOWER_LIMIT){
			command_u = CONTROL_LOWER_LIMIT;
		}
		
		sprintf(u,"%d",command_u);
		u[STR_CMD_LEN-1] = '\0';
		steer.data = ((void*)(u));
		send_sys_message(&steer);
		
		/*if(diff < 0){diff+=360;}
		if(diff<180){
			//turn right
			if(diff < STRAIGHT_THRESH){
				send_sys_message(&straight);
			}else if(diff > HARD_TURN_THRESH) {
				send_sys_message(&hard_right);
			}else {
				send_sys_message(&slight_right);
			}
		}else{
			//turn left
			diff = 360 - diff;
			if(diff < STRAIGHT_THRESH){
				send_sys_message(&straight);
			}else if(diff > HARD_TURN_THRESH) {
				send_sys_message(&hard_left);
			}else {
				send_sys_message(&slight_left);
			}
		}*/
	}
}

void* Compass::read_data(int command) {
	switch(command){
		case CPS_SET_HEADING:
			float data;
			std::cin >> data;
			return *((void**)(&data));
		case CPS_LEFT_90:
		case CPS_RIGHT_90:
		case CPS_180:
		case CPS_DISABLE:
		case CPS_ENABLE:
			return NULL;
			break;
		default:
			std::cout << "Unknown command passed to compass subsystem for reading data! Command was : " << command << std::endl;
			return NULL;
			break;
	}
}

void Compass::handle_message(MESSAGE* message){
	MESSAGE data_request = MESSAGE(subsys_num, 0, 0);
	switch(message->command){
		case CPS_SET_HEADING:
			desired_heading = (*(float*)&message->data);
			break;
		case CPS_LEFT_90:
			desired_heading -= 90;
			if(desired_heading<0){desired_heading+=360;}
			break;
		case CPS_RIGHT_90:
			desired_heading += 90;
			if(desired_heading>360){desired_heading-=360;}
			break;
		case CPS_180:
			desired_heading += 180;
			if(desired_heading>360){desired_heading-=360;}
			break;
		case CPS_DISABLE:
			enabled = 0;
			break;
		case CPS_ENABLE:
			enabled = 1;
			break;
		case CPS_GET_READING:
			std::cout << "Compass reading: " << meas_heading << std::endl;
			break;
		case CPS_RETURN_DES_HEADING:
			data_request.to = message->from;
			data_request.command = CPS_RET_DES_HEADING;
			data_request.data = *((void**)(&desired_heading));
			send_sys_message(&data_request);
			break;
		default:
			std::cout << "Unknown command passed to compass subsystem! Command was : " << message->command << std::endl;
			break;
	}
}
