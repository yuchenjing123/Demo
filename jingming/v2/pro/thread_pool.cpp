/* ************************************************************************
 *       Filename:  thread_pool.cpp
 *    Description:  
 *        Version:  1.0
 *        Created:  2020年06月20日 09时34分22秒
 *       Revision:  none
 *       Compiler:  g++
 *         Author:  YOUR NAME (jingming), 
 *        Company:  
 * ************************************************************************/

#include "thread_pool.h"

Cthread_pool::Cthread_pool(int max_thread_num){
	pthread_mutex_init(&(this->queue_lock),NULL);
	pthread_cond_init(&(this->queue_ready),NULL);
	this->queue_head = NULL;
	this->max_thread_num = max_thread_num;
	this->cur_queue_size =0;
	this->shutdown =0;
	this->threadid = (pthread_t*)malloc(max_thread_num*sizeof(pthread_t));
	//创建线程池
	int i =0;
	for(i=0;i<max_thread_num;i++){
		pthread_create(&(this->threadid[i]),NULL,thread_routine,this);
	}

}

Cthread_pool::~Cthread_pool(){
	if(this->shutdown){
		printf("shutdown err\n");
		exit(1);
	}
	this->shutdown =1;
	
	//唤醒所有等待线程，线程池要销毁了
	pthread_cond_broadcast(&(this->queue_ready));
	//阻塞等待线程退出，防止变成僵尸线程
	int i;
	for(i=0;i<this->max_thread_num;i++){
		pthread_join(this->threadid[i],NULL);
	}
	if(this->threadid){
		free(this->threadid);
		this->threadid = NULL;
	} 
	
	//销毁等待队列
	Cthread_worker* head = NULL;
	while(this->queue_head !=  NULL){
		head = this->queue_head;
		this->queue_head = this->queue_head->pnext;
		free(head);
	}
	//销毁条件变量和互斥量
	pthread_mutex_destroy(&(this->queue_lock));
	pthread_cond_destroy(&(this->queue_ready));
	
}

//参数arg应该为内存池中的block节点的指针
int Cthread_pool::pool_add_worker(void* arg){
	Cthread_worker* newworker = (Cthread_worker*)malloc(sizeof(Cthread_worker));
	newworker->process  = *(Cthread_pool::myprocess);
	newworker->arg = arg;
	newworker->pnext = NULL;
	
	//投递任务加锁
	pthread_mutex_lock(&(this->queue_lock));
	//将任务加入到等待队列中
	Cthread_worker* member =  this->queue_head;
	if(member!= NULL){
		//遍历链表
		while(member->pnext != NULL)
			member = member->pnext;
		member->pnext = newworker;
	}
	else{
		this->queue_head = newworker;
	}

	assert(this->queue_head!= NULL);
	this->cur_queue_size++;
	pthread_mutex_unlock(&(this->queue_lock));
	//唤醒一个线程工做，使用pthread_cond_signal函数使处于阻塞的线程脱离阻塞状态如果没有线程处于阻塞
	//等待的线程pthread_cond_signal也会成功返回
	pthread_cond_signal(&(this->queue_ready));
	return 0; 
		
}

void* Cthread_pool::thread_routine(void* arg){
	Cthread_pool* pthis = (Cthread_pool*)arg;
	//printf("starting thread 0x%x\n",pthread_self());
	while(1){
		pthread_mutex_lock(&(pthis->queue_lock));
		//如果等待队列为0，并且不销毁线程池，则处于阻塞状态，pthread_cond_wait是一个原子操作
		//等待前会解锁唤醒后会加锁
		while(pthis->cur_queue_size == 0 && !pthis->shutdown){
		//printf("thread 0x%x is waiting \n",pthread_self());
			pthread_cond_wait(&(pthis->queue_ready),&(pthis->queue_lock));
		}
	

		if(pthis->shutdown){
			//先解锁
			pthread_mutex_unlock(&(pthis->queue_lock));
		//	printf("thread 0x%x will exit\n",pthread_self());
			pthread_exit(0);  //当前线程退出
		}
		
		printf("ready to run task\n");
		pthis->cur_queue_size--;
	
		//从任务队列中取出任务
		Cthread_worker* worker = pthis->queue_head;
		pthis->queue_head = worker->pnext;
		pthread_mutex_unlock(&(pthis->queue_lock));
	

		//调用执行任务函数
		(*(worker->process))(worker->arg,pthis);
		free(worker);
		worker= NULL;
	}
}

void Cthread_pool::myprocess(void* arg,void* arg2){
	//此处进行图片YUV分离，并进行文件的写入
	printf("myprocess is runing \n");
	//解析this指针
	Cthread_pool* pthis = (Cthread_pool*)arg2;
	//解析arg参数,获得内存块的使用权
	memblock* pblock = (memblock*)arg;
	//记录block下的内存初始位置
	char* pmark = pblock->pMem;
	//记录写入的字节数
	int n= 0;
	

	//以下是业务区域,注释为要点说明
	
	//进行正式工作,从内存块中读取数据，写入到./output中
	//新的文件名为内存块记录的文件编号后缀名为.nv12
	
	char outfilename[50]={0};
	sprintf(outfilename,"./output/%d.nv12",pblock->filenum);
	pthis->m_cfile.Fopen(outfilename,"wb");
	
	while(n<(w*h)){
		if((fwrite(pblock->pMem,1,1,pthis->m_cfile.Pfile))<1)break;
		pblock->pMem++;
		n++;
	}
	int n1 =0;
	int tem =128;
	while(n1<(w*h/2)){
		if((fwrite(&tem,1,1,pthis->m_cfile.Pfile))<1)break;
		n1++;
	}
	printf("%d.nv12处理成功，写入了%d个字节\n",pblock->filenum,n+n1);

	//关闭文件
	pthis->m_cfile.Fclose();
	//以下结束业务区域

	//释放使用过的内存块
	pblock->pMem = pmark;
	if(pblock){
		if(pblock->pMem){
			free(pblock->pMem);
			pblock->pMem = NULL;
		}
		free(pblock);
		pblock = NULL;	
	}
	
	
}
