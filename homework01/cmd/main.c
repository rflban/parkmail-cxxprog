#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "exitcodes.h"
#include "io.h"
#include "rating.h"
#include "team.h"

#define TOP_SIZE 10
#define SECS_PER_MIN 60

static void print_error(int exit_code) {
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
    if (!rating) {
        print_error(O_BAD_ALLOC);
        return 0;
    }

    if ((exit_code = rating_read(rating, stdin)) != O_SUCCESS) {
        print_error(exit_code);
        rating_destroy(&rating);
        return 0;
    }

    size_t rating_size;
    size_t top_size = 0;
    struct team* top_teams = NULL;

    if ((exit_code = rating_get_size(rating, &rating_size)) != O_SUCCESS) {
        print_error(exit_code);
        rating_destroy(&rating);
        return 0;
    }

    top_size = (rating_size > TOP_SIZE) ? TOP_SIZE : rating_size;
    top_teams = (struct team*)malloc(sizeof(struct team) * top_size);
    if (!top_teams) {
        print_error(O_BAD_ALLOC);
        rating_destroy(&rating);
        return 0;
    }

    exit_code = rating_pick_best_of(rating, top_teams, top_size);
    if (exit_code != O_SUCCESS) {
        print_error(exit_code);
        rating_destroy(&rating);
        free(top_teams);
        return 0;
    }

    printf("Top %lu teams:\n", top_size);
    for (size_t idx = 0; idx < top_size; ++idx) {
        printf(
            "%3lu) Team #%-3d \"%7s\" has achieved %3d control "
            "points in %3d minutes %3d seconds\n",
            idx + 1, top_teams[idx].number, top_teams[idx].name,
            top_teams[idx].control_point_qty,
            top_teams[idx].route_time_secs / SECS_PER_MIN,
            top_teams[idx].route_time_secs % SECS_PER_MIN);
    }
    printf("\n");

    free(top_teams);
    rating_destroy(&rating);

    return 0;
}
