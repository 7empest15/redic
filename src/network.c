#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/sha.h>
#include <poll.h>
#include "network.h"
#include "auth.h"
#include <stdbool.h>

typedef struct {
    int fd;
    bool authenticated;
} ClientState;

ClientState client_states[MAX_CLIENTS];

static void initialize_client_states() {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_states[i].fd = -1;
        client_states[i].authenticated = false;
    }
}

static ClientState *get_client_state(int fd) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_states[i].fd == fd) {
            return &client_states[i];
        }
    }
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_states[i].fd == -1) {
            client_states[i].fd = fd;
            return &client_states[i];
        }
    }
    return NULL;
}

void network_start_server(Cache *cache) {
    initialize_client_states();

    int server_fd;
    struct pollfd fds[MAX_CLIENTS];
    int nfds = 1;

    struct sockaddr_in address;
    int opt = 1;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    listen(server_fd, 10);
    printf("Server started on port %d\n", PORT);

    for (int i = 0; i < MAX_CLIENTS; i++) fds[i].fd = -1;
    
    fds[0].fd = server_fd;
    fds[0].events = POLLIN;

    while (1) {
        int ret = poll(fds, nfds, -1);
        if (ret < 0) {
            perror("Poll error");
            break;
        }

        for (int i = 0; i < nfds; i++) {
            if (fds[i].revents == 0) continue;

            if (fds[i].fd == server_fd) {
                int new_client = accept(server_fd, NULL, NULL);
                if (new_client == -1) continue;

                int added = 0;
                for (int j = 1; j < MAX_CLIENTS; j++) {
                    if (fds[j].fd < 0) {
                        fds[j].fd = new_client;
                        fds[j].events = POLLIN;
                        if (j >= nfds) nfds = j + 1;
                        added = 1;
                        break;
                    }
                }
                if (!added) {
                    printf("Max clients reached\n");
                    close(new_client);
                }
            }
            else {
                int status = network_handle_client(fds[i].fd, cache);
                if (status <= 0) {
                    close(fds[i].fd);
                    fds[i].fd = -1;
                }
            }
        }
    }
}

int network_handle_client(int client_socket, Cache *cache) {
    char buffer[BUFFER_SIZE] = {0};
    ssize_t valread = read(client_socket, buffer, BUFFER_SIZE - 1);

    if (valread <= 0) return 0;

    ClientState *client_state = get_client_state(client_socket);
    if (!client_state) {
        send(client_socket, "-ERR Server error\r\n", 19, 0);
        return 0;
    }

    char password[50];
    unsigned char stored_hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char *)"1234", strlen("1234"), stored_hash);

    if (sscanf(buffer, "AUTH %49s", password) == 1) {
        if (check_password(password, (const char *)stored_hash)) {
            client_state->authenticated = true;
            send(client_socket, "+OK Authenticated\r\n", 20, 0);
        } else {
            send(client_socket, "-ERR Authentication failed\r\n", 29, 0);
        }
        return 1;
    }

    if (!client_state->authenticated) {
        send(client_socket, "-ERR Please authenticate first\r\n", 33, 0);
        return 1;
    }

    char cmd[10], key[50], val[50];
    int args = sscanf(buffer, "%s %s %s", cmd, key, val);

    if (args < 1) return 1;

    if (strcmp(cmd, "SET") == 0) {
        cache_set(cache, key, val);
        send(client_socket, "+OK\r\n", 5, 0);
    } else if (strcmp(cmd, "GET") == 0) {
        char *value = cache_get(cache, key);
        if (value) {
            send(client_socket, value, strlen(value), 0);
            send(client_socket, "\r\n", 2, 0);
        } else {
            send(client_socket, "$-1\r\n", 5, 0);
        }
    } else if (strcmp(cmd, "QUIT") == 0) {
        return 0;
    } else {
        send(client_socket, "-ERR unknown command\r\n", 22, 0);
    }

    return 1;
}