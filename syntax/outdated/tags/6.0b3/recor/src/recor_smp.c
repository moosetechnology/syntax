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
   *                                                      *
   ******************************************************** */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030516 13:07 (phd):	Suppression de #ifdef MSDOS		*/
/************************************************************************/
/* 22-10-93 10:00 (pb):		Adaptation a OSF			*/
/************************************************************************/
/* 20-05-92 16:10 (phd):	Declaration de options_text		*/
/*				Correction de l'ecriture de la date	*/
/************************************************************************/
/* 20-05-92 16:10 (phd):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/

/******************************************************************/
/*                                                                */
/*                                                                */
/*  This program has been translated from recor.semantic_pass.pl1 */
/*  on Thursday the thirty-first of July, 1986, at 10:45:27,      */
/*  on the Multics system at INRIA,                               */
/*  by the release 3.3i PL1_C translator of INRIA,                */
/*         developped by the "Langages et Traducteurs" team,      */
/*         using the SYNTAX (*), FNC and Paradis systems.         */
/*                                                                */
/*                                                                */
/******************************************************************/
/* (*) SYNTAX is a trademark of INRIA.                            */
/******************************************************************/




#define NODE struct recor_node

/*   I N C L U D E S   */
#include "sxunix.h"
#include "put_edit.h"
#include "B_tables.h"
#include "S_tables.h"
#include "R_tables.h"

#include "recor_node.h"
char WHAT_RECORSMP[] = "@(#)SYNTAX - $Id: recor_smp.c 1116 2008-02-28 15:53:33Z rlacroix $" WHAT_DEBUG;



/*
N O D E   N A M E S
*/
#define ERROR_n 1
#define C_PARAM_n 2
#define C_STRING_n 3
#define DIGIT_n 4
#define DONT_DELETE_n 5
#define DONT_INSERT_n 6
#define FOLLOWERS_NUMBER_n 7
#define GLOBAL_PARSER_n 8
#define GLOBAL_SCANNER_n 9
#define ITEM_s_n 10
#define KEY_TERMINAL_S_n 11
#define LOCAL_PARSER_n 12
#define LOCAL_SCANNER_n 13
#define MODEL_n 14
#define MODEL_s_n 15
#define NUMBER_n 16
#define OCTAL_CODE_n 17
#define PARAM_n 18
#define PARAM_s_n 19
#define PARSER_n 20
#define RECOR_n 21
#define S_n 22
#define SCANNER_n 23
#define SLICE_n 24
#define STRING_n 25
#define S_PARAM_n 26
#define TITLE_s_n 27
#define TUNING_SETS_n 28
#define VALIDATION_LENGTH_n 29
#define VOID_n 30
#define X_n 31
/*
E N D   N O D E   N A M E S
*/


/*   E X T E R N S   */

extern BOOLEAN	recor_write (struct R_tables_s *R_tables, char *langname);
extern VOID	recor_free (struct R_tables_s *R_tables);
extern VOID     lecl_free (struct lecl_tables_s *lecl_tables_ptr);
extern BOOLEAN  lecl_read (struct lecl_tables_s *lecl_tables_ptr, char *langname);
extern BOOLEAN	is_source;
extern char	by_mess [], release_mess [], *prgentname;

/*   L O C A L    D A T A   */

static char	**err_titles;
static struct bnf_ag_item	bnf_ag;
static struct lecl_tables_s	lecl_tables;
static struct R_tables_s	R_tables;
static SXINT	maxlregle, xstring_mess, nbcart, lstring_mess, maxparam, maxglobal_mess, xparam, nmax, xregle;
static BOOLEAN	is_scanner;
static SXINT	**lregle;
static char	*string_mess /* lstring_mess */ ;
static struct local_mess	*local_mess;
static struct global_mess	*global_mess;




/*   P R O C E D U R E S   */

static VOID	header (void)
{
    put_edit_nnl (9, "Listing Of:");
    put_edit_nnl (25, sxsrcmngr.source_coord.file_name);
    put_edit_nnl (9, "By:");
    put_edit (25, by_mess);
    put_edit (25, release_mess);

    {
	extern char	*options_text (char *line);
	char	line [132];

	put_edit_nnl (9, "Options:");
	put_edit (25, options_text (line));
    }

    {
	long	date_time;

	put_edit_nnl (9, "Generated on:");
	date_time = time (0);
	put_edit (25, ctime (&date_time));
    }

    put_edit_nl (2);
}


