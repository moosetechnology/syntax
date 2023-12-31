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
/* 20030520 10:37 (phd):	Modifs pour calmer le compilo SGI	*/
/************************************************************************/
/* 05-01-94 11:38 (pb):		On generait "case default:" dans le	*/
/*				switch de "language_smp"!!		*/
/************************************************************************/
/* 03-02-93 09:15 (pb):		Ajout de la generation du point d'entree*/
/*				SEMPASS pour l'e'valuation de sous-arbre*/
/*				en cours d'analyse syntaxique		*/
/************************************************************************/
/* 03-02-93 09:15 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/

/******************************************************************/
/*                                                                */
/*                                                                */
/*  This program has been translated from gen_at_pass_in_pl1.pl1  */
/*  on Thursday the twenty-fifth of September, 1986, at 10:02:10, */
/*  on the Multics system at INRIA,                               */
/*  by the release 3.3k PL1_C translator of INRIA,                */
/*         developped by the "Langages et Traducteurs" team,      */
/*         using the SYNTAX (*), FNC and Paradis systems.         */
/*                                                                */
/*                                                                */
/******************************************************************/
/* (*) SYNTAX is a trademark of INRIA.                            */
/******************************************************************/





#include "sxunix.h"
#include "B_tables.h"
#include "bnf_vars.h"
#include "T_tables.h"
char WHAT_SEMATGENAT[] = "@(#)SYNTAX - $Id: genat_c.c 587 2007-05-25 12:18:30Z rlacroix $" WHAT_DEBUG;

extern int      get_node_name_ref (int i);
extern int      delta_to_lispro (int x_prod, int delta);
int	max_line_length;
static BOOLEAN	inherited;
static SXBA	*node_to_red_set /* 1:T_nbnod, T_nbpro */ ;
static SXBA	b_used, prod_set, node_set;



static VOID	node_name_set (nt)
    int		nt;
{
    register int	j, k, m, n, lim1, lim2;

    lim1 = bnf_ag.WS_NTMAX [nt + 1].npg;

    for (m = bnf_ag.WS_NTMAX [nt].npg; m < lim1; m++) {
	n = bnf_ag.WS_NBPRO [m].numpg;

	if (!sxba_bit_is_set (b_used, n)) {
	    sxba_1_bit (b_used, n);

	    if (bnf_ag.WS_NBPRO [n].action != 0) {
		sxba_1_bit (node_set, get_node_name_ref (n));
	    }
	    else {
		lim2 = bnf_ag.WS_NBPRO [n + 1].prolon;

		for (j = bnf_ag.WS_NBPRO [n].prolon; j < lim2; j++) {
		    if ((k = bnf_ag.WS_INDPRO [j].lispro) > 0 /* [x]nt ou action*/ && !IS_A_PARSACT (k)) {
			node_name_set (XNT_TO_NT (k));
		    }
		}
	    }
	}
    }
}



