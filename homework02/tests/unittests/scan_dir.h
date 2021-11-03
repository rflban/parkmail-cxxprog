#include <gtest/gtest.h>
#include <cstdlib>
#include <fstream>

extern "C" {
#include <sys/sysinfo.h>
#include "collections/string.h"
#include "collections/vector.h"
#include "matches/scan_dir.h"
#include "matches/returncodes.h"
}

ENABLE_VECTOR_OF(match_t)

const char SEP[] = " ";
const char TOKEN[] = "I am token!";
const char NOT_EXISTING_DIR[] = "/idonotexists";
const char ROOT_DIR[] = "/root";
const char TEMP_DATA_DIR[] = "/tmp/matches_unittests_data";
const size_t QTY_OF_BESTS = 5;

static void gen_files(const char* dirpath, const char* token, size_t qty);

TEST(SCAN_DIR, NULL_ARGS) {
    EXPECT_EQ(scan_dir(NULL, NULL, NULL, NULL), MATCHES_NULLARG_ERROR);
}

TEST(SCAN_DIR, NOT_EXISTS) {
    size_t best_of = QTY_OF_BESTS;
    vector_of_match_t matches;

    vector_of_match_t_init(&matches);

    int rc = scan_dir(NOT_EXISTING_DIR, TOKEN, &matches, &best_of);
    EXPECT_EQ(rc, MATCHES_OPENDIR_ERROR);

    for (size_t idx = 0; idx < matches.size; ++idx) {
        string_deinit(&matches.data[idx].filename);
    }
    vector_of_match_t_deinit(&matches);
}

TEST(SCAN_DIR, PERMISSION_DENIED) {
    size_t best_of = QTY_OF_BESTS;
    vector_of_match_t matches;

    vector_of_match_t_init(&matches);

    int rc = scan_dir(ROOT_DIR, TOKEN, &matches, &best_of);
    EXPECT_EQ(rc, MATCHES_OPENDIR_ERROR);

    for (size_t idx = 0; idx < matches.size; ++idx) {
        string_deinit(&matches.data[idx].filename);
    }
    vector_of_match_t_deinit(&matches);
}

TEST(SCAN_DIR, EMPTY_DIR) {
    system((std::string("mkdir ") + TEMP_DATA_DIR).c_str());

    size_t best_of = QTY_OF_BESTS;
    vector_of_match_t matches;

    vector_of_match_t_init(&matches);

    int rc = scan_dir(TEMP_DATA_DIR, TOKEN, &matches, &best_of);
    EXPECT_EQ(rc, MATCHES_EMPTYDIR_ERROR);

    for (size_t idx = 0; idx < matches.size; ++idx) {
        string_deinit(&matches.data[idx].filename);
    }
    vector_of_match_t_deinit(&matches);

    system((std::string("rm -rf ") + TEMP_DATA_DIR).c_str());
}

TEST(SCAN_DIR, NOT_REGFILE_BEFORE) {
    system((std::string("mkdir ") + TEMP_DATA_DIR).c_str());
    system((std::string("mkdir ") + TEMP_DATA_DIR + "/a").c_str());
    system((std::string("touch ") + TEMP_DATA_DIR + "/b").c_str());

    size_t best_of = QTY_OF_BESTS;
    vector_of_match_t matches;

    vector_of_match_t_init(&matches);

    int rc = scan_dir(TEMP_DATA_DIR, TOKEN, &matches, &best_of);
    EXPECT_EQ(rc, MATCHES_ISNOTFILE_ERROR);

    for (size_t idx = 0; idx < matches.size; ++idx) {
        string_deinit(&matches.data[idx].filename);
    }
    vector_of_match_t_deinit(&matches);

    system((std::string("rm -rf ") + TEMP_DATA_DIR).c_str());
}

TEST(SCAN_DIR, NOT_REGFILE_AFTER) {
    system((std::string("mkdir ") + TEMP_DATA_DIR).c_str());
    system((std::string("mkdir ") + TEMP_DATA_DIR + "/b").c_str());
    system((std::string("touch ") + TEMP_DATA_DIR + "/a").c_str());

    size_t best_of = QTY_OF_BESTS;
    vector_of_match_t matches;

    vector_of_match_t_init(&matches);

    int rc = scan_dir(TEMP_DATA_DIR, TOKEN, &matches, &best_of);
    EXPECT_EQ(rc, MATCHES_ISNOTFILE_ERROR);

    for (size_t idx = 0; idx < matches.size; ++idx) {
        string_deinit(&matches.data[idx].filename);
    }
    vector_of_match_t_deinit(&matches);

    system((std::string("rm -rf ") + TEMP_DATA_DIR).c_str());
}

TEST(SCAN_DIR, FILES_L_PROCS) {
    size_t files_qty = 1;
    system((std::string("mkdir ") + TEMP_DATA_DIR).c_str());
    gen_files(TEMP_DATA_DIR, TOKEN, files_qty);

    size_t best_of = QTY_OF_BESTS;
    vector_of_match_t matches;

    vector_of_match_t_init(&matches);

    int rc = scan_dir(TEMP_DATA_DIR, TOKEN, &matches, &best_of);
    EXPECT_EQ(rc, MATCHES_SUCCESS);
    EXPECT_EQ(matches.size, std::min(files_qty, best_of));

    for (size_t idx = 0; idx < matches.size; ++idx) {
        EXPECT_EQ(matches.data[idx].count,
                  atoi(matches.data[idx].filename.data));
    }

    for (size_t idx = 0; idx < matches.size; ++idx) {
        string_deinit(&matches.data[idx].filename);
    }
    vector_of_match_t_deinit(&matches);

    system((std::string("rm -rf ") + TEMP_DATA_DIR).c_str());
}

