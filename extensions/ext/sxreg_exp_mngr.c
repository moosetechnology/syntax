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

/* Gestionnaire dynamique d'expressions regulieres (a la LECL) */

#define SXNODE struct reg_exp_node

#define sxtables	reg_exp_tables
#define SCANACT		lecl_scan_act
#define PARSACT		lecl_pars_act
#define SEMPASS		sxre_smp

#include "sxversion.h"
#include "sxcommon.h"
#include <stdlib.h>

#include "reg_exp_t.h"	/* lecl_t.c */
#include "reg_exp_pact.h" /* lecl_pact.c */
#include "reg_exp_sact.h" /* lecl_sact.c */
#include "sxdynam_scanner.h"
#include "varstr.h"
#include "sxalloc.h"

char WHAT_SXREG_EXP_MNGR[] = "@(#)SYNTAX - $Id: sxreg_exp_mngr.c 4111 2024-07-17 11:22:43Z garavel $" WHAT_DEBUG;

struct reg_exp_node {
    SXNODE_HEADER_S SXVOID_NAME;
    SXINT item_no,
	node_no /* init 0 */ ;
    bool is_empty;
   };

static struct {
    char	*name;
    char	*elems;
}	predefs [] = {/* {"NUL", "\0"}, cas particulier */
		      {"SOH", "\1"},
		      {"STX", "\2"},
		      {"ETX", "\3"},
		      {"EOT", "\4"},
		      {"ENQ", "\5"},
		      {"ACK", "\6"},
		      {"BEL", "\7"},
		      {"BS", "\b"},
		      {"HT", "\t"},
		      {"LF", "\n"},
		      {"NL", "\n"},
		      {"EOL", "\n"},
		      {"VT", "\13"},
		      {"FF", "\f"},
		      {"CR", "\r"},
		      {"SO", "\16"},
		      {"SI", "\17"},
		      {"DLE", "\20"},
		      {"DC1", "\21"},
		      {"DC2", "\22"},
		      {"DC3", "\23"},
		      {"DC4", "\24"},
		      {"NAK", "\25"},
		      {"SYN", "\26"},
		      {"ETB", "\27"},
		      {"CAN", "\30"},
		      {"EM", "\31"},
		      {"SUB", "\32"},
		      {"ESC", "\33"},
		      {"FS", "\34"},
		      {"GS", "\35"},
		      {"RS", "\36"},
		      {"US", "\37"},
		      {"SP", " "},
		      {"QUOTE", "\""},
		      {"DEL", "\177"},
		      {"LETTER", "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"},
		      {"UPPER", "ABCDEFGHIJKLMNOPQRSTUVWXYZ"},
		      {"LOWER", "abcdefghijklmnopqrstuvwxyz"},
		      {"A", "Aa"},
		      {"B", "Bb"},
		      {"C", "Cc"},
		      {"D", "Dd"},
		      {"E", "Ee"},
		      {"F", "Ff"},
		      {"G", "Gg"},
		      {"H", "Hh"},
		      {"I", "Ii"},
		      {"J", "Jj"},
		      {"K", "Kk"},
		      {"L", "Ll"},
		      {"M", "Mm"},
		      {"N", "Nn"},
		      {"O", "Oo"},
		      {"P", "Pp"},
		      {"Q", "Qq"},
		      {"R", "Rr"},
		      {"S", "Ss"},
		      {"T", "Tt"},
		      {"U", "Uu"},
		      {"V", "Vv"},
		      {"W", "Ww"},
		      {"X", "Xx"},
		      {"Y", "Yy"},
		      {"Z", "Zz"},
		      {"DIGIT", "0123456789"},};

static struct sx_variables store;
static SXINT	node_no, item_no;

static VARSTR	vstr;
static SXINT	master_reg_exp_no;

static void
reg_exp_pcn_oflw (SXINT old_size, SXINT new_size)
{
    if (SXWORD_is_static (SXDS->sc_names))
    {
	SXINT *old = SXDS->sc_names_to_char_sets;

	SXDS->sc_names_to_char_sets = (SXINT*) sxalloc (new_size,
						      sizeof (SXINT));
	memcpy (SXDS->sc_names_to_char_sets, old, sizeof (SXINT) * old_size);
    }
    else
	SXDS->sc_names_to_char_sets = (SXINT*) sxrealloc (SXDS->sc_names_to_char_sets,
							new_size,
							sizeof (SXINT));
}

static void
reg_exp_pcn_free (void)
{
    if (!SXDS->is_char_sets_static)
	sxbm_free (SXDS->char_sets);

    if (!SXWORD_is_static (SXDS->sc_names))
    {
	sxfree (SXDS->sc_names_to_char_sets);
	sxword_free (&SXDS->sc_names);
    }
}

static void
reg_exp_pcn_alloc (void)
{
    SXINT 		number = sizeof (predefs) / sizeof (predefs [0]);
    SXINT			c, i;
    SXINT	ste;
    SXBA		set;
    char		*string;

    sxword_alloc (&SXDS->sc_names, "simple_class_names",
		  (SXUINT) 2*number,
		  1, 16, sxcont_malloc, sxcont_alloc, sxcont_realloc, (sxresize_t) NULL, sxcont_free, reg_exp_pcn_oflw, NULL);

    SXDS->sc_names_to_char_sets = (SXINT*) sxalloc (SXWORD_size (SXDS->sc_names),
						  sizeof (SXINT));

    SXDS->char_sets_size = SXWORD_size (SXDS->sc_names);
    SXDS->char_sets_top = -1;
    SXDS->char_sets = sxbm_calloc (SXDS->char_sets_size, 1/*EOF*/+256);
    SXDS->is_char_sets_static = false;

    /* SXNUL et ANY sont des cas particuliers */
    ste = sxword_2save (&SXDS->sc_names, "NUL", 3);
    SXDS->sc_names_to_char_sets [ste] = ++SXDS->char_sets_top;
    SXBA_1_bit (SXDS->char_sets [SXDS->char_sets_top], 1);

    ste = sxword_2save (&SXDS->sc_names, "ANY", 3);
    SXDS->sc_names_to_char_sets [ste] = ++SXDS->char_sets_top;
    sxba_range (SXDS->char_sets [SXDS->char_sets_top], 1, 128);

    for (i = 0; i < number; i++)
    {
	ste = sxword_save (&SXDS->sc_names, predefs [i].name);
	SXDS->sc_names_to_char_sets [ste] = ++SXDS->char_sets_top;
	set = SXDS->char_sets [SXDS->char_sets_top];
	string = predefs [i].elems;

	while ((c = (SXINT) *string++) != 0)
	{
	    c++;
	    SXBA_1_bit (set, c);
	}
    }
}


static SXINT
char_sets_oflw (void)
{
    if (SXDS->is_char_sets_static)
    {
	SXBA			*old_cs = SXDS->char_sets;

	/* On fait ca en deux temps:
	   - copie conforme
	   - puis "resize"
	*/
	SXDS->char_sets = sxbm_calloc (SXDS->char_sets_size, 257);
	memcpy (SXDS->char_sets, old_cs, SXBA_total_sizeof (old_cs [0]) * SXDS->char_sets_size);
	SXDS->is_char_sets_static = false;
    }

    SXDS->char_sets = sxbm_resize (SXDS->char_sets,
				   SXDS->char_sets_size ,
				   SXDS->char_sets_size * 2,
				   257);
    SXDS->char_sets_size*=2;

    return SXDS->char_sets_top;
}

#define char_sets_incr()	((++SXDS->char_sets_top == SXDS->char_sets_size) \
				 ? char_sets_oflw () : SXDS->char_sets_top)

static void
terminals_oflw (SXINT old_size, SXINT new_size)
{
    if (SXWORD_is_static (SXDS->terminals))
    {
	struct terminal_to_attr *old = SXDS->terminal_to_attr;

	SXDS->terminal_to_attr =
	    (struct terminal_to_attr*) sxalloc (new_size,
						sizeof (struct terminal_to_attr));
	memcpy (SXDS->terminal_to_attr, old, sizeof (struct terminal_to_attr) * old_size);
    }
    else
	SXDS->terminal_to_attr =
	    (struct terminal_to_attr*) sxrealloc (SXDS->terminal_to_attr,
						  new_size,
						  sizeof (struct terminal_to_attr));
}

static void
abbrev_names_oflw (SXINT old_size, SXINT new_size)
{
    if (SXWORD_is_static (SXDS->abbrev_names))
    {
	struct abbrev_to_attr *old = SXDS->abbrev_to_attr;

	SXDS->abbrev_to_attr = (struct abbrev_to_attr*) sxalloc (new_size,
								 sizeof (struct abbrev_to_attr));
	memcpy (SXDS->abbrev_to_attr, old, sizeof (struct abbrev_to_attr) * old_size);
    }
    else
	SXDS->abbrev_to_attr =
	    (struct abbrev_to_attr*) sxrealloc (SXDS->abbrev_to_attr,
						new_size,
						sizeof (struct abbrev_to_attr));
}


static void
reg_exp_names_oflw (SXINT old_size, SXINT new_size)
{
    if (SXWORD_is_static (SXDS->reg_exp_names))
    {
	struct reg_exp_to_attr	*old = SXDS->reg_exp_to_attr;
	SXBA			*old_cs = SXDS->context_sets;

	SXDS->reg_exp_to_attr =
	    (struct reg_exp_to_attr*) sxalloc (new_size, sizeof (struct reg_exp_to_attr));
	memcpy (SXDS->reg_exp_to_attr, old, sizeof (struct reg_exp_to_attr) * old_size);

	/* On fait ca en deux temps:
	   - copie conforme
	   - puis "resize"
	   */
	SXDS->context_sets = sxbm_calloc (old_size, old_size);
	memcpy (SXDS->context_sets, old_cs, SXBA_total_sizeof (old_cs [0]) * old_size);
    }
    else
    {
	SXDS->reg_exp_to_attr =
	    (struct reg_exp_to_attr*) sxrealloc (SXDS->reg_exp_to_attr,
						 new_size,
						 sizeof (struct reg_exp_to_attr));
    }

    SXDS->context_sets = sxbm_resize (SXDS->context_sets,
				      old_size,
				      new_size,
				      new_size);

    SXDS->ctxt_set = SXDS->context_sets [0];
    SXDS->filter = SXDS->context_sets [1];
}


/* Si "sxreg_exp" est appele' depuis une analyse par syntax, chaque appel
   de "sxre_alloc", "sxre_free" et  "sxre_doit" doit etre
   precede par un appel a "sxre_store" et suivi par un appel a
   "sxre_restore". */
void
sxre_store (void)
{
    store.sxsrcmngr = sxsrcmngr;
    store.sxstrmngr = sxstrmngr;
    store.sxerrmngr = sxerrmngr;
    store.sxsvar = sxsvar;
    store.sxplocals = sxplocals;
    store.sxatcvar = sxatcvar;

    sxsrcmngr = SXDS->sx.sxsrcmngr;
    sxstrmngr = SXDS->sx.sxstrmngr;
    sxerrmngr = SXDS->sx.sxerrmngr;
    sxsvar = SXDS->sx.sxsvar;
    sxplocals = SXDS->sx.sxplocals;
    sxatcvar = SXDS->sx.sxatcvar;
}


void
sxre_restore (void)
{
    SXDS->sx.sxsrcmngr = sxsrcmngr;
    SXDS->sx.sxstrmngr = sxstrmngr;
    SXDS->sx.sxerrmngr = sxerrmngr;
    SXDS->sx.sxsvar = sxsvar;
    SXDS->sx.sxplocals = sxplocals;
    SXDS->sx.sxatcvar = sxatcvar;

    sxsrcmngr = store.sxsrcmngr;
    sxstrmngr = store.sxstrmngr;
    sxerrmngr = store.sxerrmngr;
    sxsvar = store.sxsvar;
    sxplocals = store.sxplocals;
    sxatcvar = store.sxatcvar;
}

