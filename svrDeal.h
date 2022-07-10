#ifndef _SVRDEAL_H_
#define _SVRDEAL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/types.h>
#include <dirent.h>
#include <libgen.h>
#include <fcntl.h>
#include <errno.h>

#define MAX 256
#define SERVER_HOST "hou"
#define SERVER_IP "192.168.6.128"//"39.152.149.145"
#define RANDOM_SERVER_PORT 0

extern char *t1;
extern char *t2;
extern int fd;
extern struct sockaddr_in  server_addr, actual_server_addr, client_addr;
extern int mysock,csock;
extern int r,len,n;
extern char buff[MAX];
extern char rootDir[];

int virtualDir2ActualDir(char* virtual, char* actual, int len_actual);
int actualDir2VirtualDir(char* actual, char* virtual, int len_actual);
int server_init();
int deal_ls();
int ls_file(char *fname);
int ls_dir(char *dname);
int deal_get();
int deal_cd();
int dealRelativePath(char* relative, char* absolute);//deal with . and ..
int deal_pwd();
int deal_mkdir();
int deal_rmdir();
int deal_rm();
int deal_put();

#endif