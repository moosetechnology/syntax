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
/* 20030520 10:37 (phd):	Ajout de cette rubrique "modifications"	*/
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
char WHAT_SEMATGENATPASCAL[] = "@(#)SYNTAX - $Id: genat_pascal.c 587 2007-05-25 12:18:30Z rlacroix $" WHAT_DEBUG;

extern int      get_node_name_ref (int i);
extern int      delta_to_lispro (int x_prod, int delta);
int	max_line_length;
static BOOLEAN	inherited;
static SXBA	*node_to_red_set /* 1:T_nbnod, T_nbpro */ ;
static SXBA	b_used, prod_set, node_set;



static SXBA	node_name_set (nt, node_name_set_node_set, node_name_set_b_used)
    int		nt;
    SXBA	node_name_set_node_set, node_name_set_b_used;
{
    register int	j, k, m, n, lim1, lim2;

    lim1 = bnf_ag.WS_NTMAX [nt + 1].npg;

    for (m = bnf_ag.WS_NTMAX [nt].npg; m < lim1; m++) {
	n = bnf_ag.WS_NBPRO [m].numpg;

	if (!sxba_bit_is_set (node_name_set_b_used, n)) {
	    sxba_1_bit (node_name_set_b_used, n);

	    if (bnf_ag.WS_NBPRO [n].action != 0) {
		sxba_1_bit (node_name_set_node_set, get_node_name_ref (n));
	    }
	    else {
		lim2 = bnf_ag.WS_NBPRO [n + 1].prolon;

		for (j = bnf_ag.WS_NBPRO [n].prolon; j < lim2; j++) {
		    if ((k = bnf_ag.WS_INDPRO [j].lispro) > 0 /* [x]nt ou action*/ && !IS_A_PARSACT (k)) {
			node_name_set (XNT_TO_NT (k), node_name_set_node_set, node_name_set_b_used);
		    }
		}
	    }
	}
    }

    return node_name_set_node_set;
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
	lim = T_ag->SXT_node_info[i].T_ss_indx;
	m = 0;
	act = bnf_ag.WS_NBPRO[i].action;

	for (j = T_ag->SXT_node_info[i + 1].T_ss_indx - 1; j >= lim; j--) {
	    m++;

	    if (m == son || (act == 2 /* add to left list */ && m == 2) || (act == 3 /* add to right list */ && m == 1) ||
		act == 6 /* list */ ) {
		k = bnf_ag.WS_INDPRO[delta_to_lispro (i, T_ag->T_stack_schema[j])].lispro;

		if (k < 0) {
		    sxba_1_bit (node_set, T_ag->T_ter_to_node_name[-XT_TO_T (k)]);
		}
		else  /* [x]nt ou action*/
		    if (!IS_A_PARSACT (k)) {
			sxba_empty (b_used);
			node_name_set (XNT_TO_NT (k), node_set, b_used);
		    }
	    }
	}
    }

    if ((i = sxba_scan (node_set, 0)) > 0) {
	printf ("(* VISITED^.name = ");
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

	if ((j += 3 /* strlen (" *)") */) > max_line_length) {
	    putchar ('\n');
	    /* j = 3; inutile */
	}

	puts (" *)");
    }
}




static SXVOID	gen_header ()
{
    printf ("\nprocedure %s_p%c;\n\
begin\n\n(*\n%s\n*)\n\n\
case VISITED^.%sname of\n\n", prgentname, inherited ? 'i' : 'd', inherited ? "I N H E R I T E D" :
	 "D E R I V E D", inherited ? "father^." : "");
}



static SXVOID	gen_trailer ()
{
    printf ("\n(*\nZ Z Z Z\n*)\n\nOTHERWISE;\nend;\nend (* %s_p%c *) ;\n\n", prgentname, inherited ? 'i' : 'd');
}

SXVOID
genat_pascal (T_ag)
    struct T_ag_item	*T_ag;
{
    register int  i, j, k, l;

    node_to_red_set = sxbm_calloc (T_ag->T_constants.T_nbnod + 1, T_ag->T_constants.T_nbpro + 1);


/* generation du code  pour la semantique par arbre abstrait */

/* table inverse */

    for (j = 1; j <= T_ag->T_constants.T_nbpro; j++) {
	if ((i = get_node_name_ref (j)) != 0) {
	    sxba_1_bit (node_to_red_set[i], j);
	}
    }

