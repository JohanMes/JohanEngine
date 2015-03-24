#ifndef CLOCK_INCLUDE
#define CLOCK_INCLUDE

#include <cstdio>

#if BUILDING_DLL
#define DLLIMPORT __declspec(dllexport)
#else
#define DLLIMPORT __declspec(dllimport)
#endif

class DLLIMPORT Clock {
	private:
		__int64 starttime;
		double timerperiod;
	public:
		Clock(bool start);
		~Clock();
		void Start();
		double GetTimeSec();
		double Reset();
};

#endif
