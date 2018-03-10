#ifndef __EVENTCONTROL_H__
#define __EVENTCONTROL_H__

struct epoll_event;
class RecordEvent;

class EventControl{

public:
	
	inline void set_epollfd(const int & epoll_fd){ this->epoll_fd = epoll_fd; }
	inline int get_epollfd(){ return epoll_fd; }
	
	void event_add(const int & events, RecordEvent * record_event);
	void event_del(RecordEvent * record_event);
	void event_set(const int & client_fd, RecordEvent * record_event);

private:
	int epoll_fd;
	

};


#endif
