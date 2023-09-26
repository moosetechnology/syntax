/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1984 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *     Produit de l'equipe Langages et Traducteurs.     *
   *                                                      *
   ******************************************************** */



/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030516 13:07 (phd):	Suppression de #ifdef MSDOS		*/
/************************************************************************/
/* 20030512 13:07 (phd):	Adaptation à SGI (64 bits)		*/
/************************************************************************/
/* Jeu 28 Mar 2002 (pb):      Adaptation de la 3.9 aux B_tables de la 5	*/
/************************************************************************/
/* Ven 26 Nov 1999 10:43(pb):	Remplacement de state_nt_to_items par	*/
/*				la XxY STATExNT_hd			*/
/************************************************************************/
/* Mer 24 Nov 1999 11:26(pb):	On genere l'instruction Error ds nt_fe  */
/*				a l'index t_error			*/
/************************************************************************/
/* 09-02-89 18:05 (pb):		Ajout de majc1 (gros BUG)		*/
/************************************************************************/
/* 14-12-88 10:35 (pb):		Passage en LR(1)			*/
/************************************************************************/
/* 24-08-88 18:53 (bl):		Ajout de #ifdef MSDOS			*/
/************************************************************************/
/* 22-06-88 15:55 (pb):		Reecriture complete du calcul des  	*/
/*				look-ahead sets: gain impressionnant	*/
/************************************************************************/
/* 19-04-88 10:32 (pb):		Message de non conformite avec path  	*/
/*				(option is_path et fsa_to_re)		*/
/************************************************************************/
/* 15-02-88 10:32 (pb):		Adaptation a XNT		  	*/
/************************************************************************/
/* 02-02-88 10:50 (pb):		Adaptation a V3.0 et V3.1 (-tspe)  	*/
/************************************************************************/
/* 20-03-87 17:45 (phd):	Utilisation de put_edit_finalize  	*/
/************************************************************************/
/* 05-02-87 10:15 (phd):	Revision des sorties de messages  	*/
/************************************************************************/
/* 04-02-87 14:07 (phd):	Amelioration des messages au terminal  	*/
/************************************************************************/
/* 28-01-87 09:05 (pb):		Options "luc", "lsc", "lc"	  	*/
/************************************************************************/
/* 31-10-86 14:50 (phd&pb):	Correction d'une bogue dans les sorties	*/
/************************************************************************/
/* 30-10-86 11:05 (phd):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/

static char	ME [] = "LALR1";

/*************************************************************/
/*                                                           */
/*                                                           */
/*  This program has been translated from LALR1.pl1          */
/*  on Tuesday the twenty-seventh of May, 1986, at 9:13:14,  */
/*  on the Multics system at INRIA,                          */
/*  by the release 3.3g PL1_C translator of INRIA,           */
/*         developped by the "Langages et Traducteurs" team, */
/*         using the SYNTAX (*), FNC and Paradis systems.    */
/*                                                           */
/*                                                           */
/*************************************************************/
/* (*) SYNTAX is a trademark of INRIA.                       */
/*************************************************************/

/* Modifications:
  
Release 2.0 10/29/84:
Shift_Reduce conflict, priority is given to Shift
Reduce_Reduce conflict, priority is given to the first reduction
The unused paths are not erased
The call to report_non_LR_ness is replaced by a call to ioa_
assoc$set is replaced by value_$set
  
*/



#include "P_tables.h"

#include "csynt_optim.h"

#include "D_tables.h"
char WHAT_LALR1[] = "@(#)SYNTAX - $Id: LALR1.c 1103 2008-02-25 15:22:05Z rlacroix $" WHAT_DEBUG;

extern BOOLEAN	prio_read (SXINT t_priorities_size, 
			   SXINT r_priorities_size,
			   struct priority **t_priorities,
			   struct priority **r_priorities,
			   char *langname),
                prio_free (struct priority **t_priorities, struct priority **r_priorities);
extern VARSTR	prod_to_str (VARSTR prod_to_str_vstr, SXINT prod);
extern VARSTR	path (SXINT init, SXINT final);
extern BOOLEAN	check_lr1 (SXINT xac1, SXBA t_set, SXINT nt_trans1, SXINT nt_trans2, BOOLEAN is_duplicate, BOOLEAN is_sample_path);
extern BOOLEAN	check_ambig (SXINT t, SXINT item, SXINT nt_trans);
extern BOOLEAN	open_listing (SXINT kind, char *language_name, char *ME_open_listing, char *component_name);
extern VARSTR	cat_t_string (VARSTR cat_t_string_vstr, SXINT code);
extern char	*laset (VARSTR varstring, SXINT maxi, SXBA look_ahead_set);
static struct priority	*t_priorities /* 1:xtmax */ ;
static struct priority	*r_priorities /* 1:xnbpro */ ;
static struct H1 {
	   SXINT	hshr, hshat, hsht, hshant, hshnt;
       }	*H1 /* -31:lgt (1) */ ;
static struct S3 {
	   SXINT	pkod, kod;
       }	*S3 /* 0:lgt (3) */ ;
static SXINT	*tr /* 1:lgt (4) */ ;
static SXINT	*ptrans /* 1:lgt (7) */ ;
static char	star_71 [] = "***********************************************************************";
static BOOLEAN	is_lalr1, is_lr1, is_ambiguous;
static SXINT	limitem, limt, limnt, limpro;
static SXINT	lgt [13];
static SXINT	usr, ssr, urr, srr;
static SXINT	xtr2, xlstprd, xptr;
static SXINT	xlas, xplxi, Mt_items, Mnt_items;
static SXINT	*item /* 0:indpro */ ;
static SXINT	xptr2;
static SXINT	*tsymb1 /* limt:xntmax */ ;
static SXINT	*tsymb2 /* limt:xntmax */ ;
static SXINT	xpter, xpconf, xkod;
static SXINT	plulong;
static SXINT	*pter /* 1:xtmax */ ;
static SXINT	*pconf /* 1:xtmax */ ;
static SXINT	*ter /* -xtmax:-1 */ ;
static SXINT	*conf /* -xtmax:-1 */ ;
static SXINT	*plxi /* 1:xtr2 */ ;
static SXINT	*nbt /* 0:max_red_per_state-1 */ ;
static SXBA	/* xtmax */ *look_ahead_sets /* 1:reduce_item_no */ ;
static SXBA	/* nbpro */ not_used_red_set;
static SXBA	/* nbpro */ s_red_set, /* xtmax */ t_set;
static struct {
	   SXINT	conflict_kind /* SHIFT_REDUCE, REDUCE_REDUCE */ ;
	   SXINT	desambig_rule_kind /* USER, SYSTEM */ ;
	   SXINT	action_taken
	   /* SHIFT, REDUCE, ERROR_ACTION,
        SYSTEM_ACTION */
			    ;
	   SXINT	reduce_no;
	   struct priority	prio;
       }	chosen;
static SXINT	max_red_per_state, cur_red_no, base_xlas;
/* static SXBA	*FIRST; *NV* */
static SXINT	xor_hash_size;
static SXINT	*xor_hash;
static SXINT	**Titems_set, *items_set;

#define UNDEF_CONFLICT ((SXINT)0)
#define SHIFT_REDUCE ((SXINT)1)
#define REDUCE_REDUCE ((SXINT)2)
#define UNDEF_ACTION ((SXINT)0)
#define SHIFT ((SXINT)1)
#define REDUCE ((SXINT)2)
#define ERROR_ACTION ((SXINT)3)
#define SYSTEM_ACTION ((SXINT)4)
#define UNDEF_DESAMBIG_RULE ((SXINT)0)
#define USER ((SXINT)1)
#define SYSTEM ((SXINT)2)
#define UNDEF_ASSOC ((SXINT)0)
#define LEFT_ASSOC ((SXINT)1)
#define RIGHT_ASSOC ((SXINT)2)
#define NON_ASSOC ((SXINT)3)

static struct priority	UNDEF_PRIO = {0, 0};
static struct priority	current_prio;
static SXINT	min_reduce;
static BOOLEAN	is_error_code_needed;
static char	header [] = "\
Conflicts | Shift-Reduce  | Reduce-Reduce\n\
-----------------------------------------\n\
          | User | System | User | System\n\
          -------------------------------\n";

static void
oflw_STATExNT2tnt (SXINT old_size, SXINT new_size)
{
    sxuse(old_size); /* pour faire taire gcc -Wunused */
    STATExNT2tnt = (SXINT*) sxrealloc (STATExNT2tnt, new_size + 1, sizeof (SXINT));
}



#define maj(x,n)	if (++*(x) > lgt [n]) sature (n); else ((void)0)
#define majc1(x)	if (++*(x) > lgt [1]) {sature ((SXINT)1); aS1xac1=S1+xac1;} else ((void)0)

static SXVOID	sature (SXINT sature_nbt)
{
    SXINT	old_size, new_size;
    sxinitialise(new_size); /* pour faire taire gcc -Wuninitialized */

    if (sature_nbt > 0) {
	lgt [sature_nbt] = new_size = 2 * (old_size = lgt [sature_nbt]);

	if (sxverbosep) {
	    sat_mess (ME, sature_nbt, old_size, new_size);
	}
    }

    switch (sature_nbt) {
    case 0:
	/* initialisations */
	S1 = (struct S1*) sxalloc (lgt [1] + 1, sizeof (struct S1));
	H1 = (struct H1*) sxcalloc (lgt [1] + 32, sizeof (struct H1)) + 31;
	xor_StNts = (struct xor_StNt*) sxalloc (lgt [2] + 1, sizeof (struct xor_StNt));
	S3 = (struct S3*) sxalloc (lgt [3] + 1, sizeof (struct S3));
	tr = (SXINT*) sxalloc (lgt [4] + 1, sizeof (SXINT));
	includes = (struct include*) sxalloc (lgt [5] + 1, sizeof (struct include));
	lstprd = (struct lstprd*) sxalloc (lgt [6] + 1, sizeof (struct lstprd));
	ptrans = (SXINT*) sxalloc (lgt [7] + 1, sizeof (SXINT));
	tt = (SXINT*) sxalloc (lgt [8] + 1, sizeof (SXINT));
	tat = (SXINT*) sxalloc (lgt [9] + 1, sizeof (SXINT));
	tant = (SXINT*) sxalloc (lgt [10] + 1, sizeof (SXINT));
	tnt = (SXINT*) sxalloc (lgt [11] + 1, sizeof (SXINT));
	tnt_next = (SXINT*) sxalloc (lgt [11] + 1, sizeof (SXINT));
	nucl = (SXINT*) sxalloc (lgt [12] + 1, sizeof (SXINT));
	break;

    case 1:
	S1 = (struct S1*) sxrealloc (S1, new_size + 1, sizeof (struct S1));
	H1 = (struct H1*) sxrealloc (H1 - 31, new_size + 32, sizeof (struct H1)) + 31;
	break;

    case 2:
	xor_StNts = (struct xor_StNt*) sxrealloc (xor_StNts, new_size + 1, sizeof (struct xor_StNt));
	break;

    case 3:
	S3 = (struct S3*) sxrealloc (S3, new_size + 1, sizeof (struct S3));
	break;

    case 4:
	tr = (SXINT*) sxrealloc (tr, new_size + 1, sizeof (SXINT));
	break;

    case 5:
	includes = (struct include*) sxrealloc (includes, new_size + 1, sizeof (struct include));
	break;

    case 6:
	lstprd = (struct lstprd*) sxrealloc (lstprd, new_size + 1, sizeof (struct lstprd));
	break;

    case 7:
	ptrans = (SXINT*) sxrealloc (ptrans, new_size + 1, sizeof (SXINT));
	break;

    case 8:
	tt = (SXINT*) sxrealloc (tt, new_size + 1, sizeof (SXINT));
	break;

    case 9:
	tat = (SXINT*) sxrealloc (tat, new_size + 1, sizeof (SXINT));
	break;

    case 10:
	tant = (SXINT*) sxrealloc (tant, new_size + 1, sizeof (SXINT));
	break;

    case 11:
	tnt = (SXINT*) sxrealloc (tnt, new_size + 1, sizeof (SXINT));
	tnt_next = (SXINT*) sxrealloc (tnt_next, new_size + 1, sizeof (SXINT));
	break;

    case 12:
	nucl = (SXINT*) sxrealloc (nucl, new_size + 1, sizeof (SXINT));
	break;
    default:
#if EBUG
      sxtrap(ME,"unknown switch case #1");
#endif
      break;
    }
}



VOID	MAJ (SXINT *x, SXINT n)
{
    maj (x, n);
}



VOID	CLEAR (SXINT *T)
{
    SXINT	x, y;

    for (x = T [y = 0]; x > 0; x = T [y = x])
	T [y] = 0;

    T [y] = 0;
    T [0] = -1;
}



void
store_nt_items (SXINT state, SXINT xtnt)
{
    /* Pour chaque etat state qui a des transitions non-terminales sur le non-terminal (etendu) xnt
       on note l'indice xtnt de tnt ou commence la liste des items correspondants.  En fait,
       on le note a partir du couple (state, nt) ou nt est la racine de xnt.  Ca permet
       d'avoir un acces direct en retrouvant tous les xnt d'un etat a partir de leur racine nt */
    SXINT		xnt = bnf_ag.WS_INDPRO [tant [S1 [tnt [xtnt]].ptant]].lispro;
    SXINT 	nt = XNT_TO_NT_CODE (xnt), couple;
    SXINT		next = (XxY_set (&STATExNT_hd, state, nt, &couple)) ? STATExNT2tnt [couple] : 0;

    STATExNT2tnt [couple] = xtnt;
    tnt_next [xtnt] = next;
}



