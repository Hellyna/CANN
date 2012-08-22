#ifndef UTIL_H
#define UTIL_H


#if !(defined __LP64__ || defined __LLP64__) || defined _WIN32 && !defined _WIN64
#define SIZEOF_PTR 4
#else
#define SIZEOF_PTR 8
#endif

#define free_and_null(p)   do { free(p); (p) = NULL; } while(0)

inline double
rand_double();

inline void
exit_if_null (void* p);

inline void
exit_if_not_zero (int n);

inline int
printferr (const char *format, ...);

inline void
printferr_and_exit (const char *format, ...);

inline void
printferr_and_exitno (int exit_value, const char *format, ...);
#endif /* UTIL_H */


