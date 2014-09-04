#ifndef MSGBOX_INCLUDE
#define MSGBOX_INCLUDE

#include "Components.h"

#if BUILDING_DLL
#define DLLIMPORT __declspec(dllexport)
#else
#define DLLIMPORT __declspec(dllimport)
#endif

class DLLIMPORT Messagebox : public Window {
    Label* lbltext;
    Button* okbtn;
    
	static void OKClick(Component* sender);
    public:
    	Messagebox(const char* text);
    	~Messagebox();
};

#endif
