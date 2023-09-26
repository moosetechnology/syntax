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
/* Lun 20 Sep 1999 10:16 (pb):	Utilisation de sxto(lower|upper)	*/
/************************************************************************/
/* 21-01-94 14:30 (pb):		Caractere change de "char" en "SXINT"	*/
/************************************************************************/
/* 21-10-93 13:00 (phd&pb):	Adaptation a OSF			*/
/************************************************************************/
/* 11-05-93 17:25 (pb):		ref a is_predicate changee en ref a	*/
/*				kind == IsPredicate			*/
/************************************************************************/
/* 14-04-92 14:30 (pb):		Etiquette ccnewlextok			*/
/************************************************************************/
/* 08-04-92 11:40 (pb):		Utilisation de sxsvar.sxlv.mode		*/
/************************************************************************/
/* 11-02-92 13:35 (pb):		token_mngr devient un module separe	*/
/************************************************************************/
/* 10-02-92 13:38 (pb) :	Le resultat du scanner est l'appel de	*/
/*				la macro sxput_token			*/
/************************************************************************/
/* 10-02-92 13:38 (pb) :	Si le resultat d'une post-action est	*/
/*				un token de "lahead" nul, on reboucle	*/
/************************************************************************/
/* 18-09-90 10:10 (pb) :	Allocation de "counters" dans "OPEN"	*/
/*				Deallocation de "counters" dans "CLOSE"	*/
/*				Initialisation dans "INIT"		*/
/************************************************************************/
/* 02-05-88 16:42 (pb) :	Nouveau check_keyword			*/
/************************************************************************/
/* 02-10-87 08:55 (pb) :	Le premier caractere d'un include est lu*/
/*				par l'action utilisateur (sxpush_incl)	*/
/************************************************************************/
/* 01-10-87 16:35 (pb) :	Test du token EOF au lieu du caractere	*/
/*				pour tester la fin d'un include		*/
/************************************************************************/
/* 22-06-87 11:32 (pb) :	Calcul de current_class apres @Release	*/
/************************************************************************/
/* 25-05-87 11:03 (phd) :	Ajout d'un message d'erreur		*/
/************************************************************************/
/* 06-05-87 17:14 (pb) :	si ^&n alors param == 1		        */
/************************************************************************/
/* 05-05-87 13:51 (pb) :	Appel de sxcheck_keyword en librairie   */
/************************************************************************/
/* 24-04-87 15:52 (pb) :	Remplacement des "user_flags" par       */
/*				"counters" + "@Incr" et "@Decr"		*/
/*				Seule l'option "BUG" est conservee	*/
/************************************************************************/
/* 16-04-87 11:09 (pb) :	Refonte totale pour nouveaux codops	*/
/************************************************************************/
/* 26-03-87 10:48 (pb) :	Suppression de sxscanner (END) et	*/
/*				ajout d'un parametre a recovery		*/
/************************************************************************/
/* 20-03-87 16:07 (pb) :	Appel de scramble et recovery par	*/
/*				l'intermediaire des tables		*/
/************************************************************************/
/* 02-03-87 14:09 (phd) :	Renommage et optimisation de		*/
/*				sxcheck_keyword				*/
/************************************************************************/
/* 13-02-87 13:30 (phd) :	La version BUG ne lit que via le 	*/
/*				source manager				*/
/************************************************************************/
/* 13-02-87 09:40 (pb) :	Refonte du traitement des commentaires 	*/
/************************************************************************/
/* 12-02-87 11:14 (pb&phd) :	Utilisation de sxnextchar()	 	*/
/************************************************************************/
/* 30-01-87 14:08 (pb) :	EMPTY_STE, sxstr2save		 	*/
/************************************************************************/
/* 20-01-87 13:40 (phd&pb) :	previous_char est passe a sxsrcpush	*/
/************************************************************************/
/* 16-01-87 09:17 (phd&pb) :	Bug dans comments_put	 		*/
/************************************************************************/
/* 07-01-87 11:54 (phd&pb) :	Erase prend en compte Mark		*/
/************************************************************************/
/* 06-01-87 13:46 (phd) :	Definition de BUGF et BUGA lorsque BUG	*/
/************************************************************************/
/* 03-12-86 11:29 (pb) :	Calcul de la classe a chaque appel	*/
/************************************************************************/
/* 01-12-86 15:12 (pb) :	Suppression de sxccc	   		*/
/************************************************************************/
/* 01-12-86 11:12 (pb&phd) :	Appel de sxcheck_magic_number		*/
/************************************************************************/
/* 16-11-86 15:00 (pb) :	Separation du traitement des erreurs	*/
/************************************************************************/
/* 23-10-86 15:00 (pb) :	Introduction de current_class et	*/
/*				class_in_la (register)	  	*/
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

