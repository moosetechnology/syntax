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

#define SXNODE struct recor_node

/*   I N C L U D E S   */
#include "sxversion.h"
#include "sxunix.h"
#include "put_edit.h"
#include "B_tables.h"
#include "S_tables.h"
#include "R_tables.h"

#include "recor_node.h"

char WHAT_RECORSMP[] = "@(#)SYNTAX - $Id: recor_smp.c 3601 2023-09-23 19:44:39Z garavel $" WHAT_DEBUG;

/*  N O D E   N A M E S  */
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
/*  E N D   N O D E   N A M E S  */

/*   E X T E R N S   */

extern SXBOOLEAN	recor_write (struct R_tables_s *R_tables, char *langname);
extern SXVOID	recor_free (struct R_tables_s *R_tables);
extern SXVOID     lecl_free (struct lecl_tables_s *lecl_tables_ptr);
extern SXBOOLEAN  lecl_read (struct lecl_tables_s *lecl_tables_ptr, char *langname);
extern SXBOOLEAN	is_source;
extern char	by_mess [], *prgentname;

/*   L O C A L    D A T A   */

static char	**err_titles;
static struct bnf_ag_item	bnf_ag;
static struct lecl_tables_s	lecl_tables;
static struct R_tables_s	R_tables;
static SXINT	maxlregle, xstring_mess, nbcart, lstring_mess, maxparam, maxglobal_mess, xparam, nmax, xregle;
static SXBOOLEAN	is_scanner;
static SXINT	**lregle;
static char	*string_mess /* lstring_mess */ ;
static struct local_mess	*local_mess;
static struct global_mess	*global_mess;




/*   P R O C E D U R E S   */

static SXVOID	header (void)
{
    put_edit_nnl (9, "Listing of:");
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
	time_t	date_time;

	put_edit_nnl (9, "Generated on:");
	date_time = time (0);
	put_edit (25, ctime (&date_time));
    }

    put_edit_nl (2);
}


#include "varstr.h"

static SXVOID	listing_output (void)
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



static SXVOID	get_t_code (struct recor_node *node_ptr, SXBA no)
{
    /* Positionne dans "no" le terminal repere par ate s'il existe */
    SXINT	t, x, sl, tl;
    struct sxsource_coord	designator;
    char	*s;

    designator = node_ptr->token.source_index;

    if (node_ptr->name != C_STRING_n) {
	sxerror (designator,
		 err_titles [2][0],
		 "%sOnly terminal symbols are allowed.",
		 err_titles [2]+1);
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

    sxerror (designator,
	     err_titles [2][0],
	     "%sThis unknown terminal symbol is ignored.",
	     err_titles [2]+1);
}



static SXVOID	set_code (SXBA no, SXINT ic, struct sxsource_coord designator)
{
    if (lecl_tables.S_char_to_simple_class [ic] == 1)
	sxerror (designator,
		 err_titles [2][0],
		 "%sThe illegal character \"%s\" is ignored.",
		 err_titles [2]+1,
		 SXCHAR_TO_STRING (ic));
    else
	sxba_1_bit (no, ic + 1);
}



static SXVOID	process (SXBA no, struct recor_node *node_ptr, SXINT *bi, SXINT *bs)
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
	    sxerror (designator,
		     err_titles [2][0],
		     "%sThis value cannot exceed %ld.",
		     err_titles [2]+1,
		     (SXINT) lecl_tables.S_tbl_size.S_last_char_code);
	    val = -1;
	}

	*bs = *bi = val;

	if (val >= 0)
	    set_code (no, *bi, designator);
    }
}



static SXVOID	set_classes (struct recor_node *node_ptr, SXBA no)
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
		    sxerror (designator,
			     err_titles [2][0],
			     "%sErroneous slice specification.",
			     err_titles [2]+1);
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



