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

/* Decompilateur-paragrapheur de SYNTAX */

#define SXPP_STRING_INFO struct string_info
#define SXPP_SAVED_INFO  struct saved_info

#include "sxversion.h"
#include "sxunix.h"

char WHAT_SXPPP[] = "@(#)SYNTAX - $Id: sxppp.c 3558 2023-09-03 06:07:29Z garavel $" WHAT_DEBUG;

#if BUG
#define	BUG_CODE
#define	BUG_STRINGS
#define	BUG_TREE
#endif

static char	ME [] = "paragrapher";
static char	verbose_format [] = "\b\b\b\b\b\b\b%7d";

#include <ctype.h>
#include <errno.h>
#include <string.h>

#ifndef toupper
#define	toupper(c)	((c)-'a'+'A')
#endif
#ifndef tolower
#define	tolower(c)	((c)-'A'+'a')
#endif
#ifndef isascii
#define isascii(c)      (!(((int)(c)) & ~0177))
#endif

#define DFLT_TABS_INTERVAL	SXTAB_INTERVAL


#define SKIPs		1
#define PAGEs		2
#define SPACEs		3
#define TABs		4
#define MARGIN		5
#define CALL		6
#define COL		7
#define RETURN		8


#define SPACE		' '
#define HORIZONTAL_TAB	'\t'
#define NEW_LINE	'\n'
#define FORM_FEED	'\f'
#define BACKSPACE	'\b'

#if defined _WIN32
/* under windows, the end-of-line requires two characters "\r\n" */
#define NEW_LINE_BYTES        2
#else
/* under unix, the end-of-line requires one character "\n" */
#define NEW_LINE_BYTES        1
#endif


/* the stack used to save information in a recursive descent */

static struct saved_info {
	   struct sxnode_pp	*noeud /*	node being displayed		*/ ;
	   struct SXPP_schema	*CO /*		current statement pointer	*/ ;
	   SXINT	marge /*			current margin value		*/ ;
	   SXINT	marge_erreur /*			current error margin value	*/ ;
       }	saved_info;
static SXINT	save_stack_index;
static SXINT	column /* Where shall the next character lay */ ;
static SXINT	current_column /* Where we are going to write */ ;
static SXBOOLEAN	this_is_a_new_line /* There is nothing on the current line */ ;

/* Output medium */

static char	*sortie, *base_sortie;
static SXINT	dispo_sortie, taille_sortie;



static SXVOID	vider_sortie (SXBOOLEAN continuer)
{
    size_t lgth = (size_t)(taille_sortie - dispo_sortie);
  
    if (lgth != fwrite (base_sortie, sizeof (char), lgth, sxstdout)) {
        fprintf (sxstderr, "Cannot fwrite block of length %ld in file sxstdout = %s (errno = %s)\n",
		 (long) lgth, sxppvariables.logical_file_name, strerror (errno));
	sxtrap (ME, "vider_sortie");
    }

    if (continuer) {
	sxfree (base_sortie);
	base_sortie = sxalloc (taille_sortie *= 2, sizeof (*sortie));
    }

    dispo_sortie = taille_sortie;
    sortie = base_sortie;
}



static SXVOID	vider_sortie_puis_1_car (char c)
{
    /* On a failli aller un cran trop loin... */
    dispo_sortie++;
    vider_sortie (SXTRUE);
    dispo_sortie--;
    *sortie++ = c;
}




#define sortie_1_car(c)	((dispo_sortie--==0) ? (vider_sortie_puis_1_car(c),1) : ((*sortie++ = (c)),1))


static SXVOID	sortie_n_car (char *s, SXINT l)
{
    while (dispo_sortie < l) {
	SXINT	d = dispo_sortie;

	if (d == 0)
	    vider_sortie (SXTRUE);
	else {
	    sortie_n_car (s, d);
	    l -= d;
	    s += d;
	}
    }

    dispo_sortie -= l;

    while (l--) {
	*sortie++ = *s++;
    }
}


#include <stdarg.h> 
#ifdef __GNUC__
__attribute__ ((noreturn))
#endif
static SXVOID     ppabort (char *format, ...)
{
   va_list args;

   va_start (args, format);
   vfprintf (stdout, format, args);
   va_end (args);
   sxexit (SXSEVERITIES);
}


/* Information from the string manager on the current string */

static struct string_info {
	   SXINT		ste /*		string table entry	*/ ;
	   SXINT		lgth /*		string length		*/ ;
	   SXINT	width /*		first line length	*/ ;
	   short	goes_to_column /*	last line length	*/ ;
	   SXINT	lahead /*		how it was scanned	*/ ;
       }	string_info;



