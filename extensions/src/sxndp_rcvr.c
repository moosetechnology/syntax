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

/************************************************************************/
/* It is the nondeterministic version of the SYNTAX error recovery 	*/
/* module								*/
/************************************************************************/

#include "sxversion.h"
#include "sxcommon.h"

#ifndef VARIANT_32
char WHAT_SXNDP_RECOVERY[] = "@(#)SYNTAX - $Id: sxndp_rcvr.c 3233 2023-05-15 16:16:17Z garavel $" WHAT_DEBUG;
#else
char WHAT_SXNDP_RECOVERY32[] = "@(#)SYNTAX - $Id: sxndp_rcvr.c 3233 2023-05-15 16:16:17Z garavel $ SXNDP_RCVR_32" WHAT_DEBUG;
#endif

#if EBUG
static char	ME [] = "NDPRECOVERY";
#endif

#include "sxnd.h"

#if EBUG
#define stdout_or_NULL	stdout
#else
#define stdout_or_NULL	NULL
#endif

extern SXINT			NDP_access (struct SXP_bases *abase, SXINT j);

extern SXINT			new_parser (SXINT son, SXINT state, SXINT symbol);

extern SXVOID			set_first_trans (struct SXP_bases *abase, 
						 SXINT *check, 
						 SXINT *action, 
						 SXINT Max, 
						 SXINT *next_action_kind);

extern SXBOOLEAN			set_next_trans (struct SXP_bases *abase, 
						SXINT *check, 
						SXINT *action, 
						SXINT Max, 
						SXINT *next_action_kind);

extern SXBOOLEAN		sxndsubparse_a_token (SXINT parser, SXINT ref, SXINT symbol, sxaction_new_top_t f);

extern SXVOID			reducer (void);

extern struct sxmilstn_elem	*milestone_new (struct sxmilstn_elem *prev_milstn_ptr, SXINT my_milestone, SXINT kind);

extern SXINT                      mreds_push (SXINT parser, SXINT ref, SXINT tok_no, SXBOOLEAN is_new_triple, SXBOOLEAN is_shift_reduce, SXINT symbol);

extern SXVOID                   sxndparser_GC (SXINT current_parser);


/* E R R O R   R E C O V E R Y */

static SXVOID compute_a_trans (SXINT parser_seed, SXINT head)
{
    /* On est en correction, les parser : (state, tok_no) ne doivent pas
       etre confondus entre eux lorsque le meme token est utilise dans des modeles
       differents. Ils ne doivent non plus pas etre confondus avec des parsers
       de l'analyse normale (il existe des parsers avec tok_no = TOK [0]
       ceux avec tok_no = NO_de_TOK [1] -les reduces eventuels
       executes avant la detection d'erreur- sont devenus inaccessibles grace a l'appel
       de GC () en tete du traitement d'erreur). 
       De plus il faut pouvoir traiter le cas SXGENERATOR.
       Au kieme appel de compute_a_trans (), parse_stack.delta_generator vaut
       -k*(tmax+1) et current_token est initialise a delta_generator - t
       si SXRECOGNIZER ou a delta_generator.
       current_milestone vaut delta_generator. */
    /* calcule dans for_scanner.next_hd l'ensemble des parsers atteints depuis
       parser_seed par transition sur le[s] token[s] specifie[s] par head. */

    SXINT	xmscan, scan_ref;

    parse_stack.current_milestone = parse_stack.current_token =
	parse_stack.delta_generator -= (sxplocals.SXP_tables.P_tmax + 1);

    if (head < 0) {
	/* X ou S */
	/* Attention, current_token devra prendre une valeur differente de tout ce
	   qui existe deja ds "parsers" et donc differente de celle donnee par un
	   autre X. */
	sxplocals.mode.mode = SXGENERATOR;
	parse_stack.current_lahead = 0;
    }
    else {
	sxplocals.mode.mode = SXRECOGNIZER;
	sxplocals.rcvr.TOK_i = parse_stack.rcvr.TOK [head];
	parse_stack.current_lahead = SXGET_TOKEN (sxplocals.rcvr.TOK_i).lahead;
	parse_stack.current_token -= parse_stack.current_lahead;
    }

    *parse_stack.for_scanner.next_hd = 0;

    scan_ref = parse_stack.parser_to_attr [parser_seed].scan_ref;

    if (scan_ref < 0) {
	XxY_Xforeach (parse_stack.mscans, parser_seed, xmscan) {
	    sxndsubparse_a_token (parser_seed,
				  XxY_Y (parse_stack.mscans, xmscan),
				  0 /* symbol */,
				  (sxaction_new_top_t) NULL);
	}

	scan_ref = -scan_ref;
    }

    sxndsubparse_a_token (parser_seed, scan_ref, 0 /* symbol */, (sxaction_new_top_t) NULL);
    
    if (parse_stack.for_reducer.top > 0)
	reducer ();
}



static SXINT	get_tail (SXINT binf, SXINT bsup)
{
    SXINT	lahead;

    if (binf == 0)
	binf = 1;

    for (; binf <= bsup; binf++) {
	sxplocals.rcvr.TOK_i = parse_stack.rcvr.TOK [binf];
	lahead = SXGET_TOKEN (sxplocals.rcvr.TOK_i).lahead;

	if (lahead == sxplocals.SXP_tables.P_tmax
	    || (sxplocals.rcvr.truncate_context
	        && sxgetbit (sxplocals.SXP_tables.PER_tset, lahead)))
	    return binf;
    }

    return bsup;
}



static SXBOOLEAN	morgan (char *c1, char *c2)

/* rend vrai si c1 = c2 a une faute d'orthographe pres */

