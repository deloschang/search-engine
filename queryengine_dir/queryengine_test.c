// Filename: Test cases for queryengine.h/.c and querylogic.h/.c
// Description: A unit test harness for query engine processing
// 

//
// Test Harness Spec:
// ------------------
//
//  It uses these files but they are not unit tested in this test harness:
//
//   void sanitizeKeywords(char** queryList);
//   void cleanUpIndex(INVERTED_INDEX* index);
//   DocumentNode* newDocNode(DocumentNode* docNode, int docId, int page_freq);
//   void cleanUpQueryList(char** queryList);
//   
// 
//  It tests the following functions:
//
//   INVERTED_INDEX* initStructure(INVERTED_INDEX* index);
//   DocumentNode** intersection(DocumentNode** final, DocumentNode** list,
//   char** curateWords(char** queryList, char* query);
//   void rankByFrequency(DocumentNode** saved, int l, int r);
//
//  If any of the tests fail it prints status 
//  If all tests pass it prints status.
//
//  Test Cases:
//  -----------
//   
//  The test harness runs a number of test cases to test the code.
//  The approach is to first set up the environment for the test,
//  invoke the function to be tested, then validate the state of
//  the data structures using the SHOULD_BE macro. This is repeated
//  for each test case. 
//
//  The test harness isolates the functions under test and setting
//  up the environment the code expects when integrated in the real
//  system.
//
  /*The test harness dummies out the real hash function and through*/
  /*use of a variable called hash manipulates where DNODEs are */
  /*inserted into the DICTIONARY. For example, collisions can be controlled. */
//
//  The following test cases  (1) are for function:
//
//  INVERTED_INDEX* initStructure(INVERTED_INDEX* index);
//
//  Test case: TestStructure:1
//  This test case calls initStructure() and checks to make sure 
//  search engine can initialize the basic inverted index structure
//  
//  The following test cases  (1-3) are for function:
//
//  DocumentNode** intersection(DocumentNode** final, DocumentNode** list,
//    DocumentNode** result, int* resultSlot);
//
//  Test case: TestANDOp:1
//  This test case calls intersection() for the condition where one list is empty
//  and the other is not. The result should be that the intersection will be
//  an empty list
//
//  Test case: TestANDOp:2
//  This test case calls intersection() for the condition where both lists
//  are not empty. They have the same DocumentNode. The result should be that the 
//  intersection will NOT be an empty list
//
//  Test case: TestANDOp:3
//  This test case calls intersection() for the condition where both lists
//  are not empty. They do not have the same DocumentNode. The intersection
//  and thus resultant list should be empty 
//
//  The following test cases (1-2) for function:
//
//   char** curateWords(char** queryList, char* query);
//
//  Test case: TestCurate:1
//  This test case calls curateWords() for the condition where the query
//  are all keywords (no non-alpha characters)
//
//  Test case: TestCurate:2
//  This test case calls curateWords() for the condition where the query
//  is empty.
//
//  The following test cases (1) for function:
//
//   void rankByFrequency(DocumentNode** saved, int l, int r);
//
//  Test case: TestRanking:1
//  This test case calls rankByFrequency() for the condition the DocumentNode
//  page frequencies are different and need to be ranked accordingly
//
//  Test case:DREMOVE:4
//  This test case is tries to see how DRemove() works with multiple nodes of the same hash value, 
//  the node to be deleted is at the middle of the dynamic list.
//
//  The following test cases (1) for function:
//
//  void* GetDataWithKey(DICTIONARY* dict, char* key);
//
//  Test case:GetDataWithKey:1
//  This test case tests GetDataWithKey - to get a data with the a certain key.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/header.h"
#include "../utils/index.h"
#include "../utils/file.h"
#include "querylogic.h"

// Useful MACROS for controlling the unit tests.

// each test should start by setting the result count to zero
#define START_TEST_CASE  int rs=0

// check a condition and if false print the test condition failed
// e.g., SHOULD_BE(dict->start == NULL)
// note: the construct "#x" below is the sstringification preprocessor operator that
//       converts the argument x into a character string constant
#define SHOULD_BE(x) if (!(x))  {rs=rs+1; \
    printf("Line %d test [%s] Failed\n", __LINE__,#x); \
  }


// return the result count at the end of a test
#define END_TEST_CASE return rs

//
// general macro for running a best
// e.g., RUN_TEST(TestDAdd1, "DAdd Test case 1");
// translates to:
// if (!TestDAdd1()) {
//     printf("Test %s passed\n","DAdd Test case 1");
// } else { 
//     printf("Test %s failed\n", "DAdd Test case 1");
//     cnt = cnt +1;
// }
//

