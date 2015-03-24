#ifndef FXSHADER_INCLUDE
#define FXSHADER_INCLUDE

#include <list>
#include <cstdio>
#include "FXHandle.h" // inheritance
#include "Object.h"

#if BUILDING_DLL
#define DLLIMPORT __declspec(dllexport)
#else
#define DLLIMPORT __declspec(dllimport)
#endif

class DLLIMPORT FXShader : public FXHandle {
	private:
		ObjectIterator begin;
		ObjectIterator end;
		bool beginvalid;
		bool endvalid;
	public:
		FXShader(LPD3DXEFFECT FX,const char* name);
		~FXShader();
		ObjectIterator GetBegin();
		ObjectIterator GetEnd();
		void Print();
		bool HasValidRange();
		void SetRange(ObjectIterator begin,ObjectIterator end);
		void SetBegin(ObjectIterator begin);
		void SetEnd(ObjectIterator end);
		void Reset();
};

#endif
