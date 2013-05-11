README FILE (QUERY ENGINE)
================
Basic Query Engine
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

