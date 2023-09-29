/******************************************************************************
 *                                S Y N T A X
 *-----------------------------------------------------------------------------
 *   Copyright (C) 1972-2018 INRIA (Institut National de Recherche en
 *   Informatique et Automatique)
 *-----------------------------------------------------------------------------
 *   URL: http://syntax.gforge.inria.fr
 *-----------------------------------------------------------------------------
 *   The source code of SYNTAX is distributed with two different licenses,
 *   depending on the files:
 *   - The recursive content of src/ and incl/ and the non-recursive content
 *     of SYNTAX's root directory are distributed under the CeCILL-C license
 *   - The recursive content of all other repertories is distributed under
 *     the CeCILL license
 *   All code produced by SYNTAX must be considered as being under the
 *   CeCILL-C license. Information about the CeCILL and CeCILL-C licenses
 *   can be found at, e.g., http://www.cecill.fr
 *****************************************************************************/




#include "sxunix.h"
#include "B_tables.h"
char WHAT_BNFREAD[] = "@(#)SYNTAX - $Id: bnfread.c 2227 2018-09-26 16:22:59Z garavel $" WHAT_DEBUG;


#define READ(f,p,l)		\
	if (bytes = (l), (read (f, p, bytes) != bytes))		\
		goto read_error
#define BA_READ(f, b)	\
	if (!sxba_read (f,b))					\
		goto read_error


