#include "collections/string.h"

#include <stdlib.h>
#include <string.h>

#include "collections/returncodes.h"

int string_init(string_t* self, const char* str) {
    if (!self || !str) {
        return COLLECTIONS_NULLARG_ERROR;
    }

    size_t str_len = strlen(str);
    char* buffer = (char*)malloc(sizeof(char) * (str_len + 1));
    if (!buffer) {
        return COLLECTIONS_ALLOC_ERROR;
    }

    memcpy(buffer, str, str_len + 1);
    self->data = buffer;

    return COLLECTIONS_SUCCESS;
}

void string_deinit(string_t* self) {
    if (!self) {
        return;
    }

    free(self->data);
    self->data = NULL;
}
