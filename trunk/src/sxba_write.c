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

/* Ecriture de tableaux de bits pour SYNTAX */

#include "sxversion.h"
#include "sxcommon.h"
#include "sxba.h"
#include "varstr.h"
#include <stdio.h>
#include <unistd.h>

char WHAT_SXBA_WRITE[] = "@(#)SYNTAX - $Id: sxba_write.c 3633 2023-12-20 18:41:19Z garavel $" WHAT_DEBUG;

bool
sxba_write (sxfiledesc_t file, SXBA bits_array)
/*
 * "sxba_write" writes "bits_array" on "file".
 * "file" must be the result of "open" or "create".
 */

{
    SXBA_INDEX	bytes_number = sxba_cast (SXNBLONGS (SXBASIZE (bits_array)) * sizeof (SXBA_ELT));

    sxbassert ((*(bits_array+SXNBLONGS (SXBASIZE (bits_array))) & ((~((SXBA_ELT)0)) << 1 << MOD (SXBASIZE (bits_array) - 1))) == 0, "write");

    return
      (write (file, bits_array, sizeof (SXBA_ELT)) == sizeof (SXBA_ELT)) &&
      (write (file, bits_array + 1, (size_t) bytes_number) == bytes_number)
      ;
}


void
sxba32_to_c (FILE *file, SXBA set)
{
    /* On genere pour une machine 32 bits */

    SXINT	i, lgth, nbmot;

    /* LINTED : this is 32bit-specific code : ignore the lint-64bit warning */
    lgth = SXBASIZE (set);
    nbmot = SXNBLONGS (lgth);

    sxbassert ((*(set+nbmot) & ((~((SXBA_ELT)0)) << 1 << MOD (lgth - 1))) == 0, "32_to_c");

    fprintf (file, "{%li, ", (SXINT) lgth);

#if SXBITS_PER_LONG == 32
    /* Execution sur une machine 32 */
    for (i = 1; i <= nbmot; i++) {
	if ((i & 7) == 7)
	    fprintf (file, "\n");

	fprintf (file, "0X%08lX, ", set [i]);
    } 
#else
    /* Execution sur une machine 64 */
    for (i = 1; i <= nbmot; i++) {
	if ((i & 3) == 3)
	    fprintf (file, "\n");

	fprintf (file, "0X%08lX, 0X%08lX, ", set [i]&((~(((SXBA_ELT)0)))>>32), set [i]>>32);
    } 
#endif

    fprintf (file, "}");
}

void
sxba64_to_c (FILE *file, SXBA set)
{
    /* On genere pour une machine 64 bits */
    SXBA_INDEX i, lgth, nbmot;

    lgth = sxba_cast (SXBASIZE (set));
    nbmot = SXNBLONGS (lgth);

    sxbassert ((*(set+nbmot) & ((~((SXBA_ELT)0)) << 1 << MOD (lgth - 1))) == 0, "64_to_c");


    fprintf (file, "{%li, ", (SXINT) lgth);

#if SXBITS_PER_LONG == 32
    /* Execution sur une machine 32 */
    for (i = 1; i <= nbmot; i += 2) {
	if ((i & 7) == 7)
	    fprintf (file, "\n");

	if (i < nbmot)
	    fprintf (file, "0X%08lX%08lX, ", set [i+1], set [i]);
	else
	    fprintf (file, "0X%016lX, ", set [i]);
    }
#else
    /* Execution sur une machine 64 */
    for (i = 1; i <= nbmot; i++) {
	if ((i & 3) == 3)
	    fprintf (file, "\n");

	fprintf (file, "0X%016lX, ", set [i]);
    }
#endif

    fprintf (file, "}");
}


