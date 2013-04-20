#ifndef	GPS_H_
#define	GPS_H_

#include "Sensor.h"

#define GPS_NAME	"GPS"
#define GPS_MAX_LENGTH 100
#define GPS_PORT_NAME "/dev/ttyO1"
#define GPS_DEBUG false
#define GPS_GPGAA "$GPGGA"
#define GPS_GPGAA_L 6
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
		void data_grab(float& output_lat,float& output_lon);
		
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
		
		/**
		 * \brief Converts the input command into lat/long and stores in output
		 */
		bool convert_data(char* input,int length,
			float& output_lat,float& output_lon);

	protected:
		int enabled;
		int output_heading;
		float last_lat;
		float last_lon;
		int serial_port;
};

#endif
