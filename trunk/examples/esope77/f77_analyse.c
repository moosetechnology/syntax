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

static char ME [] = "f77_analyse.c";

#define ESOPE

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

************************************************************************/



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

#include "sxversion.h"
#include "sxunix.h"
#include "f77_td.h"
#include <ctype.h>

char WHAT_F77ANALYSE[] = "@(#)SYNTAX - $Id: f77_analyse.c 4130 2024-07-29 12:27:32Z garavel $";

/* Il faudrait INTERDIRE l'insertion d'un EOL par le rattrapage d'erreur
   du scanner. Utiliser sxs_srcvr? */
/* Pierre : il semblerait que sxplocals.rcvr.truncate_context soit toujours positionné à vrai au début de chaque traitement d'erreur!!
   Dans ce cas, il semblerait que les tokens de validation des modèles ne soient pas pris en compte au-dela des tokens de l'ensemble
   Key-Terminals du .recor.  Par exemple si on examine le modèle "X 2 3 4", que	Key_Terminals = {"%EOL"} et que le token en position
   "3" soit %EOL, alors le token en position "4" ne sera pas examiné et ce modèle pourra être validé comme s'il était "X 2 3". */


extern bool		is_ansi, is_json, is_pretty_printer, is_input_free_fortran, is_extension; /* Options d'appel */

/* assert: is_json => on execute f77 (reciproque fausse) */
/* assert: is_pretty_printer <=> on execute ppf77 */
/* assert: !(is_json && is_pretty_printer) */

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
#define DOUBLECOMPLEX_p	35 /* [[EXTENSION]] */
#define ENDDO_p		36 /* [[EXTENSION]] */
#define NONE_p		37 /* [[EXTENSION]] */
#define UNDEFINED_p	38 /* [[EXTENSION]] */

#ifdef ESOPE
#define ENDSEGMENT_p	39 /* [[ESOPE]] */
#define IMPLIED_p	40 /* [[ESOPE]] */
#define POINTEUR_p	41 /* [[ESOPE]] */
#define SEGACT_p        42 /* [[ESOPE]] */
#define SEGADJ_p        43 /* [[ESOPE]] */
#define SEGDES_p        44 /* [[ESOPE]] */
#define SEGINA_p        45 /* [[ESOPE]] */
#define SEGIND_p        46 /* [[ESOPE]] */
#define SEGINI_p        47 /* [[ESOPE]] */
#define SEGMENT_p       48 /* [[ESOPE]] */
#define SEGPRT_p        49 /* [[ESOPE]] */
#define SEGSUP_p        50 /* [[ESOPE]] */
#endif // [[ESOPE]]

static char *kw_prefixes [] = {"", "DO", "ASSIGN", "CALL", "GOTO", "RETURN", "PRINT", "READ",
				   "BACKSPACE", "ENDFILE", "REWIND", "INTEGER", "REAL",
				   "DOUBLEPRECISION", "COMPLEX", "LOGICAL", "CHARACTER",
				   "ENTRY", "SUBROUTINE", "BLOCKDATA", "COMMON",
				   "PARAMETER", "EXTERNAL", "INTRINSIC", "FUNCTION",
				   "CONTINUE", "STOP", "PAUSE", "SAVE", "DATA",
				   "IMPLICIT", "DIMENSION", "PROGRAM", "ELSEIF", "ENDIF"
/* [[EXTENSION]] */		   , "DOUBLECOMPLEX", "ENDDO", "NONE", "UNDEFINED"
#ifdef ESOPE
				   , "ENDSEGMENT", "IMPLIED", "POINTEUR", "SEGACT", "SEGADJ", "SEGDES"
				   , "SEGINA", "SEGIND", "SEGINI", "SEGMENT", "SEGPRT", "SEGSUP"
#endif // [[ESOPE]]
                              };
static SXINT kw_codes [] = {0, DO_t, ASSIGN_t, CALL_t, GO_t, RETURN_t, PRINT_t, READ_KW_t,
			      BACKSPACE_KW_t, ENDFILE_KW_t, REWIND_KW_t, INTEGER_t, REAL_t,
			      DOUBLE_t, COMPLEX_t, LOGICAL_t, CHARACTER_t,
			      ENTRY_t, SUBROUTINE_t, BLOCK_t, COMMON_t,
			      PARAMETER_t, EXTERNAL_t, INTRINSIC_t, FUNCTION_t,
			      CONTINUE_t, STOP_t, PAUSE_t, SAVE_t, DATA_t,
			      IMPLICIT_t, DIMENSION_t, PROGRAM_t, ELSE_t, END_t
/* [[EXTENSION]] */           , DOUBLE_t, END_t, NONE_t, UNDEFINED_t
#ifdef ESOPE
			      , END_t, IMPLIED_t, POINTEUR_t, SEGACT_t, SEGADJ_t, SEGDES_t
			      , SEGINA_t, SEGIND_t, SEGINI_t, SEGMENT_t, SEGPRT_t, SEGSUP_t
#endif // [[ESOPE]]
                           };
static SXINT kw_lgth [] = {0, 2, 6, 4, 4, 6, 5, 4,
			     9, 7, 6, 7, 4,
			     15, 7, 7, 9,
			     5, 10, 9, 6,
			     9, 8, 9, 8,
			     8, 4, 5, 4, 4,
			     8, 9, 7, 6, 5
/* [[EXTENSION]] */	     , 13, 5, 4, 9
#ifdef ESOPE
			     , 10, 7, 8, 6, 6, 6
			     , 6, 6, 6, 7, 6, 6
#endif // [[ESOPE]]
                           };

struct do_item {
    SXINT			label;
    struct sxsource_coord	source_index;
};

static struct do_item	*do_stack;
static SXINT		do_stack_top, do_stack_size, do_stack_xlbl;
static struct sxtoken   *tt_stack;
static SXINT            tt_stack_top, tt_stack_size;

#define f77PUSH(t,x)	t[x]=t[0],t[0]=x

static SXINT		*t_stack;
static SXINT		*iokw_stack;
static SXINT		iokw_stack_top, iokw_stack_size;

struct source_item {
    short			c;
    SXUINT                      last_non_blank_col, eol_col;
    struct sxsource_coord	source_index;
};

static struct source_item	source_item;

struct source_line {
    short			*buffer;
    struct sxsource_coord	*bufcoords;
    SXINT			buflength;
    SXINT			bufused;
};

static struct source_line	*source_line;

#define SOURCE_BUF_SIZE		4
#define INCR(x)			((x)+1 == SOURCE_BUF_SIZE ? 0 : (x)+1)

static struct source_line	source_lines [SOURCE_BUF_SIZE];
static SXINT			source_line_head, source_line_tail;

#ifdef ESOPE
static SXINT                    star_flag_mode_tok_no;
#endif // [[ESOPE]]

static bool is_tab_already_met, is_hollerith_already_met, is_quoted_string_already_met,
  is_lower_to_upper_already_met, is_implicit_extension_already_met;

/* Pierre le 16/03/2023 : grand chantier, dans le cas pretty-printer, je change tout à la gestion des commentaires.

   Principes : On ne doit pas en perdre et ils doivent être sortis dans le même ordre.
   Problèmes : Dans SYNTAX les commentaires sont associes aux tokens (plus précisemment aux coordonnées du 1er
               caractère de ce token).  En FORTRAN on peut mettre des commentaires dans les tokens.
   Question :  Les lignes vides (lv) sont-elles des commentaires?
               En fait, il y a deux formes de lv, les lv syntaxiques (lvs) et les lv commentaires (lvc).
	       Les lvs sont introduites par la version pp de la bnf, les lvc par l'écrivain du programme.
	       Les lv ne peuvent pas être gardées car elles proliféreraient au fur et à mesure des pp (à chaque
	       passage, on ajouterait les lvs).  Or, dans le cas général, on ne sait pas reconnaître une lvs d'une lvc.
	       En fait les lvs ne peuvent se trouver que juste après un token (une ligne instruction en f77), mais une telle
	       lv peut aussi être une lvc.  On va donc supprimer les lv situées après une ligne instruction et garder
	       les autres (on les considère donc comme des commentaires).

   Finalement, on gère les commentaires indépendemment de SYNTAX.  Ils sont collectés (séparés des instructions)
   et stockés au niveau le plus bas (lecture du caractère) et réinsérés directement dans l'arbre construit pour
   le pretty-printer par le parser.  Donc toute les parties analyses lexicale et syntaxique s'exécutent comme s'il n'y
   avait pas de commentaires à gérer.  Pour cela, il faut, dans le cas ppf77, recompiler sxatcpp.c et sxppp.c avec les
   options
               -DPOSTPONED_COMMENTS=f77_process_postponed_comments
   et
   	       -DUSERS_COMMENTS=1
   et utiliser les .o dans ppf77.out

   En particulier, les commentaires n'etant pas stockes dans le champ 'comment'
   de la structure sxtoken, on ne pourra pas y acceder via la meta-fonction
   $pcomment() de SEMC.
*/

#include "sxstack.h"
#include "sxba.h"
#include "varstr.h"

/* uniquement si is_json (c-a-d si l'option -json de f77 est positionnee) */
FILE *json_pipe = NULL;
bool first_comment = true;

/* uniquement si is_pretty_printer */
static struct comments {
  SXINT    *offsets; /* DSTACK, oflw automatique, offset[i] est l'offset dans buf de la ième carte  */
  SXINT    *lines_no; /* DSTACK, oflw automatique, lines_no[i] est le no de ligne de la ième carte  */
  VARSTR   vstr; /* on y met les commentaires */
} comments;

static struct stmts {
  SXINT     continuation_line_no;
  bool is_eof_hit, is_a_continuation_card;
  VARSTR    string;
} stmts;

static void stmts_alloc (void) {
  /* On part avec la taille d'une carte (aucune réallocation si is_input_free_fortran == false */
  stmts.string = varstr_alloc (84);
  stmts.continuation_line_no = 0;
  stmts.is_eof_hit = false;
  stmts.is_a_continuation_card = false;
}


static void stmts_free (void) {
  varstr_free (stmts.string);
}


static char stmts_put_a_char (SXINT col, char c) {
  while (is_input_free_fortran && varstr_maxlength (stmts.string) <= col)
    varstr_realloc (stmts.string);

  return varstr_tostr (stmts.string) [col] = c;
}


static char stmts_get_a_char (SXINT col) {
  return varstr_tostr (stmts.string) [col];
}


static void comments_alloc (SXINT size) {
  /* assert is_pretty_printer */
  size = size >> 2;
  
  if (size == 0)
    size = 2048;

  comments.vstr = varstr_alloc (size);
  DALLOC_STACK (comments.offsets, 128);
  DALLOC_STACK (comments.lines_no, 128);
}

static void comments_free (void) {
  /* assert is_pretty_printer */
  varstr_free (comments.vstr), comments.vstr = NULL;
  DFREE_STACK (comments.offsets);
  DFREE_STACK (comments.lines_no);
}

#define varstr_charptr(v,l) ((v)->first+(l))

static void tab_hit (struct sxsource_coord source_index)
{
    if (!is_extension && !is_tab_already_met) {
	is_tab_already_met = true;
		
	sxerror (source_index,
		 sxsvar.sxtables->err_titles [1][0] /* warning */,
		 "%sA tabulation character is illegal outside comments or character constants.\n\
In this program, each such tabulation character is replaced by spaces.",
		 sxsvar.sxtables->err_titles [1]+1 /* warning */);
    }
}

static unsigned int COL_MAX;

static void dump_comment_char (char c)
{
    /* assert is_json && (json_pipe != NULL) */
    if (c == '"')
       fprintf (json_pipe, "\\\"");
    else if (c == SXNEWLINE)
       fprintf (json_pipe, "\\n");
    else
       fprintf (json_pipe, "%c", c);
}

static bool is_a_comment_flag (short c) {
     /* c est un caractère (lu par getc ()) en colonne 1, on regarde s'il commence une carte commentaire */
     return c == 'C' || c == 'c' || c == '*' /* f77 */
	  || c == '!' /* extension */
	  || c == 'D' || c == 'd' /* Oracle */
	  || c == '#' /* include */
	  ;
}

/* Lit le caractère suivant de infile. Ce caractère est éliminé ssi c'est un '\r' (carriage return).
   Dans ce cas, le caractère suivant doit être un '\n' qui est retourné */
static short f77getc (FILE *infile) {
     short c;

     c = getc (infile);

     if (c == '\r') {
	  c = getc (infile);

	  if (c != '\n')
	       sxtrap (ME, "Illegal carriage return detected in f77getc");
     }

     return c;
}


/* Remplit la structure stmts avec la prochaine carte stmt (initiale ou continuation) suivante.
   Cette carte stmt peut être EOF.
   Si !is_pretty_printer && !is_json, les commentaires (précédents cette carte) sont éliminés.
   Si is_json, ils sont affiches sur json_pipe.
   Si is_pretty_printer, ils sont stockés dans la structure comments pour être exploités (beaucoup) plus tard
   après la construction de l'arbre d'analyse permettant d'exploiter les instructions de pp */
/* Traitement du free fortran (gouverné par is_input_free_fortran) :
   - suppression de la limite à 80 charactères, les colonnes 73 a 80 (et plus) deviennent des colonnes "normales"
   - ça ne supprime pas la possible existence de lignes suites, i.e. avec un caractere en colonne 6 */

static void fill_comments_and_stmts (void)
{
  short       c, cc;
  SXUINT        x;
  SXINT         contiguous_empty_lines_nb, first_non_blank_col, last_non_blank_col;
  bool	ligne_vide = false, is_header_empty;

#if EBUG
  SXINT prev_comments_top;
#endif

  /* source_item référence le caractère précédent */

  if (stmts.is_eof_hit) {
    /* Le coup précédent, on a rencontré le EOF en fin de ligne sans EOL!! */
    /* On dit qu'il est tout seul sur la ligne suivante */
    source_item.source_index.line++;
    source_item.c = EOF;
    source_item.source_index.column = 1;

    return;
  }

#if EBUG
  if (is_pretty_printer) {
    prev_comments_top = DTOP (comments.lines_no);
  }
#endif
	
  contiguous_empty_lines_nb = 0;

  do {
    /* On elimine les commentaires */
    c = f77getc (sxsrcmngr.infile);

    if (is_a_comment_flag (c)) {
      if (is_json || is_pretty_printer) {

        if (is_json) {
          /* code specifique a f77 pour la traduction en JSON */
          /* assert json_pipe != NULL */
          if (first_comment) {
            fprintf (json_pipe, "\n");
            first_comment = false;
          } else {
            fprintf (json_pipe, ",\n");
          }
          fprintf (json_pipe, "\"%lu\":\"", source_item.source_index.line+1);
        } else {
	  /* On ne push que le 1er commentaire d'un groupe */
	  DPUSH (comments.lines_no, source_item.source_index.line+1);
	  DPUSH (comments.offsets, varstr_length (comments.vstr));
        }
      
	do {
          if (is_json)
            dump_comment_char (c);
          else
	    varstr_catchar (comments.vstr, c);

	  while ((c = f77getc (sxsrcmngr.infile)) != SXNEWLINE && c != EOF) {
            if (is_json)
              dump_comment_char (c);
            else
	      varstr_catchar (comments.vstr, c);
	  }

          if (is_json)
            dump_comment_char (SXNEWLINE);
          else
	    varstr_catchar (comments.vstr, SXNEWLINE);

	  ++source_item.source_index.line; /* No de la carte commentaire que l'on vient de stocker */

	  if (c == EOF)
	    break;

	  c = f77getc (sxsrcmngr.infile);
	} while (is_a_comment_flag (c));
	
        if (is_json) {
          fprintf (json_pipe, "\"");
        } 
      }
      else {
	do {
	  while ((c = f77getc (sxsrcmngr.infile)) != SXNEWLINE && c != EOF);

	  ++source_item.source_index.line;

	  if (c == EOF)
	    break;

	  c = f77getc (sxsrcmngr.infile);
	} while (is_a_comment_flag (c));
      }
    }

    /* c contient le 1er caractère d'une ligne et ce n'est pas une ligne commentaire, mais elle peut être vide */
    /* c peut être EOF, il est sensé être situé en ligne source_item.source_index.line+1, colonne 1 */
    is_header_empty = true;
    ++source_item.source_index.line; /* repère la carte courante */
    last_non_blank_col = first_non_blank_col = 0;
    x = 1; /* No de colonne de c */

    /* on remplit stmts.string */
    while (c != SXNEWLINE && c != EOF && x <= 5) {
      if (isdigit (c)) {
	is_header_empty = false;
	last_non_blank_col = x;

	if (first_non_blank_col == 0)
	  /* On repère la colonne du label */
	  first_non_blank_col = x;
      }
      else if (c != ' ') {
	source_item.source_index.column = x;

	if (c == '\t') {
	  tab_hit (source_item.source_index);

	  while (x <= 5) {
	    /* simulation du \t */
	    stmts_put_a_char (x++, ' ');
	  }

	  c = ' ';

	  break;
	}
	else {
	  sxerror (source_item.source_index,
		   sxsvar.sxtables->err_titles [2][0] /* error */,
		   "%sThe illegal character \"%s\" is changed into blank.",
		   sxsvar.sxtables->err_titles [2]+1 /* error */,
		   SXCHAR_TO_STRING (c));
	}

	c = ' ';
      }

      stmts_put_a_char (x++, (char)c);
      c = f77getc (sxsrcmngr.infile);
    }

    if (x == 6) {
      /* si la ligne est vide, elle le reste */
      if (c == SXNEWLINE || c == EOF)
	 cc = ' ';
      else {
         /* On mémorise le caractère en colonne 6 dans cc. */
         cc = (char)c;
         stmts_put_a_char (x++, ' ');
         c = f77getc (sxsrcmngr.infile);
      }
    }
    else {
      /* x < 6 && (c == SXNEWLINE || c == EOF) => ... */
      cc = ' '; /* ... carte vide ou initiale ne contenant qu'un label */
    }

    if (cc == '0')
      /* debut de carte instruction */
      cc = stmts_put_a_char (6, ' ');

    /* c contient le caractère de la colonne 6, on est sur une carte stmt, initiale ou continuation */
    stmts.is_a_continuation_card = (cc != ' ');

    if (!stmts.is_a_continuation_card) {
      stmts.continuation_line_no = 0;
    }	
    else {
      if (++stmts.continuation_line_no == 20 && is_ansi) {
	source_item.source_index.column = 6;
	sxerror (source_item.source_index,
		 sxsvar.sxtables->err_titles [1][0] /* Warning */,
		 "%sContinuation line number exceeded.",
		 sxsvar.sxtables->err_titles [1]+1 /* Warning */);
      }
    }

    if (!is_header_empty && stmts.is_a_continuation_card) {
      SXINT y;

      source_item.source_index.column = first_non_blank_col;

      sxerror (source_item.source_index,
	       sxsvar.sxtables->err_titles [2][0] /* error */,
	       "%sA label is not allowed in a continuation line, it is erased.",
	       sxsvar.sxtables->err_titles [2]+1 /* error */);

      for (y = first_non_blank_col; y < 6; y++) {
	stmts_put_a_char (y, ' ');
      }
      
      last_non_blank_col = first_non_blank_col = 0;
      is_header_empty = true;
    }

    ligne_vide = is_header_empty;

    while (c != SXNEWLINE && c != EOF && x <= COL_MAX) {
      if (c != ' ') {
	last_non_blank_col = x;
	ligne_vide = false;

	/* On note la colonne du 1er caractère non blanc de la carte dans la partie instruction */
	if (x > 6 && first_non_blank_col == 0)
	  first_non_blank_col = x;
      }

      stmts_put_a_char (x++, (char)c);	    
      c = f77getc (sxsrcmngr.infile);
    }

    /* Ici soit on a rencontré un eol (ou eof) soit x == 73, la ligne est donc finie */
    if (!is_input_free_fortran) {
      /* On transforme en blancs les caractères de 73 à 80 */
      while (c != SXNEWLINE && c != EOF && x <= 80) {
	stmts_put_a_char (x++, ' ');
	c = f77getc (sxsrcmngr.infile);
      }

      /* On recherche le eol */
      /* On ignore les caractères au-dela de 80 */
      while (c != SXNEWLINE && c != EOF) {
	/* x++; on cesse de repérer les colonnes */
	c = f77getc (sxsrcmngr.infile);
      }

      /* On a x <= 81 */
    }
    
    if (ligne_vide && c != EOF) {
      if (cc == ' ') {
	if (is_json || is_pretty_printer) {
	  /* Les lignes vides sont conservées (elles seront éventuellement éliminées plus tard) */
	  if (++contiguous_empty_lines_nb == 1) {
            if (is_json)
              /* rien */ ;
            else {
	      /* On ne push que la 1ere, les suivantes sont concaténées */
	      DPUSH (comments.lines_no, source_item.source_index.line);
	      DPUSH (comments.offsets, varstr_length (comments.vstr));
            }    
	  }

          if (is_json)
            /* ne pas faire : dump_comment_char (SXNEWLINE) */ ;
          else
	    varstr_catchar (comments.vstr, SXNEWLINE);
	}
      }
      else {
	/* carte qui a un caractère non blanc en colonne 6 et dont le reste est vide */
	/* On la supprime silencieusement (à voir)!! (pb avec les chaînes de caractères de tout type??) */
      }
    }
  } while (ligne_vide && c != EOF);

  if (c == EOF)
    stmts.is_eof_hit = true;
  else  {
    /* carte qui a un caractère non blanc en colonne 6 et dont le reste est vide */
    /* On la supprime silencieusement (à voir)!! */

    /* Ici, on a traité les commentaires et les lignes vides d'un groupe et buf contient la carte stmt suivante
       (carte initiale ou continuation). */
    
    /* On complete l'entete... */
    while (x <= 5)
      stmts_put_a_char (x++, ' ');
  }

  stmts_put_a_char (x, SXNEWLINE); /* eol dans tous les cas et x est sa colonne */

  source_item.c = first_non_blank_col ? stmts_get_a_char (first_non_blank_col) : EOF;
  source_item.source_index.column = (first_non_blank_col == 0) ? 1 : first_non_blank_col;
  source_item.eol_col = x;
  source_item.last_non_blank_col = last_non_blank_col;
  

#if EBUG
  {
    SXINT top, line_no, binf, bsup;

    stmts_put_a_char (x+1, SXNUL);

    fprintf (stdout, "%s(fill_stmts) : line %ld, stmt  = \"%s\"\nlast_non_blank_col = %ld, EOL_col = %ld\n",
	     ME,
	     source_item.source_index.line,
	     source_item.c == EOF ? "(EOF)" : varstr_tostr (stmts.string),
	     source_item.last_non_blank_col,
	     source_item.eol_col);

    if (is_pretty_printer) {
      top = DTOP (comments.lines_no);
    
      fprintf (stdout, "%s(fill_comments(%ld))\n", ME, top-prev_comments_top);

      while (++prev_comments_top <= top) {
	binf = comments.offsets [prev_comments_top];
	bsup = prev_comments_top == top ? varstr_length (comments.vstr) : comments.offsets [prev_comments_top+1];
	line_no = comments.lines_no [prev_comments_top];

	c = (bsup == 0) ? EOF : *varstr_charptr (comments.vstr, bsup);
	*varstr_charptr (comments.vstr, bsup) = SXNUL;
	fprintf (stdout, "line %ld : comment  = \"%s\"\n",
		 line_no,
		 (c == EOF) ? "(EOF)" : varstr_charptr (comments.vstr, binf));

	if (c != EOF)
	  *varstr_charptr (comments.vstr, bsup) = c;
      }
    }
  }
#endif
}



static struct ppc {
  SXINT last_offset, top, prev_comments_top, binf, bsup, line_no;
} ppc;

static void init_ppc (void) {
  /* assert is_pretty_printer */
  ppc.last_offset = varstr_length (comments.vstr); /* Longueur totale de tous les commentaires */
  ppc.top = DTOP (comments.lines_no); /* nombre de commentaires */
  ppc.prev_comments_top = 0; /* nombre de commentaires déjà traités */
  ppc.line_no = ppc.binf = ppc.bsup = 0; /* longueur totale des commentaires déjà traités */
}


/* Met dans la structure ppc (process_postponed_comments) les paramètres du prochain commentaire.
   Il y en a un ssi ppc.line_no != 0 et il commence à cette ligne */

static void first_comment_ppc (void) {
    /* assert is_pretty_printer */
  if (ppc.last_offset == ppc.bsup)
    /* Tous les commentaires ont été vus... */
    ppc.line_no = 0;
  else {
    /* ... ici, il en reste donc */
    ++ppc.prev_comments_top; /* Le 1er de ce groupe à ce numéro */
    ppc.binf = comments.offsets [ppc.prev_comments_top]; /* Il commence ici dans vstr... */
    ppc.bsup = ppc.prev_comments_top == ppc.top ? ppc.last_offset
      : comments.offsets [ppc.prev_comments_top+1]; /* ... et se termine là */
    ppc.line_no = comments.lines_no [ppc.prev_comments_top]; /* Il commence à ce numéro de ligne */
    /* Le 1er commentaire d'index prev_comments_top (il y en a top) se trouve dans vstr [binf:bsup] et se trouve
       dans le source à la ligne line_no (attention, ce sont peut-être des lignes vides.
    */
  } 
}

/* Retourne (dans ppc) la concaténation de tous les commentaires positionnés avant la ligne line_no
   (il y en a au moins 1) */

static void regroup_ppc (SXINT line_no) {
  /* ppc.prev_comments_top repère le 1er commentaire du groupe */

  /* assert is_pretty_printer */
  while (ppc.prev_comments_top < ppc.top) {
    if (comments.lines_no [ppc.prev_comments_top+1] > line_no)
      break;

    ppc.bsup = (++ppc.prev_comments_top == ppc.top) ? ppc.last_offset
      : comments.offsets [ppc.prev_comments_top+1];
  }

  /* ppc.prev_comments_top repère le dernier commentaire du groupe et ppc.line_no est (toujours) la ligne
   du 1er commentaire du groupe */
}

/* On garde, supprime à gauche, supprime à droite ou supprime à gauche et à droite les lignes vides des commentaires */
#define noBP 0
#define lBP  1
#define rBP  2
#define lrBP 3

#include <string.h>

/* Crée un commentaire avec les paramètres de ppc en supprimant les lignes vides de tête et de queue suivant
   la valeur de BP_type */

static char *ppc2comment (SXINT BP_type) {
  SXINT binf, bsup;
  char *comment;

  /* assert is_pretty_printer */
  binf = ppc.binf;
  bsup = ppc.bsup;

  if (BP_type & lBP) {
    /* Suppression des lignes vides de tête */
    while (binf < bsup && *varstr_charptr (comments.vstr, binf) == SXNEWLINE)
      binf++;
  }

  if (binf < bsup && (BP_type & rBP)) {
    /* Suppression des lignes vides de queue (attention, le dernier caractère doit être un EOL */
    while (binf < bsup && *varstr_charptr (comments.vstr, --bsup) == SXNEWLINE);

    bsup += 2;
  }

  if (binf < bsup) {
    /* On utilise le comment_mngr.c */
    comment = sxcomment_put (varstr_charptr (comments.vstr, binf), (size_t) (bsup - binf));
  }
  else
    comment = NULL;

#if EBUG
  fprintf (stdout, "%s(ppc2comment) binf = %ld, bsup = %ld : line = %ld, comment = \"%s\"\n", ME,
	   (long) binf,
	   (long) bsup,
	   ppc.line_no,
	   comment == NULL ? "(void)" : comment);
#endif

  return comment;
}

