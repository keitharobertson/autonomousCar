#include <iostream>
#include <time.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

#include "GPS.h"

#define NS_PER_MS	1000000
#define NS_PER_S	1000000000
#define DATA_COL_PERIOD_MS	250


//Compass class

GPS::GPS() {
	subsys_name = GPS_NAME;
	subsys_num = SUBSYS_GPS;
}

GPS::~GPS(){
	close(gps_fd);
}

void GPS::init_sensor() {
	sprintf(gps_filepath,"/dev/ttyO2");
	if ((gps_fd = open(gps_filepath,O_RDWR)) < 0) {
		perror("Failed to open the bus for GPS read.\n");
	}
}

float GPS::data_grab(){
	char buff[2];
	if(read(gps_fd,buff,2) != 2) {
		perror("Failed to read data from compass");
	}
	return ((float)(((int)buff[0] << 8) + (int)buff[1]));
}

void GPS::collector(){
	float gps_reading;
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC ,&t);
	while(1){
		t.tv_nsec+= DATA_COL_PERIOD_MS*NS_PER_MS;
		while(t.tv_nsec > NS_PER_S){
			t.tv_sec++;
			t.tv_nsec -= NS_PER_S;
		}
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
		gps_reading = data_grab();
		std::cout << "GPS Heading: " << gps_reading << std::endl;
	}
}

void GPS::analysis(){
	
}

void GPS::handle_message(MESSAGE* message){
	std::cout << "GPS Recieved Message:" << std::endl;
	std::cout << message <<std::endl << std::endl;
}
