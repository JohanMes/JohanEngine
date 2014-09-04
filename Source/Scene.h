#ifndef SCENE_INCLUDE
#define SCENE_INCLUDE

#include "Objects.h"
#include "Lights.h"
#include "Heightmap.h"
#include "Timers.h"
#include "Animations.h"
#include "TimeEvent.h"

#if BUILDING_DLL
#define DLLIMPORT __declspec(dllexport)
#else
#define DLLIMPORT __declspec(dllimport)
#endif

class DLLIMPORT Scene {
	static void StaticOnRenderFrame(void* sender,double data);
	static void StaticOnCameraChange(void* sender,double data);
	static void StaticOnObjectsUpdate(void* sender,double data);
	public:
		Scene();
		~Scene();
		
		void Load(const char* scnpath);
	//	void Save(const char* scnpath);
		
		Objects* objects;
		Lights* lights;
		Timers* timers;
		Animations* animations; // uses timers to move objects
		
		// Heightmap
		Heightmap* groundheight;
		Heightmap* AddHeightMap(const char* matpath,const char* mappath,float width,float minz,float maxz,unsigned int textiling);
		
		// macro's
		void Update();
		void Clear();
		
		// Rebuild shadow maps etc.
		void OnLostDevice();
		void OnResetDevice();
		
		// Niet bij elke scheet de buffers fixen
		void BeginUpdate();
		void EndUpdate();
		
		void OnRenderFrame(float dt);
		void OnCameraChange(float t);
		void OnObjectsUpdate(float t);
		
		// Collision shit
		Collision IntersectScene(float3 worldpos,float3 worlddir);
		
		TimeEvent* OnUpdateTime;
};

extern Scene* scene;

#endif
