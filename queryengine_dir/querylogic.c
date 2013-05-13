/*

FILE: querylogic.c
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

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/index.h"
#include "../utils/hash.h"
#include "querylogic.h"
 
int resultSlot[1000]; // stores the document_id of matches
int nextFreeSlot = 0; // stores the position of the next free slot in results array
int next_free = 0; // stores the position of the next free slot in results array

// converts the raw query from the command line processing into
// a list of words to be cross-referenced with the index
char** curateWords(char** queryList, char* query){
  char* keyword = NULL;

  // create storage for the query command
  char copy[1000];
  strcpy(copy, query);

  /// BEGIN PARSING THE QUERY ///
  keyword = strtok(copy, " ");

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

  return queryList;
}

// Filters the keywords with sanitize. But if a keyword is
// OR then don't sanitize, because that is distinct from 'or'
void sanitizeKeywords(char** queryList){
  // convert keywords to lowercase
  // if the keyword is OR then don't convert
  for (int i=0; queryList[i]; i++){
    // if it matches special operators OR and AND, skip.
    if (!strncmp(queryList[i], "OR", strlen("OR") + 1) ){
      continue;
    }

    if (!strncmp(queryList[i], "AND", strlen("AND") + 1) ){
      continue;
    }

    capitalToLower(queryList[i]);
  }
}


void cleanUpList(DocumentNode** usedList){
  int i = 0;
  while(usedList[i] != NULL){
    free(usedList[i]);
    usedList[i] = NULL;
    i++;
  }
}

int rankSplit(DocumentNode** saved, int l, int r){
  DocumentNode* t;
  int pivot, i, j;

  pivot = saved[l]->page_word_frequency;
  i = l;
  j = r + 1;

  while (1){
    do ++i; while( saved[i]->page_word_frequency >= pivot && i < r);
    do --j; while( saved[j]->page_word_frequency < pivot);
    if ( i >= j){
      break;
    }
    t = saved[i];
    saved[i] = saved[j];
    saved[j] = t;

  }

  t = saved[l];
  saved[l] = saved[j];
  saved[j] = t;

  return j;
}

// implements QuickSort in descending order
void rankByFrequency(DocumentNode** saved, int l, int r){
  int num;

  if (l < r){
    // divide and conquer
    num = rankSplit(saved, l, r);
    rankByFrequency(saved, l, num - 1);
    rankByFrequency(saved, num + 1, r);
  }

}

// Duplicates the Document nodes
void copyDocNode(DocumentNode* docNode, DocumentNode* orig){
  if (docNode == NULL){
    fprintf(stderr, "Out of memory for indexing! Aborting. \n");
  } else {
    MALLOC_CHECK(docNode);
    BZERO(docNode, sizeof(DocumentNode));
    docNode->next = NULL; 
    docNode->document_id = orig->document_id;
    docNode->page_word_frequency = orig->page_word_frequency;
  }
}

// given a word to search for, this function will return a list of 
// DocumentNodes with the word in it
DocumentNode** searchForKeyword(DocumentNode** list, char* keyword, INVERTED_INDEX* indexReload){
  // look for the keyword in the inverted index
  int wordHash = hash1(keyword) % MAX_NUMBER_OF_SLOTS;

  WordNode* checkWordNode = NULL; 
  checkWordNode = indexReload->hash[wordHash];

  // Word could not be found in indexer
  if (checkWordNode == NULL){
    // move on to next word
    return NULL;
  }

  // hash slot wasn't empty
  WordNode* matchedWordNode = NULL;

  // loop through each wordNode
  for (checkWordNode = indexReload->hash[wordHash];
      checkWordNode != NULL; checkWordNode = checkWordNode->next){
    if (!strncmp(checkWordNode->word, keyword, WORD_LENGTH)){
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
    int num = 0;
    while(matchedDocNode != NULL){
      // save into the list and return
      DocumentNode* docNode = (DocumentNode*)malloc(sizeof(DocumentNode));
      copyDocNode(docNode, matchedDocNode);

      list[num] = docNode;

      matchedDocNode = matchedDocNode->next;
      num++;
    }
    return list;

  } else {
    printf("Word %s could not be found in indexer \n", keyword);
    return NULL;
  }

}

void printOutput(DocumentNode* matchedDocNode, char* urlDir){
  int document_id_int = matchedDocNode->document_id;

  ///////////// REFACTOR ///////////////
  char* readableTest;

  char* document_id;
  document_id = malloc(sizeof(char) * 1000);
  BZERO(document_id, 1000);
  sprintf(document_id, "%d", document_id_int);

  size_t string1 = strlen(urlDir); 
  size_t string2 = strlen("/");
  size_t string3 = strlen(document_id);

  readableTest = (char*) malloc(string1 + string2 + string3 + 1);
  sprintf(readableTest, "%s/%s", urlDir, document_id);

  free(document_id);

  /////////// REFACTOR /////////////////

  FILE* fp;
  fp = fopen(readableTest, "r");
  if (fp == NULL){
    fprintf(stderr, "Error opening the document! \n");
    free(readableTest);
    exit(1);
  }

  char* docURL;
  docURL = (char*) malloc(sizeof(char) * MAX_URL_LENGTH);
  BZERO(docURL, MAX_URL_LENGTH);

  if (fgets(docURL, MAX_URL_LENGTH, fp) == NULL){
    fprintf(stderr, "Error copying URL from document. \n");

    free(docURL);
    free(readableTest);
    exit(1);
  }

  /*printf("Document ID:%d URL:%s freq:%d \n", matchedDocNode->document_id, docURL, matchedDocNode->page_word_frequency);*/
  printf("Document ID:%d URL:%s", matchedDocNode->document_id, docURL);

  fclose(fp);
  free(docURL);
  free(readableTest);
}

