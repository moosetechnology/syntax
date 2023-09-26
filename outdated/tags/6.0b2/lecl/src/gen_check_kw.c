/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1984 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *     Produit de l'equipe Langages et Traducteurs.     *
   *                                                      *
   ******************************************************** */



/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20040213 14:18 (phd&pb):	Modifs pour calmer le compilo HP	*/
/************************************************************************/
/* 15 Sep 1999 13:35 (pb):	Utilisation de unsigned char ds 	*/
/*				check_keyword				*/
/************************************************************************/
/* 28-06-90 13:45 (pb):		Suppression de "cat_cstr" (plus utilise)*/
/************************************************************************/
/* 14-06-90 16:02 (pb):		Pb ds le source genere si un mot-cle	*/
/*				contient une etoile suivie d'un slash	*/
/************************************************************************/
/* 18-11-88 10:20 (pb):		le caractere '\'' etait genere ''' 	*/
/************************************************************************/
/* 08-06-88 09:23 (pb):		Bug dans le genere ( string [n] ne 	*/
/*				tenait pas compte des string++)		*/
/************************************************************************/
/* 27-04-88 10:00 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/

/*****************************************************************/
/* (*) SYNTAX is a trademark of INRIA.                           */
/*****************************************************************/

#include "sxunix.h"
#include "varstr.h"
#include "lecl_ag.h"
char WHAT_LECLGENCHECKKW[] = "@(#)SYNTAX - $Id: gen_check_kw.c 607 2007-06-04 09:35:31Z syntax $" WHAT_DEBUG;

#define char_max 256

static struct couple {
	   int	lgth, delta;
       }	*couples, *couples_syno;
static struct kw {
    unsigned char *str;
    int lgth, code, syno;
} *kws;
static char blanks [] = "                                                                                                   ";
static int blanks_len;
static char this_program [] = "   *  This program has been generated from ";
static char	head [] = "\n\
static int check_keyword (string, length)\n\
register unsigned char	*string;\n\
register int	length;\n\
{\n\
   register int  t_code, delta;\n\
   register unsigned char *keyword;";
static char	tail [] = "   do {\n\
      if (*string++ != *keyword++)\n\
          return 0;\n\
   } while (--length > 0) ;\n\
\n\
   return t_code;\n";
static int	*sorted, *sorted_by_char;
static SXBA	char_set1, char_set2;
static int	x_char, x_couple, x_couple_syno;
static BOOLEAN	is_compare, is_syno, is_compare_syno;
static char new_line []= "\n";

static VOID cat_nnl (level, str)
int level;
char *str;
{
    register int nb = 3 * level;

    while (nb > blanks_len) {
	varstr_catenate (check_kw, blanks);
	nb -= blanks_len;
    }
    
    varstr_catenate (check_kw, blanks + (blanks_len - nb));
    varstr_catenate (check_kw, str);
}

static VOID cat (level, str)
int level;
char *str;
{
    cat_nnl (level, str);
    varstr_catenate (check_kw, new_line);
}


static VOID cat_ap (str)
char *str;
{
    varstr_catenate (check_kw, str);
    varstr_catenate (check_kw, new_line);
}

static VOID cat_apnnl (str)
char *str;
{
    varstr_catenate (check_kw, str);
}

static VOID cat_cstr_in_comment (str)
    char *str;
{
    register char *p = str;

    do {
	while ((p = sxindex (p, '*')) != NULL && *++p != '/');

	if (p == NULL)
	    break;
	    
	varstr_lcat_cstring (check_kw, str, p - str);
	varstr_catenate (check_kw, "\\");
	str = p;
    } while (TRUE);

    varstr_cat_cstring (check_kw, str);
}


static VOID cat_c (c)
char c;
{
    if (c != '\'')
	varstr_catenate (check_kw, CHAR_TO_STRING (c));
    else
	varstr_catenate (check_kw, "\\'");
}

static VOID cat_nl (n)
int n;
{
    register int i;

    for (i = 1; i <= n;i++) {
	varstr_catenate (check_kw, new_line);
    }
}

static VOID cat_apnb (nb)
int nb;
{
    char s[8];

    sprintf (s, "%d", nb);
    varstr_catenate (check_kw, s);
}

static BOOLEAN	by_lgth (i, j)
    int		i, j;
{
    return kws [i].lgth < kws [j].lgth;
}



static BOOLEAN	less (i, j)
    int		i, j;
{
    return kws [sorted [i]].str [x_char] < kws [sorted [j]].str [x_char];
}



