#include <windowsx.h>

#include "Interface.h"
#include "Renderer.h"

Interface::Interface() {
	hotcomponent = NULL;
	dragcomponent = NULL;
	downcomponent = NULL;
	focuscomponent = NULL;
	
	mousepos.x = 0;
	mousepos.y = 0;
	lmousedown = false; // Key van maken?
	mmousedown = false;
	rmousedown = false;
	
	OnKeyChar = NULL;
	OnKeyDown = NULL;
	OnKeyUp = NULL;
	OnMouseWheel = NULL;
	OnMouseMove = NULL;
	OnMouseDown = NULL;
	OnMouseUp = NULL;
}
Interface::~Interface() {
	// Gooi windows weg
	for(unsigned int i = 0;i < componentlist.size();i++) {
		delete componentlist[i];
	}
	
	// Gooi knoppen weg
	for(unsigned int i = 0;i < keylist.size();i++) {
		delete keylist[i];
	}
}
Component* Interface::GetComponentAt(int left,int top) {
	POINT testpoint = {left,top};
	RECT testrect, addrect;
	
	std::vector<Component*> children = componentlist;
	Component* result = NULL;
	
	unsigned int i = 0;
	while(i < children.size()) {
		
		Component* thiscomponent = children[i];
		
		if(thiscomponent->IsVisible()) {

			thiscomponent->GetRect(&testrect);
			
			// Increase size of test rect...
			for(unsigned int j = 0;j < thiscomponent->children.size();j++) {
				
				Component* thischild = thiscomponent->children[j];
				
				thischild->GetRect(&addrect);
				
				// Support children that reach outside of the parent rect...	
				UnionRect(&testrect,&testrect,&addrect);
			}
			
			// Deze parent moet doorzocht worden...
			if(PtInRect(&testrect,testpoint)) {
				
				// Nu gaan we testen of we wellicht op childs geklikt hebben...
				
				if(thiscomponent->children.size() > 0) { // probeer nu deze lijst
					children = thiscomponent->children;
					result = thiscomponent; // mochten de children niks opleveren, dan deze returnen
					i = 0;
					continue;
				} else { // done, found topmost item
					return thiscomponent; 	
				}
			}
		}
		i++;
	}
	return result;
}

void Interface::AddComponent(Component* thiscomponent) {
	componentlist.push_back(thiscomponent);
}
void Interface::RemoveComponent(Component* thiscomponent) {
	for(unsigned int i = 0;i < componentlist.size();i++) {
		if(componentlist[i] == thiscomponent) {
			componentlist.erase(componentlist.begin() + i);
			break; // duplicates are not allowed
		}
	}
}

void Interface::AddKey(Key* thiskey) {
	keylist.push_back(thiskey);
}
void Interface::RemoveKey(Key* thiskey) {
	for(unsigned int i = 0;i < keylist.size();i++) {
		if(keylist[i] == thiskey) {
			keylist.erase(keylist.begin() + i);
			break; // duplicates are not allowed
		}
	}
}
void Interface::OnLostDevice() {
	// alles is managed...
}
void Interface::OnResetDevice() {
	
	// Verschuif de top level planes (regelen eigen children)
	for(unsigned int i = 0;i < componentlist.size();i++) {
		if(componentlist[i]->IsVisible()) {
			componentlist[i]->OnResetDevice();
		}
	}
}
bool Interface::OnMessage(HWND hwnd,UINT Message,WPARAM wParam,LPARAM lParam) {
	switch(Message) {
		case WM_CHAR: {
			DoKeyChar(wParam,lParam);
			break;
		}
		case WM_KEYUP: {
			DoKeyUp(wParam,lParam);
			break;
		}
		case WM_KEYDOWN: {
			DoKeyDown(wParam,lParam);
			break;
		}
		case WM_SYSKEYDOWN: {
			DoKeyDown(wParam,lParam); // VK_F10
			break;
		}
		case WM_MOUSEWHEEL: {
			DoMouseWheel(wParam,lParam);
			break;
		}
		case WM_LBUTTONDOWN: {
			DoMouseDown(MK_LBUTTON,lParam);
			break;
		}
		case WM_LBUTTONUP: {
			DoMouseUp(MK_LBUTTON,lParam);
			break;
		}
		case WM_MBUTTONDOWN: {
			DoMouseDown(MK_MBUTTON,lParam);
			break;
		}
		case WM_MBUTTONUP: {
			DoMouseUp(MK_MBUTTON,lParam);
			break;
		}
		case WM_RBUTTONDOWN: {
			DoMouseDown(MK_RBUTTON,lParam);
			break;
		}
		case WM_RBUTTONUP: {
			DoMouseUp(MK_RBUTTON,lParam);
			break;
		}
		case WM_MOUSEMOVE: {
			DoMouseMove(wParam,lParam);
			break;
		}
		case WM_SIZE: {
			if(renderer) {
				renderer->UpdateBufferSize();	
			}
			break;
		}
		default: {
			return false; // not processed...
		}
	}
	return true; // message processed...
}
void Interface::DoKeyChar(WPARAM wParam,LPARAM lParam) {
	
	bool handled = false;
	
	if(focuscomponent) {
		Edit* focused = (Edit*)focuscomponent;
		switch(wParam) {
			case VK_RETURN: {
				if(focused->OnReturn) {
					focused->OnReturn(NULL);
				}
				break;
			}
			case VK_BACK: {
				focused->RemoveChar();
				break;
			}
			default: {
				focused->AddChar((char)wParam);			
			}
		}
		handled = true;
	}
	
	// Propagate
	if(OnKeyChar) {
		OnKeyChar(wParam,lParam,handled);
	}
}
void Interface::DoKeyDown(WPARAM wParam,LPARAM lParam) {
	
	bool handled = false;
	
	// Geen focus ergens? Stuur message door naar handler
	if(!focuscomponent) {
		for(unsigned int i = 0;i < keylist.size();i++) {
			Key* thiskey = keylist[i];
			if(thiskey->key == wParam) {
				thiskey->Press();
				handled = true;
			}
		}
	} else if(focuscomponent->type == ctEdit) {
		handled = true;
	}
	
	// Propagate
	if(OnKeyDown) {
		OnKeyDown(wParam,lParam,handled);
	}
}
void Interface::DoKeyUp(WPARAM wParam,LPARAM lParam) {
	
	bool handled = false;
	
	// Geen focus ergens? Stuur message door naar handler
	if(!focuscomponent) {
		for(unsigned int i = 0;i < keylist.size();i++) {
			Key* thiskey = keylist[i];
			if(thiskey->key == wParam) {
				thiskey->Release();
				handled = true;
			}
		}
	} else if(focuscomponent->type == ctEdit) {
		handled = true;
	}
	
	// Propagate
	if(OnKeyUp) {
		OnKeyUp(wParam,lParam,handled);
	}
}
void Interface::DoMouseWheel(WPARAM wParam,LPARAM lParam) {
	
	// Niets, niks hardcoden
	
	// Propagate
	if(OnMouseWheel) {
		OnMouseWheel(wParam,lParam);
	}
}
void Interface::DoMouseMove(WPARAM wParam,LPARAM lParam) {
	int tmpx = GET_X_LPARAM(lParam);
	int tmpy = GET_Y_LPARAM(lParam);
	
	int dx = tmpx - mousepos.x;
	int dy = tmpy - mousepos.y;
	
	mousepos.x = tmpx;
	mousepos.y = tmpy;
	
	// Remove old hot track
	if(hotcomponent && hotcomponent->type == ctButton) {
		((Button*)hotcomponent)->hot = false;
	}
	
	// Get new component under mouse
	hotcomponent = GetComponentAt(tmpx,tmpy);
	
	// Set new hot track
	if(hotcomponent && hotcomponent->type == ctButton) {
		((Button*)hotcomponent)->hot = true;
	}

	if(lmousedown) {
		
		// Stop clicking when the mouse moves off a button
		if(downcomponent && downcomponent != hotcomponent) {
			((Button*)downcomponent)->down = false;
			downcomponent = NULL;
		}

		// Drag windows OR move the Camera
		if(dragcomponent) {
			dragcomponent->Move(dx,dy); // deze eet het bericht op
		}
	}
	
	// Always propagate
	if(OnMouseMove) {
		OnMouseMove(wParam,lParam,dx,dy);
	}
}

