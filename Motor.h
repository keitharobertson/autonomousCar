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
		 * \brief Actuator default constructor
		 * 
		 * sets subsystem parameters
		 */
		Motor();
		
		~Motor();
		
		void init_device();
		
		/**
		 * \brief controls the mechanical system
		 * 
		 * 
		 */
		void mech_control();
		
		void mech_command(char *value);
		
		/**
		 * \brief handles messages sent to the compass subsystem
		 */
		void handle_message(MESSAGE* message);		
		
	protected:
	
		void set_new_pwm_duty_cycle(const char* value);
		
		int min_priority;
		char motor_duty_cycle[2];
		int direction;
		sem_t motor_cmd_ctrl;
		
		int motor_fd;
		char motor_filepath[40];
		volatile uint32_t* gptimer_reg;
};

#endif
