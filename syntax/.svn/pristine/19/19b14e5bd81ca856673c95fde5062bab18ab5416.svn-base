
#define SHIFT_VAL	4
#define AND_VAL		15

#define ATOM			1
#define VARIABLE		2
#define TERM			3
#define STATIC_LIST		4
#define DYNAM_LIST		5
#define INTEGER_CONSTANT	6
#define DYNAM_ATOM		7
#define INTEGER_OP		8


/* i.e. (0||STATIC_LIST) */
#define EMPTY_LIST	STATIC_LIST
/* i.e. (0||VARIABLE) */
#define DUM_VAR		VARIABLE


#define CONCAT			((SXINT)1)
#define MEMBER			((SXINT)2)
#define SIZE			((SXINT)3)
#define NTH			((SXINT)4)
#define REVERSE			((SXINT)5)
#define LESS			((SXINT)6)
#define SUP			((SXINT)7)
#define LESS_EQUAL		((SXINT)8)
#define SUP_EQUAL		((SXINT)9)
#define RANDOM			((SXINT)10)
#define IS			((SXINT)11)
#define DIVIDE			((SXINT)12)
#define MINUS			((SXINT)13)
#define MODULO			((SXINT)14)
#define MULTIPLY		((SXINT)15)
#define PLUS			((SXINT)16)
#define UMINUS			((SXINT)17)

#define LAST_PPP		UMINUS

#define KV2REF(k,v)	(((v)<<SHIFT_VAL)+(k))
#define REF2VAL(v)	((v)>>SHIFT_VAL)
#define REF2KIND(v)	((v)&AND_VAL)

#define ADA	-1 /* Atom/Atom (un au moins est dynamique) */
#define TT	-2 /* Term/Term */
#define LL	-3 /* LIST/LIST (statiques ou dynamiques) */
#define II	-4 /* INT/INT (au moins un INTEGER_OP) */
#define DAI	-5 /* DYNAM_ATOM/INT (INT_CST ou INTEGER_OP) */

/* 1 => unifiable ssi egalite
   0 => non unifiable
   -1=> appeler la procedure d'unification */
