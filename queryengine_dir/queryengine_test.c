// Filename: Test cases for dictionary.h/.c
// Description: A unit test for query engine processing
// 

//
// Test Harness Spec:
// ------------------
//
//  It uses these files but they are not unit tested in this test harness:
//
//  DICTIONARY* InitDictionary();
//  int make_hash(char* c);
//  void CleanDictionary(DICTIONARY* dict)
// 
//  It tests the following functions:
//
//   void DAdd(DICTIONARY* dict, void* data, char* key);
//   void DRemove(DICTIONARY* dict, char* key);
//   void* GetDataWithKey(DICTIONARY* dict, char* key);
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
//  The test harness dummies out the real hash function and through
//  use of a variable called hash manipulates where DNODEs are 
//  inserted into the DICTIONARY. For example, collisions can be controlled. 
//
//  The following test cases  (1-3) are for function:
//
//  void DAdd(DICTIONARY* dict, void* data, char* key);
//
//  Test case: DADD:1
//  This test case calls DAdd() for the condition where dict is empty
//  result is to add a DNODE to the dictionary and look at its values.
//
//  Test case: DADD:2
//  This test case calls DAdd() puts multiple DNODEs on the dict when there is no hash collisions
//  We put multiply elements in dictionary with no collisions.
//
//  Test case: DADD:3
//  This test case calls DAdd() puts multiple DNODEs on the dict when there is hash collisions
//  We put multiply elements in dictionary with collisions.
//
//  The following test cases (1-4) for function:
//
//  void DRemove(DICTIONARY* dict, char* key);
//
//  Test case:DREMOVE:1
//  This test case DAdd() and DRemove()  DNODE from dict for only one element.
//
//  Test case:DREMOVE:2
//  This test case is tries to see how DRemove() works with multiple nodes for the same 
//  hash value, the node to be deleted is at the end of the dynamic list.
//
//  Test case:DREMOVE:3
//  This test case is tries to see how DRemove() works with multiple nodes of the same hash value, 
//  the node to be deleted is at the start of the dynamic list.
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
#include "dictionary.h"

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


// Test case: DADD:1
// This test case calls DAdd() for the condition where dict is empty
// result is to add a DNODE to the dictionary and look at its values.

int TestDAdd1() {
  START_TEST_CASE;
  DICTIONARY* dict = InitDictionary();
  int* d1; d1 = (int *)malloc(sizeof(int)); *d1 = 1;
  hash = 0;
  DAdd(dict, d1, "1");
  SHOULD_BE(dict->hash[0]->data == (void*)d1);
  SHOULD_BE(dict->start == dict->hash[0]);
  SHOULD_BE(dict->end == dict->hash[0]);
  SHOULD_BE(dict->hash[0]->prev == NULL);
  SHOULD_BE(dict->hash[0]->next == NULL);
  SHOULD_BE(!strcmp(dict->hash[0]->key, "1"));
  CleanDictionary(dict);
  free(dict);
  END_TEST_CASE;
}

// Test case: DADD:2
// This test case calls DAdd() puts multiple DNODEs on the dict when there is no hash collisions
// We put multiply elements in dictionary with no collisions.

int TestDAdd2() {
  START_TEST_CASE;
  DICTIONARY* dict = InitDictionary();
  int *d1; d1 = (int *)malloc(sizeof(int)); *d1 = 1;
  int *d2; d2 = (int *)malloc(sizeof(int)); *d2 = 2;
  int *d3; d3 = (int *)malloc(sizeof(int)); *d3 = 3;
  hash = 0;
  DAdd(dict, d1, "1");
  hash = 1;
  DAdd(dict, d2, "2");
  hash = 2;
  DAdd(dict, d3, "3");
  SHOULD_BE(dict->end == dict->hash[2]);
  SHOULD_BE(dict->start == dict->hash[0]);
  SHOULD_BE(dict->hash[0]->data == (void*)d1);
  SHOULD_BE(dict->hash[0]->prev == (void*)NULL);
  SHOULD_BE(dict->hash[0]->next == dict->hash[1]);
  SHOULD_BE(dict->hash[1]->data == (void*)d2);
  SHOULD_BE(dict->hash[1]->prev == dict->hash[0]);
  SHOULD_BE(dict->hash[1]->next == dict->hash[2]);
  SHOULD_BE(dict->hash[2]->data == (void*)d3);
  SHOULD_BE(dict->hash[2]->prev == dict->hash[1]);
  SHOULD_BE(dict->hash[2]->next == (void*)NULL);
  CleanDictionary(dict);
  free(dict);
  END_TEST_CASE;
}


// Test case: DADD:3
// This test case calls DAdd() puts multiple DNODEs on the dict when there is hash collisions
// We put multiply elements in dictionary with collisions.

int TestDAdd3() {
  START_TEST_CASE;
  DICTIONARY* dict = InitDictionary();
  int *d1; d1 = (int *)malloc(sizeof(int)); *d1 = 1;
  int *d2; d2 = (int *)malloc(sizeof(int)); *d2 = 2;
  hash = 0;
  DAdd(dict, d1, "1");
  hash = 0;
  DAdd(dict, d2, "2");
  SHOULD_BE(dict->end == dict->hash[0]->next);
  SHOULD_BE(dict->start == dict->hash[0]);
  SHOULD_BE(dict->hash[0]->data == (void*)d1);
  SHOULD_BE(dict->hash[0]->prev == NULL);
  SHOULD_BE(dict->hash[0]->next->data == (void*)d2);
  SHOULD_BE(dict->hash[0]->next->prev == dict->hash[0]);
  SHOULD_BE(dict->hash[0]->next->next == NULL);  
  CleanDictionary(dict);
  free(dict);
  END_TEST_CASE;
}

