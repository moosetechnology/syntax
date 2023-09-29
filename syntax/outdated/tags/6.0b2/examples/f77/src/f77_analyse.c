/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1986 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *  		Produit de l'equipe ChLoE.        	  *
   *			(pb)				  *
   *                                                      *
   ******************************************************** */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 27-04-92 13:33 (pb):		Decompte des "continuation lines" (pas	*/
/*				plus de 19!).
/*				Test des "character_constants" separes	*/
/*				par des blancs.				*/
/************************************************************************/
/* 24-04-92 11:05 (pb):		Le test sur la longueur des ident est	*/
/*				fait ds les actions du scanner		*/
/************************************************************************/
/* 11-02-92 14:45 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/

static char ME [] = "f77_analyse.c";

/* Ce module contient les actions et predicats lexico-syntaxiques necessaires
   a l'analyse de FORTRAN 77. */



/************************************************************************

                        EXTENSIONS TO FORTRAN77 ANSI
                 (checked only when the option -ansi is set)

- A statement must not have more than nineteen continuation lines
  A statement must contain no more than 1320=20*(72-6) characters .

- Hollerith constants outside FORMAT statements.  They are converted
   into character constants

- Character constants enclosed in double quotes.  They are converted
   into character constants

- Symbolic names are sequences of one to six letters or digits, the
   first of which must be a letter.

- Lower case letters are prohibited outside character constants or
   comments.

- An executable program must contain one main program.

- The sequence "* len_specification" occurring after a type name other
   than CHARACTER.

/************************************************************************



/************************************************************************

                     WHAT SHOULD BE CHECKED LATTER ON


- Le sequencement de statement-function-statement est a faire, sa
    reconnaissance necessite la table des symboles.

- In a control_info_list check that a format_control does not appear
    both as a single format_identifier and as "FMT= format_identifier.

- The maximum number of dimensions in an array declarator or in an array
    element name is seven.

- Only a dummy array declarator may contain an asterisk.

- A Subscript or substring expression in an EQUIVALENCE statement must
    be an integer constant expression.
  
- A <110b:label_ref> must have less than 6 digits.

- A <56:unit_identifier> must be of type integer or character, or be an
    asterisk.

- A <57:format_identifier> that is a variable name or array name must be
    of type integer or character.

- A nonzero unsigned integer constant must contain a nonzero digit.

- A label must contain a nonzero digit.

************************************************************************/

#include "sxunix.h"
#include "varstr.h"
#include "f77_td.h"
#include <ctype.h>
char WHAT_F77ANALYSE[] = "@(#)SYNTAX - $Id: f77_analyse.c 529 2007-05-09 14:26:33Z rlacroix $" WHAT_DEBUG;

/* Il faudrait INTERDIRE l'insertion d'un EOL par le rattrapage d'erreur
   du scanner. Utiliser sxs_srcvr? */


BOOLEAN		is_ansi, is_pretty_printer; /* Options d'appel */

#define UNKNOWN_p	0
#define DO_p		1
#define ASSIGN_p	2
#define CALL_p		3
#define GOTO_p		4
#define RETURN_p	5
#define PRINT_p		6
#define READ_p		7
#define BACKSPACE_p	8
#define ENDFILE_p	9

#define REWIND_p	10
#define INTEGER_p	11
#define REAL_p		12
#define DOUBLEPRECISION_p	13
#define COMPLEX_p	14
#define LOGICAL_p	15
#define CHARACTER_p	16
#define ENTRY_p		17
#define SUBROUTINE_p	18
#define BLOCKDATA_p	19

#define COMMON_p	20
#define PARAMETER_p	21
#define EXTERNAL_p	22
#define INTRINSIC_p	23
#define FUNCTION_p	24
#define CONTINUE_p	25
#define STOP_p		26
#define PAUSE_p		27
#define SAVE_p		28
#define DATA_p		29

#define IMPLICIT_p	30
#define DIMENSION_p	31
#define PROGRAM_p	32
#define ELSEIF_p	33
#define ENDIF_p		34


static char *kw_prefixes [] = {"", "DO", "ASSIGN", "CALL", "GOTO", "RETURN", "PRINT", "READ",
				   "BACKSPACE", "ENDFILE", "REWIND", "INTEGER", "REAL",
				   "DOUBLEPRECISION", "COMPLEX", "LOGICAL", "CHARACTER",
				   "ENTRY", "SUBROUTINE", "BLOCKDATA", "COMMON",
				   "PARAMETER", "EXTERNAL", "INTRINSIC", "FUNCTION",
				   "CONTINUE", "STOP", "PAUSE", "SAVE", "DATA",
				   "IMPLICIT", "DIMENSION", "PROGRAM", "ELSEIF", "ENDIF"};
static int kw_codes [] = {0, DO_t, ASSIGN_t, CALL_t, GO_t, RETURN_t, PRINT_t, READ_KW_t,
			      BACKSPACE_KW_t, ENDFILE_KW_t, REWIND_KW_t, INTEGER_t, REAL_t,
			      DOUBLE_t, COMPLEX_t, LOGICAL_t, CHARACTER_t,
			      ENTRY_t, SUBROUTINE_t, BLOCK_t, COMMON_t,
			      PARAMETER_t, EXTERNAL_t, INTRINSIC_t, FUNCTION_t,
			      CONTINUE_t, STOP_t, PAUSE_t, SAVE_t, DATA_t,
			      IMPLICIT_t, DIMENSION_t, PROGRAM_t, ELSE_t, END_t};
static int kw_lgth [] = {0, 2, 6, 4, 4, 6, 5, 4,
			     9, 7, 6, 7, 4,
			     15, 7, 7, 9,
			     5, 10, 9, 6,
			     9, 8, 9, 8,
			     8, 4, 5, 4, 4,
			     8, 9, 7, 6, 5};



struct do_item {
    int				label;
    struct sxsource_coord	source_index;
};

static struct do_item	*do_stack;
static int		do_stack_top, do_stack_size;
static int		enddo_nb;

#define PUSH(t,x)	t[x]=t[0],t[0]=x

static int		*t_stack;
static int		*iokw_stack;
static int		iokw_stack_top, iokw_stack_size;

static SHORT		buf [74], prev_source_char;
static int		comment_line_no, continuation_line_no;
struct source_item {
    SHORT			c;
    struct sxsource_coord	source_index;
};

static struct source_item	source_item;

struct source_line {
    SHORT			*buffer;
    struct sxsource_coord	*bufcoords;
    int				buflength;
    int				bufused;
};

static struct source_line	*source_line;

#define SOURCE_BUF_SIZE		4
#define INCR(x)			((x)+1 == SOURCE_BUF_SIZE ? 0 : (x)+1)

static struct source_line	source_lines [SOURCE_BUF_SIZE];
static int			source_line_head, source_line_tail;


static BOOLEAN is_tab_already_met, is_hollerith_already_met, is_quoted_string_already_met,
    is_lower_to_upper_already_met;

static char	*comment, *cur_comment;
static int	comment_lgth, cur_comment_lgth;

static BOOLEAN *Tlen;
static int Tlen_size;

static int	ETAT;
static int	*stmt_kind;
static int	main_program_nb;
static BOOLEAN	is_function_stmt, is_iokw_allowed;
static int	program_unit_kind;

static struct sxtoken	*PTOK;
static int		TOK_i;

#define MAIN_PROGRAM		1
#define FUNCTION_SUBPROGRAM	2
#define SUBROUTINE_SUBPROGRAM	3	
#define BLOCK_DATA_SUBPROGRAM	4


#define ETAT_0			0
#define ETAT_1			1
#define ETAT_2			2
#define ETAT_3			3
#define ETAT_4			4


#define format_or_entry_k	1
#define parameter_k		2
#define implicit_k		3
#define other_specification_k	4
#define data_k			5
#define executable_k		6
#define end_k			7



/* Les nouveaux "mode" d'analyse */
static struct sxparse_mode	do_parse_mode_nomess = {SXPARSER, SXWITHOUT_ERROR, 0, 0, 0, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE};
static struct sxparse_mode	format_parse_mode_nomess = {SXPARSER, SXWITH_CORRECTION, 0, 0, 0, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE};
/*
  C'est OK si on arrive a corriger. 
  new_mode.kind = SXWITH_CORRECTION;
  Decompte du nombre de corrections. 
  new_mode.error_nb = 0;
  On conserve tous les tokens 
  new_mode.look_back = 0;
  On ne parse qu'un prefixe du source 
  new_mode.is_prefixe = TRUE;
  Pas de message d'erreur 
  new_mode.is_silent = TRUE;
  On inhibe toutes les actions et predicats. 
  new_mode.with_semact = FALSE;
  new_mode.with_parsact = FALSE;
  new_mode.with_parsprdct = FALSE;
*/

static struct sxscan_mode scan_mode_nomess = {0, SXNORMAL_SCAN, TRUE, TRUE, TRUE, TRUE, TRUE};

#define ts_put(c)							\
(    (sxsvar.sxlv.ts_lgth+2 == sxsvar.sxlv_s.ts_lgth_use)			\
	? sxsvar.sxlv_s.token_string = (char *) sxrealloc (sxsvar.sxlv_s.	\
	    token_string, sxsvar.sxlv_s.ts_lgth_use *= 2, sizeof (char))	\
	: NULL,								\
     sxsvar.sxlv_s.token_string [sxsvar.sxlv.ts_lgth ++] = c		\
)



static VARSTR	vstr;

static VOID	CLEAR (T)
    register int	*T;
{
    register int	x, y;

    for (x = T [y = 0]; x > 0; x = T [y = x])
	T [y] = 0;

    T [y] = 0;
    T [0] = -1;
}



#ifdef EBUG
static VOID print_toks_buf ()
{
    register int i;
    register struct sxtoken	*pt;

    for (i = sxplocals.min; i <= sxplocals.Mtok_no ; i++) {
	pt = &(SXGET_TOKEN (i));

	fprintf (sxstdout,
		 "%d = (%s, %s, %d, %d, %s)\n",
		 i,
		 sxttext (sxsvar.sxtables, pt->lahead),
		 pt->string_table_entry > 1 ? sxstrget (pt->string_table_entry) : (pt->string_table_entry == 1 ?
		 "EMPTY_STE" : "ERROR_STE"),
		 pt->source_index.line,
		 pt->source_index.column,
		 pt->comment);
    }
}
#endif

static VOID tab_hit (source_index)
    struct sxsource_coord source_index;
{
    if (!is_tab_already_met) {
	is_tab_already_met = TRUE;
		
	sxput_error (source_index,
		     "%sA tabulation character is illegal outside comments or character constants.\n\
In this executable program, each such tabulation character is changed into one space.",
		     sxsvar.sxtables->err_titles [1] /* warning */);
    }
}



static SXVOID	comment_put (comment, comment_lgth, str, lgth)
    char	**comment;
    int		*comment_lgth;
    char	*str;
    int		lgth;
{
    if (*comment == NULL) {
	strcpy (*comment = (char*) sxalloc (lgth + 1, sizeof (char)), str);
	*comment_lgth = lgth;
    }
    else {
	strcpy ((*comment = (char*) sxrealloc (*comment, *comment_lgth +
	     lgth + 1, sizeof (char))) + *comment_lgth, str);
	*comment_lgth += lgth;
    }
}




static BOOLEAN fill_buf ()
{
    register SHORT c;
    register unsigned int x;
    register BOOLEAN	ligne_vide = FALSE, is_an_initial_line, is_header_empty;
    struct sxsource_coord source_index;

    comment_line_no = 0;

    do {
	if (ligne_vide)
	    comment_line_no++;

	/* On elimine les commentaires */
	if ((c = getc (sxsrcmngr.infile)) == 'C' || c == 'c' || c == '*') {
	    if (is_pretty_printer) {
		do {
		    varstr_catchar (vstr, '\n');
		    varstr_catchar (vstr, '\n');
		    varstr_catchar (vstr, c);

		    while ((c = getc (sxsrcmngr.infile)) != '\n' && c != EOF){
			varstr_catchar (vstr, c);
		    }

		    varstr_catchar (vstr, '\n');

		    if (c == EOF)
			break;
	    
		    comment_line_no++;

		} while ((c = getc (sxsrcmngr.infile)) == 'C' || c == 'c' || c == '*');

		varstr_complete(vstr);
	    }
	    else {
		do {
		    while ((c = getc (sxsrcmngr.infile)) != '\n' && c != EOF);
	    
		    if (c == EOF)
			break;
	    
		    comment_line_no++;
		} while ((c = getc (sxsrcmngr.infile)) == 'C' || c == 'c' || c == '*');
	    }
	}
	
	x = 0;
	is_header_empty = TRUE;

	/* on remplit buf */
	while (c != '\n' && c != EOF && x < 5) {
	    if (isdigit (c)) {
		is_header_empty = FALSE;
	    }
	    else if (c != ' ') {
		source_index.file_name = source_item.source_index.file_name;
		source_index.line = source_item.source_index.line + comment_line_no + 1;
		source_index.column = x + 1;

		if (c == '\t') {
		    tab_hit (source_index);
		}
		else {
		    sxput_error (source_index,
				 "%sThe illegal character \"%s\" is changed into blank.",
				 sxsvar.sxtables->err_titles [2] /* error */,
				 CHAR_TO_STRING (c));
		}

		c = ' ';
	    }

	    buf [++x] = c;
	    c = getc (sxsrcmngr.infile);
	}

	ligne_vide = is_header_empty;

	while (c != '\n' && c != EOF && x < 72) {
	    if ((buf [++x] = c) != ' ')
		ligne_vide = FALSE;
	    
	    c = getc (sxsrcmngr.infile);
	}

	/* On cherche la fin de ligne */
	while (c != '\n' && c != EOF)
	    c = getc (sxsrcmngr.infile);

    } while (ligne_vide && c != EOF);
    
    /* On complete l'entete... */
    while (x < 6)
	buf [++x] = ' ';
    
    buf [++x] = c; /* EOL ou EOF */

    if ((c = buf [6]) == '0')
	/* debut de carte instruction */
	c = buf [6] = ' ';

    if (is_an_initial_line = c == ' ')
	continuation_line_no = 0;
    else {
	if (++continuation_line_no == 20 && is_ansi) {
	    source_index.file_name = source_item.source_index.file_name;
	    source_index.line = source_item.source_index.line + comment_line_no + 1;
	    source_index.column = 6;
	    sxput_error (source_index,
			 "%sContinuation line number exceeded.",
			 sxsvar.sxtables->err_titles [1] /* Warning */);
	}
    }

    /* ligne initiale, on verifie que les colonnes 1 a 5 ne contiennent
       que des blancs ou des chiffres */
    /* ligne suite, on verifie que les colonnes 1 a 5 ne contiennent
       que des blancs */

    if (!(is_header_empty || is_an_initial_line)) {
	for (x = 1; x < 6;x++) {
	    if (buf [x] != ' ') {
		source_index.file_name = source_item.source_index.file_name;
		source_index.line = source_item.source_index.line + comment_line_no + 1;
		source_index.column = x;

		sxput_error (source_index,
			     "%sA label is not allowed in a continuation line, it is erased.",
			     sxsvar.sxtables->err_titles [2] /* error */);

		do {
		    buf [x] = ' ';
		} while (++x < 6);
	    }
	}
    }
    
    if (is_pretty_printer) {
	if (is_an_initial_line) {
	    if (comment == NULL) {
		comment = cur_comment;
		comment_lgth = cur_comment_lgth;
	    }
	    else if (cur_comment != NULL) {
		/* Au cas ou... */
		comment_put (&comment, &comment_lgth, cur_comment, cur_comment_lgth);
		sxfree (cur_comment);
	    }

	    cur_comment = NULL;
	    cur_comment_lgth = 0;
	}

	if (varstr_length (vstr) > 0) {
	    comment_put (&cur_comment, &cur_comment_lgth, varstr_tostr (vstr), varstr_length (vstr));
	    varstr_raz (vstr);
	}
    }

    return is_an_initial_line;
}


static VOID get_next_source_item ()
{
    if (prev_source_char == EOF)
	return;

    if (prev_source_char == '\n')
	source_item.source_index.line += comment_line_no + 1, source_item.source_index.column = 0;

    for (;;) {
	source_item.c = buf [++source_item.source_index.column];

	if (source_item.c == '\n') {
	    if (fill_buf ())
		break;
	    
	    source_item.source_index.column = 6;
	    source_item.source_index.line += comment_line_no + 1;
	    continue;
	}

	break;
    }

    prev_source_char = source_item.c;
}


static VOID put_source_item ()
{
    if (++source_line->bufused >= source_line->buflength) {
	source_line->buffer = (SHORT*) sxrealloc (source_line->buffer, source_line->buflength *= 2, sizeof (SHORT));
	source_line->bufcoords = (struct sxsource_coord*) sxrealloc (source_line->bufcoords, source_line->buflength,
								  sizeof (struct sxsource_coord));
    }

    source_line->buffer [source_line->bufused] = source_item.c;
    source_line->bufcoords [source_line->bufused] = source_item.source_index;
}

static VOID complete (c, col)
    SHORT	c;
    int col;
{
    /* On ajoute des "car" jusqu'a la colonne "col" */
    /* On ne change pas les source_index. */
    SHORT	prev_c;
    register int nb;
    
    if ((nb = col - source_item.source_index.column) >= 0) {
	prev_c = source_item.c;
	source_item.c = c;

	do {
	    put_source_item ();
	} while (--nb >= 0);
	    
	source_item.c = prev_c;
    }
}

#define is_next_to(si1,si2)	(si1.line == si2.line && si1.column + 1 == si2.column || \
				 si1.column == 72 && si2.column == 7)


