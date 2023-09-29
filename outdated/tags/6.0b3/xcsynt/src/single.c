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
/* 13-10-92 16:25 (pb):		Modif de partial_spe (ajout de		*/
/*				restriction_set).			*/
/************************************************************************/
/* 24-08-88 18:53 (bl):		Ajout de #ifdef MSDOS			*/
/************************************************************************/
/* 07-06-88 14:10 (pb):		Non RAZ de variables statiques pour	*/
/*			executions multiples dans le meme process	*/
/************************************************************************/
/* 17-02-88 14:06 (pb):		Adaptation aux XNT (V3.2)		*/
/************************************************************************/
/* 11-01-88 09:50 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/
#if 0
static char	ME [] = "SINGLE";
#endif
#include "P_tables.h"

#include "csynt_optim.h"
char WHAT_XCSYNTSINGLE[] = "@(#)SYNTAX - $Id: single.c 1128 2008-03-04 11:49:22Z rlacroix $" WHAT_DEBUG;


static SXINT	xnt_trans, nt_trans_size, max_lhs;
static struct nt_trans {
	   SXINT	lnk, check, action;
       }	*nt_trans;
static struct nt_etat {
	   SXINT	lnk, lgth;
       }	*nt_etats;
static SXINT	xnt_size, Delta, xntstr;
static SXINT	*xnt_lnk;
static SXINT	xnt_hash [HASH_SIZE];




static SXINT	set_an_xnt (SXINT nt, SXINT prdct)
{
    SXINT	x, y, hash;
    BOOLEAN	is_new_xnt = nt > bnf_ag.WS_TBL_SIZE.xntmax;
    char	s [12];

    for (y = 0, x = xnt_hash [hash = (2 * nt + prdct) % HASH_SIZE]; x != 0; x = xnt_lnk [(y = x) - Delta]) {
	if (bnf_ag.XNT_TO_NT_PRDCT [x - bnf_ag.WS_TBL_SIZE.ntmax].v_code == nt && bnf_ag.XNT_TO_NT_PRDCT [x - bnf_ag.WS_TBL_SIZE.ntmax].prdct_no == prdct)
	    return x;
    }

    if (++bnf_ag.WS_TBL_SIZE.xntmax > xnt_size + bnf_ag.WS_TBL_SIZE.ntmax) {
	xnt_size *= 2;
	bnf_ag.XNT_TO_NT_PRDCT = (struct xv_to_v_prdct_s*) sxrealloc (bnf_ag.XNT_TO_NT_PRDCT, xnt_size + 1, sizeof (
	     struct xv_to_v_prdct_s));
	xnt_lnk = (SXINT*) sxrealloc (xnt_lnk, xnt_size, sizeof (SXINT));
	bnf_ag.ADR = (SXINT*) sxrealloc (bnf_ag.ADR - bnf_ag.WS_TBL_SIZE.xtmax - 1, bnf_ag.WS_TBL_SIZE.xtmax + bnf_ag.
	     WS_TBL_SIZE.ntmax + 3 + xnt_size, sizeof (SXINT)) + bnf_ag.WS_TBL_SIZE.xtmax + 1;
    }

    bnf_ag.XNT_TO_NT_PRDCT [bnf_ag.WS_TBL_SIZE.xntmax - bnf_ag.WS_TBL_SIZE.ntmax].v_code = nt;
    bnf_ag.XNT_TO_NT_PRDCT [bnf_ag.WS_TBL_SIZE.xntmax - bnf_ag.WS_TBL_SIZE.ntmax].prdct_no = prdct;

    if (is_new_xnt)
	sprintf (s, ">%ld<", (long)nt);
    else
	sprintf (s, " &%ld", (long)prdct);

    if (xntstr + strlen (s) + 1 + (is_new_xnt ? 0 : get_nt_length (nt)) > (SXUINT)bnf_ag.WS_TBL_SIZE.nt_string_length) {
	bnf_ag.WS_TBL_SIZE.nt_string_length += 16 * xnt_size;
	bnf_ag.NT_STRING = (char*) sxrealloc (bnf_ag.NT_STRING, bnf_ag.WS_TBL_SIZE.nt_string_length, sizeof (char));
    }

    if (is_new_xnt)
	sprintf (bnf_ag.NT_STRING + xntstr, ">%ld<", (long)nt);
    else
	sprintf (bnf_ag.NT_STRING + xntstr, "%s &%ld", get_nt_string (nt), (long)prdct);

    xntstr += strlen (bnf_ag.NT_STRING + xntstr) + 1;
    bnf_ag.ADR [bnf_ag.WS_TBL_SIZE.xntmax + 1] = xntstr;

    if (y == 0)
	xnt_hash [hash] = bnf_ag.WS_TBL_SIZE.xntmax;
    else
	xnt_lnk [y - Delta] = bnf_ag.WS_TBL_SIZE.xntmax;

    xnt_lnk [bnf_ag.WS_TBL_SIZE.xntmax - Delta] = 0;
    return bnf_ag.WS_TBL_SIZE.xntmax;
}



static SXINT	get_next_xnt (SXINT nt, SXINT prdct)
{
    /* Fabrique pour tout couple (nt, prdct) le xnt correspondant
       Si nt == 0, cree en plus un nouveau non terminal */
    SXINT	x, y, z, hash;

    if (nt == 0)
	nt = bnf_ag.WS_TBL_SIZE.xntmax + 1;
    else if (nt <= bnf_ag.WS_TBL_SIZE.ntmax && prdct == -1)
	return nt;

    if (xnt_lnk == NULL) {
	/* On cree les structures */
	xnt_size = 10 + bnf_ag.WS_TBL_SIZE.xntmax - bnf_ag.WS_TBL_SIZE.ntmax;
	Delta = bnf_ag.WS_TBL_SIZE.ntmax + 1;
	xnt_lnk = (SXINT*) sxcalloc (xnt_size, sizeof (SXINT));
	bnf_ag.ADR = (SXINT*) sxrealloc (bnf_ag.ADR - bnf_ag.WS_TBL_SIZE.xtmax - 1, bnf_ag.WS_TBL_SIZE.xtmax + bnf_ag.
	     WS_TBL_SIZE.ntmax + 3 + xnt_size, sizeof (SXINT)) + bnf_ag.WS_TBL_SIZE.xtmax + 1;
	xntstr = bnf_ag.WS_TBL_SIZE.nt_string_length;
	bnf_ag.WS_TBL_SIZE.nt_string_length += 16 * 10;
	bnf_ag.NT_STRING = (char*) sxrealloc (bnf_ag.NT_STRING, bnf_ag.WS_TBL_SIZE.nt_string_length, sizeof (char));

	/* Executions multiples */
	for (x = 0; x < HASH_SIZE ; x++)
	    xnt_hash [x] = 0;

	if (bnf_ag.XNT_TO_NT_PRDCT == NULL)
	    bnf_ag.XNT_TO_NT_PRDCT = (struct xv_to_v_prdct_s*) sxalloc (xnt_size + 1, sizeof (struct xv_to_v_prdct_s));
	else {
	    bnf_ag.XNT_TO_NT_PRDCT = (struct xv_to_v_prdct_s*) sxrealloc (bnf_ag.XNT_TO_NT_PRDCT, xnt_size + 1,
		 sizeof (struct xv_to_v_prdct_s));

	    for (x = bnf_ag.WS_TBL_SIZE.ntmax + 1; x <= bnf_ag.WS_TBL_SIZE.xntmax; x++) {
		for (y = 0, z = xnt_hash [hash = (2 * XNT_TO_NT_CODE (x) + XNT_TO_PRDCT_CODE (x)) % HASH_SIZE]; z != 0; z
		     = xnt_lnk [(y = z) - Delta])
		    ;

		if (y == 0)
		    xnt_hash [hash] = x;
		else
		    xnt_lnk [y - Delta] = x;
	    }
	}
    }

    return set_an_xnt (nt, prdct);
}




