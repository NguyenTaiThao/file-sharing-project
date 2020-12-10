typedef struct node node;

typedef struct node {
	void *element;
	node *next;
}node;

typedef struct singleList{
  node * root;
  node * cur;
  node * prev;
  node * tail;
} singleList;


typedef struct user
{
	char user_name[20];
	char password[20];
    singleList joined_groups;
	int status;
}user_struct;

typedef struct simple_user
{
	char user_name[20];
}simple_user_struct;

typedef struct file {
    char name[50];
    char owner[50];
    char create_at[50];
    int downloaded_times;
    int status;
}file_struct;

typedef struct simple_file {
    char file_name[50];
}simple_file_struct;

typedef struct simple_group {
    char file_group[50];
}simple_group_struct;
typedef struct group{
    char group_name[50];
    char owner[50];
    singleList members;
    singleList files;
    int number_of_members;
    int number_of_files;
}group_struct;