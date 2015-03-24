#ifndef MODEL_INCLUDE
#define MODEL_INCLUDE

#include <list>
using std::list;
#include "float2.h"
#include "float3.h"

class Heightmap;

struct VERTEX {
	float3 pos;
	float2 tex;
	float3 nor;
	float3 tan;
	float3 bin;
};

#if BUILDING_DLL
#define DLLIMPORT __declspec(dllexport)
#else
#define DLLIMPORT __declspec(dllimport)
#endif

class Model;
typedef std::list<Model*>::iterator ModelIterator;

class DLLIMPORT Model {
	private:
		friend class Models; // manages creation and destruction
		std::list<Model*>::iterator bufferlocation;
		Model(); // create empty shell
		Model(const char* filename,bool sendtogpu); // load OBJ
		~Model();
	public:
		__int64 GetSize();
		void FreeBuffers();
		void CreateTangents();
		void Optimize();
		void SendToGPU();
		void GetBoundingSphere();
		void LoadFromOBJ(const char* filename);
		void Load2DQuad(float left,float top,float right,float bottom);
		void LoadPlane(unsigned int tiling,unsigned int textiling,float edgelen,Heightmap* height);
		void LoadBuffer(VERTEX* vb,unsigned int* ib,unsigned int numvertices,unsigned int numindices);
		void LoadParticle();
		void Print();

		// TODO: private?
		LPDIRECT3DVERTEXBUFFER9 vertexbuffer;
		LPDIRECT3DINDEXBUFFER9 indexbuffer;
		float r;
		float3 center;
		VERTEX* localvertexbuffer;
		unsigned int* localindexbuffer;
		unsigned int numvertices;
		unsigned int numfaces;
		char* fullpath;
		char* filename;
};

#endif
