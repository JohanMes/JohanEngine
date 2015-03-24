#ifndef CURVE_INCLUDE
#define CURVE_INCLUDE

#include <vector>
using std::vector;
#include "float3.h"
#include "float4x4.h"

class Curve {
	private:
		vector<float3> points; // max order 30
		float length; // cached
		void Update(); // set length
	public:
		Curve();
		Curve(float3 p0,float3 p1);
		Curve(float3 p0,float3 p1,float3 p2);
		Curve(float3 p0,float3 p1,float3 p2,float3 p3);
		Curve(vector<float3>& points);
		~Curve();
		float3 GetPoint(float t);
		float3 GetTangent(float t);
		float4x4 GetAngle(float t);
		float3 GetStart();
		void SetEnd(float3 p0);
		float GetLength();
};

#endif
