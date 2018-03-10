#ifndef __RECORDEVENT_H__
#define __RECORDEVENT_H__

#include <pthread.h>

#include "task.h"

class EventControl;

class RecordEvent{

public:
	RecordEvent();
	~RecordEvent();

	void set_bufSize(const int & buf_size){ this->buf_size = buf_size; buf = new char [buf_size]; }
	int get_bufSize(){ return buf_size; }
	void set_fd(const int & fd){ this->fd = fd; task.set_fd(fd); }
	int get_fd(){ return fd; } 
	void set_events(const int & events){ this->events = events; }
	int get_events(){ return events; }
	void set_status(const int & status){ this->status = status; }
	int get_status(){ return status; }
	void set_active(const long long & active){ this->active = active; }
	long long get_active(){ return active; }
	
	bool send_data(EventControl * event_control);
	bool recv_data(EventControl * event_control);

	pthread_mutex_t mutex;
	
private:
	int buf_size;
	int fd;
	int events;
	int status;
	char * buf;
	int len;
	long long active;

	Task task;
	
};


#endif 