{
    char	*tmax, *t1, *t2;
    SXINT		l1, l2;
    char	d1, d2, e1, e2;

    switch ((l1 = strlen (c1)) - (l2 = strlen (c2))) {
    default:
	/* longueurs trop differentes */
	return SXFALSE;

    case 0:
	/* longueurs egales: on autorise un remplacement ou une interversion
	   de caracteres */
	tmax = c1 + l1, t1 = c1, t2 = c2;

	while (t1 < tmax && *t1 == *t2)
	    t1++, t2++;

	if (t1 == tmax)
	    /* egalite */
	    return SXTRUE;

	d1 = *t1++, d2 = *t2++ /* caracteres courants, differents */ ;
	e1 = *t1++, e2 = *t2++ /* caracteres suivants */ ;

	if (e1 != e2 /* caracteres suivants differents */
		     && (d1 != e2 || e1 != d2))
	    /* caracteres pas inverses */
	    return SXFALSE;


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

    if (*t2 == SXNUL)
	/* egalite au dernier caractere pres */
	return SXTRUE;

    t1++ /* on saute un caractere dans la chaine la plus longue */ ;


/* les restes des chaines doivent etre egaux */

    while (t1 < tmax && *t1 == *t2)
	t1++, t2++;

    return t1 >= tmax;
}



static char	*tok_text (SXINT tok_no)
{
    struct sxtoken	*ptok = &(SXGET_TOKEN (tok_no));

    return sxgenericp (sxplocals.sxtables, ptok->lahead)
	? sxstrget (ptok->string_table_entry)
	    : sxttext (sxplocals.sxtables, ptok->lahead);
}


static char	*ttext (struct sxtables *tables, SXINT tcode)
{
    return (tcode == sxplocals.SXP_tables.P_tmax)
	? sxplocals.SXP_tables.P_global_mess [sxplocals.SXP_tables.P_followers_number + 2]
	    : sxttext (tables, tcode);
}



static SXVOID nd2_modify_dag (void)
{
    /* On a trouve une correction (dans rcvr.best), on modifie le dag
       des tokens en consequence. */
    SXINT			model_no, pos, first_ms, prev_ms, last_ms, x, cur_tok, cur_ms, tde, elem;
    struct sxmilstn_elem
	                *p;
    struct sxtoken	tok;
    SXINT 		*model;
    SXINT 		rch;

    model_no = parse_stack.rcvr.best.model_no;
    model = sxplocals.SXP_tables.P_lregle [model_no];
    rch = sxplocals.SXP_tables.P_right_ctxt_head [model_no];
    pos = model [1] == 0 ? 1 : 0;

    /* Suppression des transitions */
    /* On ne touche ni au dag, ni au chainage des milestones, on se contente
       de les "reinitialiser" depuis le nouveau sxndtkn.milstn_current. */
    /* On suppose que les transitions de dag seront supprimees par le mecanisme
       normal. */
    
    /* Creation des nouveaux milestones. */
    first_ms = prev_ms = parse_stack.rcvr.best.MS [pos];

    if (pos == 0) {
	/* On repositionne milstn_current sur le "nouveau" debut. */
	/* et on "reinitialise toute la chaine. */
	p = sxndtkn.milstn_current = &sxmilstn_access (sxndtkn.milestones, first_ms);
	last_ms = parse_stack.rcvr.best.MS [1];

	do {
	    p->kind &= MS_COMPLETE + MS_BEGIN;
	    p->next_for_scanner = 0; /* Au cas ou. */
	} while ((p = p->next_milstn) != NULL && p->my_index != last_ms);

#if EBUG
	if (p == NULL)
	    sxtrap (ME, "nd2_modify_dag");
#endif
    }

    last_ms = parse_stack.rcvr.best.MS [rch + 1];
    x = pos;
    parse_stack.rcvr.is_warning = SXTRUE;

    do {
	elem = model [++pos];

	if (elem < 0) {
	    /* On cree un token */
	    tok.lahead = parse_stack.rcvr.best.correction [pos];
	    tok.string_table_entry = SXERROR_STE /* error value */ ;
	    sxplocals.rcvr.TOK_i = parse_stack.rcvr.best.TOK [x];

	    if (x + 1 < rch)
		x++; /* approximatif */

	    tok.source_index = SXGET_TOKEN (sxplocals.rcvr.TOK_i).source_index;
	    tok.comment = sxplocals.rcvr.com [pos];

	    if (sxgenericp (sxplocals.sxtables, tok.lahead))
		parse_stack.rcvr.is_warning = SXFALSE;
	}
	else {
	    x = elem + 1; /* approximatif */
	    cur_tok = parse_stack.rcvr.best.TOK [elem];
	    tok = SXGET_TOKEN (cur_tok);
	    tok.comment = sxplocals.rcvr.com [pos];
	}

	/* Dans tous les cas on cree un token pour ne pas melanger les tok_no. */
	sxput_token (tok);
	cur_tok = sxplocals.Mtok_no;

	if (elem < rch) {
	    /* On cree un milestone */
	    p = milestone_new (&sxmilstn_access (sxndtkn.milestones, prev_ms),
			       ++sxndtkn.last_milestone_no,
			       MS_BEGIN | MS_COMPLETE);
	    cur_ms = p->my_index;
	}
	else
	    cur_ms = last_ms;

	/* On remplit dag */
	XxYxZ_set (&sxndtkn.dag,
		   prev_ms,
		   cur_tok,
		   cur_ms,
		   &tde);
	
	prev_ms = cur_ms;
    } while (elem < rch);

/* On pourrait ici, si on a cree des milestones, renumeroter toute la chaine depuis
   sxndtkn.milstn_current afin que les milestones successifs aient des my_milestone
   croissants. Attention, si on tombe sur des milestones non MS_COMPLET, il faut
   aussi changer le numero de milestone de ces scanners actifs (et de ceux empiles pour
   le traitement des includes). Est-ce indispensable ? */
}



static SXVOID put_in_repair_list (SXINT repair_tok_no, 
				  SXINT	error_tok_no, 
				  SXINT	model_no, 
				  SXINT kind,
				  char	*msg_params [5])
{
    SXINT 		i;
    struct repair	*pelem;

    if (parse_stack.rcvr.repair_list == NULL) {
	parse_stack.rcvr.repair_list =
	    (struct repair*) sxalloc ((parse_stack.rcvr.repair_list_size = 31) + 1,
				      sizeof (struct repair));
	parse_stack.rcvr.repair_list_top = 0;
	parse_stack.rcvr.repair_string =
	    (char*) sxalloc ((parse_stack.rcvr.repair_string_size = 512) + 1,
			     sizeof (char));
	parse_stack.rcvr.repair_string_top = 0;
    }
    else if (++parse_stack.rcvr.repair_list_top > parse_stack.rcvr.repair_list_size)
	parse_stack.rcvr.repair_list =
	    (struct repair*) sxrealloc (parse_stack.rcvr.repair_list,
					(parse_stack.rcvr.repair_list_size *= 2) + 1,
					sizeof (struct repair));

    pelem = parse_stack.rcvr.repair_list + parse_stack.rcvr.repair_list_top;
    pelem->repair_tok_no = repair_tok_no;
    pelem->error_tok_no = error_tok_no;
    pelem->model_no = model_no;
    pelem->kind = kind;

    for (i = 0; i < sxplocals.SXP_tables.SXP_local_mess [model_no].P_param_no; i++) {
	pelem->params [i] = parse_stack.rcvr.repair_string_top;
	parse_stack.rcvr.repair_string_top += strlen (msg_params [i]) + 1;

	if (parse_stack.rcvr.repair_string_top > parse_stack.rcvr.repair_string_size) {
	    do {
		parse_stack.rcvr.repair_string_size *= 2;
	    }while (parse_stack.rcvr.repair_string_top > parse_stack.rcvr.repair_string_size);

	    parse_stack.rcvr.repair_string =
		(char*) sxrealloc (parse_stack.rcvr.repair_string,
				   parse_stack.rcvr.repair_string_size + 1,
				   sizeof (char));
	}

	strcpy (parse_stack.rcvr.repair_string + pelem->params [i], msg_params [i]);
    }
}


SXVOID output_repair_string (SXINT output_repair_string_index)
{
    SXINT			i;
    struct repair	*pelem = parse_stack.rcvr.repair_list + output_repair_string_index;
    char		*msg_params [5];

    for (i = 0; i < sxplocals.SXP_tables.SXP_local_mess [pelem->model_no].P_param_no; i++) {
	msg_params [i] = parse_stack.rcvr.repair_string + pelem->params [i];
    }

    sxerror (SXGET_TOKEN (pelem->error_tok_no).source_index,
	     sxplocals.sxtables->err_titles [pelem->kind][0],
	     sxplocals.SXP_tables.SXP_local_mess [pelem->model_no].P_string,
	     sxplocals.sxtables->err_titles [pelem->kind]+1,
	     msg_params [0],
	     msg_params [1],
	     msg_params [2],
	     msg_params [3],
	     msg_params [4]);

}


SXVOID output_repair_list (void)
{
    SXINT i;

    for (i = 0; i <= parse_stack.rcvr.repair_list_top; i++)
	output_repair_string (i);
}


static SXINT set_token_dag (SXBOOLEAN to_be_cloned, 
			    SXBOOLEAN is_copy,
			    struct sxtoken *ptok,
			    SXINT  tok_no, 
			    SXINT ms1, 
			    SXINT ms2)
{
    /* Si to_be_cloned, creation obligatoire d'un nouveau token.
       Si is_copy, ptok est NULL ou *ptok est une copie exacte de tok_no
       Si !is_copy, tok_no est non significatif => reutilisation ou creation
          d'un nouveau token. */
    SXINT			tok_no2, xyz;
    struct sxtoken	*ptok2;

    if (!to_be_cloned) {
	/* On regarde s'il y a des triplets (ms1, tok_no2, ms2). */
	if (ptok != NULL) {
	    XxYxZ_Xforeach (sxndtkn.dag, ms1, xyz) {
		if (XxYxZ_Z (sxndtkn.dag, xyz) == ms2) {
		    tok_no2 = XxYxZ_Y (sxndtkn.dag, xyz);
		    ptok2 = &(SXGET_TOKEN (tok_no2));
		    
		    if (ptok->lahead == ptok2->lahead
			&& ptok->string_table_entry == ptok2->string_table_entry
			&& ptok->comment == ptok2->comment
			&& ptok->source_index.line == ptok2->source_index.line
			&& ptok->source_index.column == ptok2->source_index.column
			&& ptok->source_index.file_name == ptok2->source_index.file_name)
			/* existe deja */
			return xyz;
		}
	    }
	}
	
	if (is_copy) {
	    XxYxZ_set (&sxndtkn.dag, ms1, tok_no, ms2, &xyz);
	    return xyz;
	}
    }
    
    sxput_token (*ptok);
    XxYxZ_set (&sxndtkn.dag, ms1, sxplocals.Mtok_no, ms2, &xyz);

    return xyz;
}


static SXVOID one_correction_message (void)
{
    /* OK ca a marche */
    char				*msg_params [5];
    SXINT				        *regle;
    struct SXP_local_mess		*local_mess;
    struct sxtoken			*tok;
    
    SXINT		n, l, x, y, x1, y1, im, ll, j, i, ate, carac_tok_no;
    SXINT		*mm;
    SXBOOLEAN	do_put_error, is_copy, is_carac;
    SXBA	back_bone;
    
    sxinitialise(back_bone); /* pour faire taire "gcc -Wuninitialized" */
    sxinitialise(mm); /* pour faire taire "gcc -Wuninitialized" */
    sxinitialise(is_copy); /* pour faire taire "gcc -Wuninitialized" */
    sxinitialise(carac_tok_no); /* pour faire taire "gcc -Wuninitialized" */
    sxinitialise(is_carac); /* pour faire taire "gcc -Wuninitialized" */
    sxplocals.mode.local_errors_nb++;
    im = 0;
    regle = sxplocals.SXP_tables.P_lregle [parse_stack.rcvr.best.model_no];
    local_mess = sxplocals.SXP_tables.SXP_local_mess + parse_stack.rcvr.best.model_no;
    ll = regle [0];
    
    /* preparation du message d'erreur */
    
    for (j = ll; j > 0; j--) {
	if (regle [j] > im)
	    im = regle [j];
    }
    
    for (j = local_mess->P_param_no - 1; j >= 0; j--) {
	i = local_mess->P_param_ref [j];

	if (i < 0) {
	  /* X or S */
	  msg_params [j] = ttext (sxplocals.sxtables, parse_stack.rcvr.best.correction [-i]);
	}
	else {
	  sxplocals.rcvr.TOK_i = parse_stack.rcvr.best.TOK [i];
	  tok = &(SXGET_TOKEN (sxplocals.rcvr.TOK_i));
	  msg_params [j] = ((ate = tok->string_table_entry) == SXEMPTY_STE || ate == SXERROR_STE
			    ? ttext (sxplocals.sxtables, tok->lahead)
			    : sxstrget (ate));
	}

#if 0
	/* Le 13/05/03 */
	sxplocals.rcvr.TOK_i = parse_stack.rcvr.best.TOK [i];
	tok = &(SXGET_TOKEN (sxplocals.rcvr.TOK_i));
	msg_params [j] = i < 0 /* X or S */
	    ? ttext (sxplocals.sxtables, parse_stack.rcvr.best.correction [-i])
		: ((ate = tok->string_table_entry) == SXEMPTY_STE || ate == SXERROR_STE
		   ? ttext (sxplocals.sxtables, tok->lahead)
		   : sxstrget (ate));
#endif
    }
    
    /* On doit retrouver NULL ds le token, meme si les commentaires
       ne sont pas gardes. */
    for (y1 = 1; y1 <= ll; y1++)
	sxplocals.rcvr.com [y1] = NULL;
    
    if (!sxsvar.SXS_tables.S_are_comments_erased) {
	/* deplacement des commentaires */
	
	x = im;
	
	for (y = ll; regle [y] == x && x >= 0; y--) {
	    sxplocals.rcvr.TOK_i = parse_stack.rcvr.best.TOK [x--];
	    sxplocals.rcvr.com [y] = SXGET_TOKEN (sxplocals.rcvr.TOK_i).comment;
	}
	
	if (ll < ++y)
	    y = ll;
	
	y1 = 1;
	
	for (x1 = 0; x1 <= x && y1 < y; x1++) {
	    sxplocals.rcvr.TOK_i = parse_stack.rcvr.best.TOK [x1];
	    sxplocals.rcvr.com [y1++] = SXGET_TOKEN (sxplocals.rcvr.TOK_i).comment;
	}
	
	for ( /* x1 = x1 */ ; x1 <= x; x1++) {
	    sxplocals.rcvr.TOK_i = parse_stack.rcvr.best.TOK [x1];
	    tok = &(SXGET_TOKEN (sxplocals.rcvr.TOK_i));
	    
	    if (sxplocals.rcvr.com [y] == NULL)
		sxplocals.rcvr.com [y] = tok->comment;
	    else if (tok->comment != NULL) {
		/* concatenation des commentaires */
	        sxplocals.rcvr.com [y] = sx_alloc_and_copy (strlen (sxplocals.rcvr.com [y]) + strlen (tok->comment) + 1,
							    sizeof(char),
							    sxplocals.rcvr.com[y],
							    strlen (sxplocals.rcvr.com[y]) );
		strcat (sxplocals.rcvr.com [y], tok->comment);
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
    
    if (sxplocals.sxtables->SXS_tables.S_is_non_deterministic) {
	nd2_modify_dag ();
    }
    else {
	j = sxplocals.SXP_tables.P_right_ctxt_head [parse_stack.rcvr.best.model_no];

	if (parse_stack.rcvr.all_corrections) {
	    /* Si dans tokens.dag on a :
	       p ->t q et p' ->t q' alors p==p' et q==q'
	       Les transitions differentes doivent obligatoirement se faire sur des
	       numero de token differents (clone du meme token). Cela est du a la facon
	       dont l'analyseur calcule ses (nouveaux) "parser". */

	    mm = parse_stack.rcvr.mm [parse_stack.rcvr.best.model_no];
	    back_bone = parse_stack.rcvr.back_bone [parse_stack.rcvr.best.model_no];

	    if (regle [1] == 0)
		set_token_dag (SXFALSE,
			       SXTRUE,
			       (struct sxtoken*) NULL, /* creation du triplet */
			       sxplocals.rcvr.TOK_0,
			       mm [1],
			       mm [2]);

	    parse_stack.rcvr.milstn_elems [mm [2]].my_milestone = -1;

	    /* Le futur token rencontre' sera caracteristique */
	    is_carac = SXTRUE;
	}

	l = (regle [1] == 0) ? 2 : 1;
	x = l - 1;
	n = 0;

	parse_stack.rcvr.is_warning = SXTRUE;
	
	for (tok = sxplocals.rcvr.toks; (i = regle [l]) < j; l++, tok++) {
	    if (i < 0) {
		tok->lahead = parse_stack.rcvr.best.correction [l];
		tok->string_table_entry = SXERROR_STE /* error value */ ;
		sxplocals.rcvr.TOK_i = parse_stack.rcvr.best.TOK [x];
		tok->source_index = SXGET_TOKEN (sxplocals.rcvr.TOK_i).source_index;
		tok->comment = sxplocals.rcvr.com [l];
		
		if (sxgenericp (sxplocals.sxtables, tok->lahead))
		    parse_stack.rcvr.is_warning = SXFALSE;

		if (x < j)
		    ++x;

		is_copy = SXFALSE;
	    }
	    else {
		sxplocals.rcvr.TOK_i = parse_stack.rcvr.best.TOK [i];
		*tok = SXGET_TOKEN (sxplocals.rcvr.TOK_i);
		
		if (tok->comment != sxplocals.rcvr.com [l]) {
		    tok->comment = sxplocals.rcvr.com [l];
		    is_copy = SXFALSE;
		}
		else
		    if (parse_stack.rcvr.all_corrections)
			is_copy = SXBA_bit_is_set (back_bone, l);

		x = i + 1;
	    }
	    
	    if (parse_stack.rcvr.all_corrections) {
		/* Le token caracteristique n'appartient jamais au back_bone. */
		set_token_dag (is_carac,
			       is_copy,
			       tok,
			       sxplocals.rcvr.TOK_i,
			       mm [l],
			       mm [l + 1]);
		
		if (is_carac)
		    /* C'est obligatoirement un clone... */
		    carac_tok_no = sxplocals.Mtok_no;

		/* noeud utilise'. */
		parse_stack.rcvr.milstn_elems [mm [l + 1]].my_milestone = -1;
		is_carac = SXFALSE;
	    }
	    
	    n++;
	    /* et voila */
	}
	
	if (parse_stack.rcvr.all_corrections) {
	    /* i, l reperent le debut de la zone de validation
	       (les commentaires seront OK et a chaque correction correspond (au moins)
	       un nouveau token unique. */
	    sxplocals.rcvr.TOK_i = parse_stack.rcvr.best.TOK [i];
	    *tok = SXGET_TOKEN (sxplocals.rcvr.TOK_i);
		
	    if (tok->comment != sxplocals.rcvr.com [l]) {
		tok->comment = sxplocals.rcvr.com [l];
		is_copy = SXFALSE;
	    }
	    else
		is_copy = SXBA_bit_is_set (back_bone, l);

	    set_token_dag (is_carac,
			   is_copy,
			   tok,
			   sxplocals.rcvr.TOK_i,
			   mm [l],
			   mm [l + 1]);
	    
	    if (is_carac)
		/* C'est obligatoirement un clone... */
		carac_tok_no = sxplocals.Mtok_no;

	    /* noeud utilise'. */
	    parse_stack.rcvr.milstn_elems [mm [l + 1]].my_milestone = -1;

	    /* On suppose que le token dag entre mrch et Mrch, en suivant back_bone
	       est realise'. On le complete. */
	    if (parse_stack.rcvr.mrch <= parse_stack.rcvr.Mrch) {
		/* Ce n'est pas la premiere fois. */
		SXINT	elem, pos, lim;
		sxinitialise(lim); /* pour faire taire gcc -Wuninitialized */
		sxinitialise(elem); /* pour faire taire gcc -Wuninitialized */

		if (j < parse_stack.rcvr.mrch) {
		    elem = j;
		    pos = l;
		    lim = parse_stack.rcvr.mrch;
		}
		else if (j > parse_stack.rcvr.Mrch) {
		    regle = sxplocals.SXP_tables.P_lregle [parse_stack.rcvr.Mrch_model_no];
		    mm = parse_stack.rcvr.mm [parse_stack.rcvr.Mrch_model_no];
		    pos = parse_stack.rcvr.Mrch_pos;
		    lim = j;
		    elem = parse_stack.rcvr.Mrch;
		}
		else
		    pos = 0;

		if (pos != 0) {
		    while (++elem <= lim) {
			++pos;
			set_token_dag (SXFALSE,
			       SXTRUE,
			       (struct sxtoken*) NULL, /* creation du triplet */
			       parse_stack.rcvr.best.TOK [elem],
			       mm [pos],
			       mm [pos + 1]);
			parse_stack.rcvr.milstn_elems [mm [pos + 1]].my_milestone = -1;
		    }
		}
	    }

	    if (j > parse_stack.rcvr.Mrch) {
		parse_stack.rcvr.Mrch = j;
		parse_stack.rcvr.Mrch_model_no = parse_stack.rcvr.best.model_no;
		parse_stack.rcvr.Mrch_pos = l;
	    }

	    if (j < parse_stack.rcvr.mrch)
		parse_stack.rcvr.mrch = j;

	    if (!sxplocals.mode.is_silent)
		put_in_repair_list (carac_tok_no,
				    sxplocals.rcvr.TOK_0 + 1,
				    parse_stack.rcvr.best.model_no,
				    parse_stack.rcvr.is_warning ? (SXINT)1 : (SXINT)2,
				    msg_params);
	}
	else {
	    /* Pour ne pas risquer de refabriquer des symboles ou regles (arbre d'analyse)
	       qui on ete invalides par la correction courante, la nouvelle valeur
	       de tok_no (qui donne les milestones), devra etre au moins egale (le 1er
	       milestone fabrique' par le parser sera ptok_no+1) au tok_no du terminal
	       sur lequel l'erreur a ete detectee. */

	    parse_stack.for_scanner.top = FS_DECR (parse_stack.for_scanner.top);
	    
	    if (regle [1] == 0) {
		j--;
		sxplocals.rcvr.TOK_0++;
	    }
	    else {
		/* Le GC de sortie supprimera tous les parsers dont le second composant
		   est le numero de token sxplocals.rcvr.TOK_0. */
		parse_stack.for_scanner.previouses [parse_stack.rcvr.cur_top] = 0;
		parse_stack.for_scanner.top = FS_DECR (parse_stack.for_scanner.top); 
	    }
	    
	    sxtknmdf (sxplocals.rcvr.toks,
		      n,
		      sxplocals.rcvr.TOK_0,
		      j);
	    
	    sxplocals.ptok_no = sxplocals.rcvr.TOK_0 - 1;
    
	    parse_stack.for_scanner.next_hd =
		parse_stack.for_scanner.previouses + parse_stack.for_scanner.top;
	}
    }
    
    if (parse_stack.rcvr.all_corrections) {
#if EBUG
	do_put_error = !sxplocals.mode.is_silent
#else
	do_put_error = SXFALSE
#endif
	;
    }
    else
	do_put_error = !sxplocals.mode.is_silent;
    

    /* "Warning: "machin" is replaced by chose." ou similaire */
    if (do_put_error)
	sxerror (parse_stack.rcvr.source_index,
		 parse_stack.rcvr.is_warning
		 ? sxplocals.sxtables->err_titles [1][0]
		 : sxplocals.sxtables->err_titles [2][0],
		 local_mess->P_string,
		 parse_stack.rcvr.is_warning
		 ? sxplocals.sxtables->err_titles [1]+1
		 : sxplocals.sxtables->err_titles [2]+1,
		 msg_params [0],
		 msg_params [1],
		 msg_params [2],
		 msg_params [3],
		 msg_params [4]);
}


static SXBOOLEAN correction_already_found (SXINT *model, SXINT lim)
{
    /* On considere que deux corrections sont identiques si les chaines corrigees
       sont identiques jusqu'au token de validation max. */
    SXINT		hd, tail, l, elem, correction_already_found_index;

    hd = 0;
    l = 0;

    while ((elem = model [++l]) < lim) {
	tail = (elem < 0) ? sxplocals.rcvr.lter [l]
	    : SXGET_TOKEN (parse_stack.rcvr.TOK [elem]).lahead;
	XxY_set (&parse_stack.rcvr.correction_strings, hd, tail, &correction_already_found_index);
	hd = correction_already_found_index;
    }

    while (elem < parse_stack.rcvr.MMrch) {
	tail = SXGET_TOKEN (parse_stack.rcvr.TOK [elem]).lahead;
	XxY_set (&parse_stack.rcvr.correction_strings, hd, tail, &correction_already_found_index);
	hd = correction_already_found_index;
	elem++;
    }

    /* Attention aux prefixes communs. */
    return XxY_set (&parse_stack.rcvr.correction_strings, hd, 0, &correction_already_found_index);
}

static SXBOOLEAN validate_submodel (SXINT model_no, SXINT *model, SXINT parser, SXINT pos, SXINT bot, SXBOOLEAN a_la_rigueur)
{
    /* Retourne vrai ssi un modele est trouve'. */
    SXINT		lim, l, x, seed, elem, t;
    SXBOOLEAN	local_a_la_rigueur;

    if (pos < bot) {
	/* lter contient une correction */ 
	lim = sxplocals.SXP_tables.P_right_ctxt_head [model_no];

	/* Si parse_stack.rcvr.best.model_no == model_no (cas parse_stack.rcvr.all_corrections)
	   et si la correction est identique a une precedente, il ne faut pas la resortir!. */


	if (parse_stack.rcvr.all_corrections
	    && correction_already_found (/* model_no, */model, lim))
	    /* deja trouve'. */
	    return SXFALSE;

	parse_stack.rcvr.best.model_no = model_no;
	parse_stack.rcvr.best.a_la_rigueur = a_la_rigueur;
	l = 0;
    
	while (model [++l] < lim)
	    parse_stack.rcvr.best.correction [l] = sxplocals.rcvr.lter [l];

	/* Recopie ds best afin d'avoir le meme traitement que le scanner soit ou non
	   deterministe. */
	for (l = 0; l <= sxplocals.SXP_tables.P_nmax; l++) {
	    parse_stack.rcvr.best.TOK [l] = parse_stack.rcvr.TOK [l];
	    parse_stack.rcvr.best.MS [l] = parse_stack.rcvr.MS [l];
	}

	parse_stack.rcvr.best.MS [l] = parse_stack.rcvr.MS [l];

	if (parse_stack.rcvr.all_corrections)
	    one_correction_message ();

	return SXTRUE;
    }

    XxY_Yforeach (parse_stack.rcvr.seeds, parser, x) {
	local_a_la_rigueur = a_la_rigueur;
	seed = XxY_X (parse_stack.rcvr.seeds, x);
		
	switch (elem = model [pos]) {
	case -2: /* S */
	    sxplocals.rcvr.TOK_i = parse_stack.rcvr.TOK [pos - 1];
	    /* On suppose que le S a l'index pos remplace le token pos-1 */
	    /* On continue en sequence */
#if defined (__GNUC__) && (__GNUC__ - 0 >= 7)
	    __attribute__ ((fallthrough));
#endif
	    
	case -1: /* X */
#if EBUG
	    if (seed < 0)
		sxtrap (ME, "validate_submodel");
#endif

	    t = (-XxYxZ_Y (parse_stack.parsers, seed)) % (sxplocals.SXP_tables.P_tmax + 1);
		
	    if (elem == -1 || (sxkeywordp (sxplocals.sxtables, t)
		&& morgan (tok_text (sxplocals.rcvr.TOK_i), sxttext (sxplocals.sxtables, t)))) {
		if (sxgetbit (sxplocals.SXP_tables.P_no_insert, t)) {
		    /* On ne peut inserer t */
		    local_a_la_rigueur = SXTRUE;
		}
	    }
	    else
		t = 0;
		    
	    break;
	    
	default:
	    sxplocals.rcvr.TOK_i = parse_stack.rcvr.TOK [elem];
	    t = SXGET_TOKEN (sxplocals.rcvr.TOK_i).lahead;
	    break;
	}
	
	if (t != 0) {
	    sxplocals.rcvr.lter [pos] = t;

	    if (parse_stack.rcvr.all_corrections) {
		if (!local_a_la_rigueur /* pur (pour l'instant) */)
		    validate_submodel (model_no, model, seed, pos - 1,
				       bot, local_a_la_rigueur);
	    }
	    else {
		if (!local_a_la_rigueur /* pur (pour l'instant) */
		    || model_no < parse_stack.rcvr.best.model_no)
		    if (validate_submodel (model_no, model, seed, pos - 1,
					   bot, local_a_la_rigueur))
			if (!parse_stack.rcvr.best.a_la_rigueur || a_la_rigueur)
			    /* On ne pourra pas trouver mieux localement. */
			    return SXTRUE;
	    }
	}
    }

    return parse_stack.rcvr.best.model_no == model_no;
}


static SXBOOLEAN validate_model (SXINT parser, SXINT model_no, SXINT pos)
{
    /* Si parser < 0 (-model_no), on a execute' l'instruction halt sur fin de fichier. */
    SXINT 	*model = sxplocals.SXP_tables.P_lregle [model_no];
    SXINT		bot;

    bot = (model [1] == 0) ? 2 : 1;
    
    do {
	if (validate_submodel (model_no, model, parser, pos - 1,
			       bot, parse_stack.rcvr.dont_delete)) {
	    if (!parse_stack.rcvr.all_corrections)
		if (!parse_stack.rcvr.best.a_la_rigueur)
		    /* On ne pourra pas trouver mieux localement. */
		    return SXTRUE;
	}
    } while (parser > 0 && (parser = parse_stack.parser_to_attr [parser].next_for_scanner) > 0);
    
    return parse_stack.rcvr.best.model_no == model_no;
}



static SXBOOLEAN try_a_model (SXINT pos, SXINT parser_seed, SXINT model_no, SXBOOLEAN check_first_trans, SXINT tok_no_to_be_checked)
{
    SXINT 		*model = sxplocals.SXP_tables.P_lregle [model_no];
    SXINT 		lmod = model [0];
    SXINT			head, tail, try_a_model_index, parser, seed, prev_parser;
    SXBOOLEAN		ret_val, is_halt_hit;

    tail = get_tail (sxplocals.SXP_tables.P_right_ctxt_head [model_no], model [lmod]);

    do {
	head = model [++pos];
	try_a_model_index = parse_stack.rcvr.prefixes [model_no] [pos];
	
	if (SXBA_bit_is_reset_set (parse_stack.rcvr.prefixes_set, try_a_model_index)) {
	    parse_stack.rcvr.prefixes_to_milestones [try_a_model_index].next_for_scanner = 0;
	    /* On ajoute try_a_model_index a parse_stack.rcvr.milstn_hd pour GC () */
	    parse_stack.rcvr.prefixes_to_milestones [try_a_model_index].next_milstn =
		parse_stack.rcvr.milstn_head;
	    parse_stack.rcvr.milstn_head =
		&(parse_stack.rcvr.prefixes_to_milestones [try_a_model_index]);
	    
	    is_halt_hit = SXFALSE;
	    parse_stack.halt_hit = SXFALSE;

	    do {
		/* On calcule la transition. */
		if (!check_first_trans
		    || XxYxZ_Y (parse_stack.parsers, parser_seed) == tok_no_to_be_checked) {
		    compute_a_trans (parser_seed, head);
		    
		    if (parse_stack.halt_hit) {
			parse_stack.halt_hit = SXFALSE;
			
			if (*parse_stack.for_scanner.next_hd == 0 && head == tail) {
			    /* Si *parse_stack.for_scanner.next_hd != 0, on le privilegie */
			    is_halt_hit = SXTRUE; 
			    XxY_set (&parse_stack.rcvr.seeds, parser_seed, -model_no, &seed); 
			}
		    }
		    
		    if ((parser = *parse_stack.for_scanner.next_hd) > 0) {
			do {
			    /* On memorise les couples */
			    XxY_set (&parse_stack.rcvr.seeds, parser_seed, parser, &seed); 
			    prev_parser = parser;
			} while ((parser =
				  parse_stack.parser_to_attr [parser].next_for_scanner) > 0);
			
			/* On concatene le resultat. */
			parse_stack.parser_to_attr [prev_parser].next_for_scanner = 
			    parse_stack.rcvr.prefixes_to_milestones [try_a_model_index].next_for_scanner;
			parse_stack.rcvr.prefixes_to_milestones [try_a_model_index].next_for_scanner =
			    *parse_stack.for_scanner.next_hd;
			*parse_stack.for_scanner.next_hd = 0; /* Pour GC () */
		    }
		}
	    } while ((parser_seed =
		      parse_stack.parser_to_attr [parser_seed].next_for_scanner) > 0);

	    if (parse_stack.rcvr.prefixes_to_milestones [try_a_model_index].next_for_scanner == 0
		&& is_halt_hit)
		SXBA_1_bit (parse_stack.rcvr.halt_hit_set, try_a_model_index);
	}

	if ((parser_seed =
	     parse_stack.rcvr.prefixes_to_milestones [try_a_model_index].next_for_scanner) == 0) {
	    if (SXBA_bit_is_set (parse_stack.rcvr.halt_hit_set, try_a_model_index))
		parser_seed = -model_no;
	    else
		return SXFALSE;
	}

	check_first_trans = SXFALSE;
    } while (head != tail);

    /* Le modele est correct, il faut maintenant le valider vis-a-vis des 
       dont-delete, dont-insert et des fautes d'orthographe. */
    ret_val = validate_model (parser_seed, model_no, pos);
    /* Ex: modele 0 S 2 marche, ou 2 est eof (=> parse_stack.halt_hit == SXTRUE) mais
       modele invalide' (pas de faute d'orthographe sur un mot-cle'). */
    /* parse_stack.halt_hit = SXFALSE; deja fait */
    return ret_val;
}


static SXINT dont_delete (SXINT model_no)
{
    /* Retourne 1 si le modele model_no implique au moins un element de dont_delete. */
    /* Retourne 0 si le modele model_no n'implique pas d'element de dont_delete. */
    /* Retourne -1 si le modele model_no implique la suppression de eof. */
    SXINT 	*model = sxplocals.SXP_tables.P_lregle [model_no];
    SXINT		lmod = (SXINT) model [0];
    SXINT		n = (SXINT) model [lmod];
    SXINT		i, x, lahead;
    
    /* On recherche les indices i inferieurs a` n qui n'appartiennent pas au modele. */
    
    for (i = 0; i < n; i++) {
	for (x = 1; x < lmod; x++) {
	    if (model [x] == i)
		break;
	}
	
	if (x == lmod) {
	    /* i est supprime'. */
	    sxplocals.rcvr.TOK_i = parse_stack.rcvr.TOK [i];
	    lahead = SXGET_TOKEN (sxplocals.rcvr.TOK_i).lahead;
	    
	    if (lahead == sxplocals.SXP_tables.P_tmax)
		return -1;

	    if (sxgetbit (sxplocals.SXP_tables.P_no_delete, lahead))
		return 1;
	}
    }
    
    return 0;
}


static SXBOOLEAN check_for_spelling (SXINT model_no)
{
    /* Retourne SXTRUE ssi ce n'est pas un modele de correction de faute d'orthographe
       ou si la correction semble possible (vu les renseignements dont on dispose). */
    /* Si le modele est un modele de correction de faute d'orthographe
       on ne continue que si le langage definit des mot-cles et si la chaine de
       caracteres correspondante a une longueur superieure a 2*/

    SXINT 	*model = sxplocals.SXP_tables.P_lregle [model_no];
    SXINT 	lim = sxplocals.SXP_tables.P_right_ctxt_head [model_no];
    SXINT 	val;
    SXINT		x;

    for (x = 1; (val = model [x]) < lim; x++) {
	if (val == -2		/* S */) {
	    /* On suppose un seul S et que ce S en position x ds un modele est mis pour
	       une faute d'orthographe du (x-1)ieme token. */
	    sxplocals.rcvr.TOK_i = parse_stack.rcvr.TOK [x - 1];

	    return sxplocals.sxtables->SXS_tables.check_keyword != NULL
		&& SXGET_TOKEN (sxplocals.rcvr.TOK_i).lahead != sxplocals.SXP_tables.P_tmax
		    /* strlen ("END OF FILE") > 2 */
		    && strlen (tok_text (sxplocals.rcvr.TOK_i)) > 2;
	}
    }

    return SXTRUE;
}


static SXVOID fill_vt_set (SXBA vt_set, SXINT parser)
{
    SXINT t;

    sxba_empty (vt_set);

    do {
	t = (-XxYxZ_Y (parse_stack.parsers, parser)) % (sxplocals.SXP_tables.P_tmax + 1);
	SXBA_1_bit (vt_set, t);
    } while ((parser = parse_stack.parser_to_attr [parser].next_for_scanner) > 0);
}


static SXVOID compute_vt_set (SXBA vt_set, SXINT parser)
{
    SXINT xmscan, scan_ref;

    sxplocals.mode.mode = SXGENERATOR;
    parse_stack.current_milestone = parse_stack.current_token = parse_stack.delta_generator -= (sxplocals.SXP_tables.P_tmax + 1);
    parse_stack.current_lahead = 0;
    *parse_stack.for_scanner.next_hd = 0;
    
    do {
	scan_ref = parse_stack.parser_to_attr [parser].scan_ref;

	if (scan_ref < 0) {
	    XxY_Xforeach (parse_stack.mscans, parser, xmscan) {
		sxndsubparse_a_token (parser,
				      XxY_Y (parse_stack.mscans, xmscan),
				      0 /* symbol */,
				      (sxaction_new_top_t) NULL);
	    }

	    scan_ref = -scan_ref;
	}

	sxndsubparse_a_token (parser, scan_ref, 0 /* symbol */, (sxaction_new_top_t) NULL);
    } while ((parser = parse_stack.parser_to_attr [parser].next_for_scanner) > 0);
    
    if (parse_stack.for_reducer.top > 0)
	reducer ();
    
    fill_vt_set (vt_set, *parse_stack.for_scanner.next_hd);
    *parse_stack.for_scanner.next_hd = 0;
}


static SXVOID local_correction (void)
{
    SXINT		z, i, model_no, parser, parser_seed;
    SXBOOLEAN	is_a_0_model;
    
    /* Si parse_stack.rcvr.all_corrections, les elements de dont-insert, dont-delete
       deviennent obligatoires (aucune correction les impliquant ne peut etre choisie).
       A priori surtout utile pour dont_insert, ca permet d'eviter l'insertion multiple
       de synonymes. Ex: on met tous les operateurs sauf "+" ds dont-insert. */
    
    /* L'erreur sera marquee (listing) a l'endroit de la detection */
    parse_stack.rcvr.source_index = SXGET_TOKEN (sxplocals.ptok_no).source_index;
    sxplocals.rcvr.TOK_0 = sxplocals.ptok_no - 1;

/* On lit tous les tokens dont on aura besoin pour la correction locale. */
    sxget_token (sxplocals.rcvr.TOK_0 + sxplocals.SXP_tables.P_nmax);
/* On se souvient du dernier token lu */
    parse_stack.rcvr.Mtok_no = sxplocals.Mtok_no; 

/* On met leur numeros ds TOK, MS (compatibilite avec le traitement d'erreur quand
   le scanner est non-deterministe. */
    for (i = 0; i <= sxplocals.SXP_tables.P_nmax; i++)
	parse_stack.rcvr.TOK [i] = parse_stack.rcvr.MS [i] = sxplocals.rcvr.TOK_0 + i;
    
    parse_stack.rcvr.cur_top = FS_DECR (parse_stack.for_scanner.top);
    parse_stack.rcvr.prev_top = FS_DECR (parse_stack.rcvr.cur_top);

    XxY_clear (&parse_stack.rcvr.seeds);
    sxba_empty (parse_stack.rcvr.prefixes_set);
    sxba_empty (parse_stack.rcvr.halt_hit_set);
    parse_stack.rcvr.milstn_head = NULL;

    for (model_no = 1; model_no <= sxplocals.SXP_tables.P_nbcart; model_no++) {
	if ((i = dont_delete (model_no)) >= 0) {
	    parse_stack.rcvr.dont_delete = i > 0;
	    
	    if (!parse_stack.rcvr.all_corrections || !parse_stack.rcvr.dont_delete) {
		if (check_for_spelling (model_no)) {
		    if (model_no < parse_stack.rcvr.best.model_no
			|| !parse_stack.rcvr.dont_delete) {
			/* On poursuit si aucun modele n'a ete trouve ou si
			   le modele courant ne supprime pas d'elements de "no_delete". */
			is_a_0_model = sxplocals.SXP_tables.P_lregle [model_no] [1] == 0;
			parser_seed = parse_stack.for_scanner.previouses
			    [is_a_0_model ? parse_stack.rcvr.cur_top 
			     : parse_stack.rcvr.prev_top];

			if (parser_seed > 0 && try_a_model (is_a_0_model ? (SXINT)1 : (SXINT)0,
							    parser_seed,
							    model_no,
							    SXFALSE,
							    (SXINT)0))
			    if (!parse_stack.rcvr.all_corrections &&
				!parse_stack.rcvr.best.a_la_rigueur)
				/* Un pur */
				break;
		    }
		}
	    }
	}
    }
    
    if (parse_stack.rcvr.best.model_no > sxplocals.SXP_tables.P_nbcart) {
	/* pas de correction, on s'occupe de vt_set */
	if (parse_stack.rcvr.index_0X > 0
	    && (parser = parse_stack.rcvr.prefixes_to_milestones
		[parse_stack.rcvr.index_0X].next_for_scanner) > 0)
	    /* A pu ne pas etre calcule' (presence d'eof). */
	    /* On extrait vt_set */
	    fill_vt_set (parse_stack.rcvr.best.vt_set, parser);
	else
	    compute_vt_set (parse_stack.rcvr.best.vt_set, *parse_stack.for_scanner.current_hd);
	
	z = sxba_scan (parse_stack.rcvr.best.vt_set, 0);
	
	if (sxba_scan (parse_stack.rcvr.best.vt_set, z) > 0 /* multiple */
	    || z == sxplocals.SXP_tables.P_tmax)
	    z = 0;
	else
	    parse_stack.rcvr.best.single_la = z;
	
	parse_stack.rcvr.best.model_no = sxplocals.SXP_tables.P_nbcart + (z == 0 ? 2 : 1);
    }

    /* On remet tout en etat */
    parse_stack.rcvr.milstn_head = NULL; /* Pour GC () */
}


static SXVOID set_next_shift_state (SXINT son, SXINT fathers_state, SXINT ref, SXINT tnt)
{
    /* On ne cree le pere de son que s'il est interessant. */
    SXINT			father, state, x;
    struct SXP_prdct	*aprdct;

    if ((state = ref + sxplocals.SXP_tables.Mref) < 0 /* ARC traversal */)
	ref = sxplocals.SXP_tables.germe_to_gr_act [-state];
	
    if (ref <= 0)
	ref = -ref;
    else if (tnt == parse_stack.current_lahead
	     && (ref <= sxplocals.SXP_tables.Mred /* scan-reduce */
		 || ref > sxplocals.SXP_tables.Mprdct /* scan */)) {
	/* C'est un scan[-reduce] sur le terminal cle */
	XxYxZ_set (&parse_stack.rcvr.shifts, son, ref, parse_stack.current_token, &x);

	/* son appartient soit au squelette, soit a ete cree avec ref==0 et symb==0. */
	/* S'il n'appartient pas au squelette, on le chaine ds parse_stack.for_scanner.next_hd
	   pour qu'il ne soit pas GCed. */

	if (parse_stack.parser_to_attr [son].scan_ref == 0 /* ref == 0 */) {
	    parse_stack.parser_to_attr [son].next_for_scanner = *parse_stack.for_scanner.next_hd;
	    *parse_stack.for_scanner.next_hd = son;
	}
    }

    if (ref <= sxplocals.SXP_tables.Mred /* [shift|scan] reduce | halt | erreur */)
	return ;

    if ((state = ref - sxplocals.SXP_tables.Mprdct) > 0 /* shift */) {
	if (state != fathers_state /* chemin trouve par le pere */
	    && SXBA_bit_is_set (parse_stack.rcvr.trans, state) /* interessant */) {
#if 0
	    if ((ref = NDP_access (sxplocals.SXP_tables.t_bases + state,
				   parse_stack.current_lahead)) > 0) {
		/* scan sur le terminal cle */
		father = seek_parser (son, state, ref, 0 /* symbol */);
		XxYxZ_set (&parse_stack.rcvr.shifts, father, ref, parse_stack.current_token, &x);
		/* Il est egalement possible que state ait d'autres transitions interessantes
		   on empile donc egalement state par mreds_push. */
	    }
#endif
		
	    father = new_parser (son, state, 0 /* symbol */);
	    mreds_push (father, 0 /* ref */, parse_stack.current_token, SXFALSE, SXFALSE, 0);
#if 0
	    father = seek_parser (son, state, 0 /* ref */, 0 /* symbol */);
	    /* Si "father" est nouveau, mreds_push (father, -0); a ete appele' */
#endif
	}

	return;
    }

    /* predicat ou non-determinisme */
    aprdct = sxplocals.SXP_tables.prdcts + ref;

    for (;;) {
	set_next_shift_state (son, fathers_state, (SXINT) aprdct->action, tnt);
	
	if (aprdct->prdct == 20000 || aprdct->prdct < 0)
	    break;
	
	aprdct++;
    }
}


static SXVOID create_path (SXINT son, SXINT fathers_state)
{
    /* Attention, les parsers crees ne doivent pas etre GCer... */
    /* On range les intermediaires par mreds_push et les feuilles ds rcvr.shifts. */
    struct SXP_bases	*abase;
    SXINT			next_action_kind, state;
    SXINT			check, ref;

    state = XxYxZ_X (parse_stack.parsers, son);

    if (parse_stack.rcvr.trans == NULL
	|| !SXBA_bit_is_set (parse_stack.rcvr.trans, state))
	return;

    /* Il existe depuis l'etat state une sequence de transitions vers le terminal
       cle courant. */

    abase = sxplocals.SXP_tables.t_bases + state;

    /* Il existe depuis l'etat state une sequence non vide de transitions vers le terminal
       cle courant. */

    /* On regarde les transitions terminales... */
    set_first_trans (abase, &check, &ref,
		     sxplocals.SXP_tables.P_tmax, &next_action_kind);
    
    do {
	set_next_shift_state (son, fathers_state, ref, check);
    } while (set_next_trans (abase, &check, &ref,
			     sxplocals.SXP_tables.P_tmax, &next_action_kind)
	     && check > 0);
    
    if (state <= sxplocals.SXP_tables.M0ref - sxplocals.SXP_tables.Mprdct) {
	/* On regarde les transitions non terminales... */
	abase = sxplocals.SXP_tables.nt_bases + state;
	set_first_trans (abase, &check, &ref,
			 sxplocals.SXP_tables.P_ntmax, &next_action_kind);
	
	do {
	    set_next_shift_state (son, fathers_state, ref, check);
	} while (set_next_trans (abase, &check, &ref,
				 sxplocals.SXP_tables.P_ntmax, &next_action_kind)
		 && check > 0);
    }
}



static SXVOID process_sons (SXINT father, SXINT grand_fathers_state)
{
    /* father est un parser du squelette, on initialise les chemins issus
       de ses fils sans creer de "clone" de father. */
    /* Cette facon de proceder n'evite pas le clonage de peres multiples
       du squelette. */
    SXINT				x, son, state;

    if (SXBA_bit_is_reset_set (parse_stack.parsers_set [0], father)) {
	create_path (father, grand_fathers_state);
	state = XxYxZ_X (parse_stack.parsers, father);

	if ((son = parse_stack.parser_to_attr [father].son) < 0) {
	    son = -son;

	    XxY_Xforeach (parse_stack.sons, father, x) {
		process_sons (XxY_Y (parse_stack.sons, x), state);
	    }
	}

	if (son > 0)
	    process_sons (son, state);
    }
}



static SXINT clone (SXINT father, SXINT ref)
{
    /* A REGARDER, semble faux car "father" ne depend plus de "ref". */

    /* cree un clone de father en changeant "ref". */
    SXINT				state, son, x, new_father, xmscan;
    struct  parsers_attr	*pattr;

#if EBUG
    if (ref <= 0)
	sxtrap (ME, "clone");
#endif

    state = XxYxZ_X (parse_stack.parsers, father);
    son = parse_stack.parser_to_attr [father].son;

    if (son < 0) {
	son = -son;

	XxY_Xforeach (parse_stack.sons, father, x) {
	    new_parser (XxY_Y (parse_stack.sons, x), state, 0 /* symbol */);
	    
#if 0
	    seek_parser (XxY_Y (parse_stack.sons, x), state, ref, 0);
#endif
	}
    }

    new_father = new_parser (son, state, 0 /* symbol */);

    pattr = parse_stack.parser_to_attr + new_father;

    if (pattr->scan_ref == 0) {
	/* 1er scan */
	pattr->scan_ref = ref;
	pattr->next_for_scanner = *parse_stack.for_scanner.next_hd;
	*parse_stack.for_scanner.next_hd = father;
    }
    else if (pattr->scan_ref != ref && pattr->scan_ref != -ref) {
	/* Plusieurs scan_ref differents sont possibles
	   ds le cas de predicats differents  vrai simultanement (cas de non-determinisme)
	   associes au meme terminal ou en correction d'erreur.
	   Le non-determinisme (normal) produisant au plus un shift (cas du conflit
	   Shift/Reduce) et un ou des reduces (traites par mreds_push). */
	XxY_set (&parse_stack.mscans, father, ref, &xmscan);
	    
	if (pattr->scan_ref > 0)
	    pattr->scan_ref = -pattr->scan_ref;
    }

    return new_father;

#if 0
    return seek_parser (son, state, ref, 0);
#endif
}




static SXBOOLEAN is_a_right_ctxt (SXINT head, SXINT tail)
{
    /* On verifie que tokens [TOK_0 + head .. TOK_0 + tail]
       est un contexte droit valide. */
    /* Les parsers source sont dans parse_stack.for_scanner.next_hd */
    SXINT current, parser, xmscan, scan_ref;

    do {
	parse_stack.rcvr.milstn_head =
	    &(parse_stack.rcvr.milestones [head]);
	current = parse_stack.rcvr.milestones [head].next_for_scanner =
	    *parse_stack.for_scanner.next_hd;
	parse_stack.rcvr.milestones [head].next_milstn = NULL;

	sxplocals.rcvr.TOK_i = parse_stack.rcvr.TOK [head];
	parse_stack.current_milestone = parse_stack.current_token = sxplocals.rcvr.TOK_i;
	parse_stack.current_lahead = SXGET_TOKEN (sxplocals.rcvr.TOK_i).lahead;

	*parse_stack.for_scanner.next_hd = 0;
    
	for (parser = current;
	     parser > 0;
	     parser = parse_stack.parser_to_attr [parser].next_for_scanner) {
	    scan_ref = parse_stack.parser_to_attr [parser].scan_ref;

	    if (scan_ref < 0) {
		XxY_Xforeach (parse_stack.mscans, parser, xmscan) {
		    sxndsubparse_a_token (parser,
					  XxY_Y (parse_stack.mscans, xmscan),
					  0 /* symbol */,
					  (sxaction_new_top_t) NULL);
		}

		scan_ref = -scan_ref;
	    }

	    sxndsubparse_a_token (parser, scan_ref, 0 /* symbol */, (sxaction_new_top_t) NULL);
	}

	if (parse_stack.for_reducer.top > 0)
	    reducer ();

	if (parse_stack.current_lahead == sxplocals.SXP_tables.P_tmax
	    && *parse_stack.for_scanner.next_hd > 0
	    && XxYxZ_X (parse_stack.parsers, *parse_stack.for_scanner.next_hd)
	    == sxplocals.SXP_tables.final_state) {
	    tail = head; /* OK, checked */
	}
    } while (*parse_stack.for_scanner.next_hd > 0 && (++head <= tail));

    return head > tail;
}



static SXVOID nd2_try_a_correction (void)
{
    SXINT				z, dd, model_no, parser_seed, p2_ms;
    SXBOOLEAN			is_local_correction;
    struct sxmilstn_elem	*prev_ms_ptr;

    XxY_clear (&parse_stack.rcvr.seeds);
    sxba_empty (parse_stack.rcvr.prefixes_set);
    sxba_empty (parse_stack.rcvr.halt_hit_set);

    prev_ms_ptr = &sxmilstn_access (sxndtkn.milestones, parse_stack.rcvr.MS [0]);

    for (model_no = 1; model_no <= sxplocals.SXP_tables.P_nbcart; model_no++) {
	if (model_no >= parse_stack.rcvr.best.model_no
	    && !parse_stack.rcvr.best.a_la_rigueur)
	    break;

	if (check_for_spelling (model_no)) {
	    is_local_correction = SXFALSE;
	    
	    /* On poursuit si le modele courant est meilleur que le modele actuel
	       ou si le meilleur modele est un "a_la_rigueur" et si le modele courant
	       ne supprime pas d'elements de "no_delete". */
	    
	    if (sxplocals.SXP_tables.P_lregle [model_no] [1] == 0) {
		/* doit etre calcule pour chaque modele, peut utiliser TOK [0] */
		if ((dd = dont_delete (model_no)) >= 0) {
		    parse_stack.rcvr.dont_delete = dd > 0;

		    if ((model_no < parse_stack.rcvr.best.model_no
			 || !parse_stack.rcvr.dont_delete))
			is_local_correction =
			    try_a_model ((SXINT)1, prev_ms_ptr->next_for_scanner, model_no,
					 SXTRUE, parse_stack.rcvr.TOK [0]);
		}
	    }
	    else {
		XxYxZ_Zforeach (sxndtkn.dag, prev_ms_ptr->my_index, z) {
		    /* doit etre calcule pour chaque modele, peut utiliser TOK [0] */
		    if ((dd = dont_delete (model_no)) >= 0) {
			parse_stack.rcvr.dont_delete = dd > 0;
			
			if ((model_no < parse_stack.rcvr.best.model_no
			     || !parse_stack.rcvr.dont_delete)) {
			    p2_ms = XxYxZ_X (sxndtkn.dag, z);
			    parser_seed =
				sxmilstn_access (sxndtkn.milestones, p2_ms).next_for_scanner;
			    
			    /* On ne considere que les milestones non vides */
			    if (parser_seed > 0
				&& try_a_model ((SXINT)0, parser_seed, model_no,
						SXTRUE, XxYxZ_Y (sxndtkn.dag, z))) {
				is_local_correction = SXTRUE;
				break;
			    }
			}
		    }
		}
	    }
	    
	    if (is_local_correction)
		if (!parse_stack.rcvr.best.a_la_rigueur)
		    /* Un pur */
		    break;
	}
    }
}



static SXVOID nd2_unfold (SXINT prev_tok_no, SXINT mstn, SXINT n, SXINT lim)
{
    SXINT 			xyz, tok_no;
    struct sxmilstn_elem 	*mstn_ptr;

    if (mstn > 1 && SXGET_TOKEN (prev_tok_no).lahead == sxplocals.SXP_tables.P_tmax) {
	/* Le vrai EOF, pas celui du debut du texte source! repere ici par mstn == 1. */
	while (n <= lim) {
	    parse_stack.rcvr.TOK [n] = prev_tok_no;
	    parse_stack.rcvr.MS [n] = mstn;
	    n++;
	} 
    }

    parse_stack.rcvr.MS [n] = mstn;

    if (n > lim) {
	/* TOK [0..P_nmax] est rempli */
	nd2_try_a_correction ();
    }
    else {
	mstn_ptr = &sxmilstn_access (sxndtkn.milestones, mstn);
	
	while ((mstn_ptr->kind & MS_COMPLETE) == 0)
	    sxndscan_it ();
	
	XxYxZ_Xforeach (sxndtkn.dag, mstn, xyz) {
	    tok_no = parse_stack.rcvr.TOK [n] = XxYxZ_Y (sxndtkn.dag, xyz);
	    nd2_unfold (tok_no, XxYxZ_Z (sxndtkn.dag, xyz), n + 1, lim);
	}
    }
}


static SXBOOLEAN 	global_recovery (void)
{
    SXINT	tail, x, top, p, current_milestone;

    current_milestone = parse_stack.current_milestone;
    
    for (x = 1; x <= parse_stack.for_reducer.top; x++) {
	create_path (parse_stack.for_reducer.triples [x].parser,
		     (SXINT)-1 /* new parser */);
    }
    
    sxba_empty (parse_stack.parsers_set [0]);
    parse_stack.for_reducer.top = 0;

    tail = get_tail ((SXINT)0, sxplocals.SXP_tables.P_validation_length - 1);
    
    /* Toutes les "feuilles" sont dans parse_stack.rcvr.shifts */
    /* Les "nouvelles feuilles" sont dans parse_stack.for_scanner.next_hd. */
    
    if ((top = XxYxZ_top (parse_stack.rcvr.shifts)) > 0) {
	/* parse_stack.rcvr.shifts contient la liste des pre'tendants
	   ils ont tous une transition sur le terminal cle. */
	if (tail > 0) {
	    /* On filtre par le contexte */
	    if ((x = *parse_stack.for_scanner.next_hd) > 0) {
		/* Pour GC () */
		parse_stack.rcvr.milstn_head =
		    &(parse_stack.rcvr.milestones [0]);
		parse_stack.rcvr.milestones [0].next_for_scanner = x;
		parse_stack.rcvr.milestones [0].next_milstn = NULL;
	    }
	    
	    sxplocals.rcvr.TOK_i = parse_stack.rcvr.TOK [1];
	    parse_stack.current_milestone = --parse_stack.delta_generator;
	    parse_stack.current_token = sxplocals.rcvr.TOK_i;
	    parse_stack.current_lahead = SXGET_TOKEN (sxplocals.rcvr.TOK_i).lahead;
	    
	    for (x = 1; x <= top; x++) {
		*parse_stack.for_scanner.next_hd = 0;
		
		sxndsubparse_a_token (XxYxZ_X (parse_stack.rcvr.shifts, x),
				      XxYxZ_Y (parse_stack.rcvr.shifts, x),
				      0,
				      (sxaction_new_top_t) NULL);
		
		if (parse_stack.for_reducer.top > 0)
		    reducer ();
		
		if (tail > 1 && *parse_stack.for_scanner.next_hd > 0)
		    is_a_right_ctxt ((SXINT)1, tail);
		
		if (*parse_stack.for_scanner.next_hd == 0)
		    XxYxZ_erase (parse_stack.rcvr.shifts, x);
	    }
	}
	
	parse_stack.current_milestone = current_milestone;
	parse_stack.current_token = sxplocals.rcvr.TOK_0;
	parse_stack.current_lahead = SXGET_TOKEN (sxplocals.rcvr.TOK_0).lahead;
	*parse_stack.for_scanner.next_hd = 0;
	
	for (x = 1; x <= top; x++) {
	    if (!XxYxZ_is_erased (parse_stack.rcvr.shifts, x)) {
		p = XxYxZ_X (parse_stack.rcvr.shifts, x);
		
		if (parse_stack.parser_to_attr [p].milestone != parse_stack.current_milestone) {
		    /* C'est un parser du squelette, il faut donc le cloner
		       avec les bonnes valeurs. */
		    p = clone (p, XxYxZ_Y (parse_stack.rcvr.shifts, x));
		    /* parse_stack.for_scanner.next_hd est maj par clone () */
		}
		else {
		    parse_stack.parser_to_attr [p].next_for_scanner =
			*parse_stack.for_scanner.next_hd;
		    *parse_stack.for_scanner.next_hd = p;
		    /* Suspect, peut-il y avoir plusieurs scan_ref pour le meme p ? */
		    parse_stack.parser_to_attr [p].scan_ref = XxYxZ_Y (parse_stack.rcvr.shifts, x);
		}
	    }
	}
	
	XxYxZ_clear (&parse_stack.rcvr.shifts);
	parse_stack.rcvr.milstn_head = NULL;
    }
    
    return *parse_stack.for_scanner.next_hd > 0;
}



static SXINT nd2_search_validation_context (struct sxmilstn_elem *orig_mstn_ptr,
					    SXINT mstn_nb, 
					    SXINT n, 
					    SXINT lim)
{
    
    struct sxmilstn_elem 	*mstn_ptr;
    SXINT				xyz, orig_tok_no, ret_val, parser;

    parse_stack.rcvr.MS [n] = mstn_nb;

    if (n == lim) {
	parse_stack.current_milestone =
	    sxmilstn_access (sxndtkn.milestones, parse_stack.rcvr.MS [0]).my_milestone;
	parse_stack.current_token = sxplocals.rcvr.TOK_0;
	parse_stack.current_lahead = SXGET_TOKEN (sxplocals.rcvr.TOK_0).lahead;

	*parse_stack.for_scanner.next_hd = 0;

	XxYxZ_Zforeach (sxndtkn.dag, orig_mstn_ptr->my_index, xyz) {
	    orig_tok_no = XxYxZ_Y (sxndtkn.dag, xyz);
    
	    for (parser = sxmilstn_access (sxndtkn.milestones,
					   XxYxZ_X (sxndtkn.dag, xyz)).next_for_scanner;
		 parser > 0;
		 parser = parse_stack.parser_to_attr [parser].next_for_scanner) {
		if (XxYxZ_Y (parse_stack.parsers, parser) == orig_tok_no)
		    process_sons (parser, (SXINT)0 /* father's state : son is top state */);
	    }
	}

	if (global_recovery ())
	    return SXTRUE;
	else
	    return SXFALSE;
    }

    mstn_ptr = &sxmilstn_access (sxndtkn.milestones, mstn_nb);
    
    while ((mstn_ptr->kind & MS_COMPLETE) == 0)
	sxndscan_it ();
    
    XxYxZ_Xforeach (sxndtkn.dag, mstn_nb, xyz) {
	sxplocals.rcvr.TOK_i = parse_stack.rcvr.TOK [n] = XxYxZ_Y (sxndtkn.dag, xyz);

	if (SXGET_TOKEN (sxplocals.rcvr.TOK_i).lahead == sxplocals.SXP_tables.P_tmax)
	    ret_val = -sxplocals.rcvr.TOK_i;
	else
	    ret_val = nd2_search_validation_context (orig_mstn_ptr,
						     XxYxZ_Z (sxndtkn.dag, xyz),
						     n + 1,
						     lim);

	if (ret_val != 0)
	    return ret_val;	/* eof hit ou OK */
    }

    return 0;
}


static SXINT nd2_search_key_terminal (void)
{
    /* Recherche d'un milestone dont est issu un terminal-cle */
    SXINT				xyz, next_mstn_nb, mstn_nb, lahead, i, ret_val, tok_no;
    struct sxmilstn_elem 	*orig_mstn_ptr = sxndtkn.milstn_current;

    for (;;) {
	while ((sxndtkn.milstn_current->kind & MS_COMPLETE) == 0)
	    sxndscan_it ();
	
	/* sera recupere par le mecanisme normal */
	sxndtkn.milstn_current->kind |= MS_EMPTY;
	mstn_nb = sxndtkn.milstn_current->my_index;

	XxYxZ_Xforeach (sxndtkn.dag, mstn_nb, xyz) {
	    tok_no = XxYxZ_Y (sxndtkn.dag, xyz);
	    lahead = SXGET_TOKEN (tok_no).lahead;
	    
	    if (lahead == sxplocals.SXP_tables.P_tmax)
		return -tok_no;

	    if ((i = sxgetbit (sxplocals.SXP_tables.PER_tset, lahead)) > 0) {
		/* terminal courant == terminal cle */
		parse_stack.rcvr.trans = sxplocals.SXP_tables.PER_trans_sets != NULL
		    ? sxplocals.SXP_tables.PER_trans_sets [i]
			: NULL;
		sxplocals.rcvr.TOK_0 = parse_stack.rcvr.TOK [0] = tok_no ;
		parse_stack.rcvr.MS [0] = mstn_nb;
		next_mstn_nb = XxYxZ_Z (sxndtkn.dag, xyz);
		
		ret_val = nd2_search_validation_context (orig_mstn_ptr,
							 next_mstn_nb,
							 (SXINT)1,
							 sxplocals.SXP_tables.P_validation_length);

		if (ret_val != 0) {
		    if (ret_val > 0) {
			sxndtkn.milstn_current->next_for_scanner =
			    *parse_stack.for_scanner.next_hd;
			*parse_stack.for_scanner.next_hd = 0;
			sxndtkn.milstn_current->kind |= MS_WAIT; /* calcule' non vide */

			while ((sxndtkn.milstn_current =
				sxndtkn.milstn_current->next_milstn) != NULL
			       && sxndtkn.milstn_current->my_index != next_mstn_nb) {
			    sxndtkn.milstn_current->kind |= MS_EMPTY;
			}

			/* sxndtkn.milstn_current ==
			    &sxmilstn_access (sxndtkn.milestones, next_mstn_nb) */
		    }

		    return ret_val;
		}
	    }
	}

	sxndtkn.milstn_current = sxndtkn.milstn_current->next_milstn;
    }
}



static SXBOOLEAN	less_equal (SXINT z1, SXINT z2)
{
    return XxYxZ_X (sxndtkn.dag, z1) <= XxYxZ_X (sxndtkn.dag, z2);
}




static SXVOID nd2_local_correction (void)
{
    /* Dans un premier temps, on s'occupe uniquement de milstn_current.
       Il serait possible de tenter des corrections (meme eventuellement multiples)
       sur les milestones MS_EMPTY precedents. */
    SXINT 			prev_ms_nb, cur_ms_nb, z, preds_nb, parser;
    struct sxmilstn_elem	*prev_ms_ptr;
	    
    parse_stack.rcvr.milstn_head = NULL;
    
    /* On traite les predecesseurs de milstn_current par valeur de milestone
       decroissante (progression maximale ds le texte source). */

    /* On le repositionne correctement */
    sxndtkn.milstn_current->kind &= MS_COMPLETE + MS_BEGIN;
    /* sxndtkn.milstn_current->next_for_scanner == 0 */
    preds_nb = 0;
    cur_ms_nb = sxndtkn.milstn_current->my_index;

    XxYxZ_Zforeach (sxndtkn.dag, cur_ms_nb, z) {
	prev_ms_nb = XxYxZ_X (sxndtkn.dag, z);

	if (sxmilstn_access (sxndtkn.milestones, prev_ms_nb).kind & MS_WAIT) {
	    if (++preds_nb > parse_stack.rcvr.preds_size) {
		parse_stack.rcvr.preds = (SXINT*) sxrealloc (parse_stack.rcvr.preds,
							   (parse_stack.rcvr.preds_size *= 2) + 1,
							   sizeof (SXINT));
		parse_stack.rcvr.to_be_sorted = (SXINT*) sxrealloc (parse_stack.rcvr.to_be_sorted,
								  parse_stack.rcvr.preds_size + 1,
								  sizeof (SXINT));
	    }

	    parse_stack.rcvr.to_be_sorted [preds_nb] = preds_nb;
	    parse_stack.rcvr.preds [preds_nb] = z;
	}
    }
    
    if (preds_nb > 1)
	sort_by_tree (parse_stack.rcvr.to_be_sorted, 1, preds_nb, less_equal);

    /* On lit P_nmax tokens */	    

    while (preds_nb > 0) {
	z = parse_stack.rcvr.preds [parse_stack.rcvr.to_be_sorted [preds_nb]];
	prev_ms_nb = XxYxZ_X (sxndtkn.dag, z);
	prev_ms_ptr = &sxmilstn_access (sxndtkn.milestones, prev_ms_nb);
		
	parse_stack.rcvr.TOK [0] = XxYxZ_Y (sxndtkn.dag, z);
	parse_stack.rcvr.MS [0] = prev_ms_nb;

	if ((sxplocals.mode.kind & SXWITH_CORRECTION)
	    && sxplocals.SXP_tables.P_nbcart > 0)
	    nd2_unfold (parse_stack.rcvr.TOK [0], cur_ms_nb, (SXINT)1, sxplocals.SXP_tables.P_nmax);
	    
	if (parse_stack.rcvr.best.model_no > sxplocals.SXP_tables.P_nbcart + 1) {
	    /* pas de correction et pas d'insertion forcee */ 
	    if (parse_stack.rcvr.index_0X > 0
		&& (parser = parse_stack.rcvr.prefixes_to_milestones
		    [parse_stack.rcvr.index_0X].next_for_scanner) > 0)
		/* A pu ne pas etre calcule' (presence d'eof). */
		/* On extrait vt_set */
		fill_vt_set (sxplocals.rcvr.vt_set, parser);
	    else
		compute_vt_set (sxplocals.rcvr.vt_set, prev_ms_ptr->next_for_scanner);
		
	    z = sxba_scan (sxplocals.rcvr.vt_set, 0);
		
	    if (sxba_scan (sxplocals.rcvr.vt_set, z) > 0 /* multiple */
		|| z == sxplocals.SXP_tables.P_tmax)
		z = 0;

	    if (parse_stack.rcvr.best.model_no > sxplocals.SXP_tables.P_nbcart + 2
		|| z > 0) {
		parse_stack.rcvr.best.model_no = sxplocals.SXP_tables.P_nbcart + 
		    (z == 0 ? 2 : 1);
		/* marquage de l'erreur sur parse_stack.rcvr.TOK [1] */
		if (z == 0)
		    /* On prend le premier */
		    sxba_copy (parse_stack.rcvr.best.vt_set, sxplocals.rcvr.vt_set);
		else
		    parse_stack.rcvr.best.single_la = z;
	    }
	}
	
	preds_nb--;
    }
    
    /* On remet tout en etat */
    parse_stack.rcvr.milstn_head = NULL; /* Pour GC () */
    *parse_stack.for_scanner.next_hd = 0;

    /* Dans tous les cas (correction, insertion forcee, rattrapage global)
       l'erreur sera marquee (listing) sur le token sur lequel on a choisi de
       faire le rattrapage. */
    parse_stack.rcvr.source_index = SXGET_TOKEN (parse_stack.rcvr.TOK [1]).source_index;
} 



static SXVOID sort_milestones (void)
{
    /* On lie les milestones afin que le parser execute ca ds le bon ordre. */

    /* Il faut :
          - connaitre le numero du token a partir duquel se refera l'analyse
	    sur un texte sequentiel
          - que toutes les transitions entrantes de la derniere milestone soient
	    sur le meme token
	  - etre sur qu'aucune erreur ne sera detectee en lisant le dag des tokens. */
    struct sxmilstn_elem	*cur_ms_ptr, *prev_ms_ptr;
    SXINT				parser, tde, my_milestone, node, prev_node;

    /* Si parse_stack.for_scanner.previouses [parse_stack.rcvr.prev_top] est non nul,
       on "recalculera" le noeud 1 (a partir du noeud 0), ce nouveau calcul prendra donc
       en compte toutes les corrections issues de ce noeud. Pour ne pas avoir d'ennuis
       le GC de sortie d'erreur doit "recuperer" les parsers de parse_stack.rcvr.cur_top. */

    /* mm [1] == 1 */

    sxinitialise(prev_node); /* pour faire taire "gcc -Wuninitialized" */

    if ((parser = parse_stack.for_scanner.previouses [parse_stack.rcvr.prev_top]) != 0) {
	parse_stack.for_scanner.previouses [parse_stack.rcvr.cur_top] = 0; /* pour GC */
	parse_stack.rcvr.milstn_elems [0].next_for_scanner = parser;
	parse_stack.rcvr.milstn_elems [0].my_milestone =
	    parse_stack.parser_to_attr [parser].milestone;
	parse_stack.rcvr.milstn_elems [1].my_milestone = -1;

	/* Transition initiale */
	XxYxZ_set (&sxndtkn.dag,
		   0,
		   XxYxZ_Y (parse_stack.parsers, parser),
		   1,
		   &tde);

    }
    else {
	parse_stack.rcvr.milstn_elems [1].next_for_scanner =
	    parse_stack.for_scanner.previouses [parse_stack.rcvr.cur_top];
	parse_stack.rcvr.milstn_elems [1].my_milestone = parse_stack.rcvr.MS [0];
    }

    /* On numerote et lie les milestones ds l'ordre d'execution. */
    /* Il faut etre sur de ne pas reconstruire les memes symboles que ceux qui peuvent
       etre invalides par la correction, le premier milestone est donc superieur au
       milestone du token en erreur. */
    my_milestone = parse_stack.rcvr.MS [1];
    prev_ms_ptr = NULL;

    for (node = 1; node <= parse_stack.rcvr.milstn_nb; node++) {
	if (parse_stack.rcvr.milstn_elems [node].my_milestone != 0) {
	    parse_stack.rcvr.milstn_elems [prev_node = node].my_milestone = ++my_milestone;

	    cur_ms_ptr = &(parse_stack.rcvr.milstn_elems [node]);
	    cur_ms_ptr->prev_milstn = prev_ms_ptr;
	    cur_ms_ptr->next_milstn = NULL;
	    cur_ms_ptr->next_for_scanner = 0;

	    if (prev_ms_ptr == NULL)
		parse_stack.milstn_tail = cur_ms_ptr;
	    else
		prev_ms_ptr->next_milstn = cur_ms_ptr;

	    prev_ms_ptr = cur_ms_ptr;
	}
    }

    /* On positionne ptok_no */
    sxplocals.ptok_no = sxplocals.Mtok_no;

#if EBUG
    if (my_milestone > sxplocals.ptok_no + 1)
	sxtrap (ME, "sort_milestones");
#endif

    /* On renomme le dernier "node" pour la compatibilite avec la facon dont le parser
       calcule "current_milestone". */
    parse_stack.rcvr.milstn_elems [prev_node].my_milestone = sxplocals.ptok_no + 1;

    sxplocals.rcvr.TOK_i = parse_stack.rcvr.best.TOK [parse_stack.rcvr.Mrch];

    /* On "renomme" les tokens qui conduisent a une analyse "sequentielle". */
    while (++sxplocals.rcvr.TOK_i <= parse_stack.rcvr.Mtok_no) {
	sxput_token (SXGET_TOKEN (sxplocals.rcvr.TOK_i));
    }
}



static SXBOOLEAN	ndp_recovery (void)
{
    char				*msg_params [5];
    struct SXP_local_mess		*local_mess;
    struct sxtoken			*tok;
    
    SXBOOLEAN			with_semact = sxplocals.mode.with_semact;
    SXBOOLEAN			ret_val = SXTRUE;
    SXINT				mode = sxplocals.mode.mode;
    SXINT				i, j, ate;
    SXINT				vt_set_card;
    
    sxinitialise(tok); /* pour faire taire gcc -Wuninitialized */
    /* On commence par un petit GC afin de ne pas laisser trainer des parsers reduce
       ayant le milestone associe au token en erreur, le traitement d'erreur ou l'analyse
       apres traitement d'erreur va peut etre essayer de refabriquer de tels parser, il ne faut
       pas trouver qu'ils existaient deja. */

    sxndparser_GC (0);

    /* Le GC effectue a l'entree de rcvr a supprime tous les parsers dont le second
       composant est le tok_no du terminal en erreur (ces parsers correspondent a
       des reductions effectuees sans look-ahead et qui n'ont pas ete validees).
       Il est donc possible de reutiliser le tok_no du terminal en erreur.
       Pour pouvoir reutiliser (calculer des parsers avec) le tok_no du terminal precedent
       le terminal en erreur, il faut faire qqch d'analogue i.e.
       Si parse_stack.for_scanner.previouses [parse_stack.rcvr.prev_top] est non nul,
       le mettre a zero et faire un GC. Cette possibilite est utilisee ds le cas de
       corrections multiples afin de prendre en compte les corrections (eventuelles)
       sur le token zero (voir sort_milestones). */

    parse_stack.rcvr.is_up = SXTRUE;
    sxplocals.mode.with_semact = SXFALSE;
    parse_stack.delta_generator = 0;

    /* + 1 => vt_set_card == 1
       + 2 => vt_set_card > 1
       + 3 => non calcule' */
    parse_stack.rcvr.best.model_no = sxplocals.SXP_tables.P_nbcart + 3; /* vide */

    /* Meme si on n'a pas demande' de correction, positionne vt_set */
    if (sxplocals.sxtables->SXS_tables.S_is_non_deterministic)
	nd2_local_correction ();
    else
	local_correction ();

    /* Si, dans la suite on a besoin ne nouveaux milestones... */
    parse_stack.delta_generator -= (sxplocals.SXP_tables.P_tmax + 1);

    if (parse_stack.rcvr.best.model_no <= sxplocals.SXP_tables.P_nbcart) {
	if (parse_stack.rcvr.all_corrections)
	    sort_milestones ();
	else
	    one_correction_message ();
    }
    else if (sxplocals.mode.kind & SXWITH_RECOVERY) {
	/* best.vt_set est remplit */
	if (parse_stack.rcvr.best.model_no == sxplocals.SXP_tables.P_nbcart + 1) {
	    /* cas de l'insertion forcee... */
	    struct sxtoken toks;

	    sxplocals.mode.local_errors_nb++;
	    /* un seul suivant valide different de EOF ==> insertion forcee */
	    /* Pas de predicats dans le coup */
	    local_mess = sxplocals.SXP_tables.SXP_local_mess + sxplocals.SXP_tables.P_nbcart + 1;
	    
	    /* preparation du message */
	    for (j = local_mess->P_param_no - 1; j >= 0; j--) {
		i = local_mess->P_param_ref [j];

		if (i < 0)
		    msg_params [j] = ttext (sxplocals.sxtables, parse_stack.rcvr.best.single_la);
		else {
		    sxplocals.rcvr.TOK_i = parse_stack.rcvr.TOK [i]; 
		    tok = &(SXGET_TOKEN (sxplocals.rcvr.TOK_i));
		    ate = tok->string_table_entry;
		    msg_params [j] = (ate == SXEMPTY_STE || ate == SXERROR_STE
				      ? ttext (sxplocals.sxtables, tok->lahead)
				      : sxstrget (ate));
		}
	    }
	    
	    /* insertion du nouveau token, avec source_index et comment */
	    toks.lahead = parse_stack.rcvr.best.single_la;
	    toks.string_table_entry = SXERROR_STE /* error value */ ;
	    toks.comment = NULL;
	    toks.source_index = parse_stack.rcvr.source_index;
	    
	    if (sxplocals.sxtables->SXS_tables.S_is_non_deterministic) {
		SXINT			cur_ms, tde, xyz;
		struct sxmilstn_elem	*milstn_error;

		/* On insere un nouveau milestone */
		milstn_error = milestone_new (sxndtkn.milstn_current,
					      ++sxndtkn.last_milestone_no,
					      MS_BEGIN | MS_COMPLETE);

		cur_ms = milstn_error->my_index;

		/* On "duplique" les anciennes transitions */
		XxYxZ_Xforeach (sxndtkn.dag, sxndtkn.milstn_current->my_index, tde) {
		    sxplocals.rcvr.TOK_i = XxYxZ_Y (sxndtkn.dag, tde);
		    sxput_token (SXGET_TOKEN (sxplocals.rcvr.TOK_i));
		    XxYxZ_set (&sxndtkn.dag,
			       cur_ms,
			       sxplocals.Mtok_no,
			       XxYxZ_Z (sxndtkn.dag, tde),
			       &xyz);
		}

		/* Creation du nouveau token en sxplocals.Mtok_no */
		sxput_token (toks);

		/* On remplit dag */
		XxYxZ_set (&sxndtkn.dag,
			   sxndtkn.milstn_current->my_index,
			   sxplocals.Mtok_no,
			   cur_ms,
			   &tde);
	    }
	    else {
		sxtknmdf (&toks, 1, sxplocals.ptok_no, 0);
		
		parse_stack.for_scanner.top = FS_DECR (parse_stack.for_scanner.top);
		sxplocals.ptok_no = sxplocals.rcvr.TOK_0;
	    }
	    
	    /* "Warning: "machin" is forced before chose." ou similaire */
	    if (!sxplocals.mode.is_silent)
		sxerror (parse_stack.rcvr.source_index,
			 sxgenericp (sxplocals.sxtables, parse_stack.rcvr.best.single_la)
			 ? sxplocals.sxtables->err_titles [2][0]
			 : sxplocals.sxtables->err_titles [1][0],
			 local_mess->P_string,
			 sxgenericp (sxplocals.sxtables, parse_stack.rcvr.best.single_la)
			 ? sxplocals.sxtables->err_titles [2]+1
			 : sxplocals.sxtables->err_titles [1]+1,
			 msg_params [0],
			 msg_params [1],
			 msg_params [2],
			 msg_params [3],
			 msg_params [4]);
	    
	}
	else {
	    /* rattrapage global */
	    sxplocals.mode.global_errors_nb++;
	    sxplocals.mode.mode = SXRECOGNIZER;
	    /* message sur les suivants attendus */
	    vt_set_card = sxba_cardinal (parse_stack.rcvr.best.vt_set);
	    
	    if (vt_set_card <= sxplocals.SXP_tables.P_followers_number) {
		i = 0;
		j = 0;
		
		while ((j = sxba_scan (parse_stack.rcvr.best.vt_set, j)) > 0) {
		    msg_params [i++] = ttext (sxplocals.sxtables, j);
		}
		
		/* "Error: "machin" is expected." */
		
		if (!sxplocals.mode.is_silent)
		    sxerror (parse_stack.rcvr.source_index,
			     sxplocals.sxtables->err_titles [2][0],
			     sxplocals.SXP_tables.P_global_mess [vt_set_card - 1],
			     sxplocals.sxtables->err_titles [2]+1,
			     msg_params [0],
			     msg_params [1],
			     msg_params [2],
			     msg_params [3],
			     msg_params [4]);
	    }
	    
	    
	    /* "Error: Global Recovery." */
	    
	    if (!sxplocals.mode.is_silent)
		sxerror (parse_stack.rcvr.source_index,
			 sxplocals.sxtables->err_titles [2][0],
			 sxplocals.SXP_tables.P_global_mess
			 [sxplocals.SXP_tables.P_followers_number],
			 sxplocals.sxtables->err_titles [2]+1);
	    
	    /* on y va */
	    
	    if (sxplocals.sxtables->SXS_tables.S_is_non_deterministic) {
		if ((i = nd2_search_key_terminal ()) < 0) {
		    /* on a trouve la fin du fichier */
		    /* "Error: Parsing stops on End of File." */
		    tok = &(SXGET_TOKEN (-i));
		    ret_val = SXFALSE;
		}
		else
		  ret_val = SXTRUE;
	    }
	    else {
		for (;;) {
		    sxget_token (sxplocals.rcvr.TOK_0 + sxplocals.SXP_tables.P_validation_length);
		    
		    /* avaler un token */
		    sxplocals.rcvr.TOK_0++;
		    tok = &(SXGET_TOKEN (sxplocals.rcvr.TOK_0));
		    
		    if (tok->lahead == sxplocals.SXP_tables.P_tmax) {
			/* Les conse'quences de la non re'cupe'ration seront traite'es par
			   le parser. */
			sxplocals.ptok_no = sxplocals.rcvr.TOK_0;
			ret_val = SXFALSE;
			break;
		    }
		    
		    if ((i = sxgetbit (sxplocals.SXP_tables.PER_tset, tok->lahead)) > 0) {
			/* terminal courant == terminal cle */
			/* on regarde si les P_validation_length terminaux suivants
			   sont des suivants valides, puis sinon on "pop" la pile et
			   on recommence; si on tombe au fond de la pile on avance
			   d'un token */
			parse_stack.rcvr.trans = sxplocals.SXP_tables.PER_trans_sets != NULL
			    ? sxplocals.SXP_tables.PER_trans_sets [i]
				: NULL;
			
			for (i = 0; i < sxplocals.SXP_tables.P_validation_length; i++)
			    parse_stack.rcvr.TOK [i] = sxplocals.rcvr.TOK_0 + i;
			
			parse_stack.current_milestone =
			    parse_stack.current_token = sxplocals.rcvr.TOK_0;
			parse_stack.current_lahead = SXGET_TOKEN (sxplocals.rcvr.TOK_0).lahead;
			
			/* We look for all visible parsers from current_for_scanner. */
			*parse_stack.for_scanner.next_hd = 0;
			
			for (i = *parse_stack.for_scanner.current_hd;
			     i > 0;
			     i = parse_stack.parser_to_attr [i].next_for_scanner) {
			    process_sons (i, (SXINT)0 /* father's state : son is top state */);
			}

			if (global_recovery ()) {
			    ret_val = SXTRUE;
			    break;
			}
		    }
		}
		
		parse_stack.for_scanner.previouses [parse_stack.rcvr.cur_top] = 0;
		parse_stack.for_scanner.previouses [parse_stack.rcvr.prev_top] = 0;
		sxplocals.ptok_no = sxplocals.rcvr.TOK_0;
		parse_stack.for_scanner.next_hd =
		    parse_stack.for_scanner.previouses + parse_stack.for_scanner.top;
	    }
	    
	    if (ret_val) {
		if ((i = sxplocals.SXP_tables.P_validation_length) > 5)
		    i = 5;
		
		while (i-- > 0) {
		    sxplocals.rcvr.TOK_i = parse_stack.rcvr.TOK [i];
		    tok = &(SXGET_TOKEN (sxplocals.rcvr.TOK_i));
		    msg_params [i] = (ate = tok->string_table_entry) == SXEMPTY_STE
			|| ate == SXERROR_STE
			    ? ttext (sxplocals.sxtables, tok->lahead)
			    : sxstrget (ate);
		}
		
		/* "Warning: Parsing resumes on "machin"." */
		if (!sxplocals.mode.is_silent)
		    sxerror ((SXGET_TOKEN (sxplocals.rcvr.TOK_0)).source_index,
			     sxplocals.sxtables->err_titles [1][0],
			     sxplocals.SXP_tables.P_global_mess
			     [sxplocals.SXP_tables.P_followers_number + 1],
			     sxplocals.sxtables->err_titles [1]+1,
			     msg_params [0],
			     msg_params [1],
			     msg_params [2],
			     msg_params [3],
			     msg_params [4]);
		
	    }
	    else {
		/* on a trouve la fin du fichier */
		/* "Error: Parsing stops on End of File." */
		if (!sxplocals.mode.is_silent)
		    sxerror (tok->source_index,
			     sxplocals.sxtables->err_titles [2][0],
			     sxplocals.SXP_tables.P_global_mess
			     [sxplocals.SXP_tables.P_followers_number + 3],
			     sxplocals.sxtables->err_titles [2]+1);
		
	    }
	}
    }
    else
	ret_val = SXFALSE;
    
    /* Il FAUT lancer le GC avant de retourner :
       - les etats crees par is_a_right_ctxt ne peuvent etre reutilises : puisqu'ils
         existent deja, ils sont sense etre actif et donc ne seront pas empiles
         (et donc pas traites) ni dans for_scanner ni dans for_reducer
       - On recupere explicitement la place de la correction locale afin de ne pas
         laisser trainer de champs tok_no negatifs qui pourraient interferer avec
         une utilisation ulterieure (calcul de vt_set ou traitement d'une autre
         erreur)
       - De plus si correction multiple, on peut devoir recalculer les parser associes
         au milestone du token precedent le token en erreur.
    */
    sxndparser_GC (0);

    parse_stack.delta_generator = 0;
    sxplocals.mode.mode = mode;
    sxplocals.mode.with_semact = with_semact;
    parse_stack.rcvr.is_up = SXFALSE;
    parse_stack.halt_hit = SXFALSE; /* Au cas ou */

    return ret_val;
}



SXBOOLEAN		sxndprecovery (SXINT what_to_do)
{
    static SXINT	rcvr_shifts_foreach [] = {0, 0, 0, 0, 0, 0};
    
    switch (what_to_do) {
    case SXOPEN:
	parse_stack.rcvr.all_corrections = SXFALSE;
	sxplocals.rcvr.vt_set = NULL;
	break;

    case SXINIT:
	/* valeurs par defaut qui peuvent etre changees ds les
	   scan_act ou pars_act. */
	sxplocals.rcvr.truncate_context = SXTRUE;
	parse_stack.rcvr.with_parsprdct = SXTRUE;

	/* Allocation will be done at first call. */
	break;

    case SXACTION:
	if (sxplocals.rcvr.vt_set == NULL) {
	    SXINT 	x;
	    SXINT   	sizofpts = sxplocals.SXP_tables.P_sizofpts;
	    
	    sxplocals.rcvr.com = (char**) sxalloc (sizofpts + 1, sizeof (char*));
	    sxplocals.rcvr.lter = (SXINT*) sxalloc (sizofpts + 1, sizeof (SXINT));
	    sxplocals.rcvr.toks = (struct sxtoken*)
		sxalloc (sizofpts + 1, sizeof (struct sxtoken));
	    sxplocals.rcvr.vt_set = sxba_calloc (sxplocals.SXP_tables.P_tmax + 1);
	    parse_stack.rcvr.best.vt_set = sxba_calloc (sxplocals.SXP_tables.P_tmax + 1);
	    sxplocals.rcvr.mvector = sxplocals.SXP_tables.vector + sxplocals.SXP_tables.mxvec;
	    sxplocals.rcvr.Mvector = sxplocals.SXP_tables.vector + sxplocals.SXP_tables.Mxvec;
	    parse_stack.rcvr.preds = (SXINT*) sxalloc ((parse_stack.rcvr.preds_size = 10) + 1,
						     sizeof (SXINT));
	    parse_stack.rcvr.to_be_sorted = (SXINT*) sxalloc (parse_stack.rcvr.preds_size + 1,
							    sizeof (SXINT));


	    x = sxplocals.SXP_tables.P_nmax > sxplocals.SXP_tables.P_validation_length
		? sxplocals.SXP_tables.P_nmax : sxplocals.SXP_tables.P_validation_length;
	    parse_stack.rcvr.TOK = (SXINT*) sxalloc (x + 1, sizeof (SXINT));
	    parse_stack.rcvr.MS = (SXINT*) sxalloc (x + 2, sizeof (SXINT));
	    parse_stack.rcvr.best.TOK = (SXINT*) sxalloc (x + 1, sizeof (SXINT));
	    parse_stack.rcvr.best.MS = (SXINT*) sxalloc (x + 2, sizeof (SXINT));
	    
	    parse_stack.rcvr.index_0X = 0;

	    if ((sxplocals.mode.kind & SXWITH_CORRECTION) && sxplocals.SXP_tables.P_nbcart > 0) {
		SXINT	sxndprecovery_index, model_no, best_l, mn, l, best_model_no;
		SXINT	*m, *cur_model;

		sxinitialise(best_model_no); /* pour faire taire "gcc -Wuninitialized" */
		XxY_alloc (&parse_stack.rcvr.seeds, "rcvr_seeds", 200, 1, 0, 1, (sxoflw0_t) NULL, NULL); 
		parse_stack.rcvr.best.correction =
		    (SXINT*) sxalloc (sxplocals.SXP_tables.P_sizofpts + 1, sizeof (SXINT));
		parse_stack.rcvr.prefixes = (SXINT**) sxalloc (sxplocals.SXP_tables.P_nbcart + 1,
							     sizeof (SXINT*));
			    
		sxndprecovery_index = 0;

		for (model_no = 1; model_no <= sxplocals.SXP_tables.P_nbcart; model_no++) {
		    cur_model = sxplocals.SXP_tables.P_lregle [model_no];
		    parse_stack.rcvr.prefixes [model_no] = (SXINT*)
			sxalloc (cur_model [0] + 1, sizeof (SXINT));

		    best_l = 0;

		    for (mn = 1; mn < model_no; mn++) {
			m = sxplocals.SXP_tables.P_lregle [mn];

			for (l = 1; l <= cur_model [0]; l++) {
			    if (m [l] != cur_model [l]
				&& !(m [l] < 0 && cur_model [l] < 0)) {
				if (l - 1 > best_l) {
				    best_model_no = mn;
				    best_l = l - 1;
				}

				break;
			    }
			}
		    }

		    if (best_l > 0) {
			for (l = 1; l <= best_l; l++) {
			   parse_stack.rcvr.prefixes [model_no] [l] = 
			      parse_stack.rcvr.prefixes [best_model_no] [l] ;
			}
		    }

		    for (l = best_l + 1; l <= cur_model [0]; l++) {
			parse_stack.rcvr.prefixes [model_no] [l] = ++sxndprecovery_index;
		    }

		    if (parse_stack.rcvr.index_0X == 0
			&& cur_model [1] == 0
			&& cur_model [2] < 0)
			parse_stack.rcvr.index_0X = parse_stack.rcvr.prefixes [model_no] [2];
		}

		parse_stack.rcvr.prefixes_to_milestones =
		    (struct sxmilstn_elem *) sxalloc (sxndprecovery_index + 1, sizeof (struct sxmilstn_elem));
		parse_stack.rcvr.prefixes_set = sxba_calloc (sxndprecovery_index + 1);
		parse_stack.rcvr.halt_hit_set = sxba_calloc (sxndprecovery_index + 1);
	    }

	    parse_stack.rcvr.milestones =
		    (struct sxmilstn_elem *) sxalloc (sxplocals.SXP_tables.P_validation_length + 1,
						      sizeof (struct sxmilstn_elem));

	    XxYxZ_alloc (&parse_stack.rcvr.shifts, "rcvr_shifts", 31, 1,
			 rcvr_shifts_foreach, (sxoflw0_t) NULL, NULL);

	}

	return ndp_recovery ();

    case SXFINAL:
	break;

    case SXCLOSE:
	if (sxplocals.rcvr.vt_set != NULL) {
	    sxfree (sxplocals.rcvr.vt_set), sxplocals.rcvr.vt_set = NULL;
	    sxfree (parse_stack.rcvr.best.vt_set);
	    sxfree (sxplocals.rcvr.com);
	    sxfree (sxplocals.rcvr.lter);
	    sxfree (sxplocals.rcvr.toks);
	    sxfree (parse_stack.rcvr.TOK);
	    sxfree (parse_stack.rcvr.MS);
	    sxfree (parse_stack.rcvr.best.TOK);
	    sxfree (parse_stack.rcvr.best.MS);
	    sxfree (parse_stack.rcvr.preds);
	    sxfree (parse_stack.rcvr.to_be_sorted);

	    if ((sxplocals.mode.kind & SXWITH_CORRECTION)
		&& sxplocals.SXP_tables.P_nbcart > 0) {   
		SXINT model_no;

		XxY_free (&parse_stack.rcvr.seeds);
		sxfree (parse_stack.rcvr.best.correction);
		sxfree (parse_stack.rcvr.prefixes_to_milestones);
		sxfree (parse_stack.rcvr.prefixes_set);
		sxfree (parse_stack.rcvr.halt_hit_set);

		for (model_no = 1; model_no <= sxplocals.SXP_tables.P_nbcart; model_no++)
		    sxfree (parse_stack.rcvr.prefixes [model_no]);

		sxfree (parse_stack.rcvr.prefixes);
	    }

	    sxfree (parse_stack.rcvr.milestones);
	    XxYxZ_free (&parse_stack.rcvr.shifts);
	}
	
	break;

    default:
	fprintf (sxstderr, "The function \"sxndprecovery\" is out of date with respect to its specification.\n");
	sxexit(1);
    }

    return SXTRUE;
}


/* Tout ca peut etre construit statiquement... */
static SXVOID models_dag (void)
{
    /* On construit le dag (pas de boucle!) des modeles
       Les noeuds sont des milestones
       Les transitions les elements des modeles
       On partage les prefixes et les suffixes connus. Ex :
          0 X 1 2 et 0 X X 1 2 partagent 0, 1 et 2 mais aucun X
    */
    SXINT 	*model, lim;
    SXINT		elem, model_no, node, next_node, pos, xy, xsm, x, z, models_dag_index, node_set_size;
    SXINT		*aux, *mm, *sorted_milestones;
    SXBA	to_be_processed, already_processed, back_bone, node_set;
    XxY_header	models_dag_var;

    static SXINT tokens_dag_foreach [] = {1, 0, 1, 0, 0, 0};

/* On est dans le cas d'un scanner deterministe, on alloue "sxndtkn.dag" qui contiendra
   l'ensemble des "corrections". */

    parse_stack.rcvr.repair_list = NULL;

    XxYxZ_alloc (&sxndtkn.dag, "tokens_dag", 31, 1, tokens_dag_foreach, (sxoflw0_t) NULL, stdout_or_NULL); 
    XxY_alloc (&models_dag_var, "models_dag", 31, 1, 1, 1, (sxoflw0_t) NULL, NULL);
    XxY_alloc (&parse_stack.rcvr.correction_strings,
	       "correction_strings", 50, 1, 0, 0, (sxoflw0_t) NULL, NULL);

    parse_stack.rcvr.back_bone = sxbm_calloc (sxplocals.SXP_tables.P_nbcart + 1,
					      sxplocals.SXP_tables.P_sizofpts + 1);

    node_set_size = sxplocals.SXP_tables.P_nmax + 2 /* back_bone */ +
	2 * sxplocals.SXP_tables.P_nbcart /* estimation */;
    node_set = sxba_calloc (node_set_size + 1);
    aux = (SXINT*) sxcalloc ((SXUINT)sxplocals.SXP_tables.P_nmax + 1, sizeof (SXINT));

    /* On initialise le dag avec un modele bidon appele' back_bone	*/
    /* trans :   0   1 ...          P_nmax		*/
    /* noeud : 1   2  ... P_nmax+1        P_nmax+2	*/
    /* Sur le back_bone on a : (p, p+1). */
    /* On note ds parse_stack.rcvr.back_bone tous les token d'un modele qui
       appartiennent a back_bone. */
    /* Pour chaque modele le token associe a la position 2 si (model [1] == 0) ou
       sinon a la position 1 n'appartient pas a back_bone. Ce token est donc
       caracteristique du modele et sera toujours un nouveau token (cree s'il
       correspond a X ou S ou clone du token associe. */
    /* On reserve le noeud zero pour la transition (eventuelle) sur TOK [-1] */
    parse_stack.rcvr.milstn_nb = 1;

    for (elem = 0; elem <= sxplocals.SXP_tables.P_nmax; elem++) {
	XxY_set (&models_dag_var, parse_stack.rcvr.milstn_nb, parse_stack.rcvr.milstn_nb + 1, &models_dag_index);
	parse_stack.rcvr.milstn_nb++;    
    }

    /* Noeud initial associe aux modeles corrigeant le terminal precedant celui en erreur. */
    parse_stack.rcvr.MMrch = 0;

    parse_stack.rcvr.mm = (SXINT**) sxalloc (sxplocals.SXP_tables.P_nbcart + 1,
					   sizeof (SXINT*));

    for (model_no = 1; model_no <= sxplocals.SXP_tables.P_nbcart; model_no++) {
	model = sxplocals.SXP_tables.P_lregle [model_no];
	mm = parse_stack.rcvr.mm [model_no] = (SXINT*) sxalloc (sxplocals.SXP_tables.P_sizofpts + 1,
						  sizeof (SXINT));

	back_bone = parse_stack.rcvr.back_bone [model_no];
	lim = sxplocals.SXP_tables.P_right_ctxt_head [model_no];

	if (lim > parse_stack.rcvr.MMrch)
	    parse_stack.rcvr.MMrch = lim;

	mm [1] = 1;

	if (model [1] == 0) {
	    SXBA_1_bit (back_bone, 1);
	    node = 2;
	    pos = 1;
	    mm [2] = 2;
	}
	else {
	    node = 1;
	    pos = 0;
	}

	sxba_empty (node_set);
	SXBA_1_bit (node_set, node);

	do {
	    elem = model [++pos];

	    if (elem == lim) {
		/* Le premier noeud traite est la limite... */
		XxY_set (&models_dag_var, node, lim + 2, &xy);
	    }
	    else if (model [pos+1] == lim) {
		pos++;
		next_node = lim + 1;

		if (SXBA_bit_is_set (node_set, next_node)) {
		    /* Detection d'un cycle */
		    if ((next_node = aux [lim]) == 0) {
			aux [lim] = next_node = ++parse_stack.rcvr.milstn_nb;
			XxY_set (&models_dag_var, node, next_node, &xy);
			XxY_set (&models_dag_var, next_node, lim + 2, &xy);
		    }
		    else {
			/* Utilisation de la transition auxiliaire */
			XxY_set (&models_dag_var, node, next_node, &xy);
		    }
		}
		else {
		    SXBA_1_bit (back_bone, pos);
		    XxY_set (&models_dag_var, node, next_node, &xy);
		}

		mm [pos] = next_node;
		elem = lim; /* break */
	    }
	    else {
		next_node = ++parse_stack.rcvr.milstn_nb;
		XxY_set (&models_dag_var, node, next_node, &xy);
		mm [pos + 1] = next_node;
		node = next_node;

		if (node > node_set_size)
		    node_set = sxba_resize (node_set, (node_set_size *= 2) + 1);

		SXBA_1_bit (node_set, node);
	    }
	} while (elem != lim);

	/* On complete back-bone et mm comme si tous les modeles se terminaient
	   sur P_nmax. */

	while (elem <= sxplocals.SXP_tables.P_nmax) {
	    pos++;
	    SXBA_1_bit (back_bone, pos);
	    mm [pos] = elem + 2;
	    elem++;
	}

	mm [++pos] = elem + 2;
	mm [0] = pos; /* longueur */
    }

    /* On fait un tri topologique des milestones. */
    already_processed = sxba_calloc (parse_stack.rcvr.milstn_nb + 1);
    to_be_processed = sxba_calloc (parse_stack.rcvr.milstn_nb + 1);
    sorted_milestones = (SXINT*) sxalloc (parse_stack.rcvr.milstn_nb + 1,
							 sizeof (SXINT));

    parse_stack.rcvr.milstn_elems = (struct sxmilstn_elem*)
	sxalloc (parse_stack.rcvr.milstn_nb + 1, sizeof (struct sxmilstn_elem));

    for (node = parse_stack.rcvr.milstn_nb; node >= 0; node--)
	parse_stack.rcvr.milstn_elems [node].my_index = node;

    SXBA_1_bit (already_processed, 1);
    sorted_milestones [1] = xsm = 1;

    XxY_Xforeach (models_dag_var, 1, xy) {
	z = XxY_Y (models_dag_var, xy);
	SXBA_1_bit (to_be_processed, z);
    }

    node = 0;

    while ((node = sxba_scan (to_be_processed, node)) > 0) {
	XxY_Yforeach (models_dag_var, node, xy) {
	    x = XxY_X (models_dag_var, xy);

	    if (!sxba_bit_is_set (already_processed, x))
		break;
	}

	if (xy == 0) {
	    /* OK, tous ses predecesseurs sont deja traites. */
	    SXBA_1_bit (already_processed, node);
	    sorted_milestones [node] = ++xsm;
	    SXBA_0_bit (to_be_processed, node);

	    /* ses successeurs deviennent des candidats */
	    XxY_Xforeach (models_dag_var, node, xy) {
		z = XxY_Y (models_dag_var, xy);
		SXBA_1_bit (to_be_processed, z);
	    }

	    node = 0;
	}
    }
    
    for (model_no = 1; model_no <= sxplocals.SXP_tables.P_nbcart; model_no++) {
	mm = parse_stack.rcvr.mm [model_no];

	for (pos = mm [0]; pos > 0; pos--)
	    mm [pos] = sorted_milestones [mm [pos]];
    }

    sxfree (sorted_milestones);
    sxfree (aux);
    sxfree (node_set);
    sxfree (to_be_processed);
    sxfree (already_processed);
    XxY_free (&models_dag_var);
}



SXBOOLEAN		sxndpallcorrections (SXINT what_to_do)
{
    SXINT i;

    switch (what_to_do) {
    case SXOPEN:
	parse_stack.rcvr.back_bone = NULL;
	parse_stack.rcvr.all_corrections = SXTRUE;
	return SXTRUE;

    case SXACTION:
	if (parse_stack.rcvr.back_bone == NULL)
	    models_dag ();

	for (i = parse_stack.rcvr.milstn_nb; i >= 0; i--)
	    parse_stack.rcvr.milstn_elems [i].my_milestone = 0;
    
	parse_stack.rcvr.Mrch = 0;
	parse_stack.rcvr.mrch = sxplocals.SXP_tables.P_nmax;

	XxYxZ_clear (&sxndtkn.dag);
	XxY_clear (&parse_stack.rcvr.correction_strings);
	break;

    case SXCLOSE:
	if (parse_stack.rcvr.back_bone != NULL) {
	    XxY_free (&parse_stack.rcvr.correction_strings);
	    XxYxZ_free (&sxndtkn.dag);
	    sxfree (parse_stack.rcvr.milstn_elems);
	    sxbm_free (parse_stack.rcvr.back_bone), parse_stack.rcvr.back_bone = NULL;

	    for (i = 1; i <= sxplocals.SXP_tables.P_nbcart; i++) {
		sxfree (parse_stack.rcvr.mm [i]);
	    }
	    
	    sxfree (parse_stack.rcvr.mm);

	    if (parse_stack.rcvr.repair_list != NULL) {
		sxfree (parse_stack.rcvr.repair_list), parse_stack.rcvr.repair_list = NULL;
		sxfree (parse_stack.rcvr.repair_string);
	    }
	}

	break;

    default:
	break;
    }

    return sxndprecovery (what_to_do);
}

SXBOOLEAN		sxndpsrecovery (SXINT what_to_do)
{
    SXINT		kind = sxplocals.mode.kind;
    SXBOOLEAN	ret_val;

    sxplocals.mode.kind = SXWITH_RECOVERY;
    ret_val = sxndprecovery (what_to_do);
    sxplocals.mode.kind = kind;

    return ret_val;
}

/* Pour la version ou toutes les corrections valides sont retournees. */


