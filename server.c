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
#include <time.h>

#define BUFF_SIZE 100

int REQUEST;
singleList groups, files, users;

void readGroupFile(singleList *groups){
	
	// clear list
	deleteSingleList(groups);
	FILE *fp;
	fp = fopen("./storage/group.txt","r");
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
	fp = fopen("./storage/group.txt","a");
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
	FILE * f = fopen("./storage/user.txt","r");

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
	FILE *fp;
	char str_tmp[100];
	str_tmp[0] = '\0';
	fp = fopen("./storage/file.txt", "r");
	if(fp == NULL){
		fprintf(stderr, "File missing: can not find \"file.txt\".\n");
		exit(-1);
	}

	while(1){
		file_struct *file = (file_struct*)malloc(sizeof(file_struct));
		// name
		fgets (str_tmp, 100, fp);
		str_tmp[strlen(str_tmp)-1] = '\0';
		strcpy(file->name, str_tmp);
		// owner
		fgets (str_tmp, 100, fp);
		str_tmp[strlen(str_tmp)-1] = '\0';
		strcpy(file->owner, str_tmp);
		// group
		fgets (str_tmp, 100, fp);
		str_tmp[strlen(str_tmp)-1] = '\0';
		strcpy(file->group, str_tmp);
		// uploaded
		fgets (str_tmp, 100, fp);
		str_tmp[strlen(str_tmp)-1] = '\0';
		strcpy(file->uploaded_at, str_tmp);
		fgets (str_tmp, 100, fp);
		if(str_tmp[strlen(str_tmp)-1] == '\n'){
			str_tmp[strlen(str_tmp)-1] = '\0';
		}
		file->downloaded_times = atoi(str_tmp);
		insertEnd(files, file);
		char c = fgetc(fp);
    	if (c != EOF){
			int res = fseek( fp, -1, SEEK_CUR );
		}else
        	break;
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

void* findByName(int type, singleList list, char string[50])
{
	// type = 1 find user
	// type = 2 find group
	// type = 3 find file
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
					return list.cur->element;
				}
				
			}
			return NULL; 
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
					return list.cur->element;
				}
				
			}
			return NULL; 
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
					return list.cur->element;
				}
				
			}
			return NULL; 
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

