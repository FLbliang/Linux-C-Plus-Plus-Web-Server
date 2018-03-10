#include <sys/epoll.h>
#include <time.h>
#include <iostream>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>

#include "utils.h"
#include "record_event.h"
#include "event_control.h"


using std::string;

void EventControl::event_add(const int & events, RecordEvent * record_event){

	struct epoll_event epv = {0, {0}};

	epv.data.ptr = record_event;
	epv.events = events;
	record_event->set_events(events);

	int option = (record_event->get_status() == 1 ? EPOLL_CTL_MOD : EPOLL_CTL_ADD);
	record_event->set_status(1);

	if(epoll_ctl(epoll_fd, option, record_event->get_fd(), &epv) < 0){
		string buf = "time: ";
		buf += get_time();
		buf += "\tevent_add failed!\n";
		write(1, buf.data(), buf.length());
	}

}

void EventControl::event_del(RecordEvent * record_event){

	if(record_event->get_status() != 1)return;

	struct epoll_event epv = {0, {0}};
	
	epv.data.ptr = record_event;
	if(epoll_ctl(epoll_fd, EPOLL_CTL_DEL, record_event->get_fd(), &epv) < 0){
		string buf = "time: ";
		buf += get_time();
		buf += "\tevent_del failed!\n";
		write(1, buf.data(), buf.length());
	}
	record_event->set_status(0);

}

void EventControl::event_set(const int & client_fd, RecordEvent * record_event){

	record_event->set_status(0);
	record_event->set_active(time(NULL));
	record_event->set_fd(client_fd);

}