static SXINT	get_single_prod (SXINT A, SXINT B)
{
    /* Cette fonction retourne le numero de la production simple A -> B (B peut etre un xnt) */
    SXINT	i, prod_no, lim;

    lim = bnf_ag.WS_NTMAX [A + 1].npg;

    for (i = bnf_ag.WS_NTMAX [A].npg; i < lim; i++) {
	if (bnf_ag.WS_NBPRO [prod_no = bnf_ag.WS_NBPRO [i].numpg].bprosimpl && bnf_ag.WS_INDPRO [bnf_ag.WS_NBPRO [prod_no
	     ].prolon].lispro == B)
	    return prod_no;
    }

    return 0 /* garde fou */ ;
}



static SXBA	pred (SXBA set, SXBA temp, SXINT lgth)
{
    /* retourne l'ensemble des etats predecesseurs des etats de set sur la longueur lgth;
       temp est un ensemble de manoeuvre */

    SXINT	s, x, prd;

    if (lgth == 0)
	return set;

    sxba_copy (temp, set);
    sxba_empty (set);
    s = 0;

    while ((s = sxba_scan (temp, s)) > 0) {
	XxY_Yforeach (Q0xQ0_hd, s, x) {
	    prd =  XxY_X (Q0xQ0_hd, x);
	    SXBA_1_bit (set, prd);
	}
    }

    return pred (set, temp, lgth - 1);
}


static VOID	create_new_trans (struct nt_etat *aetat,
				  SXINT	A, 
				  SXINT prdct, 
				  SXINT action)
{
    /* Met dans la nt_table aetat la transition ((A, prdct), action)
       et retourne vrai si prdct != -1 */

    SXINT	xA, k, nt, prev;
    struct nt_trans	*atrans;

    xA = get_next_xnt (A, prdct);

    for (k = aetat->lnk, prev = 0; k != 0; k = nt_trans [prev = k].lnk) {
	if ((nt = nt_trans [k].check) >= xA) {
	    if (nt == xA)
		return;

	    break;
	}
    }


/* la transition n'existe pas, on la cree */

    aetat->lgth++;
    nt_etats [0].lgth++;

    if (++xnt_trans > nt_trans_size)
	nt_trans = (struct nt_trans*) sxrealloc (nt_trans, (nt_trans_size *= 2) + 1, sizeof (struct nt_trans));

    (atrans = nt_trans + xnt_trans)->check = xA;
    atrans->action = action;
    atrans->lnk = k;

    if (prev == 0)
	aetat->lnk = xnt_trans;
    else
	nt_trans [prev].lnk = xnt_trans;
}




static SXINT	complete_transition (SXBA state_set, 
				     SXBA noyau, 
				     SXBA temp_set, 
				     SXBA *xnt_set1, 
				     SXBA *xnt_set2, 
				     SXINT A, 
				     SXINT prdct, 
				     SXINT C, 
				     struct P_prdct *xAs, 
				     struct P_prdct *xCs)
{
    /* On est en train de supprimer, pour un certain look_ahead t et dans un etat q la production
       simple numero p. Soit A->B cette production simple (attention sa partie gauche A a pu deja
       etre modifiee et donc ne pas etre celle de la grammaire initiale).
       Noyau est l'ensemble des etats predecesseurs de q sur B.
       Complete_transition calcule pour chaque etat de state_set une nouvelle transition
       non_terminale (A', action)
       Noyau est un sous ensemble de state_set tel qu'il existe une transition sur A correcte.
       Temp_set, xnt_set1 et xnt_set2 sont des ensembles de manoeuvre.
       xAs et xCs sont des tableaux de manoeuvre.
       Dans un certain etat s, on change la partie gauche C de la production C->alpha par A
       (state_set = Pred (s, alpha))
       En fait C sera change en A' et non pas en A et C peut etre different de B, consequence
       d'une transformation precedente.
       Le non terminal A' doit etre consistant pour tous les etats de state_set.

       Calcul de A'
         Si state_set == noyau alors A' = A
	 Sinon on regarde si sur state_set - noyau A convient cad il n'existe pas de transition sur
	 	A ou cette transition est identique a celle qui existe sur C.
	 Si A ne convient pas (il existe un etat de sub_set avec transition sur A dont l'action
	 est differente de celle de C) on calcule l'ensemble xnt_set1 de tous les non_terminaux
	 possibles.
	 Pour etre un non terminal A' possible il faut que dans tous les etats s de state_set
	 on ait:
	         goto (s, A') = erreur
	    ou   si s est un etat de noyau
	    	      alors goto (s, A') = goto (s, A) 
	    	      sinon goto (s, A') = goto (s, C)

       Le calcul de A' se complique par la presence de non-terminaux etendus xnt - couple
       (nt, predicat) -
       La suppression de la production simple A->B prdct entraine le changement de la partie gauche
       C de C->alpha en A'.
       Pour chaque etat de noyau il existe une transition sur A prdct' et une sur C prdct.
       Pour chaque etat de state_set - noyau il existe une transition sur C prdct.
       Soit A' le non terminal choisi, on fabrique (si elles n'existent pas deja) les transitions
       suivantes:

         Pour un etat du noyau
		pour chaque A &i
			on cree (A' prdct &i, action)

		pour chaque C &i != C prdct
			on cree (A' &i, action)

         Pour un etat n'appartenant pas au noyau
		pour chaque (C &i, action)
			on cree (A' &i, action)

       En fait A' est choisi comme etant un non-terminal qui permet toutes les creations
       precedentes sur tous les etats de state_set. */
	    

