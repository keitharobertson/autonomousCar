/** @file */

#ifndef _SUBSYS_COMMANDS_H_
#define	_SUBSYS_COMMANDS_H_

/**
 * \brief Defines the structure of a message to be sent to a subsystem.
 * 
 * Struct containing message information.  Message designed to be passed through POSIX message queues.
 * The SystemControl will relay the message according to the to parameter.
 */
typedef struct _MESSAGE {
	
	/** Specifies the subsystem from which the message was sent */
	int from;
	
	/** Specifies the destination subsystem for the message */
	int to;
	
	/** Specfies the command to be issued to the receiving subsystem */
	int command;
	
	/** Specifies data to be used in conjuction with the command. For example when setting a new compass heading, 
	 * we need to know the new heading which would be passed in the message data */
	void* data;
	
	/** A default constructor for the message */
	_MESSAGE(){}
	
	/** Constructor for the message */
	_MESSAGE(int from, int to, int command)
		: from(from), to(to), command(command) { }
	
	/** Constructor for the message */
	_MESSAGE(int from, int to, int command, void* data)
		: from(from), to(to), command(command), data(data) { }
	
} MESSAGE;

/** \defgroup subsys_nums Subsystem Numbers
 * Numbers assigned to each subsystem used for unique identification by the messaging system.
 * @{ */
/** GPS Subsystem Number */
#define SUBSYS_GPS		0	
/** Compass Subsystem Number */
#define SUBSYS_COMPASS	1
/** Sonar Subsystem Number */
#define SUBSYS_SONAR	2
/** Motor Subsystem Number */
#define SUBSYS_MOTOR	3
/** Steering Subsystem Number */
#define SUBSYS_STEERING	4
/** Camera Subsystem Number */
#define SUBSYS_CAMERA	5
/**@}*/

/** \defgroup steering_commands Steering Commands
 * Commands that can be issued to the Steering subsystem.
 * @{ */
/** hard left turn */
#define	STR_HARD_LEFT		0
/** hard right turn */
#define	STR_HARD_RIGHT		1
/** slight left turn */
#define	STR_SLIGHT_LEFT		2
/** slight right turn */
#define STR_SLIGHT_RIGHT	3
/** incremental left turn */
#define STR_FINE_LEFT		4
/** incremental right turn */
#define STR_FINE_RIGHT		5
/** go straight */
#define	STR_STRAIGHT		6
/** set steering to value between 0 and 100. Straight is ~50.  */
#define STR_SET_STEERING	7
/** disable steering control */
#define	STR_DISABLE			8
/** enable steering control */
#define	STR_ENABLE			9
/** set minimum command priority (uses posix mq priority) - useful for obstacle avoidance 
 * when you want to disable compass and gps commands while navigating around obstacle */
#define STR_SET_MIN_PRIO	10
/** @} */

/** \defgroup motor_commands Motor Commands
 * Commands that can be issued to the Motor subsystem.
 * @{ */
/** fast speed */
#define	MOT_FAST			0
/** slow speed */
#define	MOT_SLOW			1
/** stop the motors */
#define	MOT_STOP			2
/** medium speed */
#define	MOT_MID				3
/** set speed to value between 0 and 100 */
#define	MOT_SET_SPEED		4
/** disable motor control */
#define	MOT_DISABLE			5
/** enable motor control */
#define	MOT_ENABLE			6
/** set minimum command priority (uses posix mq priority) - useful for 
 * obstacle avoidance when you want to disable compass and gps commands while navigating around obstacle */
#define MOT_SET_MIN_PRIO	7
/** set direction of motor (1=forward, 0=backward) */
#define MOT_DIRECTION		8
/** @} */

/** \defgroup compass_commands Compass Commands
 * Commands that can be issued to the Compass subsystem.
 * @{ */
/** set the desired compass heading */
#define	CPS_SET_HEADING		0
/** subtract 90 from compass heading */
#define	CPS_LEFT_90			1
/** add 90 to compass heading */
#define	CPS_RIGHT_90		2
/** add 180 to compass heading */
#define	CPS_180				3
/** disable compass subsystem */
#define	CPS_DISABLE			4
/** enable compass subsystem */
#define	CPS_ENABLE			5
/** output current compass reading */
#define	CPS_GET_READING		6
/** sends back the desired compass heading */
#define CPS_RETURN_DES_HEADING	7
/** used for returning the compass desired heading (command can be sent to ANY subsystem that is configured to accept it) */
#define CPS_RET_DES_HEADING	99
/** @} */

/** \defgroup gps_commands GPS Commands
 * Commands that can be issued to the GPS subsystem.
 * @{ */
/** set destination latitude */
#define	GPS_SET_LAT			0
/** set destination longitude */
#define	GPS_SET_LNG			1
/** disable GPS navigation */
#define	GPS_DISABLE			2
/** enable GPS navigation */
#define	GPS_ENABLE			3
/** enable display */
#define GPS_DISPLAY 		4
/** enable GPS navigation */
#define GPS_NO_DISPLAY 		5
 /** Add a waypoint at the current location */
#define GPS_ADDWAY			6
/** @}*/

/** \defgroup sonar_commands Sonar Commands
 * Commands that can be issued to the Sonar subsystem.
 * @{ */
/** set the sonar distance threshold - the maximum distance at which the subsystem takes control to avoid an obstacle. */
#define SNR_SET_DIST_THR	0
/** diable sonar control */
#define SNR_DISABLE			1
/** enable sonar control */
#define SNR_ENABLE			2
/** output sonar reading */
#define	SNR_GET_READING		3
/** print sonar data every time it is collected for testing/debugging. Turn on, data=1; turn off, data=0 */
#define SNR_PRINT_DATA		4
/**@}*/

#endif
