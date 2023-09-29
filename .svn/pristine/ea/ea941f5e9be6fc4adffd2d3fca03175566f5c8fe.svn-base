
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


#define CONCAT			1
#define MEMBER			2
#define SIZE			3
#define NTH			4
#define REVERSE			5
#define LESS			6
#define SUP			7
#define LESS_EQUAL		8
#define SUP_EQUAL		9
#define RANDOM			10
#define IS			11
#define DIVIDE			12
#define MINUS			13
#define MODULO			14
#define MULTIPLY		15
#define PLUS			16
#define UMINUS			17

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