#define RUN_TEST(x, y) if (!x()) {              \
    printf("Test %s passed\n", y);              \
} else {                                        \
    printf("Test %s failed\n", y);              \
    cnt = cnt + 1;                              \
}
    
// Dumming out routines 
// We want to isolate this set of functions and test them and control
// various conditions with the hash table and function. For example,
// we want to create collisions. So in order to do this we dummy out the
// hash function and make it return whatever the current value of hash is
// In out test suite we mainpulate the value of hash so when the "real code"
// calls our dummy hash function it always returns the value we set in hash.
// Devious, hey?

// what we want the hash function to return

unsigned long hash = 0;

// The dummy hash function, which returns the value we set up in hash

unsigned long hash1(char* str) {
  return hash;
}


// Main reloaded index structure
INVERTED_INDEX* indexReload = NULL;

void create(){
  indexReload = initStructure(indexReload);
  LOG("Initializing INVERTED INDEX structure");
}

// Test case: TestStructure:1
// This test case calls initStructure() and checks to make sure 
// search engine can initialize the basic inverted index structure
int TestStructure1(){
  START_TEST_CASE;
  LOG("Initializing INVERTED INDEX structure");

  indexReload = initStructure(indexReload);
  SHOULD_BE(indexReload != NULL);

  cleanUpIndex(indexReload);
  END_TEST_CASE;
}

// Test case: TestArgs:1
// This test case calls reloadIndexFromFile() and lookUp() and 
// checks to make sure that all arguments are processed
// by the query engine successfully
int TestArgs1() {
  START_TEST_CASE;

  create();

  INVERTED_INDEX* result = NULL;
  result = reloadIndexFromFile("../indexer_dir/index.dat", indexReload);
  SHOULD_BE(result != NULL);
  LOG("Reloading INVERTED INDEX structure");

  char query[1000] = "andrew";
  sanitize(query);

  char* queryList[1000];
  curateWords(queryList, query);

  sanitizeKeywords(queryList); 

  int lookUpResult = 0;
  lookUpResult = lookUp(queryList, "../crawler_dir/data", indexReload);

  SHOULD_BE(lookUpResult == 1);

  cleanUpQueryList(queryList);
  cleanUpIndex(indexReload);

  END_TEST_CASE;
}

// Test case: TestANDOp:1
// This test case calls intersection() for the condition where one list is empty
// and the other is not. The result should be that the intersection will be
// an empty list
int TestANDOp1() {
  START_TEST_CASE;

  int resultSlot[1000];
  DocumentNode* result[10000];
  BZERO(result, 10000);
  DocumentNode* list1[1000];
  BZERO(list1, 1000);
  DocumentNode* list2[1000];
  BZERO(list2, 1000);

  DocumentNode* docNode = NULL;
  docNode = newDocNode(docNode, 15, 1);

  list1[0] = docNode;

  intersection(list1, list2, result, resultSlot);
  SHOULD_BE(result[15] == NULL);

  free(docNode);

  END_TEST_CASE;
}

// Test case: TestANDOp:2
// This test case calls intersection() for the condition where both lists
// are not empty. They have the same DocumentNode. The result should be that 
// the intersection will NOT be an empty list (same DocumentNode)
int TestANDOp2() {
  START_TEST_CASE;

  int resultSlot[1000];
  DocumentNode* result[10000];
  BZERO(result, 10000);
  DocumentNode* list1[1000];
  BZERO(list1, 1000);
  DocumentNode* list2[1000];
  BZERO(list2, 1000);

  DocumentNode* docNode = NULL;
  docNode = newDocNode(docNode, 15, 1);

  DocumentNode* docNode2 = NULL;
  docNode2 = newDocNode(docNode2, 15, 1);

  list1[0] = docNode;
  list2[0] = docNode2;

  intersection(list1, list2, result, resultSlot);
  SHOULD_BE(result[15] != NULL);

  free(docNode);
  free(docNode2);

  END_TEST_CASE;
}

