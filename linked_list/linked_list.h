typedef struct
{
    char username[50];
    char password[50];
    int isActive;
    char homepage[100];
} user_info;

typedef struct node
{
    user_info *user;
    struct node *next;
} node;

node *initNode();

node *createNode(char *username, char *password, int isActive, char *homepage);

void removeNodeAtIndex(int index);

node* searchNode(char *keyword, node *head);

node *getNode(int index);

void insertNode(node *newNode, node **head);

int listLength();

void freeList();

void printList(node *head);
