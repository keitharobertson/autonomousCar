#ifndef _motor_h_
#define _motor_h_

#include <stdint.h>
#include <semaphore.h>

#include "Actuator.h"

#define MOTOR	"MOTOR"
/**
 * \class Motor
 * \brief motor control
 * 
 * receives motor control commands and commands the motor accordingly
 */
class Motor : public Actuator {
	public:
	
		/**
		 * \brief Motor default constructor
		 * 
		 * sets subsystem parameters and sets up the motor command-control sync semaphore
		 */
		Motor();
		
		/**
		 * \brief Motor destructor
		 * 
		 * closes the motor file descriptor (PWM file descriptor)
		 */
		~Motor();
		
		/**
		 * \brief initialize the motor
		 * 
		 * Responsible for setting the motor up for control by the mechanical control task.
		 * The init_device function will set up the PWM, initialize the system variables, 
		 * and perform other tasks necessary before the motor can be controlled. 
		 */
		void init_device();
		
		/**
		 * \brief the mechanical controller function for the actuator.  
		 * 
		 * Runs in the mech_control thread and waits for a sync semaphore before issuing a mechanical 
		 * command if the motor subsystem is enabled.
		 */
		void mech_control();
		
		/**
		 * \brief The mechanical command function issues commands directly to the motor hardware.
		 * 
		 * The mechanical command function is typically called from the mech_control function and 
		 * will issue the actual commands to hardware necessary to control the motor.  This function 
		 * will write to PWM to control the motor speed and direction.
		 */
		void mech_command(char *value);
		
		/**
		 * \brief Reads in data from message to the motor.
		 * 
		 * Will read data into appropriate data type and then cast to a void* and return.  The type of 
		 * data received depends on the message command.  For example if trying to set the motor 
		 * pwm duty cycle, a three byte char* is expected. This function will read the data in from 
		 * standard in (using cin) into an appropriate datatype for the given command and will return 
		 * the data to the system message handler for further processing.
		 */
		void* read_data(int command);
		
		/**
		 * \brief Handles message sent to the motor
		 * 
		 * Will handle the messages sent to the motor from the system controller.  Messages could be from 
		 * command line interface or from another subsystem. 
		 */
		void handle_message(MESSAGE* message);		
		
	protected:
		
		/**
		 * \brief Sets the motor PWM duty cycle.
		 * 
		 * Sets the motor_duty_cycle and gives the motor_cmd_ctrl sync semaphore which releases the mech_control task.
		 */
		void set_new_pwm_duty_cycle(const char* value);
		
		/** minimum priority for commands from other subsystems. For example if compass has priority 5, sonar has priority 2,
		 * and min_priority is set to 3, commands from the compass will be ignored and commands from sonar will be accepted.*/
		int min_priority;
		
		/** The current set duty cycle for the motor PWM */
		char motor_duty_cycle[6];
		
		/** the motor direction (1=forward, 0=reverse) */
		int direction;
		
		/** the command/control sync semaphore */
		sem_t motor_cmd_ctrl;
		
		/** the motor file descriptor (for the PWM device) */
		int motor_fd;
		
		/** the filepath to the PWM device (in /dev) */
		char motor_filepath[40];
		
		/** message used to send back data requested by other subsystems (typically sonar) */
		MESSAGE data_request;
};

#endif
