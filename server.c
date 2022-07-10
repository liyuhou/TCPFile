#include "svrDeal.h"

int main()
{
    server_init();
    while(1){
        printf("server: accepting new connection ...\n");
        len=sizeof(client_addr);
        csock = accept(mysock,(struct sockaddr*)&client_addr,&len);
        if(csock<0){
            printf("server: accept error\n");
            exit(1);
        }
        printf("server: accepted a client connection from\n");
        printf("-----------------------------------------\n");
        printf("Client: IP=%s, port=%d\n",inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
        printf("---------------------------------------\n");
        while(1){
            memset(buff,0,sizeof(buff));
            n=recv(csock, buff, MAX, 0);
            if(n<=0){
                if(errno!=EINTR){
                    printf("server: client died, server loops\n");
                    close(csock);
                    break;
                }
                else{
                    //never be here
                }
                
            }
            printf("server: read n=%dbyte; line=%s\n",n,buff);
            //parse cmd
            char* command=strtok(buff," "); 
            if(0 == strcmp(command, "ls")){
                deal_ls();
            }
            else if(0 == strcmp(command, "get")){
                deal_get();
            }
            else if(0 == strcmp(command, "cd")){
                deal_cd();
            }
            else if(0 == strcmp(command, "pwd")){
                deal_pwd();
            }
            else if(0 == strcmp(command, "mkdir")){
                deal_mkdir();
            }
            else if(0 == strcmp(command, "rmdir")){
                deal_rmdir();
            }
            else if(0 == strcmp(command, "rm")){
                deal_rm();
            }
            else if(0 == strcmp(command, "put")){
                deal_put();
            }
            else{
                printf("cmd %s not supported yet\n", command);
            }

            //n=send(csock,line,strlen(line),0);
            //printf("server: wrote n=%dbytes; ECHO=%s\n",n,line);
            printf("server: ready for next request\n");
        }
    }
}