#include <iostream>

#include "Sensor.h"
#include "shirtt.h"


//Task Spawning ( called from Compass::init() )
static void* collector_task(void* c) {
	setup_rt_task(10);
	Sensor* s = (Sensor*)c;
	s->collector();
}
static void* analysis_task(void* c) {
	setup_rt_task(11);
	Sensor* s = (Sensor*)c;
	s->analysis();
}

void Sensor::init(){
	init_sensor();
	iret_Collector = pthread_create( &tCollector, NULL, &collector_task, (void *)(this));
	iret_Collector = pthread_create( &tAnalysis, NULL, &analysis_task, (void *)(this));
}

void Sensor::shutdown() {
	pthread_cancel(tCollector);
	pthread_cancel(tAnalysis);
}

Sensor::~Sensor(){
	shutdown();
}
