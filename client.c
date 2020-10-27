// INCLUDE LIBRARY
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// DECLARE ENVIROMENT VARIABLE
#define BUFF_SIZE 100
#define MAXLINE 100

// DECLARE GLOBAL VARIABLE
char loginUser[BUFF_SIZE], deviceId[BUFF_SIZE];

// DECLARE FUNCTIONS
int menu1();
int menu2(int option1);
void sendRequest(char* content, int sockfd);


// MAIN FUNCTION
int main(int argc, char *argv[]){

    int sockfd, serverPortNum, rcvByte;
    struct sockaddr_in servaddr;
    char sendline[MAXLINE], recvline[MAXLINE];

    // create a socket for theo client
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("Error: ");
        exit(-1);
    }

    serverPortNum = atoi(argv[2]);

    // create the remote server socket information structure
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);
    // inet_aton("127.0.0.1", &servaddr.sin_addr); 
    servaddr.sin_port = htons(serverPortNum);

    // connection theo client to the server socket
    if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){
        perror("Problem in connecting to the server");
        exit(-1);
    }

    return 0;
}

// MENU CHA
int menu1(){
    int option1;
    printf("1. Login.\n");
    printf("2. Register.\n");
    printf("3. Create your own group.\n");
    printf("4. Manage your groups.\n");
    printf("5. Log out.\n");
    printf("6. Exit.\n");
    do{
        scanf("%d", &option1);
    }while(option1 > 0 && option1 < 7);

    return option1;
}

// MENU CON
int menu2(int option1){
    int option2;
    printf("1. Upload your to the group.\n");
    printf("2. Download file from the group.\n");
    printf("3. All files in the group.\n");
    printf("4. Search files in the group.\n");
    printf("5. Delete the group.\n");
    printf("6. Back.\n");
}

// SEND REQUEST
void sendRequest(char* content, int sockfd){
    int sendByte;

    content[strlen(content)-1] = '\0';
    sendByte = send(sockfd, content, strlen(content) + 1, 0);
    if(sendByte < 0){
        perror("Can't send request: ");
    }
}