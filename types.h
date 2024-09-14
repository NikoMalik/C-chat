#ifndef TYPES_H
#define TYPES_H

#define MAX_CLIENTS 30

#define PORT 8080

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef enum
{
	CONNECT,
	DISCONNECT,
	GET_USERS,
	SET_USERNAME,
	PUBLIC_MESSAGE,
	PRIVATE_MESSAGE,
	TOO_FULL,
	USERNAME_ERROR,
	SUCCESS,
	ERROR
} message_type;

typedef struct
{
	message_type type;
	char message[1000];
	char username[48];
} message;

typedef struct connectionInfo
{
	int socket;
	struct sockaddr_in address;
	char username[48];
} connectionInfo;

#endif // TYPES_H