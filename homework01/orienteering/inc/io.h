#ifndef HW01_ORIENTEERING_IO_H_
#define HW01_ORIENTEERING_IO_H_

#include <stdio.h>

struct rating;

int rating_read(struct rating* rating, FILE* stream);

#endif  // HW01_ORIENTEERING_IO_H_
