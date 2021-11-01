#ifndef HW02_MATCHES_SCANDIR_H_
#define HW02_MATCHES_SCANDIR_H_

#include <assert.h>
#include <stddef.h>
#include <string.h>

#include "collections/string.h"

typedef struct vector_of_match_t vector_of_match_t ;

typedef struct match {
    string_t filename;
    size_t count;
} match_t;

int scan_dir(const char* dirpath, const char* token,
             vector_of_match_t* matches, size_t* best_of);

static int match_cmp(const void* a, const void* b) {
    assert(a && b && "NULL args are not allowed");

    int diff = ((const match_t*)b)->count - ((const match_t*)a)->count;
    if (diff == 0) {
        diff = strcmp(((const match_t*)a)->filename.data,
                      ((const match_t*)b)->filename.data);
    }
    return diff;
}

#endif  // HW02_MATCHES_SCANDIR_H_
