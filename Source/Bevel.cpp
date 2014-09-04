#include "Bevel.h"

Bevel::Bevel(int left,int top,int width) : Component(left,top,width,1) {
	type = ctBevel;

	backcolor = float4(0.9f,0.9f,0.9f,1.0f);
}