/* Cette procédure parcourt la liste des noeuds construits par atcpp et remplit les champs (pre_)comment et post_comment
   avec les commentaires créés à partir de la structure comments */

void f77_process_postponed_comments (struct sxnode_pp *root_ptr) {
  struct sxnode_pp *brother_ptr;

  /* assert is_pretty_printer */
  init_ppc ();

  if (ppc.last_offset != 0 /* Y'a du commentaire dans la structure comments */
      && root_ptr != NULL /* et de l'arbre */
      ) {
    first_comment_ppc (); /* On extrait le 1er commentaire */

    if ((SXUINT)ppc.line_no < root_ptr->terminal_token.source_index.line /* ce commentaire est en tête */) {
      /* On le sort */
      regroup_ppc (root_ptr->terminal_token.source_index.line); /* on forme un "groupe de commentaires... */
      root_ptr->terminal_token.comment = ppc2comment (lBP); /* ... que l'on associe au champ comment du noeud courant */
      first_comment_ppc (); /* On extrait le commentaire suivant */
    }
    
    brother_ptr = root_ptr;

    while (brother_ptr != NULL /* Ce n'est pas la fin de l'arbre */
	   && ppc.line_no != 0 /* Il reste des commentaires à sortir */
	   ) {
      if (brother_ptr->terminal_token.lahead == EOL_t) {
	/* On est sur un noeud du terminal générique %EOL qui marque la fin d'un stmt.
	   Il sera suivi d'une directive "SKIP".
	   On va sortir un (groupe) de commentaires que l'on va associer (en pré) au noeud suivant */
	if ((brother_ptr = brother_ptr->next) != NULL /* %EOL n'est pas le dernier noeud */) {
	  if ((SXUINT)ppc.line_no < brother_ptr->terminal_token.source_index.line /* il y a du commentaire à sortir */) {
	    regroup_ppc (brother_ptr->terminal_token.source_index.line); /* on forme un "groupe de commentaires... */
	    brother_ptr->terminal_token.comment = ppc2comment (lBP); /* ... que l'on associe au champ comment du noeud courant */
	    first_comment_ppc ();
	  }

	  brother_ptr = brother_ptr->next; /* On passe au noeud suivant */	  
	}
      }
      else
	brother_ptr = brother_ptr->next; /* On passe au noeud suivant */
    }

    if (ppc.line_no != 0) {
      /* Il reste du commentaire devant EOF, on le rattache en post au noeud racine */
	regroup_ppc (ppc.line_no+1);
	root_ptr->post_comment = ppc2comment (lrBP); /* pas de lignes vides, ni avant, ni après */
      }
  }

  comments_free ();
}



static SXINT	ETAT;
static SXINT	*stmt_kind;
static SXINT	main_program_nb;
static bool	is_function_stmt, is_iokw_allowed, is_implicit_statement;
static SXINT	program_unit_kind;
static bool     we_are_in_parse_in_la;

static struct sxtoken	*PTOK;
static SXINT		TOK_i;

#define MAIN_PROGRAM		1
#define FUNCTION_SUBPROGRAM	2
#define SUBROUTINE_SUBPROGRAM	3	
#define BLOCK_DATA_SUBPROGRAM	4

static char *executable_program_unit_name [5] = {
  "NO_PROGRAM_UNIT",
  "MAIN_PROGRAM",
  "FUNCTION_SUBPROGRAM",
  "SUBROUTINE_SUBPROGRAM",
  "BLOCK_DATA_SUBPROGRAM"
};

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
#if 0
/* Pour savoir qui est quoi */
struct sxparse_mode {
    SXUINT	        mode; /* SXPARSER, SXRECOGNIZER, SXGENERATOR */
    SXUINT	        kind; /* SXWITH_RECOVERY, SXWITH_CORRECTION, SXWITHOUT_ERROR */
    SXUINT	        local_errors_nb;
    SXUINT	        global_errors_nb;
    SXUINT	        look_back; /* 0 => infinity, k>0 => look_back of k tokens. */
    bool		is_prefixe; /* parse a prefixe */
    bool		is_silent; /* no error message */
    bool		with_semact; /* semantic actions should be performed */
    bool		with_parsact; /* parsing actions should be performed */
    bool		with_parsprdct; /* parsing predicates should be performed */
    bool		with_do_undo; /* do (and undo) parsing actions in look_ahead */
};
#endif

static struct sxparse_mode
f77_parse_mode = {SXPARSER, SXWITH_RECOVERY, 0, 0, 0 /* tous les tokens */, false, false, true, true, true, false};
static struct sxparse_mode
do_parse_mode_noerr = {SXPARSER, SXWITHOUT_ERROR, 0, 0, 0, true, false, false, false, false, false};
static struct sxparse_mode
format_parse_mode_withmess = {SXPARSER, SXWITH_CORRECTION, 0, 0, 0, true, false, false, false, false, false};

/*
  C'est OK si on arrive a corriger. 
  new_mode.kind = SXWITH_CORRECTION;
  Decompte du nombre de corrections. 
  new_mode.error_nb = 0;
  On conserve tous les tokens 
  new_mode.look_back = 0;
  On ne parse qu'un prefixe du source 
  new_mode.is_prefixe = true;
  Pas de message d'erreur 
  new_mode.is_silent = true;
  On inhibe toutes les actions et predicats. 
  new_mode.with_semact = false;
  new_mode.with_parsact = false;
  new_mode.with_parsprdct = false;
*/



#if 0
/* Qui est quoi */
#define SXNORMAL_SCAN	0
#define SXLA_SCAN	1
#define SXRCVR_SCAN	2

struct sxscan_mode {
    SXUINT	        errors_nb;
    SXUINT	        mode /* SXNORMAL_SCAN, SXLA_SCAN, SXRCVR_SCAN */;
    bool		is_silent; /* no error message */
    bool		with_system_act; /* scanning system actions should be performed */
    bool		with_user_act; /* scanning user actions should be performed */
    bool		with_system_prdct; /* scanning system predicates should be performed */
    bool		with_user_prdct; /* scanning user predicates should be performed */
};
#endif

static struct sxscan_mode scan_mode_withmess = {0, SXNORMAL_SCAN, false, true, true, true, true};

#define ts_put(c)							\
(    (sxsvar.sxlv.ts_lgth+2 == sxsvar.sxlv_s.ts_lgth_use)			\
	? sxsvar.sxlv_s.token_string = (char *) sxrealloc (sxsvar.sxlv_s.	\
	    token_string, sxsvar.sxlv_s.ts_lgth_use *= 2, sizeof (char))	\
	: NULL,								\
     sxsvar.sxlv_s.token_string [sxsvar.sxlv.ts_lgth ++] = c		\
)



static void	CLEAR (SXINT *T)
{
    SXINT	x, y;

    for (x = T [y = 0]; x > 0; x = T [y = x])
	T [y] = 0;

    T [y] = 0;
    T [0] = -1;
}



#if EBUG
static void print_tok (struct sxtoken	*pt)
{
  fprintf (stdout,
	   "\ttoken = (%ld, \"%s\")\n\
\t\t\tste = (%ld, \"%s\")\n\
\t\t\tsource_index = (%s, %ld, %ld)\n\
\t\t\tcomment = \"%s\"\n\
",
	   pt->lahead,
	   sxttext (sxsvar.sxtables, pt->lahead),
	   pt->string_table_entry,
	   pt->string_table_entry > 1 ? sxstrget (pt->string_table_entry) : (pt->string_table_entry == 1 ?
									     "SXEMPTY_STE" : "SXERROR_STE"),
	   pt->source_index.file_name,
	   (SXUINT) pt->source_index.line,
	   (SXUINT) pt->source_index.column,
	   (pt->comment != NULL) ? pt->comment : "(NULL)");
}

static void print_toks_buf (SXINT min, SXINT max)
{
    SXINT i;
    
    fprintf (stdout, "\n%s (print_toks_buf [%ld:%ld])\n", ME, min, max);

    for (i = min; i <= max ; i++) {
	fprintf (stdout,
		 "toks_buf [%ld(%ld, %ld)]",
		 (SXINT) i,
		 (SXINT) SXTOKEN_PAGE(i),
		 (SXINT) SXTOKEN_INDX(i));
	print_tok (&(SXGET_TOKEN (i)));
    }
    
    fputs ("\n", stdout);
}

static void print_sxsvar (void)
{
  SXINT i;
      
  fprintf (stdout,
	   "sxsvar.sxlv_s.token_string [%ld] = \"", (SXUINT) sxsvar.sxlv.ts_lgth);
    
  for (i = 0; i < sxsvar.sxlv.ts_lgth ; i++) {
    fprintf (stdout,
	     "%c", 
	     sxsvar.sxlv_s.token_string [i]);
  }
  
  fputs ("\"\n", stdout);

  print_tok (&(sxsvar.sxlv.terminal_token));
      
  fprintf (stdout,
	   "sxsvar.sxlv.previous_char = '%c'_0x%x",
	   sxsvar.sxlv.previous_char,
	   (unsigned short)sxsvar.sxlv.previous_char);
}

static void print_srcmngr_curchar (void)
{
  fprintf (stdout,
	   "\tsxsrcmngr.previous_char = '%c'_0x%x,\n\tsxsrcmngr.current_char = '%c'_0x%x, sxsrcmngr.source_coord = (%s, %ld, %ld)\n",
	   (char)sxsrcmngr.previous_char, (unsigned short)sxsrcmngr.previous_char,
	   (char)sxsrcmngr.current_char, (unsigned short)sxsrcmngr.current_char,
	   sxsrcmngr.source_coord.file_name,
	   (SXUINT) sxsrcmngr.source_coord.line,
	   (SXUINT) sxsrcmngr.source_coord.column);
}
#endif

static void f77put_token (struct sxtoken tok) {
  sxput_token (tok);
	
#if EBUG
  print_toks_buf (sxplocals.Mtok_no, sxplocals.Mtok_no);
#endif
}

static struct source_item store_source_item, store2_source_item;

/* Remplit source_item */
static void get_next_source_item_from_stmts (void)
{
  /* ATTENTION : fill_comments_and_stmts les eol ne sont pas tous des %eol (ne pas en tenir compte
     si la carte suivante est une continuation) */
  
  if (store_source_item.c != SXNEWLINE) {
    /* L'appel précédent a mémorisé le source_item à retourner */
    source_item = store_source_item;
    /* Ce ne sera pas le cas le coup d'après */
    store_source_item.c = SXNEWLINE;
  }
  else {
    for (;;) { /* marche avec les break pour éviter des "return" intermédiaires */
      /* source_item contient les valeurs retournée par l'appel précédent */
      if (source_item.c != SXNEWLINE) {
	/* Le source_item précédent, n'est pas un EOL, on récupère le suivant... */
	while (++source_item.source_index.column > source_item.last_non_blank_col &&
	       source_item.source_index.column < source_item.eol_col);

	if ((source_item.c = stmts_get_a_char (source_item.source_index.column)) != SXNEWLINE)
	  /* ... et on le retourne si ce n'est pas un EOL */
	  break;

	/* Ici source_item.source_index.column == source_item.eol_col et repère donc le %eol */
      }

      /* source_item est un EOL */

      /* On le sauvegarde */
      store_source_item = source_item;
      /* On lit la carte stmt suivante par get_next_source_item_from_stmts qui positionne source_item
	 sur le 1er caractère non blanc de la ligne (marche même sur eof) */
      fill_comments_and_stmts ();
      /* source_index.c != EOL */

      if (!stmts.is_a_continuation_card) {
	/* Le eol de store est un %eol, il faut le retourner ... */
	if (store_source_item.source_index.line == 0) {
	  /* ... sauf si c'est le 1er source_item du source */
	  break;
	}

	/* On se souvient du source_item courant */
	store2_source_item = source_item;
	/* Le eol de store est un %eol */
	source_item = store_source_item;
	store_source_item = store2_source_item; /* On le sortira à l'appel suivant, ce n'est pas un EOL */
      }
      else {
	if (store_source_item.source_index.line == 0) {
	  /* c'est le 1er source_item du source dans une carte continuation, ça commence mal */
	  /* source_item.source_index.column est positionné sur le 1er caractère non nul
	     de la carte continuation. */
	  SXINT col = source_item.source_index.column;
	  source_item.source_index.column = 6;
	  sxerror (source_item.source_index,
		   sxsvar.sxtables->err_titles [1][0] /* Warning */,
		   "%sA program cannot start with a continuation line.",
		   sxsvar.sxtables->err_titles [1]+1 /* Warning */);
	  source_item.source_index.column = col;
	}
      }
      
      break; /* On sort du for(;;) */
    }
  }

  /* on retourne source_item (point de sortie unique) */
#if EBUG
    fprintf (stdout,
	     "%s(get_next_source_item_from_stmts) source_item : (c = '%c'_0x%x, line %ld, column = %ld)\n",
	     ME,
	     (char)source_item.c,
	     (unsigned short)source_item.c,
	     source_item.source_index.line,
	     source_item.source_index.column);
#endif
}


static void put_source_item (void)
{
    if (++source_line->bufused >= source_line->buflength) {
        /* On se laisse une marge de 1 afin de pouvoir toujour ajouter un caractère sans vérification */
        source_line->buffer = (short*) sxrealloc (source_line->buffer, (source_line->buflength *= 2)+1, sizeof (short));
	source_line->bufcoords = (struct sxsource_coord*) sxrealloc (source_line->bufcoords, source_line->buflength+1,
								  sizeof (struct sxsource_coord));
    }

    source_line->buffer [source_line->bufused] = source_item.c;
    source_line->bufcoords [source_line->bufused] = source_item.source_index;
}

static void complete (short c, SXINT col)
{
    /* On ajoute des "car" jusqu'a la colonne "col" */
    /* On ne change pas les source_index. */
    short	prev_c;
    SXINT nb;
    
    if ((nb = col - source_item.source_index.column) >= 0) {
	prev_c = source_item.c;
	source_item.c = c;

	do {
	    put_source_item ();
	} while (--nb >= 0);
	    
	source_item.c = prev_c;
    }
}

#define is_next_to(si1,si2)	((si1.line == si2.line && si1.column + 1 == si2.column) || \
				 (si1.column == 72 && si2.column == 7))


static void fill_buffer (void)
{
    /* fabrique a partir du source une carte etendue (tableau d'items) qui
       a ete debarassee de tous les caracteres inutiles (et seulement ceux la).
       Tient compte des chaines (string ou hollerith). */
    short		c, prev_c;
    struct sxsource_coord	prev_source_index;
    SXINT			hollerith_lgth;

    prev_c = SXNEWLINE;
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
#if EBUG
      if (source_item.c == SXNEWLINE || source_item.c == EOF)
	sxtrap (ME, "fill_buffer");
#endif
      
      prev_c = source_item.c;
      put_source_item ();

    init:
      get_next_source_item_from_stmts ();

    STATE0:
      while (source_item.c == ' ' || source_item.c == '\t') {
	if (source_item.c == '\t')
	  tab_hit (source_item.source_index);

	get_next_source_item_from_stmts ();
      }

      if (source_item.c == '\"')
	/* On entre dans une chaine debutant par une double quote */
	goto STATE2;

      if (source_item.c == '\'')
	/* On entre dans une chaine debutant par une simple quote */
	goto STATE3;

      if (source_item.c == SXNEWLINE || source_item.c == EOF)
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
      for (;;) {
	put_source_item ();
	prev_source_index = source_item.source_index;
	get_next_source_item_from_stmts ();

	if (source_item.c == SXNEWLINE || source_item.c == EOF) {
	  sxerror (source_item.source_index,
		   sxsvar.sxtables->err_titles [2][0] /* error */,
		   "%sIllegal EOL in this character constant\n\
a double quote character is inserted.",
		   sxsvar.sxtables->err_titles [2]+1 /* error */);

	  if (source_item.source_index.column < 73) {
	    complete (' ', 71);
	  }

	  c = source_item.c;
	  source_item.c = '\"';
	  put_source_item ();
	  source_item.c = c;

	  /* Fin de chaîne et fin de ligne */
	  goto STATE6;
	}

#if 0
	/* semble impossible maintenant */
	if (prev_source_index.line < source_item.source_index.line) {
	  /* On a change de ligne dans une chaine, on complete par des blancs. */
	  complete (' ', 72);
	}
#endif

	if (source_item.c == '\"') {
	  /* Fin de la chaine ou deux double quote. */
	  prev_source_index = source_item.source_index;
	  put_source_item ();
	  prev_c = source_item.c;
	  get_next_source_item_from_stmts ();
	
	  while (source_item.c == ' ' || source_item.c == '\t') {
	    if (source_item.c == '\t')
	      tab_hit (source_item.source_index);

	    prev_c = source_item.c;
	    get_next_source_item_from_stmts ();
	  }

	  if (source_item.c == '\"') {
	    /* double double quote... */
	    if (!is_next_to (prev_source_index, source_item.source_index))
	      /* ... sur des lignes différentes */
	      sxerror (source_item.source_index,
		       sxsvar.sxtables->err_titles [2][0] /* error */,
		       "%sThis illegal character constant sequence is ignored.",
		       sxsvar.sxtables->err_titles [2]+1 /* error */);
	    /* On commence donc une nouvelle chaîne */
	    continue; /* ... du for (;;) */
	  }

	  if (source_item.c == SXNEWLINE || source_item.c == EOF)
	    /* Fin de chaîne et fin de ligne */
	    goto STATE6;

	  /* Fin de chaîne, mais la ligne continue */
	  goto STATE1;
	}
	/* On reste dans une chaîne */
      }


    STATE3:
      /* Traitement des chaines debutant par une simple quote */
      for (;;) {
	put_source_item ();
	prev_source_index = source_item.source_index;
	get_next_source_item_from_stmts ();

	if (source_item.c == SXNEWLINE || source_item.c == EOF) {
	  sxerror (source_item.source_index,
		   sxsvar.sxtables->err_titles [2][0] /* error */,
		   "%sIllegal EOL in this character constant\n\
an apostrophe is inserted.",
		   sxsvar.sxtables->err_titles [2]+1 /* error */);

	  if (source_item.source_index.column < 73) {
	    complete (' ', 71);
	  }

	  c = source_item.c;
	  source_item.c = '\'';
	  put_source_item ();
	  source_item.c = c;

	  /* Fin de chaîne et fin de ligne */
	  goto STATE6;
	}

#if 0
	/* semble impossible maintenant */
	if (prev_source_index.line < source_item.source_index.line) {
	  /* On a change de ligne dans une chaine, on complete par des blancs. */
	  complete (' ', 72);
	}
#endif

	if (source_item.c == '\'') {
	  /* Fin de la chaine ou double apostrophe. */
	  prev_source_index = source_item.source_index;
	  put_source_item ();
	  prev_c = source_item.c;
	  get_next_source_item_from_stmts ();
	
	  while (source_item.c == ' ' || source_item.c == '\t') {
	    if (source_item.c == '\t')
	      tab_hit (source_item.source_index);

	    prev_c = source_item.c;
	    get_next_source_item_from_stmts ();
	  }

	  if (source_item.c == '\'') {
	    /* double apostrophe... */
	    if (!is_next_to (prev_source_index, source_item.source_index))
	      /* ... sur des lignes différentes */
	      sxerror (source_item.source_index,
		       sxsvar.sxtables->err_titles [2][0] /* error */,
		       "%sThis illegal character constant sequence is ignored.",
		       sxsvar.sxtables->err_titles [2]+1 /* error */);
	    /* On commence donc une nouvelle chaîne */
	    continue; /* ... du for (;;) */
	  }

	  if (source_item.c == SXNEWLINE || source_item.c == EOF)
	    /* Fin de chaîne et fin de ligne */
	    goto STATE6;

	  /* Fin de chaîne, mais la ligne continue */
	  goto STATE1;
	}
	/* On reste dans une chaîne */
      }


    STATE4:
      /* Debut possible de zone Hollerith */
      hollerith_lgth *= 10;
      hollerith_lgth += source_item.c - '0';
      put_source_item ();
      get_next_source_item_from_stmts ();

      while (source_item.c == ' ' || source_item.c == '\t') {
	if (source_item.c == '\t')
	  tab_hit (source_item.source_index);

	get_next_source_item_from_stmts ();
      }

      if (source_item.c == SXNEWLINE || source_item.c == EOF)
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
       {
	 SXINT  nb;
	 struct source_item prev_source_item
	   , save2_source_item;

	 if (hollerith_lgth == 0 || hollerith_lgth > 255)
	   sxerror (source_item.source_index,
		    sxsvar.sxtables->err_titles [2][0] /* error */,
		    "%sIllegal Hollerith length.",
		    sxsvar.sxtables->err_titles [2]+1 /* error */);

	 /* source_item repère le 'H' */
	 put_source_item ();
	 prev_source_item = source_item;
	 get_next_source_item_from_stmts ();

	 if (source_item.c == SXNEWLINE || source_item.c == EOF) {
	   /* On est dans le cas nH%EOL */
	   if (source_item.source_index.line > prev_source_item.source_index.line) {
	     /* ... mais le %eol est sur la carte suivante, ça marche donc peut être */
	     /* ce caractère n'appartient pas à la constante */
	   }
	   else {
	     sxerror (source_item.source_index,
		      sxsvar.sxtables->err_titles [2][0] /* error */,
		      "%sIllegal empty Hollerith constant.",
		      sxsvar.sxtables->err_titles [2]+1 /* error */);

	     /* Correction de l'erreur, on met 1 blanc */
	     c = source_item.c;
	     source_item.c = ' ';
	     put_source_item ();
	     /* ... et on ne décale pas d'un la colonne du EOL (pour le marquage de l'erreur éventuelle) */
	     source_item.c = c;
	     hollerith_lgth = 0; /* On dit que tout est OK */
	   }
	 }

	 /* Ici, on peut avoir hollerith_lgth == 0 et source_item.c == SXNEWLINE */
	 while (hollerith_lgth > 0) {
	   /* Ici, source_item repère le caractère qui vient d'être lu */
	   if (source_item.source_index.line > prev_source_item.source_index.line
	       /* On a changé de ligne dans 2 chars successifs */
	       || source_item.c == SXNEWLINE || source_item.c == EOF
	       /* cet eol peut être sur la même ligne */
	       ) {
	     /* source_item repère le 1er caractère non blanc (ça peut être EOL), il peut être ou ne pas être dans la portée
		de la constante Hollerith, portée donnée par sa longueur */
	     /* dans tous les cas on reconstitue les blancs supprimés. */
	     /* ... sur la ligne courante */
	     nb = 72-prev_source_item.source_index.column;

	     if (nb > 0) {
	       /* y'a du blanc supprimé en fin de ligne, on le simule */
	       save2_source_item = source_item;
	       source_item.source_index.line = prev_source_item.source_index.line;
	       source_item.source_index.column = prev_source_item.source_index.column;
	       source_item.c = ' ';

	       do {
		 source_item.source_index.column++;
		 put_source_item ();
		 nb--, hollerith_lgth--;
	       } while (nb > 0 && hollerith_lgth > 0);
	      
	       source_item = save2_source_item;
	     }

	     /* Ici, on a simulé tous les blancs possibles en fin de carte.
		nb et hollerith_lgth sont >= 0 */
	     /* On a fini ssi hollerith_lgth == 0 */
	     /* s'il y a plusieurs lignes continuation!! */
	     /* source_item repère le 1er caractère non espace sur la carte physique. */
	     /* On suppose qu'on ne peut pas mettre de commentaires dans les constantes hollerith!! */
	     nb = source_item.source_index.line - prev_source_item.source_index.line;
	     nb = hollerith_lgth / 66; /* 66 est le nombre de blancs d'une ligne vide */

	     if (nb > 1) {
	       nb--;
	       /* On suppose donc qu'il y a nb lignes de 66 blancs entre ces 2 caractères */
	       /* 66 est le nombre de blancs d'une ligne vide */
	       /* On sort donc nb sequences de 66 blancs (si elles sont compatibles avec hollerith_lgth) */

	       save2_source_item = source_item;

	       source_item.source_index.line = prev_source_item.source_index.line+1;
	       source_item.source_index.column = 6;
	       source_item.c = ' ';

	       do {
	         SXINT i;
		 i = 66;

		 while (i-- > 0 && hollerith_lgth > 0) {
		   i--, hollerith_lgth--;
		   source_item.source_index.column++;
		   put_source_item ();
		 };

		 source_item.source_index.line++;
		 source_item.source_index.column = 6;
		 nb--;
	       } while (nb > 0 && hollerith_lgth > 0);
	       
	       source_item = save2_source_item;
	     }

	     nb = source_item.source_index.column-7; /* nombre de blancs commençant la carte */

	     if (nb > hollerith_lgth)
	       /* On ne sort plus que des blancs (le caractère non blanc ne fait pas partie de la constante) */
	       nb = hollerith_lgth;

	     if (nb > 0) {
	       /* y'a du blanc en tête de ligne */
	       save2_source_item = source_item;
	       source_item.source_index.column = 6;
	       source_item.c = ' ';

	       do {
		 source_item.source_index.column++;
		 put_source_item ();
		 nb--, hollerith_lgth--;
	       } while (nb > 0 && hollerith_lgth > 0);
	      
	       source_item = save2_source_item;
	     }
	   }	    

	   if (hollerith_lgth > 0) {
	     prev_source_item = source_item;
	     put_source_item ();
	     hollerith_lgth--;
	     get_next_source_item_from_stmts ();
	   }
	 }
       }

       if (hollerith_lgth != 0) {
	 sxerror (source_item.source_index,
		  sxsvar.sxtables->err_titles [2][0] /* error */,
		  "%sIllegal EOL in this Hollerith constant\n\
which has been truncated.",
		  sxsvar.sxtables->err_titles [2]+1 /* error */);

	 goto STATE6;
       }

      /* A ce niveau, permet-t-on une zone Hollerith derriere une zone hollerith? */
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
    
#if EBUG
  /* Pierre le 01/03/2023 */
    {
      SXINT i, c1;
      struct sxsource_coord	sc;

      for (i = 1; i <= sxsrcmngr.bufused; i++) {
	sc = sxsrcmngr.bufcoords [i];
	c1 = sxsrcmngr.buffer [i];

	fprintf (stdout,
		 "%s(fill_buffer) : sxsrcmngr.buffer [%u, %u] = '%c'_0x%x (at #pos = %u in #buf = %u)\n",
		 ME,
		 (unsigned int)sc.line,
		 (unsigned int)sc.column,
		 (char)c1,
		 (unsigned short)c1,
		 (unsigned int)i,
		 (unsigned int)source_line_head);
      }
    }
#endif
}


static SXINT cmp_si (struct sxsource_coord *si1, struct sxsource_coord *si2)
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


