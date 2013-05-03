#include <string.h>

#include "Sonar.h"


#define	SONAR_ADDR	0x21

#define NS_PER_MS	1000000
#define NS_PER_S	1000000000
#define DATA_COL_PERIOD_MS	50
#define AVOIDANCE_TIME_SEC	1

#define	DEFAULT_TURN_THRESH		65.0
#define	DEFAULT_REVERSE_THRESH	36.0

#define MV_PER_INCH		6.4

//Task Spawning ( called from Sonar::init() )
static void* reset_heading_task(void* c) {
	setup_rt_task(10);
	Sonar* s = (Sonar*)c;
	s->reset_heading();
}

//Sonar class Implementation

Sonar::Sonar(ADC_DATA* adc_data_ptr) {
	subsys_name = SONAR;
	subsys_num = SUBSYS_SONAR;
	//setup subsystem variables.
	avoidance_mode = false;//avoidance mode not active
	turn_threshold = DEFAULT_TURN_THRESH;//set default turn threshold
	reverse_threshold = DEFAULT_REVERSE_THRESH;//set default reverse threshold
	print_data = false;//do not print out data by default
	adc_data = adc_data_ptr;
	//setup parameters for SPI data requests from the ADC
	msg[0].len = 2;
	msg[0].speed_hz = 500000;
	msg[0].delay_usecs = 0;
	msg[0].cs_change = 0;
	msg[0].pad = 0;	
	//setup collector/analysis task sync semaphore
	if(sem_init(&collect_analysis_sync, 0, 0) != 0){
		perror("Failed to init the sonar collector/analysis sync sem \n");
	}
	if(sem_init(&avoid_reset_control, 0, 0) != 0){
		perror("Failed to init the sonar avoid/reset control sem \n");
	}
}

void Sonar::init_sensor() {
	sprintf(sonar_filepath,"/dev/spidev3.0"); //filepath for SPI device
	static uint8_t mode = SPI_MODE_0;
	//open the SPI device
	if ((sonar_fd = open(sonar_filepath,O_RDWR)) < 0) {
		perror("Failed to open the bus for sonar read.\n");
	}
	//put SPI in write mode
	if(ioctl(sonar_fd, SPI_IOC_WR_MODE, &mode) < 0){
		perror("Failed to set up SPI for write mode for sonar setup");
	}
	char command[2];
	char read_buff[2];
	//setup SPI for talking to the ADC
	command[0] = 0b01101000;
	command[1] = 0xFF;
	write(sonar_fd,command,2);
	
	read(sonar_fd,read_buff,2);
	
	if(ioctl(sonar_fd, SPI_IOC_RD_MODE, &mode) < 0){
		perror("Failed to set up SPI for read mode for sonar setup");
	}
	
}

float Sonar::data_grab(){
	//create tx and rx buffers for data trasmission 
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
	if(ioctl(sonar_fd, SPI_IOC_MESSAGE(1), msg) < 0) {
		perror("Error requesting data from ADC (SPI IOC MESSAGE failed)\n");
	}
	
	memcpy(adc_data->rx_buf_adc1,rx_buf1,2);
	
	//set tx and rx buffers in the message
	msg[0].tx_buf=(uint64_t)tx_buf2;
	msg[0].rx_buf=(uint64_t)rx_buf2;
	//setup tx buffer to request data from ADC
	tx_buf2[0] = 0b01111000;
	tx_buf2[1] = 0xFF;
	#ifdef SONAR_DEBUG
		printf("tx buff: %x\n",(int)tx_buf2[0]);
	#endif
	//requestd data from the ADC. Response put in rx_buf
	if(ioctl(sonar_fd, SPI_IOC_MESSAGE(1), msg) < 0) {
		perror("Error requesting data from ADC (SPI IOC MESSAGE failed)\n");
	}
	memcpy(adc_data->rx_buf_adc2,rx_buf2,2);
	
	std::cout << "adc2 reading: " << ( (((int)(rx_buf2[0] & 0b00000011)) << 8) + ((int)(rx_buf2[1])) ) << std::endl;
	
	//shift and mask out the 10 bit ADC reading
	int reading = ( (((int)(rx_buf1[0] & 0b00000011)) << 8) + ((int)(rx_buf1[1])) );
	//convert reading to a distance in inches
	float distance = (float)reading / 1023.0 * 3300.0 / MV_PER_INCH;
	
	return distance;
}

void Sonar::collector(){
	double reading_sum = 0;
	int count = 0;
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
			reading_sum += data_grab();
			count++;
			if(count%5==0){
				sonar_reading = reading_sum/5.0;
				reading_sum = 0.0;
				count = 0;
				#ifdef SONAR_DEBUG
					std::cout << "Sonar Reading: " << sonar_reading << std::endl;
				#endif
				//if printing of sonar data is enabled, print out the reading
				if(print_data) {
					std::cout << "Sonar Distance (in): " << sonar_reading << std::endl;
				}
				sem_post(&collect_analysis_sync);
			}
		}
	}
}

