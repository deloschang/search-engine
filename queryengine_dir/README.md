README FILE
================

-- the make file will run BATS.sh which tests the parameters + the diff
(for debug mode)

0. Entering in 5 parameters enters DEBUG mode which will (1) index the
   files normally and (2) reload it back into memory and output it again 
   
1. The indexer will iterate through all the files (directories filtered
out) in the target directory that are positive non-zero ints (from the crawler). So:

FILES:
 - 1 --> OK
 - .. --> NOT ITERATED
 - .git --> NOT ITERATED

The assumption that the files will be numbered (e.g. 1, 2, 3) and used
as documentID is practical based on the crawler.

2. If the indexer is loading a document but is unable to find it based
   on the filepath even though scandir() has already found it,
   the program will spit an error message and stop.

3. The indexer will overwrite the target destination file (though it
   gives a warning)

4. To cleanly parse the HTML words, the indexer will do the following from the buffer: 
  a. filter any word that is less than 3 characters
  b. convert uppercase letters to lowercase letters
  c. remove commas, apostrophes, quotes, majority of non-alpha
  characters (preserving the '<' and '>'
  d. filtering all ASCII characters below and including 13.
  -- The result is that HTML words are scraped without much of the 
  "junk" that comes with it (like newlines).
  
5. The reload file for the 5 parameter DEBUG mode will overwrite files
   (with warning)
