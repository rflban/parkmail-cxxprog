#include <gtest/gtest.h>

extern "C" {
#include "collections/vector.h"
#include "collections/returncodes.h"
}

const char SAMPLE_CHAR_VECTOR[] = {4, 5, 2, 1, 8, 3, 9, 7, 0, 6};
const int SAMPLE_INT_VECTOR[] = {4, 5, 2, 1, 8, 3, 9, 7, 0, 6};

ENABLE_VECTOR_OF(char)
ENABLE_VECTOR_OF(int)

TEST(VECTOR_OF_CHAR_INIT, NULL_ARG) {
    EXPECT_EQ(vector_of_char_init(NULL), COLLECTIONS_NULLARG_ERROR);
}

TEST(VECTOR_OF_CHAR_INIT, SUCCESS_INIT) {
    vector_of_char vector;
    EXPECT_EQ(vector_of_char_init(&vector), COLLECTIONS_SUCCESS);
    EXPECT_EQ(vector.size, 0);
    EXPECT_EQ(vector.data, nullptr);
    EXPECT_EQ(vector.capacity, 0);
    vector_of_char_deinit(&vector);
}

TEST(VECTOR_OF_CHAR_ADD, ADD_ONE_ITEM) {
    vector_of_char vector;
    vector_of_char_init(&vector);

    EXPECT_EQ(vector_of_char_add(&vector, 0), COLLECTIONS_SUCCESS);
    EXPECT_EQ(vector.size, 1);
    EXPECT_EQ(vector.data[0], 0);

    vector_of_char_deinit(&vector);
}

TEST(VECTOR_CHAR_ADD, FILL) {
    vector_of_char vector;
    vector_of_char_init(&vector);

    size_t vector_len = (sizeof(SAMPLE_CHAR_VECTOR) /
                         sizeof(SAMPLE_CHAR_VECTOR[0]));

    for (size_t idx = 0; idx < vector_len; ++idx) {
        vector_of_char_add(&vector, SAMPLE_CHAR_VECTOR[idx]);
    }

    EXPECT_EQ(vector.size, vector_len);
    for (size_t idx = 0; idx < vector_len; ++idx) {
        EXPECT_EQ(vector.data[idx], SAMPLE_CHAR_VECTOR[idx]);
    }

    vector_of_char_deinit(&vector);
}

TEST(VECTOR_OF_INT_INIT, NULL_ARG) {
    EXPECT_EQ(vector_of_int_init(NULL), COLLECTIONS_NULLARG_ERROR);
}

TEST(VECTOR_OF_INT_INIT, SUCCESS_INIT) {
    vector_of_int vector;
    EXPECT_EQ(vector_of_int_init(&vector), COLLECTIONS_SUCCESS);
    EXPECT_EQ(vector.size, 0);
    EXPECT_EQ(vector.data, nullptr);
    EXPECT_EQ(vector.capacity, 0);
    vector_of_int_deinit(&vector);
}

TEST(VECTOR_OF_INT_ADD, ADD_ONE_ITEM) {
    vector_of_int vector;
    vector_of_int_init(&vector);

    EXPECT_EQ(vector_of_int_add(&vector, 0), COLLECTIONS_SUCCESS);
    EXPECT_EQ(vector.size, 1);
    EXPECT_EQ(vector.data[0], 0);

    vector_of_int_deinit(&vector);
}

TEST(VECTOR_INT_ADD, FILL) {
    vector_of_int vector;
    vector_of_int_init(&vector);

    size_t vector_len = (sizeof(SAMPLE_INT_VECTOR) /
                         sizeof(SAMPLE_INT_VECTOR[0]));

    for (size_t idx = 0; idx < vector_len; ++idx) {
        vector_of_int_add(&vector, SAMPLE_INT_VECTOR[idx]);
    }

    EXPECT_EQ(vector.size, vector_len);
    for (size_t idx = 0; idx < vector_len; ++idx) {
        EXPECT_EQ(vector.data[idx], SAMPLE_INT_VECTOR[idx]);
    }

    vector_of_int_deinit(&vector);
}
