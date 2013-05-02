echo ""
echo '............. TESTING CRAWLER ..............'
echo ""

# test insufficient arguments
./crawler

# test sufficient but incorrect depth (too high)
./crawler www.cs.dartmouth.edu ./data/ 5

# test sufficient but incorrect depth (too low)
./crawler www.cs.dartmouth.edu ./data/ -1 

# test sufficient but invalid directory (doesn't exist)
./crawler www.cs.dartmouth.edu ./nonexistent/ 1 
