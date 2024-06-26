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

/*   S Y M B O L _ T A B L E   */

#define SXNODE struct lecl_node

#include "sxversion.h"
#include "sxunix.h"
#include "sxba.h"
#include "varstr.h"
#include "lecl_ag.h"
#include "lecl_pcn.h"
#include "lecl_node.h"

char WHAT_LECLST[] = "@(#)SYNTAX - $Id: lecl_st.c 3633 2023-12-20 18:41:19Z garavel $" WHAT_DEBUG;

extern void	lecl_pi (void);
extern void	lecl_pd (void);
extern SXINT	lecl_bsc (SXBA *, 
			  SXBA, 
			  SXINT, 
			  SXINT, 
			  SXINT *, 
			  SXINT * /* out */ );
extern SXINT	lecl_prs (struct lecl_node *visited);
extern SXINT	lecl_pcn (SXINT *, 
			  SXINT [], 
			  SXBA *);
static SXINT	define_class_by_char (SXINT car);
static SXINT	i, j, r, s, x, alpha_table_entry, origine, master, t_master, syn, token_no_of_eof, last_user_xlis;
static SXBA	/* ers_size */ item_set, already_processed;
static SXBA	/* termax */ fil_or_set, fil_and_set;
static SXINT	*sub_s /* 1:ers_size */ ;



static void	st_retrieve_re_name (SXINT name, 
				     SXINT component, 
				     struct sxsource_coord source_index, 
				     SXBA ctxt_set)
{
    SXINT	st_retrieve_re_name_i, st_retrieve_re_name_j, k;

    sxba_empty (ctxt_set);

    if (name != 0) {
	st_retrieve_re_name_i = ate_to_token_no [name];

	if (component == 0) {
	    /* reference simple */
	    if (st_retrieve_re_name_i > 0) {
		/* reference non ambigue */
		st_retrieve_re_name_j = ers_disp [st_retrieve_re_name_i].component_no;

		if (st_retrieve_re_name_j > 0)
		    /* union */
		    while (st_retrieve_re_name_j--) {
			++st_retrieve_re_name_i;
			SXBA_1_bit (ctxt_set, st_retrieve_re_name_i);
		    }
		else
		    SXBA_1_bit (ctxt_set, st_retrieve_re_name_i);

		return;
	    }
	}
	else {
	    /* Union */
	    if (st_retrieve_re_name_i > 0)
		for (st_retrieve_re_name_j = 1; st_retrieve_re_name_j <= ers_disp [st_retrieve_re_name_i].component_no; st_retrieve_re_name_j++) {
		    k = ers_disp [st_retrieve_re_name_i + st_retrieve_re_name_j].reduc;

		    if ((k >= 0 ? k : -k) == component) {
			st_retrieve_re_name_i += st_retrieve_re_name_j;
			SXBA_1_bit (ctxt_set, st_retrieve_re_name_i);
			return;
		    }
		}
	    else if (st_retrieve_re_name_i < 0) {
		st_retrieve_re_name_j = ate_to_token_no [component];

		if (st_retrieve_re_name_j > 0) {
		    SXBA_1_bit (ctxt_set, st_retrieve_re_name_j);
		    return;
		}
	    }
	}

	if (st_retrieve_re_name_i < 0)
	    sxerror (source_index, err_titles [1][0] /* warning */, "%sAmbiguous reference.", err_titles [1]+1 /* warning */);
	else {
	    for (st_retrieve_re_name_i = 1; st_retrieve_re_name_i <= xnd; st_retrieve_re_name_i++) {
		if (not_declared [st_retrieve_re_name_i] == name)
		    return; /* error already output */
	    }

	    not_declared [++xnd] = name;
	    st_retrieve_re_name_i = ate_to_t [name];

	    if (st_retrieve_re_name_i > 0)
		if (sxba_bit_is_set (is_a_keyword, st_retrieve_re_name_i)) {
		    sxerror (source_index, 
			     err_titles [1][0] /* warning */,
			     "%sA keyword cannot be used in a context clause.",
			     err_titles [1]+1 /* warning */ );
		    return;
		}

	    for (st_retrieve_re_name_i = current_abbrev_no; st_retrieve_re_name_i <= cmax; st_retrieve_re_name_i++) {
		if (abbrev_or_class_to_ate [st_retrieve_re_name_i] == name) {
		    sxerror (source_index,
			     err_titles [1][0] /* warning */,
			     (st_retrieve_re_name_i < 0) ?
			     "%sAn abbreviation name cannot be used in a context clause." :
			     "%sA class name cannot be used in a context clause.",
			     err_titles [1]+1 /* warning */ );
		    return;
		}
	    }

	    sxerror (source_index,
		     err_titles [1][0] /* warning */,
		     "%sThis unknown context name is erased.",
		     err_titles [1]+1 /* warning */ );
	}
    }
}



