#include "Curve.h"
#include "resource.h"
#include "Messagebox.h"
#include <stdio.h>

Curve::Curve() {
}

Curve::Curve(float3 p0,float3 p1) {
	this->points.push_back(p0);
	this->points.push_back(p1);

	Update();
}

Curve::Curve(float3 p0,float3 p1,float3 p2) {
	this->points.push_back(p0);
	this->points.push_back(p1);
	this->points.push_back(p2);

	Update();
}

Curve::Curve(float3 p0,float3 p1,float3 p2,float3 p3) {
	this->points.push_back(p0);
	this->points.push_back(p1);
	this->points.push_back(p2);
	this->points.push_back(p3);
	
	Update();
}

Curve::Curve(std::vector<float3>& points) {
	
	// allow max of 30
	this->points = points;
	if(this->points.size() > 30) {
		this->points.resize(30); 
	}
	
	Update();
}

Curve::~Curve() {
	// ...
}

float3 Curve::GetPoint(float t) {
	
	float3 result(0,0,0);
	for(unsigned int i = 0;i < points.size();i++) {
		result += points[i] * pow(1 - t,points.size() - i - 1) * pow(t,i) * Binomial(points.size() - 1,i);
	}
	
	return result;
}

float3 Curve::GetTangent(float t) {
	
	float dt = 0.0001f;
	
	t = std::max(0.0f,std::min(t,1.0f - dt));
	
	float3 tangent = GetPoint(t + dt) - GetPoint(t);
	return tangent.Normalize();
}

float Curve::GetLength() {
	return length;
}

void Curve::Update() {
	
	// calculate length once
	length = 0.0f;
	
	int numsegs = 100; // TODO: dit berekenen i.p.v. constante gebruiken
	
	float3 oldpoint,newpoint,movement;
	
	for(int i = 0;i < numsegs;i++) {
		
		oldpoint = GetPoint(1.0f/numsegs * i);
		newpoint = GetPoint(1.0f/numsegs * (i + 1));
		
		movement = newpoint - oldpoint;
		
		length += movement.Length();
	}
}

D3DXMATRIX Curve::GetAngle(float t) {
	
	float3 x = GetTangent(t);
	
	float3 y = GramSchmidt2(x,float3(0,1,0));
	y.Normalize();
	
	float3 z = GramSchmidt3(x,y,float3(0,0,1));
	z.Normalize();

	D3DXMATRIX result;
	D3DXMatrixIdentity(&result);
	
	// insert vectors in rows (DX uses vec*matrix)
	result._11 = x.x;
	result._12 = x.y;
	result._13 = x.z;
	
	result._21 = y.x;
	result._22 = y.y;
	result._23 = y.z;
	
	result._31 = z.x;
	result._32 = z.y;
	result._33 = z.z;
	
	// Flip or swap one if determinant = -1...
	if(fequals(D3DXMatrixDeterminant(&result),-1)) { // lame epsilon trick
		result._31 = -z.x;
		result._32 = -z.y;
		result._33 = -z.z;
	}

	return result;
}

void Curve::SetEnd(float3 p0) {
	if(points.size() > 0) {
		points[points.size() - 1] = p0;
	}
	
	Update();
}

float3 Curve::GetStart() {
	if(points.size() > 0) {
		return points[0];
	} else {
		return float3(0,0,0);
	}
}
