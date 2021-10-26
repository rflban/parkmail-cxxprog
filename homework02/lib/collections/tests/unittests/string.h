#include <gtest/gtest.h>
#include <cstring>

extern "C" {
#include "collections/string.h"
#include "collections/exitcodes.h"
}

#define SAMPLE_CSTR "I'm sample 'C' str"

TEST(STRING_INIT, NULL_SRC_PTR) {
    EXPECT_EQ(string_init(NULL, SAMPLE_CSTR), PHWC_EXIT_NULL_ARG);
}

TEST(STRING_INIT, NULL_DEST_PTR) {
    string_t string;
    EXPECT_EQ(string_init(&string, NULL), PHWC_EXIT_NULL_ARG);
}

TEST(STRING_INIT, SUCCESS) {
    string_t string;
    EXPECT_EQ(string_init(&string, SAMPLE_CSTR), PHWC_EXIT_SUCCESS);
    EXPECT_EQ(strcmp(string_raw(&string), SAMPLE_CSTR), 0);
    string_deinit(&string);
}
