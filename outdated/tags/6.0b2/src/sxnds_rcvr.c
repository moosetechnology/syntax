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
/* 20030507 09:09 (phd):	Modifs pour calmer le compilo SGI	*/
/************************************************************************/
/* 14-05-93 10:25 (pb):		Version non deterministe		*/
/************************************************************************/
/* 11-05-93 17:25 (pb):		ref a is_predicate changee en ref a	*/
/*				kind == IsPredicate			*/
/************************************************************************/
/* 03-11-92 12:06 (pb):		Bug ds "check" si "is_la"		*/
/************************************************************************/
/* 08-04-92 15:45 (pb):		Utilisation de sxsvar.sxlv.mode		*/
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



static char	ME [] = "NDS_RCVR";

#include        "sxcommon.h"
#include "sxsstmt.h"
#include "sxnd.h"

#ifndef VARIANT_32
char WHAT_SXNDS_RECOVERY[] = "@(#)SYNTAX - $Id: sxnds_rcvr.c 617 2007-06-11 08:24:34Z rlacroix $" WHAT_DEBUG;
#else
char WHAT_SXNDS_RECOVERY[] = "@(#)SYNTAX - $Id: sxnds_rcvr.c 617 2007-06-11 08:24:34Z rlacroix $ SXNDS_RCVR_32" WHAT_DEBUG;
#endif

extern SXSTMI   predicate_processing ();
extern SXSTMI   action_processing ();
extern int      clone_active_scanner (SXSTMI stmt);

#define char_to_class(c) (sxndsvar.SXS_tables.S_char_to_simple_class[c])


static VOID	allouer_correction ()
{
    int	nmax;
    static int paths_foreach [] = {0, 0, 1, 0, 0, 0};

    if (sxndsvar.rcvr.source_char == NULL) {
	nmax = sxndsvar.SXS_tables.S_nmax + 1; /* au moins 1 */
	sxndsvar.rcvr.source_char = (SHORT*) sxalloc (nmax + 1, sizeof (SHORT));
	sxndsvar.rcvr.source_classes = (int*) sxalloc (nmax + 1, sizeof (int));
	/* place pour 4 X ... */
	sxndsvar.rcvr.current.string = (SHORT*) sxalloc (nmax + 1 + 4, sizeof (SHORT));
	sxndsvar.rcvr.may_be.string = (SHORT*) sxalloc (nmax + 1 + 4, sizeof (SHORT));
	sxndsvar.rcvr.class_to_insert =
	    (SHORT*) sxcalloc ((unsigned long int)sxndsvar.SXS_tables.S_last_simple_class_no + 1, sizeof (SHORT));
	XxYxZ_alloc (&sxndsvar.rcvr.paths, "paths", 20, 1, paths_foreach, NULL, NULL);
    }

    if (sxndsvar.rcvr.state_set == NULL) {
	sxndsvar.rcvr.state_set = sxba_calloc (sxndsvar.SXS_tables.S_last_state_no + 1);
	X_alloc (&sxndsvar.rcvr.stmt_set, "rcvr.stmt_set", 5, 1, NULL, NULL);
    }
}


static VOID	recovery_free ()
{
    if (sxndsvar.rcvr.source_char != NULL) {
	XxYxZ_free (&sxndsvar.rcvr.paths);
	sxfree (sxndsvar.rcvr.class_to_insert);
	sxfree (sxndsvar.rcvr.may_be.string);
	sxfree (sxndsvar.rcvr.current.string);
	sxfree (sxndsvar.rcvr.source_classes);
	sxfree (sxndsvar.rcvr.source_char), sxndsvar.rcvr.source_char = NULL;
    }

    if (sxndsvar.rcvr.state_set != NULL) {
	sxfree (sxndsvar.rcvr.state_set), sxndsvar.rcvr.state_set = NULL;
	X_free (&sxndsvar.rcvr.stmt_set);
    }
}





/* *********** */
/* CORRECTION  */
/* *********** */

