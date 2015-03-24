#include "Clock.h"
#include "Resource.h"

Clock::Clock(bool start) {
	starttime = 0;
	timerperiod = Utils::GetTimerPeriod();
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
	return (currenttime-starttime)*timerperiod;	
}
double Clock::Reset() {
	double result = GetTimeSec(); // return current time
	Start(); // restart clock
	return result;
}
