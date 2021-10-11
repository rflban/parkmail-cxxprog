#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "exitcodes.h"
#include "rating.h"
#include "team.h"
#include "io.h"

#define TOP_SIZE 10

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

    struct rating* rating = rating_create();
    if (!rating)
        exit_code = O_BAD_ALLOC;
    else
        exit_code = rating_read(rating, stdin);

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