static BOOLEAN	is_insertable (tm_line, stmt, sample)
    int		*sample;
    SXSTMI	*tm_line, stmt;
{
    /* cherche s'il existe un caractere (sample) n'appartenant pas a
       S_dont_insert et ayant une transition vers stmt.
       Memoize function. */
    int	i, class_no;

    for (class_no = 2; class_no <= sxndsvar.SXS_tables.S_last_simple_class_no; class_no++) {
	/* On saute illegal_chars */
	if (stmt == tm_line [class_no]) {
	    if ((i = sxndsvar.rcvr.class_to_insert [class_no]) == 0) {
		/* pas encore calcule', on le fait. */
		for (i = 0; i <= sxndsvar.SXS_tables.S_last_char_code; i++) {
		    if (class_no == char_to_class (i)
			&& !sxgetbit (sxndsvar.SXS_tables.S_no_insert, i)) {
			i++;
			break;
		    }
		}

		sxndsvar.rcvr.class_to_insert [class_no] = i;
	    }

	    if (i > 0) {
		*sample = i - 1;
		return TRUE;
	    }
	}
    }

    return FALSE;
}


static void walk ();

static void look (stmt)
    SXSTMI	stmt;
{
    int					look_index;
    struct SXS_action_or_prdct_code	*action_or_prdct_code;

    switch (CODOP (stmt)) {
    case FirstLookAhead:
    case NextLookAhead:
	walk (NEXT (stmt));
	break;

    case ActPrdct:
	action_or_prdct_code = &sxndsvar.SXS_tables.SXS_action_or_prdct_code [NEXT (stmt)];
		    
	if (action_or_prdct_code->kind == IsPredicate) {
	    action_or_prdct_code--;

	    do {
		action_or_prdct_code++;
		look (action_or_prdct_code->stmt);
	    } while (!(action_or_prdct_code->is_system
		     && action_or_prdct_code->action_or_prdct_no == IsTrue));

	    break;
	}

	if (action_or_prdct_code->kind == IsNonDeter) {
	    action_or_prdct_code--;

	    do {
		action_or_prdct_code++;
		X_set (&sxndsvar.rcvr.stmt_set, (int) action_or_prdct_code->stmt, &look_index);
	    } while (action_or_prdct_code->action_or_prdct_no > 0);

	    break;
	}
		     
	/* On suppose cas HashReducePost ou ReducePost */
	/* on continue en sequence */
	stmt = action_or_prdct_code->stmt;

    case SameState:
    case State:
    case Reduce:
    case HashReduce:
    case HashReducePost:
    case ReducePost:
	X_set (&sxndsvar.rcvr.stmt_set, (int) stmt, &look_index);
	break;
				   
    case Error:
	break;
    default: /* pour faire taire gcc -Wswitch-default */
#if EBUG
      sxtrap(ME,"unknown switch case #1");
#endif
      break;
    }
}

static void walk (state)
    int 	state;
{
    /* state est un etat en look-ahead */
    /* On collecte ds result_set l'ensemble des stmt permettant de "sortir"
       du graphe de look-ahead enracine' en state. */
    int class;

    if (SXBA_bit_is_reset_set (sxndsvar.rcvr.state_set, state)) {
	for (class = 2; class <= sxndsvar.SXS_tables.S_last_simple_class_no; class++) {
	    look (sxndsvar.SXS_tables.S_transition_matrix [state] [class]);
	}
    }
}


static void look_ahead_extract (current_state_no)
    int current_state_no;
{
    SXSTMI	stmt;
    int 	x, new_index;

    if (sxndsvar.rcvr.state_set == NULL) {
	sxndsvar.rcvr.state_set = sxba_calloc (sxndsvar.SXS_tables.S_last_state_no + 1);
	X_alloc (&sxndsvar.rcvr.stmt_set, "rcvr.stmt_set", 5, 1, NULL, NULL);
    }
    else {
	sxba_empty (sxndsvar.rcvr.state_set);
	X_clear (&sxndsvar.rcvr.stmt_set);
    }

    walk (current_state_no);

    for (x = X_top (sxndsvar.rcvr.stmt_set); x > 0; x--) {
	stmt = (SXSTMI) X_X (sxndsvar.rcvr.stmt_set, x);
			
	if (x > 1) {
	    new_index = clone_active_scanner (stmt);
	    SXBA_1_bit (sxndsvar.active_ndlv_set, new_index);
	}
	else {
	    /* On reutilise le scanner courant comme dernier clone */
	    sxndsvar.current_ndlv->stmt = stmt;
	    SXBA_1_bit (sxndsvar.active_ndlv_set, sxndsvar.current_ndlv->my_index);
	}
    }
}





