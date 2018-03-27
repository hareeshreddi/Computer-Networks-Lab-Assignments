#include "header.h"

#define TEMPFILENAME "8876577559"
#define DOWNLOADFILECOPY "copy.txt"

struct PeerNodes
{
    char address[64];
    int port;
};

int ObtainFile(int clientSock,char * filename){
    char buffer[1024];
    int recvMsgSize,sendMsgSize;

    if((recvMsgSize = read(clientSock,buffer,sizeof(buffer) - 1)) < 0){
        printf("read() error nodes list size");
        exit(EXIT_FAILURE);
    }

    SendAck(clientSock,"client");
    FILE* fp = fopen(filename,"w");
    if(fp == NULL){
        return -1;
    }
    struct Container* packet = packet_parser(buffer);
    int buffer_size = atoi(getfieldvalue(packet,"filesize"));
    int remain_data = buffer_size;

    memset(buffer,'\0',sizeof(buffer));
    while((recvMsgSize = read(clientSock,buffer,sizeof(buffer) - 1)) > 0 && remain_data>0){
        fprintf(fp, "%s", buffer);
        remain_data -= recvMsgSize;
        // printf("Receive %d bytes and remaining :- %d bytes\n", recvMsgSize, remain_data);
        
        memset(buffer,'\0',sizeof(buffer));
        SendAck(clientSock,"client");
    }
    fclose(fp);
    return 0;
}

struct PeerNodes* getnodeslist(int clientSock){
    char address[64];
    char port[16];
    int recvMsgSize,sendMsgSize;
    struct PeerNodes* nodeslist = (struct PeerNodes*)malloc(sizeof(struct PeerNodes)*128);

    if(ObtainFile(clientSock,TEMPFILENAME) < 0){
        printf("ObtainFile() error while fetching peer nodes list");
        exit(EXIT_FAILURE);
    }
    
    FILE* fp = fopen(TEMPFILENAME,"r");

    int count = 1;
    char c;
    c = getc(fp);
    while(c!=EOF){
        ungetc(c,fp);

        fscanf(fp,"%s",address);
        fscanf(fp,"%s",port);
        strcpy(nodeslist[count].address,address);
        nodeslist[count++].port = atoi(port);
        c = getc(fp);
    }
    remove(TEMPFILENAME);
    memset(nodeslist[0].address,'\0',sizeof(nodeslist[0].address));
    nodeslist[0].port = count;
    return nodeslist;
}

int ObtainFile_(int clientSock,char * filename){
    char buffer[1024];
    int recvMsgSize,sendMsgSize;

    if((recvMsgSize = read(clientSock,buffer,sizeof(buffer) - 1)) < 0){
        return -1;
    }

    SendAck(clientSock,"client");
    FILE* fp = fopen(filename,"w");
    if(fp == NULL){
        return -1;
    }
    struct Container* packet = packet_parser(buffer);
    int buffer_size = atoi(getfieldvalue(packet,"filesize"));
    int remain_data = buffer_size;

    memset(buffer,'\0',sizeof(buffer));
    while((recvMsgSize = read(clientSock,buffer,sizeof(buffer) - 1)) > 0 && remain_data>0){
        fprintf(fp, "%s", buffer);
        remain_data -= recvMsgSize;
        // printf("Receive %d bytes and remaining :- %d bytes\n", recvMsgSize, remain_data);
        
        memset(buffer,'\0',sizeof(buffer));
        SendAck(clientSock,"client");
    }
    fclose(fp);
    return 0;
}
void clientUtil(struct PeerNodes*);
int connectnode(char *,int,char*);