static BOOLEAN	is_old_StNt (SXINT state, SXINT nt, SXINT *StNt)
{
    /* Met dans StNt l'index dans le tableau xor_StNts du couple (state, nt).
       Si ce couple n'existait pas, il est insere et la fonction retourne FALSE */

    /* Pour un etat state donne les couples (state, nt) sont lies depuis S1[state].Stlnk
       a travers xor_StNts[].Stlnk */

    /* Un item etendu est un couple (state, nt) ou nt est un element marque de state auquel
       la fonction is_old_StNt a associe un entier StNt > 0 tel que:
       Soit (state, nt) ==> StNt et (state', nt') => StNt' on a
          (state, nt) == (state', nt') <=> StNt == StNt'
	  De plus connaissant StNt on peut trouver state et nt
	  xor_StNts [StNt].state et xor_StNts [StNt].nt */
	  
    SXINT	x, y, hash;
    struct xor_StNt	*aStNt;

    for (x = xor_hash [hash = state ^ nt], y = 0; x != 0; x = xor_StNts [y = x].lnk) {
	if (xor_StNts [x].state == state) {
	    *StNt = x;
	    return TRUE;
	}
    }

    maj (&xxor_StNt, (SXINT)2);
    aStNt = xor_StNts + xxor_StNt;
    aStNt->lnk = 0;
    aStNt->state = state;
    aStNt->xptant = 0;
    aStNt->X1_head = 0;
    aStNt->Stlnk = S1 [state].StNt;
    S1 [state].StNt = xxor_StNt;

    if (y == 0)
	xor_hash [hash] = xxor_StNt;
    else
	xor_StNts [y].lnk = xxor_StNt;

    *StNt = xxor_StNt;
    return FALSE;
}



static VOID	put_newr_in_includes (SXINT X1, SXINT X2)
{
    /* Ajoute en tete l'element (X1, X2) --Il n'existe pas deja-- a la relation includes
       et lie entre eux les elements ayant meme X1 */

    struct include	*aelem;

    maj (&xincludes, (SXINT)5);
    aelem = includes + xincludes;
    aelem->X2 = X2;
    aelem->lnk = xor_StNts [X1].X1_head;
    xor_StNts [X1].X1_head = xincludes;
}



static VOID	put_r_in_includes (SXINT X1, SXINT X2)
{
    /* Ajoute l'element (X1, X2) --s'il n'existe pas deja-- a la relation includes
       Les deuxiemes champs sont tries pa r valeur decroissante */

    SXINT	x, y, F2;
    struct include	*aelem;

    for (x = xor_StNts [X1].X1_head, y = 0; x != 0; x = includes [y = x].lnk) {
	F2 = includes [x].X2;

	if (F2 <= X2) {
	    if (F2 == X2)
		return;

	    break;
	}
    }

    maj (&xincludes, (SXINT)5);
    aelem = includes + xincludes;
    aelem->X2 = X2;
    aelem->lnk = x;

    if (y == 0)
	xor_StNts [X1].X1_head = xincludes;
    else
	includes [y].lnk = xincludes;
}



#if 0
SXINT	state_nt_to_items (SXINT state, SXINT nt)
{
    /* Recherche dans l'etat "state" la liste des items dont le marqueur LR repere
       le non-terminal "nt" */
    /* Recherche par dichotomie */
    /* Suppose que "nt" existe */

    SXINT	bottom, top, xtnt, mid_nt;

    bottom = S1 [xtnt = S1 [state].shftnt].ptnt;
    top = S1 [xtnt + 1].ptnt - 1;

    while (bottom < top) {
	xtnt = (bottom + top) >> 1;
	mid_nt = bnf_ag.WS_INDPRO [tant [S1 [tnt [xtnt]].ptant]].lispro;

	if (nt <= XNT_TO_NT_CODE (mid_nt))
	    top = xtnt;
	else
	    bottom = xtnt + 1;
    }

    return tnt [top];
}



static VOID	lookback (SXINT state, SXINT item, SXINT StNt)
{
    /* Cette fonction recursive calcule la relation binaire includes entre des couples
       (StNt, StNt1) ou chaque StNt est un couple (etat, nt)
       StNt = (s, nt) includes StNt1 = (s1, nt1) ou
       "nt -> . alpha" et "nt1 -> delta . nt gamma" sont dans s
       s = goto*(s1, delta), et gamma =>* epsilon */

    if (bnf_ag.WS_INDPRO [item - 1].lispro != 0 /* item du kernel A -> alpha X .beta */ ) {
	/* Calcul des etats predecesseurs de state (sur X) */
	SXINT	x;

	for (x = S1 [state].pprd; x != 0; x = lstprd [x].f2) {
	    lookback (lstprd [x].f1, item - 1, StNt);
	}
    }
    else /* item de la fermeture: A -> . beta */  {
	/* On ajoute (StNt, (state, A)) a la relation includes */
	SXINT	nt;
	SXINT	StNt1;

	nt /* A */  = bnf_ag.WS_NBPRO [bnf_ag.WS_INDPRO [item].prolis].reduc;

	if (!is_old_StNt (state, nt, &StNt1 /* (state, A) */ )) {
	    /* C'est la premiere fois qu'on rencontre StNt1 = (state, nt)
	       (StNt, StNt1) est obligatoirement nouveau
	       on calcule donc ses predecesseurs */

	    SXINT	xtant, lim;

	    put_newr_in_includes (StNt, StNt1);

	    {
		/* Recherche des items de l'etat state ayant le marqueur LR devant A */
		SXINT	xptant;

		xptant = state_nt_to_items (state, nt);
		xor_StNts [StNt1].xptant = xptant;
		xtant = S1 [xptant].ptant;
		lim = S1 [xptant + 1].ptant;
	    }

	    while (xtant < lim /* B -> delta .A gamma */ ) {
		/* gamma derive-t-il dans le vide? */
		if (sxba_bit_is_set (bnf_ag.NULLABLE, (item = tant [xtant++]) + 1))
		    lookback (state, item, StNt1);
	    }
	}
	else
	    put_r_in_includes (StNt, StNt1);
    }
}
#endif

static VOID	lookback (SXINT state, SXINT lookback_item, SXINT StNt)
{
    /* Cette fonction recursive calcule la relation binaire includes entre des couples
       (StNt, StNt1) ou chaque StNt est un couple (etat, nt)
       StNt = (s, nt) includes StNt1 = (s1, nt1) ou
       "nt -> . alpha" et "nt1 -> delta . nt gamma" sont dans s
       s = goto*(s1, delta), et gamma =>* epsilon */
    SXINT				x, nt, xnt, StNt1, couple, xtant, lim, lookback_xptant;

    if (bnf_ag.WS_INDPRO [lookback_item - 1].lispro != 0 /* item du kernel A -> alpha X .beta */ ) {
	/* Calcul des etats predecesseurs de state (sur X) */
	for (x = S1 [state].pprd; x != 0; x = lstprd [x].f2) {
	    lookback (lstprd [x].f1, lookback_item - 1, StNt);
	}
    }
    else /* item de la fermeture: A -> . beta */  {
	/* On ajoute (StNt, (state, A)) a la relation includes */
	nt /* A */  = bnf_ag.WS_NBPRO [bnf_ag.WS_INDPRO [lookback_item].prolis].reduc;
	couple = XxY_is_set (&STATExNT_hd, state, nt); /* Should be > 0 */

#if EBUG
	if (couple == 0)
	    sxtrap (ME, "lookback");
#endif

	for (x = STATExNT2tnt [couple]; x != 0; x = tnt_next [x]) {
	    /* Parcours des items de l'etat state ayant le marqueur LR devant A&i */
	    lookback_xptant = tnt [x];
	    xtant = S1 [lookback_xptant].ptant;
	    xnt = bnf_ag.WS_INDPRO [tant [xtant]].lispro;

	    if (!is_old_StNt (state, xnt, &StNt1 /* (state, A) */ )) {
		/* C'est la premiere fois qu'on rencontre StNt1 = (state, nt)
		   (StNt, StNt1) est obligatoirement nouveau
		   on calcule donc ses predecesseurs */
		put_newr_in_includes (StNt, StNt1);

		xor_StNts [StNt1].xptant = lookback_xptant;
		lim = S1 [lookback_xptant + 1].ptant;

		while (xtant < lim /* B -> delta .A gamma */ ) {
		    /* gamma derive-t-il dans le vide? */
		    if (sxba_bit_is_set (bnf_ag.NULLABLE, (lookback_item = tant [xtant++]) + 1))
			lookback (state, lookback_item, StNt1);
		}
	    }
	    else
		put_r_in_includes (StNt, StNt1);
	}
    }
}



VOID	compute_includes (SXINT xac1)
{
    SXINT	y, lim;

    y = S1 [xac1].pred;
    lim = S1 [y + 1].ptr;

    if ((y = S1 [y].ptr) < lim) {
	SXINT	ntbidon = bnf_ag.WS_TBL_SIZE.xntmax;
	SXINT	*orig = plxi + xplxi;

	for (; y < lim; y++, orig++, xplxi++) {
	    SXINT	compute_includes_item = tr [y];

	    is_old_StNt (xac1, ++ntbidon, orig) /* FALSE */ ;
	    xor_StNts [*orig].xptant = -compute_includes_item /* permet de retrouver l'origine */ ;
	    lookback (xac1, compute_includes_item, *orig);
	}
    }
}




SXINT	put_in_fe (struct floyd_evans *fe, SXINT *hashs, SXINT *lnks, struct floyd_evans *afe)
{
    SXINT	hash, x, y;
    struct floyd_evans		*afex;

    hash = (afe->codop + afe->reduce + afe->pspl + afe->next) % HASH_SIZE;

    for (y = 0, x = hashs [hash]; x != 0; y = x, x = lnks [x]) {
	afex = fe + x;

	if (afex->next == afe->next && afex->codop == afe->codop && afex->reduce == afe->reduce && afex->pspl == afe->
	     pspl)
	    return x;
    }

    x = ++*lnks /* nouvelle taille de fe */ ;
    fe [x] = *afe;
    lnks [x] = 0;

    if (y == 0)
	hashs [hash] = x;
    else
	lnks [y] = x;

    return x;
}



static SXVOID	filitem (SXINT a)
{
    SXINT	x, y;

    for (y = item [x = 0]; y <= a; y = item [x = y]) {
    }

    item [x] = a;
    item [a] = y;
}



static SXINT	isint (SXINT *t, SXINT kind, SXINT xpt, SXINT hsh)
{
    SXINT		m1, taille, taille1, m;
    SXINT	i, x, y;

    switch (kind) {
    case 1:
	m = S1 [xpt].ptr;
	taille = S1 [xpt + 1].ptr - m;
	break;

    case 2:
	m = S1 [xpt].ptat;
	taille = S1 [xpt + 1].ptat - m;
	break;

    case 3:
	m = S1 [xpt].ptt;
	taille = S1 [xpt + 1].ptt - m;
	break;

    case 4:
	m = S1 [xpt].ptant;
	taille = S1 [xpt + 1].ptant - m;
	break;

    case 5:
	m = S1 [xpt].ptnt;
	taille = S1 [xpt + 1].ptnt - m;
	break;
    default:
#if EBUG
      sxtrap(ME,"unknown switch case #2");
#endif
      sxinitialise(m); /* pour faire taire gcc -Wuninitialized */
      sxinitialise(taille); /* pour faire taire gcc -Wuninitialized */
      break;
    }

    if (taille == 0)
	return 0;
    else {
	x = -(hsh & 037);

loop:	switch (kind) {
	case 1:
	    if ((y = H1 [x].hshr) == 0)
		goto new;

	    m1 = S1 [y].ptr;
	    taille1 = S1 [y + 1].ptr - m1;
	    break;

	case 2:
	    if ((y = H1 [x].hshat) == 0)
		goto new;

	    m1 = S1 [y].ptat;
	    taille1 = S1 [y + 1].ptat - m1;
	    break;

	case 3:
	    if ((y = H1 [x].hsht) == 0)
		goto new;

	    m1 = S1 [y].ptt;
	    taille1 = S1 [y + 1].ptt - m1;
	    break;

	case 4:
	    if ((y = H1 [x].hshant) == 0)
		goto new;

	    m1 = S1 [y].ptant;
	    taille1 = S1 [y + 1].ptant - m1;
	    break;

	case 5:
	    if ((y = H1 [x].hshnt) == 0)
		goto new;

	    m1 = S1 [y].ptnt;
	    taille1 = S1 [y + 1].ptnt - m1;
	    break;
        default:
#if EBUG
            sxtrap(ME,"unknown switch case #3");
#endif
	    sxinitialise(taille1); /* pour faire taire gcc -Wuninitialized */
	    sxinitialise(m1); /* pour faire taire gcc -Wuninitialized */
	    sxinitialise(y); /* pour faire taire gcc -Wuninitialized */
            break;
	}

	if (taille1 == taille) {
	    for (i = taille - 1; i >= 0; i--) {
		if (t [m + i] != t [m1 + i])
		    goto different;
	    }

	    return y;
	}

different:
	x = y;
	goto loop;

new:	switch (kind) {
	case 1:
	    H1 [x].hshr = xpt;
	    H1 [xpt].hshr = 0;
	    break;

	case 2:
	    H1 [x].hshat = xpt;
	    H1 [xpt].hshat = 0;
	    break;

	case 3:
	    H1 [x].hsht = xpt;
	    H1 [xpt].hsht = 0;
	    break;

	case 4:
	    H1 [x].hshant = xpt;
	    H1 [xpt].hshant = 0;
	    break;

	case 5:
	    H1 [x].hshnt = xpt;
	    H1 [xpt].hshnt = 0;
	    break;
        default:
#if EBUG
            sxtrap(ME,"unknown switch case #4");
#endif
            break;
	}

	return xpt;
    } /* end isint */
}