void Sonar::reset_heading() {
	#ifdef SONAR_DEBUG
		std::cout << "Sonar reset heading started. Waiting " << AVOIDANCE_TIME_SEC << " seconds..." << std::endl;
	#endif
	
	#ifdef SONAR_DEBUG
		std::cout << "Sonar is resetting compass heading. Obstacle avoidance complete." << std::endl;
	#endif
	//enable compass
	en_or_dis_subsys = MESSAGE(SUBSYS_SONAR,SUBSYS_COMPASS,CPS_ENABLE);
	send_sys_message(&en_or_dis_subsys);
	//go forward
	change_direction = MESSAGE(SUBSYS_SONAR,SUBSYS_MOTOR,MOT_DIRECTION,(void*)1); //forward!
	send_sys_message(&change_direction);
	//reset compass heading
	inter_subsys_command = MESSAGE(SUBSYS_SONAR,SUBSYS_COMPASS,CPS_SET_HEADING,*((void**)(&old_compass_heading)));
	send_sys_message(&inter_subsys_command);
	//reset compass priority
	set_cps_prio = MESSAGE(SUBSYS_SONAR, SUBSYS_COMPASS, CPS_RESET_MIN_PRIO);
	send_sys_message(&set_cps_prio);
	//reset motor speed
	change_speed = MESSAGE(SUBSYS_SONAR,SUBSYS_MOTOR,MOT_SET_SPEED,(void*)old_motor_speed);
	send_sys_message(&change_speed);
}

void Sonar::reverse_direction() {
	enabled=0;
	setup_avoidance();
	sem_wait(&avoid_reset_control);
	//disable compass (want to go straight back)
	en_or_dis_subsys = MESSAGE(SUBSYS_SONAR,SUBSYS_COMPASS,CPS_DISABLE);
	send_sys_message(&en_or_dis_subsys);
	//steer straight
	if(last_reverse_left){
		std::cout << "steering hard right" << std::endl;
		last_reverse_left = false;
		steer_straight = MESSAGE(SUBSYS_SONAR,SUBSYS_STEERING,STR_HARD_RIGHT);
	}else{
		std::cout << "steering hard left" << std::endl;
		last_reverse_left = true;
		steer_straight = MESSAGE(SUBSYS_SONAR,SUBSYS_STEERING,STR_HARD_LEFT);
	}
	send_sys_message(&steer_straight);
	//go backwards
	change_direction = MESSAGE(SUBSYS_SONAR,SUBSYS_MOTOR,MOT_DIRECTION,(void*)0); //reverse!
	send_sys_message(&change_direction);
	//run for 20 seconds
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC ,&t);
	t.tv_sec += 10;
	/*while(t.tv_nsec > NS_PER_S){
		t.tv_sec++;
		t.tv_nsec -= NS_PER_S;
	}*/
	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
	//change_speed = MESSAGE(SUBSYS_SONAR,SUBSYS_MOTOR,MOT_SLOW);
	//send_sys_message(&change_speed);
	enabled=1;
}
void Sonar::setup_avoidance() {
	//request current desired compass heading
	request_compass_data = MESSAGE(SUBSYS_SONAR, SUBSYS_COMPASS, CPS_RETURN_DES_HEADING); //request current compass heading
	send_sys_message(&request_compass_data);
	//request current motor speed
	request_motor_data = MESSAGE(SUBSYS_SONAR, SUBSYS_MOTOR, MOT_RET_SPEED); //request current motor speed so it can be reset later
	send_sys_message(&request_motor_data);
	//disable lower priority subsystems from changing compass heading
	set_cps_prio = MESSAGE(SUBSYS_SONAR, SUBSYS_COMPASS, CPS_SET_MIN_PRIO); //set compass min priority to prevent other subsystems from overriding the avoidance
	send_sys_message(&set_cps_prio);
}

void Sonar::avoid_obstacle() {
	enabled = 0;
	//now in avoidance mode
	#ifdef SONAR_DEBUG
		std::cout << "Sonar is avoiding an obstacle!" << std::endl;
	#endif
	
	setup_avoidance();
	sem_wait(&avoid_reset_control);
	//slow down the motor
	change_speed = MESSAGE(SUBSYS_SONAR,SUBSYS_MOTOR,MOT_SLOW);
	send_sys_message(&change_speed);
	//turn left 90 degrees
	inter_subsys_command = MESSAGE(SUBSYS_SONAR,SUBSYS_COMPASS,CPS_LEFT_90);
	send_sys_message(&inter_subsys_command);
	
	//re-enable sonar
	enabled = 1;
}

