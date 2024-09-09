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

/* paragrapheur de F77 */

#include "sxversion.h"
#include "sxunix.h"
#include "varstr.h"
#include "ppf77_td.h"

#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

char WHAT_PPF77MAIN[] = "@(#)SYNTAX - $Id: ppf77_main.c 4177 2024-08-26 12:38:27Z garavel $";

extern SXTABLES	sxtables;

extern void f77_src_mngr (SXINT what, FILE *infile, char *file_name);

/*---------------*/
/*    options    */
/*---------------*/

bool		is_ansi, is_json, is_pretty_printer, is_input_free_fortran, is_output_free_fortran, is_extension;

static char	ME [] = "ppf77_main";
static char	Usage [] = "\
Usage:\t%s [options] [files]\n\
options=\t-v, -verbose, -nv, -noverbose,\n\
\t\t-ansi, -noansi,\n\
\t\t-X, -extension, -nX, -noextension,\n\
\t\t-fs, -format_suffix, -nfs, -noformat_suffix,\n		\
\t\t-iff, -input_free_fortran, -niff, -noinput_free_fortran.\n\
\t\t-off, -output_free_fortran, -noff, -nooutput_free_fortran.\n";


#define OPTION(opt)	(1 << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB ((sizeof (option_tbl) / sizeof (*option_tbl)) - 1)

#define UNKNOWN_ARG 		0
#define VERBOSE 		1
#define ANSI 			2
#define FORMAT_SUFFIX		3
#define INPUT_FREE_FORTRAN	4
#define OUTPUT_FREE_FORTRAN	5
#define EXTENSION               6

static char	*option_tbl [] = {"", "v", "verbose", "nv", "noverbose",
				  "ansi", "noansi",
				  "fs", "nfs", "format_suffix", "no_format_suffix",
				  "iff", "input_free_fortran", "niff", "noinput_free_fortran",
				  "off", "output_free_fortran", "noff", "nooutput_free_fortran",
				  "X", "extension", "nX", "noextension"
                                  };
static SXINT	option_kind [] = {UNKNOWN_ARG, VERBOSE, VERBOSE, -VERBOSE, -VERBOSE,
				  ANSI, -ANSI, FORMAT_SUFFIX, -FORMAT_SUFFIX, FORMAT_SUFFIX, -FORMAT_SUFFIX,
				  INPUT_FREE_FORTRAN, INPUT_FREE_FORTRAN, -INPUT_FREE_FORTRAN, -INPUT_FREE_FORTRAN,
				  OUTPUT_FREE_FORTRAN, OUTPUT_FREE_FORTRAN, -OUTPUT_FREE_FORTRAN, -OUTPUT_FREE_FORTRAN,
				  EXTENSION, EXTENSION, -EXTENSION, -EXTENSION
                                 };


static SXINT continuation_line_nb;

static FILE	*fout;
static SXINT	cumul;
static SXINT	is_ok;
static VARSTR	vstr;

static char next_continuation_char (void)
{
    char v;

    v = (++continuation_line_nb) % 10;

    if (v == 0)
	v++, ++continuation_line_nb;

    return '0' + v;
}

static SXINT	option_get_kind (char *arg)
{
    char	**opt;

    if (*arg++ != '-')
	return UNKNOWN_ARG;

    for (opt = &(option_tbl [OPT_NB]); opt > option_tbl; opt--) {
	if (strcmp (*opt, arg) == 0 /* egalite */ )
	    break;
    }

    return option_kind [opt - option_tbl];
}

static char cs [0201];
#define ascii(c)	(cs+1)[c]
#define is_sep(c)	(ascii(c)!=0)

