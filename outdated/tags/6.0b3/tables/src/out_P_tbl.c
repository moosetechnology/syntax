/* ********************************************************
   *							  *
   *							  *
   * Copyright (c) 2000 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *							  *
   *							  *
   ******************************************************** */




/* ********************************************************
   *							  *
   *  Produit de l'�quipe Langages et Traducteurs.        *
   *							  *
   ******************************************************** */





/* Sortie des tables de l'analyseur syntaxique produit par SYNTAX */

/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20040211 16:42 (phd):	Modif pour calmer le compilo HPUX	*/
/************************************************************************/
/* 20030523 10:41 (phd):	Ajout de l'�l�ment -1 de reductions	*/
/*	t_bases nt_bases P_lregle P_right_ctxt_head SXP_local_mess	*/
/************************************************************************/
/* 20030520 10:37 (phd):	Modifs pour calmer le compilo SGI	*/
/************************************************************************/
/* 20000411 16:49 (phd):	Ajout du �copyright�			*/
/************************************************************************/
/* 20000411 16:03 (phd):	Ajout de cette rubrique �modifications�	*/
/************************************************************************/

#include "sxunix.h"
#include "tables.h"
#include "out.h"
char WHAT_TABLESOUTPTBL[] = "@(#)SYNTAX - $Id: out_P_tbl.c 1195 2008-03-11 13:26:11Z rlacroix $" WHAT_DEBUG;

extern BOOLEAN		is_lig;

static char	FMT_4_DIGITS [] = "{%d, %d, %d, %d},\n";
static char	FMT_3_DIGITS [] = "{%d, %d, %d},\n";
static char	FMT_2_DIGITS [] = "{%d, %d},\n";



static char	*copy (char *s1, char *s2, long l)
{
    strncpy (s1, s2, l);
    *(s1 + l) = '\0';
    return s1;
}



static VOID	out_reductions (void)
{
    SXINT	i;

    if (PR == NULL)
	return;

    puts ("static struct SXP_reductions reductions[]={{0,0,0,0},");

    for (i = 1; i <= -PC.mMred; i++) {
	printf (FMT_4_DIGITS, PR [i].action, PR [i].reduce, PR [i].lgth, PR [i].lhs);
    }

    out_end_struct ();
}



static VOID	out_t_bases (void)
{
    SXINT	i;

    if (PT == NULL)
	return;

    puts ("static struct SXP_bases t_bases[]={{0,0,0},");

    for (i = 1; i <= PC.Mtstate; i++) {
	printf (FMT_3_DIGITS, PT [i].defaut, PT [i].commun, PT [i].propre);
    }

    out_end_struct ();
}



static VOID	out_nt_bases (void)
{
    SXINT	i;

    if (PNT == NULL)
	return;

    puts ("static struct SXP_bases nt_bases[]={{0,0,0},");

    for (i = 1; i <= PC.Mntstate; i++) {
	printf (FMT_3_DIGITS, PNT [i].defaut, PNT [i].commun, PNT [i].propre);
    }

    out_end_struct ();
}



static VOID	out_vector (void)
{
    SXINT	i;

    if (PV == NULL)
	return;

    puts ("static struct SXP_item vector[]={");

    for (i = 1; i <= PC.Mvec; i++) {
	printf (FMT_2_DIGITS, PV [i].check, PV [i].action);
    }

    out_end_struct ();
}



#if 0
static VOID	out_gotos (void)
{
    SXINT	i;

    if (PGO == NULL)
	return;

    puts ("static struct SXP_goto gotos[]={");

    for (i = 1; i <= PC.Mgotos; i++) {
	printf (FMT_2_DIGITS, PGO [i].state, PGO [i].next);
    }

    out_end_struct ();
}
#endif


static VOID	out_prdcts (void)
{
    SXINT	i, prdct;

    if (PPR == NULL)
	return;

    puts ("static struct SXP_prdct prdcts[]={");

    for (i = 1; i <= PC.Mprdct + PC.mMred; i++) {
	prdct = PPR [i].prdct;

	if (prdct >= 10000 && prdct < 20000)
	    is_lig = TRUE;

	printf (FMT_2_DIGITS, prdct, PPR [i].action);
    }

    out_end_struct ();
}



