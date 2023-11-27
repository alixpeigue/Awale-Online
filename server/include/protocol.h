#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    void (*connect)(const char *name);
    void (*create_room)(void);
    void (*join_room)(uint32_t room_id);
    void (*spectate_room)(uint32_t room_id);
    void (*play)(uint8_t pos);
    void (*leave_room)(void);
} Handlers;

void handlers_init(Handlers *handler);

size_t client_server_protocol_read(const uint8_t *buf,
                                   const Handlers *handlers);
size_t server_client_protocol_write_connection_successful(uint8_t *buf);
size_t
server_client_protocol_write_connection_refused(uint8_t *buf,
                                                const char *error_message);
size_t server_client_protocol_write_room_creation_successful(uint8_t *buf, uint32_t room_id);
size_t
server_client_protocol_write_room_creation_refused(uint8_t *buf,
                                                   const char *error_message);
size_t server_client_protocol_write_join_room_successful(uint8_t *buf,
                                                         const char **users,
                                                         uint8_t nb_users);
size_t
server_client_protocol_write_join_room_refused(uint8_t *buf,
                                               const char *error_message);
size_t server_client_protocol_write_played(uint8_t *buf, uint8_t pos);
size_t server_client_protocol_write_game_start(uint8_t *buf, uint8_t side);
size_t server_client_protocol_write_player_joined_room(uint8_t *buf,
                                                       const char *username);
size_t server_client_protocol_write_spectator_joined_room(uint8_t *buf,
                                                          const char *username);
size_t server_client_protocol_write_game_stopped(uint8_t *buf, uint8_t winner);
#endif
