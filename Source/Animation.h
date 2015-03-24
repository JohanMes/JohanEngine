#ifndef ANIMATION_H
#define ANIMATION_H

#include <vector>
using std::vector;
#include <list>
using std::list;
#include "float3.h"

class Object;
class TimeEvent;

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

class DLLIMPORT Keyframe {
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

// for easier typing
class Animation;
typedef std::list<Animation*>::iterator AnimationIterator;

class DLLIMPORT Animation {
	friend class Animations;
	AnimationIterator bufferlocation;
	double t;
	public:
		Animation(Object* object,AnimationType type);
		~Animation();
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
		double GetTime();
		
		// TODO: private
		AnimationType type;
		Object* object;
		vector<Keyframe*> keyframes;
		
		// Events
		TimeEvent* OnAnimationEnd;
};

#endif