static VOID fill_buffer ()
{
    /* fabrique a partir du source une carte etendue (tableau d'items) qui
       a ete debarassee de tous les caracteres inutiles (et seulement ceux la).
       Tient compte des chaines (string ou hollerith). */
    SHORT		c, prev_c;
    struct sxsource_coord	prev_source_index;
    int			hollerith_lgth;

    prev_c = '\n';
    /* On change de buffer */

    if ((source_line_head = INCR (source_line_head)) == source_line_tail)
	/* On raz le suivant. */
	source_lines [source_line_tail = INCR (source_line_tail)].bufused = 0;

    source_line = &(source_lines [source_line_head]);

    if (source_line->bufused == 0) {
	/* Buffer vide */

    goto init;

 STATE1:
    /* etat principal, source_item est positionne. Ce n'est pas ni EOL ni EOF */
    prev_c = source_item.c;
    put_source_item ();

 init:
    get_next_source_item ();

 STATE0:
    while (source_item.c == ' ' || source_item.c == '\t') {
	if (source_item.c == '\t')
	    tab_hit (source_item.source_index);

	get_next_source_item ();
    }

    if (source_item.c == '\"')
	/* On entre dans une chaine debutant par une double quote */
	goto STATE2;

    if (source_item.c == '\'')
	/* On entre dans une chaine debutant par une simple quote */
	goto STATE3;

    if (source_item.c == '\n' || source_item.c == EOF)
	goto STATE6;

    if (isdigit (source_item.c) &&
	source_item.source_index.column > 6 &&
	!(isalnum (prev_c) || prev_c == '*')) {
	hollerith_lgth = 0;
	goto STATE4;
    }

    goto STATE1;
	

 STATE2:
    /* Traitement des chaines debutant par une double quote */
    put_source_item ();
    prev_source_index = source_item.source_index;
    get_next_source_item ();

    if (source_item.c == '\n' || source_item.c == EOF) {
	sxput_error (source_item.source_index,
		     "%sIllegal EOL in this character constant\n\
an double quote character is inserted.",
		     sxsvar.sxtables->err_titles [2] /* error */);

	if (source_item.source_index.column < 73) {
	    complete (' ', 71);
	}

	c = source_item.c;
	source_item.c = '\"';
	put_source_item ();
	source_item.c = c;
	goto STATE6;
    }

    if (prev_source_index.line < source_item.source_index.line) {
	/* On a change de ligne dans une chaine, on complete par des blancs. */
	complete (' ', 72);
    }

    if (source_item.c == '\"') {
	/* Fin de la chaine ou deux double quote. */
	prev_source_index = source_item.source_index;
	put_source_item ();
	prev_c = source_item.c;
	get_next_source_item ();
	
	while (source_item.c == ' ' || source_item.c == '\t') {
	    if (source_item.c == '\t')
		tab_hit (source_item.source_index);

	    prev_c = source_item.c;
	    get_next_source_item ();
	}

	if (source_item.c == '\"') {
	    if (!is_next_to (prev_source_index, source_item.source_index))
		sxput_error (source_item.source_index,
			     "%sThis illegal character constant sequence is ignored.",
			     sxsvar.sxtables->err_titles [2] /* error */);

	    goto STATE2;
	}
	
	goto STATE1;
    }

    goto STATE2;


 STATE3:
    /* Traitement des chaines debutant par une simple quote */
    put_source_item ();
    prev_source_index = source_item.source_index;
    get_next_source_item ();

    if (source_item.c == '\n' || source_item.c == EOF) {
	sxput_error (source_item.source_index,
		     "%sIllegal EOL in this character constant\n\
an apostrophe is inserted.",
		     sxsvar.sxtables->err_titles [2] /* error */);

	if (source_item.source_index.column < 73) {
	    complete (' ', 71);
	}

	c = source_item.c;
	source_item.c = '\'';
	put_source_item ();
	source_item.c = c;
	goto STATE6;
    }

    if (prev_source_index.line < source_item.source_index.line) {
	/* On a change de ligne dans une chaine, on complete par des blancs. */
	complete (' ', 72);
    }

    if (source_item.c == '\'') {
	/* Fin de la chaine ou double apostrophe. */
	prev_source_index = source_item.source_index;
	put_source_item ();
	prev_c = source_item.c;
	get_next_source_item ();
	
	while (source_item.c == ' ' || source_item.c == '\t') {
	    if (source_item.c == '\t')
		tab_hit (source_item.source_index);

	    prev_c = source_item.c;
	    get_next_source_item ();
	}

	if (source_item.c == '\'') {
	    if (!is_next_to (prev_source_index, source_item.source_index))
		sxput_error (source_item.source_index,
			     "%sThis illegal character constant sequence is ignored.",
			     sxsvar.sxtables->err_titles [2] /* error */);

	    goto STATE3;
	}
	
	goto STATE1;
    }

    goto STATE3;


 STATE4:
    /* Debut possible de zone Hollerith */
    hollerith_lgth *= 10;
    hollerith_lgth += source_item.c - '0';
    put_source_item ();
    get_next_source_item ();

    while (source_item.c == ' ' || source_item.c == '\t') {
	if (source_item.c == '\t')
	    tab_hit (source_item.source_index);

	get_next_source_item ();
    }

    if (source_item.c == '\n' || source_item.c == EOF)
	goto STATE6;

    if (source_item.c == 'H')
	goto STATE5;

    if (source_item.c == 'h') {
	source_item.c = 'H';
	goto STATE5;
    }

    if (isdigit (source_item.c))
	goto STATE4;

    if (source_item.c == '\"')
	/* On entre dans une chaine debutant par une double quote */
	goto STATE2;

    if (source_item.c == '\'')
	/* On entre dans une chaine debutant par une simple quote */
	goto STATE3;

    goto STATE1;


 STATE5:
    /* Debut de zone Hollerith de longueur hollerith_lgth */
    put_source_item ();

    if (hollerith_lgth == 0)
	sxput_error (source_item.source_index,
		     "%sIllegal empty Hollerith string.",
		     sxsvar.sxtables->err_titles [2] /* error */);

    get_next_source_item ();

    while (hollerith_lgth-- > 0) {
	if (source_item.c == '\n' || source_item.c == EOF) {
	    register int nb;

	    if ((nb = 73 - source_item.source_index.column) > 0) {
		/* On repousse artificiellement la fin de carte en ajoutant
		   des blancs. */
		c = source_item.c;
		source_item.c = ' ';

		do {
		    put_source_item ();
		    nb--;
		} while (hollerith_lgth-- > 0 && nb > 0);

		source_item.c = c;
	    }

	    break;
	}
	else {
	    put_source_item ();
	    get_next_source_item ();
	}
    }

    if (hollerith_lgth >= 0) {
	sxput_error (source_item.source_index,
		     "%sIllegal EOL in this Hollerith constant\n\
which has been truncated.",
		     sxsvar.sxtables->err_titles [2] /* error */);

	goto STATE6;
    }


    /* A ce niveau, permet-t-on une zone Hollerith deriere une zone hollerith? */
    prev_c = ' '; /* oui ! */
    goto STATE0;

    
 STATE6:
    put_source_item ();
    }
    
    /* Le buffer courant devient celui du source_mngr. */
    sxsrcmngr.buffer = source_line->buffer;
    sxsrcmngr.bufcoords = source_line->bufcoords;
    sxsrcmngr.buflength = source_line->buflength;
    sxsrcmngr.bufused = source_line->bufused;
}


