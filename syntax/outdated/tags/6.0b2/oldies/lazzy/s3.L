listof int l;
listof listof int f2 (listof int), l2;
listof listof listof int a3 [3];
int i;

l = cons (i, ());

if tail (()) == tail (()) then ; else ;
if () == () then ; else ;

if a3 [1] == cons (f2 (cons (i, ())), ())
	then a3 [1] = cons (cons (l, f2 (l)), a3 [2]);
	else l2 = tail (f2 (tail (())));

a3 [1] = cons (i, l);
