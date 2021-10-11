#include <gtest/gtest.h>
#include <vector>
#include <ctime>
#include <cstdlib>

extern "C" {
#include "rating.h"
#include "team.h"
#include "io.h"
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

TEST(RATING_PEAKBESTOF_TEST, ASSERT_PICK) {
    char name[] = "name";
    std::vector<team> teams;

    for (size_t i = 0; i < RATING_SIZE; ++i) {
        teams.push_back(
            {.name = name, .control_point_qty = (unsigned)rand()}
        );
    }

    struct rating* rating = rating_create();
    struct team* top_teams =
        (struct team*)malloc(PICK_SIZE * sizeof(struct team));

    for (const auto& team: teams)
        rating_add(rating, &team);

    rating_pick_best_of(rating, top_teams, PICK_SIZE);
    std::sort(
        teams.begin(), teams.end(),
        [](const struct team& a, const struct team& b) {
            return team_cmp(a, b) > 0;
        }
    );

    for (int i = 0; i < PICK_SIZE; ++i) {
        EXPECT_EQ(top_teams[i].control_point_qty, teams[i].control_point_qty);
        EXPECT_EQ(top_teams[i].route_time_secs, teams[i].route_time_secs);
        EXPECT_EQ(top_teams[i].number, teams[i].number);
        EXPECT_EQ(strcmp(top_teams[i].name, teams[i].name), 0);
    }

    free(top_teams);
    rating_destroy(&rating);
}

TEST(RATING_READ_TEST, ASSERT_FILL_FROM_FILE) {
    char name[] = "name";
    std::vector<team> teams;

    for (size_t i = 0; i < RATING_SIZE; ++i) {
        teams.push_back(
            {.name = name, .control_point_qty = (unsigned)i}
        );
    }

    FILE* stream = fopen("RATING_READ_TEST_ASSERT_FILL_FROM_FILE.txt", "w");
    for (const auto& res: teams)
        fprintf(stream, "%d\n%s\n%d\n%d\n", 
                res.number, res.name,
                res.route_time_secs, res.control_point_qty);
    fclose(stream);


    struct rating* rating = rating_create();
    struct team* top_teams =
        (struct team*)malloc(RATING_SIZE * sizeof(struct team));

    stream = fopen("RATING_READ_TEST_ASSERT_FILL_FROM_FILE.txt", "r");
    rating_read(rating, stream);
    fclose(stream);

    unsigned int rating_len;
    rating_size(rating, &rating_len);

    rating_pick_best_of(rating, top_teams, RATING_SIZE);
    std::sort(
        teams.begin(), teams.end(),
        [](const struct team& a, const struct team& b) {
            return team_cmp(a, b) > 0;
        }
    );

    for (int i = 0; i < RATING_SIZE; ++i) {
        EXPECT_EQ(top_teams[i].control_point_qty, teams[i].control_point_qty);
        EXPECT_EQ(top_teams[i].route_time_secs, teams[i].route_time_secs);
        EXPECT_EQ(top_teams[i].number, teams[i].number);
        EXPECT_EQ(strcmp(top_teams[i].name, teams[i].name), 0);
    }

    free(top_teams);
    rating_destroy(&rating);
}

int main(int argc, char** argv) {
    srand(time(NULL));
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
