#ifndef PARKMAIL_HW_COLLECTIONS_VECTOR_H_
#define PARKMAIL_HW_COLLECTIONS_VECTOR_H_

#include <stddef.h>
#include <stdlib.h>

#include "collections/returncodes.h"
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

#define ENABLE_VECTOR_OF(T)                                                    \
                                                                               \
typedef struct VECTOR_TYPE(T) {                                                \
    T* data;                                                                   \
    size_t size;                                                               \
    size_t capacity;                                                           \
} VECTOR_TYPE(T);                                                              \
                                                                               \
static inline                                                                  \
int XCONCAT(VECTOR_FUN_PREFIX(T), _init)(VECTOR_TYPE(T)* self) {               \
    if (!self) {                                                               \
        return COLLECTIONS_NULLARG_ERROR;                                      \
    }                                                                          \
                                                                               \
    self->size = 0;                                                            \
    self->data = NULL;                                                         \
    self->capacity = 0;                                                        \
                                                                               \
    return COLLECTIONS_SUCCESS;                                                \
}                                                                              \
                                                                               \
static inline                                                                  \
void XCONCAT(VECTOR_FUN_PREFIX(T), _deinit)(VECTOR_TYPE(T)* self) {            \
    if (!self) {                                                               \
        return;                                                                \
    }                                                                          \
                                                                               \
    free(self->data);                                                          \
    self->size = 0;                                                            \
    self->data = NULL;                                                         \
    self->capacity = 0;                                                        \
    return;                                                                    \
}                                                                              \
                                                                               \
static inline                                                                  \
int XCONCAT(VECTOR_FUN_PREFIX(T), _grow)(VECTOR_TYPE(T)* self) {               \
    if (!self) {                                                               \
        return COLLECTIONS_NULLARG_ERROR;                                      \
    }                                                                          \
                                                                               \
    size_t new_capacity = self->capacity;                                      \
                                                                               \
    if (new_capacity == 0) {                                                   \
        new_capacity = 1;                                                      \
    } else {                                                                   \
        new_capacity *= VECTOR_GROW_RATIO;                                     \
    }                                                                          \
                                                                               \
    T* new_buffer = (T*)realloc(                                               \
            self->data,                                                        \
            new_capacity * sizeof(T));                                         \
    if (!new_buffer) {                                                         \
        return COLLECTIONS_ALLOC_ERROR;                                        \
    }                                                                          \
                                                                               \
    self->data = new_buffer;                                                   \
    self->capacity = new_capacity;                                             \
    return COLLECTIONS_SUCCESS;                                                \
}                                                                              \
                                                                               \
static inline                                                                  \
int XCONCAT(VECTOR_FUN_PREFIX(T), _add)(VECTOR_TYPE(T)* self, T val) {         \
    if (!self) {                                                               \
        return COLLECTIONS_NULLARG_ERROR;                                      \
    }                                                                          \
                                                                               \
    int rc = COLLECTIONS_SUCCESS;                                              \
                                                                               \
    if (self->size >= self->capacity) {                                        \
        rc = XCONCAT(VECTOR_FUN_PREFIX(T), _grow)(self);                       \
    }                                                                          \
    if (rc == COLLECTIONS_SUCCESS) {                                           \
        self->data[self->size++] = val;                                        \
    }                                                                          \
                                                                               \
    return rc;                                                                 \
}                                                                              \

#endif  // PARKMAIL_HW_COLLECTIONS_VECTOR_H_
