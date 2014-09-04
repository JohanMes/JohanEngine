#include "Lights.h"
#include "Console.h"
#include "Renderer.h"
#include "Options.h"

Lights::Lights() {
	updatecount = 0;
	
	ambientlight = 0.05f;
	
	FXDirlightshadowindex = new FXVariable("dirlightshadowindex");
	FXNumdirlights = new FXVariable("numdirlights");
	FXDirlights = new FXVariable("dirlights");
	FXPointlightshadowindex = new FXVariable("pointlightshadowindex");
	FXNumpointlights = new FXVariable("numpointlights");
	FXPointlights = new FXVariable("pointlights");
	FXSpotlightshadowindex = new FXVariable("spotlightshadowindex");
	FXNumspotlights = new FXVariable("numspotlights");
	FXSpotlights = new FXVariable("spotlights");
	FXFogcolor = new FXVariable("fogcolor");
	FXAmbientLight = new FXVariable("lightambient");
	
	dirlightshadow = NULL;
	pointlightshadow = NULL;
	spotlightshadow = NULL;
	
	// Make sure we can release safely
	ShadowTex = new RenderTarget();
	castshadows = false; // true if textures above are set
}
Lights::~Lights() {
	BeginUpdate(); // make sure Update doesn't happen
	Clear(); // clear light arrays
	delete ShadowTex;
}

void Lights::SetAmbientlight(float ambientlight) {
	this->ambientlight = ambientlight;
	FXAmbientLight->Set(ambientlight); // immediately update, don't wait for Update()
}
Dirlight* Lights::AddDirlight(Dirlight* dirlight) {
	if((int)dirlights.size() >= maxdirlights) {
		console->Write("Cannot have more than %u active directional lights!\r\n",maxdirlights);
	}
	BeginUpdate();
	dirlights.push_back(dirlight);
	dirlight->bufferlocation = --dirlights.end();
	EndUpdate();
	return dirlight;
}
Pointlight* Lights::AddPointlight(float3 pos,float3 color,bool enabled) {
	if((int)pointlights.size() >= maxpointlights) {
		console->Write("Cannot have more than %u point lights!\r\n",maxpointlights);
	}
	
	BeginUpdate();
	
	Pointlight* newlight = new Pointlight(this,pos,color,enabled);
	
	// Sla op waar we hem achterlaten
	pointlights.push_back(newlight);
	newlight->bufferlocation = --pointlights.end();
	
	EndUpdate();

	return newlight;
}
Spotlight* Lights::AddSpotlight(Spotlight* spotlight) {
	if((int)spotlights.size() >= maxspotlights) {
		console->Write("Cannot have more than %u active spot lights!\r\n",maxspotlights);
	}
	BeginUpdate();
	spotlights.push_back(spotlight);
	spotlight->bufferlocation = --spotlights.end();	
	EndUpdate();
	return spotlight;
}

void Lights::DeleteDirlight(Dirlight* thislight) {
	if(thislight) {
		BeginUpdate();
		dirlights.erase(thislight->bufferlocation);
		EndUpdate();
	}
}
void Lights::DeletePointlight(Pointlight* thislight) {
	if(thislight) {
		BeginUpdate();
		pointlights.erase(thislight->bufferlocation);
		delete thislight;
		EndUpdate();
	}
}
void Lights::DeleteSpotlight(Spotlight* thislight) {
	if(thislight) {
		BeginUpdate();
		spotlights.erase(thislight->bufferlocation);
		EndUpdate();
	}
}

void Lights::Clear() {
	BeginUpdate();
	
	// Gooi dirlights weg
	std::list<Dirlight*>::iterator i = dirlights.begin();
	while(i != dirlights.end()) { // Make sure we can remove items while iterating
		std::list<Dirlight*>::iterator next = std::next(i); // Store next item iterator (as current will be invalidated)
		delete *i; // Delete current
		i = next; // Goto next
	}
	
	// Gooi pointlights weg
	for(std::list<Pointlight*>::iterator i = pointlights.begin();i != pointlights.end();i++) {
		delete *i;
	}
	pointlights.clear();
	
	// Gooi spotlights weg
	std::list<Spotlight*>::iterator k = spotlights.begin();
	while(k != spotlights.end()) { // Make sure we can remove items while iterating
		std::list<Spotlight*>::iterator next = std::next(k); // Store next item iterator (as current will be invalidated)
		delete *k; // Delete current
		k = next; // Goto next
	}

	EndUpdate();
}

