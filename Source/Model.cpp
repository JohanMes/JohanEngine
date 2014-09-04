#include <stdio.h>
#include "Model.h"
#include "Renderer.h"
#include "Console.h"

Model::Model() {
	// Pointers naar VRAM
	vertexbuffer = NULL;
	indexbuffer = NULL;
	
	// Kopietje voor de CPU
	localvertexbuffer = NULL;
	localindexbuffer = NULL;

	fullpath = NULL;
	filename = NULL;
}
Model::Model(const char* filename,bool sendtogpu) {
	
	// Kopietje voor de GPU
	vertexbuffer = NULL;
	indexbuffer = NULL;
	
	// Kopietje voor de CPU
	localvertexbuffer = NULL;
	localindexbuffer = NULL;

	// Set name to loaded item
	LoadFromOBJ(filename);
	if(sendtogpu) {
		SendToGPU();
	}
}
Model::~Model() {
	FreeBuffers();

	delete[] filename;
	delete[] fullpath;
}

void Model::FreeBuffers() {
	free(localvertexbuffer);
	free(localindexbuffer);
	SafeRelease(vertexbuffer);
	SafeRelease(indexbuffer);
	
	localvertexbuffer = NULL;
	localindexbuffer = NULL;
	vertexbuffer = NULL;
	indexbuffer = NULL;
}