static char *where_to_cut (char *str, SXINT lgth, SXINT *kind)
{
    /* On ne coupe que sur des separateurs : " ()+,-:=" */
    /* apres une chaine, il faut inhiber l'indentation. */
    char		*start = str;
    char	*tail = NULL;
    int         c;
    SXINT	nb = 0, val;

#define next_char()	((++nb > lgth) ? -1 : (int)*str++)

    val = *kind;
    *kind = 0;

switch (val) {
 case -1:
    /* string */
    tail = str;
    goto STATE_5;

 case 0:
   goto STATE_1;

 default:
    /* Hollerith */
    goto STATE_3b;
}


STATE_1:
    c = next_char();

STATE_1b:
    while (is_sep (c))
	c = next_char();
	
    if (c == -1)
	return str;

    tail = str - 1;

    if (c == '*' || c == '/')
	goto STATE_2;

    if (isdigit (c))
	goto STATE_3;

    if (c == '\'')
	goto STATE_5;

    goto STATE_4;

STATE_2:
    /* On ne coupe pas au milieu de "**" ou "//". */
    if ((c = next_char()) == -1)
	return str - 1;
    
    if (c == '*' || c == '/')
	goto STATE_1;

    goto STATE_1b;

STATE_3:
    while (isdigit (c = next_char()));

    if (c == -1)
	return tail;

    if (c != 'H')
	goto STATE_4;

    val = 0;
    nb -= str - tail;
    str = tail;
    
    while (isdigit (c = next_char())) {
	val *= 10;
	val += c - '0';
    }
    
 STATE_3b:
    if (nb + val > lgth) {
	if (tail == start) {
	    /* Coupure dans la chaine */
	    while ((c = next_char()) != -1)
		val--;
	    
	    *kind = val;
	    return str-1;
	}
	
	/* On essaie de faire tenir une chaine sur une ligne sans
	   la couper. */
	return tail;
    }
    
    while (val-- > 0)
	next_char ();
    
    goto STATE_1;


STATE_4:
    while (c != -1 && !is_sep (c))
	c = next_char();

    if (c == -1)
	return tail;

    goto STATE_1b;  

STATE_5:
    while ((c = next_char()) != '\'') {
	if (c == -1) {
	    if (tail == start) {
		/* Coupure dans la chaine */
		*kind = -1;
		return str;
	    }

	    /* On essaie de faire tenir une chaine sur une ligne sans
	       la couper. */
	    return tail; 
	}
    }

    goto STATE_1;
}



static SXINT my_write (char *str, SXINT l, SXINT with_nl)
{
    SXINT	result;
    char c, *s;

    s = str + l;
    c = *s;
    *s = '\0';

    if (with_nl)
	cumul++, result = fprintf (fout, "%s\n", str);
    else
	result = fputs (str, fout);

    *s = c;
    cumul += l;

    return result; /* nb de caractères écrits ou EOF si erreur */
}

static char  format_suffix_model [] = "%05dXXX";
static char  format_suffix [9];
static char *save_format_suffix;

static SXINT my_write_with_format_suffix (char *str, SXINT l, SXINT with_nl)
{
    static SXINT card_no;
    static char  card [82];
    static char  spaces [] = "                                                                                ";

    if (save_format_suffix == NULL)
      return my_write (str, l, with_nl);

    /* format_suffix est une copie de save_format_suffix de la forme "%05dXXX" où les X ont été remplacés par
       les derniers caractères du (préfixe du) nom du source */

    strncpy (card+1, str, (l>72)?72:l);

    if (l < 72)
      strncpy (card+1+l, spaces, 72-l);

    sprintf (card+73, format_suffix, ++card_no);

    return my_write (card+1, 80, with_nl);
}

