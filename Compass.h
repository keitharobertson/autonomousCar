#ifndef	COMPASS_H_
#define	COMPASS_H_

#include "Sensor.h"

#define COMPASS	"COMPASS"
#define SUBSYS_COMPASS	0

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
		void data_grab();
		
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
		
	protected:
	
		/**
		 * \brief handles messages sent to the compass subsystem
		 */
		void handle_message(char* message);
};

#endif
