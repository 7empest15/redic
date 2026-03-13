#ifndef NETWORK_H
#define NETWORK_H

#include "cache.h"

#define PORT 6379
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 100

typedef struct ClientCtx
{
    int fd;
    int is_auth;
    char buffer[BUFFER_SIZE];
} ClientCtx;

void network_start_server(Cache *cache);

int network_handle_client(int client_socket, Cache *cache);

#endif