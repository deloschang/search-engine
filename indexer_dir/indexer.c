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


int main(int argc, char* argv[]){

  printf("Hello world.");

  return 0;
}
