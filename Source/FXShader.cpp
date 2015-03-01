#include "FXShader.h"

FXShader::FXShader(const char* name) : FXHandle(name) {
	handle = FX->GetTechniqueByName(name);
	if(!handle) {
		char buffer[1024];
		snprintf(buffer,sizeof(buffer),"Error creating D3DXHANDLE to shader '%s'\r\n",name);
		MessageBox(hwnd,buffer,"Error",MB_ICONERROR); // Don't use console, it might not exist yet
	}
	Reset();
}
FXShader::~FXShader() {
}
void FXShader::Print() {
	console->WriteVar("name",name);
	console->WriteVar("handle",handle);
	if(HasValidRange()) {
		console->WriteVar("(*begin)->name",(*begin)->GetName());
		console->WriteVar("(*end)->name",(*end)->GetName());
	}
	console->WriteVar("HasValidRange()",HasValidRange());
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