static void ndrcvr_one_scan ()
{
    int					current_index, codop;
    SXSTMI				stmt;
    struct SXS_action_or_prdct_code	*action_or_prdct_code;

    while ((current_index = sxba_scan_reset (sxndsvar.active_ndlv_set, -1)) != -1) {
	sxndsvar.current_ndlv = sxndsvar.ndlv + current_index;
	stmt = sxndsvar.current_ndlv->stmt;

	if (SCAN (stmt)) {
	    SXBA_1_bit (sxndsvar.read_char_ndlv_set, current_index);
	}
	else {
	    switch (codop = CODOP (stmt)) {
	    case ActPrdct:
/* Les actions et predicats sont executes pendant le traitement des erreurs
   (toutes les variables sont locales), l'utilisateur etant prevenu (macro
   accedant a une variable globale) du mode d'execution (normal, erreur
   look-ahead). A la construction, la detection de l'execution d'une action en
   look_ahead produit du non-determinisme. */

		action_or_prdct_code =
		    &sxndsvar.SXS_tables.SXS_action_or_prdct_code [NEXT (stmt)];
		codop = CODOP (action_or_prdct_code->stmt);
		
		if (action_or_prdct_code->kind == IsAction) {
		    if (codop != ReducePost && codop != HashReducePost) {
			/* transition vers une action ou un groupe de predicats */
			stmt = action_processing (action_or_prdct_code);
		    }
		}
		else if (action_or_prdct_code->kind == IsPredicate) {
		    /* transition vers une action ou un groupe de predicats */
		    stmt = predicate_processing (action_or_prdct_code);
		}
		else {
		    /* Non determinism processing */
		    /* Au moins 2 possibilites. */
		    do {
			int new_index;
			
			new_index = clone_active_scanner (action_or_prdct_code->stmt);
			SXBA_1_bit (sxndsvar.active_ndlv_set, new_index);
		    } while ((++action_or_prdct_code)->action_or_prdct_no > 0);
		    
		    /* On reutilise le scanner courant comme dernier clone */
		    stmt = action_or_prdct_code->stmt;
		}
		
		if (codop != ReducePost && codop != HashReducePost) {
		    sxndsvar.current_ndlv->stmt = stmt;
		    SXBA_1_bit (sxndsvar.active_ndlv_set, current_index);
		    break;
		}
		/* else : en sequence */

	    case Reduce:
	    case HashReduce:
	    case ReducePost:
	    case HashReducePost:
		/* On n'execute pas les post_actions */
		if (NEXT (stmt) == (SXSTMI)(sxndsvar.SXS_tables.S_termax)) {
		    SXBA_1_bit (sxndsvar.read_char_ndlv_set, current_index);
		    break;
		}

		stmt = 1;
		
	    case SameState: /* ?? */
	    case State:
		sxndsvar.current_ndlv->stmt =
		    sxndsvar.SXS_tables.S_transition_matrix
			[NEXT (stmt)] [sxndsvar.current_ndlv->terminal_token.string_table_entry];
		SXBA_1_bit (sxndsvar.active_ndlv_set, current_index);
		break;
		
	    case FirstLookAhead:
		/* On suit toutes les actions immediates. */
		look_ahead_extract (NEXT (stmt));
		break;
		
	    case Error:
		sxindex_release (sxndsvar.index_header, current_index); 
		break;
		
	    default:
		/* impossible */
		sxtrap (ME, "ndrcvr_one_scan");
	    }
	}
    }
}



