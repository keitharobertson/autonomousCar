#ifndef	COMPASS_H_
#define	COMPASS_H_

#include "Sensor.h"

#define COMPASS	"COMPASS"
#define SUBSYS_COMPASS	0

class Compass : public Sensor {
	public:
		Compass();
		void data_grab();
		void collector();
		void analysis();
		void control();
	protected:
		std::string subsys_name;
		void handle_message(char* message);
};

#endif
