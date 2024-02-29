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

#include "sxunix.h"
#include "B_tables.h"
#include "xbnf_vars.h"

char WHAT_XBNFDUMP[] = "@(#)SYNTAX - $Id: xbnf_dump.c 3676 2024-02-05 18:17:45Z garavel $" WHAT_DEBUG;

void
xbnf_dump (void)
{
    SXINT		i;
    char	*ts;
    char	*format;

    fflush (stdout);
    ts = ctime (&(bnf_ag.bnf_modif_time));
    printf ("Date de derniere modification du fichier .bnf: %s\n\n", ts);
    printf ("\nW\n->nbpro:%ld\n->xnbpro:%ld\n->indpro:%ld\n->tmax:%ld\n->xtmax:%ld\n->ntmax:%ld\n->xntmax:%ld\n->maxtpd:%ld\n->maxnpd:%ld\n->t_string_length:%ld\n->nt_string_length:%ld\n->sem_kind:%ld\n", 
    (SXINT) W.nbpro, (SXINT) W.xnbpro, (SXINT) W.indpro, (SXINT) W.tmax, (SXINT) W.xtmax, (SXINT) W.ntmax, (SXINT) W.xntmax, (SXINT) W.maxtpd, (SXINT) W.maxnpd, (SXINT) W.t_string_length, (SXINT) W.nt_string_length, (SXINT) W.sem_kind);
    printf ("\nWN[i].\n  i prolo reduc numpg bpros  act\n");
    format = "%3d%6d%6d%6d%6d%6d%6d%4d\n";

    for (i = 0; i <= W.xnbpro + 1; i++)
	printf (format, i, WN [i].prolon, WN [i].reduc, WN [i].numpg, WN [i].bprosimpl, WN [i].action);

    format = "%12d%10d%10d\n";
    printf ("WI[i].     i       lispro    prolis\n");

    for (i = 1; i <= W.indpro; i++)
	printf (format, i, WI [i].lispro, WI [i].prolis);

    format = "%12d%10d%10d\n";
    printf ("\n XT[i].     i       t_code    prdct_no\n");

    for (i = -W.tmax + 1; i <= W.xtmax; i++)
	printf (format, i, bnf_ag.XT_TO_T_PRDCT [i + W.tmax].v_code, bnf_ag.XT_TO_T_PRDCT [i + W.tmax].prdct_no);

    format = "%12d%10d%10d\n";
    printf ("\n XNT[i].     i       nt_code    prdct_no\n");

    for (i = W.ntmax + 1; i <= W.xntmax; i++)
	printf (format, i, bnf_ag.XNT_TO_NT_PRDCT [i - W.ntmax].v_code, bnf_ag.XNT_TO_NT_PRDCT [i - W.ntmax].prdct_no);

    format = "%10d%10d%10d      %s\n";
    printf ("\n terminaux       adr       tpd      texte\n");

    for (i = 1; i <= W.xtmax; i++)
	printf (format, i, adr [-i], tpd [-i], t_string + adr [-i]);

    format = "%6d%10d\n";
    printf ("tnumpd\n");

    for (i = 1; i <= W.maxtpd; i++)
	printf (format, i, tnumpd [i]);

    format = "%10d%10d%13d%13d       %s\n";
    printf ("\n non_terminaux    adr       WX.npg       WX.npd      texte\n");

    for (i = 1; i <= W.xntmax; i++)
	printf (format, i, adr [i], WX [i].npg, WX [i].npd, nt_string + adr [i]);

    format = "%6d%10d\n";
    printf (" numpd\n");

    for (i = 1; i <= W.maxnpd; i++)
	printf (format, i, numpd [i]);

    printf ("\nis_a_generic_terminal\n");
    sxba_print (is_a_generic);
    printf ("\n");
    printf ("\nbvide\n");
    sxba_print (bvide);
    printf ("\n");
    printf ("\nfollow\n");

    for (i = 1; i <= -W.tmax; i++)
	sxba_print (follow [i]);

    fflush (stdout);
}