void
sxre_alloc (void)
{
    /* Allocation et initialisation de sc_names, sc_names_to_char_sets et char_sets */
    vstr = NULL;
    reg_exp_pcn_alloc ();

    sxword_alloc (&SXDS->abbrev_names, "abbrev_names", 50,
		  1, 16, sxcont_malloc, sxcont_alloc, sxcont_realloc, (sxresize_t) NULL, sxcont_free, abbrev_names_oflw, NULL);

    SXDS->abbrev_to_attr =
	(struct abbrev_to_attr*) sxalloc (SXWORD_size (SXDS->abbrev_names),
					  sizeof (struct abbrev_to_attr));

    sxword_alloc (&SXDS->reg_exp_names, "reg_exp_names", 100,
		  1, 16, sxcont_malloc, sxcont_alloc, sxcont_realloc, (sxresize_t) NULL, sxcont_free, reg_exp_names_oflw, NULL);

    SXDS->reg_exp_to_attr = (struct reg_exp_to_attr*) sxalloc (SXWORD_size (SXDS->reg_exp_names),
							       sizeof (struct reg_exp_to_attr));

    SXDS->context_sets = sxbm_calloc ((SXINT) SXWORD_size (SXDS->reg_exp_names),
				      (SXINT) SXWORD_size (SXDS->reg_exp_names));

    SXDS->ctxt_set = SXDS->context_sets [0];
    SXDS->filter = SXDS->context_sets [1];

    SXDS->tfirst = NULL;

    SXDS->item_to_attr = (struct item_to_attr*) sxalloc (SXDS->item_size = 500,
							 sizeof (struct item_to_attr));
    SXDS->is_item_to_attr_static = false;
    SXDS->item_top = 0;

    XxY_alloc (&SXDS->nfsa, "nfsa", 500, 1, 1, 0, (sxoflw0_t) NULL, NULL);

    sxword_alloc (&SXDS->terminals, "terminals", 50,
		  1, 16, sxcont_malloc, sxcont_alloc, sxcont_realloc, (sxresize_t) NULL, sxcont_free, terminals_oflw, NULL);

    SXDS->terminal_to_attr = (struct terminal_to_attr*) sxalloc (SXWORD_size (SXDS->terminals),
								 sizeof (struct terminal_to_attr));
    SXDS->token_no_to_terminal_size = 0;
    SXDS->token_no_to_terminal = NULL;

    SXDS->counters_size = -1;
    SXDS->current_reduce_id = 0;

    SXDS->ws_lgth = 0, SXDS->ws = NULL;

    SXDS->eof_reg_exp_name = -1;
    SXDS->eof_code = -1;

}


SXINT
sxre_process_eof (SXINT eof_code)
{
    /* On initialise avec l'er : "EOF = -eof" ou eof est le code du "caractere"
       fin de fichier qui est pris egal a EOF_char, ce qui permet d'utiliser les SXBA. */

    SXUINT		eof_name;
    SXINT			couple;
    struct reg_exp_to_attr	*preattr;
    struct item_to_attr	*pitattr;

    eof_name = sxword_2save (&SXDS->reg_exp_names, "EOF", 3);

    preattr = SXDS->reg_exp_to_attr + eof_name;

    preattr->init_item = SXDS->item_top;
    preattr->post_action = -1;
    SXDS->eof_code = preattr->token_no = eof_code;
    preattr->reduce_id = ++SXDS->current_reduce_id;
    preattr->context_kind = Context; /* Context EOF */
    SXBA_1_bit (SXDS->context_sets [eof_name], eof_name);
    SXBA_1_bit (SXDS->filter, eof_name);
    preattr->priority_kind = NoPriority;
    preattr->kw_kind = KW_NO;
	
    pitattr = SXDS->item_to_attr + SXDS->item_top++;

    pitattr->reg_exp_no = eof_name;
    pitattr->val = 0;
    pitattr->param = 0;
    pitattr->is_erased = false;
    pitattr->kind = KEMPTY;

    XxY_set (&SXDS->nfsa, SXDS->item_top - 1, SXDS->item_top, &couple);

    pitattr = SXDS->item_to_attr + SXDS->item_top++;

    pitattr->reg_exp_no = eof_name;
    pitattr->val = char_sets_incr ();
    SXBA_1_bit (SXDS->char_sets [pitattr->val], EOF_char);
    pitattr->param = 0;
    pitattr->is_erased = true;
    pitattr->kind = KCLASS;

    XxY_set (&SXDS->nfsa, SXDS->item_top - 1, SXDS->item_top, &couple);

    pitattr = SXDS->item_to_attr + SXDS->item_top++;

    pitattr->reg_exp_no = eof_name;
    pitattr->val = 0;
    pitattr->param = 0;
    pitattr->is_erased = false;
    pitattr->kind = KREDUCE;

    return SXDS->eof_reg_exp_name = eof_name;
}

void
sxre_doit (FILE *infile, char *file_name_or_source_string)
{
    if (sxpglobals.parse_stack == NULL)
    {
	/* Non encore allouees */
	/* Allocation des variables globales du parser */
	(*(reg_exp_tables.SX_parser)) (SXBEGIN, NULL /* dummy */) ;
	SXDS->begin_parser = true;
    }
    else
	SXDS->begin_parser = false;

    if (SXDS->tfirst == NULL)
	/* Toujours dynamique */
	SXDS->tfirst = sxbm_calloc (SXDS->tfirst_line_nb = 64,
				    SXDS->tfirst_col_nb = 32);

    if (vstr == NULL)
	vstr = varstr_alloc (64);

    /* sxstr_mngr (SXBEGIN); Fait depuis l'exterieur */
    syntax (SXOPEN, &reg_exp_tables) /* Initialisation de SYNTAX */ ;

    SXDS->is_smp_error = false;

    /* sxerr_mngr (SXBEGIN); Fait depuis l'exterieur */
    sxsrc_mngr (SXINIT, infile, file_name_or_source_string);

    syntax (SXACTION, &reg_exp_tables);

    sxsrc_mngr (SXFINAL);
    /* sxerr_mngr (SXEND); Fait a` l'exterieur */

    syntax (SXCLOSE, &reg_exp_tables);
    /* sxstr_mngr (SXEND); Fait a` l'exterieur */

    if (SXDS->begin_parser)
    {
	SXDS->begin_parser = false;
	(*(reg_exp_tables.SX_parser)) (SXEND, NULL /* dummy */);
    }
}

void
sxre_free (void)
{
    /* Liberation de sc_names, sc_names_to_char_sets et char_sets */
    reg_exp_pcn_free ();

    if (!SXWORD_is_static (SXDS->abbrev_names))
    {
	sxword_free (&SXDS->abbrev_names);
	sxfree (SXDS->abbrev_to_attr);
    }

    if (!SXWORD_is_static (SXDS->reg_exp_names))
    {
	sxword_free (&SXDS->reg_exp_names);
	sxfree (SXDS->reg_exp_to_attr);
	sxbm_free (SXDS->context_sets);
    }

    /* Toujours dynamique */
    sxbm_free (SXDS->tfirst);

    if (!SXDS->is_item_to_attr_static)
	sxfree (SXDS->item_to_attr);

    if (!XxY_is_static (SXDS->nfsa))
	XxY_free (&SXDS->nfsa);

    if (!SXWORD_is_static (SXDS->terminals))
    {
	sxword_free (&SXDS->terminals);
	sxfree (SXDS->terminal_to_attr);
    }

    if (SXDS->token_no_to_terminal != NULL)
	sxfree (SXDS->token_no_to_terminal), SXDS->token_no_to_terminal = NULL;

    if (SXDS->ws != NULL)
	sxfree (SXDS->ws), SXDS->ws = NULL;

    if (vstr != NULL)
	varstr_free (vstr), vstr = NULL;
}



static void
initialize_reg_exp (SXINT init_item, SXBA first)
{
    /* Localement, pour une ER,  les items sont reperes a partir de 0. */
    SXINT		couple, next_item = -1;

    SXDS->delta = init_item + 1;

    while ((next_item = sxba_scan (first, next_item)) >= 0)
	XxY_set (&SXDS->nfsa, init_item, next_item + SXDS->delta, &couple);
}


static void
set_new_item (SXINT kind, SXINT set_new_item_item_no, SXINT val, SXINT param, SXBA next, bool is_erased)
{
    SXINT new_item_no, next_item, couple;
    struct item_to_attr	*pattr;

    new_item_no = set_new_item_item_no + SXDS->delta;
    pattr = SXDS->item_to_attr + new_item_no;

    pattr->reg_exp_no = SXDS->current_reg_exp_or_abbrev_no;
    pattr->val = val;
    pattr->param = param;
    pattr->kind = kind;
    pattr->is_erased = is_erased;

    if (next != NULL)
    {
	next_item = -1;

	while ((next_item = sxba_scan (next, next_item)) >= 0)
	    XxY_set (&SXDS->nfsa, new_item_no, next_item + SXDS->delta, &couple);
    }
}

static void
insert_abbrev (SXINT abbrev_no, SXINT insert_abbrev_item_no, SXBA next, bool is_erased)
{
    SXINT			init_item, final_item, new_item, delta, item, next_item,
                        couple, new_couple;
    struct item_to_attr	*pattr, *new_pattr;

    init_item = SXDS->abbrev_to_attr [abbrev_no].init_item;
    final_item = SXDS->abbrev_to_attr [abbrev_no].final_item;
    delta = insert_abbrev_item_no + SXDS->delta - init_item - 1;

    for (item = init_item + 1; item < final_item; item++)
    {
	new_item = item + delta;
	pattr = SXDS->item_to_attr + item;
	new_pattr = pattr + delta;

	new_pattr->reg_exp_no = SXDS->current_reg_exp_or_abbrev_no;
	new_pattr->val = pattr->val;
	new_pattr->kind = pattr->kind;
	new_pattr->is_erased = is_erased || pattr->is_erased;

	XxY_Xforeach (SXDS->nfsa, item, couple)
	{
	    next_item = XxY_Y (SXDS->nfsa, couple);

	    if (next_item == final_item)
	    {
		next_item = -1;

		while ((next_item = sxba_scan (next, next_item)) >= 0)
		    XxY_set (&SXDS->nfsa, new_item, next_item + SXDS->delta, &new_couple);
	    }
	    else
		XxY_set (&SXDS->nfsa, new_item, next_item + delta, &new_couple);
	}
    }
}

static void
abbrev_to_first (SXINT abbrev_no, SXINT abbrev_to_first_item_no, SXBA first)
{
    SXINT		init_item, delta, new_item, couple;

    init_item = SXDS->abbrev_to_attr [abbrev_no].init_item;
    delta = abbrev_to_first_item_no - init_item - 1;

    XxY_Xforeach (SXDS->nfsa, init_item, couple)
    {
	new_item = XxY_Y (SXDS->nfsa, couple) + delta;
	SXBA_1_bit (first, new_item);
    }
}



#include "reg_exp_nn.h"

static void
error (char *string, struct reg_exp_node *visited)
{
    sxerror (visited->token.source_index,
	     err_titles [2][0],
	     "%s%s.",
	     err_titles [2]+1,
	     string);
}


