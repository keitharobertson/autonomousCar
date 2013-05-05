#ifndef _HEADING_CORRECTION_
#define _HEADING_CORRECTION_

#include <math.h>

void get_angles(int Ax, int Ay, int Az) {
	double phi;
	double theta;
	double psi;
	
	double Bfy;
	double Bfx;
	
	double Vx = 0;
	double Vy = 0;
	double Vz = 0;
	
	phi = atan2(Ay, Az);
	theta = atan2(-1*Ax, Ay*sin(phi)+Az*cos(phi));
	
	Bfy = (By - Vy)*cos(phi) - (Bz-Vz)*sin(phi);
	Bfx = (Bx - Vx)*cos(theta) + (By-Vy)*sin(phi)*sin(phi) + (Bz-Vz)*sin(theta)*cos(phi);
	psi = atan2(-1*Bfy, Bfx);
}

#endif
