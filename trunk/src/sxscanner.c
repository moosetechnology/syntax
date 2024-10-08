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

#ifdef sxgetchar_is_redefined
#error you really mess up everything, please do not define sxgetchar_is_redefined
#endif /* sxgetchar_is_redefined */

#ifdef sxgetchar
#define sxgetchar_is_redefined
#endif /* sxgetchar */

#include "sxversion.h"
#include "sxunix.h"

#ifdef sxgetchar_is_redefined
/* on a donc demande' une compilation speciale de sxscanner.c avec -Dsxgetchar, car on veut se le
   redefinir differement de la macro definie dans sxunix.h */
#undef sxgetchar
extern short sxgetchar (void);
#define sxgetchar sxgetchar
#endif /* sxgetchar_is_redefined */

#ifndef VARIANT_32
char WHAT_SXSCANNER[] = "@(#)SYNTAX - $Id: sxscanner.c 4166 2024-08-19 09:00:49Z garavel $" WHAT_DEBUG;
#endif

static char	ME [] = "SCANNER";

#include <ctype.h>
#include "sxsstmt.h"


#if BUG
#undef sxnextchar
#define sxnextchar sxnext_char
#endif

#define IS_IN_LA  (sxsvar.SXS_tables.S_transition_matrix [current_state] [1] != 0)


static SXINT	ind_incl, ind_incl_use;
static struct sxlv	*sxsvar_include;


/* Nelle version qui utilise sxcomment_mngr */
static void
comments_put (char *str, SXINT lgth)
{
  sxsvar.sxlv.terminal_token.comment = sxcomment_put (str, lgth);
}


#define ts_put(c)							\
(    (sxsvar.sxlv.ts_lgth+2 == sxsvar.sxlv_s.ts_lgth_use)			\
	? sxsvar.sxlv_s.token_string = (char *) sxrealloc (sxsvar.sxlv_s.	\
	    token_string, sxsvar.sxlv_s.ts_lgth_use *= 2, sizeof (char))	\
	: NULL,								\
     sxsvar.sxlv_s.token_string [sxsvar.sxlv.ts_lgth ++] = (char)c		\
)


#define ts_null()	(sxsvar.sxlv_s.token_string [sxsvar.sxlv.ts_lgth] = SXNUL)

#define char_to_class(c) (sxsvar.SXS_tables.S_char_to_simple_class[c])




