#ifndef _PERSISTENCE_H
#define _PERSISTENCE_H

#include <stdio.h>
#include <pthread.h>

// Net
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_CLIENTS 500

struct Client {
    int sockfd;
    int uid;
    pthread_t thread_id;
    struct Client *next;
};

struct Client *clients[MAX_CLIENTS];
int hash_function(int uid);

#endif