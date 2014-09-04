#include <cstdio>
#include "Renderer.h"
#include "Dialogs.h"
#include "Options.h"
#include "resource.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	abcd
*/
Renderer::Renderer() {
	
	OnRenderFrame = new TimeEvent();
	
	// Vertexdeclaraties ook
	standard = NULL;
	normalmap = NULL;
	
	// Fonts ook
	tooltipfontwhite = NULL;
	tooltipfontshadow = NULL;
	interfacefont = NULL;
	interfacesprite = NULL;
	tooltipfontsprite = NULL;

	// Zet alle textures op nul, zie header voor meer informatie
	AmbientDataTex = new RenderTarget();
	AmbientFactorTex = new RenderTarget();
	FloatTex1 = new RenderTarget();
	FloatTex2 = new RenderTarget();
	HalfFloatTex1 = new RenderTarget();
	HalfFloatTex2 = new RenderTarget();

	// Pointers naar scherm
	backbuffercolor = NULL;
	backbufferdepth = NULL;
	
	// Nog meer rommel op 0
	schedulesave = false;
	
	// Tooltipopties
	framecount = 0;
	tooltipmode = 2;
	timemulti = 1;
	time = 0.0f;
	frameratechecktime = 0.0f;
	saveframetimes = false;
	
	// Renderstate-handling
	cullmodestate = new State(D3DRS_CULLMODE);
	fillmodestate = new State(D3DRS_FILLMODE);
	alphateststate = new State(D3DRS_ALPHATESTENABLE);
	alphablendstate = new State(D3DRS_ALPHABLENDENABLE);
	
	// Maak de frameclock (leest alleen tijd af)
	frameclock = new Clock(true);
	
	// En stel NOG meer op 0 in
    drawnfaces = 0;
    drawnvertices = 0;
    drawcalls = 0;

	RECT rcClient;
	GetClientRect(hwnd,&rcClient);
	
	// Vul de standaardopties in en neem die direct in gebruik
	memset(&presentparameters,0,sizeof(presentparameters));
	presentparameters.Windowed = true;
	presentparameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	presentparameters.BackBufferCount = 1;
	presentparameters.hDeviceWindow = hwnd;
	presentparameters.BackBufferFormat = D3DFMT_X8R8G8B8;
	presentparameters.EnableAutoDepthStencil = true;
	presentparameters.BackBufferWidth = rcClient.right;
	presentparameters.BackBufferHeight = rcClient.bottom;
	presentparameters.AutoDepthStencilFormat = D3DFMT_D24X8;
	presentparameters.MultiSampleType = D3DMULTISAMPLE_NONE;
	presentparameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	presentparameters.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
	
	// Maak het videokaartobject
	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if(!d3d) {
		MessageBox(hwnd,"Call to Direct3DCreate9 failed!","Error",MB_ICONERROR);
	}
	
	// Verbind met GPU #0
	if(d3d->GetAdapterCount() > 0) {
		d3d->GetAdapterIdentifier(0,0,&properties);
		if(options->usesoftware) {
			d3d->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_REF,hwnd,D3DCREATE_SOFTWARE_VERTEXPROCESSING|D3DCREATE_PUREDEVICE,&presentparameters,&d3ddev);
		} else {
			d3d->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,hwnd,D3DCREATE_HARDWARE_VERTEXPROCESSING|D3DCREATE_PUREDEVICE,&presentparameters,&d3ddev);
		}
	} else {
		MessageBox(hwnd,"Cannot find any graphics adapters!","Error",MB_ICONERROR);
	}

	// Fonts maken
	HDC hdc = GetDC(hwnd);
	D3DXCreateFont(d3ddev,GetFontSizePt(14,hdc),0,0,0,0,0,OUT_TT_ONLY_PRECIS,0,0,"Consolas",&tooltipfontwhite);
	D3DXCreateFont(d3ddev,GetFontSizePt(14,hdc),0,FW_BLACK,0,0,0,OUT_TT_ONLY_PRECIS,0,0,"Consolas",&tooltipfontshadow);
	D3DXCreateFont(d3ddev,GetFontSizePt(9,hdc),0,0,0,0,0,OUT_TT_ONLY_PRECIS,0,0,"Segoe UI",&interfacefont);
	D3DXCreateSprite(d3ddev,&interfacesprite);
	D3DXCreateSprite(d3ddev,&tooltipfontsprite);
	ReleaseDC(hwnd,hdc);
	
	// Compileer dan de shaders (gebruik prebuilt?)
	LPD3DXBUFFER error = NULL;
	D3DXCreateEffectFromFile(d3ddev,"Data\\Shaders\\main.fxo",0,0,D3DXFX_NOT_CLONEABLE|D3DXSHADER_PREFER_FLOW_CONTROL,0,&FX,&error);
	if(error) {
		MessageBox(hwnd,(char*)error->GetBufferPointer(),"Error",MB_ICONERROR);
	} else {
		
		// Shaders, Per vertex
		shaders.push_back(new FXShader("PerVertex"));
		shaders.push_back(new FXShader("PureTexture"));
		shaders.push_back(new FXShader("PureTextureMultitexConst"));
		shaders.push_back(new FXShader("NoShade"));
		
		// Shaders, Per pixel
		shaders.push_back(new FXShader("PerPixel"));
		shaders.push_back(new FXShader("PerPixelSpecular"));
		shaders.push_back(new FXShader("PerPixelGooch"));
		shaders.push_back(new FXShader("PerPixelMinnaert"));
		shaders.push_back(new FXShader("PerPixelMultitexConst"));
		shaders.push_back(new FXShader("PerPixelMultitexMixer"));
		shaders.push_back(new FXShader("PerPixelOrenNayar"));
		shaders.push_back(new FXShader("PerPixelCookTorrance"));
		
		// Shaders, Per pixel, tangent space
		shaders.push_back(new FXShader("PerPixelNormal"));
		shaders.push_back(new FXShader("PerPixelNormalSpecular"));
		shaders.push_back(new FXShader("PerPixelNormalParallax"));
		shaders.push_back(new FXShader("PerPixelNormalSpecularParallax"));
		shaders.push_back(new FXShader("PerPixelNormalSpecularAmbient"));
		
		// Buffervullers voor shadow/SSAO/hardcoded zooi
		TechShadowMap = new FXShader("ShadowMap");
		TechAmbientMap = new FXShader("AmbientMap");
		TechGaussian = new FXShader("GaussFilter");
		TechSSAOFactor = new FXShader("SSAOFactor");
		TechSSAOBlur = new FXShader("SSAOBlur");
		TechBrightPass = new FXShader("BrightPass");
		TechBrightBlur = new FXShader("BrightBlur");
		TechToneMap = new FXShader("ToneMap");
		TechInterface = new FXShader("Interface");
		TechStock = new FXShader("Stock");
		
		// Variabelen
		FXMatWorld = new FXVariable("World");
		FXMatWorldView = new FXVariable("WorldView");
		FXMatWorldViewProj = new FXVariable("WorldViewProj");
		FXMatLightWorldViewProj = new FXVariable("LightWorldViewProj");
		
		FXMaterialdiffuse = new FXVariable("materialdiffuse");
		FXMaterialspecular = new FXVariable("materialspecular");
		FXMaterialshininess = new FXVariable("materialshininess");
		FXMaterialTiling = new FXVariable("materialtiling");
		FXMaterialMixer = new FXVariable("materialmixer");
		
		FXWidth = new FXVariable("width");
		FXHeight = new FXVariable("height");
		FXInvwidth = new FXVariable("invwidth");
		FXInvheight = new FXVariable("invheight");
		FXTimevar = new FXVariable("timevar");
		FXInterfacecolor = new FXVariable("interfacecolor");
		
		// textures
		FXScreentex1 = new FXVariable("screentex1");
		FXScreentex2 = new FXVariable("screentex2");
		FXScreentex3 = new FXVariable("screentex3");
		FXDiffusetex = new FXVariable("diffusetex");
		FXSpeculartex = new FXVariable("speculartex");
		FXNormaltex = new FXVariable("normaltex");
		FXParallaxtex = new FXVariable("parallaxtex");
		FXAmbienttex = new FXVariable("ambienttex");
		FXShadowtex = new FXVariable("shadowtex");
	}
	
	// Screenspacequad
	screenspacequad = models->Add();
	screenspacequad->Load2DQuad(-1.0f,-1.0f,1.0f,1.0f);
	screenspacequad->SendToGPU();
}
Renderer::~Renderer() {
	
	// Stateonthouders
	delete cullmodestate;
	delete fillmodestate;
	delete alphateststate;
	delete alphablendstate;
	
	// Frameclock
	delete frameclock;
	
	// Gooi render targets helemaal weg (niet alleen clearen)
	delete AmbientDataTex;
	delete AmbientFactorTex;
	delete FloatTex1;
	delete FloatTex2;
	delete HalfFloatTex1;
	delete HalfFloatTex2;
	
	// quad gooit zichzelf weg uit buffers
	
	// Gooi alle shaders weg
	for(unsigned int i = 0;i < shaders.size();i++) {
		delete shaders[i];
	}
	shaders.clear();
	SafeRelease(FX);
	// TODO: FXVariable clearen?
	
	// Gooi de fonts e.d. weg
	SafeRelease(tooltipfontwhite);
	SafeRelease(tooltipfontshadow);
	SafeRelease(interfacefont);
	SafeRelease(interfacesprite);
	SafeRelease(tooltipfontsprite);
	
	// Multievent
	delete OnRenderFrame;
	
	// Pointers naar de backbuffer
	SafeRelease(backbuffercolor);
	SafeRelease(backbufferdepth);
	
	// Vertexdeclaraties
	SafeRelease(standard);
	SafeRelease(normalmap);
	
	// Device pointers
	SafeRelease(d3ddev);
	SafeRelease(d3d);
}
void Renderer::OnLostDevice() {
	
	// Hieronder stellen we presentparameters in, omdat die voor het resetten moeten worden ingesteld...
	
	// Anti aliasing
	presentparameters.MultiSampleType = (D3DMULTISAMPLE_TYPE)options->aasamples;
	if(presentparameters.MultiSampleType > 0) {
		presentparameters.MultiSampleQuality = options->aaquality;
	} else {
		presentparameters.MultiSampleQuality = 0;
	}
	
	// Only support vsync when using fullscreen
	if(!presentparameters.Windowed) {
		presentparameters.BackBufferWidth = options->backbufferwidth;
		presentparameters.BackBufferHeight = options->backbufferheight;
		presentparameters.FullScreen_RefreshRateInHz = options->refreshrate;
	} else {
		presentparameters.FullScreen_RefreshRateInHz = 0;
	}
	
	// V-sync
	if(options->enablevsync) {
		presentparameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	} else {
		presentparameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	}
	
	// Redbare resources
	tooltipfontwhite->OnLostDevice();
	tooltipfontshadow->OnLostDevice();
	interfacefont->OnLostDevice();
	interfacesprite->OnLostDevice();
	tooltipfontsprite->OnLostDevice();
	FX->OnLostDevice();
	scene->OnLostDevice(); // Delete light buffers...
	ui->OnLostDevice();
	
	// Gooi alle zooi in VRAM weg
	ReleaseResources();

	// En we beginnen opnieuw
	d3ddev->Reset(&presentparameters);

	// Al het redbare redden
	tooltipfontwhite->OnResetDevice();
	tooltipfontshadow->OnResetDevice();
	interfacefont->OnResetDevice();
	interfacesprite->OnResetDevice();
	tooltipfontsprite->OnResetDevice();
	FX->OnResetDevice();
	scene->OnResetDevice(); // Recreate light buffers
	ui->OnResetDevice(); // check if it exists?
	
	// Maak eigen buffers opnieuw en reset states
	OnResetDevice();
}
void Renderer::OnResetDevice() {

	// Statische opties setten
	d3ddev->SetRenderState(D3DRS_ALPHAREF,30);
	d3ddev->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_GREATER);
	d3ddev->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	d3ddev->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

	// Backbuffer krijgen
	d3ddev->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&backbuffercolor);
	d3ddev->GetDepthStencilSurface(&backbufferdepth);
	
	D3DVERTEXELEMENT9 standarddeclare[] = {
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		{0, 20, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
		D3DDECL_END()
	};
	D3DVERTEXELEMENT9 normalmapdeclare[] = {
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		{0, 20, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
		{0, 32, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0},
		{0, 44, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0},
		D3DDECL_END()
	};

	// Bandbreedtebesparing
	d3ddev->CreateVertexDeclaration(standarddeclare,&standard);
	d3ddev->CreateVertexDeclaration(normalmapdeclare,&normalmap);

	// SSAO-spulletjes
	if(options->ssaosamples > 0) {
		AmbientDataTex->Create(presentparameters.BackBufferWidth,presentparameters.BackBufferHeight,D3DFMT_A32B32G32R32F,false); // Data, zoals normalen en diepte
		AmbientFactorTex->Create(presentparameters.BackBufferWidth,presentparameters.BackBufferHeight,D3DFMT_R32F,false); // Factor, zonder blur
	}
	
	// HDR-spulletjes
	if(options->enablehdr) {
		FloatTex1->Create(presentparameters.BackBufferWidth,presentparameters.BackBufferHeight,D3DFMT_A16B16G16R16F,false);
		FloatTex2->Create(presentparameters.BackBufferWidth,presentparameters.BackBufferHeight,D3DFMT_A16B16G16R16F,false);
		HalfFloatTex1->Create(presentparameters.BackBufferWidth/2,presentparameters.BackBufferHeight/2,D3DFMT_A16B16G16R16F,false);
		HalfFloatTex2->Create(presentparameters.BackBufferWidth/2,presentparameters.BackBufferHeight/2,D3DFMT_A16B16G16R16F,false);
	}
	
	// Stel het filter in
	const char* option = options->texturefilter; // scheelt getyp
	if(!strcmp(option,"Bilinear")) {
		FX->SetInt("filter",D3DTEXF_POINT); // D3DSAMP_MAXANISOTROPY
	} else if(!strcmp(option,"Trilinear")) {
		FX->SetInt("filter",D3DTEXF_LINEAR);
	} else if(!strcmp(option,"Anisotropic")) {
		FX->SetInt("filter",D3DTEXF_ANISOTROPIC);
	}
	FX->SetInt("numaniso",options->afsamples);

	// Afmetingen van het beeld
	FXWidth->Set(presentparameters.BackBufferWidth);
	FXHeight->Set(presentparameters.BackBufferHeight);
	
	// 1/ dat van hierboven
	FXInvwidth->Set(1.0f/(float)presentparameters.BackBufferWidth);
	FXInvheight->Set(1.0f/(float)presentparameters.BackBufferHeight);
	
	// Schaduwen
	float offset = 0.5f + (0.5f/(float)options->shadowmapsize);
	D3DXMATRIX ShadowOffset = D3DXMATRIX(0.5f,     0.0f,     0.0f,     0.0f,
					          		 	 0.0f,    -0.5f,     0.0f,     0.0f,
					          		 	 0.0f,     0.0f,     1.0f,     0.0f,
						      		     offset, offset,     0.0f,     1.0f);

	FX->SetMatrix("ShadowOffset",&ShadowOffset);
	FX->SetFloat("invshadowmapsize",1.0f/(float)options->shadowmapsize);
	FX->SetBool("enableshadows",options->shadowmapsize > 0);
	FX->SetFloat("shadowbias",options->shadowbias);
	FX->SetFloat("shadowcoeff",0.1f);

	// SSAO
	FX->SetInt("ssaosamples",options->ssaosamples);
	FX->SetFloat("ssaomultiplier",options->ssaomultiplier);
	FX->SetFloat("ssaodepthbias",options->ssaodepthbias);
	FX->SetFloat("ssaodepthmultiplier",options->ssaodepthmultiplier);
	FX->SetFloat("ssaoradius",options->ssaoradius);
	
	// HDR
	FX->SetFloat("hdrexposure",options->hdrexposure);
	FX->SetFloat("hdrbloomthreshold",options->hdrbloomthreshold);
	FX->SetFloat("hdrbloommultiplier",options->hdrbloommultiplier);
	
	// Misc.
	FX->SetFloat("minviewdistance",options->minviewdistance);
	FX->SetFloat("maxviewdistance",options->maxviewdistance);
	
	// Stel de Camera in op de nieuwe resolutie
	if(camera) {
		camera->SetMinViewDistance(options->minviewdistance); // TODO: beginupdate?
		camera->SetMaxViewDistance(options->maxviewdistance);
		camera->SetRatio((float)presentparameters.BackBufferWidth/(float)presentparameters.BackBufferHeight);
	}
}
void Renderer::ReleaseResources() {
	
	// Texture voor SSAO (diepte)
	AmbientDataTex->Clear();
	AmbientFactorTex->Clear();
	
	// HDR-texture, pingpong, 100%
	FloatTex1->Clear();
	FloatTex2->Clear();
	
	// HDR-texture, pingpong, 50%
	HalfFloatTex1->Clear();
	HalfFloatTex2->Clear();

	// Pointers naar de backbuffer
	SafeRelease(backbuffercolor);
	SafeRelease(backbufferdepth);
	
	// Vertexdeclaraties
	SafeRelease(standard);
	SafeRelease(normalmap);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	Tooltipspulletjes
*/
void Renderer::PrintTooltip() {
	if(tooltipmode == 1) {
    	snprintf(tooltip,sizeof(tooltip),
			"Framerate: %.1f\n"
			"Frametime: %.3fms (%.3fms .. %.3fms)\n"
			"Time: %02d:%02d",
			framerate,
			frametimes[0],
			lotime,
			hitime,
			GetClockTimeHours(),
			GetClockTimeMins());
	} else if(tooltipmode == 2) {
    	snprintf(tooltip,sizeof(tooltip),
			"Device: %s\n"
			"Framecount: %d\n"
			"Framerate: %.1f\n"
			"Frametime: %.3fms (%.3fms .. %.3fms)\n"
			"Time: %02d:%02d",
			properties.Description,
			framecount,
			framerate,
			frametimes[0],
			lotime,
			hitime,
			GetClockTimeHours(),
			GetClockTimeMins());
	} else if(tooltipmode == 3) {
		snprintf(tooltip,sizeof(tooltip),
			"Device: %s\n"
			"Camera: %.4f %.4f %.4f, Angle: %.2f %.2f\n"
			"Framecount: %d\n"
			"Framerate: %.1f\n"
			"Frametime: %.3fms (%.3fms .. %.3fms)\n"
			"Triangles: %d\n"
			"Vertices: %d\n"
			"Draw calls: %d\n"
			"Time: %02d:%02d (%dx)",
			properties.Description,
			camera->pos.x,
			camera->pos.y,
			camera->pos.z,
			RadToDeg(camera->angleH),
			RadToDeg(camera->angleV),
			framecount,
			framerate,
			frametimes[0],
			lotime,
			hitime,
			drawnfaces,
			drawnvertices,
			drawcalls,
			GetClockTimeHours(),
			GetClockTimeMins(),
			timemulti);
	}
}
void Renderer::ShowTooltip(int mode) {
	tooltipmode = mode;
}
void Renderer::ToggleTooltip() {
	tooltipmode = (tooltipmode+1) % 4; // 0 = off, 1 = FPS, 2 = FPS+, 3 = uitgebreid
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	Tijddingen
*/
int Renderer::GetTimeMulti() {
	return timemulti;
}
void Renderer::SetTimeMulti(int value) {
	timemulti = std::max(0,value);
}
double Renderer::GetTime() {
	return time;
}
int Renderer::GetClockTimeSec() {
	return (int)fmod(time,60.0f);
}
int Renderer::GetClockTimeMins() {
	float timemins = floorf(time/60.0f);
	return (int)fmod(timemins,60.0f);
}
int Renderer::GetClockTimeHours() {
	float timehours = floorf(time/3600.0f);
	return (int)fmod(timehours,24.0f);
}
void Renderer::SetTime(int hours,int mins) {
	time = hours*3600 + mins*60;
}
void Renderer::AddTime(int hours,int mins) {
	time += hours*3600 + mins*60;
}
void Renderer::UpdateTime() {
	
	double frametimesec = frameclock->Reset();
	double frametimems = frametimesec * 1000.0;
	double gametimesec = frametimesec * timemulti;

	// Update game time
	time += gametimesec;
	
	// Add frame to begin, remove last, keep maximum of 100
	frametimes.insert(frametimes.begin(),frametimems);
	if(frametimes.size() > 100) {
		if(saveframetimes) {
			SaveFrameTimes();
		}
		frametimes.pop_back(); // remove last
	}
	
	// Keep average, update every half a second
	frameratechecktime += frametimems;
	if(frameratechecktime > 500.0) {
		
		double avgframetime = 0.0;
		
		// Calculate average based on frametimes array of past 500 ms
		if(frametimes.size() > 0) {
			
			int samplecount = 0;
			
			// Only average the frames inside the check time (prefer newest)
			for(unsigned int i = 0;i < frametimes.size();i++) {
				samplecount++;
				avgframetime += frametimes[i];
				if(avgframetime > frameratechecktime) {
					break;
				}
			}
			
			// Get average frametime
			avgframetime /= samplecount;
			
			// Get average FPS (in seconds)
			framerate = 1000.0/avgframetime;
		}
		
		frameratechecktime = 0.0;
	}
	
	// Keep lows
	lotime = frametimes[0]; // always exists
	for(unsigned int i = 1;i < frametimes.size();i++) {
		lotime = std::min(lotime,frametimes[i]);
	}
	
	// Keep highs
	hitime = frametimes[0]; // always exists
	for(unsigned int i = 1;i < frametimes.size();i++) {
		hitime = std::max(hitime,frametimes[i]);
	}

	drawnfaces = 0;
	drawnvertices = 0;
	drawcalls = 0;
	framecount++;

	FXTimevar->Set((float)time);
	
	// Inform everyone that a frame has been rendered...
	OnRenderFrame->Execute(frametimems);
}
void Renderer::FlushFrames() {
	frametimes.clear();
	frameratechecktime = 0.0f;
}
void Renderer::ScheduleSaveFrameTimes() {
	frametimes.clear();
	saveframetimes = true;
}
void Renderer::SaveFrameTimes() {
	
	// write to csv
	char finalpath[MAX_PATH];
	sprintf(finalpath,"%s\\%s",exepath,"Frames.csv");
	FILE* logfile = fopen(finalpath,"wb");
	if(logfile) {
	
		char tmp[128];
		
		for(unsigned int i = 0;i < frametimes.size();i++) {
			
			sprintf(tmp,"%g\n",frametimes[i]);
			
			// Replace period by comma
			char* period = strchr(tmp,'.');
			if(period) {
				*period = ','; // excel uses , for decimal (NL)
			}
			
			fputs(tmp,logfile); // excel 2010 uses ; for cells
		}
		fclose(logfile);
		
		new Messagebox("Frames saved!");
	}
	saveframetimes = false;	
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	Generic drawing functions
*/
void Renderer::DrawTextLine(const char* text,int left,int top) {
	RECT final = {0};
	
	// Gebruik altijd een sprite
	tooltipfontsprite->Begin(D3DXSPRITE_ALPHABLEND);

	final.left = left+1;
	final.top = top+1;
	tooltipfontshadow->DrawText(tooltipfontsprite,text,-1,&final,DT_NOCLIP,D3DCOLOR_XRGB(100,100,100)); // Blur
	
	final.left = left;
	final.top = top;
	tooltipfontwhite->DrawText(tooltipfontsprite,text,-1,&final,DT_NOCLIP,D3DCOLOR_XRGB(255,255,255)); // Wit
	
	tooltipfontsprite->End();
}
void Renderer::DrawTextBlock(const char* text,int left,int top,int right,int bottom) {
	RECT final = {0};
	
	// Gebruik altijd een sprite
	tooltipfontsprite->Begin(D3DXSPRITE_ALPHABLEND);
	
	final.left = left+1;
	final.top = top+1;
	final.right = right+1;
	final.bottom = bottom+1;
	tooltipfontshadow->DrawText(tooltipfontsprite,text,-1,&final,DT_WORDBREAK,D3DCOLOR_XRGB(100,100,100));
	
	final.left = left;
	final.top = top;
	final.right = right;
	final.bottom = bottom;
	tooltipfontwhite->DrawText(tooltipfontsprite,text,-1,&final,DT_WORDBREAK,D3DCOLOR_XRGB(255,255,255));
	
	tooltipfontsprite->End();
}
void Renderer::DrawTexture(Texture* texture,float left,float top,float right,float bottom) {
	DrawTexture(texture->pointer,left,top,right,bottom);
}
void Renderer::DrawTexture(LPDIRECT3DTEXTURE9 texture,float left,float top,float right,float bottom) {
	
	// Maak quad met hoeken left/top/right/bottom
	Model* quad = models->Add();
	quad->Load2DQuad(left,top,right,bottom);
	quad->SendToGPU();

	// Zet deze texture voorop de Camera
	FXDiffusetex->SetTexture(texture);

	// En teken het vierkant recht voor de Camera
	BeginTechnique(TechStock->handle);
	DrawModel(quad);
	EndTechnique();
	
	models->Delete(quad);
}
void Renderer::DrawTextureFullScreen(LPDIRECT3DTEXTURE9 texture) {
	
	// Zet deze texture voorop de Camera
	FXDiffusetex->SetTexture(texture);

	// En teken het vierkant recht voor de Camera
	BeginTechnique(TechStock);
	DrawModel(screenspacequad);
	EndTechnique();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	ui
*/
float2 Renderer::GetCenteringCorner(float2 windowsize) {
	float2 result;
	result.x = presentparameters.BackBufferWidth/2 - windowsize.x/2;
	result.y = presentparameters.BackBufferHeight/2 - windowsize.y/2;
	return result;
}
void Renderer::DrawComponent(Component* component) {

	// Verschuif met matrix, niet met bufferrecreate
	FXMatWorld->Set(component->matWorld);
	
	// Kleur en tekst kiezen
	switch(component->type) {
		case ctLabel:
		case ctDropdown:
		case ctBase:
		case ctBevel:
		case ctWindow: {
			FXInterfacecolor->Set(component->backcolor);
			break;
		}
		case ctButton: {
			
			// Da's wel zo handig
			Button* button = (Button*)component;
			
			if(button->down) {
				FXInterfacecolor->Set(button->downcolor);
			} else if(button->hot) {
				FXInterfacecolor->Set(button->hotcolor);
			} else {
				FXInterfacecolor->Set(button->backcolor);
			}
			break;
		}
		case ctEdit: {
			
			// Da's wel zo handig
			Edit* edit = (Edit*)component;

			if(edit->focused) {
				FXInterfacecolor->Set(edit->focuscolor);
			} else {
				FXInterfacecolor->Set(edit->backcolor);
			}
			break;
		}
	}
	FX->CommitChanges();
	
	// En teken het vlak
	DrawModel(component->plane);
	
	// Label tekenen
	switch(component->type) {
		case ctDropdown: { // teken een hele boel items
			((Dropdown*)component)->DrawText(interfacefont,interfacesprite);
			break;
		}
		case ctWindow: { // centreren in titelbalk
			((Window*)component)->DrawText(interfacefont,interfacesprite);
			break;
		}
		case ctLabel: { // ietsjes inset toevoegen
			((Label*)component)->DrawText(interfacefont,interfacesprite);
			break;
		}
		case ctEdit: { // top left
			((Edit*)component)->DrawText(interfacefont,interfacesprite);
			break;
		}
		case ctButton: { // centreren
			((Button*)component)->DrawText(interfacefont,interfacesprite);
			break;
		}
		default: {} // disable compiler warning
	}
	
	// Dan de children ervan
	for(int i = component->children.size() - 1;i >= 0;i--) {
		
		Component* child = component->children[i];
		
		if(!child->IsVisible()) {
			continue;
		}
		
		// Teken eerst de component zelf...
		DrawComponent(child);
	}
}
void Renderer::DrawInterface(Interface* thisinterface) {
	if(ui->componentlist.size() == 0) {
		return;
	}
	
	alphablendstate->Set(true);
	
	BeginTechnique(TechInterface);
	
	for(int i = thisinterface->componentlist.size() - 1;i >= 0;i--) {
		
		Component* thiscomponent = thisinterface->componentlist[i];
		
		// Eerst component zelf, dan children erbovenop
		if(thiscomponent->IsVisible()) {
			DrawComponent(thiscomponent);
		}
	}
	
	EndTechnique();
	
	alphablendstate->Set(false);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	blabla
*/
float2 Renderer::GetBufferSize() {
	return float2(
		presentparameters.BackBufferWidth,
		presentparameters.BackBufferHeight);
}
void Renderer::GetResolutions(std::vector<D3DDISPLAYMODE>& list) {
	unsigned int count = d3d->GetAdapterModeCount(D3DADAPTER_DEFAULT,D3DFMT_X8R8G8B8);
	list.resize(count);
	for(unsigned int i = 0;i < count;i++) {
		d3d->EnumAdapterModes(D3DADAPTER_DEFAULT,D3DFMT_X8R8G8B8,i,&list[i]);
	}
}
float3 Renderer::GetPixelWorldRay(float2 pixelpos) {
	float2 projectionpos = PixelsToProjection(pixelpos);
	
	// Muis in view coords
	float3 mouseview(
		projectionpos.x / camera->matProj._11,
		projectionpos.y / camera->matProj._22,
		1.0f);
		
	// Muis in world space (z = z)
	return mouseview.TransformNormal(camera->matViewInverse);
}
float2 Renderer::PixelsToProjection(float2 pixelpos) {
	float2 result;
	result.x =  2.0f * pixelpos.x / (float)presentparameters.BackBufferWidth  - 1.0f;
	result.y = -2.0f * pixelpos.y / (float)presentparameters.BackBufferHeight + 1.0f;
	return result; // [-1,1] voor x en y, resultaat van P(V(W(x)))
}
float2 Renderer::ProjectionToPixels(float2 projectionpos) {
	float2 result;
	result.x =  (projectionpos.x * presentparameters.BackBufferWidth  + presentparameters.BackBufferWidth)  / 2.0f;
	result.y = (-projectionpos.y * presentparameters.BackBufferHeight + presentparameters.BackBufferHeight) / 2.0f;
	return result;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	Begin/End macros
*/
void Renderer::Begin(bool clear) {
	d3ddev->BeginScene();
	if(clear) {
		d3ddev->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
	}
}
void Renderer::End() {
	d3ddev->EndScene();
	if(schedulesave) {
		SaveBuffers();
	}
	d3ddev->Present(NULL,NULL,NULL,NULL);
}
void Renderer::EndScene() {
	d3ddev->EndScene();
}
void Renderer::Present() {
	d3ddev->Present(NULL,NULL,NULL,NULL);
}
void Renderer::BeginTechnique(D3DXHANDLE tech) {
	FX->SetTechnique(tech);
	FX->Begin(0,D3DXFX_DONOTSAVESTATE);
	FX->BeginPass(0);
}
void Renderer::BeginTechnique(FXShader* shader) {
	FX->SetTechnique(shader->handle);
	FX->Begin(0,D3DXFX_DONOTSAVESTATE);
	FX->BeginPass(0);
}
void Renderer::EndTechnique() {
	FX->EndPass();
	FX->End();	
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	Target saver
*/
void Renderer::ScheduleSaveBuffers() {
	schedulesave = true;
}
void Renderer::SaveBuffers() {
	
	char buf[512];
	
	if(scene->lights->castshadows) {
		snprintf(buf,512,"%s\\Shadowmap.png",exepath);
		D3DXSaveSurfaceToFile(buf,D3DXIFF_PNG,scene->lights->ShadowTex->GetTopSurface(),NULL,NULL);
	}

	if(options->ssaosamples > 0) {
		snprintf(buf,512,"%s\\AmbientData.png",exepath);
		D3DXSaveSurfaceToFile(buf,D3DXIFF_PNG,AmbientDataTex->GetTopSurface(),NULL,NULL);
	
		snprintf(buf,512,"%s\\AmbientFactor.png",exepath);
		D3DXSaveSurfaceToFile(buf,D3DXIFF_PNG,AmbientFactorTex->GetTopSurface(),NULL,NULL);
	}
	
	if(options->enablehdr) {
		snprintf(buf,512,"%s\\FullPingPong1.png",exepath);
		D3DXSaveSurfaceToFile(buf,D3DXIFF_PNG,FloatTex1->GetTopSurface(),NULL,NULL);
		
		snprintf(buf,512,"%s\\FullPingPong2.png",exepath);
		D3DXSaveSurfaceToFile(buf,D3DXIFF_PNG,FloatTex2->GetTopSurface(),NULL,NULL);

		snprintf(buf,512,"%s\\HalfPingPong1.png",exepath);
		D3DXSaveSurfaceToFile(buf,D3DXIFF_PNG,HalfFloatTex1->GetTopSurface(),NULL,NULL);
		
		snprintf(buf,512,"%s\\HalfPingPong2.png",exepath);
		D3DXSaveSurfaceToFile(buf,D3DXIFF_PNG,HalfFloatTex2->GetTopSurface(),NULL,NULL);
	}

	snprintf(buf,512,"%s\\FinalFrame.png",exepath);
	D3DXSaveSurfaceToFile(buf,D3DXIFF_PNG,backbuffercolor,NULL,NULL);
	
	// Laat met nieuwe UI zien dat we klaar zijn
	snprintf(buf,512,"Saved buffers to %s",exepath);
	new Messagebox(buf);
	
	schedulesave = false;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	...
*/
float2 Renderer::GetUITextExtent(const char* text) {
	RECT textrect = {0};
	interfacefont->DrawText(NULL,text,-1,&textrect,DT_CALCRECT,D3DCOLOR_XRGB(255,255,255));
	return float2(textrect.right,textrect.bottom);
}
float2 Renderer::GetTooltipTextExtent(const char* text) {
	RECT textrect = {0};
	tooltipfontwhite->DrawText(NULL,text,-1,&textrect,DT_CALCRECT,D3DCOLOR_XRGB(255,255,255));
	return float2(textrect.right,textrect.bottom);	
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	Render target net zo groot als de client rect van het window maken, alleen als de GPU naar ons luistert
*/
void Renderer::UpdateBufferSize() {
	if(d3ddev) {
		
		// Negeer als we fullscreen gebruiken...
		if(presentparameters.Windowed) {
			
			RECT rcClient;
			GetClientRect(hwnd,&rcClient);

			// Are we not minimizing?
			if(rcClient.right > 0) {
				
				// Fix de resolutie, update de renderer
				presentparameters.BackBufferWidth = rcClient.right;
				presentparameters.BackBufferHeight = rcClient.bottom;
				renderer->paused = false;
				
				// Alleen resetten en zooi opnieuw maken als de buffer > 0, anders crasht de boel
				renderer->OnLostDevice();

			// Minimizing...
			} else {
				
				// En pauzeer het zaakje
				renderer->paused = true;
			}
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	Blabla
*/
void Renderer::SetFullScreen(bool value) {
	
	// This mode is already set, don't bother
	if(!value == presentparameters.Windowed) {
		return;
	}
	
	// Tell D3D to go fullscreen (or not)
	presentparameters.Windowed = !value;
	
	if(value) {
		
		// Save old window size
		GetWindowRect(hwnd,&oldwindowsize);

		// Hide frame and stretch window to screen
		SetWindowLongPtr(hwnd,GWL_STYLE,WS_VISIBLE);
		SetWindowPos(hwnd,NULL,
			0,
			0,
			GetSystemMetrics(SM_CXSCREEN),
			GetSystemMetrics(SM_CYSCREEN),
			SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|SWP_FRAMECHANGED);
		
	} else {
		
		// Unhide frame
		SetWindowLongPtr(hwnd,GWL_STYLE,WS_VISIBLE|WS_OVERLAPPEDWINDOW);
		SetWindowPos(hwnd,NULL,0,0,0,0,SWP_NOACTIVATE|SWP_NOSENDCHANGING|SWP_NOSIZE|SWP_NOMOVE|SWP_NOZORDER|SWP_FRAMECHANGED);
		
		// Fit window to client (do NOT set everything using one SetWindowPos call!!!)
		SetWindowPos(hwnd,NULL,
			oldwindowsize.left,
			oldwindowsize.top,
			oldwindowsize.right - oldwindowsize.left,
			oldwindowsize.bottom - oldwindowsize.top,
			SWP_NOZORDER|SWP_FRAMECHANGED);
			
		// Remember the maximized state, fix?
		if((oldwindowsize.right - oldwindowsize.left) == GetSystemMetrics(SM_CXSCREEN)) {
			ShowWindow(hwnd,SW_MAXIMIZE);
		}
	}
	
	OnLostDevice();
}
void Renderer::ToggleFullScreen() {
	SetFullScreen(presentparameters.Windowed);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Renderer::SetCameraTransforms(Object* object) {
	
	// Object to World space
	FXMatWorld->Set(
		object->matWorld);
		
	// Object to View space
	float4x4 matWorldView;
	FXMatWorldView->Set(
		D3DXMatrixMultiply(&matWorldView,&object->matWorld,&camera->matView));
	
	// Object to Proj space
	float4x4 matWorldViewProj;
	FXMatWorldViewProj->Set(
		D3DXMatrixMultiply(&matWorldViewProj,&object->matWorld,&camera->matViewProj));	
}
void Renderer::SetLightTransforms(Object* object,float4x4 lightprojection) {
	// Object to sun Proj space
	float4x4 matLightWorldViewProj;
	FXMatLightWorldViewProj->Set(
		D3DXMatrixMultiply(&matLightWorldViewProj,&object->matWorld,&lightprojection));
}
void Renderer::SetMaterial(Object* object) {
		
	Material* material = object->material;
	
	// Diffuse, only apply when texture is used
	if(material->diffusetex) {
		FXMaterialdiffuse->Set(&material->diffuse);
		FXDiffusetex->SetTexture(material->diffusetex);
	}
	
	// Idem for specular
	if(material->speculartex) {
		FXMaterialspecular->Set(&material->specular);
		FXMaterialshininess->Set(&material->shininess);
		FXSpeculartex->SetTexture(material->speculartex);
	}

	// Tiling for multitexturing (else not needed?)
	if(material->multitex) {
		FXMaterialTiling->Set(material->tiling);
		FXMaterialMixer->Set(material->mixer);
	}

	// And apply special textures
	if(material->normaltex) {
		FXNormaltex->SetTexture(material->normaltex);
	}
	if(material->parallaxtex) {
		FXParallaxtex->SetTexture(material->parallaxtex);
	}
	if(material->ambienttex) {
		FXAmbienttex->SetTexture(material->ambienttex);
	}
	
	// TODO: zorg ervoor dat we deze calls niet te vaak maken
	cullmodestate->Set(material->cullmode);
	fillmodestate->Set(material->fillmode);
	alphateststate->Set(material->alphatest);
	alphablendstate->Set(material->alphablend);
	
	// blendsoort per object instellen?	
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Renderer::SetSSAOVariables(Object* thisobject) {
	
	// Optimalisatie vs. SetCameraTransforms: matWorld niet nodig!
	
	// Object to View space
	float4x4 matWorldView;
	FXMatWorldView->Set(
		D3DXMatrixMultiply(&matWorldView,&thisobject->matWorld,&camera->matView));
	
	// Object to Proj space
	float4x4 matWorldViewProj;
	FXMatWorldViewProj->Set(
		D3DXMatrixMultiply(&matWorldViewProj,&thisobject->matWorld,&camera->matViewProj));	
	
	// We samplen diffuse voor alpha
	if(thisobject->material->diffusetex) {
		FXDiffusetex->SetTexture(thisobject->material->diffusetex);
	}
	
	FX->CommitChanges();
}
void Renderer::SetShadowMapVariables(Object* object,float4x4 lightprojection) {
	
	// Transformeren vanaf lichtbron X
	SetLightTransforms(object,lightprojection);
	
	// We samplen diffuse voor alpha
	if(object->material->diffusetex) {
		FXDiffusetex->SetTexture(object->material->diffusetex);
	}
	
	FX->CommitChanges();
}
void Renderer::SetGenericVariables(Object* object,float4x4 lightprojection) {
	
	// Transformation matrices
	SetCameraTransforms(object);
	
	// Sample from here
	SetLightTransforms(object,lightprojection);
	
	// Textures, diffuse, specular, ...
	SetMaterial(object);
	
	// Send to GPU
	FX->CommitChanges();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Renderer::DrawModel(Model* model) {
	// Add to stats
	drawnfaces += model->numfaces;
	drawnvertices += model->numvertices;
	drawcalls++;
	
	d3ddev->SetStreamSource(0, model->vertexbuffer,0, sizeof(VERTEX));
	d3ddev->SetIndices(model->indexbuffer);
	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, model->numvertices, 0, model->numfaces);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	This one passes all the created batches
*/
void Renderer::PassShader(FXShader* thisshader) {
	
	// Skip empty shaders the low cost way (don't call BeginPass)
	if(thisshader->unused) {
		return;
	}
	
	BeginTechnique(thisshader->handle);
	
	for(std::list<Object*>::iterator i = thisshader->begin;i != thisshader->end;i++) {
		Object* object = *i;
		
		if(!object->visible) {
			continue;
		}
		
		// Get distance to nearest part of bounding sphere
		float3 modeldir = object->worldcenter - camera->pos;
		float distance = std::max(0.0f,modeldir.Length() - object->worldr);
		
		// Determine which LOD we should draw
		DetailLevel* detaillevel = object->GetDetailLevel(distance);
		if(detaillevel) {
			SetGenericVariables(object,scene->lights->matLightViewProj); // Sample shadows from ONE caster
			DrawModel(detaillevel->model);			
		}
	}

	EndTechnique();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	Macro for screen space effects
*/
void Renderer::PassEffect(FXShader* effect,LPDIRECT3DSURFACE9 target) {
	d3ddev->SetRenderTarget(0,target);
	BeginTechnique(effect);
	DrawModel(screenspacequad);
	EndTechnique();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	Teken een scene, en dus niet alles wat in de buffers zit
*/
void Renderer::DrawScene(Scene* scene) {
	
	if(!scene) {
		return;
	}
	
	UpdateTime();

	// Zorg ervoor dat de speciale maps fatsoenlijk getekend worden
	d3ddev->SetVertexDeclaration(standard);
	
	// Deze staan vast
	d3ddev->SetRenderState(D3DRS_ZENABLE,1);
	d3ddev->SetRenderState(D3DRS_ZWRITEENABLE,1);
	
	// Fix de per-objectstates
	fillmodestate->Set(D3DFILL_SOLID); // fill shadow/ssao map with solid colors
	cullmodestate->Set(D3DCULL_NONE); // cull back faces (lessens load)
	alphateststate->Set(false); // use manual alpha testing
	alphablendstate->Set(false);
	
	// Determine which light casts shadows (if there is any)
	if(options->shadowmapsize > 0 && scene->lights->castshadows) {
		
		// Hier moet FX->SetTexture(FXShadowtex,ShadowTex); unbound worden...
		for(int i = 0;i < 16;i++) {
			d3ddev->SetTexture(i,NULL);
		}
		
		// Vul de shadowmap op...
		d3ddev->SetRenderTarget(0,scene->lights->ShadowTex->GetTopSurface());
		d3ddev->SetDepthStencilSurface(scene->lights->ShadowTex->GetDepthTopSurface());
		d3ddev->Clear(0, 0, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(255,255,255,255), 1, 0); // TODO: clear R only?
		
		BeginTechnique(TechShadowMap);
		
		for(std::list<Object*>::iterator i = scene->objects->begin();i != scene->objects->end();i++) {
			Object* object = *i;
			
			if(!object->castshadows) {
				continue;
			}
			
			// Get distance to nearest part of bounding sphere
			float3 modeldir = object->worldcenter - camera->pos;
			float distance = std::max(0.0f,modeldir.Length() - object->worldr);
			if(distance < options->shadowdistance) {
				
				// Determine which LOD we should draw
				DetailLevel* detaillevel = object->GetDetailLevel(distance);
				if(detaillevel) {
					SetShadowMapVariables(object,scene->lights->matLightViewProj); // Sample shadows from ONE caster
					DrawModel(detaillevel->model);			
				}	
			}
		}
		
		EndTechnique();
		
		// Use this texture as the final
		FXShadowtex->SetTexture(scene->lights->ShadowTex); // TODO: Deze moet unbound worden???
	}
	
	// Vul de SSAO-map...
	if(options->ssaosamples > 0) {
		
		d3ddev->SetRenderTarget(0,AmbientDataTex->GetTopSurface());
		d3ddev->SetDepthStencilSurface(backbufferdepth);
		d3ddev->Clear(0, 0, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0.0f, 1.0f, 0);
		
		// Render de SSAO-coefficient naar een texture ter grootte van het scherm
		BeginTechnique(TechAmbientMap);
		
		for(std::list<Object*>::iterator i = scene->objects->begin();i != scene->objects->end();i++) {
			Object* object = *i;
			
			if(!object->visible) {
				continue;
			}
			
			// Get distance to nearest part of bounding sphere
			float3 modeldir = object->worldcenter - camera->pos;
			float distance = std::max(0.0f,modeldir.Length() - object->worldr);
			if(distance < options->ssaodistance) {
				
				// Determine which LOD we should draw
				DetailLevel* detaillevel = object->GetDetailLevel(distance);
				if(detaillevel) {
					SetSSAOVariables(object);
					DrawModel(detaillevel->model);						
				}
			}
		}
		
		EndTechnique();
		
		// Texture wordt d.m.v. FXScreenTex gelezen
	}
	
	// Nu gaan we het beeld vullen (m.b.v. de special maps)
	if(options->enablehdr) {
		d3ddev->SetRenderTarget(0,FloatTex1->GetTopSurface());
	} else {
		d3ddev->SetRenderTarget(0,backbuffercolor);
	}
	d3ddev->SetDepthStencilSurface(backbufferdepth);
	
	// TODO: Clear altijd?
	d3ddev->Clear(0, 0, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
	
	// Shaders, Per vertex
	// vertex decl. staat al goed
	for(unsigned int i = 0;i < 4;i++) {
		PassShader(shaders[i]);
	}
	
	// Shaders, Per pixel
	for(unsigned int i = 4;i < 12;i++) {
		PassShader(shaders[i]);
	}
	
	// Shaders, Per pixel, tangent space
	d3ddev->SetVertexDeclaration(normalmap);
	for(unsigned int i = 12;i < 17;i++) {
		PassShader(shaders[i]);
	}
	
	// Zorg ervoor dat de screen space quads snel getekend worden
	d3ddev->SetVertexDeclaration(standard);
	
	// Zorgt voor meer tempo
	d3ddev->SetRenderState(D3DRS_ZENABLE,0);
	d3ddev->SetRenderState(D3DRS_ZWRITEENABLE,0);
	
	// Zet de per-objectstates vast voor screen space quads
	fillmodestate->Set(D3DFILL_SOLID);
	cullmodestate->Set(D3DCULL_CCW);
	alphateststate->Set(false);
	alphablendstate->Set(false);
	
	// Pas tone mapping en bloom toe
	if(options->enablehdr) {
		
		// Apply occlusion to FP buffers only
		if(options->ssaosamples > 0) {
			
			// Zorg ervoor dat het volgende in de samplers terecht komt:
			// 1 bevat de data benodigd voor de occlusion factor, zonder blur
			// 2 bevat de occlusion factor, zonder blur
			// 3 bevat de de originele kleur, waar de factor overheen moet
			
			// Use normal and depth data from AmbientDataTex to compute an occlusion factor (not blurred)
			FXScreentex1->SetTexture(AmbientDataTex);
			PassEffect(TechSSAOFactor,AmbientFactorTex->GetTopSurface());
			
			// Use normal and depth data from AmbientDataTex
			FXScreentex2->SetTexture(AmbientFactorTex); // occlusion factor, 100% size
			FXScreentex3->SetTexture(FloatTex1); // color to modify
			PassEffect(TechSSAOBlur,FloatTex2->GetTopSurface()); // to this
		}
		
		// Zorg ervoor dat het volgende in de samplers terecht komt:
		// 1 bevat het origineel in FP-formaat
		// 2 bevat de bright pass, zonder blur
		// 3 bevat de bright pass, met blur
		
		// Gooi een bright pass over FloatTex heen en sla op in HalfFloatTex1
		if(options->ssaosamples > 0) {
			FXScreentex1->SetTexture(FloatTex2);
		} else {
			FXScreentex1->SetTexture(FloatTex1);
		}
		PassEffect(TechBrightPass,HalfFloatTex1->GetTopSurface()); // to this
		
		// Gooi een gaussian blur over HalfFloatTex1 heen en sla op in HalfFloatTex2
		FXScreentex2->SetTexture(HalfFloatTex1); // plak deze op de quad, deze gaan we uitsmeren
		PassEffect(TechBrightBlur,HalfFloatTex2->GetTopSurface());
		
		// Sla als resultaat de tonemap van het origineel plus de blur op
		FXScreentex3->SetTexture(HalfFloatTex2); // uitgesmeerde blur
		PassEffect(TechToneMap,backbuffercolor);
	}
	
	// Teken de GUI, alle toplevels
	DrawInterface(ui); // Teken alle top levels
	
	// En als allerlaatst de tooltip
	if(tooltipmode != 0) {
		PrintTooltip();
		DrawTextLine(tooltip,0,0);
	}
}
