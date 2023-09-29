/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1985 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *     Produit de l'equipe Langages et Traducteurs.     *
   *                                                      *
   ******************************************************** */




/*************************************************************/
/*                                                           */
/*                                                           */
/*  This program has been translated from parser.pl1         */
/*  on Thursday the second of January, 1986, at 11:32:44,    */
/*  on the Multics system at INRIA,                          */
/*  by the release 3.3.e PL1_C translator of INRIA,          */
/*         developped by the "Langages et Traducteurs" team, */
/*         using the SYNTAX (*), FNC and Paradis systems.    */
/*                                                           */
/*                                                           */
/*************************************************************/
/* (*) SYNTAX is a trademark of INRIA.                       */
/*************************************************************/


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 21-10-93 13:00 (phd&pb):	Adaptation a OSF			*/
/************************************************************************/
/* 25-03-93 14:19 (pb):		Suppression du parametre "stack" dans	*/
/*				"is_a_right_ctxt" et "check_nt_ctxt"	*/
/*				(bug sur oflow).			*/
/************************************************************************/
/* 27-01-93 14:50 (pb):		Adaptation au nouveau token_mngr	*/
/************************************************************************/
/* 24-04-92 10:35 (pb):		Bug sur la valeur de atok_no retournee	*/
/*				lorsque un predicat est associe au	*/
/*				token precedent.			*/
/************************************************************************/
/* 08-04-92 16:08 (pb):		Utilisation du "mode" d'analyse		*/
/************************************************************************/
/* 24-11-88 09:40 (pb):		Suppression du dernier parametre de	*/
/*				P_access				*/
/*				is_a_right_ctxt rend FALSE si predicat	*/
/************************************************************************/
/* 03-06-88 10:19 (pb):		Bug pour STACKtop et STACKnew_top dans	*/
/*				le cas "End Of File"			*/
/************************************************************************/
/* 05-05-88 11:15 (pb):		Bug recuperation globale introduit sans */
/*				doute avec la 3.0			*/
/************************************************************************/
/* 27-04-88 11:45 (pb):		Adaptation au "token manager"		*/
/************************************************************************/
/* 29-03-88 18:00 (pb):		Adaptation aux sequences de predicats	*/
/************************************************************************/
/* 24-03-88 09:42 (pb):		Les prdcts rendent tous faux (prudence)	*/
/************************************************************************/
/* 23-03-88 09:08 (pb):		Bug dans set_[first/next]_trans		*/
/************************************************************************/
/* 04-03-88 10:05 (pb):		Modification pour predicats sur nt	*/
/************************************************************************/
/* 10-02-88 11:50 (pb):		Bug dans is_a_right_ctxt sur les prdcts	*/
/*				On n'execute les predicats que sur les	*/
/*				positions 0-1 comme en analyse normale	*/
/*				L'insertion d'un symbole unique est	*/
/*				conditionnee par la validation du	*/
/*				predicat associe			*/
/************************************************************************/
/* 21-01-88 14:50 (phd):	Suppression des macros "min" et "max",	*/
/*				remplacement "truncatate" -> "truncate"	*/
/************************************************************************/
/* 22-12-87 17:42 (phd):	Adaptation aux bits arrays		*/
/************************************************************************/
/* 04-12-87 10:35 (pb):		Adaptation a la V3			*/
/************************************************************************/
/* 10-11-87 10:50 (pb):		Possibilite de correction reduite si	*/
/*				non-partitionnement des predicats.	*/
/************************************************************************/
/* 04-11-87 15:10 (pb&phd):	Prise en compte de la possibilite de	*/
/*				non-partitionnement des predicats.	*/
/************************************************************************/
/* 05-10-87 09:48 (pb):		Message "Global Recovery" sorti avant	*/
/*				"parsing stops on EOF"			*/
/************************************************************************/
/* 29-07-87 15:06 (phd):	Suppression d'une affectation inutile	*/
/************************************************************************/
/* 02-07-87 10:00 (pb) :	Adaptation au nouveau recor		*/
/************************************************************************/
/* 15-06-87 09:02 (pb) :	Adaptation aux actions et predicats	*/
/*				+ ameliorations				*/
/************************************************************************/
/* 25-05-87 11:03 (phd) :	Ajout d'un message d'erreur		*/
/************************************************************************/
/* 19-05-87 17:49 (pb&phd) :	Utilisation des tables du scanner pour 	*/
/*				sortir le texte de "End Of File"	*/
/************************************************************************/
/* 03-05-87 08:55 (pb) :	Adaptation aux nouvelles tables        	*/
/************************************************************************/
/* 26-03-87 11:15 (pb) :	"END" change en "CLOSE"	          	*/
/************************************************************************/
/* 12-03-87 11:52 (pb) :	sxprecovery transforme en fonction   	*/
/************************************************************************/
/* 04-03-87 10:22 (pb) :	Bug dans l'insertion forcee	   	*/
/************************************************************************/
/* 27-02-87 10:40 (pb) :	Separation parser et error_recovery	*/
/************************************************************************/
/* 16-05-86 17:11 (phd):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/


#ifndef lint
char	what_sxp_srcvr [] = "@(#)sxpsrecovery.c\t- SYNTAX [unix] - Jeu 21 Oct 1993";

#endif

# include "sxunix.h"

#define Commun 1
#define Propre 2
#define Defaut 3
#define Done   4

extern SXSHORT	P_access ();

/*   V A R I A B L E S   */

