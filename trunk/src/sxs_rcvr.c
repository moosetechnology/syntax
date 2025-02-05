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

#include "sxversion.h"
#include "sxunix.h"
#include "sxsstmt.h"

#ifndef SXVERSION
/* if SXVERSION is not defined, then this file is being compiled by an external program,
   and WHAT_DEBUG is also not defined
*/
#define WHAT_DEBUG
#endif /* defined(SXVERSION) */

#ifndef VARIANT_32
char WHAT_SXS_RECOVERY[] = "@(#)SYNTAX - $Id: sxs_rcvr.c 4089 2024-06-21 12:41:17Z garavel $" WHAT_DEBUG;
#endif

/*
 * Note: a simpler version of this module once existed. It performed a
 * simplified form of scanner recovery, by deleting the incorrect character. 
 * The source code of this module can be found in 
 * outdated/deleted/src/sxs_srcvr.c 
 */

/* Tables pour la correction erreur de taille max de tous les langages */

static SXINT	*source_classes, *ranks;
static short	*source_char;
static SXBA	insertable_valid_class_set, non_insertable_valid_class_set;
static SXINT	nmax = 0, last_simple_class_no = 0;




#define char_to_class(c) (sxsvar.SXS_tables.S_char_to_simple_class[c])


static void	allouer_correction (void)
{
    if (nmax == 0) {
	if ((nmax = sxsvar.SXS_tables.S_nmax) == 0)
	    nmax = 1 /* On lit toujours le caractere suivant */;

	source_char = (short*) sxalloc (nmax + 1, sizeof (short));
	source_classes = (SXINT*) sxalloc (nmax + 2, sizeof (SXINT)) + 1;
    }
    else if (nmax < sxsvar.SXS_tables.S_nmax) {
	nmax = sxsvar.SXS_tables.S_nmax;
	source_char = (short*) sxrealloc (source_char, nmax + 1, sizeof (short));
	source_classes = (SXINT*) sxrealloc (source_classes - 1, nmax + 2, sizeof (SXINT)) + 1;
    }

    if (last_simple_class_no == 0) {
	last_simple_class_no = sxsvar.SXS_tables.S_last_simple_class_no;
	ranks = (SXINT*) sxalloc (last_simple_class_no + 1, sizeof (SXINT));
	insertable_valid_class_set = sxba_calloc (last_simple_class_no);
	non_insertable_valid_class_set = sxba_calloc (last_simple_class_no);
    }
    else if (last_simple_class_no < sxsvar.SXS_tables.S_last_simple_class_no) {
	last_simple_class_no = sxsvar.SXS_tables.S_last_simple_class_no;
	ranks = (SXINT*) sxrealloc (ranks, last_simple_class_no + 1, sizeof (SXINT));
	insertable_valid_class_set = sxba_resize (insertable_valid_class_set, last_simple_class_no);
	non_insertable_valid_class_set = sxba_resize (non_insertable_valid_class_set, last_simple_class_no);
    }
}




/* *********** */
/* CORRECTION  */
/* *********** */

static bool	is_insertable (SXINT class_no, SXINT *rank)
{
    /* cherche s'il existe un caractere de classe class_no n'appartenant pas a
       S_dont_insert et rend son code interne dans rank */

    SXINT	i;

    for (i = 0; i <= sxsvar.SXS_tables.S_last_char_code; i++) {
	if (class_no == char_to_class (i) && !sxgetbit (sxsvar.SXS_tables.S_no_insert, *rank = i))
		return true;
    }

    return false;
}



