#ifndef SERVER_CLIENT_PROTOCOL_H
#define SERVER_CLIENT_PROTOCOL_H 

#include <stdint.h>
typedef enum {
    CONNECTION_SUCCESSFUL,
    CONNECTION_REFUSED,
    ROOM_CREATION_SUCCESSFUL,
    ROOM_CREATION_REFUSED,
    JOIN_ROOM_SUCCESSFUL,
    JOIN_ROOM_REFUSED,
    PLAYED,
    GAME_START,
    PLAYER_JOINED_ROOM,
    SPECTATOR_JOINED_ROOM,
    GAME_STOPPED
} ServerClientProtocol;

void server_client_protocol_read(uint8_t* buf) {
    uint16_t length = 0;
    length = buf[0]<<8|buf[1];
    ServerClientProtocol message = buf[3];
    
}

#endif
