#include "collections/string.h"

#include <stdlib.h>
#include <string.h>

#include "collections/exitcodes.h"

int string_init(string_t* self, const char* str) {
    if (!self || !str) {
        return PHWC_EXIT_NULL_ARG;
    }

    size_t str_len = strlen(str);
    char* buffer = (char*)malloc(sizeof(char) * (str_len + 1));
    if (!buffer) {
        return PHWC_EXIT_BAD_ALLOC;
    }

    memcpy(buffer, str, str_len + 1);
    self->buffer = buffer;

    return PHWC_EXIT_SUCCESS;
}

int string_deinit(string_t* self) {
    if (!self) {
        return PHWC_EXIT_NULL_ARG;
    }

    free(self->buffer);
    return PHWC_EXIT_SUCCESS;
}

const char* string_raw(string_t* self) {
    if (!self) {
        return NULL;
    }

    return self->buffer;
}
