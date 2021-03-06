#include "io.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "exitcodes.h"
#include "rating.h"
#include "team.h"

static inline char is_digit(const char* begin, const char* end) {
    assert(begin && end);

    while (begin < end) {
        if (!isdigit(*(begin++))) {
            return 0;
        }
    }
    return 1;
}

int uint_read(unsigned int* value, FILE* stream, char** buffer,
              size_t* buffer_size) {
    if (!value || !stream || !buffer || !buffer_size) {
        return O_UNEXPECTED_NULL_ARG;
    }

    if (getline(buffer, buffer_size, stream) <= 0) {
        return O_DAMAGED_DATA;
    }

    char* parse_end = *buffer + strlen(*buffer) - 1;

    if (*parse_end == '\n') {
        --parse_end;
    }

    if (is_digit(*buffer, parse_end)) {
        *value = atoi(*buffer);
    } else {
        return O_DAMAGED_DATA;
    }

    return O_SUCCESS;
}

int team_read(struct team* team, FILE* stream, char** buffer,
              size_t* buffer_size) {
    int exit_code = O_SUCCESS;

    char* name_buffer = NULL;
    unsigned number = 0;
    unsigned route_time = 0;
    unsigned control_point_qty = 0;

    if (!team || !stream || !buffer || !buffer_size) {
        return O_UNEXPECTED_NULL_ARG;
    }

    unsigned recieved_number = 0;

    exit_code = uint_read(&recieved_number, stream, buffer, buffer_size);
    if (exit_code != O_SUCCESS) {
        return exit_code;
    }
    number = recieved_number;

    if (getline(buffer, buffer_size, stream) <= 0) {
        return O_DAMAGED_DATA;
    }

    size_t recieved_str_len = strlen(*buffer);
    if (buffer[0][recieved_str_len - 1] == '\n')
        buffer[0][--recieved_str_len] = '\0';

    name_buffer = (char*)malloc(sizeof(char) * (strlen(*buffer) + 1));
    if (!name_buffer)
        return O_BAD_ALLOC;

    strncpy(name_buffer, *buffer, recieved_str_len + 1);

    exit_code = uint_read(&recieved_number, stream, buffer, buffer_size);
    if (exit_code != O_SUCCESS) {
        free(name_buffer);
        return exit_code;
    }
    route_time = recieved_number;

    exit_code = uint_read(&recieved_number, stream, buffer, buffer_size);
    if (exit_code != O_SUCCESS) {
        free(name_buffer);
        return exit_code;
    }
    control_point_qty = recieved_number;

    team->number = number;
    team->name = name_buffer;
    team->route_time_secs = route_time;
    team->control_point_qty = control_point_qty;

    return exit_code;
}

int rating_read(struct rating* rating, FILE* stream) {
    int exit_code = O_SUCCESS;

    int test_char = 0;
    char* buffer = NULL;
    size_t buffer_size = 0;
    struct team team = {0};

    if (!rating || !stream)
        return O_UNEXPECTED_NULL_ARG;

    while (exit_code == O_SUCCESS && !feof(stream)) {
        exit_code = team_read(&team, stream, &buffer, &buffer_size);
        if (exit_code == O_SUCCESS) {
            exit_code = rating_add(rating, &team);
            free(team.name);
        }

        test_char = fgetc(stream);
        if (test_char != EOF)
            ungetc(test_char, stream);
    }
    free(buffer);

    return exit_code;
}
