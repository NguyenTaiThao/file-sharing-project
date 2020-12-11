// Client side C/C++ program to demonstrate Socket programming 
#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h>
#include <stdlib.h> 
#include "./request_code.h"


int menu1()
{
    int choice, catch;
	char err[10];
	printf("\n\n");
    printf("====================UPLOAD FILE SHARING===================\n");
    printf("1. Dang ky\n");
    printf("2. Dang nhap\n");
	printf("==========================================================\n");
    printf("=> Nhap lua chon cua ban: ");
    catch = scanf("%d",&choice);
	
	printf("\n\n");
	
	if(catch > 0) return choice;
	else {
		fgets(err, 10, stdin);
		err[strlen(err)-1] = '\0';
		printf("\"%s\" is not allowed!\n",err);
		return -1;
	}
}

int menu2()
{
    int choice, catch;
	char err[10];
	printf("\n\n");
	printf("========================== GROUP ========================\n");
    printf("1. Tao nhom\n");
    printf("2. Vao nhom\n");
    printf("3. Truy cap nhom da vao\n");
    printf("4. Dang xuat\n");
	printf("==========================================================\n");
    printf("=> Nhap lua chon cua ban: ");
    catch = scanf("%d",&choice);

	printf("\n\n");

    if(catch > 0) return choice;
	else {
		fgets(err, 10, stdin);
		err[strlen(err)-1] = '\0';
		printf("\"%s\" is not allowed!\n",err);
		return -1;
	}
}

void navigation(int sock){
	int z1,z2;
	char code[10];

	z1 = menu1();
	
	switch (z1)
	{
		case 1:
			sendCode(sock, REGISTER_REQUEST);
			printf("Dang ki thanh cong\n");
			break;
		case 2:
			do {
				sendCode(sock, LOGIN_REQUEST);
				printf("Dang ki thanh cong\n");

				z2 = menu2();
				switch (z2)
				{
				case 1:
					printf("Day la chuc nang tao nhom\n");
					sendCode(sock, CREATE_GROUP_REQUEST);
					break;
				case 2:
					printf("Day la chuc nang vao nhom\n");
					sendCode(sock, JOIN_GROUP_REQUEST);
					break;
				case 3:
					printf("Day la chuc nang truy cap nhom da vao\n");
					sendCode(sock, ACCESS_GROUP_REQUEST);
					break;
				case 4:
					printf("Day la chuc nang dang xuat\n");
					sendCode(sock, LOGOUT_REQUEST);
					z1=-1;
					break;
				default:
					z2 = 1;
					break;
				}
			}while(z2 >= 1 && z2 < 4);
			break;
		default:
			break;
	}
}

void sendCode(int sock, int code){
	char codeStr[10];
	sprintf(codeStr, "%d", code);
	// send(sock , codeStr , strlen(codeStr) + 1 , 0 ); 
}

int main(int argc, char *argv[]) 
{
	if(argc!=3){
		printf("Please input IP address and port number\n");
		return 0;
	}
	// ip_address of server
	// port number	
	char *ip_address = argv[1];
	char *port_number = argv[2];
	int port = atoi(port_number);
	int sock = 0; 
	struct sockaddr_in serv_addr; 

	// Try catch false when connecting
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Socket creation error \n"); 
		return -1; 
	} 

	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(port); 
	
	// Convert IPv4 and IPv6 addresses from text to binary form 
	if(inet_pton(AF_INET, ip_address, &serv_addr.sin_addr)<=0) 
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		return -1; 
	} 

	// if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
	// { 
	// 	printf("\nConnection Failed \n"); 
	// 	return -1; 
	// } 


	// ============================Start to communicate with Server======================
	// ==================================================================================
	do {
		char buffer[100];
		char message[100];
			
		
		navigation(sock);
		// send(sock , "anhyeuem" , 9, 0 ); 
		
		// waiting for response 
		// read(sock, buffer,100); 
        // printf("%s\n", buffer);
		
	}while(1);	
	
	// close the descriptor 
	close(sock); 
	
					
	
	return 0; 
} 