static void	st_set_lispro (SXINT re_no, SXINT item_code, SXINT item_no)
{
    struct ers_item	*erse_ptr;

    erse_ptr = ers + xlis;
    erse_ptr->lispro = item_code;
    erse_ptr->prdct_no = 0;
    erse_ptr->prolis = re_no;
    erse_ptr->liserindx.file_name = NULL;
    erse_ptr->liserindx.line = 0;
    erse_ptr->liserindx.column = 0;
    erse_ptr->is_erased = true;
    item_no++;
    SXBA_1_bit (suivant [xlis++], item_no);
}



static void	st_initialize_lispro (SXINT re_no, SXINT ate, SXINT t_code)
{
    struct ers_item	*erse_ptr;
    struct ers_disp_item	*erse_disp_ptr;

    if (re_no >= 0) {
	erse_disp_ptr = ers_disp + re_no;
	erse_disp_ptr->reduc = ate;
	erse_disp_ptr->lex_name_source_index.file_name = NULL;
	erse_disp_ptr->lex_name_source_index.line = 0;
	erse_disp_ptr->lex_name_source_index.column = 0;
	erse_disp_ptr->subtree_ptr = NULL;
	erse_disp_ptr->is_a_user_defined_context = false;
	erse_disp_ptr->restricted_context = false;
	erse_disp_ptr->is_unbounded_context = false;
	erse_disp_ptr->priority_kind = 0 /* "000" b */ ;
	erse_disp_ptr->master_token = re_no;
	erse_disp_ptr->post_action = 0;
	SXBA_1_bit (RE_to_T [re_no], t_code);
	SXBA_1_bit (t_is_defined_by_a_token, t_code);
	ate_to_token_no [ate] = re_no;
    }

    erse_ptr = ers + xlis;
    erse_ptr->lispro = -1;
    erse_ptr->prdct_no = 0;
    erse_ptr->prolis = re_no;
    erse_ptr->liserindx.file_name = NULL;
    erse_ptr->liserindx.line = 0;
    erse_ptr->liserindx.column = 0;
    erse_ptr->is_erased = false;
    SXBA_1_bit (suivant [xlis++], 1);
}



static void	st_finalize_lispro (void)
{
    ers [xlis].lispro = -1;
    ers_disp [++current_re_no].prolon = xlis;
    ers_disp [current_re_no].pexcl = xexclude;
    ers_disp [current_re_no].pkw = xkw;
}

