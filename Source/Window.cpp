#include "Window.h"
#include "Renderer.h"

Window::Window(int left,int top,int width,int height,const char* text) : Label(left,top,width,height,text,false) {
	this->backcolor = float4(0.3f,0.3f,0.3f,0.8f);
	this->type = ctWindow;
	this->dragging = false;
	
	// Show for the first time if we can
	this->visible = false;
	
	if(ui) {
		ui->AddComponent(this);
	}
}

Window::~Window() {
	if(ui) {
		ui->RemoveComponent(this);
	}
}

void Window::DrawText(LPD3DXFONT font,LPD3DXSPRITE sprite) {
	
	// Center it a bit
	RECT textrect;
	GetRect(&textrect);
	InflateRect(&textrect,0,-2);
	
	// And draw
	sprite->Begin(D3DXSPRITE_ALPHABLEND);
	font->DrawText(sprite,caption.c_str(),std::min(1024,(int)caption.length()),&textrect,DT_NOCLIP|DT_CENTER,D3DCOLOR_XRGB(255,255,255));
	sprite->End();
}
