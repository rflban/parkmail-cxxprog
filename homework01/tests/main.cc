#include <gtest/gtest.h>
#include <vector>
#include <ctime>
#include <cstdlib>

extern "C" {
#include "rating.h"
#include "teamresult.h"
}

#define RATING_SIZE 10
#define PICK_SIZE 5

TEST(TEAMRESULT_CMP_TEST, ASSERT_EQ) {
    struct team a = {};
    struct team b = {};
    EXPECT_EQ(team_cmp(a, b), 0);
}

TEST(TEAMRESULT_CMP_TEST, ASSERT_GT_CONTROL_POINTS) {
    struct team a = {.control_point_qty = 1};
    struct team b = {};
    EXPECT_GT(team_cmp(a, b), 0);
}

TEST(TEAMRESULT_CMP_TEST, ASSERT_GT_TIME) {
    struct team a = {};
    struct team b = {.route_time_secs = 1};
    EXPECT_GT(team_cmp(a, b), 0);
}

TEST(TEAMRESULT_CMP_TEST, ASSERT_LT_CONTROL_POINTS) {
    struct team a = {};
    struct team b = {.control_point_qty = 1};
    EXPECT_LT(team_cmp(a, b), 0);
}

TEST(TEAMRESULT_CMP_TEST, ASSERT_LT_TIME) {
    struct team a = {.route_time_secs = 1};
    struct team b = {};
    EXPECT_LT(team_cmp(a, b), 0);
}

TEST(RATING_SIZE_TEST, ASSERT_EMPTY) {
    unsigned int size = -1;
    struct rating* rating = rating_create();

    rating_size(rating, &size);
    EXPECT_EQ(size, 0);
    rating_destroy(&rating);
}

TEST(RATING_SIZE_TEST, ASSERT_SIZE_1) {
    unsigned int size = -1;
    char name[] = "name";
    struct rating* rating = rating_create();
    struct team team = {.name = name};
    rating_add(rating, &team);

    rating_size(rating, &size);
    EXPECT_EQ(size, 1);
    rating_destroy(&rating);
}

TEST(RATING_PEAKBESTOF_TEST, ASSERT_FIRST_5_OF_10) {
    char name[] = " ";
    std::vector<team> results;

    for (int i = 0; i < RATING_SIZE; ++i) {
        name[0] = 'A' + i;
        results.push_back(
            {.name = name, .control_point_qty = (unsigned)rand()}
        );
    }

    struct rating* rating = rating_create();
    struct team* top =
        (struct team*)malloc(PICK_SIZE * sizeof(struct team));

    for (const auto& res: results)
        rating_add(rating, &res);

    rating_pick_best_of(rating, top, PICK_SIZE);
    std::sort(
        results.begin(), results.end(),
        [](const struct team& a, const struct team& b) {
            return team_cmp(a, b) > 0;
        }
    );

    for (int i = 0; i < PICK_SIZE; ++i) {
        EXPECT_EQ(top[i].control_point_qty, results[i].control_point_qty);
        EXPECT_EQ(top[i].route_time_secs, results[i].route_time_secs);
        EXPECT_EQ(top[i].number, results[i].number);
        EXPECT_EQ(strcmp(top[i].name, results[i].name), 0);
    }

    free(top);
    rating_destroy(&rating);
}

int main(int argc, char** argv) {
    srand(time(NULL));
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
