#ifndef CLIENT_H
#define CLIENT_H

#include "socket.h"

typedef struct {
   SOCKET sock;
   char name[1024];
}Client;

#endif /* guard */