/* This entrypoint processes the first pass on Abbreviations and Tokens
   It computes:
   - item_no
   - node_no
   - is_empty
*/
static	bool
first_pass (struct reg_exp_node *visited, bool *is_action)
{
    /* collect explicit declarations of abbreviations and tokens */
    struct reg_exp_node	*son_1, *son_2;
    char	*name;
    SXINT		ste;
    bool	is_action_1, is_action_2;

    switch (visited->name) {
    case PUT_n:
    case ACTION_ERASE_n:
    case ACTION_NO_n:
    case DECR_n:
    case INCR_n:
    case LOWER_CASE_n:
    case LOWER_TO_UPPER_n:
    case MARK_n:
    case RELEASE_n:
    case RESET_n:
    case SET_n:
    case UPPER_CASE_n:
    case UPPER_TO_LOWER_n:
	visited->item_no = ++item_no;
	visited->node_no = ++node_no;
	visited->is_empty = false;
	*is_action = true;

	break;


    case NOT_n:
    case OCTAL_CODE_n  :
    case SLICE_n  :
    case STRING_n  :

    case IS_FALSE_n:
    case IS_TRUE_n:
    case IS_FIRST_COL_n:
    case IS_LAST_COL_n:
    case IS_RESET_n:
    case IS_SET_n:
    case PREDICATE_NO_n:
    case PREDICATE_NAME_n:
	visited->item_no = ++item_no;
	visited->node_no = ++node_no;
	visited->is_empty = false;
	*is_action = false;

	break;

    case ID_n  :
	visited->item_no = ++item_no;
	visited->node_no = ++node_no;
	*is_action = false;

	if (visited->token.string_table_entry == SXERROR_STE)
	    return false /* Correction d'erreur du parser */;

	name = sxstrget (visited->token.string_table_entry);
	ste = sxword_retrieve (&SXDS->abbrev_names, name);

	if (ste != SXERROR_STE)
	{
	    /* ste est l'identifiant d'une abbreviation */
	    item_no += SXDS->abbrev_to_attr [ste].final_item -
		SXDS->abbrev_to_attr [ste].init_item - 2;
	    visited->is_empty = SXDS->abbrev_to_attr [ste].is_empty;

	    break;
	}
	else
	    if (sxword_retrieve (&SXDS->sc_names, name) != SXERROR_STE)
	    {
		visited->is_empty = false;

		break;
	    }

	return error ("undefined", visited), false;

    case ALTERNATIVE_n:
	if (!first_pass (son_1 = sxson (visited, 1), &is_action_1)) return false;
	if (!first_pass (son_2 = son_1->brother, &is_action_2)) return false;
	if (is_action_1 && is_action_2)
	    return error ("alternative involving only actions", visited), false;
	visited->is_empty = son_1->is_empty || son_2->is_empty;
	*is_action = is_action_1 || is_action_2;
	visited->node_no = ++node_no;

	break;

    case SEQUENCE_n:
	if (!first_pass (son_1 = sxson (visited, 1), &is_action_1)) return false;
	if (!first_pass (son_2 = son_1->brother, &is_action_2)) return false;
	visited->is_empty = son_1->is_empty && son_2->is_empty;
	*is_action = is_action_1 && is_action_2;
	visited->node_no = son_1->is_empty ? ++node_no : son_1->node_no;

	break;

    case ERASE_n:
    case EXPRESSION_n:
	if (!first_pass (son_1 = sxson (visited, 1), is_action)) return false;
	visited->is_empty = son_1->is_empty;
	visited->node_no = son_1->node_no;

	break;

    case EXTENDED_CLASS_REF_n:
	if (!first_pass (son_1 = sxson (visited, 1), is_action)) return false;
	if (!first_pass (son_2 = son_1->brother, is_action)) return false;
	visited->is_empty = false;
	*is_action = false;
	visited->node_no = son_1->node_no;

	break;

    case OPTION_n:
	if (!first_pass (son_1 = sxson (visited, 1), is_action)) return false;
	if (*is_action) return error ("optional action", visited), false;
	visited->is_empty = true;
	visited->node_no = son_1->node_no;

	break;

    case REF_TRANS_CLOSURE_n:
	if (!first_pass (son_1 = sxson (visited, 1), is_action)) return false;
	if (*is_action) return error ("unbounded action sequence", visited), false;
	visited->is_empty = true;
	visited->node_no = ++node_no;

	break;

    case TRANS_CLOSURE_n:
	if (!first_pass (son_1 = sxson (visited, 1), is_action)) return false;
	if (*is_action) return error ("unbounded action sequence", visited), false;
	visited->is_empty = son_1->is_empty;
	visited->node_no = ++node_no;

	break;

    default:
	return error ("not yet implemented", visited), false;
    }

    return true;
}


/* This entrypoint processes the second pass on Abbreviations and Tokens
   It computes SXDS->tfirst
*/

static	bool
second_pass (struct reg_exp_node *visited)
{
    struct reg_exp_node	*son_1, *son_2;
    char		*name;
    SXINT			ste;

    switch (visited->name) {
    case PUT_n:
    case ACTION_ERASE_n:
    case ACTION_NO_n:
    case DECR_n:
    case INCR_n:
    case LOWER_CASE_n:
    case LOWER_TO_UPPER_n:
    case MARK_n:
    case RELEASE_n:
    case RESET_n:
    case SET_n:
    case UPPER_CASE_n:
    case UPPER_TO_LOWER_n:

    case NOT_n:
    case OCTAL_CODE_n  :
    case SLICE_n  :
    case STRING_n  :

    case IS_FALSE_n:
    case IS_TRUE_n:
    case IS_FIRST_COL_n:
    case IS_LAST_COL_n:
    case IS_RESET_n:
    case IS_SET_n:
    case PREDICATE_NO_n:
    case PREDICATE_NAME_n:
	SXBA_1_bit (sxba_empty (SXDS->tfirst [visited->node_no]), visited->item_no);
	break;

    case ID_n  :
	sxba_empty (SXDS->tfirst [visited->node_no]);

	name = sxstrget (visited->token.string_table_entry);
	ste = sxword_retrieve (&SXDS->abbrev_names, name);

	if (ste != SXERROR_STE)
	{
	    /* ste est l'identifiant d'une abbreviation */
	    /* Calcul specifique de SXDS->tfirst [visited->node_no] */
	    abbrev_to_first (ste, visited->item_no, SXDS->tfirst [visited->node_no]);
	}
	else
	    SXBA_1_bit (SXDS->tfirst [visited->node_no], visited->item_no);

	break;

    case ALTERNATIVE_n:
	if (!second_pass (son_1 = sxson (visited, 1))) return false;
	if (!second_pass (son_2 = son_1->brother)) return false;
	sxba_copy (SXDS->tfirst [visited->node_no], SXDS->tfirst [son_1->node_no]);
	sxba_or (SXDS->tfirst [visited->node_no], SXDS->tfirst [son_2->node_no]);

	break;

    case SEQUENCE_n:
	if (!second_pass (son_1 = sxson (visited, 1))) return false;
	if (!second_pass (son_2 = son_1->brother)) return false;

	if (son_1->is_empty)
	{	
	    sxba_copy (SXDS->tfirst [visited->node_no], SXDS->tfirst [son_1->node_no]);
	    sxba_or (SXDS->tfirst [visited->node_no], SXDS->tfirst [son_2->node_no]);
	}

	break;

    case OPTION_n:
    case ERASE_n:
    case EXPRESSION_n:
	if (!second_pass (sxson (visited, 1))) return false;

	break;

    case REF_TRANS_CLOSURE_n:
    case TRANS_CLOSURE_n:
	if (!second_pass (son_1 = sxson (visited, 1))) return false;
	sxba_copy (SXDS->tfirst [visited->node_no], SXDS->tfirst [son_1->node_no]);

	break;

    case EXTENDED_CLASS_REF_n:
	if (!second_pass (son_1 = sxson (visited, 1))) return false;
	if (!second_pass (son_2 = son_1->brother)) return false;

	break;

    default:
	return error ("not yet implemented", visited), false;
    }

    return true;
}



static bool
class_denotations (struct reg_exp_node *visited)
{
    struct reg_exp_node	*son_1;
    char		*name, *last, c;
    SXINT			ste, value, i, j;
    SXBA		char_set1, char_set2, char_set;

/* Visite recursive des denotations de classes */
    switch (visited->name) {
    case ID_n  :
	if (visited->token.string_table_entry == SXERROR_STE)
	    return false /* Correction d'erreur du parser */;

	name = sxstrget (visited->token.string_table_entry);
	ste = sxword_retrieve (&SXDS->sc_names, name);

	if (ste != SXERROR_STE)
	{
	    /* ste est l'identifiant d'une classe simple */
	    /* On est ds le cas d'une copie (Classes ou Operande de NOT ou SLICE). */
	    sxba_copy (SXDS->char_sets [char_sets_incr ()],
		       SXDS->char_sets [SXDS->sc_names_to_char_sets [ste]]);
	    break;
	}

	return error ("undefined class denotation", visited), false;

    case OCTAL_CODE_n  :
	if (visited->token.string_table_entry == SXERROR_STE)
	    return false;

        name = sxstrget (visited->token.string_table_entry) + 1 /* On saute le "#". */;
        value = 0;

	while ((c = *name++) != SXNUL)
	{
	    value *= 8;
	    value += (SXINT) (c - '0');
	}

	/* Les codes des caracteres sont decales de 1, EOF a le code 0 */
	if (++value > 256)
	    return error ("octal codes must range between 0 and 255", visited), false;

	SXBA_1_bit (sxba_empty (SXDS->char_sets [char_sets_incr ()]), value);
	
	break;


    case SLICE_n  :
	if (!class_denotations (son_1 = sxson (visited, 1))) return false;

	if (!class_denotations (son_1->brother)) return false;

	char_set2 = SXDS->char_sets [SXDS->char_sets_top--];
	char_set1 = SXDS->char_sets [SXDS->char_sets_top];

	if (sxba_cardinal (char_set1) == 1 && sxba_cardinal (char_set2) == 1)
	{
	    i = sxba_scan (char_set1, -1);
	    j = sxba_scan (char_set2, -1);

	    if (j >= i)
	    {
		sxba_range (char_set1, i, j - i + 1);
		break;
	    }
	}

	return error ("erroneous slice denotation", visited), false;
	    


    case STRING_n  :
	if (visited->token.string_table_entry == SXERROR_STE)
	    return false;

	/* On "saute" les doubles quotes. */
	name = sxstrget (visited->token.string_table_entry) + 1;
	last = name + sxstrlen (visited->token.string_table_entry) - 2;
	sxba_empty (char_set = SXDS->char_sets [char_sets_incr ()]);

	while (name != last)
	{
	    c = *name++ + 1;
	    SXBA_1_bit (char_set, c);
	}

	break;

    default :
	return error ("not yet implemented", visited), false;
    }

    return true;
}


static	bool
process_action (struct reg_exp_node *visited, SXINT *action, SXINT *param)
{
    SXBA	char_set;

    *param = 0;

    switch (visited->name) {
    case PUT_n:
	*action = Put;
	
	if (!class_denotations (visited->son)) return false;

	char_set = SXDS->char_sets [SXDS->char_sets_top--];
	*param = sxba_scan (char_set, -1);

	if (*param == -1 || sxba_scan (char_set, *param) != -1)
	    return error ("erroneous parameter", visited), false;

	(*param)--;

	break;

    case ACTION_ERASE_n:
	*action = Erase;
	break;

    case ACTION_NO_n:
	*action = atoi (sxstrget (visited->token.string_table_entry));
	break;

    case DECR_n:
	*action = Decr;
	*param = atoi (sxstrget (visited->token.string_table_entry));

	if (*param > SXDS->counters_size) SXDS->counters_size = *param;

	break;

    case INCR_n:
	*action = Incr;
	*param = atoi (sxstrget (visited->token.string_table_entry));

	if (*param > SXDS->counters_size) SXDS->counters_size = *param;

	break;

    case LOWER_CASE_n:
	*action = LowerCase;
	break;

    case LOWER_TO_UPPER_n:
	*action = LowerToUpper;
	break;

    case MARK_n:
	*action = Mark;
	break;

    case RELEASE_n:
	*action = Release;
	break;

    case RESET_n:
	*action = Reset;
	*param = atoi (sxstrget (visited->token.string_table_entry));

	if (*param > SXDS->counters_size) SXDS->counters_size = *param;

	break;

    case SET_n:
	*action = Set;
	*param = atoi (sxstrget (visited->token.string_table_entry));

	if (*param > SXDS->counters_size) SXDS->counters_size = *param;

	break;

    case UPPER_CASE_n:
	*action = UpperCase;
	break;

    case UPPER_TO_LOWER_n:
	*action = UpperToLower;
	break;
    default: /* pour faire taire gcc -Wswitch-default */
#if EBUG
      sxtrap("sxreg_exp_mngr","unknown switch case #1");
#endif
      break;
    }

    return true;
}


