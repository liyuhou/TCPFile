#ifndef _CLTDEAL_H_
#define _CLTDEAL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define MAX 256
#define SERVER_HOST "hou"

extern char server_ip[16];
extern char server_port[6];
extern char rootDir[];

extern struct sockaddr_in server_addr;
extern int sock,r;

int client_init(char *server_ip, char *server_port);

#endif