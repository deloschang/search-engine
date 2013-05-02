echo ""
echo '............. TESTING CRAWLER ..............'
echo ""

# test insufficient arguments
./crawler
echo ""

# test sufficient but incorrect depth (too high)
./crawler www.cs.dartmouth.edu ./data/ 5
echo ""

# test sufficient but incorrect depth (too low)
./crawler www.cs.dartmouth.edu ./data/ -1 
echo ""

# test sufficient args but invalid directory (doesn't exist)
./crawler www.cs.dartmouth.edu ./nonexistent/ 1 
echo ""

# test invalid URL
./crawler www.cs.dartmouth.edu/deadlink ./data/ 1 
echo ""

# correct input 
./crawler www.cs.dartmouth.edu ./data/ 1 
echo ""
