#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include "types.h"
#include "chat.h"

#define BUFFER_SIZE 2048

static unsigned char finish = 0;

void error(const char *msg)
{
	perror(msg);
	printf("Error code: %d\n", errno);
	exit(EXIT_FAILURE);
}

static void handleSignal(int signal __attribute__((unused)))
{
	finish = 1;
}

int createIPV4Addr(connectionInfo *address, const char *ip, int port)
{
	if (!address)
	{
		errno = EINVAL;
		error("createIPV4Addr: address is NULL");
	}

	address->address.sin_family = AF_INET;
	address->address.sin_port = htons(port);
	int result = inet_pton(AF_INET, ip, &address->address.sin_addr);

	if (result < 0)
		error("createIPV4Addr: inet_pton failed");
	else if (result == 0)
	{
		errno = EINVAL;
		error("createIPV4Addr: inet_pton returned 0");
	}

	return 0;
}

int createTCPSocket(int domain, int type, int protocol)
{
	int socketFD = socket(domain, type, protocol);
	if (socketFD < 0)
	{
		perror("socket");
		printf("Error code: %d\n", errno);
	}

	return socketFD;
}

void *handleClient(void *arg)
{
	int clientFD = *(int *)arg;
	free(arg);

	char buffer[BUFFER_SIZE];
	while (1)
	{
		int bytesRead = read(clientFD, buffer, sizeof(buffer) - 1);
		if (bytesRead <= 0)
		{

			close(clientFD);
			pthread_exit(NULL);
		}
		buffer[bytesRead] = '\0';
		printf("Received message: %s\n", buffer);
	}
}

void setupSignalHandling()
{
	struct sigaction action;
	memset(&action, 0, sizeof(action));
	action.sa_handler = handleSignal;
	action.sa_flags = 0;
	sigemptyset(&action.sa_mask);
	sigaction(SIGINT, &action, NULL);
}

void setupServer(int *serverFD, connectionInfo *connInfo)
{
	if ((*serverFD = createTCPSocket(AF_INET, SOCK_STREAM, 0)) < 0)
		exit(EXIT_FAILURE);

	if (createIPV4Addr(connInfo, "127.0.0.1", PORT) != 0)
	{
		close(*serverFD);
		exit(EXIT_FAILURE);
	}

	if (bind(*serverFD, (struct sockaddr *)&connInfo->address, sizeof(connInfo->address)) < 0)
	{
		error("bind");
		close(*serverFD);
		exit(EXIT_FAILURE);
	}

	if (listen(*serverFD, MAX_CLIENTS) < 0)
	{
		error("listen");
		close(*serverFD);
		exit(EXIT_FAILURE);
	}
}

void runServerLoop(int serverFD, connectionInfo *clients[])
{
	printf("Server is listening on 127.0.0.1:%d\n", PORT);

	while (!finish)
	{
		struct sockaddr_in clientAddr;
		socklen_t clientAddrLen = sizeof(clientAddr);
		int clientFD = accept(serverFD, (struct sockaddr *)&clientAddr, &clientAddrLen);
		if (clientFD < 0)
		{
			if (finish)
				break;
			error("accept");
		}

		printf("New client connected! : %d\n", clientFD);

		addClient(clients, clientFD, clientAddr);

		pthread_t thread;
		int *clientFDPtr = malloc(sizeof(int));
		*clientFDPtr = clientFD;
		if (pthread_create(&thread, NULL, handleClient, clientFDPtr) != 0)
		{
			error("pthread_create");
		}
		pthread_detach(thread);
	}
}

void notifyClients(connectionInfo *clients[])
{
	pthread_mutex_lock(&mutex);
	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		if (clients[i] != NULL)
		{
			const char *shutdownMsg = "Server is shutting down.\n";
			send(clients[i]->socket, shutdownMsg, strlen(shutdownMsg), 0);
		}
	}
	pthread_mutex_unlock(&mutex);
}

void gracefulExit(int serverFD, connectionInfo *clients[], int clientCount)
{
	printf("Shutting down server...\n");

	notifyClients(clients);

	for (int i = 0; i < clientCount; i++)
	{
		if (clients[i] != NULL)
		{
			close(clients[i]->socket);
			free(clients[i]);
		}
	}

	close(serverFD);
	printf("Server shutdown complete.\n");
}
int main(void)
{
	setupSignalHandling();

	connectionInfo *clients[MAX_CLIENTS] = {0};

	int serverFD;
	connectionInfo address;
	setupServer(&serverFD, &address);

	runServerLoop(serverFD, clients);

	gracefulExit(serverFD, clients, MAX_CLIENTS);

	return 0;
}
