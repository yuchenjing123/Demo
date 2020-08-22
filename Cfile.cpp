/* ************************************************************************
 *       Filename:  Cfile.cpp
 *    Description:  
 *        Version:  1.0
 *        Created:  2020年06月22日 11时06分54秒
 *       Revision:  none
 *       Compiler:  g++
 *         Author:  YOUR NAME (jingming), 
 *        Company:  
 * ************************************************************************/

#include "./Cfile.h"

cfile::cfile(){
	printf("cfile construct is run\n");
	pdir = NULL;
	pdirent = NULL;
	Pfile = NULL;
}

cfile::~cfile(){
	printf("cfile destruct is run\n");
}

int cfile::getdir_filenum(const char* path){
	int n = 0;
	DIR* pdir;
	dirent* pdirent;
	pdir = opendir(path);
	while((pdirent = readdir(pdir))!= NULL){
		if(strcmp(pdirent->d_name,".")==0||strcmp(pdirent->d_name,"..")==0) continue;
		n++;
	}
	closedir(pdir);
	return n;
}

long cfile::filesize(FILE* pFile){
	long curpos,length;
	curpos = ftell(pFile);
	fseek(pFile,0L,SEEK_END);
	length = ftell(pFile);
	fseek(pFile,curpos,SEEK_SET);
	return length;
	
}

void cfile::OpenDir(const char* str){
	this->pdir = opendir(str);
}

bool cfile::ReadDir(){
	this->pdirent = readdir(this->pdir);
	if(this->pdirent  == NULL){
		return false;
	}
	return true;
}

void cfile::Fopen(const char*str,const char* rw){
	this->Pfile = fopen(str,rw);
}

void cfile::Fclose(){
	if(this->Pfile){
		fclose(this->Pfile);
		this->Pfile = NULL;
	}
}

int cfile::getfilenum(){
	int num = 0;
	char  filenumstr[10] = {0};
	int i =0;
	while(this->pdirent->d_name[i] != '.'){
		filenumstr[i] = this->pdirent->d_name[i];
		i++;
	}
	num = atoi(filenumstr);
	return num;
}
