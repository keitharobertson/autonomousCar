#include <stdio.h>
#include "Actuator.h"
#include "shirtt.h"


static void* mech_control_task(void* c) {
	setup_rt_task(9);
	Actuator* a = (Actuator*)c;
	a->mech_control();
}

void Actuator::init(){
	init_device();
	if(pthread_create( &tmech_control, NULL, &mech_control_task, (void *)(this)) != 0){
		perror("Error creating tmech_control thread ");
	}
}

void Actuator::shutdown() {
	pthread_cancel(tmech_control);
}

Actuator::~Actuator(){
	shutdown();
}
