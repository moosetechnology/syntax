/* ********************************************************
   *                                                      *
   * Copyright (c) 1985 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   ******************************************************** */



/* ********************************************************
   *     Produit de l'equipe Langages et Traducteurs.     *
   ******************************************************** */




/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 14-10-99 16:40 (phd) :	Utilisation de O_BINARY (pour Windows)	*/
/************************************************************************/
/* 14-10-99 16:39 (phd):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/

#include "sxunix.h"
#include "B_tables.h"
char WHAT_BNFWRITE[] = "@(#)SYNTAX - $Id: bnfwrite.c 529 2007-05-09 14:26:33Z rlacroix $" WHAT_DEBUG;


#define WRITE(f,p,l)		\
	if (p != NULL && (bytes = l) > 0 && (write (f, p, bytes) != bytes))	\
		goto write_error
#define BA_WRITE(f,b)	\
	if (!sxba_write (f, b))					\
		goto write_error


BOOLEAN		bnf_write (B, langname)
    register struct bnf_ag_item		*B;
    char	*langname;
{
    register int	bytes;
    register int	i;
    register int	/* file descriptor */ F_bnf;
    int		bt_version;
    char	bnf_name [128];
    static char		ME [] = "bnf_write";

    if ((F_bnf = open (strcat (strcpy (bnf_name, langname), ".bt"), O_WRONLY+O_CREAT+O_BINARY, 0666)) < 0) {
	fprintf (sxstderr, "%s: cannot open (create) ", ME);
	goto file_error;
    }

    bt_version = bt_num_version;
    /* numero de version des tables */

    WRITE (F_bnf, &(bt_version), sizeof (int));
    WRITE (F_bnf, &(B->bnf_modif_time), sizeof (long));
    WRITE (F_bnf, &(B->WS_TBL_SIZE), sizeof (struct ws_tbl_size_s));

    for (i = 0; i <= B->WS_TBL_SIZE.xnbpro + 1; i++)
	WRITE (F_bnf, B->WS_NBPRO + i, sizeof (struct ws_nbpro_s));

    for (i = 1; i <= B->WS_TBL_SIZE.indpro; i++)
	WRITE (F_bnf, B->WS_INDPRO + i, sizeof (struct ws_indpro_s));

    for (i = 1; i <= B->WS_TBL_SIZE.xtmax + B->WS_TBL_SIZE.tmax; i++)
	WRITE (F_bnf, B->XT_TO_T_PRDCT + i, sizeof (struct xv_to_v_prdct_s));

    for (i = 1; i <= B->WS_TBL_SIZE.xntmax - B->WS_TBL_SIZE.ntmax; i++)
	WRITE (F_bnf, B->XNT_TO_NT_PRDCT + i, sizeof (struct xv_to_v_prdct_s));

    for (i = 1; i <= B->WS_TBL_SIZE.xntmax + 1; i++)
	WRITE (F_bnf, B->WS_NTMAX + i, sizeof (struct ws_ntmax_s));

    WRITE (F_bnf, B->ADR - B->WS_TBL_SIZE.xtmax - 1, (B->WS_TBL_SIZE.xntmax + B->WS_TBL_SIZE.xtmax + 3) * sizeof (int));
    WRITE (F_bnf, B->TPD - B->WS_TBL_SIZE.xtmax - 1, (B->WS_TBL_SIZE.xtmax + 1) * sizeof (int));
    WRITE (F_bnf, B->NUMPD + 1, B->WS_TBL_SIZE.maxnpd * sizeof (int));
    WRITE (F_bnf, B->TNUMPD + 1, B->WS_TBL_SIZE.maxtpd * sizeof (int));

    /* WRITE (F_bnf, B->RULE_TO_REDUCE + 1, (B->WS_TBL_SIZE.xnbpro - B->WS_TBL_SIZE.actpro) * sizeof (int)); */
    if (B->WS_TBL_SIZE.xnbpro - B->WS_TBL_SIZE.actpro)
      /* Le 21/06/2002 */
      WRITE (F_bnf, B->RULE_TO_REDUCE, (1 + B->WS_TBL_SIZE.xnbpro - B->WS_TBL_SIZE.actpro) * sizeof (int));

    BA_WRITE (F_bnf, B->IS_A_GENERIC_TERMINAL);
    BA_WRITE (F_bnf, B->BVIDE);
    BA_WRITE (F_bnf, B->NULLABLE);

    for (i = 1; i <= B->WS_TBL_SIZE.ntmax; i++)
	BA_WRITE (F_bnf, B->FIRST [i]);

    for (i = 1; i <= -B->WS_TBL_SIZE.tmax; i++)
	BA_WRITE (F_bnf, B->FOLLOW [i]);

    WRITE (F_bnf, B->T_STRING, B->WS_TBL_SIZE.t_string_length * sizeof (char));
    WRITE (F_bnf, B->NT_STRING, B->WS_TBL_SIZE.nt_string_length * sizeof (char));
    close (F_bnf);
    return TRUE;

write_error:
    fprintf (sxstderr, "%s: write error on ", ME);
file_error:
    sxperror (bnf_name);
    return FALSE;
}
