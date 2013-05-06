#include <climits>
#include <iostream>
#include "subsys_timing.h"

subsys_timing::subsys_timing(){
	reset_data();
}
void subsys_timing::reset_data(){
	release_N = 0;
	end_N = 0;
	period_sum = 0;
	execution_sum = 0;
	max_period = 0;
	min_period = 89789548897.0; //an arbitrary large number
	max_execution = 0;
	min_execution = 89789548897.0; //an arbitrary large number
	first_time = true;
}
double subsys_timing::ms_time_diff(struct timespec* t1, struct timespec* t2) {
	long ns_diff = t2->tv_nsec - t1->tv_nsec;
	int sec_diff = t2->tv_sec - t1->tv_sec;
	if(ns_diff < 0) {
		ns_diff += 1000000000;
		sec_diff -= 1;
	}
	return (double)(sec_diff*1000) + ((double)(ns_diff/10000))/100.0;
}

void subsys_timing::add_release_time(struct timespec* time){
	if(!first_time){
		//get the period
		double period = ms_time_diff(&last_release_time, time);
		//add to running period sum
		period_sum += period;
		//increment the release counter
		release_N++;
		//new max
		if(period > max_period) {
			max_period = period;
		}
		//new min
		if(period < min_period){
			min_period = period;
		}
	}else{
		first_time = false;
	}
	//replace old release time
	last_release_time.tv_sec = time->tv_sec;
	last_release_time.tv_nsec = time->tv_nsec;
}

void subsys_timing::add_end_time(struct timespec* time){
	//get the period
	double execution = ms_time_diff(&last_release_time, time);
	//add to running execution sum
	execution_sum += execution;
	//replace old release time
	last_end_time.tv_sec = time->tv_sec;
	last_end_time.tv_nsec = time->tv_nsec;
	//increment the release counter
	end_N++;
	if(execution > max_execution) {
		max_execution = execution;
	}
	if(execution < min_execution){
		min_execution = execution;
	}
}
double subsys_timing::get_average_period(){
	if(first_time)
		return 0.0;
	return period_sum/release_N;
}
double subsys_timing::get_max_period(){
	if(first_time)
		return 0.0;
	return max_period;
}
double subsys_timing::get_min_period(){
	if(first_time)
		return 0.0;
	return min_period;
}
double subsys_timing::get_average_execution(){
	if(first_time)
		return 0.0;
	return execution_sum/end_N;
}
double subsys_timing::get_max_execution(){
	if(first_time)
		return 0.0;
	return max_execution;
}
double subsys_timing::get_min_execution(){
	if(first_time)
		return 0.0;
	return min_execution;
}
