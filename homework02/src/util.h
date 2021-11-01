#ifndef HW02_MATCHES_UTIL_H_
#define HW02_MATCHES_UTIL_H_

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

static int is_regular_file(const char *path)
{
    struct stat statbuf;
    stat(path, &statbuf);
    if (S_ISREG(statbuf.st_mode))
        return 1;
    return 0;
}

static void sort(void* arr, size_t n, size_t size,
                 int (*cmp)(const void*, const void*)) {
    assert(arr && cmp && "NULL args are not allowed");
    qsort(arr, n, size, cmp);
}

#endif  // HW02_MATCHES_UTIL_H_
