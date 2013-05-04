README FILE
================

* Makefile *
  - make cleanlog will clean both log and crawled data files

* Miscellaneous *
  - Temp files are removed after they are created at end of function.
  - Crawler will print out statistics on how many pages it crawled and how many pages it could not crawl
  - If the seed URL cannot be crawled, the program aborts.
  - If non-seed URLs cannot be crawled, they are tried twice (instead of 20 times) and then skipped.

* Debugging *
  - Use make valgrind to check for memory leaks
  - crawler_test.sh is a script that automatically tests different basic parameters (sanity check)
