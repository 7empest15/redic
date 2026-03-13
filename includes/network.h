#ifndef NETWORK_H
#define NETWORK_H

#include "cache.h"

#define PORT 6379
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 100

void network_start_server(t_cache *cache);

void network_handle_client(int client_socket, t_cache *cache);

#endif