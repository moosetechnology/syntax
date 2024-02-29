/* ********************************************************
   *							  *
   *							  *
   * Copyright (c) 1984 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *							  *
   *							  *
   ******************************************************** */




/* ********************************************************
   *							  *
   *     Produit de l'equipe Langages et Traducteurs.	  *
   *							  *
   ******************************************************** */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20040112 14:10 (phd):	Adaptation à G5 (faux 64 bits)		*/
/************************************************************************/
/* 20030515 14:43 (phd):	Adaptation à SGI (64 bits)		*/
/************************************************************************/
/* 20030515 14:43 (phd):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/

char WHAT[] = "@(#)mix_main.c\t- SYNTAX [unix] - Lundi 12 Janvier 2004";

static char	ME [] = "mix";


#define SX_DFN_EXT_VAR	/* Nécessaire sur G5... */
#include "sxunix.h"
#include <ctype.h>

// FILE	*sxstderr, *sxstdout, *sxtty;
// bool	sxverbosep;

struct t_lines {
    int		xline, max_line_no;
    char	**t;
};
static FILE	*old_pass, *frame;
static char	ol [132], fl [132], on [32], fn [32];
static int	oste;
static char	*old_pass_name, *frame_name;
static SXBA	is_kept;
static char	Inherited [] = "I N H E R I T E D";
static char	Derived [] = "D E R I V E D";
static char	NodeNames [] = "N O D E   N A M E S";
static char	EndNodeNames [] = "E N D   N O D E   N A M E S";
static char	Zzzz [] = "Z Z Z Z";

/*---------------*/
/*    options    */
/*---------------*/

static bool	is_c;
static char	Usage [] = "Usage:\t%s [-c | -pascal] old_pass new_skeleton\n";

#define OPTION(opt)	(1 << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB ((sizeof(option_tbl)/sizeof(*option_tbl))-1)

#define UNKNOWN_ARG 		0
#define C	 		1
#define PASCAL	 		2
#define LAST_OPTION		PASCAL

static char	*option_tbl [] = {"", "c", "pascal"};
static int	option_kind [] = {UNKNOWN_ARG, C, PASCAL};



static int	option_get_kind (char *arg)
{
    register char	**opt;

    if (*arg++ != '-')
	return UNKNOWN_ARG;

    for (opt = &(option_tbl [OPT_NB]); opt > option_tbl; opt--) {
	if (strcmp (*opt, arg) == 0 /* egalite */ )
	    break;
    }

    return option_kind [opt - option_tbl];
}



static int	read_a_line (FILE *source, char *line, int line_size, int *line_length)
{
    register int	c, ll = 0;
    register char	*lim = line + (line_size - 2);

    while ((c = getc (source)) != '\n' && c != EOF && line < lim)
	*line++ = c, ll++;

    if (lim == line)
	*line++ = c, ll++;

    *line = '\0';
    *line_length = ll;
    return c;
}



static int	is_header (char *line, char *node_name)
{
    /* recherche de :
       si is_c
       {SP|HT} "c" "a" "s" "e" {SP|HT}+ LETTER {["_"] (LETTER|DIGIT)} "_" "n" {SP|HT}+ ":"
       sinon
       {SP|HT} LETTER {["_"] (LETTER|DIGIT)} "_" "n" {SP|HT}+ ":"
    */
    register char	c;

    while ((c = *line++) == ' ' || c == '\t')
	;

    if (is_c) {
	if (c != 'c' || *line++ != 'a' || *line++ != 's' || *line++ != 'e')
	    return 0;

	if ((c = *line++) != ' ' && c != '\t')
	    return 0;

	while ((c = *line++) == ' ' || c == '\t')
	    ;
    }

    if (!isalpha (c))
	return 0;

    *node_name++ = c;

    for (; ; ) {
	c = *line++;
	*node_name++ = c;

	if (c == '_') {
	    c = *line++;
	    *node_name++ = c;

	    if (c == 'n') {
		c = *line++;

		if (isalnum (c) || c == '_') {
		    *node_name++ = c;
		    continue;
		}
		else {
		    *node_name = '\0';
		    break;
		}
	    }
	}

	if (!isalnum (c))
	    return 0;
    }

    while (c == ' ' || c == '\t')
	c = *line++;

    return c == ':';
}



