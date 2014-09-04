#include "TimeEvent.h"

TimeEvent::TimeEvent() {
	persistentcount = 0;
}
TimeEvent::~TimeEvent() {
	for(unsigned int i = 0;i < list.size();i++) {
		delete list[i];
	}
}
TimeEventStruct* TimeEvent::AddPersistent(void (*function)(void* sender,double data),void* sender) {
	TimeEventStruct* result = new TimeEventStruct(function,sender);
	list.insert(list.begin() + persistentcount,result);
	persistentcount++;
	return result;
}
TimeEventStruct* TimeEvent::Add(void (*function)(void* sender,double data),void* sender) {
	TimeEventStruct* result = new TimeEventStruct(function,sender);
	list.push_back(result);
	return result;
}
void TimeEvent::Execute(double data) {
	for(unsigned int i = 0;i < list.size();i++) {
		list[i]->function(list[i]->sender,data);
	}
}
void TimeEvent::Clear() {
	list.clear();
}
void TimeEvent::Delete(TimeEventStruct* value) {
	for(unsigned int i = 0;i < list.size();i++) {
		if(list[i] == value) {
			delete value;
			list.erase(list.begin() + i);
			break;
		}
	}
}
