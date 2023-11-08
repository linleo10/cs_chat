#include "persistence.h"

// struct Client *clients[MAX_CLIENTS] = { 0 };

int hash_function(int uid) {
    return uid % MAX_CLIENTS;
}