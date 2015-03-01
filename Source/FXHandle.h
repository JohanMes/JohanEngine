#ifndef FXHANDLE_INCLUDE
#define FXHANDLE_INCLUDE

#include <d3dx9.h> // D3DXHANDLE

class FXHandle {
	public:
		FXHandle(const char* name);
		~FXHandle();

		char* name;
		D3DXHANDLE handle;
};

#endif
