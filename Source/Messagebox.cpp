#include "Messagebox.h"
#include "renderer.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	Banana
*/
Messagebox::Messagebox(const char* text) : Window(0,0,100,100,"Message") {
	
	// Get text extent by dummy drawing
	float2 size = renderer->GetUITextExtent(text);
	size.x += 10; // border
	size.y += 50 + 25; // border + button + border
	
	// Set minimum size
	size.x = std::max(100.0f,size.x);
	size.y = std::max(100.0f,size.y);
	Resize(size.x,size.y);
	
	// Center window
	float2 topleft = renderer->GetCenteringCorner(size);
	Move(topleft.x,topleft.y);
	
	lbltext = new Label(5,25,width-10,height-55,text,true);
	AddChild(lbltext);

	okbtn = new Button(width/2 - 20,height - 25,40,20,"OK");
	okbtn->OnClick = OKClick;
	AddChild(okbtn);
	
	MessageBeep(MB_ICONINFORMATION);
	Show(1);
}

Messagebox::~Messagebox() {
}

void Messagebox::OKClick(Component* sender) {
	delete ((Messagebox*)sender->parent); // commit suicide
}
