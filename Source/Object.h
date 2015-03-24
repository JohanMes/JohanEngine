#ifndef OBJECT_INCLUDE
#define OBJECT_INCLUDE

#include <cfloat>
#include <cstdio>
#include <vector>
using std::vector;
#include <list>
using std::list;
#include <string>
using std::string;
#include "float3.h"
#include "float4x4.h"
#include "Material.h"

#if BUILDING_DLL
#define DLLIMPORT __declspec(dllexport)
#else
#define DLLIMPORT __declspec(dllimport)
#endif

class Object;
class Model;
class Animation;

struct DLLIMPORT Collision {
	Object* object;
	float3 point;
	float t;
};

class DLLIMPORT DetailLevel {
	public:
		DetailLevel(Model* model,float maxdistance) {
			this->model = model;
			this->maxdistance = maxdistance;
		}
		Model* model;
		float maxdistance;
};

class Object;
typedef std::list<Object*>::iterator ObjectIterator;

class DLLIMPORT Object {
	private:
		friend class Scene; // constructors? TODO: move to Objects
		friend class Objects; // bufferlocation
		string name;
		Animation* animation; // remove when deleting object
		vector<DetailLevel*> detaillevels;
		ObjectIterator bufferlocation;
		float3 worldcenter; // calculated on Update
		float worldr; // calculated on Update
		bool visible; // calculated on Update and camera change
		bool worldmatrixoverride; // if true, rotate, translate, scale not valid
		float4x4 rotationmatrix;
		float4x4 translationmatrix;
		float4x4 scalingmatrix;
		float4x4 worldmatrix;
		float4x4 worldinversematrix;
		unsigned int updatecount;
		void Update();
		Object(const char* name);
		Object(const char* objectpath,const float3& pos,const float4x4& rot,float scale); // from .object file
		Object(const char* name,const char* modelpath,const char* materialpath,const float3& pos,const float4x4& rot,float scale); // from .obj + .mtl
		~Object();
	public:
		void LoadFromFile(const char* objectpath);
		Collision IntersectSphere(float3 worldpos,float3 worlddir);
		Collision IntersectModel(float3 worldpos,float3 worlddir);
		Animation* GetAnimation();
		const char* GetName();
		Model* GetBoundingModel();
		float3 GetTranslation();
		float4x4 GetTranslationMatrix();
		float3 GetScaling();
		float4x4 GetScalingMatrix();
		//float3 GetRotation();
		float4x4 GetRotationMatrix();
		float4x4 GetWorldTransformMatrix();
		float4x4 GetInvWorldTransformMatrix();
		void ClearDetailLevels();
		void AddDetailLevel(const char* modelpath);
		void AddDetailLevel(Model* model);
		void AddDetailLevel(Model* model,float maxdistance);
		void AddDetailLevel(DetailLevel* detaillevel);
		DetailLevel* GetDetailLevel(float distance);
		void SetTranslation(const float3& value);
		void SetTranslation(const float4x4& value);
		void SetRotation(const float3& value);
		void SetRotationDeg(const float3& value);
		void SetRotation(const float4x4& value);
		void SetScaling(float value);
		void SetWorldTransForm(const float4x4& value);
		void Move(const float3& value);
		void SetName(const char* value);
		void SetAnimation(Animation* animation);
		void BeginUpdate();
		void EndUpdate();
		void Print();
		void Reset();
		float3 GetWorldCenter();
		float GetWorldRadius();
		bool IsVisible();
		void SetVisible(bool value);
		bool IsWorldMatrixOverridden();
		
		// Events
		void (*OnClick)(Object* Sender);

		// TODO: private?
		bool castshadows;
		Model* boundingmodel;
		Material* material;
};

bool CompareObject(Object* a,Object* b);

#endif
