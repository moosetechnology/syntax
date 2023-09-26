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

static char	ME [] = "NDS_RCVR";

#include        "sxversion.h"
#include        "sxcommon.h"
#include "sxsstmt.h"
#include "sxnd.h"

#ifndef VARIANT_32
char WHAT_SXNDS_RECOVERY[] = "@(#)SYNTAX - $Id: sxnds_rcvr.c 2429 2023-01-18 13:45:40Z garavel $" WHAT_DEBUG;
#else
char WHAT_SXNDS_RECOVERY32[] = "@(#)SYNTAX - $Id: sxnds_rcvr.c 2429 2023-01-18 13:45:40Z garavel $ SXNDS_RCVR_32" WHAT_DEBUG;
#endif

extern SXSTMI   predicate_processing (struct SXS_action_or_prdct_code *action_or_prdct_code);
extern SXSTMI   action_processing (struct SXS_action_or_prdct_code *action_or_prdct_code);
extern SXINT      clone_active_scanner (SXSTMI stmt);

#define char_to_class(c) (sxndsvar.SXS_tables.S_char_to_simple_class[c])


static SXVOID	allouer_correction (void)
{
    SXINT	nmax;
    static SXINT paths_foreach [] = {0, 0, 1, 0, 0, 0};

    if (sxndsvar.rcvr.source_char == NULL) {
	nmax = sxndsvar.SXS_tables.S_nmax + 1; /* au moins 1 */
	sxndsvar.rcvr.source_char = (SXSHORT*) sxalloc (nmax + 1, sizeof (SXSHORT));
	sxndsvar.rcvr.source_classes = (SXINT*) sxalloc (nmax + 1, sizeof (SXINT));
	/* place pour 4 X ... */
	sxndsvar.rcvr.current.string = (SXSHORT*) sxalloc (nmax + 1 + 4, sizeof (SXSHORT));
	sxndsvar.rcvr.may_be.string = (SXSHORT*) sxalloc (nmax + 1 + 4, sizeof (SXSHORT));
	sxndsvar.rcvr.class_to_insert =
	    (SXSHORT*) sxcalloc ((SXUINT)sxndsvar.SXS_tables.S_last_simple_class_no + 1, sizeof (SXSHORT));
	XxYxZ_alloc (&sxndsvar.rcvr.paths, "paths", 20, 1, paths_foreach, (sxoflw0_t) NULL, NULL);
    }

    if (sxndsvar.rcvr.state_set == NULL) {
	sxndsvar.rcvr.state_set = sxba_calloc (sxndsvar.SXS_tables.S_last_state_no + 1);
	X_alloc (&sxndsvar.rcvr.stmt_set, "rcvr.stmt_set", 5, 1, (sxoflw0_t) NULL, NULL);
    }
}


static SXVOID	recovery_free (void)
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

static SXBOOLEAN	is_insertable (SXSTMI *tm_line, SXSTMI stmt, SXSHORT *sample)
{
    /* cherche s'il existe un caractere (sample) n'appartenant pas a
       S_dont_insert et ayant une transition vers stmt.
       Memoize function. */
    SXSHORT     i;
    SXINT	class_no;

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
		return SXTRUE;
	    }
	}
    }

    return SXFALSE;
}


static void walk (SXINT state);

