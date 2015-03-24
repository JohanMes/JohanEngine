#ifndef SCENE_INCLUDE
#define SCENE_INCLUDE

#include <cstdio> // scanf etc

#if BUILDING_DLL
#define DLLIMPORT __declspec(dllexport)
#else
#define DLLIMPORT __declspec(dllimport)
#endif

struct Collision;
class Heightmap;
class float2;
class float3;
class float4;
class float4x4;
class Object;
class Objects;
class Timer;
class TimeEvent;
class Timers;
class Lights;
class Animations;

class DLLIMPORT Scene {
	private:
		static void StaticOnRenderFrame(void* sender,double data);
		static void StaticOnCameraChange(void* sender,double data);
		static void StaticOnObjectsUpdate(void* sender,double data);
	public:
		Scene();
		~Scene();
//		Heightmap* AddHeightMap(const char* matpath,const char* mappath,float width,float minz,float maxz,unsigned int textiling);
		void Update();
		void Clear();
		void OnLostDevice();
		void OnResetDevice();
		void BeginUpdate();
		void EndUpdate();
		void OnRenderFrame(float dt);
		void OnCameraChange(float t);
		void OnObjectsUpdate(float t);
		Collision IntersectScene(float3 worldpos,float3 worlddir);
		void LoadFromFile(const char* filename);
//		void SaveToFile(const char* scnpath);
		Object* AddObject(const char* name); // TODO: move to Objects?
		Object* AddObject(const char* objectpath,const float3& pos,const float3& rot,float scale);
		Object* AddObject(const char* objectpath,const float3& pos,const float4x4& rot,float scale);
		Object* AddObject(const char* name,const char* modelpath,const char* materialpath,const float3& pos,const float3& rot,float scale);
		Object* AddObject(const char* name,const char* modelpath,const char* materialpath,const float3& pos,const float4x4& rot,float scale);
		void DeleteObject(Object* object);
		Timer* AddTimer();
		Timer* AddTimer(int miliseconds, bool start);
		void DeleteTimer(Timer* timer);

		// Events
		TimeEvent* OnUpdateTime;

		// TODO: private?
		Objects* objects;
		Lights* lights;
		Timers* timers;
		Animations* animations; // uses timers to move objects
		Heightmap* groundheight;
};

namespace Globals {
	extern Scene* scene;
}

#endif
