#include <iostream>
#include <time.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <linux/spi/spidev.h>
#include <string.h>

#include "shirtt.h"

#include "Compass.h"

#define PI 3.14159265359

#define LOG_TIMING

#define	COMPASS_ADDR	0x1E //0x21

#define NS_PER_MS	1000000
#define NS_PER_S	1000000000
#define DATA_COL_PERIOD_MS	20 //20

#define	HARD_TURN_THRESH	45
#define	SLIGHT_TURN_THRESH	20
#define STRAIGHT_THRESH		5

#define K_PROP_CONTROL		250 
#define CONTROL_OFFSET		15000
#define STR_CMD_LEN			6

#define SAT_LIMIT			10
#define	CONTROL_LOWER_LIMIT	10000
#define	CONTROL_UPPER_LIMIT	20000

#define COMPASS_DECLINE -78

//Compass class

Compass::Compass(ADC_DATA* adc_data_ptr) {
	subsys_name = COMPASS;
	subsys_num = SUBSYS_COMPASS;
	desired_heading = 0;
	adc_data = adc_data_ptr;
	msg[0].len = 2;
	msg[0].speed_hz = 500000;
	msg[0].delay_usecs = 0;
	msg[0].cs_change = 0;
	msg[0].pad = 0;	
	//init the collect/analysis sync semaphore.
	if(sem_init(&collect_analysis_sync, 0, 0) != 0){
		perror("Failed to init the compass collector/analysis sync sem \n");
	}
	subsys_priorities[SUBSYS_SONAR] = 0;	//sonar priority
	subsys_priorities[SUBSYS_GPS] = 1;		//gps priority
	subsys_priorities[NUM_SUBSYSTEMS] = 0; //console priority
	min_priority = NUM_SUBSYSTEMS;
	compass_avg=0;
	data_grab_count = 0;
}

void Compass::init_sensor() {
	char command[2];
	char read_buff[2];
	
	// SET UP SPI FOR ACCEL
	sprintf(accel_filepath,"/dev/spidev3.1"); //filepath for SPI device
	static uint8_t mode = SPI_MODE_0;
	//open the SPI device
	if ((accel_fd = open(accel_filepath,O_RDWR)) < 0) {
		perror("Failed to open the bus for accel read.\n");
	}
	//put SPI in write mode
	if(ioctl(accel_fd, SPI_IOC_WR_MODE, &mode) < 0){
		perror("Failed to set up SPI for write mode for accl setup");
	}
	//setup SPI for talking to the ADC
	command[0] = 0b01101000;
	command[1] = 0xFF;
	write(accel_fd,command,2);
	
	read(accel_fd,read_buff,2);
	
	if(ioctl(accel_fd, SPI_IOC_RD_MODE, &mode) < 0){
		perror("Failed to set up SPI for read mode for ACCEL setup");
	}
	
	//SET UP MAGNETOMETER
	/*struct timespec t;
	clock_gettime(CLOCK_MONOTONIC ,&t);
	t.tv_nsec+= 50*NS_PER_MS;
	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);*/
	sprintf(compass_filepath,"/dev/i2c-3");
	if ((compass_fd = open(compass_filepath,O_RDWR)) < 0) {
		perror("Failed to open the bus for compass read.\n");
	}
	/*clock_gettime(CLOCK_MONOTONIC ,&t);
	t.tv_nsec+= 50*NS_PER_MS;
	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);*/
	if (ioctl(compass_fd,I2C_SLAVE,COMPASS_ADDR) < 0) {
		perror("Failed to acquire bus access and/or talk to slave.\n");
	}
	/*clock_gettime(CLOCK_MONOTONIC ,&t);
	t.tv_nsec+= 50*NS_PER_MS;
	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);*/
	
	/*command[0] = 'r';
	command[1] = 0x08;
	write(compass_fd,command,2);
	
	read(compass_fd,command,1);
	std::cout << "op byte: " << (int)command[0] << std::endl;*/
	
	/*command[0] = 'w';
	command[1] = 0x08;
	command[2] = 0b01110010;*/
/*				   ||||||||- continuous mode lsb
				   |||||||-- continuous mode msb
				   ||||||--- 0
				   |||||---- 0
				   ||||----- periodic set/reset on
				   |||------ continuous mode measurement rate msb
				   ||------- continuous mode measurement rate lsb
				   |-------- 0
*/
	//put compass into continuous mode
	//command[0] = 0x3C;
	command[0] = 0x02;
	command[1] = 0x00;

	if(write(compass_fd,command,2) != 2){
		perror("Failed to set operational byte");
	}
	/*clock_gettime(CLOCK_MONOTONIC ,&t);
	t.tv_nsec+= 50*NS_PER_MS;
	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);*/
	
	//command[0] = 0x3C;
	command[0] = 0x00;
	command[1] = 0b00010000;//1100;

	if(write(compass_fd,command,2) != 2){
		perror("Failed to set operational byte");
	}
	/*clock_gettime(CLOCK_MONOTONIC ,&t);
	t.tv_nsec+= 50*NS_PER_MS;
	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);*/
}