static int cmp_si (si1, si2)
    register struct sxsource_coord *si1, *si2;
{
    /* retourne -1, 0 ou 1 suivant que l'on a
       si1 < si2, si1 == si2, si1 > si2 */

    if (si1->line < si2->line)
	return -1;

    if (si1->line > si2->line)
	return 1;

    if (si1->column < si2->column)
	return -1;

    if (si1->column > si2->column)
	return 1;

    return 0;
}


static int get_buffer_nb (coord)
    register struct sxsource_coord *coord;
{
    register int		i;
    register BOOLEAN		head_hit;
    register struct source_line	*source_line;
    /* Recherche dans source_lines l'index du buffer dans lequel se trouve coord. */

    /* On recherche entre tail + 1 et head */

    i = INCR (source_line_tail);
    head_hit = FALSE;

    do {
	source_line = &(source_lines [i]);

	if (source_line->bufused != 0) {
	    if (cmp_si (source_line->bufcoords + 1, coord) <= 0 &&
		cmp_si (coord, source_line->bufcoords + source_line->bufused) <= 0)
	    return i;
	}

	i = INCR (i);

	if (i == source_line_head)
	    head_hit = TRUE;
    } while (i == source_line_head || !head_hit);

    sxtrap (ME, "get_buffer_nb");
}



static int get_buffer_index (nb, coord, delta)
    int	nb, delta;
    struct sxsource_coord *coord;
{
    register int			i;
    register struct sxsource_coord	*bufcoords = source_lines [nb].bufcoords ;

    i = 1;

    while (cmp_si (bufcoords + i, coord) < 0)
	i++;

    if ((i += delta) <= source_lines [nb].bufused)
	return i;

    sxtrap (ME, "get_buffer_index");
}


static struct sxsource_coord *get_source_index (coord, delta)
    struct sxsource_coord *coord;
    int delta;
{
    register int nb, index;

    nb = get_buffer_nb (coord);
    index = get_buffer_index (nb, coord, delta);
    return &(source_lines [nb].bufcoords [index]);
}




/* CLONE du SOURCE_MANAGER */

#ifndef	LOCALBUFLENGTH
#	define	LOCALBUFLENGTH	256	/* Smallest buffer allocated */
#endif



/* Acces au caractere suivant */

SHORT	f77_next_char ()
{
    sxsrcmngr.previous_char = sxsrcmngr.current_char;

    if (sxsrcmngr.bufindex == sxsrcmngr.bufused) {
	fill_buffer ();
	sxsrcmngr.bufindex = 0;
    }

    sxsrcmngr.source_coord = sxsrcmngr.bufcoords [++sxsrcmngr.bufindex];
    return sxsrcmngr.current_char = sxsrcmngr.buffer [sxsrcmngr.bufindex];
}




/* Acces au caractere en avance suivant */

SHORT	f77_lanext_char ()
{
    if (sxsrcmngr.labufindex == sxsrcmngr.bufused) {
	/* On retourne tout le temps EOL! */
	/* Il ne faut pas lire de nouveau buffer. */
	sxsrcmngr.labufindex--;
    }

    return sxsrcmngr.buffer [++sxsrcmngr.labufindex];

}




/* Acces au premier caractere en avance */

SHORT	f77_lafirst_char ()
{
    sxsrcmngr.labufindex = sxsrcmngr.bufindex;
    return f77_lanext_char ();
}




/* Recul d'un caractere: le scanner veut inserer un caractere devant */

VOID	f77_X (inserted)
    SHORT	inserted;

/* On est forcement dans le cas ou le buffer est valide, puisqu'une	*/
/* correction d'erreur impose un look-ahead prealable.  Par ailleurs,	*/
/* cette procedure n'est jamais appelee deux fois de suite, puisqu'une	*/
/* correction d'erreur est censee etre correcte...			*/

{
    sxsrcmngr.bufindex--;
    sxsrcmngr.current_char = inserted;
}




/* Retour en arriere dans la lecture en avance */

VOID	f77_laback (backward_number)
    int		backward_number;

/* Une erreur a ete detectee et corrigee en look-ahead.  Celui-ci doit	*/
/* reprendre plus haut.  "backward_number" est cense etre valide	*/
/* vis-a-vis de la situation dans le "buffer".				*/

{
    sxsrcmngr.labufindex -= backward_number;
}


/* Non utilise ds FORTRAN */
VOID	f77_srcpush ()
{
    sxtrap (ME, "f77_srcpush");
}

/* Au cas ou... */
#if 0
/* Push a string ahead of the current character */

VOID	f77_srcpush (previous_char, chars, coord)
    SHORT	previous_char;
    register char	*chars;
    struct sxsource_coord	coord;

/* On traite une action "@Release".  Le "source manager" est dans un */
/* etat quelconque.  En particulier "buffer" peut exister ou non...  */

{
    register int	length = strlen (chars);

    /* Must keep its contents in the right place */
    
    /* First see if it is big enough */
{
    register int	minlength;
    
    minlength = (sxsrcmngr.bufused - sxsrcmngr.bufindex + 1) + length;
    
    if (sxsrcmngr.buflength < minlength)
	sxsrcmngr.buffer = (SHORT*) sxrealloc (sxsrcmngr.buffer, sxsrcmngr.buflength = minlength, sizeof (SHORT)), sxsrcmngr.bufcoords = (struct sxsource_coord*) sxrealloc (sxsrcmngr.bufcoords, sxsrcmngr.buflength, sizeof (struct sxsource_coord));
}


/* If there is not room enough, push the contents to the right */

if (sxsrcmngr.bufindex < length) {
    register SHORT	*buf1, *buf2;
    register struct sxsource_coord	*coords1, *coords2;
    register int	count;
    
    count = sxsrcmngr.bufused - sxsrcmngr.bufindex;
    buf1 = &sxsrcmngr.buffer [sxsrcmngr.bufused], coords1 = &sxsrcmngr.bufcoords [sxsrcmngr.bufused];
    buf2 = &sxsrcmngr.buffer [sxsrcmngr.bufused = sxsrcmngr.buflength - 1], coords2 = &sxsrcmngr.bufcoords [
													    sxsrcmngr.bufused];
    sxsrcmngr.bufindex = sxsrcmngr.bufused - count;
    
    while (count-- != 0)
	*buf2-- = *buf1--, *coords2-- = *coords1--;
}

/* Now copy the last char returned, preceded by the "chars" */

sxsrcmngr.buffer [sxsrcmngr.bufindex] = sxsrcmngr.current_char, sxsrcmngr.bufcoords [sxsrcmngr.bufindex] = sxsrcmngr.
    source_coord;

{
    register SHORT	*buf;
    register struct sxsource_coord	*coords;
    
    buf = &sxsrcmngr.buffer [sxsrcmngr.bufindex -= length], coords = &sxsrcmngr.bufcoords [sxsrcmngr.bufindex];
    
    while (length-- != 0) {
	*coords++ = coord;
	
	if ((*buf++ = *chars++) != NEWLINE)
	    coord.column++;
	else
	    coord.line++, coord.column = 1;
    }
}


/* And lastly update what is seen from the outside */

    sxsrcmngr.previous_char = previous_char;
    sxsrcmngr.current_char = sxsrcmngr.buffer [sxsrcmngr.bufindex], sxsrcmngr.source_coord = sxsrcmngr.bufcoords [
	 sxsrcmngr.bufindex];
    source_line->buffer = sxsrcmngr.buffer;
    source_line->bufcoords = sxsrcmngr.bufcoords;
    source_line->buflength = sxsrcmngr.buflength;
    source_line->bufused = sxsrcmngr.bufused;
}
#endif



/*VARARGS1*/

VOID	f77_src_mngr (what, infile, file_name)
    int		what;
    FILE	*infile;
    char	*file_name;
{
    register int i;

    switch (what) {
    case INIT:
	comment = NULL;

	if (is_pretty_printer) {
	    vstr = varstr_alloc (128);
	}

	for (i = 0; i < SOURCE_BUF_SIZE; i++) {
	    source_lines [i].buffer = (SHORT*) sxalloc (source_lines [i].buflength = LOCALBUFLENGTH, sizeof (SHORT));
	    source_lines [i].bufcoords = (struct sxsource_coord*) sxalloc (LOCALBUFLENGTH, sizeof (struct sxsource_coord));
	    source_lines [i].bufused = 0;
	}

	source_line_head = source_line_tail = SOURCE_BUF_SIZE - 1;

	prev_source_char = '\n';
	source_item.source_index.file_name = file_name;
	source_item.source_index.line = 0;
	source_item.source_index.column = 0;

	sxsrcmngr.infile = infile; /* Pour les getc */

	if (!fill_buf ()) {
	    /* la premiere ligne du programme est une carte suite, c,a commence mal! */
	    buf [6] = ' ';
	    source_item.source_index.line = comment_line_no + 1;
	    source_item.source_index.column = 6;
	    sxput_error (source_item.source_index,
			 "%sA program cannot start with a continuation line.",
			 sxsvar.sxtables->err_titles [2] /* error */);
	}

	/* On lit la premiere ligne logique */
	fill_buffer ();
	sxsrcmngr.source_coord.file_name = file_name;
	sxsrcmngr.source_coord.line = sxsrcmngr.source_coord.column = 0;
	sxsrcmngr.previous_char = EOF;
	sxsrcmngr.current_char = NEWLINE;
	sxsrcmngr.buffer [0] = sxsrcmngr.current_char;
	sxsrcmngr.bufcoords [0] = sxsrcmngr.source_coord;
	sxsrcmngr.bufindex = sxsrcmngr.labufindex = 0;
	sxsrcmngr.has_buffer = TRUE;

	break;

    case FINAL:
	if (is_pretty_printer) {
	    varstr_free (vstr);
	}

	for (i = 0; i < SOURCE_BUF_SIZE; i++) {
	    sxfree (source_lines [i].buffer), source_lines [i].buffer = NULL;
	    sxfree (source_lines [i].bufcoords) ,source_lines [i].bufcoords = NULL;
	}

	break;
    }
}



static char *skip_an_int (str)
    char 	*str;
{
    /* saute DIGIT */
    /* Cette chaine est terminee par NUL */
    register char c;

    c = *str;
    
    while (isdigit (c)) {
	c = *++str;
    };

    return str;
}

static int get_int_val (str)
    char 	*str;
{
    /* pointe sur un chiffre */
    /* calcule la valeur de DIGIT+ */
    /* Cette chaine est terminee par NUL */
    register char	c;
    register int	val = 0;
    
    while (c = *str, isdigit (c)) {
	str++;
	val *=10;
	val += c - '0';
    } 

    return val;
}

static VOID do_stack_mngr (str, source_index)
    register char *str;
    struct sxsource_coord	*source_index;
{
    register int	val;

    if ((val = get_int_val (str)) == 0)
	sxput_error (*source_index,
		     "%sIllegal null label in a do-statement.",
		     sxsvar.sxtables->err_titles [2] /* error */);
				
    if (++do_stack_top > do_stack_size)
	do_stack = (struct do_item*) sxrealloc (do_stack, (do_stack_size += 10) + 1, sizeof (struct do_item));
				
    do_stack [do_stack_top].label = val;
    do_stack [do_stack_top].source_index = *source_index;
}

static BOOLEAN check_balanced (tok_no)
    int	*tok_no;
{
    /* Cette fonction verifie si la sequence de tokens tok_no+1 ...tok_no+n est
       une structure bien parenthesee dans une carte logique.
       Dans tous les cas retourne dans tok_no le numero du dernier token examine.
       C'est la derniere parenthese fermante s'il y a eu succes. */
    register int count, lahead;

    if (sxget_token (++*tok_no)->lahead != LEFTP_t)
	return FALSE;

    count = 1;

    while (count > 0) {
	lahead = sxget_token (++*tok_no)->lahead;

	if (lahead == RIGHTP_t)
	    count--;
	else if (lahead == LEFTP_t)
	    count++;
	else if (lahead == EOL_t)
	    return FALSE;
    }

    return TRUE;
}