static SXVOID	string_modify (char *string, SXINT strl, SXCASE its_case, SXBOOLEAN must_darken)
{

#define NEEDED_CHARS(l)	(6 * l + 2)	/* l+1 + 5l+1 (normal + dark) */
#define ID_LENGTH	33		/* NEEDED_CHARS (ID_LENGTH) == 200 */

    char	ID [NEEDED_CHARS (ID_LENGTH)]
		/* Should be long enough to contain any identifier */
					     ;
    char	ch;
    char	*chp;
    char	*newchp;

    newchp = chp = strcpy (strl <= ID_LENGTH ? &(ID [0]) : sxalloc (NEEDED_CHARS (strl), sizeof (char)), string);

    switch (its_case) {
    case SXUPPER_CASE:
	for (; (ch = *chp) != SXNUL; chp++) {
	    *chp = sxtoupper (ch);
	}

	break;

    case SXLOWER_CASE:
	for (; (ch = *chp) != SXNUL; chp++) {
	    *chp = sxtolower (ch);
	}

	break;

    case SXCAPITALISED_INITIAL:
	if ((ch = *chp) != SXNUL) {
	    *chp = sxtoupper (ch);

	    while ((ch = *++chp) != SXNUL)
		*chp = sxtolower (ch);

	}

	break;

    case SXNO_SPECIAL_CASE:
	break;
    default: /* pour faire taire gcc -Wswitch-default */
#if EBUG
      sxtrap(ME,"unknown switch case #1");
#endif
      break;
    }

    chp = newchp;

    if (must_darken) {
	char	*dkp, *dark_string;

	dark_string = dkp = chp + strl + 1;

	while ((*dkp++ = ch = *chp++) != SXNUL) {
	    if (isascii (ch) && isprint (ch)) {
		*dkp++ = BACKSPACE, *dkp++ = ch;
		*dkp++ = BACKSPACE, *dkp++ = ch;
	    }
	}

	string_info.lgth = dkp - (chp = dark_string) - 1;
    }

    string_info.ste = sxstrsave (chp);

    if (strl > ID_LENGTH) {
	sxfree (newchp);
    }

#undef ID_LENGTH
#undef NEEDED_CHARS
}

/* This procedure fills the global variable "string_info" with information
   concerning the string associated with the parameters "ste" and "lahead" */

static SXVOID	string_manager (SXINT ste, SXINT lahead)
{
#ifdef BUG_STRINGS
  {
      printf ("string_manager (%ld, %ld):\t", (SXINT) ste, (SXINT) lahead);
  }
#endif

  string_info = sxppvariables.terminal_string_info [ste];

  if (string_info.lahead != lahead) {
    SXINT	chl;
    char	*chp;

    chp = sxstrget (string_info.ste = ste);
    string_info.lgth = chl = sxstrlen (ste);

#ifdef BUG_STRINGS
    {
        printf ("(First_occurrence) Generic %s = \"%s\" of length %ld =>\t", sxttext (sxppvariables.sxtables, lahead), chp, chl);
    }
#endif

    {
      char	*nlp;

      if ((nlp = sxindex (chp, NEW_LINE)) == NULL) {
	string_info.width = chl;
	string_info.goes_to_column = -1;
      }
      else {
	string_info.width = nlp - chp;
	string_info.goes_to_column = chl - (sxrindex (nlp, NEW_LINE) - chp) - 1;
      }
    }

    {
      SXCASE	its_case;
      SXBOOLEAN	must_darken;

      its_case = (sxppvariables.terminal_case != NULL ? sxppvariables.terminal_case [lahead] : SXNO_SPECIAL_CASE);
      must_darken = (sxppvariables.terminal_dark != NULL && sxppvariables.terminal_dark [lahead]);

      if (must_darken || its_case != SXNO_SPECIAL_CASE) {
	string_modify (chp, chl, its_case, must_darken);
      }
    }

    if (string_info.lahead < 0) {
      /* not seen yet: keep it */
      string_info.lahead = lahead;
      sxppvariables.terminal_string_info [ste] = string_info;
    }
  }
  
#ifdef BUG_STRINGS
  else
    printf ("(Next_occurrence) Generic ");
    
  {
     printf ("\"%s\"\n", sxstrget (string_info.ste));
  }
#endif
}




/* This procedure fills the global variable "string_info" with information
   concerning the string associated with the keyword of name "lahead" */

