/*!
  \file util/util.h
  \brief Common utility functions for the C Programming Language
  \author Hellyna Ng (hellyna@hellyna.com)
  */
#ifndef UTIL_H_84A458F6_C279_4C42_9C15_5DB1871DA11B
#define UTIL_H_84A458F6_C279_4C42_9C15_5DB1871DA11B

#include <stddef.h>

#ifdef DOXYGEN
  /*!
    Automatically defines the size of pointer in 32/64bit machines.
    */
  #define SIZEOF_PTR
#else
  #if !(defined __LP64__ || defined __LLP64__) || defined _WIN32 && !defined _WIN64
    #define SIZEOF_PTR 4
  #else
    #define SIZEOF_PTR 8
  #endif
#endif

/*!
  Generates a random double between 0 and 1
  */
double
rand_double();

/*!
  Allocates memory.
  \param size the size to be allocated in bytes.
  \return a pointer to the allocated memory, or never returns, but exit with \b EXIT_FAILURE if the allocation failed.
  */
void*
malloc_exit_if_null(const size_t size);

/*!
  Allocates and initializes memory to 0.
  \param num the number of elements to be allocated.
  \param size the size of each element.
  \return a pointer to the allocated memory, or never returns, but exit with \b EXIT_FAILURE if the allocation failed.
  */
void*
calloc_exit_if_null(const size_t num,
                    const size_t size);

/*!
  Free memory pointed by \b p, and set it to \b NULL.
  \param p the pointer to be freed.
  */
void
free_and_null (void* p);

/*!
  Exit with \b EXIT_FAILURE if memory pointed by \b p is \b NULL.
  \param p the pointer to test for.
  */
void
exit_if_null (const void* const p);

/*!
  Exit with \b EXIT_FAILURE if \b n is not zero.
  \param n the number to test for zero.
  */
void
exit_if_not_zero (const int n);

/*!
  Prints to \b stderr.
  \param str the string to print.
  \return positive integer on success, \b EOF on failure.
  */
int
putserr (const char* const str);

/*!
  Prints to \b stderr and exit with \b EXIT_FAILURE.
  \param str the string to print.
  */
void
putserr_and_exit (const char* const str);

/*!
  Prints to \b stderr and exit with \b exit_value.
  \param exit_value the exit_value to exit with.
  \param str the string to print.
  */
void
putserr_and_exitno (const int         exit_value,
                    const char* const str);

/*!
  Prints to \b stderr.
  \param format a \b printf() style format.
  \param ... arguments specified in format.
  \return positive integer on success, \b EOF on failure.
  */
int
printferr (const char* const format,
           ...);

/*!
  Prints to \b stderr and exit with \b EXIT_FAILURE.
  \param format a \b printf() style format.
  \param ... arguments specified in format.
*/
void
printferr_and_exit (const char* const format,
                    ...);

/*!
  Prints to \b stderr and exit with \b exit_value.
  \param format a \b printf() style format.
  \param ... arguments specified in format.
  */
void
printferr_and_exitno (const int         exit_value,
                      const char* const format,
                      ...);
#endif /* UTIL_H */


