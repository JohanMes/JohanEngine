#ifndef RENDERER_INCLUDE
#define RENDERER_INCLUDE

#include "Object.h"
#include "FXShader.h"
#include "Camera.h"
#include "Component.h"
#include "State.h"
#include "Clock.h"
#include "FXVariable.h"
#include "Scene.h"
#include "Options.h"
#include "Interface.h"
#include "float2.h"
#include "TimeEvent.h"
#include "RenderTarget.h"

#if BUILDING_DLL
#define DLLIMPORT __declspec(dllexport)
#else
#define DLLIMPORT __declspec(dllimport)
#endif

class DLLIMPORT Renderer {
	// Statemachines
	State* cullmodestate;
	State* fillmodestate;
	State* alphateststate;
	State* alphablendstate;
	
	// Frametijdmeter
	Clock* frameclock;

	LPDIRECT3DVERTEXDECLARATION9 standard; // Inhoud per vertex
	LPDIRECT3DVERTEXDECLARATION9 normalmap;
	
	// Size editing
	RECT oldwindowsize;
	
	// SSAO
	RenderTarget* AmbientDataTex; // bevat float4(Nx,Ny,Nz,Dview)
	RenderTarget* AmbientFactorTex; // zonder blur
	
	// FP16-pingpongbuffers op 100% grootte
	RenderTarget* FloatTex1; // hier worden HDR-kleuren naar geschreven
	RenderTarget* FloatTex2; // iets...
	
	// FP16-pingpongbuffers op 50% grootte
	RenderTarget* HalfFloatTex1; // hier wordt de bright pass uitgevoerd
	RenderTarget* HalfFloatTex2; // hier wordt bright pass geblurd
	
	// Met dit model passen we postprocessing toe
	Model* screenspacequad;
	
	// tooltipspul
	int tooltipmode;
	char tooltip[500];
	
	// Counters
	int drawnfaces;
	int drawnvertices;
	int drawcalls;
	
	// Zooi die voor bufferopslag zorgt
	bool schedulesave;
	
	// Frametijden enzo
	std::vector<double> frametimes;
	double frameratechecktime;
	int framecount;
	double lotime;
	double hitime;
	bool saveframetimes;
	int timemulti;
	double time;
	
	// Variabelen van GPU
	FXVariable* FXMatWorld;
	FXVariable* FXMatWorldView;
	FXVariable* FXMatWorldViewProj;
	FXVariable* FXMatLightWorldViewProj;
	
	FXVariable* FXMaterialdiffuse;
	FXVariable* FXMaterialspecular;
	FXVariable* FXMaterialshininess;
	FXVariable* FXMaterialTiling;
	FXVariable* FXMaterialMixer;
	
	FXVariable* FXWidth;
	FXVariable* FXHeight;
	FXVariable* FXInvwidth;
	FXVariable* FXInvheight;
	
	FXVariable* FXTimevar;
	FXVariable* FXInterfacecolor;
	
	// Texturepointers naar GPU
	FXVariable* FXScreentex1;
	FXVariable* FXScreentex2;
	FXVariable* FXScreentex3;
	FXVariable* FXDiffusetex;
	FXVariable* FXSpeculartex;
	FXVariable* FXNormaltex;
	FXVariable* FXParallaxtex;
	FXVariable* FXAmbienttex;
	FXVariable* FXShadowtex;
	
	LPD3DXFONT tooltipfontwhite; // Het GPU-font (wit)
	LPD3DXFONT tooltipfontshadow; // Het GPU-font (zwartborder)
	LPD3DXFONT interfacefont; // Het GPU-font (ui)
	LPD3DXSPRITE interfacesprite;
	LPD3DXSPRITE tooltipfontsprite;
	
