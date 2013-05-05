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
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


// this function prints generic usage information 
void printUsage(){
    printf("Normal Usage: ./indexer [TARGET DIRECTORY] [RESULTS FILENAME]\n");
    printf("Testing Usage: ./indexer [TARGET DIRECTORY] [RESULTS FILENAME] [RESULTS FILENAME] [REWRITTEN FILENAME]\n");
}

// this function will validate the arguments passed
void validateArgs(int argc, char* argv[]){
  // struct for checking whether directory exists
  struct stat s;

  // test URL with default correct exit status
  int readableResult = 0;
  char* readableTest = NULL; // dynamically allocate to prevent overflow

  // check for correct number of parameters first
  if ( (argc != 3) && (argc != 5) ){
    fprintf(stderr, "Error: insufficient arguments. 3 required or 5 (testing), you provided %d \n", argc);
    printUsage();

    exit(1);
  }

  // Validate that directory exists
  if ( stat(argv[1], &s) != 0){
    fprintf(stderr, "Error: The dir argument %s was not found.  Please \
    enter a readable and valid directory. \n", argv[1]);
    printUsage();

    exit(1);
  }

  // Validate that directory is readable
  // Allocate memory to prevent overflow
  size_t len1 = strlen("if [[ -r "), len2 = strlen(argv[1]), len3 = strlen(" ]]; then exit 0; else exit 1; fi");
  readableTest = (char*) malloc(len1 + len2 + len3 + 1);

  if (readableTest == NULL){
    fprintf(stderr, "Out of memory! \n ");
    exit(1);
  }

  memcpy(readableTest, "if [[ -r ", len1);
  memcpy(readableTest+len1, argv[1], len2);
  strcat(readableTest, " ]]; then exit 0; else exit 1; fi");

  readableResult = system(readableTest);

  free(readableTest);

  // check the exit status
  if ( readableResult != 0){
    fprintf(stderr, "Error: The dir argument %s was not readable. \
    Please enter readable and valid directory. \n", argv[1]);
    printUsage();

    exit(1);
  }
}

char* loadDocument(char* filepath){
  FILE* fp;
  char* html = NULL;

  fp = fopen(filepath, "r");

  // If unable to find file, skip.
  // Could be problem of skipped files i.e. (1, 3, 4, 5)
  if (fp == NULL){
    fprintf(stderr, "Could not read file %s. Aborting. \n", filepath);
    exit(1);
  }

  // Read from the file and put it in a buffer
  if (fseek(fp, 0L, SEEK_END) == 0){
    long tempFileSize = ftell(fp);
    if (tempFileSize == -1){
      fprintf(stderr, "Error: file size not valid \n");
      exit(1);
    }

    // Allocate buffer to that size
    // Length of buffer should be the same of output of wget + 1
    html = malloc(sizeof(char) * (tempFileSize + 2) ); // +1 for terminal byte

    // Rewind to top in preparation for reading
    rewind(fp);

    // Read file to buffer
    size_t readResult = fread(html, sizeof(char), tempFileSize, fp);

    if (readResult == 0){
      fprintf(stderr, "Error reading the file into buffer. Aborting. \n");
      exit(1);
    } else {
      readResult++;
      html[readResult] = '\0'; // add terminal byte
    }
  }
  
  fclose(fp);
  return html;
}

// Builds an index from the files in the directory
/*void buildIndexFromDir(char* dir, int numOfFiles, INVERTED_INDEX* index){*/
void buildIndexFromDir(char* dir, int numOfFiles){
  char* writable;
  char* loadedDocument;

  // Loop through each of the files 
  printf("%d", numOfFiles);
  for (int i = 1; i < numOfFiles; i++){
    char converted_i[1001];

    // cut off if more than 1000 digits
    snprintf(converted_i, 1000, "%d", i);

    size_t string1 = strlen(dir); 
    size_t string2 = strlen("/");
    size_t string3 = strlen(converted_i);

    writable = (char*) malloc(string1 + string2 + string3 + 1);

    // Construct the filepath
    sprintf(writable, "%s/%s", dir, converted_i);

    // Load the document from the filepath
    loadedDocument = loadDocument(writable);
    free(writable);

    // Loop through and index the words

    // Got the HTML page
    /*printf("%s\n", loadedDocument);*/

    /*documentId = getDocumentId(d);*/
    /*currentPosition = 0;*/
    /*while (currentPosition = getNextWordFromHTMLDocument(loadedDocument, word, position))*/
    /*updateIndex(index, word, documentId);*/

    free(loadedDocument);
  }

  /*saveIndexToFile(index);*/

}


int main(int argc, char* argv[]){
  char* targetDir;
  int numOfFiles;

  // (1) Validate the parameters
  validateArgs(argc, argv);

  // (2) Grab number of files in target dir to loop through
  targetDir = argv[1]; // set the directory
  numOfFiles = dirScan(targetDir); 

  // (3) Loop through files to build index
  /*buildIndexFromDir(targetDir, numOfFiles, index);*/
  buildIndexFromDir(targetDir, numOfFiles);

  return 0;
}
