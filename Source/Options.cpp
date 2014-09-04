#include <cstdio>

#include "Options.h"
#include "Console.h"
#include "Renderer.h"
#include "Options.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	abcd
*/
Options::Options(const char* inifile) : Window(1024-205,768-650,200,645,"Options") {
	
	// Stel alles in
	SetToDefaults();
	LoadFromFile(inifile);

	// En maak dan een dialoog zodat we shit kunnen verbouwen
	OnShow = OnShowDialog;
	
	// Resoluties enummen doen we pas als we getoond worden...
	AddChild(new Label(5,20,190,20,"Resolution:",true));
	cmbreso = new Dropdown(5,45,190,20);
	AddChild(cmbreso);
	
    // Schaduwen 1
	AddChild(new Label(5,70,190,20,"Shadow map size:",true));
	edshadowmapsize = new Edit(5,95,190,20,"");
	AddChild(edshadowmapsize);
	
    // Schaduwen 2
	AddChild(new Label(5,120,190,20,"Shadow distance:",true));
	edshadowdistance = new Edit(5,145,190,20,"");
	AddChild(edshadowdistance);
	
    // Schaduwen 3
	AddChild(new Label(5,170,190,20,"Shadow bias:",true));
	edshadowbias = new Edit(5,195,190,20,"");
	AddChild(edshadowbias);
	
	// Objectdetail
	AddChild(new Label(5,220,190,20,"Object detail:",true));
	edlodmultiplier = new Edit(5,245,190,20,"");
	AddChild(edlodmultiplier);
	
	// SSAO-samples
	AddChild(new Label(5,270,190,20,"SSAO samples:",true));
	edssaosamples = new Edit(5,295,190,20,"");
	AddChild(edssaosamples);

	// SSAO-afstand
	AddChild(new Label(5,320,190,20,"SSAO distance:",true));
	edssaodistance = new Edit(5,345,190,20,"");
	AddChild(edssaodistance);

	// SSAO-kernelsize
	AddChild(new Label(5,370,190,20,"SSAO radius:",true));
	edssaoradius = new Edit(5,395,190,20,"");
	AddChild(edssaoradius);
	
	// SSAO-multiplier
	AddChild(new Label(5,420,190,20,"SSAO multiplier:",true));
	edssaomultiplier = new Edit(5,445,190,20,"");
	AddChild(edssaomultiplier);
	
    // Texture filter
	AddChild(new Label(5,470,190,20,"Texture filter:",true));
	cmbfilter = new Dropdown(5,495,190,20);
	cmbfilter->AddItem("Bilinear");
	cmbfilter->AddItem("Trilinear");
	cmbfilter->AddItem("Anisotropic (1x)");
	cmbfilter->AddItem("Anisotropic (2x)");
	cmbfilter->AddItem("Anisotropic (4x)");
	cmbfilter->AddItem("Anisotropic (8x)");
	cmbfilter->AddItem("Anisotropic (16x)");
	AddChild(cmbfilter);

	// HDR, exposure
	AddChild(new Label(5,520,190,20,"HDR exposure:",true));
	edhdrexposure = new Edit(5,545,190,20,"");
	AddChild(edhdrexposure);
	
	// HDR, bloom start
	AddChild(new Label(5,570,190,20,"HDR bloom threshold:",true));
	edhdrbloomthreshold = new Edit(5,595,190,20,"");
	AddChild(edhdrbloomthreshold);
	
	// Knopjes
	btnapply = new Button(5,620,190,20,"Apply");
	btnapply->OnClick = OnApplyClick;
	AddChild(btnapply);
}
Options::~Options() {
	// Dialog gooit alle windows weg
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	Reageer op knopjes
*/
void Options::OnShowDialog(Component* Sender) { // fake static scope
	options->LoadInterface();
}
void Options::OnApplyClick(Component* Sender) { // fake static scope
	options->SaveInterface();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	Reageer op knopjes
*/
void Options::SetToDefaults() {
	// Reso
	backbufferwidth = 1024;
	backbufferheight = 768;
	refreshrate = 60;
	fullscreen = true;
	maxfsresolution = true;
	
	// Schaduwen
	shadowmapsize = 0; // 0 == disable
	shadowdistance = 512;
	shadowbias = 0.0001;
	
	// Objects
	lodmultiplier = 1;
	minviewdistance = 0.1; // not tweakable
	maxviewdistance = 4000; // idem

	// SSAO
	ssaosamples = 0; // 0 == disable
	ssaodistance = 1000;
	ssaomultiplier = 1;
	ssaodepthbias = 0.001f;
	ssaodepthmultiplier = 1;
	ssaoradius = 1;
	
	// AA
	aasamples = 0; // TODO: remove?
	aaquality = 1; // TODO: remove?
	
	// Textures
	texturefilter[0] = 0; // random guess
	afsamples = 0;
	
	// HDR
	enablehdr = true;
	hdrexposure = 1.5;
	hdrbloomthreshold = 1.0f;
	hdrbloommultiplier = 1.5f;
	
	// Misc.
	enablevsync = 0;
	usesoftware = 0;
}
void Options::LoadFromFile(const char* inipath) {
	
	// Storage
	char currentregel[256] = "";
	char currentoption[128] = "";
	char currentsetting[128] = "";

	FILE* ini = fopen(inipath,"r");
	if(ini == NULL) {
		console->Write("Error opening config file %s\r\n",inipath);
		return;
	};

	while(fgets(currentregel,sizeof(currentregel),ini)) {
		
		// Read option
		if(sscanf(currentregel,"%[^=]=%[^\n]",currentoption,currentsetting) != 2) {
			continue;
		}

		// Reso
		if(!strcmp(currentoption,"resolutionx")) {
			backbufferwidth = atoi(currentsetting);
		} else if(!strcmp(currentoption,"resolutiony")) {
			backbufferheight = atoi(currentsetting);
		} else if(!strcmp(currentoption,"refreshrate")) {
			refreshrate = atoi(currentsetting);
		} else if(!strcmp(currentoption,"fullscreen")) {
			fullscreen = (bool)atoi(currentsetting);
		} else if(!strcmp(currentoption,"maxfsresolution")) {
			maxfsresolution = (bool)atoi(currentsetting);

		// Schaduwen
		} else if(!strcmp(currentoption,"shadowmapsize")) {
			shadowmapsize = atoi(currentsetting);
		} else if(!strcmp(currentoption,"shadowdistance")) {
			shadowdistance = atof(currentsetting);
		} else if(!strcmp(currentoption,"shadowbias")) {
			shadowbias = atof(currentsetting);
	
		// SSAO
		} else if(!strcmp(currentoption,"ssaosamples")) {
			ssaosamples = atoi(currentsetting);
		} else if(!strcmp(currentoption,"ssaodistance")) {
			ssaodistance = atof(currentsetting);
		} else if(!strcmp(currentoption,"ssaomultiplier")) {
			ssaomultiplier = atof(currentsetting);
		} else if(!strcmp(currentoption,"ssaodepthbias")) {
			ssaodepthbias = atof(currentsetting);
		} else if(!strcmp(currentoption,"ssaodepthmultiplier")) {
			ssaodepthmultiplier = atof(currentsetting);
		} else if(!strcmp(currentoption,"ssaoradius")) {
			ssaoradius = atof(currentsetting);
		
		// LOD
		} else if(!strcmp(currentoption,"lodmultiplier")) {
			lodmultiplier = atof(currentsetting);
		} else if(!strcmp(currentoption,"minviewdistance")) { // not really tweakable
			minviewdistance = atof(currentsetting);
		} else if(!strcmp(currentoption,"maxviewdistance")) { // not really tweakable
			maxviewdistance = atof(currentsetting);
		
		// Anti aliasing				
		} else if(!strcmp(currentoption,"antialiasing")) {
			aasamples = atoi(currentsetting);
		} else if(!strcmp(currentoption,"antialiasingquality")) {
			aaquality = atoi(currentsetting);
		
		// Texture filtering
		} else if(!strcmp(currentoption,"texturefilter")) {
			strncpy(texturefilter,currentsetting,128);
		} else if(!strcmp(currentoption,"anisotropic")) {
			afsamples = atoi(currentsetting);
		
		// HDR
		} else if(!strcmp(currentoption,"enablehdr")) {
			enablehdr = (bool)atoi(currentsetting);
		} else if(!strcmp(currentoption,"hdrexposure")) {
			hdrexposure = atof(currentsetting);
		} else if(!strcmp(currentoption,"hdrbloomthreshold")) {
			hdrbloomthreshold = atof(currentsetting);
		} else if(!strcmp(currentoption,"hdrbloommultiplier")) {
			hdrbloommultiplier = atof(currentsetting);
			
		// Misc.
		} else if(!strcmp(currentoption,"enablevsync")) {
			enablevsync = (bool)atoi(currentsetting);
		} else if(!strcmp(currentoption,"usesoftware")) {
			usesoftware = (bool)atoi(currentsetting);
		} else {
			console->Write("Unknown setting: %s\r\n",currentregel);
		}
	}
	fclose(ini);
}
void Options::FindMaxResolution() {
	if(!renderer) {
		return;
	}
	
	// Get list of resolutions
	std::vector<D3DDISPLAYMODE> resolutions;
	renderer->GetResolutions(resolutions);
	
	int maxpixels = -1;
	int maxpixelsindex = -1;
	refreshrate = 60;
	
	// Obtain resolution with most pixels
	for(unsigned int i = 0;i < resolutions.size();i++) {
		int pixels = resolutions[i].Width * resolutions[i].Height;
		if(pixels >= maxpixels) {
			maxpixelsindex = i;
			maxpixels = pixels;
		}
	}
	
	// Now try to find 60Hz of this resolution. Otherwise pick highest
	for(unsigned int i = 0;i < resolutions.size();i++) {
		if(resolutions[i].Width == resolutions[maxpixelsindex].Width && 
		   resolutions[i].Height == resolutions[maxpixelsindex].Height)
		{
			if(resolutions[i].RefreshRate == 60) {
				backbufferwidth = resolutions[i].Width;
				backbufferheight = resolutions[i].Height;
				refreshrate = 60;
				break; // stop searching
			} else {
				refreshrate = std::max(refreshrate,(int)resolutions[i].RefreshRate);
			}
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	Interface, van RAM naar UI
*/
void Options::LoadInterface() {
	
	char buffer[128];
	
	cmbreso->Clear();
	std::vector<D3DDISPLAYMODE> resolutions;
	renderer->GetResolutions(resolutions);
	for(unsigned int i = 0;i < resolutions.size();i++) {
		char* reso = new char[64]; // leakt niet, wordt door UI opgeruimd
		snprintf(reso,128,"%dx%d %dHz",resolutions[i].Width,resolutions[i].Height,resolutions[i].RefreshRate);
		cmbreso->AddItem(reso);
	}
	
	// Stel de fullscreenresolutie in
	snprintf(buffer,128,"%dx%d %dHz",
		options->backbufferwidth,
		options->backbufferheight,
		options->refreshrate);
	cmbreso->SetSelection(buffer);
	
	// Stel de schaduwmapsize in
	snprintf(buffer,128,"%d",options->shadowmapsize);
	edshadowmapsize->SetCaption(buffer);
	
	// Stel de schaduwdistance in
	snprintf(buffer,128,"%g",options->shadowdistance);
	edshadowdistance->SetCaption(buffer);
	
	// Stel shadow correction in
	snprintf(buffer,128,"%g",options->shadowbias);
	edshadowbias->SetCaption(buffer);
	
	// Stel objectdetail in
	snprintf(buffer,128,"%g",options->lodmultiplier);
	edlodmultiplier->SetCaption(buffer);

	// Stel SSAO-samples in
	snprintf(buffer,128,"%d",options->ssaosamples);
	edssaosamples->SetCaption(buffer);

	// Stel SSAO-afstand in
	snprintf(buffer,128,"%g",options->ssaodistance);
	edssaodistance->SetCaption(buffer);

	// Stel SSAO-radius in
	snprintf(buffer,128,"%g",options->ssaoradius);
	edssaoradius->SetCaption(buffer);

	// Blabla
	snprintf(buffer,128,"%g",options->ssaomultiplier);
	edssaomultiplier->SetCaption(buffer);
			
	// Stel het texturefilter in
	if(!strcmp(options->texturefilter,"Bilinear")) {
		cmbfilter->SetSelection("Bilinear");
	} else if(!strcmp(options->texturefilter,"Trilinear")) {
		cmbfilter->SetSelection("Trilinear");
	} else {
		snprintf(buffer,128,"Anisotropic (%dx)",options->afsamples);
		cmbfilter->SetSelection(buffer);
	}
	
	// Stel HDR exposure in
	snprintf(buffer,128,"%g",options->hdrexposure);
	edhdrexposure->SetCaption(buffer);
	
	// Meer HDR
	snprintf(buffer,128,"%g",options->hdrbloomthreshold);
	edhdrbloomthreshold->SetCaption(buffer);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	Van UI naar RAM
*/
void Options::SaveInterface() {
	
	// Stel de resolutie in in
	sscanf(cmbreso->GetSelection(),"%dx%d %dHz",
		&backbufferwidth,
		&backbufferheight,
		&refreshrate);
	
	// Schaduwen 1
	shadowmapsize = atof(edshadowmapsize->GetCaption());
	
	// Schaduwen 2
	shadowdistance = atof(edshadowdistance->GetCaption());
	
	// Schaduwen 3
	shadowbias = atof(edshadowbias->GetCaption());
	
	// Objectdetail
	lodmultiplier = atof(edlodmultiplier->GetCaption());

	// SSAO 1
	ssaosamples = atoi(edssaosamples->GetCaption());

	// SSAO 2
	ssaodistance = atof(edssaodistance->GetCaption());

	// SSAO 3
	ssaoradius = atof(edssaoradius->GetCaption());

	// SSAO 4
	ssaomultiplier = atof(edssaomultiplier->GetCaption());
				
	// Stel het filter in
	const char* option = cmbfilter->GetSelection();
	if(!strcmp(option,"Bilinear")) {
		strcpy(texturefilter,option);
		afsamples = 1;
	} else if(!strcmp(option,"Trilinear")) {
		strcpy(texturefilter,option);
		afsamples = 1;
	} else if(!strncmp(option,"Anisotropic",11)) {
		strncpy(texturefilter,option,11);
		sscanf(option,"Anisotropic (%dx)",&afsamples);
	}
	
	// Stel exposure in
	hdrexposure = atof(edhdrexposure->GetCaption());
	
	// Stel exposure in
	hdrbloomthreshold = atof(edhdrbloomthreshold->GetCaption());

	// En naar de drivers sturen
	renderer->OnLostDevice();
}
