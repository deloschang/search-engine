#ifndef _INDEX_H_
#define _INDEX_H_

// *****************Impementation Spec********************************
// File: index.c
// Author: Delos Chang
// This file contains useful information for implementing the indexer:
// - DEFINES
// - MACROS
// - DATA STRUCTURES
// - PROTOTYPES

// DEFINES

// Word length maximum
#define WORD_LENGTH 1000

// max url length
#define MAX_URL_LENGTH 2049

// represents the document that was found with the word contained. 
typedef struct _DocumentNode {
  struct _DocumentNode *next;        // pointer to the next member of the list.
  int document_id;                   // document identifier
  int page_word_frequency;           // number of occurrences of the word
} DocumentNode;

// fills each hash slot 
typedef struct _WordNode {
  struct _WordNode *prev;           // pointer to the previous word
  struct _WordNode *next;           // pointer to the next word
  char word[WORD_LENGTH];           // the word
  DocumentNode  *page;              // pointer to the first element of the page list.
} WordNode;


typedef struct _INVERTED_INDEX {
                                        // Start and end pointer of the dynamic links.
  WordNode *start;                      // start of the list
  WordNode *end;                        // end of the list
  WordNode *hash[MAX_NUMBER_OF_SLOTS];  // hash slot
} INVERTED_INDEX;

// initReloadStructure: This function initializes the reloaded index structure that will be used
// to "reload" the index via reading the index.dat and writing output to
// an index_new.dat. We do this to make sure that the index file can be 
// properly retrieved
INVERTED_INDEX* initReloadStructure();

// "reloads" the index data structure from the file 
// reloadIndexFromFile: This function does the heavy lifting of 
// "reloading" a file into an index in memory. It goes through
// each of the characters and uses strtok to split by space
/*int reloadIndexFromFile(char* loadFile, char* writeReload){*/
INVERTED_INDEX* reloadIndexFromFile(char* loadFile, char* writeReload, INVERTED_INDEX* indexReload);

void cleanUpIndex(INVERTED_INDEX* index);

void sanitize(char* loadedDocument);

void capitalToLower(char* buffer);

#endif