static int	is_define (char *line, char *node_name)
{
    /* recherche de :
       si is_c
       "#" {SP|HT} "d" "e" "f" "i" "n" "e" {SP|HT}+ LETTER {["_"] (LETTER|DIGIT)} "_" "n" {SP|HT}+ {DIGIT}+ {SP|HT} EOL
       sinon
       {SP|HT} LETTER {["_"] (LETTER|DIGIT)} "_" "n" {SP|HT} "=" {SP|HT} {DIGIT}+ {SP|HT} ";"
    */

    register char	c;

    if (is_c && *line++ != '#')
	return 0;

    while ((c = *line++) == ' ' || c == '\t')
	;

    if (is_c) {
	if (c != 'd' || *line++ != 'e' || *line++ != 'f' || *line++ != 'i' || *line++ != 'n' || *line++ != 'e')
	    return 0;

	if ((c = *line++) != ' ' && c != '\t')
	    return 0;

	while ((c = *line++) == ' ' || c == '\t')
	    ;
    }

    if (!isalpha (c))
	return 0;

    *node_name++ = c;

    for (; ; ) {
	*node_name++ = c = *line++;

	if (c == '_') {
	    *node_name++ = c = *line++;

	    if (c == 'n') {
		c = *line++;

		if (isalnum (c) || c == '_') {
		    *node_name++ = c;
		    continue;
		}
		else {
		    *node_name = '\0';
		    break;
		}
	    }
	}

	if (!isalnum (c))
	    return 0;
    }

    if (is_c) {
	if (c != ' ' && c != '\t')
	    return 0;
    }
    else {
	while (c == ' ' || c == '\t')
	    c = *line++;

	if (c != '=')
	    return 0;
    }

    while ((c = *line++) == ' ' || c == '\t')
	;

    if (!isdigit (c))
	return 0;

    while (isdigit (c = *line++))
	;

    while (c == ' ' || c == '\t')
	c = *line++;

    return c == (is_c ? '\n' : ';');
}



static char	*next_define (struct t_lines *t_lines_ptr, int *xline, char *node)
{
    for (; *xline <= t_lines_ptr->xline; ++*xline) {
	if (is_define (t_lines_ptr->t [*xline], node))
	    return node;
    }

    return NULL;
}



static char	*skip (struct t_lines *t_lines_ptr, int *xline, char *node)
{
    for (; *xline <= t_lines_ptr->xline; ++*xline) {
	if (is_header (t_lines_ptr->t [*xline], node))
	    return node;
    }

    return strcpy (node, Zzzz);
}



static char	*read_write (struct t_lines *t_lines_ptr, int *xline, FILE *objet, char *node)
{
    for (; *xline <= t_lines_ptr->xline; ++*xline) {
	if (is_header (t_lines_ptr->t [*xline], node))
	    return node;

	fputs (t_lines_ptr->t [*xline], objet);
    }

    return strcpy (node, Zzzz);
}



static void	read_until (FILE *source, char *source_name, char *line, int line_size, char *string)
{
    int			line_lgth, string_lgth = strlen (string);
    register int	c;

    do {
	if ((c = read_a_line (source, line, line_size, &line_lgth)) == EOF) {
	    fprintf (stderr, "%s: can't find \"%s\" in file \"%s\".\n", ME, string, source_name);
	    sxexit (SXSEVERITIES - 1);
	}
    } while (c != '\n' ||
	     line_lgth < string_lgth ||
	     strncmp (line + line_lgth - string_lgth, string, string_lgth) != 0);
}



