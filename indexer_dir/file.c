// FILE: file.c
//
// These are utility functions that deal with files
//
// Directory scanner utility implementation

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dirent.h>

/****

*dirScan*
------------

uses scandir function to retrieve the number of files
excluding '.' or '..' or '.git' (only numbers)

*/

int dirScan(char *dir) {
  struct dirent **namelist;
  int n;

  n = scandir(dir, &namelist, 0, alphasort);
  if (n < 0){
    perror("scandir");
    return NULL;
  } else {
    /*while(n--) {*/
      /*printf("%s\n", namelist[n]->d_name);*/
      /*free(namelist[n]);*/
    /*}*/
    free(namelist);

    return n;
  }
}
                           
