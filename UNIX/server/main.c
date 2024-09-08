#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
void error(char *msg)
{
	perror(msg);
	printf("Error code: %d\n", errno);
	exit(1);
}
int createIPV4Addr(struct sockaddr_in *address, char *ip, int port)
{
	if (address == NULL)
	{
		errno = EINVAL;
		error("createIPV4Addr: address is NULL");
		return 1;
	}
	address->sin_family = AF_INET;
	address->sin_port = htons(port);
	int result = inet_pton(AF_INET, ip, &address->sin_addr);
	if (result < 0)
	{
		error("createIPV4Addr: inet_pton failed");
		return 1;
	}
	else if (result == 0)
	{
		errno = EINVAL;
		error("createIPV4Addr: inet_pton returned 0");
		return 1;
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
		return -1;
	}
	return socketFD;
}

int main()
{
	int serverFD = createTCPSocket(AF_INET, SOCK_STREAM, 0);
	if (serverFD < 0)
	{
		return 1;
	}

	struct sockaddr_in address;
	createIPV4Addr(&address, "127.0.0.1", 8080);

	if (bind(serverFD, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		error("bind");
		close(serverFD);
		return 1;
	}

	if (listen(serverFD, 10) < 0)
	{
		error("listen");
		close(serverFD);
		return 1;
	}

	printf("Server is listening on 127.0.0.1:8080\n");

	while (1)
	{
		struct sockaddr_in clientAddr;
		socklen_t clientAddrLen = sizeof(clientAddr);
		int clientFD = accept(serverFD, (struct sockaddr *)&clientAddr, &clientAddrLen);
		if (clientFD < 0)
		{
			error("accept");
			close(serverFD);
			return 1;
		}

		printf("Client connected!\n");

		char buffer[1024];
		ssize_t bytesRead;

		while ((bytesRead = recv(clientFD, buffer, sizeof(buffer) - 1, 0)) > 0)
		{
			buffer[bytesRead] = '\0';
			printf("Received: %s\n", buffer);
		}

		if (bytesRead < 0)
		{
			error("recv");
		}
		else
		{
			printf("Client disconnected.\n");
		}

		close(clientFD);
	}

	close(serverFD);
	return 0;
}