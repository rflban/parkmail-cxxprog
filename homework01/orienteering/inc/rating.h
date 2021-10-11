#ifndef HW01_ORIENTEERING_RATING_H_
#define HW01_ORIENTEERING_RATING_H_

struct rating;
struct team;

struct rating* rating_create();
void rating_destroy(struct rating** rating);

int rating_size(const struct rating* rating, unsigned int* size);

int rating_add(struct rating* rating, const struct team* result);

int rating_pick_best_of(const struct rating* rating, struct team* best_results,
                        unsigned int qty);

#endif  // HW01_ORIENTEERING_RATING_H_
