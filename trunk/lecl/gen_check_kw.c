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

#include "sxversion.h"
#include "sxunix.h"
#include "varstr.h"
#include "lecl_ag.h"
#include "sxdico.h"

char WHAT_LECLGENCHECKKW[] = "@(#)SYNTAX - $Id: gen_check_kw.c 3986 2024-06-01 10:06:49Z garavel $" WHAT_DEBUG;

#define char_max 256

static struct couple {
	   SXINT	lgth, delta;
       }	*couples, *couples_syno;

static struct kw {
    unsigned char *str;
    SXINT lgth, code, syno;
} *kws;

static char blanks [] = "                                                                                                   ";

static SXINT blanks_len;

static char this_program [] = "   *  This program has been generated from ";

static char	head [] = "\n\
static SXINT sxcheck_keyword (unsigned char *string, SXINT length)\n\
{\n\
   SXINT  t_code, delta;\n\
   unsigned char *keyword;";

static char	tail [] = "   do {\n\
      if (*string++ != *keyword++)\n\
          return 0;\n\
   } while (--length > 0) ;\n\
\n\
   return t_code;\n";

static SXINT	*sorted, *sorted_by_char;
static SXBA	char_set1, char_set2;
static SXINT	x_char, x_couple, x_couple_syno;
static bool	is_compare, is_syno, is_compare_syno;
static char new_line []= "\n";

static void cat_nnl (SXINT level, char *str)
{
    SXINT nb = 3 * level;

    while (nb > blanks_len) {
	varstr_catenate (check_kw, blanks);
	nb -= blanks_len;
    }
    
    varstr_catenate (check_kw, blanks + (blanks_len - nb));
    varstr_catenate (check_kw, str);
}

static void cat (SXINT level, char *str)
{
    cat_nnl (level, str);
    varstr_catenate (check_kw, new_line);
}


static void cat_ap (char *str)
{
    varstr_catenate (check_kw, str);
    varstr_catenate (check_kw, new_line);
}

static void cat_apnnl (char *str)
{
    varstr_catenate (check_kw, str);
}

static void cat_cstr_in_comment (char *str)
{
    char *p = str;

    do {
	while ((p = sxindex (p, '*')) != NULL && *++p != '/');

	if (p == NULL)
	    break;
	    
	varstr_lcat_cstring (check_kw, str, p - str);
	varstr_catenate (check_kw, "\\");
	str = p;
    } while (true);

    varstr_cat_cstring (check_kw, str);
}


static void cat_c (char c)
{
    if (c != '\'')
	varstr_catenate (check_kw, SXCHAR_TO_STRING (c));
    else
	varstr_catenate (check_kw, "\\'");
}

static void cat_nl (SXINT n)
{
    SXINT i;

    for (i = 1; i <= n;i++) {
	varstr_catenate (check_kw, new_line);
    }
}

static void cat_apnb (SXINT nb)
{
    char s[22];

    sprintf (s, "%ld", nb);
    varstr_catenate (check_kw, s);
}

static bool	by_lgth (SXINT i, SXINT j)
{
    return kws [i].lgth < kws [j].lgth;
}



static bool	less (SXINT i, SXINT j)
{
    return kws [sorted [i]].str [x_char] < kws [sorted [j]].str [x_char];
}



static void	sort (SXINT bi, SXINT bf, SXINT x)
{
    SXINT	i;

    for (i = bi; i <= bf; i++) {
	sorted_by_char [i] = i;
    }

    x_char = x;
    sort_by_tree (sorted_by_char, bi, bf, less);

    for (i = bi; i <= bf; i++) {
	sorted_by_char [i] = sorted [sorted_by_char [i]];
    }

    for (i = bi; i <= bf; i++) {
	sorted [i] = sorted_by_char [i];
    }
}



static void	store (struct couple *store_couples,
                       SXINT *store_x_couple, 
                       SXINT lgth, 
                       SXINT delta)
{
    struct couple	*xc;

    for (xc = store_couples + *store_x_couple; xc > store_couples; xc--) {
	if (xc->lgth == lgth && xc->delta == delta)
	    return;
    }

    xc = store_couples + ++*store_x_couple;
    xc->lgth = lgth;
    xc->delta = delta;
}



