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

/* Genere a partir d'une Range Concatenation Grammar
   - une bnf qui definit un sur-langage de celui de la RCG et
   qui permet d'utiliser SYNTAX (scanner et parser)
   - l'idee d'utiliser le parser bnf en premiere passe en O(n^3)
     peut aider l'analyse de la RCG.  Une piste est de noter les
     triplets (A, p, [i..j]) valides en bnf et de n'utiliser que
     ceux-la en RCG.  Le pieme parametre de A derive ds w^{[i..j]}.
 */

/*   I N C L U D E S   */
#include "sxversion.h"
#include "sxunix.h"
#include "sxba.h"
#include "rcg.h"
#include "varstr.h"
#include <ctype.h>
#include <string.h>

char WHAT_RCGGENBNF[] = "@(#)SYNTAX - $Id: rcg_gen_bnf.c 3633 2023-12-20 18:41:19Z garavel $" WHAT_DEBUG;

#define	Tstar_		0
#define	S_		1
#define	T_		2

#define	predef_nt_nb	3

extern SXINT is_in_Tstar (SXINT, SXINT);

static FILE		*bnf_file;
static SXINT		*arg_stack, *lhs_param_stack, *rhs_pos2Ak, *rhs_pos2bot3, *rhs_pos2top3;
static SXBA		rhs_pos_set, spcl_pos_set;
static bool		Tstar_is_needed;
static XH_header	XH_bnf;

static void
gen_bnf_rule (void)
{
    /* L'arg est ds arg_stack */
    /* arg_stack [1] definit la LHS */
    SXINT		x, symb, Ak, A, k, top, gen_bnf_rule_rule_nb;
    char                *str;

    top = arg_stack [0];

    /* On commence par verifier que cette regle est nouvelle */
    for (x = 1; x <= top; x++) {
	XH_push (XH_bnf, arg_stack [x]);
    }

    if (!XH_set (&XH_bnf, &gen_bnf_rule_rule_nb)) {
	/* Nouvelle regle, on la genere */
	for (x = 1; x <= top; x++) {
	    symb = arg_stack [x];

	    if (symb < 0) {
		/* terminal */
	      symb = -symb;

	      if (symb > max_ste)
		/* !! On ne doit pas pouvoir generer de bnf */
		fprintf (bnf_file, "%ld ", symb-max_ste-1);
	      else {
		str = sxstrget (t2ste [-symb]);

		if (*str == '"')
		  str++;

		fprintf (bnf_file, "\"%s\" ", str);
	      }
	    }
	    else {
		if (symb < predef_nt_nb) {
		    switch (symb) {
		    case S_:
			fputs ("<S> ", bnf_file);
			break;

		    case T_:
			fputs ("<T> ", bnf_file);
			break;

		    case Tstar_:
			Tstar_is_needed = true;
			fputs ("<T*> ", bnf_file);
			break;
		    default:
#if EBUG
		      sxtrap("rcg_gen_bnf","unknown switch case #1");
#endif
		      break;
		    }
		}
		else {
		    /* C'est un couple (nt, position de l'arg) */
		    Ak = symb-predef_nt_nb;
		    A = Ak2A (Ak);

		    if (SXBA_bit_is_set (is_lhs_nt, A)) {
			/* C'est un NT dont la def est locale */
			k = Ak_A2k (Ak, A);
			fprintf (bnf_file, "<%s[%ld]> ", sxstrget (nt2ste [A]), (SXINT) k+1);
		    }
		    else {
			/* C'est un vrai externe, on produit T* */
			Tstar_is_needed = true;
			fputs ("<T*> ", bnf_file);
		    }
		}
	    }

	    if (x == 1)
		fputs ("= ", bnf_file);
	}

	fputs (";\n", bnf_file);
    }
}