static bool	file_copy (char *in, char *out)
{
#define read_a_char(i,n)	(i==n) ?				\
    (((n = read (fin, buffer, BUFSIZ)) == 0) ? EOF : (i = 1, (SXINT)buffer [0])) : \
  (SXINT)buffer [i++]

  int	fin;
  /* initialise: */
  {
    struct stat	stat_buf;

    if ((fin = open (in, 0+O_BINARY)) == -1 || fstat (fin, &stat_buf) != 0) {
      fprintf (sxstderr, "%s: Unable to open (read) ", ME);
      sxperror (in);
      return false;
    }

    if (stat_buf.st_size != (off_t) sxppvariables.char_count) {
      fprintf (sxstderr, "%s: SEVERE ERROR: Only %lld bytes have been written on %s (instead of %ld).\n",
	       ME,
	       (long long) stat_buf.st_size,
	       in,
	       (SXINT) sxppvariables.char_count);

      close (fin);
      return false;
    }

    if (out != NULL) {
      if ((fout = sxfopen (out, "w")) == NULL) {
	fprintf (sxstderr, "%s: Unable to open (create) ", out);
	sxperror (out);
	close (fin);
	return false;
      }
    }
    else
      fout = stdout;

#if EBUG
    fprintf (sxstderr, "%s/%s: decoupage de la version logique %s dans la version physique %s.\n",
	     ME, "file_copy", in, (out == NULL) ? "stdout" : out);
#endif
  }

  /* copy: */
  {
    char	buffer [BUFSIZ];
    SXINT	c;
    SXINT	margin, j, l;
    SXINT	x, nbread;
    char	*str, *cut, f;
    SXINT	is_empty, kind, prev_kind;

    is_ok = 1;
    cumul = x = nbread = 0;
    c = read_a_char (x, nbread);

    while (is_ok && c != EOF) {
      varstr_raz (vstr);
      is_empty = 1;

      while (c != '\n' && c != EOF) {
	varstr_catchar (vstr, c);

	if (c != ' ')
	  is_empty = 0;

	c = read_a_char (x, nbread);
      }

      if (is_empty == 0) {
	/* Nouvelle version Pierre le 28/08/2023 */
	str = varstr_tostr (vstr);
	l = varstr_length (vstr);
	
	if (l <= 72 ||
	    (f = *str) == 'C' || f == 'c' || f == '*') {
	  is_ok = my_write_with_format_suffix (str, l, c == '\n') != EOF;
	}
	else {
	  /* Il faut decouper. */
	  /* On ne verifie pas qu'apres pp
	     A statement must not have more than nineteen continuation lines.
	     A statement must contain no more than 1320 characters. */

	  /* On transforme vstr en une vraie chaine c */
	  (void) varstr_complete (vstr);
	  
#if EBUG
	  fprintf (sxstderr, "%s/%s: il faut découper cette ligne logique en plusieurs lignes physiques :\n%s\n",
		   ME, "file_copy", str); /* Pb si str contient des \0 dans une chaine!! */
#endif
	  
	  /* On ne met pas de '0' sur la ligne initiale en colonne 6 */
	  str+= 5;
		
		
	  /* On regarde ou est la marge */
	  /* Le pretty-printer assure qu'elle ne se trouve pas trop loin... */
	  margin = 0;
		
	  while (*++str == ' ')
	    margin++;

	  /* ici, str pointe juste après le dernier ' ' de la marge */

	  kind = 0;
	  cut = where_to_cut (str, 66 - margin /* limite du cut */, &kind);
	  str = varstr_tostr (vstr); /* Ici, on pointe au début de la ligne logique */
	  l = cut-str; /* on doit sortir l caractères à partir de str... */
	  /* ... et il en reste forcément à sortir car on est dans le cas découpage!! */

	  do {
#if EBUG
	     {
	       char car;

	       car = str [l];
	       str [l] = SXNUL;
	       fprintf (sxstderr, "%s\n", str); /* Pb si str contient des \0 dans une chaine!! */
	       str [l] = car;
	     }
#endif
	    is_ok = my_write (str, l, 1) != EOF; /* ... on les sort et si is_ok, l'écriture s'est bien passee... */
	    
	    str = cut; /* Le suffixe commence en str */

	    if (kind == 0)
	      /* On n'a pas coupe dans une chaine. */
	      while (*str == ' ')
		/* On ne laisse pas de blancs, excepte ceux de
		   l'indentation. */
		str++;

	    prev_kind = kind;
	    /* On regarde quelle sera la nouvelle tranche à sortir */
	    l = 65 - ((prev_kind == 0) ? margin : 0);
	    /* cette tranche comportera au plus l caractères significatifs */

	    if (vstr->current - str > l)
	      /* Il reste plus d'une tranche */
	      cut = where_to_cut (str, l, &kind);
	    else
	      /* c'est la dernière tranche */
	      cut = vstr->current;

	    l = cut - str; /* ... en fait elle en contient l */

	    if (prev_kind == 0 && l != 0) {
	      /* Pas dans une chaine et ca tient */
	      /* On "ajoute" 5 blancs + no de continuation line + margin + 1 blanc (petit decalage). */
	      l += margin + 1;

	      for (j = margin; j >= 0; j--)
		*--str = ' ';
	    }
	    /* else aucun decalage */

	    l += 6;
	    *--str = next_continuation_char ();
		    
	    for (j = 5; j > 0; j--)
	      *--str = ' ';
	  } while (is_ok && cut < vstr->current);
		
	  /* On sort la derniere tranche. */
	  if (is_ok) {
	    l = vstr->current - str;
#if EBUG
	    {
	      char car;

	      car = str [l];
	      str [l] = SXNUL;
	      fprintf (sxstderr, "%s\n", str); /* Pb si str contient des \0 dans une chaine!! */
	      str [l] = car;
	    }
#endif
	    is_ok = my_write (str, l, c == '\n') != EOF;
	  }

	  continuation_line_nb = 0;
	}
      }

      if (c == '\n')
	c = read_a_char (x, nbread);
    }

    /* finalise: */
    {
      close (fin);
      fclose (fout);

      if (!is_ok) {
	fprintf (sxstderr, "%s: SEVERE ERROR: Only the %ld first bytes have been written\non %s (out of the %ld in %s).\n",
		 ME, (SXINT) cumul, out, (SXINT) sxppvariables.char_count, in);
      }

      return is_ok;
    }
  }
}