static SXVOID	mettre (SXINT a)
{
    SXINT	x, y, z;

    z = bnf_ag.WS_INDPRO [a].lispro;

    if (z != 0) {
	if (z > 0) {
	    if (tsymb1 [z] == limitem) {
		ptrans [xptr2] = z;
		maj (&xptr2, (SXINT)7);
	    }

	    for (y = tsymb2 [x = 0]; y <= z; y = tsymb2 [x = y]) {
	    }
	}
	else
	    for (y = tsymb2 [x = limt]; z <= y; y = tsymb2 [x = y]) {
	    }

	tsymb2 [x] = z;
	tsymb2 [z] = y;
    }

    item [0] = tsymb1 [z];
    filitem (a);
    tsymb1 [z] = item [0];
}



VOID	filprd (SXINT pred, SXINT suc)
{
    maj (&xlstprd, (SXINT)6);
    lstprd [xlstprd].f1 = pred;
    lstprd [xlstprd].f2 = S1 [suc].pprd;
    S1 [suc].pprd = xlstprd; /* end filprd */
}



static SXVOID	sornt (SXINT x)
{
    SXINT		z, suc;
    SXINT	lim, lim1, y;

    put_edit (1, star_71);
    lim = S1 [x + 1].ptnt;

    for (x = S1 [x].ptnt; x < lim; x++) {
	y = tnt [x];
	suc = S1 [y].tsucnt;
	lim1 = S1 [y + 1].ptant;
	y = S1 [y].ptant;
	z = tant [y];


/* if (z >= 4) */

	{
	    char	s1 [12];

	    sprintf (s1, " ---> %ld", suc);
	    soritem (z, 1, 71, "*", s1);

	    for (++y; y < lim1; y++) {
		z = tant [y];
		soritem (z, 1, 71, "*", "");
	    }
	}
    }
}



static SXVOID	sort (SXINT x)
{
    SXINT		lim, lim1;
    SXINT	z, suc, y;
    char	s1 [12];

    put_edit (1, star_71);
    lim = S1 [x + 1].ptt;

    for (x = S1 [x].ptt; x < lim; x++) {
	y = tt [x];
	suc = S1 [y].tsuct;
	lim1 = S1 [y + 1].ptat;
	y = S1 [y].ptat;
	z = tat [y];

	if (bnf_ag.WS_INDPRO [z].lispro == bnf_ag.WS_TBL_SIZE.tmax)
	    strcpy (s1, " ---> HALT");
	else
	    sprintf (s1, " ---> %ld", (long)suc);

	soritem (z, 1, 71, "*", s1);

	for (++y; y < lim1; y++) {
	    z = tat [y];
	    soritem (z, 1, 71, "*", "");
	}
    }
}



static SXVOID	set_chosen (SXINT drk, SXINT at, SXINT rn, struct priority p)
{
    chosen.desambig_rule_kind = drk;
    chosen.action_taken = at;
    chosen.reduce_no = rn;
    chosen.prio = p;
}



BOOLEAN		t_in_deriv (SXINT t, SXINT t_in_deriv_item)
{
    /* Teste si le terminal "t" appartient a S avec
       S = FIRST (beta) et t_in_deriv_item == A->alpha . beta */

    SXINT	t_in_deriv_tnt;

    while ((t_in_deriv_tnt = bnf_ag.WS_INDPRO [t_in_deriv_item++].lispro) > 0) {
	/* Non terminal */
	if (sxba_bit_is_set (bnf_ag.FIRST [t_in_deriv_tnt = XNT_TO_NT_CODE (t_in_deriv_tnt)], -t))
	    return TRUE;

	if (!sxba_bit_is_set (bnf_ag.BVIDE, t_in_deriv_tnt))
	    break;
    }

    return t_in_deriv_tnt == t;
}



static VOID	erase (SXINT StNt, SXINT StNt1)
{
    SXINT	x, y;

    for (y = 0, x = xor_StNts [StNt].X1_head; x != 0; x = includes [y = x].lnk) {
	if (includes [x].X2 == StNt1) {
	    if (y == 0)
		xor_StNts [StNt].X1_head = includes [x].lnk;
	    else
		includes [y].lnk = includes [x].lnk;

	    break;
	}
    }
}



static VOID	cutback (SXINT St1, SXINT cutback_item, SXINT StNt, SXINT q, SXINT q1, BOOLEAN to_be_cut)
{
    SXINT	x, St;
    SXINT		StNt1;

    if (bnf_ag.WS_INDPRO [cutback_item - 1].lispro != 0) {
	/* item du kernel: a -> alpha X . beta */
	for (x = S1 [St1].pprd; x != 0; x = lstprd [x].f2) {
	    /* Calcul des etats predecesseurs de St1 (sur X) */
	    St = lstprd [x].f1;
	    cutback (St, cutback_item - 1, StNt, q, q1, to_be_cut || (q == St && q1 == St1));
	}
    }
    else /* item de la fermeture: A -> . beta */
	 if (to_be_cut) {
	is_old_StNt (St1, bnf_ag.WS_NBPRO [bnf_ag.WS_INDPRO [cutback_item].prolis].reduc /* A */ , &StNt1) /* TRUE */ ;
	erase (StNt, StNt1);
    }
}



VOID	cut (SXINT St, SXINT q, SXINT q1)
{
    /* Supprime tous les elements de la relation includes "passant" par q1 et q tels que
           (St, Nt) includes (St1, Nt1)
       ou Nt, St1 et NT1 sont tels que:
           Nt -> . alpha  est dans ST
	   Nt1 -> delta . Nt gamma  est dans St
	   gamma =>* epsilon
	   St = goto* (St1, delta)
       On a
           goto*(St1, delta1) = q
           goto (q, X) = q1
           goto*(q1, delta2) = St
       avec delta1 X delta2 = delta */

    SXINT	StNt, cut_xptant, xtant, lim, cut_item;

    for (StNt = S1 [St].StNt; StNt != 0; StNt = xor_StNts [StNt].Stlnk) {
	if (xor_StNts [StNt].X1_head != 0) {
	    if ((cut_xptant = xor_StNts [StNt].xptant) > 0) {
		xtant = S1 [cut_xptant].ptant;
		/* tant[xtant] est un item de la forme
		   A -> alpha . nt Y1 Y2 ... Yn */
		lim = S1 [cut_xptant + 1].ptant;

		while (xtant < lim) {
		    cut_item = tant [xtant++];

		    if (sxba_bit_is_set (bnf_ag.NULLABLE, cut_item + 1))
			cutback (St, cut_item, StNt, q, q1, FALSE);
		}
	    }
	    else {
		cutback (St, -cut_xptant, StNt, q, q1, FALSE);
	    }
	}
    }
}



static SXINT	state_t_to_items (SXINT state, SXINT xt)
{
    /* Recherche dans l'etat "state" la liste des items dont le marqueur LR repere
       le terminal "xt" */
    /* Recherche par dichotomie */
    /* Suppose que "xt" existe */

    SXINT	bottom, top, xtt, mid_xt;

    bottom = S1 [xtt = S1 [state].shftt].ptt;
    top = S1 [xtt + 1].ptt - 1;

    while (bottom < top) {
	xtt = (bottom + top) >> 1;
	mid_xt = bnf_ag.WS_INDPRO [tat [S1 [tt [xtt]].ptat]].lispro;

	if (xt >= mid_xt)
	    top = xtt;
	else
	    bottom = xtt + 1;
    }

    return tt [top];
}



#if 0
SXINT	gotostar (SXINT state, SXINT start, SXINT goal)
{
    /* calcule goto*(state,X1 X2 ... Xn) avec
       start : A -> alpha .X1 X2 ... Xn beta
       goal :  A -> alpha X1 X2 ... Xn .beta */

    SXINT	tnt;

    while (start < goal) {
	if ((tnt = bnf_ag.WS_INDPRO [start++].lispro) > 0)
	    state = S1 [state_nt_to_items (state, XNT_TO_NT_CODE (tnt))].tsucnt;
	else
	    state = S1 [state_t_to_items (state, tnt)].tsuct;
    }

    return state;
}
#endif
/* Mofifie' le Ven 26 Nov 1999 11:56:08 */
SXINT	gotostar (SXINT state, SXINT start, SXINT goal)
{
    /* calcule goto*(state,X1 X2 ... Xn) avec
       start : A -> alpha .X1 X2 ... Xn beta
       goal :  A -> alpha X1 X2 ... Xn .beta */

    SXINT		xtnt, nt, couple, x;

    while (start < goal) {
	if ((xtnt = bnf_ag.WS_INDPRO [start++].lispro) > 0) {
	    nt = XNT_TO_NT_CODE (xtnt);
	    couple = XxY_is_set (&STATExNT_hd, state, nt); /* Should be > 0 */

#if EBUG
	    if (couple == 0)
		sxtrap (ME, "gotostar");
#endif

	    for (x = STATExNT2tnt [couple]; x != 0; x = tnt_next [x]) {
		xtnt = tnt [x];

		if (bnf_ag.WS_INDPRO [tant [S1 [xtnt].ptant]].lispro == xtnt) {
		    state = S1 [xtnt].tsucnt;
		    break;
		}
	    }
	}
	else
	    state = S1 [state_t_to_items (state, xtnt)].tsuct;
    }

    return state;
}



VOID	fill_items_set (SXINT *fill_items_set_nt_trans_set, SXINT *fill_items_set_nt_trans_stack, SXINT nt_trans)
{
    /* Rappelons qu'un noeud est une transition non terminale cad un couple (etat, liste d'items)
       ou chaque item de la liste est un element de "etat" et repere le meme non-terminal */
    /* Cette procedure non recursive traverse une fois et une seule chaque noeud du graphe
       induit par la relation includes.
       Sur chaque noeud range chaque item de la liste dans items_set s'il ne s'y trouve pas
       deja */

    SXINT	xpnt1, xpnt2, x, xtant;
    SXINT		X2, fill_items_set_xptant, lim, fill_items_set_item;

    CLEAR (fill_items_set_nt_trans_set);
    PUSH (fill_items_set_nt_trans_set, nt_trans);
    fill_items_set_nt_trans_stack [xpnt2 = 1] = nt_trans, xpnt1 = 0;

    while (++xpnt1 <= xpnt2) {
	for (x = xor_StNts [fill_items_set_nt_trans_stack [xpnt1]].X1_head; x != 0; x = includes [x].lnk) {
	    if (fill_items_set_nt_trans_set [X2 = includes [x].X2] == 0 /* nouvelle transition */ ) {
		PUSH (fill_items_set_nt_trans_set, X2);
		fill_items_set_nt_trans_stack [++xpnt2] = X2;
		fill_items_set_xptant = xor_StNts [X2].xptant;
		xtant = S1 [fill_items_set_xptant].ptant;
		/* tant [xtant] est un item de la forme A -> alpha . nt Y1 Y2 ... Yn
		   Les terminaux visibles sont ceux de FIRST (Y1 Y2 ... Yn) */
		lim = S1 [fill_items_set_xptant + 1].ptant;

		while (xtant < lim) {
		    /* On remplit l'ensemble items_set */
		    if (items_set [fill_items_set_item = tant [xtant++] + 1] == 0)
			PUSH (items_set, fill_items_set_item);
		}
	    }
	}
    }
}

static SXINT
equiv_rule_processing (SXINT StNt, SXINT lim, SXINT equiv_rule_processing_xlas, SXINT equiv_rule_processing_cur_red_no)
{
    /* On est dans le cas has_xprod */
    /* Recherche des productions ayant meme representant que StNt */
    /* On cumule les look-ahead et les item_set (si equiv_rule_processing_cur_red_no >= 0). */
    /* Retourne le nombre de productions equivalentes trouvees. */
    SXINT	x, nb, red_no, xred_no;

    nb = 0;

    if (StNt + 1 < lim) {
	xred_no = bnf_ag.WS_INDPRO [tr [StNt]].prolis;
	red_no = PROD_TO_RED (xred_no);

	if (equiv_rule_processing_cur_red_no >= 0)
	    items_set = Titems_set [equiv_rule_processing_cur_red_no];

	x = 0;

	while (++StNt < lim) {
	    xred_no = bnf_ag.WS_INDPRO [tr [StNt]].prolis;

	    if (PROD_TO_RED (xred_no) != red_no)
		/* Elles sont consecutives. */
		break;

	    nb++;
	    sxba_or (look_ahead_sets [equiv_rule_processing_xlas], look_ahead_sets [equiv_rule_processing_xlas + ++x]);

	    if (equiv_rule_processing_cur_red_no >= 0)
		fill_items_set (nt_trans_set, nt_trans_stack, plxi [equiv_rule_processing_xlas + x]);
	}
    }

    return nb;
}


static SXINT
equiv_rule_nb (SXINT StNt, SXINT lim)
{
    /* On est dans le cas has_xprod */
    /* Recherche des productions ayant meme representant que StNt. */
    /* Retourne le nombre de productions equivalentes trouvees. */
    SXINT	nb, red_no;

    nb = 0;

    if (StNt + 1 < lim) {
	red_no = PROD_TO_RED (bnf_ag.WS_INDPRO [tr [StNt]].prolis);

	 while (++StNt < lim) {
	     if (PROD_TO_RED (bnf_ag.WS_INDPRO [tr [StNt]].prolis) != red_no)
		 /* Elles sont consecutives. */
		 break;

	     nb++;
	 }
    }

    return nb;
}





