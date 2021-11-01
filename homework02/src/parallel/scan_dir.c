#include "parallel/scan_dir_helper.h"

int scan_dir(const char* dirpath, const char* token,
             vector_of_match_t* best_matches, size_t* best_of) {
    if (!dirpath || !token || !best_matches || !best_of) {
        return MATCHES_NULLARG_ERROR;
    }

    int rc = MATCHES_SUCCESS;
    vector_of_string_t files;
    vector_of_string_t_init(&files);

    char init_dirpath[PATH_MAX] = "";
    if (!getcwd(init_dirpath, sizeof(init_dirpath))) {
        return MATCHES_OPENDIR_ERROR;
    }
    chdir(dirpath);

    rc = get_filenames(&files);
    if (rc != MATCHES_SUCCESS) {
        vector_of_string_t_deinit(&files);
        chdir(init_dirpath);
        return rc;
    }

    size_t files_qty = files.size;
    match_t* all_matches = (match_t*)mmap(NULL, files_qty * sizeof(match_t),
                                          PROT_READ | PROT_WRITE,
                                          MAP_SHARED | MAP_ANONYMOUS,
                                          -1, 0);
    if (all_matches == MAP_FAILED) {
        rc = MATCHES_MMAP_ERROR;
    } else {
        for (size_t idx = 0; idx < files_qty; ++idx) {
            all_matches[idx].filename = files.data[idx];
        }
    }
    vector_of_string_t_deinit(&files);

    if (*best_of > files_qty) {
        *best_of = files_qty;
    }

    if (rc == MATCHES_SUCCESS) {
        rc = scan_files(all_matches, files_qty,
                              token, best_matches, *best_of);
    }

    for (size_t idx = 0; idx < files_qty; ++idx) {
        string_deinit(&all_matches[idx].filename);
    }
    munmap(all_matches, files_qty * sizeof(match_t));

    chdir(init_dirpath);
    return rc;
}
