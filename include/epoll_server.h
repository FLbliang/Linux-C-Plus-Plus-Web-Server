#ifndef __EPOLLSERVER_H__
#define __EPOLLSERVER_H__

#include <sys/epoll.h>

#include "select_server.h"
#include "event_control.h"
#include "record_event.h"
#include "pthread_pool.h"


class EpollServer : public SelectServer{

public:
	EpollServer();
	~EpollServer();

	void check_active();

	virtual void init_data();
	virtual void process_run();
	virtual void wait_requests();
	virtual void accept_clients();
	virtual void close_client(const int & client_fd);
	virtual void deal_clients_messages();

private:
	static void * do_task(void * arg);
	struct DataRecord{
		RecordEvent * re_ev;
		EpollServer * server;
		bool is_rec;
		DataRecord(RecordEvent * re_ev, EpollServer * server, bool is_rec = true) : re_ev(re_ev), server(server), is_rec(is_rec){}
	};

private:
	int epoll_fd = 0;
	struct epoll_event * socket_events = nullptr;
	RecordEvent * record_events = nullptr;
	EventControl * event_control = nullptr;
	PthreadPool * pool = nullptr;

};


#endif
	