// Test case: TestANDOp:3
// This test case calls intersection() for the condition where both lists
// are not empty. They do not have the same DocumentNode. The intersection
// and thus resultant list should be empty 
int TestANDOp3() {
  START_TEST_CASE;

  create();

  INVERTED_INDEX* indexResult = NULL;
  indexResult = reloadIndexFromFile("../indexer_dir/index.dat", indexReload);
  SHOULD_BE(indexResult != NULL);
  LOG("Reloading INVERTED INDEX structure");

  int resultSlot[1000];
  DocumentNode* result[10000];
  BZERO(result, 10000);
  DocumentNode* list1[1000];
  BZERO(list1, 1000);
  DocumentNode* list2[1000];
  BZERO(list2, 1000);

  DocumentNode* docNode = NULL;
  docNode = newDocNode(docNode, 15, 1);

  DocumentNode* docNode2 = NULL;
  docNode2 = newDocNode(docNode2, 16, 1);

  list1[0] = docNode;
  list2[0] = docNode2;

  intersection(list1, list2, result, resultSlot);
  SHOULD_BE(result[0] == NULL);

  free(docNode);
  free(docNode2);

  cleanUpIndex(indexReload);
  END_TEST_CASE;
}

// Test case: TestCurate:1
// This test case calls curateWords() for the condition where the query
// are all keywords (no non-alpha characters)
int TestCurate1() {
  START_TEST_CASE;

  char query[1000] = "dog cat mouse world";
  sanitize(query);

  char* queryList[1000];
  curateWords(queryList, query);

  SHOULD_BE(strcmp(queryList[0],"dog") == 0);
  SHOULD_BE(strcmp(queryList[1],"cat") == 0);
  SHOULD_BE(strcmp(queryList[2],"mouse") == 0);
  SHOULD_BE(strcmp(queryList[3],"world") == 0);

  END_TEST_CASE;
}

// Test case: TestCurate:2
// This test case calls curateWords() for the condition where the query
// is empty.
int TestCurate2() {
  START_TEST_CASE;

  char query[1000] = "";
  sanitize(query);

  char* queryList[1000];
  curateWords(queryList, query);

  SHOULD_BE(strcmp(queryList[0],"") == 0);

  END_TEST_CASE;
}

// Test case: TestRanking:1
// This test case calls rankByFrequency() for the condition the DocumentNode
// page frequencies are different and need to be ranked accordingly
int TestRanking1() {
  START_TEST_CASE;

  DocumentNode* list1[1000];
  BZERO(list1, 1000);

  DocumentNode* docNode = NULL;
  docNode = newDocNode(docNode, 1, 1);

  DocumentNode* docNode2 = NULL;
  docNode2 = newDocNode(docNode2, 16, 2);

  DocumentNode* docNode3 = NULL;
  docNode3 = newDocNode(docNode3, 1, 3);

  DocumentNode* docNode4 = NULL;
  docNode4 = newDocNode(docNode4, 1, 4);

  list1[0] = docNode4;
  list1[1] = docNode3;
  list1[2] = docNode2;
  list1[3] = docNode;

  rankByFrequency(list1, 0, 3);

  free(docNode);
  free(docNode2);
  free(docNode3);
  free(docNode4);

  END_TEST_CASE;
}

/*int TestDAdd2() {*/
  /*START_TEST_CASE;*/
  /*DICTIONARY* dict = InitDictionary();*/
  /*int *d1; d1 = (int *)malloc(sizeof(int)); *d1 = 1;*/
  /*int *d2; d2 = (int *)malloc(sizeof(int)); *d2 = 2;*/
  /*int *d3; d3 = (int *)malloc(sizeof(int)); *d3 = 3;*/
  /*hash = 0;*/
  /*DAdd(dict, d1, "1");*/
  /*hash = 1;*/
  /*DAdd(dict, d2, "2");*/
  /*hash = 2;*/
  /*DAdd(dict, d3, "3");*/
  /*SHOULD_BE(dict->end == dict->hash[2]);*/
  /*SHOULD_BE(dict->start == dict->hash[0]);*/
  /*SHOULD_BE(dict->hash[0]->data == (void*)d1);*/
  /*SHOULD_BE(dict->hash[0]->prev == (void*)NULL);*/
  /*SHOULD_BE(dict->hash[0]->next == dict->hash[1]);*/
  /*SHOULD_BE(dict->hash[1]->data == (void*)d2);*/
  /*SHOULD_BE(dict->hash[1]->prev == dict->hash[0]);*/
  /*SHOULD_BE(dict->hash[1]->next == dict->hash[2]);*/
  /*SHOULD_BE(dict->hash[2]->data == (void*)d3);*/
  /*SHOULD_BE(dict->hash[2]->prev == dict->hash[1]);*/
  /*SHOULD_BE(dict->hash[2]->next == (void*)NULL);*/
  /*CleanDictionary(dict);*/
  /*free(dict);*/
  /*END_TEST_CASE;*/