static SXVOID	fill_look_ahead_set (SXINT *fill_lookahead_set_items_set, SXBA la_set)
{
    /* Cette procedure remplit l'ensemble la_set avec les terminaux visibles depuis les items
       de fill_lookahead_set_items_set. Si item est un element de fill_lookahead_set_items_set: A -> alpha . Y1 Y2 ... Yn 
       Les terminaux visibles sont ceux de FIRST (Y1 Y2 ... Yn) */

    SXINT	x, fill_lookahead_set_tnt, fill_lookahead_set_item;

    for (x = fill_lookahead_set_items_set [0]; x > 0; x = fill_lookahead_set_items_set [x]) {
	fill_lookahead_set_item = x;

	while ((fill_lookahead_set_tnt = bnf_ag.WS_INDPRO [fill_lookahead_set_item++].lispro) != 0) {
	    if (fill_lookahead_set_tnt > 0) {
		sxba_or (la_set, bnf_ag.FIRST [fill_lookahead_set_tnt = XNT_TO_NT_CODE (fill_lookahead_set_tnt)]);

		if (!sxba_bit_is_set (bnf_ag.BVIDE, fill_lookahead_set_tnt))
		    break;
	    }
	    else {
		sxba_1_bit (la_set, -fill_lookahead_set_tnt);
		break;
	    }
	}
    }
}




