#ifndef HW02_MATCHES_SCANDIR_H_
#define HW02_MATCHES_SCANDIR_H_

#include <stddef.h>

#include "collections/string.h"

typedef struct match {
    string_t filename;
    size_t count;
} match_t;

int scan_dir(const char* dirpath, const char* token,
             match_t* matches, size_t* best_of);

static int cmp_matches(const void* a, const void* b) {
    return ((const match_t*)a)->count - ((const match_t*)b)->count;
}


#endif  // HW02_MATCHES_SCANDIR_H_
