#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>

#include "netapi.h"


void NetApi::Perror(const char * str){

	perror(str);
	exit(-1);

}

int NetApi::Socket(int domain, int type, int protocol){

	int socket_fid = socket(domain, type, protocol);

	if(socket_fid < 0){
		Perror("create socket error");
	}

	return socket_fid;

}


int NetApi::Bind(int sockfd, const struct sockaddr * addr, socklen_t addrlen){

	int flag = bind(sockfd, addr, addrlen);

	if(flag < 0){
		Perror("bind error");
	}

	return flag;


}


int NetApi::Listen(int sockfd, int backlog){

	int flag = listen(sockfd, backlog);
	if(flag < 0){
		Perror("listen error");
	}
	
	return flag;

}


int NetApi::Accept(int sockfd, struct sockaddr * addr, socklen_t * addrlen){

	int flag;
	while((flag = accept(sockfd, addr, addrlen)) < 0){

		if(errno == ECONNABORTED || errno == EINTR){
			continue;
		}else{
			Perror("accept error");
		}	

	}

	return flag;	

}

int NetApi::Connect(int sockfd, const struct sockaddr * addr, socklen_t addrlen){

	int flag = connect(sockfd, addr, addrlen);
	if(flag < 0){

		Perror("connect error");
	}
	
	return flag;

}

int NetApi::Close(int fd){

	int n;

	if((n = close(fd)) == -1){
		printf("close error\n");
	}

	return n;
}


ssize_t NetApi::Read(int fd, void * ptr, size_t nbytes){


	int n;
	while((n = read(fd, ptr, nbytes)) == -1){

		if(errno == EINTR){
			continue;
		}

		return -1;
	}

	return n;
}


ssize_t NetApi::Write(int fd, const void * ptr, size_t nbytes){

	int n;
	while((n = write(fd, ptr, nbytes)) == -1){
			
		if(errno == EINTR){
			continue;
		}

		return -1;
	}

	return n;
}




















