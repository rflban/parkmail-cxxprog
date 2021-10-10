#include <gtest/gtest.h>
#include <vector>
#include <ctime>
#include <cstdlib>

extern "C" {
#include "rating.h"
#include "teamresult.h"
}

TEST(TEAMRESULT_CMP_TEST, Assert_EQ) {
    struct team_result a = {};
    struct team_result b = {};
    EXPECT_EQ(team_result_cmp(&a, &b), 0);
}

TEST(TEAMRESULT_CMP_TEST, Assert_GT_control_points) {
    struct team_result a = {.control_point_qty = 1};
    struct team_result b = {};
    EXPECT_GT(team_result_cmp(&a, &b), 0);
}

TEST(TEAMRESULT_CMP_TEST, Assert_GT_time) {
    struct team_result a = {};
    struct team_result b = {.route_time_secs = 1};
    EXPECT_GT(team_result_cmp(&a, &b), 0);
}

TEST(TEAMRESULT_CMP_TEST, Assert_LT_control_points) {
    struct team_result a = {};
    struct team_result b = {.control_point_qty = 1};
    EXPECT_LT(team_result_cmp(&a, &b), 0);
}

TEST(TEAMRESULT_CMP_TEST, Assert_LT_time) {
    struct team_result a = {.route_time_secs = 1};
    struct team_result b = {};
    EXPECT_LT(team_result_cmp(&a, &b), 0);
}

TEST(RATING_SIZE_TEST, Assert_Empty) {
    unsigned int size = -1;
    struct rating* rating = rating_create();

    rating_size(rating, &size);
    EXPECT_EQ(size, 0);
    rating_destroy(&rating);
}

TEST(RATING_SIZE_TEST, Assert_Size_1) {
    unsigned int size = -1;
    char name[] = "name";
    struct rating* rating = rating_create();
    struct team_result team_result = {.name = name};
    rating_add(rating, &team_result);

    rating_size(rating, &size);
    EXPECT_EQ(size, 1);
    rating_destroy(&rating);
}

TEST(RATING_PEAKBESTOF_TEST, Assert_first_5_of_10) {
    int top_size = 5;
    int total_size = 10;
    char name[] = " ";
    std::vector<team_result> results;

    for (int i = 0; i < total_size; ++i) {
        name[0] = 'A' + i;
        results.push_back(
            {.name = name, .control_point_qty = (unsigned)rand()}
        );
    }

    struct rating* rating = rating_create();
    struct team_result* top =
        (struct team_result*)malloc(top_size * sizeof(struct team_result));

    for (const auto& res: results)
        rating_add(rating, &res);

    rating_pick_best_of(rating, top, top_size);
    std::sort(
        results.begin(), results.end(),
        [](const struct team_result& a, const struct team_result& b) {
            return team_result_cmp(&a, &b) > 0;
        }
    );

    for (int i = 0; i < top_size; ++i) {
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