	// Rendering stuff
	void SetCameraTransforms(Object* object);
	void SetLightTransforms(Object* object,float4x4 lightprojection);
	void SetMaterial(Object* object);
	void SetSSAOVariables(Object* object);
	void SetShadowMapVariables(Object* object,float4x4 lightprojection);
	void SetGenericVariables(Object* object,float4x4 lightprojection);
	void PassShader(FXShader* shader);
	void PassEffect(FXShader* effect,LPDIRECT3DSURFACE9 target);
	
	// Tooltipspul
	void PrintTooltip();

	// Misc.
	void UpdateTime();
	
	public:
		Renderer();
		~Renderer();
		
		// Vaste shaders
		FXShader* TechInterface;
		FXShader* TechShadowMap;
		FXShader* TechAmbientMap; // saves normal in view coords
		FXShader* TechSSAOFactor; // computes SSAO coeff
		FXShader* TechSSAOBlur;
		FXShader* TechGaussian;
		FXShader* TechBrightPass;
		FXShader* TechBrightBlur;
		FXShader* TechToneMap;
		FXShader* TechStock;

		// Deze mogen de objecten gebruiken
		std::vector<FXShader*> shaders;
		
		// Pointers naar de automatisch gemaakte buffers
		LPDIRECT3DSURFACE9 backbuffercolor;
		LPDIRECT3DSURFACE9 backbufferdepth;
		
		// Alle opties, worden na een Reset toegepast
		LPDIRECT3D9 d3d; // COM-ding
		D3DADAPTER_IDENTIFIER9 properties; // huidige videokaarteigenschappen
		D3DPRESENT_PARAMETERS presentparameters;
		
		// pause
		bool paused;
		float framerate;
		
		// Executes before rendering each frame
		TimeEvent* OnRenderFrame;

		// Handig voor de ui e.d.
		float2 GetUITextExtent(const char* text);
		float2 GetTooltipTextExtent(const char* text);
		float2 GetCenteringCorner(float2 windowsize);
		void UpdateBufferSize(); // adapt buffer to window
		
		// Klok
		int GetTimeMulti();
		void SetTimeMulti(int value);
		double GetTime(); // sum of frame times ('exact')
		int GetClockTimeSec(); // seconds, [0..59,0..59 ...]
		int GetClockTimeMins(); // same idea as above
		int GetClockTimeHours(); // idem
		void SetTime(int hours,int mins);
		void AddTime(int hours,int mins);
		
		// Tooltipzooi
		void ShowTooltip(int mode);
		void ToggleTooltip();
		void FlushFrames();
		void ScheduleSaveFrameTimes();
		void SaveFrameTimes();
		
		// Fullscreen
		void SetFullScreen(bool value);
		void ToggleFullScreen();
		
		// Drawing
		void DrawTextLine(const char* text,int left,int top);
		void DrawTextBlock(const char* text,int left,int top,int right,int bottom);
		void DrawTexture(Texture* texture,float left,float top,float right,float bottom);
		void DrawTexture(LPDIRECT3DTEXTURE9 texture,float left,float top,float right,float bottom);
		void DrawTextureFullScreen(LPDIRECT3DTEXTURE9 texture);
		void DrawScene(Scene* scene);
		void DrawModel(Model* thismodel);
		void DrawInterface(Interface* thisinterface);
		void DrawComponent(Component* component);
	
		// Macro's voor passes
		void Begin(bool clear);
		void End();
		void EndScene();
		void Present();
		void BeginTechnique(D3DXHANDLE tech);
		void BeginTechnique(FXShader* shader);
		void EndTechnique();
		
		// Device lost rescue en dergelijke
		void ReleaseResources();
		void OnLostDevice();
		void OnResetDevice();
			
		// Save render targets to HDD
		void ScheduleSaveBuffers();
		void SaveBuffers();
		
		// Misc.
		float2 GetBufferSize();
		void GetResolutions(std::vector<D3DDISPLAYMODE>& list);
		float3 GetPixelWorldRay(float2 pixelpos);
		float2 PixelsToProjection(float2 pixelpos);
		float2 ProjectionToPixels(float2 projectionpos);
};

extern Renderer* renderer;

#endif
