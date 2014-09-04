#include "Clock.h"

Clock::Clock(bool start) {
	
	starttime = 0;
	
	// Kijk zelf hoe snel de klok tikt
	__int64 countspersec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countspersec);
	secpercount = 1.0/(double)countspersec;
	
	if(start) {
		Start();
	}
}

Clock::~Clock() {
}

void Clock::Start() {
	QueryPerformanceCounter((LARGE_INTEGER*)&starttime);
}

double Clock::GetTimeSec() {
	__int64 currenttime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currenttime);
	return (currenttime-starttime)*secpercount;	
}

double Clock::Reset() {
	double result = GetTimeSec(); // return current time
	Start(); // restart clock
	return result;
}
