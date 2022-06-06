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

#include "util.h"
#include "count_token_file.h"
#include "matches/returncodes.h"
#include "collections/string.h"
#include "collections/vector.h"

struct runner_args {
    size_t start;
    size_t end;
    size_t files_qty;
    match_t* matches;
    const char* token;
};

static int perform_task(struct runner_args* args) {
    assert(args && "NULL argument is not allowed");

    int rc = MATCHES_SUCCESS;
    FILE* stream = NULL;

    for (size_t idx = args->start; idx < args->end; ++idx) {
        stream = fopen(args->matches[idx].filename.data, "r");
        if (!stream) {
            rc = MATCHES_OPENFILE_ERROR;
            break;
        }

        rc = count_token_file(stream, args->token,
                             &args->matches[idx].count);
        fclose(stream);
        if (rc != MATCHES_SUCCESS) {
            break;
        }
    }
    for (size_t idx = 0; idx < args->files_qty; ++idx) {
        string_t to_delete = args->matches[idx].filename;
        string_deinit(&to_delete);
    }

    return rc;
}

static pid_t run_fork(struct runner_args args, int* rc) {
    assert(rc && "NULL argument is not allowed");

    pid_t child = fork();

    if (child == 0) {
        *rc = perform_task(&args);
        exit(EXIT_SUCCESS);
    } else if (child < 0) {
        *rc = MATCHES_FORK_ERROR;
    }

    return child;
}

static int collect_result(match_t* matches, size_t matches_qty,
                          vector_of_match_t* best_matches, size_t best_of,
                          int* returncodes, size_t nforks) {
    assert(matches && best_matches && returncodes &&
           "NULL args are not allowed");
    assert(matches_qty >= best_of && "Size of best matches must "
                                     "be le than all matches quantity");

    for (int idx = 0; idx < nforks; ++idx) {
        if (returncodes[idx] != MATCHES_SUCCESS) {
            return returncodes[idx];
        }
    }

    sort(matches, matches_qty, sizeof(match_t), &match_cmp);

    for (size_t idx = 0; idx < best_of; ++idx) {
        string_t filename;
        string_init(&filename, matches[idx].filename.data);
        match_t match = {.filename = filename, .count = matches[idx].count};
        vector_of_match_t_add(best_matches, match);
    }

    return MATCHES_SUCCESS;
}

static int scan_files(match_t* all_matches, size_t files_qty,
                      const char* token,
                      vector_of_match_t* best_matches, size_t best_of) {
    assert(all_matches && token && best_matches &&
           "NULL args are not allowed");
    assert(files_qty >= best_of && "Size of best matches must "
                                   "be le than files quantity");

    int rc = MATCHES_SUCCESS;
    size_t nprocs = (size_t)get_nprocs();
    size_t nforks = nprocs <= files_qty ? nprocs : files_qty;

    int* returncodes = (int*)mmap(NULL, nforks * sizeof(int),
                                PROT_READ | PROT_WRITE,
                                MAP_SHARED | MAP_ANONYMOUS,
                                -1, 0);
    if (returncodes == MAP_FAILED) {
        return MATCHES_MMAP_ERROR;
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

        run_fork(runner_args, returncodes + idx);
    }

    int status;
    while (wait(&status) != -1) {
        if (!WIFEXITED(status)) {
            rc = MATCHES_CHILD_ERROR;
        }
    }
    if (errno != ECHILD) {
        rc = MATCHES_WAIT_ERROR;
    }

    if (rc == MATCHES_SUCCESS) {
        rc = collect_result(all_matches, files_qty, best_matches,
                            best_of, returncodes, nforks);
    }

    if (0 != munmap(returncodes, nforks * sizeof(int))) {
        rc = MATCHES_MUNMAP_ERROR;
    }

    return rc;
}

static int get_filenames(vector_of_string_t* filenames) {
    assert(filenames && "NULL args are not allowed");

    int rc = MATCHES_SUCCESS;
    DIR* dir = NULL;
    struct dirent* dent = NULL;

    dir = opendir(".");
    if (!dir) {
        return MATCHES_OPENDIR_ERROR;
    }

    size_t read_counter = 0;
    while (rc == MATCHES_SUCCESS && (dent = readdir(dir))) {
        if (strcmp(dent->d_name, ".") == 0 ||
            strcmp(dent->d_name, "..") == 0) {
            continue;
        }

        if (!is_regular_file(dent->d_name)) {
            rc = MATCHES_ISNOTFILE_ERROR;
            break;
        }

        string_t file;
        rc = convert_returncode_from_collections(
            string_init(&file, dent->d_name)
        );

        if (rc == MATCHES_SUCCESS) {
            rc = convert_returncode_from_collections(
                vector_of_string_t_add(filenames, file)
            );
        }

        ++read_counter;
    }
    closedir(dir);

    if (MATCHES_SUCCESS == rc && read_counter == 0) {
        rc = MATCHES_EMPTYDIR_ERROR;
    }

    if (MATCHES_SUCCESS != rc) {
        for (size_t idx = 0; idx < filenames->size; ++idx) {
            string_deinit(filenames->data + idx);
        }
    }

    return rc;
}

int scan_dir(const char* dirpath, const char* token,
             vector_of_match_t* best_matches, size_t* best_of) {
    if (!dirpath || !token || !best_matches || !best_of) {
        return MATCHES_NULLARG_ERROR;
    }

    char init_dirpath[PATH_MAX] = "";
    if (!getcwd(init_dirpath, sizeof(init_dirpath))) {
        return MATCHES_OPENDIR_ERROR;
    }
    if (chdir(dirpath)) {
        return MATCHES_OPENDIR_ERROR;
    }

    int rc = MATCHES_SUCCESS;
    vector_of_string_t files;
    vector_of_string_t_init(&files);

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

    if (0 != munmap(all_matches, files_qty * sizeof(match_t))) {
        rc = MATCHES_MUNMAP_ERROR;
    }

    chdir(init_dirpath);
    return rc;
}
