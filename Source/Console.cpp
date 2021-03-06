#include <cstdio>
#include "Console.h"
#include "Renderer.h"
#include "Object.h"
#include "Scene.h"
#include "Camera.h"
#include "Objects.h"
#include "Models.h"
#include "Lights.h"
#include "Textures.h"
#include "Animations.h"
#include "FXShader.h"
#include "Timers.h"

Console::Console() : Window(270,458,300,305,"Console") {
	textctrl = new Label(5,20,290,255,"",false);
	AddChild(textctrl);

	btnsave = new Button(5,280,60,20,"Save");
	btnsave->OnClick = OnSaveClick;
	AddChild(btnsave);

	btnclear = new Button(70,280,60,20,"Clear");
	btnclear->OnClick = OnClearClick;
	AddChild(btnclear);

	inputctrl = new Edit(135,280,160,20,"");
	inputctrl->OnReturn = OnReturnPress;
	AddChild(inputctrl);
}
Console::~Console() {
	for(unsigned int i = 0; i < clocks.size(); i++) {
		delete clocks[i];
	}
	clocks.clear();

	// Window* deletes itself
}
void Console::OnSaveClick(Component* sender) { // fake static scope
	char finalpath[MAX_PATH];
	snprintf(finalpath,MAX_PATH,"%s\\%s",Globals::exepath,"Log.txt");
	FILE* logfile = fopen(finalpath,"ab");
	if(logfile) {
		fputs(Globals::console->textctrl->GetCaption(),logfile);
		fclose(logfile);
	}
}
void Console::OnClearClick(Component* sender) { // fake static scope
	Globals::console->textctrl->SetCaption("");
}
void Console::OnReturnPress(Component* sender) {
	Globals::console->ProcessString(Globals::console->inputctrl->GetCaption());
	Globals::console->inputctrl->SetCaption("");
}
void Console::ProcessString(const char *input) {

	// Clearen
	char word1[128];

	if(sscanf(input,"%[^ \n]",word1) == 1) { // if there is a word present...
		if(!strcmp(word1,"exit")) {
			SendMessage(Globals::hwnd,WM_CLOSE,0,0);
		} else if(!strcmp(word1,"clear")) {
			OnClearClick(NULL);
		} else if(!strcmp(word1,"print")) {
			if(sscanf(input,"print %[^ \n]",word1) == 1) { // if there is a word after info
				if(!strcmp(word1,"object")) {
					if(sscanf(input,"print object %s",word1) == 1) { // if there is a name after that
						Object* infoobject = Globals::scene->objects->GetByName(word1);
						if(infoobject) {
							infoobject->Print();
						} else {
							Write("(%s) Cannot find object!\r\n",input);
						}
					}
				} else if(!strcmp(word1,"camera")) {
					Globals::camera->Print();
				} else if(!strcmp(word1,"objects")) {
					Globals::scene->objects->Print();
				} else if(!strcmp(word1,"models")) {
					Globals::models->Print();
				} else if(!strcmp(word1,"textures")) {
					Globals::textures->Print();
				} else if(!strcmp(word1,"lights")) {
					Globals::scene->lights->Print();
				} else if(!strcmp(word1,"animations")) {
					Globals::scene->animations->Print();
				} else if(!strcmp(word1,"shaders")) {
					for(unsigned int i = 0; i < Globals::renderer->shaders.size(); i++) { // TODO: create class?
						Globals::renderer->shaders[i]->Print();
					}
				} else {
					Write("(%s) Cannot print information for that!\r\n",input);
				}
			} else {
				Write("(%s) Invalid input!\r\n",input);
			}
		} else if(!strcmp(word1,"save")) {
			if(sscanf(input,"save %[^ \n]",word1) == 1) { // if there is a word after save
				if(!strcmp(word1,"console")) {
					OnSaveClick(NULL);
				} else if(!strcmp(word1,"models")) {
					Globals::models->SaveToCSV();
				} else if(!strcmp(word1,"textures")) {
					Globals::textures->SaveToCSV();
				} else if(!strcmp(word1,"animations")) {
					Globals::scene->animations->SaveToCSV();
				} else if(!strcmp(word1,"timers")) {
					Globals::scene->timers->SaveToCSV();
				} else {
					Write("(%s) Cannot save info for that!\r\n",input);
				}
			} else {
				Write("(%s) Invalid input!\r\n",input);
			}
		} else {
			Write("%s: Unknown Command!\r\n",input);
		}
	}
}
void Console::WriteVar(const char* name,int value) {
	Write("%s =\r\n\t%d (int)\r\n",name,value);
}
void Console::WriteVar(const char* name,unsigned int value) {
	Write("%s =\r\n\t%u (unsigned int)\r\n",name,value);
}
void Console::WriteVar(const char* name,void* value) {
	Write("%s =\r\n\t%x (void*)\r\n",name,value);
}
void Console::WriteVar(const char* name,float value) {
	Write("%s =\r\n\t%g (float)\r\n",name,value);
}
void Console::WriteVar(const char* name,double value) {
	Write("%s =\r\n\t%g (float)\r\n",name,value);
}
void Console::WriteVar(const char* name,bool value) {
	if(value) {
		Write("%s =\r\n\ttrue (bool)\r\n",name);
	} else {
		Write("%s =\r\n\tfalse (bool)\r\n",name);
	}
}
void Console::WriteVar(const char* name,const float2& value) {
	Write("%s =\r\n\t%12g %12g (float2)\r\n",name,value.x,value.y);
}
void Console::WriteVar(const char* name,const float3& value) {
	Write("%s =\r\n\t%12g %12g %12g (float3)\r\n",name,value.x,value.y,value.z);
}
void Console::WriteVar(const char* name,const float4& value) {
	Write("%s =\r\n\t%12g %12g %12g %12g (float4)\r\n",name,value.x,value.y,value.z,value.w);
}
void Console::WriteVar(const char* name,const float4x4& value) {
	Write("%s =\r\n\t%12g %12g %12g %12g\r\n\t%12g %12g %12g %12g\r\n\t%12g %12g %12g %12g\r\n\t%12g %12g %12g %12g (float4x4)\r\n",
	      name,
	      value._11,value._21,value._31,value._41,
	      value._12,value._22,value._32,value._42,
	      value._13,value._23,value._33,value._43,
	      value._14,value._24,value._34,value._44);
}
void Console::WriteVar(const char* name,const char* value) {
	Write("%s =\r\n\t%s (char*)\r\n",name,value);
}
void Console::Write(const char* format,...) {
	if(textctrl) {
		// Allocate space
		char buffer[1024];

		// Print
		va_list parameters;
		va_start(parameters,format);
		vsnprintf(buffer,sizeof(buffer),format,parameters);
		va_end(parameters);

		// Add to control
		textctrl->AddCaption(buffer);
	}
}
void Console::WriteBeginTimer(const char* text) {
	clocks.push_back(new Clock(true));
	Write(text);
}
void Console::WriteEndTimer(const char* text) {
	if(clocks.size() > 0) {
		float passedms = clocks.back()->GetTimeSec() * 1000.0f;
		clocks.pop_back();
		Write("%s (%gms)\r\n",text,passedms);
	} else {
		Write("ERROR: no timers to end!\r\n");
	}
}
