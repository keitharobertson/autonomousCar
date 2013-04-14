#include <iostream>
#include <string.h>
#include <stdlib.h>
#include "shirtt.h"
#include "SystemControl.h"
#include "SUBSYS_COMMANDS.h"

#define MAIN_PRIORITY (49)/* we use 49 as the PRREMPT_RT use 50
                            as the priority of kernel tasklets
                            and interrupt handler by default */
                        
void send_message_to_subsys(SystemControl *c, MESSAGE* message){
	char* mess = new char[4];
	memcpy(mess, &message, sizeof(MESSAGE*));
	int prio = 1;
	if(mq_send (c->sys_mq, mess, MSG_SIZE, prio) == ERROR) {
		perror("System message failed to send!");
		exit(-1);
	}
}

void test_exit(SystemControl* c){
	c->shutdown();
	exit(0);
}

int main() {
	SystemControl c;
	c.init();
	std::string input;
	int subsys_num;
	int has_data;
	int command;
	int data;
	MESSAGE subsys_mess;
	subsys_mess.from = 7;
	while(1) {
		std::cout << "Command> ";
		input[0]='\0';
		std::cin >> input;
		if(input[0] == '\0'){
			test_exit(&c);
		}
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
		}else if(input == "subsys"){
			std::cin >> has_data;
			std::cin >> subsys_mess.to;
			std::cin >> subsys_mess.command;
			if(has_data){
				std::cin >> subsys_mess.data;
			}
			
		}else if(input == "exit") {
			test_exit(&c);
		}else{
			std::cout << "unknown command!" << std::endl;
		}		
		
	}
	return 0;
}
