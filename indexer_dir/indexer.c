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

#include "indexer.h"
#include "hash.h"

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

  free(readableTest);

  // check the exit status
  if ( readableResult != 0){
    fprintf(stderr, "Error: The dir argument %s was not readable. \
    Please enter readable and valid directory. \n", argv[1]);
    printUsage();

    exit(1);
  }
}

char* loadDocument(char* filepath){
  FILE* fp;
  char* html = NULL;

  fp = fopen(filepath, "r");

  // If unable to find file, skip.
  // Could be problem of skipped files i.e. (1, 3, 4, 5)
  if (fp == NULL){
    fprintf(stderr, "Could not read file %s. Aborting. \n", filepath);
    exit(1);
  }

  // Read from the file and put it in a buffer
  if (fseek(fp, 0L, SEEK_END) == 0){
    long tempFileSize = ftell(fp);
    if (tempFileSize == -1){
      fprintf(stderr, "Error: file size not valid \n");
      exit(1);
    }

    // Allocate buffer to that size
    // Length of buffer should be the same of output of wget + 1
    html = malloc(sizeof(char) * (tempFileSize + 2) ); // +1 for terminal byte

    // Rewind to top in preparation for reading
    rewind(fp);

    // Read file to buffer
    size_t readResult = fread(html, sizeof(char), tempFileSize, fp);

    if (readResult == 0){
      fprintf(stderr, "Error reading the file into buffer. Aborting. \n");
      exit(1);
    } else {
      readResult++;
      html[readResult] = '\0'; // add terminal byte
    }
  }
  
  fclose(fp);
  return html;
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
        // check if the matched Doc Node has the same document ID
        if (matchDocNode->document_id == documentId){
          // this is the correct document to increase page frequency
          matchDocNode->page_word_frequency++;
          break;
        }
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

/****

*getNextWordFromHTMLDocument*
------------

Grabs the next word from HTML Document and returns the position of the word.
This is used by a bigger loop to continuously index the HTML page

*/
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


// Strips the buffer of non-letters
void sanitize(char* loadedDocument){
  char* temp;
  char* clear;

  // set aside buffer for the document
  temp = malloc(sizeof(char) * strlen(loadedDocument) + 1); 
  BZERO(temp, (strlen(loadedDocument) + 1)); // to be safe

  // Initialize a clear that is 1 character long.
  // We will put all valid letters into this and then transfer it
  // into the temp buffer
  clear = malloc(1);
  BZERO(clear, 1);

  // loop through document until end
  for (int i = 0; loadedDocument[i]; i++){

    // only copy into buffer if it is valid by the following parameters

    // any letter or space 
    // any number

    // 32 is space
    // 39 is an apostrophe, sanitize these
    if (loadedDocument[i] > 13){
    /*if (loadedDocument[i] > 31){*/
    /*if (loadedDocument[i] > 44 || loadedDocument[i] == 32 || loadedDocument[i] == '&'){*/

      // filter apostrophe and periods and commas and quotes
      if (loadedDocument[i] == 39 || loadedDocument[i] == ',' 
        || loadedDocument[i] == '.'
        || loadedDocument[i] == '"'){
        continue;
      }
      
      // filter out '!' '#' '$' etc
      if ((loadedDocument[i] >= 33 && loadedDocument[i] <=44) && 
        (loadedDocument[i] != '&')){
        continue;
      }

      // filter characters like ':' ';' '@' '?' etc.
      // make sure '<' and '>' pass through 
      if (loadedDocument[i] >= 59 && loadedDocument[i] <= 64
        && loadedDocument[i] != 60 && loadedDocument[i] != 62){
        continue;
      }
      
      // filter characters like '[' '\' '^' '_'
      if (loadedDocument[i] >= 91 && loadedDocument[i] <= 96){
        continue;
      }

      // filter characters like '{' '|' '~'
      if (loadedDocument[i] >= 123 && loadedDocument[i] <= 127){
        continue;
      }

      // if letters, convert them from upper case to lower case
      if (loadedDocument[i] >= 'A' && loadedDocument[i] <= 'Z'){
        loadedDocument[i] = 'a' + loadedDocument[i] - 'A';
      } 

      // put that character into the clearzone
      sprintf(clear, "%c", loadedDocument[i]);

      // append it to temp
      strcat(temp, clear);
    }
  }

  // replace original with sanitized version
  strcpy(loadedDocument, temp);
  
  // clean up
  free(temp);
  free(clear);
}

// Builds an index from the files in the directory
void buildIndexFromDir(char* dir, int numOfFiles, INVERTED_INDEX* index){
  char* writable;
  char* loadedDocument;
  int currentPosition;

  // Loop through each of the files 
  for (int i = 1; i < numOfFiles; i++){
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

    free(writable);

    // Loop through and index the words

    // Got the HTML page
    /*printf("%s\n", loadedDocument);*/

    /*documentId = getDocumentId(d);*/
    int documentId = i;

    char* word = NULL;
    currentPosition = 0;

    // Updating the index is done within the getNextWordFromHTMLDocument function
    while ( (currentPosition = getNextWordFromHTMLDocument(
      loadedDocument, word, currentPosition, index, documentId)) != -1){
      }

    free(loadedDocument);
    printf("Indexing document %d\n", i);
  }


}


int initStructures(){
  // create the index structure
  index = (INVERTED_INDEX*)malloc(sizeof(INVERTED_INDEX));
  MALLOC_CHECK(index);
  index->start = index->end = NULL;
  BZERO(index, sizeof(INVERTED_INDEX));

  if (index == NULL){
    return 0;
  }

  return 1;
}

int initReloadStructure(){
  // create the index structure
  indexReload = (INVERTED_INDEX*)malloc(sizeof(INVERTED_INDEX));
  MALLOC_CHECK(indexReload);
  indexReload->start = indexReload->end = NULL;
  BZERO(indexReload, sizeof(INVERTED_INDEX));

  if (indexReload == NULL){
    return 0;
  }

  return 1;
}

// saves the inverted index into a file
// returns 1 if successful, 0 if not
int saveIndexToFile(INVERTED_INDEX* index, char* targetFile){
  WordNode* startWordNode;
  DocumentNode* startPage;
  FILE* fp;

  int count;

  // open the targeted file
  fp = fopen(targetFile, "w");

  if (fp == NULL){
    fprintf(stderr, "Error writing to the file %s", targetFile);
  }

  // loop through every possible hash slot 
  for(int i=0; i < MAX_NUMBER_OF_SLOTS; i++){
    // loop through every word in the list
    for ( startWordNode = index->hash[i]; startWordNode != NULL; startWordNode=startWordNode->next){

      count = 0;
      // count the number of documents
      for (startPage = (DocumentNode*) startWordNode->page; 
        startPage != NULL; startPage = startPage->next){

        count++;
      }

      /*fprintf(fp, "%s %d ", (char*) startWordNode->word, count);*/
      fprintf(fp, "%s %d ", startWordNode->word, count);

      // rest are docID and number of occurrences
      // loop again
      for (startPage = startWordNode->page; startPage != NULL; startPage=startPage->next){
        // keep adding the doc identifier and the page freq until no more
        fprintf(fp, "%d %d ", startPage->document_id, startPage->page_word_frequency);
      }
      fprintf(fp, "\n");
    }
  }


  // writing is done; free resources
  fclose(fp);

  // sort the outputted list with a command

  size_t string1 = strlen("sort -o ");
  size_t string2 = strlen(targetFile);
  char* sortCommand = (char*) malloc(string1 + string2 + string2 + 1);

  sprintf(sortCommand, "%s%s %s", "sort -o ", targetFile, targetFile); 
  system(sortCommand);

  free(sortCommand);

  return 1;
}

void cleanupIndex(INVERTED_INDEX* index){
  WordNode* startWordNode;
  WordNode* toWordFreedom;
  DocumentNode* startPage;
  DocumentNode* toFreedom;


  // loop through each hash slot
  for(int i=0; i < MAX_NUMBER_OF_SLOTS; i++){

    startWordNode = index->hash[i];
    // loop through every word in the list
    while (startWordNode != NULL){

      toWordFreedom = startWordNode;

      // for each WordNode, loop through each of the DocNodes
      while (startPage != NULL){
        toFreedom = startPage;
        startPage = startPage->next;

        // release the DocNode to FREEDOM!!
        if (toFreedom != NULL){
          free(toFreedom);
        }
      }

      // continue
      startWordNode = startWordNode->next;
      
      // we have no use for the WordNode anymore
      if (toWordFreedom != NULL){
        free(toWordFreedom);
      }


    }

  }

  free(index);

}

int reconstructIndex(char* wordNode, int docNode, int page_word_frequency){

}

// "reloads" the index data structure from the file 
int reloadIndexFromFile(char* loadFile, char* writeReload){
  struct stat checkLoadFile;
  struct stat checkWriteReload;
  FILE* fp;

  // Validate that loadFile exists
  if ( stat(loadFile, &checkLoadFile) != 0){
    fprintf(stderr, "Error: The target file argument %s was not found \n", loadFile);
    printUsage();
    return 0;
  }

  // warn user that the writeReload will be overwritten
  if ( stat(writeReload, &checkWriteReload) == 0){
    fprintf(stderr, "Warning: The target file argument %s will be overwritten \n", writeReload);
  }

  fp = fopen(loadFile, "r");
  if (fp == NULL){
    fprintf(stderr, "Error opening the file to be reloaded: %s \n", loadFile);
    return 0;
  }

  // Commence reloading the index from the file
  // indexReload has been initialized already
  char* placeholder;
  char* loadedFile = loadDocument(loadFile);

  size_t string1 = strlen(loadedFile);

  placeholder = (char*) malloc(sizeof(char) * string1 + 1);
  BZERO(placeholder, string1 + 1);

  while(1){
    // continue until end
    if (fgets(placeholder, string1 + 1, fp) == NULL){
      break;
    } else {
      char buffer[string1 + 1];
      strcpy(buffer, placeholder);

      // takes the first word from the line
      char* docNode;
      char* wordNode = strtok(buffer, " ");
      char* numberOfDocs = strtok(NULL, " ");

      while ((docNode = strtok(NULL, " ")) != NULL){
        char* page_word_frequency = strtok(NULL, " ");
        // insert into index here
        reconstructIndex(wordNode, atoi(docNode), atoi(page_word_frequency));
      }
    }

  }

  
  // every line represents a word node with all its Document Nodes


  // Write the index to the file
  /*saveIndexFromFile(indexReload, writeReload);*/

  /*cleanupIndex(indexReload);*/

  fclose(fp);
  return 1;
}

int main(int argc, char* argv[]){
  char* targetDir;
  char* targetFile;
  int numOfFiles;

  // (1) Validate the parameters
  validateArgs(argc, argv);

  // NORMAL CREATE-INDEX MODE
  if ( argc == 3){
    // (2) Grab number of files in target dir to loop through
    targetDir = argv[1]; // set the directory
    targetFile = argv[2]; // set the new file to be written to 
    numOfFiles = dirScan(targetDir); 

    // (3) Initialize the inverted index
    if (initStructures() == 0){
      fprintf(stderr, "Could not initialize data structures. Exiting");
      exit(1);
    }

    // (4) Loop through files to build index
    buildIndexFromDir(targetDir, numOfFiles, index);
    LOG("Index finished building");

    // (5) Save the index to a file (sorted)
    int saveResult = saveIndexToFile(index, targetFile);
    if (saveResult != 1){
      fprintf(stderr, "Could not save index to file! \n");
      exit(1);
    } else {
      LOG("Writing index to file finished");
    }

    // clean up here
    cleanupIndex(index);
  }

  // DEBUG MODE: reloading the index file
  if ( argc == 5){
    char* loadFile = argv[3];
    char* writeReload = argv[4];

    // will reload the index from the saved file (e.g. from index.dat)
    if (initReloadStructure() != 1){
      fprintf(stderr, "Could not initialize data structures. Exiting");
      exit(1);
    }

    int reloadResult = reloadIndexFromFile(loadFile, writeReload);
    if (reloadResult != 1){
      fprintf(stderr, "Could not reload the index from the file! \n");
      exit(1);
    } else {
      LOG("Reloading index from file finished");
    }

    // clean up here;
    cleanupIndex(indexReload);

    // run a script to test the integrity of the reloaded file

  }
  return 0;
}
