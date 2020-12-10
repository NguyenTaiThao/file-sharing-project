// Client side C/C++ program to demonstrate Socket programming 
#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h>
#include<stdlib.h> 

int check_new_password(char new_password[100]){
	for(int i = 0; i < strlen(new_password); i++){
		if((new_password[i] < '0' || new_password[i] > '9') && 
		(new_password[i] < 'A' || new_password[i] > 'Z') && 
		(new_password[i] < 'a' || new_password[i] > 'z'))
		{ 
			return 0;
		}
		
	}
	return 1;
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

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
	{ 
		printf("\nConnection Failed \n"); 
		return -1; 
	} 


	// ============================Start to communicate with Server======================
	// ==================================================================================
	do {
		int g= 0;
		int signed_in = 0;
		char buffer[100];
		char message[100];
		char username[100];
		char password[100];
		char new_password[100];
		printf("Username: ");
		g = scanf("%[^\n]", message); 
		if (g == 0) break;
		getchar();
		strcpy(username, message);


		send(sock , "anhyeuem" , 9, 0 ); 
		
		// waiting for response 
		read(sock, buffer,100); 
        printf("%s\n", buffer);
		
	}while(1);	
	// close the descriptor 
	close(sock); 
	
					
	
	return 0; 
} 
