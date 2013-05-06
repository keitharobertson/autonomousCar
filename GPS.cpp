#include "GPS.h"

#define LOG_TIMING

//GPS class

GPS::GPS() {
	subsys_name = GPS_NAME;
	subsys_num = SUBSYS_GPS;
	locBufferIndex=0;
	locBufferIndexB=0;
	target=NULL;
	if(sem_init(&collect_analysis_sync, 0, 0) != 0){
		perror("Failed to init the compass collector/analysis sync sem \n");
	}
}

GPS::~GPS(){
	close(serial_port);
}


// Clear Buffer
static void clearBuffer(){
	printf("Clear Buffers\n");
	struct termios options_original;
	struct termios options;
	int serial_port = open(GPS_PORT_NAME, O_RDWR | O_NONBLOCK);
	char read_buffer[GPS_MAX_LENGTH + 1] = {0};
	read_buffer[0]='\0';
	uint counter=0;
	printf("start\n");
	if (serial_port != -1){
		#ifdef GPS_DEBUG
			printf("Serial Port open\n");
		#endif
		tcgetattr(serial_port,&options_original);
		tcgetattr(serial_port, &options);
		cfsetispeed(&options, B57600);
		cfsetospeed(&options, B57600);
		options.c_cflag |= (CLOCAL | CREAD);
		options.c_lflag |= ICANON;
		if (tcsetattr(serial_port, TCSAFLUSH, &options)!=0){ //TCSANOW replaced with TCSAFLUSH
			printf("error %d from tcsetattr", errno);
			return;
		}
	}else{
		printf("Unable to open %s",GPS_PORT_NAME);
		printf("Error %d opening %s: %s",errno, GPS_PORT_NAME, strerror(errno));
	}

	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC ,&t);
	t.tv_sec += 2;
	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);

	while(read(serial_port,read_buffer, GPS_MAX_LENGTH)>0){
		printf("%d\n",counter++);
		clock_gettime(CLOCK_MONOTONIC ,&t);
		t.tv_nsec += 200;
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
	}
	printf("Done\n");
	close(serial_port);
	return;
}

void GPS::init_sensor() {
	clearBuffer();
	bool error=0;
	struct termios options_original;
	struct termios options;
	printf("B");
	serial_port = open(GPS_PORT_NAME, O_RDWR); // | O_NONBLOCK);
	printf("A");
	if (serial_port != -1){
		#ifdef GPS_DEBUG
			printf("Serial Port open\n");
		#endif
		tcgetattr(serial_port,&options_original);
		tcgetattr(serial_port, &options);
		cfsetispeed(&options, B57600);
		cfsetospeed(&options, B57600);
		options.c_cflag |= (CLOCAL | CREAD);
		options.c_lflag |= ICANON;
		if (tcsetattr(serial_port, TCSAFLUSH, &options)!=0){ //TCSANOW replaced with TCSAFLUSH
			printf("error %d from tcsetattr", errno);
			error -1;
			return;
		}
	}else{
		printf("Unable to open %s",GPS_PORT_NAME);
		printf("Error %d opening %s: %s",errno, GPS_PORT_NAME, strerror(errno));
	}

	const char* write_buffer1 = "$PMTK313,1*2E\r\n"; //SBAS_ENABLED
	write(serial_port, write_buffer1, strlen(write_buffer1));
	const char* write_buffer2 = "$PMTK301,2*2E\r\n";//SET_DGPS_MODE
	write(serial_port, write_buffer2, strlen(write_buffer2));
	//const char* write_buffer3 = "$PMTK300,200,0,0,0,0*2F\r\n";//SET 5Hz update 
	const char* write_buffer3 = "$PMTK300,1000,0,0,0,0*1C\r\n";//SET .5Hz update 
	write(serial_port, write_buffer3, strlen(write_buffer3));
	const char* write_buffer4 = "$PMTK314,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n";//only output data I care about 
	write(serial_port, write_buffer4, strlen(write_buffer4));
	/*const char* write_buffer5 = "$PMTK101*32\r\n";//hot restart
	write(serial_port, write_buffer5, strlen(write_buffer5));*/
	sleep(2); //required to make flush work, for some reason
	tcflush(serial_port,TCIOFLUSH);
/*
	pthread_t clearBuffer_control;
	if(pthread_create( &clearBuffer_control, NULL, &clearBuffer, (void *)(this)) != 0){
		perror("Error creating tmech_control thread ");
	}
*/
}

