#!/bin/bash
# Name: BATS_TSE.sh
#
# Description: This script will build, test and run the search engine
#
# Input:  None.
# Output: Built search engine with command line processing ready
#
# Command Line Options: None.

# Pseudocode: It uses the makefile to build the search engine.
# Then it runs unit tests to test the search engine.
# Finally, it runs the search engine and allows for command-line
# processing

# Run script crawler_test.sh to test and run the crawler

# Next, run BATS.sh to build, test and run the indexer.

# Build the search engine
cd ./queryengine_dir/
make

# Run query engine unit tests

# Launch the query engine
./queryengine ../indexer_dir/index.dat ../data

#INDEX_FILE="index.dat"
#NEW_INDEX_FILE="index_new.dat"

#indexer_testlog="indexer_testlog.`date`"

#echo ""
#echo '............. TESTING INDEXER ..............'
#echo ""

#j=0


#### TEST FORMAT ###
## Test Name: title of the test
## Test Expected: expected output of the test 
## Test Command:  actual command of the test


## test insufficient arguments
#testName[j]="$j. testing insufficient arguments (0)"
#testExpected[j]="Expected Error: insufficient arguments. 3 required (normal) or 5 required (testing), you provided 0."
#testCmd[j]="./indexer"
#let j++

### test sufficient args but invalid directory (doesn't exist)
#testName[j]="$j. testing invalid directory"
#testExpected[j]="Expected Error: The dir argument ./nonexistent/ was not found.  Please enter readable and valid directory."
#testCmd[j]="./indexer ./nonexistent/ index.dat"
#let j++

### test sufficient args but directory not readable
#testName[j]="$j. testing unreadable directory"
#testExpected[j]="Expected Error: The dir argument ./unreadable/ was not
#readable.  Please enter readable and valid directory."
#testCmd[j]="./indexer ./unreadable/ index.dat"
#let j++

## correct input for 3 parameters
#testName[j]="$j. testing correct input arguments for filename index.dat (3 parameters)"
#testExpected[j]="No errors expected."
#testCmd[j]="./indexer ../crawler_dir/data/ index.dat"
#let j++

## correct input for 5 parameters
#testName[j]="$j. testing correct input arguments for filename index.dat (5 parameters)"
#testExpected[j]="No errors expected (or warning that file will be overwritten)"
#testCmd[j]="./indexer ../crawler_dir/data/ $INDEX_FILE $INDEX_FILE $NEW_INDEX_FILE"
#let j++


#iterate=0
#while (($iterate < $j)); do

  #echo ""

  ## test to STDOUT for easy viewing
  #echo ${testName[iterate]} 2>&1 | tee -a "$indexer_testlog" 
  #echo ${testExpected[iterate]} 2>&1 | tee -a "$indexer_testlog"
  #${testCmd[iterate]} 2>&1 | tee -a "$indexer_testlog"

  ## checks if the index file and reloaded index file are the same
  #if [[ -e "$INDEX_FILE" && -e "$NEW_INDEX_FILE" ]]; then
    #echo "Indexes have been built, read and rewritten correctly!" | tee -a "$indexer_testlog"

    ## make sure sorting is the same
    #sort -o $INDEX_FILE $INDEX_FILE
    #sort -o $NEW_INDEX_FILE $NEW_INDEX_FILE

    #diff $INDEX_FILE $NEW_INDEX_FILE

    ## check the integrity of the files
    #if [ $? -eq 0 ]; then
      #echo "Index storage passed test!" | tee -a "$indexer_testlog"
    #else
      #echo "Index storage didn't pass test!" | tee -a "$indexer_testlog"
    #fi

    #debugFlag=0;
  #else
    #echo "Index files $INDEX_FILE and $NEW_INDEX_FILE do not exist. Not testing."
  #fi

  ## increment and test next
  #let iterate++
#done

#echo ""
#echo "Done testing. Cleaning up"

# cleanup
make clean

exit 0
