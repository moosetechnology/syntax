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
#include "sxcommon.h"

#ifndef VARIANT_32
char WHAT_SXNDSCANNER[] = "@(#)SYNTAX - $Id: sxndscanner.c 4166 2024-08-19 09:00:49Z garavel $" WHAT_DEBUG;
#else
char WHAT_SXNDSCANNER32[] = "@(#)SYNTAX - $Id: sxndscanner.c 4166 2024-08-19 09:00:49Z garavel $ SXNDSCANNER_32" WHAT_DEBUG;
#endif

/*
  Principe: Les predicats doivent pouvoir s'executer tout le temps (analyse
  normale, erreur, look-ahead), l'utilisateur etant prevenu de ce mode.
  La consequence est que les actions dont ils peuvent dependre doivent etre
  dans tous les cas executees avant.
  Une action pour s'executer ne doit pas avoir besoin de look-ahead
  (apres une action utilisateur, le ctxt peut ne pas etre celui que l'on
  croit, l'action ayant pu lire du texte source)
  Dans le cas general, une action forme donc une barriere impermeable a la
  connaissance de son contexte droit.
  Si une E.R. commence par une action, les E.R. qui peuvent la preceder doivent
  pouvoir se reconnaitre sans look-ahead
  On veut de plus que si une action @1 precede un predicat &1 (Ex @1 t &1),
  &1 peut dependre de @1.
  Pour y parvenir si un conflit implique (meme par fermeture) une action
  le resoudre par du non-determinisme.
  Les actions en look-ahead (meme celles qui ne demandent pas de contexte)
  vont donc produire la "resolution" du conflit initial par non-determinisme.
*/

static char	ME [] = "NDSCANNER";

#include <ctype.h>
#include <memory.h>
#include "sxsstmt.h"
#include "sxnd.h"



#if	BUG
#	undef	sxnextchar
#	define	sxnextchar sxnext_char
#endif


static void push_include (SXINT include_action)
{
    struct ndincl_elem *top;
    
    top = (struct ndincl_elem*) sxalloc (1, sizeof (struct ndincl_elem));
    top->prev =  sxndsvar.top_incl;
    top->include_action = include_action;
    sxndsvar.top_incl = top;
    sxba_copy (top->ndlsets [0] = sxba_calloc (sxndsvar.ndlv_size), sxndsvar.active_ndlv_set);
    sxba_copy (top->ndlsets [1] = sxba_calloc (sxndsvar.ndlv_size), sxndsvar.reduce_ndlv_set);
    sxba_copy (top->ndlsets [2] = sxba_calloc (sxndsvar.ndlv_size), sxndsvar.read_char_ndlv_set);
    sxba_empty (sxndsvar.active_ndlv_set);
    sxba_empty (sxndsvar.reduce_ndlv_set);
    sxba_empty (sxndsvar.read_char_ndlv_set);
}


static SXINT pop_include (void)
{
    SXINT			include_action;
    struct ndincl_elem	*top;
    
    top = sxndsvar.top_incl;
    sxndsvar.top_incl = top->prev;
    top->prev =  sxndsvar.top_incl;
    include_action = top->include_action;

    sxba_copy (sxndsvar.active_ndlv_set, top->ndlsets [0]);
    sxba_copy (sxndsvar.reduce_ndlv_set, top->ndlsets [1]);
    sxba_copy (sxndsvar.read_char_ndlv_set, top->ndlsets [2]);

    sxfree (top->ndlsets [0]);
    sxfree (top->ndlsets [1]);
    sxfree (top->ndlsets [2]);
    sxfree (top);

    return include_action;
}


static void	comments_put (char *str, SXINT lgth)
{
    SXINT	lgth_comment;

    if (sxndsvar.current_ndlv->terminal_token.comment == NULL)
	strcpy (sxndsvar.current_ndlv->terminal_token.comment =
		(char*) sxalloc (lgth + 1, sizeof (char)), str);
    else {
	lgth_comment = strlen (sxndsvar.current_ndlv->terminal_token.comment);
	strcpy ((sxndsvar.current_ndlv->terminal_token.comment =
		 (char*) sxrealloc (sxndsvar.current_ndlv->terminal_token.comment, lgth_comment +
	     lgth + 1, sizeof (char))) + lgth_comment, str);
    }
}

#define ts_put(c)									\
(    (sxndsvar.current_ndlv->ts_lgth+2 == sxndsvar.current_ndlv->ts_lgth_use)		\
	? sxndsvar.current_ndlv->token_string = (char *) sxrealloc (sxndsvar.current_ndlv->	\
	    token_string, sxndsvar.current_ndlv->ts_lgth_use *= 2, sizeof (char))		\
	: NULL,										\
     sxndsvar.current_ndlv->token_string [sxndsvar.current_ndlv->ts_lgth ++] = (char)c		\
)


