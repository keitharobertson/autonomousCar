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
		 * \brief data grabber
		 * 
		 * virtual fuction defined at the specific sensor level
		 */
		virtual void data_grab() = 0;
		
		/**
		 * \brief data collector
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
	protected:
		pthread_t tCollector, tAnalysis;//collector and analysis threads
		int  iret_Collector, iret_Analysis;
		
		/**
		 * \brief message handler
		 * 
		 * virtual fuction defined at the specific sensor level
		 */
		virtual void handle_message(char* message) = 0;
};

#endif
