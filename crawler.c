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

int fileCounter = 0; // counter for the html files scraped

/*

(5) *Crawler*
-------------



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
  int writableResult = 0;
  char testURL[MAX_URL_LENGTH + 10]; // max URL length and space for the "wget -q"
  char *writableTest = NULL; // dynamically allocate to prevent overflow

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
  // Allocate memory to prevent overflow
  size_t len1 = strlen("if [ -w "), len2 = strlen(argv[2]), len3 = strlen(" ] ; then exit 0 ; else exit 1 ; fi");
  writableTest = (char*) malloc(len1 + len2 + len3 + 1);

  memcpy(writableTest, "if [ -w ", len1);
  memcpy(writableTest+len1, argv[2], len2);
  memcpy(writableTest+len2, " ] ; then exit 0 ; else exit 1 ; fi", len3+1);

  writableResult = system(writableTest);

  if ( writableResult != 0){
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
}

int initLists(){
  /*char *url_list[MAX_URL_PER_PAGE];*/

  /*url_list[i] = malloc(MAX_URL_LENGTH);*/
  /*MALLOC_CHECK(url_list[i]);*/
  /*BZERO(url_list[i], MAX_URL_LENGTH);*/
  /*strncpy(url_list[i], url, MAX_URL_LENGTH);*/

  // malloc the dictionary and initialize with null for 
  // the MAX_HASH_SLOT amount
  dict = (DICTIONARY*)malloc(sizeof(DICTIONARY));
  MALLOC_CHECK(dict);
  BZERO(dict, sizeof(DICTIONARY)); // set the bytes to zero
  dict->start = dict->end = NULL;  // make explicit
  
  // initialize
  /*for (int i=0; i < MAX_HASH_SLOT; i++){*/
    /*dict->hash[i] = NULL;*/
  /*}*/

  return(1);
}

char* getPage(char* url, int current_depth, char* target_directory){
  char* wgetCmd;
  char* fileBuffer = NULL;
  int wgetResult;
  int i = 0; 
  int tempFileSize= 0;
  FILE* tempStore;
  FILE* fileSave;
  size_t readResult;
  char dirWithCounter[MAX_URL_LENGTH + 100];

  printf("\n [Crawler] Crawling %s \n", url);

  // allocate space to avoid overflow
  size_t len1 = strlen("wget -O temp "), len2 = strlen(url);
  wgetCmd = (char*) malloc(len1 + len2 + 1);

  memcpy(wgetCmd, "wget -O temp ", len1);
  memcpy(wgetCmd + len1, url, len2);

  wgetResult = system(wgetCmd);

  // check if wget was successful
  do {
    if (wgetResult != 0) {
      // if not successful, try 2 more times before giving up
      fprintf(stderr, "\n Did not successfully crawl page on try %d", i);

      if (i == 2){
        fprintf(stderr, "\n Retrieval failed after %d tries. Next URL \n", i);
        return NULL;
      }
      i++;
    } else {
      // successful, exit loop and proceed
      break;
    }
  } while (i < 3);

  // Open the temp stored file from the successful wget
  tempStore = fopen("temp", "r");

  // Make sure temp exists first
  if (tempStore == NULL){
    fprintf(stderr, "Could not open the file from the URL %s. Skipping...\n", url);
    return NULL;
  }

  if (fseek(tempStore, 0L, SEEK_END) == 0){
    long tempFileSize = ftell(tempStore);
    if (tempFileSize == -1){
      fprintf(stderr, "Error: file size not valid \n");
      exit(1);
    }
    
    // Allocate buffer to that size
    fileBuffer = malloc(sizeof(char) * (tempFileSize + 1));

    rewind(tempStore);
    /*if (fseek(tempStore, 0L, SEEK_SET) == 0){*/
      /*fprintf(stderr, "Error moving back to beginning of file \n");*/
      /*exit(1);*/
    /*}*/

    // Read file to memory
    readResult = fread(fileBuffer, sizeof(char), tempFileSize, tempStore);
    if (readResult == 0){
      fprintf(stderr, "Error reading the temp file. Skipping. \n");
      return NULL;
    } else {
      fileBuffer[++readResult] = '\0'; 
    }
  }

  fclose(tempStore);


  // increment the file counter for writing
  fileCounter++;

  sprintf(dirWithCounter, "%s/%d", target_directory, fileCounter);
  fileSave = fopen(dirWithCounter, "w");

  if (fileSave == NULL){
    fprintf(stderr, "Error writing temp file to target directory. Aborting \n");
    exit(1);
  }

  // Commit the buffer to file
  fprintf(fileSave, "%s\n%d\n%s", url, current_depth, fileBuffer);
  fclose(fileSave);

  // return the pointer to the buffer
  return fileBuffer;
}

int main(int argc, char* argv[]) {
  int current_depth;
  char* target_directory;
  char* seedURL;
  char* page;

  // Input command processing logic
  //(1) Command line processing on arguments
  // If there are incorrect arguments, the program will exit
  validateArgs(argc, argv);

  // Initialization of any data structures
  //(2) *initLists* Initialize any data structure and variables
  if (initLists() != 1){
    // Creation of dictionary failed
    printf("Initialization failed! Cannot Continue \n");
    exit(1);
  }

  // Bootstrap part of Crawler for first time through with SEED_URL
  seedURL = argv[1];
  current_depth = 0;
  target_directory = argv[2];
  
  // Get HTML into a string and return as page, 
  page = getPage(seedURL, current_depth, target_directory);
            /*also save a file (1..N) with correct format (URL, depth, HTML) */

  printf("*******************PAGE GOTTEN page gotten*******");
  if (page == NULL){
    printf("Cannot crawl seed URL: %s. Aborting \n", seedURL);
    exit(1);
  }

  // extract the URLs from the page


  return 0;
}
