#include "Timer.h"
#include "TimeEvent.h" // OnTimer

Timer::Timer() {
	this->OnTimer = new TimeEvent();
	this->miliseconds = -1;
	this->running = false;
}
Timer::Timer(int miliseconds, bool start) {
	this->OnTimer = new TimeEvent();
	this->miliseconds = miliseconds;
	this->running = start;
}
Timer::~Timer() {
	delete OnTimer;
}
void Timer::AddEvent(void (*function)(void* sender,double data),void* sender) {
	OnTimer->Add(function,sender);
}
void Timer::Set(int miliseconds,bool start) {
	this->miliseconds = miliseconds;
	this->running = start;
}
void Timer::OnUpdateTime(double dt) {
	if(running) {
		miliseconds -= (dt * 1000);
		if(miliseconds <= 0) {
			running = false;
			OnTimer->Execute(0); // unused parameters
		}
	}
}
bool Timer::IsRunning() {
	return running;
}
bool Timer::IsFinished() {
	return (miliseconds <= 0) and (not running);
}
int Timer::MilisLeft() {
	if(running) {
		return miliseconds;
	} else {
		return -1;
	}
}
std::list<Timer*>::iterator Timer::GetBufferLocation() {
	return bufferlocation;
}
void Timer::SetBufferLocation(std::list<Timer*>::iterator i) {
	bufferlocation = i;
}
