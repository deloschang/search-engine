/*

FILE: indexer.c

Description: an indexer that creates an inverted list of words
and their occurrences amongst different documents. This inverted index
will be constructed into a file and reloaded to the query engine.

Inputs: ./indexer [TARGET DIRECTORY WHERE TO FIND THE DATA] [RESULTS FILE NAME]
 OR (for debugging / to test reloading)
  ./indexer [TARGET_DIRECTORY] [RESULTS FILENAME] [RESULTS FILENAME] [REWRITTEN FILENAME]

Outputs: For each file in the target directory, the indexer will check
all the words in each file and count their occurrences. This will
written to the result file in the format of:

  [word] [number of files containing the word] [document ID] [how many
  occurrences in document ID] etc.
  
  e.g. cat 2 3 4 7 6
  - "cat" is the word
  - number 2 is the number of documents containing the word cat
  - 3 4 mean the document with identifier 3 has 4 occurrences of cat in
    it
  - 7 6 mean the document with identifier 7 has 6 occurrences of cat in
    it

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>

#include "../utils/header.h"
#include "../utils/index.h"
#include "../utils/hash.h"
#include "../utils/file.h"
#include "indexer.h"

// create the index
INVERTED_INDEX* index = NULL;
INVERTED_INDEX* indexReload = NULL;

// this function prints generic usage information 
void printUsage(){
    printf("Normal Usage: ./indexer [TARGET DIRECTORY] [RESULTS FILENAME]\n");
    printf("Testing Usage: ./indexer [TARGET DIRECTORY] [RESULTS FILENAME] [RESULTS FILENAME] [REWRITTEN FILENAME]\n");
}

// this function will validate the arguments passed
void validateArgs(int argc, char* argv[]){
  // struct for checking whether directory exists
  struct stat s;
  struct stat checkDest;

  // test URL with default correct exit status
  int readableResult = 0;
  char* readableTest = NULL; // dynamically allocate to prevent overflow

  // check for correct number of parameters first
  if ( (argc != 3) && (argc != 5) ){
    fprintf(stderr, "Error: insufficient arguments. 3 required or 5 (testing), you provided %d \n", argc);
    printUsage();

    exit(1);
  }

  // Validate that directory exists
  if ( stat(argv[1], &s) != 0){
    fprintf(stderr, "Error: The dir argument %s was not found.  Please \
    enter a readable and valid directory. \n", argv[1]);
    printUsage();

    exit(1);
  }

  // Warn user that the destination file will be overwritten
  if ( stat(argv[2], &checkDest) == 0){
    fprintf(stderr, "Warning: The file %s already exists and will be overwritten \n", argv[2]);
  }

  // Validate that directory is readable
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
    fprintf(stderr, "Error: The dir argument %s was not readable. \
    Please enter readable and valid directory. \n", argv[1]);
    printUsage();

    free(readableTest);
    exit(1);
  }
  free(readableTest);


}

// For the debug mode, this validates the parameters
void validateDebugArgs(char* loadFile, char* writeReload){
  struct stat checkLoadFile;
  struct stat checkWriteReload;
  // Validate that loadFile exists
  if ( stat(loadFile, &checkLoadFile) != 0){
    fprintf(stderr, "Error: The target file argument %s was not found \n", loadFile);
    printUsage();
    exit(1);
  }

  // warn user that the writeReload will be overwritten
  if ( stat(writeReload, &checkWriteReload) == 0){
    fprintf(stderr, "Warning: The target file argument %s will be overwritten \n", writeReload);
  }
}

/****

*updateIndex*
------------

Updates the inverted index by hashing the word and storing it in a 
WordNode with accompanying DocumentNodes

returns 1 if successful
returns 0 if false

*/
int updateIndex(INVERTED_INDEX* index, char* word, int documentId){
  // hash first
  int wordHash = hash1(word) % MAX_NUMBER_OF_SLOTS;

  // check if hash slot occupied first?
  if (index->hash[wordHash] == NULL){
    // not occupied, fill the spot
    // create Document node first
    DocumentNode* docNode = (DocumentNode*)malloc(sizeof(DocumentNode));
    
    if (docNode == NULL){
      fprintf(stderr, "Out of memory for indexing! Aborting. \n");
      return 0;
    }

    MALLOC_CHECK(docNode);
    BZERO(docNode, sizeof(DocumentNode));
    docNode->next = NULL; // first in hash slot, no connections yet
    docNode->document_id = documentId;

    // loading one word at a time.
    // Since this word has not been seen before (nothing in hash slot), 
    // start the count at 1
    docNode->page_word_frequency = 1;

    // create Word Node of word and document node first
    WordNode* wordNode = (WordNode*)malloc(sizeof(WordNode));
    if (wordNode == NULL){
      fprintf(stderr, "Out of memory for indexing! Aborting. \n");
      return 0;
    }

    MALLOC_CHECK(wordNode);
    wordNode->prev = wordNode->next = NULL; // first in hash slot, no connections
    wordNode->page = docNode; // pointer to 1st element of page list

    BZERO(wordNode->word, WORD_LENGTH);
    strncpy(wordNode->word, word, WORD_LENGTH);

    // indexing at this slot will bring this wordNode up first
    index->hash[wordHash] = wordNode;

    // change end of inverted Index
    index->end = wordNode;

    return 1;
  } else {
    // occupied, move down the list checking for identical WordNode
    WordNode* checkWordNode = index->hash[wordHash];


    WordNode* matchedWordNode;
    WordNode* endWordNode = NULL;

    // check if the pointer is on the current word
    // we are looking for.
    // if not, run down the list and check for the word

    if (!strncmp(checkWordNode->word, word, WORD_LENGTH)){
      matchedWordNode = checkWordNode;

    } else {
      // loop until the end of the word list
      // the last WordNode 

      while (checkWordNode != NULL){
        // reached end of the WordNode list?
        if ( (checkWordNode->next == NULL) ){
          // store this last word node in the list
          endWordNode = checkWordNode;
          break;
        }

        checkWordNode = checkWordNode->next;
        if (!strncmp(checkWordNode->word, word, WORD_LENGTH)){
          // found a match that wasn't the first in the list
          matchedWordNode = checkWordNode; // store this word node
          break;
        }
      }

    }

    // either we ran to end of list and the word was not found
    // OR we have found a match
    if (matchedWordNode != NULL){
      // WordNode already exists
      // see if document Node exists

      // grab first of the document nodes
      DocumentNode* matchDocNode = matchedWordNode->page;
      DocumentNode* endDocNode;

      while (matchDocNode != NULL){
        // check if the matched Doc Node has the same document ID
        if (matchDocNode->document_id == documentId){
          // this is the correct document to increase page frequency
          matchDocNode->page_word_frequency++;
          break;
        }
        if ( (matchDocNode->next == NULL) ){

          // end of the doc node listing
          endDocNode = matchDocNode;

          // create Document node first
          DocumentNode* docNode = (DocumentNode*)malloc(sizeof(DocumentNode));

          if (docNode == NULL){
            fprintf(stderr, "Out of memory for indexing! Aborting. \n");
            return 0;
          }

          MALLOC_CHECK(docNode);
          BZERO(docNode, sizeof(DocumentNode));
          docNode->next = NULL; // first in hash slot, no connections yet
          docNode->document_id = documentId;

          // loading one word at a time.
          // Since this word has not been seen before (nothing in hash slot), 
          // start the count at 1
          docNode->page_word_frequency = 1;

          // the docNode should be last now
          endDocNode->next = docNode;
          break;
        }

        matchDocNode = matchDocNode->next;
      }
    } else {
      // WordNode doesn't exist, create new
      ////// IDIOM //////
      WordNode* wordNode = (WordNode*)malloc(sizeof(WordNode));
      if (wordNode == NULL){
        fprintf(stderr, "Out of memory for indexing! Aborting. \n");
        return 0;
      }

      // create Document node first
      DocumentNode* docNode = (DocumentNode*)malloc(sizeof(DocumentNode));

      if (docNode == NULL){
        fprintf(stderr, "Out of memory for indexing! Aborting. \n");
        return 0;
      }

      MALLOC_CHECK(docNode);
      BZERO(docNode, sizeof(DocumentNode));
      docNode->next = NULL; // first in hash slot, no connections yet
      docNode->document_id = documentId;

      // loading one word at a time.
      // Since this word has not been seen before (nothing in hash slot), 
      // start the count at 1
      docNode->page_word_frequency = 1;

      MALLOC_CHECK(wordNode);
      wordNode->prev = endWordNode;
      wordNode->next = NULL; // first in hash slot, no connections
      wordNode->page = docNode; // pointer to 1st element of page list

      BZERO(wordNode->word, WORD_LENGTH);
      strncpy(wordNode->word, word, WORD_LENGTH);

      // adjust previous 
      endWordNode->next = wordNode;

    }

    return 1;
  }



  // do indexing here
  /*printf (" reached ** %s\n", word);*/
  return 1;
}


