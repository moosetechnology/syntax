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

/*   P R E D E F _ C L A S S _ N A M E   */

#include "sxversion.h"
#include "sxunix.h"
#include "sxba.h"

char WHAT_LECLPCN[] = "@(#)SYNTAX - $Id: lecl_pcn.c 3621 2023-12-17 11:11:31Z garavel $" WHAT_DEBUG;

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



void	lecl_pcn (SXINT *cmax, 
		  SXINT abbrev_or_class_to_ate[], 
		  SXBA *class_to_char_set)
{
    SXINT	j;
    char	*s;
    SXBA	cts;
    SXINT	i, ate, k;


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
    sxba_fill (class_to_char_set [any]);
    SXBA_0_bit (class_to_char_set [any], 0);
    j = any;

    for (i = 0; (SXUINT)i < sizeof (predefs) / sizeof (predefs [0]); i++) {
	ate = sxstrretrieve (predefs [i].name);

	if (ate != SXERROR_STE) {
	    /* classe predefinie utilisee dans le source */
	    j++;
	    abbrev_or_class_to_ate [j] = ate;
	    cts = class_to_char_set [j];
	    s = predefs [i].elems;

	    do {
		k = *s++ + 1;
		SXBA_1_bit (cts, k);
	    } while (*s != SXNUL);
	}
    }

    *cmax = j;
}