#include "varstr.h"

static VOID	listing_output (void)
{
    FILE	*listing;
    VARSTR      vstr;
    char	*lst_name;

    if (sxverbosep)
	fputs ("Listing Output\n", sxtty);

    vstr = varstr_alloc (32);

    vstr = varstr_catenate (vstr, prgentname);
    vstr = varstr_catenate (vstr, ".rc.l");
    lst_name = varstr_tostr (vstr);

    if ((listing = sxfopen (lst_name, "w")) == NULL) {
      fprintf (sxstderr, "listing_output: cannot open (create) ");
      sxperror (lst_name);
      return;
    }

    varstr_free (vstr);

    put_edit_initialize (listing);
    header ();
    sxlisting_output (listing);
    put_edit_finalize ();
}



static VOID	get_t_code (struct recor_node *node_ptr, SXBA no)
{
    /* Positionne dans "no" le terminal repere par ate s'il existe */
    SXINT	t, x, sl, tl;
    struct sxsource_coord	designator;
    char	*s;

    designator = node_ptr->token.source_index;

    if (node_ptr->name != C_STRING_n) {
	sxput_error (designator, "%sOnly terminal symbols are allowed.", err_titles [2]);
	return;
    }

    {
	SXINT	ate;

	s = sxstrget (ate = node_ptr->token.string_table_entry);
	sl = sxstrlen (ate) + 1;
    }

    for (t = -1; t >= bnf_ag.WS_TBL_SIZE.tmax; t--) {
	x = bnf_ag.ADR [t];
	tl = bnf_ag.ADR [t - 1] - x;

	if (sl == tl && strcmp (bnf_ag.T_STRING + x, s) == 0) {
	    sxba_1_bit (no, -t);
	    return;
	}
    }

    sxput_error (designator, "%sThis unknown terminal symbol is ignored.", err_titles [2]);
}



static VOID	set_code (SXBA no, SXINT ic, struct sxsource_coord designator)
{
    if (lecl_tables.S_char_to_simple_class [ic] == 1)
	sxput_error (designator, "%sThe illegal character \"%s\" is ignored.", err_titles [2], CHAR_TO_STRING (ic));
    else
	sxba_1_bit (no, ic + 1);
}



static VOID	process (SXBA no, struct recor_node *node_ptr, SXINT *bi, SXINT *bs)
{
    SXINT	sl, i;
    struct sxsource_coord	designator;
    char	*s;

    {
	SXINT	ate;

	s = sxstrget (ate = node_ptr->token.string_table_entry);
	sl = sxstrlen (ate);
    }

    designator = node_ptr->token.source_index;

    if (node_ptr->name == C_STRING_n) {
	SXINT	ic;

	for (i = 0; i < sl; i++) {
	    ic = *(s + i);

	    if (i == 0)
		*bi = ic;

	    if (i == sl - 1)
		*bs = ic;

	    designator.column += i;
	    set_code (no, ic, designator);
	}
    }
    else {
	/* OCTAL_CODE */
	SXINT	val = 0;

	for (i = 0; i < sl; i++) {
	    val <<= 3;
	    val += *(s + i) - '0';
	}

	if (val > lecl_tables.S_tbl_size.S_last_char_code) {
	    sxput_error (designator, "%sThis value cannot exceed %d.", err_titles [2], lecl_tables.S_tbl_size.
		 S_last_char_code);
	    val = -1;
	}

	*bs = *bi = val;

	if (val >= 0)
	    set_code (no, *bi, designator);
    }
}



static VOID	set_classes (struct recor_node *node_ptr, SXBA no)
{
    /* Remplit "no" avec les classes de caracteres reperes par ate */
    struct recor_node	*son;
    struct sxsource_coord	designator;
    SXINT	ic;
    SXINT		bi, bs, binf, bsup;

    if (node_ptr->name == SLICE_n) {
	son = node_ptr->son;
	designator = son->token.source_index;
	process (no, son, &bi, &bs);

	if (bs >= 0) {
	    binf = bs;
	    process (no, son->brother, &bsup, &bs);

	    if (bsup >= 0) {
		if (binf > bsup)
		    sxput_error (designator, "%sErroneous slice specification.", err_titles [2]);
		else
		    for (ic = binf + 1; ic < bsup; ic++) {
			set_code (no, ic, designator);
		    }
	    }
	}
    }
    else
	process (no, node_ptr, &bi, &bs);
}




