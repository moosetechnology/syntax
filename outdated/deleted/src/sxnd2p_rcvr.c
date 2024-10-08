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

/*** THIS FILE IS NOT PART OF THE SYNTAX LIBRARY libsx.a ***/ 

/************************************************************************/
/* It is the nondeterministic version of the SYNTAX error recovery 	*/
/* module								*/
/************************************************************************/

static char	ME [] = "NDPRECOVERY";

#include "sxversion.h"
#include "sxunix.h"
# include "sxba.h"
# include "XxY.h"
# include "XxYxZ.h"
# include "XH.h"
# include "sxnd.h"

char WHAT_SXND2P_RECOVERY[] = "@(#)SYNTAX - $Id: sxnd2p_rcvr.c 3907 2024-04-20 08:51:26Z garavel $" WHAT_DEBUG;

extern SXINT			NDP_access ();
extern SXINT			seek_parser ();
extern void			set2_first_trans ();
extern bool			set2_next_trans ();
extern void			sxndsubparse_a_token ();
extern void			reducer ();
extern struct sxmilstn_elem	*milestone_new ();
extern void                   sxnd2parser_GC ();


/* E R R O R   R E C O V E R Y */

static void compute_a_trans (parser_seed, head)
    SXINT parser_seed, head;
{
    /* On est en correction, les parser : (state, ref, tok_no) ne doivent pas
       etre confondus entre eux lorsque le meme token est utilise dans des regles
       differentes. Ils ne doivent non plus pas etre confondus avec des parsers
       de l'analyse normale (il existe des parsers avec tok_no = NO_de_TOK (0)
       il en existe meme avec tok_no = NO_de_TOK (1) : les reduces eventuels
       executes avant la detection d'erreur). 
       De plus il faut pouvoir traiter le cas SXGENERATOR.
       Au kieme appel de compute_a_trans (), parse_stack.delta_generator vaut
       -k*tmax et current_token est initialise a delta_generator - t
       si SXRECOGNIZER ou a delta_generator.
       current_milestone vaut delta_generator. */
    /* calcule dans for_scanner.next_hd l'ensemble des parsers atteints depuis
       parser_seed par transition sur le[s] token specifie par head. */
    parse_stack.current_milestone = parse_stack.current_token =
	parse_stack.delta_generator -= sxplocals.SXP_tables.P_tmax;

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
    sxndsubparse_a_token (parser_seed, XxYxZ_Y (parse_stack.parsers, parser_seed), 0, true);

    if (parse_stack.for_reducer.top > 0)
	reducer ();
}



static SXINT	get_tail (binf, bsup)
    SXINT		binf, bsup;
{
    SXINT	lahead;

    if (binf == 0)
	binf = 1;

    for (; binf <= bsup; binf++) {
	sxplocals.rcvr.TOK_i = parse_stack.rcvr.TOK [binf];
	lahead = SXGET_TOKEN (sxplocals.rcvr.TOK_i).lahead;

	if (lahead == sxplocals.SXP_tables.P_tmax
	    || sxplocals.rcvr.truncate_context
	    && sxgetbit (sxplocals.SXP_tables.PER_tset, lahead))
	    return binf;
    }

    return bsup;
}



static bool	morgan (c1, c2)
    char	*c1, *c2;

/* rend vrai si c1 = c2 a une faute d'orthographe pres */

