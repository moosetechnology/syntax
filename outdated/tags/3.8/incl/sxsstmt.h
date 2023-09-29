#define KEEP(stmt)  ((stmt) & 0x0800)
#define SCAN(stmt)  ((stmt) & 0x0400)
#define CODOP(stmt) ((stmt) >> 12)
#define NEXT(stmt)  ((stmt) & 0x3FF)
#define KSC(stmt)  ((stmt) & 0xFC00)

#define STMT(K,S,C,N) ((K)<<11|(S)<<10|(C)<<12|(N))

/* scanner statement operation code */
#define Error		0
#define SameState	1
#define State		2
#define HashReduce	3
#define Reduce		4
#define HashReducePost	5
#define ReducePost	6
#define FirstLookAhead	7
#define NextLookAhead	8
#define ActPrdct	9

/* system action code */
#define LowerToUpper	1
#define UpperToLower	2
#define Set		3
#define Reset		4
#define Erase		5
#define Include		6
#define UpperCase	7
#define LowerCase	8
#define Put		9
#define Mark		10
#define Release		11
#define Incr		12
#define Decr		13

/* system predicate code */
#define IsTrue		0
#define IsFirstCol	1
#define	IsSet		2
#define	IsReset		3
#define	NotIsFirstCol	4
#define IsLastCol	5
#define NotIsLastCol	6