static int seek_kw (string)
    register char *string;
{
    /* string est le texte d'un ident en debut d'instruction. On regarde si un element de kw_prefixes
       est un prefixe de string. */
    register int i = UNKNOWN_p;
    register char c;

    switch (*string) {
    case 'A': /* ASSIGN */
	i = ASSIGN_p;
	break;

    case 'B': /* BACKSPACE ou BLOCKDATA */
	if (strlen (string) > 8) {
	    c = string [1];
	    i = c == 'A' ? BACKSPACE_p : BLOCKDATA_p;
	}

	break;

    case 'C': /* CALL ou COMPLEX ou CHARACTER ou COMMON */
	if (strlen (string) > 3) {
	    c = string [3];
	    i = c == 'L' ? CALL_p : (c == 'P' ? COMPLEX_p : (c == 'R' ? CHARACTER_p : COMMON_p));
	}

	break;

    case 'D': /* DO ou DOUBLEPRECISION ou DIMENSION or DATA */
	if (strlen (string) > 3) {
	    c = string [2];
	    i = c == 'U' ? DOUBLEPRECISION_p : (c == 'M' ? DIMENSION_p : (c == 'T' ? DATA_p : DO_p));
	}

	break;

    case 'E': /* ENDFILE or ENTRY or EXTERNAL ou ELSEIF ou ENDIF */
	if (strlen (string) > 4) {
	    c = string [3];
	    i = c == 'F' ? ENDFILE_p :
		(c == 'R' ? ENTRY_p :
		 (c == 'E' ? (string [1] == 'X' ? EXTERNAL_p : ELSEIF_p) :
		  ENDIF_p));
	}

	break;

    case 'F': /* FUNCTION */
	i = FUNCTION_p;
	break;

    case 'G': /* GOTO */
	i = GOTO_p;
	break;

    case 'I': /* INTEGER or IMPLICIT or INTRINSIC */
	if (strlen (string) > 6) {
	    c = string [3];
	    i = c == 'E' ? INTEGER_p : (c == 'L' ? IMPLICIT_p : INTRINSIC_p);
	}

	break;

    case 'L': /* LOGICAL */
	i = LOGICAL_p;
	break;

    case 'P': /* PRINT or PAUSE or PROGRAM */
	if (strlen (string) > 4) {
	    c = string [2];
	    i = c == 'I' ? PRINT_p : (c == 'U' ? PAUSE_p : PROGRAM_p);
	}

	break;

    case 'R': /* READ ou REWIND ou RETURN ou REAL */
	if (strlen (string) > 3) {
	    c = string [3];
	    i = c == 'D' ? READ_p : (c == 'I' ? REWIND_p : (c == 'U' ? RETURN_p : REAL_p));
	}

	break;

    case 'S': /* STOP or SAVE or SUBROUTINE */
	if (strlen (string) > 3) {
	    c = string [1];
	    i = c == 'T' ? STOP_p : (c == 'A' ? SAVE_p : SUBROUTINE_p);
	}

	break;

    default:
	return UNKNOWN_p;

    }

    return i == UNKNOWN_p ? UNKNOWN_p :
	((strncmp (kw_prefixes [i], string, kw_lgth [i]) == 0) ? i : UNKNOWN_p);
}



#define change_tok(tok_no,la,name)	\
        PTOK = &(SXGET_TOKEN (tok_no)),	\
        PTOK->lahead = la,		\
	PTOK->string_table_entry = sxstrsave (name)
	    


static BOOLEAN check_for_a_lhs (tok_no)
    int tok_no;
{
    /* tok_no designe le token (ident ou mot_cle) qui se trouve en debut d'une
       instruction.  On regarde si ce token est l'ident d'une partie gauche
       d'affectation. */
    register int lahead;

    if ((lahead = sxget_token (tok_no + 1)->lahead) == EGAL_t)
	/* variable name */
	return TRUE;

    switch (SXGET_TOKEN (tok_no).lahead) {
    case BACKSPACE_t:
    case CLOSE_t:
    case ENDFILE_t:
    case INQUIRE_t:
    case OPEN_t:
    case READ_t:
    case REWIND_t:
    case WRITE_t:
	is_iokw_allowed = TRUE;
	break;

    default:
	break;

    }

    if (!check_balanced (&tok_no)) {
	is_iokw_allowed = FALSE;
	return FALSE;
    }

    is_iokw_allowed = FALSE;

    if ((lahead = sxget_token (tok_no + 1)->lahead) == EGAL_t)
	/* array element name */
	return TRUE;

    if (!check_balanced (&tok_no))
	return FALSE;

    return sxget_token (tok_no + 1)->lahead == EGAL_t; /* substring name */
}


static SXVOID check_id_lgth (tt)
    struct sxtoken	*tt;
{
    /* is_ansi est vrai et tt refere un ident. */
    /* Verification de la longueur des id. */

    register int ste = tt->string_table_entry;

    if (sxstrlen (ste) > 6) {
	if (Tlen == NULL)
	    Tlen = (BOOLEAN*) sxcalloc ((Tlen_size = SXSTRtop ()) + 1, sizeof (BOOLEAN));
	else if (ste > Tlen_size) {
	    register BOOLEAN	*low = Tlen + Tlen_size, *high;

	    while (ste > Tlen_size)
		Tlen_size *= 2;

	    Tlen = (BOOLEAN*) sxrealloc (Tlen, Tlen_size + 1, sizeof (BOOLEAN));
	    high = Tlen + Tlen_size;

	    while (++low <= high)
		*low = FALSE;
	}

	if (!Tlen [ste]) {
	    Tlen [ste] = TRUE;

	    sxput_error (tt->source_index,
			 "%sIn the standard, a symbolic name is a sequence of one to\n\
six letters or digits, the first of which must be a letter. This name exceeds this limit.",
			 sxsvar.sxtables->err_titles [1] /* warning */);
	}
    }
}

static BOOLEAN check_FUNCTION (str, strlgth, l, Mtok_no)
    int		l, strlgth, Mtok_no;
    char	*str;
{
    /* On est sur un type en debut d'instruction,
       on regarde si ce token est suivi par FUNCTION id. */
    struct sxtoken	tt [2];
    int	l1 = l+8;

    if (strlgth > l1 && strncmp (str + l, "FUNCTION", 8) == 0 && isalpha (str [l1])) {
	PTOK = &(SXGET_TOKEN (Mtok_no));
	PTOK->string_table_entry = EMPTY_STE;
	tt [0].lahead = FUNCTION_t;
	tt [0].string_table_entry = EMPTY_STE;
	tt [0].source_index =
	    *get_source_index (&(PTOK->source_index), l);
	tt [0].comment = NULL;
	tt [1].lahead = ID_t;
	tt [1].string_table_entry = sxstrsave (str + l1);
	tt [1].source_index =
	    *get_source_index (&(PTOK->source_index), l1);
	tt [1].comment = NULL;

	if (is_ansi)
	    check_id_lgth (&(tt [1]));

	sxtknmdf (&(tt [0]), 2, Mtok_no + 1, 0);
	return TRUE;
    }

    return FALSE;
}



static VOID process_initial_id (Mtok_no, after_if_log)
    int		Mtok_no;
    BOOLEAN	after_if_log;
{
    /* L'ident situe en Mtok_no est en debut d'instruction.
       C'est le token ID_t.
       On sait que ce n'est pas (le debut d') une lhs.
       On verifie s'il ne s'agit pas de la concatenation d'un mot_cle avec un suffixe adequat. */
    /* Si ce n'est pas le cas, on ne change rien et on laisse le rattrapage d'erreur agir. */
    register char	*str, *str1;
    register int	lahead;
    int			tok_no, strlgth, kw, l;
    struct sxsource_coord si1, si2;
    struct sxtoken	tt [3];

    PTOK = &(SXGET_TOKEN (Mtok_no));
    str = sxstrget (PTOK->string_table_entry);
    strlgth = sxstrlen (PTOK->string_table_entry);

    switch (kw = seek_kw (str)) {
	
    case ASSIGN_p :
	tok_no = Mtok_no;
	str += 6;
	
	if ((str1 = skip_an_int (str)) > str) {
	    /* Assign entier ... */
	    if (str1 [0] == 'T' && str1 [1] == 'O' && isalpha (str1 [2])) {
		si1 = *get_source_index (&(PTOK->source_index), 6);
		si2 = *get_source_index (&(PTOK->source_index), 8 + str1 - str);
		
		if (sxget_token (++tok_no)->lahead == EOL_t) {
		    PTOK->lahead = ASSIGN_t;
		    PTOK->string_table_entry = EMPTY_STE;
		    tt [0].lahead = ENTIER_t;
		    tt [0].string_table_entry = sxstr2save (str, str1 - str);
		    tt [0].source_index =
			*get_source_index (&(PTOK->source_index), 6);
		    tt [0].comment = NULL;
		    tt [1].lahead = TO_t;
		    tt [1].string_table_entry = EMPTY_STE;
		    tt [1].source_index =
			*get_source_index (&(PTOK->source_index), 6 + str1 - str);
		    tt [1].comment = NULL;
		    tt [2].lahead = ID_t;
		    tt [2].string_table_entry = sxstrsave (str1 + 2);
		    tt [2].source_index =
			*get_source_index (&(PTOK->source_index), 8 + str1 - str);
		    tt [2].comment = NULL;

		    if (is_ansi)
			check_id_lgth (&(tt [2]));

		    sxtknmdf (&(tt [0]), 3, Mtok_no + 1, 0);
		}
	    }
	}
	
	break;
	
    case BLOCKDATA_p :
	/* On retourne 2 tokens pour BLOCK DATA  */
	lahead = DATA_t;
	l = 5;

insert_a_token:	
	PTOK->lahead = kw_codes [kw];
	PTOK->string_table_entry  = EMPTY_STE;

	tt [0].lahead = lahead;
	tt [0].string_table_entry = EMPTY_STE;
	tt [0].source_index =
	    *get_source_index (&(PTOK->source_index), l);
	tt [0].comment = NULL;
	sxtknmdf (&(tt [0]), 1, ++Mtok_no, 0);
	/* l doit indiquer le decalage depuis le debut de Mtok_no. */
	str += l;
	l = kw_lgth [kw] - l;
	goto extract_id_int;
	
    case ELSEIF_p:
	/* On retourne 2 tokens pour ELSE IF  */
	lahead = IF_t;
	l = 4;
	goto insert_a_token;

    case ENDIF_p:
	/* On retourne 2 tokens pour END IF  */
	lahead = IF_t;
	l = 3;
	goto insert_a_token;

    case GOTO_p :
	/* On retourne 2 tokens pour GO TO  */
	lahead = TO_t;
	l = 2;
	goto insert_a_token;

    case CHARACTER_p:
    case COMPLEX_p:
    case DOUBLEPRECISION_p:
    case INTEGER_p:
    case LOGICAL_p:
    case REAL_p:
	PTOK->lahead = kw_codes [kw];
	PTOK->string_table_entry  = EMPTY_STE;

	if (kw == DOUBLEPRECISION_p) {
	    /* On retourne 2 tokens pour DOUBLE PRECISION  */
	    tt [0].lahead = PRECISION_t;
	    tt [0].string_table_entry = EMPTY_STE;
	    tt [0].source_index =
		*get_source_index (&(PTOK->source_index), 6);
	    tt [0].comment = NULL;
	    sxtknmdf (&(tt [0]), 1, ++Mtok_no, 0);
	    /* l doit indiquer le decalage depuis le debut de Mtok_no. */
	    str += 6;
	    strlgth -= 6;
	    l = 9;
	}
	else
	    l = kw_lgth [kw];
	
	if (check_FUNCTION (str, strlgth, l, Mtok_no)) {
	    is_function_stmt = TRUE;
	    break;
	}
	
	goto extract_id_int;
	
    case FUNCTION_p :
	is_function_stmt = TRUE;

    case BACKSPACE_p :
    case CALL_p :
    case COMMON_p :
    case DATA_p:
    case DIMENSION_p:
    case ENDFILE_p :
    case ENTRY_p :
    case EXTERNAL_p:
    case INTRINSIC_p:
    case PAUSE_p:
    case PRINT_p :
    case PROGRAM_p :
    case READ_p :
    case RETURN_p :
    case REWIND_p :
    case SAVE_p:
    case STOP_p:
    case SUBROUTINE_p :
	/* Cas ou le mot_cle peut etre suivi d'un ENTIER ou d'un ID */
	PTOK->lahead = kw_codes [kw];
	PTOK->string_table_entry  = EMPTY_STE;
	l = kw_lgth [kw];

    extract_id_int:
	str += l;

	if (*str != NUL) {
	    if (kw == RETURN_p && program_unit_kind == FUNCTION_SUBPROGRAM) {
		/* alterate return in a FUNCTION */
		int eol_tok_no;

		sxput_error (*get_source_index (&(PTOK->source_index), l),
			     "%sAn alternate return is not allowed in a function subprogram,\n\
this expression is erased.",
			     sxsvar.sxtables->err_titles [2] /* error */);
		eol_tok_no = Mtok_no;

		while (sxget_token (++eol_tok_no)->lahead != EOL_t);

		if (eol_tok_no -= Mtok_no + 1)
		    sxtknmdf (NULL, 0, Mtok_no + 1, eol_tok_no);
	    }
	    else {
		/* On extrait le suffixe qu'on insere ds le flot de tokens */
		tt [0].lahead = isdigit (*str) ? ENTIER_t : ID_t;
		tt [0].string_table_entry = sxstrsave (str);
		tt [0].source_index = *get_source_index (&(PTOK->source_index), l);
		tt [0].comment = NULL;

		if (is_ansi && tt [0].lahead == ID_t)
		    check_id_lgth (&(tt [0]));

		sxtknmdf (&(tt [0]), 1, Mtok_no + 1, 0);
	    }
	}

	break;
	
    case DO_p  :
	if (!after_if_log &&
	    strlgth >= 3 &&
	    (str += 2, (str1 = skip_an_int (str)) > str)) {
	    /* l'id commence par do int */
	    /* si les tokens suivants sont "virgule %id" on rend "do %int virgule %id" */
	    if (*str1 == NUL &&
		sxget_token (Mtok_no + 1)->lahead == VIRGULE_t &&
		sxget_token (Mtok_no + 2)->lahead == ID_t) {
		do_stack_mngr (str, &(PTOK->source_index));
		PTOK->lahead = DO_t;
		PTOK->string_table_entry = EMPTY_STE;
		tt [0].lahead = ENTIER_t;
		tt [0].string_table_entry = sxstr2save (str, str1 - str);
		tt [0].source_index =
		    *get_source_index (&(PTOK->source_index), 2);
		tt [0].comment = NULL;
		sxtknmdf (&(tt [0]), 1, Mtok_no + 1, 0);
	    }
	    else if (isalpha (*str1)) {
		/* Si l'id est de la forme do %int %id => appel recursif du parser
		   pour savoir si c'est l'instruction do */
		/* Candidat pour une instruction do */
		/* On appelle recusivement le parser en simulant le point d'entree
		   !DO de la grammaire. */
		/* Si OK on rend "do %int %id". */
		/* On ne change rien dans le mode du scanner. */
		/* Aucune erreur ne doit etre detectee dans le parser. */
		/* Elles ne sont donc pas comptabilisees dans do_parse_mode_nomess.error_nb. */
		if (sxparse_in_la (DO_EP_t, Mtok_no, &tok_no, &do_parse_mode_nomess)) {
		    /* Instruction do */
		    do_stack_mngr (str, &(PTOK->source_index));
		    PTOK->lahead = DO_t;
		    PTOK->string_table_entry = EMPTY_STE;
		    tt [0].lahead = ENTIER_t;
		    tt [0].string_table_entry = sxstr2save (str, str1 - str);
		    tt [0].source_index =
			*get_source_index (&(PTOK->source_index), 2);
		    tt [0].comment = NULL;
		    tt [1].lahead = ID_t;
		    tt [1].string_table_entry = sxstrsave (str1);
		    tt [1].source_index =
			*get_source_index (&(PTOK->source_index), 2 + str1 - str);
		    tt [1].comment = NULL;

		    if (is_ansi)
			check_id_lgth (&(tt [1]));

		    sxtknmdf (&(tt [0]), 2, Mtok_no + 1, 0);
		}
	    }
	}
	
	break;
	
	
    case IMPLICIT_p:
	str += 8;

	switch (kw = seek_kw (str)) {
	case CHARACTER_p:
	case COMPLEX_p:
	case DOUBLEPRECISION_p:
	case INTEGER_p:
	case LOGICAL_p:
	case REAL_p:
	    PTOK->lahead = IMPLICIT_t;
	    PTOK->string_table_entry = EMPTY_STE;
	    tt [0].lahead = kw_codes [kw];
	    tt [0].string_table_entry = EMPTY_STE; 
	    tt [0].source_index = *get_source_index (&(PTOK->source_index), 8);
	    tt [0].comment = NULL;
	    sxtknmdf (&(tt [0]), 1, Mtok_no + 1, 0);

	    if (kw == DOUBLEPRECISION_p) {
		/* On retourne 2 tokens pour DOUBLE PRECISION  */
		tt [0].lahead = PRECISION_t;
		tt [0].string_table_entry = EMPTY_STE;
		tt [0].source_index =
		    *get_source_index (&(PTOK->source_index), 14);
		tt [0].comment = NULL;
		sxtknmdf (&(tt [0]), 1, Mtok_no + 2, 0);
	    }

	    break;

	default:
	    break;
	}

	break;
	
    default:
	break;
    }
}
    
	    
	    
