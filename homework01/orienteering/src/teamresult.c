#include "teamresult.h"

int team_cmp(struct team a, struct team b) {
    int cmp = a.control_point_qty - b.control_point_qty;
    if (cmp == 0)
        cmp = b.route_time_secs - a.route_time_secs;

    return cmp;
}
