#include <cstdio>

#include "Heightmap.h"
#include "Console.h"

Heightmap::Heightmap(const char* path,float minheight,float maxheight,float widthmeters) {
	this->widthpixels = 0; // halen we uit bitmap
	this->widthmeters = widthmeters;
	this->minheight = minheight;
	this->maxheight = maxheight;
	Load(path,minheight,maxheight);
}
Heightmap::~Heightmap() {
	if(colortable) {
		for(int i = 0;i < widthpixels;i++) {
			free(colortable[i]);
		}
		free(colortable);
		colortable = NULL;
	}
}
void Heightmap::Load(const char* path,float minheight,float maxheight) {
	
	// Open the heightmap
	FILE* bitmap = fopen(path,"rb");
	if(bitmap == NULL) {
		console->Write("Error opening heightmap %s\r\n",path);
		return;
	}
	
	// Eerst de headers lezen
	BITMAPFILEHEADER bmfh;
	fread(&bmfh,sizeof(BITMAPFILEHEADER),1,bitmap);
	
	// Nog eentje
	BITMAPINFOHEADER bmih;
	fread(&bmih,sizeof(BITMAPINFOHEADER),1,bitmap);
	
	if(bmih.biWidth != bmih.biHeight) {
		console->Write("Heightmap must be a square picture\r\n",path);
		fclose(bitmap);
		return;
	}
	
	// 1D-array van de kleuren van de pixels
	float* colors1d = NULL;
	
	// Wat gegevens instellen
	widthpixels = bmih.biWidth;
	this->minheight = minheight;
	this->maxheight = maxheight;
	unsigned int sizebytes = bmfh.bfSize - bmfh.bfOffBits;	
	
	if(bmih.biBitCount == 8) {
	
		// Bij 8bpp wordt van elke pixel een index in de colortable opgeslagen, dus moeten we die tabel even aanmaken
		RGBQUAD* colortable = new RGBQUAD[256];
		fread(colortable,sizeof(RGBQUAD),256,bitmap);

		// Leede colortable-indices
		BYTE* colortableindices = new BYTE[sizebytes];
		fread(colortableindices,1,sizebytes,bitmap);
		
		// Maak van de indices van de colortable floats
		colors1d = new float[sizebytes];
		for(unsigned int i = 0;i < sizebytes;i++) { // alle pixelaflopen
			colors1d[i] = (colortable[colortableindices[i]].rgbRed + 
						   colortable[colortableindices[i]].rgbGreen + 
						   colortable[colortableindices[i]].rgbBlue)/3.0f;
		}
		
		delete[] colortable;
		delete[] colortableindices;
	} else if(bmih.biBitCount == 24) {
		
		// Lees de RGB-componenten
		BYTE* rgb = new BYTE[sizebytes];
		fread(rgb,1,sizebytes,bitmap);

		// Maak floats van de RGB-componenten
		colors1d = new float[sizebytes/3]; // elke 24 bit (per pixel) leveren 4 bytes (1 float) op
		for(unsigned int i = 0;i < sizebytes/3;i++) {
			colors1d[i] = rgb[3*i]*rgb[3*i+1]*rgb[3*i+2];
		}
		
		delete[] rgb;
	} else {
		delete[] colors1d;
		console->Write("Heightmap must be an 8bpp or 24bpp file\r\n",path);
		fclose(bitmap);
		return;
	}
	
	// En de 2D array maken
	colortable = new float*[widthpixels]; // array of pointers
	for(int i = 0;i < widthpixels;i++) {
		colortable[i] = new float[widthpixels]; // require a square picture
	}
	
	float newlen = maxheight-minheight;
	float oldlen = 1 << bmih.biBitCount;
	
	// En bij elke pixel een box filter toepassen
	float final = 0.0f;
	int samplecount = 0;
	
	// Elke pixel van de 2D-array langsgaan...
	for(int Z = 0;Z < widthpixels;Z++) { // Right
		for(int X = 0;X < widthpixels;X++) { // Down
		
			final = 0.0f;
			samplecount = 0;
			
			// Sample nine points and take average around the integer point
			for(int i = X - 1;i <= X + 1;i++) {
				if(i >= 0 && i < widthpixels) {
					for(int j = Z - 1;j <= Z + 1;j++) {
						if(j >= 0 && j < widthpixels) {
							final += colors1d[i + widthpixels*j];
							samplecount++;
						}
					}
				}
			}
			final /= samplecount;
			
			// Hier schalen van [0,2^x] tot [0,maxheight-minheight]
			final *= (newlen/oldlen);
			
			// En het nulpunt ophogen tot minheight
			final += minheight;
		
			colortable[X][Z] = final;
		}
	}
	
	delete[] colors1d;
	
	fclose(bitmap);
}
float Heightmap::GetNearestHeight(float x,float z) {

	// Just round off the coordinates and sample the nearest point
	
	// Convert to uniform [0..1] coordinates
	float xscaled = 2.0f*x/widthmeters; // [-1..1]
	xscaled = (xscaled + 1.0f)/2.0f; // [0..1]

	// Convert to uniform [0..1] coordinates
	float zscaled = 2.0f*z/widthmeters; // [-1..1]
	zscaled = (zscaled + 1.0f)/2.0f; // [0..1]
	
	// Get the actual indices
	int intx = round(xscaled * (widthpixels - 1));
	int intz = round(zscaled * (widthpixels - 1));
	
	if(intx >= widthpixels) {
		intx = widthpixels-1;
	} else if(intx < 0) {
		intx = 0;
	}
	if(intx >= widthpixels) {
		intz = widthpixels-1;
	} else if(intz < 0) {
		intz = 0;
	}

	return colortable[intx][intz];
}
float Heightmap::GetBilinearHeight(float x,float z) {
	
	// Convert to uniform [0..1] coordinates
	float xscaled = 2.0f*x/widthmeters; // [-1..1]
	xscaled = (xscaled + 1.0f)/2.0f; // [0..1]

	// Convert to uniform [0..1] coordinates
	float zscaled = 2.0f*z/widthmeters; // [-1..1]
	zscaled = (zscaled + 1.0f)/2.0f; // [0..1]

	// Clamp to edge
	xscaled = std::min(1.0f,xscaled);
	xscaled = std::max(0.0f,xscaled);
	zscaled = std::min(1.0f,zscaled);
	zscaled = std::max(0.0f,zscaled);
		
	// Sample four coordinates around our point
	float xidx = xscaled * (widthpixels - 1);
	float zidx = zscaled * (widthpixels - 1);
	
	// Sample four points around the float point
	float topleft = colortable[(int)floor(xidx)][(int)floor(zidx)];
	float topright = colortable[(int)ceil(xidx)][(int)floor(zidx)];
	float bottomleft = colortable[(int)floor(xidx)][(int)ceil(zidx)];
	float bottomright = colortable[(int)ceil(xidx)][(int)ceil(zidx)];

	// Interpolate over Xtop
	float xtop = topleft + (topright - topleft) * (xidx - floor(xidx));
	
	// Interpolate over Xbottom
	float xbottom = bottomleft + (bottomright - bottomleft) * (xidx - floor(xidx));
	
	// Interpolate over Z
	return xtop + (xbottom - xtop) * (zidx - floor(zidx));
}
