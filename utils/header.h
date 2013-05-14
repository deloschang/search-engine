#ifndef _HEADER_H_
#define _HEADER_H_
// Filename: header.h
// Description: General macros and definitions for the search macros and
//

// DEFINES

// Word length maximum
#define WORD_LENGTH 1000

// The max length of each URL path.
// Since there is no defined limit by IETF or W3C, we'll go with the 
// effective limit imposed by popular browsers, plus a little:
//    2048 characters + a NULL
#define MAX_URL_LENGTH 2049

// Make the hash large in comparison to the number of URLs found
// for example depth of 2 on www.cs.dartmouth.edu approx 200.
// This will minimize collisions and mostly likely slots will be
// empty or have only one or two DNODES. Access is O(1). Fast.
#define MAX_NUMBER_OF_SLOTS 10000


#define min(x,y)   ((x)>(y))?(y):(x)

//! Print arg together with the source file name and the current line number.
#define LOG(s)  printf("[%s:%d]%s\n", __FILE__, __LINE__, s)

//! Check whether arg is NULL or not. Quit this program if it is NULL.
#define MYASSERT(s)  if (!(s))   {                                      \
    printf("General Assert Error at %s:line%d\n", __FILE__, __LINE__);  \
    exit(-1); \
  }

//! Check whether arg is NULL or not on a memory allocation. Quit this program if it is NULL.
#define MALLOC_CHECK(s)  if ((s) == NULL)   {                     \
    printf("No enough memory at %s:line%d ", __FILE__, __LINE__); \
    perror(":");                                                  \
    exit(-1); \
  }

//! Set memory space starts at pointer \a n of size \a m to zero. 
#define BZERO(n,m)  memset(n, 0, m)

#endif
