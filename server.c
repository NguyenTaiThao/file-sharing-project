#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <ctype.h>
#include <sys/types.h> 
#include <arpa/inet.h> 
#include <netdb.h>
#include <errno.h>
#include "linked_list.h"


void readGroupFile(singleList *list){
	
	// clear list
	deleteSingleList(list);
	FILE *fp;
	fp = fopen("group.txt","r");
	char str_tmp[100];
	fgets (str_tmp, 100, fp);
	while (1)
	{
		//====================initialize============================================
		group_struct *group_element = (group_struct*) malloc(sizeof(group_struct));
		singleList members;
		createSingleList(&members);
		singleList files;
		createSingleList(&files);
		//======================end initialize======================================
		str_tmp[strlen(str_tmp)-1] = '\0';
		strcpy(group_element->group_name, str_tmp);
		//=====================read members=========================================
		fgets (str_tmp, 100, fp);
		str_tmp[strlen(str_tmp)-1] = '\0';
		strcpy(group_element->owner, str_tmp);
		fgets (str_tmp, 100, fp);
		str_tmp[strlen(str_tmp)-1] = '\0';
		int number_of_members = atoi(str_tmp);
		group_element->number_of_members = number_of_members; // number_of_members
		for(int i = 0; i < number_of_members; i++){
			simple_user_struct *member_element = (simple_user_struct*) malloc(sizeof(simple_user_struct));
			fgets(str_tmp, 100, fp);
			str_tmp[strlen(str_tmp)-1] = '\0';
			strcpy(member_element->user_name, str_tmp);
			insertEnd(&members, member_element);
		}
		group_element->members = members;
		//===================end read members========================================
		//=====================read files============================================
		fgets (str_tmp, 100, fp);
		str_tmp[strlen(str_tmp)-1] = '\0';
		int number_of_files = atoi(str_tmp);
		group_element->number_of_files = number_of_files;
		for(int i = 0; i < number_of_files; i++){
			simple_file_struct *file_element = (simple_file_struct*) malloc(sizeof(simple_file_struct));
			fgets (str_tmp, 100, fp);
			if(str_tmp[strlen(str_tmp)-1] == '\n'){
				str_tmp[strlen(str_tmp)-1] = '\0';
			}
			strcpy(file_element->file_name, str_tmp);
			insertEnd(&files, file_element);
		}
		group_element->files = files;
		//=====================end read files=========================================
		insertEnd(list, group_element); // add group_element to group_list
		char c = fgetc(fp);
    	if (c != EOF){
			int res = fseek( fp, -1, SEEK_CUR );
			fgets (str_tmp, 100, fp);
		}else
        	break;
	}
	fclose(fp);
}

void writeToGroupFile(char group_name[50], char owner[50], int number_of_files, int number_of_members, singleList members, singleList files){
	FILE *fp;
	fp = fopen("group.txt","a");
	fprintf(fp,"\n%s", group_name);
	fprintf(fp,"\n%s", owner);
	fprintf(fp,"\n%d", number_of_members);
	members.cur = members.root;
	while(members.cur!=NULL){
		fprintf(fp,"\n%s",((simple_user_struct*)members.cur->element)->user_name);
		members.cur = members.cur->next;
	}
	fprintf(fp,"\n%d", number_of_files);
	files.cur = files.root;
	while(files.cur!=NULL){
		fprintf(fp,"\n%s",((simple_file_struct*)files.cur->element)->file_name);
		files.cur = files.cur->next;
	}
	fclose(fp);
}


void readUserFile(singleList* users){
	char username[50], password[50], group_name[50];
	int status, count_group;
	FILE * f = fopen("user.txt","r");

	if(f == NULL)
	{
		perror("Error while opening the file.\n");
		exit(EXIT_FAILURE);
	}

	while (fscanf(f, "%s %s %d\n", username, password, &status) > 0) {
		singleList groups;
		createSingleList(&groups);

		if(fscanf(f, "%d\n", &count_group) > 0){
			for(int i = 0; i < count_group; i++){
				fscanf(f, "%s\n", group_name);
				insertEnd(&groups, strdup(group_name));
			}
		}

		user_struct *user = (user_struct*)malloc(sizeof(user_struct));
		strcpy(user->user_name, username);
		strcpy(user->password, password);
		user->status = status;
		user->joined_groups = groups;
		user->count_group = count_group;

		insertEnd(users, user);
	}
    fclose(f);

}

void readFileFile(singleList *files){
	FILE *f;
	char name[50], owner[50], uploaded_at[50];
	int download_times;

	f = fopen("./file.txt", "r");
	if(f == NULL){
		fprintf(stderr, "File missing: can not find \"file.txt\".\n");
		exit(-1);
	}

	while(fscanf(f, "%s %s %s %d\n", name, owner, uploaded_at, &download_times)>0){
		file_struct *file = (file_struct*)malloc(sizeof(file_struct));
		strcpy(file->name, name);
		strcpy(file->owner,owner);
		strcpy(file->uploaded_at, uploaded_at);
		insertEnd(files, file);
	}
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
	singleList list, files, users;

	createSingleList(&list);
	createSingleList(&files);
	createSingleList(&users);
	
	readGroupFile(&list);
	readFileFile(&files);
	readUserFile(&users);
	// while(1){
    //     x = read( new_socket , buffer1, 100);
    //     printf("%s\n", buffer1);
        
    //     send(new_socket , "123", 4 , 0 );
        
    // }
	return 0; 
} 
