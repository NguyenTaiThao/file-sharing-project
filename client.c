// Client side C/C++ program to demonstrate Socket programming 
#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h>
#include <stdlib.h> 
#include <pthread.h>
#include "./communication_code.h"

#define BUFF_SIZE 100

int printAvailableElements(char str[1000], char available_elements[20][50]){
	char *token;
	int number_of_available_elements = 0;
   	/* get the first token */
   	token = strtok(str, "+");
   
   	/* walk through other tokens */
   	while( token != NULL ) {
    	printf( "%d. %s\n", number_of_available_elements + 1, token );
		strcpy(available_elements[number_of_available_elements], token);
    	token = strtok(NULL, "+");
		number_of_available_elements++;
   	}
	return number_of_available_elements;
}

int receiveFile(int sock){
	int bytesReceived = 0;
	char recvBuff[1024];
	memset(recvBuff, '0', sizeof(recvBuff));
	FILE *fp;
	char fname[100], path[100];
	read(sock, fname, 256);
	path[0] = '\0';
	strcat(path, "./client_source/");
	strcat(path, fname);
	printf("File Name: %s\n",path);
	printf("Receiving file...");
	fp = fopen(path, "ab"); 
	if(NULL == fp)
	{
		printf("Error opening file");
		return 1;
	}
	double sz=1;
	/* Receive data in chunks of 256 bytes */
	while((bytesReceived = read(sock, recvBuff, 1024)) > 0)
	{
		system("clear"); 
		
		printf("\n\n\nbytes = %d\n",bytesReceived);
		sz++;
		printf("Received: %lf Mb\n",(sz/1024));
		fflush(stdout);
		// recvBuff[n] = 0;
		fwrite(recvBuff, 1,bytesReceived,fp);

		if(bytesReceived < 1024){
			break;
		}
	}
	fclose(fp);
	if(bytesReceived < 0)
	{
		printf("\n Read Error \n");
		return 0;
	}
	printf("\nFile OK....Completed\n");
	return 1;
}

int menu1();
int menu2();
int menu3(char group_name[50]);
void navigation(int sock);
void signUp(int sock);
int signIn(int sock);
void createGroup(int sock);
void uploadFile(int sock, char groupName[50]);
void sendCode(int sock, int code);
void clearBuff();

//=============== MAIN ==================
int main(int argc, char *argv[]) 
{
	pthread_t thread;

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


	// if (pthread_create(&thread, (void *) NULL, receiveResponse, (void *) &sock ) < 0) {
    //     printf("ERROR: creating thread\n");
    //     exit(1);
    // }


	// ============================Start to communicate with Server======================
	// ==================================================================================
	do {
		navigation(sock);
	}while(1);	

	
	// close the descriptor 
	close(sock); 
		
	return 0; 
} 



