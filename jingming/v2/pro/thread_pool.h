/* ************************************************************************
 *       Filename:  thread_pool.h
 *    Description:  
 *        Version:  1.0
 *        Created:  2020年06月19日 15时11分01秒
 *       Revision:  none
 *       Compiler:  g++
 *         Author:  YOUR NAME (jing ming), 
 *        Company:  
 * ************************************************************************/

#ifndef _THREAD_POOL_H
#define _THREAD_POOL_H
#include <iostream>
using namespace std;
#include "Cfile.h"
#include "mmanager.h"

#ifdef __cplusplus
extern "C" {
#endif 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <assert.h>

#ifdef __cplusplus
}
#endif

#define w 4000
#define h 3000


typedef struct worker
{
	void (*process)(void* arg,void* arg2);
	void* arg; 
	struct worker* pnext;

}Cthread_worker;


class Cthread_pool{

	public:
		Cthread_pool(int max_thread_num);
		~Cthread_pool();
	public:
		//Cfile
		cfile m_cfile;
		//互斥锁
		pthread_mutex_t queue_lock;
		//条件变量
		pthread_cond_t queue_ready;
		//任务队列的链表头
		Cthread_worker* queue_head;
		int shutdown;  //线程退出标志
		//记录线程ID的数组，记录所有线程ID
		pthread_t* threadid;


		//线程池最大线程数
		int max_thread_num;
		//等待队列中任务个数
		int cur_queue_size;
	public:
		//成员函数
		//添加任务
		int pool_add_worker(void* arg);
		//线程处理函数,线程处理函数是一个全局的回调函数，如果设置为类的成员函数应该设置为static类型，才能在全局空间被调用
		//调用方式使用类名::成员函数名来调用
		
		static void* thread_routine(void* arg); 
		//线程池任务处理函数，同理设置为static类型使此成员函数可以在全局空间使用
		static void myprocess(void* arg,void* arg2);
};



#endif



