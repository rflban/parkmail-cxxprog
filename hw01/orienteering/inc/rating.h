#ifndef HW01_ORIENTEERING_RATING_H_
#define HW01_ORIENTEERING_RATING_H_

struct rating;
struct team_result;

int rating_size(const struct rating* rating, unsigned int* size);

int rating_add(struct rating* rating, const struct team_result* result);

int rating_pick_best_of(
    const struct rating* rating,
    struct team_result*  best_results,
    unsigned int         qty
);

#endif  // HW01_ORIENTEERING_RATING_H_
