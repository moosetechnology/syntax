/* ********************************************************
   *							  *
   *							  *
   * Copyright (c) 1996 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *							  *
   *							  *
   ******************************************************** */




/* ********************************************************
   *							  *
   *  Produit de l'equipe ATOLL.			  *
   *							  *
   ******************************************************** */





/* Main module for "make_tdef" a SYNTAX module similar to tdef which accepts the
   same input as tdef.
   It must be used instead of tdef when the input BNF is "huge".
   In that case the ".bt" tables are not generated (and thus tdef cannot work at all).
   This module must be [re]compiled for each new LC_TABLES_H (which are the GG.h C data)
   generated by
   bnf -nsc -nls -h GG.bnf > ../incl/GG.h
*/


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* Dec 21 2006 10:30 (pb):	Ajout de cette rubrique "modifications" */
/************************************************************************/

static char	ME [] = "make_tdef_main";

/*   I N C L U D E S   */

#define SX_DFN_EXT_VAR
#include "sxunix.h"
char WHAT_TDEFMAKETDEFMAIN[] = "@(#)SYNTAX - $Id: make_tdef_main.c 646 2007-06-21 13:40:55Z sagot $" WHAT_DEBUG;

#ifdef LC_TABLES_H
/* On compile les tables "left_corner" ... */
/* ... uniquement celles dont on va avoir besoin  */

#define def_inflected_form_names
#define def_non_terminal_names

#include LC_TABLES_H
#endif /* LC_TABLES_H */

#include "sxword.h"

/* On lit a priori sur stdin, et cetera */

FILE	*sxstdout = {NULL}, *sxstderr = {NULL};
FILE	*sxtty;

extern struct sxtables  sxtables;

/*   E X T E R N S   */

BOOLEAN	        sxverbosep;


/*  S T A T I C     V A R I A B L E S   */
static BOOLEAN	is_help, is_error;
static char	**err_titles;


/*---------------*/
/*    options    */
/*---------------*/

static char	Usage [] = "\
Usage:\t%s [options] [file]\n\
options=\t--help,\n\
\t\t-v, -verbose,\n\
";

#define OPTION(opt)	(1 << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB ((sizeof(option_tbl)/sizeof(*option_tbl))-1)

#define UNKNOWN_ARG 	  0
#define HELP	          1
#define VERBOSE		  2
#define SOURCE_FILE	  3
#define STDIN	          4


static char	*option_tbl [] = {
    "",
    "-help",  
    "v", "verbose",
};

static int	option_kind [] = {
    UNKNOWN_ARG,
    HELP,
    VERBOSE, VERBOSE,
};



static int	option_get_kind (arg)
    char	*arg;
{
  char	**opt;

  if (*arg++ != '-')
    return SOURCE_FILE;

  if (*arg == NUL)
    return STDIN;

  for (opt = &(option_tbl [OPT_NB]); opt > option_tbl; opt--) {
    if (strcmp (*opt, arg) == 0 /* egalite */ )
      break;
  }

  return option_kind [opt - option_tbl];
}


#if 0
static char	*option_get_text (kind)
    int	kind;
{
  int	i;

  for (i = OPT_NB; i > 0; i--) {
    if (option_kind [i] == kind)
      break;
  }

  return option_tbl [i];
}
#endif /* 0 */



static void
make_tdef_run (pathname)
    char	*pathname;
{
  FILE	*infile;

  if (pathname == NULL) {
    int	c;

    if (sxverbosep) {
      fputs ("\"stdin\":\n", sxtty);
    }

    if ((infile = sxtmpfile ()) == NULL) {
      fprintf (sxstderr, "%s: Unable to create ", ME);
      sxperror ("temp file");
      sxerrmngr.nbmess [2]++;
      return;
    }

    while ((c = getchar ()) != EOF) {
      putc (c, infile);
    }

    rewind (infile);
    sxsrc_mngr (INIT, infile, "");
  }
  else {
    if ((infile = sxfopen (pathname, "r")) == NULL) {
      fprintf (sxstderr, "%s: Cannot open (read) ", ME);
      sxperror (pathname);
      sxerrmngr.nbmess [2]++;
      return;
    }
    else {
      if (sxverbosep) {
	fprintf (sxtty, "%s:\n", pathname);
      }

      sxsrc_mngr (INIT, infile, pathname);
    }
  }

  sxerr_mngr (BEGIN);
  syntax (ACTION, &sxtables);
  sxerr_mngr (END);
  fclose (infile);
  sxsrc_mngr (FINAL);
}