SXBOOLEAN		bnf_read (B, langname)
    struct bnf_ag_item		*B;
    char	*langname;
{
    SXINT	bytes;
    SXINT	i;
    sxfiledesc_t	/* file descriptor */ F_bnf;
    SXINT	tmax, xtmax, xntmax, ntmax;
    SXINT		bt_version;
    char	bnf_name [128];
    static char		ME [] = "bnf_read";

    if ((F_bnf = open (strcat (strcpy (bnf_name, langname), ".bt"), 0+O_BINARY)) < 0) {
	fprintf (sxstderr, "%s: cannot open (read) ", ME);
	goto file_error;
    }

    READ (F_bnf, &(bt_version), sizeof (SXINT));

    if (bt_version != bt_num_version) {
	fprintf (sxstderr, "tables format has changed : please use the new bnf\n");
	return (SXFALSE);
    }

    READ (F_bnf, &(B->bnf_modif_time), sizeof (long));
    READ (F_bnf, &(B->WS_TBL_SIZE), sizeof (struct ws_tbl_size_s));
    xntmax = B->WS_TBL_SIZE.xntmax, tmax = -B->WS_TBL_SIZE.tmax, xtmax = B->WS_TBL_SIZE.xtmax;

    {
	struct ws_nbpro_s	*begin, *end;

	B->WS_NBPRO = begin = (struct ws_nbpro_s*) sxalloc (B->WS_TBL_SIZE.xnbpro + 2, sizeof (struct ws_nbpro_s));
	end = begin + B->WS_TBL_SIZE.xnbpro + 1;

	while (begin <= end)
	    READ (F_bnf, begin++, sizeof (struct ws_nbpro_s));
    }

    {
	struct ws_indpro_s	*begin, *end;

	B->WS_INDPRO = begin = (struct ws_indpro_s*) sxalloc (B->WS_TBL_SIZE.indpro + 1, sizeof (struct ws_indpro_s));
	end = begin++ + B->WS_TBL_SIZE.indpro;

	while (begin <= end)
	    READ (F_bnf, begin++, sizeof (struct ws_indpro_s));
    }

    {
	struct xv_to_v_prdct_s	*begin, *end;
	SXINT size = xtmax + tmax;

	B->XT_TO_T_PRDCT = size == 0 ? NULL : (struct xv_to_v_prdct_s*) sxalloc (size + 1, sizeof (struct xv_to_v_prdct_s));

	if (size > 0) {
	    begin = &(B->XT_TO_T_PRDCT [1]);
	    end = &(B->XT_TO_T_PRDCT [B->WS_TBL_SIZE.xtmax + B->WS_TBL_SIZE.tmax]);

	    while (begin <= end)
		READ (F_bnf, begin++, sizeof (struct xv_to_v_prdct_s));
	}
    }

    {
	struct xv_to_v_prdct_s	*begin, *end;
	SXINT size = xntmax - (ntmax = B->WS_TBL_SIZE.ntmax);

	B->XNT_TO_NT_PRDCT = size == 0 ? NULL : (struct xv_to_v_prdct_s*) sxalloc (size + 1, sizeof (struct xv_to_v_prdct_s));

	if (size > 0) {
	    begin = &(B->XNT_TO_NT_PRDCT [1]);
	    end = &(B->XNT_TO_NT_PRDCT [B->WS_TBL_SIZE.xntmax - B->WS_TBL_SIZE.ntmax]);

	    while (begin <= end)
		READ (F_bnf, begin++, sizeof (struct xv_to_v_prdct_s));
	}
    }

    {
	struct ws_ntmax_s	*begin, *end;

	B->WS_NTMAX = begin = (struct ws_ntmax_s*) sxalloc (xntmax + 2, sizeof (struct ws_ntmax_s));
	end = begin++ + xntmax + 1;

	while (begin <= end)
	    READ (F_bnf, begin++, sizeof (struct ws_ntmax_s));
    }

    READ (F_bnf, B->ADR = (SXINT*) sxalloc (xntmax + 1 + xtmax + 1 + 1, sizeof (SXINT)), (xntmax + 1 + xtmax + 1 + 1) * sizeof (SXINT));
    B->ADR += xtmax + 1;
    READ (F_bnf, B->TPD = (SXINT*) sxalloc (xtmax + 1, sizeof (SXINT)), (xtmax + 1) * sizeof (SXINT));
    B->TPD += xtmax + 1;
    READ (F_bnf, (B->NUMPD = (SXINT*) sxalloc (B->WS_TBL_SIZE.maxnpd + 1, sizeof (SXINT))) + 1, B->WS_TBL_SIZE.maxnpd * sizeof (SXINT));
    READ (F_bnf, (B->TNUMPD = (SXINT*) sxalloc (B->WS_TBL_SIZE.maxtpd + 1, sizeof (SXINT))) + 1, B->WS_TBL_SIZE.maxtpd * sizeof (SXINT));

    if ((i = B->WS_TBL_SIZE.xnbpro - B->WS_TBL_SIZE.actpro) > 0) {
      /* Le 21/06/2002
	 READ (F_bnf, (B->RULE_TO_REDUCE = (SXINT*) sxalloc (i + 1, sizeof (SXINT))) + 1, i * sizeof (SXINT)); */
      READ (F_bnf, (B->RULE_TO_REDUCE = (SXINT*) sxalloc (i + 1, sizeof (SXINT))), (i+1) * sizeof (SXINT));
    }
    else
	B->RULE_TO_REDUCE = NULL;

    BA_READ (F_bnf, B->IS_A_GENERIC_TERMINAL = sxba_calloc (tmax + 1));
    BA_READ (F_bnf, B->BVIDE = sxba_calloc (xntmax + 1));
    BA_READ (F_bnf, B->NULLABLE = sxba_calloc (B->WS_TBL_SIZE.indpro + 1));
    B->FIRST = sxbm_calloc (ntmax + 1, xtmax + 1);

    for (i = 1; i <= ntmax; i++)
	BA_READ (F_bnf, B->FIRST [i]);

    B->FOLLOW = sxbm_calloc (tmax + 1, tmax + 1);

    for (i = 1; i <= tmax; i++)
	BA_READ (F_bnf, B->FOLLOW [i]);

    READ (F_bnf, B->T_STRING = (char*) sxalloc (B->WS_TBL_SIZE.t_string_length, sizeof (char)), B->WS_TBL_SIZE.
	 t_string_length * sizeof (char));
    READ (F_bnf, B->NT_STRING = (char*) sxalloc (B->WS_TBL_SIZE.nt_string_length, sizeof (char)), B->WS_TBL_SIZE.
	 nt_string_length * sizeof (char));
    close (F_bnf);
    return SXTRUE;

read_error:
    fprintf (sxstderr, "%s: read error on ", ME);
file_error:
    sxperror (bnf_name);
    return SXFALSE;
}