//Grabs the next word from HTML Document and returns the position of the word.
//This is used by a bigger loop to continuously index the HTML page
// grabNextWordFromHTMLDocument: uses the loadedDocument buffer to parse for the words
// it will retrieve everything between a set of tags, including javascript. The word
// retrieved from this function is passed to updateIndex to be updated
int getNextWordFromHTMLDocument(char* loadedDocument, char* word, int position, 
INVERTED_INDEX* index, int documentId){
  int tagFlag = 0;
  int recordFlag = 1;
  char character;

  while ( loadedDocument[position] != 0) {
    character = loadedDocument[position];
    if ( character == '<' ){
      // if recordFlag is set, stop the recording
      if (recordFlag){
        recordFlag = 0;
      }

      tagFlag = 1;
      position++; // keep moving along the HTML (skipping the tag)

    } else if ( character == '>'){
      tagFlag = 0;
      position++;

      // start recording at this position; set the recordFlag
      // must be opening tag
      if (!recordFlag){
        recordFlag = 1;
      }
    } 

    // if the tagFlag is 0 and has reached this point, 
    // then we must have encountered the '>' and moved 1 position after it
    else if ( tagFlag == 0 && recordFlag){
      int validLength = 0;
      // start recording the words
      char* startRecord = &(loadedDocument[position]);

      // keep going until the closing tag is next
      while ((loadedDocument[position + 1] != '<' && (loadedDocument[position + 1] != '\0')) ){
        position++;
        validLength++;
      }

      // make sure it is at least 2 characters long
      if (validLength > 1){
        // stop recording the words 
        char* endRecord = &(loadedDocument[position + 1]);

        // take what is between endRecord and startRecord and place
        // into grabWords
        char grabWords[(endRecord - startRecord) + 1];
        BZERO(grabWords, (endRecord - startRecord) + 1);
        strncpy(grabWords, startRecord, (endRecord - startRecord));

        // grabWords now contains the segment of words between tags

        // split by space to get words
        word = strtok(grabWords, " ");
        while (word != NULL){

          // make sure the word is at least 3 characters long
          if ( strlen(word) > 2){

            // update the index with this word
            int result = updateIndex(index, word, documentId);

            // check if index update was successful
            if (result != 1){
              fprintf(stderr, "Could not successfully index %s", word);
            }
          }

          // get the next word
          word = strtok(NULL, " "); // uses null pointer
        }
        return(position + 1);
      }

      // stop recording because next characters will be a tag
      recordFlag = 0;

    } else {
      position++;
    }
  }

  return -1;

}


