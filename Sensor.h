#ifndef	SENSOR_H_
#define	SENSOR_H_
#include "Subsystem.h"

/**
 * \class Sensor
 * \brief Sensor class manages sensor tasks
 * 
 * Sets up collector and analysis tasks on init and cancels tasks on shutdown
 */
class Sensor : public Subsystem {
	public:
	
		/**
		 * \brief Sensor default constructor
		 * 
		 * does nothing
		 */
		Sensor(){}
		
		/**
		 * \brief Sensor destructor
		 * 
		 * shuts down the subsystem (cancels the collector and analysis tasks)
		 */
		~Sensor();
		
		/**
		 * \brief initializes the sensor
		 * 
		 * starts the collector and anaysis tasks
		 */
		void init();
		
		/**
		 * \brief shuts down the sensor
		 * 
		 * cancels the collector and analysis tasks
		 */
		void shutdown();
		
		
		/**
		 * \brief Reads in data from message to the sensor.
		 * 
		 * Virtual function implemented at the subsystem level.
		 * Will read data into appropriate data type and then cast to a void* and return.  
		 * The type of data received depends on the message command.  
		 * For example if trying to set the motor pwm duty cycle, a three byte char* is expected. 
		 * This function will read the data in from standard in (using cin) into an appropriate datatype 
		 * for the given command and will return the data to the system message handler for further processing.
		 */
		virtual void* read_data(int command) = 0;
		
		/**
		 * \brief Virtual function to initialize the sensor device.
		 * 
		 * Implemented by each sensor, the init_sensor function is responsible for setting the 
		 * sensor up for data collection.  For example in the case of the 
		 * compass subsystem, the init_device task will set up the i2c bus, initialize the system variables, 
		 * and perform other tasks necessary before data can be collected from the compass. 
		 */
		virtual void init_sensor() = 0;
		
		
		/**
		 * \brief Periodically collects data from the sensor
		 * 
		 * virtual fuction defined at the specific sensor level
		 */
		virtual void collector() = 0;
		
		/**
		 * \brief data analyzer
		 * 
		 * virtual fuction defined at the specific sensor level
		 */
		virtual void analysis() = 0;
		
		
		
		/**
		 * \brief message handler
		 * 
		 * virtual fuction defined at the specific sensor level
		 */
		virtual void handle_message(MESSAGE* message) = 0;
		
	protected:
		pthread_t tCollector, tAnalysis;//collector and analysis threads
		
};

#endif
