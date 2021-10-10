#include "teamresult.h"

#include <assert.h>

int team_result_cmp(
        const struct team_result* a, const struct team_result* b
    ) {
    assert(a && b);

    int cmp = a->control_point_qty - b->control_point_qty;
    if (cmp == 0)
        cmp = b->route_time_secs - a->route_time_secs;

    return cmp;
}
