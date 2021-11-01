#include <stdio.h>
#include <stdlib.h>

#include "matches/scan_dir.h"
#include "matches/returncodes.h"
#include "collections/vector.h"
#include "collections/returncodes.h"

#define QTY_OF_BESTS 5

ENABLE_VECTOR_OF(match_t);

void print_usage(const char* pname);
void print_matches_err(int rc);
void print_collections_err(int rc);

int main(int argc, char** argv) {
    if (argc != 3) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    size_t best_of = QTY_OF_BESTS;
    vector_of_match_t matches;

    int rc = vector_of_match_t_init(&matches);
    if (rc != COLLECTIONS_SUCCESS) {
        print_collections_err(rc);
        return EXIT_FAILURE;
    }

    rc = scan_dir(argv[1], argv[2], &matches, &best_of);
    if (rc != MATCHES_SUCCESS) {
        vector_of_match_t_deinit(&matches);
        print_matches_err(rc);
        return EXIT_FAILURE;
    }

    for (size_t idx = 0; idx < best_of; ++idx) {
        printf("%s: %lu\n",
               matches.data[idx].filename.data, matches.data[idx].count);
        string_deinit(&matches.data[idx].filename);
    }
    vector_of_match_t_deinit(&matches);

    return EXIT_SUCCESS;
}

void print_usage(const char* pname) {
    fprintf(stderr, "Usage:\n\t%s <path> <token>\n", pname);
}

void print_matches_err(int rc) {
    switch(rc) {
        case EXIT_SUCCESS:
            break;
        case MATCHES_ALLOC_ERROR:
            perror("MATCHES: Internal memory allocation error");
            break;
        case MATCHES_NULLARG_ERROR:
            fprintf(stderr,
                    "MATCHES: Forbidden NULL pointer have been"
                    "passed to function parameters\n");
            break;
        case MATCHES_MMAP_ERROR:
            perror("MATCHES: Internal memory map error");
            break;
        case MATCHES_INPUT_ERROR:
            fprintf(stderr, "MATCHES: wrong input\n");
            break;
        case MATCHES_WAIT_ERROR:
            perror("MATCHES: Internal process wait error");
            break;
        case MATCHES_CHILD_ERROR:
            perror("MATCHES: Child process terminated with error");
            break;
        case MATCHES_OPENDIR_ERROR:
            perror("MATCHES: Can't open specified directory");
            break;
        case MATCHES_OPENFILE_ERROR:
            perror("MATCHES: Can't open file in specified directory");
            break;
        case MATCHES_ISNOTFILE_ERROR:
            fprintf(stderr,
                    "MATCHES: Not regular file has been "
                    "detected in specified directory\n");
            break;
        case MATCHES_3RDPTY_ERROR:
            fprintf(stderr, "MATCHES: Undefiend 3rd-party error\n");
            break;
        default:
            fprintf(stderr, "MATCHES: Undefiend error\n");
            break;
    }
}

void print_collections_err(int rc) {
    switch(rc) {
        case COLLECTIONS_SUCCESS:
            break;
        case COLLECTIONS_ALLOC_ERROR:
            perror("COLLECTIONS: Internal memory allocation error");
            break;
        case COLLECTIONS_NULLARG_ERROR:
            fprintf(stderr,
                    "COLLECTIONS: Forbidden NULL pointer have been"
                    "passed to function parameters\n");
            break;
        default:
            fprintf(stderr, "COLLECTIONS: Undefiend error\n");
            break;
    }
}
