#include <windows.h>
#include <cstdio>
#include "Timers.h"
#include "Scene.h"
#include "resource.h"

Timers::Timers() {
}
Timers::~Timers() {
	Clear();
}

void Timers::Add(Timer* timer) {
	list.push_back(timer);
	timer->bufferlocation = --list.end();
}
void Timers::Delete(Timer* timer) {
	if(timer) {
		list.erase(timer->bufferlocation);
	}
}
void Timers::OnUpdateTime(float dt) {
	// Make sure we can remove items while iterating
	std::list<Timer*>::iterator i = list.begin();
	while(i != list.end()) { 
		std::list<Timer*>::iterator next = std::next(i); // Store next item iterator (as current will be invalidated)
		
		// Update timer and remove when finished	
		Timer* timer = *i;
		timer->OnUpdateTime(dt);
		if(timer->GetFinished()) {
			delete timer; // removes itself from list
		}
		i = next; // Goto next
	}
}
void Timers::Clear() {
	std::list<Timer*>::iterator i = list.begin();
	while(i != list.end()) { // Make sure we can remove items while iterating
		std::list<Timer*>::iterator next = std::next(i); // Store next item iterator (as current will be invalidated)
		delete *i; // Delete current
		i = next; // Goto next
	}
}
void Timers::SaveToCSV() {
		
	// Save next to exe
	char finalpath[MAX_PATH];
	sprintf(finalpath,"%s\\%s",exepath,"Timers.csv");
	
	FILE* file = fopen(finalpath,"wb");
	if(file) {
		fprintf(file,"Set;ms left\r\n");
		for(std::list<Timer*>::iterator i = list.begin();i != list.end();i++) {
			Timer* timer = *i;
			fprintf(file,"%d;%u\r\n",
				timer->set,
				timer->ms);
		}		
		fclose(file);
	}
}
void Timers::StaticOnUpdateTime(void* sender,double data) {
	((Timers*)sender)->OnUpdateTime(data);
}