/*
float Compass::correct_heading(int Bx, int By, int Bz, int Ax, int Ay, int Az) {
	/* roll pitch and yaw angles computed by iecompass */
/*	int16_t iPhi iThe iPsi; 
	/* magnetic field readings corrected for hard iron effects and PCB orientation */
/*	int16_t iBfx iBfy iBfz;
	/* hard iron estimate */
/*	int16_t iVx iVy iVz;
	
	int16_t iSin, iCos; /* sine and cosine */
	/* subtract the hard iron offset */
/*	Bx -= iVx; /* see Eq 16 */
/*	By -= iVy; /* see Eq 16 */
/*	Bz -= iVz; /* see Eq 16 */
	
	/* calculate current roll angle Phi */
/*	iPhi = atan2(iGpy, iGpz)*180/PI*100;/* Eq 13 */
	/* calculate sin and cosine of roll angle Phi */
/*	iSin = iTrig(iGpy, iGpz); /* Eq 13: sin = opposite / hypotenuse */
/*	iCos = iTrig(iGpz, iGpy); /* Eq 13: cos = adjacent / hypotenuse */
	/* de-rotate by roll angle Phi */
/*	iBfy = (Int16)((iBpy * iCos - iBpz * iSin) >> 15);/* Eq 19 y component */
/*	iBpz = (Int16)((iBpy * iSin + iBpz * iCos) >> 15);/* Bpy*sin(Phi)+Bpz*cos(Phi)*/
/*	iGpz = (Int16)((iGpy * iSin + iGpz * iCos) >> 15);/* Eq 15 denominator */
	/* calculate current pitch angle Theta */
/*	iThe = iHundredAtan2Deg((Int16)-iGpx, iGpz);/* Eq 15 */
	/* restrict pitch angle to range -90 to 90 degrees */
/*	if (iThe > 9000) iThe = (Int16) (18000 - iThe);
/*	if (iThe < -9000) iThe = (Int16) (-18000 - iThe);
	/* calculate sin and cosine of pitch angle Theta */
/*	iSin = (Int16)-iTrig(iGpx, iGpz); /* Eq 15: sin = opposite / hypotenuse */
/*	iCos = iTrig(iGpz, iGpx); /* Eq 15: cos = adjacent / hypotenuse */
	/* correct cosine if pitch not in range -90 to 90 degrees */
/*	if (iCos < 0) iCos = (Int16)-iCos;
	/* de-rotate by pitch angle Theta */
/*	iBfx = (Int16)((iBpx * iCos + iBpz * iSin) >> 15); /* Eq 19: x component */
/*	iBfz = (Int16)((-iBpx * iSin + iBpz * iCos) >> 15);/* Eq 19: z component */
	/* calculate current yaw = e-compass angle Psi */
/*	iPsi = iHundredAtan2Deg((Int16)-iBfy, iBfx); /* Eq 22 */
	
