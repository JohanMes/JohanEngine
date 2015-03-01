#ifndef FXSHADER_INCLUDE
#define FXSHADER_INCLUDE

#include <list>
#include <cstdio>
#include "FXHandle.h" // inheritance
#include "Object.h" // iterating
#include "Console.h" // printing

#define ObjectIterator std::list<Object*>::iterator

class FXShader : public FXHandle {
	private:
		ObjectIterator begin;
		ObjectIterator end;
		bool beginvalid;
		bool endvalid;
	public:
		FXShader(const char* name);
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
