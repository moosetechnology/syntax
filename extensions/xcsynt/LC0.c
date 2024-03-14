/******************************************************************************
 *                                S Y N T A X
 *-----------------------------------------------------------------------------
 *   Copyright (C) 1972-2023 INRIA (Institut National de Recherche en
 *   Informatique et Automatique)
 *-----------------------------------------------------------------------------
 *   URL: http://sourcesup.renater.fr/projects/syntax
 *-----------------------------------------------------------------------------
 *   The source code of SYNTAX is distributed with two different licenses,
 *   depending on the files:
 *   - The recursive content of src/ and incl/ and the non-recursive content
 *     of SYNTAX's root directory are distributed under the CeCILL-C license
 *   - The recursive content of all other repertories is distributed under
 *     the CeCILL license
 *   All code produced by SYNTAX must be considered as being under the
 *   CeCILL-C license. Information about the CeCILL and CeCILL-C licenses
 *   can be found at, e.g., http://www.cecill.info
 *****************************************************************************/

/* Principe :
   Un symbole en look-ahead n'est teste explicitement au cours d'une analyse
   que si ce test est susceptible de diminuer le non-determinisme.
   Plus concretement soit L le look-ahead set associe a l'etat p = [A -> alpha.]
   et soit K le look-ahead set associe a l'ensemble des etats qui peuvent
   etre atteints en // avec p.
   - si K est inclus ds L on peut supprimer le test des elements de L
   - si K-L est non vide, les elements de L sont testes (Ca permet pour
     tout t ds K-L de ne pas faire la reduction A->alpha)
*/

static char	ME [] = "left_corner automaton constructor";

#include "sxversion.h"
#include "P_tables.h"
#include "csynt_optim.h"

char WHAT_XCSYNTLC0[] = "@(#)SYNTAX - $Id: LC0.c 3717 2024-02-10 07:24:01Z garavel $" WHAT_DEBUG;

extern char	*laset (VARSTR varstring, SXINT maxi, SXBA look_ahead_set);


SXINT
gotostar (SXINT state, SXINT start, SXINT goal)
{
    /* Bidon (pour le loader), ne doit pas etre utilise ds la version left corner.
       Appele' depuis "single.c", qui n'est pas utilise'. */
    sxuse(state); /* pour faire taire gcc -Wunused */
    sxuse(start); /* pour faire taire gcc -Wunused */
    sxuse(goal); /* pour faire taire gcc -Wunused */
    sxtrap (ME, "gotostar");	
    return 0;
}

static char	star_71 [] = "***********************************************************************";

static SXBA	*left_corner, *left_corner_star;
/* left_corner = {(A, B)  | A -> B beta dans P}*/

static XxY_header	LALC1; /* (prev_state, next_state) */

static XxY_header	ntXnt; /* sous relation de left_corner */
static SXBA		*ntXnt2la_set;
static struct ntXnt2attr {
    bool	is_nullable;
    }			*ntXnt2attr;

static XxY_header	stateXlhs; /* sous relation de left_corner */
static SXBA		*stateXlhs2la_set;

static SXBA		prod_set, reached_set;

static SXBA	state_set;
static SXINT	*state_stack;
static SXINT	state_stack_top;

static SXBA	has_epsilon_rule,
                is_epsilon_nt;

static XxY_header	forward_hd; /* memo structure for [walk_]forward */

/* Sous ensemble de stateXlhs */
static XxY_header	empty_hd; /* (state, xprod) et xprod : nt -> .epsilon ds closure (state) */
static struct empty2attr {
    SXINT		la;
    }			*empty2attr;

static XxY_header	depend_hd;
static SXINT		*tbp_stack;
static SXBA		tbp_set;


static SXINT	limitem, limt, limnt, limxnt, limpro, INDPRO;

static SXINT	Mt_items, Mnt_items;


static void
ntXnt_oflw (SXINT old_size, SXINT new_size)
{
    ntXnt2la_set = sxbm_resize (ntXnt2la_set, old_size +1, new_size + 1, limt);
    ntXnt2attr =
	(struct ntXnt2attr*) sxrealloc (ntXnt2attr, new_size + 1, sizeof (struct ntXnt2attr));
}

static void
empty_oflw (SXINT old_size, SXINT new_size)
{
    sxuse(old_size); /* pour faire taire gcc -Wunused */
    empty2attr =
	(struct empty2attr*) sxrealloc (empty2attr, new_size + 1, sizeof (struct empty2attr));
}


static void
depend_oflw (SXINT old_size, SXINT new_size)
{
    sxuse(old_size); /* pour faire taire gcc -Wunused */
    tbp_set = sxba_resize (tbp_set, new_size + 1);
}


static void
stateXlhs_oflw (SXINT old_size, SXINT new_size)
{
    stateXlhs2la_set = sxbm_resize (stateXlhs2la_set, old_size + 1, new_size + 1, limt);
}



static bool
set_reached_set (SXINT state, SXINT item)
{
    sxuse(item); /* pour faire taire gcc -Wunused */
    SXBA_1_bit (reached_set, state);
    return false;
}



static bool
walk_forward (SXINT state, SXINT item, bool (*f)(SXINT, SXINT))
{
    /* item : A -> alpha . X1 ... Xn
       On visite tous les etats atteints depuis state par transition sur X1 ... Xn
       Si item est complet, on appelle f ().
       Si f () retourne vrai, on arrete la visite. */
    SXINT		tnt, trans, next_state, dum;
    
/* 17/03/2003 BUG trans non initialise (merci Mr alpha)
    if (XxY_set (&forward_hd, state, trans, &dum))
	return false;
*/
    if (XxY_set (&forward_hd, state, item, &dum))
	return false;

    tnt = bnf_ag.WS_INDPRO [item].lispro;
    
    if (tnt == 0)
	/* Bout du chemin ... */
	return f (state, item);

    XxY_Xforeach (LALC1, state, trans) {
	next_state = XxY_Y (LALC1, trans);

	if (bnf_ag.WS_INDPRO [next_state - 1].lispro == tnt)
	    if (walk_forward (next_state, item + 1, f))
		return true;
		
    }

    return false;
}


static bool
forward (SXINT state, SXINT item)
{
    /* item : A -> alpha . X1 ... Xn
       On visite tous les etats atteints depuis state par transition sur X1 ... Xn
       On marque tous les etats reduce atteints sauf l'etat A -> alpha X1 ... Xn . a moins
       qu'il soit atteint en // avec d'autres!. */
    bool	ret_val;
    SXINT		tnt, xprod, trans, next_state, dum;
    
/* 17/03/2003 BUG trans non initialise (merci Mr alpha)
    if (XxY_set (&forward_hd, state, trans, &dum))
	return false;
*/
    if (XxY_set (&forward_hd, state, item, &dum))
	return false;

    tnt = bnf_ag.WS_INDPRO [item].lispro;
    
    if (tnt == 0) {
	/* Bout du chemin ... */
	if (state == item) {
	    /* ...principal */
	    ret_val = false;
	}
	else {
	    /* ... secondaire */	
	    if (bnf_ag.WS_INDPRO [state].lispro == 0) {
		xprod = bnf_ag.WS_INDPRO [state].prolis;
		SXBA_1_bit (prod_set, xprod);
	    }

	    ret_val = true;
	}
    }
    else {
	ret_val = false;

	XxY_Xforeach (LALC1, state, trans) {
	    next_state = XxY_Y (LALC1, trans);

	    if (bnf_ag.WS_INDPRO [next_state - 1].lispro == tnt &&
		forward (next_state, item + 1))
		ret_val = true;
	}
    }

    return ret_val;
}


