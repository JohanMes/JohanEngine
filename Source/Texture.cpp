#include "Texture.h"
#include "Resource.h"
#include "Console.h"

Texture::Texture() {
	// bufferlocation?
	d3dinterface = NULL;
	fullpath = NULL;
	filename = NULL;
}
Texture::Texture(const char* filename) {
	// bufferlocation?
	LoadFromFile(filename);
}
Texture::~Texture() {
	delete[] fullpath;
	delete[] filename;
	Utils::SafeRelease(d3dinterface);
}
void Texture::LoadFromFile(const char* filename) {
	// Get full path if necessary
	this->fullpath = new char[MAX_PATH];
	Utils::GetFullPath(filename,"Data\\Textures",fullpath);

	// Remove directory
	this->filename = new char[MAX_PATH];
	Utils::ExtractFileName(filename,this->filename);

	// Attempt to load from disk
	d3dinterface = NULL;
	HRESULT result = D3DXCreateTextureFromFile(Globals::d3ddev,fullpath,&d3dinterface);
	switch(result) {
		case D3DERR_NOTAVAILABLE: {
			Globals::console->Write("ERROR: cannot creat texture from file '%s': D3DERR_NOTAVAILABLE\r\n",fullpath);
			break;
		}
		case D3DERR_OUTOFVIDEOMEMORY: {
			Globals::console->Write("ERROR: cannot creat texture from file '%s': D3DERR_OUTOFVIDEOMEMORY\r\n",fullpath);
			break;
		}
		case D3DERR_INVALIDCALL: {
			Globals::console->Write("ERROR: cannot creat texture from file '%s': D3DERR_INVALIDCALL\r\n",fullpath);
			break;
		}
		case D3DXERR_INVALIDDATA: {
			Globals::console->Write("ERROR: cannot creat texture from file '%s': D3DXERR_INVALIDDATA\r\n",fullpath);
			break;
		}
		case E_OUTOFMEMORY: {
			Globals::console->Write("ERROR: cannot creat texture from file '%s': E_OUTOFMEMORY\r\n",fullpath);
			break;
		}
		case D3D_OK: {
			break; // success!
		}
	}
}
const char* Texture::GetFullPath() {
	return fullpath;
}
const char* Texture::GetFileName() {
	return filename;
}
LPDIRECT3DTEXTURE9 Texture::GetD3DInterface() {
	return d3dinterface;
}
TextureIterator Texture::GetBufferLocation() {
	return bufferlocation;
}
bool Texture::GetInfo(D3DSURFACE_DESC* result) {
	if(d3dinterface) {
		if(d3dinterface->GetLevelDesc(0,result) == D3D_OK) {
			return true;
		} else {
			return false;
		}
	} else {
		return false;
	}
}
int Texture::GetHeightPixels() {
	D3DSURFACE_DESC info;
	if(GetInfo(&info)) {
		return info.Height;
	} else {
		return -1;
	}
}
int Texture::GetWidthPixels() {
	D3DSURFACE_DESC info;
	if(GetInfo(&info)) {
		return info.Width;
	} else {
		return -1;
	}
}
__int64 Texture::GetSizeBytes() {
	D3DSURFACE_DESC info;
	if(GetInfo(&info)) {
		int bytespp;
		switch(info.Format) {
			case D3DFMT_DXT1: {
				bytespp = 3; // no alpha
				break;
			}
			default: {
				bytespp = 4; // assume ARGB/XRGB flavors
			}
		}
		return info.Width *  info.Height * bytespp;
	} else {
		return -1;
	}
}
void Texture::Print() {
	Globals::console->Write("\r\n----- Info for class Texture -----\r\n\r\n");
	Globals::console->WriteVar("d3dinterface",d3dinterface);
	Globals::console->WriteVar("fullpath",fullpath);
	Globals::console->WriteVar("filename",filename);
	Globals::console->WriteVar("GetHeightPixels()",GetHeightPixels());
	Globals::console->WriteVar("GetWidthPixels()",GetWidthPixels());
	Globals::console->WriteVar("GetWidthPixels()",GetWidthPixels());
}
