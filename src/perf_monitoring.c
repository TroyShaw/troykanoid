#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "perf_monitoring.h"

//Gets the number of ticks.
static int clock_get_time(void);

static int lastStartTime;
static int lastEndDuration;

void start_perf_timing(void)
{
	lastStartTime = clock_get_time();
}

void end_perf_timing(void)
{
	lastEndDuration = clock_get_time() - lastStartTime;
}

void print_perf_timing(const char *eventName)
{
	printf("PERFORMANCE MONITORING: %s: %d\n", eventName, lastEndDuration);
}

int get_perf_timing(void)
{
	return lastEndDuration;
}

static int clock_get_time(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);

    return tv.tv_sec * 1000000LL + tv.tv_usec;
}