double GPS::getAngle(LatLon startLoc,LatLon eenndLoc){
	LatLon rotation=eenndLoc-startLoc;
	if(rotation.lat==0){
		rotation.lat+=0.00001f;
	}
	if(rotation.lon==0){
		rotation.lon+=0.00001f;
	}
	#ifdef GPS_DEBUG
	printf("\tDIFF %f %f\n",rotation.lat,rotation.lon);
	#endif
	double angle=atan(rotation.lon/rotation.lat);
	//printf("\tRANG %f\n",angle);
	angle=angle*360/(2*3.14159);///(3.14159);
	//angle*=-1;
	//printf("\tOANG %f\n",angle);
	//printf("What is going on? %f %f %f %f\n",atan(1/1)/(2*3.14159)*360,atan(-1/1)/(2*3.14159)*360,atan(1/-1)/(2*3.14159)*360,atan(-1/-1)/(2*3.14159)*360);
	//angle*=-1;
	//angle+=180;
	if(rotation.lat<0){
		angle=180+angle;
	}
	if(angle<0){angle+=360;}
	if(angle>360){angle-=360;}
	#ifdef GPS_DEBUG
	printf("\tANGL: %f Lat: %f %f Lon: %f %f\n",angle,startLoc.lat,eenndLoc.lat,startLoc.lon,eenndLoc.lon);
	#endif
	return angle;
}

bool GPS::convert_data(char* input,int length,LatLon& output){
	// Allocate memory to variables
	char command[GPS_GPGAA_L+1];
	// Lat long in charicter form
	char latLonChar[20];
	// Lat long in double form [lat,long]
	double latLon[2];
	// Cound number of lat long/2
	int latLonCounter=0;
	// Allocate memory to input
	strncpy(command,input,GPS_GPGAA_L);
	command[GPS_GPGAA_L]='\0';
	// Comma counter
	int commaCount=0;

	// Check for GPGGA Command
	int gps_OS=-1;
	int i=0;
	if(strcmp(command,GPS_GPGAA)==0){
		gps_OS=GPS_GPGAA_OS;
		i+=GPS_GPGAA_L;
	}
	if(strcmp(command,GPS_GPGLL)==0){
		gps_OS=GPS_GPGLL_OS;
		i+=GPS_GPGLL_L;
	}
	if(strcmp(command,GPS_GPRMC)==0){
		gps_OS=GPS_GPRMC_OS;
		i+=GPS_GPRMC_L;
	}

	// If GPS command is known
	if(gps_OS!=-1){
		// Now that the command is found, 
		//  we grab all the data between the two commas
		//  then convert it to a float
		for(i=GPS_GPGAA_L;i<length;i++){
			if(input[i]==','){
				commaCount++;
			}
			if(commaCount==gps_OS||commaCount==gps_OS+2){\
				for(int ii=i+1;ii<length;ii++){
					if(input[ii]==','){
						strncpy(latLonChar,input+i+1,ii-(i+1));
						latLon[latLonCounter++]=atof(latLonChar);
						i=ii-1;
						break;
					}
				}
			}
			if(commaCount==gps_OS+2){
				break;
			}
		}

		//Decode GPS data and store in outputs
		if(latLonCounter>=2){
			int   degrees[2];
			double minutes[2];
			double outputs[2];
			for(int i=0;i<2;i++){
				degrees[i]=latLon[i]/100;
				minutes[i]=latLon[i]-degrees[i]*100;
				outputs[i]=degrees[i]+minutes[i]/60;
			}
			output.lat=outputs[0];
			output.lon=outputs[1]*-1;
		}
		return 1;
	}
	return 0;
}

