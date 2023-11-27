#ifndef CLIENT_SERVER_PROTOCOL_H
#define CLIENT_SERVER_PROTOCOL_H 

typedef enum {
    CONNECT,
    CREATE_ROOM,
    JOIN_ROOM,
    SPECTATE_ROOM,
    PLAY,
    LEAVE_ROOM,
    SEND_MESSAGE
} ClientServerProtocol;

#endif
