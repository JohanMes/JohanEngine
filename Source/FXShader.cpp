#include "FXShader.h"
#include "Object.h" // iterating
#include "Console.h"
#include "Resource.h"

FXShader::FXShader(LPD3DXEFFECT FX,const char* name) : FXHandle(FX,name) {
	handle = FX->GetTechniqueByName(name);
	if(!handle) {
		Globals::console->Write("ERROR: cannot create D3DXHANDLE to shader '%s'\r\n",name);
	}
	Reset();
}
FXShader::~FXShader() {
}
void FXShader::Print() {
//	console->WriteVar("name",name);
//	console->WriteVar("handle",handle);
//	if(HasValidRange()) {
//		console->WriteVar("(*begin)->name",(*begin)->GetName());
//		console->WriteVar("(*end)->name",(*end)->GetName());
//	}
//	console->WriteVar("HasValidRange()",HasValidRange());
}
bool FXShader::HasValidRange() {
	return (beginvalid and endvalid);
}
void FXShader::SetRange(ObjectIterator begin,ObjectIterator end) {
	SetBegin(begin);
	SetEnd(end);
}
void FXShader::SetBegin(ObjectIterator begin) {
	this->begin = begin;
	beginvalid = true;
}
void FXShader::SetEnd(ObjectIterator end) {
	this->end = end;
	endvalid = true;
}
void FXShader::Reset() {
	beginvalid = false;
	endvalid = false;
}
ObjectIterator FXShader::GetBegin() {
	return begin; // can be invalid
}
ObjectIterator FXShader::GetEnd() {
	return end; // can be invalid
}
