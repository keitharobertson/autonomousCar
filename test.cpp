#include <iostream>
#include "shirtt.h"
#include "SystemControl.h"

#define MAIN_PRIORITY (49)/* we use 49 as the PRREMPT_RT use 50
                            as the priority of kernel tasklets
                            and interrupt handler by default */

int main() {
	SystemControl c;
	c.init();
	while(1) {
		for(int i=0;i<10000;i++);
	}
	return 0;
}
