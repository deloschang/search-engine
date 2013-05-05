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

## test invalid URL
#testName[j]="$j. testing invalid URL using wget"
#testExpected[j]="Expected Error: The URL www.cs.dartmouth.edu/deadlink was invalid. Please enter a valid URL."
#testCmd[j]="./crawler www.cs.dartmouth.edu/deadlink ./data/ 1"
#let j++

## test dangerous crawling at foreign URL
#testName[j]="$j. testing dangerous crawling"
#testExpected[j]="Please only crawl Dartmouth websites. Example:cs.dartmouth.edu. Skipping."
#testCmd[j]="./crawler www.google.com ./data/ 1"
#let j++

# correct input for 3 parameters
testName[j]="$j. testing correct input arguments for filename index.dat (3 parameters)"
testExpected[j]="No errors expected."
testCmd[j]="./indexer ../crawler_dir/data/ index.dat"
let j++

# correct input for 5 parameters
testName[j]="$j. testing correct input arguments for filename index.dat (5 parameters)"
testExpected[j]="No errors expected."
testCmd[j]="./indexer ../crawler_dir/data/ index.dat index.dat index_new.dat"
let j++


iterate=0
while (($iterate < $j)); do

  echo ""

  # test to STDOUT for easy viewing
  echo ${testName[iterate]}  
  echo ${testExpected[iterate]} 
  ${testCmd[iterate]}


  ## send test result to the log
  echo ${testName[iterate]} >> "$indexer_testlog"
  echo ${testExpected[iterate]} >> "$indexer_testlog"
  echo -n "Output -->" >> "$indexer_testlog"
  ${testCmd[iterate]} >> "$indexer_testlog" 2>&1
  
  # increment and test next
  let iterate++
done

echo ""
echo "Done testing. Cleaning up"

# cleanup
make clean
