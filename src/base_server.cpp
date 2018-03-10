#include <sys/types.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#include "base_server.h"

BaseServer::BaseServer(){

	listen_fd = -1;
}

BaseServer::~BaseServer(){

	if(listen_fd != -1){
		net_api.Close(listen_fd);
	}
}

void BaseServer::init_socket(const int & port){

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	listen_fd = net_api.Socket(AF_INET, SOCK_STREAM, 0);

	int opt = 1;
	if(setsockopt(listen_fd, SOL_SOCKET,SO_REUSEADDR, (const void *) &opt, sizeof(opt))){
		perror("setsockopt");
		exit(1);
	}

	net_api.Bind(listen_fd, (struct sockaddr *) & server_addr, sizeof(server_addr));


	net_api.Listen(listen_fd, 128);


}

bool BaseServer::is_listenfd(const int & fd){

	return fd == listen_fd;
}
			
int BaseServer::get_listenfd(){
	
	return listen_fd;
}

	