static void	NEXT (char *string, SXINT ate, SXINT t_code)
{
    SXINT	NEXT_i, NEXT_j, k, xs1;
    SXINT		q, NEXT_r, ner, codofcar, xs2, NEXT_x, t, l, prdct_no, lim, NEXT_origine;
    SXBA	/* max_re_lgth */ NEXT_follow;
    char	*NEXT_s;
    bool	is_true_found, is_in_list, mandatory_kw = false;

    /* cette procedure remplit titem avec les suivants de
   substate(1 .. xsubstate-1).index .
   - les actions sont sautees
   - Les classes effacees sont sautees si elles appartiennent a une ER tq reduc<0
   - On s'arrete en fin d' ER
*/

/* On realise une determination naive des mots-cles: si une ER peut reconnaitre
   un terminal (on ne tient pas compte des actions, les predicats rendent
   toujours vrai et l'on tient compte de l'operateur "-" dans les dummy) de nom
   different du nom de l'unite lexicale correspondante alors c'est un mot-cle.
   Afin d'etre plus precis, il faudrait prendre en compte les conflits, les
   modes de resolution (1la, unbounded) et les priorites (utilisateur ou
   systeme): c'est complique (en particulier lors d'un conflit unbounded sur
   un prefixe d'un mot-cle) et peut etre inutile (pas important). De toutes 
   les facons cette determination ne peut se faire completement statiquement
   a cause des actions et des predicats. Meme en lecl, il est impossible 
   d'interpreter les predicats et actions systeme car la communication entre
   unites lexicales differentes ne peut etre assuree (regions de textes, 
   premiere colonne, ...) */
/* Modif du 05/08/91 : pour chaque ER l'utilisateur peut dire si l'ER reconnait
   (ou pas) des mot_cles et lesquels. */
    xs2 = 1;

    for (NEXT_j = 1; NEXT_j <= current_token_no; NEXT_j++) {
	NEXT_i = ers_disp [NEXT_j].prolon;

	if (NEXT_i != ers_disp [NEXT_j + 1].prolon) {
	    if (!ers_disp [NEXT_j].is_keywords_spec)
		/* participe sans restriction a la recherche. */
		sub_s [xs2++] = NEXT_i;
	    else {
		if (ers_disp [NEXT_j].kw_kind & KW_NOT_) {
		    /* Ne reconnait aucun mot_cle */
		    is_in_list = false;
		}
		else if (ers_disp [NEXT_j].kw_kind & KW_LIST_) {
		    is_in_list = false;

		    for (k = ers_disp [NEXT_j + 1].pkw - 1 ; k >= ers_disp [NEXT_j].pkw; k--) {
			if (keyword [k] == t_code) {
			    is_in_list = true;
			    break;
			}
		    }
		}
		else
		    /* Pas de liste represente l'ensemble des terminaux non specifies. */
		    is_in_list = true;

		if (ers_disp [NEXT_j].kw_kind & KW_EXCLUDE_)
		    is_in_list = !is_in_list;
		
		if (!is_in_list) {
		    /* Reponse negative => ne participe pas a la recherche. */
		}
		else {
		    /* Reponse positive => ne participe pas a la recherche. */
		    /* C'est un mot-cle */
		    sxba_1_bit (RE_to_T [NEXT_j], labs (t_code));
		    /* Le mot_cle t_code est reconnu par NEXT_j :
		       - on met -2 dans le descripteur de NEXT_j => recherche en table hash
		       a l'execution */
		    ers [NEXT_i].lispro = -2;
		    mandatory_kw = true;
		}
	    }
	}
    }

    sxba_empty (item_set);

    for (NEXT_s = string + 1; *NEXT_s; NEXT_s++) {
	is_true_found = false;
	lim = xs2 - 1;

	for (xs1 = 1; xs1 <= lim && !is_true_found; xs1++) {
	    if (ers [sub_s [xs1]].prdct_no == prdct_true_code)
		is_true_found = true;
	}

	sxba_empty (already_processed);

	for (xs1 = 1; xs1 < xs2; xs1++) {
	    NEXT_x = sub_s [xs1];
	    prdct_no = ers [NEXT_x].prdct_no;

	    if (xs1 > lim || ((!is_true_found && prdct_no != prdct_false_code) || prdct_no == prdct_true_code)) {
		ner = ers [NEXT_x].prolis;
		NEXT_origine = ers_disp [ner].prolon;
		NEXT_follow = suivant [NEXT_x];
		t = 0;

		while ((t = sxba_scan (NEXT_follow, t)) > 0) {
		    k = NEXT_origine + t;
		    l = ers [k].lispro;

		    if (l < xactmin || (l >= 0 && ers [k].is_erased && ers_disp [ers [k].prolis].reduc < 0)) {
			if (SXBA_bit_is_reset_set (already_processed, k)) {
			    sub_s [xs2++] = k;
			}
		    }
		    else
			SXBA_1_bit (item_set, k);
		}
	    }
	}

	if (*(NEXT_s + 1)) {
	    codofcar = (unsigned char)*NEXT_s;
	    xs2 = 1;
	    q = 0;

	    while ((q = sxba_scan (item_set, q)) > 0) { 
		SXBA_0_bit (item_set, q);
		NEXT_r = ers [q].lispro;

		if (NEXT_r > 0)
		    /* classe definie : NEXT_r=0 => erreur de syntaxe */
		    if (SXBA_bit_is_set (class_to_char_set [NEXT_r], codofcar + 1))
			sub_s [xs2++] = q;
	    }
	}
    }

    q = 0;

    while ((q = sxba_scan (item_set, q)) > 0) { 
	if (ers [q].lispro >= 0 ||
	    ers_disp [ers [q - 1].prolis].master_token == comments_re_no ||
	    ers_disp [ers [q - 1].prolis].master_token == include_re_no)
	    SXBA_0_bit (item_set, q);
	/* Les commentaires et les includes ne reconnaissent pas de mot_cle */
    }


/* si le terminal courant est reconnu,on est en fin d'E.R. */

    if (mandatory_kw || !sxba_is_empty (item_set)) {
	/* pourquoi pas cette definition des keywords */
	q = labs (t_code);
	SXBA_1_bit (is_a_keyword, q);
	keywords [++nbndef].string_table_entry = ate;
	keywords [nbndef].t_code = t_code;


/* Si t_code est negatif il s'agit d'un synonyme du terminal -t_code */

	q = 0;

	while ((q = sxba_scan (item_set, q)) > 0) { 
	    SXBA_0_bit (item_set, q);
	    ner = ers [q - 1].prolis;
	    /* Abus: on melange les synonymes */
	    NEXT_j = labs (t_code);
	    SXBA_1_bit (RE_to_T [ner], NEXT_j);
	    /* Le terminal NEXT_i est reconnu par ner :
   - on met -2 dans le descripteur de ner => recherche en table hash
   a l'execution */
	    ers [ers_disp [ner].prolon].lispro = -2;
	}
    }
    else {
	/* on engendre l'E.R. reconnaissant le terminal */
	st_initialize_lispro (current_re_no, ate, labs (t_code));

	for (NEXT_s = string + 1, NEXT_j = 1; *(NEXT_s + 1); NEXT_s++, NEXT_j++) {
	    k = define_class_by_char ((unsigned char)*NEXT_s);
	    SXBA_1_bit (is_class_used_in_re, k);
	    st_set_lispro (current_re_no, k, NEXT_j);
	}

	st_finalize_lispro ();
    }
}



