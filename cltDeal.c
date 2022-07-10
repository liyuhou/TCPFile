#include "cltDeal.h"

char server_ip[16];
char server_port[6];
char rootDir[] = "/home/liyu/vsPro_exercise/TCPfile/clientRootDir";

struct sockaddr_in server_addr;
int sock,r;

int client_init(char *server_ip, char *server_port)
{
    printf("===== client init ======\n");
    if(chdir(rootDir) < 0){
        printf("enter client root directory fail\n");
        exit(-1);
    }
    else{
        printf("enter client root directory success\n");
    }
    printf("1: create a TCP socket\n");
    sock=socket(AF_INET,SOCK_STREAM,0);
    if(sock<0){
        printf("socket call failed\n");
        exit(1);
    }

    printf("2: fill server_addr with server's IP and PORT#\n");
    
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(atoi(server_port));
    inet_aton(server_ip,&server_addr.sin_addr);
  
    printf("3: connecting to server ...\n");
    r=connect(sock,(struct sockaddr*)&server_addr,sizeof(server_addr));
    if(r<0){
        printf("connect failed\n");
        exit(3);
    }

    printf("4: connected OK to\n");
    printf("------------------------------------------\n");
    printf("Server hostname=%s, PORT=%d\n", SERVER_HOST, atoi(server_port));
    printf("------------------------------------------\n");
    printf("============ init done ============\n");
    printf("********************************** menu **********************************\n");
    printf("* get put ls cd pwd mkdir rmdir rm exit(for end client program) *\n");
    printf("**************************************************************************\n");
}