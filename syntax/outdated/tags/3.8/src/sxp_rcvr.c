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
/*				"is_a_right_ctxt", "check_nt_ctxt" et	*/
/*				"deriv" (bug sur oflow).		*/
/************************************************************************/
/* 27-01-93 14:15 (pb):		Adaptation au nouveau token_mngr	*/
/************************************************************************/
/* 24-04-92 10:35 (pb):		Bug sur la valeur de atok_no retournee	*/
/*				lorsque un predicat est associe au	*/
/*				token precedent.			*/
/************************************************************************/
/* 30-03-92 10:35 (pb):		Utilisation du "mode" d'analyse		*/
/************************************************************************/
/* 18-03-92 11:20 (pb):		La zone "comment" des tokens inseres	*/
/*				n'etait pas remise a NULL lorsque les	*/
/*				commentaires etaient supprimes.		*/
/************************************************************************/
/* 11-09-91 11:20 (pb):		Amelioration ds deriv : les actions	*/
/*				reduce (avec champ test explicite et	*/
/*				sans scan) ne sont plus executees qu'une*/
/*				seule fois a un niveau donne.		*/
/************************************************************************/
/* 05-07-91 09:15 (pb):		Bug (grossier) detecte par GEC-ALSTHOM	*/
/*				La taille de la "parse_stack" pouvait	*/
/*				etre superieure a la taille des piles	*/
/*				locales paralleles			*/
/************************************************************************/
/* 05-10-89 11:15 (pb):		Prise en compte d'un appel recursif de	*/
/*				"recovery" par l'intermediaire du	*/
/*				"scanner" (sxget_token)			*/
/************************************************************************/
/* 24-11-88 09:40 (pb):		Suppression du dernier parametre de	*/
/*				P_access				*/
/*				is_a_right_ctxt rend FALSE si predicat	*/
/************************************************************************/
/* 03-06-88 10:19 (pb):		Bug pour STACKtop et STACKnew_top dans	*/
/*				le cas "End Of File"			*/
/************************************************************************/
/* 31-05-88 10:02 (pb):		Bug dans set_next_trans			*/
/************************************************************************/
/* 05-05-88 11:15 (pb):		Bug recuperation globale introduit sans */
/*				doute avec la 3.0			*/
/************************************************************************/
/* 15-04-88 14:12 (pb):		Adaptation au "token manager"		*/
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

char	what_sxp_rcvr [] = "@(#)sxprecovery.c\t- SYNTAX [unix] - Jeu 21 Oct 1993";

#endif

# include "sxunix.h"

#define Commun 1
#define Propre 2
#define Defaut 3
#define Done   4

extern SXSHORT	P_access ();

/*   V A R I A B L E S   */

static int	lgt1, sizofpts = 0, tmax = 0, nmax = 0, min_xs;
static SXSHORT	*stack = NULL, *st_stack /* lgt1 */ ;
static int	*ster, *lter /* sizofpts */, Mtstate;
static struct sxtoken	*toks /* sizofpts */ ;
static char	**com /* sizofpts */ ;
static SXBA	modified_set /* lgt1 */ , vt_set /* tmax */ , to_be_checked /* nmax+1 */ ;
static struct {
	   int	*correction /* sizofpts */ ;
	   int	modele_no;
       }	a_la_rigueur;
static int	nomod;
static BOOLEAN	is_correction_on_error_token;
static struct SXP_item	*mvector, *Mvector;
static BOOLEAN	truncate_context, has_prdct;