static SXINT	define_class_by_char (SXINT car)
{
    SXBA	/* char_max */ char_set;
    SXINT	define_class_by_char_i;
    char	str [4];

    char_set = class_to_char_set [cmax + 1];
    car++;
    SXBA_1_bit (char_set, car);

    for (define_class_by_char_i = 1; define_class_by_char_i <= cmax; define_class_by_char_i++) {
	if (sxba_first_difference (class_to_char_set [define_class_by_char_i], char_set) == -1) {
	    SXBA_0_bit (char_set, car);
	    return (define_class_by_char_i);
	}
    }

    str [0] = '"';
    str [1] = car - 1;
    str [2] = '"';
    str [3] = SXNUL;
    abbrev_or_class_to_ate [++cmax] = sxstrsave (str);
    return (cmax);
}



static void	filerset (SXBA erset, SXINT re_no)
{
    /* cette procedure emplit erset, ensemble des E.R. pouvant suivre le terminal t */
    SXINT	filerset_i, filerset_j;
    SXBA	/* termax */ quel_T;

    sxba_empty (fil_or_set);
    quel_T = RE_to_T [re_no];
    filerset_i = 0;

    while ((filerset_i = sxba_scan (quel_T, filerset_i)) > 0) {
	sxba_or (fil_or_set, follow [filerset_i]);
    }

    for (filerset_j = 1; filerset_j <= max_re; filerset_j++) {
	if (ers_disp [filerset_j].prolon != ers_disp [filerset_j + 1].prolon) {
	    sxba_and (sxba_copy (fil_and_set, RE_to_T [filerset_j]), fil_or_set);

	    if (!sxba_is_empty (fil_and_set))
		SXBA_1_bit (erset, filerset_j); /* l'ER filerset_i reconnait des elements de fil_or_set */
	}
    }

    if (are_comments_defined) {
	if (ers_disp [comments_re_no].component_no == 0)
	    SXBA_1_bit (erset, comments_re_no);
	else
	    for (filerset_j = comments_re_no + (filerset_i = ers_disp [comments_re_no].component_no); filerset_i > 0 ; filerset_j--, filerset_i--) {
		SXBA_1_bit (erset, filerset_j);
	    }
    }
    if (include_re_no > 0) {
	if (ers_disp [include_re_no].component_no == 0)
	    sxba_1_bit (erset, include_re_no);
	else
	    for (filerset_j = include_re_no + (filerset_i = ers_disp [include_re_no].component_no); filerset_i > 0 ; filerset_j--, filerset_i--) {
		SXBA_1_bit (erset, filerset_j);
	    }
    }
}