static SXVOID	kw_manager (SXINT lahead)
{
#ifdef BUG_STRINGS
  {
    printf ("kw_manager (%ld):\t", (SXINT) lahead);
  }
#endif

  string_info = sxppvariables.terminal_string_info [-lahead];

  if (string_info.lahead != lahead) {
    SXINT	chl;
    char	*chp;

    string_info.lgth = chl = strlen (chp = sxttext (sxppvariables.sxtables, lahead));

#ifdef BUG_STRINGS
    {
      printf ("(First_occurrence) Non-Generic \"%s\" of length %ld =>\t", chp, chl);
    }
#endif

    {
      char	*nlp;

      if ((nlp = sxindex (chp, NEW_LINE)) == NULL) {
	string_info.width = chl;
	string_info.goes_to_column = -1;
      }
      else {
	string_info.width = nlp - chp;
	string_info.goes_to_column = chl - (sxrindex (nlp, NEW_LINE) - chp) - 1;
      }
    }

    {
      SXCASE	its_case;
      SXBOOLEAN	must_darken;

      its_case = (sxppvariables.terminal_case != NULL ? sxppvariables.terminal_case [lahead] : (sxkeywordp (sxppvariables.sxtables, lahead) ? sxppvariables.kw_case : SXNO_SPECIAL_CASE));
      must_darken = (sxppvariables.terminal_dark != NULL ? sxppvariables.terminal_dark [lahead] : (sxkeywordp (sxppvariables.sxtables, lahead) && sxppvariables.kw_dark));

      if (must_darken || its_case != SXNO_SPECIAL_CASE) {
	string_modify (chp, chl, its_case, must_darken);
      }
      else {
	string_info.ste = sxstrsave (chp);
      }
    }

    sxppvariables.terminal_string_info [-(string_info.lahead = lahead)] = string_info;
  }

#ifdef BUG_STRINGS
  else
    printf ("(Next_occurrence) Non-Generic ");

  {
    printf ("\"%s\"\n", sxstrget (string_info.ste));
  }
#endif
}



static SXVOID	call_put_error (void)
{
    struct sxsource_coord	source_index;
    static char		severity [2] = {SXSEVERITIES - 1, 0};

    source_index = saved_info.noeud->terminal_token.source_index;

    if (saved_info.noeud->name != 0) {
	struct SXPP_schema	*co;
	SXINT	rd;

	if (saved_info.noeud->name > 0) {
	    /* non-terminal node */
	    rd = saved_info.noeud->name;
	    co = saved_info.CO;
	}
	else {
	    /* terminal: use information on the father */
	    rd = sxppvariables.save_stack [save_stack_index].noeud->name;
	    co = sxppvariables.save_stack [save_stack_index].CO;
	}

	sxerror (source_index, 
		 severity[0],
		 "Error on margin in directive %d of production %ld.", 
		 co - sxppvariables.PP_tables [rd] + 1, 
		 (SXINT) rd);
    }
    else
	sxerror (source_index, severity[0], "Paragraphing cannot be done left of first column.");

    sxppvariables.is_error = SXTRUE;
    column = 0;
}



static SXVOID	go_to_column (void)
{
    SXINT	nb_spaces;
    SXINT	nb_tabs;

    if (column - 1 == current_column) {
	/* On privilegie le cas le plus frequent */
	sortie_1_car (SPACE);
	sxppvariables.char_count += NEW_LINE_BYTES;
    }
    else {
	if (current_column > column) {
	    sortie_1_car (NEW_LINE);
	    sxppvariables.char_count++;
	    current_column = 0;
	    this_is_a_new_line = SXTRUE;

	    if (sxverbosep) {
		fprintf (sxtty, verbose_format, sxppvariables.char_count);
	    }
	}

	if (column > current_column) {
	    if ((nb_tabs = (sxppvariables.no_tabs ? 0 : (column / sxppvariables.tabs_interval - (current_column / sxppvariables.tabs_interval)))) > 0) {
		sxppvariables.char_count += nb_tabs;

		do
		    sortie_1_car (HORIZONTAL_TAB);
		while (--nb_tabs > 0);

		nb_spaces = column % sxppvariables.tabs_interval;
	    }
	    else
		nb_spaces = column - current_column;

	    if (nb_spaces > 0) {
		sxppvariables.char_count += nb_spaces;

		do
		    sortie_1_car (SPACE);
		while (--nb_spaces > 0);
	    }
	}
    }

    current_column = column;
}



static SXVOID	cadrage (SXINT taille)
{
    if (taille > sxppvariables.line_length)
	column = 0;
    else if ((column = sxppvariables.line_length - taille) > saved_info.marge_erreur)
	column = saved_info.marge_erreur;
}



