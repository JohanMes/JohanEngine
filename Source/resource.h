#ifndef	RES_INCLUDE
#define RES_INCLUDE

#include <d3d9.h>
#include <d3dx9.h>
#include <vector>

#include "float2.h"
#include "float3.h"
#include "float4x4.h"

#define pi 3.141592654
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	Rommel
*/
extern LPD3DXEFFECT FX;
extern LPDIRECT3DDEVICE9 d3ddev; // De videokaart
extern HWND hwnd; // Het venster
extern char exepath[MAX_PATH]; // Current directory
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	Utils
*/
void InitEngine();
void DeleteEngine();
std::vector<float3> ComputeSSAOKernel(int size);
std::vector<float3> ComputeGaussKernel(int size,float sigma2);
float RandomRange(float min,float max);
char* ExtractFileName(const char* text,char* result);
char* ExtractFilePath(const char* text,char* result);
char* ExtractFileExt(const char* text,char* result);
char* ChangeFileExt(const char* text,const char* newext,char* result);
void GetCPUName(char* result,int size);
void GetGPUName(char* result,int size);
int GetFontSizePt(int points,HDC hdc);
void GetCSIDLPath(int csidl,char* result);
void SafeRelease(IUnknown* resource);
int CountChar(const char* text,char token);
char* TrimLeft(char* text);
char* TrimRight(char* text);
char* Trim(char* text);
bool FileExists(const char* path);
float DegToRad(float degin);
float RadToDeg(float radin);
void EnumerateFiles(const char* folder,const char* filter,std::vector<char*>* list);
unsigned int Faculty(unsigned int n);
unsigned int Binomial(unsigned int n,unsigned int k);
float3 GramSchmidt2(float3 v1,float3 x2);
float3 GramSchmidt3(float3 v1,float3 v2,float3 x3);
bool fequals(float x1,float x2);
float4x4 LookAt(const D3DXVECTOR3& from,const D3DXVECTOR3& to);
void GetFullPath(const char* file,const char* folder,char* fullpath);
char* StripQuotes(const char* text);

#endif
