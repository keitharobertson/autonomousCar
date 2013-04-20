#ifndef _steering_h_
#define _steering_h_

#include <stdint.h>
#include <semaphore.h>

#include "Actuator.h"

#define STEERING	"STEERING"
/**
 * \class Steering
 * \brief Steering control
 * 
 * receives steering control commands and commands the servo accordingly
 */
class Steering : public Actuator {
	public:
	
		/**
		 * \brief Steering default constructor
		 * 
		 * sets subsystem parameters and sets up the motor command-control sync semaphore
		 */
		Steering();
		
		/**
		 * \brief Steering destructor
		 * 
		 * closes the steering file descriptor (the SPI file descriptor)
		 */
		~Steering();
		
		/**
		 * \brief initialize the steering
		 * 
		 * Responsible for setting the steering up for control by the mechanical control task.
		 * The init_device function will set up the PWM, initialize the system variables, 
		 * and perform other tasks necessary before the steering can be controlled. 
		 */
		void init_device();
		
		/**
		 * \brief the mechanical controller function for the steering.  
		 * 
		 * Runs in the mech_control thread and waits for a sync semaphore before issuing a mechanical 
		 * command if the steering subsystem is enabled.
		 */
		void mech_control();
		
		/**
		 * \brief The mechanical command function issues commands directly to the steering hardware.
		 * 
		 * The mechanical command function is typically called from the mech_control function and 
		 * will issue the actual commands to hardware necessary to control the steering servo.  This function 
		 * will write to PWM to control the steering angle.
		 */
		void mech_command(char *value);
		
		/**
		 * \brief Reads in data from message to steering.
		 * 
		 * Will read data into appropriate data type and then cast to a void* and return.  The type of 
		 * data received depends on the message command.  For example if trying to set the steering 
		 * pwm duty cycle, a three byte char* is expected. This function will read the data in from 
		 * standard in (using cin) into an appropriate datatype for the given command and will return 
		 * the data to the system message handler for further processing.
		 */
		void* read_data(int command);
		
		/**
		 * \brief Handles message sent to steering
		 * 
		 * Will handle the messages sent to steering from the system controller.  Messages could be from 
		 * command line interface or from another subsystem. 
		 */
		void handle_message(MESSAGE* message);		
		
	protected:
	
		/**
		 * \brief Sets the steering PWM duty cycle.
		 * 
		 * Sets the steering_duty_cycle and gives the motor_cmd_ctrl sync semaphore which releases the mech_control task.
		 */
		void set_new_pwm_duty_cycle(const char* value);
		
		/** minimum priority for commands from other subsystems. For example if compass has priority 5, sonar has priority 2,
		 * and min_priority is set to 3, commands from the compass will be ignored and commands from sonar will be accepted.*/
		int min_priority;
		
		/** The current set duty cycle for the steering PWM */
		char steering_duty_cycle[2];
		
		/** the command/control sync semaphore */
		sem_t steer_cmd_ctrl;
		
		/** the steering file descriptor (for the PWM device) */
		int steering_fd;
		
		/** the filepath to the PWM device (in /dev) */
		char steering_filepath[40];
};

#endif
