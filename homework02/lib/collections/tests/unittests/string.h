#include <gtest/gtest.h>
#include <cstring>

extern "C" {
#include "collections/string.h"
#include "collections/returncodes.h"
}

#define SAMPLE_CSTR "I'm sample 'C' str"

TEST(STRING_INIT, NULL_SRC_PTR) {
    EXPECT_EQ(string_init(NULL, SAMPLE_CSTR), COLLECTIONS_NULLARG_ERROR);
}

TEST(STRING_INIT, NULL_DEST_PTR) {
    string_t string;
    EXPECT_EQ(string_init(&string, NULL), COLLECTIONS_NULLARG_ERROR);
}

TEST(STRING_INIT, SUCCESS) {
    string_t string;
    EXPECT_EQ(string_init(&string, SAMPLE_CSTR), COLLECTIONS_SUCCESS);
    EXPECT_EQ(strcmp(string.data, SAMPLE_CSTR), 0);
    string_deinit(&string);
}