    {
	register char *p;
	char          s[132];
	long          date_time;

	puts ("(* ********************************************************************");

	if ((p = sxrindex (sxsrcmngr.source_coord.file_name, '/')) == NULL)
	    p = sxsrcmngr.source_coord.file_name;
	else
	    p++;

	sprintf (s, "   *  This program has been generated from %s", p);
	printf ("%-*s*\n", 70, s);
	date_time = time (0);
	p = ctime (&date_time);
	*(p + 24) = '\0';
	sprintf (s, "   *  on %s", p);
	printf ("%-*s*\n", 70, s);
	puts ("   *  by the SYNTAX (@) abstract tree constructor SEMAT               *");
	puts ("   ********************************************************************");
	puts ("   *  (@) SYNTAX is a trademark of INRIA.                             *");
	puts ("   ******************************************************************** *)\n\n\n");
    }

    puts ("MODULE semantic_pass;\n");
    puts ("const");
    puts ("%include 'pascal/constants.h';\n");
    puts ("(*\nN O D E   N A M E S\n*)");

    for (i = 1; i <= T_ag->T_constants.T_nbnod; i++) {
	printf ("%s_n\t= %d ;\n", T_ag->T_node_name_string + T_ag->T_nns_indx[i], i);
    }

    puts ("(*\nE N D   N O D E   N A M E S\n*)\n");
    puts ("type");
    puts ("%include 'pascal/types.h';\n");
    puts ("visit_kind = (INHERITED, DERIVED);\n");
    puts ("NodePtr = ^Node;\n\n");
    puts ("Node =\n    record\n");
    puts ("\tfather,\n\tbrother,\n\tson\t\t\t: NodePtr;\n");
    puts ("\tname,\n\tdegree,\n\tposition\t\t: CInt;\n");
    puts ("\tis_list,\n\tfirst_list_element,\n\tlast_list_element\t: CBoolean;\n");
    puts ("\ttoken\t\t\t: sxtoken;");
    puts ("\n\t(* your attribute declarations... *)\n\n    end;\n");
    puts ("var");
    puts ("VISITED, LAST_ELEM, LEFT : NodePtr;\n");
    puts ("VISIT : visit_kind;\n");
    puts ("NextVisit :\n\
    record\n\
\tvisited\t: NodePtr;\n\
\tvisit\t: visit_kind;\n\
\tnormal\t: boolean\n\
    end;\n\n");
    puts ("\n%include 'pascal/routines.h';\n");
    puts ("procedure SpecifyNextVisit (node:NodePtr; kind:visit_kind);");
    puts ("begin");
    puts ("    NextVisit.normal := FALSE;");
    puts ("    NextVisit.visited := node;");
    puts ("    NextVisit.visit := kind");
    puts ("end;");

/* pass inherited generation */

    b_used = sxba_calloc (bnf_ag.WS_TBL_SIZE.nbpro + 1);
    node_set = sxba_calloc (T_ag->T_constants.T_nbnod + 1);
    inherited = TRUE;
    gen_header ();

    for (i = 1; i <= T_ag->T_constants.T_nbnod; i++) {
	prod_set = node_to_red_set[i];

	for (j = sxba_scan (prod_set, 0); j != -1 && i != T_ag->SXT_node_info[j].T_node_name; j = sxba_scan
	     (prod_set, j)) {
	}

	if (j != -1) {
	    k = T_ag->SXT_node_info[j + 1].T_ss_indx - T_ag->SXT_node_info[j].T_ss_indx;

	    if ((k > 0 || bnf_ag.WS_NBPRO[j].action == 6) && bnf_ag.WS_NBPRO[j].action != 5) {
		/* prod_set contient toutes les productions de nom i */
		printf ("%s_n :\n", T_ag->T_node_name_string + T_ag->T_nns_indx[i]);

		if (bnf_ag.WS_NBPRO[j].action == 6 /* list */ ) {
		    print_sons (T_ag, prod_set, 0, 0);
		    puts ("\tbegin");
		}
		else if (k == 1) {
		    print_sons (T_ag, prod_set, 1, 0);
		    puts ("\tbegin");
		}
		else {
		    puts ("\tcase VISITED^.position of");

		    for (l = 1; l <= k; l++) {
			printf ("\t%d :\n", l);
			print_sons (T_ag, prod_set, l, 0);
			puts ("\t\tbegin");
			puts ("\t\tend;\n");
		    }
		}

		puts ("\tend;\n");
	    }
	}
	else if (strcmp (T_ag->T_node_name_string + T_ag->T_nns_indx[i], "ERROR") == 0) {
	    puts ("ERROR_n :\n\tbegin\n\tend;\n");
	}
    }

