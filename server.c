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
#include <sys/stat.h>
#include "communication_code.h"
#include "linked_list.h"


int REQUEST;


void readGroupFile(singleList *groups){
	
	// clear list
	deleteSingleList(groups);
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
		insertEnd(groups, group_element); // add group_element to group_list
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

int checkExistence(int type, singleList list, char string[50])
{
	// type = 1 check user
	// type = 2 check group
	// type = 3 check file
	switch (type)
	{
	case 1:
		// Check user
		{
			int i = 0;
			list.cur = list.root;
			while (list.cur != NULL)
			{
				i++;
				if(strcmp(((user_struct*)list.cur->element)->user_name,string) != 0)
				{
					list.cur = list.cur->next;
				}
				else {
					return 1;
				}
				
			}
			return 0; 
		}
		break;
	case 2:
		// Check Group
		{
			int i = 0;
			list.cur = list.root;
			while (list.cur != NULL)
			{
				i++;
				if(strcmp(((group_struct*)list.cur->element)->group_name,string) != 0)
				{
					list.cur = list.cur->next;
				}
				else {
					return 1;
				}
				
			}
			return 0; 
		}
		break;
	case 3:
		// Check File
		{
			int i = 0;
			list.cur = list.root;
			while (list.cur != NULL)
			{
				i++;
				if(strcmp(((file_struct*)list.cur->element)->name,string) != 0)
				{
					list.cur = list.cur->next;
				}
				else {
					return 1;
				}
				
			}
			return 0; 
		}
		break;

	default:
		printf("Type chua hop le !! (1,2 or 3)\n");
		break;
	}
}

int addMember(singleList groups, char group_name[50], char username[50]){
	singleList members;
	createSingleList(&members);
  	groups.cur = groups.root;
	while(groups.cur != NULL)
	{
		if(strcmp(((group_struct*)groups.cur->element)->group_name, group_name) == 0){
			members = ((group_struct*)groups.cur->element)->members;
			simple_user_struct *member_element = (simple_user_struct*) malloc(sizeof(simple_user_struct));
			strcpy(member_element->user_name, username);
			insertEnd(&members, member_element);
			((group_struct*)groups.cur->element)->number_of_members++;
			return 1;
		}
		groups.cur = groups.cur->next;
	}
	return 0;
}

int addGroupToJoinedGroups(singleList users, char username[50], char group_name[50]){
	users.cur = users.root;
	while(users.cur != NULL)
	{
		if(strcmp(((user_struct*)users.cur->element)->user_name, username) == 0){
			simple_group_struct *group_element = (simple_group_struct*) malloc(sizeof(simple_group_struct));
			strcpy(group_element->group_name, group_name);
			insertEnd(&((user_struct*)users.cur->element)->joined_groups, group_element);
			((user_struct*)users.cur->element)->count_group++;
			return 1;
		}
		users.cur = users.cur->next;
	}
	return 0;
}

singleList unJoinedGroups(singleList groups, singleList users, char username[50]){
	singleList joined_groups;
	createSingleList(&joined_groups);
	singleList un_joined_groups;
	createSingleList(&un_joined_groups);
	users.cur = users.root;
	while(users.cur != NULL)
	{
		if(strcmp(((user_struct*)users.cur->element)->user_name, username) == 0){
			groups.cur = groups.root;
			joined_groups = ((user_struct*)users.cur->element)->joined_groups;
			break;
		}
		users.cur = users.cur->next;
	}

	groups.cur = groups.root;
	while(groups.cur != NULL)
	{
		int check = 0;
		joined_groups.cur = joined_groups.root;
		while(joined_groups.cur != NULL)
		{
			if( strcmp( ((group_struct*)groups.cur->element)->group_name, ((simple_group_struct*)joined_groups.cur->element)->group_name) == 0)
			{
				check = 1;
				break;
			}
			joined_groups.cur = joined_groups.cur->next;
		}
		if(check == 0){
			simple_group_struct *group_element = (simple_group_struct*) malloc(sizeof(simple_group_struct));
			strcpy(group_element->group_name, ((group_struct*)groups.cur->element)->group_name);
			insertEnd(&un_joined_groups, group_element);
		}
		groups.cur = groups.cur->next;
	}
	return un_joined_groups;
}


void convertSimpleGroupsToString(singleList simple_group, char str[1000]){
	str[0] = '\0';
	simple_group.cur = simple_group.root;
	while(simple_group.cur != NULL)
  	{
		strcat(str, ((simple_group_struct*)simple_group.cur->element)->group_name);
		if(simple_group.cur->next == NULL){
			str[strlen(str)] = '\0';
		}else{
			strcat(str, "+");
		}
    	simple_group.cur = simple_group.cur->next;
  	}
}

void convertSimpleFilesToString(singleList simple_file, char str[1000]){
	str[0] = '\0';
	simple_file.cur = simple_file.root;
	while(simple_file.cur != NULL)
  	{
		strcat(str, ((simple_file_struct*)simple_file.cur->element)->file_name);
		if(simple_file.cur->next == NULL){
			str[strlen(str)] = '\0';
		}else{
			strcat(str, "+");
		}
    	simple_file.cur = simple_file.cur->next;
  	}
}

void getBasicInfoOfGroup(singleList groups, char group_name[50], char group_info[200]){
	char temp_str[10];
	group_info[0] = '\0';
	strcat(group_info, "===========================================\n\t\t");
	strcat(group_info, group_name);
	groups.cur = groups.root;
	while(groups.cur != NULL){
		if(strcmp( ((group_struct*)groups.cur->element)->group_name, group_name) == 0){
			strcat(group_info, "\n- Created by ");
			strcat(group_info, ((group_struct*)groups.cur->element)->owner);
			strcat(group_info, ".\n- ");
			sprintf(temp_str, "%d", ((group_struct*)groups.cur->element)->number_of_members);
			strcat(group_info, temp_str);
			strcat(group_info, " members.\n- ");
			sprintf(temp_str, "%d", ((group_struct*)groups.cur->element)->number_of_files);
			strcat(group_info, temp_str);
			strcat(group_info, " files shared.\n");
			strcat(group_info, "===========================================\n");
			strcat(group_info, "1. Upload\n");
			strcat(group_info, "2. Download\n");
			strcat(group_info, "3. Delete\n");
			strcat(group_info, "4. View Files\n");
			strcat(group_info, "5. Back to menu\n");
			strcat(group_info, "===========================================\n");
			group_info[strlen(group_info)] = '\0';
			break;
		}
		groups.cur = groups.cur->next;
	}
}

void createGroup(int sock, singleList *groups){
	char buff[100], noti[100], cmd[100];
	read(sock, buff, 100);

	if(checkExistence(2, *groups, buff) == 1){
		strcpy(noti, "Ten nhom vua nhap da duoc su dung.");
		send(sock, noti, strlen(noti) + 1, 0);
	}else{
		group_struct *group_element = (group_struct*) malloc(sizeof(group_struct));
		singleList members, files;
		
		createSingleList(&members);
		createSingleList(&files);

		strcpy(group_element->group_name, buff);
		group_element->files = files;
		group_element->members = members;
		group_element->number_of_files = 0;
		group_element->number_of_members = 1;

		insertEnd(groups, group_element);
		strcpy(cmd, "mkdir ");
		strcat(cmd, "./files/");
		strcat(cmd, buff);
		system(cmd);

		strcpy(noti, "Nhom duoc tao thanh cong.");
		send(sock, noti, strlen(noti) + 1, 0);
	}
}

void sendCode(int sock, int code){
	char codeStr[10];
	sprintf(codeStr, "%d", code);
	send(sock , codeStr , strlen(codeStr) + 1 , 0 ); 
}

singleList joinedGroups(singleList users, char username[50]){
	singleList joined_groups;
	createSingleList(&joined_groups);
	users.cur = users.root;
	while(users.cur != NULL)
	{
		if(strcmp(((user_struct*)users.cur->element)->user_name, username) == 0){
			joined_groups = ((user_struct*)users.cur->element)->joined_groups;
			break;
		}
		users.cur = users.cur->next;
	}
	return joined_groups;
}

singleList getAllFilesOfGroup(singleList groups, char group_name[50]){
	singleList files;
	createSingleList(&files);
	groups.cur = groups.root;
	while (groups.cur != NULL)
	{
		if(strcmp( ((group_struct*)groups.cur->element)->group_name, group_name) == 0){
			files = ((group_struct*)groups.cur->element)->files;
			break;
		}
		groups.cur = groups.cur->next;
	}
	return files;
}



int SendFileToClient(int new_socket, char fname[50])
{
	char sizeStr[10];
	struct stat st;
	stat(fname, &st);
	long int size = st.st_size;
	printf("%ld\n", size);
	sprintf(sizeStr, "%ld", size);
	send(new_socket , sizeStr, strlen(sizeStr)+1, 0 );
    FILE *fp = fopen(fname,"rb");
    if(fp==NULL)
    {
        printf("%s\n","File opern error");
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
            //printf("Sending \n");
			send(new_socket , buff, nread + 1, 0 );
            //send(new_socket, buff, nread);
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



int main(int argc, char *argv[]) 
{
	//catch wrong input
	if(argc==1){
		printf("Please input port number\n");
		return 0;
	}
	char *port_number = argv[1];
	int port = atoi(port_number);
	int opt = 1;
	int server_fd, new_socket; 
	struct sockaddr_in address;
	int addrlen = sizeof(address); 

	// Creating socket file descriptor 
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
	{ 
		perror("socket failed"); 
		exit(EXIT_FAILURE); 
	} 
	
	// Forcefully attaching socket to the port 
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
	{ 
		perror("setsockopt"); 
		exit(EXIT_FAILURE); 
	} 
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons( port ); 
	
	// Forcefully attaching socket to the port 
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 
	if (listen(server_fd, 3) < 0) 
	{ 
		perror("listen"); 
		exit(EXIT_FAILURE); 
	} 
	if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) 
	{ 
		perror("accept"); 
		exit(EXIT_FAILURE); 
	} 



	//============================Start to communicate with client=====================================
	//=================================================================================================
	int x;
	char buff[100];


	singleList groups, files, users;

	createSingleList(&groups);
	createSingleList(&files);
	createSingleList(&users);
	
	readGroupFile(&groups);
	readFileFile(&files);
	readUserFile(&users);
	

	

	// read(new_socket, buff, 100);
	// if(strcasecmp(buff, "senddeem")==0){
	// 	SendFileToClient(new_socket, "group.txt");
	// }


	
	// printf("%s\n", buff);
	// send(new_socket , "123", 4 , 0 );
	
	
	while(1){
        x = read( new_socket , buff, 100);
		REQUEST = atoi(buff);
		switch (REQUEST)
		{
		case REGISTER_REQUEST:
			printf("REGISTER_REQUEST\n");
			/* code */
			break;
		case LOGIN_REQUEST:
			// nhan username va password
			printf("LOGIN_REQUEST\n");
			while(REQUEST != LOGOUT_REQUEST){
				x = read( new_socket , buff, 100);
				REQUEST = atoi(buff);
				switch (REQUEST)
				{
				case CREATE_GROUP_REQUEST: //request code: 11
					/* code */
					printf("CREATE_GROUP_REQUEST\n");
					createGroup(new_socket, &groups);
					break;
				case JOIN_GROUP_REQUEST: //request code: 12
					/* code */
					printf("JOIN_GROUP_REQUEST\n");
					singleList un_joined_group;
					createSingleList(&un_joined_group);
					un_joined_group = unJoinedGroups(groups, users, "trung2");
					char str[200];
					convertSimpleGroupsToString(un_joined_group, str);
					send(new_socket , str, strlen(str) + 1, 0 );
					x = read( new_socket , buff, 100);
					printf("nhom da chon: %s\n", buff);
					if(addMember(groups, buff, "trung2") + addGroupToJoinedGroups(users, "trung2", buff) == 2){
						sendCode(new_socket , JOIN_GROUP_SUCCESS);
					}else{
						send(new_socket , "something wrong", 16, 0 );
					}
					break;
				case ACCESS_GROUP_REQUEST: //request code: 13
					printf("ACCESS_GROUP_REQUEST\n");
					singleList joined_group;
					createSingleList(&joined_group);
					joined_group = joinedGroups(users, "thao1");
					convertSimpleGroupsToString(joined_group, str);
					send(new_socket , str, strlen(str) + 1, 0 );
					read( new_socket , buff, 100);
					printf("nhom da chon: %s\n", buff);
					char current_group[50];
					strcpy(current_group, buff);
					sendCode(new_socket, ACCESS_GROUP_SUCCESS);
					while(REQUEST != BACK_REQUEST){
						read( new_socket , buff, 100);
						REQUEST = atoi(buff);
						switch (REQUEST)
						{
						case UPLOAD_REQUEST: //request code: 131
						/* code */
						printf("UPLOAD_REQUEST\n");
						break;
						case DOWNLOAD_REQUEST: //request code: 132
						/* code */
							printf("DOWNLOAD_REQUEST\n");
							singleList all_files;
							createSingleList(&all_files);
							all_files = getAllFilesOfGroup(groups, current_group);
							convertSimpleFilesToString(all_files, str);
							send(new_socket , str, strlen(str) + 1, 0 );
							read( new_socket , buff, 100);
							printf("file da chon: %s\n", buff);
							SendFileToClient(new_socket, buff);
							break;
						case DELETE_REQUEST: //request code: 133
						/* code */
						printf("DELETE_REQUEST\n");
						break;
						case VIEW_FILES_REQUEST: //request code: 134
						/* code */
							printf("VIEW_FILES_REQUEST\n");
							all_files = getAllFilesOfGroup(groups, current_group);
							convertSimpleFilesToString(all_files, str);
							send(new_socket , str, strlen(str) + 1, 0 );
						break;
						case BACK_REQUEST: //request code: 135
						/* code */
						printf("BACK_REQUEST\n");
						break;
						
						default:
							break;
						}
					}
					/* code */
					break;
				case LOGOUT_REQUEST: //request code: 14
					/* code */
					printf("LOGOUT_REQUEST\n");
					break;

				default:
					break;
				}
			}
			break;
		default:
			break;
		}
        
        
    }
	return 0; 
} 
