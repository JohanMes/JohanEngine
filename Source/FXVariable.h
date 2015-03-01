#ifndef FXVARIABLE_INCLUDE
#define FXVARIABLE_INCLUDE

#include <cstdio>
#include "FXHandle.h" // inheritance
#include "RenderTarget.h"
#include "Texture.h"
#include "Resource.h" // FX

class FXVariable : public FXHandle {
	LPDIRECT3DBASETEXTURE9 texturevalue;
	float floatvalue;
	unsigned int intvalue;
	public:
		FXVariable(const char* name);
		~FXVariable();
		void Set(void* value);
		void Set(float value);
		void Set(unsigned int value);
		void SetTexture(LPDIRECT3DBASETEXTURE9 value);
		void SetTexture(Texture* value);
		void SetTexture(RenderTarget* value);
};

#endif
