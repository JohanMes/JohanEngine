#include "FXVariable.h"

FXVariable::FXVariable(const char* name) : FXHandle(name) {
	texturevalue = NULL;
	floatvalue = 0.0f;
	intvalue = 0;
	handle = FX->GetParameterByName(NULL,name);
	if(!handle) {
		char buffer[1024];
		snprintf(buffer,sizeof(buffer),"Error creating D3DXHANDLE to variable '%s'\r\n",name);
		MessageBox(hwnd,buffer,"Error",MB_ICONERROR); // Don't use console, it might not exist yet
	}
}
FXVariable::~FXVariable() {
}
void FXVariable::Set(void* value) {
	FX->SetValue(handle,value,D3DX_DEFAULT); // skip size validation...
}
void FXVariable::Set(float value) {
	if(floatvalue != value) {
		floatvalue = value;
		FX->SetValue(handle,&value,D3DX_DEFAULT); // skip size validation...
	}
}
void FXVariable::Set(unsigned int value) {
	if(intvalue != value) {
		intvalue = value;
		FX->SetValue(handle,&value,D3DX_DEFAULT); // skip size validation...
	}
}
void FXVariable::SetTexture(LPDIRECT3DBASETEXTURE9 value) {
	if(texturevalue != value) { // prevent slow texture state changes
		texturevalue = value;
		FX->SetTexture(handle,texturevalue);
	}
}
void FXVariable::SetTexture(Texture* value) {
	SetTexture(value->pointer);
}
void FXVariable::SetTexture(RenderTarget* value) {
	SetTexture(value->GetTexture());
}