// returns the intersection of the two lists (final and list)
DocumentNode** intersection(DocumentNode** final, DocumentNode** list,
    DocumentNode** result, int* resultSlot){
  int i = 0; 
  int j = 0;

  int nextFreeSlot = 0;

  while ( (final[i] != NULL) ){
    while ( (list[j] != NULL) ) {

      // check if the doc ID's match
      if ( (final[i]->document_id == list[j]->document_id) ){
        // check if the DocNode has been added already
        // since DocIDs are unique, there cannot be collisions
        if ( result[final[i]->document_id] != NULL){
          // should not happen
          fprintf(stderr, "Warning: Doc Id: %d is colliding (multiple docs of the same id). Skipping.\n", final[i]->document_id);


        } else {
          // DocNode doesn't exist already

          // REFACTOR ***** 
          DocumentNode* docNode = (DocumentNode*)malloc(sizeof(DocumentNode));

          if (docNode == NULL){
            fprintf(stderr, "Out of memory for indexing! Aborting. \n");
            return 0;
          }

          MALLOC_CHECK(docNode);
          BZERO(docNode, sizeof(DocumentNode));
          docNode->next = NULL; 
          docNode->document_id = final[i]->document_id;

          // combine the two frequencies for ranking algorithm
          docNode->page_word_frequency = final[i]->page_word_frequency +
            list[j]->page_word_frequency;

          // add docNode into the result list
          result[final[i]->document_id] = docNode;

          // store the valid document ID into resultSlot for looping
          resultSlot[nextFreeSlot] = final[i]->document_id;
          nextFreeSlot++;
        }
        break;
      }
      j++;
    }

    i++;
    j = 0; // start from beginning of other list again
  }
  return result;
}

// copies the two Document Node lists by copying the DocNode
// lists over
void copyList(DocumentNode** result, DocumentNode** orig){
  int i = 0;
  while (orig[i] != NULL){
    DocumentNode* docNode = (DocumentNode*)malloc(sizeof(DocumentNode));
    copyDocNode(docNode, orig[i]);
    result[i] = docNode;

    i++;
  }
}