static SXINT
lc_look_ahead (SXINT nt1, SXINT nt2)
{
    /* nt2 est dans left_corner de nt1 */
    /* Calcul le look-ahead associe' a nt2 quand il est calcule' a partir de nt1. */
    SXINT 	la, lim, i, xprod, item, tnt, t;
    SXBA	la_set;
    bool	is_nullable;

    if (!XxY_set (&ntXnt, nt1, nt2, &la)) {
	/* nouveau */
	la_set = ntXnt2la_set [la];
	is_nullable = false;

	/* On utilise les regles ou nt1 est en lhs. */
	lim = bnf_ag.WS_NTMAX [nt1 + 1].npg;

	for (i = bnf_ag.WS_NTMAX [nt1].npg; i < lim; i++) {
	    xprod = bnf_ag.WS_NBPRO [i].numpg;
	    item = bnf_ag.WS_NBPRO [xprod].prolon;
	    tnt = bnf_ag.WS_INDPRO [item].lispro;

	    if (tnt > 0 && XNT_TO_NT_CODE (tnt) == nt2) {
		/* C'est une bonne */
		while ((tnt = bnf_ag.WS_INDPRO [++item].lispro) > 0) {
		    sxba_or (la_set, bnf_ag.FIRST [XNT_TO_NT_CODE (tnt)]);

		    if (!SXBA_bit_is_set (bnf_ag.BVIDE, tnt))
			break;
		}

		if (tnt == 0)
		    is_nullable = true;
		else if (tnt < 0) {
		    t = -tnt;
		    SXBA_1_bit (la_set, t);
		}
	    }
	}

	/* si is_nullable il existe une regle :
	   nt1 -> nt2 alpha avec alpha =>* epsilon */
	ntXnt2attr [la].is_nullable = is_nullable;
    }
    
    return la;
}



static SXINT
nt_look_ahead (SXINT state, SXINT lhs)
{
    /* Calcule le look-ahead associe' a lhs ds l'etat state. */
    /* lhs est un non terminal de closure (state) */
    SXINT		la, nt_kernel, nt, lcla, ntla, item, tnt, t, xprod, trans, depend;
    SXBA	top_line, line;

    if (!XxY_set (&stateXlhs, state, lhs, &la)) {
	/* nouveau */
	nt_kernel = bnf_ag.WS_INDPRO [state].lispro;

#if EBUG
	if (nt_kernel <= 0)
	    sxtrap (ME, "look_ahead");
#endif

	nt_kernel = XNT_TO_NT_CODE (nt_kernel);

	top_line = left_corner_star [nt_kernel];

	line = left_corner [lhs];
	nt = 0;

	while ((nt = sxba_scan (line, nt)) > 0) {
	    /* lhs est dans left_corner de nt */
	    if (SXBA_bit_is_set (top_line, nt)) {
		/* nt est lui meme un left_corner* du nt du kernel de state */
		lcla = lc_look_ahead (nt, lhs);
	       
		sxba_or (stateXlhs2la_set [la], ntXnt2la_set [lcla]);

		if (ntXnt2attr [lcla].is_nullable) {
		    /* Il peut y avoir reallocation... */
		    ntla = nt_look_ahead (state, nt);

		    XxY_set (&depend_hd, ntla, la, &depend);

		    if (SXBA_bit_is_reset_set (tbp_set, ntla))
			SS_push (tbp_stack, ntla);
		}
	    }
	}
    
	if (nt_kernel == lhs) {
	    /* On est remonte' jusqu'au kernel */
	    item = state;

	    while ((tnt = bnf_ag.WS_INDPRO [++item].lispro) > 0) {
		/* initialisation */
		sxba_or (stateXlhs2la_set [la], bnf_ag.FIRST [XNT_TO_NT_CODE (tnt)]);

		if (!SXBA_bit_is_set (bnf_ag.BVIDE, tnt))
		    break;
	    }

	    if (tnt == 0) {
		xprod = bnf_ag.WS_INDPRO [state].prolis;
		item = bnf_ag.WS_NBPRO [xprod].prolon;
		lhs = bnf_ag.WS_NBPRO [xprod].reduc;

		XxY_Yforeach (LALC1, item + 1, trans) {
		    state = XxY_X (LALC1, trans);
		    /* Il peut y avoir reallocation... */
		    ntla = nt_look_ahead (state, lhs);

		    XxY_set (&depend_hd, ntla, la, &depend);

		    if (SXBA_bit_is_reset_set (tbp_set, ntla))
			SS_push (tbp_stack, ntla);
		}
	    }
	    else if (tnt < 0) {
		t = -tnt;
		SXBA_1_bit (stateXlhs2la_set [la], t);
	    }
	}
    }

    return la;
}



static void
print_state (SXINT state)
{
    SXINT		next_state, tnt, trans, la, xprod, empty, item;
    char	s1 [28], *string;
    bool	is_non_kernel;

    put_edit (1, star_71);
    tnt = bnf_ag.WS_INDPRO [state].lispro;

    if (tnt == bnf_ag.WS_TBL_SIZE.tmax) {
	strcpy (s1, " ---> HALT");
	string = s1;
    }
    else {
	if (tnt == 0) {
	    xprod = bnf_ag.WS_INDPRO [state].prolis;

	    if (SXBA_bit_is_set (prod_set, xprod)) {
		/* Il y a du look_ahead */
		la = XxY_is_set (&stateXlhs, state, 0);
		string = laset (vstr, 60, stateXlhs2la_set [la]);
	    }
	    else {
		sprintf (s1, " {}");
		string = s1;
	    }
	}
	else {
	    sprintf (s1, " ---> %ld", (SXINT) state + 1);
	    string = s1;
	}
    }

    soritem (state, 1, 71, "*", string);
    put_edit (1, star_71);

    if (tnt > 0) {
	is_non_kernel = false;

	XxY_Xforeach (LALC1, state, trans) {
	    next_state = XxY_Y (LALC1, trans);

	    if (next_state != state + 1) {
		sprintf (s1, " ---> %ld", (SXINT) next_state);
		soritem (next_state - 1, 1, 71, "*", s1);
		is_non_kernel = true;
	    }
	}

	if (is_non_kernel)
	    put_edit (1, star_71);

	if (SXBA_bit_is_set (has_epsilon_rule, state)) {
	    XxY_Xforeach (empty_hd, state, empty) {
		xprod = XxY_Y (empty_hd, empty);
		la = empty2attr [empty].la;
		item = bnf_ag.WS_NBPRO [xprod].prolon;
		soritem (item, 1, 71, "*",
			 laset (vstr, 60, stateXlhs2la_set [la]));
	    }

	    put_edit (1, star_71);
	}
    }
}


