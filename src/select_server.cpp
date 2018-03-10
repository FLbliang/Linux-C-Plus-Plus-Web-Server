#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <arpa/inet.h>

#include "utils.h"
#include "select_server.h"

using std::string;
using std::map;
using std::reverse;

SelectServer::SelectServer(){

	is_select = false;
	client_max_num = 0;
	init_flag = false;
}

SelectServer::~SelectServer(){

	if(client_max_num && is_select){
		for(int i = 1; i <= max_coord; ++i){

			if(clients[i] != -1){
				net_api.Close(clients[i]);
			}
		}
		delete [] clients;
		delete [] client_addrs;
	}
}

void SelectServer::set_max_num(const int & num){
	client_max_num = num;
}

void SelectServer::init_data(){
	
	if(!client_max_num)return;

	client_addrs = new sockaddr_in[client_max_num];
	clients = new int [client_max_num];

	FD_ZERO(&all_sets);
	max_coord = 0;
	max_fd = listen_fd;
	clients[max_coord] = max_fd;
	map_coord[listen_fd] = 0;
	
	for(int i = max_coord+1; i < client_max_num; ++i){
		clients[i] = -1;
		map_coord[i] = -1;	
	}

	FD_SET(max_fd, &all_sets);
	init_flag = true;
	is_select = true;
	
}

void SelectServer::display_socket_addr(const int & coord){

	char bufdata[BUFSIZ];
	string buf = "time: ";
	buf += get_time();

	const char * ip = inet_ntop(AF_INET, &client_addrs[coord].sin_addr, bufdata, sizeof(bufdata));
	int port = ntohs(client_addrs[coord].sin_port);

	buf += "\tclient ip is: ";
	buf += ip;
	buf += "\tclient port is: ";

	string tmp = "";
	while(port){

		tmp += port%10 + '0';
		port /= 10;
	}
	reverse(tmp.begin(), tmp.end());
	tmp += "\n";

	buf += tmp;
	
	net_api.Write(1, buf.data(), buf.length());
}

void SelectServer::process_run(){

	if(!init_flag)return;
	
	while(true){

		wait_requests();
		accept_clients();
		deal_clients_messages();

	}

}

void SelectServer::wait_requests(){

	read_sets = all_sets;
	request_nums = select(max_fd+1, &read_sets, NULL, NULL, NULL);

}

void SelectServer::accept_clients(){
		
	if(FD_ISSET(listen_fd, &read_sets)){

		int coord = -1;
		for(int i = 1; i < client_max_num; ++i){
			if(clients[i] == -1){
				coord = i;
				break;
			}
		}

		if(coord == -1){
			return;
		}

		socket_len = sizeof(client_addrs[coord]);
		int client_fd = net_api.Accept(listen_fd, (struct sockaddr *) &client_addrs[coord], &socket_len);
		clients[coord] = client_fd;
		FD_SET(client_fd, &all_sets);

		update_coord(client_fd, coord);

		--request_nums;

		display_socket_addr(coord);
	}
}

void SelectServer::update_coord(const int & client_fd, const int & coord ){

	map_coord[client_fd] = coord;
	max_coord = max_coord > coord ? max_coord : coord;
	max_fd = max_fd > client_fd ? max_fd : client_fd;
}

void SelectServer::close_client(const int & client_fd){

	net_api.Close(client_fd);
	int coord = map_coord[client_fd];
	map_coord[client_fd] = -1;
	clients[coord] = -1;
	FD_CLR(client_fd, &all_sets);
	
}

void SelectServer::deal_clients_messages(){

	if( !request_nums )return;

	char buf[BUFSIZ];
	const string tmp = "\nServer respone: You are very handsome!\n";

	for(int i = 1; i <= max_coord; ++i){

		if(FD_ISSET(clients[i], &read_sets)){

			int client_fd = clients[i];
			int n = net_api.Read(client_fd, buf, sizeof(buf));
			
			if( n <= 0){
				close_client(client_fd);
				display_socket_addr(i);
			}else{

				for(int j = 0; j < n; ++j){

					buf[j] = toupper(buf[j]);
					if(buf[j] == '\0'){

						n = j;
						break;
					}
				}
				int len = tmp.length();
				for(int j = 0; j < len; ++j){

					buf[j+n] = tmp[j];
				}
				buf[n+len] = '\0';
				
				net_api.Write(client_fd, buf, n+len);
				
			}
		}

	}
}














