#ifndef TYPES_H
#define TYPES_H

#define MAX_CLIENTS 30

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
	char username[20];
} message;

#endif // TYPES_H