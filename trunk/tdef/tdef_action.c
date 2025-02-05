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

/*   I N C L U D E S   */

#include "sxversion.h"
#include "sxunix.h"
#include "put_edit.h"
#include "B_tables.h"

char WHAT_TDEFACTION[] = "@(#)SYNTAX - $Id: tdef_action.c 4166 2024-08-19 09:00:49Z garavel $" WHAT_DEBUG;

/*   E X T E R N S   */

extern bool	is_source, is_c;
extern char	by_mess [], *prgentname;

/*  S T A T I C     V A R I A B L E S   */

static char	**err_titles;
static bool	is_error;
static struct bnf_ag_item	bnf_ag;

/*   P R O C E D U R E S   */

static void	header (void)
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

static void	listing_output (void)
{
  VARSTR vstr;
  FILE	*listing;
  char	*lst_name;

  if (sxverbosep)
    fputs ("Listing Output\n", sxtty);

  vstr = varstr_alloc (32);

  vstr = varstr_catenate (vstr, prgentname);
  vstr = varstr_catenate (vstr, ".td.l");
  lst_name = varstr_tostr (vstr);

  if ((listing = sxfopen (lst_name, "w")) == NULL) {
    fprintf (sxstderr, "listing_output: cannot open (create) ");
    sxperror (lst_name);
    return;
  }

  varstr_free (vstr), vstr = NULL;

  put_edit_initialize (listing);
  header ();
  sxlisting_output (listing);
  put_edit_finalize ();
}

static void
gen_header (void)
{
    printf ("%s\n", SXCOPYRIGHT);
    printf ("/* This file has been generated from \"%s\" by the TDEF processor of SYNTAX */\n\n",
	    sxsrcmngr.source_coord.file_name);
}



struct codes {
  SXINT t, nt;
};

static struct codes *codes;
static SXINT codes_size;

static void	init (void)
{
  SXINT	i, ste;


/* On lit les "bnf_tables" */

  if (!bnf_read (&bnf_ag, prgentname))
    sxexit (2);

  codes_size = SXSTRtop() + bnf_ag.WS_TBL_SIZE.ntmax - bnf_ag.WS_TBL_SIZE.tmax;
  codes = (struct codes*) sxcalloc (codes_size + 1, sizeof (struct codes));

/* On range les terminaux */

  for (i = -1; i >= bnf_ag.WS_TBL_SIZE.tmax; i--) {
    ste = sxstrsave (bnf_ag.T_STRING + bnf_ag.ADR [i]);
    if (ste > codes_size) {
      SXINT l = codes_size + 1; 
      codes = (struct codes*) sxrecalloc (codes, l, (codes_size *= 2) + 1, sizeof (struct codes));
    }
    codes [ste].t = -i /* positifs */;
  }

/* On range les non-terminaux */
/* Attention, la ste d'un nt peut etre celle (de la forme canonique) d'un t. */
  for (i = 1; i <= bnf_ag.WS_TBL_SIZE.ntmax; i++) {
    ste = sxstrsave (bnf_ag.NT_STRING + bnf_ag.ADR [i]);
    if (ste > codes_size) {
      SXINT l = codes_size + 1; 
      codes = (struct codes*) sxrecalloc (codes, l, (codes_size *= 2) + 1, sizeof (struct codes));
    }     
    codes [ste].nt = i;
  }

  /* On sort un petit header et un define caracteristique de la date/heure.  Ca permet de faire des
     verifs eventuelles qui assurent que les tdef sont a jour */
  gen_header ();

  is_error = false;
}



static void	action (SXINT action_no)
{
    SXINT	code, ste;

    if (is_error)
	return;

    switch (action_no) {
    case 0:
	return;

    case 1:
    /* <def>		= %T_NAME ~TAB~ =  <terminal> ~TAB~ ";"	~~ ; 1 */
	ste = SXSTACKtoken (SXSTACKtop () - 1).string_table_entry;

	if (ste <= 0 || ste > codes_size || (code = codes [ste].t) == 0)
	    sxerror (SXSTACKtoken (SXSTACKtop () - 1).source_index,
		     err_titles [1][0],
		     "%sThis unknown terminal symbol is ignored.",
		     err_titles [1]+1);
	else
	    printf ("%s%s\t%s%ld%s\n", is_c ? "#define " : "", sxstrget (SXSTACKtoken (SXSTACKnewtop ()).string_table_entry),
		 is_c ? "" : "= ", code, is_c ? "" : ";");

	return;

    case 2:
    /* <def>		= %T_NAME ~TAB~ =  %NON_TERMINAL ~TAB~ ";"	~~ ; 2 */
        ste = SXSTACKtoken (SXSTACKtop () - 1).string_table_entry;

	if (ste <= 0 || ste > codes_size || (code = codes [ste].nt) == 0)
	    sxerror (SXSTACKtoken (SXSTACKtop () - 1).source_index,
		     err_titles [1][0],
		     "%sThis unknown non-terminal symbol is ignored.",
		     err_titles [1]+1);
	else
	    printf ("%s%s\t%s%ld%s\n", is_c ? "#define " : "", sxstrget (SXSTACKtoken (SXSTACKnewtop ()).string_table_entry),
		 is_c ? "" : "= ", code, is_c ? "" : ";");

	return;

    default:
	fputs ("The function \"action\" is out of date with respect to its specification.\n", sxstderr);
	sxexit(1);
    }
}



static void	final (void)
{
    bnf_free (&bnf_ag);
    sxfree (codes);

    if (is_source) {
	listing_output ();
    }
}



void tdef_action (SXINT what, SXINT action_no, SXTABLES *arg)
{
    switch (what) {
    case SXOPEN:
	err_titles = arg->err_titles;
	break;

    case SXINIT:
	init ();
	break;

    case SXACTION:
	action (action_no);
	break;

    case SXERROR:
	is_error = true;
	break;

    case SXFINAL:
	final ();
	break;

    case SXCLOSE:
    case SXSEMPASS:
	break;

    default:
	fputs ("The function \"tdef_action\" is out of date with respect to its specification.\n", sxstderr);
	sxexit(1);
    }
}
