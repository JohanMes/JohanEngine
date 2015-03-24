#ifndef OBJECTS_INCLUDED
#define OBJECTS_INCLUDED

#include <list> // allow fast deletion
using std::list;
#include <cstdio>
#include <algorithm>

class Object;
class float3;
class TimeEvent;
class Heightmap;

#if BUILDING_DLL
#define DLLIMPORT __declspec(dllexport)
#else
#define DLLIMPORT __declspec(dllimport)
#endif

class DLLIMPORT Objects {
	private:
		unsigned int updatecount;
		list<Object*> objects;
	public:
		Objects();
		~Objects();
		Object* Add(Object* object);
//		Object* AddPlane(const char* name,const char* matpath,const float3& pos,const float3& rot,float edgelen,unsigned int tiling,unsigned int textiling,Heightmap* height);
		Object* GetByName(const char* name);
		void Delete(Object* thisobject);
		void Clear();
		std::list<Object*>::iterator begin();
		std::list<Object*>::iterator end();
		unsigned int size();
		void Print();
		void Update(); // sort by shader
		void BeginUpdate();
		void EndUpdate();
		
		// Events
		TimeEvent* OnUpdate;
};

#endif
