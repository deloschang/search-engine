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
//   DocumentNode** lookUp(DocumentNode** saved, char** queryList, INVERTED_INDEX* indexReload);
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
//  The following test cases (1-5) for function:
//
//   DocumentNode** lookUp(DocumentNode** saved, char** queryList, INVERTED_INDEX* indexReload);
//
//  Test case: TestLookUp:1
//  This test calls lookUp() for the condition where 
//  the query has an OR operator. 
//
//  Test case: TestLookUp:2
//  This test calls lookUp() for the condition where 
//  the query has an AND operator. 
//  Tests that summed page frequencies returned are correct 
//
//  Test case: TestLookUp:3
//  This test calls lookUp() for the condition where 
//  the query has an IMPLICIT AND operator. 
//  Tests that summed page frequencies returned are correct 
//
//  Test case: TestLookUp:4
//  This test calls lookUp() for the condition where 
//  the query has extraneous AND and OR queries
//
//  Test case: TestLookUp:5
//  This test calls lookUp() for the condition where 
//  the query has both AND and OR
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/hash.h"
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

  int lookUpResult = 0;

  create();

  INVERTED_INDEX* result = NULL;
  result = reloadIndexFromFile("../indexer_dir/index.dat", indexReload);
  SHOULD_BE(result != NULL);
  LOG("Reloading INVERTED INDEX structure");

  char query[1000] = "andrew";
  sanitize(query);

  char* queryList[1000];
  curateWords(queryList, query);
  SHOULD_BE(strcmp(queryList[0], "andrew") == 0);

  sanitizeKeywords(queryList); 
  SHOULD_BE(strcmp(queryList[0], "andrew") == 0);

  DocumentNode* saved[1000];
  BZERO(saved, 1000);
  lookUp(saved, queryList, indexReload);

  lookUpResult = rankAndPrint(saved, "../crawler_dir/data"); 

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

  int k = 0;
  while (resultSlot[k]){
    // freeing the matched DocNodes and putting them in tempHolder
    free(result[resultSlot[k]]);
    k++;
  }

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

  cleanUpQueryList(queryList);
  END_TEST_CASE;
}