#ifndef VARIANT_32
char WHAT_SXSCANNER[] = "@(#)SYNTAX - $Id: sxscanner.c 1041 2008-02-15 08:40:14Z rlacroix $" WHAT_DEBUG;
#else
char WHAT_SXSCANNER[] = "@(#)SYNTAX - $Id: sxscanner.c 1041 2008-02-15 08:40:14Z rlacroix $ SXSCANNER_32" WHAT_DEBUG;
#endif
static char	ME [] = "SCANNER";

#include <ctype.h>
#include "sxsstmt.h"


#if	BUG
#	undef	sxnextchar
#	define	sxnextchar sxnext_char
#endif

#define IS_IN_LA  (sxsvar.SXS_tables.S_transition_matrix [current_state] [1] != 0)


static SXINT	ind_incl, ind_incl_use;
struct lv	*sxsvar_include;



static SXVOID	comments_put (char *str, SXINT lgth)
{
    SXINT	lgth_comment;

    if (sxsvar.sxlv.terminal_token.comment == NULL)
	strcpy (sxsvar.sxlv.terminal_token.comment = (char*) sxalloc (lgth + 1, sizeof (char)), str);
    else {
	lgth_comment = strlen (sxsvar.sxlv.terminal_token.comment);
	strcpy ((sxsvar.sxlv.terminal_token.comment = (char*) sxrealloc (sxsvar.sxlv.terminal_token.comment, lgth_comment +
	     lgth + 1, sizeof (char))) + lgth_comment, str);
    }
}




#define ts_put(c)							\
(    (sxsvar.sxlv.ts_lgth+2 == sxsvar.sxlv_s.ts_lgth_use)			\
	? sxsvar.sxlv_s.token_string = (char *) sxrealloc (sxsvar.sxlv_s.	\
	    token_string, sxsvar.sxlv_s.ts_lgth_use *= 2, sizeof (char))	\
	: NULL,								\
     sxsvar.sxlv_s.token_string [sxsvar.sxlv.ts_lgth ++] = (char)c		\
)


#define ts_null()	(sxsvar.sxlv_s.token_string [sxsvar.sxlv.ts_lgth] = NUL)

#define char_to_class(c) (sxsvar.SXS_tables.S_char_to_simple_class[c])




