// File: hash.c
// Description: a simple hash function.
// 
// This is a utility for the CS50 search engine project.
// It is an implementation of the hash.
// Originally from http://www.cse.yorku.ca/~oz/hash.html
// Hash functions are not randomly designed, there are theories behind.
// Read the above web page for details.

#include <string.h>
#include <stdlib.h>

unsigned long hash1(char* str) {
     unsigned long hash = 5381;
     int c;

     while ((c = *str++) != 0)
          hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
     return hash;
}

