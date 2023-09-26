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
   *  Produit de l'equipe Langages et Traducteurs.  (phd) *
   *                                                      *
   ******************************************************** */




/* Decompilateur-paragrapheur de SYNTAX */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030514 13:34 (phd):	Adaptation à SGI (64 bits)		*/
/************************************************************************/
/* 19991201 13:25 (phd):	Utilisation de sxeof_code		*/
/************************************************************************/
/* 19991019 14:18 (phd):	Adaptation à Windows+Cygnus		*/
/************************************************************************/
/* Lun  4 Oct 1999 14:19(pb):	Utilisation de sxto(lower|upper)	*/
/************************************************************************/
/* Ven 22 Nov 1996 17:07(pb):	Adaptation a LINUX			*/
/************************************************************************/
/* 21-10-93 13:00 (phd&pb):	Adaptation a OSF			*/
/************************************************************************/
/* 24-05-88 16:06 (phd):	Colonne 1 <==> column == 0		*/
/************************************************************************/
/* 25-04-88 10:18 (phd):	Ecriture sur stdout pour efficacite	*/
/************************************************************************/
/* 21-04-88 18:12 (phd):	Ecriture des chaines contenant des '\0'	*/
/************************************************************************/
/* 12-04-88 11:03 (phd) :	Correction du test de debut de fichier	*/
/************************************************************************/
/* 28-03-88 16:18 (phd) :	Utilisation de sxindex et sxrindex	*/
/************************************************************************/
/* 03-12-87 14:55 (phd) :	Ajout de macros pour plaire a lint	*/
/************************************************************************/
/* 30-11-87 14:05 (phd) :	Suppression de "post_source_index"	*/
/************************************************************************/
/* 24-11-87 18:22 (phd) :	Correction d'une bogue sur "-ll 1"	*/
/************************************************************************/
/* 19-11-87 10:35 (phd) :	Renommage node_pp => sxnode_pp		*/
/************************************************************************/
/* 25-05-87 11:03 (phd) :	Ajout d'un message d'erreur		*/
/************************************************************************/
/* 03-03-87 17:10 (phd) :	Mode verbose plus fin			*/
/************************************************************************/
/* 17-07-87 16:29 (phd) :	Amelioration du "BUG_TREE"		*/
/************************************************************************/
/* 17-07-87 12:22 (phd) :	Correction de "ppabort"			*/
/************************************************************************/
/* 13-07-87 12:36 (phd) :	Messages d'erreur en patois		*/
/************************************************************************/
/* 10-12-86 10:56 (phd) :	Modification des messages d'erreur	*/
/************************************************************************/
/* 04-12-86 10:06 (phd) :	Utilisation de TAB_INTERVAL. Correction	*/
/*				d'une bogue a l'initialisation		*/
/************************************************************************/
/* 03-12-86 17:56 (phd) :	Liberation des chaines de commentaire	*/
/************************************************************************/
/* 01-12-86 11:12 (pb&phd) :	Appel de sxcheck_magic_number		*/
/************************************************************************/
/* 23-10-86 12:12 (phd):	Amelioration des options d'ecriture	*/
/************************************************************************/
/* 21-10-86 10:46 (phd):	Diminution des allocations de chaines	*/
/************************************************************************/
/* 20-10-86 18:03 (phd):	Refonte de la manipulation des chaines	*/
/************************************************************************/
/* 17-10-86 15:23 (phd):	Ajout du mode verbeux			*/
/************************************************************************/
/* 17-10-86 13:53 (phd):	Renommage de la fonction exportee	*/
/************************************************************************/
/* 17-10-86 11:32 (phd):	Macros de mise au point			*/
/************************************************************************/
/* 15-10-86 16:15 (phd):	Adaptation au monde SYNTAX-UNIX		*/
/************************************************************************/
/* 13-10-86 17:45 (phd):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/
#define SXPP_STRING_INFO struct string_info
#define SXPP_SAVED_INFO  struct saved_info

#include "sxunix.h"

char WHAT_SXPPP[] = "@(#)SYNTAX - $Id: sxppp.c 1211 2008-03-13 11:12:11Z rlacroix $" WHAT_DEBUG;

/*************************************************************/
/*                                                           */
/*                                                           */
/*  This program has been translated from paragrapheur.pl1   */
/*  on Monday the sixteenth of December, 1985, at 16:43:44,  */
/*  on the Multics system at INRIA,                          */
/*  by the release 3.3.d PL1_C translator of INRIA,          */
/*         developped by the "Langages et Traducteurs" team, */
/*         using the SYNTAX (*), FNC and Paradis systems.    */
/*                                                           */
/*                                                           */
/*************************************************************/
/* (*) SYNTAX is a trademark of INRIA.                       */
/*************************************************************/


