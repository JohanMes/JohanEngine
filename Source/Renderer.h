#ifndef RENDERER_INCLUDE
#define RENDERER_INCLUDE

#include <cstdio>
#include <vector>
using std::vector;
#include "Object.h" // drawing
#include "State.h"
#include "Clock.h"
#include "RenderTarget.h"
#include "FXVariable.h"
#include "FXShader.h"
#include "float2.h"
#include "float3.h"
#include "float4.h"
#include "float4x4.h"
#include "TimeEvent.h"
#include "Interface.h"
#include "Camera.h"
#include "Scene.h"

/*
#include "Component.h"
#include "Options.h"
#include "Dialogs.h"
#include "Resource.h"
*/

#if BUILDING_DLL
#define DLLIMPORT __declspec(dllexport)
#else
#define DLLIMPORT __declspec(dllimport)
#endif

enum TooltipMode {
	ttmNone,
	ttmFramerate,
	ttmTiming,
	ttmTimingDevice,
	ttmFull,
};

class Scene;
class FXShader;

class DLLIMPORT Renderer {
	private:
		State* cullmodestate;
		State* fillmodestate;
		State* alphateststate;
		State* alphablendstate;
		Clock* frameclock; // calculates frame times
		LPDIRECT3DVERTEXDECLARATION9 standard; // position, texcoord, normal
		LPDIRECT3DVERTEXDECLARATION9 normalmap; // position, texcoord, normal, tangent, binormal
		RenderTarget* AmbientDataTex; // 32bit float for normal/depth per pixel
		RenderTarget* AmbientFactorTex; // calculated ambient darkening factor
		RenderTarget* FloatTex1; // HDR pass helper
		RenderTarget* FloatTex2; // HDR pass helper
		RenderTarget* HalfFloatTex1; // HDR pass helper, half resolution
		RenderTarget* HalfFloatTex2; // HDR pass helper, half resolution
		Model* screenspacequad; // use this model to apply screen space effects
		TooltipMode tooltipmode;
		char tooltip[512]; // tooltip print/draw buffer
		int drawnfaces;
		int drawnvertices;
		int drawcalls;
		bool schedulesave;
		vector<double> frametimes; // list of past frame times
		double frameratechecktime;
		int framecount;
		double lotime;
		double hitime;
		bool saveframetimes;
		int timemulti;
		double time;
		float framerate;
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
		FXVariable* FXPurecolor;
		FXVariable* FXScreentex1;
		FXVariable* FXScreentex2;
		FXVariable* FXScreentex3;
		FXVariable* FXDiffusetex;
		FXVariable* FXSpeculartex;
		FXVariable* FXNormaltex;
		FXVariable* FXParallaxtex;
		FXVariable* FXAmbienttex;
		FXVariable* FXShadowtex;
		FXShader* TechShadowMap;
		FXShader* TechAmbientMap; // saves normal in view coords
		FXShader* TechSSAOFactor; // computes SSAO coeff
		FXShader* TechSSAOBlur;
		FXShader* TechGaussian;
		FXShader* TechBrightPass;
		FXShader* TechBrightBlur;
		FXShader* TechToneMap;
		FXShader* TechStock;
		LPD3DXFONT tooltipfontwhite; // Tooltip font
		LPD3DXFONT tooltipfontshadow; // Tooltip font shadow
		LPD3DXFONT interfacefont; // Interface font
		LPD3DXSPRITE interfacesprite; // dummy used to draw sprites
		LPD3DXSPRITE tooltipfontsprite; // TODO: not needed
		void SetCameraTransforms(Object* object);
		void SetLightTransforms(Object* object,float4x4 lightprojection);
		void SetMaterial(Object* object);
		void SetSSAOVariables(Object* object);
		void SetShadowMapVariables(Object* object,float4x4 lightprojection);
		void SetGenericVariables(Object* object,float4x4 lightprojection);
		void PassShader(FXShader* shader);
		void PassEffect(FXShader* effect,LPDIRECT3DSURFACE9 target);
		void PrintTooltip();
		void UpdateTime();
		void SaveBuffers();
	public:
		Renderer();
		~Renderer();
		float2 GetUITextExtent(const char* text);
		float2 GetTooltipTextExtent(const char* text);
		float2 GetCenteringCorner(float2 windowsize);
		void UpdateBufferSize(); // adapt buffer to window
		int GetTimeMulti();
		void SetTimeMulti(int value);
		double GetTime(); // sum of frame times ('exact')
		int GetClockTimeSec(); // seconds, [0..59,0..59 ...]
		int GetClockTimeMins(); // same idea as above
		int GetClockTimeHours(); // idem
		void SetTime(int hours,int mins);
		void AddTime(int hours,int mins);
		void SetTooltipMode(TooltipMode mode);
		void FlushFrames();
		void ScheduleSaveFrameTimes();
		void SaveFrameTimes();
		void SetFullScreen(bool value);
		void ToggleFullScreen();
		void DrawTextLine(const char* text,int left,int top);
		void DrawTextBlock(const char* text,int left,int top,int right,int bottom);
		void DrawTexture(Texture* texture,float left,float top,float right,float bottom);
		void DrawTexture(LPDIRECT3DTEXTURE9 texture,float left,float top,float right,float bottom);
		void DrawTextureFullScreen(LPDIRECT3DTEXTURE9 texture);
		void DrawScene(Scene* scene);
		void DrawModel(Model* thismodel);
		void DrawInterface(Interface* thisinterface);
		void DrawComponent(Component* component);
		void Begin(bool clear);
		void End();
		void EndScene();
		void Present();
		void BeginTechnique(D3DXHANDLE tech);
		void BeginTechnique(FXShader* shader);
		void EndTechnique();
		void ReleaseResources();
		void OnLostDevice();
		void OnResetDevice();
		void ScheduleSaveBuffers(); // save when we've finished next frame
		float2 GetBufferSize();
		void GetResolutions(std::vector<D3DDISPLAYMODE>& list);
		float3 GetPixelWorldRay(float2 pixelpos);
		float2 PixelsToProjection(float2 pixelpos);
		float2 ProjectionToPixels(float2 projectionpos);
		float GetFrameRate();
		
		// TODO: private?
		bool paused;
		vector<FXShader*> shaders;
		LPDIRECT3DSURFACE9 backbuffercolor;
		LPDIRECT3DSURFACE9 backbufferdepth;
		LPDIRECT3D9 d3d; // COM-ding
		D3DADAPTER_IDENTIFIER9 properties; // huidige videokaarteigenschappen
		D3DPRESENT_PARAMETERS presentparameters;

		// Events
		TimeEvent* OnRenderFrame;
};

extern Renderer* renderer;

#endif
