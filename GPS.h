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

#include "shirtt.h"
#include "Sensor.h"

#define GPS_NAME	"GPS"
#define GPS_MAX_LENGTH 512 
#define GPS_PORT_NAME "/dev/ttyO1"

/**
 *  GPS Commands and their offsets to obtain the coordinates
 */
#define GPS_GPGAA "$GPGGA"
#define GPS_GPGAA_L 6
#define GPS_GPGAA_OS 2
#define GPS_GPGLL "$GPGLL"
#define GPS_GPGLL_L 6
#define GPS_GPGLL_OS 1
#define GPS_GPRMC "$GPRMC"
#define GPS_GPRMC_L 6
#define GPS_GPRMC_OS 3

/**
 *  Rolling buffer size. Higher = more smooth, slower update.
 */
#define GPS_ROLLBUFF_SIZE 1

/**
 *  Timing attributes
 */
#define NS_PER_MS	1000000
#define NS_PER_S	1000000000
#define DATA_COL_PERIOD_MS	2000
#define DATA_ANL_PERIOD_MS  2000

/**
 * GPS Magnetic north correction
 */
#define GPS_MAG_CORRECTION -20

/**
 *  Define this variable to enable printf debugging
 */
//#define GPS_DEBUG

/**
 * \class GPS
 * \brief GPS data collection and analysis
 * 
 * Collects data from the GPS and analyzes the data. Sends system 
 * messages to control actuators
 */
class GPS : public Sensor {
	public:
		/**
		 *  \class LatLon
		 *  \brief Structure that hold latitude and longitude
		 *
		 *  Holds latitude and longitude 
		 *   preforms various calculations on them
		 */
		class LatLon{
		public:
			double lat;
			double lon;
			/** Default Constructer **/
			LatLon(){
				lat=lon=0;
			}
			/** Pass in a lat long to assign values accrodingly **/
			LatLon(double newLat,double newLon){
				lat=newLat;
				lon=newLon;
			}
			/** Gets the norm distance between this point and another **/
			double getDistance(const LatLon& other){
				double diff_lat=other.lat-lat;
				double diff_lon=other.lon-lon;
				return diff_lat*diff_lat+diff_lon*diff_lon;
			}
			/** Sums each of the components (lat lon) repsectivly **/
			LatLon & operator+=(const LatLon &other) {
				lat+=other.lat;
				lon+=other.lon;
				return *this;
			}
			/** Subtracts each of the componenets (lat lon) repsectivly **/
			LatLon operator-(const LatLon &other) {
				LatLon output;
				output.lat=lat-other.lat;
				output.lon=lon-other.lon;
				return output;
			}
			/** Devide by any class that can devide a double **/
			template<class T>
			LatLon & operator/=(const T &other) {
				lat/=other;
				lon/=other;
				return *this;
			}
			/** Output lat lon to cout **/
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
					rad=rad_new*rad_new;
					next=next_new;
				}
				/**
				 *  \brief returns true if the lat/long passed in is within range
				 */
				bool inRange(const LatLon latLon_new){
					double distance=latLon.getDistance(latLon_new);
					//printf("Dist: %f/%f\n",distance,rad);
					if(distance<rad)
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
		/**
		 * \brief deconstructor
		 */
		~GPS();
		
		/**
		 * \brief grabs data from GPS
		 * 
		 * Gets data from GPS over UART
		 */
		void data_grab(LatLon& output);
		

		/**
		 * \brief initialize the GPS connection
		 */
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
		
		/**
		 * Read the data based of a command
		 */
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

		/**
		 * \brief Get location from rolling buffer (averaged)
		 */
		LatLon getLocBufferAvg();

		/**
		 * \brief Get the angle between two lat long
		 */
		double getAngle(LatLon startLoc,LatLon eenndLoc);

		/**
		 * \brief Add a value to the lat long rolling buffer
		 */
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
		LatLon locBuffer[GPS_ROLLBUFF_SIZE*2];

		/** Index into Location Buffer (used to smooth noise) */
		int locBufferIndex;
		/** Back buffer used for rejected data points */
		int locBufferIndexB;

		/** The collect/analysis sync semaphore */
		sem_t collect_analysis_sync;

		/** Temperary location of lat used when loading in lat */
		double temp_lat;
		/** Temperary location of long used when loading in long */
		double temp_lon;
};

#endif
