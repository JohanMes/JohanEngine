#ifndef PANEL_INCLUDE
#define PANEL_INCLUDE

#include <vector>
using std::vector;
#include "float4.h"
#include "float4x4.h"

class Model;

enum ComponentType {
    ctBase,
    ctButton,
    ctLabel,
    ctWindow,
    ctEdit,
    ctBevel,
    ctDropdown,
};

#if BUILDING_DLL
#define DLLIMPORT __declspec(dllexport)
#else
#define DLLIMPORT __declspec(dllimport)
#endif

class DLLIMPORT Component {
	private:
		bool visible;
		float4x4 matWorld;
		Component* parent;
		void ComputeWorldTransform();
	public:
		Component(int left,int top,int width,int height);
		~Component();
		void GetRect(RECT* result);
		void CreatePlane();
		void Move(int dx,int dy);
		void SetPos(int x,int y);
		void Resize(int width,int height);
		void AddChild(Component* child);
		void Toggle();
		bool IsVisible();
		void Show(bool value);
		float4x4 GetWorldTransform();
		Component* GetParent();
		void OnResetDevice();

		// TODO: private?
		Model* plane;
		int left;
		int top;
		int width;
		int height;
		int absleft;
		int abstop;
		float4 backcolor;
		ComponentType type;
		vector<Component*> children;
		
		// Events
		void (*OnShow)(Component* Sender); // TODO: TimeEvent?
};

#endif
