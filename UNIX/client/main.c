#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>

int main()
{
	int socketFD = socket(AF_INET, SOCK_STREAM, 0);
	// domain: AF_NET TYPE = iPv4 SOCK_STREAM = TCP PROTOCOL: 0 //auto == tcp
	if (socketFD < 0)
	{
		perror("socket");
		return 1;
	}
	char *ip = "127.0.0.1";
	printf("ip = %s\n", ip);
	struct sockaddr_in address; // address
	memset(&address, 0, sizeof(address));
	// Setup the address structure
	address.sin_family = AF_INET;	// Use IPv4
	address.sin_port = htons(8080); // Use port 8080
	// Use IP address 127.0.0.1
	inet_pton(AF_INET, ip, &address.sin_addr);

	if (connect(socketFD, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		perror("connect");
		printf("Error code: %d\n", errno);
		close(socketFD);
		return 1;
	}

	printf("Connected successfully!\n");

	close(socketFD);
	return 0;
}
