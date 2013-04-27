#ifndef	GPS_H_
#define	GPS_H_

#include <errno.h>
#include <iostream>
#include <linux/i2c-dev.h>
#include <math.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#include "Sensor.h"

#define GPS_NAME	"GPS"
#define GPS_MAX_LENGTH 100
#define GPS_PORT_NAME "/dev/ttyO1"
#define GPS_DEBUG
#define GPS_GPGAA "$GPGGA"
#define GPS_GPGAA_L 6
#define GPS_ROLLBUFF_SIZE 3
/**
 * \class GPS
 * \brief GPS data collection and analysis
 * 
 * Collects data from the GPS and analyzes the data. Sends system 
 * messages to control actuators
 */
class GPS : public Sensor {
	public:
	
		class LatLon{
		public:
			double lat;
			double lon;
			LatLon(){
				lat=lon=0;
			}
			LatLon(double newLat,double newLon){
				lat=newLat;
				lon=newLon;
			}
			double getDistance(const LatLon& other){
				double diff_lat=other.lat-lat;
				double diff_lon=other.lon-lon;
				return diff_lat*diff_lat+diff_lon*diff_lon;
			}
			LatLon & operator+=(const LatLon &other) {
				lat+=other.lat;
				lon+=other.lon;
				return *this;
			}
			LatLon operator-(const LatLon &other) {
				LatLon output;
				output.lat=lat-other.lat;
				output.lon=lon-other.lon;
				return output;
			}
			template<class T>
			LatLon & operator/=(const T &other) {
				lat/=other;
				lon/=other;
				return *this;
			}
			friend std::ostream& operator<<(std::ostream& os, const LatLon& dt){
				os << "(" << dt.lat << "," << dt.lon << ")";
			}
		};

		/**
		 *  \class GPSWayPoint
		 *  \brief Stores GPS WayPoints as a linked list
		 */
		class GPSWayPoint{
			public:
				/**
				 *  \brief GPSWayPoint Default constructor
				 */
				GPSWayPoint(){
					next=NULL;
				}
				/**
				 *  \brief GPSWayPoint standard constructor
				 */
				GPSWayPoint(const LatLon latLon_new,double rad_new,GPSWayPoint* next_new=NULL){
					set( latLon_new, rad_new,next_new);
				}
				/**
				 *  \brief set the latitude,longitude and radius of the waypoint
				 */
				void set(const LatLon latLon_new,double rad_new,GPSWayPoint* next_new=NULL){
					latLon=latLon_new;
					rad=rad_new;
					next=next_new;
				}
				/**
				 *  \brief returns true if the lat/long passed in is within range
				 */
				bool inRange(const LatLon latLon_new){
					if(latLon.getDistance(latLon_new)<rad)
						return true;
					return false;
				}



				/**
				 *  After arrival, this is where the gps should aim
				 */
				GPSWayPoint* next;

			//private:
				/**
				 *  Target latitude/longitude
				 */
				LatLon latLon;
				/**
				 *  Target radius 
				 *  This is ow close the robot needs to get before it has 'arrived'
				 */
				double rad;
		};

		/**
		 * \brief GPS default constructor
		 * 
		 * sets subsystem parameters
		 */
		GPS();
		
		~GPS();
		
		/**
		 * \brief grabs data from GPS
		 * 
		 * Gets data from GPS over UART
		 */
		void data_grab(LatLon& output);
		
		void init_sensor();
		
		/**
		 * \brief collects GPS data
		 * 
		 * collects GPS data and averages data prior to analysis
		 */
		void collector();
		
		/**
		 * \brief data analysis
		 * 
		 * Issues commands to acuators based on GPS data
		 */
		void analysis();
		
		void* read_data(int command);
		
		/**
		 * \brief handles messages sent to the GPS subsystem
		 */
		void handle_message(MESSAGE* message);
		
		/**
		 *  \brief Converts the input command into lat/long and stores in output
		 */
		bool convert_data(char* input,int length,LatLon& output);

		/**
		 *  \brief Update waypoint if necissary
		 */
		bool updateWayPoint();

		/**
		 *  \brief Add Waypoint
		 */
		bool addWayPoint(LatLon latlon,double radius,int index=-1);

		LatLon getLocBufferAvg();

		float getAngle(LatLon startLoc,LatLon eenndLoc);

		void setLocBuffer(const GPS::LatLon location);
	protected:
		/**
		 *  Set 1=cout the heading
		 */
		int output_heading;
		/**
		 *  Last latitude received.
		 */
		//float last_lat;
		/**
		 *  Last longitude received
		 */
		//float last_lon;
		/**
		 *  Serial port file descriptor that GPS connected to.
		 */
		int serial_port;

		/**
		 *  This is the current target for the GPS
		 */
		GPSWayPoint* target;

		/**
		 *  Use this as a head pointer (for memory clearing)
		 */
		GPSWayPoint* headPtr;

		/** Location Buffer (used to smooth noise) */
		LatLon locBuffer[GPS_ROLLBUFF_SIZE];

		/** Index into Location Buffer (used to smooth noise) */
		int locBufferIndex;

		/** The collect/analysis sync semaphore */
		sem_t collect_analysis_sync;
};

#endif