static VOID	out_germe_to_gr_act (void)
{
    SXINT	i;

    if (PGA == NULL)
	return;

    puts ("static SXSHORT germe_to_gr_act[]={");

    for (i = 0; i < PC.Mgerme - PC.mgerme; i++) {
	printf ("%ld,\n", (long)PGA [i]);
    }

    out_end_struct ();
}

static VOID	out_prdct_list (void)
{
    SXINT	i;

    if (P.prdct_list == NULL)
	return;

    puts ("static struct SXP_prdct_list prdct_list[]={");

    for (i = 1; i <= PC.Mprdct_list; i++) {
	printf (FMT_3_DIGITS,
		P.prdct_list [i].red_no, P.prdct_list [i].pos, P.prdct_list [i].prdct);
    }

    out_end_struct ();
}



static VOID	out_P_lregle (void)
{
    SXINT	j, i;

    out_int ("P_lrgl", "");

    for (i = 1; i <= RC.P_nbcart; i++) {
	for (j = 0; j <= RC.P_maxlregle; j++) {
	    printf ("%ld,", (long)R.P_lregle [i] [j]);
	}

	putchar (NEWLINE);
    }

    out_end_struct ();
    out_int ("*P_lregle", P0);

    for (i = 0; i < RC.P_nbcart; i++) {
	printf ("&P_lrgl[%ld],\n", (long)(i * (RC.P_maxlregle + 1)));
    }

    out_end_struct ();
}



static VOID	out_P_right_ctxt_head (void)
{
    out_tab_int ("P_right_ctxt_head", R.P_right_ctxt_head, 1, RC.P_nbcart, "0,");
}



static VOID	out_P_local_mess (void)
{
    SXINT	j, i, param, lg;

    out_int ("P_param_ref", "");

    for (i = 1; i <= RC.P_nbcart + 1; i++) {
	for (j = 1; j <= PL [i].param_no; j++) {
	    printf ("%ld,", (long)PL [i].string_ref [j].param);
	}

	putchar (NEWLINE);
    }

    out_end_struct ();
    out_struct ("SXP_local_mess", "{0,NULL,NULL},");

    for (param = 0, i = 1; i <= RC.P_nbcart + 1; param += PL [i++].param_no) {
	printf ("{%ld,\"", (long)PL [i].param_no);

	for (j = 0; j <= PL [i].param_no; j++) {
	    fputs ("%s", stdout);

	    if ((lg = PL [i].string_ref [j].length) != 0) {
	      /* LINTED this cast form long to int is needed by printf() */
		printf ("%.*s", (int)lg, R.P_string_mess + PL [i].string_ref [j].index);
	    }
	}

	printf ("\",&P_param_ref[%ld]},\n", (long)param);
    }

    out_end_struct ();
}



static VOID	out_P_global_mess (void)
{
    SXINT	i, j, lg;
    char	*m, *prefixe, *motif;
    /* chaines */
    char	*dmotif, *fmotif, *suffixe;
    /* pointeurs */

    out_char ("*P_global_mess", "");
    lg = PG [1].length;
    m = (char*) sxalloc (lg + 1, sizeof (char));
    copy (m, R.P_string_mess + PG [1].index, (long) lg);

/* recherche ^( */

    dmotif = sxindex (m, '^');

    if ((dmotif != NULL) && (*(dmotif + 1) == '(')) {
	/* on a trouve ^(, on recherche ^) */
	fmotif = sxrindex (m, '^');

/* on a trouve ^), on alloue et on copie */

	prefixe = (char*) sxalloc (lg + 1, sizeof (char));
	copy (prefixe, m, dmotif - m);
	motif = (char*) sxalloc (lg + 1, sizeof (char));
	copy (motif, dmotif + 2, fmotif - dmotif - 2);
	suffixe = fmotif + 2;


/* on cree : prefixe motif* suffixe */

	for (i = 1; i <= RC.P_followers_number; i++) {
	    printf ("\"%%s%s", prefixe);

	    for (j = 1; j < i; j++)
		fputs (motif, stdout);

	    printf ("%s\",\n", suffixe);
	}


/* on libere */

	sxfree (motif);
	sxfree (prefixe);
    }
    else {
	for (i = 1; i <= RC.P_followers_number; i++) {
	    printf ("\"%%s%s\",\n", m);
	}
    }

    for (i = 2; i <= 5; i++) {
	if (i == 4)
	    if ((lg = PG [i].length) != 0)
	        /* LINTED this cast form long to int is needed by printf() */
		printf ("\"%.*s\",\n", (int)lg, R.P_string_mess + PG [i].index);
	    else
		puts ("\"\",");
	else if ((lg = PG [i].length) != 0) 
	    /* LINTED this cast form long to int is needed by printf() */
	    printf ("\"%%s%.*s\",\n", (int)lg, R.P_string_mess + PG [i].index);
	else
	    puts ("\"%s\",");
    }

    out_end_struct ();
    sxfree (m);
}



