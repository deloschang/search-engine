#!/bin/bash
# Name: BATS_TSE.sh
#
# Description: This script will build, test and run the search engine
#
# Input:  None.
# Output: Built search engine with command line processing ready
#
# Command Line Options: None.

# Pseudocode: The script uses the makefile to build the search engine.
# Then it runs unit tests to test the search engine.
# Finally, it runs the search engine and allows for command-line
# processing


# Build the crawler
echo "(1) Building crawler"
cd ./crawler_dir/
make

# Test the crawler
if [ $? -eq 0 ]; then
  echo "(2) Testing crawler"
  ./crawler_test.sh
else 
  echo "Building crawler failed (make [crawler])"
  make clean
  exit 1
fi

# Run the crawler
if [ $? -eq 0 ]; then
  echo "(3) Running the crawler"
  ./crawler www.cs.dartmouth.edu ./data/ 2
  make clean
else 
  echo "Testing crawler failed (crawler_test.sh)"
  make clean
  exit 1
fi

exit 0
#########

# Next, run BATS.sh to build, test and run the indexer.

# Build the search engine
echo "Building query engine"
cd ../queryengine_dir/
make

# Build query engine unit tests
if [ $? -eq 0 ]; then
  echo "Building query engine unit tests"
  make unit
else 
  echo "Building query engine failed (make [queryengine])"
  make clean
  exit 1
fi

# Run query engine unit tests
if [ $? -eq 0 ]; then
  echo "Running query engine unit tests"
  ./queryengine_test
else 
  echo "Building query engine unit tests failed (make unit)"
  make clean
  exit 1
fi


# Launch the query engine
if [ $? -eq 0 ]; then
  # do something
  ./queryengine ../indexer_dir/index.dat ../crawler_dir/data
else 
  echo "Running unit tests failed (./queryengine_test)"
  make clean
  exit 1
fi

# cleanup
make clean

exit 0
