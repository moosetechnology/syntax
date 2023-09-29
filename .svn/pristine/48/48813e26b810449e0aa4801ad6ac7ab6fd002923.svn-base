/* ********************************************************
   *							  *
   *							  *
   * Copyright (©) 1985 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *							  *
   *							  *
   ******************************************************** */




/* ********************************************************
   *							  *
   *  Produit de l'équipe Langages et Traducteurs.  (phd) *
   *							  *
   ******************************************************** */





/* Constructeur d'automates d'états finis de SYNTAX® */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20020318 14:30 (phd):	"sxtmpfile" dans "trico_run"		*/
/************************************************************************/
/* 20000623 17:07 (phd&pb):	Initialisation dynamique de sxstd???	*/
/************************************************************************/
/* 20000521 15:33 (phd):	Création à partir de sxmain.c et lecl	*/
/************************************************************************/

#ifndef lint
#ifdef	BUG
#define WHAT	"@(#)trico.d.c	- SYNTAX [WNT] - 18 mars 2002"
static struct what {
  struct what	*whatp;
  char		what [sizeof (WHAT)];
} what = {&what, WHAT};

#else
#define WHAT	"@(#)trico.c	- SYNTAX [WNT] - 18 mars 2002"
static struct what {
  struct what	*whatp;
  char		what [sizeof (WHAT)];
} what = {&what, WHAT};

#endif
#endif


#include "sxunix.h"
#include "trico_env.h"



char	by_mess [] = "the SYNTAX automaton constructor TRICO";

#include "release.h"


/* On lit a priori sur stdin, et cetera */

FILE	*sxstdout = {NULL}, *sxstderr = {NULL};

FILE		*sxtty;
BOOLEAN		sxverbosep = {TRUE};

/* On est dans un cas "mono-langage": */

extern struct sxtables	trico_tables;

/*---------------*/
/*    options    */
/*---------------*/

BOOLEAN		sxverbosep;
static char	ME [] = "trico";
static char	Usage [] = "\
Utilisation :\t%s [options] [fichiers]\n\
options=\t-sc, -source, -^sc, -^source,\n\
\t\t-v, -verbeux, -^v, -^verbeux,\n\
\t\t-ob, -objet, -^ob, -^objet\n\
\t\t-tb, -table, -^tb, -^table\n\
\t\t-ls, -liste, -^ls, -^liste,\n\
\t\t-ot, -optimaliser, -^ot, -^optimaliser,\n\
\t\t-ln nom, -langage nom\n\
\tL'option \"-ln\" est obligatoire s'il n'y a pas de fichier d'entrée.\n";

#define OPTION(opt)	(1 << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB ((sizeof(option_tbl)/sizeof(*option_tbl))-1)

#define UNKNOWN_ARG 	  0
#define SOURCE		  1
#define VERBEUX		  2
#define OBJET		  3
#define TABLE		  4
#define LISTE		  5
#define OPTIMALISER	  6
#define LANGAGE		  7
#define LAST_OPTION	  LANGAGE


static char	*option_tbl [] = {
  "",
  "sc", "source", "^sc", "^source",
  "v", "verbeux", "^v", "^verbeux",
  "ob", "objet", "^ob", "^objet",
  "tb", "table", "^tb", "^table",
  "ls", "liste", "^ls", "^liste",
  "ot", "optimaliser", "^ot", "^optimaliser",
  "ln", "langage",
};
static int	option_kind [] = {
  UNKNOWN_ARG,
  SOURCE, SOURCE, -SOURCE, -SOURCE,
  VERBEUX, VERBEUX, -VERBEUX, -VERBEUX,
  OBJET, OBJET, -OBJET, -OBJET,
  TABLE, TABLE, -TABLE, -TABLE,
  LISTE, LISTE, -LISTE, -LISTE,
  OPTIMALISER, OPTIMALISER, -OPTIMALISER, -OPTIMALISER,
  LANGAGE, LANGAGE,
};



