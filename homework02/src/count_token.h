#ifndef HW02_MATCHES_COUNTTOKEN_H_
#define HW02_MATCHES_COUNTTOKEN_H_

#include <stddef.h>
#include <string.h>
#include <assert.h>

static size_t count_token(const char* str, const char* token,
                         size_t* common_len) {
    assert(str && token && common_len);

    size_t token_len = strlen(token);
    size_t matches_count = 0;

    for (size_t idx = 0; str[idx] != '\0'; ++idx) {
        if (str[idx] == token[*common_len]) {
            *common_len += 1;
            if (*common_len == token_len) {
                ++matches_count;
                *common_len = 0;
            }
        } else {
            *common_len = 0;
        }
    }

    return matches_count;
}

#endif  // HW02_MATCHES_COUNTTOKEN_H_