#define ts_null()	(sxndsvar.current_ndlv->token_string [sxndsvar.current_ndlv->ts_lgth] = SXNUL)

#define char_to_class(c) (sxndsvar.SXS_tables.S_char_to_simple_class[c])


void ndlv_clear (struct sxndlv *ndlv, bool keep_comments)
{
    if (!keep_comments)
	ndlv->terminal_token.comment = NULL;

    ndlv->ts_lgth = 0;
    ndlv->terminal_token.lahead = 0;
    ndlv->terminal_token.string_table_entry = SXEMPTY_STE;
    ndlv->terminal_token.source_index = sxsrcmngr.source_coord;
    ndlv->mark.index = -1 /* no explicit Mark yet */ ;
    ndlv->previous_char = sxsrcmngr.previous_char;
    ndlv->current_state_no = 1; /* etat initial */
    ndlv->stmt = 0;
    SXBA_1_bit (sxndsvar.active_ndlv_set, ndlv->my_index);
}

static void hashreduce (SXSTMI stmt)
{
    if ((sxndsvar.current_ndlv->terminal_token.lahead =
	 (*sxndsvar.SXS_tables.S_check_keyword) (sxndsvar.current_ndlv->token_string,
					       sxndsvar.current_ndlv->ts_lgth)) == 0) {
	/* not a keyword */
	sxndsvar.current_ndlv->terminal_token.string_table_entry =
	    sxstr2save (sxndsvar.current_ndlv->token_string, sxndsvar.current_ndlv->ts_lgth);
	
	if ((sxndsvar.current_ndlv->terminal_token.lahead = NEXT (stmt)) == 0)
	    sxndsvar.current_ndlv->terminal_token.lahead = -1; /* KW Error */
    }
}




static void ndlv_oflw (sxindex_header *index_hd_ptr, SXINT old_line_nb_parameter, SXINT old_size_parameter)
{
    SXINT old_size, i;

    sxuse (old_line_nb_parameter);
    sxuse (old_size_parameter);

    sxuse(index_hd_ptr); /* pour faire taire gcc -Wunused */
    old_size = sxndsvar.ndlv_size;
    sxndsvar.ndlv_size = sxindex_size (sxndsvar.index_header);
    sxndsvar.active_ndlv_set = sxba_resize (sxndsvar.active_ndlv_set, sxndsvar.ndlv_size);
    sxndsvar.reduce_ndlv_set = sxba_resize (sxndsvar.reduce_ndlv_set, sxndsvar.ndlv_size);
    sxndsvar.read_char_ndlv_set = sxba_resize (sxndsvar.read_char_ndlv_set, sxndsvar.ndlv_size);

    if (sxndsvar.top_incl != NULL) {
	struct ndincl_elem *top = sxndsvar.top_incl;

	do {
	    top->ndlsets [0] = sxba_resize (top->ndlsets [0], sxndsvar.ndlv_size);
	    top->ndlsets [1] = sxba_resize (top->ndlsets [1], sxndsvar.ndlv_size);
	    top->ndlsets [2] = sxba_resize (top->ndlsets [2], sxndsvar.ndlv_size);
	} while ((top = top->prev) != NULL);
    }

    if (sxndsvar.rcvr.ndlv_set != NULL)
	sxndsvar.rcvr.ndlv_set = sxba_resize (sxndsvar.rcvr.ndlv_set, sxndsvar.ndlv_size);

    sxndsvar.ndlv = (struct sxndlv*) sxrealloc (sxndsvar.ndlv,
					      sxndsvar.ndlv_size,
					      sizeof (struct sxndlv));

    for (i = old_size; i < sxndsvar.ndlv_size; i++) {
	sxndsvar.ndlv [i].token_string = NULL;
	sxndsvar.ndlv [i].counters = NULL;
    } 

    if (sxndsvar.SXS_tables.S_is_user_action_or_prdct
	&& sxndsvar.mode.with_user_act)
	(void) (*sxndsvar.SXS_tables.S_scanact) (SXOFLW, sxndsvar.ndlv_size);
}

		

