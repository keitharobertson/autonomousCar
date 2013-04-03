#include <iostream>
#include <time.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

#include "Compass.h"


#define	COMPASS_ADDR	0x21

#define NS_PER_MS	1000000
#define NS_PER_S	1000000000
#define DATA_COL_PERIOD_MS	50


//Compass class

Compass::Compass() {
	subsys_name = COMPASS;
	subsys_num = SUBSYS_COMPASS;
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
	float heading;
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC ,&t);
	while(1){
		t.tv_nsec+= DATA_COL_PERIOD_MS*NS_PER_MS;
		while(t.tv_nsec > NS_PER_S){
			t.tv_sec++;
			t.tv_nsec -= NS_PER_S;
		}
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
		heading = data_grab();
		std::cout << "Compass Heading: " << heading << std::endl;
	}
}

void Compass::analysis(){
	struct timespec t;
	send_sys_message((char *)std::string("hello from compass analysis").c_str());
	clock_gettime(CLOCK_MONOTONIC ,&t);
	t.tv_sec++;
	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
	send_sys_message((char *)std::string("hello from compass analysis 1 second later").c_str());
}

void Compass::handle_message(char* message){
	std::cout << "Compass Recieved Message:" << std::endl;
	std::cout << message <<std::endl << std::endl;
}
