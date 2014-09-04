#include "Pointlight.h"
#include "Lights.h"

Pointlight::Pointlight(Lights* parent,float3 pos,float3 color,bool enabled) {
	this->pos = pos;
	this->color = color;
	this->enabled = enabled;
	
	castshadows = false; // not supported
	factory = parent; // this one recreates buffers when we change
}
Pointlight::~Pointlight() {
}

float3 Pointlight::GetColor() {
	return color;
}
float3 Pointlight::GetPosition() {
	return pos;
}
bool Pointlight::GetEnabled() {
	return enabled;
}
void Pointlight::SetColor(float3 color) {
	factory->BeginUpdate();
	
	this->color = color;
	
	factory->EndUpdate();
}
void Pointlight::SetPosition(float3 pos) {
	factory->BeginUpdate();
	
	this->pos = pos;
	
	factory->EndUpdate();
}
void Pointlight::SetEnabled(bool enabled) {
	if(this->enabled != enabled) {
		factory->BeginUpdate();
		
		this->enabled = enabled;
		
		factory->EndUpdate();
	}
}