static void
substitute (SXINT lhs_bot3, SXINT lhs_top3)
{
    SXINT		lgth, rhs_pos, rhs_bot3, rhs_cur3, rhs_top3, lhs_cur3, symb, x;
    bool	has_a_substitution;

    if (lhs_bot3 == lhs_top3) {
	/* la substitution est faite */
	/* Le resultat est ds arg_stack */
	gen_bnf_rule ();
    }
    else {
	has_a_substitution = false;
	lgth = lhs_top3-lhs_bot3;
	rhs_pos = -1;

	while ((rhs_pos = sxba_scan (rhs_pos_set, rhs_pos)) >= 0) {
	     rhs_bot3 = rhs_pos2bot3 [rhs_pos];
	     rhs_top3 = rhs_pos2top3 [rhs_pos];

	     if (lgth >= rhs_top3-rhs_bot3) {
		 for (lhs_cur3 = lhs_bot3, rhs_cur3 = rhs_bot3; rhs_cur3 < rhs_top3; lhs_cur3++, rhs_cur3++) {
		     if (S1_get (lhs_param_stack, lhs_cur3) != XH_list_elem (rcg_parameters, rhs_cur3))
			 break;
		 }

		 if (rhs_cur3 == rhs_top3) {
		     /* rhs_pos marche */
		     PUSH (arg_stack, rhs_pos2Ak [rhs_pos]+predef_nt_nb);
		     has_a_substitution = true;
		     substitute (lhs_bot3+rhs_top3-rhs_bot3, lhs_top3);
		     TOP (arg_stack)--;
		 }
	     }
	}

	if (!has_a_substitution) {
	    symb = S1_get (lhs_param_stack, lhs_bot3);

	    if (symb > 0) {
		if ((x = arg_stack [0]) == 1 || arg_stack [x] != 0)
		    /* On n'empile pas 2 <T*> de suite (inutile) */
		    symb = 0; /* <T*> */
	    }

	    if (symb <= 0)
		PUSH (arg_stack, symb);

	    substitute (lhs_bot3+1, lhs_top3);

	    if (symb <= 0)
		TOP (arg_stack)--;
	}
    }
}

static void
pre_substitute (SXINT lhs_bot3, SXINT lhs_top3, bool is_spcl)
{
    SXINT		lhs_lgth, rhs_lgth, rhs_pos, rhs_bot3, rhs_cur3, rhs_top3, lhs_cur3, symb;
    bool	has_a_substitution;

    lhs_lgth = lhs_top3-lhs_bot3;

    if (lhs_lgth == 0) {
	/* la substitution par les $StrEq("a", X) est faite */
	/* Le resultat est ds lhs_param_stack */
	substitute ((SXINT)1, S1_top (lhs_param_stack)+1);
    }
    else {
	if (is_spcl) {
	    has_a_substitution = false;
	    rhs_pos = -1;

	    while ((rhs_pos = sxba_scan (spcl_pos_set, rhs_pos)) >= 0) {
		rhs_bot3 = rhs_pos2bot3 [rhs_pos];
		rhs_top3 = rhs_pos2top3 [rhs_pos];
		rhs_lgth = rhs_top3-rhs_bot3;

		if (lhs_lgth >= rhs_lgth) {
		    for (lhs_cur3 = lhs_bot3, rhs_cur3 = rhs_bot3; rhs_cur3 < rhs_top3; lhs_cur3++, rhs_cur3++) {
			if (XH_list_elem (rcg_parameters, lhs_cur3) != XH_list_elem (rcg_parameters, rhs_cur3))
			    break;
		    }

		    if (rhs_cur3 == rhs_top3) {
			/* rhs_pos marche */
			/* On recopie le 1er param ds lhs_param_stack */
			rhs_bot3 = rhs_pos2bot3 [rhs_pos-1];
			rhs_top3 = rhs_pos2top3 [rhs_pos-1];

			for (rhs_cur3 = rhs_bot3; rhs_cur3 < rhs_top3; rhs_cur3++) {
			    S1_push (lhs_param_stack, XH_list_elem (rcg_parameters, rhs_cur3));
			}

			has_a_substitution = true;
			pre_substitute (lhs_bot3+rhs_lgth, lhs_top3, is_spcl);
			S1_top (lhs_param_stack) -= rhs_top3-rhs_bot3;
		    }
		}
	    }

	    if (!has_a_substitution) {
		symb = XH_list_elem (rcg_parameters, lhs_bot3);
		S1_push (lhs_param_stack, symb);
		pre_substitute (lhs_bot3+1, lhs_top3, is_spcl);
		S1_top (lhs_param_stack)--;
	    }
	}
	else {
	    for (lhs_cur3 = lhs_bot3; lhs_cur3 < lhs_top3; lhs_cur3++) {
		S1_push (lhs_param_stack, XH_list_elem (rcg_parameters, lhs_cur3));
	    }

	    substitute ((SXINT)1, S1_top (lhs_param_stack)+1);
	    S1_top (lhs_param_stack) -= lhs_lgth;
	}
    }
}