/************************************************************************/
/* main function
/************************************************************************/
int main (int argc, char *argv[])
{
  int		argnum;
  BOOLEAN	is_source_file, is_stdin;
  char          *source_file;

  if (sxstdout == NULL) {
    sxstdout = stdout;
  }
  if (sxstderr == NULL) {
    sxstderr = stderr;
  }
  
#ifdef BUG
  /* Suppress bufferisation, in order to have tdef	 */
  /* messages when something goes wrong...		 */
  setbuf (stdout, NULL);
#endif
  
  sxopentty ();

  /* valeurs par defaut */
  sxverbosep = FALSE;
  is_help = FALSE;
  is_stdin = TRUE;
  is_source_file = FALSE;
  
  argnum = 0;

  while (++argnum < argc) {
    switch (option_get_kind (argv [argnum])) {
    case HELP:
      is_help = TRUE;
      break;

    case VERBOSE:
      sxverbosep = TRUE;
      break;

    case STDIN:
      is_stdin = TRUE;
      break;

    case SOURCE_FILE:
      is_stdin = FALSE;
      is_source_file = TRUE;
      source_file = argv [argnum];
      break;

    case UNKNOWN_ARG:
      fprintf (sxstderr, "%s: unknown option \"%s\".\n", ME, argv [argnum]);
      fprintf (sxstderr, Usage, ME);
      SXEXIT (3);
    }
  }

  if (!is_stdin && !is_source_file || is_help) {
    fprintf (sxstderr, Usage, ME);
    SXEXIT (3);
  }


  sxstr_mngr (BEGIN);
  (*(sxtables.analyzers.parser)) (BEGIN, &sxtables) /* Allocation des variables globales du parser */ ;
  syntax (OPEN, &sxtables) /* Initialisation de SYNTAX */ ;

  make_tdef_run (is_stdin ? NULL : source_file);

  syntax (CLOSE, &sxtables);
  (*(sxtables.analyzers.parser)) (END, &sxtables);
  sxstr_mngr (END);

  {
    int	severity;

    for (severity = SEVERITIES - 1; severity > 0 && sxerrmngr.nbmess [severity] == 0; severity--)
      ;

    SXEXIT (severity);
  }

  return EXIT_SUCCESS; /* Jamais atteint !! pour les compilo susceptibles ... */
}





static void
gen_header ()
{
  extern char	*ctime ();
  long	        date_time;
    
  date_time = time (0);

  printf ("\
/*   ******************************************************************************\n\
     This include tdef file for the language \"%s\" has been generated\n\
     on %s\
     by the SYNTAX(*) make_tdef processor\n\
     ******************************************************************************\n\
     (*) SYNTAX is a trademark of INRIA.\n\
     ****************************************************************************** */\n\n\n",
	  LANGUAGE_NAME, ctime (&date_time));

  printf ("#undef TDEF_TIME\n#define TDEF_TIME\t%i\n\n", date_time);
}




int
make_tdef_semact (what, arg)
    int		what;
    struct sxtables	*arg;
{
  switch (what) {
  case OPEN:
    err_titles = arg->err_titles;
    break;

  case INIT:
    sxword_reuse (&inflected_form_names, "inflected_form_names", sxcont_malloc, sxcont_alloc, sxcont_realloc, NULL, sxcont_free, NULL, NULL);
    sxword_reuse (&non_terminal_names, "non_terminal_names", sxcont_malloc, sxcont_alloc, sxcont_realloc, NULL, sxcont_free, NULL, NULL);

    /* On sort un petit header et un define caracteristique de la date/heure.  Ca permet de faire des
       verifs eventuelles qui assurent que les tdef sont a jour */
    gen_header ();
    is_error = FALSE;

    break;

  case ACTION:
    {
      int	        cas, code;
      char              *tnt_name;
      struct sxtoken	*tok;

      if (is_error)
	return;

      switch (cas = (int)arg) {
      case 0:
	return;

      case 1: /* <def>		= %T_NAME ~TAB~ =  <terminal> ~TAB~ ";"	~~ ; 1 */
      case 2: /* <def>		= %T_NAME ~TAB~ =  %NON_TERMINAL ~TAB~ ";"	~~ ; 2 */
	tok = &(STACKtoken (STACKtop () - 1));
	tnt_name = sxstrget (tok->string_table_entry);
	code = (cas == 1) ? sxword_retrieve (&inflected_form_names, tnt_name) : sxword_retrieve (&non_terminal_names, tnt_name);;

	if (code <= 1)
	  sxput_error (tok->source_index,
		       "%sThis unknown %sterminal symbol is ignored.", err_titles [1], (cas == 1) ? "" : "non-");
	else
	  printf ("#define %s %i\n", sxstrget (SXSTACKtoken (SXSTACKnewtop ()).string_table_entry), code-1);

	return;

      default:
	fputs ("The function \"make_tdef_semact\" is out of date with respect to its specification.\n", sxstderr);
	abort ();
      }
    }

    break;

  case ERROR:
    is_error = TRUE;
    break;

  case FINAL:
  case CLOSE:
  case SEMPASS:
    break;

  default:
    fputs ("The function \"make_tdef_semact\" is out of date with respect to its specification.\n", sxstderr);
    abort ();
  }

  return 0;
}




SXVOID
make_tdef_scanact (code, act_no)
    int		code;
    int		act_no;
{
  switch (code) {
  case OPEN:
  case CLOSE:
  case INIT:
  case FINAL:
    return;

  case ACTION:
    switch (act_no) {
      SHORT	c;

    case 1: /* \nnn => char */
      {
	int	val;
	char	c, *s, *t;

	t = s = sxsvar.lv_s.token_string + sxsvar.lv.mark.index;

	for (val = *s++ - '0'; (c = *s++) != NUL; ) {
	  val = (val << 3) + c - '0';
	}

	*t = val;
	sxsvar.lv.ts_lgth = sxsvar.lv.mark.index + 1;
	sxsvar.lv.mark.index = -1;
      }

      return;
    }

  default:
    fputs ("The function \"make_tdef_scanact\" is out of date with respect to its specification.\n", sxstderr);
    abort ();
  }
}


/* Le 20/12/04 sxtime devient un module de la librairie  sxtm_mngr.c */

VOID
sxvoid ()
/* procedure ne faisant rien */
{
}

BOOLEAN
sxbvoid ()
/* procedure ne faisant rien */
{
}

int
sxivoid ()
/* procedure ne faisant rien */
{
}
