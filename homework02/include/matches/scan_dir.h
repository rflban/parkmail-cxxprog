#ifndef HW02_MATCHES_SCANDIR_H_
#define HW02_MATCHES_SCANDIR_H_

#include <stddef.h>

#include "collections/string.h"

typedef struct token_matches {
    string_t filename;
    size_t count;
} token_matches_t;

int scan_dir(const char* dirpath, const char* token,
            struct token_matches* matches, size_t* best_of);

#endif  // HW02_MATCHES_SCANDIR_H_