static void seek_active_scanner (void)
{
    SXINT i = sxindex_seek (&sxndsvar.index_header);

    sxndsvar.current_ndlv = sxndsvar.ndlv + i;

    if (sxndsvar.current_ndlv->token_string == NULL) {
	sxndsvar.current_ndlv->token_string =
	    (char*) sxalloc (sxndsvar.current_ndlv->ts_lgth_use = 120, sizeof (char));
	
	if (sxndsvar.SXS_tables.S_counters_size)
	    sxndsvar.current_ndlv->counters =
		(SXINT *) sxcalloc ((SXUINT)sxndsvar.SXS_tables.S_counters_size, sizeof (SXINT));
    }

    sxndsvar.current_ndlv->my_index = i;
    ndlv_clear (sxndsvar.current_ndlv, false);
}


SXINT clone_active_scanner (SXSTMI stmt)
{
    SXINT j, i = sxindex_seek (&sxndsvar.index_header);
    struct sxndlv *ndlv = sxndsvar.ndlv + i;

    if (ndlv->token_string == NULL) {
	ndlv->token_string =
	    (char*) sxalloc (ndlv->ts_lgth_use = sxndsvar.current_ndlv->ts_lgth_use,
			     sizeof (char));
	
	if (sxndsvar.SXS_tables.S_counters_size)
	    ndlv->counters = (SXINT *) sxalloc (sxndsvar.SXS_tables.S_counters_size, sizeof (SXINT));
    }
    else if (ndlv->ts_lgth_use <= sxndsvar.current_ndlv->ts_lgth)
	ndlv->token_string =
	    (char *) sxrealloc (ndlv->token_string,
				ndlv->ts_lgth_use = sxndsvar.current_ndlv->ts_lgth_use,
				sizeof (char));

    ndlv->my_index = i;

    if ((j = sxndsvar.SXS_tables.S_counters_size))
	while (--j >= 0)
	    ndlv->counters [j] = sxndsvar.current_ndlv->counters [j];

    memcpy (ndlv->token_string,
	     sxndsvar.current_ndlv->token_string,
	     (size_t)(sxndsvar.current_ndlv->ts_lgth));
    ndlv->ts_lgth = sxndsvar.current_ndlv->ts_lgth;
    ndlv->terminal_token = sxndsvar.current_ndlv->terminal_token;

    if (ndlv->terminal_token.comment != NULL) {
	strcpy (ndlv->terminal_token.comment =
		(char*) sxalloc (strlen (sxndsvar.current_ndlv->terminal_token.comment) + 1,
				 sizeof (char)),
		sxndsvar.current_ndlv->terminal_token.comment);
    }

    ndlv->previous_char = sxndsvar.current_ndlv->previous_char;
    ndlv->mark = sxndsvar.current_ndlv->mark;
    ndlv->current_state_no = sxndsvar.current_ndlv->current_state_no;
    ndlv->stmt = stmt;
    ndlv->milestone = sxndsvar.current_ndlv->milestone;

/*  Il faut prevenir les actions du clonage pour qu'elles puissent gerer les structures
    eventuelles associees aux scanner actifs (recopie). */

    if (sxndsvar.SXS_tables.S_is_user_action_or_prdct
	&& sxndsvar.mode.with_user_act)
	(void) (*sxndsvar.SXS_tables.S_scanact) (SXCLONE, i);

    return i;
}

SXSTMI predicate_processing (struct SXS_action_or_prdct_code *action_or_prdct_code)
{
    bool	is_prdct_satisfied = false;

    /* transition vers un groupe de predicats */
    
    action_or_prdct_code--;
    
    do {
	if ((++action_or_prdct_code)->is_system) {
	    switch (action_or_prdct_code->action_or_prdct_no) {
	    case IsTrue:
		is_prdct_satisfied = true;
		break;
		
	    case IsFirstCol:
		is_prdct_satisfied =
		    (SXNDNORMAL_SCAN_P
		     ? sxsrcmngr.previous_char
		     : sxsrcmngr.buffer [sxsrcmngr.labufindex - 1]) == SXNEWLINE;
		break;
		
	    case NotIsFirstCol:
		is_prdct_satisfied =
		    (SXNDNORMAL_SCAN_P
		     ? sxsrcmngr.previous_char
		     : sxsrcmngr.buffer [sxsrcmngr.labufindex - 1]) != SXNEWLINE;
		break;
		
	    case IsSet:
		is_prdct_satisfied =
		    (sxndsvar.current_ndlv->counters [action_or_prdct_code->param] != 0);
		break;
		
	    case IsReset:
		is_prdct_satisfied =
		    (sxndsvar.current_ndlv->counters [action_or_prdct_code->param] == 0);
		break;
		
	    case IsLastCol:
	    case NotIsLastCol:
	    {
		SXINT	sxchar;
		bool	is_a_lookahead;
		
		sxchar = (is_a_lookahead = !SXNDNORMAL_SCAN_P)
		    ? sxlanext_char () : sxlafirst_char ();
		
		if (is_a_lookahead)
		    sxlaback (1);
		
		is_prdct_satisfied = sxchar == SXNEWLINE || sxchar == EOF;
	    }
		
		if (action_or_prdct_code->action_or_prdct_no == 6)
		    is_prdct_satisfied = !is_prdct_satisfied;
		
		break;
		
	    default:
		break;
	    }
	} /* system predicate */
	else {
	    /* user predicate */
	    /* sxndsvar.current_ndlv->current_state_no = current_state; */
	    ts_null ();
	    
	    if (sxndsvar.mode.with_user_prdct)
		is_prdct_satisfied =
		    (*sxndsvar.SXS_tables.S_scanact)
			(SXPREDICATE,
			 action_or_prdct_code->action_or_prdct_no);
	    
	    if (action_or_prdct_code->param != 0)
		is_prdct_satisfied = !is_prdct_satisfied;
	}
    } while (!is_prdct_satisfied);
    
    return action_or_prdct_code->stmt;
}


