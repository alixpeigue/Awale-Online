#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "client_server_protocol.h"
#include "server_client_protocol.h"
#include <stddef.h>
#include <stdint.h>

typedef struct {
    void (*connection_successful)(void);
    void (*connection_refused)(const char *error_message);
    void (*room_creation_successful)(uint32_t room_id);
    void (*room_creation_refused)(const char *error_message);
    void (*join_room_successful)(const char **users, uint8_t nb_users);
    void (*join_room_refused)(const char *);
    void (*played)(uint8_t played);
    void (*game_start)(uint8_t pos);
    void (*player_joined_room)(const char *username);
    void (*spectator_joined_room)(const char *username);
    void (*game_stopped)(uint8_t winner);
} Handlers;

size_t server_client_protocol_read(const uint8_t *buf,
                                   const Handlers *handlers);
size_t server_client_protocol_write_connect(uint8_t *buf, const char *name);
size_t server_client_protocol_write_create_room(uint8_t *buf);
size_t server_client_protocol_write_join_room(uint8_t *buf, uint32_t room_id);
size_t server_client_protocol_write_spectate_room(uint8_t *buf,
                                                  uint32_t room_id);
size_t server_client_protocol_write_play(uint8_t *buf, uint8_t pos);
size_t server_client_protocol_write_leave_room(uint8_t *buf);

#endif