static	void ppf77_run (char *pathname) {
  FILE	*infile, *old_sxstdout;
  char	*fn;
  char	out_file_name [20];
  bool success;

  sxinitialise (infile); /* pour faire taire gcc -Wuninitialized */
  old_sxstdout = sxstdout; /* pour remettre en état, au cas où... */

  strcpy (sxppvariables.logical_file_name,"/tmp/ppf77XXXXXX"); 

  if ((sxstdout = sxnewfile (sxppvariables.logical_file_name)) == NULL) {
    fprintf (sxstderr, "%s: Cannot open (write) %s.\n", ME, sxppvariables.logical_file_name);
    sxstdout = stdout; /* On pp sur stdout */
    sxperror (sxppvariables.logical_file_name);
    sxerrmngr.nbmess [2]++;
    return;
  }

#if EBUG
  fprintf (sxstdout, "%s: sxstdout = %s.\n",
	   ME,
	   (sxstdout == stdout) ? "stdout" : sxppvariables.logical_file_name);
#endif

  if (pathname == NULL) {
    if (sxverbosep) {
      fputs ("\"stdin\":\n", sxtty);
    }
    /* impossible d'utiliser syntax (SXBEGIN...) a cause de f77_src_mngr */
    sxerr_mngr (SXBEGIN);
    f77_src_mngr (SXINIT, stdin, "");
  }
  else {
    if ((infile = sxfopen (pathname, "r")) == NULL) {
      fprintf (sxstderr, "%s: Cannot open (read) ", ME);
      sxperror (pathname);
      sxerrmngr.nbmess [2]++;
      return;
    }

    /* traitement du FORMAT_SUFFIX éventuel */
    if (save_format_suffix) {
      SXINT X_nb, col, i;
      char  c;

      /* En cas d'appel multiple avec des sources différents */
      strncpy (format_suffix, save_format_suffix, 8);	      
      
      X_nb = 0, col = 7;

      while (--col >= 0) {
	if (format_suffix [col] != 'X')
	  break;

	X_nb++;
      }

      i = -1;

      while ((c = pathname [++i]) != SXNUL) {
	if (c == '.')
	  break;
      }

      /* On remplace les X par les derniers caractères du (préfixe) de pathname */
      col = 7;
      
      while (X_nb-- > 0) 
	format_suffix [--col] = pathname [--i];
    }

    if (sxverbosep) {
      fprintf (sxtty, "%s:\n", pathname);
    }

    /* impossible d'utiliser syntax (SXBEGIN...) a cause de f77_src_mngr */
    sxerr_mngr (SXBEGIN);
    f77_src_mngr (SXINIT, infile, pathname);
  }

  syntax (SXACTION, &sxtables);

  if (pathname != NULL)
    fclose (infile);

  if (is_output_free_fortran) {
    fprintf (sxtty, "%s: the output free fortran version is in the file \"%s\".\n",
	     ME, sxppvariables.logical_file_name);
  }
  else {
    if (sxstdout == stdout) {
      /* On a perdu la version logique, on ne peut donc pas faire la version physique */
    }
    else {
      /* Là, on essaie de fabriquer la version physique à partir de la version logique */
#if EBUG
      fprintf (sxstderr, "%s: the logical pp version is in %s (%ld bytes long).\n",
	       ME, sxppvariables.logical_file_name, ftell (sxstdout));
#endif

      if (sxppvariables.is_error || sxerrmngr.nbmess [2] != 0 || sxerrmngr.nbmess [1] != 0) {
	/* Prudence, s'il y a eu un problème, on ne va pas écraser le source */
	FILE *tmpfn;

	if ((tmpfn = sxnewfile (strcpy (out_file_name, "/tmp/ppf77XXXXXX"))) != NULL) {
	  fn = &(out_file_name[0]);
	  fclose (tmpfn);
	}
	else
	  fn = NULL;
      }
      else
	fn = pathname;

      /* La version physique va être fabriquée dans le fichier fn (si fn == null ce sera stdout!!) */
      success = file_copy (sxppvariables.logical_file_name, fn);
    
#if EBUG
      fprintf (sxstderr, "%s: The logical paragraphed version is in %s.\n", ME, sxppvariables.logical_file_name);
      fprintf (sxstderr, "%s: The physical paragraphed version (%s) is in %s.\n",
	       ME, success ? "success" : "fail", (fn == NULL) ? "stdout" : fn);
#else
      if (success)
	/* On efface cette version */
	unlink (sxppvariables.logical_file_name);
#endif

      if (fn != pathname) {
	(sxerrmngr.nbmess [2])++;
      }
    }
  }

  if (sxstdout != stdout && sxstdout != old_sxstdout) {
    fclose (sxstdout);
    sxstdout = old_sxstdout;
  }

  /* impossible d'utiliser syntax (SXEND...) a cause de f77_src_mngr */
  f77_src_mngr (SXFINAL, NULL, NULL);
  sxerr_mngr (SXEND);
}