void Lights::Update() {

	memset(gpudirlights,0,sizeof(GPUDIRLIGHT)*maxdirlights);
	memset(gpupointlights,0,sizeof(GPUPOINTLIGHT)*maxpointlights);
	memset(gpuspotlights,0,sizeof(GPUSPOTLIGHT)*maxspotlights);
	
	int numdirlights = 0;
	int numpointlights = 0;
	int numspotlights = 0;
	
	dirlightshadow = NULL;
	pointlightshadow = NULL;
	spotlightshadow = NULL;
	
	int dirlightshadowindex = -1;
	int pointlightshadowindex = -1;
	int spotlightshadowindex = -1;

	// Only recreate buffers on change
	bool oldcastshadows = castshadows;
	castshadows = false;
	
	for(std::list<Dirlight*>::iterator i = dirlights.begin();i != dirlights.end();i++) {
		Dirlight* light = *i;
		
		if(light->GetEnabled()) {
			gpudirlights[numdirlights].color = light->GetColor();
			gpudirlights[numdirlights].direction = light->GetDirection();

			// last light to cast shadows is enabled
			if(light->GetCastShadows()) {
				castshadows = true;
				dirlightshadow = light;
				dirlightshadowindex = numdirlights;
			}
			
			numdirlights++;
			if(numdirlights == maxdirlights) {
				break;
			}
		}
	}
	
	for(std::list<Pointlight*>::iterator i = pointlights.begin();i != pointlights.end();i++) {
		Pointlight* light = *i;
		
		if(light->GetEnabled()) {
			gpupointlights[numpointlights].color = light->GetColor();
			gpupointlights[numpointlights].position = light->GetPosition();
			
			// TODO: implement point light shadows

			numpointlights++;
			if(numpointlights == maxpointlights) {
				break;
			}
		}
	}

	for(std::list<Spotlight*>::iterator i = spotlights.begin();i != spotlights.end();i++) {
		Spotlight* light = *i;
		
		if(light->GetEnabled()) {
			gpuspotlights[numspotlights].color = light->GetColor();
			gpuspotlights[numspotlights].position = light->GetPosition();			
			gpuspotlights[numspotlights].direction = light->GetDirection();
			gpuspotlights[numspotlights].angle = light->GetAngle();
			
			// Last light to cast shadows is enabled
			if(light->GetCastShadows()) {
				castshadows = true;
				spotlightshadow = light;
				spotlightshadowindex = numspotlights;
			}
			
			numspotlights++;
			if(numspotlights == maxspotlights) {
				break;
			}
		}
	}

	// Set light data in GPU cache
	FXDirlightshadowindex->Set(&dirlightshadowindex);
	FXPointlightshadowindex->Set(&pointlightshadowindex);
	FXSpotlightshadowindex->Set(&spotlightshadowindex);

	FXNumdirlights->Set(&numdirlights);
	FXNumpointlights->Set(&numpointlights);
	FXNumspotlights->Set(&numspotlights);
	
	FXDirlights->Set(&gpudirlights);
	FXPointlights->Set(&gpupointlights);
	FXSpotlights->Set(&gpuspotlights);
	
	// Did this option change? Recreate/Remove buffer
	if(oldcastshadows != castshadows) {
		if(castshadows && options->shadowmapsize > 0) {
			OnResetDevice(); // create new buffers...
		} else {
			OnLostDevice(); // remove unused buffers
		}
	}
}
void Lights::BeginUpdate() {
	updatecount++;
}
void Lights::EndUpdate() {
	updatecount--;
	if(updatecount == 0) {
		Update();
	}
}
void Lights::OnLostDevice() {
	ShadowTex->Clear();
}
void Lights::OnResetDevice() {
	unsigned int size = options->shadowmapsize;
	if(size > 0 && castshadows) {
		ShadowTex->Create(size,size,D3DFMT_R32F,true);
	}
}
void Lights::OnUpdateTime(float dt) {
	
	// Update matrices every frame
	if(dirlightshadow) { // sun changes all the time, so keep fitting frustum to scene
		dirlightshadow->GetProjection(); // fit frustum to scene, update every time
		matLightView = dirlightshadow->matView;
		matLightProj = dirlightshadow->matProj;
		matLightViewProj = dirlightshadow->matViewProj;	
	} else if(spotlightshadow) {
		// this one updates its own matrices
		matLightView = spotlightshadow->matView;
		matLightProj = spotlightshadow->matProj;
		matLightViewProj = spotlightshadow->matViewProj;		
	}
}
void Lights::Print() {
	
	console->Write("\r\n----- Info for class Lights -----\r\n\r\n");
	
	console->WriteVar("dirlightshadow",dirlightshadow);
	console->WriteVar("pointlightshadow",pointlightshadow);
	console->WriteVar("spotlightshadow",spotlightshadow);
	
//	console->WriteVar("ShadowTex",ShadowTex);
	console->WriteVar("castshadows",castshadows);
	
	console->WriteVar("matLightView",matLightView);
	console->WriteVar("matLightProj",matLightProj);
	console->WriteVar("matLightViewProj",matLightViewProj);
	
	console->WriteVar("dirlights.size()",(int)dirlights.size());
	for(std::list<Dirlight*>::iterator i = dirlights.begin();i != dirlights.end();i++) {
		(*i)->Print();
	}

	console->WriteVar("pointlights.size()",(int)pointlights.size());
	for(std::list<Pointlight*>::iterator i = pointlights.begin();i != pointlights.end();i++) {
	//	(*i)->Print(); // TODO
	}
	
	console->WriteVar("spotlights.size()",(int)spotlights.size());
	for(std::list<Spotlight*>::iterator i = spotlights.begin();i != spotlights.end();i++) {
		(*i)->Print();
	}
}
void Lights::StaticOnUpdateTime(void* sender,double data) {
	((Lights*)sender)->OnUpdateTime(data);
}