static void	copy_until (FILE *source, char *source_name, char *line, int line_size, char *string)
{
    register int	c;
    int			line_lgth, string_lgth = strlen (string);

    do {
	c = read_a_line (source, line, line_size, &line_lgth);

	if (c == EOF) {
	    fprintf (stderr, "%s: can't find \"%s\" in file \"%s\".\n", ME, string, source_name);
	    sxexit (SXSEVERITIES - 1);
	}

	if (c == '\n')
	    puts (line);
	else
	    fputs (line, stdout);
    } while (c != '\n' ||
	     line_lgth < string_lgth ||
	     strncmp (line + line_lgth - string_lgth, string, string_lgth) != 0);
}



static int	copy_until2 (FILE *source, char *source_name, char *line, int line_size, char *string1, char *string2)
{
    register int	c;
    int			line_lgth;
    register int	string1_lgth = strlen (string1);
    register int	string2_lgth = strlen (string2);

    (void) source_name; // unused

    for (;;) {
	c = read_a_line (source, line, line_size, &line_lgth);

	if (c == EOF) {
	    return 0;
	}

	if (c == '\n')
	    puts (line);
	else
	    fputs (line, stdout);

	if (c == '\n' &&
	    line_lgth >= string1_lgth &&
	    strncmp (line + line_lgth - string1_lgth, string1, string1_lgth) == 0)
	    return 1;

	if (c == '\n' &&
	    line_lgth >= string2_lgth &&
	    strncmp (line + line_lgth - string2_lgth, string2, string2_lgth) == 0) 
	    return 2;
    }
}



static void	read_lines_until (FILE *source, char *source_name, char *string, struct t_lines *t_lines_ptr)
{
    register char	*line;
    register int	string_lgth = strlen (string);
    register int	c, l, n;

    t_lines_ptr->xline = 0;
    t_lines_ptr->t = (char**) sxalloc ((t_lines_ptr->max_line_no = 32) + 1, sizeof (char*));

    do {
	if (++(t_lines_ptr->xline) > t_lines_ptr->max_line_no) {
	    t_lines_ptr->t = (char**) sxrealloc (t_lines_ptr->t, (t_lines_ptr->max_line_no <<= 1) + 1, sizeof (char*));
	}

	line = (char*) sxalloc (n = 64, sizeof (char));
	l = 0;

	do {
	    if (l >= n - 1) {
		line = (char*) sxrealloc (line, n <<= 1, sizeof (char));
	    }

	    if ((c = getc (source)) == EOF) {
		fprintf (stderr, "%s: can't find \"%s\" in file \"%s\".\n", ME, string, source_name);
		sxexit (SXSEVERITIES - 1);
	    }

	    line [l++] = c;
	} while (c != '\n');

	line [l--] = SXNUL;
	t_lines_ptr->t [t_lines_ptr->xline] = line;
    } while (l < string_lgth ||
	     strncmp (line + l - string_lgth, string, string_lgth) != 0);
}



static void	free_lines (struct t_lines *t_lines_ptr)
{
    register int	xl;

    for (xl = t_lines_ptr->xline; xl > 0; xl--) {
	sxfree (t_lines_ptr->t [xl]);
    }

    sxfree (t_lines_ptr->t);
}



static void	process (int kind, struct t_lines *frame_ptr, struct t_lines *cur_old_pass_ptr, SXBA is_kept_arg)
{
    int		xlcop, xlf;

    xlcop = xlf = 1;
    read_write (cur_old_pass_ptr, &xlcop, stdout, on);
    skip (frame_ptr, &xlf, fn);


/* On sort en premier les nouveaux noeuds */

    while (strcmp (fn, Zzzz) != 0) {
	if (!sxba_bit_is_set (is_kept_arg, sxstrretrieve (fn))) {
	    fputs (frame_ptr->t [xlf++], stdout);
	    read_write (frame_ptr, &xlf, stdout, fn);
	}
	else {
	    xlf++;
	    skip (frame_ptr, &xlf, fn);
	}
    }

    while ((strcmp (on, Zzzz) != 0)) {
	if (sxstrretrieve (on) == 0) {
	    fprintf (stderr, "****  E R A S E D  in  %s  ****\n", (kind == 1) ? Inherited : Derived);
	    fputs (cur_old_pass_ptr->t [xlcop++], stderr);
	    read_write (cur_old_pass_ptr, &xlcop, stderr, on);
	}
	else {
	    fputs (cur_old_pass_ptr->t [xlcop++], stdout);
	    read_write (cur_old_pass_ptr, &xlcop, stdout, on);
	}
    }
}