static	bool
process_predicate (struct reg_exp_node *visited, SXINT *action, SXINT *param)
{
    *param = 0;

    switch (visited->name) {

    case PREDICATE_NO_n:
	*action = atoi (sxstrget (visited->token.string_table_entry));
	break;

    case IS_TRUE_n:
	*action = IsTrue;
	break;

    case IS_FALSE_n:
	*action = IsFalse;
	break;

    case IS_FIRST_COL_n:
	*action = IsFirstCol;
	break;

    case IS_LAST_COL_n:
	*action = IsLastCol;
	break;

    case IS_RESET_n:
	*action = IsReset;
	*param = atoi (sxstrget (visited->token.string_table_entry));
	break;

    case IS_SET_n:
	*action = IsSet;
	*param = atoi (sxstrget (visited->token.string_table_entry));
	break;

    case PREDICATE_NAME_n:
	return error ("not yet implemented", visited), false;
    default: /* pour faire taire gcc -Wswitch-default */
#if EBUG
      sxtrap("sxreg_exp_mngr","unknown switch case #2");
#endif
      break;
    }

    return true;
}


/* This entrypoint processes the third pass on Abbreviations and Tokens
   It computes next
*/


static	bool
third_pass (struct reg_exp_node *visited, SXBA next, bool is_del)
{
    struct reg_exp_node	*son_1, *son_2;
    char		*name;
    SXINT			ste, action, param;
    SXBA		char_set1;

    sxinitialise(action); /* pour faire taire "gcc -Wuninitialized" */
    switch (visited->name) {
    case PUT_n:
    case ACTION_ERASE_n:
    case ACTION_NO_n:
    case DECR_n:
    case INCR_n:
    case LOWER_CASE_n:
    case LOWER_TO_UPPER_n:
    case MARK_n:
    case RELEASE_n:
    case RESET_n:
    case SET_n:
    case UPPER_CASE_n:
    case UPPER_TO_LOWER_n:
	if (!process_action (visited, &action, &param)) return false;

	set_new_item ((SXINT)KACTION, visited->item_no, action, param, next, is_del);
	break;

    case NOT_n:
	/* ANY - top == ~top & ANY */
	if (!class_denotations (sxson (visited, 1))) return false;

	char_set1 = SXDS->char_sets [SXDS->char_sets_top];
	sxba_not (char_set1);
	ste = sxword_2retrieve (&SXDS->sc_names, "ANY", 3);
	sxba_and (char_set1, SXDS->char_sets [SXDS->sc_names_to_char_sets [ste]]);
	set_new_item ((SXINT)KCLASS, visited->item_no, SXDS->char_sets_top, (SXINT)0, next, is_del);
	break;


    case OCTAL_CODE_n  :
    case SLICE_n  :
    case STRING_n  :
	if (!class_denotations (visited)) return false;

	set_new_item ((SXINT)KCLASS, visited->item_no, SXDS->char_sets_top, (SXINT)0,
		      next, is_del);
	break;

    case IS_FALSE_n:
    case IS_TRUE_n:
    case IS_FIRST_COL_n:
    case IS_LAST_COL_n:
    case IS_RESET_n:
    case IS_SET_n:
    case PREDICATE_NO_n:
    case PREDICATE_NAME_n:
	if (!process_predicate (visited, &action, &param)) return false;

	set_new_item ((SXINT)KPREDICATE, visited->item_no, action, param, next, is_del);
	break;

    case ID_n  :
	name = sxstrget (visited->token.string_table_entry);
	ste = sxword_retrieve (&SXDS->abbrev_names, name);

	if (ste != SXERROR_STE)
	{
	    /* ste est l'identifiant d'une abbreviation */
	    if (ste == SXDS->current_reg_exp_or_abbrev_no ||
		ste == -SXDS->current_reg_exp_or_abbrev_no)
		/* Utilisation recursive */
		return error ("recursive usage", visited), false;

	    insert_abbrev (ste, visited->item_no, next, is_del);
	}
	else
	{
	    ste = sxword_retrieve (&SXDS->sc_names, name);
	    /* ste est l'identifiant d'une classe simple */
	    set_new_item ((SXINT)KCLASS, visited->item_no, SXDS->sc_names_to_char_sets [ste], (SXINT)0,
			  next, is_del);
	}

	break;

    case ALTERNATIVE_n:
	if (!third_pass (son_1 = sxson (visited, 1), next, is_del)) return false;
	if (!third_pass (son_2 = son_1->brother, next, is_del)) return false;

	break;

    case SEQUENCE_n:
	son_1 = sxson (visited, 1);
	son_2 = son_1->brother;

	/* On commence par le dernier composant afin de pouvoir reutiliser SXDS->tfirst. */
	if (!third_pass (son_2, next, is_del)) return false;

	if (son_2->is_empty)
	    sxba_or (SXDS->tfirst [son_2->node_no], next);

	if (!third_pass (son_1, SXDS->tfirst [son_2->node_no], is_del)) return false;

	break;

    case ERASE_n:
	son_1 = sxson (visited, 1);
	if (!third_pass (son_1, next, true)) return false;

	break;


    case OPTION_n:
    case EXPRESSION_n:
	son_1 = sxson (visited, 1);
	if (!third_pass (son_1, next, is_del)) return false;

	break;

    case REF_TRANS_CLOSURE_n:
    case TRANS_CLOSURE_n:
	son_1 = sxson (visited, 1);
	if (!third_pass (son_1, sxba_or (SXDS->tfirst [son_1->node_no], next), is_del))
	    return false;

	break;


    case EXTENDED_CLASS_REF_n:
	son_1 = sxson (visited, 1);
	son_2 = son_1->brother;

	if (!third_pass (son_1, SXDS->tfirst [son_2->node_no], is_del)) return false;
	if (!third_pass (son_2, next, is_del)) return false;

	break;

    default:
	return error ("not yet implemented", visited), false;
    }

    return true;
}


static bool
classes (struct reg_exp_node *visited)
{
    struct reg_exp_node	*son_1, *son_2;
    char		*name;
    SXINT			lgth, ste;
    SXBA		char_set1, char_set2;

/* Visite recursive des CLASSES */
    switch (visited->name) {
    case CLASS_n  :
	son_1 = sxson (visited, 1); /* CLASS_NAME_n */
	son_2 = son_1->brother; /* <CLASS_EXP> */

	if (!classes (son_2)) return false;

	if (son_1->token.string_table_entry == SXERROR_STE)
	    return false;

	name = sxstrget (son_1->token.string_table_entry);
	lgth = sxstrlen (son_1->token.string_table_entry);

	if (sxword_2retrieve (&SXDS->abbrev_names, name, (SXUINT) lgth) != SXERROR_STE)
	    /* C'est une abbrev */
	    return error ("illegal usage of an abbreviation name", son_1), false;

	/* On peut redefinir une classe simple */
	ste = sxword_2save (&SXDS->sc_names, name, (SXUINT) lgth);
	SXDS->sc_names_to_char_sets [ste] = SXDS->char_sets_top;

	break;

    /* case CLASS_NAME_n  : Non visite' 
       break; */

    case CLASS_S_n  :
	for (son_1 = sxson (visited, 1); son_1 != NULL; son_1 = son_1->brother)
	    if (!classes (son_1)) return false;

	break;

    case ID_n  :
    case OCTAL_CODE_n  :
    case SLICE_n  :
    case STRING_n  :
	return class_denotations (visited);

    case MINUS_n  :
    case PLUS_n  :
	son_1 = sxson (visited, 1);

	if (!classes (son_1)) return false;

	son_2 = son_1->brother;

	if (!classes (son_2)) return false;

	char_set2 = SXDS->char_sets [SXDS->char_sets_top--];
	char_set1 = SXDS->char_sets [SXDS->char_sets_top];

	if (visited->name == MINUS_n)
	    sxba_minus (char_set1, char_set2);
	else
	    sxba_or (char_set1, char_set2);

	break;

    default:
	return error("Internal error in function \"classes\", please report.", visited), false;
    }

    return true;
}



static SXINT
re_maj_item_top (SXINT n)
{

    SXDS->item_top += n;

    if (SXDS->item_top >= SXDS->item_size)
    {
	SXINT	old_size = SXDS->item_size;

	while (SXDS->item_top >= SXDS->item_size)
	    SXDS->item_size *= 2;

	if (SXDS->is_item_to_attr_static)
	{
	    struct item_to_attr *old = SXDS->item_to_attr;

	    SXDS->item_to_attr = (struct item_to_attr*) sxalloc (SXDS->item_size,
								 sizeof (struct item_to_attr));
	    memcpy (SXDS->item_to_attr, old, sizeof (struct item_to_attr) * old_size);
	    SXDS->is_item_to_attr_static = false;
	}
	else
	    SXDS->item_to_attr =
		(struct item_to_attr*) sxrealloc (SXDS->item_to_attr,
						  SXDS->item_size,
						  sizeof (struct item_to_attr));
    }

    return SXDS->item_top;
}


static bool
regular_expression (struct reg_exp_node *visited, bool *is_action)
{
    /* visited->name == REGULAR_EXPRESSION_n */

    struct reg_exp_node	*son_1 = sxson (visited, 1);

    node_no = -1;
    item_no = -1;

    if (!first_pass (son_1, is_action)) return false;

    visited->node_no = ++node_no;
    visited->item_no = ++item_no; /* etat final */

    if (node_no >= SXDS->tfirst_line_nb ||
	item_no >= SXDS->tfirst_col_nb)
    {
      SXINT prev_line_nb = SXDS->tfirst_line_nb;

      while (node_no >= SXDS->tfirst_line_nb)
	SXDS->tfirst_line_nb *= 2;

      while (item_no >= SXDS->tfirst_col_nb)
	SXDS->tfirst_col_nb *= 2;

      SXDS->tfirst = sxbm_resize (SXDS->tfirst,
				  prev_line_nb,
				  SXDS->tfirst_line_nb,
				  SXDS->tfirst_col_nb);
    }
	
    re_maj_item_top (item_no + 2 /* items initial et final */);

    if (!second_pass (son_1)) return false;

    initialize_reg_exp (SXDS->current_reg_exp_or_abbrev_no >= 0
		      ? SXDS->reg_exp_to_attr [SXDS->current_reg_exp_or_abbrev_no].init_item
		      : SXDS->abbrev_to_attr [-SXDS->current_reg_exp_or_abbrev_no].init_item,
		      SXDS->tfirst [son_1->node_no]);

    SXBA_1_bit (sxba_empty (SXDS->tfirst [node_no]), item_no);

    if (!third_pass (son_1, SXDS->tfirst [node_no], false)) return false;

    return true;
}


static bool
abbreviations (struct reg_exp_node *visited)
{
    struct reg_exp_node	*son_1, *son_2;
    char		*name;
    SXINT			lgth;
    bool		is_action;


/* Visite recursive des ABBREVIATIONS */
    switch (visited->name) {
    case ABBREVIATION_S_n  :
	for (son_1 = sxson (visited, 1); son_1 != NULL; son_1 = son_1->brother)
	    if (!abbreviations (son_1)) return false;

	break;

    case ABBREVIATION_n  :
	son_1 = sxson (visited, 1); /* ABBREVIATION_RE_NAME_n  or ABBREVIATION_PRDCT_NAME_n */

	if (son_1->name == ABBREVIATION_PRDCT_NAME_n)
	    /* Pour le moment */
	    return error ("sorry, not implemented", son_1), false;

	if (son_1->token.string_table_entry == SXERROR_STE)
	    return false;

	son_2 = son_1->brother; /* <REGULAR_EXPRESSION> or <&_EXPRESSION> */

	name = sxstrget (son_1->token.string_table_entry);
	lgth = sxstrlen (son_1->token.string_table_entry);

	if (sxword_2retrieve (&SXDS->sc_names, name, (SXUINT) lgth) != SXERROR_STE)
	    /* C'est une classe simple */
	    return error ("already defined as a class name", son_1), false;
	
	if (sxword_2retrieve (&SXDS->abbrev_names, name, (SXUINT) lgth) != SXERROR_STE)
	    /* Double definition */
	    return error ("already defined", son_1), false;

	SXDS->current_reg_exp_or_abbrev_no = -sxword_2save (&SXDS->abbrev_names, name, (SXUINT) lgth);
	SXDS->abbrev_to_attr [-SXDS->current_reg_exp_or_abbrev_no].init_item =
	    SXDS->item_top;

	if (!regular_expression (son_2, &is_action /* unused */)) return false;

	SXDS->abbrev_to_attr [-SXDS->current_reg_exp_or_abbrev_no].is_empty = son_2->is_empty;
	SXDS->abbrev_to_attr [-SXDS->current_reg_exp_or_abbrev_no].final_item =
	    SXDS->item_top - 1;
	
	break;
    default: /* pour faire taire gcc -Wswitch-default */
#if EBUG
      sxtrap("sxreg_exp_mngr","unknown switch case #3");
#endif
      break;
    }

    return true;
}