static SXINLINE int
option_get_kind (arg)
    register char	*arg;
{
  register char	**opt;

  if (*arg++ != '-')
    return UNKNOWN_ARG;

  for (opt = &(option_tbl [OPT_NB]); opt > option_tbl; opt--) {
    if (strcmp (*opt, arg) == 0)
      break;
  }

  return option_kind [opt - option_tbl];
}



static SXINLINE char*
option_get_text (kind)
    register int	kind;
{
  register int	i;

  for (i = OPT_NB; i > 0; i--) {
    if (option_kind [i] == kind)
      break;
  }

  return option_tbl [i];
}



static SXINLINE VOID
language_name (path_name)
    char	*path_name;
{
  register char	*p;

  if ((p = sxrindex (path_name, '/')) == NULL)
    p = path_name;
  else
    p++;

  prgentname = (char*) sxalloc (strlen (p)+1, sizeof (char));

  if ((p = sxindex (strcpy (prgentname, p), '.')) != NULL) {
    *p = NUL;
  }
}


static SXINLINE SXVOID
trico_run (pathname)
    register char	*pathname;
{
  register FILE	*infile;

  if (pathname == NULL) {
    register int	c;

    sxverbosep && fputs ("\"stdin\":\n", sxtty);

    if ((infile = sxtmpfile ()) == NULL) {
      fprintf (sxstderr, "%s: Impossible de créer ", ME);
      sxperror ("le fichier temporaire ");
      sxerrmngr.nbmess [2]++;
      return;
    }

    while ((c = getchar ()) != EOF) {
      putc (c, infile);
    }

    rewind (infile);
    sxsrc_mngr (INIT, infile, "");
  }
  else if ((infile = sxfopen (pathname, "r")) == NULL) {
    fprintf (sxstderr, "%s: Impossible d'ouvrir (en lecture) ", ME);
    sxperror (pathname);
    sxerrmngr.nbmess [2]++;
    return;
  }
  else {
    sxverbosep && fprintf (sxtty, "%s:\n", pathname);
    language_name (pathname);
    sxsrc_mngr (INIT, infile, pathname);
  }

  sxerr_mngr (BEGIN);
  syntax (ACTION, &trico_tables);
  sxerr_mngr (END);
  fclose (infile);
  sxsrc_mngr (FINAL);

    if (prgentname != NULL)
      sxfree (prgentname), prgentname = NULL;
}



