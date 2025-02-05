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
   Une RCG equivalente dont le nombre de variables par arg est minimal */

static char	ME [] = "rcg_1rcg_2var";

/*   I N C L U D E S   */

#include "sxversion.h"
#include "sxunix.h"
#include "B_tables.h"
#include "sxba.h"
#include "XxYxZ.h"
#include "rcg.h"
#include "rcg_td.h"
#include "varstr.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "sxmatrix2vector.h"

char WHAT_RCG1RCG2VAR[] = "@(#)SYNTAX - $Id: rcg_1rcg_2var.c 4191 2024-08-29 10:29:33Z garavel $" WHAT_DEBUG;

extern SXINT is_arg_in_Tstar (SXINT param);

/* commun a -lbnf et -lfsa */
static SXINT *nt2lhs_nb, *nt2rhs_nb; /* Cas is_print == false */

static char	lst_name [131];
static FILE	*out_file;

static SXINT	*clause2disp, *new_clause2disp, *new_clauses_body, *nt2clause_disp, *clause_disp2clause;
static SXINT	new_max_rhs_prdct, new_prdct_nb, new_clauses_body_top, new_axiom, init_clause_nb;
static bool	to_be_memorized;
static SXINT	*clause2clause2var_form;
static SXINT	*clause2new_clause, *new_clause2rhs_disp, *new_clause2rhs_list, new_clause2rhs_list_top;

#define add_item(i)		new_clauses_body [++new_clauses_body_top] = i
#define close_new_arg()		add_item (EOA)
#define close_new_prdct()	add_item (EOP), new_prdct_nb++
#define close_new_clause(c)	new_clause2disp [c+1] = new_clauses_body_top+1,\
                                (new_prdct_nb > new_max_rhs_prdct) ? new_max_rhs_prdct = new_prdct_nb : 0,\
                                new_prdct_nb = 0
#define close_clause()		clause2disp [clause+1] = new_clause_nb+1

#define EOC	((SXINT)((~((SXUINT)0))>>1))
#define EOA	(EOC-1)
#define EOP	(EOA-1)

/* ********************************************************************************** */
static XH_header	local_prdct_calls, args_1rcg;
static SXINT		*call_kinds;
static XxY_header	guide_nts;

static char rcg_header [] = "\n\
%%   ********************************************************************\n\
%%\t\"%s\" is %s\n\
%%\tgenerated from \"%s.%s\"\n\
%%\ton %s\
%%\tby the SYNTAX(*) Range Concatenation Grammar processor\n\
%%   ********************************************************************\n\
%%\t(*) SYNTAX is a trademark of INRIA.\n\
%%   ********************************************************************\n\n\n";

static char c_header [] = "\n\
/* ********************************************************************\n\
\t\"%s\" is %s\n\
\tgenerated from \"%s.%s\"\n\
\ton %s\
\tby the SYNTAX(*) Range Concatenation Grammar processor\n\
   ********************************************************************\n\
\t(*) SYNTAX is a trademark of INRIA.\n\
   ******************************************************************** */\n\n\n";

static char bnf_header [] = "\n\
*   ********************************************************************\n\
*\t\"%s\" is %s\n\
*\tgenerated from \"%s.%s\"\n\
*\ton %s\
*\tby the SYNTAX(*) Range Concatenation Grammar processor\n\
*   ********************************************************************\n\
*\t(*) SYNTAX is a trademark of INRIA.\n\
*   ********************************************************************\n\n\n";



static void
out_header (char *format, char *file_name, char *kind_str)
{
    time_t date_time;
    
    date_time = time (0);

    fprintf (out_file, format, file_name, kind_str, prgentname, suffixname, ctime (&date_time));
	
}

static void
print_t (SXINT t)
{
    char	c, *p1, *p;

    if (t == max_t+1) {
      /* eof */
      /* on genere "" */
      fputs ("\"\"", out_file);
      return;
    }

    if (t > max_ste) {
      fprintf (out_file, "%ld", t-max_ste-1);
      return;
    }

    p1 = sxstrget (t2ste [t]);

    if (strlen (p1) == 1)
	fprintf (out_file, "\\%s", p1);
    else {
	fputs ("\"", out_file);

	if (*p1 == '"')
	  p1++;

	while ((p = strpbrk (p1, "\"*")) != NULL) {
	    c = *p;
	    *p = SXNUL;
	    fprintf (out_file, "%s", p1);
	    *p = c;

	    if (c == '"') {
		fputs ("\\\"", out_file);
	    }
	    else {
		if (p [1] == '/') {
		    fputs ("*\\", out_file);
		    p++;
		}
	    }

	    p1 = p;
	}

	fprintf (out_file, "%s\"", p1);
    }
}


static void
gen_false_clause (char *prefix, char *suffix)
{
    SXINT t, nt, arity;

    fprintf (out_file, "%s%s (", prefix, suffix);

    for (t = 1; t <= max_t; t++) {
	if (t > 1)
	    fputs (" ", out_file);

	print_t (t);
    }

    if (is_2var_form) {
	for (t = 0; t < max_garity; t++) {
	    fprintf (out_file, " X%ld", (SXINT) t);
	}
    }

    fputs (") --> &False () ", out_file);

    if (is_2var_form) {
	for (nt = 2; nt <= max_nt; nt++) {
	    if (!SXBA_bit_is_set (is_nt_internal, nt))
		fputs ("&", out_file);

	    fprintf (out_file, "%s (", sxstrget (nt2ste [nt]));

	    arity = nt2arity [nt];

	    for (t = 0; t < arity; t++) {
		if (t > 0)
		    fputs (", ", out_file);

		fprintf (out_file, "X%ld", (SXINT) t);
	    }

	    fputs (") ", out_file);
	}
    }

    fputs (". % 1\n\n", out_file);
}

static void
print_nt (SXINT A)
{
  char	*p1;

  if (A > 0) {
    if (SXBA_bit_is_set (is_nt_internal, A))
      fprintf (out_file, "%s", sxstrget (nt2ste [A]));
    else
      fprintf (out_file, "&%s", sxstrget (nt2ste [A]));
  }
  else {
    switch (A) {
    case STREQ_ic:
      p1 = sxttext(sxtab_ptr, streq_t);
      break;

    case STREQLEN_ic:
      p1 = sxttext(sxtab_ptr, streqlen_t);
      break;

    case STRLEN_ic:
      p1 = sxttext(sxtab_ptr, strlen_t);
      break;

    case FALSE_ic:
      p1 = sxttext(sxtab_ptr, false_t);
      break;
	
    case FIRST_ic:
      p1 = sxttext(sxtab_ptr, first_t);
      break;

    case LAST_ic:
      p1 = sxttext(sxtab_ptr, last_t);
      break;

    case CUT_ic:
      p1 = sxttext(sxtab_ptr, cut_t);
      break;

    case TRUE_ic:
      p1 = sxttext(sxtab_ptr, true_t);
      break;

    case LEX_ic:
      p1 = sxttext(sxtab_ptr, lex_t);
      break;

    default:
#if EBUG
      sxtrap(ME,"unknown switch case #1");
#endif
      sxinitialise(p1); /* pour faire taire gcc -Wuninitialized */
      break;
    }

    fprintf (out_file, "&%s", p1);
  }
}


static void
out_nt_text (SXINT A)
{
    if (to_be_memorized) {
	add_item (A);
    }
    else {
      print_nt (A);
    }
}

static void
out_t_text (SXINT t)
{
    if (to_be_memorized) {
	add_item (-t);
    }
    else {
	print_t (t);
    }
}

static void
out_var_text (SXINT var)
{
    if (to_be_memorized) {
	add_item (var);
    }
    else {
	fprintf (out_file, "X%ld", (SXINT) var-1);
    }
}

static void
out_arg (SXINT param)
{
    SXINT bot2, top2, y, symb;

    if (param != 0) {
	bot2 = XH_X (args_1rcg, param);
	top2 = XH_X (args_1rcg, param+1);

	for (y = bot2; y < top2; y++) {
	    if (!to_be_memorized && y != bot2)
		fputs (" ", out_file);

	    symb = XH_list_elem (args_1rcg, y);

	    if (symb < 0) {
		/* terminal L'element caracteristique vaut 1 */
		out_t_text (-symb);
	    }
	    else {
		if (symb > 0)
		    out_var_text (symb);
		else {
		    if (to_be_memorized)
			add_item (0);
		    else
			fputs ("...", out_file);
		}
	    }
	}
    }

    if (to_be_memorized)
	add_item (EOA);
}

static SXINT
is_sub_arg (SXINT lhs_param, SXINT rhs_param)
{
    /* retourne la position de rhs_param ds lhs_param ou -1 */
    SXINT lhs_bot3, lhs_top3, rhs_bot3, rhs_top3, rhs_size, lhs_cur3, lcur3, rcur3, k;

    lhs_bot3 = XH_X (rcg_parameters, lhs_param);
    lhs_top3 = XH_X (rcg_parameters, lhs_param+1);

    rhs_bot3 = XH_X (rcg_parameters, rhs_param);
    rhs_top3 = XH_X (rcg_parameters, rhs_param+1);
    rhs_size = rhs_top3 - rhs_bot3;

    for (k = 0, lhs_cur3 = lhs_bot3; lhs_cur3 <= lhs_top3-rhs_size; k++, lhs_cur3++) {
	for (lcur3 = lhs_cur3, rcur3 = rhs_bot3; rcur3 < rhs_top3 && lcur3 < lhs_top3; lcur3++, rcur3++) {
	    if (XH_list_elem (rcg_parameters, lcur3) != XH_list_elem (rcg_parameters, rcur3))
		break;
	}

	if (rcur3 == rhs_top3)
	    return k;
    }

    return -1;
}

static void
out_prdct_text (SXINT prdct)
{
    SXINT bot, cur, top, nt, param, bot2, cur2, top2, symb;

    bot = XH_X (rcg_predicates, prdct);
    top = XH_X (rcg_predicates, prdct+1);

    if (XH_list_elem (rcg_predicates, bot) == 1) {
	if (to_be_memorized)
	    add_item (1); /* neg_call */
	else
	    fputs ("!", out_file);
    }
    else {
	if (to_be_memorized)
	    add_item (0); /* pos_call */
    }

    nt = XH_list_elem (rcg_predicates, bot+1);
    out_nt_text (nt);

    if (!to_be_memorized) {
	fputs ("(", out_file);
    }

    cur = bot+2;
    
    while (cur < top) {
	param = XH_list_elem (rcg_predicates, cur);

	if (param != 0) {
	    bot2 = XH_X (rcg_parameters, param);
	    top2 = XH_X (rcg_parameters, param+1);
	    cur2 = bot2;

	    while (cur2 < top2) {
		symb = XH_list_elem (rcg_parameters, cur2);

		if (nt == STRLEN_ic && cur == bot+2) {
		    if (to_be_memorized)
			add_item (symb);
		    else
			fprintf (out_file, "%ld", (SXINT) symb);
		}
		else {
		    if (symb < 0)
			out_t_text (-symb);
		    else {
			if (symb > 0)
			    out_var_text (symb);
			else {
			    if (to_be_memorized)
				add_item (0);
			    else
				fputs ("...", out_file);
			}
		    }
		}

		cur2++;

		if (!to_be_memorized && cur2 < top2)
		    fputs (" ", out_file);
	    }
	}

	if (to_be_memorized)
	    add_item (EOA);

	cur++;

	if (!to_be_memorized && cur < top)
	    fputs (", ", out_file);
    }

    if (to_be_memorized)
	close_new_prdct();
    else
	fputs (")", out_file);
}

static void
out_clause_body (SXINT clause)
{
    SXINT rhs, bot, cur, top, ste;

    out_prdct_text (XxY_X (rcg_clauses, clause) & lhs_prdct_filter);

    if (!to_be_memorized)
	fputs (" --> ", out_file);

    rhs = XxY_Y (rcg_clauses, clause);
    top = XH_X (rcg_rhs, rhs+1);
    bot = XH_X (rcg_rhs, rhs);
    cur = bot;

    while (cur < top) {
	out_prdct_text (XH_list_elem (rcg_rhs, cur));
	
	cur++;

	if (!to_be_memorized && cur < top)
	    fputs (" ", out_file);
    }

    if (!to_be_memorized) {
	ste = clause2action [clause];

	if (ste == SXEMPTY_STE)
	    fputs (" .", out_file);
	else
	    fprintf (out_file, " . @%s", sxstrget (ste));
    }
}

static void
out_clause (SXINT clause)
{
    out_clause_body (clause);

    if (!to_be_memorized)
	fputs ("\n", out_file);
}

static void
out_clause_nb (SXINT clause, SXINT nb)
{
    out_clause_body (clause);

    if (to_be_memorized)
	close_new_clause (nb);
    else
	fprintf (out_file, " %% %ld\n", (SXINT) nb);
}

static bool
less_nt (SXINT nt1, SXINT nt2)
{
    /* Ordre : predicat de l'utilisateur puis StrEq puis les predefinis */
    if (nt1 < 0) {
	if (nt2 < 0) {
	    if (nt1 != STREQ_ic && nt2 != STREQ_ic)
		return nt1 < nt2;

	    return nt1 == STREQ_ic ? true : false;
	}

	return false;
    }

    return (nt2 < 0) ? true : (nt1 < nt2);
}

static int
compar (const SXINT *p1, const SXINT *p2)
{
    SXINT	bot1, bot2, top1, top2, cur1, cur2, arg1, arg2;
    SXINT x1 = *p1, x2 = *p2;

    /* On ne tient pas compte du type d'appel
    {
       SXINT k1, k2;

       if ((k1 = call_kinds [x1]) != (k2 = call_kinds [x2]))
	return (k1 < k2) ? -1 : 1;
    }
    */

    bot1 = XH_X (local_prdct_calls, x1);
    top1 = XH_X (local_prdct_calls, x1+1);

    bot2 = XH_X (local_prdct_calls, x2);
    top2 = XH_X (local_prdct_calls, x2+1);

    for (cur1 = bot1, cur2 = bot2; cur1 < top1 && cur2 < top2; cur1++, cur2++) {
	arg1 = XH_list_elem (local_prdct_calls, cur1);
	arg2 = XH_list_elem (local_prdct_calls, cur2);

	if (arg1 != arg2) {
	    if (cur1 == bot1)
		return (less_nt (arg1, arg2)) ? -1 : 1;

	    return (arg1 < arg2) ? -1 : 1;
	}
    }

    return 0; /* egalite */
}


static SXINT
guide_nt_mngr (SXINT nt, SXINT k)
{
    /* calcule et memorise le code interne qu'aura le nt nt_k ds le guide */
    SXINT pair, Ak;

    if (!XxY_set (&guide_nts, nt, k, &pair)) {
	/* nouveau */
	Ak = A_k2Ak (nt, k);
	Ak2guide_nt [Ak] = pair;
    }

    return pair;
}




static SXBA args_1rcg_set;

static void
args_1rcg_oflw (SXINT old_size, SXINT new_size)
{
    sxuse(old_size); /* pour faire taire gcc -Wunused */
    args_1rcg_set = sxba_resize (args_1rcg_set, new_size+1);
}


#define		ml	(max_prdct_nb/rule_nb)
#define		mp	3
#define		ms	3


void
one_rcg (void)
{
  SXINT 	clause, lhs_prdct, lhs_bot, lhs_top, lhs_cur, lhs_nt, lhs_param, lhs_pos, xbot, xtop, Ak, k;
  SXINT		rhs, rhs_cur, rhs_top, rhs_prdct, rhs_bot2, rhs_nt, rhs_param, rhs_param1, rhs_top2, rhs_pos, rhs_cur2;
  SXINT 	is_pos_call, xcall, x, bot3, top3, new_var, cur3, symb, bot, top, cur, y, len, high, low, IJ, I, J, AI;
  XH_header	XH_1rcg_clauses; /* INUTILE ds la version courante !! */
  SXBA	local_Ak_set, t_set, strlen_set;
  SXINT		*to_be_sorted, *var2new, *local_rhs_param, *call_pos;
  bool	rhs_ok, comment_clause, pushed, is_arg1, is_arg2, has_t;
  SXINT		rhs_pos2lhs_pos_top, rhs_pos2disp_top, rhs_arg_pos, rhs_pos_1rcg;
  SXINT		lhs_cur2, lhs_param2, lhs_cur3, lhs_bot3, lhs_top3, list_elem, size, rhs_top3, rhs_cur3;
  SXINT		*guide_A2guide_Aclause, guide_lhs_nt, guide_rhs_nt;
  /* Structures utilisees par le traitement de lrprod_file */
  /* Si on a une k-RCG, k > 2, les prod "du milieu" peuvent etre a la fois gauches et droites */
  /* Une production est gauche ssi ce n'est pas la "derniere" production generee par la clause (elle
     ne correspond pas a l'argument le + a droite).  Elle "predit" son argument suivant. */
  /* Donc, chaque production (gauche) predit un ensemble (eventuellement vide) de productions droites */
  SXBA          *left2right_prod_set, *right2left_prod_set;
  XH_header     prod_hd;
  SXINT           lognt, left_prod, right_prod, bot2, top2, xcur;
  bool       is_skipped;

  sxinitialise(right2left_prod_set); /* pour faire taire gcc -Wuninitialized */
  sxinitialise(left2right_prod_set); /* pour faire taire gcc -Wuninitialized */
  sxinitialise(lognt); /* pour faire taire gcc -Wuninitialized */
  sxinitialise(symb); /* pour faire taire gcc -Wuninitialized */
  sxinitialise(rhs_pos); /* pour faire taire gcc -Wuninitialized */
  sxinitialise (guide_rhs_nt); /* pour faire taire gcc -Wuninitialized */
  if (max_garity == 1) {
    if (sxverbosep) {
      fputs ("This grammar is already a 1-RCG.\n", sxtty);
      sxttycol1p = true;
    }

    return;
  }

  if (lrprod_file && !is_simple_grammar) {
    /* L'option "-lrprod lrprod_file_name" permet, si la RCG provient d'une TAG (ou + generalement
       une simple k-RCGs, k > 1) d'e'crire ds le fichier lrprod_file un certain nombres de structures C
       qui vont permettre d'acce'le'rer une analyse CF a` la Earley (guide, supertaggeur, ...) comme suit. */
    /* L'idee est la suivante : un arbre auxiliaire TAG produit 2 CF productions, une production gauche et
       une production droite.  Ces 2 productions doivent rester "lie'es".  En particulier, dans une analyse
       gauche-droite, la reconnaissace d'une production droite ne peut commencer que si sa production gauche
       associee a e'te' reconnue. */
    if (sxverbosep) {
      fputs ("WARNING: Only simple k-RCGs, k > 1 can generate an lrprod file.\n", sxtty);
      sxttycol1p = true;
    }

    return;
  }
    
  if (sxverbosep) {
    if (lrprod_file)
      fputs ("\t1-RCG & lrprod form .... ", sxtty);
    else
      fputs ("\t1-RCG .... ", sxtty);

    sxttycol1p = false;
  }

  if (is_full_lex)
    sprintf (lst_name, "%s_%s.fl_1rcg", prgentname, suffixname);
  else
    sprintf (lst_name, "%s_%s.1rcg", prgentname, suffixname);

  if ((out_file = fopen (lst_name, "w")) == NULL) {
    fprintf (sxstderr, "rcg -1rcg: cannot open (create) ");
    sxperror (lst_name);
    return;
  }

  out_header (rcg_header, lst_name, "a 1_rcg");

  if (lrprod_file) {
    lognt = sxlast_bit (max_nt*max_garity);

    left2right_prod_set = sxbm_calloc (max_clausek+1, max_clausek+1);
    right2left_prod_set = sxbm_calloc (max_clausek+1, max_clausek+1);
    XH_alloc (&prod_hd, "prod_hd", max_clausek+1, 1, max_rhs_prdct+1, NULL, statistics);
  }

  guide_clause2clause = (SXINT *) sxalloc (last_clause*max_garity+1, sizeof (SXINT));
  guide_clause2clause_id = (SXINT *) sxalloc (last_clause*max_garity+1, sizeof (SXINT));
  guide_clause2clause_id [0] = 0;
  guide_clause2arg_pos = (SXINT *) sxalloc (last_clause*max_garity+1, sizeof (SXINT));
    
  /* Dans tout les cas, on genere en premier une false clause qui assure l'egalite des codes des
     terminaux entre les 2 grammaires.  On ne s'occupe pas des codes des nt */
  /* Cette clause vient remplacer la premiere, si cette derniere est une false */
  if (SXBA_bit_is_set (false_clause_set, 1)) {
    init_clause_nb = 2;
    fputs ("\n%% 1: ", out_file);
    out_clause ((SXINT)1);
  }
  else
    init_clause_nb = 1;

  gen_false_clause (sxstrget (nt2ste [1]), "_1");
  guide_clause = 1;

  guide_clause2clause [1] = 0; /* On peut mettre clausek= (clause, lhs_pos) si necessaire */
  guide_clause2arg_pos [1] = 0;
  guide_clause2clause_id [1] = 0;


  local_Ak_set = sxba_calloc (max_Ak+1);
  local_rhs_param = (SXINT *) sxalloc (max_Ak+1, sizeof (SXINT));

  size = max_rhs_arg_nb+1; /* ATTENTION false() ne compte pas ds max_rhs_arg_nb */

  XH_alloc (&XH_1rcg_clauses, "XH_1rcg_clauses", last_clause*max_garity + 1, 1, size*5+3, NULL, statistics);
  XH_alloc (&local_prdct_calls, "local_prdct_calls", size+1, 1, size*5+1, NULL, statistics);
  XH_alloc (&args_1rcg, "args_1rcg", size+1, 1, ms, args_1rcg_oflw, statistics);
  args_1rcg_set = sxba_calloc (XH_size (args_1rcg)+1);
  strlen_set = sxba_calloc (size+1);

  call_kinds = (SXINT *) sxalloc (size+1, sizeof (SXINT));
  call_pos = (SXINT *) sxalloc (size+1, sizeof (SXINT));
  to_be_sorted = (SXINT *) sxalloc (size+1, sizeof (SXINT));
  var2new = (SXINT *) sxalloc (max_gvar+1, sizeof (SXINT));

  clause2rhs_pos = (SXINT *) sxalloc (last_clause+2, sizeof (SXINT));
  rhs_pos2disp = (SXINT *) sxalloc (last_clause*size+2, sizeof (SXINT));
  rhs_pos2rhs_nt = (SXINT *) sxalloc (last_clause*size+2, sizeof (SXINT));
  rhs_pos2lhs_pos = (SXINT *) sxalloc (last_clause*size*max_garity+1, sizeof (SXINT));
  rhs_pos2lhs_arg_no = (SXINT *) sxalloc (last_clause*size*max_garity+1, sizeof (SXINT));
  rhs_pos2rhs_pos_1rcg = (SXINT *) sxalloc (last_clause*size*max_garity+1, sizeof (SXINT));
  rhs_pos2lhs_pos_top = rhs_pos2disp_top = 1;
  rhs_pos2disp [rhs_pos2disp_top] = rhs_pos2lhs_pos_top;

  t_set = sxba_calloc (max_t+1);

  Ak2guide_nt = (SXINT *) sxalloc (max_Ak+1, sizeof (SXINT));
  XxY_alloc (&guide_nts, "guide_nts", max_Ak+1, 1, 0, 0, NULL, statistics);
  clausek2guide_Aclause = (SXINT *) sxalloc (max_clausek+1, sizeof (SXINT));
  clausek2guide_clause = (SXINT *) sxalloc (max_clausek+1, sizeof (SXINT));
  guide_A2guide_Aclause = (SXINT *) sxcalloc (max_Ak+1, sizeof (SXINT));

  clause2rhs_pos [1] = rhs_pos2disp_top; /* Si init_clause_nb == 2 */

  for (clause = init_clause_nb; clause <= last_clause; clause++) {
    clause2rhs_pos [clause] = rhs_pos2disp_top;

    /* Le 13/6/2002 On ne genere rien sur les false et les loop */
    if (clause2identical [clause] == clause
	&& !SXBA_bit_is_set (false_clause_set, clause)
	&& !SXBA_bit_is_set (loop_clause_set, clause)) {
      rhs = XxY_Y (rcg_clauses, clause);

      if (rhs > 0) {
	lhs_prdct = XxY_X (rcg_clauses, clause) & lhs_prdct_filter;
	lhs_bot = XH_X (rcg_predicates, lhs_prdct);
	lhs_top = XH_X (rcg_predicates, lhs_prdct+1);

	for (rhs_top = XH_X (rcg_rhs, rhs+1), rhs_cur = XH_X (rcg_rhs, rhs); rhs_cur < rhs_top; rhs_cur++) {
	  rhs_prdct = XH_list_elem (rcg_rhs, rhs_cur);
	  rhs_bot2 = XH_X (rcg_predicates, rhs_prdct);
	  rhs_top2 = XH_X (rcg_predicates, rhs_prdct+1);

	  rhs_nt = XH_list_elem (rcg_predicates, rhs_bot2+1);

	  for (rhs_cur2 = rhs_bot2+2; rhs_cur2 < rhs_top2; rhs_cur2++) {
	    rhs_param = XH_list_elem (rcg_predicates, rhs_cur2);

	    if (rhs_nt < 0 && ((rhs_nt == STRLEN_ic && rhs_cur2 == rhs_bot2+2) /* Le 1er arg de &StrLen est un entier */ ||
			       (rhs_nt == FIRST_ic && rhs_cur2 == rhs_bot2+2) /* Le 1er arg de &First est ds T* */ ||
			       (rhs_nt == LAST_ic && rhs_cur2 == rhs_bot2+2) /* Le 1er arg de &Last est ds T* */ ||
			       rhs_nt == LEX_ic /* un seul arg ds T* */ ||
			       is_arg_in_Tstar (rhs_param) == 1
			       )
		) {
	      rhs_pos2lhs_pos [rhs_pos2lhs_pos_top] = -1; /* not a range */
	      rhs_pos2rhs_pos_1rcg [rhs_pos2lhs_pos_top] = -1; /* initialisation */
	      rhs_pos2lhs_arg_no [rhs_pos2lhs_pos_top++] = -1;
	    }
	    else {
	      for (lhs_cur = lhs_bot+2, lhs_pos = 0; lhs_cur < lhs_top; lhs_pos++, lhs_cur++) {
		lhs_param = XH_list_elem (rcg_predicates, lhs_cur);

		if ((k = is_sub_arg (lhs_param, rhs_param)) >= 0) {
		  rhs_pos2lhs_pos [rhs_pos2lhs_pos_top] = k;
		  rhs_pos2rhs_pos_1rcg [rhs_pos2lhs_pos_top] = -1; /* initialisation */
		  rhs_pos2lhs_arg_no [rhs_pos2lhs_pos_top++] = lhs_pos;
		}
	      }
	    }

	    rhs_pos2rhs_nt [rhs_pos2disp_top] = rhs_nt;
	    rhs_pos2disp [++rhs_pos2disp_top] = rhs_pos2lhs_pos_top;
	  }
	}
      }
    }
  }

  clause2rhs_pos [clause] = rhs_pos2disp_top;
    
  for (clause = init_clause_nb; clause <= last_clause; clause++) {
    /* Le 13/6/2002 On ne genere rien sur les false et les loop */
    if (clause2identical [clause] == clause
	&& !SXBA_bit_is_set (false_clause_set, clause)
	&& !SXBA_bit_is_set (loop_clause_set, clause)) {
      left_prod = 0;
      fprintf (out_file, "\n%% %ld: ", (SXINT) clause);
      out_clause (clause);

#if 0
      /* On ne produit rien ds la 1-RCG pour les clauses dupliquees */
      if (!check_set && has_identical_clauses && SXBA_bit_is_set (has_an_identical_clause_set, clause)) {
	fputs ("% Duplicate clause\n", out_file);
	continue;
      }
#endif
	
      lhs_prdct = XxY_X (rcg_clauses, clause) & lhs_prdct_filter;
      lhs_bot = XH_X (rcg_predicates, lhs_prdct);
      lhs_nt = XH_list_elem (rcg_predicates, lhs_bot+1);

      has_t = false;

      rhs = XxY_Y (rcg_clauses, clause);

      for (lhs_top = XH_X (rcg_predicates, lhs_prdct+1), lhs_cur = lhs_bot+2, lhs_pos = 0;
	   lhs_cur < lhs_top;
	   lhs_pos++, lhs_cur++) {
	lhs_param = XH_list_elem (rcg_predicates, lhs_cur);
	rhs_ok = true;
	xcall = 0;

	if (lhs_param > 0 && rhs > 0) {
	  rhs_arg_pos = 0;

	  for (rhs_top = XH_X (rcg_rhs, rhs+1), rhs_cur = XH_X (rcg_rhs, rhs); rhs_cur < rhs_top; rhs_cur++) {
	    rhs_prdct = XH_list_elem (rcg_rhs, rhs_cur);
	    rhs_bot2 = XH_X (rcg_predicates, rhs_prdct);
	    rhs_top2 = XH_X (rcg_predicates, rhs_prdct+1);

	    is_pos_call = (XH_list_elem (rcg_predicates, rhs_bot2) == 0); /* 1 => neg call */
	    rhs_nt = XH_list_elem (rcg_predicates, rhs_bot2+1);

	    if (rhs_nt > 0 || rhs_nt == CUT_ic) {
	      for (rhs_pos = 0, rhs_cur2 = rhs_bot2+2; rhs_cur2 < rhs_top2; rhs_pos++, rhs_cur2++) {
		rhs_param = XH_list_elem (rcg_predicates, rhs_cur2);

		if (is_sub_arg (lhs_param, rhs_param) >= 0) {
		  XH_push (local_prdct_calls, (rhs_nt < 0) ? rhs_nt : A_k2Ak (rhs_nt, rhs_pos));
		  XH_push (local_prdct_calls, rhs_param);

		  if (!XH_set (&local_prdct_calls, &x)) {
		    /* nouveau */
		    call_kinds [x] = is_pos_call;
		    xcall = x;
		    call_pos [x] = rhs_arg_pos+rhs_pos;
		  }
		  else {
		    /* Si l'un est neg et l'autre pos, on supprime la clause */
		    if (call_kinds [x] != is_pos_call) {
		      rhs_ok = false;
		      break;
		    }
		  }
		}
	      }

	      if (rhs_ok == false)
		break;
	    }
	    else {
	      pushed = false;

	      if (rhs_nt == STREQ_ic || rhs_nt == STREQLEN_ic) {
		is_arg2 = is_sub_arg (lhs_param, rhs_param = XH_list_elem (rcg_predicates, rhs_bot2+3)) >= 0;
		is_arg1 = is_sub_arg (lhs_param, rhs_param1 = XH_list_elem (rcg_predicates, rhs_bot2+2)) >= 0;

		if (is_arg1 || is_arg2) {
		  if (is_arg1 && is_arg2) {
		    XH_push (local_prdct_calls, rhs_nt);
		    XH_push (local_prdct_calls, rhs_param1);
		    XH_push (local_prdct_calls, rhs_param);
		  }
		  else {
		    /* Pour que le mapping RCG guidee <--> guide reste coherent, on
		       genere &True (arg). */
		    XH_push (local_prdct_calls, TRUE_ic);
		    XH_push (local_prdct_calls, is_arg1 ? rhs_param1 : rhs_param);
		  }

		  pushed = true;
		}
	      }
	      else {
		if (rhs_nt == STRLEN_ic || rhs_nt == FIRST_ic || rhs_nt == LAST_ic) {
		  rhs_param = XH_list_elem (rcg_predicates, rhs_bot2+3);

		  if (is_sub_arg (lhs_param, rhs_param) >= 0) {
		    rhs_param1 = XH_list_elem (rcg_predicates, rhs_bot2+2);
		    XH_push (local_prdct_calls, rhs_nt);

		    if (rhs_nt == STRLEN_ic)
		      XH_push (local_prdct_calls, XH_list_elem (rcg_parameters, XH_X (rcg_parameters, rhs_param1)));
		    else
		      XH_push (local_prdct_calls, rhs_param1);

		    XH_push (local_prdct_calls, rhs_param);
		    pushed = true;
		  }
		}
		else {
		  if (rhs_nt == FALSE_ic) {
		    XH_push (local_prdct_calls, rhs_nt);
		    pushed = true;
		  }
		  else {
		    if (rhs_nt == TRUE_ic) {
		      if (is_sub_arg (lhs_param, rhs_param1 = XH_list_elem (rcg_predicates, rhs_bot2+2)) >= 0) {
			XH_push (local_prdct_calls, rhs_nt);
			XH_push (local_prdct_calls, rhs_param1);
			pushed = true;
		      }
		    }
		    else {
		      if (rhs_nt == LEX_ic) {
			rhs_param = XH_list_elem (rcg_predicates, rhs_bot2+2);
			rhs_top3 = XH_X (rcg_parameters, rhs_param+1);

			for (rhs_cur3 = XH_X (rcg_parameters, rhs_param); rhs_cur3 < rhs_top3; rhs_cur3++) {
			  symb = -XH_list_elem (rcg_parameters, rhs_cur3);
			  has_t = true;
			  SXBA_1_bit (t_set, symb);
			}
		      }
		    }
		  }
		}
	      }

	      if (pushed) {
		if (!XH_set (&local_prdct_calls, &x)) {
		  /* nouveau */
		  call_kinds [x] = is_pos_call;
		  xcall = x;
		  call_pos [x] = rhs_arg_pos;
		}
		else {
		  /* Si l'un est neg et l'autre pos, on supprime la clause */
		  if (call_kinds [x] != is_pos_call) {
		    rhs_ok = false;
		    break;
		  }
		}
	      }
	    }

	    rhs_arg_pos += rhs_top2-rhs_bot2-2;
	  }
	}

	if (rhs_ok) {
	  /* On renomme les variables */
	  if (lhs_param > 0) {
	    bot3 = XH_X (rcg_parameters, lhs_param);
	    top3 = XH_X (rcg_parameters, lhs_param+1);

	    new_var = 1;

	    for (cur3 = bot3; cur3 < top3; cur3++) {
	      symb = XH_list_elem (rcg_parameters, cur3);

	      if (symb > 0) {
		var2new [symb] = new_var;
		symb = new_var++;
	      }

	      XH_push (args_1rcg, symb);
	    }

	    XH_set (&args_1rcg, &lhs_param);
	  }

	  if (xcall > 0) {
	    /* RHS non vide */
	    /* On renomme les variables */
	    for (x = 1; x <= xcall; x++) {
	      bot = XH_X (local_prdct_calls, x);
	      top = XH_X (local_prdct_calls, x+1);

	      rhs_nt = XH_list_elem (local_prdct_calls, bot);
	      rhs_param = 0; /* prudence */

	      for (cur = bot+1; cur < top; cur++) {
		rhs_param = XH_list_elem (local_prdct_calls, cur);

		if ((rhs_nt == STRLEN_ic) && cur == bot+1) {
		  /* le 1er arg est OK */
		}
		else {
		  bot3 = XH_X (rcg_parameters, rhs_param);
		  top3 = XH_X (rcg_parameters, rhs_param+1);

		  for (cur3 = bot3; cur3 < top3; cur3++) {
		    symb = XH_list_elem (rcg_parameters, cur3);

		    if (symb > 0) {
		      symb = var2new [symb];
		    }

		    XH_push (args_1rcg, symb);
		  }

		  XH_set (&args_1rcg, &rhs_param);
		  /* Attention: maintenant (sauf pour STRLEN_ic)
		     les arg de local_prdct_calls sont des index ds args_1rcg et non plus
		     ds rcg_parameters */
		  XH_list_elem (local_prdct_calls, cur) = rhs_param;
		}
	      }
	    }

	    for (x = 1; x <= xcall; x++) {
	      to_be_sorted [x] = x;
	    }

	    if (xcall > 1) {
	      /* on reordonne les appels de predicats */
	      /* void qsort(void *base, size_t nmemb, size_t size, SXINT (*compar)(const void *, const void *)) */
	      qsort (to_be_sorted+1, xcall, sizeof (SXINT), (int (*) (const void *, const void *))compar);
	    }
	  }
		
	  if (rhs_ok) {
	    if (xcall > 0) {
	      rhs_pos_1rcg = 0;

	      for (x = 1; x <= xcall; x++) {
		/* Tous les appels sont differents */
		y = to_be_sorted [x];

		XH_push (XH_1rcg_clauses, call_kinds [y]);
		rhs_arg_pos = call_pos [y];

		bot = XH_X (local_prdct_calls, y);
		top = XH_X (local_prdct_calls, y+1);

		for (cur = bot; cur < top; cur++) {
		  list_elem = XH_list_elem (local_prdct_calls, cur);
		  XH_push (XH_1rcg_clauses, list_elem);

		  if (cur > bot) {
		    /* c'est un arg qui se trouve en position rhs_pos_1rcg ds la 1rcg
		       et en rhs_arg_pos ds la rcg */
		    xbot = rhs_pos2disp [clause2rhs_pos [clause]+rhs_arg_pos];
		    xtop = rhs_pos2disp [clause2rhs_pos [clause]+rhs_arg_pos+1];

		    while (xbot < xtop) {
		      if (rhs_pos2lhs_arg_no [xbot] == lhs_pos) {
			rhs_pos2rhs_pos_1rcg [xbot] = rhs_pos_1rcg;
			rhs_arg_pos++;
			rhs_pos_1rcg++;
			break;
		      }

		      xbot++;
		    }
		  }
		}
	      }
	    }
	  }
	}

	if (rhs_ok) {
	  XH_set (&XH_1rcg_clauses, &x);
	  guide_lhs_nt = guide_nt_mngr (lhs_nt, lhs_pos);

	  /* On peut generer 2 clauses identiques */
	  /* On ne fait pas le test pour que le mapping inverse clause du guide -> (clause,k) du parser guide'
	     soit une fonction */
	  comment_clause = false;

	  if (comment_clause)
	    fputs ("% ", out_file);
	  else {
	    guide_clause++;
	    guide_clause2clause [guide_clause] = clause; /* On peut mettre clausek= (clause, lhs_pos) si necessaire */
	    guide_clause2arg_pos [guide_clause] = lhs_pos;
	    guide_clause2clause_id [guide_clause] = ++guide_A2guide_Aclause [guide_lhs_nt];
	    /* Permet le mapping guide'/guide */
	    clausek2guide_Aclause [clause_k2clausek (clause, lhs_pos)] = guide_A2guide_Aclause [guide_lhs_nt];
	    clausek2guide_clause [clause_k2clausek (clause, lhs_pos)] = guide_clause;
	  }

	  if (lrprod_file) {
	    is_skipped = false;
	    XH_push (prod_hd, guide_lhs_nt);

	    if (lhs_param != 0) {

	      xbot = XH_X (XH_1rcg_clauses, x);
	      xtop = XH_X (XH_1rcg_clauses, x+1);

	      bot2 = XH_X (args_1rcg, lhs_param);
	      top2 = XH_X (args_1rcg, lhs_param+1);

	      for (y = bot2; y < top2; y++) {
		symb = XH_list_elem (args_1rcg, y);

		if (symb < 0) {
		  XH_push (prod_hd, symb);
		}
		else {
		  xcur = xbot;

		  while (xcur < xtop) {
		    if (XH_list_elem (XH_1rcg_clauses, xcur) == 0)
		      /* Negatif call!! */
		      sxtrap (ME, "one_rcg");
		    
		    xcur++; /* sur le predicat */
		    Ak = XH_list_elem (XH_1rcg_clauses, xcur);

		    xcur++; /* sur le 1er arg */
		    rhs_param = XH_list_elem (XH_1rcg_clauses, xcur);

		    if (Ak < 0)
		      sxtrap (ME, "one_rcg");

		    if (XH_list_elem (args_1rcg, XH_X (args_1rcg, rhs_param)) == symb) {
		      rhs_nt = Ak2A (Ak);
		      rhs_pos = Ak_A2k (Ak, rhs_nt);
		      guide_rhs_nt = guide_nt_mngr (rhs_nt, rhs_pos);
		      XH_push (prod_hd, guide_rhs_nt);
		      break;
		    }

		    xcur++;
		  }

		  if (xcur == xtop)
		    sxtrap (ME, "one_rcg");
		}
	      }

	      if (top2-bot2==1 && symb>0 && guide_lhs_nt == guide_rhs_nt) {
		/* Loop */
		/* La prod ne sera pas generee... */
		is_skipped = true;
		XH_pop (prod_hd, guide_rhs_nt);
		XH_pop (prod_hd, guide_lhs_nt);
	      }
	      else {
		/* Ici les args sont epuises, il reste les predicats systeme */

		while (xbot < xtop) {
		  if (XH_list_elem (XH_1rcg_clauses, xbot) == 0)
		    /* Negatif call!! */
		    sxtrap (ME, "one_rcg");

		  xbot++; /* sur le predicat */
		  rhs_nt = XH_list_elem (XH_1rcg_clauses, xbot);

		  if (rhs_nt > 0) {
		    xbot += 2;
		  }
		  else {
		    if (rhs_nt != FALSE_ic) {
		      if (rhs_nt == STRLEN_ic) {
			rhs_param = XH_list_elem (XH_1rcg_clauses, xbot);
			XH_push (prod_hd, rhs_param<<lognt); /* Simulation des appels de predicats... */
		      }

		      xbot++; /* sur le 2eme arg (ou le type d'appel du predicat suivant) */

		      if (rhs_nt == STRLEN_ic || rhs_nt == FIRST_ic || rhs_nt == LAST_ic ||
			  rhs_nt == STREQ_ic || rhs_nt == STREQLEN_ic) {
			/* Arity == 2 */
			xbot++; /* sur le type d'appel du predicat suivant */
		      }
		    }
		  }
		}
	      }
	    }

	    if (!is_skipped) {
	      /* Si 2 prod sont identiques, elles proviennent de la traduction du meme arg de A-clauses differentes */
	      XH_set (&prod_hd, &right_prod);

#if 0
	      /* debug */
	      fprintf (stderr, "clause=%ld, guide_clause=%ld, pos=%ld, prod=%ld\n", clause, guide_clause, lhs_pos, right_prod);
#endif

	      /* Ici on a lhs_pos = [0, 1, 2, ..., arity-1] */
	      if (left_prod > 0) {
		SXBA_1_bit (left2right_prod_set [left_prod], right_prod);
		SXBA_1_bit (right2left_prod_set [right_prod], left_prod);
	      }

	      left_prod = right_prod;
	    }
#if 0
	    /* debug */
	    else {
	      fprintf (stderr, "clause=%ld, guide_clause=%ld, pos=%ld, SKIPPED\n", clause, guide_clause, lhs_pos);
	    }
#endif
	  }

	  out_nt_text (lhs_nt);
#if 0
	  fprintf (out_file, "_%ld_%ld(", lhs_nt, lhs_pos);
#endif
	  fprintf (out_file, "_%ld(", (SXINT) guide_lhs_nt);

	  out_arg (lhs_param);
	  fputs (") --> ", out_file);

	  sxba_empty (args_1rcg_set);

	  xbot = XH_X (XH_1rcg_clauses, x);
	  xtop = XH_X (XH_1rcg_clauses, x+1);

	  while (xbot < xtop) {
	    is_pos_call = XH_list_elem (XH_1rcg_clauses, xbot);
	    xbot++;

	    Ak = XH_list_elem (XH_1rcg_clauses, xbot);
	    xbot++;

	    if (Ak > 0) {
	      rhs_nt = Ak2A (Ak);
	      rhs_pos = Ak_A2k (Ak, rhs_nt);

	      if (!comment_clause) {
		guide_rhs_nt = guide_nt_mngr (rhs_nt, rhs_pos);
	      }
	    }
	    else {
	      rhs_nt = Ak;

	      /* Si l'appel courant est de la forme &True(arg) et si arg a deja ete produit, il est
		 inutile de generer cet appel */
	      if (rhs_nt == TRUE_ic) {
		rhs_param = XH_list_elem (XH_1rcg_clauses, xbot);

		if (SXBA_bit_is_set (args_1rcg_set, rhs_param)) {
		  rhs_nt = 0;
		  xbot++;
		}
	      }
	    }

	    if (rhs_nt != 0) {
	      fprintf (out_file, "%s", (is_pos_call == 1 /* is_pos_call */) ? "" : "!");
	      out_nt_text (rhs_nt);

	      if (rhs_nt > 0) {
#if 0
		fprintf (out_file, "_%ld_%ld", rhs_nt, rhs_pos);
#endif
		fprintf (out_file, "_%ld", (SXINT) guide_rhs_nt);
	      }

	      fputs ("(", out_file);

	      if (rhs_nt != FALSE_ic) {
		rhs_param = XH_list_elem (XH_1rcg_clauses, xbot);
		xbot++;

		if (rhs_nt == STRLEN_ic)
		  fprintf (out_file, "%ld", (SXINT) rhs_param);
		else {
		  SXBA_1_bit (args_1rcg_set, rhs_param);
		  out_arg (rhs_param);
		}

		if (rhs_nt == STRLEN_ic || rhs_nt == FIRST_ic || rhs_nt == LAST_ic ||
		    rhs_nt == STREQ_ic || rhs_nt == STREQLEN_ic) {
		  /* Arity == 2 */
		  rhs_param = XH_list_elem (XH_1rcg_clauses, xbot);

		  if (is_pos_call && rhs_nt == STRLEN_ic)
		    SXBA_1_bit (strlen_set, rhs_param);

		  xbot++;
			    
		  fputs (", ", out_file);
		  out_arg (rhs_param);
		}
	      }

	      fputs (") ", out_file);
	    }
	  }

	  /* Les &StrLen Ajoutes sont mis en fin de clause pour ne pas perturber les positions
	     des autres args */
	  xbot = XH_X (XH_1rcg_clauses, x);

	  while (xbot < xtop) {
	    xbot++;
	    Ak = XH_list_elem (XH_1rcg_clauses, xbot);
	    xbot++;

	    if (Ak > 0) {
	      rhs_nt = Ak2A (Ak);
	      rhs_pos = Ak_A2k (Ak, rhs_nt);
	    }
	    else
	      rhs_nt = Ak;

	    if (rhs_nt != FALSE_ic) {
	      rhs_param = XH_list_elem (XH_1rcg_clauses, xbot);

	      if (Ak > 0) {
		if (SXBA_bit_is_reset_set (local_Ak_set, Ak))
		  local_rhs_param [Ak] = rhs_param;
	      }

	      xbot++;

	      if (rhs_nt == STRLEN_ic || rhs_nt == FIRST_ic || rhs_nt == LAST_ic ||
		  rhs_nt == STREQ_ic || rhs_nt == STREQLEN_ic) {
		/* Arity == 2 */
		xbot++;
	      }
	    }

	    if (Ak > 0 && (len = Ak2len [Ak]) != -1 && !SXBA_bit_is_set (strlen_set, rhs_param)) {
	      /* On genere &StrLen (len, arg (Ak))  */
	      SXBA_1_bit (strlen_set, rhs_param);
	      fprintf (out_file, "&StrLen(%ld, ", (SXINT) len);
	      out_arg (rhs_param);
	      fputs (") ", out_file);
	    }

	    if (Ak > 0) {
	      high = nt2IeqJ_disp [rhs_nt+1];
	      low = nt2IeqJ_disp [rhs_nt];

	      if (low < high) {
		do {
		  IJ = nt2IeqJ [low];
		  J = IJ % max_garity;

		  if (J == rhs_pos) {
		    I = IJ / max_garity; /* I < J */
		    /* On recherche si AI a ete genere */
		    AI = A_k2Ak (rhs_nt, I);

		    if (SXBA_bit_is_set (local_Ak_set, AI)) {
		      /* On genere &StrEqLen (arg (AI), arg (Ak))  */
		      I = local_rhs_param [AI];

		      if (!SXBA_bit_is_set (strlen_set, I) && !SXBA_bit_is_set (strlen_set, rhs_param)) {
			fputs ("&StrEqLen(", out_file);
			out_arg (I);
			fputs (", ", out_file);
			out_arg (rhs_param);
			fputs (") ", out_file);
		      }
		    }
		  }
		} while (++low < high);
	      }
	    }
	  }

	  if (is_full_lex || (nt2arity [lhs_nt] > 1)) {
	    /* ... puis on genere les &Lex */
	    for (lhs_cur2 = lhs_bot+2;lhs_cur2 < lhs_top; lhs_cur2++) {
	      if (is_full_lex || (lhs_cur2 != lhs_cur)) {
		lhs_param2 = XH_list_elem (rcg_predicates, lhs_cur2);

		if (lhs_param2 > 0) {
		  lhs_bot3 = XH_X (rcg_parameters, lhs_param2);
		  lhs_top3 = XH_X (rcg_parameters, lhs_param2+1);

		  for (lhs_cur3 = lhs_bot3; lhs_cur3 < lhs_top3; lhs_cur3++) {
		    symb = -XH_list_elem (rcg_parameters, lhs_cur3);

		    if (symb > 0) {
		      SXBA_1_bit (t_set, symb);
		      has_t = true;
		    }
		  }
		}
	      }
	    }
	  }

	  if (has_t) {
	    has_t = false;
	    fputs ("&Lex(", out_file);
	    symb = sxba_scan_reset (t_set, 0);

	    do {
	      out_t_text (symb);
	      symb = sxba_scan_reset (t_set, symb);

	      if (symb > 0)
		fputs (" ", out_file);
	    } while (symb > 0);

	    fputs (") ", out_file);
	  }
		
	  if (!comment_clause)
	    fprintf (out_file, ". %% %ld\n", (SXINT) guide_clause);
	  else 
	    fputs (".\n", out_file);
		
	  sxba_empty (local_Ak_set);
	}

	XH_clear (&XH_1rcg_clauses);
	XH_clear (&local_prdct_calls);
	XH_clear (&args_1rcg);
	sxba_empty (strlen_set);
      }
    }
    else {
      fprintf (out_file, "\n%% %ld: ", (SXINT) clause);
      out_clause (clause);

      if (SXBA_bit_is_set (false_clause_set, clause)) {
	fprintf (out_file, "\n%% %ld is a &False clause.\n", (SXINT) clause);
      }
      else {
	if (SXBA_bit_is_set (loop_clause_set, clause)) {
	  fprintf (out_file, "\n%% %ld is a loop clause.\n", (SXINT) clause);
	}
	else {
	  fprintf (out_file, "\n%% %ld is identical to clause #%ld\n", (SXINT) clause, (SXINT) clause2identical [clause]);
	}
      }
    }
  }

  guide_lhs_nt = XxY_top (guide_nts);
  guide_lognt = sxlast_bit (guide_lhs_nt);

  guide_max_clause_id = 0;

  for (lhs_nt = 1; lhs_nt <= guide_lhs_nt; lhs_nt++) {
    if ((k = guide_A2guide_Aclause [lhs_nt]) > guide_max_clause_id)
      guide_max_clause_id = k;
  }

  XH_free (&XH_1rcg_clauses);
  XH_free (&local_prdct_calls);
  XH_free (&args_1rcg);
  sxfree (args_1rcg_set), args_1rcg_set = NULL;
  sxfree (strlen_set);
  sxfree (call_kinds);
  sxfree (call_pos);
  sxfree (to_be_sorted);
  sxfree (var2new);
  sxfree (local_Ak_set);
  sxfree (local_rhs_param);
  sxfree (t_set);
  XxY_free (&guide_nts);
  sxfree (guide_A2guide_Aclause);

  fclose (out_file);

  
  if (lrprod_file) {
    SXINT max_prod, prod, left_prod_nb, right_prod_nb, prod_nb, one_rcg_left_prod, one_rcg_right_prod;
    SXUINT v;
    SXBA line;

#define X0 (0U)
#define XFF (~X0)
#define X7F (XFF>>1)
#define X1F (XFF>>2)
#define X80 (~X7F)
#define X40 (X80>>1)
#define X20 (X80>>2)
#define IS_LEFT_PROD X80
#define IS_RIGHT_PROD X40
#define IS_SINGLE_PROD X20

    out_file = lrprod_file;

    out_header (c_header, lrprod_file_name, "the lrprod structures");

    fputs ("/* Il faut verifier que ce nombre est identique au nb de productions de la CFG courante... */\n", out_file);
    fprintf (out_file, "#define LEFT_RIGHT_PROD_NB %ld\n\n", (SXINT) (max_prod = XH_top (prod_hd)-1));

    fputs ("#define X0 (0U)\n", out_file);
    fputs ("#define XFF (~X0)\n", out_file);
    fputs ("#define X7F (XFF>>1)\n", out_file);
    fputs ("#define X1F (XFF>>3)\n", out_file);
    fputs ("#define X80 (~X7F)\n", out_file);
    fputs ("#define X40 (X80>>1)\n", out_file);
    fputs ("#define X20 (X80>>2)\n", out_file);
    fputs ("#define IS_LEFT_PROD X80\n", out_file);
    fputs ("#define IS_RIGHT_PROD X40\n", out_file);
    fputs ("#define IS_SINGLE_PROD X20\n", out_file);
    fputs ("#define EXTRACT_PROD_BODY X1F\n", out_file);

    fputs ("/* Soit p une production et v = left_right_prod_hd [p].  Si :\n\
v == 0 ==> p provient d'un arbre initial\n\
v & IS_LEFT_PROD == 1 <==> p est une production gauche\n\
v & IS_RIGHT_PROD == 1 <==> p est une production droite\n\
v & IS_SINGLE_PROD == 1 <==> p est associe avec une prod unique v & EXTRACT_PROD_BODY.\n\
Attention, une production peut etre a la fois droite et gauche si on a une simple k-PRCG avec k > 2.\n\
v & EXTRACT_PROD_BODY est l'indice ds left_right_prod_list de la liste des productions associee a p\n\
Chacune de ces productions peut etre gauche ou droite, la liste est terminee par 0. */\n"
	   , out_file);
    fputs ("\nstatic SXUINT left_right_prod_hd [] = {0,\n", out_file);

    top = 1;

    for (prod = 1; prod <= max_prod; prod++) {
      right_prod_nb = sxba_cardinal (left2right_prod_set [prod]);
      left_prod_nb = sxba_cardinal (right2left_prod_set [prod]);
      prod_nb = left_prod_nb+right_prod_nb;

      v = X0;

      if (right_prod_nb > 0) {
	/* prod est gauche */
	v |= IS_LEFT_PROD;
      }

      if (left_prod_nb > 0) {
	/* prod est droite */
	v |= IS_RIGHT_PROD;
      }

      if (prod_nb > 1) {
	v |= top;
	top += 1 + prod_nb;
      }
      else {
	if (prod_nb == 1) {
	  v |= IS_SINGLE_PROD |
	       ((left_prod_nb) ? sxba_scan (right2left_prod_set [prod], 0) : sxba_scan (left2right_prod_set [prod], 0));
	}
      }

      fprintf (out_file, "/* %ld */ 0X%lX,\n", (SXINT) prod, (SXINT) v);
    }

    fputs ("};\n", out_file);

    fputs ("\nstatic SXUINT left_right_prod_list [] = {0,\n", out_file);

    for (prod = 1; prod <= max_prod; prod++) {
      right_prod_nb = sxba_cardinal (left2right_prod_set [prod]);
      left_prod_nb = sxba_cardinal (right2left_prod_set [prod]);

      if (right_prod_nb + left_prod_nb > 1) {
	fprintf (out_file, "/* %ld */ ", (SXINT) prod);

	if (right_prod_nb) {
	  one_rcg_right_prod = 0;
	  line = left2right_prod_set [prod];
	  
	  while ((one_rcg_right_prod = sxba_scan (line, one_rcg_right_prod)) > 0) {
	    fprintf (out_file, "0X%lX, ", (SXINT) one_rcg_right_prod|IS_RIGHT_PROD);
	  }
	}

	if (left_prod_nb) {
	  one_rcg_left_prod = 0;
	  line = right2left_prod_set [prod];
	  
	  while ((one_rcg_left_prod = sxba_scan (line, one_rcg_left_prod)) > 0) {
	    fprintf (out_file, "0X%lX, ", (SXINT) one_rcg_left_prod|IS_LEFT_PROD);
	  }
	}

	fputs ("0,\n", out_file);
      }
    }

    fputs ("};\n", out_file);

    fclose (out_file);

    sxbm_free (left2right_prod_set);
    sxbm_free (right2left_prod_set);
    XH_free (&prod_hd);
  }

  if (sxverbosep) {
    fputs ("done\n", sxtty);
    sxttycol1p = true;
  }
}

/* ******************************************************************************************************* */
static SXINT	min_degree, cover_size;
static SXBA	t_set, local_t_set, var_set, known_lgth_var_set;
static bool	is_t_set, is_local_t_set, has_known_lgth_var, block_extraction, previous_block_extraction;

static SXINT	arg2bot_top_top, arg2bot_top_size, new_var_nb;
static SXINT	*pos2symb, *slice2disp, *slice2new_clause, *var2eq, *var2lgth, *var_stack, *equal_pair_stack, *streqlen_pair_stack;
static SXINT	*var2eq_len, *eq2nb;
static SXBA	eq_set, local_eq_set, best_eq_set, previous_best_eq_set, close_eq_set, cover_set, eq_len_set;

static struct	new_nts {
    SXINT	old_clause, new_clause;
    } *new_nts;
static SXINT	*nt2ident, new_nts_size, new_nts_top, *slice2new_nt;

static SXINT	MAX_DEGREE;

static struct arg2bot_top {
    SXINT bot, top;
} *arg2bot_top, *cover;



static void
gen_symb (SXINT symb, bool *is_first)
{
    if (!to_be_memorized && !*is_first)
	fputs (" ", out_file);

    if (symb > 0) {
	out_var_text (symb);
	SXBA_1_bit (var_set, symb);
    }
    else {
	symb = -symb;
	out_t_text (symb);

	SXBA_1_bit (local_t_set, symb);
	is_local_t_set = true;
    }

    *is_first = false;
}


static void
gen_arg (SXINT bot, SXINT top, bool *is_first)
{
    SXINT pos;

    for (pos = bot; pos < top; pos++) {
	gen_symb (pos2symb [pos], is_first);
    }
}


static SXINT
get_super_var (SXINT lpos, SXINT rpos)
{
    /* On regarde si la super-var entre lpos et rpos n'a pas deja ete generee */
    SXINT svar, bot, top, cur1, cur2;

    for (svar = max_gvar+1; svar <= new_var_nb; svar++) {
	bot = cover [svar].bot;
	top = cover [svar].top;

	if (rpos-lpos == top-bot) {
	    for (cur1 = bot, cur2 = lpos; cur1 < top; cur1++, cur2++) {
		if (pos2symb [cur1] != pos2symb [cur2])
		    break;
	    }

	    if (cur1 == top)
		return svar;
	}
    }

    return 0;
}


#define m_weight	100
#define sv_weight	101


static SXINT
lhs_gen_or_degree (struct arg2bot_top *p, SXINT arity, bool is_gen/* , is_main */)
{
    SXINT		degree, arg, pos, top, bot, cur, lpos, rpos, symb, eq, super_var_nb, eq_len;
    bool	is_first;
	
    /* Le Mar  5 Dec 2000 13:45:09 */
    /* Les variables d'une eq dont toutes les tailles sont connues ne sont pas principales.
       lhs_gen_or_degree n'est pas appele', elles seront generees ds la foulee */
    /* Chaque occurrence d'une variable principale compte pour m_weight */
    /* Chaque occurrence d'une super-variable compte pour sv_weight */
    /* Il faut avoir : (. = main, _ = super_var, 0 = main vide)

       . . < . _ < _ _ < . . . < . _ . < _ . _ < . . . .
       200   201  202    300     301     302     400

       . ., . . < . ., _ . < . _, _ . < . ., . . .
       400        401        402        500
    */

    cover_size = 0;
    new_var_nb = max_gvar;
    sxba_empty (cover_set);
    degree = 0;

    if (is_gen)
	is_first = true;
    else
	sxba_empty (eq_len_set);

    for (arg = 0; arg < arity; arg++) {
	if (is_gen && arg > 0 && !to_be_memorized)
	    fputs (", ", out_file);

	pos = p [arg].bot;
	top = p [arg].top;

	while (pos < top) {
	    bot = pos;
	    lpos = 0;
	    sxinitialise (rpos); /* pour faire taire gcc -Wuninitialized */

	    while (pos < top) {
		/* On cherche la 1ere occur de best_eq_set */
		symb = pos2symb [pos];

		if (symb > 0) {
		    eq = var2eq [symb];

		    if (SXBA_bit_is_set (best_eq_set, eq))
			/* trouvee */
			break;

		    if (SXBA_bit_is_set (local_eq_set, eq)) {
			if (lpos == 0)
			    lpos = pos;

			rpos = pos;
		    }
		}

		pos++;
	    }

	    for (cur = bot; cur < pos; cur++) {
		symb = pos2symb [cur];

		if (symb > 0) {
		    eq = var2eq [symb];

		    if (cur == lpos) {
			if (lpos == rpos) {
			    /* Cette var "fait partie" d'une supervar */
			    /* On la sort et on la met ds cover */
			    if (SXBA_bit_is_reset_set (cover_set, symb)) {
				cover_size++;
				cover [symb].bot = cur;
				cover [symb].top = cur+1;

				if (!is_gen && (degree += sv_weight) >= min_degree)
				    /* variable seule ds une supervar => poids == 2 */
				    /* Le Mar  5 Dec 2000 09:59:27, le poids est une option de l'execution */
				    return MAX_DEGREE;
			    }

			    if (is_gen)
				gen_symb (symb, &is_first);
			}
			else {
			    /* supervariable */
			    /* verifier que cette supervar n'a pas deja ete generee */
			    if ((super_var_nb = get_super_var (lpos, rpos+1)) == 0) {
				super_var_nb = ++new_var_nb;
				SXBA_1_bit (cover_set, new_var_nb);
				cover_size++;
				cover [new_var_nb].bot = lpos;
				cover [new_var_nb].top = rpos+1;

				if (!is_gen && (degree += sv_weight) >= min_degree)
				    /* variables multiples ds une supervar => poids == 3 */
				    /* Le Mar  5 Dec 2000 09:59:27, le poids est une option de l'execution */
				    return MAX_DEGREE;
			    }

			    if (is_gen) {
				if (!to_be_memorized && !is_first)
				    fputs (" ", out_file);

				out_var_text (super_var_nb);
				is_first = false;
			    }

			    cur = rpos;
			}
		    }
		    else {
			if (is_gen)
			    gen_symb (symb, &is_first);
			else {
			    eq_len = var2eq_len [symb];

			    if (!SXBA_bit_is_set (known_lgth_var_set, symb) &&
				SXBA_bit_is_reset_set (eq_len_set, eq_len)) {
				/* var secondaire de taille inconnue non encore generee */
				if ((degree += m_weight) >= min_degree)
				    return MAX_DEGREE;
			    }
			}
		    }
		}
		else {
		    if (is_gen)
			gen_symb (symb, &is_first);
		}
	    }

	    if (pos < top) {
		/* pos designe une best_eq, on la sort */
		if (!block_extraction) {
		    /* Extraction de best_eq 1 par 1 */
		    if (is_gen)
			gen_arg (pos, pos+1, &is_first);
		    else {
			symb = pos2symb [pos];
			eq_len = var2eq_len [symb];

			if (!SXBA_bit_is_set (known_lgth_var_set, symb) &&
			    SXBA_bit_is_reset_set (eq_len_set, eq_len)) {
			    /* var principale de longueur inconnue et aucune var de la meme classe d'equivalence et de meme
			       longueur n'a ete trouvee, on la decompte */
			    if ((degree += m_weight) >= min_degree)
				return MAX_DEGREE;
			}
		    }

		    pos++;
		}
		else {
		    lpos = rpos = pos;
		    cur = pos+1;

		    while (cur < top) {
			symb = pos2symb [cur];

			if (symb > 0) {
			    eq = var2eq [symb];

			    if (!SXBA_bit_is_set (best_eq_set, eq))
				/* Fin du bloc des best_eq */
				break;
				
			    rpos = cur;
			}

			cur++;
		    }

		    if (lpos == rpos) {
			if (is_gen)
			    gen_arg (pos, pos+1, &is_first);
			else {
			    symb = pos2symb [pos];
			    eq_len = var2eq_len [symb];

			    if (!SXBA_bit_is_set (known_lgth_var_set, symb) &&
				SXBA_bit_is_reset_set (eq_len_set, eq_len)) {
				/* var principale de longueur inconnue et aucune var de la meme classe d'equivalence et de meme
				   longueur n'a ete trouvee, on la decompte */
				if ((degree += m_weight) >= min_degree)
				    return MAX_DEGREE;
			    }
			}

			pos++;
		    }
		    else {
			/* supervariable principale*/
			/* verifier que cette supervar n'a pas deja ete generee */
			if ((super_var_nb = get_super_var (lpos, rpos+1)) == 0) {
			    super_var_nb = ++new_var_nb;
			    SXBA_1_bit (cover_set, new_var_nb);
			    cover_size++;
			    cover [new_var_nb].bot = lpos;
			    cover [new_var_nb].top = rpos+1;

			    if (!is_gen && (degree += sv_weight) >= min_degree)
				/* variables multiples ds une supervar => poids == 3 */
				/* Le Mar  5 Dec 2000 09:59:27, le poids est une option de l'execution */
				return MAX_DEGREE;
			}

			if (is_gen) {
			    if (!to_be_memorized && !is_first)
				fputs (" ", out_file);

			    out_var_text (super_var_nb);
			    is_first = false;
			}

			pos = rpos+1;
		    }
		}
	    }
	}

	if (is_gen && to_be_memorized) {
	    add_item (EOA);
	}
    }

    if (is_gen && to_be_memorized)
	close_new_prdct();

    return degree;
}


static void
fill_local_eq_set (struct arg2bot_top *p, SXINT arity)
{
    SXINT	arg, pos, top, lpos, rpos, symb, cur_eq, eq_len, cur;

    sxinitialise(eq_len) /* Pour un compilateur crois� pour l'architecture itanium64 qui est gcc version 3.4.5
			    et qui dit "warning: 'eq_len' might be used uninitialized in this function" */;

    sxba_empty (local_eq_set);
    sxba_empty (eq_len_set);

    /* On remplit d'abord local_eq_set avec les eq des var qui seront des composants des supervar */
    /* On tient compte des eq_len */
    for (arg = 0; arg < arity; arg++) {
	pos = p [arg].bot;
	top = p [arg].top;

	do {
	    sxinitialise (rpos); /* pour faire taire gcc -Wuninitialized */
	    lpos = 0;

	    while (pos < top) {
		/* On cherche la 1ere occur ds best_eq_set */
		symb = pos2symb [pos];

		if (symb > 0) {
		    cur_eq = var2eq [symb];

		    if (SXBA_bit_is_set (best_eq_set, cur_eq))
			break;

		    if (!SXBA_bit_is_set (known_lgth_var_set, symb)) {
			/* var de longueur inconnue */
			if (lpos == 0)
			    lpos = pos;

			rpos = pos;
		    }
		}

		pos++;
	    }

	    if (lpos > 0) {
		if (lpos == rpos) {
		    /* Une seule variable de taille inconnue */
		    /* Ca sera une supervar ssi elle a des var equivalentes qui sont membres d'une super var */
		    symb = pos2symb [lpos];
		    eq_len = var2eq_len [symb];
		    SXBA_1_bit (eq_len_set, eq_len);
		}
		else {
		    /* precede d'une super variable */
		    /* On tient compte des eq_len des prefixes et suffixes */
		    for (cur = lpos; cur <= rpos; cur++) {
			symb = pos2symb [cur];

			if (symb > 0) {
			    eq_len = var2eq_len [symb];

			    if (!SXBA_bit_is_set (eq_len_set, eq_len)) {
				lpos = cur;
				break;
			    }
			}
		    }

		    if (cur <= rpos) {
			for (cur = rpos; lpos <= cur; cur--) {
			    symb = pos2symb [cur];

			    if (symb > 0) {
				eq_len = var2eq_len [symb];

				if (!SXBA_bit_is_set (eq_len_set, eq_len)) {
				    rpos = cur;
				    break;
				}
			    }
			}

			if (lpos == rpos) {
			    /* Une seule variable de taille inconnue */
			    /* Aucune variable equivalente de meme taille generee */
			    SXBA_1_bit (eq_len_set, eq_len);
			}
			else {
			    for (cur = lpos; cur <= rpos; cur++) {
				symb = pos2symb [cur];

				if (symb > 0) {
				    cur_eq = var2eq [symb];
				    SXBA_1_bit (local_eq_set, cur_eq);
				}
			    }
			}
		    }
		}

	    }
	} while (++pos < top);
    }
}

static void
clause_in_2var_form (SXINT clause)
{
    /* On genere la A_clause clause, ses caracteristiques sont ds slice2disp [0] */
    SXINT 		cur_slice, slice, arity, eq, degree, arg, bot, top;
    SXINT			symb, cur_eq, best_eq, pos, lpos, rhs, cur;
    SXINT			rhs_prdct, rhs_bot2, rhs_nt, rhs_top2, rhs_param, rhs_cur2, rhs_bot3, x, X, Y, rhs_pos; 
    struct arg2bot_top	*p;
    bool		is_main, OK, done, is_first, before, after;

    for (cur_slice = 0, slice = 1; cur_slice < slice; cur_slice++) {
	bot = slice2disp [cur_slice];
	p = arg2bot_top + bot;
	arity = slice2disp [cur_slice+1]-bot;

	/* On remplit eq_set */
	sxba_empty (eq_set);

	for (arg = 0; arg < arity; arg++) {
	    for (top = p [arg].top, pos = p [arg].bot; pos < top; pos++) {
		if ((symb = pos2symb [pos]) > 0) {
		    eq = var2eq [symb];

		    if (SXBA_bit_is_reset_set (eq_set, eq))
			eq2nb [eq] = 0;

		    if (!SXBA_bit_is_set (known_lgth_var_set, symb))
			eq2nb [eq]++;
		}
	    }
	}

	is_main = (cur_slice == 0);

	if (!is_main) {
	    if (to_be_memorized) {
		add_item (0); /* call kind */
		add_item (slice2new_nt [cur_slice]);
	    }
	    else {
		fprintf (out_file, "clause_%ld_%ld", (SXINT) clause, (SXINT) slice2new_clause [cur_slice]);
	    }
	}
	
	if (!to_be_memorized) fputs ("(", out_file);

	/* Pour chaque eq, on compte le nombre de var engendre par son extraction */
	/* A FAIRE : Regarder le nb de variables necessaire a la representation d'une partition
	   P1 U P2 avec 2<= |P1| <= |P2| */

	best_eq = 0;
	min_degree = MAX_DEGREE;
	sxba_empty (best_eq_set);
	sxba_empty (previous_best_eq_set);

	eq = 0;
	
	while ((eq = sxba_scan (eq_set, eq)) > 0) {
	    /* On traite eq */
	    if (eq2nb [eq]) {
		/* Si toutes ses var ne sont pas connues */
		SXBA_1_bit (best_eq_set, eq);
		block_extraction = false;
		best_eq = eq;
		fill_local_eq_set (p, arity);

		/* On calcule le degree pour eq */
		degree = lhs_gen_or_degree (p, arity, false/* , is_main */);

		if (degree < min_degree) {
		    min_degree = degree;
		    sxba_copy (previous_best_eq_set, best_eq_set);
		    previous_block_extraction = false;
		}

		/* On regarde s'il n'est pas plus avantageux d'extraire best_eq par supervar */
		before = after = false;

		for (arg = 0; arg < arity; arg++) {
		    pos = p [arg].bot;
		    top = p [arg].top;
		    lpos = 0;

		    while (pos < top) {
			/* On cherche la 1ere occur de best_eq */
			symb = pos2symb [pos];

			if (symb > 0) {
			    cur_eq = var2eq [symb];

			    if (cur_eq == best_eq) {
				lpos = pos;
				pos = top;

				while (lpos < --pos) {
				    symb = pos2symb [pos];

				    if (symb > 0) {
					cur_eq = var2eq [symb];

					if (cur_eq == best_eq)
					    break;

					after = true;
				    }
				}

				while (lpos < --pos) {
				    symb = pos2symb [pos];

				    if (symb > 0) {
					cur_eq = var2eq [symb];

					SXBA_1_bit (best_eq_set, cur_eq);
				    }
				}

				break;
			    }

			    before = true;
			}

			pos++;
		    }
		}

		if (before || after) {
		    /* Le Jeu  7 Jun 2001 11:47:02 */
		    /* Au moins un bloc ds un arg va etre precede ou suivi d'une supervar */
		    /* On remplit d'abord local_eq_set avec les eq des var qui seront des composants des supervar */
		    block_extraction = true;
		    fill_local_eq_set (p, arity);

		    /* On calcule le degree pour best_eq_set */
		    degree = lhs_gen_or_degree (p, arity, false/* , is_main */);

		}
		else
		    degree = MAX_DEGREE;

		if (degree < min_degree) {
		    min_degree = degree;
		    sxba_copy (previous_best_eq_set, best_eq_set);
		    previous_block_extraction = true;
		}

		sxba_empty (best_eq_set);
	    }
	}

	/* On regarde s'il n'est pas plus avantageux d'extraire best_eq par supervar */
	/* A FAIRE On pourrait tenter des supervar partielles qui ne couvrent pas
	   toutes les occur de best_eq ds un arg (il faut au moins 3 occur de best_eq ds cet arg) */
	/* Pour ca, un arg doit contenir au moins 2 occur de best_eq */
	if (sxba_is_empty (eq_set))
	    SXBA_1_bit (previous_best_eq_set, 0); /* garde-fou */

	sxba_copy (best_eq_set, previous_best_eq_set);
	block_extraction = previous_block_extraction;
	/* On extrait best_eq_set */
	/* Les sequences de variables multiples sont regroupees dans des supervariables */

	fill_local_eq_set (p, arity);

	lhs_gen_or_degree (p, arity, true/* , is_main */);

	if (!to_be_memorized)
	    fputs (") --> ", out_file);

	/* les clauses generees doivent contenir explicitement les &StrLen correspondant aux Ak2len
	   et les &StrEqLen correspondant aux IeqJ car leurs calculs sur la forme de la grammaire
	   peuvent ne pas donner les memes reultats (prudence) */
        /* On sort la RHS dont les var sont ds var_set */
	rhs = XxY_Y (rcg_clauses, clause);

	if (rhs > 0) {
	    top = XH_X (rcg_rhs, rhs+1);
	    cur = XH_X (rcg_rhs, rhs);
	    rhs_pos = 1;

	    while (cur < top) {
	        sxinitialise (symb); /* pour faire taire gcc -Wuninitialized */
		rhs_prdct = XH_list_elem (rcg_rhs, cur);
		rhs_bot2 = XH_X (rcg_predicates, rhs_prdct);
		rhs_nt = XH_list_elem (rcg_predicates, rhs_bot2+1);
		rhs_top2 = XH_X (rcg_predicates, rhs_prdct+1);

		if (rhs_nt > 0) {
		    for (rhs_cur2 = rhs_bot2+2; rhs_cur2 < rhs_top2; rhs_cur2++) {
			rhs_param = XH_list_elem (rcg_predicates, rhs_cur2);
			rhs_bot3 = XH_X (rcg_parameters, rhs_param);
			symb = XH_list_elem (rcg_parameters, rhs_bot3);

			if (!SXBA_bit_is_set (var_set, symb)) {
			    symb = 0;
			    break;
			}
		    }

		    if (symb > 0)
		      new_clause2rhs_list [new_clause2rhs_list_top++] = rhs_pos;

		    rhs_pos++;
		}
		else {
		    if (rhs_nt == STRLEN_ic || rhs_nt == STREQLEN_ic || rhs_nt == STREQ_ic ||
			rhs_nt == FIRST_ic || rhs_nt == LAST_ic) {
			rhs_param = XH_list_elem (rcg_predicates, rhs_bot2+3);
			symb = XH_list_elem (rcg_parameters, XH_X (rcg_parameters, rhs_param)); /* >0 */

			if (SXBA_bit_is_set (var_set, symb)) {
			    if (rhs_nt == STREQLEN_ic || rhs_nt == STREQ_ic) {
				rhs_param = XH_list_elem (rcg_predicates, rhs_bot2+2);
				symb = XH_list_elem (rcg_parameters, XH_X (rcg_parameters, rhs_param)); /* >0 */

				if (!SXBA_bit_is_set (var_set, symb)) {
				    symb = 0;
				}
				else {
				    if (rhs_nt == STREQLEN_ic) {
					if (SXBA_bit_is_set_reset (known_lgth_var_set, symb))
					    /* Les 2 longueurs sont connues, on ne genere rien */
					    symb = 0;
				    }
				}
			    }
			    else {
				if (rhs_nt == STRLEN_ic && !SXBA_bit_is_set_reset (known_lgth_var_set, symb))
				    symb = 0;
			    }
			}
			else
			    symb = 0;
		    }
		    else
			symb = 0;
		}

		if (symb > 0) {
		    out_prdct_text (rhs_prdct);

		    if (!to_be_memorized) {
			fputs (" ", out_file);
		    }
		}

		cur++;
	    }

	    /* On genere tous les &StrLen (non deja generes) des var de la LHS dont la longueur est connue */
	    if (has_known_lgth_var) {
		sxba_and (var_set, known_lgth_var_set);
		symb = 0;

		while ((symb = sxba_scan_reset (var_set, symb)) > 0) {
		    if (to_be_memorized)
			add_item (0);

		    out_nt_text (STRLEN_ic);

		    if (to_be_memorized) {
			add_item (var2lgth [symb]);
			add_item (EOA);
		    }
		    else
			fprintf (out_file, "(%ld, ", (SXINT) var2lgth [symb]);

		    out_var_text (symb);

		    if (to_be_memorized) {
			add_item (EOA);
			close_new_prdct();
		    }
		    else
			fputs (") ", out_file);
		}
	    }

	    /* On genere les &STREQLEN des IeqJ */
	    if ((top = equal_pair_stack [0]) > 0) {
		do {
		    if ((X = equal_pair_stack [top-1]) < 0) {
			X = -X;

			if (SXBA_bit_is_set (var_set, X)) {
			    Y = equal_pair_stack [top]; /* aussi ds var_set */

			    if (to_be_memorized)
				add_item (0);

			    out_nt_text (STREQLEN_ic);

			    if (!to_be_memorized)
				fputs ("(", out_file);

			    out_var_text (X);

			    if (to_be_memorized)
				add_item (EOA);
			    else
				fputs (", ", out_file);

			    out_var_text (Y);

			    if (to_be_memorized) {
				add_item (EOA);
				close_new_prdct();
			    }
			    else
				fputs (") ", out_file);
			}
		    }
		} while ((top -= 2) > 0);
	    }
	}

	sxba_empty (var_set);

	/* Le but des d'un &Lex(t) ds une A-clause p est de permettre, a l'analyse, si le source
	   contient le symbole t, la preselection de la clause p.  Cependant, cette preselection
	   n'a pas d'effet si l'ensemble des A-clauses est de cardinalite 1 (il n'y a pas de switch genere).
	   Il est donc inutile de generer des &Lex sur les clauses non-principales qui sont des singletons.
	   Ca peut meme etre nuisible car le partage des clauses identiques (option -factorize) peut en
	   etre deteriore. */
	/* Cependant, des couvertures ulterieures (en particulier 1fsa) peut tirer parti de ces lex singletons
	   Ex A --> ... B ... , il y a plusieurs a-clauses et le B call peut etre inlined.  Si B a du lex, il
	   peut etre remonte' au niveau de A et servir a choisir entre les A_clauses */
	/* Le 22/10/01, la generation des lex est gouvernee par l'option -is_full_lex */
	if ((is_main || is_full_lex) && is_t_set) {
	    is_first = true;
	    symb = 0;

	    while ((symb = sxba_scan (t_set, symb)) > 0) {
		if (is_full_lex || !SXBA_bit_is_set (local_t_set, symb)) {
		    /* On genere du &Lex */
		    if (is_first) {
			is_first = false;

			if (to_be_memorized)
			    add_item (0);

			out_nt_text (LEX_ic);
		
			if (!to_be_memorized)
			    fputs ("(", out_file);
		    }
		    else {
			if (!to_be_memorized)
			    fputs (" ", out_file);
		    }

		    out_t_text (symb);
		}
	    }

	    if (!is_first) {
		if (to_be_memorized) {
		    add_item (EOA);
		    close_new_prdct();
		}
		else
		    fputs (") ", out_file);
	    }
	}

	/* Maintenant on s'occupe des supervariables */
	/* A une supervariable correspond un arg */
	/* 2 variables equivalentes sont ds le meme appel */

	if (new_var_nb > max_gvar) {
	    arg2bot_top_top += cover_size;

	    if (arg2bot_top_top > arg2bot_top_size) {
		while (arg2bot_top_top >= (arg2bot_top_size *= 2));

		arg2bot_top = (struct arg2bot_top*) sxrealloc (arg2bot_top, arg2bot_top_size+1, sizeof (struct arg2bot_top));
	    }

	    p = arg2bot_top + arg2bot_top_top - (cover_size);
	    
	    while (sxba_scan (cover_set, 0) > 0) {
		done = false;
		new_clause_nb++;
		from2var2clause_list [++from2var2clause_list_top] = clause;
		from2var2clause_disp [new_clause_nb+1] = from2var2clause_list_top+1;
		new_nts_top++;

		if (to_be_memorized) {
		    if (new_nts_top >= new_nts_size)
			new_nts = (struct new_nts*) sxrealloc (new_nts, (new_nts_size *= 2)+1, sizeof (struct new_nts));

		    new_nts [new_nts_top].old_clause = clause;
		    new_nts [new_nts_top].new_clause = new_clause_nb;

		    add_item (0);
		    add_item (max_nt+new_nts_top);
		    slice2new_nt [slice] = max_nt+new_nts_top;
		}
		else {
		    fprintf (out_file, "clause_%ld_%ld(", (SXINT) clause, (SXINT) new_clause_nb);
		}

		new_clause2rhs_list [new_clause2rhs_list_top++] = -new_clause_nb;

		slice2new_clause [slice] = new_clause_nb;
		arity = 0;
		sxba_empty (local_eq_set);

		while (!done) {
		    done = true;
		    x = 0;

		    while ((x = sxba_scan (cover_set, x)) > 0) {
			bot = cover [x].bot;
			top = cover [x].top;
			OK = false;

			if (arity == 0) {
			    OK = true;
			}
			else {
			    for (cur = bot; cur < top; cur++) {
				symb = pos2symb [cur];

				if (symb > 0) {
				    eq = var2eq [symb];

				    if (SXBA_bit_is_set (local_eq_set, eq)) {
					OK = true;
					break;
				    }
				}
			    }
			}

			if (OK) {
			    done = false;

			    for (cur = bot; cur < top; cur++) {
				symb = pos2symb [cur];

				if (symb > 0) {
				    eq = var2eq [symb];
				    SXBA_1_bit (local_eq_set, eq);
				}
			    }

			    p [arity] = cover [x];
			    SXBA_0_bit (cover_set, x);

			    if (arity != 0) {
				if (to_be_memorized) {
				    add_item (EOA);
				}
				else
				    fputs (", ", out_file);
			    }
			    
			    out_var_text (x);
			    arity++;
			}
		    }
		}

		if (to_be_memorized) {
		    add_item (EOA);
		    close_new_prdct();
		}
		else
		    fputs (") ", out_file);

		slice++;
		slice2disp [slice] = slice2disp [slice-1] + arity;
		p += arity;
	    }
	}
	else {
	    if (cover_size)
		sxba_empty (cover_set);
	}
    
	if (to_be_memorized)
	    close_new_clause (slice2new_clause [cur_slice]);
	else {
	    fprintf (out_file, ". %% %ld\n", (SXINT) slice2new_clause [cur_slice]);
	}

	new_clause2rhs_disp [slice2new_clause [cur_slice]+1] = new_clause2rhs_list_top;

	if (is_local_t_set) {
	    is_local_t_set = false;
	    sxba_empty (local_t_set);
	}
    }

#if EBUG
    if (!sxba_is_empty (cover_set))
	sxtrap (ME, "clause_in_2var_form");
#endif
}



static void
put_in_var2eq (void)
{
    /* var_stack contient une liste de variables qui sont equivalentes */
    /* On remplit var2eq */
    SXINT top, eq, bot, X, Y;

    top = var_stack [0];
    eq = max_gvar;

    for (bot = 1; bot <= top; bot++) {
	X = var_stack [bot];
	Y = var2eq [X];

	if (Y < eq)
	    eq = Y;
    }

    for (bot = 1; bot <= top; bot++) {
	X = var_stack [bot];
	Y = var2eq [X];
	var2eq [X] = eq;

	if (Y > eq) {
	    /* Toutes les var equivalentes a Y doivent etre changees en eq */
	    for (X = 1; X <= max_var; X++) {
		if (var2eq [X] == Y)
		    var2eq [X] = eq;
	    }
	}
    }

    var_stack [0] = 0;
}


static void
put_in_var2eq_len (void)
{
    /* equal_pair_stack contient des couples de variables de taille egale */
    /* Pour chaque var on met de var2eq_len la var representante de la classe d'equivalence des tailles egales */
    SXINT top, X, Y, Xeq_len, Yeq_len, eq_len, old_eq_len, var;

    top = equal_pair_stack [0];

    while (top > 0) {
	Y = equal_pair_stack [top--];
	X = equal_pair_stack [top--];

	if (X < 0)
	    /* Couple provenant d'un IeqJ */
	    X = -X;

	Xeq_len = var2eq_len [X];
	Yeq_len = var2eq_len [Y];

	if (Xeq_len < Yeq_len) {
	    eq_len = Xeq_len;
	    old_eq_len = Yeq_len;
	}
	else {
	    eq_len = Yeq_len;
	    old_eq_len = Xeq_len;
	}

	for (var = 1; var <= max_var; var++) {
	    if (var2eq_len [var] == old_eq_len)
		var2eq_len [var] = eq_len;
	}
    }
}

	
static void
process_equal_pair_stack (void)
{
    /* equal_pair_stack contient des couples de variables de taille egale */
    /* Ca permet de completer known_lgth_var_set */
    SXINT		top, X, Y;
    bool 	done;


    done = false;

    while (!done) {
	done = true;

	top = equal_pair_stack [0];

	while (top > 0) {
	    Y = equal_pair_stack [top--];
	    X = equal_pair_stack [top--];

	    if (X < 0)
		/* Couple provenant d'un IeqJ */
		X = -X;

	    if (SXBA_bit_is_set (known_lgth_var_set, X) && SXBA_bit_is_reset_set (known_lgth_var_set, Y)) {
		has_known_lgth_var = true;
		var2lgth [Y] = var2lgth [X];
		done = false;
	    }

	    if (SXBA_bit_is_set (known_lgth_var_set, Y) && SXBA_bit_is_reset_set (known_lgth_var_set, X)) {
		has_known_lgth_var = true;
		var2lgth [X] = var2lgth [Y];
		done = false;
	    }
	}
    }
}

static void
fill_rhs_map (SXINT new_clause)
{
    SXINT cur, top, rhs;

    for (top = new_clause2rhs_disp [new_clause+1], cur = new_clause2rhs_disp [new_clause];
	 cur < top;
	 cur++) {
	rhs = new_clause2rhs_list [cur];

	if (rhs > 0)
	    rhs_stack_map [rhs_stack_map_top++] = rhs;
	else
	    fill_rhs_map (-rhs);
    }
}


static void process_clause_equiv (void);
static void print_new_clauses (void);
void
two_var_form (void)
{
    SXINT		clause, lhs_prdct, lhs_bot2, lhs_nt, lhs_param, symb, symb1, symb2;
    SXINT		rhs, rhs_prdct, rhs_bot2, rhs_nt, rhs_param, Ak, rhs_bot3, rhs_top3, rhs_cur3;
    SXINT		lhs_top2, lhs_arity, lhs_pos, lhs_arg, lhs_cur2, lhs_bot3, lhs_top3, lhs_cur3, rhs_top, rhs_cur;
    SXINT		rhs_top2, k, rhs_cur2, lgth, high, low, I, J, IJ, top, size;
    SXINT		new_clause;
    bool	is_pos_call;

    if (max_gvar <= 2) {
	if (sxverbosep) {
	    fputs ("This grammar is already in 2-var form.\n", sxtty);
	    sxttycol1p = true;
	}

	/* Il faudrait faire qqchose ds gen_parser pour accepter ici "return;" */
	sxexit (3);
    }

    if (sxverbosep) {
	fputs ("\t2var_form .... ", sxtty);
	sxttycol1p = false;
    }

    if (is_full_lex)
      sprintf (lst_name, "%s_%s.fl_2var", prgentname, suffixname);
    else
      sprintf (lst_name, "%s_%s.2var", prgentname, suffixname);

    if ((out_file = fopen (lst_name, "w")) == NULL) {
	fprintf (sxstderr, "rcg -2var_form: cannot open (create) ");
	sxperror (lst_name);
	return;
    }

    out_header (rcg_header, lst_name, "in 2var form");
    
    /* Dans tout les cas, on genere en premier une false clause qui assure l'egalite des codes des
       terminaux et des nt communs entre les 2 grammaires. */
    /* Cette clause vient remplacer la premiere, si cette derniere est une false */
    if (SXBA_bit_is_set (false_clause_set, 1)) {
	init_clause_nb = 2;
	fputs ("\n%% 1: ", out_file);
	out_clause ((SXINT)1);
    }
    else
	init_clause_nb = 1;

    gen_false_clause (sxstrget (nt2ste [1]), "");
    new_axiom = 1;

    new_clause_nb = new_axiom;

    t_set = sxba_calloc (max_t+1);
    local_t_set = sxba_calloc (max_t+1);
    var_set = sxba_calloc (max_gvar+1);
    known_lgth_var_set = sxba_calloc (max_gvar+1);

    pos2symb = (SXINT*) sxalloc (1+max_lhs_adr, sizeof (SXINT));
    var2eq = (SXINT*) sxalloc (1+max_gvar, sizeof (SXINT));
    var2eq_len = (SXINT*) sxalloc (1+max_gvar, sizeof (SXINT));
    var2lgth = (SXINT*) sxalloc (1+max_gvar, sizeof (SXINT));
    var_stack = (SXINT*) sxalloc (1+max_gvar, sizeof (SXINT));
    var_stack [0] = 0;
    arg2bot_top = (struct arg2bot_top *) sxalloc ((arg2bot_top_size = max_gvar)+1, sizeof (struct arg2bot_top));
    cover = (struct arg2bot_top *) sxalloc (2*max_gvar+1, sizeof (struct arg2bot_top));
    cover_set = sxba_calloc (2*max_gvar+1);
    eq_set = sxba_calloc (max_gvar+1);
    eq2nb = (SXINT*) sxalloc (max_gvar+1, sizeof (SXINT));
    eq_len_set = sxba_calloc (max_gvar+1);
    local_eq_set = sxba_calloc (max_gvar+1);
    best_eq_set = sxba_calloc (max_gvar+1);
    previous_best_eq_set = sxba_calloc (max_gvar+1);
    close_eq_set = sxba_calloc (max_gvar+1);
    slice2disp = (SXINT*) sxalloc (max_gvar+1, sizeof (SXINT));
    slice2new_clause = (SXINT*) sxalloc (max_gvar+1, sizeof (SXINT));
    equal_pair_stack = (SXINT*) sxalloc (2*max_gvar+1, sizeof (SXINT));
    equal_pair_stack [0] = 0;
    streqlen_pair_stack = (SXINT*) sxalloc (2*max_gvar+1, sizeof (SXINT));
    streqlen_pair_stack [0] = 0;

    clause2clause2var_form = (SXINT*) sxcalloc (last_clause+1, sizeof (SXINT));
    from2var2clause_disp = (SXINT*) sxalloc (last_clause*(max_rhs_prdct+1)+2, sizeof (SXINT));
    from2var2clause_list = (SXINT*) sxalloc (last_clause*(max_rhs_prdct+1)+2, sizeof (SXINT));
    from2var2clause_disp [0] = from2var2clause_list [0] = 0;
    from2var2clause_disp [new_clause_nb+1] = from2var2clause_disp [new_clause_nb] = from2var2clause_list_top+1;
    
    clause2new_clause = (SXINT*) sxcalloc (last_clause+1, sizeof (SXINT));
    new_clause2rhs_disp  = (SXINT*) sxalloc (last_clause*(max_rhs_prdct+1)+2, sizeof (SXINT));
    size = last_clause*(max_rhs_prdct+max_clause_arg_nb*max_arg_size);
    new_clause2rhs_list = (SXINT*) sxalloc (size+1, sizeof (SXINT));
    new_clause2rhs_disp [new_clause_nb+1] = new_clause2rhs_list_top = 1;

    if (is_factorize) {
	clause2disp = (SXINT*) sxalloc (last_clause+2, sizeof (SXINT));
	new_clause2disp = (SXINT*) sxalloc (last_clause*(max_rhs_prdct+1)+2, sizeof (SXINT));
	size = last_clause*(3/*call_kind+nt+EOP*/*(max_rhs_prdct+1/*lhs_prdct*/)+
			    max_clause_arg_nb*(max_arg_size+1/*EOA*/)
			    +1/*EOC*/);
	new_clauses_body = (SXINT*) sxalloc (size+1, sizeof (SXINT));
	new_nts = (struct new_nts*) sxalloc ((new_nts_size = last_clause)+1, sizeof (struct new_nts));
	slice2new_nt = (SXINT*) sxalloc (max_gvar+1, sizeof (SXINT));

	clause2disp [1] = new_clause_nb+1;
	new_clause2disp [new_clause_nb+1] = 1;

	to_be_memorized = true;
    }

    MAX_DEGREE = sv_weight*(max_gvar+1);

    for (clause = 1; clause <= last_clause; clause++) {
	if (clause2identical [clause] == clause && clause >= init_clause_nb) {
	    lhs_prdct = XxY_X (rcg_clauses, clause) & lhs_prdct_filter;
	    lhs_bot2 = XH_X (rcg_predicates, lhs_prdct);
	    lhs_top2 = XH_X (rcg_predicates, lhs_prdct+1);
	    lhs_nt = XH_list_elem (rcg_predicates, lhs_bot2+1);
	    lhs_arity = nt2arity [lhs_nt];
	    lhs_pos = 0;
	    pos2symb [lhs_pos] = 0;
	    max_var = 0;

	    for (lhs_arg = 0, lhs_cur2 = lhs_bot2+2; lhs_cur2 < lhs_top2; lhs_arg++, lhs_cur2++) {
		lhs_param = XH_list_elem (rcg_predicates, lhs_cur2);
		lhs_pos++;
		arg2bot_top [lhs_arg].bot = lhs_pos;

		if (lhs_param > 0) {
		    lhs_bot3 = XH_X (rcg_parameters, lhs_param);
		    lhs_top3 = XH_X (rcg_parameters, lhs_param+1);

		    for (lhs_cur3 = lhs_bot3; lhs_cur3 < lhs_top3; lhs_cur3++) {
			symb = XH_list_elem (rcg_parameters, lhs_cur3);
			pos2symb [lhs_pos] = symb;
			lhs_pos++;

			if (symb > 0) {
#if EBUG
			    var2lgth [symb] = -1; /* Pour xdbg */
#endif
			
			    var2eq_len [symb] = symb;
			    var2eq [symb] = symb;

			    if (symb > max_var)
				max_var = symb; 
			}
			else {
			    symb = -symb;
			    SXBA_1_bit (t_set, symb);
			    is_t_set = true;
			}
		    }
		}

		pos2symb [lhs_pos] = 0;
		arg2bot_top [lhs_arg].top = lhs_pos;
	    }

	    is_combinatorial = false;

	    /* On va chercher en RHS toutes les variables dont on connait la longueur : il n'est pas necessaire
	       de les decompter */
	    rhs = XxY_Y (rcg_clauses, clause);
	    rhs_top = XH_X (rcg_rhs, rhs+1);
	    rhs_cur = XH_X (rcg_rhs, rhs);
	    has_known_lgth_var = false;

	    while (!is_combinatorial && (rhs_cur < rhs_top)) {
		rhs_prdct = XH_list_elem (rcg_rhs, rhs_cur);
		rhs_bot2 = XH_X (rcg_predicates, rhs_prdct);
		rhs_top2 = XH_X (rcg_predicates, rhs_prdct+1);
		rhs_nt = XH_list_elem (rcg_predicates, rhs_bot2+1);

		if (rhs_nt > 0) {
		    for (k = 0, rhs_cur2 = rhs_bot2+2; rhs_cur2 < rhs_top2; k++, rhs_cur2++) {
			rhs_param = XH_list_elem (rcg_predicates, rhs_cur2);
			rhs_bot3 = XH_X (rcg_parameters, rhs_param);
			rhs_top3 = XH_X (rcg_parameters, rhs_param+1);

			if (rhs_bot3 == rhs_top3-1) {
			    symb = XH_list_elem (rcg_parameters, rhs_bot3);
			    PUSH (var_stack, symb);
			    Ak = A_k2Ak (rhs_nt, k);

			    if ((lgth = Ak2len [Ak]) != -1 && SXBA_bit_is_reset_set (known_lgth_var_set, symb)) {
				has_known_lgth_var = true;
				var2lgth [symb] = lgth;
			    }
			}
			else
			    is_combinatorial = true;
		    }

		    if (k > 1 && !is_combinatorial) {
			high = nt2IeqJ_disp [rhs_nt+1];
			low = nt2IeqJ_disp [rhs_nt];

			if (low < high) {
			    do {
				IJ = nt2IeqJ [low];
				I = IJ / max_garity; /* I < J */
				symb = XH_list_elem (rcg_parameters,
						     XH_X (rcg_parameters, XH_list_elem (rcg_predicates,
											 rhs_bot2+2+I)));
				PUSH (equal_pair_stack, -symb); /* 1er elem d'un IeqJ */
				J = IJ % max_garity;
				symb = XH_list_elem (rcg_parameters,
						     XH_X (rcg_parameters, XH_list_elem (rcg_predicates,
											 rhs_bot2+2+J)));
				PUSH (equal_pair_stack, symb);
			    } while (++low < high);
			}
		    }
		}
		else {
		    if (rhs_bot2+4 == rhs_top2) {
			/* predicat predefini a 2 args, le 2eme est un range */
			is_pos_call = XH_list_elem (rcg_predicates, rhs_bot2) == 0; /* 1 => neg call */
			rhs_param = XH_list_elem (rcg_predicates, rhs_bot2+3);
			rhs_bot3 = XH_X (rcg_parameters, rhs_param);
			rhs_top3 = XH_X (rcg_parameters, rhs_param+1);

			if (rhs_bot3 == rhs_top3-1) {
			    symb = XH_list_elem (rcg_parameters, rhs_bot3); /* >0 */

			    if (rhs_nt == STREQLEN_ic)
				PUSH (streqlen_pair_stack, (is_pos_call ? symb : -symb));
			    /* Ds le cas STREQLEN_ic, on differe la memorisation de l'equivalence des 2 args
			       car si les 2 longueurs sont connues statiquement, aucune verif dynam n'est
			       necessaire */
			    else
				PUSH (var_stack, symb);

			    if (is_pos_call) {
				if (rhs_nt == STRLEN_ic) {
				    if (SXBA_bit_is_reset_set (known_lgth_var_set, symb)) {
					has_known_lgth_var = true;
					var2lgth [symb] = XH_list_elem (rcg_parameters,
									XH_X (rcg_parameters,
									      XH_list_elem (rcg_predicates, rhs_bot2+2)));
				    }
				}
				else {
				    if (rhs_nt == STREQ_ic || rhs_nt == STREQLEN_ic) {
					PUSH (equal_pair_stack, symb);
				    }
				}
			    }
			}
			else
			    is_combinatorial = true;

			if (!is_combinatorial && (rhs_nt == STREQ_ic || rhs_nt == STREQLEN_ic)) {
			    rhs_param = XH_list_elem (rcg_predicates, rhs_bot2+2);
			    rhs_bot3 = XH_X (rcg_parameters, rhs_param);
			    rhs_top3 = XH_X (rcg_parameters, rhs_param+1);

			    if (rhs_bot3 == rhs_top3-1) {
				symb = XH_list_elem (rcg_parameters, rhs_bot3); /* >0 */

				if (rhs_nt == STREQLEN_ic)
				    PUSH (streqlen_pair_stack, symb);
				/* Ds le cas STREQLEN_ic, on differe la memorisation de l'equivalence des 2 args
				   car si les 2 longueurs sont connues statiquement, aucune verif dynam n'est
				   necessaire */
				else
				    PUSH (var_stack, symb);

				if (is_pos_call)
				    PUSH (equal_pair_stack, symb);
			    }
			    else
				is_combinatorial = true;
			}
		    }
		    else {
			if (rhs_nt == LEX_ic) {
			    /* On complete t_set */
			    /* A FAIRE : le completer aussi pour les &First, &Last et &StrEq spe'ciaux */
			    is_t_set = true;
			    rhs_param = XH_list_elem (rcg_predicates, rhs_bot2+2);
			    rhs_bot3 = XH_X (rcg_parameters, rhs_param);
			    rhs_top3 = XH_X (rcg_parameters, rhs_param+1);

			    for (rhs_cur3 = rhs_bot3; rhs_cur3 < rhs_top3; rhs_cur3++) {
				symb = -XH_list_elem (rcg_parameters, rhs_cur3);
				SXBA_1_bit (t_set, symb);
			    }
			}
			else {
			    if (rhs_nt == FALSE_ic)
				is_combinatorial = true; /* inutile de la passer a la moulinette */
			}
		    }
		}

		if (var_stack [0] > 0 && !is_combinatorial)
		    put_in_var2eq ();

		var_stack [0] = 0;

		rhs_cur++;
	    }

	    if (equal_pair_stack [0] > 0 && !is_combinatorial) {
		if (has_known_lgth_var)
		    process_equal_pair_stack ();

		put_in_var2eq_len ();
	    }

	    if (!is_combinatorial && (top = streqlen_pair_stack [0]) > 0) {
		do {
		    symb1 = streqlen_pair_stack [top];
		    symb2 = streqlen_pair_stack [top-1];

		    if (symb2 > 0)
			is_pos_call = true;
		    else {
			symb2 = -symb2;
			is_pos_call = false;
		    }

		    if (!SXBA_bit_is_set (known_lgth_var_set, symb1)) {
			PUSH (var_stack, symb1);
			PUSH (var_stack, symb2);
			put_in_var2eq ();
		    }
#if EBUG
		    else {
			if (!SXBA_bit_is_set (known_lgth_var_set, symb2) ||
			    (is_pos_call ? (var2lgth [symb1] != var2lgth [symb2]) : (var2lgth [symb1] == var2lgth [symb2])))
			    sxtrap (ME, "two_var_form");
		    }
#endif
		} while ((top -= 2) > 0);
	    }

	    streqlen_pair_stack [0] = 0;

	    if (!to_be_memorized) {
		fprintf (out_file, "\n%% %ld: ", (SXINT) clause);
		out_clause (clause);
	    }

	    new_clause_nb++;
	    from2var2clause_list [++from2var2clause_list_top] = clause;
	    from2var2clause_disp [new_clause_nb+1] = from2var2clause_list_top+1;

	    clause2new_clause [clause] = new_clause_nb;

	    if (!to_be_memorized) {
		clause2clause2var_form [clause] = new_clause_nb;
	    }

	    if (is_combinatorial) {
		/* Pour l'instant, on ne traite pas les combinatorial : A FAIRE */
		/* On ne fait rien */
		out_clause_nb (clause, new_clause_nb);
	    }
	    else {
		if (to_be_memorized)
		    add_item (0); /* pos call */

		out_nt_text (lhs_nt);
		slice2disp [0] = 0;
		slice2disp [1] = arg2bot_top_top = lhs_arity;
		slice2new_clause [0] = new_clause_nb;
		clause_in_2var_form (clause);
	    }

	    equal_pair_stack [0] = 0;

	    if (is_t_set) {
		is_t_set = false;
		sxba_empty (t_set);
	    }

	    if (has_known_lgth_var) {
		sxba_empty (known_lgth_var_set);
	    }
	}
	else {
	    if (clause == 1 && !to_be_memorized) {
		clause2clause2var_form [1] = new_clause_nb;
	    }
	}

	if (to_be_memorized) {
	    close_clause();
	}
    }

    sxfree (t_set), t_set = NULL;
    sxfree (local_t_set), local_t_set = NULL;
    sxfree (var_set), var_set = NULL;
    sxfree (known_lgth_var_set), known_lgth_var_set = NULL;

    sxfree (pos2symb), pos2symb = NULL;
    sxfree (var2eq), var2eq = NULL;
    sxfree (var2lgth), var2lgth = NULL;
    sxfree (var2eq_len), var2eq_len = NULL;
    sxfree (var_stack), var_stack = NULL;
    sxfree (arg2bot_top), arg2bot_top = NULL;
    sxfree (cover), cover = NULL;
    sxfree (cover_set), cover_set = NULL;
    sxfree (eq_set), eq_set = NULL;
    sxfree (eq2nb), eq2nb = NULL;
    sxfree (eq_len_set), eq_len_set = NULL;
    sxfree (local_eq_set), local_eq_set = NULL;
    sxfree (best_eq_set), best_eq_set = NULL;
    sxfree (previous_best_eq_set), previous_best_eq_set = NULL;
    sxfree (close_eq_set), close_eq_set = NULL;
    sxfree (slice2disp), slice2disp = NULL;
    sxfree (slice2new_clause), slice2new_clause = NULL;
    sxfree (equal_pair_stack), equal_pair_stack = NULL;
    sxfree (streqlen_pair_stack), streqlen_pair_stack = NULL;

    if (is_factorize) {
	sxfree (slice2new_nt), slice2new_nt = NULL;
    }
    
    if (sxverbosep) {
	fputs ("done\n", sxtty);
	sxttycol1p = true;
    }

    if (is_factorize) {
	if (sxverbosep) {
	    fputs ("\t\tfactorize .... ", sxtty);
	    sxttycol1p = false;
	}

	to_be_memorized = false;

	process_clause_equiv ();

	if (nt2clause_disp) sxfree (nt2clause_disp), nt2clause_disp = NULL;
	if (clause_disp2clause) sxfree (clause_disp2clause), clause_disp2clause = NULL;

	print_new_clauses ();

	sxfree (clause2disp), clause2disp = NULL;
	sxfree (new_clause2disp), new_clause2disp = NULL;
	sxfree (new_clauses_body), new_clauses_body = NULL;
	sxfree (new_nts), new_nts = NULL;
	sxfree (nt2ident), nt2ident = NULL;

	if (sxverbosep) {
	    fputs ("done\n", sxtty);
	    sxttycol1p = true;
	}
    }

    /* On remplit les from2var2... */
    new_max_nt = max_nt+new_nts_top;

    from2var2clause = (SXINT*) sxcalloc (new_clause_nb+1, sizeof (SXINT));
     
    for (clause = 1; clause <= last_clause; clause++)
	from2var2clause [clause2clause2var_form [clause]] = clause;

{
    /* On remplit les rhs_stack_map... */
    SXINT		*pos_list2clause, x;
    XH_header	XH_pos_list;

    XH_alloc (&XH_pos_list, "XH_pos_list", new_clause_nb+1, 1, max_rhs_prdct+1, NULL, statistics);
    pos_list2clause = (SXINT*) sxalloc (last_clause+1, sizeof (SXINT));
    clause2rhs_stack_map = (SXINT*) sxalloc (last_clause+1, sizeof (SXINT));
    clause2rhs_stack_map [0] = 0;
    rhs_stack_map = (SXINT*) sxalloc (last_clause*(max_rhs_prdct+1)+1, sizeof (SXINT));
    rhs_stack_map [0] = 0;
    rhs_stack_map_top = 1;

    for (clause = 1; clause <= last_clause; clause++) {
	if (clause2identical [clause] == clause && clause >= init_clause_nb) {
	    /* On deplie */
	    clause2rhs_stack_map [clause] = rhs_stack_map_top;
	    new_clause = clause2new_clause [clause];

	    fill_rhs_map (new_clause);

	    rhs_bot2 = clause2rhs_stack_map [clause];

	    for (k = 1, rhs_cur2 = rhs_bot2; rhs_cur2 < rhs_stack_map_top; rhs_cur2++, k++) {
		if (rhs_stack_map [rhs_cur2] != k)
		    break;
	    }

	    if (rhs_cur2 < rhs_stack_map_top) {
		/* On regarde si cette liste est nouvelle */
		
		for (rhs_cur2 = rhs_bot2; rhs_cur2 < rhs_stack_map_top; rhs_cur2++) {
		    XH_push (XH_pos_list, rhs_stack_map [rhs_cur2]);
		}

		if (XH_set (&XH_pos_list, &x)) {
		    /* La sequence existe deja en x */
		    rhs_stack_map_top = clause2rhs_stack_map [clause];
		    clause2rhs_stack_map [clause] = clause2rhs_stack_map [pos_list2clause [x]];
		}
		else {
		    /* Nouvelle sequence */
		    pos_list2clause [x] = clause;
		    rhs_stack_map [rhs_stack_map_top++] = 0; /* fin */
		}
	    }
	    else {
		/* identity mapping */
		rhs_stack_map_top = clause2rhs_stack_map [clause];
		clause2rhs_stack_map [clause] = 0;
	    }
	}
	else
	    clause2rhs_stack_map [clause] = 0;
    }

    sxfree (clause2clause2var_form);
    sxfree (clause2new_clause);
    sxfree (new_clause2rhs_disp);
    sxfree (new_clause2rhs_list);
    XH_free (&XH_pos_list);
    sxfree (pos_list2clause);
}

    fclose (out_file);

    if (sxverbosep && last_clause == new_clause_nb) {
	fprintf (sxtty, "WARNING: The input RCG is already in 2-var form; \"%s\", the generated RCG is useless.\n", lst_name);
	sxttycol1p = true;
    }
}


/* ******************************************************************************************************* */
/* FACTORISATION des "sous-arbres" */
static SXINT		max_new_nt, max_peql;
static SXINT		*nf_clause2lhs, *nf_clause2rhs, *clause2next, *clause2lhs_nt;
static SXINT		*new_clause_rhs_nts, new_clause_rhs_nts_top, *new_clause2rhs_nt_disp;
static SXINT		*nt2clause_nb, *clause2eq, *clause2eq_list, *nt2ident_list, *clause2ident, *clause2ident_list;
static SXBA		clause1_set, clause2_set, pseudo_equiv_hd_set;

static int
new_compar (const SXINT *p1, const SXINT *p2)
{
    SXINT	item1, item2;
    SXINT cur1 = *p1, cur2 = *p2;

    item1 = new_clauses_body [cur1++];
    item2 = new_clauses_body [cur2++];

    if (item1 != item2) return item1 < item2 ? -1 : 1;

    item1 = new_clauses_body [cur1++];
    item2 = new_clauses_body [cur2++];

    if (item1 != item2) return item1 > item2 ? -1 : 1; /* On met les predicats predef en fin */

    while ((item1 = new_clauses_body [cur1++]) != EOP && (item2 = new_clauses_body [cur2++]) != EOP) {
	if (item1 != item2) {
	    if (item1 == EOA || item2 == EOA) {
		return item1 == EOA  ? -1 : 1;
	    }

	    return item1 < item2 ? -1 : 1;
	}
    }

    if (item1 != item2) {
	return item1 == EOP  ? -1 : 1;
    }

    return 0; /* egalite */
}


static void
normal_form (void)
{
    /* Pour chaque clause, on renomme les variables et on trie les appels de predicats en rhs
       On met 0 comme nom de predicat utilisateur */
    SXINT		clause, new_clause_top, new_clause, bot, cur, top, new_var, symb, symb_list, prdct_call_nb, nf_prdct_pos;
    SXINT		arg_list, lhs_nt, item;
    SXINT		*var2new, *to_be_sorted;
    XH_header	XH_prdct_list, XH_arg_list, XH_symb_list;

    var2new = (SXINT*) sxcalloc (2*max_gvar+1, sizeof (SXINT));
    to_be_sorted = (SXINT *) sxalloc (new_max_rhs_prdct+1, sizeof (SXINT));

    XH_alloc (&XH_prdct_list, "XH_prdct_list", new_clause_nb+1, 1, (max_rhs_prdct/2)+1, NULL, statistics);
    XH_alloc (&XH_arg_list, "XH_arg_list", max_prdct_nb+1, 1, max_garity+3, NULL, statistics);
    XH_alloc (&XH_symb_list, "XH_symb_list", 2*max_prdct_nb+1, 1, max_arg_size+1, NULL, statistics);

    for (clause = init_clause_nb; clause <= last_clause; clause++) {
	new_clause_top = clause2disp [clause+1];

	for (new_clause = clause2disp [clause]; new_clause < new_clause_top; new_clause++) {
	    bot = new_clause2disp [new_clause];
	    top = new_clause2disp [new_clause+1];

	    clause2lhs_nt [new_clause] = lhs_nt = new_clauses_body [bot+1];

	    if (lhs_nt > max_nt) {
		/* c'est une nouvelle clause => c'est un candidat */
		XH_push (XH_arg_list, 0);
		cur = bot+2;
		XH_push (XH_arg_list, 0);
		nt2clause_nb [lhs_nt]++;

		new_var = 0;

		while ((symb = new_clauses_body [cur++]) != EOP) {
		    if (symb != EOA) {
			if (symb > 0) {
			    if (var2new [symb] == 0)
				var2new [symb] = ++new_var;

			    symb = var2new [symb];
			}

			XH_push (XH_symb_list, symb);
		    }
		    else {
			XH_set (&XH_symb_list, &symb_list);
			XH_push (XH_arg_list, symb_list);
		    }
		}

		XH_set (&XH_arg_list, &(nf_clause2lhs [new_clause]));

		prdct_call_nb = 0;

		while (cur < top) {
		    to_be_sorted [prdct_call_nb++] = cur;
		    cur += 2;
		    while (new_clauses_body [cur++] != EOP);
		}
	    
		if (prdct_call_nb > 1)
		    qsort (to_be_sorted, prdct_call_nb, sizeof (SXINT), (int (*) (const void *, const void *))new_compar);

		for (nf_prdct_pos = 0; nf_prdct_pos < prdct_call_nb; nf_prdct_pos++) {
		    cur = to_be_sorted [nf_prdct_pos];

		    XH_push (XH_arg_list, new_clauses_body [cur++]); /* call kind */
		    item = new_clauses_body [cur++];

		    new_clause_rhs_nts [++new_clause_rhs_nts_top] = item;

		    /* On empile 0 si item est un nouveau nt */
		    if (item > max_nt)
			item = 0;

		    XH_push (XH_arg_list, item);

		    while ((symb = new_clauses_body [cur++]) != EOP) {
			if (symb != EOA) {
			    if (symb > 0) {
				symb = var2new [symb];
			    }

			    XH_push (XH_symb_list, symb);
			}
			else {
			    XH_set (&XH_symb_list, &symb_list);
			    XH_push (XH_arg_list, symb_list);
			}
		    }

		    XH_set (&XH_arg_list, &arg_list);
		    XH_push (XH_prdct_list, arg_list);
		}

		XH_set (&XH_prdct_list, &(nf_clause2rhs [new_clause]));

		for (symb = 2*max_gvar; symb > 0; symb--)
		    var2new [symb] = 0;
	    }
	    else {
		/* Anciennes A-clauses ignorees (meme si elles referencent de nouveaux predicats */
		nf_clause2lhs [new_clause] = nf_clause2rhs [new_clause] = 0;
	    }

	    new_clause2rhs_nt_disp [new_clause+1] = new_clause_rhs_nts_top+1;
	}
    }

    sxfree (var2new), var2new = NULL;
    sxfree (to_be_sorted), to_be_sorted = NULL;

    XH_free (&XH_prdct_list);
    XH_free (&XH_arg_list);
    XH_free (&XH_symb_list);
}


static bool
is_pseudo_equiv (void)
{
    SXINT		new_clause, nf_clause, prev_clause, l, lhs;
    SXINT		*nf_clause2clause;
    XxY_header	nf_clause_hd;

    nf_clause2clause = (SXINT*) sxalloc (new_clause_nb+1, sizeof (SXINT));
    XxY_alloc (&nf_clause_hd, "nf_clause_hd", new_clause_nb+1, 1, 0, 0, NULL, statistics);

    for (new_clause = 1; new_clause <= new_clause_nb; new_clause++) {
	lhs = nf_clause2lhs [new_clause];

	if (lhs > 0) {
	    if (XxY_set (&nf_clause_hd, nf_clause2lhs [new_clause], nf_clause2rhs [new_clause], &nf_clause)) {
		prev_clause = nf_clause2clause [nf_clause];

		if ((l = clause2next [prev_clause]) == -2)
		    /* 1ere fois */
		    SXBA_1_bit (pseudo_equiv_hd_set, prev_clause);

		clause2next [new_clause] = --l; /* End of list */

		if (max_peql < -l)
		    max_peql = -l;
	    
		clause2next [prev_clause] = new_clause;
#if 0
		fprintf (out_file, "%ld et %ld sont pseudo-equivalents.\n", nf_clause2clause [nf_clause], new_clause);
#endif
	    }
	    else {
		clause2next [new_clause] = -2;

		if (max_peql < 2)
		    max_peql = 2;
	    }

	    nf_clause2clause [nf_clause] = new_clause;
	}
    }

    XxY_free (&nf_clause_hd);
    sxfree (nf_clause2clause);

    sxfree (nf_clause2lhs), nf_clause2lhs = NULL;
    sxfree (nf_clause2rhs), nf_clause2rhs = NULL;

    return nf_clause < new_clause_nb;
}

static bool
is_clause1_eq_clause2 (SXINT clause1, SXINT clause2)
{
    /* 2 clauses sont equivalentes ssi elles sont pseudo equivalentes et si tous les noms des predicats
       utilisateur en RHS sont 2 a 2 equivalents */
    SXINT bot1, cur1, top1, bot2, cur2, nt1, nt2;

    bot1 = new_clause2rhs_nt_disp [clause1];
    top1 = new_clause2rhs_nt_disp [clause1+1];
    bot2 = new_clause2rhs_nt_disp [clause2];

    for (cur1 = bot1, cur2 = bot2; cur1 < top1; cur1++, cur2++) {
	nt1 = new_clause_rhs_nts [cur1];

	if (nt1 > 0) {
	    nt2 = new_clause_rhs_nts [cur2]; /* nt2 > 0 */

	    if (nt1 != nt2 && nt2ident [nt1] != nt2ident [nt2])
		return false;
	}
    }

    return true;
}

static void
put_in_eq (SXINT *eq, SXINT *eq_list, SXINT it1, SXINT it2)
{
    /* it1 < it2 */
    /* indique ds le tableau eq que les elements it1 et it2 sont equivalents */
    /* eq_list contient pour chaque x la liste des elements y >= x qui sont equivalents a x */
    SXINT x1, prev_x1, it;

    it1 = eq [it1];
    it2 = eq [it2];

    if (it2 < it1) {
	it = it2;
	it2 = it1;
	it1 = it;
    }

    x1 = it1;
    prev_x1 = 0;

    /* fusion des listes eq_list [it1] et eq_list [it2] et maj de eq */
    do {
	while (x1 > prev_x1 && x1 < it2) {
	    prev_x1 = x1;
	    x1 = eq_list [prev_x1];
	}

	eq [it2] = it1;
	eq_list [prev_x1] = it2;
	prev_x1 = it2;
	it2 = eq_list [prev_x1];
	eq_list [prev_x1] = (x1 > prev_x1) ? x1 : prev_x1;
    } while (it2 > prev_x1);
}

static bool
is_nt1_eq_nt2 (SXINT nt1, SXINT nt2)
{
    SXINT		bot1, cur1, top1, bot2, cur2, top2, clause1, clause2, nb1, nb2;
    bool	ret_val;

    /* Toutes les nt1-clauses et les nt2-clauses ont des equivalents */
    /* Sont-ce les memes ? */
    bot1 = nt2clause_disp [nt1];
    top1 = nt2clause_disp [nt1+1];
    nb1 = 0;

    for (cur1 = bot1; cur1 < top1; cur1++) {
	clause1 = clause2eq [clause_disp2clause [cur1]];

	if (SXBA_bit_is_reset_set (clause1_set, clause1))
	    nb1++;
    }

    bot2 = nt2clause_disp [nt2];
    top2 = nt2clause_disp [nt2+1];
    nb2 = 0;

    for (cur2 = bot2; cur2 < top2; cur2++) {
	clause2 = clause2eq [clause_disp2clause [cur2]];

	if (!SXBA_bit_is_set (clause2_set, clause2)) {
	    if (!SXBA_bit_is_set_reset (clause1_set, clause2)) {
		top2 = cur2;
		break;
	    }

	    nb2++;
	    SXBA_1_bit (clause2_set, clause2);
	}
    }

    ret_val = (cur2 == top2) && (nb1 == nb2);

#if EBUG
    if ((cur2 == top2) && (nb1 != nb2))
	/* Normalement, ici, on a nb1 == nb2 */
	sxtrap (ME, "is_nt1_eq_nt2");
#endif

    for (cur2 = bot2; cur2 < top2; cur2++) {
	clause2 = clause2eq [clause_disp2clause [cur2]];
	SXBA_0_bit (clause2_set, clause2);
    }
    
    for (cur1 = bot1; cur1 < top1; cur1++) {
	clause1 = clause2eq [clause_disp2clause [cur1]];
	SXBA_0_bit (clause1_set, clause1);
    }

    return ret_val;
}


static void
process_clause_equiv (void)
{
    /* Passage en forme normale des clauses en forme 2var */
    /* renumerotation des variables */
    /* tri de la rhs */
    /* "suppression" des noms de predicats internes utilisateur ou construits par la mise en forme 2var */
    SXINT		nt, clause, lhs_nt, next, clause1, clause2, nt1, nt2, nb1, top, next1, next2;
    SXINT		*clause_stack;
    bool	done, has_eq;

    nf_clause2lhs = (SXINT *) sxalloc (new_clause_nb+1, sizeof (SXINT));
    nf_clause2rhs = (SXINT *) sxalloc (new_clause_nb+1, sizeof (SXINT));

    clause2next = (SXINT *) sxalloc (new_clause_nb+1, sizeof (SXINT));
    clause2eq = (SXINT *) sxalloc (new_clause_nb+1, sizeof (SXINT));
    clause2eq_list = (SXINT *) sxalloc (new_clause_nb+1, sizeof (SXINT));
    clause2ident = (SXINT *) sxalloc (new_clause_nb+1, sizeof (SXINT));
    clause2ident_list = (SXINT *) sxalloc (new_clause_nb+1, sizeof (SXINT));

    for (clause = 0; clause <= new_clause_nb; clause++)
	clause2ident [clause] = clause2ident_list [clause] = clause2eq [clause] = clause2eq_list [clause] = clause;

    clause2lhs_nt = (SXINT *) sxalloc (new_clause_nb+1, sizeof (SXINT));
    new_clause2rhs_nt_disp = (SXINT *) sxalloc (new_clause_nb+2, sizeof (SXINT));
    new_clause2rhs_nt_disp [1] = 1;

    new_clause_rhs_nts = (SXINT *) sxalloc (new_clause_nb*max_rhs_prdct+1, sizeof (SXINT));
    pseudo_equiv_hd_set = sxba_calloc (new_clause_nb+1);

    max_new_nt = max_nt+new_nts_top;
    nt2ident_list = (SXINT *) sxalloc (max_new_nt+1, sizeof (SXINT));
    nt2ident = (SXINT *) sxalloc (max_new_nt+1, sizeof (SXINT));

    for (nt = 0; nt <= max_new_nt; nt++)
	nt2ident [nt] = nt2ident_list [nt] = nt;

    nt2clause_nb = (SXINT *) sxcalloc (max_new_nt+1, sizeof (SXINT));

    normal_form ();

    if (is_pseudo_equiv ()) {
	clause1_set = sxba_calloc (new_clause_nb+1);
	clause2_set = sxba_calloc (new_clause_nb+1);
	nt2clause_disp = (SXINT *) sxalloc (max_new_nt+3, sizeof (SXINT));
	clause_disp2clause = (SXINT *) sxalloc (new_clause_nb+1, sizeof (SXINT));
	clause_stack = (SXINT*) sxalloc (new_clause_nb+1, sizeof (SXINT));
	clause_stack [0] = 0;

	/* Attention, finesse !! */
	nt2clause_disp [max_nt+1] = nt2clause_disp [max_nt+2] = 1;

	for (nt = max_nt+1; nt <= max_new_nt; nt++) {
	    nt2clause_disp [nt+1] = nt2clause_disp [nt] + nt2clause_nb [nt-1];
	}

	for (clause = init_clause_nb; clause <= new_clause_nb; clause++) {
	    lhs_nt = clause2lhs_nt [clause];

	    if (lhs_nt > max_nt)
		clause_disp2clause [nt2clause_disp [lhs_nt+1]++] = clause;
	}

	done = false;

	while (!done) {
	    done = true;
	    clause = 0;

	    while ((clause = sxba_scan (pseudo_equiv_hd_set, clause)) > 0) {
		next = clause2next [clause];

		for (clause1 = clause; next > 0; clause1 = next, next = clause2next [next]) {
		    if (clause2eq [clause1] == clause1) {
			has_eq = false;
			
			for (clause2 = next; clause2 > 0; clause2 = clause2next [clause2]) {
			    if (clause2eq [clause2] == clause2) {
				/* clause1 et clause2 sont pseudo equivalents ... */
				/* ... on regarde s'ils sont equivalents */
				if (is_clause1_eq_clause2 (clause1, clause2)) {
				    /* On enregistre que clause2eq [clause1] et clause2eq [clause2] sont 2 clauses equivalentes
				       (ayant meme partie droite et des LHS args compatibles) */
				    has_eq = true;
				    nt1 = clause2lhs_nt [clause1];
				    nt2 = clause2lhs_nt [clause2];

				    if (nt1 != nt2) {
					if (clause2eq_list [clause1] == clause1) {
					    /* liste triviale */
					    nt2clause_nb [nt1]--;
					}

					if (clause2eq_list [clause2] == clause2) {
					    /* liste triviale */
					    nt2clause_nb [nt2]--;
					}
				    }
				    else {
					if (clause2eq_list [clause1] == clause1 || clause2eq_list [clause2] == clause2) {
					    nt2clause_nb [nt1]--;
					}
				    }

				    put_in_eq (clause2eq, clause2eq_list, clause1, clause2);
				}
			    }
			}

			if (has_eq)
			    /* Debut d'une nouvelle liste de clauses equivalentes */
			    PUSH (clause_stack, clause1);
		    }
		}

		if ((top = clause_stack [0]) > 0) {
		    do {
			clause1 = clause_stack [top--];

			/* On extrait de la liste clause2eq_list[clause1] tous les doublets calcules */
			next1 = clause2eq_list [clause1];

			while (next1 > clause1) {
			    nt1 = clause2lhs_nt [clause1];
			    nb1 = nt2clause_nb [nt1];
			    clause2 = next1;

			    do {
				nt2 = clause2lhs_nt [clause2];

				if (nt1 == nt2) {
				    /* Ici, les A-clauses clause1 et clause2 sont egales */
#if 0
				    has_identical_clauses = true;

				    if (check_set) {
					sxerror (clause2visited [clause2]->token.source_index,
						 sxtab_ptr->err_titles [2][0],
						 "%sEquivalent with the clause defined at line #%ld.",
						 sxtab_ptr->err_titles [2]+1,
						 clause2visited [clause1]->token.source_index.line);
				    }

				    SXBA_1_bit (has_an_identical_clause_set, clause2);
				    XxY_erase (rcg_clauses, clause2);
#endif

				    put_in_eq (clause2ident, clause2ident_list, clause1, clause2);
				}

				if (nb1 <= 0  && (nt2clause_nb [nt2] <= 0)) {
				    if (nt2ident [nt1] != nt2ident [nt2]) {
					if (nt2 < nt1) {
					    nt = nt2;
					    nt2 = nt1;
					    nt1 = nt;
					}

					/* On regarde si nt1 et nt2 sont equivalents */
					if (is_nt1_eq_nt2 (nt1, nt2)) {
					    /* nt1 et nt2 sont equivalents */
#if 0
					    has_identical_clauses = true;
					    has_identical_nts = true;

					    if (nt1 > max_nt) {
						fprintf (out_file, "clause_%ld_%ld",
							 new_nts [nt1-max_nt].old_clause, new_nts [nt1-max_nt].new_clause);
					    }
					    else
						out_nt_text (nt1);

					    fprintf (out_file, "(nt1 = %ld) and ", nt1);

					    if (nt2 > max_nt) {
						fprintf (out_file, "clause_%ld_%ld",
							 new_nts [nt2-max_nt].old_clause, new_nts [nt2-max_nt].new_clause);
					    }
					    else
						out_nt_text (nt2);

					    fprintf (out_file, "(nt2 = %ld) are equivalent.\n", nt2);
#endif

					    done = false;
					    put_in_eq (nt2ident, nt2ident_list, nt1, nt2);
					}

				    }
				}

				next2 = clause2;
				clause2 = clause2eq_list [clause2];
			    } while (clause2 > next2);

			    clause1 = next1;
			    next1 = clause2eq_list [clause1];
			}
		    } while (top > 0);

		    clause_stack [0] = 0;
		}
	    }
	}

	sxfree (clause1_set), clause1_set = NULL;
	sxfree (clause2_set), clause2_set = NULL;
	sxfree (clause_stack), clause_stack = NULL;
    }

    sxfree (clause2next), clause2next = NULL;
    sxfree (clause2eq), clause2eq = NULL;
    sxfree (clause2eq_list), clause2eq_list = NULL;
    sxfree (clause2ident), clause2ident = NULL;
    sxfree (clause2ident_list), clause2ident_list = NULL;
    sxfree (clause2lhs_nt), clause2lhs_nt = NULL;
    sxfree (new_clause2rhs_nt_disp), new_clause2rhs_nt_disp = NULL;
    sxfree (new_clause_rhs_nts), new_clause_rhs_nts = NULL;
    sxfree (pseudo_equiv_hd_set), pseudo_equiv_hd_set = NULL;
    sxfree (nt2clause_nb), nt2clause_nb = NULL;
    sxfree (nt2ident_list), nt2ident_list = NULL;
}

static SXINT
print_new_prdct (bool is_lhs, SXINT cur)
{
    SXINT 	call_kind, nt, symb;
    bool 	is_strlen;

    call_kind = new_clauses_body [cur++];
    fprintf (out_file, "%s", (call_kind == 0) ? "" : "!");
    nt = new_clauses_body [cur++];

    if (nt > 0)
	nt = nt2ident [nt];

    if (nt > max_nt) {
	nt -= max_nt;
	fprintf (out_file, "clause_%ld_%ld", (SXINT) new_nts [nt].old_clause, (SXINT) new_nts [nt].new_clause);

	if (is_lhs)
	    new_nts_top++;
    }
    else {
	out_nt_text (nt);
    }

    is_strlen = nt == STRLEN_ic;

    fputs ("(", out_file);

    while ((symb = new_clauses_body [cur++]) != EOP) {
	if (symb != EOA) {
	    if (symb >= 0) {
		if (is_strlen) {
		    is_strlen = false;
		    fprintf (out_file, "%ld", (SXINT) symb);
		}
		else
		    out_var_text (symb);
	    }
	    else
		out_t_text (-symb);

	    if (new_clauses_body [cur] != EOA)
		fputs (" ", out_file);
	}
	else {
	    if (new_clauses_body [cur] != EOP)
		fputs (", ", out_file);
	}
    }

    fputs (") ", out_file);

    return cur;
}


static void
print_new_clause (SXINT bot, SXINT top)
{
    SXINT cur;

    cur = print_new_prdct (true, bot);
    fputs ("--> ", out_file);

    while (cur < top) {
	cur = print_new_prdct (false, cur);
    }

    fprintf (out_file, ". %% %ld\n", (SXINT) new_clause_nb);
}



static void
print_new_clauses (void)
{
    SXINT		clause, new_clause_top, new_clause, bot, lhs_nt, top;
    bool	is_first;
    SXINT		*new_clause2lnk, *lnk2clause, *lnk2lnk, lnk, lnk_top, clo;

    new_clause2lnk = (SXINT*) sxalloc (new_clause_nb+1, sizeof (SXINT));
    lnk2clause = (SXINT*) sxalloc (new_clause_nb+1, sizeof (SXINT));
    lnk2lnk = (SXINT*) sxalloc (new_clause_nb+1, sizeof (SXINT));

    /* On commence par recalculer le 2eme composant des nt correspondant aux supervar */
    new_clause_nb = new_axiom;

    for (clause = init_clause_nb; clause <= last_clause; clause++) {
	new_clause_top = clause2disp [clause+1];

	for (new_clause = clause2disp [clause]; new_clause < new_clause_top; new_clause++) {
	    bot = new_clause2disp [new_clause];
	    lhs_nt = new_clauses_body [bot+1];

	    if (lhs_nt == nt2ident [lhs_nt]) {
		new_clause_nb++;

		if (lhs_nt > max_nt) {
		    lhs_nt -= max_nt;
		    new_nts [lhs_nt].new_clause = new_clause_nb;
		}
	    }
	}
    }

    new_clause_nb = new_axiom;
    new_nts_top = 0;
    lnk_top = 1;

    for (clause = init_clause_nb; clause <= last_clause; clause++) {
	fprintf (out_file, "\n%% %ld: ", (SXINT) clause);
	out_clause (clause);
	new_clause_top = clause2disp [clause+1];
	is_first = true;

	for (new_clause = clause2disp [clause]; new_clause < new_clause_top; new_clause++) {
	    bot = new_clause2disp [new_clause];
	    lhs_nt = new_clauses_body [bot+1];

	    if (lhs_nt == nt2ident [lhs_nt]) {
		new_clause_nb++;
		top = new_clause2disp [new_clause+1];

		print_new_clause (bot, top);

		if (is_first) {
		    is_first = false;
		    clause2clause2var_form [clause] = new_clause_nb;
		}

		lnk2clause [lnk_top] = clause;
		lnk2lnk [lnk_top] = 0;
		new_clause2lnk [new_clause_nb] = lnk_top++;
	    }
	    else {
		if (lhs_nt <= max_nt) {
		    fputs ("%% ", out_file);
		    out_nt_text (lhs_nt);
		    fputs (" is equivalent to ", out_file);
		    out_nt_text (nt2ident [lhs_nt]);
		    fputs (".\n", out_file);
		}
		else {
		    lnk2clause [lnk_top] = clause;
		    clo = new_nts [lhs_nt-max_nt].new_clause;
		    lnk2lnk [lnk_top] = new_clause2lnk [clo];
		    new_clause2lnk [clo] = lnk_top++;
		}
	    }
	}
    }

    /* On remplit les from2var2... */
    from2var2clause_list_top = 0;

    for (new_clause = new_axiom+1; new_clause <= new_clause_nb; new_clause++) {
	for (lnk = new_clause2lnk [new_clause]; lnk > 0; lnk = lnk2lnk [lnk]) {
	    clause = lnk2clause [lnk];
	    from2var2clause_list [++from2var2clause_list_top] = clause;
	}

	from2var2clause_disp [new_clause+1] = from2var2clause_list_top+1;
    }

    sxfree (new_clause2lnk);
    sxfree (lnk2clause);
    sxfree (lnk2lnk);
}


#if 0
static void
nt1_nt22clauses (SXINT nt1, SXINT nt2)
{
    /* nt1 et nt2 sont equivalents on associe a chaque clause de nt2 la clause de nt1 qui lui est equivalente */
    SXINT		bot1, cur1, top1, bot2, cur2, top2, clause, clause1, clause2;

    bot1 = nt2clause_disp [nt1];
    top1 = nt2clause_disp [nt1+1];

    bot2 = nt2clause_disp [nt2];
    top2 = nt2clause_disp [nt2+1];

    for (cur1 = bot1; cur1 < top1; cur1++) {
	clause1 = clause_disp2clause [cur1];

	if (clause2identical [clause1] == clause1) {
	    clause = clause2eq [clause1];

	    for (cur2 = bot2; cur2 < top2; cur2++) {
		clause2 = clause_disp2clause [cur2];

		if (clause2eq [clause2] == clause) {
		    SXBA_1_bit (has_an_identical_clause_set, clause2);
		    clause2identical [clause2] = clause1;
		    XxY_erase (rcg_clauses, clause2);
		}
	    }
	}
    }
}

/* Appele depuis rcg_smp.  Calcule les equivalences entre predicats et entre clauses */
void
do_reduction (void)
{
    SXINT		size, clause, prdct, bot2, top2, call_kind, nt, param, bot3, top3, symb, bot, top, *pnt, new_nt, rhs;
    SXINT		nt1, nt2, clause2, next2;

    clause2disp = (SXINT*) sxalloc (last_clause+2, sizeof (SXINT));
    new_clause2disp = (SXINT*) sxalloc (last_clause*(max_rhs_prdct+1)+2, sizeof (SXINT));
    size = last_clause*(3	/*call_kind+nt+EOP*/*(max_rhs_prdct+1/*lhs_prdct*/)+
			max_clause_arg_nb*(max_arg_size+1 /*EOA*/)
			+1	/*EOC*/);
    new_clauses_body = (SXINT*) sxalloc (size+1, sizeof (SXINT));
    new_nts = (struct new_nts*) sxalloc ((new_nts_size = last_clause)+1, sizeof (struct new_nts));
    slice2new_nt = (SXINT*) sxalloc (max_gvar+1, sizeof (SXINT));
    clause2disp [1] = new_clause2disp [1] = 1;

    for (clause = 1; clause <= last_clause; clause++) {
	new_clause_nb++;
	prdct = XxY_X (rcg_clauses, clause) & lhs_prdct_filter;

	bot2 = XH_X (rcg_predicates, prdct);
	top2 = XH_X (rcg_predicates, prdct+1);

	call_kind = XH_list_elem (rcg_predicates, bot2++);
	add_item (call_kind);
	nt = XH_list_elem (rcg_predicates, bot2++);
	add_item (nt);

	while (bot2 < top2) {
	    param = XH_list_elem (rcg_predicates, bot2++);

	    bot3 = XH_X (rcg_parameters, param);
	    top3 = XH_X (rcg_parameters, param+1);

	    while (bot3 < top3) {
		symb = XH_list_elem (rcg_parameters, bot3++);
		add_item (symb);
	    }

	    close_new_arg();
	}

	close_new_prdct();
	
	rhs = XxY_Y (rcg_clauses, clause);

	if (rhs > 0) {
	    bot = XH_X (rcg_rhs, rhs);
	    top = XH_X (rcg_rhs, rhs+1);

	    while (bot < top) {
		prdct = XH_list_elem (rcg_rhs, bot++);
		bot2 = XH_X (rcg_predicates, prdct);
		top2 = XH_X (rcg_predicates, prdct+1);
		
		call_kind = XH_list_elem (rcg_predicates, bot2++);
		add_item (call_kind);
		nt = XH_list_elem (rcg_predicates, bot2++);
		add_item (nt);

		while (bot2 < top2) {
		    param = XH_list_elem (rcg_predicates, bot2++);

		    bot3 = XH_X (rcg_parameters, param);
		    top3 = XH_X (rcg_parameters, param+1);

		    while (bot3 < top3) {
			symb = XH_list_elem (rcg_parameters, bot3++);
			add_item (symb);
		    }

		    close_new_arg();
		}

		close_new_prdct();
	    }
	}

	close_new_clause(new_clause_nb);
	close_clause();
    }

    process_clause_equiv ();

    if (has_identical_nts) {
	for (nt1 = 1; nt1 <= max_nt; nt1++) {
	    nt2 = nt2ident [nt1];

	    if (nt2 != nt1) {
		if (check_set) {
		    sxtmsg (sxsrcmngr.source_coord.file_name,
			"%sPredicate names %s and %s are equivalent.",
				 sxtab_ptr->err_titles [2]+1,
				 sxstrget (nt2ste [nt1]),
				 sxstrget (nt2ste [nt2]));
		}
		else {
		    SXBA_1_bit (has_an_identical_nt_set, nt2);
		    nt2identical [nt1] = nt2;
		}

		nt1_nt22clauses (nt1, nt2);
	    }
	    else
		nt2identical [nt1] = nt1;
	}

	top = XH_top (rcg_predicates);

	for (prdct = 1; prdct <= top; prdct++) {
	    pnt = &(XH_list_elem (rcg_predicates, XH_X (rcg_predicates, prdct)+1));
	    nt =*pnt;

	    if (nt > 0) {
		if ((new_nt = nt2ident [nt]) != nt)
		    *pnt = new_nt;
	    }
	}
    }

    if (nt2clause_disp) sxfree (nt2clause_disp), nt2clause_disp = NULL;
    if (clause_disp2clause) sxfree (clause_disp2clause), clause_disp2clause = NULL;

    sxfree (clause2eq), clause2eq = NULL;
    sxfree (clause2disp), clause2disp = NULL;
    sxfree (new_clause2disp), new_clause2disp = NULL;
    sxfree (new_clauses_body), new_clauses_body = NULL;
    sxfree (new_nts), new_nts = NULL;
    sxfree (nt2ident), nt2ident = NULL;

    new_clauses_body_top = new_prdct_nb = new_clause_nb = new_max_rhs_prdct = 0;
}
#endif


/* ********************************************************************************** */
/* LRFSA */
#define is_fsa2rcg false

/* Les essais ont montre que ca marchait mieux ... */
#define process_single_transition false
#define is_early_cycle_erasing true

/* h_value : hauteur de la pile pour construire un fsa a partir d'une simulation du pda */
/* lfsa_file : l'automate lira le source de la gauche vers la droite et notera les bornes sup des reductions */
/* rfsa_file : l'automate lira le source de la droite vers la gauche et notera les bornes inf des reductions */
/* is_left_corner : le pda initial est un automate left_corner */
/* !is_left_corner : le pda initial est un automate LR(0) */
/* Si useless_clause_file est non NULL, on fabrique useless_clause_set, ensemble des clauses dont on ne
   veut pas reconnaitre les bornes (cas du shallow parsing par exemple) */
/* toutes les clauses de useless_clause_set, lors du passage pda -> ndfsa sont notees pas une transition sur 0 (elles
   deviennent donc equivalentes). */
/* Si used_nfndfsa : Les transitions sur 0 sont eliminees du ndfsa avant le passage en fsa 
   Ds le cas de l'option -tag (construction d'un "taggeur") il semble que l'option -nfndfsa donne une
   analyse beaucoup + rapide (mais un peu long a la construction) */
/* S'il reste des transitions sur 0, lors du passage fsa->RCG, elles ne generent aucun "shift" */


static bool is_bnf_in_reverse, fsa_has_reduction;

static SXBA    useless_clause_set, shallow_set, eps_clause_set;
static SXBA    *clause2lex_set;
static SXBA    clause_has_lex;
static SXBA    *state2look_ahead_set;
static bool has_null_trans, is_deterministic;
static SXINT     tmax, ntmax, last_prod;

static struct bnf {
  SXINT tmax, ntmax, maxnpd, rhs_lgth, nbpro, indpro, last_action;
  SXINT *reduc, *prolon, *numpg, *lispro, *prolis, *npd, *npg, *numpd, *action;
} bnf;


struct k_equiv {
  SXINT glbl, local;
};

static struct k_equiv *marked;
static SXINT *dfsa2eq;


static XH_header ndfsa_states;

#define pda_top(p)	(XH_list_elem (ndfsa_states, XH_X (ndfsa_states, p+1)-1))

static XxYxZ_header pda_hd, ndfsa_hd, dfsa_hd, fsa_hd;
static SXINT pda_assoc_list [6] = {1, 0, 1, 1, 0, 0}; /* X, Y, Z, XY, XZ, YZ */
static SXINT ndfsa_assoc_list [6] = {1, 0, 0, 1, 0, 0}; /* X, Y, Z, XY, XZ, YZ */
static SXINT dfsa_assoc_list [6] = {1, 0, 0, 0, 0, 0}; /* X, Y, Z, XY, XZ, YZ */
static SXINT fsa_assoc_list [6] = {1, 0, 0, 0, 0, 0}; /* X, Y, Z, XY, XZ, YZ */
static SXINT pda_state_nb, pda_init_state;
static SXINT ndfsa_init_state, ndfsa_state_nb;
static SXINT *ndfsa_state_stack;
static SXINT dfsa_init_state, dfsa_state_nb, fsa_init_state, fsa_state_nb;
static SXBA ndfsa_final_state_set, dfsa_final_state_set, fsa_final_state_set;
static SXBA *NULL_RED, *RED, *CRED; /* fermeture transitive de p A->\alpha q */
static SXINT R_nb_de_cycles;
static XH_header mtrans_hd;

static  SXBA lex_set, prod_set;


static bool
fill_lex_set (SXINT clause, SXBA fill_lex_set_lex_set)
{
  SXINT prdct, bot2, param, cur3, top3, symb, rhs, cur, top;
  bool result = false;

  if (clause) {
    prdct = XxY_X (rcg_clauses, clause) & lhs_prdct_filter;
    bot2 = XH_X (rcg_predicates, prdct);
    param = XH_list_elem (rcg_predicates, bot2+2);

    if (param > 0) {
      top3 = XH_X (rcg_parameters, param+1);

      for (cur3 = XH_X (rcg_parameters, param); cur3 < top3; cur3++) {
	symb = XH_list_elem (rcg_parameters, cur3);

	if (symb < 0) {
	  symb = -symb;
	  SXBA_1_bit (fill_lex_set_lex_set, symb);
	  result = true;
	}
      }
    }

    rhs = XxY_Y (rcg_clauses, clause);

    if (rhs) {
      top = XH_X (rcg_rhs, rhs+1);
      cur = XH_X (rcg_rhs, rhs);

      while (cur < top) {
	prdct = XH_list_elem (rcg_rhs, cur++);
	bot2 = XH_X (rcg_predicates, prdct);
    
	if (XH_list_elem (rcg_predicates, bot2+1) == LEX_ic) {
	  /* On complete t_set */
	  /* A FAIRE : le completer aussi pour les &First, &Last et &StrEq spe'ciaux */
	  param = XH_list_elem (rcg_predicates, bot2+2);
	  top3 = XH_X (rcg_parameters, param+1);

	  for (cur3 = XH_X (rcg_parameters, param); cur3 < top3; cur3++) {
	    symb = -XH_list_elem (rcg_parameters, cur3);
	    SXBA_1_bit (fill_lex_set_lex_set, symb);
	    result = true;
	  }
	}
      }
    }
  }
  
  return result;
}

static void
pre_processing_clause_set (void)
{
  /* On note ds fsa_loop_clause_set les clauses de la forme
     A(\alpha) --> A(\alpha) + des & */
  /* le 10/4/2002 a la place de fsa_loop_clause_set, je prends loop_clause_set : il faut que la
     def des loop soit coherente entre les LRFSA et les parseurs RCG */
  SXINT clause, prdct, bot2, param;

  for (clause = 1; clause <= last_clause; clause++) {
    if (clause2identical [clause] == clause && !SXBA_bit_is_set (false_clause_set, clause)) {
      if (fill_lex_set (clause, clause2lex_set [clause])) {
	SXBA_1_bit (clause_has_lex, clause);
      }

      prdct = XxY_X (rcg_clauses, clause) & lhs_prdct_filter;
      bot2 = XH_X (rcg_predicates, prdct);
      param = XH_list_elem (rcg_predicates, bot2+2);

      if (param == 0)
	SXBA_1_bit (eps_clause_set, clause);
    }
  }
}

static void
free_bnf (struct bnf free_bnf_bnf)
{
  sxfree (free_bnf_bnf.reduc), free_bnf_bnf.reduc = NULL;
  sxfree (free_bnf_bnf.prolon), free_bnf_bnf.prolon = NULL;
  sxfree (free_bnf_bnf.numpg), free_bnf_bnf.numpg = NULL;
  sxfree (free_bnf_bnf.lispro), free_bnf_bnf.lispro = NULL;
  sxfree (free_bnf_bnf.prolis), free_bnf_bnf.prolis = NULL;
  sxfree (free_bnf_bnf.numpd), free_bnf_bnf.numpd = NULL;
  sxfree (free_bnf_bnf.npd), free_bnf_bnf.npd = NULL;
  sxfree (free_bnf_bnf.npg), free_bnf_bnf.npg = NULL;
  sxfree (free_bnf_bnf.action), free_bnf_bnf.action = NULL;
}

static void rcg2bnf (bool is_print);

/* Cette procedure transforme une 1_RCG simple en une CFG a la SYNTAX */
static void 
simple1_rcg2cfg (void)
{
  SXINT symb; 
  SXINT i, xnumpd, xnumpg;

  bnf.tmax = max_t;
  bnf.ntmax = max_nt;
  bnf.maxnpd = bnf.rhs_lgth = 0;
  bnf.last_action = last_clause;

  bnf.reduc = (SXINT*) sxalloc (bnf.last_action+2, sizeof (SXINT));
  bnf.prolon = (SXINT*) sxalloc (bnf.last_action+2, sizeof (SXINT));
  bnf.numpg = (SXINT*) sxalloc (bnf.last_action+2, sizeof (SXINT));
  bnf.lispro = (SXINT*) sxalloc (bnf.last_action*(max_arg_size+1)+4, sizeof (SXINT));
  bnf.prolis = (SXINT*) sxalloc (bnf.last_action*(max_arg_size+1)+4, sizeof (SXINT));
  bnf.npd = (SXINT*) sxalloc (bnf.ntmax+2, sizeof (SXINT));
  bnf.npg = (SXINT*) sxalloc (bnf.ntmax+2, sizeof (SXINT));

  bnf.action = (SXINT*) sxcalloc (bnf.last_action+1, sizeof (SXINT));

  nt2lhs_nb = (SXINT*) sxcalloc (bnf.ntmax+1, sizeof (SXINT));
  nt2rhs_nb = (SXINT*) sxcalloc (bnf.ntmax+1, sizeof (SXINT));


  /*-----------------------------------------------------------*/
  /*  D U M M Y   P R O D U C T I O N    P R O C E S S I N G   */
  /*-----------------------------------------------------------*/

  /* 0: <dummy_non_terminal> = <axiom> "End Of File" ; */

  bnf.reduc [0] = bnf.prolis [0] = bnf.lispro [0] = 0;
  bnf.lispro [1] = 1;
  bnf.prolis [1] = 0;
  bnf.maxnpd++;
  /* <axiom> in the RHS of the dummy rule */
  bnf.maxnpd++;
  /* compatibility with the old INTRO    */
  bnf.lispro [2] = -(bnf.tmax+1);
  bnf.prolis [2] = 0;
  bnf.prolis [3] = bnf.lispro [3] = 0;
  bnf.prolon [0] = 1;
  bnf.indpro = 3;
  bnf.action [0] = 0;
  nt2lhs_nb [0] = 1;
  nt2rhs_nb [1] = 1;
	  
  bnf.nbpro = 0;

  /* Il faut "calculer" la meme bnf que ds le cas -lbnf -rbnf */
  rcg2bnf (false /* On met ds bnf */);

#if 0
  {
    SXINT		clause;

    for (clause = 1; clause <= bnf.last_action; clause++) {
      if (clause2identical [clause] == clause
	  && !SXBA_bit_is_set (false_clause_set, clause)
	  && !SXBA_bit_is_set (loop_clause_set, clause)) {
	SXINT
	  prdct = XxY_X (rcg_clauses, clause) & lhs_prdct_filter,
	  bot2 = XH_X (rcg_predicates, prdct),
	  nt = XH_list_elem (rcg_predicates, bot2+1);
	SXINT	rhs, bot, cur, top, param;

	nt2lhs_nb [nt]++;
	bnf.nbpro++;
	bnf.action [bnf.nbpro] = clause;
	bnf.reduc [bnf.nbpro] = nt;
	bnf.prolon [bnf.nbpro] = ++bnf.indpro;

	rhs = XxY_Y (rcg_clauses, clause);
	bot = XH_X (rcg_rhs, rhs);
	top = XH_X (rcg_rhs, rhs+1);
	param = XH_list_elem (rcg_predicates, bot2+2);

	if (param > 0) {
	  SXINT
	    top3 = XH_X (rcg_parameters, param+1),
	    bot3 = XH_X (rcg_parameters, param),
	    l = 0,	
	  cur3 = is_bnf_in_reverse ? top3-1 : bot3;

	  for (;;) {
	    symb = XH_list_elem (rcg_parameters, cur3);

	    if (symb < 0) {
	      bnf.maxnpd++;
	      l++;
	    }
	    else {
	      cur = bot;

	      while (cur < top) {
		SXINT	rhs_nt;

		prdct = XH_list_elem (rcg_rhs, cur++);
		bot2 = XH_X (rcg_predicates, prdct);
		rhs_nt = XH_list_elem (rcg_predicates, bot2+1);

		if (rhs_nt > 0) {
		  param = XH_list_elem (rcg_predicates, bot2+2);

		  if (symb == XH_list_elem (rcg_parameters, XH_X (rcg_parameters, param))) {
		    bnf.maxnpd++;
		    l++;
		    symb = rhs_nt;
		    nt2rhs_nb [symb]++;

		    break;
		  }
		}
	      }
	    }

	    bnf.lispro [bnf.indpro] = symb;
	    bnf.prolis [bnf.indpro] = bnf.nbpro;
	    bnf.indpro++;

	    if (is_bnf_in_reverse) {
	      if (--cur3 < bot3) break;
	    }
	    else {
	      if (++cur3 >= top3) break;
	    }
	  }

	  if (l > bnf.rhs_lgth)
	    bnf.rhs_lgth = l;
	}

	bnf.lispro [bnf.indpro] = 0;
	bnf.prolis [bnf.indpro] = bnf.nbpro;
      }
    }
  }
#endif /* 0 */

  bnf.prolon [bnf.nbpro+1] = bnf.indpro+1;
  last_prod = bnf.nbpro;

  bnf.numpd = (SXINT*) /* 0:bnf.maxnpd */ sxalloc (bnf.maxnpd + 1, sizeof (SXINT));

  xnumpd = 1;
  xnumpg = 1;

  for (symb = 1; symb <= bnf.ntmax; symb++) {
    bnf.npg [symb] = xnumpg += nt2lhs_nb [symb];
    bnf.npd [symb] = xnumpd += nt2rhs_nb [symb];
  }

  bnf.npg [bnf.ntmax + 1] = xnumpg;
  bnf.npd [bnf.ntmax + 1] = xnumpd;

  for (i = bnf.nbpro; i >= 1; i--)
    bnf.numpg [--bnf.npg [bnf.reduc [i]]] = i;

  for (i = bnf.indpro; i >= 1; i--) {
    if ((symb = bnf.lispro [i]) > 0) {
      bnf.numpd [--bnf.npd [symb]] = i;
    }
  }

  sxfree (nt2lhs_nb);
  sxfree (nt2rhs_nb);
}




static SXBA *nt2item_set, *left_corner;

/* calcule la relation left_corner :
   left_corner = LC* avec
   A LC B <==> A -> B \beta \in P 

   Calcule de plus nt2item_set [A] = {B -> . \beta | A left_corner B} */
static void
LC_relation (void)
{
  SXINT 	xprod, item, A, B, lim, i;
  SXBA	line, init_line;

  for (xprod = 1; xprod <= bnf.nbpro; xprod++) {
    item = bnf.prolon [xprod];
    A = bnf.reduc [xprod];
    line = left_corner [A];
    B = bnf.lispro [item];

    if (B > 0)
      SXBA_1_bit (line, B);
  }

  fermer (left_corner, bnf.ntmax+1);

  for (A = 1; A <= bnf.ntmax; A++)
    SXBA_1_bit (left_corner [A], A);

  /* On remplit nt2item_set [A] = {B -> . \beta | A left_corner B } */
  for (A = 1; A <= bnf.ntmax; A++) {
    line = left_corner [A];
    init_line = nt2item_set [A];

    B = 0;

    while ((B = sxba_scan (line, B)) > 0) {
      for (lim = bnf.npg [B + 1], i = bnf.npg [B]; i < lim; i++) {
	item = bnf.prolon [bnf.numpg [i]];
	SXBA_1_bit (init_line, item);
      }
    }
  }
}



	

/* retourne l'etat [pi q] : h_value */
/* > 0 si nouveau, negatif si existe deja */
static SXINT
make_ndfsa_state (SXINT s, SXINT pi, SXINT delta, SXINT q)
{
    SXINT bot, top, result;

    if (pi == 0) {
      /* On fait [s q] : h_value */
	if (s && h_value > 1)
	    XH_push (ndfsa_states, s);

	XH_push (ndfsa_states, q);
    }
    else {
	/* On conserve les delta du fond et on ajoute q */
	bot = XH_X (ndfsa_states, pi);
	top = bot+delta;

	if (delta >= h_value)
	    bot++;

	while (bot < top) {
	    XH_push (ndfsa_states, XH_list_elem (ndfsa_states, bot));
	    bot++;
	}

	XH_push (ndfsa_states, q);
    }

    if (XH_set (&ndfsa_states, &result))
      return -result;

    return ndfsa_state_nb = result;
}


static void
reduce (SXINT s, SXINT pi, SXINT delta, SXINT prod)
{
  SXINT A, q, triple, foo, piq, X;

  /* prod = A -> \alpha */
  A = bnf.reduc [prod];

  if (s != 0) {
    /* On est ds le pda */
    XxYxZ_XYforeach (pda_hd, s, A, triple) {
      q = XxYxZ_Z (pda_hd, triple);

      if (q <= 0) {
	/* Ds le cas shift reduce le sommet peut etre negatif, il code alors triple */
	q = -triple;
	s = 0;
      }

      if ((piq = make_ndfsa_state (s, (SXINT)0, (SXINT)0, q)) > 0) {
	/* nouveau */
	S1_push (ndfsa_state_stack, piq);
      }
      else
	piq = -piq;

      if (is_shallow) {
	/* On s'occupe des predicats en lhs et non des prods */
	X = A;

	if (shallow_set != NULL && !SXBA_bit_is_set (shallow_set, A)) {
	  has_null_trans = true;
	  X = 0;
	}
      }
      else {
	/* On s'occupe des prods */
	X = bnf.action [prod];
	/* Transition entre pi et piq sur la prod X */

	if (useless_clause_set && SXBA_bit_is_set (useless_clause_set, X)) {
	  has_null_trans = true;
	  X = 0;
	}
	else {
	  /* Changement le 2/10/2002 Les lrfsa manipulent des prod et non des clauses */
	  X = prod;
	}
      }

      XxYxZ_set (&ndfsa_hd, pi, X, piq, &foo);
    }
  }
  else {
    /* On est ds pi, le fond de pile a une hauteur de delta */
#if EBUG
    if (pi == 0)
      sxtrap (ME, "reduce");
#endif

    s = XH_list_elem (ndfsa_states, XH_X (ndfsa_states, pi) + delta - 1);

    if (s < 0) {
      /* C'est un shift-reduce */
      s = XxYxZ_X (pda_hd, -s);
    }

    foo = 0;

    XxYxZ_XYforeach (pda_hd, s, A, triple) {
      q = XxYxZ_Z (pda_hd, triple); /* q peut etre negatif */

      if (q <= 0)
	q = -triple;

      if ((piq = make_ndfsa_state ((SXINT)0, pi, delta, q)) > 0) {
	/* nouveau */
	S1_push (ndfsa_state_stack, piq);
      }
      else
	piq = -piq;

      if (is_shallow) {
	/* On s'occupe des predicats en lhs et non des prods */
	X = A;

	if (shallow_set != NULL && !SXBA_bit_is_set (shallow_set, A)) {
	  has_null_trans = true;
	  X = 0;
	}
      }
      else {
	X = bnf.action [prod];
	/* Transition entre pi et piq sur la prod X */

	if (useless_clause_set && SXBA_bit_is_set (useless_clause_set, X)) {
	  has_null_trans = true;
	  X = 0;
	}
	else {
	  /* Changement le 2/10/2002 Les lrfsa manipulent des prod et non des clauses */
	  X = prod;
	}
      }

      XxYxZ_set (&ndfsa_hd, pi, X, piq, &foo);
    }

#if EBUG
    if (foo == 0)
      sxtrap (ME, "reduce");
#endif
  }
}


/* Appelle reduce sur tous les predecesseurs de pi a la distance l */
static void
pred (SXINT q, SXINT pi, SXINT delta, SXINT l, SXINT prod)
{
  SXINT triple, s;

  if (l == 0)
    reduce (q, pi, delta, prod);
  else {
    if (q == 0) {
      /* On est encore ds pi */
      if (delta == 1) {
	/* fond de la pile */
	q = XH_list_elem (ndfsa_states, XH_X (ndfsa_states, pi));

	if (q < 0) {
	  q = XxYxZ_X (pda_hd, -q);
	}
      }
    }

    if (q > 0) {
      /* On est ds le pda */
      XxYxZ_Zforeach (pda_hd, q, triple) {
	s = XxYxZ_X (pda_hd, triple);
	pred (s, pi, (SXINT)0, l-1, prod);
      }
    }
    else {
      /* On est dans la pile */
#if EBUG
      if (q < 0)
	sxtrap (ME, "pred");
#endif
      pred ((SXINT)0, pi, delta-1, l-1, prod);
    }
  }
}

static void
ndfsa_states_oflw (SXINT old_size, SXINT new_size)
{
  sxuse(old_size); /* pour faire taire gcc -Wunused */
  ndfsa_final_state_set = sxba_resize (ndfsa_final_state_set, new_size+1);
}

/* Calcule dans ndfsa_hd une couverture re'guliere de pda_hd en utilisant pour etats
   des suffixes de piles (de pda_hd) de hauteur h */
/* (p, X, q) : q > 0 => transition de p vers q sur X (X > 0 => non-terminal, X < 0 => terminal)
               q <= 0 => reduction
	                 q == 0 (X == -eof) etat final
			 q < 0 reduce -q
			       X == 0 => reduce normal
			       X != 0 => shift X reduce -q
*/

static void
pda2ndfsa (void)
{
  SXINT pi, delta, s, triple, X, q, l, piq, foo, prod;

  XH_alloc (&ndfsa_states, "ndfsa_states", pda_state_nb*h_value, 1, h_value, ndfsa_states_oflw, statistics);
  ndfsa_final_state_set = sxba_calloc (XH_size (ndfsa_states)+1);

  XxYxZ_alloc (&ndfsa_hd, "ndfsa_hd", XH_size (ndfsa_states)*2, 1, ndfsa_assoc_list, NULL, statistics);

  S1_alloc (ndfsa_state_stack, pda_state_nb+1);

  /* Calcul de l'etat initial de ndfsa */
  XH_push (ndfsa_states, pda_init_state);
  XH_set (&ndfsa_states, &ndfsa_init_state);
  ndfsa_state_nb = ndfsa_init_state;
    
  S1_push (ndfsa_state_stack, ndfsa_init_state);

  while (!S1_is_empty (ndfsa_state_stack)) {
    pi = S1_pop (ndfsa_state_stack);
    /* delta = |pi| */
    delta = XH_list_lgth (ndfsa_states, pi);
    s = pda_top (pi);

    if (s < 0) {
      triple = -s;
      X = XxYxZ_Y (pda_hd, triple);
      prod = -XxYxZ_Z (pda_hd, triple);
#if EBUG
      if (X == 0 || X < -bnf.tmax || prod <= 0)
	/* C'est un shift reduce pas sur eof */
	sxtrap (ME, "pda2ndfsa");
#endif
      /* prod est une reduction  A -> \alpha X */
      l = bnf.prolon [prod+1]-bnf.prolon [prod]-2;

      pred ((SXINT)0, pi, delta, l, prod);
    }
    else {
      XxYxZ_Xforeach (pda_hd, s, triple) {
	X = XxYxZ_Y (pda_hd, triple);

	if (X <= 0) {
	  /* Les transitions non-terminales sont supprimees */
	  if (X < -bnf.tmax) {
	    /* etat final */
	    /* Aucune transition depuis piq, on ne lui donne pas de prefixe */
	    piq = make_ndfsa_state ((SXINT)0, (SXINT)0, (SXINT)0, (SXINT)0);
#if EBUG
	    if (piq < 0)
	      /* unique */
	      sxtrap (ME, "pda2ndfsa");
#endif
	    XxYxZ_set (&ndfsa_hd, pi, X, piq, &foo);
	    /* On dit que piq est final */
	    SXBA_1_bit (ndfsa_final_state_set, piq);
	  }
	  else {
	    q = XxYxZ_Z (pda_hd, triple);

	    if (q > 0) {
	      /* Transition de p vers q sur le terminal X */
	      /* On supprime les transitions non-terminales */
	      if ((piq = make_ndfsa_state ((SXINT)0, pi, delta, q)) > 0) {
		/* nouveau */
		S1_push (ndfsa_state_stack, piq);
	      }
	      else
		piq = -piq;

	      XxYxZ_set (&ndfsa_hd, pi, X, piq, &foo);
	    }
	    else {
	      /* [scan-]reduce */
	      if (X < 0) {
		/* scan-reduce */
		if ((piq = make_ndfsa_state ((SXINT)0, pi, delta-1, -triple)) > 0) {
		  /* nouveau */
		  S1_push (ndfsa_state_stack, piq);
		}
		else
		  piq = -piq;

		XxYxZ_set (&ndfsa_hd, pi, X, piq, &foo);
	      }
	      else {
		/* reduce */
		prod = -q;
#if EBUG
		if (prod == 0)
		  sxtrap (ME, "pda2ndfsa");
#endif
		/* prod est une reduction  A -> \alpha */
		l = bnf.prolon [prod+1]-bnf.prolon [prod]-1;
		pred ((SXINT)0, pi, delta, l, prod);
	      }
	    }
	  }
	}
      }
    }
  }

  S1_free (ndfsa_state_stack), ndfsa_state_stack = NULL;
  XH_free (&ndfsa_states);
}


/* Calcul de l'automate LC(0) */
/* delta(s,X) = q <==> (s,X,q) \in pda_hd */
/* On code les "reduce" ds l'automate c,a permet une vision uniforme qqsoit le type d'automate :
   Si p = A -> \alpha . \in q <==> (s,0,-p) \in LC0_hd */
/* En cas de shift-reduce (ce qui est toujours le cas en LC on code par
   (s,X,-p) */
/* L'etat final f est code' par (f, -eof, 0) */

static void
LC0_automaton (void)
{
  SXINT s, X, xprod, triple, item;
  SXBA line;

  pda_state_nb = 0;
  pda_init_state = 1;

  for (s = 1; s <= bnf.indpro; s++) {
    X = bnf.lispro [s];

    if (!is_shift_reduce || X != 0) {
      /* Les reduce dont la partie droite est non vide sont tous codes par des "shift-reduce" */
      xprod = bnf.prolis [s];

      if (s == 1 || s > bnf.prolon [xprod]) {
	/* On saute les items de la forme [A -> . \alpha] qui sont traites ds la fermeture */
	pda_state_nb++;

	if (s == 2) {
	  /* Etat final a la forme (s, -eof, 0) */
	  XxYxZ_set (&pda_hd, 2, X, 0, &triple);
	}
	else {
	  if (X == 0 || (is_shift_reduce && bnf.lispro [s+1] == 0)) {
	    /* cas reduce ou shift-reduce */
	    XxYxZ_set (&pda_hd, s, X, -xprod, &triple);
	  }
	  else {
	    XxYxZ_set (&pda_hd, s, X, s+1, &triple);

	    if (X > 0) {
	      line = nt2item_set [X];

	      item = 0;

	      while ((item = sxba_scan (line, item)) > 0) {
		if (item != s) {
		  X = bnf.lispro [item];
      
		  if (X == 0) {
		    /* Reduction vide */
		    /* Les "etats reduction" sont codes en <=0 a cause des Zforeach */
		    XxYxZ_set (&pda_hd, s, 0, -bnf.prolis [item], &triple);
		  }
		  else {
		    if (is_shift_reduce && bnf.lispro [item+1] == 0) {
		      /* cas shift reduce */
		      XxYxZ_set (&pda_hd, s, X, -bnf.prolis [item], &triple);
		    }
		    else {
		      XxYxZ_set (&pda_hd, s, X, item+1, &triple);
		    }
		  }
		}
	      }
	    }
	  }
	}
      }
    } 
  }
}


static SXINT	limitem, limt, limnt;
static SXINT	*tsymb1, *items_list, *nt_stack;
static SXBA	nt_set;
static bool	is_t_trans, is_nt_trans;

static	void
mettre (SXINT item)
{
  SXINT	x, tnt;
  SXINT	*q;

  q = tsymb1 + (tnt = bnf.lispro [item]);

  if (tnt != 0) {
    if (*q == limitem) {
      /* Nouvelle sous_liste */
      if (tnt > 0) {
	if (SXBA_bit_is_reset_set (nt_set, tnt)) {
	  is_nt_trans = true;
	  nt_stack [(*nt_stack)++] = tnt;
	}
      }
      else {
	SXBA_1_bit (t_set, -tnt);
	is_t_trans = true;
      }
    }
  }

  while ((x = *q) <= item)
    q = items_list + x;

  *q = item;
  items_list [item] = x;
}


static void
LR0_automaton (void)
{
  SXINT	        i, xac1, x, lim, xnt_stack, xt, xnt, item, next_state;
  SXINT           triple, bot;
  XH_header	nucleus_hd;

  limitem = bnf.indpro + 1;
  limt = -bnf.tmax - 1;
  limnt = bnf.ntmax + 1;

  XH_alloc (&nucleus_hd, "nucleus", bnf.indpro, 2, 4, NULL, statistics);

  items_list = (SXINT*) sxalloc (limitem, sizeof (SXINT));
  tsymb1 = (SXINT*) sxalloc (limnt - limt, sizeof (SXINT)) - limt;
  /* limt est negatif (-eof) */
  nt_stack = (SXINT*) sxcalloc (limnt, sizeof (SXINT));
  nt_stack [0] = 1;
  nt_set = sxba_calloc (limnt);

  for (i = limt; i < limnt; i++) {
    tsymb1 [i] = limitem;
  }

  /* initialisation */
  XH_push (nucleus_hd, 0);
  XH_set (&nucleus_hd, &pda_init_state);

  for (xac1 = 1; xac1 < XH_top (nucleus_hd); xac1++) {
    is_t_trans = is_nt_trans = false;
	    
    /* Fermeture. */
    for (lim = XH_X (nucleus_hd, xac1 + 1), x = XH_X (nucleus_hd, xac1); x < lim; x++) {
      mettre (XH_list_elem (nucleus_hd, x) + 1);
    }

    for (xnt_stack = 1; xnt_stack < *nt_stack; xnt_stack++) {
      x = nt_stack [xnt_stack];

      for (lim = bnf.npg [x + 1], i = bnf.npg [x]; i < lim; i++) {
	mettre (bnf.prolon [bnf.numpg [i]]);
      }
    }

    /* Traitement des transitions terminales. */
    if (is_t_trans) {
      /* Il y a des transitions terminales */
      xt = 0;

      while ((xt = sxba_scan_reset (t_set, xt)) > 0) {
	item = tsymb1 [-xt];
	bot = XH_list_top (nucleus_hd);

	do {
	  if (is_shift_reduce && bnf.lispro [item+1] == 0) {
	    /* item scan-reduce */
	    /* Attention ce codage de l'automate LR(0) n'est pas deterministe !! */
	    XxYxZ_set (&pda_hd, xac1, -xt, -bnf.prolis [item], &triple);
	  }
	  else {
	    XH_push (nucleus_hd, item);
	  }
	} while ((item = items_list [item]) != limitem);

	if (XH_list_top (nucleus_hd) > bot) {
	  XH_set (&nucleus_hd, &next_state);
	  XxYxZ_set (&pda_hd, xac1, -xt, next_state, &triple);
	}

	tsymb1 [-xt] = limitem;
      }
    }

    /* Traitement des transitions non-terminales. */
    if (is_nt_trans) {
      /* Il y a des transitions non-terminales */
      xnt = 0;

      while ((xnt = sxba_scan_reset (nt_set, xnt)) > 0) {
	item = tsymb1 [xnt];
	bot = XH_list_top (nucleus_hd);

	do {
	  if (is_shift_reduce && bnf.lispro [item+1] == 0) {
	    /* item scan-reduce */
	    /* Attention ce codage de l'automate LR(0) n'est pas deterministe !! */
	    XxYxZ_set (&pda_hd, xac1, xnt, -bnf.prolis [item], &triple);
	  }
	  else {
	    XH_push (nucleus_hd, item);
	  }
	} while ((item = items_list [item]) != limitem);

	if (XH_list_top (nucleus_hd) > bot) {
	  XH_set (&nucleus_hd, &next_state);
	  XxYxZ_set (&pda_hd, xac1, xnt, next_state, &triple);
	}

	tsymb1 [xnt] = limitem;
      }

      /* Pour le coup suivant. */
      nt_stack [0] = 1;
    }

    /* Traitement des reductions. */
    if ((item = tsymb1 [0]) != limitem) {
      do {
	/* Les "etats reduction" sont codes en <=0 a cause des Zforeach */
	XxYxZ_set (&pda_hd, xac1, 0, -bnf.prolis [item], &triple);
      } while ((item = items_list [item]) != limitem);

      tsymb1 [0] = limitem;
    }
  }

  pda_state_nb = XH_top (nucleus_hd);
  XH_free (&nucleus_hd);
  sxfree (nt_stack), nt_stack = NULL;
  sxfree (nt_set), nt_set = NULL;
  sxfree (tsymb1 + limt), tsymb1 = NULL;
  sxfree (items_list), items_list = NULL;
}


/* Nouvelle version de fermer dans laquelle la relation est d'abord partitionnee en morceaux connexes */
static SXBA *R, *F, R_deja_traite, *C, used_p2c, used_c;
static SXINT *p2kind, *p2c, *c2p, *c2kind;
static bool R_is_cyclic, appel_a_fermer;

/* Si c n'est pas deja un cycle, on cree un nouveau cycle de representant p */
static SXINT
create_new_cycle (SXINT p, SXINT c)
{
  if (c) return c;

  c = sxba_0_lrscan (used_c, 0);

  if (c == -1)
    sxtrap (ME, "create_new_cycle");

  SXBA_1_bit (used_c, c);
      
  if (c2p [c])
    /* Reutilisation */
    sxba_empty (C [c]);

  c2p [c] = p; /* p est le representant de c */

  return c;
}

/* On ajoute p au cycle c */
static void
fill_p2c (SXINT p, SXINT c)
{
  SXINT old_c, r;
  SXINT *ptr;
  SXBA liner, lineor;

  sxinitialise(liner); /* pour faire taire gcc -Wuninitialized */
  if (c == 0 || (old_c = *(ptr = p2c+p)) == c) return;

  if (old_c == 0) {
    SXBA_1_bit (used_p2c, p);
    *ptr = c;

    if (appel_a_fermer) {
      /* On est ds le cas fermer */
      if ((r = c2p [c]) != p) {
	/* p n'est pas r, representant de c */
	F [p] = F [r]; /* On force p R* a` r R* */
      }
    }

    return;
  }

  /* ici old_c devient equivalent a c */
  if (appel_a_fermer) {
    liner = F [c2p [c]];

    if (liner != (lineor = F [c2p [old_c]]))
      sxba_or (liner, lineor);
  }

  /* On recupere old_c */
  p = 0;

  while ((p = sxba_scan (used_p2c, p)) > 0) {
    if (*(ptr = p2c+p) == old_c) {
      *ptr = c;

      if (appel_a_fermer) {
	/* p n'est pas r, representant de c */
	F [p] = liner; /* On force p F* a` r F* */
      }
    }
  }

  SXBA_0_bit (used_c, old_c);
}


static SXINT
is_cyclic (SXINT p)
{
  SXINT *ppkind, *pqkind, qkind, q, cp, cq;
  SXBA linep, linefp, linefq;

  SXBA_1_bit (R_deja_traite, p);

  if ((cp = p2c [p])) {
    /* p est un element du cycle cp */
    ppkind = c2kind + cp;
    linep = C [cp];
  }
  else {
    ppkind = p2kind + p;
    linep = R [p];
  }

  *ppkind = 1;

  q = -1;

  while ((q = sxba_scan_reset (linep, q)) >= 0) {
    if ((qkind = *(pqkind = (cq = p2c [q]) ? (c2kind + cq) : (p2kind + q))) == -1) {
      /* q est deja calcule' */
      if (appel_a_fermer) {
	if ((linefp = F [p]) != (linefq = F [q]))
	  sxba_or (linefp, linefq);

	SXBA_1_bit (linefp, q);
      }

      continue;
    }

    if (qkind == 1) {
      /* detection d'un cycle sur q qui contient p -> q */
      R_is_cyclic = true;

      *pqkind = 2; /* On depilera jusqu'a q */
      cq = create_new_cycle (q, cq); /* Ne cree un nouvel identifiant de cycle que si cq==0 
				        (q n'etait pas deja ds un cycle) */
    }
    else {
      /* qkind == 0 */
      if ((cq = is_cyclic (q)) == 0) {
	/* Ici, p et q ne sont pas ds le meme cycle et q est calcule */
#if EBUG
	if (*((p2c [q]) ? (c2kind + p2c [q]) : (p2kind + q)) != -1)
	  sxtrap (ME, "is_cyclic");
#endif
	if (appel_a_fermer) {
	  if ((linefp = F [p]) != (linefq = F [q]))
	    sxba_or (linefp, linefq);

	  SXBA_1_bit (linefp, q);
	}

	continue;
      }

      /* Ici, q est ds un cycle de representant cq */
    }

    fill_p2c (q, cq);

    if (appel_a_fermer) SXBA_1_bit (F [c2p [cq]], q);

    if (cp != cq)
      /* C [cq] et linep ne reperent pas le meme ensemble */
      sxba_or (C [cq], linep);

    /* SXBA_0_bit (C [cq], q); On coupe le cycle (inutile avec le sxba_scan_reset de linep) */

    if (*ppkind == 1) return cq; /* On continue a depiler le cycle */

    /* Fin du cycle, on repart sur c */
    q = -1; /* cycle sur p, on recommence avec le cycle */
    cp = cq;
    *(ppkind = c2kind + cp) = 1;
    linep = C [cp];
  }

  *ppkind = -1;

  return 0;
}

/* Cette fonction retourne F, les cycles de la relation M et remplit nb_de_cycles le nombre de cycles */
/* M [0..size-1, 0..size-1] */
/* Si cycle_nb > 0 F [0..cycle_nb-1, 0..size-1] */
/* F [i, j] <=> il y a un cycle passant par i et j */
/* Si appel_a_fermer est true, retourne ds F la fermeture transitive de M */
/* Attention si i et j sont ds un cycle, les lignes F[i] et F[j] sont partagees */
static SXBA*
R_cycle (SXBA *M, SXINT size, SXINT *nb_de_cycles)
{
  SXINT i, nb;
  SXINT *c2nc;

  if (size <= 0) return NULL;

  appel_a_fermer = nb_de_cycles == NULL;

  R_deja_traite = sxba_calloc (size);
  p2kind = (SXINT*) sxcalloc (size, sizeof (SXINT));

  if (appel_a_fermer)
    F = sxbm_calloc (size, size);

  C = sxbm_calloc (size, size);
  c2p = (SXINT*) sxcalloc (size, sizeof (SXINT));
  p2c = (SXINT*) sxcalloc (size, sizeof (SXINT));
  used_p2c = sxba_calloc (size);
  used_c = sxba_calloc (size);
  c2kind = (SXINT*) sxcalloc (size, sizeof (SXINT));
  
  R = M;
  R_is_cyclic = false;

  i = -1;

  while ((i = sxba_0_lrscan (R_deja_traite, i)) >= 0) {
    is_cyclic (i);
  }

  sxfree (R_deja_traite), R_deja_traite = NULL;
  sxfree (p2kind), p2kind = NULL;

  sxbm_free (C), C = NULL;
  sxfree (c2kind), c2kind = NULL;

  if (!appel_a_fermer) {
    if (R_is_cyclic) {
      c2nc = (SXINT*) sxalloc (size, sizeof (SXINT));

      i = -1;
      nb = 0;

      while ((i = sxba_scan (used_c, i)) >= 0) {
	c2nc [i] = nb++;
      }

      *nb_de_cycles = nb;
      F = sxbm_calloc (nb, size);

      i = -1;

      while ((i = sxba_scan (used_p2c, i)) >= 0) {
	SXBA_1_bit (F [c2nc [p2c [i]]], i);
      }

      sxfree (c2nc);
    }
    else {
      F = NULL;
      *nb_de_cycles = 0;
    }
  }

  sxfree (c2p), c2p = NULL;
  sxfree (p2c), p2c = NULL;
  sxfree (used_p2c), used_p2c = NULL;
  sxfree (used_c), used_c = NULL;

  return F;
}

static void
fill_CRED (SXINT size, XxYxZ_header automaton)
{
  SXINT p, q, X, triple;
  SXBA linep, *fill_CRED_R;

  fill_CRED_R = sxbm_calloc (size+1, size+1);
  
  for (p = 1; p <= size; p++) {
    linep = fill_CRED_R [p];

    XxYxZ_Xforeach (automaton, p, triple) {
      X = XxYxZ_Y (automaton, triple);

      if (X >= 0) {
	q = XxYxZ_Z (automaton, triple);
	SXBA_1_bit (linep, q);
      }
    }
  }

  CRED = R_cycle (fill_CRED_R, size+1, &R_nb_de_cycles);

  sxbm_free (fill_CRED_R);
}


static void
fill_RED (SXINT size, XxYxZ_header automaton)
{
  SXINT p, q, X, triple;
  SXBA linep, *fill_RED_R;

  fill_RED_R = sxbm_calloc (size+1, size+1);
  
  for (p = 1; p <= size; p++) {
    linep = fill_RED_R [p];

    XxYxZ_Xforeach (automaton, p, triple) {
      X = XxYxZ_Y (automaton, triple);

      if (X >= 0) {
	q = XxYxZ_Z (automaton, triple);
	SXBA_1_bit (linep, q);
      }
    }
  }

  RED = R_cycle (fill_RED_R, size+1, (SXINT*)NULL);

  sxbm_free (fill_RED_R);
}


static void
dfsa_states_oflw (SXINT old_size, SXINT new_size)
{
  sxuse(old_size); /* pour faire taire gcc -Wunused */
  dfsa_final_state_set = sxba_resize (dfsa_final_state_set, new_size+1);
}

static SXBA ndfsa_state_set;
static XH_header dfsa_states;

static void
fill_dfsa_states (void)
{
  SXINT q = 0;

  while ((q = sxba_scan_reset (ndfsa_state_set, q)) > 0)
    XH_push (dfsa_states, q);
}


/* L'automate non deterministe est transforme en un automate non deterministe sans transition nulles */
/* Les transitions nulles sont issues de useless_clause_set */
static void
ndfsa2nfndfsa (void)
{
  SXINT             nfstate, state, p, X, nfq, q, triple, foo, nfndfsa_state_nb, nfndfsa_init_state;
  bool         is_first;
  SXINT             *nfstate2state, *state2nfstate;
  SXBA            closure, nfndfsa_final_state_set;
  XxYxZ_header    nfndfsa_hd;

  sxinitialise(closure); /* pour faire taire gcc -Wuninitialized */
  XxYxZ_alloc (&nfndfsa_hd, "ndfsa_hd", XxYxZ_top (ndfsa_hd), 1, ndfsa_assoc_list, NULL, statistics);
  nfndfsa_final_state_set = sxba_calloc (ndfsa_state_nb+1);
  nfstate2state = (SXINT*) sxalloc (ndfsa_state_nb+1, sizeof (SXINT));
  state2nfstate = (SXINT*) sxcalloc (ndfsa_state_nb+1, sizeof (SXINT));
  
  nfstate2state [nfndfsa_state_nb = nfndfsa_init_state = 1] = ndfsa_init_state;
  state2nfstate [ndfsa_init_state] = nfndfsa_init_state;

  for (nfstate = 1; nfstate <= nfndfsa_state_nb; nfstate++) {
    state = nfstate2state [nfstate];
    is_first = true;
    p = state;

    do {
      if (SXBA_bit_is_set (ndfsa_final_state_set, p))
	SXBA_1_bit (nfndfsa_final_state_set, nfstate);

      /* Les transitions sortantes non vides de p sont des transitions sortantes de state */
      XxYxZ_Xforeach (ndfsa_hd, p, triple) {
	X = XxYxZ_Y (ndfsa_hd, triple);
	q = XxYxZ_Z (ndfsa_hd, triple);

	if (X != 0) {
	  /* transition terminale ou prod non nulle */
	  if ((nfq = state2nfstate [q]) == 0) {
	    nfq = state2nfstate [q] = ++nfndfsa_state_nb;
	    nfstate2state [nfq] = q;
	  }

	  XxYxZ_set (&nfndfsa_hd, nfstate, X, nfq, &foo);
	}
      }

      if (is_first) {
	is_first = false;
	closure = NULL_RED [p];
	p = 0;
      }
    } while ((p = sxba_scan (closure, p)) > 0);
  }

  XxYxZ_free (&ndfsa_hd), ndfsa_hd = nfndfsa_hd;
  sxfree (ndfsa_final_state_set), ndfsa_final_state_set = nfndfsa_final_state_set;
  ndfsa_init_state = nfndfsa_init_state;
  ndfsa_state_nb = nfndfsa_state_nb;

  sxfree (nfstate2state);
  sxfree (state2nfstate);
}




static void
ndfsa2dfsa (void)
{
  /* On fabrique l'automate fini deterministe dfsa_hd a partir de l'automate non-deterministe ndfsa_hd */
  /* Aucune transition n'est "vide" */
  SXINT pi, cur, bot, top, q, triple, X, t, s, prod, pis, foo;
  SXINT *dfsa_state_stack;
  SXBA wndfsa_state_set, xprod_set;

  xprod_set = sxba_calloc (last_prod+XH_top(mtrans_hd)+1);

  XH_alloc (&dfsa_states, "dfsa_states", ndfsa_state_nb+1, 1, 5, dfsa_states_oflw, statistics);
  dfsa_final_state_set = sxba_calloc (XH_size (dfsa_states)+1);
  S1_alloc (dfsa_state_stack, ndfsa_state_nb+1);
  wndfsa_state_set = sxba_calloc (ndfsa_state_nb+1);
  ndfsa_state_set = sxba_calloc (ndfsa_state_nb+1);

  XxYxZ_alloc (&dfsa_hd, "dfsa_hd", XH_size (dfsa_states)*2, 1, dfsa_assoc_list, NULL, statistics);

  /* Calcul de l'etat initial de dfsa */
  XH_push (dfsa_states, ndfsa_init_state);
  XH_set (&dfsa_states, &dfsa_init_state);
  dfsa_state_nb = 1;

  S1_push (dfsa_state_stack, dfsa_init_state);

  while (!S1_is_empty (dfsa_state_stack)) {
    pi = S1_pop (dfsa_state_stack);
    foo = 0;

    bot = XH_X (dfsa_states, pi);
    top = XH_X (dfsa_states, pi+1);

    for (cur = bot; cur < top; cur++) {
      q = XH_list_elem (dfsa_states, cur);

      if (SXBA_bit_is_set (ndfsa_final_state_set, q))
	SXBA_1_bit (dfsa_final_state_set, pi);

      XxYxZ_Xforeach (ndfsa_hd, q, triple) {
	X = XxYxZ_Y (ndfsa_hd, triple);

	if (X < 0) {
	  X = -X;
	  SXBA_1_bit (t_set, X);
	}
	else {
	  SXBA_1_bit (xprod_set, X);
	}
      }
    }

    t = 0;

    while ((t = sxba_scan_reset (t_set, t)) > 0) {
      for (cur = bot; cur < top; cur++) {
	q = XH_list_elem (dfsa_states, cur);

	XxYxZ_XYforeach (ndfsa_hd, q, -t, triple) {
	  s = XxYxZ_Z (ndfsa_hd, triple);
	  SXBA_1_bit (ndfsa_state_set, s);
	}
      }

      fill_dfsa_states ();

      if (!XH_set (&dfsa_states, &pis)) {
	S1_push (dfsa_state_stack, pis);
	dfsa_state_nb = pis;
      }

      XxYxZ_set (&dfsa_hd, pi, -t, pis, &foo);
    }

    prod = -1;

    while ((prod = sxba_scan_reset (xprod_set, prod)) >= 0) {
      for (cur = bot; cur < top; cur++) {
	q = XH_list_elem (dfsa_states, cur);

	XxYxZ_XYforeach (ndfsa_hd, q, prod, triple) {
	  s = XxYxZ_Z (ndfsa_hd, triple);
	  SXBA_1_bit (ndfsa_state_set, s);
	}
      }

      fill_dfsa_states ();

      if (!XH_set (&dfsa_states, &pis)) {
	S1_push (dfsa_state_stack, pis);
	dfsa_state_nb = pis;
      }

      XxYxZ_set (&dfsa_hd, pi, prod, pis, &foo);
    }

#if EBUG
    if (!SXBA_bit_is_set (dfsa_final_state_set, pi) && foo == 0)
      /* etat non final sans transition */
      sxtrap (ME, "ndfsa2dfsa");
#endif
  }

  XH_free (&dfsa_states);
  S1_free (dfsa_state_stack);
  sxfree (ndfsa_state_set);
  sxfree (wndfsa_state_set);

  sxfree (xprod_set);

  XxYxZ_free (&ndfsa_hd);
  sxfree (ndfsa_final_state_set), ndfsa_final_state_set = NULL;
}

static SXINT *pt2p2, *pred2p2;

static bool
k_equiv (void)
{
  /* Les k-1 equivalences sont ds marked et dfsa2eq */
  /* remplit les k equivalences ds marked et dfsa2eq */
  /* Retourne true si k \neq k-1 */
  SXINT p, triple, X, p2, q, q2, prevp, prevq, new_repr, new_prevq, nextp, nextq;
  bool has_changed = false, is_first, p_kequiv;

  sxinitialise(new_prevq); /* pour faire taire gcc -Wuninitialized */
  nextp = marked [prevp = 0].glbl;

  while (nextp > 0) {
    nextp = marked [p = nextp].glbl;

    new_repr = 0;
    p_kequiv = false;

    do {
      XxYxZ_Xforeach (dfsa_hd, p, triple) {
	X = XxYxZ_Y (dfsa_hd, triple);
	p2 = dfsa2eq [XxYxZ_Z (dfsa_hd, triple)];

	if (X < 0)
	  pt2p2 [-X] = p2;
	else
	  pred2p2 [X] = p2;
      }

      prevq = q = p;
      nextq = marked [q].local;
      marked [q].glbl = marked [q].local = 0;

      while ((q = nextq) > 0) {
	nextq = marked [q].local;
	marked [q].glbl = marked [q].local = 0;

	/* On regarde si p et q sont k-equivalents */
	/* On commence par regarder s'ils ont les memes transitions */
	is_first = true;

	XxYxZ_Xforeach (dfsa_hd, q, triple) {
	  is_first = false;
	  X = XxYxZ_Y (dfsa_hd, triple);
	  q2 = dfsa2eq [XxYxZ_Z (dfsa_hd, triple)];

	  if ((X < 0 && pt2p2 [-X] != q2) ||
	      (X >= 0 && pred2p2 [X] != q2))
	    break;
	}

	if (triple == 0 && !is_first) {
	  /* p kequiv q */
	  /* dfsa2eq [q] = p; deja fait */
	  marked [prevq].local = q;
	  prevq = q;
	  p_kequiv = true;
	}
	else {
	  if (new_repr) {
	    marked [new_prevq].local = q;
	    new_prevq = q;
	  }
	  else  {
	    new_prevq = new_repr = q;
	  }
	}
      }

      if (p_kequiv) {
	marked [prevp].glbl = p;
	prevp = p;
      }

      if (new_repr) {
	/* Il y a des k-1_equiv qui ne sont pas k_equiv */
	/* la liste est ds new_repr */
	has_changed = true;

	q = new_repr;

	do {
	  /* mise a jour obligatoirement differee */
	  dfsa2eq [q] = new_repr;
	} while ((q = marked [q].local));

	if (marked [new_repr].local != 0) {
	  /* la liste est multiple */
	  p = new_repr;
	  new_repr = 0; /* On reboucle */
	}
      }
      else
	new_repr = 1;
    } while (new_repr == 0);
  }

  return has_changed;
}

/* Minimisation du dfsa ds fsa */
static void
dfsa2min_fsa (void)
{
  SXINT p, triple, X, i, pt, dfsa2min_fsa_pred, q, p2, q2, foo, prevp, prevq;
  bool p_0equiv;
  SXINT *dfsa_state2t_trans, *dfsa_state2red_trans, *dfsa2fsa;
  SXBA wset, xprod_set;
  XH_header dfsa_trans;

  xprod_set = sxba_calloc (last_prod+XH_top(mtrans_hd)+1);

  marked = (struct k_equiv*) sxalloc (dfsa_state_nb+1, sizeof (struct k_equiv));
  marked [0].glbl = marked [0].local = 0;
  dfsa2eq = (SXINT*) sxalloc (dfsa_state_nb+1, sizeof (SXINT));
  dfsa2eq [0] = 0;

  XH_alloc (&dfsa_trans, "dfsa_trans", dfsa_state_nb+1, 1, (XxYxZ_top (dfsa_hd)/dfsa_state_nb)+1, NULL, statistics);
  dfsa_state2t_trans = (SXINT*) sxalloc (dfsa_state_nb+1, sizeof (SXINT));
  dfsa_state2red_trans = (SXINT*) sxalloc (dfsa_state_nb+1, sizeof (SXINT));
  wset = sxba_calloc (dfsa_state_nb+1);

  /* Qq pre-calculs */
  /* Pour chaque etat du dfsa, on stocke ds un XH les ensembles t_set et xprod_set de ses transitions */
  for (p = 1; p <= dfsa_state_nb; p++) {
    XxYxZ_Xforeach (dfsa_hd, p, triple) {
      X = XxYxZ_Y (dfsa_hd, triple);

      if (X < 0) {
	X = -X;
#if EBUG
	if (!SXBA_bit_is_reset_set (t_set, X))
	  /* L'automate n'est pas deterministe!! */
	  sxtrap (ME, "dfsa2min_fsa");
#else
	SXBA_1_bit (t_set, X);
#endif
      }
      else {
#if EBUG
	if (!SXBA_bit_is_reset_set (xprod_set, X))
	  /* L'automate n'est pas deterministe!! */
	  sxtrap (ME, "dfsa2min_fsa");
#else
	SXBA_1_bit (xprod_set, X);
#endif
      }
    }

    X = 0;

    while ((X = sxba_scan_reset (t_set, X)) > 0) {
      XH_push (dfsa_trans, X);
    }

    XH_set (&dfsa_trans, dfsa_state2t_trans+p);

    X = -1;

    while ((X = sxba_scan_reset (xprod_set, X)) >= 0) {
      XH_push (dfsa_trans, X);
    }

    XH_set (&dfsa_trans, dfsa_state2red_trans+p);
  }

  sxfree (xprod_set);

  /* initialisation de la 0-equivalence ds marked */
  i = 2;
  prevp = 0;

  do {
    sxba_copy (wset, dfsa_final_state_set);

    if (i == 1)
      sxba_not (wset);

    p = 0;

    while ((p = sxba_scan_reset (wset, p)) > 0) {
      p_0equiv = false;
      pt = dfsa_state2t_trans [p];
      dfsa2min_fsa_pred = dfsa_state2red_trans [p];

      dfsa2eq [p] = p;
      marked [p].glbl = marked [p].local = 0;
      prevq = q = p;

      while ((q = sxba_scan (wset, q)) > 0) {
	if (pt == dfsa_state2t_trans [q] && dfsa2min_fsa_pred == dfsa_state2red_trans [q]) {
	  /* p 0equiv q */
	  SXBA_0_bit (wset, q);
	  dfsa2eq [q] = p;
	  marked [q].glbl = marked [q].local = 0;
	  marked [prevq].local = q;
	  prevq = q;
	  p_0equiv = true;
	}
      }

      if (p_0equiv) {
	marked [prevp].glbl = p;
	prevp = p;
      }
    }
  } while (--i);

  XH_free (&dfsa_trans);
  sxfree (dfsa_state2t_trans);
  sxfree (dfsa_state2red_trans);
  sxfree (wset);

  pt2p2 =  (SXINT*) sxalloc (bnf.tmax+2, sizeof (SXINT));
  pred2p2 = (SXINT*) sxalloc (last_prod+XH_top(mtrans_hd)+1, sizeof (SXINT));

  while (k_equiv ()); /* Les equivalences sont dans dfsa2eq*/

  sxfree (pt2p2), pt2p2 = NULL;
  sxfree (pred2p2), pred2p2 = NULL;
  sxfree (marked), marked = NULL;

  dfsa2fsa = (SXINT*) sxcalloc (dfsa_state_nb+1, sizeof (SXINT));

  /* On remplit fsa_hd, fsa_final_state_set, fsa_state_nb et fsa_init_state */
  XxYxZ_alloc (&fsa_hd, "fsa_hd", XxYxZ_top (dfsa_hd)/2+1, 1, fsa_assoc_list, NULL, statistics);
  fsa_final_state_set = sxba_calloc (dfsa_state_nb+1);

  fsa_init_state = dfsa_init_state;
  fsa_state_nb = 0;
  fsa_has_reduction = false;

  for (p = 1; p <= dfsa_state_nb; p++) {
    if (dfsa2eq [p] == p) {
      if ((p2 = dfsa2fsa [p]) == 0)
	p2 = dfsa2fsa [p] = ++fsa_state_nb;

      if (SXBA_bit_is_set (dfsa_final_state_set, p))
	SXBA_1_bit (fsa_final_state_set, p2);

      XxYxZ_Xforeach (dfsa_hd, p, triple) {
	X = XxYxZ_Y (dfsa_hd, triple);
	if (X >= 0) fsa_has_reduction = true;
	q = dfsa2eq [XxYxZ_Z (dfsa_hd, triple)];

	if ((q2 = dfsa2fsa [q]) == 0)
	  q2 = dfsa2fsa [q] = ++fsa_state_nb;

	XxYxZ_set (&fsa_hd, p2, X, q2, &foo);
      }
    }
  }

  sxfree (dfsa2eq);
  sxfree (dfsa2fsa);
}


/* Elimine les cycles de reductions du fsa, que ces red produisent ou non le vide */
static bool
cycle_erasing (SXINT *state_nb, SXINT *init_state, SXBA *final_state_set, XxYxZ_header *automaton, SXINT *assoc_list)
{
  /* Il faut calculer le nb de cycles et la taille cumulee des cycles pour connaitre
     le nouveau nb d'etat de ncfsa */
  SXINT     c, p, q, size, p2, p3, ncfsa_init_state, ncfsa_state_nb, foo, triple, r, q2, X, prod, pushed_nb;
  SXINT     cycle_sizes;
  SXINT     *fsa2ncfsa, *fsa2eq, *cycle_erasing_p2c;
  XxYxZ_header    ncfsa_hd;
  SXBA    linep, cycle_set, cycle, ncfsa_final_state_set, cycle_erasing_prod_set, prod2_set, cycle_erasing_t_set;

  fill_CRED (*state_nb, *automaton);

  /* L'automate non cyclique est deterministe a priori */
  is_deterministic = true;

  if (R_nb_de_cycles) {
    /* On refabrique un fsa sans cycle sur les reduces */
    cycle_set = sxba_calloc (*state_nb+1);
    fsa2eq = (SXINT*) sxcalloc (*state_nb+1, sizeof (SXINT));
    cycle_erasing_p2c = (SXINT*) sxalloc (*state_nb+1, sizeof (SXINT));
    cycle_sizes = 0;

    for (c = 0; c < R_nb_de_cycles; c++) {
      linep = CRED [c];
      cycle_sizes += sxba_cardinal (linep);
      p = q = sxba_scan (linep, -1);
      cycle_erasing_p2c [p] = c;
      SXBA_1_bit (cycle_set, p); /* p est le representant du cycle */

      do {
	fsa2eq [q] = p;
      } while ((q = sxba_scan (linep, q)) >= 0);
    }

    for (p = 1; p <= *state_nb; p++) {
      if (fsa2eq [p] == 0)
	fsa2eq [p] = p;
    }

    /* nb d'etats prevus de ncfsa */
    size = *state_nb - cycle_sizes + 2*R_nb_de_cycles;

    fsa2ncfsa = (SXINT*) sxcalloc (*state_nb+1, sizeof (SXINT));
    XxYxZ_alloc (&ncfsa_hd, automaton->name, XxYxZ_top (*automaton), 1, assoc_list, NULL, statistics);
    ncfsa_final_state_set = sxba_calloc (size+1);
    cycle_erasing_prod_set = sxba_calloc (last_prod+1);

    prod2_set = sxba_calloc (last_prod+1);
    cycle_erasing_t_set = sxba_calloc (bnf.tmax+3);

    ncfsa_init_state = *init_state;
    ncfsa_state_nb = 0;

    for (p = 1; p <= *state_nb; p++) {
      if (fsa2eq [p] == p) {
	if ((p2 = fsa2ncfsa [p]) == 0)
	  p2 = fsa2ncfsa [p] = ++ncfsa_state_nb;

	if (SXBA_bit_is_set (cycle_set, p)) {
	  /* Il y a un cycle autour de p et p est le representant */
	  /* On cree un nouvel etat */

	  /* On cree une transition sur l'ensemble du cycle entre p2 et p3 */
	  p3 = ++ncfsa_state_nb;

	  /* on regroupe les transitions du cycle qui forment une transition entre p2 et p3
	     Toutes les transitions entrant ds le cycle seront des transitions entrantes ds p2.
	     Elles sont traitees par le cas general
	     Toutes les transitions sortant du cycle seront des transitions sortant de p3.
	     Elles sont traitees ici */
	  cycle = CRED [cycle_erasing_p2c [p]];

	  q = p;

	  do {
	    if (SXBA_bit_is_set (*final_state_set, q)) {
	      /* si q est final ds fsa, p3 est final ds ncfsa */
	      SXBA_1_bit (ncfsa_final_state_set, p3);
	    }

	    XxYxZ_Xforeach (*automaton, q, triple) {
	      X = XxYxZ_Y (*automaton, triple);
	      r = XxYxZ_Z (*automaton, triple);
	      
	      if (X >= 0 && SXBA_bit_is_set (cycle, r)) {
		/* C'est une transition du cycle */
#if EBUG
		if (X > last_prod)
		  sxtrap (ME, "cycle_erasing");
#endif

		SXBA_1_bit (cycle_erasing_prod_set, X);
	      }
	      else {
		/* r n'est pas ds le cycle, c'est une transition out pour p3 */
		r = fsa2eq [r];

		if ((q2 = fsa2ncfsa [r]) == 0) {
		  /* nouvel etat */
		  q2 = fsa2ncfsa [r] = ++ncfsa_state_nb;
		}

		XxYxZ_set (&ncfsa_hd, p3, X, q2, &foo);

		if (X < 0) {
		  X = -X;

		  if (!SXBA_bit_is_reset_set (cycle_erasing_t_set, X))
		    is_deterministic = false;
		}
		else {
#if EBUG
		  if (X > last_prod)
		    sxtrap (ME, "cycle_erasing");
#endif

		  if (!SXBA_bit_is_reset_set (prod2_set, X))
		    is_deterministic = false;
		}
	      }
	    }
	  } while ((q = sxba_scan (cycle, q)) > 0);

	  if (!is_deterministic) {
	    sxba_empty (cycle_erasing_t_set);
	    sxba_empty (prod2_set);
	  }

	  prod = -1;
	  pushed_nb = 0;

	  while ((prod = sxba_scan_reset (cycle_erasing_prod_set, prod)) >= 0) {
	    pushed_nb++;
	    XH_push (mtrans_hd, prod);
	  }

	  if (pushed_nb > 1) {
	    XH_set (&mtrans_hd, &X);
	    XxYxZ_set (&ncfsa_hd, p2, last_prod+X, p3, &foo);
	  }
	  else {
	    if (pushed_nb == 1) {
	      XH_pop (mtrans_hd, prod);
	      XxYxZ_set (&ncfsa_hd, p2, prod, p3, &foo);
	    }
#if EBUG
	    else
	      /* pushed_nb == 0 => p3 inaccessible */
	      sxtrap (ME, "cycle_erasing");
#endif
	  }
	}
	else {
	  /* p n'est pas implique' ds un cycle */
	  if (SXBA_bit_is_set (*final_state_set, p))
	    SXBA_1_bit (ncfsa_final_state_set, p2);

	  XxYxZ_Xforeach (*automaton, p, triple) {
	    X = XxYxZ_Y (*automaton, triple);
	    q = fsa2eq [XxYxZ_Z (*automaton, triple)];

	    if ((q2 = fsa2ncfsa [q]) == 0)
	      q2 = fsa2ncfsa [q] = ++ncfsa_state_nb;

	    XxYxZ_set (&ncfsa_hd, p2, X, q2, &foo);
	  }
	}
      }
    }

#if EBUG
    if (sxverbosep) {
      fprintf (sxtty, " %ld cycles over %ld states ", R_nb_de_cycles, cycle_sizes);
      sxttycol1p = false;
    }

    if (ncfsa_state_nb != size)
      sxtrap (ME, "cycle_erasing");
#endif

    sxfree (fsa2ncfsa);
    sxfree (cycle_erasing_prod_set);
    sxfree (prod2_set);
    sxfree (cycle_erasing_t_set);

    sxfree (cycle_set);
    sxfree (fsa2eq);
    sxfree (cycle_erasing_p2c);

    /* On "ecrase" fsa */
    XxYxZ_free (automaton), *automaton = ncfsa_hd;
    sxfree (*final_state_set), *final_state_set = ncfsa_final_state_set;
    *state_nb = ncfsa_state_nb;
    *init_state = ncfsa_init_state;
  }

  if (CRED) sxbm_free (CRED), CRED = NULL;

  return R_nb_de_cycles != 0;
}




/* Les reductions multiples entre p et q sont factorisees */
/* ainsi que les chaines de reductions */
/* A FAIRE mtrans_hd pourra contenir des transitions multiples inutilisees, il est donc inutile
   de les sortir */
static void
reduce_fsa (void)
{
  SXINT p, X, q, sq, triple, foo, prod, trans_nb, cur, bot, top, pushed_nb, size = 0;
  SXINT last_xprod = last_prod+XH_top(mtrans_hd);
  SXINT *q2trans_nb;
  SXBA state_set, state_set2, xprod_set, *mtrans2xprod_set, *S_RED = NULL, lower_set, upper_set;
  XxYxZ_header    ncfsa_hd;
  bool is_multiple, must_shrink = false, is_final;

  sxinitialise(sq); /* pour faire taire gcc -Wuninitialized */
  sxinitialise(upper_set); /* pour faire taire gcc -Wuninitialized */
  sxinitialise(lower_set); /* pour faire taire gcc -Wuninitialized */
  sxinitialise (mtrans2xprod_set); /* pour faire taire gcc -Wuninitialized */
  /* La seule chose qui change, sont les transitions */
  XxYxZ_alloc (&ncfsa_hd, "fsa_hd", XxYxZ_top (fsa_hd), 1, fsa_assoc_list, NULL, statistics);
  q2trans_nb = (SXINT*) sxalloc (fsa_state_nb+1, sizeof (SXINT));
  state_set = sxba_calloc (fsa_state_nb+1);
  state_set2 = sxba_calloc (fsa_state_nb+1);

  for (p = 1; p <= fsa_state_nb; p++) {
    trans_nb = 0;

    XxYxZ_Xforeach (fsa_hd, p, triple) {
      X = XxYxZ_Y (fsa_hd, triple);

      if (X >= 0) {
	q = XxYxZ_Z (fsa_hd, triple);

	if (SXBA_bit_is_reset_set (state_set, q)) {
	  q2trans_nb [q] = 0;
	}
	else {
	  /* q est multiple */
	  if (SXBA_bit_is_reset_set (state_set2, q)) {
	    q2trans_nb [q] = trans_nb++;
	  }
	}
      }
    }

    if (trans_nb) {
      if (trans_nb > size) {
	if (size)
	  mtrans2xprod_set = sxbm_resize (mtrans2xprod_set, size, trans_nb, last_xprod+1);
	else
	  mtrans2xprod_set = sxbm_calloc (trans_nb, last_xprod+1);

	size = trans_nb;
      }
    }

    is_multiple = false;
    trans_nb = 0;

    XxYxZ_Xforeach (fsa_hd, p, triple) {
      X = XxYxZ_Y (fsa_hd, triple);
      q = XxYxZ_Z (fsa_hd, triple);

      if (X < 0) {
	XxYxZ_set (&ncfsa_hd, p, X, q, &foo);
	trans_nb = -1;
      }
      else {
	SXBA_0_bit (state_set, q);

	if (SXBA_bit_is_set (state_set2, q)) {
	  is_multiple = true;
	  xprod_set = mtrans2xprod_set [q2trans_nb [q]];
	  SXBA_1_bit (xprod_set, X);
	}
	else {
	  if (trans_nb >= 0) trans_nb++, sq = q;
	  XxYxZ_set (&ncfsa_hd, p, X, q, &foo);
	}
      }
    }

    if (is_multiple) {
      q = 0;

      while ((q = sxba_scan_reset (state_set2, q)) > 0) {
	/* On cree une trans multiple */
	xprod_set = mtrans2xprod_set [q2trans_nb [q]];

	prod = -1;
	pushed_nb = 0;

	while ((prod = sxba_scan_reset (xprod_set, prod)) >= 0) {
#if EBUG
	  if (prod > last_prod)
	    sxtrap (ME, "reduce_fsa");
#endif
	  pushed_nb++;
	  XH_push (mtrans_hd, prod);
	}

	if (pushed_nb > 1) {
	  XH_set (&mtrans_hd, &X);
	  XxYxZ_set (&ncfsa_hd, p, last_prod+X, q, &triple);
	}
	else {
	  if (pushed_nb == 1) {
	    XH_pop (mtrans_hd, prod);
	    XxYxZ_set (&ncfsa_hd, p, prod, q, &triple);
	  }
	}

	if (trans_nb >= 0) trans_nb++, sq = q;
      }
    }

    if (trans_nb == 1) {
      /* Il y a une seule transition de p a sq qui est une transition reduce, on la note */
      if (S_RED == NULL) {
	S_RED = sxbm_calloc (fsa_state_nb+1, fsa_state_nb+1);
	lower_set = sxba_calloc (fsa_state_nb+1);
	upper_set = sxba_calloc (fsa_state_nb+1);
      }

      SXBA_1_bit (S_RED [p], sq);

      if (!must_shrink) {
	if (SXBA_bit_is_set (upper_set, p) || SXBA_bit_is_set (lower_set, sq))
	  /* Il existe au moins un chemin de longueur 2 */
	  must_shrink = true;

	SXBA_1_bit (lower_set, p);
	SXBA_1_bit (upper_set, sq);
      }
    }
  }

  /* On "ecrase" fsa */
  XxYxZ_free (&fsa_hd), fsa_hd = ncfsa_hd;
  if (size) sxbm_free (mtrans2xprod_set);
  sxfree (q2trans_nb);
  sxfree (state_set);
  sxfree (state_set2);

  if (S_RED) {
    sxfree (lower_set);
    sxfree (upper_set);
  }

  if (must_shrink) {
    /* Il y a des chaines de reductions, on les factorise */
    SXINT ncfsa_state_nb, ncfsa_init_state, ncp, ncq;
    XxYxZ_header reduce_fsa_ncfsa_hd;
    SXBA ncfsa_final_state_set, reduce_fsa_prod_set;
    SXINT *p2ncp, *ncp2p;

    XxYxZ_alloc (&reduce_fsa_ncfsa_hd, "fsa_hd", XxYxZ_top (fsa_hd), 1, fsa_assoc_list, NULL, statistics);
    ncfsa_final_state_set = sxba_calloc (fsa_state_nb+1);
    p2ncp = (SXINT*) sxcalloc (fsa_state_nb+1, sizeof (SXINT));
    ncp2p = (SXINT*) sxalloc (fsa_state_nb+1, sizeof (SXINT));
    reduce_fsa_prod_set = sxba_calloc (last_prod+1);

    p2ncp [fsa_init_state] = ncfsa_state_nb = ncfsa_init_state = 1;
    ncp2p [1] = fsa_init_state;

    if (SXBA_bit_is_set (fsa_final_state_set, fsa_init_state))
      SXBA_1_bit (ncfsa_final_state_set, ncfsa_init_state);
    
    for (ncp = 1; ncp <= ncfsa_state_nb; ncp++) {
      p = ncp2p [ncp];
      q = sxba_scan (S_RED [p], 0);

      if (q != -1  && sxba_scan (S_RED [q], 0) != -1) {
	/* Les transitions depuis p doivent etre factorisees */
	/* On cherche le bout de la chaine en cumulant les reduces (attention
	   elles peuvent deja etre multiples) */
	is_final = false;

	do {
	  if (SXBA_bit_is_set (fsa_final_state_set, p))
	    is_final = true;

	  XxYxZ_Xforeach (fsa_hd, p, triple) {
	    q = XxYxZ_Z (fsa_hd, triple);

	    X = XxYxZ_Y (fsa_hd, triple);

#if EBUG
	    if (X < 0)
	      sxtrap (ME, "reduce_fsa");
#endif

	    if (X <= last_prod)
	      SXBA_1_bit (reduce_fsa_prod_set, X);
	    else {
	      X -= last_prod;

	      bot = XH_X (mtrans_hd, X);
	      top = XH_X (mtrans_hd, X+1);

	      for (cur = bot; cur < top; cur++) {
		prod = XH_list_elem (mtrans_hd, cur);
		SXBA_1_bit (reduce_fsa_prod_set, prod);
	      }
	    }
	  }

	  p = q;
	} while (sxba_scan (S_RED [p], 0) > 0);
      
	if ((ncq = p2ncp [q]) == 0) {
	  ncq = p2ncp [q] = ++ncfsa_state_nb;
	  ncp2p [ncfsa_state_nb] = q;
	}

	if (is_final)
	  SXBA_1_bit (ncfsa_final_state_set, ncq);

	prod = -1;
	pushed_nb = 0; /* Ca peut etre une sequence de 0 */

	while ((prod = sxba_scan_reset (reduce_fsa_prod_set, prod)) >= 0) {
	  pushed_nb++;
	  XH_push (mtrans_hd, prod);
	}

	if (pushed_nb > 1) {
	  XH_set (&mtrans_hd, &X);
	  XxYxZ_set (&reduce_fsa_ncfsa_hd, ncp, last_prod+X, ncq, &foo);
	}
	else {
	  if (pushed_nb == 1) {
	    XH_pop (mtrans_hd, prod);
	    XxYxZ_set (&reduce_fsa_ncfsa_hd, ncp, prod, ncq, &foo);
	  }
	}
      }
      else {
	/* Recopie */
	if (SXBA_bit_is_set (fsa_final_state_set, p))
	  SXBA_1_bit (ncfsa_final_state_set, ncp);

	XxYxZ_Xforeach (fsa_hd, p, triple) {
	  q = XxYxZ_Z (fsa_hd, triple);
      
	  if ((ncq = p2ncp [q]) == 0) {
	    ncq = p2ncp [q] = ++ncfsa_state_nb;
	    ncp2p [ncfsa_state_nb] = q;
	  }

	  X = XxYxZ_Y (fsa_hd, triple);
	  XxYxZ_set (&reduce_fsa_ncfsa_hd, ncp, X, ncq, &foo);
	}
      }
    }

    XxYxZ_free (&fsa_hd), fsa_hd = reduce_fsa_ncfsa_hd;
    sxfree (fsa_final_state_set), fsa_final_state_set = ncfsa_final_state_set;
    fsa_state_nb = ncfsa_state_nb;
    fsa_init_state = ncfsa_init_state;
    sxfree (p2ncp);
    sxfree (ncp2p);
    sxfree (reduce_fsa_prod_set);
  }

  if (S_RED) sxbm_free (S_RED);
}


static void
fill_state2look_ahead_set (void)
{
  SXINT p, X, triple;
  SXBA linep;
  
  for (p = 1; p <= fsa_state_nb; p++) {
    linep = state2look_ahead_set [p];

    XxYxZ_Xforeach (fsa_hd, p, triple) {
      if ((X = -XxYxZ_Y (fsa_hd, triple)) > 0) 
	SXBA_1_bit (linep, X);
    }
  }
}

/* Calcule ds t_set, l'ensemble des transitions terminales accessibles depuis q */
/* fsa est non cyclique sur les transitions de reduction */
static void
look_ahead (SXINT p)
{
  SXINT q;
  SXBA linep;

  linep = RED [p];
  q = sxba_scan (linep, 0);

  do {
    sxba_or (t_set, state2look_ahead_set [p]);
    p = q;
    q = sxba_scan (linep, q);
  } while (p > 0);
}

#if 0

static bool has_lex;
static VARSTR varstr;


static void
gen_Lex (SXBA lex_set)
{
  SXINT symb;
  bool is_first = true;

  if ((symb = sxba_scan (lex_set, 0)) > 0) {
    print_nt (LEX_ic);
    fputs ("(", out_file);

    do {
      if (is_first)
	is_first = false;
      else
	fputs (" ", out_file);

      print_t (symb);
    } while ((symb = sxba_scan (lex_set, symb)) > 0);

    fputs (") ", out_file);
  }
}


static VARSTR
get_UBs (SXINT t, bool *has_UBs)
{
  SXINT clause, cur, bot, top;
  char string [16];
	  
  *has_UBs = false;

  if (t > last_clause) {
    /* trans multiple de clauses */
    bot = XH_X (mtrans_hd, t-last_clause);
    top = XH_X (mtrans_hd, t-last_clause+1);

    for (cur = bot; cur < top; cur++) {
      clause = XH_list_elem (mtrans_hd, cur);
    
      if (clause > 0) {
	*has_UBs = true;
	
	if (SXBA_bit_is_reset_set (clause_set, clause)) {
	  sprintf (string, "UB%ld(T0) ", clause);
	  varstr_catenate (varstr, string);
	}
      }
    }
      
    /* Prudence, on ajoute pas de &Lex sur les trans multiples */
  }
  else {
    if (t > 0) {
      *has_UBs = true;
	
      if (SXBA_bit_is_reset_set (clause_set, t)) {
	sprintf (string, "UB%ld(T0) ", t);
	varstr_catenate (varstr, string);
      }
    
      /* On ajoute des &Lex */
      if (SXBA_bit_is_set (clause_has_lex, t)) {
	has_lex = true;
	sxba_or (lex_set, clause2lex_set [t]);
      }
    }
  }

  return varstr;
}

static bool
single_trans (SXINT q, SXINT *X, SXINT *r)
{
  /* retourne true ssi il y a une unique transition depuis q (elle est mise ds X et r) */
  SXINT foo, t, s;
  bool is_first = true;

  XxYxZ_Xforeach (fsa_hd, q, foo) {
    if (!is_first) return false;

    is_first = false;
    t = XxYxZ_Y (fsa_hd, foo);
    s = XxYxZ_Z (fsa_hd, foo); 
  }

  if (is_first || t > last_clause)
    /* On ne factorize pas les cycles car ils peuvent etre trop gros! */
    return false;

  *X = t;
  *r = s;
  return true;
}


static void
gen_First (SXINT q)
{
  SXINT symb;
  bool is_first;

  look_ahead (q);

  /* Si t_set est "complet", on ne genere rien */
  symb = sxba_0_lrscan (t_set, 0);

  if (symb > 0) {
    /* non complet */
    fputs ("&First(", out_file);
    is_first = true;
    symb = 0;

    while ((symb = sxba_scan_reset (t_set, symb)) > 0) {
      if (is_first)
	is_first = false;
      else
	fputs (" ", out_file);

      print_t (symb);
    }

    fputs (", X) ", out_file);
  }
}

/* Generation de la RCG a partir de fsa_hd */
static void
fsa2rcg (void)
{
  SXINT clause, prdct, bot2, nt, param, p, triple, foo, r, t, q, red, symb, cur, bot, top;
  SXBA fsa_state_set, line;
  SXINT *state_stack;
  bool is_first, has_xtrans, has_null, has_UBs, T0;

  /* Generer une clause bidon pour avoir les memes codes de terminaux */
  fputs ("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n", out_file);
  fputs ("%% For terminal internal codes identity...\n", out_file);
  gen_false_clause (sxstrget (nt2ste [1]), "");

  fsa_clause = 1;

  /* Pour chaque clause c, on genere
     UBc () --> .
     qui permettra de noter la borne sup de son range. */
  fputs ("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n", out_file);
  fputs ("%% For clause upper bounds...\n", out_file);

  for (clause = 1; clause <= last_clause; clause++) {
    if (clause2identical [clause] == clause
	&& !SXBA_bit_is_set (false_clause_set, clause)
	&& !SXBA_bit_is_set (loop_clause_set, clause)
	&& (!useless_clause_set || !SXBA_bit_is_set (useless_clause_set, clause))) {
      prdct = XxY_X (rcg_clauses, clause) & lhs_prdct_filter;
      bot2 = XH_X (rcg_predicates, prdct);
      nt = XH_list_elem (rcg_predicates, bot2+1);

      fprintf (out_file, "%% %ld: ", clause);
      out_clause (clause);
      fsa_clause++;
      fprintf (out_file, "UB%ld() --> . %% %ld\n", clause, fsa_clause);
      /* On note le mapping pour le guide
      fsa_clause2clause [fsa_clause] = clause; */
    }
    else
      nt = 0;
    
    if (nt == 0 && !SXBA_bit_is_set (false_clause_set, clause)) {
      fprintf (out_file, "%% %ld: ", clause);
      out_clause_body (clause);
      fputs (" SKIPPED\n", out_file);
    }
  }

  fputs ("\n%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n", out_file);
  fputs ("%% ... and the FSA\n", out_file);
      
  fsa_clause++;
  fprintf (out_file, "%s(X) --> q%ld(X) . %% %ld\n", sxstrget (nt2ste [1]), fsa_init_state, fsa_clause);

  varstr = varstr_alloc (128);

  fsa_state_set = sxba_calloc (fsa_state_nb+1);
  state_stack = (SXINT*) sxalloc (fsa_state_nb+1, sizeof (SXINT)), state_stack [0] = 0;

  SXBA_1_bit (fsa_state_set, fsa_init_state);
  PUSH (state_stack, fsa_init_state);

  while (!IS_EMPTY (state_stack)) {
    fputs ("\n", out_file);
    p = POP (state_stack);

    XxYxZ_Xforeach (fsa_hd, p, triple) {
      q = XxYxZ_Z (fsa_hd, triple);
      fsa_clause++;
      has_lex = false;
      
      if (SXBA_bit_is_set (fsa_final_state_set, q)) {
	/* Etat final */
	fputs ("% That's all\n", out_file);
	fprintf (out_file, "q%ld() --> . %% %ld\n", p, fsa_clause);
      }
      else {
	t = XxYxZ_Y (fsa_hd, triple);

	if (t < 0) {
	  /* Transition terminale */
	  /* On genere qp(t X) --> qq(X) */
#if EBUG
	  if (t < -bnf.tmax)
	    sxtrap (ME, "fsa2rcg");
#endif

	  fprintf (out_file, "q%ld(", p);
	  print_t (-t);

	  while (single_trans (q, &t, &r)) {
	    /* On capture [T*] [red] */
	    q = r;

	    if (t < 0) {
#if EBUG
	      if (t < -bnf.tmax)
		sxtrap (ME, "fsa2rcg");
#endif
	      fputs (" ", out_file);
	      print_t (-t);
	    }
	    else {
	      varstr_raz (varstr), sxba_empty (clause_set), sxba_empty (lex_set);
	      varstr = get_UBs (t, &has_UBs);

	      while (single_trans (q, &t, &r)) {
		/* On capture [red*] */
		if (t < 0) break;
		
		q = r;
		varstr = get_UBs (t, &T0);

		if (T0)
		  has_UBs = true;
	      }

	      if (has_UBs)
		fputs (" T0 X) --> &StrLen(0, T0) ", out_file);
	      else
		fputs (" X) --> ", out_file);
	      
	      if (has_lex)
		gen_Lex (lex_set);

	      if (is_look_ahead)
		gen_First (q);

	      fprintf (out_file, "%sq%ld(X) . %% %ld\n", varstr_tostr (varstr), q, fsa_clause);

	      break;
	    }
	  }

	  if (t < 0)
	    fprintf (out_file, " X) --> q%ld(X) . %% %ld\n", q, fsa_clause);
		
	  if (SXBA_bit_is_reset_set (fsa_state_set, q))
	    PUSH (state_stack, q);
	}
	else {
	  /* reduction */
	  if (t > last_clause && process_single_transition) {
	    /* On genere individuellement p(T0 X) --> UBt(T0)  &Lex(...) q(X) . */
	    t -= last_clause;
	    bot = XH_X (mtrans_hd, t);
	    top = XH_X (mtrans_hd, t+1);
	    is_first = true;
	    has_xtrans = false;
	    has_null = false;

	    for (cur = bot; cur < top; cur++) {
	      clause = XH_list_elem (mtrans_hd, cur);
    
	      if (clause > 0 && SXBA_bit_is_set (clause_has_lex, clause)) {
		/* On individualise les clauses qui ont des &Lex */
		fprintf (out_file, "q%ld(T0 X) --> &StrLen(0, T0) UB%ld(T0) ", p, clause);
		gen_Lex (clause2lex_set [clause]);

		if (is_look_ahead)
		  gen_First (q);

		if (is_first)
		  is_first = false;
		else
		  fsa_clause++;

		fprintf (out_file, "q%ld(X) . %% %ld\n", q, fsa_clause);

		if (SXBA_bit_is_reset_set (fsa_state_set, q))
		  PUSH (state_stack, q);
	      }
	      else {
		if (clause == 0)
		  has_null = true;
		else
		  has_xtrans = true;
	      }
	    }

	    if (has_xtrans || has_null) {
	      /* Une clause unique pour le reste */
	      if (has_xtrans) {
		fprintf (out_file, "q%ld(T0 X) --> &StrLen(0, T0) ", p);

		for (cur = bot; cur < top; cur++) {
		  clause = XH_list_elem (mtrans_hd, cur);
    
		  if (clause > 0 && !SXBA_bit_is_set (clause_has_lex, clause)) {
		    fprintf (out_file, "UB%ld(T0) ", clause);
		  }
		}
	      }
	      else {
		fprintf (out_file, "q%ld(X) --> ", p);
	      }

	      if (is_look_ahead)
		gen_First (q);

	      fsa_clause++;
	      fprintf (out_file, "q%ld(X) . %% %ld\n", q, fsa_clause);
		    
	      if (SXBA_bit_is_reset_set (fsa_state_set, q))
		PUSH (state_stack, q);
	    }
	  }
	  else {
	    fprintf (out_file, "q%ld(", p);

	    varstr_raz (varstr), sxba_empty (clause_set), sxba_empty (lex_set);
	    varstr = get_UBs (t, &has_UBs);

	    if (has_UBs) {
	      /* transition sur clause gardee */
	      while (single_trans (q, &t, &r)) {
		/* On capture [red*] */
		if (t < 0) break;
		
		q = r;
		varstr = get_UBs (t, &T0);
	      }
	    }
	    else {
	      /* transition "vide" entre p et q */
	      is_first = true;

	      while (single_trans (q, &t, &r)) {
		/* On capture [T|red vide]* red [red|red vide]* */
		if (t < -bnf.tmax) break; /* prudence */
	      
		q = r;

		if (t < 0) {
		  fputs (" ", out_file);
		  print_t (-t);
		}
		else {
		  varstr = get_UBs (t, &has_UBs);

		  if (has_UBs) {
		    while (single_trans (q, &t, &r)) {
		      /* On capture [red*] */
		      if (t < 0) break;
		
		      q = r;
		      varstr = get_UBs (t, &T0);
		    }

		    break;
		  }
		}
	      }
	    }

	    if (has_UBs)
	      fputs (" T0 X) --> &StrLen(0, T0) ", out_file);
	    else
	      fputs (" X) --> ", out_file);
	      
	    if (has_lex)
	      gen_Lex (lex_set);
	      
	    if (is_look_ahead)
	      gen_First (q);

	    fprintf (out_file, "%sq%ld(X) . %% %ld\n", varstr_tostr (varstr), q, fsa_clause);

	    if (SXBA_bit_is_reset_set (fsa_state_set, q))
	      PUSH (state_stack, q);
	  }
	}
      }
    }
  }

  sxfree (fsa_state_set);
  sxfree (state_stack);
  varstr_free (varstr), varstr = NULL;
}
#endif /* 0 */


static void
fsa2c (SXINT *last_index, SXINT *last_red)
{


  struct matrix fsa_shift;
  struct vector vector;

  SXINT *fsa_red_disp;
  struct trans {
    SXINT X, q;
  } *fsa_red_trans;

  SXINT  i, p, X, q, triple, top, *ip, *intarea, fsa_red_top, prod, final_state, prev_final_state, fsa_shift_nb;
  char string [22];
  SXBA for_look_ahead = NULL;

  final_state = sxba_scan (fsa_final_state_set, -1);
  prev_final_state = 0;

  if (final_state == -1 || sxba_scan (fsa_final_state_set, final_state) != -1)
    /* On suppose qu'il y a un etat final unique */
    sxtrap (ME, "fsa2c");

#if EBUG
  if (sxba_scan (fsa_final_state_set, -1) == -1)
    /* Pas d'etat final */
    sxtrap (ME, "fsa2c");
#endif

  fsa_shift.last_line = fsa_state_nb;
  fsa_shift.last_col = tmax+1;
  fsa_shift.matrix = (SXINT**) sxalloc (fsa_state_nb+1, sizeof (SXINT*));
  ip = intarea = (SXINT*) sxalloc ((fsa_state_nb+1)*(tmax+1), sizeof (SXINT));

  for (i = 0; i <= fsa_state_nb; i++) {
    fsa_shift.matrix [i] = ip;
    ip += tmax+1;
  }

  fsa_shift.signature = sxbm_calloc (fsa_state_nb+1, tmax+1);

  fsa_red_disp = (SXINT*) sxalloc (fsa_state_nb+2, sizeof (SXINT)), fsa_red_disp [0] = 0;
  fsa_red_trans = (struct trans*) sxalloc (XxYxZ_top (fsa_hd)+1, sizeof (struct trans));
  fsa_red_trans [0].X = 0;
  fsa_red_trans [0].q = 0;
  fsa_red_top = 1;

  if (is_look_ahead)
    for_look_ahead = sxba_calloc (fsa_state_nb+1);

  for (p = 1; p <= fsa_state_nb; p++) {
    fsa_red_disp [p] = fsa_red_top;

    XxYxZ_Xforeach (fsa_hd, p, triple) {
      q = XxYxZ_Z (fsa_hd, triple);
      X = XxYxZ_Y (fsa_hd, triple);

      if (X >= 0) {
	fsa_red_trans [fsa_red_top].X = X;
	fsa_red_trans [fsa_red_top].q = q;
	fsa_red_top++;

	if (for_look_ahead && X > 0 && X <= last_prod)
	  SXBA_1_bit (for_look_ahead, q);
      }
      else {
	X = -X;

	if (X > tmax) {
	  /* eof code par 0 */
	  X = 0;
	  
	  if (q != final_state)
	    sxtrap (ME, "fsa2c");

	  prev_final_state = p;
	}

	fsa_shift.matrix [p][X] = q;
	SXBA_1_bit (fsa_shift.signature [p], X);
      }
    }
  }

  fsa_red_disp [p] = fsa_red_top;
#if 0
  /* Le 01/10/2002 is_lex devient une option de compilation "-Dis_lex=(0|1)" */
  fprintf (out_file, "#define is_lex %ld\n", (!is_tagger || is_keep_lex) ? 1 : 0);
#endif
  fprintf (out_file, "#define is_shallow %ld\n", (SXINT) (is_shallow ? 1 : 0));
  fprintf (out_file, "\n#define fsa_state_nb %ld\n", (SXINT) fsa_state_nb);
  fprintf (out_file, "#define last_prod %ld\n", (SXINT) last_prod);
  fprintf (out_file, "#define ntmax %ld\n", (SXINT) max_nt);
  fprintf (out_file, "/* Pour le debug */\n\
static SXINT final_state = %ld, prev_final_state = %ld;\n", (SXINT) final_state, (SXINT) prev_final_state);

#if 0
  /* inutile */
  sxba_to_c (fsa_final_state_set, out_file, "fsa_final_state_set", "", true /* static */);
#endif

  matrix2vector (&fsa_shift, &vector, 25);
  vector2c (&vector, out_file, "fsa_shift", true /* static */);
  fsa_shift_nb = vector.last_index;

  if (fsa_red_top == 1) {
    fputs ("static SXINT *fsa_red_disp;\n", out_file);
    fputs ("static struct trans *fsa_red_trans;\n", out_file);
  }
  else {
    fprintf (out_file, "\nstatic SXINT fsa_red_disp [%ld] = {0,", (SXINT) fsa_state_nb+2);

    for (i = 1; i <= fsa_state_nb+1; i++) {
      if ((i%10)==1)
	fprintf (out_file, "\n/* %ld */ ", (SXINT) i);

      fprintf (out_file, "%ld, ", (SXINT) fsa_red_disp [i]);
    }

    fputs ("\n};\n", out_file);

    fprintf (out_file, "\nstatic struct trans fsa_red_trans [%ld] = {{0, 0},", (SXINT) fsa_red_top);

    for (i = 1; i < fsa_red_top; i++) {
      if ((i%10)==1)
	fprintf (out_file, "\n/* %ld */ ", (SXINT) i);

      fprintf (out_file, "{%ld, %ld}, ", (SXINT) fsa_red_trans [i].X, (SXINT) fsa_red_trans [i].q);
    }

    fputs ("\n};\n", out_file);
  }

  if ((top = XH_top (mtrans_hd)) > 1) {
    fprintf (out_file, "\nstatic SXINT mtrans_disp [%ld] = {0,", (SXINT) top+1);

    for (i = 1; i <= top; i++) {
      if (i % 10 == 1)
	fprintf (out_file, "\n/* %ld */ ", (SXINT) i);

      fprintf (out_file, "%ld, ", (SXINT) XH_X (mtrans_hd, i));
    }

    fputs ("\n};\n", out_file);

    fprintf (out_file, "static SXINT mtrans_list [%ld] = {0,", (SXINT) XH_list_top (mtrans_hd));

    for (i = 1; i < XH_list_top (mtrans_hd); i++) {
      if (i % 10 == 1)
	fprintf (out_file, "\n/* %ld */ ", (SXINT) i);
      prod = XH_list_elem (mtrans_hd, i);

#if EBUG
      if (prod > last_prod)
	sxtrap (ME, "fsa2c");
#endif

      fprintf (out_file, "%ld, ", (SXINT) prod);
    }

    fputs ("\n};\n", out_file);
  }
  else
    fputs ("\nstatic SXINT *mtrans_disp, *mtrans_list;\n", out_file);

  if (is_look_ahead) {
    q = 0;

    while ((q = sxba_scan (for_look_ahead, q)) > 0) {
      look_ahead (q); /* t_set contient les look_ahead */
      sprintf (string, "_%ld", (SXINT) q);
      sxba_to_c (t_set, out_file, "look_ahead", string, true /* static */);
      sxba_empty (t_set);
    }

    fprintf (out_file, "\nstatic SXBA look_ahead [%ld] = {NULL,\n", (SXINT) fsa_state_nb+1);

    for (q = 1; q <= fsa_state_nb; q++) {
      if (SXBA_bit_is_set (for_look_ahead, q))
	fprintf (out_file, "look_ahead_%ld,\n", (SXINT) q);
      else
	fputs ("NULL,\n", out_file);
    }

    fputs ("};\n", out_file);
  }
  else
    fputs ("\nstatic SXBA *look_ahead;\n", out_file);

  sxfree (fsa_shift.matrix);
  sxfree (intarea);
  sxfree (fsa_shift.signature);
  sxfree (fsa_red_disp);
  sxfree (fsa_red_trans);

  vector_free (&vector);
  /* sxfree (vector.list);
     sxfree (vector.disp); */

  if (for_look_ahead) sxfree (for_look_ahead);

  if (!is_tagger || is_keep_lex) {
    SXINT bot, nb;
    SXBA *t2prod_set;

    t2prod_set = sxbm_calloc (max_t+1, last_prod+1);

    for (prod = 1; prod <= last_prod; prod++) {
      bot = bnf.prolon [prod];
      nb = 0;

      while ((X = bnf.lispro [bot++]) != 0) {
	if (X < 0) {
	  nb++;
	  X = -X;
	  SXBA_1_bit (t2prod_set [X], prod);
	}
      }

      if (nb == 0) {
	SXBA_1_bit (t2prod_set [0], prod);
      }
    }

#if 0
    /* sans objet maintenant */
    if (is_tagger)
      sxba_empty (t2prod_set [0]); /* Pour accelerer fsa_lexicalize */
#endif /* 0 */

    fputs ("#if is_lex\n", out_file);
    sxbm3_to_c (out_file, t2prod_set, max_t+1, "t2prod_set", "", true /* is_static */);
    fputs ("#endif /* is_lex */\n", out_file);

    sxbm_free (t2prod_set);
  }

  if (!is_tagger || is_keep_lex) {
    fputs ("\nstatic struct fsaG G = {\n\
last_prod, ntmax, 1, 0, 0,\n\
#if is_lex\n\
t2prod_set,\n\
#else\n\
NULL, /* t2prod_set */\n\
#endif\n\
NULL, NULL, NULL,/* Lex, fsa_valid_prod_set, *prod2lub */\n\
NULL, NULL,/* process_fsa, final_fsa */\n\
is_shallow,\n\
};\n",
	     out_file);
  }
  else {
    fputs ("\nstatic struct fsaG G = {\n\
last_prod, ntmax, 1, 0, 0,\n\
NULL, /* t2prod_set */\n\
NULL, NULL, NULL,/* Lex, fsa_valid_prod_set, *prod2lub */\n\
NULL, NULL,/* process_fsa, final_fsa */\n\
is_shallow,\n\
};\n",
	   out_file);
  }

  *last_index = fsa_shift_nb;
  *last_red = fsa_red_top-1;
}


/* Construction d'un automate (transducteur) fini qui decrit un sur-langage d'une simple 1-RCG donnee */
static void
fsa (void)
{
  SXINT last_index, last_red;

#if EBUG
  if (sxverbosep) {
    fputs ("CFG = ", sxtty);
    sxttycol1p = true;
  }
#endif

  /* transforme la 1_RCG simple en une CFG a la SYNTAX */
  simple1_rcg2cfg ();

#if EBUG
  if (sxverbosep) {
    fprintf (sxtty, "%ld/%ld, ", bnf.nbpro, bnf.indpro);
    sxttycol1p = true;
  }
#endif

  t_set = sxba_calloc (tmax+1/* eof */+1);
  lex_set = sxba_calloc (tmax+1);
  prod_set = sxba_calloc (last_prod+1);
  /* fsa_clause2clause = (SXINT*) sxcalloc (last_clause+2, sizeof (SXINT)); */

  XxYxZ_alloc (&pda_hd, "pda_hd", 2*bnf.indpro+1, 1, pda_assoc_list, NULL, statistics);

  if (is_left_corner) {
#if EBUG
    if (sxverbosep) {
      fputs ("LC = ", sxtty);
      sxttycol1p = true;
    }
#endif

    /* calcul de la relation left_corner */
    nt2item_set = sxbm_calloc (ntmax+1, bnf.indpro+1);
    left_corner = sxbm_calloc (ntmax+1, ntmax+1);

    LC_relation ();

    sxbm_free (left_corner), left_corner = NULL;

    LC0_automaton ();

    sxbm_free (nt2item_set), nt2item_set = NULL;
  }
  else {
#if EBUG
    if (sxverbosep) {
      fputs ("LR(0) = ", sxtty);
      sxttycol1p = true;
    }
#endif

    LR0_automaton ();
  }

#if EBUG
  if (sxverbosep) {
    fprintf (sxtty, "%ld/%ld, NDFSA = ", pda_state_nb, XxYxZ_top (pda_hd));
    sxttycol1p = true;
  }
#endif

  pda2ndfsa ();

  XxYxZ_free (&pda_hd);

#if EBUG
  if (sxverbosep) {
    fprintf (sxtty, "%ld/%ld", ndfsa_state_nb, XxYxZ_top (ndfsa_hd));
    sxttycol1p = true;
  }
#endif


  XH_alloc (&mtrans_hd, "mtrans_hd", last_prod+1, 1, 8, NULL, statistics);

  if (is_early_cycle_erasing) {
    cycle_erasing (&ndfsa_state_nb, &ndfsa_init_state, &ndfsa_final_state_set, &ndfsa_hd, ndfsa_assoc_list);

#if EBUG
    if (sxverbosep) {
      fprintf (sxtty, " -- cf[%ld/%ld]", ndfsa_state_nb, XxYxZ_top (ndfsa_hd));
      sxttycol1p = true;
    }
#endif
  }

  if (has_null_trans && used_nfndfsa) {
    /* On supprime les transitions nulles de ndfsa */
    SXINT p, q, X, triple;
    SXBA linep, *fsa_R;

    fsa_R = sxbm_calloc (ndfsa_state_nb+1, ndfsa_state_nb+1);

    for (p = 1; p <= ndfsa_state_nb; p++) {
      linep = fsa_R [p];

      XxYxZ_Xforeach (ndfsa_hd, p, triple) {
	X = XxYxZ_Y (ndfsa_hd, triple);

	if (X == 0) {
	  /* On ne supprime pas les transitions vides faisant partie des transitions multiples */
	  q = XxYxZ_Z (ndfsa_hd, triple);
	  SXBA_1_bit (linep, q);
	}
      }
    }
    
    NULL_RED = R_cycle (fsa_R, ndfsa_state_nb+1, (SXINT*)NULL);

    sxbm_free (fsa_R);

    ndfsa2nfndfsa ();

    has_null_trans = false;

    sxbm_free (NULL_RED),  NULL_RED = NULL;

#if EBUG
    if (sxverbosep) {
      fprintf (sxtty, " -- nf[%ld/%ld]", ndfsa_state_nb, XxYxZ_top (ndfsa_hd));
      sxttycol1p = true;
    }
#endif
  }


#if EBUG
  if (sxverbosep) {
    fputs (", DFSA = ", sxtty);
    sxttycol1p = true;
  }
#endif 

  ndfsa2dfsa ();

#if EBUG
  if (sxverbosep) {
    fprintf (sxtty, "%ld/%ld, FSA = ", dfsa_state_nb, XxYxZ_top (dfsa_hd));
    sxttycol1p = true;
  }
#endif

  dfsa2min_fsa ();

  /* Ici on a p RED q <==> il y a une chaine non vide de reductions entre p et q */

  XxYxZ_free (&dfsa_hd);
  sxfree (dfsa_final_state_set), dfsa_final_state_set = NULL;

#if EBUG
  if (sxverbosep) {
    fprintf (sxtty, "%ld/%ld", fsa_state_nb, XxYxZ_top (fsa_hd));
    sxttycol1p = true;
  }
#endif

  if (!is_early_cycle_erasing) {
    cycle_erasing (&fsa_state_nb, &fsa_init_state, &fsa_final_state_set, &fsa_hd, fsa_assoc_list);

#if EBUG
    if (sxverbosep) {
      fprintf (sxtty, " -- %ld/%ld", fsa_state_nb, XxYxZ_top (fsa_hd));
      sxttycol1p = true;
    }
#endif

    if (!is_deterministic) {
      /* L'elimination des cycles l'a rendu non deterministe */
      /* On recommence */

#if EBUG
      if (sxverbosep) {
	fputs (", DFSA = ", sxtty);
	sxttycol1p = true;
      }
#endif 
      ndfsa_hd = fsa_hd; /* FAUX ndfsa_hd a du XxYxZ_XYforeach!! */
      ndfsa_final_state_set = fsa_final_state_set;
      ndfsa_init_state = fsa_init_state;

      ndfsa2dfsa ();

#if EBUG
      if (sxverbosep) {
	fprintf (sxtty, "%ld/%ld, FSA = ", dfsa_state_nb, XxYxZ_top (dfsa_hd));
	sxttycol1p = true;
      }
#endif

      dfsa2min_fsa ();

      /* Ici on a p RED q <==> il y a une chaine non vide de reductions entre p et q */

      XxYxZ_free (&dfsa_hd);
      sxfree (dfsa_final_state_set), dfsa_final_state_set = NULL;

#if EBUG
      if (sxverbosep) {
	fprintf (sxtty, "%ld/%ld", fsa_state_nb, XxYxZ_top (fsa_hd));
	sxttycol1p = true;
      }
#endif
    }
  }

  if (fsa_has_reduction && !process_single_transition) {
    /* Les reductions multiples entre p et q sont factorisees */
    reduce_fsa ();

#if EBUG
    if (sxverbosep) {
      fprintf (sxtty, " -- %ld/%ld", fsa_state_nb, XxYxZ_top (fsa_hd));
      sxttycol1p = true;
    }
#endif
  }

  if (is_look_ahead) {
    fill_RED (fsa_state_nb, fsa_hd);
    state2look_ahead_set = sxbm_calloc (fsa_state_nb+1, tmax+1);
    fill_state2look_ahead_set ();
  }
    
#if 0
  /* C,a ne semble plus servir a rien (Le 2/10/2002 */
  if (is_fsa2rcg) {
    /* On genere une rcg */
    /* Si p ->0 q on genere p(X) --> q(X) . */
#if EBUG
    if (sxverbosep) {
      fputs (", RCG = ", sxtty);
      sxttycol1p = true;
    }
#endif

    if (is_left_corner)
      out_header (rcg_header, lfsa_file_name, "in left-corner form");
    else
      out_header (rcg_header, lfsa_file_name, "in LR(0) form");

    fsa2rcg ();

#if EBUG
    if (sxverbosep) {
      fprintf (sxtty, "%ld, ", fsa_prod);
      sxttycol1p = true;
    }
#endif
  }
  else
#endif /* 0 */
    {
#if EBUG
    if (sxverbosep) {
      fputs (", GEN_C", sxtty);
      sxttycol1p = true;
    }
#endif

    if (lfsa_file) {
      if (is_left_corner)
	out_header (c_header, lfsa_file_name, "in left-corner form");
      else
	out_header (c_header, lfsa_file_name, "in LR(0) form");
    }
    else {
      if (is_left_corner)
	out_header (c_header, rfsa_file_name, "in left-corner form");
      else
	out_header (c_header, rfsa_file_name, "in LR(0) form");
    }

    fsa2c (&last_index, &last_red);

#if EBUG
    if (sxverbosep) {
      fprintf (sxtty, "[%ld/%ld] ", last_index, last_red);
      sxttycol1p = true;
    }
#endif
  }

  if (is_look_ahead) {
    sxbm_free (RED), RED = NULL;
    sxbm_free (state2look_ahead_set), state2look_ahead_set = NULL;
  }

  XxYxZ_free (&fsa_hd);
  sxfree (fsa_final_state_set), fsa_final_state_set = NULL;
  XH_free (&mtrans_hd);

  free_bnf (bnf);

  sxfree (t_set), t_set = NULL;
  sxfree (lex_set), lex_set = NULL;
  sxfree (prod_set), prod_set = NULL;

  fclose (out_file);

  if (sxverbosep) {
    fputs ("done\n", sxtty);
    sxttycol1p = true;
  }
}


void
lrfsa (void)
{
  if (max_garity != 1 || !is_left_linear_grammar || !is_right_linear_grammar || is_combinatorial_grammar ||
      is_top_down_erasing_grammar || is_bottom_up_erasing_grammar) {
    if (sxverbosep) {
      fputs ("WARNING: Only simple 1-RCGs can generate linear time guides.\n", sxtty);
      sxttycol1p = true;
    }

    return;
  }

  clause2lex_set = sxbm_calloc (last_clause+1, max_t+1);
  clause_has_lex = sxba_calloc (last_clause+1);
  eps_clause_set = sxba_calloc (last_clause+1);

  pre_processing_clause_set ();

  if (is_tagger) {
    /* Le FSA sera utilise pour faire du POS tagging, on ne s'interesse donc pas aux reductions.
       Cependant, le langage du FSA peut etre reduit si on conserve (et utilise) les transitions
       reduce A-> \alpha associees a des Lex non vide.  Cette possibilite est gouvernee par l'option
       is_keep_lex. 
       L'experience a montre que si is_keep_lex est false, l'analyse est + rapide
    */
    useless_clause_set = sxba_calloc (last_clause+1);

    if (is_keep_lex) {
      sxba_copy (useless_clause_set, clause_has_lex);
      sxba_not (useless_clause_set);

      if (!is_keep_epsilon_rules) {
	/* On va supprimer les productions vides */
	sxba_or (useless_clause_set, eps_clause_set);
      }
    }
    else
      /* On supprime toutes les reductions */
      sxba_fill (useless_clause_set);
  }
  else {
    if (useless_clause_file) {
      useless_clause_set = sxba_calloc (last_clause+1);
      /* On suppose qu'on dispose de l'ensemble useless_clause_set des clauses dont il ne faut pas reperer les bornes */
      /* Cet ensemble est fabrique a partir de la lecture du fichier useless_clause_file (A FAIRE) */
    }
  }

  if (is_shallow) {
    if (shallow_file != NULL) {
      SXINT        ste;
      int          symb;
      SXINT        nt, unknown_nt_nb = 0;
      VARSTR       prdct_name;

      shallow_set = sxba_calloc (max_nt+1);

      /* On remplit shallow_set */
      prdct_name = varstr_alloc (64);

      do {
	symb = getc (shallow_file);

	if (symb == EOF || isspace (symb)) {
	  if (varstr_length (prdct_name)) {
	    (void) varstr_complete (prdct_name);
	    ste = sxstrretrieve (varstr_tostr (prdct_name));
	    varstr_raz (prdct_name);

	    if (ste != SXERROR_STE && (nt = ste2nt [ste]) != 0) {
	      SXBA_1_bit (shallow_set, nt);
	    }
	    else
	      unknown_nt_nb++;
	  }
	}
	else {
	  prdct_name = varstr_catchar (prdct_name, (char)symb);
	}
      } while (symb != EOF);

      varstr_free (prdct_name);

      if (unknown_nt_nb)
	fprintf (sxtty, "WARNING: the \"shallow\" file %s contains %ld unknown predicate names.\n",
		 shallow_file_name, (SXINT) unknown_nt_nb);
    }
  }

  tmax = max_t;
  ntmax = max_nt;

  if (lfsa_file) {
    if (sxverbosep) {
      fputs ("\tlfsa_form .... ", sxtty);
      sxttycol1p = false;
    }

    out_file = lfsa_file;
    is_bnf_in_reverse = false;

    fsa ();
  }

  if (rfsa_file) {
    if (sxverbosep) {
      fputs ("\trfsa_form .... ", sxtty);
      sxttycol1p = false;
    }

    out_file = rfsa_file;
    is_bnf_in_reverse = true;

    fsa ();
  }

  if (useless_clause_set) sxfree (useless_clause_set), useless_clause_set = NULL;

  if (shallow_set) sxfree (shallow_set), shallow_set = NULL;

  sxfree (eps_clause_set), eps_clause_set = NULL;
  sxbm_free (clause2lex_set), clause2lex_set = NULL;
  sxfree (clause_has_lex), clause_has_lex = NULL;
}






#if 0
/* A conserver au cas ou */
/* "Traitement" des productions vides */
static SXBA bvide, bnonvide, prod_vide, prod_nonvide, clause_vide, clause_nonvide;
static bool has_eps_rules, has_eps_trans, eps_rules_are_null;

static SXINT *eps_trans_stack;
static SXBA *EPS_RED;


static void
fill_bvide ()
{
  /* This function computes the set bvide of all non terminal symbols */
  /* which can generate an empty string 		  */
  SXINT	rule, nt, i, top, rhs_nt;
  SXINT *null_stack, *rhs_size;

  null_stack = (SXINT*) sxalloc (ntmax + 1, sizeof (SXINT)), null_stack [0] = 0;
  rhs_size = (SXINT*) sxalloc (bnf.nbpro + 1, sizeof (SXINT));

  for (rule = 1; rule <= bnf.nbpro; rule++) {
    if ((rhs_size [rule] = bnf.prolon[rule+1] - bnf.prolon[rule] - 1) == 0) {
      nt = bnf.reduc [rule];
	  
      if (SXBA_bit_is_reset_set (bvide, nt)) {
	PUSH (null_stack, nt);
      }
    }
  }

  while (!IS_EMPTY (null_stack)) {
    rhs_nt = POP (null_stack);

    for (top = bnf.npd [rhs_nt+1], i = bnf.npd [rhs_nt]; i < top; i++) {
      rule = bnf.prolis [bnf.numpd [i]];

      if ((rule > 0) && (0 == --rhs_size [rule])) {
	nt = bnf.reduc [rule];
	  
	if (SXBA_bit_is_reset_set (bvide, nt))
	  PUSH (null_stack, nt);
      }
    }
  }

  sxfree (null_stack);
  sxfree (rhs_size);
}



static void
fill_bnonvide ()
{
  /* This function computes bnonvide, the set of all non terminal symbols  */
  /* which can generate a non empty terminal string              */
  /* We assume that the grammar is proper                        */
  SXINT	rule, top, item, X, nt;
  bool has_changed = true;
  SXBA rule_set = sxba_calloc (bnf.nbpro+1);

  while (has_changed) {
    has_changed = false;
    rule = 0;

    while ((rule = sxba_0_lrscan (rule_set, rule)) > 0) {
      nt = bnf.reduc [rule];
    
      if (!SXBA_bit_is_set (bnonvide, nt)) {
	for (top = bnf.prolon [rule+1] - 1, item = bnf.prolon [rule]; item < top; item++) {
	  X = bnf.lispro [item];

	  if (X < 0 || SXBA_bit_is_set (bnonvide, X)) {
	    SXBA_1_bit (bnonvide, nt);
	    SXBA_1_bit (rule_set, rule);
	    has_changed = true;
	    break;
	  }
	}
      }
      else
	SXBA_1_bit (rule_set, rule);
    }
  }

  sxfree (rule_set);
}

static void
fill_prodvide ()
{
  /* Emplit prod_vide = {A -> \alpha | \alpha =>* \varepsilon}  */
  /* et prod_nonvide = {A -> \alpha | \alpha =>* x, x \in T^+}  */
  SXINT prod, item, X, clause;

  for (prod = 1; prod <= bnf.nbpro; prod++) {
    item = bnf.prolon [prod];

    while ((X = bnf.lispro [item++]) != 0) {
      if (X < 0 || !SXBA_bit_is_set (bvide, X)) {
	break;
      }
    }

    if (X == 0)
      SXBA_1_bit (prod_vide, prod);

    item = bnf.prolon [prod];

    while ((X = bnf.lispro [item++]) != 0) {
      if (X < 0 || SXBA_bit_is_set (bnonvide, X)) {
	SXBA_1_bit (prod_nonvide, prod);
	break;
      }
    }
  }
}





static SXINT efnbpro, efindpro, efsize1, efsize2;
static SXINT *efstack, *efprolon, *eflispro;

static void
efnext (item)
     SXINT item;
{
  SXINT X, l, i;

  X = bnf.lispro [item];

  if (X == 0) {
    if (!IS_EMPTY (efstack)) {
      /* On cree une nelle prod a partir de efstack */
      efbnf.nbpro++;
      efbnf.indpro++;

      if (efbnf.nbpro >= efsize1)
	efbnf.prolon = (SXINT *) sxrealloc (efbnf.prolon, (efsize1 *= 2) + 1, sizeof (SXINT));

      efbnf.prolon [efbnf.nbpro] = efbnf.indpro;

      l = efstack [0];

      if (efbnf.indpro+l+1 >= efsize2)
	efbnf.lispro = (SXINT *) sxrealloc (efbnf.lispro, (efsize2 *= 2) + 1, sizeof (SXINT));

      for (i = 1; i <= l; i++)
	efbnf.lispro [efbnf.indpro++] = efstack [i];

      efbnf.lispro [efbnf.indpro] = bnf.prolis [item]; /* provisoire */
    }
  }
  else {
    if (X < 0) {
      PUSH (efstack, X);
      efnext (item+1);
      POP (efstack);
    }
    else {
      if (SXBA_bit_is_set (bvide, X)) {
	if (SXBA_bit_is_set (bnonvide, X)) {
	  PUSH (efstack, X);
	  efnext (item+1);
	  POP (efstack);
	}

	efnext (item+1);
      }
      else {
	PUSH (efstack, X);
	efnext (item+1);
	POP (efstack);
      }
    }
  }
}


/* Elimine les productions vides */
static void
cfg2efcfg ()
{
  SXINT prod, efprod, top, *p, *q, *pbot, symb, efxnumpd, efxnumpg, i, nt;
  struct bnf efbnf;
  SXINT *efnt2lhs_nb, *efnt2rhs_nb;

  efsize1 = 2*bnf.nbpro;
  efsize2 = 2*bnf.indpro;

  efbnf.prolon = (SXINT*) sxalloc (efsize1+1, sizeof (SXINT));
  efbnf.lispro = (SXINT*) sxalloc (efsize2+1, sizeof (SXINT));

  efstack  = (SXINT*) sxalloc (bnf.rhs_lgth+1, sizeof (SXINT)), efstack [0] = 0;

  efbnf.indpro = 3;
  efbnf.nbpro = 0;

  for (prod = 1; prod <= bnf.nbpro; prod++) {
    efnext (bnf.prolon [prod]);
  }

  /* petite marge pour S -> eps eventuel */
  if (efbnf.nbpro+1 >= efsize1)
    efbnf.prolon = (SXINT *) sxrealloc (efbnf.prolon, (efsize1 += 1) + 1, sizeof (SXINT));

  if (efbnf.indpro+1 >= efsize2)
    efbnf.lispro = (SXINT *) sxrealloc (efbnf.lispro, (efsize2 += 1) + 1, sizeof (SXINT));

  efbnf.prolon [efbnf.nbpro+1] = efbnf.indpro+1;
  
  efbnf.reduc = (SXINT*) sxalloc (efbnf.nbpro+2, sizeof (SXINT));
  efbnf.numpg = (SXINT*) sxalloc (efbnf.nbpro+2, sizeof (SXINT));
  efbnf.prolis = (SXINT*) sxalloc (efbnf.indpro+2, sizeof (SXINT));
  efbnf.npd = (SXINT*) sxalloc (bnf.ntmax+2, sizeof (SXINT));
  efbnf.npg = (SXINT*) sxalloc (bnf.ntmax+2, sizeof (SXINT));

  efbnf.action = (SXINT*) sxcalloc (efbnf.nbpro+2, sizeof (SXINT));

  efnt2lhs_nb = (SXINT*) sxcalloc (bnf.ntmax+1, sizeof (SXINT));
  efnt2rhs_nb = (SXINT*) sxcalloc (bnf.ntmax+1, sizeof (SXINT));
  
  efbnf.maxnpd = 0;

  efbnf.reduc [0] = efbnf.prolis [0] = efbnf.lispro [0] = 0;
  efbnf.lispro [1] = 1;
  efbnf.prolis [1] = 0;
  efbnf.maxnpd++;
  /* <axiom> in the RHS of the dummy rule */
  efbnf.maxnpd++;
  /* compatibility with the old INTRO    */
  efbnf.lispro [2] = -(tmax+1);
  efbnf.prolis [2] = 0;
  efbnf.prolis [3] = efbnf.lispro [3] = 0;
  efbnf.prolon [0] = 1;
  efbnf.action [0] = 0;
  efnt2lhs_nb [0] = 1;
  efnt2rhs_nb [1] = 1;

  for (efprod = 1; efprod <= efbnf.nbpro; efprod++) {
    top = efbnf.prolon [efprod+1]-1;
    p = efbnf.lispro + top;
    prod = *p;
    *p = 0;

    efbnf.action [efprod] = bnf.action [prod];

    efbnf.reduc [efprod] = nt = bnf.reduc [prod];
    efnt2lhs_nb [nt]++;
    q = efbnf.prolis + top;
    *q = efprod;
    pbot = efbnf.lispro+efbnf.prolon [efprod];

    while (pbot <= --p) {
      *--q = efprod;
      efbnf.maxnpd++;

      if ((symb = *p) > 0) {
	efnt2rhs_nb [symb]++;
      }
    }
  }

  if (SXBA_bit_is_set (bvide, 1)) {
    /* La chaine vide est ds le langage, on genere S --> ; */
      efbnf.nbpro++;
      efbnf.indpro++;

      efbnf.reduc [efbnf.nbpro] = 1;
      efnt2lhs_nb [1]++;
      efbnf.lispro [efbnf.indpro] = 0;
      efbnf.prolis [efbnf.indpro] = efbnf.nbpro;

      efbnf.prolon [efbnf.nbpro+1] = efbnf.indpro+1;
      has_eps_rules = true;
  }
  else
      has_eps_rules = false;

  efbnf.numpd = (SXINT*) sxalloc (efbnf.maxnpd + 1, sizeof (SXINT));

  efxnumpd = 1;
  efxnumpg = 1;

  for (symb = 1; symb <= bnf.ntmax; symb++) {
    efbnf.npg [symb] = efxnumpg += efnt2lhs_nb [symb];
    efbnf.npd [symb] = efxnumpd += efnt2rhs_nb [symb];
  }

  efbnf.npg [bnf.ntmax + 1] = efxnumpg;
  efbnf.npd [bnf.ntmax + 1] = efxnumpd;

  for (i = efbnf.nbpro; i >= 1; i--)
    efbnf.numpg [--efbnf.npg [efbnf.reduc [i]]] = i;

  for (i = efbnf.indpro; i >= 1; i--) {
    if ((symb = efbnf.lispro [i]) > 0) {
      efbnf.numpd [--efbnf.npd [symb]] = i;
    }
  }

  sxfree (efnt2lhs_nb);
  sxfree (efnt2rhs_nb);
  sxfree (efstack), efstack = NULL;

  free_bnf (bnf);

  bnf.reduc = efbnf.reduc;
  bnf.prolon = efbnf.prolon;
  bnf.numpg = efbnf.numpg;
  bnf.lispro = efbnf.lispro;
  bnf.prolis = efbnf.prolis;
  bnf.npd = efbnf.npd;
  bnf.npg = efbnf.npg;
  bnf.action = efbnf.action;

  bnf.indpro = efbnf.indpro;
  bnf.nbpro = efbnf.nbpro;
}



  if (skip_eps_rules && has_eps_rules && used_efcfg) {
    bvide = sxba_calloc (max_nt+1);
    fill_bvide ();

    bnonvide = sxba_calloc (max_nt+1);
    fill_bnonvide ();

#if EBUG
    if (sxverbosep) {
      fprintf (sxtty, "%ld/%ld -- ", bnf.nbpro, bnf.indpro);
      sxttycol1p = true;
    }
#endif

    cfg2efcfg ();
    /* Attention, has_eps_rules peut etre encore vrai si eps est ds le langage */

    sxfree (bvide), bvide = NULL;
    sxfree (bnonvide), bnonvide = NULL;
  }

  if (has_eps_rules) {
    bvide = sxba_calloc (max_nt+1);
    fill_bvide ();

    bnonvide = sxba_calloc (max_nt+1);
    fill_bnonvide ();

    prod_vide = sxba_calloc (bnf.nbpro+1);
    prod_nonvide = sxba_calloc (bnf.nbpro+1);
    fill_prodvide ();
  }

  if (bvide) sxfree (bvide), bvide = NULL;
  if (bnonvide) sxfree (bnonvide), bnonvide = NULL;
  if (prod_vide) sxfree (prod_vide), prod_vide = NULL;
  if (prod_nonvide) sxfree (prod_nonvide), prod_nonvide = NULL;
  if (clause_nonvide) sxfree (clause_nonvide), clause_nonvide = NULL;
  if (clause_vide) sxfree (clause_vide), clause_vide = NULL;

#endif /* #if 0 */

#if 0

QUELQUES RESULTATS ...

boullier@bourbon /parseurs/TAG/spec 
Wed Feb 13, 17:49:48 time rcg -no_bnf -no_check_set -simple -h 1 -lr0\
>                                        -lfsa ../src/xtag3_rcg_1rcg_lr0_lfsa.h\
>                                        -rfsa ../src/xtag3_rcg_1rcg_lr0_rfsa.h\
>                                        xtag3_rcg.1rcg > /dev/null
Release 3.9 of tomorrow
xtag3_rcg.1rcg:
        Semantic Pass .... done
        Checking .... done
        First & last .... done
        lfsa_form .... CFG = 1148/16034, LR(0) = 9127/146248, NDFSA = 9126/590113 9 cycles over 3831 states  -- 5313/129154, DFSA = 5484/44189, FSA = 1847/15557 -- 1846/12091, GEN_C[8289/4273] done
        rfsa_form .... CFG = 1148/16034, LR(0) = 7392/73650, NDFSA = 7391/303805 9 cycles over 1049 states  -- 6360/255708, DFSA = 2634/20816, FSA = 1942/15805 -- 1941/11375, GEN_C[6290/5157] done
        gen parser .... (arity=1, clause_nb=1696, degree=27 for clause #336, size=15618) done
17.12user 0.37system 0:17.51elapsed 99%CPU (0avgtext+0avgdata 0maxresident)k
0inputs+0outputs (455major+30988minor)pagefaults 0swaps
boullier@bourbon /parseurs/TAG/spec 
Wed Feb 13, 17:50:07 time rcg -no_bnf -no_check_set -simple -h 1 -lr0 -tag\
>                                        -lfsa ../src/xtag3_rcg_1rcg_lr0_tag_lfsa.h\
>                                        -rfsa ../src/xtag3_rcg_1rcg_lr0_tag_rfsa.h\
>                                        xtag3_rcg.1rcg > /dev/null
Release 3.9 of tomorrow
xtag3_rcg.1rcg:
        Semantic Pass .... done
        Checking .... done
        First & last .... done
        lfsa_form .... CFG = 1148/16034, LR(0) = 9127/146248, NDFSA = 9126/590113 9 cycles over 3831 states  -- 5313/126750, DFSA = 7370/116581, FSA = 2451/44065 -- 2451/41261, GEN_C[72025/5426] done
        rfsa_form .... CFG = 1148/16034, LR(0) = 7392/73650, NDFSA = 7391/303805 9 cycles over 1049 states  -- 6360/252292, DFSA = 3340/40622, FSA = 2588/33518 -- 2587/29602, GEN_C[33863/6168] done
        gen parser .... (arity=1, clause_nb=1696, degree=27 for clause #336, size=15618) done
24.54user 0.35system 0:24.90elapsed 99%CPU (0avgtext+0avgdata 0maxresident)k
0inputs+0outputs (455major+33891minor)pagefaults 0swaps
boullier@bourbon /parseurs/TAG/spec 
Wed Feb 13, 17:50:32 time rcg -no_bnf -no_check_set -simple -h 1 -sr -lr0\
>                                        -lfsa ../src/xtag3_rcg_1rcg_sr_lr0_lfsa.h\
>                                        -rfsa ../src/xtag3_rcg_1rcg_sr_lr0_rfsa.h\
>                                        xtag3_rcg.1rcg > /dev/null
Release 3.9 of tomorrow
xtag3_rcg.1rcg:
        Semantic Pass .... done
        Checking .... done
        First & last .... done
        lfsa_form .... CFG = 1148/16034, LR(0) = 8021/145131, NDFSA = 17613/596043 9 cycles over 3838 states  -- 13793/139551, DFSA = 12122/58123, FSA = 3780/20213 -- 3773/16271, GEN_C[10006/6749] done
        rfsa_form .... CFG = 1148/16034, LR(0) = 6328/72018, NDFSA = 13232/327314 99 cycles over 1212 states  -- 12218/283452, DFSA = 8040/41936, FSA = 5499/34279 -- 5376/25601, GEN_C[13487/12279] done
        gen parser .... (arity=1, clause_nb=1696, degree=27 for clause #336, size=15618) done
17.52user 0.70system 0:18.22elapsed 99%CPU (0avgtext+0avgdata 0maxresident)k
0inputs+0outputs (455major+56737minor)pagefaults 0swaps
boullier@bourbon /parseurs/TAG/spec 
Wed Feb 13, 17:50:51 time rcg -no_bnf -no_check_set -simple -h 1 -sr -lr0 -tag\
>                                        -lfsa ../src/xtag3_rcg_1rcg_sr_lr0_tag_lfsa.h\
>                                        -rfsa ../src/xtag3_rcg_1rcg_sr_lr0_tag_rfsa.h\
>                                        xtag3_rcg.1rcg > /dev/null
Release 3.9 of tomorrow
xtag3_rcg.1rcg:
        Semantic Pass .... done
        Checking .... done
        First & last .... done
        lfsa_form .... CFG = 1148/16034, LR(0) = 8021/145131, NDFSA = 17613/596043 9 cycles over 3838 states  -- 13793/137148, DFSA = 20111/180583, FSA = 6679/71619 -- 6672/68116, GEN_C[102091/10342] done
        rfsa_form .... CFG = 1148/16034, LR(0) = 6328/72018, NDFSA = 13232/327314 99 cycles over 1212 states  -- 12218/280197, DFSA = 10976/84289, FSA = 8104/74472 -- 7958/67348, GEN_C[71936/15588] done
        gen parser .... (arity=1, clause_nb=1696, degree=27 for clause #336, size=15618) done
40.07user 0.76system 0:40.83elapsed 99%CPU (0avgtext+0avgdata 0maxresident)k
0inputs+0outputs (455major+66601minor)pagefaults 0swaps

boullier@bourbon /parseurs/TAG/spec 
Wed Feb 13, 17:51:31 time rcg -no_bnf -no_check_set -simple -h 1 -lr0\
>                                        -lfsa ../src/xtag3_rcg_2var_1rcg_2var_lr0_lfsa.h\
>                                        -rfsa ../src/xtag3_rcg_2var_1rcg_2var_lr0_rfsa.h\
>                                        xtag3_rcg_2var_1rcg.2var > /dev/null
Release 3.9 of tomorrow
xtag3_rcg_2var_1rcg.2var:
        Semantic Pass .... done
        Checking .... done
        First & last .... done
        lfsa_form .... CFG = 4016/12447, LR(0) = 7042/332452, NDFSA = 7041/320785 9 cycles over 4520 states  -- 2539/33048, DFSA = 2285/13295, FSA = 1994/11124 -- 1920/9384, GEN_C[5827/3935] done
        rfsa_form .... CFG = 4016/12447, LR(0) = 6987/582308, NDFSA = 6986/143439 9 cycles over 1442 states  -- 5562/102882, DFSA = 4247/20857, FSA = 3678/16195 -- 3485/13846, GEN_C[5794/8191] done
        gen parser .... (arity=1, clause_nb=4251, degree=3 for clause #3, size=13824) done
24.89user 0.46system 0:25.36elapsed 99%CPU (0avgtext+0avgdata 0maxresident)k
0inputs+0outputs (455major+35026minor)pagefaults 0swaps
boullier@bourbon /parseurs/TAG/spec 
Wed Feb 13, 17:54:06 time rcg -no_bnf -no_check_set -simple -h 1 -lr0 -tag\
>                                        -lfsa ../src/xtag3_rcg_2var_1rcg_2var_lr0_tag_lfsa.h\
>                                        -rfsa ../src/xtag3_rcg_2var_1rcg_2var_lr0_tag_rfsa.h\
>                                        xtag3_rcg_2var_1rcg.2var > /dev/null
Release 3.9 of tomorrow
xtag3_rcg_2var_1rcg.2var:
        Semantic Pass .... done
        Checking .... done
        First & last .... done
        lfsa_form .... CFG = 4016/12447, LR(0) = 7042/332452, NDFSA = 7041/320785 9 cycles over 4520 states  -- 2539/32110, DFSA = 2056/41178, FSA = 1601/32731 -- 1573/31617, GEN_C[73569/2269] done
        rfsa_form .... CFG = 4016/12447, LR(0) = 6987/582308, NDFSA = 6986/143439 9 cycles over 1442 states  -- 5562/100444, DFSA = 2553/19888, FSA = 2124/17175 -- 2002/15555, GEN_C[17882/3291] done
        gen parser .... (arity=1, clause_nb=4251, degree=3 for clause #3, size=13824) done
27.67user 0.34system 0:28.10elapsed 99%CPU (0avgtext+0avgdata 0maxresident)k
0inputs+0outputs (455major+35599minor)pagefaults 0swaps
boullier@bourbon /parseurs/TAG/spec 
Wed Feb 13, 17:54:34 time rcg -no_bnf -no_check_set -simple -h 1 -sr -lr0\
>                                        -lfsa ../src/xtag3_rcg_2var_1rcg_2var_sr_lr0_lfsa.h\
>                                        -rfsa ../src/xtag3_rcg_2var_1rcg_2var_sr_lr0_rfsa.h\
>                                        xtag3_rcg_2var_1rcg.2var > /dev/null
Release 3.9 of tomorrow
xtag3_rcg_2var_1rcg.2var:
        Semantic Pass .... done
        Checking .... done
        First & last .... done
        lfsa_form .... CFG = 4016/12447, LR(0) = 3059/334081, NDFSA = 39235/397804 9 cycles over 17488 states  -- 21765/68815, DFSA = 4463/17320, FSA = 3658/14334 -- 3542/12521, GEN_C[6593/6363] done
        rfsa_form .... CFG = 4016/12447, LR(0) = 3038/576317, NDFSA = 57355/830419 34 cycles over 25276 states  -- 32147/715548, DFSA = 10423/40768, FSA = 4907/18634 -- 4503/17145, GEN_C[7003/10467] done
        gen parser .... (arity=1, clause_nb=4251, degree=3 for clause #3, size=13824) done
32.07user 2.99system 0:46.95elapsed 74%CPU (0avgtext+0avgdata 0maxresident)k
0inputs+0outputs (455major+301565minor)pagefaults 0swaps
boullier@bourbon /parseurs/TAG/spec 
Wed Feb 13, 17:55:21 time rcg -no_bnf -no_check_set -simple -h 1 -sr -lr0 -tag\
>                                        -lfsa ../src/xtag3_rcg_2var_1rcg_2var_sr_lr0_tag_lfsa.h\
>                                        -rfsa ../src/xtag3_rcg_2var_1rcg_2var_sr_lr0_tag_rfsa.h\
>                                        xtag3_rcg_2var_1rcg.2var > /dev/null
Release 3.9 of tomorrow
xtag3_rcg_2var_1rcg.2var:
        Semantic Pass .... done
        Checking .... done
        First & last .... done
        lfsa_form .... CFG = 4016/12447, LR(0) = 3059/334081, NDFSA = 39235/397804 9 cycles over 17488 states  -- 21765/67893, DFSA = 6693/89724, FSA = 5005/71258 -- 4965/69638, GEN_C[155866/5661] done
        rfsa_form .... CFG = 4016/12447, LR(0) = 3038/576317, NDFSA = 57355/830419 34 cycles over 25276 states  -- 32147/713516, DFSA = 5975/36064, FSA = 4036/26571 -- 3788/24553, GEN_C[24315/5576] done
        gen parser .... (arity=1, clause_nb=4251, degree=3 for clause #3, size=13824) done
44.80user 3.10system 0:50.90elapsed 94%CPU (0avgtext+0avgdata 0maxresident)k
0inputs+0outputs (455major+304948minor)pagefaults 0swaps


#endif /* #if 0 */

static void
print_bnf_t (SXINT t)
{
  char	c, *p1, *p;

  if (t > max_ste) {
    fprintf (out_file, "%ld", t-max_ste-1);
  }

  p1 = sxstrget (t2ste [t]);

  if (*p1 == '"')
    p1++;

  fputs ("\"", out_file);

  while ((p = strpbrk (p1, "\"\\\
")) != NULL) {
    c = *p;
    *p = SXNUL;
    fprintf (out_file, "%s", p1);
    *p = c;

    if (c == '"') {
      fputs ("\\\"", out_file);
    }
    else {
      if (c == '\\')
         fputs ("\\\\", out_file);
      else
         fputs ("\\\
", out_file);
    }

    p1 = p;
  }

  fprintf (out_file, "%s\"", p1);
}


static void
rcg2bnf (bool is_print)
{
  SXINT clause, lhs_prdct, bot2, lhs_nt, lhs_param, rhs, top, bot, cur, rhs_prdct, rhs_nt, rhs_param, l;
  SXINT symb, top3, bot3, cur3, prod_no, len, lognt;
  SXINT *var2nt, *var2strlen;
  XH_header prod_hd;

  sxinitialise(bot3);  /* pour faire taire gcc -Wuninitialized */
  sxinitialise(top3);  /* pour faire taire gcc -Wuninitialized */
  lognt = sxlast_bit (max_nt);

  XH_alloc (&prod_hd, "prod_hd", last_clause+1, 1, max_rhs_prdct+1, NULL, statistics);
  var2nt = (SXINT*) sxalloc (max_gvar+1, sizeof (SXINT));
  var2strlen = (SXINT*) sxalloc (max_gvar+1, sizeof (SXINT));

  for (cur = 0; cur <= max_gvar; cur++)
    var2strlen [cur] = -1; /* pas de strlen */

  for (clause = 1; clause <= last_clause; clause++) {
    if (clause2identical [clause] == clause
	&& !SXBA_bit_is_set (false_clause_set, clause)
	&& !SXBA_bit_is_set (loop_clause_set, clause)) {
      lhs_prdct = XxY_X (rcg_clauses, clause) & lhs_prdct_filter;
      bot2 = XH_X (rcg_predicates, lhs_prdct);
      lhs_nt = XH_list_elem (rcg_predicates, bot2+1);
      lhs_param = XH_list_elem (rcg_predicates, bot2+2);

      rhs = XxY_Y (rcg_clauses, clause);

      if (rhs != 0) {
	top = XH_X (rcg_rhs, rhs+1);
	bot = cur = XH_X (rcg_rhs, rhs);

	for (cur = bot; cur < top; cur++) {
	  rhs_prdct = XH_list_elem (rcg_rhs, cur);
	  bot2 = XH_X (rcg_predicates, rhs_prdct);
	  rhs_nt = XH_list_elem (rcg_predicates, bot2+1);

	  if (rhs_nt > 0) {
	    rhs_param = XH_list_elem (rcg_predicates, bot2+2);
	    symb = XH_list_elem (rcg_parameters, XH_X (rcg_parameters, rhs_param));
	    var2nt [symb] = rhs_nt;
	  }
	  else {
	    /* Le 20/6/2002 les &Strlen (p, X) produisent sur A(X) le
	       non-terminal etendu <A>&p */
	    /* ATTENTION : les 2 clauses
	       A(X Y) --> B(X) C(Y) &Strlen (3, X) .
	       et
	       A(X Y) --> B(X) C(Y) &Strlen (3, Y) .
	       doivent produire des prod # */
	    if (rhs_nt == STRLEN_ic) {
	      /* la grammaire etant simple, on est sur que X est l'arg d'un A */
	      len = XH_list_elem (rcg_parameters, XH_X (rcg_parameters, XH_list_elem (rcg_predicates, bot2+2)));
	      symb = XH_list_elem (rcg_parameters, XH_X (rcg_parameters, XH_list_elem (rcg_predicates, bot2+3)));
	      var2strlen [symb] = len;
	    }
	  }
	}
      }

      XH_push (prod_hd, lhs_nt);

      if (lhs_param > 0) {
	top3 = XH_X (rcg_parameters, lhs_param+1);
	bot3 = XH_X (rcg_parameters, lhs_param);

	for (cur3 = is_bnf_in_reverse ? top3-1 : bot3;
	     is_bnf_in_reverse ? bot3 <= cur3 : cur3 < top3;
	     is_bnf_in_reverse ? cur3-- : cur3++) {
	  symb = XH_list_elem (rcg_parameters, cur3);

	  if (symb < 0)
	    XH_push (prod_hd, symb);
	  else {
	    if ((len = var2strlen [symb]) >= 0) {
	      /* cas xnt */
	      XH_push (prod_hd, ((1+len)<<lognt)+var2nt [symb]);
	    }
	    else
	      XH_push (prod_hd, var2nt [symb]);
	  }
	}
      }

      if (!XH_set (&prod_hd, &prod_no)) {
	/* Nouvelle production */
	if (is_print)
	  fprintf (out_file, "<%s> = ", sxstrget (nt2ste [lhs_nt]));
	else {
	  nt2lhs_nb [lhs_nt]++;
	  bnf.nbpro++;
	  /* prod_no == bnf.nbpro */
	  bnf.action [bnf.nbpro] = clause;
	  bnf.reduc [bnf.nbpro] = lhs_nt;
	  bnf.prolon [bnf.nbpro] = ++bnf.indpro;
	}

	if (lhs_param > 0) {
	  l = 0;

	  for (cur3 = is_bnf_in_reverse ? top3-1 : bot3;
	       is_bnf_in_reverse ? bot3 <= cur3 : cur3 < top3;
	       is_bnf_in_reverse ? cur3-- : cur3++) {
	    symb = XH_list_elem (rcg_parameters, cur3);

	    if (symb < 0) {
	      if (is_print) {
		print_bnf_t (-symb);
		fputs (" ", out_file);
	      }
	      else {
		bnf.maxnpd++;
		l++;
		bnf.lispro [bnf.indpro] = symb;
	      }
	    }
	    else {
	      rhs_nt = var2nt [symb];

	      if (is_print) {
		fprintf (out_file, "<%s>", sxstrget (nt2ste [rhs_nt]));

		if ((len = var2strlen [symb]) >= 0) {
		  fprintf (out_file, "&%ld", (SXINT) len);
		}

		fputs (" ", out_file);
	      }
	      else {
		bnf.maxnpd++;
		l++;
		nt2rhs_nb [rhs_nt]++;
		bnf.lispro [bnf.indpro] = rhs_nt;
	      }

	      var2strlen [symb] = -1;
	    }

	    if (!is_print) {
	      bnf.prolis [bnf.indpro] = bnf.nbpro;
	      bnf.indpro++;
	    }
	  }

	  if (!is_print) {
	    if (l > bnf.rhs_lgth)
	      bnf.rhs_lgth = l;
	  }
	}

	if (is_print)
	  fprintf (out_file, "; * %ld\n", (SXINT) clause);
	else {
	  bnf.lispro [bnf.indpro] = 0;
	  bnf.prolis [bnf.indpro] = bnf.nbpro;
	}
      }
      else {
	if (lhs_param > 0) {
	  for (cur3 = bot3; cur3 < top3; cur3++) {
	    symb = XH_list_elem (rcg_parameters, cur3);

	    if (symb > 0) {
	      var2strlen [symb] = -1;
	    }
	  }
	}
      }
    }
  }

  sxfree (var2nt);
  sxfree (var2strlen);
  XH_free (&prod_hd);
}


static void
rcg2Lex_file (void)
{
  SXINT clause, lhs_prdct, bot2, lhs_nt, lhs_param, rhs_nt_nb, rhs, top, bot, cur, rhs_prdct, rhs_nt, rhs_param, rhs_param2;
  SXINT symb, top3, bot3, cur3, rhs_nt1, prod_no, pos, x, y, len, lognt;
  SXINT *var2nt, *var2strlen, *var2pos;
  SXINT *prod2rhs_stack_map, *rcg2Lex_file_rhs_stack_map, rcg2Lex_file_rhs_stack_map_top;
  XH_header prod_hd;
  SXINT litem, lgth, nb, t, nbpro, indpro, mclause;
  SXINT *clause2prod, *prod2clause, *prolon;
  SXINT *clause2streq, *streq_list;
  SXBA *rcg2Lex_file_clause2lex_set, *Lex2prod_item_set, *prod2multiple_Lex_set, multiple_Lex_item_set, must_check_identical_prod_set;
  SXBA has_Lex;
  SXBA identical_clause_has_non_identical_Lex, rcg2Lex_file_loop_clause_set;
  bool has_lex, has_identical_Lex, is_map_needed, has_strlen, has_streq;

  SXINT *clause2idem_disp, *clause2idem, *idem2next, itop;

  sxinitialise(rhs_param); /* pour faire taire gcc -Wuninitialized */
  sxinitialise(bot3); /* pour faire taire gcc -Wuninitialized */
  sxinitialise(top3); /* pour faire taire gcc -Wuninitialized */
  sxinitialise(rhs_nt1); /* pour faire taire gcc -Wuninitialized */
  lognt = sxlast_bit (max_nt);

  XH_alloc (&prod_hd, "prod_hd", last_clause+1, 1, max_rhs_prdct+1, NULL, statistics);
  var2nt = (SXINT*) sxalloc (max_gvar+1, sizeof (SXINT));
  var2strlen = (SXINT*) sxalloc (max_gvar+1, sizeof (SXINT));

  for (cur = 0; cur <= max_gvar; cur++)
    var2strlen [cur] = -1; /* pas de strlen */

  var2pos = (SXINT*) sxalloc (max_gvar+1, sizeof (SXINT));

  /* Attention, en bnf seule la prod qui est la representante de clauses identiques (aux predicats predefinis prets)
     est generee.  Il lui sera associe des &Lex que s'ils sont tous identiques.
     Les &Lex des clauses identiques seront verifiee au cours du mapping prod->clause */
  clause2prod = (SXINT*) sxcalloc (last_clause+1, sizeof (SXINT));
  prod2clause = (SXINT*) sxcalloc (last_clause+1, sizeof (SXINT));
  prolon = (SXINT*) sxalloc (last_clause+2, sizeof (SXINT));
  prolon [0] = 0;
  prolon [nbpro = 1] = indpro = 4;
  has_Lex = sxba_calloc (last_clause+1);
  rcg2Lex_file_clause2lex_set = sxbm_calloc (last_clause+1, max_t+2);

  clause2idem_disp = (SXINT*) sxcalloc (last_clause+2, sizeof (SXINT));
  clause2idem = (SXINT*) sxalloc (last_clause+1, sizeof (SXINT));
  idem2next = (SXINT*) sxalloc (last_clause+1, sizeof (SXINT));

  rcg2Lex_file_loop_clause_set = sxba_calloc (last_clause+1);

  prod2rhs_stack_map = (SXINT*) sxalloc (last_clause+1, sizeof (SXINT));
  rcg2Lex_file_rhs_stack_map = (SXINT*) sxalloc ((max_gvar+1)*last_clause+1, sizeof (SXINT));
  rcg2Lex_file_rhs_stack_map_top = 1;

  clause2streq = (SXINT*) sxcalloc (last_clause+1, sizeof (SXINT));
  S1_alloc (streq_list, last_clause+1);
  has_strlen = has_streq = false;

  itop = 1;

  for (clause = 1; clause <= last_clause; clause++) {
    if (clause2identical [clause] == clause && !SXBA_bit_is_set (false_clause_set, clause)) {
      lhs_prdct = XxY_X (rcg_clauses, clause) & lhs_prdct_filter;
      bot2 = XH_X (rcg_predicates, lhs_prdct);
      lhs_nt = XH_list_elem (rcg_predicates, bot2+1);
      lhs_param = XH_list_elem (rcg_predicates, bot2+2);
      rhs_nt_nb = 0;
      lex_set = rcg2Lex_file_clause2lex_set [clause];

      rhs = XxY_Y (rcg_clauses, clause);

      if (rhs != 0) {
	top = XH_X (rcg_rhs, rhs+1);
	bot = cur = XH_X (rcg_rhs, rhs);
	pos = 0;

	for (cur = bot; cur < top; cur++) {
	  rhs_prdct = XH_list_elem (rcg_rhs, cur);
	  bot2 = XH_X (rcg_predicates, rhs_prdct);
	  rhs_nt = XH_list_elem (rcg_predicates, bot2+1);

	  if (rhs_nt > 0) {
	    rhs_nt1 = rhs_nt;
	    rhs_param = XH_list_elem (rcg_predicates, bot2+2);
	    symb = XH_list_elem (rcg_parameters, XH_X (rcg_parameters, rhs_param));
	    var2nt [symb] = rhs_nt;
	    rhs_nt_nb++;
	    var2pos [symb] = ++pos;
	  }
	  else {
	    if (rhs_nt == LEX_ic) {
	      SXBA_1_bit (has_Lex, clause);
	      rhs_param2 = XH_list_elem (rcg_predicates, bot2+2);
	      top3 = XH_X (rcg_parameters, rhs_param2+1);

	      for (cur3 = XH_X (rcg_parameters, rhs_param2); cur3 < top3; cur3++) {
		symb = -XH_list_elem (rcg_parameters, cur3);
		SXBA_1_bit (lex_set, symb);
	      }
	    }
	    else {
	      if (rhs_nt == STREQLEN_ic || rhs_nt == STREQ_ic) {
		has_streq = true;
	      }
	      else {
		if (rhs_nt == STRLEN_ic) {
		  has_strlen = true;
		  /* la grammaire etant simple, on est sur que X est l'arg d'un A */
		  len = XH_list_elem (rcg_parameters, XH_X (rcg_parameters, XH_list_elem (rcg_predicates, bot2+2)));
		  symb = XH_list_elem (rcg_parameters, XH_X (rcg_parameters, XH_list_elem (rcg_predicates, bot2+3)));
		  var2strlen [symb] = len;
		}
	      }
	    }
	  }
	}

	if (has_streq) {
	  has_streq = false;
	  clause2streq [clause] = S1_top (streq_list)+1;

	  for (cur = bot; cur < top; cur++) {
	    rhs_prdct = XH_list_elem (rcg_rhs, cur);
	    bot2 = XH_X (rcg_predicates, rhs_prdct);
	    rhs_nt = XH_list_elem (rcg_predicates, bot2+1);
		
	    if (rhs_nt == STREQLEN_ic || rhs_nt == STREQ_ic) {
	      symb = XH_list_elem (rcg_parameters, XH_X (rcg_parameters, XH_list_elem (rcg_predicates, bot2+2)));
	      pos = var2pos [symb];

	      if (rhs_nt == STREQ_ic)
		pos = -pos;

	      S1_push (streq_list, pos);
	      
	      symb = XH_list_elem (rcg_parameters, XH_X (rcg_parameters, XH_list_elem (rcg_predicates, bot2+3)));
	      pos = var2pos [symb];

	      if (rhs_nt == STREQ_ic)
		pos = -pos;

	      S1_push (streq_list, pos);
	    }
	  }

	  S1_push (streq_list, 0); /* Fin de liste */
	}
      }

      if (!(rhs_nt_nb == 1 && lhs_nt == rhs_nt1 && lhs_param == rhs_param)) {
	/* pas loop */
	XH_push (prod_hd, lhs_nt);

	if (lhs_param > 0) {
	  top3 = XH_X (rcg_parameters, lhs_param+1);
	  bot3 = XH_X (rcg_parameters, lhs_param);

	  for (cur3 = bot3;
	       cur3 < top3;
	       cur3++) {
	    symb = XH_list_elem (rcg_parameters, cur3);

	    if (symb < 0)
	      XH_push (prod_hd, symb);
	    else {
	      if ((len = var2strlen [symb]) >= 0) {
		/* cas xnt */
		XH_push (prod_hd, ((1+len)<<lognt)+var2nt [symb]);
		var2strlen [symb] = -1;
	      }
	      else
		XH_push (prod_hd, var2nt [symb]);
	    }
	  }
	}

	if (!XH_set (&prod_hd, &prod_no)) {
	  /* Nouvelle prod */
	  clause2prod [clause] = nbpro; /* nbpro == prod_no */
	  prod2clause [nbpro] = clause;
	  prolon [++nbpro] = indpro += XH_list_lgth (prod_hd, prod_no); /* On compte le 0 en fin de lispro... */

	  if (lhs_param > 0) {
	    prod2rhs_stack_map [prod_no] = rcg2Lex_file_rhs_stack_map_top;
	    pos = 0;
	    is_map_needed = false;

	    for (cur3 = bot3;
		 cur3 < top3;
		 cur3++) {
	      symb = XH_list_elem (rcg_parameters, cur3);

	      if (symb > 0) {
		rcg2Lex_file_rhs_stack_map [rcg2Lex_file_rhs_stack_map_top++] = var2pos [symb];

		if (var2pos [symb] != ++pos)
		  is_map_needed = true;
	      }
	    }

	    if (is_map_needed)
	      rcg2Lex_file_rhs_stack_map [rcg2Lex_file_rhs_stack_map_top++] = 0;
	    else {
	      rcg2Lex_file_rhs_stack_map_top = prod2rhs_stack_map [prod_no];
	      prod2rhs_stack_map [prod_no] = 0;
	    }
	  }
	  else
	    prod2rhs_stack_map [prod_no] = 0;
	}
	else {
	  /* Ancienne prod */
	  /* On construit la classe d'equivalence */
	  mclause = prod2clause [prod_no];
	  clause2idem [itop] = clause;
	  idem2next [itop] = clause2idem_disp [mclause];
	  clause2idem_disp [mclause] = itop++;
	}
      }
      else
	SXBA_1_bit (rcg2Lex_file_loop_clause_set, clause);
    }
  }

  /* Pour faire une verif dynamique de la coherence entre Ca et l'analyseur earley/LC qui sera construit */
  fprintf (out_file, "#define rcg_itemmax %ld\n", (SXINT) indpro-1);

  /* La bnf generee a des predicats (&Strlen (p, X) a produit le xnt <A>&p si on a A(X) ds la clause) */
  /* Les Strlen sont donc verifies par un appel de predicat lors de la reconnaissance */ 
  fprintf (out_file, "#define has_strlen %ld\n", (SXINT) (has_strlen ? 1 : 0));
  fprintf (out_file, "#define has_streq %ld\n", (SXINT) (S1_is_empty (streq_list) ? 0 : 1));

  fprintf (out_file, "static SXINT prod2clause [%ld] = {0,", (SXINT) nbpro);

  for (prod_no = 1; prod_no < nbpro; prod_no++) {
    if (prod_no % 10 == 1)
      fprintf (out_file, "\n/* %ld */ ", (SXINT) prod_no);

    fprintf (out_file, "%ld, ", (SXINT) prod2clause [prod_no]);
  }

  fputs ("};\n", out_file);
  
  fprintf (out_file, "\nstatic SXINT clause2identical_disp [%ld] = {1,\n", (SXINT) last_clause+2);
  
  top = 1;

  for (clause = 1; clause <= last_clause; clause++) {
    fprintf (out_file, "/* %ld */ %ld,\n", (SXINT) clause, (SXINT) top);

    if ((cur = clause2idem_disp [clause])) {
      do {
	top++;
      } while ((cur = idem2next [cur]) > 0);
    }
  }

  fprintf (out_file, "/* dummy %ld */ %ld,\n", (SXINT) clause, (SXINT) top);
  fputs ("};\n", out_file);
  
  fprintf (out_file, "\nstatic SXINT clause2identical [%ld] = {0,\n", (SXINT) top);
  
  bot = 1;

  for (clause = 1; clause <= last_clause; clause++) {

    if ((cur = clause2idem_disp [clause])) {
      fprintf (out_file, "/* %ld */ ", (SXINT) clause);

      do {
	fprintf (out_file, "%ld, ", (SXINT) clause2idem [cur]);
	bot++;
      } while ((cur = idem2next [cur]) > 0);

      fputs ("\n", out_file);
    }
  }

  fputs ("};\n", out_file);

  sxfree (prod2clause);

  sxfree (var2nt);
  sxfree (var2strlen);
  sxfree (var2pos);
  XH_free (&prod_hd);

  /* Lex2prod_item_set [0] = ensemble des items des prod n'ayant pas de Lex */
  /* Lex2prod_item_set [t] = ensemble des items des prod ayant &Lex(t) */
  Lex2prod_item_set = sxbm_calloc (max_t+1, indpro);
  /* prod2multiple_Lex_set [prod] = ensemble non singleton des Lex de prod */
  prod2multiple_Lex_set = sxbm_calloc (nbpro, max_t+2);
  /* Ensemble des items des prod ayant des Lex multiples */
  multiple_Lex_item_set = sxba_calloc (indpro);
  /* Ensemble des representants des prod identiques pour lesquelles il faut faire la verif des Lex
     apres reconnaissance du representant */
  must_check_identical_prod_set = sxba_calloc (nbpro);

  identical_clause_has_non_identical_Lex = sxba_calloc (last_clause+1);

  for (clause = 1; clause <= last_clause; clause++) {
    if ((prod_no = clause2prod [clause]) != 0) {
      /* clause est le representant (peut etre trivial) de la classe d'equivalence des clauses identiques */
      has_identical_Lex = true; /* A priori */

      if ((cur = clause2idem_disp [clause])) {
	/* classe multiple */
	/* On regarde si toutes les clauses de la classe ont les memes Lex */
	has_lex = SXBA_bit_is_set (has_Lex, clause);
	lex_set = rcg2Lex_file_clause2lex_set [clause];

	do {
	  mclause = clause2idem [cur];

	  if (has_lex != SXBA_bit_is_set (has_Lex, mclause)
	      || sxba_first_difference (lex_set, rcg2Lex_file_clause2lex_set [mclause]) != -1) {
	    has_identical_Lex = false;
	    break;
	  }
	} while ((cur = idem2next [cur]) > 0);

	/* MODIF le 04/12/2002 */
	/* Si une clause n'a pas de lex et si sa prod associee est verifiee, il n'est pas necessaire de
	   verifier les Lex de la prod associee.  Cependant, a un moment donne, on aura besoin de verifier
	   ces Lex, avant de revenir aux RCG.  On decide donc de noter le fait qu'on a besoin de qqchose de special */

	if (!has_identical_Lex)
	  SXBA_1_bit (must_check_identical_prod_set, prod_no);

#if 0
	/* A l'analyse, pour qu'une classe d'equivalence soit validee
	   il faut que l'un au moins des membres soit valide.  Cette verification doit donc etre faite
	   ssi tous les membres ont des Lex associes.  Il faut donc noter ce cas pour lancer cette verif */

	if (!has_identical_Lex) {
	  /* Pour l'analyse "normale" tout se passe comme si les prod n'avaient pas de Lex,
	     leur verif eventuelle sera faite lors de la reconnaissance du representant */
	  bool	must_check_identical_prod = true;

	  if (!SXBA_bit_is_set (has_Lex, clause))
	    must_check_identical_prod = false;

	  SXBA_1_bit (identical_clause_has_non_identical_Lex, clause);

	  cur = clause2idem_disp [clause];

	  do {
	    mclause = clause2idem [cur];

	    if (!SXBA_bit_is_set (has_Lex, mclause))
	      must_check_identical_prod = false;

	    SXBA_1_bit (identical_clause_has_non_identical_Lex, mclause);
	  } while ((cur = idem2next [cur]) > 0);

	  if (must_check_identical_prod)
	    /* Contient les representants des prod identiques pour lesquelles il faut faire
	       la verif des Lex apres reconnaissance */
	    SXBA_1_bit (must_check_identical_prod_set, prod_no);
	}
	/* else
	   Ici la verif (eventuelle)  se fait comme ds le cas trivial */
#endif /* 0 */
      }

      litem = prolon [prod_no];
      lgth = prolon [prod_no+1]-litem;

      if (SXBA_bit_is_set (has_Lex, clause) && has_identical_Lex) {
	/* y-a du Lex qui sera verifie au cours de l'analyse normale*/
	SXBA_0_bit (has_Lex, clause);
	lex_set = rcg2Lex_file_clause2lex_set [clause];
	t = 0;
	nb = 0;

	while ((t = sxba_scan (lex_set, t)) > 0) {
	  sxba_range (Lex2prod_item_set [t], litem, lgth);
	  nb++;
	}

	if (nb > 1) {
	  sxba_copy (prod2multiple_Lex_set [prod_no], lex_set);
	  sxba_range (multiple_Lex_item_set, litem, lgth);
	}
      }
      else {
	/* Pas de Lex */
	/* prod_no est selectionne inconditionnellement */
	sxba_range (Lex2prod_item_set [0], litem, lgth);
      }

      if ((cur = clause2idem_disp [clause])) {
	/* Ds tous les cas, on raz le has_Lex des clauses qui ne sont pas des representants */
	do {
	  mclause = clause2idem [cur];
	  SXBA_0_bit (has_Lex, mclause);
	} while ((cur = idem2next [cur]) > 0);
      }
    }
  }

  for (t = 1; t <= max_t; t++) {
    if (sxba_is_empty (Lex2prod_item_set [t]))
      Lex2prod_item_set [t] = NULL;
  }

  for (prod_no = 1; prod_no < nbpro; prod_no++) {
    if (sxba_is_empty (prod2multiple_Lex_set [prod_no]))
      prod2multiple_Lex_set [prod_no] = NULL;
  }

  for (clause = 1; clause <= last_clause;clause++) {
    if (SXBA_bit_is_set (identical_clause_has_non_identical_Lex, clause)
	|| SXBA_bit_is_set (rcg2Lex_file_loop_clause_set, clause)) {
      if (sxba_is_empty (rcg2Lex_file_clause2lex_set [clause]))
	rcg2Lex_file_clause2lex_set [clause] = NULL;
    }
    else {
      rcg2Lex_file_clause2lex_set [clause] = NULL;
    }
  }

  sxfree (clause2idem_disp);
  sxfree (clause2idem);
  sxfree (idem2next);

  sxfree (clause2prod);
  sxfree (prolon);

  fputs ("\n#if has_Lex\n", out_file);
  sxbm3_to_c (out_file, Lex2prod_item_set, max_t+1, "Lex2prod_item_set", "", true /* is_static */);
  sxbm3_to_c (out_file, prod2multiple_Lex_set, nbpro, "prod2multiple_Lex_set", "", true /* is_static */);
  sxba_to_c (multiple_Lex_item_set, out_file, "multiple_Lex_item_set", "", true /* static */);
  sxba_to_c (must_check_identical_prod_set, out_file, "must_check_identical_prod_set", "", true /* static */);

  fprintf (out_file, "#define last_clause\t%ld\n\n", (SXINT) last_clause);
  /* Ici has_Lex[clause]==true <==> clause est le representant non trivial de clauses equivalentes ayant des Lex
     non identiques */
  sxba_to_c (has_Lex, out_file, "clause_has_Lex", "", true /* static */);
  fputs ("#endif /* has_Lex */\n", out_file);
  
  if (sxba_is_empty (rcg2Lex_file_loop_clause_set))
    fputs ("static SXBA rcg2Lex_file_loop_clause_set\n", out_file);
  else
    sxba_to_c (rcg2Lex_file_loop_clause_set, out_file, "rcg2Lex_file_loop_clause_set", "", true /* static */);

  fputs ("\n#if has_Lex\n", out_file);
  sxbm3_to_c (out_file, rcg2Lex_file_clause2lex_set, last_clause+1, "rcg2Lex_file_clause2lex_set", "", true /* is_static */);
  fputs ("#endif /* has_Lex */\n", out_file);

  fputs ("\n#if is_full_guide\n", out_file);

  fputs ("static SXINT prod2rhs_stack_map [] = {0,", out_file);

  for (prod_no = 1; prod_no < nbpro; prod_no++) {
    if (prod_no % 10 == 1)
      fprintf (out_file, "\n/* %ld */ ", (SXINT) prod_no);

    fprintf (out_file, "%ld, ", (SXINT) prod2rhs_stack_map [prod_no]);
  }

  fputs ("\n};\n\n", out_file);

  fputs ("static SXINT rcg2Lex_file_rhs_stack_map [] = {0,", out_file);

  for (prod_no = 1; prod_no < nbpro; prod_no++) {
    if ((x = prod2rhs_stack_map [prod_no])) {
      fprintf (out_file, "\n/* %ld */ ", (SXINT) prod_no);

      do {
	y = rcg2Lex_file_rhs_stack_map [x++];
	fprintf (out_file, "%ld, ", (SXINT) y);
      } while (y);
    }
  }

  fputs ("\n};\n\n", out_file);

  /* Le 20/6/2002 les &Streq[len] (X, Y) produisent sur la clause des couples (i,j) pour &Streqlen
     ou des couples (-i, -j) pour &Streq ou les i et j referencent les positions en rhs ds la CLAUSE
     des predicats A(X) et B(Y) qui utilisent ces variables.  Ces predicats predefinis ne sont verifies
     par l'analyseur earley que ds le cas fully_coupled. */

  if (!S1_is_empty (streq_list)) {
    fputs ("static SXINT clause2streq [] = {0,", out_file);

    for (clause = 1; clause <= last_clause;clause++) {
      if (clause % 10 == 1)
	fprintf (out_file, "\n/* %ld */ ", (SXINT) clause);

      fprintf (out_file, "%ld, ", (SXINT) clause2streq [clause]);
    }

    fputs ("\n};\n\n", out_file);

    fputs ("static SXINT streq_list [] = {0,", out_file);

    for (clause = 1; clause <= last_clause;clause++) {
      if ((x = clause2streq [clause])) {
	fprintf (out_file, "\n/* %ld */ ", (SXINT) clause);

	while ((y = streq_list [x++]) != 0) {
	  fprintf (out_file, "%ld, ", (SXINT) y);
	}
    
	fputs ("0,", out_file);
      }
    }

    fputs ("\n};\n\n", out_file);
  }

  fputs ("#endif /* is_full_guide */\n", out_file);

  sxfree (prod2rhs_stack_map);
  sxfree (rcg2Lex_file_rhs_stack_map);
  sxbm_free (rcg2Lex_file_clause2lex_set);
  sxfree (has_Lex);
  sxbm_free (Lex2prod_item_set);
  sxbm_free (prod2multiple_Lex_set);
  sxfree (multiple_Lex_item_set);
  sxfree (must_check_identical_prod_set);
  sxfree (identical_clause_has_non_identical_Lex);
  sxfree (rcg2Lex_file_loop_clause_set);
  sxfree (clause2streq);
  S1_free (streq_list);
}


void
lrbnf (void)
{
  if (max_garity != 1 || !is_simple_grammar) {
    if (sxverbosep) {
      fputs ("WARNING: Only simple 1-RCGs can generate BNF grammars.\n", sxtty);
      sxttycol1p = true;
    }

    return;
  }

  if (lbnf_file) {
    if (sxverbosep) {
      fputs ("\tlbnf_form .... ", sxtty);
      sxttycol1p = false;
    }

    out_file = lbnf_file;
    is_bnf_in_reverse = false;

    out_header (bnf_header, lbnf_file_name, "in direct BNF form");

    rcg2bnf (true);

    fclose (out_file);

    if (sxverbosep) {
      fputs ("done\n", sxtty);
      sxttycol1p = true;
    }
  }

  if (rbnf_file) {
    if (sxverbosep) {
      fputs ("\trbnf_form .... ", sxtty);
      sxttycol1p = false;
    }

    out_file = rbnf_file;
    is_bnf_in_reverse = true;

    out_header (bnf_header, rbnf_file_name, "in reverse BNF form");

    rcg2bnf (true);

    fclose (out_file);

    if (sxverbosep) {
      fputs ("done\n", sxtty);
      sxttycol1p = true;
    }
  }

  if (Lex_file) {
    /* On cree un fichier qui contient les structures contenant toutes les infos sur les &Lex necessaires
       pour faire de la lexicalisation (option -Dhas_Lex) lors de l'analyse CF des lbnf/rbnf sortis
       par un analyseur a la Earley */
    if (sxverbosep) {
      fputs ("\tLex_file .... ", sxtty);
      sxttycol1p = false;
    }

    out_file = Lex_file;

    out_header (c_header, Lex_file_name, "the &Lex file");

    rcg2Lex_file ();

    fclose (out_file);

    if (sxverbosep) {
      fputs ("done\n", sxtty);
      sxttycol1p = true;
    }
  }
}

/* ****************************************************************************************************** */
/* TRAITEMENT de l'option "-tig pathname" */
#if 0
#define is_case2 0
/* Sur XTAG, le cas is_case2==1 ne donne rien de plus, qqsoit le nt A, ii y a de vraies wrapping A-clauses */

static SXINT
tag2tig (void)
{
  SXINT clause, lhs_prdct, lhs_bot2, lhs_nt, lhs_arity, lhs_top2, lhs_arg, lhs_cur2, lhs_param, lhs_bot3, lhs_top3, lhs_cur3, symb;
  SXINT rhs, rhs_top, rhs_bot, rhs_cur, rhs_prdct, rhs_bot2, rhs_nt, rhs_arity, left_symb, right_symb;
  SXINT  strong_left_A_nb, strong_right_A_nb, A, left_clause_nb, right_clause_nb, left_var, right_var, modified_clause_nb; 
  bool is_strong_left_clause, is_strong_right_clause, is_strong_left, is_strong_right, done, is_first, has_changed; 
  SXBA *clause2lvar_set, lvar_set, strong_left_clause_set, strong_right_clause_set, A_clause_set, strong_left_A_set, strong_right_A_set;

  modified_clause_nb = 0; /* A priori, aucune transformation possible! */

  clause2lvar_set = sxbm_calloc (last_clause+1, max_gvar+1);
  strong_left_clause_set = sxba_calloc (last_clause+1);
  strong_right_clause_set = sxba_calloc (last_clause+1);
  strong_left_A_set = sxba_calloc (max_nt+1);
  strong_right_A_set = sxba_calloc (max_nt+1);

  for (clause = 1; clause <= last_clause; clause++) {
    if (clause2identical [clause] == clause) {
      lhs_prdct = XxY_X (rcg_clauses, clause) & lhs_prdct_filter;
      lhs_bot2 = XH_X (rcg_predicates, lhs_prdct);
      lhs_nt = XH_list_elem (rcg_predicates, lhs_bot2+1);
      lhs_arity = nt2arity [lhs_nt];
      lvar_set = clause2lvar_set [clause];

      if (lhs_arity == 2) {
	/* On ne s'occupe que des arbres auxiliaires */
	lhs_top2 = XH_X (rcg_predicates, lhs_prdct+1);
	is_strong_left_clause = is_strong_right_clause = true;

	for (lhs_arg = 0, lhs_cur2 = lhs_bot2+2; lhs_cur2 < lhs_top2; lhs_arg++, lhs_cur2++) {
	  lhs_param = XH_list_elem (rcg_predicates, lhs_cur2);

	  if (lhs_param > 0) {
	    lhs_bot3 = XH_X (rcg_parameters, lhs_param);
	    lhs_top3 = XH_X (rcg_parameters, lhs_param+1);

	    for (lhs_cur3 = lhs_bot3; lhs_cur3 < lhs_top3; lhs_cur3++) {
	      symb = XH_list_elem (rcg_parameters, lhs_cur3);

	      if (symb > 0) {
		if (lhs_arg == 0)
		  SXBA_1_bit (lvar_set, symb);
	      }
	      else {
		if (lhs_arg == 0) {
		  is_strong_right_clause = false;
		}
		else {
		  is_strong_left_clause = false;
		}
	      }
	    }
	  }
	}

	if (is_strong_left_clause || is_strong_right_clause) {
	  /* Non bi-lexicale */
	  rhs = XxY_Y (rcg_clauses, clause);
	  rhs_top = XH_X (rcg_rhs, rhs+1);
	  rhs_cur = XH_X (rcg_rhs, rhs);

	  while (rhs_cur < rhs_top) {
	    rhs_prdct = XH_list_elem (rcg_rhs, rhs_cur);
	    rhs_bot2 = XH_X (rcg_predicates, rhs_prdct);
	    rhs_nt = XH_list_elem (rcg_predicates, rhs_bot2+1);

	    if (rhs_nt > 0) {
	      rhs_arity = nt2arity [rhs_nt];
	      left_symb = XH_list_elem (rcg_parameters, XH_X (rcg_parameters, XH_list_elem (rcg_predicates, rhs_bot2+2)));

	      if (rhs_arity == 1) {
		/* Substitution... */
		if (SXBA_bit_is_set (lvar_set, left_symb)) {
		  /* ... a gauche */
		  is_strong_right_clause = false;
		}
		else
		  is_strong_left_clause = false; 
	      }
	      else {
		/* Adjonction ... */
		right_symb = XH_list_elem (rcg_parameters, XH_X (rcg_parameters, XH_list_elem (rcg_predicates, rhs_bot2+3)));

		if (SXBA_bit_is_set (lvar_set, left_symb) && SXBA_bit_is_set (lvar_set, right_symb)) {
		  /* ... dans l'arbre gauche */
		  is_strong_right_clause = false;
		}
		else {
		  if (!SXBA_bit_is_set (lvar_set, left_symb) && !SXBA_bit_is_set (lvar_set, right_symb)) {
		    /* ... dans l'arbre droit */
		    is_strong_left_clause = false;
		  }
		  /*
		  else {
		    ... sur le spine
		  }
		  */
		}
	      }
	    }

	    rhs_cur++;
	  }
	}
	
	if (is_strong_left_clause)
	  SXBA_1_bit (strong_left_clause_set, clause);

	if (is_strong_right_clause)
	  SXBA_1_bit (strong_right_clause_set, clause);

	/* On a (pour l'instant on n'a pas tenu compte du type des noeuds du spine) :
	   lr
	   11 Clause vide ou que des adjonctions sur le spine
	   10 left clause
	   01 right clause
	   00 wrapping clause
	*/
      }
    }
  }

  /* On suppose qu'il y a adjonction a la racine (et pas au pied)
     Il y a deux possibilites : 
     I) Contrainte forte : un arbre est gauche seulement si tous les noeuds du spine (meme la racine) sont gauches ;
                           un arbre est droit seulement si tous les noeuds du spine (meme la racine) sont droits.

     II) Contrainte faible : un arbre est gauche seulement si tous les noeuds du spine autre que la racine sont gauches
                             et si la racine est gauche ou gauche et droite ;
                             un arbre est droit seulement si tous les noeuds du spine autre que la racine sont droits
                             et si la racine est droite ou gauche et droite.

     Le cas I a l'avantage de produire une correspondance un<->un entre TAG et TIG (certaines clauses vides peuvent
        disparaitre.  Cependant, sur XTAG, aucune clause ne beneficie de cette transformation !
     Dans le cas II, si un label A est a la fois gauche et droit les A-clauses gauches et les A-clauses droites sont
     generees differemment, les gauches sont binaires alors que les droites sont unaires, elles deviennent des
     A_right-clauses et il faut generer une A-clause qui fait passer de A a A_right :
           A(, X) --> A_right(X) .
  */

  strong_left_A_nb = strong_right_A_nb = 0;

  for (A = 1; A <= max_nt; A++) {
    if (nt2arity [A] == 2) {
      /* Attention A peut etre a la fois left et right ex: la seule A-clause est vide */
      A_clause_set = lhsnt2clause [A];

      is_strong_left = is_strong_right = true;
      left_clause_nb = right_clause_nb = 0;
      clause = 0;

      while ((clause = sxba_scan (A_clause_set, clause)) > 0) {
	if (clause2identical [clause] == clause) {
#if is_case2
	  /* Si une A-clause est wrapping => A est wrapping */
	  if (!SXBA_bit_is_set (strong_left_clause_set, clause) && !SXBA_bit_is_set (strong_right_clause_set, clause))
	    is_strong_left = is_strong_right = false;
#else /* !is_case2 */
	  if (!SXBA_bit_is_set (strong_left_clause_set, clause))
	    is_strong_left = false;
	  
	  if (!SXBA_bit_is_set (strong_right_clause_set, clause))
	    is_strong_right = false;
#endif /* !is_case2 */

	  if (SXBA_bit_is_set (strong_left_clause_set, clause))
	    left_clause_nb++;
	  
	  if (SXBA_bit_is_set (strong_right_clause_set, clause))
	    right_clause_nb++;
	  
	}
      }

      if (is_strong_left && left_clause_nb)
	SXBA_1_bit (strong_left_A_set, A), strong_left_A_nb++;

      if (is_strong_right && right_clause_nb)
	SXBA_1_bit (strong_right_A_set, A), strong_right_A_nb++;

      /* Si l'on suppose qu'il y a adjonction a la racine, toutes les A-clauses sont wrapping, 
	 on pourrait donc razer ici strong_left_clause_set et strong_right_clause_set
	 mais comme il y a pu y avoir des contraintes d'adjonction, ca sera traite ds le cas general */
    }
  }

  if (strong_left_A_nb+strong_right_A_nb) {
    /* Il faut verifier que les adjonctions sur le spine sont 
       - gauches pour les strong_left_clause et
       - droites pour les strong_right_clause */
    /* Ds le cas is_case2, cette verif ne s'applique pas a la racine si elle n'est pas wrapping */
    do {
      done = true;

      if (strong_left_A_nb) {
	clause = 0;

	while ((clause = sxba_scan (strong_left_clause_set, clause)) > 0) {
	  lvar_set = clause2lvar_set [clause];
	  lhs_prdct = XxY_X (rcg_clauses, clause) & lhs_prdct_filter;
	  lhs_bot2 = XH_X (rcg_predicates, lhs_prdct);
	  lhs_nt = XH_list_elem (rcg_predicates, lhs_bot2+1);
#if is_case2
	  lhs_param = XH_list_elem (rcg_predicates, lhs_bot2+2);
	  left_var = (lhs_param > 0) ? XH_list_elem (rcg_parameters, XH_X (rcg_parameters, lhs_param)) : 0;
	  lhs_param = XH_list_elem (rcg_predicates, lhs_bot2+3);
	  right_var = (lhs_param > 0) ? XH_list_elem (rcg_parameters,  XH_X (rcg_parameters, lhs_param+1)-1) : 0;
#endif /* is_case2 */

	  rhs = XxY_Y (rcg_clauses, clause);
	  rhs_top = XH_X (rcg_rhs, rhs+1);
	  rhs_cur = XH_X (rcg_rhs, rhs);

	  while (rhs_cur < rhs_top) {
	    rhs_prdct = XH_list_elem (rcg_rhs, rhs_cur);
	    rhs_bot2 = XH_X (rcg_predicates, rhs_prdct);
	    rhs_nt = XH_list_elem (rcg_predicates, rhs_bot2+1);

	    if (rhs_nt > 0 && nt2arity [rhs_nt] == 2) {
	      /* Adjonction gauche ou sur le spine */
	      rhs_arity = nt2arity [rhs_nt];

	      if (rhs_arity == 2) {
		left_symb = XH_list_elem (rcg_parameters, XH_X (rcg_parameters, XH_list_elem (rcg_predicates, rhs_bot2+2)));
		right_symb = XH_list_elem (rcg_parameters, XH_X (rcg_parameters, XH_list_elem (rcg_predicates, rhs_bot2+3)));

		if (SXBA_bit_is_set (lvar_set, left_symb) && !SXBA_bit_is_set (lvar_set, right_symb)) {
		  /* Adjonction sur le spine */
#if is_case2
		  /* a la racine ? */
		  if (lhs_nt==rhs_nt && left_var==left_symb && right_var==right_symb) {
		    /* Oui */
		    if (!SXBA_bit_is_set (strong_left_A_set, rhs_nt) && !SXBA_bit_is_set (strong_right_A_set, rhs_nt)) {
		      /* damned, la racine est wrapping */
		      SXBA_0_bit (strong_left_clause_set, clause);
		    }
		  }
		  else
#endif /* is_case2 */
		  /* Elle doit etre gauche */
		  if (!SXBA_bit_is_set (strong_left_A_set, rhs_nt)) {
		    /* Damned, clause n'est pas strong-left donc lhs_nt non-plus, il faudra boucler */
		    SXBA_0_bit (strong_left_clause_set, clause);

		    if (SXBA_bit_is_set_reset (strong_left_A_set, lhs_nt)) {
		      strong_left_A_nb--;
		      done = false;
		    }

		    break;
		  }
		}
	      }
	    }

	    rhs_cur++;
	  }
	}
      }

      if (strong_right_A_nb) {
	clause = 0;

	while ((clause = sxba_scan (strong_right_clause_set, clause)) > 0) {
	  lvar_set = clause2lvar_set [clause];
	  lhs_prdct = XxY_X (rcg_clauses, clause) & lhs_prdct_filter;
	  lhs_bot2 = XH_X (rcg_predicates, lhs_prdct);
	  lhs_nt = XH_list_elem (rcg_predicates, lhs_bot2+1);
#if is_case2
	  lhs_param = XH_list_elem (rcg_predicates, lhs_bot2+2);
	  left_var = (lhs_param > 0) ? XH_list_elem (rcg_parameters, XH_X (rcg_parameters, lhs_param)) : 0;
	  lhs_param = XH_list_elem (rcg_predicates, lhs_bot2+3);
	  right_var = (lhs_param > 0) ? XH_list_elem (rcg_parameters,  XH_X (rcg_parameters, lhs_param+1)-1) : 0;
#endif /* is_case2 */

	  rhs = XxY_Y (rcg_clauses, clause);
	  rhs_top = XH_X (rcg_rhs, rhs+1);
	  rhs_cur = XH_X (rcg_rhs, rhs);

	  while (rhs_cur < rhs_top) {
	    rhs_prdct = XH_list_elem (rcg_rhs, rhs_cur);
	    rhs_bot2 = XH_X (rcg_predicates, rhs_prdct);
	    rhs_nt = XH_list_elem (rcg_predicates, rhs_bot2+1);

	    if (rhs_nt > 0 && nt2arity [rhs_nt] == 2) {
	      /* Adjonction gauche ou sur le spine */
	      rhs_arity = nt2arity [rhs_nt];

	      if (rhs_arity == 2) {
		left_symb = XH_list_elem (rcg_parameters, XH_X (rcg_parameters, XH_list_elem (rcg_predicates, rhs_bot2+2)));
		right_symb = XH_list_elem (rcg_parameters, XH_X (rcg_parameters, XH_list_elem (rcg_predicates, rhs_bot2+3)));

		if (SXBA_bit_is_set (lvar_set, left_symb) && !SXBA_bit_is_set (lvar_set, right_symb)) {
		  /* Adjonction sur le spine */
#if is_case2
		  /* a la racine ? */
		  if (lhs_nt==rhs_nt && left_var==left_symb && right_var==right_symb) {
		    /* Oui */
		    if (!SXBA_bit_is_set (strong_left_A_set, rhs_nt) && !SXBA_bit_is_set (strong_right_A_set, rhs_nt)) {
		      /* damned, la racine est wrapping */
		      SXBA_0_bit (strong_right_clause_set, clause);
		    }
		  }
		  else
#endif /* is_case2 */
		  /* Elle doit etre droite */
		  if (!SXBA_bit_is_set (strong_right_A_set, rhs_nt)) {
		    /* Damned, clause n'est pas strong-right donc lhs_nt non-plus, il faudra boucler */
		    SXBA_0_bit (strong_right_clause_set, clause);

		    if (SXBA_bit_is_set_reset (strong_right_A_set, lhs_nt)) {
		      strong_right_A_nb--;
		      done = false;
		    }

		    break;
		  }
		}
	      }
	    }

	    rhs_cur++;
	  }
	}
      }
    } while (!done);

#if is_case2
    /* Generation non faite car sur XTAG, la transformation TIG est aussi sans effet!! */
#else /* !is_case2 */
    if (strong_left_A_nb+strong_right_A_nb) {
      /* Ici la transformation TIG va avoir de l'effet */

#if 0
      fprintf (sxtty, "left_tig = %ld, right_tig = %ld ",
	       sxba_cardinal (strong_left_clause_set), sxba_cardinal (strong_right_clause_set));
#endif /* 0 */
      out_file = tig_file;

      out_header (rcg_header, tig_file_name, "in \"tig\" form");

      for (clause = 1; clause <= last_clause; clause++) {
	if (clause2visited [clause]->token.comment)
	  fprintf (out_file, "%s", clause2visited [clause]->token.comment);

	lhs_prdct = XxY_X (rcg_clauses, clause) & lhs_prdct_filter;
	lhs_bot2 = XH_X (rcg_predicates, lhs_prdct);
	lhs_nt = XH_list_elem (rcg_predicates, lhs_bot2+1);
	lhs_arity = nt2arity [lhs_nt];
	rhs = XxY_Y (rcg_clauses, clause);
      
	fprintf (out_file, "\n%% %ld: ", clause);
	out_clause (clause);

	/* On supprime les variables "vides" */
	lvar_set = clause2lvar_set [clause];
	lhs_top2 = XH_X (rcg_predicates, lhs_prdct+1);
	
	for (lhs_cur2 = lhs_bot2+2; lhs_cur2 < lhs_top2; lhs_cur2++) {
	  lhs_param = XH_list_elem (rcg_predicates, lhs_cur2);

	  if (lhs_param > 0) {
	    lhs_bot3 = XH_X (rcg_parameters, lhs_param);
	    lhs_top3 = XH_X (rcg_parameters, lhs_param+1);

	    for (lhs_cur3 = lhs_bot3; lhs_cur3 < lhs_top3; lhs_cur3++) {
	      symb = XH_list_elem (rcg_parameters, lhs_cur3);

	      if (symb > 0) {
		SXBA_1_bit (lvar_set, symb);
	      }
	    }
	  }
	}

	rhs_top = XH_X (rcg_rhs, rhs+1);
	rhs_cur = rhs_bot = XH_X (rcg_rhs, rhs);

	while (rhs_cur < rhs_top) {
	  rhs_prdct = XH_list_elem (rcg_rhs, rhs_cur);
	  rhs_bot2 = XH_X (rcg_predicates, rhs_prdct);
	  rhs_nt = XH_list_elem (rcg_predicates, rhs_bot2+1);

	  if (rhs_nt > 0) {
	    if (nt2arity [rhs_nt] == 2) {
	      left_symb = XH_list_elem (rcg_parameters, XH_X (rcg_parameters, XH_list_elem (rcg_predicates, rhs_bot2+2)));
	      right_symb = XH_list_elem (rcg_parameters, XH_X (rcg_parameters, XH_list_elem (rcg_predicates, rhs_bot2+3)));

	      if (SXBA_bit_is_set (strong_left_A_set, rhs_nt))
		SXBA_0_bit (lvar_set, right_symb);

	      if (SXBA_bit_is_set (strong_right_A_set, rhs_nt))
		SXBA_0_bit (lvar_set, left_symb);
	    }
	  }

	  rhs_cur++;
	}

	has_changed = false;

	if (lhs_arity == 2 && SXBA_bit_is_set (strong_left_A_set, lhs_nt) && SXBA_bit_is_set (strong_right_A_set, lhs_nt)) {
	  /* c'est la seule A-clause a la forme "A(, ) --> ;", elle disparait */
	  has_changed = true;
	}
	else {
	  out_nt_text (lhs_nt);
	  fputs ("(", out_file);

	  if (lhs_arity == 1) {
	    /* Initial tree */
	    lhs_param = XH_list_elem (rcg_predicates, lhs_bot2+2);
	  }
	  else {
	    if (SXBA_bit_is_set (strong_left_A_set, lhs_nt)) {
	      /* auxiliary left clause */
	      lhs_arity = 1;
	      lhs_param = XH_list_elem (rcg_predicates, lhs_bot2+2);
	    }
	    else {
	      if (SXBA_bit_is_set (strong_right_A_set, lhs_nt)) {
		/* auxiliary right clause */
		lhs_arity = 1;
		lhs_param = XH_list_elem (rcg_predicates, lhs_bot2+3);
	      }
	      else
		lhs_param = XH_list_elem (rcg_predicates, lhs_bot2+2);
	    }
	  }

	  if (lhs_param > 0) {
	    lhs_bot3 = XH_X (rcg_parameters, lhs_param);
	    lhs_top3 = XH_X (rcg_parameters, lhs_param+1);
	    is_first = true;

	    for (lhs_cur3 = lhs_bot3; lhs_cur3 < lhs_top3; lhs_cur3++) {
	      symb = XH_list_elem (rcg_parameters, lhs_cur3);

	      if (symb > 0) {
		if (SXBA_bit_is_set (lvar_set, symb)) {
		  if (is_first) is_first = false; else fputs (" ", out_file);
		  fprintf (out_file, "X%ld", symb-1);
		}
		else
		  has_changed = true;
	      }
	      else {
		if (is_first) is_first = false; else fputs (" ", out_file);
		out_t_text (-symb);
	      }	
	    }
	  }			    

	  if (lhs_arity == 2) {
	    fputs (", ", out_file);
	    lhs_param = XH_list_elem (rcg_predicates, lhs_bot2+3);

	    if (lhs_param > 0) {
	      lhs_bot3 = XH_X (rcg_parameters, lhs_param);
	      lhs_top3 = XH_X (rcg_parameters, lhs_param+1);
	      is_first = true;

	      for (lhs_cur3 = lhs_bot3; lhs_cur3 < lhs_top3; lhs_cur3++) {
		symb = XH_list_elem (rcg_parameters, lhs_cur3);

		if (symb > 0) {
		  if (SXBA_bit_is_set (lvar_set, symb)) {
		    if (is_first) is_first = false; else fputs (" ", out_file);
		    fprintf (out_file, "X%ld", symb-1);
		  }
		  else
		    has_changed = true;
		}
		else {
		  if (is_first) is_first = false; else fputs (" ", out_file);
		  out_t_text (-symb);	
		}
	      }
	    }
	  }

	  fputs (") --> ", out_file);

	  rhs_cur = rhs_bot;

	  while (rhs_cur < rhs_top) {
	    rhs_prdct = XH_list_elem (rcg_rhs, rhs_cur);
	    rhs_bot2 = XH_X (rcg_predicates, rhs_prdct);
	    rhs_nt = XH_list_elem (rcg_predicates, rhs_bot2+1);

	    if (rhs_nt > 0) {
	      if (nt2arity [rhs_nt] == 2) {
		left_symb = XH_list_elem (rcg_parameters, XH_X (rcg_parameters, XH_list_elem (rcg_predicates, rhs_bot2+2)));
		right_symb = XH_list_elem (rcg_parameters, XH_X (rcg_parameters, XH_list_elem (rcg_predicates, rhs_bot2+3)));

		if (SXBA_bit_is_set (lvar_set, left_symb) && SXBA_bit_is_set (lvar_set, right_symb)) {
		  out_prdct_text (rhs_prdct);
		  fputs (" ", out_file);
		}
		else {
		  if (SXBA_bit_is_set (lvar_set, left_symb)) {
		    /* on genere A(left_symb); */
		    print_nt (rhs_nt);
		    fprintf (out_file, "(X%ld) ", left_symb-1);
		  }
		  else {
		    if (SXBA_bit_is_set (lvar_set, right_symb)) {
		      /* on genere A(right_symb); */
		      print_nt (rhs_nt);
		      fprintf (out_file, "(X%ld) ", right_symb-1);
		    }
		    /* else Ca disparait */
		  }
		}
	      }
	      else {
		out_prdct_text (rhs_prdct);
		fputs (" ", out_file);
	      }
	    }
	    else {
	      switch (rhs_nt) {
	      case STREQ_ic:
	      case STREQLEN_ic:
		left_symb = XH_list_elem (rcg_parameters, XH_X (rcg_parameters, XH_list_elem (rcg_predicates, rhs_bot2+2)));
		right_symb = XH_list_elem (rcg_parameters, XH_X (rcg_parameters, XH_list_elem (rcg_predicates, rhs_bot2+3)));

		if (SXBA_bit_is_set (lvar_set, left_symb) && SXBA_bit_is_set (lvar_set, right_symb)) {
		  out_prdct_text (rhs_prdct);
		  fputs (" ", out_file);
		}
		else {
		  if (SXBA_bit_is_set (lvar_set, left_symb)) {
		    /* on genere &StrLen(0, left_symb); */
		    fprintf (out_file, "&%s(0,X%ld) ", sxttext(sxtab_ptr, strlen_t), left_symb-1);
		  }
		  else {
		    if (SXBA_bit_is_set (lvar_set, right_symb)) {
		      /* on genere &StrLen(0, right_symb); */
		      fprintf (out_file, "&%s(0,X%ld) ", sxttext(sxtab_ptr, strlen_t), right_symb-1);
		    }
		  }
		}

		break;

	      case STRLEN_ic:
		/* Le 1er arg de &StrLen est un entier */ 
	      case FIRST_ic:
		/* Le 1er arg de &First est ds T* */
	      case LAST_ic:
		/* Le 1er arg de &Last est ds T* */
		right_symb = XH_list_elem (rcg_parameters, XH_X (rcg_parameters, XH_list_elem (rcg_predicates, rhs_bot2+3)));

		if (SXBA_bit_is_set (lvar_set, right_symb)) {
		  /* On conserve */
		  out_prdct_text (rhs_prdct);
		  fputs (" ", out_file);
		}

		break;

	      case FALSE_ic:
	      case CUT_ic:
	      case TRUE_ic:
	      case LEX_ic:
		/* nb var d'arg, on ne doit pas le trouver la!! */
		out_prdct_text (rhs_prdct);
		fputs (" ", out_file);
		break;
	      }

	    }

	    rhs_cur++;
	  }

	  fputs (".\n", out_file);
	}

	if (has_changed)
	  modified_clause_nb++;
      }

      fclose (out_file);
    }
#endif /* !is_case2 */
  }

  sxbm_free (clause2lvar_set);
  sxfree (strong_left_clause_set);
  sxfree (strong_right_clause_set);
  sxfree (strong_left_A_set);
  sxfree (strong_right_A_set);

  return modified_clause_nb;
}
#endif /* 0 */

/* Il est + fin (ds le cas is_case2==0) d'utiliser Ak2len[Ak] que de determiner les arbres strong left
   et strong right car c'est + general et ce n'est pas limite' aux TAG!! */
static SXINT
tag2tig (void)
{
  SXINT modified_clause_nb, A, arity, clause, lhs_prdct, lhs_bot2, lhs_top2, lhs_cur2, lhs_nt, k, lhs_param, lhs_bot3, lhs_top3, lhs_cur3;
  SXINT rhs_param, rhs_top, rhs_bot, rhs_cur, rhs_bot3, rhs_top3, rhs_cur3, symb, rhs_prdct, rhs_bot2, rhs_top2, rhs_cur2, rhs_nt, left_symb, right_symb, rhs; 
  bool is_vanished, has_changed, is_first_arg, is_first_symb; 
  SXBA lvar_set;

  sxinitialise(rhs_bot); /* pour faire taire gcc -Wuninitialized */
  sxinitialise(rhs_top); /* pour faire taire gcc -Wuninitialized */
  modified_clause_nb = 0;

  for (A = 2; A <= max_nt; A++) {
    /* Prudence, on ne touche pas a l'axiome */
    arity = nt2arity [A];

    for (k = 0; k < arity; k++) {
      if (Ak2len [A_k2Ak (A, k)] == 0) {
	modified_clause_nb = 1;
	break;
      }
    }

    if (modified_clause_nb)
      break;
  }

  if (modified_clause_nb) {
    /* Ici la transformation TIG va avoir de l'effet */
    modified_clause_nb = 0;

    lvar_set = sxba_calloc (max_gvar+1);

    out_file = tig_file;
    out_header (rcg_header, tig_file_name, "in \"tig\" form");

    for (clause = 1; clause <= last_clause; clause++) {
      lhs_prdct = XxY_X (rcg_clauses, clause) & lhs_prdct_filter;
      lhs_bot2 = XH_X (rcg_predicates, lhs_prdct);
      lhs_top2 = XH_X (rcg_predicates, lhs_prdct+1);
      lhs_nt = XH_list_elem (rcg_predicates, lhs_bot2+1);

      is_vanished = true;
      sxba_empty (lvar_set);
	
      for (k = 0, lhs_cur2 = lhs_bot2+2; lhs_cur2 < lhs_top2; k++, lhs_cur2++) {
	if (Ak2len [A_k2Ak (lhs_nt, k)] != 0) {
	  is_vanished = false;
	  break;
	}
      }

      if (!is_vanished) {
	rhs = XxY_Y (rcg_clauses, clause);

	/* On note les variables qui ne designent pas des arguments vides */
	rhs_top = XH_X (rcg_rhs, rhs+1);
	rhs_cur = rhs_bot = XH_X (rcg_rhs, rhs);

	while (rhs_cur < rhs_top) {
	  rhs_prdct = XH_list_elem (rcg_rhs, rhs_cur);
	  rhs_bot2 = XH_X (rcg_predicates, rhs_prdct);
	  rhs_top2 = XH_X (rcg_predicates, rhs_prdct+1);
	  rhs_nt = XH_list_elem (rcg_predicates, rhs_bot2+1);

	  if (rhs_nt > 0) {
	    for (k = 0, rhs_cur2 = rhs_bot2+2; rhs_cur2 < rhs_top2; k++, rhs_cur2++) {
	      if (Ak2len [A_k2Ak (rhs_nt, k)] != 0) {
		rhs_param = XH_list_elem (rcg_predicates, rhs_cur2);

		if (rhs_param > 0) {
		  rhs_bot3 = XH_X (rcg_parameters, rhs_param);
		  rhs_top3 = XH_X (rcg_parameters, rhs_param+1);

		  for (rhs_cur3 = rhs_bot3; rhs_cur3 < rhs_top3; rhs_cur3++) {
		    symb = XH_list_elem (rcg_parameters, rhs_cur3);

		    if (symb > 0)
		      SXBA_1_bit (lvar_set, symb);
		  }
		}
	      }
	    }
	  }

	  rhs_cur++;
	}
      }
      
      if (clause2visited [clause]->token.comment)
	fprintf (out_file, "%s", clause2visited [clause]->token.comment);

      fprintf (out_file, "\n%% %ld: ", (SXINT) clause);
      out_clause (clause);

      if (is_vanished) {
	modified_clause_nb++;
      }
      else {
	has_changed = false;
	is_first_arg = true;
	out_nt_text (lhs_nt);
	fputs ("(", out_file);
	
	for (k = 0, lhs_cur2 = lhs_bot2+2; lhs_cur2 < lhs_top2; k++, lhs_cur2++) {
	  if (Ak2len [A_k2Ak (lhs_nt, k)] != 0) {
	    if (is_first_arg)
	      is_first_arg = false;
	    else
	      fputs (", ", out_file);

	    lhs_param = XH_list_elem (rcg_predicates, lhs_cur2);

	    if (lhs_param > 0) {
	      lhs_bot3 = XH_X (rcg_parameters, lhs_param);
	      lhs_top3 = XH_X (rcg_parameters, lhs_param+1);
	      is_first_symb = true;

	      for (lhs_cur3 = lhs_bot3; lhs_cur3 < lhs_top3; lhs_cur3++) {
		symb = XH_list_elem (rcg_parameters, lhs_cur3);

		if (symb > 0) {
		  if (SXBA_bit_is_set (lvar_set, symb)) {
		    if (is_first_symb) is_first_symb = false; else fputs (" ", out_file);
		    fprintf (out_file, "X%ld", (SXINT) symb-1);
		  }
		  else
		    has_changed = true;
		}
		else {
		  if (is_first_symb) is_first_symb = false; else fputs (" ", out_file);
		  out_t_text (-symb);
		}
	      }
	    }	
	  }
	}

	fputs (") --> ", out_file);

	rhs_cur = rhs_bot;

	while (rhs_cur < rhs_top) {
	  rhs_prdct = XH_list_elem (rcg_rhs, rhs_cur);
	  rhs_bot2 = XH_X (rcg_predicates, rhs_prdct);
	  rhs_top2 = XH_X (rcg_predicates, rhs_prdct+1);
	  rhs_nt = XH_list_elem (rcg_predicates, rhs_bot2+1);

	  if (rhs_nt > 0) {
	    is_vanished = true;
	
	    for (k = 0, rhs_cur2 = rhs_bot2+2; rhs_cur2 < rhs_top2; k++, rhs_cur2++) {
	      if (Ak2len [A_k2Ak (rhs_nt, k)] != 0) {
		is_vanished = false;
		break;
	      }
	    }

	    if (!is_vanished) {
	      /* l'appel du predicat utilisateur n'a pas completement disparu */
	      is_first_arg = true;
	      out_nt_text (rhs_nt);
	      fputs ("(", out_file);

	      for (k = 0, rhs_cur2 = rhs_bot2+2; rhs_cur2 < rhs_top2; k++, rhs_cur2++) {
		if (Ak2len [A_k2Ak (rhs_nt, k)] != 0) {
		  if (is_first_arg)
		    is_first_arg = false;
		  else
		    fputs (", ", out_file);

		  is_first_symb = true;
		  rhs_param = XH_list_elem (rcg_predicates, rhs_cur2);

		  if (rhs_param > 0) {
		    rhs_bot3 = XH_X (rcg_parameters, rhs_param);
		    rhs_top3 = XH_X (rcg_parameters, rhs_param+1);

		    for (rhs_cur3 = rhs_bot3; rhs_cur3 < rhs_top3; rhs_cur3++) {
		      symb = XH_list_elem (rcg_parameters, rhs_cur3);

		      if (is_first_symb) is_first_symb = false; else fputs (" ", out_file);

		      if (symb > 0) {
			fprintf (out_file, "X%ld", (SXINT) symb-1);
		      }
		      else {
			out_t_text (-symb);
		      }
		    }
		  }
		}
		else
		  has_changed = true;
	      }

	      fputs (") ", out_file);
	    }
	  }
	  else {
	    switch (rhs_nt) {
	    case STREQ_ic:
	    case STREQLEN_ic:
	      left_symb = XH_list_elem (rcg_parameters, XH_X (rcg_parameters, XH_list_elem (rcg_predicates, rhs_bot2+2)));
	      right_symb = XH_list_elem (rcg_parameters, XH_X (rcg_parameters, XH_list_elem (rcg_predicates, rhs_bot2+3)));

	      if (SXBA_bit_is_set (lvar_set, left_symb) && SXBA_bit_is_set (lvar_set, right_symb)) {
		out_prdct_text (rhs_prdct);
		fputs (" ", out_file);
	      }
	      else {
		if (SXBA_bit_is_set (lvar_set, left_symb)) {
		  /* on genere &StrLen(0, left_symb); */
		  fprintf (out_file, "&%s(0,X%ld) ", sxttext(sxtab_ptr, strlen_t), (SXINT) left_symb-1);
		}
		else {
		  if (SXBA_bit_is_set (lvar_set, right_symb)) {
		    /* on genere &StrLen(0, right_symb); */
		    fprintf (out_file, "&%s(0,X%ld) ", sxttext(sxtab_ptr, strlen_t), (SXINT) right_symb-1);
		  }
		}
	      }

	      break;

	    case STRLEN_ic:
	      /* Le 1er arg de &StrLen est un entier */ 
	    case FIRST_ic:
	      /* Le 1er arg de &First est ds T* */
	    case LAST_ic:
	      /* Le 1er arg de &Last est ds T* */
	      right_symb = XH_list_elem (rcg_parameters, XH_X (rcg_parameters, XH_list_elem (rcg_predicates, rhs_bot2+3)));

	      if (SXBA_bit_is_set (lvar_set, right_symb)) {
		/* On conserve */
		out_prdct_text (rhs_prdct);
		fputs (" ", out_file);
	      }

	      break;

	    case FALSE_ic:
	    case CUT_ic:
	    case TRUE_ic:
	    case LEX_ic:
	      /* nb var d'arg, on ne doit pas le trouver la!! */
	      out_prdct_text (rhs_prdct);
	      fputs (" ", out_file);
	      break;
	    default:
#if EBUG
	      sxtrap(ME,"unknown switch case #2");
#endif
	      break;
	    }

	  }

	  rhs_cur++;
	}

	fputs (".\n", out_file);

	if (has_changed)
	  modified_clause_nb++;
      }
    }

    fclose (out_file);

    sxfree (lvar_set);
  }

  return modified_clause_nb;
}


void
tig_form (void)
{
  SXINT modified_clause_nb;

#if 0
  if (max_garity != 2 || !is_simple_grammar) {
    /* Ce n'est pas une TAG */
    if (sxverbosep) {
      fputs ("WARNING: the option \"-tig\" only applies to TAGs.\n", sxtty);
      sxttycol1p = true;
    }

    return;
  }
#endif /* 0 */
    
  if (sxverbosep) {
    fputs ("\ttig form .... ", sxtty);
    sxttycol1p = false;
  }

  modified_clause_nb = tag2tig ();

  if (sxverbosep) {
    if (modified_clause_nb)
      fprintf (sxtty, "succes[%ld/%ld]\n", (SXINT) modified_clause_nb, (SXINT) last_clause);
    else
      fputs ("failure\n", sxtty);

    sxttycol1p = true;
  }
}


#if 0 
#define tmax()    bnf_ag.WS_TBL_SIZE.tmax
#define ntmax()   bnf_ag.WS_TBL_SIZE.ntmax
#define maxnpd()  bnf_ag.WS_TBL_SIZE.maxnpd

#define lispro(i) bnf_ag.WS_INDPRO [i].lispro
#define prolis(i) bnf_ag.WS_INDPRO [i].prolis
#define reduc(i)  bnf_ag.WS_NBPRO [i].reduc
#define prolon(i) bnf_ag.WS_NBPRO [i].prolon
#define numpg(i)  bnf_ag.WS_NBPRO [i].numpg
#define action(i) bnf_ag.WS_NBPRO [i].action
#define npg(i)    bnf_ag.WS_NTMAX [i].npg
#define npd(i)    bnf_ag.WS_NTMAX [i].npd
#endif