static void
gen_fe (SXINT state, SXINT red_no, SXINT xtnt)
{
    SXINT			next_state, tnt, xprod, reduce, next, pspl;
    struct P_item	*aitem;

    if (red_no < 0) {
	/* Shift */ 
	next_state = -red_no;
					
	if ((tnt = bnf_ag.WS_INDPRO [next_state].lispro) == 0) {
	    /* Etat final */
	    xprod = bnf_ag.WS_INDPRO [next_state].prolis;

	    if (!SXBA_bit_is_set (prod_set, xprod)) {
		/* Il n'y a pas de look_ahead */
		if (state == 2) {
		    /* Generation de l'instruction Halt */
		    /* xtnt == tmax */
		    t_fe->codop = Reduce;
		    t_fe->reduce = 0;
		    t_fe->pspl = 2;
		    t_fe->next = 1;
		}
		else {
		    reduce = PROD_TO_RED (xprod);
		    next = bnf_ag.WS_NBPRO [xprod].reduc;
		    next = XNT_TO_NT_CODE (next);
		    pspl = LGPROD (reduce) - 1;

		    if (xtnt < 0) {
			t_fe->codop = ScanReduce;
			t_fe->reduce = reduce;
			t_fe->pspl = pspl;
			t_fe->next = next;
		    }
		    else {
			nt_fe->codop = Reduce;
			nt_fe->reduce = reduce;
			nt_fe->pspl = pspl;
			nt_fe->next = next;
		    }

		    SXBA_0_bit (not_used_red_set, reduce);
		}
	    }
	    else {
		if (xtnt < 0) {
		    t_fe->codop = ScanShift;
		    t_fe->reduce = 0;
		    t_fe->pspl = 0;
		    t_fe->next = next_state;
		}
		else {
		    nt_fe->codop = Shift;
		    nt_fe->reduce = 0;
		    nt_fe->pspl = 0;
		    nt_fe->next = next_state;
		}
	    }
	}
	else {
	    if (xtnt < 0) {
		t_fe->codop = ScanShift;
		t_fe->reduce = 0;
		t_fe->pspl = tnt < 0 ? 0 : next_state;
		t_fe->next = next_state;
	    }
	    else {
		nt_fe->codop = Shift;
		nt_fe->reduce = 0;
		nt_fe->pspl = tnt < 0 ? 0 : next_state;
		nt_fe->next = next_state;
	    }
	}
    }
    else			/* Reduce */ {
	/* red_no est OK. */
	t_fe->codop = Reduce;
	t_fe->pspl = LGPROD (red_no);
	t_fe->next = bnf_ag.WS_NBPRO [red_no].reduc;
	t_fe->reduce = red_no;
	SXBA_0_bit (not_used_red_set, red_no);
    }
				    
    aitem = xtnt < 0 ? t_items + xt_items++ : nt_items + xnt_items++;
    aitem->check = xtnt < 0 ? -xtnt : xtnt;
    aitem->action = xtnt < 0 ? put_in_fe (t_fe, t_hash, t_lnks, t_fe)
	: put_in_fe (nt_fe, nt_hash, nt_lnks, nt_fe);
}


    
#define ND_LIST_PUT	((SXINT)1)
#define ND_LIST_GET	((SXINT)2)
#define ND_LIST_CLEAR	((SXINT)4)
#define ND_LIST_FREE	((SXINT)3)

static void
nd_list (SXINT kind, SXINT t, SXINT *act)
{
    /* gere, pour chaque etat les actions du non-determinisme */
    static SXINT *nd_hd, *nd_stack, *nd_lnk;

    SXINT		hd, size;

    switch (kind) {
    case ND_LIST_PUT:
	if (nd_hd == NULL) {
	    size = limxnt > limt ? limxnt : limt;

	    nd_hd = (SXINT*) sxcalloc (size, sizeof (SXINT));
	    nd_stack = SS_alloc (size);
	    nd_lnk = SS_alloc (size);
	}

	SS_push (nd_stack, *act);
	SS_push (nd_lnk, nd_hd [t]);
	nd_hd [t] = SS_top (nd_stack) - 1;
	break;

    case ND_LIST_GET:
	if ((hd = nd_hd [t]) != 0) {
	    *act = SS_get (nd_stack, hd);
	    nd_hd [t] = SS_get (nd_lnk, hd);
	}
	else
	    *act = 0;

	break;

    case ND_LIST_CLEAR:
	if (nd_hd != NULL) {
	    SS_clear (nd_stack);
	    SS_clear (nd_lnk);
	}

	break;

    case ND_LIST_FREE:
	if (nd_hd != NULL) {
	    sxfree (nd_hd), nd_hd = NULL;
	    SS_free (nd_stack);
	    SS_free (nd_lnk);
	}
	break;
    default:
#if EBUG
      sxtrap(ME,"unknown switch case #1");
#endif
      break;
    }
}


/* Floyd_Evans des transitions non-terminales */
static void
ntfe (void)
{
    SXINT			state, next_state, tnt, xnt, act, red_no, x, trans, xprod;
    struct state	*astate;
    SXINT			*nter;
    bool		is_nd_list;
    SXBA		xnt_set;

    if (sxverbosep)
	fputs ("\tNT_Tables ... ", sxtty);

    xnt_set = sxba_calloc (limxnt);
    xnt_state_set = NULL;

    nt_states = (struct state*) sxcalloc (limitem + 1, sizeof (struct state));
    nt_items = (struct P_item*) sxalloc (Mnt_items + 1, sizeof (struct P_item));
    nt_fe = (struct floyd_evans*) sxalloc (limitem + 1, sizeof (struct floyd_evans));
    nt_lnks = (SXINT*) sxalloc (limitem + 1, sizeof (SXINT));

    nter = (SXINT*) sxcalloc (limxnt, sizeof (SXINT));

    for (x = 0; x < HASH_SIZE; x++)
	t_hash [x] = nt_hash [x] = 0;

    nt_lnks [0] = 0;
    xnt_items = 1;

    for (state = 1; state < limitem; state++) {
	if (state != 3 &&
	    (state == 1 ||
	     bnf_ag.WS_NBPRO [xprod = bnf_ag.WS_INDPRO [state].prolis].prolon != state)) {
	    tnt = bnf_ag.WS_INDPRO [state].lispro;

	    if (tnt > 0) {
		is_nd_list = false;

		XxY_Xforeach (LALC1, state, trans) {
		    next_state = -XxY_Y (LALC1, trans);
		    xnt = bnf_ag.WS_INDPRO [-next_state - 1].lispro;

		    if (xnt > 0) {
			if (SXBA_bit_is_reset_set (xnt_set, xnt))
			    nter [xnt] = next_state;
			else {
			    is_nd_list = true;

			    if ((act = nter [xnt]) != 0) {
				nter [xnt] = 0;
				nd_list (ND_LIST_PUT, xnt, &act);
			    }

			    nd_list (ND_LIST_PUT, xnt, &next_state);
			}
		    }
		}
	    
		/* On ne s'occupe pas [trop] des predicats c'est l'optimiseur
		   qui produira le code correspondant */

		astate = nt_states + state;
		/* astate->lgth = 0; inutile (calloc) */
		astate->start = xnt_items;
		
		xnt = 0;

		while ((xnt = sxba_scan_reset (xnt_set, xnt)) > 0) {
		    red_no = nter [xnt];
		    nter [xnt] = 0;

		    if (xnt > bnf_ag.WS_TBL_SIZE.ntmax) {
			xprdct_items++;
			xpredicates++;
			    
			if (xnt_state_set == NULL)
			    xnt_state_set = sxba_calloc (limitem + 1);
			    
			SXBA_1_bit (xnt_state_set, state);
		    }
			
		    if (red_no != 0) {
			/* Action unique */
			gen_fe (state, red_no, xnt);
			astate->lgth++;
		    }
		    else {
			nd_list (ND_LIST_GET, xnt, &red_no);

			/* Ici on est dans le cas non deterministe. */
			/* Il peut y avoir plusieurs instructions shift */
			if (nd_state_set == NULL)
			    nd_state_set = sxba_calloc (limitem + 1);
				
			SXBA_1_bit (nd_state_set, state);
			xpredicates++;

			do {
			    nd_degree++;
			    gen_fe (state, red_no, xnt);
			    astate->lgth++;

			    xprdct_items++; /* mis avec les predicats. */
			    nd_list (ND_LIST_GET, xnt, &red_no);
			} while (red_no != 0);
		    }
		}

		if (is_nd_list)
		    nd_list (ND_LIST_CLEAR, (SXINT)0, (SXINT*)NULL);
	    }
	}
    }

    nd_list (ND_LIST_FREE, (SXINT)0, (SXINT*)NULL);

    xprdct_items += xpredicates /* Nombre de &Else */ ;

    sxfree (xnt_set);

    if (sxverbosep)
	sxtime (SXACTION, "done");
}