static void
process_clause (SXINT clause)
{
    SXINT 	lhs_prdct, lhs_bot2, lhs_cur2, lhs_top2, lhs_arity, lhs_param, lhs_bot3, lhs_top3, lhs_arg, lhs_nt;
    SXINT		rhs_cur, rhs_top, rhs_bot, rhs_prdct, rhs_bot2, rhs_cur2, rhs_nt, rhs_top2, k, rhs_param, rhs_bot3,
                rhs_cur3, rhs_top3, rhs_pos, rhs, lsymb, rsymb;
    SXINT		max_rhs_pos, Ak;
    bool	has_component, is_spcl;

    lhs_prdct = XxY_X (rcg_clauses, clause) & lhs_prdct_filter;
    lhs_bot2 = XH_X (rcg_predicates, lhs_prdct);
    lhs_top2 = XH_X (rcg_predicates, lhs_prdct+1);
    lhs_nt = XH_list_elem (rcg_predicates, lhs_bot2+1);
    rhs_pos = lhs_arity = nt2arity [lhs_nt];
    rhs = XxY_Y (rcg_clauses, clause);
    rhs_top = XH_X (rcg_rhs, rhs+1);
    rhs_bot = XH_X (rcg_rhs, rhs);
    has_component = false;
    is_spcl = false;

    /* On commence par glaner qq renseignements sur la RHS */
    sxba_empty (rhs_pos_set);
    sxba_empty (spcl_pos_set);

    for (rhs_cur = rhs_bot; rhs_cur < rhs_top; rhs_cur++) {
	rhs_prdct = XH_list_elem (rcg_rhs, rhs_cur);
	rhs_bot2 = XH_X (rcg_predicates, rhs_prdct);
	rhs_nt = XH_list_elem (rcg_predicates, rhs_bot2+1);

	if (rhs_nt > 0) {
	    /* On oublie les predef */
	    /* Meme ds le cas &false(), on genere la regle bnf correspondante car
	       elle peut definir des terminaux */
	    rhs_top2 = XH_X (rcg_predicates, rhs_prdct+1);

	    for (k = 0, rhs_cur2 = rhs_bot2+2; rhs_cur2 < rhs_top2; k++, rhs_pos++, rhs_cur2++) {
		rhs_param = XH_list_elem (rcg_predicates, rhs_cur2);
		rhs_pos2Ak [rhs_pos] = A_k2Ak (rhs_nt, k);

		if (rhs_param > 0) {
		    rhs_pos2bot3 [rhs_pos] = rhs_bot3 = XH_X (rcg_parameters,  rhs_param);
		    rhs_pos2top3 [rhs_pos] = rhs_top3 = XH_X (rcg_parameters, rhs_param+1);

		    if (max_gcomponent_nb > 0) {
			for (rhs_cur3 = rhs_bot3;  rhs_cur3 < rhs_top3; rhs_cur3++) {
			    if (XH_list_elem (rcg_parameters, rhs_cur3) == 0)
				/* "..." */
				break;
			}
		    }
		    else
			rhs_cur3 = rhs_top3;

		    if (rhs_cur3 == rhs_top3)
			/* pas de "..." */
			SXBA_1_bit (rhs_pos_set, rhs_pos);
		    else
			has_component = true;
		}
	    }
	}
	else {
	    if (rhs_nt == STREQ_ic) {
		/* sauf &StrEq("a", X) */
		rhs_param = XH_list_elem (rcg_predicates, rhs_bot2+2);

		if (rhs_param == 0 ||
		    is_in_Tstar (XH_X (rcg_parameters, rhs_param), XH_X (rcg_parameters, rhs_param+1)) == 1) {
		    is_spcl = true;

		    if (rhs_param) {
			rhs_pos2bot3 [rhs_pos] = XH_X (rcg_parameters,  rhs_param);
			rhs_pos2top3 [rhs_pos] = XH_X (rcg_parameters, rhs_param+1);
		    }
		    else {
			rhs_pos2bot3 [rhs_pos] = 0;
			rhs_pos2top3 [rhs_pos] = 0;
		    }

		    rhs_pos++;
		    SXBA_1_bit (spcl_pos_set, rhs_pos);
		    rhs_param = XH_list_elem (rcg_predicates, rhs_bot2+3);
		    rhs_pos2bot3 [rhs_pos] = XH_X (rcg_parameters,  rhs_param);
		    rhs_pos2top3 [rhs_pos] = XH_X (rcg_parameters, rhs_param+1);
		    rhs_pos++;
		}
	    }
	}
    }

    max_rhs_pos = rhs_pos;

    /* Pour chaque sous_chaine d'un arg de la LHS, on regarde si elle est un arg de la rhs */
    for (lhs_arg = 0, lhs_cur2 = lhs_bot2+2; lhs_cur2 < lhs_top2; lhs_arg++, lhs_cur2++) {
	lhs_param = XH_list_elem (rcg_predicates, lhs_cur2);
	PUSH (arg_stack, A_k2Ak (lhs_nt, lhs_arg)+predef_nt_nb);

	if (lhs_param > 0) {
	    lhs_bot3 = XH_X (rcg_parameters, lhs_param);
	    lhs_top3 = XH_X (rcg_parameters, lhs_param+1);
	    pre_substitute (lhs_bot3, lhs_top3, is_spcl);
	}
	else {
	    /* output ("<A[lhs_arg]> = ;") */
	    gen_bnf_rule ();
	}

	TOP (arg_stack)--;
    }

    if (has_component) {
	/* On genere (on suppose que c'est le 1er arg) :
	      (1)	B(...)			=>	<S> = <B[1]> ;		
	      (2)	B(... \alpha)		=>	<S> = <B[1]> <T*> ;
	      (3)	B(\alpha ...)		=>	<S> = <T*> <B[1]> ;
	      (4)	B(\alpha ... \beta)	=>	<S> = <T*> <B[1]> <T*> ;
	   L'idee est d'analyser <B[1]> avec la bonne sous-chaine du source.
	   Par exemple B(\alpha ...) derive ds un suffixe du source.
	   Le source (<S>) est donc forme d'un prefixe <T*> suivi de <B[1]>. */
	Tstar_is_needed = true;

	for (rhs_pos = lhs_arity; rhs_pos < max_rhs_pos; rhs_pos++) {
	    if (!SXBA_bit_is_set (rhs_pos_set, rhs_pos)) {
		rhs_bot3 = rhs_pos2bot3 [rhs_pos];
		rhs_top3 = rhs_pos2top3 [rhs_pos];

		if (rhs_bot3 < rhs_top3) {
		    /* Normalement il n'y a pas de param vide en RHS */
		    lsymb = XH_list_elem (rcg_parameters, rhs_bot3);
		    rsymb = XH_list_elem (rcg_parameters, rhs_top3-1);

		    PUSH (arg_stack, S_);

		    Ak = rhs_pos2Ak [rhs_pos];

		    if (lsymb == 0) {
			/* cas (1) */
			PUSH (arg_stack, Ak+predef_nt_nb);

			if (rsymb != 0) {
			    /* cas (2) */
			    PUSH (arg_stack, Tstar_);
			}
		    }
		    else {
			/* cas (3) */
			PUSH (arg_stack, Tstar_);
			PUSH (arg_stack, Ak+predef_nt_nb);

			if (rsymb != 0) {
			    /* cas (4) */
			    PUSH (arg_stack, Tstar_);
			}
		    }

		    gen_bnf_rule ();
		    arg_stack [0] = 0;
		}
	    }
	}
    }
}