static void	gen_cmp (struct couple *gen_cmp_couples,
                         SXINT gen_cmp_x_couple,
                         bool gen_cmp_is_syno)
{
    struct couple	*xc;

    for (xc = gen_cmp_couples + gen_cmp_x_couple--; gen_cmp_x_couple >= 0; xc--, gen_cmp_x_couple--) {
	cat_apnnl ("cmp_");

	if (gen_cmp_is_syno)
	    cat_apnnl ("syno_");

	cat_apnb (xc->lgth);
	cat_apnnl ("_");
	cat_apnb (xc->delta);
	cat_ap (":");

	if (xc->lgth != 0) {
	    cat_apnnl ("   length -= ");
	    cat_apnb (xc->lgth);
	    cat_ap (";");
	}

	cat_apnnl ("   delta = ");
	cat_apnb (xc->delta);
	cat_ap (";");

	if (gen_cmp_x_couple > 0) {
	    if (gen_cmp_is_syno)
		cat_ap ("   goto compare_syno;\n");
	    else
		cat_ap ("   goto compare;\n");
	}
    }

    cat_apnnl ("compare");

    if (gen_cmp_is_syno)
	cat_apnnl ("_syno");

    cat_apnnl (":\n\
   keyword = (unsigned char*)sxtables.SXS_tables.S_adrp [t_");
    cat_apnnl (gen_cmp_is_syno ? "syno" : "code");

    cat_ap ("] + delta;\n");

}



static SXINT	get_best (SXINT bi, SXINT bf, SXINT kw_lgth, SXINT *min, SXINT *max, bool *is_min)
{
    SXINT	x, imin, imax, l1, l2, xmin;
    unsigned char	*str;

    sxinitialise(xmin);
    sxba_empty (char_set1);
    sxba_empty (char_set2);
    imin = *min + 1;
    imax = kw_lgth - *max;

    for (x = bi; x <= bf; x++) {
	str = kws [sorted [x]].str;
	sxba_1_bit (char_set1, str [imin]);
	sxba_1_bit (char_set2, str [imax]);
    }

    l1 = sxba_cardinal (char_set1);
    l2 = sxba_cardinal (char_set2);

    if (l1 > 1 || l2 > 1) {
	/* On doit progresser dans la discrimination */

	if (l1 >= l2) {
	    *is_min = true;
	    ++*min;
	    x = imin;
	}
	else {
	    *is_min = false;
	    ++*max;
	    x = imax;
	}
    }
    else {
	for (imin++; imin < imax; imin++) {
	    sxba_empty (char_set1);

	    for (x = bi; x <= bf; x++) {
		str = kws [sorted [x]].str;
		sxba_1_bit (char_set1, str [imin]);
	    }

	    if ((l1 = sxba_cardinal (char_set1)) > l2) {
		/* On a trouve mieux */
		l2 = l1;
		xmin = imin;
	    }
	}

	*is_min = false;
	x = xmin;
    }

    sort (bi, bf, x);
    return x;
}