    SXINT	i, j, k, xa, xc;
    struct nt_etat	*aetat;
    struct nt_trans	*atrans;
    struct P_prdct	*pA, *pC = NULL /* Pour un compilateur croisé pour l'architecture itanium64 qui est gcc version 3.4.5
						   et qui dit "warning: 'pC' might be used uninitialized in this function" */;
    SXINT		Aprime, nt, xnt;
    BOOLEAN	hors_noyau, is_xtrans, trans_exists;

    if (sxba_first_difference (state_set, noyau) == -1 /* state_set == noyau */) {
	if (xnt_state_set == NULL)
	    return A;

	if (prdct == -1) {
	    sxba_and (sxba_copy (temp_set, noyau), xnt_state_set);

	    if (sxba_is_empty (temp_set))
		return A;
	}
    }


/* On regarde si A convient quand meme */

    trans_exists = TRUE;
    i = 0;

    while ((i = sxba_scan (state_set, i)) > 0) {
	hors_noyau = !SXBA_bit_is_set (noyau, i);
	xa = xc = 0;

	for (j = nt_etats [i].lnk; j != 0; j = atrans->lnk) {
	    atrans = nt_trans + j;

	    if ((nt = XNT_TO_NT_CODE (atrans->check)) == A || nt == C) {
		pA = (nt == A) ? xAs + ++xa : xCs + ++xc;
		pA->prdct = XNT_TO_PRDCT_CODE (atrans->check);
		pA->action = atrans->action;
	    }
	}

	if (xa == 0) {
	    /* A n'existe pas, on est hors noyau, A convient donc mais il faut creer la transition */
	    trans_exists = FALSE;
	}
	else if (hors_noyau) {
	    if (xa == xc) {
		for (j = 1; j <= xc; j++) {
		    pA = xAs + j;
		    pC = xCs + j;

		    if (pC->prdct != pA->prdct || pC->action != pA->action)
			/* A ne convient pas */
			break;
		}

		if (j <= xc)
		    /* A ne convient pas */
		    break;
	    }
	    else
		/* A ne convient pas */
		break;
	}
	else {
	    /* On est dans le noyau */
	    if (xc > 1 || prdct != -1)
		/* A ne convient pas (sans doute trop brutal) */
		break;
	}
    }

    if (i != 0) {
	/* A ne convient pas */
	/* calcul de xnt_set1 */
	sxba_fill (*xnt_set1), SXBA_0_bit (*xnt_set1, 0);
	SXBA_0_bit (*xnt_set1, A) /* A ne convient pas */ ;
	SXBA_0_bit (*xnt_set1, C) /* C ne convient pas */ ;
	i = 0;

	while ((i = sxba_scan (state_set, i)) > 0) {
	    sxba_empty (*xnt_set2);
	    hors_noyau = !SXBA_bit_is_set (noyau, i);
	    xc = 0;

	    for (j = nt_etats [i].lnk; j != 0; j = atrans->lnk) {
		atrans = nt_trans + j;

		if (((nt = XNT_TO_NT_CODE (atrans->check)) == C && (hors_noyau || XNT_TO_PRDCT_CODE (atrans->check) !=
		     prdct)) || (nt == A && !hors_noyau)) {
		    /* On met dans xCs a la fois des A et des C
		       hors_noyau => pas de A et tous les C
		       noyau	  => tous les A et tous les C suf la transition simple (C prdct) */

		    (pC = xCs + ++xc)->prdct = XNT_TO_PRDCT_CODE (atrans->check);
		    pC->action = j /* pour reconnaitre les A des C */ ;
		}
		else if (SXBA_bit_is_set (*xnt_set1, nt))
		    SXBA_1_bit (*xnt_set2, nt);
	    }

	    nt = 0;

	    while ((nt = sxba_scan (*xnt_set2, nt)) > 0) {
		xa = 0;

		for (j = nt_etats [i].lnk; j != 0; j = atrans->lnk) {
		    atrans = nt_trans + j;

		    if (XNT_TO_NT_CODE (atrans->check) == nt) {
			(pA = xAs + ++xa)->prdct = XNT_TO_PRDCT_CODE (atrans->check);
			pA->action = atrans->action;
		    }
		}

		j = 0;

		if (xa == xc) {
		    for (j = 1; j <= xa; j++) {
			pA = xAs + j;

			for (k = 1; k <= xc; k++) {
			    pC = xCs + k;

			    if (pA->action == nt_trans [pC->action].action)
				break;
			}

			if (k <= xc) {
			    if (hors_noyau) {
				if (pA->prdct != pC->prdct)
				    /* nt ne convient pas */
				    break;
			    }
			    else {
				xnt = nt_trans [pC->action].check;

				if (XNT_TO_NT_CODE (xnt) == C) {
				    if (pA->prdct != pC->prdct)
					/* nt ne convient pas */
					break;
				}
				else {
				    /* C'est une A transition */
				    if (pC->prdct == -1) {
					if (pA->prdct != prdct)
					    /* nt ne convient pas */
					    break;
				    }
				    else if (prdct == -1 && xc == 1) {
					if (pA->prdct != pC->prdct)
					    break;
				    }
				    else {
					/* cas complique */
					if (!prdct_equal (pA->prdct, prdct, pC->prdct))
					    /* pA->prdct != prdct && pC->prdct */
					    break;
				    }
				}
			    }
			}
			else
			    break;
		    }
		}

		if (j <= xa) {
		    /* nt ne convient pas */	    
		    SXBA_0_bit (*xnt_set1, nt);
		}
	    }
	}

	Aprime = 0;

	while ((Aprime = sxba_scan (*xnt_set1, Aprime)) > 0) {
	    if (XNT_TO_PRDCT_CODE (Aprime) == -1)
		break;
	}

	if (Aprime == -1) {
	    Aprime = NTMAX = get_next_xnt ((SXINT)0, (SXINT)-1);
	    sxfree (*xnt_set1);
	    sxfree (*xnt_set2);
	    *xnt_set1 = sxba_calloc (NTMAX + 1);
	    *xnt_set2 = sxba_calloc (NTMAX + 1);
	}
    }
    else {
	/* A convient */
	if (trans_exists)
	    return A;

	Aprime = A;
    }


/* On insere A'  */