void Model::LoadFromOBJ(const char* filename) {
	
	// Remove old stuff
	FreeBuffers();
	
	// Reset OBJ loading counters
	numvertices = 0;
	numfaces = 0;
	unsigned int numnormals = 0;
	unsigned int numcoords = 0;
	
	// Reset bounding sphere
	center = float3(0,0,0);
	r = 0.0f;
	
	// Obtain full path
	fullpath = new char[MAX_PATH];
	GetFullPath(filename,"Data\\Models",fullpath);
	
	// Set name to loaded item
	this->filename = new char[MAX_PATH];
	ExtractFileName(fullpath,this->filename);
	
	// Try to open filename, return dummy on failure
	FILE* modelfile = fopen(fullpath,"rb");
	if(modelfile == NULL) {
		console->Write("Error opening model %s\r\n",fullpath);
		return;
	}
	
	// Store each line here
	char currentline[300];
	char word1[100];
	
	while(fgets(currentline,sizeof(currentline),modelfile)) {		
		if(sscanf(currentline,"%[^ #\r\n]",word1) == 1) {
			if(!strcmp(word1,"v")) {
				numvertices++;
			} else if(!strcmp(word1,"vn")) {
				numnormals++;
			} else if(!strcmp(word1,"vt")) {
				numcoords++;
			} else if(!strcmp(word1,"f")) {
				int spacecount = CountChar(TrimRight(currentline),' ');
				if(spacecount == 3) { // Add a triangle
					numfaces++;
				} else if(spacecount == 4) { // Add a quad (two triangles)
					numfaces += 2;
				}
			}
		}
	}

	fseek(modelfile,0,SEEK_SET);
	
	// Ruimte maken voor elke unieke pos/normal/texcoord
	float3* positions = new float3[numvertices];
	float3* normals = new float3[numnormals];
	float2* coords = new float2[numcoords];
	
	// Deze gaan we bewaren voor van alles en nog wat
	localvertexbuffer = new VERTEX[3*numfaces];
	localindexbuffer = new unsigned int[3*numfaces];
	for(unsigned int i = 0;i < 3*numfaces;i++) {
		localindexbuffer[i] = i;
	}
	
	int curvertex = 0;
	int curnormal = 0;
	int curcoord = 0;
	int curface = 0;
	
	int p1,p2,p3,p4;
	int n1,n2,n3,n4;
	int c1,c2,c3,c4;

	// En nu alle regels die ertoe doen lezen en in de arrays zetten
	while(fgets(currentline,sizeof(currentline),modelfile)) {
		if(sscanf(currentline,"%[^ #\n]",word1) == 1) {
			if(!strcmp(word1,"v")) { // Vertex
				sscanf(currentline,"v %f %f %f",
					&positions[curvertex].x,
					&positions[curvertex].y,
					&positions[curvertex].z);
				curvertex++;
			} else if(!strcmp(word1,"vn")) { // Normal
				sscanf(currentline,"vn %f %f %f",
					&normals[curnormal].x,
					&normals[curnormal].y,
					&normals[curnormal].z);
				curnormal++;
			} else if(!strcmp(word1,"vt")) { // Texture coordinate
				sscanf(currentline,"vt %f %f",
					&coords[curcoord].x,
					&coords[curcoord].y);
				coords[curcoord].y = 1.0f - coords[curcoord].y; // why?
				curcoord++;
			} else if(!strcmp(word1,"f")) { // face, can be triangle or quad
			
				int spacecount = CountChar(TrimRight(currentline),' ');
			
				// Triangle...
				if(spacecount == 3) {
				
					if(!numnormals && !numcoords) {
						
						// f pos pos pos
						if(sscanf(currentline,"f %d %d %d",&p1,&p2,&p3) == 3) {
							
							// Vul geleverde XYZ in
							localvertexbuffer[curface+0].pos = positions[p1-1];
							localvertexbuffer[curface+1].pos = positions[p2-1];
							localvertexbuffer[curface+2].pos = positions[p3-1];
							
							// Vul random normals in
							localvertexbuffer[curface+0].nor = float3(0.0f,0.0f,0.0f);
							localvertexbuffer[curface+1].nor = float3(0.0f,0.0f,0.0f);
							localvertexbuffer[curface+2].nor = float3(0.0f,0.0f,0.0f);
				
							// Vul random texcoords in
							localvertexbuffer[curface+0].tex = float2(0.0f,0.0f);
							localvertexbuffer[curface+1].tex = float2(0.0f,0.0f);
							localvertexbuffer[curface+2].tex = float2(0.0f,0.0f);
						}
					} else if(numnormals && !numcoords) {
						
						// f pos//nor pos//nor pos//nor
						if(sscanf(currentline,"f%d//%d %d//%d %d//%d",&p1,&n1,&p2,&n2,&p3,&n3) == 6) {
							
							// Vul geleverde XYZ in
							localvertexbuffer[curface+0].pos = positions[p1-1];
							localvertexbuffer[curface+1].pos = positions[p2-1];
							localvertexbuffer[curface+2].pos = positions[p3-1];
							
							// Vul geleverde normals in
							localvertexbuffer[curface+0].nor = normals[n1-1];
							localvertexbuffer[curface+1].nor = normals[n2-1];
							localvertexbuffer[curface+2].nor = normals[n3-1];
							
							// Vul random texcoords in
							localvertexbuffer[curface+0].tex = float2(0.0f,0.0f);
							localvertexbuffer[curface+1].tex = float2(0.0f,0.0f);
							localvertexbuffer[curface+2].tex = float2(0.0f,0.0f);
						}
					} else if(numnormals && numcoords) {
						
						// f pos/nor/tex pos/nor/tex pos/nor/tex
						if(sscanf(currentline,"f %d/%d/%d %d/%d/%d %d/%d/%d",&p1,&c1,&n1,&p2,&c2,&n2,&p3,&c3,&n3) == 9) {
							
							// Vul geleverde XYZ in
							localvertexbuffer[curface+0].pos = positions[p1-1];
							localvertexbuffer[curface+1].pos = positions[p2-1];
							localvertexbuffer[curface+2].pos = positions[p3-1];	
							
							// Vul geleverde normals in
							localvertexbuffer[curface+0].nor = normals[n1-1];
							localvertexbuffer[curface+1].nor = normals[n2-1];
							localvertexbuffer[curface+2].nor = normals[n3-1];
			
							// Vul geleverde texcoords in
							localvertexbuffer[curface+0].tex = coords[c1-1];
							localvertexbuffer[curface+1].tex = coords[c2-1];
							localvertexbuffer[curface+2].tex = coords[c3-1];
						}
					}
			
					curface += 3;
				
				// Accepteer ook vierkanten...
				} else if(spacecount == 4) {
				
					// Add triangles for 1,2,4 and 3,4,2
					
					if(!numnormals && !numcoords) {
						
						// f pos pos pos pos
						if(sscanf(currentline,"f %d %d %d %d",&p1,&p2,&p3,&p4) == 4) {
							
							// Vul geleverde XYZ in
							localvertexbuffer[curface+0].pos = positions[p1-1];
							localvertexbuffer[curface+1].pos = positions[p2-1];
							localvertexbuffer[curface+2].pos = positions[p4-1];	
							
							// Vul random normals in
							localvertexbuffer[curface+0].nor = float3(0.0f,0.0f,0.0f);
							localvertexbuffer[curface+1].nor = float3(0.0f,0.0f,0.0f);
							localvertexbuffer[curface+2].nor = float3(0.0f,0.0f,0.0f);
				
							// Vul random texcoords in
							localvertexbuffer[curface+0].tex = float2(0.0f,0.0f);
							localvertexbuffer[curface+1].tex = float2(0.0f,0.0f);
							localvertexbuffer[curface+2].tex = float2(0.0f,0.0f);
							
							// Vul geleverde XYZ in
							localvertexbuffer[curface+3].pos = positions[p3-1];
							localvertexbuffer[curface+4].pos = positions[p4-1];
							localvertexbuffer[curface+5].pos = positions[p2-1];	
							
							// Vul random normals in
							localvertexbuffer[curface+3].nor = float3(0.0f,0.0f,0.0f);
							localvertexbuffer[curface+4].nor = float3(0.0f,0.0f,0.0f);
							localvertexbuffer[curface+5].nor = float3(0.0f,0.0f,0.0f);
				
							// Vul random texcoords in
							localvertexbuffer[curface+3].tex = float2(0.0f,0.0f);
							localvertexbuffer[curface+4].tex = float2(0.0f,0.0f);
							localvertexbuffer[curface+5].tex = float2(0.0f,0.0f);			
						}
					} else if(numnormals && !numcoords) {
						
						// f pos//nor pos//nor pos//nor pos//nor
						if(sscanf(currentline,"f%d//%d %d//%d %d//%d %d//%d",&p1,&n1,&p2,&n2,&p3,&n3,&p4,&n4) == 8) {
							
							// Vul geleverde XYZ in
							localvertexbuffer[curface+0].pos = positions[p1-1];
							localvertexbuffer[curface+1].pos = positions[p2-1];
							localvertexbuffer[curface+2].pos = positions[p4-1];
							
							// Vul geleverde normals in
							localvertexbuffer[curface+0].nor = normals[n1-1];
							localvertexbuffer[curface+1].nor = normals[n2-1];
							localvertexbuffer[curface+2].nor = normals[n4-1];
							
							// Vul random texcoords in
							localvertexbuffer[curface+0].tex = float2(0.0f,0.0f);
							localvertexbuffer[curface+1].tex = float2(0.0f,0.0f);
							localvertexbuffer[curface+2].tex = float2(0.0f,0.0f);
							
							// Vul geleverde XYZ in
							localvertexbuffer[curface+3].pos = positions[p3-1];
							localvertexbuffer[curface+4].pos = positions[p4-1];
							localvertexbuffer[curface+5].pos = positions[p2-1];
							
							// Vul geleverde normals in
							localvertexbuffer[curface+3].nor = normals[n3-1];
							localvertexbuffer[curface+4].nor = normals[n4-1];
							localvertexbuffer[curface+5].nor = normals[n2-1];
							
							// Vul random texcoords in
							localvertexbuffer[curface+3].tex = float2(0.0f,0.0f);
							localvertexbuffer[curface+4].tex = float2(0.0f,0.0f);
							localvertexbuffer[curface+5].tex = float2(0.0f,0.0f);
						}
					} else if(numnormals && numcoords) {
						
						// f pos/tex/nor pos/tex/nor pos/tex/nor pos/tex/nor
						if(sscanf(currentline,"f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",&p1,&c1,&n1,&p2,&c2,&n2,&p3,&c3,&n3,&p4,&c4,&n4) == 12) {
							
							// Vul geleverde XYZ in
							localvertexbuffer[curface+0].pos = positions[p1-1];
							localvertexbuffer[curface+1].pos = positions[p2-1];
							localvertexbuffer[curface+2].pos = positions[p4-1];	
							
							// Vul geleverde normals in
							localvertexbuffer[curface+0].nor = normals[n1-1];
							localvertexbuffer[curface+1].nor = normals[n2-1];
							localvertexbuffer[curface+2].nor = normals[n4-1];
							
							// Vul geleverde texcoords in
							localvertexbuffer[curface+0].tex = coords[c1-1];
							localvertexbuffer[curface+1].tex = coords[c2-1];
							localvertexbuffer[curface+2].tex = coords[c4-1];
							
							// Vul geleverde XYZ in
							localvertexbuffer[curface+3].pos = positions[p3-1];
							localvertexbuffer[curface+4].pos = positions[p4-1];
							localvertexbuffer[curface+5].pos = positions[p2-1];	
	
							// Vul geleverde normals in
							localvertexbuffer[curface+3].nor = normals[n3-1];
							localvertexbuffer[curface+4].nor = normals[n4-1];
							localvertexbuffer[curface+5].nor = normals[n2-1];
			
							// Vul geleverde texcoords in
							localvertexbuffer[curface+3].tex = coords[c3-1];
							localvertexbuffer[curface+4].tex = coords[c4-1];
							localvertexbuffer[curface+5].tex = coords[c2-1];
						}
					}
	
					curface += 6;
				}
			}
		}
	}
	fclose(modelfile);
	
	// En tijdelijke rommel opruimen
	delete[] positions;
	delete[] normals;
	delete[] coords;
	
	// Neem aan dat we alles m.b.v. triangles opbouwen (geen optimize)
	numvertices = 3*numfaces;
	
	// Generate tangents (even if the shaders don't use them)...
	CreateTangents();
	
	// Generate bounding sphere for frustum culling
	GetBoundingSphere();
	
	// Don't send to GPU immediately
}

