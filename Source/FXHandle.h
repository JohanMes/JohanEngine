#ifndef FXHANDLE_INCLUDE
#define FXHANDLE_INCLUDE

#include <d3dx9.h> // D3DXHANDLE

#if BUILDING_DLL
#define DLLIMPORT __declspec(dllexport)
#else
#define DLLIMPORT __declspec(dllimport)
#endif

class DLLIMPORT FXHandle {
	protected:
		char* name;
		D3DXHANDLE handle;
		LPD3DXEFFECT FX;
	public:
		FXHandle(LPD3DXEFFECT FX,const char* name);
		~FXHandle();
		char* GetName();
		D3DXHANDLE GetD3DXHandle();
};

#endif
