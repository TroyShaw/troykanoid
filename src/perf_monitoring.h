#pragma once

//Begins recording performance monitoring. Essentially stores the current tick-count;
void start_perf_timing(void);

//Ends recording performance monitoring. Stores the time since the last call to start_perf_timing().
void end_perf_timing(void);

//Pretty-prints the last recorded performance time, using the given label.
void print_perf_timing(const char *eventName);

//Returns the last recorded performance time.
int get_perf_timing(void);