void Model::Load2DQuad(float left,float top,float right,float bottom) {

	// Remove old stuff
	FreeBuffers();
	
	numvertices = 4;
	numfaces = 2;
	
	// 4 hoeken
	localvertexbuffer = new VERTEX[numvertices];
	
	localvertexbuffer[0].pos = float3(left,-top,0.0f); // linksboven
	localvertexbuffer[1].pos = float3(right,-top,0.0f); // rechtsboven
	localvertexbuffer[2].pos = float3(left,-bottom,0.0f); // linksonder
	localvertexbuffer[3].pos = float3(right,-bottom,0.0f); // rechtsonder
	
	localvertexbuffer[0].nor = float3(1.0f,0.0f,0.0f);
	localvertexbuffer[1].nor = float3(1.0f,0.0f,0.0f);
	localvertexbuffer[2].nor = float3(1.0f,0.0f,0.0f);
	localvertexbuffer[3].nor = float3(1.0f,0.0f,0.0f);

	localvertexbuffer[0].tex = float2(0.0f,0.0f);
	localvertexbuffer[1].tex = float2(1.0f,0.0f);
	localvertexbuffer[2].tex = float2(0.0f,1.0f);
	localvertexbuffer[3].tex = float2(1.0f,1.0f);
	
	// 2 faces
	localindexbuffer = new unsigned int[3*numfaces];
	
	localindexbuffer[0] = 0;
	localindexbuffer[1] = 1;
	localindexbuffer[2] = 2;
	localindexbuffer[3] = 1;
	localindexbuffer[4] = 3;
	localindexbuffer[5] = 2;
	
	// Generate tangents (even if the shaders don't use them)...
	CreateTangents();
	
	// Generate bounding sphere for frustum culling
	GetBoundingSphere();
	
	// Don't send to GPU immediately
}

