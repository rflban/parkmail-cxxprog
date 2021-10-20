#include "rating.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "exitcodes.h"
#include "team.h"

struct rating {
    struct team* buffer;
    size_t buffer_size;
    size_t size;
};

static void sift_down(struct team* heap, size_t size, size_t parent) {
    assert(heap);

    char subtree_is_heap = 0;
    struct team temp;

    while (!subtree_is_heap && ((parent << 1) + 1 < size)) {
        size_t left = (parent << 1) + 1;
        size_t right = (parent << 1) + 2;
        size_t min_child = left;

        if (right < size && team_cmp(heap[right], heap[left]) > 0)
            min_child = right;

        subtree_is_heap = team_cmp(heap[parent], heap[min_child]) >= 0;

        if (!subtree_is_heap) {
            temp = heap[parent];
            heap[parent] = heap[min_child];
            heap[min_child] = temp;

            parent = min_child;
        }
    }
}

static void build_heap(struct team* heap, size_t size) {
    assert(heap);
    for (int idx = size / 2; idx >= 0; --idx)
        sift_down(heap, size, idx);
}

static int rating_grow_buffer(struct rating* rating) {
    if (!rating)
        return O_UNEXPECTED_NULL_ARG;

    size_t new_size = rating->buffer_size * 2;
    if (new_size == 0)
        new_size = 1;

    struct team* new_buffer = (struct team*)realloc(
        rating->buffer, new_size * sizeof(rating->buffer[0]));
    if (!new_buffer)
        return O_BAD_ALLOC;

    rating->buffer = new_buffer;
    rating->buffer_size = new_size;

    return O_SUCCESS;
}

struct rating* rating_create() {
    struct rating* rating = (struct rating*)malloc(sizeof(struct rating));

    if (rating) {
        rating->buffer = NULL;
        rating->buffer_size = 0;
        rating->size = 0;
    }

    return rating;
}

void rating_destroy(struct rating** rating) {
    if (rating) {
        for (size_t idx = 0; idx < (*rating)->size; ++idx)
            free((*rating)->buffer[idx].name);

        free((*rating)->buffer);
        free(*rating);
        *rating = NULL;
    }
}

int rating_get_size(const struct rating* rating, size_t* size) {
    if (!rating || !size)
        return O_UNEXPECTED_NULL_ARG;

    *size = rating->size;
    return O_SUCCESS;
}

int rating_add(struct rating* rating, const struct team* result) {
    int exit_code = O_SUCCESS;

    if (!rating || !result)
        return O_UNEXPECTED_NULL_ARG;

    if (!rating->buffer && rating->buffer_size != 0)
        return O_DAMAGED_DATA;

    size_t name_buffer_size = strlen(result->name) + 1;
    char* name_buffer = (char*)malloc(name_buffer_size);
    if (!name_buffer)
        return O_BAD_ALLOC;

    if (rating->size >= rating->buffer_size)
        exit_code = rating_grow_buffer(rating);
    if (exit_code != O_SUCCESS) {
        free(name_buffer);
        return exit_code;
    }

    strncpy(name_buffer, result->name, name_buffer_size);
    rating->buffer[rating->size] = *result;
    rating->buffer[rating->size].name = name_buffer;
    rating->size += 1;

    return exit_code;
}

int rating_pick_best_of(const struct rating* rating, struct team* best_results,
                        size_t qty) {
    struct team* heap = NULL;

    if (!rating || !best_results) {
        return O_UNEXPECTED_NULL_ARG;
    }
    if (!rating->buffer || rating->size < qty) {
        return O_DAMAGED_DATA;
    }

    heap = (struct team*)malloc(rating->size * sizeof(struct team));
    if (!heap)
        return O_BAD_ALLOC;

    memcpy(heap, rating->buffer, rating->size * sizeof(struct team));
    build_heap(heap, rating->size);

    struct team temp;
    size_t heap_size = rating->size;
    for (size_t idx = 0; idx < qty; ++idx) {
        temp = heap[0];
        heap[0] = heap[rating->size - 1 - idx];
        heap[rating->size - 1 - idx] = temp;

        --heap_size;
        sift_down(heap, heap_size, 0);
    }

    for (size_t idx = 0; idx < qty; ++idx)
        best_results[idx] = heap[rating->size - 1 - idx];

    free(heap);

    return O_SUCCESS;
}
