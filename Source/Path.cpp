#include "Path.h"
#include "float4x4.h"

Path::Path() {
}

Path::Path(Curve* piece) {
	Add(piece);
}

Path::~Path() {
	for(unsigned int i = 0;i < list.size();i++) {
		delete list[i];
	}
	list.clear();
}

void Path::Add(Curve* piece) {
	list.push_back(piece);
}

void Path::AddToTail(Curve* piece) {
	
	// fix previous piece
	if(list.size() > 0) {
		list[list.size() - 1]->SetEnd(piece->GetStart());
	}
	
	list.push_back(piece);
}

Curve* Path::GetCurve(float t) {
	if(list.size() > 0) {
		
		// assign 1/count seconds to each curve, assign t = 1 to final curve
		unsigned int curveindex = floor(t * list.size());
		
		// Assign t = 1 to final curve
		if(curveindex == list.size()) {
			curveindex--;
		}
		
		return list[curveindex];		
	} else {
		return NULL;
	}
}

float3 Path::GetPoint(float t) {
	if(list.size() > 0) {
		
		// Supply time in [0..1] to the curve
		float curvetime = fmod(t * list.size(),1.0f);
		
		return GetCurve(t)->GetPoint(curvetime);
	} else {
		return float3(0,0,0);
	}
}

float3 Path::GetTangent(float t) {
	if(list.size() > 0) {
		
		// Supply time in [0..1] to the curve
		float curvetime = fmod(t * list.size(),1.0f);
		
		return GetCurve(t)->GetTangent(curvetime);
	} else {
		return float3(0,0,0);
	}
}

float4x4 Path::GetAngle(float t) {
	if(list.size() > 0) {
		
		// Supply time in [0..1] to the curve
		float curvetime = fmod(t * list.size(),1.0f);
		
		return GetCurve(t)->GetAngle(curvetime);
	} else {
		float4x4 tmp;
		return tmp.Identity();
	}
}

float Path::GetLength() {
	float result = 0.0f;
	for(unsigned int i = 0;i < list.size();i++) {
		result += list[i]->GetLength();
	}
	return result;
}
