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

// Input command processing logic

(1) Command line processing on arguments
    Inform the user if arguments are not present
    IF target_directory does not exist OR depth exceeds max_depth THEN
       Inform user of usage and exit failed

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


int main() {

  printf("Get cooking cs50!\n");

  printf("Translate the pseudo code. Data structures are in crawler.h\n");

  printf("Good luck\n");

  return 0;
}