static SXVOID	impr_commentaire (char *ch_ptr)
{
    SXINT	lg, longueur_premiere_ligne /* 0 si lg */ , depart;
    SXINT		nb_NEW_LINEs, nb_NEW_LINEs_en_fin;
    SXINT		sv_column;
    SXBOOLEAN	sv_this_is_a_new_line;

    if (column < 0)
	call_put_error ();


/* Initialisations */

    lg = strlen (ch_ptr);

    {
	char	ch;
	char	*nlp;

	longueur_premiere_ligne = 0;
	nb_NEW_LINEs = 0;

	for (nlp = ch_ptr; (ch = *nlp) != SXNUL; nlp++)
	    if (ch == NEW_LINE || ch == FORM_FEED) {
		if ((longueur_premiere_ligne = nlp - ch_ptr + 1) == 1) {
		    do
			nb_NEW_LINEs++;
		    while ((ch = *++nlp) != SXNUL && (ch == NEW_LINE || ch == FORM_FEED));

		    if (ch == SXNUL)
			nb_NEW_LINEs = -1;
		}

		break;
	    }
    }

    sv_column = column;
    sv_this_is_a_new_line = this_is_a_new_line;

#ifdef BUG_STRINGS
    printf ("(impr_commentaire): on_a_NL = %s, column = %ld, nb_NEW_LINEs = %ld, comment(%ld) = \"%s\"",
	     this_is_a_new_line ? "TRUE" : "FALSE",
	     column,
	     nb_NEW_LINEs,
	     lg,
	     ch_ptr);
#endif

#ifdef USERS_COMMENTS
    /* Le contenu des commentaires n'est pas touché (il est entièrement géré par l'utilisateur)
       Il est sorti colonne 1 */
    nb_NEW_LINEs = -2;
#endif
      
/* Positionnement du commentaire */

    if (sxppvariables.char_count == 0 && nb_NEW_LINEs != -2) {
	/* Commentaire en debut de fichier */
	if ((depart = nb_NEW_LINEs - 2) < 0)
	    /* On prend au plus un NL */
	    depart = 0;

	nb_NEW_LINEs = 2 /* Pour le repositionnement */ ;
	column = current_column = 0 /* Pas de deplacement inutile */ ;
    }
    else
	switch (nb_NEW_LINEs) {
	case -2:
	  depart = 0; /* on prend tout le commentaire... */
	  
	  if (!this_is_a_new_line) {
	    sortie_1_car (NEW_LINE);
	    sxppvariables.char_count += NEW_LINE_BYTES;
	  }
	  
	  column = current_column = 0  /*  ... sur la ligne suivante */ ;
	  break;

	case -1:
	    /* Commentaire ne contenant que des NL et FF */
	    if (sxindex (ch_ptr, FORM_FEED) != NULL) {
		if (!this_is_a_new_line) {
		    sortie_1_car (NEW_LINE);
		    sxppvariables.char_count += NEW_LINE_BYTES;
		}

		sortie_1_car (FORM_FEED);
		sxppvariables.char_count++;
		current_column = sxppvariables.line_length /* On ira a la ligne */ ;
		this_is_a_new_line = SXFALSE;
	    }


/* Desole, mais il n'y a rien d'autre a faire... */

	    return;

	case 0:
	    /* Le commentaire est dans la foulee */
	    depart = 0 /* On prend tout le commentaire */ ;

	    if ((longueur_premiere_ligne != 0) && (longueur_premiere_ligne != lg))
		/* Il prend plus d'une ligne */
		goto a_la_marge;

	    if (!this_is_a_new_line && (column == current_column))
		column++ /* Un blanc avant le commentaire */ ;

	    break;

	case 1:
	    /* Le commentaire commence une ligne */
	    depart = 1 /* On ne prend pas le NL de debut */ ;

a_la_marge:
	    /* positionnement sur la marge dans une ligne vide */
	    if (!this_is_a_new_line) {
		current_column = sxppvariables.line_length;
		column = saved_info.marge;
	    }

	    {
		char	ch;
		char	*spp;


/* Suppression des espaces de tete */

		for (spp = &(ch_ptr [depart]); (ch = *spp) != SXNUL; spp++)
		    if (ch != HORIZONTAL_TAB && ch != SPACE)
			break;
		    else
			depart++;
	    }

	    if (longueur_premiere_ligne != 0) {
		char	*nlp;
		char	ch;

		longueur_premiere_ligne = 0;

		for (nlp = &(ch_ptr [depart]); (ch = *nlp) != SXNUL; nlp++)
		    if (ch == NEW_LINE || ch == FORM_FEED) {
			longueur_premiere_ligne = nlp - ch_ptr + 1;
			break;
		    }
	    }

	    break;

	default:
	    /* Commentaire en colonne 1 */
	    if (this_is_a_new_line)
		depart = nb_NEW_LINEs - 1; /* On ne prend qu'un NL */
	    else
		depart = 0 /* Il faut aller a la ligne */ ;

	    column = current_column = 0 /* Pas de deplacement inutile */ ;
	    break;
	}


/* On verifie que le commentaire tient sur la ligne */

    if (column + longueur_premiere_ligne > sxppvariables.line_length)
	cadrage (longueur_premiere_ligne);
    else if ((longueur_premiere_ligne == 0) && (column + lg > sxppvariables.line_length))
	cadrage (lg);


/* Ecriture du commentaire */

    if (current_column != column)
	go_to_column ();

    sortie_n_car (&(ch_ptr [depart]), lg - depart);
    sxppvariables.char_count += lg - depart;

    {
	char	*nlp;
	char	ch;

	nb_NEW_LINEs_en_fin = 0;

	for (nlp = &(ch_ptr [lg - 1]); nlp >= ch_ptr;)
	    if (((ch = *nlp--) != NEW_LINE && ch != FORM_FEED) || ++nb_NEW_LINEs_en_fin == 2)
		break;
    }


/* Repositionnement apres ecriture */

    switch (nb_NEW_LINEs) {
    case -2:
      /* User's comment */
      column = sv_column;
      break;

    case 0:
	/* Le commentaire est dans la foulee */
	if (longueur_premiere_ligne == lg) {
	    /* Style Ada ou new_10cl */
	    current_column = 0;
	    column = sv_column;
	}
	else {
	    if (sv_this_is_a_new_line || (longueur_premiere_ligne != 0))
		/* On ne l'a pas sorti dans la foulee */
		goto a_la_colonne;

	    current_column = column + lg - depart /* Tant pis pour les HT et autres BS */ ;
	    column = current_column + 1 /* Un blanc apres */ ;
	}

	break;

    case 1:
/* Le commentaire commence une ligne */
a_la_colonne:
	/* Positionnement sur la colonne originelle */
	column = sv_column;

	if (nb_NEW_LINEs_en_fin == 0)
	    current_column = sxppvariables.line_length; /* On ira a la ligne */
	else
	    current_column = 0;

	break;

    default:
	/* Commentaire en colonne 1 */
	column = sv_column;
	current_column = 0;

	if ((nb_NEW_LINEs_en_fin = 2 - nb_NEW_LINEs_en_fin /* Nombre de NL a ajouter */ ) > 0) {
	  sxppvariables.char_count += (nb_NEW_LINEs_en_fin * NEW_LINE_BYTES);

	    do
		sortie_1_car (NEW_LINE);
	    while (--nb_NEW_LINEs_en_fin > 0);
	}

	break;
    }

    this_is_a_new_line = current_column == 0;
    /* sxfree (ch_ptr); Les commentaires sont supprimes maintenant par un appel ulterieur a sxcomment_mngr */
}