void convertSimpleUsersToString(singleList simple_user, char str[1000]){
	str[0] = '\0';
	simple_user.cur = simple_user.root;
	while(simple_user.cur != NULL)
  	{
		strcat(str, ((simple_user_struct*)simple_user.cur->element)->user_name);
		if(simple_user.cur->next == NULL){
			str[strlen(str)] = '\0';
		}else{
			strcat(str, "+");
		}
    	simple_user.cur = simple_user.cur->next;
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

void createGroup(int sock, singleList *groups, user_struct *loginUser){
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
		strcpy(group_element->owner, loginUser->user_name);
		printf("ownner: %s\n", group_element->owner);
		group_element->files = files;
		group_element->members = members;
		group_element->number_of_files = 0;
		group_element->number_of_members = 1;

		insertEnd(groups, group_element);

		addMember(*groups, group_element->group_name, loginUser->user_name);
		
		addGroupToJoinedGroups(users, loginUser->user_name, group_element->group_name);

		strcpy(cmd, "mkdir ");
		strcat(cmd, "./files/\'");
		strcat(cmd, buff);
		strcat(cmd, "\'");
		system(cmd);

		strcpy(noti, "Nhom duoc tao thanh cong.");
		send(sock, noti, strlen(noti) + 1, 0);
	}
}

void sendCode(int sock, int code){
	char codeStr[10];
	sprintf(codeStr, "%d", code);
	printf("-->Response: %s\n", codeStr);
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

singleList getAllMembersOfGroup(singleList groups, char group_name[50]){
	singleList members;
	createSingleList(&members);
	groups.cur = groups.root;
	while (groups.cur != NULL)
	{
		if(strcmp( ((group_struct*)groups.cur->element)->group_name, group_name) == 0){
			members = ((group_struct*)groups.cur->element)->members;
			break;
		}
		groups.cur = groups.cur->next;
	}
	return members;
}

singleList getFilesOwns(singleList files, char username[50]){
	singleList files_owns;
	createSingleList(&files_owns);
	files.cur = files.root;
	while (files.cur != NULL)
	{
		if(strcmp(  ((file_struct*)files.cur->element)->owner, username) == 0){
			simple_file_struct *file_element = (simple_file_struct*) malloc(sizeof(simple_file_struct));
			strcpy(file_element->file_name, ((file_struct*)files.cur->element)->name);
			insertEnd(&files_owns, file_element);
		}
		files.cur = files.cur->next;
	}
	return files_owns;
}

void* SendFileToClient(int new_socket, char fname[50])
{
    write(new_socket, fname,256);

    FILE *fp = fopen(fname,"rb");
    if(fp==NULL)
    {
        printf("File opern error");
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
            write(new_socket, buff, nread);
        }
        if (nread < 1024)
        {
            if (feof(fp))
            {
                printf("End of file\n");
				// send(new_socket, "ENDOFFILE", strlen("ENDOFFILE"), 0);
            }
            if (ferror(fp))
                printf("Error reading\n");
            break;
        }
    }
}

void signUp(int sock, singleList *users);

int signIn(int sock, singleList users, user_struct **loginUser);

singleList getFilesCanDelete(singleList files, singleList groups, char group_name[], char username[]){
	singleList files_can_delete, all_files_of_group, all_files_owns;
	createSingleList(&files_can_delete); //file ma nguoi dung co the xoa
	createSingleList(&all_files_of_group); //tat ca file trong group
	createSingleList(&all_files_owns); //tat ca file nguoi dung so huu
	groups.cur = groups.root;
	while (groups.cur != NULL)
	{
		if( strcmp( ((group_struct*)groups.cur->element)->group_name, group_name ) == 0){
			if( strcmp( ((group_struct*)groups.cur->element)->owner, username ) == 0){
				return ((group_struct*)groups.cur->element)->files;
			}else{
				all_files_of_group = getAllFilesOfGroup(groups, group_name);
				all_files_owns = getFilesOwns(files, username);
				all_files_of_group.cur = all_files_of_group.root;
				while (all_files_of_group.cur != NULL)	
				{
					all_files_owns.cur = all_files_owns.root;
					while (all_files_owns.cur != NULL)	
					{
						if( strcmp( ((simple_file_struct*)all_files_owns.cur->element)->file_name, ((simple_file_struct*)all_files_of_group.cur->element)->file_name) == 0){
							simple_file_struct *file_element = (simple_file_struct*) malloc(sizeof(simple_file_struct));
							strcpy(file_element->file_name, ((simple_file_struct*)all_files_owns.cur->element)->file_name);
							insertEnd(&files_can_delete, file_element);
						}
						all_files_owns.cur = all_files_owns.cur->next;
					}
					all_files_of_group.cur = all_files_of_group.cur->next;
				}
			}
			break;
			
		}
		groups.cur = groups.cur->next;
	}
	
	return files_can_delete;
}

void deleteFile(singleList *files, singleList groups, char group_name[], char file_name[50]){
	//delete file in singleList files
	if( strcmp( ((file_struct*)(*files).root->element)->name, file_name) == 0 && strcmp( ((file_struct*)(*files).root->element)->group, group_name) == 0){
		deleteBegin(files);
	}else{
		(*files).cur = (*files).prev = (*files).root;
		while ((*files).cur != NULL && strcmp( ((file_struct*)(*files).cur->element)->name, file_name) != 0 && strcmp( ((file_struct*)(*files).cur->element)->group, group_name) != 0)
		{
			(*files).prev = (*files).cur;
            (*files).cur = (*files).cur->next;
		}
		node *newNode = (*files).cur;
		(*files).prev->next = (*files).cur->next;
		(*files).cur = (*files).prev;
		free(newNode);
	}
	//delete file in singleList groups
	singleList *files_of_group;
	*files_of_group = getAllFilesOfGroup(groups, group_name);
	if( strcmp( ((simple_file_struct*)(*files_of_group).root->element)->file_name, file_name) == 0){
		deleteBegin(files_of_group);
	}else{
		(*files_of_group).cur = (*files_of_group).prev = (*files_of_group).root;
		while ((*files_of_group).cur != NULL && strcmp( ((simple_file_struct*)(*files_of_group).cur->element)->file_name, file_name) != 0)
		{
			(*files_of_group).prev = (*files_of_group).cur;
            (*files_of_group).cur = (*files_of_group).cur->next;
		}
		(*files_of_group).prev->next = (*files_of_group).cur->next;
		(*files_of_group).cur = (*files_of_group).prev;
	}
}

int isFileExistInGroup(singleList groups, char group_name[], char file_name[]){
	groups.cur = groups.root;
	while(groups.cur != NULL){
		if( strcmp( ((group_struct*)groups.cur->element)->group_name, group_name) == 0){
			singleList files;
			createSingleList(&files);
			files = ((group_struct*)groups.cur->element)->files;
			files.cur = files.root;
			while (files.cur != NULL)
			{
				if( strcmp( ((simple_file_struct*)files.cur->element)->file_name, file_name) == 0){
					return 1;
				}
				files.cur = files.cur->next;
			}
			break; 
		}
		groups.cur = groups.cur->next;
	}
	return 0;
}

void uploadFile(int sock, user_struct *loginUser);

int receiveUploadedFile(int sock, char filePath[100]);


int isOwnerOfGroup(singleList groups, char group_name[], char username[]){
	groups.cur = groups.root;
	while(groups.cur != NULL){
		if( strcmp( ((group_struct*)groups.cur->element)->group_name, group_name) == 0){
			if( strcmp( ((group_struct*)groups.cur->element)->owner, username) == 0){
				return 1;
			}
		}
		groups.cur = groups.cur->next;
	}
	return 0;
}

int getAllFilesOfUserInGroup(singleList *files, char group_name[50], char username[50], char all_files[20][50]){
	int number_of_files = 0;
	(*files).cur = (*files).root;
	while ((*files).cur != NULL)
	{
		if( strcmp( ((file_struct*)(*files).cur->element)->owner, username) == 0 && strcmp( ((file_struct*)(*files).cur->element)->group, group_name) == 0){
			strcpy(all_files[number_of_files], ((file_struct*)(*files).cur->element)->name);
			number_of_files++;
		}
		(*files).cur = (*files).cur->next;
	}
	return number_of_files;
}

void kickMemberOut(singleList *files, singleList groups, char group_name[50], char username[50]){
	//delete all file of member in singleList groups
	char available_group[20][50] = {'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};
	int number_of_available_groups = getAllFilesOfUserInGroup(files, group_name, username, available_group);
	singleList files_of_group;
	files_of_group = getAllFilesOfGroup(groups, group_name);
	for(int i = 0; i < number_of_available_groups; i++){
		printf("%s\n",available_group[i]);
		if( strcmp( ((simple_file_struct*)files_of_group.root->element)->file_name, available_group[i]) == 0){
			files_of_group.root = files_of_group.root->next;
			((group_struct*)groups.root->element)->files = files_of_group;
			printf("ok\n");
		}else{
			files_of_group.cur = files_of_group.prev = files_of_group.root;
			while (files_of_group.cur != NULL && strcmp( ((simple_file_struct*)files_of_group.cur->element)->file_name, available_group[i]) != 0)
			{
				files_of_group.prev = files_of_group.cur;
				files_of_group.cur = files_of_group.cur->next;
			}
			files_of_group.prev->next = files_of_group.cur->next;
			files_of_group.cur = files_of_group.prev;
		}
	}
	
	// delete file in singleList files
	(*files).cur = (*files).root;
	while ((*files).cur != NULL)
	{
		if( strcmp( ((file_struct*)(*files).root->element)->owner, username) == 0 && strcmp( ((file_struct*)(*files).root->element)->group, group_name) == 0){
			deleteBegin(files);
		}else{
			while ((*files).cur != NULL && strcmp( ((file_struct*)(*files).cur->element)->owner, username) != 0 && strcmp( ((file_struct*)(*files).root->element)->group, group_name) != 0)
			{
				(*files).prev = (*files).cur;
				(*files).cur = (*files).cur->next;
			}
			if((*files).cur != NULL){
				node *newNode = (*files).cur;
				(*files).prev->next = (*files).cur->next;
				(*files).cur = (*files).prev;
				free(newNode);
			}
		}
		if((*files).cur == NULL) break;
		(*files).cur = (*files).cur->next;
	}

	//delete user in singleList groups
	singleList members;
	createSingleList(&members);
	members = getAllMembersOfGroup(groups, group_name);
	if( strcmp(username, ((simple_user_struct*)members.root->element)->user_name) == 0){
		members.root = members.root->next;
		members.cur = members.cur->next;
	}else{
		members.cur = members.prev = members.root;
		while (members.cur != NULL && strcmp( ((simple_user_struct*)members.cur->element)->user_name, username) != 0)
		{
			members.prev = members.cur;
            members.cur = members.cur->next;
		}
		node *newNode = members.cur;
		members.prev->next = members.cur->next;
		members.cur = members.prev;
		free(newNode);
	}
}

void sortBySize(singleList *files){

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

	createSingleList(&groups);
	createSingleList(&files);
	createSingleList(&users);
	
	readGroupFile(&groups);
	readFileFile(&files);
	readUserFile(&users);
	
	while(1){
		user_struct *loginUser = NULL;
        x = read( new_socket , buff, 100);
		REQUEST = atoi(buff);
		switch (REQUEST)
		{
		case REGISTER_REQUEST:
			printf("REGISTER_REQUEST\n");
			signUp(new_socket, &users);
			saveUsers(users);
			break;
		case LOGIN_REQUEST:
			// nhan username va password
			printf("LOGIN_REQUEST\n");
			if(signIn(new_socket, users, &loginUser) == 1){
				while(REQUEST != LOGOUT_REQUEST){
					x = read( new_socket , buff, 100);
					REQUEST = atoi(buff);
					switch (REQUEST)
					{
					case CREATE_GROUP_REQUEST: //request code: 11
						/* code */
						printf("CREATE_GROUP_REQUEST\n");
						createGroup(new_socket, &groups, loginUser);
						break;
					case JOIN_GROUP_REQUEST: //request code: 12
						printf("JOIN_GROUP_REQUEST\n");
						singleList un_joined_group;
						createSingleList(&un_joined_group);
						un_joined_group = unJoinedGroups(groups, users, loginUser->user_name);
						char str[200];
						convertSimpleGroupsToString(un_joined_group, str);
						send(new_socket , str, strlen(str) + 1, 0 );
						x = read( new_socket , buff, 100);
						printf("nhom da chon: %s\n", buff);
						if(addMember(groups, buff, loginUser->user_name) + addGroupToJoinedGroups(users, loginUser->user_name, buff) == 2){
							sendCode(new_socket , JOIN_GROUP_SUCCESS);
						}else{
							send(new_socket , "something went wrong", 21, 0 );
						}
						break;
					case ACCESS_GROUP_REQUEST: //request code: 13
						printf("ACCESS_GROUP_REQUEST\n");
						singleList joined_group;
						createSingleList(&joined_group);
						joined_group = joinedGroups(users, loginUser->user_name);
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
									printf("UPLOAD_REQUEST\n");
									uploadFile(new_socket, loginUser);
									break;
								case DOWNLOAD_REQUEST: //request code: 132
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
									printf("DELETE_REQUEST\n");
									singleList files_can_delete;
									createSingleList(&files_can_delete);
									files_can_delete = getFilesCanDelete(files, groups, current_group ,loginUser->user_name);
									convertSimpleFilesToString(files_can_delete, str);
									send(new_socket , str, strlen(str) + 1, 0 );
									read( new_socket , buff, 100);
									printf("file da chon: %s\n", buff);
									deleteFile(&files, groups, current_group, buff);
									singleList test;
									createSingleList(&test);
									test = getAllFilesOfGroup(groups, current_group);
									printFile(test);
									break;
								case VIEW_FILES_REQUEST: //request code: 134
								/* code */
									printf("VIEW_FILES_REQUEST\n");
									all_files = getAllFilesOfGroup(groups, current_group);
									convertSimpleFilesToString(all_files, str);
									send(new_socket , str, strlen(str) + 1, 0 );
									break;
								case KICK_MEMBER_REQUEST:
									printf("KICK_MEMBER_REQUEST\n");
									if(isOwnerOfGroup(groups, current_group,loginUser->user_name) == 0){
										sendCode(new_socket, NOT_OWNER_OF_GROUP);
									}else{
										singleList members;
										createSingleList(&members);
										members = getAllMembersOfGroup(groups, current_group);
										convertSimpleUsersToString(members, str);
										send(new_socket, str, strlen(str)+1, 0);
										read(new_socket, buff, 100);
										printf("group = %s, member = %s\n", current_group, buff);
										kickMemberOut(&files,groups, current_group, buff);
										singleList members1;
										createSingleList(&members1);
										members1 = getAllMembersOfGroup(groups, current_group);
										printUser(members1);
									}
									
									break;
								case BACK_REQUEST: //request code: 135
								/* code */
								printf("BACK_REQUEST\n");
									break;
								default:
									break;
							}
						}
						break;
					case LOGOUT_REQUEST: //request code: 14
						printf("LOGOUT_REQUEST\n");
						loginUser = NULL;
						sendCode(new_socket, LOGOUT_SUCCESS);
						break;

					default:
						break;
					}
				}
			}
			break;
		default:
			break;
		}
        
        
    }
	return 0; 
} \