    i = 0;

    while ((i = sxba_scan (state_set, i)) > 0) {
	hors_noyau = !SXBA_bit_is_set (noyau, i);
	xa = xc = 0;

	for (j = nt_etats [i].lnk; j != 0; j = atrans->lnk) {
	    atrans = nt_trans + j;

	    if ((nt = XNT_TO_NT_CODE (atrans->check)) == C && (hors_noyau || XNT_TO_PRDCT_CODE (atrans->check) != prdct))
		 {
		pC = xCs + ++xc;
		pC->prdct = XNT_TO_PRDCT_CODE (atrans->check);
		pC->action = atrans->action;
	    }
	    else if (nt == A && !hors_noyau) {
		pA = xAs + ++xa;
		pA->prdct = XNT_TO_PRDCT_CODE (atrans->check);
		pA->action = atrans->action;
	    }
	    else if (nt == Aprime)
		break;
	}

	if (j != 0)
	    /* Aprime pour l'etat i existe, on a deja verifie que ca marche */
	    continue;


/* On cree, pour l'etat i, les transitions sur Aprime */

	is_xtrans = FALSE;
	aetat = nt_etats + i;

	for (j = 1; j <= xc; j++) {
	    pC = xCs + j;
	    create_new_trans (aetat, Aprime, pC->prdct, pC->action);

	    if (pC->prdct != -1) {
		is_xtrans = TRUE;
		xprdct_items++;
	    }
	}

	if (!hors_noyau) {
	    if (xa == 1 && xAs [1].prdct == -1) {
		create_new_trans (aetat, Aprime, prdct, xAs [1].action);

		if (prdct != -1) {
		    is_xtrans = TRUE;
		    xprdct_items++;
		}
	    }
	    else if (xc == 0 && prdct == -1) {
		for (j = 1; j <= xa; j++) {
		    pA = xAs + j;
		    create_new_trans (aetat, Aprime, pA->prdct, pA->action);

		    if (pA->prdct != -1) {
			is_xtrans = TRUE;
			xprdct_items++;
		    }
		}
	    }
	    else {
		/* Cas complique */
		for (j = 1; j <= xa; j++) {
		    pA = xAs + j;
		    create_new_trans (aetat, Aprime, get_xprdct (prdct, pA->prdct), pA->action);

		    if (pA->prdct != -1) {
			is_xtrans = TRUE;
			xprdct_items++;
		    }
		}
	    }
	}

	if (is_xtrans) {
	    xprdct_items++ /* &Else */ ;
	    xpredicates++ /* suppose nouvelle transition non terminale */ ;
	}
    }

    return Aprime;
}



static SXINT	nt_repr (SXINT ntpd, SXINT *pd_to_pg)
{
    /* Cette fonction retourne le representant de la classe d'equivalence du non terminal
       ntpd pour les productions simples */
    SXINT	ntpg;

    if ((ntpg = pd_to_pg [ntpd]) == 0)
	return ntpd;

    return pd_to_pg [ntpd] = nt_repr (ntpg, pd_to_pg);
}




