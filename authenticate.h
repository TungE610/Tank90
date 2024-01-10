#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>

typedef struct
{
	char username[256];
	char password[256];
	int status;
	int logged;
	int id;
} ListElementType;
#define BUFF_SIZE 1024
#include "linkedlist.h"

void readDatatoList(singleLinkedList *list);
void appendAccountToFile(char accountname[], char password[], int id);
void blockUser(singleLinkedList *list, char username[]);
int checkBlock(singleLinkedList *list, char username[]);
void rewriteFile(singleLinkedList *list);
int registerUser(singleLinkedList *list, char *username, char * password, int id);
void signin(singleLinkedList *list, char *signedInUser);
void signout(singleLinkedList *list, char *signedInUser);
void searchUser(singleLinkedList *list);
int accountNum(singleLinkedList *list);
/*
	readDatatoList: read data from file to list
*/
void readDatatoList(singleLinkedList *list) {
    ListElementType element;
    deleteSingleList(list);  // Clear the list

    FILE *fp = fopen("account.txt", "r");

    if (fp == NULL) {
        perror("Error opening account file");
        return;
    }

    while (fscanf(fp, "%s %s %d %d %d", element.username, element.password, &element.status, &element.logged, &element.id) == 5) {
        insertEnd(list, element);
    }

    fclose(fp);
}

/*
	checkExisted: check if username existed in account file
*/

node* findAccount(singleLinkedList *list, char username[]) {

	readDatatoList(list);									//read data to list
	list->cur = list->root;

	while (list->cur != NULL) {													// traverse till find out matching account

		if (strcmp(list->cur->element.username, username) == 0) { 				// if found return username
			return list->cur;
		}

		list->cur = list->cur->next;
	}
	return NULL; 																// Not found return NULL
}

/*
	authenicate: check username and password is true
	true: return 1
	false: return 0
*/

char* authenticate(singleLinkedList *list, char *username, char *password) {
	readDatatoList(list);

	list->cur = list->root;

	while (list->cur != NULL) {									// traverse till find out account with matching username and password

		if ((strcmp(list->cur->element.username, username) == 0) && (strcmp(list->cur->element.password, password) == 0)) {
			return list->cur->element.username;					// if find out return this user's username
		}

		list->cur = list->cur->next;
	}

	return "";													// not found return empty string
}

/*
	checkBlock: check if existed user's state is block (0)
	blocked returns 0
	active return 1
*/

int checkBlock(singleLinkedList *list, char username[]) {
	list->cur = list->root;

	while (list->cur != NULL) {

		if (strcmp(list->cur->element.username, username) == 0) {

			if (list->cur->element.status == 1) { 					// case: user is active returns 1
				return 1;
			} else {
				return 0; 											// case: user is active returns 0
			}
		}

		list->cur = list->cur->next;
	}

	printf("Some things wen't wrong. Not found user in system !");
	return -1;													    // case: not found this user, return -1
}

// Hash a password using SHA-256
void hashPassword(const char *password, char *hashedPassword) {
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, password, strlen(password));
    unsigned char digest[SHA256_DIGEST_LENGTH];
    SHA256_Final(digest, &sha256);

    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(hashedPassword + (i * 2), "%02x", digest[i]);
    }

    hashedPassword[SHA256_DIGEST_LENGTH * 2] = '\0'; // Null terminator
}

// Ham giai ma hash
int validatePassword(const char *inputPassword, const char *hashedPassword) {
    char hashedInput[SHA256_DIGEST_LENGTH];

	// hash chuoi duoc nhap vao 
    hashPassword(inputPassword, hashedInput);

    // so sanh chuoi hash duoc tao voi chuoi hash duoc luu tru
    return memcmp(hashedInput, hashedPassword, SHA256_DIGEST_LENGTH) == 0;
}

/*
	appendAccountToFile: write new user to account file
*/

void appendAccountToFile(char username[], char password[], int id) {
	FILE *fp = fopen("account.txt", "a+"); 							// open file with append mode
	
	if (fp == NULL) {
		perror("Data file not found");
		return;
	}

	char hashedPassword[SHA256_DIGEST_LENGTH * 2 + 1];
    hashPassword(password, hashedPassword);

	fprintf(fp, "\n%s %s 1 0 %d", username, hashedPassword, id); 					// new user's state is active

	fclose(fp); 													// close file
}