static void gen (SXINT bi, SXINT bf, SXINT kw_lgth, SXINT min, SXINT max, SXINT level)
{
    SXINT	x, y;
    SXINT		char_no, d;
    struct kw *kw;
    bool	is_min;
    char	*str;

    if (bi == bf) {
	str = (char *) (kw = kws + sorted [bi])->str + 1;

	if ((d = kw_lgth - (min + max)) <= 2) {
	    cat_nnl (level, "return ");
	    
	    if (d == 2) {
		cat_apnnl ("*string++ == '");
		cat_c (str [min++]);
		cat_apnnl ("' && ");
	    }

	    if (d > 0) {
		cat_apnnl ("*string == '");
		cat_c (str [min]);
		cat_apnnl ("'");
		cat_apnnl (" ? ");
	    }

	    cat_apnb (kw->code);
	    cat_apnnl (" /* ");
	    cat_cstr_in_comment (str - 1);
	    cat_apnnl (" */");

	    if (d > 0) {
		cat_apnnl (": 0");
	    }

	    cat_ap (";\n");
	}
	else {
	    cat_nnl (level, "t_code = ");
	    cat_apnb (kw->code);
	    cat_apnnl (" /* ");

	    if (kw->code != kw->syno) {
		cat_cstr_in_comment (sxstrget (t_to_ate [kw->code]));
		cat_ap (" */;");
		cat_nnl (level, "t_syno = ");
		cat_apnb (kw->syno);
		cat_apnnl (" /* ");
		is_compare_syno = true;
		store (couples_syno, &x_couple_syno, min + max, min);
	    }
	    else {
		is_compare = true;
		store (couples, &x_couple, min + max, min);
	    }

	    cat_cstr_in_comment (str - 1);
	    cat_ap (" */;");
	    cat_nnl (level, "goto cmp_");

	    if (kw->code != kw->syno)
		cat_apnnl ("syno_");

	    cat_apnb (min + max);
	    cat_apnnl ("_");
	    cat_apnb (min);
	    cat_ap (";\n");
	}
    }
    else {
	char_no = get_best (bi, bf, kw_lgth, &min, &max, &is_min);

	if (is_min) {
	    cat_nnl (level, "switch (*string");

	    if (min + max < kw_lgth)
		cat_apnnl ("++");

	    cat_ap (") {");
	}
	else {
	    cat_nnl (level, "switch (string [");
	    cat_apnb (char_no - 1 - min);
	    cat_ap ("]) {");
	}

	for (x = bi; x <= bf; x = y) {
	    unsigned char car = kws [sorted [x]].str [char_no];

	    for (y = x + 1; y <= bf && kws [sorted [y]].str [char_no] == car; y++)
		;

	    cat_nnl (level, "case '");
	    cat_c ((char) car);
	    cat_ap ("':");
	    gen (x, y - 1, kw_lgth, min, max, level + 1);
	}

	cat (level, "default:");
	cat (level + 1, "return 0;");
	cat (level, "}\n");
    }
}



