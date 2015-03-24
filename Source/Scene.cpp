#include "Scene.h"
#include "Objects.h" // lists
#include "Lights.h" // lists
#include "Timers.h" // lists
#include "Animations.h" // lists
#include "Heightmap.h" // ground stuff
#include "TimeEvent.h" // OnUpdateTime
#include "Object.h" // object, intersects, collision
#include "Parser.h" // LoadFromFile
#include "Dialogs.h" // ???
#include "Renderer.h" // shaders
#include "Bezier.h" // ???
#include "Camera.h"
#include "FXShader.h"

Scene::Scene() {
	groundheight = NULL;

	// Create subparts of our scene
	objects = new Objects();
	lights = new Lights();
	timers = new Timers();
	animations = new Animations();

	// Let our children listen to game time changes
	OnUpdateTime = new TimeEvent();
	OnUpdateTime->AddPersistent(lights->StaticOnUpdateTime,lights);
	OnUpdateTime->AddPersistent(timers->StaticOnUpdateTime,timers);
	OnUpdateTime->AddPersistent(animations->StaticOnUpdateTime,animations);

	// Convert frametimes to game times
	Globals::renderer->OnRenderFrame->AddPersistent(StaticOnRenderFrame,this);

	// Apply frustrum culling when the camera moves
	Globals::camera->OnChange->AddPersistent(StaticOnCameraChange,this);

	// Recreate batches when the object database changes
	objects->OnUpdate->AddPersistent(StaticOnObjectsUpdate,this);
}
Scene::~Scene() {
	delete objects;
	delete lights;
	delete timers;
	delete animations;
	delete OnUpdateTime;
}
void Scene::LoadFromFile(const char* filename) {

	Globals::console->WriteBeginTimer("INFO: loading scene from file");

	// Reset everything
	BeginUpdate();
	{
		// Reset all
		Clear();

		// Get full path
		char fullpath[MAX_PATH];
		Utils::GetFullPath(filename,"Data\\Scenes",fullpath);

		// Execute script file
		Parser parser(true,false);
		parser.Execute(fullpath);
	}
	EndUpdate();

	Globals::console->WriteEndTimer("");
}
//void Scene::SaveToFile(const char* scnpath) {
//
//	// Bouw volledig pad op
//	char finalscnpath[MAX_PATH] = "";
//	GetFullPath(scnpath,"\\",finalscnpath);
//
//	// Probeer bestand aan te maken
//	FILE* scene = fopen(finalscnpath,"w");
//	if(scene == NULL) {
//		console->Write("Error saving scene %s\r\n",finalscnpath);
//		return;
//	}
//
//	// Sla de camera op
//	fprintf(scene,"SetCameraPos(%.4g,%.4g,%.4g)\n",
//		camera->pos.x,
//		camera->pos.y,
//		camera->pos.z);
//	fprintf(scene,"SetCameraAngleDeg(%.4g,%.4g)\n\n",
//		RadToDeg(camera->angleH),
//		RadToDeg(camera->angleV));

//	// Stel de tijd in
//	fprintf(scene,"SetTime(%02d,%02d)\n",renderer->GetTimeHours(),renderer->GetTimeMins());
//	fprintf(scene,"SetTimeMulti(%d)\n\n",renderer->GetTimeMulti());