static SXINT get_buffer_nb (struct sxsource_coord *coord)
{
    SXINT		i;
    bool		head_hit;
    struct source_line	*source_line_local;
    /* Recherche dans source_lines l'index du buffer dans lequel se trouve coord. */

    /* On recherche entre tail + 1 et head */

    i = INCR (source_line_tail);
    head_hit = false;

    do {
	source_line_local = &(source_lines [i]);

	if (source_line_local->bufused != 0) {
	    if (cmp_si (source_line_local->bufcoords + 1, coord) <= 0 &&
		cmp_si (coord, source_line_local->bufcoords + source_line_local->bufused) <= 0)
	    return i;
	}

	i = INCR (i);

	if (i == source_line_head)
	    head_hit = true;
    } while (i == source_line_head || !head_hit);

    sxtrap (ME, "get_buffer_nb");
    return 0;
}



static SXINT get_buffer_index (SXINT nb, struct sxsource_coord *coord, SXINT delta)
{
    SXINT			i;
    struct sxsource_coord	*bufcoords = source_lines [nb].bufcoords ;

    i = 1;

    while (cmp_si (bufcoords + i, coord) < 0)
	i++;

    if ((i += delta) <= source_lines [nb].bufused)
	return i;

    sxtrap (ME, "get_buffer_index");
    return 0;
}


static struct sxsource_coord *get_source_index (struct sxsource_coord *coord,
						SXINT delta )
{
    SXINT nb, index_local;

    nb = get_buffer_nb (coord);
    index_local = get_buffer_index (nb, coord, delta);
    return &(source_lines [nb].bufcoords [index_local]);
}




/* CLONE du SOURCE_MANAGER */

#ifndef LOCALBUFLENGTH
#define	LOCALBUFLENGTH	256	/* Smallest buffer allocated */
#endif

static struct sxsource_coord sxsrcmngr_source_coord;
static short                 sxsrcmngr_previous_char;
static short                 sxsrcmngr_current_char;
static SXINT                 sxsrcmngr_bufused;
static SXINT                 sxsrcmngr_bufindex;
/* Attention : on ne store/restore pas buflength car si le buffer de sxsrcmngr a changé (longueur, emplacement, ...)
   on conserve cette nouvelle def */
static void restore_sxsrcmngr (void) {
     sxsrcmngr.source_coord = sxsrcmngr_source_coord;
     sxsrcmngr.previous_char = sxsrcmngr_previous_char;
     sxsrcmngr.current_char = sxsrcmngr_current_char;
     sxsrcmngr.bufused = sxsrcmngr_bufused;
     sxsrcmngr.bufindex = sxsrcmngr_bufindex;
}
static void save_sxsrcmngr (void) {
     sxsrcmngr_source_coord = sxsrcmngr.source_coord;
     sxsrcmngr_previous_char = sxsrcmngr.previous_char;
     sxsrcmngr_current_char = sxsrcmngr.current_char;
     sxsrcmngr_bufused = sxsrcmngr.bufused;
     sxsrcmngr_bufindex = sxsrcmngr.bufindex;
}

/* Acces au caractere suivant */
short	f77_next_char (void)
{
  sxsrcmngr.previous_char = sxsrcmngr.current_char;

  if (sxsrcmngr.bufindex == sxsrcmngr.bufused) {
    fill_buffer ();
    sxsrcmngr.bufindex = 0;
  }

  sxsrcmngr.source_coord = sxsrcmngr.bufcoords [++sxsrcmngr.bufindex];
  
#if EBUG
  /* Pierre le 24/02/2023 */
  fprintf (stdout,
           "%s(f77_next_char) : sxsrcmngr.current_char [%u, %u] = '%c'_0x%x (at #pos = %u in #buf = %u)\n",
	   ME,
	   (unsigned int)sxsrcmngr.source_coord.line,
	   (unsigned int)sxsrcmngr.source_coord.column,
	   (char)sxsrcmngr.buffer [sxsrcmngr.bufindex],
	   (unsigned short)sxsrcmngr.buffer [sxsrcmngr.bufindex],
	   (unsigned int)sxsrcmngr.bufindex,
	   (unsigned int)source_line_head);
#endif
  
  return sxsrcmngr.current_char = sxsrcmngr.buffer [sxsrcmngr.bufindex];
}

/* Acces au caractere en avance suivant */

short	f77_lanext_char (void)
{
  
#if EBUG
  /* Pierre le 24/02/2023 */
  SXINT i = sxsrcmngr.labufindex;
  
  if (i != sxsrcmngr.bufused)
    i++;
  
  fprintf (stdout,
           "f77_lanext_char : sxsrcmngr.current_char [%u, %u] = '%c'_0x%x (at #lapos = %u in #buf = %u)\n",
	   (unsigned int)sxsrcmngr.source_coord.line,
	   (unsigned int)sxsrcmngr.source_coord.column,
	   (char)sxsrcmngr.buffer [i],
	   (unsigned short)sxsrcmngr.buffer [i],
	   (unsigned int)i,
	   (unsigned int)source_line_head);
#endif
  
    if (sxsrcmngr.labufindex == sxsrcmngr.bufused) {
	/* On retourne tout le temps EOL! */
	/* Il ne faut pas lire de nouveau buffer. */
	sxsrcmngr.labufindex--;
    }

  return sxsrcmngr.buffer [++sxsrcmngr.labufindex];
}




/* Acces au premier caractere en avance */

short	f77_lafirst_char (void)
{
    sxsrcmngr.labufindex = sxsrcmngr.bufindex;
    return f77_lanext_char ();
}




/* Recul d'un caractere: le scanner veut inserer un caractere devant */

void	f77_X (short inserted)
/* On est forcement dans le cas ou le buffer est valide, puisqu'une	*/
/* correction d'erreur impose un look-ahead prealable.  Par ailleurs,	*/
/* cette procedure n'est jamais appelee deux fois de suite, puisqu'une	*/
/* correction d'erreur est censee etre correcte...			*/

{
    sxsrcmngr.bufindex--;
    sxsrcmngr.current_char = inserted;
}




/* Retour en arriere dans la lecture en avance */

void	f77_laback (SXINT backward_number)
/* Une erreur a ete detectee et corrigee en look-ahead.  Celui-ci doit	*/
/* reprendre plus haut.  "backward_number" est cense etre valide	*/
/* vis-a-vis de la situation dans le "buffer".				*/

{
    sxsrcmngr.labufindex -= backward_number;
}


/* Non utilise ds FORTRAN */
void	f77_srcpush (void)
{
    sxtrap (ME, "f77_srcpush");
}

/* Au cas ou... */
#if 0
/* Push a string ahead of the current character */

void	f77_srcpush (previous_char, chars, coord)
    short	previous_char;
    char	*chars;
    struct sxsource_coord	coord;

/* On traite une action "@Release".  Le "source manager" est dans un */
/* etat quelconque.  En particulier "buffer" peut exister ou non...  */