static VOID push_iokw (tok_no)
    int tok_no;
{
    /* On memorise tok_no dans iokw_stack. */
    if (++iokw_stack_top > iokw_stack_size)
	iokw_stack = (int*) sxrealloc (iokw_stack, (iokw_stack_size += 10) + 1, sizeof (int));
				
    iokw_stack [iokw_stack_top] = tok_no;
}


static VOID check_iokw (head, tail)
    int tail;
{
    /* On vient de trouver une instruction d'entree sortie qui peut impliquer
       des utilisations de iokw (FMT, UNIT, ... ), or au plus une seule occurrence
       est permise. On le verifie et, eventuellement on sort les messages correspondants. */
    /* head est le tok_no du mot-cle : WRITE, READ,... */
    /* tail est le tok_no de la parenthese fermante terminant la liste. */

    /* La liste doit contenir exactement un "unit_identifier".
       Si le premier mot-cle de iokw_stack ne se trouve pas en head+2, alors
       le premier specifier de la liste est un unit_identifier qui sera interdit
       dans la suite. */

    /* Si on est dans une controle_info_list (WRITE ou READ), la liste peut
       avoir la forme : "WRITE ( unit_identifier, format_identifier, ...".
       Auquel cas il est interdit d'y retrouver "FMT = ..." ds la suite.
       Difficile a reperer, car unit_identifier peut etre un array_element_name
       et donc avoir des ",". On pourrait lancer un "sxparse_in_la" qui
       reconnaisse un unit_identifier. Pour l'instant, on laisse le soin
       a la suite de traiter ce cas. */

    register int	i, tok_no, t;
    BOOLEAN		is_error = FALSE;

    CLEAR (t_stack);

    if (iokw_stack [1] != head + 2) {
	PUSH (t_stack, UNIT_t);
    }

    for (i = 1; i <= iokw_stack_top; i++) {
	tok_no = iokw_stack [i];
	t = SXGET_TOKEN (tok_no).lahead;

	if (t_stack [t] == 0) {
	    PUSH (t_stack, t);

	    if (t != END_t || SXGET_TOKEN (head).lahead != WRITE_t)
		iokw_stack [i] = - tok_no;
	    else
		/* An "END=" specifier must not appear in a WRITE statement. */
		is_error = TRUE;
	}
	else
	    is_error = TRUE;
    }

    if (is_error) {
	push_iokw (tail + 1); /* Il existe toujours un suivant. */

	/* On sort les messages d'erreur en premier... */
	for (i = iokw_stack_top - 1; i > 0; i--) {
	    if ((tok_no = iokw_stack [i]) > 0) {
		TOK_i = tok_no - 1;
		sxput_error (SXGET_TOKEN (TOK_i).source_index,
			     "%sThis illegal occurrence of a specifier is erased from here....",
			     sxsvar.sxtables->err_titles [2] /* error */);
		
		/* On cherche le specifier suivant. */
		if ((t = iokw_stack [i+1]) < 0)
		    t = -t;

		TOK_i = t - 1;
		sxput_error (SXGET_TOKEN (TOK_i).source_index,
			     "%s...until there.",
			     sxsvar.sxtables->err_titles [1]);
	    }
	}

	/* ...puis on tasse. Ca ne peut etre fait en meme temps car le
	   token t-1 aurait pu disparaitre. */
	for (i = iokw_stack_top - 1; i > 0; i--) {
	    if ((tok_no = iokw_stack [i]) > 0) {
		/* On cherche le specifier suivant. */
		if ((t = iokw_stack [i+1]) < 0)
		    t = -t;

		/* On supprime [tok_no-1 : t-2] */
		sxtknmdf (NULL, 0, tok_no - 1, t - tok_no);
	    }
	}
    }

    i = t_stack [UNIT_t];

    if (SXGET_TOKEN (head).lahead == INQUIRE_t) {
	/* INQUIRE by File or INQUIRE by Unit. */
	t = t_stack [FILE_t];

	if (i == 0 && t == 0 || i != 0 && t != 0)
	    sxput_error (SXGET_TOKEN (head).source_index,
			 "%sThis statement should be either an INQUIRE by File\n\
or an INQUIRE by Unit.",
			 sxsvar.sxtables->err_titles [2] /* error */);
    }
    else if (i == 0)
	sxput_error (SXGET_TOKEN (head).source_index,
		     "%sThis statement must contain exactly one unit specifier.",
		     sxsvar.sxtables->err_titles [2] /* error */);

}


int f77_parsact (code, act_no)
    int		code;
    int		act_no;
{
    int lahead, tok_no;

    switch (code) {
    case OPEN:
    case CLOSE:
	return 0;

    case INIT:
	return 0;

    case FINAL:
	return 0;

    case ACTION:
	switch (act_no) {
	case 0:
	    /* Illegal <22:len_specifiaction> */
	    if (is_ansi) {
		TOK_i = sxplocals.atok_no - 1;
		sxput_error (SXGET_TOKEN (TOK_i).source_index,
			     "%sIn the standard, a length specification can only be\n\
associated with the type CHARACTER.",
			     sxsvar.sxtables->err_titles [1] /* warning */);
	    }

	    return 0;


	default:
	    break;
	}

    case PREDICATE:
	switch (act_no) {
	case 0:
	    break;

	case 1:
	    /* On est sur une virgule dans une io_list, une io_imply_do_list ou une
	       constante complexe. retourne VRAI si on n'est pas dans un complexe. */
	    if ((lahead = sxget_token (tok_no = sxplocals.ptok_no + 1)->lahead) == ID_t)
		return 1;

	    if (lahead == PLUS_t || lahead == MINUS_t)
		lahead = sxget_token (++tok_no)->lahead;

	    if ((lahead == UIC_t || lahead == URC_t) &&
		sxget_token (tok_no + 1)->lahead == RIGHTP_t)
		return 0;
		
	    return 1;

	default:
	    break;
	}

    default:
	fputs ("The function \"f77_parsact\" is out of date with respect to its specification.\n", sxstderr);
	abort ();
    }
}


