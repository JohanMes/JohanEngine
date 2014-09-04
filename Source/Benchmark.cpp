#include "Benchmark.h"
#include "Renderer.h"
#include "Messagebox.h"
#include "Console.h"

Benchmark::Benchmark() : Window(5,5,300,170,"Benchmark") {
	listener = NULL;
	ignoreindex = 0;
	CreateInterface();
}
Benchmark::Benchmark(float testtime) : Window(5,5,300,170,"Benchmark") {
	listener = NULL;
	CreateInterface();
	Start(testtime);
}
Benchmark::~Benchmark() {
	if(listener) {
		renderer->OnRenderFrame->Delete(listener);
		listener = NULL;		
	}
}

void Benchmark::CreateInterface() {

	// Resultaten
	minfpstext = new Label(5,20,50,20,"Min. FPS:",true);
	AddChild(minfpstext);
	
	minfpsfield = new Label(60,20,235,20,"",false);
	AddChild(minfpsfield);
	
	avgfpstext = new Label(5,45,50,20,"Avg. FPS:",true);
	AddChild(avgfpstext);
	
	avgfpsfield = new Label(60,45,235,20,"",false);
	AddChild(avgfpsfield);
	
	maxfpstext = new Label(5,70,50,20,"Max. FPS:",true);
	AddChild(maxfpstext);
	
	maxfpsfield = new Label(60,70,235,20,"",false);
	AddChild(maxfpsfield);
	
	cputext = new Label(5,95,30,20,"CPU:",true);
	AddChild(cputext);
	
	cpufield = new Label(40,95,255,20,"",false);
	AddChild(cpufield);
	
	gputext = new Label(5,120,30,20,"GPU:",true);
	AddChild(gputext);
	
	gpufield = new Label(40,120,255,20,"",false);
	AddChild(gpufield);
	
	btnsave = new Button(5,145,140,20,"Save");
	btnsave->OnClick = OnSaveClick;
	AddChild(btnsave);
	
	btnprint = new Button(155,145,140,20,"Print");
	btnprint->OnClick = OnPrintClick;
	AddChild(btnprint);
	
	float2 windowsize = float2(300,170);
	float2 topleft = renderer->GetCenteringCorner(windowsize);
	Move(topleft.x,topleft.y);
}
void Benchmark::GetPicSaveFileName(char* result) {
	char desktop[MAX_PATH] = "";
	GetCSIDLPath(0,desktop); // CSIDL_DESKTOP = 0
	snprintf(result,MAX_PATH,"%s\\JohanMarkResults\\%s.png",desktop,gpufield->GetCaption());
}
void Benchmark::GetLogSaveFileName(char* result) {
	char desktop[MAX_PATH] = "";
	GetCSIDLPath(0,desktop); // CSIDL_DESKTOP = 0
	snprintf(result,MAX_PATH,"%s\\JohanMarkResults\\Frametimes.m",desktop);
}
void Benchmark::GetSaveFolder(char* result) {
	char desktop[MAX_PATH] = "";
	GetCSIDLPath(0,desktop); // CSIDL_DESKTOP = 0
	snprintf(result,MAX_PATH,"%s\\JohanMarkResults",desktop);	
}
void Benchmark::Start(float testtime) {
	frametimes.clear();
	listener = renderer->OnRenderFrame->Add(OnRenderFrame,this);
	
	// Stop when the timer ends
	Timer* stoptimer = new Timer();
	stoptimer->AddEvent(OnTimerEnd,this);
	stoptimer->SetTime(testtime * 1000.0f,true);
}
bool Benchmark::IsRunning() {
	return (bool)listener;
}
void Benchmark::Stop() {
	if(!IsRunning()) {
		return;
	}
	
	// Stop listening to renderer
	renderer->OnRenderFrame->Delete(listener);
	listener = NULL;
	
	char buffer[256];
	
	// Ignore initialisation time
	ignoreindex = 0;
	float tmpframetimesum = 0;
	for(unsigned int i = 0;i < frametimes.size();i++) {
		tmpframetimesum += frametimes[i];
		if(tmpframetimesum > 1000) { // ignore the first 1sec OR the first 25 frames
			ignoreindex = i;//std::max(25u,i);
			break;
		}
	}
	
	// This is not equal to benchmarktime for some reason
	float frametimesum = 0;
	for(unsigned int i = ignoreindex;i < frametimes.size();i++) {
		frametimesum += frametimes[i];
	}
	frametimesum /= 1000.0f; // keep seconds

	// Show framerates instead of frametimes...
	minfps = 9999999999999;
	maxfps = 0;
	for(unsigned int i = ignoreindex;i < frametimes.size();i++) {
		minfps = std::min(minfps,1000.0f/frametimes[i]);
		maxfps = std::max(maxfps,1000.0f/frametimes[i]);
	}
	avgfps = (float)(frametimes.size() - ignoreindex)/frametimesum;

	snprintf(buffer,256,"%g",minfps);
	minfpsfield->SetCaption(buffer);
	snprintf(buffer,256,"%g",avgfps);
	avgfpsfield->SetCaption(buffer);	
	snprintf(buffer,256,"%g",maxfps);
	maxfpsfield->SetCaption(buffer);
	
	// Set PC information
	GetCPUName(buffer,256);
	cpufield->SetCaption(buffer);
	
	GetGPUName(buffer,256);
	gpufield->SetCaption(buffer);
	
	Show(true);
}
void Benchmark::Save() {
	
	// Teken alleen het eigen venster
	renderer->Begin(true);
	renderer->BeginTechnique(renderer->TechInterface->handle);
	renderer->DrawComponent(this);
	renderer->EndTechnique();
	renderer->EndScene();
	
	RECT windowrect;
	GetRect(&windowrect);
	
	// Create folder to save in
	char finalname[MAX_PATH];
	GetSaveFolder(finalname);
	CreateDirectory(finalname,NULL);

	// Geef het de naam van de GPU
	GetPicSaveFileName(finalname);
	D3DXSaveSurfaceToFile(finalname,D3DXIFF_PNG,renderer->backbuffercolor,NULL,&windowrect);
	
	// Save 1/frametimes in matlab format...
	GetLogSaveFileName(finalname);
	FILE* frametimefile = fopen(finalname,"wb");
	if(frametimefile) {
		fprintf(frametimefile,"clear all;\r\n");
		fprintf(frametimefile,"close all;\r\n");
		fprintf(frametimefile,"clc;\r\n");
		fprintf(frametimefile,"\r\n");
		fprintf(frametimefile,"x = [");
		for(unsigned int i = 0;i < frametimes.size();i++) {
			if(i != frametimes.size()-1) {
				fprintf(frametimefile,"%g, ",1000.0f/frametimes[i]);
			} else {
				fprintf(frametimefile,"%g",1000.0f/frametimes[i]);
			}	
		}
		fprintf(frametimefile,"];\r\n");
		fprintf(frametimefile,"\r\n");
		fprintf(frametimefile,"grid on;\r\n");
		fprintf(frametimefile,"hold on;\r\n");
		fprintf(frametimefile,"plot(x,'r','LineWidth',2);\r\n");
		fprintf(frametimefile,"line([%d %d],[0 %g],'LineWidth',2);\r\n",ignoreindex,ignoreindex,1.2f*maxfps); // indicate with vertical bar
		fprintf(frametimefile,"\r\n");
		fprintf(frametimefile,"xlabel('Frame number');\r\n");
		fprintf(frametimefile,"ylabel('Frame rate');\r\n");
	//	fprintf(frametimefile,"legend('CPU: %s\r\nGPU: %s;FPS: %s');\r\n",
	//		cpufield->GetCaption(),gpufield->GetCaption(),avgfpsfield->GetCaption());
		fprintf(frametimefile,"legend('Average: %s');\r\n",
			avgfpsfield->GetCaption());
		fprintf(frametimefile,"ylim([0 %g]);",1.2f*maxfps); // gok
		fclose(frametimefile);
	}
	
	// Raak deze frame kwijt
	renderer->Present();	
}
void Benchmark::Print() {
	
	Save();
	
	// Then attempt to print the saved file
	char finalname[MAX_PATH];
	GetPicSaveFileName(finalname);

	// En printen maar
	ShellExecute(hwnd,"print",finalname,NULL,NULL,SW_SHOWNORMAL);
	ShowWindow(hwnd,SW_MINIMIZE);
}
void Benchmark::AddFrameTime(float dt) {
	frametimes.push_back(dt);
}
void Benchmark::OnRenderFrame(void* sender,double dt) {
	((Benchmark*)sender)->AddFrameTime(dt);
}
void Benchmark::OnTimerEnd(void* sender,double dt) {
	((Benchmark*)sender)->Stop();
}
void Benchmark::OnSaveClick(Component* sender) {
	((Benchmark*)sender->parent)->Save();
}
void Benchmark::OnPrintClick(Component* sender) {
	((Benchmark*)sender->parent)->Print();
}
