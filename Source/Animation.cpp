#include "Animation.h" // includes as little as possible
#include "Object.h" // thing we act upon
#include "TimeEvent.h" // events
#include "float3.h" // keyframe stuff
#include "Resource.h"
#include "Renderer.h"

Animation::Animation(Object* object,AnimationType type) {
	// Apply animation to object
	this->object = object;
	this->object->SetAnimation(this); // throws us away on deletion
	OnAnimationEnd = new TimeEvent();
	
	// Start at time t = 0
	this->type = type;
	t = Globals::renderer->GetTime();
}
Animation::~Animation() {
	ClearKeyframes();
	delete OnAnimationEnd;
	
	// Remove animation from object
	if(object) {
		object->SetAnimation(NULL);
	}
}
void Animation::OnUpdateTime(double dt) {
	t += dt;
	
	// Determine what to do with our time value
	switch(type) {
		case atLinear: {
			if(t >= GetEndTime()) {
				t = GetEndTime();
				OnAnimationEnd->Execute(t);
				break;
			}
			break;
		}
		case atLinearLoop: {
			if(t > GetEndTime()) {
				t = GetStartTime() + (t - GetEndTime()); // loop
			}
			break;
		}
	}
	
	// Applying to nothing? Ignore
	if(!object) {
		return;
	}
	
	switch(type) {
		case atLinear:
		case atLinearLoop: { // interpolate between keyframes
			int fromindex = GetKeyframeBefore(t);
			if(fromindex != -1) { // it's a valid time
				Keyframe* from = keyframes[fromindex];
				Keyframe* to = keyframes[fromindex+1];
				
				// Lerp between the two
				object->BeginUpdate();
				object->SetTranslation(from->position + (to->position - from->position) * (t - from->t)/(to->t - from->t));
				object->SetRotation(from->rotation + (to->rotation - from->rotation) * (t - from->t)/(to->t - from->t));
				object->EndUpdate();
			}
			break;
		}
	}
}
int Animation::GetKeyframeBefore(double t) {
	for(int i = 0;i < (int)keyframes.size()-1;i++) {
		if(keyframes[i]->t < t && keyframes[i+1]->t >= t) {
			return i;
		}
	}
	return -1;
}
int Animation::GetKeyframeAfter(double t) {
	int before = GetKeyframeBefore(t);
	if(before != -1) {
		return before+1;
	} else {
		return -1;
	}
}
double Animation::GetStartTime() {
	if(keyframes.size() > 0) {
		return keyframes[0]->t;
	} else {
		return 0;
	}
}
double Animation::GetEndTime() {
	if(keyframes.size() > 0) {
		return keyframes[keyframes.size()-1]->t;
	} else {
		return 0;
	}
}
double Animation::GetPeriod() {
	return GetEndTime() - GetStartTime();
}
void Animation::Print() {
	console->Write("\r\n----- Info for class Animation -----\r\n\r\n");
	
	console->WriteVar("t",t);
	console->WriteVar("AnimationType",type);
	if(object) {
		console->WriteVar("object->name",object->GetName());
	}
	console->WriteVar("GetStartTime()",GetStartTime());
	console->WriteVar("GetEndTime()",GetEndTime());
	console->WriteVar("GetPeriod()",GetPeriod());
	console->WriteVar("keyframes.size()",(int)keyframes.size());
	
	console->Write("\r\n----- End of info -----\r\n\r\n");
}
void Animation::ClearKeyframes() {
	for(unsigned int i = 0;i < keyframes.size();i++) {
		delete keyframes[i];
	}
	keyframes.clear();
}
void Animation::AddKeyframe(Keyframe* keyframe) {
	keyframes.push_back(keyframe);
	if(keyframes.size() == 1) {
		t = keyframes[0]->t;
	}
}
bool Animation::IsFinished() {
	switch(type) {
		case atLinear: {
			return (t == GetPeriod()); // time is clamped to
		}
		case atLinearLoop: {
			return false;
		}
		default: {
			return false;
		}
	}
}
double Animation::GetTime() {
	return t;
}
