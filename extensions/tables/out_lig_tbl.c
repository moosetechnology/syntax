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
#include "tables.h"
#include "out.h"
#include "lig.h"

char WHAT_TABLESOUTLIGTBL[] = "@(#)SYNTAX - $Id: out_lig_tbl.c 3650 2023-12-23 07:32:10Z garavel $" WHAT_DEBUG;

extern bool		is_lig;

#define SECONDARY	0
#define PRIMARY		1

static SXINT	*prdct_disp, prdct_disp_top, *prdct_code;
static char	**prdct_strings; 

static bool lig_read (char *lig_read_prgentname)
{
#define READ(f,p,l)						\
    if (bytes = (l), (read (f, p, bytes) != bytes))		\
	goto read_error

    SXINT		bytes;
    sxfiledesc_t 		F_lig;	/* file descriptor */
    static char		ME [] = "lig_read";
    SXINT			lig_version, lgth, elem, x, y, n, d, bnf_rule_no, bnf_predicates_top;
    char		lig_name [32];
    SXINT                 *bnf_disp;

    if ((F_lig = open (strcat (strcpy (lig_name, lig_read_prgentname), ".pat"), 0)) < 0) {
	fprintf (sxstderr, "%s: cannot open (read) ", ME);
	goto file_error;
    }

    READ (F_lig, &(lig_version), sizeof (SXINT));

    if (lig_version != current_lig_version) {
	fprintf (sxstderr, "tables format has changed : please use the new lig2bnf\n");
	return (false);
    }

    READ (F_lig, &(prdct_disp_top), sizeof (SXINT));
    
    prdct_strings = (char**) sxalloc (prdct_disp_top, sizeof (char*)); 

    prdct_disp = (SXINT*) sxalloc (prdct_disp_top + 1, sizeof (SXINT));
    prdct_disp [0] = 0;
    prdct_disp [1] = 1;

    puts ("\n/********************* start of LIG tables ********************/");

    fputs ("static SXINT SXLIG_prdct_or_act_disp [] = {\n 0, 1,", stdout);

    for (x = 2; x <= prdct_disp_top;x++)
    {
	if ((x&07) == 0)
	    putchar ('\n');

	READ (F_lig, prdct_disp + x, sizeof (SXINT));
	printf (" %ld,", (SXINT) prdct_disp [x]);
    }

    puts ("\n};\n");

    prdct_code = (SXINT*) sxalloc (prdct_disp [prdct_disp_top], sizeof (SXINT));
    prdct_code [0] = 0;

    prdct_strings [0] = "( )";

    puts ("static SXINT SXLIG_prdct_or_act_code [] = {\n\
/* (&|@)10000 */ 0, /* ( ) */");

    for (x = 1; x < prdct_disp_top;x++)
    {
	y = prdct_disp [x];
	printf ("/* (&|@)%ld */", (SXINT) 10000 + x);
	n = prdct_disp [x + 1] - y - 1;

	for (d = 0; d <= n; d++)
	{
	    READ (F_lig, &(elem), sizeof (SXINT));
	    prdct_code [y + d] = elem;
	    printf (" %ld,", (SXINT) elem);
	}

	READ (F_lig, &(lgth), sizeof (SXINT));

	prdct_strings [x] = (char*) sxalloc (lgth + 1, sizeof (char));

	READ (F_lig, prdct_strings [x], lgth);
	prdct_strings [x] [lgth] = SXNUL;

	printf (" %s\n", prdct_strings [x]);
    }

    puts ("};\n");

    READ (F_lig, &(bnf_rule_no), sizeof (SXINT));
    READ (F_lig, &(bnf_predicates_top), sizeof (SXINT));

    bnf_disp = (SXINT*) sxalloc (bnf_rule_no + 1, sizeof (SXINT));
    
    fputs ("\nstatic SXINT SXLIG_map_disp [] = {\n 0,", stdout);

    for (x = 1; x <= bnf_rule_no; x++)
    {
	if ((x&07) == 0)
	    putchar ('\n');

	READ (F_lig, bnf_disp + x, sizeof (SXINT));
	printf (" %ld,", (SXINT) bnf_disp [x] + 2);
    }

    puts ("\n};\n");
    
    puts ("\nstatic SXINT SXLIG_map_code [] = {\n\
/* 0\t: */ -1, -1, -1,");

    for (x = 1; x < bnf_rule_no; x++)
    {
	y = bnf_disp [x];
	printf ("/* %ld\t: */", (SXINT) x);
	n = bnf_disp [x + 1] - y - 1;

	for (d = 0; d <= n; d++)
	{
	    READ (F_lig, &(elem), sizeof (SXINT));
	    printf (" %ld,", (SXINT) elem);
	}

	fputs ("\n", stdout);
    }

    puts ("};\n");

    sxfree (bnf_disp);

    puts ("SXINT PARSACT_2 (SXINT what, ...);\n");

    puts ("static struct sxligparsact sxligparsact = {\n SXLIG_prdct_or_act_disp,\n\
 SXLIG_prdct_or_act_code,\n\
 SXLIG_map_disp,\n\
 SXLIG_map_code,\n\
 PARSACT_2\n\
};");

    puts ("/********************* end of LIG tables ********************/");

    close (F_lig);
    return true;

read_error:
    fprintf (sxstderr, "%s: read error on ", ME);
file_error:
    sxperror (lig_name);
    return false;
}


