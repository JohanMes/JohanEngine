#include <cstdio>
#include "Label.h"

Label::Label(int left,int top,int width,int height,const char* text,bool parentbg) : Component(left,top,width,height) {
	if(!parentbg) {
		backcolor = float4(0.1f,0.1f,0.1f,1.0f);
	}

	type = ctLabel;
	
	SetCaption(text);
}

Label::~Label() {
}

void Label::SetCaption(float value,const char* format) {
	char tmp[128];
	snprintf(tmp,128,format,value);
	SetCaption(tmp);
}

void Label::SetCaption(const char* text) {
	caption = text;
}

void Label::AddCaption(const char* text) {
	caption += text;
}

void Label::AddChar(char key) {
	caption += key;
}

void Label::RemoveChar() {
	if(caption.size() > 0) {
		caption.erase(--caption.end());
	}
}

const char* Label::GetCaption() {
	return caption.c_str();
}

void Label::DrawText(LPD3DXFONT font,LPD3DXSPRITE sprite) {
	
	RECT textrect;
	GetRect(&textrect);
	InflateRect(&textrect,-1,-1);

	// And draw
	sprite->Begin(D3DXSPRITE_ALPHABLEND);
	font->DrawText(sprite,caption.c_str(),std::min(1024,(int)caption.length()),&textrect,0,D3DCOLOR_XRGB(255,255,255));// draw only 1024 chars
	sprite->End();
}