static SXVOID	pass_inherited (void)
{
    struct recor_node	*visited = SXVISITED;
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
	/* SXVISITED->name = {C_STRING_n, OCTAL_CODE_n, SLICE_n} */
      if (!visited->not_is_first_visit) {
	    if (is_scanner)
		set_classes (SXVISITED, R_tables.S_no_delete);
	    else
		get_t_code (SXVISITED, R_tables.P_no_delete);
      }
	return;

    case DONT_INSERT_n:
	/* SXVISITED->name = {C_STRING_n, OCTAL_CODE_n, SLICE_n} */
      if (!visited->not_is_first_visit) {
	    if (is_scanner) 
		set_classes (SXVISITED, R_tables.S_no_insert);
	    else
		get_t_code (SXVISITED, R_tables.P_no_insert);
      }
	return;

    case FOLLOWERS_NUMBER_n:
	switch (visited->position) {
	    SXINT	l;

	case 1:
	    /* SXVISITED->name = NUMBER_n */
	    if (visited->not_is_first_visit)
		R_tables.R_tbl_size.P_followers_number = atoi (sxstrget (visited->token.string_table_entry));

	    return;

	case 2:
	    /* SXVISITED->name = STRING_n */
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
        return; /* added by Hubert Garavel and Wendelin Serwe */

    case GLOBAL_PARSER_n:
	node_ptr = visited;
	
	switch (visited->position) {
	    SXINT	l;

	case 1:
	    /* SXVISITED->name = {KEY_TERMINAL_S_n, VOID_n} */
	case 2:
	    /* SXVISITED->name = VALIDATION_LENGTH_n */
	case 3:
	    /* SXVISITED->name = FOLLOWERS_NUMBER_n */
	    return;

	case 6 :/* SXVISITED->name = {STRING_n, VOID_n} */
		/* [EOF] */
	    if (visited->name == VOID_n)
		node_ptr = sxson (sxson (sxson (visited->father->father->father, 2), 2), 4);
		/* On prend la specif du scanner : recor.scanner.global_scanner.eof */
#if defined (__GNUC__) && (__GNUC__ - 0 >= 7)
	    /* absence de "break" (intentionnelle ?) */
	    __attribute__ ((fallthrough));
#endif

	case 4:
	    /* SXVISITED->name = STRING_n */
	    /* Detection */
	case 5:
	    /* SXVISITED->name = STRING_n */
	    /* Restarting */
	case 7:
	    /* SXVISITED->name = STRING_n */
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
        return; /* added by Hubert Garavel and Wendelin Serwe */
 
    case GLOBAL_SCANNER_n:
	/* SXVISITED->name = STRING_n */
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
	/* SXVISITED->name = {DIGIT_n, S_n, X_n} */
	return;

    case KEY_TERMINAL_S_n:
	/* SXVISITED->name = C_STRING_n */
	if (!visited->not_is_first_visit)
	    get_t_code (SXVISITED, R_tables.PER_tset);

	return;

    case LOCAL_PARSER_n:
	switch (visited->position) {
	case 1:
	    /* SXVISITED->name = MODEL_s_n */
	    if (!visited->not_is_first_visit) {
		nbcart = visited->degree;
		R_tables.P_no_delete = sxba_calloc (R_tables.R_tbl_size.P_tmax + 1);
		R_tables.P_no_insert = sxba_calloc (R_tables.R_tbl_size.P_tmax + 1);
	    }
	    return;
	case 2:
	    /* SXVISITED->name = TUNING_SETS_n */
	    return;
	default:
#if EBUG
	  sxtrap("recor_smp","unknown switch case #3");
#endif
	  break;
	}
        return; /* added by Hubert Garavel and Wendelin Serwe */

    case LOCAL_SCANNER_n:
	switch (visited->position) {
	case 1:
	    /* SXVISITED->name = MODEL_s_n */
	    if (!visited->not_is_first_visit) {
		nbcart = visited->degree;
		R_tables.S_no_delete = sxba_calloc (R_tables.R_tbl_size.S_last_char_code + 1);
		R_tables.S_no_insert = sxba_calloc (R_tables.R_tbl_size.S_last_char_code + 1);
	    }
	    return;
	case 2:
	    /* SXVISITED->name = TUNING_SETS_n */
	    return;
	default:
#if EBUG
	  sxtrap("recor_smp","unknown switch case #4");
#endif
	  break;
	}
        return; /* added by Hubert Garavel and Wendelin Serwe */

    case MODEL_n:
	switch (visited->position) {
	case 1:
	    /* SXVISITED->name = ITEM_s_n */
	    if (!visited->not_is_first_visit)
		maxlregle = (maxlregle > visited->degree) ? maxlregle : visited->degree;

	    return;

	case 2:
	    /* SXVISITED->name = PARAM_s_n */
	    if (!visited->not_is_first_visit)
		maxparam = (maxparam > visited->degree - 1) ? maxparam : visited->degree - 1;

	    return;
	default:
#if EBUG
	  sxtrap("recor_smp","unknown switch case #5");
#endif
	  break;
	}
        return; /* added by Hubert Garavel and Wendelin Serwe */

    case MODEL_s_n:
	/* SXVISITED->name = MODEL_n */
	xregle = visited->position;
	return;

    case PARAM_n:
	switch (visited->position) {
	    SXINT	l;

	case 1:
	    /* SXVISITED->name = {C_PARAM_n, S_PARAM_n, VOID_n} */
	    return;

	case 2:
	    /* SXVISITED->name = STRING_n */
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
        return; /* added by Hubert Garavel and Wendelin Serwe */

    case PARAM_s_n:
	/* SXVISITED->name = PARAM_n */
	xparam = visited->position - 1;
	return;

    case PARSER_n:
	switch (visited->position) {
	case 1:
	    /* SXVISITED->name = {LOCAL_PARSER_n, VOID_n} */
	    is_scanner = SXFALSE;

	    if (!visited->not_is_first_visit) {
		nbcart = 0;
		maxparam = maxlregle = 0;
		lstring_mess = 1; /* compte du \0 */
	    }

	    return;

	case 2:
	    /* SXVISITED->name = PARAM_s_n */
	    if (!visited->not_is_first_visit)
		maxparam = (maxparam > visited->degree - 1) ? maxparam : visited->degree - 1;

	    xregle = nbcart + 1;

	case 3:
	    /* SXVISITED->name = GLOBAL_PARSER_n */
	    return;
	default:
#if EBUG
	  sxtrap("recor_smp","unknown switch case #7");
#endif
	  break;
	}
        return; /* added by Hubert Garavel and Wendelin Serwe */

    case RECOR_n:
	switch (visited->position) {
	case 1:
	    /* SXVISITED->name = TITLE_s_n */
	    if (!visited->not_is_first_visit) {
		R_tables.R_tbl_size.E_nb_kind = visited->degree;
		R_tables.R_tbl_size.E_maxltitles = 0;
	    }

	case 2:
	    /* SXVISITED->name = SCANNER_n */
	case 3:
	    /* SXVISITED->name = PARSER_n */
	    return;

	case 4:
	    /* SXVISITED->name = {STRING_n, VOID_n} */
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
        return; /* added by Hubert Garavel and Wendelin Serwe */

    case SCANNER_n:
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = {LOCAL_SCANNER_n, VOID_n} */
	    is_scanner = SXTRUE;

	    if (!visited->not_is_first_visit) {
		nbcart = 0;
		maxparam = maxlregle = 0;
		lstring_mess = 1; /* compte du \0 */
	    }

	case 2 :/* SXVISITED->name = GLOBAL_SCANNER_n */
	return;
	default:
#if EBUG
	  sxtrap("recor_smp","unknown switch case #9");
#endif
	  break;

	}

    case SLICE_n:
	/* SXVISITED->name = {C_STRING_n, OCTAL_CODE_n} */
	return;

    case TITLE_s_n:
	/* SXVISITED->name = STRING_n */
	if (!visited->not_is_first_visit) {
	    SXINT	l;

	    R_tables.R_tbl_size.E_maxltitles = (R_tables.R_tbl_size.E_maxltitles > (l = sxstrlen (visited->token.
		 string_table_entry) + 1)) ? R_tables.R_tbl_size.E_maxltitles : l;
	}
	else
	    strcpy (R_tables.E_titles [visited->position], sxstrget (visited->token.string_table_entry));

	return;

    case TUNING_SETS_n:
	/* SXVISITED->name = {DONT_DELETE_n, DONT_INSERT_n, VOID_n} */
	return;


/*
Z Z Z Z
*/

    default:
	break;
    }
}



static SXVOID	pass_derived (void)
{
    struct recor_node	*visited = SXVISITED;


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
	    lregle [xregle] [0] = sxson (SXVISITED, 1)->degree;

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

	    R_tables.P_right_ctxt_head = (nbcart == 0) ? NULL : (SXINT*) sxcalloc (nbcart + 1, sizeof (SXINT));
	    local_mess = R_tables.SXP_local_mess = (struct local_mess*) sxalloc (nbcart + 2, sizeof (struct local_mess));

	    for (i = 1; i <= nbcart + 1; i++)
		R_tables.SXP_local_mess [i].string_ref = (struct string_ref*) sxcalloc (maxparam + 1, sizeof (struct
		     string_ref));

	    string_mess = R_tables.P_string_mess = (char*) sxalloc (lstring_mess, sizeof (char));
	    global_mess = R_tables.P_global_mess = (struct global_mess*) sxcalloc (maxglobal_mess + 1, sizeof (struct global_mess));
	    xstring_mess = 0;
	    sxat_snv (SXINHERITED, SXVISITED);
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
		R_tables.SXS_local_mess [i].string_ref = (struct string_ref*) sxcalloc (maxparam + 1, sizeof (struct
		     string_ref));

	    string_mess = R_tables.S_string_mess = (char*) sxalloc (lstring_mess, sizeof (char));
	    global_mess = R_tables.S_global_mess = (struct global_mess*) sxcalloc (maxglobal_mess + 1, sizeof (struct global_mess));
	    xstring_mess = 0;
	    sxat_snv (SXINHERITED, SXVISITED);
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
		R_tables.E_titles [i] = (char*) sxcalloc (R_tables.R_tbl_size.E_maxltitles + 1, sizeof (char));

	    sxat_snv (SXINHERITED, SXVISITED);
	}

	break;

    case TUNING_SETS_n:
	break;

    case VALIDATION_LENGTH_n:
	if (visited->not_is_first_visit)
	    R_tables.R_tbl_size.P_validation_length = atoi (sxstrget (SXVISITED->token.string_table_entry));

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

    visited->not_is_first_visit = SXTRUE;
}



static SXVOID	smpopen (struct sxtables *sxtables_ptr)
{
    err_titles = sxtables_ptr->err_titles;
    sxatcvar.atc_lv.node_size = sizeof (struct recor_node);
}



static SXVOID	smppass (void)
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
	sxexit (2);

    recor_free (&R_tables);

    if (is_source) {
	listing_output ();
    }
}



SXVOID	recor_smp (SXINT what, struct sxtables *sxtables_ptr)
{
    switch (what) {
    case SXOPEN:
	smpopen (sxtables_ptr);
	break;

    case SXCLOSE:
	break;

    case SXACTION:
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
