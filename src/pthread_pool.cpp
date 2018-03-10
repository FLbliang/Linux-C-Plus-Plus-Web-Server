#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "pthread_pool.h"

PthreadPool::PthreadPool(const int min_num, const int max_num, const int task_size)
: m_min_num(min_num), m_max_num(max_num), m_queue(new Queue(task_size)){}
				
PthreadPool::~PthreadPool(){
	if(m_create_flag){
		pool_destroy();
	}else{
		delete m_queue;
	}
}

void PthreadPool::pool_destroy(){
	wait_mutex(m_pool_mutex);
	m_shutdown = true;
	int tmp_current_num = m_current_num;
	post_mutex(m_pool_mutex);
	
	// 唤醒并关闭当前所有线程
	for(int i = 0; i < tmp_current_num; ++i){
		m_queue->shutdown_wake();
	}

	bool flag = true;

	while(flag){
		wait_mutex(m_pool_mutex);
		flag = m_current_num > 0;
		post_mutex(m_pool_mutex);
	}

	pthread_mutex_destroy(&m_pool_mutex);
	pthread_mutex_destroy(&m_busy_mutex);
	delete [] m_threads;
	delete m_queue;
}

// 创建线程池
bool PthreadPool::create_pool(){
	m_threads = new pthread_t [m_max_num];

	// 初始化互斥锁
	if(pthread_mutex_init(&m_pool_mutex, NULL) || pthread_mutex_init(&m_busy_mutex, NULL)){
		return false;
	}

	// 往线程池里创建多个线程
	for(int i = 0; i < m_min_num; ++i){
		create_thread(m_threads[i], work_thread);	
	}

	// 创建管理者线程
	create_thread(m_manage_id, manage_thread);
	m_create_flag = true;

	return true;
	
}

void PthreadPool::create_thread(pthread_t & thread_id, void *(*thread)(void *)){
	
	if(pthread_create(&thread_id, NULL, thread, (void *)this) != 0){
		perror("pthread_create error");
		exit(1);
	}

	// 线程分离,销毁时自动回收
	if(pthread_detach(thread_id) != 0){
		perror("pthread_detach error");
		exit(1);
	}

	//printf("create thread\n");

	wait_mutex(m_pool_mutex);
	if(thread_id != m_manage_id){
		++m_current_num;
	}
	post_mutex(m_pool_mutex);
}

// 判断线程是否存在
bool PthreadPool::judge_thread(const pthread_t & thread_id){

	return thread_id != 0;
}

// 添加任务
bool PthreadPool::add_task(void *(*func)(void *), void * arg){
	Record task;
	task.func = func;
	task.arg = arg;
	bool flag = true;
	// 获取操作线程池的互斥锁
	wait_mutex(m_pool_mutex);

	if(m_queue->get_maxSize() <= m_queue->get_curSize()){
		post_mutex(m_pool_mutex);
		return false;
	}

	flag = m_queue->add_task(task);
		
	// 释放操作线程池的互斥锁
	post_mutex(m_pool_mutex);

	return flag;
}

// 线程池管理线程
void * PthreadPool::manage_thread(void * arg){

	PthreadPool * pool = static_cast<PthreadPool *>(arg);
	bool shutdown_flag = false;
	int create_num = 0;
	while(!shutdown_flag){

		// 任务过多且空余线程较少时，增加线程个数
		pool->wait_mutex(pool->m_pool_mutex);
		shutdown_flag = pool->m_shutdown;
		pool->post_mutex(pool->m_pool_mutex);

		if(shutdown_flag)break;
				
		// 任务队列的任务数大于最少线程数且当前的线程数少于线程数量的最大容量时

		pool->wait_mutex(pool->m_pool_mutex);
		if(pool->m_queue->get_curSize() > pool->m_min_num && pool->m_current_num < pool->m_max_num){
			create_num = MAX_DEFAULT_EXIT_NUM;
		}else{
			create_num = 0;
		}
		pool->post_mutex(pool->m_pool_mutex);

		while(create_num--){
			for(int i = 0; i < pool->m_max_num; ++i){
				if(pool->judge_thread(pool->m_threads[i]))continue;

				pool->create_thread(pool->m_threads[i], PthreadPool::work_thread);
				break;
				
			}
		
			pool->wait_mutex(pool->m_pool_mutex);
			create_num = (pool->m_queue->get_curSize() > pool->m_min_num 
								&& pool->m_current_num < pool->m_max_num ? create_num : 0);
			pool->post_mutex(pool->m_pool_mutex);
					
		}

		// 释放CPU资源
		sleep(1);

		// 任务较少且空余线程过多时，销毁一些线程
		pool->wait_mutex(pool->m_pool_mutex);
		// 只有一半线程在忙碌且当前线程数大于最少的线程数时
		int exit_num = pool->MAX_DEFAULT_EXIT_NUM;
		if(pool->m_current_num > pool->m_min_num && pool->m_busy_num*2 < pool->m_current_num){
			pool->m_exit_num = pool->MAX_DEFAULT_EXIT_NUM;
		}else{
			exit_num = 0;
		}

		pool->post_mutex(pool->m_pool_mutex);

		while(exit_num--){
			pool->wait_mutex(pool->m_pool_mutex);
			if(!pool->m_queue->shutdown_wake()){
				perror("shutdown_wake error");
				exit(1);
			}
			pool->post_mutex(pool->m_pool_mutex);
		}
	}

	//printf("shutdown manage thread\n");
	return NULL;
}