static bool
set_priority_kind (struct reg_exp_node *visited, char current)
{
    char *p = &(SXDS->reg_exp_to_attr [SXDS->current_reg_exp_or_abbrev_no].priority_kind);

    if ((*p & current) ||
	((current != ReduceReduce) && (*p & (ShiftReduce + ReduceShift))))
	return error ("illegal priority clause", visited), false;

    *p |= current;

    return true;
}

static bool
environment (struct reg_exp_node *visited)
{
    struct reg_exp_node	*son_1;
    char	*name;
    SXINT		lgth, ste;

    switch (visited->name) {
    case CONTEXT_n:
	son_1 = sxson (visited, 1);
	
	SXDS->reg_exp_to_attr [SXDS->current_reg_exp_or_abbrev_no].context_kind = 
	    Context;

	if (!environment (son_1)) return false;

	break;

    case RESTRICTED_CONTEXT_n:
	son_1 = sxson (visited, 1);
	
	SXDS->reg_exp_to_attr [SXDS->current_reg_exp_or_abbrev_no].context_kind = 
	    (son_1->name == VOID_n) ? ContextAll : ContextAllBut;

	if (!environment (son_1)) return false;

	break;

    case UNBOUNDED_CONTEXT_n:
	son_1 = sxson (visited, 1);
	
	SXDS->reg_exp_to_attr [SXDS->current_reg_exp_or_abbrev_no].context_kind = 
	    UnboundedContext;

	if (!environment (son_1)) return false;

	break;

    case UNBOUNDED_RESTRICTED_CONTEXT_n:
	son_1 = sxson (visited, 1);
	
	SXDS->reg_exp_to_attr [SXDS->current_reg_exp_or_abbrev_no].context_kind = 
	    (son_1->name == VOID_n) ? UnboundedContextAll : UnboundedContextAllBut;

	if (!environment (son_1)) return false;

	break;

    case COMPONENT_NAME_REF_n:
	/* Le deuxieme fils est traite' depuis son frere gauche. */
	break;

    case COMPONENT_REF_n:
	if (!environment (sxson (visited, 1))) return false;
	/* Le deuxieme fils est traite' depuis son frere gauche. */

	break;

    case CONTEXT_COMMENTS_n:
	name = "COMMENTS";
	lgth = 8;
	goto process_context;

    case CONTEXT_EOF_n:
	name = "EOF";
	lgth = 3;
	goto process_context;

    case CONTEXT_INCLUDE_n:
	name = "INCLUDE";
	lgth = 7;
	goto process_context;

    case CONTEXT_NAME_n:
	if (visited->token.string_table_entry == SXERROR_STE)
	    return false;

	name = sxstrget (visited->token.string_table_entry);
	lgth = sxstrlen (visited->token.string_table_entry);

process_context:
	son_1 = visited->brother;

	if (son_1->name == COMPONENT_NAME_REF_n)
	{
	    if (son_1->token.string_table_entry == SXERROR_STE)
		return false;

	    varstr_raz (vstr);
	    varstr_catenate (vstr, name);
	    varstr_catchar (vstr, '.');
	    varstr_catenate (vstr, sxstrget (son_1->token.string_table_entry));
	    name = varstr_tostr (vstr);
	    lgth = varstr_length (vstr);
	}

	ste = sxword_2retrieve (&SXDS->reg_exp_names, name, (SXUINT) lgth);

	if (ste == SXERROR_STE)
	{
	    /* Premiere occurrence de ce nom de reg_exp */
	    struct reg_exp_to_attr	*p;

	    ste = sxword_2save (&SXDS->reg_exp_names, name, (SXUINT) lgth);
	    SXBA_1_bit (SXDS->filter, ste); /* a priori */
	    p = &(SXDS->reg_exp_to_attr [ste]);

	    p->reduce_id = -1;
	    p->init_item = -1;
	    p->post_action = -1;
	    p->token_no = -1;
	    p->priority_kind = NoPriority;
	    p->context_kind = ContextAll;
	    p->kw_kind = KW_NO;
	}

	SXBA_1_bit (SXDS->context_sets [SXDS->current_reg_exp_or_abbrev_no], ste);
	break;

    case ENVIRONMENT_S_n:
    case PRIORITY_KIND_S_n:
    case TOKEN_REF_S_n:
    case KEYWORDS_n:
    case NOT_KEYWORDS_n:
	for (son_1 = sxson (visited, 1); son_1 != NULL; son_1 = son_1->brother)
	    if (!environment (son_1)) return false;

	break;

    case KEYWORD_NAME_n:
	if (visited->token.string_table_entry == SXERROR_STE)
	    return false;

	name = sxstrget (visited->token.string_table_entry);
	lgth = sxstrlen (visited->token.string_table_entry);

	if (*name == '"')
	    name++, lgth -= 2;

	ste = sxword_2save (&SXDS->terminals, name, (SXUINT) lgth);
        {
	    struct terminal_to_attr *p = SXDS->terminal_to_attr + ste;

	    p->token_no = -1;
	    p->next_syno = -1;
	    p->is_kw = true;
	    p->is_generic = false;
	}

	break;

    case KEYWORDS_SPEC_n:
	son_1 = sxson (visited, 1);

	if (son_1->name == VOID_n)
	{
	    SXDS->reg_exp_to_attr [SXDS->current_reg_exp_or_abbrev_no].kw_kind = KW_YES;

	    if (!environment (son_1->brother->brother)) return false;
	}
	else
	    SXDS->reg_exp_to_attr [SXDS->current_reg_exp_or_abbrev_no].kw_kind = KW_NO | KW_PROHIB;

	break;

    case POST_ACTION_n:
	SXDS->reg_exp_to_attr [SXDS->current_reg_exp_or_abbrev_no].post_action =
	    atoi (sxstrget (visited->token.string_table_entry));
	break;

    case SHIFT_REDUCE_n:
	if (!set_priority_kind (visited, ShiftReduce)) return false; 

	break;

    case REDUCE_REDUCE_n:
	if (!set_priority_kind (visited, ReduceReduce)) return false; 

	break;

    case REDUCE_SHIFT_n:
	if (!set_priority_kind (visited, ReduceShift)) return false; 

	break;
	
    case VOID_n :
	break;
    default: /* pour faire taire gcc -Wswitch-default */
#if EBUG
      sxtrap("sxreg_exp_mngr","unknown switch case #4");
#endif
      break;
    }

    return true;
}


static SXINT
GetReduceId (SXINT re1)
{
    SXINT				re2;
    struct reg_exp_to_attr	*pre1, *pre2;

    pre1 = SXDS->reg_exp_to_attr + re1;

    for (re2 = SXWORD_top (SXDS->reg_exp_names); re2 > SXEMPTY_STE; re2--)
    {
	if (re2 != re1)
	{
	    pre2 = SXDS->reg_exp_to_attr + re2;

	    if (pre2->reduce_id >= 0)
	    {
		/* Deja initialise' */
		if (pre1->token_no == pre2->token_no &&
		    pre1->post_action == pre2->post_action &&
		    pre1->kw_kind == pre2->kw_kind &&
		    pre1->priority_kind == pre2->priority_kind &&
		    pre1->context_kind == pre2->context_kind &&
		    sxba_first_difference (SXDS->context_sets [re1],
					   SXDS->context_sets [re2]) == -1
		    )
		    return pre2->reduce_id;
	    }
	}
    }

    return ++SXDS->current_reduce_id;
} 