VOID	total_spe (SXBA total_spe_single_prod)
{
    /*  ELIMINATION DE TOUTES LES PRODUCTIONS SIMPLES */
    
    SXINT		last_sp;
    SXINT		*sp_to_t_state;
    struct sorted_single_prod {
	SXINT	lnk, t_occur;
    }	*sorted_single_prod;
    SXBA	sp_states_set /* ensemble des etats ayant une production simple */ ;

    {
	/* On commence par recopier toutes les transitions non terminales dans une structure
	   extensible, l'elimination des productions simples pouvant en recreer de nouvelles */

	SXINT	i;
	struct P_item	*aitem;
	struct P_item	*lim;
	struct nt_trans	*atrans;
	struct state	*astate;
	struct nt_etat		*aetat;

	sxinitialise(aetat); /* pour faire taire gcc -Wuninitialized */
	nt_etats = (struct nt_etat*) sxcalloc (xac2 + 1, sizeof (struct nt_etat));
	nt_trans = (struct nt_trans*) sxcalloc ((nt_trans_size = 2 * xnt_items) + 1, sizeof (struct nt_trans));
	xnt_trans = 0;

	for (i = 1; i < xac2; i++) {
	    if ((astate = nt_states + i)->lgth > 0) {
		for (lim = (aitem = nt_items + astate->start) + astate->lgth, atrans = NULL; aitem < lim; aitem++) {
		    if (aitem->action != 0) {
			xnt_trans++;

			if (atrans == NULL)
			    (aetat = nt_etats + i)->lnk = xnt_trans;
			else
			    atrans->lnk = xnt_trans;

			atrans = nt_trans + xnt_trans;
			atrans->check = aitem->check;
			atrans->action = aitem->action;
			(aetat->lgth)++;
			(nt_etats [0].lgth)++;
		    }
		}
	    }
	}
    }

    sorted_single_prod = (struct sorted_single_prod*) sxcalloc (bnf_ag.WS_TBL_SIZE.xnbpro + 1, sizeof (struct
	 sorted_single_prod));
    sp_states_set = sxba_calloc (xe1 + 1);

    {
	/* On ordonne les productions simples dans la liste "sorted_single_prod".
	   Si i precede j dans sorted_single_prod et si i: A -> B et j: A' -> B'
	   alors 
	      soit B ->* A' (par des chaines de productions simples)
	      soit i et j sont dans des arbres (multi racine) de productions simples differents */

	SXBA	*head_tail, *head_xtail, heads, tails, origs;
	SXBA		line;
	SXINT	i, j, k;

	head_tail = sxbm_calloc (bnf_ag.WS_TBL_SIZE.ntmax + 1, bnf_ag.WS_TBL_SIZE.ntmax + 1);
	head_xtail = sxbm_calloc (bnf_ag.WS_TBL_SIZE.xntmax + 1, bnf_ag.WS_TBL_SIZE.xntmax + 1);
	heads = sxba_calloc (bnf_ag.WS_TBL_SIZE.ntmax + 1);
	tails = sxba_calloc (bnf_ag.WS_TBL_SIZE.ntmax + 1);
	origs = sxba_calloc (bnf_ag.WS_TBL_SIZE.ntmax + 1);

/* On remplit head_tail et head_xtail
   	   head_tail [A, B] <= A -> B&i est une production simple
   	   head_xtail [A, B&i] <=> A -> B&i est une production simple */
	/* On calcule max_lhs */

	max_lhs = 0;

	for (i = 1; i <= bnf_ag.WS_TBL_SIZE.xnbpro; i++) {
	    j = bnf_ag.WS_NBPRO [i + 1].prolon - (k = bnf_ag.WS_NBPRO [i].prolon) - 1;
	    max_lhs = max (max_lhs, j);

	    if (bnf_ag.WS_NBPRO [i].bprosimpl) {
		SXBA_1_bit (total_spe_single_prod, i);
		j = bnf_ag.WS_NBPRO [i].reduc;
		k = bnf_ag.WS_INDPRO [k].lispro;
		SXBA_1_bit (head_xtail [j], k);
		SXBA_1_bit (heads, j);
		SXBA_1_bit (tails, k);
		k = XNT_TO_NT_CODE (k);
		SXBA_1_bit (head_tail [j], k);
	    }
	}

	last_sp = 0;

	while (!sxba_is_empty (heads)) {
	    /* Il reste des productions simples non triees */
	    sxba_minus (sxba_copy (origs, heads), tails);
	    /* origs est non vide car A =>+ A est impossible */
	    sxba_empty (tails);
	    i = 0;

	    while ((i = sxba_scan (heads, i)) > 0) {
		if (SXBA_bit_is_set (origs, i) /* origine sans predecesseur */ ) {
		    line = head_xtail [i];
		    SXBA_0_bit (heads, i);
		    j = 0;

		    while ((j = sxba_scan (line, j)) > 0) {
			k = sorted_single_prod [last_sp].lnk = get_single_prod (i, j);
			last_sp = k;
		    }
		}
		else
		    sxba_or (tails, head_tail [i]);
	    }
	}

	sxfree (origs);
	sxfree (tails);
	sxfree (heads);
	sxbm_free (head_xtail);
	sxbm_free (head_tail);
    }

    {
	/* On commence par calculer le nombre d'occurrences de chaque production simple dans les
	   t_tables et on remplit sp_states_set */

	struct state	*astatei;
	struct P_item	*aitem, *lim;
	struct floyd_evans	*afe;
	SXINT	i;
	SXINT	sp_t_occur_no;
	SXBA	already_found;

	already_found = sxba_calloc (bnf_ag.WS_TBL_SIZE.xnbpro + 1);
	sp_t_occur_no = 0;

	for (i = 1; i < xe1; i++) {
	    if ((astatei = t_states + i)->lgth > 0) {
		sxba_empty (already_found);

		for (lim = (aitem = t_items + astatei->start) + astatei->lgth; aitem < lim; aitem++) {
		    afe = t_fe + aitem->action;

		    if (afe->codop == Reduce && bnf_ag.WS_NBPRO [afe->reduce].bprosimpl && SXBA_bit_is_reset_set (already_found, afe->
			 reduce)) {
			SXBA_1_bit (sp_states_set, i);
			sp_t_occur_no++;
			sorted_single_prod [afe->reduce].t_occur++;
		    }
		}
	    }
	}

	sp_to_t_state = (SXINT*) sxcalloc (sp_t_occur_no + 1, sizeof (SXINT));

/* On remplit t_occur avec les valeurs cumulees des nombres d'occurrences */

	sp_t_occur_no = 1;

	for (i = sorted_single_prod [0].lnk; i != 0; i = sorted_single_prod [i].lnk) {
	    sorted_single_prod [i].t_occur = (sp_t_occur_no += sorted_single_prod [i].t_occur);
	}

	sorted_single_prod [0].t_occur = sp_t_occur_no;


/* On remplit sp_to_t_state */

	for (i = 1; i < xe1; i++) {
	    if ((astatei = t_states + i)->lgth > 0) {
		sxba_empty (already_found);

		for (lim = (aitem = t_items + astatei->start) + astatei->lgth; aitem < lim; aitem++) {
		    afe = t_fe + aitem->action;

		    if (afe->codop == Reduce && bnf_ag.WS_NBPRO [afe->reduce].bprosimpl && SXBA_bit_is_reset_set (already_found, afe->
			 reduce)) {
			sp_to_t_state [--(sorted_single_prod [afe->reduce].t_occur)] = i;
		    }
		}
	    }
	}

	sxfree (already_found);
    }


    {
	/* Suppression des productions simples, renommage des parties gauches des productions
	   impliquees et creation de nouvelles transitions non terminales */
	SXINT	i, j, s, l;
	SXINT	lhs, rhs, xrhs, lim1, lim2, t, n, red, a, lhsprime, old_action, new_action;
	SXINT	*la_to_lhs;
	struct P_prdct	*xAs, *xCs;
	struct state	*astate;
	struct P_item	*aitem, *lim;
	struct floyd_evans	*afe;
	SXBA	pred_set, pred1_set, pred2_set, and_set_local, temp_set, la_red_set, xnt_set1, xnt_set2;

	pred_set = sxba_calloc (xe1 + 1);
	pred1_set = sxba_calloc (xe1 + 1);
	pred2_set = sxba_calloc (xe1 + 1);
	and_set_local = sxba_calloc (xe1 + 1);
	temp_set = sxba_calloc (xe1 + 1);
	la_red_set = sxba_calloc (bnf_ag.WS_TBL_SIZE.xtmax + 1 /* pour any */ + 1 );
	xnt_set1 = sxba_calloc (NTMAX /* == bnf_ag.WS_TBL_SIZE.ntmax pour l'instant */ + 1 );
	xnt_set2 = sxba_calloc (NTMAX /* == bnf_ag.WS_TBL_SIZE.ntmax pour l'instant */ + 1);
	la_to_lhs = (SXINT*) sxcalloc (bnf_ag.WS_TBL_SIZE.xtmax + 1, sizeof (SXINT));
	xAs = (struct P_prdct*) sxalloc ((i = bnf_ag.WS_TBL_SIZE.xntmax - bnf_ag.WS_TBL_SIZE.ntmax + 1) + 1, sizeof (struct
	     P_prdct));
	xCs = (struct P_prdct*) sxalloc (i + 1, sizeof (struct P_prdct));
	xnt_lnk = NULL /* Executions multiples dans le meme process */;

	for (i = sorted_single_prod [0].lnk; i != 0; i = sorted_single_prod [i].lnk) {
	    /* On supprime la production simple no i: lhs -> xrhs */
	    /* lhs sera calcule plus tard comme etant le champ next du floyd-evans, ce champ
	       ayant deja pu etre modifie par une suppression precedente */
	    xrhs = bnf_ag.WS_INDPRO [bnf_ag.WS_NBPRO [i].prolon].lispro;
	    lim1 = sorted_single_prod [sorted_single_prod [i].lnk].t_occur;

	    for (j = sorted_single_prod [i].t_occur; j < lim1; j++) {
		sxba_empty (pred_set);
		t = sp_to_t_state [j];
		SXBA_1_bit (pred_set, t);
		/* t est un etat ou la reduction no i apparait */
		pred_set = pred (pred_set, temp_set, (SXINT)1);

/* On remplit la_to_lhs */

		astate = t_states + t;

		for (lim = (aitem = t_items + astate->start) + astate->lgth; aitem < lim; aitem++) {
		    afe = t_fe + aitem->action;

		    if (afe->codop == Reduce && afe->reduce == i) {
			la_to_lhs [aitem->check] = afe->next;
		    }
		}

		lim2 = bnf_ag.WS_NTMAX [(rhs = XNT_TO_NT_CODE (xrhs)) + 1].npg;

		for (l = bnf_ag.WS_NTMAX [rhs].npg; l < lim2; l++) {
		    red = bnf_ag.WS_NBPRO [l].numpg;
		    sxba_copy (pred1_set, pred_set);
		    s = 0;

		    while ((s = sxba_scan (pred1_set, s)) > 0) {
			/* goto (s, xrhs) = t */ 
			n = gotostar (s, bnf_ag.WS_NBPRO [red].prolon, bnf_ag.WS_NBPRO [red + 1].prolon - 1);

/* red : rhs -> alpha ; n = goto* (s, alpha) */
/* Pour chaque looh-ahead a tel que dans l'etat t on fait la reduction
   simple i on cherche (s'il existe) dans l'etat n le meme look-ahead a
   tel qu'on fait la reduction red; soit rhs' sa partie gauche
   (on a deja pu modifier rhs).
   On remplace dans ce cas sa partie gauche (rhs') par lhs'.
   lhs' est calcule par "complete_transition" a partir de lhs, rhs',
   pred1_set et l'ensemble pred2_set de tous les etats predecesseurs de n
   par alpha.
   lhs' peut etre un nouveau non terminal cree par "xnt_mngr"
   il y a sur chaque etat de pred2_set une transition sur lhs' */
			

			sxba_empty (pred2_set);
			SXBA_1_bit (pred2_set, n);
			pred2_set = pred (pred2_set, temp_set, bnf_ag.WS_NBPRO [red + 1].prolon - bnf_ag.WS_NBPRO [red].
			     prolon - 1);

/* Tous les etats de and_set_local sont traites simultanement (s en fait partie) */

			sxba_and (sxba_copy (and_set_local, pred1_set), pred2_set);
			sxba_minus (pred1_set, and_set_local);

/* On remplit la_red_set */

			astate = t_states + n;

			for (lim = (aitem = t_items + astate->start) + astate->lgth; aitem < lim; aitem++) {
			    if (aitem->action != 0 && la_to_lhs [aitem->check] != 0 && (afe = t_fe + aitem->action)->
				 codop == Reduce && afe->reduce == red) {
				/* c'est la reduction red avec un "bon" look-ahead */
				SXBA_1_bit (la_red_set, aitem->check);
			    }
			}


/* On exploite la_red_set */

			a = 0;

			while ((a = sxba_scan_reset (la_red_set, a)) > 0) {
			    lhs = la_to_lhs [a];

			    /* recherche de l'action associee au look_ahead a dans l'etat n */

			    for (aitem = t_items + astate->start; aitem->check != a; aitem++)
				;

			    afe = t_fe + (old_action = aitem->action);
			    lhsprime = complete_transition (pred2_set, and_set_local, temp_set, &xnt_set1, &xnt_set2, lhs,
				 XNT_TO_PRDCT_CODE (xrhs), afe->next, xAs, xCs);
			    *t_fe = *afe;
			    t_fe->next = lhsprime;
			    new_action = aitem->action = put_in_fe (t_fe, t_hash, t_lnks, t_fe);

			    while (++aitem < lim) {
				if (aitem->action != 0 && SXBA_bit_is_set (la_red_set, aitem->check) && la_to_lhs [aitem->check]
				     == lhs && aitem->action == old_action) {
				    aitem->action = new_action;
				    SXBA_0_bit (la_red_set, aitem->check);
				}
			    }
			}
		    }
		}


/* On supprime la production simple i dans l'etat t et raz de la_to_lhs */

		astate = t_states + t;

		for (lim = (aitem = t_items + astate->start) + astate->lgth; aitem < lim; aitem++) {
		    afe = t_fe + aitem->action;

		    if (afe->codop == Reduce && afe->reduce == i) {
			la_to_lhs [aitem->check] = 0;
			aitem->check = aitem->action = 0;
			xt_items--;
		    }
		}
	    }
	}

	if (xnt_lnk != NULL)
	    sxfree (xnt_lnk);

	sxfree (xCs);
	sxfree (xAs);
	sxfree (la_to_lhs);
	sxfree (xnt_set2);
	sxfree (xnt_set1);
	sxfree (la_red_set);
	sxfree (temp_set);
	sxfree (and_set_local);
	sxfree (pred2_set);
	sxfree (pred1_set);
	sxfree (pred_set);


/* Pour accelerer il faut eviter de refaire "complete_transition" quand son travail
	   a deja ete fait par un appel precedent.
	   Or une execution de "complete_transition" pour une production simple i dans un etat t
	   ne depend que de lhs (partie gauche de i pour un certain look_ahead a), C (partie
	   gauche de la reduction red: C->alpha dans l'etat n pour le look_ahead a) et pred2_set
	   ensemble des etats predecesseurs de n sur alpha.
	   Il suffit donc, dans un premier passage de noter les (lhs, C, pred2_set) differents
	   et les aitems auquels ils correspondent au lieu d'appeler "complete_transition".
	   Dans une deuxieme passe on parcourt les aitems notes precedemment; si un aitem
	   correspond a un triplet non execute, on appelle "complete_transition" et on note
	   le resultat sinon on utilise le resultat calcule par un appel precedent.
	   Pour ca il faut peut etre:
	   	- allouer des listes de SXBA (sinon tableaux de SXBA pouvant deborder...)
		- calculer du hash-code sur des SXBA (reutilisation des pred2_set identiques)
		- gerer une liste libre de SXBA (reutilisation des elements de la liste pour
		  des i et t differents)
		- calcul du hash-code sur les triplets
		- ....

	   Le courage me manque un peu....plus tard peut etre... */

    }


    {
	/* Suppression des transitions non terminales jamais utilisees */
	/* Si pour un etat s il existe une transition non terminale sur N telle que
	   pour tout etat s' dans lequel il existe une reduction N' -> alpha telle que
	   s appartient a pred (s', alpha) on a N' different de XNT_TO_NT_CODE (N),
	   il est possible de supprimer la transition sur N depuis s */

	SXBA	*nt_sets, lhs_set, pred_set, temp_set, nt_set;

	nt_sets = sxbm_calloc (xe1 + 1, bnf_ag.WS_TBL_SIZE.xntmax + 1);
	lhs_set = sxba_calloc (max_lhs + 1 + 1);
	pred_set = sxba_calloc (xe1 + 1);
	temp_set = sxba_calloc (xe1 + 1);
	nt_set = sxba_calloc (bnf_ag.WS_TBL_SIZE.xntmax + 1);

	{
	    /* Dans les T_tables */
	    SXINT	i, j, k;
	    struct floyd_evans		*afe;
	    struct P_item	*aitem, *lim;
	    struct state	*astate;

	    for (i = 1; i < xe1; i++) {
		if ((astate = t_states + i)->lgth > 0) {
		    for (lim = (aitem = t_items + astate->start) + astate->lgth; aitem < lim; aitem++) {
			if (aitem->action != 0) {
			    if (((afe = t_fe + aitem->action)->codop == Reduce && afe->reduce != 0) /* pas Halt */  || afe
				 ->codop == ScanReduce) {
				SXBA_1_bit (lhs_set, afe->pspl + 1);
			    }
			}
		    }

		    j = 0;

		    while ((j = sxba_scan_reset (lhs_set, j)) > 0) {
			SXBA_1_bit (pred_set, i);
			pred_set = pred (pred_set, temp_set, j - 1);
			sxba_empty (nt_set);

			for (aitem = t_items + astate->start; aitem < lim; aitem++) {
			    if (aitem->action != 0) {
				if ((((afe = t_fe + aitem->action)->codop == Reduce && afe->reduce != 0) /* pas Halt */  ||
				     afe->codop == ScanReduce) && afe->pspl == j - 1) {
				    SXBA_1_bit (nt_set, afe->next);
				}
			    }
			}

			k = 0;

			while ((k = sxba_scan_reset (pred_set, k)) > 0) {
			    sxba_or (nt_sets [k], nt_set);
			}
		    }
		}
	    }
	}

	{
	    /* dans les nt_tables */

	    SXINT	i, j, k;
	    struct floyd_evans		*afe;
	    struct nt_trans	*atrans;
	    struct nt_etat	*aetat;

	    for (i = 1; i < xac2; i++) {
		if ((aetat = nt_etats + i)->lgth > 0) {
		    for (atrans = nt_trans + aetat->lnk; atrans != nt_trans; atrans = nt_trans + atrans->lnk) {
			if ((afe = nt_fe + atrans->action)->codop == Reduce) {
			    SXBA_1_bit (lhs_set, afe->pspl + 1);
			}
		    }

		    j = 0;

		    while ((j = sxba_scan_reset (lhs_set, j)) > 0) {
			SXBA_1_bit (pred_set, i);
			pred_set = pred (pred_set, temp_set, j - 1);
			sxba_empty (nt_set);

			for (atrans = nt_trans + aetat->lnk; atrans != nt_trans; atrans = nt_trans + atrans->lnk) {
			    if ((afe = nt_fe + atrans->action)->codop == Reduce && afe->pspl == j - 1) {
				SXBA_1_bit (nt_set, afe->next);
			    }
			}

			k = 0;

			while ((k = sxba_scan_reset (pred_set, k)) > 0) {
			    sxba_or (nt_sets [k], nt_set);
			}
		    }
		}
	    }
	}

	sxfree (nt_set);
	sxfree (temp_set);
	sxfree (pred_set);
	sxfree (lhs_set);

	{
	    /* Suppression */
	    SXINT	i, nt;
	    struct nt_trans	*patrans, *natrans;
	    struct nt_etat	*aetat;

	    for (i = 1; i < xac2; i++) {
		if ((aetat = nt_etats + i)->lgth > 0) {
		    nt_set = nt_sets [i];

		    for (natrans = nt_trans + aetat->lnk, patrans = NULL; natrans != nt_trans; natrans = nt_trans +
			 natrans->lnk) {
			nt = XNT_TO_NT_CODE (natrans->check);
			
			if (!SXBA_bit_is_set (nt_set, nt)) {
			    aetat->lgth--;
			    nt_etats [0].lgth--;

			    if (patrans == NULL)
				aetat->lnk = natrans->lnk;
			    else
				patrans->lnk = natrans->lnk;
			}
			else {
			    patrans = natrans;
			}
		    }
		}
	    }
	}

	sxbm_free (nt_sets);
    }

    {
	/* On supprime les t_states vides et les transitions inutiles */

	SXBA	empty_t_states;

	empty_t_states = sxba_calloc (xe1 + 1);

	{
	    /* On memorise les etats vides, seuls les etats ayant des reductions simples ont
	       pu le devenir */

	    SXINT	i;
	    struct P_item	*aitem, *lim;
	    struct state	*astate;

	    i = 0;

	    while ((i = sxba_scan (sp_states_set, i)) > 0) {
		astate = t_states + i;

		for (lim = (aitem = t_items + astate->start) + astate->lgth - 1; aitem < lim; aitem++) {
		    if (aitem->action != 0)
			break;
		}

		if (aitem == lim && aitem->action == t_error) {
		    SXBA_1_bit (empty_t_states, i);
		    astate->lgth = 0;
		}
	    }
	}

	{
	    /* On supprime des nt_tables les transitions vers des t_states vides */

	    SXINT	i;
	    struct nt_trans	*patrans, *natrans;
	    struct nt_etat	*aetat;
	    struct floyd_evans		*afe;

	    for (i = 1; i < xac2; i++) {
		if ((aetat = nt_etats + i)->lgth > 0) {
		    for (natrans = nt_trans + aetat->lnk, patrans = NULL; natrans != nt_trans; natrans = nt_trans +
			 natrans->lnk) {
			afe = nt_fe + natrans->action;

			if (afe->codop == Shift && SXBA_bit_is_set (empty_t_states, afe->next)) {
			    aetat->lgth--;
			    nt_etats [0].lgth--;

			    if (patrans == NULL)
				aetat->lnk = natrans->lnk;
			    else
				patrans->lnk = natrans->lnk;
			}
			else {
			    patrans = natrans;
			}
		    }
		}
	    }
	}

	sxfree (empty_t_states);
    }

    {
	/* On recree t_states et t_items */

	SXINT	i, lgth;
	struct P_item	*aitem, *lim;
	struct state	*astate;
	struct P_item	*old_t_items = t_items;

	t_items = (struct P_item*) sxcalloc (xt_items + 1, sizeof (struct P_item));
	xt_items = 1;

	for (i = 1; i < xe1; i++) {
	    if ((lgth = (astate = t_states + i)->lgth) > 0) {
		astate->lgth = 0;
		aitem = old_t_items + astate->start;
		astate->start = xt_items;

		for (lim = aitem + lgth; aitem < lim; aitem++) {
		    if (aitem->action != 0) {
			t_items [xt_items++] = *aitem;
			astate->lgth++;
		    }
		}
	    }
	}

	sxfree (old_t_items);
    }

    {
	/* On recree nt_states et nt_items a partir de la structure extensible nt_trans */

	SXINT	i, j;
	struct P_item	*aitem;
	struct nt_trans	*atrans;
	struct state	*astate;
	struct nt_etat		*aetat;

	sxfree (nt_items);
	nt_items = (struct P_item*) sxcalloc (nt_etats [0].lgth + 1, sizeof (struct P_item));
	xnt_items = 1;

	for (i = 1; i < xac2; i++) {
	    if ((aetat = nt_etats + i)->lgth > 0) {
		astate = nt_states + i;
		astate->lgth = aetat->lgth;
		astate->start = xnt_items;

		for (j = aetat->lnk; j != 0; j = atrans->lnk) {
		    atrans = nt_trans + j;
		    aitem = nt_items + xnt_items++;
		    aitem->check = atrans->check;
		    aitem->action = atrans->action;
		}
	    }
	}
    }

    sxfree (sp_to_t_state);
    sxfree (sp_states_set);
    sxfree (sorted_single_prod);
    sxfree (nt_trans);
    sxfree (nt_etats);
}




