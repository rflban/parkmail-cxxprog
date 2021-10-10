#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "exitcodes.h"
#include "rating.h"
#include "teamresult.h"

#define TOP_SIZE 10

static inline char isdigit_str(const char* str) {
    while (*str)
        if (!isdigit(*(str++)))
            return 0;
    return 1;
}

static void print_if_error(int exit_code) {
    switch (exit_code) {
        case O_UNEXPECTED_NULL_ARG:
            fprintf(stderr, "unexpected NULL argument have been passed\n");
            break;
        case O_DAMAGED_DATA:
            fprintf(stderr, "damaged data\n");
            break;
        case O_BAD_ALLOC:
            fprintf(stderr, "memory allocation error\n");
            break;
    }
}

int main(void) {
    int exit_code = O_SUCCESS;
    char* buffers[4] = {};
    size_t buffers_size[4] = {};
    ssize_t bytes_recieved = 1;
    struct team received_team;

    struct rating* rating = rating_create();
    if (!rating)
        exit_code = O_BAD_ALLOC;

    while (!feof(stdin) && bytes_recieved > 0 && exit_code == O_SUCCESS) {
        int i = 0;
        for (; i < 4 && !feof(stdin) && bytes_recieved > 0; ++i)
            bytes_recieved = getline(buffers + i, buffers_size + i, stdin);

        if (bytes_recieved > 0) {
            for (int i = 0; buffers[i] && i < 4; ++i) {
                buffers[i][strlen(buffers[i]) - 1] = '\0';
            }

            if (buffers[0] && buffers[1] && buffers[2] && buffers[3] &&
                isdigit_str(buffers[0]) && isdigit_str(buffers[2]) &&
                isdigit_str(buffers[3])) {
                received_team.number = atoi(buffers[0]);
                received_team.name = buffers[1];
                received_team.route_time_secs = atoi(buffers[2]);
                received_team.control_point_qty = atoi(buffers[3]);
                exit_code = rating_add(rating, &received_team);
            }
        } else if (i != 1)
            exit_code = O_DAMAGED_DATA;
    }

    for (int i = 0; i < 4; ++i) free(buffers[i]);

    unsigned int rating_len;
    if (exit_code == O_SUCCESS)
        exit_code = rating_size(rating, &rating_len);

    unsigned int top_size = 0;
    struct team* top_teams = NULL;
    if (exit_code == O_SUCCESS) {
        top_size = (rating_len > TOP_SIZE) ? TOP_SIZE : rating_len;

        top_teams =
            (struct team*)malloc(sizeof(struct team) * top_size);
        if (!top_teams) {
            exit_code = O_BAD_ALLOC;
        }
    }

    if (exit_code == O_SUCCESS)
        exit_code = rating_pick_best_of(rating, top_teams, top_size);

    if (exit_code == O_SUCCESS) {
        printf("Top %d teams:\n", top_size);
        for (size_t idx = 0; idx < top_size; ++idx) {
            printf(
                "%3lu) Team #%-3d \"%7s\" has achieved %3d control "
                "points in %3d minutes %3d seconds\n",
                idx + 1, top_teams[idx].number, top_teams[idx].name,
                top_teams[idx].control_point_qty,
                top_teams[idx].route_time_secs / 60,
                top_teams[idx].route_time_secs % 60);
        }
        printf("\n");
    }

    free(top_teams);
    rating_destroy(&rating);

    print_if_error(exit_code);
    return exit_code;
}
