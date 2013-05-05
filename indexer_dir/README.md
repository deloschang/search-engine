README FILE
================

1. The indexer will iterate through all the files (directories filtered
out)in the target directory that are positive non-zero ints (from the crawler). So:

FILES:
 - 1 --> OK
 - .. --> NOT ITERATED
 - .git --> NOT ITERATED


2. If the indexer is loading a document but is unable to find it based
   on the filepath even though scandir() has already found it,
   the program will spit an error message and stop.
