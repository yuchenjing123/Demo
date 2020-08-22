/* ************************************************************************
 *       Filename:  Cfile.h
 *    Description:  
 *        Version:  1.0
 *        Created:  2020年06月22日 09时38分59秒
 *       Revision:  none
 *       Compiler:  g++
 *         Author:  YOUR NAME (jingming), 
 *        Company:  
 * ************************************************************************/


#ifndef _CFILE_H
#define _CFILE_H

#ifdef __cplusplus
extern "C"{
#endif

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <dirent.h>
#include <string.h>

#ifdef __cplusplus
}
#endif 


class cfile{
	public: 
		cfile();
		~cfile();
	public:
		DIR* pdir;
		dirent* pdirent;
		FILE* Pfile;
	public:
		//获得目录下文件个数
		int getdir_filenum(const char* path);
		//获取文件大小
		long filesize(FILE* pFile);
		//opendir
		void OpenDir(const char* str);
		//readdir
		bool ReadDir();
		//getfilenum 获取文件编号
		int getfilenum();
		
		//openfile
		void Fopen(const char* str ,const char* rw);
		void Fclose();

};

#endif

