#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<netdb.h>	
#include<arpa/inet.h>
#include "functions.h"

int activateFail = 0, changePassFail = 0, loginFail = 0;
char loginUser[50];

void readFile(node **head)
{
    char username[50], password[50], homepage[100];
    int isActive, count = 0, hi;

    FILE *f = fopen("server_code/account.txt", "r");
    if (f == NULL)
    {
        printf("Error opening file\n");
        exit(0);
    }
    while (fscanf(f, "%s %s %d %s", username, password, &isActive, homepage) > 0)
    {
        node *newNode = createNode(username, password, isActive, homepage);
        insertNode(newNode, head);
        count++;
    }

    printf("_%d imported...\n", count);
}

void writeFile(node *head)
{
    node *iter = head;
    FILE *f = fopen("server_code/account.txt", "w");
    while (iter != NULL)
    {
        fprintf(f, "%s %s %d %s", iter->user->username, iter->user->password, iter->user->isActive, iter->user->homepage);
        if (iter->next != NULL)
        {
            fprintf(f, "\n");
        }
        iter = iter->next;
    }
}

int Menu()
{
    int Option = 0;
    char input[50];
    
    printf("USER MANAGEMENT PROGRAM\n");
    printf("-------------------------------------------\n");
    printf("1. Register\n");
    printf("2. Activate\n");
    printf("3. Sign in\n");
    printf("4. Search\n");
    printf("5. Change password\n");
    printf("6. Sign out\n");
    printf("7. Homepage with domain name\n");
    printf("8. Homapage with IP address\n\n");
    printf("Your choice (1-8, other to quit): ");
    
    fgets(input,100, stdin);
    if(strlen(input) > 2){
        return 0;
    }
    if(input[0]=='\n'){
        fgets(input,100, stdin);
    }
    return atoi(&input[0]);
}

void Navigator(node **head)
{

    int Option = 0;

    do
    {
        Option = Menu();
        switch (Option)
        {
        case 1:
            Register(head);
            break;
        case 2:
            Activate(*head);
            break;
        case 3:
            SignIn(*head);
            break;
        case 4:
            Search(*head);
            break;
        case 5:
            ChangePassword(*head);
            break;
        case 6:
            SignOut(*head);
            break;
        case 7:
            HomePageDN(*head);
            break;
        case 8:
            HomePageIP(*head);
            break;
        default:
            break;
        }
    } while (Option > 0 && Option < 9);
}

void Register(node **head)
{
    char username[50], password[50], homepage[100];
    node *newNode = NULL;
    printf("Username: ");
    fgets(username, 50, stdin);
    if(countSpace(username) > 0){
        printf("Username has space character\n");
        return;
    }
    if (searchNode(username, *head) != NULL)
    {
        printf("Account existed\n");
    }
    else
    {
        printf("Password: ");
        fgets(password, 50, stdin);
        if(countSpace(password) > 0){
            printf("Password has space character\n");
            return;
        }
        printf("Homepage: ");
        fgets(homepage, 50, stdin);
        if(countSpace(homepage) > 0){
            printf("Homepage has space character\n");
            return;
        }

        newNode = createNode(username, password, 2, homepage);
        insertNode(newNode, head);
        writeFile(*head);
        printf("Successful registration. Activation required.\n");
    }
};

void Activate(node *head)
{
    char username[50], password[50], code[10];
    node *target = NULL;
    printf("Username: ");
    scanf("%s", username);
    if (searchNode(username, head) == NULL)
    {
        printf("Cannot find account\n");
    }
    else
    {
        printf("Password: ");
        scanf("%s", password);
        printf("Code: ");
        scanf("%s", code);

        target = searchNode(username, head);

        if (target->user->isActive == 1)
        {
            printf("Account is activated\n");
        }
        else if (target->user->isActive == 0)
        {
            printf("Account is not activated\n");
        }
        else
        {
            if (strcmp(password, (target)->user->password) == 0)
            {
                if (strcmp(code, "LTM121216") == 0)
                {
                    (target)->user->isActive = 1;
                    printf("Account is activated\n");
                }
                else
                {
                    activateFail++;
                    if (activateFail < 3)
                    {
                        printf("Account is not activated\n");
                    }
                    else
                    {
                        (target)->user->isActive = 0;
                        activateFail = 0;
                        printf("Activation code is incorrect.\nAccount is blocked\n");
                    }
                }
            }
            else
            {
                printf("Account is not activated\n");
            }
        }
    }
};

void SignIn(node *head)
{
    char username[50], password[50];
    node *target = NULL;

    if (strlen(loginUser) > 0)
    {
        printf("Hello %s\n", loginUser);
    }
    else
    {
        printf("Username: ");
        scanf("%s", username);
        if (searchNode(username, head) == NULL)
        {
            printf("Cannot find account\n");
        }
        else
        {
            printf("Password: ");
            scanf("%s", password);

            target = searchNode(username, head);
            if (target->user->isActive == 0)
            {
                printf("Cannot find account\n");
            }
            else if (target->user->isActive == 2)
            {
                printf("Cannot find account\n");
            }
            else
            {
                if (strcmp(target->user->password, password) == 0)
                {
                    strcpy(loginUser, target->user->username);
                    printf("Hello %s\n", target->user->username);
                }
                else
                {
                    loginFail++;
                    if (loginFail < 3)
                    {
                        printf("Password is incorrect\n");
                    }
                    else
                    {
                        printf("Password is incorrect. Account is blocked\n");
                        loginFail = 0;
                        target->user->isActive = 0;
                        writeFile(head);
                    }
                }
            }
        }
    }
};

