#include "parallel/scan_dir_helper.h"

int scan_dir(const char* dirpath, const char* token,
             match_t* best_matches, size_t* best_of) {
    if (!dirpath || !token || !best_matches || !best_of) {
        return EXIT_UNEXPECTED_NULL_ARG_;
    }

    int exitcode = EXIT_SUCCESS_;
    vector_of_string_t files;
    vector_of_string_t_init(&files);

    exitcode = get_filenames(dirpath, &files);
    if (exitcode != EXIT_SUCCESS_) {
        vector_of_string_t_deinit(&files);
        return exitcode;
    }

    size_t files_qty = files.size;
    match_t* all_matches = (match_t*)mmap(NULL, files_qty * sizeof(match_t),
                                          PROT_READ | PROT_WRITE,
                                          MAP_SHARED | MAP_ANONYMOUS,
                                          -1, 0);
    if (all_matches == MAP_FAILED) {
        exitcode = EXIT_MMAP_ERROR_;
    } else {
        for (size_t idx = 0; idx < files_qty; ++idx) {
            all_matches[idx].filename = files.buffer[idx];
        }
    }
    vector_of_string_t_deinit(&files);

    if (*best_of > files_qty) {
        *best_of = files_qty;
    }

    if (exitcode == EXIT_SUCCESS_) {
        exitcode = scan_files(all_matches, files_qty, token, best_matches, *best_of);
    }

    for (size_t idx; idx < files_qty; ++idx) {
        string_deinit(&all_matches[idx].filename);
    }
    munmap(all_matches, files_qty * sizeof(match_t));

    return exitcode;
}
