echo '\n'
echo '............. TESTING CRAWLER ..........'
echo '\n'

# test insufficient arguments
./crawler

# test sufficient but incorrect depth (too high)
./crawler www.cs.dartmouth.edu ./data/ 5

# test sufficient but incorrect depth (too low)
./crawler www.cs.dartmouth.edu ./data/ -1 