static VOID	pass_inherited (void)
{
    struct recor_node	*visited = VISITED;
    struct recor_node	*node_ptr;


#ifdef DEBUG

    printf ("PI: father_node = %d, visited_node = %d\n", (visited->father)->name, visited->name);


#endif


/*
I N H E R I T E D
*/

    switch ((visited->father)->name) {
    case ERROR_n:
	return;

    case DONT_DELETE_n:
	/* VISITED->name = {C_STRING_n, OCTAL_CODE_n, SLICE_n} */
      if (!visited->not_is_first_visit) {
	    if (is_scanner)
		set_classes (VISITED, R_tables.S_no_delete);
	    else
		get_t_code (VISITED, R_tables.P_no_delete);
      }
	return;

    case DONT_INSERT_n:
	/* VISITED->name = {C_STRING_n, OCTAL_CODE_n, SLICE_n} */
      if (!visited->not_is_first_visit) {
	    if (is_scanner) 
		set_classes (VISITED, R_tables.S_no_insert);
	    else
		get_t_code (VISITED, R_tables.P_no_insert);
      }
	return;

    case FOLLOWERS_NUMBER_n:
	switch (visited->position) {
	    SXINT	l;

	case 1:
	    /* VISITED->name = NUMBER_n */
	    if (visited->not_is_first_visit)
		R_tables.R_tbl_size.P_followers_number = atoi (sxstrget (visited->token.string_table_entry));

	    return;

	case 2:
	    /* VISITED->name = STRING_n */
	    l = sxstrlen (visited->token.string_table_entry);

	    if (!visited->not_is_first_visit)
		lstring_mess += l;
	    else {
		global_mess [1].index = xstring_mess;
		global_mess [1].length = l;
		strcpy (string_mess + xstring_mess, sxstrget (visited->token.string_table_entry));
		xstring_mess += l;
	    }

	    return;
	default:
#if EBUG
	  sxtrap("recor_smp","unknown switch case #1");
#endif
	  break;
	}

    case GLOBAL_PARSER_n:
	node_ptr = visited;
	
	switch (visited->position) {
	    SXINT	l;

	case 1:
	    /* VISITED->name = {KEY_TERMINAL_S_n, VOID_n} */
	case 2:
	    /* VISITED->name = VALIDATION_LENGTH_n */
	case 3:
	    /* VISITED->name = FOLLOWERS_NUMBER_n */
	    return;

	case 6 :/* VISITED->name = {STRING_n, VOID_n} */
		/* [EOF] */
	    if (visited->name == VOID_n)
		node_ptr = sxson (sxson (sxson (visited->father->father->father, 2), 2), 4);
		/* On prend la specif du scanner : recor.scanner.global_scanner.eof */

	case 4:
	    /* VISITED->name = STRING_n */
	    /* Detection */
	case 5:
	    /* VISITED->name = STRING_n */
	    /* Restarting */
	case 7:
	    /* VISITED->name = STRING_n */
	    /* Halt */
	    l = sxstrlen (node_ptr->token.string_table_entry);

	    if (!visited->not_is_first_visit)
		lstring_mess += l;
	    else {
		global_mess [visited->position - 2].index = xstring_mess;
		global_mess [visited->position - 2].length = l;
		strcpy (string_mess + xstring_mess, sxstrget (node_ptr->token.string_table_entry));
		xstring_mess += l;
	    }

	    return;
	default:
#if EBUG
	  sxtrap("recor_smp","unknown switch case #2");
#endif
	  break;
	}

    case GLOBAL_SCANNER_n:
	/* VISITED->name = STRING_n */
	{
	    SXINT	l;

	    l = sxstrlen (visited->token.string_table_entry);

	    if (!visited->not_is_first_visit)
		lstring_mess += l;
	    else {
		global_mess [visited->position].index = xstring_mess;
		global_mess [visited->position].length = l;
		strcpy (string_mess + xstring_mess, sxstrget (visited->token.string_table_entry));
		xstring_mess += l;
	    }
	}

	return;

    case ITEM_s_n:
	/* VISITED->name = {DIGIT_n, S_n, X_n} */
	return;

    case KEY_TERMINAL_S_n:
	/* VISITED->name = C_STRING_n */
	if (!visited->not_is_first_visit)
	    get_t_code (VISITED, R_tables.PER_tset);

	return;

    case LOCAL_PARSER_n:
	switch (visited->position) {
	case 1:
	    /* VISITED->name = MODEL_s_n */
	    if (!visited->not_is_first_visit) {
		nbcart = visited->degree;
		R_tables.P_no_delete = sxba_calloc (R_tables.R_tbl_size.P_tmax + 1);
		R_tables.P_no_insert = sxba_calloc (R_tables.R_tbl_size.P_tmax + 1);
	    }
	case 2:
	    /* VISITED->name = TUNING_SETS_n */
	    return;
	default:
#if EBUG
	  sxtrap("recor_smp","unknown switch case #3");
#endif
	  break;
	}

    case LOCAL_SCANNER_n:
	switch (visited->position) {
	case 1:
	    /* VISITED->name = MODEL_s_n */
	    if (!visited->not_is_first_visit) {
		nbcart = visited->degree;
		R_tables.S_no_delete = sxba_calloc (R_tables.R_tbl_size.S_last_char_code + 1);
		R_tables.S_no_insert = sxba_calloc (R_tables.R_tbl_size.S_last_char_code + 1);
	    }
	case 2:
	    /* VISITED->name = TUNING_SETS_n */
	    return;
	default:
#if EBUG
	  sxtrap("recor_smp","unknown switch case #4");
#endif
	  break;
	}

    case MODEL_n:
	switch (visited->position) {
	case 1:
	    /* VISITED->name = ITEM_s_n */
	    if (!visited->not_is_first_visit)
		maxlregle = (maxlregle > visited->degree) ? maxlregle : visited->degree;

	    return;

	case 2:
	    /* VISITED->name = PARAM_s_n */
	    if (!visited->not_is_first_visit)
		maxparam = (maxparam > visited->degree - 1) ? maxparam : visited->degree - 1;

	    return;
	default:
#if EBUG
	  sxtrap("recor_smp","unknown switch case #5");
#endif
	  break;
	}

    case MODEL_s_n:
	/* VISITED->name = MODEL_n */
	xregle = visited->position;
	return;

    case PARAM_n:
	switch (visited->position) {
	    SXINT	l;

	case 1:
	    /* VISITED->name = {C_PARAM_n, S_PARAM_n, VOID_n} */
	    return;

	case 2:
	    /* VISITED->name = STRING_n */
	    l = sxstrlen (visited->token.string_table_entry);

	    if (!visited->not_is_first_visit)
		lstring_mess += l;
	    else {
		local_mess [xregle].string_ref [xparam].index = xstring_mess;
		local_mess [xregle].string_ref [xparam].length = l;
		strcpy (string_mess + xstring_mess, sxstrget (visited->token.string_table_entry));
		xstring_mess += l;
	    }

	    return;
	default:
#if EBUG
	  sxtrap("recor_smp","unknown switch case #6");
#endif
	  break;
	}

    case PARAM_s_n:
	/* VISITED->name = PARAM_n */
	xparam = visited->position - 1;
	return;

    case PARSER_n:
	switch (visited->position) {
	case 1:
	    /* VISITED->name = {LOCAL_PARSER_n, VOID_n} */
	    is_scanner = FALSE;

	    if (!visited->not_is_first_visit) {
		nbcart = 0;
		maxparam = maxlregle = 0;
		lstring_mess = 1; /* compte du \0 */
	    }

	    return;

	case 2:
	    /* VISITED->name = PARAM_s_n */
	    if (!visited->not_is_first_visit)
		maxparam = (maxparam > visited->degree - 1) ? maxparam : visited->degree - 1;

	    xregle = nbcart + 1;

	case 3:
	    /* VISITED->name = GLOBAL_PARSER_n */
	    return;
	default:
#if EBUG
	  sxtrap("recor_smp","unknown switch case #7");
#endif
	  break;
	}

    case RECOR_n:
	switch (visited->position) {
	case 1:
	    /* VISITED->name = TITLE_s_n */
	    if (!visited->not_is_first_visit) {
		R_tables.R_tbl_size.E_nb_kind = visited->degree;
		R_tables.R_tbl_size.E_maxltitles = 0;
	    }

	case 2:
	    /* VISITED->name = SCANNER_n */
	case 3:
	    /* VISITED->name = PARSER_n */
	    return;

	case 4:
	    /* VISITED->name = {STRING_n, VOID_n} */
	    if (visited->name == VOID_n) {
		R_tables.R_tbl_size.E_labstract = 0;
		R_tables.E_abstract = NULL;
	    }
	    else {
		R_tables.R_tbl_size.E_labstract = sxstrlen (visited->token.string_table_entry);
		R_tables.E_abstract = (char*) sxalloc (R_tables.R_tbl_size.E_labstract + 1, sizeof (char));
		strcpy (R_tables.E_abstract, sxstrget (visited->token.string_table_entry));
	    }

	    return;
	default:
#if EBUG
	  sxtrap("recor_smp","unknown switch case #8");
#endif
	  break;
	}

    case SCANNER_n:
	switch (VISITED->position) {
	case 1 :/* VISITED->name = {LOCAL_SCANNER_n, VOID_n} */
	    is_scanner = TRUE;

	    if (!visited->not_is_first_visit) {
		nbcart = 0;
		maxparam = maxlregle = 0;
		lstring_mess = 1; /* compte du \0 */
	    }

	case 2 :/* VISITED->name = GLOBAL_SCANNER_n */
	return;
	default:
#if EBUG
	  sxtrap("recor_smp","unknown switch case #9");
#endif
	  break;

	}

    case SLICE_n:
	/* VISITED->name = {C_STRING_n, OCTAL_CODE_n} */
	return;

    case TITLE_s_n:
	/* VISITED->name = STRING_n */
	if (!visited->not_is_first_visit) {
	    SXINT	l;

	    R_tables.R_tbl_size.E_maxltitles = (R_tables.R_tbl_size.E_maxltitles > (l = sxstrlen (visited->token.
		 string_table_entry) + 1)) ? R_tables.R_tbl_size.E_maxltitles : l;
	}
	else
	    strcpy (R_tables.E_titles [visited->position], sxstrget (visited->token.string_table_entry));

	return;

    case TUNING_SETS_n:
	/* VISITED->name = {DONT_DELETE_n, DONT_INSERT_n, VOID_n} */
	return;


/*
Z Z Z Z
*/

    default:
	break;
    }
}