// Test case:DREMOVE:1
// This test case DAdd() and DRemove()  DNODE from dict for only one element.
int TestDRemove1() {
  START_TEST_CASE;
  DICTIONARY* dict = InitDictionary();
  int *d1; d1 = (int *)malloc(sizeof(int)); *d1 = 1;
  hash = 0;
  DAdd(dict, d1, "1");
  DRemove(dict, "1");
  SHOULD_BE(dict->hash[0] == NULL);
  SHOULD_BE(dict->start == NULL);
  SHOULD_BE(dict->end == NULL);
  CleanDictionary(dict);
  free(dict);
  END_TEST_CASE;
}


// Test case:DREMOVE:2
// This test case is tries to see how DRemove() works with multiple nodes for the same hash value, the node to be deleted is at the end of the dynamic list.

int TestDRemove2() {
  START_TEST_CASE;
  DICTIONARY* dict = InitDictionary();
  int *d1; d1 = (int *)malloc(sizeof(int)); *d1 = 1;
  int *d2; d2 = (int *)malloc(sizeof(int)); *d1 = 2;
  hash = 0;
  DAdd(dict, d1, "1");
  DAdd(dict, d2, "2");
  DRemove(dict, "2");
  SHOULD_BE(dict->hash[0]->data == (void*)d1);
  SHOULD_BE(dict->start == dict->hash[0]);
  SHOULD_BE(dict->end == dict->hash[0]);
  SHOULD_BE(dict->hash[0]->prev == NULL);
  SHOULD_BE(dict->hash[0]->next == NULL);
  CleanDictionary(dict);
  free(dict);
  END_TEST_CASE;
}


// Test case:DREMOVE:3
// This test case is tries to see how DRemove() works with multiple nodes of the same hash value, the node to be deleted is at the start of the dynamic list

int TestDRemove3() {
  START_TEST_CASE;
  DICTIONARY* dict = InitDictionary();
  int *d1; d1 = (int *)malloc(sizeof(int)); *d1 = 1;
  int *d2; d2 = (int *)malloc(sizeof(int)); *d2 = 2;
  hash = 0;
  DAdd(dict, d1, "1");
  DAdd(dict, d2, "2");
  DRemove(dict, "1");
  SHOULD_BE(dict->hash[0]->data == (void*)d2);
  SHOULD_BE(dict->start == dict->hash[0]);
  SHOULD_BE(dict->end == dict->hash[0]);
  SHOULD_BE(dict->hash[0]->prev == NULL);
  SHOULD_BE(dict->hash[0]->next == NULL);
  CleanDictionary(dict);
  free(dict);
  END_TEST_CASE;
}


// Test case:DREMOVE:4
// This test case is tries to see how DRemove() works with multiple nodes of the same hash value, the node to be deleted is at the middle of the dynamic list
int TestDRemove4() {
  START_TEST_CASE;
  DICTIONARY* dict = InitDictionary();
  int *d1; d1 = (int*)malloc(sizeof(int)); *d1 = 1;
  int *d2; d2 = (int*)malloc(sizeof(int)); *d2 = 2;
  int *d3; d3 = (int*)malloc(sizeof(int)); *d3 = 3;
  hash = 0;
  DAdd(dict, d1, "1");
  hash = 1;
  DAdd(dict, d2, "2");
  DAdd(dict, d3, "3");
  DRemove(dict, "2");
  SHOULD_BE(dict->start == dict->hash[0]);
  SHOULD_BE(dict->end == dict->hash[1]);
  SHOULD_BE(dict->hash[0]->data == (void*)d1);
  SHOULD_BE(dict->hash[1]->data == (void*)d3);
  SHOULD_BE(dict->hash[1]->prev == dict->hash[0]);
  SHOULD_BE(dict->hash[1]->next == NULL);
  CleanDictionary(dict);
  free(dict);
  END_TEST_CASE;
}

// Test case:GetDataWithKey:1
// This test case tests GetDataWithKey - to get a data with the a certain key.
int TestGetData1() {
  START_TEST_CASE;
  DICTIONARY* dict = InitDictionary();
  int *d1; d1 = (int*)malloc(sizeof(int)); *d1 = 1;
  int *d2; d2 = (int*)malloc(sizeof(int)); *d2 = 2;
  int *d3; d3 = (int*)malloc(sizeof(int)); *d3 = 3;
  hash = 0;
  DAdd(dict, d1, "1");
  hash = 1;
  DAdd(dict, d2, "2");
  DAdd(dict, d3, "3");
  hash = 0;
  SHOULD_BE(GetDataWithKey(dict, "1") == (void*)d1);
  hash = 1;
  SHOULD_BE(GetDataWithKey(dict, "2") == (void*)d2);
  SHOULD_BE(GetDataWithKey(dict, "3") == (void*)d3);
  // case that there is no such key in the dictionary
  SHOULD_BE(GetDataWithKey(dict, "") == NULL);
  SHOULD_BE(GetDataWithKey(dict, "4") == NULL);
  CleanDictionary(dict);
  free(dict);
  END_TEST_CASE;
}

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

  RUN_TEST(TestDAdd1, "DAdd Test case 1");
  RUN_TEST(TestDAdd2, "DAdd Test case 2");
  RUN_TEST(TestDAdd3, "DAdd Test case 3");
  RUN_TEST(TestDRemove1, "DRemove Test case 1");
  RUN_TEST(TestDRemove2, "DRemove Test case 2");
  RUN_TEST(TestDRemove3, "DRemove Test case 3");
  RUN_TEST(TestDRemove4, "DRemove Test case 4");
  RUN_TEST(TestGetData1, "GetDataWithKey Test case 1");
  if (!cnt) {
    printf("All passed!\n"); return 0;
  } else {
    printf("Some fails!\n"); return 1;
  }
}