{
    char	*tmax, *t1, *t2;
    SXINT		l1, l2;
    char	d1, d2, e1, e2;

    switch ((l1 = strlen (c1)) - (l2 = strlen (c2))) {
    default:
	/* longueurs trop differentes */
	return false;

    case 0:
	/* longueurs egales: on autorise un remplacement ou une interversion
	   de caracteres */
	tmax = c1 + l1, t1 = c1, t2 = c2;

	while (t1 < tmax && *t1 == *t2)
	    t1++, t2++;

	if (t1 == tmax)
	    /* egalite */
	    return true;

	d1 = *t1++, d2 = *t2++ /* caracteres courants, differents */ ;
	e1 = *t1++, e2 = *t2++ /* caracteres suivants */ ;

	if (e1 != e2 /* caracteres suivants differents */
		     && (d1 != e2 || e1 != d2))
	    /* caracteres pas inverses */
	    return false;


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
	return true;

    t1++ /* on saute un caractere dans la chaine la plus longue */ ;


/* les restes des chaines doivent etre egaux */

    while (t1 < tmax && *t1 == *t2)
	t1++, t2++;

    return t1 >= tmax;
}



static char	*tok_text (tok_no)
    SXINT	tok_no;
{
    struct sxtoken	*ptok = &(SXGET_TOKEN (tok_no));

    return sxgenericp (sxplocals.sxtables, ptok->lahead)
	? sxstrget (ptok->string_table_entry)
	    : sxttext (sxplocals.sxtables, ptok->lahead);
}


static bool validate_submodel (model_no, model, parser, pos, bot, a_la_rigueur)
    SXINT		model_no, parser, pos, bot;
    SXINT 	*model;
    bool	a_la_rigueur;
{
    /* Retourne vrai ssi un modele est trouve'. */
    SXINT		lim, l, x, seed, elem, t;
    bool	local_a_la_rigueur;

    if (pos < bot) {
	/* lter contient une correction */ 
	parse_stack.rcvr.best.model_no = model_no;
	parse_stack.rcvr.best.a_la_rigueur = a_la_rigueur;
	lim = sxplocals.SXP_tables.P_right_ctxt_head [model_no];
	l = 0;
    
	while (model [++l] < lim)
	    parse_stack.rcvr.best.correction [l] = sxplocals.rcvr.lter [l];

	for (l = model [model [0]]; l >= 0; l--) {
	    parse_stack.rcvr.best.TOK [l] = parse_stack.rcvr.TOK [l];
	    parse_stack.rcvr.best.MS [l] = parse_stack.rcvr.MS [l];
	}

	return true;
    }

    XxY_Yforeach (parse_stack.rcvr.seeds, parser, x) {
	local_a_la_rigueur = a_la_rigueur;
	seed = XxY_X (parse_stack.rcvr.seeds, x);
		
	switch (elem = model [pos]) {
	case -2: /* S */
	    sxplocals.rcvr.TOK_i = parse_stack.rcvr.TOK [pos - 1];
	    /* On suppose que le S a l'index pos remplace le token pos-1 */
	    /* On continue en sequence */
	    
	case -1: /* X */
	    t = (-XxYxZ_Z (parse_stack.parsers, seed)) % sxplocals.SXP_tables.P_tmax;
		
	    if (elem == -1 || sxkeywordp (sxplocals.sxtables, t)
		&& morgan (tok_text (sxplocals.rcvr.TOK_i), sxttext (sxplocals.sxtables, t))) {
		if (sxgetbit (sxplocals.SXP_tables.P_no_insert, t)) {
		    /* On ne peut inserer t */
		    local_a_la_rigueur = true;
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

	    if (!local_a_la_rigueur /* pur (pour l'instant) */
		|| model_no < parse_stack.rcvr.best.model_no)
		if (validate_submodel (model_no, model, seed, pos - 1, bot, local_a_la_rigueur))
		    if (!parse_stack.rcvr.best.a_la_rigueur || a_la_rigueur)
			/* On ne pourra pas trouver mieux localement. */
			return true;
	}
    }

    return parse_stack.rcvr.best.model_no == model_no;
}


static bool validate_model (model_no, pos)
    SXINT		model_no, pos;
{
    SXINT 	*model = sxplocals.SXP_tables.P_lregle [model_no];
    SXINT		bot, parser, index;

    bot = (model [1] == 0) ? 2 : 1;
    index = parse_stack.rcvr.prefixes [model_no] [pos];

    for (parser = parse_stack.rcvr.prefixes_to_milestones [index].next_for_scanner;
	 parser > 0;
	 parser = parse_stack.parser_to_attr [parser].next_for_scanner) {
	if (validate_submodel (model_no, model, parser, pos - 1,
			       bot, parse_stack.rcvr.dont_delete)) {
	    if (!parse_stack.rcvr.best.a_la_rigueur)
		/* On ne pourra pas trouver mieux localement. */
		return true;
	}
    }

    return parse_stack.rcvr.best.model_no == model_no;
}



static bool try_a_model (pos, parser_seed, model_no)
    SXINT		pos, parser_seed, model_no;
{
    SXINT 		*model = sxplocals.SXP_tables.P_lregle [model_no];
    SXINT 		lmod = model [0];
    SXINT			head, tail, index, parser, seed, prev_parser;

    tail = get_tail (sxplocals.SXP_tables.P_right_ctxt_head [model_no], model [lmod]);

    do {
	head = model [++pos];
	index = parse_stack.rcvr.prefixes [model_no] [pos];
	
	if (SXBA_bit_is_reset_set (parse_stack.rcvr.prefixes_set, index)) {
	    parse_stack.rcvr.prefixes_to_milestones [index].next_for_scanner = 0;
	    /* On ajoute index a parse_stack.rcvr.milstn_hd pour GC () */
	    parse_stack.rcvr.prefixes_to_milestones [index].next_milstn =
		parse_stack.rcvr.milstn_head;
	    parse_stack.rcvr.milstn_head =
		&(parse_stack.rcvr.prefixes_to_milestones [index]);
	    
	    do {
		/* On calcule la transition. */
		compute_a_trans (parser_seed, head);
		
		if ((parser = *parse_stack.for_scanner.next_hd) > 0) {
		    do {
			/* On memorise les couples */
			XxY_set (&parse_stack.rcvr.seeds, parser_seed, parser, &seed); 
			prev_parser = parser;
		    } while ((parser =
			      parse_stack.parser_to_attr [parser].next_for_scanner) > 0);
		    
		    /* On concatene le resultat. */
		    parse_stack.parser_to_attr [prev_parser].next_for_scanner = 
			parse_stack.rcvr.prefixes_to_milestones [index].next_for_scanner;
		    parse_stack.rcvr.prefixes_to_milestones [index].next_for_scanner =
			*parse_stack.for_scanner.next_hd;
		    *parse_stack.for_scanner.next_hd = 0; /* Pour GC () */
		}
	    } while ((parser_seed =
		      parse_stack.parser_to_attr [parser_seed].next_for_scanner) > 0);
	}

	if ((parser_seed = parse_stack.rcvr.prefixes_to_milestones [index].next_for_scanner) == 0)
	    return false;
    } while (head != tail);

    /* Le modele est correct, il faut maintenant le valider vis-a-vis des 
       dont-delete, dont-insert et des fautes d'orthographe. */
    return validate_model (model_no, pos);
}


static bool dont_delete (model_no)
    SXINT model_no;
{
    /* Retourne vrai si le modele model_no implique au moins un element de dont_delete. */
    SXINT 	*model = sxplocals.SXP_tables.P_lregle [model_no];
    SXINT		lmod = (SXINT) model [0];
    SXINT		n = (SXINT) model [lmod];
    SXINT		i, x;
    
    /* On recherche les indices i inferieurs a` n qui n'appartiennent pas au modele. */
    
    for (i = 0; i < n; i++) {
	for (x = 1; x < lmod; x++) {
	    if (model [x] == i)
		break;
	}
	
	if (x == lmod) {
	    /* i est supprime'. */
	    sxplocals.rcvr.TOK_i = parse_stack.rcvr.TOK [i];
	    
	    if (sxgetbit (sxplocals.SXP_tables.P_no_delete,
			  SXGET_TOKEN (sxplocals.rcvr.TOK_i).lahead))
		return true;
	}
    }
    
    return false;
}


static SXINT is_a_spelling_model (model_no)
    SXINT model_no;
{
    SXINT 	*model = sxplocals.SXP_tables.P_lregle [model_no];
    SXINT 	lim = sxplocals.SXP_tables.P_right_ctxt_head [model_no];
    SXINT 	val;
    SXINT		x;

    for (x = 1; (val = model [x]) < lim; x++) {
	if (val == -2 /* S */) {
	    /* On suppose qu'un S en position x ds un modele est mis pour
	       une faute d'orthographe du (x-1)ieme token. */
	    return x;
	}
    }

    return 0;
}


static void fill_vt_set (vt_set, parser)
    SXBA	vt_set;
    SXINT		parser;
{
    SXINT t;

    sxba_empty (vt_set);

    do {
	t = (-XxYxZ_Z (parse_stack.parsers, parser)) % sxplocals.SXP_tables.P_tmax;
	SXBA_1_bit (vt_set, t);
    } while ((parser = parse_stack.parser_to_attr [parser].next_for_scanner) > 0);
}


static compute_vt_set (vt_set, parser)
    SXBA	vt_set;
    SXINT		parser;
{
    sxplocals.mode.mode = SXGENERATOR;
    parse_stack.current_milestone = parse_stack.current_token =
	parse_stack.delta_generator -= sxplocals.SXP_tables.P_tmax;
    parse_stack.current_lahead = 0;
    *parse_stack.for_scanner.next_hd == 0;
    
    do {
	sxndsubparse_a_token (parser, XxYxZ_Y (parse_stack.parsers, parser), 0, true);
    } while ((parser = parse_stack.parser_to_attr [parser].next_for_scanner) > 0);
    
    if (parse_stack.for_reducer.top > 0)
	reducer ();
    
    fill_vt_set (vt_set, *parse_stack.for_scanner.next_hd);
    *parse_stack.for_scanner.next_hd = 0;
}


static void local_correction ()
{
    SXINT		z, i, model_no, pos;
    bool	is_a_0_model;
    
    
    /* L'erreur sera marquee (listing) a l'endroit de la detection */
    parse_stack.rcvr.source_index = SXGET_TOKEN (sxplocals.ptok_no).source_index;
    sxplocals.rcvr.TOK_0 = sxplocals.ptok_no - 1;

/* On lit tous les tokens dont on aura besoin pour la correction locale. */
    sxget_token (sxplocals.rcvr.TOK_0 + sxplocals.SXP_tables.P_nmax);
/* On met leur numeros ds TOK (compatibilite avec le traitement d'erreur quand
   le scanner est non-deterministe. */

    for (i = 0; i <= sxplocals.SXP_tables.P_nmax; i++)
	parse_stack.rcvr.TOK [i] = sxplocals.rcvr.TOK_0 + i;
    
    parse_stack.rcvr.cur_top = FS_DECR (parse_stack.for_scanner.top);
    parse_stack.rcvr.prev_top = FS_DECR (parse_stack.rcvr.cur_top);

    XxY_clear (&parse_stack.rcvr.seeds);
    sxba_empty (parse_stack.rcvr.prefixes_set);
    parse_stack.delta_generator = 0;
    parse_stack.rcvr.milstn_head = NULL;

    for (model_no = 1; model_no <= sxplocals.SXP_tables.P_nbcart; model_no++) {
	if ((pos = is_a_spelling_model (model_no)) == 0
	    || sxplocals.sxtables->SXS_tables.check_keyword != NULL
	    && strlen (tok_text (parse_stack.rcvr.TOK [pos - 1])) > 2) {
	    /* Si le modele est un modele de correction de faute d'orthographe
	       on ne continue que si le langage definit des mot-cles et si la chaine de
	       caracteres correspondante a une longueur superieure a 2 */
	    parse_stack.rcvr.dont_delete = dont_delete (model_no);
	    if (model_no < parse_stack.rcvr.best.model_no
		|| !parse_stack.rcvr.dont_delete) {
		/* On poursuit si aucun modele n'a ete trouve ou si
		   le modele courant ne supprime pas d'elements de "no_delete". */
		is_a_0_model = sxplocals.SXP_tables.P_lregle [model_no] [1] == 0;

		if (try_a_model (is_a_0_model ? 1 : 0,
			     parse_stack.for_scanner.previouses
			     [is_a_0_model ? parse_stack.rcvr.cur_top : parse_stack.rcvr.prev_top],
			     model_no))
		    if (!parse_stack.rcvr.best.a_la_rigueur)
			/* Un pur */
			break;
	    }
	}
    }
    
    if (parse_stack.rcvr.best.model_no > sxplocals.SXP_tables.P_nbcart) {
	/* pas de correction, on s'occupe de vt_set */
	if (parse_stack.rcvr.index_0X > 0)
	    /* On extrait vt_set */
	    fill_vt_set (parse_stack.rcvr.best.vt_set, parse_stack.rcvr.prefixes_to_milestones
			 [parse_stack.rcvr.index_0X].next_for_scanner);
	else
	    compute_vt_set (parse_stack.rcvr.best.vt_set, *parse_stack.for_scanner.current_hd);
		
	z = sxba_scan (sxplocals.rcvr.vt_set, 0);
	
	if (sxba_scan (sxplocals.rcvr.vt_set, z) > 0 /* multiple */
	    || z == sxplocals.SXP_tables.P_tmax)
	    z = 0;
	else
	    parse_stack.rcvr.best.single_la = z;
	
	parse_stack.rcvr.best.model_no = sxplocals.SXP_tables.P_nbcart + (z == 0 ? 2 : 1);
    }

    /* On remet tout en etat */
    parse_stack.rcvr.milstn_head = NULL; /* Pour GC () */
    parse_stack.delta_generator = 0;

    /* On recupere explicitement la place de la correction locale afin de ne pas
       laisser trainer de champs tok_no negatifs qui pourraient interferer avec
       une utilisation ulterieure (calcul de vt_set ou traitement d'une autre
       erreur). */
    sxnd2parser_GC ();
}


static void set_next_shift_state (son, fathers_state, ref, tnt)
    SXINT		son, fathers_state;
    SXINT		ref, tnt;
{
    /* On ne cree le pere de son que s'il est interessant. */
    SXINT			father, link, state, x;
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

	if (XxYxZ_Y (parse_stack.parsers, son) == 0 /* ref == 0 */) {
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
	    father = seek_parser (son, state, 0 /* ref */, 0 /* symbol */);
	    /* Si "father" est nouveau, mreds_push (father, -0); a ete appele' */
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


static void create_path (son, fathers_state)
    SXINT son, fathers_state;
{
    /* Attention, les parsers crees ne doivent pas etre GCer... */
    /* On range les intermediaires par mreds_push et les feuilles ds rcvr.shifts. */
    struct SXP_bases	*abase;
    SXINT			size, link, father, next_action_kind, state;
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
    set2_first_trans (abase, &check, &ref,
		     sxplocals.SXP_tables.P_tmax, &next_action_kind);
    
    do {
	set_next_shift_state (son, fathers_state, ref, check);
    } while (set2_next_trans (abase, &check, &ref,
			     sxplocals.SXP_tables.P_tmax, &next_action_kind)
	     && check > 0);
    
    if (state <= sxplocals.SXP_tables.M0ref - sxplocals.SXP_tables.Mprdct) {
	/* On regarde les transitions non terminales... */
	abase = sxplocals.SXP_tables.nt_bases + state;
	set2_first_trans (abase, &check, &ref,
			 sxplocals.SXP_tables.P_ntmax, &next_action_kind);
	
	do {
	    set_next_shift_state (son, fathers_state, ref, check);
	} while (set2_next_trans (abase, &check, &ref,
				 sxplocals.SXP_tables.P_ntmax, &next_action_kind)
		 && check > 0);
    }
}



static void process_sons (father, grand_fathers_state)
    SXINT		father, grand_fathers_state;
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



static SXINT clone (father, ref)
    SXINT father, ref;
{
    /* cree un clone de father en changeant "ref". */
    SXINT state, son, x;

    state = XxYxZ_X (parse_stack.parsers, father);
    son = parse_stack.parser_to_attr [father].son;

    if (son < 0) {
	son = -son;

	XxY_Xforeach (parse_stack.sons, father, x) {
	    seek_parser (XxY_Y (parse_stack.sons, x), state, ref, 0);
	}
    }

    return seek_parser (son, state, ref, 0);
}




static char	*ttext (tables, tcode)
    struct sxtables	*tables;
    SXINT		tcode;
{
    return (tcode == sxplocals.SXP_tables.P_tmax)
	? sxplocals.SXP_tables.P_global_mess [sxplocals.SXP_tables.P_followers_number + 2]
	    : sxttext (tables, tcode);
}



static bool is_a_right_ctxt (head, tail)
    SXINT head, tail;
{
    /* On verifie que tokens [TOK_0 + head .. TOK_0 + tail]
       est un contexte droit valide. */
    /* Les parsers source sont dans parse_stack.for_scanner.next_hd */
    SXINT current, parser;

    do {
	parse_stack.rcvr.milstn_head =
	    &(parse_stack.rcvr.milestones [head]);
	current = parse_stack.rcvr.milestones [head].next_for_scanner = *parse_stack.for_scanner.next_hd;
	parse_stack.rcvr.milestones [head].next_milstn = NULL;

	sxplocals.rcvr.TOK_i = parse_stack.rcvr.TOK [head];
	parse_stack.current_milestone = parse_stack.current_token = sxplocals.rcvr.TOK_i;
	parse_stack.current_lahead = SXGET_TOKEN (sxplocals.rcvr.TOK_i).lahead;

	*parse_stack.for_scanner.next_hd = 0;
    
	for (parser = current;
	     parser > 0;
	     parser = parse_stack.parser_to_attr [parser].next_for_scanner) {
	    sxndsubparse_a_token (parser, XxYxZ_Y (parse_stack.parsers, parser), 0, true);
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



static void nd2_try_a_correction ()
{
    SXINT				z, model_no, pos, index, prev_for_scanner, parser_seed;
    bool			is_local_correction;
    struct sxmilstn_elem	*prev_ms_ptr;

    XxY_clear (&parse_stack.rcvr.seeds);
    sxba_empty (parse_stack.rcvr.prefixes_set);

    for (model_no = 1; model_no <= sxplocals.SXP_tables.P_nbcart; model_no++) {
	if (model_no >= parse_stack.rcvr.best.model_no
	    && !parse_stack.rcvr.best.a_la_rigueur)
	    break;

	if ((pos = is_a_spelling_model (model_no)) == 0
	    || sxplocals.sxtables->SXS_tables.check_keyword != NULL) {
	    /* Si le modele est un modele de correction de faute d'orthographe
	       on ne continue que si le langage definit des mot-cles */
	    /* TOK [0] n'est pas calcule', la verification de la longueur 
	       sera faite plus tard. */
	    
	    prev_ms_ptr =
		&sxmilstn_access (sxndtkn.milestones, parse_stack.rcvr.nd2.prev_ms_nb);
	    prev_for_scanner = prev_ms_ptr->next_for_scanner;
	    is_local_correction = false;
	    
	    /* On poursuit si le modele courant est meilleur que le modele actuel
	       ou si le meilleur modele est un "a_la_rigueur" et si le modele courant
	       ne supprime pas d'elements de "no_delete". */
	    /* Si le modele est un modele de correction de faute d'orthographe
	       on ne continue que si la chaine de
	       caracteres correspondante a une longueur superieure a 2 */
	    
	    if (sxplocals.SXP_tables.P_lregle [model_no] [1] == 0) {
		/* doit etre calcule pour chaque modele, peut utiliser TOK [0] */
		parse_stack.rcvr.dont_delete = dont_delete (model_no);
		
		if ((model_no < parse_stack.rcvr.best.model_no
		     || !parse_stack.rcvr.dont_delete)
		    && (pos == 0 || strlen (tok_text (parse_stack.rcvr.TOK [pos - 1])) > 2))
		    is_local_correction = try_a_model (1, prev_for_scanner, model_no);
	    }
	    else {
		XxYxZ_Zforeach (sxndtkn.dag, prev_ms_ptr->my_index, z) {
		    parse_stack.rcvr.TOK [0] = XxYxZ_Y (sxndtkn.dag, z);
		    parse_stack.rcvr.MS [0] = XxYxZ_X (sxndtkn.dag, z);
		    /* doit etre calcule pour chaque modele, peut utiliser TOK [0] */
		    parse_stack.rcvr.dont_delete = dont_delete (model_no);
		    
		    if ((model_no < parse_stack.rcvr.best.model_no
			 || !parse_stack.rcvr.dont_delete)
			&& (pos == 0
			    || strlen (tok_text (parse_stack.rcvr.TOK [pos - 1])) > 2)) {
			parser_seed =
			    sxmilstn_access (sxndtkn.milestones,
					     parse_stack.rcvr.MS [0]).next_for_scanner;
			/* On peut etre en debut de texte source, sur le milestone initial
			   parser_seed vaut 0. */
			
			if (parser_seed > 0 && try_a_model (0, parser_seed, model_no)) {
			    is_local_correction = true;
			    break;
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



static void nd2_unfold (mstn, n, lim)
    SXINT mstn, n, lim;
{
    SXINT 			x;
    struct sxmilstn_elem 	*mstn_ptr;

    if (mstn == sxndtkn.eof_milestone || n > lim) {
	/* TOK [1..P_nmax] est rempli */
	parse_stack.rcvr.MS [n] = mstn;
	nd2_try_a_correction ();
    }
    else {
	mstn_ptr = &sxmilstn_access (sxndtkn.milestones, mstn);
	
	while ((mstn_ptr->kind & MS_COMPLETE) == 0)
	    sxndscan_it ();
	
	XxYxZ_Xforeach (sxndtkn.dag, mstn, x) {
	    parse_stack.rcvr.TOK [n] = XxYxZ_Y (sxndtkn.dag, x);
	    parse_stack.rcvr.MS [n] = mstn;
	    nd2_unfold (XxYxZ_Z (sxndtkn.dag, x), n + 1, lim);
	}
    }
}



static bool 	global_recovery (parser_seed)
    SXINT parser_seed;
{
    SXINT	head, tail, x, top, p;
    
    parse_stack.current_level = sxplocals.rcvr.TOK_0;
    parse_stack.current_milestone =
	parse_stack.current_token = sxplocals.rcvr.TOK_0;
    parse_stack.current_lahead = SXGET_TOKEN (sxplocals.rcvr.TOK_0).lahead;
    
    head = 0;
    tail = get_tail (0, sxplocals.SXP_tables.P_validation_length - 1);
    
    /* We look for all visible parsers from current_for_scanner. */
    *parse_stack.for_scanner.next_hd = 0;
    
    for (x = parser_seed;
	 x > 0;
	 x = parse_stack.parser_to_attr [x].next_for_scanner) {
	process_sons (x, 0 /* father's state : son is top state */);
    }
    
    for (x = 1; x <= parse_stack.for_reducer.top; x++) {
	create_path (parse_stack.for_reducer.triples [x].parser,
		     -1 /* new parser */);
    }
    
    sxba_empty (parse_stack.parsers_set [0]);
    parse_stack.for_reducer.top = 0;
    
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
	    parse_stack.current_milestone =
		parse_stack.current_token = sxplocals.rcvr.TOK_i;
	    parse_stack.current_lahead =
		SXGET_TOKEN (sxplocals.rcvr.TOK_i).lahead;
	    
	    for (x = 1; x <= top; x++) {
		*parse_stack.for_scanner.next_hd = 0;
		
		sxndsubparse_a_token (XxYxZ_X (parse_stack.rcvr.shifts, x),
				      XxYxZ_Y (parse_stack.rcvr.shifts, x),
				      0,
				      true);
		
		if (parse_stack.for_reducer.top > 0)
		    reducer ();
		
		if (tail > 1 && *parse_stack.for_scanner.next_hd > 0)
		    is_a_right_ctxt (1, tail);
		
		if (*parse_stack.for_scanner.next_hd == 0)
		    XxYxZ_erase (parse_stack.rcvr.shifts, x);
	    }
	}
	
	parse_stack.current_level = sxplocals.rcvr.TOK_0;
	parse_stack.current_milestone =
	    parse_stack.current_token = sxplocals.rcvr.TOK_0;
	parse_stack.current_lahead = SXGET_TOKEN (sxplocals.rcvr.TOK_0).lahead;
	*parse_stack.for_scanner.next_hd = 0;
	
	for (x = 1; x <= top; x++) {
	    if (!XxYxZ_is_erased (parse_stack.rcvr.shifts, x)) {
		p = XxYxZ_X (parse_stack.rcvr.shifts, x);
		
		if (parse_stack.parser_to_attr [p].milestone !=
		    parse_stack.current_milestone) {
		    /* C'est un parser du squelette, il faut donc le cloner
		       avec les bonnes valeurs. */
		    p = clone (p, XxYxZ_Y (parse_stack.rcvr.shifts, x));
		    /* parse_stack.for_scanner.next_hd est maj par clone () */
		}
		else {
		    parse_stack.parser_to_attr [p].next_for_scanner =
			*parse_stack.for_scanner.next_hd;
		    *parse_stack.for_scanner.next_hd = p;
		}
	    }
	}
	
	XxYxZ_clear (parse_stack.rcvr.shifts);
	parse_stack.rcvr.milstn_head = NULL;
    }
    
    return *parse_stack.for_scanner.next_hd > 0;
}



static SXINT nd2_search_validation_context (parser_seed, mstn_nb, n, lim)
    SXINT parser_seed, mstn_nb, n, lim;
{
    struct sxmilstn_elem 	*mstn_ptr;
    SXINT				x, ret_val;

    if (mstn_nb == sxndtkn.eof_milestone)
	return -1;

    parse_stack.rcvr.MS [n] = mstn_nb;

    if (n == lim) {
	return (SXINT) global_recovery (parser_seed);
    }

    mstn_ptr = &sxmilstn_access (sxndtkn.milestones, mstn_nb);
    
    while ((mstn_ptr->kind & MS_COMPLETE) == 0)
	sxndscan_it ();
    
    XxYxZ_Xforeach (sxndtkn.dag, mstn_nb, x) {
	parse_stack.rcvr.TOK [n] = XxYxZ_Y (sxndtkn.dag, x);
	ret_val = nd2_search_validation_context (parser_seed,
						 XxYxZ_Z (sxndtkn.dag, x), n + 1, lim);

	if (ret_val != 0)
	    return ret_val; /* eof hit ou OK */
    }

    return 0;
}


static bool nd2_search_key_terminal (parser_seed, mstn_ptr)
    SXINT				parser_seed;
    struct sxmilstn_elem 	*mstn_ptr;
{
    /* Recherche d'un milestone qui est atteint par un terminal-cle */
    SXINT	z, mstn_nb, lahead, i, ret_val, tok_no;

/* A FAIRE : "suppression" des noeuds du dag que l'on parcourt... */

    for (;;) {
	while (mstn_ptr->next_milstn == NULL)
	    sxndscan_it ();
	
	mstn_ptr->kind |= MS_EMPTY;
	sxndtkn.milstn_current = mstn_ptr = mstn_ptr->next_milstn;
	mstn_nb = mstn_ptr->my_index;

	if (mstn_nb == sxndtkn.eof_milestone)
	    return false;

	XxYxZ_Zforeach (sxndtkn.dag, mstn_nb, z) {
	    tok_no = XxYxZ_Y (sxndtkn.dag, z);
	    lahead = SXGET_TOKEN (tok_no).lahead;
	    
	    if ((i = sxgetbit (sxplocals.SXP_tables.PER_tset, lahead)) > 0) {
		/* terminal courant == terminal cle */
		parse_stack.rcvr.trans = sxplocals.SXP_tables.PER_trans_sets != NULL
		    ? sxplocals.SXP_tables.PER_trans_sets [i]
			: NULL;
		sxplocals.rcvr.TOK_0 = parse_stack.rcvr.TOK [0] = tok_no ;
		parse_stack.rcvr.MS [0] = XxYxZ_X (sxndtkn.dag, z);
		
		ret_val = nd2_search_validation_context (parser_seed,
							 mstn_nb,
							 1,
							 sxplocals.SXP_tables.P_validation_length);

		if (ret_val != 0)
		    return ret_val > 0;
	    }
	}
    }
}



static void modify_dag ()
{
    /* On a trouve une correction (dans rcvr.best), on modifie le dag
       des tokens en consequence. */
    SXINT			model_no, bot, first_ms, prev_ms, last_ms, x, l, i, cur_tok, cur_ms, tde;
    struct sxmilstn_elem
	                *p, *q;
    struct sxtoken	tok;
    SXINT 		*model;
    SXINT 		lmod, rch;

    model_no = parse_stack.rcvr.best.model_no;
    model = sxplocals.SXP_tables.P_lregle [model_no];
    lmod = model [0];
    rch = sxplocals.SXP_tables.P_right_ctxt_head [model_no];
    bot = model [1] == 0 ? 1 : 0;

    /* Suppression des transitions */
    /* On suppose que les transitions de dag seront supprimees par le mecanisme
       normal, les milestones sautes sont mis a MS_EMPTY. */
#if 0
    for (i = bot; i <= rch, i++) {
	if (!XxYxZ_is_set (sxndtkn.dag,
			   parse_stack.rcvr.best.MS [i],
			   parse_stack.rcvr.best.TOK [i],
			   parse_stack.rcvr.best.MS [i + 1],
			   &x))
	    sxtrap (ME, "modify_dag");

	XxYxZ_erase (sxndtkn.dag, x);
    }
#endif
    
    /* Creation des nouveaux milestones. */
    first_ms = prev_ms = parse_stack.rcvr.best.MS [bot];
    last_ms = parse_stack.rcvr.best.MS [rch + 1];

    /* first_ms doit preceder last_ms en suivant les liens internes, on supprime
       tous les intermediaires. */

    /* On repositionne milstn_current sur le precedent. */
    sxndtkn.milstn_current = p = &sxmilstn_access (sxndtkn.milestones, first_ms);

    while ((q = p->next_milstn) != NULL && q->my_index != last_ms) {
	q->kind |= MS_EMPTY;
	
	if ((p->next_milstn = q->next_milstn) != NULL)
	    p->next_milstn->prev_milstn = p;
    }

    if (q == NULL)
	sxtrap (ME, "modify_dag");

    x = bot;
    parse_stack.rcvr.is_warning = true;

    for (l = bot + 1; l <= lmod; l++) {
	i = model [l];

	if (i < 0) {
	    /* On cree un token */
	    tok.lahead = parse_stack.rcvr.best.correction [l];
	    tok.string_table_entry = SXERROR_STE /* error value */ ;
	    sxplocals.rcvr.TOK_i = parse_stack.rcvr.best.TOK [x];

	    if (x + 1 < rch)
		x++; /* approximatif */

	    tok.source_index = SXGET_TOKEN (sxplocals.rcvr.TOK_i).source_index;
	    tok.comment = sxplocals.rcvr.com [l];

	    if (sxgenericp (sxplocals.sxtables, tok.lahead))
		parse_stack.rcvr.is_warning = false;

	    sxput_token (tok);
	    cur_tok = sxplocals.Mtok_no;
	}
	else {
	    x = i + 1; /* approximatif */
	    cur_tok = parse_stack.rcvr.best.TOK [i];
	    SXGET_TOKEN (cur_tok).comment = sxplocals.rcvr.com [l];
	}

	if (i < rch) {
	    /* On cree un milestone */
	    p = milestone_new (&sxmilstn_access (sxndtkn.milestones, prev_ms), MS_BEGIN);
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
	
	if (i == rch)
	    break;

	prev_ms = cur_ms;
    }
}


static bool	less_equal (z1, z2)
    SXINT z1, z2;
{
    return XxYxZ_X (sxndtkn.dag, z1) <= XxYxZ_X (sxndtkn.dag, z2);
}




static void nd2_local_correction ()
{
    /* Dans un premier temps, on s'occupe uniquement de milstn_current.
       Il serait possible de tenter des corrections (meme eventuellement multiples)
       sur les milestones MS_EMPTY precedents. */
    SXINT 			cur_ms_nb, z, preds_nb;
    struct sxmilstn_elem	*prev_ms_ptr;
	    
    parse_stack.delta_generator = 0;
    parse_stack.rcvr.milstn_head = NULL;
    
    /* On traite les predecesseurs de milstn_current par valeur de milestone
       decroissante (progression maximale ds le texte source). */

    preds_nb = 0;
    cur_ms_nb = sxndtkn.milstn_current->my_index;

    XxYxZ_Zforeach (sxndtkn.dag, cur_ms_nb, z) {
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
    
    if (preds_nb > 1)
	sort_by_tree (parse_stack.rcvr.to_be_sorted, 1, preds_nb, less_equal);

    /* On lit P_nmax tokens */	    

    while (preds_nb > 0) {
	z = parse_stack.rcvr.preds [parse_stack.rcvr.to_be_sorted [preds_nb]];
	parse_stack.rcvr.nd2.prev_ms_nb = XxYxZ_X (sxndtkn.dag, z);
	prev_ms_ptr = &sxmilstn_access (sxndtkn.milestones, parse_stack.rcvr.nd2.prev_ms_nb);
		
	if (prev_ms_ptr->kind & MS_WAIT) {
	    parse_stack.rcvr.TOK [1] = XxYxZ_Y (sxndtkn.dag, z);
	    parse_stack.rcvr.MS [1] = parse_stack.rcvr.nd2.prev_ms_nb;

	    if ((sxplocals.mode.kind & SXWITH_CORRECTION)
		&& sxplocals.SXP_tables.P_nbcart > 0)
		nd2_unfold (cur_ms_nb, 2, sxplocals.SXP_tables.P_nmax);
	    
	    if (parse_stack.rcvr.best.model_no > sxplocals.SXP_tables.P_nbcart + 1) {
		/* pas de correction et pas d'insertion forcee */ 
		if (parse_stack.rcvr.index_0X > 0)
		    /* On extrait vt_set */
		    fill_vt_set (sxplocals.rcvr.vt_set,
				 parse_stack.rcvr.prefixes_to_milestones
				 [parse_stack.rcvr.index_0X].next_for_scanner);
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
		    /* marquage de l'erreur */
		    parse_stack.rcvr.best.TOK [1] = parse_stack.rcvr.TOK [1];
		    /* on stocke le milestone precedent. */
		    parse_stack.rcvr.best.MS [1] = parse_stack.rcvr.MS [1];
		    if (z == 0)
			/* On prend le premier */
			sxba_copy (parse_stack.rcvr.best.vt_set, sxplocals.rcvr.vt_set);
		    else
			parse_stack.rcvr.best.single_la = z;
		}
	    }
	}
	
	preds_nb--;
    }
    
    /* On remet tout en etat */
    parse_stack.rcvr.milstn_head = NULL; /* Pour GC () */
    parse_stack.delta_generator = 0;
    *parse_stack.for_scanner.next_hd = 0;

    /* On recupere explicitement la place de la correction locale afin de ne pas
       laisser trainer de champs tok_no negatifs qui pourraient interferer avec
       une utilisation ulterieure (traitement d'une autre erreur). */
    sxnd2parser_GC ();

    /* Dans tous les cas (correction, insertion forcee, rattrapage global)
       l'erreur sera marquee (listing) sur le token sur lequel on a choisi de
       faire le rattrapage. */
    parse_stack.rcvr.source_index = SXGET_TOKEN (parse_stack.rcvr.best.TOK [1]).source_index;
} 



static bool	ndp_recovery ()
{
    char				*msg_params [5];
    SXINT 				*regle;
    struct SXP_local_mess		*local_mess;
    struct sxtoken			*tok;

    bool			with_semact = sxplocals.mode.with_semact;
    bool			ret_val = true;
    SXINT				mode = sxplocals.mode.mode;
    SXINT				im, ll, i, j, k, p, x, y, ate;
    SXINT				vt_set_card;

    parse_stack.rcvr.is_up = true;
    sxplocals.mode.with_semact = false;

    /* + 1 => vt_set_card == 1
       + 2 => vt_set_card > 1
       + 3 => non calcule' */
    parse_stack.rcvr.best.model_no = sxplocals.SXP_tables.P_nbcart + 3; /* vide */

    /* Meme si on n'a pas demande' de correction, positionne vt_set */
    if (sxplocals.sxtables->SXS_tables.S_is_non_deterministic)
	nd2_local_correction ();
    else
	local_correction ();

    if (parse_stack.rcvr.best.model_no <= sxplocals.SXP_tables.P_nbcart) {
	/* OK ca a marche */
	SXINT	n, l, x1, y1;

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
	    sxplocals.rcvr.TOK_i = parse_stack.rcvr.TOK [i];
	    tok = &(SXGET_TOKEN (sxplocals.rcvr.TOK_i));
	    msg_params [j] = i < 0 /* X or S */
		? ttext (sxplocals.sxtables, parse_stack.rcvr.best.correction [-i])
		    : ((ate = tok->string_table_entry) == SXEMPTY_STE || ate == SXERROR_STE
		       ? ttext (sxplocals.sxtables, tok->lahead)
		       : sxstrget (ate));
	}
	
	/* On doit retrouver NULL ds le token, meme si les commentaires
	   ne sont pas gardes. */
	for (y1 = 1; y1 <= ll; y1++)
	    sxplocals.rcvr.com [y1] = NULL;
	
	if (!sxsvar.SXS_tables.S_are_comments_erased) {
	    /* deplacement des commentaires */
	    
	    x = im;
	    
	    for (y = ll; regle [y] == x && x >= 0; y--) {
		sxplocals.rcvr.TOK_i = parse_stack.rcvr.TOK [x--];
		sxplocals.rcvr.com [y] = SXGET_TOKEN (sxplocals.rcvr.TOK_i).comment;
	    }
	    
	    if (ll < ++y)
		y = ll;
	    
	    y1 = 1;
	    
	    for (x1 = 0; x1 <= x && y1 < y; x1++) {
		sxplocals.rcvr.TOK_i = parse_stack.rcvr.TOK [x1];
		sxplocals.rcvr.com [y1++] = SXGET_TOKEN (sxplocals.rcvr.TOK_i).comment;
	    }
	    
	    for (x1 = x1; x1 <= x; x1++) {
		sxplocals.rcvr.TOK_i = parse_stack.rcvr.TOK [x1];
		tok = &(SXGET_TOKEN (sxplocals.rcvr.TOK_i));
		
		if (sxplocals.rcvr.com [y] == NULL)
		    sxplocals.rcvr.com [y] = tok->comment;
		else if (tok->comment != NULL) {
		    /* concatenation des commentaires */
		    sxplocals.rcvr.com [y] = sx_alloc_and_copy (strlen (sxplocals.rcvr.com [y]) + strlen (tok->comment) + 1,  
								sizeof(char),
								sxplocals.rcvr.com [y],
								strlen(sxplocals.rcvr.com [y]));
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
	    modify_dag ();
	}
	else {
	    x = 0;
	    j = sxplocals.SXP_tables.P_right_ctxt_head [parse_stack.rcvr.best.model_no];
	    l = (regle [1] == 0) ? 2 : 1;
	    n = 0;
	    parse_stack.rcvr.is_warning = true;
	    
	    for (tok = sxplocals.rcvr.toks; (i = regle [l]) < j; l++, tok++) {
		if (i < 0) {
		    tok->lahead = parse_stack.rcvr.best.correction [l];
		    tok->string_table_entry = SXERROR_STE /* error value */ ;
		    sxplocals.rcvr.TOK_i = parse_stack.rcvr.TOK [x];
		    tok->source_index = SXGET_TOKEN (sxplocals.rcvr.TOK_i).source_index;
		    
		    if (sxgenericp (sxplocals.sxtables, tok->lahead))
			parse_stack.rcvr.is_warning = false;
		}
		else {
		    sxplocals.rcvr.TOK_i = parse_stack.rcvr.TOK [i];
		    *tok = SXGET_TOKEN (sxplocals.rcvr.TOK_i);
		    x = i + 1;
		}
		
		n++;
		tok->comment = sxplocals.rcvr.com [l];
		/* et voila */
	    }
	    
	    parse_stack.for_scanner.top = FS_DECR (parse_stack.for_scanner.top);
	    
	    if (regle [1] == 0) {
		j--;
		sxplocals.rcvr.TOK_0++;
	    }
	    else
		parse_stack.for_scanner.top = FS_DECR (parse_stack.for_scanner.top); 
	    
	    sxtknmdf (sxplocals.rcvr.toks,
		      n,
		      sxplocals.rcvr.TOK_0,
		      j);
	    
	    sxplocals.ptok_no = sxplocals.rcvr.TOK_0 - 1;
    
	    parse_stack.for_scanner.next_hd =
		parse_stack.for_scanner.previouses + parse_stack.for_scanner.top;
	}
	
	/* "Warning: "machin" is replaced by chose." ou similaire */
	if (!sxplocals.mode.is_silent)
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
    else if (sxplocals.mode.kind & SXWITH_RECOVERY) {
	struct sxmilstn_elem	*milstn_error;

	/* On repositionne milstn_current sur le precedent. */
	if (sxplocals.sxtables->SXS_tables.S_is_non_deterministic) {
	    milstn_error = sxndtkn.milstn_current;
	    sxndtkn.milstn_current = &sxmilstn_access (sxndtkn.milestones,
						       parse_stack.rcvr.best.MS [1]);
	}

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
		sxplocals.rcvr.TOK_i = parse_stack.rcvr.TOK [i]; 
		tok = &(SXGET_TOKEN (sxplocals.rcvr.TOK_i));
		msg_params [j] = i < 0 /* assume %0 */
		    ? ttext (sxplocals.sxtables, parse_stack.rcvr.best.single_la)
			: ((ate = tok->string_table_entry) == SXEMPTY_STE || ate == SXERROR_STE
			   ? ttext (sxplocals.sxtables, tok->lahead)
			   : sxstrget (ate));
	    }
	    
	    /* insertion du nouveau token, avec source_index et comment */
	    toks.lahead = parse_stack.rcvr.best.single_la;
	    toks.string_table_entry = SXERROR_STE /* error value */ ;
	    toks.comment = NULL;
	    toks.source_index = parse_stack.rcvr.source_index;
	    
	    if (sxplocals.sxtables->SXS_tables.S_is_non_deterministic) {
		SXINT cur_ms, tde;

		/* Creation du nouveau token en sxplocals.Mtok_no */
		sxput_token (toks);

		/* On insere un nouveau milestone */
		cur_ms = milestone_new (sxndtkn.milstn_current, MS_BEGIN)->my_index;

		/* On remplit dag */
		XxYxZ_set (&sxndtkn.dag,
			   sxndtkn.milstn_current->my_index,
			   sxplocals.Mtok_no,
			   cur_ms,
			   &tde);

		XxYxZ_set (&sxndtkn.dag,
			   cur_ms,
			   parse_stack.rcvr.best.TOK [1],
			   milstn_error->my_index,
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
		
		while ((j = sxba_scan (sxplocals.rcvr.vt_set, j)) > 0) {
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
		ret_val = nd2_search_key_terminal (sxndtkn.milstn_current->next_for_scanner,
						   milstn_error);

		if (ret_val) {
		    sxndtkn.milstn_current->kind |= MS_WAIT; /* calcule' */
		    sxndtkn.milstn_current->next_for_scanner =
			*parse_stack.for_scanner.next_hd;
		    *parse_stack.for_scanner.next_hd = 0;
		}
		else {
		    /* on a trouve la fin du fichier */
		    /* "Error: Parsing stops on End of File." */
		    tok = &(SXGET_TOKEN (sxndtkn.eof_tok_no));
		}
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
			ret_val = false;
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
			
			if (global_recovery (*parse_stack.for_scanner.current_hd)) {
			    ret_val = true;
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
	    
	    /* Il FAUT lancer le GC avant de retourner, les etats crees par is_a_right_ctxt ne
	       peuvent etre reutilises : puisqu'ils existent deja, ils sont sense etre actif
	       et donc ne seront pas empiles (et donc pas traites) ni dans for_scanner ni dans
	       for_reducer. */
	    sxnd2parser_GC ();

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
	ret_val = false;
    
    sxplocals.mode.mode = mode;
    sxplocals.mode.with_semact = with_semact;
    parse_stack.rcvr.is_up = false;
    parse_stack.halt_hit = false; /* Au cas ou */

    return ret_val;
}



bool		sxndprecovery (SXINT what_to_do)
{
    SXINT		x;
    static SXINT	rcvr_shifts_foreach [] = {0, 0, 0, 0, 0, 0};
    
    switch (what_to_do) {
    case SXOPEN:
	break;

    case SXINIT:
	/* valeurs par defaut qui peut etre changee ds les
	   scan_act ou pars_act. */
	sxplocals.rcvr.truncate_context = true;
	parse_stack.rcvr.with_parsprdct = false;

	/* Allocation will be done at first call. */
	break;

    case SXACTION:
	if (sxplocals.rcvr.vt_set == NULL) {
	    SXINT 	size, model_no, x, index, prev_index, elem;
	    SXINT 	*model;
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
	    parse_stack.rcvr.best.MS = (SXINT*) sxalloc (x + 2, sizeof (SXINT));
	    parse_stack.rcvr.best.TOK = (SXINT*) sxalloc (x + 1, sizeof (SXINT));
	    
	    parse_stack.rcvr.index_0X = 0;

	    if ((sxplocals.mode.kind & SXWITH_CORRECTION) && sxplocals.SXP_tables.P_nbcart > 0) {
		SXINT	index, model_no, best_l, mn, l, best_model_no;
		SXINT	*m, *cur_model;

		XxY_alloc (&parse_stack.rcvr.seeds, "rcvr_seeds", 200, 1, 0, 1, NULL, NULL); 
		parse_stack.rcvr.best.correction =
		    (SXINT*) sxalloc (sxplocals.SXP_tables.P_sizofpts + 1, sizeof (SXINT));
		parse_stack.rcvr.prefixes = (SXINT**) sxalloc (sxplocals.SXP_tables.P_nbcart + 1,
							     sizeof (SXINT*));
			    
		index = 0;

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
			parse_stack.rcvr.prefixes [model_no] [l] = ++index;
		    }

		    if (parse_stack.rcvr.index_0X == 0
			&& cur_model [1] == 0
			&& cur_model [2] < 0)
			parse_stack.rcvr.index_0X = parse_stack.rcvr.prefixes [model_no] [2];
		}

		parse_stack.rcvr.prefixes_to_milestones =
		    (struct sxmilstn_elem *) sxalloc (index + 1, sizeof (struct sxmilstn_elem));
		parse_stack.rcvr.prefixes_set = sxba_calloc (index + 1);
	    }

	    parse_stack.rcvr.milestones =
		    (struct sxmilstn_elem *) sxalloc (sxplocals.SXP_tables.P_validation_length + 1,
						      sizeof (struct sxmilstn_elem));

	    XxYxZ_alloc (&parse_stack.rcvr.shifts, "rcvr_shifts", 31, 1,
			 rcvr_shifts_foreach, NULL, NULL);

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
	    sxfree (parse_stack.rcvr.best.MS);
	    sxfree (parse_stack.rcvr.best.TOK);
	    sxfree (parse_stack.rcvr.preds);
	    sxfree (parse_stack.rcvr.to_be_sorted);

	    if ((sxplocals.mode.kind & SXWITH_CORRECTION)
		&& sxplocals.SXP_tables.P_nbcart > 0) {   
		SXINT model_no;

		XxY_free (&parse_stack.rcvr.seeds);
		sxfree (parse_stack.rcvr.best.correction);
		sxfree (parse_stack.rcvr.prefixes_to_milestones);
		sxfree (parse_stack.rcvr.prefixes_set);

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

    return true;
}

bool		sxndpsrecovery (SXINT what_to_do)
{
    SXINT		kind = sxplocals.mode.kind;
    bool	ret_val;

    sxplocals.mode.kind = SXWITH_RECOVERY;
    ret_val = sxndprecovery (what_to_do);
    sxplocals.mode.kind = kind;

    return ret_val;
}

