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
int menu2();
int navigator1();
int navigator2(int option2);
void sendRequest(char* content, int sockfd);


// MAIN FUNCTION
int main(int argc, char *argv[]){

    int sockfd, serverPortNum, rcvByte;
    struct sockaddr_in servaddr;
    char sendline[MAXLINE], recvline[MAXLINE];

    // create a socket for the client
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("Error: ");
        exit(-1);
    }

    serverPortNum = 9090;

    // create the remote server socket information structure
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(serverPortNum);

    // connection theo client to the server socket
    // if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){
    //     perror("Problem in connecting to the server");
    //     exit(-1);
    // }
    while(1){
        navigator1();
    }

    return 0;
}

// MENU CHA
int menu1(){
    int option1;
    printf("---------------------------MAIN MENU---------------------------\n");
    printf("1. Login.\n");
    printf("2. Register.\n");
    printf("3. Create your own group.\n");
    printf("4. Manage your groups.\n");
    printf("5. Join new groups.\n");
    printf("6. Log out.\n");
    printf("7. Exit.\n");
    printf("---------------------------------------------------------------\n");

    do{
        printf("=>Enter your option: ");
        scanf("%d", &option1);
        system("clear");
    }while(option1 <= 0 || option1 > 7);

    return option1;
}

// MENU CON
int menu2(){
    int option2;
    printf("---------------------------GROUP MENU--------------------------\n");
    printf("1. Upload your to the group.\n");
    printf("2. Download file from the group.\n");
    printf("3. All files in the group.\n");
    printf("4. Search files in the group.\n");
    printf("5. Delete the group.\n");
    printf("6. Back.\n");
    printf("---------------------------------------------------------------\n");

    do{
        printf("=>Enter your option: ");
        scanf("%d", &option2);
        system("clear");
    }while(option2 <= 0 || option2 >= 7);

    return option2;
}

// NAVIGATOR CHA
int navigator1(){
    int option1, option2;
    option1 = menu1();
    switch (option1)
    {
    case 1:
        printf("dang nhap\n");
        break;
    case 2:
        printf("dang ki\n");
        break;
    case 3:
        printf("tao nhom\n");
        break;
    case 4:
        printf("nhom\n");
        option2 = menu2();
        break;
    case 5:
        printf("tham gia nhom\n");
        break;
    case 6:
        printf("dang xuat\n");
        break;
    case 7:
        exit(0);
        break;
    default:
        printf("INTERNAL ERROR!\n");
        break;
    }
}

int navigator2(int option2){
    switch (option2)
    {
    case 1:
        printf("upload\n");
        break;
    case 2:
        printf("download\n");
        break;
    case 3:
        printf("list\n");
        break;
    case 4:
        printf("search\n");
        break;
    case 5:
        printf("delete\n");
        break;
    case 6:
        printf("back\n");
        break;
    default:
        printf("INTERNAL ERROR!\n");
        break;
    }
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