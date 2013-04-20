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
		 * \brief collects data from the sensor
		 * 
		 * Virtual function implemented at the specific sensor level.
		 * The collector function executes in the collector thread and executes indefinitely at a set frequency.  
		 * It calls the data_grab to do the actual communication with the hardware to provide a layer of abstraction 
		 * between the software and specifics of the hardware. It can optionally average data prior to analysis if 
		 * the system is expected to be under heavy load and the tasks are not schedulable with the typical release 
		 * frequencies (averaging will have the effect that the analysis task will be released less frequently).
		 */
		virtual void collector() = 0;
		
		/**
		 * \brief Performs the analysis of the data collected from the sensor.
		 * 
		 * Virtual function implemented at the specific sensor level. 
		 * The analysis function executes in the analysis thread and is synced with the collector task by the 
		 * collect_analysis_sync semaphore. Messages will be sent to the actuators using the system message queue.
		 */
		virtual void analysis() = 0;
		
		/**
		 * \brief Handles message sent to the subsystem
		 * 
		 * Virtual function defined at the specific subsystem level. Will handle the messages sent to 
		 * the sensor from the system controller.  Messages could be from command line interface or 
		 * from another subsystem. 
		 */
		virtual void handle_message(MESSAGE* message) = 0;
		
	protected:
		/** collector thread */
		pthread_t tCollector;
		/** analysis thread */
		pthread_t tAnalysis;
		
};

#endif
