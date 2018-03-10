#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "daemond.h"

Daemond::Daemond(const char * dir, const char * file_name){

	this->dir = dir;
	this->file_name = file_name;

}

Daemond::Daemond(){
	
	dir = NULL;
	file_name = "daemond.log";
}

void Daemond::create_daemond(){

	int pid;
	while((pid = fork()) == -1);

	if(pid > 0){
		sleep(1);
		exit(0);
	}

	setsid();

	if(dir && chdir(dir) == -1){
		perror("chdir error");
		exit(1);
	}

	close(0);
	
	if(open(file_name, O_RDWR | O_APPEND | O_CREAT, 00664) == -1){
		perror("open file error");
		exit(1);
	}

	if(dup2(0, 1) == -1 || dup2(0, 2) == -1){
		perror("dup2 error");
		exit(1);
	}
	

}