int main(int argc,char** argv)
{
    if(argc!=3){
        printf("<executable code> <Server IP Address> <Server Port number>");
        exit(EXIT_FAILURE);
    }

    int clientSock;
    int servPort = atoi(argv[2]);
    char* servIP = argv[1];

    if((clientSock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    	printf("socket() error");
        exit(EXIT_FAILURE);
    } 

    struct sockaddr_in servAddr;
    memset(&servAddr, '\0', sizeof(servAddr)); 

    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(servPort);
    servAddr.sin_addr.s_addr = inet_addr(servIP);

    if( connect(clientSock, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0){
       printf("connect() failed");
       exit(EXIT_FAILURE);
    }
    else{
        printf("Connected to server with address %s\n",servIP);
    }

    char buffer[1024];
    int recvMsgSize,sendMsgSize;
    
    strcpy(buffer,"request:client");

    if((sendMsgSize = write(clientSock,buffer,strlen(buffer))) < 0){
    	printf("write() error");
        exit(EXIT_FAILURE);
    }
    

    memset(buffer,'\0',sizeof(buffer));
    if((recvMsgSize = read(clientSock,buffer,sizeof(buffer) - 1)) < 0){
    	printf("read() error");
        exit(EXIT_FAILURE);
    }
    
    SendAck(clientSock,"client");

    struct Container *packet = packet_parser(buffer);
    printf("Peer nodes list fetched from server\n");

    if(strcmp(getfieldvalue(packet,"status"),"connected")!=0){
        printf("Unexpected response from server");
        exit(EXIT_FAILURE);
    }
    
    struct PeerNodes * nodeslist = getnodeslist(clientSock);
    
    if(shutdown(clientSock,0)< 0){
        printf("shutdown() error closing connection with server.");
        exit(EXIT_FAILURE);
    }
    else{
        printf("Connection with server closed gracefully\n");
    }
    
    char c[10] = "y";
    while(1){
        
        if(strcmp("Y",c)==0 || strcmp("y",c)==0){
            clientUtil(nodeslist);
        }
        else if(strcmp("N",c)==0 || strcmp("n",c)==0){
            printf("Client exiting......\n");    
            break;
        }
        else{
            printf("Unknown option\n");
        }
        printf("Press (N\\n) to quit or (Y\\y) to continue\n");
        scanf("%s",c);
    }

    return 0;
}

void clientUtil(struct PeerNodes * nodeslist){

    int i,num_nodes=nodeslist[0].port;

    char filename[64];
    printf("File to download : ");
    scanf("%s",filename);

    int flag = 0;
    for(i = 1;i<num_nodes-1;i++){
        // printf("%s %d\n",nodeslist[i].address,nodeslist[i].port);
        if(connectnode(nodeslist[i].address,nodeslist[i].port,filename) == 0){
            flag = 1;
            break;
        }
    }

    if(flag == 1){
        printf("File '%s' found on peer %s:%d\n",filename,nodeslist[i].address,nodeslist[i].port);
    }
    else{
        printf("File '%s' not found on any peer node\n",filename);
    }
}

int connectnode(char* servIP,int servPort,char* filename){
    
    int clientSock;
    if((clientSock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        return -1;
    } 

    struct sockaddr_in servAddr;
    memset(&servAddr, '\0', sizeof(servAddr)); 

    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(servPort);
    servAddr.sin_addr.s_addr = inet_addr(servIP);

    if( connect(clientSock, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0){
        close(clientSock);
        return -1;
    }
    else{
        printf("Connected to peer %s:%d\n",servIP,servPort);
    }

    char buffer[1024];
    int recvMsgSize,sendMsgSize;
    
    strcpy(buffer,"request:client");

    if((sendMsgSize = write(clientSock,buffer,strlen(buffer))) < 0){
        close(clientSock);
        return -1;
    }

    
    if(checkack(clientSock) < 0){
        close(clientSock);
        return -1;
    }

    sprintf(buffer,"request:client\nfilename:%s",filename);
    if((sendMsgSize = write(clientSock,buffer,strlen(buffer))) < 0){
        close(clientSock);
        return -1;
    }
    
    
    if(checkack(clientSock) < 0){
        close(clientSock);
        return -1;
    }

    memset(buffer,'\0',sizeof(buffer));
    if((recvMsgSize = read(clientSock,buffer,sizeof(buffer) - 1)) < 0){
        close(clientSock);
        return -1;
    }
    SendAck(clientSock,"client");

    if(strcmp(getfieldvalue(packet_parser(buffer),"file"),"yes") ==0){
        if(ObtainFile_(clientSock,TEMPFILENAME) < 0){
            close(clientSock);
            return -1;
        }
        FILE* fp1 = fopen(TEMPFILENAME,"r");
        FILE* fp2 = fopen(DOWNLOADFILECOPY,"w");
        // printf("File '%s' found on peer %s:%d\n",filename,servIP,servPort);
        char c;
        c = getc(fp1);
        while(c!=EOF){
            ungetc(c,fp1);
            memset(buffer,'\0',sizeof(buffer));
            fgets(buffer,sizeof(buffer)-1,fp1);
            printf("%s", buffer);
            fprintf(fp2,"%s\n",buffer);
            c = getc(fp1);
        }
        printf("\n");
        fclose(fp1);
        fclose(fp2);
        printf("Fetched file in %s\n", DOWNLOADFILECOPY);
        remove(TEMPFILENAME);
        printf("Closing connection with peer %s:%d gracefully\n", servIP,servPort);
        shutdown(clientSock,0);
        return 0;
    }
    else{
        // printf("File '%s' not found on peer %s:%d\n",filename,servIP,servPort);
        printf("Closing connection with peer %s:%d gracefully\n", servIP,servPort);
        shutdown(clientSock,0);
        return -1;
    }   
}