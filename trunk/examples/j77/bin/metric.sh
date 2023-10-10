#!/bin/sh

total=`grep -c '^<[0-9][0-9]*:' f77.semc`
done_list=`grep -c '^\$LIST' f77.semc`
done_name=`grep -c '^\$NAME' f77.semc`
unknown_statement=`grep -c "unknown_statement" f77.semc`
done=`expr $done_list + $done_name - $unknown_statement`
percent_done=`expr $done \* 100 / $total`

echo "# rules: $total"
echo "# rules done: $done"
echo "(# unknown_statement: $unknown_statement)"
echo "% rules done: $percent_done %"