/*	return 0.0;
}
*/
float Compass::data_grab(){
//create tx and rx buffers for data trasmission 
	data_grab_count++;
	char tx_buf1[2];
	char rx_buf1[2];
	char tx_buf2[2];
	char rx_buf2[2];
	//set tx and rx buffers in the message
	msg[0].tx_buf=(uint64_t)tx_buf1;
	msg[0].rx_buf=(uint64_t)rx_buf1;
	//setup tx buffer to request data from ADC
	tx_buf1[0] = 0b01101000;
	tx_buf1[1] = 0xFF;
	#ifdef SONAR_DEBUG
		printf("tx buff: %x\n",(int)tx_buf1[0]);
	#endif
	//requestd data from the ADC. Response put in rx_buf
	if(ioctl(accel_fd, SPI_IOC_MESSAGE(1), msg) < 0) {
		perror("Error requesting data from ADC (SPI IOC MESSAGE failed)\n");
	}
	
	memcpy(adc_data->rx_buf_adc3,rx_buf1,2);
	
	//set tx and rx buffers in the message
	msg[0].tx_buf=(uint64_t)tx_buf2;
	msg[0].rx_buf=(uint64_t)rx_buf2;
	//setup tx buffer to request data from ADC
	tx_buf2[0] = 0b01111000;
	tx_buf2[1] = 0xFF;
	#ifdef COMPASS_DEBUG
		printf("tx buff: %x\n",(int)tx_buf2[0]);
	#endif
	//requestd data from the ADC. Response put in rx_buf
	if(ioctl(accel_fd, SPI_IOC_MESSAGE(1), msg) < 0) {
		perror("Error requesting data from ADC (SPI IOC MESSAGE failed)\n");
	}
	memcpy(adc_data->rx_buf_adc4,rx_buf2,2);
	
	//std::cout << "adc2 reading: " << ( (((int)(rx_buf2[0] & 0b00000011)) << 8) + ((int)(rx_buf2[1])) ) << std::endl;
	
	int Ax = (((int)(rx_buf1[0] & 0b00000011)) << 8) + ((int)(rx_buf1[1]));
	int Ay = (((int)(rx_buf2[0] & 0b00000011)) << 8) + ((int)(rx_buf2[1]));
	int Az = (((int)(adc_data->rx_buf_adc2[0] & 0b00000011)) << 8) + ((int)(adc_data->rx_buf_adc2[1]));
	
	char command[1];
	//command[0] = 0x3C;
	if(data_grab_count % 5 == 0){
		data_grab_count = 0;
		command[0] = 3;
		if(write(compass_fd,command,1) != 1){
			perror("Failed to request data");
		}
		
		unsigned char buff[13];
		int read_result;
		if((read_result = read(compass_fd,buff,6)) != 6) {
			if(errno == ETIMEDOUT){
				
			}else{
				perror("Failed to read data from compass");
			}
			std::cout << "Not reading 6" << std::endl;
		}
		x=(buff[0] << 8) | (buff[1]);
		y=(buff[2] << 8) | (buff[3]);
		z=(buff[4] << 8) | (buff[5]);
	}
	
	/*x = (x << 4) >> 4;
	y = (y << 4) >> 4;
	z = (z << 4) >> 4;*/
	
	
	/*
	x = (int16_t)(((int16_t)buff[0] << 8) | (int16_t)(buff[1]));
	y = (int16_t)(((int16_t)buff[2] << 8) | (int16_t)(buff[3]));
	z = (int16_t)(((int16_t)buff[4] << 8) | (int16_t)(buff[5]));
	*/
	/*if(x > 0x07FF) {
		x= ~x;
		x= (x+1);
	}
	if(y>0x07FF) {
		y= ~y;
		y= (y+1);
	}
	if(z>0x07FF) {
		z= ~z;
		z= (z+1);
	}*/

	
	/*if(write(compass_fd,command,1) != 1){
		perror("Failed to request data");
	}*/
	/*float ret = (float)(atan2((double)y,(double)x)*180.0/3.141592 + 180);
	ret = (ret>360) ? ret-360 : ret;*/
	//Dylan CHanges
	/*
	double xyzNorm=sqrt(x*x+y*y+z*z);
	double xB=x/xyzNorm;
	double yB=y/xyzNorm;
	double zB=z/xyzNorm;
	*/
	Vector3d adxl=Vector3d(-(Ax-480),-(Ay-500),-(Az-500));
	Vector3d magn=Vector3d(x,y,z);
	
	adxlA[compass_avgA]=adxl;
	magnA[compass_avg]=magn;
	compass_avgA=(compass_avgA+1)%COMPASS_AVERAGEA;
	compass_avg=(compass_avg+1)%COMPASS_AVERAGE;
	Vector3d avg_adxl;
	Vector3d avg_magn;
	for(int i=0;i<COMPASS_AVERAGEA;i++){
		avg_adxl+=adxlA[i];
	}
	for(int i=0;i<COMPASS_AVERAGE;i++){
		avg_magn+=magnA[i];
	}
	avg_adxl/=COMPASS_AVERAGEA;
	avg_magn/=COMPASS_AVERAGE;
	adxl=avg_adxl;
	magn=avg_magn;
	Vector3d U=magn.normalize().cross(adxl.normalize());
	Vector3d V=adxl.normalize().cross(U.normalize());
	x=V.X;
	y=V.Y;
	z=V.Z;

	//float ret = (float)(atan2((double)y,(double)x)*180.0/3.141592 + 180);
	float ret = (float)(atan2(V.X,V.Y)*180.0/3.141592);
	ret*=-1;
	ret+=COMPASS_DECLINE;
	if(ret>=360){ret-=360;}if(ret<0){ret+=360;}
	//std::cout << "heading B: \t" //<< Ax  << "\t" << Ay  << "\t" << Az 
	//<< adxl << "\t" << magn << V << " \t" << ret << std::endl;


	//double incline = 0;// asin(zB);
	//std::cout << "heading: Inc: "<< incline <<"\tNORM: "<< xyzNorm <<"\t" << xB << ",\t" << yB << ",\t" << zB << " \t" << ret << std::endl;
	return ret;
	//end dylan changes
	//return correct_heading(x, y, z, Ax, Ay, Az);

	//std::cout << "heading: " << head << std::endl;
	
	
	
	//return ((float)(((int)buff[0] << 8) + (int)buff[1]))/10.0;
}