static void
gen_header (void)
{
    fprintf (bnf_file,
"\n\
***********************************************************************\n\
*\tThis CFG in BNF form defines a superset of the language \"%s\".\n\
*\tIt has been generated\
*\tby the SYNTAX(*) Range Concatenation Grammar Processor\n\
***********************************************************************\n\
*\t(*) SYNTAX is a trademark of INRIA.\n\
***********************************************************************\n\n\n",
	     prgentname);
}

void
gen_bnf (void)
{
    SXINT	clause, t;
    char	lst_name [131], *str;

    if (sxverbosep) {
	fputs ("\tgen bnf .... ", sxtty);
	sxttycol1p = false;
    }

    if ((bnf_file = fopen (strcat (strcpy (lst_name, prgentname), ".bnf"), "w")) == NULL) {
	fprintf (sxstderr, "gen_bnf: cannot open (create) ");
	sxperror (lst_name);
	return;
    }

    gen_header ();

    Tstar_is_needed = false;

    if (is_CF_parser) {
	/* Allocations */
	arg_stack = (SXINT*) sxalloc (max_clause_arg_nb+1, sizeof (SXINT)), arg_stack [0] = 0;
	S1_alloc (lhs_param_stack, max_arg_size+1);
	rhs_pos2Ak = (SXINT*) sxalloc (max_clause_arg_nb+1, sizeof (SXINT));
	rhs_pos2bot3 = (SXINT*) sxalloc (max_clause_arg_nb+1, sizeof (SXINT));
	rhs_pos2top3 = (SXINT*) sxalloc (max_clause_arg_nb+1, sizeof (SXINT));
	rhs_pos_set = sxba_calloc (max_clause_arg_nb+1);
	spcl_pos_set = sxba_calloc (max_clause_arg_nb+1);
	XH_alloc (&XH_bnf, "XH_bnf", last_clause*max_garity + 1, 1, max_arg_size+1, NULL, NULL);

	PUSH (arg_stack, S_);
	PUSH (arg_stack, A_k2Ak (1, 0)+predef_nt_nb); /* <S[1]> */
	gen_bnf_rule (); /* <S> = <S[1]> ; */
	arg_stack [0] = 0;

	for (clause = 1; clause <= last_clause; clause++)
	    process_clause (clause);

	sxfree (arg_stack), arg_stack = NULL;
	S1_free (lhs_param_stack), lhs_param_stack = NULL;
	sxfree (rhs_pos2Ak), rhs_pos2Ak = NULL;
	sxfree (rhs_pos2bot3), rhs_pos2bot3 = NULL;
	sxfree (rhs_pos2top3), rhs_pos2top3 = NULL;
	sxfree (rhs_pos_set), rhs_pos_set = NULL;
	sxfree (spcl_pos_set), spcl_pos_set = NULL;
	XH_free (&XH_bnf);
    }

    if (Tstar_is_needed || !is_CF_parser) {
	/* Si on n'a pas besoin d'un CF-parser, la bnf est minimale et utilisee
	   pour que lecl puisse marcher */
	if (is_OLTAG)
	    fputs ("<axiom> = <S> ;\n", bnf_file);
	else
	    fputs ("<T*> = ;\n", bnf_file);

	if (max_t > 0) {
	    if (!is_OLTAG)
		fputs ("<T*> = <T*> <T> ;\n", bnf_file);

	    for (t = 1; t <= max_t; t++) {
	      str = sxstrget (t2ste [t]);
	      
	      if (*str == '"')
		str++;

	      fprintf (bnf_file, "<T> = \"%s\" ;\n", str);
	    }
	}

	if (is_OLTAG) {
	    fputs ("<TL> = ;\n", bnf_file);
	    fputs ("<TL> = <TL> <F> ; 1\n", bnf_file);
	    if (max_t > 0) fputs ("<F> = <T> ;\n", bnf_file);
	    fputs ("<F> = %MOT ;\n", bnf_file);
	    fputs ("<FF> = %MOT ; 2\n", bnf_file);
	    if (max_t > 0) fputs ("<FF> = <T> ; 3\n", bnf_file);
	    fputs ("<FF> = ; 4\n", bnf_file);
	    fputs ("<S> = <S> <L> ;\n", bnf_file);
	    fputs ("<S> = ;\n", bnf_file);
	    fputs ("<L> = <FF> ( <TL> ) ;\n", bnf_file);
	}
    }

    fclose (bnf_file);

    if (sxverbosep) {
	fputs ("done\n", sxtty);
	sxttycol1p = true;
    }
}
