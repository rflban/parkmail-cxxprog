#include <gtest/gtest.h>
#include <stdlib.h>

extern "C" {
#include "matches/scan_dir.h"
#include "matches/returncodes.h"
}

TEST(SCAN_DIR, NULL_ARGS) {
    EXPECT_EQ(scan_dir(NULL, NULL, NULL, NULL), MATCHES_NULLARG_ERROR);
}

TEST(SCAN_DIR, NOT_EXISTS) {
}

TEST(SCAN_DIR, NO_PERMISSION) {
}

TEST(SCAN_DIR, EMPTY_DIR) {
}

TEST(SCAN_DIR, NOT_REGFILE_BEFORE) {
}

TEST(SCAN_DIR, NOT_REGFILE_AFTER) {
}

TEST(SCAN_DIR, FILES_L_PROCS) {
}

TEST(SCAN_DIR, FILES_EQ_PROCS) {
}

TEST(SCAN_DIR, FILES_G_PROCS) {
}