static VOID	print_sons (T_ag, print_sons_prod_set, son, init_col)
    struct T_ag_item	*T_ag;
    SXBA	print_sons_prod_set;
    int		son, init_col;
{
    register int i, j, m, k;
    int		act, lim;
    BOOLEAN	is_set;
    register char *s;

    i = 0;

    while ((i = sxba_scan (print_sons_prod_set, i)) > 0) {
	lim = T_ag->SXT_node_info [i].T_ss_indx;
	m = 0;
	act = bnf_ag.WS_NBPRO [i].action;

	for (j = T_ag->SXT_node_info [i + 1].T_ss_indx - 1; j >= lim; j--) {
	    m++;

	    if (m == son || (act == 2 /* add to left list */  && m == 2) || (act == 3 /* add to right list */  && m == 1) ||
		 act == 6 /* list */ ) {
		k = bnf_ag.WS_INDPRO [delta_to_lispro (i, T_ag->T_stack_schema [j])].lispro;

		if (k < 0) {
		    sxba_1_bit (node_set, T_ag->T_ter_to_node_name [-XT_TO_T (k)]);
		}
		else  /* [x]nt ou action*/
		    if (!IS_A_PARSACT (k)) {
			sxba_empty (b_used);
			node_name_set (XNT_TO_NT (k));
		    }
	    }
	}
    }

    if ((i = sxba_scan (node_set, 0)) > 0) {
	printf ("/* VISITED->name = ");
	j = init_col + 19;

	if (sxba_cardinal (node_set) > 1) {
	    j++;
	    printf ("{");
	    is_set = TRUE;
	}
	else {
	    is_set = FALSE;
	}

	while (i > 0) {
	    k = strlen (s = T_ag->T_node_name_string + T_ag->T_nns_indx [i]) + 2 /* strlen ("_n") */;

	    if ((j += k) > max_line_length) {
		putchar ('\n');
		j = k;
	    }

	    printf ("%s_n", s);
	    sxba_0_bit (node_set, i);

	    if ((i = sxba_scan (node_set, i)) > 0) {
		if ((j += 2 /* strlen (", ") */) > max_line_length) {
		    putchar ('\n');
		    j = 2;
		}

		printf (", ");
	    }
	}

	if (is_set) {
	    if ((j += 1 /* strlen ("}") */) > max_line_length) {
		putchar ('\n');
		j = 1;
	    }

	    printf ("}");
	}

	if ((j += 3 /* strlen (" *\/") */) > max_line_length) {
	    putchar ('\n');
	    /* j = 3; inutile */
	}

	puts (" */");
    }
}



static SXVOID	gen_header ()
{
    printf ("\f\nstatic void %s_p%c () {\n\n/*\n%s\n*/\n\nswitch (VISITED->%sname) {\n\n", prgentname, inherited ? 'i' : 'd',
	 inherited ? "I N H E R I T E D" : "D E R I V E D", inherited ? "father->" : "");
}



static SXVOID	gen_trailer ()
{
    printf ("\n\n/*\nZ Z Z Z\n*/\n\ndefault:\nbreak;\n}\n/* end %s_p%c */\n}\n", prgentname, inherited ? 'i' : 'd');
}

SXVOID
genat_c (T_ag)
    struct T_ag_item	*T_ag;
{
    int		i, j, k, l;
    int		init_col;
    char	*s;

    node_to_red_set = sxbm_calloc (T_ag->T_constants.T_nbnod + 1, T_ag->T_constants.T_nbpro + 1);


/* generation du code  pour la semantique par arbre abstrait */

/* table inverse */

    for (j = 1; j <= T_ag->T_constants.T_nbpro; j++) {
	if ((i = get_node_name_ref (j)) != 0) {
	    sxba_1_bit (node_to_red_set [i], j);
	}
    }

    {
	register char	*p;
	char	genat_c_s [132];
	long	date_time;

	puts ("/* ********************************************************************");

	if ((p = sxrindex (sxsrcmngr.source_coord.file_name, '/')) == NULL)
	    p = sxsrcmngr.source_coord.file_name;
	else
	    p++;

	sprintf (genat_c_s, "   *  This program has been generated from %s", p);
	printf ("%-*s*\n", 70, genat_c_s);
	date_time = time (0);
	p = ctime (&date_time);
	*(p + 24) = '\0';
	sprintf (genat_c_s, "   *  on %s", p);
	printf ("%-*s*\n", 70, genat_c_s);
	puts ("   *  by the SYNTAX (*) abstract tree constructor SEMAT               *");
	puts ("   ********************************************************************");
	puts ("   *  (*) SYNTAX is a trademark of INRIA.                             *");
	puts ("   ******************************************************************** */\n\n\n");
    }

    printf ("\
/*   I N C L U D E S   */\n\
#define NODE struct %s_node\n\
#include \"sxunix.h\"\n\n\
struct %s_node {\n\
    SXNODE_HEADER_S VOID_NAME;\n\n\
/*\n\
your attribute declarations...\n\
*/\n\
};\n\
", prgentname, prgentname);

/* node name #define generation */

    puts ("/*\nN O D E   N A M E S\n*/");

