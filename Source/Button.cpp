#include "Button.h"

Button::Button(int left,int top,int width,int height,const char* text) : Label(left,top,width,height,text,false) {

	backcolor = float4(0.4f,0.4f,0.4f,1.0f);
	downcolor = float4(0.0f,0.0f,0.0f,1.0f);
	hotcolor = float4(0.45f,0.45f,0.45f,1.0f);
	
	OnClick = NULL;
	
	down = false;
	hot = false;
	
	type = ctButton;
}

Button::~Button() {
}

void Button::DrawText(LPD3DXFONT font,LPD3DXSPRITE sprite) {
	
	// Center it a bit
	RECT textrect;
	GetRect(&textrect);
	
	sprite->Begin(D3DXSPRITE_ALPHABLEND);
	font->DrawText(sprite,caption.c_str(),std::min(1024,(int)caption.length()),&textrect,DT_NOCLIP|DT_CENTER|DT_VCENTER,D3DCOLOR_XRGB(255,255,255));
	sprite->End();
}
