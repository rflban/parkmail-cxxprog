#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "rating.h"
#include "teamresult.h"
#include "exitcodes.h"

#define TOP_SIZE 10

static inline char isdigit_str(const char* str) {
    while (*str)
        if (!isdigit(*(str++)))
            return 0;
    return 1;
}

static void exit_if_error(int exit_code) {
    switch (exit_code) {
        case O_SUCCESS:
            break;
        case O_UNEXPECTED_NULL_ARG :
            fprintf(stderr, "unexpected NULL argument have been passed\n");
            exit(exit_code);
        case O_DAMAGED_DATA:
            fprintf(stderr, "damaged data\n");
            exit(exit_code);
        case O_BAD_ALLOC:
            fprintf(stderr, "memory allocation error\n");
            exit(exit_code);
        default:
            exit(exit_code);
    }
}

int main(void) {
    int exit_code = O_SUCCESS;
    char* buffers[4] = {};
    size_t buffers_size[4] = {};
    ssize_t bytes_recieved = 1;
    struct team_result read_team_result;

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

            if (
                buffers[0] && buffers[1] && buffers[2] && buffers[3] &&
                isdigit_str(buffers[0]) &&
                isdigit_str(buffers[2]) &&
                isdigit_str(buffers[3])
            ) {
                read_team_result.number = atoi(buffers[0]);
                read_team_result.name = buffers[1];
                read_team_result.route_time_secs = atoi(buffers[2]);
                read_team_result.control_point_qty = atoi(buffers[3]);
                exit_code = rating_add(rating, &read_team_result);
            }
        }
        else if (i != 1)
            exit_code = O_DAMAGED_DATA;
    }

    for (int i = 0; i < 4; ++i)
        free(buffers[i]);

    unsigned int rating_len;
    if (exit_code == O_SUCCESS)
        exit_code = rating_size(rating, &rating_len);

    unsigned int top_size = 0;
    struct team_result* top_of_rating = NULL;
    if (exit_code == O_SUCCESS) {
        top_size = (rating_len > TOP_SIZE) ? TOP_SIZE : rating_len;

        top_of_rating = (struct team_result*)
            malloc(sizeof(struct team_result) * top_size);
        if (!top_of_rating) {
            exit_code = O_BAD_ALLOC;
        }
    }

    if (exit_code == O_SUCCESS)
        exit_code = rating_pick_best_of(rating, top_of_rating, top_size);

    if (exit_code == O_SUCCESS) {
        printf("Top %d teams:\n", top_size);
        for (unsigned int idx = 0; idx < top_size; ++idx) {
            printf(
                "%3d) Team #%-3d \"%7s\" has achieved %3d control "
                "points in %3d minutes %3d seconds\n",
                idx + 1,
                top_of_rating[idx].number,
                top_of_rating[idx].name,
                top_of_rating[idx].control_point_qty,
                top_of_rating[idx].route_time_secs / 60,
                top_of_rating[idx].route_time_secs % 60
            );
        }
        printf("\n");
    }


    free(top_of_rating);
    rating_destroy(&rating);

    exit_if_error(exit_code);
    return 0;
}
