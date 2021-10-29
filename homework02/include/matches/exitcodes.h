#ifndef HW02_MATCHES_EXITCODES_H_
#define HW02_MATCHES_EXITCODES_H_

#include "collections/exitcodes.h"

enum {
    EXIT_SUCCESS_,
    EXIT_BAD_ALLOC_,
    EXIT_INPUT_ERROR_,
    EXIT_UNEXPECTED_NULL_ARG_,
    EXIT_OPENDIR_ERROR_,
    EXIT_OPENFILE_ERROR_,
    EXIT_3RDPTY_ERROR_,
    EXIT_MMAP_ERROR_,
    EXIT_CHILD_FAILED_,
    EXIT_WAIT_ERROR_
};

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
