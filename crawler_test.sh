#!/bin/bash
# Description: This script will test the crawler that is generated from the 
# Makefile. Also, this script is automatically run with the default make 
# as a sanity check

# Input:   None.
# Output: Result of the different battery tests of parameters
# and the expectant results

# Command Line Options: None.

# Pseudocode: It runs through a list of incorrect parameters 
# with an expectant result


crawler_testlog="crawler_testlog.`date`"

echo ""
echo '............. TESTING CRAWLER ..............'
echo ""

j=0


### TEST FORMAT ###
# Test Name: title of the test
# Test Expected: expected output of the test 
# Test Command:  actual command of the test



# test insufficient arguments
testName[j]="$j. testing insufficient arguments (0)"
testExpected[j]="Expected Error: insufficient arguments. 3 required, you provided 0."
testCmd[j]="./crawler"
let j++


# test sufficient but incorrect depth (too high)
testName[j]="$j. testing sufficient arguments but incorrect depth (too high)"
testExpected[j]="Expected Error: Depth must be between 0 and 4. You entered 5"
testCmd[j]="./crawler www.cs.dartmouth.edu ./data/ 5"
let j++

# test sufficient but incorrect depth (too low)
testName[j]="$j. testing sufficient but incorrect depth (too low)"
testExpected[j]="Expected Error: Depth must be between 0 and 4. You entered -1"
testCmd[j]="./crawler www.cs.dartmouth.edu ./data/ -1"
let j++

# test sufficient args but invalid directory (doesn't exist)
testName[j]="$j. testing invalid directory"
testExpected[j]="Expected Error: The dir argument ./nonexistent/ was not found.  Please enter writable and valid directory."
testCmd[j]="./crawler www.cs.dartmouth.edu ./nonexistent/ 1"
let j++

# test sufficient args but directory not writable
testName[j]="$j. testing unwritable directory"
testExpected[j]="Expected Error: The dir argument ./unwritable/ was not writable.  Please enter writable and valid directory."
testCmd[j]="./crawler www.cs.dartmouth.edu ./unwritable/ 1"
let j++

# test invalid URL
testName[j]="$j. testing invalid URL using wget"
testExpected[j]="Expected Error: The URL www.cs.dartmouth.edu/deadlink was invalid. Please enter a valid URL."
testCmd[j]="./crawler www.cs.dartmouth.edu/deadlink ./data/ 1"
let j++

# correct input 
testName[j]="$j. testing correct input arguments"
testExpected[j]="Expected: All arguments expected to be correct"
testCmd[j]="./crawler www.cs.dartmouth.edu ./data/ 1"
let j++

iterate=0
while (($iterate < $j)); do

  echo ""

  # test to STDOUT for easy viewing
  echo ${testName[iterate]}  
  echo ${testExpected[iterate]} 
  ${testCmd[iterate]}


  ## send test result to the log
  echo ${testName[iterate]} >> "$crawler_testlog"
  echo ${testExpected[iterate]} >> "$crawler_testlog"
  echo -n "Output -->" >> "$crawler_testlog"
  { ${testCmd[iterate]} >> "$crawler_testlog";} 2>/dev/null # suppress
  
  # increment and test next
  let iterate++
done

echo "Done testing. Cleaning up"

# cleanup
make clean
