#ifndef STATE_INCLUDE
#define STATE_INCLUDE

#include <d3d9.h> // D3DRENDERSTATETYPE

#if BUILDING_DLL
#define DLLIMPORT __declspec(dllexport)
#else
#define DLLIMPORT __declspec(dllimport)
#endif

class DLLIMPORT State {
	private:
		D3DRENDERSTATETYPE renderstate;
		DWORD value;
	public:
		State(D3DRENDERSTATETYPE renderstate);
		~State();
		DWORD Get();
		void Set(DWORD value);
};

#endif