int	main (int argc, char *argv[])
{
    struct t_lines	inh_frame, der_frame, cur_old_pass, nn_lines;
    int		xline, kind;
    register int	argnum;

    sxstderr = stderr;

    if (argc < 3) {
	fprintf (sxstderr, Usage, ME);
	sxexit (SXSEVERITIES);
    }


/* valeurs par defaut */

    is_c = true;


/* Decodage des options */

    for (argnum = 1; argnum < argc - 2; argnum++) {
	switch (option_get_kind (argv [argnum])) {
	case C:
	    is_c = true;
	    break;

	case PASCAL:
	    is_c = false;
	    break;

	default:
	    fprintf (sxstderr, "%s: internal error %d while decoding options.\n", ME, option_get_kind (argv [argnum]));
	    sxexit (SXSEVERITIES);
	}
    }

    old_pass_name = argv [argc - 2];
    frame_name = argv [argc - 1];

    if ((old_pass = sxfopen (old_pass_name, "r")) == NULL) {
	fprintf (stderr, "%s: Cannot open (read) ", ME);
	perror (old_pass_name);
	sxexit (SXSEVERITIES - 1);
    }

    if ((frame = sxfopen (frame_name, "r")) == NULL) {
	fprintf (stderr, "%s: Cannot open (read) ", ME);
	perror (frame_name);
	sxexit (SXSEVERITIES - 1);
    }

    sxstr_mngr (SXBEGIN);
    copy_until (old_pass, old_pass_name, ol, 132, NodeNames);
    read_until (frame, frame_name, fl, 132, NodeNames);
    read_lines_until (frame, frame_name, EndNodeNames, &nn_lines);
    xline = 1;

    while (next_define (&nn_lines, &xline, fn) != NULL) {
	xline++;
	sxstrsave (fn);
    }

    is_kept = sxba_calloc (sxstrmngr.top + 1);

    for (xline = 1; xline <= nn_lines.xline; xline++) {
	fputs (nn_lines.t [xline], stdout);
    }

    free_lines (&nn_lines);
    read_lines_until (old_pass, old_pass_name, EndNodeNames, &nn_lines);
    xline = 1;

    while (next_define (&nn_lines, &xline, on) != NULL) {
	if ((oste = sxstrretrieve (on)) != 0) {
	    sxba_1_bit (is_kept, oste);
	}

	xline++;
    }

    free_lines (&nn_lines);
    read_until (frame, frame_name, fl, 132, Inherited);
    read_lines_until (frame, frame_name, Zzzz, &inh_frame);
    read_until (frame, frame_name, fl, 132, Derived);
    read_lines_until (frame, frame_name, Zzzz, &der_frame);

    while ((kind = copy_until2 (old_pass, old_pass_name, ol, 132, Inherited, Derived)) != 0) {
	read_lines_until (old_pass, old_pass_name, Zzzz, &cur_old_pass);
	process (kind, (kind == 1) ? &inh_frame : &der_frame, &cur_old_pass, is_kept);
	free_lines (&cur_old_pass);
    }

    free_lines (&der_frame);
    free_lines (&inh_frame);
    sxfree (is_kept);
    sxstr_mngr (SXEND);
    fclose (old_pass);
    fclose (frame);
    sxexit (0);
}
