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
		 * \brief Actuator default constructor
		 * 
		 * sets subsystem parameters
		 */
		Steering();
		
		~Steering();
		
		void init_device();
		
		/**
		 * \brief controls the mechanical system
		 * 
		 * 
		 */
		void mech_control();
		
		void mech_command(char *value);
		
		void* read_data(int command);
		
		/**
		 * \brief handles messages sent to the compass subsystem
		 */
		void handle_message(MESSAGE* message);		
		
	protected:
		void set_new_pwm_duty_cycle(const char* value);
		
		int min_priority;
		char steering_duty_cycle[2];
		sem_t steer_cmd_ctrl;
		int steering_fd;
		char steering_filepath[40];
		volatile uint32_t* gptimer_reg;
};

#endif
