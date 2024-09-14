#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include "types.h"

int sendAll(message *msg, int socketFD, connectionInfo *clients[])
{
	int i;
	pthread_mutex_lock(&mutex);

	for (i = 0; i < MAX_CLIENTS; i++)
	{
		if (clients[i])
		{
			ssize_t bytesSent = send(clients[i]->socket, msg, strlen(msg), 0);
			if (bytesSent < 0)
			{
				perror("sendAll: send() failed");
				pthread_mutex_unlock(&mutex);
				return 1;
			}
			else if (bytesSent < strlen(msg))
			{
				printf("sendAll: Not all data sent\n");
				pthread_mutex_unlock(&mutex);
				return 1;
			}
		}
	}

	pthread_mutex_unlock(&mutex);
	return 0;
}

void addClient(connectionInfo *clients[], int clientFD, struct sockaddr_in address)
{
	pthread_mutex_lock(&mutex);
	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		if (clients[i] == NULL)
		{
			clients[i] = malloc(sizeof(connectionInfo));
			clients[i]->socket = clientFD;
			clients[i]->address = address;
			break;
		}
	}
	pthread_mutex_unlock(&mutex);
}
void removeClient(connectionInfo *clients[], int clientFD)
{
	pthread_mutex_lock(&mutex);
	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		if (clients[i] != NULL && clients[i]->socket == clientFD)
		{
			close(clients[i]->socket);
			free(clients[i]);
			clients[i] = NULL;
			break;
		}
	}
	pthread_mutex_unlock(&mutex);
}