void	sxscan_it (void)
{
    SXSTMI	stmt;
    SXINT	current_state;
    unsigned char	current_class;
    unsigned char	current_class_in_la;
    struct SXS_action_or_prdct_code	*action_or_prdct_code;

    sxinitialise(action_or_prdct_code); /* pour faire taire "gcc -Wuninitialized" */
    sxsvar.sxlv.terminal_token.comment = NULL;

ccnewlextok:
    current_class = char_to_class (sxsrcmngr.current_char);

/* debut de la reconnaissance d'une nouvelle unite lexicale */

newlextok:
    sxsvar.sxlv.ts_lgth = 0;
    sxsvar.sxlv.terminal_token.source_index = sxsrcmngr.source_coord;
    sxsvar.sxlv.mark.index = -1 /* no explicit Mark yet */ ;
    sxsvar.sxlv.previous_char = sxsrcmngr.previous_char;
    current_state = 1;
read_transition:
    stmt = sxsvar.SXS_tables.S_transition_matrix [current_state] [current_class];

SWITCH:
    if (KEEP (stmt)) {
	ts_put (sxsrcmngr.current_char);
    }

    if (SCAN (stmt)) {
	current_class = char_to_class (sxnextchar ());
    }

    switch (CODOP (stmt)) {
    case Error:
	/* E R R O R   R E C O V E R Y */
	{
	    unsigned char	class;

	    if ((*sxsvar.SXS_tables.S_recovery) (SXACTION, current_state, &class)) {
		if (IS_IN_LA) {
		    current_class_in_la = class;
		    goto read_la_transition;
		}
		else {
		    current_class = class;
		    goto read_transition;
		}
	    }
	    else {
		sxsvar.sxlv.terminal_token.lahead = sxsvar.SXS_tables.S_termax;
		sxsvar.sxlv.terminal_token.string_table_entry = SXEMPTY_STE;
		sxsvar.sxlv.ts_lgth = 0;
		goto done;
	    }
	}

    case SameState:
	{
	    SXINT	seed_class;
	    SXSTMI	*ctmline = sxsvar.SXS_tables.S_transition_matrix [current_state];
	    SXSTMI	seed_stmt = stmt;

	    if ((stmt = ctmline [current_class]) == seed_stmt) {
		seed_class = current_class;

		if (KEEP (stmt))
		    do
			ts_put (sxsrcmngr.current_char);
		    while ((current_class = char_to_class (sxnextchar ())) == seed_class || (stmt = ctmline [
			 current_class]) == seed_stmt);
		else
		    while ((current_class = char_to_class (sxnextchar ())) == seed_class || (stmt = ctmline [
			 current_class]) == seed_stmt)
			;
	    }
	}

	goto SWITCH;

    case State:
	current_state = NEXT (stmt);
	goto read_transition;

    case Reduce:
    case ReducePost:
	if ((sxsvar.sxlv.terminal_token.lahead = NEXT (stmt)) == 0 /* comments */ ) {
	    if (sxsvar.sxlv.ts_lgth != 0) {
		ts_null ();
		comments_put (sxsvar.sxlv_s.token_string, sxsvar.sxlv.ts_lgth);
	    }

	    goto newlextok;
	}

	if (sxsvar.sxlv.ts_lgth == 0)
	    sxsvar.sxlv.terminal_token.string_table_entry = SXEMPTY_STE;
	else
	    sxsvar.sxlv.terminal_token.string_table_entry = sxstr2save (sxsvar.sxlv_s.token_string, sxsvar.sxlv.ts_lgth);

done:
	/* fin de la reconnaisance */

	if ((sxsvar.sxlv.terminal_token.lahead == sxsvar.SXS_tables.S_termax) && (sxsvar.sxlv_s.include_no > 0)) {
	    /* restore "sxsvar.sxlv" et appel utilisateur */
	    sxsvar.sxlv = sxsvar_include [--ind_incl];
	    sxsvar.sxlv_s.include_no--;
	    /* appel numeros de la post action */

	    ts_null ();

	    if (sxsvar.sxlv.mode.with_user_act)
		(void) (*sxsvar.SXS_tables.S_scanact) (SXACTION, sxsvar.sxlv.include_action);

	    goto ccnewlextok; /* sur l'ancien source */
	}

	break;

    case HashReduce:
    case HashReducePost:
	if ((sxsvar.sxlv.terminal_token.lahead =
	     (*sxsvar.SXS_tables.S_check_keyword) (sxsvar.sxlv_s.token_string, sxsvar.sxlv.ts_lgth)) == 0) {
	    /* not a keyword */
	    sxsvar.sxlv.terminal_token.lahead = NEXT (stmt);

	    if (sxsvar.sxlv.terminal_token.lahead == 0) {
		/* E.R. ne donnant qu'une recherche en table hash ayant echouee */
		sxsvar.sxlv.mode.errors_nb++;

		if (!sxsvar.sxlv.mode.is_silent)
		    sxerror (sxsvar.sxlv.terminal_token.source_index,
			     sxsvar.sxtables->err_titles [2][0],
			     sxsvar.SXS_tables.S_global_mess [2],
			     sxsvar.sxtables->err_titles [2]+1);
		goto newlextok;
	    }

	    sxsvar.sxlv.terminal_token.string_table_entry = sxstr2save (sxsvar.sxlv_s.token_string, sxsvar.sxlv.ts_lgth);
	}
	else
	    sxsvar.sxlv.terminal_token.string_table_entry = SXEMPTY_STE;

	goto done;

    case FirstLookAhead:
	current_class_in_la = char_to_class (sxlafirst_char ());
	goto read_next_char_in_la;

    case NextLookAhead:
	current_class_in_la = char_to_class (sxlanext_char ());
read_next_char_in_la:
	current_state = NEXT (stmt);
read_la_transition:
	stmt = sxsvar.SXS_tables.S_transition_matrix [current_state] [current_class_in_la];
	goto SWITCH;

    case ActPrdct:
	action_or_prdct_code = &sxsvar.SXS_tables.SXS_action_or_prdct_code [NEXT (stmt)];

	if (action_or_prdct_code->kind == IsPredicate) {
	    /* transition vers un groupe de predicats */
	    bool	is_prdct_satisfied = false;

	    action_or_prdct_code--;

	    do {
		if ((++action_or_prdct_code)->is_system) {
		    switch (action_or_prdct_code->action_or_prdct_no) {
		    case IsTrue:
			is_prdct_satisfied = true;
			break;

		    case IsFirstCol:
			is_prdct_satisfied = (IS_IN_LA ? sxsrcmngr.buffer [sxsrcmngr.labufindex - 1] : sxsrcmngr.
			     previous_char) == SXNEWLINE;
			break;

		    case NotIsFirstCol:
			is_prdct_satisfied = (IS_IN_LA ? sxsrcmngr.buffer [sxsrcmngr.labufindex - 1] : sxsrcmngr.
			     previous_char) != SXNEWLINE;
			break;

		    case IsSet:
			is_prdct_satisfied = (sxsvar.sxlv_s.counters [action_or_prdct_code->param] != 0);
			break;

		    case IsReset:
			is_prdct_satisfied = (sxsvar.sxlv_s.counters [action_or_prdct_code->param] == 0);
			break;

		    case IsLastCol:
		    case NotIsLastCol:
			{
			    SXINT	sxchar;
			    bool	is_a_lookahead;

			    sxchar = (is_a_lookahead = IS_IN_LA) ? sxlanext_char () : sxlafirst_char ();

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
		    sxsvar.sxlv.current_state_no = current_state;
		    ts_null ();

		    if (sxsvar.sxlv.mode.with_user_prdct)
			is_prdct_satisfied = (*sxsvar.SXS_tables.S_scanact) (SXPREDICATE, action_or_prdct_code->
			 action_or_prdct_no);

		    if (action_or_prdct_code->param != 0)
			is_prdct_satisfied = !is_prdct_satisfied;
		}
	    } while (!is_prdct_satisfied);

	    stmt = action_or_prdct_code->stmt;
	}
	else {
	    /* transition vers une action */
	    switch (CODOP (stmt = action_or_prdct_code->stmt)) {
	    case HashReducePost:
	    case ReducePost:
		goto SWITCH;

	    default:
		break;
	    }

	    if (action_or_prdct_code->is_system && sxsvar.sxlv.mode.with_system_act) {
		switch (action_or_prdct_code->action_or_prdct_no) {
		    char	*pstr;

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
		    sxsvar.sxlv_s.counters [action_or_prdct_code->param] = 1;
		    break;

		case Reset:
		    sxsvar.sxlv_s.counters [action_or_prdct_code->param] = 0;
		    break;

		case Erase:
		    if (sxsvar.sxlv.ts_lgth > sxsvar.sxlv.mark.index)
			sxsvar.sxlv.ts_lgth = sxsvar.sxlv.mark.index == -1 ? 0 : sxsvar.sxlv.mark.index;

		    sxsvar.sxlv.mark.index = -1 /* no more Mark */ ;
		    break;

		case Include:
		    /* sauvegarde de sxsvar.sxlv_s */

		    sxsvar.sxlv.include_action = action_or_prdct_code->param;
		    sxsvar.sxlv_s.include_no++;

		    if (sxsvar_include == NULL) {
		        sxprepare_for_possible_reset (sxsvar_include);
			sxsvar_include = (struct sxlv*) sxalloc ((ind_incl_use = 5), sizeof (struct sxlv));
		    }
		    else if (ind_incl >= ind_incl_use)
			sxsvar_include = (struct sxlv*) sxrealloc (sxsvar_include, ind_incl_use += 5, sizeof (struct sxlv));

		    sxsvar_include [ind_incl++] = sxsvar.sxlv;
		    /* le premier caractere est lu par l'action utilisateur */
		    goto ccnewlextok;

		case UpperCase:
		    /* Upper_Case (token_string) */
		    {
			SXINT	i;

			for (i = 0; i < sxsvar.sxlv.ts_lgth; i++) {
			    pstr = &(sxsvar.sxlv_s.token_string [i]);
			    *pstr = sxtoupper (*pstr);
			}

			break;
		    }

		case LowerCase:
		    /* Lower_Case (token_string)_To_Lower */
		    {
			SXINT	i;

			for (i = 0; i < sxsvar.sxlv.ts_lgth; i++) {
			    pstr = &(sxsvar.sxlv_s.token_string [i]);
			    *pstr = sxtolower (*pstr);
			}

			break;
		    }

		case Put:
		    /* Unput (char) */

		    ts_put (action_or_prdct_code->param);
		    break;

		case Mark:
		    sxsvar.sxlv.mark.source_coord = sxsrcmngr.source_coord;
		    sxsvar.sxlv.mark.index = sxsvar.sxlv.ts_lgth;
		    sxsvar.sxlv.mark.previous_char = sxsrcmngr.previous_char;
		    break;

		case Release:
		    if (sxsvar.sxlv.ts_lgth >= sxsvar.sxlv.mark.index) {
			ts_null ();

			if (sxsvar.sxlv.mark.index == -1)
			    sxsrcpush (sxsvar.sxlv.previous_char, &(sxsvar.sxlv_s.token_string [sxsvar.sxlv.ts_lgth = 0]),
				 sxsvar.sxlv.terminal_token.source_index);
			else
			    sxsrcpush (sxsvar.sxlv.mark.previous_char, &(sxsvar.sxlv_s.token_string [sxsvar.sxlv.ts_lgth = sxsvar.sxlv.mark.index]),
				 sxsvar.sxlv.mark.source_coord);

			current_class = char_to_class (sxsrcmngr.current_char) /* Cf user action */;
		    }

		    sxsvar.sxlv.mark.index = -1 /* no more Mark */ ;
		    break;

		case Incr:
		    sxsvar.sxlv_s.counters [action_or_prdct_code->param]++;
		    break;

		case Decr:
		    sxsvar.sxlv_s.counters [action_or_prdct_code->param]--;
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
		ts_null ();

		if (sxsvar.sxlv.mode.with_user_act)
		    (void) (*sxsvar.SXS_tables.S_scanact) (SXACTION, action_or_prdct_code->action_or_prdct_no);
		current_class = char_to_class (sxsrcmngr.current_char);
		

/* cas tordu :

	%STRING		= QUOTE {
				  ^QUOTE |
				  @Mark -EOL&True @Put (QUOTE) @Put (EOL) @Release @1
			     	}
			  QUOTE ;

-- @1 : Error message "Illegal occurrence of EOL in STRING".

*/
	    }
	}

	goto SWITCH;

    default:
	break;
    }

    switch (CODOP (stmt)) {
    case HashReducePost:
    case ReducePost:
	ts_null ();

	if (sxsvar.sxlv.mode.with_user_act)
	    (void) (*sxsvar.SXS_tables.S_scanact) (SXACTION, action_or_prdct_code->action_or_prdct_no);

	if (sxsvar.sxlv.terminal_token.lahead == 0) {
	    /* Dans tous les cas, la post-action s'est occupee des commentaires eventuels. */
	    if (sxsvar.sxlv.terminal_token.string_table_entry == SXERROR_STE)
		/* La post-action a decide' de retourner a l'appelant sans rien faire... */
		return;

	    /* La post-action a decide' de reboucler... */
	    goto ccnewlextok;
	}

    default:
	break;
    }

    sxput_token(sxsvar.sxlv.terminal_token);

#if BUG
 {
   struct sxtoken *tok_ptr;

   tok_ptr = &SXGET_TOKEN(sxplocals.Mtok_no);
   
   fprintf (stdout,
	    "%s(sxscan_it)\t\ttoken [%ld(%ld, %ld)] = (%ld, \"%s\")\n" /* & */
	    "\t\t\tste = (%ld, \"%s\")\n" /* & */
	    "\t\t\tsource_index = (%s, %ld, %ld)\n" /* & */
	    "\t\t\tcomment = \"%s\"\n",
	    ME,
	    sxplocals.Mtok_no,
	    (SXINT) SXTOKEN_PAGE(sxplocals.Mtok_no),
	    (SXINT) SXTOKEN_INDX(sxplocals.Mtok_no),
	    tok_ptr->lahead,
	    sxttext (sxsvar.sxtables, tok_ptr->lahead),
	    tok_ptr->string_table_entry,
	    tok_ptr->string_table_entry > 1 ? sxstrget (tok_ptr->string_table_entry) : (tok_ptr->string_table_entry == 1 ?
											"SXEMPTY_STE" : "SXERROR_STE"),
	    tok_ptr->source_index.file_name,
	    (SXUINT) tok_ptr->source_index.line,
	    (SXUINT) tok_ptr->source_index.column,
	    (tok_ptr->comment != NULL) ? tok_ptr->comment : "(NULL)"
	    );
 }
#endif

}




/* VARARGS1 */

void sxscanner (SXINT what, SXTABLES *arg)
{
    SXINT i;

    switch (what) {
    case SXOPEN:
	/* new language: new tables, */
	/* the global variable "sxsvar" must have been saved in case of
	   recursive invocation */

	sxcheck_magic_number (arg->magic, SXMAGIC_NUMBER, ME);
	sxsvar.sxtables = arg;
	sxsvar.SXS_tables = arg->SXS_tables;
	sxsvar.sxlv_s.token_string = (char*) sxalloc (sxsvar.sxlv_s.ts_lgth_use = 120, sizeof (char));
	sxsvar.sxlv_s.counters = (sxsvar.SXS_tables.S_counters_size != 0) ? (SXINT *) sxalloc (sxsvar.SXS_tables.
	     S_counters_size, sizeof (SXINT)) : NULL;

	sxsvar.sxlv.mode.errors_nb = 0;
	sxsvar.sxlv.mode.is_silent = false;
	sxsvar.sxlv.mode.with_system_act = true;
	sxsvar.sxlv.mode.with_user_act = true;
	sxsvar.sxlv.mode.with_system_prdct = true;
	sxsvar.sxlv.mode.with_user_prdct = true;

	char_to_class (EOF) = 2 /* EOF class */ ;

	if (!sxsvar.SXS_tables.S_are_comments_erased)
	  sxcomment_mngr (SXOPEN, 1024);

	break;

    case SXINIT:
	/* Beginning of new source */
	sxsvar.sxlv_s.include_no = 0;

	if (sxsvar.sxlv_s.counters != NULL) {
	    for (i = sxsvar.SXS_tables.S_counters_size - 1; i >= 0; i--)
		sxsvar.sxlv_s.counters[i] = 0;
	}

	/* read the character that will be current next scan */
	sxnext_char ();
	break;

    case SXACTION:
	sxscan_it ();
	break;

    case SXFINAL:
	/* End of source */
	break;

    case SXCLOSE:
	/* End of language */
	sxfree (sxsvar.sxlv_s.token_string), sxsvar.sxlv_s.token_string = NULL;

	if (sxsvar.sxlv_s.counters != NULL)
	    sxfree (sxsvar.sxlv_s.counters), sxsvar.sxlv_s.counters = NULL;

	if (sxsvar_include != NULL)
	    sxfree (sxsvar_include), sxsvar_include = NULL;

	(*sxsvar.SXS_tables.S_recovery) (SXCLOSE, (SXINT) 0 /* dummy */, NULL /* dummy */);

	if (!sxsvar.SXS_tables.S_are_comments_erased)
	  sxcomment_mngr (SXCLOSE, 0);

	break;

    default:
	fprintf (sxstderr, "The function \"sxscanner\" is out of date with respect to its specification.\n");
	sxexit(1);
	    /*NOTREACHED*/
    }
}