void signUp(int sock, singleList *users){
	char buff[BUFF_SIZE], username[50], password[50];
	int size;
	sendCode(sock, REGISTER_SUCCESS);

	while(1){
		size = read(sock, buff, BUFF_SIZE);

		strcpy(username, buff);
		username[strlen(username) - 1] = '\0';
		if(username[strlen(username) - 2] == '\n'){
			username[strlen(username) - 2] = '\0';
		}
		printf("username: \'%s\'\n", username);
		if(checkExistence(1, *users, username) == 1){
			sendCode(sock, EXISTENCE_USERNAME);
		}else{
			sendCode(sock, REGISTER_SUCCESS);
			break;
		}
	}
	singleList groups;
	createSingleList(&groups);

	read(sock, buff, BUFF_SIZE);
	buff[strlen(buff) - 1] = '\0';
	if(buff[strlen(buff) - 2] == '\n'){
		buff[strlen(buff) - 2] = '\0';
	}
	printf("password: %s\n", buff);
	
	strcpy(password, buff);
	user_struct *user = (user_struct*)malloc(sizeof(user_struct));
	strcpy(user->user_name, username);
	strcpy(user->password, password);
	user->count_group = 0;
	user->status = 1;
	user->joined_groups = groups;
	insertEnd(users, user);

	sendCode(sock, REGISTER_SUCCESS);
}

