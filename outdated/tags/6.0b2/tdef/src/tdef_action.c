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
   *     Produit de l'équipe Langages et Traducteurs.     *
   *                                                      *
   ******************************************************** */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20050301 12:11 (phd):	Production de #undef TDEF_TIME		*/
/************************************************************************/
/* 20030520 14:37 (phd):	Modifs pour calmer le compilo SGI	*/
/************************************************************************/
/* 20010327 18:08 (pb&phd):	Robustesse des accès à codes		*/
/************************************************************************/
/* 20-05-92 16:10 (phd):	Declaration de options_text		*/
/*				Correction de l'ecriture de la date	*/
/************************************************************************/
/* 20-05-92 16:10 (phd):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/

/*   I N C L U D E S   */

#include "sxunix.h"
#include "put_edit.h"
#include "B_tables.h"
char WHAT_TDEFACTION[] = "@(#)SYNTAX - $Id: tdef_action.c 580 2007-05-24 12:45:54Z rlacroix $" WHAT_DEBUG;



/*   E X T E R N S   */

extern BOOLEAN	is_source, is_c, is_pascal;
extern char	by_mess [], release_mess [], *prgentname;

/*  S T A T I C     V A R I A B L E S   */

static char	**err_titles;
static BOOLEAN	is_error;
static struct bnf_ag_item	bnf_ag;




/*   P R O C E D U R E S   */

static VOID	header ()
{
    put_edit_nnl (9, "Listing Of:");
    put_edit_nnl (25, sxsrcmngr.source_coord.file_name);
    put_edit_nnl (9, "By:");
    put_edit (25, by_mess);
    put_edit (25, release_mess);

    {
	extern char	*options_text ();
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

static VOID	listing_output ()
{
  VARSTR vstr;
  register FILE	*listing;
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
gen_header ()
{
    long	date_time;
    
    date_time = time (0);

    printf (
	    "\n\
/* ********************************************************************\n\
\tThis data base for \"%s\" has been generated\n\
\ton %s\
\tby the SYNTAX(*) TDEF processor\n\
   ********************************************************************\n\
\t(*) SYNTAX is a trademark of INRIA.\n\
   ******************************************************************** */\n\n\n",
	    sxsrcmngr.source_coord.file_name,
	    ctime (&date_time));

    printf ("#undef TDEF_TIME\n#define TDEF_TIME\t%ld\n\n", date_time);
}



struct codes {
  int t, nt;
};

static struct codes *codes;
static int codes_size;

static VOID	init ()
{
  int	i, ste;


/* On lit les "bnf_tables" */

  if (!bnf_read (&bnf_ag, prgentname))
    SXEXIT (2);

  codes_size = SXSTRtop() + bnf_ag.WS_TBL_SIZE.ntmax - bnf_ag.WS_TBL_SIZE.tmax;
  codes = (struct codes*) sxcalloc (codes_size + 1, sizeof (struct codes));

/* On range les terminaux */

  for (i = -1; i >= bnf_ag.WS_TBL_SIZE.tmax; i--) {
    ste = sxstrsave (bnf_ag.T_STRING + bnf_ag.ADR [i]);
    if (ste > codes_size) {
      int l = codes_size + 1; 
      codes = (struct codes*) sxrecalloc (codes, l, (codes_size *= 2) + 1, sizeof (struct codes));
    }
    codes [ste].t = -i /* positifs */;
  }

/* On range les non-terminaux */
/* Attention, la ste d'un nt peut etre celle (de la forme canonique) d'un t. */
  for (i = 1; i <= bnf_ag.WS_TBL_SIZE.ntmax; i++) {
    ste = sxstrsave (bnf_ag.NT_STRING + bnf_ag.ADR [i]);
    if (ste > codes_size) {
      int l = codes_size + 1; 
      codes = (struct codes*) sxrecalloc (codes, l, (codes_size *= 2) + 1, sizeof (struct codes));
    }     
    codes [ste].nt = i;
  }

  /* On sort un petit header et un define caracteristique de la date/heure.  Ca permet de faire des
     verifs eventuelles qui assurent que les tdef sont a jour */
  gen_header ();

  is_error = FALSE;
}



static VOID	action (action_no)
    long	action_no;
{
    register int	code, ste;

    if (is_error)
	return;

    switch (action_no) {
    case 0:
	return;

    case 1:
    /* <def>		= %T_NAME ~TAB~ =  <terminal> ~TAB~ ";"	~~ ; 1 */
	ste = SXSTACKtoken (SXSTACKtop () - 1).string_table_entry;

	if (ste <= 0 || ste > codes_size || (code = codes [ste].t) == 0)
	    sxput_error (SXSTACKtoken (SXSTACKtop () - 1).source_index,
		 "%sThis unknown terminal symbol is ignored.", err_titles [1]);
	else
	    printf ("%s%s\t%s%d%s\n", is_c ? "#define " : "", sxstrget (SXSTACKtoken (SXSTACKnewtop ()).string_table_entry),
		 is_c ? "" : "= ", code, is_c ? "" : ";");

	return;

    case 2:
    /* <def>		= %T_NAME ~TAB~ =  %NON_TERMINAL ~TAB~ ";"	~~ ; 2 */
        ste = SXSTACKtoken (SXSTACKtop () - 1).string_table_entry;

	if (ste <= 0 || ste > codes_size || (code = codes [ste].nt) == 0)
	    sxput_error (SXSTACKtoken (SXSTACKtop () - 1).source_index,
		 "%sThis unknown non-terminal symbol is ignored.", err_titles [1]);
	else
	    printf ("%s%s\t%s%d%s\n", is_c ? "#define " : "", sxstrget (SXSTACKtoken (SXSTACKnewtop ()).string_table_entry),
		 is_c ? "" : "= ", code, is_c ? "" : ";");

	return;

    default:
	fputs ("The function \"action\" is out of date with respect to its specification.\n", sxstderr);
	abort ();
    }
}



static SXVOID	final ()
{
    bnf_free (&bnf_ag);
    sxfree (codes);

    if (is_source) {
	listing_output ();
    }
}



int
tdef_action (what, arg)
    int		what;
    struct sxtables	*arg;
{
    switch (what) {
    case OPEN:
	err_titles = arg->err_titles;
	break;

    case INIT:
	init ();
	break;

    case ACTION:
	action ((long) arg);
	break;

    case ERROR:
	is_error = TRUE;
	break;

    case FINAL:
	final ();
	break;

    case CLOSE:
    case SEMPASS:
	break;

    default:
	fputs ("The function \"tdef_action\" is out of date with respect to its specification.\n", sxstderr);
	abort ();
    }

    return 0;
}