void blockUser(singleLinkedList *list, char username[]) {

	//readDatatoList(list);
	list->cur = list->root;

	while (list->cur != NULL) {										//traverse till find out user which has username matching

		if (strcmp(list->cur->element.username, username) == 0) {
			list->cur->element.status = 0;
			break;
		}

		list->cur = list->cur->next;
	}

	rewriteFile(list);												//rewrite file to update active state
	printf("Account %s is blocked", username);
}

void userIsLoggin(singleLinkedList *list, char username[]) {

	list->cur = list->root;
	
	while (list->cur != NULL) {										//traverse till find out user which has username matching

		if (strcmp(list->cur->element.username, username) == 0) {
			list->cur->element.logged = 1;
			break;
		}

		list->cur = list->cur->next;
	}

	rewriteFile(list);									
}

void logoutUser(singleLinkedList *list, char username[]) {

	//readDatatoList(list);
	list->cur = list->root;

	while (list->cur != NULL) {										//traverse till find out user which has username matching

		if (strcmp(list->cur->element.username, username) == 0) {
			list->cur->element.logged = 0;
			break;
		}

		list->cur = list->cur->next;
	}

	rewriteFile(list);												//rewrite file to update active state
}

/*
	rewriteFile: rewrite all data file
*/
void rewriteFile(singleLinkedList *list) {
	list->cur = list->root;

	FILE *fp = fopen("account.txt", "w+");

	if (fp == NULL) {
		perror("Data file not found");
		return;
	}

	while (list->cur != NULL) {									// traverse till end to rewrite
		fprintf(fp, "%s %s %d %d %d \n", list->cur->element.username, list->cur->element.password, list->cur->element.status, list->cur->element.logged, list->cur->element.id);
		list->cur = list->cur->next;
	}

	fclose(fp);														//close file
};

int registerUser(singleLinkedList *list, char *username, char*password, int id) {
	if (findAccount(list, username)) { 							// check if username existed in account file
		return 0;
	} else {
		//readDatatoList(list);							// save new user infomation to file
		appendAccountToFile(username, password, id); 	
		return 1;
	}

	return 2;
}

void signin(singleLinkedList* list, char* signedInUser) {
	char username[30], password[30];
	while ((getchar()) != '\n');

	readDatatoList(list);										//read data to list

	printf("\n---------Sign In---------\n");
	printf("Username : ");
	scanf("%s", username);

	if (findAccount(list, username)){							// find account in data list

		if (checkBlock(list, username) == 1) {					// check block status of found username
			int wrongPasswordCount = 0;							// input wrong password times

			do {
				while ((getchar()) != '\n');

				printf("Password: ");							//input password
				scanf("%s", password);

				if (strlen(authenticate(list, username, password)) > 0) {	//authenticate
					printf("Hello %s", username);
					strcpy(signedInUser, authenticate(list, username, password));	//update signedInUser
					break;
				} else {
					wrongPasswordCount++;						//increase input wrong password times
				}

			} while (wrongPasswordCount < 3);

			if (wrongPasswordCount == 3) {						// if input wrongs password more than 3 times, block user
				blockUser(list, username);
			}
		} else {
			printf("Account is blocked");
		}
	} else {
		printf("Cannot find account");
	}
}

void signout(singleLinkedList* list, char* signedInUser) {
	char username[30];
	while ((getchar()) != '\n');
	readDatatoList(list);
	printf("\n---------Signout---------\n");
	printf("Username: ");
	scanf("%s", username);

	if (findAccount(list, username)) {
		if (strcmp(signedInUser, username) == 0) {
			printf("Goodbye %s\n", username);
			strcpy(signedInUser, "");						// update signedInUser
		} else {
			printf("Cannot find account");					//input username not matching with logined username
		}
	} else {												// not found user
		printf("Cannot find account");
	}
}

void searchUser(singleLinkedList *list)
{
	char username[30];

	while ((getchar()) != '\n');

	readDatatoList(list);									// read data file to list

	printf("\n---------Search---------\n");
	printf("Username : ");
	scanf("%s", username);

	node *foundUser = findAccount(list, username);			// find account

	if (foundUser) {

		if (foundUser->element.status == 1) {				// find status of the account
			printf("Account is active");
		} else {
			printf("Account is blocked");
		}
	} else {
		printf("Cannot find account");
	}
}

int accountNum(singleLinkedList *list){
	int count = 0;
	list->cur = list->root;

	while (list->cur != NULL) {									// traverse till end to rewrite
		count++;
	}

	return count;
}
