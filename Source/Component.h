#ifndef PANEL_INCLUDE
#define PANEL_INCLUDE

#include <vector>

#include "Models.h"
#include "float4.h"
#include "float4x4.h"

enum ComponentType {
	ctBase,
	ctButton,
	ctLabel,
	ctWindow,
	ctEdit,
	ctBevel,
	ctDropdown,
};

class DLLIMPORT Component {
	public:
		Component(int left,int top,int width,int height);
		~Component();
		
		Component* parent;
		int left;
		int top;
		int width;
		int height;
		int absleft;
		int abstop;
		float4 backcolor;
		ComponentType type;
		bool visible; // private maken?
		float4x4 matWorld;
		
		// Handigheidjes
		void GetRect(RECT* result);
		
		// 3D representation
		Model* plane;
		void CreatePlane();
		void Move(int dx,int dy);
		void SetPos(int x,int y);
		void Resize(int width,int height);
		
		// Child windows with position relatieve to us
		void AddChild(Component* child);
		std::vector<Component*> children;
		
    	// Nu kunnen we dingen doen als we tevoorschijn komen
    	void (*OnShow)(Component* Sender); // TODO: TimeEvent maken?
		
		// Visibility
		void Toggle();
		bool IsVisible();
		void Show(bool value);
		void OnResetDevice();
};

#endif