static BOOLEAN backward_traversal (node, stmt, j)
    int stmt, node, j;
{
    int triple, source_index, sample, c;

    if ((source_index = sxndsvar.rcvr.current.model [j]) < 0) {
	/* X */
	if (is_insertable (sxndsvar.SXS_tables.S_transition_matrix [NEXT (node)],
			   (SXSTMI) stmt,
			   &sample)) {
	    /* on met sample en j */
	    sxndsvar.rcvr.current.string [j] = sample;
	}
	else {
	    if (sxndsvar.rcvr.is_may_be)
		/* Deja un on abandonne le chemin */
		return FALSE;

#if 0
	    /* on met -sample en j */ /* FAUX : sample est non positionne' si is_insertable() retourne FALSE ... */
	    sxndsvar.rcvr.current.string [j] = -sample;
#endif /* 0 */
	    /* ... mais je ne sais pas le corriger !!  Je fais donc un truc pour que gcc -O -Wuninitialized ne crie pas (a juste titre) */
	    sxndsvar.rcvr.current.string [j] = -((SXSHORT)(unsigned char)'a');
	}
    }
    else {
	sxndsvar.rcvr.current.string [j] = sxndsvar.rcvr.source_char [source_index];
    }

    if (--j == 0) {
	/* On a un chemin */
	for (j = sxndsvar.rcvr.current.model [0]; j > 0; j--) {
	    if (sxndsvar.rcvr.current.string [j] < 0)
		break;
	}

	if (j == 0)
	    return sxndsvar.rcvr.is_done = TRUE;

	for (j = sxndsvar.rcvr.current.model [0]; j > 0; j--) {
	    if ((c = sxndsvar.rcvr.current.string [j]) < 0)
		c = -c;

	    sxndsvar.rcvr.may_be.string [j] = c;
	}

	sxndsvar.rcvr.is_may_be = TRUE;
	return FALSE;
    }

    
    XxYxZ_Zforeach (sxndsvar.rcvr.paths, node, triple) {
	if (backward_traversal (XxYxZ_X (sxndsvar.rcvr.paths, triple),
				XxYxZ_Y (sxndsvar.rcvr.paths, triple),
				j))
	    return TRUE;
    }

    return FALSE;
}


