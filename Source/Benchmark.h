#ifndef BENCHMARK_H
#define BENCHMARK_H

#include "Components.h"
#include "TimeEvent.h"
#include "Timer.h"

#if BUILDING_DLL
#define DLLIMPORT __declspec(dllexport)
#else
#define DLLIMPORT __declspec(dllimport)
#endif

class DLLIMPORT Benchmark : public Window {
	std::vector<float> frametimes;
	TimeEventStruct* listener; // is NULL when not running and vice versa
	int ignoreindex;
	float minfps;
	float avgfps;
	float maxfps;
	static void OnRenderFrame(void* sender,double dt);
	static void OnTimerEnd(void* sender,double dt);
	static void OnSaveClick(Component* sender);
	static void OnPrintClick(Component* sender);
	
	void CreateInterface();
	void GetPicSaveFileName(char* result);
	void GetLogSaveFileName(char* result);
	void GetSaveFolder(char* result);
	void AddFrameTime(float dt);
	int GetUsedFrameCount();
	public:
		Benchmark();
		Benchmark(float testtime);
		~Benchmark();
		
    	// Sla een printscreen van het venster op als we klaar zijn
    	Button* btnsave;
    	Button* btnprint;
    	
		// Resultaten
    	Label* minfpstext;
    	Label* minfpsfield;
    	Label* avgfpstext;
    	Label* avgfpsfield;
       	Label* maxfpstext;
    	Label* maxfpsfield;
       	Label* cputext;
    	Label* cpufield;
       	Label* gputext;
    	Label* gpufield;
    	
    	void Start(float testtime);
    	bool IsRunning();
    	void Stop();
    	void Save();
    	void Print();
};

#endif