bool GPS::updateWayPoint(){
	if(target==NULL) return false;
	if(target->inRange(getLocBufferAvg()))
		target=target->next;
}
bool GPS::addWayPoint(LatLon latlon,double radius,int index){
	#ifdef GPS_DEBUG
	printf("Add Waypoint: %f %f\n",latlon.lat,latlon.lon);
	#endif
	GPSWayPoint* target_loop=target;
	index=target_loop==NULL?-2:index;
	int i;
	for(i=0;index!=-2&&(index==-1||i<index)&&target_loop->next!=NULL;i++){
		target_loop=target_loop->next;
	}
	if(index==-2||index==0){
		target=new GPSWayPoint(latlon,radius,target);
	}else{
		target_loop->next=new GPSWayPoint(latlon,radius,target_loop->next);
	}
}
void GPS::data_grab(LatLon& output){//float& output_lat,float& output_lon){
	#ifdef LOG_TIMING
		int num_times = 0;
		struct timespec timing;
		long prev_nsec;
	#endif
	LatLon outputB;
	if (serial_port != -1){
		char read_buffer[GPS_MAX_LENGTH + 1] = {0};
		read_buffer[0]='\0';
		int chars_read = read(serial_port,read_buffer, GPS_MAX_LENGTH);
		#ifdef LOG_TIMING
			clock_gettime(CLOCK_MONOTONIC ,&timing);
			log_release_time(&timing, COLLECTOR);
		#endif
		char* read_bufferA=read_buffer;
		for(int i=0;i<chars_read;i++){
			if(read_buffer[i]=='$'){
				read_bufferA=&read_buffer[i];
				chars_read-=i;
				break;
			}
		}
		if(chars_read>0){
			read_bufferA[chars_read]='\0';
			#ifdef GPS_DEBUG
				printf(">>B %d\n %s\n",chars_read,read_bufferA);
			#endif

			//Conversion time
			convert_data(read_bufferA,chars_read,outputB);
			if(outputB.lat!=0){
				#ifdef GPS_DEBUG
				printf("\t>>C %f\t%f\n",outputB.lat,outputB.lon);
				#endif
			}
		}else{
			#ifdef GPS_DEBUG
			printf("No GPS Data\n");
			#endif
		}
	}else{
		std::cout << "Bad GPS Serial" << std::endl;
	}
	output.lat=outputB.lat;
	output.lon=outputB.lon;
	#ifdef LOG_TIMING
		clock_gettime(CLOCK_MONOTONIC ,&timing);
		log_end_time(&timing, COLLECTOR);
	#endif
	return;
}

void GPS::collector(){
	LatLon gps_reading;
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC ,&t);
	while(1){
		if(enabled){
			// Get data from GPS reading
			data_grab(gps_reading);
			
			// Only If GPS is not zero
			if(gps_reading.lat!=0||gps_reading.lon!=0){
				// If outputing gps data
				if(1==1){//output_heading){
					#ifdef GPS_DEBUG
					printf("> %f\t%f\n", gps_reading.lat, gps_reading.lon);
					#endif
				}
				setLocBuffer(gps_reading);
				sem_post(&collect_analysis_sync);
			}
		}else{
			// Just Delay
			t.tv_nsec+= DATA_COL_PERIOD_MS*NS_PER_MS;
			while(t.tv_nsec > NS_PER_S){
				t.tv_sec++;
				t.tv_nsec -= NS_PER_S;
			}
			clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
		}
	}
}

// Rolling buffer
void GPS::setLocBuffer(const GPS::LatLon location){
	if(getLocBufferAvg().getDistance(location)<0.001){
		//printf("No Swap Required\n");
		locBuffer[locBufferIndex]=location;
		locBufferIndex=(locBufferIndex+1)%GPS_ROLLBUFF_SIZE;
	}else{
		//printf("Place in back buffer\n");
		locBuffer[GPS_ROLLBUFF_SIZE+locBufferIndexB]=location;
		locBufferIndex=(locBufferIndexB+1)%GPS_ROLLBUFF_SIZE;
		bool swapBuffer=true;
		for(int i=0;i<GPS_ROLLBUFF_SIZE;i++){
			for(int ii=i+1;ii<GPS_ROLLBUFF_SIZE;ii++){
				if(locBuffer[GPS_ROLLBUFF_SIZE+i].getDistance(locBuffer[GPS_ROLLBUFF_SIZE+ii])>0.0001){
					swapBuffer=false;
				}
			}
		}
		if(swapBuffer){
			//printf("Swap back buffer\n");
			for(int i=0;i<GPS_ROLLBUFF_SIZE;i++){
				locBuffer[i]=locBuffer[GPS_ROLLBUFF_SIZE+i];
			}
		}
	}
}

