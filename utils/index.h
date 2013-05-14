#ifndef _INDEX_H_
#define _INDEX_H_

// *****************Impementation Spec********************************
// File: index.c
// Author: Delos Chang
// This file contains useful information for implementing the indexer:
// - DATA STRUCTURES
// - PROTOTYPES

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

// function PROTOTYPES 

// initReloadStructure: This function initializes the reloaded index structure that will be used
// to "reload" the index via reading the index.dat and writing output to
// an index_new.dat. We do this to make sure that the index file can be 
// properly retrieved
INVERTED_INDEX* initStructure(INVERTED_INDEX* index);

// "reloads" the index data structure from the file 
// reloadIndexFromFile: This function does the heavy lifting of 
// "reloading" a file into an index in memory. It goes through
// each of the characters and uses strtok to split by space
INVERTED_INDEX* reloadIndexFromFile(char* loadFile, INVERTED_INDEX* indexReload);


DocumentNode* newDocNode(DocumentNode* docNode, int docId, int page_freq);

WordNode* newWordNode(WordNode* wordNode, DocumentNode* docNode, char* word);

char* loadDocument(char* filepath);

// saves the inverted index into a file
// returns 1 if successful, 0 if not
// saveIndexToFile: this function will save the index in memory to a file
// by going through each WordNode and Document Node, parsing them 
// and writing them in the specified format
// cat 2 2 3 4 5 
// the first 2 indicates that there are 2 documents with 'cat' found
// the second 2 indicates the document ID with 3 occurrences of 'cat'
// the 4 indicates the document ID with 5 occurrences of 'cat'
void saveIndexToFile(INVERTED_INDEX* index, char* targetFile);

void cleanUpIndex(INVERTED_INDEX* index);

void sanitize(char* loadedDocument);

void capitalToLower(char* buffer);

#endif
