#include "Animations.h"
#include "Console.h"
#include "Object.h"
#include "Scene.h"

Animations::Animations() {
}

Animations::~Animations() {
	Clear();
}

Animation* Animations::Add(Animation* animation) {
	list.push_back(animation);
	animation->bufferlocation = --list.end();
	return animation;
}
void Animations::Delete(Animation* animation) {
	if(animation) {
		list.erase(animation->bufferlocation);
	}
}
void Animations::Clear() {
	std::list<Animation*>::iterator i = list.begin();
	while(i != list.end()) { // Make sure we can remove items while iterating
		std::list<Animation*>::iterator next = std::next(i); // Store next item iterator (as current will be invalidated)
		delete *i; // Delete current
		i = next; // Goto next
	}
}
void Animations::OnUpdateTime(float dt) {
	std::list<Animation*>::iterator i = list.begin();
	while(i != list.end()) { // Make sure we can remove items while iterating
		std::list<Animation*>::iterator next = std::next(i); // Store next item iterator (as current will be invalidated)	
		
		// Play animation and remove when finished	
		Animation* animation = *i;
		animation->OnUpdateTime(dt);
		if(animation->IsFinished()) {
			delete animation; // removes itself from list
		}
		
		i = next; // Goto next
	}
	for(std::list<Animation*>::iterator i = list.begin();i != list.end();i++) {
		(*i)->OnUpdateTime(dt);
	}
}
void Animations::Print() {
	console->WriteVar("list.size()",(int)list.size());
	for(std::list<Animation*>::iterator i = list.begin();i != list.end();i++) {
		(*i)->Print();
	}
}
void Animations::SaveToCSV() {
	
	// Save next to exe
	char finalpath[MAX_PATH];
	sprintf(finalpath,"%s\\%s",exepath,"Animations.csv");

	FILE* file = fopen(finalpath,"wb");
	if(file) {
		fprintf(file,"Type;Object;Time;Start Time;End Time;Period;Keyframes\r\n");
		for(std::list<Animation*>::iterator i = list.begin();i != list.end();i++) {
			Animation* animation = *i;
			if(animation->object) {
				fprintf(file,"%d;%s;%.3f;%.3f;%.3f;%.3f;%d\r\n",
					animation->type,
					animation->object->GetName(),
					animation->t,
					animation->GetStartTime(),
					animation->GetEndTime(),
					animation->GetPeriod(),
					(int)animation->keyframes.size());
			} else {
				fprintf(file,"%d;;%.3f;%.3f;%.3f;%.3f;%d\r\n",
					animation->type,
					animation->t,
					animation->GetStartTime(),
					animation->GetEndTime(),
					animation->GetPeriod(),
					(int)animation->keyframes.size());
			}
		}
		fclose(file);
	}
}
void Animations::StaticOnUpdateTime(void* sender,double data) {
	((Animations*)sender)->OnUpdateTime(data);
}
