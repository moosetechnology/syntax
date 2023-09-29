/*   P R E D E F _ C L A S S _ N A M E   */

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
   *     Produit de l'equipe Langages et Traducteurs.     *
   *                                                      *
   ******************************************************** */



/*************************************************************/
/*                                                           */
/*                                                           */
/*  This program has been translated from lecl_pcn.pl1       */
/*  on Wednesday the twelfth of March, 1986, at 10:31:13,    */
/*  on the Multics system at INRIA,                          */
/*  by the release 3.3g PL1_C translator of INRIA,           */
/*         developped by the "Langages et Traducteurs" team, */
/*         using the SYNTAX (*), FNC and Paradis systems.    */
/*                                                           */
/*                                                           */
/*************************************************************/
/* (*) SYNTAX is a trademark of INRIA.                       */
/*************************************************************/



/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030423 17:59 (phd):	Le paramètre abbrev_or_class_to_ate	*/
/*				n'est pas un long[]			*/
/************************************************************************/
/* 20030423 17:58 (phd):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/

#include "sxunix.h"
#include "sxba.h"
char WHAT_LECLPCN[] = "@(#)SYNTAX - $Id: lecl_pcn.c 582 2007-05-25 08:25:51Z rlacroix $" WHAT_DEBUG;

#define undef_name 0
#define illegal_chars 1
#define eof 2
#define any 3

struct {
    char	*name;
    char	*elems;
}	predefs [] = {{"NUL", "\0"},
		      {"SOH", "\1"},
		      {"STX", "\2"},
		      {"ETX", "\3"},
		      {"EOT", "\4"},
		      {"ENQ", "\5"},
		      {"ACK", "\6"},
		      {"BEL", "\7"},
		      {"BS", "\b"},
		      {"HT", "\t"},
		      {"LF", "\n"},
		      {"NL", "\n"},
		      {"EOL", "\n"},
		      {"VT", "\13"},
		      {"FF", "\f"},
		      {"CR", "\r"},
		      {"SO", "\16"},
		      {"SI", "\17"},
		      {"DLE", "\20"},
		      {"DC1", "\21"},
		      {"DC2", "\22"},
		      {"DC3", "\23"},
		      {"DC4", "\24"},
		      {"NAK", "\25"},
		      {"SYN", "\26"},
		      {"ETB", "\27"},
		      {"CAN", "\30"},
		      {"EM", "\31"},
		      {"SUB", "\32"},
		      {"ESC", "\33"},
		      {"FS", "\34"},
		      {"GS", "\35"},
		      {"RS", "\36"},
		      {"US", "\37"},
		      {"SP", " "},
		      {"QUOTE", "\""},
		      {"DEL", "\177"},
		      {"LETTER", "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"},
		      {"UPPER", "ABCDEFGHIJKLMNOPQRSTUVWXYZ"},
		      {"LOWER", "abcdefghijklmnopqrstuvwxyz"},
		      {"A", "Aa"},
		      {"B", "Bb"},
		      {"C", "Cc"},
		      {"D", "Dd"},
		      {"E", "Ee"},
		      {"F", "Ff"},
		      {"G", "Gg"},
		      {"H", "Hh"},
		      {"I", "Ii"},
		      {"J", "Jj"},
		      {"K", "Kk"},
		      {"L", "Ll"},
		      {"M", "Mm"},
		      {"N", "Nn"},
		      {"O", "Oo"},
		      {"P", "Pp"},
		      {"Q", "Qq"},
		      {"R", "Rr"},
		      {"S", "Ss"},
		      {"T", "Tt"},
		      {"U", "Uu"},
		      {"V", "Vv"},
		      {"W", "Ww"},
		      {"X", "Xx"},
		      {"Y", "Yy"},
		      {"Z", "Zz"},
		      {"DIGIT", "0123456789"},};



VOID	lecl_pcn (cmax, abbrev_or_class_to_ate, class_to_char_set)
    int		*cmax;
    int		abbrev_or_class_to_ate[];
    SXBA	*class_to_char_set;
{
    register int	j;
    register char	*s;
    register SXBA	cts;
    register int	i, ate, k;


/* UNDEFINED NAME */

    abbrev_or_class_to_ate [undef_name] = sxstrsave ("Undefined Name");
    /* ILLEGAL CHARS */
    abbrev_or_class_to_ate [illegal_chars] = sxstrsave ("Illegal Chars");
    /* EOF */
    abbrev_or_class_to_ate [eof] = sxstrsave ("EOF");
    /* ANY */

/* On met "ANY" 
   car il peut etre reference implicitement par l'operateur "^" */
    abbrev_or_class_to_ate [any] = sxstrsave ("ANY");
    sxba_fill (class_to_char_set [any]), SXBA_0_bit (class_to_char_set [any], 0);
    j = any;

    for (i = 0; (unsigned)i < sizeof (predefs) / sizeof (predefs [0]); i++) {
	ate = sxstrretrieve (predefs [i].name);

	if (ate != ERROR_STE) {
	    /* classe predefinie utilisee dans le source */
	    j++;
	    abbrev_or_class_to_ate [j] = ate;
	    cts = class_to_char_set [j];
	    s = predefs [i].elems;

	    do {
		k = *s++ + 1;
		SXBA_1_bit (cts, k);
	    } while (*s != NUL);
	}
    }

    *cmax = j;
}