/* Floyd_Evans des reductions avec look_ahead */
static void
redfe (void)
{
    SXINT			xprod, item, trans, state, para_state, next_state, t, empty, nt, la, xt,
                        prev_item;
    SXBA		para_set, la_set;
    struct P_item	*aitem;
    struct state	*astate;

    para_set = stateXlhs2la_set [0];
    sxba_empty (para_set);
    xprod = 0;

    while ((xprod = sxba_scan (prod_set, xprod)) > 0) {
	/* Il y a du look_ahead */
	/* Il faut le generer (a l'execution il est en // avec un
	   autre etat et la presence de look-ahead peut diminuer le
	   non-determinisme. */
	/* On peut etre + fin.
	   Principe : on ne genere un test de look-ahead que si, en //
	   il existe une autre action sur le meme test. Ici on sait
	   qu'il existe d'autres etats q atteints en // avec state.
	   Soit K = U loook-ahead(q), L = look-ahead(state) et M = K-L.
	   - Si M est vide on fait la reduction ds tous les cas
	   - Si M est non vide les elements de L sont testes (Ca permet pour
	   tout t ds K-L de ne pas faire la reduction A->alpha). */

	item = bnf_ag.WS_NBPRO [xprod].prolon;
	XxY_clear (&forward_hd);

	XxY_Yforeach (LALC1, item + 1, trans) {
	    prev_item = XxY_X (LALC1, trans);
	    walk_forward (prev_item, item, set_reached_set);
	}

	state = bnf_ag.WS_NBPRO [xprod + 1].prolon - 1;
	SXBA_0_bit (reached_set, state);

	para_state = 0;

	while ((para_state = sxba_scan_reset (reached_set, para_state)) > 0) {
	    if (bnf_ag.WS_INDPRO [para_state].lispro == 0) {
		/* etat final */
		sxba_or (para_set,
			 stateXlhs2la_set [XxY_is_set (&stateXlhs, para_state, 0)]);
	    }
	    else {
		/* On accumule les transitions terminales avec les look-ahead
		   des productions vides. */
		XxY_Xforeach (LALC1, para_state, trans) {
		    next_state = XxY_Y (LALC1, trans);

		    if ((t = -bnf_ag.WS_INDPRO [next_state - 1].lispro) > 0)
			SXBA_1_bit (para_set, t);
		}

		if (SXBA_bit_is_set (has_epsilon_rule, para_state)) {
		    XxY_Xforeach (empty_hd, para_state, empty) {
			nt = bnf_ag.WS_NBPRO [XxY_Y (empty_hd, empty)].reduc;
			la = nt_look_ahead (state, nt);
			sxba_or (para_set, stateXlhs2la_set [la]);
		    }
		}
	    }
	}

	la = XxY_is_set (&stateXlhs, state, 0);
	la_set = stateXlhs2la_set [la];

	sxba_minus (para_set, la_set);

	if (sxba_is_empty (para_set))
	    SXBA_0_bit (prod_set, xprod);
	else {
	    sxba_empty (para_set);

	    astate = t_states + state;
	    /* astate->lgth = 0; inutile (calloc) */
	    astate->start = xt_items;

	    xt = 0;

	    while ((xt = sxba_scan (la_set, xt)) > 0) {
		if (xt > -bnf_ag.WS_TBL_SIZE.tmax) {
		    xprdct_items++;
		    xpredicates++;
			    
		    if (xt_state_set == NULL)
			xt_state_set = sxba_calloc (limitem + 1);
			    
		    SXBA_1_bit (xt_state_set, state);
		}

		gen_fe (state, xprod, -xt);
		astate->lgth++;
	    }

	    /* maj pour le defaut (error) */
	    xt_items++;
	    astate->lgth++;

	    /* Generation du defaut */
	    aitem = t_items + astate->start + astate->lgth - 1;
	    aitem->check = any;
	    aitem->action = t_error;
	}
    }
}