// Get Location Buffer Average
GPS::LatLon GPS::getLocBufferAvg(){
	LatLon output;
	bool divid_cnt=0;
	for(int i=0;i<GPS_ROLLBUFF_SIZE;i++){
		output+=locBuffer[i];
		if(output.lat!=0){
			divid_cnt++;
		}
	}
	if(divid_cnt>0){
		output/=GPS_ROLLBUFF_SIZE;
	}
	return output;
}

// Analyze the data stuff
void GPS::analysis(){
	//printf("Analysis\n");
	
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC ,&t);
	
	#ifdef LOG_TIMING
		struct timespec timing;
	#endif
	
	while(1){
		//wait for data
		sem_wait(&collect_analysis_sync);
		#ifdef LOG_TIMING
			clock_gettime(CLOCK_MONOTONIC ,&timing);
			log_release_time(&timing, ANALYSIS);
		#endif
/*
		// Delay A set amount of time
		t.tv_nsec+= DATA_ANL_PERIOD_MS*NS_PER_MS;
		while(t.tv_nsec > NS_PER_S){
			t.tv_sec++;
			t.tv_nsec -= NS_PER_S;
		}
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
*/
		// If we are targeted somewhere
		if(target!=NULL){
			// Get angle from target
			float angle=(float)getAngle(getLocBufferAvg(),target->latLon);

			// Magnetic north correction
			angle+=GPS_MAG_CORRECTION;//21;	
			
			// Send angle to compass
			#ifdef GPS_DEBUG
			printf("Angle: %f\n",angle);
			#endif
			MESSAGE request_compass_data = MESSAGE(SUBSYS_GPS, SUBSYS_COMPASS, CPS_SET_HEADING,*((void**)(&angle))); //request current compass heading
			send_sys_message(&request_compass_data);
			updateWayPoint();
		}
	}
	#ifdef LOG_TIMING
		clock_gettime(CLOCK_MONOTONIC ,&timing);
		log_end_time(&timing, ANALYSIS);
	#endif
}

void* GPS::read_data(int command) {
	double* inLat;
	double temp;
	switch(command){
		case GPS_ADDWAYDATALAT:
			inLat=new double();
			std::cin >> temp;
			*inLat=temp;
			//printf("Got lat: %f\n",*inLat);
			return ((void*)(inLat));
		case GPS_ADDWAYDATALON:
			inLat=new double();
			std::cin >> temp;
			*inLat=temp;
			//printf("Got lon: %f\n",*inLon);
			return ((void*)(inLat));
		default:
			std::cout << "Unknown command passed to GPS subsystem for reading data! Command was : " << command << std::endl;
			return NULL;
			break;
	}
}

void GPS::handle_message(MESSAGE* message){
	GPSWayPoint* target_loop;
	switch(message->command){
		case GPS_DISABLE:
			enabled = 0;
			break;
		case GPS_ENABLE:
			std::cout << "Enable GPS" << std::endl;
			enabled = 1;
			break;
		case GPS_NO_DISPLAY:
			output_heading = 0;
			break;
		case GPS_DISPLAY:
			std::cout << "Enable Display GPS" << std::endl;
			output_heading = 1;
			break;
		case GPS_ADDWAY:
			addWayPoint(getLocBufferAvg(),0.000000001);
			target_loop=target;
			std::cout << "Added Waypoint: " << getLocBufferAvg() << std::endl;
			while(target_loop!=NULL){
				std::cout << " " << target_loop->latLon << " " << 0.000000001 << std::endl;
				target_loop=target_loop->next;
			}
			break;
		case GPS_ADDWAYDATALAT:
			temp_lat=(*(double*)message->data);
			printf("Got Add Way Lat: %f\n",temp_lat);
			delete (double*)message->data;
			break;
		case GPS_ADDWAYDATALON:
			temp_lon=(*(double*)message->data);
			printf("Got Add Way Lon: %f\n",temp_lat);
			delete (double*)message->data;
			break;
		case GPS_ADDWAYDATARUN:
			addWayPoint(LatLon(temp_lat,temp_lon),0.00008);
			break;
		default:
			std::cout << "Unknown command passed to compass subsystem! Command was : " << message->command << std::endl;
			break;
	}
}



/**
 *  \brief Opens a USB virtual serial port at ttyUSB0.
 * returns - the port's file descriptor or -1 on error.
 */
