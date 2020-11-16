#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linked_list.h"

node *initNode()
{
    node *head;
    head = NULL;
    return head;
}

node *createNode(char *username, char *password, int isActive, char *homepage)
{
    node *temp;
    temp = (node *)malloc(sizeof(node));
    temp->user = (user_info *)malloc(sizeof(user_info));
    strcpy(temp->user->username, username);
    strcpy(temp->user->password, password);
    temp->user->isActive = isActive;
    strcpy(temp->user->homepage, homepage);
    temp->next = NULL;
    return temp;
}

void printList(node *head)
{
    int i = 1;
    if (head == NULL)
    {
        printf("No on in list\n");
    }
    else
    {
        node *iter = head;
        printf("STT\t %-20s\t %-10s\t %-20s Status\n", "Username", "Password", "homepage");
        while (iter != NULL)
        {
            printf("%d\t %-20s\t %-10s\t %-20s %d\n", i++, iter->user->username, iter->user->password, iter->user->homepage, iter->user->isActive);
            iter = iter->next;
        }
    }
}

node *searchNode(char *keyword, node *head)
{
    node *iter = head;
    while (iter != NULL)
    {
        if (strcmp(iter->user->username, keyword) == 0)
        {
            return iter;
        }
        iter = iter->next;
    }

    return NULL;
}

void insertNode(node *newNode, node **head)
{
    if (*head == NULL)
    {
        *head = newNode;
    }
    else
    {
        insertNode(newNode, &((*head)->next));
    }
}

void freeList(node *head)
{
    free(head->user);
    if(head->next != NULL){
        freeList(head->next);
    }
    free(head);
}