/* Floyd_Evans des transitions terminales */
static void
tfe (void)
{
    SXINT			state, tnt, xt, t, red_no, act, ntla, empty, next_state, trans,
                        xprod, nb, pop_nb, pop_red, pop_fe, size;
    SXINT			*ter, *ter2red;
    struct P_item	*aitem;
    struct state	*astate;
    SXBA		xt_set, t_set, la_set;
    bool		is_nd_list;


    if (sxverbosep)
	fputs ("\tT_Tables ... ", sxtty);

    xpredicates = xprdct_items = 0;
    any = bnf_ag.WS_TBL_SIZE.xtmax + 1;
    t_states = (struct state*) sxcalloc (limitem + 1, sizeof (struct state));
    t_items = (struct P_item*) sxalloc (Mt_items
					+ 4 /* etats initial et final */
					+ 1,
					sizeof (struct P_item));
    t_fe = (struct floyd_evans*) sxalloc (size = Mt_items
					  + 2 /* etats initial et final */
					  + 2 /* [la]error */
					  + 1,
					  sizeof (struct floyd_evans));
    t_lnks = (SXINT*) sxalloc (size, sizeof (SXINT));
    t_lnks [0] = 0;

    xt_items = 1;

    /* On genere l'instruction erreur */
    t_fe->codop = Error;
    t_fe->reduce = 0;
    t_fe->pspl = 0;
    t_fe->next = 0;
    t_error = put_in_fe (t_fe, t_hash, t_lnks, t_fe);

    /* On genere l'etat initial */
    init_state = limitem;
    astate = t_states + init_state;
    astate->lgth = 2;
    astate->start = xt_items;
    t_fe->codop = ScanShift;
    t_fe->reduce = 0;
    t_fe->pspl = 1;
    t_fe->next = 1;
    (aitem = t_items + xt_items++)->action = put_in_fe (t_fe, t_hash, t_lnks, t_fe);
    aitem->check = -bnf_ag.WS_TBL_SIZE.tmax /* Test de "End Of File" en debut de texte source */ ;
    (aitem = t_items + xt_items++)->action = t_error;
    aitem->check = any;

    if (k_value > 0)
	redfe ();

    ter = (SXINT*) sxcalloc (limt, sizeof (SXINT));
    ter2red = (SXINT*) sxcalloc (limt, sizeof (SXINT));
    t_set = sxba_calloc (limt);
    xt_set = sxba_calloc (limt);

    xt_state_set = NULL;

    for (state = 1; state < limitem; state++) {
	if (state != 3 &&
	    (state == 1 ||
	     bnf_ag.WS_NBPRO [xprod = bnf_ag.WS_INDPRO [state].prolis].prolon != state)  &&
	    (tnt = bnf_ag.WS_INDPRO [state].lispro) != 0) {
	    /* Pas item initial (sauf etat initial) ni final (deja traite) */
	    is_nd_list = false;
	    pop_fe = t_error;	/* a priori */

	    XxY_Xforeach (LALC1, state, trans) {
		/* scan */
		next_state = -XxY_Y (LALC1, trans);
		xt = -bnf_ag.WS_INDPRO [-next_state - 1].lispro;

		if (xt > 0) {
		    if (SXBA_bit_is_reset_set (xt_set, xt))
			ter [xt] = next_state;
		    else {
			is_nd_list = true;

			if ((act = ter [xt]) != 0) {
			    ter [xt] = 0;
			    nd_list (ND_LIST_PUT, xt, &act);
			}

			nd_list (ND_LIST_PUT, xt, &next_state);
		    }
		}
	    }
		
	    pop_red = 0;

	    if (SXBA_bit_is_set (has_epsilon_rule, state)) {
		XxY_Xforeach (empty_hd, state, empty) {
		    /* Reduce sur une production vide */
		    ntla = empty2attr [empty].la;
		    la_set = stateXlhs2la_set [ntla];
		    red_no = XxY_Y (empty_hd, empty);

		    xt = 0;
			
		    while ((xt = sxba_scan (la_set, xt)) > 0) {
			t = XT_TO_T_CODE (xt);

			if (SXBA_bit_is_reset_set (xt_set, xt)) {
			    if (SXBA_bit_is_reset_set (t_set, t))
				ter2red [t] = red_no;
			    else
				if (ter2red [t] != red_no)
				    ter2red [t] = 0;

			    ter [xt] = red_no;
			}
			else {
			    /* Conflit sur le xt, il faut generer ds tous les cas */
			    ter2red [t] = 0;
			    SXBA_0_bit (t_set, t);

			    is_nd_list = true;

			    if ((act = ter [xt]) != 0) {
				ter [xt] = 0;
				nd_list (ND_LIST_PUT, xt, &act);
			    }

			    nd_list (ND_LIST_PUT, xt, &red_no);
			}
		    }
		}

		pop_nb = 0;

		while ((t = sxba_scan_reset (t_set, 0)) > 0) {
		    red_no = ter2red [t];
		    nb = 1;
			    
		    while ((t = sxba_scan (t_set, t)) > 0) {
			if (ter2red [t] == red_no) {
			    SXBA_0_bit (t_set, t);
			    nb++;
			}
		    }

		    if (nb > pop_nb) {
			pop_nb = nb;
			pop_red = red_no;
		    }
		}

		if (pop_red != 0) {
		    /* On genere la reduction la plus populaire */
		    t_fe->codop = Reduce;
		    t_fe->pspl = LGPROD (pop_red);
		    t_fe->next = bnf_ag.WS_NBPRO [pop_red].reduc;
		    t_fe->reduce = pop_red;
		    SXBA_0_bit (not_used_red_set, pop_red);
		    pop_fe = put_in_fe (t_fe, t_hash, t_lnks, t_fe);
		}
	    }

	    /* Etat non reduce ou reduce avec look-ahead */
	    /* Recherche de la reduction la plus populaire : on ne fait rien!!! */

	    /* Generation du Floyd_Evans pour les t_tables */

	    /* On ne s'occupe pas [trop] des predicats c'est l'optimiseur
	       qui produira le code correspondant */

	    astate = t_states + state;
	    /* astate->lgth = 0; inutile (calloc) */
	    astate->start = xt_items;
	    xt = 0;

	    while ((xt = sxba_scan_reset (xt_set, xt)) > 0) {
		red_no = ter [xt];
		ter [xt] = 0;

		if (xt > -bnf_ag.WS_TBL_SIZE.tmax) {
		    xprdct_items++;
		    xpredicates++;
			    
		    if (xt_state_set == NULL)
			xt_state_set = sxba_calloc (limitem + 1);
			    
		    SXBA_1_bit (xt_state_set, state);
		}
			
		if (red_no != 0) {
		    /* Action unique */
		    t = XT_TO_T_CODE (xt);

		    if (red_no < 0 /* scan */
			|| red_no != pop_red /* pas la + populaire */
			|| ter2red [t] == 0 /* "conflit" reduce/reduce
					       sur le representant */) {
			gen_fe (state, red_no, -xt);
			astate->lgth++;
		    }

		    ter2red [t] = 0;
		}
		else {
		    nd_list (ND_LIST_GET, xt, &red_no);

		    /* Ici on est dans le cas non deterministe. */
		    /* Il peut y avoir plusieurs instructions shift */
		    if (nd_state_set == NULL)
			nd_state_set = sxba_calloc (limitem + 1);
				
		    SXBA_1_bit (nd_state_set, state);
		    xpredicates++;

		    do {
			nd_degree++;
			gen_fe (state, red_no, -xt);
			astate->lgth++;

			xprdct_items++; /* mis avec les predicats. */
			nd_list (ND_LIST_GET, xt, &red_no);
		    } while (red_no != 0);
		}
	    }
			    
	    /* maj pour le defaut */
	    xt_items++;
	    astate->lgth++;

	    /* Generation du defaut */
	    aitem = t_items + astate->start + astate->lgth - 1;
	    aitem->check = any;
	    aitem->action = pop_fe;

	    if (is_nd_list)
		nd_list (ND_LIST_CLEAR, (SXINT)0, (SXINT*)NULL);
	}
    }

    nd_list (ND_LIST_FREE, (SXINT)0, (SXINT*)NULL);

    xprdct_items += xpredicates /* Nombre de &Else */ ;

    sxfree (ter), ter = NULL;
    sxfree (ter2red);
    sxfree (t_set);
    sxfree (xt_set);

    if (sxverbosep)
	sxtime (SXACTION, "done");

}


