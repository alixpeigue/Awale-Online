#ifndef CLIENT_H
#define CLIENT_H

#include "socket.h"
#include <stdint.h>

typedef struct {
    SOCKET sock;
    char name[1024];
    uint32_t room_id;
} Client;

#endif /* guard */