void Model::LoadBuffer(VERTEX* vb,unsigned int* ib,unsigned int numvertices,unsigned int numindices) {
	
	// Remove old stuff
	FreeBuffers();
	
	localvertexbuffer = vb; // move, don't copy!
	localindexbuffer = ib; // idem

	this->numvertices = numvertices;
	numfaces = numindices/3;
	
	// Tangent should already be present
	
	// Generate bounding sphere for frustum culling
	GetBoundingSphere();
	
	// Don't send to GPU immediately
}

void Model::LoadPlane(unsigned int tiling,unsigned int textiling,float edgelen,Heightmap* height) {
	
	// Remove old stuff
	FreeBuffers();
	
	// We moeten minstens 1 vierkant maken
	if(tiling < 2) {
		console->Write("Invalid tiling value of %u: must be at least 2\r\n",tiling);
		tiling = 2;
	}
	
	// Losse punten, posities
	numvertices = tiling*tiling;
	numfaces = 2*(tiling*tiling-2*tiling+1);
	
	localvertexbuffer = new VERTEX[numvertices];

	// Stel de coordinaten in
	for(unsigned int i = 0;i < numvertices;i++) {
		localvertexbuffer[i].pos.x = ((i%tiling)/(float)(tiling-1))*edgelen - 0.5f*edgelen;
		localvertexbuffer[i].pos.z = ((i - (i%tiling))/(float)(tiling*tiling-tiling))*edgelen - 0.5f*edgelen;
		if(height) {
			localvertexbuffer[i].pos.y = height->GetBilinearHeight(
				localvertexbuffer[i].pos.x,
				localvertexbuffer[i].pos.z);
		} else {
			localvertexbuffer[i].pos.y = 0;
		}
		
		// Verspreid de textures zodat bij textiling maal over de gehele plane getrokken wordt
		localvertexbuffer[i].tex.x = localvertexbuffer[i].pos.x/edgelen*textiling;
		localvertexbuffer[i].tex.y = localvertexbuffer[i].pos.z/edgelen*textiling;
	}

	// Bereken pas als we 3D-coordinaten hebben de normalen
	for(unsigned int i = 0;i < numvertices;i++) {
	
		// Defaults
		localvertexbuffer[i].nor.x = 0.0f;
		localvertexbuffer[i].nor.y = 1.0f;
		localvertexbuffer[i].nor.z = 0.0f;

		// Onderzijde, rechterzijde, linkerzijde, bovenzijde
		if((i+1)%tiling != 0 && i < (tiling*tiling)-tiling && i>=tiling && i%tiling != 0) {

			// Twee random vectoren, normalizen, crossen, normalizen
			float3 iplus1 = localvertexbuffer[i+1].pos - localvertexbuffer[i].pos;
			float3 iplusn = localvertexbuffer[i+tiling].pos - localvertexbuffer[i].pos;
			iplus1.Normalize();
			iplusn.Normalize();

			// Het crossproduct van twee normalen op het vlak levert de normaal op (probeersel 1)
			float3 tempnor1;
			tempnor1 = iplus1.Cross(iplusn);
			tempnor1.Normalize();
			
			// Nogmaals twee random vectoren, normalizen, crossen, normalizen
			float3 iminus1 = localvertexbuffer[i-1].pos - localvertexbuffer[i].pos;
			float3 iminusn = localvertexbuffer[i-tiling].pos - localvertexbuffer[i].pos;
			iminus1.Normalize();
			iminusn.Normalize();

			// Het crossproduct van twee normalen op het vlak levert de normaal op (probeersel 2)
			float3 tempnor2;
			tempnor2 = iminus1.Cross(iminusn);
			tempnor2.Normalize();
			
			// Average voor echte normal...
			localvertexbuffer[i].nor.x = (tempnor1.x + tempnor2.x)/2.0f;
			localvertexbuffer[i].nor.y = (tempnor1.y + tempnor2.y)/2.0f;
			localvertexbuffer[i].nor.z = (tempnor1.z + tempnor2.z)/2.0f;
			
			if(localvertexbuffer[i].nor.y < 0.0f) {
				localvertexbuffer[i].nor = -localvertexbuffer[i].nor; // Wrong cross product...
			}
		}
	}
	
	// Faceindices
	localindexbuffer = new unsigned int[3*numfaces];
	int actualindex = 0;
	for(unsigned int i = 0;i < (tiling*tiling-tiling)-1;i++) {
		if((i < (numvertices-tiling)) && (i+1)%tiling != 0) {
		
			// Eerste face
			localindexbuffer[6*actualindex+0] = i;
			localindexbuffer[6*actualindex+1] = i+tiling;
			localindexbuffer[6*actualindex+2] = i+1;
			
			// Tweede face
			localindexbuffer[6*actualindex+3] = i+1;
			localindexbuffer[6*actualindex+4] = i+tiling;
			localindexbuffer[6*actualindex+5] = i+1+tiling;
			
			actualindex++;
		}
	}

	// Generate tangents (even if the shaders don't use them)...
	CreateTangents();
	
	// Generate bounding sphere for frustum culling
	GetBoundingSphere();
	
	// Don't send to GPU immediately
}

