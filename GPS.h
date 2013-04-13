#ifndef	GPS_H_
#define	GPS_H_

#include "Sensor.h"

#define GPS_NAME	"GPS"

/**
 * \class GPS
 * \brief GPS data collection and analysis
 * 
 * Collects data from the GPS and analyzes the data. Sends system 
 * messages to control actuators
 */
class GPS : public Sensor {
	public:
	
		/**
		 * \brief GPS default constructor
		 * 
		 * sets subsystem parameters
		 */
		GPS();
		
		~GPS();
		
		/**
		 * \brief grabs data from GPS
		 * 
		 * Gets data from GPS over UART
		 */
		float data_grab();
		
		void init_sensor();
		
		/**
		 * \brief collects GPS data
		 * 
		 * collects GPS data and averages data prior to analysis
		 */
		void collector();
		
		/**
		 * \brief data analysis
		 * 
		 * Issues commands to acuators based on GPS data
		 */
		void analysis();
		
		void* read_data(int command);
		
		/**
		 * \brief handles messages sent to the GPS subsystem
		 */
		void handle_message(MESSAGE* message);
		
	protected:
		
		int gps_fd;
		char gps_filepath[40];
};

#endif