/************************************************************************/
/* main function
/************************************************************************/
int main (int argc, char *argv[])
{
  int	argnum;

  if (sxstdout == NULL) {
    sxstdout = stdout;
  }
  if (sxstderr == NULL) {
    sxstderr = stderr;
  }

#ifdef BUG
      /* Suppress bufferisation, in order to have proper */
      /* messages when something goes wrong...		 */

  setbuf (stdout, NULL);

#endif

  sxopentty ();
  sxttycol1p = TRUE;

/* valeurs par defaut */

  options_set = OPTION (SOURCE) | OPTION (VERBEUX) | OPTION (OBJET) | OPTION (OPTIMALISER);
  is_source = sxverbosep = is_object = is_optimize = TRUE;
  is_table = is_list = FALSE;

/* Décodage des options */

  for (argnum = 1; argnum < argc; argnum++) {
    switch (option_get_kind (argv [argnum])) {
    case SOURCE:
      is_source = TRUE, options_set |= OPTION (SOURCE);
      break;

    case -SOURCE:
      is_source = is_object = is_table = is_list = FALSE, options_set &= noOPTION (SOURCE) & noOPTION (OBJET) & noOPTION (TABLE) & noOPTION (LISTE);
      break;

    case VERBEUX:
      sxverbosep = TRUE, options_set |= OPTION (VERBEUX);
      break;

    case -VERBEUX:
      sxverbosep = FALSE, options_set &= noOPTION (VERBEUX);
      break;

    case OBJET:
      is_source = is_object = TRUE, options_set |= OPTION (SOURCE) | OPTION (OBJET);
      break;

    case -OBJET:
      is_object = FALSE, options_set &= noOPTION (OBJET);
      break;

    case TABLE:
      is_source = is_table = TRUE, options_set |= OPTION (SOURCE) | OPTION (TABLE);
      break;

    case -TABLE:
      is_table = FALSE, options_set &= noOPTION (TABLE);
      break;

    case LISTE:
      is_source = is_object = is_table = is_list = TRUE, options_set |= OPTION (SOURCE) | OPTION (OBJET) | OPTION (TABLE) | OPTION (LISTE);
      break;

    case -LISTE:
      is_list = FALSE, options_set &= noOPTION (LISTE);
      break;

    case OPTIMALISER:
      is_optimize = TRUE, options_set |= OPTION (OPTIMALISER);
      break;

    case -OPTIMALISER:
      is_optimize = FALSE, options_set &= noOPTION (OPTIMALISER);
      break;

    case LANGAGE:
      if (++argnum >= argc) {
	fprintf (sxstderr, "%s: il faut un nom après l'option \"%s\" ;\n\ton choisit ''.\n", ME, option_get_text (LANGAGE));
	prgentname = (char*) sxalloc (1, sizeof (char));
	prgentname [0] = NUL;
      }
      else {
	prgentname = (char*) sxalloc (strlen (argv [argnum])+1, sizeof (char));
	strcpy (prgentname, argv [argnum]);
      }

      options_set |= OPTION (LANGAGE);
      break;

    case UNKNOWN_ARG:
      if (options_set & OPTION (LANGAGE)) {
	fprintf (sxstderr, "%s: on ne peut utiliser l'option \"%s\" pour désigner des fichiers.\n", ME, option_get_text (LANGAGE));
	options_set &= noOPTION (LANGAGE);
      }

      goto run;

    default:
      fprintf (sxstderr, "%s: erreur interne %d lors du décodage des options.\n", ME, option_get_kind (argv [argnum]));
      break;
    }
  }

  if (!(options_set & OPTION (LANGAGE))) {
    fprintf (sxstderr, "%s: L'option \"-%s\" est obligatoire s'il n'y a pas de fichier d'entrée ;\n\ton choisit ''\n", ME, option_get_text (LANGAGE));
    prgentname = (char*) sxalloc (1, sizeof (char));
    prgentname [0] = NUL;
  }

  run:
  if (sxverbosep) {
    fprintf (sxtty, "%s\n", release_mess);
  }

  sxstr_mngr (BEGIN) /* Création de la table des chaines */ ;
  (*(trico_tables.analyzers.parser)) (BEGIN) /* Allocation des variables globales du parser */ ;
  syntax (OPEN, &trico_tables) /* Initialisation de SYNTAX (mono-langage) */ ;

  if (options_set & OPTION (LANGAGE)) {
    trico_run (NULL);
  }
  else {
    do {
      register int	severity = sxerrmngr.nbmess [2];

      sxerrmngr.nbmess [2] = 0;
      /* sxstr_mngr (BEGIN) /* remise a vide de la string table */ ;
      trico_run (argv [argnum++]);
      sxerrmngr.nbmess [2] += severity;
    } while (argnum < argc);
  }

  syntax (CLOSE, &trico_tables);
  (*(trico_tables.analyzers.parser)) (END) /* Inutile puisque le process va etre termine */ ;
  sxstr_mngr (END) /* Inutile puisque le process va etre termine */ ;

  {
    int	severity;

    for (severity = SEVERITIES - 1; severity > 0 && sxerrmngr.nbmess [severity] == 0; severity--) {
	  /* null; */
    }

    SXEXIT (severity);
  }

  return EXIT_SUCCESS; /* Jamais atteint !! pour les compilo susceptibles ... */
}



char*
options_text (line)
    register char	*line;
{
  register int	i;
  BOOLEAN	is_first = TRUE;

  *line = NUL;

  for (i = 1; i <= LAST_OPTION; i++)
    if (options_set & OPTION (i)) {
      if (is_first)
	is_first = FALSE;
      else
	strcat (line, ", ");

      strcat (line, option_get_text (i));
    }

  return line;
}



VOID
sxvoid ()
/* Procédure ne faisant rien, mais le faisant bien ! */
{
}
