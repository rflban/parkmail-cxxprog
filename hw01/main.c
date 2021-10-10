#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "rating.h"
#include "teamresult.h"

#define TOP_SIZE 10

static inline char isdigit_str(const char* str) {
    while (*str) {
        if (!isdigit(*(str++)))
            return 0;
    }

    return 1;
}

int main(void) {
    char* buffers[4] = {};
    size_t buffers_size[4] = {};
    ssize_t bytes_recieved = 1;
    struct team_result read_team_result;

    struct rating* rating = rating_create();

    while (!feof(stdin) && bytes_recieved > 0) {
        for (int i = 0; i < 4 && bytes_recieved > 0; ++i)
            bytes_recieved = getline(buffers + i, buffers_size + i, stdin);

        if (bytes_recieved > 0) {
            for (int i = 0; i < 4; ++i) {
                buffers[i][strlen(buffers[i]) - 1] = '\0';
            }

            if (
                isdigit_str(buffers[0]) &&
                isdigit_str(buffers[2]) &&
                isdigit_str(buffers[3])
            ) {
                read_team_result.number = atoi(buffers[0]);
                read_team_result.name = buffers[1];
                read_team_result.route_time_secs = atoi(buffers[2]);
                read_team_result.control_point_qty = atoi(buffers[3]);
                rating_add(rating, &read_team_result);
            }
        }
    }

    for (int i = 0; i < 4; ++i)
        free(buffers[i]);

    unsigned int rating_len;
    rating_size(rating, &rating_len);
    unsigned int top_size = (rating_len > TOP_SIZE) ? TOP_SIZE : rating_len;

    struct team_result* top_of_rating = (struct team_result*)
        malloc(sizeof(struct team_result) * top_size);
    int rc = rating_pick_best_of(rating, top_of_rating, top_size);

    printf("Top %d teams:\n", top_size);
    for (unsigned int idx = 0; idx < top_size; ++idx) {
        printf(
            "%d) Team #%d \"%s\" has achieved %d control "
            "points in %d minutes %d seconds\n",
            idx + 1,
            top_of_rating[idx].number,
            top_of_rating[idx].name,
            top_of_rating[idx].control_point_qty,
            top_of_rating[idx].route_time_secs / 60,
            top_of_rating[idx].route_time_secs % 60
        );
    }
    printf("\n");

    free(top_of_rating);
    rating_destroy(&rating);

    return 0;
}
