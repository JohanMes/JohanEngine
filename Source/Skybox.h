#ifndef SKYBOX_H
#define SKYBOX_H

#if BUILDING_DLL
#define DLLIMPORT __declspec(dllexport)
#else
#define DLLIMPORT __declspec(dllimport)
#endif

class Object;
class Dirlight;

class DLLIMPORT Skybox {
	private:
		static void StaticOnUpdateTime(void* sender,double data);
	public:
		Skybox();
		~Skybox();
		void SetSkybox(const char* modelpath,const char* materialpath);
		void SetSunlight(const char* modelpath,const char* materialpath); // creates light too
		void Clear();
		void OnLostDevice();
		void OnResetDevice();
		void OnUpdateTime(double dt);

		Object* skybox; // sphere for now, creates one, calculates radius
		Object* sunobject; // puts sun plane on skybox at raduis R
		Dirlight* sunlight; // manages direction of this light
};

#endif