void
sxba_to_c (SXBA set, FILE *file_descr, char *name1, char *name2, bool is_static)
{
    SXBA_INDEX lgth, nbmot;

    lgth = sxba_cast (SXBASIZE (set));
    nbmot = SXNBLONGS (lgth);

    sxbassert ((*(set+nbmot) & ((~((SXBA_ELT)0)) << 1 << MOD (lgth - 1))) == 0, "to_c");

    fprintf (file_descr, "\n\
%s SXBA_ELT %s%s\n\
", is_static ? "static " : "",
	     name1, name2);

    /* On genere pour une machine 32 bits */
    fprintf (file_descr, "#if SXBITS_PER_LONG==32\n\
 [%li] = ", (SXINT)
#if SXBITS_PER_LONG==32 /* On est sur une machine 32 bits */
	     nbmot
#else /* On est sur une machine 64 bits */
	     2*nbmot
#endif
	     +1);

    sxba32_to_c (file_descr, set);

    /* On genere pour une machine 64 bits */
    fprintf (file_descr, "\n#else\n\
 [%li] = ", (SXINT)
#if SXBITS_PER_LONG==32 /* On est sur une machine 32 bits */
	     (nbmot+1)/2
#else /* On est sur une machine 64 bits */
	     nbmot
#endif
	     +1);

    sxba64_to_c (file_descr, set);

    fprintf (file_descr, "\n#endif\n\
/* End %s%s */;\n\
", name1, name2);
}


/* A mettre eventuellement ds sxba_write et sxbitsmatrix */

static char hexa [] = "0123456789ABCDEF";

static VARSTR
sxba32_to_vstr (SXBA set, VARSTR vstr)
{
    /* On genere pour une machine 32 bits */
    SXBA_ELT	mot;
    SXINT	i, lgth, nbmot;
    char	*l, *r;
#if SXBITS_PER_LONG == 32
    /* Execution sur une machine 32 */
    static char str [] = "0X........, ";
#else
    /* Execution sur une machine 64 */
    static char str [] = "0X........, 0X........, ";
#endif

    /* LINTED : this is 32bit-specific code : ignore the lint-64bit warning */
    lgth = SXBASIZE (set);
    nbmot = SXNBLONGS (lgth);

    sxbassert ((*(set+nbmot) & ((~((SXBA_ELT)0)) << 1 << MOD (lgth - 1))) == 0, "32_to_vstr");

    for (i = 1; i <= nbmot; i++) {
	mot = set [i];

#if SXBITS_PER_LONG == 32
	/* Execution sur une machine 32 */
	if ((i & 7) == 7)
#else
    /* Execution sur une machine 64 */
	if ((i & 3) == 3)
#endif

	    varstr_catchar (vstr, '\n');

	l = str + 2;
	r = l + 8;
	
	while (--r >= l) {
	    *r = hexa [mot & 0xF];
	    mot >>= 4;
	}

#if SXBITS_PER_LONG == 64
	l = str + 14;
	r = l + 8;
	
	while (--r >= l) {
	    *r = hexa [mot & 0xF];
	    mot >>= 4;
	}
#endif

#if SXBITS_PER_LONG == 32
	/* Execution sur une machine 32 */
	varstr_lcatenate (vstr, str, 12);
#else
	/* Execution sur une machine 64 */
	varstr_lcatenate (vstr, str, 24);
#endif
    } 

    return vstr;
}

static VARSTR
sxba64_to_vstr (SXBA set, VARSTR vstr)
{
    /* On genere pour une machine 64 bits */
    SXBA_ELT	mot;
    SXBA_INDEX	i, lgth, nbmot;
    char	*l, *r;
    static char str [] = "0X................, ";

    lgth = sxba_cast (SXBASIZE (set));
    nbmot = SXNBLONGS (lgth);

    sxbassert ((*(set+nbmot) & ((~((SXBA_ELT)0)) << 1 << MOD (lgth - 1))) == 0, "64_to_vstr");

    for (i = 1; i <= nbmot; i++) {
	mot = set [i];

#if SXBITS_PER_LONG == 32
    /* Execution sur une machine 32 */
    	if ((i & 7) == 7)
	    varstr_catchar (vstr, '\n');

	l = str + 10;
	r = l + 8;
	
	while (--r >= l) {
	    *r = hexa [mot & 0xF];
	    mot >>= 4;
	}

	if (++i > nbmot)
	    mot = ((SXBA_ELT)0);
	else
	    mot = set [i];

	l = str + 2;
	r = l + 8;
#else
    /* Execution sur une machine 64 */
	if ((i & 3) == 3)
	    varstr_catchar (vstr, '\n');

	l = str + 2;
	r = l + 16;
#endif

	while (--r >= l) {
	    *r = hexa [mot & 0xF];
	    mot >>= 4;
	}

	varstr_lcatenate (vstr, str, 20);
    }

    return vstr;
}


