#ifndef TEXTURE_INCLUDE
#define TEXTURE_INCLUDE

#include <list>
#include <d3d9.h>


#if BUILDING_DLL
#define DLLIMPORT __declspec(dllexport)
#else
#define DLLIMPORT __declspec(dllimport)
#endif

// for easier typing
class Texture;
typedef std::list<Texture*>::iterator TextureIterator;

class DLLIMPORT Texture {
	private:
		friend class Textures;
		TextureIterator bufferlocation;
		LPDIRECT3DTEXTURE9 d3dinterface;
		char* fullpath; // includes path
		char* filename; // filename only
		Texture(); // create empty shell
		Texture(const char* filename); // load from file
		~Texture();
	public:
		bool GetInfo(D3DSURFACE_DESC* result);
		int GetWidthPixels();
		int GetHeightPixels();
		__int64 GetSizeBytes();
		void LoadFromFile(const char* filename);
		void Print();
		const char* GetFullPath();
		const char* GetFileName();
		LPDIRECT3DTEXTURE9 GetD3DInterface();
		TextureIterator GetBufferLocation();
};

#endif
