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
#include "B_tables.h"
#include "bnf_vars.h"
#include "T_tables.h"

char WHAT_SEMATGENAT[] = "@(#)SYNTAX - $Id: genat_c.c 4166 2024-08-19 09:00:49Z garavel $" WHAT_DEBUG;

extern SXINT      get_node_name_ref (SXINT i);
extern SXINT      delta_to_lispro (SXINT x_prod, SXINT delta);
extern SXINT	max_line_length;
static bool	inherited;
static SXBA	*node_to_red_set /* 1:T_nbnod, T_nbpro */ ;
static SXBA	b_used, prod_set, node_set;

static void	node_name_set (SXINT nt)
{
    SXINT	j, k, m, n, lim1, lim2;

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



static void	print_sons (struct T_ag_item *T_ag,
			    SXBA print_sons_prod_set,
			    SXINT son, 
			    SXINT init_col)
{
    SXINT i, j, m, k;
    SXINT		act, lim;
    bool	is_set;
    char *s;

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
	printf ("/* SXVISITED->name = ");
	j = init_col + 19;

	if (sxba_cardinal (node_set) > 1) {
	    j++;
	    printf ("{");
	    is_set = true;
	}
	else {
	    is_set = false;
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



static void	gen_header (void)
{
    printf ("\nstatic void %s_p%c (void) {\n\n/*\n%s\n*/\n\nswitch (SXVISITED->%sname) {\n\n", prgentname, inherited ? 'i' : 'd',
	 inherited ? "I N H E R I T E D" : "D E R I V E D", inherited ? "father->" : "");
}



static void	gen_trailer (void)
{
    printf ("\n\n/*\nZ Z Z Z\n*/\n\ndefault:\nbreak;\n}\n/* end %s_p%c */\n}\n", prgentname, inherited ? 'i' : 'd');
}

static void indent(SXINT indentation)
{
  SXINT j;
  printf("\n");
  for ( j=0 ; j<indentation ; j++)
    printf("\t");
}
  

static void gen_default_switch_case (SXINT indentation)
{
  indent (indentation);
  printf ("default:");

  indent (indentation);
  printf ("break;\n");
}


void
genat_c (struct T_ag_item *T_ag)
{
    SXINT		i, j, k, l;
    SXINT		init_col;
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
	char	*p;
	char	genat_c_s [132];

	puts ("/* ********************************************************************");

	if ((p = sxrindex (sxsrcmngr.source_coord.file_name, '/')) == NULL)
	    p = sxsrcmngr.source_coord.file_name;
	else
	    p++;

	sprintf (genat_c_s, "   *  This program has been generated from %s", p);
	printf ("%-*s*\n", 70, genat_c_s);
	puts ("   *  by the SYNTAX (*) abstract tree constructor SEMAT               *");
	puts ("   ********************************************************************");
	puts ("   *  (*) SYNTAX is a trademark of INRIA.                             *");
	puts ("   ******************************************************************** */\n\n\n");
    }

    printf ("\
/*   I N C L U D E S   */\n\
#define SXNODE struct %s_node\n\
#include \"sxunix.h\"\n\n\
struct %s_node {\n\
    SXNODE_HEADER_S SXVOID_NAME;\n\n\
/*\n\
your attribute declarations...\n\
*/\n\
};\n\
", prgentname, prgentname);

/* node name #define generation */

    puts ("/*\nN O D E   N A M E S\n*/");

    for (i = 1; i <= T_ag->T_constants.T_nbnod; i++) {
	printf ("#define %s_n %ld\n", T_ag->T_node_name_string + T_ag->T_nns_indx [i], i);
    }

    puts ("/*\nE N D   N O D E   N A M E S\n*/\n");

/* pass inherited generation */

    b_used = sxba_calloc (bnf_ag.WS_TBL_SIZE.nbpro + 1);
    node_set = sxba_calloc (T_ag->T_constants.T_nbnod + 1);
    inherited = true;
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
		    print_sons (T_ag, prod_set, (SXINT)0, init_col);
		else if (k == 1) {
		    print_sons (T_ag, prod_set, (SXINT)1, init_col);
		}
		else {
		    puts ("\n\tswitch (SXVISITED->position) {");

		    for (l = 1; l <= k; l++) {
			printf ("\tcase %ld :", l);
			print_sons (T_ag, prod_set, l, (SXINT)17 /* environ */);
			puts ("\tbreak;\n");
		    }
		    gen_default_switch_case(1);
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

    inherited = false;
    gen_header ();

    for (i = 1; i <= T_ag->T_constants.T_nbnod; i++) {
	printf ("case %s_n :\nbreak;\n\n", T_ag->T_node_name_string + T_ag->T_nns_indx [i]);
    }

    gen_trailer ();

/* tail generation */

    puts ("\nstatic void smpopen (SXTABLES *sxtables_ptr)");
    puts ("{");
    puts ("sxuse(sxtables_ptr);");
    printf ("sxatcvar.atc_lv.node_size = sizeof (struct %s_node);\n", prgentname);
    puts ("}");
    puts ("\nstatic void smppass (void)");
    puts ("{");
    puts ("\n/*   I N I T I A L I S A T I O N S   */");
    puts ("/* ........... */\n");
    puts ("/*   A T T R I B U T E S    E V A L U A T I O N   */");
    printf ("sxsmp (sxatcvar.atc_lv.abstract_tree_root, %s_pi, %s_pd);\n", prgentname, prgentname);
    puts ("\n/*   F I N A L I S A T I O N S   */");
    puts ("/* ........... */\n");
    puts ("}");
    printf ("\nint %s_smp (SXINT what, SXTABLES *sxtables_ptr)\n", prgentname);
    puts ("{");
    puts ("sxuse (sxtables_ptr);");
    puts ("switch (what) {");
    puts ("case SXOPEN:");
    puts ("smpopen (sxtables_ptr);");
    puts ("break;");
    puts ("case SXSEMPASS: /* For [sub-]tree evaluation during parsing */");
    puts ("break;");
    puts ("case SXACTION:");
    puts ("smppass ();");
    puts ("break;");
    puts ("case SXCLOSE:");
    puts ("break;");
    puts ("default:");
    puts ("break;");
    puts ("}");
    puts ("return 1;");
    puts ("}");
    sxbm_free (node_to_red_set);
}