static bool
reg_exps (struct reg_exp_node *visited)
{
    struct reg_exp_node	*son_1, *son_2;
    struct reg_exp_to_attr	*p;
    char		*name;
    SXINT			lgth, init_item, couple, new_couple, cur_reg_exp;
    bool		is_action;

    sxinitialise(lgth); /* pour faire taire "gcc -Wuninitialized" */
    sxinitialise(name); /* pour faire taire "gcc -Wuninitialized" */
    /* Visite recursive des TOKENS */
    switch (visited->name) {
    case COMPONENTS_S_n  :
    case TOKEN_S_n  :
	for (son_1 = sxson (visited, 1); son_1 != NULL; son_1 = son_1->brother)
	    if (!reg_exps (son_1)) return false;

	break;

    case COMPONENT_n  :
	son_1 = sxson (visited, 1); /* <COMPONENT_DEF> */
	son_2 = son_1->brother; /* <VOID> or <ENVIRONMENT_LIST> */

	/* On visite <COMPONENT_NAME_DEF> depuis COMPONENT_n */
	son_1 = sxson (son_1, 1); /* COMPONENT_NAME_DEF_n */

	if (son_1->token.string_table_entry == SXERROR_STE)
	    return false;

	varstr_raz (vstr);
	varstr_catenate (vstr, SXWORD_get (SXDS->reg_exp_names, master_reg_exp_no));
	varstr_catchar (vstr, '.');
	varstr_catenate (vstr, sxstrget (son_1->token.string_table_entry));
	name = varstr_tostr (vstr);
	lgth = varstr_length (vstr);

	SXDS->current_reg_exp_or_abbrev_no =
	    sxword_2retrieve (&SXDS->reg_exp_names, name, (SXUINT) lgth);

	p = &(SXDS->reg_exp_to_attr [SXDS->current_reg_exp_or_abbrev_no]);

	if (SXDS->current_reg_exp_or_abbrev_no != SXERROR_STE && p->init_item != -1)
	    /* Double definition */
	    return error ("redefinition", son_1), false;

	/* Un nom de token peut etre une abreviation ou une classe simple */
	if (SXDS->current_reg_exp_or_abbrev_no == SXERROR_STE)
	{
	    /* Premiere occurrence de ce nom de composant d'union */
	    SXDS->current_reg_exp_or_abbrev_no =
		sxword_2save (&SXDS->reg_exp_names, name, (SXUINT) lgth);
	    SXBA_1_bit (SXDS->filter, SXDS->current_reg_exp_or_abbrev_no); /* a priori */
	    p = &(SXDS->reg_exp_to_attr [SXDS->current_reg_exp_or_abbrev_no]);

	    /* Herite de l'environement maitre. */
	    *p = SXDS->reg_exp_to_attr [master_reg_exp_no];
	    p->init_item = -1;
	}

	/* On commence par traiter l'environement */
	if (!environment (son_2)) return false;

	if (!reg_exps (sxson (visited, 1))) return false;

	p = &(SXDS->reg_exp_to_attr [SXDS->current_reg_exp_or_abbrev_no]);

	if (p->init_item >= 0)
	    /* Definition complete, on calcule le "reduce_id". */
	    p->reduce_id = GetReduceId (SXDS->current_reg_exp_or_abbrev_no);

	break;

    case COMPONENT_DEF_n  :
	son_1 = sxson (visited, 1); /* COMPONENT_NAME_DEF_n */
	/* Deja visite' par le pere */

	son_2 = son_1->brother; /* <REGULAR_EXPRESSION> */

	p = &(SXDS->reg_exp_to_attr [SXDS->current_reg_exp_or_abbrev_no]);
	p->init_item = SXDS->item_top;

	if (!regular_expression (son_2, &is_action)) return false;

	if (is_action) return error ("illegal action usage in a regular expression", son_2), false;
	
	set_new_item ((SXINT)KREDUCE, item_no, (SXINT)0, (SXINT)0, (SXBA)NULL, false);

	p = &(SXDS->reg_exp_to_attr [SXDS->current_reg_exp_or_abbrev_no]);
	init_item = SXDS->reg_exp_to_attr [master_reg_exp_no].init_item;

	XxY_Xforeach (SXDS->nfsa, p->init_item, couple)
	{
	    XxY_set (&SXDS->nfsa, init_item, XxY_Y (SXDS->nfsa, couple), &new_couple);
	}

	break;

    case TOKEN_n  :
	son_1 = sxson (visited, 1); /* <TOKEN_DEF> */
	son_2 = son_1->brother; /* <VOID> or <ENVIRONMENT_LIST> */

	/* On visite <LEXICAL_UNIT_NAME> depuis TOKEN_n */
	son_1 = sxson (son_1, 1); /* <LEXICAL_UNIT_NAME> */
	/* %GENERIC_NAME, %STRING_LITERAL, %IDENTIFIER, EOF, COMMENTS, INCLUDE */

	if (son_1->name != LEXICAL_UNIT_NAME_n)
	{
	    switch (son_1->name)
	    {
	    case COMMENTS_n:
		name = "COMMENTS";
		lgth = 8;
		break;

	    case EOF_n:
		/* Pre'de'clare' ds "sxre_alloc" => erreur Double definition */
		name = "EOF";
		lgth = 3;
		break;

	    case INCLUDE_n:
		name = "INCLUDE";
		lgth = 7;
		break;
	    default: /* pour faire taire gcc -Wswitch-default */
#if EBUG
	      sxtrap("sxreg_exp_mngr","warning unknown switch case #5");
#endif
	      break;
	    }
	}
	else
	{
	    if (son_1->token.string_table_entry == SXERROR_STE)
		return false;

	    name = sxstrget (son_1->token.string_table_entry);
	    lgth = sxstrlen (son_1->token.string_table_entry);
	}

	SXDS->current_reg_exp_or_abbrev_no =
	    sxword_2retrieve (&SXDS->reg_exp_names, name, (SXUINT) lgth);

	p = &(SXDS->reg_exp_to_attr [SXDS->current_reg_exp_or_abbrev_no]);

	if (SXDS->current_reg_exp_or_abbrev_no != SXERROR_STE && p->init_item != -1)
	    /* Double definition */
	    return error ("redefinition", son_1), false;

	/* Un nom de token peut etre une abreviation ou une classe simple */
	if (SXDS->current_reg_exp_or_abbrev_no == SXERROR_STE)
	{
	    /* Premiere occurrence de ce nom de reg_exp */
	    SXDS->current_reg_exp_or_abbrev_no =
		sxword_2save (&SXDS->reg_exp_names, name, (SXUINT) lgth);
	    SXBA_1_bit (SXDS->filter, SXDS->current_reg_exp_or_abbrev_no); /* a priori */
	    p = &(SXDS->reg_exp_to_attr [SXDS->current_reg_exp_or_abbrev_no]);

	    p->init_item = -1;
	    p->reduce_id = -1;
	    p->post_action = -1;
	    p->token_no = son_1->name == COMMENTS_n ? 0 : -1;
	    p->priority_kind = NoPriority;
	    p->context_kind = ContextAll;
	    p->kw_kind = KW_NO;
	}

	cur_reg_exp = SXDS->current_reg_exp_or_abbrev_no; /* Le 13/04/07 */

	/* On commence par traiter l'environment */
	if (!environment (son_2)) return false;

	if (!reg_exps (sxson (visited, 1))) return false;

	p = &(SXDS->reg_exp_to_attr [cur_reg_exp]);

	if (p->init_item >= 0)
	    /* Definition complete, on calcule le "reduce_id". */
	    p->reduce_id = GetReduceId (cur_reg_exp);

	break;

    case TOKEN_DEF_n  :
	son_1 = sxson (visited, 1); /* <LEXICAL_UNIT_NAME> */
	/* Deja visite' par le pere */

	son_2 = son_1->brother; /* <TOKEN_BODY> */

	p = &(SXDS->reg_exp_to_attr [SXDS->current_reg_exp_or_abbrev_no]);

	if (son_2->name == COMPONENTS_S_n /* <UNION> */)
	{
	    p->init_item = SXDS->item_top;
	    re_maj_item_top ((SXINT)1);
	    master_reg_exp_no = SXDS->current_reg_exp_or_abbrev_no;

	    if (!reg_exps (son_2)) return false;
 	}
	else
	{
	    p->init_item = SXDS->item_top;
	    master_reg_exp_no = -1;

	    if (!regular_expression (son_2, &is_action)) return false;

	    if (is_action)
		return error ("illegal action usage in a regular expression", son_2), false;
	
	    set_new_item ((SXINT)KREDUCE, item_no, (SXINT)0, (SXINT)0, (SXBA)NULL, false);
	}

	break;
    default: /* pour faire taire gcc -Wswitch-default */
#if EBUG
      sxtrap("sxreg_exp_mngr","unknown switch case #6");
#endif
      break;
    }

    return true;
}


static bool
synonyms (struct reg_exp_node *visited, SXINT *prev_ste)
{
    struct reg_exp_node		*son_1;
    struct terminal_to_attr	*p;
    char			*name;
    SXINT				lgth, ste;

    /* Visite recursive des SYNONYMS */
    switch (visited->name) {
    case SYNONYM_S_n  :
    case DENOTATION_S_n  :
	for (son_1 = sxson (visited, 1); son_1 != NULL; son_1 = son_1->brother)
	    if (!synonyms (son_1, prev_ste)) return false;

	break;

    case ID_DENOTATION_n:
    case STRING_DENOTATION_n:
	if (visited->token.string_table_entry == SXERROR_STE)
	    return false;

	name = sxstrget (visited->token.string_table_entry);
	lgth = sxstrlen (visited->token.string_table_entry);

	if (visited->name == STRING_DENOTATION_n)
	    name++, lgth -= 2;

	ste = sxword_2save (&SXDS->terminals, name, (SXUINT) lgth);
	p = SXDS->terminal_to_attr + ste;

	p->token_no = -1;
	p->next_syno = -1;
	p->is_kw = true;
	p->is_generic = false;

	if (visited->position != 1)
	    SXDS->terminal_to_attr [*prev_ste].next_syno = ste;

	*prev_ste = ste;

	break;
    default: /* pour faire taire gcc -Wswitch-default */
#if EBUG
      sxtrap("sxreg_exp_mngr","unknown switch case #7");
#endif
      break;
    }

    return true;
}


void sxre_smp (SXINT sxre_smp_what, struct reg_exp_tables *sxtables_ptr)
{
    struct reg_exp_node	*son;
    SXINT		old_counters_size, prev_ste;
    bool		b_ret;

    sxuse(sxtables_ptr); /* pour faire taire gcc -Wunused */
    switch (sxre_smp_what) {
    case SXOPEN:
	sxatcvar.atc_lv.node_size = sizeof (struct reg_exp_node);
	break;

    case SXACTION:
	old_counters_size = SXDS->counters_size;

	if ( sxatcvar.atc_lv.abstract_tree_is_error_node ) {
	  SXDS->is_smp_error = true;
	} else {
	  b_ret =  ( ((son = sxatcvar.atc_lv.abstract_tree_root->son)->name == VOID_n || classes (son)) &&
		     ((son = son->brother)->name == VOID_n || abbreviations (son)) &&
		     ((son = son->brother)->name == VOID_n || reg_exps (son)) &&
		     ((son = son->brother)->name == VOID_n || synonyms (son, &prev_ste)) ) ;
	  if ( b_ret ) {
	    if ( (son = son->brother)->name != VOID_n ) {
	      error ("representation specification clause is not implemented", son);
	      b_ret = false;
	    }
	  }
	  SXDS->is_smp_error = !b_ret;
	}

	if (SXDS->counters_size > old_counters_size &&
	    SXDS->dslv != NULL &&
	    !SXDS->is_smp_error)
	{
	    /* Les variables locales des actions du scanner ont deja ete allouees et
	       le nombre de compteurs a augmente' => reallocation. */
	    SXINT i;

	    for (i = 0; i < SXDS->lv_size; i++)
	    {
		if (SXDS->dslv [i].counters == NULL)
		    break;

		SXDS->dslv [i].counters = (SXINT *) sxrealloc (SXDS->dslv [i].counters,
							      SXDS->counters_size + 1,
							      sizeof (SXINT));

		while (++old_counters_size <= SXDS->counters_size)
		    SXDS->dslv [i].counters [old_counters_size] = 0;
	    }
	}

	break;

    case SXCLOSE:
	break;

    default:
	fputs ("The function \"sxre_smp\" is out of date with respect to its specification.\n",
	       sxstderr);
	sxexit(1);
    }
}