static int TOK_0, TOK_i;


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
    struct SXP_bases	*abase;
    SXSHORT	*check, *action, Max;
    int		*next_action_kind;
{
    SXSHORT	ref = abase->commun;

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
    SXSHORT	*check, *action, Max;
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



static BOOLEAN	reduce_already_seen (abase, check, action)
    register struct SXP_bases	*abase;
    SXSHORT	check, action;
{
    /* action ( >0) reference une action reduce qui n'est pas l'action par defaut
       du abase courant. On regarde si cette action n'a pas deja ete traitee. */

	SXSHORT		ref, test;
	int	next_action_kind;

	set_first_trans (abase, &test, &ref, sxplocals.SXP_tables.P_tmax, &next_action_kind);

	do {
	    if (test == check)
		return FALSE;

	    if (ref == -action)
		return TRUE;
	} while (set_next_trans (abase, &test, &ref, sxplocals.SXP_tables.P_tmax, &next_action_kind));

	return FALSE; /* garde-fou */
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

	if (sxgetbit (sxplocals.SXP_tables.PER_tset,
		      SXGET_TOKEN (TOK_i).lahead))
	    return binf;
    }

    return bsup;
}



static	void restaure_stack (m, M)
    int		m, M;
{
    register int	i;

    for (i = m; i <= M; i++) {
	/* sauvegarde de la pile */
	stack [i] = sxpglobals.parse_stack [i].state;
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

    if (!keep_stack)
	min_xs = xs;

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

		if (!keep_stack && xs < min_xs)
		    min_xs = xs;

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

    if (keep_stack) {
	register int	elem = -1;

	while ((elem = sxba_scan (modified_set, elem)) != -1) {
	    sxba_0_bit (modified_set, elem);
	    stack [elem + 1] = st_stack [elem + 1];
	}
    }
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

   {
	/* Shift */
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




static BOOLEAN	morgan (c1, c2)
    char	*c1, *c2;

/* rend vrai si c1 = c2 a une faute d'orthographe pres */

{
    register char	*tmax, *t1, *t2;
    int		l1, l2;
    char	d1, d2, e1, e2;

    switch ((l1 = strlen (c1)) - (l2 = strlen (c2))) {
    default:
	/* longueurs trop differentes */
	return FALSE;

    case 0:
	/* longueurs egales: on autorise un remplacement ou une interversion
	   de caracteres */
	tmax = c1 + l1, t1 = c1, t2 = c2;

	while (t1 < tmax && *t1 == *t2)
	    t1++, t2++;

	if (t1 == tmax)
	    /* egalite */
	    return TRUE;

	d1 = *t1++, d2 = *t2++ /* caracteres courants, differents */ ;
	e1 = *t1++, e2 = *t2++ /* caracteres suivants */ ;

	if (e1 != e2 /* caracteres suivants differents */
		     && (d1 != e2 || e1 != d2))
	    /* caracteres pas inverses */
	    return FALSE;


/* les restes des chaines doivent etre egaux */

	while (t1 < tmax && *t1 == *t2)
	    t1++, t2++;

	return t1 >= tmax /* >= pour le cas ou seuls les deux derniers caracteres sont differents */ ;

    case -1:
	tmax = c2 + l2, t1 = c2, t2 = c1;
	break;

    case 1:
	tmax = c1 + l1, t1 = c1, t2 = c2;
	break;
    }


/* longueurs egales a +- 1, t1 est la chaine la plus longue */
    /* il y a un caractere en trop dans la chaine la plus longue et ce doit
       etre la seule difference */

    while (t1 < tmax && *t1 == *t2)
	t1++, t2++;

    if (*t2 == NUL)
	/* egalite au dernier caractere pres */
	return TRUE;

    t1++ /* on saute un caractere dans la chaine la plus longue */ ;


/* les restes des chaines doivent etre egaux */

    while (t1 < tmax && *t1 == *t2)
	t1++, t2++;

    return t1 >= tmax;
}




static BOOLEAN match_a_tok (cur_mod, n, tok)
    int 	cur_mod, n, tok;
{
    /* Cette fonction regarde si le token "tok" match le nieme element de modele cur_mod */
    register SXSHORT	*regle;
    register int	j;
    
    regle = sxplocals.SXP_tables.P_lregle [cur_mod];

    if (n > regle [0] || (j = regle [n]) == -1)
	return TRUE;

    if (j >= 0) {
	TOK_i = TOK_0 + j;

	if (tok != SXGET_TOKEN (TOK_i).lahead)
	    return FALSE;

	return TRUE;
    }

    TOK_i = TOK_0 + n - 1;

    if (sxplocals.sxtables->SXS_tables.check_keyword == NULL || sxstrlen (j = SXGET_TOKEN (TOK_i).string_table_entry) <= 2 || !sxkeywordp (sxplocals.sxtables, tok) ||
	!morgan (sxstrget (j), sxttext (sxplocals.sxtables, tok)))
	return FALSE;

    return TRUE;
}


static BOOLEAN search_a_rule (cur_mod, n, best_mod, ster)
    int 		*cur_mod, n, best_mod, *ster;
{
    /* Recherche un modele de priorite inferieure a cur_mod dont le prefixe verifie ster */

    register int 	mod, k, bot;
    register SXSHORT	*regle;

    for (mod = *cur_mod + 1; mod < best_mod; mod++) {
	regle = sxplocals.SXP_tables.P_lregle [mod];
	k = regle [1];

	if (is_correction_on_error_token ? k == 0 : (k != 0)) {
	    bot = is_correction_on_error_token ? 1 : 0;

	    if ((k = regle [0]) > n)
		k = n;

	    for (; k > bot; k--) {
		if (!match_a_tok (mod, k, ster [k]))
		    break;
	    }

	    if (k == bot)
		break;
	}
    }

    return (*cur_mod = mod) < best_mod;
}


static BOOLEAN is_valid (cur_mod, n, ster)
    int cur_mod, n, *ster;
{
    /* Regarde si la regle de correction d'erreur cur_mod est valide par rapport
       aux don't delete et don't insert */
    register SXSHORT	*regle;
    register int	j, k, lmod, head;

    regle = sxplocals.SXP_tables.P_lregle [cur_mod];

    if ((lmod = regle [0]) > n)
	lmod = n;

    head = sxplocals.SXP_tables.P_right_ctxt_head [cur_mod];
    sxba_fill (to_be_checked);

    for (k = 1; k <= lmod; k++) {
	if ((j = regle [k]) >= 0) {
	    if (j >= head)
		break;

	    sxba_0_bit (to_be_checked, j);
	}
	else if (sxgetbit (sxplocals.SXP_tables.P_no_insert, ster [k]))
	    return FALSE;
    }

    j = -1;

    while ((j = sxba_scan (to_be_checked, j)) >= 0 && j < head) {
	TOK_i = TOK_0 + j;

	if (sxgetbit (sxplocals.SXP_tables.P_no_delete, SXGET_TOKEN (TOK_i).lahead))
	    return FALSE;
    }

    return TRUE;
}


static VOID	deriv (string_has_grown, n, xstd, t_state, rule_no)
    int		n, xstd, rule_no;
    SXSHORT	t_state;
    BOOLEAN	string_has_grown;

/* fabrique les corrections locales; place ses resultats dans "nomod" (numero
   du modele) et "lter" (nouveau flot de tokens) */
/* Suppose que t_state designe une T_table */

{
    if (string_has_grown) {
	BOOLEAN must_return = TRUE;

	if (match_a_tok (rule_no, n, ster [n]) || search_a_rule (&rule_no, n, nomod, ster))
	    do {
	    register SXSHORT	*regle = sxplocals.SXP_tables.P_lregle [rule_no];
	    register int	head, lmod;
	    register int 	*best_str, i;

	    if (n < (lmod = regle [0])) {
		if ((head = regle [n + 1]) >= sxplocals.SXP_tables.P_right_ctxt_head [rule_no]) {
		    if (!is_a_right_ctxt (head, get_tail (head, regle [lmod]), &xstd, &t_state, TRUE))
			continue;
		}
		else {
		    must_return = FALSE;
		    break;
		}
	    }

	    /* Here the model rule_no has been checked */
	    if (is_valid (rule_no, n, ster)) {
		nomod = rule_no;
		best_str = lter;
	    }
	    else if (rule_no < a_la_rigueur.modele_no) {
		a_la_rigueur.modele_no = rule_no;
		best_str = a_la_rigueur.correction;
	    }
	    else return;

	    for (i = 1; i <= n; i++) {
		best_str [i] = ster [i];
	    }

	    for (; i <= lmod; i++) {
		TOK_i = TOK_0 + regle [i];
		best_str [i] = SXGET_TOKEN (TOK_i).lahead;
	    }

	    return;
	} while (search_a_rule (&rule_no, n, nomod, ster));

	if (must_return)
	    return;
    }

    /* Here the string ster [1:n] matches the prefix of length n of model rule_no */

    {
	register struct SXP_reductions	*ared;
	register SXSHORT		state, *astackj;
	register struct SXP_bases	*abase = sxplocals.SXP_tables.t_bases + t_state;
	SXSHORT				ref, test, j, k, prev_state;
	int				next_action_kind;
	BOOLEAN				scan;

	set_first_trans (abase, &test, &ref, sxplocals.SXP_tables.P_tmax, &next_action_kind);

	do {
	    k = n;
	    j = xstd;

NEW_REF:    if (scan = (ref > 0) /* scan */ ) {
		if (n == 1 && is_correction_on_error_token)
		    sxba_1_bit (vt_set, test - 1);

		ster [++k] = test;
	    }
	    else
		ref = -ref;

	    if (ref > sxplocals.SXP_tables.Mprdct /* Shift */ ) {
		if (++j > lgt1)
		    oflow ();

		astackj = stack + j;
		prev_state = *astackj;

		if ((state = ref - sxplocals.SXP_tables.Mfe) > 0 /* direct */ ) {
		    if ((ref -= sxplocals.SXP_tables.M0ref) > 0 /* etat == etiq */ ) {
			*astackj = sxplocals.SXP_tables.Mref + (state = ref);
		    }
		    else
			*astackj = 0;
		}
		else {
		    register struct SXP_goto	*afe = sxplocals.SXP_tables.gotos + ref;

		    *astackj = afe->state;
		    state = afe->next;
		}

		deriv (k > n, k, j, state, rule_no);
		*astackj = prev_state;
	    }
	    else if (ref > sxplocals.SXP_tables.Mred /* predicates */ ) {
		if (n == 1 && is_correction_on_error_token)
		    has_prdct = TRUE;
		/* On ne valide pas les terminaux etendus */
	    }
	    else if (ref > 0 /* Reduce or Halt */ ) {
		ared = sxplocals.SXP_tables.reductions + ref;
		j -= ared->lgth;

		if (ared->reduce == 0 /* Halt */ ) {
		    int cur_mod;

		    for (k++; k <= sxplocals.SXP_tables.P_maxlregle; k++) {
			ster [k] = sxplocals.SXP_tables.P_tmax;
		    }

		    /* deriv suppose que rule_no est verifie jusqu'a n-1, ce n'est pas le
		       cas ici => appel de search_a_rule */
		    cur_mod = rule_no - 1;

		    if (search_a_rule (&cur_mod, sxplocals.SXP_tables.P_maxlregle, nomod, ster))
			deriv (TRUE, sxplocals.SXP_tables.P_maxlregle, j, 0, cur_mod);
		}
		else /* Reduce */
		if (test == 0 || scan || !reduce_already_seen (abase, test, ref)) {
		    register SXSHORT	nt, state;

		    if ((nt = (ref = ared->lhs) - sxplocals.SXP_tables.Mref) > 0 /* pas branchement direct */  && (state
			 = (ref = stack [j]) - sxplocals.SXP_tables.Mref) > 0
			/* pas adresse de branchement en pile */
									     ) {
			ref = P_access (sxplocals.SXP_tables.nt_bases + state, nt);
		    }

		    goto NEW_REF;
		}
		/* else reduction deja examinee, on ne recommence pas... */
	    }
	} while (set_next_trans (abase, &test, &ref, sxplocals.SXP_tables.P_tmax, &next_action_kind));
    }
}




static BOOLEAN	is_a_correction (force_correction_on_error_token, xs, state)
    BOOLEAN	force_correction_on_error_token;
    int		xs;
    SXSHORT	state;

/* rend vrai si on peut trouver une correction locale */
/* state est un t_state */

{
    register int	i, l;
    int		xs2 = xs;
    SXSHORT	state2 = state;

    a_la_rigueur.modele_no = sxplocals.SXP_tables.P_nbcart + 1;
    sxba_empty (vt_set);
    has_prdct = FALSE;

    if (!force_correction_on_error_token)
	if (!is_a_right_ctxt (0, 0, &xs2, &state2, FALSE))
	    /* Prdct on previous token */
	    return FALSE;

    ster [1] = (SXGET_TOKEN (TOK_0)).lahead;
    nomod = sxplocals.SXP_tables.P_min_prio_0 + 1;
    is_correction_on_error_token = TRUE;
    deriv (TRUE, 1, xs2, state2, 1);

    if (nomod > sxplocals.SXP_tables.P_min_prio_0)
	nomod = sxplocals.SXP_tables.P_nbcart + 1;

    if (!force_correction_on_error_token)
	restaure_stack (min_xs + 1, xs);


/* On regarde s'il existe un modele plus prioritaire que nomod pouvant
   corriger le terminal precedant le terminal en erreur. */

    if (!force_correction_on_error_token && sxplocals.SXP_tables.P_max_prio_X < nomod) {
	is_correction_on_error_token = FALSE;
	deriv (FALSE, 0, xs, state, sxplocals.SXP_tables.P_max_prio_X);
    }

    if (nomod > sxplocals.SXP_tables.P_nbcart) {
	if (a_la_rigueur.modele_no > sxplocals.SXP_tables.P_nbcart)
	    return FALSE;

	l = sxplocals.SXP_tables.P_lregle [nomod = a_la_rigueur.modele_no] [0];

	for (i = 1; i <= l; i++)
	    lter [i] = a_la_rigueur.correction [i];
    }

    return TRUE;
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

    if ((no_correction_on_previous_token = (sxplocals.ptok_no == sxplocals.atok_no))) {
	/* On suppose qu'on n'est jamais en debut de tokens_buf! */
	sxplocals.atok_no--;
    }

/* accumulation de tokens en avance */
/* L'appel a sxget_token doit s'effectuer avant tout positionnement de variables statiques
   car un appel recursif par l'intermediaire d'une action du scanner appele par
   sxget_token. */
    sxget_token (sxplocals.atok_no + sxplocals.SXP_tables.P_nmax);

    if (stack == NULL) {
	/* first error in the process */
	stack = (SXSHORT*) sxalloc ((lgt1 = 200) + 1, sizeof (SXSHORT));
	st_stack = (SXSHORT*) sxalloc (lgt1 + 1, sizeof (SXSHORT));
	modified_set = sxba_calloc (lgt1);
	com = (char**) sxalloc (sizofpts + 1, sizeof (char*));
	ster = (int*) sxalloc (sizofpts + 1, sizeof (int));
	lter = (int*) sxalloc (sizofpts + 1, sizeof (int));
	toks = (struct sxtoken*) sxalloc (sizofpts + 1, sizeof (struct sxtoken));
	a_la_rigueur.correction = (int*) sxalloc (sizofpts + 1, sizeof (int));
	vt_set = sxba_calloc (tmax);
    }

    if (to_be_checked == NULL)
	to_be_checked = sxba_calloc (nmax + 1);

    mvector = sxplocals.SXP_tables.vector + sxplocals.SXP_tables.mxvec;
    Mvector = sxplocals.SXP_tables.vector + sxplocals.SXP_tables.Mxvec;

/* L'erreur sera marquee (listing) a l'endroit de la detection */

    source_index = SXGET_TOKEN (sxplocals.ptok_no).source_index;
    is_warning = TRUE;
    TOK_0 = sxplocals.atok_no;
    state = sxplocals.state;
    xps = sxpglobals.xps;

    while (lgt1 <= xps)
	oflow ();

    restaure_stack (sxpglobals.stack_bot + 1, xps);

    if (!is_a_correction (no_correction_on_previous_token, xps, state)) {
	/* correction a echouee */
	if (sxplocals.mode.kind == SXWITH_CORRECTION)
	    /* Seule une tentative de correction a ete demandee. */
	    return FALSE;

	goto riennevaplus;
    }


/* OK ca a marche */

    im = 0;
    regle = sxplocals.SXP_tables.P_lregle [nomod];
    local_mess = sxplocals.SXP_tables.SXP_local_mess + nomod;
    ll = regle [0];


/* preparation du message d'erreur */

    for (j = ll; j > 0; j--) {
	if (regle [j] > im)
	    im = regle [j];
    }

    for (j = local_mess->P_param_no - 1; j >= 0; j--) {
	register struct sxtoken		*tok;
	register int ate;

	i = local_mess->P_param_ref [j];
	tok = (TOK_i = TOK_0 + i, &SXGET_TOKEN (TOK_i));
	msg_params [j] = i < 0 /* X or S */
			       ? ttext (sxplocals.sxtables, lter [-i])
			       : ((ate = tok->string_table_entry) == EMPTY_STE || ate == ERROR_STE
				  ? ttext (sxplocals.sxtables, tok->lahead)
				  : sxstrget (ate));
    }

    /* On doit retrouver NULL ds le token, meme si les commentaires
       ne sont pas gardes. */
    for (y1 = 1; y1 <= ll; y1++)
	com [y1] = NULL;

    if (!sxsvar.SXS_tables.S_are_comments_erased) {

/* deplacement des commentaires */

	x = im;

	for (y = ll; regle [y] == x && x >= 0; y--) {
	    TOK_i = TOK_0 + x--;
	    com [y] = SXGET_TOKEN (TOK_i).comment;
	}

	if (ll < ++y)
	    y = ll;

	y1 = 1;

	for (x1 = 0; x1 <= x && y1 < y; x1++) {
	    TOK_i = TOK_0 + x1;
	    com [y1++] = SXGET_TOKEN (TOK_i).comment;
	}

	for (x1 = x1; x1 <= x; x1++) {
	    register struct sxtoken	*tok;

	    TOK_i = TOK_0 + x1;
	    tok = &(SXGET_TOKEN (TOK_i));

	    if (com [y] == NULL)
		com [y] = tok->comment;
	    else if (tok->comment != NULL) {
		/* concatenation des commentaires */
		char	*tcom = tok->comment;

		com [y] = sxrealloc (com [y], strlen (com [y]) + strlen (tcom) + 1, sizeof (char));
		strcat (com [y], tcom);
		sxfree (tcom);
	    }
	}
    }


/* deplacement des source_index
   principe:
   si un token se retrouve dans la chaine corrigee, il conserve son
   source_index, cela dans tous les cas (echange, duplication, ..)
   s'il correspond a un remplacement, il prend le source_index du
   token qu'il remplace
   s'il correspond a une insertion, il prend le source_index du token
   qui le suit.
*/

    x = 0;
    j = sxplocals.SXP_tables.P_right_ctxt_head [nomod];

    for (l = 1, p = toks; (i = regle [l]) < j; l++, p++) {
	if (i < 0) {
	    p->lahead = lter [l];
	    p->string_table_entry = ERROR_STE /* error value */ ;
	    TOK_i = TOK_0 + x;
	    p->source_index = SXGET_TOKEN (TOK_i).source_index;

	    if (sxgenericp (sxplocals.sxtables, lter [l]))
		is_warning = FALSE;
	}
	else {
	    *p = (TOK_i = TOK_0 + i, SXGET_TOKEN (TOK_i));
	    x = i + 1;
	}

	p->comment = com [l];
	/* et voila */
    }

exit_of_er_re:
    sxtknmdf (toks, l - 1, sxplocals.atok_no, j);
    sxplocals.ptok_no = sxplocals.atok_no += (no_correction_on_previous_token ? 1 : 0);

/* "Warning: "machin" is replaced by chose." ou similaire */

    if (!sxplocals.mode.is_silent)
	sxput_error (source_index, local_mess->P_string, is_warning ?
		     sxplocals.sxtables->err_titles [1] :
		     sxplocals.sxtables->err_titles [2],
		     msg_params [0],
		     msg_params [1],
		     msg_params [2],
		     msg_params [3],
		     msg_params [4]);

    return TRUE;


/* la correction locale a echoue */

riennevaplus:
    if ((vt_set_card = sxba_cardinal (vt_set)) == 1 && !has_prdct && (k = sxba_scan (vt_set, -1) + 1) != sxplocals.
	 SXP_tables.P_tmax) {
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

	toks [0] = (SXGET_TOKEN (TOK_0));
	p = toks + 1;
	p->lahead = k;
	p->string_table_entry = ERROR_STE /* error value */ ;
	p->comment = NULL;
	p->source_index = source_index;
	l = 3, j = 1;

	if (sxgenericp (sxplocals.sxtables, k))
	    is_warning = FALSE;

	goto exit_of_er_re;
    }


/* rattrapage global */

/* message sur les suivants attendus */

    if (((unsigned int) (vt_set_card - 1)) < sxplocals.SXP_tables.P_followers_number /* a < x <= b */  && !has_prdct) {
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




BOOLEAN		sxprecovery (what_to_do)
    int		what_to_do;
{
    switch (what_to_do) {
    case OPEN:

/* update sizes of other static arrays and maybe reallocate */

	stack = NULL;
	to_be_checked = NULL;

	if (sizofpts < sxplocals.SXP_tables.P_sizofpts) {
	    sizofpts = sxplocals.SXP_tables.P_sizofpts;

	    if (com != NULL) {
		com = (char**) sxrealloc (com, sizofpts + 1, sizeof (char*));
		ster = (int*) sxrealloc (ster, sizofpts + 1, sizeof (int));
		lter = (int*) sxrealloc (lter, sizofpts + 1, sizeof (int));
		toks = (struct sxtoken*) sxrealloc (toks, sizofpts + 1, sizeof (struct sxtoken));
		a_la_rigueur.correction = (int*) sxrealloc (a_la_rigueur.correction, sizofpts + 1, sizeof (int));
	    }
	}

	if (tmax < sxplocals.SXP_tables.P_tmax) {
	    tmax = sxplocals.SXP_tables.P_tmax;

	    if (vt_set != NULL) {
		vt_set = sxba_resize (vt_set, tmax);
	    }
	}

	if (nmax < sxplocals.SXP_tables.P_nmax) {
	    nmax = sxplocals.SXP_tables.P_nmax;

	    if (to_be_checked != NULL) {
		to_be_checked = sxba_resize (to_be_checked, nmax + 1);
	    }
	}

/* Quand P_Mtstate sera dans les tables
	if (Mtstate < sxplocals.SXP_tables.P_Mtstate) {
	    Mtstate = sxplocals.SXP_tables.P_Mtstate;
	}
*/

	Mtstate = 200 /* Ca vaut pas la peine de se fouler!! */;
	break;

    case ACTION:
	truncate_context = TRUE;
	return recovery ();

    case CLOSE:
	if (to_be_checked != NULL)
	    sxfree (to_be_checked), to_be_checked = NULL;

	if (stack != NULL) {
	    sxfree (vt_set), vt_set = NULL;
	    sxfree (a_la_rigueur.correction), a_la_rigueur.correction = NULL;
	    sxfree (toks), toks = NULL;
	    sxfree (lter), lter = NULL;
	    sxfree (ster), ster = NULL;
	    sxfree (com), com = NULL;
	    sxfree (modified_set), modified_set = NULL;
	    sxfree (stack), stack = NULL;
	}

	break;

    default:
	fprintf (sxstderr, "The function \"sxprecovery\" is out of date with respect to its specification.\n");
	abort ();
    }

    return TRUE;
}
