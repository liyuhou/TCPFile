#include "svrDeal.h"

char *t1 = "xwrxwrxwr-------";
char *t2 = "----------------";
int fd=0, oldStdFd=0;
struct sockaddr_in  server_addr, actual_server_addr, client_addr;
int mysock,csock;
int r,len,n;
char buff[MAX];
char rootDir[] = "/home/liyu/vsPro_exercise/TCPfile/serverRootDir";//modify this for transplantation

int virtualDir2ActualDir(char* virtual, char* actual, int len_actual)
{
    if('/' == virtual[0]){
        strcpy(actual, rootDir);
        strcat(actual, virtual);
    }
    else{
        getcwd(actual, len_actual);
        strcat(actual, "/");
        strcat(actual, virtual);   
    }
}

int actualDir2VirtualDir(char* actual, char* virtual, int len_actual)
{
    int r=strlen(rootDir);
    virtual[0]='/';
    for(int i=r;i<len_actual;i++){
        virtual[i-r] = actual[i];
    }
}

int server_init(){
    printf("========== server init ===========\n");
    if(chdir(rootDir) < 0){
        printf("enter root directory fail\n");
        exit(-1);
    }
    else{
        printf("enter root directory success\n");
    }
    printf("1: create a TCP STREAM socket\n");
    mysock = socket(AF_INET,SOCK_STREAM,0);
    if(mysock<0) {
        printf("socket call failed\n");
        exit(1);
    }
    printf("2: fill server_addr with host IP and PORT# info\n");
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=RANDOM_SERVER_PORT;
    server_addr.sin_addr.s_addr=inet_addr(SERVER_IP);

    printf("3: bind socket to server address\n");
    r=bind(mysock,(struct sockaddr*)&server_addr,sizeof(server_addr));
    if(r<0){
        printf("bind failed\n");
        exit(3);
    }
    len=sizeof(actual_server_addr);//necessary
    r=getsockname(mysock, (struct sockaddr *)&actual_server_addr, &len);
    //printf("r=%d\n",r);
    printf("hostname = %s, ip=%s, port = %d\n",SERVER_HOST,inet_ntoa(actual_server_addr.sin_addr),ntohs(actual_server_addr.sin_port));
    printf("4: server is listening ...\n");
    listen(mysock,5);//queue length = 5
    printf("=========== init done ===========\n");
    
}

int deal_ls()
{
    struct stat mystat, *sp=&mystat;
    oldStdFd = dup(1);//1
    int r;
    char relative[1024];
    char* filename = strtok(NULL, " ");
    if(filename!=NULL){
        virtualDir2ActualDir(filename, relative, sizeof(relative));
    }
    else{
        getcwd(relative, 1024);
    }
    
    if(r = lstat(relative, sp) < 0){
        printf("no such file %s\n", filename);
        return -1;
    }
    fd = open("/home/liyu/vsPro_exercise/TCPfile/tmpFile", O_CREAT | O_RDWR, 0664);
    dup2(fd, 1);//stdout 
    if(S_ISDIR(sp->st_mode))
        ls_dir(relative);
    else 
        ls_file(relative);

    dup2(oldStdFd, 1);
    struct stat st;
    if(stat("/home/liyu/vsPro_exercise/TCPfile/tmpFile", &st) < 0){
        printf("ls:\tread stat fail\n");
        return -1;
    }
    int size = st.st_size;
    sprintf(buff, "%d", size);
    send(csock, buff, MAX,0);
    lseek(fd, 0, SEEK_SET);
    while(read(fd,buff,MAX)){
        send(csock, buff, MAX,0);
        memset(buff, 0, sizeof(buff));
    }
    printf("ls:\tdone\n");
    close(fd);
    if(remove("/home/liyu/vsPro_exercise/TCPfile/tmpFile") != 0){
        printf("delete temp file fail\n");
    }
}

int ls_file(char *fname)
{
    struct stat fstat, *sp;
    int r, i;
    char ftime[64], linkname[64];
    sp = &fstat;
    if((r = lstat(fname, &fstat)) < 0){
        printf("can't stat %s\n",fname);
        exit(1);
    }
    if((sp->st_mode & 0xF000) == 0x8000)
        printf("-");
    if((sp->st_mode & 0xF000) == 0x4000)
        printf("d");
    if((sp->st_mode & 0xF000) == 0xA000)
        printf("l");
    for(int i=8;i>=0;i--){
        if(sp->st_mode &(1 << i))
            printf("%c",t1[i]);
        else 
            printf("%c",t2[i]);
    }
    printf("%4ld ", sp->st_nlink);
    printf("%4u ", sp->st_gid);
    printf("%4u ", sp->st_uid);
    printf("%8ld ", sp->st_size);
    strcpy(ftime, ctime(&sp->st_ctim.tv_sec));
    ftime[strlen(ftime)-1] = 0;//kill \n
    printf("%s ", ftime);
    printf("%s", basename(fname));
    if((sp->st_mode & 0xF000) == 0xA000){
        readlink(fname, linkname, 63);
        printf(" -> %s", linkname);
    }
    printf("\n");
}

int ls_dir(char *dname)
{
    struct dirent *ep;
    
    DIR *dp = opendir(dname);
    while(ep = readdir(dp)){
        if(0 == strcmp(ep->d_name, ".") || 0 == strcmp(ep->d_name, "..")) continue;
        ls_file(ep->d_name);
    }
    printf("\n");
}