static int	lgt1, tmax = 0;
static SXSHORT	*stack = NULL, *st_stack /* lgt1 */ ;
static int	*ster, *lter /* sizofpts */ ;
static char	**com /* sizofpts */ ;
static SXBA	modified_set /* lgt1 */ , vt_set /* tmax */ ;
static BOOLEAN	truncate_context, has_prdct;
static struct SXP_item	*mvector, *Mvector;

static int	TOK_0, TOK_i;


/*

          !scan                                              scan
------------^-------------\/-----------------------------------^---------------------------------
                          0         Mrednt          Mred    Mprdct              Mfe M0ref  Mref
--------------------------|------------|--------------|-------|------------------|----|------|--->
                           \___________/
                          trans sur nt
                           \__________________________/\______/\_________________________________
                                    reduction           prdct              shift


                                               ^
                                               | acces vector [ref]
                                         Mref  -
					       | Shift: empiler ref+Mref-M0ref, goto ref-M0ref
                                               |
                                        M0ref  -
					       | Shift: empiler 0, goto ref-Mfe
                                               |
                                          Mfe  -
                                               | Shift: acces Floyd_Evans [ref]
                                               |
                                       Mprdct  -
                                               | Prdct: acces Predicates [-ref]
                                               |
                                         Mred  -
                                               | Reduce: acces Reductions [ref]
                                               |
                                       Mrednt  -
                                               | Reduce: acces Reductions [ref]
                                               | (sur transition non terminale)
                                               |
                                            0  - Error
                                              ref


*/


static char	*ttext (tables, tcode)
    struct sxtables	*tables;
    int		tcode;
{
    return (tcode == sxplocals.SXP_tables.P_tmax) ? sxplocals.SXP_tables.P_global_mess [sxplocals.SXP_tables.
	 P_followers_number + 2] : sxttext (tables, tcode);
}



static VOID	oflow ()

/* overflow of stack */

{
    lgt1 *= 2;
    modified_set = sxba_resize (modified_set, lgt1);
    st_stack = (SXSHORT*) sxrealloc (st_stack, lgt1 + 1, sizeof (SXSHORT));
    stack = (SXSHORT*) sxrealloc (stack, lgt1 + 1, sizeof (SXSHORT));
}




static BOOLEAN	set_next_item (vector, check, action, Max)
    struct SXP_item	*vector;
    register SXSHORT	*check, *action, Max;
{
    register struct SXP_item	*aitem;
    register SXSHORT	val;

    if ((val = mvector - vector) > *check)
	*check = val - 1;

    if ((val = Mvector - vector) < Max)
	*check = val;

    for (aitem = vector + ++*check; *check <= Max; aitem++, ++*check)
	if (aitem->check == *check) {
	    *action = aitem->action;
	    return TRUE;
	}

    return FALSE;
}



