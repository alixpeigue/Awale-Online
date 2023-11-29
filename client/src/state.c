#include "state.h"
#include "actions.h"
#include "client.h"
#include "protocol.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/socket.h>

void set_current_state(State *state, State new_state) {
    *state = new_state;
    printf("New state: %d\n", new_state);
    switch (new_state) {
    case CONNECTING:
        action_connect();
        break;
    case CONNECTED:
        action_choose();
        break;
    case WAITING_PLAY_INPUT:
        action_play();
        break;
    case WAITING_CREATE_ROOM_RESPONSE:
        action_create_room();
        break;
    case WAITING_JOIN_ROOM_INPUT:
        action_join_room();
        break;
    case WAITING_SPECTATE_ROOM_INPUT:
        action_spectate_room();
        break;
    }
}

void handle_user_input(State *state, const char *in) {
    uint8_t buf[1024];
    size_t size;
    switch (*state) {
    case CONNECTING: {
        printf("Welcome, %s\n", in);
        size = server_client_protocol_write_connect(buf, in);
        write_server((char *)buf, size);
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
        case 1:
            set_current_state(state, WAITING_CREATE_ROOM_RESPONSE);
            break;
        case 2:
            set_current_state(state, WAITING_JOIN_ROOM_INPUT);
            break;
        case 3:
            set_current_state(state, WAITING_SPECTATE_ROOM_INPUT);
            break;
        }
    } break;
    case WAITING_JOIN_ROOM_INPUT: {
        uint32_t room_id = strtol(in, NULL, 16);
        size = server_client_protocol_write_join_room(buf, room_id);
        write_server((char *)buf, size);
        set_current_state(state, WAITING_JOIN_ROOM_RESPONSE);
    } break;
    case WAITING_SPECTATE_ROOM_INPUT: {
        uint32_t room_id = strtol(in, NULL, 16);
        size = server_client_protocol_write_spectate_room(buf, room_id);
        write_server((char *)buf, size);
        set_current_state(state, WAITING_SPECTATE_ROOM_RESPONSE);
    } break;
    case WAITING_PLAY_INPUT: {
        char * endptr;
        uint8_t pos = strtol(in, &endptr ,10) - 1;
        if(*endptr == '\0') { // input is valid pos
            size = server_client_protocol_write_play(buf, pos);
            write_server((char *)buf, size);
            set_current_state(state, WAITING_PLAY_ACK);
        } else {
            size = server_client_protocol_write_send_message(buf, in);
            write_server((char *)buf, size);
            printf("Message Sent !\n");
            set_current_state(state, WAITING_PLAY_INPUT);
        }
    } break;
    case IN_ROOM:
    case SPECTATING:
    case IN_GAME: {
        size = server_client_protocol_write_send_message(buf, in);
        write_server((char*)buf, size);
        
    } break;
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
    if (*state == WAITING_CREATE_ROOM_RESPONSE) {
        printf("Room successfully created with id %x\n", room_id);
        set_current_state(state, IN_ROOM);
    }
}

void handle_room_creation_refused(State *state, const char *error_message) {
    if (*state == WAITING_CREATE_ROOM_RESPONSE) {
        printf("Couldn't create room : %s\n", error_message);
        set_current_state(state, CONNECTED);
    }
}

void handle_join_room_successful(State *state, uint8_t nb_users,
                                 const char **users) {
    if (*state == WAITING_JOIN_ROOM_RESPONSE) {
        printf("Room joined\nUsers in room spectating room : \n");
        for (uint8_t i = 0; i < nb_users; ++i) {
            printf(" - %s\n", users[i]);
        }
        set_current_state(state, IN_ROOM);
    }
}

void handle_join_room_refused(State *state, const char *error_message) {
    if (*state == WAITING_JOIN_ROOM_RESPONSE) {
        printf("Could not join room : %s\n", error_message);
        set_current_state(state, CONNECTED);
    }
}

void handle_spectate_room_successful(State *state) {
    if (*state == WAITING_SPECTATE_ROOM_RESPONSE) {
        printf("Successfilly joined room\n");
        set_current_state(state, SPECTATING);
    }
}

void handle_spectate_room_refused(State *state, const char *error_message) {
    if (*state == WAITING_CREATE_ROOM_RESPONSE) {
        printf("Could not spectate room : %s\n", error_message);
        set_current_state(state, CONNECTED);
    }
}

void handle_played(State *state, uint8_t s_score, uint8_t o_score, uint8_t *board) {
    if (*state == IN_GAME) {
        action_show_board(s_score, o_score, board);
        set_current_state(state, WAITING_PLAY_INPUT);
    } else if (*state == SPECTATING) {
        action_show_board(s_score, o_score, board);
    } else if (*state == WAITING_PLAY_ACK) {
        action_show_board(s_score, o_score, board);
        set_current_state(state, IN_GAME);
    }
}

void handle_game_start(State *state, uint8_t pos) {
    printf("Game starting ! oui oui"); 
    if (*state == IN_ROOM) {
        printf("You start at position %d", pos);
        if(pos==0) {
            set_current_state(state, WAITING_PLAY_INPUT);
        } else if (pos==1) {
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

void handle_game_stopped(State *state, uint8_t draw, const char *winner) {
    if (*state == IN_GAME || *state == SPECTATING || *state == IN_ROOM || *state == WAITING_PLAY_INPUT) {
        if (!draw) {
            printf("Game ended ! %s won\n", winner);
        } else {
            printf("Game ended ! It's a draw !\n");
        }
        set_current_state(state, CONNECTED);
    }
}

void handle_message(State *state, const char *username, const char *message, bool first) {
    if (*state == IN_ROOM || *state == SPECTATING || *state == IN_GAME || *state == WAITING_PLAY_INPUT || *state == WAITING_PLAY_ACK) {
        printf("(%s) %s\n", username, message);
    }
}


void handle_invalid_play(State *state, const char *message) {
    if (*state == WAITING_PLAY_ACK) {
        printf("Invalid move : %s\n", message);
        set_current_state(state, WAITING_PLAY_INPUT);
    }
}
