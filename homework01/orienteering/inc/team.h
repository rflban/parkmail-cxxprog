#ifndef HW01_ORIENTEERING_TEAMRESULT_H_
#define HW01_ORIENTEERING_TEAMRESULT_H_

struct team {
    unsigned int number;
    char* name;
    unsigned int route_time_secs;
    unsigned int control_point_qty;
};

int team_cmp(struct team a, struct team b);

#endif  // HW01_ORIENTEERING_TEAMRESULT_H_