/*  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  * */
/*											  */
/* Historique des modifications depuis le 20 Decembre 1983 (ordre chronologique inverse): */
/*											  */
/*											  */
/* 5 Mars 1986: (4.6a)									  */
/* Utilisation de "com_err_" plutot que de "put_error" pour sortir un message a		  */
/* l'initialisation.									  */
/*											  */
/* 10 Decembre 1985: (4.6)								  */
/* Implantation de l'option "-output_file" ("-of"), qui utilise la "convention egale".	  */
/* Verification de la coherence entre les tables et la version courante du paragrapheur.  */
/*											  */
/* 5 Decembre 1985: (4.5)								  */
/* Adaptation a la version de SYNTAX necessitant des entrees "open", "close" et autres.   */
/*											  */
/* 16 Septembre 1985: (4.4turbo)							  */
/* L'arbre sur lequel on travaille est linearise: il n'y a plus qu'un seul champ "next"   */
/* qui remplace les deux champs precedents "son" et "brother".				  */
/*											  */
/* 30 Janvier 1985: (4.3gtx)								  */
/* Ajout de l'option "-every N" qui permet de definir l'intervalle entre deux positions   */
/* de tabulation.  Utile pour pretty-printer vers UN*X.					  */
/*											  */
/* 11 Avril 1984: (4.2gtx)								  */
/* Correction d'un malentendu sur la definition de ce qu'est un mot cle.		  */
/*											  */
/* 20 Mars 1984: (4.2gti)								  */
/* Correction d'une imprecision qui provoquait une erreur dans le paragrapheur lorsque le */
/* texte source etait errone, et que le parser inserait un identificateur sans texte.	  */
/*											  */
/* 20 Mars 1984: (4.2gt)								  */
/* Nouvelle version des noeuds: le champ "father" est supprime.  Le dernier fils d'un     */
/* noeud a maintenant son pere comme "brother"!!!					  */
/* En consequence, suppression de l'ancienne version du paragrapheur, qui commencait a	  */
/* devenir lourde a maintenir!								  */
/*											  */
/* 19 Mars 1984: (4.2)									  */
/* Ajout des options "-dark" et "-keywords_in_dark".  Modification du traitement des	  */
/* options relatives a "capitalise_initial", pour mettre en majuscule la premiere lettre  */
/* de chaque mot, et non plus le premier caractere de l'unite.				  */
/*											  */
/* 16 Mars 1984: (4.1)									  */
/* Remplacement de la procedure recursive "internal_subtree" par la "quick-procedure"	  */
/* "pretty_printer" (gain en temps d'execution: plus de 30%...).			  */
/*											  */
/* 13 Mars 1984: (3.4)									  */
/* Modifications dans le traitement des options traitant de la casse des mots-cles et	  */
/* des terminaux, pour ameliorer la vitesse.						  */
/* Ajout de l'option "tabs", qui commande l'utilisation du caractere "HT" chaque fois que */
/* cela diminue la taille du programme produit.						  */
/*											  */
/* 20 Decembre 1983: (3.3 et 2.7)							  */
/* Suppression du champ "lahead" dans les noeuds, qui est remplace par une valeur	  */
/* negative du champ "name".  La valeur nulle de ce champ sert a indiquer un noeud	  */
/* erreur.										  */
/*  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  * */


#if	BUG
#	define	BUG_CODE
#	define	BUG_STRINGS
#	define	BUG_TREE
#endif


static char	ME [] = "paragrapher";
static char	verbose_format [] = "\b\b\b\b\b\b\b%7d";


#include <ctype.h>

#ifndef toupper
#define	toupper(c)	((c)-'a'+'A')
#endif
#ifndef tolower
#define	tolower(c)	((c)-'A'+'a')
#endif