// looks up the keywords
// returns 1 if successful
int lookUp(char** queryList, char* urlDir, INVERTED_INDEX* indexReload){
  // loop through each keyword
  // REFACTOR *** return a match if found. otherwise return NULL
  DocumentNode* temp[1000];
  BZERO(temp, 1000);

  DocumentNode* tempHolder[1000]; 
  BZERO(tempHolder, 1000);

  // placeholder for result intersections etc.
  DocumentNode* result[10000]; // should be max number of possible files
  BZERO(result, 10000);

  // list used to be returned at end
  DocumentNode* saved[1000];
  BZERO(saved, 1000);

  DocumentNode* list[1000];
  BZERO(list, 1000);


  int firstRunFlag = 1;


  // if there is a 'space', it will default to AND'ing with orFlag = 0;
  int orFlag = 0;
  for (int i=0; queryList[i]; i++){

    // if the word is OR, that means we will concatenate
    // otherwise, default to AND'ing
    if (!strncmp(queryList[i], "OR", strlen("OR") + 1) ){
      orFlag = 1;
      continue;
    }

    if (!strncmp(queryList[i], "AND", strlen("AND") + 1) ){
      orFlag = 0;
      continue;
    }

    BZERO(list, 1000);

    searchForKeyword(list, queryList[i], indexReload);

    // if nothing is in tempHolder yet
    if ( tempHolder[0] == NULL && firstRunFlag){
      /*int j = 0;*/
      // save the list to the tempHolder list
      copyList(tempHolder, list);
      cleanUpList(list);

      firstRunFlag = 0;
    } else {
      if (orFlag == 1 ){
        // OR'ing

        // bank the "tempHolder" list and start a new tempHolder list
        // start where last left off using next_free
        int index = 0;
        while (tempHolder[index]){
          saved[next_free] = tempHolder[index];
          index++;
          next_free++;
        }

        BZERO(tempHolder, 1000); // clear out the "tempHolder" list because it was banked

        // move current results into the "tempHolder" list
        copyList(tempHolder, list);
        cleanUpList(list);

        orFlag = 0;
      } else {
        // AND'ing (default)
        // AND list and tempHolder together

        if (list[0] != NULL){
          // e.g. "dog cat"
          // dog --> stored in tempHolder
          // cat --> stored in list 
          intersection(tempHolder, list, result, resultSlot);
          // intersection will be stored in result
          // result needs to be indexed by matching document id

          // free helper lists 
          cleanUpList(tempHolder); 
          BZERO(tempHolder, 1000);

          cleanUpList(list); 
          BZERO(list, 1000);

          // copy result back
          int k = 0;
          if (resultSlot[k] != '\0'){
            while (resultSlot[k]){
              // move the docNodes into tempHolder
              /*tempHolder[k] = result[resultSlot[k]];*/
              DocumentNode* docNode = (DocumentNode*)malloc(sizeof(DocumentNode));
              copyDocNode(docNode, result[resultSlot[k]]);
              tempHolder[k] = docNode;

              // freeing the matched DocNodes and putting them in tempHolder
              free(result[resultSlot[k]]);
              k++;
            }

          }

        } else {
          // edge case where no results were found
          // e.g. "alskdfjsalkdfjk asdflkjsldf"
          cleanUpList(tempHolder);
          BZERO(tempHolder, 1000);

          BZERO(temp, 1000);
          firstRunFlag = 0;
        }

        BZERO(result, 10000);
        BZERO(resultSlot, 1000);
        nextFreeSlot = 0;
      }
    }
  }
  //////// end of for loop ////////

  // neither AND or OR
  // e.g. "dog"
  if (tempHolder[0] != NULL ){
    int index = 0;
    while (tempHolder[index]){
      DocumentNode* docNode = (DocumentNode*)malloc(sizeof(DocumentNode));
      copyDocNode(docNode, tempHolder[index]);
      saved[next_free] = docNode;

      index++;
      next_free++;
    }

    cleanUpList(tempHolder);
  }


  // Ranking algorithm
  // saved has the desired list, sort by page frequency
  if (saved[0] != NULL){

    // count length 
    int num = 0;
    while (saved[num] != NULL){
      num++;
    }

    // Simple ranking algorithm by page frequency
    rankByFrequency(saved, 0, num - 1);

    // sanity check
    num = 0;
    while (saved[num] != NULL){
      printOutput(saved[num], urlDir);

      num++;
    }
  } else {
    printf("No matches from search \n \n");
  }


  // reset
  BZERO(resultSlot, 1000);
  nextFreeSlot = 0;
  next_free = 0;  // for saved, reset for the new search

  BZERO(result, 10000);

  cleanUpList(saved);
  BZERO(saved, 1000);

  return 1;
}

void cleanUpQueryList(char** queryList){
  int i = 0;
  while (queryList[i]){
    free(queryList[i]);
    i++;
  }
}