// 线程池工作线程
void * PthreadPool::work_thread(void * arg){

	PthreadPool * pool = static_cast<PthreadPool *>(arg);
	Record task;
	bool shutdown_flag = false;
	while(!shutdown_flag){
		
		task.clear();
		// 阻塞取任务
		if(!pool->m_queue->get_task(task)){
			// 系统异常
			perror("get task error");
			exit(1);
		}


		pool->wait_mutex(pool->m_pool_mutex);
		// 需要销毁一些空闲线程的情况
		if(pool->m_exit_num > 0 && pool->m_current_num > pool->m_min_num){
			shutdown_flag = true;
			--pool->m_exit_num;
		}else if(pool->m_current_num <= pool->m_min_num){
			pool->m_exit_num = 0;
		}
		pool->post_mutex(pool->m_pool_mutex);

		//关闭标志被设置, 退出循环
		if(shutdown_flag){
			break;
		}

		// 获取是否要销毁的状态标志
		pool->wait_mutex(pool->m_pool_mutex);
		shutdown_flag = pool->m_shutdown;
		pool->post_mutex(pool->m_pool_mutex);
			
		//关闭标志被设置, 退出循环
		if(shutdown_flag){
			break;
		}

		//printf("add task\n");

		// 增加忙线程个数
		pool->wait_mutex(pool->m_busy_mutex);			
		++pool->m_busy_num;
		pool->post_mutex(pool->m_busy_mutex);
		
		// 执行回调函数
		if(task.func != nullptr){
			(*task.func)(task.arg);
		}

		//printf("end task\n");

		// 减少忙线程个数
		pool->wait_mutex(pool->m_busy_mutex);
		--pool->m_busy_num;
		pool->post_mutex(pool->m_busy_mutex);
		

	}

	pool->wait_mutex(pool->m_pool_mutex);
	--pool->m_current_num;
	pthread_t cur_thread = pthread_self();
	for(int i = 0; i < pool->m_max_num; ++i){
		if(pool->m_threads[i] == cur_thread){
			pool->m_threads[i] = 0;
			break;
		}
	}
	pool->post_mutex(pool->m_pool_mutex);

	//printf("exit work thread\n");

	return NULL;
}

// 等待互斥锁
void PthreadPool::wait_mutex(pthread_mutex_t & mutex){
	//printf("wait\n");
	if(pthread_mutex_lock(&mutex) != 0){
		perror("wait mutex error");	
		exit(1);
	}
}

// 释放互斥锁
void PthreadPool::post_mutex(pthread_mutex_t & mutex){
	//printf("post\n");
	if(pthread_mutex_unlock(&mutex) != 0){
		perror("wait mutex error");
		exit(1);
	}
}

PthreadPool::Queue::Queue(const int max_size) : m_que_max_size(max_size)
{
	m_tasks = new Record [max_size];
	sem_init(&m_sem_full, 0, 0);
	sem_init(&m_sem_empty, 0, max_size);
	pthread_mutex_init(&m_mutex, NULL);
}

PthreadPool::Queue::~Queue(){
	sem_destroy(&m_sem_full);
	sem_destroy(&m_sem_empty);
	pthread_mutex_destroy(&m_mutex);	
	delete m_tasks;
}

// 请求信号量
bool PthreadPool::Queue::wait_sem(sem_t & sem){

	return sem_wait(&sem) == 0 && pthread_mutex_lock(&m_mutex) == 0;
}

// 释放信号量
bool PthreadPool::Queue::post_sem(sem_t & sem){
	
	return sem_post(&sem) == 0 && pthread_mutex_unlock(&m_mutex) == 0;
}

// 任务入队
bool PthreadPool::Queue::add_task(const Record & task){
	if(!wait_sem(m_sem_empty)){
		return false;
	}

	m_tasks[m_que_rear].func = task.func;
	m_tasks[m_que_rear].arg = task.arg;
	m_que_rear = (m_que_rear + 1) % m_que_max_size;
	++m_que_size;

	return post_sem(m_sem_full);

}

// 出队，获取任务
bool PthreadPool::Queue::get_task(Record & task){
	if(!wait_sem(m_sem_full)){
		return false;
	}

	if(m_que_size > 0){
		task.func = m_tasks[m_que_front].func;
		task.arg = m_tasks[m_que_front].arg;
	
		m_tasks[m_que_front].func = nullptr;
		m_tasks[m_que_front].arg = nullptr;

		m_que_front = (m_que_front + 1) % m_que_max_size;
		--m_que_size;
	}

	return post_sem(m_sem_empty);
}

bool PthreadPool::Queue::shutdown_wake(){
	
	if(!wait_sem(m_sem_empty)){
		return false;
	}

	return post_sem(m_sem_full);
	
}





