SXSTMI action_processing (struct SXS_action_or_prdct_code *action_or_prdct_code)
{
    /* transition vers une action */
    SXINT		i; 
    SXSTMI	stmt;
    char	*pstr;
	    
    
    stmt = action_or_prdct_code->stmt;
    
    if (action_or_prdct_code->is_system && sxndsvar.mode.with_system_act) {
	switch (action_or_prdct_code->action_or_prdct_no) {
	case LowerToUpper:
	    /* Lower_To_Upper (last char) */
	    pstr = &(sxsvar.sxlv_s.token_string [sxsvar.sxlv.ts_lgth - 1]);
	    *pstr = sxtoupper (*pstr);
	    break;

	case UpperToLower:
	    /* Upper_To_Lower (last char) */
	    pstr = &(sxsvar.sxlv_s.token_string [sxsvar.sxlv.ts_lgth - 1]);
	    *pstr = sxtolower (*pstr);
	    break;

	case Set:
	    sxndsvar.current_ndlv->counters [action_or_prdct_code->param] = 1;
	    break;
	    
	case Reset:
	    sxndsvar.current_ndlv->counters [action_or_prdct_code->param] = 0;
	    break;
	    
	case Erase:
	    if (sxndsvar.current_ndlv->ts_lgth > sxndsvar.current_ndlv->mark.index)
		sxndsvar.current_ndlv->ts_lgth = sxndsvar.current_ndlv->mark.index == -1
		    ? 0 : sxndsvar.current_ndlv->mark.index;
	    
	    sxndsvar.current_ndlv->mark.index = -1 /* no more Mark */ ;
	    break;
	    
	case Include:
	    if (SXNDNORMAL_SCAN_P) {
		/* Quoi faire en erreur ? */
		push_include (action_or_prdct_code->param);
		ndlv_clear (sxndsvar.current_ndlv, false);
		stmt = 0;	/* sera calcule' */
	    }

	    break;
	    
	case UpperCase:
	    /* Upper_Case (token_string) */
	    for (i = 0; i < sxsvar.sxlv.ts_lgth; i++) {
		pstr = &(sxsvar.sxlv_s.token_string [i]);
		*pstr = sxtoupper (*pstr);
	    }
	    
	    break;
	    
	case LowerCase:
	    /* Lower_Case (token_string)_To_Lower */
	    for (i = 0; i < sxsvar.sxlv.ts_lgth; i++) {
		pstr = &(sxsvar.sxlv_s.token_string [i]);
		*pstr = sxtolower (*pstr);
	    }
	    
	    break;
	    
	case Put:
	    /* Unput (char) */
	    ts_put (action_or_prdct_code->param);
	    break;
	    
	case Mark:
	    sxndsvar.current_ndlv->mark.source_coord = sxsrcmngr.source_coord;
	    sxndsvar.current_ndlv->mark.index = sxndsvar.current_ndlv->ts_lgth;
	    sxndsvar.current_ndlv->mark.previous_char = sxsrcmngr.previous_char;
	    break;
	    
	case Release:
	    if (sxndsvar.current_ndlv->ts_lgth >= sxndsvar.current_ndlv->mark.index) {
		ts_null ();
		
		if (sxndsvar.current_ndlv->mark.index == -1)
		    sxsrcpush (sxndsvar.current_ndlv->previous_char,
			       &(sxndsvar.current_ndlv->token_string
				 [sxndsvar.current_ndlv->ts_lgth = 0]),
			       sxndsvar.current_ndlv->terminal_token.source_index);
		else
		    sxsrcpush (sxndsvar.current_ndlv->mark.previous_char,
			       &(sxndsvar.current_ndlv->token_string
				 [sxndsvar.current_ndlv->ts_lgth =
				  sxndsvar.current_ndlv->mark.index]),
			       sxndsvar.current_ndlv->mark.source_coord);
	    }
	    
	    sxndsvar.current_ndlv->mark.index = -1 /* no more Mark */ ;
	    break;
	    
	case Incr:
	    sxndsvar.current_ndlv->counters [action_or_prdct_code->param]++;
	    break;
	    
	case Decr:
	    sxndsvar.current_ndlv->counters [action_or_prdct_code->param]--;
	    break;
	default: /* pour faire taire gcc -Wswitch-default */
#if EBUG
	  sxtrap(ME,"unknown switch case #1");
#endif
	  break;
	} /* case  */
    } /* end system action */
    else {
	/* user action */
	/* Probleme de la synchronisation entre une action qui lit du source
	   et les autres scanners non-deterministes en parallele.
	   La description lexicale doit se faire par :
	       ... @k {Any &l} ...
	   @k : est l'action qui traite du source jusqu'en (par exemple ligne
	        i colonne j). Le texte doit etre lu en look-ahead (afin de pouvoir etre
		relu).
	   &l : retourne vrai tant que le caractere courant n'est pas en ligne i colonne j
	*/
	ts_null ();
	
	if (sxndsvar.mode.with_user_act)
	    (void) (*sxndsvar.SXS_tables.S_scanact)
		(SXACTION, action_or_prdct_code->action_or_prdct_no);
	
	/* cas tordu :
	   
	   %STRING		= QUOTE {
	   ^QUOTE |
	   @Mark -EOL&True @Put (QUOTE) @Put (EOL) @Release @1
	   }
	   QUOTE ;
	   
	   -- @1 : Error message "Illegal occurrence of EOL in STRING".
	   
	   */
    }
    
    return stmt;
}

