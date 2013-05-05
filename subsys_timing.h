#ifndef _SUBSYS_TIMING_H_
#define _SUBSYS_TIMING_H_

#include <time.h>

class subsys_timing {
	public:
		subsys_timing();
		void add_release_time(struct timespec* time);
		void add_end_time(struct timespec* time);
		double get_average_period();
		double get_max_period();
		double get_min_period();
		double get_average_execution();
		double get_max_execution();
		double get_min_execution();
		void reset_data();
	
	private:
		double ms_time_diff(struct timespec* t1, struct timespec* t2);
		
		int release_N;
		int end_N;
		double period_sum;
		double execution_sum;
		double max_period;
		double min_period;
		double max_execution;
		double min_execution;
		struct timespec last_release_time;
		struct timespec last_end_time;
		
		bool first_time;
};

#endif