VOID	sxscan_it (void)
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

	    if ((*sxsvar.SXS_tables.recovery) (ACTION, current_state, &class)) {
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
		sxsvar.sxlv.terminal_token.string_table_entry = EMPTY_STE;
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
	    sxsvar.sxlv.terminal_token.string_table_entry = EMPTY_STE;
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
		(VOID) (*sxsvar.SXS_tables.scanact) (ACTION, sxsvar.sxlv.include_action);

	    goto ccnewlextok; /* sur l'ancien source */
	}

	break;

    case HashReduce:
    case HashReducePost:
	if ((sxsvar.sxlv.terminal_token.lahead =
	     (*sxsvar.SXS_tables.check_keyword) (sxsvar.sxlv_s.token_string, sxsvar.sxlv.ts_lgth)) == 0) {
	    /* not a keyword */
	    sxsvar.sxlv.terminal_token.lahead = NEXT (stmt);

	    if (sxsvar.sxlv.terminal_token.lahead == 0) {
		/* E.R. ne donnant qu'une recherche en table hash ayant echouee */
		sxsvar.sxlv.mode.errors_nb++;

		if (!sxsvar.sxlv.mode.is_silent)
		    sxput_error (sxsvar.sxlv.terminal_token.source_index,
				 sxsvar.SXS_tables.S_global_mess [2],
				 sxsvar.sxtables->err_titles [2]);
		goto newlextok;
	    }

	    sxsvar.sxlv.terminal_token.string_table_entry = sxstr2save (sxsvar.sxlv_s.token_string, sxsvar.sxlv.ts_lgth);
	}
	else
	    sxsvar.sxlv.terminal_token.string_table_entry = EMPTY_STE;

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
	    BOOLEAN	is_prdct_satisfied = FALSE;

	    action_or_prdct_code--;

	    do {
		if ((++action_or_prdct_code)->is_system) {
		    switch (action_or_prdct_code->action_or_prdct_no) {
		    case IsTrue:
			is_prdct_satisfied = TRUE;
			break;

		    case IsFirstCol:
			is_prdct_satisfied = (IS_IN_LA ? sxsrcmngr.buffer [sxsrcmngr.labufindex - 1] : sxsrcmngr.
			     previous_char) == NEWLINE;
			break;

		    case NotIsFirstCol:
			is_prdct_satisfied = (IS_IN_LA ? sxsrcmngr.buffer [sxsrcmngr.labufindex - 1] : sxsrcmngr.
			     previous_char) != NEWLINE;
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
			    BOOLEAN	is_a_lookahead;

			    sxchar = (is_a_lookahead = IS_IN_LA) ? sxlanext_char () : sxlafirst_char ();

			    if (is_a_lookahead)
				sxlaback (1);

			    is_prdct_satisfied = sxchar == NEWLINE || sxchar == EOF;
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
			is_prdct_satisfied = (*sxsvar.SXS_tables.scanact) (PREDICATE, action_or_prdct_code->
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

		    if (sxsvar_include == NULL)
			sxsvar_include = (struct lv*) sxalloc ((ind_incl_use = 5), sizeof (struct lv));
		    else if (ind_incl >= ind_incl_use)
			sxsvar_include = (struct lv*) sxrealloc (sxsvar_include, ind_incl_use += 5, sizeof (struct lv));

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
		    (VOID) (*sxsvar.SXS_tables.scanact) (ACTION, action_or_prdct_code->action_or_prdct_no);
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
	    (VOID) (*sxsvar.SXS_tables.scanact) (ACTION, action_or_prdct_code->action_or_prdct_no);

	if (sxsvar.sxlv.terminal_token.lahead == 0) {
	    /* Dans tous les cas, la post-action s'est occupee des commentaires eventuels. */
	    if (sxsvar.sxlv.terminal_token.string_table_entry == ERROR_STE)
		/* La post-action a decide' de retourner a l'appelant sans rien faire... */
		return;

	    /* La post-action a decide' de reboucler... */
	    goto ccnewlextok;
	}

    default:
	break;
    }

    sxput_token(sxsvar.sxlv.terminal_token);

# if BUG

    printf ("\n\t\t\ttoken = (%ld, \"%s\")\n",
	    sxsvar.sxlv.terminal_token.lahead,
	    sxttext (sxsvar.sxtables, sxsvar.sxlv.terminal_token.lahead));
    printf ("\t\t\tstring = \"%s\"\n",
	    sxsvar.sxlv.terminal_token.string_table_entry == EMPTY_STE ? "" : sxstrget (sxsvar.lv.terminal_token.string_table_entry));
    printf ("\t\t\tsource_index = (%s, %lu, %lu)\n",
	    sxsvar.sxlv.terminal_token.source_index.file_name,
	    sxsvar.sxlv.terminal_token.source_index.line,
	    sxsvar.sxlv.terminal_token.source_index.column);

    if (sxsvar.sxlv.terminal_token.comment != NULL) {
	printf ("\t\t\tcomment = \"%s\"\n", sxsvar.sxlv.terminal_token.comment);
    }


# endif

}




/* VARARGS1 */

SXINT sxscanner (SXINT what_to_do, struct sxtables *arg)
{
    SXINT i;

    switch (what_to_do) {
    case OPEN:
	/* new language: new tables, */
	/* the global variable "sxsvar" must have been saved in case of */
	/* recursive invocation */

	sxcheck_magic_number (arg->magic, SXMAGIC_NUMBER, ME);
	sxsvar.sxtables = arg;
	sxsvar.SXS_tables = arg->SXS_tables;
	sxsvar.sxlv_s.token_string = (char*) sxalloc (sxsvar.sxlv_s.ts_lgth_use = 120, sizeof (char));
	sxsvar.sxlv_s.counters = (sxsvar.SXS_tables.S_counters_size != 0) ? (long*) sxalloc (sxsvar.SXS_tables.
	     S_counters_size, sizeof (long)) : NULL;

	sxsvar.sxlv.mode.errors_nb = 0;
	sxsvar.sxlv.mode.is_silent = FALSE;
	sxsvar.sxlv.mode.with_system_act = TRUE;
	sxsvar.sxlv.mode.with_user_act = TRUE;
	sxsvar.sxlv.mode.with_system_prdct = TRUE;
	sxsvar.sxlv.mode.with_user_prdct = TRUE;

	char_to_class (EOF) = 2 /* EOF class */ ;
	break;

    case INIT:
	/* Beginning of new source */
	sxsvar.sxlv_s.include_no = 0;

	if (sxsvar.sxlv_s.counters != NULL) {
	    for (i = sxsvar.SXS_tables.S_counters_size - 1; i >= 0; i--)
		sxsvar.sxlv_s.counters[i] = 0;
	}

	/* read the character that will be current next scan */
	sxnext_char ();
	break;

    case ACTION:
	sxscan_it ();
	break;

    case FINAL:
	/* End of source */
	break;

    case CLOSE:
	/* End of language */
	sxfree (sxsvar.sxlv_s.token_string), sxsvar.sxlv_s.token_string = NULL;

	if (sxsvar.sxlv_s.counters != NULL)
	    sxfree (sxsvar.sxlv_s.counters), sxsvar.sxlv_s.counters = NULL;

	if (sxsvar_include != NULL)
	    sxfree (sxsvar_include), sxsvar_include = NULL;

	(*sxsvar.SXS_tables.recovery) (CLOSE);
	break;

    default:
	fprintf (sxstderr, "The function \"sxscanner\" is out of date with respect to its specification.\n");
	sxexit(1);
	    /*NOTREACHED*/
    }

    return 0;
}