/* C'est un peu moins mal que les autres versions ... */
/* mais ca reste d'une lenteur effrayante. */

void
sxba2c (SXBA set, FILE *file_descr, char *name1, char *name2, bool is_static, VARSTR vstr)
{
    SXBA_INDEX	lgth, nbmot;

    lgth = sxba_cast (SXBASIZE (set));
    nbmot = SXNBLONGS (lgth);

    sxbassert ((*(set+nbmot) & ((~((SXBA_ELT)0)) << 1 << MOD (lgth - 1))) == 0, "2c");

    fprintf (file_descr, "\n\
%s SXBA_ELT %s%s\n\
", is_static ? "static " : "",
	     name1, name2);

    /* On genere pour une machine 32 bits */
    fprintf (file_descr, "#if SXBITS_PER_LONG==32\n\
 [%li] = ",  (SXINT)
#if SXBITS_PER_LONG==32 /* On est sur une machine 32 bits */
	     nbmot
#else /* On est sur une machine 64 bits */
	     2*nbmot
#endif
	     +1);

    varstr_raz (vstr);
    fprintf (file_descr, "{%li, %s}", (SXINT) lgth, varstr_tostr (sxba32_to_vstr (set, vstr)));

    /* On genere pour une machine 64 bits */
    fprintf (file_descr, "\n#else\n\
 [%li] = ",  (SXINT)
#if SXBITS_PER_LONG==32 /* On est sur une machine 32 bits */
	     (nbmot+1)/2
#else /* On est sur une machine 64 bits */
	     nbmot
#endif
	     +1);

    varstr_raz (vstr);
    fprintf (file_descr, "{%li, %s}", (SXINT) lgth, varstr_tostr (sxba64_to_vstr (set, vstr)));

    fprintf (file_descr, "\n#endif\n\
/* End %s%s */;\n\
", name1, name2);
}


void
sxbm2c (FILE *file_descr, SXBA *bits_matrix, SXBA_INDEX line_nb, char *name1, char *name2, bool is_static, VARSTR vstr)
{
    /* Genere un SXBA_ELT [] [] et non un sxbm */
    SXBA_INDEX	i, lgth, nbmot;

#if SXBITS_PER_LONG!=32 && SXBITS_PER_LONG!=64
    sxtrap ("sxba_write", "sxbm2c");
#endif

    lgth = sxba_cast (SXBASIZE (bits_matrix [0]));
    nbmot = SXNBLONGS (lgth);
    
fprintf (file_descr, "\n\
%s SXBA_ELT %s%s [%li]\n\
", is_static ? "static " : "",
	     name1, name2, (SXINT) line_nb);

    /* On genere pour une machine 32 bits */
    fprintf (file_descr, "#if SXBITS_PER_LONG==32\n\
 [%li] = {\n\
",  (SXINT)
#if SXBITS_PER_LONG==32 /* On est sur une machine 32 bits */
	 nbmot
#else /* On est sur une machine 64 bits */
	 2*nbmot
#endif
+1);

    for (i = 0; i < line_nb; i++) {
	varstr_raz (vstr);
	fprintf (file_descr, "/* %li */ {%li, %s},\n\
", (SXINT) i, (SXINT) lgth, varstr_tostr (sxba32_to_vstr (bits_matrix [i], vstr)));
    }

    /* On genere pour une machine 64 bits */
    fprintf (file_descr, "#else\n\
 [%li] = {\n\
",  (SXINT)
#if SXBITS_PER_LONG==32 /* On est sur une machine 32 bits */
	 (nbmot+1)/2
#else /* On est sur une machine 64 bits */
	 nbmot
#endif
+1);

    for (i = 0; i < line_nb; i++) {
	varstr_raz (vstr);
	fprintf (file_descr, "/* %li */ {%li, %s},\n\
", (SXINT) i, (SXINT) lgth, varstr_tostr (sxba64_to_vstr (bits_matrix [i], vstr)));
    }

    fprintf (file_descr, "#endif\n\
} /* End %s%s */;\n\
", name1, name2);
}

