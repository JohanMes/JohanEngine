#include "Texture.h"
#include "Console.h"
#include "Textures.h"
#include "Renderer.h"
#include "resource.h"

Texture::Texture() {
	pointer = NULL;
	fullpath = NULL;
	filename = NULL;
}
Texture::Texture(const char* filename) {

	// Bouw volledig pad op als dat nodig is
	this->fullpath = new char[MAX_PATH];
	GetFullPath(filename,"Data\\Textures",fullpath);

	// Verwijder pad als dat nodig is
	this->filename = new char[MAX_PATH];
	ExtractFileName(filename,this->filename);
	
	// Als faal, maak het duidelijk aan factory door pointer = NULL
	HRESULT result = D3DXCreateTextureFromFile(d3ddev,fullpath,&pointer);
	switch(result) {
		case D3DERR_NOTAVAILABLE: {
			console->Write("Error creating texture of file %s: D3DERR_NOTAVAILABLE\r\n",fullpath);
			break;
		}
		case D3DERR_OUTOFVIDEOMEMORY: {
			console->Write("Error creating texture of file %s: D3DERR_OUTOFVIDEOMEMORY\r\n",fullpath);
			break;
		}
		case D3DERR_INVALIDCALL: {
			console->Write("Error creating texture of file %s: D3DERR_INVALIDCALL\r\n",fullpath);
			break;
		}
		case D3DXERR_INVALIDDATA: {
			console->Write("Error creating texture of file %s: D3DXERR_INVALIDDATA\r\n",fullpath);
			break;
		}
		case E_OUTOFMEMORY: {
			console->Write("Error creating texture of file %s: E_OUTOFMEMORY\r\n",fullpath);
			break;
		}
		default: {
			return; // success!
		}
	}
	pointer = NULL; // indicates failure
}
Texture::~Texture() {
	delete[] fullpath;
	delete[] filename;
	SafeRelease(pointer);
}
bool Texture::GetInfo(D3DSURFACE_DESC* result) {
	if(pointer) {
		if(pointer->GetLevelDesc(0,result) == D3D_OK) {
			return true;
		} else {
			return false;
		}
	} else {
		return false;
	}
}
int Texture::GetHeight() {
	D3DSURFACE_DESC info;
	if(GetInfo(&info)) {
		return info.Height;
	} else {
		return -1;
	}
}
int Texture::GetWidth() {
	D3DSURFACE_DESC info;
	if(GetInfo(&info)) {
		return info.Width;
	} else {
		return -1;
	}
}
__int64 Texture::GetSize() {
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
	
	console->Write("\r\n----- Info for class Texture -----\r\n\r\n");
	
	console->WriteVar("pointer",pointer);
	console->WriteVar("fullpath",fullpath);
	console->WriteVar("filename",filename);
	console->WriteVar("GetHeight()",GetHeight());
	console->WriteVar("GetWidth()",GetWidth());
}
