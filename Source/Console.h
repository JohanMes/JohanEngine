#ifndef CONSOLE_INCLUDE
#define CONSOLE_INCLUDE

#include <vector>
using std::vector;
#include "Components.h"
#include "Clock.h"
#include "float4x4.h"

#if BUILDING_DLL
#define DLLIMPORT __declspec(dllexport)
#else
#define DLLIMPORT __declspec(dllimport)
#endif

class DLLIMPORT Console : public Window {
	private:
		vector<Clock*> clocks;
		Label* textctrl;
		Button* btnsave;
		Button* btnclear;
		Edit* inputctrl;
		static void OnSaveClick(Component* sender);
		static void OnClearClick(Component* sender);
		static void OnReturnPress(Component* sender);
	public:
		Console();
		~Console();
		void WriteVar(const char* name,int value);
		void WriteVar(const char* name,unsigned int value);
		void WriteVar(const char* name,void* value);
		void WriteVar(const char* name,bool value);
		void WriteVar(const char* name,float value);
		void WriteVar(const char* name,double value);
		void WriteVar(const char* name,const float2& value);
		void WriteVar(const char* name,const float3& value);
		void WriteVar(const char* name,const float4& value);
		void WriteVar(const char* name,const float4x4& value);
		void WriteVar(const char* name,const char* value);
		void Write(const char* format,...);
		void WriteBeginTimer(const char* text);
		void WriteEndTimer(const char* text);
		void GetInput();
		void ProcessString(const char *input);
};

namespace Globals {
	extern Console* console;
}

#endif
