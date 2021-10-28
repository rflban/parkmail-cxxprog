#ifndef HW02_MATCHES_EXITCODES_H_
#define HW02_MATCHES_EXITCODES_H_

#include "collections/exitcodes.h"

#define EXIT_SUCCESS_ 0
#define EXIT_BAD_ALLOC_ 1
#define EXIT_INPUT_ERROR_ 2
#define EXIT_UNEXPECTED_NULL_ARG_ 3
#define EXIT_OPENDIR_ERROR_ 4
#define EXIT_OPENFILE_ERROR_ 5
#define EXIT_3RDPTY_ERROR_ 6

static inline int convert_exitcode_from_collections(int exitcode) {
    switch (exitcode) {
        case PHWC_EXIT_SUCCESS:
            return EXIT_SUCCESS_;
        case PHWC_EXIT_NULL_ARG:
            return EXIT_UNEXPECTED_NULL_ARG_;
        case PHWC_EXIT_BAD_ALLOC:
            return EXIT_BAD_ALLOC_;
        default:
            return EXIT_3RDPTY_ERROR_;
    }
}

#endif  // HW02_MATCHES_EXITCODES_H_