////	// Sla de lichten op, zonlicht
////	if(lights->sunlight) {
////		fprintf(scene,"SetSunlight(%.4g,%.4g,%.4g)\n\n",
////			lights->sunlight->GetColor().x,
////			lights->sunlight->GetColor().y,
////			lights->sunlight->GetColor().z);
////	}
////
////	// Pointlights
////	for(unsigned int i = 0;i < lights->poslights.size();i++) {
////		Pointlight* thislight = lights->poslights[i];
////
////		fprintf(scene,"pointlight %.4g %.4g %.4g %.4g %.4g %.4g\n",
////			thislight->GetPosition().x,
////			thislight->GetPosition().y,
////			thislight->GetPosition().z,
////			thislight->GetColor().x,
////			thislight->GetColor().y,
////			thislight->GetColor().z);
////	}
////	if(lights->poslights.size() > 0) {
////		fprintf(scene,"\n");
////	}
////
////	// Zaklampen
////	for(unsigned int i = 0;i < lights->spotlights.size();i++) {
////		Spotlight* thislight = lights->spotlights[i];
////
////		fprintf(scene,"AddSpotlight(%.4g,%.4g,%.4g,%.4g,%.4g,%.4g,%.4g,%.4g,%.4g,%.4g)\n",
////			thislight->GetPosition().x,
////			thislight->GetPosition().y,
////			thislight->GetPosition().z,
////			thislight->GetDirection().x,
////			thislight->GetDirection().y,
////			thislight->GetDirection().z,
////			thislight->GetColor().x,
////			thislight->GetColor().y,
////			thislight->GetColor().z,
////			thislight->GetAngle());
////	}
////	if(lights->spotlights.size() > 0) {
////		fprintf(scene,"\n");
////	}
//
//	// Sla de objecten op
//	for(std::list<Object*>::iterator i = objects->begin();i != objects->end();i++) {
//		Object* thisobject = *i;
//
//		// Sla een plane op...
//		if(!strcmp(thisobject->model0->fullpath,"ProceduralPlane")) {
//
//			// wat nu???
//
//		} else { // sla een object op
//
//			fprintf(scene,"AddObject(\"NULL\",\"NULL\",%.4g,%.4g,%.4g,%.4g,%.4g,%.4g,%.4g)\n",
//			//	NULL,//thisobject->modelpath,
//			//	NULL,//thisobject->materialpath,
//				thisobject->GetTranslation().x,
//				thisobject->GetTranslation().y,
//				thisobject->GetTranslation().z,
//				0.0f,//thisobject->rotation.x,
//				0.0f,//thisobject->rotation.y,
//				0.0f,//thisobject->rotation.z,
//				thisobject->GetScaling().x);
//		}
//	}
//
//	fclose(scene);
//}
//Heightmap* Scene::AddHeightMap(const char* matpath,const char* mappath,float width,float minz,float maxz,unsigned int textiling) {
//
//	// Bouw volledig pad op
//	char finalmappath[MAX_PATH];
//	Utils::GetFullPath(mappath,"Data\\Textures",finalmappath);
//
//	// Store height information
//	if(!groundheight) {
//		groundheight = new Heightmap(finalmappath,minz,maxz,width);
//	} else {
//		groundheight->Load(finalmappath,minz,maxz); // reuse object
//	}
//
//	objects->AddPlane("Ground",matpath,float3(0,0,0),float3(0,0,0),width,groundheight->widthpixels,textiling,groundheight);
//
//	return groundheight;
//}
void Scene::Update() {
	objects->Update();
	lights->Update();
}
void Scene::Clear() {
	objects->Clear();
	lights->Clear();
	animations->Clear();
}
void Scene::OnLostDevice() {
	lights->OnLostDevice();
}
void Scene::OnResetDevice() {
	lights->OnResetDevice();
}
void Scene::BeginUpdate() {
	objects->BeginUpdate();
	lights->BeginUpdate();
}
void Scene::EndUpdate() {
	objects->EndUpdate();
	lights->EndUpdate();
}
void Scene::OnRenderFrame(float dt) {
	int multiplier = Globals::renderer->GetTimeMulti();
	if(multiplier > 0) {
		float gametimesec = dt / 1000.0f * multiplier;
		OnUpdateTime->Execute(gametimesec);
	}
}
void Scene::OnCameraChange(float t) {
	for(std::list<Object*>::iterator i = objects->begin(); i != objects->end(); i++) {
		Object* thisobject = *i;
		thisobject->SetVisible(Globals::camera->IsVisible(thisobject));
	}
}
void Scene::OnObjectsUpdate(float t) {

	// Remove all ranges
	for(unsigned int i = 0; i < Globals::renderer->shaders.size(); i++) {
		Globals::renderer->shaders[i]->Reset();
	}

	// Find first and last item belonging to each shader
	for(std::list<Object*>::iterator i = objects->begin(); i != objects->end(); i++) {
		Object* thisobject = *i;
		FXShader* objectshader = Globals::renderer->shaders[thisobject->material->shaderindex];

		// Always set ending to one more than current item
		objectshader->SetEnd(next(i)); // exclusive ending

		// Move start only once
		if(!objectshader->HasValidRange()) {
			objectshader->SetBegin(i);
		}
	}
}
Collision Scene::IntersectScene(float3 worldpos,float3 worlddir) {

	Collision result = {0};

	// For each object in scene, do a fast sphere test
	std::vector<Object*> spherepass;

	// Bekijk welke objecten er achter het punt zitten, door ray door bounding sphere heen te trekken
	for(ObjectIterator i = objects->begin(); i != objects->end(); i++) {
		Object* thisobject = *i;
		if(thisobject->IntersectSphere(worldpos,worlddir).object) {
			spherepass.push_back(thisobject);
		}
	}

	float minimumtime = 1.0f;

	// Now, for each object in spherepass, do a slow triangle test
	for(unsigned int i = 0; i < spherepass.size(); i++) {
		Object* thisobject = spherepass[i];
		Collision intersect = thisobject->IntersectModel(worldpos,worlddir);
		if(intersect.object) {
			if(intersect.t >= 0.0f && intersect.t < minimumtime) {
				result.object = intersect.object;
				result.point = intersect.point;
				result.t = intersect.t;
				minimumtime = intersect.t;
			}
		}
	}
	return result;
}
Object* Scene::AddObject(const char* name) {
	return objects->Add(new Object(name));
}
Object* Scene::AddObject(const char* objectpath,const float3& pos,const float3& rot,float scale) {
	float4x4 rotationmatrix;
	rotationmatrix.EulerRotation(rot);
	return objects->Add(new Object(objectpath,pos,rotationmatrix,scale));
}
Object* Scene::AddObject(const char* objectpath,const float3& pos,const float4x4& rot,float scale) {
	return objects->Add(new Object(objectpath,pos,rot,scale));
}
Object* Scene::AddObject(const char* name,const char* modelpath,const char* materialpath,const float3& pos,const float3& rot,float scale) {
	float4x4 rotationmatrix;
	rotationmatrix.EulerRotation(rot);
	return objects->Add(new Object(name,modelpath,materialpath,pos,rotationmatrix,scale));
}
Object* Scene::AddObject(const char* name,const char* modelpath,const char* materialpath,const float3& pos,const float4x4& rot,float scale) {
	return objects->Add(new Object(name,modelpath,materialpath,pos,rot,scale));
}
void Scene::DeleteObject(Object* object) {
	objects->Delete(object);
}
Timer* Scene::AddTimer() {
	timers->Add(new Timer());
}
Timer* Scene::AddTimer(int miliseconds, bool start) {
	timers->Add(new Timer(miliseconds,start));
}
void Scene::DeleteTimer(Timer* timer) {
	timers->Delete(timer);
}
void Scene::StaticOnRenderFrame(void* sender,double data) {
	((Scene*)sender)->OnRenderFrame(data);
}
void Scene::StaticOnCameraChange(void* sender,double data) {
	((Scene*)sender)->OnCameraChange(data);
}
void Scene::StaticOnObjectsUpdate(void* sender,double data) {
	((Scene*)sender)->OnObjectsUpdate(data);
}
