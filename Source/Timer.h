#ifndef TIMER_H
#define TIMER_H

#include <list>
#include "TimeEvent.h"

#if BUILDING_DLL
#define DLLIMPORT __declspec(dllexport)
#else
#define DLLIMPORT __declspec(dllimport)
#endif

class DLLIMPORT Timer {
	friend class Timers;
	std::list<Timer*>::iterator bufferlocation;
	public:
		Timer();
		~Timer();

		bool set; // opposite of paused
		int ms;
		TimeEvent* OnTimer;

		bool GetFinished();
		void AddEvent(void (*function)(void* sender,double data),void* sender);
		void SetTime(int ms,bool set);
		void OnUpdateTime(double dt);
};

#endif
