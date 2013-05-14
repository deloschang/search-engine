README FILE (QUERY ENGINE)

Created by: Delos Chang
Description: 

================
BATS_TSE.sh Assumptions
0. Script assumes that user is running BATS_TSE.sh from the current dir
   (i.e. ./BATS_TSE.sh and NOT ../BATS_TSE.sh)
   
Basic Query Engine Assumptions
0. Query has a maximum limit of 999 characters.
1. Same keywords are treated as one (e.g. dog dog dog yields the same
   as 'dog')
2. If there are multiple operators like "AND OR AND", the last operator
   takes effect. For example:
   - dog AND OR cat -> dog OR cat
   - dog OR AND cat -> dog AND cat
   - dog AND OR AND cat -> dog AND cat
3. Operators at the beginning/end of the query are discarded. For example:
   - AND dog -> dog
   - OR dog -> dog
   - AND dog OR cat -> dog OR cat
   - dog OR cat AND OR -> dog OR cat
   - dog OR cat AND -> dog OR cat

Functional Credit
1. To exit, type in "!exit" and press enter

Refactoring Credit
1. Refactored common definitions and macros to
   (../utils/header.h)
2. Refactored common index structs to (../utils/index.h)
3. Refactored indexer BATS.sh diff test to BATS_TSE.sh
4. Refactored common idioms in indexer and query engine to
   ../utils/index.c (i.e. creating a Document Node with X docId and Y
   frequency, getting a filepath, creating a WordNode)