static VOID	pass_derived (void)
{
    struct recor_node	*visited = VISITED;


#ifdef DEBUG

    printf ("PD: visited_node = %d\n", visited->name);


#endif


/*
D E R I V E D
*/

    switch (visited->name) {
    case ERROR_n:
	break;

    case C_PARAM_n:
	if (visited->not_is_first_visit)
	    local_mess [xregle].string_ref [xparam].param = -atoi (sxstrget (visited->token.string_table_entry)) - 1;

	break;

    case C_STRING_n:
	break;

    case DIGIT_n:
	if (visited->not_is_first_visit) {
	    SXINT	i;

	    lregle [xregle] [visited->position] = i = atoi (sxstrget (visited->token.string_table_entry));

	    if (nmax < i)
		nmax = i;
	}

	break;

    case DONT_DELETE_n:
	break;

    case DONT_INSERT_n:
	break;

    case FOLLOWERS_NUMBER_n:
	break;

    case GLOBAL_PARSER_n:
	break;

    case GLOBAL_SCANNER_n:
	break;

    case ITEM_s_n:
	if (visited->not_is_first_visit)
	    lregle [xregle] [0] = visited->degree;

	break;

    case KEY_TERMINAL_S_n:
	break;

    case LOCAL_PARSER_n:
	break;

    case LOCAL_SCANNER_n:
	break;

    case MODEL_n:
	if (visited->not_is_first_visit)
	    lregle [xregle] [0] = sxson (VISITED, 1)->degree;

	break;

    case MODEL_s_n:
	break;

    case NUMBER_n:
	break;

    case OCTAL_CODE_n:
	break;

    case PARAM_n:
	break;

    case PARAM_s_n:
	if (visited->not_is_first_visit)
	    local_mess [xregle].param_no = visited->degree - 1;

	break;

    case PARSER_n:
	if (!visited->not_is_first_visit) {
	    SXINT	i;

	    R_tables.R_tbl_size.P_nbcart = nbcart;
	    R_tables.R_tbl_size.P_maxlregle = maxlregle;
	    R_tables.R_tbl_size.P_maxparam = maxparam;
	    R_tables.R_tbl_size.P_lstring_mess = lstring_mess;
	    nmax = 0;
	    maxglobal_mess = 5;
	    lregle = R_tables.P_lregle = (nbcart == 0) ? NULL : (SXINT**) sxalloc (nbcart + 1, sizeof (SXINT*));

	    for (i = 1; i <= nbcart; i++)
		R_tables.P_lregle [i] = (SXINT*) sxcalloc (maxlregle + 1, sizeof (SXINT));

	    R_tables.P_right_ctxt_head = (nbcart == 0) ? NULL : (SXINT*) sxalloc (nbcart + 1, sizeof (SXINT));
	    local_mess = R_tables.SXP_local_mess = (struct local_mess*) sxalloc (nbcart + 2, sizeof (struct local_mess));

	    for (i = 1; i <= nbcart + 1; i++)
		R_tables.SXP_local_mess [i].string_ref = (struct string_ref*) sxalloc (maxparam + 1, sizeof (struct
		     string_ref));

	    string_mess = R_tables.P_string_mess = (char*) sxalloc (lstring_mess, sizeof (char));
	    global_mess = R_tables.P_global_mess = (struct global_mess*) sxalloc (maxglobal_mess + 1, sizeof (struct global_mess));
	    xstring_mess = 0;
	    sxat_snv (INHERITED, VISITED);
	}
	else {
	    R_tables.R_tbl_size.P_nmax = nmax;
	    R_tables.R_tbl_size.P_sizeofpts = nmax + maxlregle;
	}

	break;

    case RECOR_n:
	break;

    case S_n:
	if (visited->not_is_first_visit)
	    lregle [xregle] [visited->position] = -2;

	break;

    case SCANNER_n:
	if (!visited->not_is_first_visit) {
	    SXINT	i;

	    R_tables.R_tbl_size.S_nbcart = nbcart;
	    R_tables.R_tbl_size.S_maxlregle = maxlregle;
	    R_tables.R_tbl_size.S_maxparam = maxparam;
	    R_tables.R_tbl_size.S_lstring_mess = lstring_mess;
	    nmax = 0;
	    maxglobal_mess = 5;
	    lregle = R_tables.S_lregle = (nbcart == 0) ? NULL : (SXINT**) sxalloc (nbcart + 1, sizeof (SXINT*));

	    for (i = 1; i <= nbcart; i++)
		R_tables.S_lregle [i] = (SXINT*) sxcalloc (maxlregle + 1, sizeof (SXINT));

	    local_mess = R_tables.SXS_local_mess = (nbcart == 0) ? NULL : (struct local_mess*) sxalloc (nbcart + 1, sizeof (struct local_mess));

	    for (i = 1; i <= nbcart; i++)
		R_tables.SXS_local_mess [i].string_ref = (struct string_ref*) sxalloc (maxparam + 1, sizeof (struct
		     string_ref));

	    string_mess = R_tables.S_string_mess = (char*) sxalloc (lstring_mess, sizeof (char));
	    global_mess = R_tables.S_global_mess = (struct global_mess*) sxalloc (maxglobal_mess + 1, sizeof (struct global_mess));
	    xstring_mess = 0;
	    sxat_snv (INHERITED, VISITED);
	}
	else
	    R_tables.R_tbl_size.S_nmax = nmax;

	break;

    case SLICE_n:
	break;

    case STRING_n:
	break;

    case S_PARAM_n:
	if (visited->not_is_first_visit)
	    local_mess [xregle].string_ref [xparam].param = atoi (sxstrget (visited->token.string_table_entry));

	break;

    case TITLE_s_n:
	if (!visited->not_is_first_visit) {
	    SXINT	i;

	    R_tables.E_titles = (char**) sxalloc (R_tables.R_tbl_size.E_nb_kind + 1, sizeof (char*));

	    for (i = 1; i <= R_tables.R_tbl_size.E_nb_kind; i++)
		R_tables.E_titles [i] = (char*) sxalloc (R_tables.R_tbl_size.E_maxltitles + 1, sizeof (char));

	    sxat_snv (INHERITED, VISITED);
	}

	break;

    case TUNING_SETS_n:
	break;

    case VALIDATION_LENGTH_n:
	if (visited->not_is_first_visit)
	    R_tables.R_tbl_size.P_validation_length = atoi (sxstrget (VISITED->token.string_table_entry));

	break;

    case VOID_n:
	break;

    case X_n:
	if (visited->not_is_first_visit)
	    lregle [xregle] [visited->position] = -1;

	break;


/*
Z Z Z Z
*/

    default:
	break;
    }

    visited->not_is_first_visit = TRUE;
}




