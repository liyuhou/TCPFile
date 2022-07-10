#include "cltDeal.h"

int main(char argc, char** argv){
    int n;
    char line[MAX], sendMessage[MAX], ans[MAX+1];
    if(argc!=3){
        printf("usage: ./client server_ip server_port\n");
        exit(0);
    }
    strcpy(server_ip, argv[1]);
    strcpy(server_port, argv[2]);
    client_init(server_ip,server_port);
    printf("***** processing loop *********\n");
    while(1){
        printf("input a line : ");
        bzero(line,MAX);
        fgets(line, MAX, stdin);
        if(line[0]=='\n') continue;
        line[strlen(line)-1]=0;
        if(line[0]==0) exit(0);
        strcpy(sendMessage, line);
        char* cmd = strtok(line, " ");
        if(0 == strcmp(cmd,"exit")){
            close(sock);
            exit(0);
        }
        else {
            n=send(sock, sendMessage, strlen(sendMessage),0);
        }
        //receive data
        memset(ans,0,sizeof(ans));
        if(0 == strcmp(cmd,"ls")){
            n=recv(sock, ans, MAX, 0);
            int fileSize = atoi(ans), count=0;
            printf("size of return message is %d\n", fileSize);
            while(count<fileSize){
                memset(ans,0,MAX+1);//n not ans, for efficiency
                n=recv(sock, ans, MAX, 0);
                printf("%s",ans);
                count += n;
            }
            printf("done\n");
        }
        else if(0 == strcmp(cmd,"get")){
            char* fileName = strtok(NULL, " ");
            n=recv(sock, ans, MAX, 0);
            int fileSize = atoi(ans), count=0;
            if(0 == fileSize){
                printf("get file %s fail\n", fileName);
            }
            else{
                printf("size of return file is %d\n", fileSize);
                int fd = open(fileName, O_RDWR | O_CREAT, 0664);
                while(count<fileSize){
                    memset(ans,0,MAX);
                    n=recv(sock, ans, MAX, 0);
                    write(fd, ans, n);
                    count += n;
                    printf("n=%d count=%d\n",n,count);
                }
                close(fd);
                printf("done\n");
            } 
        }
        else if(0 == strcmp(cmd,"cd")){
            n=recv(sock, ans, MAX, 0);
            printf("%s\n", ans);
        }
        else if(0 == strcmp(cmd,"pwd")){
            n=recv(sock, ans, MAX, 0);
            printf("%s\n", ans);
        }
        else if(0 == strcmp(cmd,"mkdir")){
            n=recv(sock, ans, MAX, 0);
            printf("%s\n", ans);
        }
        else if(0 == strcmp(cmd,"rmdir")){
            n=recv(sock, ans, MAX, 0);
            printf("%s\n", ans);
        }
        else if(0 == strcmp(cmd,"rm")){
            n=recv(sock, ans, MAX, 0);
            printf("%s\n", ans);
        }
        else if(0 == strcmp(cmd,"put")){
            char* filePath = strtok(NULL, " ");
            char buff[MAX]={0};
            int fd = open(filePath, O_RDONLY);
            if(fd < 0){
                printf("file does't exist\n");
                return -1;
            }
            else{
                struct stat mystat;
                if(stat(filePath, &mystat) < 0){
                    printf("file does't exist\n");
                    return -1;
                }
                else{
                    int size = mystat.st_size;
                    sprintf(buff, "%d", size);
                    send(sock, buff, MAX, 0);
                    memset(buff,0,sizeof(buff));
                    n=recv(sock, ans, MAX, 0);
                    if(strcmp(ans,"ok") != 0){
                        printf("put fail\n");
                        return -1;
                    }
                    while( (n = read(fd, buff, MAX)) > 0){
                        //printf("n=%d\n",n);
                        send(sock, buff, n, 0);
                    }
                    close(fd);
                }
            }
            printf("client send done\n");
            n=recv(sock, ans, MAX, 0);
            printf("%s\n", ans);
        }
        else{
            printf("sorry, not supported yet\n");
        }
        //n=recv(sock, ans, MAX, 0);
        //printf("client: read n=%d bytes; echo=%s\n",n,ans);
    }
}