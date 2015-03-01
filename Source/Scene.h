#ifndef SCENE_INCLUDE
#define SCENE_INCLUDE

#include <cstdio> // scanf etc
#include "Objects.h" // lists
#include "Lights.h" // lists
#include "Timers.h" // lists
#include "Animations.h" // lists
#include "Heightmap.h" // ground stuff
#include "TimeEvent.h" // OnUpdateTime
#include "Object.h" // object, intersects, collision
#include "Parser.h" // LoadFromFile
#include "Dialogs.h" // ???
#include "Renderer.h" // shaders
#include "Bezier.h" // ???

#if BUILDING_DLL
#define DLLIMPORT __declspec(dllexport)
#else
#define DLLIMPORT __declspec(dllimport)
#endif

struct Collision;

class Objects;

class DLLIMPORT Scene {
	private:
		static void StaticOnRenderFrame(void* sender,double data);
		static void StaticOnCameraChange(void* sender,double data);
		static void StaticOnObjectsUpdate(void* sender,double data);
	public:
		Scene();
		~Scene();
		Heightmap* AddHeightMap(const char* matpath,const char* mappath,float width,float minz,float maxz,unsigned int textiling);
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
		void Load(const char* scnpath);
//		void Save(const char* scnpath);

		// Events
		TimeEvent* OnUpdateTime;

		// TODO: private?
		Objects* objects;
		Lights* lights;
		Timers* timers;
		Animations* animations; // uses timers to move objects
		Heightmap* groundheight;
};

extern Scene* scene;

#endif
