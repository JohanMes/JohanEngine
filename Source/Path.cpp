#include "Path.h"
#include "float4x4.h"
#include "Curve.h"

Path::Path() {
}
Path::Path(Curve* piece) {
	Add(piece);
}
Path::~Path() {
	for(unsigned int i = 0; i < curves.size(); i++) {
		delete curves[i];
	}
	curves.clear();
}
void Path::Add(Curve* piece) {
	curves.push_back(piece);
}
void Path::AddToTail(Curve* piece) {

	// fix previous piece
	if(curves.size() > 0) {
		curves[curves.size() - 1]->SetEnd(piece->GetStart());
	}

	curves.push_back(piece);
}
Curve* Path::GetCurve(float t) {
	if(curves.size() > 0) {

		// assign 1/count seconds to each curve, assign t = 1 to final curve
		unsigned int curveindex = floor(t * curves.size());

		// Assign t = 1 to final curve
		if(curveindex == curves.size()) {
			curveindex--;
		}

		return curves[curveindex];
	} else {
		return NULL;
	}
}
float3 Path::GetPoint(float t) {
	if(curves.size() > 0) {

		// Supply time in [0..1] to the curve
		float curvetime = fmod(t * curves.size(),1.0f);

		return GetCurve(t)->GetPoint(curvetime);
	} else {
		return float3(0,0,0);
	}
}
float3 Path::GetTangent(float t) {
	if(curves.size() > 0) {

		// Supply time in [0..1] to the curve
		float curvetime = fmod(t * curves.size(),1.0f);

		return GetCurve(t)->GetTangent(curvetime);
	} else {
		return float3(0,0,0);
	}
}
float4x4 Path::GetAngle(float t) {
	if(curves.size() > 0) {

		// Supply time in [0..1] to the curve
		float curvetime = fmod(t * curves.size(),1.0f);

		return GetCurve(t)->GetAngle(curvetime);
	} else {
		float4x4 tmp;
		return tmp.Identity();
	}
}
float Path::GetLength() {
	float result = 0.0f;
	for(unsigned int i = 0; i < curves.size(); i++) {
		result += curves[i]->GetLength();
	}
	return result;
}
