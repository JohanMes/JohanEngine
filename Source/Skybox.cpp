#include "Skybox.h"
#include "Lights.h"
#include "Camera.h"
#include "Renderer.h"

Skybox::Skybox() {
	skybox = NULL;
	sunobject = NULL;
	sunlight = NULL;
	skyboxradius = 0;
	updateevent = scene->OnUpdateTime->Add(StaticOnUpdateTime,this);
}
Skybox::~Skybox() {
	scene->OnUpdateTime->Delete(updateevent);
	Clear();
}

void Skybox::Clear() {
	delete skybox;
	delete sunobject;
	delete sunlight;
	scene->lights->SetAmbientlight(0); // set ambient term to 0
}
void Skybox::SetSkybox(const char* modelpath,const char* materialpath) {
	delete skybox; // TODO: reuse
	skybox = new Object("Skybox",modelpath,materialpath,0,0,1);
	skybox->castshadows = false;
}
void Skybox::SetSunlight(const char* modelpath,const char* materialpath) {
	if(sunobject) {
		delete sunobject; // TODO: reuse
	}
	sunobject = new Object("Sunobject",modelpath,materialpath,0,0,1);
	sunobject->castshadows = false;
	
	// Add light source too
	if(!sunlight) {
		sunlight = new Dirlight(
			float3(0,0,0),
			float3(1.4,1.4,1.2),
			false);
		sunlight->SetCastShadows(true);
	}
	
	// Add some skylight
	scene->lights->SetAmbientlight(0.05);
}
void Skybox::OnLostDevice() {
}
void Skybox::OnResetDevice() {
}
void Skybox::OnUpdateTime(double dt) {
	
	scene->lights->BeginUpdate();
	
	// default, 4000 is max view distance by default
	skyboxradius = 4000.0f;
	if(skybox) {
		skyboxradius = skybox->worldr;
	}
	
	// Beweeg zonlicht	
	sunlight->SetDirection(float3(
		-0.65f,
		-cos((2.0f*pi/(60.0f * 60 * 24))*renderer->GetTime()),
		-sin((2.0f*pi/(60.0f * 60 * 24))*renderer->GetTime())));
	
	// roteer plane zonobject naar camera
	sunobject->matWorld = camera->matViewInverse; 
	
	// Behoud translate
	float3 worldpos = 0.95f*sunlight->GetDirection()*skyboxradius + camera->pos;
	sunobject->matWorld._41 = worldpos.x;
	sunobject->matWorld._42 = worldpos.y;
	sunobject->matWorld._43 = worldpos.z;
	
	// Onthoud ook waar het midden zit
	sunobject->worldcenter = sunobject->worldcenter.Transform(sunobject->matWorld);
	
	// Skybox centreren op camerapos
	if(skybox) {
		skybox->SetTranslation(camera->pos);
	}
	
	// Update mixing between day time and nighttime texture
	float sunheight = sunlight->GetDirection().y;
	if(skybox) {
		skybox->material->mixer = float3(
			std::max(0.0f,1.5f*sunheight + 0.4f),
			std::max(0.0f,-sunheight + 0.8f),
			0);
	}
	
	// Disable it during the night
	if(sunheight < 0) {
		sunlight->SetEnabled(false);
	} else {
		sunlight->SetEnabled(true);
	}
	
	scene->lights->EndUpdate();
}
void Skybox::StaticOnUpdateTime(void* sender,double data) {
	((Skybox*)sender)->OnUpdateTime(data);
}
