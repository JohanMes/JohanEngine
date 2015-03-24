#include "Material.h"
#include "Console.h" // printing
#include "Textures.h"
#include "Renderer.h"
#include "Texture.h"
#include "Resource.h"
#include "FXShader.h"
using Globals::console;

Material::Material() {
}
Material::~Material() {
}
void Material::Clear() {
	diffusetex = NULL; // Verwijder GEEN textures, die ownen we niet!
	speculartex = NULL;
	normaltex = NULL;
	parallaxtex = NULL;
	ambienttex = NULL;
	diffuse = 1.0f;
	specular = 0.0f;
	shininess = 0.0f;
	tiling = 1.0f;
	mixer = float3(0,0,0);
	color = float4(1,1,1,1);
	cullmode = D3DCULL_CCW;
	fillmode = D3DFILL_SOLID;
	alphatest = false;
	alphablend = false;
	multitex = false;
	shaderindex = 0; // PureTexture
}
void Material::LoadFromFile(const char* materialpath) {
	Clear();

	// Dump for line parts
	char line[1024];
	char word1[512];
	char word2[512];

	// Get full path
	char fullmaterialpath[MAX_PATH];
	Utils::GetFullPath(materialpath,"Data\\Materials",fullmaterialpath);

	// Try to open the file
	FILE* material = fopen(fullmaterialpath,"r");
	if(material == NULL) {
		console->Write("ERROR: cannot open material file '%s'\r\n",fullmaterialpath);
		return;
	}

	// Read the lines one by one
	while(fgets(line,sizeof(line),material)) {
		if(sscanf(line,"%[^ #\n]",word1) == 1) {
			if(!strcmp(word1,"diffuse")) {
				if(sscanf(line,"diffuse %s %f",word2,&diffuse) == 2) {
					diffusetex = Globals::textures->Add(word2);
				} else {
					console->Write("ERROR: not enough arguments in line '%s'\r\n",line);
				}
			} else if(!strcmp(word1,"specular")) {
				if(sscanf(line,"specular %s %f %f",word2,&specular,&shininess) == 3) {
					speculartex = Globals::textures->Add(word2);
				} else {
					console->Write("ERROR: not enough arguments in line '%s'\r\n",line);
				}
			} else if(!strcmp(word1,"normal")) {
				if(sscanf(line,"normal %s",word2) == 1) {
					normaltex = Globals::textures->Add(word2);
				} else {
					console->Write("ERROR: not enough arguments in line '%s'\r\n",line);
				}
			} else if(!strcmp(word1,"parallax")) {
				if(sscanf(line,"parallax %s",word2) == 1) {
					parallaxtex = Globals::textures->Add(word2);
				} else {
					console->Write("ERROR: not enough arguments in line '%s'\r\n",line);
				}
			} else if(!strcmp(word1,"ambient")) {
				if(sscanf(line,"ambient %s",word2) == 1) {
					ambienttex = Globals::textures->Add(word2);
				} else {
					console->Write("ERROR: not enough arguments in line '%s'\r\n",line);
				}
			} else if(!strcmp(word1,"diffuse1")) {
				if(sscanf(line,"diffuse1 %s",word2) == 1) {
					diffusetex = Globals::textures->Add(word2);
					multitex = true;
				} else {
					console->Write("ERROR: not enough arguments in line '%s'\r\n",line);
				}
			} else if(!strcmp(word1,"diffuse2")) {
				if(sscanf(line,"diffuse2 %s",word2) == 1) {
					speculartex = Globals::textures->Add(word2);
					multitex = true;
				} else {
					console->Write("ERROR: not enough arguments in line '%s'\r\n",line);
				}
			} else if(!strcmp(word1,"diffuse3")) {
				if(sscanf(line,"diffuse3 %s",word2) == 1) {
					normaltex = Globals::textures->Add(word2);
					multitex = true;
				} else {
					console->Write("ERROR: not enough arguments in line '%s'\r\n",line);
				}
			} else if(!strcmp(word1,"mixer")) {
				if(sscanf(line,"mixer %s",word2) == 1) {
					parallaxtex = Globals::textures->Add(word2);
					multitex = true;
				} else {
					console->Write("ERROR: not enough arguments in line '%s'\r\n",line);
				}
			} else if(!strcmp(word1,"color")) {
				if(sscanf(line,"color %f %f %f %f",&color.x,&color.y,&color.z,&color.w) == 4) {
					// ...
				} else {
					console->Write("ERROR: not enough arguments in line '%s'\r\n",line);
				}
			} else if(!strcmp(word1,"shader")) {
				if(sscanf(line,"shader %s",word2) == 1) {
					for(int i = 0; i < (int)Globals::renderer->shaders.size(); i++) {
						if(!strcmp(word2,Globals::renderer->shaders[i]->GetName())) {
							shaderindex = i;
							break;
						} else if(i == (int)Globals::renderer->shaders.size()-1) {
							console->Write("Cannot find shader %s\r\n",word2);
						}
					}
				} else {
					console->Write("ERROR: not enough arguments in line '%s'\r\n",line);
				}
			} else if(!strcmp(word1,"tiling")) {
				if(sscanf(line,"tiling %f",&tiling) != 1) {
					
				} else {
					console->Write("ERROR: not enough arguments in line '%s'\r\n",line);
				}
			} else if(!strcmp(word1,"cullmode")) {
				if(sscanf(line,"cullmode %s",word2) == 1) {
					if(!strcmp(word2,"none")) {
						cullmode = D3DCULL_NONE;
					} else if(!strcmp(word2,"cw")) {
						cullmode = D3DCULL_CW;
					} else if(!strcmp(word2,"ccw")) {
						cullmode = D3DCULL_CCW;
					} else {
						console->Write("ERROR: invalid cullmode '%s' in line '%s'\r\n",word2,line);
					}
				} else {
					console->Write("ERROR: not enough arguments in line '%s'\r\n",line);
				}
			} else if(!strcmp(word1,"fillmode")) {
				if(sscanf(line,"fillmode %s",word2) == 1) {
					if(!strcmp(word2,"point")) {
						fillmode = D3DFILL_POINT;
					} else if(!strcmp(word2,"wireframe")) {
						fillmode = D3DFILL_WIREFRAME;
					} else if(!strcmp(word2,"solid")) {
						fillmode = D3DFILL_SOLID;
					} else {
						console->Write("ERROR: invalid fillmode '%s' in line '%s'\r\n",word2,line);
					}
				} else {
					console->Write("ERROR: not enough arguments in line '%s'\r\n",line);
				}
			} else if(!strcmp(word1,"alphatest")) {
				if(sscanf(line,"alphatest %s",word2) == 1) {
					alphatest = !strcmp(word2,"1");
				} else {
					console->Write("ERROR: not enough arguments in line '%s'\r\n",line);
				}
			} else if(!strcmp(word1,"alphablend")) {
				if(sscanf(line,"alphablend %s",word2) == 1) {
					alphablend = !strcmp(word2,"1");
				} else {
					console->Write("ERROR: not enough arguments in line '%s'\r\n",line);
				}
			} else {
				console->Write("ERROR: unknown command '%s' on line '%s'\r\n",word1,line);
			}
		}
	}
	fclose(material);
}
void Material::Print() {

	console->Write("\r\n----- Info for class Material -----\r\n\r\n");

	if(diffusetex) {
		diffusetex->Print();
	}
	if(speculartex) {
		speculartex->Print();
	}
	if(normaltex) {
		normaltex->Print();
	}
	if(parallaxtex) {
		parallaxtex->Print();
	}
	if(ambienttex) {
		ambienttex->Print();
	}

	console->WriteVar("diffuse",diffuse);
	console->WriteVar("specular",specular);
	console->WriteVar("shininess",shininess);
	console->WriteVar("tiling",tiling);
	console->WriteVar("mixer",mixer);
	console->WriteVar("cullmode",cullmode);
	console->WriteVar("fillmode",fillmode);
	console->WriteVar("alphatest",alphatest);
	console->WriteVar("alphablend",alphablend);
	console->WriteVar("shaderindex",shaderindex);

	console->Write("\r\n----- End of info -----\r\n\r\n");
}