int deal_get()
{
    char* fileName = strtok(NULL, " ");
    char path[1024];
    virtualDir2ActualDir(fileName, path, 1024);
    int fd = open(path, O_RDONLY);
    if(fd < 0){
        printf("file does't exist: send '0' to client\n");
        send(csock, "0", MAX,0);
    }
    else{
        struct stat mystat;
        if(stat(path, &mystat) < 0){
            printf("file does't exist: send '0' to client\n");
            send(csock, "0", MAX,0);
        }
        else{
            int size = mystat.st_size;
            sprintf(buff, "%d", size);
            send(csock, buff, MAX, 0);
            while( (n = read(fd, buff, MAX)) > 0){
                printf("n=%d\n",n);
                send(csock, buff, n, 0);
            }
            close(fd);
        }
    }
    printf("get:\tdone\n");
}

int deal_cd()
{
    char* virtualDirName = strtok(NULL, " ");
    char actualDirName[1024];
    virtualDir2ActualDir(virtualDirName, actualDirName, 1024);
    char clientDirName[1024], absolute_actualDirName[1024];
    dealRelativePath(actualDirName, absolute_actualDirName);
    if(strncmp(rootDir, absolute_actualDirName, strlen(rootDir)) != 0){//strlen(absolute_actualDirName) < strlen(rootDir) || 
        printf("dir overstep the boundary\n");
        send(csock,"dir overstep the boundary",MAX,0);
    }
    else{
        if(chdir(absolute_actualDirName) < 0){
            printf("enter path %s fail\n", absolute_actualDirName);
            send(csock,"enter path fail, check the directory",MAX,0);
        }
        else{
            printf("current dir: %s\n", absolute_actualDirName);
            char absolute_virtualDirName[1024]={0};
            for(int i=strlen(rootDir);i<strlen(absolute_actualDirName);i++){
                absolute_virtualDirName[i-strlen(rootDir)] = absolute_actualDirName[i];
            }
            if(absolute_virtualDirName[0] == 0){
                absolute_virtualDirName[0] = '/';
            }
            //strcpy(clientDirName,absolute_actualDirName+strlen(rootDir));
            send(csock,absolute_virtualDirName,MAX,0);
        }
    }
}

int dealRelativePath(char* relative, char* absolute)
{
    int i=0, j=0;
    while(i<strlen(relative)){
        if(relative[i] != '.'){
            absolute[j++] = relative[i++];
        }
        else{
            if(i+1 < strlen(relative)){
                if(relative[i+1] == '.'){
                    i+=2;j-=2;
                    while(absolute[j] != '/'){
                        j--;
                        if(j<0){
                            printf("dir is illegal\n");
                            strcpy(absolute, rootDir);
                            return -1;
                        }
                    }
                    absolute[j++]=0;
                }
                else{
                    j--; i++;
                    if(j<0){
                        printf("dir is illegal\n");
                        strcpy(absolute, rootDir);
                        return -1;
                    }
                    absolute[j]=0;
                }
            }
            else{
                j--; i++;
                if(j<0){
                    printf("dir is illegal\n");
                    strcpy(absolute, rootDir);
                    return -1;
                }
                absolute[j]=0;
            }
        }
    }
    if(absolute[0] == 0){
        strcpy(absolute, rootDir);
        absolute[1]=0;
    }
}

int deal_pwd()
{
    char pwd_actual[1024]={0}, pwd_virtual[1024]={0};
    getcwd(pwd_actual,1024);
    actualDir2VirtualDir(pwd_actual, pwd_virtual, strlen(pwd_actual));
    send(csock,pwd_virtual,MAX,0);
}

int deal_mkdir()
{
    char* virtualDir = strtok(NULL, " ");
    char actualDir[1024];
    virtualDir2ActualDir(virtualDir, actualDir, strlen(actualDir));
    if(mkdir(actualDir, 0755) < 0){
        printf("mkdir %s fail\n", actualDir);
    }
    else{
        send(csock,"done",MAX,0);
    }
}

int deal_rmdir()
{
    char* virtualDir = strtok(NULL, " ");
    char actualDir[1024];
    virtualDir2ActualDir(virtualDir, actualDir, strlen(actualDir));
    if(rmdir(actualDir) < 0){
        printf("rmdir %s fail\n", actualDir);
    }
    else{
        send(csock,"done",MAX,0);
    }
}

int deal_rm()
{
    char* virtualFile = strtok(NULL, " ");
    char actualFile[1024];
    virtualDir2ActualDir(virtualFile, actualFile, strlen(actualFile));
    if(remove(actualFile) < 0){
        printf("remove %s fail\n", actualFile);
    }
    else{
        send(csock,"done",MAX,0);
    }    
}

int deal_put()
{
    char* fileName = strtok(NULL, " ");
    char fileName_backup[256]={0};
    strcpy(fileName_backup, fileName);
    printf("file is %s\n", fileName);
    memset(buff,0,sizeof(buff));//this 
    n=recv(csock, buff, MAX, 0);
    int fileSize = atoi(buff), count=0;
    if(0 == fileSize){
        printf("receive file %s fail\n", fileName);
    }
    else{
        send(csock, "ok", MAX, 0);
        printf("size of return file is %d\n", fileSize);
        int fd = open(fileName_backup,  O_CREAT | O_RDWR, 0664);
        if(fd < 0){
            printf("open file %s fail\n", fileName);
            printf("%s\n",strerror(errno));
            send(csock,"server create file fail\n",MAX,0);
            return -1;
        }
        while(count<fileSize){
            memset(buff,0,MAX);
            n=recv(csock, buff, MAX, 0);
            write(fd, buff, n);
            count += n;
            printf("n=%d count=%d\n",n,count);
        }
        close(fd);
        send(csock,"server done",MAX,0);
    } 
}