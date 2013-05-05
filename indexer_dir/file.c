// FILE: file.c
//
// These are utility functions that deal with files
//
// Directory scanner utility implementation

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

/****

*dirScan*
------------

uses scandir function to retrieve the number of files

excluding '.' or '..' or '.git' (only numbers) and non-files (e.g. directories)

*/

int dirScan(char *dir) {
  struct dirent **namelist;

  int n;
  int saveN;

  int checkNum;
  int notNum = 0;

  n = scandir(dir, &namelist, 0, alphasort);

  // Check for error condition
  if (n < 0){
    fprintf(stderr, "Unable to scan directory %s for files. Exiting.  \n", dir);
    exit(1);
  } else if (n ==0){
    // Check for no files
    fprintf(stderr, "No files within directory %s. Exiting. \n", dir);
    exit(1);
  } else {

    saveN = n;
    while(n--) {
      // filter only for numbers and files
      if ( (checkNum = strtol(namelist[n]->d_name, NULL, 10)) ){
        struct stat st;

        // Construct the filepath
        char* readableTest;

        size_t string1 = strlen(dir); 
        size_t string2 = strlen("/");
        size_t string3 = strlen(namelist[n]->d_name);

        // Allocate space for teh filepath
        readableTest = (char*) malloc(string1 + string2 + string3 + 1);
        sprintf(readableTest, "%s/%s", dir, namelist[n]->d_name);

        stat( readableTest, &st);
        free(readableTest);

        ////// FILTER FOR FILES ONLY ////////
        if (S_ISDIR(st.st_mode)){
          notNum++;
        }

        continue;

      } else {
        notNum++;
      }
      free(namelist[n]);
    }

    free(namelist);

    // Build index from the directory
    return (saveN - notNum);
  }
}
                           
