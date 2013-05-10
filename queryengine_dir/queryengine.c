/*

FILE: queryengine.c
By: Delos Chang

Description: a command-line processing engine that asks users for input
and creates a ranking from the crawler and indexer to display to the user

INPUTS: ./queryengine [TARGET INDEXER FILENAME] [RESULTS FILE NAME]
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
#include "../utils/hash.h"

INVERTED_INDEX* indexReload = NULL;

// this function prints generic usage information 
void printUsage(){
    printf("Usage: ./queryengine ../indexer_dir/index.dat ../crawler_dir/data \n"); 
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
    fprintf(stderr, "Error: The file argument %s was not found.  Please enter a readable and valid file. \n", argv[1]);
    printUsage();

    exit(1);
  }

  // Validate that file exists
  if ( stat(argv[2], &s) != 0){
    fprintf(stderr, "Error: The file argument %s was not found.  Please enter a readable and valid file. \n", argv[2]);
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

  // VALIDATE THAT argv[2] is readable
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

  int num = 0;
  if ( keyword != NULL){
    // index for the word in the list
    queryList[num] = (char*) malloc(sizeof(char) * 1000);
    BZERO(queryList[num], 1000); // being safe

    // move keyword in 
    strcpy(queryList[num], keyword);
  } else {
    printf("No keywords were valid in your query \n");
  }

  /// LOOP THROUGH THE QUERY FOR KEYWORDS ///
  while ( (keyword = strtok(NULL, " ")) != NULL){
    num++;
    queryList[num] = (char*) malloc(sizeof(char) * 1000);
    BZERO(queryList[num], 1000); // being safe

    // move keyword in 
    strcpy(queryList[num], keyword);
  }

  free(keyword);
  return queryList;
}

// Filters the keywords with sanitize. But if a keyword is
// OR then don't sanitize, because that is distinct from 'or'
void sanitizeKeywords(char** queryList){
    // convert keywords to lowercase
    // if the keyword is OR then don't convert
    for (int i=0; queryList[i]; i++){
        if (strncmp(queryList[i], "OR", strlen("OR") + 1) ){
            capitalToLower(queryList[i]);
        }

        printf("%s \n", queryList[i]);
    }
}

// looks up the keywords
void lookUp(char** queryList, char* urlDir){
    // loop through each keyword
    // REFACTOR *** return a match if found. otherwise return NULL
    for (int i=0; queryList[i]; i++){
      // look for the keyword in the inverted index
      int wordHash = hash1(queryList[i]) % MAX_NUMBER_OF_SLOTS;

      WordNode* checkWordNode = NULL; 
      checkWordNode = indexReload->hash[wordHash];
      if (checkWordNode == NULL){
        printf("Word %s could not be found in indexer", queryList[i]);

        // next word
        continue;
      }

      // hash slot wasn't empty
      WordNode* matchedWordNode = NULL;

      // loop through each wordNode
      for (checkWordNode = indexReload->hash[wordHash];
        checkWordNode != NULL; checkWordNode = checkWordNode->next){
        if (!strncmp(checkWordNode->word, queryList[i], WORD_LENGTH)){
          // found a match that wasn't the first in the list
          matchedWordNode = checkWordNode;
          break;
        } 
      }

      // check if the match was found
      if (matchedWordNode != NULL){
        // loop through the URL docs
        // REFACTOR *****
        DocumentNode* matchedDocNode = matchedWordNode->page;
        while(matchedDocNode != NULL){
          // do some ranking algorithm
          // valid document with the word in it
          int document_id_int = matchedDocNode->document_id;

          // find the URL name with the document id
          /////////////// REFACTOR ///////////////
          // Construct the filepath
          char* readableTest;

          char* document_id;
          document_id = malloc(sizeof(char) * 1000);
          BZERO(document_id, 1000);
          sprintf(document_id, "%d", document_id_int);

          size_t string1 = strlen(urlDir); 
          size_t string2 = strlen("/");
          size_t string3 = strlen(document_id);

          // Allocate space for the filepath
          readableTest = (char*) malloc(string1 + string2 + string3 + 1);
          sprintf(readableTest, "%s/%s", urlDir, document_id);

          free(document_id);

          ///////////// REFACTOR /////////////////

          FILE* fp;
          fp = fopen(readableTest, "r");
          if (fp == NULL){
            fprintf(stderr, "Error opening the document! \n");
            free(readableTest);
            break;
          }

          char* docURL;
          docURL = (char*) malloc(sizeof(char) * MAX_URL_LENGTH);
          BZERO(docURL, MAX_URL_LENGTH);

          if (fgets(docURL, MAX_URL_LENGTH, fp) == NULL){
            fprintf(stderr, "Error copying URL from document. \n");
            free(docURL);
            free(readableTest);
            break;
          }

          printf("Document ID:%d URL:%s", matchedDocNode->document_id, docURL);
          fclose(fp);
          free(docURL);
          free(readableTest);
          
          matchedDocNode = matchedDocNode->next;
        }
      } else {
        printf("Word %s could not be found in indexer", queryList[i]);
        continue;
      }
    }
}

void cleanUpQueryList(char** queryList){
  int i = 0;
  while (queryList[i]){
    free(queryList[i]);
    i++;
  }
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
  char* urlDir = argv[2];

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

    // (3a) Check for exit parameter
    if (!strncmp(query, "!exit\n", strlen("!exit\n") + 1) ){
      break;
    }

    sanitize(query);

    LOG("Querying..\n");
    printf("You queried:  %s \n", query); 

    // (4) Cross-reference the query with the index and retrieve results
    // (4a) Convert the actual query into a list of keywords, in queryList
    char* queryList[1000];
    curateWords(queryList, query);

    // (4b) Convert keywords from uppercase to lowercase (except OR)
    sanitizeKeywords(queryList);

    // (4b) Validate the keywords
    lookUp(queryList, urlDir);
      // if one word present, then make sure it is not OR

    // Clean up the word list
    cleanUpQueryList(queryList);
  }

  // (5) Rank results via an algorithm based on word frequency with AND / OR operators

  // (7) Clean up the reloaded index
  LOG("Cleaning up");
  cleanupIndex(indexReload);

  return 0;
}