/* Le pretty-printing d'un programme FORTRAN se passe en deux phases : 
 *   - la premiere ne tient pas compte du decoupage en cartes et le resultat
 *     est mis dans un fichier temporaire de /tmp ;
 *   - la deuxieme decoupe en cartes.
 */

int main (int argc, char *argv[])
{
    int 	argnum;
    SXINT	val;

    sxopentty ();
    sxuse (val);

/* valeurs par defaut */

    is_json = false; /* always false for ppf77 */
    is_pretty_printer = true; /* always true for ppf77 */
    sxverbosep = false;
    is_ansi = false;
    is_input_free_fortran = false;
    is_output_free_fortran = false;
    is_extension = false; /* ppf77 accepte ou non les extensions */

    sxppvariables.kw_case = SXUPPER_CASE /* How should keywords be written */ ;
    sxppvariables.terminal_case = (SXCASE *) sxcalloc (sxeof_code (&sxtables)+1, sizeof (SXCASE));
    sxppvariables.kw_dark = false /* keywords are not artificially darkened */ ;
    sxppvariables.terminal_dark = NULL /* Same as kw_dark, but for each type of terminal */ ;
    sxppvariables.no_tabs = true /* do not optimize spaces into tabs */ ;
    sxppvariables.block_margin = true /* do not structure when deeply nested */ ;
    sxppvariables.line_length = 0x7fff /* What it says */ ;
    sxppvariables.max_margin = 30 /* Do not indent lines further than that */ ;
    sxppvariables.tabs_interval = 1 /* number of columns between two tab positions */ ;

    sxppvariables.terminal_case [ID_t] = SXLOWER_CASE; /* On met les %id en minuscules */


/* Decodage des options */

    for (argnum = 1; argnum < argc; argnum++) {
	switch (option_get_kind (argv [argnum])) {
	case VERBOSE:
	    sxverbosep = true;
	    break;

	case -VERBOSE:
	    sxverbosep = false;
	    break;

	case ANSI:
	    is_ansi = true;
	    break;

	case -ANSI:
	    is_ansi = false;
	    break;

	case FORMAT_SUFFIX:
	    save_format_suffix = (char*) sxalloc (8, sizeof (char));

	    if (++argnum >= argc) {
	      /* L'option FORMAT_SUFFIX n'est pas suivie d'une chaine, on prend le format par defaut "%05dXXX" */
		fprintf (sxstderr, "%s: a printf-format must follow the \"format_suffix\" option;\n\t'%s' is assumed.\n",
			 ME, format_suffix_model);
		strncpy (save_format_suffix, format_suffix_model, 8);
	    }
	    else {
	      strncpy (save_format_suffix, argv [argnum], 8);
	    }

	    save_format_suffix [7] = SXNUL;

	    break;
	  
	case -FORMAT_SUFFIX:
	    save_format_suffix = NULL;
	    break;

	case INPUT_FREE_FORTRAN:
	    is_input_free_fortran = true;
	    break;

	case -INPUT_FREE_FORTRAN:
	    is_input_free_fortran = false;
	    break;

	case OUTPUT_FREE_FORTRAN:
	    is_output_free_fortran = true;
	    break;

	case -OUTPUT_FREE_FORTRAN:
	    is_output_free_fortran = false;
	    break;

	case EXTENSION:
	    is_extension = true;
	    break;

	case -EXTENSION:
	    is_extension = false;
	    break;

	case UNKNOWN_ARG:
	    goto run;

	default:
	    fprintf (sxstderr, Usage, "ppf77.out");
	    fprintf (sxstderr, "%s: internal error %ld while decoding options.\n", ME, (SXINT) option_get_kind (argv [argnum]));
	    sxexit (3);
	}
    }

run:
    if (is_output_free_fortran && save_format_suffix != NULL) {
      /* Options incompatibles */
      fprintf (sxstderr, "%s: The option 'format_suffix' is ignored because of its incompatibility with the option 'output_free_fortran'.\n", ME);
      sxfree (save_format_suffix), save_format_suffix = NULL;
    }

    if (sxverbosep) {
	setbuf (stdout, NULL);
    }

    vstr = varstr_alloc (256);

    syntax (SXINIT, &sxtables, false /* no includes */);

    ascii(' ') = true;
    ascii('(') = true;
    ascii(')') = true;
    ascii('+') = true;
    ascii('-') = true;
    ascii(',') = true;
    ascii(':') = true;
    ascii('=') = true;

    if (argnum == argc) {
	ppf77_run (NULL);
    }
    else {
	do {
	    int	sseverity, pseverity;

	    sseverity = sxerrmngr.nbmess [1], sxerrmngr.nbmess [1] = 0;
	    pseverity = sxerrmngr.nbmess [2], sxerrmngr.nbmess [2] = 0;
	    ppf77_run (argv [argnum++]);
	    sxerrmngr.nbmess [1] += sseverity;
	    sxerrmngr.nbmess [2] += pseverity;
	} while (argnum < argc);
    }

    syntax (SXFINAL, &sxtables, true);

    varstr_free (vstr);
    sxfree (sxppvariables.terminal_case), sxppvariables.terminal_case = NULL;

    if (save_format_suffix)
      sxfree (save_format_suffix), save_format_suffix = NULL;

    sxexit (sxerr_max_severity ());
    return 0;
}

