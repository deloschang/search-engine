/*

FILE: queryengine.c
By: Delos Chang

Description: a command-line processing engine that asks users for input
and creates a ranking from the crawler and indexer to display to the user

Inputs: ./queryengine [TARGET INDEXER FILENAME] [RESULTS FILE NAME]
  AND / OR operators for command-line processing
  - a space (" ") represents an 'AND' operator
  - a capital OR represents an 'OR' operator

  TO EXIT: '!exit'

Outputs: The query engine will output a ranking based on the queries that the 
user enters

Design Spec: 

Implementation Spec Pseudocode: 
  1. Validate user input arguments
  2. Load the indexer's index file into memory
  3. Query the user via the command line
  4. Change the capital letters to lower case letters
  5. Cross-reference the query with the index and retrieve results
  6. Rank results via an algorithm based on word frequency with AND / OR operators
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>

#include "../utils/index.h"

INVERTED_INDEX* indexReload = NULL;

// this function prints generic usage information 
void printUsage(){
    printf("Usage: ./queryengine ../indexer_dir/index.dat ../data \n"); 
}

void validateArgs(int argc, char* argv[]){
  // struct for checking whether directory exists
  struct stat s;
  struct stat checkDest;

  // test URL with default correct exit status
  int readableResult = 0;
  char* readableTest = NULL; // dynamically allocate to prevent overflow

  // check for correct number of parameters first
  if ( (argc != 3) ){
    fprintf(stderr, "Error: insufficient arguments. 3 required. You provided %d \n", argc);
    printUsage();

    exit(1);
  }

  // Validate that file exists
  if ( stat(argv[1], &s) != 0){
    fprintf(stderr, "Error: The file argument %s was not found.  Please \
    enter a readable and valid file. \n", argv[1]);
    printUsage();

    exit(1);
  }

  // Warn user that the destination file will be overwritten
  if ( stat(argv[2], &checkDest) == 0){
    fprintf(stderr, "Warning: The file %s already exists and will be overwritten \n", argv[2]);
  }

  // Validate that file is readable
  // Allocate memory to prevent overflow
  size_t len1 = strlen("if [[ -r "), len2 = strlen(argv[1]), len3 = strlen(" ]]; then exit 0; else exit 1; fi");
  readableTest = (char*) malloc(len1 + len2 + len3 + 1);

  if (readableTest == NULL){
    fprintf(stderr, "Out of memory! \n ");
    exit(1);
  }

  memcpy(readableTest, "if [[ -r ", len1);
  memcpy(readableTest+len1, argv[1], len2);
  strcat(readableTest, " ]]; then exit 0; else exit 1; fi");

  readableResult = system(readableTest);

  // check the exit status
  if ( readableResult != 0){
    fprintf(stderr, "Error: The file argument %s was not readable. \
    Please enter readable and valid file. \n", argv[1]);
    printUsage();

    free(readableTest);
    exit(1);
  }
  free(readableTest);
}

// converts the raw query from the command line processing into
// a list of words to be cross-referenced with the index
char** curateWords(char** queryList, char* query){
  char* keyword;
  char* queryCopy;

  // create storage for keyword
  keyword = (char*) malloc(sizeof(char) * 1000);
  BZERO(keyword, 1000);

  // create storage for the query command
  queryCopy = (char*) malloc(sizeof(char) * 1000);
  BZERO(queryCopy, 1000);


  /// BEGIN PARSING THE QUERY ///
  strcpy(queryCopy, query);
  keyword = strtok(queryCopy, " ");

  // index for the word in the list
  int num = 0;
  queryList[num] = (char*) malloc(sizeof(char) * 1000);
  BZERO(queryList[num], 1000); // being safe

  // move keyword in 
  strcpy(queryList[num], keyword);

  /// LOOP THROUGH THE QUERY FOR KEYWORDS ///
  while ( (keyword = strtok(NULL, " ")) != NULL){
    num++;
    queryList[num] = (char*) malloc(sizeof(char) * 1000);
    BZERO(queryList[num], 1000); // being safe

    // move keyword in 
    strcpy(queryList[num], keyword);
  }

  return queryList;
}

int main(int argc, char* argv[]){
  // (1) Validate the parameters
  validateArgs(argc, argv);

  // (2) Initialize the inverted index
  if ( (indexReload = initReloadStructure()) == NULL){
    fprintf(stderr, "Could not initialize data structures. Exiting");
    exit(1);
  }

  // (2b) Load the index into memory
  char* loadFile = argv[1];
  char* writeReload = "index_new.dat";

  INVERTED_INDEX* reloadResult = reloadIndexFromFile(loadFile, writeReload, indexReload);
  if (reloadResult == NULL){
    fprintf(stderr, "Could not reload the index from the file! \n");
    exit(1);
  } else {
    LOG("Finished reloading index from file");
  }

  // (3) Query the user via the command line
  while (1) {
    char query[1000];
    printf(" \n KEY WORD:> ");
    fgets(query, 999, stdin);

    // (3b) Change capital letters to lower case letters
    sanitize(query);

    // (3c) Check for exit parameter
    if (!strncmp(query, "!exit", strlen("!exit") + 1) ){
      break;
    }

    LOG("Querying..\n");
    printf("You queried:  %s \n", query); 

    // (4) Cross-reference the query with the index and retrieve results
    char* queryList[1000];
    curateWords(queryList, query);
  }

  // (5) Rank results via an algorithm based on word frequency with AND / OR operators

  // (7) Clean up the reloaded index
  LOG("Cleaning up");
  cleanupIndex(indexReload);

  return 0;
}
