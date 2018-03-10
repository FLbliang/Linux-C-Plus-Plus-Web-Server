#include "wrap.h"

#include <time.h>
#include <string>

using std::string;

string get_time(){

	struct tm * c_time;
	char buf[666];
	time_t t;
	time(&t);
	c_time = localtime(&t);
	strftime(buf, sizeof(buf), "%F %T", c_time);

	return string(buf);
}
	

void Perror(const char * str){

	perror(str);
	exit(-1);

}

int Socket(int domain, int type, int protocol){

	int socket_fid = socket(domain, type, protocol);

	if(socket_fid < 0){
		Perror("create socket error");
	}

	return socket_fid;

}


int Bind(int sockfd, const struct sockaddr * addr, socklen_t addrlen){

	int flag = bind(sockfd, addr, addrlen);

	if(flag < 0){
		Perror("bind error");
	}

	return flag;


}


int Listen(int sockfd, int backlog){

	int flag = listen(sockfd, backlog);
	if(flag < 0){
		Perror("listen error");
	}
	
	return flag;

}


int Accept(int sockfd, struct sockaddr * addr, socklen_t * addrlen){

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

int Connect(int sockfd, const struct sockaddr * addr, socklen_t addrlen){

	int flag = connect(sockfd, addr, addrlen);
	if(flag < 0){

		Perror("connect error");
	}
	
	return flag;

}























