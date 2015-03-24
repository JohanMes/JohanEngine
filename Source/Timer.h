#ifndef TIMER_H
#define TIMER_H

#include <list>
using std::list;

#if BUILDING_DLL
#define DLLIMPORT __declspec(dllexport)
#else
#define DLLIMPORT __declspec(dllimport)
#endif

class TimeEvent;

class DLLIMPORT Timer {
	private:
		std::list<Timer*>::iterator bufferlocation;
		bool running; // opposite of paused
		int miliseconds;
	public:
		Timer();
		Timer(int miliseconds, bool start);
		~Timer();
		std::list<Timer*>::iterator GetBufferLocation();
		void SetBufferLocation(std::list<Timer*>::iterator i);
		int MilisLeft();
		bool IsRunning();
		bool IsFinished();
		void AddEvent(void (*function)(void* sender,double data),void* sender);
		void Set(int miliseconds, bool start);
		void OnUpdateTime(double dt);

		// Events
		TimeEvent* OnTimer;
};

#endif