void Interface::DoMouseDown(WPARAM wParam,LPARAM lParam) {
	
	bool handled = false; // click was above UI
	
	// Don't use LOWORD, HIWORD to support multimonitor
	mousepos.x = GET_X_LPARAM(lParam);
	mousepos.y = GET_Y_LPARAM(lParam);

	switch(wParam) {
		case MK_LBUTTON: {
			lmousedown = true;
			
			Component* clicked = GetComponentAt(mousepos.x,mousepos.y);
			
			// Disable old focus
			if(focuscomponent && focuscomponent != clicked) {
				((Edit*)focuscomponent)->focused = false;
				focuscomponent = NULL;
			}
			
			// Send message to interface
			if(clicked) {
				if(clicked->type == ctButton) { // Check if a button received a click
					downcomponent = clicked;
					((Button*)downcomponent)->down = true;
				} else if(clicked->type == ctWindow) { // Or begin dragging a window
					dragcomponent = clicked;
				} else if(clicked->type == ctEdit) { // Or set focus to an edit control
					focuscomponent = clicked;
					((Edit*)focuscomponent)->focused = true;
				} else if(clicked->type == ctDropdown) {
					Dropdown* drop = (Dropdown*)clicked;
					if(drop->dropped) {
						
						// Make new selection...
						drop->selindex = (mousepos.y - drop->abstop)/(float)drop->itemheight - 1;
					}
					drop->Toggle();	
				}
				
				handled = true; // click was handled by UI
			}
			break;
		}
		case MK_MBUTTON: {
			mmousedown = true;
			break;
		}
		case MK_RBUTTON: {
			rmousedown = true;
			break;
		}
	}
	
	if(OnMouseDown) {
		OnMouseDown(wParam,lParam,handled);
	}
}
void Interface::DoMouseUp(WPARAM wParam,LPARAM lParam) {
	
	bool handled = false; // click was above UI
	
	// Don't use LOWORD, HIWORD to support multimonitor
	mousepos.x = GET_X_LPARAM(lParam);
	mousepos.y = GET_Y_LPARAM(lParam);
	
	switch(wParam) {
		case MK_LBUTTON: {
			lmousedown = false;

			Component* clicked = GetComponentAt(mousepos.x,mousepos.y);
			if(clicked) {

				// Check if a button received a click
				if(clicked->type == ctButton) {
					
					Button* thisbutton = (Button*)clicked;
					
					if(thisbutton->down) {
						thisbutton->down = false; // don't use thisbutton after event, it can be deleted
						if(thisbutton->OnClick) { // click when going up
							thisbutton->OnClick(thisbutton);
						}
					}
				}
				handled = true;
			}
			
			// Stop dragging
			dragcomponent = NULL;
			
			break;
		}
		case MK_MBUTTON: {
			mmousedown = false;
			break;
		}
		case MK_RBUTTON: {
			rmousedown = false;
			break;
		}
	}
	
	if(OnMouseUp) {
		OnMouseUp(wParam,lParam,handled);
	}
}
