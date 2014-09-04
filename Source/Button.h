#ifndef BUTTON_INCLUDE
#define BUTTON_INCLUDE

#include "Label.h"
#include "resource.h"
#include "TimeEvent.h"

class DLLIMPORT Button : public Label {
	public:
		Button(int left,int top,int width,int height,const char* text);
		~Button();
		
		float4 downcolor; // click
		float4 hotcolor; // mouse hover
		bool down;
		bool hot;		
		//TimeEvent* OnClick;
		void (*OnClick)(Component* sender);
		
		void DrawText(LPD3DXFONT font,LPD3DXSPRITE sprite);
};

#endif
