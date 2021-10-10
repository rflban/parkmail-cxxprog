#include "rating.h"

#include <stdlib.h>
#include <string.h>

#include "exitcodes.h"
#include "teamresult.h"

struct rating {
    struct team_result* buffer;
    unsigned int buffer_size;
    unsigned int size;
};

static int sift_down(struct team_result* heap, unsigned int size,
                     unsigned int parent) {
    int exit_code = O_SUCCESS;
    char subtree_is_heap = 0;
    struct team_result temp;

    if (!heap)
        exit_code = O_UNEXPECTED_NULL_ARG;
    else
        while (!subtree_is_heap && ((parent << 1) + 1 < size)) {
            unsigned int left = (parent << 1) + 1;
            unsigned int right = (parent << 1) + 2;
            unsigned int min_child = left;

            if (right < size && team_result_cmp(heap[right], heap[left]) > 0)
                min_child = right;

            subtree_is_heap =
                team_result_cmp(heap[parent], heap[min_child]) >= 0;

            if (!subtree_is_heap) {
                temp = heap[parent];
                heap[parent] = heap[min_child];
                heap[min_child] = temp;

                parent = min_child;
            }
        }

    return exit_code;
}

static int build_heap(struct team_result* heap, unsigned int size) {
    int exit_code = O_SUCCESS;

    if (!heap)
        exit_code = O_UNEXPECTED_NULL_ARG;
    else
        for (int idx = size / 2; idx >= 0; --idx)
            exit_code = sift_down(heap, size, idx);

    return exit_code;
}

static int grow_rating_buffer(struct rating* rating) {
    int exit_code = O_SUCCESS;

    if (!rating)
        exit_code = O_UNEXPECTED_NULL_ARG;
    else {
        unsigned int new_size = rating->buffer_size * 2;

        if (new_size == 0)
            new_size = 1;

        struct team_result* new_buffer = (struct team_result*)realloc(
            rating->buffer, new_size * sizeof(rating->buffer[0]));

        if (!new_buffer)
            exit_code = O_BAD_ALLOC;
        else {
            rating->buffer = new_buffer;
            rating->buffer_size = new_size;
        }
    }

    return exit_code;
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
        for (unsigned int idx = 0; idx < (*rating)->size; ++idx)
            free((*rating)->buffer[idx].name);

        free((*rating)->buffer);
        free(*rating);
        *rating = NULL;
    }
}

int rating_size(const struct rating* rating, unsigned int* size) {
    if (!rating || !size)
        return O_UNEXPECTED_NULL_ARG;

    *size = rating->size;
    return O_SUCCESS;
}

int rating_add(struct rating* rating, const struct team_result* result) {
    int exit_code = O_SUCCESS;

    if (!rating || !result)
        exit_code = O_UNEXPECTED_NULL_ARG;
    else if (!rating->buffer && rating->buffer_size != 0)
        exit_code = O_DAMAGED_DATA;
    else if (rating->size >= rating->buffer_size)
        exit_code = grow_rating_buffer(rating);

    if (exit_code == O_SUCCESS) {
        size_t name_buffer_size = strlen(result->name) + 1;
        char* name_buffer = (char*)malloc(name_buffer_size);
        if (!name_buffer)
            exit_code = O_BAD_ALLOC;
        else {
            strncpy(name_buffer, result->name, name_buffer_size);
            rating->buffer[rating->size] = *result;
            rating->buffer[rating->size].name = name_buffer;
            rating->size += 1;
        }
    }

    return exit_code;
}

int rating_pick_best_of(const struct rating* rating,
                        struct team_result* best_results, unsigned int qty) {
    int exit_code = O_SUCCESS;
    struct team_result* heap = NULL;

    if (!rating || !best_results)
        exit_code = O_UNEXPECTED_NULL_ARG;
    else if (!rating->buffer || rating->size < qty)
        exit_code = O_DAMAGED_DATA;
    else {
        heap = (struct team_result*)malloc(rating->size *
                                           sizeof(struct team_result));
        if (!heap)
            exit_code = O_BAD_ALLOC;
        else {
            memcpy(heap, rating->buffer,
                   rating->size * sizeof(struct team_result));

            exit_code = build_heap(heap, rating->size);

            if (exit_code != O_SUCCESS)
                free(heap);
        }
    }

    if (exit_code == O_SUCCESS) {
        unsigned int heap_size = rating->size;
        struct team_result temp;

        for (unsigned int idx = 0; exit_code == O_SUCCESS && idx < qty; ++idx) {
            temp = heap[0];
            heap[0] = heap[rating->size - 1 - idx];
            heap[rating->size - 1 - idx] = temp;

            --heap_size;
            exit_code = sift_down(heap, heap_size, 0);
        }

        if (exit_code == O_SUCCESS) {
            for (unsigned int idx = 0; idx < qty; ++idx)
                best_results[idx] = heap[rating->size - 1 - idx];
        }

        free(heap);
    }

    return exit_code;
}