static void	current_model_no_processing ()
{
    /* Verifie si le source valide le modele current. */

    int		current_index, new_index, state_no, source_index, source_class, current_model_no_processing_index, top, i, j, x;
    SXSTMI	*tm_line, stmt;
    struct SXS_action_or_prdct_code	*action_or_prdct_code;

    sxinitialise(top); /* pour faire taire "gcc -Wuninitialized" */
    sxinitialise(new_index); /* pour faire taire "gcc -Wuninitialized" */
    XxYxZ_clear (&sxndsvar.rcvr.paths);
    /* sxndsvar.read_char_ndlv_set == PHI */
    current_index = -1;

    while ((current_index = sxba_scan (sxndsvar.rcvr.ndlv_set, current_index)) >= 0) {
	/* pour clone_active_scanner () */
	sxndsvar.current_ndlv = sxndsvar.ndlv + current_index;
	new_index = clone_active_scanner (0);
	SXBA_1_bit (sxndsvar.read_char_ndlv_set, new_index);
    }
    
    for (j = 1; j <= sxndsvar.rcvr.current.model [0]; j++) {
	source_index = sxndsvar.rcvr.current.model [j];
	current_index = -1;

	while ((current_index =
		sxba_scan_reset (sxndsvar.read_char_ndlv_set, current_index)) >= 0) {
	    sxndsvar.current_ndlv = sxndsvar.ndlv + current_index;
	    tm_line =
		sxndsvar.SXS_tables.S_transition_matrix [sxndsvar.current_ndlv->current_state_no];

	    if (source_index < 0) {
		/* X */
		X_clear (&sxndsvar.rcvr.stmt_set);

		for (i = 3; i <= sxndsvar.SXS_tables.S_last_simple_class_no; i++) {
		    /* On saute illegal_chars et EOF */
		    stmt = tm_line [i];

		    if (CODOP (stmt) != Error)
			if (!X_set (&sxndsvar.rcvr.stmt_set, (int) stmt, &current_model_no_processing_index)) {
			    /* 1ere occurrence de stmt */
			    new_index = clone_active_scanner (stmt);
			    sxndsvar.ndlv [new_index].terminal_token.lahead = (int) stmt;
			    /* Pour aller d'etat en etat ds l'automate si pas de scan. */
			    sxndsvar.ndlv [new_index].terminal_token.string_table_entry = i;
			    SXBA_1_bit (sxndsvar.active_ndlv_set, new_index);
			}
		}
		
		sxindex_release (sxndsvar.index_header, current_index); 
	    }
	    else {
		source_class = sxndsvar.rcvr.source_classes [source_index];
		sxndsvar.current_ndlv->stmt =
		    sxndsvar.current_ndlv->terminal_token.lahead =
			tm_line [source_class]; 
		sxndsvar.ndlv [new_index].terminal_token.string_table_entry = source_class;
		SXBA_1_bit (sxndsvar.active_ndlv_set, current_index);
	    }
	}

	ndrcvr_one_scan ();

	if ((current_index = sxba_scan (sxndsvar.read_char_ndlv_set, -1)) >= 0) {
	    top = XxYxZ_top (sxndsvar.rcvr.paths);

	    do {
		sxndsvar.current_ndlv = sxndsvar.ndlv + current_index;
		state_no = sxndsvar.current_ndlv->current_state_no;
		stmt = sxndsvar.current_ndlv->stmt;

		while ((CODOP (stmt)) == ActPrdct) {
		    /* On suit les sequences d'actions (au plus 1 scan par sequence). */
		    action_or_prdct_code =
			&sxndsvar.SXS_tables.SXS_action_or_prdct_code [NEXT (stmt)];
		
		    if (action_or_prdct_code->kind != IsAction)
			sxtrap (ME, "current_model_no_processing");

		    stmt = action_or_prdct_code->stmt;
		}
		    
		switch (CODOP (stmt)) {
		case Reduce:
		case HashReduce:
		case ReducePost:
		case HashReducePost:
		    if (NEXT (stmt) == (SXSTMI)(sxndsvar.SXS_tables.S_termax)) {
			/* Reconnaissance du token EOF */
			if (backward_traversal (STMT (0, 0, j, state_no),
						sxndsvar.current_ndlv->terminal_token.lahead,
						j))
			    /* C'est une bonne correction */
			    return;
		    }
		    
		    stmt = 1;
		    break;
		    
		case SameState:
		case State:
		    break;
		    
		default:
		    /* impossible */
		    sxtrap (ME, "current_model_no_processing");
		}

		sxndsvar.current_ndlv->current_state_no = NEXT (stmt);
		/* On code le couple (j, state_no) comme un STMT du scanner. */

		XxYxZ_set (&sxndsvar.rcvr.paths,
			   STMT (0, 0, j, state_no),
			   sxndsvar.current_ndlv->terminal_token.lahead,
			   STMT (0, 0, j + 1, sxndsvar.current_ndlv->current_state_no),
			   &current_model_no_processing_index);
	    } while ((current_index =
		      sxba_scan (sxndsvar.read_char_ndlv_set, current_index)) >= 0);
	}
	else
	    break;
    }

    /* Liberation des scanners... */
    current_index = -1;

    while ((current_index = sxba_scan_reset (sxndsvar.read_char_ndlv_set, current_index)) >= 0)
	sxindex_release (sxndsvar.index_header, current_index);

    if (j > sxndsvar.rcvr.current.model [0]) {
	/* Au moins une reconnaissance */
	/* On extrait un chemin */

	for (x = XxYxZ_top (sxndsvar.rcvr.paths); x > top; x--) {
	    if (backward_traversal (XxYxZ_X (sxndsvar.rcvr.paths, x),
				    XxYxZ_Y (sxndsvar.rcvr.paths, x),
				    j - 1))
		break;
	}
    }
}


