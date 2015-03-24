#ifndef	RES_INCLUDE
#define RES_INCLUDE

#include <d3d9.h>
#include <d3dx9.h>
#include <vector>
using std::vector;
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <cstdio>
#include <shlobj.h>

class float2;
class float3;
class float4;
class float4x4;

#if BUILDING_DLL
#define DLLIMPORT __declspec(dllexport)
#else
#define DLLIMPORT __declspec(dllimport)
#endif

#define cpuid(func,ax,bx,cx,dx)\
	__asm__ __volatile__ ("cpuid": "=a" (ax), "=b" (bx), "=c" (cx), "=d" (dx) : "a" (func));

// All global variables
namespace Globals {
	// renderer, camera, etc defined in their specific headers
	extern DLLIMPORT LPDIRECT3DDEVICE9 d3ddev; // d3d interface to graphics card
	extern DLLIMPORT char exepath[MAX_PATH]; // current directory
}

// All global utilities
namespace Utils {
	DLLIMPORT void InitEngine();
	DLLIMPORT void DeleteEngine();
	DLLIMPORT vector<float3> ComputeSSAOKernel(int size);
	DLLIMPORT vector<float3> ComputeGaussKernel(int size,float sigma2);
	DLLIMPORT float RandomRange(float min,float max);
	DLLIMPORT char* ExtractFileName(const char* text,char* result);
	DLLIMPORT DLLIMPORT char* ExtractFilePath(const char* text,char* result);
	DLLIMPORT char* ExtractFileExt(const char* text,char* result);
	DLLIMPORT char* ChangeFileExt(const char* text,const char* newext,char* result);
	DLLIMPORT void GetCPUName(char* result,int size);
	DLLIMPORT void GetGPUName(char* result,int size);
	DLLIMPORT int GetFontSizePt(int points,HDC hdc);
	DLLIMPORT void GetCSIDLPath(int csidl,char* result);
	DLLIMPORT void SafeRelease(IUnknown* resource);
	DLLIMPORT int CountChar(const char* text,char token);
	DLLIMPORT char* TrimLeft(char* text);
	DLLIMPORT char* TrimRight(char* text);
	DLLIMPORT char* Trim(char* text);
	DLLIMPORT bool FileExists(const char* path);
	DLLIMPORT float DegToRad(float degin);
	DLLIMPORT float RadToDeg(float radin);
	DLLIMPORT void EnumerateFiles(const char* folder,const char* filter,std::vector<char*>* list);
	DLLIMPORT unsigned int Faculty(unsigned int n);
	DLLIMPORT unsigned int Binomial(unsigned int n,unsigned int k);
	DLLIMPORT float3 GramSchmidt2(float3 v1,float3 x2);
	DLLIMPORT float3 GramSchmidt3(float3 v1,float3 v2,float3 x3);
	DLLIMPORT bool fequals(float x1,float x2);
	DLLIMPORT float4x4 LookAt(const D3DXVECTOR3& from,const D3DXVECTOR3& to);
	DLLIMPORT bool IsFullPath(const char* path);
	DLLIMPORT void GetFullPath(const char* file,const char* folder,char* fullpath);
	DLLIMPORT char* StripQuotes(const char* text);
	DLLIMPORT double GetTimerPeriod();
	DLLIMPORT float DegToRad();
	DLLIMPORT float RadToDeg();
}

#endif
