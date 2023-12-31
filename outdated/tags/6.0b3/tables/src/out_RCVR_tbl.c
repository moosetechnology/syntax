/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1993 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *  Produit de l'equipe Langages et Traducteurs.  (pb)  *
   *                                                      *
   ******************************************************** */





/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030523 14:14 (phd):	Ajout de l'�l�ment -1 de SXPBM_trans	*/
/************************************************************************/
/* 20030506 17:08 (phd):	Modifs pour calmer le compilo SGI	*/
/************************************************************************/
/* 15-02-93 14:25 (pb):		Ajout de cette rubrique			*/
/************************************************************************/

static char	ME [] = "out_RCVR_tbl";

#include "sxunix.h"
#include "tables.h"
#include "out.h"
#include "sxba.h"
char WHAT_TABLESOUTRCVRTBL[] = "@(#)SYNTAX - $Id: out_RCVR_tbl.c 1120 2008-02-28 15:53:59Z rlacroix $" WHAT_DEBUG;

#define Commun 1
#define Propre 2
#define Defaut 3
#define Done   4

static     SXBA	*TNT_trans_plus, *T_trans, t_line_0, t_line;

#if 0
/* Remplace le 2 avril 2002 par un appel a sxba_to_c */
static	out_SXBA (char *nom, SXINT suffixe, SXBA set)
{
    /* static SXBA_ELT nom []={...}; */
    SXINT		nb_bits, nb_word, nb_slice ;
    SXINT		j, i, d;

    nb_bits = BASIZE(set);
    nb_word = NBLONGS (nb_bits);
    nb_slice = (nb_word + 1) / slice_length;
    printf ("static SXBA_ELT %s%u[]={\n", nom, suffixe);
    d = 0;

    for (i = 1; i <= nb_slice; i++) {
	for (j = d, d += slice_length; j < d; j++) {
	    printf ("%u,", set [j]);
	}

	putchar (NEWLINE);
    }

    for (j = d; j <= nb_word; j++) {
	printf ("%u,", set [j]);
    }

    out_end_struct ();
}
#endif /* 0 */


#if 0
SXSHORT		sxP_access (struct P_base *abase,
			    SXINT j)
{
    struct P_item	*ajvector, *ajrefvector;
    SXINT			ref = abase->commun;

    if (ref <= PC.Mref /* codage direct */ )
	return (abase->propre == j /* checked */ ) ? ref : abase->defaut;

    if ((ajrefvector = (ajvector = P.vector + j) + ref - PC.deltavec)->check == j /* dans commun */
	|| (ref = abase->propre) != 0 /* propre est non vide */
	&& (ajrefvector = ajvector + ref - PC.deltavec)->check == j /* dans propre */)
	return ajrefvector->action;

    return abase->defaut;
}
#endif

static BOOLEAN	set_next_item (SXINT base, SXINT *check, SXINT *action, SXINT Max)
{
    struct P_item	*aitem;

    base -= PC.deltavec;

    if (base + *check < 0)
	*check = -base;

    if (base + Max > PC.Mvec)
	Max = PC.Mvec - base;

    for (aitem = P.vector + base + ++*check; *check <= Max; aitem++, ++*check)
	if (aitem->check == *check) {
	    *action = aitem->action;
	    return TRUE;
	}

    return FALSE;
}

static VOID	set_first_trans (struct P_base *abase,
				 SXINT *check,
				 SXINT *action, 
				 SXINT Max, 
				 SXINT *next_action_kind)
{
    SXINT			ref = abase->commun;

    if (ref <= PC.Mref /* codage direct */ ) {
	if ((*check = abase->propre) != 0) {
	    *action = ref;
	    *next_action_kind = Defaut;
	}
	else /* defaut */  {
	    *action = abase->defaut;
	    *next_action_kind = Done;
	}
    }
    else {
	*check = 0;
	set_next_item (ref, check, action, Max) /* always true (commun non vide) */ ;
	*next_action_kind = Commun;
    }
}

static BOOLEAN	set_next_trans (struct P_base *abase,
				SXINT *check, 
				SXINT *action, 
				SXINT Max, 
				SXINT *next_action_kind)
{
    switch (*next_action_kind) {
    case Commun:
	if (set_next_item (abase->commun, check, action, Max))
	    return TRUE;

	if (abase->propre != 0 /* partie propre non vide */ ) {
	    *next_action_kind = Propre;
	    *check = 0;

    case Propre:
	    if (set_next_item (abase->propre, check, action, Max))
	        return TRUE;
	}

    case Defaut:
	*check = 0;
	*action = abase->defaut;
	*next_action_kind = Done;
	return TRUE;

    default:
	sxtrap ("out_RCVR_tbl", "set_next_trans");
	  /*NOTREACHED*/

    case Done:
	return FALSE;
    }
}

