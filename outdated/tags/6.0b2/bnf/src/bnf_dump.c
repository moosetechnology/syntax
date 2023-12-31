/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1987 by Institut National de Recherche *
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
/* 20030512 13:07 (phd):	Adaptation � SGI (64 bits)		*/
/************************************************************************/
/* 10-04-90 14:30 (phd):	Suppression des "typedef" de bl		*/
/*				Ajout de cette rubrique "modifications"	*/
/************************************************************************/


#include "sxunix.h"
#include "B_tables.h"
#include "bnf_vars.h"
char WHAT_BNFDUMP[] = "@(#)SYNTAX - $Id: bnf_dump.c 580 2007-05-24 12:45:54Z rlacroix $" WHAT_DEBUG;

SXVOID
bnf_dump ()
{
    int		i;
    char	*ts;
    char	*format;

    fflush (stdout);
    ts = ctime (&(bnf_ag.bnf_modif_time));
    printf ("Date de derniere modification du fichier .bnf: %s\n\n", ts);
    printf (
"\nW\n->nbpro:%d\n->xnbpro:%d\n->indpro:%d\n->tmax:%d\n->xtmax:%d\n->ntmax:%d\n->xntmax:%d\n->maxtpd:%d\n->maxnpd:%d\n->t_string_length:%d\n->nt_string_length:%d\n->sem_kind:%d\n"
	 , W.nbpro, W.xnbpro, W.indpro, W.tmax, W.xtmax, W.ntmax, W.xntmax, W.maxtpd, W.maxnpd, W.t_string_length, W.nt_string_length, W.sem_kind);
    printf ("\nWN[i].\n     i  prolo  reduc  numpg  bpros   act\n");
    format = "%6d %6d %6d %6d %6d %6d\n";

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
