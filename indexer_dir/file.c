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
excluding '.' or '..' or '.git' (only numbers)

*/

int dirScan(char *dir) {
  struct dirent **namelist;
  struct stat checkFile;

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
      if ( (checkNum = strtol(namelist[n]->d_name, NULL, 10)) && 
        stat( namelist[n]->d_name, &checkFile ) != 0){

        // is a valid file (number and file)
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
                           