void	lecl_st (void)
{
    /* I N I T I A L I Z A T I O N S */
    if (sxverbosep) {
	if (!sxttycol1p) {
	    fputc ('\n', sxtty);
	    sxttycol1p = true;
	}

	fputs ("Symbol Table\n", sxtty);
    }

    post_action_no = 0;
    comments_re_no = 0;
    include_re_no = 0;
    current_abbrev_no = 0;
    current_token_no = 0;
    max_node_no_in_re = 0;
    release_code = 0;
    max_re_lgth++;
    /* marqueur de fin d'ER */
    xexclude = 1;
    xkw = 1;
    xnd = 0;
    abbrev [-1].pront = 1;
    abbrev [-1].is_used = false;
    xactmax = xactmin;
    action_or_prdct_to_ate [prdct_true_code] = true_ate;
    action_or_prdct_to_ate [prdct_false_code] = false_ate;
    xprdct_to_ate = 2;
    lecl_pcn (&cmax, abbrev_or_class_to_ate, class_to_char_set);

/* A T T R I B U T E S   E V A L U A T I O N   */

    sxsmp (sxatcvar.atc_lv.abstract_tree_root, lecl_pi, lecl_pd);

/*   R E G U L A R   E X P R E S S I O N S   B U I L T   B Y   L E C L  */

    current_re_no = current_token_no + 1;
    last_user_xlis = xlis - 1;

    if (sxba_bit_is_set (t_is_defined_by_a_token, termax) == 0) {
	/*  definition par defaut de EOF   */
	token_no_of_eof = current_re_no;
	ate_to_token_no [t_to_ate [termax]] = token_no_of_eof;
	st_initialize_lispro (current_re_no, t_to_ate [termax], termax);
	SXBA_1_bit (is_class_used_in_re, 2);
	st_set_lispro (current_re_no, (SXINT)2, (SXINT)1);
	/* eof simple classe number */
	st_finalize_lispro ();
    }
    else
	for (token_no_of_eof = 1; token_no_of_eof <= current_token_no; token_no_of_eof++) {
	    if (sxba_bit_is_set (RE_to_T [token_no_of_eof], termax))
		goto complete_st;
	}

complete_st:
    nbndef = 0;
    item_set = sxba_calloc (ers_size + 1);
    already_processed = sxba_calloc (ers_size + 1);
    sub_s = (SXINT*) sxalloc ((SXINT) (ers_size + 1), sizeof (SXINT));

    for (i = termax - 1; i >= 1; i--) {
	if (!sxba_bit_is_set (t_is_defined_by_a_token, i) && !sxba_bit_is_set (zombies_tset, i)) {
	     /* Terminal de la grammaire qui est ni défini au niveau lexical par
 une ER, ni un zombie */
	    if (sxba_bit_is_set (is_a_generic_terminal, i)) {
		/* C'est un terminal générique, on a aucune idée de sa forme lexicale => erreur */
		sxtmsg (sxsrcmngr.source_coord.file_name, "%sA description of %s must be given.", 
			err_titles [2]+1 /* error */ , sxstrget (t_to_ate [i]));
	    }
	    else {
		/* on cherche si une E.R. reconnait le terminal courant ( i ieme)
   (Les caracteres ne doivent pas etre supprimes
   Les predicats sont senses retourner "vrai")
   - si reconnue par une E.R. existante => recherche en table hash
   - si non reconnue on genere une E.R. specifique reconnaissant
   ce terminal. Chaque classe simple de cette E.R. est supprimee
   ex. "**" = -"*" -"*" ;

*/
		alpha_table_entry = t_to_ate [i];
		NEXT (sxstrget (alpha_table_entry), alpha_table_entry, i);
	    }
	}
    }


/* i=termax-1 by -1 to 1 */


/* Traitement des synonymes */

    if (is_synonym_list)
	for (i = 1; i < synonym_list_no; i++) {
	    origine = synonym_list [i];
	    master = synonym [origine].string_table_entry;

	    if (master > 0) {
		/* On verifie qu'il s'agit d'un terminal non ambigu */
		t_master = ate_to_t [master];

		if (t_master < 0)
		    if (t_master == -1)
			sxerror (synonym [origine].source_index, 
				 err_titles [1][0] /* warning */ ,
				 "%sAmbiguous denotation of a lexical token.",
				 err_titles [1]+1 /* warning */ );
		    else
			sxerror (synonym [origine].source_index,
				 err_titles [1][0] /* warning */,
				 "%sNot a lexical token.",
				 err_titles [1]+1 /* warning */ );
		else
		    for (x = origine + 1; x < synonym_list [i + 1]; x++) {
			syn = synonym [x].string_table_entry;


/* On verifie qu'il ne s'agit pas d'un terminal du niveau syntaxique */

			if (syn > 0) {
			    if (ate_to_t [syn] != -2)
				sxerror (synonym [x].source_index,
					 err_titles [1][0] /* warning */ ,
					 "%sA synonym cannot be a syntactic level terminal.",
					 err_titles [1]+1 /* warning */ );
			    else {
				NEXT (sxstrget (syn), syn, -t_master);
				ate_to_t [syn] = t_master;
			    }
			}
		    }
	    }
	}


/* Les predicats systeme et utilisateur dynamiques utilises dans des RE sont
   mis dans ers (lispro = prdct_no) afin de permettre un traitement homogene 
   avec les noms des predicats designant des expressions de predicats */

    for (i = prdct_false_code + 1; i <= xprdct_to_ate; i++) {
	if (prdct_to_ers [i] == 0) {
	    prdct_to_ers [i] = xlis;
	    st_initialize_lispro (-i, (SXINT)0, (SXINT)0);
	    st_set_lispro (-i /* prolis en negatif */ , i, (SXINT)1);
	    ers [xlis].lispro = -1;
	}
    }


/* calcul des classes simples */

    lecl_bsc (class_to_char_set, is_class_used_in_re, char_max, cmax, &smax
	      /* out */
									   , simple_classe /* out */ );
    kod = sxbm_calloc (cmax + 1, smax + 1);
    class_set = sxba_calloc (smax + 1);

    {
	SXINT	sc;
	SXBA	/* char_max */ char_set;

	i = 0;

	while ((i = sxba_scan (is_class_used_in_re, i)) > 0) {
	    sxba_empty (class_set);
	    char_set = class_to_char_set [i];
	    j = 0;

	    while ((j = sxba_scan (char_set, j)) > 0) {
		sc = simple_classe [j - 1];
		SXBA_1_bit (class_set, sc);
	    }

	    sxba_copy (kod [i], class_set);
	}

	SXBA_1_bit (kod [eof], 2); /* Eof */
    }

    sc_to_char_set = sxbm_calloc (smax + 1, char_max + 1);

    for (i = 1; i <= char_max; i++) {
	SXBA_1_bit (sc_to_char_set [simple_classe [i - 1]], i);
    }


/* traitement des specifs de representation */

    lecl_prs (sxatcvar.atc_lv.abstract_tree_root);
    /* construction des classes simples etendues : 
   couple (classe_simple,predicat) */
    sc_prdct_to_esc = (SXINT**) sxalloc ((SXINT) (smax + 1), sizeof (SXINT*));

    {
	SXINT	lecl_st_i;

	sc_prdct_to_esc [0] = NULL;

	for (lecl_st_i = 1; lecl_st_i <= smax; lecl_st_i++)
	    sc_prdct_to_esc [lecl_st_i] = (SXINT*) sxcalloc ((SXINT) xprdct_to_ate + 1, sizeof (SXINT));
    }

    esc_to_sc_prdct = (struct esc_to_sc_prdct_item*) sxcalloc ((SXINT) (smax * (xprdct_to_ate + 1) + 1), sizeof (struct
	 esc_to_sc_prdct_item));

    for (i = 1; i <= smax; i++) {
	sc_prdct_to_esc [i] [0] = i;
	esc_to_sc_prdct [i].simple_class = i;
    }

    last_esc = smax;

    for (i = ers_disp [1].prolon; i <= last_user_xlis; i++) {
	if ((r = ers [i].lispro) > 0 && (s = ers [i].prdct_no) > 0) {
	    x = 0;

	    while ((x = sxba_scan (kod [r], x)) > 0) {
		if (sc_prdct_to_esc [x] [s] == 0) {
		    sc_prdct_to_esc [x] [s] = ++last_esc;
		    esc_to_sc_prdct [last_esc].simple_class = x;
		    esc_to_sc_prdct [last_esc].predicate = s;
		}
	    }
	}
    }


/* setting_up of max_re */

    max_re = current_re_no - 1;
    ctxt_size = 3 * max_re;
    x_ctxt = max_re << 1;
    /* construction de la table des contextes droits de chaque E.R. */
    /* Pour l'unite lexicale i, son contexte droit initial (donne directement par
       Intro) est trouve en ctxt (max_re + i); son contexte, apres application des
       clauses "Context" est trouve en ctxt (i) */
    is_re_generated = sxba_calloc (max_re + 1);
    ctxt = sxbm_calloc (ctxt_size + 1, max_re + 1);

    {
	SXINT lecl_st_j;
	SXBA	/* max_re */ erset, ctxt_set, valid_re_set, eof_re_set, and_set;

	erset = sxba_calloc (max_re + 1);
	ctxt_set = sxba_calloc (max_re + 1);
	valid_re_set = sxba_calloc (max_re + 1);
	eof_re_set = sxba_calloc (max_re + 1);
	and_set = sxba_calloc (max_re + 1);
	fil_or_set = sxba_calloc (termax + 1);
	fil_and_set = sxba_calloc (termax + 1);

	if (ers_disp [token_no_of_eof].component_no == 0)
	    SXBA_1_bit (eof_re_set, token_no_of_eof);
	else
	    for (lecl_st_j = token_no_of_eof + (i = ers_disp [token_no_of_eof].component_no); i > 0 ; lecl_st_j--, i--) {
		SXBA_1_bit (eof_re_set, lecl_st_j);
	    }

	for (i = 1; i <= max_re; i++) {
	    if (ers_disp [i].prolon != ers_disp [i + 1].prolon)
		SXBA_1_bit (valid_re_set, i);
	}

	i = 0;

	while ((i = sxba_scan (valid_re_set, i)) > 0) {
	    lecl_st_j = ers_disp [i].master_token;

	    if (ers_disp [i].reduc < 0) {
		SXBA_1_bit (is_re_generated, i);

		if (ers [ers_disp [i].prolon].lispro == -1 && lecl_st_j != include_re_no)
		    /* ER inutile */
		    sxerror (ers_disp [i].lex_name_source_index,
			     err_titles [1][0] /* warning */,
			     "%sThis token definition is ignored.",
			     err_titles [1]+1 /* warning */ );
	    }

	    if (lecl_st_j != i /* component */ )
		SXBA_1_bit (is_re_generated, i);

	    if (lecl_st_j == comments_re_no || lecl_st_j == include_re_no)
		sxba_copy (erset, valid_re_set);
	    else if (lecl_st_j == token_no_of_eof)
		sxba_copy (erset, eof_re_set);
	    else {
		sxba_empty (erset);
		filerset (erset, i);
	    }

	    sxba_copy (ctxt [max_re + i], erset);


/* valeur initiale */

	    if (!ers_disp [i].is_a_user_defined_context)
		sxba_copy (ctxt [i], erset);
	    else {
		if (ers_disp [i].restricted_context)
		    sxba_copy (ctxt [i], erset);

		for (lecl_st_j = ers_disp [i].pexcl; lecl_st_j <= ers_disp [i + 1].pexcl - 1; lecl_st_j++) {
		    st_retrieve_re_name (exclude [lecl_st_j].name, exclude [lecl_st_j].component, exclude [lecl_st_j].source_index, ctxt_set);
		    sxba_and (sxba_copy (and_set, erset), ctxt_set);

		    if (sxba_first_difference (and_set, ctxt_set) != -1) {
			if (i == ers_disp [i].master_token) {
			    sxerror (exclude [lecl_st_j].source_index,
				     err_titles [1][0] /* warning */,
				     "%sNot a valid follower.",
				     err_titles [1]+1 /* warning */ );
			}
			else {
			    sxerror (exclude [lecl_st_j].source_index,
					 err_titles [1][0] /* warning */ ,
					 "%sNot a valid follower of %s.%s.", 
					 err_titles [1]+1 /* warning */ ,
					 sxstrget (ers_disp [ers_disp [i].master_token].reduc),
					 sxstrget (ers_disp [i].reduc));
			}
		    }

		    if (ers_disp [i].restricted_context)
			sxba_minus (ctxt [i], ctxt_set); /* moins */
		    else
			sxba_or (ctxt [i], ctxt_set);
		}
	    }
	}

	if (include_re_no != 0) {
	    /* On force eof comme suivant valide d'un token qcq afin de pouvoir
	       terminer les includes sans pb */
	    i = 0;

	    while ((i = sxba_scan (valid_re_set, i)) > 0) {
		sxba_or (ctxt [i], eof_re_set);
	    }
	}

	sxfree (fil_and_set);
	sxfree (fil_or_set);
	sxfree (sub_s);
	sxfree (already_processed);
	sxfree (item_set);
	sxfree (and_set);
	sxfree (eof_re_set);
	sxfree (valid_re_set);
	sxfree (ctxt_set);
	sxfree (erset);
    }
}
