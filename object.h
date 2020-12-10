
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

typedef struct file {
    char name[50];
    char owner[50];
    char create_at[50];
    int downloaded_times;
    int status;
}file_struct;

typedef struct group{
    char name[50];
    char owner[50];
    singleList members;
    singleList files;
    int status;
}group_struct;