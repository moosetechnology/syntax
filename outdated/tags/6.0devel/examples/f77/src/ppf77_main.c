/* ********************************************************
   *							  *
   *							  *
   * Copyright (c) 1986 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *							  *
   *							  *
   ******************************************************** */







/* paragrapheur de F77 */


#include "sxunix.h"
#include "varstr.h"
#include <ctype.h>
char WHAT_PPF77MAIN[] = "@(#)SYNTAX - $Id: ppf77_main.c 2209 2018-09-24 14:36:32Z garavel $";

extern struct sxtables	ppf77_tables;

extern SXVOID f77_src_mngr (SXINT what, FILE *infile, char *file_name);

/*---------------*/
/*    options    */
/*---------------*/

SXBOOLEAN		is_ansi, is_pretty_printer;
static char	ME [] = "ppf77";


#define OPTION(opt)	(1 << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB ((sizeof (option_tbl) / sizeof (*option_tbl)) - 1)

#define UNKNOWN_ARG 		0
#define VERBOSE 		1
#define ANSI 			2

static char	*option_tbl [] = {"", "v", "verbose", "nv", "noverbose", "ansi", "noansi",};
static SXINT	option_kind [] = {UNKNOWN_ARG, VERBOSE, VERBOSE, -VERBOSE, -VERBOSE,
     ANSI, -ANSI};


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

#define next_char()	(++nb > lgth) ? -1 : (int)*str++

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

    return result;
}




#include <sys/types.h>
#include <sys/stat.h>

static SXBOOLEAN	file_copy (char *in, char *out)
{


#define read_a_char(i,n)	(i==n) ? \
    (((n = read (fin, buffer, BUFSIZ)) == 0) ? EOF : (i = 1, (SXINT)buffer [0])) : \
    (SXINT)buffer [i++]


    int	fin;
/* initialise: */
    {
	struct stat	stat_buf;

	if ((fin = open (in, 0+O_BINARY)) == -1 || fstat (fin, &stat_buf) != 0) {
	    fprintf (sxstderr, "%s: Unable to open (read) ", ME);
	    sxperror (in);
	    return SXFALSE;
	}

	if (stat_buf.st_size != (off_t) sxppvariables.char_count) {
	    fprintf (sxstderr, "%s: SEVERE ERROR: Only %lld bytes have been written\non %s (instead of %ld).\n", ME, (long long)stat_buf.
		 st_size, in, (long)sxppvariables.char_count);
	    close (fin);
	    return SXFALSE;
	}

	if (out != NULL) {
	    if ((fout = sxfopen (out, "w")) == NULL) {
		fprintf (sxstderr, "%s: Unable to open (create) ", out);
		sxperror (out);
		close (fin);
		return SXFALSE;
	    }
	}
	else
	    fout = stdout;
    }

/* copy: */
{
    char		buffer [BUFSIZ];
    SXINT	c;
    SXINT	margin, j, l;
    SXINT	x, nbread;
    char	*str, *cut, f;
    SXINT			is_empty, kind, prev_kind;

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
	    if ((l = varstr_length (vstr)) <= 72 ||
		(f = *varstr_tostr (vstr)) == 'C' || f == 'c' || f == '*') {
		is_ok = my_write (varstr_tostr (vstr), l, c == '\n') != EOF;
	    }
	    else {
		/* Il faut decouper. */
/* On ne verifie pas qu'apres pp
  A statement must not have more than nineteen continuation lines.
  A statement must contain no more than 1320 characters. */

		/* On met un '0' sur la ligne initiale en colonne 6. */
		*(str = varstr_tostr (vstr) + 5) = '0';
		
		/* On regarde ou est la marge */
		/* Le pretty-printer assure qu'elle ne se trouve pas trop loin... */
		margin = 0;
		
		while (*++str == ' ')
		    margin++;
		
		str = varstr_tostr (vstr);
		kind = 0;
		cut = where_to_cut (str + 6 + margin, 66 - margin, &kind);
		l = cut - str;

		do {
		    is_ok = my_write (str, l, 1) != EOF;
		    str += l;

		    if (kind == 0)
			/* On n'a pas coupe dans une chaine. */
			while (*str == ' ')
			    /* On ne laisse pas de blancs, excepte ceux de
			       l'indentation. */
			    str++;

		    prev_kind = kind;
		    cut = where_to_cut (str, 65 - ((prev_kind == 0) ? margin : 0), &kind);
		    l = cut - str;

		    if (prev_kind == 0 && (l = cut - str) != 0) {
			/* Pas dans une chaine et ca tient */
			/* On "ajoute" 5 blancs + no de continuation line + margin + 1 blancs
			   (petit decalage). */
			l += margin + 1;

			for (j = margin; j >= 0; j--)
			    *--str = ' ';
		    }
		    /* else aucun decalage */

		    l += 6;
		    *--str = next_continuation_char ();
		    
		    for (j = 5; j > 0; j--)
			*--str = ' ';
		} while (is_ok && str + 72 < vstr->current);
		
		/* On sort la derniere tranche. */
		if (is_ok) {
		    l = vstr->current - str;
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
		     ME, (long)cumul, out, (long)sxppvariables.char_count, in);
	}

	return is_ok;
    }
}
}