SXINT	LALR1 (char *language_name)
{
    if (sxverbosep) {
	fprintf (sxtty, "%s\n", ME);
	sxttycol1p = TRUE;
    }

    {
	SXINT	NBPRO, INDPRO, INDPROt3, INDPROt2;
	SXINT	INDPROd2, INDPROt6, NBPROd2;

	NBPRO = bnf_ag.WS_TBL_SIZE.nbpro;
	NBPRO = max (10, NBPRO);
	INDPRO = bnf_ag.WS_TBL_SIZE.indpro;
	INDPRO = max (10, INDPRO);
	NBPROd2 = NBPRO / 2;
	INDPROd2 = INDPRO / 2;
	INDPROt2 = INDPRO * 2;
	INDPROt3 = INDPRO * 3;
	INDPROt6 = INDPRO * 6;
	lgt [1] = min (1000, INDPROd2);
	lgt [2] = min (2500, INDPROt2);
	lgt [3] = 100;
	lgt [4] = min (300, NBPROd2);
	lgt [5] = min (6000, INDPROt6);
	lgt [6] = min (3500, INDPROt3);
	lgt [7] = 100;
	lgt [8] = min (2000, INDPROt2);
	lgt [9] = min (800, INDPRO);
	lgt [10] = min (2000, INDPROt2);
	lgt [11] = min (4000, INDPROt3);
	lgt [12] = min (2000, INDPROt2);
    }

    usr = urr = srr = ssr = 0;
    is_lalr1 = TRUE;
    is_lr1 = TRUE;
    is_ambiguous = FALSE;
    is_listing_opened = FALSE;
    sature ((SXINT)0);
    XxY_alloc (&STATExNT_hd, "STATExNT", lgt [9], 2, 0, 0, oflw_STATExNT2tnt, NULL);
    STATExNT2tnt = (SXINT*) sxalloc (XxY_size (STATExNT_hd) + 1, sizeof (SXINT));
    limitem = bnf_ag.WS_TBL_SIZE.indpro + 1;
    limt = -bnf_ag.WS_TBL_SIZE.xtmax - 1;
    limnt = bnf_ag.WS_TBL_SIZE.xntmax + 1;
    limpro = bnf_ag.WS_TBL_SIZE.xnbpro + 1;
    has_xprod = limpro > bnf_ag.WS_TBL_SIZE.actpro + 1;
    not_used_red_set = sxba_calloc (bnf_ag.WS_TBL_SIZE.actpro + 1);
    sxba_fill (not_used_red_set), sxba_0_bit (not_used_red_set, 0);

    if (is_tspe) {

/* Construction de s_red_set ensemble des reductions qui ont une lhs qui est la rhs d'une
   production simple */

	SXBA	nt_set;
	SXINT	i, j, k;

	s_red_set = sxba_calloc (bnf_ag.WS_TBL_SIZE.actpro + 1);
	nt_set = sxba_calloc (bnf_ag.WS_TBL_SIZE.ntmax + 1);

	for (i = 1; i <= bnf_ag.WS_TBL_SIZE.actpro; i++) {
	    if (bnf_ag.WS_NBPRO [i].bprosimpl) {
		k = bnf_ag.WS_INDPRO [bnf_ag.WS_NBPRO [i].prolon].lispro;
		sxba_1_bit (nt_set, XNT_TO_NT_CODE (k));
	    }
	}

	for (i = sxba_scan (nt_set, 0); i != -1; i = sxba_scan (nt_set, i)) {
	    k = bnf_ag.WS_NTMAX [i + 1].npg;

	    for (j = bnf_ag.WS_NTMAX [i].npg; j < k; j++) {
		sxba_1_bit (s_red_set, bnf_ag.WS_NBPRO [j].numpg);
	    }
	}

	sxfree (nt_set);
    }
    else
	s_red_set = NULL;


/* Construction de l'automate LR (0) */

    {
	if (sxverbosep) {
	    fputs ("\tLR (0) Automaton\n", sxtty);
	    sxttycol1p = TRUE;
	}

	item = (SXINT*) sxalloc (limitem, sizeof (SXINT));
	tsymb1 = (SXINT*) sxalloc (bnf_ag.WS_TBL_SIZE.xntmax - limt + 1, sizeof (SXINT)) - limt;
	/* limt est negatif (-xtmax-1) */
	tsymb2 = (SXINT*) sxalloc (bnf_ag.WS_TBL_SIZE.xntmax - limt + 1, sizeof (SXINT)) - limt;


/* limt est negatif (-xtmax-1) */

	{
	    SXINT	i;

	    for (i = 0; i <= bnf_ag.WS_TBL_SIZE.indpro; i++) {
		item [i] = limitem;
	    }

	    for (i = -bnf_ag.WS_TBL_SIZE.xtmax; i <= bnf_ag.WS_TBL_SIZE.xntmax; i++) {
		tsymb1 [i] = limitem;
	    }

	    for (i = limt; i <= -1; i++) {
		tsymb2 [i] = limt;
	    }

	    for (i = 0; i <= bnf_ag.WS_TBL_SIZE.xntmax; i++) {
		tsymb2 [i] = limnt;
	    }
	}

	xnucl2 = S1 [2].etat = xac2 = 2;
	S1 [0].ptr = S1 [1].ptr = xptr = S1 [0].ptt = S1 [1].ptt = xptt = S1 [0].ptat = S1 [1].ptat = xptat = S1 [0].ptnt
	     = S1 [1].ptnt = xptnt = S1 [0].ptant = S1 [1].ptant = xptant = xtnt2 = xtant2 = xtt2 = xtat2 = xtr2 = nucl [
	     1] = S1 [1].etat = 1;
	xlstprd = reduce_item_no = 0;
	max_red_per_state = 0;

	{
	    SXINT	xac1, lim;

	    for (xac1 = 1; xac1 < xac2; xac1++) {
		struct S1	*aS1xac1;

		aS1xac1 = S1 + xac1;
		aS1xac1->StNt = 0;
		xptr2 = 1;

		{
		    SXINT	x = aS1xac1->etat;

		    /* *NV* */
		    if (nucl [x] == 2) /* [S' -> $ S . $] */
			final_state = xac1;

		    lim = S1 [xac1 + 1].etat;

		    while (x < lim) {
			mettre (nucl [x]);
			x++;
		    }
		}

		{
		    SXINT	xptr1, x, i;
		    SXINT		j;

		    for (xptr1 = 1; xptr1 < xptr2; xptr1++) {
			x = ptrans [xptr1];
			x = XNT_TO_NT_CODE (x);
			j = bnf_ag.WS_NTMAX [x + 1].npg;

			for (i = bnf_ag.WS_NTMAX [x].npg; i < j; i++) {
			    mettre (bnf_ag.WS_NBPRO [bnf_ag.WS_NBPRO [i].numpg].prolon);
			}
		    }
		}

		item [0] = tsymb1 [0];
		tsymb1 [0] = limitem;

		{
		    SXINT	l1, l2, hsh;
		    SXINT		xtr1, k;

		    hsh = 0;
		    xtr1 = xtr2;

		    for (l2 = item [l1 = 0]; l2 < limitem; l2 = item [l1 = l2]) {
			item [l1] = limitem;
			tr [xtr2] = l2;
			maj (&xtr2, (SXINT)4);
			reduce_item_no++;
			hsh += l2;
		    }

		    k = xptr;
		    majc1 (&xptr);
		    S1 [xptr].ptr = xtr2;

		    if ((aS1xac1->pred = isint (tr, (SXINT)1, k, hsh)) != k) {
			xtr2 = xtr1;
			xptr = k;
		    }
		    else {
			k = xtr2 - xtr1;
			max_red_per_state = max (max_red_per_state, k);
		    }
		}

		{
		    SXINT	m1, m2;
		    SXINT		hsh, hsh1, xtt1, xtat1, k, xtant1, xtnt1, x;

		    xtt1 = xtt2;
		    hsh1 = 0;

		    for (m2 = tsymb2 [m1 = limt]; m2 != limt; m2 = tsymb2 [m1 = m2]) {
			tsymb2 [m1] = limt;
			item [0] = tsymb1 [m2];
			tsymb1 [m2] = limitem;
			hsh = 0;
			xtat1 = xtat2;

			{
			    SXINT	l1, l2;

			    for (l2 = item [l1 = 0]; l2 < limitem; l2 = item [l1 = l2]) {
				item [l1] = limitem;
				tat [xtat2] = l2;
				maj (&xtat2, (SXINT)9);
				hsh += l2;
			    }
			}

			k = xptat;
			majc1 (&xptat);
			S1 [xptat].ptat = xtat2;

			{
			    SXINT	y;
			    SXINT		suc, reduce;

			    if ((x = isint (tat, (SXINT)2, k, hsh)) == k) {
				if ((xtat2 - xtat1) == 1 && bnf_ag.WS_INDPRO [(y = tat [xtat1]) + 1].lispro == 0 && (!
				     is_tspe || (reduce = bnf_ag.WS_INDPRO [y].prolis) == 0 || !sxba_bit_is_set (
				     s_red_set, reduce)))
				    suc = 0;
				else {
				    suc = xac2;
				    S1 [suc].pprd = 0;

				    for (y = xtat1; y < xtat2; y++) {
					nucl [xnucl2] = tat [y] + 1;
					maj (&xnucl2, (SXINT)12);
				    }

				    majc1 (&xac2);
				    S1 [xac2].etat = xnucl2;
				}

				S1 [k].tsuct = suc;
			    }
			    else {
				xptat = k;
				xtat2 = xtat1;
				suc = S1 [x].tsuct;
			    }

			    if (suc != 0)
				filprd (xac1, suc);

			    tt [xtt2] = x;
			    maj (&xtt2, (SXINT)8);
			    hsh1 += x;
			}
		    }

		    k = xptt;
		    majc1 (&xptt);
		    S1 [xptt].ptt = xtt2;

		    if ((aS1xac1->shftt = isint (tt, (SXINT)3, k, hsh1)) != k) {
			xtt2 = xtt1;
			xptt = k;
		    }

		    xtnt1 = xtnt2;
		    hsh1 = 0;

		    for (m2 = tsymb2 [m1 = 0]; m2 != limnt; m2 = tsymb2 [m1 = m2]) {
			tsymb2 [m1] = limnt;
			item [0] = tsymb1 [m2];
			tsymb1 [m2] = limitem;
			hsh = 0;
			xtant1 = xtant2;

			{
			    SXINT	l1, l2;

			    for (l2 = item [l1 = 0]; l2 < limitem; l2 = item [l1]) {
				item [l1] = limitem;
				tant [xtant2] = l2;
				maj (&xtant2, (SXINT)10);
				hsh += l2;
				l1 = l2;
			    }
			}

			k = xptant;
			majc1 (&xptant);
			S1 [xptant].ptant = xtant2;

			{
			    SXINT	y;
			    SXINT		i, l, suc;

			    i = tant [xtant1];

			    if ((x = isint (tant, (SXINT)4, k, hsh)) == k) {
				/* L'instruction Halt (reduce 0) sera generee dans les T_tables */

				if (xtant2 - xtant1 == 1) {
				    l = bnf_ag.WS_INDPRO [i + 1].lispro;

				    if (l == 0 && (!is_tspe || (!sxba_bit_is_set (s_red_set, (y = bnf_ag.WS_INDPRO [i].
					 prolis)) && !bnf_ag.WS_NBPRO [y].bprosimpl))) {
					suc = 0;
					goto filsuc;
				    }
				    /* On ne genere plus de "test scan reduce" dans les NT_tables */
				}

				suc = xac2;
				S1 [suc].pprd = 0;

				for (y = xtant1; y < xtant2; y++) {
				    nucl [xnucl2] = tant [y] + 1;
				    maj (&xnucl2, (SXINT)12);
				}

				majc1 (&xac2);
				S1 [xac2].etat = xnucl2;
filsuc:				S1 [k].tsucnt = suc;
			    }
			    else {
				xtant2 = xtant1;
				xptant = k;
				suc = S1 [x].tsucnt;
			    }

			    if (suc != 0)
				filprd (xac1, suc);

			    tnt [xtnt2] = x;
			    maj (&xtnt2, (SXINT)11);
			    hsh1 += x;
			}
		    }

		    k = xptnt;
		    majc1 (&xptnt);
		    S1 [xptnt].ptnt = xtnt2;

		    if ((x = aS1xac1->shftnt = isint (tnt, (SXINT)5, k, hsh1)) != k) {
			xtnt2 = xtnt1;
			xptnt = k;
		    }

		    /* On remplit STATExNT_hd */
		    for (lim = S1 [x+1].ptnt, x = S1 [x].ptnt; x < lim; x++) {
			store_nt_items (xac1, x);
		    }
		}
	    }
	    /* while (xac1 < xac2) */
	}

	sxfree (tsymb2 + limt), tsymb2 = NULL;
	sxfree (tsymb1 + limt), tsymb1 = NULL;
	sxfree (item), item = NULL;
    }


    t_set = sxba_calloc (bnf_ag.WS_TBL_SIZE.xtmax + 1);
    plxi = (SXINT*) sxalloc (reduce_item_no + 1, sizeof (SXINT));
    look_ahead_sets = sxbm_calloc (reduce_item_no + 1, bnf_ag.WS_TBL_SIZE.xtmax + 1);

    {
	if (sxverbosep) {
	    fputs ("\tLALR (1) Look Ahead Sets\n", sxtty);
	    sxttycol1p = TRUE;
	}

#if 0
	/* *NV* */
	{
	    /* Calcul de FIRST  et de nullable_set*/

	    SXBA	*DEB_s, *BA_s, bidon, line;
	    SXINT	i, j, item;
	    SXINT	head, tail;
	    SXINT		limnt, limtnt;

	    limnt = bnf_ag.WS_TBL_SIZE.ntmax + 1;
	    limtnt = limnt + bnf_ag.WS_TBL_SIZE.xtmax;
	    nullable_set = sxba_calloc (bnf_ag.WS_TBL_SIZE.indpro + 1);
	    DEB_s = sxbm_calloc (limnt, limtnt);
	    bidon = sxba_calloc (limtnt);
	    BA_s = (SXBA_ELT**) sxalloc (limtnt, sizeof (SXBA_ELT*));

	    for (i = limtnt - 1, j = limnt - 1; j > 0; i--, j--)
		BA_s [i] = DEB_s [j];

	    for (; i >= 0; i--)
		BA_s [i] = bidon;

	    for (i = 1; i <= bnf_ag.WS_TBL_SIZE.nbpro; i++) {
		line = DEB_s [bnf_ag.WS_NBPRO [i].reduc];
		head = bnf_ag.WS_NBPRO [i].prolon;
		tail = bnf_ag.WS_NBPRO [i + 1].prolon - 1;

		for (j = head; j < tail; j++) {
		    if ((item = bnf_ag.WS_INDPRO [j].lispro) < 0) {
			sxba_1_bit (line, -item);
			break;
		    }

		    sxba_1_bit (line, (item = XNT_TO_NT_CODE (item)) + bnf_ag.WS_TBL_SIZE.xtmax);

		    if (!sxba_bit_is_set (bnf_ag.BVIDE, item))
			break;
		}

		sxba_1_bit (nullable_set, tail);

		for (j = tail - 1; j >= head; j--) {
		    if ((item = bnf_ag.WS_INDPRO [j].lispro) < 0 || !sxba_bit_is_set (bnf_ag.BVIDE, item))
			break;

		    sxba_1_bit (nullable_set, j);
		}
	    }

	    fermer (BA_s, bnf_ag.WS_TBL_SIZE.ntmax - limt);
	    sxfree (BA_s);
	    sxfree (bidon);
	    FIRST = sxbm_resize (DEB_s, limnt, limnt, -limt);
	}
#endif

	xor_hash_size = xac2 + bnf_ag.WS_TBL_SIZE.ntmax + max_red_per_state + 1;
	xor_hash = (SXINT*) sxcalloc (xor_hash_size, sizeof (SXINT));
	xxor_StNt = 0;
	xincludes = 0;
	xplxi = 1;


/* Calcul de la relation includes */

	{
	    SXINT	xac1;

	    for (xac1 = 1; xac1 < xac2; xac1++) {
		compute_includes (xac1);
	    }
	}

	nt_trans_set = (SXINT*) sxcalloc (xxor_StNt + 1, sizeof (SXINT));
	nt_trans_set [0] = -1;
	nt_trans_stack = (SXINT*) sxalloc (xxor_StNt + 1, sizeof (SXINT));
	Titems_set = (SXINT**) sxalloc (max_red_per_state, sizeof (SXINT*));

	{
	    SXINT	i;

	    for (i = 0; i < max_red_per_state; i++) {
		Titems_set [i] = (SXINT*) sxcalloc (bnf_ag.WS_TBL_SIZE.indpro + 1, sizeof (SXINT));
		Titems_set [i] [0] = -1;
	    }
	}


	if (is_lr1_constructor) {
	    /* On regarde si la grammaire n'est pas LALR(1) mais LR(1) et, si oui, on duplique les
	       etats responsables pour obtenir un automate LR(1) (non canonique) */

	    SXINT	        xac1;
	    SXINT		old_xac2, old_reduce_item_no;
	    SXINT	        x, xt1, xtr1, base_xtr1, lim, lim_xtr1, t, l, l2, LALR1_xtr2;
	    SXBA	la_set_r1;
	    SXINT		r1;
	    BOOLEAN	LALR1_is_lalr1, LALR1_is_lr1;


	    if (sxverbosep) {
		fputs ("\tLR (1) state splitting\n", sxtty);
		sxttycol1p = TRUE;
	    }

	    xac1 = 1;
	    xlas = 1;
	    path_reallocation (bnf_ag.FIRST);

	    while (xac1 < xac2) {
		old_xac2 = xac2;
		old_reduce_item_no = reduce_item_no;


/* Ca permet de ne faire les reallocations que par "tranche" d'automate */

		do {
		    /* Recherche des conflits Reduce/Reduce (eventuels) en LALR(1) */
		    t = S1 [xac1].pred;

		    if ((lim_xtr1 = S1 [t + 1].ptr) - (base_xtr1 = S1 [t].ptr) > 0) {
			/* Il y a au moins un reduce */
			cur_red_no = 0;
			base_xlas = xlas;

			for (xtr1 = base_xtr1; xtr1 < lim_xtr1; xtr1++) {
			    items_set = Titems_set [cur_red_no];
			    CLEAR (items_set);
			    fill_items_set (nt_trans_set, nt_trans_stack, plxi [xlas]);
			    fill_look_ahead_set (items_set, look_ahead_sets [xlas]);
			    xlas++;
			    cur_red_no++;
			}


/* On regarde s'il y a un conflit Shift/Reduce, si oui => non LR(1) */

			t = S1 [xac1].shftt;
			lim = S1 [t + 1].ptt;
			LALR1_is_lr1 = LALR1_is_lalr1 = TRUE;

			for (xt1 = S1 [t].ptt; LALR1_is_lr1 && xt1 < lim; xt1++) {
			    t = bnf_ag.WS_INDPRO [tat [S1 [tt [xt1]].ptat]].lispro;

			    for (x = base_xlas; x < xlas; x++) {
				if (sxba_bit_is_set (look_ahead_sets [x], -t)) {
				    /* Conflit Shift/Reduce */
				    LALR1_is_lr1 = LALR1_is_lalr1 = FALSE;
				    break;
				}
			    }
			}

			/* BEGIN *NV* */
			if (has_xprod) {
			    /* On fusionne les XNT equivalents eventuels */
			    r1 = base_xlas;
			    cur_red_no = 0;

			    for (xtr1 = base_xtr1; xtr1 < lim_xtr1; xtr1++) {
				l = 1+equiv_rule_processing (xtr1, lim_xtr1, r1, cur_red_no);
				xtr1 += l;
				r1 += l;
				cur_red_no += l;
			    }
			}

			r1 = base_xlas;
			cur_red_no = 0;

			for (xtr1 = base_xtr1; LALR1_is_lr1 && xtr1 < (lim_xtr1-1); xtr1++) {
			    if (has_xprod)
				l = equiv_rule_nb (xtr1, lim_xtr1);
			    else
				l = 0;

			    la_set_r1 = look_ahead_sets [r1];

			    for (LALR1_xtr2 = xtr1+l+1; LALR1_xtr2 < lim_xtr1; LALR1_xtr2++) {
				x = r1+l+1;
				sxba_copy (t_set, look_ahead_sets [x]);

				if (!sxba_is_empty (sxba_and (t_set, la_set_r1))) {
				    /* Conflit Reduce/Reduce => non LALR(1) */
				    LALR1_is_lalr1 = FALSE;

				    if (!check_lr1 (xac1, t_set, plxi [r1], plxi [x], FALSE, FALSE)) {
					LALR1_is_lr1 = FALSE;
					break;
				    }
				}
			    }

			    xtr1 += l;
			    r1 += l;
			}

			if (LALR1_is_lr1) {
			    while (!LALR1_is_lalr1) {
				LALR1_is_lalr1 = TRUE;

				r1 = base_xlas;
				cur_red_no = 0;

				for (xtr1 = base_xtr1; LALR1_is_lalr1 && xtr1 < (lim_xtr1-1); xtr1++) {
				    if (has_xprod)
					l = equiv_rule_nb (xtr1, lim_xtr1);
				    else
					l = 0;

				    la_set_r1 = look_ahead_sets [r1];

				    for (LALR1_xtr2 = xtr1+l+1; LALR1_xtr2 < lim_xtr1; LALR1_xtr2++) {
					x = r1+l+1;
					sxba_copy (t_set, look_ahead_sets [x]);

					if (!sxba_is_empty (sxba_and (t_set, la_set_r1))) {
					    /* Conflit Reduce/Reduce => non LALR(1) */
					    LALR1_is_lalr1 = FALSE;
					    check_lr1 (xac1, t_set, plxi [r1], plxi [x], TRUE, FALSE);
					    /* Recalcul des look_ahead_sets */
					    xlas = base_xlas;
					    cur_red_no = 0;

					    for (xtr1 = base_xtr1; xtr1 < lim_xtr1; xtr1++) {
						items_set = Titems_set [cur_red_no];
						CLEAR (items_set);
						fill_items_set (nt_trans_set, nt_trans_stack, plxi [xlas]);
						fill_look_ahead_set (items_set, sxba_empty (look_ahead_sets [xlas]));
						xlas++;
						cur_red_no++;
					    }

					    /* *NV* */
					    if (has_xprod) {
						/* On fusionne les XNT equivalents eventuels */
						xlas = base_xlas;
						cur_red_no = 0;

						for (xtr1 = base_xtr1; xtr1 < lim_xtr1; xtr1++) {
						    l2 = 1+equiv_rule_processing (xtr1, lim_xtr1, xlas, cur_red_no);
						    xtr1 += l2;
						    xlas += l2;
						    cur_red_no += l2;
						}
					    }
					    /* *NV* */

					    break;
					}
				    }

				    if (LALR1_xtr2 < lim_xtr1)
					break;

				    xtr1 += l;
				    r1 += l;
				}
			    }
			}
			/* END *NV* */

#if 0
			for (r1 = base_xlas; LALR1_is_lr1 && r1 < xlas - 1; r1++) {
			    la_set_r1 = look_ahead_sets [r1];

			    for (x = r1 + 1; x < xlas; x++) {
				sxba_copy (t_set, look_ahead_sets [x]);

				if (!sxba_is_empty (sxba_and (t_set, la_set_r1))) {
				    /* Conflit Reduce/Reduce => non LALR(1) */
				    LALR1_is_lalr1 = FALSE;

				    if (!check_lr1 (xac1, t_set, plxi [r1], plxi [x], FALSE, FALSE)) {
					LALR1_is_lr1 = FALSE;
					break;
				    }
				}
			    }
			}

			if (LALR1_is_lr1) {
			    while (!LALR1_is_lalr1) {
				LALR1_is_lalr1 = TRUE;

				for (r1 = base_xlas; LALR1_is_lalr1 && r1 < xlas - 1; r1++) {
				    la_set_r1 = look_ahead_sets [r1];

				    for (x = r1 + 1; x < xlas; x++) {
					sxba_copy (t_set, look_ahead_sets [x]);

					if (!sxba_is_empty (sxba_and (t_set, la_set_r1))) {
					    /* Conflit Reduce/Reduce => non LALR(1) */
					    LALR1_is_lalr1 = FALSE;
					    check_lr1 (xac1, t_set, plxi [r1], plxi [x], TRUE, FALSE);
					    /* Recalcul des look_ahead_sets */
					    xlas = base_xlas;
					    cur_red_no = 0;

					    for (xtr1 = base_xtr1; xtr1 < lim_xtr1; xtr1++) {
						items_set = Titems_set [cur_red_no];
						CLEAR (items_set);
						fill_items_set (nt_trans_set, nt_trans_stack, plxi [xlas]);
						fill_look_ahead_set (items_set, sxba_empty (look_ahead_sets [xlas]));
						xlas++;
						cur_red_no++;
					    }

					    break;
					}
				    }
				}
			    }
			}
#endif
		    }
		    else
			xlas += lim_xtr1 - base_xtr1;
		} while (++xac1 < old_xac2);

		if (xac2 > old_xac2) {
		    plxi = (SXINT*) sxrealloc (plxi, reduce_item_no + 1, sizeof (SXINT));
		    look_ahead_sets = sxbm_resize (look_ahead_sets, old_reduce_item_no + 1, reduce_item_no + 1, bnf_ag.
			 WS_TBL_SIZE.xtmax + 1);

/* Mise a jour de la relation includes */

		    xor_hash_size = xac2 + bnf_ag.WS_TBL_SIZE.ntmax + max_red_per_state + 1;
		    xor_hash = (SXINT*) sxrealloc (xor_hash, xor_hash_size, sizeof (SXINT));

		    {
			SXINT	*p, *LALR1_lim;

			LALR1_lim = xor_hash + xor_hash_size;

			for (p = LALR1_lim + old_xac2 - xac2; p < LALR1_lim; p++)
			    *p = 0;
		    }

		    {
			SXINT	state;

			for (state = old_xac2; state < xac2; state++) {
			    compute_includes (state);
			}
		    }

		    nt_trans_set = (SXINT*) sxrealloc (nt_trans_set, xxor_StNt + 1, sizeof (SXINT));

		    {
			SXINT	*p, *LALR1_lim_2;

			LALR1_lim_2 = nt_trans_set + xxor_StNt;

			for (p = nt_trans_set + 1; p <= LALR1_lim_2; p++)
			    *p = 0;
		    }

		    nt_trans_set [0] = -1;
		    nt_trans_stack = (SXINT*) sxrealloc (nt_trans_stack, xxor_StNt + 1, sizeof (SXINT));
		    /* reallocations */
		    path_reallocation (bnf_ag.FIRST);
		}
	    }

	    if (!is_path)
		path_free ();
	    /* sinon les variables sont reutilisees */
	}
    }


    {
	/* On calcule une borne max de la taille du genere pour

	   les transitions terminales+reduces (Mt_items).
	   si xac1 n'a pas de reduce     => +tt + 1 (error)
	   si xac1 a 1 reduce	     => +tmax (prudence)
	   si xac1 a >1 reduce	     => +tmax (prudence)

	   les transitions non-terminales (Mnt_items) */

	SXINT	xac1, x;

	Mt_items = Mnt_items = 0;

	for (xac1 = 1; xac1 < xac2; xac1++) {
	    struct S1	*aS1xac1 = S1 + xac1;

	    x = aS1xac1->pred;

	    if (S1 [x + 1].ptr - S1 [x].ptr > 0 /* Il y a au moins 1 reduce */ ) {
	      Mt_items += -limt; /* Le 20/12/2004 on tient compte de xt !! */
		/* Mt_items += -bnf_ag.WS_TBL_SIZE.tmax; */
	    }
	    else {
		x = aS1xac1->shftt;
		Mt_items += S1 [x + 1].ptt - S1 [x].ptt + 1;
	    }

	    x = aS1xac1->shftnt;
	    Mnt_items += S1 [x + 1].ptnt - S1 [x].ptnt;
	}
    }


/* Floyd_Evans des transitions non-terminales */

    {
	SXINT	xac1, x, lim, xpt;
	SXINT	itm, next, xnt;
	struct P_item	*aitem;
	struct state	*astate;
	SXINT	*tant_fe;
	BOOLEAN		is_xnt;
	SXBA	nt_set;

	/* Le 8/12/04 t_items a deborde' de 1 !! 
	 => utilisation de garde_fou */
#if 0
#define garde_fou (Mt_items+2*xac2)
#endif /* 0 */
	/* Le 20/12/04 Mt_items tient compte des xt !! */

	if (sxverbosep) {
	    fputs ("\tNT_Tables\n", sxtty);
	    sxttycol1p = TRUE;
	}

	xnt_state_set = NULL;
	nt_set = sxba_calloc (bnf_ag.WS_TBL_SIZE.xntmax + 1);
	xpredicates = xprdct_items = 0;
	any = bnf_ag.WS_TBL_SIZE.xtmax + 1;
	xe1 = xac2;
	t_states = (struct state*) sxcalloc (xe1+
					     xac2 /* tous les nt en ScanReduce */+
					     2 /* etats initial et final */+
					     1
					     /* +garde_fou */,
					     sizeof (struct state));
	t_items = (struct P_item*) sxalloc (Mt_items+
					    2*xac2/* tous les nt en ScanReduce */+
					    4 /* etats initial et final */+
					    1
					    /* +garde_fou */,
					    sizeof (struct
	     P_item));
	t_fe = (struct floyd_evans*) sxalloc (Mt_items+
					      xac2 /* tous les nt en ScanReduce */+
					      2 /* etats initial et final */+
					      1 /* error */+
					      1
					      /* +garde_fou */,
					      sizeof (struct floyd_evans));
	t_lnks = (SXINT*) sxalloc (Mt_items+
				 xac2 /* tous les nt en ScanReduce */+
				 2 /* etats initial et final */+
				 1 /* error */+
				 1
				 /* +garde_fou */,
				 sizeof (SXINT));
	nt_states = (struct state*) sxcalloc (xac2, sizeof (struct state));
	nt_items = (struct P_item*) sxalloc (Mnt_items + 1, sizeof (struct P_item));
	nt_fe = (struct floyd_evans*) sxalloc (xptant + xe1 /* pour all_single */  + 1 /* error */ + 1,
					       sizeof (struct floyd_evans));
	nt_lnks = (SXINT*) sxalloc (xptant + xe1 + 1 + 1, sizeof (SXINT));
	tant_fe = (SXINT*) sxcalloc (xptant + 1, sizeof (SXINT));

	for (x = 0; x < HASH_SIZE; x++)
	    t_hash [x] = nt_hash [x] = 0;

	nt_lnks [0] = t_lnks [0] = 0;
	xnt_items = xt_items = 1;
	t_fe->codop = Error;
	t_fe->reduce = 0;
	t_fe->pspl = 0;
	t_fe->next = 0;
	t_error = put_in_fe (t_fe, t_hash, t_lnks, t_fe);
	/* Correction du Mer 24 Nov 1999 11:23:24 : t_error est utilise ds OPTIM (sequence de predicats)
	   ds le fe des transitions non-terminales */
	put_in_fe (nt_fe, nt_hash, nt_lnks, t_fe); /* Retourne t_error */
	force_error = 0;

/* On genere l'etat initial */

	astate = t_states + (init_state = xe1++);
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

	for (xac1 = 1; xac1 < xac2; xac1++) {
	    if ((x = S1 [xac1].shftnt) != 0) {
		astate = nt_states + xac1;
		aitem = nt_items + (astate->start = xnt_items);
		lim = S1 [x + 1].ptnt;
		is_xnt = FALSE;

		for (x = S1 [x].ptnt; x < lim; x++) {
		    aitem->check = bnf_ag.WS_INDPRO [itm = tant [S1 [xpt = tnt [x]].ptant]].lispro;

		    if (aitem->check > bnf_ag.WS_TBL_SIZE.ntmax) {
			is_xnt = TRUE;
			xnt = aitem->check;
			sxba_1_bit (nt_set, XNT_TO_NT_CODE (xnt));

			if (xnt_state_set == NULL)
			    xnt_state_set = sxba_calloc (xac2);

			sxba_1_bit (xnt_state_set, xac1);
			xprdct_items++;
		    }

		    if ((aitem->action = tant_fe [xpt]) == 0) {
			if ((next = S1 [xpt].tsucnt) == 0
			    /* pas de successeur => un seul item en fin de partie droite */
							 ) {
			    /* *NV* */
			    nt_fe->codop = Reduce;
			    nt_fe->reduce = bnf_ag.WS_INDPRO [itm].prolis;
			    nt_fe->next = bnf_ag.WS_NBPRO [nt_fe->reduce].reduc;
			    nt_fe->next = XNT_TO_NT_CODE (nt_fe->next);
			    nt_fe->pspl = LGPROD (nt_fe->reduce) - 1;
			    nt_fe->reduce = PROD_TO_RED (nt_fe->reduce);
			    sxba_0_bit (not_used_red_set, nt_fe->reduce);
			}
			else {
			    /* cas general */
			    nt_fe->codop = Shift;
			    nt_fe->reduce = 0;
			    nt_fe->next = next;
			    nt_fe->pspl = S1 [next].shftnt == 0 ? 0 : next;
			}

			tant_fe [xpt] = aitem->action = put_in_fe (nt_fe, nt_hash, nt_lnks, nt_fe);
		    }

		    aitem++;
		    xnt_items++;
		    astate->lgth++;
		}

		if (is_xnt) {
		    xpredicates += sxba_cardinal (nt_set) /* nb de sequences */;
		    sxba_empty (nt_set);
		}
	    }
	}

	sxfree (tant_fe);
	sxfree (nt_set);
    }


/* Floyd_Evans des transitions terminales */

    {
	BOOLEAN		is_fsa_built;

	if (sxverbosep) {
	    fputs ("\tT_Tables\n", sxtty);
	    sxttycol1p = TRUE;
	}

	pter = (SXINT*) sxalloc (-limt, sizeof (SXINT));
	pconf = (SXINT*) sxalloc (-limt, sizeof (SXINT));
	ter = (SXINT*) sxcalloc (-limt, sizeof (SXINT)) - limt;
	conf = (SXINT*) sxcalloc (-limt, sizeof (SXINT)) - limt;
	nbt = (SXINT*) sxalloc (max_red_per_state + 1, sizeof (SXINT));
	xt_state_set = NULL;
	/* Allocation et initialisation de t_priorities et r_priorities par lecture de
	       L.dt (desambig_tables). */
	prio_read (-limt, limpro, &t_priorities, &r_priorities, language_name);
	is_fsa_built = FALSE;

	xlas = 1;

	{
	    /* Au boulot */
	    SXINT	xac1;
	    SXINT		pop_fe;
	    BOOLEAN	is_xt, is_message;

	    for (xac1 = 1; xac1 < xac2; xac1++) {
		is_message = FALSE;
		xkod = 0, xpconf = 0, xpter = 0;

		{
		    SXINT	x, xtt1, t, lim;

		    x = S1 [xac1].shftt;
		    lim = S1 [x + 1].ptt;

		    for (xtt1 = S1 [x].ptt; xtt1 < lim; xtt1++) {
			t = bnf_ag.WS_INDPRO [tat [S1 [tt [xtt1]].ptat]].lispro;
			pter [++xpter] = t;
			ter [t] = -xtt1;
		    }
		}

#if 0
		/* Version 3.9 */
		{
		    SXINT	t, xtr1, lim;

		    t = S1 [xac1].pred;
		    lim = S1 [t + 1].ptr;
		    xtr1 = S1 [t].ptr;

		    if (xtr1 < lim) {
			cur_red_no = 0;
			base_xlas = xlas;

			for (; xtr1 < lim; xtr1++) {
			    SXINT	*cur_nbt;
			    SXBA	la_set = look_ahead_sets [xlas];

			    items_set = Titems_set [cur_red_no];
			    CLEAR (items_set);
			    fill_items_set (tnt_trans_set, nt_trans_stack, plxi [xlas]);

			    if (!is_lr1_constructor) {
				/* Les look-ahead sets ne sont pas deja calcules */
				fill_look_ahead_set (items_set, la_set);
			    }

			    *(cur_nbt = nbt + cur_red_no) = 0;

			    for (t = -sxba_scan (la_set, 0); t != 1; t = -sxba_scan (la_set, -t)) {
				++*cur_nbt;

				if (ter [t] == 0) {
				    pter [++xpter] = t;
				    ter [t] = xtr1;
				}
				else {
				    if (conf [t] == 0) {
					pconf [++xpconf] = t;
				    }

				    maj (&xkod, 3);
				    S3 [xkod].kod = xtr1;
				    S3 [xkod].pkod = conf [t];
				    conf [t] = xkod;
				}
			    }

			    xlas++;
			    cur_red_no++;
			}
		    }
		}
#endif /* 0 */

		/* *NV* */
		{
		    SXINT	t, xtr1, lim, base_xtr1, r1;
		    SXINT	*cur_nbt;
		    SXBA la_set;

		    t = S1 [xac1].pred;
		    lim = S1 [t + 1].ptr;
		    base_xtr1 = S1 [t].ptr;

		    if (base_xtr1 < lim) {
			cur_red_no = 0;
			base_xlas = xlas;

			for (xtr1 = base_xtr1; xtr1 < lim; xtr1++) {
			    items_set = Titems_set [cur_red_no];
			    CLEAR (items_set);
			    fill_items_set (nt_trans_set, nt_trans_stack, plxi [xlas]);

			    if (!is_lr1_constructor) {
				/* Les look-ahead sets ne sont pas deja calcules */
				fill_look_ahead_set (items_set, look_ahead_sets [xlas]);
			    }

			    xlas++;
			    cur_red_no++;
			}

			cur_red_no = 0;
			xlas = base_xlas;
			xtr1 = base_xtr1;

			/* Attention: il ne peut y avoir de conflit Reduce/Reduce entre deux
			   productions etendues (leur LHS est un XNT) ayant meme representant. */
			/* On fusionne donc leur look-aheads */
			while (xtr1 < lim) {
			    if (has_xprod)
				r1 = 1+equiv_rule_processing (xtr1, lim, xlas, cur_red_no);
			    else
				r1 = 1;

			    la_set = look_ahead_sets [xlas];

			    *(cur_nbt = nbt + cur_red_no) = 0;

			    for (t = -sxba_scan (la_set, 0); t != 1; t = -sxba_scan (la_set, -t)) {
				++*cur_nbt;

				if (ter [t] == 0) {
				    pter [++xpter] = t;
				    ter [t] = xtr1;
				}
				else {
				    if (conf [t] == 0) {
					pconf [++xpconf] = t;
				    }

				    maj (&xkod, (SXINT)3);
				    S3 [xkod].kod = xtr1;
				    S3 [xkod].pkod = conf [t];
				    conf [t] = xkod;
				}
			    }

			    xtr1 += r1;
			    cur_red_no += r1;
			    xlas += r1;
			}
		    }
		}


		is_error_code_needed = FALSE;


/* T R A I T E M E N T   D E S   C O N F L I T S   */

/*
   R E G L E S    D E    D E S A M B I G U A T I O N
  
   The precedences and associativities are used by SYNTAX to resolve parsing
conflicts; they give rise to disambiguating rules. Formally the rules work as
follows.
  
User Disambiguating Rules (UDR):
     1.  The precedences and associativities are recorded for those tokens and
         literals that have them (specified in L.prio).
  
     2.  A precedence and associativity is associated with each grammar rule;
         it is the precedence and associativity of the last token or literal
         in the body of the rule. If the %prec construction is used, it
	 overrides this default. Some grammar rules may have no precedence and
	 associativity associated with them.
  
     3.  When there is a Reduce/Reduce conflict and one grammar rule has no
         precedence and associativity, or there is a Shift/Reduce conflict
         and either the input symbol or one Reduce grammar rule has no
	 precedence and associativity, then the System Disambiguating Rules
	 (SDR) are used and the conflicts are reported.
  
     4.  If there is a Shift/Reduce conflict, and both the grammar rule(s)
	 involved in the Reduce conflict(s) and the input token have
	 precedence and associativity associated with them, then the conflict
	 is resolved in favor of the action (Shift or Reduce) associated with
	 the higher precedence. If the precedences are the same, then the
	 associativity is used; left associative implies Reduce, right
	 associative implies Shift and nonassociating implies Error (at analysis time).
  
     5.  If there is a Reduce/Reduce conflict, and all the grammar rules have
	 precedence and associativity associated with them, then the conflict
	 is resolved in favor of the Reduce action associated with the higher
	 precedence. If the precedences are the same, then the SDR are used.
	 Associativity is meaningless in this case.
  
System Disambiguating Rules (SDR)
  
     6.  If there is a Shift/Reduce conflict and the UDR do not apply then the
         conflict is resolved in favor of the Shift action.
  
     7.  If there is a Reduce/Reduce conflict and the UDR do not apply then
	 the conflict is resolved in favor of the Reduce action involving the
	 rule which appears first in the grammar text.
*/

		if (xpconf > 0) {
		    if (is_lalr1) {
			is_lalr1 = FALSE;

			if (sxverbosep && !sxttycol1p) {
			    fputc ('\n', sxtty);
			    sxttycol1p = TRUE;
			}

			if (is_lr1_constructor) {
			    /* Il reste des conflits => non LR(1) */
			    is_lr1 = FALSE;
			    fprintf (sxstderr, "**** not  L R ( 1 ) ****\n");
			}
			else
			    fprintf (sxstderr, "**** not  L A L R ( 1 ) ****\n");

			if (is_list_user_conflicts || is_list_system_conflicts) {
			    open_listing (1, language_name, ME, ".la.l");
			}
		    }

		    if (sxverbosep) {
			putc ('?', sxtty);
			sxttycol1p = FALSE;
		    }

		    for (; xpconf >= 1; xpconf--) {
			SXINT	t;

			t = pconf [xpconf];

			if (ter [t] > 0) {
			    maj (&xkod, (SXINT)3);
			    S3 [xkod].pkod = conf [t];
			    S3 [xkod].kod = ter [t];
			    ter [t] = 0;
			    S3 [0].pkod = xkod;
			    chosen.conflict_kind = REDUCE_REDUCE;
			}
			else {
			    S3 [0].pkod = conf [t];
			    chosen.conflict_kind = SHIFT_REDUCE;
			}

			conf [t] = 0;
			set_chosen (USER, REDUCE, (SXINT)0, UNDEF_PRIO);

			if (chosen.conflict_kind == SHIFT_REDUCE) {
			    current_prio = t_priorities [-t];

			    if (current_prio.assoc == UNDEF_ASSOC)
				set_chosen (SYSTEM, SHIFT, (SXINT)0, UNDEF_PRIO);
			    else
				set_chosen (USER, SHIFT, (SXINT)0, current_prio);
			}

			/* min_reduce = bnf_ag.WS_TBL_SIZE.xnbpro + 1; */
			min_reduce = bnf_ag.WS_TBL_SIZE.actpro + 1; /* *NV* */

			{
			    SXINT	x, y, reduce;

			    x = 0;

			    while ((y = S3 [x].pkod) != 0) {
				reduce = bnf_ag.WS_INDPRO [tr [S3 [y].kod]].prolis;
				reduce = PROD_TO_RED (reduce); /* *NV* */
				min_reduce = (min_reduce < reduce) ? min_reduce : reduce;
				current_prio = r_priorities [reduce];

				if (current_prio.assoc == UNDEF_ASSOC || chosen.desambig_rule_kind == SYSTEM) {
				    if (chosen.conflict_kind == REDUCE_REDUCE)
					set_chosen (SYSTEM, REDUCE, min_reduce, current_prio);
				    else
					set_chosen (SYSTEM, SHIFT, (SXINT)0, current_prio);
				}
				else {
				    if (chosen.prio.value < current_prio.value)
					set_chosen (USER, REDUCE, reduce, current_prio);
				    else if (chosen.prio.value == current_prio.value)
				      if (chosen.action_taken == SHIFT) {
					    if (chosen.prio.assoc == current_prio.assoc) {
						if (current_prio.assoc == LEFT_ASSOC)
						    set_chosen (USER, REDUCE, reduce, current_prio);
						else if (current_prio.assoc == RIGHT_ASSOC) {
						    /* On ne change rien */
						}
						else
						    /* nonassoc */
						    set_chosen (USER, ERROR_ACTION, (SXINT)0, current_prio);
					    }
					    else
						/* REDUCE, ERROR_ACTION,
					       SYSTEM_ACTION */
						set_chosen (USER, SYSTEM_ACTION, (SXINT)0, current_prio);
				      }
				}

				x = y;
			    }
			}

			if (chosen.action_taken == SYSTEM_ACTION) {
			    if (chosen.conflict_kind == SHIFT_REDUCE)
				set_chosen (SYSTEM, SHIFT, (SXINT)0, UNDEF_PRIO);
			    else
				set_chosen (SYSTEM, REDUCE, min_reduce, UNDEF_PRIO);
			}

/* Mise a jour du decompte des conflits */

			if (chosen.conflict_kind == SHIFT_REDUCE)
			    if (chosen.desambig_rule_kind == USER)
				usr++;
			    else
				ssr++;
			else if (chosen.desambig_rule_kind == USER)
			    urr++;
			else
			    srr++;


/* Generation du code pour les conflits resolus en faveur de shift.
   Elaboration des messages d'erreur */

			if (is_listing_opened && ((chosen.desambig_rule_kind == SYSTEM && is_list_system_conflicts) ||
			     (chosen.desambig_rule_kind == USER && is_list_user_conflicts))) {
			    if (is_path) {
				if (!is_fsa_built) {
				    build_fsa (nucl, bnf_ag.FIRST);
				    is_fsa_built = TRUE;
				}

				if (!is_message && xac1 != 1) {
				    is_message = TRUE;
				    put_edit_ff ();
				    fprintf (listing, "SAMPLE PATH from state 1 to %ld\n\n", (long)xac1);
				    fputs (varstr_tostr (path (1, xac1)), listing);
				}
			    }

			    put_edit_nl (3);
			    fprintf (listing, "    N O T    %sL R (1)\n", chosen.conflict_kind == SHIFT_REDUCE ? "" :
				 "L A ");
			    fprintf (listing, "%s-Reduce conflict in state %ld on %s between:\n", chosen.conflict_kind ==
				 SHIFT_REDUCE ? "Shift" : "Reduce", (long)xac1, varstr_tostr (cat_t_string (varstr_raz (vstr),
				 t)));
			}

			{
			    SXINT		LALR1_xptat;
			    SXINT		bxac1 = S1 [S1 [xac1].pred].ptr;

			    LALR1_xptat = tt [-ter [t]];

			    {
				SXINT	y, LALR1_item;

				y = 0;

				while ((y = S3 [y].pkod) != 0) {
				    SXINT		xtr1, reduce;

				    reduce = bnf_ag.WS_INDPRO [LALR1_item = tr [xtr1 = S3 [y].kod]].prolis;
				    reduce = PROD_TO_RED (reduce); /* *NV* */
				    cur_red_no = xtr1 - bxac1;

/* L'action reduce si elle existe sera generee en fin d'etat */

				    if (chosen.action_taken != REDUCE || chosen.reduce_no != reduce) {
					(nbt [cur_red_no])--;
				    }
				    else {
					ter [t] = xtr1;
				    }

				    if (is_listing_opened && ((chosen.desambig_rule_kind == SYSTEM &&
					 is_list_system_conflicts) || (chosen.desambig_rule_kind == USER &&
					 is_list_user_conflicts))) {
					SXINT	*LALR1_items_set = Titems_set [cur_red_no];

					put_edit (9, "- Reduce:");
					soritem (LALR1_item, 9, 81, "", "");
					put_edit (17, "derived from:");

					for (LALR1_item = LALR1_items_set [0]; LALR1_item > 0; LALR1_item = LALR1_items_set [LALR1_item]) {
					    if (t_in_deriv (t, LALR1_item)) {
						soritem (LALR1_item - 1, 17, 91, "", "");

						if (is_path) {
						    out_reduce_derivation (t, LALR1_item, plxi [base_xlas + cur_red_no]);
						}
					    }
					}
				    }
				}
			    }

			    if (chosen.conflict_kind == SHIFT_REDUCE && is_listing_opened && ((chosen.desambig_rule_kind
				 == SYSTEM && is_list_system_conflicts) || (chosen.desambig_rule_kind == USER &&
				 is_list_user_conflicts))) {
				/* L'action Shift sera generee plus tard */
				SXINT	xtat1, lim;

				put_edit (9, "- Shift:");
				lim = S1 [LALR1_xptat + 1].ptat;

				for (xtat1 = S1 [LALR1_xptat].ptat; xtat1 < lim; xtat1++) {
				    soritem (tat [xtat1], 9, 81, "", "");

				    if (is_path) {
					put_edit (17, "derivation:");
					out_shift_derivation (/* 1, xac1, */tat [xtat1]);
				    }
				}

				if (is_path && is_lr1) {
				    is_lr1 = FALSE;

				    if (sxverbosep && !sxttycol1p) {
					fputc ('\n', sxtty);
					sxttycol1p = TRUE;
				    }

				    fprintf (sxstderr, "**** not  L R ( 1 ) ****\n");
				}
			    }


/* On regarde si la grammaire est ambigue (une condition suffisante) */


			    if (is_path && is_listing_opened && ((chosen.desambig_rule_kind == SYSTEM &&
				 is_list_system_conflicts) || (chosen.desambig_rule_kind == USER && is_list_user_conflicts))
				 ) {
				SXINT	y, LALR1_item_2;
				BOOLEAN		is_local_ambiguity = FALSE;

				y = 0;

				while ((y = S3 [y].pkod) != 0) {
				    SXINT	*LALR1_items_set_2 = Titems_set [cur_red_no = S3 [y].kod - bxac1];

				    for (LALR1_item_2 = LALR1_items_set_2 [0]; LALR1_item_2 > 0; LALR1_item_2 = LALR1_items_set_2 [LALR1_item_2]) {
					if (t_in_deriv (t, LALR1_item_2) && check_ambig (t, LALR1_item_2, plxi [base_xlas + cur_red_no]))
					     {
					    is_local_ambiguity = TRUE;

					    if (!is_ambiguous) {
						is_ambiguous = TRUE;
						is_lr1 = FALSE;

						if (sxverbosep && !sxttycol1p) {
						    fputc ('\n', sxtty);
						    sxttycol1p = TRUE;
						}

						fprintf (sxstderr, "**** A M B I G U O U S ****\n");
					    }
					}
				    }
				}


/* On regarde si la grammaire est LR(1) et si non on dit pourquoi */

				if (chosen.conflict_kind == REDUCE_REDUCE && !is_local_ambiguity) {
				    SXINT	z, nt_trans1;

				    y = 0;
				    sxba_1_bit (sxba_empty (t_set), -t);

				    while ((y = S3 [y].pkod) != 0) {
					nt_trans1 = plxi [base_xlas + S3 [y].kod - bxac1];
					z = y;

					while ((z = S3 [z].pkod) != 0) {
					    if (!check_lr1 (xac1, t_set, nt_trans1, plxi [base_xlas + S3 [z].kod - bxac1]
						 , FALSE, TRUE)) {
						if (is_lr1) {
						    is_lr1 = FALSE;

						    if (sxverbosep && !sxttycol1p) {
							fputc ('\n', sxtty);
							sxttycol1p = TRUE;
						    }

						    fprintf (sxstderr, "**** not  L R ( 1 ) ****\n");
						}

						break /* Un seul exemple */ ;
					    }
					}

					if (z != 0)
					    break /* Un seul exemple */ ;
				    }
				}
			    }
			}

			if (chosen.action_taken == ERROR_ACTION)
			    is_error_code_needed = TRUE;

			if (is_listing_opened && ((chosen.desambig_rule_kind == SYSTEM && is_list_system_conflicts) ||
			     (chosen.desambig_rule_kind == USER && is_list_user_conflicts))) {
			    fprintf (listing, "Using the %s disambiguating rules", chosen.desambig_rule_kind == USER ?
				 "user" : "system");

			    if (chosen.action_taken == ERROR_ACTION)
				fprintf (listing, " (non associativity), the action \"error\" is generated.\n");
			    else {
				fputs (", priority is given to ", listing);

				if (chosen.action_taken == SHIFT)
				    fputs ("shift.\n", listing);
				else
				    fprintf (listing, "reduction number %ld.\n", (long)chosen.reduce_no);
			    }
			}
		    }
		}


/* Recherche de la reduction la plus populaire */

/* Du fait du traitement des conflits tous les reduce ont pu disparaitre, dans
   ce cas on produit le code erreur */

/* S'il y a au moins un terminal etendu dans le coup, on ne fait rien; on laisse
   l'optimiseur s'en occuper */

		{
		    SXINT	xt;

		    is_xt = FALSE;

		    for (xt = -bnf_ag.WS_TBL_SIZE.xtmax; xt < bnf_ag.WS_TBL_SIZE.tmax; xt++) {
			if (ter [xt] != 0) {
			    is_xt = TRUE;
			    break;
			}
		    }
		}

		{
		    SXINT	n, xtr1, lim, cur_nbt, red_no, x;

		    n = plulong = 0;

		    if (!is_xt && !is_error_code_needed) {
			lim = S1 [(x = S1 [xac1].pred) + 1].ptr;
			cur_red_no = 0;

			for (xtr1 = S1 [x].ptr; xtr1 < lim; xtr1++) {


/* Il y a[vait] au moins un Reduce */
		    

			    if ((cur_nbt = nbt [cur_red_no]) > n) {
				red_no = PROD_TO_RED (bnf_ag.WS_INDPRO [tr [xtr1]].prolis); /* *NV* */

				if (is_tspe && (sxba_bit_is_set (s_red_set, red_no) ||
						bnf_ag.WS_NBPRO [red_no].bprosimpl)) {
				    /* la plus populaire sera generee ulterieurement dans OPTIM */
				    plulong = 0;
				    break;
				}

				n = cur_nbt;
				plulong = red_no; /* *NV* */
				/* plulong = xtr1; */
			    }

			    cur_red_no++;

			    /* *NV* */
			    if (has_xprod)
				cur_red_no += equiv_rule_nb (xtr1, lim);
			}
		    }
		}

		if (plulong != 0) {
		    /* On genere la reduction la plus populaire */
		    t_fe->codop = Reduce;
		    t_fe->reduce = plulong; /* *NV* */
		    /* t_fe->reduce = bnf_ag.WS_INDPRO [tr [plulong]].prolis; */
		    t_fe->pspl = LGPROD (t_fe->reduce);
		    t_fe->next = bnf_ag.WS_NBPRO [t_fe->reduce].reduc;
		    sxba_0_bit (not_used_red_set, t_fe->reduce);
		    pop_fe = put_in_fe (t_fe, t_hash, t_lnks, t_fe);
		}
		else
		    pop_fe = 0;


/* Generation du Floyd_Evans pour les t_tables */

/* On ne s'occupe pas [trop] des predicats c'est l'optimiseur qui produira le code correspondant */

		{
		    SXINT			xt, xtr1, red_no;
		    struct P_item	*aitem;
		    struct state	*astate;

		    astate = t_states + xac1;
		    /* astate->lgth = 0; inutile (calloc) */
		    astate->start = xt_items;
		    sxba_empty (t_set);

		    for (xt = 1; xt < -limt; xt++) {
			if ((xtr1 = ter [-xt]) != 0) {
			    ter [-xt] = 0;

			    if (xt > -bnf_ag.WS_TBL_SIZE.tmax) {
				xprdct_items++;
				sxba_1_bit (t_set, XT_TO_T_CODE (xt));

				if (xt_state_set == NULL)
				    xt_state_set = sxba_calloc (xe1 + 1);

				sxba_1_bit (xt_state_set, xac1);
			    }

			    if (xtr1 < 0 /* Shift */ ) {
				struct S1	*aS1;

				if ((t_fe->next = (aS1 = S1 + tt [-xtr1])->tsuct) == 0) {
				    red_no = PROD_TO_RED (bnf_ag.WS_INDPRO [tat [aS1->ptat]].prolis); /* *NV* */
				    /* red_no = bnf_ag.WS_INDPRO [tat [aS1->ptat]].prolis; */

				    if ((t_fe->reduce = red_no) == 0) {
					/* Generation de l'instruction Halt */
					t_fe->codop = Reduce;
					t_fe->pspl = 2;
					t_fe->next = 1;
				    }
				    else {
					t_fe->codop = ScanReduce;
					t_fe->pspl = LGPROD (t_fe->reduce) - 1;
					t_fe->next = bnf_ag.WS_NBPRO [t_fe->reduce].reduc;
					sxba_0_bit (not_used_red_set, t_fe->reduce);
				    }
				}
				else {
				    t_fe->codop = ScanShift;
				    t_fe->reduce = 0;
				    t_fe->pspl = S1 [t_fe->next].shftnt == 0 ? 0 : t_fe->next;
				}
			    }
			    else /* Reduce */  {
				red_no = PROD_TO_RED (bnf_ag.WS_INDPRO [tr [xtr1]].prolis); /* *NV* */

				if (red_no == plulong) /* *NV* */
				    /* if (xtr1 == plulong) */
				    continue;

				t_fe->codop = Reduce;
				t_fe->reduce = red_no;
				/* t_fe->reduce = bnf_ag.WS_INDPRO [tr [xtr1]].prolis; */
				t_fe->pspl = LGPROD (t_fe->reduce);
				t_fe->next = bnf_ag.WS_NBPRO [t_fe->reduce].reduc;
				sxba_0_bit (not_used_red_set, t_fe->reduce);
			    }

			    aitem = t_items + xt_items++;
			    aitem->check = xt;
			    astate->lgth++;
			    aitem->action = put_in_fe (t_fe, t_hash, t_lnks, t_fe);
			}
		    }

		    if (is_xt) {
			xpredicates += sxba_cardinal (t_set) /* nb de sequences */;
		    }


/* Generation du defaut */

		    aitem = t_items + xt_items++;
		    aitem->check = any;

		    if (is_error_code_needed) {
			if (force_error == 0) {
			    t_fe->codop = ForceError;
			    t_fe->reduce = t_fe->pspl = t_fe->next = 0;
			    force_error = put_in_fe (t_fe, t_hash, t_lnks, t_fe);
			}

			aitem->action = force_error;
		    }
		    else
			aitem->action = pop_fe == 0 ? t_error : pop_fe;

		    astate->lgth++;
		}
	    }
	}

	xprdct_items += xpredicates /* Nombre de &Else */ ;


/* Recapitulatif au terminal des conflits */

	if (!is_lalr1) {
	    char	results [44];

	    if (sxverbosep && !sxttycol1p) {
		fputc ('\n', sxtty);
		sxttycol1p = TRUE;
	    }

	    sprintf (results, "   %-4ld   | %-4ld |  %-4ld  | %-4ld |  %-4ld\n", (long)usr + ssr + urr + srr, (long)usr, (long)ssr, (long)urr, (long)srr);
	    fputs (header, sxstderr);
	    fputs (results, sxstderr);

	    if (is_listing_opened) {
		put_edit_ff ();
		fputs (header, listing);
		fputs (results, listing);
	    }
	}

	if (is_automaton && open_listing (1, language_name, ME, ".la.l")) {
	    VARSTR	varstring;
	    SXINT	xac1, x, z;
	    SXINT		lim;

	    if (sxverbosep) {
		if (!sxttycol1p) {
		    fputc ('\n', sxtty);
		    sxttycol1p = TRUE;
		}

		fputs ("\tAutomaton Listing Output\n", sxtty);
	    }

	    varstring = varstr_alloc (256);
	    put_edit_ff ();
	    put_edit (1, "*****        A U T O M A T O N       *****");
	    xlas = 1;

	    for (xac1 = 1; xac1 < xac2; xac1++) {
		x = nucl [S1 [xac1].etat] - 1;
		put_edit_nl (6);
		put_edit_nnl (1, "STATE ");
		put_edit_apnb (xac1);

		if (xac1 != 1) {
		    put_edit_nnl (11, "Transition on ");
		    z = bnf_ag.WS_INDPRO [x].lispro;
		    put_edit_apnnl ((z > 0) ? get_nt_string (z) : varstr_tostr (cat_t_string (varstr_raz (vstr), z)));
		    put_edit_apnnl (" from ");

		    if (lstprd [S1 [xac1].pprd].f2 == 0) {
			put_edit_apnnl ("state ");
			put_edit_apnb ((long)lstprd [S1 [xac1].pprd].f1);
			put_edit_nl (1);
		    }
		    else {
			SXINT	j;

			put_edit_ap ("states:");
			lstprd [0].f1 = 0;
			lstprd [0].f2 = S1 [xac1].pprd;
			j = 11;

			for (x = lstprd [0].f2; x != 0; x = lstprd [x].f2) {
			    put_edit_fnb (j, 4, lstprd [x].f1);

			    if (lstprd [x].f2 != 0)
				put_edit_apnnl (",");

			    if ((j += 5) >= 81) {
				put_edit_nl (1);
				j = 11;
			    }
			}
		    }
		}

		sornt (S1 [xac1].shftnt);
		sort (S1 [xac1].shftt);
		put_edit (1, star_71);
		x = S1 [xac1].pred;
		lim = S1 [x + 1].ptr;

		for (x = S1 [x].ptr; x < lim; x++) {
		    soritem (tr [x], 1, 71, "*", laset (varstring, 60, look_ahead_sets [xlas++]));
		}

		put_edit (1, star_71);
	    }

	    varstr_free (varstring);
	}

	{
	    SXINT	i;

	    for (i = max_red_per_state - 1; i >= 0; i--)
		sxfree (Titems_set [i]);

	    sxfree (Titems_set);
	    sxfree (nt_trans_stack);
	    sxfree (nt_trans_set);
	    /* sxbm_free (FIRST); *NV* */
	    /* sxfree (nullable_set); */
	    sxfree (xor_hash);
	}

	if (is_fsa_built)
	    erase_fsa ();

	prio_free (&t_priorities, &r_priorities);
	sxfree (nbt), nbt = NULL;
	sxfree (conf + limt), conf = NULL;
	sxfree (ter + limt), ter = NULL;
	sxfree (pconf), pconf = NULL;
	sxfree (pter), pter = NULL;
    }

    sxbm_free (look_ahead_sets);
    sxfree (plxi), plxi = NULL;
    sxfree (t_set), t_set = NULL;
    sxfree (nucl), nucl = NULL;
    sxfree (ptrans), ptrans = NULL;
    sxfree (includes), includes = NULL;
    sxfree (tr), tr = NULL;
    sxfree (S3), S3 = NULL;
    sxfree (xor_StNts), xor_StNts = NULL;
    sxfree (H1 - 31), H1 = NULL;

    XxY_free (&STATExNT_hd);
    sxfree (STATExNT2tnt), STATExNT2tnt = NULL;
    sxfree (tnt_next), tnt_next = NULL;

    if (!is_tspe) {
	sxfree (tnt), tnt = NULL;
	sxfree (tant), tant = NULL;
	sxfree (tat), tat = NULL;
	sxfree (tt), tt = NULL;
	sxfree (lstprd), lstprd = NULL;
	sxfree (S1), S1 = NULL;
    }


    {

/* On verifie que toutes les reductions sont generees.
   Le traitement des conflits ayant pu en supprimer. */

	SXINT	i;

	if (!sxba_is_empty (not_used_red_set)) {
	    /* Il y a des reductions non generees */
	    static char		ws [] = "\nWarning :\tThe following grammar rules are not recognized:\n";

	    fputs (ws, sxstderr);

	    if (is_listing_opened) {
		put_edit_ff ();
		put_edit_apnnl (ws);
	    }

	    for (i = sxba_scan (not_used_red_set, 0); i != -1; i = sxba_scan (not_used_red_set, i)) {
		prod_to_str (vstr, i);
		fputs (varstr_tostr (vstr), sxstderr);

		if (is_listing_opened)
		    put_edit_apnnl (varstr_tostr (vstr));
	    }

	    sxttycol1p = TRUE;
	}

	if (is_tspe)
	    sxfree (s_red_set);

	sxfree (not_used_red_set);
    }

    {
	/* On alloue et initialise certaines structures de OPTIM pour pouvoir sortir le
	   Floyd_Evans si long_listing a ete demande */

	SXINT	i;

	t_state_equiv_class = (SXINT*) sxalloc (xe1, sizeof (SXINT));
	nt_state_equiv_class = (SXINT*) sxalloc (xac2, sizeof (SXINT));
	nt_to_ad = (SXINT*) sxcalloc (bnf_ag.WS_TBL_SIZE.xntmax + 1, sizeof (SXINT));
	nt_to_nt = (SXINT*) sxalloc (bnf_ag.WS_TBL_SIZE.xntmax + 1, sizeof (SXINT));
	NTMAX = bnf_ag.WS_TBL_SIZE.ntmax;

	for (i = 0; i < xe1; i++)
	    t_state_equiv_class [i] = (t_states [i].lgth == 0) ? 0 : i;

	for (i = 0; i < xac2; i++)
	    nt_state_equiv_class [i] = (nt_states [i].lgth == 0) ? 0 : i;

	for (i = 1; i <= bnf_ag.WS_TBL_SIZE.xntmax; i++) {
	    nt_to_nt [i] = i;
	}
    }

    if (is_long_listing && open_listing (1, language_name, ME, ".la.l")) {
	struct state	*astate;
	struct P_item	*aitem, *lim;
	SXINT	i;

	if (sxverbosep) {
	    fprintf (sxtty, "\tNon Optimized Floyd-Evans Listing Output\n");
	    sxttycol1p = TRUE;
	}

	put_edit_ff ();
	put_edit_ap ("************ T _ T A B L E S *************");
	put_edit_nl (2);
	put_edit_nnl (11, "Parsing starts in Etq ");
	put_edit_apnb ((long)init_state);

	put_edit_nl (2);
	put_edit_nnl (11, "size = ");
	put_edit_apnb ((long)t_lnks [0]);
	put_edit_nl (2);

	for (i = 1; i < xe1; i++) {
	    astate = t_states + i;

	    if (astate->lgth != 0 /* etat non vide */ ) {
		put_edit_nl (1);
		put_edit_nnl (1, "Etq ");
		put_edit_apnb ((long)i);

		for (lim = (aitem = t_items + astate->start) + astate->lgth; aitem < lim; aitem++) {
		    if (aitem->action != 0)
			output_fe (Terminal, aitem->action, aitem->check, t_fe + aitem->action);
		}
	    }
	}

	put_edit_ff ();
	put_edit_ap ("************ N T _ T A B L E S *************");
	put_edit_nl (2);
	put_edit_nnl (11, "size = ");
	put_edit_apnb ((long)nt_lnks [0]);
	put_edit_nl (2);

	for (i = 1; i < xac2; i++) {
	    astate = nt_states + i;

	    if (astate->lgth != 0 /* etat non vide */ ) {
		put_edit_nl (1);
		put_edit_nnl (1, "State ");
		put_edit_apnb ((long)i);

		for (lim = (aitem = nt_items + astate->start) + astate->lgth; aitem < lim; aitem++) {
		    if (aitem->action != 0)
			output_fe (NonTerminal, aitem->action, aitem->check, nt_fe + aitem->action);
		}
	    }
	}
    }

    if (is_listing_opened) {
	put_edit_finalize ();
	is_listing_opened = FALSE;
    }

    nd_degree = -1;

    return ssr + srr != 0;
}