SXINT
sxre_string_to_re (char *string, SXINT t_code, bool is_a_generic_terminal)
{
    /* string est cense etre un nom de terminal de la grammaire du niveau
       syntaxique.
       Traite les terminaux generiques, les mot-cles et les non generiques. */

    /* La forme normale des noms d'ER est capitalisee, les noms en "bnf" sont
       inchanges. */
    SXINT 			string_lgth, string_ste, ste, re_no, couple, old_size;
    char			*pc;
    struct reg_exp_to_attr	*preattr;
    struct item_to_attr		*pitattr;
    struct configs		*pconfigs;
    struct config		*pconfig;
    struct terminal_to_attr	*pteattr;

    string_lgth = strlen (string);

    string_ste = sxword_2save (&SXDS->terminals, string, (SXUINT) string_lgth);

    pteattr = SXDS->terminal_to_attr + string_ste;

    pteattr->token_no = t_code;
    pteattr->is_generic = is_a_generic_terminal;
    pteattr->is_kw = false; /* a priori */

    if (SXDS->token_no_to_terminal_size == 0)
    {
	SXDS->token_no_to_terminal_size = t_code < 100 ? 100 : t_code + 100;
	SXDS->token_no_to_terminal =
	    (SXINT*) sxalloc (SXDS->token_no_to_terminal_size, sizeof (SXINT));
	SXDS->is_token_no_to_terminal_static = false;
    }
    else
	if ((old_size = SXDS->token_no_to_terminal_size) <= t_code)
	{
	    SXDS->token_no_to_terminal_size = t_code + 100;

	    if (SXDS->is_token_no_to_terminal_static)
	    {
		SXINT	*old = SXDS->token_no_to_terminal;

		SXDS->token_no_to_terminal =
		    (SXINT*) sxalloc (SXDS->token_no_to_terminal_size, sizeof (SXINT));

		memcpy (SXDS->token_no_to_terminal, old, sizeof (SXINT) * old_size);
		SXDS->is_token_no_to_terminal_static = false;
	    }
	    else
		SXDS->token_no_to_terminal =
		    (SXINT*) sxrealloc (SXDS->token_no_to_terminal,
				      SXDS->token_no_to_terminal_size,
				      sizeof (SXINT));
	}
	
    SXDS->token_no_to_terminal [t_code] = string_ste;

    if (SXDS->ws == NULL)
	SXDS->ws = (char*) sxalloc ((SXDS->ws_lgth = string_lgth + 32),
				    sizeof (char));
    else if (SXDS->ws_lgth < string_lgth)
	SXDS->ws = (char*) sxrealloc (SXDS->ws,
				      (SXDS->ws_lgth = string_lgth + 32),
				      sizeof (char));
    memcpy (SXDS->ws + 1, string, (size_t) string_lgth);
    SXDS->ws [0] = '"';
    SXDS->ws [string_lgth + 1] = '"';

    if ((ste = sxword_2retrieve (&SXDS->reg_exp_names, SXDS->ws, (SXUINT) string_lgth + 2)) != SXERROR_STE)
    {
	preattr = SXDS->reg_exp_to_attr + ste;
	preattr->token_no = t_code;

	if (preattr->init_item >= 0)
	    /* Definition complete, on (re)calcule le "reduce_id". */
	    preattr->reduce_id = GetReduceId (ste);

	return string_ste;
    }

    for (pc = string + string_lgth; pc != string; pc--)
	*pc = sxtoupper (*pc)	/* passage en majuscules */ ;

    if ((ste = sxword_2retrieve (&SXDS->reg_exp_names, SXDS->ws + 1, (SXUINT) string_lgth)) != SXERROR_STE)
    {
	preattr = SXDS->reg_exp_to_attr + ste;
	preattr->token_no = t_code;

	if (preattr->init_item >= 0)
	    /* Definition complete, on (re)calcule le "reduce_id". */
	    preattr->reduce_id = GetReduceId (ste);

	return string_ste;
    }

    /* Les conflits de terminaux dont les noms different uniquement sur la capitalisation
       ne sont pas pris en compte!. */

    if (is_a_generic_terminal)
	return SXERROR_STE;		/* Doit exister */

    /* On regarde si "string" est reconnu par une des ER. */
    /* On commence par eliminer de l'espace de recherche les ER qui ont eu
       l'environement "NOT KEYWORD [ALL]". */
    sxba_empty (SXDS->ctxt_set);

    for (ste = SXWORD_top (SXDS->reg_exp_names) - 1; ste > SXEMPTY_STE; ste--)
    {
	if ((SXDS->reg_exp_to_attr [ste].kw_kind & KW_PROHIB) == 0)
	    SXBA_1_bit (SXDS->ctxt_set, ste);
    }

    pconfigs = sxds_string_to_token (string, true);

    if (pconfigs->conflict_kind & FirstReduce)
    {
	/* Mot-cle' */
	pteattr->is_kw = true;

	/* On met a jour la liste des synonymes. */
	while ((ste = pteattr->next_syno) != -1)
	{
	    pteattr = SXDS->terminal_to_attr + ste;
	    pteattr->token_no = t_code;
	}

	ForeachConfig (pconfigs, pconfig)
	{
	    /* 17/03/2003 Le if ci-dessous est mis ds le ForeachConfig, mais je n'ai pas cherche
	       a comprendre (Merci Mr alpha, pconfig etait non initialise a l'exterieur du ForeachConfig) */
	    if (SXDS->dslv [pconfig->current.lvno].ts_lgth != string_lgth ||
		strncmp (string, SXDS->dslv [pconfig->current.lvno].token_string, (size_t) string_lgth) != 0)
	    {
		/* La reconnaissance de la chaine l'a changee... */
		ste = sxword_2save (&SXDS->terminals,
				    SXDS->dslv [pconfig->current.lvno].token_string,
				    (SXUINT) (SXDS->dslv [pconfig->current.lvno].ts_lgth));

		pteattr = SXDS->terminal_to_attr + ste;

		pteattr->token_no = t_code;
		pteattr->next_syno = -1;
		pteattr->is_generic = is_a_generic_terminal; /* false!! */
		pteattr->is_kw = true;
	    }

	    if (SXDS->item_to_attr [pconfig->current.state].kind == KREDUCE)
	    {
		re_no = SXDS->item_to_attr [pconfig->current.state].reg_exp_no;
		preattr = SXDS->reg_exp_to_attr + re_no;
		preattr->kw_kind = KW_YES;

		if (preattr->init_item >= 0)
		    /* Definition complete, on (re)calcule le "reduce_id". */
		    preattr->reduce_id = GetReduceId (ste);
	    }
	}

	return string_ste;		/* ? */
    }

    /* On genere l'er STRING = -("s" "t" "r" "i" "n" "g"); */

    re_no = sxword_2save (&SXDS->reg_exp_names, string, (SXUINT) string_lgth);

    preattr = SXDS->reg_exp_to_attr + re_no;

    preattr->init_item = SXDS->item_top;
    preattr->post_action = -1;
    preattr->token_no = t_code;
    preattr->reduce_id = ++SXDS->current_reduce_id;

    preattr->context_kind = ContextAll;
    preattr->priority_kind = NoPriority;
    preattr->kw_kind = KW_NO;
    
    SXBA_1_bit (SXDS->filter, re_no);

    pitattr = SXDS->item_to_attr + SXDS->item_top++;

    pitattr->reg_exp_no = re_no;
    pitattr->val = 0;
    pitattr->param = 0;
    pitattr->is_erased = false;
    pitattr->kind = KEMPTY;

    XxY_set (&SXDS->nfsa, SXDS->item_top - 1, SXDS->item_top, &couple);

    for (pc = string; pc != string + string_lgth; pc++)
    {
	pitattr = SXDS->item_to_attr + SXDS->item_top++;

	pitattr->reg_exp_no = re_no;
	pitattr->val = char_sets_incr ();
	SXBA_1_bit (SXDS->char_sets [pitattr->val], *pc);
	pitattr->param = 0;
	pitattr->is_erased = true;
	pitattr->kind = KCLASS;

	XxY_set (&SXDS->nfsa, SXDS->item_top - 1, SXDS->item_top, &couple);
    }

    pitattr = SXDS->item_to_attr + SXDS->item_top++;

    pitattr->reg_exp_no = re_no;
    pitattr->val = 0;
    pitattr->param = 0;
    pitattr->is_erased = false;
    pitattr->kind = KREDUCE;

    return string_ste;
}


bool
sxre_write (sxfiledesc_t file_descr /* file descriptor */)
{
#define WRITE(p,l)	((bytes=(l))>0&&(write(file_descr, p, (size_t) bytes) == bytes))
  SXINT	bytes;  

  return (sxword_write (&(SXDS->sc_names), file_descr)
	  && WRITE (SXDS->sc_names_to_char_sets, sizeof (SXINT) * (SXWORD_top (SXDS->sc_names)))

	  && WRITE (&SXDS->char_sets_top, sizeof (SXINT))
	  && WRITE (&SXDS->char_sets_size, sizeof (SXINT))
	  && sxbm_write (file_descr, SXDS->char_sets, SXDS->char_sets_top + 1)

	  && sxword_write (&(SXDS->abbrev_names), file_descr)
	  && WRITE (SXDS->abbrev_to_attr, sizeof (struct abbrev_to_attr) * 
		    (SXWORD_top (SXDS->abbrev_names)))

	  && sxword_write (&(SXDS->reg_exp_names), file_descr)
	  && WRITE (SXDS->reg_exp_to_attr, sizeof (struct reg_exp_to_attr) * 
		    (SXWORD_top (SXDS->reg_exp_names)))
	  && WRITE (&SXDS->current_reg_exp_or_abbrev_no, sizeof (SXINT))
	  && WRITE (&SXDS->current_reduce_id, sizeof (SXINT))
	  && WRITE (&SXDS->delta, sizeof (SXINT))
	  && sxbm_write (file_descr, SXDS->context_sets, (SXINT) SXWORD_top (SXDS->reg_exp_names))	  

	  && WRITE (&SXDS->item_size, sizeof (SXINT))
	  && WRITE (&SXDS->item_top, sizeof (SXINT))
	  && WRITE (SXDS->item_to_attr, sizeof (struct item_to_attr) * (SXDS->item_top))

	  && XxY_write (&SXDS->nfsa, file_descr)

	  && sxword_write (&(SXDS->terminals), file_descr)
	  && WRITE (SXDS->terminal_to_attr,
		    sizeof (struct terminal_to_attr) * (SXWORD_top (SXDS->terminals)))

	  && WRITE (&SXDS->token_no_to_terminal_size, sizeof (SXINT))
	  && (SXDS->token_no_to_terminal_size > 0
	      ? WRITE (SXDS->token_no_to_terminal, sizeof (SXINT) * SXDS->token_no_to_terminal_size)
	      : true)

	  && WRITE (&SXDS->counters_size, sizeof (SXINT))
	  && WRITE (&SXDS->eof_code, sizeof (SXINT))
	  && WRITE (&SXDS->eof_reg_exp_name, sizeof (SXINT))

	  && WRITE (&SXDS->is_smp_error, sizeof (bool))
	  );
}


bool
sxre_read (sxfiledesc_t file_descr /* file descriptor */)
{
#define READ(p,l)	((bytes=(l))>0&&(read (file_descr, p, (size_t)bytes) == bytes)) 
  SXINT	        bytes;
  
  bool b_ret = (sxword_read (&(SXDS->sc_names), file_descr, "simple_class_names",
				sxcont_malloc, sxcont_alloc, sxcont_realloc, (sxresize_t) NULL, sxcont_free, reg_exp_pcn_oflw, NULL)
		   && READ (SXDS->sc_names_to_char_sets =
			    (SXINT*) sxalloc (SXWORD_size (SXDS->sc_names), sizeof (SXINT)),
			    sizeof (SXINT) * (SXWORD_top (SXDS->sc_names)))

		   && READ (&SXDS->char_sets_top, sizeof (SXINT))
		   && READ (&SXDS->char_sets_size, sizeof (SXINT))
		   && sxbm_read (file_descr,
				 SXDS->char_sets = sxbm_calloc (SXDS->char_sets_size, 1 /*EOF*/+256),
				 SXDS->char_sets_top + 1)
		     
		   && sxword_read (&(SXDS->abbrev_names), file_descr, "abbrev_names",
				   sxcont_malloc, sxcont_alloc, sxcont_realloc, (sxresize_t) NULL, sxcont_free, abbrev_names_oflw, NULL)
		   && READ (SXDS->abbrev_to_attr =
			    (struct abbrev_to_attr*) sxalloc (SXWORD_size (SXDS->abbrev_names),
							      sizeof (struct abbrev_to_attr)),
			    sizeof (struct abbrev_to_attr) * (SXWORD_top (SXDS->abbrev_names)))
		     
		   && sxword_read (&(SXDS->reg_exp_names), file_descr, "reg_exp_names",
				   sxcont_malloc, sxcont_alloc, sxcont_realloc, (sxresize_t) NULL, sxcont_free, reg_exp_names_oflw, NULL)
		   && READ (SXDS->reg_exp_to_attr =
			    (struct reg_exp_to_attr*) sxalloc (SXWORD_size (SXDS->reg_exp_names),
							       sizeof (struct reg_exp_to_attr)),
			    sizeof (struct reg_exp_to_attr) * (SXWORD_top (SXDS->reg_exp_names)))
		   && READ (&SXDS->current_reg_exp_or_abbrev_no, sizeof (SXINT))
		   && READ (&SXDS->current_reduce_id, sizeof (SXINT))
		   && READ (&SXDS->delta, sizeof (SXINT))
		   && sxbm_read (file_descr,
				 (SXBA*)(SXDS->context_sets = sxbm_calloc ((SXINT)SXWORD_size (SXDS->reg_exp_names),
									   (SXINT)SXWORD_size (SXDS->reg_exp_names))),
				 (SXINT)SXWORD_top (SXDS->reg_exp_names))	  

		   && (SXDS->ctxt_set = SXDS->context_sets [0])
		   && (SXDS->filter = SXDS->context_sets [1])

		   && (SXDS->tfirst = sxbm_calloc (SXDS->tfirst_line_nb = 64,
						   SXDS->tfirst_col_nb = 32))

		   && READ (&SXDS->item_size, sizeof (SXINT))
		   && READ (&SXDS->item_top, sizeof (SXINT))
		   && READ (SXDS->item_to_attr =
			    (struct item_to_attr*) sxalloc (SXDS->item_size,
							    sizeof(struct item_to_attr)),
			    sizeof (struct item_to_attr) * (SXDS->item_top))

		   && XxY_read (&SXDS->nfsa, file_descr, "nfsa", (sxoflw0_t) NULL, NULL)

		   && sxword_read (&(SXDS->terminals), file_descr, "terminals",
				   sxcont_malloc, sxcont_alloc, sxcont_realloc, (sxresize_t)NULL, sxcont_free, terminals_oflw, NULL)
		   && READ (SXDS->terminal_to_attr =
			    (struct terminal_to_attr*) sxalloc (SXWORD_size (SXDS->terminals),
								sizeof (SXINT)),
			    sizeof (struct terminal_to_attr) * (SXWORD_top (SXDS->terminals)))

		   && READ (&SXDS->token_no_to_terminal_size, sizeof (SXINT)) );
  if ( b_ret ) {
    if ( SXDS->token_no_to_terminal_size > 0 )
      b_ret = READ (SXDS->token_no_to_terminal =
		    (SXINT*) sxalloc (SXDS->token_no_to_terminal_size, sizeof (SXINT)),
		    sizeof (SXINT) * SXDS->token_no_to_terminal_size) ;
    else {
      SXDS->token_no_to_terminal =NULL;
    }
  }
  if ( b_ret ) {
    SXDS->ws_lgth = 0;
    SXDS->ws = NULL;
    b_ret = READ (&SXDS->counters_size, sizeof (SXINT))
         && READ (&SXDS->eof_code, sizeof (SXINT))
         && READ (&SXDS->eof_reg_exp_name, sizeof (SXINT))
         && READ (&SXDS->is_smp_error, sizeof (bool)) ;
  }
  return b_ret;
}


