#include "RenderTarget.h"
#include "resource.h"

RenderTarget::RenderTarget() {
	texture = NULL;
	topsurface = NULL;
	depthtopsurface = NULL;
}
RenderTarget::RenderTarget(unsigned int width,unsigned int height,D3DFORMAT format,bool createdepth) {
	texture = NULL;
	topsurface = NULL;
	depthtopsurface = NULL;
	Create(width,height,format,createdepth);
}
RenderTarget::~RenderTarget() {
	Clear();
}
void RenderTarget::Create(unsigned int width,unsigned int height,D3DFORMAT format,bool createdepth) {
	Clear();
	Globals::d3ddev->CreateTexture(width,height,1,D3DUSAGE_RENDERTARGET,format,D3DPOOL_DEFAULT,&texture,NULL);
	texture->GetSurfaceLevel(0,&topsurface);
	if(createdepth) {
		Globals::d3ddev->CreateDepthStencilSurface(width,height,D3DFMT_D24X8,D3DMULTISAMPLE_NONE,0,0,&depthtopsurface,NULL);
	}
}
void RenderTarget::Clear() {
	Utils::SafeRelease(texture);
	Utils::SafeRelease(topsurface);
	Utils::SafeRelease(depthtopsurface);
	texture = NULL;
	topsurface = NULL;
	depthtopsurface = NULL;
}
LPDIRECT3DTEXTURE9 RenderTarget::GetTexture() {
	return texture;
}
LPDIRECT3DSURFACE9 RenderTarget::GetTopSurface() {
	return topsurface;
}
LPDIRECT3DSURFACE9 RenderTarget::GetDepthTopSurface() {
	return depthtopsurface;
}
