#include <time.h>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "event_control.h"
#include "record_event.h"


RecordEvent::RecordEvent() : buf_size(0), fd(-1), events(-1), status(0), buf(NULL), len(0), active(time(NULL)){
	pthread_mutex_init(&mutex, NULL);
}

RecordEvent::~RecordEvent(){
	delete [] buf;
	pthread_mutex_destroy(&mutex);
}

bool RecordEvent::send_data(EventControl * event_control){

	task.response_send();	
	event_control->event_add(EPOLLIN, this);

	return true;
		
}

bool RecordEvent::recv_data(EventControl * event_control){

	len = recv(fd, buf, buf_size, 0);

	if(len > 0){

		buf[len] = '\0';
		task.handle_request(buf);
		event_control->event_add(EPOLLOUT, this);

		return true;
		
	}else{
		return false;
	}
}




