SXVOID set_NT_trans (SXINT x, SXINT ref)
{
    if (ref > 0)
	sxtrap (ME, "out_rcvr_trans");
    
    ref = -ref;
    
    if (ref > PC.Mref)
	sxtrap (ME, "set_NT_trans");
    
    if ((ref -= PC.Mprdct) > 0) {
	SXBA_1_bit (TNT_trans_plus [ref], x);
    }
}


SXVOID set_T_trans (SXINT x, SXINT ref)
{
    if (ref > PC.Mref)
	sxtrap (ME, "set_T_trans");
    
    if ((ref -= PC.Mprdct) > 0) {
	SXBA_1_bit (TNT_trans_plus [ref], x);
    }
}


SXVOID out_rcvr_trans (void)
{
    /* On construit la BM T_trans qui a chaque terminal cle t associe l'ensemble
       des ref (a Mprdct pres) (chaque ref identifie un couple q = (etat, etq)) tel
       que l'on a "q ->*X q' ->t" cad il existe entre etat et etat' une
       sequence (eventuellement vide) de transitions (terminales ou non-terminales)
       et il existe depuis etq' une transition sur t. */
       
    /* Les Mref-M0ref premiers index de rcvr_nt_states referencent des etats
       tels que state==etq, les suivants sont paralleles aux elements de gotos. */


    SXINT			x, t, nt, nt_top; 
    SXINT			ref, next_action_kind;
    struct P_base	*abase;

    nt_top = PC.Mref - PC.Mprdct;
    TNT_trans_plus = sxbm_calloc (nt_top + 1, nt_top + 1);
    T_trans = sxbm_calloc (PC.tmax + 1, nt_top + 1);

    for (x = 1; x <= nt_top; x++) {
	if (x <= PC.M0ref - PC.Mprdct) {
	    abase = P.nt_bases + x;
	    set_first_trans (abase, &nt, &ref, PC.ntmax, &next_action_kind);
		
	    do {
		set_NT_trans (x, ref);
	    } while (set_next_trans (abase, &nt, &ref, PC.ntmax, &next_action_kind));
	}
	
#if 0	
/* Si on ne tient pas compte des transitions terminales... */	
	t = 0;

	while ((t = sxba_scan (R.PER_tset, t)) > 0) {
	    if ((ref = sxP_access (P.t_bases + etq, t)) > 0)
		SXBA_1_bit (T_trans [t], x);
	}
#endif

	abase = P.t_bases + x;
	set_first_trans (abase, &t, &ref, PC.tmax, &next_action_kind);
	
	do {
	    set_T_trans (x, ref);

	    if (ref > 0 && SXBA_bit_is_set (R.PER_tset, t))
		SXBA_1_bit (T_trans [t], x);
	} while (set_next_trans (abase, &t, &ref, PC.tmax, &next_action_kind));
    }

    /* Fermeture transitive */
    fermer (TNT_trans_plus, nt_top + 1);

    t = 0;

    while ((t = sxba_scan (R.PER_tset, t)) > 0) {
	t_line = T_trans [t];
	t_line_0 = T_trans [0];
	sxba_copy (t_line_0, t_line);
	x = 0;

	while ((x = sxba_scan (t_line_0, x)) > 0) {
	    sxba_or (t_line, TNT_trans_plus [x]);
	}
    }

    t = 0;

    while ((t = sxba_scan (R.PER_tset, t)) > 0) {
	char	str [10];
	/* out_SXBA ("SXPBA_kt", t, T_trans [t]); le 2 avril 2002 */
	sprintf (str, "%ld", (long)t);
	sxba_to_c (T_trans [t], stdout, "SXPBA_kt", str, TRUE /* static */);
    }

    puts ("static SXBA SXPBM_trans[]={NULL,");
    t = 0;

    while ((t = sxba_scan (R.PER_tset, t)) > 0) {
	printf ("&SXPBA_kt%ld[0],\n", (long)t);
    }

    out_end_struct ();

    sxbm_free (T_trans);
    sxbm_free (TNT_trans_plus);
}
