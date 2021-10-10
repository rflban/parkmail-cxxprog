#include "teamresult.h"

int team_result_cmp(struct team_result a, struct team_result b) {
    int cmp = a.control_point_qty - b.control_point_qty;
    if (cmp == 0)
        cmp = b.route_time_secs - a.route_time_secs;

    return cmp;
}
