#ifndef RENDERTARGET_H
#define RENDERTARGET_H

#include <d3d9.h>

#if BUILDING_DLL
#define DLLIMPORT __declspec(dllexport)
#else
#define DLLIMPORT __declspec(dllimport)
#endif

class DLLIMPORT RenderTarget {
	LPDIRECT3DTEXTURE9 texture;
	LPDIRECT3DSURFACE9 topsurface;
	LPDIRECT3DSURFACE9 depthtopsurface;
	public:
		RenderTarget();
		RenderTarget(unsigned int width,unsigned int height,D3DFORMAT format,bool createdepth);
		~RenderTarget();
		
		void Create(unsigned int width,unsigned int height,D3DFORMAT format,bool createdepth);
		void Clear();
		LPDIRECT3DTEXTURE9 GetTexture();
		LPDIRECT3DSURFACE9 GetTopSurface();
		LPDIRECT3DSURFACE9 GetDepthTopSurface();
};

#endif
