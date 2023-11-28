#include "actions.h"
#include "client.h"
#include "protocol.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void action_connect() {
    printf("What is your name?\n");
}

void action_play() {
    printf("Please enter your next move?\n");
}

void action_choose() {
    printf("What do you want to do ?\n 1 - Create a room\n 2 - Join a room\n 3 "
           "- Spectate a room\n");
}

void action_create_room() {
    uint8_t buf[1024];
    size_t size = server_client_protocol_write_create_room(buf);
    write_server((char *)buf, size);
}

void action_join_room() {
    printf("What room do you want to join ?\n");
}

void action_spectate_room() {
    printf("What room do you want to spectate ?\n");
}

void action_show_board(uint8_t s_score, uint8_t o_score, uint8_t *board) {

    char *score="Score: ";

    char buf[3];
    
    int col_size = 3;
    int col_nb = 6;
    int total = col_nb*(col_size+1)+1;

    printf("\n")
    
    // first line
    
    printf("╭");
    for(int i = 0; i<total - 2; ++i) {
        printf("─");
    }
    printf("╮\n");

    // First Score
    
    snprintf(buf, 2, "%d", o_score);
    printf("│");
    printf("%s%s", score, buf);
    for(int i=0; i<total-2-strlen(score)-strlen(buf); ++i) {
        printf(" ");
    }
    printf("│\n");

    // Sep line
    
    printf("├");
    for(int i=0; i<col_nb-1; ++i) {
        for(int j=0; j<col_size; ++j) {
            printf("─");
        }
        printf("┬");
    }
    for(int j=0; j<col_size; ++j) {
        printf("─");
    }
    printf("┤\n");

    // First array line

    printf("│");
    for(int i=col_nb-1; i>=0; --i) {
        snprintf(buf, 2, "%d", board[i+col_nb]);
        for(int j=0; j<col_size-1-strlen(buf); j++) {
            printf(" ");
        }
        printf("%s │", buf);
    }
    printf("\n");

    // Middle sep

    printf("├");
    for(int i=0; i<col_nb-1; ++i) {
        for(int j=0; j<col_size; ++j) {
            printf("─");
        }
        printf("┼");
    }
    for(int j=0; j<col_size; ++j) {
        printf("─");
    }
    printf("┤\n");

    // Second array line
    
    printf("│");
    for(int i=0; i<col_nb; ++i) {
        snprintf(buf, 2, "%d", board[i]);
        for(int j=0; j<col_size-1-strlen(buf); j++) {
            printf(" ");
        }
        printf("%s │", buf);
    }
    printf("\n");

    // Second score separator
    printf("├");
    for(int i=0; i<col_nb-1; ++i) {
        for(int j=0; j<col_size; ++j) {
            printf("─");
        }
        printf("┴");
    }
    for(int j=0; j<col_size; ++j) {
        printf("─");
    }
    printf("┤\n");

    // Second score line
    
    snprintf(buf, 2, "%d", s_score);
    printf("│");
    printf("%s%s", score, buf);
    for(int i=0; i<total-2-strlen(score)-strlen(buf); ++i) {
        printf(" ");
    }
    printf("│\n");

    // Last line
    
    printf("╰");
    for(int i = 0; i<total - 2; ++i) {
        printf("─");
    }
    printf("╯\n");
    
    printf("\n");
}