void Model::LoadParticle() {
	// niets
}

void Model::Optimize() {

	// loop door vertices van model heen
	for(unsigned int i = 0;i < numvertices;i++) {

		// loop langs alles dat we nog niet gehad hebben
		for(unsigned int j = i + 1;j < numvertices;j++) {

			// Als *alle* members gelijk zijn
			if(localvertexbuffer[i].pos == localvertexbuffer[j].pos && 
			   localvertexbuffer[i].nor == localvertexbuffer[j].nor &&
			   localvertexbuffer[i].tex == localvertexbuffer[j].tex)
			{

				// Opslaan dat de grootte van de buffer 1 kleiner is geworden
				numvertices--;

				// Schuif alle bovenliggende items 1 plaats naar beneden, zodat dubbele wordt overschreven
				for(unsigned int c = j;c < 3*numfaces;c++) {
					localvertexbuffer[c] = localvertexbuffer[c+1];

					// Indexverwijzingen bijwerken
					if(localindexbuffer[c] == j) {
						localindexbuffer[c] = i;
					} else if(localindexbuffer[c] > j) {
						localindexbuffer[c]--;
					}
				}
			}
		}
	}
	
	// Gooi rommel aan het einde weg...
	localvertexbuffer = (VERTEX*)realloc(localvertexbuffer,numvertices*sizeof(VERTEX));
	
	// Don't immediately update GPU copy
}

