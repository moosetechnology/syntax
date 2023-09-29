/* ********************************************************************
   *  This program has been generated from lig.at                     *
   *  on Fri Jan  6 15:57:33 1995                                     *
   *  by the SYNTAX (*) abstract tree constructor SEMAT               *
   ********************************************************************
   *  (*) SYNTAX is a trademark of INRIA.                             *
   ******************************************************************** */



/*   I N C L U D E S   */
#define NODE struct lig_node
#include "sxunix.h"

struct lig_node {
    SXNODE_HEADER_S VOID_NAME;

/*
your attribute declarations...
*/
};
/*
N O D E   N A M E S
*/
#define ERROR_n 1
#define ACTION_n 2
#define GENERIC_TERMINAL_n 3
#define LHS_NON_TERMINAL_n 4
#define LHS_OBJECT_n 5
#define LIG_n 6
#define NON_TERMINAL_n 7
#define PREDICATE_n 8
#define P_OBJECT_n 9
#define RHS_OBJECT_n 10
#define RULE_n 11
#define RULE_S_n 12
#define S_OBJECT_n 13
#define TERMINAL_n 14
#define TERMINAL_S_n 15
#define VOCABULARY_S_n 16
#define VOID_n 17
#define X_TERMINAL_n 18
/*
E N D   N O D E   N A M E S
*/


static void lig_pi () {

/*
I N H E R I T E D
*/

switch (VISITED->father->name) {

case ERROR_n :
break;

case LHS_OBJECT_n :
	switch (VISITED->position) {
	case 1 :/* VISITED->name = LHS_NON_TERMINAL_n */
	break;

	case 2 :/* VISITED->name = {P_OBJECT_n, S_OBJECT_n, VOID_n} */
	break;

	case 3 :/* VISITED->name = {ACTION_n, VOID_n} */
	break;

	}

break;

case LIG_n :/* VISITED->name = RULE_S_n */
break;

case P_OBJECT_n :/* VISITED->name = TERMINAL_S_n */
break;

case RHS_OBJECT_n :
	switch (VISITED->position) {
	case 1 :/* VISITED->name = NON_TERMINAL_n */
	break;

	case 2 :/* VISITED->name = {P_OBJECT_n, S_OBJECT_n, VOID_n} */
	break;

	case 3 :/* VISITED->name = {PREDICATE_n, VOID_n} */
	break;

	}

break;

case RULE_n :
	switch (VISITED->position) {
	case 1 :/* VISITED->name = LHS_OBJECT_n */
	break;

	case 2 :/* VISITED->name = VOID_n */
	break;

	case 3 :/* VISITED->name = VOCABULARY_S_n */
	break;

	case 4 :/* VISITED->name = VOID_n */
	break;

	}

break;

case RULE_S_n :/* VISITED->name = RULE_n */
break;

case S_OBJECT_n :/* VISITED->name = TERMINAL_S_n */
break;

case TERMINAL_S_n :break;

case VOCABULARY_S_n :/* VISITED->name = {ACTION_n, RHS_OBJECT_n, X_TERMINAL_n} */
break;

case X_TERMINAL_n :
	switch (VISITED->position) {
	case 1 :/* VISITED->name = {GENERIC_TERMINAL_n, TERMINAL_n} */
	break;

	case 2 :/* VISITED->name = {PREDICATE_n, VOID_n} */
	break;

	}

break;



/*
Z Z Z Z
*/

default:
break;
}
/* end lig_pi */
}

static void lig_pd () {

/*
D E R I V E D
*/

switch (VISITED->name) {

case ERROR_n :
break;

case ACTION_n :
break;

case GENERIC_TERMINAL_n :
break;

case LHS_NON_TERMINAL_n :
break;

case LHS_OBJECT_n :
break;

case LIG_n :
break;

case NON_TERMINAL_n :
break;

case PREDICATE_n :
break;

case P_OBJECT_n :
break;

case RHS_OBJECT_n :
break;

case RULE_n :
break;

case RULE_S_n :
break;

case S_OBJECT_n :
break;

case TERMINAL_n :
break;

case TERMINAL_S_n :
break;

case VOCABULARY_S_n :
break;

case VOID_n :
break;

case X_TERMINAL_n :
break;



/*
Z Z Z Z
*/

default:
break;
}
/* end lig_pd */
}

static void smpopen (sxtables_ptr)
struct sxtables *sxtables_ptr;
{
sxatcvar.atc_lv.node_size = sizeof (struct lig_node);
}

static void smppass ()
{

/*   I N I T I A L I S A T I O N S   */
/* ........... */

/*   A T T R I B U T E S    E V A L U A T I O N   */
sxsmp (sxatcvar.atc_lv.abstract_tree_root, lig_pi, lig_pd);

/*   F I N A L I S A T I O N S   */
/* ........... */

}

lig_smp (what, sxtables_ptr)int what;
struct sxtables *sxtables_ptr;
{
switch (what) {
case OPEN:
smpopen (sxtables_ptr);
break;
case SEMPASS: /* For [sub-]tree evaluation during parsing */
break;
case ACTION:
smppass ();
break;
case CLOSE:
break;
default:
break;
}
}
