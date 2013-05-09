/*

FILE: queryengine.c

Description: an indexer that creates an inverted list of words
and their occurrences amongst different documents. This inverted index
will be constructed into a file and reloaded to the query engine.

Inputs: ./queryengine [TARGET DIRECTORY WHERE TO FIND THE DATA] [RESULTS FILE NAME]
 OR (for debugging / to test reloading)
  ./indexer [TARGET_DIRECTORY] [RESULTS FILENAME] [RESULTS FILENAME] [REWRITTEN FILENAME]

Outputs: For each file in the target directory, the indexer will check
all the words in each file and count their occurrences. This will
written to the result file in the format of:


 */

#include <stdio.h>
#include <stdlib.h>
/*#include <string.h>*/

/*#include <sys/types.h>*/
/*#include <sys/stat.h>*/
/*#include <unistd.h>*/
/*#include <ctype.h>*/

/*#include "indexer.h"*/
/*#include "hash.h"*/
/*#include "file.h"*/


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

  // DEBUG MODE: reloading the index file
  if ( argc == 5){
    validateDebugArgs(argv[3], argv[4]);
    LOG("Testing index");

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
    LOG("Cleaning up");
    cleanupIndex(indexReload);

    // BATS.sh will test the integrity of the reloaded index.
  }
  return 0;
}
