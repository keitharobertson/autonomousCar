#ifndef	SENSOR_H_
#define	SENSOR_H_
#include "Subsystem.h"

class Sensor : public Subsystem {
	public:
		Sensor(){}
		~Sensor();
		void init();
		void shutdown();
		virtual void data_grab() = 0;
		virtual void collector() = 0;
		virtual void analysis() = 0;
	protected:
		pthread_t tCollector, tAnalysis;
		int  iret_Collector, iret_Analysis;
		virtual void handle_message(char* message) = 0;
};

#endif
