#ifndef SERVER_CLIENT_PROTOCOL_H
#define SERVER_CLIENT_PROTOCOL_H 

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
    GAME_STOPPED,
    MESSAGE,
    INVALID_PLAY,
} ServerClientProtocol;

#endif
