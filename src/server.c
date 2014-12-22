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
    printf("[ICE-CORE] Creating server...\n");
    IceServer* server = malloc(sizeof(*server));
    check(server);

    server->family = family;

    switch(server->family) {
        case AF_INET:
            server->addrlen = INET_ADDRSTRLEN;
            break;
        case AF_INET6:
            server->addrlen = INET6_ADDRSTRLEN;
            break;
        default:
            printf("[ICE-CORE] Address family not supported.\n");
            goto error;
    }
    server->ip = malloc(server->addrlen);
    strcpy(server->ip, ip);
    server->backlog = IC_BACKLOG;
    server->port = port;
    server->run = 0;
    server->connections = 0;
    server->socket = socket(server->family, SOCK_STREAM, 0);

    ice_t address_converted = iceserver_address_create(server);
    check_ok(address_converted);

    return server;
error:
    iceserver_destroy(server);
    return NULL;
}


void iceserver_destroy(IceServer* server) {
    close(server->socket);
    if(server->ip) free(server->ip);
    if(server->address) free(server->address);
    free(server);
    printf("[ICE-CORE] Server destroyed.\n");
}


ice_t iceserver_address_create(IceServer* server) {
    server->address = malloc(sizeof(*server->address));
    server->address->sin_family = server->family;
    server->address->sin_port = htons(server->port);

    int ip_put = inet_pton(server->family, server->ip, &server->address->sin_addr);
    pcheck_negative(ip_put, "IP ADDRESS CONVERSION");

    return ICE_OK;
error:
    return ICE_SERVER_ERROR;
}


ice_t iceserver_set_reuse_address(IceServer* server) {
    int yes = 1;
    int set_opt = setsockopt(server->socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    pcheck_negative(set_opt, "SET SOCKET OPTION SO_REUSEADDR");

    return ICE_OK;
error:
    return ICE_SERVER_ERROR;
}


ice_t iceserver_bind(const IceServer* server) {
    socklen_t address_size = sizeof(struct sockaddr_in);
    int binding = bind(server->socket, (struct sockaddr*) server->address, address_size);
    pcheck_negative(binding, "CHECK");

    return ICE_OK;
error:
    return ICE_SERVER_ERROR;
}


ice_t iceserver_listen(const IceServer* server) {
    printf("[ICE-CORE] Listening on %s:%d\n", server->ip, server->port);
    int listening = listen(server->socket, server->backlog);
    pcheck_negative(listening, "LISTEN");

    return ICE_OK;
error:
    return ICE_SERVER_ERROR;
}


ice_t iceserver_init(IceServer* server) {
    check_ok(iceserver_set_reuse_address(server));
    check_ok(iceserver_bind(server));
    check_ok(iceserver_listen(server));
    return ICE_OK;
error:
    return ICE_SERVER_ERROR;
}


ice_t iceserver_do_stuff(const int* socket, const struct sockaddr_in* address) {
    struct timeval stop, start;

    gettimeofday(&start, NULL);

    pcheck_negative(*socket, "ACCEPT");

    char buffer[IC_BUFFER_SIZE];
    memset(buffer, 0, IC_BUFFER_SIZE);
    int r = read(*socket, buffer, IC_BUFFER_SIZE - 1);
    pcheck_negative(r, "READ");

    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, (void*)&(address->sin_addr), ip, INET_ADDRSTRLEN);
    pcheck(ip, "CLIENT IP CONVERSION");

    gettimeofday(&stop, NULL);
    printf("[%s] Read in %u: %s\n", ip, stop.tv_usec - start.tv_usec, buffer);

    char ack[] = "ack";
    gettimeofday(&start, NULL);

    send(*socket, ack, strlen(ack), 0);

    gettimeofday(&stop, NULL);
    printf("[%s] Wrote in %u: %s\n", ip, stop.tv_usec - start.tv_usec, ack);

    return ICE_OK;
error:
    return ICE_SERVER_ERROR;
}


void iceserver_run(IceServer* server) {
    socklen_t address_size = sizeof(struct sockaddr_in);
    int descriptor;
    while(server->run == 0) {
        struct sockaddr_in client_addr;
        descriptor = accept(server->socket, (struct sockaddr*) &client_addr, &address_size);
        pcheck_negative(descriptor, "ACCEPT DESCRIPTOR");
        server->connections++;
        ice_t result = iceserver_do_stuff(&descriptor, &client_addr);
        printf("[ICE-CORE] Open connections: %u.\n", server->connections);
        printf("[ICE-CORE] Last descriptor: %d.\n", descriptor);
        check_ok(result);
        close(descriptor);
    }
    return;
error:
    server->run = 1;
    printf("[ICE-CORE] Server unexpectly stopped running.\n");
    return;
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
    pcheck(ic_server, "CREATION");
    check_ok(iceserver_init(ic_server));

    signal(SIGINT, iceserver_quit_signal);
    iceserver_run(ic_server);
    iceserver_destroy(ic_server);

    return EXIT_SUCCESS;
error:
    if(ic_server) iceserver_destroy(ic_server);
    return EXIT_FAILURE;
}