#define impr_key_word(kw)		(kw_manager (kw), impr ())
#define impr_token(ste, la)		(string_manager (ste, la), impr ())


/* This procedure prints the string pointed to by "string_info" */

static SXVOID	impr (void)
{
  /* initialise:
     (pas de goto sur ce label, ce qui provoque un warning du compilo icc d'intel) */

    {
	SXINT	column_min;

	if (column < 0)
	    call_put_error ();

	if (column > (column_min = sxppvariables.line_length - string_info.width))
	    column = saved_info.marge_erreur > column_min ? (0 >= column_min ? 0 : column_min) : saved_info.marge_erreur;

	if (current_column != column)
	    go_to_column ();
    }

    /* copy:
     (pas de goto sur ce label, ce qui provoque un warning du compilo icc d'intel) */

    {
	sxppvariables.char_count += string_info.lgth;
	sortie_n_car (sxstrget (string_info.ste), string_info.lgth);
    }

    /* finalise:
     (pas de goto sur ce label, ce qui provoque un warning du compilo icc d'intel) */

    {
	if (string_info.goes_to_column == -1) {
	    current_column = column += string_info.width;
	    this_is_a_new_line = SXFALSE;
	}
	else {
	    current_column = column = string_info.goes_to_column;
	    this_is_a_new_line = (string_info.goes_to_column == 0);
	}
    }
}




