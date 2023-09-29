#include "sxunix.h"
#include "tables.h"
#include "out.h"
#include "lig.h"
char WHAT_TABLESOUTLIGTBL[] = "@(#)SYNTAX - $Id: out_lig_tbl.c 585 2007-05-25 11:09:26Z rlacroix $" WHAT_DEBUG;

BOOLEAN		is_lig;

#define SECONDARY	0
#define PRIMARY		1

static int	*prdct_disp, prdct_disp_top, *prdct_code;
static char	**prdct_strings; 

static BOOLEAN lig_read (lig_read_prgentname)
    char	*lig_read_prgentname;
{
#define READ(f,p,l)						\
    if (bytes = (l), (read (f, p, bytes) != bytes))		\
	goto read_error

	    int			bytes;
    int 		F_lig;	/* file descriptor */
    static char		ME [] = "lig_read";
    int			lig_version, lgth, elem, x, y, n, d, bnf_rule_no, bnf_predicates_top;
    char		lig_name [32];
    int                 *bnf_disp;

    if ((F_lig = open (strcat (strcpy (lig_name, lig_read_prgentname), ".pat"), 0)) < 0) {
	fprintf (sxstderr, "%s: cannot open (read) ", ME);
	goto file_error;
    }

    READ (F_lig, &(lig_version), sizeof (int));

    if (lig_version != current_lig_version) {
	fprintf (sxstderr, "tables format has changed : please use the new lig2bnf\n");
	return (FALSE);
    }

    READ (F_lig, &(prdct_disp_top), sizeof (int));
    
    prdct_strings = (char**) sxalloc (prdct_disp_top, sizeof (char*)); 

    prdct_disp = (int*) sxalloc (prdct_disp_top + 1, sizeof (int));
    prdct_disp [0] = 0;
    prdct_disp [1] = 1;

    puts ("\n/********************* start of LIG tables ********************/");

    fputs ("static int SXLIG_prdct_or_act_disp [] = {\n 0, 1,", stdout);

    for (x = 2; x <= prdct_disp_top;x++)
    {
	if ((x&07) == 0)
	    putchar ('\n');

	READ (F_lig, prdct_disp + x, sizeof (int));
	printf (" %i,", prdct_disp [x]);
    }

    puts ("\n};\n");

    prdct_code = (int*) sxalloc (prdct_disp [prdct_disp_top], sizeof (int));
    prdct_code [0] = 0;

    prdct_strings [0] = "( )";

    puts ("static int SXLIG_prdct_or_act_code [] = {\n\
/* (&|@)10000 */ 0, /* ( ) */");

    for (x = 1; x < prdct_disp_top;x++)
    {
	y = prdct_disp [x];
	printf ("/* (&|@)%i */", 10000 + x);
	n = prdct_disp [x + 1] - y - 1;

	for (d = 0; d <= n; d++)
	{
	    READ (F_lig, &(elem), sizeof (int));
	    prdct_code [y + d] = elem;
	    printf (" %i,", elem);
	}

	READ (F_lig, &(lgth), sizeof (int));

	prdct_strings [x] = (char*) sxalloc (lgth + 1, sizeof (char));

	READ (F_lig, prdct_strings [x], lgth);
	prdct_strings [x] [lgth] = NUL;

	printf (" %s\n", prdct_strings [x]);
    }

    puts ("};\n");

    READ (F_lig, &(bnf_rule_no), sizeof (int));
    READ (F_lig, &(bnf_predicates_top), sizeof (int));

    bnf_disp = (int*) sxalloc (bnf_rule_no + 1, sizeof (int));
    
    fputs ("\nstatic int SXLIG_map_disp [] = {\n 0,", stdout);

    for (x = 1; x <= bnf_rule_no; x++)
    {
	if ((x&07) == 0)
	    putchar ('\n');

	READ (F_lig, bnf_disp + x, sizeof (int));
	printf (" %i,", bnf_disp [x] + 2);
    }

    puts ("\n};\n");
    
    puts ("\nstatic int SXLIG_map_code [] = {\n\
/* 0\t: */ -1, -1, -1,");

    for (x = 1; x < bnf_rule_no; x++)
    {
	y = bnf_disp [x];
	printf ("/* %i\t: */", x);
	n = bnf_disp [x + 1] - y - 1;

	for (d = 0; d <= n; d++)
	{
	    READ (F_lig, &(elem), sizeof (int));
	    printf (" %i,", elem);
	}

	fputs ("\n", stdout);
    }

    puts ("};\n");

    sxfree (bnf_disp);

    puts ("int PARSACT_2 ();\n");

    puts ("static struct sxligparsact sxligparsact = {\n SXLIG_prdct_or_act_disp,\n\
 SXLIG_prdct_or_act_code,\n\
 SXLIG_map_disp,\n\
 SXLIG_map_code,\n\
 PARSACT_2\n\
};");

    puts ("/********************* end of LIG tables ********************/");

    close (F_lig);
    return TRUE;

read_error:
    fprintf (sxstderr, "%s: read error on ", ME);
file_error:
    sxperror (lig_name);
    return FALSE;
}


static BOOLEAN check_couple (prdct1, prdct2)
    int prdct1, prdct2;
{
    /* Predicats due a la LIG, on verifie qu'ils sont exclusifs l'un de l'autre. */
    /* prdct1 != prdct2 */
    int		*bot1, *top1, *bot2, *top2, kind1, kind2;

    if (prdct1 == 0 || prdct2 == 0)
	/* () */
	return TRUE;

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
	    return TRUE;

	--top1, --top2;
      }

    if (top1 == bot1 && top2 == bot2)
	return FALSE;

    /* Longueur differente et partagent un suffixe */
    /* Type du plus long */

    /* ( .. a b c ) est disjoint de ( b c ) alors que
       ( .. b c ) et ( a b c ) ne le sont pas. */
    return ((top1 > bot1) ? kind1 : kind2) == PRIMARY;
}

static void check (prdct1, prdct2)
    int	prdct1, prdct2;
{
    if (!check_couple (prdct1, prdct2))
	fprintf (sxstderr, 
		 "Warning: since the LIG is deterministic the stack schemas\n\
%s and %s should be disjoint.\n",
		 prdct_strings [prdct1],
		 prdct_strings [prdct2]);
}

BOOLEAN out_lig_tables (language_name)
    char	*language_name;
{
    int		i, j, prdct, x, y;
    BOOLEAN	ret_val;

    if (sxverbosep)
	fputs ("\tLIG Tables\n", sxtty);

    ret_val = lig_read (language_name);

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
