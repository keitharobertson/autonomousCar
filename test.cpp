#include <iostream>
#include <string.h>
#include <stdlib.h>
#include "shirtt.h"
#include "SystemControl.h"
#include "SUBSYS_COMMANDS.h"

#define MAIN_PRIORITY (49)/* we use 49 as the PRREMPT_RT use 50
                            as the priority of kernel tasklets
                            and interrupt handler by default */
                            
// setup message queue attributes
attr.mq_maxmsg = MAX_MSG;
attr.mq_msgsize = MSG_SIZE;
attr.mq_flags = 0;
// disable subsystem by default
enabled = 0;
// Open message queues
subsys_mq = mq_open( (char*)((std::string("/MQ_").append(subsys_name)).c_str()), O_RDWR | O_CREAT, 0664, &attr);
if(subsys_mq == ERROR){
	perror("failed to open subsys mq");
	exit(-1);
}
sys_mq = mq_open((char*)MQ_SYSTEM, O_RDWR | O_CREAT, 0664, &attr);
if(sys_mq == ERROR){
	perror("failed to open sys mq");
	exit(-1);
}
                            
void Subsystem::send_sys_message(MESSAGE* message){
	char* mess = new char[4];
	memcpy(mess, &message, sizeof(MESSAGE*));
	if(mq_send (sys_mq, mess, MSG_SIZE, prio) == ERROR) {
		perror("System message failed to send!");
		exit(-1);
	}
}

int main() {
	SystemControl c;
	c.init();
	std::string input;
	int subsys_num;
	while(1) {
		std::cout << "Command> ";
		std::cin >> input;
		std::cout << std::endl;
		std::cout << "Command: " << input << std::endl;
		
		if(input == "en_subsys"){
			std::cout << "enabling subsystem..." << std::endl;
			std::cin >> subsys_num;
			if(subsys_num < NUM_SUBSYSTEMS){
				c.subsys[subsys_num]->enabled = 1;
				std::cout << "Subsystem " << c.subsys[subsys_num]->subsys_name << " (" << subsys_num << ") enabled" << std::endl;
			}
		}else if(input == "dis_subsys") {
			std::cout << "disabling subsystem..." << std::endl;
			std::cin >> subsys_num;
			std::cout << "Subsystem number: " << subsys_num;
			if(subsys_num < NUM_SUBSYSTEMS){
				c.subsys[subsys_num]->enabled = 0;
				std::cout << "Subsystem " << c.subsys[subsys_num]->subsys_name << " (" << subsys_num << ") disabled" << std::endl;
			}
		}else if(input == "exit") {
			c.shutdown();
			exit(0);
		}else{
			std::cout << "unknown command!" << std::endl;
		}		
		
	}
	return 0;
}