#ifdef BUG_TREE
static SXVOID	sxppnp (struct sxnode_pp *node)
{
    if (node != NULL) {
	printf ("Node : cur = %lX (la = %ld)\tnext = %lX\n\t",
		(SXUINT) node, node->terminal_token.lahead, (SXUINT) node->next);

	if (node->name > 0) {
	    printf ("Prod = %ld", node->name);
	}
	else if (node->name == 0) {
	    printf ("ERROR SXNODE");
	}
	else {
	    printf ("ste = %s (%s)", sxstrget (node->terminal_token.string_table_entry), sxttext (sxppvariables.sxtables, -node->name));
	}

	printf ("\t(line = %ld, column = %ld)",
		node->terminal_token.source_index.line,
	  	node->terminal_token.source_index.column);

	if (node->terminal_token.comment != NULL) {
	    printf ("\n\tcomment = \"%s\"", node->terminal_token.comment);
	}

	if (node->post_comment != NULL) {
	    printf ("\n\tpost_comment = \"%s\"", node->post_comment);
	}
    }
    else
	puts ("Node : NULL");

    puts ("\n");
}
#endif


static SXVOID	paragrapheur (struct sxnode_pp *root_ptr)
{
    struct SXPP_schema	directive /* current directive */ ;
    struct sxnode_pp	*node_ptr /* last node having been displayed */ ;
    SXBOOLEAN	inhibit_CR_FF /* Next CR must be inhibited */ ;
    static char		consistency_msg [] = "Your tables may be inconsistent...";


/* Initialisations */

    save_stack_index = 0;
    saved_info.noeud = node_ptr = root_ptr;
    saved_info.marge = 0;
    saved_info.marge_erreur = 5;
    current_column = column = 0;
    this_is_a_new_line = SXTRUE;
    inhibit_CR_FF = SXFALSE;
    sortie = base_sortie = sxalloc (dispo_sortie = taille_sortie = 256 * BUFSIZ, sizeof (*sortie));

#ifdef BUG_TREE

    printf ("\nHere is the tree:\n");

    {
	struct sxnode_pp	*node;

	node = root_ptr;

	do {
	    sxppnp (node);
	} while ((node = node->next));
    }


#endif

next_sub_tree:
    if (saved_info.noeud == NULL) {
	/* something nasty happened somewhere */
	ppabort ("%s: unexpected NULL pointer.  %s\n", ME, consistency_msg);
    }

    if (saved_info.noeud->terminal_token.comment != 0)
	impr_commentaire (saved_info.noeud->terminal_token.comment);

    /* switch_on_name:
     (pas de goto sur ce label, ce qui provoque un warning du compilo icc d'intel) */
    {
	SXINT	node_name;

	if ((node_name = saved_info.noeud->name) > 0) {
	    /* Non terminal */
	    saved_info.CO = sxppvariables.PP_tables [node_name];
	    goto decode_directive;
	}

	if (node_name < 0)
	    /* Terminal generique */
	    impr_token (saved_info.noeud->terminal_token.string_table_entry, -node_name);
	else
	    /* Noeud erreur */
	    sxppvariables.is_error = SXTRUE;

	goto end_sub_tree;
    }

check_column:
    /* On verifie la colonne: si l'on est revenu trop en arriere, on force un
       retour-ligne */
    if (column < current_column) {
	SXINT	temp;

	temp = column, column = 0, go_to_column (), column = temp;
    }

next_op:
    saved_info.CO++ /* fetch next directive */ ;
decode_directive:
    directive = *(saved_info.CO);


#ifdef BUG_CODE

    {
	static char	*statements [] = {"???", "SKIP", "PAGE", "SPACE", "TAB", "MARGIN", "CALL", "COL", "RETURN", "????"};

	printf ("directive #%ld of production %ld:\t", (SXINT) (saved_info.CO - sxppvariables.PP_tables [saved_info.noeud->name] + 1), saved_info.noeud->name);
	printf ("%s\t%ld\n", statements [directive.PP_codop], directive.PP_act);
    }


#endif

    switch (directive.PP_codop) {
    default:
	ppabort ("%s: illegal directive found\n", ME);

    case SKIPs:
	if (inhibit_CR_FF)
	    inhibit_CR_FF = SXFALSE;
	else {
	    SXINT	nb;

	    sxppvariables.char_count += nb = directive.PP_act;

	    while (nb-- > 0)
		sortie_1_car (NEW_LINE);

	    current_column = column = 0;
	    this_is_a_new_line = SXTRUE;

	    if (sxverbosep) {
		fprintf (sxtty, verbose_format, sxppvariables.char_count);
	    }
	}

	goto next_op;

    case PAGEs:
	if (inhibit_CR_FF)
	    inhibit_CR_FF = SXFALSE;
	else {
	    SXINT	nb;

	    sxppvariables.char_count += nb = directive.PP_act;

	    while (nb-- > 0)
		sortie_1_car (FORM_FEED);

	    current_column = column = 0;
	    this_is_a_new_line = SXTRUE;
	}

	goto next_op;

    case SPACEs:
	column += directive.PP_act;
	inhibit_CR_FF = SXFALSE;
	goto check_column;

    case TABs:
	column = (column / sxppvariables.tabs_interval + directive.PP_act) * sxppvariables.tabs_interval;
	inhibit_CR_FF = SXFALSE;
	goto next_op;

    case MARGIN:
	/* Positionnement par rapport a la marge */
	if ((column = saved_info.marge + directive.PP_act) < 0)
	    call_put_error ();
	else if (column > sxppvariables.max_margin)
	    column = sxppvariables.block_margin ? sxppvariables.max_margin : (column % sxppvariables.max_margin);

	inhibit_CR_FF = SXFALSE;
	goto check_column;

    case CALL:
	/* Terminal ou non-terminal */
	inhibit_CR_FF = SXFALSE;

	{
	    SXINT	act;

	    if ((act = directive.PP_act) != 0) {
		/* Terminal non generique */
		impr_key_word (act);
		goto next_op;
	    }
	}

	if (++save_stack_index == sxppvariables.save_stack_length)
	    sxppvariables.save_stack = (struct saved_info*) sxrealloc (sxppvariables.save_stack, sxppvariables.save_stack_length *= 2, sizeof (struct saved_info));

	sxppvariables.save_stack [save_stack_index] = saved_info;
	/* PUSH (noeud, reduc, co, marge, marge_erreur); */
	saved_info.noeud = node_ptr = node_ptr->next;
	saved_info.marge = column;

	if (this_is_a_new_line)
	    saved_info.marge_erreur = column + 5;

	goto next_sub_tree /* Terminal generique ou non-terminal */ ;

    case COL:
	/* Positionnement absolu */
	column = directive.PP_act - 1;
	goto check_column;

    case RETURN /* [INH] */ :
	if (!inhibit_CR_FF)
	    inhibit_CR_FF = (directive.PP_act == 1);

	goto end_sub_tree;
    }

end_sub_tree:
    if (saved_info.noeud->post_comment != 0)
	impr_commentaire (saved_info.noeud->post_comment);

    if (save_stack_index != 0) {
	/* must work more */
	saved_info = sxppvariables.save_stack [save_stack_index--];
	/* POP (noeud, reduc, co, marge, marge_erreur); */
	goto next_op;
    }

    if (saved_info.noeud != root_ptr || node_ptr->next != NULL) {
	ppabort ("%s: Expected NULL pointer not found.  %s\n", ME, consistency_msg);
    }


/* On finit en beaute */

    go_to_column ();

    if (sxverbosep) {
	fprintf (sxtty, verbose_format, sxppvariables.char_count);
    }

    vider_sortie (SXFALSE);
    sxfree (base_sortie);
}



