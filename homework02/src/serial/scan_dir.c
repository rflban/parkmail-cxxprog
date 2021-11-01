#include "matches/scan_dir.h"

#include <dirent.h>
#include <assert.h>

#include "util.h"
#include "counttoken_file.h"
#include "matches/returncodes.h"
#include "collections/string.h"
#include "collections/vector.h"

ENABLE_VECTOR_OF(match_t);

static int scan_file(const char* dent_name, const char* token,
                     vector_of_match_t* best_matches) {
    assert(dent_name && token && best_matches && "NULL args are not allowed");

    int rc = MATCHES_SUCCESS;
    size_t token_count;
    FILE* stream = NULL;

    if (!(stream = fopen(dent_name, "r"))) {
        return MATCHES_OPENFILE_ERROR;
    }

    rc = counttoken_file(stream, token, &token_count);
    fclose(stream);
    if (rc != MATCHES_SUCCESS) {
        return rc;
    }

    string_t fname;
    rc = convert_returncode_from_collections(
        string_init(&fname, dent_name)
    );
    if (rc != MATCHES_SUCCESS) {
        return rc;
    }
    match_t match = { fname, token_count };

    rc = convert_returncode_from_collections(
        vector_of_match_t_add(best_matches, match)
    );
    if (rc != MATCHES_SUCCESS) {
        string_deinit(&fname);
        return rc;
    }

    return rc;
}

int scan_dir(const char* dirpath, const char* token,
             vector_of_match_t* best_matches, size_t* best_of) {
    if (!dirpath || !token || !best_matches || !best_of) {
        return MATCHES_NULLARG_ERROR;
    }

    int rc = MATCHES_SUCCESS;
    vector_of_match_t all_matches;
    rc = convert_returncode_from_collections(
        vector_of_match_t_init(&all_matches)
    );
    if (rc != MATCHES_SUCCESS) {
        return rc;
    }

    char init_dirpath[PATH_MAX] = "";
    if (!getcwd(init_dirpath, sizeof(init_dirpath))) {
        vector_of_match_t_deinit(&all_matches);
        return MATCHES_OPENDIR_ERROR;
    }
    chdir(dirpath);

    DIR* dir = NULL;

    dir = opendir(".");
    if (!dir) {
        vector_of_match_t_deinit(&all_matches);
        chdir(init_dirpath);
        return MATCHES_OPENDIR_ERROR;
    }

    FILE* stream = NULL;
    size_t token_count = 0;
    struct dirent* dent = NULL;
    while (rc == MATCHES_SUCCESS && (dent = readdir(dir))) {
        if (strcmp(dent->d_name, ".") == 0 ||
            strcmp(dent->d_name, "..") == 0) {
            continue;
        }

        if (!is_regular_file(dent->d_name)) {
            rc = MATCHES_ISNOTFILE_ERROR;
        } else {
            rc = scan_file(dent->d_name, token, &all_matches);
        }
    }
    closedir(dir);

    if (rc == MATCHES_SUCCESS && all_matches.size > 0) {
        sort(all_matches.data, all_matches.size, sizeof(match_t), &match_cmp);
        if (all_matches.size < *best_of) {
            *best_of = all_matches.size;
        }
        size_t idx;
        for (idx = 0; idx < *best_of; ++idx) {
            vector_of_match_t_add(best_matches, all_matches.data[idx]);
        }
        for (; idx < all_matches.size; ++idx) {
            string_deinit(&all_matches.data[idx].filename);
        }
    }

    chdir(init_dirpath);
    vector_of_match_t_deinit(&all_matches);

    return rc;
}