static VOID	sort (bi, bf, x)
    int		bi, bf, x;
{
    register int	i;

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



static VOID	store (store_couples, store_x_couple, lgth, delta)
    struct couple	*store_couples;
    int		*store_x_couple, lgth, delta;
{
    register struct couple	*xc;

    for (xc = store_couples + *store_x_couple; xc > store_couples; xc--) {
	if (xc->lgth == lgth && xc->delta == delta)
	    return;
    }

    xc = store_couples + ++*store_x_couple;
    xc->lgth = lgth;
    xc->delta = delta;
}



static VOID	gen_cmp (gen_cmp_couples, gen_cmp_x_couple, gen_cmp_is_syno)
     struct couple	*gen_cmp_couples;
     int gen_cmp_x_couple;
     BOOLEAN gen_cmp_is_syno;
{
    register struct couple	*xc;

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



static int	get_best (bi, bf, kw_lgth, min, max, is_min)
    int		bi, bf, kw_lgth, *min, *max;
    BOOLEAN	*is_min;
{
    register int	x, imin, imax, l1, l2, xmin;
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
	    *is_min = TRUE;
	    ++*min;
	    x = imin;
	}
	else {
	    *is_min = FALSE;
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

	*is_min = FALSE;
	x = xmin;
    }

    sort (bi, bf, x);
    return x;
}



static VOID gen (bi, bf, kw_lgth, min, max, level)
    int		bi, bf, kw_lgth, min, max, level;
{
    register int	x, y;
    int		char_no, d;
    register struct kw *kw;
    BOOLEAN	is_min;
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
		is_compare_syno = TRUE;
		store (couples_syno, &x_couple_syno, min + max, min);
	    }
	    else {
		is_compare = TRUE;
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
	    cat_c (car);
	    cat_ap ("':");
	    gen (x, y - 1, kw_lgth, min, max, level + 1);
	}

	cat (level, "default:");
	cat (level + 1, "return 0;");
	cat (level, "}\n");
    }
}



