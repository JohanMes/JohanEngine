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
	animations.push_back(animation);
	animation->bufferlocation = --animations.end();
	return animation;
}
void Animations::Delete(Animation* animation) {
	if(animation) {
		animations.erase(animation->bufferlocation);
	}
}
void Animations::Clear() {
	AnimationIterator i = animations.begin();
	while(i != animations.end()) { // Make sure we can remove items while iterating
		AnimationIterator next = std::next(i); // Store next item iterator (as current will be invalidated)
		delete *i; // Delete current
		i = next; // Goto next
	}
}
void Animations::OnUpdateTime(float dt) {
	AnimationIterator i = animations.begin();
	while(i != animations.end()) { // Make sure we can remove items while iterating
		AnimationIterator next = std::next(i); // Store next item iterator (as current will be invalidated)	
		
		// Play animation and remove when finished	
		Animation* animation = *i;
		animation->OnUpdateTime(dt);
		if(animation->IsFinished()) {
			delete animation; // removes itself from list
		}
		
		i = next; // Goto next
	}
	for(AnimationIterator i = animations.begin();i != animations.end();i++) {
		(*i)->OnUpdateTime(dt);
	}
}
void Animations::Print() {
	console->WriteVar("list.size()",(int)animations.size());
	for(AnimationIterator i = animations.begin();i != animations.end();i++) {
		(*i)->Print();
	}
}
void Animations::SaveToCSV() {
	
	// Save next to exe
	char finalpath[MAX_PATH];
	sprintf(finalpath,"%s\\%s",Globals::exepath,"Animations.csv");

	FILE* file = fopen(finalpath,"wb");
	if(file) {
		fprintf(file,"Type;Object;Time;Start Time;End Time;Period;Keyframes\r\n");
		for(AnimationIterator i = animations.begin();i != animations.end();i++) {
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
