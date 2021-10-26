#ifndef HW02_MATCHES_COUNTTOKENFILE_H_
#define HW02_MATCHES_COUNTTOKENFILE_H_

#define BUFFER_SIZE 120

#include <stdio.h>
#include <stdlib.h>

#include "counttoken.h"
#include "matches/exitcodes.h"

static int counttoken_file(FILE* stream, const char* token, size_t* count) {
    assert(stream && token && count);

    int exitcode = EXIT_SUCCESS_;
    char test_char = 0;
    char* buffer = NULL;
    size_t common_len = 0;
    size_t matches_count = 0;

    buffer = (char*)malloc(sizeof(char) * BUFFER_SIZE);
    if (!buffer) {
        return EXIT_BAD_ALLOC_;
    }

    while (!feof(stream)) {
        if (fgets(buffer, BUFFER_SIZE, stream) == NULL) {
            exitcode = EXIT_INPUT_ERROR_;
            break;
        }

        matches_count += counttoken(buffer, token, &common_len);

        test_char = fgetc(stream);
        if (test_char != EOF) {
            ungetc(test_char, stream);
        }
    }

    free(buffer);

    if (EXIT_SUCCESS_ == exitcode) {
        *count = matches_count;
    }

    return exitcode;
}

#endif  // HW02_MATCHES_COUNTTOKENFILE_H_