static VOID	out_err_titles (void)
{
    SXINT	i;

    puts ("\nstatic char *err_titles[SXSEVERITIES]={");

    for (i = 1; i <= RC.E_nb_kind; i++) {
        /* LINTED this cast form long to int is needed by printf() */
	printf ("\"\\%03o%s\",\n", (int)i - 1, R.E_titles [i]);
    }

    out_end_struct ();
}



static VOID	out_abstract (void)
{
    if (RC.E_labstract != 0)
	printf ("static char abstract []= \"%s\";\n", R.E_abstract);
}


SXVOID
out_P_tables (void)
{
    if (sxverbosep)
	fputs ("\tParser Tables\n", sxtty);

    out_reductions ();
    out_t_bases ();
    out_nt_bases ();
    out_vector ();
#if 0
    out_gotos ();
#endif
    out_prdcts ();
    out_germe_to_gr_act ();
    out_prdct_list ();

    if (R.PER_tset != NULL
	&& sxba_scan (R.PER_tset, 0) > 0)
	out_rcvr_trans ();

    if (RC.P_nbcart != 0) {
	out_P_lregle ();
	out_P_right_ctxt_head ();
    }

    out_P_local_mess ();

    if (RC.P_nbcart != 0) {
	out_bit ("P_no_delete", R.P_no_delete);
	out_bit ("P_no_insert", R.P_no_insert);
    }

    out_P_global_mess ();
    out_bit ("PER_tset", R.PER_tset);
    out_err_titles ();
    out_abstract ();

    if (PPR != NULL || PC.xnbpro > PC.nbpro)
	/* P_is_user_action_or_predicate */
	out_ext_int_newstyle ("PARSACT(SXINT what, SXINT action_no)");

    out_ext_BOOLEAN_newstyle (RC.P_nbcart == 0
		     ? (PC.nd_degree >= 0 ? "sxndpsrecovery(SXINT what)" : "sxpsrecovery(SXINT what, SXINT *at_state, SXINT latok_no)")
		     : (PC.nd_degree >= 0 ? "sxndprecovery(SXINT what_to_do)" : "sxprecovery (SXINT what_to_do, SXINT *at_state, SXINT latok_no)"));
}

SXINT max_P_tables (void)
{
    /* retourne le max de la valeur absolue de toutes les valeurs utilisees ds les tables du parser */
    SXINT i;
#define TROUVE_MAX_ABS(x)	if(((v=(x))<0?v=-v:v)>max) max=v;else ((void)0)
    SXINT	max = 0, v;

    if (PR != NULL)
	for (i = 1; i <= -PC.mMred; i++) {
	    TROUVE_MAX_ABS(PR [i].reduce);
	    TROUVE_MAX_ABS(PR [i].lhs);
	}

    if (PT != NULL)
	for (i = 1; i <= PC.Mtstate; i++) {
	    TROUVE_MAX_ABS(PT [i].commun);
	    TROUVE_MAX_ABS(PT [i].propre);
	}

    if (PNT != NULL)
	for (i = 1; i <= PC.Mntstate; i++) {
	    TROUVE_MAX_ABS(PNT [i].commun);
	    TROUVE_MAX_ABS(PNT [i].propre);
	}

    if (PV != NULL)
	for (i = 1; i <= PC.Mvec; i++) {
	    TROUVE_MAX_ABS(PV [i].check);
	    TROUVE_MAX_ABS(PV [i].action);
	}

#if 0
    if (PGO != NULL)
	for (i = 1; i <= PC.Mgotos; i++) {
	    TROUVE_MAX_ABS(PGO [i].state);
	    TROUVE_MAX_ABS(PGO [i].next);
	}
#endif

    if (PPR != NULL)
	for (i = 1; i <= PC.Mprdct + PC.mMred; i++) {
	    TROUVE_MAX_ABS(PPR [i].prdct);
	    TROUVE_MAX_ABS(PPR [i].action);
	}

    return max;
}
