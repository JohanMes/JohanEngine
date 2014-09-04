#include "Spotlight.h"
#include "Lights.h"
#include "Renderer.h"
#include "resource.h"
#include "Console.h"

Spotlight::Spotlight(float3 pos,float3 dir,float3 color,float angle,bool enabled) {
	this->pos = pos;
	this->dir = dir;
	this->color = color;
	this->angle = angle;
	this->enabled = enabled;
	GetProjection();
	
	castshadows = false; // slow, so off by default
	scene->lights->AddSpotlight(this); // add to global dump
}
Spotlight::~Spotlight() {
	scene->lights->DeleteSpotlight(this);
}

float3 Spotlight::GetColor() {
	return color;
}
bool Spotlight::GetEnabled() {
	return enabled;
}
bool Spotlight::GetCastShadows() {
	return castshadows;
}
float3 Spotlight::GetDirection() {
	return dir;
}
float3 Spotlight::GetPosition() {
	return pos;
}
float Spotlight::GetAngle() {
	return angle;
}
void Spotlight::SetEnabled(bool enabled) {
	if(this->enabled != enabled) {
		scene->lights->BeginUpdate();
		
		this->enabled = enabled;
		
		scene->lights->EndUpdate();
	}
}
void Spotlight::SetColor(float3 color) {
	scene->lights->BeginUpdate();
	
	this->color = color;
	
	scene->lights->EndUpdate();
}
void Spotlight::SetCastShadows(bool castshadows) {
	if(this->castshadows != castshadows) {
		scene->lights->BeginUpdate();
		
		this->castshadows = castshadows;
		
		scene->lights->EndUpdate();
	}
}
void Spotlight::SetPosition(float3 pos) {
	scene->lights->BeginUpdate();
	
	this->pos = pos;
	GetProjection();
	
	scene->lights->EndUpdate();
}
void Spotlight::SetDirection(float3 dir) {
	scene->lights->BeginUpdate();
	
	this->dir = dir;
	GetProjection();
	
	scene->lights->EndUpdate();
}
void Spotlight::SetLookat(float3 pos) {
	scene->lights->BeginUpdate();
	
	this->dir = pos - this->pos;
	this->dir.Normalize();
	GetProjection();
	
	scene->lights->EndUpdate();
}
void Spotlight::SetAngle(float angle) {
	scene->lights->BeginUpdate();
	
	this->angle = angle;
	
	scene->lights->EndUpdate();
}
void Spotlight::GetProjection() {
	
	float3 lookat = pos + dir;
	float3 up = float3(0,1,0);
	
	// Weird fix
	if(dir.x == 0 && dir.z == 0) { // pure vertical direction
		lookat.x += 0.01;
	}
	
	D3DXMatrixLookAtLH(&matView,&pos,&lookat,&up);
	D3DXMatrixPerspectiveFovLH(&matProj,pi/2,1,0.1f,256);
	D3DXMatrixMultiply(&matViewProj,&matView,&matProj);
}
void Spotlight::Print() {
	
	console->Write("\r\n----- Info for class Spotlight -----\r\n\r\n");
	
	console->WriteVar("color",color);
	console->WriteVar("dir",dir);
	console->WriteVar("pos",pos);
	console->WriteVar("enabled",enabled);
	console->WriteVar("angle",angle);
	console->WriteVar("castshadows",castshadows);
	console->WriteVar("matView",matView);
	console->WriteVar("matProj",matProj);
	console->WriteVar("matViewProj",matViewProj);
	
	console->Write("\r\n----- End of info -----\r\n\r\n");
}