static VOID	set_first_trans (abase, check, action, Max, next_action_kind)
    register struct SXP_bases	*abase;
    register SXSHORT	*check, *action, Max;
    int		*next_action_kind;
{
    register struct SXP_item	*aitem;
    register SXSHORT	ref = abase->commun;

    if (ref <= sxplocals.SXP_tables.Mref /* codage direct */ ) {
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
	set_next_item (sxplocals.SXP_tables.vector + ref, check, action, Max) /* always true (commun non vide) */ ;
	*next_action_kind = Commun;
    }
}



static BOOLEAN	set_next_trans (abase, check, action, Max, next_action_kind)
    register struct SXP_bases	*abase;
    register SXSHORT	*check, *action, Max;
    int		*next_action_kind;
{
    switch (*next_action_kind) {
    case Commun:
	if (set_next_item (sxplocals.SXP_tables.vector + abase->commun, check, action, Max))
	    return TRUE;
	if (abase->propre != 0 /* partie propre non vide */ ) {
	    *next_action_kind = Propre;
	    *check = 0;
        }
        /* pas de break; ici */

    case Propre:
	if (set_next_item (sxplocals.SXP_tables.vector + abase->propre, check, action, Max))
	    return TRUE;
	/* else pas de break; ici */

    case Defaut:
	*check = 0;
	*action = abase->defaut;
	*next_action_kind = Done;
	return TRUE;

    case Done:
	return FALSE;
    }
    /* NOTREACHED */
}




static int	get_tail (binf, bsup)
    int		binf, bsup;
{
    if (!truncate_context)
	return bsup;

    if (binf == 0)
	binf = 1;

    for (; binf <= bsup; binf++) {
	TOK_i = TOK_0 + binf;

	if (sxgetbit (sxplocals.SXP_tables.PER_tset, SXGET_TOKEN (TOK_i).lahead))
	    return binf;
    }

    return bsup;
}



static	void restaure_stack ()
{
    register int	elem;

    elem = -1;

    while ((elem = sxba_scan (modified_set, elem)) >= 0) {
	sxba_0_bit (modified_set, elem);
	stack [elem + 1] = st_stack [elem + 1];
    }
}



static BOOLEAN	is_a_right_ctxt (head, tail, pxs, pstate, keep_stack)
    int		head, tail, *pxs;
    SXSHORT	*pstate;
    BOOLEAN	keep_stack;

/* Check if tokens contained in tokens[head..tail] are a valid right context of
   stack. Assume that tail takes into account the occurrences of key-terminals.
   Predicates are tested only at places where they are supposed to return the same value as
   in normal analysis i.e. head==0 and head==1
   If keep_stack do not modify stack */


{
    register SXSHORT	ref;
    register int	xs = *pxs;
    register struct SXP_reductions	*ared;
    register SXSHORT	state = *pstate, look_ahead;
    BOOLEAN	is_rc, bscan;

    is_rc = TRUE;

    for (; head <= tail && is_rc; head++) {
	TOK_i = TOK_0 + head;
	look_ahead = SXGET_TOKEN (TOK_i).lahead;
	bscan = FALSE;

	while (!bscan) {
	    ref = P_access (sxplocals.SXP_tables.t_bases + state, look_ahead);

NEW_REF:    if (ref > 0 /* Scan */ )
		bscan = TRUE;
	    else
		ref = -ref;

	    if (ref > sxplocals.SXP_tables.Mprdct /* Shift */ ) {
		if (++xs > lgt1)
		    oflow ();

		if (keep_stack && !sxba_bit_is_set (modified_set, xs - 1)) {
		    sxba_1_bit (modified_set, xs - 1);
		    st_stack [xs] = stack [xs];
		}

		if ((state = ref - sxplocals.SXP_tables.Mfe) > 0 /* direct */ ) {
		    if ((ref -= sxplocals.SXP_tables.M0ref) > 0 /* etat == etiq */ ) {
			stack [xs] = sxplocals.SXP_tables.Mref + (state = ref);
		    }
		    else
			stack [xs] = 0;
		}
		else {
		    register struct SXP_goto	*afe = sxplocals.SXP_tables.gotos + ref;

		    stack [xs] = afe->state;
		    state = afe->next;
		}
	    }
	    else if (ref > sxplocals.SXP_tables.Mred /* Predicates */ || ref == 0 /* Error */ ) {
		is_rc = FALSE;
		break;
	    }
	    else {
		/* Reduce or Halt*/
		ared = sxplocals.SXP_tables.reductions + ref;
		xs -= ared->lgth;

		if (ared->reduce == 0 /* Halt */ ) {
		    is_rc = (look_ahead == sxplocals.SXP_tables.P_tmax);
		    break;
		}

		{
		    register SXSHORT	nt;

		    if ((nt = (ref = ared->lhs) - sxplocals.SXP_tables.Mref) > 0 /* pas branchement direct */  && (state
			 = (ref = stack [xs]) - sxplocals.SXP_tables.Mref) > 0
			/* pas adresse de branchement en pile */
									      ) {
			ref = P_access (sxplocals.SXP_tables.nt_bases + state, nt);
		    }

		    goto NEW_REF;
		}
	    }
	}
    }

    if (keep_stack)
	restaure_stack ();
    else
	*pxs = xs, *pstate = state;

    return is_rc;
}