static BOOLEAN check_stmt_sequence (lahead, tok_no, newla)
    int lahead, tok_no, newla;
{
    /* Cette procedure verifie le sequencement correct des instructions */
    /* ETAT est une variable globale initialisee a ETAT_0 au debut et apres
       chaque END. */
    /* lahead peut etre different de SXGET_TOKEN (tok_no).lahead] */
    register int	kind;
    int			eol_tok_no;
    struct sxtoken	tt;

    if ((kind = stmt_kind [lahead]) == 0)
	return TRUE;

    switch (ETAT) {
    case ETAT_1:
	switch (kind) {
	case format_or_entry_k:
	case parameter_k :
	case implicit_k :
	    return TRUE;

	case other_specification_k:
	    ETAT = ETAT_2;
	    return TRUE;

	case data_k :
	case executable_k :
	    ETAT = ETAT_3;
	    return TRUE;

	case end_k :
	    ETAT = ETAT_0;
	    return TRUE;

	}

	break;

    case ETAT_2:
	switch (kind) {
	case format_or_entry_k:
	case parameter_k :
	case other_specification_k:
	    return TRUE;

	case data_k :
	case executable_k :
	    ETAT = ETAT_3;
	    return TRUE;

	case end_k :
	    ETAT = ETAT_0;
	    return TRUE;

	case implicit_k :
	    break;
	}

    break;

    case ETAT_3:
	switch (kind) {
	case format_or_entry_k:
	case data_k :
	case executable_k :
	    return TRUE;

	case end_k :
	    ETAT = ETAT_0;
	    return TRUE;

	case parameter_k :
	case implicit_k :
	case other_specification_k:
	    break;

	}

    break;

    }

    /* Ici, erreur de sequence */
    /* Si newla est nul on efface tout
       si non nul on change le stmt fautif en CONTINUE , ca permet
          - de ne pas toucher au label eventuel
	  - d'avoir un comportement "correct" si jamais ce stmt labelle fermait
	    un DO
	  - il n'est pas pris en compte ds le sequencement => les autres erreurs de
	    sequencement seront decouvertes. */

    /* message */
    sxput_error (SXGET_TOKEN (tok_no).source_index,
		 "%sThis %s statement is out of sequence,\n\
it is %s.",
		 sxsvar.sxtables->err_titles [2] /* Error */,
		 sxttext (sxsvar.sxtables, lahead),
		 newla == 0 ? "erased" : "changed into a CONTINUE statement");

    /* Remplacement des tokens jusqu'a EOL par CONTINUE */
    eol_tok_no = tok_no;

    if (newla != 0) {
	tt = SXGET_TOKEN (tok_no);
	tt.lahead = CONTINUE_t;
	tt.string_table_entry = EMPTY_STE;
    }
    else {
	TOK_i = tok_no - 1;

	if (SXGET_TOKEN (TOK_i).lahead == LABEL_t)
	--tok_no;
    }


    while (sxget_token (++eol_tok_no)->lahead != EOL_t);

    if (newla != 0)
	sxtknmdf (&tt, 1, tok_no, eol_tok_no - tok_no);
    else
	sxtknmdf (NULL, 0, tok_no, eol_tok_no - tok_no + 1);

    return FALSE;
}


static VOID entry_or_return_in_main (lahead, tok_no)
    int lahead, tok_no;
{
    /* On est ds un MAIN program sur un ENTRY ou RETURN stmt. */
    int			eol_tok_no;
    struct sxtoken	tt;

    sxput_error (SXGET_TOKEN (tok_no).source_index,
		 "%sThis %s statement is illegal in a MAIN program,\n\
it is changed into a CONTINUE statement.",
		 sxsvar.sxtables->err_titles [2] /* Error */,
		 sxttext (sxsvar.sxtables, lahead));

    /* Remplacement des tokens jusqu'a EOL par CONTINUE */
    eol_tok_no = tok_no;
    tt = SXGET_TOKEN (tok_no);
    tt.lahead = CONTINUE_t;
    tt.string_table_entry = EMPTY_STE;

    while (sxget_token (++eol_tok_no)->lahead != EOL_t);

    sxtknmdf (&tt, 1, tok_no, eol_tok_no - tok_no);
}

static BOOLEAN check_block_data_subprogram (lahead, tok_no)
    int lahead, tok_no;
{
    /* On est ds un block_data_subprogram. lahead ne peut etre que
       BLOCK, IMPLICIT, PARAMETER, DIMENSION, COMMON, SAVE, EQUIVALENCE,
       DATA, END or type. */
    register int	kind;
    int			eol_tok_no;

    kind = stmt_kind [lahead];

    switch (kind) {
    case format_or_entry_k:
    case executable_k :
	break;
	
    case implicit_k :
    case parameter_k :
    case data_k :
    case end_k :
	return TRUE;
	
    case other_specification_k:
	if (lahead != INTRINSIC_t && lahead != EXTERNAL_t)
	    return TRUE;
	
	break;
    }
    
    sxput_error (SXGET_TOKEN (tok_no).source_index,
		 "%sThis %s statement is illegal in a BLOCK DATA SUBPROGRAM,\n\
it is erased.",
		 sxsvar.sxtables->err_titles [2] /* Error */,
		 kind == executable_k ? "executable" : sxttext (sxsvar.sxtables, lahead));

    /* Suppression des tokens jusqu'a EOL. */
    eol_tok_no = tok_no;

    TOK_i = tok_no - 1;

    if (SXGET_TOKEN (TOK_i).lahead == LABEL_t)
	--tok_no;

    while (sxget_token (++eol_tok_no)->lahead != EOL_t);

    sxtknmdf (NULL, 0, tok_no, eol_tok_no - tok_no + 1);
    return FALSE;
}