    sxfree (node_set);
    sxfree (b_used);
    gen_trailer ();

/* pass derived generation */

    inherited = FALSE;
    gen_header ();

    for (i = 1; i <= T_ag->T_constants.T_nbnod; i++) {
	printf ("%s_n :\n\tbegin\n\tend;\n\n", T_ag->T_node_name_string + T_ag->T_nns_indx[i]);
    }

    gen_trailer ();

/* tail generation */

    puts ("\nprocedure smppass;");
    puts ("var\nroot,\nnode_ptr\t: NodePtr;\n");
    puts ("done\t: Boolean;\n");
    puts ("begin");
    puts ("\n(*   I N I T I A L I S A T I O N S   *)");
    puts ("(* ........... *)\n");
    puts ("(*   A T T R I B U T E S    E V A L U A T I O N   *)");
    puts ("begin");
    puts ("    root := sxatroot;");
    puts ("");
    puts ("    if root <> nil then begin");
    puts ("\tdone := FALSE;");
    puts ("\tNextVisit.normal := TRUE;");
    puts ("\tVISITED := root^.son;");
    puts ("");
    puts ("\tif VISITED <> nil then VISIT := INHERITED");
    puts ("\telse begin");
    puts ("\t    VISITED := root;");
    puts ("\t    VISIT := DERIVED");
    puts ("\tend;");
    puts ("");
    puts ("\trepeat");
    puts ("\t    if VISIT = INHERITED then begin");
    printf ("\t\t%s_pi;\n", prgentname);
    puts ("\t\tnode_ptr := VISITED^.son;");
    puts ("");
    puts ("\t\tif node_ptr <> nil then begin");
    puts ("\t\t    VISITED := node_ptr;");
    puts ("\t\t    LEFT := nil");
    puts ("\t\tend");
    puts ("\t\telse begin");
    puts ("\t\t    VISIT := DERIVED;");
    puts ("\t\t    LAST_ELEM := nil");
    puts ("\t\tend");
    puts ("\t    end");
    puts ("\t    else begin");
    printf ("\t\t%s_pd;\n", prgentname);
    puts ("");
    puts ("\t\tif VISITED = root then done := TRUE");
    puts ("\t\telse begin");
    puts ("\t\t    node_ptr := VISITED^.brother;");
    puts ("");
    puts ("\t\t    if node_ptr <> nil then begin");
    puts ("\t\t\tVISIT := INHERITED;");
    puts ("\t\t\tLEFT := VISITED;");
    puts ("\t\t\tVISITED := node_ptr");
    puts ("\t\t    end");
    puts ("\t\t    else begin");
    puts ("\t\t\tLAST_ELEM := VISITED;");
    puts ("\t\t\tVISITED := VISITED^.father");
    puts ("\t\t    end");
    puts ("\t\tend");
    puts ("\t    end;");
    puts ("");
    puts ("\t    if not NextVisit.normal then begin");
    puts ("\t\tdone := FALSE;");
    puts ("\t\tNextVisit.normal := TRUE;");
    puts ("\t\tVISITED := NextVisit.visited;");
    puts ("\t\tVISIT := NextVisit.visit;");
    puts ("");
    puts ("\t\tif VISIT = INHERITED then");
    puts ("\t\t    LEFT := sxbrother (VISITED, VISITED^.position - 1)");
    puts ("\t\telse");
    puts ("\t\t    LAST_ELEM := sxson (VISITED, VISITED^.degree)");
    puts ("\t    end");
    puts ("\tuntil not done");
    puts ("    end");
    puts ("end;");
    puts ("\n(*   F I N A L I S A T I O N S   *)");
    puts ("(* ........... *)\n");
    puts ("end (* smppass *) ;");
    printf ("\nprocedure %s_smp (what : CInt; sxtables_ptr : Address);\n", prgentname);
    puts ("Val_Param;\n");
    puts ("begin");
    puts ("case what of\n");
    puts ("SXOPEN:\n\tsxsetnodesize (SizeOf (Node));\n\n");
    puts ("SXACTION:\n\tsmppass;\n");
    puts ("OTHERWISE;\n");
    puts ("end");
    puts ("end;");
    sxbm_free (node_to_red_set);
}
