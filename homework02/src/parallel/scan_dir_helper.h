#ifndef HW02_MATCHES_SCANDIR_HELPER_H_
#define HW02_MATCHES_SCANDIR_HELPER_H_

#include "matches/scan_dir.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <assert.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/sysinfo.h>

#include "counttoken_file.h"
#include "matches/exitcodes.h"
#include "collections/string.h"
#include "collections/vector.h"

ENABLE_VECTOR_OF(string_t);

struct runner_args {
    size_t start;
    size_t end;
    size_t files_qty;
    match_t* matches;
    const char* token;
};

static int perform_task(struct runner_args* args) {
    assert(args && "NULL argument is not allowed");

    int exitcode = EXIT_SUCCESS_;
    FILE* stream = NULL;

    for (size_t idx = args->start; idx < args->end; ++idx) {
        stream = fopen(args->matches[idx].filename.buffer, "r");
        if (!stream) {
            return EXIT_INPUT_ERROR_;
        }

        exitcode = counttoken_file(stream, args->token,
                                   &args->matches[idx].count);
        fclose(stream);
        if (exitcode != EXIT_SUCCESS_) {
            return exitcode;
        }
    }

    return EXIT_SUCCESS_;
}

static pid_t run_fork(struct runner_args args, int* exitcode) {
    assert(exitcode && "NULL argument is not allowed");

    pid_t child = fork();

    if (child == 0) {
        *exitcode = perform_task(&args);
        exit(EXIT_SUCCESS);
    }

    return child;
}

static int collect_result(match_t* matches, size_t matches_qty,
                          match_t* best_matches, size_t best_of,
                          int* exitcodes, size_t nforks) {
    assert(matches && best_matches && exitcodes && "NULL args are not allowed");
    assert(matches_qty >= best_of && "Size of best matches must "
                                     "be le than all matches quantity");

    for (int idx = 0; idx < nforks; ++idx) {
        if (exitcodes[idx] != EXIT_SUCCESS_) {
            return exitcodes[idx];
        }
    }

    qsort(matches, matches_qty, sizeof(match_t), &cmp_matches);
    memcpy(best_matches, matches, sizeof(match_t) * best_of);

    for (size_t idx = 0; idx < best_of; ++idx) {
        string_init(&best_matches[idx].filename, matches[idx].filename.buffer);
    }

    return EXIT_SUCCESS_;
}

static int scan_files(match_t* all_matches, size_t files_qty,
                      const char* token,
                      match_t* best_matches, size_t best_of) {
    assert(all_matches && token && best_matches &&
           "NULL args are not allowed");
    assert(files_qty >= best_of && "Size of best matches must "
                                   "be le than files quantity");

    int exitcode = EXIT_SUCCESS_;
    size_t nprocs = (size_t)get_nprocs();
    size_t nforks = nprocs <= files_qty ? nprocs : files_qty;

    int* exitcodes = (int*)mmap(NULL, nforks * sizeof(int),
                                PROT_READ | PROT_WRITE,
                                MAP_SHARED | MAP_ANONYMOUS,
                                -1, 0);
    if (exitcodes == MAP_FAILED) {
        return EXIT_MMAP_ERROR_;
    }

    size_t step = files_qty / nforks;
    size_t unasigned_qty = files_qty % nforks;
    size_t asigned_qty = 0;
    struct runner_args runner_args = {
        .matches = all_matches, .files_qty = files_qty, .token = token
    };

    for (size_t idx = 0; idx < nforks; ++idx) {
        runner_args.start = asigned_qty + step * idx;
        if (unasigned_qty > 0) {
            ++asigned_qty;
            --unasigned_qty;
        }
        runner_args.end = asigned_qty + step * (idx + 1);

        run_fork(runner_args, exitcodes + idx);
    }

    int status;
    pid_t child;

    while ((child = wait(&status)) != -1 && errno != ECHILD)
    {
        if (!WIFEXITED(status)) {
            exitcode = EXIT_CHILD_FAILED_;
        }
    }
    if (errno != ECHILD)
    {
        exitcode = EXIT_WAIT_ERROR_;
    }

    if (exitcode == EXIT_SUCCESS_) {
        exitcode = collect_result(all_matches, files_qty, best_matches,
                                  best_of, exitcodes, nforks);
    }

    munmap(exitcodes, nforks * sizeof(int));

    return exitcode;
}

static int get_filenames(const char* dirpath, vector_of_string_t* filenames) {
    assert(dirpath && filenames && "NULL args are not allowed");

    int exitcode = EXIT_SUCCESS_;
    DIR* dir = NULL;
    struct dirent* dent = NULL;

    dir = opendir(dirpath);
    if (!dir) {
        return EXIT_OPENDIR_ERROR_;
    }

    while (exitcode == EXIT_SUCCESS_ && (dent = readdir(dir))) {
        string_t file;
        exitcode = convert_exitcode_from_collections(
            string_init(&file, dent->d_name)
        );

        if (exitcode == EXIT_SUCCESS_) {
            exitcode = convert_exitcode_from_collections(
                vector_of_string_t_add(filenames, file)
            );
        }
    }
    closedir(dir);

    return exitcode;
}

#endif  // HW02_MATCHES_SCANDIR_HELPER_H_
