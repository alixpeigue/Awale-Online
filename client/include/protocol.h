#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "client_server_protocol.h"
#include "server_client_protocol.h"
#include "state.h"
#include <stddef.h>
#include <stdint.h>

typedef struct {
    void (*connection_successful)(State *);
    void (*connection_refused)(State *, const char *error_message);
    void (*room_creation_successful)(State *, uint32_t room_id);
    void (*room_creation_refused)(State *, const char *error_message);
    void (*join_room_successful)(State *, uint8_t nb_users,
                                 uint8_t nb_spectators, const char **users,
                                 const char **bios, const char **spectators);
    void (*join_room_refused)(State *, const char *);
    void (*played)(State *, uint8_t s_score, uint8_t o_score, uint8_t *board);
    void (*game_start)(State *, uint8_t pos);
    void (*player_joined_room)(State *, const char *username);
    void (*spectator_joined_room)(State *, const char *username);
    void (*game_stopped)(State *, uint8_t draw, const char *winner);
    void (*message)(State *, const char *username, const char *message);
    void (*invalid_play)(State *, const char *message);
} Handlers;

size_t server_client_protocol_read(const uint8_t *buf, const Handlers *handlers,
                                   State *state);
size_t server_client_protocol_write_connect(uint8_t *buf, const char *name);
size_t server_client_protocol_write_create_room(uint8_t *buf);
size_t server_client_protocol_write_join_room(uint8_t *buf, uint32_t room_id,
                                              uint8_t spectate);
size_t server_client_protocol_write_play(uint8_t *buf, uint8_t pos);
size_t server_client_protocol_write_leave_room(uint8_t *buf);
size_t server_client_protocol_write_send_message(uint8_t *buf,
                                                 const char *message);
size_t server_client_protocol_write_set_biography(uint8_t *buf,
                                                  const char *bio);

#endif