int f77_scanact (code, act_no)
    int		code;
    int		act_no;
{
    int prev_lahead;

    switch (code) {
    case OPEN:
	break;

    case INIT:
	is_tab_already_met = is_hollerith_already_met = is_quoted_string_already_met =
	    is_lower_to_upper_already_met = FALSE;
	do_stack = (struct do_item*) sxalloc ((do_stack_size = 10) + 1, sizeof (struct do_item));
	do_stack_top = 0;
	t_stack = (int*) sxcalloc (sxeof_code (sxsvar.sxtables) + 1, sizeof (int));
	t_stack [0] = -1; /* vide */
	iokw_stack = (int*) sxalloc ((iokw_stack_size = 10) + 1, sizeof (int));
	stmt_kind = (int*) sxcalloc (sxeof_code (sxsvar.sxtables) + 1, sizeof (int));
	ETAT = ETAT_0;

	stmt_kind [ASSIGN_t] = executable_k;
	stmt_kind [BACKSPACE_t] = executable_k;
	stmt_kind [BACKSPACE_KW_t] = executable_k;
	stmt_kind [CALL_t] = executable_k;
	stmt_kind [CHARACTER_t] = other_specification_k;
	stmt_kind [CLOSE_t] = executable_k;
	stmt_kind [COMPLEX_t] = other_specification_k;
	stmt_kind [COMMON_t] = other_specification_k;
	stmt_kind [CONTINUE_t] = executable_k;
	stmt_kind [DATA_t] = data_k;
	stmt_kind [DIMENSION_t] = other_specification_k;
	stmt_kind [DO_t] = executable_k;
	stmt_kind [DOUBLE_t] = other_specification_k;
	stmt_kind [ELSE_t] = executable_k;
	stmt_kind [END_t] = end_k;
	stmt_kind [ENDFILE_t] = executable_k;
	stmt_kind [ENDFILE_KW_t] = executable_k;
	stmt_kind [ENTRY_t] = format_or_entry_k;
	stmt_kind [EQUIVALENCE_t] = other_specification_k;
	stmt_kind [EXTERNAL_t] = other_specification_k;
	stmt_kind [FORMAT_t] = format_or_entry_k;
	stmt_kind [GO_t] = executable_k;
	stmt_kind [IF_t] = executable_k;
	stmt_kind [IMPLICIT_t] = implicit_k;
	stmt_kind [INQUIRE_t] = executable_k;
	stmt_kind [INTEGER_t] = other_specification_k;
	stmt_kind [INTRINSIC_t] = other_specification_k;
	stmt_kind [LOGICAL_t] = other_specification_k;
	stmt_kind [OPEN_t] = executable_k;
	stmt_kind [PARAMETER_t] = parameter_k;
	stmt_kind [PAUSE_t] = executable_k;
	stmt_kind [PRINT_t] = executable_k;
	stmt_kind [READ_t] = executable_k;
	stmt_kind [READ_KW_t] = executable_k;
	stmt_kind [REAL_t] = other_specification_k;
	stmt_kind [RETURN_t] = executable_k;
	stmt_kind [REWIND_t] = executable_k;
	stmt_kind [REWIND_KW_t] = executable_k;
	stmt_kind [SAVE_t] = other_specification_k;
	stmt_kind [STOP_t] = executable_k;
	stmt_kind [WRITE_t] = executable_k;

	main_program_nb = 0;
	break;

    case FINAL:
#ifdef EBUG
	if (!is_pretty_printer)
	    print_toks_buf ();
#endif

	if (Tlen != NULL)
	    sxfree (Tlen), Tlen = NULL;

	sxfree (stmt_kind);
	sxfree (do_stack);
	sxfree (iokw_stack);
	sxfree (t_stack);
	break;

    case CLOSE:
	break;

    case ACTION:
	switch (act_no) {
	case 0:
	{
	    /* On vient de reconnaitre un label */
	    /* On exploite la pile des do */
	    register int val, i, new_top;

	    if (comment != NULL) {
		sxsvar.sxlv.terminal_token.comment = comment;
		comment = NULL;
	    }

	    val = atoi (sxsvar.sxlv_s.token_string);

	    for (i = 1; i <= do_stack_top ; i++) {
		if (do_stack [i].label == val)
		    break;
	    }

	    if (i <= do_stack_top) {
		/* etiquette d'un enddo */
		new_top = i - 1;
		enddo_nb = do_stack_top - new_top;
		
		while (++i <= do_stack_top) {
		    if (do_stack [i].label != val)
			sxput_error (do_stack [i].source_index,
				     "%sThis do-statement has been gracefully closed while\n\
processing the enclosing do labeled %i and terminated at line %i.",
				     sxsvar.sxtables->err_titles [2] /* error */, val,
				     sxsvar.sxlv.terminal_token.source_index.line);
		    
		}
		
		do_stack_top = new_top;
	    }

	    /* Afin de traiter certains cas d'erreur (suppression totale de
	       l'instruction, label compris), on lit le token suivant maintenant. */

	    /* On range le token courant */
	    sxput_token (sxsvar.sxlv.terminal_token);
	    /* on lit le suivant (et peut-etre beaucoup d'autres). */
	    sxget_token (sxplocals.Mtok_no + 1);
	    /* On sort du scanner sans retourner aucun token */
	    sxsvar.sxlv.terminal_token.lahead = 0;
	    sxsvar.sxlv.terminal_token.string_table_entry = ERROR_STE;
	}
	
	return 0;

	case 1: {
	    /* Post action de %id */
	    int			Mtok_no, lahead, cur_lahead, tok_no, tail_tok_no;
	    register char	*str;
	    register int	strlgth, kw;
	    BOOLEAN		after_logical_if;

	    switch (cur_lahead = sxsvar.sxlv.terminal_token.lahead) {
	    case A_t:
	    case B_t:
	    case D_t:
	    case E_t:
	    case F_t:
	    case G_t:
	    case I_t:
	    case L_t:
	    case N_t:
	    case P_t:
	    case R_t:
	    case S_t:
	    case T_t:
	    case X_t:
	    case Z_t:
		/* Ca a ete reconnu par %symbolic_name, on n'est donc pas dans un format
		   et ce ne sont donc pas les mots_cles. On les change en %symbolic_name. */
		/* Ils ont la bonne longueur!. */
		sxsvar.sxlv.terminal_token.lahead = ID_t;
		sxsvar.sxlv.terminal_token.string_table_entry =
		    sxstrsave (sxttext (sxsvar.sxtables, cur_lahead));

		return 0;

	    default:
		break;
	    }

	    prev_lahead = SXGET_TOKEN (sxplocals.Mtok_no).lahead;

	    if (prev_lahead == EOL_t ||
		prev_lahead == LABEL_t ||
		prev_lahead == sxeof_code (sxsvar.sxtables)) {
		/* On est en debut d'instruction */
		/* Si on est ds le pretty-printer, on traite les commentaires. */
		if (comment != NULL) {
		    sxsvar.sxlv.terminal_token.comment = comment;
		    comment = NULL;
		}


		/* On range le token courant */
		sxput_token (sxsvar.sxlv.terminal_token);
		/* en Mtok_no */
		tail_tok_no = Mtok_no = sxplocals.Mtok_no;
		is_function_stmt = FALSE;
		is_iokw_allowed = FALSE;
		after_logical_if = TRUE;
		
		while (after_logical_if) {
		    after_logical_if = FALSE;
		    iokw_stack_top = 0;

		    if (cur_lahead == FORMAT_t && prev_lahead == LABEL_t) {
			/* On teste "format" avant "lhs" afin de permettre de protester
			   plus facilement si on trouve des chaines Hollerith a
			   l'exterieur des formats. */
			int			head;
			struct sxsrcmngr	prev_sxsrcmngr;
			struct sxscan_mode	old_scan_mode;
			BOOLEAN			is_format_stmt;

			/* On teste un FORMAT */
			sxsvar.sxlv_s.counters [0] = 1; 
			
			/* On memorise ou on en est dans le source. */
			/* Attention, il faudra eventuellement pouvoir remonter
			   en arriere d'un buffer si sxparse_in_la est tombe'
			   sur EOL. */
			head = source_line_head;
			prev_sxsrcmngr = sxsrcmngr;
			
			/* Le "mode" actuel du scanner */
			old_scan_mode = sxsvar.sxlv.mode;
			sxsvar.sxlv.mode = scan_mode_nomess;
			/* Dans tous les cas on "supprime" les tokens lus :
			   - soit on remplace par %format_specification
			   - soit le flot de caracteres peut etre decoupe autrement. */
			
			is_format_stmt = sxparse_in_la (FORMAT_EP_t, Mtok_no, &tok_no,
							&format_parse_mode_nomess) &&
							    sxget_token(tok_no)->lahead == EOL_t;
			sxsvar.sxlv.mode = old_scan_mode;

			if (is_format_stmt) {
			    /* C'est un format_stmt */
			    if (sxsvar.sxlv.mode.errors_nb +
				format_parse_mode_nomess.local_errors_nb + format_parse_mode_nomess.global_errors_nb > 0) {
				/* mais avec des erreurs corrigibles */
				/* On recommence tout avec sortie des messages. */
				/* La correction d'erreur a pu modifier le flot de tokens. */
				/* On remet le source "en place" */
				source_line_head = head;
				sxsrcmngr = prev_sxsrcmngr;

				/* On remet le token_mngr */
				tok_no = sxplocals.Mtok_no = Mtok_no;
				/* On sort les messages. */
				/* On rescanne en mode "format" */
				sxsvar.sxlv.mode = old_scan_mode;
				/* Jusqu'au EOL */
				while (sxget_token (++tok_no)->lahead != EOL_t);
			    }
			    /* else sans erreur, on ne touche a rien */
			}
			else {
			    /* Ce n'est pas un format_statement. */
			    /* On remet le source "en place" */
			    source_line_head = head;
			    sxsrcmngr = prev_sxsrcmngr;

			    /* On dit que c'est un ident. */
			    change_tok (Mtok_no, ID_t, sxttext (sxsvar.sxtables, cur_lahead));
			    sxplocals.Mtok_no = Mtok_no;
			}

			format_parse_mode_nomess.global_errors_nb = 0;
			format_parse_mode_nomess.local_errors_nb = 0;
			sxsvar.sxlv_s.counters[0] = 0; 
		    }
		    else if (check_for_a_lhs (Mtok_no)) {
			if ((cur_lahead = SXGET_TOKEN (Mtok_no).lahead) != ID_t) {
			    /* Ce n'est pas un mot_cle */
			    /* on en fait un ident. */	
			    change_tok (Mtok_no, ID_t, sxttext (sxsvar.sxtables, cur_lahead));
			}
			else {
			    /* Attention, ca peut etre "DO label ident =" qui ressemble
			       etrangement a une affectation. */
			    TOK_i = Mtok_no + 1;

			    if (SXGET_TOKEN (TOK_i).lahead == EGAL_t &&
				seek_kw (sxstrget (SXGET_TOKEN (Mtok_no).string_table_entry)) ==
				DO_p)
				process_initial_id (Mtok_no, FALSE);
			}
		    }
		    else {
			/* On est en debut d'instruction et le token reconnu par %ident
			   n'est pas le debut d'une partie gauche d'affectation. */
			switch (cur_lahead) {
			case CHARACTER_t:
			    /* On est dans le cas ou CHARACTER est suivi d'un delimiteur
			       on lit la suite pour reconnaitre si on est dans une
			       FUNCTION statement. */
			    /* La lecture de 2 tokens devrait suffire */
			    sxget_token (Mtok_no + 2);
			    break;
			    
			case CLOSE_t:
			case INQUIRE_t:
			case OPEN_t:
			case WRITE_t:
			    tok_no = Mtok_no;
			    
			    if (check_balanced (&tok_no))
				check_iokw (Mtok_no, tok_no);

			    break;


			case BACKSPACE_t:
			case ENDFILE_t:
			case REWIND_t:
			    /* kw suivi par un delimiteur */
			    tok_no = Mtok_no;
			    
			    if (check_balanced (&tok_no) &&
				  sxget_token (tok_no + 1)->lahead == EOL_t)
				check_iokw (Mtok_no, tok_no);
			    else if (cur_lahead == BACKSPACE_t)
				change_tok (Mtok_no, BACKSPACE_KW_t, "BACKSPACE");
			    else if (cur_lahead == ENDFILE_t)
				change_tok (Mtok_no, ENDFILE_KW_t, "ENDFILE");
			    else
				change_tok (Mtok_no, REWIND_KW_t, "REWIND");   

			    break;
			    
			case END_t:
			    /* c'est une instruction END, le "D" doit etre
			       sur la meme carte que le "E" !!! */
			    /* s'il y a un label, il doit egalement etre sur la meme
			       carte (initiale). */
			    /* Si elle n'est pas suivie de EOL, on laisse
			       le rattrapage d'erreur normal s'en occuper. */
			    tok_no = Mtok_no;
			    
			    if (sxget_token (++tok_no)->lahead == EOL_t) {
				/* C'est l'instruction end */
				TOK_i = Mtok_no - 1;

				if (SXGET_TOKEN (TOK_i).lahead == LABEL_t &&
				    (SXGET_TOKEN (TOK_i).source_index.line !=
				     SXGET_TOKEN (Mtok_no).source_index.line) ||
				    SXGET_TOKEN (Mtok_no).source_index.line !=
				    SXGET_TOKEN (tok_no).source_index.line)
				    sxput_error (SXGET_TOKEN (Mtok_no).source_index,
						 "%sAn \"END\" statement is written only in columns 7 through 72 of an initial line.",
					     sxsvar.sxtables->err_titles [1] /* warning */);
			    }
			    
			    break;
			    
			case FORMAT_t:
			    /* Ce point d'entree ne peut etre atteint que sur une erreur,
			       ce n'est ni un format statement ni une lhs, on change le mot-cle
			       en ident pour avoir le traitement d'erreur du cas lhs. */
			    change_tok (Mtok_no, ID_t, sxttext (sxsvar.sxtables, cur_lahead));
			    break;
			    
			case ID_t:
			    process_initial_id (Mtok_no, after_logical_if);
			    break;
			    
			case IF_t:
			    /* Instructions IF. */
			    /* Si on est dans un IF logique, l'instruction qui suit doit etre
			       consideree comme etant en debut d'instruction. */
			    tok_no = Mtok_no;
			    check_balanced (&tok_no); /* Should be TRUE! */
			    lahead = sxget_token (++tok_no)->lahead;
			    
			    if (!(lahead == ENTIER_t && sxget_token (tok_no + 1)->lahead == VIRGULE_t ||
				  lahead == THEN_t && sxget_token (tok_no + 1)->lahead == EOL_t)) {
				/* c'est un IF logique */
				/* Le token lahead doit etre considere comme etant en debut
				   d'instruction. */
				/* Malheureusement, si c'est un ident, c'est peut etre la
				   lecture en avance (car il n'a pas alors ete considere
				   comme etant en debut d'instruction) qui d'un mot-cle
				   l'a change en ident.  On regarde donc si c'est un mot-cle. */
				
				if (lahead == ID_t) {
				    register int ste, kw;
				    
				    ste = SXGET_TOKEN (tok_no).string_table_entry;
				    kw = (*sxsvar.SXS_tables.check_keyword) (sxstrget (ste), sxstrlen (ste));
				    
				    if (kw != 0) {
					SXGET_TOKEN (tok_no).lahead = lahead = kw;
					SXGET_TOKEN (tok_no).string_table_entry = EMPTY_STE;
				    }
				}
				
				/* on reboucle. */
				after_logical_if = TRUE;
				cur_lahead = lahead;
				Mtok_no = tok_no;
			    }
			    
			    break;
			    
			case READ_t:
			    /* On differencie l'identifieur des mots-cles READ suivis soit de
			       ( <control_info_list> ) cas READ_t, soit de <format_identifier>
			       qui peut etre une <character-expression> et donc commencer par une
			       "(" cas READ_KW_t. */
			    /* Il y a ambiguite si <format_identifier> est
			       une <character_expression> representee par "( id )"
			       et si la <control_info_list> est aussi un id
			       et si l'io_list est vide. Dans ce cas on choisit READ_KW_t... */
			    tok_no = Mtok_no;
			    
			    if (check_balanced (&tok_no)) {
				/* Si le contenu des parentheses est un
				   <format_identifier>, le token suivant
				   est "," ou "//". */
				lahead = sxget_token (tok_no + 1)->lahead;
				
				if (lahead == VIRGULE_t || lahead == CONCAT_t)
				    change_tok (Mtok_no, READ_KW_t, "READ");
				else
				    check_iokw (Mtok_no, tok_no);
			    }
			    else {
				/* Assume format_identifier */
				change_tok (Mtok_no, READ_KW_t, "READ");
			    }
			    
			    break;
			    
			case RETURN_t:
			    TOK_i = Mtok_no + 1;

			    if (program_unit_kind == FUNCTION_SUBPROGRAM &&
				SXGET_TOKEN (TOK_i).lahead != EOL_t) {
				/* alterate return in a FUNCTION */
				int eol_tok_no;

				sxput_error (SXGET_TOKEN (TOK_i).source_index,
					     "%sAn alternate return is not allowed in a function subprogram,\n\
this expression is erased.",
					     sxsvar.sxtables->err_titles [2] /* error */);
				eol_tok_no = Mtok_no + 1;

				while (sxget_token (++eol_tok_no)->lahead != EOL_t);

				sxtknmdf (NULL, 0, Mtok_no + 1, eol_tok_no - Mtok_no - 1);
			    }

			    break;

			default:
			    /* Les mots-cles restent les mots_cles. */
			    break;
			}
		    }
		}
		    
		lahead = SXGET_TOKEN (tail_tok_no).lahead;

		if (ETAT == ETAT_0) {
		    /* Entree ds un programme principal ou un sous_programme. */
		    ETAT = ETAT_1;

		    if (lahead == BLOCK_t)
			program_unit_kind = BLOCK_DATA_SUBPROGRAM;
		    else if (lahead == SUBROUTINE_t)
			program_unit_kind = SUBROUTINE_SUBPROGRAM;
		    else if (is_function_stmt) {
			program_unit_kind = FUNCTION_SUBPROGRAM ;
		    }
		    else {
			/* entree d'un main_program */
			program_unit_kind = MAIN_PROGRAM;

			if (++main_program_nb > 1) {
			    sxput_error (SXGET_TOKEN (tail_tok_no).source_index,
					 "%sThis multiple MAIN program is illegal.",
					 sxsvar.sxtables->err_titles [2] /* Error */);

			}
		    }
		}
		else {
		    if (lahead == ID_t)
			lahead = ASSIGN_t;
		    else {
			TOK_i = tail_tok_no + 1;

			if (lahead == END_t && SXGET_TOKEN (TOK_i).lahead == IF_t)
			lahead = IF_t; /* pourquoi pas? */
		    }

		    if (program_unit_kind == BLOCK_DATA_SUBPROGRAM) {
			if (check_block_data_subprogram (lahead, tail_tok_no))
			    check_stmt_sequence (lahead, tail_tok_no, 0);
		    }
		    else
			check_stmt_sequence (lahead, tail_tok_no, 1);
		}

		if (program_unit_kind == MAIN_PROGRAM && (lahead == ENTRY_t || lahead == RETURN_t))
		    entry_or_return_in_main (lahead, tail_tok_no);
		    

		if (is_ansi && SXGET_TOKEN (tail_tok_no).lahead == ID_t)
		    check_id_lgth (&(SXGET_TOKEN (tail_tok_no)));

		/* On sort du scanner sans retourner aucun token */
		sxsvar.sxlv.terminal_token.lahead = 0;
		sxsvar.sxlv.terminal_token.string_table_entry = ERROR_STE;

	    }
	    else 
		/* Ident ou mot-cle pas en debut d'instruction. */
		/* On change les mot-cles en ident quand c'est possible. */
		switch (cur_lahead) {
		    /* "mot_cles" traites lexicalement. */

		case ACCESS_t:
		case BLANK_t:
		case DIRECT_t:
		case END_t:
		case ERR_t:
		case EXIST_t:
		case FILE_t:
		case FMT_t:
		case FORM_t:
		case FORMATTED_t:
		case IOSTAT_t:
		case NAME_t:
		case NAMED_t:
		case NEXTREC_t:
		case NUMBER_t:
		case OPENED_t:
		case REC_t:
		case RECL_t:
		case SEQUENTIAL_t:
		case STATUS_t:
		case UNFORMATTED_t:
		case UNIT_t:
		    /* Ces noms intervenant dans des instructions d'entree/sortie
		       sont des mots_cles ssi is_iokw_allowed et s'ils sont suivis
		       par "=" */
		    /* On range le token courant */
		    sxput_token (sxsvar.sxlv.terminal_token);
		    /* en Mtok_no */
		    Mtok_no = sxplocals.Mtok_no;
		    TOK_i = Mtok_no + 1;

		    if (is_iokw_allowed && sxget_token (TOK_i)->lahead == EGAL_t) {
			/* On s'en souvient pour check_iokw eventuel */
			    push_iokw (Mtok_no);
		    }
		    else {
			/* On en fait un %id. */
			change_tok (Mtok_no, ID_t, sxttext (sxsvar.sxtables, cur_lahead));

			if (is_ansi)
			    check_id_lgth (&(SXGET_TOKEN (Mtok_no)));
		    }

		    /* On sort du scanner sans retourner aucun token */
		    sxsvar.sxlv.terminal_token.lahead = 0;
		    sxsvar.sxlv.terminal_token.string_table_entry = ERROR_STE;
		    break;


		case ASSIGN_t:
		case BACKSPACE_t:
		case BLOCK_t:
		case CALL_t:
		case CHARACTER_t:
		case CLOSE_t:
		case COMMON_t:
		case COMPLEX_t:
		case CONTINUE_t:
		case DATA_t:
		case DIMENSION_t:
		case DO_t:
		case DOUBLE_t:
		case ELSE_t:
		case ENDFILE_t:
		case ENTRY_t:
		case EQUIVALENCE_t:
		case EXTERNAL_t:
		case FORMAT_t:
		case FUNCTION_t:
		case GO_t:
		case IF_t:
		case IMPLICIT_t:
		case INQUIRE_t:
		case INTEGER_t:
		case INTRINSIC_t:
		case LOGICAL_t:
		case OPEN_t:
		case PARAMETER_t:
		case PAUSE_t:
		case PRECISION_t:
		case PROGRAM_t:
		case READ_t:
		case REAL_t:
		case RETURN_t:
		case REWIND_t:
		case SAVE_t:
		case STOP_t:
		case SUBROUTINE_t:
		case TO_t:
		case WRITE_t:
		    /* On en fait des idents. */
		    sxsvar.sxlv.terminal_token.lahead = ID_t;
		    sxsvar.sxlv.terminal_token.string_table_entry =
			sxstrsave (sxttext (sxsvar.sxtables, cur_lahead));
		    /* On continue en sequence */

		default:
		    /* "mot_cles" traites syntaxiquement. */
		    if (is_ansi && sxsvar.sxlv.terminal_token.lahead == ID_t)
			check_id_lgth (&(sxsvar.sxlv.terminal_token));

		    break;
		
	    }
	}

	return 0;
	
    case 2: {
	/* Traitement des chaines entre quotes.
	   Ce n'est pas de l'ansi standard, dans tous les cas on les change en apostrophe. */
	register char *str;
	register int strlen ;

	if (is_ansi && !is_quoted_string_already_met)  {
	    is_quoted_string_already_met = TRUE;
	    sxput_error (sxsvar.sxlv.terminal_token.source_index,
			 "%sIn the standard, CHARACTER constants must be enclosed between apostrophes,\n\
This one, and all others in the sequel, are converted into a legal form.",
			 sxsvar.sxtables->err_titles [1] /* warning */);
	}

	str = sxstrget (sxsvar.sxlv.terminal_token.string_table_entry) + 1; /* apres " */
	strlen = sxstrlen (sxsvar.sxlv.terminal_token.string_table_entry) - 2;
	sxsvar.sxlv.ts_lgth = 0;
	ts_put ('\'');

	/* On double les apostrophes. */
	while (strlen-- > 0) {
	    if (*str == '\'')
		ts_put ('\'');

	    ts_put (*str++);
	}

	ts_put ('\'');
	sxsvar.sxlv.terminal_token.string_table_entry =
	    sxstr2save (sxsvar.sxlv_s.token_string, sxsvar.sxlv.ts_lgth);
    }

	return 0;
	
    case 3:
	/* On est sur eol, traitement du enddo */
	if (enddo_nb != 0) {
	    struct sxtoken	tt;
	    /* on doit generer des enddo */
	    
	    tt = sxsvar.sxlv.terminal_token;
	    tt.lahead = ENDDO_t;

	    while (enddo_nb > 0) {
		/* On range les  eol enddo */
		enddo_nb--;
		sxput_token(sxsvar.sxlv.terminal_token);
		sxput_token(tt);
	    }
	}

	    return 0;

	case 4: {
	    /* traitement de la zone Hollerith */

	    if (sxsvar.sxlv_s.counters [0]) {
		/* On est dans un format, occurrence licite.
		   Les erreurs eventuelles ont deja ete sorties, mais la longueur
		   annoncee peut ne pas etre compatible avec la longueur reelle. */
		register int val, val_lgth, val_init;
		char string [3];

		val = val_init = atoi (sxsvar.sxlv_s.token_string);
		val_lgth = sxsvar.sxlv.ts_lgth - 1;
		/* H est deja stocke. */
		/* On stocke val (on s'arrete sur eol) caracteres. */

		while (val-- > 0 && sxsrcmngr.current_char != '\n') {
		    ts_put (sxsrcmngr.current_char);
		    f77_next_char ();
		}

		if (val > 0) {
		    /* On diminue la longueur annoncee. */
		    /* Il faut representer (val_init - val) sur val_lgth caracteres. */
		    /* val_init - val tient sur 2 chiffres. */
		    register int ts_lgth = sxsvar.sxlv.ts_lgth;

		    sprintf (string, "%.2d", val_init - val);
		    sxsvar.sxlv.ts_lgth = 0;

		    while (val_lgth > 2) {
			ts_put ('0');
			val_lgth--;
		    }

		    if (val_lgth == 2)
			ts_put (string [0]);

		    ts_put (string [1]);
		    sxsvar.sxlv.ts_lgth = ts_lgth;
		}
	    }
	    else if (SXGET_TOKEN (sxplocals.Mtok_no).lahead == STAR_t) {
		/* On regarde si le token courant est precede' par *,
		   dans ce cas, il s'agit d'une <22:len_specification> suivie d'un ident
		   qui commence par 'H'. */
		/* ds ce cas les blancs on du etre elimines par "fill_buffer". */
		register char *str = sxsvar.sxlv_s.token_string;
		register int lgth = 0;
		register SHORT c;

		while (isdigit (str [lgth]))
		    lgth++;

		sxsvar.sxlv.terminal_token.lahead = ENTIER_t;
		sxsvar.sxlv.terminal_token.string_table_entry = sxstr2save (str, lgth);
		sxput_token(sxsvar.sxlv.terminal_token);

		/* On lit (manuellement) l'ident */
		sxsvar.sxlv.ts_lgth = 0, ts_put ('H');
		c = sxsrcmngr.current_char;

		while (isalnum (c)) {
		    if (islower (c))
			c = toupper (c);
			
		    ts_put (c);
		    c = f77_next_char ();
		}
		
		sxsvar.sxlv.terminal_token.lahead = ID_t; 
		sxsvar.sxlv.terminal_token.source_index =
		    *get_source_index (&(sxsvar.sxlv.terminal_token.source_index), lgth);

		if (is_ansi)
		    check_id_lgth (&(sxsvar.sxlv.terminal_token));
	    }
	    else {
		/* On proteste si is_ansi et si c'est la premiere fois.
		   Dans tous les cas on change la definition. */
		register int val;

		if (is_ansi && !is_hollerith_already_met)  {
		    is_hollerith_already_met = TRUE;
		    sxput_error (sxsvar.sxlv.terminal_token.source_index,
				 "%sIn the standard, Hollerith constants can only occur inside format specifications,\n\
This one, and all others in the sequel, are converted into a CHARACTER constant.",
				 sxsvar.sxtables->err_titles [1] /* warning */);
		}

		val = atoi (sxsvar.sxlv_s.token_string);
		sxsvar.sxlv.ts_lgth = 0;
		ts_put ('\'');
		/* On stocke val (on s'arrete sur eol) caracteres. */

		/* On double les apostrophes. */
		while (val-- > 0 && sxsrcmngr.current_char != '\n') {
		    if (sxsrcmngr.current_char == '\'')
			ts_put ('\'');

		    ts_put (sxsrcmngr.current_char);
		    f77_next_char ();
		}

		ts_put ('\'');
	    }

	    sxsvar.sxlv.terminal_token.string_table_entry =
		sxstr2save (sxsvar.sxlv_s.token_string, sxsvar.sxlv.ts_lgth);
	}

	    return 0;

	case 5: {
	    /* traitement de int||id */
	    /* On regarde si le token courant est precede' par CHARACTER * */
	    TOK_i = sxplocals.Mtok_no - 1;

	    if (SXGET_TOKEN (sxplocals.Mtok_no).lahead == STAR_t /* &&
		SXGET_TOKEN (TOK_i).lahead == CHARACTER_t */) {
		register char *str = sxsvar.sxlv_s.token_string;
		register int lgth, prev_lahead;

		lgth = 0;

		while (isdigit (str [lgth]))
		    lgth++;

		switch (prev_lahead = sxsvar.sxlv.terminal_token.lahead) {
		case UDPC_t:
		    /* int E [+-] int */
		case URC_t:
		    /* int D [+-] int */
		    if (str [lgth+1] == '+' || str [lgth+1] == '-')
			/* Erreur, tant pis, on laisse courrir... */
			break;

		case INT_ID_t:
		    sxsvar.sxlv.terminal_token.lahead = ENTIER_t;
		    sxsvar.sxlv.terminal_token.string_table_entry = sxstr2save (str, lgth);
		    sxput_token(sxsvar.sxlv.terminal_token);

		    if (prev_lahead == INT_ID_t &&
			strncmp (str + lgth, "FUNCTION", 8) == 0 &&
			isalpha (str [lgth + 8])) {
			sxsvar.sxlv.terminal_token.lahead = FUNCTION_t;
			sxsvar.sxlv.terminal_token.string_table_entry = EMPTY_STE;
			sxsvar.sxlv.terminal_token.source_index =
			    *get_source_index (&(sxsvar.sxlv.terminal_token.source_index), lgth);
			sxput_token(sxsvar.sxlv.terminal_token);
			is_function_stmt = TRUE;
			lgth += 8;
		    }

		    sxsvar.sxlv.terminal_token.lahead = ID_t;
		    sxsvar.sxlv.terminal_token.string_table_entry = sxstrsave (str + lgth);
		    sxsvar.sxlv.terminal_token.source_index =
			*get_source_index (&(sxsvar.sxlv.terminal_token.source_index), lgth);

		    if (is_ansi)
			check_id_lgth (&(sxsvar.sxlv.terminal_token));

		    break;
		}
	    }
	}

	return 0;

    case 6:
	/* @Lower_To_Upper + test ansi */
	sxsvar.sxlv_s.token_string [sxsvar.sxlv.ts_lgth - 1] += 'A' - 'a';

	if (is_ansi && !is_lower_to_upper_already_met)  {
	    is_lower_to_upper_already_met = TRUE;
	    sxput_error (*get_source_index (&sxsrcmngr.source_coord, -1),
			 "%sIn the standard, lower case letters ouside comments & strings are illegal.\n\
This one, and all others in the sequel, are converted into the corresponding upper case version.",
			 sxsvar.sxtables->err_titles [1] /* warning */);
	}

	return 0;


    case 7:
	/* On est sur EOF */
	/* On verifie qu'il y a eu un MAIN program (on a deja verifie qu'il n'y en a pas
	   eu plus d'un). */
	if (is_ansi && main_program_nb == 0)
	    sxtmsg (sxsvar.sxlv.terminal_token.source_index.file_name,
			 "%sThis executable program must contain one main program.",
			 sxsvar.sxtables->err_titles [2] /* Error */);

	return 0;


	default:
	    fprintf (sxstderr, "\"%d\"is an illegal action number in \"f77_scanact\".\n", act_no);
	    abort ();
	}

	break;

    case PREDICATE:
	switch (act_no) {
	case 0: {
	    /* On est sur un chiffre, est-il entre les colonnes 1 a 5 ? */
	    register int col;

	    col = sxsrcmngr.source_coord.column;
	    return col >= 1 && col <= 5;
	}

	default:
	    fprintf (sxstderr, "\"%d\"is an illegal predicate number in \"f77_scanact\".\n", act_no);
	    abort ();
	}


    default:
	fputs ("The function \"f77_scanact\" is out of date with respect to its specification.\n", sxstderr);
	abort ();
    }
}
