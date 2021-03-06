#ifndef KEY_H
#define KEY_H

#include <windows.h> // WPARAM

#if BUILDING_DLL
#define DLLIMPORT __declspec(dllexport)
#else
#define DLLIMPORT __declspec(dllimport)
#endif

class DLLIMPORT Key {
	private:
		bool pressed;
		WPARAM key;
	public:
		Key(WPARAM key);
		Key(WPARAM key,void (*OnClick)(void* data));
		~Key();

		void (*OnClick)(void* data);
		bool Press(); // true if key state changes
		bool Release(); // true if key state changes
		bool IsPressed();
		WPARAM GetKeyCode();
};

#endif
