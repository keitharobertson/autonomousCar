#ifndef _actuator_h_
#define _actuator_h_
#include "Subsystem.h"

/**
 * \class Actuator
 * \brief Sensor class manages sensor tasks
 * 
 * Sets up collector and analysis tasks on init and cancels tasks on shutdown
 */
class Actuator : public Subsystem {
	public:
		Actuator(){}
		~Actuator();
		void init();
		void shutdown();
		virtual void init_device() = 0;
		virtual void mech_control() = 0;
		virtual void mech_command(char *value) = 0;
	protected:
		pthread_t tmech_control;//mech control thread
		int  iret_mech_control;
		virtual void handle_message(char* message) = 0;
		
};
		

#endif