static bool check_couple (SXINT prdct1, SXINT prdct2)
{
    /* Predicats due a la LIG, on verifie qu'ils sont exclusifs l'un de l'autre. */
    /* prdct1 != prdct2 */
    SXINT		*bot1, *top1, *bot2, *top2, kind1, kind2;

    if (prdct1 == 0 || prdct2 == 0)
	/* () */
	return true;

    top1 = prdct_code + prdct_disp [prdct1 + 1] - 2;
    top2 = prdct_code + prdct_disp [prdct2 + 1] - 2;
    bot1 = prdct_code + prdct_disp [prdct1];
    bot2 = prdct_code + prdct_disp [prdct2];
    kind1 = *bot1;
    kind2 = *bot2;
	/* On saute kind, lhs_index, @lgth et on pointe sur common_prefix_lgth juste
	   devant la liste des elements. */
    bot1 += 3;
    bot2 += 3;
    
    while (top1 > bot1 && top2 > bot2)
      {
	if (*top1 != *top2)
	    return true;

	--top1, --top2;
      }

    if (top1 == bot1 && top2 == bot2)
	return false;

    /* Longueur differente et partagent un suffixe */
    /* Type du plus long */

    /* ( .. a b c ) est disjoint de ( b c ) alors que
       ( .. b c ) et ( a b c ) ne le sont pas. */
    return ((top1 > bot1) ? kind1 : kind2) == PRIMARY;
}

static void check (SXINT prdct1, SXINT prdct2)
{
    if (!check_couple (prdct1, prdct2))
	fprintf (sxstderr, 
		 "Warning: since the LIG is deterministic the stack schemas\n\
%s and %s should be disjoint.\n",
		 prdct_strings [prdct1],
		 prdct_strings [prdct2]);
}

bool out_lig_tables (char *ln)
{
    SXINT		i, j, prdct, x, y;
    bool	ret_val;

    if (sxverbosep)
	fputs ("\tLIG Tables\n", sxtty);

    ret_val = lig_read (ln);

    if (PC.nd_degree < 0)
    {
	/* La grammaire provient d'une LIG (par lig2bnf) et l'analyseur est deterministe.
	   On verifie que les predicats (provenant de la LIG, i.e. > 10000) sont
	   exclusifs les uns des autres. */

	for (i = 1; i <= PC.Mprdct + PC.mMred; i++) {
	    prdct = PPR [i].prdct;

	    j = i;

	    while (PPR [++i].prdct >= 0); /* &Else == -1 */

	    if (prdct > 10000 && i > j + 1)
	    {
		for (x = j; x < i - 1; x++)
		    for (y = x + 1; y < i; y++)
			check (PPR [x].prdct - 10000, PPR [y].prdct - 10000);
	    }
	    
	    i = j;
	}
    }

    if (prdct_disp != NULL)
	sxfree (prdct_disp);

    if (prdct_code != NULL)
	sxfree (prdct_code);

    if (prdct_strings != NULL)
    {
	for (x = 2; x < prdct_disp_top;x++)
	    sxfree (prdct_strings [x]);

	sxfree (prdct_strings);
    }

    return ret_val;
}
