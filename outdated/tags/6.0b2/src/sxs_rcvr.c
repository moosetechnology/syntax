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
   *                (DP d'apres PB)			  *
   *                                                      *
   ******************************************************** */

/*************************************************************/
/*                                                           */
/*                                                           */
/*  This program has been translated from scanner.pl1        */
/*  on Friday the tenth of January, 1986, at 15:16:18,       */
/*  on the Multics system at INRIA,                          */
/*  by the release 3.3f PL1_C translator of INRIA,           */
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
/* 03-11-92 12:06 (pb):		Bug ds "check" si "is_la"		*/
/************************************************************************/
/* 30-03-92 13:45 (pb):		Utilisation de sxsvar.sxlv.mode		*/
/************************************************************************/
/* 19-07-90 11:42 (pb):		Bug sur la raz de nmax (detecte par 	*/
/*				Xavier Pandolfi LIFIA-IMAG)		*/
/************************************************************************/
/* 03-05-88 16:06 (pb):		Bug si sxsvar.SXS_tables.S_nmax == 0	*/
/************************************************************************/
/* 22-12-87 17:42 (phd):	Adaptation aux bits arrays		*/
/************************************************************************/
/* 09-09-87 15:52 (pb) :	Test de S_no_delete simplifie		*/
/*				Suppression de to_be_checked		*/
/************************************************************************/
/* 25-05-87 11:03 (phd) :	Ajout d'un message d'erreur		*/
/************************************************************************/
/* 05-05-87 17:36 (pb&phd) :	Ajout de la classe de previous_char en	*/
/*				source_classes [-1].			*/
/************************************************************************/
/* 16-04-87 13:45 (pb) :	Refonte totale pour les nouveaux codops	*/
/************************************************************************/
/* 26-03-87 11:02 (pb) :	Ajout du parametre what	        	*/
/************************************************************************/
/* 30-01-87 14:39 (pb) :	Apres lecture de EOF le srcmngr n'est 	*/
/*				plus appele				*/
/************************************************************************/
/* 16-01-87 09:45 (pb) :	Prise en compte du nouveau sxsrcmngr	*/
/************************************************************************/
/* 18-12-86 11:11 (phd) :	Corrections et ameliorations diverses	*/
/************************************************************************/
/* 16-11-86 15:00 (pb) :	Separation du traitement des erreurs	*/
/************************************************************************/
/* 23-10-86 15:00 (pb) :	Introduction de current_class et	*/
/*				current_class_in_la (register)	  	*/
/************************************************************************/
/* 14-10-86 12:32 (phd) :	Suppression des points d'entree qui	*/
/*				deviennent des macros dans SXUNIX.H	*/
/************************************************************************/
/* 09-10-86 14:26 (dp) :	Correction de deux erreurs dans le	*/
/*				rattrapage d'erreurs.			*/
/************************************************************************/
/* 07-10-86 18:00 (phd & pb) :	Correction d'une erreur dans le		*/
/*				rattrapage d'erreurs, due a la meme	*/
/*				erreur sur Multics.  Nous en avons	*/
/*				profite pour supprimer quelques		*/
/*				variables et fonctions.			*/
/************************************************************************/
/* 07-10-86 12:00 (phd & pb) :	Les caracteres sont compris entre -128	*/
/*				et 255; S_char_to_simple_class [EOF]	*/
/*				est positionne a 2, dynamiquement...	*/
/************************************************************************/
/* 27-05-86 16:00 (dp) :	Register			 	*/
/************************************************************************/
/* 22-05-86 12:00 (dp) :	Sur EOF d'include: pas de commentaire 	*/
/************************************************************************/
/* 20-05-86 13:00 (dp) :	Ajout de sxccc() et source_class 	*/
/*				Suppression de source_rank		*/
/************************************************************************/
/* 16-05-86 17:20 (phd):	Suppression de la variable "x_act" et	*/
/*				des appels redondants aux actions	*/
/************************************************************************/
/* 16-05-86 17:18 (phd):	Ajout du "copyright"			*/
/************************************************************************/
/* 16-05-86 17:18 (phd):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/



#include "sxunix.h"
#include "sxsstmt.h"

#ifndef VARIANT_32
char WHAT_SXS_RECOVERY[] = "@(#)SYNTAX - $Id: sxs_rcvr.c 617 2007-06-11 08:24:34Z rlacroix $" WHAT_DEBUG;
#else
char WHAT_SXS_RECOVERY[] = "@(#)SYNTAX - $Id: sxs_rcvr.c 617 2007-06-11 08:24:34Z rlacroix $ SXS_RCVR_32" WHAT_DEBUG;
#endif
/* Tables pour la correction erreur de taille max de tous les langages */

static int	*source_classes, *ranks;
static SHORT	*source_char;
static SXBA	insertable_valid_class_set, non_insertable_valid_class_set;
static int	nmax = 0, last_simple_class_no = 0;




#define char_to_class(c) (sxsvar.SXS_tables.S_char_to_simple_class[c])


static VOID	allouer_correction ()
{
    if (nmax == 0) {
	if ((nmax = sxsvar.SXS_tables.S_nmax) == 0)
	    nmax = 1 /* On lit toujours le caractere suivant */;

	source_char = (SHORT*) sxalloc (nmax + 1, sizeof (SHORT));
	source_classes = (int*) sxalloc (nmax + 2, sizeof (int)) + 1;
    }
    else if (nmax < sxsvar.SXS_tables.S_nmax) {
	nmax = sxsvar.SXS_tables.S_nmax;
	source_char = (SHORT*) sxrealloc (source_char, nmax + 1, sizeof (SHORT));
	source_classes = (int*) sxrealloc (source_classes - 1, nmax + 2, sizeof (int)) + 1;
    }

    if (last_simple_class_no == 0) {
	last_simple_class_no = sxsvar.SXS_tables.S_last_simple_class_no;
	ranks = (int*) sxalloc (last_simple_class_no + 1, sizeof (int));
	insertable_valid_class_set = sxba_calloc (last_simple_class_no);
	non_insertable_valid_class_set = sxba_calloc (last_simple_class_no);
    }
    else if (last_simple_class_no < sxsvar.SXS_tables.S_last_simple_class_no) {
	last_simple_class_no = sxsvar.SXS_tables.S_last_simple_class_no;
	ranks = (int*) sxrealloc (ranks, last_simple_class_no + 1, sizeof (int));
	insertable_valid_class_set = sxba_resize (insertable_valid_class_set, last_simple_class_no);
	non_insertable_valid_class_set = sxba_resize (non_insertable_valid_class_set, last_simple_class_no);
    }
}




/* *********** */
/* CORRECTION  */
/* *********** */

static BOOLEAN	is_insertable (class_no, rank)
    int		class_no, *rank;
{
    /* cherche s'il existe un caractere de classe class_no n'appartenant pas a
       S_dont_insert et rend son code interne dans rank */

    register int	i;

    for (i = 0; i <= sxsvar.SXS_tables.S_last_char_code; i++) {
	if (class_no == char_to_class (i) && !sxgetbit (sxsvar.SXS_tables.S_no_insert, *rank = i))
		return TRUE;
    }

    return FALSE;
}



static BOOLEAN	check (state_no, class_of_X, model, is_la)
    int		state_no, class_of_X;
    int 	*model;
    BOOLEAN	is_la;
{
    /* verifie si le sous_modele model_no debutant a l'index index_no est
       compatible avec la sous chaine correspondante de source_classes ou
       state_no est l'etat initial */

    /* Si l'erreur a ete detectee en look-ahead (is_la == TRUE) on ne peut
       verifier le modele qu'en look-ahead (toute instruction ramenant en
       scan normal ramene en fait avant la detection de l'erreur..) */

    SXSTMI	stmt;
    register struct SXS_action_or_prdct_code	*action_or_prdct_code;
    register int	xm, xm_la, xs, class, codop, next, firstlookaheadclass;
    SHORT	sxchar;
    BOOLEAN	is_satisfied, is_in_tm, char_checked;

    sxinitialise(action_or_prdct_code); /* pour faire taire "gcc -Wuninitialized" */
    sxinitialise(firstlookaheadclass); /* pour faire taire "gcc -Wuninitialized" */
    next = state_no;
    is_in_tm = TRUE;
    xm_la = 0;

    for (xm = 1; xm <= model [0]; xm++) {
	xs = model [xm];
	class = (xs < 0 /* X */ ) ? class_of_X : source_classes [xs];
	char_checked = FALSE;

	while (!char_checked) {
	    if (is_in_tm) {
		stmt = sxsvar.SXS_tables.S_transition_matrix [next] [class];
	    }
	    else {
		action_or_prdct_code = &(sxsvar.SXS_tables.SXS_action_or_prdct_code [next]);
		stmt = action_or_prdct_code->stmt;
	    }

	    codop = CODOP (stmt);
	    is_satisfied = TRUE;

	    if (!is_in_tm && action_or_prdct_code->kind == IsPredicate) {
		is_satisfied = FALSE;


/* Probleme de l'execution des predicats lors du rattrapage
	       d'erreur: delicat meme pour certains predicats systeme (&Is_Set
	       et &Is_Reset car les actions correspondantes ne sont pas
	       executees), on peut boucler... Par prudence, on leur fait
	       rendre "FALSE", on peut donc rater des modeles valides. */

		if (action_or_prdct_code->is_system) {
		    switch (action_or_prdct_code->action_or_prdct_no) {
		    case IsTrue:
			is_satisfied = TRUE;
			break;

		    case NotIsFirstCol:
		    case IsFirstCol:
			if (xm == 1) {
			    /* on teste si le caractere courant est en
			       premiere colonne */
			    class = source_classes [-1];
			}
			else {
			    class = (xs = model [xm - 1]) < 0 /* X */  ? class_of_X : source_classes [xs];
			}

			is_satisfied = (class == char_to_class (NEWLINE));

			if (action_or_prdct_code->action_or_prdct_no == 4)
			    is_satisfied = !is_satisfied;

			break;

		    case IsSet:
		    case IsReset:
			is_satisfied = FALSE;
			/* prudence */
			break;

		    case IsLastCol:
		    case NotIsLastCol:
			if (xm == model [0] /* pointe apres le modele */ ) {
			    /* recuperer le caractere suivant en look_ahead */
			    sxchar = sxlanext_char ();
			    sxlaback (1);
			}
			else
			    sxchar = source_char [model [xm + 1]];

			is_satisfied = (sxchar == EOF || sxchar == NEWLINE);

			if (action_or_prdct_code->action_or_prdct_no == 6)
			    is_satisfied = !is_satisfied;
			break;
		    default: /* pour faire taire gcc -Wswitch-default */
#if EBUG
		      sxtrap("sxs_rcvr","unknown switch case #1");
#endif
		      break;
		    }
		}
		else
		    is_satisfied = FALSE;


/* prudence */

		if (!is_satisfied) {
		    next++;
		}
	    }

	    if (is_satisfied) {
		if (SCAN (stmt)) {
		    char_checked = TRUE;
		}

		next = NEXT (stmt);
		is_in_tm = TRUE;

		switch (codop) {
		case Error:
		    return FALSE;

		case Reduce:
		case HashReduce:
		case ReducePost:
		case HashReducePost:
		    if (is_la)
			return FALSE;

		    if (next == sxsvar.SXS_tables.S_termax) {
			char_checked = TRUE;
		    }

		    next = 1;

		    if (xm_la > 0) {
			/* Sortie d'un look-ahead local */
			class = firstlookaheadclass;
			xm_la = 0;
		    }

		    break;

		case ActPrdct:
		    is_in_tm = FALSE;
		    break;

		case FirstLookAhead:
		    firstlookaheadclass = class;
		    xm_la = xm;

		case NextLookAhead:
		    if (is_la) {
			char_checked = TRUE;
		    }
		    else {
			if (++xm_la > model [0])
			    return TRUE;
			
			xs = model [xm_la];
			class = (xs < 0 /* X */ ) ? class_of_X : source_classes [xs];
		    }

		    break;

		case SameState:
		case State:
		    if (is_la)
			return FALSE;

		    if (xm_la > 0) {
			/* Sortie d'un look-ahead local */
			class = firstlookaheadclass;
			xm_la = 0;
		    }

		    break;
		default: /* pour faire taire gcc -Wswitch-default */
#if EBUG
		  sxtrap("sxs_rcvr","unknown switch case #2");
#endif
		  break;
		}
	    }
	}
    }

    return TRUE;
}



static BOOLEAN	is_valid (state_no, current, is_validated, is_la)
    int		state_no;
    BOOLEAN	*is_validated, is_la;
    struct SXS_correction_item	*current;
{
    /* Verifie si le source valide le modele current. Si is_validated est
       faux, cette validation a necessite l'insertion d'un caractere de
       dont_insert */

    register int	i, x;
    SXSTMI	*tm_line, stmt;
    register SXBA	to_be_processed;

    if (current->model [1] < 0) {

	if (current->model [2] == 1 /* remplacement */ && source_char [0] == EOF /* de EOF */)
	    return FALSE;

	sxba_empty (non_insertable_valid_class_set);
	sxba_empty (insertable_valid_class_set);
	tm_line = sxsvar.SXS_tables.S_transition_matrix [state_no];

	for (i = 3; i <= sxsvar.SXS_tables.S_last_simple_class_no; i++) {
	    /* On saute illegal_chars et EOF */

	    if (CODOP (tm_line [i]) != Error) {
		 sxba_1_bit (is_insertable (i, &(ranks [i])) ? insertable_valid_class_set : non_insertable_valid_class_set, i - 1);
	    }
	}

	for (i = 1; i <= 2; i++) {
	    to_be_processed = i == 1 ? insertable_valid_class_set : non_insertable_valid_class_set;
	    current->simple_class = 0;

	    while ((current->simple_class = sxba_scan (to_be_processed, current->simple_class - 1) + 1) != 0) {
		stmt = tm_line [x = current->simple_class];

		if (x <= sxsvar.SXS_tables.S_last_simple_class_no) {
		    x = -1;

		    while ((x = sxba_scan (to_be_processed, x)) >= 0) {
			if (stmt == tm_line [x + 1])
			    sxba_0_bit (to_be_processed, x);
		    }
		}

		if (check (state_no, current->simple_class, current->model, is_la)) {
		    *is_validated = (i == 1);
		    current->rank_of_char = ranks [current->simple_class];
		    return TRUE;
		}
	    }
	}
    }
    else {

	if (source_char [0] == EOF /* Suppression de EOF */)
	    return FALSE;

	*is_validated = TRUE;
	return check (state_no, 0, current->model, is_la);
    }

    return FALSE;
}

static struct SXS_correction_item empty_SXS_correction_item;

static VOID	tryacorr (state_no, is_la, best)
    int		state_no;
    BOOLEAN	is_la;
    struct SXS_correction_item	*best;
{
    BOOLEAN	is_validated;
    struct SXS_correction_item	a_la_rigueur, current;

    a_la_rigueur = current = empty_SXS_correction_item; /* Le 13/04/07 */

    for (current.model_no = 1; current.model_no <= sxsvar.SXS_tables.S_nbcart; current.model_no++) {
	current.model = &(sxsvar.SXS_tables.S_lregle [current.model_no] [0]);
	is_validated = TRUE;

	if (is_valid (state_no, &current, &is_validated, is_la)) {
	    if (is_validated &&
		current.model [2] != 0 /* le caractere en erreur est detruit */ &&
		source_classes [0] > 2 &&
		sxgetbit (sxsvar.SXS_tables.S_no_delete, source_char [0]))
			is_validated = FALSE;

	    if (is_validated) {
		*best = current;
		return;
	    }

	    if (a_la_rigueur.model_no == 0)
		a_la_rigueur = current;
	}
    }

    *best = a_la_rigueur;
}



static BOOLEAN	recovery (state_no, class)
    int		state_no;
    unsigned char	*class;
{
    /* Une correction impliquant la suppression d'un caractere dont la classe
       simple est dans S_dont_delete ne peut etre validee que s'il n'existe
       pas de modele (meme moins prioritaire) valide. Une correction
       impliquant l'insertion d'un caractere dont la classe simple est dans
       S_dont_insert ne peut etre validee que s'il n'existe pas de modele
       (meme moins prioritaire) valide. */

    /* Si l'erreur est detectee en look_ahead, si le look_ahead se termine
       lors de la correction, on revient alors traiter des symboles situes
       dans le source avant le point de detection de l'erreur (a une distance
       peut etre non bornee) => dur dur. Pour simplifier, dans ce cas, on se
       contente de verifier des prefixes de modeles. On arrete "check" lors du
       retour en traitement normal. */

    register int	i, j;
    register BOOLEAN	is_error_in_la;
    struct SXS_correction_item	best;

    allouer_correction ();

    if ((is_error_in_la = sxsvar.SXS_tables.S_transition_matrix [state_no] [1])) {
	source_classes [-1] = char_to_class (sxsrcmngr.buffer [sxsrcmngr.labufindex - 1]);
	source_char [0] = sxsrcmngr.buffer [sxsrcmngr.labufindex];
	source_char [1] = (source_char [0] == EOF) ? EOF : sxlanext_char ();
    }
    else {
	source_char [0] = sxsrcmngr.current_char;
	source_char [1] = (source_char [0] == EOF) ? EOF : sxlafirst_char ();
	source_classes [-1] = char_to_class (sxsrcmngr.previous_char);
    }

    source_classes [0] = char_to_class (source_char [0]);
    source_classes [1] = char_to_class (source_char [1]);


/* lecture des caracteres en look_ahead */

    for (i = 2; i <= sxsvar.SXS_tables.S_nmax; i++) {
	source_classes [i] = char_to_class (source_char [i] = (source_char [i - 1] == EOF) ? EOF : sxlanext_char ());
    }

    tryacorr (state_no, is_error_in_la, &best);


/* ecriture des messages d'erreur */

    if (best.model_no == 0 && source_char [0] == EOF) {
	    if (!sxsvar.sxlv.mode.is_silent)
		sxput_error (sxsrcmngr.source_coord,
			     sxsvar.SXS_tables.S_global_mess [5],
			     sxsvar.sxtables->err_titles [2]);

	    sxlaback (sxsvar.SXS_tables.S_nmax);
	    return FALSE;
	}

    if (!is_error_in_la && !sxsvar.sxlv.mode.is_silent) {
	/* no error message in look_ahead */
	register SHORT	sxchar;

	if (best.model_no == 0) {
	    /* No Correction => Recovery */
	    sxchar = source_char [0];
	    sxput_error (sxsrcmngr.source_coord,
			 sxsvar.SXS_tables.S_global_mess [1],
			 sxsvar.sxtables->err_titles [2],
			 (sxchar == EOF ?
			  sxsvar.SXS_tables.S_global_mess [4] :
			  (sxchar == NEWLINE ?
			   sxsvar.SXS_tables.S_global_mess [3] :
			   CHAR_TO_STRING (sxchar))));
	}
	else {
	    char	*msg_params [5];

	    for (j = sxsvar.SXS_tables.SXS_local_mess [best.model_no].S_param_no - 1; j >= 0; j--) {
		i = sxsvar.SXS_tables.SXS_local_mess [best.model_no].S_param_ref [j];
		sxchar = i < 0 /* X */	? best.rank_of_char : source_char [i];
		msg_params [j] = (sxchar == EOF ? sxsvar.SXS_tables.S_global_mess [4] : (sxchar == NEWLINE ? sxsvar.
		     SXS_tables.S_global_mess [3] : CHAR_TO_STRING (sxchar)));
	    }

	    sxput_error (sxsrcmngr.source_coord,
			 sxsvar.SXS_tables.SXS_local_mess [best.model_no].S_string,
			 sxsvar.sxtables->err_titles [1],
			 msg_params [0],
			 msg_params [1],
			 msg_params [2],
			 msg_params [3],
			 msg_params [4]);
	}
    }

    if (best.model_no == 0) {

/* suppression du caractere courant */

	if (!is_error_in_la) {
	    sxsrcmngr.current_char = sxsrcmngr.previous_char;
	    sxnext_char ();
	}
	else
	    sxlaback (sxsvar.SXS_tables.S_nmax - 1);

	*class = source_classes [1];
    }
    else if (best.model [1] == -1) {
	/* les seuls modeles corrects sont de la forme : X n n+1 n+2 .... ou
	   n>=0, p p+1 p+2 .... ou p>=1  */
	/* best.model [2] = 0 => ajout d'un caractere avant le courant, sinon
	   remplacement du caractere */

	if (best.model [2] == 0) {
	    if (!is_error_in_la)
		sxX (best.rank_of_char);
	    else
		sxlaback (sxsvar.SXS_tables.S_nmax + 1);
	}
	else if (!is_error_in_la)
		sxsrcmngr.current_char = best.rank_of_char;
	    else
		sxlaback (sxsvar.SXS_tables.S_nmax);

	*class = best.simple_class;
    }
    else {
	if (!is_error_in_la) {
	    sxsrcmngr.current_char = sxsrcmngr.previous_char;
	    sxnext_char ();
	}
	else
	    sxlaback (sxsvar.SXS_tables.S_nmax - 1);

	*class = source_classes [1];
    }

    return TRUE;
}



static VOID	recovery_free ()
{
    if (source_char != NULL) {
	nmax = last_simple_class_no = 0;
	sxfree (non_insertable_valid_class_set), non_insertable_valid_class_set = NULL;
	sxfree (insertable_valid_class_set), insertable_valid_class_set = NULL;
	sxfree (ranks), ranks = NULL;
	sxfree (source_classes - 1), source_classes = NULL;
	sxfree (source_char), source_char = NULL;
    }
}



BOOLEAN		sxsrecovery (sxsrecovery_what, state_no, class)
    int		sxsrecovery_what, state_no;
    unsigned char	*class;
{
    switch (sxsrecovery_what) {
    case ACTION:
	return recovery (state_no, class);

    case CLOSE:
	recovery_free ();
	break;

    default:
	fprintf (sxstderr, "The function \"sxsrecovery\" is out of date with respect to its specification.\n");
	abort ();
    }

    return TRUE;
}
