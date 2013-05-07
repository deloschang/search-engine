#ifndef _FILE_H_
#define _FILE_H_

// *****************Impementation Spec********************************
// File: file.c
// This file contains useful information for implementing the indexer:
// - DEFINES
// - MACROS
// - DATA STRUCTURES
// - PROTOTYPES

// DEFINES



// DATA STRUCTURES. All these structures should be malloc 'd

// This is the key data structure that holds the information of each word.

  
// function PROTOTYPES used by file.c 

// dirScan: scans through the directory and returns files that
// are numbers and files only
// e.g. 1, 12, 999 (all files)
// FILTERED: abc (file), 1 (folder), 1s (file)
int dirScan(char *dir);

#endif