static void
re_out_tab_int (FILE *file, char *name1, char *name2, SXINT *tab, SXINT size, SXINT top)
{
    SXINT i;

    fprintf (file, "\n\nstatic SXINT %s%s [%ld] = {", name1, name2, (SXINT) size);

    for (i = 0; i < top; i++) {
	if ((i & 7) == 0)
	    fprintf (file, "\n/* %ld */ ", (SXINT) i);

	fprintf (file, "%ld, ", (SXINT) tab [i]);
    } 

    fprintf (file, "\n} /* End %s%s */;\n", name1, name2);
}

static void
re_out_tab_struct (FILE *file,
		   char *struct_name,
		   char *name1,
		   char *name2,
		   void (*print) (FILE*, SXINT),
		   SXINT size,
		   SXINT top)
{
    SXINT i;

    fprintf (file, "\n\nstatic %s %s%s [%ld] = {", struct_name, name1, name2, (SXINT) size + 1);

    for (i = 0; i < top; i++) {
	if ((i & 3) == 0)
	    fprintf (file, "\n/* %ld */ ", (SXINT) i);

	print (file, i);
    } 

    fprintf (file, "\n} /* End %s %s%s */;\n", struct_name, name1, name2);
}


static void
re_print_abbrev_to_attr (FILE *file, SXINT re_print_abbrev_to_attr_index)
{
    struct abbrev_to_attr	*p = SXDS->abbrev_to_attr + re_print_abbrev_to_attr_index;

    fprintf (file, "{%ld, %ld, %i}, ", (SXINT) p->init_item, (SXINT) p->final_item, p->is_empty);
}


static void
re_print_reg_exp_to_attr (FILE *file, SXINT re_print_reg_exp_to_attr_index)
{
    struct reg_exp_to_attr	*p = SXDS->reg_exp_to_attr + re_print_reg_exp_to_attr_index;

    fprintf (file, "{%ld, %ld, %ld, %ld, %i, %i, %i}, ", (SXINT) p->init_item, (SXINT) p->post_action, (SXINT) p->token_no, (SXINT) p->reduce_id, p->context_kind, p->priority_kind, p->kw_kind);
}

static void
re_print_item_to_attr (FILE *file, SXINT re_print_item_to_attr_index)
{
    struct item_to_attr	*p = SXDS->item_to_attr + re_print_item_to_attr_index;

    fprintf (file, "{%ld, %ld, %ld, %i, %ld}, ", (SXINT) p->reg_exp_no, (SXINT) p->val, (SXINT) p->param, p->is_erased, (SXINT) p->kind);
}

static void
re_print_terminal_to_attr (FILE *file, SXINT re_print_terminal_to_attr_index)
{
    struct terminal_to_attr	*p = SXDS->terminal_to_attr + re_print_terminal_to_attr_index;

    fprintf (file, "{%ld, %ld, %i, %i}, ", (SXINT) p->token_no, (SXINT) p->next_syno, p->is_kw, p->is_generic);
}

#define re_cat(s)	(vstr->current = vstr->first + prefix_lgth,	\
			 varstr_catenate (vstr, s),			\
			 varstr_tostr (vstr))

static SXUINT sc_names_total_lgth, abbrev_names_total_lgth, reg_exp_names_total_lgth, terminals_total_lgth;

void
sxre_array_to_c (FILE *file, char *name)
{
    SXINT prefix_lgth;

    if (vstr == NULL)
	vstr = varstr_alloc (64);
    else
	varstr_raz (vstr);

    varstr_catenate (vstr, name);
    prefix_lgth = varstr_length (vstr);

    sc_names_total_lgth = sxword_array_to_c (&SXDS->sc_names, file, re_cat ("_sc_names"));
    re_out_tab_int (file, name, "_sc_names_to_char_sets", SXDS->sc_names_to_char_sets,
		 SXWORD_size (SXDS->sc_names), SXWORD_top (SXDS->sc_names));
    sxbm_to_c (file, SXDS->char_sets, SXDS->char_sets_size, name, "_char_sets", true);
    abbrev_names_total_lgth = sxword_array_to_c (&SXDS->abbrev_names, file, re_cat ("_abbrev_names"));
    re_out_tab_struct (file, "struct abbrev_to_attr", name, "_abbrev_to_attr",
		    re_print_abbrev_to_attr, SXWORD_size (SXDS->abbrev_names),
		    SXWORD_top (SXDS->abbrev_names));
    reg_exp_names_total_lgth = sxword_array_to_c (&SXDS->reg_exp_names, file, re_cat ("_reg_exp_names"));
    re_out_tab_struct (file, "struct reg_exp_to_attr", name, "_reg_exp_to_attr",
		    re_print_reg_exp_to_attr, SXWORD_size (SXDS->reg_exp_names),
		    SXWORD_top (SXDS->reg_exp_names));
    sxbm_to_c (file, SXDS->context_sets, (SXINT) SXWORD_size (SXDS->reg_exp_names),
	       name, "_context_sets", true);
    re_out_tab_struct (file, "struct item_to_attr", name, "_item_to_attr",
		    re_print_item_to_attr, SXDS->item_size, SXDS->item_top + 1);
    XxY_array_to_c (&SXDS->nfsa, file, re_cat ("_nfsa"));
    terminals_total_lgth = sxword_array_to_c (&SXDS->terminals, file, re_cat ("_terminals"));
    re_out_tab_struct (file, "struct terminal_to_attr", name, "_terminal_to_attr",
		    re_print_terminal_to_attr,
		    SXWORD_size (SXDS->terminals), SXWORD_top (SXDS->terminals));

    if (SXDS->token_no_to_terminal_size > 0)
	re_out_tab_int (file, name, "_token_no_to_terminal", SXDS->token_no_to_terminal,
			SXDS->token_no_to_terminal_size, SXDS->token_no_to_terminal_size);

    varstr_free (vstr), vstr = NULL;
}


void
sxre_header_to_c (FILE *file, char *name)
{
    SXINT prefix_lgth;

    if (vstr == NULL)
	vstr = varstr_alloc (64);
    else
	varstr_raz (vstr);

    varstr_catenate (vstr, name);
    prefix_lgth = varstr_length (vstr);

    fprintf (file, "{\n");
    sxword_header_to_c (&(SXDS->sc_names), file, re_cat ("_sc_names"), sc_names_total_lgth);
    fprintf (file, ",\n");
    fprintf (file, "%s_sc_names_to_char_sets /* SXINT *sc_names_to_char_sets */,\n", name);
    fprintf (file, "%s_char_sets /* SXBA *char_sets */,\n", name);
    fprintf (file, "%ld /* SXINT char_sets_top */,\n", (SXINT) SXDS->char_sets_top);
    fprintf (file, "%ld /* SXINT char_sets_size */,\n", (SXINT) SXDS->char_sets_size);
    sxword_header_to_c (&(SXDS->abbrev_names), file, re_cat ("_abbrev_names"), abbrev_names_total_lgth);
    fprintf (file, ",\n");
    fprintf (file, "%s_abbrev_to_attr /* struct abbrev_to_attr *abbrev_to_attr */,\n", name);
    sxword_header_to_c (&(SXDS->reg_exp_names), file, re_cat ("_reg_exp_names"), reg_exp_names_total_lgth);
    fprintf (file, ",\n");
    fprintf (file, "%s_reg_exp_to_attr /* struct reg_exp_to_attr *reg_exp_to_attr */,\n", name);
    fprintf (file, "%ld /* SXINT current_reg_exp_or_abbrev_no */,\n",
	     (SXINT) SXDS->current_reg_exp_or_abbrev_no);
    fprintf (file, "%ld /* SXINT current_reduce_id */,\n", (SXINT) SXDS->current_reduce_id);
    fprintf (file, "%ld /* SXINT delta */,\n", (SXINT) SXDS->delta);
    fprintf (file, "%s_context_sets /* SXBA *context_sets */,\n", name);
    fprintf (file, "NULL /* SXBA *tfirst */,\n");
    fprintf (file, "0 /* SXINT tfirst_line_nb */,\n");
    fprintf (file, "0 /* SXINT tfirst_col_nb */,\n");
    fprintf (file, "%s_item_to_attr /* struct item_to_attr *item_to_attr */,\n", name);
    fprintf (file, "%ld /* SXINT item_size */,\n", (SXINT) SXDS->item_size);
    fprintf (file, "%ld /* SXINT item_top */,\n", (SXINT) SXDS->item_top);
    XxY_header_to_c (&SXDS->nfsa, file, re_cat ("_nfsa"));
    fprintf (file, ",\n");
    sxword_header_to_c (&(SXDS->terminals), file, re_cat ("_terminals"), terminals_total_lgth);
    fprintf (file, ",\n");
    fprintf (file, "%s_terminal_to_attr /* struct terminal_to_attr *terminal_to_attr */,\n", name);

    if (SXDS->token_no_to_terminal_size == 0)
	fprintf (file, "NULL /* SXINT *token_no_to_terminal */,\n");
    else
	fprintf (file, "%s_token_no_to_terminal /* SXINT *token_no_to_terminal */,\n", name);

    fprintf (file, "%ld /* SXINT token_no_to_terminal_size */,\n", (SXINT) SXDS->token_no_to_terminal_size);
    fprintf (file, "NULL /* char *ws */,\n");
    fprintf (file, "0 /* SXINT ws_lgth */,\n");
    fprintf (file, "%s_context_sets_line_0 /* SXBA ctxt_set */,\n", name);
    fprintf (file, "%s_context_sets_line_1 /* SXBA filter */,\n", name);
    fprintf (file, "%ld /* SXINT counters_size */,\n", (SXINT) SXDS->counters_size);
    fprintf (file, "%ld /* SXINT eof_code */,\n", (SXINT) SXDS->eof_code);
    fprintf (file, "%ld /* SXINT eof_reg_exp_name */,\n", (SXINT) SXDS->eof_reg_exp_name);    
    fprintf (file, "true /* is_char_sets_static */,\n");
    fprintf (file, "true /* is_item_to_attr_static */,\n");
    fprintf (file, "true /* is_token_no_to_terminal_static */,\n");
    fprintf (file, "%i /* bool is_smp_error */,\n", SXDS->is_smp_error);

    fprintf (file, "/* ... */\n");

    fprintf (file, "}");
    varstr_free (vstr), vstr = NULL;
}


void
sxre_to_c (FILE *file, char *name, bool is_static)
{
    sxre_array_to_c (file, name);

    fprintf (file, "\n\n%sstruct sxdynam_scanner %s = ", is_static ? "static " : "", name);

    sxre_header_to_c (file, name);

    fprintf (file, " /* struct sxdynam_scanner %s */;\n", name);

    fprintf (file, ";\n");
}


void
sxre_reuse (void)
{
    sxword_reuse (&SXDS->sc_names, "simple_class_names",
		  malloc, calloc, realloc, (sxresize_t) NULL, free, reg_exp_pcn_oflw, NULL);

    sxword_reuse (&SXDS->abbrev_names, "abbrev_names",
		  malloc, calloc, realloc, (sxresize_t) NULL, free, abbrev_names_oflw, NULL);

    sxword_reuse (&SXDS->reg_exp_names, "reg_exp_names",
		  malloc, calloc, realloc, (sxresize_t) NULL, free, reg_exp_names_oflw, NULL);

    XxY_reuse (&SXDS->nfsa, "nfsa", (sxoflw0_t) NULL, NULL);

    sxword_reuse (&SXDS->terminals, "terminals",
		  malloc, calloc, realloc, (sxresize_t) NULL, free, terminals_oflw, NULL);

}
