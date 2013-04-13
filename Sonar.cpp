#include <iostream>
#include <time.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
 #include <stdint.h>
 #include <linux/spi/spidev.h>

#include "Sonar.h"

#define SONAR_DEBUG


#define	SONAR_ADDR	0x21

#define NS_PER_MS	1000000
#define NS_PER_S	1000000000
#define DATA_COL_PERIOD_MS	50

#define	HARD_TURN_THRESH	45
#define	SLIGHT_TURN_THRESH	20
#define STRAIGHT_THRESH		5


//Sonar class

Sonar::Sonar() {
	subsys_name = SONAR;
	subsys_num = SUBSYS_SONAR;
	if(sem_init(&collect_analysis_sync, 0, 0) != 0){
		perror("Failed to init the sonar collector/analysis sync sem \n");
	}
}

void Sonar::init_sensor() {
	sprintf(sonar_filepath,"/dev/spidev4.0");
	static uint8_t mode = SPI_MODE_0;
	if ((sonar_fd = open(sonar_filepath,O_RDWR)) < 0) {
		perror("Failed to open the bus for sonar read.\n");
	}
	if(ioctl(sonar_fd, SPI_IOC_WR_MODE, &mode) < 0){
		perror("Failed to set up SPI for write mode for sonar setup");
	}
	char command[2];
	char read_buff[2];
	
	command[0] = 0b01101000;
	command[1] = 0xFF;
	write(sonar_fd,command,2);
	
	read(sonar_fd,read_buff,2);
	
	if(ioctl(sonar_fd, SPI_IOC_RD_MODE, &mode) < 0){
		perror("Failed to set up SPI for read mode for sonar setup");
	}
	
}

float Sonar::data_grab(){
	char tx_buf[2];
	char rx_buf[2];
	struct spi_ioc_transfer msg[1]; 
		msg[0].tx_buf=(uint64_t)tx_buf;
		msg[0].rx_buf=(uint64_t)rx_buf;
		msg[0].len = 2;
		msg[0].speed_hz = 500000;
		msg[0].delay_usecs = 0;
		msg[0].cs_change = 0;
		msg[0].pad = 0;
	
	tx_buf[0] = 0b01101000;
	tx_buf[1] = 0xFF;
	#ifdef SONAR_DEBUG
		printf("tx buff: %x\n",(int)tx_buf[0]);
	#endif
	if(ioctl(sonar_fd, SPI_IOC_MESSAGE(1), msg) < 0) {
		perror("Error requesting data from ADC (SPI IOC MESSAGE failed)\n");
	}
	#ifdef SONAR_DEBUG
		std::cout << "sonar reading: " << ( (((int)(rx_buf[0] & 0b00000011)) << 8) + ((int)(rx_buf[1])) ) << std::endl;
	#endif
	return 0.0;
}

void Sonar::collector(){
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
			sonar_reading = data_grab();
			#ifdef SONAR_DEBUG
				std::cout << "Sonar Reading: " << sonar_reading << std::endl;
			#endif
			sem_post(&collect_analysis_sync);
		}
	}
}

void Sonar::analysis(){
	while(1) {
		//wait for data
		sem_wait(&collect_analysis_sync);
		//analyze data
	}
}

void* Sonar::read_data(int command) {
	switch(command){
		default:
			std::cout << "Unknown command passed to sonar subsystem for reading data! Command was : " << command << std::endl;
			return NULL;
			break;
	}
}

void Sonar::handle_message(MESSAGE* message){
	switch(message->command){
		case SNR_SET_DIST_THR:
			break;
		case SNR_DISABLE:
			enabled=0;
			break;
		case SNR_ENABLE:
			enabled=1;
			break;
		case SNR_GET_READING:
			std::cout << "Sonar Reading: " << sonar_reading << std::endl;
			break;
		default:
			std::cout << "Unknown command passed to soar subsystem! Command was : " << message->command << std::endl;
			break;
	}
}
