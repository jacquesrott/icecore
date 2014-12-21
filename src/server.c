#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <signal.h>

#include "error.h"
#include "server.h"


IceServer* iceserver_create(const char* ip, int port, sa_family_t family) {
    IceServer* server = malloc(sizeof(*server));
    if(server == NULL) {
        return NULL;
    }
    server->family = family;

    if(server->family == AF_INET) {
        server->addrlen = INET_ADDRSTRLEN;
    } else if(server->family == AF_INET6) {
        server->addrlen = INET6_ADDRSTRLEN;
    } else {
        iceserver_destroy(server);
        return NULL;
    }
    server->ip = malloc(sizeof(char) * server->addrlen);
    strcpy(server->ip, ip);
    server->backlog = IC_BACKLOG;
    server->port = port;
    server->run = 0;
    server->socket = socket(server->family, SOCK_STREAM, 0);

    ice_t address_converted = iceserver_address_create(server);
    if(address_converted != ICE_OK) {
        iceserver_destroy(server);
        return NULL;
    }

    return server;
}


void iceserver_destroy(IceServer* server) {
    close(server->socket);
    if(server->ip) {
        free(server->ip);
    }
    if(server->address) {
        free(server->address);
    }
    printf("[ICE-CORE] Server destroyed.\n");
    server = NULL;
}


ice_t iceserver_address_create(IceServer* server) {
    server->address = malloc(sizeof(*server->address));
    server->address->sin_family = server->family;
    server->address->sin_port = htons(server->port);

    int ip_put = inet_pton(server->family, server->ip, &server->address->sin_addr);
    if(ip_put == -1) {
        perror("IP ADDRESS CONVERSION");
        return ICE_SERVER_ERROR;
    }

    return ICE_OK;
}


ice_t iceserver_set_reuse_address(IceServer* server) {
    int yes = 1;
    int set_opt = setsockopt(server->socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    if(set_opt == -1) {
        perror("SET SOCKET OPTION SO_REUSEADDR");
        return ICE_SERVER_ERROR;
    }
    return ICE_OK;
}


ice_t iceserver_bind(const IceServer* server) {
    socklen_t address_size = sizeof(struct sockaddr_in);
    int binding = bind(server->socket, (struct sockaddr*) server->address, address_size);
    if(binding == -1) {
        perror("BIND");
        return ICE_SERVER_ERROR;
    }
    return ICE_OK;
}


ice_t iceserver_listen(const IceServer* server) {
    printf("[ICE-CORE] Listening on %s:%d\n", server->ip, server->port);
    int listening = listen(server->socket, server->backlog);
    if(listening == -1) {
        perror("LISTEN");
        return ICE_SERVER_ERROR;
    }

    return ICE_OK;
}


ice_t iceserver_init(IceServer* server) {
    if(iceserver_set_reuse_address(server) != ICE_OK) {
        return ICE_SERVER_ERROR;
    }
    if(iceserver_bind(server) != ICE_OK) {
        return ICE_SERVER_ERROR;
    }
    if(iceserver_listen(server) != ICE_OK) {
        return ICE_SERVER_ERROR;
    }
    return ICE_OK;
}


ice_t iceserver_do_stuff(const int* socket, const struct sockaddr_in* address) {
    struct timeval stop, start;

    gettimeofday(&start, NULL);

    if(*socket == -1) {
        perror("ACCEPT");
        return ICE_SERVER_ERROR;
    }

    char buffer[IC_BUFFER_SIZE];
    memset(buffer, 0, IC_BUFFER_SIZE);
    int r = read(*socket, buffer, IC_BUFFER_SIZE - 1);
    if(r == -1) {
        perror("READ");
        return ICE_SERVER_ERROR;
    }
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, (void*)&(address->sin_addr), ip, INET_ADDRSTRLEN);
    if(ip == NULL) {
        perror("CLIENT IP CONVERSION");
        return ICE_SERVER_ERROR;
    }
    gettimeofday(&stop, NULL);
    printf("[%s] Read in %u: %s\n", ip, stop.tv_usec - start.tv_usec, buffer);

    char ack[] = "ack";
    gettimeofday(&start, NULL);

    send(*socket, ack, strlen(ack), 0);

    gettimeofday(&stop, NULL);
    printf("[%s] Wrote in %u: %s\n", ip, stop.tv_usec - start.tv_usec, ack);

    return ICE_OK;
}


void iceserver_run(const IceServer* server) {
    socklen_t address_size = sizeof(struct sockaddr_in);
    while(server->run == 0) {
        struct sockaddr_in client_addr;
        int conn_socket = accept(server->socket, (struct sockaddr*) &client_addr, &address_size);
        ice_t result = iceserver_do_stuff(&conn_socket, &client_addr);
        close(conn_socket);
        if(result != ICE_OK) {
            break;
        }
    }
}



void iceserver_quit_signal(int sig) {
    if(ic_server) {
        printf("[ICE-CORE] Gracefully quitting...\n");
        ic_server->run = 1;
    }
}


int main() {
    char ip[INET_ADDRSTRLEN] = "127.0.0.1";
    int port = 6666;

    ic_server = iceserver_create(ip, port, AF_INET);
    if(iceserver_init(ic_server) != ICE_OK) {
        iceserver_destroy(ic_server);
        return EXIT_FAILURE;
    }
    signal(SIGINT, iceserver_quit_signal);
    iceserver_run(ic_server);
    iceserver_destroy(ic_server);

    return EXIT_SUCCESS;
}

