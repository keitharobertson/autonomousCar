#ifndef _actuator_h_
#define _actuator_h_
#include "Subsystem.h"

/**
 * \class Actuator
 * \brief Actuator class manages actuator tasks
 * 
 * Sets up collector and analysis tasks on init and cancels tasks on shutdown
 */
class Actuator : public Subsystem {
	public:
	
		/**
		 * \brief Actuator default constructor
		 * 
		 * Sets up subsystem and system message queues and receiver task
		 */
		Actuator(){}
		
		/**
		 * \brief Actuator destructor
		 * 
		 * shuts down the actuator subsystem. Calls shutdown.
		 */
		~Actuator();
		
		/**
		 * \brief initializes the actuator
		 * 
		 * calls the virtual init_device function and creates the mechanical control task.
		 */
		void init();
		
		/**
		 * \brief Actuator Shutdown
		 * 
		 * shuts down the actuator subsystem.  Cancels the mechanical control thread.
		 */
		void shutdown();
		
		/**
		 * \brief Virtual function to initialize the actuator device.
		 * 
		 * Implemented by each actuator, the init_device function is responsible for setting the 
		 * actuator up for control by the mechanical control task.  For example in the case of the 
		 * motor subsystem, the init_device function will set up the PWM, initialize the system variables, 
		 * and perform other tasks necessary before the motor can be controlled. 
		 */
		virtual void init_device() = 0;
		
		/**
		 * \brief Reads in data from message to the actuator.
		 * 
		 * Will read data into appropriate data type and then cast to a void* and return.  The type of 
		 * data received depends on the message command.  For example if trying to set the motor 
		 * pwm duty cycle, a three byte char* is expected. This function will read the data in from 
		 * standard in (using cin) into an appropriate datatype for the given command and will return 
		 * the data to the system message handler for further processing.
		 */
		virtual void* read_data(int command) = 0;
		
		/**
		 * \brief the mechanical controller function for the actuator.  
		 * 
		 * Run in the mech_control thread and waits for a sync semaphore before issuing a 
		 * mechanical command if the subsystem is enabled.
		 */
		virtual void mech_control() = 0;
		
		/**
		 * \brief The mechanical command function issues commands directly to hardware.
		 * 
		 * The mechanical command function is typically called from the mech_control function and 
		 * will issue the actual commands to hardware necessary to control the actuator.  For example, 
		 * for the motor subsystem, this function will write to PWM.
		 */
		virtual void mech_command(char *value) = 0;
		
		/**
		 * \brief Handles message sent to the subsystem
		 * 
		 * Virtual function defined at the specific subsystem level. Will handle the messages sent 
		 * to the actuator from the system controller.  Messages could be from command line interface
		 * or from another subsystem. 
		 */
		virtual void handle_message(MESSAGE* message) = 0;
	
	protected:
	
		/** the mech_control thread */
		pthread_t tmech_control;
		
};
		

#endif
