#ifndef TIMERS_H
#define TIMERS_H

#include <list>
using std::list;
#include <iterator> // std::next
#include <cstdio> // fopen/fwrite
#include <windows.h> // MAX_PATH

#if BUILDING_DLL
#define DLLIMPORT __declspec(dllexport)
#else
#define DLLIMPORT __declspec(dllimport)
#endif

class Timer;

typedef std::list<Timer*>::iterator TimerIterator;

class DLLIMPORT Timers {
	private:
		list<Timer*> timerlist;
	public:
		Timers();
		~Timers();
		void Add(Timer* timer);
		void Delete(Timer* timer);
		void Clear();
		void OnUpdateTime(float dt);
		void SaveToCSV();
		
		// Events		
		static void StaticOnUpdateTime(void* sender,double data);
};

#endif
