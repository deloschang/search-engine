README FILE
================

The indexer will iterate through all the files (directories filtered
out)in the target directory that are positive non-zero ints (from the crawler). So:

FILES:
 - 1 --> OK
 - .. --> NOT ITERATED
 - .git --> NOT ITERATED