int signIn(int sock, singleList users, user_struct **loginUser){
	char buff[BUFF_SIZE], username[50], password[50];

	sendCode(sock, LOGIN_SUCCESS);

	while(1){
		read(sock, buff, BUFF_SIZE);
		buff[strlen(buff) - 1] = '\0';
		printf("username: %s\n", buff);

		strcpy(username, buff);
		if(checkExistence(1, users, username) == 1){
			sendCode(sock, LOGIN_SUCCESS);
			break;
		}else{
			sendCode(sock, NON_EXISTENCE_USERNAME);
		}
	}
	read(sock, buff, BUFF_SIZE);
	buff[strlen(buff) - 1] = '\0';
	printf("password: %s\n", buff);
	strcpy(password, buff);

	*loginUser = (user_struct*)(findByName(1, users, username));
	if(strcmp((*loginUser)->password, password) == 0){
		sendCode(sock, LOGIN_SUCCESS);
		return 1;
	}
	sendCode(sock, INCORRECT_PASSWORD);
	return 0;
}

void uploadFile(int sock, user_struct *loginUser){
	char buff[50], filePath[100], group_name[50], file_name[50], today[50];

	sendCode(sock, UPLOAD_SUCCESS);

	read(sock, buff, BUFF_SIZE);
	strcpy(group_name, buff);

	read(sock, buff, BUFF_SIZE);
	buff[strlen(buff) - 1] = '\0';
	strcpy(file_name, buff);

	filePath[0] = '\0';
	strcat(filePath, "./files/");
	strcat(filePath, group_name);
	strcat(filePath, "/");
	strcat(filePath, file_name);

	receiveUploadedFile(sock, filePath);

	// get date of upload
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	sprintf( today, "%02d-%02d-%d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);

	file_struct *file = (file_struct*)malloc(sizeof(file_struct));
	strcpy(file->name, file_name);
	strcpy(file->group, group_name);
	strcpy(file->owner, loginUser->user_name);
	strcpy(file->uploaded_at, today);
	file->downloaded_times = 0;

	insertEnd(&files, file);

	singleList fileOfGroup = getAllFilesOfGroup(groups, group_name);

	simple_file_struct *file_element = (simple_file_struct*) malloc(sizeof(simple_file_struct));
	strcpy(file_element->file_name, file_name);

	insertEnd(&fileOfGroup, file_element); 

	saveFiles(files);
}

int receiveUploadedFile(int sock, char filePath[100]){
	int bytesReceived = 0;
	char recvBuff[1024], fname[100], path[100];
	FILE *fp;

	printf("Receiving file...\n");

	printf("Uploaded file: %s\n", filePath);
	fp = fopen(filePath, "ab"); 
	if(NULL == fp)
	{
		printf("Error opening file\n");
		return -1;
	}
	
	double sz=1;
	/* Receive data in chunks of 256 bytes */
	while((bytesReceived = read(sock, recvBuff, 1024)) > 0)
	{ 
		printf("\n\n\nbytes = %d\n",bytesReceived);
		sz++;
		printf("Received: %lf Mb\n",(sz/1024));
		fflush(stdout);
		fwrite(recvBuff, 1,bytesReceived,fp);

		if(bytesReceived < 1024){
			break;
		}
	}
	fclose(fp);
	if(bytesReceived < 0)
	{
		printf("\n Read Error \n");
		return -1;
	}
	
	printf("\nFile OK....Completed\n");
	return 1;
}