{
    SXINT	length = strlen (chars);

    /* Must keep its contents in the right place */
    
    /* First see if it is big enough */
{
    SXINT	minlength;
    
    minlength = (sxsrcmngr.bufused - sxsrcmngr.bufindex + 1) + length;
    
    if (sxsrcmngr.buflength < minlength)
	sxsrcmngr.buffer = (short*) sxrealloc (sxsrcmngr.buffer, sxsrcmngr.buflength = minlength, sizeof (short)), sxsrcmngr.bufcoords = (struct sxsource_coord*) sxrealloc (sxsrcmngr.bufcoords, sxsrcmngr.buflength, sizeof (struct sxsource_coord));
}


/* If there is not room enough, push the contents to the right */

if (sxsrcmngr.bufindex < length) {
    short	*buf1, *buf2;
    struct sxsource_coord	*coords1, *coords2;
    SXINT	count;
    
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
    short	*buf;
    struct sxsource_coord	*coords;
    
    buf = &sxsrcmngr.buffer [sxsrcmngr.bufindex -= length], coords = &sxsrcmngr.bufcoords [sxsrcmngr.bufindex];
    
    while (length-- != 0) {
	*coords++ = coord;
	
	if ((*buf++ = *chars++) != SXNEWLINE)
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
#include <limits.h>

static bool is_ansi_lgth_already_checked;

#define XCHARACTER_kind    1
#define XLENGTH_kind       2
#define XIMPLICIT_kind     3
#define XDO_LOOP_kind      4
#define XDO_WHILE_kind     5
#define X2ops_kind         6
#define X2relop_kind       7
#define X2dblcplx_kind     8
#define X2ioidl_kind       9
#define Xlast_kind         9

static bool extension_already_seen [Xlast_kind+1];

static char *extension_mess [] = 
  {
    "",
    "character ('_' or '$')",
    "scalar type length",
    "IMPLICIT statement",
    "DO-LOOP statement",
    "DO-WHILE statement",
    "operator sequence",
    "relational operator",
    "DOUBLE COMPLEX type",
    "io_imply_do_list",
  };

static SXINT head_tok_no; /* On se souvient du tok_no du 1er token d'une ligne */
static bool  after_logical_if;
static struct {
     SXUINT tok_no, line_no;
} eol_flag;


void	f77_src_mngr (SXINT what, FILE *infile, char *file_name)
{
  SXINT i;

  switch (what) {
  case SXINIT:
    for (i = 0; i < SOURCE_BUF_SIZE; i++) {
      /* On se laisse une marge de 1 afin de pouvoir toujour ajouter un caractère sans vérification */
      source_lines [i].buffer = (short*) sxalloc ((source_lines [i].buflength = LOCALBUFLENGTH)+1, sizeof (short));
      source_lines [i].bufcoords = (struct sxsource_coord*) sxalloc (LOCALBUFLENGTH+1, sizeof (struct sxsource_coord));
      source_lines [i].bufused = 0; /* Les buffers sont vides */
    }

    source_line_head = source_line_tail = SOURCE_BUF_SIZE - 1;

    source_item.c = store_source_item.c = store2_source_item.c = SXNEWLINE;
    source_item.source_index.file_name = file_name;
    source_item.source_index.line = 0;
    source_item.source_index.column = 0;

    sxsrcmngr.infile = infile; /* Pour les getc */

    stmts_alloc ();

    is_tab_already_met = false;
    is_hollerith_already_met = false;
    is_quoted_string_already_met = false;
    is_lower_to_upper_already_met = false;
    is_implicit_extension_already_met = false;

    is_ansi_lgth_already_checked = false;

//    Avec cet essai, au cours d'une analyse par lot (analyse de multiples programmes FORTRAN),
//    sur la détection d'une extension (sans option -X), chaque warning n'est émis, au plus, qu'une seule fois.
//    for (i = Xlast_kind; i > 0; i--)
//      extension_already_seen [i] = false;

    /* Initialisation */
    varstr_catchar (stmts.string, SXNEWLINE); /* On suppose que toute nouvelle carte est précédée d'un EOL */
    COL_MAX = is_input_free_fortran ? UINT_MAX : 72;
 
    if (is_pretty_printer) {
      /* Les noms des mots-clés sortis par le pp sont donnés par sxttext (sxtables, lahead) i.e.
	 sxsvar.SXS_tables.S_adrp [lahead].  Or, si on ne fait rien, on aura donc dans le pp du source des trucs du genre
	 REWINDkw sur les Zombies, on change donc S_adrp!!
      */
      SXINT t, orig_tlgth, total_tlgth = 0;
      char *pc, *total_tstring, *orig_tstring;
      char **f77_adrp;

      /* On fabrique une copie de S_adrp... */
      for (t = 1; t <= sxsvar.SXS_tables.S_termax; t++)
	total_tlgth += strlen (sxsvar.SXS_tables.S_adrp [t]) + 1;

      f77_adrp = (char **) sxalloc (sxsvar.SXS_tables.S_termax+1, sizeof (char*));
      f77_adrp [0] = total_tstring = (char *) sxalloc (total_tlgth, sizeof (char));

      for (t = 1; t <= sxsvar.SXS_tables.S_termax; t++) {
	orig_tstring = sxsvar.SXS_tables.S_adrp [t];
	orig_tlgth = strlen (orig_tstring) + 1;

	f77_adrp [t] = total_tstring;

	while (orig_tlgth-- > 0)
	  *total_tstring++ = *orig_tstring++;
      }	  

      /* ... que l'on modifie pour les mots_clés BLABLAkw ... */
      pc = f77_adrp [REWIND_KW_t]; pc += strlen (pc) - strlen ("kw"); *pc = SXNUL;
      pc = f77_adrp [READ_KW_t]; pc += strlen (pc) - strlen ("kw"); *pc = SXNUL;
      pc = f77_adrp [BACKSPACE_KW_t]; pc += strlen (pc) - strlen ("kw"); *pc = SXNUL;
      pc = f77_adrp [ENDFILE_KW_t]; pc += strlen (pc) - strlen ("kw"); *pc = SXNUL;
      /* ... et que l'on met à la place de S_adrp */
      sxsvar.sxtables->SXS_tables.S_adrp = sxsvar.SXS_tables.S_adrp = f77_adrp;

      comments_alloc ((file_name == NULL) ? 0 : ftell (infile));
    }

    /* Comme les source_lines sont vides (initialisation ci-dessus) cet appel se contente de lier
       sxsrcmngr.buffer et sxsrcmngr.bufcoords à une source_line pour permettre les initialisations
       qui suivent */
    fill_buffer ();

    sxsrcmngr.source_coord.file_name = file_name;
    sxsrcmngr.source_coord.line = sxsrcmngr.source_coord.column = 0;
    sxsrcmngr.previous_char = EOF;
    sxsrcmngr.current_char = SXNEWLINE;
    sxsrcmngr.buffer [0] = sxsrcmngr.current_char;
    sxsrcmngr.bufcoords [0] = sxsrcmngr.source_coord;
    sxsrcmngr.bufindex = sxsrcmngr.labufindex = 0;
    sxsrcmngr.has_buffer = true;

    break;

  case SXFINAL:
    for (i = 0; i < SOURCE_BUF_SIZE; i++) {
      sxfree (source_lines [i].buffer), source_lines [i].buffer = NULL;
      sxfree (source_lines [i].bufcoords) ,source_lines [i].bufcoords = NULL;
    }

    stmts_free ();

    if (is_pretty_printer)
      sxfree (sxsvar.SXS_tables.S_adrp [0]), sxfree (sxsvar.SXS_tables.S_adrp);

    break;

  default:
#if EBUG
    sxtrap(ME, "unknown switch case #1");
#endif
    break;
  }
}



static char *skip_an_int (char *str)
{
    /* saute DIGIT* */
    /* Cette chaine est terminee par SXNUL */
    char c;

    c = *str;
    
    while (isdigit (c)) {
	c = *++str;
    };

    return str;
}


static SXINT get_int_val (char *str)
{
    /* Ce n'est pas atoi() */
    /* retourne -1 si str ne pointe pas sur un chiffre */
    /* sinon calcule la valeur de DIGIT+ */
    /* La séquence DIGIT+ peut être en milieu de chaîne */
    char	c;
    SXINT	val;

    c = *str++;

    if (!isdigit (c))
      val = -1;
    else  {
      val = 0;
      
      do {
	val *=10;
	val += c - '0';
	c = *str++;
      } while (isdigit (c));
    }

    return val;
}




#define tt_push(y,t) (y ? tt_stack_mngr_push(&(t)) : f77put_token(t))
#define tt_stack_mngr_clear() tt_stack_top=0

static void tt_stack_mngr_alloc (SXINT size)
{
    tt_stack = (struct sxtoken*) sxalloc ((tt_stack_size = size) + 1, sizeof (struct sxtoken));
    tt_stack_top = 0;
}

static void tt_stack_mngr_free (void)
{
  sxfree (tt_stack), tt_stack = NULL;
}

static void tt_stack_mngr_push (struct sxtoken *ptt)
{

    if (++tt_stack_top > tt_stack_size)
	tt_stack = (struct sxtoken*) sxrealloc (tt_stack, (tt_stack_size += 10) + 1, sizeof (struct sxtoken));
				
    tt_stack [tt_stack_top] = *ptt;
}


static void do_stack_mngr_alloc (SXINT size)
{
    do_stack = (struct do_item*) sxalloc ((do_stack_size = size) + 1, sizeof (struct do_item));
    do_stack_top = 0;
}


static void do_stack_mngr_free (void)
{
  sxfree (do_stack), do_stack = NULL;
}



static void do_stack_mngr_push (SXINT label, struct sxsource_coord *source_index)
{
    if (label == 0)
        /* On l'empile quand même */
	sxerror (*source_index,
		 sxsvar.sxtables->err_titles [2][0] /* error */,
		 "%sIllegal null label in a do-statement.",
		 sxsvar.sxtables->err_titles [2]+1 /* error */);

    /* ici, si label == -1 on est dans l'extension du <do_statement> "block do loop" */
				
    if (++do_stack_top > do_stack_size)
	do_stack = (struct do_item*) sxrealloc (do_stack, (do_stack_size += 10) + 1, sizeof (struct do_item));
				
    do_stack [do_stack_top].label = label;
    do_stack [do_stack_top].source_index = *source_index;
}

/* recherche "label" dans la do_stack et retourne son index ou 0 */
/* Si label == -1, on fait une recherche haut-bas sinon bas-haut (on veut la labeled_do_loop la + externe définissant
   ce label car un seul label peut fermer plusieurs labeled_do_loop) */
static SXINT do_stack_mngr_search (SXINT label)
{
  SXINT i;

  if (label == -1) {
    for (i = do_stack_top; i > 0; i--)
      if (do_stack [i].label == label)
	break;
  }
  else {
    for (i = 1; i <= do_stack_top; i++)
      if (do_stack [i].label == label)
	break;

    if (i > do_stack_top)
      i = 0;
  }

  return i;
}

/* Si non nul, indx repère dans do_stack un label qui correspond à la fermeture d'une do_loop.
   On ferme non seulement cette do_loop mais également toutes les do_loop internes.
   Deux cas
     - label == -1 : On vient de rencontrer un "END DO" explicite
     - label >(=) 0 : Fin d'une labeled_do_loop
 */
static void do_stack_mngr_process (SXINT indx, struct sxsource_coord *source_index, bool use_tt_stack)
{
  SXINT           i, label, labi;
  struct do_item *do_item_ptr;
  struct sxtoken  tt;

  if (indx > 0) {
    tt.source_index = *source_index;
    tt.string_table_entry = SXEMPTY_STE;
    tt.comment = NULL;
    label = do_stack [indx].label;

    if (indx < do_stack_top) {
      /* cas de la fermeture multiple */
      if (label == -1) {
	/* On génère des {EOL_t ENDDO_t}+ */

	for (i = do_stack_top; i > indx; i--) {
	  /* Ici on n'a que des label > 0 */
	  tt.lahead = ENDDO_t;
	  tt_push (use_tt_stack, tt);
	  tt.lahead = EOL_t;
	  tt_push (use_tt_stack, tt);
	}
      }
      else {
	for (i = do_stack_top; i > indx; i--) {
	  do_item_ptr = &(do_stack [i]);
	  labi = do_item_ptr->label;

	  if (labi == -1) {
	    /* Fermeture erronée d'un block_do_loop par un label */
	    /* On proteste et on génère "END_t DO_t EOL_t" */
	    sxerror (do_item_ptr->source_index,
		     sxsvar.sxtables->err_titles [2][0] /* ERROR */,
		     "%sThis block do loop statement has been gracefully closed while\n\
processing the enclosing do_loop labeled %i and terminated at the end of line %i.",
		     sxsvar.sxtables->err_titles [2]+1 /* ERROR */,
		     label,
		     tt.source_index.line);
	    tt.lahead = END_t;
	    tt_push (use_tt_stack, tt);
	    tt.lahead = DO_t;
	    tt_push (use_tt_stack, tt);
	    tt.lahead = EOL_t;
	    tt_push (use_tt_stack, tt);
	  }
	  else {
	    if (labi != label) {
	      /* On a un labeled_do_loop avec un autre label, on proteste mais on le ferme quand même */
	      sxerror (do_item_ptr->source_index,
		       sxsvar.sxtables->err_titles [2][0] /* ERROR */,
		       "%sThis do-statement has been gracefully closed while\n\
processing the enclosing do_loop labeled %i and terminated at the end of line %i.",
		       sxsvar.sxtables->err_titles [2]+1 /* ERROR */,
		       label,
		       tt.source_index.line);
	    }
	    /* else
	       labeled_do_loop avec le même label, cas valide, on le ferme */
	    tt.lahead = ENDDO_t;
	    tt_push (use_tt_stack, tt);
	    tt.lahead = EOL_t;
	    tt_push (use_tt_stack, tt);
	  }
	}
      }
    }

    /* On n'a plus qu'à générer le ENDDO de label */
    if (label == -1) {
      /* On génère des END_t DO_t */
      tt.lahead = END_t;
      tt_push (use_tt_stack, tt);
      tt.lahead = DO_t;
      tt_push (use_tt_stack, tt);
      tt.lahead = EOL_t;
      tt_push (use_tt_stack, tt);
    }
    else {
      /* On génère les derniers EOL_t ENDDO_t */
      tt.lahead = ENDDO_t;
      tt_push (use_tt_stack, tt);
      tt.lahead = EOL_t;
      tt_push (use_tt_stack, tt);
    }

    /* On dépile la séquence */
    do_stack_top = indx-1;
  }
}
/* ***************************************************************************** */
/* Pour la version memoisée de check_balanced () */
/* pour chaque '(' d'une carte logique, on note si ce token à une ')' et
   si oui, son tok_no */

#include "X.h"

static struct {
     X_header X_hd; /* tok_no --> pos */
     SXINT    *toks;
} memo_cb; /* memo_check_balanced */


static void memo_cb_oflw (SXINT old_size, SXINT new_size) {
  sxuse (old_size);
  memo_cb.toks = (SXINT*) sxrealloc (memo_cb.toks, new_size + 1, sizeof (SXINT));
}

static void memo_cb_mngr (SXINT what) {
     switch (what) {
     case SXOPEN:
	  /* Alloc */
	  X_alloc (&memo_cb.X_hd, "memo_cb.X_hd", 23 /* why not? */, 1, memo_cb_oflw, (FILE *)NULL);
	  memo_cb.toks = (SXINT*) sxalloc (X_size (memo_cb.X_hd) + 1, sizeof (SXINT));
	  break;
	  
     case SXCLOSE:
	  /* Free */
	  sxfree (memo_cb.toks), memo_cb.toks = NULL;
	  X_free (&memo_cb.X_hd);
	  break;

     case SXINIT:
	  /* RAZ */
	  X_clear (&memo_cb.X_hd);
	  break;

     default:
#if EBUG
	  sxtrap(ME, "memo_cb_mngr : unknown switch case #1");
#endif
	  break;
     }
}


static bool check_balanced (SXINT *tok_no)
{
    /* tok_no designe un token (ident ou mot_cle) ou une "*".
       Cette fonction verifie si la sequence de tokens tok_no+1 ...tok_no+n est
       une structure bien parenthesee dans une carte logique.
       Dans tous les cas retourne dans tok_no le numero du dernier token examine.
       C'est la derniere parenthese fermante s'il y a eu succes.
       Sur échec c'est soit le token qui suit id si ce n'est pas une "(" soit l'%eol de fin de carte.
       Cette fonction est mémoisée */
    SXINT     count, lahead, pos;
    bool      save_is_implicit_statement;

    count = (sxget_token (++*tok_no)->lahead == LEFTP_t) ? 1 : 0;

    if (count == 0)
      /* pas de "(" après l'id */
      return false;

    if (X_set (&memo_cb.X_hd, *tok_no, &pos)) {
	 /* Cette '(' a été memoisée */
	 SXINT right_tok_no = memo_cb.toks [pos];

	 if (right_tok_no < 0)
	      return false;

	 *tok_no = right_tok_no;
	 return true;
    }

    save_is_implicit_statement = is_implicit_statement;
    is_implicit_statement = false; /* Les id entre les parenthèses doivent être traitées "normalement" */

    while (count > 0) {
      /* Excès de "(" */
	lahead = sxget_token (++*tok_no)->lahead;

	if (lahead == RIGHTP_t)
	    count--;
	else if (lahead == LEFTP_t)
	    count++;
	else if (lahead == EOL_t)
	  break;
    }

    is_implicit_statement = save_is_implicit_statement;

    memo_cb.toks [pos] = (count == 0) ? *tok_no /* balanced */ : -1 /* not balanced */;
	 
    return count == 0;
}

/* On a binf_tok_no < bsup_tok_no, binf_tok_no repère une "(" et bsup_tok_no une ")", on vérifie que ces parenthèses correspondent */
static bool is_well_balanced (SXINT binf_tok_no, SXINT bsup_tok_no) {
     SXINT pos;

     return X_set (&memo_cb.X_hd, binf_tok_no, &pos) ? memo_cb.toks [pos] == bsup_tok_no : false /* Normalement jamais atteint!! */;
}

/* ***************************************************************************** */

#if 0

/* On a binf_tok_no < bsup_tok_no, binf_tok_no repère une "(" et bsup_tok_no une ")", on vérifie que les tokens internes, déjà lus,
   sont bien balancés */
static bool is_well_balanced (SXINT binf_tok_no, SXINT bsup_tok_no) {
     SXINT tok_no, lahead, count = 0;

     for (tok_no = binf_tok_no+1; tok_no < bsup_tok_no; tok_no++) {
	  lahead = SXGET_TOKEN (tok_no).lahead;

	  if (lahead == LEFTP_t)
	       count++;
	  else if (lahead == RIGHTP_t && --count < 0)
	       break;
     }

     return count == 0;
}

static bool check_balanced (SXINT *tok_no)
{
    /* tok_no designe un token (ident ou mot_cle) ou une "*".
       Cette fonction verifie si la sequence de tokens tok_no+1 ...tok_no+n est
       une structure bien parenthesee dans une carte logique.
       Dans tous les cas retourne dans tok_no le numero du dernier token examine.
       C'est la derniere parenthese fermante s'il y a eu succes.
       Sur échec c'est soit le token qui suit id si ce n'est pas une "(" ou un %eol.
    */
    SXINT     count, lahead;
    bool save_is_implicit_statement;

    count = (sxget_token (++*tok_no)->lahead == LEFTP_t) ? 1 : 0;

    if (count == 0)
      /* pas de "(" après l'id */
      return false;

    save_is_implicit_statement = is_implicit_statement;
    is_implicit_statement = false; /* Les id entre les parenthèses doivent être traitées "normalement" */

    while (count > 0) {
      /* Excès de "(" */
	lahead = sxget_token (++*tok_no)->lahead;

	if (lahead == RIGHTP_t)
	    count--;
	else if (lahead == LEFTP_t)
	    count++;
	else if (lahead == EOL_t)
	  break;
    }

    is_implicit_statement = save_is_implicit_statement;

    return count == 0;
}
#endif


static SXINT seek_kw (char *string)
{
    /* string est le texte d'un ident en debut d'instruction. On regarde si un element de kw_prefixes
       est un prefixe de string. */
    SXINT i = UNKNOWN_p;
    char c;

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

    case 'D': /* DO ou DOUBLEPRECISION ou DIMENSION ou DATA ou DOUBLECOMPLEX ([[EXTENSION]]) */
      /* Attention, avec l'extension du <do_statement>, on peut ne plus avoir de label et l'entête peut être
	 DO  <99:symbolic_name>
	 donc un "DOi" de longueur 3 */
	if (strlen (string) >= 3) {
	  if (string [1] == 'O') {
	    /* DO_p ou DOUBLEPRECISION_p ou DOUBLECOMPLEX_p ? */
	    /* PB : "DO UBLEPRECISION... = 1" ou "DO UBLECOMPLEX... = 1" */
	    /* dans ces 3 cas on rend DO, si ce n'est pas un <do_statement>, on testera le cas DOUBLEPRECISION
	       ou DOUBLECOMPLEX + tard */
	      return DO_p;
	  }

	  i = (string [2] == 'M' ? DIMENSION_p : DATA_p);
	}

	break;

    case 'E': /* ENDFILE ou ENTRY ou EXTERNAL ou ELSEIF ou ENDIF ou
		 [[EXTENSION]] ENDDO ou
		 [[ESOPE]] ENDSEGMENT */
	if (strlen (string) > 4) {
	    c = string [3];
	    i = c == 'F' ? ENDFILE_p :
		(c == 'R' ? ENTRY_p :
		 (c == 'I' ? ENDIF_p :
		  (c == 'D' ? ENDDO_p :
#ifdef ESOPE
		   (c == 'S' ? ENDSEGMENT_p :
#endif // [[ESOPE]]
		    (string [1] == 'X' ? EXTERNAL_p : ELSEIF_p)
#ifdef ESOPE
		   )
#endif // [[ESOPE]]
		  )
		 )
		);
	}

	break;

    case 'F': /* FUNCTION */
	i = FUNCTION_p;
	break;

    case 'G': /* GOTO */
	i = GOTO_p;
	break;

    case 'I': /* INTEGER ou IMPLICIT ou INTRINSIC ou
		 [[ESOPE]] IMPLIED */
	if (strlen (string) > 6) {
	    c = string [6];
	    i = c == 'R' ? INTEGER_p : (c == 'I' ? IMPLICIT_p : (c == 'S' ? INTRINSIC_p :
#ifdef ESOPE
								 IMPLIED_p
#else // [[ESOPE]]
								 UNKNOWN_p
#endif // [[ESOPE]]
					     ));
	}

	break;

    case 'L': /* LOGICAL */
	i = LOGICAL_p;
	break;

    case 'N': /* NONE */
        i = NONE_p;
        break;

    case 'P': /* PRINT ou PAUSE ou PROGRAM ou
		 [[ESOPE]] POINTEUR */
	if (strlen (string) > 4) {
	    c = string [1];
	    i = c == 'R' ? (string [2] == 'I' ? PRINT_p : PROGRAM_p) : (c == 'A' ? PAUSE_p :
#ifdef ESOPE
									POINTEUR_p
#else // [[ESOPE]]
									UNKNOWN_p
#endif // [[ESOPE]]
		 );
	}

	break;

    case 'R': /* READ ou REWIND ou RETURN ou REAL */
	if (strlen (string) > 3) {
	    c = string [3];
	    i = c == 'D' ? READ_p : (c == 'I' ? REWIND_p : (c == 'U' ? RETURN_p : REAL_p));
	}

	break;

    case 'S': /* STOP ou SAVE ou SUBROUTINE ou
		  [[ESOPE]] SEGINA ou SEGIND ou SEGMENT ou SEGINI ou SEGACT ou SEGDES ou SEGSUP ou SEGIADJ ou SEGPRT */
	if (strlen (string) > 3) {
	    c = string [1];
	    i = c == 'T' ? STOP_p
		         : (c == 'A' ? SAVE_p
			             : (c == 'U' ? SUBROUTINE_p
#ifdef ESOPE
					         : ((strlen (string) > 5) ? (c = string [5], c == 'A' ? SEGINA_p
								                           : c == 'D' ? SEGIND_p
									     : c == 'N' ? SEGMENT_p
							                     : c == 'I' ? SEGINI_p
							                     : c == 'S' ? SEGDES_p
							                     : c == 'P' ? SEGSUP_p
							                     : c == 'J' ? SEGADJ_p
							                     : c == 'T' ? (c = string [4], c == 'C' ? SEGACT_p : SEGPRT_p)
							                     : UNKNOWN_p)
					                                  : UNKNOWN_p)
#else // [[ESOPE]]
					         : UNKNOWN_p
#endif // [[ESOPE]]
					  ));
	}

	break;

    case 'U': /* UNDEFINED */
      i = UNDEFINED_p;

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
	    
static bool check_for_a_lhs (SXINT tok_no)
{
    /* tok_no designe le token (ident ou mot_cle) qui se trouve en debut d'une
       instruction.  On regarde si ce token est l'ident d'une partie gauche
       d'affectation.
       On en profite pour regarder si tok_no est un iokw.
       On a tok_no == sxplocals.Mtok_no
    */
    SXINT lahead;
    bool  ret_val;
#if EBUG
    SXINT ini_tok_no = tok_no;
#endif

#if EBUG
    fprintf (stdout,
	     "########################## Entering check_for_a_lhs (tok_no = %ld) ##########################",
	     (SXINT) tok_no);
    print_toks_buf (tok_no, tok_no);
#endif

    lahead = SXGET_TOKEN (tok_no).lahead;

    if (sxget_token (tok_no + 1)->lahead == LEFTP_t) {
	 memo_cb_mngr (SXINIT); /* RAZ */

	 switch (lahead) {
	 case BACKSPACE_t:
	 case CLOSE_t:
	 case ENDFILE_t:
	 case INQUIRE_t:
	 case OPEN_t:
	 case READ_t:
	 case REWIND_t:
	 case WRITE_t:
	      is_iokw_allowed = true;
	      break;

	 default:
	      is_iokw_allowed = false;
	      break;

	 }
    }

    for (;;) /* Pour avoir un point de sortie unique de la fonction et traiter les "pointeurs" de ESOPE */
    {
	 /* tok_no/lahead repèrent un id */
	 lahead = sxget_token (tok_no+1)->lahead;
	 /* lahead repère ce qui suit un id */

	 if (lahead == LEFTP_t) {
	      if (!check_balanced (&tok_no)) {
		   is_iokw_allowed = false;
		   ret_val = false;
		   break;
	      }

	      /* Ici "id (...)" et tok_no repère la ")" */
	      lahead = sxget_token (tok_no + 1)->lahead;

	      if (lahead == EGAL_t) {
		   /* "id(...)=" : array element name */
		   ret_val = true;
		   break;
	      }

	      if (lahead == EOL_t || lahead == sxeof_code (sxsvar.sxtables) /* EOF_t */) {
		   /* "id(...) %EOL" (id peut être un iokw) */
		   ret_val = false; /* dans tous les cas */
		   break;
	      }

	      is_iokw_allowed = false;

	      if (lahead == LEFTP_t && !check_balanced (&tok_no)) {
		   ret_val = false;
		   break;
	      }

	      /* Ici "id (...)(...)" : substring name et tok_no repère la ")" de droite */
	      lahead = sxget_token (tok_no+1)->lahead;
	 }

	 tok_no++; /* lahead/tok_no repère ce qui suit un [array_/substring_]name */

	 if (lahead == EGAL_t) {
	      /* "id ... =" : lhs */
	      ret_val = true;
	      break;
	 }

#ifdef ESOPE
	 if (lahead != DOT_t) {
	      ret_val = false;
	      break;
	 }

	 /* Ici on est dans un pointeur ESOPE ... */
	 lahead = sxget_token (++tok_no)->lahead;
	 /* ... derrière un '.', donc lahead et tok_no doivent repérer un id */

	 if (lahead != ID_t) {
	      ret_val = false; /* On laisse le traitement des erreurs se débrouiller avec ça ... */
	      break;
	 }

	 /* ... on boucle */
#else // [[ESOPE]]
	 ret_val = false;
	 break;
#endif // [[ESOPE]]
    }

#if EBUG
    fprintf (stdout,
	     "########################## Leaving check_for_a_lhs (init_tok_no = %ld, last_tok_no = %ld) %s ##########################",
	     (SXINT) ini_tok_no,
	     (SXINT) sxplocals.Mtok_no,
	     ret_val ? "true" : "false");
    print_toks_buf (ini_tok_no, sxplocals.Mtok_no);
#endif

    return ret_val;
}

/* On est sur un id au milieu d'une ligne, on regarde si id a la forme FUNCTIONid( et est précédé d'un <10a:type> */
/* str en est le texte
   Mtok_no est le numero du token précédent l'id (qui est lui dans sxsvar.sxlv.terminal_token
   head_tok_no est le numero du 1er token de la ligne */

static bool check_inside_FUNCTION (SXINT Mtok_no, char *str, SXINT str_lgth)
{
  if (str_lgth < 9 /*  |FUNCTION| + 1 (taille min de l'id) */
      || seek_kw (str) != FUNCTION_p)
    /* str ne commence pas par FUNCTIONc... */
    return false;

  /* On regarde le préfixe.
     Il semble qu'on ne puisse venir ici que si le préfixe est un <10a:type> de la forme CHARACTER ... */
  /* Les extensions du type INTEGER*8 FUNCTION myfunct( ont été traitées par "intid" sur le source "8FUNCTIONmyfunct" */
  if (SXGET_TOKEN (head_tok_no).lahead != CHARACTER_t || SXGET_TOKEN (head_tok_no+1).lahead != STAR_t)
    return false;

  /* Là on suppose que les cas CHARACTER*int[id] ont été traités.
     On regarde uniquement CHARACTER*(...), sans rentrer dans les parenthèses, à voir */
  if (SXGET_TOKEN (head_tok_no+2).lahead != LEFTP_t || SXGET_TOKEN (Mtok_no).lahead != RIGHTP_t)
    return false;
  
  if (sxsrcmngr.current_char != '(')
   return false;

  /* C'est bien FUNCTION ident */
  sxsvar.sxlv.terminal_token.lahead = FUNCTION_t;
  sxsvar.sxlv.terminal_token.string_table_entry = SXEMPTY_STE;
  f77put_token (sxsvar.sxlv.terminal_token);

  /* c'est le scanner qui rangera ce token */
  sxsvar.sxlv.terminal_token.lahead = ID_t;
  sxsvar.sxlv.terminal_token.string_table_entry = sxstrsave (str + 8);
  sxsvar.sxlv.terminal_token.source_index.column += 8;

  return true;
}

static void extension_hit (SXINT Xstmt_kind, struct sxsource_coord source_index)
{
  if (!is_extension && !extension_already_seen [Xstmt_kind]) {
    extension_already_seen [Xstmt_kind] = true;
    sxerror (source_index,
	     sxsvar.sxtables->err_titles [1][0] /* warning */,
	     "%s Not a valid f77 %s,\n\
For extended FORTRAN, use the \"-X\" option.\n\
Subsequent non-f77 %ss will not be reported.",
	     sxsvar.sxtables->err_titles [1]+1 /* warning */,
	     extension_mess [Xstmt_kind],
	     extension_mess [Xstmt_kind]);
    
  }
}

static void check_id (struct sxtoken *tt)
{
  char  c, *str;

  if (!is_extension && !extension_already_seen [XCHARACTER_kind]) {
    str = sxstrget (tt->string_table_entry);

    while ((c = *str++) != SXNUL) {
      if (!isalnum (c))
	break;
    }

    if (c != SXNUL) {
      extension_already_seen [XCHARACTER_kind] = true;
      extension_hit (XCHARACTER_kind, tt->source_index);
    }
  }

  if (is_ansi && !is_ansi_lgth_already_checked) {
    if (sxstrlen (tt->string_table_entry) > 6) {
      is_ansi_lgth_already_checked = true;
      sxerror (tt->source_index,
	       sxsvar.sxtables->err_titles [1][0] /* warning */,
	       "%sIn ansi f77 standard, a symbolic name is a sequence of one to\n\
six letters or digits, the first of which must be a letter. This name exceeds this limit.\n\
Subsequent non-f77 symbolic names will not be reported.",
	       sxsvar.sxtables->err_titles [1]+1 /* warning */);
    }
  }
}

static bool isXalpha (char c)
{
  /* En f77 étendu, un id peut commencer par un '_' */
  return isalpha (c) || c == '_';
}

/* On est sur un source de la forme INTEGER*8
   type est INTEGER (ou un autre type) et int_ptok repère un token de lahead INTEGER_t qui contient 8
   On vérifie que 8 est une bonne taille pour la représentation du type INTEGER.
   De plus c'est du f77 etendu => message d'erreur eventuel */
static void check_scalar_type_repr_lgth (SXINT type, struct sxtoken *int_ptok) {
  static char *default_type_lgth [5] = {
    "16" /* COMPLEX */,
    "8" /* INTEGER */,
    "1" /* LOGICAL */,
    "8" /* REAL */,
    "16" /* (DOUBLE) PRECISION */
  };

  char *str;
  bool is_ok;

  str = sxstrget (int_ptok->string_table_entry);

  switch (type) {
  case COMPLEX_t :
    type = 0;
    is_ok = (strcmp (str, "8") == 0) ||
      (strcmp (str, "16") == 0) ||
      (strcmp (str, "32") == 0);
		
    break;
		
  case INTEGER_t :
    type = 1;
    is_ok = (strcmp (str, "2") == 0) ||
      (strcmp (str, "4") == 0) ||
      (strcmp (str, "8") == 0);
		
    break;
		
  case LOGICAL_t :
    type = 2;
    is_ok = (strcmp (str, "1") == 0) ||
      (strcmp (str, "2") == 0) ||
      (strcmp (str, "4") == 0) ||
      (strcmp (str, "8") == 0);
		
    break;
		
  case REAL_t :
    type = 3;
    is_ok = (strcmp (str, "4") == 0) ||
      (strcmp (str, "8") == 0) ||
      (strcmp (str, "16") == 0);
		
    break;
		
  case PRECISION_t : /* On est en fait sur DOUBLE PRECISION, on lui permet d'avoir une specif de longueur pour
			un rattrapage d'erreur + spécifique!! */
    type = 4;
    is_ok = (strcmp (str, "16") == 0);
		
    break;

  default :
    is_ok = true;
    break;
  }

  if (!is_ok) {
    sxerror (int_ptok->source_index,
	     sxsvar.sxtables->err_titles [2][0] /* error */,
	     "%sIllegal length specification for an extended scalar type, \"%s\" is assumed.",
	     sxsvar.sxtables->err_titles [2]+1 /* error */,
	     default_type_lgth [type]);
    int_ptok->string_table_entry = sxstrsave (default_type_lgth [type]);
  }
	  
  if (type != CHARACTER_t)
    extension_hit (XLENGTH_kind, int_ptok->source_index);
}

static bool check_FUNCTION (char *str, SXINT strlgth, SXINT l, SXINT Mtok_no)
{
    /* On est sur un type en debut d'instruction,
       on regarde si ce token est suivi par FUNCTION id. */
    struct sxtoken	tt [2];
    SXINT	l1 = l+8;

    if (strlgth > l1 && strncmp (str + l, "FUNCTION", 8) == 0 && isXalpha (str [l1])) {
	PTOK = &(SXGET_TOKEN (Mtok_no));
	PTOK->string_table_entry = SXEMPTY_STE;
	tt [0].lahead = FUNCTION_t;
	tt [0].string_table_entry = SXEMPTY_STE;
	tt [0].source_index =
	    *get_source_index (&(PTOK->source_index), l);
	tt [0].comment = NULL;
	tt [1].lahead = ID_t;
	tt [1].string_table_entry = sxstrsave (str + l1);
	tt [1].source_index =
	    *get_source_index (&(PTOK->source_index), l1);
	tt [1].comment = NULL;

	check_id (&(tt [1]));

	sxtknmdf (&(tt [0]), 2, Mtok_no + 1, 0);
	return true;
    }

    return false;
}


/* On est en debut d'instruction
   sxsvar.sxlv.terminal_token.lahead est un ID_t ou un kw
   retourne vrai ssi la ligne courante est le debut d'un <21:implicit_statement> donc de la forme
   IMPLICIT (NONE EOL | xtype ('*' | suffixe) ...)
   avec
       xtype = CHARACTER | COMPLEX | DOUBLE PRECISION | INTEGER | LOGICAL | REAL | UNDEFINED | DOUBLE COMPLEX
       suffixe = '(' elem {',' elem} ')'
       elem = (LETTER | '_') ['-'(LETTER | '_')]
   
   Le corps de cette fonction est donc le DFSA
   
               --<','------------------------
               |        |                   |
   (i)--'('>--(1)--l>--(2)--'-'>--(3)--l>--(4)--')'>--(f)
                        |                              |
			-----------------')'>-----------

   qui est code directement en C

   Dans tous les cas, au retour, toutes les variables sont "remises en état"
 */

static bool check_IMPLICIT_stmt ()
{
  short                 save_previous_char, save_current_char;
  SXINT                 save_bufindex, kw;
  struct sxsource_coord	save_source_coord;
  char                  c, *str;
  bool                  result, extension_met;

  if (sxsvar.sxlv.terminal_token.lahead != ID_t) return false;
  if (sxsvar.sxlv.ts_lgth < (SXINT)strlen ("IMPLICITNONE" /* Le + petit */)) return false;

  str = sxsvar.sxlv_s.token_string;

  if (seek_kw (str) != IMPLICIT_p) return false;

  str += strlen ("IMPLICIT");

  kw = seek_kw (str);

  if (kw == DO_p) {
       /* On regarde si ce n'est pas en fait DOUBLE PRECISION... */
       if (strncmp (str, "DOUBLEPRECISION", strlen ("DOUBLEPRECISION")) == 0) {
	    /* En fait on est sur "IMPLICIT DOUBLE PRECISION" */
	    kw = DOUBLEPRECISION_p;
       }
       /* ...ou DOUBLE COMPLEX */
       else if (strncmp (str, "DOUBLECOMPLEX", strlen ("DOUBLECOMPLEX")) == 0) {
	    /* En fait on est sur "IMPLICIT DOUBLE COMPLEX" */
	    kw = DOUBLECOMPLEX_p;
       }
       
  }

  if (str [kw_lgth [kw]] != SXNUL)
      /* L'ID_t str etait de la forme "IMPLICITINTEGERblabla" => id, donc on ne fait rien!! */
    return false;

  c = (char)sxsrcmngr.current_char;

  if ((c != SXNEWLINE) && (c != '*') && (c != '(')) return false;

  switch (kw) {
  case NONE_p:
    if (c != SXNEWLINE) return false;
    extension_met = true;
    break;
    
  case CHARACTER_p:
    if (c == SXNEWLINE) return false;
    extension_met = false;
    break;
    
  case COMPLEX_p:
  case DOUBLECOMPLEX_p:
  case DOUBLEPRECISION_p:
  case INTEGER_p:
  case LOGICAL_p:
  case REAL_p:
    if (c == '*' && !isdigit ((char)sxsrcmngr.buffer [sxsrcmngr.bufindex+1])) return false; /* On attendait une spécif de longueur */
    extension_met = false;
    break;
    
  case UNDEFINED_p:
    if (c == SXNEWLINE || c == '*') return false;
    extension_met = true;
    break;

  default:
    return false;
  }

  if (c == '(') {
    /* On est dans le cas "suffixe", il en faut plus pour valider un implicit statement */
    /* On se souvient de l'état du source_mngr */
    /* Normalement, on ne change pas de buffer!! */
    save_previous_char = sxsrcmngr.previous_char ;
    save_current_char = sxsrcmngr.current_char;
    save_bufindex = sxsrcmngr.bufindex;
    save_source_coord = sxsrcmngr.source_coord;

    result = false;

    /* Codage du DFSA */
    for (;;) { /* Afin de pouvoir utiliser "break" */
      /* statei: */
      if (c != '(') break;
    state1:
      c = f77_next_char ();
      if (!isXalpha (c)) break;
      /* state2: */
      c = f77_next_char ();
      if (c == ')') goto statef;
      if (c == ',') goto statef;
      if (c != '-') break;
      /* state3: */
      c = f77_next_char ();
      if (!isXalpha (c)) break;
      /* state4: */
      c = f77_next_char ();
      if (c == ',') goto state1;
      if (c != ')') break;
    statef:
      result = true;
      break;
    }

    /* On remet en état... */
    sxsrcmngr.previous_char = save_previous_char;
    sxsrcmngr.current_char = save_current_char;
    sxsrcmngr.bufindex = save_bufindex;
    sxsrcmngr.source_coord = save_source_coord;
  }
  else
    /* cas IMPLICITINTEGER*digit, on valide */
    result = true;

  if (!result) return false; /* On considère que ce n'est pas un IMPLICIT stmt */

  /* Ici, on considère qu'on a un IMPLICIT stmt */

  if (extension_met)
    extension_hit (XIMPLICIT_kind, sxsvar.sxlv.terminal_token.source_index);

  /* On transforme l'id en IMPLICIT kw */
  sxsvar.sxlv.terminal_token.lahead = IMPLICIT_t;
  sxsvar.sxlv.terminal_token.string_table_entry = SXEMPTY_STE;
  f77put_token (sxsvar.sxlv.terminal_token);

  if (kw == DOUBLEPRECISION_p || kw == DOUBLECOMPLEX_p) {
    sxsvar.sxlv.terminal_token.lahead = DOUBLE_t;
    sxsvar.sxlv.terminal_token.source_index =
      *get_source_index (&(sxsvar.sxlv.terminal_token.source_index), strlen ("IMPLICIT"));
    sxsvar.sxlv.terminal_token.comment = NULL;
    f77put_token (sxsvar.sxlv.terminal_token);
    sxsvar.sxlv.terminal_token.lahead = kw == DOUBLEPRECISION_p ? PRECISION_t : COMPLEX_t;
  }
  else
    sxsvar.sxlv.terminal_token.lahead = kw_codes [kw];

  sxsvar.sxlv.terminal_token.source_index =
    *get_source_index (&(sxsvar.sxlv.terminal_token.source_index), strlen ("IMPLICIT"));
  sxsvar.sxlv.terminal_token.comment = NULL;

  return true;
}

#ifdef ESOPE
static SXINT SEGINA_SEGIND_tok_no;
#endif // [[ESOPE]]

static void process_initial_id (SXINT Mtok_no, bool is_a_lhs)
{
    /* L'ident situe en Mtok_no est en debut d'instruction.
       On sait que ce n'est pas le debut d'un IMPLICIT statement
       C'est le token ID_t (ce n'est donc pas un kw).
       On sait que ce n'est pas (le debut d') une lhs d'affectation, sauf si cet ID_t commence par les lettres 'D' puis 'O'.
       On sait que sxplocals.Mtok_no > Mtok_no car on a déjà vérifié des tokens du look_ahead par l'appel à check_for_a_lhs().
       Cet appel a au minimum construit le token suivant et au maximum construit le token %EOL marquant la fin de la carte logique.
       On verifie s'il ne s'agit pas de la concatenation d'un mot_cle avec un suffixe adequat. */
    /* Si ce n'est pas le cas, on ne change rien et on laisse le rattrapage d'erreur agir. */
    char	*str, *str1;
    SXINT	lahead;
    SXINT	tok_no, strlgth, kw, l, kw_nb;
    struct sxtoken	tt [3];
    bool        insert_a_token = false, extract_int_id = false, check_double_precision = false, check_double_complex = false;

#if EBUG
    SXINT	left_tok_no;

    left_tok_no = Mtok_no;

    fprintf (stdout,
	     "**************** Entering process_initial_id (after_logical_if = %s, is_a_lhs = %s) ****************\n",
	     after_logical_if ? "TRUE" : "FALSE",
    	     is_a_lhs ? "TRUE" : "FALSE");
    print_toks_buf (left_tok_no, sxplocals.Mtok_no);
#endif

    PTOK = &(SXGET_TOKEN (Mtok_no));
    str = sxstrget (PTOK->string_table_entry);
    strlgth = sxstrlen (PTOK->string_table_entry);
    kw = seek_kw (str);

    switch (kw) {
    case ASSIGN_p :
	tok_no = Mtok_no;
	str += 6;
	
	if (((str1 = skip_an_int (str)) > str) && strlen (str1) >= 3) {
	    /* On a reconnu ASSIGN int */
	    /* On regarde si on est dans le cas ASSIGN label TO variable_name %eol */
	    if ((str1 [0] == 'T') && (str1 [1] == 'O') && isXalpha (str1 [2])) {
		if (sxget_token (++tok_no)->lahead == EOL_t) {
		    PTOK->lahead = ASSIGN_t;
		    PTOK->string_table_entry = SXEMPTY_STE;
		    tt [0].lahead = ENTIER_t;
		    tt [0].string_table_entry = sxstr2save (str, str1 - str);
		    tt [0].source_index =
			*get_source_index (&(PTOK->source_index), 6);
		    tt [0].comment = NULL;
		    tt [1].lahead = TO_t;
		    tt [1].string_table_entry = SXEMPTY_STE;
		    tt [1].source_index =
			*get_source_index (&(PTOK->source_index), 6 + str1 - str);
		    tt [1].comment = NULL;
		    tt [2].lahead = ID_t;
		    tt [2].string_table_entry = sxstrsave (str1 + 2);
		    tt [2].source_index =
			*get_source_index (&(PTOK->source_index), 8 + str1 - str);
		    tt [2].comment = NULL;

		    check_id (&(tt [2]));

		    sxtknmdf (&(tt [0]), 3, Mtok_no + 1, 0);
		}
		/* else
		   on laisse l'analyse normale se débrouiller avec ça!! */
	    }
	}
	
	break;
	
    case BLOCKDATA_p :
	/* On retourne 2 tokens pour BLOCK DATA  */
	lahead = DATA_t;
	l = 5;
	insert_a_token = true;

	break;
	
    case ELSEIF_p:
    case ENDDO_p: /* [[EXTENSION]] */
    case ENDIF_p:
        if (str [kw_lgth [kw]] != SXNUL)
	  /* str est de la forme "KW1KW2blabla" => id, donc on ne fait rien */
	  break;

	if (kw == ENDDO_p) {
	  SXINT                 i;
	  struct sxtoken        *pMtok_no, tok;
	  struct sxsource_coord source_coord;

	  /* Là, on suppose qu'on a bien un END DO (fermeture d'un bloc_do_loop non standard) */
	  /* On suppose de plus que le token %EOL (qui suit ce END DO) à été traité et donc que
	     la post-action lexicale @3 a été exécutée. Ce qui veut dire que si le source à la forme
	     lab   END DO %EOL
	     et si lab est l'étiquette terminale d'une labeled_do_loop, toutes les fins de bloc associées à ce label
	     ont déjà été générées par @3 devant le token %EOL qui se trouve en sxplocals.Mtok_no.
	     Dans ce cas le flot de tokens a la forme
	     ... EOL_t LABEL_t ID_t(Mtok_no) {END_t DO_t EOL_t|ENDDO_t} ENDDO_t EOL_t(sxplocals.Mtok_no)
	  */
	  pMtok_no = &(SXGET_TOKEN (sxplocals.Mtok_no));

	  if (pMtok_no->lahead != EOL_t) {
	    sxerror (pMtok_no->source_index,
		     sxsvar.sxtables->err_titles [2][0] /* ERROR */,
		     "%sThe token \"%s\" is unexpected after the non-standard END DO statement.",
		     sxsvar.sxtables->err_titles [2]+1 /* ERROR */,
		     sxttext (sxsvar.sxtables, pMtok_no->lahead)
		     );
	    sxtrap (ME, "Please warn the f77 maintenance.");
	  }

	  i = do_stack_mngr_search (-1);

	  if (i == 0) {
	    /* Il n'y a pas de DO correspondant à ce END DO!!
	       On pourrait laisser sxp_rcvr s'en occuper, on préfère un traitement + spécifique
	       On va remplacer ce END DO par l'instruction CONTINUE, ça permet de ne pas se soucier ni de ce qui précède
	       (label en particulier) ni de ce qui suit (%EOL, ...) */
	    /* On va donc remplacer le flot de tokens
	       ... EOL_t LABEL_t ID_t(Mtok_no) {END_t DO_t EOL_t|ENDDO_t} ENDDO_t EOL_t(sxplocals.Mtok_no)
	       par
	       ... EOL_t LABEL_t CONTINUE(Mtok_no) EOL_t {END_t DO_t EOL_t|ENDDO_t} ENDDO_t EOL_t(sxplocals.Mtok_no)
	    */
	    sxerror (PTOK->source_index,
		     sxsvar.sxtables->err_titles [2][0] /* error */,
		     "%sWithout visible block DO loop, this illegal END DO statement is replaced by CONTINUE.",
		     sxsvar.sxtables->err_titles [2]+1 /* error */
		     );
	    PTOK->lahead = CONTINUE_t;
	    PTOK->string_table_entry  = SXEMPTY_STE;

	    if (SXGET_TOKEN (Mtok_no+1).lahead != EOL_t) {
	      /* On insére un EOL_t */
	      tok = *pMtok_no;
	      sxtknmdf (&(tok), 1, Mtok_no+1, 0); /* sxplocals.Mtok_no a pu changer mais repère toujours l'EOL_t */
	    }

	    /* Que fait-on de la do_stack ? On choisit de ne pas y toucher, on laisse do_stack_top inchangé */

	    break;
	  }

	  /* 
	     Actuellement le flot de tokens peut avoir deux formes
	     1) ... EOL_t [LABEL_t] ID_t(Mtok_no) EOL_t(sxplocals.Mtok_no)
	     2) ... EOL_t LABEL_t ID_t(Mtok_no) {END_t DO_t EOL_t|ENDDO_t} ENDDO_t EOL_t(sxplocals.Mtok_no)
	     Dans le cas 1), il suffit de remplacer ID_t par {ENDDO_t} END_t DO_t pour obtenir
	     1') ... EOL_t [LABEL_t] {ENDDO_t} END_t DO_t EOL_t(sxplocals.Mtok_no)
	     Dans le cas 2) il faut remplacer ID_t par CONTINUE_t EOL_t et insérer {ENDDO_t} END_t DO_t
	     entre ENDDO_t EOL_t(sxplocals.Mtok_no) pour obtenir
	     2') ... EOL_t LABEL_t CONTINUE_t(Mtok_no) EOL_t {END_t DO_t EOL_t|ENDDO_t} ENDDO_t {ENDDO_t} END_t DO_t
	             EOL_t(sxplocals.Mtok_no)

	     Rappelons que la séquence
	     ... LABEL_t ENDDO_t...
	     est interdite et qu'elle doit être remplacée par
	     ... LABEL_t CONTINUE_t EOL_t ENDDO_t...
	  */

	  tt_stack_mngr_clear();

	  if (SXGET_TOKEN (Mtok_no - 1).lahead == LABEL_t
	      && (SXGET_TOKEN (sxplocals.Mtok_no - 1).lahead == ENDDO_t || i < do_stack_top)) {
	    /* On est dans le cas où le généré serait LABEL_t ENDDO_t, on va donc préparer CONTINUE_t EOL_t dans la tt_stack */
	    sxerror (PTOK->source_index,
		     sxsvar.sxtables->err_titles [2][0] /* ERROR */,
		     "%sThe label before this END DO statement cannot close a labeled do loop,\n\
a CONTINUE statement is gracefully inserted here.",
		     sxsvar.sxtables->err_titles [2]+1 /* ERROR */
		     );

	    tok = *PTOK;
	    tok.lahead = CONTINUE_t;
	    tt_push (true, tok);
	    tok.lahead = EOL_t;
	    tt_push (true, tok);
	  }

	  source_coord = pMtok_no->source_index; /* On note les coords de l'id */
	  /* On remplace ID_t par la séquence de la tt_stack (qui peut être vide) */
	  sxtknmdf (tt_stack+1, tt_stack_top, Mtok_no, 1); /* sxplocals.Mtok_no a pu changer mais repère toujours l'EOL_t */
	  //tok = SXGET_TOKEN (sxplocals.Mtok_no);
	  sxplocals.Mtok_no--; /* On enlève EOL_t ... */
	  do_stack_mngr_process (i, &(source_coord), false /* dans les tokens */); /* ... on génère les {ENDDO_t} END_t DO_t... */
	  do_stack_top = i-1; /* Mise à jour de la do_stack, on dépile le -1 */
	  //Déjà fait f77put_token (tok); /* ... et on remet l'EOL_t */

	  /* ... et voilà */
	  break;
	}
	else {
	  /* On retourne 2 tokens pour ELSE IF ou END IF */
	  lahead = IF_t;
	  l = (kw == ELSEIF_p) ? 4 : 3;
	}

	insert_a_token = true;

	break;

#ifdef ESOPE
    case ENDSEGMENT_p: /* [[ESOPE]] */
	/* On retourne la séquence de mots-clés END SEGMENT */
	lahead = SEGMENT_t;
	l = strlen ("END");
	insert_a_token = true;

	break;
#endif // [[ESOPE]]

    case GOTO_p :
        /* str est de la forme "GOTO(label|(|variable_name)=> ... */
	/* ...on retourne 2 tokens pour GO TO  */
	lahead = TO_t;
	l = 2;
	insert_a_token = true;

	break;

    case DOUBLEPRECISION_p:
    case DOUBLECOMPLEX_p:
	 /* Ce cas ne peut (normalement) etre atteint par ici, il l'est dorenavant par l'intermédiaire du "case DO_p:" */
	 sxtrap (ME, "process_initial_id for \"DOUBLE PRECISION\" or \"DOUBLE COMPLEX\"");
	 break;

    case CHARACTER_p:
    case COMPLEX_p:
    case INTEGER_p:
    case LOGICAL_p:
    case REAL_p:
	PTOK->lahead = kw_codes [kw];
	PTOK->string_table_entry  = SXEMPTY_STE;
	l = kw_lgth [kw];
	kw_nb = 1; /* nb de kw effectif */
	tok_no = Mtok_no;
	
	if (check_FUNCTION (str, strlgth, l, tok_no)) {
	    is_function_stmt = true;
	    break;
	}

	extract_int_id = true;

	break;

#ifdef ESOPE
    case IMPLIED_p: /* [[ESOPE]] */
    case POINTEUR_p: /* [[ESOPE]] */
    case SEGACT_p: /* [[ESOPE]] */
    case SEGADJ_p: /* [[ESOPE]] */
    case SEGDES_p: /* [[ESOPE]] */
    case SEGINA_p: /* [[ESOPE]] */
    case SEGIND_p: /* [[ESOPE]] */
    case SEGINI_p: /* [[ESOPE]] */
    case SEGMENT_p: /* [[ESOPE]] */
    case SEGPRT_p: /* [[ESOPE]] */
    case SEGSUP_p: /* [[ESOPE]] */
    {
	 char c = str [kw_lgth [kw]];

	 if (c == SXNUL) {
	      /* str est exactement un des mots-clés ESOPE en début d'instruction,
		 ça semble une erreur, on le transforme en id
		 et on laisse le traitement des erreurs se débrouiller avec ça!! */
	      PTOK->lahead = ID_t;
	      PTOK->string_table_entry = sxstrsave (str);
	      break;
	 }
	 
	 /* ici str est de la forme kwblabla */
	 if (isXalpha (c)) {
	      /* ici, blabla est un id */
	      if (kw == IMPLIED_p || kw == POINTEUR_p) {
		   PTOK->lahead = kw_codes [kw];
		   PTOK->string_table_entry = SXEMPTY_STE;
		   l = kw_lgth [kw];
		   kw_nb = 1; /* nb de kw effectif */
	
		   extract_int_id = true;
	      }
	      else {
		   SXINT curtok_no = Mtok_no+1; /* Le tok_no suivant kwblabla */
		   SXINT la1head = sxget_token (curtok_no)->lahead;

		   if (kw != SEGINA_p && kw != SEGIND_p) {
			/* Ici on est dans l'extension ESOPE qui permet la suppression de la ',' après le mot-clé
			 et PTOK a la forme kw id */
			/*
			  SEGMENT id %EOL
			  (SEGINI | SEGACT) id ['('...')'] (',' | '=' | '\n')
                          SEGDES id ['('...')'] (',' | '*' | '\n')
                          (SEGSUP | SEGADJ | SEGPRT) id ['('...')'] (',' | '\n')
			*/
			if (kw != SEGMENT_p) {
			     if (la1head == LEFTP_t) {
				  curtok_no = Mtok_no; /* Ne pas toucher à Mtok_no!! */

				  if (!check_balanced (&curtok_no))
				       break;

				  la1head = sxget_token (++curtok_no)->lahead; /* Le token après la ')' */
			     }
			     
			     if (la1head != VIRGULE_t && la1head != EOL_t) {
				  if ((kw == SEGDES_p && la1head == STAR_t) || ((kw == SEGINI_p || kw == SEGACT_p) && la1head == EGAL_t)) {
				  /* Ici
				     SEGDES id ['('...')'] '*' ou
				     (SEGINI | SEGACT) id ['('...')'] '='
				  */
				       if (kw == SEGDES_p) {
					    /* On est dans le cas SEGDES non suivi d'une virgule, le token STAR_t a été reconnu mais l'exécution
					       de sa post-action lexicale n'a pas pu positionner star_flag_mode_tok_no (pas encore de SEGDES, pas de ',', ...)
					       On le fait donc ici. */
					    star_flag_mode_tok_no = curtok_no;
#if EBUG
					    fprintf (stdout, "++++++++++++++ @12 : \"*\" is a star_flag_mode_tok_no at tok_no = %ld ++++++++++++++\n", curtok_no);
#endif
				       }
				  }
				  else
				       break;
			     }
			     /* else OK */
			}
			else {
			     if (la1head != EOL_t)
				  /* On laisse id */
				  break;
			}
		   }
		   else {
			/* Ici on a (SEGINA|SEGIND)id, le préfixe est un mot-clé ssi le suffixe de str est de la forme
			   = id , (CHAINE|BIN|XDR) %EOL */
			if (la1head != EGAL_t)
			     break;

			la1head = sxget_token (Mtok_no+2)->lahead;

			if (la1head != ID_t && sxkeywordp (sxsvar.sxtables, la1head) == 0)
			     break;

			/* Ici la1head est un id ou un mot-clé et sxsrcmngr.current_char contient le caractère suivant */
			
			if (sxget_token (Mtok_no+3)->lahead != VIRGULE_t)
			     break;

			SEGINA_SEGIND_tok_no = Mtok_no+3; /* signale à la scanact @1 qu'on est dans le traitement SEGINA/SEGIND
							     et on repère la ',' */
			lahead = sxget_token (Mtok_no+4)->lahead; /* (CHAINE|BIN|XDR) Attention, on appelle (récursivement) la scanact @1 */
			SEGINA_SEGIND_tok_no = 0; /* La @1 associée au kw CHAINE|BIN|XDR a été appelée */
		   
			if (lahead == CHAINE_t || lahead == BIN_t || lahead == XDR_t) {
			     /* On a reconnu une <123:segment_operation> */
			     /* Voir le traitement de ces tokens dans sxscanner_action (1).
				Le token suivant est %EOL */

			     if (la1head != ID_t) {
				  /* c'est un kw qu'il faut changer en id */
				  SXGET_TOKEN (Mtok_no+2).string_table_entry = sxstrsave (sxttext (sxsvar.sxtables, la1head));
				  SXGET_TOKEN (Mtok_no+2).lahead = ID_t;
			     }
			}
			else {
			     /* Ici incohérence, on choisit de reconnaître un SEGINA|SEGIND, on poursuit donc en séquence.
				Pour laisser le id initial (et donc une affectation erronée "id = id|kw ,", faire un break; */
			}
		   }

		   /* Ici on a reconnu kw id ctxt */
		   PTOK->lahead = kw_codes [kw]; /* On change le ID_t initial en kw ... */
		   PTOK->string_table_entry = SXEMPTY_STE;
		   /* ... et on insère derrière son suffixe comme étant un ID_t */
		   l = kw_lgth [kw];
		   kw_nb = 1; /* nb de kw effectif */
		   
		   extract_int_id = true;
	      }
	 }
    }

    break;
#endif // [[ESOPE]]

    case FUNCTION_p :
	is_function_stmt = true;
        /* FALLTHROUGH */

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
	PTOK->string_table_entry  = SXEMPTY_STE;
	l = kw_lgth [kw];
	kw_nb = 1; /* nb de kw effectif */
	extract_int_id = true;

	break;
	
    case DO_p:
      /* Spécif des do (voir f77.bnf)
	 Standard fortran 77
	 1:	= DO  <110b:label_ref> ,  <99:symbolic_name>  =  <40b:do_parameters> %EOL ;
	 2:	= DO  <110b:label_ref>  <99:symbolic_name>  =  <40b:do_parameters> %EOL ;
	 [[EXTENSION]]
	 3:	= DO  <99:symbolic_name>  =  <40b:do_parameters> %EOL ;
	 4:	= DO  WHILE ( <84:logical_expression> ) %EOL
	 5:	= DO  <110b:label_ref>  WHILE ( <84:logical_expression> ) %EOL ;
	 6:	= DO  <110b:label_ref> ,  WHILE ( <84:logical_expression> ) %EOL ;
       */
	if (!after_logical_if &&
	    strlgth >= 3) {
	  struct sxtoken	*PTOK1;
	  bool		        is_do_stmt;

	  str += strlen ("DO");
	  str1 = skip_an_int (str);

	  PTOK1 = &(SXGET_TOKEN (Mtok_no + 1)); /* safe */
	  /* PTOK1 == VIRGULE_t, EGAL_t ou LEFTP_t (ou STAR_t)
	     ou même EOL_t dans le cas DOUBLE PRECISION id */

	  if (str1 > str
	      && *str1 == SXNUL
	      && PTOK1->lahead == VIRGULE_t) {
	    /* le source est de la forme "do SXINT ," (cas 1 ou 6)
	       on suppose donc que c'est l'entête d'un <do_statement> avec label et on transforme l'id en DO_t suivi de ENTIER_t */
	    /* Dans le cas 6, extension_hit() sera appelé sur le WHILE */
	    is_do_stmt = true;
	    PTOK->lahead = DO_t;
	    PTOK->string_table_entry = SXEMPTY_STE;
	    tt [0].lahead = ENTIER_t;
	    tt [0].string_table_entry = sxstr2save (str, str1 - str);
	    tt [0].source_index =
	      *get_source_index (&(PTOK->source_index), strlen ("DO"));
	    tt [0].comment = NULL;
	    sxtknmdf (&(tt [0]), 1, Mtok_no + 1, 0); /* l'id est de la forme do SXINT */
	  }
	  else if ((isXalpha (*str1)) && (PTOK1->lahead == EGAL_t)) {
	    /* Ici l'id courant est de la forme "do [int] id", et le token suivant est "=" (cas 2 ou 3),
	       ça peut donc être une partie gauche d'affectation ou un <do_statement> */
	    /* Remarque si "[int]" est vide i.e. "str1 == str" on peut être dans l'extension de f77 */
	    /* Cas délicat où il faut analyser le contexte droit sur une longueur non bornée. */
	    /* Ce sous-langage contexte droit est décrit dans la bnf par la sous-grammaire introduite
	       par le terminal "!DO". Ce terminal ne peut pas être fabriqué par le scanner, quelque soit
	       la combinaison de caractères présente dans le source : l'analyse de ce sous-langage ne
	       peut donc pas être lancée accidentellement. */
	    /* Si l'id est de la forme "do [%SXINT] %id" => appel recursif du parser
	       pour savoir si c'est l'instruction do */
	    /* Candidat pour une instruction do */
	    /* On appelle recusivement le parser en simulant le point d'entree
	       !DO de la grammaire. */
	    /* Pierre le 10/03/2023 : j'ai changé la bnf pour faciliter la détection du sous-langage !DO ...
	       => traitement identique au !FORMAT
	       et on permet des corrections dans <40b:do_parameters> */
	    /* Si OK on rend "do [%SXINT] %id". */
	    /* On ne change rien dans le mode du scanner. */
	    SXINT		      head;
	    struct sxscan_mode        old_scan_mode;
	    struct sxtoken	      TOK, TOK1;

	    /* aucune modif sur sxsvar.sxlv_s.counters (aucune analyse lexicale particulière sur l'instruction DO) */
	    /* On memorise où on en est dans le source (si on doit repartir d'ici). */
	    head = source_line_head; /* No de buffer */
	    save_sxsrcmngr (); /* On ne sauve que les variables essentielles */

	    /* PTOK et PTOK1 pointent sur ID_t et EGAL_t, on mémorise ces tokens au cas où do... n'est pas une do loop */
	    TOK = *PTOK;
	    TOK1 = *PTOK1;

	    /* Le "mode" actuel du scanner */
	    old_scan_mode = sxsvar.sxlv.mode;
	    sxsvar.sxlv.mode = scan_mode_withmess;
	    
	    /*  on ajoute après le eol de fin de buffer un eof pour restreindre parse_in_la
		à rester sur la ligne logique courante */
	    if (sxsrcmngr.buffer [sxsrcmngr.bufused] != SXNEWLINE) {
	      /* Si ça se produit dans la vraie vie, ça veut dire que le token_manager a déjà eu besoin de ce
		 caractère et que le token %EOL a déjà été fabriqué et que c'est donc lui qu'il faudrait changer
		 en %EOF!! En fait il faudrait une détection plus fine (est-on bien dans le même buffer que DO?) */
	      sxtrap (ME, "sxscanner_action (before sxparse_in_la call for a DO stmt");
	    }

	    /* On met un EOF après le %EOL (il y a toujours la place) */
	    /* ... car on ne veut pas que EOF soit (syntaxiquement) un suivant valide du dernier token d'un
	       do-statement. */
	    sxsrcmngr.bufcoords [sxsrcmngr.bufused+1] = sxsrcmngr.bufcoords [sxsrcmngr.bufused];
	    sxsrcmngr.buffer [++sxsrcmngr.bufused] = EOF; /* caractère eof */ /* f77_next_char () doit pouvoir l'atteindre */
	      
	    /* On découpe le token *PTOK : str = "DO[int]id" en DO [ENTIER_t] et ID_t. */
	    /* On se souvient du token */
	    tt [0] = *PTOK;
	      
	    /* On change le token *PTOK en DO (même source_index) */
	    PTOK->lahead = DO_t;
	    PTOK->string_table_entry = SXEMPTY_STE;

	    /* On fabrique un token %int, si nécessaire */
	    if (str1 >  str) {
	      tt [1].lahead = ENTIER_t;
	      tt [1].string_table_entry = sxstr2save (str, str1 - str);
	      tt [1].source_index =
		*get_source_index (&(PTOK->source_index), 2 /* lgth("DO") */);
	      tt [1].comment = NULL;
	    }	    

	    /* ... et un token %id */
	    tt [2].lahead = ID_t;
	    tt [2].string_table_entry = sxstrsave (str1);
	    tt [2].source_index =
	      *get_source_index (&(PTOK->source_index), 2 + str1 - str /* lgth("DO%int") */);
	    tt [2].comment = NULL;

	    /* ... et on les insère devant le token suivant (le "=") qui se trouve en Mtok_no+1 */
	    sxtknmdf (&(tt [(str1 >  str) ? 1 : 2]), (str1 >  str) ? 2 : 1, Mtok_no + 1, 0);
	
#if EBUG
	    fprintf (stdout,
		     "**************** Entering sxparse_in_la (DO_EP_t = %ld, Mtok_no = %ld) ****************\n",
		     (SXINT) DO_EP_t,
		     (SXINT) Mtok_no);
	    print_sxsvar ();
	    fputs ("\n", stdout);
	    print_srcmngr_curchar ();
	    print_toks_buf (Mtok_no, sxplocals.Mtok_no);
	    fputs ("********************************************************************************************\n", stdout);
#endif
	      
	    if (we_are_in_parse_in_la)
	      /* A FAIRE */
	      sxtrap (ME, "Illegal recursive call of sxparse_in_la.");

	    {
	      /* Si, pendant sxparse_in_la on détecte une erreur que l'on peut corriger, on sort le message d'erreur
		 à ce moment puis, au retour, on conservera les tokens corrigés.  L'analyse normale les utilisera et
		 ça devrait bien se passer tout en exécutant en plus les actions sémantiques éventuelles.
	      */
	      /* PB : le 28/11/2023
		 Marche arrière car sur le source (qui est donc une affectation correcte)
		 do1i = 1
		 l'essai dans sxparse_in_la du source (donc d'une labeled do loop )
		 do 1 i = 1
		 conduit a une détection d'erreur qui se corrige en
		 do 1 i = 1 , %unsigned_dp_constant
		 cette correction validant le source comme étant une do loop!!
		 Maintenant, s'il y a eu une détection d'erreur dans sxparse_in_la, ce n'est pas une do loop
	      */
	      /* dans sxparse_in_la
		 on va uniquement (éventuellement) incrémenter do_parse_mode_noerr.mode.global_errors_nb
		 On n'exécute pas de traitement d'erreur car kind == SXWITHOUT_ERROR */
	      we_are_in_parse_in_la = true;
	      is_do_stmt = sxparse_in_la (DO_EP_t, Mtok_no, &tok_no,
					  &do_parse_mode_noerr /* détection mais aucun traitement des erreurs */);
	      /* is_do_stmt == true <=> pas de détection d'erreur */
	      /* Dans tous les cas sxplocals.mode n'a pas été touché */
	      /* ... en revanche do_parse_mode_noerr.global_errors_nb a pu l'être ssi is_do_stmt == false */
	      we_are_in_parse_in_la = false;
	      /* Remarque, le %EOL de la fin de carte logique n'a pas été rencontré et donc @3 non exécutée.*/

	      /* Il faut bien voir ici que do_parse_mode_noerr.global_errors_nb > 0 n'indique pas une erreur
		 (de syntaxe) mais uniquement qu'une construction de la forme DO... n'est pas un do-statement */
	      /* Au cas où ... */
	      do_parse_mode_noerr.global_errors_nb = 0;
	    }	    
		
#if EBUG
	    fprintf (stdout,
		     "**************** Leaving sxparse_in_la (eol/eof_tok_no = %ld, Mtok_no = %ld) %s ****************\n",
		     (SXINT) tok_no,
		     (SXINT) Mtok_no,
		     is_do_stmt ? "true" : "false");
	    print_sxsvar ();
	    fputs ("\n", stdout);
	    print_srcmngr_curchar ();
	    print_toks_buf (Mtok_no, tok_no);
	    fputs ("********************************************************************************************\n", stdout);
#endif
		
	    if (do_parse_mode_noerr.kind != SXWITHOUT_ERROR /* la lecture des tokens s'est arrêté sur la détection d'erreur */
		/* On met cette instruction au cas où "do_parse_mode" sera changé */ &&
		SXGET_TOKEN (tok_no).lahead != sxeof_code (sxsvar.sxtables) /* EOF_t */)
	      /* Dans tous ces cas, erreur ou pas, l'analyse du sous-langage s'est poursuivie jusqu'au EOF bidon */
	      sxtrap (ME, "sxscanner_action (after sxparse_in_la call for DO)");

	    /* On remet en état */
	    /* ... le mode du scanner */
	    sxsvar.sxlv.mode = old_scan_mode;

	    if (is_do_stmt) {
	      /* C'est un do_stmt (sans correction) */
	      /* Dans ce cas (pas d'erreur) on recommence toute
		 l'analyse car ce coup-ci, il faut exécuter les parsact/semact. */
	      /* En revanche, on conserve les tokens lus (donc aucune erreur
		 ne sera détectée dans cette partie). */
	      /* Il faut cependant supprimer le token bidon EOF. */
	      sxplocals.Mtok_no = tok_no-1; /* Le dernier token devient le %EOL */
	      /* On ne relit pas le source, pas d'appel à restore_sxsrcmngr () */
	      /* ... globalement sxsrcmngr est bon, excepté qu'on a lu eof qui n'existe pas dans la vraie vie!! */
	      sxsrcmngr.current_char = SXNEWLINE; /* On doit avoir sxsrcmngr.previous_char == SXNEWLINE */
	      sxsrcmngr.bufindex--; /* ... et on simule que la denière lecture est celle du EOL */
	      sxsrcmngr.bufused--; /* On rend le EOF inaccessible */
	      /* On lit (manuellement) le 1er caractère de la ligne suivante (à la place du eof bidon lu!!) */
	      f77_next_char ();

	      if (str1 == str /* pas de label => DO-LOOP (cas 3) */)
		extension_hit (XDO_LOOP_kind, SXGET_TOKEN (Mtok_no).source_index);

	      /* On fait cette vérif ici, après validation du do */
	      check_id (sxget_token (Mtok_no+((str1 == str) ? 1 : 2)));
	    }
	    else {
	      /* finalement, meme en corrigeant, ce n'est pas un do_stmt, on suppose donc que c'est un %id en lhs */
	      /* On remet le token stream (on pointe sur DO[int]id)... */
	      *PTOK = TOK;
	      *PTOK1 = TOK1;
	      sxplocals.Mtok_no = Mtok_no+1;
	      /* On remet le source "en place", on va tout relire */
	      source_line_head = head;
	      restore_sxsrcmngr ();
	      /* ... et on laisse l'analyse normale relire les tokens suivants */
	    }
	  }
	  else if (strcmp (str1, "WHILE") == 0) {
	    /* On est sur 'DO [int] WHILE ...' */
	    is_do_stmt = false;

	    if (SXGET_TOKEN (Mtok_no+1).lahead == LEFTP_t) {
	      /* On est sur 'DO [int] WHILE (...' => (cas 4 ou 5) */
	      is_do_stmt = true;
	      extension_hit (XDO_WHILE_kind, SXGET_TOKEN (Mtok_no).source_index);
	      /* On se souvient du token */
	      tt [0] = *PTOK;
	      
	      /* On change le token *PTOK en DO (même source_index) */
	      PTOK->lahead = DO_t;
	      PTOK->string_table_entry = SXEMPTY_STE;

	      /* On fabrique un token %int, si nécessaire */
	      if (str1 >  str) {
		tt [1].lahead = ENTIER_t;
		tt [1].string_table_entry = sxstr2save (str, str1 - str);
		tt [1].source_index =
		  *get_source_index (&(PTOK->source_index), 2 /* lgth("DO") */);
		tt [1].comment = NULL;
	      }	    

	      /* ... et un token WHILE */
	      tt [2].lahead = WHILE_t;
	      tt [2].string_table_entry = SXEMPTY_STE;
	      tt [2].source_index =
		*get_source_index (&(PTOK->source_index), 2 + str1 - str /* lgth("DOint") */);
	      tt [2].comment = NULL;

	      /* ... et on les insère devant le token suivant (le "=") qui se trouve en Mtok_no+1 */
	      sxtknmdf (&(tt [(str1 >  str) ? 1 : 2]), (str1 >  str) ? 2 : 1, Mtok_no + 1, 0);
	    }	    
	  }
	  else
	    /* Ici l'id à la forme "do ...", et n'est pas un <do_statement> */
	    is_do_stmt = false;

	  if (is_do_stmt) {
	    /* c'est une des trois formes de DO statement */
	    if (do_stack_xlbl != 0) {
	      /* Damned, ce do_statement est étiqueté par un label d'une labeled do loop.
		 C'est illegal
		 On va protester
		 on va générer un CONTINUE_t EOL_t
		 On va clore les do loop ouvertes (conformément au label)
	      */
	      struct sxtoken	*PTOKm1, tok;

	      PTOKm1 = &(SXGET_TOKEN (Mtok_no-1));

	      if (PTOKm1->lahead != LABEL_t)
		sxtrap (ME, "Unconsistancy during the processing of labels.");
	      
	      sxerror (do_stack [do_stack_xlbl].source_index,
		       sxsvar.sxtables->err_titles [2][0] /* ERROR */,
		       "%sThe end of this labeled do loop at line %i is illegal.",
		       sxsvar.sxtables->err_titles [2]+1 /* ERROR */,
		       PTOKm1->source_index.line
		       );
	      
	      sxerror (PTOK->source_index,
		       sxsvar.sxtables->err_titles [2][0] /* ERROR */,
		       "%sThe label before this do statement cannot close a labeled do loop,\n\
a CONTINUE statement is gracefully inserted here.",
		       sxsvar.sxtables->err_titles [2]+1 /* ERROR */
		       );

	      /* On génère CONTINUE_t EOL_t */

	      tt_stack_mngr_clear();
	      tok = *PTOK;
	      tok.lahead = CONTINUE_t;
	      tt_push (true, tok);
	      tok.lahead = EOL_t;
	      tt_push (true, tok);
	      
	      /* On génère les ENDDO */
	      do_stack_mngr_process (do_stack_xlbl, &(PTOK->source_index), true /* on range dans la tt_stack */);
	      do_stack_xlbl = 0;

	      /* ... et on insère tout ça devant DO_t */
	      sxtknmdf (tt_stack+1, tt_stack_top, Mtok_no, 0 /* insertion */);
	    }

	    /* on met à jour la do_stack */
	    do_stack_mngr_push (str1 == str ? -1 : get_int_val (str), &(PTOK->source_index));
	  }
	  else {
	       /* "do..." ce n'est pas un do stmt, on retourne donc sans rien changer... */
	       /* ... sauf si on est dans le cas DOUBLE(PRECISION|COMPLEX)[*len]id */
	       /* ... on peut être en lhs */
	       check_double_precision = (!is_a_lhs && strlgth >= (SXINT)strlen ("DOUBLEPRECISION") && strncmp (str, "UBLEPRECISION", strlen ("UBLEPRECISION")) == 0);
	       check_double_complex = (!is_a_lhs && strlgth >= (SXINT)strlen ("DOUBLECOMPLEX") && strncmp (str, "UBLECOMPLEX", strlen ("UBLECOMPLEX")) == 0);
	  }
	}
	
	break;
	
	
    case IMPLICIT_p:
      /* Ici l'id à la forme "IMPLICITblabla", mais il a ete decide precedemment (ds check_IMPLICIT_stmt()) que
	 ce n'était pas le début d'une IMPLICIT stmt!! => on ne fait rien, on laisse l'id */
      break;
	
    default:
	break;
    }
    
    if (check_double_precision || check_double_complex) {
	 /* initial_id est de la forme doblabla, ce n'est pas un do_statement,
	    c'est un doublecomplex ou un doubleprecision */
	 /* On retourne 2 tokens pour DOUBLE (PRECISION|COMPLEX) */
	 PTOK->lahead = DOUBLE_t;
	 PTOK->string_table_entry  = SXEMPTY_STE;
	 
	 tt [0].lahead = check_double_precision ? PRECISION_t : COMPLEX_t;
	 tt [0].string_table_entry = SXEMPTY_STE;
	 tt [0].source_index =
	      *get_source_index (&(PTOK->source_index), strlen ("DOUBLE"));
	 tt [0].comment = NULL;
	 sxtknmdf (&(tt [0]), 1, Mtok_no+1, 0);
	 str -= strlen ("DO"); /* On repointe vers le debut */
	 /* l doit indiquer le decalage depuis le debut de Mtok_no. */
	 l = check_double_precision ? strlen ("DOUBLEPRECISION") : strlen ("DOUBLECOMPLEX");
	 kw_nb = 2; /* nb de kw effectif */
	 tok_no = Mtok_no+1;
	
	 if (check_FUNCTION (str, strlgth, l, tok_no))
	      is_function_stmt = true;
	 else
	      extract_int_id = true;;
    }

    if (insert_a_token) {
	/* On change le %symbolic_name en kw */
	PTOK->lahead = kw_codes [kw]; /* code des préfixes */
	PTOK->string_table_entry  = SXEMPTY_STE;

	/* On prépare l'insertion du 2ème kw */
	tt [0].lahead = lahead;
	tt [0].string_table_entry = SXEMPTY_STE;
	tt [0].source_index =
	    *get_source_index (&(PTOK->source_index), l);
	tt [0].comment = NULL;
	sxtknmdf (&(tt [0]), 1, Mtok_no+1, 0);
	/* l doit indiquer le decalage depuis le debut de Mtok_no. */
	l = kw_lgth [kw]; /* str et l représentent le kw total */
	kw_nb = 2; /* nb de kw effectif */
        extract_int_id = true;
    }

    if (extract_int_id) {
	 str += l; /* On saute le(s) kw */

	 if (*str != SXNUL) {
	      /* Il reste qqchose derrière le/les kw que l'on vient d'insérer */
	      if (kw == RETURN_p && program_unit_kind == FUNCTION_SUBPROGRAM) {
		   /* alterate return in a FUNCTION */
		   SXINT eol_tok_no;

		   sxerror (*get_source_index (&(PTOK->source_index), l),
			    sxsvar.sxtables->err_titles [2][0] /* error */,
			    "%sAn alternate return is not allowed in a function subprogram,\n\
this expression is erased.",
			    sxsvar.sxtables->err_titles [2]+1 /* error */);
		   eol_tok_no = Mtok_no;

		   while (sxget_token (++eol_tok_no)->lahead != EOL_t);

		   if (eol_tok_no -= Mtok_no + 1)
			sxtknmdf (NULL, 0, Mtok_no + kw_nb, eol_tok_no);
	      }
	      else {
		   /* On extrait le suffixe qu'on insere ds le flot de tokens */
		   tt [0].lahead = isdigit (*str) ? ENTIER_t : ID_t;
		   tt [0].string_table_entry = sxstrsave (str);
		   tt [0].source_index = *get_source_index (&(PTOK->source_index), l);
		   tt [0].comment = NULL;
		   /* Attention check_for_a_lhs a lu le token suivant et peut-être beaucoup d'autres */

		   if (tt [0].lahead == ID_t)
			check_id (&(tt [0]));

		   sxtknmdf (&(tt [0]), 1, Mtok_no + kw_nb, 0);
	      }
	 }
    }
	
#if EBUG
    fputs ("**************** Leaving process_initial_id ****************\n",
	   stdout);
    print_toks_buf (left_tok_no, sxplocals.Mtok_no);
#endif
}
    
	    
	    
static void push_iokw (SXINT tok_no)
{
    /* On memorise tok_no dans iokw_stack. */
    if (++iokw_stack_top > iokw_stack_size)
	iokw_stack = (SXINT*) sxrealloc (iokw_stack, (iokw_stack_size += 10) + 1, sizeof (SXINT));
				
    iokw_stack [iokw_stack_top] = tok_no;
}


static void check_iokw (SXINT head, SXINT tail)
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

    SXINT	i, tok_no, t;
    bool	is_error = false;

#if 0
    CLEAR (t_stack);
#endif
    
    if (iokw_stack [1] != head + 2) {
	f77PUSH (t_stack, UNIT_t);
    }

    for (i = 1; i <= iokw_stack_top; i++) {
	tok_no = iokw_stack [i];
	t = SXGET_TOKEN (tok_no).lahead;

	if (t_stack [t] == 0) {
	    f77PUSH (t_stack, t);

	    if (t != END_t || SXGET_TOKEN (head).lahead != WRITE_t)
		iokw_stack [i] = - tok_no;
	    else
		/* An "END=" specifier must not appear in a WRITE statement. */
		is_error = true;
	}
	else
	    is_error = true;
    }

    if (is_error) {
	push_iokw (tail + 1); /* Il existe toujours un suivant. */

	/* On sort les messages d'erreur en premier... */
	for (i = iokw_stack_top - 1; i > 0; i--) {
	    if ((tok_no = iokw_stack [i]) > 0) {
		TOK_i = tok_no - 1;
		sxerror (SXGET_TOKEN (TOK_i).source_index,
			 sxsvar.sxtables->err_titles [2][0] /* error */,
			 "%sThis illegal occurrence of a specifier is erased from here....",
			 sxsvar.sxtables->err_titles [2]+1 /* error */);
		
		/* On cherche le specifier suivant. */
		if ((t = iokw_stack [i+1]) < 0)
		    t = -t;

		TOK_i = t - 1;
		sxerror (SXGET_TOKEN (TOK_i).source_index,
			 sxsvar.sxtables->err_titles [1][0],
			 "%s...until there.",
			 sxsvar.sxtables->err_titles [1]+1);
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

	if ((i == 0 && t == 0) || (i != 0 && t != 0))
	    sxerror (SXGET_TOKEN (head).source_index,
		     sxsvar.sxtables->err_titles [2][0] /* error */,
		     "%sThis statement should be either an INQUIRE by File\n\
or an INQUIRE by Unit.",
		     sxsvar.sxtables->err_titles [2]+1 /* error */);
    }
    else if (i == 0)
	sxerror (SXGET_TOKEN (head).source_index,
		 sxsvar.sxtables->err_titles [2][0] /* error */,
		 "%sThis statement must contain exactly one unit specifier.",
		 sxsvar.sxtables->err_titles [2]+1 /* error */);

    iokw_stack_top = 0;
    CLEAR (t_stack);
}


bool sxparser_action (SXINT code, SXINT act_no)
{
    SXINT lahead, tok_no;

    switch (code) {
    case SXOPEN:
    case SXCLOSE:
	return SXANY_BOOL;

    case SXINIT:
	return SXANY_BOOL;

    case SXFINAL:
	return SXANY_BOOL;

    case SXACTION:
	switch (act_no) {
	case 0:
	    /* Illegal <22:len_specification> */
	    if (is_ansi) {
		TOK_i = sxplocals.atok_no - 1;
		sxerror (SXGET_TOKEN (TOK_i).source_index,
			 sxsvar.sxtables->err_titles [1][0] /* warning */,
			 "%sIn the standard, a length specification can only be\n\
associated with the type CHARACTER.",
			 sxsvar.sxtables->err_titles [1]+1 /* warning */);
	    }
	    return SXANY_BOOL;
	    
	case 1:
	    /* @1 : on vient de détecter exactement 2 opérateurs consécutifs, cette [[EXTENSION]] est non valide en standard */
	    extension_hit (X2ops_kind, SXGET_TOKEN (sxplocals.atok_no - 1).source_index);
	    return SXANY_BOOL;

	case 2:
	    /* @2 : on vient de detecter un <87:rel_op> non standard (<, <=, ==, /=, >=, >),
	       cette [[EXTENSION]] est non valide en standard */
	    extension_hit (X2relop_kind, SXGET_TOKEN (sxplocals.atok_no - 1).source_index);
	    return SXANY_BOOL;

	case 3:
	    /* @3 : on vient de detecter un <10b:scalar_type> = DOUBLE  COMPLEX @3 ;
	       cette [[EXTENSION]] est non valide en standard */
	    extension_hit (X2dblcplx_kind, SXGET_TOKEN (sxplocals.atok_no - 1).source_index);
	    return SXANY_BOOL;

	case 4:
	    /* @4 : on vient de detecter un
	       <49:io_imply_do_list> = ( ( <48:io_list> ) &2 , @4  <99:symbolic_name>  =  <40b:do_parameters> ) ;
	       cette [[EXTENSION]] est non valide en standard */
	    extension_hit (X2ioidl_kind, SXGET_TOKEN (sxplocals.atok_no - 2).source_index);
	    return SXANY_BOOL;

          default:
	    break;
	}
        break;

    case SXPREDICATE:
	switch (act_no) {
	case 0:
            return false; /* Hubert : au lieu de "break" */

	case 1:
	    /* On est sur une virgule dans une io_list, une io_imply_do_list ou une
	       constante complexe. retourne VRAI si on n'est pas dans un complexe. */
	    if ((lahead = sxget_token (tok_no = sxplocals.ptok_no + 1)->lahead) == ID_t)
		return true;

	    if (lahead == PLUS_t || lahead == MINUS_t)
		lahead = sxget_token (++tok_no)->lahead;

	    if ((lahead == UIC_t || lahead == URC_t) &&
		sxget_token (tok_no + 1)->lahead == RIGHTP_t)
		return false;
		
	    return true;

	case 2:
	     /* [[EXTENSION]]
		On vérifie qu'on est bien sur la ')' fermant l' <48:io_list> d'une <49:io_imply_do_list> :
		<49:io_imply_do_list> = ( ( <48:io_list> ) &2 , @4  <99:symbolic_name>  =  <40b:do_parameters> ) ;
	     */
	    if (sxget_token (tok_no = sxplocals.ptok_no + 1)->lahead != VIRGULE_t)
		return false;

	    if (sxget_token (++tok_no)->lahead != ID_t)
		return false;

	    return sxget_token (++tok_no)->lahead == EGAL_t;

          default:
            break;
	}
        break;

    default:
        break;
    }
    fputs ("The function \"sxparser_action\" for f77 is out of date with respect to its specification.\n", sxstderr);
    abort ();
}


static bool check_stmt_sequence (SXINT lahead, SXINT tok_no
#if 0
				 , SXINT newla
#endif
     )
{
    /* Cette procedure verifie le sequencement correct des instructions */
    /* ETAT est une variable globale initialisee a ETAT_0 au debut et apres
       chaque END. */
    /* lahead peut etre different de SXGET_TOKEN (tok_no).lahead] */
    SXINT	        kind = stmt_kind [lahead];
#if 0
    SXINT		eol_tok_no;
    struct sxtoken	tt;
#endif
    bool                ret_val = true;

#if EBUG
    fprintf (stdout,
	   "================ Entering check_stmt_sequence (lahead = %s, tok_no = %ld) ETAT = %ld, kind = %ld ================\n",
	   sxttext (sxsvar.sxtables, lahead),
	   tok_no,
	   ETAT,
	   kind
	 );
#endif

    if (kind != 0) {
	 switch (ETAT) {
	 case ETAT_1:
	      switch (kind) {
	      case format_or_entry_k:
	      case parameter_k :
	      case implicit_k :
		   break;

	      case other_specification_k:
		   ETAT = ETAT_2;
		   break;

	      case data_k :
	      case executable_k :
		   ETAT = ETAT_3;
		   break;

	      case end_k :
		   ETAT = ETAT_0;
		   break;
	      default:
#if EBUG
		   sxtrap ("f77_analyse", "unknown switch case #3");
#endif
		   break;
	      }

	      break;

	 case ETAT_2:
	      switch (kind) {
	      case format_or_entry_k:
	      case parameter_k :
	      case other_specification_k:
		   break;

	      case data_k :
	      case executable_k :
		   ETAT = ETAT_3;
		   break;

	      case end_k :
		   ETAT = ETAT_0;
		   break;

	      case implicit_k :
		   break; /* Le 16/04/2023 à la place de ...
				   break; */
	    
	      default:
#if EBUG
		   sxtrap ("f77_analyse", "unknown switch case #4");
#endif
		   break;
	      }

	      break;

	 case ETAT_3:
	      switch (kind) {
	      case format_or_entry_k:
	      case data_k :
	      case executable_k :
		   break;

	      case end_k :
		   ETAT = ETAT_0;
		   break;

	      case parameter_k :
	      case implicit_k :
	      case other_specification_k:
		   ret_val = false;
		   break;

	      default:
#if EBUG
		   sxtrap ("f77_analyse", "unknown switch case #5");
#endif
		   break;
	      }

	      break;
	 default:
#if EBUG
	      sxtrap ("f77_analyse", "unknown switch case #2");
#endif
	      break;
	 }
    }

    if (!ret_val) {
	 /* Ici, erreur de sequence */

	 /* message */
	 sxerror (SXGET_TOKEN (tok_no).source_index,
		  sxsvar.sxtables->err_titles [2][0] /* Error */,
		  "%sThis %s statement is out of sequence.",
		  sxsvar.sxtables->err_titles [2]+1 /* Error */,
		  sxttext (sxsvar.sxtables, lahead));

#if 0
	 sxerror (SXGET_TOKEN (tok_no).source_index,
		  sxsvar.sxtables->err_titles [2][0] /* Error */,
		  "%sThis %s statement is out of sequence,\n\
it is %s.",
		  sxsvar.sxtables->err_titles [2]+1 /* Error */,
		  sxttext (sxsvar.sxtables, lahead),
		  newla == 0 ? "erased" : "changed into a CONTINUE statement");

	 /* Supprimé par PB le 13/10/24, le parser peut avoir commencé l'analyse de cette carte!! */
	 /* Si newla est nul on efface tout
	    si non nul on change le stmt fautif en CONTINUE, ca permet
	    - de ne pas toucher au label eventuel
	    - d'avoir un comportement "correct" si jamais ce stmt label fermait
	    un DO
	    - il n'est pas pris en compte ds le sequencement => les autres erreurs de
	    sequencement seront decouvertes. */
	 /* Remplacement des tokens jusqu'a EOL par CONTINUE */
	 eol_tok_no = tok_no;

	 if (newla != 0) {
	      tt = SXGET_TOKEN (tok_no);
	      tt.lahead = CONTINUE_t;
	      tt.string_table_entry = SXEMPTY_STE;
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
#endif
    }
    
#if EBUG
  fprintf (stdout,
	   "================ Leaving check_stmt_sequence () ETAT = %ld, ret_val = %s ================\n",
	   ETAT,
	   ret_val ? "true" : "false"
       );
#endif

    return ret_val;
}


static void entry_or_return_in_main (SXINT lahead, SXINT tok_no)
{
    /* On est ds un MAIN program sur un ENTRY ou RETURN stmt. */
    SXINT			eol_tok_no;
    struct sxtoken	tt;

    sxerror (SXGET_TOKEN (tok_no).source_index,
	     sxsvar.sxtables->err_titles [2][0] /* Error */,
	     "%sThis %s statement is illegal in a MAIN program,\n\
it is changed into a CONTINUE statement.",
	     sxsvar.sxtables->err_titles [2]+1 /* Error */,
	     sxttext (sxsvar.sxtables, lahead));

    /* Remplacement des tokens jusqu'a EOL par CONTINUE */
    eol_tok_no = tok_no;
    tt = SXGET_TOKEN (tok_no);
    tt.lahead = CONTINUE_t;
    tt.string_table_entry = SXEMPTY_STE;

    while (sxget_token (++eol_tok_no)->lahead != EOL_t);

    sxtknmdf (&tt, 1, tok_no, eol_tok_no - tok_no);
}

static bool check_block_data_subprogram (SXINT lahead, SXINT tok_no)
{
    /* On est ds un block_data_subprogram. lahead ne peut etre que
       BLOCK, IMPLICIT, PARAMETER, DIMENSION, COMMON, SAVE, EQUIVALENCE,
       DATA, END or type. */
    SXINT	kind;
    SXINT			eol_tok_no;

    kind = stmt_kind [lahead];

    switch (kind) {
    case format_or_entry_k:
    case executable_k :
	break;
	
    case implicit_k :
    case parameter_k :
    case data_k :
    case end_k :
	return true;
	
    case other_specification_k:
	if (lahead != INTRINSIC_t && lahead != EXTERNAL_t)
	    return true;
	
	break;
    default:
#if EBUG
      sxtrap ("f77_analyse", "unknown switch case #6");
#endif
      break;
    }
    
    sxerror (SXGET_TOKEN (tok_no).source_index,
	     sxsvar.sxtables->err_titles [2][0] /* Error */,
	     "%sThis %s statement is illegal in a BLOCK DATA SUBPROGRAM,\n\
it is erased.",
	     sxsvar.sxtables->err_titles [2]+1 /* Error */,
	     kind == executable_k ? "executable" : sxttext (sxsvar.sxtables, lahead));

    /* Suppression des tokens jusqu'a EOL. */
    eol_tok_no = tok_no;

    TOK_i = tok_no - 1;

    if (SXGET_TOKEN (TOK_i).lahead == LABEL_t)
	--tok_no;

    while (sxget_token (++eol_tok_no)->lahead != EOL_t);

    sxtknmdf (NULL, 0, tok_no, eol_tok_no - tok_no + 1);
    return false;
}

/* tok_no repère le END_t fermant un executable program unit et la do_stack est non vide.
   On proteste et on génère les ENDDO_t/END_t DO_t correspondants */
static void unclosed_do_loop (SXINT tok_no) {
  SXINT                 label;
  struct do_item	*do_item_ptr;
  static struct sxtoken	tok;

  tok = SXGET_TOKEN (tok_no);
  tt_stack_mngr_clear();

  while (do_stack_top > 0) {
    do_item_ptr = do_stack + do_stack_top;
    label = do_item_ptr->label;

    sxerror (do_item_ptr->source_index,
	     sxsvar.sxtables->err_titles [2][0] /* error */,
	     "%sThis %s do loop is not closed.",
	     sxsvar.sxtables->err_titles [2]+1 /* error */,
	     label == -1 ? "block" : "labeled"
	     );

     sxerror (tok.source_index,
	     sxsvar.sxtables->err_titles [2][0] /* error */,
	     "%sThe terminal statement of the %s do loop at line %ld is forced before the END of this %s.",
	     sxsvar.sxtables->err_titles [2]+1 /* error */,
	     label == -1 ? "block" : "labeled",
	     do_item_ptr->source_index.line,
	     executable_program_unit_name [program_unit_kind]
	     );

    if (label == -1) {
      tok.lahead = END_t;
      tt_stack_mngr_push (&tok);
    }

    tok.lahead = (label == -1) ? DO_t : ENDDO_t;
    tt_stack_mngr_push (&tok);

    tok.lahead = EOL_t;
    tt_stack_mngr_push (&tok);
	  
    do_stack_top--;
  }

  sxtknmdf (tt_stack+1, tt_stack_top, tok_no, 0);
}

/* On vérifie que eol_flag repère toujours le bon eol */
/* Si ce n'est pas le cas c'est qu'il a eu un sxtknmdf sans doute dû à une correction d'erreur, il faut peut-être
   remettre en état certaines variables statiques */
/* Pour l'instant, on traite head_tok_no */
static void check_eol_flag (void) {
     SXINT tok_no;
     
     if (eol_flag.tok_no > 0 &&
	 (SXGET_TOKEN (eol_flag.tok_no).lahead != EOL_t ||
	  SXGET_TOKEN (eol_flag.tok_no).source_index.line != eol_flag.line_no)) {
	  /* y'a un problème */
#if EBUG
	  fprintf (stdout, "???????????? Issue detected in check_eol_flag () :\
\t\teol_flag.(tok_no = %ld, line_no = %ld) while\n",
		   eol_flag.tok_no,
		   eol_flag.line_no);
	  print_tok (&(SXGET_TOKEN (eol_flag.tok_no)));
#endif
	  /* On cherche le tok de l'EOL le + à droite... */
	  for (tok_no = sxplocals.Mtok_no; tok_no > 0; tok_no--) {
	       if (SXGET_TOKEN (tok_no).lahead == EOL_t)
		    break;
	  }

	  if (tok_no > 0) {
	       /* ... c'est lui */
	       /* tok_no peut être le eol_flag déplacé ou un autre eol (cas où on a ajouté un eol après l'eol_flag ou supprimé
		  cet eol_flag */
	       /* tok_no - eol_flag.tok_no est le delta entre l'ancien et le nouveau eol le +à droite */
	       /* On suppose que ce delta s'applique aussi à head_tok_no (grossier) */
#if EBUG
	       {
		    SXINT delta = tok_no - eol_flag.tok_no;

		    fprintf (stdout, "head_tok_no : old = %ld, new = %ld\n", head_tok_no, head_tok_no + delta);
		    print_toks_buf (delta > 0 ? head_tok_no : head_tok_no+delta, sxplocals.Mtok_no);
	       }
#endif
	       head_tok_no += tok_no - eol_flag.tok_no;

	       eol_flag.tok_no = tok_no;
	       eol_flag.line_no = SXGET_TOKEN (tok_no).source_index.line;
	  }
     }
}

bool sxscanner_action (SXINT code, SXINT act_no) {
  SXINT prev_lahead;
#if 0
#if EBUG
  SXINT old_Mtok_no;
#endif
#endif

  switch (code) {
  case SXOPEN:

#if EBUG
  {
       SXINT kw = (*sxsvar.SXS_tables.S_check_keyword) ("FORMAT", strlen ("FORMAT"));

       if (kw != FORMAT_t) {
	    fprintf (stdout, "\tf77_t.c et f77_td.h ne sont pas compatibles :\n\
\tle mot-clé \"FORMAT\" vaut %ld (f77_t.c) et FORMAT_t vaut %ld (f77_td.h)\n",
		     kw, (SXINT)FORMAT_t);
	    sxtrap ("f77_analyse", "sxscanner_action (SXOPEN)");
       }
  }
#endif

    if (is_pretty_printer)
      /* La gestion des commentaires finaux est assurée par sxcomment_mngr.c */
      sxcomment_mngr (SXOPEN, 1024);

    do_stack_mngr_alloc (10);
    tt_stack_mngr_alloc (10);
    t_stack = (SXINT*) sxcalloc (sxeof_code (sxsvar.sxtables) + 1, sizeof (SXINT));
    iokw_stack = (SXINT*) sxalloc ((iokw_stack_size = 10) + 1, sizeof (SXINT));

    stmt_kind = (SXINT*) sxcalloc (sxeof_code (sxsvar.sxtables) + 1, sizeof (SXINT));

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
#ifdef ESOPE
    stmt_kind [IMPLIED_t] = other_specification_k; // [[ESOPE]]
#endif // [[ESOPE]]
    stmt_kind [INQUIRE_t] = executable_k;
    stmt_kind [INTEGER_t] = other_specification_k;
    stmt_kind [INTRINSIC_t] = other_specification_k;
    stmt_kind [LOGICAL_t] = other_specification_k;
    stmt_kind [OPEN_t] = executable_k;
    stmt_kind [PARAMETER_t] = parameter_k;
    stmt_kind [PAUSE_t] = executable_k;
#ifdef ESOPE
    stmt_kind [POINTEUR_t] = other_specification_k; // [[ESOPE]]
#endif // [[ESOPE]]
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

    /* Pour la mémoisation de check_balanced */
    memo_cb_mngr (SXOPEN);

    break;

  case SXINIT:
    /* On (ré)initialise toutes les variables locales */
       
    is_tab_already_met = is_hollerith_already_met = is_quoted_string_already_met =
      is_lower_to_upper_already_met = is_implicit_extension_already_met = false;
    t_stack [0] = -1; /* vide */
    do_stack_xlbl = 0;
    iokw_stack [0] = iokw_stack [1] = 0 /* Pour check_iokw () */;

    ETAT = ETAT_0;
    
#ifdef ESOPE
    star_flag_mode_tok_no = -1; /* Never seen */
#endif // [[ESOPE]]

    head_tok_no = 0;
    after_logical_if = false;
    eol_flag.tok_no = 0;

    main_program_nb = 0;
    is_function_stmt = is_iokw_allowed = is_implicit_statement = false;
    program_unit_kind = 0;
    we_are_in_parse_in_la = false;

    /* On modifie le parse_mode du parser (on garde tous les tokens), doit être appelé après sxparser (SXINIT, ...) */
    sxplocals.mode = f77_parse_mode;
    /* Raz des errors_nb qui peuvent provenir d'une exécution précédente */
    do_parse_mode_noerr.local_errors_nb = do_parse_mode_noerr.global_errors_nb = 0;
    format_parse_mode_withmess.local_errors_nb = format_parse_mode_withmess.global_errors_nb = 0;

#ifdef PLUS_TARD
    dont_is_mandatory = true; /* true <=> never select a Dont_Delete Dont_Insert */
#endif

    /* On vide le string_mngr */
    sxstr_mngr (SXCLEAR);

    break;

  case SXFINAL:
#if EBUG
    if (!is_pretty_printer)
      print_toks_buf (sxplocals.min, sxplocals.Mtok_no);
#endif

    break;

  case SXCLOSE:
    if (is_pretty_printer)
      /* La gestion des commentaires finaux est assurée par sxcomment_mngr.c */
      sxcomment_mngr (SXCLOSE, 0);

    sxfree (stmt_kind);
    do_stack_mngr_free();
    tt_stack_mngr_free();
    sxfree (iokw_stack);
    sxfree (t_stack);

    memo_cb_mngr (SXCLOSE);

    break;

  case SXACTION:
#if EBUG
    if (act_no != 6) {
      /* On ne regarde que les post-actions */
      fprintf (stdout, "**************** entering f77_scan_act (@%ld) sxplocals.Mtok_no = %ld ****************\n",
	       (SXINT) act_no,
	       (SXINT) sxplocals.Mtok_no);
    }
#endif
    
    if (act_no != 6) {
      /* On ne regarde que les post-actions */
      check_eol_flag ();
    }
    
    switch (act_no) {
#ifdef ESOPE
      /* Pour @10/@11 */
      static short                 save_previous_char, save_current_char;
      static SXINT                 save_bufindex;
      static struct sxsource_coord save_source_coord;
#endif // [[ESOPE]]

    case 0:
      {
	/* On vient de reconnaitre un label */
	/* On regarde dans la do_stack pour savoir si ce label ferme une labeled_do_loop... */
	do_stack_xlbl = do_stack_mngr_search (atoi (sxsvar.sxlv_s.token_string));
	/* ...oui si do_stack_xlbl > 0.  Dans ce cas, le traitement de ce ENDDO est différé jusqu'au %EOL (@3) suivant car
	   l'instruction ainsi étiqueté fait partie du bloc courant. */

	/* PB le 23/11/2023.  Je ne comprends plus la pertinence de ce qui suit => mise à l'écart (temporaire?) */
#if 0
	/* Afin de traiter certains cas d'erreur (suppression totale de
	   l'instruction, label compris), on lit le token suivant maintenant. */

	/* On range le token courant */
	f77put_token (sxsvar.sxlv.terminal_token);
#if EBUG
	old_Mtok_no = sxplocals.Mtok_no;
	print_toks_buf (old_Mtok_no, old_Mtok_no);
#endif
	/* on lit le suivant (et peut-etre beaucoup d'autres). */
	sxget_token (sxplocals.Mtok_no + 1);
#if EBUG
	print_toks_buf (old_Mtok_no, sxplocals.Mtok_no);
#endif
	/* On sort du scanner sans retourner aucun token */
	sxsvar.sxlv.terminal_token.lahead = 0;
	sxsvar.sxlv.terminal_token.string_table_entry = SXERROR_STE;
#endif
      }
	
      break;

    case 1:
      /* @1 est le point central et le plus délicat de l'analyse.
	 RAPPEL :
	 C'est la post-action lexicale appelée après la reconnaissance de l'E.R. %symbolic_name.
	 Cette E.R. a reconnu une séquence de lettres ou de chiffres précédée par une lettre.
	 Cette séquence est la plus longue possible.
	 Les blancs ou commentaires intermédiaires ont été éliminés.
	 Si cette séquence ne contient que des lettres qui constituent un mot-clé, c'est le code de ce mot-clé
	 (dans lahead) qui est positionné, sinon c'est le code de %symbolic_name.
	 Cependant ce code, fabriqué uniquement suivant des considérations lexicales peut ne pas être correct.
	 BUT :
	 Fabriquer le (ou la séquence de) code correct.
	 Pour cela, @1 va s'appuyer sur les contextes, à la fois gauche et droit, où apparaît ce mot.
	 Les transformations qui peuvent se produire sont de deux ordres :
	    - transformation d'un mot-clé en un %symbolic_name
	    - transformation d'un %symbolic_name en une séquence (comportant un ou plusieurs mots-clés) et des "résidus"
	      Par exemple do150blabla peut se transformer en DO int(label_ref) blabla(identificateur) ou
	      doubleprecisionfunctionmyfunc en DOUBLE PRECISION FUNCTION myfunc.
	 Les contextes droits vont être fournis par la lecture de tokens en avance.  Bien entendu, ces tokens peuvent
	 eux-mêmes être des %symbolic_name et donc appeler (récursivement) @1.  Dans tous les cas ce contexte droit
	 ne peut pas s'étendre au-delà du %eol de la ligne logique courante.  Dans certains cas (do_stmt et format_stmt),
	 ces tokens doivent être assemblés en unités syntaxiques (non-terminaux) et donc être reconnus par le parser
	 de syntax (appelé lui aussi récursivement par l'intermédiaire de la fonction sxparse_in_la).
      */
      {
	/* Post action de %id */	
	SXINT	Mtok_no, lahead, cur_lahead, tok_no;
	char	*str;
	SXINT	strlgth;
	bool	is_leading_symbolic_name, must_return = false;

	sxuse (strlgth);
	sxuse (str);

#if EBUG
	Mtok_no = sxplocals.Mtok_no;
#endif

	prev_lahead = SXGET_TOKEN (sxplocals.Mtok_no).lahead;
	cur_lahead = sxsvar.sxlv.terminal_token.lahead;

	if (cur_lahead == ID_t)
	  check_id (&sxsvar.sxlv.terminal_token);

	is_leading_symbolic_name = (prev_lahead == EOL_t ||
				    prev_lahead == LABEL_t ||
				    prev_lahead == sxeof_code (sxsvar.sxtables));

	if (is_leading_symbolic_name) {
	  /* C'est + sûr ici que ds @3 qui peut être exécutée en avance */
	  is_implicit_statement = false;
	  sxsvar.sxlv_s.counters[0] = 0;
	  sxsvar.sxlv_s.counters [1] = 0; /* Prudence */
#ifdef ESOPE
	  sxsvar.sxlv_s.counters [2] = 0; /* Pour les "old_seg_kw_type" Cf @8 et .lecl */
#endif // [[ESOPE]]
	  after_logical_if = false;
	}
	else if (!after_logical_if) {
	     /* On regarde si on est juste après un if logique : "if (...)" */
	     if (SXGET_TOKEN (head_tok_no).lahead == IF_t &&
		 cur_lahead != THEN_t /* pas <36:block_if_statement> */ &&
		 SXGET_TOKEN (head_tok_no+1).lahead == LEFTP_t &&
		 prev_lahead == RIGHTP_t &&
		 is_well_balanced (head_tok_no+1, sxplocals.Mtok_no)) {
		  after_logical_if = true;
		  is_leading_symbolic_name = true; /* On considere que cet id/kw est en début de carte */
	     }
	}

	switch (cur_lahead) {
#ifdef ESOPE
	case BIN_t: /* [[ESOPE]] */
	case CHAINE_t: /* [[ESOPE]] */
	case XDR_t: /* [[ESOPE]] */
	     if (SEGINA_SEGIND_tok_no == sxplocals.Mtok_no && sxsrcmngr.current_char == SXNEWLINE)
		  /* On est dans le traitement d'une chaine de la forme
		     (SEGINA|SEGIND)blabla = (id|kw) , (BIN|CHAINE|XDR)
		     et prev_lahead == VIRGULE_t
		     && on est devant un (futur) %EOL */
		  /* ... et on laisse au scanner le soin de ranger ce kw */
		  goto leaving_f77_scan_act_1;

	     goto change2id;

	case MOD_t: /* [[ESOPE]] */
	case MRU_t: /* [[ESOPE]] */
	case NOMOD_t: /* [[ESOPE]] */
	     /* En général ce sont des ids...
		sauf si on est derrière le flag '*' de :
		SEGDES , <88a:variable_or_array_element_name> * [ <123c:mode> | ( <123c:mode> , <123c:mode> { , <123c:mode> } ) ]
		avec <123c:mode> = MOD_t | MRU_t | NOMOD_t
		Voir case 12:
	     */
	     if (SXGET_TOKEN (head_tok_no).lahead == SEGDES_t &&
		 star_flag_mode_tok_no > head_tok_no /* On a rencontré le flag '*' du SEGDES courant */)
		  /* on laisse au scanner le soin de ranger ce kw */
		  goto leaving_f77_scan_act_1;

	     goto change2id;
#endif // [[ESOPE]]

	case THEN_t:
	     /* En général THEN est un id sauf si le début de la ligne est un [ELSE] IF
		et THEN est en fin de ligne */
	     if (!is_leading_symbolic_name)
		  /* Voir plus loin */
		  break;
	  
	     /* Ici, c'est un id */
	     /* FALLTHROUGH */

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
		et ce ne sont donc pas les mots_cles. On les change en ID_t. */
	case DOUBLE_t:
	case IMPLICIT_t:
	case NONE_t:
	case PRECISION_t:
	case UNDEFINED_t:
	     /* Ces mots-clefs ne sont jamais isolés => ce sont des ids */

#ifdef ESOPE
	change2id:
#endif // [[ESOPE]]
	     sxsvar.sxlv.terminal_token.lahead = ID_t;
	     sxsvar.sxlv.terminal_token.string_table_entry =
		  sxstrsave (sxttext (sxsvar.sxtables, cur_lahead));
	     
#ifdef ESOPE
	leaving_f77_scan_act_1:
#endif // [[ESOPE]]
	     must_return = true;
	     break;
#if EBUG
	     print_sxsvar ();
	     fputs ("\n", stdout);
	     print_srcmngr_curchar ();
	     fprintf (stdout,
		      "**************** leaving f77_scan_act (@%ld) sxplocals.Mtok_no = %ld ****************\n",
		      (SXINT) act_no,
		      (SXINT) sxplocals.Mtok_no);
#endif

	     /* ... et on laisse au scanner le soin de ranger cet ID_t */
	     return SXANY_BOOL;

	default:
	     break;
	}

	if (must_return)
	  break;

	if (is_leading_symbolic_name) {
	  /* On est en début d'instruction */
	  is_implicit_statement = check_IMPLICIT_stmt ();

	  if (is_implicit_statement)
	    /* Ici l'id initial a été transformé, à l'aide du contexte droit considere comme une chaine de caracteres, en les
	       mots clés IMPLICIT (CHARACTER | COMPLEX | DOUBLE PRECISION | INTEGER | LOGICAL | NONE |REAL | UNDEFINED)
	       On laisse au scanner le soin de ranger le dernier token */
	    return SXANY_BOOL;

	  /* si on n'est pas (exactement) dans ce cas, on continue... */
	    
	  /* On range le token courant reconnu par %symbolic_name.  Ce token peut donc être, pour l'instant,
	     un mot-clé, car la reconnaissance lexicale leur a donné priorité... */
	  f77put_token (sxsvar.sxlv.terminal_token);
	  /* ... en Mtok_no */

	  if (head_tok_no != 0) {
	    /* On n'est pas sur le 1er statement */
	    /* Tout ce qui suit a été déplacé depuis la fin de ce bloc et s'applique donc à la ligne précédente */
	    lahead = SXGET_TOKEN (head_tok_no).lahead; /* C'est donc le token de tête de la ligne précédente */

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
		  sxerror (SXGET_TOKEN (head_tok_no).source_index,
			   sxsvar.sxtables->err_titles [2][0] /* Error */,
			   "%sThis multiple MAIN program is illegal.",
			   sxsvar.sxtables->err_titles [2]+1 /* Error */);
		}
	      }
	    }
	    else {
	      if (lahead == ID_t)
		/* suppose type affectation */
		lahead = ASSIGN_t;
	      else {
		SXINT next_lahead;
		
		next_lahead = SXGET_TOKEN (head_tok_no + 1).lahead;

		if (lahead == END_t && (next_lahead == IF_t || next_lahead == DO_t
#ifdef ESOPE
					|| next_lahead == SEGMENT_t
#endif // [[ESOPE]]
			 ))
		  /* On ne veut pas repasser dans ETAT_0 */
#ifdef ESOPE
		     lahead = (next_lahead == SEGMENT_t)
			  ? INTEGER_t /* END SEGMENT : on reste dans une déclaration */
			  : IF_t; /* pourquoi pas? */
#else // [[ESOPE]]
		     lahead = IF_t; /* pourquoi pas? */
#endif // [[ESOPE]]
	      }

	      if (program_unit_kind == BLOCK_DATA_SUBPROGRAM) {
		if (check_block_data_subprogram (lahead, head_tok_no))
		  check_stmt_sequence (lahead, head_tok_no
#if 0
				       , 0
#endif
		       );
	      }
	      else
		check_stmt_sequence (lahead, head_tok_no
#if 0
				     , 1
#endif				     
		     );
	    }

	    if (program_unit_kind == MAIN_PROGRAM && (lahead == ENTRY_t || lahead == RETURN_t))
	      entry_or_return_in_main (lahead, head_tok_no);
		    

	    if (SXGET_TOKEN (head_tok_no).lahead == ID_t)
	      check_id (&(SXGET_TOKEN (head_tok_no)));
	  }

	  head_tok_no = sxplocals.Mtok_no;

	  Mtok_no = sxplocals.Mtok_no;

	  is_function_stmt = false;
	  is_iokw_allowed = false;

	    if (cur_lahead == FORMAT_t && prev_lahead == LABEL_t) {
	      /* On a %symbolic_name = "format".  Est-ce le mot-clé ou un id (en lhs par exemple)
		 On teste mot-clé avant "lhs" afin de permettre de protester
		 plus facilement si on trouve des chaines Hollerith a l'exterieur des formats. */
	      SXINT		head;
	      short             *last_buffer_ptr;
	      struct sxscan_mode	old_scan_mode;
	      bool		is_format_stmt;

	      /* On teste un FORMAT */
	      sxsvar.sxlv_s.counters [0] = 1; 
			
	      /* On memorise où on en est dans le source. */
	      head = source_line_head;
	      save_sxsrcmngr (); /* On ne sauve que les variables essentielles */
	    
	      /* Le "mode" actuel du scanner */
	      old_scan_mode = sxsvar.sxlv.mode;
	      sxsvar.sxlv.mode = scan_mode_withmess;
	      /* on change le eol de fin de buffer en eof pour restreindre parse_in_la
		 à rester sur la ligne logique courante */
	      last_buffer_ptr = sxsrcmngr.buffer + sxsrcmngr.bufused;
		
	      if (*last_buffer_ptr != SXNEWLINE) {
		/* Si ça se produit dans la vraie vie, ça veut dire que le token_manager a déjà eu besoin de ce
		   caractère et que le token %EOL a déjà été fabriqué et que c'est donc lui qu'il faudrait changer
		   en %EOF!! En fait il faudrait une détection plus fine (est-on bien dans le même buffer que FORMAT?) */
		sxtrap (ME, "sxscanner_action (before sxparse_in_la call for a FORMAT stmt");
	      }
    
	      *last_buffer_ptr = EOF; /* caractère eof */
	
#if EBUG
	      fprintf (stdout,
		       "**************** Entering sxparse_in_la (FORMAT_EP_t = %ld, Mtok_no = %ld) ****************\n",
		       (SXINT) FORMAT_EP_t,
		       (SXINT) Mtok_no);
	      print_sxsvar ();
	      fputs ("\n", stdout);
	      print_srcmngr_curchar ();
	      fputs ("********************************************************************************************\n", stdout);
#endif
	    
	      if (we_are_in_parse_in_la)
		/* A FAIRE */
		sxtrap (ME, "Illegal recursive call of sxparse_in_la.");

	      /* Si, pendant sxparse_in_la on détecte une erreur que l'on peut corriger, on sort le message d'erreur
		 à ce moment puis, au retour, on conservera les tokens corrigés.  L'analyse normale les utilisera et devrait bien
		 se passer tout en exécutant en plus les actions sémantiques éventuelles. */
	      we_are_in_parse_in_la = true;
	      is_format_stmt = sxparse_in_la (FORMAT_EP_t, Mtok_no, &tok_no, &format_parse_mode_withmess);
	      /* is_format_stmt => FORMAT... est un format-statement, soit tel quel, soit après correction
		 !is_format_stmt => FORMAT... n'est pas un format statement (aucune correction n'a marché)
		 il y a donc eu une détection d'erreur avec rattrapage global donc
		 format_parse_mode_withmess.global_errors_nb == 1 */
	      we_are_in_parse_in_la = false;

	      if (is_format_stmt) {
		   if (format_parse_mode_withmess.local_errors_nb) {
			/* ... donc FORMAT après correction */
			sxplocals.mode.local_errors_nb++; /* On comptabilise (globalement) la correction */
			format_parse_mode_withmess.local_errors_nb = 0; /* pour le coup d'après */
		   }
	      }
	      else {
		   /* Ici, rattrapage global, ce nest pas une erreur, "FORMAT ..." n'est pas un format-statement */
		   format_parse_mode_withmess.global_errors_nb = 0; /* pour le coup d'après */
	      }
		
		
#if EBUG
	      fprintf (stdout,
		       "**************** Leaving sxparse_in_la (eol/eof_tok_no = %ld, Mtok_no = %ld) %s ****************\n",
		       (SXINT) tok_no,
		       (SXINT) Mtok_no,
		       is_format_stmt ? "true" : "false");
	      print_sxsvar ();
	      fputs ("\n", stdout);
	      print_srcmngr_curchar ();
	      print_toks_buf (Mtok_no, tok_no);
	      fputs ("********************************************************************************************\n", stdout);
#endif

	      if (SXGET_TOKEN (tok_no).lahead != sxeof_code (sxsvar.sxtables) /* EOF_t */)
		/* Dans tous les cas, erreur ou pas, l'analyse du sous-langage s'est poursuivie jusqu'au EOF bidon */
		sxtrap (ME, "sxscanner_action (after sxparse_in_la call for FORMAT)");

	      /* On remet en état */
	      /* ... le mode du scanner */
	      sxsvar.sxlv.mode = old_scan_mode;

	      /* Dans tous les cas ce n'est plus un format_statement :
		 - ça a marché, on conserve les tokens lus ;
		 - ça a échoué et on repart avec un %id en lhs. */
	      sxsvar.sxlv_s.counters[0] = 0; 
	      /* sxsrcmngr.current_char contient le caractère eof, on y remet EOL pour être propre (dans tous les cas) */
	      *last_buffer_ptr = SXNEWLINE;
		
	      if (is_format_stmt) {
		/* C'est un format_stmt (éventuellement après correction) */
		/* Dans ce cas (pas d'erreur ou erreur corrigible) on recommence toute
		   l'analyse car ce coup-ci, il faut exécuter les parsact/semact. */
		/* En revanche, on conserve les tokens lus, même les tokens ERROR éventuels (donc plus aucune erreur
		   ne sera détectée dans cette partie). Il faut cependant supprimer le token bidon EOF de fin. */
		SXGET_TOKEN (tok_no).lahead = EOL_t;
		/* prudence, il se peut qu'il y ait eu plusieurs tokens EOF de fabriqués, on les supprime (ils seront écrasés) */
		sxplocals.Mtok_no = tok_no;
		sxsrcmngr.current_char = SXNEWLINE;
		/* On lit le 1er caractère de la ligne suivante car le eof ne l'a pas fait!! */
		f77_next_char ();
	      }
	      else {
		/* On dit que c'est un ident. */
		change_tok (Mtok_no, ID_t, sxttext (sxsvar.sxtables, cur_lahead));
		/* Et on supprime tous les tokens lus car ils peuvent changer (en genre, nombre ou source_index)
		   car on n'est plus sous l'influence d'un "FORMAT" */
		sxplocals.Mtok_no = Mtok_no;
		/* On remet le source "en place" */
		source_line_head = head;
		restore_sxsrcmngr ();
		/* ... et on laisse l'analyse normale relire les tokens suivants */
	      }

#if EBUG
	      fprintf (stdout,
		       "\nOn repart avec toks_buf [%ld:%ld]",
		       (SXINT) Mtok_no,
		       (SXINT) sxplocals.Mtok_no);
	      print_toks_buf (Mtok_no, sxplocals.Mtok_no);
#endif

	      /* Au cas où... */
	      format_parse_mode_withmess.global_errors_nb = 0;
	      format_parse_mode_withmess.local_errors_nb = 0;
	    }
	    else if (check_for_a_lhs (Mtok_no)) {
	      /* ça ressemble à une lhs... */
	      if ((cur_lahead = SXGET_TOKEN (Mtok_no).lahead) != ID_t /* ... reconnu comme mot-clé... */) {
		/* ...or ce n'est pas un mot_cle */
		/* on en fait un ident. */	
		change_tok (Mtok_no, ID_t, sxttext (sxsvar.sxtables, cur_lahead));
	      }
	      else {
		/* C'est un ident, mais... */
		/* ...attention, ca peut etre "DO [label] ident =" qui ressemble étrangement à une affectation. */
		/* En revanche, l'extension "DO [label] WHILE (...) %eol" ne ressemble pas à une affectation.
		   Ici, on n'est donc pas dans le cas "DO WHILE", il est traité plus loin (voir case ID_t) */
		TOK_i = Mtok_no + 1;

		if (SXGET_TOKEN (TOK_i).lahead == EGAL_t) {
		     SXINT kw = seek_kw (sxstrget (SXGET_TOKEN (Mtok_no).string_table_entry));

#if EBUG
		     fprintf (stdout,
			      "**************** seek_kw (\"%s\") = %ld ****************\n",
			      sxstrget (SXGET_TOKEN (Mtok_no).string_table_entry),
			      kw);
		     print_toks_buf (Mtok_no, Mtok_no);
#endif

		     if (kw == DO_p
#ifdef ESOPE
			 || kw == SEGINA_p || kw == SEGIND_p
#endif // [[ESOPE]]
			)
			  /* ident est de la forme do..., segina... ou segind... (cas [[ESOPE]] et est suivi de = */
			  /* ident est de la forme do...
			   Cet ident peut donc etre doubleprecision... ou doublecomplex... */
			  process_initial_id (Mtok_no, true /* is_a_lhs */);
		}
	      }
	    }
	    else {
	      /* On est en debut d'instruction et le token reconnu par %symbolic_name
		 n'est pas le debut d'une partie gauche d'affectation. */
	      switch (cur_lahead) {
	      case CHARACTER_t:
		/* On est dans le cas ou CHARACTER est suivi d'un delimiteur
		   on lit la suite pour reconnaitre si on est dans une
		   FUNCTION statement. */
		/* La lecture de 2 tokens devrait suffire? Non CHARACTER*(*)FUNCTION ou même
		   CHARACTER*(<int_constant_expr>)FUNCTION */
		sxget_token (Mtok_no + 2);
		break;
			    
	      case CLOSE_t:
	      case INQUIRE_t:
	      case OPEN_t:
	      case WRITE_t:
		tok_no = Mtok_no;
			    
		if (check_balanced (&tok_no))
		  check_iokw (Mtok_no, tok_no);
		else {
		  /* On en fait un %id. */
		  change_tok (Mtok_no, ID_t, sxttext (sxsvar.sxtables, cur_lahead));
		  check_id (&(SXGET_TOKEN (Mtok_no)));
		}

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

#if EBUG
		fprintf (stdout, "************************* %s : On est sur END, tok [%ld] et tok [%ld] doivent etre sur la meme ligne.\n",
			 ME, Mtok_no, tok_no+1);
		print_toks_buf (TOK_i, tok_no+1);
#endif
			    
		if (sxget_token (++tok_no)->lahead == EOL_t) {
		  /* C'est l'instruction end */
		  TOK_i = Mtok_no - 1;

		  if ((SXGET_TOKEN (TOK_i).lahead == LABEL_t &&
		       (SXGET_TOKEN (TOK_i).source_index.line !=
			SXGET_TOKEN (Mtok_no).source_index.line)) ||
		      SXGET_TOKEN (Mtok_no).source_index.line !=
		      SXGET_TOKEN (tok_no).source_index.line)
		    sxerror (SXGET_TOKEN (Mtok_no).source_index,
			     sxsvar.sxtables->err_titles [1][0] /* warning */,
			     "%sAn \"END\" statement is written only in columns 7 through 72 of an initial line.",
			     sxsvar.sxtables->err_titles [1]+1 /* warning */);

		  if (do_stack_top > 0)
		    /* L'action @3 a été exécutée */
		    unclosed_do_loop (Mtok_no);

		}
			    
		break;
			    
	      case FORMAT_t:
		/* Ce point d'entree ne peut etre atteint que sur une erreur,
		   ce n'est ni un format statement ni une lhs, on change le mot-cle
		   en ident pour avoir le traitement d'erreur du cas lhs. */
		change_tok (Mtok_no, ID_t, sxttext (sxsvar.sxtables, cur_lahead));
		break;
			    
	      case ID_t:
		process_initial_id (Mtok_no, false /* !is_a_lhs */);
		break;

#ifdef ESOPE
	      case SEGMENT_t:
		   /* (plus) rien!! */
		   break;
#endif // [[ESOPE]]
			    
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
#ifdef ESOPE
		  lahead = sxget_token (Mtok_no + 1)->lahead;
				
		  if (lahead == DOT_t) {
		       /* On en fait un %id. */
		       change_tok (Mtok_no, ID_t, sxttext (sxsvar.sxtables, cur_lahead));
		       check_id (&(SXGET_TOKEN (Mtok_no)));
		     }
		  else
#endif // [[ESOPE]]
		    /* Assume format_identifier */
		    change_tok (Mtok_no, READ_KW_t, "READ");
		}
			    
		break;
			    
	      case RETURN_t:
		TOK_i = Mtok_no + 1;

		if (program_unit_kind == FUNCTION_SUBPROGRAM &&
		    SXGET_TOKEN (TOK_i).lahead != EOL_t) {
		  /* alterate return in a FUNCTION */
		  SXINT eol_tok_no;

		  sxerror (SXGET_TOKEN (TOK_i).source_index,
			   sxsvar.sxtables->err_titles [2][0] /* error */,
			   "%sAn alternate return is not allowed in a function subprogram,\n\
this expression is erased.",
			   sxsvar.sxtables->err_titles [2]+1 /* error */);
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

	  /* On sort du scanner sans retourner aucun token */
	  sxsvar.sxlv.terminal_token.lahead = 0;
	  sxsvar.sxlv.terminal_token.string_table_entry = SXERROR_STE;
	}
	else 
	  /* Ident ou mot-cle en milieu d'instruction. */
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
	    f77put_token (sxsvar.sxlv.terminal_token);
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
	      check_id (&(SXGET_TOKEN (Mtok_no)));
	    }

	    /* On sort des actions sans retourner aucun token */
	    sxsvar.sxlv.terminal_token.lahead = 0;
	    sxsvar.sxlv.terminal_token.string_table_entry = SXERROR_STE;
	    break;

	  case CHARACTER_t:
	  case COMPLEX_t:
	  case INTEGER_t:
	  case LOGICAL_t:
	  case REAL_t:
	    if (is_implicit_statement) {
	      /* DOUBLE PRECISION est traité par ID_t */
	      break;
	    }

	    /* FALLTHROUGH */

	  case ASSIGN_t:
	  case BACKSPACE_t:
	  case BLOCK_t:
	  case CALL_t:
	  case CLOSE_t:
	  case COMMON_t:
	  case CONTINUE_t:
	  case DATA_t:
	  case DIMENSION_t:
	  case DO_t:
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
#ifdef ESOPE
	  case IMPLIED_t:
#endif // [[ESOPE]]
	  case INQUIRE_t:
	  case INTRINSIC_t:
	  case NONE_t:
	  case OPEN_t:
	  case PARAMETER_t:
	  case PAUSE_t:
#ifdef ESOPE
	  case POINTEUR_t:
#endif // [[ESOPE]]
	  case PRECISION_t:
	  case PROGRAM_t:
	  case READ_t:
	  case RETURN_t:
	  case REWIND_t:
	  case SAVE_t:
#ifdef ESOPE
	  case SEGACT_t:
	  case SEGADJ_t:
	  case SEGDES_t:
	  case SEGINA_t:
	  case SEGIND_t:
	  case SEGINI_t:
	  case SEGMENT_t:
	  case SEGPRT_t:
	  case SEGSUP_t:
#endif // [[ESOPE]]
	  case STOP_t:
	  case SUBROUTINE_t:
	  case TO_t:
	  case UNDEFINED_t:
	  case WRITE_t:
	    /* On en fait des idents. */
	    sxsvar.sxlv.terminal_token.lahead = ID_t;
	    sxsvar.sxlv.terminal_token.string_table_entry =
	      sxstrsave (sxttext (sxsvar.sxtables, cur_lahead));

	      check_id (&(sxsvar.sxlv.terminal_token));

	    break;
	    
#ifdef ESOPE
	  case CHAINE_t:
	  case BIN_t:
	  case XDR_t:
	       /* Si le 1er token de la ligne est un <123b:other_segment_operation_name> (SEGINA_t ou SEGIND_t) alors
		  ce sont des mots-clés (on ne touche rien) sinon, ils sont changés en ids */
	    if (SXGET_TOKEN (head_tok_no).lahead != SEGINA_t &&
		 SXGET_TOKEN (head_tok_no).lahead != SEGIND_t) {
	      /* On en fait un id (check_id () est inutile) */
	      sxsvar.sxlv.terminal_token.lahead = ID_t;
	      sxsvar.sxlv.terminal_token.string_table_entry =
		sxstrsave (sxttext (sxsvar.sxtables, cur_lahead));
	    }

	    break;
#endif // [[ESOPE]]

	  case WHILE_t:
	    /* On est dans le cas mot-clé ssi le 1er token de la ligne est un DO */
	    if (SXGET_TOKEN (head_tok_no).lahead != DO_t) {
	      /* On en fait un id (check_id () est inutile) */
	      sxsvar.sxlv.terminal_token.lahead = ID_t;
	      sxsvar.sxlv.terminal_token.string_table_entry =
		sxstrsave (sxttext (sxsvar.sxtables, cur_lahead));
	    }
	    else
	      extension_hit (XDO_WHILE_kind, SXGET_TOKEN (head_tok_no).source_index);

	    break;

	  case THEN_t:
	    /* On n'est pas en début d'instruction */
	    /* En général THEN est un id sauf si le début de la ligne est un [ELSE] IF
	       et THEN est en fin de ligne */
	    /* Si on est dans le cas ELSE IF, cette action @1 est appelée depuis l'action @1 qui traite l'id ELSEIF_t.
	       On simplifie la reconnaissance de ce cas (on ne regarde que EOL). Si pb, il faudra améliorer en verifiant
	       le ELSEIF */
	    if (sxsrcmngr.current_char != SXNEWLINE) {
	      /* Ce n'est pas le kw THEN, on le change en id... */
	      sxsvar.sxlv.terminal_token.lahead = ID_t;
	      sxsvar.sxlv.terminal_token.string_table_entry =
		sxstrsave (sxttext (sxsvar.sxtables, cur_lahead));
	    }	
#if EBUG
	    print_sxsvar ();
	    fputs ("\n", stdout);
	    print_srcmngr_curchar ();
	    fprintf (stdout,
		     "**************** leaving f77_scan_act (@%ld) sxplocals.Mtok_no = %ld ****************\n",
		     (SXINT) act_no,
		     (SXINT) sxplocals.Mtok_no);
#endif
	    /* ... et on laisse au scanner le soin de ranger cet ID_t */

	  break;

	  case ID_t:
	    /* id n'est pas en tete de carte */
	    {
	      SXINT ste = sxsvar.sxlv.terminal_token.string_table_entry;
	      
	      if (is_implicit_statement) {
		bool is_double_precision = strcmp (sxstrget (ste), "DOUBLEPRECISION") == 0;

		/* On regarde si on est dans le cas DOUBLE PRECISION ou DOUBLE COMPLEX */
		if (is_double_precision || strcmp (sxstrget (ste), "DOUBLECOMPLEX") == 0)	    {
		  sxsvar.sxlv.terminal_token.lahead = DOUBLE_t;
		  sxsvar.sxlv.terminal_token.string_table_entry = SXEMPTY_STE;
		  /* On range DOUBLE_t... */
		  f77put_token (sxsvar.sxlv.terminal_token);
		  /* ... en Mtok_no et ...*/
		  sxsvar.sxlv.terminal_token.lahead = is_double_precision ? PRECISION_t : COMPLEX_t;
		  sxsvar.sxlv.terminal_token.string_table_entry = SXEMPTY_STE;
		  sxsvar.sxlv.terminal_token.source_index =
		    *get_source_index (&(sxsvar.sxlv.terminal_token.source_index), strlen ("DOUBLE"));
		  sxsvar.sxlv.terminal_token.comment = NULL;
		  /* ... on laisse PRECISION_t ou COMPLEX_t au scanner */
		}
	      }
	      else if (check_inside_FUNCTION (sxplocals.Mtok_no, sxstrget (ste), sxstrlen (ste))) {
		/* on regarde si cet id a la forme FUNCTIONid( et est précédé d'un <10a:type> */	    
		/* ... Le token FUNCTION a été sorti, on laisse le scanner sortir l'id suffixe */
		is_function_stmt = true;
	      }
	    }
	    
	    /* On continue en sequence */
	    /* FALLTHROUGH */
	
	  default:
	    /* "mot_cles" traites syntaxiquement. */
	    if (sxsvar.sxlv.terminal_token.lahead == ID_t)
	      check_id (&(sxsvar.sxlv.terminal_token));

	    break;
	  }
	
#if EBUG
	fprintf (stdout, "****** @1 ******\n");
	print_toks_buf (Mtok_no-1, sxplocals.Mtok_no);
#endif

	if (SXGET_TOKEN (sxplocals.Mtok_no).lahead == EOL_t) {
	     /* Le eol_flag déjà mémorisé pour la ligne courante par un appel à @3 a pu être déplacé par le traitement
		courant (@1), par exemple un id changé en kw id'
		Son line_no, lui, n'a pas changé */
	     eol_flag.tok_no = sxplocals.Mtok_no;
	}
      }

      break;
	
    case 2:
      {
	/* Traitement des chaines entre doubles quotes.
	   Ce n'est pas de l'ansi standard, dans tous les cas on les change en apostrophe, sauf si on est en f77 étendu. */
	char *str;
	SXINT strlen_local ;

	if ((is_ansi || !is_extension) && !is_quoted_string_already_met)  {
	  is_quoted_string_already_met = true;
	  sxerror (sxsvar.sxlv.terminal_token.source_index,
		   sxsvar.sxtables->err_titles [1][0] /* warning */,
		   "%sIn the standard, CHARACTER constants must be enclosed between apostrophes,\n\
This one, and all others in the sequel, are converted into a legal form.",
		   sxsvar.sxtables->err_titles [1]+1 /* warning */);
	}

	if (!is_extension) {
	  str = sxstrget (sxsvar.sxlv.terminal_token.string_table_entry) + 1; /* apres " */
	  strlen_local = sxstrlen (sxsvar.sxlv.terminal_token.string_table_entry) - 2;
	  sxsvar.sxlv.ts_lgth = 0;
	  ts_put ('\'');

	  /* On double les apostrophes. */
	  while (strlen_local-- > 0) {
	    if (*str == '\'')
	      ts_put ('\'');

	    ts_put (*str++);
	  }

	  ts_put ('\'');
	  sxsvar.sxlv.terminal_token.string_table_entry =
	    sxstr2save (sxsvar.sxlv_s.token_string, sxsvar.sxlv.ts_lgth);
	}
      }

      break;
	
    case 3:
      /* On est sur eol */
      eol_flag.tok_no = sxplocals.Mtok_no+1; /* Celui de eol */
      eol_flag.line_no = sxsvar.sxlv.terminal_token.source_index.line;

      if (do_stack_xlbl != 0) {
	/* On range l'eol courant */
	f77put_token (sxsvar.sxlv.terminal_token);
	/* On génère les {EOL_t ENDDO_t}+ */
	tt_stack_mngr_clear ();
	do_stack_mngr_process (do_stack_xlbl, &(sxsvar.sxlv.terminal_token.source_index), false /* on utilise sxput_token() */);
	do_stack_xlbl = 0;
	/* On sort des actions sans retourner aucun token (Le EOL_t vient d'être mis!!) */
	sxsvar.sxlv.terminal_token.lahead = 0;
	sxsvar.sxlv.terminal_token.string_table_entry = SXERROR_STE;
      }

      break;

    case 4: {
      /* traitement de la zone Hollerith */

      if (sxsvar.sxlv_s.counters [0]) {
	/* On est dans un format, occurrence licite.
	   Les erreurs eventuelles ont deja ete sorties, mais la longueur
	   annoncee peut ne pas etre compatible avec la longueur reelle. */
	SXINT val, val_lgth, val_init;
	char string [12];

	val = val_init = atoi (sxsvar.sxlv_s.token_string);
	val_lgth = sxsvar.sxlv.ts_lgth - 1;
	/* H est deja stocke. */
	/* On stocke val (on s'arrete sur eol) caracteres. */

	while (val-- > 0 && sxsrcmngr.current_char != SXNEWLINE) {
	  ts_put (sxsrcmngr.current_char);
	  f77_next_char ();
	}

	if (val > 0) {
	  /* On diminue la longueur annoncee. */
	  /* Il faut representer (val_init - val) sur val_lgth caracteres. */
	  /* val_init - val tient sur 2 chiffres. */
	  SXINT ts_lgth = sxsvar.sxlv.ts_lgth;

	  sprintf (string, "%.2ld", (SXINT) (val_init - val));
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
	char *str = sxsvar.sxlv_s.token_string;
	SXINT lgth = 0;
	short c;

	while (isdigit (str [lgth]))
	  lgth++;

	sxsvar.sxlv.terminal_token.lahead = ENTIER_t;
	sxsvar.sxlv.terminal_token.string_table_entry = sxstr2save (str, lgth);
	f77put_token(sxsvar.sxlv.terminal_token);

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

	check_id (&(sxsvar.sxlv.terminal_token));
      }
      else {
	/* C'est une chaine Hollerith que l'on va transformer en chaine standard entre apostrophes */
	/* On proteste si is_ansi et si c'est la premiere fois. */
	SXINT val;

	if (is_ansi && !is_hollerith_already_met)  {
	  is_hollerith_already_met = true;
	  sxerror (sxsvar.sxlv.terminal_token.source_index,
		   sxsvar.sxtables->err_titles [1][0] /* warning */,
		   "%sIn the standard, Hollerith constants can only occur inside format specifications,\n\
This one, and all others in the sequel, are converted into a CHARACTER constant.",
		   sxsvar.sxtables->err_titles [1]+1 /* warning */);
	}

	val = atoi (sxsvar.sxlv_s.token_string);
	sxsvar.sxlv.ts_lgth = 0;
	ts_put ('\'');
	/* On stocke val (on s'arrete sur eol) caracteres. */

	/* On double les apostrophes. */
	while (val-- > 0 && sxsrcmngr.current_char != SXNEWLINE) {
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

      break;

    case 5:
      /* traitement de SXINT||id */
      /* On regarde si le token courant est précédé par "CHARACTER *", on découpe en "int id" */
      /* Sinon, on ne fait rien, si incorrect, on laisse le traitement d'erreur s'en occuper? */
      if (SXGET_TOKEN (sxplocals.Mtok_no).lahead == STAR_t) {
	SXINT prev_Mtok_no_lahead = SXGET_TOKEN (sxplocals.Mtok_no - 1).lahead;
	
	if (prev_Mtok_no_lahead == CHARACTER_t ||
	    prev_Mtok_no_lahead == COMPLEX_t ||
	    prev_Mtok_no_lahead == INTEGER_t ||
	    prev_Mtok_no_lahead == LOGICAL_t ||
	    prev_Mtok_no_lahead == PRECISION_t ||
	    prev_Mtok_no_lahead == REAL_t) {
	  /* On accepte la spécif de longueur, même si !is_extension => message d'erreur + précis */
	  char *str = sxsvar.sxlv_s.token_string;
	  SXINT lgth, prev_lahead_local, int_tok_no = sxplocals.Mtok_no;
	  SXINT id_lgth, column_shift;

	  lgth = 0;

	  while (isdigit (str [lgth]))
	    lgth++;

	  switch (prev_lahead_local = sxsvar.sxlv.terminal_token.lahead) {
	  case UDPC_t:
	    /* SXINT D [+-] SXINT */
	  case URC_t:
	    /* SXINT E [+-] SXINT */
	    if (str [lgth+1] == '+' || str [lgth+1] == '-') {
	      /* Erreur, on "corrige" */
	      struct sxsource_coord	si;
	    
	      str [lgth+1] = '0'; /* Pourquoi pas? */
	      si = sxsvar.sxlv.terminal_token.source_index;
	      si.column += lgth+1; /* sur le '+-' */

	      sxerror (si,
		       sxsvar.sxtables->err_titles [2][0] /* error */,
		       "%sIllegal character replaced by a '0'.",
		       sxsvar.sxtables->err_titles [2]+1 /* error */);
	    }		  
	    /* FALLTHROUGH */

	    case INT_ID_t:
	    sxsvar.sxlv.terminal_token.lahead = ENTIER_t;
	    sxsvar.sxlv.terminal_token.string_table_entry = sxstr2save (str, lgth);
	    /* On ne change pas le source_index */
	    f77put_token(sxsvar.sxlv.terminal_token);
	    int_tok_no = sxplocals.Mtok_no; /* tok_no de l'entier */

	    id_lgth = strlen (str) - lgth;

	    if (prev_lahead_local == INT_ID_t
		&& id_lgth > 8
		&& strncmp (str + lgth, "FUNCTION", 8) == 0
		&& isXalpha (str [lgth + 8])) {
	      sxsvar.sxlv.terminal_token.lahead = FUNCTION_t;
	      sxsvar.sxlv.terminal_token.string_table_entry = SXEMPTY_STE;
	      sxsvar.sxlv.terminal_token.source_index =
		*get_source_index (&(sxsvar.sxlv.terminal_token.source_index), lgth);
	      f77put_token(sxsvar.sxlv.terminal_token);
	      is_function_stmt = true;
	      column_shift = lgth += 8;
	    }
	    else
	      column_shift = 0;

	    sxsvar.sxlv.terminal_token.lahead = ID_t;
	    sxsvar.sxlv.terminal_token.string_table_entry = sxstrsave (str + lgth);
	    sxsvar.sxlv.terminal_token.source_index =
	      *get_source_index (&(sxsvar.sxlv.terminal_token.source_index), column_shift);

	    check_id (&(sxsvar.sxlv.terminal_token));

	    break;

	  default:
#if EBUG
	    sxtrap ("f77_analyse", "unknown switch case #7");
#endif
	    break;
	  }

	  /* On vérifie les tailles (même si !is_extension) */
	  check_scalar_type_repr_lgth (prev_Mtok_no_lahead, &(SXGET_TOKEN (int_tok_no)));
	  
	}
	/* else ERROR, on laisse la rcvr se débrouiller avec ça!! */
      }

#if EBUG
      fprintf (stdout, "****** @5 ******\n");
      print_toks_buf (1, sxplocals.Mtok_no);
#endif

      break;

    case 6:
      /* @Lower_To_Upper + test ansi */
      sxsvar.sxlv_s.token_string [sxsvar.sxlv.ts_lgth - 1] += 'A' - 'a';

      if (is_ansi && !is_lower_to_upper_already_met)  {
	is_lower_to_upper_already_met = true;
	sxerror (*get_source_index (&sxsrcmngr.source_coord, -1),
		 sxsvar.sxtables->err_titles [1][0] /* warning */,
		 "%sIn the standard, lower case letters outside comments and strings are illegal.\n\
This one, and all others in the sequel, are converted into the corresponding upper case version.",
		 sxsvar.sxtables->err_titles [1]+1 /* warning */);
      }

      break;


    case 7:
      /* On est sur EOF */
	  
      if (!we_are_in_parse_in_la) {
	/* Vrai EOF */
	/* Si cet EOF n'est pas précédé de END %EOL, on les ajoute */
	if (SXGET_TOKEN (sxplocals.Mtok_no).lahead != EOL_t ||
	    SXGET_TOKEN (sxplocals.Mtok_no-1).lahead != END_t) {
	  if (is_extension) {
	       /* on insère !END (le zombie de END) %EOL devant EOF... */
	       sxsvar.sxlv.terminal_token.lahead = END_EP_t;
	       f77put_token (sxsvar.sxlv.terminal_token);
	       sxsvar.sxlv.terminal_token.lahead = EOL_t;
	       f77put_token (sxsvar.sxlv.terminal_token);
	       sxsvar.sxlv.terminal_token.lahead = sxeof_code (sxsvar.sxtables) /* EOF_t */;
	       /* ... et on laisse le scanner ranger cet EOF */
	  }
	  /* else On laisse le traitement d'erreurs s'en occuper */
	}

	/* On verifie qu'il y a eu un MAIN program (on a deja verifie qu'il n'y en a pas
	   eu plus d'un). */
	if (is_ansi && main_program_nb == 0)
	  sxtmsg (sxsvar.sxlv.terminal_token.source_index.file_name,
		  "%sThis executable program must contain one main program.",
		  sxsvar.sxtables->err_titles [2]+1 /* Error */);

	/* vérifier ls statut des scan/parse act en la/rcvr
	   les scanact sont exécutées en parse_in_la (dont rcvr), certaines doivent être inhibées
	   (en particulier celle-ci) */
#if EBUG
	print_toks_buf (1, sxplocals.Mtok_no);
#endif
      }
      /* else on est sur le eof bidon simulant la fin du sous_langage !DO... ou !FORMAT..., on ne fait rien */

      break;

    case 8:
      /* @8 : On est sur "(",
	 si is_implicit_statement =>  @Set(1)
	 En [[ESOPE]], si SEGMENT / ... / ( => @Set(2) */
#if EBUG
      print_srcmngr_curchar ();
#endif

      if (is_implicit_statement) {
	sxsvar.sxlv_s.counters [1] = 1;
      }
#ifdef ESOPE
      else if (SXGET_TOKEN (head_tok_no).lahead == SEGMENT_t &&
	   SXGET_TOKEN (head_tok_no+1).lahead == SLASH_t &&
	   SXGET_TOKEN (sxplocals.Mtok_no).lahead == SLASH_t) {
	   sxsvar.sxlv_s.counters [2] = 1; /* @Set [2] */
      }
#endif // [[ESOPE]]
      break;

    case 9:
	/* @9 : On est sur %letter, La lettre a pu etre upgradee en mot-clef, on la retransforme en %letter */
       {
	 SXINT cur_lahead;
	 
	 if ((cur_lahead = sxsvar.sxlv.terminal_token.lahead) != LETTER_t) {
	   sxsvar.sxlv.terminal_token.lahead = LETTER_t;
	   sxsvar.sxlv.terminal_token.string_table_entry =
	     sxstrsave (sxttext (sxsvar.sxtables, cur_lahead));
	 }
       }

       break;

#ifdef ESOPE
    case 10:
	 /*
	   kw_or_dot : "." @10 id ["."&True] ; @11 ;
	   On note où on en est dans le source
	   On suppose qu'on ne change pas de buffer (ligne logique)
	 */
	 save_previous_char = sxsrcmngr.previous_char;
	 save_current_char = sxsrcmngr.current_char;
	 save_bufindex = sxsrcmngr.bufindex;
	 save_source_coord = sxsrcmngr.source_coord;

	 break;
	 
    case 11:
	 /*
	   kw_or_dot : "." @10 id ["."&True] ; @11 ;
	   Si on n'est pas sur un mot-clef, on remet le source dans l'état ou il était après la lecture du "." de gauche.
	 */

	 if (sxsvar.sxlv.terminal_token.lahead == DOT_t) {
	    /* Ce n'est pas un mot-clé */
	    /* C'est bien DOT_t, mais on change le ste, comme s'il y avait eu un "erase" dans la spécif lecl */
	    sxsvar.sxlv.terminal_token.string_table_entry = SXEMPTY_STE;

	    /* On repositionne le source_mngr après la lecture du '.' de gauche */
	    sxsrcmngr.previous_char = save_previous_char;
	    sxsrcmngr.current_char = save_current_char;
	    sxsrcmngr.bufindex = save_bufindex;
	    sxsrcmngr.source_coord = save_source_coord;
	    /* ... et on laisse l'analyse normale (re)lire les tokens suivants */
	 }
	 /* else
	    mot_clé, on ne fait rien */

#if EBUG
	 print_sxsvar ();
	 fputs ("\n", stdout);
	 print_srcmngr_curchar ();
#endif
	 
#if EBUG
	 fprintf (stdout, "****** @%ld ******\n", act_no);
	 print_toks_buf (sxplocals.Mtok_no, sxplocals.Mtok_no);
#endif

	 break;

    case 12:
	 /* On est sur un "*", on regarde si c'est le flag de l'instruction [[ESOPE]] :
	    SEGDES , <88a:variable_or_array_element_name> * [ <123c:mode> | ( <123c:mode> , <123c:mode> { , <123c:mode> } ) ]
	    avec <123c:mode> = MOD_t | MRU_t | NOMOD_t., dans ce cas, on note le tok_no de ce flag dans
	    star_flag_mode_tok_no.
	    Si ce mot-clé est dans une carte SEGDES et si son tok_no est compris dans ]head_tok_no : star_flag_mode_tok_no[
	    alors c'est un id, sinon c'est le mot clé */
	 if (SXGET_TOKEN (head_tok_no).lahead == SEGDES_t &&
	     (sxplocals.Mtok_no == head_tok_no+2 /* <88a:variable_or_array_element_name> =>+ id */ ||
	      (SXGET_TOKEN (sxplocals.Mtok_no).lahead == RIGHTP_t &&
	       SXGET_TOKEN (head_tok_no+3).lahead == LEFTP_t /* Cas SEGDES , array_name (...) * */ &&
	       is_well_balanced (head_tok_no+3, sxplocals.Mtok_no)))) {
	      star_flag_mode_tok_no = sxplocals.Mtok_no+1;
#if EBUG
	      fprintf (stdout, "++++++++++++++ @12 : \"*\" is a star_flag_mode_tok_no at tok_no = %ld ++++++++++++++\n", star_flag_mode_tok_no);
#endif
	 }
	 
	 break;
#endif // [[ESOPE]]

    default:
      fprintf (sxstderr, "\"%ld\" is an illegal action number in \"sxscanner_action\".\n", (SXINT) act_no);
      abort ();
    }
	
#if EBUG
    if (act_no != 6) {
      /* Seulement sur les post-actions */
      print_sxsvar ();
      fputs ("\n", stdout);
      print_srcmngr_curchar ();
      fprintf (stdout,
	       "**************** leaving f77_scan_act (@%ld) sxplocals.Mtok_no = %ld ****************\n",
	       (SXINT) act_no,
	       (SXINT) sxplocals.Mtok_no);
    }
#endif

    break;

  case SXPREDICATE:
#if EBUG
    fprintf (stdout,
	     "**************** entering f77_scan_act (&%ld) sxplocals.Mtok_no = %ld ****************\n",
	     (SXINT) act_no,
	     (SXINT) sxplocals.Mtok_no);
#endif
    switch (act_no) {
    case 0: {
      /* On est sur un chiffre, est-il entre les colonnes 1 a 5 ? */
      SXINT col;

      col = sxsrcmngr.source_coord.column;
	
#if EBUG
      fprintf (stdout,
	       "**************** leaving f77_scan_act (&%ld) %s  sxplocals.Mtok_no = %ld ****************\n",
	       (SXINT) act_no,
	       (col >= 1 && col <= 5) ? "true" : "false",
	       (SXINT) sxplocals.Mtok_no);
#endif
	    
      return (col >= 1) && (col <= 5);
    }

    default:
      fprintf (sxstderr, "\"%ld\" is an illegal predicate number in \"sxscanner_action\".\n", (SXINT) act_no);
      abort ();
    }
    
  default:
    fputs ("The function \"sxscanner_action\" for f77 is out of date with respect to its specification.\n", sxstderr);
    abort ();
  }
    
  return SXANY_BOOL;
}
