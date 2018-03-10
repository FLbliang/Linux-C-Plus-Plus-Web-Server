#include <string>
#include <unistd.h>

#include "utils.h"
#include "daemond.h"
#include "epoll_server.h"

using std::string;
const int SERVER_PORT = 80;
const int MAX_CLIENT = 1024;

int main(){


	Daemond daemond;
	daemond.create_daemond();

	EpollServer server;
	
	server.init_socket(SERVER_PORT);

	server.set_max_num(MAX_CLIENT);

	server.init_data();

	string buf = "time: ";
	buf += get_time();
	buf += "\tWeb Server is running...\n";
	write(1, buf.data(), buf.length());
	
	server.process_run();

	return 0;
}