static SXVOID	sxppopen (struct sxtables *sxtables)
{
    if (sxppvariables.line_length <= 1)
	sxppvariables.line_length = 79;

    if (sxppvariables.tabs_interval <= 0)
	sxppvariables.tabs_interval = DFLT_TABS_INTERVAL;

    if (sxppvariables.max_margin > sxppvariables.line_length)
	sxhmsg (sxsrcmngr.source_coord.file_name, "%s%s: incorrect \"max_margin\". Value taken: %d.", sxtables->err_titles [0]+1, ME, sxppvariables.max_margin = (sxppvariables.line_length * 2) / 3);
    else if (sxppvariables.max_margin <= 0)
	sxppvariables.max_margin = (sxppvariables.line_length * 2) / 3;

    if (sxppvariables.tabs_interval > sxppvariables.line_length)
	sxhmsg (sxsrcmngr.source_coord.file_name, "%s%s: incorrect tabulation interval. Value taken: %d.", sxtables->err_titles [0]+1, ME, sxppvariables.tabs_interval = (DFLT_TABS_INTERVAL > sxppvariables.max_margin ? sxppvariables.max_margin : DFLT_TABS_INTERVAL));

    sxppvariables.tmax = sxeof_code (sxtables);
    sxppvariables.terminal_string_info = (struct string_info*) sxcalloc ((SXUINT)(sxppvariables.tsi_top = 100) + sxppvariables.tmax, sizeof (struct string_info)) + sxppvariables.tmax;
    sxppvariables.save_stack = (struct saved_info*) sxalloc (sxppvariables.save_stack_length = 100, sizeof (struct saved_info));

    {
	SXINT	key;
	struct string_info	*tsi;

	for (tsi = &(sxppvariables.terminal_string_info [key = -sxppvariables.tmax]); key < 0; key++) {
	    tsi++->lahead = -1;
	}
    }

    sxppvariables.PP_tables = (struct SXPP_schema**) sxtables->sem_tables;
    sxppvariables.sxtables = sxtables;


#ifdef BUG_STRINGS

    {
	printf ("kw_case: %d, kw_dark: %d\n", sxppvariables.kw_case, sxppvariables.kw_dark);
    }




#endif

}



