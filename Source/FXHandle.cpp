#include "FXHandle.h"

FXHandle::FXHandle(LPD3DXEFFECT FX,const char* name) {
	handle = NULL;
	this->FX = FX;
	this->name = strdup(name);
}
FXHandle::~FXHandle() {
}
char* FXHandle::GetName() {
	return name;
}
D3DXHANDLE FXHandle::GetD3DXHandle() {
	return handle;
}
