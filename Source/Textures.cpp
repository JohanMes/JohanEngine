#include <cstdio>
#include "Textures.h"
#include "Console.h"

Textures::Textures() {
}
Textures::~Textures() {
	Clear();
}

Texture* Textures::Add() {
	return Add(new Texture());
}
Texture* Textures::Add(const char* filename) {
	
	// Obtain full path
	char fullpath[MAX_PATH];
	GetFullPath(filename,"Data\\Textures",fullpath);

	// Did we add that one already?
	Texture* search = GetByFullPath(fullpath);
	if(search) {
		return search;
	}
	
	// Not found? Add it to the pile
	return Add(new Texture(filename));
}

Texture* Textures::Add(Texture* texture) {
	list.push_back(texture);
	texture->bufferlocation = --list.end();
	return texture;
}

Texture* Textures::GetByFullPath(const char* fullpath) {
	for(std::list<Texture*>::iterator i = list.begin();i != list.end();i++) {
		if(!strcmp(fullpath,(*i)->fullpath)) {
			return *i;
		}
	}
	return NULL;
}

Texture* Textures::GetByFileName(const char* filename) {
	for(std::list<Texture*>::iterator i = list.begin();i != list.end();i++) {
		if(!strcmp(filename,(*i)->filename)) {
			return *i;
		}
	}
	return NULL;
}

void Textures::Delete(Texture* thistexture) {
	if(thistexture) {
		list.erase(thistexture->bufferlocation);
		delete thistexture;
	}
}

void Textures::Clear() {
	for(std::list<Texture*>::iterator i = list.begin();i != list.end();i++) {
		delete *i;
	}
	list.clear();
}

void Textures::Print() {
	console->WriteVar("list.size()",(int)list.size());
	for(std::list<Texture*>::iterator i = list.begin();i != list.end();i++) {
		(*i)->Print();
	}
}

void Textures::SaveToCSV() {
	
	// Save next to exe
	char finalpath[MAX_PATH];
	sprintf(finalpath,"%s\\%s",exepath,"Textures.csv");

	// Save grand total too
	unsigned int totalbytes = 0;
	
	FILE* file = fopen(finalpath,"wb");
	if(file) {
		fprintf(file,"Full path;Width;Height;Size (bytes)\r\n");
		for(std::list<Texture*>::iterator i = list.begin();i != list.end();i++) {
			Texture* texture = *i;
			fprintf(file,"%s;%u;%u;%llu\r\n",
				texture->fullpath,
				texture->GetWidth(),
				texture->GetHeight(),
				texture->GetSize());
			
			totalbytes += texture->GetSize();
		}
		
		// TODO: create function
		if(totalbytes > 1024*1024*1024) { // 1 GiB
			fprintf(file,";;;%u (%.2f GiB)",totalbytes,totalbytes/1024.0f/1024.0f/1024.0f);
		} else if(totalbytes > 1024*1024) { // 1 MiB
			fprintf(file,";;;%u (%.2f MiB)",totalbytes,totalbytes/1024.0f/1024.0f);
		} else if(totalbytes > 1024) { // 1 KiB
			fprintf(file,";;;%u (%.2f KiB)",totalbytes,totalbytes/1024.0f);
		} else {
			fprintf(file,";;;%u bytes",totalbytes);
		}

		fclose(file);
	}
}
