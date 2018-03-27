#include "header.h"


void servUtil(int clientSock,char *client_IP,int client_Port){
    int recvMsgSize, sendMsgSize;
    char buffer[1024];
    memset(buffer,'\0',sizeof(buffer));

    if(recvMsgSize = read(clientSock,buffer,sizeof(buffer) - 1) < 0){
        printf("read() error");
        exit(EXIT_FAILURE);
    }
    if(strcmp(buffer,"request:client") != 0){
        printf("Unexpected request");
        exit(EXIT_FAILURE);
    }
    SendAck(clientSock,"node");

    memset(buffer,'\0',sizeof(buffer));
    if(recvMsgSize = read(clientSock,buffer,sizeof(buffer) - 1) < 0){
        printf("read() error");
        exit(EXIT_FAILURE);
    }
    SendAck(clientSock,"node");

    char* filefield;
    filefield = getfieldvalue(packet_parser(buffer),"filename");
    printf("Client %s:%d fetch request for file '%s'\n",client_IP,client_Port, filefield);
    if(KeepFile(clientSock,filefield) == 0){
        fprintf(stdout,"File '%s' sent\n", filefield);
    }
    else{
        fprintf(stdout,"File '%s' not found\n", filefield);
    }
}

void startNodeServer(int port){

    int servPort = port;
    int servSock;

    if((servSock=socket(AF_INET,SOCK_STREAM, 0)) < 0){
        printf("socket() failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in servAddr;
    memset((void *)&servAddr,'\0',sizeof(servAddr));

    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(servPort);

    if(bind(servSock,(struct sockaddr*) &servAddr, sizeof(servAddr)) < 0){
        printf("bind() failed");
        exit(EXIT_FAILURE);
    }

    if(listen(servSock,BACKLOG) < 0){
        printf("listen() failed");
        exit(EXIT_FAILURE);
    }
    else{
        printf("Node server listening on port %d\n",servPort);
    }

    while(1){

        int clntLen,clientSock;
        struct sockaddr_in clntAddr;

        clntLen = sizeof(clntAddr);

        int client_Port;
        char client_IP[INET_ADDRSTRLEN];
        if((clientSock=accept(servSock,(struct sockaddr *)&clntAddr,&clntLen)) < 0){
            printf("accept() failed");
            exit(EXIT_FAILURE);
        }
        else{

            memset(client_IP,'\0',sizeof(client_IP));
            if(inet_ntop(AF_INET, &(clntAddr.sin_addr),client_IP,sizeof(client_IP)) == 0){
                printf("inet_ntop() error");
                exit(EXIT_FAILURE);
            }
            client_Port = ntohs(clntAddr.sin_port);
            printf("Client %s:%d accepted\n", client_IP,client_Port);
        }

        int pid = fork();

        if(pid < 0){
            printf("fork() error");
            exit(EXIT_FAILURE);
        }

        if(pid == 0){
            close(servSock);
            servUtil(clientSock,client_IP,client_Port);
            exit(EXIT_SUCCESS);
        }
        else{
            close(clientSock);
        }
    }
}

int main(int argc,char **argv)
{
    if(argc!=3){
        printf("<executable code> <Server IP Address> <Server Port number>");
        exit(EXIT_FAILURE);
    }

    int nodeSock;
    int servPort = atoi(argv[2]);
    char* servIP = argv[1];

    if((nodeSock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("socket() error");
        exit(EXIT_FAILURE);
    } 

    struct sockaddr_in servAddr;
    memset(&servAddr, '\0', sizeof(servAddr)); 

    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(servPort);
    servAddr.sin_addr.s_addr = inet_addr(servIP);

    if( connect(nodeSock, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0){
       printf("connect() failed");
       exit(EXIT_FAILURE);
    }
    else{
        printf("Connected to server %s:%d\n",servIP,servPort);
    }

    int recvMsgSize,sendMsgSize;
    char* request = "request:node";
    
    if((sendMsgSize=write(nodeSock,request,strlen(request))) < 0){
        printf("write() request error");
        exit(EXIT_FAILURE);
    }
    else{
        printf("Registering with server %s:%d\n",servIP,servPort);
    }
    
    char buffer[1024];
    memset(buffer,'\0',sizeof(buffer));

    if((recvMsgSize = read(nodeSock,buffer,sizeof(buffer) - 1)) < 0){
        printf("read() error");
        exit(EXIT_FAILURE);
    }


    char* responsefield = strtok(buffer,"\n");
    char* statusfield = strtok(NULL,"\n");
    char* portfield = strtok(NULL,"\n");
    if(strcmp(responsefield,"response:server") != 0 || strcmp(statusfield,"status:connected")!=0){
        printf("Unexpected response from server");
        exit(EXIT_FAILURE);
    }
    else{
        portfield = strtok(portfield,":");
        portfield = strtok(NULL,":");
        if(shutdown(nodeSock,0)< 0){
            printf("shutdown() error");
            exit(EXIT_FAILURE);
        }
        else{
            printf("Registered with server %s:%d\n",servIP,servPort);
            printf("Gracefully closing connection with server %s:%d\n",servIP,servPort );
        }
        int nodeServerPort = atoi(portfield);
        startNodeServer(nodeServerPort);
    }
    return 0;
}
