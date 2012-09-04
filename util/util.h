#ifndef UTIL_H_84A458F6_C279_4C42_9C15_5DB1871DA11B
#define UTIL_H_84A458F6_C279_4C42_9C15_5DB1871DA11B

#include <stdlib.h>

#if !(defined __LP64__ || defined __LLP64__) || defined _WIN32 && !defined _WIN64
#define SIZEOF_PTR 4
#else
#define SIZEOF_PTR 8
#endif

double
rand_double();

void*
malloc_exit_if_null(const size_t size);

void*
calloc_exit_if_null(const size_t num,
                    const size_t size);

void
free_and_null (void* p);

void
exit_if_null (const void* const p);

void
exit_if_not_zero (const int n);

int
putserr (const char* const str);

void
putserr_and_exit (const char* const str);

void
putserr_and_exitno (const int         exit_value,
                    const char* const str);

int
printferr (const char* const format,
           ...);

void
printferr_and_exit (const char* const format,
                    ...);

void
printferr_and_exitno (const int         exit_value,
                      const char* const format,
                      ...);
#endif /* UTIL_H */


