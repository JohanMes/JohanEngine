#ifndef MATERIAL_H
#define MATERIAL_H

#include "Texture.h"
#include "float4.h"

#if BUILDING_DLL
#define DLLIMPORT __declspec(dllexport)
#else
#define DLLIMPORT __declspec(dllimport)
#endif

class DLLIMPORT Material {
	public:
		Material();
		~Material();
		
		Texture* diffusetex;
		Texture* speculartex;
		Texture* normaltex;
		Texture* parallaxtex;
		Texture* ambienttex;
		
		float diffuse;
		float specular;
		float shininess;
		float tiling;
		float3 mixer; // for constant mixing, use this
		float4 color; // color, replaces all other color properties
		
		int cullmode;
		int fillmode;
		bool alphatest;
		bool alphablend;
		bool multitex;
		int shaderindex; // shader to use

		void LoadFromFile(const char* materialpath);
		void Clear(); // set to boring white default
		void Print();
};

#endif
