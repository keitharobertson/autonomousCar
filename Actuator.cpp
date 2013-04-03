#include "Actuator.h"
#include "shirtt.h"

static void* mech_control_task(void* c) {
	setup_rt_task(9);
	Actuator* a = (Actuator*)c;
	a->mech_control();
}

void Actuator::init(){
	init_device();
	iret_mech_control = pthread_create( &tmech_control, NULL, &mech_control_task, (void *)(this));
}

void Actuator::shutdown() {
	pthread_cancel(tmech_control);
}

Actuator::~Actuator(){
	shutdown();
}