#define DFLT_TABS_INTERVAL	TAB_INTERVAL


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
static BOOLEAN	this_is_a_new_line /* There is nothing on the current line */ ;

/* Output medium */

static char	*sortie, *base_sortie;
static long	dispo_sortie, taille_sortie;



static VOID	vider_sortie (BOOLEAN continuer)
{
    fwrite (base_sortie, sizeof (char), (size_t)(taille_sortie - dispo_sortie), sxstdout);

    if (continuer) {
	sxfree (base_sortie);
	base_sortie = sxalloc (taille_sortie *= 2, sizeof (*sortie));
    }

    dispo_sortie = taille_sortie;
    sortie = base_sortie;
}



static VOID	vider_sortie_puis_1_car (char c)
{
    /* On a failli aller un cran trop loin... */
    dispo_sortie++;
    vider_sortie (TRUE);
    dispo_sortie--;
    *sortie++ = c;
}




#define sortie_1_car(c)	((dispo_sortie--==0) ? (vider_sortie_puis_1_car(c),1) : ((*sortie++ = (c)),1))


static VOID	sortie_n_car (char *s, long l)
{
    while (dispo_sortie < l) {
	long	d = dispo_sortie;

	if (d == 0)
	    vider_sortie (TRUE);
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


#if 0 /* remplacé par la version proposée par VASY, avec utilisation de va_list, va_start et va_end grâce à #include <stdarg.h> */
#ifndef __USE_POSIX2
/*VARARGS1*/

static VOID	ppabort (char *format, char *arg1, char *arg2, char *arg3)
{
    fprintf (sxstderr, format, arg1, arg2, arg3);
    SXEXIT (SEVERITIES);
}
#else
# define ppabort(...)	do {fprintf (sxstderr, __VA_ARGS__); SXEXIT (SEVERITIES);} while (0)
#endif
#endif /* 0 */

// version VASY
#include <stdarg.h> 
static VOID     ppabort (char *format, ...)
{
   va_list args;

   va_start (args, format);
   vfprintf (stdout, format, args);
   va_end (args);
   SXEXIT (SEVERITIES);
}


/* Information from the string manager on the current string */

static struct string_info {
	   SXUINT		ste /*		string table entry	*/ ;
	   long		lgth /*			string length		*/ ;
	   SXINT	width /*		first line length	*/ ;
	   short	goes_to_column /*	last line length	*/ ;
	   SXINT	lahead /*		how it was scanned	*/ ;
       }	string_info;



static VOID	string_modify (char *string, long strl, CASE its_case, BOOLEAN must_darken)
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
    case UPPER_CASE:
	for (; (ch = *chp) != NUL; chp++) {
	    *chp = sxtoupper (ch);
	}

	break;

    case LOWER_CASE:
	for (; (ch = *chp) != NUL; chp++) {
	    *chp = sxtolower (ch);
	}

	break;

    case CAPITALISED_INITIAL:
	if ((ch = *chp) != NUL) {
	    *chp = sxtoupper (ch);

	    while ((ch = *++chp) != NUL)
		*chp = sxtolower (ch);

	}

	break;

    case NO_SPECIAL_CASE:
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

	while ((*dkp++ = ch = *chp++) != NUL) {
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

static VOID	string_manager (SXUINT ste, SXINT lahead)
{

#ifdef	BUG_STRINGS

    {
	printf ("string_manager (%lu, %ld):\t", ste, lahead);
    }


#endif

    string_info = sxppvariables.terminal_string_info [ste];

    if (string_info.lahead != lahead) {
	long	chl;
	char	*chp;

	chp = sxstrget (string_info.ste = ste);
	string_info.lgth = chl = sxstrlen (ste);


#ifdef	BUG_STRINGS

	{
	    printf ("Generic \"%s\" of length %ld =>\t", chp, chl);
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
	    CASE	its_case;
	    BOOLEAN	must_darken;

	    its_case = (sxppvariables.terminal_case != NULL ? sxppvariables.terminal_case [lahead] : NO_SPECIAL_CASE);
	    must_darken = (sxppvariables.terminal_dark != NULL && sxppvariables.terminal_dark [lahead]);

	    if (must_darken || its_case != NO_SPECIAL_CASE) {
		string_modify (chp, chl, its_case, must_darken);
	    }
	}

	if (string_info.lahead < 0) {
	    /* not seen yet: keep it */
	    string_info.lahead = lahead;
	    sxppvariables.terminal_string_info [ste] = string_info;
	}
    }


#ifdef	BUG_STRINGS

    {
	printf ("%s\n", sxstrget (string_info.ste));
    }




#endif

}




/* This procedure fills the global variable "string_info" with information
   concerning the string associated with the keyword of name "lahead" */

static VOID	kw_manager (SXINT lahead)
{

#ifdef	BUG_STRINGS

    {
	printf ("kw_manager (%ld):\t", (long)lahead);
    }


#endif

    string_info = sxppvariables.terminal_string_info [-lahead];

    if (string_info.lahead != lahead) {
	long	chl;
	char	*chp;

	string_info.lgth = chl = strlen (chp = sxttext (sxppvariables.sxtables, lahead));


#ifdef	BUG_STRINGS

	{
	    printf ("Non-Generic \"%s\" of length %ld =>\t", chp, chl);
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
	    CASE	its_case;
	    BOOLEAN	must_darken;

	    its_case = (sxppvariables.terminal_case != NULL ? sxppvariables.terminal_case [lahead] : (sxkeywordp (sxppvariables.sxtables, lahead) ? sxppvariables.kw_case : NO_SPECIAL_CASE));
	    must_darken = (sxppvariables.terminal_dark != NULL ? sxppvariables.terminal_dark [lahead] : (sxkeywordp (sxppvariables.sxtables, lahead) && sxppvariables.kw_dark));

	    if (must_darken || its_case != NO_SPECIAL_CASE) {
		string_modify (chp, chl, its_case, must_darken);
	    }
	    else {
		string_info.ste = sxstrsave (chp);
	    }
	}

	sxppvariables.terminal_string_info [-(string_info.lahead = lahead)] = string_info;
    }


#ifdef	BUG_STRINGS

    {
	printf ("%s\n", sxstrget (string_info.ste));
    }




#endif

}



static VOID	call_put_error (void)
{
    struct sxsource_coord	source_index;
    static char		severity [2] = {SEVERITIES - 1, 0};

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

	sxput_error (source_index, "%sError on margin in directive %d of production %d.", severity, co - sxppvariables.PP_tables [rd] + 1, rd);
    }
    else
	sxput_error (source_index, "Paragraphing cannot be done left of first column.", severity);

    sxppvariables.is_error = TRUE;
    column = 0;
}



static VOID	go_to_column (void)
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
	    this_is_a_new_line = TRUE;

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



static VOID	cadrage (long taille)
{
    if (taille > sxppvariables.line_length)
	column = 0;
    else if ((column = sxppvariables.line_length - taille) > saved_info.marge_erreur)
	column = saved_info.marge_erreur;
}



static VOID	impr_commentaire (char *ch_ptr)
{
    long	lg, longueur_premiere_ligne /* 0 si lg */ , depart;
    SXINT		nb_NEW_LINEs, nb_NEW_LINEs_en_fin;
    SXINT		sv_column;
    BOOLEAN	sv_this_is_a_new_line;

    if (column < 0)
	call_put_error ();


/* Initialisations */

    lg = strlen (ch_ptr);

    {
	char	ch;
	char	*nlp;

	longueur_premiere_ligne = 0;
	nb_NEW_LINEs = 0;

	for (nlp = ch_ptr; (ch = *nlp) != NUL; nlp++)
	    if (ch == NEW_LINE || ch == FORM_FEED) {
		if ((longueur_premiere_ligne = nlp - ch_ptr + 1) == 1) {
		    do
			nb_NEW_LINEs++;
		    while ((ch = *++nlp) != NUL && (ch == NEW_LINE || ch == FORM_FEED));

		    if (ch == NUL)
			nb_NEW_LINEs = -1;
		}

		break;
	    }
    }

    sv_column = column;
    sv_this_is_a_new_line = this_is_a_new_line;


/* Positionnement du commentaire */

    if (sxppvariables.char_count == 0) {
	/* Commentaire en debut de fichier */
	if ((depart = nb_NEW_LINEs - 2) < 0)
	    /* On prend au plus un NL */
	    depart = 0;

	nb_NEW_LINEs = 2 /* Pour le repositionnement */ ;
	column = current_column = 0 /* Pas de deplacement inutile */ ;
    }
    else
	switch (nb_NEW_LINEs) {
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
		this_is_a_new_line = FALSE;
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

		for (spp = &(ch_ptr [depart]); (ch = *spp) != NUL; spp++)
		    if (ch != HORIZONTAL_TAB && ch != SPACE)
			break;
		    else
			depart++;
	    }

	    if (longueur_premiere_ligne != 0) {
		char	*nlp;
		char	ch;

		longueur_premiere_ligne = 0;

		for (nlp = &(ch_ptr [depart]); (ch = *nlp) != NUL; nlp++)
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
    sxfree (ch_ptr);
}




#define impr_key_word(kw)		(kw_manager (kw), impr ())
#define impr_token(ste, la)		(string_manager (ste, la), impr ())


/* This procedure prints the string pointed to by "string_info" */

static VOID	impr (void)
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
	    this_is_a_new_line = FALSE;
	}
	else {
	    current_column = column = string_info.goes_to_column;
	    this_is_a_new_line = (string_info.goes_to_column == 0);
	}
    }
}




