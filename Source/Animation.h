#ifndef ANIMATION_H
#define ANIMATION_H

#include <vector>
#include <list>
#include "float3.h"
#include "TimeEvent.h"

class Object;

#if BUILDING_DLL
#define DLLIMPORT __declspec(dllexport)
#else
#define DLLIMPORT __declspec(dllimport)
#endif

enum AnimationType {
	atLinear,
	atLinearLoop,
	// ease
	// bezier
};

class Keyframe {
	public:
		Keyframe(double t,float3 position) {
			this->t = t;
			this->position = position;
			this->rotation = 0;
		}
		Keyframe(double t,float3 position,float3 rotation) {
			this->t = t;
			this->position = position;
			this->rotation = rotation;
		}
		double t;
		float3 position;
		float3 rotation;
};

class DLLIMPORT Animation {
	friend class Animations;

	std::list<Animation*>::iterator bufferlocation;
	public:
		Animation(Object* object,AnimationType type);
		~Animation();
	
		AnimationType type;
		Object* object;
		double t;
		std::vector<Keyframe*> keyframes;
		TimeEvent* OnAnimationEnd;
		
		bool IsFinished();
		double GetStartTime();
		double GetEndTime();
		double GetPeriod();
		void OnUpdateTime(double dt);
		int GetKeyframeBefore(double t);
		int GetKeyframeAfter(double t);
		void AddKeyframe(Keyframe* keyframe);
		void ClearKeyframes();
		void Print();
};

#endif