void Model::GetBoundingSphere() {
	
	r = 0.0f;
	
	float len;
	float3 pair1,pair2,lenvec;
	int testindex = 0,farthestindex = 0;
	
	// Determine which vertex is farthest apart from index 'testindex'
	while(1) {
		for(unsigned int i = 0;i < numvertices;i++) {
			lenvec = localvertexbuffer[i].pos - localvertexbuffer[testindex].pos;
			len = lenvec.Length();
	
			if(len > r) {
				r = len;
				pair1 = localvertexbuffer[i].pos;
				pair2 = localvertexbuffer[testindex].pos;
				farthestindex = i;
			}
		}
		
		// Continue the search with the new farthest vertex
		if(farthestindex == testindex) { // no progress
			break;
		} else {
			testindex = farthestindex; // test against farthest vertex
		}
	}

	// Radius equals half the maximum distance, and apply scaling
	r /= 2.0f;
	
	// Center is right in between the pair of vector2
	center = 0.5f*(pair1 + pair2);
}

void Model::SendToGPU() {
	if(numvertices == 0) {
		console->Write("Cannot send empty vertex buffer of file %s to GPU\r\n",filename);
		return;
	}

	void* voidptr = NULL;
	
	unsigned int vertexbuffersize = numvertices*sizeof(VERTEX);
	unsigned int indexbuffersize = 3*numfaces*sizeof(unsigned int);

	HRESULT result = d3ddev->CreateVertexBuffer(vertexbuffersize,0,0,D3DPOOL_MANAGED,&vertexbuffer,NULL);
	switch(result) {
		case D3DERR_INVALIDCALL: {
			console->Write("Error creating vertex buffer of size %d (%d * %d): D3DERR_INVALIDCALL\r\n",vertexbuffersize,numvertices,sizeof(VERTEX));
			break;
		}
		case D3DERR_OUTOFVIDEOMEMORY: {
			console->Write("Error creating vertex buffer of size %d (%d * %d): D3DERR_OUTOFVIDEOMEMORY\r\n",vertexbuffersize,numvertices,sizeof(VERTEX));
			break;
		}
		case E_OUTOFMEMORY: {
			console->Write("Error creating vertex buffer of size %d (%d * %d): E_OUTOFMEMORY\r\n",vertexbuffersize,numvertices,sizeof(VERTEX));
			break;
		}
	}
	
	// Try anyway...
	vertexbuffer->Lock(0, 0, (void**)&voidptr, 0);
	memcpy(voidptr,localvertexbuffer,vertexbuffersize);
	vertexbuffer->Unlock();

	result = d3ddev->CreateIndexBuffer(indexbuffersize,0,D3DFMT_INDEX32,D3DPOOL_MANAGED,&indexbuffer,NULL);
	switch(result) {
		case D3DERR_INVALIDCALL: {
			console->Write("Error creating index buffer of size %d: D3DERR_INVALIDCALL\r\n",indexbuffersize);
			break;
		}
		case D3DERR_OUTOFVIDEOMEMORY: {
			console->Write("Error creating index buffer of size %d: D3DERR_OUTOFVIDEOMEMORY\r\n",indexbuffersize);
			break;
		}
		case D3DXERR_INVALIDDATA: {
			console->Write("Error creating index buffer of size %d: D3DXERR_INVALIDDATA\r\n",indexbuffersize);
			break;
		}
		case E_OUTOFMEMORY: {
			console->Write("Error creating index buffer of size %d: E_OUTOFMEMORY\r\n",indexbuffersize);
			break;
		}
	}
	
	// Again, try anyway
	indexbuffer->Lock(0, 0, (void**)&voidptr, 0);
	memcpy(voidptr,localindexbuffer, indexbuffersize);
	indexbuffer->Unlock();
}