void Sonar::analysis(){
	struct timespec t;
	char mot_speed_rev[6];
	int mot_speed_int = 15500;
	while(1) {
		//wait for data
		sem_wait(&collect_analysis_sync);
		//analyze data
		if(sonar_reading < reverse_threshold) {
			avoidance_mode = false;
			if(!reverse_mode){
				reverse_mode = true;
				if(print_data) {
					std::cout << "Obstacle was detected! Sonar avoidance activated! Reversing!" << std::endl;
				}
				#ifdef SONAR_DEBUG
					std::cout << "Obstacle was detected! Sonar avoidance activated! Reversing!" << std::endl;
				#endif
				reverse_direction();
			}else{
				//reverse (and stop)
				change_direction = MESSAGE(SUBSYS_SONAR,SUBSYS_MOTOR,MOT_DIRECTION,(void*)0); //reverse!
				send_sys_message(&change_direction);
				/*clock_gettime(CLOCK_MONOTONIC ,&t);
				t.tv_nsec+= 500*NS_PER_MS;
				while(t.tv_nsec > NS_PER_S){
					t.tv_sec++;
					t.tv_nsec -= NS_PER_S;
				}*/
				//clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
				//slow down the motor
				mot_speed_int = (mot_speed_int<18000) ? mot_speed_int+85: 18000;
				sprintf(mot_speed_rev,"%d",mot_speed_int);
				change_speed = MESSAGE(SUBSYS_SONAR,SUBSYS_MOTOR,MOT_SLOW);
				change_speed = MESSAGE(SUBSYS_SONAR,SUBSYS_MOTOR,MOT_SET_SPEED,(void*)mot_speed_rev);
				send_sys_message(&change_speed);
			}
		}else if (sonar_reading < turn_threshold) {
			if(reverse_mode) {
				reverse_mode = false;
				if(print_data) {
					std::cout << "Resetting heading and motor. reverse complete." << std::endl;
				}
				reset_heading();
			}
			if(!avoidance_mode){
				avoidance_mode = true;
				if(print_data) {
					std::cout << "Obstacle was detected! Sonar avoidance activated! Turning!" << std::endl;
				}
				#ifdef SONAR_DEBUG
					std::cout << "Obstacle was detected! Sonar avoidance activated! Turning!" << std::endl;
				#endif
				avoid_obstacle();
			}
		}else{
			mot_speed_int = 15500;
			#ifdef SONAR_DEBUG
				std::cout << "No obstacles detected by sonar! No avoidance necessary." << std::endl;
			#endif
			if(print_data) {
				std::cout << "No obstacles detected by sonar! No avoidance necessary." << std::endl;
			}
			if(avoidance_mode || reverse_mode) {
				reverse_mode = false;
				avoidance_mode = false;
				if(print_data) {
					std::cout << "Resetting heading and motor. Obstacle avoidance complete." << std::endl;
				}
				reset_heading();
			}
		}
	}
}

void* Sonar::read_data(int command) {
	switch(command){
		case SNR_SET_REVERSE_THR:
		case SNR_SET_TURN_THR:
			float data;
			std::cin >> data;
			return *((void**)(&data));
			break;
		case SNR_PRINT_DATA:
			bool in_data;
			std::cin >> in_data;
			return *((void**)(&in_data));
			break;
		case SNR_DISABLE:
		case SNR_ENABLE:
		case SNR_GET_READING:
			std::cout << "No data to read in for that command!" << std::endl;
			break;
		default:
			std::cout << "Unknown command passed to sonar subsystem for reading data! Command was : " << command << std::endl;
			return NULL;
			break;
	}
}

void Sonar::handle_message(MESSAGE* message){
	switch(message->command){
		case SNR_SET_TURN_THR:
			turn_threshold = (*(float*)&message->data);
			break;
		case SNR_SET_REVERSE_THR:
			reverse_threshold = (*(float*)&message->data);
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
		case CPS_RET_DES_HEADING:
			old_compass_heading = (*(float*)&message->data);
			break;
		case MOT_RET_SPEED:
			memcpy(old_motor_speed, (char*)message->data, 6);
			#ifdef SONAR_DEBUG
				old_motor_speed[5] = '\0';
				std::cout << "old motor speed recieved: " << old_motor_speed << std::endl;
			#endif
			sem_post(&avoid_reset_control);
			break;
		case SNR_PRINT_DATA:
			print_data = (*(bool*)&message->data);
			break;
		default:
			std::cout << "Unknown command passed to sonar subsystem! Command was : " << message->command << std::endl;
			break;
	}
}