static SXVOID	sxppclose (void)
{
    if (sxppvariables.save_stack != NULL) {
	sxfree (sxppvariables.save_stack), sxppvariables.save_stack = NULL;
    }

    if (sxppvariables.terminal_string_info != NULL) {
	sxfree (sxppvariables.terminal_string_info - sxppvariables.tmax), sxppvariables.terminal_string_info = NULL;
    }
}

SXVOID	sxppp (SXINT sxppp_what, struct sxtables *sxtables)
{
  switch (sxppp_what) {
  case SXOPEN:
    sxcheck_magic_number (sxtables->magic, SXMAGIC_NUMBER, ME);
    sxppopen (sxtables);
    break;

  case SXCLOSE:
    sxppclose ();
    break;

  case SXACTION:
    sxppvariables.char_count = 0;
    sxppvariables.is_error = SXFALSE;

    {
      SXINT	old_top = sxppvariables.tsi_top;

      if ((sxppvariables.tsi_top = sxstrmngr.top) > old_top) {
	sxppvariables.terminal_string_info = (struct string_info*) sxrealloc (sxppvariables.terminal_string_info - sxppvariables.tmax, sxppvariables.tsi_top + sxppvariables.tmax, sizeof (struct string_info)) + sxppvariables.tmax;
      }
    }

    {
      SXINT	ste;
      struct string_info		*tsi;

      for (tsi = &(sxppvariables.terminal_string_info [ste = sxppvariables.tsi_top - 1]); ste >= 0; ste--) {
	tsi--->lahead = -1;
      }
    }

    if (sxverbosep) {
      fprintf (sxtty, "char count:%7d", 0);
    }

    {
      /* Pierre le 13/03/2023 */
      off_t cur_pos_in_sxstdout;

      if (sxstdout != stdout) {
	/* Ce qui est le cas standard : le ppXXX_main.c a lié sxstdout au fichier sxppvariables.logical_file_name */
	if ((cur_pos_in_sxstdout = ftello (sxstdout)) != 0) {
	  /* Non vide, (le debug de) syntax a deja mis quelque chose dans sxppvariables.logical_file_name */
	  fprintf (sxstderr, "sxppp: %s contient %lld octets de traces/debug.\n",
		   sxppvariables.logical_file_name,
		   (long long int) cur_pos_in_sxstdout);
	  /* On le ferme ... */
	  fclose (sxstdout);
	  /* On crée un nouveau temporaire ... */
	  strcpy (sxppvariables.logical_file_name,"/tmp/sxpppXXXXXX"); 

	  if ((sxstdout = sxnewfile (sxppvariables.logical_file_name)) == NULL) {
	    /* Echec, on va sortir la version pp logique sur stdout */
	    sxstdout = stdout;
	  }
	}
	/* else sxstdout est vide (cas usuel), on l'utilise pour y mettre la version logique de pp */
      }
      /* else sxstdout == stdout => on ne fait rien */
    }

#if EBUG
    fprintf (sxstderr, "sxppp: the logical pp version will be on %s.\n",
	     (sxstdout != stdout) ? sxppvariables.logical_file_name : "stdout");
#endif
	  
    paragrapheur ((struct sxnode_pp*) sxtables);

    if (sxstdout != stdout) {
      long real_size;

      if (fflush (sxstdout) != 0) {
        fprintf (sxstderr, "Cannot fflush the file sxstdout = %s (errno = %s)\n",
		 sxppvariables.logical_file_name, strerror (errno));
	sxppvariables.is_error = SXTRUE;
      }
      else if ((real_size = ftell (sxstdout)) != (long)sxppvariables.char_count) {
	fprintf (sxstderr, "%s : Only %ld bytes have been written in the file sxstdout = %s (instead of %ld).\n",
		 ME,
		 real_size,
		 sxppvariables.logical_file_name,
		 (long) sxppvariables.char_count);
	sxppvariables.is_error = SXTRUE;
      }	
    }

    if (sxverbosep) {
      fputc ('\n', sxtty);
    }

    break;

  default:
    fprintf (sxstderr, "The function \"sxppp\" is out of date with respect to its specification.\n");
    sxexit(1);
  }
}