void Search(node *head)
{
    char username[50];
    int resultCount = 0;
    node* target = NULL;
    
    printf("Username: ");
    scanf("%s", username);

    target = searchNode(username, head);

    if(target == NULL){
        printf("Cannot find account\n");
    }else if(target->user->isActive == 0){
        printf("Account is blocked\n");
    }else if(target->user->isActive == 1){
        printf("Account is active\n");
    }else{
        printf("Account is not active\n");
    }
};

void ChangePassword(node *head)
{
    if (strlen(loginUser) > 0)
    {
        char username[50], password[50], newPass[50];
        node *target = NULL;
        printf("Password: ");
        scanf("%s", password);
        printf("New password: ");
        fgets(newPass, 50, stdin);
        if(countSpace(newPass) > 0){
            printf("New password contains space character\n");
            return;
        }
        target = searchNode(loginUser, head);

        if (target == NULL)
        {
            printf("Cannot find account\n");
        }
        else if (target->user->isActive == 0)
        {
            printf("Cannot find account\n");
        }
        else if (target->user->isActive == 2)
        {
            printf("Cannot find account\n");
        }
        else
        {
            if (strcmp(target->user->password, password) == 0)
            {
                strcpy(target->user->password, newPass);
                printf("Password is changed\n");
                writeFile(head);
            }
            else
            {
                changePassFail++;
                if (changePassFail < 3)
                {
                    printf("Password is incorrect\n");
                }
                else
                {
                    printf("Password is incorrect. Account is blocked\n");
                    target->user->isActive = 0;
                    changePassFail = 0;
                    writeFile(head);
                }
            }
        }
    }
    else
    {
        printf("You are not logging in\n");
    }
};

void HomePageDN(node *head){
    if(strlen(loginUser) > 0){
        node *target = searchNode(loginUser, head);
        convert(1, target->user->homepage);
    }else{
        printf("You are not logging in\n");
    }
}

void HomePageIP(node *head){
    if(strlen(loginUser) > 0){
        node *target = searchNode(loginUser, head);
        convert(2, target->user->homepage);
    }else{
        printf("You are not logging in\n");
    }
}

void SignOut(node *head)
{
    char username[50];
    node *target = NULL;

    printf("Username: ");
    scanf("%s", username);

    target = searchNode(username, head);

    if (target == NULL)
    {
        printf("Cannot find account\n");
    }
    else if (strlen(loginUser) <= 0)
    {
        printf("Account is not sign in\n");
    }
    else
    {
        if (strcmp(username, loginUser) == 0)
        {
            printf("Goodbye %s\n", loginUser);
            loginUser[0] = '\0';
        }
        else
        {
            printf("Account is not sign in\n");
        }
    }
};

int countDot(char *str){
    int i = 0, count=0;
    while(str[i] != '\0'){
        if(str[i] == '.') count++;
        i++;
    }
    return count;
}

int countSpace(char *str){
    int i = 0, count=0;
    while(str[i] != '\0'){
        if(str[i] == ' ') count++;
        i++;
    }
    return count;
}

void convert(int option , char *ipOrHostName)
{
    char **aliasList = NULL;
	char desiredValue[100];
	struct hostent *hostInfo = NULL;
    struct in_addr ** addressList = NULL;
    struct in_addr address;
	switch (option)
    {
    case 1:
        if(inet_addr(ipOrHostName) != -1 && countDot(ipOrHostName) == 3){
            inet_aton(ipOrHostName, &address);
            hostInfo = gethostbyaddr(&address, sizeof (address), AF_INET);
            if(hostInfo == NULL){
                printf("Not found information\n");
                exit(0);
            }

            aliasList = (char **)(hostInfo->h_aliases);
            addressList = (struct in_addr **)(hostInfo->h_addr_list);

            printf("Domain name: %s\n" , hostInfo->h_name);
        }else{
            printf("Domain name: %s\n", ipOrHostName);
        }
        break;
    case 2:
        if(inet_addr(ipOrHostName) == -1 || countDot(ipOrHostName) != 3){
            hostInfo = gethostbyname(ipOrHostName);

            if(hostInfo == NULL){
                printf("Not found information\n");
                exit(0);
            }

            addressList = (struct in_addr **)(hostInfo->h_addr_list);

            printf("IP address: %s\n" , inet_ntoa(*addressList[0]));
        }else{
            printf("IP address: %s\n", ipOrHostName);
        }
        break;
    default:
        printf("Error! System down\n");
        break;
    }
}

void resetBuff(){
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}