#ifndef	COMPASS_H_
#define	COMPASS_H_

#include <semaphore.h>

#include "Sensor.h"

#define COMPASS	"COMPASS"

/**
 * \class Compass
 * \brief compass data collection and analysis
 * 
 * Collects data from the compass and analyzes the data. Sends system 
 * messages to control actuators
 */
class Compass : public Sensor {
	public:
	
		/**
		 * \brief Copass default constructor
		 * 
		 * sets subsystem parameters
		 */
		Compass();
		
		/**
		 * \brief grabs data from compass
		 * 
		 * Gets data from compass over I2C
		 */
		float data_grab();
		
		void init_sensor();
		
		/**
		 * \brief collects compass data
		 * 
		 * collects compass data and averages data prior to analysis
		 */
		void collector();
		
		/**
		 * \brief data analysis
		 * 
		 * Issues commands to acuators based on compass data
		 */
		void analysis();
		
		/**
		 * \brief handles messages sent to the compass subsystem
		 */
		void handle_message(MESSAGE* message);
		
		MESSAGE hard_right;
		MESSAGE slight_right;
		
		MESSAGE straight;
		
		MESSAGE hard_left;
		MESSAGE slight_left;
		
	protected:
		
		float desired_heading;
		float meas_heading;
		sem_t collect_analysis_sync;
		
		int compass_fd;
		char compass_filepath[40];
};

#endif