// Builds an index from the files in the directory
// buildIndexFromDir:  scans through the target directory. It iterates through 
// each file in the directory and uses the getNextWordFromHTMLDocument to 
// parse it and then subsequently update the index
void buildIndexFromDir(char* dir, int numOfFiles, INVERTED_INDEX* index){
  char* writable;
  char* loadedDocument;
  int currentPosition;

  // Loop through each of the files 
  for (int i = 1; i < numOfFiles + 1; i++){
    char converted_i[1001];

    // cut off if more than 1000 digits
    snprintf(converted_i, 1000, "%d", i);

    size_t string1 = strlen(dir); 
    size_t string2 = strlen("/");
    size_t string3 = strlen(converted_i);

    writable = (char*) malloc(string1 + string2 + string3 + 1);

    // Construct the filepath
    sprintf(writable, "%s/%s", dir, converted_i);

    // Load the document from the filepath
    loadedDocument = loadDocument(writable);
    sanitize(loadedDocument);
    capitalToLower(loadedDocument);

    free(writable);

    // Loop through and index the words
    int documentId = i;

    char* word = NULL;
    currentPosition = 0;

    // Updating the index is done within the getNextWordFromHTMLDocument function
    while ( (currentPosition = getNextWordFromHTMLDocument(
      loadedDocument, word, currentPosition, index, documentId)) != -1){}

    free(loadedDocument);
    printf("Indexing document %d\n", i);
  }


}

int main(int argc, char* argv[]){
  char* targetDir;
  char* targetFile;
  int numOfFiles;

  // (1) Validate the parameters
  validateArgs(argc, argv);

  // NORMAL CREATE-INDEX MODE
    // (2) Grab number of files in target dir to loop through
    targetDir = argv[1]; // set the directory
    targetFile = argv[2]; // set the new file to be written to 
    numOfFiles = dirScan(targetDir); 

    // (3) Initialize the inverted index
    index = initStructure(index);

    // (4) Loop through files to build index
    buildIndexFromDir(targetDir, numOfFiles, index);
    LOG("Index finished building");

    // (5) Save the index to a file (sorted)
    saveIndexToFile(index, targetFile);

    LOG("Writing index to file finished");

    // clean up here
    cleanUpIndex(index);

  // DEBUG MODE: reloading the index file
  if ( argc == 5){
    validateDebugArgs(argv[3], argv[4]);
    LOG("Testing index");

    char* loadFile = argv[3];
    char* writeReload = argv[4];

    // Reload the index from the saved file (e.g. from index.dat)
    indexReload = initStructure(indexReload);

    INVERTED_INDEX* reloadResult = reloadIndexFromFile(loadFile, indexReload);
    if (reloadResult == NULL){
      exit(1);
    } else {
      LOG("Finished reloading index from file");
    }

    // Write the index in memory to file
    saveIndexToFile(reloadResult, writeReload);

    // clean up here;
    LOG("Cleaning up");
    cleanUpIndex(indexReload);

    // BATS.sh will test the integrity of the reloaded index.
  }
  return 0;
}
