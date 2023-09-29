/* ********************************************************
   *                                                      *
   * Copyright (c) 1985 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   ******************************************************** */



/* ********************************************************
   *     Produit de l'equipe Langages et Traducteurs.     *
   ******************************************************** */

static char	what [] = "@(#)bnf_read.c\t- SYNTAX [unix] - 3 Avril 1990";

#include "sxunix.h"
#include "B_tables.h"


#define READ(f,p,l)		\
	if (bytes = (l), (read (f, p, bytes) != bytes))		\
		goto read_error
#define BA_READ(f, b)	\
	if (!sxba_read (f,b))					\
		goto read_error


BOOLEAN		bnf_read (B, langname)
    register struct bnf_ag_item		*B;
    char	*langname;
{
    register int	bytes;
    register int	i;
    register int	/* file descriptor */ F_bnf;
    register int	tmax, xtmax, /* tmaxp1, */ xtmaxp1, xntmaxp1;
    int		bt_version;
    char	bnf_name [128];
    static char		ME [] = "bnf_read";

    if ((F_bnf = open (strcat (strcpy (bnf_name, langname), ".bt"), 0)) < 0) {
	fprintf (sxstderr, "%s: cannot open (read) ", ME);
	goto file_error;
    }

    READ (F_bnf, &(bt_version), sizeof (int));

    if (bt_version != bt_num_version) {
	fprintf (sxstderr, "tables format has changed : please use the new bnf\n");
	return (FALSE);
    }

    READ (F_bnf, &(B->bnf_modif_time), sizeof (long));
    READ (F_bnf, &(B->WS_TBL_SIZE), sizeof (struct ws_tbl_size_s));
    xntmaxp1 = B->WS_TBL_SIZE.xntmax + 1, /* tmaxp1 = */ (tmax = -B->WS_TBL_SIZE.tmax) + 1, xtmaxp1 = (xtmax = B->WS_TBL_SIZE.xtmax) + 1;

    {
	register struct ws_nbpro_s	*begin, *end;

	B->WS_NBPRO = begin = (struct ws_nbpro_s*) sxalloc (B->WS_TBL_SIZE.xnbpro + 2, sizeof (struct ws_nbpro_s));
	end = begin + B->WS_TBL_SIZE.xnbpro + 1;

	while (begin <= end)
	    READ (F_bnf, begin++, sizeof (struct ws_nbpro_s));
    }

    {
	register struct ws_indpro_s	*begin, *end;

	B->WS_INDPRO = begin = (struct ws_indpro_s*) sxalloc (B->WS_TBL_SIZE.indpro + 1, sizeof (struct ws_indpro_s));
	end = begin++ + B->WS_TBL_SIZE.indpro;

	while (begin <= end)
	    READ (F_bnf, begin++, sizeof (struct ws_indpro_s));
    }

    {
	register struct xv_to_v_prdct_s	*begin, *end;
	register int size = B->WS_TBL_SIZE.xtmax + B->WS_TBL_SIZE.tmax;

	B->XT_TO_T_PRDCT = size == 0 ? NULL : (struct xv_to_v_prdct_s*) sxalloc (size + 1, sizeof (struct xv_to_v_prdct_s));

	if (size > 0) {
	    begin = &(B->XT_TO_T_PRDCT [1]);
	    end = &(B->XT_TO_T_PRDCT [B->WS_TBL_SIZE.xtmax + B->WS_TBL_SIZE.tmax]);

	    while (begin <= end)
		READ (F_bnf, begin++, sizeof (struct xv_to_v_prdct_s));
	}
    }

    {
	register struct xv_to_v_prdct_s	*begin, *end;
	register int size = B->WS_TBL_SIZE.xntmax - B->WS_TBL_SIZE.ntmax;

	B->XNT_TO_NT_PRDCT = size == 0 ? NULL : (struct xv_to_v_prdct_s*) sxalloc (size + 1, sizeof (struct xv_to_v_prdct_s));

	if (size > 0) {
	    begin = &(B->XNT_TO_NT_PRDCT [1]);
	    end = &(B->XNT_TO_NT_PRDCT [B->WS_TBL_SIZE.xntmax - B->WS_TBL_SIZE.ntmax]);

	    while (begin <= end)
		READ (F_bnf, begin++, sizeof (struct xv_to_v_prdct_s));
	}
    }

    {
	register struct ws_ntmax_s	*begin, *end;

	B->WS_NTMAX = begin = (struct ws_ntmax_s*) sxalloc (xntmaxp1 + 1, sizeof (struct ws_ntmax_s));
	end = begin++ + xntmaxp1;

	while (begin <= end)
	    READ (F_bnf, begin++, sizeof (struct ws_ntmax_s));
    }

    READ (F_bnf, B->ADR = (int*) sxalloc (xntmaxp1 + xtmaxp1 + 1, sizeof (int)), (xntmaxp1 + xtmaxp1 + 1) * sizeof (int));
    B->ADR += xtmaxp1;
    READ (F_bnf, B->TPD = (int*) sxalloc (xtmaxp1, sizeof (int)), xtmaxp1 * sizeof (int));
    B->TPD += xtmaxp1;
    B->NUMPD = (int*) sxalloc (B->WS_TBL_SIZE.maxnpd + 1, sizeof (int));
    READ (F_bnf, B->NUMPD + 1, B->WS_TBL_SIZE.maxnpd * sizeof (int));
    B->TNUMPD = (int*) sxalloc (B->WS_TBL_SIZE.maxtpd + 1, sizeof (int));
    READ (F_bnf, B->TNUMPD + 1, B->WS_TBL_SIZE.maxtpd * sizeof (int));
    BA_READ (F_bnf, B->IS_A_GENERIC_TERMINAL = sxba_calloc (tmax + 1));
    BA_READ (F_bnf, B->BVIDE = sxba_calloc (B->WS_TBL_SIZE.ntmax + 1));
    B->FOLLOW = sxbm_calloc (tmax + 1, tmax + 1);

    for (i = 1; i <= tmax; i++)
	BA_READ (F_bnf, B->FOLLOW [i]);

    READ (F_bnf, B->T_STRING = (char*) sxalloc (B->WS_TBL_SIZE.t_string_length, sizeof (char)), B->WS_TBL_SIZE.
	 t_string_length * sizeof (char));
    READ (F_bnf, B->NT_STRING = (char*) sxalloc (B->WS_TBL_SIZE.nt_string_length, sizeof (char)), B->WS_TBL_SIZE.
	 nt_string_length * sizeof (char));
    close (F_bnf);
    return TRUE;

read_error:
    fprintf (sxstderr, "%s: read error on ", ME);
file_error:
    sxperror (bnf_name);
    return FALSE;
}