static BOOLEAN	check_nt_ctxt (head, tail, x, ref)
    int		head, tail, *x;
    SXSHORT	*ref;
{
    /* On vient d'effectuer une reduction (*ref < 0). On regarde si t[head..tail] est un
       contexte valide. */

    register SXSHORT	REF = -*ref;

    if (REF <= sxplocals.SXP_tables.Mprdct /* Error, Reduction or Predicates */ )
	/* On ne suit pas les chaines de reductions. Ces nouvelles reductions seront
	   examinees plus tard quand on sera au bon niveau de pile */
	/* User's predicate or &Else (assume always returning false) */
	    return FALSE;

    {   /* Shift */
	register SXSHORT	state, prev_state, *astackx;

	if (++*x > lgt1)
	    oflow ();

	prev_state = *(astackx = stack + *x);

	if ((state = REF - sxplocals.SXP_tables.Mfe) > 0 /* direct */ ) {
	    if ((REF -= sxplocals.SXP_tables.M0ref) > 0 /* etat == etiq */ ) {
		*astackx = sxplocals.SXP_tables.Mref + REF;
	    }
	    else
		*astackx = 0, REF = state;
	}
	else {
	    register struct SXP_goto	*afe = sxplocals.SXP_tables.gotos + REF;

	    *astackx = afe->state;
	    REF = afe->next;
	}

	*ref = REF;

	if (is_a_right_ctxt (head, tail, x, ref, TRUE))
	    return TRUE;


/* On remet les lieux dans l'etat initial */

	*astackx = prev_state;
	--*x;
    }

    return FALSE;
}




static VOID	set_vt_set (stack, xstd, state1)
    int		xstd;
    SXSHORT	state1;
    SXSHORT	*stack;

/* remplit vt_set */

{
    register struct SXP_reductions	*ared;
    register SXSHORT	*astackj;
    register struct SXP_bases	*abase = sxplocals.SXP_tables.t_bases + state1;
    SXSHORT	ref, test, j, k, prev_state;
    int		next_action_kind;

    set_first_trans (abase, &test, &ref, sxplocals.SXP_tables.P_tmax, &next_action_kind);

    do {
	j = xstd;

NEW_REF:
	if (ref > 0 /* scan */ ) {
	    sxba_1_bit (vt_set, test - 1);
	    continue;
	}

	ref = -ref;

	if (ref > sxplocals.SXP_tables.Mprdct /* Shift */ ) {
	    if (++j > lgt1)
		oflow ();

	    astackj = stack + j;
	    prev_state = *astackj;

	    if (ref > sxplocals.SXP_tables.Mfe /* direct */ ) {
		if ((ref -= sxplocals.SXP_tables.M0ref) > 0 /* etat == etiq */ ) {
		    *astackj = sxplocals.SXP_tables.Mref + ref;
		}
		else
		    *astackj = 0;
	    }
	    else {
		register struct SXP_goto	*afe = sxplocals.SXP_tables.gotos + ref;

		*astackj = afe->state;
		ref = afe->next;
	    }

	    set_vt_set (stack, j, ref);
	    *astackj = prev_state;
	}
	else if (ref > sxplocals.SXP_tables.Mred /* predicates */ ) {
	    has_prdct = TRUE;
	    /* On ne valide pas les terminaux etendus */	    
	}
	else if (ref == 0 /* error */ ) {
	    return;
	}
	else {
	    /* Reduce or Halt */
	    ared = sxplocals.SXP_tables.reductions + ref;
	    j -= ared->lgth;

	    if (ared->reduce == 0 /* Halt */ ) {
		sxba_1_bit (vt_set, sxplocals.SXP_tables.P_tmax - 1);
		return;
	    }

	    {
		register SXSHORT	nt, state;

		if ((nt = (ref = ared->lhs) - sxplocals.SXP_tables.Mref) > 0 /* pas branchement direct */  && (state = (
		     ref = stack [j]) - sxplocals.SXP_tables.Mref) > 0
		    /* pas adresse de branchement en pile */
								      ) {
		    ref = P_access (sxplocals.SXP_tables.nt_bases + state, nt);
		}

		goto NEW_REF;
	    }
	}
    } while (set_next_trans (abase, &test, &ref, sxplocals.SXP_tables.P_tmax, &next_action_kind));
}