SXINT
LC0 (void)
{
    SXINT		xprod, B, A, nt, state, tnt, next_state, trans, lim, i, item, lhs, la, ntla,
                empty, prev_state, n, x, z;
    SXINT		prev, next, depend;
    SXBA	work_set_local, prev_set, next_set;
    SXBA	line;
    bool	is_epsilon, look_ahead_needed;

    if (sxverbosep) {
	fprintf (sxtty, "%s\n", ME);

	if (print_time)
	    sxtime (SXINIT, NULL);
    }

    if (is_statistics) {
	char	statistics_file_name [32];

	if ((statistics_file = fopen (strcat (strcpy (statistics_file_name, language_name), ".la.s"), "w")) == NULL) {
	    fprintf (sxstderr, "%s: cannot open (create) ", ME);
	    sxperror (statistics_file_name);
	}
    }
    else
	statistics_file = NULL;

    if (sxverbosep) {
      if (k_value == 0)
	fputs ("\tLC (0) Automaton ... ", sxtty);
      else
	fputs ("\tLALC (1) Automaton ... ", sxtty);
    }


    MAX_LA = 1;			/* nombre max de tokens en avance. */
    k_value_max = k_value;
    is_not_LRpi = false;
    is_ambiguous = false;
    is_listing_opened = false;
    nd_degree = 0;

    limt = bnf_ag.WS_TBL_SIZE.xtmax + 1;
    limnt = bnf_ag.WS_TBL_SIZE.ntmax + 1;
    limxnt = bnf_ag.WS_TBL_SIZE.xntmax + 1;
    limpro = bnf_ag.WS_TBL_SIZE.xnbpro + 1;
    limitem = bnf_ag.WS_TBL_SIZE.indpro + 1;
    INDPRO = max (10, limitem);
    has_xprod = limpro > bnf_ag.WS_TBL_SIZE.actpro + 1;
    not_used_red_set = sxba_calloc (bnf_ag.WS_TBL_SIZE.actpro + 1);
    sxba_fill (not_used_red_set), SXBA_0_bit (not_used_red_set, 0);

    left_corner = sxbm_calloc (bnf_ag.WS_TBL_SIZE.ntmax + 1, bnf_ag.WS_TBL_SIZE.ntmax + 1);
    left_corner_star = sxbm_calloc (bnf_ag.WS_TBL_SIZE.ntmax + 1, bnf_ag.WS_TBL_SIZE.ntmax + 1);

    XxY_alloc (&LALC1, "LALC1", 4*INDPRO, 1, 1, 1, NULL,
#if EBUG
	       stdout
#else
	       NULL
#endif
	       ); 

    XxY_alloc (&ntXnt, "ntXnt", 2 * limxnt, 1, 0, 0, ntXnt_oflw,
#if EBUG
	       stdout
#else
	       NULL
#endif
	       ); 

    ntXnt2la_set = sxbm_calloc (XxY_size (ntXnt) + 1, limt);
    ntXnt2attr = (struct ntXnt2attr*) sxalloc (XxY_size (ntXnt) + 1, sizeof (struct ntXnt2attr));


    XxY_alloc (&stateXlhs, "stateXlhs", 4*limpro, 1, 0, 0, stateXlhs_oflw,
#if EBUG
	       stdout
#else
	       NULL
#endif
	       ); 

    stateXlhs2la_set = sxbm_calloc (XxY_size (stateXlhs) + 1, limt);


    XxY_alloc (&forward_hd, "forward_hd", limpro, 1, 0, 0, NULL,
#if EBUG
	       stdout
#else
	       NULL
#endif
	       ); 


    prod_set = sxba_calloc (limpro);

    state_set = sxba_calloc (INDPRO);
    state_stack = (SXINT*) sxalloc (INDPRO, sizeof (SXINT));

    has_epsilon_rule = sxba_calloc (limitem);
    is_epsilon_nt = sxba_calloc (limnt);

    XxY_alloc (&empty_hd, "empty_hd", limnt, 1, 1, 0, empty_oflw,
#if EBUG
	       stdout
#else
	       NULL
#endif
	       ); 

    empty2attr = (struct empty2attr*) sxalloc (XxY_size (empty_hd) + 1,
					       sizeof (struct empty2attr));


    XxY_alloc (&depend_hd, "depend_hd", limitem, 1, 1, 0, depend_oflw,
#if EBUG
	       stdout
#else
	       NULL
#endif
	       ); 

    tbp_stack = SS_alloc (XxY_size (depend_hd));
    tbp_set = sxba_calloc (XxY_size (depend_hd) + 1);

    is_epsilon = false;
    /* calcul de left_corner */
    for (xprod = 1; xprod <= bnf_ag.WS_TBL_SIZE.nbpro; xprod++) {
	item = bnf_ag.WS_NBPRO [xprod].prolon;
	B = bnf_ag.WS_INDPRO [item].lispro;

	if (B >= 0) {
	    A = bnf_ag.WS_NBPRO [xprod].reduc;
	    A = XNT_TO_NT_CODE (A);

	    if (B > 0) {
		B = XNT_TO_NT_CODE (B);
		SXBA_1_bit (left_corner [B], A); /* On stocke la relation inverse. */
		SXBA_1_bit (left_corner_star [A], B);
	    }
	    else {
		is_epsilon = true;
		SXBA_1_bit (is_epsilon_nt, A);
	    }
	}
    }

    fermer (left_corner_star, bnf_ag.WS_TBL_SIZE.ntmax + 1);

    for (nt = 1; nt <= bnf_ag.WS_TBL_SIZE.ntmax; nt++)
	SXBA_1_bit (left_corner_star [nt], nt);

    /* Majorants de la taille du genere. */
    Mt_items = limitem; /* codes erreur */
    Mnt_items = 0;

    /* Calcul de LALC1 */
    state_stack [state_stack_top = 0] = 1; /* item initial */

    while (state_stack_top >= 0) {
	state = state_stack [state_stack_top--];
	tnt = bnf_ag.WS_INDPRO [state].lispro;

	if (tnt != 0) {
	    next_state = state + 1;
	    XxY_set (&LALC1, state, next_state, &trans);

	    if (SXBA_bit_is_reset_set (state_set, next_state))
		state_stack [++state_stack_top] = next_state;

	    if (tnt > 0) {
		tnt = XNT_TO_NT_CODE (tnt);
		Mnt_items++;
		line = left_corner_star [tnt];
		nt = 0;

		while ((nt = sxba_scan (line, nt)) > 0) {
		    lim = bnf_ag.WS_NTMAX [nt + 1].npg;

		    for (i = bnf_ag.WS_NTMAX [nt].npg; i < lim; i++) {
			xprod = bnf_ag.WS_NBPRO [i].numpg;
			item = bnf_ag.WS_NBPRO [xprod].prolon;
			tnt = bnf_ag.WS_INDPRO [item].lispro;
		
			if (tnt == 0) {
			    /* Reduction vide, il faut s'en souvenir */
			    SXBA_1_bit (has_epsilon_rule, state);
			    XxY_set (&empty_hd, state, xprod, &empty);
			}
			else {
			    next_state = item + 1;
			    XxY_set (&LALC1, state, next_state, &trans);

			    if (tnt > 0)
				Mnt_items++;
			    else
				Mt_items++;

			    if (SXBA_bit_is_reset_set (state_set, next_state))
				state_stack [++state_stack_top] = next_state;
			}
		    }
		}
	    }
	    else
		Mt_items++;
	}
    }

/* On traite les productions vides. */
    if (is_epsilon) {
	state = 0;

	while ((state = sxba_scan (has_epsilon_rule, state)) > 0) {
	    XxY_Xforeach (empty_hd, state, empty) {
		xprod = XxY_Y (empty_hd, empty);
		nt = bnf_ag.WS_NBPRO [xprod].reduc;
		empty2attr [empty].la = nt_look_ahead (state, nt);
	    }
	    
	    /* On pourrait etre plus precis... */
	    Mt_items += bnf_ag.WS_TBL_SIZE.xtmax;
	}
    }

    if (k_value > 0) {
	/* Recherche des etats LC qui ont besoin de look-ahead.  Ils en ont besoin ssi
	   ils se trouvent en // avec d'autres etats (shift ou reduce). */
	look_ahead_needed = false;

	for (xprod = 1; xprod <= bnf_ag.WS_TBL_SIZE.nbpro; xprod++) {
	    if (!SXBA_bit_is_set (prod_set, xprod)) {
		/* forward () a deja pu en trouver. */
		item = bnf_ag.WS_NBPRO [xprod].prolon;
		tnt = bnf_ag.WS_INDPRO [item].lispro;

		if (tnt != 0) {
		    /* Seules les productions non vides nous interessent */
		    /* A -> tnt . alpha est un item qui a une occurrence unique ds l'automate LC(0)
		       (C'est le noyau de l'etat A -> tnt . alpha). Le chemin de A -> tnt . alpha a
		       A -> tnt alpha . (etat Reduce xprod) est donc unique. */
		    XxY_clear (&forward_hd);

		    XxY_Yforeach (LALC1, item + 1, trans) {
			state = XxY_X (LALC1, trans);

			if (forward (state, item)) {
			    look_ahead_needed = true;
			    SXBA_bit_is_reset_set (prod_set, xprod);
			}
		    }
		}
	    }
	}

	if (look_ahead_needed) {
	    /* calcul du look-ahead associe' aux seuls items necessaires au calcul du look-ahead
	       des etats finals. */
    
	    /* Les ensembles solutions correspondent aux plus petits sous-ensembles
	       verifiant les conditions. Pour ne pas risquer de boucler, on procede
	       comme indique ci-dessous. */

	    /* On procede en deux passes :
	       - la premiere passe calcule les look-ahead des couples utiles (nt1, nt2) ou
	         nt2 est ds le left corner de nt1 et note de plus si on a besoin de contexte
		 herite de nt1 (ntXnt2attr [(nt1, nt2)].is_nullable). Elle note de plus les
		 couples k = (state, nt) --- nt est le nt du noyau de state --- qu'ils faudra
		 calculer au cours de la seconde passe. On note egalement les dependances
		 (k depend de (n1, nt2), ... et de k1)
	       - La seconde passe calcule les ki. Le calcule est interrompu lorsque la
	         stabilite est atteinte.
	    */

	    xprod = 0;

	    while ((xprod = sxba_scan (prod_set, xprod)) > 0) {
		lhs = bnf_ag.WS_NBPRO [xprod].reduc;
		item = bnf_ag.WS_NBPRO [xprod].prolon;
		state = bnf_ag.WS_NBPRO [xprod + 1].prolon - 1;
		XxY_set (&stateXlhs, state, 0, &la);

		XxY_Yforeach (LALC1, item + 1, trans) {
		    state = XxY_X (LALC1, trans);
		    ntla = nt_look_ahead (state, lhs);

		    XxY_set (&depend_hd, ntla, la, &depend);

		    if (SXBA_bit_is_reset_set (tbp_set, ntla))
			SS_push (tbp_stack, ntla);
		}

		/* On pourrait prendre sxba_cardinal (stateXlhs2la_set [la]) + 1. */
		Mt_items += bnf_ag.WS_TBL_SIZE.xtmax;
	    }
	}
    }

    /* tbp_stack a pu etre positionne par les productions vides */
    if (!SS_is_empty (tbp_stack)) {
	work_set_local = stateXlhs2la_set [0];

	while (!SS_is_empty (tbp_stack)) {
	    prev = SS_pop (tbp_stack);
	    SXBA_0_bit (tbp_set, prev);
	    prev_set = stateXlhs2la_set [prev];

	    XxY_Xforeach (depend_hd, prev, depend) {
		next = XxY_Y (depend_hd, depend);
		next_set = stateXlhs2la_set [next];

		/* Si prev_set <= next_set => localement stable */
		sxba_copy (work_set_local, prev_set);
		sxba_minus (work_set_local, next_set);

		if (!sxba_is_empty (work_set_local)) {
		    /* non inclus */
		    sxba_or (next_set, prev_set);
			
		    if (SXBA_bit_is_reset_set (tbp_set, next))
			SS_push (tbp_stack, next);
		}
	    }
	}
    }

    XxY_free (&depend_hd);
    SS_free (tbp_stack);
    sxfree (tbp_set);

    reached_set = sxba_calloc (limitem + 1);

    if (sxverbosep)
	sxtime (SXACTION, "done");

    if (is_automaton) {
	if (!is_listing_opened)
	    open_listing (1, language_name, constructor_name, ".la.l");

	if (is_listing_opened) {
	    SXINT		prev_item;

	    if (sxverbosep)
		fputs ("\tAutomaton Listing Output ... ", sxtty);

	    put_edit (1, "*****        A U T O M A T O N       *****");

	    for (state = 1; state < limitem; state++) {
		if (state != 3 &&
		    (state == 1 ||
		     bnf_ag.WS_NBPRO [bnf_ag.WS_INDPRO [state].prolis].prolon != state)) {

		    put_edit_nl (6);
		    put_edit_nnl (1, "STATE ");
		    put_edit_apnb (state);

		    if (state != 1) {
			put_edit_nnl (31, "Transition on ");
			z = bnf_ag.WS_INDPRO [state - 1].lispro; 
			put_edit_apnnl ((z > 0) ? get_nt_string (z) : xt_to_str (z));
			put_edit_apnnl (" from ");
			n = 0;
			sxinitialise (prev_state); /* pour faire taire gcc -Wuninitialized */

			XxY_Yforeach (LALC1, state, x) {
			    if (++n > 1)
				break /* ont sort du foreach */;

			    prev_state = XxY_X (LALC1, x);
			}

			if (n == 1) {
			    put_edit_apnnl ("state ");
			    put_edit_apnb (prev_state);
			    put_edit_nl (1);
			}
			else {
			    put_edit_ap ("states:");
			    put_edit_nb (11, prev_state);
			    n = 0;

			    XxY_Yforeach (LALC1, state, x) {
				if (++n > 1) {
				    put_edit_apnnl (", ");
				    prev_state = XxY_X (LALC1, x);

				    if (put_edit_get_col () >= 81)
					put_edit_nb (11, prev_state);
				    else
					put_edit_apnb (prev_state);
				}
			    }
			}

			if (k_value > 0 &&
			    bnf_ag.WS_INDPRO [state].lispro == 0 &&
			    (xprod = bnf_ag.WS_INDPRO [state].prolis) &&
			    SXBA_bit_is_set (prod_set, xprod)) {
			    /* On sort la liste des etats atteints en // avec state. */
			    item = bnf_ag.WS_NBPRO [xprod].prolon;
			    XxY_clear (&forward_hd);

			    XxY_Yforeach (LALC1, item + 1, trans) {
				prev_item = XxY_X (LALC1, trans);
				walk_forward (prev_item, item, set_reached_set);
			    }

			    SXBA_0_bit (reached_set, state);
			    n = sxba_cardinal (reached_set);

			    put_edit_nnl (31, "Reached in // with ");

			    prev_state = sxba_scan_reset (reached_set, 0);

			    if (n == 1) {
				put_edit_apnnl ("state ");
				put_edit_apnb (prev_state);
				put_edit_nl (1);
			    }
			    else {
				put_edit_ap ("states:");
				put_edit_nb (11, prev_state);
				n = 0;

				while ((prev_state = sxba_scan_reset (reached_set, prev_state)) > 0) {
				    put_edit_apnnl (", ");

				    if (put_edit_get_col () >= 81)
					put_edit_nb (11, prev_state);
				    else
					put_edit_apnb (prev_state);
				}
			    }
			}
		    }

		    print_state (state);
		}
	    }

	    if (sxverbosep)
		sxtime (SXACTION, "done");
	}
    }

    tfe ();
    ntfe ();

    if (sxverbosep || is_listing_opened) {
	if (nd_state_set != NULL) {
	    static char mess [] = "The generated push-down automaton is nondeterministic\n";

	    if (sxverbosep)
		fputs (mess, sxtty);
	    if (is_listing_opened) {
		fputs (mess, listing);
		put_edit_nl (2);
	    }
	}
    }

    sxfree (reached_set);
    sxbm_free (left_corner);
    sxbm_free (left_corner_star);
    XxY_free (&ntXnt);
    sxbm_free (ntXnt2la_set);
    sxfree (ntXnt2attr);
    XxY_free (&stateXlhs);
    sxbm_free (stateXlhs2la_set);
    sxfree (state_set);
    sxfree (state_stack);
    sxfree (has_epsilon_rule);
    sxfree (is_epsilon_nt);
    XxY_free (&empty_hd);
    sxfree (empty2attr);

    XxY_free (&forward_hd);

    XxY_free (&LALC1);

    {

/* On verifie que toutes les reductions sont generees.
   Le traitement des conflits ayant pu en supprimer. */

	SXINT	LC0_i;

	if ((LC0_i = sxba_scan (not_used_red_set, 0)) > 0) {
	    /* Il y a des reductions non generees */
	    static char		ws_local [] = "\nWarning :\tThe following grammar rules are not recognized:\n";

	    fputs (ws_local, sxstderr);

	    if (is_listing_opened) {
		put_edit_apnnl (ws_local);
	    }

	    do {
		prod_to_str (vstr, LC0_i);
		fputs (varstr_tostr (vstr), sxstderr);

		if (is_listing_opened)
		    put_edit_apnnl (varstr_tostr (vstr));
	    } while ((LC0_i = sxba_scan (not_used_red_set, LC0_i)) > 0);

	    sxttycol1p = true;
	}

	sxfree (not_used_red_set);
    }

    {
	/* On alloue et initialise certaines structures de OPTIM pour pouvoir sortir le
	   Floyd_Evans si long_listing a ete demande */

	SXINT	LC0_i_2;

	t_state_equiv_class = (SXINT*) sxalloc (limitem + 1, sizeof (SXINT));
	nt_state_equiv_class = (SXINT*) sxalloc (limitem + 1, sizeof (SXINT));
	nt_to_ad = (SXINT*) sxcalloc (bnf_ag.WS_TBL_SIZE.xntmax + 1, sizeof (SXINT));
	nt_to_nt = (SXINT*) sxalloc (bnf_ag.WS_TBL_SIZE.xntmax + 1, sizeof (SXINT));
	NTMAX = bnf_ag.WS_TBL_SIZE.ntmax;

	for (LC0_i_2 = 0; LC0_i_2 <= limitem; LC0_i_2++)
	    t_state_equiv_class [LC0_i_2] = (t_states [LC0_i_2].lgth == 0) ? 0 : LC0_i_2;

	for (LC0_i_2 = 0; LC0_i_2 <= limitem; LC0_i_2++)
	    nt_state_equiv_class [LC0_i_2] = (nt_states [LC0_i_2].lgth == 0) ? 0 : LC0_i_2;

	for (LC0_i_2 = 1; LC0_i_2 <= bnf_ag.WS_TBL_SIZE.xntmax; LC0_i_2++) {
	    nt_to_nt [LC0_i_2] = LC0_i_2;
	}
    }

    if (is_long_listing) {
	if (!is_listing_opened)
	    open_listing (1, language_name, ME, ".la.l");

	if (is_listing_opened) {
	    struct state	*astate;
	    struct P_item	*aitem, *LC0_lim;
	    SXINT			LC0_i_3;

	    if (sxverbosep)
		fprintf (sxtty, "\tNon Optimized Floyd-Evans Listing Output ... ");

	    put_edit_ap ("************ T _ T A B L E S *************");
	    put_edit_nl (2);
	    put_edit_nnl (11, "Parsing starts in Etq ");
	    put_edit_apnb (init_state);
	    put_edit_nl (2);
	    put_edit_nnl (11, "size = ");
	    put_edit_apnb (t_lnks [0]);
	    put_edit_nl (2);

	    for (LC0_i_3 = 1; LC0_i_3 <= limitem; LC0_i_3++) {
		astate = t_states + LC0_i_3;

		if (astate->lgth != 0 /* etat non vide */ ) {
		    put_edit_nl (1);
		    put_edit_nnl (1, "Etq ");
		    put_edit_apnb (LC0_i_3);

		    for (LC0_lim = (aitem = t_items + astate->start) + astate->lgth;
			 aitem < LC0_lim;
			 aitem++) {
			if (aitem->action != 0)
			    output_fe (Terminal,
				       aitem->action,
				       aitem->check,
				       t_fe + aitem->action);
		    }
		}
	    }

	    put_edit_ap ("************ N T _ T A B L E S *************");
	    put_edit_nl (2);
	    put_edit_nnl (11, "size = ");
	    put_edit_apnb (nt_lnks [0]);
	    put_edit_nl (2);

	    for (LC0_i_3 = 1; LC0_i_3 <= limitem; LC0_i_3++) {
		astate = nt_states + LC0_i_3;

		if (astate->lgth != 0 /* etat non vide */ ) {
		    put_edit_nl (1);
		    put_edit_nnl (1, "State ");
		    put_edit_apnb (LC0_i_3);

		    for (LC0_lim = (aitem = nt_items + astate->start) + astate->lgth;
			 aitem < LC0_lim;
			 aitem++) {
			if (aitem->action != 0)
			    output_fe (NonTerminal,
				       aitem->action,
				       aitem->check,
				       nt_fe + aitem->action);
		    }
		}
	    }

	    if (sxverbosep)
		sxtime (SXACTION, "done");
	}
    }

    if (is_listing_opened) {
	put_edit_finalize ();
	is_listing_opened = false;
    }

    final_state = 2; /* 0: ? = 1 <AXIOME> 2 EOF 3 ; */

    is_non_deterministic_automaton = true;

    /* POUR OPTIM */
    xe1 = (xac2 = init_state) + 1;

    /* On n'elimine pas les productions simples car ca peut poser des PB...
       Ex : si A -> B est elimine d'une nt_table et s'il y a plusieurs transitions
       sur A... */
    is_pspe = is_tspe = false;

    return 0;
}

