/*

FILE: index.c

Description: 

Inputs: ./indexer [TARGET DIRECTORY WHERE TO FIND THE DATA] [RESULTS FILE NAME]
 OR (for debugging / to test reloading)
  ./indexer [TARGET_DIRECTORY] [RESULTS FILENAME] [RESULTS FILENAME] [REWRITTEN FILENAME]

Outputs: For each file in the target directory, the indexer will check
all the words in each file and count their occurrences. This will
written to the result file in the format of:

  [word] [number of files containing the word] [document ID] [how many
  occurrences in document ID] etc.

 */

// This function initializes the reloaded index structure that will be used
// to "reload" the index via reading the index.dat and writing output to
// an index_new.dat. We do this to make sure that the index file can be 
// properly retrieved
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