TEST(SCAN_DIR, FILES_EQ_PROCS) {
    size_t files_qty = get_nprocs();
    system((std::string("mkdir ") + TEMP_DATA_DIR).c_str());
    gen_files(TEMP_DATA_DIR, TOKEN, files_qty);

    size_t best_of = QTY_OF_BESTS;
    vector_of_match_t matches;

    vector_of_match_t_init(&matches);

    int rc = scan_dir(TEMP_DATA_DIR, TOKEN, &matches, &best_of);
    EXPECT_EQ(rc, MATCHES_SUCCESS);
    EXPECT_EQ(matches.size, std::min(files_qty, best_of));

    for (size_t idx = 0; idx < matches.size; ++idx) {
        EXPECT_EQ(matches.data[idx].count,
                  atoi(matches.data[idx].filename.data));
    }

    for (size_t idx = 0; idx < matches.size; ++idx) {
        string_deinit(&matches.data[idx].filename);
    }
    vector_of_match_t_deinit(&matches);

    system((std::string("rm -rf ") + TEMP_DATA_DIR).c_str());
}

TEST(SCAN_DIR, FILES_G_PROCS) {
    size_t files_qty = get_nprocs() + 1;
    system((std::string("mkdir ") + TEMP_DATA_DIR).c_str());
    gen_files(TEMP_DATA_DIR, TOKEN, files_qty);

    size_t best_of = QTY_OF_BESTS;
    vector_of_match_t matches;

    vector_of_match_t_init(&matches);

    int rc = scan_dir(TEMP_DATA_DIR, TOKEN, &matches, &best_of);
    EXPECT_EQ(rc, MATCHES_SUCCESS);
    EXPECT_EQ(matches.size, std::min(files_qty, best_of));

    for (size_t idx = 0; idx < matches.size; ++idx) {
        EXPECT_EQ(matches.data[idx].count,
                  atoi(matches.data[idx].filename.data));
    }

    for (size_t idx = 0; idx < matches.size; ++idx) {
        string_deinit(&matches.data[idx].filename);
    }
    vector_of_match_t_deinit(&matches);

    system((std::string("rm -rf ") + TEMP_DATA_DIR).c_str());
}

TEST(SCAN_DIR, FILES_L_BEST_OF) {
    size_t files_qty = QTY_OF_BESTS - 1;
    system((std::string("mkdir ") + TEMP_DATA_DIR).c_str());
    gen_files(TEMP_DATA_DIR, TOKEN, files_qty);

    size_t best_of = QTY_OF_BESTS;
    vector_of_match_t matches;

    vector_of_match_t_init(&matches);

    int rc = scan_dir(TEMP_DATA_DIR, TOKEN, &matches, &best_of);
    EXPECT_EQ(rc, MATCHES_SUCCESS);
    EXPECT_EQ(matches.size, files_qty);

    for (size_t idx = 0; idx < matches.size; ++idx) {
        EXPECT_EQ(matches.data[idx].count,
                  atoi(matches.data[idx].filename.data));
    }

    for (size_t idx = 0; idx < matches.size; ++idx) {
        string_deinit(&matches.data[idx].filename);
    }
    vector_of_match_t_deinit(&matches);

    system((std::string("rm -rf ") + TEMP_DATA_DIR).c_str());
}

TEST(SCAN_DIR, FILES_E_BEST_OF) {
    size_t files_qty = QTY_OF_BESTS;
    system((std::string("mkdir ") + TEMP_DATA_DIR).c_str());
    gen_files(TEMP_DATA_DIR, TOKEN, files_qty);

    size_t best_of = QTY_OF_BESTS;
    vector_of_match_t matches;

    vector_of_match_t_init(&matches);

    int rc = scan_dir(TEMP_DATA_DIR, TOKEN, &matches, &best_of);
    EXPECT_EQ(rc, MATCHES_SUCCESS);
    EXPECT_EQ(matches.size, files_qty);
    EXPECT_EQ(matches.size, best_of);

    for (size_t idx = 0; idx < matches.size; ++idx) {
        EXPECT_EQ(matches.data[idx].count,
                  atoi(matches.data[idx].filename.data));
    }

    for (size_t idx = 0; idx < matches.size; ++idx) {
        string_deinit(&matches.data[idx].filename);
    }
    vector_of_match_t_deinit(&matches);

    system((std::string("rm -rf ") + TEMP_DATA_DIR).c_str());
}

TEST(SCAN_DIR, FILES_G_BEST_OF) {
    size_t files_qty = QTY_OF_BESTS + 1;
    system((std::string("mkdir ") + TEMP_DATA_DIR).c_str());
    gen_files(TEMP_DATA_DIR, TOKEN, files_qty);

    size_t best_of = QTY_OF_BESTS;
    vector_of_match_t matches;

    vector_of_match_t_init(&matches);

    int rc = scan_dir(TEMP_DATA_DIR, TOKEN, &matches, &best_of);
    EXPECT_EQ(rc, MATCHES_SUCCESS);
    EXPECT_EQ(matches.size, best_of);

    for (size_t idx = 0; idx < matches.size; ++idx) {
        EXPECT_EQ(matches.data[idx].count,
                  atoi(matches.data[idx].filename.data));
    }

    for (size_t idx = 0; idx < matches.size; ++idx) {
        string_deinit(&matches.data[idx].filename);
    }
    vector_of_match_t_deinit(&matches);

    system((std::string("rm -rf ") + TEMP_DATA_DIR).c_str());
}

static void gen_files(const char* dirpath, const char* token, size_t qty) {
    for (size_t idx = 0; idx < qty; ++idx) {
        std::ofstream stream(dirpath + std::string("/") +
                             std::to_string(idx + 1));

        for (size_t i = 0; i < idx + 1; ++i) {
            stream << token << SEP;
        }
    }
}