int menu1()
{
	//system("clear");
    int choice, catch;
	char err[10];
	printf("\n\n");
    printf("====================UPLOAD FILE SHARING===================\n");
    printf("1. Sign up\n");
    printf("2. Sign in\n");
	printf("==========================================================\n");
    printf("=> Enter your choice: ");
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
	//sleep(0.5);
	//system("clear");
    int choice, catch;
	char err[10];
	printf("\n\n");
	printf("========================= GROUPS ========================\n");
	printf("1. Create group\n");
    printf("2. Join group\n");
    printf("3. Access joined group\n");
	printf("4. Search\n");
    printf("5. Logout\n");
	printf("=========================================================\n");
    printf("=> Enter your choice: ");
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

int menu3(char group_name[50])
{
	//sleep(0.5);
	//system("clear");
    int choice, catch;
	char err[10];
	printf("\n\n");
	printf("========================== %s ========================\n", group_name);
    printf("1. Upload\n");
    printf("2. Download\n");
    printf("3. Delete file\n");
    printf("4. View all files\n");
	printf("5. Kick\n");
	printf("6. Back\n");
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

void signUp(int sock){
	char username[50], password[50], buff[BUFF_SIZE];
	
	sendCode(sock, REGISTER_REQUEST);
    read(sock, buff, BUFF_SIZE);
	printf("========================= SIGNUP ========================\n");
	
	clearBuff();
	while(1){
		printf("Nhap tai khoan: ");
		fgets(username, 50, stdin);
		while(strlen(username) <= 0 || username[0] == '\n'){
			printf("Username khong duoc de trong!!\n");
			printf("Nhap tai khoan: ");
			fgets(username, 50, stdin);
		}
		send(sock, username, sizeof(username), 0);

		read(sock, buff, BUFF_SIZE);
		if(atoi(buff) == EXISTENCE_USERNAME){
			printf("Username da duoc su dung!!\n");
		}else{
			break;
		}
	};

	printf("Nhap mat khau: ");
	fgets(password, 50, stdin);
	while(strlen(password) <= 0 || password[0] == '\n'){
		printf("Mat khau khong duoc de trong!!\n");
		printf("Nhap mat khau: ");
		fgets(password, 50, stdin);
	}
	send(sock, password, sizeof(password), 0);

	read(sock, buff, BUFF_SIZE);
	if(atoi(buff) != REGISTER_SUCCESS){
		printf("He thong dang bao tri!!\n");
	}else{
		printf("Dang ki tai khoan thanh cong\n");
	}
}

int signIn(int sock){
	char username[50], password[50], buff[BUFF_SIZE];

	sendCode(sock, LOGIN_REQUEST);
	read(sock, buff, BUFF_SIZE);
    printf("========================= SIGNIN ========================\n");
	
	clearBuff();
	while(1){
		printf("Nhap tai khoan: ");
		fgets(username, 50, stdin);
		while(strlen(username) <= 0 || username[0] == '\n'){
			username[0] = '\0';
			printf("Username khong duoc de trong!!\n");
			printf("Nhap tai khoan: ");
			fgets(username, 50, stdin);
		}
		
		send(sock, username, sizeof(username), 0);

		read(sock, buff, BUFF_SIZE);
		if(atoi(buff) == NON_EXISTENCE_USERNAME){
			printf("Username khong ton tai!!\n");
		}else{
			break;
		}
	}

	printf("Nhap mat khau: ");
	// scanf("%s", password);
	fgets(password, 50, stdin);
	while(strlen(password) <= 0 || password[0] == '\n'){
		printf("Mat khau khong duoc de trong!!\n");
		printf("Nhap mat khau: ");
		fgets(password, 50, stdin);
		// scanf("%s", password);
	}
	// password[strlen(password) - 1] = '\0';
	send(sock, password, sizeof(password) + 1, 0);
	read(sock, buff, BUFF_SIZE);
	if(atoi(buff) != LOGIN_SUCCESS){
		printf("Login failed!!\n");
		return 0;
	}else{
		return 1;
	}
}

void navigation(int sock){
	int z1, z2, z3;
	char buffer[100], code[10], username[100], password[100], new_password[100];
	z1 = menu1();
	
	switch (z1)
	{
		case 1:
			signUp(sock);
			break;
		case 2:
			if(signIn(sock) == 1){
				do {
					z2 = menu2();
					switch (z2)
					{
					case 1:
						sendCode(sock, CREATE_GROUP_REQUEST);
						createGroup(sock);
						read(sock, buffer, 1000);
						printf("%s\n", buffer);
						break;
					case 2:
						printf("========================== Available Group ==========================\n");
						sendCode(sock, JOIN_GROUP_REQUEST);
						read(sock, buffer, 1000); 
						char available_group[20][50] = {'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};
						int number_of_available_groups = printAvailableElements(buffer, available_group);
						int selected_group;
						if(number_of_available_groups > 0){
							printf("Which group do you want to join? (1-%d): ", number_of_available_groups);
							scanf("%d", &selected_group);
							send(sock , available_group[selected_group-1] , strlen(available_group[selected_group-1]) + 1 , 0 ); 
							read(sock, buffer, 1000);
							if(atoi(buffer) == JOIN_GROUP_SUCCESS){
								printf("Join successfully\n");
							}else{
								printf("Something went wrong!!!\n");
							}
						}else{
							printf("Have not any groups\n");
						}
						break;
					case 3:
						printf("==================== Available Group ====================\n");
						sendCode(sock, ACCESS_GROUP_REQUEST);
						read(sock, buffer, 1000);
						number_of_available_groups = printAvailableElements(buffer, available_group);
						if(number_of_available_groups > 0){
							printf("Which group do you want to access? (1-%d): ", number_of_available_groups);
							scanf("%d", &selected_group);
							send(sock , available_group[selected_group-1] , strlen(available_group[selected_group-1]) + 1 , 0 );
							read(sock, buffer, 1000);
							if(atoi(buffer) == ACCESS_GROUP_SUCCESS){
								printf(" => Access %s successfully\n", available_group[selected_group-1]);
								z3 = 0;
							}else{
								printf("Something wrong!!!\n");
							}
						}else{
							printf("You have not joined any groups.\n");
							z3 = 6;
						}
						while(z3 != 6){
							z3 = menu3(available_group[selected_group-1]);
							switch (z3)
							{
								case 1:{
									uploadFile(sock, available_group[selected_group-1]);
									break;
								}
								case 2:
									sendCode(sock, DOWNLOAD_REQUEST);
									printf("==================== Available Files =====================\n");
									read(sock, buffer, 1000);
									char available_files[20][50] = {'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};
									int number_of_available_files = printAvailableElements(buffer, available_files);
									int selected_file;
									if(number_of_available_files > 0){
										printf("Which file do you want to download? (1-%d): ", number_of_available_files);
										scanf("%d", &selected_file);
										send(sock, available_files[selected_file-1] , strlen(available_files[selected_file-1]) + 1 , 0 );
										if(receiveFile(sock) == 1){
											printf("Open the file? (Y/n): ");
											char choice[10], command[100];
											scanf("%s", choice);
											if(strcmp(choice, "Y") == 0){
												command[0] = '\0';
												strcat(command, "xdg-open ./client_source/");
												strcat(command, available_files[selected_file-1]);
												system(command);
											}
										}
									}else{
										printf("This group does not have any files\n");
									}
									break;
								case 3:
									sendCode(sock, DELETE_REQUEST);
									printf("==================== Available Files =====================\n");
									read(sock, buffer, 1000); 
									number_of_available_files = printAvailableElements(buffer, available_files);
									if(number_of_available_files > 0){
										printf("Which file do you want to delete? (1-%d): ", number_of_available_files);
										scanf("%d", &selected_file);
										send(sock, available_files[selected_file-1] , strlen(available_files[selected_file-1]) + 1 , 0 );
									}else{
										printf("This group does not have any files");
										sendCode(sock, NO_FILE_TO_DELETE);
									}
									break;
								case 4:
									printf("======================= All Files ========================\n");
									sendCode(sock, VIEW_FILES_REQUEST);
									read(sock, buffer, 1000); 
									number_of_available_files = printAvailableElements(buffer, available_files);
									break;
								case 5:
									sendCode(sock, KICK_MEMBER_REQUEST);
									read(sock, buffer, 1000);
									if(atoi(buffer) == NOT_OWNER_OF_GROUP){
										printf("!!! Only the administrator of group can do this\n");
									}else{
										printf("====================== All Members =======================\n");
										char available_members[20][50] = {'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};
										int number_of_available_members = printAvailableElements(buffer, available_members);
										if(number_of_available_members > 0){
											printf("Which member do you want to kick? (1-%d): ", number_of_available_members);
											int selected_member;
											scanf("%d", &selected_member);
											printf("select = %d\n%s\n", selected_member, available_members[selected_member-1]);
											send(sock, available_members[selected_member-1] , strlen(available_members[selected_member-1]) + 1 , 0 );
										}else{printf("This group does not have any members\n");}
									}
									break;
								case 6:
									sendCode(sock, BACK_REQUEST);
									z3 = 6;
									break;
								default:
									break;
								}
							}
							break;
					case 4:
						sendCode(sock, SEARCH_FILE_REQUEST);
						char category[6][10] = {"text","image","audio","video","other"};
						printf("1. Text\n");
						printf("2. Image\n");
						printf("3. Audio\n");
						printf("4. Video\n");
						printf("5. Other\n");
						printf("6. Back\n");
						printf("==========================================================\n");
						printf("=> Enter your choice: ");
						int category_choice;
						scanf("%d", &category_choice);
						send(sock, category[category_choice-1] , strlen(category[category_choice-1]) + 1 , 0 );
						read(sock, buffer, 1000);
						char available_files[60][50] = {'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};
						int number_of_available_files = printAvailableElements(buffer, available_files);
						break;
					case 5:
						sendCode(sock, LOGOUT_REQUEST);
						read(sock,buffer, BUFF_SIZE);
						printf("-->logout: %s\n", buffer);
						if(atoi(buffer) == LOGOUT_SUCCESS){
							printf("Logout successfully.\n");
						}
						break;
					default:
						z2 = 1;
						break;
					}
				}while(z2 >= 1 && z2 < 5);
			}	
			break;
		default:
			break;
	}
}

void createGroup(int sock){
	char group_name[50];
	printf("Nhap ten nhom: ");

	clearBuff();

	fgets(group_name, 50, stdin);
	group_name[strlen(group_name) - 1] = '\0';
	send(sock, group_name, strlen(group_name) + 1, 0);
}

void sendCode(int sock, int code){
	char codeStr[10];
	sprintf(codeStr, "%d", code);
	send(sock , codeStr , strlen(codeStr) + 1 , 0 ); 
}

void clearBuff(){
	char c;
	while ((c = getchar()) != '\n' && c != EOF) { }
}

void* SendFileToServer(int new_socket, char fname[50])
{
	
    FILE *fp = fopen(fname,"rb");
    if(fp==NULL)
    {
        printf("File open error");
    }   

    /* Read data from file and send it */
    while(1)
    {
        /* First read file in chunks of 256 bytes */
        unsigned char buff[1024]={0};
        int nread = fread(buff,1,1024,fp);
        //printf("Bytes read %d \n", nread);        

        /* If read was success, send data. */
        if(nread > 0)
        {
            write(new_socket, buff, nread);
        }
        if (nread < 1024)
        {
            if (feof(fp))
            {
                printf("End of file\n");
            }
            if (ferror(fp))
                printf("Error reading\n");
            break;
        }
    }
}

void uploadFile(int sock, char groupName[50]){
	char fileName[50], filePath[100];
	char buffer[BUFF_SIZE];

	sendCode(sock, UPLOAD_REQUEST);
	read(sock, buffer, BUFF_SIZE);

	if(atoi(buffer) == UPLOAD_SUCCESS){
		printf("group_name: %s %ld\n", groupName, sizeof(groupName));
		send(sock, groupName, strlen(groupName) + 1, 0);

		printf("Nhap ten file: ");
		clearBuff();
		fgets(fileName, 50, stdin);
		send(sock, fileName, sizeof(fileName), 0);

		do{
			printf("Nhap duong dan toi file: ");
			fgets(buffer, 100, stdin);
			buffer[strlen(buffer) - 1] = '\0';
			if(fopen(buffer, "r") != NULL){
				break;
			}else{
				printf("File khong hop le!!\n");
			}
		}while(1);
		filePath[0] = '\0';
		strcat(filePath, buffer);
		SendFileToServer(sock, filePath);

		
	}else{
		printf("He thong dang bao tri!!\n");
	}
}