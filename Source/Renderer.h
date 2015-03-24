#ifndef RENDERER_INCLUDE
#define RENDERER_INCLUDE

#include <cstdio>
#include <vector>
using std::vector;
#include <d3d9.h>
#include <d3dx9.h>

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

struct FrameInfo {
	int facecount;
	int vertexcount;
	int callcount;
	double frametime; // current frametime in seconds
	double frametimelow; // lowest frametime over X time
	double frametimehigh; // current frametime over X time
	double framerate; // current FPS averaged over X time
	int index; // count since start
};

class Scene;
class FXShader;
class FXVariable;
class State;
class Clock;
class RenderTarget;
class Model;
class Object;
class float2;
class float3;
class float4;
class float4x4;
class Texture;
class Interface;
class Component;
class TimeEvent;

class DLLIMPORT Renderer {
	private:
		Clock* frameclock; // calculates frame times
		State* cullmodestate;
		State* fillmodestate;
		State* alphateststate;
		State* alphablendstate;
		State* alpharefstate;
		State* alphafuncstate;
		State* alphasrcblendstate;
		State* alphadestblendstate;
		State* zenablestate;
		State* zwriteenablestate;
		RenderTarget* AmbientDataTex; // 32bit float for normal/depth per pixel
		RenderTarget* AmbientFactorTex; // calculated ambient darkening factor
		RenderTarget* FloatTex1; // HDR pass helper
		RenderTarget* FloatTex2; // HDR pass helper
		RenderTarget* HalfFloatTex1; // HDR pass helper, half resolution
		RenderTarget* HalfFloatTex2; // HDR pass helper, half resolution
		FXVariable* FXMatWorld;
		FXVariable* FXMatWorldView;
		FXVariable* FXMatWorldViewProj;
		FXVariable* FXMatLightWorldViewProj;
		FXVariable* FXMaterialdiffuse;
		FXVariable* FXMaterialspecular;
		FXVariable* FXMaterialshininess;
		FXVariable* FXMaterialTiling;
		FXVariable* FXMaterialMixer;
		FXVariable* FXFilter;
		FXVariable* FXNumaniso;
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
		Model* screenspacequad; // use this model to apply screen space effects
		LPD3DXEFFECT FX; // effects framework interface
		LPDIRECT3DVERTEXDECLARATION9 standard; // position, texcoord, normal
		LPDIRECT3DVERTEXDECLARATION9 normalmap; // position, texcoord, normal, tangent, binormal
		LPD3DXFONT tooltipfontwhite; // Tooltip font
		LPD3DXFONT tooltipfontshadow; // Tooltip font shadow
		LPD3DXFONT interfacefont; // Interface font
		LPD3DXSPRITE interfacesprite; // dummy used to draw sprites
		LPD3DXSPRITE tooltipfontsprite; // TODO: not needed
		LPDIRECT3DSURFACE9 backbuffercolor; // auto created back buffer
		LPDIRECT3DSURFACE9 backbufferdepth; // auto created depth buffer
		LPDIRECT3D9 d3d; // COM object
		D3DADAPTER_IDENTIFIER9 properties; // graphics card properties
		TooltipMode tooltipmode;
		char tooltip[1024]; // tooltip print/draw buffer
		vector<FrameInfo> frameinfo; // timing info of previous frames
		FrameInfo currentframe;
		int timemulti;
		double time; // TODO: move to scene?
		bool saveframes;
		bool valid;
		HWND hwnd;
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
		FrameInfo* GetLastFrame();
	public:
		Renderer(HWND hwnd);
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
		void SetFullScreen(bool value);
		void ToggleFullScreen();
		void DrawTextLine(const char* text,int left,int top);
		void DrawTextBlock(const char* text,int left,int top,int right,int bottom);
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
		void OnLostDevice();
		void OnResetDevice();
		void ScheduleSaveBuffers(); // save when we've finished next frame
		void GetResolutions(std::vector<D3DDISPLAYMODE>& list);
		float3 GetPixelWorldRay(float2 pixelpos);
		float2 PixelsToProjection(float2 pixelpos);
		float2 ProjectionToPixels(float2 projectionpos);
		float GetFrameRate();
		LPDIRECT3DSURFACE9 GetBackBufferColor();
		LPDIRECT3DSURFACE9 GetBackBufferDepth();
		LPDIRECT3D9 GetD3D();
		D3DADAPTER_IDENTIFIER9 GetAdapterInfo();
		bool IsValid();
		
		// TODO: private?
		bool paused;
		vector<FXShader*> shaders;
		D3DPRESENT_PARAMETERS presentparameters;
		
		// Events
		TimeEvent* OnRenderFrame;
};

namespace Globals {
	extern Renderer* renderer;
}

#endif