static VOID	smpopen (struct sxtables *sxtables_ptr)
{
    err_titles = sxtables_ptr->err_titles;
    sxatcvar.atc_lv.node_size = sizeof (struct recor_node);
}




static VOID	smppass (void)
{
    /*   I N I T I A L I S A T I O N S   */
    R_tables.E_abstract = NULL;
    R_tables.E_titles = NULL;
    R_tables.S_global_mess = NULL;
    R_tables.S_no_insert = NULL;
    R_tables.S_no_delete = NULL;
    R_tables.S_string_mess = NULL;
    R_tables.SXS_local_mess = NULL;
    R_tables.S_lregle = NULL;
    R_tables.PER_tset = NULL;
    R_tables.P_global_mess = NULL;
    R_tables.P_no_insert = NULL;
    R_tables.P_no_delete = NULL;
    R_tables.P_string_mess = NULL;
    R_tables.SXP_local_mess = NULL;
    R_tables.P_lregle = NULL;
    R_tables.P_right_ctxt_head = NULL;


/* Lecture des tables bnf et lecl */

    if (!bnf_read (&bnf_ag, prgentname))
	return;

    if (!lecl_read (&lecl_tables, prgentname))
	return;


/* Allocations initiales */

    R_tables.bnf_modif_time = bnf_ag.bnf_modif_time;
    R_tables.R_tbl_size.P_tmax = -bnf_ag.WS_TBL_SIZE.tmax;
    R_tables.R_tbl_size.S_last_char_code = lecl_tables.S_tbl_size.S_last_char_code;
    R_tables.PER_tset = sxba_calloc (R_tables.R_tbl_size.P_tmax + 1);

/*   A T T R I B U T E S    E V A L U A T I O N   */

    sxsmp (sxatcvar.atc_lv.abstract_tree_root, pass_inherited, pass_derived);
    R_tables.R_tbl_size.P_max_prio_X = R_tables.R_tbl_size.P_nbcart + 1;
    R_tables.R_tbl_size.P_min_prio_0 = 0;

    {
	SXINT	l, x, h;

	for (x = 1; x <= R_tables.R_tbl_size.P_nbcart; x++) {
	    if (R_tables.P_lregle [x] [1] == 0)
		R_tables.R_tbl_size.P_min_prio_0 = x;
	    else
		R_tables.R_tbl_size.P_max_prio_X = (R_tables.R_tbl_size.P_max_prio_X < x) ? R_tables.R_tbl_size.
		     P_max_prio_X : x;

	    l = R_tables.P_lregle [x] [0];
	    h = R_tables.P_lregle [x] [l];

	    while (--l > 0) {
		SXINT	i = R_tables.P_lregle [x] [l];

		if (i >= 0 && i == h - 1)
		    h--;
		else
		    break;
	    }

	    R_tables.P_right_ctxt_head [x] = (1 > h) ? 1 : h;
	}
    }

    lecl_free (&lecl_tables);
    bnf_free (&bnf_ag);

    if (!recor_write (&R_tables, prgentname))
	SXEXIT (2);

    recor_free (&R_tables);

    if (is_source) {
	listing_output ();
    }
}



VOID	recor_smp (SXINT what, struct sxtables *sxtables_ptr)
{
    switch (what) {
    case OPEN:
	smpopen (sxtables_ptr);
	break;

    case CLOSE:
	break;

    case ACTION:
	if (sxerrmngr.nbmess [2] == 0) {
	    if (sxverbosep)
		fputs ("Semantic Pass\n", sxtty);

	    smppass ();
	}
	else
	    fprintf (sxstderr, "recor: due to previous errors, the semantic pass is not performed.\n");

	break;

    default:
	fputs ("The function \"recor_smp\" is out of date with respect to its specification.\n", sxstderr);
	sxexit(1);
    }
}
