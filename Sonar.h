#ifndef	SONAR_H_
#define	SONAR_H_

#include <semaphore.h>

#include "Sensor.h"

#define SONAR	"SONAR"

/**
 * \class Sonar
 * \brief sonar data collection and analysis
 * 
 * Collects data from the sonar and analyzes the data. Sends system 
 * messages to control actuators
 */
class Sonar : public Sensor {
	public:
	
		/**
		 * \brief sonar default constructor
		 * 
		 * sets subsystem parameters
		 */
		Sonar();
		
		/**
		 * \brief grabs data from sonar
		 * 
		 * Gets data from sonar over SPI
		 */
		float data_grab();
		
		void init_sensor();
		
		/**
		 * \brief collects sonar data
		 * 
		 * collects sonar data and averages data prior to analysis
		 */
		void collector();
		
		/**
		 * \brief data analysis
		 * 
		 * Issues commands to acuators based on sonar data
		 */
		void analysis();
		
		void* read_data(int command);
		
		/**
		 * \brief handles messages sent to the sonar subsystem
		 */
		void handle_message(MESSAGE* message);
		
	protected:
		float sonar_reading;
		sem_t collect_analysis_sync;
		
		int sonar_fd;
		char sonar_filepath[40];
};

#endif