void Model::CreateTangents() {
	
	// [T]              [Q1]
	// [B] = inv(s,t) * [Q2]
	
	for(unsigned int i = 0;i < 3*numfaces;i+=3) {
		
		// Ga alle triangles af...
		VERTEX* v0 = &localvertexbuffer[localindexbuffer[i]];
		VERTEX* v1 = &localvertexbuffer[localindexbuffer[i+1]];
		VERTEX* v2 = &localvertexbuffer[localindexbuffer[i+2]];
		
		// Vector along first edge in tex coords
		float2 s;
		s.x = v1->tex.x - v0->tex.x;
		s.y = v2->tex.x - v0->tex.x;
		
		// Vector along second edge in tex coords
		float2 t;
		t.x = v1->tex.y - v0->tex.y;
		t.y = v2->tex.y - v0->tex.y;
		
		// Relative object positions
		float3 q1 = v1->pos - v0->pos;
		float3 q2 = v2->pos - v0->pos;
		
		// then calculate inverse coefficient
		float invcoeff = 1.0f/(s.x*t.y - s.y*t.x); // ad - bc
		
		// calculate unnormalized *triangle* tangents
		float3 tangent = invcoeff * (t.y * q1 - t.x * q2); // Matrixje met dotproduct uitrekenen
		float3 binormal = invcoeff * (-s.y * q1 + s.x * q2);
		
		tangent.Normalize();
		binormal.Normalize();
		
		// Then spread across vertices...
		v0->tan = tangent;
		v0->bin = binormal;
		
		v1->tan = tangent;
		v1->bin = binormal;
		
		v2->tan = tangent;
		v2->bin = binormal;
	}
}

void Model::Print() {
	
	console->Write("\r\n----- Info for class Model -----\r\n\r\n");
	
	console->WriteVar("r",r);
	console->WriteVar("center",center);
	console->WriteVar("numvertices",numvertices);
	console->WriteVar("numfaces",numfaces);
	console->WriteVar("fullpath",fullpath);
	console->WriteVar("filename",filename);
	
	console->Write("\r\n----- End of info -----\r\n\r\n");
}

__int64 Model::GetSize() {
	return numvertices * sizeof(VERTEX) + numfaces * sizeof(int);
}
