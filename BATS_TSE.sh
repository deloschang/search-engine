#!/bin/bash
# Name: BATS_TSE.sh
#
# Description: This script will build, test and run the search engine
#
# Input:  None.
# Output: Built search engine with command line processing ready
#
# Command Line Options: None.

# Pseudocode: 
#
# 1. Build Crawler with make
# 2. Test crawler with BATS.sh script
# 3. Run the crawler
#
# 4. Build the indexer with make
# 5. Test the indexer with BATS.sh script
# 6. Run the indexer 
# 7. Test the indexer reloading with diff

# 8. Build the query engine
# 9. Build query engine unit tests
# 10. Run query engine unit tests
# 11. Run query engine


CRAWLER_DIR="data/"
CRAWL_SITE="www.cs.dartmouth.edu"
DEPTH=2

INDEX_FILE="index.dat"
NEW_INDEX_FILE="index_new.dat"

###### CRAWLER ######

# Build the crawler
echo -e "\n (1) Building crawler"
cd ./crawler_dir/
make

# Test the crawler
if [ $? -eq 0 ]; then
  #############
  #############
  ############# ENABLE CRAWLER #####
  echo -e "\n (2) Testing crawler"
  ./BATS.sh
else 
  echo "Building crawler failed (make [crawler])"
  make clean
  exit 1
fi

# Run the crawler
if [ $? -eq 0 ]; then
  #############
  #############
  ############# ENABLE SLEEP #####
  echo -e "\n (3) Running the crawler"
  #############
  #############
  ############# ENABLE CRAWLER #####
  ./crawler $CRAWL_SITE ./$CRAWLER_DIR $DEPTH
else 
  echo "Testing crawler failed (BATS.sh)"
  make clean
  exit 1
fi

###### INDEXER ######

# Build the indexer
if [ $? -eq 0 ]; then
  make clean
  echo -e "\n (4) Building the indexer"

  cd ../indexer_dir
  make
else
  echo "Running crawler failed (./crawler)"
  exit 1
fi

# Test the indexer
if [ $? -eq 0 ]; then
  echo -e "\n (5) Testing indexer"
  ./BATS.sh
else 
  echo "Building indexer failed (make [indexer])"
  make clean
  exit 1
fi

# Run the indexer
if [ $? -eq 0 ]; then
  echo -e "\n (6) Running the indexer and testing Reload"
  ./indexer ../crawler_dir/data/ $INDEX_FILE $INDEX_FILE $NEW_INDEX_FILE
else
  echo "Testing indexer failed (BATS.sh)"
fi

# Testing reloaded File via diff
if [ $? -eq 0 ]; then
  make clean
  echo -e "\n (7) Testing reloaded indexer diff"

  if [[ -e "$INDEX_FILE" && -e "$NEW_INDEX_FILE" ]]; then

    # make sure sorting is the same
    sort -o $INDEX_FILE $INDEX_FILE
    sort -o $NEW_INDEX_FILE $NEW_INDEX_FILE

    #diff -q $INDEX_FILE $NEW_INDEX_FILE > /dev/null
    diff $INDEX_FILE $NEW_INDEX_FILE 

    # check the integrity of the files
    if [ $? -eq 0 ]; then
      echo "Index storage passed test!" 
    else
      echo "Index storage didn't pass test!" 
    fi

    debugFlag=0;
  else
    echo "Index files $INDEX_FILE and $NEW_INDEX_FILE do not exist. Not testing."
  fi
else 
  echo "Running indexer failed (./indexer)"
  make clean
  exit 1
fi

###### QUERY ENGINE ######

# Build the search engine
echo -e "\n (8) Building query engine"
cd ../queryengine_dir/
make

# Build query engine unit tests
if [ $? -eq 0 ]; then
  echo -e "\n (9) Building query engine unit tests"
  make unit
else 
  echo "Building query engine failed (make [queryengine])"
  make clean
  exit 1
fi

# Run query engine unit tests
if [ $? -eq 0 ]; then
  echo -e "\n (10) Running query engine unit tests"
  ./queryengine_test
else 
  echo "Building query engine unit tests failed (make unit)"
  make clean
  exit 1
fi


# Launch the query engine
if [ $? -eq 0 ]; then
  echo -e "\n (11) Running query engine (type !exit as query to exit"
  ./queryengine ../indexer_dir/$INDEX_FILE ../crawler_dir/$CRAWLER_DIR
else 
  echo "Running unit tests failed (./queryengine_test)"
  make clean
  exit 1
fi

# cleanup
make clean
cd ../utils
rm -f *.gch
rm -f *.o
cd ..

exit 0