void Compass::collector(){
	struct timespec t;
	#ifdef LOG_TIMING
		struct timespec timing;
	#endif
	while(1){
		t.tv_nsec+= DATA_COL_PERIOD_MS*NS_PER_MS;
		while(t.tv_nsec > NS_PER_S){
			t.tv_sec++;
			t.tv_nsec -= NS_PER_S;
		}
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
		#ifdef LOG_TIMING
			clock_gettime(CLOCK_MONOTONIC ,&timing);
			log_release_time(&timing, COLLECTOR);
		#endif
		if(enabled){
			meas_heading = data_grab();
			#ifdef COMPASS_DEBUG
				std::cout << "Compass Heading: " << meas_heading << std::endl;
			#endif
			sem_post(&collect_analysis_sync);
		}
		#ifdef LOG_TIMING
			clock_gettime(CLOCK_MONOTONIC ,&timing);
			log_end_time(&timing, COLLECTOR);
		#endif
	}
}

void Compass::analysis(){
	
	//message sent to the steering subsystem to change direction. Data will be set with steering command.
	MESSAGE steer = MESSAGE(subsys_num, SUBSYS_STEERING, STR_SET_STEERING);
	
	//the steering command
	char u[STR_CMD_LEN];
	
	#ifdef LOG_TIMING
		struct timespec timing;
	#endif
	
	while(1) {
		//wait for data
		sem_wait(&collect_analysis_sync);
		#ifdef LOG_TIMING
			clock_gettime(CLOCK_MONOTONIC ,&timing);
			log_release_time(&timing, ANALYSIS);
		#endif
		//------------------
		//---analyze data---
		//------------------
		//get the difference between the measured and desired heading
		int diff = (int)meas_heading - (int)desired_heading;
		//get diff range to be -180 to 180 degrees
		if(diff > 0) {
			diff = (diff>180) ? diff-360 : diff;
		}else{
			diff = (diff<-180) ? diff+360 : diff;
		}
		//calculate the steering command. (proprotional feedback controller)
		int command_u = (int)((diff) * K_PROP_CONTROL) + CONTROL_OFFSET;
		//impose saturation limits on the steering command
		if(command_u > CONTROL_UPPER_LIMIT){
			command_u = CONTROL_UPPER_LIMIT;
		}else if(command_u < CONTROL_LOWER_LIMIT){
			command_u = CONTROL_LOWER_LIMIT;
		}
		//steering subsystem expects commands as char*
		sprintf(u,"%d",command_u);
		u[STR_CMD_LEN-1] = '\0';
		steer.data = ((void*)(u));
		//send command to steering subsystem
		send_sys_message(&steer);
		#ifdef LOG_TIMING
			clock_gettime(CLOCK_MONOTONIC ,&timing);
			log_end_time(&timing, ANALYSIS);
		#endif
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
	if(subsys_priorities[message->from] <= min_priority){
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
			case CPS_SET_MIN_PRIO:
				min_priority = subsys_priorities[message->from];
				break;
			case CPS_RESET_MIN_PRIO:
				min_priority = NUM_SUBSYSTEMS;
				break;
			default:
				std::cout << "Unknown command passed to compass subsystem! Command was : " << message->command << std::endl;
				std::cout << "From: " << message->from << std::endl;
				break;
		}
	}
}
