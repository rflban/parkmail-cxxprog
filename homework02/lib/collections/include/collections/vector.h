#ifndef PARKMAIL_HW_COLLECTIONS_VECTOR_H_
#define PARKMAIL_HW_COLLECTIONS_VECTOR_H_

#include <stddef.h>

#include "collections/exitcodes.h"
#include "collections/utils.h"

#define VECTOR_GROW_RATIO 2

#ifdef VECTOR_TYPE
#undef VECTOR_TYPE
#endif //VECTOR_TYPE
#ifdef VECTOR_FUN_PREFIX
#undef VECTOR_FUN_PREFIX
#endif //VECTOR_FUN_PREFIX

#define VECTOR_TYPE(T) XCONCAT(vector_of_, T)
#define VECTOR_FUN_PREFIX(T) XCONCAT(vector_of_, T)

#define ENABLE_VECTOR_OF(T)                                                     \
                                                                                \
typedef struct {                                                                \
    T* buffer;                                                                  \
    size_t size;                                                                \
    size_t capacity;                                                            \
} VECTOR_TYPE(T);                                                               \
                                                                                \
static int XCONCAT(VECTOR_FUN_PREFIX(T), _init)(VECTOR_TYPE(T)* self) {         \
    if (!self) {                                                                \
        return PHWC_EXIT_NULL_ARG;                                              \
    }                                                                           \
                                                                                \
    self->size = 0;                                                             \
    self->buffer = NULL;                                                        \
    self->capacity = 0;                                                         \
                                                                                \
    return PHWC_EXIT_SUCCESS;                                                   \
}                                                                               \
                                                                                \
static int XCONCAT(VECTOR_FUN_PREFIX(T), _deinit)(VECTOR_TYPE(T)* self) {       \
    if (!self) {                                                                \
        return PHWC_EXIT_NULL_ARG;                                              \
    }                                                                           \
                                                                                \
    free(self->buffer);                                                         \
    return PHWC_EXIT_SUCCESS;                                                   \
}                                                                               \
                                                                                \
static int XCONCAT(VECTOR_FUN_PREFIX(T), _grow)(VECTOR_TYPE(T)* self) {         \
    if (!self) {                                                                \
        return PHWC_EXIT_NULL_ARG;                                              \
    }                                                                           \
                                                                                \
    size_t new_capacity = self->capacity;                                       \
                                                                                \
    if (new_capacity == 0) {                                                    \
        new_capacity = 1;                                                       \
    } else {                                                                    \
        new_capacity *= VECTOR_GROW_RATIO;                                      \
    }                                                                           \
                                                                                \
    T* new_buffer = (T*)realloc(                                                \
            self->buffer,                                                       \
            new_capacity * sizeof(T));                                          \
    if (!new_buffer) {                                                          \
        return PHWC_EXIT_BAD_ALLOC;                                             \
    }                                                                           \
                                                                                \
    self->buffer = new_buffer;                                                  \
    self->capacity = new_capacity;                                              \
    return PHWC_EXIT_SUCCESS;                                                   \
}                                                                               \
                                                                                \
static int XCONCAT(VECTOR_FUN_PREFIX(T), _add)(VECTOR_TYPE(T)* self, T val) {   \
    if (!self) {                                                                \
        return PHWC_EXIT_NULL_ARG;                                              \
    }                                                                           \
                                                                                \
    int exitcode = PHWC_EXIT_SUCCESS;                                           \
                                                                                \
    if (self->size >= self->capacity) {                                         \
        exitcode = XCONCAT(VECTOR_FUN_PREFIX(T), _grow)(self);                  \
    }                                                                           \
    if (exitcode == PHWC_EXIT_SUCCESS) {                                        \
        self->buffer[self->size++] = val;                                       \
    }                                                                           \
                                                                                \
    return exitcode;                                                            \
}                                                                               \

#endif  // PARKMAIL_HW_COLLECTIONS_VECTOR_H_
