#include "Timers.h"

Timers::Timers() {
}
Timers::~Timers() {
	Clear();
}
void Timers::Add(Timer* timer) {
	timerlist.push_back(timer);
	timer->SetBufferLocation(--timerlist.end());
}
void Timers::Delete(Timer* timer) {
	if(timer) {
		timerlist.erase(timer->GetBufferLocation());
	}
}
void Timers::OnUpdateTime(float dt) {
	// Make sure we can remove items while iterating
	std::list<Timer*>::iterator i = timerlist.begin();
	while(i != timerlist.end()) {
		std::list<Timer*>::iterator next =
		    std::next(i); // Store next item iterator (current will be invalidated)

		// Update timer and remove when finished
		Timer* timer = *i;
		timer->OnUpdateTime(dt);
		if(timer->IsFinished()) {
			delete timer; // removes itself from list
		}
		i = next; // Goto next
	}
}
void Timers::Clear() {
	std::list<Timer*>::iterator i = timerlist.begin();
	while(i != timerlist.end()) { // Make sure we can remove items while iterating
		std::list<Timer*>::iterator next =
		    std::next(i); // Store next item iterator (current will be invalidated)
		delete *i; // Delete current
		i = next; // Goto next
	}
}
void Timers::SaveToCSV() {

	// Save next to exe
	char finalpath[MAX_PATH];
	snprintf(finalpath,MAX_PATH,"%s\\%s",exepath,"Timers.csv");

	FILE* file = fopen(finalpath,"wb");
	if(file) {
		fprintf(file,"Set;ms left\r\n");
		for(std::list<Timer*>::iterator i = timerlist.begin(); i != timerlist.end();
		        i++) {
			Timer* timer = *i;
			fprintf(file,"%d;%d;%u\r\n",
			        timer->IsRunning(),
			        timer->IsFinished(),
			        timer->MilisLeft());
		}
		fclose(file);
	}
}
void Timers::StaticOnUpdateTime(void* sender,double data) {
	((Timers*)sender)->OnUpdateTime(data);
}