void	sxndscan_it (void)
{
    /* debut de la reconnaissance d'une nouvelle unite lexicale */
    SXSTMI				stmt, codop;
    SXINT					current_index;
    struct SXS_action_or_prdct_code	*action_or_prdct_code;
    bool				has_reduce, has_read;
    
    do {
	while ((current_index = sxba_scan_reset (sxndsvar.active_ndlv_set, -1)) != -1) {
	    sxndsvar.current_ndlv = sxndsvar.ndlv + current_index;
	    
	    if ((stmt = sxndsvar.current_ndlv->stmt) == 0)
		sxndsvar.current_ndlv->stmt = stmt = sxndsvar.SXS_tables.S_transition_matrix
		    [sxndsvar.current_ndlv->current_state_no]
			[char_to_class (sxsrcmngr.current_char)];
	    
	    if (SCAN (stmt)) {
		SXBA_1_bit (sxndsvar.read_char_ndlv_set, current_index);
	    }
	    else
		switch (codop = CODOP (stmt)) {
		case SameState: /* ?? */
		case State:
		    sxndsvar.current_ndlv->stmt = 0;
		    sxndsvar.current_ndlv->current_state_no = NEXT (stmt);
		    SXBA_1_bit (sxndsvar.active_ndlv_set, current_index);
		    break;

		case Reduce:
		case HashReduce:
		    SXBA_1_bit (sxndsvar.reduce_ndlv_set, current_index);
		    break;
		    
		case FirstLookAhead:
		    sxndsvar.mode.mode = SXLA_SCAN;
		    stmt = sxndsvar.SXS_tables.S_transition_matrix
			[NEXT (stmt)] [char_to_class (sxlafirst_char ())];
		    
		    while ((codop = CODOP (stmt)) == NextLookAhead || codop == ActPrdct) {
			if (codop == NextLookAhead)
			    stmt = sxndsvar.SXS_tables.S_transition_matrix
				[NEXT (stmt)] [char_to_class (sxlanext_char ())];
			else {
			    action_or_prdct_code =
				&sxndsvar.SXS_tables.SXS_action_or_prdct_code [NEXT (stmt)];
			    
			    if (action_or_prdct_code->kind == IsPredicate) {
				stmt = predicate_processing (action_or_prdct_code);
			    }
			    else if (action_or_prdct_code->kind == IsAction) {
				codop = CODOP (action_or_prdct_code->stmt);

				if (codop == ReducePost || codop == HashReducePost)
				    break;

				/* transition vers une action ou un groupe de predicats */
				sxtrap (ME, "action_in_look_ahead");
			    }
			    else /* non-determinisme */
				break;
			}
		    }
		    
		    switch (codop) {
		    case Error:
			(*sxndsvar.SXS_tables.S_recovery) (SXERROR, (SXINT) 0 /* dummy */, NULL /* dummy */);
			break;
			
		    default:
			sxndsvar.current_ndlv->stmt = stmt;
			SXBA_1_bit (sxndsvar.active_ndlv_set, current_index);
			break;
		    }
		    
		    sxndsvar.mode.mode = SXNORMAL_SCAN;
		    break;
		    
		case ActPrdct:
		    action_or_prdct_code =
			&sxndsvar.SXS_tables.SXS_action_or_prdct_code [NEXT (stmt)];

		    if (action_or_prdct_code->kind == IsAction) {
			codop = CODOP (action_or_prdct_code->stmt);

			if (codop == ReducePost || codop == HashReducePost) {
			    SXBA_1_bit (sxndsvar.reduce_ndlv_set, current_index);
			    break;
			}
			else {
			    /* transition vers une action ou un groupe de predicats */
			    stmt = action_processing (action_or_prdct_code);
			    /* On ne touche pas a current_state_no pour l'error recovery
			       eventuel. */
			}
		    }
		    else if (action_or_prdct_code->kind == IsPredicate) {
			/* transition vers une action ou un groupe de predicats */
			stmt = predicate_processing (action_or_prdct_code);
			/* On ne touche pas a current_state_no pour l'error recovery
			   eventuel. */
		    }
		    else {
			/* Non determinism processing */
			/* Au moins 2 possibilites. */
			do {
			    SXINT new_index;
			    
			    new_index = clone_active_scanner (action_or_prdct_code->stmt);
			    SXBA_1_bit (sxndsvar.active_ndlv_set, new_index);
			} while ((++action_or_prdct_code)->action_or_prdct_no > 0);
			
			/* On reutilise le scanner courant comme dernier clone */
			stmt = action_or_prdct_code->stmt;
		    }

		    sxndsvar.current_ndlv->stmt = stmt;
		    SXBA_1_bit (sxndsvar.active_ndlv_set, current_index);
		    break;
		    
		case Error:
		    if (sxndsvar.rcvr.ndlv_set == NULL)
			sxndsvar.rcvr.ndlv_set = sxba_calloc (sxndsvar.ndlv_size);

		    SXBA_1_bit (sxndsvar.rcvr.ndlv_set, current_index);
		    break;

		default:
		    /* impossible */
		    sxtrap (ME, "sxndscan_it");
		}
	}
	
	if ((has_reduce = ((current_index = sxba_scan (sxndsvar.reduce_ndlv_set, -1)) >= 0))) {
	    do {
		sxndsvar.current_ndlv = sxndsvar.ndlv + current_index;
		stmt = sxndsvar.current_ndlv->stmt;
		
		if (KEEP (stmt))
		    ts_put (sxsrcmngr.current_char);
		
		switch (CODOP (stmt)) {
		case Reduce:
		    if ((sxndsvar.current_ndlv->terminal_token.lahead = NEXT (stmt)) == 0) {
			/* comments */ 
			if (sxndsvar.current_ndlv->ts_lgth != 0) {
			    ts_null ();
			    comments_put (sxndsvar.current_ndlv->token_string,
					  sxndsvar.current_ndlv->ts_lgth);
			}
		    }
		    else {
			if (sxndsvar.current_ndlv->ts_lgth > 0)
			    sxndsvar.current_ndlv->terminal_token.string_table_entry =
				sxstr2save (sxndsvar.current_ndlv->token_string,
					    sxndsvar.current_ndlv->ts_lgth);
		    }
		    
		    break;
		    
		case HashReduce:
		    hashreduce (stmt);
		    break;
		    
		case ActPrdct:
		    action_or_prdct_code = &sxndsvar.SXS_tables.SXS_action_or_prdct_code [NEXT (stmt)];
		    
		    if (action_or_prdct_code->kind == IsAction) {
			/* transition vers une action */
			codop = CODOP (stmt = action_or_prdct_code->stmt);
			
			if (codop == ReducePost || codop == HashReducePost) {
			    if (codop == HashReducePost)
				hashreduce (stmt);
			    else {
				sxndsvar.current_ndlv->terminal_token.lahead = NEXT (stmt);
				/* pas comment ? */
				
				if (sxndsvar.current_ndlv->ts_lgth > 0)
				    sxndsvar.current_ndlv->terminal_token.string_table_entry =
					sxstr2save (sxndsvar.current_ndlv->token_string,
						    sxndsvar.current_ndlv->ts_lgth);
			    }
			    
			    ts_null ();
			    
			    if (sxndsvar.mode.with_user_act)
				(void) (*sxndsvar.SXS_tables.S_scanact)
				    (SXACTION, action_or_prdct_code->action_or_prdct_no);
			    /* Dans tous les cas, la post-action s'est occupee
			       des commentaires eventuels. */
			}
			else
			    sxtrap (ME, "sxndscan_it");		    
		    }
		    else {
			/* transition vers un groupe de predicats */
			/* Non determinism processing */
			sxtrap (ME, "sxndscan_it");
		    }
		    
		    break;
		    
		default:
		    sxtrap (ME, "sxndscan_it");
		}
	    } while ((current_index = sxba_scan (sxndsvar.reduce_ndlv_set, current_index)) != -1);
	}
	
	if ((current_index = sxba_scan_reset (sxndsvar.read_char_ndlv_set, -1)) >= 0) {
	    has_read = true;
	    
	    if (!has_reduce) {
		do {
		    sxndsvar.current_ndlv = sxndsvar.ndlv + current_index;
		    stmt = sxndsvar.current_ndlv->stmt;
		    
		    if (KEEP (stmt))
			ts_put (sxsrcmngr.current_char);
		    
		    SXBA_1_bit (sxndsvar.active_ndlv_set, current_index);
		    
		    switch (CODOP (stmt)) {
		    case SameState:
		    case State:
			sxndsvar.current_ndlv->current_state_no = NEXT (stmt);
			sxndsvar.current_ndlv->stmt = 0;
			break;
			
		    case Reduce:
		    case HashReduce:
		    case ActPrdct:
			sxndsvar.current_ndlv->stmt = NC (stmt);
			break;
			
		    default:
			sxtrap (ME, "sxndscan_it");
		    }
		} while ((current_index = sxba_scan_reset (sxndsvar.read_char_ndlv_set,
							   current_index)) != -1);

		sxnextchar ();
	    }
	    else {
		do {
		    SXBA_1_bit (sxndsvar.active_ndlv_set, current_index);
		} while ((current_index = sxba_scan_reset (sxndsvar.read_char_ndlv_set,
							   current_index)) >= 0);
	    }
	}
	else {
	    has_read = false;

	    if (!has_reduce) {
		/* ni shift ni reduce */
		/* E R R O R   R E C O V E R Y */
		sxndsvar.mode.mode = SXRCVR_SCAN;
		(*sxndsvar.SXS_tables.S_recovery) (SXACTION, (SXINT) 0 /* dummy */, NULL /* dummy */);
		sxndsvar.mode.mode = SXNORMAL_SCAN;
		
		current_index = -1;
		
		while ((current_index =
			sxba_scan_reset (sxndsvar.rcvr.ndlv_set, current_index)) >= 0) {
		    sxndsvar.current_ndlv = sxndsvar.ndlv + current_index;
		    
		    if (sxsrcmngr.current_char == EOF) {
			sxndsvar.ndlv->terminal_token.lahead = sxndsvar.SXS_tables.S_termax;
			/* End Of File */ 
			SXBA_1_bit (sxndsvar.reduce_ndlv_set, current_index);
			has_reduce = true;
		    }
		    else {
			sxndsvar.current_ndlv->stmt = 0;
			SXBA_1_bit (sxndsvar.active_ndlv_set, current_index);
		    }
		}
	    }
	}
	
	if (has_reduce ) {
	    current_index = -1;
	    
	    while ((current_index = sxba_scan (sxndsvar.reduce_ndlv_set,
						     current_index)) >= 0) {
		sxndsvar.current_ndlv = sxndsvar.ndlv + current_index;
		
		if (sxndsvar.current_ndlv->terminal_token.lahead == -1) {
		    /* nelle valeur pour les erreurs sur les mots-cles */
		    if (!has_read) {
			/* E.R. ne donnant qu'une recherche en table hash ayant echouee */
			sxndsvar.mode.errors_nb++;
			sxndsvar.current_ndlv->terminal_token.lahead = 0;
		    
			if (!sxndsvar.mode.is_silent)
			    sxerror (sxndsvar.current_ndlv->terminal_token.source_index,
				     sxndsvar.sxtables->err_titles [2][0],
				     sxndsvar.SXS_tables.S_global_mess [2],
				     sxndsvar.sxtables->err_titles [2]+1);
		    }

		    sxndsvar.current_ndlv->terminal_token.lahead = 0;
		}
		
		if ((sxndsvar.current_ndlv->terminal_token.lahead
		     == sxndsvar.SXS_tables.S_termax)
		    && (sxndsvar.top_incl != NULL)) {
		    /* On suppose une reconnaissance unique du EOF de l'include */
		    SXINT include_action;
		    
		    include_action = pop_include ();
		    ts_null ();
		    
		    if (sxndsvar.mode.with_user_act)
			(void) (*sxndsvar.SXS_tables.S_scanact)
			    (SXACTION, include_action);

		    /* Tout se passe comme si on avait reconnu un comment
		       au niveau de l'appel de l'include */
		    SXBA_1_bit (sxndsvar.reduce_ndlv_set, current_index);
		    sxndsvar.current_ndlv->terminal_token.lahead = 0;
		}
	    }

	    sxndtkn_put (sxndsvar.reduce_ndlv_set);
	}
	
	if (sxndsvar.rcvr.ndlv_set != NULL) {
	    /* On recupere les scanners des branches mortes... */
	    current_index = -1;
	
	    while ((current_index =
		    sxba_scan_reset (sxndsvar.rcvr.ndlv_set, current_index)) >= 0)
		sxindex_release (sxndsvar.index_header, current_index);
	}
    } while (!has_reduce);
}



