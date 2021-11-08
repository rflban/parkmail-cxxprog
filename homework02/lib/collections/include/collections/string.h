#ifndef PARKMAIL_HW_COLLECTIONS_STRING_H_
#define PARKMAIL_HW_COLLECTIONS_STRING_H_

typedef struct string {
    char* data;
} string_t;

int string_init(string_t* self, const char* str);
void string_deinit(string_t* self);

#endif  // PARKMAIL_HW_COLLECTIONS_STRING_H_
