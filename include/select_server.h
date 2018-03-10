#ifndef __SELECT_SERVER_H__
#define __SELECT_SERVER_H__

#include <map>

#include "base_server.h"

class SelectServer : public BaseServer{
protected:
	bool init_flag;

	socklen_t socket_len;
	sockaddr_in * client_addrs;

	int client_max_num;
	int request_nums;
	std::map<int, int> map_coord;
	int max_fd;
	int max_coord;

public:
	SelectServer();
	~SelectServer();

	void set_max_num(const int & num);
	void display_socket_addr(const int & coord);
	void update_coord(const int & client_fd, const int & coord);
	virtual void init_data();

	virtual void process_run();
	virtual void wait_requests();
	virtual void accept_clients();
	virtual void close_client(const int & client_fd);
	virtual void deal_clients_messages();

private:
	bool is_select;
	int * clients;

	fd_set all_sets;
	fd_set read_sets;

};



#endif
