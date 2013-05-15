#!/bin/bash
# Name: BATS.sh
#
# Description: This script will test the indexer that is generated from the 
# Makefile. Also, this script is automatically run with the default make 
# as a sanity check.

# Input:  None.
# Output: Result of the different battery tests of parameters
# and the expectant results

# Command Line Options: None.

# Pseudocode: It runs through a list of incorrect parameters 
# with an expectant result

INDEX_FILE="index.dat"
NEW_INDEX_FILE="index_new.dat"

indexer_testlog="indexer_testlog.`date`"

echo ""
echo '............. TESTING INDEXER ..............'
echo ""

j=0


### TEST FORMAT ###
# Test Name: title of the test
# Test Expected: expected output of the test 
# Test Command:  actual command of the test


# test insufficient arguments
testName[j]="$j. testing insufficient arguments (0)"
testExpected[j]="Expected Error: insufficient arguments. 3 required (normal) or 5 required (testing), you provided 0."
testCmd[j]="./indexer"
let j++

## test sufficient args but invalid directory (doesn't exist)
testName[j]="$j. testing invalid directory"
testExpected[j]="Expected Error: The dir argument ./nonexistent/ was not found.  Please enter readable and valid directory."
testCmd[j]="./indexer ./nonexistent/ index.dat"
let j++

## test sufficient args but directory not readable
testName[j]="$j. testing unreadable directory"
testExpected[j]="Expected Error: The dir argument ./unreadable/ was not
readable.  Please enter readable and valid directory."
testCmd[j]="./indexer ./unreadable/ index.dat"
let j++

# correct input for 3 parameters
#testName[j]="$j. testing correct input arguments for filename index.dat (3 parameters)"
#testExpected[j]="No errors expected."
#testCmd[j]="./indexer ../crawler_dir/data/ index.dat"
#let j++

# correct input for 5 parameters
#testName[j]="$j. testing correct input arguments for filename index.dat (5 parameters)"
#testExpected[j]="No errors expected (or warning that file will be overwritten)"
#testCmd[j]="./indexer ../crawler_dir/data/ $INDEX_FILE $INDEX_FILE $NEW_INDEX_FILE"
#let j++


iterate=0
while (($iterate < $j)); do

  echo ""

  # test to STDOUT for easy viewing
  echo ${testName[iterate]} 2>&1 | tee -a "$indexer_testlog" 
  echo ${testExpected[iterate]} 2>&1 | tee -a "$indexer_testlog"
  ${testCmd[iterate]} 2>&1 | tee -a "$indexer_testlog"


  # increment and test next
  let iterate++
done

echo ""
echo "Done testing. Cleaning up"

exit 0
