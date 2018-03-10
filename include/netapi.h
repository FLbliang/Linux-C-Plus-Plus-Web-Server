#ifndef __NETAPI_H__
#define __NETAPI_H__


class NetApi{
	
public:
		int Socket(int domain, int type, int protocol);
		int Bind(int sockfd, const struct sockaddr * addr, socklen_t addrlen);
		int Listen(int sockfd, int backlog);
		int Accept(int sockfd, struct sockaddr * addr, socklen_t * addrlen);
		int Connect(int sockfd, const struct sockaddr * addr, socklen_t addrlen);
		ssize_t Read(int fd, void * ptr, size_t nbytes);
		ssize_t Write(int fd, const void * ptr, size_t nbytes);
		int Close(int fd);
		void Perror(const char * str);
};


#endif
