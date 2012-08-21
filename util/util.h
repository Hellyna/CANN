#ifndef UTIL_H
#define UTIL_H


#if !(defined __LP64__ || defined __LLP64__) || defined _WIN32 && !defined _WIN64
#define SIZEOF_PTR 4
#else
#define SIZEOF_PTR 8
#endif

#define free_and_null(p)   do { free(p); (p) = NULL; } while(0)

double
rand_double();

void
exit_if_null (void* p);

void
exit_if_not_zero (int n);


#endif /* UTIL_H */


