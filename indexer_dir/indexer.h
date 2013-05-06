#ifndef _INDEXER_H_
#define _INDEXER_H_

// *****************Impementation Spec********************************
// File: indexer.c
// This file contains useful information for implementing the indexer:
// - DEFINES
// - MACROS
// - DATA STRUCTURES
// - PROTOTYPES

// DEFINES




// Make the hash large 
// This will minimize collisions 
#define MAX_NUMBER_OF_SLOTS 10000

// Word length maximum
#define WORD_LENGTH 1000

//! Check whether arg is NULL or not on a memory allocation. Quit this program if it is NULL.
#define MALLOC_CHECK(s)  if ((s) == NULL)   {                     \
    printf("No enough memory at %s:line%d ", __FILE__, __LINE__); \
    perror(":");                                                  \
    exit(-1); \
  }

//! Set memory space starts at pointer \a n of size \a m to zero. 
#define BZERO(n,m)  memset(n, 0, m)

// LOG
#define LOG(s)  printf("[%s:%d]%s\n", __FILE__, __LINE__, s)



// DATA STRUCTURES. All these structures should be malloc 'd

// This is the key data structure that holds the information of each word.


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
  
// function PROTOTYPES used by crawler.c You have to code them.

// getPage: Assumption: if you are dowloading the file it must be unique. 
// Two cases. First its the SEED URL so its unique. Second, wget only gets a page 
// once a URL is determined to be unique. Get the HTML file saved in TEMP 
// and read it into a string that is returned by getPage. Store TEMP
// to a file 1..N after writing the URL and depth on the first and second 
// lines respectively.

char *getPage(char* url, int depth,  char* path);

// extractURL: Given a string of the HTML page, parse it (you have the code 
// for this GetNextURL) and store all the URLs in the url_list (defined above).
// NULL pointer represents end of list (no more URLs). Return the url_list

char **extractURLs(char* html_buffer, char* current);

// setURLasVisited: Mark the URL as visited in the URLNODE structure.

void setURLasVisited(char* url);

// updateListLinkToBeVisited: Heavy lifting function. Could be made smaller. It takes
// the url_list and for each URL in the list it first determines if it is unique.
// If it is then it creates a DNODE (using malloc) and URLNODE (using malloc).
// It copies the URL to the URLNODE and initialises it and then links it into the
// DNODE (and initialise it). Then it links the DNODE into the linked list dict.
// at the point in the list where its key cluster is (assuming that there are
// elements hashed at the same slot and the URL was found to be unique. It does
// this for *every* URL in the url_list

void updateListLinkToBeVisited(char *url_list[ ], int depth);

// getAddressFromTheLinksToBeVisited: Scan down thie hash table (part of dict) and
// find the first URL that has not already been visited and return the pointer 
// to that URL. Note, that the pointer to the depth is also passed. Update the
// depth using the depth of the URLNODE that is next to be visited. 

char *getAddressFromTheLinksToBeVisited(int *depth);

#endif