static int	has_dont_delete ()
{
    /* Regarde si le choix courant n'utilise pas des caracteres de
       dont_delete. */
    int i, l, x, kind;

    kind = 0;
    l = sxndsvar.rcvr.current.model [0];
    x = sxndsvar.rcvr.current.model [l];

    while (--x >= 0) {
	i = l;

	while (--i > 0) {
	    if (sxndsvar.rcvr.current.model [i] == x)
		break;
	}

	if (i == 0) {
	    /* x n'existe pas dans le modele */
	    if (sxndsvar.rcvr.source_char [x] == EOF)
		return 2;

	    if (sxgetbit (sxndsvar.SXS_tables.S_no_delete, sxndsvar.rcvr.source_char [x]))
		kind = 1;
	}
    }

    return kind;
}




static VOID tryacorr ()
{
    int j, kind;

    sxndsvar.rcvr.is_done = sxndsvar.rcvr.is_may_be = FALSE;

    for (sxndsvar.rcvr.current.model_no = 1;
	 sxndsvar.rcvr.current.model_no <= sxndsvar.SXS_tables.S_nbcart;
	 sxndsvar.rcvr.current.model_no++) {
	sxndsvar.rcvr.current.model =
	    &(sxndsvar.SXS_tables.S_lregle [sxndsvar.rcvr.current.model_no] [0]);

	if ((kind = has_dont_delete ()) < 2) {
	    /* Le modele courant n'essaiera pas de supprimer EOF. */
	    current_model_no_processing ();
	    
	    if (sxndsvar.rcvr.is_done) {
		if (kind == 1) {
		    /* Le modele courant a supprimer des caracteres de dont_delete */
		    if (!sxndsvar.rcvr.is_may_be) {
			sxndsvar.rcvr.is_may_be = TRUE;
			sxndsvar.rcvr.may_be.model = sxndsvar.rcvr.current.model;
			sxndsvar.rcvr.may_be.model_no = sxndsvar.rcvr.current.model_no;
			
			for (j = sxndsvar.rcvr.current.model [0]; j > 0; j--)
			    sxndsvar.rcvr.may_be.string [j] = sxndsvar.rcvr.current.string [j];
		    }
		    /* else on conserve l'autre */
		    
		    sxndsvar.rcvr.is_done = FALSE;
		}
	    }
	    
	    if (sxndsvar.rcvr.is_done)
		return;
	}
    }

    if (sxndsvar.rcvr.is_may_be) {
	sxndsvar.rcvr.is_done = TRUE;
	sxndsvar.rcvr.current.model = sxndsvar.rcvr.may_be.model;
	sxndsvar.rcvr.current.model_no = sxndsvar.rcvr.may_be.model_no;
			
	for (j = sxndsvar.rcvr.may_be.model [0]; j > 0; j--)
	    sxndsvar.rcvr.current.string [j] = sxndsvar.rcvr.may_be.string [j];
    }
}



