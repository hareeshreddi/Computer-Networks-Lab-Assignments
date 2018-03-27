#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <math.h>
#include <assert.h>
#include <limits.h>
#include <signal.h>

#define BACKLOG 5

struct Container
{
    char* name;
    char* value;
};

char * getfieldvalue(struct Container* packet,char *field){
    int n = atoi(packet[0].value);
    while(n>0 && strcmp(packet[--n].name,field)!=0);
    if(n<=0){
        return NULL;
    }
    return packet[n].value;
}

struct Container * packet_parser(char *buffer){
    struct Container* packet_fields = (struct Container*)malloc(sizeof(struct Container)*32);
    int fields_num = 1;
    char* temp = strtok(buffer,"\n");
    while(temp){
        packet_fields[fields_num++].name = temp;
        temp = strtok(NULL,"\n");
    }

    char* fields_num_string = (char*)malloc(sizeof(char)*32);
    sprintf(fields_num_string,"%d",fields_num);

    packet_fields[0].name = NULL;
    packet_fields[0].value = fields_num_string;

    int i;
    for(i = 1;i<fields_num;i++){
        temp = strtok(packet_fields[i].name,":");
        packet_fields[i].name = temp;
        temp = strtok(NULL,":");
        packet_fields[i].value = temp;
    }

    return packet_fields;
}

int SendAck(int clientSock,char *prefix){
    char buffer[1024];
    int recvMsgSize,sendMsgSize;
    if(strcmp(prefix,"client") == 0)    strcpy(buffer,"response:client\nack:1");
    else    strcpy(buffer,"response:node\nack:1");
    if((sendMsgSize = write(clientSock,buffer,strlen(buffer))) < 0){
        if(strcmp(prefix,"node") == 0)
            printf("request write() ack error");
        else
            printf("write() ack send error");
        exit(EXIT_FAILURE);
    }
}

int checkack(int clientSock){
    char buffer[1024];
    int recvMsgSize;
    memset(buffer,'\0',sizeof(buffer));
    if((recvMsgSize = read(clientSock,buffer,sizeof(buffer) - 1)) < 0){
        return -1;
    }
    
    if(strcmp(getfieldvalue(packet_parser(buffer),"ack"),"1") != 0){
        return -1;
    }
    return 0;
}

int KeepFile(int clientSock,const char * filename){
    int recvMsgSize, sendMsgSize;
    // FILE *fp = fopen(filename,"r");
    // if(fp==NULL){
    //     return 0;
    // }
    // fclose(fp);
    char response[128];
    int fd;
    struct stat file_stat;
    if((fd = open(filename,O_RDONLY)) < 0){
        // printf("Error opening file");
        strcpy(response,"response:node\nfile:#####");
        if((sendMsgSize = write(clientSock,response,strlen(response))) < 0){
            printf("Could not send node server response\n");
            exit(EXIT_FAILURE);
        }
    
        if(checkack(clientSock) < 0){
            printf("No ack recieved");
            exit(EXIT_FAILURE);
        }
        return -1;
    }

    strcpy(response,"response:node\nfile:yes");
    if((sendMsgSize = write(clientSock,response,strlen(response))) < 0){
        printf("write () error");
    }

    if(checkack(clientSock) < 0){
        printf("No ack recieved");
        exit(EXIT_FAILURE);
    }


    if(fstat(fd,&file_stat)<0){
        printf("fstsat() error");
        exit(EXIT_FAILURE);
    }
    char buffer[1024];
    sprintf(buffer,"response:node\nfile:%s\nfilesize:%d",filename,(int)file_stat.st_size);

    if((sendMsgSize = write(clientSock,buffer,strlen(buffer))) < 0){
        printf("write() error while sending filesize response");
        exit(EXIT_FAILURE);
    }

    if(checkack(clientSock) < 0){
        printf("No ack recieved");
        exit(EXIT_FAILURE);
    }
    
    off_t offset = 0,remain_data = file_stat.st_size;
    float z = remain_data/1024.0;
    printf("File info - Filename : %s, Size : %.3f KB\n",filename,z);

    while((sendMsgSize = sendfile(clientSock,fd,&offset,BUFSIZ)) > 0 && remain_data){
        // printf("1. Server sent %d bytes from file's data and remaining data = %d\n", sendMsgSize, (int)remain_data);
        remain_data -= sendMsgSize;
        // printf("2. Server sent %d bytes from file's data and remaining data = %d\n", sendMsgSize, (int)remain_data);
        
        if(checkack(clientSock) < 0){
            printf("No ack recieved");
            exit(EXIT_FAILURE);
        }
    }
    return 0;
}


int KeepFile1(int clientSock,char * filename){
    int recvMsgSize, sendMsgSize;
    
    int fd;
    struct stat file_stat;
    if((fd = open(filename,O_RDONLY)) < 0){
        return -1;
    }

    if(fstat(fd,&file_stat)<0){
        printf("fstsat() error");
        exit(EXIT_FAILURE);
    }
    char buffer[1024];
    sprintf(buffer,"response:server\nfile:%s\nfilesize:%d",filename,(int)file_stat.st_size);
    if((sendMsgSize = write(clientSock,buffer,strlen(buffer))) < 0){
        printf("write() error while sending filesize");
        exit(EXIT_FAILURE);
    }

    
    memset(buffer,'\0',sizeof(buffer));

    if((recvMsgSize = read(clientSock,buffer,sizeof(buffer) - 1)) < 0){
        printf("read() error");
        exit(EXIT_FAILURE);
    }

    struct Container* packet = packet_parser(buffer);
    int n = atoi(packet[0].value);
    while(n>0 && strcmp(packet[--n].name,"ack")!=0);

    if(n<=0){
        printf("No ack recieved");
        exit(EXIT_FAILURE);
    }
    off_t offset = 0,remain_data = file_stat.st_size;

    while((sendMsgSize = sendfile(clientSock,fd,&offset,BUFSIZ)) > 0 && remain_data){
        // printf("1. Server sent %d bytes from file's data and remaining data = %d\n", sendMsgSize, (int)remain_data);
        remain_data -= sendMsgSize;
        // printf("2. Server sent %d bytes from file's data and remaining data = %d\n", sendMsgSize, (int)remain_data);
        
        memset(buffer,'\0',sizeof(buffer));
        if((recvMsgSize = read(clientSock,buffer,sizeof(buffer) - 1)) < 0){
            printf("read() error");
            exit(EXIT_FAILURE);
        }
        if(strcmp(getfieldvalue(packet_parser(buffer),"ack"),"1") != 0){
            printf("No ack recieved");
            exit(EXIT_FAILURE);
        }
    }
    return 0;
}