static	SXVOID ppf77_run (char *pathname)
{
    FILE	*infile;
    char	un_nom [20];
    char	*fn;
    char	out_file_name [20];

    sxinitialise (infile); /* pour faire taire gcc -Wuninitialized */
    strcpy (un_nom,"/tmp/ppf77XXXXXX"); 

    if ((sxstdout = sxnewfile (un_nom)) == NULL) {
	fprintf (sxstderr, "%s: Cannot open (write) ", ME);
	sxperror (un_nom);
	sxerrmngr.nbmess [2]++;
	return;
    }

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

	if (sxverbosep) {
	    fprintf (sxtty, "%s:\n", pathname);
	}

	/* impossible d'utiliser syntax (SXBEGIN...) a cause de f77_src_mngr */
	sxerr_mngr (SXBEGIN);
	f77_src_mngr (SXINIT, infile, pathname);
    }

    syntax (SXACTION, &ppf77_tables);
    fclose (sxstdout);

    if (pathname != NULL)
	fclose (infile);

    if (sxppvariables.is_error || sxerrmngr.nbmess [2] != 0 || sxerrmngr.nbmess [1] != 0) {
        FILE *tmpfn;

	if ((tmpfn = sxnewfile (strcpy (out_file_name, "/tmp/ppf77XXXXXX"))) != NULL) {
	  fn = &(out_file_name[0]);
	  fclose (tmpfn);
	} else
	  fn = NULL;
    }
    else
	fn = pathname;

    if (file_copy (un_nom, fn))
	unlink (un_nom);
    else
	fprintf (sxstderr, "%s: The logical paragraphed version is in %s.\n", ME, un_nom);


    if (fn != pathname) {
	fprintf (sxstderr, "%s: The paragraphed version is in %s.\n", ME, fn);
	(sxerrmngr.nbmess [2])++;
    }

    /* impossible d'utiliser syntax (SXEND...) a cause de f77_src_mngr */
    f77_src_mngr (SXFINAL, NULL, NULL);
    sxerr_mngr (SXEND);
}



int main (int argc, char *argv[])
{
    int 	argnum;
    SXINT		val;

    sxopentty ();
    sxuse (val);

/* valeurs par defaut */

    is_pretty_printer = SXTRUE;
    sxverbosep = SXFALSE;
    is_ansi = SXFALSE;

    sxppvariables.kw_case = SXUPPER_CASE /* How should keywords be written */ ;
    sxppvariables.terminal_case = NULL;
    sxppvariables.kw_dark = SXFALSE /* keywords are not artificially darkened */ ;
    sxppvariables.terminal_dark = NULL /* Same as kw_dark, but for each type of terminal */ ;
    sxppvariables.no_tabs = SXTRUE /* do not optimize spaces into tabs */ ;
    sxppvariables.block_margin = SXTRUE /* do not structure when deeply nested */ ;
    sxppvariables.line_length = 0x7fff /* What it says */ ;
    sxppvariables.max_margin = 30 /* Do not indent lines further than that */ ;
    sxppvariables.tabs_interval = 1 /* number of columns between two tab positions */ ;


/* Decodage des options */

    for (argnum = 1; argnum < argc; argnum++) {
	switch (option_get_kind (argv [argnum])) {
	case VERBOSE:
	    sxverbosep = SXTRUE;
	    break;

	case -VERBOSE:
	    sxverbosep = SXFALSE;
	    break;

	case ANSI:
	    is_ansi = SXTRUE;
	    break;

	case -ANSI:
	    is_ansi = SXFALSE;
	    break;

	case UNKNOWN_ARG:
	    goto run;

	default:
	    fprintf (sxstderr, "%s: internal error %ld while decoding options.\n", ME, (long)option_get_kind (argv [argnum]));
	    SXEXIT (3);
	}
    }

run:
    if (sxverbosep) {
	setbuf (stdout, NULL);
    }

    vstr = varstr_alloc (256);

    syntax (SXINIT, &ppf77_tables, SXFALSE /* no includes */);

    ascii(' ') = SXTRUE;
    ascii('(') = SXTRUE;
    ascii(')') = SXTRUE;
    ascii('+') = SXTRUE;
    ascii('-') = SXTRUE;
    ascii(',') = SXTRUE;
    ascii(':') = SXTRUE;
    ascii('=') = SXTRUE;

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

    syntax (SXFINAL, &ppf77_tables, SXTRUE);

    varstr_free (vstr);

    SXEXIT (sxerr_max_severity ());
    return 0;
}

