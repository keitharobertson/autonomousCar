#ifndef _SUBSYS_COMMANDS_H_
#define	_SUBSYS_COMMANDS_H_

//subsystems:
#define SUBSYS_COMPASS	0
#define SUBSYS_GPS		1
#define SUBSYS_SONAR	2
#define SUBSYS_MOTOR	3
#define SUBSYS_STEERING	4
#define SUBSYS_CAMERA	5

typedef struct _MESSAGE {
	int from;
	int to;
	int command;
	void* data;
	
	_MESSAGE(){}
	_MESSAGE(int from, int to, int command)
		: from(from), to(to), command(command) { }
	_MESSAGE(int from, int to, int command, void* data)
		: from(from), to(to), command(command), data(data) { }
	
} MESSAGE;

//Steering Servo
#define	STR_HARD_LEFT		0	//hard left turn
#define	STR_HARD_RIGHT		1	//hard right turn
#define	STR_SLIGHT_LEFT		2	//slight left turn
#define STR_SLIGHT_RIGHT	3	//slight right turn
#define STR_FINE_LEFT		4	//incremental left turn
#define STR_FINE_RIGHT		5	//incremental right turn
#define	STR_STRAIGHT		6	//go straight
#define STR_SET_STEERING	7	//set steering to value between 0 and 100. Straight is ~50. 
#define	STR_DISABLE			8	//disable steering control
#define	STR_ENABLE			9	//enable steering control
#define STR_SET_MIN_PRIO	10	//set minimum command priority (uses posix mq priority) - useful for obstacle avoidance when you want to disable compass and gps commands while navigating around obstacle

//MOTORS
#define	MOT_FAST			0	//fast speed
#define	MOT_SLOW			1	//slow speed
#define	MOT_STOP			2	//stop the motors
#define	MOT_MID				3	//medium speed
#define	MOT_SET_SPEED		4	//set speed to value between 0 and 100
#define	MOT_DISABLE			5	//disable motor control
#define	MOT_ENABLE			6	//enable motor control
#define MOT_SET_MIN_PRIO	7	//set minimum command priority (uses posix mq priority) - useful for obstacle avoidance when you want to disable compass and gps commands while navigating around obstacle
#define MOT_DIRECTION		8	//set direction of motor (1=forward, 0=backward)

//COMPASS
#define	CPS_SET_HEADING		0	//set the desired compass heading
#define	CPS_LEFT_90			1	//subtract 90 from compass heading
#define	CPS_RIGHT_90		2	//add 90 to compass heading
#define	CPS_180				3	//add 180 to compass heading
#define	CPS_DISABLE			4	//disable compass subsystem
#define	CPS_ENABLE			5	//enable compass subsystem

//GPS
#define	GPS_SET_LAT			0	//set destination latitude
#define	GPS_SET_LNG			1	//set destination longitude
#define	GPS_DISABLE			2	//disable GPS navigation
#define	GPS_ENABLE			3	//enable GPS navigation

//SONAR
#define SNR_SET_DIST_THR	0	//set the sonar distance threshold - the maximum distance at which the subsystem takes control to avoid an obstacle. 
#define SNR_DISABLE			1	//diable sonar control
#define SNR_ENABLE			2	//enable sonar control

#endif