// Test case: TestCurate:2
// This test case calls curateWords() for the condition where the query
// are invalid (non-alpha characters).
int TestCurate2() {
  START_TEST_CASE;

  char empty[1000] = "$$";
  sanitize(empty);

  char* list[1000];
  BZERO(list, 1000);
  list[0] = (char*) malloc(sizeof(char) * 1000);
  BZERO(list[0], 1000); 

  curateWords(list, empty);

  SHOULD_BE(strcmp(list[0], "") == 0);

  free(list[0]);
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

// Test case: TestLookUp:1
// This test calls lookUp() for the condition where 
// the query has an OR operator. 
int TestLookUp1() {
  START_TEST_CASE;
  INVERTED_INDEX* testIndex = NULL;

  int wordHash;
  int wordHash2;
  testIndex = initStructure(testIndex);

  wordHash = hash1("dog") % MAX_NUMBER_OF_SLOTS;
  DocumentNode* docNode = NULL;
  docNode = newDocNode(docNode, 15, 1);

  WordNode* wordNode = NULL;
  wordNode = newWordNode(wordNode, docNode, "dog");
  testIndex->hash[wordHash] = wordNode;


  wordHash2 = hash1("cat") % MAX_NUMBER_OF_SLOTS;
  DocumentNode* docNode2 = NULL;
  docNode2 = newDocNode(docNode2, 20, 2);

  WordNode* wordNode2 = NULL;
  wordNode2 = newWordNode(wordNode2, docNode2, "cat");

  testIndex->hash[wordHash2] = wordNode2;

  char query[1000] = "dog OR cat";
  sanitize(query);

  char* temp[1000];
  BZERO(temp, 1000);

  char* queryList[2];
  BZERO(queryList, 2);

  curateWords(queryList, query);
  SHOULD_BE(strcmp(queryList[0],"dog") == 0);
  SHOULD_BE(strcmp(queryList[1],"OR") == 0);
  SHOULD_BE(strcmp(queryList[2],"cat") == 0);

  DocumentNode* saved[1000];
  BZERO(saved, 1000);
  lookUp(saved, queryList, testIndex);

  SHOULD_BE(saved[0]->document_id == docNode->document_id);
  SHOULD_BE(saved[0]->page_word_frequency == docNode->page_word_frequency);
  SHOULD_BE(saved[1]->document_id == docNode2->document_id);
  SHOULD_BE(saved[1]->page_word_frequency == docNode2->page_word_frequency);
  
  cleanUpList(saved);
  cleanUpQueryList(queryList);
  BZERO(saved, 1000);

  cleanUpIndex(testIndex);

  END_TEST_CASE;
}

// Test case: TestLookUp:2
// This test calls lookUp() for the condition where 
// the query has an AND operator. 
// Tests that summed page frequencies returned are correct 
int TestLookUp2() {
  START_TEST_CASE;
  INVERTED_INDEX* testIndex = NULL;

  int wordHash;
  int wordHash2;
  testIndex = initStructure(testIndex);

  wordHash = hash1("dog") % MAX_NUMBER_OF_SLOTS;
  DocumentNode* docNode = NULL;
  docNode = newDocNode(docNode, 15, 1);

  WordNode* wordNode = NULL;
  wordNode = newWordNode(wordNode, docNode, "dog");
  testIndex->hash[wordHash] = wordNode;


  wordHash2 = hash1("cat") % MAX_NUMBER_OF_SLOTS;
  DocumentNode* docNode2 = NULL;
  docNode2 = newDocNode(docNode2, 15, 2);

  WordNode* wordNode2 = NULL;
  wordNode2 = newWordNode(wordNode2, docNode2, "cat");

  testIndex->hash[wordHash2] = wordNode2;

  char query[1000] = "dog AND cat";
  sanitize(query);

  char* temp[1000];
  BZERO(temp, 1000);

  char* queryList[2];
  BZERO(queryList, 2);

  curateWords(queryList, query);
  SHOULD_BE(strcmp(queryList[0],"dog") == 0);
  SHOULD_BE(strcmp(queryList[1],"AND") == 0);
  SHOULD_BE(strcmp(queryList[2],"cat") == 0);

  DocumentNode* saved[1000];
  BZERO(saved, 1000);
  lookUp(saved, queryList, testIndex);

  SHOULD_BE(saved[0]->document_id == docNode->document_id);
  SHOULD_BE(saved[0]->page_word_frequency == 3);
  
  cleanUpList(saved);
  cleanUpQueryList(queryList);
  BZERO(saved, 1000);

  cleanUpIndex(testIndex);

  END_TEST_CASE;
}

// Test case: TestLookUp:3
// This test calls lookUp() for the condition where 
// the query has an IMPLICIT AND operator. 
// Tests that summed page frequencies returned are correct 
int TestLookUp3() {
  START_TEST_CASE;
  INVERTED_INDEX* testIndex = NULL;

  int wordHash;
  int wordHash2;
  testIndex = initStructure(testIndex);

  wordHash = hash1("dog") % MAX_NUMBER_OF_SLOTS;
  DocumentNode* docNode = NULL;
  docNode = newDocNode(docNode, 15, 1);

  WordNode* wordNode = NULL;
  wordNode = newWordNode(wordNode, docNode, "dog");
  testIndex->hash[wordHash] = wordNode;


  wordHash2 = hash1("cat") % MAX_NUMBER_OF_SLOTS;
  DocumentNode* docNode2 = NULL;
  docNode2 = newDocNode(docNode2, 15, 2);

  WordNode* wordNode2 = NULL;
  wordNode2 = newWordNode(wordNode2, docNode2, "cat");

  testIndex->hash[wordHash2] = wordNode2;

  char query[1000] = "dog cat";
  sanitize(query);

  char* temp[1000];
  BZERO(temp, 1000);

  char* queryList[1000];
  BZERO(queryList, 1000);

  curateWords(queryList, query);
  SHOULD_BE(strcmp(queryList[0],"dog") == 0);
  SHOULD_BE(strcmp(queryList[1],"cat") == 0);

  DocumentNode* saved[1000];
  BZERO(saved, 1000);
  lookUp(saved, queryList, testIndex);

  SHOULD_BE(saved[0]->document_id == docNode->document_id);
  SHOULD_BE(saved[0]->page_word_frequency == 3);
  
  cleanUpList(saved);
  cleanUpQueryList(queryList);
  BZERO(saved, 1000);

  cleanUpIndex(testIndex);

  END_TEST_CASE;
}

// Test case: TestLookUp:4
// This test calls lookUp() for the condition where 
// the query has extraneous AND and OR queries
int TestLookUp4() {
  START_TEST_CASE;
  INVERTED_INDEX* testIndex = NULL;

  int wordHash;
  int wordHash2;
  testIndex = initStructure(testIndex);

  wordHash = hash1("dog") % MAX_NUMBER_OF_SLOTS;
  DocumentNode* docNode = NULL;
  docNode = newDocNode(docNode, 15, 1);

  WordNode* wordNode = NULL;
  wordNode = newWordNode(wordNode, docNode, "dog");
  testIndex->hash[wordHash] = wordNode;


  wordHash2 = hash1("cat") % MAX_NUMBER_OF_SLOTS;
  DocumentNode* docNode2 = NULL;
  docNode2 = newDocNode(docNode2, 15, 2);

  WordNode* wordNode2 = NULL;
  wordNode2 = newWordNode(wordNode2, docNode2, "cat");

  testIndex->hash[wordHash2] = wordNode2;

  char query[1000] = "AND OR dog cat AND OR AND";
  sanitize(query);

  char* temp[1000];
  BZERO(temp, 1000);

  char* queryList[1000];
  BZERO(queryList, 1000);

  curateWords(queryList, query);

  DocumentNode* saved[1000];
  BZERO(saved, 1000);
  lookUp(saved, queryList, testIndex);

  SHOULD_BE(saved[0]->document_id == docNode->document_id);
  SHOULD_BE(saved[0]->page_word_frequency == 3);
  
  cleanUpList(saved);
  cleanUpQueryList(queryList);
  BZERO(saved, 1000);

  cleanUpIndex(testIndex);

  END_TEST_CASE;
}

// Test case: TestLookUp:5
// This test calls lookUp() for the condition where 
// the query has both AND and OR
int TestLookUp5() {
  START_TEST_CASE;
  INVERTED_INDEX* testIndex = NULL;

  int wordHash;
  int wordHash2;
  int wordHash3;
  int wordHash4;
  testIndex = initStructure(testIndex);

  wordHash = hash1("dog") % MAX_NUMBER_OF_SLOTS;
  DocumentNode* docNode = NULL;
  docNode = newDocNode(docNode, 15, 1);

  WordNode* wordNode = NULL;
  wordNode = newWordNode(wordNode, docNode, "dog");
  testIndex->hash[wordHash] = wordNode;


  wordHash2 = hash1("cat") % MAX_NUMBER_OF_SLOTS;
  DocumentNode* docNode2 = NULL;
  docNode2 = newDocNode(docNode2, 15, 2);

  WordNode* wordNode2 = NULL;
  wordNode2 = newWordNode(wordNode2, docNode2, "cat");

  testIndex->hash[wordHash2] = wordNode2;

  wordHash3 = hash1("mouse") % MAX_NUMBER_OF_SLOTS;
  DocumentNode* docNode3 = NULL;
  docNode3 = newDocNode(docNode3, 23, 2);

  WordNode* wordNode3 = NULL;
  wordNode3 = newWordNode(wordNode3, docNode3, "mouse");

  testIndex->hash[wordHash3] = wordNode3;

  wordHash4 = hash1("lion") % MAX_NUMBER_OF_SLOTS;
  DocumentNode* docNode4 = NULL;
  docNode4 = newDocNode(docNode4, 23, 2);

  WordNode* wordNode4 = NULL;
  wordNode4 = newWordNode(wordNode4, docNode4, "lion");

  testIndex->hash[wordHash4] = wordNode4;

  char query[1000] = "dog cat OR mouse lion";
  sanitize(query);

  char* temp[1000];
  BZERO(temp, 1000);

  char* queryList[1000];
  BZERO(queryList, 1000);

  curateWords(queryList, query);

  DocumentNode* saved[1000];
  BZERO(saved, 1000);
  lookUp(saved, queryList, testIndex);

  SHOULD_BE(saved[0]->document_id == docNode->document_id);
  SHOULD_BE(saved[0]->page_word_frequency == 3);
  SHOULD_BE(saved[1]->document_id == docNode3->document_id);
  SHOULD_BE(saved[1]->page_word_frequency == 4);
  
  cleanUpList(saved);
  cleanUpQueryList(queryList);
  BZERO(saved, 1000);

  cleanUpIndex(testIndex);

  END_TEST_CASE;
}

// This is the main test harness for the set of query engine functions. It tests all the code
// in querylogic.c:
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
//   DocumentNode** lookUp(DocumentNode** saved, char** queryList, INVERTED_INDEX* indexReload);
//

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
  RUN_TEST(TestLookUp1, "Look Up Test case 1");
  RUN_TEST(TestLookUp2, "Look Up Test case 2");
  RUN_TEST(TestLookUp3, "Look Up Test case 3");
  RUN_TEST(TestLookUp4, "Look Up Test case 4");
  RUN_TEST(TestLookUp5, "Look Up Test case 5");

  if (!cnt) {
    printf("All passed!\n Passed: %d \n", cnt); return 0;
  } else {
    printf("Some fails!\n Passed: %d \n", cnt); return 1;
  }
}


