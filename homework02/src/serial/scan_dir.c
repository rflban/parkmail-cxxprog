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
    while ((dent = readdir(dir))) {
        if (!(stream = fopen(dent->d_name, "r"))) {
            exitcode = EXIT_OPENFILE_ERROR_;
            break;
        }
        exitcode = counttoken_file(stream, token, &token_count);
        fclose(stream);
        if (exitcode != EXIT_SUCCESS_) {
            break;
        }

        if (best_matches.size < *best_of) {
            string_t fname;
            if (PHWC_EXIT_SUCCESS != string_init(&fname, dent->d_name)) {
                exitcode = EXIT_3RDPTY_ERROR_;
                break;
            }
            token_matches_t match = { fname, token_count };

            if (PHWC_EXIT_SUCCESS != 
                    vector_of_token_matches_t_add(&best_matches, match)) {
                exitcode = EXIT_3RDPTY_ERROR_;
                string_deinit(&fname);
                break;
            }
        } else {
            size_t swap_pos = find_worst(&best_matches);

            if (best_matches.buffer[swap_pos].count < token_count) {
                string_t fname;
                if (PHWC_EXIT_SUCCESS != string_init(&fname, dent->d_name)) {
                    exitcode = EXIT_3RDPTY_ERROR_;
                    break;
                }
                token_matches_t match = { fname, token_count };
                string_deinit(&(best_matches.buffer[swap_pos].filename));
                best_matches.buffer[swap_pos] = match;
            }
        }
    }

    if (best_matches.size > 0) {
        memcpy(matches, best_matches.buffer,
               sizeof(token_matches_t) * best_matches.size);
        *best_of = best_matches.size;
    }

    vector_of_token_matches_t_deinit(&best_matches);
    return exitcode;
}
