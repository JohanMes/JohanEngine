#include "State.h"
#include "Renderer.h"

State::State(D3DRENDERSTATETYPE NewState) {
	RenderState = NewState;
	Value = 0;
}
State::~State() {
}

void State::Set(DWORD NewValue) {
	if(NewValue != Value) {
		Value = NewValue;
		d3ddev->SetRenderState(RenderState,Value);
	}
}