static SXVOID vt_set_processing (force_correction_on_error_token, xs, state)
    BOOLEAN	force_correction_on_error_token;
    int		xs;
    SXSHORT	state;

/* state est un t_state */

{
    int		xs2 = xs;
    SXSHORT	state2 = state;

    sxba_empty (vt_set);
    has_prdct = FALSE;

    if (!force_correction_on_error_token) {
	if (!is_a_right_ctxt (0, 0, &xs2, &state2, FALSE))
	    return;
    }

    set_vt_set (stack, xs2, state2);
    restaure_stack ();
}



/* E R R O R   R E C O V E R Y */

static BOOLEAN	recovery ()
{
    char	*msg_params [5];
    register int	i, j, x, l;
    register SXSHORT	*regle;
    register struct SXP_local_mess	*local_mess;
    int		head, tail, im, k, ll, vt_set_card, x1, y, y1, xps, action, next_action_kind;
    SXSHORT	state, check;
    BOOLEAN	is_warning, no_correction_on_previous_token;
    struct sxtoken	*p;
    struct sxsource_coord	source_index;

    if (stack == NULL) {
	/* first error in the process */
	stack = (SXSHORT*) sxalloc ((lgt1 = 200) + 1, sizeof (SXSHORT));
	st_stack = (SXSHORT*) sxalloc (lgt1 + 1, sizeof (SXSHORT));
	modified_set = sxba_calloc (lgt1);
	vt_set = sxba_calloc (tmax);
    }

    mvector = sxplocals.SXP_tables.vector + sxplocals.SXP_tables.mxvec;
    Mvector = sxplocals.SXP_tables.vector + sxplocals.SXP_tables.Mxvec;

/* L'erreur sera marquee (listing) a l'endroit de la detection */

    source_index = SXGET_TOKEN (sxplocals.ptok_no).source_index;

    if ((no_correction_on_previous_token = (sxplocals.ptok_no == sxplocals.atok_no))) {
	/* On suppose qu'on n'est jamais en debut de tokens_buf! */
	sxplocals.atok_no--;
    }

    is_warning = TRUE;
    state = sxplocals.state;
    xps = sxpglobals.xps;

    for (i = sxpglobals.stack_bot + 1; i <= xps; i++) {
	/* sauvegarde de la pile */
	stack [i] = sxpglobals.parse_stack [i].state;
    }

    vt_set_processing (no_correction_on_previous_token, xps, state);

    if ((vt_set_card = sxba_cardinal (vt_set)) == 1 && !has_prdct && (k = sxba_scan (vt_set, -1) + 1) != sxplocals.
	 SXP_tables.P_tmax) {
	struct sxtoken toks[2];

	/* un seul suivant valide different de EOF ==> insertion forcee */
	/* Pas de predicats dans le coup */
	local_mess = sxplocals.SXP_tables.SXP_local_mess + sxplocals.SXP_tables.P_nbcart + 1;

	/* preparation du message */
	
	for (j = local_mess->P_param_no - 1; j >= 0; j--) {
	    register struct sxtoken	*tok;
	    register int ate;

	    i = local_mess->P_param_ref [j];
	    tok = (TOK_i = TOK_0 + i, &SXGET_TOKEN (TOK_i));
	    msg_params [j] = i < 0 /* assume %0 */
				   ? ttext (sxplocals.sxtables, k)
				   : ((ate = tok->string_table_entry) == EMPTY_STE || ate == ERROR_STE
				      ? ttext (sxplocals.sxtables, tok->lahead)
				      : sxstrget (ate));
	}


/* insertion du nouveau token, avec source_index et comment */

	toks [0] = SXGET_TOKEN (TOK_0);
	p = toks + 1;
	p->lahead = k;
	p->string_table_entry = ERROR_STE /* error value */ ;
	p->comment = NULL;
	p->source_index = source_index;

	if (sxgenericp (sxplocals.sxtables, k))
	    is_warning = FALSE;

	sxtknmdf (toks, 2, sxplocals.atok_no, 1);
	sxplocals.ptok_no = sxplocals.atok_no += (no_correction_on_previous_token ? 1 : 0);

	/* "Warning: "machin" is replaced by chose." ou similaire */

    if (!sxplocals.mode.is_silent)
	sxput_error (source_index,
		     local_mess->P_string,
		     is_warning ? sxplocals.sxtables->err_titles [1] : sxplocals.sxtables->err_titles [2],
		     msg_params [0],
		     msg_params [1],
		     msg_params [2],
		     msg_params [3],
		     msg_params [4]);
	return TRUE;
    }


/* rattrapage global */

/* message sur les suivants attendus */

    if (((unsigned int) (vt_set_card - 1)) < sxplocals.SXP_tables.P_followers_number /* a < x <= b */ && !has_prdct ) {
	i = 0;
	j = -1;

	while ((j = sxba_scan (vt_set, j)) >= 0) {
	    msg_params [i++] = ttext (sxplocals.sxtables, j + 1);
	}


/* "Error: "machin" is expected." */

	if (!sxplocals.mode.is_silent)
	    sxput_error (source_index,
			 sxplocals.SXP_tables.P_global_mess [vt_set_card - 1],
			 sxplocals.sxtables->err_titles [2],
			 msg_params [0],
			 msg_params [1],
			 msg_params [2],
			 msg_params [3],
			 msg_params [4]);
    }


/* "Error: Global Recovery." */

    if (!sxplocals.mode.is_silent)
	sxput_error (source_index,
		     sxplocals.SXP_tables.P_global_mess [sxplocals.SXP_tables.P_followers_number],
		     sxplocals.sxtables->err_titles [2]);

/* on y va */

    for (;;) {
	sxget_token (sxplocals.atok_no + sxplocals.SXP_tables.P_validation_length);

/* avaler un token */
	TOK_0 = ++sxplocals.atok_no;
	p = &(SXGET_TOKEN (TOK_0));

	if (p->lahead == sxplocals.SXP_tables.P_tmax) {
	    /* on a trouve la fin du fichier */
	    /* "Error: Parsing stops on End of File." */
	    if (!sxplocals.mode.is_silent)
		sxput_error (p->source_index,
			     sxplocals.SXP_tables.P_global_mess
			        [sxplocals.SXP_tables.P_followers_number + 3],
			     sxplocals.sxtables->err_titles [2]);
	    /* On simule une reduction entre top = sxpglobals.xps - 1 (l'ancien sommet de
	       pile est non significatif en tant que token) et new_top = sxpglobals.stack_bot + 1
	       pour les actions */
	    sxpglobals.xps-- /* pour STACKtop */;
	    sxpglobals.pspl = sxpglobals.xps - sxpglobals.stack_bot - 1 /* pour STACKnew_top */ ;
	    (*(sxplocals.SXP_tables.semact)) (ERROR);
	    sxplocals.ptok_no = sxplocals.atok_no;
	    return FALSE;
	}

	if (sxgetbit (sxplocals.SXP_tables.PER_tset, p->lahead)) {
	    /* terminal courant == terminal cle */
	    /* on regarde si les P_validation_length terminaux suivants
	       sont des suivants valides, puis sinon on "pop" la pile et
	       on recommence; si on tombe au fond de la pile on avance
	       d'un token */
	    xps = sxpglobals.xps;
	    state = sxplocals.state;
	    head = 0;
	    tail = get_tail (0, sxplocals.SXP_tables.P_validation_length - 1);

	    if (is_a_right_ctxt (head, tail, &xps, &state, TRUE))
		goto parsing_resume;

	    for (xps = sxpglobals.xps; xps > sxpglobals.stack_bot; xps--) {
		/* pop the stack and check the right context */

		if ((state = sxpglobals.parse_stack [xps].state) != 0) {
		    if ((action = state - sxplocals.SXP_tables.Mref) > 0 /* vrai etat en pile */ ) {
			register struct SXP_bases	*abase = sxplocals.SXP_tables.nt_bases + action;

			set_first_trans (abase, &check, &state, sxplocals.SXP_tables.P_ntmax, &next_action_kind);

			do {
			    if (check_nt_ctxt (head, tail, &xps, &state))
				goto model0;
			} while (set_next_trans (abase, &check, &state, sxplocals.SXP_tables.P_ntmax, &next_action_kind))
			     ;
		    }
		    else /* action en pile */  {
			if (check_nt_ctxt (head, tail, &xps, &state))
			    goto model0;
		    }
		}
	    }
	}
    }

model0:
    /* On simule une reduction entre top = sxpglobals.xps - 1 (l'ancien sommet de
       pile est non significatif en tant que token) et new_top = xps - 1 (on vient
       d'effectuer un Shift) pour les actions */
       
    sxpglobals.xps--;
    sxpglobals.pspl = sxpglobals.xps - xps + 1 /* pour STACKnew_top eventuel */ ;
    (*(sxplocals.SXP_tables.semact)) (ERROR);
    /* reajustement de la pile */
    sxpglobals.parse_stack [xps - 1].token.lahead = 0 /* simulation of a NT at new_top */ ;
    sxpglobals.xps = xps;
    sxpglobals.parse_stack [xps].state = stack [xps] /* Le NT_state empile */;
    sxplocals.state = state /* Le T_state atteint */;

parsing_resume:
    {
	int	ate;

	if ((i = sxplocals.SXP_tables.P_validation_length) > 5) {
	    i = 5;
	}

	while (i-- > 0) {
	    p = (TOK_i = TOK_0 + i, &SXGET_TOKEN (TOK_i));
	    msg_params [i] = (ate = p->string_table_entry) == EMPTY_STE || ate == ERROR_STE
		? ttext (sxplocals.sxtables, p->lahead)
		: sxstrget (ate);
	}


/* "Warning: Parsing resumes on "machin"." */

	if (!sxplocals.mode.is_silent)
	    sxput_error ((SXGET_TOKEN (TOK_0)).source_index,
			 sxplocals.SXP_tables.P_global_mess
			    [sxplocals.SXP_tables.P_followers_number + 1],
			 sxplocals.sxtables->err_titles [1],
			 msg_params [0],
			 msg_params [1],
			 msg_params [2],
			 msg_params [3],
			 msg_params [4]);
    }

    sxplocals.ptok_no = sxplocals.atok_no;
    return TRUE;
}




BOOLEAN		sxpsrecovery (what_to_do)
    int		what_to_do;
{
    switch (what_to_do) {
    case OPEN:

/* update sizes of other static arrays and maybe reallocate */

	if (tmax < sxplocals.SXP_tables.P_tmax) {
	    tmax = sxplocals.SXP_tables.P_tmax;

	    if (vt_set != NULL) {
		vt_set = sxba_resize (vt_set, tmax);
	    }
	}

	break;

    case ACTION:
	if (sxplocals.mode.kind == SXWITH_CORRECTION)
	    /* Seule une tentative de correction a ete demandee. */
	    return FALSE;

	truncate_context = TRUE;
	return recovery ();

    case CLOSE:
	if (stack != NULL) {
	    sxfree (vt_set), vt_set = NULL;
	    sxfree (modified_set), modified_set = NULL;
	    sxfree (stack), stack = NULL;
	}

	break;

    default:
	fprintf (sxstderr, "The function \"sxpsrecovery\" is out of date with respect to its specification.\n");
	abort ();
    }

    return TRUE;
}