/*}*/


// Test case: DADD:3
// This test case calls DAdd() puts multiple DNODEs on the dict when there is hash collisions
// We put multiply elements in dictionary with collisions.

/*int TestDAdd3() {*/
  /*START_TEST_CASE;*/
  /*DICTIONARY* dict = InitDictionary();*/
  /*int *d1; d1 = (int *)malloc(sizeof(int)); *d1 = 1;*/
  /*int *d2; d2 = (int *)malloc(sizeof(int)); *d2 = 2;*/
  /*hash = 0;*/
  /*DAdd(dict, d1, "1");*/
  /*hash = 0;*/
  /*DAdd(dict, d2, "2");*/
  /*SHOULD_BE(dict->end == dict->hash[0]->next);*/
  /*SHOULD_BE(dict->start == dict->hash[0]);*/
  /*SHOULD_BE(dict->hash[0]->data == (void*)d1);*/
  /*SHOULD_BE(dict->hash[0]->prev == NULL);*/
  /*SHOULD_BE(dict->hash[0]->next->data == (void*)d2);*/
  /*SHOULD_BE(dict->hash[0]->next->prev == dict->hash[0]);*/
  /*SHOULD_BE(dict->hash[0]->next->next == NULL);  */
  /*CleanDictionary(dict);*/
  /*free(dict);*/
  /*END_TEST_CASE;*/
/*}*/

// Test case:DREMOVE:1
// This test case DAdd() and DRemove()  DNODE from dict for only one element.
/*int TestDRemove1() {*/
  /*START_TEST_CASE;*/
  /*DICTIONARY* dict = InitDictionary();*/
  /*int *d1; d1 = (int *)malloc(sizeof(int)); *d1 = 1;*/
  /*hash = 0;*/
  /*DAdd(dict, d1, "1");*/
  /*DRemove(dict, "1");*/
  /*SHOULD_BE(dict->hash[0] == NULL);*/
  /*SHOULD_BE(dict->start == NULL);*/
  /*SHOULD_BE(dict->end == NULL);*/
  /*CleanDictionary(dict);*/
  /*free(dict);*/
  /*END_TEST_CASE;*/
/*}*/


/*// Test case:DREMOVE:2*/
/*// This test case is tries to see how DRemove() works with multiple nodes for the same hash value, the node to be deleted is at the end of the dynamic list.*/

/*int TestDRemove2() {*/
  /*START_TEST_CASE;*/
  /*DICTIONARY* dict = InitDictionary();*/
  /*int *d1; d1 = (int *)malloc(sizeof(int)); *d1 = 1;*/
  /*int *d2; d2 = (int *)malloc(sizeof(int)); *d1 = 2;*/
  /*hash = 0;*/
  /*DAdd(dict, d1, "1");*/
  /*DAdd(dict, d2, "2");*/
  /*DRemove(dict, "2");*/
  /*SHOULD_BE(dict->hash[0]->data == (void*)d1);*/
  /*SHOULD_BE(dict->start == dict->hash[0]);*/
  /*SHOULD_BE(dict->end == dict->hash[0]);*/
  /*SHOULD_BE(dict->hash[0]->prev == NULL);*/
  /*SHOULD_BE(dict->hash[0]->next == NULL);*/
  /*CleanDictionary(dict);*/
  /*free(dict);*/
  /*END_TEST_CASE;*/
/*}*/


/*// Test case:DREMOVE:3*/
/*// This test case is tries to see how DRemove() works with multiple nodes of the same hash value, the node to be deleted is at the start of the dynamic list*/

/*int TestDRemove3() {*/
  /*START_TEST_CASE;*/
  /*DICTIONARY* dict = InitDictionary();*/
  /*int *d1; d1 = (int *)malloc(sizeof(int)); *d1 = 1;*/
  /*int *d2; d2 = (int *)malloc(sizeof(int)); *d2 = 2;*/
  /*hash = 0;*/
  /*DAdd(dict, d1, "1");*/
  /*DAdd(dict, d2, "2");*/
  /*DRemove(dict, "1");*/
  /*SHOULD_BE(dict->hash[0]->data == (void*)d2);*/
  /*SHOULD_BE(dict->start == dict->hash[0]);*/
  /*SHOULD_BE(dict->end == dict->hash[0]);*/
  /*SHOULD_BE(dict->hash[0]->prev == NULL);*/
  /*SHOULD_BE(dict->hash[0]->next == NULL);*/
  /*CleanDictionary(dict);*/
  /*free(dict);*/
  /*END_TEST_CASE;*/
