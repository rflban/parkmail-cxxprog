#include "matches/scan_dir.h"

#include <stdio.h>
#include <dirent.h>
#include <assert.h>

#include "counttoken_file.h"
#include "matches/exitcodes.h"
#include "collections/string.h"
#include "collections/vector.h"

ENABLE_VECTOR_OF(token_matches_t);

static size_t find_worst(const vector_of_token_matches_t* matches) {
    assert(matches && "NULL argument is not allowed");
    assert(matches->size && "Empty matches list is not allowed");

    size_t idx;
    size_t idx_of_min = 0;
    token_matches_t* raw_matches = matches->buffer;

    for (idx = 1; idx < matches->size; ++idx) {
        if (raw_matches[idx].count < raw_matches[idx_of_min].count) {
            idx_of_min = idx;
        }
    }

    return idx;
}

static int swap_with_worst(vector_of_token_matches_t* best_matches,
                           size_t token_count, const char* current_filename) {
    assert(best_matches && current_filename && "NULL args are not allowed");

    size_t swap_pos = find_worst(best_matches);

    if (best_matches->buffer[swap_pos].count < token_count) {
        string_t fname;
        int exitcode = convert_exitcode_from_collections(
            string_init(&fname, current_filename)
        );
        if (exitcode != EXIT_SUCCESS_) {
            return exitcode;
        }
        token_matches_t match = { fname, token_count };
        string_deinit(&(best_matches->buffer[swap_pos].filename));
        best_matches->buffer[swap_pos] = match;
    }

    return EXIT_SUCCESS_;
}

static int scan_file(const char* dent_name, const char* token,
                     vector_of_token_matches_t* best_matches, size_t* best_of) {
    assert(dent_name && token && best_matches && "NULL args are not allowed");

    int exitcode = EXIT_SUCCESS_;
    size_t token_count;
    FILE* stream = NULL;

    if (!(stream = fopen(dent_name, "r"))) {
        return EXIT_OPENFILE_ERROR_;
    }

    exitcode = counttoken_file(stream, token, &token_count);
    fclose(stream);
    if (exitcode != EXIT_SUCCESS_) {
        return exitcode;
    }

    if (best_matches->size < *best_of) {
        string_t fname;
        exitcode = convert_exitcode_from_collections(
            string_init(&fname, dent_name)
        );
        if (exitcode != EXIT_SUCCESS_) {
            return exitcode;
        }
        token_matches_t match = { fname, token_count };

        exitcode = convert_exitcode_from_collections(
            vector_of_token_matches_t_add(best_matches, match)
        );
        if (exitcode != EXIT_SUCCESS_) {
            string_deinit(&fname);
            return exitcode;
        }
    } else {
        exitcode = swap_with_worst(best_matches, token_count, dent_name);
    }

    return exitcode;
}

int scan_dir(const char* dirpath, const char* token,
             struct token_matches* matches, size_t* best_of) {
    if (!dirpath || !token || !matches || !best_of) {
        return EXIT_UNEXPECTED_NULL_ARG_;
    }

    int exitcode = EXIT_SUCCESS_;
    DIR* dir = NULL;
    vector_of_token_matches_t best_matches;
    vector_of_token_matches_t_init(&best_matches);

    dir = opendir(dirpath);
    if (!dir) {
        return EXIT_OPENDIR_ERROR_;
    }

    FILE* stream = NULL;
    size_t token_count = 0;
    struct dirent* dent = NULL;
    while (exitcode == EXIT_SUCCESS_ && (dent = readdir(dir))) {
        exitcode = scan_file(dent->d_name, token, &best_matches, best_of);
    }
    closedir(dir);

    if (best_matches.size > 0) {
        memcpy(matches, best_matches.buffer,
               sizeof(token_matches_t) * best_matches.size);
        *best_of = best_matches.size;
    }

    vector_of_token_matches_t_deinit(&best_matches);
    return exitcode;
}