static void look (SXSTMI stmt)
{
    SXINT					look_index;
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
		X_set (&sxndsvar.rcvr.stmt_set, (SXINT) action_or_prdct_code->stmt, &look_index);
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
	X_set (&sxndsvar.rcvr.stmt_set, (SXINT) stmt, &look_index);
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
    SXINT 	state;
{
    /* state est un etat en look-ahead */
    /* On collecte ds result_set l'ensemble des stmt permettant de "sortir"
       du graphe de look-ahead enracine' en state. */
    SXINT class;

    if (SXBA_bit_is_reset_set (sxndsvar.rcvr.state_set, state)) {
	for (class = 2; class <= sxndsvar.SXS_tables.S_last_simple_class_no; class++) {
	    look (sxndsvar.SXS_tables.S_transition_matrix [state] [class]);
	}
    }
}


static void look_ahead_extract (SXINT current_state_no)
{
    SXSTMI	stmt;
    SXINT 	x, new_index;

    if (sxndsvar.rcvr.state_set == NULL) {
	sxndsvar.rcvr.state_set = sxba_calloc (sxndsvar.SXS_tables.S_last_state_no + 1);
	X_alloc (&sxndsvar.rcvr.stmt_set, "rcvr.stmt_set", 5, 1, (sxoflw0_t) NULL, NULL);
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





static void ndrcvr_one_scan (void)
{
    SXINT					current_index, codop;
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
			SXINT new_index;
			
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
#if defined (__GNUC__) && (__GNUC__ - 0 >= 7)
		__attribute__ ((fallthrough));
#endif

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
		look_ahead_extract ((SXINT)NEXT (stmt));
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



static SXBOOLEAN backward_traversal (SXINT node, SXINT stmt, SXINT j)
{
    SXINT triple, source_index;
    SXSHORT sample, c;

    sxinitialise (sample);

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
		return SXFALSE;

#if 0
	    /* on met -sample en j */ /* FAUX : sample est non positionne' si is_insertable() retourne SXFALSE ... */
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
	    return sxndsvar.rcvr.is_done = SXTRUE;

	for (j = sxndsvar.rcvr.current.model [0]; j > 0; j--) {
	    if ((c = sxndsvar.rcvr.current.string [j]) < 0)
		c = -c;

	    sxndsvar.rcvr.may_be.string [j] = c;
	}

	sxndsvar.rcvr.is_may_be = SXTRUE;
	return SXFALSE;
    }

    
    XxYxZ_Zforeach (sxndsvar.rcvr.paths, node, triple) {
	if (backward_traversal (XxYxZ_X (sxndsvar.rcvr.paths, triple),
				XxYxZ_Y (sxndsvar.rcvr.paths, triple),
				j))
	    return SXTRUE;
    }

    return SXFALSE;
}


static void	current_model_no_processing (void)
{
    /* Verifie si le source valide le modele current. */

    SXINT		current_index, new_index, state_no, source_index, source_class, current_model_no_processing_index, top, i, j, x;
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
			if (!X_set (&sxndsvar.rcvr.stmt_set, (SXINT) stmt, &current_model_no_processing_index)) {
			    /* 1ere occurrence de stmt */
			    new_index = clone_active_scanner (stmt);
			    sxndsvar.ndlv [new_index].terminal_token.lahead = (SXINT) stmt;
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


static SXINT	has_dont_delete (void)
{
    /* Regarde si le choix courant n'utilise pas des caracteres de
       dont_delete. */
    SXINT i, l, x, kind;

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




static SXVOID tryacorr (void)
{
    SXINT j, kind;

    sxndsvar.rcvr.is_done = sxndsvar.rcvr.is_may_be = SXFALSE;

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
			sxndsvar.rcvr.is_may_be = SXTRUE;
			sxndsvar.rcvr.may_be.model = sxndsvar.rcvr.current.model;
			sxndsvar.rcvr.may_be.model_no = sxndsvar.rcvr.current.model_no;
			
			for (j = sxndsvar.rcvr.current.model [0]; j > 0; j--)
			    sxndsvar.rcvr.may_be.string [j] = sxndsvar.rcvr.current.string [j];
		    }
		    /* else on conserve l'autre */
		    
		    sxndsvar.rcvr.is_done = SXFALSE;
		}
	    }
	    
	    if (sxndsvar.rcvr.is_done)
		return;
	}
    }

    if (sxndsvar.rcvr.is_may_be) {
	sxndsvar.rcvr.is_done = SXTRUE;
	sxndsvar.rcvr.current.model = sxndsvar.rcvr.may_be.model;
	sxndsvar.rcvr.current.model_no = sxndsvar.rcvr.may_be.model_no;
			
	for (j = sxndsvar.rcvr.may_be.model [0]; j > 0; j--)
	    sxndsvar.rcvr.current.string [j] = sxndsvar.rcvr.may_be.string [j];
    }
}



static SXBOOLEAN	nds_recovery (void)
{
    /* Une correction impliquant la suppression d'un caractere dont la classe
       simple est dans S_dont_delete ne peut etre validee que s'il n'existe
       pas de modele (meme moins prioritaire) valide. Une correction
       impliquant l'insertion d'un caractere dont la classe simple est dans
       S_dont_insert ne peut etre validee que s'il n'existe pas de modele
       (meme moins prioritaire) valide. */

    /* Les erreurs en look-ahead sont traitees (ds le scanner) par le non
       determinisme. L'ensemble des possibilites associees au sous-automate. */

    SXINT		i, j;

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
	    sxerror (sxsrcmngr.source_coord,
		     sxndsvar.sxtables->err_titles [2][0],
		     sxndsvar.SXS_tables.S_global_mess [5],
		     sxndsvar.sxtables->err_titles [2]+1);

	sxlaback (sxndsvar.SXS_tables.S_nmax);
	return SXFALSE;
    }

    if (!sxsvar.sxlv.mode.is_silent) {
	SXSHORT	sxchar;

	if (!sxndsvar.rcvr.is_done) {
	    /* No Correction => Recovery */
	    sxchar = sxndsvar.rcvr.source_char [0];
	    sxerror (sxsrcmngr.source_coord,
		     sxndsvar.sxtables->err_titles [2][0],
		     sxndsvar.SXS_tables.S_global_mess [1],
		     sxndsvar.sxtables->err_titles [2]+1,
		     (sxchar == EOF ?
		      sxndsvar.SXS_tables.S_global_mess [4] :
		      (sxchar == SXNEWLINE ?
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
				  : (sxchar == SXNEWLINE
				     ? sxndsvar.SXS_tables.S_global_mess [3]
				     : sxc_to_str [sxchar]));
	    }

	    sxerror (sxsrcmngr.source_coord,
		     sxndsvar.sxtables->err_titles [1][0],
		     sxndsvar.SXS_tables.SXS_local_mess [sxndsvar.rcvr.current.model_no].S_string,
		     sxndsvar.sxtables->err_titles [1]+1,
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

    return SXTRUE; /* Ajoute' au pif le 14/05/2003 */
}



SXBOOLEAN		sxndsrecovery (SXINT sxndsrecovery_what)
{
    switch (sxndsrecovery_what) {
    case SXACTION:
	return nds_recovery ();

    case SXERROR:
	/* Error in look-ahead */
	look_ahead_extract (sxndsvar.ndlv->current_state_no);
	break;

    case SXCLOSE:
	recovery_free ();
	break;

    default:
	fprintf (sxstderr, "The function \"sxndsrecovery\" is out of date with respect to its specification.\n");
	sxexit(1);
    }

    return SXTRUE;
}