static BOOLEAN	nds_recovery ()
{
    /* Une correction impliquant la suppression d'un caractere dont la classe
       simple est dans S_dont_delete ne peut etre validee que s'il n'existe
       pas de modele (meme moins prioritaire) valide. Une correction
       impliquant l'insertion d'un caractere dont la classe simple est dans
       S_dont_insert ne peut etre validee que s'il n'existe pas de modele
       (meme moins prioritaire) valide. */

    /* Les erreurs en look-ahead sont traitees (ds le scanner) par le non
       determinisme. L'ensemble des possibilites associees au sous-automate. */

    int		i, j;

    allouer_correction ();

    sxndsvar.rcvr.source_char [0] = sxsrcmngr.current_char;
    sxndsvar.rcvr.source_classes [0] = char_to_class (sxndsvar.rcvr.source_char [0]);
    sxndsvar.rcvr.source_char [1] = (sxndsvar.rcvr.source_char [0] == EOF)
    ? EOF
    : sxlafirst_char ();
    sxndsvar.rcvr.source_classes [1] = char_to_class (sxndsvar.rcvr.source_char [1]);


/* lecture des caracteres en look_ahead */

    for (i = 2; i <= sxndsvar.SXS_tables.S_nmax; i++) {
	sxndsvar.rcvr.source_classes [i] =
	    char_to_class (sxndsvar.rcvr.source_char [i] =
			   (sxndsvar.rcvr.source_char [i - 1] == EOF)
			   ? EOF
			   : sxlanext_char ());
    }

    tryacorr ();


/* ecriture des messages d'erreur */

    if (!sxndsvar.rcvr.is_done && sxndsvar.rcvr.source_char [0] == EOF) {
	if (!sxsvar.sxlv.mode.is_silent)
	    sxput_error (sxsrcmngr.source_coord,
			 sxndsvar.SXS_tables.S_global_mess [5],
			 sxndsvar.sxtables->err_titles [2]);

	sxlaback (sxndsvar.SXS_tables.S_nmax);
	return FALSE;
    }

    if (!sxsvar.sxlv.mode.is_silent) {
	register SHORT	sxchar;

	if (!sxndsvar.rcvr.is_done) {
	    /* No Correction => Recovery */
	    sxchar = sxndsvar.rcvr.source_char [0];
	    sxput_error (sxsrcmngr.source_coord,
			 sxndsvar.SXS_tables.S_global_mess [1],
			 sxndsvar.sxtables->err_titles [2],
			 (sxchar == EOF ?
			  sxndsvar.SXS_tables.S_global_mess [4] :
			  (sxchar == NEWLINE ?
			   sxndsvar.SXS_tables.S_global_mess [3] :
			   sxc_to_str [sxchar])));
	}
	else {
	    char	*msg_params [5];

	    for (j = sxndsvar.SXS_tables.SXS_local_mess
		 [sxndsvar.rcvr.current.model_no].S_param_no - 1;
		 j >= 0;
		 j--) {
		i = sxndsvar.SXS_tables.SXS_local_mess
		    [sxndsvar.rcvr.current.model_no].S_param_ref [j];
		sxchar = i < 0 /* X */ 
		    ? sxndsvar.rcvr.current.string [1]
			: sxndsvar.rcvr.source_char [i];
		msg_params [j] = (sxchar == EOF
				  ? sxndsvar.SXS_tables.S_global_mess [4]
				  : (sxchar == NEWLINE
				     ? sxndsvar.SXS_tables.S_global_mess [3]
				     : sxc_to_str [sxchar]));
	    }

	    sxput_error (sxsrcmngr.source_coord,
			 sxndsvar.SXS_tables.SXS_local_mess [sxndsvar.rcvr.current.model_no].S_string,
			 sxndsvar.sxtables->err_titles [1],
			 msg_params [0],
			 msg_params [1],
			 msg_params [2],
			 msg_params [3],
			 msg_params [4]);
	}
    }

/* Les seuls modeles acceptes sont :
   1 2 3 ...
   X 0 1 2 ...
   X 1 2 3 ...
   Pour etre plus general il suffirait d'avoir une procedure du source manager
   qui permette le remplacement (en look-ahead) d'une sous-chaine par une
   autre. */

    if (sxndsvar.rcvr.is_done && sxndsvar.rcvr.current.model [1] < 0) {
	/* les seuls modeles corrects sont de la forme : X n n+1 n+2 .... ou
	   n>=0, p p+1 p+2 .... ou p>=1  */
	/* sxndsvar.rcvr.current.model [2] = 0 => ajout d'un caractere avant le courant, sinon
	   remplacement du caractere */

	if (sxndsvar.rcvr.current.model [2] == 0)
	    sxX (sxndsvar.rcvr.current.string [1]);
	else
	    sxsrcmngr.current_char = sxndsvar.rcvr.current.string [1];
    }
    else {
	/* suppression du caractere courant*/
	sxsrcmngr.current_char = sxsrcmngr.previous_char;
	sxnext_char ();
    }

    return TRUE; /* Ajoute' au pif le 14/05/2003 */
}



BOOLEAN		sxndsrecovery (sxndsrecovery_what)
    int		sxndsrecovery_what;
{
    switch (sxndsrecovery_what) {
    case ACTION:
	return nds_recovery ();

    case SXERROR:
	/* Error in look-ahead */
	look_ahead_extract (sxndsvar.ndlv->current_state_no);
	break;

    case CLOSE:
	recovery_free ();
	break;

    default:
	fprintf (sxstderr, "The function \"sxndsrecovery\" is out of date with respect to its specification.\n");
	abort ();
    }

    return TRUE;
}
