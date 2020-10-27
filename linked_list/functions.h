#include "linked_list.h"

void readFile();

void writeFile();

int Menu();

void Navigator(node **head);

void Register(node **head);

void Activate(node *head);

void SignIn(node *head);

void Search(node *head);

void ChangePassword(node *head);

void HomePageDN();

void HomePageIP();

void SignOut(node *head);

void convert(int option , char *ipOrHostName);

int countDot(char *str);

int countSpace(char *str);

void resetBuff();