#ifndef EVENT_H
#define EVENT_H

#include <vector>
using std::vector;

#if BUILDING_DLL
#define DLLIMPORT __declspec(dllexport)
#else
#define DLLIMPORT __declspec(dllimport)
#endif

class TimeEventStruct {
	public:
		TimeEventStruct(void (*function)(void* sender,double data),void* sender) {
			this->function = function;
			this->sender = sender;
		}
		void* sender;
		void (*function)(void* sender,double data); // dynamische lijst van maken...
};

class DLLIMPORT TimeEvent {
	std::vector<TimeEventStruct*> list;
	int persistentcount;
	public:
		TimeEvent();
		~TimeEvent();

		TimeEventStruct* AddPersistent(void (*function)(void* sender,double data),void* sender);
		TimeEventStruct* Add(void (*function)(void* sender,double data),void* sender);
		void Delete(TimeEventStruct* value);
		void Execute(double data); // override data
		void Clear(); // user function, keep 
};

#endif