#ifdef	BUG_TREE

static VOID	sxppnp (struct sxnode_pp *node)
{
    if (node != NULL) {
	printf ("%lo (%ld):\t%lo\n\t", (long)node, node->terminal_token.lahead, (long)node->next);

	if (node->terminal_token.comment != NULL) {
	    printf ("comment: \"%s\"\n\t", node->terminal_token.comment);
	}

	if (node->name > 0) {
	    printf ("Production %ld", node->name);
	}
	else if (node->name == 0) {
	    printf ("ERROR NODE");
	}
	else {
	    printf ("%s (%s)", sxstrget (node->terminal_token.string_table_entry), sxttext (sxppvariables.sxtables, -node->name));
	}

	if (node->post_comment != NULL) {
	    printf ("\n\tpost_comment: \"%s\"\n", node->post_comment);
	}
    }
    else
	puts ("NULL");
}




#endif


static VOID	paragrapheur (struct sxnode_pp *root_ptr)
{
    struct SXPP_schema	directive /* current directive */ ;
    struct sxnode_pp	*node_ptr /* last node having been displayed */ ;
    BOOLEAN	inhibit_CR_FF /* Next CR must be inhibited */ ;
    static char		consistency_msg [] = "Your tables may be inconsistent...";


/* Initialisations */

    save_stack_index = 0;
    saved_info.noeud = node_ptr = root_ptr;
    saved_info.marge = 0;
    saved_info.marge_erreur = 5;
    current_column = column = 0;
    this_is_a_new_line = TRUE;
    inhibit_CR_FF = FALSE;
    sortie = base_sortie = sxalloc (dispo_sortie = taille_sortie = 256 * BUFSIZ, sizeof (*sortie));

#ifdef	BUG_TREE

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
	    sxppvariables.is_error = TRUE;

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


#ifdef	BUG_CODE

    {
	static char	*statements [] = {"???", "SKIP", "PAGE", "SPACE", "TAB", "MARGIN", "CALL", "COL", "RETURN", "????"};

	printf ("directive #%ld of production %ld:\t", (long)(saved_info.CO - sxppvariables.PP_tables [saved_info.noeud->name] + 1), saved_info.noeud->name);
	printf ("%s\t%ld\n", statements [directive.PP_codop], directive.PP_act);
    }


#endif

    switch (directive.PP_codop) {
    default:
	ppabort ("%s: illegal directive found\n", ME);

    case SKIPs:
	if (inhibit_CR_FF)
	    inhibit_CR_FF = FALSE;
	else {
	    SXINT	nb;

	    sxppvariables.char_count += nb = directive.PP_act;

	    while (nb-- > 0)
		sortie_1_car (NEW_LINE);

	    current_column = column = 0;
	    this_is_a_new_line = TRUE;

	    if (sxverbosep) {
		fprintf (sxtty, verbose_format, sxppvariables.char_count);
	    }
	}

	goto next_op;

    case PAGEs:
	if (inhibit_CR_FF)
	    inhibit_CR_FF = FALSE;
	else {
	    SXINT	nb;

	    sxppvariables.char_count += nb = directive.PP_act;

	    while (nb-- > 0)
		sortie_1_car (FORM_FEED);

	    current_column = column = 0;
	    this_is_a_new_line = TRUE;
	}

	goto next_op;

    case SPACEs:
	column += directive.PP_act;
	inhibit_CR_FF = FALSE;
	goto check_column;

    case TABs:
	column = (column / sxppvariables.tabs_interval + directive.PP_act) * sxppvariables.tabs_interval;
	inhibit_CR_FF = FALSE;
	goto next_op;

    case MARGIN:
	/* Positionnement par rapport a la marge */
	if ((column = saved_info.marge + directive.PP_act) < 0)
	    call_put_error ();
	else if (column > sxppvariables.max_margin)
	    column = sxppvariables.block_margin ? sxppvariables.max_margin : (column % sxppvariables.max_margin);

	inhibit_CR_FF = FALSE;
	goto check_column;

    case CALL:
	/* Terminal ou non-terminal */
	inhibit_CR_FF = FALSE;

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

    vider_sortie (FALSE);
    sxfree (base_sortie);
}



static VOID	sxppopen (struct sxtables *sxtables)
{
    if (sxppvariables.line_length <= 1)
	sxppvariables.line_length = 79;

    if (sxppvariables.tabs_interval <= 0)
	sxppvariables.tabs_interval = DFLT_TABS_INTERVAL;

    if (sxppvariables.max_margin > sxppvariables.line_length)
	sxhmsg (sxsrcmngr.source_coord.file_name, "%s%s: incorrect \"max_margin\". Value taken: %d.", sxtables->err_titles [0], ME, sxppvariables.max_margin = (sxppvariables.line_length * 2) / 3);
    else if (sxppvariables.max_margin <= 0)
	sxppvariables.max_margin = (sxppvariables.line_length * 2) / 3;

    if (sxppvariables.tabs_interval > sxppvariables.line_length)
	sxhmsg (sxsrcmngr.source_coord.file_name, "%s%s: incorrect tabulation interval. Value taken: %d.", sxtables->err_titles [0], ME, sxppvariables.tabs_interval = (DFLT_TABS_INTERVAL > sxppvariables.max_margin ? sxppvariables.max_margin : DFLT_TABS_INTERVAL));

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


#ifdef	BUG_STRINGS

    {
	printf ("kw_case: %d, kw_dark: %d\n", sxppvariables.kw_case, sxppvariables.kw_dark);
    }




#endif

}



static VOID	sxppclose (void)
{
    if (sxppvariables.save_stack != NULL) {
	sxfree (sxppvariables.save_stack), sxppvariables.save_stack = NULL;
    }

    if (sxppvariables.terminal_string_info != NULL) {
	sxfree (sxppvariables.terminal_string_info - sxppvariables.tmax), sxppvariables.terminal_string_info = NULL;
    }
}



VOID	sxppp (SXINT sxppp_what, struct sxtables *sxtables)
{
    switch (sxppp_what) {
    case OPEN:
	sxcheck_magic_number (sxtables->magic, SXMAGIC_NUMBER, ME);
	sxppopen (sxtables);
	break;

    case CLOSE:
	sxppclose ();
	break;

    case ACTION:
	sxppvariables.char_count = 0;
	sxppvariables.is_error = FALSE;

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


/* Once upon a time, I thought I had to use little memory while going fast */
#if 0

	{
	    FILE	save_stdout;


/* hack to attempt IO efficiency */

	    if (sxstdout != stdout) {
		save_stdout = *stdout;
		*stdout = *sxstdout;
		*sxstdout = save_stdout;
	    }

	    paragrapheur ((struct sxnode_pp*) sxtables);

	    if (sxstdout != stdout) {
		save_stdout = *stdout;
		*stdout = *sxstdout;
		*sxstdout = save_stdout;
	    }
	}


#endif
/* Now I believe I have to be very portable while going even faster */

	paragrapheur ((struct sxnode_pp*) sxtables);

	if (sxverbosep) {
	    fputc ('\n', sxtty);
	}

	break;

    default:
	fprintf (sxstderr, "The function \"sxppp\" is out of date with respect to its specification.\n");
	sxexit(1);
    }
}
