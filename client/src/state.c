#include "state.h"
#include "actions.h"
#include "client.h"
#include "protocol.h"
#include <stdint.h>
#include <stdlib.h>
#include <sys/socket.h>

void set_current_state(State *state, State new_state) {
    *state = new_state;
    switch (new_state) {
    case CONNECTING:
        action_connect();
        break;
    case CONNECTED:
        action_choose();
        break;
    }
}

void handle_user_input(State *state, const char *in) {
    uint8_t buf[1024];
    size_t size;
    switch (*state) {
    case CONNECTING: {
        printf("Welcome, %s\n", in);
        server_client_protocol_write_connect(buf, in);
        set_current_state(state, WAITING_CONNECTION);
    } break;
    case CONNECTED: {
        uint8_t choice = atoi(in);
        if (choice < 1 || choice > 3) {
            printf("Invalid input, must be between 1 and 3\n");
            set_current_state(state, CONNECTED);
            return;
        }
        switch (choice) {
        case 2:
            set_current_state(state, WAITING_JOIN_ROOM_ID);
            break;
        case 3:
            set_current_state(state, WAITING_SPECTATE_ROOM_ID);
            break;
        }
    } break;
    case WAITING_JOIN_ROOM_ID: {
        uint32_t room_id = strtol(in, NULL, 16);
        size = server_client_protocol_write_join_room(buf, room_id);
        write_server((char *)buf, size);
    } break;
    case WAITING_SPECTATE_ROOM_ID: {
        uint32_t room_id = strtol(in, NULL, 16);
        size = server_client_protocol_write_spectate_room(buf, room_id);
        write_server((char *)buf, size);
    } break;
    case WAITING_PLAY: {
        uint8_t pos = atoi(in);
        size = server_client_protocol_write_play(buf, pos);
        write_server((char *)buf, size);
        set_current_state(state, IN_GAME);
    }
    default:
        printf("Input ignored\n");
    }    
}

void handle_connection_successful(State *state) {
    if (*state == WAITING_CONNECTION) {
        printf("You are successfully connected !\n");
        set_current_state(state, CONNECTED);
    }
}

void handle_connection_refused(State *state, const char *error_message) {
    if (*state == WAITING_CONNECTION) {
        printf("Connection refused : %s\n", error_message);
        set_current_state(state, CONNECTING);
    }
}

void handle_room_creation_successful(State *state, uint32_t room_id) {
    if (*state == CONNECTED) {
        printf("Room successfully created with id %x\n", room_id);
        set_current_state(state, IN_ROOM);
    }
}

void handle_room_creation_refused(State *state, const char *error_message) {
    if (*state == CONNECTED) {
        printf("Couldn't create room : %s\n", error_message);
    }
}

void handle_join_room_successful(State *state, uint8_t nb_users,
                                 const char **users) {
    if (*state == CONNECTED) {
        printf("Room joined\nUsers in room spectating room : \n");
        for (uint8_t i = 0; i < nb_users; ++i) {
            printf(" - %s\n", users[0]);
        }
        set_current_state(state, IN_ROOM);
    }
}

void handle_join_room_refused(State *state, const char *error_message) {
    if (*state == CONNECTED) {
        printf("Could not join room : %s\n", error_message);
    }
}

void handle_spectate_room_successful(State *state) {
    if (*state == CONNECTED) {
        printf("Successfilly joined room\n");
        set_current_state(state, SPECTATING);
    }
}

void handle_spectate_room_refused(State *state, const char *error_message) {
    if (*state == CONNECTED) {
        printf("Could not spectate room : %s\n", error_message);
    }
}

void handle_played(State *state, uint8_t pos) {
    if (*state == IN_GAME) {
        printf("Played : %d\n", pos);
        action_play();
        set_current_state(state, WAITING_PLAY);

    } else if (*state == SPECTATING) {
        printf("(Spectating) Played : %d\n", pos);
    }
}

void handle_game_start(State *state, uint8_t pos) {
    if (*state == IN_ROOM) {
        printf("You start at position %d", pos);
        if(pos==1) {
            set_current_state(state, WAITING_PLAY);
        } else if (pos==2) {
            set_current_state(state, IN_GAME);
        }
    }
}

void handle_player_joined_room(State *state, const char *username) {
    if (*state == IN_ROOM || *state == SPECTATING) {
        printf("Second player joined : %s\n", username);
    }
}

void handle_spectator_joined_room(State *state, const char *username) {
    if (*state == IN_ROOM || *state == IN_GAME) {
        printf("User %s is now spectating !\n", username);
    }
}

void handle_game_stopped(State *state, uint8_t winner) {
    if (*state == IN_GAME || *state == SPECTATING || *state == IN_ROOM) {
        if (winner) {
            printf("Game ended ! Player %d won\n", winner);
        } else {
            printf("Room is closed\n");
        }
        set_current_state(state, CONNECTED);
    }
}