SXINT sxndscanner (SXINT what, SXTABLES *arg)
{
    SXINT i;
    
    switch (what) {
    case SXOPEN:
	/* new language: new tables, */
	/* the global variable "sxndsvar" must have been saved in case of */
	/* recursive invocation */
	
	sxcheck_magic_number (arg->magic, SXMAGIC_NUMBER, ME);
	sxndsvar.sxtables = arg;
	sxndsvar.SXS_tables = arg->SXS_tables;
	sxindex_alloc (&sxndsvar.index_header, 0 /* vecteur */, 32 /* pourquoi pas */, ndlv_oflw);
	sxndsvar.ndlv_size = sxindex_size (sxndsvar.index_header);
	sxndsvar.active_ndlv_set = sxba_calloc (sxndsvar.ndlv_size);
	sxndsvar.reduce_ndlv_set = sxba_calloc (sxndsvar.ndlv_size);
	sxndsvar.read_char_ndlv_set = sxba_calloc (sxndsvar.ndlv_size);
	sxndsvar.rcvr.ndlv_set = NULL;
	sxndsvar.ndlv = (struct sxndlv*) sxalloc (sxndsvar.ndlv_size, sizeof (struct sxndlv));
	
	for (i = 0; i < sxndsvar.ndlv_size; i++) {
	    sxndsvar.ndlv [i].token_string = NULL;
	    sxndsvar.ndlv [i].counters = NULL;
	}
	
	sxndsvar.rcvr.state_set = NULL;
	sxndsvar.rcvr.source_char = NULL;
	sxndsvar.top_incl = NULL;

	sxndsvar.mode.errors_nb = 0;
	sxndsvar.mode.mode = SXNORMAL_SCAN;
	sxndsvar.mode.is_silent = false;
	sxndsvar.mode.with_system_act = true;
	sxndsvar.mode.with_user_act = true;
	sxndsvar.mode.with_system_prdct = true;
	sxndsvar.mode.with_user_prdct = true;
	
	char_to_class (EOF) = 2 /* EOF class */ ;
	break;
	
    case SXINIT:
	/* Beginning of new source */
	/* read the character that will be current next scan */
	sxnext_char ();

	sxba_empty (sxndsvar.active_ndlv_set);
	sxba_empty (sxndsvar.reduce_ndlv_set);
	sxba_empty (sxndsvar.read_char_ndlv_set);
	seek_active_scanner ();
	break;
	
    case SXACTION:
	sxndscan_it ();
	break;
	
    case SXFINAL:
	/* End of source */
	break;

    case SXCLOSE:
	/* End of language */
	if (sxndsvar.rcvr.ndlv_set != NULL)
	    sxfree (sxndsvar.rcvr.ndlv_set);

	for (i = 0; i < sxndsvar.ndlv_size; i++)
	    if (sxndsvar.ndlv [i].token_string)
		sxfree (sxndsvar.ndlv [i].token_string);

	if (sxndsvar.SXS_tables.S_counters_size)
	    for (i = 0; i < sxndsvar.ndlv_size; i++)
		if (sxndsvar.ndlv [i].counters)
		    sxfree (sxndsvar.ndlv [i].counters);

	sxfree (sxndsvar.ndlv);
	sxfree (sxndsvar.active_ndlv_set);
	sxfree (sxndsvar.reduce_ndlv_set);
	sxfree (sxndsvar.read_char_ndlv_set);
	sxindex_free (sxndsvar.index_header);

	(*sxndsvar.SXS_tables.S_recovery) (SXCLOSE, (SXINT) 0 /* dummy */, NULL /* dummy */);
	break;

    default:
	fprintf (sxstderr, "The function \"sxndscanner\" is out of date with respect to its specification.\n");
	sxexit(1);
	    /*NOTREACHED*/
    }

    return 0;
}