VOID	partial_spe (SXBA partial_spe_single_prod, SXBA restriction_set)
{
  /*  ELIMINATION PARTIELLE DES PRODUCTIONS SIMPLES */

  SXINT		*pd_set, *pd_to_pg, *pg_to_act, pg;
  struct P_item	**pd_to_aitem;
  struct state	*astatei;
  struct P_item	*aitem, *bi, *bs;
  struct floyd_evans		*afe;
  SXINT	i, pd, x, xpd;
  SXBA	nt_set;
  BOOLEAN	is_a_single_prod, is_xstate;

  nt_set = sxba_calloc (bnf_ag.WS_TBL_SIZE.ntmax + 1);
  pd_set = (SXINT*) sxalloc (bnf_ag.WS_TBL_SIZE.ntmax + 1, sizeof (SXINT));
  pd_to_aitem = (struct P_item**) sxalloc (bnf_ag.WS_TBL_SIZE.ntmax + 1, sizeof (struct P_item*));
  pd_to_pg = (SXINT*) sxcalloc (bnf_ag.WS_TBL_SIZE.ntmax + 1, sizeof (SXINT));
  pg_to_act = (SXINT*) sxcalloc (bnf_ag.WS_TBL_SIZE.ntmax + 1, sizeof (SXINT));
  xpd = 0;

  /* Le 25/02/2003
     Le traitement des XNT n'est pas encore fait.  Donc si la pg de la prod simple a des
     transitions sur nt et NT&i, les actions sont distinctes, on ne sait donc pas
     les traiter.  Pour l'instant, si on a une XNT sur la pg, on ne fait rien */

  for (i = 1; i < xac2; i++) {
    if ((astatei = nt_states + i)->lgth > 0) {
      is_a_single_prod = FALSE;
      bs = (bi = nt_items + astatei->start) + astatei->lgth;
      is_xstate = FALSE;

      for (aitem = bs - 1; aitem >= bi; aitem--) {
	if (aitem->check > bnf_ag.WS_TBL_SIZE.ntmax) {
	  SXINT nt;

	  bs = aitem;
	  is_xstate = TRUE;
	  nt = XNT_TO_NT_CODE (aitem->check);
	  SXBA_1_bit (nt_set, nt);
	}
	else if (!SXBA_bit_is_set (nt_set, aitem->check) &&
		 (afe = nt_fe + aitem->action)->codop == Reduce &&
		 bnf_ag.WS_NBPRO [afe->reduce].bprosimpl &&
		 (restriction_set == NULL ||
		  !SXBA_bit_is_set (restriction_set, afe->reduce))) {
	  pg = bnf_ag.WS_NBPRO [afe->reduce].reduc;

	  if (!SXBA_bit_is_set (nt_set, pg)) {
	    /* Pas de XNT sur pg */
	    is_a_single_prod = TRUE;
	    SXBA_1_bit (partial_spe_single_prod, afe->reduce);
	    pd_set [++xpd] = aitem->check;
	    pd_to_aitem [aitem->check] = aitem;
	    pg_to_act [pd_to_pg [aitem->check] = pg] = -1
	      /* used, filled up latter */
	      ;
	  }
	}
      }

      if (is_xstate)
	sxba_empty (nt_set);

      if (is_a_single_prod) {
	for (aitem = bi; aitem < bs; aitem++) {
	  if (pg_to_act [aitem->check] == -1) {
	    pg_to_act [aitem->check] = aitem->action;
	  }
	}

	for (x = xpd; x > 0; x--) {
	  pd = pd_set [x];
	  pd_to_aitem [pd]->action = pg_to_act [nt_repr (pd, pd_to_pg)];
	}

	for (; xpd > 0; xpd--) {
	  pd_to_pg [pd_set [xpd]] = 0;
	}
      }
    }
  }

  sxfree (pg_to_act);
  sxfree (pd_to_pg);
  sxfree (pd_to_aitem);
  sxfree (pd_set);
  sxfree (nt_set);
}
