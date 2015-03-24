#include "Resource.h"
#include "float2.h"
#include "float3.h"
#include "float4x4.h"
#include "Models.h"
#include "Textures.h"
#include "Scene.h"
#include "Renderer.h"
#include "Console.h"
#include "Camera.h"
#include "Interface.h"
#include "Options.h"

// All global parameters
namespace Globals {
	class Renderer* renderer = NULL;
	class Camera* camera = NULL;
	class Interface* ui = NULL;
	class Scene* scene = NULL;
	class Models* models = NULL;
	class Textures* textures = NULL;
	class Options* options = NULL;
	class Console* console = NULL;
	LPDIRECT3DDEVICE9 d3ddev; // d3d interface to graphics card
	char exepath[MAX_PATH] = ""; // current directory
}

// All global utilities
namespace Utils {
	void InitEngine(HWND hwnd) {
		// Reset random seed
		srand(GetTickCount());

		// Set exepath
		char fullexepath[MAX_PATH];
		GetModuleFileName(NULL,fullexepath,MAX_PATH);
		ExtractFilePath(fullexepath,Globals::exepath);

		// Create all globals
		Globals::console = new Console(); // Create early so we can use it for debug purposes
		Globals::options = new Options("JohanGame.ini"); // Load options before creating device (saves a reset)
		Globals::models = new Models();
		Globals::textures = new Textures();
		Globals::renderer = new Renderer(hwnd);
		Globals::camera = new Camera();
		Globals::scene = new Scene(); // TODO: let user create one
		Globals::ui = new Interface();

		// Add precreated stuff to ui
		Globals::ui->AddComponent(Globals::console);
		Globals::ui->AddComponent(Globals::options);

		// Reset (start) renderer
		if(Globals::options->fullscreen) {
			if(Globals::options->maxfsresolution) { // Load the highest resolution at 60Hz
				Globals::options->FindMaxResolution();
			}
			Globals::renderer->SetFullScreen(Globals::options->fullscreen); // resets device for us
		} else {
			Globals::renderer->OnLostDevice();
		}
	}
	void DeleteEngine() {
		delete Globals::console;
		delete Globals::options;
		delete Globals::models;
		delete Globals::textures;
		delete Globals::renderer;
		delete Globals::camera;
		delete Globals::scene;
		delete Globals::ui;
	}
	std::vector<float3> ComputeSSAOKernel(int size) {
		vector<float3> result;
		result.resize(size);
		for(unsigned int i = 0; i < result.size(); i++) {
			// uniform scale
			result[i].x = RandomRange(-1.0f,1.0f);
			result[i].y = RandomRange(-1.0f,1.0f);
			result[i].z = RandomRange(-1.0f,1.0f);

			// quadratic scale within [0.1 .. 1]
			float scale = (i / float(size)) * (i / float(size));
			scale = 0.1f * (1 - scale) + 1.0f * scale;
			result[i] *= scale;
		}
		return result;
	}
	vector<float3> ComputeGaussKernel(int size,float sigma2) {
		vector<float3> result;
		result.resize(size*size);
		for(int i = 0; i < (int)result.size(); i++) { // float3 = [x y coeff]
			result[i].x = (i % size) - 2;
			result[i].y = (i / size) - 2;
			result[i].z = 1.0f/(2*D3DX_PI*sigma2) * expf(-(powf(result[i].x - 0,2)/(2 * sigma2) + powf(result[i].y - 0,2)/(2 * sigma2))); // zeros for p0
		}
		return result;
	}
	float RandomRange(float min,float max) {
		// Scale the range to [0,1]
		float baserange = (float)rand()/(float)RAND_MAX;

		// Then scale to [0,(max-min)]
		float scalerange = baserange*(max - min);

		// And move by min to [min,max]
		float finalrange = scalerange + min;
		return finalrange;
	}
	char* ExtractFileName(const char* text,char* result) {
		char* lastslash = strrchr(text,'\\');
		if(lastslash == NULL) { // geen pad gegeven, dus text is alleen de naam
			sprintf(result,"%s",text);
		} else { // vanaf laatste slash kopieren
			strcpy(result,++lastslash);
		}

		return result;
	}
	char* ExtractFilePath(const char* text,char* result) {
		char* lastslash = strrchr(text,'\\');
		if(lastslash == NULL) { // geen mappad gegeven,niks returnen
			result[0] = 0;
		} else { // wel mappad gegeven, kopieren t/m dat ding
			lastslash++; // neem slash mee
			strncpy(result,text,lastslash-text);
			result[lastslash-text] = '\0'; // En zet er een 0 achter
		}

		return result;
	}
	char* ExtractFileExt(const char* text,char* result) {
		char* period = strrchr(text,'.');
		if(period == NULL) { // geen extensie gegeven, niks returnen
			result[0] = 0;
		} else { // wel extensie gegeven, kopieren vanaf dat ding
			strcpy(result,++period);
		}

		return result;
	}
	char* ChangeFileExt(const char* text,const char* newext,char* result) {

		// Sla alles op TOT punt...
		char* period = strrchr(text,'.');
		if(period == NULL) { // geen extensie gegeven, mooi
			result[0] = 0;
		} else { // wel extensie gegeven, kopieren tot dat ding
			strncpy(result,text,period-text);
			result[period-text] = '\0'; // En zet er een 0 achter
		}

		// gooi de nieuwe ext erachter
		strcat(result,newext);

		return result;
	}
	void IntToFourChar(int val,char* dest) {
		dest[0] = (val >> 0) & 0xFF;
		dest[1] = (val >> 8) & 0xFF;
		dest[2] = (val >> 16) & 0xFF;
		dest[3] = (val >> 24) & 0xFF;
	}
	void GetCPUName(char* cpuname,int size) {
		if(size < 49) {
			Globals::console->Write("49 characters needed to write CPU name, received %d\r\n",size);
			return;
		}

		unsigned int eax = 0;
		unsigned int ebx = 0;
		unsigned int ecx = 0;
		unsigned int edx = 0;

		// Type CPU #1
		cpuid(0x80000002,eax,ebx,ecx,edx);
		IntToFourChar(eax,&cpuname[0]);
		IntToFourChar(ebx,&cpuname[4]);
		IntToFourChar(ecx,&cpuname[8]);
		IntToFourChar(edx,&cpuname[12]);

		// Type CPU #2
		cpuid(0x80000003,eax,ebx,ecx,edx);
		IntToFourChar(eax,&cpuname[16]);
		IntToFourChar(ebx,&cpuname[20]);
		IntToFourChar(ecx,&cpuname[24]);
		IntToFourChar(edx,&cpuname[28]);

		// Type CPU #3
		cpuid(0x80000004,eax,ebx,ecx,edx);
		IntToFourChar(eax,&cpuname[32]);
		IntToFourChar(ebx,&cpuname[36]);
		IntToFourChar(ecx,&cpuname[40]);
		IntToFourChar(edx,&cpuname[44]);

		// Add terminator
		cpuname[48] = 0;
	}
	void GetGPUName(char* cpuname,int size) {
		if(Globals::renderer) {
			strncpy(cpuname,Globals::renderer->GetAdapterInfo().Description,size);
		}
	}
	int GetFontSizePt(int points,HDC hdc) {
		return -MulDiv(points, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	}
	void GetCSIDLPath(int csidl,char* result) {
		SHGetFolderPath(NULL,csidl,NULL,SHGFP_TYPE_CURRENT,result);
	}
	void SafeRelease(IUnknown* resource) {
		if(resource) {
			resource->Release();
		}
	}
	int CountChar(const char* text,char token) {
		int len = strlen(text);
		int result = 0;
		for(int i = 0; i < len; i++) {
			if(text[i] == token) {
				result++;
			}
		}
		return result;
	}
	char* TrimLeft(char* text) {
		while(isspace(*text)) {
			*text = 0; // zero fill
			text++;
		}
		return text;
	}
	char* TrimRight(char* text) {
		char* end = text + strlen(text) - 1;
		while(isspace(*end)) {
			*end = 0; // zero fill
			end--;
		}
		return text;
	}
	char* Trim(char* text) {
		return TrimLeft(TrimRight(text));
	}
	bool FileExists(const char* path) {
		WIN32_FIND_DATA info;
		HANDLE file = FindFirstFile(path,&info);
		if(file == INVALID_HANDLE_VALUE) {
			return false;
		} else {
			FindClose(file);
			return true;
		}
	}
	float DegToRad(float degin) {
		return degin/180.0f*D3DX_PI;
	}
	float RadToDeg(float radin) {
		return radin*180.0f/D3DX_PI;
	}
	void EnumerateFiles(const char* folder,const char* filter,std::vector<char*>* list) {

		WIN32_FIND_DATA FindFileData;
		HANDLE FindResult;

		char newfolder[MAX_PATH];
		sprintf(newfolder,"%s*.*",folder);

		// Spawn onszelf eerst in alle mappen
		FindResult = FindFirstFile(newfolder,&FindFileData);
		if(FindResult != INVALID_HANDLE_VALUE) {

			// Nu gaan we alles opnoemen (beginnend met FindFirstFile)
			do {
				if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					if(FindFileData.cFileName[0] != '.') {

						// Mappen gaan we ook doorzoeken
						sprintf(newfolder,"%s%s\\",folder,FindFileData.cFileName);
						EnumerateFiles(newfolder,filter,list);
					}
				}
			} while(FindNextFile(FindResult,&FindFileData));
		}
		FindClose(FindResult);

		char filename[MAX_PATH];
		sprintf(filename,"%s%s",folder,filter);

		// Zoek dan de huidige map door
		FindResult = FindFirstFile(filename,&FindFileData);
		if(FindResult != INVALID_HANDLE_VALUE) {

			// Nu gaan we weer alles opnoemen (beginnend met FindFirstFile)
			do {
				if(!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {

					// Store filename relative to folder
					//	char* finalpath = new char[MAX_PATH];
					//	sprintf(finalpath,"%s%s",folder,FindFileData.cFileName);
					list->push_back(strdup(FindFileData.cFileName));
				}
			} while(FindNextFile(FindResult,&FindFileData));
		}
		FindClose(FindResult);
	}
	unsigned int Faculty(unsigned int n) {
		unsigned int result = 1;
		for(unsigned int i = 2; i <= n; i++) {
			result *= i;
		}
		return result;
	}
	unsigned int Binomial(unsigned int n,unsigned int k) {
		return Faculty(n) / (Faculty(k) * Faculty(n - k));
	}
	float3 GramSchmidt2(float3 v1,float3 x2) {

		// v2 = x2 - dot(v1 * x2)/dot(v1,v1) * v1
		float factor1 = v1.Dot(x2)/v1.Dot(v1);

		return x2 - factor1 * v1;
	}
	float3 GramSchmidt3(float3 v1,float3 v2,float3 x3) {

		// v3 = x3 - dot(v1,x3)/dot(v1,v1) * v1 - dot(v2,x3)/dot(v2,v2) * v2
		float factor1 = v1.Dot(x3)/v1.Dot(v1);
		float factor2 = v2.Dot(x3)/v2.Dot(v2);

		return x3 - factor1 * v1 - factor2 * v2;
	}
	bool fequals(float x1,float x2) {
		return x1 + 0.000001f >= x2 && x1 - 0.000001f <= x2;
	}
	float4x4 GetLookAtMatrix(const D3DXVECTOR3& from,const D3DXVECTOR3& to) {

		// basis #1
		float3 x = to - from;
		x.Normalize();

		// basis #2
		float3 y = GramSchmidt2(x,float3(0,1,0));
		y.Normalize();

		// basis #3
		float3 z = GramSchmidt3(x,y,float3(0,0,1));
		z.Normalize();

		// 4x4, "1"
		float4x4 result;
		result.Identity();

		// insert vectors in rows (DX uses vec*matrix)
		result._11 = x.x;
		result._12 = x.y;
		result._13 = x.z;

		result._21 = y.x;
		result._22 = y.y;
		result._23 = y.z;

		result._31 = z.x;
		result._32 = z.y;
		result._33 = z.z;

		// Flip or swap one if determinant = -1...
		if(fequals(result.Determinant(),-1)) { // lame epsilon trick
			result._31 = -z.x;
			result._32 = -z.y;
			result._33 = -z.z;
		}
		return result;
	}
	bool IsFullPath(const char* path) {
		if(strlen(path) >= 2 and path[1] == ':') {
			return true;
		} else {
			return false;
		}
	}
	void GetFullPath(const char* file,const char* folder,char* fullpath) {
		if(IsFullPath(file)) {
			strcpy(fullpath,file);
		} else {
			sprintf(fullpath,"%s\\%s\\%s",Globals::exepath,folder,file); // prepend exe dir
		}
	}
	char* StripQuotes(const char* text) {

		unsigned int start = 0;
		unsigned int length = strlen(text);

		if(text[length-1] == '"') {
			length--;
		}
		if(text[0] == '"') {
			start++;
			length--;
		}

		char* noquotes = new char[length + 1];
		strncpy(noquotes,text + start,length);
		noquotes[length] = 0;

		return noquotes;
	}
	double GetTimerPeriod() {
		static __int64 frequency = 0;
		if(frequency == 0) { // only query once (querying is slow)
			if(QueryPerformanceFrequency((LARGE_INTEGER*)&frequency)) {
				return 1.0/(double)frequency; // query successful
			}
		} else { // return old result
			return 1.0/(double)frequency;
		}
	}
}