void gen_check_kw (void)
{
    SXINT	i, bi, bf, lgth;
    SXINT 		syno, code;

    check_kw = NULL;
    last_static_ste = termax;
    syno_kw_string_lgth = 0;

    if (nbndef <= 0)
	return;

    if (sxverbosep) {
	if (!sxttycol1p) {
	    fputc ('\n', sxtty);
	}
	else
	    sxttycol1p = false;

	fputs ("Check Keywords", sxtty);
    }


    kws = (struct kw*) sxalloc (nbndef + 1, sizeof (struct kw));

    is_compare = is_compare_syno = is_syno = false;
    syno = termax + 1;

    if (is_dico) {
	/* Utilisation de la forme "dictionnaire" */


	static struct mot2	mot2;
	static struct dico	dico;
	static SXINT		*syno2code, *kw_code2t_code, *kw_code2lgth;

	mot2.size = mot2.nb = nbndef;
	mot2.string = (char**) sxalloc (mot2.size + 1, sizeof (char*));
	mot2.string [0] = (char*) NULL;
	mot2.code = (SXINT*) sxalloc (mot2.size + 1, sizeof (SXINT));
	mot2.code [0] = 0;
	mot2.lgth = (SXINT*) sxalloc (mot2.size + 1, sizeof (SXINT));
	mot2.lgth [0] = 0;
	mot2.optim_kind = TIME; /* Priorite au temps sur l'espace */
	mot2.process_kind = PARTIEL; /* Seuls les prefixe et suffixe discriminants minimaux de chaque chaine sont entres ds l'automate */
	mot2.print_on_sxtty = false; /* on travaille ds le silence */
	mot2.min = (SXINT*) sxalloc (mot2.nb + 1, sizeof (SXINT));
	mot2.max = (SXINT*) sxalloc (mot2.nb + 1, sizeof (SXINT));

	syno2code = (SXINT*) sxalloc (nbndef+1, sizeof (SXINT));
	kw_code2t_code = (SXINT*) sxalloc (nbndef+1, sizeof (SXINT));
	kw_code2t_code [0] = 0;
	kw_code2lgth = (SXINT*) sxalloc (nbndef+1, sizeof (SXINT));
	kw_code2lgth [0] = 0;

	/* Attention les mots cles sont entre double quotes */
	/* La longueur n'en tient pas compte */
	for (i = 1; i <= nbndef; i++) {
	    SXINT ate, l;

	    ate = keywords [i].string_table_entry;
	    mot2.string [i] = sxstrget (ate)+1;
	    mot2.lgth [i] = l = sxstrlen (ate) - 2;

	    if ((code = keywords [i].t_code) < 0) {
		/* synonym */
		syno2code [syno-termax] = -code;
		code = syno++;
		last_static_ste++;
		syno_kw_string_lgth += l;
		is_syno = true;
	    }

	    mot2.code [i] = i;
	    kw_code2t_code [i] = code;
	    kw_code2lgth [i] = l;
	}

	if (nbndef > 1) {
	    sxdico_process (mot2, &dico);

	    if (((~((SXUINT) 0))>>dico.base_shift) < dico.max)
		sxtmsg (sxsrcmngr.source_coord.file_name, "%sToo many keywords, dictionary not produced\n",
			sxplocals.sxtables->err_titles [2]+1 /* Error */);
	}

	check_kw = varstr_alloc (2500 /* partie fixe */ + 10 /* en moyenne */ * (nbndef+dico.max));

	cat_ap ("\n#include \"sxdico.h\"\n");
	cat_apnnl ("#define KW_NB		");
	cat_apnb (nbndef);
	cat_ap ("");

	if (nbndef > 1) {
	    cat_apnnl ("#define INIT_BASE	");
	    cat_apnb (dico.init_base>>dico.base_shift);
	    cat_apnnl ("\n#define INIT_SUFFIX	");
	    cat_apnb (dico.init_base&dico.is_suffix_mask);
	    cat_apnnl ("\n#define CLASS_MASK	");
	    cat_apnb (dico.class_mask);
	    cat_apnnl ("\n#define STOP_MASK	");
	    cat_apnb (dico.stop_mask);
	    cat_apnnl ("\n#define BASE_SHIFT	");
	    cat_apnb (dico.base_shift);
	    cat_apnnl ("\n#define SUFFIX_MASK	");
	    cat_apnb (dico.is_suffix_mask);
	    cat_ap ("");
	}


	if (is_syno) {
	    cat_ap ("\n#define SYNO");
	    cat_apnnl ("#define EOF_CODE	");
	    cat_apnb (termax);
	    cat_ap ("");
	    cat_apnnl ("static SXINT syno2t_code [");
	    cat_apnb (syno-termax-1);
	    cat_ap ("] = {");

	    for (i = termax+1; i < syno; i++) {
		if ((i-termax)%16 == 0)
		    cat_nl ((SXINT)1);

		cat_apnb (syno2code [i-termax]);
		cat_apnnl (", ");
	    }

	    cat_ap ("\n};");
	}

	cat_apnnl ("\nstatic SXINT kw_code2t_code [");
	cat_apnb (nbndef+1);
	cat_ap ("] = {0,");

	for (i = 1; i <= nbndef; i++) {
	    if ((i)%16 == 0)
		cat_nl ((SXINT)1);

	    cat_apnb (kw_code2t_code [i]);
	    cat_apnnl (", ");
	}

	cat_ap ("\n};");

	cat_apnnl ("\nstatic SXINT kw_code2lgth [");
	cat_apnb (nbndef+1);
	cat_ap ("] = {0,");

	for (i = 1; i <= nbndef; i++) {
	    if ((i)%16 == 0)
		cat_nl ((SXINT)1);

	    cat_apnb (kw_code2lgth [i]);
	    cat_apnnl (", ");
	}

	cat_ap ("\n};");


	if (nbndef > 1) {
	    unsigned char	class;

	    cat_apnnl ("\nstatic SXUINT comb_vector [");
	    cat_apnb (dico.max+1);
	    cat_ap ("] = {");

	    for (i = 0; (SXUINT)i <= dico.max; i++) {
		if ((i % 10) == 9)
		    cat_nl ((SXINT)1);

		cat_apnb (dico.comb_vector [i]);
		cat_apnnl (", ");
	    }

	    cat_ap ("\n};");

	    cat_ap ("\nstatic unsigned char char2class [256] = {");

	    for (i = 0; i <= 255; i++) {
		if ((i % 8) == 7)
		    cat_nl ((SXINT)1);

		class = dico.char2class [i];
		cat_apnnl ("\'");

		if (class == '\'')
		    varstr_catenate (check_kw, "\\'");
		else
		    varstr_catenate (check_kw, SXCHAR_TO_STRING (class));

		cat_apnnl ("\', ");
	    }

	    cat_ap ("\n};");
	}

	cat_ap ("\n#include \"sxcheck_keyword.h\"");

	sxfree (mot2.string), mot2.string = NULL;
	sxfree (mot2.code), mot2.code = NULL;
	sxfree (mot2.lgth), mot2.lgth = NULL;
	sxfree (mot2.min), mot2.min = NULL;
	sxfree (mot2.max), mot2.max = NULL;
	sxfree (syno2code), syno2code = NULL;
	sxfree (kw_code2t_code), kw_code2t_code = NULL;
	sxfree (kw_code2lgth), kw_code2lgth = NULL;

	if (dico.comb_vector) sxfree (dico.comb_vector), dico.comb_vector = NULL; 
    }
    else {
	/* "sxcheck_keyword" est directement une fonction C */
	/* Attention les mots cles sont entre double quotes */
	/* La longueur n'en tient pas compte */
	for (i=1; i <= nbndef;i++) {
	    struct kw *kw = kws + i;
	    SXINT ate, l;

	    ate = keywords [i].string_table_entry;
	    kw->str = (unsigned char *)sxstrget (ate);
	    kw->lgth = l = sxstrlen (ate) - 2;

	    if ((kw->code = keywords [i].t_code) < 0) {
		/* synonym */
		kw->code = -kw->code;
		kw->syno = syno++;
		last_static_ste++;
		syno_kw_string_lgth += l;
		is_syno = true;
	    }
	    else
		kw->syno = kw->code;
	}

	sorted = (SXINT*) sxalloc (nbndef + 1, sizeof (SXINT));
	sorted_by_char = (SXINT*) sxalloc (nbndef + 1, sizeof (SXINT));
	char_set1 = sxba_calloc (char_max);
	char_set2 = sxba_calloc (char_max);
	couples = (struct couple*) sxalloc (nbndef + 1, sizeof (struct couple));
	couples_syno = is_syno ? (struct couple*) sxalloc (nbndef + 1, sizeof (struct couple)) : NULL;
	x_couple = x_couple_syno = 0;
	blanks_len = strlen (blanks);
	check_kw = varstr_alloc (1000 /* partie fixe */ + 100 /* en moyenne */ * nbndef);

	/* on genere le prologue */

	cat_ap ("/* ********************************************************************");
	cat_apnnl (this_program);
	cat_apnnl (prgentname);
	cat_apnnl (".lecl");
	i = strlen (this_program) + strlen (prgentname) + 5 /* ".lecl" */;
	cat_apnnl (blanks + (blanks_len - (70 - i)));
	cat_ap ("*");
	cat_ap ("   *  by the SYNTAX (*) lexical constructor LECL                      *");
	cat_ap ("   ********************************************************************");
	cat_ap ("   *  (*) SYNTAX is a trademark of INRIA.                             *");
	cat_ap ("   ******************************************************************** */");
	cat_nl ((SXINT)2);
	cat_ap (head);

	if (is_syno)
	    cat_ap ("   SXINT t_syno;");

	cat_ap ("\n\
   switch (length) {");

	for (i = 1; i <= nbndef; i++)
	    sorted [i] = i;

	sort_by_tree (sorted, 1, nbndef, by_lgth);

	for (bi = 1; bi <= nbndef; bi = bf) {
	    lgth = kws [sorted [bi]].lgth;

	    for (bf = bi + 1; bf <= nbndef && kws [sorted [bf]].lgth == lgth; bf++)
		;

	    cat_apnnl ("   case ");
	    cat_apnb (lgth);
	    cat_ap (":");
	    gen (bi, bf - 1, lgth, (SXINT)0, (SXINT)0, (SXINT)2);
	}

	cat_ap ("   default:\n\
      return 0;\n\
   }\n");

	if (is_compare) {
	    gen_cmp (couples, x_couple, false);
	    cat_ap (tail);
	}

	if (is_compare_syno) {
	    gen_cmp (couples_syno, x_couple_syno, true);
	    cat_ap (tail);
	}

	cat_ap ("}");

	if (is_syno)
	    sxfree (couples_syno);

	sxfree (couples);
	sxfree (char_set2);
	sxfree (char_set1);
	sxfree (sorted_by_char);
	sxfree (sorted);
    }

    sxfree (kws);
}
