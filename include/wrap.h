#ifndef WRAP_H
#define WRAP_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

int Socket(int domain, int type, int protocol);
int Bind(int sockfd, const struct sockaddr * addr, socklen_t addrlen);
int Listen(int sockfd, int backlog);
int Accept(int sockfd, struct sockaddr * addr, socklen_t * addrlen);
int Connect(int sockfd, const struct sockaddr * addr, socklen_t addrlen);
void Perror(const char * str);




#endif
