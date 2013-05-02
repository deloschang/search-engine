/*

  FILE: crawler.c

  Description:

  Inputs: ./crawler [SEED URL] [TARGET DIRECTORY WHERE TO PUT THE DATA] [MAX CRAWLING DEPTH]

  Outputs: For each webpage crawled the crawler program will create a file in the 
  [TARGET DIRECTORY]. The name of the file will start a 1 for the  [SEED URL] 
  and be incremented for each subsequent HTML webpage crawled. 

  Each file (e.g., 10) will include the URL associated with the saved webpage and the
  depth of search in the file. The URL will be on the first line of the file 
  and the depth on the second line. The HTML will for the webpage 
  will start on the third line.

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "crawler.h"
#include "html.h"
#include "hash.h"
#include "header.h"


// Define the dict structure that holds the hash table 
// and the double linked list of DNODES. Each DNODE holds
// a pointer to a URLNODE. This list is used to store
// unique URLs. The search time for this list is O(n).
// To speed that up to O(1) we use the hash table. The
// hash table holds pointers into the list where 
// DNODES with the same key are maintained, assuming
// the hash(key) is not NULL (which implies the URL has
// not been seen before). The hash table provide quick
// access to the point in the list that is relevant
// to the current URL search. 

DICTIONARY* dict = NULL; 


// This is the table that keeps pointers to a list of URL extracted
// from the current HTML page. NULL pointer represents the end of the
// list of URLs.

char *url_list[MAX_URL_PER_PAGE]; 

/*


(5) *Crawler*
-------------


// Initialization of any data structures

(2) *initLists* Initialize any data structure and variables

// Bootstrap part of Crawler for first time through with SEED_URL

(3) page = *getPage(seedURL, current_depth, target_directory)* Get HTML into a string and return as page, 
            also save a file (1..N) with correct format (URL, depth, HTML) 
    IF page == NULL THEN
       *log(PANIC: Cannot crawl SEED_URL)* Inform user
       exit failed
(4) URLList = *extractURLs(page, SEED_URL)* Extract all URLs from SEED_URL page.
  
(5) *free(page)* Done with the page so release it

(6) *updateListLinkToBeVisited(URLList, current_depth + 1)*  For all the URL 
    in the URLList that do not exist already in the dictionary then add a DNODE/URLNODE 
    pair to the DNODE list. 

(7) *setURLasVisited(SEED_URL)* Mark the current URL visited in the URLNODE.

// Main processing loop of crawler. While there are URL to visit and the depth is not 
// exceeded keep processing the URLs.

(8) WHILE ( URLToBeVisited = *getAddressFromTheLinksToBeVisited(current_depth)* ) DO
        // Get the next URL to be visited from the DNODE list (first one not visited from start)
 
      IF current_depth > max_depth THEN
    
          // For URLs that are over max_depth, we just set them to visited
          // and continue on
    
          setURLasVisited(URLToBeVisited) Mark the current URL visited in the URLNODE.
          continue;

    page = *getPage(URLToBeVisited, current_depth, target_directory)* Get HTML into a 
            string and return as page, also save a file (1..N) with correct format (URL, depth, HTML) 

    IF page == NULL THEN
       *log(PANIC: Cannot crawl URLToBeVisited)* Inform user
       setURLasVisited(URLToBeVisited) Mark the bad URL as visited in the URLNODE.
       Continue; // We don't want the bad URL to stop us processing the remaining URLs.
   
    URLList = *extractURLs(page, URLToBeVisited)* Extract all URLs from current page.
  
    *free(page)* Done with the page so release it

    *updateListLinkToBeVisited(URLList, current_depth + 1)* For all the URL 
    in the URLList that do not exist already in the dictionary then add a DNODE/URLNODE 
    pair to the DNODE list. 

    *setURLasVisited(URLToBeVisited)* Mark the current URL visited in the URLNODE.

    // You must include a sleep delay before crawling the next page 
    // See note below for reason.

    *sleep(INTERVAL_PER_FETCH)* Sneak by the server by sleeping. Use the 
     standard Linux system call

(9)  *log(Nothing more to crawl)

(10) *cleanup* Clean up data structures and make sure all files are closed,
      resources deallocated.

*/


// This function validates the arguments provided by the user.
// If there is an incorrect input, it will print an error message,
// followed by a usage message (so the user can correct his/her input)
void validateArgs(int argc, char* argv[]){
  // struct for checking whether directory exists
  struct stat s;
  // test URL with default correct exit status
  int testResult = 0;
  char testURL[MAX_URL_LENGTH + 10]; // max URL length and space for the "wget -q"

  // check for correct number of parameters first
  if (argc != 4){
    fprintf(stderr, "Error: insufficient arguments. 3 required, you provided %d \n", argc - 1);
    printf("Usage: ./crawler [SEED_URL] [TARGET_DIR WHERE TO PUT DATA] [CRAWLING_DEPTH] \n");

    exit(1);
  }

  // Validate the max depth (cannot exceed 4)
  // Validate depth is single digit 
  if ( (argv[argc - 1][1]) || (argv[argc - 1][0] > '4') 
    || (argv[argc - 1][0] < '0') ) {
    fprintf(stderr, "Error: Depth must be between 0 and 4. You entered %s \n", argv[argc - 1]);
    printf("Usage: ./crawler [SEED_URL] [TARGET_DIR WHERE TO PUT DATA] [CRAWLING_DEPTH] \n");

    exit(1);
  }

  // Validate that directory exists
  if ( stat(argv[2], &s) != 0){
    fprintf(stderr, "Error: The dir argument %s was not found.  Please enter writable and valid directory. \n", argv[2]);
    printf("Usage: ./crawler [SEED_URL] [TARGET_DIR WHERE TO PUT DATA] [CRAWLING_DEPTH] \n");

    exit(1);
  }

  // Validate that directory is writable
  if ( stat(argv[2], &s) != 0){
    fprintf(stderr, "Error: The dir argument %s was not writable.  Please enter writable and valid directory. \n", argv[2]);
    printf("Usage: ./crawler [SEED_URL] [TARGET_DIR WHERE TO PUT DATA] [CRAWLING_DEPTH] \n");

    exit(1);
  }

  // Validate that URL exists
  strcpy(testURL, "wget -qt2 " );
  strcat(testURL, argv[1]); // add the URL to the wget command
  testResult = system(testURL);

  // an exit status of not 0 means there was an error
  if (testResult != 0){
    fprintf(stderr, "Error: The URL %s was invalid. Please enter a valid URL. \n", argv[1]);
    printf("Usage: ./crawler [SEED_URL] [TARGET_DIR WHERE TO PUT DATA] [CRAWLING_DEPTH] \n");

    exit(1);
  }

  // inputs are correct
  printf("Inputs correct.\n");

}

int main(int argc, char* argv[]) {

  // Input command processing logic
  //(1) Command line processing on arguments
  validateArgs(argc, argv);

  return 0;
}
