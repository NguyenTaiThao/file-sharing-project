#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <stdlib.h> 
#include <ctype.h>
#include <sys/types.h> 
#include <arpa/inet.h> 
#include <netdb.h>
#include <errno.h>
#include "linked_list.h"


void insertToList(singleList *list){
	group_struct *group_element = (group_struct*) malloc(sizeof(group_struct));
	// make null the list
	deleteSingleList(list);
	FILE *fp;
	fp = fopen("group.txt","r");
	// members
	singleList members;
	createSingleList(&members);
	simple_user_struct *member_element = (simple_user_struct*) malloc(sizeof(simple_user_struct));
	singleList files;
	createSingleList(&files);
	simple_file_struct *file_element = (simple_file_struct*) malloc(sizeof(simple_file_struct));
	//===========================
	char c;
	char str_tmp[100];
	fgets (str_tmp, 100, fp);
	while (1)
	{
		str_tmp[strlen(str_tmp)-1] = '\0';
		strcpy(group_element->group_name, str_tmp);
		printf("name: %s\n", str_tmp);
		// read members
		fgets (str_tmp, 100, fp);
		str_tmp[strlen(str_tmp)-1] = '\0';
		strcpy(group_element->owner, str_tmp);
		fgets (str_tmp, 100, fp);
		str_tmp[strlen(str_tmp)-1] = '\0';
		int number_of_members = atoi(str_tmp);
		group_element->number_of_members = number_of_members;
		for(int i = 0; i < number_of_members; i++){
			fgets(str_tmp, 100, fp);
			str_tmp[strlen(str_tmp)-1] = '\0';
			strcpy(member_element->user_name, str_tmp);
			printf("person %d: %s\n", i+1, member_element->user_name);
			insertEnd(&members, member_element);
		}
		group_element->members = members;
		//read files
		fgets (str_tmp, 100, fp);
		str_tmp[strlen(str_tmp)-1] = '\0';
		int number_of_files = atoi(str_tmp);
		group_element->number_of_files = number_of_files;
		for(int i = 0; i < number_of_files; i++){
			fgets (str_tmp, 100, fp);
			str_tmp[strlen(str_tmp)-1] = '\0';
			strcpy(file_element->file_name, str_tmp);
			printf("file %d: %s\n", i+1, file_element->file_name);
			insertEnd(&files, file_element);
		}
		group_element->files = files;
		//==================================================
		printf("%s\n", "123");
		insertEnd(list, group_element);
		c = fgetc(fp);
    	if (c != EOF){
			int res = fseek( fp, -1, SEEK_CUR );
			fgets (str_tmp, 100, fp);
			printf("char = %c\n", c);
		}else
        	break;
	}
	fclose(fp);
	//insert group-data to group-linked_list

}








int main(int argc, char *argv[]) 
{
	

	// catch wrong input
	// if(argc==1){
	// 	printf("Please input port number\n");
	// 	return 0;
	// }
	// char *port_number = argv[1];
	// int port = atoi(port_number);
	// int opt = 1;
	// int server_fd, new_socket; 
	// struct sockaddr_in address;
	// int addrlen = sizeof(address); 

	// // Creating socket file descriptor 
	// if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
	// { 
	// 	perror("socket failed"); 
	// 	exit(EXIT_FAILURE); 
	// } 
	
	// // Forcefully attaching socket to the port 
	// if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
	// { 
	// 	perror("setsockopt"); 
	// 	exit(EXIT_FAILURE); 
	// } 
	// address.sin_family = AF_INET; 
	// address.sin_addr.s_addr = INADDR_ANY; 
	// address.sin_port = htons( port ); 
	
	// // Forcefully attaching socket to the port 
	// if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) 
	// { 
	// 	perror("bind failed"); 
	// 	exit(EXIT_FAILURE); 
	// } 
	// if (listen(server_fd, 3) < 0) 
	// { 
	// 	perror("listen"); 
	// 	exit(EXIT_FAILURE); 
	// } 
	// if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) 
	// { 
	// 	perror("accept"); 
	// 	exit(EXIT_FAILURE); 
	// } 



	//============================Start to communicate with client=====================================
	//=================================================================================================
	int x, checked;
	char buffer1[100];
	char buffer2[100];
	char username[20];
	char password[20] = "";
	char checked_str[10];
	char only_string[20];
	char only_number[20];
	int status = 0;
	//status = 0 => cho nhap username
	//status = 1 => cho nhap password
	//status = 2 => da dang nhap
	int signed_in = 0;
	int valid_username = 0;
	singleList list;
	createSingleList(&list);
	group_struct group_element; 
	insertToList(&list);
	printf("\n123\n");
	printGroup(list);
	// while(1){
    //     x = read( new_socket , buffer1, 100);
    //     printf("%s\n", buffer1);
        
    //     send(new_socket , "123", 4 , 0 );
        
    // }
	return 0; 
} 
