#ifndef CHAT_H
#define CHAT_H

#include "types.h"

int sendAll(message *msg, int socketFD, connectionInfo *clients[]);

void addClient(connectionInfo *clients[], int clientFD, struct sockaddr_in address);

#endif // CHAT_H