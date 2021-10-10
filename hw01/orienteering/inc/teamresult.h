#ifndef HW01_ORIENTEERING_TEAMRESULT_H_
#define HW01_ORIENTEERING_TEAMRESULT_H_

struct team_result {
    unsigned int number;
    char*  name;
    unsigned int route_time_secs;
    unsigned int control_point_qty;
    
};

int team_result_cmp(
    const struct team_result* a, const struct team_result* b
);

#endif  // HW01_ORIENTEERING_TEAMRESULT_H_
