#include "Key.h"

Key::Key(WPARAM key) {
	this->key = key;
	OnClick = NULL;
	pressed = false;
}
Key::Key(WPARAM key,void (*OnClick)(void* data)) {
	this->key = key;
	this->OnClick = OnClick;
	pressed = false;
}
Key::~Key() {
}
bool Key::Press() {
	if(pressed) {
		return false; // already down
	} else {
		pressed = true;
		if(OnClick) {
			OnClick(NULL);
		}
		return true; // key went down
	}
}
bool Key::Release() {
	if(pressed) {
		pressed = false;
		return true; // key went back up
	} else {
		return false; // already up
	}
}
bool Key::IsPressed() {
	return pressed;
}
WPARAM Key::GetKeyCode() {
	return key;
}
