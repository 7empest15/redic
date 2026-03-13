#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>
#include "network.h"

void network_start_server(t_cache *cache)
{
    int server_fd;
    struct pollfd fds[MAX_CLIENTS];
    int nfds = 1;

    struct sockaddr_in address;
    int opt = 1;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 3);

    printf("Server started on port %d\n", PORT);

    fds[0].fd = server_fd;
    fds[0].events = POLLIN;

    for (int i = 1; i < MAX_CLIENTS; i++) fds[i].fd = -1;

    while (1) {
        int ret = poll(fds, nfds, -1);
        if (ret < 0) break;

        for (int i = 0; i < nfds; i++) {
            if (fds[i].revents == 0) continue;

            if (fds[i].fd == server_fd) {
                int new_client = accept(server_fd, NULL, NULL);
                if (nfds < MAX_CLIENTS) {
                    fds[nfds].fd = new_client;
                    fds[nfds].events = POLLIN;
                    nfds++;
                } else {
                    close(new_client);
                }
            } 
            else {
                network_handle_client(fds[i].fd, cache);
                close(fds[i].fd);
                fds[i].fd = -1;
            }
        }
    }
}

void network_handle_client(int client_socket, t_cache *cache)
{
    char buffer[BUFFER_SIZE] = {0};
    read(client_socket, buffer, BUFFER_SIZE);

    char cmd[10], key[50], val[50];
    sscanf(buffer, "%s %s %s", cmd, key, val);

    if (strcmp(cmd, "SET") == 0) {
        cache_set(cache, key, val);
        send(client_socket, "OK\n", 3, 0);
    } else if (strcmp(cmd, "GET") == 0) {
        char *value = cache_get(cache, key);
        if (value) {
            send(client_socket, value, strlen(value), 0);
        } else {
            send(client_socket, "NULL\n", 5, 0);
        }
    } else {
        send(client_socket, "ERR unknown command\n", 20, 0);
    }
    close(client_socket);
}