#ifndef PATH_INCLUDE
#define PATH_INCLUDE

#include <d3dx9.h>
#include <vector>
using std::vector;
#include "float3.h"
#include "float4.h"
#include "float4x4.h"
class Curve;

class Path {
	private:
		std::vector<Curve*> curves;
	public:
		Path();
		Path(Curve* piece);
		~Path();

		void Add(Curve* piece);
		void AddToTail(Curve* piece);

		Curve* GetCurve(float t);
		float3 GetPoint(float t);
		float3 GetTangent(float t);
		float4x4 GetAngle(float t);

		float GetLength();
};

#endif