VOID gen_check_kw ()
{
    register int	i, bi, bf, lgth;
    int 		syno, code;
    long		date_time;
    char		*pct;

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
	    sxttycol1p = FALSE;

	fputs ("Check Keywords", sxtty);
    }


    kws = (struct kw*) sxalloc (nbndef + 1, sizeof (struct kw));

    is_compare = is_compare_syno = is_syno = FALSE;
    syno = termax + 1;

    if (is_dico) {
	/* Utilisation de la forme "dictionnaire" */
#include "sxdico.h"

	static struct mot2	mot2;
	static struct dico	dico;
	static int		*syno2code, *kw_code2t_code, *kw_code2lgth;

	mot2.size = mot2.nb = nbndef;
	mot2.string = (char**) sxalloc (mot2.size + 1, sizeof (char*));
	mot2.string [0] = (char*) NULL;
	mot2.code = (int*) sxalloc (mot2.size + 1, sizeof (int));
	mot2.code [0] = 0;
	mot2.lgth = (int*) sxalloc (mot2.size + 1, sizeof (int));
	mot2.lgth [0] = 0;
	mot2.optim_kind = TIME; /* Priorite au temps sur l'espace */
	mot2.process_kind = PARTIEL; /* Seuls les prefixe et suffixe discriminants minimaux de chaque chaine sont entres ds l'automate */
	mot2.print_on_sxtty = FALSE; /* on travaille ds le silence */
	mot2.min = (int*) sxalloc (mot2.nb + 1, sizeof (int));
	mot2.max = (int*) sxalloc (mot2.nb + 1, sizeof (int));

	syno2code = (int*) sxalloc (nbndef+1, sizeof (int));
	kw_code2t_code = (int*) sxalloc (nbndef+1, sizeof (int));
	kw_code2t_code [0] = 0;
	kw_code2lgth = (int*) sxalloc (nbndef+1, sizeof (int));
	kw_code2lgth [0] = 0;

	/* Attention les mots cles sont entre double quotes */
	/* La longueur n'en tient pas compte */
	for (i = 1; i <= nbndef; i++) {
	    register int ate, l;

	    ate = keywords [i].string_table_entry;
	    mot2.string [i] = sxstrget (ate)+1;
	    mot2.lgth [i] = l = sxstrlen (ate) - 2;

	    if ((code = keywords [i].t_code) < 0) {
		/* synonym */
		syno2code [syno-termax] = -code;
		code = syno++;
		last_static_ste++;
		syno_kw_string_lgth += l;
		is_syno = TRUE;
	    }

	    mot2.code [i] = i;
	    kw_code2t_code [i] = code;
	    kw_code2lgth [i] = l;
	}

	if (nbndef > 1) {
	    sxdico_process (mot2, &dico);

	    if ((((unsigned int) (~0))>>dico.base_shift) < dico.max)
		sxtmsg ("%sToo many keywords, dictionary not produced\n", sxplocals.sxtables->err_titles [2] /* Error */);
	}

	check_kw = varstr_alloc (2500 /* partie fixe */ + 10 /* en moyenne */ * (nbndef+dico.max));

	cat_ap ("\n#include	\"sxdico.h\"\n");
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
	    cat_apnnl ("static int syno2t_code [");
	    cat_apnb (syno-termax-1);
	    cat_ap ("] = {");

	    for (i = termax+1; i < syno; i++) {
		if ((i-termax)%16 == 0)
		    cat_nl (1);

		cat_apnb (syno2code [i-termax]);
		cat_apnnl (", ");
	    }

	    cat_ap ("\n};");
	}

	cat_apnnl ("\nstatic int kw_code2t_code [");
	cat_apnb (nbndef+1);
	cat_ap ("] = {0,");

	for (i = 1; i <= nbndef; i++) {
	    if ((i)%16 == 0)
		cat_nl (1);

	    cat_apnb (kw_code2t_code [i]);
	    cat_apnnl (", ");
	}

	cat_ap ("\n};");

	cat_apnnl ("\nstatic int kw_code2lgth [");
	cat_apnb (nbndef+1);
	cat_ap ("] = {0,");

	for (i = 1; i <= nbndef; i++) {
	    if ((i)%16 == 0)
		cat_nl (1);

	    cat_apnb (kw_code2lgth [i]);
	    cat_apnnl (", ");
	}

	cat_ap ("\n};");


	if (nbndef > 1) {
	    unsigned char	class;

	    cat_apnnl ("\nstatic unsigned int comb_vector [");
	    cat_apnb (dico.max+1);
	    cat_ap ("] = {");

	    for (i = 0; (unsigned)i <= dico.max; i++) {
		if ((i % 10) == 9)
		    cat_nl (1);

		cat_apnb (dico.comb_vector [i]);
		cat_apnnl (", ");
	    }

	    cat_ap ("\n};");

	    cat_ap ("\nstatic unsigned char char2class [256] = {");

	    for (i = 0; i <= 255; i++) {
		if ((i % 8) == 7)
		    cat_nl (1);

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

	cat_ap ("\n#include	\"sxcheck_keyword.h\"");

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
	/* "check_keyword" est directement une fonction C */
	/* Attention les mots cles sont entre double quotes */
	/* La longueur n'en tient pas compte */
	for (i=1; i <= nbndef;i++) {
	    register struct kw *kw = kws + i;
	    register int ate, l;

	    ate = keywords [i].string_table_entry;
	    kw->str = (unsigned char *)sxstrget (ate);
	    kw->lgth = l = sxstrlen (ate) - 2;

	    if ((kw->code = keywords [i].t_code) < 0) {
		/* synonym */
		kw->code = -kw->code;
		kw->syno = syno++;
		last_static_ste++;
		syno_kw_string_lgth += l;
		is_syno = TRUE;
	    }
	    else
		kw->syno = kw->code;
	}

	sorted = (int*) sxalloc (nbndef + 1, sizeof (int));
	sorted_by_char = (int*) sxalloc (nbndef + 1, sizeof (int));
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
	cat_apnnl ("   *  on ");
	date_time = time (0);
	pct = ctime (&date_time);
	*(pct + 24) = '\0';
	cat_apnnl (pct);
	i = 24			/* pct */ + 9 /* "   *  on " */;
	cat_apnnl (blanks + (blanks_len - (70 - i)));
	cat_ap ("*");
	cat_ap ("   *  by the SYNTAX (*) lexical constructor LECL                      *");
	cat_ap ("   ********************************************************************");
	cat_ap ("   *  (*) SYNTAX is a trademark of INRIA.                             *");
	cat_ap ("   ******************************************************************** */");
	cat_nl (2);
	cat_ap (head);

	if (is_syno)
	    cat_ap ("   register int t_syno;");

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
	    gen (bi, bf - 1, lgth, 0, 0, 2);
	}

	cat_ap ("   default:\n\
      return 0;\n\
   }\n");

	if (is_compare) {
	    gen_cmp (couples, x_couple, FALSE);
	    cat_ap (tail);
	}

	if (is_compare_syno) {
	    gen_cmp (couples_syno, x_couple_syno, TRUE);
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