/*}*/


/*// Test case:DREMOVE:4*/
/*// This test case is tries to see how DRemove() works with multiple nodes of the same hash value, the node to be deleted is at the middle of the dynamic list*/
/*int TestDRemove4() {*/
  /*START_TEST_CASE;*/
  /*DICTIONARY* dict = InitDictionary();*/
  /*int *d1; d1 = (int*)malloc(sizeof(int)); *d1 = 1;*/
  /*int *d2; d2 = (int*)malloc(sizeof(int)); *d2 = 2;*/
  /*int *d3; d3 = (int*)malloc(sizeof(int)); *d3 = 3;*/
  /*hash = 0;*/
  /*DAdd(dict, d1, "1");*/
  /*hash = 1;*/
  /*DAdd(dict, d2, "2");*/
  /*DAdd(dict, d3, "3");*/
  /*DRemove(dict, "2");*/
  /*SHOULD_BE(dict->start == dict->hash[0]);*/
  /*SHOULD_BE(dict->end == dict->hash[1]);*/
  /*SHOULD_BE(dict->hash[0]->data == (void*)d1);*/
  /*SHOULD_BE(dict->hash[1]->data == (void*)d3);*/
  /*SHOULD_BE(dict->hash[1]->prev == dict->hash[0]);*/
  /*SHOULD_BE(dict->hash[1]->next == NULL);*/
  /*CleanDictionary(dict);*/
  /*free(dict);*/
  /*END_TEST_CASE;*/
/*}*/

/*// Test case:GetDataWithKey:1*/
/*// This test case tests GetDataWithKey - to get a data with the a certain key.*/
/*int TestGetData1() {*/
  /*START_TEST_CASE;*/
  /*DICTIONARY* dict = InitDictionary();*/
  /*int *d1; d1 = (int*)malloc(sizeof(int)); *d1 = 1;*/
  /*int *d2; d2 = (int*)malloc(sizeof(int)); *d2 = 2;*/
  /*int *d3; d3 = (int*)malloc(sizeof(int)); *d3 = 3;*/
  /*hash = 0;*/
  /*DAdd(dict, d1, "1");*/
  /*hash = 1;*/
  /*DAdd(dict, d2, "2");*/
  /*DAdd(dict, d3, "3");*/
  /*hash = 0;*/
  /*SHOULD_BE(GetDataWithKey(dict, "1") == (void*)d1);*/
  /*hash = 1;*/
  /*SHOULD_BE(GetDataWithKey(dict, "2") == (void*)d2);*/
  /*SHOULD_BE(GetDataWithKey(dict, "3") == (void*)d3);*/
  /*// case that there is no such key in the dictionary*/
  /*SHOULD_BE(GetDataWithKey(dict, "") == NULL);*/
  /*SHOULD_BE(GetDataWithKey(dict, "4") == NULL);*/
  /*CleanDictionary(dict);*/
  /*free(dict);*/
  /*END_TEST_CASE;*/
/*}*/

// This is the main test harness for the set of dictionary functions. It tests all the code
// in dictionary.c:
//
//  It uses these files but they are not unit tested in this test harness:
//
//  DICTIONARY* InitDictionary();
//  int make_hash(char* c);
//  void CleanDictionary(DICTIONARY* dict)
// 
//  It test the following functions:
//
//   void DAdd(DICTIONARY* dict, void* data, char* key);
//   void DRemove(DICTIONARY* dict, char* key);
//   void* GetDataWithKey(DICTIONARY* dict, char* key);
//
//  If any of the tests fail it prints status 
//  If all tests pass it prints status.

int main(int argc, char** argv) {
  int cnt = 0;

  RUN_TEST(TestStructure1, "Structure Test case 1");
  RUN_TEST(TestArgs1, "CheckArgs Test case 1");
  RUN_TEST(TestANDOp1, "AND operator Test case 1");
  RUN_TEST(TestANDOp2, "AND operator Test case 2");
  RUN_TEST(TestANDOp3, "AND operator Test case 3");
  RUN_TEST(TestCurate1, "Curate Keywords Test case 1");
  RUN_TEST(TestCurate2, "Curate Keywords Test case 2");
  RUN_TEST(TestRanking1, "Quicksort Ranking Test case 1");

  if (!cnt) {
    printf("All passed!\n Passed: %d \n", cnt); return 0;
  } else {
    printf("Some fails!\n Passed: %d \n", cnt); return 1;
  }
}


