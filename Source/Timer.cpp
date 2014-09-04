#include "Timer.h"
#include "Parser.h"
#include "Scene.h"
#include "Console.h"
#include "TimeEvent.h"

Timer::Timer() {
	this->ms = -1;
	this->set = false;
	this->OnTimer = new TimeEvent();
	
	scene->timers->Add(this);
}
Timer::~Timer() {
	delete OnTimer;
	scene->timers->Delete(this);
}

void Timer::AddEvent(void (*function)(void* sender,double data),void* sender) {
	OnTimer->Add(function,sender);
}
void Timer::SetTime(int ms,bool set) {
	this->ms = ms;
	this->set = set;	
}
void Timer::OnUpdateTime(double dt) {
	if(set) {
		ms -= (dt * 1000);
		if(ms <= 0) {
			OnTimer->Execute(0); // unused parameters
		}
	}
}
bool Timer::GetFinished() {
	return (ms <= 0);
}
