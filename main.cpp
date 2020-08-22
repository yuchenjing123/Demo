 /************************************************************************
 *       Filename:  main.cpp
 *    Description:  
 *        Version:  1.0
 *        Created:  20/06/23 13:08:13
 *       Revision:  none
 *       Compiler:  g++
 *         Author:  YOUR NAME (jingming), 
 *        Company:  
 * ************************************************************************/

#include "./mmanager.h"
#include "./thread_pool.h"
#include "./Cfile.h"

#ifdef __cplusplus
extern "C"{

#endif
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus 
}
#endif

#include <iostream>
using namespace std;
int main(){

	cfile* pfile = NULL;;
	pfile = new cfile;

	//创建thread_pool
	Cthread_pool* pThreadpool = new Cthread_pool(1);


	//创建memery_pool
	MM* pmanager = NULL;
	pmanager = new MM(12,(w*h*3));
	if(pmanager == NULL){
		cout<< "create err" <<endl;
	}
	else{
		cout<< "create ok" << endl;
	}
	cout<<"内存块数量"<< pmanager->pMmanager->count <<"块大小是"<< pmanager->pMmanager->blocksize <<"使用个数为"<< pmanager->pMmanager->usedCount<<endl;
	memblock* pmemblock = pmanager->Getblock();
	cout<< "内存块的编号为" << pmemblock->blocknum << endl;
	cout<<"内存块数量"<< pmanager->pMmanager->count <<"块大小是"<< pmanager->pMmanager->blocksize <<"使用个数为"<< pmanager->pMmanager->usedCount<<endl;
    if(pmanager->Releaseblock(pmemblock)==ERR)
	{
		cout<<"release block err"<< endl;
	}
	else{
		cout<< "release block ok" <<endl;
	}
	
	//主要工作ready to run, 从./input中读取yuv数据
	//遍历./input,并将文件读入memblock中
	//当写完一帧后，将block投递到线程池
	memblock* ptemp = NULL;
	//存储格式化路径的数组
	char path[50]= {0};
	//记录文件大小
	long file_size = 0;  //初始为0
	//读到的字节数
	int readednum = 0;
	//每次读取的字节
	int oncereadnum =0;
	//记录block中mem的起始地址
	char* pstart = NULL;
	//读取./input目录
	pfile->OpenDir("./input");
	while( (pfile->ReadDir()) ==true){
		if(strcmp(pfile->pdirent->d_name,".")==0
				||strcmp(pfile->pdirent->d_name,"..")==0)
			continue;
		ptemp = pmanager->Getblock();
		pstart = ptemp->pMem;
		//获取文件编号
		ptemp->filenum = pfile->getfilenum();
		cout<< "文件编号为"<<ptemp->filenum << endl;
		cout<<ptemp->filenum<<"号文件准别读取"<<endl;
		//读取文件，并写入block,文件编号用于指定打开当前名称的文件
		sprintf(path,"./input/%d.nv12",ptemp->filenum);
		pfile->Fopen(path,"rb");
		if(pfile->Pfile){
			cout<< ptemp->filenum<<"号码文件打开成功"<<endl;
		}
		else{
			cout<< ptemp->filenum<<"号码文件打开失败"<<endl;	
		}
		//先获取文件大小
		file_size = pfile->filesize(pfile->Pfile);
		cout <<ptemp->filenum<<"号" <<"文件大小是"<< file_size << endl;
		//向内存块中写入数据并且将写好的内存投递到线程池中
		while(readednum<file_size){
			if((oncereadnum = fread(ptemp->pMem,1,1,pfile->Pfile))<1)break;
			ptemp->pMem = ptemp->pMem+oncereadnum;
			readednum = readednum + oncereadnum;
		}
		//文件写入完成
		cout<< ptemp->filenum << "号文件" <<"写入到"<<ptemp->blocknum<<"号内存块成功" <<endl;

		//还原初始值，为下次使用
		ptemp->pMem = pstart;
		file_size = 0;
		readednum = 0;
		oncereadnum = 0;
		//先关闭文件
		pfile->Fclose();
		//将内存块投递到线程池的任务队列中
		pThreadpool->pool_add_worker(ptemp);	
	}

	//结束主线程
	
	//判断任务队列是否为空，如果不为空等待任务完成
	while(pThreadpool->cur_queue_size > 0);
	//destruct thread_pool	
	delete pThreadpool;
	//destruct MM
	delete pmanager;	
    //destruct cfile
	delete pfile;

	return 0;
}


