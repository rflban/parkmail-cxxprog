#ifndef HW02_MATCHES_RETURNCODES_H_
#define HW02_MATCHES_RETURNCODES_H_

#include "collections/returncodes.h"

enum MATCHES_ERROR_CODES {
    MATCHES_SUCCESS,
    MATCHES_ALLOC_ERROR,
    MATCHES_INPUT_ERROR,
    MATCHES_NULLARG_ERROR,
    MATCHES_OPENDIR_ERROR,
    MATCHES_EMPTYDIR_ERROR,
    MATCHES_OPENFILE_ERROR,
    MATCHES_ISNOTFILE_ERROR,
    MATCHES_MMAP_ERROR,
    MATCHES_CHILD_ERROR,
    MATCHES_WAIT_ERROR,
    MATCHES_3RDPTY_ERROR,
};

static inline int convert_returncode_from_collections(int rc) {
    switch (rc) {
        case COLLECTIONS_SUCCESS:
            return MATCHES_SUCCESS;
        case COLLECTIONS_NULLARG_ERROR:
            return MATCHES_NULLARG_ERROR;
        case COLLECTIONS_ALLOC_ERROR:
            return MATCHES_ALLOC_ERROR;
        default:
            return MATCHES_3RDPTY_ERROR;
    }
}

#endif  // HW02_MATCHES_RETURNCODES_H_
