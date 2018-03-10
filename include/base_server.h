#ifndef __BASE_SERVER_H__
#define __BASE_SERVER_H__

#include <arpa/inet.h>

#include "netapi.h"

class BaseServer{

protected:
	int listen_fd;
	struct sockaddr_in server_addr;
	
	NetApi net_api;
	
public:
	BaseServer();
	~BaseServer();
	void init_socket(const int & port);
	int get_listenfd();
	bool is_listenfd(const int & fd);

	virtual void process_run() = 0;
	virtual void wait_requests() = 0;
	virtual void accept_clients() = 0;
	virtual void close_client(const int & client_fd) = 0;
	virtual void deal_clients_messages() = 0;
	
};

#endif
