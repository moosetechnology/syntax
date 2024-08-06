#!/bin/sh

total=`grep -c '^<[^>]*>' f77.semc`
done_list=`grep -c '^\$LIST' f77.semc`
done_name=`grep -c '^\$NAME' f77.semc`
unknown_statement=`grep -c "unknown_statement" f77.semc`
unknown_parameter=`grep -c "unknown_parameter" f77.semc`
done=`expr $done_list + $done_name - $unknown_statement - $unknown_parameter`
percent_done=`expr $done \* 100 / $total`

echo "# rules: $total"
echo "# rules done: $done"
echo "(# unknown_statement: $unknown_statement)"
echo "(# unknown_parameter: $unknown_parameter)"
echo "% rules done: $percent_done %"
