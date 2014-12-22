#ifndef IC_SERVER_H
#define IC_SERVER_H

#define IC_BUFFER_SIZE 1025
#define IC_BACKLOG 511

typedef struct {
    char* ip;
    int port;
    int backlog;
    int socket;
    sa_family_t family;
    socklen_t addrlen;
    int run;
    unsigned int connections;
    struct sockaddr_in* address;
} IceServer;

IceServer* ic_server;

IceServer* iceserver_create(const char* ip, int port, sa_family_t family);
ice_t iceserver_init(IceServer* server);
ice_t iceserver_address_create(IceServer* server);
void iceserver_run(IceServer* server);
void iceserver_destroy(IceServer* server);
void iceserver_quit_signal(int sig);

#endif