    for (i = 1; i <= T_ag->T_constants.T_nbnod; i++) {
	printf ("#define %s_n %d\n", T_ag->T_node_name_string + T_ag->T_nns_indx [i], i);
    }

    puts ("/*\nE N D   N O D E   N A M E S\n*/\n");

/* pass inherited generation */

    b_used = sxba_calloc (bnf_ag.WS_TBL_SIZE.nbpro + 1);
    node_set = sxba_calloc (T_ag->T_constants.T_nbnod + 1);
    inherited = TRUE;
    gen_header ();

    for (i = 1; i <= T_ag->T_constants.T_nbnod; i++) {
	prod_set = node_to_red_set [i];

	for (j = sxba_scan (prod_set, 0); j != -1 && i != T_ag->SXT_node_info [j].T_node_name; j = sxba_scan (
	     prod_set, j)) {
	}

	if (j != -1) {
	    k = T_ag->SXT_node_info [j + 1].T_ss_indx - T_ag->SXT_node_info [j].T_ss_indx;

	    if ((k > 0 || bnf_ag.WS_NBPRO [j].action == 6) && bnf_ag.WS_NBPRO [j].action != 5) {
		/* prod_set contient toutes les productions de nom i */
		printf ("case %s_n :", s = T_ag->T_node_name_string + T_ag->T_nns_indx [i]);
		init_col = 9 + strlen (s);

		if (bnf_ag.WS_NBPRO [j].action == 6 /* list */ )
		    print_sons (T_ag, prod_set, 0, init_col);
		else if (k == 1) {
		    print_sons (T_ag, prod_set, 1, init_col);
		}
		else {
		    puts ("\n\tswitch (VISITED->position) {");

		    for (l = 1; l <= k; l++) {
			printf ("\tcase %d :", l);
			print_sons (T_ag, prod_set, l, 17 /* environ */);
			puts ("\tbreak;\n");
		    }

		    puts ("\t}\n");
		}

		puts ("break;\n");
	    }
	}
	else if (strcmp (T_ag->T_node_name_string + T_ag->T_nns_indx [i], "ERROR") == 0) {
		puts ("case ERROR_n :\nbreak;\n");
	}
    }

    sxfree (node_set);
    sxfree (b_used);
    gen_trailer ();

/* pass derived generation */

    inherited = FALSE;
    gen_header ();

    for (i = 1; i <= T_ag->T_constants.T_nbnod; i++) {
	printf ("case %s_n :\nbreak;\n\n", T_ag->T_node_name_string + T_ag->T_nns_indx [i]);
    }

    gen_trailer ();

/* tail generation */

    puts ("\f\nstatic void smpopen (sxtables_ptr)");
    puts ("struct sxtables *sxtables_ptr;");
    puts ("{");
    printf ("sxatcvar.atc_lv.node_size = sizeof (struct %s_node);\n", prgentname);
    puts ("}");
    puts ("\f\nstatic void smppass ()");
    puts ("{");
    puts ("\n/*   I N I T I A L I S A T I O N S   */");
    puts ("/* ........... */\n");
    puts ("/*   A T T R I B U T E S    E V A L U A T I O N   */");
    printf ("sxsmp (sxatcvar.atc_lv.abstract_tree_root, %s_pi, %s_pd);\n", prgentname, prgentname);
    puts ("\n/*   F I N A L I S A T I O N S   */");
    puts ("/* ........... */\n");
    puts ("}");
    printf ("\f\nint %s_smp (what, sxtables_ptr)\n", prgentname);
    puts ("int what;");
    puts ("struct sxtables *sxtables_ptr;");
    puts ("{");
    puts ("switch (what) {");
    puts ("case OPEN:");
    puts ("smpopen (sxtables_ptr);");
    puts ("break;");
    puts ("case SEMPASS: /* For [sub-]tree evaluation during parsing */");
    puts ("break;");
    puts ("case ACTION:");
    puts ("smppass ();");
    puts ("break;");
    puts ("case CLOSE:");
    puts ("break;");
    puts ("default:");
    puts ("break;");
    puts ("}");
    puts ("return 1;");
    puts ("}");
    sxbm_free (node_to_red_set);
}