static bool	check (SXINT state_no, SXINT class_of_X, SXINT *model, bool is_la)
{
    /* verifie si le sous_modele model_no debutant a l'index index_no est
       compatible avec la sous chaine correspondante de source_classes ou
       state_no est l'etat initial */

    /* Si l'erreur a ete detectee en look-ahead (is_la == true) on ne peut
       verifier le modele qu'en look-ahead (toute instruction ramenant en
       scan normal ramene en fait avant la detection de l'erreur..) */

    SXSTMI	stmt;
    struct SXS_action_or_prdct_code	*action_or_prdct_code;
    SXINT	xm, xm_la, xs, class, codop, next, firstlookaheadclass;
    short	sxchar;
    bool	is_satisfied, is_in_tm, char_checked;

    sxinitialise(action_or_prdct_code); /* pour faire taire "gcc -Wuninitialized" */
    sxinitialise(firstlookaheadclass); /* pour faire taire "gcc -Wuninitialized" */
    next = state_no;
    is_in_tm = true;
    xm_la = 0;

    for (xm = 1; xm <= model [0]; xm++) {
	xs = model [xm];
	class = (xs < 0 /* X */ ) ? class_of_X : source_classes [xs];
	char_checked = false;

	while (!char_checked) {
	    if (is_in_tm) {
		stmt = sxsvar.SXS_tables.S_transition_matrix [next] [class];
	    }
	    else {
		action_or_prdct_code = &(sxsvar.SXS_tables.SXS_action_or_prdct_code [next]);
		stmt = action_or_prdct_code->stmt;
	    }

	    codop = CODOP (stmt);
	    is_satisfied = true;

	    if (!is_in_tm && action_or_prdct_code->kind == IsPredicate) {
		is_satisfied = false;


/* Probleme de l'execution des predicats lors du rattrapage
	       d'erreur: delicat meme pour certains predicats systeme (&Is_Set
	       et &Is_Reset car les actions correspondantes ne sont pas
	       executees), on peut boucler... Par prudence, on leur fait
	       rendre "false", on peut donc rater des modeles valides. */

		if (action_or_prdct_code->is_system) {
		    switch (action_or_prdct_code->action_or_prdct_no) {
		    case IsTrue:
			is_satisfied = true;
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

			is_satisfied = (class == char_to_class (SXNEWLINE));

			if (action_or_prdct_code->action_or_prdct_no == 4)
			    is_satisfied = !is_satisfied;

			break;

		    case IsSet:
		    case IsReset:
			is_satisfied = false;
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

			is_satisfied = (sxchar == EOF || sxchar == SXNEWLINE);

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
		    is_satisfied = false;


/* prudence */

		if (!is_satisfied) {
		    next++;
		}
	    }

	    if (is_satisfied) {
		if (SCAN (stmt)) {
		    char_checked = true;
		}

		next = NEXT (stmt);
		is_in_tm = true;

		switch (codop) {
		case Error:
		    return false;

		case Reduce:
		case HashReduce:
		case ReducePost:
		case HashReducePost:
		    if (is_la)
			return false;

		    if (next == sxsvar.SXS_tables.S_termax) {
			char_checked = true;
		    }

		    next = 1;

		    if (xm_la > 0) {
			/* Sortie d'un look-ahead local */
			class = firstlookaheadclass;
			xm_la = 0;
		    }

		    break;

		case ActPrdct:
		    is_in_tm = false;
		    break;

		case FirstLookAhead:
		    firstlookaheadclass = class;
		    xm_la = xm;
#if defined (__GNUC__) && (__GNUC__ - 0 >= 7)
		    /* absence de "break" (probablement volontaire) */
		    __attribute__ ((fallthrough));
#endif

		case NextLookAhead:
		    if (is_la) {
			char_checked = true;
		    }
		    else {
			if (++xm_la > model [0])
			    return true;
			
			xs = model [xm_la];
			class = (xs < 0 /* X */ ) ? class_of_X : source_classes [xs];
		    }

		    break;

		case SameState:
		case State:
		    if (is_la)
			return false;

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

    return true;
}



static bool	is_valid (SXINT state_no, struct SXS_correction_item *current, bool *is_validated, bool is_la)
{
    /* Verifie si le source valide le modele current. Si is_validated est
       faux, cette validation a necessite l'insertion d'un caractere de
       dont_insert */

    SXINT	i, x;
    SXSTMI	*tm_line, stmt;
    SXBA	to_be_processed;

    if (current->model [1] < 0) {

	if (current->model [2] == 1 /* remplacement */ && source_char [0] == EOF /* de EOF */)
	    return false;

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

		if (check (state_no, (SXINT)current->simple_class, current->model, is_la)) {
		    *is_validated = (i == 1);
		    current->rank_of_char = ranks [current->simple_class];
		    return true;
		}
	    }
	}
    }
    else {

	if (source_char [0] == EOF /* Suppression de EOF */)
	    return false;

	*is_validated = true;
	return check (state_no, (SXINT)0, current->model, is_la);
    }

    return false;
}

static struct SXS_correction_item empty_SXS_correction_item;

static void	tryacorr (SXINT state_no, bool is_la, struct SXS_correction_item *best)
{
    bool	is_validated;
    struct SXS_correction_item	a_la_rigueur, current;

    a_la_rigueur = current = empty_SXS_correction_item; /* Le 13/04/07 */

    for (current.model_no = 1; current.model_no <= sxsvar.SXS_tables.S_nbcart; current.model_no++) {
	current.model = &(sxsvar.SXS_tables.S_lregle [current.model_no] [0]);
	is_validated = true;

	if (is_valid (state_no, &current, &is_validated, is_la)) {
	    if (is_validated &&
		current.model [2] != 0 /* le caractere en erreur est detruit */ &&
		source_classes [0] > 2 &&
		sxgetbit (sxsvar.SXS_tables.S_no_delete, source_char [0]))
			is_validated = false;

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



static bool	recovery (SXINT state_no, unsigned char *class)
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

    SXINT	i, j;
    bool	is_error_in_la;
    short	lanext_char;
    struct SXS_correction_item	best;

    is_error_in_la = sxsvar.SXS_tables.S_transition_matrix [state_no] [1];

    /* Modif du 22/09/08 */
    /* Gros probleme :
       Soit une erreur est de'tecte'e en look-ahead qui se corrige d'une certaine fac,on :
          - on ne sort pas de message
	  - la correction doit se repercuter dans le buffer du source manager pour
	    que l'analyse en look-ahead puisse se continuer (e'ventuellement detection/correction
	    d'une autre erreur + loin).
          - En revanche une fois que l'action decidee par le look-ahead a ete choisie, on
	    revient en analyse normale qui doit alors passer sur le source normal (non corrige')
	    et doit alors produire une correction (on espere que c'est la meme) et le message
	    correspondant

       Il faudrait donc maintenir 2 buffers de look-ahead un utilise' par l'analyse normale
       et qui ne contient pas les corrections d'erreur faites en look-ahead
       et un buffer de look-ahead qui les contiendrait et qui serait utilise en look-ahead
       apres une correction.  Avec tous les problemes de remplissage et de switch de l'un a l'autre.

       On va faire un truc simpliste.  On fait comme ds srcvr
    */

    if (is_error_in_la) {
      /* On fait comme s le cas srcvr */

      if (sxsrcmngr.current_char == EOF) {
	*class = 2;
	return false;
      }
      else {
	lanext_char = sxlanext_char ();
	*class = char_to_class (lanext_char);
	if (lanext_char == EOF) {
	  return false;
	}
	else {
	  return true;
	}
      }
    }

    allouer_correction ();

    if (is_error_in_la) {
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
		sxerror (sxsrcmngr.source_coord,
			 sxsvar.sxtables->err_titles [2][0],
			 sxsvar.SXS_tables.S_global_mess [5],
			 sxsvar.sxtables->err_titles [2]+1);

	    sxlaback (sxsvar.SXS_tables.S_nmax);
	    return false;
	}

    if (!is_error_in_la && !sxsvar.sxlv.mode.is_silent) {
	/* no error message in look_ahead */
	short	sxchar;

	if (best.model_no == 0) {
	    /* No Correction => Recovery */
	    sxchar = source_char [0];
	    sxerror (sxsrcmngr.source_coord,
		     sxsvar.sxtables->err_titles [2][0],
		     sxsvar.SXS_tables.S_global_mess [1],
		     sxsvar.sxtables->err_titles [2]+1,
		     (sxchar == EOF ?
		      sxsvar.SXS_tables.S_global_mess [4] :
		      (sxchar == SXNEWLINE ?
		       sxsvar.SXS_tables.S_global_mess [3] :
		       SXCHAR_TO_STRING (sxchar))));
	}
	else {
	    char	*msg_params [5];

	    for (j = sxsvar.SXS_tables.SXS_local_mess [best.model_no].S_param_no - 1; j >= 0; j--) {
		i = sxsvar.SXS_tables.SXS_local_mess [best.model_no].S_param_ref [j];
		sxchar = i < 0 /* X */	? best.rank_of_char : source_char [i];
		msg_params [j] = (sxchar == EOF ? sxsvar.SXS_tables.S_global_mess [4] : (sxchar == SXNEWLINE ? sxsvar.
		     SXS_tables.S_global_mess [3] : SXCHAR_TO_STRING (sxchar)));
	    }

	    sxerror (sxsrcmngr.source_coord,
		     sxsvar.sxtables->err_titles [1][0],
		     sxsvar.SXS_tables.SXS_local_mess [best.model_no].S_string,
		     sxsvar.sxtables->err_titles [1]+1,
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

    return true;
}



static void	recovery_free (void)
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



bool		sxsrecovery (SXINT what, SXINT state_no, unsigned char *class)
{
    switch (what) {
    case SXACTION:
	return recovery (state_no, class);

    case SXCLOSE:
	recovery_free ();
	break;

    default:
	fprintf (sxstderr, "The function \"sxsrecovery\" is out of date with respect to its specification.\n");
	sxexit(1);
    }

    return true;
}
