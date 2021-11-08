#ifndef HW02_MATCHES_COUNTTOKENFILE_H_
#define HW02_MATCHES_COUNTTOKENFILE_H_

#define BUFFER_SIZE 120

#include <stdio.h>
#include <stdlib.h>

#include "count_token.h"
#include "matches/returncodes.h"

static int count_token_file(FILE* stream, const char* token, size_t* count) {
    assert(stream && token && count);

    int exitcode = MATCHES_SUCCESS;
    int test_char = 0;
    char* buffer = NULL;
    size_t common_len = 0;
    size_t matches_count = 0;

    buffer = (char*)malloc(sizeof(char) * BUFFER_SIZE);
    if (!buffer) {
        return COLLECTIONS_ALLOC_ERROR;
    }

    test_char = fgetc(stream);
    if (test_char != EOF) {
        ungetc(test_char, stream);
    }
    while (!feof(stream)) {
        if (fgets(buffer, BUFFER_SIZE, stream) == NULL) {
            exitcode = MATCHES_INPUT_ERROR;
            break;
        }

        matches_count += count_token(buffer, token, &common_len);

        test_char = fgetc(stream);
        if (test_char != EOF) {
            ungetc(test_char, stream);
        }
    }

    free(buffer);

    if (MATCHES_SUCCESS == exitcode) {
        *count = matches_count;
    }

    return exitcode;
}

#endif  // HW02_MATCHES_COUNTTOKENFILE_H_
