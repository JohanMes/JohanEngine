#include <stdio.h>
#include "Console.h"
#include "Material.h"
#include "Textures.h"
#include "Renderer.h"
#include "resource.h"

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
	
	cullmode = D3DCULL_CCW;
	fillmode = D3DFILL_SOLID;
	alphatest = false;
	alphablend = false;
	multitex = false;
	shaderindex = 0; // PureTexture
}
void Material::LoadFromFile(const char* materialpath) {
	
	// TODO: don't reset?
	Clear();

	// Vars
	char currentregel[512];
	char word1[128];
	char word2[128];
	
	char fullmaterialpath[MAX_PATH];
	GetFullPath(materialpath,"Data\\Materials",fullmaterialpath);
	
	FILE* material = fopen(fullmaterialpath,"r");
	if(material == NULL) {
		console->Write("Error opening material %s\r\n",fullmaterialpath);
		return;
	}

	while(fgets(currentregel,sizeof(currentregel),material)) {
		if(sscanf(currentregel,"%[^ #\n]",word1) == 1) {
			if(!strcmp(word1,"diffuse")) {
				if(sscanf(currentregel,"diffuse %s %f",word2,&diffuse) == 2) {
					diffusetex = textures->Add(word2);
				} else {
					console->Write("Error reading line:\r\n%s\r\n",currentregel);
				}
			} else if(!strcmp(word1,"specular")) {
				if(sscanf(currentregel,"specular %s %f %f",word2,&specular,&shininess) == 3) {
					speculartex = textures->Add(word2);
				} else {
					console->Write("Error reading line:\r\n%s\r\n",currentregel);
				}
			} else if(!strcmp(word1,"normal")) {
				if(sscanf(currentregel,"normal %s",word2) == 1) {
					normaltex = textures->Add(word2);
				} else {
					console->Write("Error reading line:\r\n%s\r\n",currentregel);
				}
			} else if(!strcmp(word1,"parallax")) {
				if(sscanf(currentregel,"parallax %s",word2) == 1) {
					parallaxtex = textures->Add(word2);	
				} else {
					console->Write("Error reading line:\r\n%s\r\n",currentregel);
				}
			} else if(!strcmp(word1,"ambient")) {
				if(sscanf(currentregel,"ambient %s",word2) == 1) {
					ambienttex = textures->Add(word2);
				} else {
					console->Write("Error reading line:\r\n%s\r\n",currentregel);
				}
			} else if(!strcmp(word1,"diffuse1")) {
				if(sscanf(currentregel,"diffuse1 %s",word2) == 1) {
					diffusetex = textures->Add(word2);
					multitex = true;
				} else {
					console->Write("Error reading line:\r\n%s\r\n",currentregel);
				}
			} else if(!strcmp(word1,"diffuse2")) {
				if(sscanf(currentregel,"diffuse2 %s",word2) == 1) {
					speculartex = textures->Add(word2);
					multitex = true;
				} else {
					console->Write("Error reading line:\r\n%s\r\n",currentregel);
				}
			} else if(!strcmp(word1,"diffuse3")) {
				if(sscanf(currentregel,"diffuse3 %s",word2) == 1) {
					normaltex = textures->Add(word2);
					multitex = true;
				} else {
					console->Write("Error reading line:\r\n%s\r\n",currentregel);
				}
			} else if(!strcmp(word1,"mixer")) {
				if(sscanf(currentregel,"mixer %s",word2) == 1) {
					parallaxtex = textures->Add(word2);
					multitex = true;
				} else {
					console->Write("Error reading line:\r\n%s\r\n",currentregel);
				}
			} else if(!strcmp(word1,"shader")) {
				if(sscanf(currentregel,"shader %s",word2) == 1) {
					for(int i = 0;i < (int)renderer->shaders.size();i++) {
						if(!strcmp(word2,renderer->shaders[i]->name)) {
							shaderindex = i;
							break;
						} else if(i == (int)renderer->shaders.size()-1) {
							console->Write("Cannot find shader %s\r\n",word2);	
						}
					}
				} else {
					console->Write("Error reading line:\r\n%s\r\n",currentregel);
				}
			} else if(!strcmp(word1,"tiling")) {
				sscanf(currentregel,"tiling %f",&tiling);
			} else if(!strcmp(word1,"cullmode")) {
				if(sscanf(currentregel,"cullmode %s",word2) == 1) {
					if(!strcmp(word2,"none")) {
						cullmode = D3DCULL_NONE;
					} else if(!strcmp(word2,"cw")) {
						cullmode = D3DCULL_CW;
					} else if(!strcmp(word2,"ccw")) {
						cullmode = D3DCULL_CCW;	
					} else {
						console->Write("Error reading line:\r\n%s\r\n",currentregel);
					}
				} else {
					console->Write("Error reading line:\r\n%s\r\n",currentregel);
				}
			} else if(!strcmp(word1,"fillmode")) {
				if(sscanf(currentregel,"fillmode %s",word2) == 1) {
					if(!strcmp(word2,"point")) {
						fillmode = D3DFILL_POINT;
					} else if(!strcmp(word2,"wireframe")) {
						fillmode = D3DFILL_WIREFRAME;
					} else if(!strcmp(word2,"solid")) {
						fillmode = D3DFILL_SOLID;	
					} else {
						console->Write("Error reading line:\r\n%s\r\n",currentregel);
					}
				} else {
					console->Write("Error reading line:\r\n%s\r\n",currentregel);
				}
			} else if(!strcmp(word1,"alphatest")) {
				if(sscanf(currentregel,"alphatest %s",word2) == 1) {
					alphatest = !strcmp(word2,"1");
				} else {
					console->Write("Error reading line:\r\n%s\r\n",currentregel);
				}
			} else if(!strcmp(word1,"alphablend")) {
				if(sscanf(currentregel,"alphablend %s",word2) == 1) {
					alphablend = !strcmp(word2,"1");
				} else {
					console->Write("Error reading line:\r\n%s\r\n",currentregel);
				}
			} else {
				console->Write("Unknown command \"%s\" in file \"%s\"\r\n",word1,fullmaterialpath);
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
