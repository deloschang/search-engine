/*

FILE: index.c

Description: Shared functions that sanitize a file and loads it into memory.
Then, reconstruct an inverted index from the file into memory. 

Inputs: 

Outputs: 


 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "index.h"
#include "hash.h"

// This function initializes the reloaded index structure that will be used
// to "reload" the index via reading the index.dat and writing output to
// an index_new.dat. We do this to make sure that the index file can be 
// properly retrieved
INVERTED_INDEX* initReloadStructure(){
  // create the index structure
  INVERTED_INDEX* indexReload = (INVERTED_INDEX*)malloc(sizeof(INVERTED_INDEX));
  MALLOC_CHECK(indexReload);
  indexReload->start = indexReload->end = NULL;
  BZERO(indexReload, sizeof(INVERTED_INDEX));

  /*if (indexReload == NULL){*/
    /*return 0;*/
  /*}*/

  return indexReload;
}

// Cleans up the index by freeing the wordnode, documentnode
// and entire index
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

      // for each wordNode, loop through each of the DocNodes
      startPage = startWordNode->page;
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

// Function that filters capital letters and makes them
// lower case letters
void capitalToLower(char* buffer){
    for(int index = 0; buffer[index]; index++){
        if (buffer[index] >= 'A' && buffer[index] <= 'Z'){
            buffer[index] = 'a' + buffer[index] - 'A';
        } 
    }
}

// Strips the buffer of non-letters
// sanitize: this will sanitize a buffer, stripping everything that should not be
// parsed into words: e.g. -- newline characters, @, & etc. When choosing what to 
// sanitize, there is a trade off between obfuscating possible words that could be
// using them. 
void sanitize(char* loadedDocument){
  char* temp;
  char* clear;

  // set aside buffer for the document
  temp = malloc(sizeof(char) * strlen(loadedDocument) + 1); 
  BZERO(temp, (strlen(loadedDocument) + 1)); // to be safe

  // Initialize a clear 
  // We will put all valid letters into this and then transfer it
  // into the temp buffer
  clear = malloc(sizeof(char) + 1);
  BZERO(clear, sizeof(char));

  // loop through document until end
  for (int i = 0; loadedDocument[i]; i++){

    // only copy into buffer if it is valid by the following parameters
    if (loadedDocument[i] > 13){
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

// Loads the file into memory
char* loadDocument(char* filepath){
  FILE* fp;
  char* html = NULL;

  fp = fopen(filepath, "r");

  // If unable to find file, skip.
  // Could be problem of skipped files i.e. (1, 3, 4, 5)
  if (fp == NULL){
    fprintf(stderr, "Could not read file %s. Aborting. \n", filepath);
    /*cleanupIndex(indexReload);*/
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

// reconstructIndex: this function will reconstruct the entire index with a
// word, document ID and page frequency passed to it. It is used in 
// debug mode to ensure that the index can be "reloaded" 
int reconstructIndex(char* word, int documentId, int page_word_frequency, INVERTED_INDEX* indexReload){
  int wordHash = hash1(word) % MAX_NUMBER_OF_SLOTS;

  // check if hash slot occupied first
  if (indexReload->hash[wordHash] == NULL){
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
    docNode->page_word_frequency = page_word_frequency;

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
    indexReload->hash[wordHash] = wordNode;

    // change end of inverted Index
    indexReload->end = wordNode;
  } else {
    // occupied, move down the list checking for identical WordNode
    WordNode* checkWordNode = indexReload->hash[wordHash];

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
          docNode->page_word_frequency = page_word_frequency;

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
      docNode->page_word_frequency = page_word_frequency;

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
  return 1;
}

// saves the inverted index into a file
// returns 1 if successful, 0 if not
// saveIndexToFile: this function will save the index in memory to a file
// by going through each WordNode and Document Node, parsing them 
// and writing them in the specified format
// cat 2 2 3 4 5 
// the first 2 indicates that there are 2 documents with 'cat' found
// the second 2 indicates the document ID with 3 occurrences of 'cat'
// the 4 indicates the document ID with 5 occurrences of 'cat'
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
  char* sortCommand = (char*) malloc(string1 + string2 + string2 + 2); // last byte

  sprintf(sortCommand, "%s%s %s", "sort -o ", targetFile, targetFile); 
  int result = system(sortCommand);

  // check the exit status
  if ( result != 0){
    fprintf(stderr, "Error: Could not sort the file %s. \n", targetFile);

    free(sortCommand);
    exit(1);
  }
  free(sortCommand);

  return 1;
}

// "reloads" the index data structure from the file 
// reloadIndexFromFile: This function does the heavy lifting of 
// "reloading" a file into an index in memory. It goes through
// each of the characters and uses strtok to split by space
INVERTED_INDEX* reloadIndexFromFile(char* loadFile, char* writeReload, INVERTED_INDEX* indexReload){
  FILE* fp;

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

  // every line represents a word node with all its Document Nodes
  while(1){
    // continue until end
    if (fgets(placeholder, string1 + 1, fp) == NULL){
      break;
    } else {

      // sanitize first so that all characters are valid to be parsed
      sanitize(placeholder);

      char buffer[string1 + 1];
      strcpy(buffer, placeholder);

      // takes the first word from the line
      char* docNode;
      char* wordNode = strtok(buffer, " ");
      strtok(NULL, " ");

      while ((docNode = strtok(NULL, " ")) != NULL){
        char* page_word_frequency = strtok(NULL, " ");
        // insert into index here
        int docId = atoi(docNode);
        int pageFreq = atoi(page_word_frequency);

        int result = reconstructIndex(wordNode, docId, pageFreq, indexReload);

        if (result != 1){
          fprintf(stderr, "Reconstruction failed for the word %s \n", wordNode);
        }

      }
    }

  }

  // Write the index to the file
  /*saveIndexToFile(indexReload, writeReload);*/

  fclose(fp);
  free(placeholder);
  /*return 1;*/
  return indexReload;
}
