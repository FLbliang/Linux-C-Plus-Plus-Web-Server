#ifndef __PTHREADPOOL__
#define __PTHREADPOOL__

#include <pthread.h>
#include <semaphore.h>


class PthreadPool{
	public:
		explicit PthreadPool(const int min_num = 4, const int max_num = 10, const int task_size = 10);
		~PthreadPool();

		bool create_pool();//创建线程池
		bool add_task(void *(*func)(void *), void * arg);// 添加任务
		void pool_destroy();

	private:
		static void * manage_thread(void *);// 线程池里面的管理线程
		static void * work_thread(void *); // 线程池里面的工作线程
		
		void create_thread(pthread_t & thread_id, void *(*thread)(void *));// 创建线程	
		bool judge_thread(const pthread_t & thread_id); // 判断线程是否存在
		void wait_mutex(pthread_mutex_t & mutex); // 阻塞等待互斥锁
		void post_mutex(pthread_mutex_t & mutex); // 释放互斥锁

	private:
		// 记录任务信息和任务执行的回调函数,便于线程回调执行任务
		struct Record{
			void *(*func)(void *) = nullptr; // 回调函数
			void * arg = nullptr; // 回调函数参数
			void clear(){
				func = nullptr;
				arg = nullptr;
			}
		};

		class Queue{
			private:
				int m_que_max_size; // 任务队列可容纳长度
				int m_que_front; // 任务队列头指针
				int m_que_rear; // 任务队列尾指针
				int m_que_size; // 任务队列长度
				sem_t m_sem_full; // 任务队列为满的信号量
				sem_t m_sem_empty; // 任务队列为空的信号量
				Record * m_tasks; // 任务队列
				pthread_mutex_t m_mutex;
				
				bool wait_sem(sem_t & sem);
				bool post_sem(sem_t & sem);

			public:
				explicit Queue(const int max_size = 10);
				~Queue();
				bool add_task(const Record & task); // 任务入队
				bool get_task(Record & task); // 出队, 获取任务
				bool shutdown_wake(); // 唤醒等待任务的线程并让其自毁
				int get_maxSize() const { return m_que_max_size; }
				int get_curSize() const { return m_que_size; }
				
		};

	private:
		enum{MAX_DEFAULT_EXIT_NUM = 4};
		pthread_mutex_t m_pool_mutex; // 操作整个线程池的互斥锁	
		pthread_mutex_t m_busy_mutex; // 忙状态线程个数操作的锁

		pthread_t m_manage_id; // 管理线程ID
		pthread_t * m_threads = nullptr; 	// 线程组
		
		int m_busy_num = 0; // 忙状态的线程数量
		int m_current_num = 0; // 当前线程池的线程总数
		int m_max_num; // 线程数量的最大容量
		int m_min_num; // 最少的线程数
		int m_exit_num = 0; // 线程管理时要销毁的线程
		Queue * m_queue = nullptr;
		bool m_shutdown = false;
		bool m_create_flag = false; // 是否创建了线程池

};



#endif
