#include "Camera.h"
#include "FXVariable.h"
#include "TimeEvent.h"
#include "Object.h"
#include "float4x4.h"
#include "Scene.h"
#include "Console.h"

Camera::Camera() {
	// Manage sending to GPU by myself...
	FXCamerapos = new FXVariable("camerapos");
	FXMatProj = new FXVariable("Proj");
	FXMatProjInverse = new FXVariable("ProjInverse");

	// Send out a message if we change
	OnChange = new TimeEvent();

	updatecount = 0;
	BeginUpdate();
	{
		up = float3(0,1,0);
		SetPos(float3(1,1,1));
		SetLookAt(float3(0,0,0));
		SetFOV(Utils::DegToRad(70));
		// ratio is defined by screen resolution
		SetMinViewDistance(0.1);
		SetMaxViewDistance(8000);
	}
	EndUpdate();
}
Camera::~Camera() {
	delete OnChange;
}
bool Camera::IsVisible(Object* object) {
	for(int i = 0; i < 6; i++) {
		float distance = frustumplanes[i].d + frustumplanes[i].normal.Dot(object->GetWorldCenter());
		if(distance < -object->GetWorldRadius()) {
			return false; // don't bother to check the other planes
		}
	}
	return true;
}
float4x4& Camera::GetMatView() {
	return matView;
}
float4x4& Camera::GetMatViewInverse() {
	return matViewInverse;
}
float4x4& Camera::GetMatProj() {
	return matProj;
}
float4x4& Camera::GetMatProjInverse() {
	return matProjInverse;
}
float4x4& Camera::GetMatViewProj() {
	return matViewProj;
}
float3 Camera::GetExactRight() {
	return exactright;
}
float3 Camera::GetRight() {
	return right;
}
float3 Camera::GetExactUp() {
	return exactup;
}
float3 Camera::GetUp() {
	return up;
}
float3 Camera::GetDir() {
	return dir;
}
float3 Camera::GetLookAt() {
	return lookat;
}
float3 Camera::GetPos() {
	return pos;
}
float Camera::GetMinViewDistance() {
	return minviewdistance;
}
float Camera::GetMaxViewDistance() {
	return maxviewdistance;
}
float Camera::GetAngleH() {
	return angleH;
}
float Camera::GetAngleV() {
	return angleV;
}
float Camera::GetRatio() {
	return ratio;
}
float Camera::GetFOV() {
	return fov;
}
void Camera::SetPos(float3 value) {
	BeginUpdate();
	{
		this->pos = value;
		this->dir = lookat - pos;
		this->dir.Normalize();
		this->lookat = pos + dir;
	}
	EndUpdate();
}
void Camera::SetLookAt(float3 value) {
	BeginUpdate();
	{
		// pos does not change
		this->dir = value - pos;
		this->dir.Normalize();
		this->lookat = pos + dir; // normalized distance from pos
	}
	EndUpdate();
}
void Camera::SetDir(float3 value) {
	BeginUpdate();
	{
		// pos does not change
		this->dir = dir;
		this->dir.Normalize();
		this->lookat = pos + dir;
	}
	EndUpdate();
}
void Camera::Move(float3 value) {
	BeginUpdate();
	{
		this->pos += value;
		// dir does not change
		this->lookat += value;
	}
	EndUpdate();
}
void Camera::SetFOV(float value) {
	BeginUpdate();
	{
		this->fov = value;
	}
	EndUpdate();
}
void Camera::SetRatio(float value) {
	BeginUpdate();
	{
		this->ratio = value;
	}
	EndUpdate();
}
void Camera::SetMinViewDistance(float value) {
	BeginUpdate();
	{
		this->minviewdistance = value;
	}
	EndUpdate();
}
void Camera::SetMaxViewDistance(float value) {
	BeginUpdate();
	{
		this->maxviewdistance = value;
	}
	EndUpdate();
}
void Camera::BeginUpdate() {
	updatecount++;
}
void Camera::EndUpdate() {
	updatecount--;
	if(updatecount == 0) {
		Update();
	}
}
void Camera::Update() {
	UpdateAngles();
	UpdateVectors();
	UpdateMatrices();
	UpdateFrustumPlanes();

	// Update shader variables
	FXCamerapos->Set(&pos);
	FXMatProj->Set(&matProj);
	FXMatProjInverse->Set(&matProjInverse);

	// Notify listeners of property changes
	OnChange->Execute(0);
}
void Camera::UpdateAngles() {

	// Determine angle difference between reference
	float3 dirXZ = float3(dir.x,0,dir.z); // dir on XZ (horizontal) plane
	if(dirXZ.Length() == 0) {
		angleH = 0; // camera points straight down
	} else {
		float3 dirX = float3(dir.x,0,0); // dir on X axis

		// SosCasToa
		angleH = acos(dirX.Length()/dirXZ.Length());
		if(dir.z > 0) {
			angleH = -angleH;
		}
	}

	// SoaCasToa
	angleV = acos(dirXZ.Length()/dir.Length());
	if(dir.y < 0) {
		angleV = -angleV;
	}
}
void Camera::UpdateVectors() {
	// Usually equal to [0 0 1]
	right = up.Cross(dir); // Usually equal to [0 1 0]
	right.Normalize();

	// Exact version of up vector
	exactup = float3(cos(angleH)*sin(angleV),
	                 cos(angleV),
	                 sin(angleH)*sin(angleV));
	exactup.Normalize(); // TODO: not needed?

	// Exact version of right vector
	exactright = exactup.Cross(dir);
	exactright.Normalize();
}
void Camera::UpdateMatrices() {
	// TODO: use float4x4 functions?
	D3DXMatrixLookAtLH(&matView,&pos,&lookat,&up);
	D3DXMatrixPerspectiveFovLH(&matProj,fov,ratio,minviewdistance,maxviewdistance);

	// Cache results
	matViewProj = matView * matProj;
	matViewInverse = matView.Inverse();
	matProjInverse = matProj.Inverse();
}
void Camera::UpdateFrustumPlanes() {
	// Determine dimensions of near plane
	float Hnear = 2.0f * tan(fov / 2.0f) * minviewdistance;
	float Wnear = Hnear * ratio;

	// Determine points of near plane
	float3 nearplanecenter = pos + dir * minviewdistance;
	float3 nearplanetl = nearplanecenter + (exactup * Hnear/2.0f) - (exactright * Wnear/2.0f);
	float3 nearplanetr = nearplanecenter + (exactup * Hnear/2.0f) + (exactright * Wnear/2.0f);
	float3 nearplanebl = nearplanecenter - (exactup * Hnear/2.0f) - (exactright * Wnear/2.0f);
	float3 nearplanebr = nearplanecenter - (exactup * Hnear/2.0f) + (exactright * Wnear/2.0f);

	// Determine dimensions of far plane
	float Hfar = 2.0f * tan(fov / 2.0f) * maxviewdistance;
	float Wfar = Hfar * ratio;

	// Determine points of far plane
	float3 farplanecenter = pos + dir * maxviewdistance;
	float3 farplanetl = farplanecenter + (exactup * Hfar/2.0f) - (exactright * Wfar/2.0f);
	float3 farplanetr = farplanecenter + (exactup * Hfar/2.0f) + (exactright * Wfar/2.0f);
	float3 farplanebl = farplanecenter - (exactup * Hfar/2.0f) - (exactright * Wfar/2.0f);
	float3 farplanebr = farplanecenter - (exactup * Hfar/2.0f) + (exactright * Wfar/2.0f);

	// Update PLANE list
	SetFrustumPlane(&frustumplanes[0],nearplanetr,nearplanetl,farplanetl);  // TOP
	SetFrustumPlane(&frustumplanes[1],nearplanebl,nearplanebr,farplanebr);  // BOTTOM
	SetFrustumPlane(&frustumplanes[2],nearplanetl,nearplanebl,farplanebl);  // LEFT
	SetFrustumPlane(&frustumplanes[3],nearplanebr,nearplanetr,farplanebr);  // RIGHT
	SetFrustumPlane(&frustumplanes[4],nearplanetl,nearplanetr,nearplanebr); // NEAR
	SetFrustumPlane(&frustumplanes[5],farplanetr, farplanetl, farplanebl);  // FAR
}
void Camera::SetFrustumPlane(PLANE* curplane,float3& c1,float3& c2,float3& c3) {

	// Maak twee vectoren in het vlak
	float3 ortho1 = c1 - c2;
	float3 ortho2 = c3 - c2;

	// En bereken de normal met het crossproduct
	curplane->normal = ortho1.Cross(ortho2);
	curplane->normal.Normalize();

	curplane->point = c2;
	curplane->d = -curplane->normal.Dot(curplane->point);
}
void Camera::Print() {
	console->Write("\r\n----- Info for class Camera -----\r\n\r\n");
	console->WriteVar("pos",pos);
	console->WriteVar("lookat",lookat);
	console->WriteVar("dir",dir);
	console->WriteVar("up",up);
	console->WriteVar("exactup",exactup);
	console->WriteVar("right",right);
	console->WriteVar("exactright",exactright);
	console->WriteVar("AngleH",angleH);
	console->WriteVar("AngleV",angleV);
	console->WriteVar("ratio",ratio);
	console->WriteVar("fov",fov);
	console->WriteVar("minviewdistance",minviewdistance);
	console->WriteVar("maxviewdistance",maxviewdistance);
}
