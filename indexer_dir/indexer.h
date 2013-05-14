#ifndef _INDEXER_H_
#define _INDEXER_H_

// *****************Impementation Spec********************************
// File: indexer.c
// This file contains useful information for implementing the indexer:
// - PROTOTYPES

// function PROTOTYPES used by indexer.c 

// validateArgs: validates all the arguments are valid (e.g. directory exists)
void validateArgs(int argc, char* argv[]);

// validateDebugArgs: validates all the arguments for debugging are valid (e.g. file exists)
void validateDebugArgs(char* loadFile, char* writeReload);

// loadDocument: loads the file into a buffer. This allows the indexer to go through
// each of the files and store all the HTML in memory using malloc. This will be
// sanitized and processed later by the indexer
//char* loadDocument(char* filepath);


// grabNextWordFromHTMLDocument: uses the loadedDocument buffer to parse for the words
// it will retrieve everything between a set of tags, including javascript. The word
// retrieved from this function is passed to updateIndex to be updated
int getNextWordFromHTMLDocument(char* loadedDocument, char* word, int position, 
INVERTED_INDEX* index, int documentId);

// updateIndex: given a word and document ID, this function will hash the word and 
// if there are other WordNodes in that slot, it will check if the WordNode for this 
// word exists already. If not, it goes through the list and checks. If none exists,
// it will insert the WordNode to the end of the list and update the index.
int updateIndex(INVERTED_INDEX* index, char* word, int documentId);

// sanitize: this will sanitize a buffer, stripping everything that should not be
// parsed into words: e.g. -- newline characters, @, & etc. When choosing what to 
// sanitize, there is a trade off between obfuscating possible words that could be
// using them. 
//void sanitize(char* loadedDocument);

// buildIndexFromDir:  scans through the target directory. It iterates through 
// each file in the directory and uses the getNextWordFromHTMLDocument to 
// parse it and then subsequently update the index
void buildIndexFromDir(char* dir, int numOfFiles, INVERTED_INDEX* index);

// initStructure: This function initializes the primary index used to read the HTML files 
// It will hold the hash list which holds the WordNodes which hold the 
// Document Nodes
//int initStructures();

// initReloadStructure: This function initializes the reloaded index structure that will be used
// to "reload" the index via reading the index.dat and writing output to
// an index_new.dat. We do this to make sure that the index file can be 
// properly retrieved
//int initReloadStructure();

// saveIndexToFile: this function will save the index in memory to a file
// by going through each WordNode and Document Node, parsing them 
// and writing them in the specified format
// cat 2 2 3 4 5 
// the first 2 indicates that there are 2 documents with 'cat' found
// the second 2 indicates the document ID with 3 occurrences of 'cat'
// the 4 indicates the document ID with 5 occurrences of 'cat'
//int saveIndexToFile(INVERTED_INDEX* index, char* targetFile);

// cleanupIndex: Cleans up the index by freeing the wordnode, documentnode
// and entire index
//void cleanupIndex(INVERTED_INDEX* index);

// reconstructIndex: this function will reconstruct the entire index with a
// word, document ID and page frequency passed to it. It is used in 
// debug mode to ensure that the index can be "reloaded" 
//int reconstructIndex(char* word, int documentId, int page_word_frequency);

// reloadIndexFromFile: This function does the heavy lifting of 
// "reloading" a file into an index in memory. It goes through
// each of the characters and uses strtok to split by space
//int reloadIndexFromFile(char* loadFile, char* writeReload);
#endif
