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
   - le parser en C correspondant
 */

/* Pour ne pas generer les tests de memoP ds le parseur */
#define is_memop 0
/* Pour generer inconditionnellement la memoisation ds le parseur */
#define is_memon 0
/* Pour generer inconditionnellement la "semantique" ds le parseur */
#define is_semantics 0

/*   I N C L U D E S   */
#include "sxversion.h"
#include "sxunix.h"
#include "sxba.h"
#include "rcg.h"
#include "varstr.h"
#include "sxword.h"
#include "fsa.h"
#include <ctype.h>
#include <string.h>

static char	ME [] = "rcg_gen_parser";

char WHAT_RCGGENPARSER[] = "@(#)SYNTAX - $Id: rcg_gen_parser.c 4152 2024-08-02 11:46:01Z garavel $" WHAT_DEBUG;


extern SXINT		is_in_Tstar (SXINT, SXINT);
extern SXINT            clause2lhs_nt (SXINT clause);
extern bool        rhs_arg2sub_arg (SXINT lhs_prdct, SXINT arg);


#define has_semantics		true
/* A VOIR */

/* ---------------------------------------------------------------------------------------------------- */
/* Le 05/02/04 prgentname est le prefixe du nom de fichier, il peut contenir
   des caracteres non alphanumeriques et peut donc ne pas etre un ident C.
   On cree donc la macro PRGENTNAME(b) : si b==true => prgentname sinon "RCG" */
#define PRGENTNAME(b)   (b)?prgentname:"RCG"

static FILE		*parser_file;


static SXINT		max_degree, max_degree_clause, *for_loops_nb;
static SXINT		Aisize_nb, ASindex_nb, Avar_nb, Arhs_arg_nb, Arhs_prdct_nb, Aetc_nb, Alhs_t_nb, Alhs_adr_nb;
static bool		Acut, Anon_range, is_source_checked, rhs_arg_nb;
static SXINT		max_tooth, max_son_nb, max_generator_pos; 
static SXINT		lhs_bot2, lhs_top2, lhs_arity, rhs_prdct_nyp_nb, wtop, D, varstrings_top, next_ii, lhs_t_nb;
static SXINT		max_equation, max_adr, max_atom, lhs_equation, eq_top, Sindex, cur_rho_size;
static bool	has_predef_prdct, has_predef_prdct_call, is_some_equiv, is_static_phase;
static bool	has_external, has_ext_def, has_ext_decl;
static bool	clause_has_cut, is_lex_clause;
static SXINT		A_clause_nb, clause_id, lhs_arg_tbp, not_substituted_nb;
static VARSTR		*varstrings, wvstr, vstr_cond, vstr_body, t_indexes_vstr;

static SXINT		**equation, *eq_list, *equation2Xnb, *equation_stack, *equation_stack2;
static SXINT		*equation2vstr_kind;

#define UNKNOWN_k	2
#define GEQ0_k		1
#define NULL_k		0
#define LEQ0_k		-1

static VARSTR		*equation2vstr;
static SXBA		equation_already_substituted_set, nyp_eq_set;


static SXINT		*lhs_pos2adr, *lhs_pos2ladr, *lhs_pos2radr, *lhs_arg_pos2prev_args_t_nb;
static VARSTR		*lhs_pos2lvstr, *lhs_pos2rvstr;
static SXBA		lhs_arg_tbp_set, useless_lhs_arg;

#define NullSize 	(char)0
#define UnknownSize	(char)1
#define NonNullSize	(char)2

static char		*rhs_pos2static_size_kind;
static SXINT		*rhs_pos2Ak, *rhs_pos2nt, *rhs_pos2param, *rhs_pos2lmadr, *rhs_pos2rmadr;
static SXINT		*rhs_pos2rhs_mpos;
static SXBA		rhs_pos2nc_max_set; /* arg non_combinatorial dont le max est connu */
static SXBA		arity_set, rhs_pos2neg_call_set;

static SXBA		check_nul_range, check_nul_range_done, check_olb, check_oub;
static SXBA		check_non_nul_range, check_non_nul_range_done;
static SXBA		is_first_complete, is_first_vide, is_last_complete, is_last_vide;

static SXBA		pos_is_a_range, ij_already_set;
static SXINT		*adr2cur, *adr2madr, *madr2olb, *madr2oub, *adr2lA, *adr2uA, *adr2pos, *adr_stack, *adr2tpos; 
static SXBA		madr_set, lmadr_set, rmadr_set, tmadr_set, synchro_set, is_madr_an_ob;
static SXINT		*synchro2adr;
static SXBA		assigned_madr_set, already_seen_madr_set, nyp_synchro_test;
static SXBA		*adr2equiv_adr_set, *wmadr_sets;
static VARSTR		*madr2lcode, *madr2rcode, *synchro2vstr;
static SXINT		*olbs, *oubs;

static SXINT		*atom2min, *atom2max, *gatom2nb, *atom_stack, *atom2var_nb, *atom2static_size;
static SXBA		Asize_set;
static VARSTR		*atom2rsize;
static XxY_header	VxV;

static SXINT		*ext_decl2clause;
static SXBA		ext_decl_set, ext_decl_clause_set, clause_set, clause_has_ext_decl;

static SXINT		*var2atom, *var2nb, *var2lhs_pos;

static XH_header	XH_prdct_args;
static SXINT		*XH_prdct_args2mprdct_pos;

static SXINT		*prdct_pos2mprdct_pos, *mprdct_pos2generated, *rhs_prdct_pos2tooth, *rhs_prdct_pos2generator_pos;
static SXBA		rhs_prdct_nyp;

static SXINT		max_component_nb;
static SXINT		*component2pos, *component2bot, *component2top, *component2lmadr, *component2rmadr;

static SXINT		*Indexes, *clause2lhst_disp;

static SXBA		non_false_A_clause_set, recursive_nt_set, recursive_clause_set, lexicalized_clause_set,
                        cyclic_clause_set, cyclic_nt_set;
static bool		is_cyclic_grammar;
static SXINT		*clause2clause_id, max_clause_id, *A2max_clause_id;

static SXBA             *Lex, *t2clause_set; 

static SXINT            *pos2counter_val, *counter2pos;
static SXINT            max_counter_nb;

static SXINT            *t2Cid_ste;
static bool        is_GET_QSTACK, is_GET_PSTACK;

/* ---------------------------------------------------------------------------------------------------- */

static void
XH_prdct_args_oflw (SXINT old_size, SXINT new_size)
{
    sxuse(old_size); /* pour faire taire gcc -Wunused */
    XH_prdct_args2mprdct_pos = (SXINT*) sxrealloc (XH_prdct_args2mprdct_pos, new_size+1, sizeof (SXINT));
}

void
equation_oflw (void)
{
  SXINT 	pos, new_top = eq_top*2;
  SXINT		*ptr, *ptr_top;

  equation = (SXINT**) sxrealloc (equation, new_top+1, sizeof (SXINT*));
  ptr = eq_list = (SXINT*) sxrealloc (eq_list, (new_top+1)*(max_gvar+1)+1, sizeof (SXINT));
  equation2Xnb = (SXINT*) sxrealloc (equation2Xnb, new_top+1, sizeof (SXINT));
  equation_stack2 = (SXINT*) sxrealloc (equation_stack2, new_top+1, sizeof (SXINT));
  equation_stack = (SXINT*) sxrealloc (equation_stack, new_top+1, sizeof (SXINT));
  equation_already_substituted_set = sxba_resize (equation_already_substituted_set, new_top+1);
  nyp_eq_set = sxba_resize (nyp_eq_set, new_top+1);
  equation2vstr_kind = (SXINT*) sxrealloc (equation2vstr_kind, new_top+1, sizeof (SXINT));
  equation2vstr = (VARSTR*) sxrealloc (equation2vstr, new_top+1, sizeof (VARSTR));

  for (pos = 0; pos <= new_top; pos++) {
    equation [pos] = ptr;
    ptr += max_gvar+1;
  }

  ptr_top = eq_list + (new_top+1)*(max_gvar+1);
  ptr = equation [eq_top+1];

  do {
    *ptr_top = 0;
  } while (--ptr_top >= ptr);

  for (pos = eq_top+1; pos <= new_top; pos++) {
    equation2vstr [pos] = varstr_alloc (32);
  }

  eq_top = new_top;
}


static void
clear_struct (void)
{
  SXINT pos, var, x, adr, atom;
  SXINT	*eq_ptr;

  rhs_prdct_nyp_nb = 0;
  sxba_empty (rhs_prdct_nyp);

  if (is_some_equiv) {
    is_some_equiv = false;

    for (adr = 1; adr < max_adr; adr++) {
      sxba_empty (adr2equiv_adr_set [adr]);
    }
  }

  for (adr = 1; adr < max_adr; adr++) {
    adr2uA [adr] = 0;
    adr2lA [adr] = 0;
    adr2madr [adr] = 0;
    synchro2adr [adr] = 0;
    madr2olb [adr] = 0;
    madr2oub [adr] = 0;
  }

  for (atom = 1; atom <= max_atom; atom++) {
    atom2min [atom] = 0;
    atom2max [atom] = -1; /* inconnu */
  }

  for (pos = 0; pos < max_equation; pos++) {
    eq_ptr = equation [pos];

    for (atom = 0; atom <= max_atom; atom++)
      eq_ptr [atom] = 0;
  }

  for (var = 1; var <= max_var; var++) {
    var2atom [var] = var2lhs_pos [var] = 0;
  }

  for (x = max_rhs_prdct+1; x >= 0; x--) {
    mprdct_pos2generated [x] = -1;
  }

  if (first) {
    sxba_empty (check_nul_range_done);
    sxba_empty (check_non_nul_range_done);
  }

  sxba_empty (pos_is_a_range);
  sxba_empty (ij_already_set);
  sxba_empty (assigned_madr_set);
  sxba_empty (already_seen_madr_set);
  sxba_empty (nyp_synchro_test);
  sxba_empty (Asize_set);
  sxba_empty (madr_set);
  sxba_empty (synchro_set);
  sxba_empty (is_madr_an_ob);
  sxba_empty (tmadr_set);
  sxba_empty (equation_already_substituted_set);
  sxba_empty (nyp_eq_set);
  sxba_empty (rhs_pos2neg_call_set);
  sxba_empty (rhs_pos2nc_max_set);

  XH_clear(&XH_prdct_args);

  XxY_clear (&VxV);
}



static VARSTR
i2varstr (VARSTR vstr, SXINT i)
{
  char	istr [20];

  sprintf (istr, "%ld", (SXINT) i);
  return varstr_catenate (vstr, istr);
}

static VARSTR
nt2varstr (VARSTR vstr, SXINT A)
{
  char	*p1;
  bool	is_external;

  if (A > 0) {
    is_external = SXBA_bit_is_set (is_nt_external, A);
    p1 = sxstrget (nt2ste [A]);
  }
  else {
    is_external = true;

    switch (A) {
    case STREQ_ic:
      p1 = sxttext (sxtab_ptr, streq_t);
      break;

    case STREQLEN_ic:
      p1 = sxttext (sxtab_ptr, streqlen_t);
      break;

    case STRLEN_ic:
      p1 = sxttext (sxtab_ptr, strlen_t);
      break;

    case FALSE_ic:
      p1 = sxttext (sxtab_ptr, false_t);
      break;
	
    case FIRST_ic:
      p1 = sxttext (sxtab_ptr, first_t);
      break;

    case LAST_ic:
      p1 = sxttext (sxtab_ptr, last_t);
      break;

    case CUT_ic:
      p1 = sxttext (sxtab_ptr, cut_t);
      break;

    case TRUE_ic:
      p1 = sxttext (sxtab_ptr, true_t);
      break;

    case LEX_ic:
      p1 = sxttext (sxtab_ptr, lex_t);
      break;

	
    default:
#if EBUG
      sxtrap(ME,"unknown switch case #1");
#endif
      sxinitialise(p1); /* pour faire taire gcc -Wuninitialized */
      break;
    }
  }

  if (is_external)
    vstr = varstr_catenate (vstr, "&");

  return varstr_catenate (vstr, p1);
}


static VARSTR
t2varstr (VARSTR vstr, SXINT t)
{
  char *str, alpha [20];

  if (t == 0)
    return varstr_catenate (vstr, "\\\"\\\"");
    
  if (t > max_ste) {
    sprintf (alpha, "%ld", t-max_ste-1);
    str = &(alpha [0]);
  }
  else {
    str = sxstrget (t2ste [t]);

    if (*str == '"')
      str++;
  }

  return varstr_catenate (vstr, str);
}


static VARSTR
X2varstr (VARSTR vstr, SXINT X)
{
  return i2varstr (varstr_catenate (vstr, "X"), X-1);
}


static VARSTR
prdct2varstr (VARSTR vstr, SXINT prdct)
{
  SXINT bot, cur, top, nt, param, bot2, cur2, top2, symb;

  bot = XH_X (rcg_predicates, prdct);
  top = XH_X (rcg_predicates, prdct+1);

  if (XH_list_elem (rcg_predicates, bot) == 1)
    vstr = varstr_catenate (vstr, "!");

  nt = XH_list_elem (rcg_predicates, bot+1);
  vstr = nt2varstr (vstr, nt);
  vstr = varstr_catenate (vstr, "(");
  cur = bot+2;
    
  while (cur < top) {
    param = XH_list_elem (rcg_predicates, cur);

    if (param != 0) {
      bot2 = XH_X (rcg_parameters, param);
      top2 = XH_X (rcg_parameters, param+1);
      cur2 = bot2;

      while (cur2 < top2) {
	symb = XH_list_elem (rcg_parameters, cur2);

	if (nt == STRLEN_ic && cur == bot+2)
	  vstr = i2varstr (vstr, symb);
	else {
	  if (symb == 0) {
	    if (cur == bot+2 && (nt == FIRST_ic || nt == LAST_ic))
	      vstr = t2varstr (vstr, (SXINT)0); /* eof */
	    else
	      vstr = varstr_catenate (vstr, "...");
	  }
	  else {
	    if (symb < 0)
	      vstr = t2varstr (vstr, -symb);
	    else 
	      vstr = X2varstr (vstr, symb);
	  }
	}

	cur2++;

	if (cur2 < top2)
	  vstr = varstr_catenate (vstr, " ");
      }
    }

    cur++;

    if (cur < top)
      vstr = varstr_catenate (vstr, ", ");
  }
    
  return varstr_catenate (vstr, ")");
}


static VARSTR
clause2varstr (VARSTR vstr, SXINT clause)
{
  SXINT rhs, bot, cur, top, ste;

  vstr = i2varstr (vstr, clause);
  vstr = varstr_catenate (vstr, ": ");
  vstr = prdct2varstr (vstr, XxY_X (rcg_clauses, clause) & lhs_prdct_filter);
  vstr = varstr_catenate (vstr, " --> ");

  rhs = XxY_Y (rcg_clauses, clause);
  top = XH_X (rcg_rhs, rhs+1);
  bot = XH_X (rcg_rhs, rhs);
  cur = bot;

  while (cur < top) {
    vstr = prdct2varstr (vstr, XH_list_elem (rcg_rhs, cur));
	
    cur++;

    if (cur < top)
      vstr = varstr_catenate (vstr, " ");
  }

  vstr = varstr_catenate (vstr, " .");

  ste = clause2action [clause];

  if (ste != SXEMPTY_STE)
    vstr = varstr_catenate (varstr_catenate (vstr, "@"), sxstrget (ste));

  return vstr;
}

   
static void
gen_nt_text (FILE *gen_nt_text_parser_file, SXINT A)
{
  char	*p1;

  if (A > 0) {
    if (SXBA_bit_is_set (is_nt_internal, A))
      fprintf (gen_nt_text_parser_file, "%s", sxstrget (nt2ste [A]));
    else
      fprintf (gen_nt_text_parser_file, "&%s", sxstrget (nt2ste [A]));
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
      sxtrap(ME,"unknown switch case #2");
#endif
      sxinitialise(p1); /* pour faire taire gcc -Wuninitialized */
      break;
    }

    fprintf (gen_nt_text_parser_file, "&%s", p1);
  }
}

static void
gen_t_text (FILE *gen_t_text_parser_file, SXINT t)
{
  SXINT lgth;
  char	c, *p1, *p;

  if (t == 0) {
    fputs ("\"\"", gen_t_text_parser_file);
  }
  else {
    if (t > max_ste) {
      fprintf (gen_t_text_parser_file, "%ld", t-max_ste-1);
    }
    else {
      p1 = sxstrget (t2ste [t]);
      lgth = strlen (p1);

      if (*p1 == '"') {
	p1++;
	lgth--;
      }

      if (lgth == 1)
	fprintf (gen_t_text_parser_file, "\\%s", p1);
      else {
	fputs ("\"", gen_t_text_parser_file);

	while ((p = strpbrk (p1, "\"*")) != NULL) {
	  c = *p;
	  *p = SXNUL;
	  fprintf (gen_t_text_parser_file, "%s", p1);
	  *p = c;

	  if (c == '"') {
	    fputs ("\\\"", gen_t_text_parser_file);
	  }
	  else {
	    if (p [1] == '/') {
	      fputs ("*\\", gen_t_text_parser_file);
	      p++;
	    }
	  }

	  p1 = p;
	}

	fprintf (gen_t_text_parser_file, "%s\"", p1);
      }
    }
  }
}

static void
gen_var_text (FILE *gen_var_text_parser_file, SXINT var)
{
  fprintf (gen_var_text_parser_file, "X%ld", (SXINT) var-1);
}

static SXINT *lhs_symb_disp, *lhs_symb_nb, *rhs_arg_pos_list;

static void
seek (SXINT lhs_prdct, SXINT rhs_bot3, SXINT i, SXINT rhs_arg_pos)
{
  /* On note toutes les occur du sous arg rhs_bot3, i de la RHS ds la LHS */
  SXINT seek_lhs_bot2, seek_lhs_top2, lhs_cur2, lhs_param, lhs_bot3, lhs_top3, j, lhs_cur3, lhs_symb, rhs_symb, lhs_symb_pos;


  seek_lhs_bot2 = XH_X (rcg_predicates, lhs_prdct);
  seek_lhs_top2 = XH_X (rcg_predicates, lhs_prdct+1);
  lhs_symb_pos = 0;

  for (lhs_cur2 = seek_lhs_bot2+2; lhs_cur2 < seek_lhs_top2; lhs_cur2++) {
    lhs_param = XH_list_elem (rcg_predicates, lhs_cur2);

    if (lhs_param > 0) {
      lhs_bot3 = XH_X (rcg_parameters, lhs_param);
      lhs_top3 = XH_X (rcg_parameters, lhs_param+1);

      j = 0;
      lhs_cur3 = lhs_bot3;

      while (lhs_cur3+j < lhs_top3 && j < i) {
	lhs_symb = XH_list_elem (rcg_parameters, lhs_cur3+j);
	rhs_symb = XH_list_elem (rcg_parameters, rhs_bot3+j);

	if (lhs_symb != rhs_symb) {
	  lhs_cur3++;
	  lhs_symb_pos++;
	  j = 0;
	}
	else
	  j++;
      }

      if (i == j) {
	/* Gagne' */
	for (j = 0; j < i; j++) {
	  rhs_arg_pos_list [lhs_symb_disp [lhs_symb_pos] + lhs_symb_nb [lhs_symb_pos]] = rhs_arg_pos;
	  lhs_symb_nb [lhs_symb_pos]++;
	  lhs_symb_pos++;
	}

	lhs_symb_pos += lhs_top3 - (lhs_cur3+i);
      }
      else
	lhs_symb_pos += j;

    }
  }
}


static void
gen_Xoccur (void)
{
  /* Si is_generator, permet de connaitre la composition des args des clauses */
  SXINT		Xoccur_top, local_top, nt, n, l;
  SXINT 	i, clause, lhs_prdct, gen_Xoccur_lhs_bot2, lhs_cur2, gen_Xoccur_lhs_top2, lhs_param, lhs_bot3, lhs_top3, lhs_cur3;
  SXINT		rhs, rhs_top, rhs_bot, rhs_cur, rhs_prdct, rhs_bot2, rhs_cur2, rhs_top2, rhs_param, rhs_bot3, rhs_top3, rhs_cur3, rhs_symb, rhs_arg_pos;
  SXINT		*Xoccur_disp;
  SXINT		nb, symb_nb, lhs_symb_pos, bot, x;
    
  sxinitialise(rhs_top); /* pour faire taire gcc -Wuninitialized */
  sxinitialise(rhs_bot); /* pour faire taire gcc -Wuninitialized */
  sxinitialise(rhs_cur); /* pour faire taire gcc -Wuninitialized */
  l = max_arg_size*max_garity;
  lhs_symb_disp = (SXINT*) sxalloc (l+1, sizeof (SXINT));
  lhs_symb_nb = (SXINT*) sxalloc (l+1, sizeof (SXINT));
  rhs_arg_pos_list = (SXINT*) sxalloc (l*max_rhs_arg_nb+1, sizeof (SXINT));

  n = lhs_symb_disp [0] = 0;

  for (i = 1; i <= l; i++) {
    n = lhs_symb_disp [i] = n + max_rhs_arg_nb;
  }

  Xoccur_disp = (SXINT*) sxalloc (last_clause+2, sizeof (SXINT));
  Xoccur_disp [0] = 0;

  Indexes = (SXINT*) sxalloc (last_clause+1, sizeof (SXINT));

  fputs ("static SXINT Xoccur [] = {0,\n", parser_file);
    
  Xoccur_disp [Xoccur_top = 1] = 1;
    
  for (clause = 1; clause <= last_clause; clause++) {
    fprintf (parser_file, "/* %ld */", (SXINT) clause);

    local_top = 0;

    if (clause2identical [clause] == clause) {
      if ((rhs = XxY_Y (rcg_clauses, clause))) {
	rhs_top = XH_X (rcg_rhs, rhs+1);
	rhs_bot = XH_X (rcg_rhs, rhs);

	for (rhs_cur = rhs_bot; rhs_cur < rhs_top; rhs_cur++) {
	  rhs_prdct = XH_list_elem (rcg_rhs, rhs_cur);
	  rhs_bot2 = XH_X (rcg_predicates, rhs_prdct);

	  if (XH_list_elem (rcg_predicates, rhs_bot2) == 0 && XH_list_elem (rcg_predicates, rhs_bot2+1) == FALSE_ic) {
	    /* Appel positif de &False() */
	    break;
	  }
	}
      }

      if (rhs == 0 || rhs_cur == rhs_top) {
	/* Ne contient pas &False () */
	/* Pour chaque (sous-)arg de la RHS on cherche les sous args de la LHS ou il occurre */

	lhs_prdct = XxY_X (rcg_clauses, clause) & lhs_prdct_filter;
	gen_Xoccur_lhs_bot2 = XH_X (rcg_predicates, lhs_prdct);
	gen_Xoccur_lhs_top2 = XH_X (rcg_predicates, lhs_prdct+1);
	nb = 0;

	for (lhs_cur2 = gen_Xoccur_lhs_bot2+2; lhs_cur2 < gen_Xoccur_lhs_top2; lhs_cur2++) {
	  lhs_param = XH_list_elem (rcg_predicates, lhs_cur2);

	  if (lhs_param > 0) {
	    lhs_bot3 = XH_X (rcg_parameters, lhs_param);
	    lhs_top3 = XH_X (rcg_parameters, lhs_param+1);
	    symb_nb = lhs_top3-lhs_bot3;
	    local_top += symb_nb-1;
	    nb += symb_nb;
	  }
	}

	if (rhs) {
	  for (i = 0; i < nb; i++) {
	    lhs_symb_nb [i] = 0;
	  }

	  rhs_arg_pos = 1;

	  for (rhs_cur = rhs_bot; rhs_cur < rhs_top; rhs_cur++) {
	    rhs_prdct = XH_list_elem (rcg_rhs, rhs_cur);
	    rhs_bot2 = XH_X (rcg_predicates, rhs_prdct);

	    /* Appel qcq */
	    nt = XH_list_elem (rcg_predicates, rhs_bot2+1);
	    rhs_top2 = XH_X (rcg_predicates, rhs_prdct+1);

	    for (rhs_cur2 = rhs_bot2+2; rhs_cur2 < rhs_top2; rhs_cur2++) {
	      rhs_param = XH_list_elem (rcg_predicates, rhs_cur2);

	      if (rhs_param > 0) {
		rhs_bot3 = XH_X (rcg_parameters, rhs_param);
		rhs_top3 = XH_X (rcg_parameters, rhs_param+1);

		if (rhs_cur2 > rhs_bot2+2 || (nt != STRLEN_ic && nt != FIRST_ic && nt != LAST_ic && nt != LEX_ic)) {
		  rhs_cur3 = rhs_bot3;
		  i = 0;

		  while (rhs_cur3+i < rhs_top3) {
		    rhs_symb = XH_list_elem (rcg_parameters, rhs_cur3+i);

		    if (rhs_symb == 0) {
		      /* On a un composant d'un arg de la RHS */
		      if (i > 0)
			seek (lhs_prdct, rhs_cur3, i, rhs_arg_pos);

		      rhs_cur3 += i+1;
		      i = 0;
		    }
		    else
		      i++;
		  }

		  if (i > 0)
		    seek (lhs_prdct, rhs_cur3, i, rhs_arg_pos);

		  rhs_arg_pos++;
		}
	      }
	    }
	  }

	  lhs_symb_pos = 0;

	  for (lhs_cur2 = gen_Xoccur_lhs_bot2+2; lhs_cur2 < gen_Xoccur_lhs_top2; lhs_cur2++) {
	    lhs_param = XH_list_elem (rcg_predicates, lhs_cur2);

	    if (lhs_param > 0) {
	      lhs_bot3 = XH_X (rcg_parameters, lhs_param);
	      lhs_top3 = XH_X (rcg_parameters, lhs_param+1);

	      for (lhs_cur3 = lhs_bot3; lhs_cur3 < lhs_top3; lhs_cur3++) {
		if ((nb = lhs_symb_nb [lhs_symb_pos]) > 0) {
		  bot = lhs_symb_disp [lhs_symb_pos];

		  for (i = 0; i < nb; i++) {
		    x = rhs_arg_pos_list [bot+i];
		    fprintf (parser_file, "%ld, ", (SXINT) x);
		    Xoccur_top++;
		  }
		}

		fputs ("0, ", parser_file);
		Xoccur_top++;
		lhs_symb_pos++;
	      }
	    }
	  }
	}
      }
    }

    Indexes [clause] = local_top;
    Xoccur_disp [clause+1] = Xoccur_top;

    fputs ("\n", parser_file);
  }

  fputs ("};\n", parser_file);

  fputs ("static SXINT Xoccur_disp [] = {0,\n\
/* 1 */ 1,", parser_file);

  for (clause = 1; clause <= last_clause; clause++) {
    if (clause % 8 == 1)
      fputs ("\n", parser_file);

    fprintf (parser_file, "/* %ld */ %ld, ", (SXINT) clause+1, (SXINT) Xoccur_disp [clause+1]);
  }

  fputs ("\n};\n", parser_file);

  sxfree (Xoccur_disp);
  sxfree (lhs_symb_disp);
  sxfree (lhs_symb_nb);
  sxfree (rhs_arg_pos_list);
}

static void
gen_ret_vals (SXBA A_clause_set)
{
  /* Si is_generator, permet de connaitre la composition des args des clauses */
  SXINT		ret_vals_top, Indexes_top;
  SXINT		clause, rhs, rhs_top, rhs_bot, local_top, rhs_cur, rhs_prdct, rhs_bot2, nt; 

  /* ret_vals est declare meme si on n'est pas en generation */
  ret_vals_top = 1;
  Indexes_top = 0;
  clause = 0;

  while ((clause = sxba_scan (A_clause_set, clause)) > 0) {
    rhs = XxY_Y (rcg_clauses, clause);
    rhs_top = XH_X (rcg_rhs, rhs+1);
    rhs_bot = XH_X (rcg_rhs, rhs);
    local_top = 1;

    for (rhs_cur = rhs_bot; rhs_cur < rhs_top; rhs_cur++) {
      rhs_prdct = XH_list_elem (rcg_rhs, rhs_cur);
      rhs_bot2 = XH_X (rcg_predicates, rhs_prdct);

      nt = XH_list_elem (rcg_predicates, rhs_bot2+1);

      if (nt > 0 || nt == STREQ_ic || nt == STREQLEN_ic || nt == STRLEN_ic)
	/* Les 2 derniers sont ds le cas ou le source est un DAG */
	local_top++;
    }

    if (local_top > ret_vals_top)
      ret_vals_top = local_top;

    if (Indexes_top < Indexes [clause])
      Indexes_top = Indexes [clause];
  }

  fprintf (parser_file, "SXINT\tret_vals[%ld];\n", (SXINT) ret_vals_top);

  fputs ("#if is_generator\n", parser_file);
  fprintf (parser_file, "SXINT\tIndexes[%ld];\n", (SXINT) Indexes_top);
  fputs ("#endif\n", parser_file);
}


static void
gen_prdct_text (FILE *gen_prdct_text_parser_file, SXINT prdct)
{
  SXINT bot, cur, top, nt, param, bot2, cur2, top2, symb;

  bot = XH_X (rcg_predicates, prdct);
  top = XH_X (rcg_predicates, prdct+1);

  if (XH_list_elem (rcg_predicates, bot) == 1)
    fputs ("!", gen_prdct_text_parser_file);

  nt = XH_list_elem (rcg_predicates, bot+1);
  gen_nt_text (gen_prdct_text_parser_file, nt);
  fputs ("(", gen_prdct_text_parser_file);
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
	  fprintf (gen_prdct_text_parser_file, "%ld", (SXINT) symb);
	}
	else {
	  if (symb == 0) {
	    if (cur == bot+2 && (nt == FIRST_ic || nt == LAST_ic))
	      gen_t_text (gen_prdct_text_parser_file, (SXINT)0); /* eof */
	    else
	      fputs ("...", gen_prdct_text_parser_file);
	  }
	  else {
	    if (symb < 0)
	      gen_t_text (gen_prdct_text_parser_file, -symb);
	    else 
	      gen_var_text (gen_prdct_text_parser_file, symb);
	  }
	}

	cur2++;

	if (cur2 < top2)
	  fputs (" ", gen_prdct_text_parser_file);
      }
    }

    cur++;

    if (cur < top)
      fputs (", ", gen_prdct_text_parser_file);
  }

  fputs (")", gen_prdct_text_parser_file);
}



static void
rec_clauses (void)
{
  /* Calcule ds recursive_clause_set et recursive_nt_set l'ensemble des clauses recursives */
  /* {A0 --> A1 ... Am | \forall i>0 Ai ->* A0} */
  /* Si B est un predicat externe non defini localement et si C est un predicat
     externe defini localement, on suppose que B -> C (prudence) */
  SXINT		clause, prdct, A, rhs, bot, cur, top, nt;
  SXBA	A_sons, ext_def_set, A_clause_set, *children;

  ext_def_set = sxba_calloc (max_nt+1);
  children = sxbm_calloc (max_nt+1, max_nt+1);

  clause = 0;

  while ((clause = sxba_scan (clause_set, clause)) > 0) {
    prdct = XxY_X (rcg_clauses, clause) & lhs_prdct_filter;
    A = XH_list_elem (rcg_predicates, XH_X (rcg_predicates, prdct)+1);

    A_sons = children [A];

    rhs = XxY_Y (rcg_clauses, clause);
    top = XH_X (rcg_rhs, rhs+1);
    bot = XH_X (rcg_rhs, rhs);

    for (cur = bot; cur < top; cur++) {
      prdct = XH_list_elem (rcg_rhs, cur);
      nt = XH_list_elem (rcg_predicates, XH_X (rcg_predicates, prdct)+1);

      if (nt > 0)
	SXBA_1_bit (A_sons, nt);
    }
  }

  if (has_external) {
    sxba_copy (ext_def_set, is_nt_external);
    sxba_and (ext_def_set, is_lhs_nt);

    if (!sxba_is_empty (ext_def_set)) {
      A = 0;

      while ((A = sxba_scan (is_nt_external, A)) > 0) {
	if (!SXBA_bit_is_set (is_lhs_nt, A)) {
	  sxba_or (children [A], ext_def_set);
	}
      }
    }
  }

  fermer (children, max_nt+1);

  for (A = 1; A <= max_nt; A++) {
    if (SXBA_bit_is_set (is_lhs_nt, A) && SXBA_bit_is_set (children [A], A)) {
      A_clause_set = lhsnt2clause [A];

      clause = 0;

      while ((clause = sxba_scan (A_clause_set, clause)) > 0) {
	if (!SXBA_bit_is_set (false_clause_set, clause) &&
	    !SXBA_bit_is_set (loop_clause_set, clause)) {
	  rhs = XxY_Y (rcg_clauses, clause);
	  top = XH_X (rcg_rhs, rhs+1);
	  bot = XH_X (rcg_rhs, rhs);

	  for (cur = bot; cur < top; cur++) {
	    prdct = XH_list_elem (rcg_rhs, cur);
	    nt = XH_list_elem (rcg_predicates, XH_X (rcg_predicates, prdct)+1);

	    if (nt > 0 && (nt == A || SXBA_bit_is_set (children [nt], A))) {
	      SXBA_1_bit (recursive_clause_set, clause);
	      SXBA_1_bit (recursive_nt_set, A);
	      break;
	    }
	  }
	}
      }
    }
  }

  sxfree (ext_def_set);
  sxbm_free (children);
}


static bool
process_0_atom (SXINT xeq, char *exit_stmt)
{
  /* Toutes les variables de l'equation xeq sont connues */
  SXINT		tnb;
  bool	is_useful = true;

  tnb = equation [xeq] [0];

  if (is_static_phase) {
    /* On est ds la phase statique */
    /* On ne s'occupe que des equations statiques */
    if (xeq >= lhs_equation) {
      if (tnb != 0) {
	is_useful = false;
	equation [xeq] [0] = 0; /* clear */
      }
    }
  }
  else {
    fprintf (parser_file, "if (%s != %ld) %s;\n",
	     varstr_tostr (equation2vstr [xeq]), (SXINT) tnb, exit_stmt);
  }

  return is_useful;
}


static bool
substitute_atom_in_eq (SXINT *substitute_atom_in_eq_equation_stack2, char *exit_stmt)
{
  /* On substitue (la taille de) atom ds les autres equations */
  /* On cherche l'equation qui a l'atom max */
  SXINT		xeq, atom, atom2, Xnb, Xnb2, xeq2, i, v, kind1, kind2, kind; 
  SXINT		*eq_ptr, *eq_ptr2;
  bool	is_useful = true;
  char	string [24];

  xeq = POP (substitute_atom_in_eq_equation_stack2);
  SXBA_1_bit (equation_already_substituted_set, xeq);
  not_substituted_nb--;
  eq_ptr = equation [xeq];
  kind1 = equation2vstr_kind [xeq];

  for (atom = max_atom; atom > 0; atom--) {
    if (atom2var_nb [atom] != 0 && eq_ptr [atom] != 0)
      break;
  }

  /* On reporte la valeur de atom ds toutes les equations qui l'utilisent */

  while (!IS_EMPTY (substitute_atom_in_eq_equation_stack2)) {
    xeq2 = POP (substitute_atom_in_eq_equation_stack2);
    eq_ptr2 = equation [xeq2];
    Xnb = eq_ptr [atom];
    Xnb2 = eq_ptr2 [atom];

    /* on remplace */
    if (Xnb != 1) {
      /* On multiplie tout par Xnb */
      for (atom2 = 0; atom2 <= max_atom; atom2++) {
	if (atom2var_nb [atom2] != 0 && eq_ptr2 [atom2] != 0)
	  eq_ptr2 [atom2] *= Xnb;
      }
    }

    /* et on retranche xeq multiplie par Xnb2 */
    for (atom2 = 0; atom2 <= atom; atom2++) {
      if (atom2var_nb [atom2] != 0 && (v = eq_ptr [atom2]) != 0) {
	if (atom2 > 0 && eq_ptr2 [atom2] == 0)
	  /* prudence !! */
	  equation2Xnb [xeq2]++;

	eq_ptr2 [atom2] -= Xnb2*v;

	if (atom2 > 0 && eq_ptr2 [atom2] == 0)
	  equation2Xnb [xeq2]--;
      }
    }

    /* atom a disparu de eq_ptr2 */
#if EBUG
    if (eq_ptr2 [atom] != 0)
      sxtrap (ME, "substitute_atom_in_eq");
#endif

    /* mise a jour de la partie droite de l'equation */
    /* cas general "(a2)*(Xnb)-(a1)*(Xnb2)" */
    kind2 = equation2vstr_kind [xeq2];

    if (kind1 == UNKNOWN_k || kind2 == UNKNOWN_k)
      kind = UNKNOWN_k;
    else {
      if (kind1 != NULL_k) {
	if (Xnb2 < 0)
	  kind1 = -kind1;
      }

      if (kind2 != NULL_k) {
	if (Xnb < 0)
	  kind2 = -kind2;
      }

      if (kind1 == kind2)
	if (kind1 == NULL_k)
	  kind = NULL_k;
	else
	  kind = UNKNOWN_k;
      else {
	if (kind1+kind2 > 0)
	  kind = GEQ0_k;
	else
	  kind = LEQ0_k;
      }
    }

    i = 0;

    for (atom2 = 0; atom2 <= max_atom; atom2++) {
      sxinitialise (v); /* pour faire taire gcc -Wuninitialized */

      if (atom2var_nb [atom2] != 0 && (v = eq_ptr2 [atom2]) > 0)
	break;

      if (v < 0) i = -1;
    }

    /* Si tous les atomes sont <= 0, et si un au moins est < 0, on change les signes! */
    /* "(a2)*(-Xnb)-(a1)*(-Xnb2)" */

    if (atom2 > max_atom && i < 0) {
      for (atom2 = 0; atom2 <= max_atom; atom2++) {
	if (atom2var_nb [atom2] != 0 && (v = eq_ptr2 [atom2]) < 0)
	  eq_ptr2 [atom2] = -v;
      }

      Xnb = -Xnb;
      Xnb2 = -Xnb2;

      if (kind == GEQ0_k)
	kind = LEQ0_k;
      else {
	if (kind == LEQ0_k)
	  kind = GEQ0_k;
      }
    }

    wvstr = varstr_raz (wvstr);

    if (kind2 != NULL_k) {
      if (Xnb != 1) {
	wvstr = varstr_catenate (wvstr, "(");
	wvstr = varstr_catenate (wvstr, varstr_tostr (equation2vstr [xeq2]));
	sprintf (string, ")*(%ld)", (SXINT) Xnb);
	wvstr = varstr_catenate (wvstr, string);
      }
      else {
	wvstr = varstr_catenate (wvstr, varstr_tostr (equation2vstr [xeq2]));
      }
    }

    if (kind1 != NULL_k) {
      if (Xnb2 < 0) {
	wvstr = varstr_catenate (wvstr, "+(");
	Xnb2 = -Xnb2;
      }
      else
	wvstr = varstr_catenate (wvstr, "-(");

      wvstr = varstr_catenate (wvstr, varstr_tostr (equation2vstr [xeq]));
      wvstr = varstr_catenate (wvstr, ")");
			
      if (Xnb2 != 1) {
	sprintf (string, "*(%ld)", (SXINT) Xnb2);
	wvstr = varstr_catenate (wvstr, string);
      }
    }
		    
    if (kind == NULL_k)
      equation2vstr [xeq2] = varstr_catenate (varstr_raz (equation2vstr [xeq2]), "0");
    else
      equation2vstr [xeq2] = varstr_catenate (varstr_raz (equation2vstr [xeq2]),
					      varstr_tostr (wvstr));
    equation2vstr_kind [xeq2] = kind;

    Xnb2 = equation2Xnb [xeq2];

    if (Xnb2 <= 1) {
      SXBA_0_bit (nyp_eq_set, xeq2);
      not_substituted_nb--;

      if (Xnb2 == 0) {
	if (!process_0_atom (xeq2, exit_stmt))
	  is_useful = false;
      }
      else {
	PUSH (equation_stack, xeq2);
      }
    }
  }

  return is_useful;
}


static void
set_Asize_set (char *exit_stmt)
{
  SXINT		atom, xeq, Xnb, nb, kind, atom2;
  SXINT		*eq_ptr;
  bool	is_assign;
  char	string [32], *Asize_str;
  VARSTR	vstr;

  while (!IS_EMPTY (atom_stack)) {
    atom = POP (atom_stack);
    Asize_str = varstr_tostr (atom2rsize [atom]);
	
    for (xeq = 0; xeq < max_equation; xeq++) {
      /* On regarde aussi si du fait des StrEq[Len], on peut calculer de nouvelles tailles */
      eq_ptr = equation [xeq];

      if ((nb = equation2Xnb [xeq]) >= 2 && (Xnb = eq_ptr [atom]) != 0) {
	/* On remplace */
	eq_ptr [atom] = 0;
	--equation2Xnb [xeq];

	kind = equation2vstr_kind [xeq];

	/* On repere le cas &StrEq(X, Y) afin de rendre equivalent Asize[X] et Asize[Y] */
	if (nb == 2 && xeq >= lhs_equation && kind == NULL_k && eq_ptr [0] == 0) {
	  /* candidat */
	  for (atom2 = 1; atom2 <= max_atom; atom2++) {
	    if (atom2var_nb [atom2] != 0 && atom2 != atom && eq_ptr [atom2] == -Xnb)
	      /* OK */
	      break;
	  }

	  if (atom2 <= max_atom) {
	    eq_ptr [atom2] = 0;
	    equation2Xnb [xeq] = 0;

	    is_assign = SXBA_bit_is_reset_set (Asize_set, atom2);

	    if (is_assign) {
	      atom2rsize [atom2] = varstr_catenate (varstr_raz (atom2rsize [atom2]), Asize_str);
	      PUSH (atom_stack, atom2);
	    }
	    else {
	      fprintf (parser_file, "if (%s != %s) %s;\n",
		       Asize_str,
		       varstr_tostr (atom2rsize [atom2]),
		       exit_stmt); 
	    }

	    continue;
	  }
	}


	switch (kind) {
	case UNKNOWN_k:
	  break;

	case GEQ0_k:
	  if (Xnb > 0)
	    kind = UNKNOWN_k;
	  break;

	case NULL_k:
	  kind = Xnb < 0 ? GEQ0_k : LEQ0_k;
	  break;

	case LEQ0_k:
	  if (Xnb < 0)
	    kind = UNKNOWN_k;
	  break;
	default:
#if EBUG
	  sxtrap(ME,"unknown switch case #3");
#endif
	  break;
	}

	equation2vstr_kind [xeq] = kind;

	vstr = equation2vstr [xeq];

	if (Xnb == 1)
	  vstr = varstr_catenate (vstr, "-");
	else {
	  if (Xnb == -1)
	    vstr = varstr_catenate (vstr, "+");
	  else {
	    if (Xnb > 0)
	      sprintf (string, "-(%ld)*", (SXINT) Xnb);
	    else
	      sprintf (string, "+(%ld)*", (SXINT) -Xnb);

	    vstr = varstr_catenate (vstr, string);
	  }
	}

	vstr = varstr_catenate (vstr, Asize_str);

	/* ATTENTION, isize peut etre negatif (par le jeu des substitutions) */
	if (nb > 2) {
	  if (xeq >= lhs_arity) {
	    /* On ne cree pas de isize sur les predicats predefinis */
	    /* Ni sur les equations resultant des points de synchro */
	    equation2vstr [xeq] = vstr;
	  }
	  else {
#if EBUG
	    if (next_ii >= Aisize_nb)
	      sxtrap (ME, "set_Asize_set");
#endif
	    /* Le 28/09/2001 ajout du test.  On peut faire beaucoup mieux */
	    /* En tenant compte ds le calcul du Asize, ds l'init du for, du nb
	       d'atomes de meme taille (streq[len]) qui restent ds le sous-arg et faire qqchose du style
	       for (Asize[i] = (taille_restante - taille_connue)/atom_nb; ...)
	       ou taille connue ne doit pas prendre en compte les autres atomes de taille egale et
	       atom_nb (si atom_nb>1), est le nb d'atomes de taille egale */
	    fprintf (parser_file, "if ((isize[%ld] = %s) < 0) %s;\n",
		     (SXINT) next_ii, varstr_tostr (vstr), exit_stmt);
	    /* le < 0 peut etre change en < k, avec k==taille min connue de ce qui reste, sans
	       tenir compte des tailles egales */
#if 0
	    fprintf (parser_file, "isize[%ld] = %s;\n",
		     next_ii, varstr_tostr (vstr));
#endif
	    sprintf (string, "isize[%ld]", (SXINT) next_ii);
	    equation2vstr [xeq] = varstr_catenate (varstr_raz (equation2vstr [xeq]), string);
	    next_ii++;
	  }
	}
	else {
	  equation2vstr [xeq] = vstr;
	  /* Il reste une seule inconnue */
	  PUSH (equation_stack, xeq);

	  if (xeq < lhs_equation) {
	    SXBA_0_bit (nyp_eq_set, xeq);
	    not_substituted_nb--;
	  }
	}
      }
    }
  }
}

static bool
propagate_1var_size (void)
{
  /* equation_stack contient les identifiants des equations qui ne contiennent plus qu'une variable.
     On peut donc calculer statiquement sa taille et la propager */
  SXINT		xeq, atom, Xnb, size, xeq2;
  SXINT		*eq_ptr, *eq_ptr2;
  char	string [12];
  bool	is_useful = true;;

  while (!IS_EMPTY (equation_stack)) {
    xeq = POP (equation_stack);

    if (xeq >= lhs_equation && equation2Xnb [xeq] == 1) {
      /* prudence elle a pu etre substituee entre temps */
      sxinitialise (Xnb); /* pour faire taire gcc -Wuninitialized */
      equation2Xnb [xeq] = 0;
      eq_ptr = equation [xeq];

      for (atom = 1; atom <= max_atom; atom++) {
	if (atom2var_nb [atom] != 0 && (Xnb = eq_ptr [atom]) != 0)
	  break;
      }

      /* On calcule la taille de l'atome */
      size = eq_ptr [0];

      if (size != 0) {
	/* Xnb et size doivent etre de signe contraire */
	if (Xnb*size > 0) {
	  is_useful = false;
	  size = 0; /* On propage 0 */
	}
	else {
	  if (Xnb < 0)
	    Xnb = -Xnb;
	  else
	    size = -size;

	  /* size doit etre divisible par Xnb */
	  if (Xnb > 1) {
	    if (size % Xnb) {
	      is_useful = false;
	      size = 0; /* On propage 0 */
	    }
	    else
	      size /= Xnb;
	  }
	}
      }

      if (size < atom2min [atom] || (atom2max [atom] >= 0 && size > atom2max [atom])) {
	/* La taille doit etre >= au nb de terminaux composant l'atome */
	is_useful = false;
	size = 0; /* On propage 0 */
      }

      /* clear */
      eq_ptr [atom] = 0;
      eq_ptr [0] = 0;	

      sprintf (string, "%ld", (SXINT) size);
      atom2rsize [atom] = varstr_catenate (varstr_raz (atom2rsize [atom]), string);
      /* La taille de atom est calculee */
      SXBA_1_bit (Asize_set, atom);
      atom2static_size [atom] = size;

      for (xeq2 = max_equation-1; xeq2 >= 0; xeq2--) {
	if (equation2Xnb [xeq2] > 0 && xeq2 != xeq) {
	  eq_ptr2 = equation [xeq2];

	  if ((Xnb = eq_ptr2 [atom]) != 0) {
	    eq_ptr2 [atom] = 0;
	    eq_ptr2 [0] += size*Xnb;
  
	    Xnb = --equation2Xnb [xeq2];

	    if (xeq2 >= lhs_equation) {
	      /* equation "statique" */
	      if (Xnb == 0) {
		if (eq_ptr2 [0] != 0) {
		  /* Incompatibilite (dynamique!) des tailles */
		  is_useful = false;
		  eq_ptr2 [0] = 0;
		}
	      }
	      else {
		if (Xnb == 1)
		  PUSH (equation_stack, xeq2);
	      }
	    }
	  }
	}
      }
    }
  }

  return is_useful;
}



static SXINT
get_lhs_ladr (SXINT tnb, SXINT var)
{
  /* retourne l'adresse du 1er arg de la LHS qui
     designent .wX avec |w|==tnb, w \in T^* et X == var */
  SXINT		pos, cur2, param, bot3, top3, cur3, adr, t, symb;

  for (pos = 0, cur2 = lhs_bot2+2; cur2 < lhs_top2; pos++, cur2++) {
    param = XH_list_elem (rcg_predicates, cur2);
	
    if (param > 0) {
      bot3 = XH_X (rcg_parameters, param);
      top3 = XH_X (rcg_parameters, param+1);
      t = 0;

      for (adr = lhs_pos2adr [pos], cur3 = bot3; cur3 < top3; adr++, cur3++) {
	symb = XH_list_elem (rcg_parameters, cur3);

	if (symb < 0)
	  t++;
	else {
	  if (symb == var)
	    break;

	  t = 0; /* On recommence */
	}
      }

      if (cur3 < top3 && tnb <= t)
	/* Ca tient */
	return adr-tnb;
    }
  }

  return 0;
}


static SXINT
get_lhs_uadr (SXINT tnb, SXINT var)
{
  /* Retourne l'adresse du 1er arg de la LHS qui
     designe Xw. avec |w|==tnb, w \in T^* et X == var */
  SXINT		pos, cur2, param, bot3, top3, cur3, adr, t, symb;

  for (pos = 0, cur2 = lhs_bot2+2; cur2 < lhs_top2; pos++, cur2++) {
    param = XH_list_elem (rcg_predicates, cur2);
	
    if (param > 0) {
      bot3 = XH_X (rcg_parameters, param);
      top3 = XH_X (rcg_parameters, param+1);
      t = 0;

      for (adr = lhs_pos2radr [pos], cur3 = top3-1; bot3 <= cur3; adr--, cur3--) {
	/* Ici adr est en avance d'un symbole sur cur3. */
	symb = XH_list_elem (rcg_parameters, cur3);

	if (symb < 0)
	  t++;
	else {
	  if (symb == var)
	    break;

	  t = 0;	/* On recommence */
	}
      }

      if (bot3 <= cur3 && tnb <= t)
	/* Ca tient */
	return adr+tnb;
    }
  }

  return 0;
}



static bool
is_normal_form (SXINT xeq)
{
  /* On regarde si le coef de l'atome max est >0, sinon on oppose tout */
  SXINT		atom, Xnb;
  SXINT		*eq_ptr;

  Xnb = 0;
  eq_ptr = equation [xeq];

  for (atom = max_atom; 0 <= atom; atom--) {
    if (atom2var_nb [atom] != 0 && (Xnb = eq_ptr [atom]) != 0)
      break;
  }

  if (Xnb >= 0)
    return true; /* Forme normale */

  /* On oppose */
  for (atom = 0; atom <= max_atom; atom++) {
    if (atom2var_nb [atom] != 0 && (Xnb = eq_ptr [atom]) != 0)
      eq_ptr [atom] = -Xnb;
  }

  return false;
}



static SXINT
get_equation_kind (SXINT xeq)
{
  SXINT		g0, l0, atom, Xnb;
  SXINT		*eq_ptr;

  l0 = g0 = 0;
  eq_ptr = equation [xeq];

  for (atom = 1; atom <= max_atom; atom++) {
    if (atom2var_nb [atom] != 0) {
      Xnb = eq_ptr [atom];

      if (Xnb > 0)
	g0++;
      else
	l0++;
    }
  }

  if (g0 > 0)
    return (l0 > 0) ? UNKNOWN_k : GEQ0_k;

  return (l0 > 0) ? LEQ0_k : NULL_k;
}


static bool
put_new_equation (SXINT prev_xeq, SXINT xeq, SXINT k1, SXINT bot1, SXINT cur1, SXINT k2, SXINT bot2, SXINT cur2)
{
  SXINT 	cur3, top3, tnb, Xnb, atom, symb, xeq2;
  SXINT		*eq_ptr = equation [xeq], *eq_ptr2;
  bool	ret_val;

  cur3 = bot1;
  top3 = cur1;
  tnb = 0;
  Xnb = 0;

  while (cur3 < top3) {
    symb = XH_list_elem (rcg_parameters, cur3);

    if (symb > 0) {
      /* On cherche a quel atome appartient symb */
      atom = var2atom [symb];
      eq_ptr [atom] = k1;
      cur3 += atom2var_nb [atom];
      Xnb++;
    }
    else {
      tnb++;
      cur3++;
    }
  }
		    
  eq_ptr [0] = tnb;
  equation2Xnb [xeq] = Xnb;

  cur3 = bot2;
  top3 = cur2;
  tnb = 0;
  Xnb = 0;

  while (cur3 < top3) {
    symb = XH_list_elem (rcg_parameters, cur3);

    if (symb > 0) {
      /* On cherche a quel atome appartient symb */
      atom = var2atom [symb];

      if (eq_ptr [atom] == 0) {
	eq_ptr [atom] = k2;
	Xnb++;
      }
      else {
	/* equation [xeq] [atom] == k1 */
	if ((eq_ptr [atom] += k2) == 0)
	  /* disparait */
	  Xnb--;
      }

      cur3 += atom2var_nb [atom];
    }
    else {
      tnb++;
      cur3++;
    }
  }

  equation2Xnb [xeq] += Xnb;
  eq_ptr [0] -= tnb;

  ret_val = is_normal_form (xeq); /* Le coef de l'atome max est > 0 */

  equation2vstr_kind [xeq] = get_equation_kind (xeq);

  if (prev_xeq > 0) {
    /* On est dans la partie ou la RHS des equations est vide */
    /* On regarde si cette equation n'existe pas deja entre prev_xeq et xeq-1 */
    /* On est ds le cas ou la RHS de l'equation est nulle, la valeur de ret_val
       est sans importance.  On l'utise donc pour savoir si l'equation est nouvelle */
    for (xeq2 = prev_xeq; xeq2 < xeq; xeq2++) {
      if (equation2Xnb [xeq] == equation2Xnb [xeq2]) {
	eq_ptr2 = equation [xeq2];

	for (atom = 0; atom <= max_atom; atom++) {
	  if (atom2var_nb [atom] != 0 && (eq_ptr [atom] != eq_ptr2 [atom]))
	    break;
	}

	if (atom > max_atom) {
	  for (atom = 0; atom <= max_atom; atom++) {
	    eq_ptr [atom] = 0;
	  }

	  equation2Xnb [xeq] = 0;

	  return false;
	}
      }
    }

    return true;
  }

  return ret_val; /* Changer vstr en -vstr si FAUX */
}


void
make_atom (void)
{
  SXINT		cur_var, var, next_var, atom, nb, couple; 

  for (cur_var = 1; cur_var <= max_var; cur_var++) {
    if (var2atom [cur_var] == 0) {
      /* atome inconnu */
      atom = cur_var;
      /* Le code interne d'un atome est le code de sa variable initiale */
      var2atom [cur_var] = atom;
      atom2var_nb [atom] = 1;
      var = cur_var;

      do {
	sxinitialise (next_var); /* pour faire taire gcc -Wuninitialized */
	nb = 0;
	    
	XxY_Xforeach (VxV, var, couple) {
	  nb++;

	  if (nb > 1 || (next_var = XxY_Y (VxV, couple)) == 0)
	    break;
	}

	if (couple == 0) {
	  /* next_var fait partie de l'atome s'il n'a que var comme predecesseur */
	  nb = 0;

	  XxY_Yforeach (VxV, next_var, couple) {
	    nb++;

	    if (nb > 1)
	      break;
	  }

	  if (couple == 0) {
	    var2atom [next_var] = atom;
	    atom2var_nb [atom]++;
	    var = next_var;
	  }
	}
      } while (couple == 0);
    }
    else {
      /* Le 31/03/2004 : atom2var_nb [atom] == 0 => atom invalide pour la clause courante */
      /* "cur_var" n'est pas le code d'un atome valide */
      atom2var_nb [cur_var] = 0;
    }
  }

  /* Ds les equations, atom==0 (eq_ptr[0]) repere nbt, cette valeur de atom doit donc etre consideree
     comme correcte.  Comme le test est_ce correct se fait par atom2var_nb [atom] != 0, on positionne
     atom2var_nb [0] a` 1 */
  atom2var_nb [0] = 1;
}


static bool
is_streq_spcl (SXINT bot)
{
  /* On est sur un &StrEq, on regarde s'il est de la forme &StrEq ("a", X) */
  SXINT	arg1, arg2, cur3;

  arg1 = XH_list_elem (rcg_predicates, bot);

  if (arg1 != 0) {
    if ((cur3 = XH_X (rcg_parameters, arg1)) == XH_X (rcg_parameters, arg1+1)-1 &&
	XH_list_elem (rcg_parameters, cur3) < 0) {
      /* Le 1er arg est un terminal */
      arg2 = XH_list_elem (rcg_predicates, bot+1);

      if ((cur3 = XH_X (rcg_parameters, arg2)) == XH_X (rcg_parameters, arg2+1)-1) {
	/* Le 2eme arg est une variable */
	return true;
      }
    }
  }

  return false;
}

static bool
is_lexicalized (SXINT clause)
{
  SXINT lhs_prdct, is_lexicalized_lhs_bot2, is_lexicalized_lhs_top2, cur2, param, cur3, top3;

  lhs_prdct = XxY_X (rcg_clauses, clause) & lhs_prdct_filter;
  is_lexicalized_lhs_bot2 = XH_X (rcg_predicates, lhs_prdct);
  is_lexicalized_lhs_top2 = XH_X (rcg_predicates, lhs_prdct+1);
 
  if (nt2arity [XH_list_elem (rcg_predicates, is_lexicalized_lhs_bot2+1)]) {
    /* En lhs si un predicat est d'arite 0, il a un param qui vaut 0 */
    for (cur2 = is_lexicalized_lhs_bot2+2; cur2 < is_lexicalized_lhs_top2; cur2++) {
      param = XH_list_elem (rcg_predicates, cur2);

      if (param > 0) {
	top3 = XH_X (rcg_parameters, param+1);

	for (cur3 = XH_X (rcg_parameters, param); cur3 < top3; cur3++) {
	  if (XH_list_elem (rcg_parameters, cur3) < 0)
	    return true;
	}
      }
    }
  }

  return false;
}


static bool
is_cyclic_clause_fun (SXINT clause)
{
  SXINT lhs_prdct, is_cyclic_clause_fun_lhs_bot2, lhs_nt, param;
  SXINT rhs, cur, bot, top, prdct, cur2, bot2, top2, nt, pos, symb, cur3, bot3, top3;
  SXINT lsymb, rcur, rprdct, rcur2, rbot2, rnt, rtop2, Ak;

  lhs_prdct = XxY_X (rcg_clauses, clause) & lhs_prdct_filter;
  is_cyclic_clause_fun_lhs_bot2 = XH_X (rcg_predicates, lhs_prdct);
  lhs_top2 = XH_X (rcg_predicates, lhs_prdct+1);
  lhs_nt = XH_list_elem (rcg_predicates, is_cyclic_clause_fun_lhs_bot2+1);
 
  if (nt2arity [lhs_nt]) {
    rhs = XxY_Y (rcg_clauses, clause);

    if (rhs) {
      top = XH_X (rcg_rhs, rhs+1);
      bot = XH_X (rcg_rhs, rhs);

      for (cur = bot; cur < top; cur++) {
	prdct = XH_list_elem (rcg_rhs, cur);
	bot2 = XH_X (rcg_predicates, prdct);
	top2 = XH_X (rcg_predicates, prdct+1);
	nt = XH_list_elem (rcg_predicates, bot2+1);
		 
	if (nt == lhs_nt && XH_list_elem (rcg_predicates, bot2) == 0) {
	  /* La clause est self-recursive */
	  /* Amelioration du traitement des cycles le Mer 30 Mai 2001 13:43:36 */
	  /* Cas des clauses non lexicalisees, self-recursives, non loop, d'une grammaire non-combinatoire */
	  /* Vu les conditions d'appel, on a
	     !is_combinatorial && !is_lex_clause && !SXBA_bit_is_set (loop_clause_set, clause) == true */
	  /* Ds ce cas, si au moins 1 arg a une taille qui diminue, la clause n'est pas cyclique */

	  for (pos = 0, cur2 = bot2+2; cur2 < top2; pos++, cur2++) {
	    param = XH_list_elem (rcg_predicates, cur2);

	    if (param > 0) {
	      /* non-combinatoire => arg compose d'une seule variable */
	      symb = XH_list_elem (rcg_parameters, XH_X (rcg_parameters, param));

	      /* param correspondant de la LHS */
	      param = XH_list_elem (rcg_predicates, is_cyclic_clause_fun_lhs_bot2+2+pos);

	      if (param > 0) {
		bot3 = XH_X (rcg_parameters, param);
		top3 = XH_X (rcg_parameters, param+1);

		if (top3-bot3 > 1) {
		  for (cur3 = bot3; cur3 < top3; cur3++) {
		    if (XH_list_elem (rcg_parameters, cur3) == symb)
		      break;
		  }

		  if (cur3 < top3) {
		    for (cur3 = bot3; cur3 < top3; cur3++) {
		      if ((lsymb = XH_list_elem (rcg_parameters, cur3)) != symb) {
			/* Si lsymb est un arg en RHS d'un predicat dont la taille min
			   est non nulle, alors clause n'est pas cyclique */
			for (rcur = bot; rcur < top; rcur++) {
			  rprdct = XH_list_elem (rcg_rhs, rcur);
			  rbot2 = XH_X (rcg_predicates, rprdct);
			  rnt = XH_list_elem (rcg_predicates, rbot2+1);

			  if (rnt > 0 && XH_list_elem (rcg_predicates, rbot2) == 0) {
			    rtop2 = XH_X (rcg_predicates, rprdct+1);

			    for (Ak = A_k2Ak (rnt, 0), rcur2 = rbot2+2; rcur2 < rtop2; Ak++, rcur2++) {
			      param = XH_list_elem (rcg_predicates, rcur2);

			      if (param > 0 &&
				  XH_list_elem (rcg_parameters, XH_X (rcg_parameters, param)) == lsymb
				  && Ak2min [Ak] > 0) {
				return false;
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
	}
      }
    }
  }

  return true;
}


#if 0
static void
process_lhs_constant_args (SXINT clause)
{
  SXINT lhs_prdct, lhs_bot2, lhs_nt, lhs_arity, lhs_top2, pos, cur2, symb, param;

  lhs_prdct = XxY_X (rcg_clauses, clause) & lhs_prdct_filter;
  lhs_bot2 = XH_X (rcg_predicates, lhs_prdct);
  lhs_nt = XH_list_elem (rcg_predicates, lhs_bot2+1);
  lhs_arity = nt2arity [lhs_nt];
 
  if (lhs_arity) {
    /* En lhs si un predicat est d'arite 0, il a un param qui vaut 0 */
    lhs_top2 = XH_X (rcg_predicates, lhs_prdct+1);
    pos = 0;

    for (cur2 = lhs_bot2+2; cur2 < lhs_top2; pos++, cur2++) {
      param = XH_list_elem (rcg_predicates, cur2);

      if (SXBA_bit_is_set (constant_param_set, param)) {
	/* Valeur entiere, pas range */
	symb = -XH_list_elem (rcg_parameters, XH_X (rcg_parameters, param))-max_ste-1;
	fprintf (parser_file, "if (iub[%ld]-ilb[%ld] != %ld) break;\n", pos, pos, symb);
      }
    }
  }
}


static void
process_rhs_constant_args (SXINT clause)
{
  SXINT rhs, top, bot, cur, rhs_pos, prdct, bot2, cur2, top2, nt, param, symb;

  rhs = XxY_Y (rcg_clauses, clause);
  top = XH_X (rcg_rhs, rhs+1);
  bot = XH_X (rcg_rhs, rhs);
  rhs_pos = 0;

  for (cur = bot; cur < top; cur++) {
    prdct = XH_list_elem (rcg_rhs, cur);
    bot2 = XH_X (rcg_predicates, prdct);
    nt = XH_list_elem (rcg_predicates, bot2+1);

    if (nt > 0) {
      top2 = XH_X (rcg_predicates, prdct+1);

      for (cur2 = bot2+2; cur2 < top2; rhs_pos++, cur2++) {
	param = XH_list_elem (rcg_predicates, cur2);

	if (SXBA_bit_is_set (constant_param_set, param)) {
	  /* Valeur entiere, pas range */
	  symb = -XH_list_elem (rcg_parameters, XH_X (rcg_parameters, param))-max_ste-1;
	  fprintf (parser_file, "oub[%ld] = (olb[%ld] = counter_lower_bound)+%ld;\n", rhs_pos, rhs_pos, symb);
	}
      }
    }
  }
}
#endif /* 0 */


static SXINT *pos2adr, *param2ladr, *pos2ladr, *param2radr, *pos2radr, *symb2madr, *symb2counter_madr;
static SXBA  dag_arg_tbp_set;
static SXBA  symb_in_strlen_set;
static SXINT *level_kind;

static void
alloc_gen_dag_parser (void)
{
  pos2adr = (SXINT*) sxalloc (max_garity+1, sizeof (SXINT));
  pos2ladr = (SXINT*) sxalloc (max_garity+1, sizeof (SXINT));
  pos2radr = (SXINT*) sxalloc (max_garity+1, sizeof (SXINT));

  param2ladr = (SXINT*) sxcalloc (XH_top (rcg_parameters)+1, sizeof (SXINT));
  param2radr = (SXINT*) sxalloc (XH_top (rcg_parameters)+1, sizeof (SXINT));

  symb2madr = (SXINT*) sxcalloc (max_gvar+1, sizeof (SXINT));
  symb2counter_madr = (SXINT*) sxcalloc (max_gvar+1, sizeof (SXINT));

  dag_arg_tbp_set = sxba_calloc (max_clause_arg_nb+1);

  symb_in_strlen_set = sxba_calloc (max_gvar+1);
  level_kind = (SXINT*) sxcalloc (max_gvar+1, sizeof (SXINT));
}

static void
free_gen_dag_parser (void)
{
  sxfree (pos2adr), pos2adr = NULL;
  sxfree (pos2ladr), pos2ladr = NULL;
  sxfree (pos2radr), pos2radr = NULL;

  sxfree (param2ladr), param2ladr = NULL;
  sxfree (param2radr), param2radr = NULL;

  sxfree (symb2madr), symb2madr = NULL;
  sxfree (symb2counter_madr), symb2counter_madr = NULL;

  sxfree (dag_arg_tbp_set), dag_arg_tbp_set = NULL;

  sxfree (symb_in_strlen_set), symb_in_strlen_set = NULL;
  sxfree (level_kind), level_kind = NULL;
}

static SXINT *clause2rhs_pos_disp;
#if 0
static SXINT *clause2symb_id_stack;
#endif /* 0 */

/* cas ou le texte source est un dag */
/* Appele' si !(is_left_linear_grammar && is_right_linear_grammar) */
/* Appele' ds tous les cas car sera aussi utilise' si l'on demande de generer le dag du yield de la rcg_spf */
static void
gen_nl_stmt_list (void)
{
  SXINT       lhs_prdct, lhs_bot2_gen_nl_stmt_list, lhs_top2_gen_nl_stmt_list, lhs_cur2, lhs_nt, rhs, top, bot, cur, pos, param, bot3, top3, cur3, size, clause, nb, rhs_pos, top_list, bot1, top1, cur1;
  SXINT       symb, prdct, bot2, top2, cur2, nt, cur_pos, Ak, x, max_symb, main_clause, top_symb_stack, next, pos1, pos2;
#if 0
  SXINT       adr;
#endif
  SXINT       symb_id, cur_lhs_t_nb;
  SXINT       *pos2symb_stack, *symb_stack, *symb2pos, *pos_stack, *symb_pos2next, *clause2nl_stmt_list_hd, *pos_symb_stack, *top_pos_symb_stack;
  SXINT       *rhs_pos2pos_stack_disp;
  XH_header   nl_stmt_list, XH_arg_pos_list, XH_pos_stack_disp;
  bool   is_a_true_non_linear_clause; 
#if 0
  bool   is_a_non_linear_clause; 
#endif

  pos2symb_stack = (SXINT *) sxalloc (max_clause_arg_nb+1, sizeof (SXINT));
  symb_stack = (SXINT *) sxalloc (max_clause_arg_nb*(max_arg_size+1)+1, sizeof (SXINT));
  symb2pos = (SXINT *) sxcalloc (max_gvar+1, sizeof (SXINT));
  pos_stack = (SXINT *) sxalloc (max_clause_arg_nb*(max_arg_size+1)+1, sizeof (SXINT));
  symb_pos2next = (SXINT *) sxalloc (max_clause_arg_nb*(max_arg_size+1)+1, sizeof (SXINT));
  clause2nl_stmt_list_hd = (SXINT *) sxalloc (last_clause+1, sizeof (SXINT));
  XH_alloc (&nl_stmt_list, "nl_stmt_list", last_clause+1, 1, 8, NULL, NULL);

#if 0
  DALLOC_STACK (symb_id_stack, last_clause*max_garity+1);
  clause2symb_id_stack = (SXINT*) sxalloc (last_clause+1, sizeof (SXINT)), clause2symb_id_stack [0] = 0;
#endif /* 0 */
  
  clause2rhs_pos_disp = (SXINT*) sxalloc (last_clause+1, sizeof (SXINT)), clause2rhs_pos_disp [0] = 0;
  rhs_pos2pos_stack_disp = (SXINT *) sxalloc (max_rhs_arg_nb+1, sizeof (SXINT));
  XH_alloc (&XH_pos_stack_disp, "XH_pos_stack_disp", last_clause+1, 1, max_rhs_arg_nb/2+1, NULL, NULL);
  XH_alloc (&XH_arg_pos_list, "XH_arg_pos_list", last_clause*max_garity+1, 1, 4, NULL, NULL);

  for (clause = 1; clause <= last_clause; clause++) {
#if 0
    DPUSH (symb_id_stack, 0); /* Le nb de var */
    clause2symb_id_stack [clause] = DTOP (symb_id_stack);
#endif /* 0 */

    clause2rhs_pos_disp [clause] = 0; /* init val */

#if 0
    is_a_non_linear_clause = false;
#endif

    if (!SXBA_bit_is_set (false_clause_set, clause)) {
      if ((main_clause = clause2identical [clause]) == clause) {
	lhs_prdct = XxY_X (rcg_clauses, clause) & lhs_prdct_filter;
	lhs_bot2_gen_nl_stmt_list = XH_X (rcg_predicates, lhs_prdct);
	lhs_top2_gen_nl_stmt_list = XH_X (rcg_predicates, lhs_prdct+1);
	lhs_nt = XH_list_elem (rcg_predicates, lhs_bot2_gen_nl_stmt_list+1);
	lhs_arity = nt2arity [lhs_nt];

	max_symb = 0;
	DRAZ (symb_stack);

	cur_lhs_t_nb = 0;

	for (Ak = A_k2Ak (lhs_nt, 0), pos = 0, lhs_cur2 = lhs_bot2_gen_nl_stmt_list+2;
	     lhs_cur2 < lhs_top2_gen_nl_stmt_list;
	     lhs_cur2++, pos++, Ak++) {
	  DPUSH (symb_stack, 0);
	  pos2symb_stack [pos] = top_symb_stack = DTOP (symb_stack);

	  param = XH_list_elem (rcg_predicates, lhs_cur2);

	  if (param > 0)  {
#if 0
	    if (!(is_left_linear_grammar && is_right_linear_grammar)
		&& SXBA_bit_is_set (non_linear_Ak_set, Ak))
	      is_a_non_linear_clause = true;
#endif

	    /* On fait c,a meme si la clause est lineaire */
	    bot3 = XH_X (rcg_parameters, param);
	    top3 = XH_X (rcg_parameters, param+1);
	    size = top3-bot3;

	    symb_stack [top_symb_stack] = size;
	    pos_stack [top_symb_stack] = pos;

	    for (cur3 = bot3; cur3 < top3; cur3++) {
	      symb = XH_list_elem (rcg_parameters, cur3);

	      DPUSH (symb_stack, symb);
	      top_symb_stack++;
	      pos_stack [top_symb_stack] = pos;
	      symb_pos2next [top_symb_stack] = 0;

	      if (symb > 0) {
		if (max_symb < symb)
		  max_symb = symb;

		next = symb2pos [symb];

		if (next) {
		  /* Occur multiple de symb */
		  symb_pos2next [top_symb_stack] = next;
		}
	      
		symb2pos [symb] = top_symb_stack;
	      }
	      else {
		if (symb >= -max_ste)
		  /* vrai terminal */
		  symb_pos2next [top_symb_stack] = ++cur_lhs_t_nb;
	      }
	    }
	  }
	}

	/* ... et ceux de la RHS. */
	rhs = XxY_Y (rcg_clauses, clause);
	cur_pos = lhs_arity;

	is_a_true_non_linear_clause = false;

	if (rhs) {
	  top = XH_X (rcg_rhs, rhs+1);
	  bot = XH_X (rcg_rhs, rhs);

	  for (cur = bot; cur < top; cur++) {
	    prdct = XH_list_elem (rcg_rhs, cur);
	    bot2 = XH_X (rcg_predicates, prdct);
	    top2 = XH_X (rcg_predicates, prdct+1);
	    nt = XH_list_elem (rcg_predicates, bot2+1);

	    if (nt > 0)
	      Ak = A_k2Ak (nt, 0);
	    else
	      Ak = 0;

	    for (cur2 = bot2+2; cur2 < top2; Ak++, cur2++) {
	      rhs_pos2pos_stack_disp [cur_pos-lhs_arity] = 0; /* initialisation */
	      DPUSH (symb_stack, 0);
	      pos2symb_stack [cur_pos] = top_symb_stack = DTOP (symb_stack);

	      if (nt > 0) {
#if 0
		if (!(is_left_linear_grammar && is_right_linear_grammar)
		    && SXBA_bit_is_set (non_linear_Ak_set, Ak))
		  is_a_non_linear_clause = true;
#endif

		param = XH_list_elem (rcg_predicates, cur2);

		if (param > 0) {
		  bot3 = XH_X (rcg_parameters, param);
		  top3 = XH_X (rcg_parameters, param+1);
		  size = top3-bot3;

		  symb_stack [top_symb_stack] = size;
		  pos_stack [top_symb_stack] = cur_pos;

		  for (cur3 = bot3; cur3 < top3; cur3++) {
		    symb = XH_list_elem (rcg_parameters, cur3);

		    DPUSH (symb_stack, symb);
		    top_symb_stack++;
		    pos_stack [top_symb_stack] = cur_pos;
		    symb_pos2next [top_symb_stack] = 0;

		    if (symb > 0) {
		      next = symb2pos [symb];

		      if (next) {
			/* Occur multiple de symb */
			if (pos_stack [next] >= lhs_arity)
			  /* ... en rhs */
			  is_a_true_non_linear_clause = true;

			symb_pos2next [top_symb_stack] = next;
		      }
	      
		      symb2pos [symb] = top_symb_stack;
		    }
		  }
		}
	      }

	      cur_pos++;
	    }
	  }
	}

	/* Ici Tout est connu et genere' */
	/* Pour chaque variable on fabrique la liste de ses occurrences */
	for (symb = 1; symb <= max_symb; symb++) {
	  nb = 0;

	  for (x = symb2pos [symb]; x != 0; x = symb_pos2next [x]) {
	    nb++;
	    pos = pos_stack [x];
	    XH_push (XH_arg_pos_list, pos);
	  }

	  XH_push (XH_arg_pos_list, nb);

	  XH_set (&XH_arg_pos_list, &symb_id);

#if 0
	  DPUSH (symb_id_stack, symb_id);
	  symb_id_stack [clause2symb_id_stack [clause]] = max_symb;
#endif /* 0 */

	  for (x = symb2pos [symb]; x != 0; x = symb_pos2next [x]) {
	    pos = pos_stack [x];

	    if (pos >= lhs_arity) {
	      rhs_pos2pos_stack_disp [pos-lhs_arity] = symb_id;
	    }
	  }
	}

	if (rhs) {
	  cur_pos -= lhs_arity;

	  for (pos = 0; pos < cur_pos; pos++) {
	    XH_push (XH_pos_stack_disp, rhs_pos2pos_stack_disp [pos]);
	  }

	  XH_set (&XH_pos_stack_disp, clause2rhs_pos_disp+clause);
	}

	size = 0; /* Le compte ne se compte pas */

	/* Si la grammaire est non-lineaire et si la clause courante necessite le calcul de sous-dags, on genere un appel */
	if (is_a_true_non_linear_clause) {
	  /* On commence par generer en 1er les intersections de sous-dags pour, a l'execution signaler un echec sans avoir
	     encore modifie de sous-dags ds la LHS */
	  nb = 0;

	  for (symb = 1; symb <= max_symb; symb++) {
	    pos1 = symb2pos [symb];

	    if (pos1 > 0 && pos_stack [pos1] >= lhs_arity) {
	      pos2 = symb_pos2next [pos1];

	      if (pos2 > 0 || pos_stack [pos2] >= lhs_arity) {
		/* Y'a de l'intersection */
		XH_push (nl_stmt_list, pos_stack [pos1]), size++;
		XH_push (nl_stmt_list, NL_LOAD); /* On met pos1 en pile d'execution */

		do {
		  /* On genere une intersection entre pos1 et pos2 */
		  XH_push (nl_stmt_list, pos_stack [pos2]), size++;
		  XH_push (nl_stmt_list, NL_LOAD), size++; /* On met pos2 en pile d'execution */
		  XH_push (nl_stmt_list, NL_AND), size++; /* On calcule l'intersection avec resultat en sommet de pile */
		} while ((pos2 = symb_pos2next [pos2]) > 0 && pos_stack [pos2] >= lhs_arity);

		/* Si cette intersection est non vide, on la reaffecte a l'ensemble des args non-lineaires, non faux!! */
		/* Elle sera reaffectee par union lors de la passe top_down suivante */
		symb2pos [symb] = - (++nb);
		/* Le resultat de cette intersection se trouvera ds la pile d'execution a l'index nb */
	      }
	    }
	  }
	}

#if 0
	adr = 0; /* Le symbole le + a gauche ds la liste des args de la lhs a l'"adresse" 1 */
#endif

	for (pos = 0; pos < lhs_arity; pos++) {
	  pos_symb_stack = symb_stack + pos2symb_stack [pos];

	  if ((nb = *pos_symb_stack) > 0) {
	    /* arg non vide */
	    /* Le 02/12/08 pour faciliter les deplacements ds la pile des instructions */
	    XH_push (nl_stmt_list, pos), size++;
	    XH_push (nl_stmt_list, NL_OR_START), size++;

	    top_pos_symb_stack = pos_symb_stack+nb;
	    nb = 0;

	    while (++pos_symb_stack <= top_pos_symb_stack) {
	      symb = *pos_symb_stack;
#if 0
	      adr++;  /* "adresse" du symb courant ds la liste des arguments */
#endif

	      if (symb < 0) {
		if (symb >= -max_ste) {
		  pos1 = pos_symb_stack-symb_stack;
		  cur_lhs_t_nb = symb_pos2next [pos1];

		  XH_push (nl_stmt_list, -cur_lhs_t_nb), size++;
		  XH_push (nl_stmt_list, NL_TERM), size++;
#if 0
		  /* adresse empilee au-dessus de l'operateur NL_TERM !! */
		  XH_push (nl_stmt_list, adr), size++;
#endif /* 0 */
		}
	      }
	      else {
		rhs_pos = symb2pos [symb];
		/* A FAIRE le cas combinatorial */

		if (rhs_pos < 0) {
		  /* non lineaire */
		  XH_push (nl_stmt_list, rhs_pos), size++; /*  < 0 */
		  XH_push (nl_stmt_list, NL_DUP), size++; /* On copie en sommet la valeur qui se trouve a l'index -rhs_pos */
		}
		else {
		  XH_push (nl_stmt_list, pos_stack [rhs_pos]), size++;
		  XH_push (nl_stmt_list, NL_LOAD), size++;
		}
	      }

	      if (++nb > 1) {
		XH_push (nl_stmt_list, NL_CAT), size++;
	      }
	    }

	    XH_push (nl_stmt_list, pos), size++;
	    XH_push (nl_stmt_list, NL_OR), size++;
	  }
	}
	      
	XH_push (nl_stmt_list, size);
	XH_set (&nl_stmt_list, clause2nl_stmt_list_hd+clause);

	for (symb = 1; symb <= max_symb; symb++)
	  symb2pos [symb] = 0;
      }
      else {
	clause2nl_stmt_list_hd [clause] = clause2nl_stmt_list_hd [main_clause];
      }
    }
    else
      clause2nl_stmt_list_hd [clause] = 0;
  }
	  
  sxfree (pos2symb_stack);
  sxfree (symb_stack);
  sxfree (symb2pos);
  sxfree (pos_stack);
  sxfree (symb_pos2next);

  fputs ("#if is_dag\n", parser_file);

  fputs ("#if 0\n\
/* Actually unused */\n", parser_file);
  /* On sort symb_id_stack une (pile de) pile qui contient pour chaque clause
     le nb de variables de cette clause et pour chaque variable un identifiant de
     XH_arg_pos_list */
  {
    SXINT list_top, cur_bot, cur_top, id;


#if 0
    top = DTOP (symb_id_stack);

    fprintf (parser_file, "/* Facilite le traitement de DAG dans le cas non-lineaire pour la passe top-down */\n\
static SXINT nl_var_id_stack [%ld] = {%ld,", (SXINT) (top+1), (SXINT) top);

    for (clause = 1; clause <= last_clause; clause++) {
      local_stack = symb_id_stack + clause2symb_id_stack [clause];

      nb = *local_stack++;
      fprintf (parser_file, "\n/* %ld */", (SXINT) nb);

      while (nb) {
	id = *local_stack++;
	fprintf (parser_file, " %ld,", (SXINT) (XH_X (XH_arg_pos_list, id)));
      }
    }

    fputs ("\n};\n", parser_file);
#endif /* 0 */


    fprintf (parser_file, "/* Facilite le traitement de DAG dans le cas non-lineaire pour la passe top-down */\n\
static SXINT nl_clause2rhs_pos_disp [%ld] = {0,", (SXINT) (last_clause+1));

    for (clause = 1; clause <= last_clause; clause++) {
      x = clause2rhs_pos_disp [clause];

      if (x)
	x = XH_X (XH_pos_stack_disp, x);

      fprintf (parser_file, "\n/* clause=%ld */ %ld,", (SXINT) clause, (SXINT) x);
    }

    fputs ("\n};\n", parser_file);
    
    top = XH_top (XH_pos_stack_disp);
    list_top = XH_list_top (XH_pos_stack_disp);

    fprintf (parser_file, "SXINT nl_rhs_pos2pos_stack_disp [%ld] = {%ld,", (SXINT) (list_top), (SXINT) (list_top-1));

    for (x = 1; x < top; x++) {
      cur_bot = XH_X (XH_pos_stack_disp, x);
      cur_top = XH_X (XH_pos_stack_disp, x+1);

      fprintf (parser_file, "\n/* [%ld..%ld] */", (SXINT)  cur_bot, (SXINT) (cur_top-1));

      while (cur_bot < cur_top) {
	id = XH_list_elem (XH_pos_stack_disp, cur_bot);
	fprintf (parser_file, " %ld,", (SXINT)  (XH_X (XH_arg_pos_list, id)));
	
	cur_bot++;
      }
    }

    fputs ("\n};\n", parser_file);

    /* On sort XH_arg_pos_list */
    top = XH_top (XH_arg_pos_list);
    list_top = XH_list_top (XH_arg_pos_list);

    fprintf (parser_file, "static SXINT nl_pos_stack [%ld] = {%ld,", (SXINT) (list_top), (SXINT) (list_top-1));

    for (id = 1; id < top; id++) {
      cur_bot = XH_X (XH_arg_pos_list, id);
      cur_top = XH_X (XH_arg_pos_list, id+1);

      fprintf (parser_file, "\n/* id=%ld [%ld..%ld]*/", (SXINT) id, (SXINT) cur_bot, (SXINT) (cur_top-1));

      while (--cur_top >= cur_bot) {
	/* de haut en bas pour les avoir ds le bon ordre */
	fprintf (parser_file, " %ld,", (SXINT) (XH_list_elem (XH_arg_pos_list, cur_top)));
      }
    }

    fputs ("\n};\n", parser_file);
  }

  fputs ("#endif /* 0 */\n", parser_file);

#if 0
  DFREE_STACK (symb_id_stack);
#endif /* 0 */

  XH_free (&XH_arg_pos_list);
  sxfree (rhs_pos2pos_stack_disp);
  XH_free (&XH_pos_stack_disp);

  /* On fabrique clause2nl_stmt_stack et on genere les tableaux nl_stmt_stack */
  {
    SXINT oper, operator, index_gen_nl_stmt_list;
    static char * NL_OPER [] = {"", "NL_AND", "NL_CAT", "NL_OR", "NL_LOAD", "NL_DUP", "NL_TERM", "NL_OR_START"};

    
    fprintf (parser_file, "static SXINT clause2nl_stmt_stack [%ld] = {0,\n", last_clause+1);

    for (clause = 1; clause <= last_clause; clause++) {
      x = clause2nl_stmt_list_hd [clause];

      if (x)
	x = XH_X (nl_stmt_list, x);
      
      fprintf (parser_file, "/* %ld */ %ld,\n", (SXINT) clause, (SXINT) x);
    }

    fputs ("};\n", parser_file);


    top = XH_top (nl_stmt_list)-1;
    top_list = XH_list_top (nl_stmt_list);

    fprintf (parser_file, "/* Permet le traitement de DAG dans le cas non-lineaire */\n\
static SXINT nl_stmt_stack [%ld] = {0,\n", top_list);
    
    index_gen_nl_stmt_list = 0;

    for (x = 1; x <= top; x++) {
      bot1 = XH_X (nl_stmt_list, x);
      top1 =  XH_X (nl_stmt_list, x+1);
    
      index_gen_nl_stmt_list++;

      fprintf (parser_file, "/* %ld */ %ld,", index_gen_nl_stmt_list, XH_list_elem (nl_stmt_list, --top1) /* size */);

      for (cur1 = bot1; cur1 < top1; cur1++) {
	index_gen_nl_stmt_list++;

	oper = XH_list_elem (nl_stmt_list, cur1);
	operator = oper >> 16;

	if (operator > 0)
	  fprintf (parser_file, " %s,", NL_OPER [operator]);
	else {
	  fprintf (parser_file, " %ld,", oper);

#if 0
	  if (oper >= 0 || terminal_file == NULL)
	    fprintf (parser_file, " %ld,", oper);
	  else
	    fprintf (parser_file, " -SXT_%s,", sxstrget (t2Cid_ste [-oper]));
#endif /* 0 */
	}
      }
  
      fputs ("\n", parser_file);
    }

    fputs ("};\n", parser_file);
  }

  
  fputs ("#include \"SS.h\"\n", parser_file);
  fputs ("static SXINT *SS_stack;\n", parser_file);

  fputs ("#endif /* is_dag */\n", parser_file);

  sxfree (clause2nl_stmt_list_hd);
  XH_free (&nl_stmt_list);
}

/* t_index est l'index ds rcg_parameters d'un vrai terminal ds l'arg lhs_arg_pos de la clause courante
   left_index est l'index gauche
   retourne le rang de ce terminal compte' de gauche a droite sur l'ensemble des args 
 */
static SXINT
get_Tpq_stack_pos (SXINT lhs_arg_pos, SXINT left_index, SXINT t_index)
{
  SXINT rank, symb;

  /* cumul du nb des terminaux situes ds les args a gauche de l'arg en position lhs_arg_pos */
  rank = lhs_arg_pos2prev_args_t_nb [lhs_arg_pos]+1;

  while (left_index < t_index) {
    symb = XH_list_elem (rcg_parameters, left_index);

    if (symb < 0 && symb >= -max_ste)
      /* vrai terminal */
      rank++;
  }

  return rank;
}

/* Le 22/04/08 (pour le cas is_dag)
   Pour chaque symbole de la LHS, on positionne la profondeur a laquelle il sera calcule' (on connaitra ses bornes inf et sup)
   initialement chaque symbole est a la profondeur 0 */
/* Le 05/05/09
   Si &StrLen (n, X) On genere

   olb [i] = adr2bound_values [j]; // i est la position de X ds le &StrLen en RHS et j est la position de X en LHS
   SS_open (SS_stack); // on prepare la sous-pile qui va recevoir l'ensemble des q qui se trouvent a la distance len de p = olb [i]
   SS_stack = idag_p_l2q_SS_stack (SS_stack, olb [2], n); // rempli cette sous-pile

   while (!SS_is_empty (SS_stack)) { // tant que cette sous-pile est non vide ...
      adr2bound_values [j+1] = SS_pop (SS_stack); // on pop le sommet
      oub [i] = adr2bound_values [j+1];
      if (_StrLen (&(rho [k]), &(olb [i]), &(oub [i]), n) == -1) exit_stmt; // Construit un noeud feuille special ds l'arbre abstrait

      ...
   }

   SS_close (SS_stack); // On ferme la sous-pile
 */
static void
gen_dag_parser (SXINT clause)
{
  SXINT     lhs_prdct, lhs_bot2_gen_dag_parser, lhs_top2_gen_dag_parser, lhs_cur2, lhs_nt, nb_of_arg_tbp, rhs, top, bot, cur, adr, pos, param, bot3, top3, cur3, size, level;
  SXINT     ladr, symb, madr, counter, radr, rhs_prdct_pos, prdct, bot2, top2, cur2, nt, cur_pos, k, Ak, len, x, rhs_prdct_arity, var, local_lhs_t_nb, Tpq_stack_pos, lhs_symb;
#if 0
  SXINT t_nb;
#endif
  bool has_changed, is_pos_call, rhs_prdct_args_are_known, is_a_counter, has_strlen; 
  char      *exit_stmt;
#if 0
  bool is_a_non_linear_clause; 
#endif

  sxinitialise (bot3); /* pour faire taire gcc -Wuninitialized */
  sxinitialise (top3); /* pour faire taire gcc -Wuninitialized */
  sxinitialise (size); /* pour faire taire gcc -Wuninitialized */
  sxinitialise (len); /* pour faire taire gcc -Wuninitialized */
  sxinitialise (counter); /* pour faire taire gcc -Wuninitialized */

  exit_stmt = "break";
  level = 0;

  lhs_prdct = XxY_X (rcg_clauses, clause) & lhs_prdct_filter;
  lhs_bot2_gen_dag_parser = XH_X (rcg_predicates, lhs_prdct);
  lhs_top2_gen_dag_parser = XH_X (rcg_predicates, lhs_prdct+1);
  lhs_nt = XH_list_elem (rcg_predicates, lhs_bot2_gen_dag_parser+1);
  nb_of_arg_tbp = lhs_arity = nt2arity [lhs_nt];
#if 0
  is_a_non_linear_clause = false;
#endif
  local_lhs_t_nb = 0;

  adr = 1;

  for (Ak = A_k2Ak (lhs_nt, 0), pos = 0, lhs_cur2 = lhs_bot2_gen_dag_parser+2;
       lhs_cur2 < lhs_top2_gen_dag_parser;
       lhs_cur2++, pos++, Ak++) {
    param = XH_list_elem (rcg_predicates, lhs_cur2);

    if (param > 0) {
      bot3 = XH_X (rcg_parameters, param);
      top3 = XH_X (rcg_parameters, param+1);
      size = top3-bot3;
    }

#if 0
    if (param > 0 && !(is_left_linear_grammar && is_right_linear_grammar) && SXBA_bit_is_set (non_linear_Ak_set, Ak)) {
      is_a_non_linear_clause = true;
    }
#endif

    if (param > 0) {
      pos2adr [pos] = adr;

      fprintf (parser_file, "adr2bound_values[%ld] = ilb[%ld], adr2bound_values[%ld] = iub[%ld];\n", adr, pos, adr+size, pos);

      if ((ladr = param2ladr [param]) == 0) {
	param2ladr [param] = pos2ladr [pos] = adr;
	param2radr [param] = pos2radr [pos] = adr+size;

	if (size == 1) {
	  symb = XH_list_elem (rcg_parameters, bot3);

	  if (symb > 0) {
	    if (SXBA_bit_is_set (counter_Ak_set, Ak)) {
	      /* La variable est un compteur */
	      if ((madr = symb2counter_madr [symb]) == 0) {
		/* 1ere fois qu'on tombe sur symb seul ds un compteur */
		symb2counter_madr [symb] = adr;
	      }
	      else {
		/* Elles ont obligatoirement meme lb */
		fprintf (parser_file, "if (adr2bound_values[%ld] != adr2bound_values[%ld]) %s;\n", adr+1, madr+1, exit_stmt);
	      }
	    }
	    else {
	      /* La variable est un range */
	      if ((madr = symb2madr [symb]) == 0) {
		/* 1ere fois qu'on tombe sur symb (seul) */
		symb2madr [symb] = adr;
	      }
	      else {
		fprintf (parser_file, "if (adr2bound_values[%ld] != adr2bound_values[%ld]\
|| adr2bound_values[%ld] != adr2bound_values[%ld] %s;\n",
			 adr, madr, adr+1, madr+1, exit_stmt);
	      }
	    }
	  }
	  else {
	    if (symb >= -max_ste) {
	      /* vrai terminal, on le scanne */
	      local_lhs_t_nb++;

	      if (terminal_file)
		fprintf (parser_file, "\
if ((rho [%ld] = p_T_q2Tpq (adr2bound_values[%ld], SXT_%s, adr2bound_values[%ld])) == 0) %s;\n",
			 (SXINT) (cur_rho_size+local_lhs_t_nb), adr, sxstrget (t2Cid_ste [-symb]), adr+1, exit_stmt);
	      else
		fprintf (parser_file, "\
if ((rho [%ld] = p_T_q2Tpq (adr2bound_values[%ld], %ld, adr2bound_values[%ld])) == 0) %s;\n",
			 (SXINT) (cur_rho_size+local_lhs_t_nb), adr, -symb, adr+1, exit_stmt);
	    }
	    else {
	      counter = -symb - max_ste - 1;
	      /* compteur */
	      fprintf (parser_file, "if (adr2bound_values[%ld]-adr2bound_values[%ld]!=%ld) %s;\n", adr+1, adr, counter, exit_stmt);
	    }
	  }

	  nb_of_arg_tbp--;
	}
	else {
	  if (SXBA_bit_is_set (counter_Ak_set, Ak)) {
	    /* On regarde si l'arg ne contient qu'une seule variable (et des compteurs),, ds ce cas la variable est connue */
	    counter = 0;
	    var = 0;

	    for (cur3 = bot3; cur3 < top3; cur3++) {
	      symb = XH_list_elem (rcg_parameters, cur3);

	      if (symb > 0) {
		if (var)
		  /* Rate' */
		  break;

		var = symb;
	      }
	      else {
		counter += -symb - max_ste - 1;
	      }
	    }

	    if (cur3 == top3) {
	      /* OK */
	      symb = var;

	      if (symb == 0) {
		/* arg de la forme "0 1 3" */
		fprintf (parser_file, "if (adr2bound_values[%ld]-adr2bound_values[%ld]!=%ld) %s;\n", adr+size, adr, counter, exit_stmt);
	      }
	      else {
		/* La variable est un compteur */
		if ((madr = symb2counter_madr [symb]) == 0) {
		  /* 1ere fois qu'on tombe sur symb variable unique ds un compteur */
		  symb2counter_madr [symb] = adr; /* On dit que c'est adr, c'est pas le cas si arg == "3 X ..." */
		  /* On verifie que le compteur est assez grand et on affecte adr+1 */
		  fprintf (parser_file, "if ((adr2bound_values[%ld]=adr2bound_values[%ld]-%ld)-adr2bound_values[%ld] < 0) %s;\n", adr+1, adr+size, counter, adr, exit_stmt);
		}
		else {
		  /* Elles ont obligatoirement meme lb */
		  fprintf (parser_file, "if ((adr2bound_values[%ld]=adr2bound_values[%ld]-%ld)-adr2bound_values[%ld] != adr2bound_values[%ld]) %s;\n", adr+1, adr+size, counter, adr, madr+1, exit_stmt);
		}
	      }

	      nb_of_arg_tbp--;
	    }
	    else
	      SXBA_1_bit (dag_arg_tbp_set, pos);
	  }
	  else {
	    SXBA_1_bit (dag_arg_tbp_set, pos);
	  }
	}
      }
      else {
	/* Cet arg a deja ete trouve', on verifie qu'il est identique au precedent, sauf si c'est une cste compteur (ge'ne're' + simple) */
	counter = 0;

	if (SXBA_bit_is_set (counter_Ak_set, Ak)) {
	  for (cur3 = bot3; cur3 < top3; cur3++) {
	    symb = XH_list_elem (rcg_parameters, cur3);

	    if (symb > 0) {
	      /* Rate' */
	      break;
	    }
	    else {
	      counter += -symb - max_ste - 1;
	    }
	  }

	  if (cur3 == top3) {
	    fprintf (parser_file, "if (adr2bound_values[%ld]-adr2bound_values[%ld]!=%ld) %s;\n", adr, adr+size, counter, exit_stmt);
	    counter = -1;
	  }
	}

	if (counter == 0) {
	  radr = param2radr [param];
	  fprintf (parser_file, "if (adr2bound_values[%ld] != adr2bound_values[%ld] || adr2bound_values[%ld] != adr2bound_values[%ld]) %s;\n", adr, ladr, adr+size, radr, exit_stmt);
	}

	nb_of_arg_tbp--;
      }

      adr += size+1;
    }
    else {
      fprintf (parser_file, "if (ilb[%ld] != iub [%ld]) %s;\n", pos, pos, exit_stmt);
      nb_of_arg_tbp--;
    }
  }

  /* ... et ceux de la RHS. */

  rhs = XxY_Y (rcg_clauses, clause);
  has_strlen = false;

  if (rhs) {
    top = XH_X (rcg_rhs, rhs+1);
    bot = XH_X (rcg_rhs, rhs);
  }
  else
    bot = top = 0;

  cur_pos = lhs_arity;

  for (cur = bot; cur < top; cur++) {
    prdct = XH_list_elem (rcg_rhs, cur);
    bot2 = XH_X (rcg_predicates, prdct);
    top2 = XH_X (rcg_predicates, prdct+1);
    nt = XH_list_elem (rcg_predicates, bot2+1);

    if (nt > 0)
      Ak = A_k2Ak (nt, 0);
    else {
      Ak = 0;

      if (nt == STRLEN_ic) {
	has_strlen = true;
	param = XH_list_elem (rcg_predicates, bot2+3);
	cur3 = XH_X (rcg_parameters, param+1)-1;
	symb = XH_list_elem (rcg_parameters, cur3); /* symbole le + a droite du 2eme arg de StrLen */
	SXBA_1_bit (symb_in_strlen_set, symb);
      }
    }

    for (cur2 = bot2+2; cur2 < top2; Ak++, cur2++) {
#if 0
      if (nt > 0 && !(is_left_linear_grammar && is_right_linear_grammar) && SXBA_bit_is_set (non_linear_Ak_set, Ak)) {
	is_a_non_linear_clause = true;
      }
#endif

      nb_of_arg_tbp++;
      SXBA_1_bit (dag_arg_tbp_set, cur_pos);
      cur_pos++;
    }
  }

  while (nb_of_arg_tbp) {
    for (cur_pos = lhs_arity, rhs_prdct_pos = 1, cur = bot; cur < top; rhs_prdct_pos++, cur_pos += rhs_prdct_arity, cur++) {
      prdct = XH_list_elem (rcg_rhs, cur);
      bot2 = XH_X (rcg_predicates, prdct);
      top2 = XH_X (rcg_predicates, prdct+1);

      rhs_prdct_arity = top2-bot2-2;

      if (SXBA_bit_is_set (dag_arg_tbp_set, cur_pos)) {
	is_pos_call = XH_list_elem (rcg_predicates, bot2) == 0;
	nt = XH_list_elem (rcg_predicates, bot2+1);

	rhs_prdct_args_are_known = true;
	pos = cur_pos;

	/* En rhs si un predicat est sans argument A(), on a bot2+2==top2 */
	for (k = 0, cur2 = bot2+2; cur2 < top2; pos++, k++, cur2++) {
	  param = XH_list_elem (rcg_predicates, cur2);

	  if (param > 0) {
	    if (param2ladr [param] == 0) {
	      if (nt == STRLEN_ic && k == 0) {
		/* arg known */
	      }
	      else {
		bot3 = XH_X (rcg_parameters, param);
		top3 = XH_X (rcg_parameters, param+1);

		for (cur3 = bot3; cur3 < top3; cur3++) {
		  symb = XH_list_elem (rcg_parameters, cur3);

		  if (symb > 0 && symb2madr [symb] == 0 && symb2counter_madr [symb] == 0) {
		    /* unknown */
		    rhs_prdct_args_are_known = false;
		    break;
		  }
		}
	      }
	    }
	    else {
	      /* Cet arg se trouve en LHS, il est donc calcule' */
	    }
	  }
	}

	if (!is_pos_call)
	  /* Pour l'instant */
	  sxtrap (ME, "gen_dag_parser (Negative predicate calls are not yet supported)");

	if (rhs_prdct_args_are_known) {
	  /* Tous les args sont calcules, on peut donc generer */
	  /* On commence par affecter les o[l|u]b */
	  pos = cur_pos-lhs_arity;

	  if (nt > 0)
	    Ak = A_k2Ak (nt, 0);
	  else
	    Ak = 0;

	  for (k = 0, cur2 = bot2+2; cur2 < top2; pos++, k++, Ak++, cur2++) {
	    param = XH_list_elem (rcg_predicates, cur2);

	    if (param > 0) {
	      nb_of_arg_tbp--;
	      SXBA_0_bit (dag_arg_tbp_set, pos+lhs_arity);

	      if ((ladr = param2ladr [param]) == 0) {
		bot3 = XH_X (rcg_parameters, param);

		if (nt == STRLEN_ic && k == 0) {
		  /* arg known */
		  len = XH_list_elem (rcg_parameters, bot3);
		}
		else {
		  top3 = XH_X (rcg_parameters, param+1);
	
		  is_a_counter = (nt > 0 && SXBA_bit_is_set (counter_Ak_set, Ak) /* Contient le cas ou l'arg contient des constantes */)
		    || !rhs_arg2sub_arg (lhs_prdct, param);

		  if (is_a_counter) {
		    counter = 0;
		    fprintf (parser_file, "oub[%ld] = (olb[%ld] = counter_lb)", pos, pos);

		    for (cur3 = bot3; cur3 < top3; cur3++) {
		      symb = XH_list_elem (rcg_parameters, cur3);

		      if (symb > 0) {
			/* Attention, symb en lhs peut provenir d'un range */
			madr = symb2counter_madr [symb];

			if (madr == 0)
			  madr = symb2madr [symb];

			fprintf (parser_file, "+(adr2bound_values[%ld]-adr2bound_values[%ld])", madr+1, madr);
		      }
		      else {
			counter += -symb - max_ste - 1;
		      }
		    }

		    if (counter)
		      fprintf (parser_file, "+%ld;\n", counter);
		    else
		      fputs (";\n", parser_file);
		  }
		  else {
		    symb = XH_list_elem (rcg_parameters, top3-1);

		    if (symb < 0)
		      sxtrap (ME, "gen_dag_parser (Terminals in the arguments of RHS predicates are not yet supported)");

		    madr = symb2madr [symb];
		    fprintf (parser_file, "oub[%ld] = adr2bound_values[%ld], ", pos, madr+1);

		    symb = XH_list_elem (rcg_parameters, bot3);

		    if (symb < 0)
		      sxtrap (ME, "gen_dag_parser (Terminals in the arguments of RHS predicates are not yet supported)");

		    madr = symb2madr [symb];
		    fprintf (parser_file, "olb[%ld] = adr2bound_values[%ld];\n", pos, madr);
		  }
		}
	      }
	      else {
		/* Cet arg se trouve en LHS, il est donc calcule' */
		fprintf (parser_file, "olb [%ld] = adr2bound_values[%ld], oub [%ld] = adr2bound_values[%ld];\n", pos, ladr, pos, param2radr [param]);
	      }
	    }
	  }

	  pos = cur_pos-lhs_arity;

	  if (nt > 0) {
	    fprintf (parser_file,
		     "if (_%s(&(rho[%ld]), &(olb[%ld]), &(oub[%ld]))==-1) %s;\n",
		     sxstrget (nt2ste [nt]), rhs_prdct_pos+1, pos, pos, exit_stmt);
	  }
	  else {
	    switch (nt) {
	    case STRLEN_ic:
	      /* Fait differemment maintenant (05/05/09) */
	      fprintf (parser_file,
		       "if (_StrLen (&(rho[%ld]), &(olb[%ld]), &(oub[%ld]), %ld)==-1) %s;\n",
		       rhs_prdct_pos+1, pos, pos, len, exit_stmt); /* meme si len == 0 */
#if 0
	      /* Il faut exploiter len==0 */
	      fprintf (parser_file, "if (_StrLen (%ld, olb [%ld], oub [%ld]) < 0) %s;\n", len, pos, pos, exit_stmt);
#endif /* 0 */
	      break;

	    case STREQLEN_ic:
	      fprintf (parser_file, "if (_StrEqLen (olb [%ld], oub [%ld], olb [%ld], oub [%ld]) < 0) %s;\n", pos, pos, pos+1, pos+1, exit_stmt);
	      break;

	    case STREQ_ic:
	      fprintf (parser_file, "if (_StrEq (olb [%ld], oub [%ld], olb [%ld], oub [%ld]) < 0) %s;\n", pos, pos, pos+1, pos+1, exit_stmt);
	      break;

	    default:
	      sxtrap (ME, "gen_dag_parser (Some system predicates are not yet supported)");
	      break;
	    }
	  }
	}
      }
    }

    if (nb_of_arg_tbp == 0)
      /* fini */
      break;

    /* On traite les args de la lhs de taille size (s'il y en a) */
    has_changed = false;

    for (Ak = A_k2Ak (lhs_nt, 0), pos = 0, lhs_cur2 = lhs_bot2_gen_dag_parser+2; !has_changed && lhs_cur2 < lhs_top2_gen_dag_parser; lhs_cur2++, pos++, Ak++) {
      if (SXBA_bit_is_set (dag_arg_tbp_set, pos)) {
	/* Non calcule' */
	param = XH_list_elem (rcg_predicates, lhs_cur2);

	if (param > 0) {
	  bot3 = XH_X (rcg_parameters, param);
	  top3 = XH_X (rcg_parameters, param+1);

	  ladr = pos2ladr [pos];
	  radr = pos2radr [pos];
	  /* On a ... ladr X Y ... radr ...
	     i.e. ladr+1 < radr */
	  cur3 = bot3 + (ladr-pos2adr [pos]);
	  symb = XH_list_elem (rcg_parameters, cur3);

	  if (symb > 0) {
	    if (SXBA_bit_is_set (counter_Ak_set, Ak)) {
	      /* L'arg est un compteur */
	      counter = 0;

	      for (x = cur3+1; x < top3; x++) {
		symb = XH_list_elem (rcg_parameters, x);

		if (symb < -max_ste)
		  counter +=  -symb - max_ste - 1;
	      }

	      if ((madr = symb2counter_madr [symb]) == 0) {
		/* 1ere fois qu'on tombe sur symb ds un compteur */
		symb2counter_madr [symb] = ladr;

		fprintf (parser_file, "for (adr2bound_values[%ld] = adr2bound_values[%ld]-%ld; adr2bound_values[%ld] >= adr2bound_values[%ld]; adr2bound_values[%ld]--) {\n", ladr+1, radr, counter, ladr+1, ladr, ladr+1);

		if (level++ == 0)
		  exit_stmt = "continue";

		has_changed = true;
	      }
	      else {
		fprintf (parser_file, "adr2bound_values[%ld] = adr2bound_values[%ld];\n", ladr+1, madr+1);
		fprintf (parser_file, "if (adr2bound_values[%ld]+%ld > adr2bound_values[%ld]) %s;\n", ladr+1, counter, radr, exit_stmt);
	      }
	    }
	    else {
	      if (SXBA_bit_is_set (symb_in_strlen_set, symb)) {
		/* Ce symb est implique' ds un StrLen (c'est le dernier de l'arg) */
		/* A FAIRE : Si c'est un compteur ? */
		if ((madr = symb2madr [symb]) == 0) {
		  /* 1ere fois qu'on tombe sur symb */
		  /* On recherche le/les StrLen dans lesquels se trouve symb */
  
		  for (cur = bot; cur < top; cur++) {
		    prdct = XH_list_elem (rcg_rhs, cur);
		    bot2 = XH_X (rcg_predicates, prdct);
		    top2 = XH_X (rcg_predicates, prdct+1);
		    nt = XH_list_elem (rcg_predicates, bot2+1);

		    if (nt == STRLEN_ic) {
		      SXINT bot4, top4, cur4;

		      param = XH_list_elem (rcg_predicates, bot2+3);
		      top4 = XH_X (rcg_parameters, param+1);

		      if (symb == XH_list_elem (rcg_parameters, top4-1)) {
			/* trouve' */
			bot4 = XH_X (rcg_parameters, param);
			madr = ladr;

			if (top4-bot4 > 1) {
			  /* arg combinatoire, on regarde s'il est inclus ds celui de la LHS */
			  for (cur4 = top4-2, x = cur3-1; cur4 >= bot4 && x >= bot3; cur4--, x--) {
			    if (XH_list_elem (rcg_parameters, x) != XH_list_elem (rcg_parameters, cur4)) {
			      /* C'est pas un bon */
			      madr = 0;
			      break;
			    }

			    madr--;
			  }
			}

			if (madr > 0) {
			  /* C'est un bon */
			  lhs_symb = XH_list_elem (rcg_parameters, bot4); (void) lhs_symb;

			  /* On genere */
			  len = XH_list_elem (rcg_predicates, bot2+2);

			  if (symb2madr [symb] == 0) {
			    symb2madr [symb] = ladr;

			    if (len == 0) {
			      fprintf (parser_file, "adr2bound_values [%ld] = adr2bound_values [%ld];\n", ladr+1, madr);
			    }
			    else {
			      fprintf (parser_file, "SS_open (SS_stack); // on prepare la sous-pile qui va recevoir l'ensemble des q qui se trouvent a la distance len de p\n\
SS_stack = idag_p_l2q_SS_stack (SS_stack, adr2bound_values [%ld], %ld); // rempli cette sous-pile\n\
while (!SS_is_empty (SS_stack)) { // tant que cette sous-pile est non vide ...\n\
adr2bound_values [%ld] = SS_pop (SS_stack); // on pop le sommet\n", madr, len, ladr+1);
			      if (level++ == 0)
				exit_stmt = "continue";

			      level_kind [level] = 1; /* C'est un strLen */
			    }			    

			    has_changed = true;
			  }
			  else {
			    /* il y a un autre StrLen avec le meme symb ? */
			    /* On fait la verif sur len */
			    if (len == 0) {
			      fprintf (parser_file, "if (adr2bound_values[%ld] != adr2bound_values[%ld]) %s;\n", madr, ladr+1, exit_stmt);
			    }
			    else {
			      fprintf (parser_file, "if (!_StrLen_check (%ld, adr2bound_values[%ld], adr2bound_values[%ld])) %s;\n", len, madr, ladr+1, exit_stmt);
			    }
			  }
			}
		      }
		    }
		  }
		}
		else {
		  fprintf (parser_file, "adr2bound_values[%ld] = adr2bound_values[%ld];\n", ladr+1, madr+1);
		  fprintf (parser_file, "if (!SXBA_bit_is_set (idag.path[adr2bound_values[%ld]], adr2bound_values[%ld])) %s;\n", ladr+1, radr, exit_stmt);
		}
	      }
	      else {
		if ((madr = symb2madr [symb]) == 0) {
		  /* 1ere fois qu'on tombe sur symb */
		  symb2madr [symb] = ladr;
		  fprintf (parser_file, "path_%ld = idag.path[adr2bound_values[%ld]];\n", ladr, ladr);
		  /* Il faut que le 1er decoupage puisse etre vide */
		  fprintf (parser_file, "for (adr2bound_values[%ld] = adr2bound_values[%ld]; adr2bound_values[%ld] >= 0 && adr2bound_values[%ld] <= adr2bound_values[%ld]; adr2bound_values[%ld] = sxba_scan (path_%ld, adr2bound_values[%ld])) {\n",
			   ladr+1, ladr, ladr+1, ladr+1, radr, ladr+1, ladr, ladr+1);
#if 0
		  fprintf (parser_file, "adr2bound_values[%ld] = adr2bound_values[%ld];\n", ladr+1, ladr);
		  fprintf (parser_file, "path_%ld = idag.path[adr2bound_values[%ld]];\n", ladr, ladr);
		  fprintf (parser_file, "while ((adr2bound_values[%ld] = sxba_scan (path_%ld, adr2bound_values[%ld])) >= 0 && adr2bound_values[%ld] <= adr2bound_values[%ld]) {\n", ladr+1, ladr, ladr+1, ladr+1, radr);
#endif /* 0 */
		  if (level++ == 0)
		    exit_stmt = "continue";

		  has_changed = true;
		}
		else {
		  fprintf (parser_file, "adr2bound_values[%ld] = adr2bound_values[%ld];\n", ladr+1, madr+1);
		  fprintf (parser_file, "if (!SXBA_bit_is_set (idag.path[adr2bound_values[%ld]], adr2bound_values[%ld])) %s;\n", ladr+1, radr, exit_stmt);
		}
	      }
	    }
	  }
	  else {
	    symb = -symb;

	    if (symb > max_ste) {
	      /* counter */
	      fprintf (parser_file, "adr2bound_values[%ld] = adr2bound_values[%ld]+%ld;\n", ladr+1, ladr, counter);
	    }
	    else {
	      /* Vrai terminal, on le scanne */
#if 0
	      /* t_nb est non utilisé!! */
	      /* On compte le nb de terminaux entre cur3 et top3 */
	      t_nb = 1; /* celui de cur3 */

	      for (x = cur3+1; x < top3; x++) {
		if (XH_list_elem (rcg_parameters, x) < 0)
		  t_nb++;
	      }
#endif
	      
	      is_GET_QSTACK = true;

	      if (terminal_file)
		fprintf (parser_file, "\
if ((qstack_%ld = GET_QSTACK (adr2bound_values[%ld], SXT_%s)) == NULL) %s;\n\
for (top_qstack_%ld = qstack_%ld + *qstack_%ld++; qstack_%ld <= top_qstack_%ld; qstack_%ld++) {\n\
  if ((adr2bound_values[%ld] = *qstack_%ld) > adr2bound_values[%ld]) break;\n",
			 (SXINT) ladr, (SXINT) ladr, sxstrget (t2Cid_ste [symb]), exit_stmt,
			 (SXINT) ladr, (SXINT) ladr, (SXINT) ladr, (SXINT) ladr, (SXINT) ladr, (SXINT) ladr,
			 (SXINT) ladr+1, (SXINT) ladr, (SXINT) radr);
	      else
		fprintf (parser_file, "\
if ((qstack_%ld = GET_QSTACK (adr2bound_values[%ld], %ld)) == NULL) %s;\n\
for (top_qstack_%ld = qstack_%ld + *qstack_%ld++; qstack_%ld <= top_qstack_%ld; qstack_%ld++) {\n\
  if ((adr2bound_values[%ld] = *qstack_%ld) > adr2bound_values[%ld]) break;\n",
			 (SXINT) ladr, (SXINT) ladr, (SXINT) symb, exit_stmt,
			 (SXINT) ladr, (SXINT) ladr, (SXINT) ladr, (SXINT) ladr, (SXINT) ladr, (SXINT) ladr,
			 (SXINT) ladr+1, (SXINT) ladr, (SXINT) radr);

	      if (level++ == 0)
		exit_stmt = "continue";
	      /* On ne touche pas a has_changed sur les terminaux */
	    }
	  }
		    
	  pos2ladr [pos] = ++ladr; /* calcule' */

	  if (ladr == radr-1) {
	    /* En fait, tout est calcule', on finalise */
	    SXBA_0_bit (dag_arg_tbp_set, pos);
	    nb_of_arg_tbp--;
	    symb = XH_list_elem (rcg_parameters, cur3+1);

	    if (symb > 0) {
	      if (SXBA_bit_is_set (counter_Ak_set, Ak)) {
		/* L'arg est un compteur */
		if ((madr = symb2counter_madr [symb]) == 0) {
		  /* 1ere fois qu'on tombe sur symb */
		  /* Pas de test ? */
		  symb2counter_madr [symb] = ladr;
		}
		else {
		  fprintf (parser_file, "if (adr2bound_values[%ld] != adr2bound_values[%ld] || adr2bound_values[%ld] != adr2bound_values[%ld]) %s;\n", ladr, madr, ladr+1, madr+1, exit_stmt);
		}
	      }
	      else {
		if ((madr = symb2madr [symb]) == 0) {
		  /* 1ere fois qu'on tombe sur symb */
		  /* Pas de test ? */
		  symb2madr [symb] = ladr;
		}
		else {
		  fprintf (parser_file, "if (adr2bound_values[%ld] != adr2bound_values[%ld] || adr2bound_values[%ld] != adr2bound_values[%ld]) %s;\n", ladr, madr, ladr+1, madr+1, exit_stmt);
		}
	      }
	    }
	    else {
	      if (symb >= -max_ste) {
		/* vrai terminal, on le scanne */
		Tpq_stack_pos = get_Tpq_stack_pos (pos, bot3, cur3+1);

		if (terminal_file)
		  fprintf (parser_file, "\
if ((rho [%ld] = p_T_q2Tpq (adr2bound_values[%ld], SXT_%s, adr2bound_values[%ld])) == 0) %s;\n",
			   (SXINT) (cur_rho_size+Tpq_stack_pos), ladr, sxstrget (t2Cid_ste [-symb]), ladr+1, exit_stmt);
		else
		  fprintf (parser_file, "\
if ((rho [%ld] = p_T_q2Tpq (adr2bound_values[%ld], %ld, adr2bound_values[%ld])) == 0) %s;\n",
			   (SXINT) (cur_rho_size+Tpq_stack_pos), ladr, -symb, ladr+1, exit_stmt);
	      }
	      else {
		counter = -symb - max_ste - 1;
		/* compteur */
		fprintf (parser_file, "if (adr2bound_values[%ld]-adr2bound_values[%ld]!=%ld) %s;\n", adr, adr+1, counter, exit_stmt);
	      }
	    }
	  }
	}
      }
    }
  }

#if 0
  /* Le 07/08/08 */
  /* Ici Tout est connu et genere' */
  if (is_a_non_linear_clause) {
    /* Si la grammaire est non-lineaire et si la clause courante necessite le calcul de sous-dags, on genere un appel */
    fprintf (parser_file, "if (bu_nl_rcg_processing (&G, rho, nl_stmt_stack+%ld, adr2bound_values) == -1) %s;\n", clause2nl_stmt_stack [clause], exit_stmt);
  }
#endif /* 0 */

  /* Copie e'dulcore'e de la fin de rhs_processing () */
  {
    /* Modif du Jeu 16 Dec 1999 10:48:28 : Les appels negatifs sont sautes par la semantique, il ne sert a rien
       de la prevenir! De plus les appels negatifs n'etaient pas comptes ds max_tooth! Utiliser rhs_prdct_pos2tooth */


#if 0
    /* Le 07/08/08 */
    /* Modif du 28/02/08 suppression de local_ret_val */
    fputs ("ret_val = 1;\n", parser_file);
#endif /* 0 */

#if 0
    fprintf (parser_file, "%sret_val = 1;\n",
	     A_clause_nb > 1 ? "local_" : "");
#endif /* 0 */

    if (is_cyclic_grammar) {
      fputs ("#if is_cyclic\n\
if (!is_glbl_cycle) {\n\
#endif /* is_cyclic */\n", parser_file);
    }

#if is_semantics==1
    fputs ("#if is_semantics\n", parser_file);
#endif /* is_semantics==1 */

#if 0
    /* A VOIR s'il faut faire c,a ds le cas is_dag ? */
    /* Modif du 28/05/2003 : l'affectation des indices des terminaux ds rho ne se fait que si on demande la semantique */
    if (lhs_t_nb) {
      fprintf (parser_file, "#if is_sdag && (is_parse_forest || is_2var_form || is_shallow)\n\
%s\n\
#endif\n", varstr_tostr (t_indexes_vstr));
    }
#endif /* 0 */
    
    fputs ("if (*rho0 == 0) rho[1] = *rho0 = local_Aij2global_Aij_pid [Aij] = MAKE_Aij_pid (Aij, G.pid);\n", parser_file); /* Le 07/08/08 */

    
#if 0
    /* Le 07/08/08 */
    /* Ici Tout est connu et genere' */
    if (is_a_non_linear_clause) {
      /* Si la grammaire est non-lineaire et si la clause courante necessite le calcul de sous-dags, on genere un appel */
      fprintf (parser_file, "if (bu_nl_rcg_processing (&G, rho, adr2bound_values) /* appelle G.semact.first_pass */ == -1) %s;\n", exit_stmt);
    }
#endif /* 0 */

#if 0
    /* Le 30/04/09, on revient a l'appel normal de la "first_pass", le traitement eventuel de grammaires non-lineaires sur des DAG se fera dans
       une 3eme passe */
    /* Le 10/09/08 */
    fprintf (parser_file, "if (rcg_first_bu_dag_pass_processing (&G, rho) /* appelle G.semact.first_pass */ == -1) %s;\n", exit_stmt);
#endif /* 0 */

  /* Modif du Mar  7 Nov 2000 14:07:00 : pour chaque clause le nombre de fils et chacun de leur pid sont
       stockes ds clause2son_nb et clause2son_pid */
    fprintf (parser_file, "if ((*G.semact.first_pass)(&G, rho) == -1) %s;\n", exit_stmt);

#if is_semantics==1
    fputs ("#endif /* is_semantics */\n", parser_file);
#endif /* is_semantics==1 */

    if (is_cyclic_grammar) {
      fputs ("#if is_cyclic\n\
}\n", parser_file);

      if (level > 1) {
	/* Si on n'est pas au niveau 1 (et si l'ip est vrai, ca ne sert a rien de continuer */
	fputs ("else break;\n", parser_file);
      }

      fputs ("#endif /* is_cyclic */\n", parser_file);
    }

    /* Le 07/08/08 */
    /* Modif du 28/02/08 suppression de local_ret_val */
    fputs ("ret_val = 1;\n", parser_file);

    while (level > 0) {
      fputs ("#if is_recognizer\n\
break;\n\
#endif /* is_recognizer */\n", parser_file);

      fputs ("}\n", parser_file);

      if (level_kind [level]) {
	fputs ("SS_close (SS_stack);\n", parser_file);
	level_kind [level] = 0;
      }

      level--;
    }
  }

  for (lhs_cur2 = lhs_bot2_gen_dag_parser+2; lhs_cur2 < lhs_top2_gen_dag_parser; lhs_cur2++) {
    param = XH_list_elem (rcg_predicates, lhs_cur2);
    param2ladr [param] = 0;
  }

  for (symb = 1; symb <= max_gvar; symb++) {
    symb2madr [symb] = symb2counter_madr [symb] = 0;
  }

  if (has_strlen)
    sxba_empty (symb_in_strlen_set);

  if (!sxba_is_empty (dag_arg_tbp_set))
    sxtrap (ME, "gen_dag_parser");
}

static bool
fill_param_pos (SXINT clause)
{
  /* Collecte les renseignements sur clause */
  SXINT		xeq, xeq2, prev_xeq, pos, pos2, lpos, rpos, mpos;
  SXINT		adr, adr1, adr2, ladr, radr, lmadr, rmadr, top_adr;
  SXINT		bot, cur, top, bot2, cur2, lcur2, rcur2, top2;
  SXINT		bot3, lbot3, rbot3, top3, ltop3, rtop3, cur3, lcur3, rcur3;
  SXINT		arg1, arg2;
  SXINT		symb, prev_symb, lsymb, rsymb, lsymb1, rsymb1, lsymb2, rsymb2;
  SXINT		atom, matom;
  SXINT		lhs_prdct, lhs_nt, prdct, rhs, nt, i, k, param, len, couple;
  SXINT		min, tnb, Xnb, xargs, rhs_prdct_pos, mprdct_pos;
  SXINT		madr, tpos;
  SXINT		lgth;
  SXINT		lA, uA, n, j, cur_wtop;
  SXINT		lparam, rparam; 
  SXINT		pos1, top_adr1, radr1, bot1, top1, top_adr2, radr2, cur1;
  SXINT		item1, param1, item2, param2, inside_etc_nb, Ak, which_arg, rhs_arity, rhs_pos;
  SXINT		xbot, xtop, IJ, I, J;
  SXINT		*eq_ptr, *eq_ptr2;
  bool	is_useful, not_found, has_local_etc, is_pos_call, arg_is_a_range, has_strlen, is_a_counter;
  SXINT         counter, counter_nb;
  char	kind;
  SXBA	wmadr_set, equiv_adr_set;
  char	lstring [25], ustring [25], string [32];
  VARSTR	vstr;
    
  sxinitialise(rcur3) /* Pour un compilateur croisé pour l'architecture itanium64 qui est gcc version 3.4.5
			    et qui dit "warning: 'rcur3' might be used uninitialized in this function" */;

  is_useful = true;
  is_static_phase = true;
  clause_has_cut = has_predef_prdct = has_predef_prdct_call = false;
  lhs_prdct = XxY_X (rcg_clauses, clause) & lhs_prdct_filter;
  lhs_bot2 = XH_X (rcg_predicates, lhs_prdct);
  lhs_top2 = XH_X (rcg_predicates, lhs_prdct+1);
  lhs_nt = XH_list_elem (rcg_predicates, lhs_bot2+1);
  lhs_arg_tbp = lhs_arity = nt2arity [lhs_nt];
  max_atom = max_var = clause2max_var [clause];
  lhs_t_nb = 0;
  varstr_raz (t_indexes_vstr);

  rhs = XxY_Y (rcg_clauses, clause);

  if (rhs) {
    top = XH_X (rcg_rhs, rhs+1);
    bot = XH_X (rcg_rhs, rhs);
  }
  else
    bot = top = 0;

  cur_rho_size = 1 /* clause */ + 1 /* lhs */ + top-bot /* rhs */;

  /* On range les args de la LHS ... */
  adr = 1; /* Si adr2madr [adr] == 0 => point non sensible */
  tpos = pos = xeq = 0;
 
  if (lhs_arity) {
    /* En lhs si un predicat est d'arite 0, il a un param qui vaut 0 */
    Ak = A_k2Ak (lhs_nt, 0);

    for (cur2 = lhs_bot2+2; cur2 < lhs_top2; pos++, xeq++, cur2++, Ak++) {
      lhs_arg_pos2prev_args_t_nb [pos] = lhs_t_nb;
      param = XH_list_elem (rcg_predicates, cur2);
      XH_push (XH_prdct_args, param);
      lhs_pos2ladr [pos] = lhs_pos2adr [pos] = adr;
      rhs_pos2Ak [pos] = 0; /* unused */
      rhs_pos2nt [pos] = lhs_nt; /* Pourquoi pas */
      rhs_pos2param [pos] = param;

      SXBA_1_bit (lhs_arg_tbp_set, pos);

      if (param > 0) {
	bot3 = XH_X (rcg_parameters, param);
	top3 = XH_X (rcg_parameters, param+1);
	prev_symb = 0;
	counter = -1;

	for (cur3 = bot3; cur3 < top3; adr++, cur3++) {
	  adr2cur [adr] = cur3;
	  adr2pos [adr] = pos;

	  symb = XH_list_elem (rcg_parameters, cur3);

	  if (symb > 0) {
	    XxY_set (&VxV, prev_symb, symb, &couple);

	    /* Le Lun  4 Dec 2000 14:59:02 pour le calcul du "degree" */
	    /* On note pour chaque variable l'arg le + a gauche ou elle occurre */
	    /* La boucle "for" engendree par son calcul sera comptabilisee ds lhs_pos */
	    if (var2lhs_pos [symb] == 0)
	      var2lhs_pos [symb] = pos;
	  }
	  else {
	    adr2tpos [adr] = tpos++;

	    if (symb >= -max_ste)
	      /* Vrai t (pas constante) */
	      lhs_t_nb++;
	    else {
	      if (counter == -1)
		counter = -symb - max_ste - 1;
	      else
		counter += -symb - max_ste - 1;
	    }

	    symb = 0;

	    if (prev_symb != 0)
	      XxY_set (&VxV, prev_symb, symb, &couple);
	  }

	  prev_symb = symb;
	}

	pos2counter_val [pos] = counter; /* -1 => pas de compteurs */
	    
	if (prev_symb != 0)
	  XxY_set (&VxV, prev_symb, 0, &couple);

	lhs_pos2radr [pos] = adr;
	adr2cur [adr] = cur3;
	adr2pos [adr] = pos;

	adr++;
      }
      else
	lhs_pos2radr [pos] = adr;

      sprintf (lstring, "ilb[%ld]", (SXINT) pos);
      lhs_pos2lvstr [pos] = varstr_catenate (varstr_raz (lhs_pos2lvstr [pos]), lstring);
      sprintf (ustring, "iub[%ld]", (SXINT) pos);
      lhs_pos2rvstr [pos] = varstr_catenate (varstr_raz (lhs_pos2rvstr [pos]), ustring);

      vstr = varstr_raz (equation2vstr [xeq]);
      vstr = varstr_catenate (vstr, ustring);
      vstr = varstr_catenate (vstr, "-");
      equation2vstr [xeq] = varstr_catenate (vstr, lstring); /* iub [pos] - ilb [pos] */
      equation2vstr_kind [xeq] = GEQ0_k;
    }

    XH_set (&XH_prdct_args, &xargs);
    XH_prdct_args2mprdct_pos [xargs] = 0; /* LHS */

    lhs_pos2adr [pos] = adr;
  }

  max_adr = adr;

  /* ... et ceux de la RHS. */
  rhs = XxY_Y (rcg_clauses, clause);
  top = XH_X (rcg_rhs, rhs+1);
  bot = XH_X (rcg_rhs, rhs);
  max_son_nb = max_tooth = component_nb = counter_nb = max_generator_pos = 0;
  rhs_arg_nb = 0;

  for (rhs_prdct_pos = 1, cur = bot; cur < top; rhs_prdct_pos++, cur++) {
    prdct = XH_list_elem (rcg_rhs, cur);
    bot2 = XH_X (rcg_predicates, prdct);
    top2 = XH_X (rcg_predicates, prdct+1);
    is_pos_call = XH_list_elem (rcg_predicates, bot2) == 0;
    nt = XH_list_elem (rcg_predicates, bot2+1);


    if (nt < 0) {
      if (nt == FALSE_ic) {
	if (is_pos_call)
	  /* Occurrence de &False() en RHS, la clause est abandonnee */
	  return false;

	/* Ici on a une occur de !&False() => tj vrai, on saute */
      }
      else {
	if (nt != FIRST_ic && nt != LAST_ic && nt != LEX_ic) {
	  has_predef_prdct = true;

	  if ((is_pos_call && (nt == CUT_ic || nt == STREQ_ic || nt == TRUE_ic)) ||
	      (!is_pos_call && nt != TRUE_ic)) {
	    /* Il faut faire qqchose sur les appels negatifs des predicats predefinis */
	    if (nt == CUT_ic)
	      clause_has_cut = true;

	    has_predef_prdct_call = true;
	    rhs_prdct_nyp_nb++;
	    SXBA_1_bit (rhs_prdct_nyp, rhs_prdct_pos);
	  }
	}
      }
    }
    else {
      if (!has_predef_prdct) {
	if (!(has_predef_prdct = ((nt2IeqJ_disp [nt+1]-nt2IeqJ_disp [nt]) > 0))) {
	  Ak = A_k2Ak (nt, 0);
	  rhs_arity = nt2arity [nt];

	  for (k = 0; k < rhs_arity; k++) {
	    if (!SXBA_bit_is_set (counter_Ak_set, Ak) && Ak2len [Ak] >= 0) {
	      has_predef_prdct = true;
	      break;
	    }

	    Ak++;
	  }
	}
      }

      rhs_prdct_nyp_nb++;
      SXBA_1_bit (rhs_prdct_nyp, rhs_prdct_pos);
    }

    /* if (is_pos_call && (nt > 0 || is_drcg && nt == STREQ_ic && is_streq_spcl (bot2+2))) { */
    /* Modif du Jeu 16 Dec 1999 10:31:06 on reserve quand meme la place des rho des appels negatifs */
    /* Modif du Mar  7 Nov 2000 15:49:09 on utilise le rho des &StrEq et des &True */
    /* if (nt > 0 || is_drcg && nt == STREQ_ic && is_streq_spcl (bot2+2) || nt == STREQ_ic || nt == TRUE_ic) { */
    /* Rectification du Ven 22 Dec 2000 11:27 */
    if (nt > 0 || (is_drcg && nt == STREQ_ic && is_streq_spcl (bot2+2)) || nt == TRUE_ic) {
      /* index ds rho */
      rhs_prdct_pos2tooth [rhs_prdct_pos] = ++max_tooth;

#if 0
      /* Le 11/02/04 essai, on compte les appels negatif !! */
      if (is_pos_call)
	/* Utilisee ds la parse-forest */
#endif /* 0 */
	max_son_nb++;
    }
    else
      /* Les "places" des prdct predef sont sautees ds la parse forest */
      rhs_prdct_pos2tooth [rhs_prdct_pos] = 0;

    /* Permet de reperer les indices de ret_vals */
    if (nt > 0 || nt == STREQ_ic || nt == STREQLEN_ic || nt == STRLEN_ic || nt == TRUE_ic)
      rhs_prdct_pos2generator_pos [rhs_prdct_pos] = ++max_generator_pos;

    /* En rhs si un predicat est sans argument A(), on a bot2+2==top2 */
    for (k = 0, cur2 = bot2+2; cur2 < top2; pos++, k++, cur2++) {
      param = XH_list_elem (rcg_predicates, cur2);

      if (nt > 0 && !SXBA_bit_is_set (is_nt_external, nt))
	/* On ne cherche pas d'equiv sur les predef ou les externes */
	XH_push (XH_prdct_args, param);

      if (!is_pos_call)
	SXBA_1_bit (rhs_pos2neg_call_set, pos);

      rhs_pos2param [pos] = param;
      rhs_pos2nt [pos] = nt;

      if (nt > 0)
	Ak = A_k2Ak (nt, k);
      else {
	if (nt == FIRST_ic || nt == LAST_ic || nt == LEX_ic) {
	  if (k == 0)
	    /* 1er param */
	    Ak = nt;
	  else
	    Ak = -rhs_prdct_pos;
	}
	else
	  Ak = 0;
      }

      rhs_pos2Ak [pos] = Ak;

      if (param > 0) {
	bot3 = XH_X (rcg_parameters, param);
	top3 = XH_X (rcg_parameters, param+1);

	is_a_counter = (nt > 0 && SXBA_bit_is_set (counter_Ak_set, Ak) /* Contient le cas ou l'arg contient des constantes */)
	  || !rhs_arg2sub_arg (lhs_prdct, param);

	if ((nt == STRLEN_ic && cur2 == bot2+2) /* Le 1er arg de &StrLen est un entier */ ||
	     (nt == FIRST_ic && cur2 == bot2+2) /* Le 1er arg de &First est ds T* */ ||
	     (nt == LAST_ic && cur2 == bot2+2) /* Le 1er arg de &Last est ds T* */ ||
	     nt == LEX_ic /* un seul arg */ ||
	     (nt < 0 && is_in_Tstar (bot3, top3) == 1 /* que des terminaux */)||
	     (nt < 0 && !rhs_arg2sub_arg (lhs_prdct, param)) ||
	    is_a_counter
	    ) {
	  arg_is_a_range = false;
	}
	else {
	  arg_is_a_range = true;
	  SXBA_1_bit (pos_is_a_range, pos);
	}

#if 0
	if (nt < 0 && ((nt == STRLEN_ic && cur2 == bot2+2) /* Le 1er arg de &StrLen est un entier */ ||
		       (nt == FIRST_ic && cur2 == bot2+2) /* Le 1er arg de &First est ds T* */ ||
		       (nt == LAST_ic && cur2 == bot2+2) /* Le 1er arg de &Last est ds T* */ ||
		       nt == LEX_ic /* un seul arg */ ||
		       is_in_Tstar (bot3, top3) == 1 /* que des terminaux */||
		       !rhs_arg2sub_arg (lhs_prdct, param)
		       )
	    ) {
	  arg_is_a_range = false;
	}
	else {
	  arg_is_a_range = true;
	  SXBA_1_bit (pos_is_a_range, pos);
	}
#endif /* 0 */

	if (arg_is_a_range) {
	  has_local_etc = false;
	  sxinitialise (symb); /* pour faire taire gcc -Wuninitialized */
	  prev_symb = 0;

	  for (cur3 = lbot3 = bot3; cur3 < top3; cur3++) {
	    symb = XH_list_elem (rcg_parameters, cur3);

	    if (symb >= 0) {
	      if (symb > 0)
		XxY_set (&VxV, prev_symb, symb, &couple);
	      else {
		/* Detection d'un composant */
		has_local_etc = true;

		if (bot3 < cur3) {
		  /* Ce n'est pas le "..." qui commence l'arg */
		  component2pos [component_nb] = pos;
		  component2bot [component_nb] = lbot3;
		  component2top [component_nb] = cur3+1; /* On repere derriere le "..." (top) */
		  component_nb++;
		  lbot3 = cur3; /* On repere le "..." */
		}
	      }
	    }
	    else {
	      symb = 0;

	      if (prev_symb != 0)
		XxY_set (&VxV, prev_symb, symb, &couple);
	    }

	    prev_symb = symb;
	  }

	  pos2counter_val [pos] = -1; /* -1 => pas de compteurs */

	  if (prev_symb != 0)
	    XxY_set (&VxV, prev_symb, 0, &couple);

	  if (has_local_etc && (symb != 0 || top3 == bot3+1)) {
	    /* Il y a un composant final qui ne se termine pas par "..." */
	    /* Ou l'arg est uniquement forme de "..." */
	    component2pos [component_nb] = pos;
	    component2bot [component_nb] = lbot3;
	    component2top [component_nb] = top3; /* On repere derriere le "..." (top) */
	    component_nb++;
	  }
	}
	else {
	  if (is_a_counter) {
	    counter = 0;

	    for (cur3 = lbot3 = bot3; cur3 < top3; cur3++) {
	      symb = XH_list_elem (rcg_parameters, cur3);

	      if (symb < -max_ste) {
		/* constante d'un compteur */
		/* On cumule */
		  counter += -symb - max_ste - 1;
	      }
	    }

	    pos2counter_val [pos] = counter;

	    if (counter >= 0) {
	      counter2pos [counter_nb] = pos;
	      counter_nb++;
	    }
	  }
	}
      }
    }

    rhs_arg_nb += k;

    if (nt > 0 && !SXBA_bit_is_set (is_nt_external, nt)) {
      if (XH_set (&XH_prdct_args, &xargs)) {
				/* deja trouve */
	mprdct_pos = XH_prdct_args2mprdct_pos [xargs];
      }
      else {
	XH_prdct_args2mprdct_pos [xargs] = mprdct_pos = rhs_prdct_pos; /* classe d'equiv */
      }
    }
    else
      mprdct_pos = rhs_prdct_pos;

    prdct_pos2mprdct_pos [rhs_prdct_pos] = mprdct_pos;
  }


  max_component_nb = component_nb;
  max_counter_nb = counter_nb;

  /* On decoupe les args en portions atomiques */
  /* Un atome est une sous-chaine maximale (des args) de la forme V+ telle qu'il n'existe pas de variable
     commune entre 2 atomes. */
  make_atom ();

  /* On genere les equations verifiees par les tailles des args.
     Les atomes sont les variables */
    
  /* On fabrique le (squelette du) systeme d'equations lineaires correspondant */
  Ak = A_k2Ak (lhs_nt, 0);

  for (pos = 0, xeq = 0, cur2 = lhs_bot2+2; cur2 < lhs_top2; pos++, xeq++, cur2++, Ak++) {
    param = XH_list_elem (rcg_predicates, cur2);
    tnb = 0;
    Xnb = 0;
    eq_ptr = equation [xeq];
	
    if (param > 0) {
      cur3 = bot3 = XH_X (rcg_parameters, param);
      top3 = XH_X (rcg_parameters, param+1);

      while (cur3 < top3) {
	symb = XH_list_elem (rcg_parameters, cur3);

	if (symb > 0) {
	  /* On cherche a quel atome appartient symb */
	  atom = var2atom [symb];
	  gatom2nb [atom]++;
	  eq_ptr [atom] = 1;
	  Xnb++;
	  cur3 += atom2var_nb [atom];
	}
	else {
	  if (symb < -max_ste)
	    tnb += -symb - max_ste -1;
	  else
	    tnb++;

	  cur3++;
	}
      }
    }

    eq_ptr [0] = tnb;
    equation2Xnb [xeq] = Xnb;
  }


  /* 2 adresses de la lhs sont equivalentes si elle reperent toutes les 2 un source-index identique */
  /* Le representant de ces classes d'equivalence est la + petite adr (madr: min_adr ou main_adr) */
  for (pos = 0, cur2 = lhs_bot2+2; cur2 < lhs_top2; pos++, cur2++) {
    param = XH_list_elem (rcg_predicates, cur2);
	
    if (param > 0) {
      cur3 = bot3 = XH_X (rcg_parameters, param);
      top3 = XH_X (rcg_parameters, param+1);

      /* Parcourt de gauche a droite */
      cur3 = bot3;
      adr = lhs_pos2adr [pos];
      uA = 0;

      while (cur3 < top3) {
	adr2uA [adr] = uA;
	symb = XH_list_elem (rcg_parameters, cur3);

	if (symb > 0) {
	  atom = var2atom [symb];
	  /* D est le nb max de terminaux+1 consecutifs des lhs-args */
	  uA = atom*D;
	  lgth = atom2var_nb [atom];
	  cur3 += lgth;
	  adr += lgth;
	}
	else {
	  if (uA != 0) {
	    uA++;
	  }

	  cur3++;
	  adr++;
	}
      }

      if (uA != 0) {
				/* On finalise la passe gauche-droite */
	adr2uA [adr] = uA;
				/* C'est une V-string */
				/* Parcourt de droite a gauche */
				/* cur3 == top3 */
				/* adr repere top3 */
	adr2lA [adr] = lA = 0;

	while (bot3 < cur3) {
	  symb = XH_list_elem (rcg_parameters, cur3-1);

	  if (symb > 0) {
	    atom = var2atom [symb];
	    lA = atom*D;
	    lgth = atom2var_nb [atom];
	    cur3 -= lgth;
	    adr -= lgth;
	  }
	  else {
	    if (lA != 0) {
	      lA++;
	    }

	    cur3--;
	    adr--;
	  }

	  adr2lA [adr] = lA;
	}
      }
    }
  }

  /* 2 adr sont equivalentes ssi elles reperent le meme source-index */
  /* On fabrique adr2madr initialement vide*/

  is_some_equiv = false;

  for (adr1 = 1; adr1 < max_adr; adr1++) {
    uA = adr2uA [adr1];
    lA = adr2lA [adr1];
	    
    if (uA > 0 || lA > 0) {
      equiv_adr_set = adr2equiv_adr_set [adr1];

      for (adr2 = adr1+1; adr2 < max_adr; adr2++) {
	if ((uA > 0 && uA == adr2uA [adr2]) ||
	    (lA > 0 && lA == adr2lA [adr2])) {
	  /* Ici adr1 et adr2 sont equivalents */
	  is_some_equiv = true;
	  SXBA_1_bit (equiv_adr_set, adr2);
	  SXBA_1_bit (adr2equiv_adr_set [adr2], adr1);
	}
      }
    }
  }

  cur_wtop = 0;
  xeq = pos = lhs_arity;

  if (is_some_equiv) {
    for (madr = 1; madr < max_adr; madr++) {
      if (adr2madr [madr] == 0) {
	adr2madr [madr] = madr;
	PUSH (adr_stack, madr);

	while (!IS_EMPTY (adr_stack)) {
	  adr1 = POP (adr_stack);
	  equiv_adr_set = adr2equiv_adr_set [adr1];
	  adr2 = 0;

	  while ((adr2 = sxba_scan (equiv_adr_set, adr2)) > 0) {
	    if (adr2madr [adr2] == 0) {
	      /* madr_set contient les "vrais" madr */
	      SXBA_1_bit (madr_set, madr);
	      adr2madr [adr2] = madr;
	      PUSH (adr_stack, adr2);
	    }
#if EBUG
	    else {
	      if (adr2madr [adr2] != madr)
		sxtrap (ME, "fill_param_pos");
	    }
#endif
	  }
	}
      }
    }

    /* verification la coherence des symboles terminaux autour des points de controle */

    madr = 0;

    while ((madr = sxba_scan (madr_set, madr)) > 0) {
      for (adr1 = madr; adr1 < max_adr; adr1++) {
	if (adr2madr [adr1] == madr) {
	  cur3 = adr2cur [adr1];
	  param = rhs_pos2param [adr2pos [adr1]];

	  if (XH_X (rcg_parameters, param) < cur3)
	    lsymb1 = XH_list_elem (rcg_parameters, cur3-1);
	  else
	    lsymb1 = 0;

	  if (cur3 < XH_X (rcg_parameters, param+1))
	    rsymb1 = XH_list_elem (rcg_parameters, cur3);
	  else
	    rsymb1 = 0;

	  if (lsymb1 < 0 || rsymb1 < 0) {
	    for (adr2 = adr1+1; adr2 < max_adr; adr2++) {
	      if (adr2madr [adr2] == madr) {
		cur3 = adr2cur [adr2];
		param = rhs_pos2param [adr2pos [adr2]];

		if (lsymb1 < 0 && XH_X (rcg_parameters, param) < cur3)
		  lsymb2 = XH_list_elem (rcg_parameters, cur3-1);
		else
		  lsymb2 = 0;

		if (rsymb1 < 0 && cur3 < XH_X (rcg_parameters, param+1))
		  rsymb2 = XH_list_elem (rcg_parameters, cur3);
		else
		  rsymb2 = 0;

		if (lsymb1 < 0 && lsymb2 < 0 && lsymb1 != lsymb2)
		  is_useful = false;

		if (rsymb1 < 0 && rsymb2 < 0 && rsymb1 != rsymb2)
		  is_useful = false;
	      }
	    }
	  }
	}
      }
    }

	
    /* Si 2 lhs_arg ont en commun une V_string (chaine ayant au moins une variable), il faut et il suffit
       d'un point de synchronisation */
    /* Ce peut etre un madr qcq */
    /* On cherche le nombre minimal de points de synchronisation (synchro_set) */

    for (lpos = 0, lcur2 = lhs_bot2+2; lcur2 < lhs_top2; lpos++, lcur2++) {
      lparam = XH_list_elem (rcg_predicates, lcur2);
	
      if (lparam > 0) {
	lbot3 = XH_X (rcg_parameters, lparam);
	ltop3 = XH_X (rcg_parameters, lparam+1);
	top_adr = lhs_pos2adr [lpos+1];
	    
	for (ladr = lhs_pos2adr [lpos]; ladr < top_adr; ladr++) {
	  lmadr = adr2madr [ladr];
	  SXBA_1_bit (lmadr_set, lmadr);
	}

	for (rpos = lpos+1, rcur2 = lcur2+1; rcur2 < lhs_top2; rpos++, rcur2++) {
	  rparam = XH_list_elem (rcg_predicates, rcur2);
	
	  if (rparam > 0) {
	    if (lparam == rparam) {
	      if (++cur_wtop > wtop) {
		wmadr_sets = sxbm_resize (wmadr_sets, wtop+1, (wtop*2)+1,
					  max_lhs_adr+1);
		wtop *= 2;
	      }

	      sxba_copy (wmadr_sets [cur_wtop], lmadr_set);
	    }
	    else {
	      rbot3 = XH_X (rcg_parameters, rparam);
	      rtop3 = XH_X (rcg_parameters, rparam+1);

	      top_adr = lhs_pos2adr [rpos+1];
	    
	      for (radr = lhs_pos2adr [rpos]; radr < top_adr; radr++) {
		rmadr = adr2madr [radr];
		SXBA_1_bit (rmadr_set, rmadr);
	      }
		
	      if (IS_AND (rmadr_set, lmadr_set)) {
		sxba_and (rmadr_set, lmadr_set);
		ladr = lhs_pos2adr [lpos];
		lcur3 = lbot3;

		while (lcur3 < ltop3) {
		  not_found = true;
		  lmadr = adr2madr [ladr];

		  if (SXBA_bit_is_set_reset (rmadr_set, lmadr)) {
		    /* C'est l point de depart d'une V_string ds lpos */
		    for (radr = lhs_pos2adr [rpos], rcur3 = rbot3;
			 rcur3 < rtop3;
			 radr++, rcur3++) {
		      rmadr = adr2madr [radr];

		      if (rmadr == lmadr) {
			/* idem pour rpos */
			lsymb = XH_list_elem (rcg_parameters, lcur3);
			rsymb = XH_list_elem (rcg_parameters, rcur3);

			if (lsymb == rsymb) {
			  /* On range l'ensemble des madr */
			  if (++cur_wtop > wtop) {
			    wmadr_sets = sxbm_resize (wmadr_sets, wtop+1, (wtop*2)+1,
						      max_lhs_adr+1);
			    wtop *= 2;
			  }

			  sxba_empty (wmadr_set = wmadr_sets [cur_wtop]);
			  SXBA_1_bit (wmadr_set, lmadr);

			  while (lcur3 < ltop3 && rcur3 < rtop3) {
			    lsymb = XH_list_elem (rcg_parameters, lcur3);
			    rsymb = XH_list_elem (rcg_parameters, rcur3);

			    if (lsymb != rsymb) {
			      /* On sort de rpos et on continue ds lpos */
			      rcur3 = rtop3; 
			      not_found = false;
			      break;
			    }

			    if (lsymb < 0) {
			      lcur3++;
			      ladr++;
			      rcur3++;
			      radr++;
			    }
			    else {
			      atom = var2atom [lsymb];
			      lgth = atom2var_nb [atom];
			      lcur3 += lgth;
			      ladr += lgth;
			      rcur3 += lgth;
			      radr += lgth;
			    }

			    lmadr = adr2madr [ladr];
			    rmadr = adr2madr [radr];

#if EBUG
			    if (lmadr != rmadr)
			      sxtrap (ME, "fill_param_pos");
#endif

			    SXBA_0_bit (rmadr_set, lmadr);
			    SXBA_1_bit (wmadr_set, lmadr);
						
			  }
			}

		      }
		    }
		  }

		  if (not_found) {
		    ladr++, lcur3++;
		  }
		}
	      }
	      else
		sxba_empty (rmadr_set);
	    }
	  }
	}

	sxba_empty (lmadr_set);
      }
    }


    /* On cherche un synchro_set de cardinalite minimale qui "couvre" les wmadr_sets */
    /* On n'atteint pas forcement l'optimum par l'algo ci-dessous : A FAIRE */

    n = 0;

    for (i = 1; i <= cur_wtop; i++) {
      wmadr_set = wmadr_sets [i];

      for (j = 1; j <= n; j++) {
	if (IS_AND (wmadr_set,  wmadr_sets [j])) {
	  sxba_and (wmadr_sets [j], wmadr_set);
	  break;
	}
      }

      if (j > n) {
	n++;
	sxba_copy (wmadr_sets [n], wmadr_set);
      }
    }

    /* On prend 1 madr ds chaque ensemble */
    for (j = 1; j <= n; j++) {
      madr = sxba_scan (wmadr_sets [j], 0);
      SXBA_1_bit (synchro_set, madr);

      /* Si jamais ce madr correspond a une borne inf ou sup, on le note (pour la premiere
	 rencontree) en negatif si ilb en positif si iub ds synchro2adr */
      /* Ca va eviter d'utiliser une variable auxiliaire Sindex [] */

      for (adr = madr; adr < max_adr; adr++) {
	if (adr2madr [adr] == madr) {
	  cur3 = adr2cur [adr];
	  param = rhs_pos2param [adr2pos [adr]];

	  if (cur3 == XH_X (rcg_parameters, param)) {
	    /* C'est une ilb */
	    synchro2adr [madr] = -adr;
	    break;
	  }

	  if (cur3 == XH_X (rcg_parameters, param+1)) {
	    /* C'est une iub */
	    synchro2adr [madr] = adr;
	    break;
	  }
	}
      }
    }

    /* Si 2 args ont une variable commune, on les synchronise */
    /* si arg1 = \alpha1 X \alpha2 et arg2 = \beta1 X \beta 2 on genlere
       ilb [1] + |\alpha1| = ilb [2] + |\beta1| et
       iub [1] - |\alpha2| = iub [2] - |\beta2|
       Au plus, une seule synchronisation par couple d'arg */
    /* Le 30/03/2004 j'enleve la restriction ci-dessus car le cas
       A (U V W X Y, V, X, ...) --> ...
       n'est pas bien ge'ne're' */
    madr = 0;
    /* pos1 = -1; */

    while ((madr = sxba_scan (synchro_set, madr)) > 0) {
      adr1 = madr;

      /*
      if (adr2pos [adr1] <= pos1)
	continue;
      */

      pos1 = adr2pos [adr1];
      item1 = adr2cur [adr1];
      param1 = rhs_pos2param [pos1];

      bot1 = XH_X (rcg_parameters, param1);
      top1 = XH_X (rcg_parameters, param1+1);
			    
      for (pos2 = pos1+1; pos2 < lhs_arity; pos2++) {
				/* On n'est pas ds le meme arg */
	top_adr2 = lhs_pos2adr [pos2+1];

	for (adr2 = lhs_pos2adr [pos2]; adr2 < top_adr2; adr2++) {
	  if (adr2madr [adr2] == madr) {
	    item2 = adr2cur [adr2];
	    param2 = rhs_pos2param [pos2];

	    bot2 = XH_X (rcg_parameters, param2);
	    top2 = XH_X (rcg_parameters, param2+1);

	    if (put_new_equation ((SXINT)0, xeq, (SXINT)1, bot1, item1, (SXINT)-1, bot2, item2)) {
	      sprintf (string, "ilb[%ld]-ilb[%ld]", (SXINT) pos2, (SXINT) pos1);
	    }
	    else {
	      sprintf (string, "ilb[%ld]-ilb[%ld]", (SXINT) pos1, (SXINT) pos2);
	    }

	    equation2vstr [xeq] = varstr_catenate (varstr_raz (equation2vstr [xeq]), string);

	    if (++xeq > eq_top)
	      equation_oflw ();

	    if (put_new_equation ((SXINT)0, xeq, (SXINT)1, item1, top1, (SXINT)-1, item2, top2)) {
	      sprintf (string, "iub[%ld]-iub[%ld]", (SXINT) pos1, (SXINT) pos2);
	    }
	    else {
	      sprintf (string, "iub[%ld]-iub[%ld]", (SXINT) pos2, (SXINT) pos1);
	    }

	    equation2vstr [xeq] = varstr_catenate (varstr_raz (equation2vstr [xeq]), string);

	    if (++xeq > eq_top)
	      equation_oflw ();

	    break;
	  }
	}
      }
    }
  }
  else {
    for (adr = 1; adr < max_adr; adr++)
      adr2madr [adr] = adr;
  }

  lhs_equation = xeq;
  /* Chaque arg de la RHS a au moins une occur en LHS.  Les adresses de ces occur sont des points
     ou la valeur de la tete de lecture doit obligatoirement etre stockee/verifiee */

  /* On traite en premier les bornes inf et sup des args de la RHS qui ne sont pas des "..." */
  /* Ca permet aux output_inside_bounds qui sont aussi des output_[lower|upper]_bound d'avoir des noms
     en olb oub et non en odddb */

  for (cur = bot; cur < top; cur++) {
    prdct = XH_list_elem (rcg_rhs, cur);
    bot2 = XH_X (rcg_predicates, prdct);
    top2 = XH_X (rcg_predicates, prdct+1);
    nt = XH_list_elem (rcg_predicates, bot2+1);

    for (cur2 = bot2+2; cur2 < top2; pos++, cur2++) {
      param = XH_list_elem (rcg_predicates, cur2);

      if (param > 0) {
	bot3 = XH_X (rcg_parameters, param);
	top3 = XH_X (rcg_parameters, param+1);

	if (cur2 > bot2+2 || (nt != STRLEN_ic && nt != FIRST_ic && nt != LAST_ic && nt != LEX_ic)) {
	  cur3 = bot3;

	  while (cur3 < top3) {
	    symb = XH_list_elem (rcg_parameters, cur3);

	    if (symb > 0) {
	      /* On cherche a quel atome appartient symb */
	      atom = var2atom [symb];
	      gatom2nb [atom]++;
	      cur3 += atom2var_nb [atom];
	    }
	    else
	      cur3++;
	  }
	}

	if (SXBA_bit_is_set (pos_is_a_range, pos) &&
	    (nt > 0 ||
	     nt == STREQ_ic ||
	     ((nt == FIRST_ic || nt == LAST_ic) && cur2 > bot2+2) ||
	     (nt == STRLEN_ic && cur2 > bot2+2 && XH_list_elem (rcg_predicates, bot2) == 1) ||
	     (nt == STREQLEN_ic && XH_list_elem (rcg_predicates, bot2) == 1) ||
	     nt == CUT_ic ||
	     nt == TRUE_ic)
	    ) {
	  /* On a ajoute Cut car les bornes de ses parametres peuvent tres bien ne pas etre
	     utilisees ailleurs.  Traitement general.  A FAIRE affectation reelle inutile. */
	  /* On cherche toutes les occurrences des bornes inf ou sup des args en RHS qui doivent
	     avoir des valeurs egales */
	  /* On peut les reperer par un couple (p,X) ou p est le nb de terminaux qui precede
	     (ou suit) la variable X. La verif de l'egalite de ces chaines terminales de long
	     p est faite ailleurs. */
	  if (XH_list_elem (rcg_parameters, bot3) != 0) {
	    /* L'arg ne commence pas par "..." */
	    sxinitialise (symb);
	    lpos = pos;
	    tnb = 0;

	    for (cur3 = bot3; cur3 < top3; cur3++) {
	      symb = XH_list_elem (rcg_parameters, cur3);

	      if (symb > 0)
		break;

	      if (symb >= -max_ste)
		/* Vrai t (pas constante) */
		tnb++;
	    }

	    if (cur3 == top3 && tnb == 0) { /* Le 10/04/08 */
	      /* Constantes */
	      rhs_pos2lmadr [pos] = 0;
	    }
	    else {
	      /* "vrai" arg */
	      adr = get_lhs_ladr (tnb, symb);

#if EBUG
	      if (adr < 0)
		sxtrap (ME, "fill_param_pos");
#endif

	      sprintf (string, "olb[%ld]", (SXINT) pos-lhs_arity);

	      madr = adr2madr [adr];

	      if (SXBA_bit_is_reset_set (is_madr_an_ob, madr)) {
		/* Attention, une madr peut aussi etre l'adresse principale d'une oub */
		/* Utilisation en partie droite d'affectation */
		madr2rcode [madr] = varstr_catenate (varstr_raz (madr2rcode [madr]), string);
		/* Utilisation en partie gauche d'affectation */
		vstr = varstr_raz (madr2lcode [madr]);
	      }
	      else {
		/* Utilisation en partie gauche d'affectation */
		/* madr2lcode [madr] a deja une valeur affectee */
		vstr = varstr_catenate (madr2lcode [madr], "=");
	      }

	      madr2lcode [madr] = varstr_catenate (vstr, string);
	      rhs_pos2lmadr [pos] = madr;

	      /* On associe a chaque madr la liste des arg (pos) de la rhs qui commencent au meme
		 endroit */
	      /* Cette liste sert pour generer les tests de last, du guide ou de Ak2max */
	      if (nt > 0 || nt == FIRST_ic || nt == LAST_ic) {
		/* Pas de test de first sur les predicats externes (predefinis ou non) */
		olbs [pos] = madr2olb [madr];
		madr2olb [madr] = pos;
	      }
	    }
	  }
	  else {
	    rhs_pos2lmadr [pos] = 0;
	    /* Fin du lien pour gen_first_test */
	    olbs [pos] = 0;
	  }

	  if (XH_list_elem (rcg_parameters, top3-1) != 0) {
	    /* L'arg ne finit pas par "..." */
	    sxinitialise (symb);
	    rpos = pos;
	    tnb = 0;

	    for (cur3 = top3-1; bot3 <= cur3; cur3--) {
	      symb = XH_list_elem (rcg_parameters, cur3);

	      if (symb > 0)
		break;

	      if (symb >= -max_ste)
		/* Vrai t (pas constante) */
		tnb++;
	    }


	    if (cur3 < bot3 && tnb == 0) { /* Le 10/04/08 */
	      /* Constantes */
	      rhs_pos2rmadr [pos] = 0;
	    }
	    else {
	      /* Pas que des cstes */
	      adr = get_lhs_uadr (tnb, symb);
#if EBUG
	      if (adr < 0)
		sxtrap (ME, "fill_param_pos");
#endif
	      sprintf (string, "oub[%ld]", (SXINT) pos-lhs_arity);

	      madr = adr2madr [adr];

	      if (SXBA_bit_is_reset_set (is_madr_an_ob, madr)) {
		/* Attention, une madr peut aussi etre l'adresse principale d'une olb */
		/* Utilisation en partie droite d'affectation */
		madr2rcode [madr] = varstr_catenate (varstr_raz (madr2rcode [madr]), string);
		/* Utilisation en partie gauche d'affectation */
		vstr = varstr_raz (madr2lcode [madr]);
	      }
	      else {
		/* Utilisation en partie gauche d'affectation */
		/* madr2lcode [madr] a deja une valeur affectee */
		vstr = varstr_catenate (madr2lcode [madr], "=");
	      }

	      madr2lcode [madr] = varstr_catenate (vstr, string);
	      rhs_pos2rmadr [pos] = madr;

	      /* On associe a chaque madr la liste des arg (pos) de la rhs qui se terminent au meme
		 endroit */
	      /* Cette liste sert pour generer les tests de last, du guide ou de Ak2max */
	      if (nt > 0 || nt == FIRST_ic || nt == LAST_ic) {
		/* Pas de test de last sur les predicats externes (predefinis ou non) */
		oubs [pos] = madr2oub [madr];
		madr2oub [madr] = pos;
	      }
	    }
	  }
	  else {
	    rhs_pos2rmadr [pos] = 0;
	    /* Fin du lien pour gen_last_test */
	    oubs [pos] = 0;
	  }
	}

	if (nt > 0) {
	  /* On regarde si cet arg a deja une occur "utile" en RHS */
	  /* meme borne inf et meme borne sup (pas necessaire d'avoir le meme param) */
	  /* Les valeurs de rhs_pos2rhs_mpos sont utilisees pour
	     - les tests de first et last (non, le comportement vis-a-vis de first/last depend
	     aussi du predicat)
	     - les MAKE_A_RANGE
	  */
	  lmadr = rhs_pos2lmadr [pos];
	  rmadr = rhs_pos2rmadr [pos];

	  for (mpos = lhs_arity; mpos < pos; mpos++) {
	    if (rhs_pos2lmadr [mpos] == lmadr && rhs_pos2rmadr [mpos] == rmadr)
	      break;
	  }
	}
	else
	  mpos = pos;

	rhs_pos2rhs_mpos [pos] = mpos;
      }
      else
	rhs_pos2rhs_mpos [pos] = pos;
    }
  }

  /* On prepare le traitement des arg de &StrEq qui ne sont pas des sous-arg de la LHS */
  /* ainsi que les appels negatifs a &StrLen et &StrEqLen */
  if (SXBA_bit_is_set (clause2non_range_arg, clause)) {
    /* Cette clause a un &StrEq ou des !&StrLen et !&StrEqLen dont un arg est denote
       par des variables non contigues */
    pos = lhs_arity;

    for (cur = bot; cur < top; cur++) {
      prdct = XH_list_elem (rcg_rhs, cur);
      bot2 = XH_X (rcg_predicates, prdct);
      nt = XH_list_elem (rcg_predicates, bot2+1);

      if (nt == STREQ_ic ||
	  (XH_list_elem (rcg_predicates, bot2) == 1 && (nt == STREQLEN_ic || nt == STRLEN_ic))) {
	for (which_arg = 0; which_arg <= 1; pos++, which_arg++) {
	  if (!SXBA_bit_is_set (pos_is_a_range, pos)) {
	    if (!(nt == STRLEN_ic && which_arg == 0)) {
	      param = XH_list_elem (rcg_predicates, bot2+2+which_arg);

	      if (param > 0) {
		bot3 = XH_X (rcg_parameters, param);
		top3 = XH_X (rcg_parameters, param+1);

		if (is_in_Tstar (bot3, top3) == 0 /* vrai param avec variable */) {
		  for (cur3 = bot3; cur3 < top3; cur3++) {
		    symb = XH_list_elem (rcg_parameters, cur3);

		    if (symb > 0) {
		      adr = get_lhs_ladr ((SXINT)0, symb);

		      for (i = 0; i <= 1; i++) {
			sprintf (string, "X%sb[%ld]", i == 0 ? "l" : "u", (SXINT) symb-1);
			madr = adr2madr [adr+i];

			if (SXBA_bit_is_reset_set (is_madr_an_ob, madr)) {
			  /* Attention, une madr peut aussi etre l'adresse principale d'une oub */
			  /* Utilisation en partie droite d'affectation */
			  madr2rcode [madr] = varstr_catenate (varstr_raz (madr2rcode [madr]), string);
			  /* Utilisation en partie gauche d'affectation */
			  vstr = varstr_raz (madr2lcode [madr]);
			}
			else {
			  /* Utilisation en partie gauche d'affectation */
			  /* madr2lcode [madr] a deja une valeur affectee */
			  vstr = varstr_catenate (madr2lcode [madr], "=");
			}

			madr2lcode [madr] = varstr_catenate (vstr, string);
		      }
		    }
		  }
		}
	      }
	    }
	  }
	}
      }
      else
	pos+= XH_X (rcg_predicates, prdct+1)-bot2;
    }

  }

  if (max_component_nb) {
    /* On s'occupe maintenant des "...", il y en a ds cette clause */
    /* D'abord les output inside lower bound */
    inside_etc_nb = 0;

    for (component_nb = 0; component_nb < max_component_nb; component_nb++) {
      cur3 = component2bot [component_nb];

      if (XH_list_elem (rcg_parameters, cur3) == 0) {
				/* Il commence par "...", ce n'est pas un olb */
	pos = component2pos [component_nb];
	param = rhs_pos2param [pos];

	if (cur3 == XH_X (rcg_parameters, param)) {
	  /* C'est le composant prefixe de l'arg pos = ...\alpha */
	  /* L'olb de \alpha est sans importance */
	  /* Output "olb[pos-lhs_arity]=0;" */
	  rhs_pos2lmadr [pos] = 0;
	}
	else {
	  /* C'est un composant interne ("...\beta") de l'arg pos = \alpha...\beta */
	  sxinitialise (symb);
	  tnb = 0;
			
	  for (;;) {
	    cur3++;
	    symb = XH_list_elem (rcg_parameters, cur3);

	    if (symb > 0)
	      break;

	    tnb++;
	  }

	  adr = get_lhs_ladr (tnb, symb);

#if EBUG
	  if (adr < 0)
	    sxtrap (ME, "fill_param_pos");
#endif

	  /* On n'ajoute aucun element ds la liste issue de madr2olb[madr] car il
	     n'y a pas de "first" */
	  madr = adr2madr [adr];

	  if (SXBA_bit_is_reset_set (is_madr_an_ob, madr)) {
	    /* C'est une inside lower bound */
#if EBUG
	    if (inside_etc_nb >= Aetc_nb)
	      sxtrap (ME, "fill_param_pos");
#endif
    
	    sprintf (string, "odddb[%ld]", (SXINT) inside_etc_nb);
	    inside_etc_nb++;
			

	    /* Attention, une madr peut aussi etre l'adresse principale d'une oub */
	    /* Utilisation en partie droite d'affectation */
	    madr2rcode [madr] = varstr_catenate (varstr_raz (madr2rcode [madr]), string);
	    /* Utilisation en partie gauche d'affectation */
	    madr2lcode [madr] = varstr_catenate (varstr_raz (madr2lcode [madr]), string);
	  }
	  else {
	    /* On utilisers les noms deja connus */
	  }

	  component2lmadr [component_nb] = madr;
	}
      }
    }

    /* Puis les output inside upper bound */
    for (component_nb = 0; component_nb < max_component_nb; component_nb++) {
      cur3 = component2top [component_nb];

      if (XH_list_elem (rcg_parameters, cur3-1) == 0) {
				/* Il finit par "...", ce n'est pas un oub */
	pos = component2pos [component_nb];
	param = rhs_pos2param [pos];

	if (cur3 == XH_X (rcg_parameters, param+1)) {
	  /* C'est le composant suffixe de l'arg pos = \alpha... */
	  /* L'oub de \alpha est sans importance */
	  /* Output "oub[pos-lhs_arity]=n;" */
	  rhs_pos2rmadr [pos] = 0;
	}
	else {
	  /* C'est un composant interne ("\alpha...") de l'arg pos = \alpha...\beta */
	  cur3--; /* Repere le "..." */
	  sxinitialise (symb);
	  tnb = 0;
			
	  for (;;) {
	    cur3--;
	    symb = XH_list_elem (rcg_parameters, cur3);

	    if (symb > 0)
	      break;

	    tnb++;
	  }

	  adr = get_lhs_uadr (tnb, symb);

#if EBUG
	  if (adr < 0)
	    sxtrap (ME, "fill_param_pos");
#endif

	  madr = adr2madr [adr];

	  if (SXBA_bit_is_reset_set (is_madr_an_ob, madr)) {
	    /* C'est une output inside upper bound */
#if EBUG
	    if (inside_etc_nb >= Aetc_nb)
	      sxtrap (ME, "fill_param_pos");
#endif
    
	    sprintf (string, "odddb[%ld]", (SXINT) inside_etc_nb);
	    inside_etc_nb++;

	    /* Attention, une madr peut aussi etre l'adresse principale d'une oub */
	    /* Utilisation en partie droite d'affectation */
	    madr2rcode [madr] = varstr_catenate (varstr_raz (madr2rcode [madr]), string);
	    /* Utilisation en partie gauche d'affectation */
	    madr2lcode [madr] = varstr_catenate (varstr_raz (madr2lcode [madr]), string);
	  }
	  else {
	    /* On utilisers les noms deja connus */
	  }

	  component2rmadr [component_nb] = madr;
	}
      }
    }
  }

  for (atom = 1; atom <= max_atom; atom++) {
    if (atom2var_nb [atom] != 0 && gatom2nb [atom] == 1) {
      for (xeq = 0; xeq < lhs_arity; xeq++) {
	if (equation [xeq] [0] == 0 && equation2Xnb [xeq] == 1 && equation [xeq] [atom] == 1) {
	  /* C'est la seule occurrence de atom ds la clause et c'est un lhs arg complet en xeq */
	  /* On ne generera RIEN */
	  SXBA_1_bit (useless_lhs_arg, xeq);
	}
      }
    }

    gatom2nb [atom] = 0; /* clear */
  }

  /* Initialisation de atom2min le Ven  3 Mar 2000 13:27:34 */
  /* Initialisation de atom2max le Ven 8 Fev 2002 */
  /* Si un rhs arg est forme d'un seul atom, on peut initialiser atom2min avec
     - i si l'atome u est le 2eme arg de &StrLen(i,u)
     - avec Ak2min [Ak] si l'atome est le kieme arg de A (Ak)*/

  has_strlen = false;
  rhs_pos = lhs_arity;

  for (cur = bot; cur < top; cur++) {
    prdct = XH_list_elem (rcg_rhs, cur);
    bot2 = XH_X (rcg_predicates, prdct);
    top2 = XH_X (rcg_predicates, prdct+1);

    if (XH_list_elem (rcg_predicates, bot2) == 0) {
      /* On ne considere que les appels positifs */
      nt = XH_list_elem (rcg_predicates, bot2+1);

      if ((nt > 0 && SXBA_bit_is_set (is_lhs_nt, nt)) || nt == STRLEN_ic) {
	for (k = 0, cur2 = bot2+2; cur2 < top2; k++, cur2++) {
	  param = XH_list_elem (rcg_predicates, cur2);

	  if (param > 0) {
	    bot3 = XH_X (rcg_parameters, param);
	    symb = XH_list_elem (rcg_parameters, bot3);

	    if (symb > 0) {
	      if (nt != STRLEN_ic || k != 0) {
		top3 = XH_X (rcg_parameters, param+1);
		/* On cherche a quel atome appartient symb */
		atom = var2atom [symb];

		if (bot3 + atom2var_nb [atom] == top3) {
		  /* Argument est non-combinatoire */
		  if (nt == STRLEN_ic) {
		    /* A FAIRE : ajouter ici les &StrEq[Len] (t1 ... tq, X) */
		    has_strlen = true;
		  }
		  else {
		    /* Argument est non-combinatoire */
		    SXBA_1_bit (rhs_pos2nc_max_set, rhs_pos+k);
		    Ak = A_k2Ak (nt, k);

		    if (!SXBA_bit_is_set (counter_Ak_set, Ak)) {
		      if (atom2min [atom] == -1)
			atom2min [atom] = Ak2min [Ak];
		      else {
			/* On prend le max des min */
			if (atom2min [atom] < Ak2min [Ak])
			  atom2min [atom] = Ak2min [Ak];
#if 0
			/* Le 12/02/04 */
			else
			  /* BUG : on ne doit pas toucher aux Ak qui sont valides sur toutes les A_clauses */
			  Ak2min [Ak] = atom2min [atom];
#endif /* 0 */
		      }

		      if (atom2max [atom] == -1)
			atom2max [atom] = Ak2max [Ak];
		      else {
			/* On prend le min des max */
			if (Ak2max [Ak] >= 0 && Ak2max [Ak] < atom2max [atom])
			  atom2max [atom] = Ak2max [Ak];
#if 0
			/* Le 12/02/04 */
			else
			  /* BUG : on ne doit pas toucher aux Ak qui sont valides sur toutes les A_clauses */
			  Ak2max [Ak] = atom2max [atom];
#endif /* 0 */
		      }

#if 0
		      /* Le 07/01/04 */
		      if (atom2min [atom] < Ak2min [Ak])
			/* On prend le max des min */
			atom2min [atom] = Ak2min [Ak];

		      if (Ak2max [Ak] >= 0
			  && (atom2max [atom] == -1 || Ak2max [Ak] < atom2max [atom])) {
			/* On prend le min des max */
			atom2max [atom] = Ak2max [Ak];
			/* Argument est non-combinatoire dont le max est connu */
			SXBA_1_bit (rhs_pos2nc_max_set, rhs_pos+k);
		      }
#endif /* 0 */
		    }
		  }
		}
	      }
	    }
	  }
	}
      }
    }

    rhs_pos += top2-bot2-2;
  }

  if (has_strlen) {
    for (cur = bot; cur < top; cur++) {
      prdct = XH_list_elem (rcg_rhs, cur);
      bot2 = XH_X (rcg_predicates, prdct);

      if (XH_list_elem (rcg_predicates, bot2) == 0) {
				/* On ne considere que les appels positifs */
	nt = XH_list_elem (rcg_predicates, bot2+1);

	if (nt == STRLEN_ic) {
	  top2 = XH_X (rcg_predicates, prdct+1);
	  len = 0; /* Cas &StrLen (0, ...) */

	  for (k = 0, cur2 = bot2+2; cur2 < top2; k++, cur2++) {
	    param = XH_list_elem (rcg_predicates, cur2);

	    if (param > 0) {
	      bot3 = XH_X (rcg_parameters, param);
	      symb = XH_list_elem (rcg_parameters, bot3);

	      if (symb > 0) {
		if (k == 0) {
		  len = symb;
		}
		else {
		  top3 = XH_X (rcg_parameters, param+1);
		  /* On cherche a quel atome appartient symb */
		  atom = var2atom [symb];

		  if (bot3 + atom2var_nb [atom] == top3) {
		    atom2max [atom] = atom2min [atom] = len;
		  }
		}
	      }
	    }
	  }
	}
      }
    }
  }

  xeq = lhs_equation;

  /* Le Mer 12 Avr 2000 14:19:23, ajout de l'utilisation de Ak2len */
  /* Le Jeu 15 Jun 2000 11:31:12, ajout de l'utilisation de nt2IeqJ (les args I et J de nt ont tj la meme longueur) */
  if (has_predef_prdct) {
    /* Les predicats predefinis positifs sont transformes en equation */
    /* Il y a des &StrEq ou &StrEqLen, => on transforme en equations */
    for (cur = bot; cur < top; cur++) {
      prdct = XH_list_elem (rcg_rhs, cur);
      bot2 = XH_X (rcg_predicates, prdct);

      if (XH_list_elem (rcg_predicates, bot2) == 0) {
				/* Appels positifs */
	nt = XH_list_elem (rcg_predicates, bot2+1);

	if (nt > 0) {
	  if ((xbot = nt2IeqJ_disp [nt]) < (xtop = nt2IeqJ_disp [nt+1])) {
	    /* On s'occupe des egalites sur les arguments */
	    do {
	      IJ = nt2IeqJ [xbot];
	      J = IJ % max_garity;
	      I = IJ / max_garity;
	      arg2 = XH_list_elem (rcg_predicates, bot2+2+J);

	      /* On stocke arg1 */
	      cur3 = bot3 = XH_X (rcg_parameters, arg2);
	      top3 = XH_X (rcg_parameters, arg2+1);
	      tnb = 0;
	      Xnb = 0;
	      min = 0;

	      while (cur3 < top3) {
		symb = XH_list_elem (rcg_parameters, cur3);

		if (symb > 0) {
		  /* On cherche a quel atome appartient symb */
		  atom = var2atom [symb];
		  equation [xeq] [atom] = 1;
		  cur3 += atom2var_nb [atom];
		  Xnb++;
		  min += atom2min [atom];
		}
		else {
		  tnb++;
		  cur3++;
		}
	      }
		    
	      equation [xeq] [0] = tnb;
	      equation2Xnb [xeq] = Xnb;

	      arg1 = XH_list_elem (rcg_predicates, bot2+2+I);
	      /* On s'occupe du 1er arg */
	      bot3 = XH_X (rcg_parameters, arg1);
	      top3 = XH_X (rcg_parameters, arg1+1);

	      tnb = 0;
	      Xnb = 0;
	      cur3 = bot3;

	      while (cur3 < top3) {
		symb = XH_list_elem (rcg_parameters, cur3);

		if (symb > 0) {
		  /* On cherche a quel atome appartient symb */
		  atom = var2atom [symb];

		  if (equation [xeq] [atom] == 0) {
		    equation [xeq] [atom] = -1;
		    Xnb++;
		  }
		  else {
		    /* equation [xeq] [atom] == 1 */
		    equation [xeq] [atom] = 0;
		    Xnb--;
		  }

		  cur3 += atom2var_nb [atom];
		}
		else {
		  tnb++;
		  cur3++;
		}
	      }

	      equation [xeq] [0] -= tnb;
	      equation2Xnb [xeq] += Xnb;

	      equation2vstr [xeq] = varstr_catenate (varstr_raz (equation2vstr [xeq]), "0");
	      equation2vstr_kind [xeq] = NULL_k;
							
	      if ((Xnb = equation2Xnb [xeq]) == 0 && equation [xeq] [0] != 0) {
		/* La clause courante va toujours rendre faux */
		is_useful = false;
		equation [xeq] [0] = 0; /* clear */
	      }
	      else {
		if (Xnb == 1)
		  PUSH (equation_stack, xeq);

		if (++xeq > eq_top)
		  equation_oflw ();
	      }
	    } while (++xbot < xtop);
	  }

	  top2 = XH_X (rcg_predicates, prdct+1);
	  Ak = A_k2Ak (nt, 0);

	  for (cur2 = bot2+2; cur2 < top2; cur2++) {
	    if ((len = Ak2len [Ak]) >= 0) {
	      param = XH_list_elem (rcg_predicates, cur2);

	      if (param > 0) {
		cur3 = bot3 = XH_X (rcg_parameters, param);
		top3 = XH_X (rcg_parameters, param+1);
		tnb = 0;
		Xnb = 0;
		min = 0;

		while (cur3 < top3) {
		  symb = XH_list_elem (rcg_parameters, cur3);

		  if (symb > 0) {
		    /* On cherche a quel atome appartient symb */
		    atom = var2atom [symb];
		    equation [xeq] [atom] = 1;
		    cur3 += atom2var_nb [atom];
		    Xnb++;
		    min += atom2min [atom];
		  }
		  else {
		    tnb++;
		    cur3++;
		  }
		}
		    
		equation [xeq] [0] = tnb;
		equation2Xnb [xeq] = Xnb;

		/* Le nombre de terminaux doit etre <= len */
		if (tnb+min > len)
		  is_useful = false;

		equation [xeq] [0] -= len;

		equation2vstr [xeq] = varstr_catenate (varstr_raz (equation2vstr [xeq]), "0");
		equation2vstr_kind [xeq] = NULL_k;
							
		if ((Xnb = equation2Xnb [xeq]) == 0 && equation [xeq] [0] != 0) {
		  /* La clause courante va toujours rendre faux */
		  is_useful = false;
		  equation [xeq] [0] = 0; /* clear */
		}
		else {
		  if (Xnb == 1)
		    PUSH (equation_stack, xeq);

		  if (++xeq > eq_top)
		    equation_oflw ();
		}
	      }
	    }

	    Ak++;
	  }
	}
	else {
	  if (nt != FIRST_ic && nt != LAST_ic && nt != CUT_ic && nt != TRUE_ic && nt != LEX_ic && nt != FALSE_ic) {
	    arg1 = XH_list_elem (rcg_predicates, bot2+2);

#if 0
	    /* Le Lun 31 Jul 2000 15:03:41.  Sinon, a quoi sert le "if (nt == STRLEN_ic) {" ci-dessous !! */
	    if (nt != STRLEN_ic) {
#endif
	      arg2 = XH_list_elem (rcg_predicates, bot2+3);

	      /* On stocke arg2 */
	      cur3 = bot3 = XH_X (rcg_parameters, arg2);
	      top3 = XH_X (rcg_parameters, arg2+1);
	      tnb = 0;
	      Xnb = 0;
	      min = 0;

	      while (cur3 < top3) {
		symb = XH_list_elem (rcg_parameters, cur3);

		if (symb > 0) {
		  /* On cherche a quel atome appartient symb */
		  atom = var2atom [symb];
		  equation [xeq] [atom] = 1;
		  cur3 += atom2var_nb [atom];
		  Xnb++;
		  min += atom2min [atom];
		}
		else {
		  tnb++;
		  cur3++;
		}
	      }
		    
	      equation [xeq] [0] = tnb;
	      equation2Xnb [xeq] = Xnb;

	      /* On s'occupe du 1er arg */
	      bot3 = XH_X (rcg_parameters, arg1);

	      if (nt == STRLEN_ic) {
		len = XH_list_elem (rcg_parameters, bot3);

		/* Le nombre de terminaux doit etre <= len */
		if (tnb+min > len)
		  is_useful = false;

		equation [xeq] [0] -= len;
	      }
	      else {
		top3 = XH_X (rcg_parameters, arg1+1);

		if (nt == STREQ_ic && (is_in_Tstar (bot3, top3) == 1)) {
		  len = top3-bot3;

		  /* Le nombre de terminaux doit etre <= len */
		  if (tnb+min > len)
		    is_useful = false;

		  /* A FAIRE : verifier la compatibilite des terminaux */
		  equation [xeq] [0] -= len;
		}
		else {
		  tnb = 0;
		  Xnb = 0;
		  cur3 = bot3;

		  while (cur3 < top3) {
		    symb = XH_list_elem (rcg_parameters, cur3);

		    if (symb > 0) {
		      /* On cherche a quel atome appartient symb */
		      atom = var2atom [symb];

		      if (equation [xeq] [atom] == 0) {
			equation [xeq] [atom] = -1;
			Xnb++;
		      }
		      else {
			/* equation [xeq] [atom] == 1 */
			equation [xeq] [atom] = 0;
			Xnb--;
		      }

		      cur3 += atom2var_nb [atom];
		    }
		    else {
		      tnb++;
		      cur3++;
		    }
		  }

		  equation [xeq] [0] -= tnb;
		  equation2Xnb [xeq] += Xnb;
		}
	      }

	      equation2vstr [xeq] = varstr_catenate (varstr_raz (equation2vstr [xeq]), "0");
	      equation2vstr_kind [xeq] = NULL_k;
							
	      if ((Xnb = equation2Xnb [xeq]) == 0 && equation [xeq] [0] != 0) {
		/* La clause courante va toujours rendre faux */
		is_useful = false;
		equation [xeq] [0] = 0; /* clear */
	      }
	      else {
		if (Xnb == 1)
		  PUSH (equation_stack, xeq);

		if (++xeq > eq_top)
		  equation_oflw ();
	      }
#if 0
	    }
#endif
	  }
	}
      }
    }
  }

  if (is_some_equiv) {
    /* Il y a des equivalences entre adr */
    /* Si 2 lhs-arg ont 2 sous chaines a1 et a2 maximales t.q. a1 != a2 et t.q. leur borne
       inf (resp. sup) sont equivalentes alors on doit avoir &StrEq (a1, a2) */
    prev_xeq = xeq;

    lmadr = 0;

    while ((lmadr = sxba_scan (madr_set, lmadr)) > 0) {
      for (adr1 = lmadr; adr1 < max_adr; adr1++) {
	if (adr2madr [adr1] == lmadr) {
	  pos1 = adr2pos [adr1];
	  top_adr1 = lhs_pos2adr [pos1+1];

	  for (radr1 = adr1+1; radr1 < top_adr1; radr1++) {
	    rmadr = adr2madr [radr1];

	    if (SXBA_bit_is_set (madr_set, rmadr)) {
	      /* pos1 est un candidat */
	      /* il a deux adresses principales */
	      for (pos2 = pos1+1; pos2 < lhs_arity; pos2++) {
		top_adr2 = lhs_pos2adr [pos2+1];

		for (adr2 = lhs_pos2adr [pos2]; adr2 < top_adr2; adr2++) {
		  if (adr2madr [adr2] == lmadr) {
		    for (radr2 = adr2+1; radr2 < top_adr2; radr2++) {
		      if (adr2madr [radr2] == rmadr) {
			/* pos1 entre adr1 et radr1 et
			   pos2 entre adr2 et radr2 sont candidats */
			/* On verifie que les chaines sont differentes */
			bot1 = adr2cur [adr1];
			top1 = adr2cur [radr1];

			bot2 = adr2cur [adr2];
			top2 = adr2cur [radr2];

			for (cur1 = bot1, cur2 = bot2;
			     cur1 < top1 && cur2 < top2;
			     cur1++, cur2++) {
			  if (XH_list_elem (rcg_parameters, cur1) !=
			      XH_list_elem (rcg_parameters, cur2))
			    break;
			}

			bot1 = cur1;
			bot2 = cur2;

			for (cur1 = top1, cur2 = top2;
			     bot1 < cur1 && bot2 < cur2;
			     cur1--, cur2--) {
			  if (XH_list_elem (rcg_parameters, cur1-1) !=
			      XH_list_elem (rcg_parameters, cur2-1))
			    break;
			}

			if (bot1 < cur1 || bot2 < cur2) {
			  if (put_new_equation (prev_xeq, xeq, (SXINT)1, bot1, cur1,
						(SXINT)-1, bot2, cur2)) {
			    /* Elle est reellement nouvelle */
			    equation2vstr [xeq] = varstr_raz (equation2vstr [xeq]);
			    /* On ecrase la valeur mise par put_new_equation */
			    equation2vstr_kind [xeq] = NULL_k;
							
			    Xnb = equation2Xnb [xeq];

			    if (Xnb == 0 && equation [xeq] [0] != 0) {
			      /* La clause courante va toujours rendre faux */
			      is_useful = false;
			      equation [xeq] [0] = 0; /* clear */
			    }
			    else {
			      if (Xnb == 1)
				PUSH (equation_stack, xeq);

			      if (++xeq > eq_top)
				equation_oflw ();
			    }
			  }
			}

			break; /* il n'y en a pas d'autre */
		      }
		    }

		    break; /* il n'y en a pas d'autre */
		  }
		}
	      }
	    }
	  }
	}
      }
    }

    /* S'il y a une equivalence entre 2 adr ds le meme LHS arg.  La sous chaine doit etre
       de taille nulle => pas de terminaux et les var ont toutes une taille nulle */
    /* A FAIRE */
    for (pos = 0, cur2 = lhs_bot2+2; cur2 < lhs_top2; pos++, cur2++) {
      param = XH_list_elem (rcg_predicates, cur2);

      if (param > 0) {
	bot3 = XH_X (rcg_parameters, param);
	top3 = XH_X (rcg_parameters, param+1);

	for (ladr = lhs_pos2ladr [pos], lcur3 = bot3; lcur3 < top3; ladr++, lcur3++) {
	  madr = adr2madr [ladr];

	  for (radr = lhs_pos2radr [pos], rcur3 = top3; lcur3 < rcur3; radr--, rcur3--) {
	    if (adr2madr [radr] == madr)
	      break;
	  }

	  if (lcur3 < rcur3)
	    break;
	}

	if (lcur3 < top3) {
	  /* On tient une chaine nulle entre lcur3 et rcur3 */

	  while (lcur3 < rcur3) {
	    symb = XH_list_elem (rcg_parameters, lcur3);

	    if (symb < 0) {
	      is_useful = false;
	      lcur3++;
	    }
	    else {
	      atom = var2atom [symb];
	      atom2rsize [atom] = varstr_catenate (varstr_raz (atom2rsize [atom]), "0");
	      /* La taille de atom est calculee */
	      SXBA_1_bit (Asize_set, atom);
	      atom2static_size [atom] = 0;

	      for (xeq2 = xeq; xeq2 >= 0; xeq2--) {
		eq_ptr2 = equation [xeq2];

		if (eq_ptr2 [atom] != 0) {
		  eq_ptr2 [atom] = 0;
		  Xnb = --equation2Xnb [xeq2];

		  if (xeq2 >= lhs_equation) {
		    /* equation "statique" */
		    if (Xnb == 0) {
		      if (eq_ptr2 [0] != 0) {
			/* Incompatibilite (dynamique!) des tailles */
			is_useful = false;
			eq_ptr2 [0] = 0;
		      }
		    }
		    else {
		      if (Xnb == 1)
			PUSH (equation_stack, xeq2);
		    }
		  }
		}
	      }

	      lcur3 += atom2var_nb [atom];
	    }
	  }
	}
      }
    }
  }

  max_equation = xeq;

  /* On applique recursivement sur toutes les equations la taille des atomes dont on connait
     la longueur (Xnb == 1) */
        
  if (!IS_EMPTY (equation_stack)) {
    if (!propagate_1var_size ())
      is_useful = false;
  }


  /* On exploite les equations produites par les &StrEq[Len] */
  if (is_useful && lhs_equation < max_equation) {
    for (xeq = max_equation-1; lhs_equation <= xeq; xeq--) {
      if (equation2Xnb [xeq] != 0) {
	eq_ptr = equation [xeq];

				/* On en profite pour regarder si on peut ameliorer "min" */
	if ((tnb = eq_ptr [0]) != 0) {
	  matom = 0;

	  if (tnb > 0) {
	    for (atom = max_atom; 0 < atom; atom--) {
	      if (atom2var_nb [atom] != 0 && (Xnb = eq_ptr [atom]) < 0) {
		if (matom == 0)
		  matom = atom;
		else
		  matom = -1;
	      }
	    }
	  }
	  else {
	    for (atom = max_atom; 0 < atom; atom--) {
	      if (atom2var_nb [atom] != 0 && (Xnb = eq_ptr [atom]) > 0) {
		if (matom == 0)
		  matom = atom;
		else
		  matom = -1;
	      }
	    }
	  }

	  if (matom > 0) {
	    if ((Xnb = eq_ptr [matom]) < 0)
	      Xnb = -Xnb;
	    else
	      tnb = -tnb;

	    min = (tnb+Xnb-1)/Xnb;

	    if (min > atom2min [matom])
	      atom2min [matom] = min;
	  }
	}

	for (atom = max_atom; 0 < atom; atom--) {
	  if (atom2var_nb [atom] != 0 && eq_ptr [atom] != 0)
	    break;
	}

	if (atom > 0) {
	  equation_stack2 [0] = 0;

	  /* On cherche un arg qui contient l'atome de code max */
	  for (xeq2 = 0; xeq2 < xeq; xeq2++) {
	    if (equation [xeq2] [atom] != 0)
	      PUSH (equation_stack2, xeq2);
	  }

	  if (equation_stack2 [0] >= 1) {
	    PUSH (equation_stack2, xeq);

	    if (substitute_atom_in_eq (equation_stack2, (char *)NULL)) {
	      if (!IS_EMPTY (equation_stack)) {
		/* Uniquement si les xeq2 sont >= lhs_equation */
		if (!propagate_1var_size ())
		  is_useful = false;
	      }
	    }
	    else
	      is_useful = false;
	  }
	}
      }
    }
  }

  if (is_useful) {
    /* Pour chaque arg de la RHS on lui affecte un type de taille connu statiquement parmi
       UnknownSize, NullSize, NonNullSize */
    pos = lhs_arity;

    for (cur = bot; cur < top; cur++) {
      prdct = XH_list_elem (rcg_rhs, cur);
      bot2 = XH_X (rcg_predicates, prdct);
      top2 = XH_X (rcg_predicates, prdct+1);
      nt = XH_list_elem (rcg_predicates, bot2+1);

      len = -1;

      for (cur2 = bot2+2; cur2 < top2; pos++, cur2++) {
	param = XH_list_elem (rcg_predicates, cur2);

	if (param > 0) {
	  bot3 = XH_X (rcg_parameters, param);
	  top3 = XH_X (rcg_parameters, param+1);

	  if (cur2 == bot2+2 && (nt == STRLEN_ic || (nt == STREQ_ic && (is_in_Tstar (bot3, top3) == 1)))) {
	    if (nt == STRLEN_ic) {
	      len = XH_list_elem (rcg_parameters, bot3);
	    }
	    else
	      len = top3-bot3;
	  }

	  if (cur2 > bot2+2 || (nt != STRLEN_ic && nt != FIRST_ic && nt != LAST_ic && nt != LEX_ic)) {
	    /* Le Lun 13 Mar 2000 10:48:38 */
	    if (XH_list_elem (rcg_predicates, bot2) == 1) {
	      kind = UnknownSize;
	    }
	    else {
	      if (len >= 0) {
		kind = len ? NonNullSize : NullSize;
	      }
	      else {
		/* Le Lun 13 Mar 2000 10:48:38 */
		if (nt > 0 && SXBA_bit_is_set (is_lhs_nt, nt)) {
		  Ak = A_k2Ak (nt, cur2-(bot2+2));
		  kind = (Ak2min [Ak] > 0) ? NonNullSize : NullSize;
		}
		else
		  kind = NullSize;

		if (kind == NullSize) {
		  cur3 = bot3;

		  while (cur3 < top3) {
		    symb = XH_list_elem (rcg_parameters, cur3);

		    if (symb > 0) {
		      /* On cherche a quel atome appartient symb */
		      atom = var2atom [symb];

		      if (SXBA_bit_is_set (Asize_set, atom)) {
			if (atom2static_size [atom]) {
			  kind = NonNullSize;
			  break;
			}
		      }
		      else
			kind = UnknownSize;

		      cur3 += atom2var_nb [atom];
		    }
		    else {
		      kind = NonNullSize;
		      break;
		    }
		  }
		}
	      }
	    }

	    rhs_pos2static_size_kind [pos] = kind;
	    len = -1; /* Mis le Ven  3 Mar 2000 14:47:46 ?? */
	  }
	}
	else {
	  if (cur2 == bot2+2 && nt == STREQ_ic)
	    len = 0;
	}
      }
    }
  }

  return is_useful;
}

static void
gen_StrEq_spcl (SXINT prdct_pos, SXINT arg_pos, SXINT bot3, SXINT top3, char *exit_string, bool is_pos_call)
{
    SXINT len, cur3, symb, tooth, i;

    len = top3-bot3;
    cur3 = bot3;
    is_source_checked = true;

    switch (len) {
    case 1:
        symb = XH_list_elem (rcg_parameters, cur3);

        if (is_pos_call) {
	  if (terminal_file)
            fprintf (parser_file, "if ((Tpq = p_T_q2Tpq (olb[%ld], SXT_%s, oub[%ld])) == 0) %s;\n",
                     (SXINT) arg_pos, sxstrget (t2Cid_ste [-symb]), (SXINT) arg_pos, exit_string);
	  else
            fprintf (parser_file, "if ((Tpq = p_T_q2Tpq (olb[%ld], %ld, oub[%ld])) == 0) %s;\n",
                     (SXINT) arg_pos, (SXINT) -symb, (SXINT) arg_pos, exit_string);

          if (is_drcg && (tooth = rhs_prdct_pos2tooth [prdct_pos]) > 0) {
                /* Pour definite part eventuel */
                fprintf (parser_file, "rho[%ld] = -oub[%ld];\n", (SXINT) tooth+1, (SXINT) arg_pos);
          }
        }
        else {
	  if (terminal_file)
            fprintf (parser_file, "if (p_T_q2Tpq (olb[%ld], SXT_%s, oub[%ld]) != 0) %s;\n",
                     (SXINT) arg_pos, sxstrget (t2Cid_ste [-symb]), (SXINT) arg_pos, exit_string);
	else
            fprintf (parser_file, "if (p_T_q2Tpq (olb[%ld], %ld, oub[%ld]) != 0) %s;\n",
                     (SXINT) arg_pos, (SXINT) -symb, (SXINT) arg_pos, exit_string);
	}
        break;

    default:
        fprintf (parser_file, "local_val = olb[%ld];\n", (SXINT) arg_pos);

        if (is_pos_call)
            fputs ("if (", parser_file);
        else
            fprintf (parser_file, "if (oub[%ld]-olb[%ld]==%ld &&\n", (SXINT) arg_pos, (SXINT) arg_pos, (SXINT) len);
        
        i = 0;

        do {
            symb = XH_list_elem (rcg_parameters, cur3++);
            len--;

	    if (terminal_file)
	      fprintf (parser_file, "if (p_T_q2Tpq (local_val+%ld, SXT_%s, local_val+%ld=1) %s= 0 %s\n",
		       (SXINT) i, sxstrget (t2Cid_ste [-symb]), (SXINT) i, is_pos_call ? "=" : "!", len ? (is_pos_call ? "||" : "&&") : "");
	    else
	      fprintf (parser_file, "if (p_T_q2Tpq (local_val+%ld, %ld, local_val+%ld=1) %s= 0 %s\n",
		       (SXINT) i, -symb, (SXINT) i, is_pos_call ? "=" : "!", len ? (is_pos_call ? "||" : "&&") : "");
	    /* Faut-il faire qqchose ds le cas is_generator ? */

	    i++;
        } while (len);

        fprintf (parser_file, ") %s;\n", exit_string);
    }
}



static bool
arg_evaluated (SXINT nt, bool is_first_arg, SXINT rhs_param_pos, SXINT cur2)
{
    SXINT param, bot3, cur3, top3, symb, adr, lmadr, rmadr;
                            
    if (nt == STRLEN_ic && is_first_arg) return true;
    
    if (SXBA_bit_is_set (pos_is_a_range, rhs_param_pos)) {
        /* C'est un range */
        lmadr = rhs_pos2lmadr [rhs_param_pos];
        rmadr = rhs_pos2rmadr [rhs_param_pos];

        return SXBA_bit_is_set (assigned_madr_set, lmadr) &&
            SXBA_bit_is_set (assigned_madr_set, rmadr);
    }

    param = XH_list_elem (rcg_predicates, cur2);

    if (param > 0) {
        bot3 = XH_X (rcg_parameters, param);
        top3 = XH_X (rcg_parameters, param+1);

        if (is_in_Tstar (bot3, top3) == 1) return true;

        /* Ici, l'arg contient des variables et n'est pas un range */
        for (cur3 = bot3; cur3 < top3; cur3++) {
            symb = XH_list_elem (rcg_parameters, cur3);

            if (symb > 0) {
                adr = get_lhs_ladr ((SXINT)0, symb);
                lmadr = adr2madr [adr];
                rmadr = adr2madr [adr+1];

                if (!SXBA_bit_is_set (assigned_madr_set, lmadr) ||
                    !SXBA_bit_is_set (assigned_madr_set, rmadr))
                    return false;
            }
        }
    }

    return true;
}


static SXBA	A_clause_set;
static bool  A_is_loop, A_is_cyclic;

static void
rhs_processing (SXINT level, SXINT clause, char *exit_string)
{
  /* On traite les predicats dont tous les args sont connus */
  SXINT         rhs, bot, cur, top, rhs_param_pos, rhs_prdct_pos, prdct, bot2, cur2, top2, nt, arity;
  SXINT         lmadr, rmadr, pos;
  SXINT         botc, curc, topc, rhs_processing_component_nb, tooth, generator_pos;
  SXINT         param, bot3, top3, l, cur3, symb, tstar1, tstar2, Ak, atom, counter_val, len;
  bool     has_c, range1, range2, is_pos_call, known_counter;

  rhs = XxY_Y (rcg_clauses, clause);
  top = XH_X (rcg_rhs, rhs+1);
  bot = XH_X (rcg_rhs, rhs);

  if (rhs_prdct_nyp_nb) {
    if (has_predef_prdct_call) {
      /* On le genere en 1er */
      rhs_param_pos = lhs_arity;

      for (rhs_prdct_pos = 1, cur = bot; cur < top; rhs_prdct_pos++, cur++) {
	prdct = XH_list_elem (rcg_rhs, cur);
	bot2 = XH_X (rcg_predicates, prdct);
	is_pos_call = XH_list_elem (rcg_predicates, bot2) == 0;	/* appel positif */
	nt = XH_list_elem (rcg_predicates, bot2+1);

	if (nt < 0 && nt != CUT_ic) {
	  /* Les &Cut sont evalues ds l'ordre gauche droite (quand ses args le sont) */
	  if (SXBA_bit_is_set (rhs_prdct_nyp, rhs_prdct_pos)
	      /* && nt == STREQ_ic || XH_list_elem (rcg_predicates, bot2) == 1 */) {
	    /* Les &False(), &First(), &Last(), &True(), &Lex() n'ont pas ete mis ds rhs_prdct_nyp */
	    /* Modif du Ven 10 Nov 2000 11:21:49 : &True() est mis ds rhs_prdct_nyp */
	    /* prdct non traite */
	    /* Evaluation dynamique du predicat predefini */
	    if (nt == TRUE_ic) {
	      /* Les appels negatifs ne sont pas pris en compte */
	      if (arg_evaluated (nt, true, rhs_param_pos, bot2+2)) {
		pos = rhs_param_pos - lhs_arity;
#if is_semantics==1
		fputs ("#if is_semantics\n", parser_file);
#endif /* is_semantics==1 */

		fprintf (parser_file, "rho[%ld] = MAKE_A_RANGE (olb[%ld], oub[%ld]); /* &True () */\n",
			 (SXINT) rhs_prdct_pos2tooth [rhs_prdct_pos]+1, (SXINT) pos, (SXINT) pos);
#if is_semantics==1
		fputs ("#endif /* is_semantics */\n", parser_file);
#endif /* is_semantics==1 */

		rhs_prdct_nyp_nb--;
		SXBA_0_bit (rhs_prdct_nyp, rhs_prdct_pos);
	      }
	    }
	    else {
	      if (arg_evaluated (nt, true, rhs_param_pos, bot2+2) &&
		  arg_evaluated (nt, false, rhs_param_pos+1, bot2+3)) {
		/* Les args sont evalues */
		range1 = SXBA_bit_is_set (pos_is_a_range, rhs_param_pos);
		range2 = SXBA_bit_is_set (pos_is_a_range, rhs_param_pos+1);

		param = XH_list_elem (rcg_predicates, bot2+2);
		bot3 = XH_X (rcg_parameters, param);

		if (nt == STRLEN_ic) {
		  len = XH_list_elem (rcg_parameters, bot3);

		  if (is_pos_call && (len = XH_list_elem (rcg_parameters, bot3)) != 0 /* lgth != 0 */) {
		    pos = rhs_param_pos - lhs_arity + 1;
		    fprintf (parser_file, "#if is_dag\n\
if (_StrLen (%ld, olb [%ld], oub [%ld]) < 0) %s;\n\
#endif /* is_dag */\n", len, pos, pos, exit_string);
		  }

		  tstar1 = -1; /* entier */
		}
		else {
		  top3 = XH_X (rcg_parameters, param+1);
		  tstar1 = is_in_Tstar (bot3, top3);/*  1 => ds T*
							-1 => entier seul
							0 => sinon
						    */
		}

		param = XH_list_elem (rcg_predicates, bot2+3);
		bot3 = XH_X (rcg_parameters, param);
		top3 = XH_X (rcg_parameters, param+1);
		tstar2 = is_in_Tstar (bot3, top3);

		pos = rhs_param_pos - lhs_arity;

		if ((!range1 && (tstar1 == 0)) || (!range2 && (tstar2 == 0))) {
		  /* Traitement special des streq, !strlen, !streqlen */
		  /* On peut generer */
				
		  switch (nt) {
		  case STREQ_ic:
		    fputs ("{\n", parser_file);
			    
		    if (!range1) {
		      param = XH_list_elem (rcg_predicates, bot2+2);
		      bot3 = XH_X (rcg_parameters, param);
		      top3 = XH_X (rcg_parameters, param+1);
		      fprintf (parser_file, "static SXINT *parg1, rcg_arg1 [] = {%ld,", (SXINT) top3-bot3);

		      for (cur3 = bot3; cur3 < top3; cur3++) {
			symb = -XH_list_elem (rcg_parameters, cur3);

			if (symb < 0) {
			  symb++;
			}

			/* Les variables sont en negatif ou nul */
			fprintf (parser_file, "%ld,", (SXINT) symb);
		      }

		      fputs ("};\n", parser_file);
		    }

		    if (!range2) {
		      param = XH_list_elem (rcg_predicates, bot2+3);
		      bot3 = XH_X (rcg_parameters, param);
		      top3 = XH_X (rcg_parameters, param+1);
		      fprintf (parser_file, "static SXINT *parg2, rcg_arg2 [] = {%ld,", (SXINT) top3-bot3);

		      for (cur3 = bot3; cur3 < top3; cur3++) {
			symb = -XH_list_elem (rcg_parameters, cur3);

			if (symb < 0) {
			  symb++;
			}

			/* Les variables sont en negatif ou nul */
			fprintf (parser_file, "%ld,", (SXINT) symb);
		      }

		      fputs ("};\n", parser_file);
		    }

		    if (!(range1 || tstar1)) {
		      fputs ("parg1 = StrConcat (true, rcg_arg1, Xlb, Xub);\n", parser_file);
		    }

		    if (!(range2 || tstar2)) {
		      fputs ("parg2 = StrConcat (false, rcg_arg2, Xlb, Xub);\n", parser_file);
		    }

		    fprintf (parser_file, "if (%s_StrEq2 (&(rho[%ld]), ",
			     is_pos_call ? "!" : "",
			     (SXINT) rhs_prdct_pos2tooth [rhs_prdct_pos]+1);

		    if (tstar1)
		      fputs ("rcg_arg1[0], rcg_arg1+1, ", parser_file);
		    else {
		      if (range1) {
			fprintf (parser_file, "oub[%ld]-olb[%ld], glbl_source_olb[%ld]+1, ",
				 (SXINT) pos, (SXINT) pos, (SXINT) pos);
		      }
		      else {
			fputs ("*parg1, parg1+1, ", parser_file);
		      }
		    }

		    if (tstar2)
		      fputs ("rcg_arg2[0], rcg_arg2+1", parser_file);
		    else {
		      if (range2) {
			fprintf (parser_file, "oub[%ld]-olb[%ld], glbl_source_olb[%ld]+1",
				 (SXINT) pos+1, (SXINT) pos+1, (SXINT) pos+1);
		      }
		      else {
			fputs ("*parg2, parg2+1", parser_file);
		      }
		    }

		    fprintf (parser_file, ")) %s;\n", exit_string);
				
		    fputs ("}\n", parser_file);
		    break;

		  case STRLEN_ic:
		    l = XH_list_elem (rcg_parameters, XH_X (rcg_parameters, XH_list_elem (rcg_predicates, bot2+2)));

		    fputs ("{\n", parser_file);
		    param = XH_list_elem (rcg_predicates, bot2+3);
		    bot3 = XH_X (rcg_parameters, param);
		    top3 = XH_X (rcg_parameters, param+1);
		    fprintf (parser_file, "static SXINT rcg_arg2 [] = {%ld,", (SXINT) top3-bot3);

		    for (cur3 = bot3; cur3 < top3; cur3++) {
		      symb = -XH_list_elem (rcg_parameters, cur3);

		      if (symb < 0) {
			symb++;
		      }

		      /* Les variables sont en negatif ou nul */
		      fprintf (parser_file, "%ld,", (SXINT) symb);
		    }

		    fputs ("};\n", parser_file);

		    param = XH_list_elem (rcg_predicates, bot2+2);
		    bot3 = XH_X (rcg_parameters, param);
		    fprintf (parser_file, "if (_StrLen2 (rcg_arg2, Xlb, Xub) == %ld) %s;\n", (SXINT) l, exit_string);
				    
		    fputs ("}\n", parser_file);

		    break;

		  case STREQLEN_ic:
		    fputs ("{\n", parser_file);
			    
		    if (!(range1 || tstar1)) {
		      param = XH_list_elem (rcg_predicates, bot2+2);
		      bot3 = XH_X (rcg_parameters, param);
		      top3 = XH_X (rcg_parameters, param+1);

		      fprintf (parser_file, "static SXINT rcg_arg1 [] = {%ld,", (SXINT) top3-bot3);

		      for (cur3 = bot3; cur3 < top3; cur3++) {
			symb = -XH_list_elem (rcg_parameters, cur3);

			if (symb < 0) {
			  symb++;
			}

			/* Les variables sont en negatif ou nul */
			fprintf (parser_file, "%ld,", (SXINT) symb);
		      }

		      fputs ("};\n", parser_file);
		    }

		    if (!(range2 || tstar2)) {
		      param = XH_list_elem (rcg_predicates, bot2+3);
		      bot3 = XH_X (rcg_parameters, param);
		      top3 = XH_X (rcg_parameters, param+1);

		      fprintf (parser_file, "static SXINT rcg_arg2 [] = {%ld,", (SXINT) top3-bot3);

		      for (cur3 = bot3; cur3 < top3; cur3++) {
			symb = -XH_list_elem (rcg_parameters, cur3);

			if (symb < 0) {
			  symb++;
			}

			/* Les variables sont en negatif ou nul */
			fprintf (parser_file, "%ld,", (SXINT) symb);
		      }

		      fputs ("};\n", parser_file);
		    }

		    fputs ("if (", parser_file);

		    if (tstar1) {
		      param = XH_list_elem (rcg_predicates, bot2+2);
		      bot3 = XH_X (rcg_parameters, param);
		      top3 = XH_X (rcg_parameters, param+1);
		      fprintf (parser_file, "%ld", (SXINT) top3-bot3);
		    }
		    else {
		      if (range1) {
			fprintf (parser_file, "oub[%ld]-olb[%ld]",
				 (SXINT) pos, (SXINT) pos);
		      }
		      else {
			fputs ("_StrLen2 (rcg_arg1, Xlb, Xub)", parser_file);
		      }
		    }

		    fputs ("==", parser_file);

		    if (tstar2) {
		      param = XH_list_elem (rcg_predicates, bot2+3);
		      bot3 = XH_X (rcg_parameters, param);
		      top3 = XH_X (rcg_parameters, param+1);
		      fprintf (parser_file, "%ld", (SXINT) top3-bot3);
		    }
		    else {
		      if (range2) {
			fprintf (parser_file, "oub[%ld]-olb[%ld]",
				 (SXINT) pos+1,(SXINT) pos+1);
		      }
		      else {
			fputs ("_StrLen2 (rcg_arg2, Xlb, Xub)", parser_file);
		      }
		    }

		    fprintf (parser_file, ")) %s;\n", exit_string);
				
		    fputs ("}\n", parser_file);
		    break;
		  default:
#if EBUG
		    sxtrap(ME,"unknown switch case #4");
#endif
		    break;
		  }
		}
		else {
		  /* Tout est connu, on peut generer */
		  /* On peut generer les appels */
		  if (is_pos_call) {
		    if (nt == STREQ_ic) {
		      /* Appel positif et nt == STREQ_ic */
		      if (tstar1) {
			if ((param = XH_list_elem (rcg_predicates, bot2+2))) {
			  bot3 = XH_X (rcg_parameters, param);
			  top3 = XH_X (rcg_parameters, param+1);
			  gen_StrEq_spcl (rhs_prdct_pos, pos+1, bot3, top3, exit_string, true);
			}
		      }
		      else {
#if 0
			fprintf (parser_file, "if (_StrEq (&(rho[%ld]), &(olb[%ld]), &(oub[%ld]))==-1) %s;\n",
				 rhs_prdct_pos2tooth [rhs_prdct_pos]+1, pos, pos, exit_string);
			/* Modif le Ven 22 Dec 2000 11:44:55 */
			fprintf (parser_file, "if (_StrEq (&(olb[%ld]), &(oub[%ld]))==-1) %s;\n",
				 (SXINT) pos, (SXINT) pos, exit_string);
#endif

			/* Modif le 22/04/08 */
			fprintf (parser_file, "if (_StrEq (olb[%ld], oub[%ld], olb[%ld], oub[%ld])==-1) %s;\n",
				 pos, pos, pos+1, pos+1, exit_string);
		      }
		    }
		  }
		  else {
		    /* Appel negatif */
		    switch (nt) {
		    case STREQ_ic:
		      if (tstar1) {
			if ((param = XH_list_elem (rcg_predicates, bot2+2))) {
			  bot3 = XH_X (rcg_parameters, param);
			  top3 = XH_X (rcg_parameters, param+1);
			  gen_StrEq_spcl (rhs_prdct_pos, pos+1, bot3, top3, exit_string, false);	
			}
			else
			  fprintf (parser_file, "if (oub[%ld]-olb[%ld]==0) %s;\n",
				   (SXINT) pos+1, (SXINT) pos+1, 
				   exit_string);
		      }
		      else {
#if 0
			fprintf (parser_file, "if (oub[%ld]-olb[%ld]==oub[%ld]-olb[%ld] && _StrEq (&(rho[%ld]), &(olb[%ld]), &(oub[%ld]))==1) %s;\n",
				 pos, pos,
				 pos+1, pos+1,
				 rhs_prdct_pos2tooth [rhs_prdct_pos]+1, 
				 pos, pos, exit_string);
#endif
			fprintf (parser_file, "if (oub[%ld]-olb[%ld]==oub[%ld]-olb[%ld] && _StrEq (&(olb[%ld]), &(oub[%ld]))==1) %s;\n",
				 (SXINT) pos, (SXINT) pos,
				 (SXINT) pos+1, (SXINT) pos+1,
				 (SXINT) pos, (SXINT) pos, exit_string);

		      }
		      break;

		    case STREQLEN_ic:
		      fprintf (parser_file, "if (oub[%ld]-olb[%ld]==oub[%ld]-olb[%ld]) %s;\n",
			       (SXINT) pos, (SXINT) pos,
			       (SXINT) pos+1, (SXINT) pos+1,
			       exit_string);
		      break;

		    case STRLEN_ic:
		      l = XH_list_elem (rcg_parameters, XH_X (rcg_parameters,
							      XH_list_elem (rcg_predicates, bot2+2)));

		      fprintf (parser_file, "if (oub[%ld]-olb[%ld]==%ld) %s;\n",
			       (SXINT) pos+1, (SXINT) pos+1,
			       (SXINT) l,
			       exit_string);

		      break;
		    default:
#if EBUG
		      sxtrap(ME,"unknown switch case #5");
#endif
		      break;
		    }
		  }

		}

		rhs_prdct_nyp_nb--;
		SXBA_0_bit (rhs_prdct_nyp, rhs_prdct_pos);
	      }
	    }
	  }
	}

	rhs_param_pos += XH_X (rcg_predicates, prdct+1)-bot2-2;
      }
    }

    rhs_prdct_pos = 1;
    rhs_param_pos = lhs_arity;

    for (cur = bot; cur < top; cur++) {
      prdct = XH_list_elem (rcg_rhs, cur);
      bot2 = XH_X (rcg_predicates, prdct);
      top2 = XH_X (rcg_predicates, prdct+1);
      arity = top2-bot2-2;
      is_pos_call = XH_list_elem (rcg_predicates, bot2) == 0;	/* appel positif */
      nt = XH_list_elem (rcg_predicates, bot2+1);

      if (nt > 0 || nt == CUT_ic) {
	if (SXBA_bit_is_set (rhs_prdct_nyp, rhs_prdct_pos)) {
	  has_c = false;

	  if (nt > 0)
	    Ak = A_k2Ak (nt, 0);
	  else
	    Ak = -arity; /* Pour le reconnaitre facilement */

	  for (pos = rhs_param_pos, cur2 = bot2+2; cur2 < top2; pos++, Ak++, cur2++) {
	    curc = topc = 0;

	    if (max_component_nb) {
	      /* On regarde si l'arg pos a des "..." */
	      for (rhs_processing_component_nb = 0; rhs_processing_component_nb < max_component_nb; rhs_processing_component_nb++) {
		if (component2pos [rhs_processing_component_nb] > pos)
		  /* Il n'en a pas */
		  break;

		if (component2pos [rhs_processing_component_nb] == pos) {
		  /* On regarde si tous les intermediaires sont verifies */
		  topc = botc = rhs_processing_component_nb;

		  while (++topc < max_component_nb && component2pos [topc] == pos);

		  if (topc-botc > 1) {
		    /* Il y a plusieurs composants, on regarde si les madr des oiub et oilb
		       sont traites */
		    for (curc = botc+1; curc < topc; curc++) {
		      rmadr = component2rmadr [curc-1];

		      if (!SXBA_bit_is_set (assigned_madr_set, rmadr))
			break;

		      lmadr = component2lmadr [curc];

		      if (!SXBA_bit_is_set (assigned_madr_set, lmadr))
			break;

		    }

		    if (curc == topc)
		      has_c = true;
		  }
		  else
		    curc = topc;

		  /* curc == topc => OK */
		  break;
		}
	      }

	      if (curc < topc) {
		/* Il en a mais tous les madr ne sont pas calcules */
		break;
	      }
	    }

	    if (topc == 0) {
	      /* pos n'a pas de "..." */
	      param = XH_list_elem (rcg_predicates, cur2);

	      /* Modif du 11/04/08 */
	      known_counter = false;

	      if (param > 0 && nt > 0 && pos2counter_val [pos] >= 0 /* compteur */) {
		/* On regarde s'il est evalue' */
		cur3 = bot3 = XH_X (rcg_parameters, param);
		top3 = XH_X (rcg_parameters, param+1);
	    
		while (cur3 < top3) {
		  symb = XH_list_elem (rcg_parameters, cur3);

		  if (symb > 0) {
		    atom = var2atom [symb];

		    if (!SXBA_bit_is_set (Asize_set, atom))
		      /* Non (encore) calcule' */
		      break;

		    cur3 += atom2var_nb [atom];
		  }
		  else
		    /* On a symb < -max_ste => OK */
		    cur3++;
		}

		if (cur3 < top3)
		  /* arg non calcule', on attend */
		  break;

		known_counter = true;
	      }

	      if (!known_counter) {
		lmadr = rhs_pos2lmadr [pos];
		rmadr = rhs_pos2rmadr [pos];

		if (!SXBA_bit_is_set (assigned_madr_set, lmadr) || !SXBA_bit_is_set (assigned_madr_set, rmadr))
		  break;
	      }
	    }
	  }

	  if (cur2 == top2) {
	    /* C'est OK */
	    rhs_prdct_nyp_nb--;
	    SXBA_0_bit (rhs_prdct_nyp, rhs_prdct_pos);

	    if (nt > 0) {
	      if (has_c) {
		/* On genere les tests
		   "if (oiub[p] > oilb[p+1]) exit;" */
		for (pos = rhs_param_pos, cur2 = bot2+2; cur2 < top2; pos++, cur2++) {
		  for (rhs_processing_component_nb = 0; rhs_processing_component_nb < max_component_nb; rhs_processing_component_nb++) {
		    if (component2pos [rhs_processing_component_nb] == pos) {
		      /* On regarde si tous les intermediaires sont verifies */
		      topc = botc = rhs_processing_component_nb;

		      while (++topc < max_component_nb && component2pos [topc] == pos);

		      if (topc-botc > 1) {
			/* Il y a plusieurs composants, on regarde si les madr des oiub et oilb
			   sont traites */
			for (curc = botc+1; curc < topc; curc++) {
			  rmadr = component2rmadr [curc-1];
			  lmadr = component2lmadr [curc];

			  fprintf (parser_file, "if (%s > %s) %s;\n",
				   varstr_tostr (madr2rcode [rmadr]),
				   varstr_tostr (madr2rcode [lmadr]),
				   exit_string);
			}
		      }
		    }
		  }
		}
	      }

	      pos = rhs_param_pos;
		
	      for (cur2 = bot2+2; cur2 < top2; pos++, cur2++) {
		if ((counter_val = pos2counter_val [pos]) >= 0) {
		  /* On genere le compteur */
		  fprintf (parser_file, "oub[%ld]=(olb[%ld]=counter_lb)", pos-lhs_arity, pos-lhs_arity);

		  param = XH_list_elem (rcg_predicates, cur2);
		  cur3 = bot3 = XH_X (rcg_parameters, param);
		  top3 = XH_X (rcg_parameters, param+1);

		  while (cur3 < top3) {
		    symb = XH_list_elem (rcg_parameters, cur3);

		    if (symb > 0) {
		      atom = var2atom [symb];
		      fprintf (parser_file, "+%s", varstr_tostr (atom2rsize [atom]));
		      cur3 += atom2var_nb [atom];
		    }
		    else
		      cur3++;
		  }

		  if (counter_val)
		    fprintf (parser_file, "+%ld", counter_val);

		  fputs (";\n", parser_file);
		}
	      }

	      pos = rhs_param_pos-lhs_arity;
	      tooth = rhs_prdct_pos2tooth [rhs_prdct_pos];
	      generator_pos = rhs_prdct_pos2generator_pos [rhs_prdct_pos];

	      if (is_pos_call) {
		if (arity) {
		  /*  Le 22/5/2002 */
		  if (Lex_file) {
		    /* Ds le cas is_earley_guide, *rho0 est calcule par l'appelant (arg in) */
		    /* ON suppose que si is_earley_guide => is_lex et is_dynam_lex=0 et is_guided2=0 et is_guided4=0 */
		    fprintf (parser_file,
"#if is_earley_guide\n\
local_val = MAKE_A_RANGE (olb [%ld], oub [%ld]);\n\
#if is_large\n\
if ((local_val = X_is_set (&range_1, local_val)) == 0) %s;\n\
#endif /* is_large */\n\
if ((rho [%ld] = X_is_set (&rhoA_hd, (local_val<<lognt)+%ld)) == 0) %s;\n\
#endif /* is_earley_guide */\n", (SXINT) pos, (SXINT) pos, exit_string, (SXINT) tooth+1, (SXINT) nt, exit_string);
		  }

		  if (is_1_RCG) {
		    fprintf (parser_file,
			     "if ((ret_vals[%ld] = _%s(&(rho[%ld]), &(olb[%ld]), &(oub[%ld])\n\
#if is_guided2\n\
, &(tc2rho[%ld])\n\
#endif\n\
))==-1) %s;\n",
			     (SXINT) generator_pos, sxstrget (nt2ste [nt]), (SXINT) tooth+1, (SXINT) pos, (SXINT) pos, (SXINT) pos, exit_string);
		  }
		  else {
		    fprintf (parser_file,
			     "if ((ret_vals[%ld] = _%s(&(rho[%ld]), &(olb[%ld]), &(oub[%ld])))==-1) %s;\n",
			     (SXINT) generator_pos, sxstrget (nt2ste [nt]), (SXINT) tooth+1, (SXINT) pos, (SXINT) pos, exit_string);

		  }
		}
		else
		  fprintf (parser_file,
			   "if ((ret_vals[%ld] = _%s(&(rho[%ld])))==-1) %s;\n",
			   (SXINT) generator_pos, sxstrget (nt2ste [nt]), (SXINT) tooth+1, exit_string);
	      }
	      else {
		/* negative call */
		/* Le 09/02/04, on ne se moque plus de la valeur de rho.  De plus on le passe en negatif
		   afin de reconnaitre facilement les appels negatifs... */
		/* negative call, on se moque de la valeur de rho */
		/* mais doit conserver la meme valeur a l'interieur de l'appel */
		if (arity) {
		  if (is_1_RCG) {
		    fprintf (parser_file,
			     "if ((ret_vals[%ld] = -_%s(&(rho[%ld]), &(olb[%ld]), &(oub[%ld])\n\
#if is_guided2\n\
, &(tc2rho[%ld])\n\
#endif\n\
))==-1) %s;rho[%ld] = -rho[%ld];\n",
			     (SXINT) generator_pos, sxstrget (nt2ste [nt]), (SXINT) tooth+1, (SXINT) pos, (SXINT) pos, (SXINT) pos, exit_string, (SXINT) tooth+1, (SXINT) tooth+1);
		  }
		  else {
		    fprintf (parser_file,
			     "if ((ret_vals[%ld] = -_%s(&(rho[%ld]), &(olb[%ld]), &(oub[%ld])))==-1) %s;rho[%ld] = -rho[%ld];\n",
			     (SXINT) generator_pos, sxstrget (nt2ste [nt]), (SXINT) tooth+1, (SXINT) pos, (SXINT) pos, exit_string, (SXINT) tooth+1, (SXINT) tooth+1);
		  }
		}
		else
		  fprintf (parser_file,
			   "if ((ret_vals[%ld] = -_%s(&(rho[%ld])))==-1) %s;rho[%ld] = -rho[%ld];\n",
			   (SXINT) generator_pos, sxstrget (nt2ste [nt]), (SXINT) tooth+1, exit_string, (SXINT) tooth+1, (SXINT) tooth+1);
	      }
	    }
	    else {
	      /* On est sur un Cut */
	      if (is_pos_call)
		fputs ("cut = true;\n", parser_file);
	      else
		fputs ("cut = false;\n", parser_file); /* Qu'est-ce a dire! */
	    }
	  }
	}
      }

      rhs_param_pos += arity;
      rhs_prdct_pos++;
    }
  }

  if (lhs_arg_tbp == 0 && rhs_prdct_nyp_nb == 0) {
#if EBUG
    if (first && (!sxba_is_empty (check_olb) || !sxba_is_empty (check_oub)))
      sxtrap (ME, "rhs_processing");
#endif
    /* A = XH_list_elem (rcg_predicates, XH_X (rcg_predicates, XxY_X (rcg_clauses, clause) & lhs_prdct_filter)+1); unused */

    fputs ("#if is_generator\n", parser_file);
    /* Appel des versions generator des predicats predefinis */

    if (max_generator_pos) {
      rhs_param_pos = 0;

      for (rhs_prdct_pos = 1, cur = bot; cur < top; rhs_prdct_pos++, cur++) {
	prdct = XH_list_elem (rcg_rhs, cur);
	bot2 = XH_X (rcg_predicates, prdct);

	is_pos_call = XH_list_elem (rcg_predicates, bot2) == 0;	/* appel positif */
		    
	nt = XH_list_elem (rcg_predicates, bot2+1);

	if (nt < 0) {
	  if (nt == STREQ_ic || nt == STREQLEN_ic || nt == STRLEN_ic) {
	    pos = rhs_param_pos - lhs_arity;

	    switch (nt) {
	    case STREQ_ic:
	      fprintf (parser_file, "ret_vals[%ld] = %s_GStrEq (&(olb[%ld]), &(oub[%ld]));\n",
		       (SXINT) rhs_prdct_pos2generator_pos [rhs_prdct_pos],
		       is_pos_call ? "" : "-", (SXINT) rhs_param_pos, (SXINT) rhs_param_pos);
	      break;

	    case STREQLEN_ic:
	      fputs ("#if AG\n", parser_file);
	      fprintf (parser_file, "ret_vals[%ld] = %s_GStrEqLen (&(olb[%ld]), &(oub[%ld]));\n",
		       (SXINT) rhs_prdct_pos2generator_pos [rhs_prdct_pos],
		       is_pos_call ? "" : "-", (SXINT) rhs_param_pos, (SXINT) rhs_param_pos);
	      fputs ("#endif\n", parser_file);
	      break;

	    case STRLEN_ic:
	      l = XH_list_elem (rcg_parameters, XH_X (rcg_parameters, XH_list_elem (rcg_predicates, bot2+2)));

	      fputs ("#if AG\n", parser_file);
	      fprintf (parser_file, "ret_vals[%ld] = %s_GStrLen (%ld, &(olb[%ld]), &(oub[%ld]));\n",
		       (SXINT) rhs_prdct_pos2generator_pos [rhs_prdct_pos],
		       is_pos_call ? "" : "-",
		       (SXINT) l,
		       (SXINT) rhs_param_pos, (SXINT) rhs_param_pos);
	      fputs ("#endif\n", parser_file);

	      break;
	    default:
#if EBUG
	      sxtrap(ME,"unknown switch case #6");
#endif
	      break;
	    }
	  }
	}

	rhs_param_pos += XH_X (rcg_predicates, prdct+1)-bot2-2;
      }
    }

    fprintf (parser_file, "if (!(ret_vals[0] = generator (&G, %ld, ret_vals, Indexes, ilb, iub, %s))) %s;\n",
	     (SXINT) clause, max_generator_pos == 0 ? "NULL, NULL" : "olb, oub", exit_string);
    fputs ("#endif /* is_generator */\n", parser_file);

    
    /* Modif du Jeu 16 Dec 1999 10:48:28 : Les appels negatifs sont sautes par la semantique, il ne sert a rien
       de la prevenir! De plus les appels negatifs n'etaient pas comptes ds max_tooth! Utiliser rhs_prdct_pos2tooth */

#if 0
    /* Le 07/08/08 */
    /* Modif du 28/02/08 suppression de local_ret_val */
    fputs ("ret_val = 1;\n", parser_file);
#endif /* 0 */

#if 0
    fprintf (parser_file, "%sret_val = 1;\n",
	     A_clause_nb > 1 ? "local_" : "");
#endif /* 0 */

    if (is_cyclic_grammar) {
      fputs ("#if is_cyclic\n\
if (!is_glbl_cycle) {\n\
#endif\n", parser_file);
    }

#if is_semantics==1
    fputs ("#if is_semantics\n", parser_file);
#endif /* is_semantics==1 */
    /* Modif du 28/05/2003 : l'affectation des indices des terminaux ds rho ne se fait que si on demande la semantique */
    if (lhs_t_nb) {
      fprintf (parser_file, "#if is_sdag && (is_parse_forest || is_2var_form || is_shallow)\n\
%s\n\
#endif\n", varstr_tostr (t_indexes_vstr));
    }

    fputs ("if (*rho0 == 0) rho[1] = *rho0 = local_Aij2global_Aij_pid [Aij] = MAKE_Aij_pid (Aij, G.pid);\n", parser_file); /* Le 07/08/08 */

    /* Modif du Mar  7 Nov 2000 14:07:00 : pour chaque clause le nombre de fils et chacun de leur pid sont
       stockes ds clause2son_nb et clause2son_pid */
    fputs ("if ((*G.semact.first_pass)(&G, rho", parser_file);

    fprintf (parser_file, "\n#if is_generator\n\
,ret_vals\n\
#endif\n\
) == -1) %s;\n", exit_string);

#if is_semantics==1
    fputs ("#endif /* is_semantics */\n", parser_file);
#endif /* is_semantics==1 */

    if (is_cyclic_grammar) {
      fputs ("#if is_cyclic\n\
}\n", parser_file);

      if (level > 1) {
	/* Si on n'est pas au niveau 1 (et si l'ip est vrai, ca ne sert a rien de continuer */
	fputs ("else break;\n", parser_file);
      }

      fputs ("#endif /* is_cyclic */\n", parser_file);
    }

    /* Le 07/08/08 */
    /* Modif du 28/02/08 suppression de local_ret_val */
    fputs ("ret_val = 1;\n", parser_file);

    if (level > 1) {
      fputs ("#if is_recognizer\n\
break;\n\
#endif\n", parser_file);
    }
  }
}





static void
gen_first_test (SXINT clause, SXINT rhs_pos, char *exit_stmt)
{
  /* On vient de positionner les olb de rhs_pos */
  /* On genere, quand c'est possible les tests qui mettent en jeu first */
  /* Les tests sur les ranges qui ne mettent pas en jeu Ak sont sortis de la boucle */
  SXINT		symb, rmadr, Ak, param, nt, rhs_prdct_pos, call_kind;
  char	kind;
  char	cur_tok_string [32];
  bool	is_lcur_tok, is_oub, ff, fe, fv;

  is_lcur_tok = false;
  param = rhs_pos2param [rhs_pos];
  symb = XH_list_elem (rcg_parameters, XH_X (rcg_parameters, param));
    
  if (!SXBA_bit_is_set (rhs_pos2neg_call_set, rhs_pos)) {
    Ak = rhs_pos2Ak [rhs_pos];

    if (Ak < 0) {
      /* 2eme arg de &First ou &Last */
      rhs_prdct_pos = -Ak;
      nt = rhs_pos2Ak [rhs_pos-1];

      /* Sur la borne inf de last, on ne fait rien */
      if (nt == FIRST_ic) {
	is_lcur_tok = true;
	is_source_checked = true;
	call_kind = XH_list_elem (rcg_predicates,
				  XH_X (rcg_predicates,
					XH_list_elem (rcg_rhs,
						      XH_X (rcg_rhs,
							    XxY_Y (rcg_clauses,
								   clause
								   )
							    )+rhs_prdct_pos-1
						      )
					)
				  );

	fprintf (parser_file, "if (lcur_tok = LSOURCE(olb[%ld]), %sFIRST_LAST(lcur_tok, first_%ld_%ld)) %s;\n",
		 (SXINT) rhs_pos-lhs_arity,
		 call_kind == 1 ? "!" : "",
		 (SXINT) clause,
		 (SXINT) rhs_prdct_pos,
		 exit_stmt);
      }
    }
    else {
      /* rmadr == 0 => la borne sup de l'arg se termine par "..." */
      rmadr = rhs_pos2rmadr [rhs_pos];
      /* La borne sup a-t-elle deja ete calculee ? */
      /* La taille de l'arg est connue dynamiquement "oub-olb" */
      is_oub = rmadr == 0 || SXBA_bit_is_set (assigned_madr_set, rmadr);

      if (is_oub) {
	if (first) {
	  if (SXBA_bit_is_set_reset (check_nul_range, rhs_pos) &&
	      SXBA_bit_is_reset_set (check_nul_range_done, rhs_pos)) {
	    /* oub a deja ete calcule, il reste a verifier que le range est vide */
	    fputs ("#if is_first_last\n", parser_file);
	    fprintf (parser_file, "if (olb[%ld] != oub[%ld]) %s;\n",
		     (SXINT) rhs_pos-lhs_arity,
		     (SXINT) rhs_pos-lhs_arity,
		     exit_stmt);
	    fputs ("#endif\n", parser_file);
	  }

	  if (SXBA_bit_is_set_reset (check_non_nul_range, rhs_pos) &&
	      SXBA_bit_is_reset_set (check_non_nul_range_done, rhs_pos)) {
	    /* oub a deja ete calcule, il reste a verifier que le range est non vide */
	    /* Si on le sait statiquement, on suppose que ca a ete fait (ou sera fait)
	       par ailleurs (le Ven  3 Mar 2000 16:16:58) */
	    if (SXBA_bit_is_set (first [Ak], 0)) {
	      fputs ("#if is_first_last\n", parser_file);
	      fprintf (parser_file, "if (oub[%ld] <= olb[%ld]) %s;\n",
		       (SXINT) rhs_pos-lhs_arity,
		       (SXINT) rhs_pos-lhs_arity,
		       exit_stmt);
	      fputs ("#endif\n", parser_file);
	    }
	  }

	  if (SXBA_bit_is_set_reset (check_olb, rhs_pos)) {
	    /* test de LAST */
	    is_source_checked = true;
	    fputs ("#if is_first_last\n", parser_file);
	    fprintf (parser_file,
		     "if (olb[%ld]<oub[%ld] && (rcur_tok = SOURCE(oub[%ld]), FIRST_LAST(rcur_tok,last_%ld))) %s;\n",
		     (SXINT) rhs_pos-lhs_arity,
		     (SXINT) rhs_pos-lhs_arity,
		     (SXINT) rhs_pos-lhs_arity,
		     (SXINT) Ak,
		     exit_stmt);
	    fputs ("#endif\n", parser_file);
	  }
	}
      }

      if (symb >= 0
	  && first /* ESSAI du 13/05/04 */) {
	ff = SXBA_bit_is_set (is_first_complete, Ak);
	fv = SXBA_bit_is_set (is_first_vide, Ak);
	fe = SXBA_bit_is_set (first [Ak], 0);
    
	if (!(ff && fe)) {
	  /* First n'est pas qcq */
	  /* Le test n'est pas toujours vrai */
	  kind = rhs_pos2static_size_kind [rhs_pos];

	  if (!ff) {
	    if (!fv) {
	      /* On a \Phi < First (Ak) < T */
	      /* On peut donc lancer le test du token courant a la condition
		 que la taille de l'arg soit strictement positive */
	      /* Cette condition peut etre statique ou dynamique */
	      /* On pourrait de plus utiliser les min A FAIRE */
	      /* Ajoute le Ven  3 Mar 2000 14:55:12 : si Ak ne derive pas ds le vide
		 on suppose que le test que le range est non vide a deja ete effectue' */
	      if (kind == NonNullSize || !SXBA_bit_is_set (first [Ak], 0)) {
		/* Statique */
		/* On est sur que la taille du range courant est non nulle */

		if (first) {
		  fputs ("#if is_first_last\n", parser_file);

		  if (!is_lcur_tok) {
		    is_lcur_tok = true;
		    is_source_checked = true;
		    sprintf (cur_tok_string, "lcur_tok = LSOURCE(olb[%ld]), ",
			     (SXINT) rhs_pos-lhs_arity);
		  }
		  else
		    cur_tok_string [0] = SXNUL;

		  fprintf (parser_file, "if (%sFIRST_LAST(lcur_tok,first_%ld)) %s;\n",
			   cur_tok_string,
			   (SXINT) Ak,
			   exit_stmt);

		  fputs ("#endif\n", parser_file);
		}
	      }
	      else {
		if (!fe) {
		  /* ici l'arg ne doit pas etre vide et cur_tok ne peut pas
		     etre qcq. On fait la verif sur cur_tok, celle sur la taille
		     de l'arg est differee (quand on connaitra la borne sup) */
		  if (kind == NullSize) {
		    /* Toujours faux */
		    if (first) {
		      fputs ("#if is_first_last\n", parser_file);
		      fprintf (parser_file, "%s;\n", exit_stmt);
		      fputs ("#endif\n", parser_file);
		    }
		  }
		  else {
		    /* kind == UnknownSize */
		    if (first) {
		      if (is_oub) {
			if (SXBA_bit_is_reset_set (check_non_nul_range_done, rhs_pos)) {
			  /* Si on le sait statiquement, on suppose que ca a ete fait (ou sera fait)
			     par ailleurs (le Ven  3 Mar 2000 16:16:58) */
			  if (SXBA_bit_is_set (first [Ak], 0)) {
			    fputs ("#if is_first_last\n", parser_file);
			    fprintf (parser_file, "if (oub[%ld] <= olb[%ld]) %s;\n",
				     (SXINT) rhs_pos-lhs_arity,
				     (SXINT) rhs_pos-lhs_arity,
				     exit_stmt);
			    fputs ("#endif\n", parser_file);
			  }
			}
		      }
		      else {
			SXBA_1_bit (check_non_nul_range, rhs_pos);
		      }

		      fputs ("#if is_first_last\n", parser_file);

		      if (!is_lcur_tok) {
			is_lcur_tok = true;
			is_source_checked = true;
			sprintf (cur_tok_string, "lcur_tok = LSOURCE(olb[%ld]), ",
				 (SXINT) rhs_pos-lhs_arity);
		      }
		      else
			cur_tok_string [0] = SXNUL;

		      /* Attention, ca veut dire que si le range est nul, lcur_tok
			 est a l'exterieur du range */
		      /* Au pire il repere eof */
		      /* Si le token qui suit le range est ds first, on continue en
			 sequence, l'erreur sera trouvee sur le test de la taille du
			 range. */
		      fprintf (parser_file, "if (%sFIRST_LAST(lcur_tok,first_%ld)) %s;\n",
			       cur_tok_string,
			       (SXINT) Ak,
			       exit_stmt);
		      fputs ("#endif\n", parser_file);
		    }
		  }
		}
		else {
		  /* !ff && !fv && fe */
		  /* ici l'arg peut etre vide.  On ne peut faire le test sur cur_tok
		     qui peut donc ne pas etre dans le range. Le test complet est
		     differe si on ne connait pas oub */
		  if (kind == UnknownSize) {
		    if (first) {
		      if (is_oub) {
			/* On doit faire le test (meme si l'oub n'en a pas besoin) */
			fputs ("#if is_first_last\n", parser_file);

			if (!is_lcur_tok) {
			  is_lcur_tok = true;
			  is_source_checked = true;
			  sprintf (cur_tok_string, "lcur_tok = LSOURCE(olb[%ld]), ",
				   (SXINT) rhs_pos-lhs_arity);
			}
			else
			  cur_tok_string [0] = SXNUL;

			fprintf (parser_file, "if (olb[%ld]<oub[%ld] && (%sFIRST_LAST(lcur_tok,first_%ld))) %s;\n",
				 (SXINT) rhs_pos-lhs_arity,
				 (SXINT) rhs_pos-lhs_arity,
				 cur_tok_string,
				 (SXINT) Ak,
				 exit_stmt);
			fputs ("#endif\n", parser_file);
		      }
		      else {
			/* La borne sup n'est pas encore calculee,
			   on fera ce test + tard */
			SXBA_1_bit (check_oub, rhs_pos);
		      }
		    }
		  }
		  /* else kind == NullSize => rien */
		}
	      }
	    }
	    else {
	      /* On doit verifier que le range est vide */
#if EBUG
	      if (!fe)
		sxtrap (ME, "gen_first_test");
#endif

	      if (kind == NonNullSize) {
		/* Toujours faux */
		if (first) {
		  fputs ("#if is_first_last\n", parser_file);
		  fprintf (parser_file, "%s;\n", exit_stmt);
		  fputs ("#endif\n", parser_file);
		}
	      }
	      else {
		if (kind == UnknownSize) {
		  if (first) {
		    if (is_oub) {
		      /* On doit faire le test (meme si l'oub n'en a pas besoin) */
		      /* Ce test est fait une seule fois pour tous les arg identiques */
		      if (SXBA_bit_is_reset_set (check_nul_range_done, rhs_pos)) {
			fputs ("#if is_first_last\n", parser_file);
			fprintf (parser_file, "if (olb[%ld] != oub[%ld]) %s;\n",
				 (SXINT) rhs_pos-lhs_arity,
				 (SXINT) rhs_pos-lhs_arity,
				 exit_stmt);
			fputs ("#endif\n", parser_file);
		      }
		    }
		    else {
		      /* La borne sup n'est pas encore calculee, on fera ce test + tard */
		      /* Ce test ne doit etre fait qu'une fois pour tous les args
			 identiques */
		      SXBA_1_bit (check_nul_range, rhs_pos);
		    }
		  }
		}
		/* else kind == NullSize => tj vrai */
	      }
	    }
	  }
	  else {
	    /* Ici tous les terminaux sont valides.
	       et \epsilon n'est pas valide. */
	    /* la seule possibilite d'echec est une taille dynamique vide (size == 0) */
	    if (kind == NullSize) {
	      /* Toujours faux */
	      if (first) {
		fputs ("#if is_first_last\n", parser_file);
		fprintf (parser_file, "%s;\n", exit_stmt);
		fputs ("#endif\n", parser_file);
	      }
	    }
	    else {
	      if (kind == UnknownSize) {
		if (first) {
		  if (is_oub) {
		    /* On doit faire le test (meme si l'oub n'en a pas besoin) */
		    /* Ce test est fait une seule fois pour tous les arg identiques */
		    if (SXBA_bit_is_reset_set (check_non_nul_range_done, rhs_pos)) {
		      /* Si on le sait statiquement, on suppose que ca a ete fait (ou sera fait)
			 par ailleurs (le Ven  3 Mar 2000 16:16:58) */
		      if (SXBA_bit_is_set (first [Ak], 0)) {
			fputs ("#if is_first_last\n", parser_file);
			fprintf (parser_file, "if (oub[%ld] <= olb[%ld]) %s;\n",
				 (SXINT) rhs_pos-lhs_arity,
				 (SXINT) rhs_pos-lhs_arity,
				 exit_stmt);
			fputs ("#endif\n", parser_file);
		      }
		    }
		  }
		  else {
		    /* La borne sup n'est pas encore calculee, on fera ce test + tard */
		    /* Ce test ne doit etre fait qu'une fois pour tous les args
		       identiques */
		    SXBA_1_bit (check_non_nul_range, rhs_pos);
		  }
		}
	      }
	      /* else
		 taille non vide, aucun echec possible */
	    }
	  }
	}
	/* else
	   Le test sur first est inutile, il serait toujours vrai */
      }
      /* else
	 Le test de first est inutile si l'arg commence par un terminal */
    }		   
  }
}



static void
gen_last_test (SXINT clause, SXINT rhs_pos, char *exit_stmt)
{
  /* On vient de positionner les oub de rhs_pos */
  /* On genere, quand c'est possible les tests qui mettent en jeu last */
  /* Les tests sur les ranges qui ne mettent pas en jeu Ak sont sortis de la boucle */
  SXINT		symb, lmadr, Ak, param, nt, rhs_prdct_pos, call_kind;
  char	kind;
  char	cur_tok_string [32];
  bool	is_rcur_tok, is_olb, lf, le, lv;

  is_rcur_tok = false;
  param = rhs_pos2param [rhs_pos];
  symb = XH_list_elem (rcg_parameters, XH_X (rcg_parameters, param+1)-1);
    
  if (!SXBA_bit_is_set (rhs_pos2neg_call_set, rhs_pos)) {
    Ak = rhs_pos2Ak [rhs_pos];

    if (Ak < 0) {
      /* 2eme arg de &First ou &Last */
      rhs_prdct_pos = -Ak;
      nt = rhs_pos2Ak [rhs_pos-1];

      /* Sur la borne sup de first, on ne fait rien */
      if (nt == LAST_ic) {
	is_rcur_tok = true;
	is_source_checked = true;
	call_kind = XH_list_elem (rcg_predicates,
				  XH_X (rcg_predicates,
					XH_list_elem (rcg_rhs,
						      XH_X (rcg_rhs,
							    XxY_Y (rcg_clauses,
								   clause
								   )
							    )+rhs_prdct_pos-1
						      )
					)
				  );

	fprintf (parser_file, "if (rcur_tok = SOURCE(oub[%ld]), %sFIRST_LAST(rcur_tok, last_%ld_%ld)) %s;\n",
		 (SXINT) rhs_pos-lhs_arity,
		 call_kind == 1 ? "!" : "",
		 (SXINT) clause,
		 (SXINT) rhs_prdct_pos,
		 exit_stmt);
      }
    }
    else {
      /* lmadr == 0 => la borne inf de l'arg commence par "..." */
      lmadr = rhs_pos2lmadr [rhs_pos];
      /* La borne inf a-t-elle deja ete examinee ? */
      /* La taille de l'arg est connue dynamiquement "oub-olb" */
      is_olb = lmadr == 0 || SXBA_bit_is_set (assigned_madr_set, lmadr);

      if (is_olb) {
	if (first) {
	  if (SXBA_bit_is_set_reset (check_nul_range, rhs_pos) &&
	      SXBA_bit_is_reset_set (check_nul_range_done, rhs_pos)) {
	    /* olb a deja ete calcule, il reste a verifier que le range est vide */
	    fputs ("#if is_first_last\n", parser_file);
	    fprintf (parser_file, "if (olb[%ld] != oub[%ld]) %s;\n",
		     (SXINT) rhs_pos-lhs_arity,
		     (SXINT) rhs_pos-lhs_arity,
		     exit_stmt);
	    fputs ("#endif\n", parser_file);
	  }

	  if (SXBA_bit_is_set_reset (check_non_nul_range, rhs_pos) &&
	      SXBA_bit_is_reset_set (check_non_nul_range_done, rhs_pos)) {
	    /* olb a deja ete calcule, il reste a verifier que le range est non vide */
	    /* Si on le sait statiquement, on suppose que ca a ete fait (ou sera fait)
	       par ailleurs (le Ven  3 Mar 2000 16:16:58) */
	    if (SXBA_bit_is_set (first [Ak], 0)) {
	      fputs ("#if is_first_last\n", parser_file);
	      fprintf (parser_file, "if (oub[%ld] <= olb[%ld]) %s;\n",
		       (SXINT) rhs_pos-lhs_arity,
		       (SXINT) rhs_pos-lhs_arity,
		       exit_stmt);
	      fputs ("#endif\n", parser_file);
	    }
	  }

	  if (SXBA_bit_is_set_reset (check_oub, rhs_pos)) {
	    /* test de FIRST */
	    fputs ("#if is_first_last\n", parser_file);
	    is_source_checked = true;
	    fprintf (parser_file,
		     "if (olb[%ld]<oub[%ld] && (lcur_tok = LSOURCE(olb[%ld]), FIRST_LAST(lcur_tok,first_%ld))) %s;\n",
		     (SXINT) rhs_pos-lhs_arity,
		     (SXINT) rhs_pos-lhs_arity,
		     (SXINT) rhs_pos-lhs_arity,
		     (SXINT) Ak,
		     exit_stmt);
	    fputs ("#endif\n", parser_file);
	  }
	}
      }

      if (symb >= 0
	  && first /* ESSAI du 13/05/04 */) {
	lf = SXBA_bit_is_set (is_last_complete, Ak);
	lv = SXBA_bit_is_set (is_last_vide, Ak);
	le = SXBA_bit_is_set (last [Ak], 0);
    
	if (!(lf && le)) {
	  /* Last n'est pas qcq */
	  /* Le test n'est pas toujours vrai */
	  kind = rhs_pos2static_size_kind [rhs_pos];

	  if (!lf) {
	    if (!lv) {
	      /* On a \Phi < Last (Ak) < T */
	      /* On peut donc lancer le test du token courant a la condition
		 que la taille de l'arg soit strictement positive */
	      /* Cette condition peut etre statique ou dynamique */
	      /* On pourrait de plus utiliser les min A FAIRE */
	      /* Ajoute le Ven  3 Mar 2000 14:55:12 : si Ak ne derive pas ds le vide
		 on suppose que le test que le range est non vide a deja ete effectue' */
	      if (kind == NonNullSize || !SXBA_bit_is_set (first [Ak], 0)) {
		/* Statique */
		/* On est sur que la taille du range courant est non nulle */
		/* On lance la verif, l'arg se termine par une var ou ... */

		if (first) {
		  fputs ("#if is_first_last\n", parser_file);

		  if (!is_rcur_tok) {
		    is_rcur_tok = true;
		    is_source_checked = true;
		    sprintf (cur_tok_string, "rcur_tok = SOURCE(oub[%ld]), ",
			     (SXINT) rhs_pos-lhs_arity);
		  }
		  else
		    cur_tok_string [0] = SXNUL;

		  fprintf (parser_file, "if (%sFIRST_LAST(rcur_tok,last_%ld)) %s;\n",
			   cur_tok_string,
			   (SXINT) Ak,
			   exit_stmt);

		  fputs ("#endif\n", parser_file);
		}
	      }
	      else {
		if (!le) {
		  /* ici l'arg ne doit pas etre vide et cur_tok ne peut pas
		     etre qcq. On fait la verif sur cur_tok, celle sur la taille
		     de l'arg est differee (quand on connaitra la borne inf) */
		  if (kind == NullSize) {
		    /* Toujours faux */
		    if (first) {
		      fputs ("#if is_first_last\n", parser_file);
		      fprintf (parser_file, "%s;\n", exit_stmt);
		      fputs ("#endif\n", parser_file);
		    }
		  }
		  else {
		    /* kind == UnknownSize */
		    if (first) {
		      if (is_olb) {
			if (SXBA_bit_is_reset_set (check_non_nul_range_done, rhs_pos)) {
			  /* Si on le sait statiquement, on suppose que ca a ete fait (ou sera fait)
			     par ailleurs (le Ven  3 Mar 2000 16:16:58) */
			  if (SXBA_bit_is_set (first [Ak], 0)) {
			    fputs ("#if is_first_last\n", parser_file);
			    fprintf (parser_file, "if (oub[%ld] <= olb[%ld]) %s;\n",
				     (SXINT) rhs_pos-lhs_arity,
				     (SXINT) rhs_pos-lhs_arity,
				     exit_stmt);
			    fputs ("#endif\n", parser_file);
			  }
			}
		      }
		      else {
			SXBA_1_bit (check_non_nul_range, rhs_pos);
		      }

		      fputs ("#if is_first_last\n", parser_file);

		      if (!is_rcur_tok) {
			is_rcur_tok = true;
			is_source_checked = true;
			sprintf (cur_tok_string, "rcur_tok = SOURCE(oub[%ld]), ",
				 (SXINT) rhs_pos-lhs_arity);
		      }
		      else
			cur_tok_string [0] = SXNUL;

		      fprintf (parser_file, "if (%sFIRST_LAST(rcur_tok,last_%ld)) %s;\n",
			       cur_tok_string,
			       (SXINT) Ak,
			       exit_stmt);

		      fputs ("#endif\n", parser_file);
		    }
		  }
		}
		else {
		  /* !lf && !lv && le */
		  /* ici l'arg peut etre vide.  On ne peut faire le test sur cur_tok
		     qui peut donc ne pas etre dans le range. Le test complet est
		     differe si on ne connait pas olb */
		  if (kind == UnknownSize) {
		    if (first) {
		      if (is_olb) {
			/* On doit faire le test (meme si l'oub n'en a pas besoin) */
			fputs ("#if is_first_last\n", parser_file);

			if (!is_rcur_tok) {
			  is_rcur_tok = true;
			  is_source_checked = true;
			  sprintf (cur_tok_string, "rcur_tok = SOURCE(oub[%ld]), ",
				   (SXINT) rhs_pos-lhs_arity);
			}
			else
			  cur_tok_string [0] = SXNUL;

			fprintf (parser_file, "if (olb[%ld]<oub[%ld] && (%sFIRST_LAST(rcur_tok,last_%ld))) %s;\n",
				 (SXINT) rhs_pos-lhs_arity,
				 (SXINT) rhs_pos-lhs_arity,
				 cur_tok_string,
				 (SXINT) Ak,
				 exit_stmt);
			fputs ("#endif\n", parser_file);
		      }
		      else {
			/* La borne inf n'est pas encore calculee,
			   on fera ce test + tard */
			SXBA_1_bit (check_olb, rhs_pos);
		      }
		    }
		  }
		  /* else kind == NullSize => rien */
		}
	      }
	    }
	    else {
	      /* On doit verifier que le range est vide */
#if EBUG
	      if (!le)
		sxtrap (ME, "gen_last_test");
#endif

	      if (kind == NonNullSize) {
		/* Toujours faux */
		if (first) {
		  fputs ("#if is_first_last\n", parser_file);
		  fprintf (parser_file, "%s;\n", exit_stmt);
		  fputs ("#endif\n", parser_file);
		}
	      }
	      else {
		if (kind == UnknownSize) {
		  if (first) {
		    if (is_olb) {
		      /* On doit faire le test (meme si l'olb n'en a pas besoin) */
		      /* Ce test est fait une seule fois pour tous les arg identiques */
		      if (SXBA_bit_is_reset_set (check_nul_range_done, rhs_pos)) {
			fputs ("#if is_first_last\n", parser_file);
			fprintf (parser_file, "if (olb[%ld] != oub[%ld]) %s;\n",
				 (SXINT) rhs_pos-lhs_arity,
				 (SXINT) rhs_pos-lhs_arity,
				 exit_stmt);
			fputs ("#endif\n", parser_file);
		      }
		    }
		    else {
		      /* La borne inf n'est pas encore calculee, on fera ce test + tard */
		      /* Ce test ne doit etre fait qu'une fois pour tous les args
			 identiques */
		      SXBA_1_bit (check_nul_range, rhs_pos);
		    }
		  }
		}
		/* else kind == NullSize => tj vrai */
	      }
	    }
	  }
	  else {
	    /* Ici tous les terminaux sont valides.
	       et \epsilon n'est pas valide. */
	    /* la seule possibilite d'echec est une taille dynamique vide (size == 0) */
	    if (kind == NullSize) {
	      /* Toujours faux */
	      if (first) {
		fputs ("#if is_first_last\n", parser_file);
		fprintf (parser_file, "%s;\n", exit_stmt);
		fputs ("#endif\n", parser_file);
	      }
	    }
	    else {
	      if (kind == UnknownSize) {
		if (first) {
		  if (is_olb) {
		    /* On doit faire le test (meme si l'oub n'en a pas besoin) */
		    /* Ce test est fait une seule fois pour tous les arg identiques */
		    if (SXBA_bit_is_reset_set (check_non_nul_range_done, rhs_pos)) {
		      /* Si on le sait statiquement, on suppose que ca a ete fait (ou sera fait)
			 par ailleurs (le Ven  3 Mar 2000 16:16:58) */
		      if (SXBA_bit_is_set (first [Ak], 0)) {
			fputs ("#if is_first_last\n", parser_file);
			fprintf (parser_file, "if (oub[%ld] <= olb[%ld]) %s;\n",
				 (SXINT) rhs_pos-lhs_arity,
				 (SXINT) rhs_pos-lhs_arity,
				 exit_stmt);
			fputs ("#endif\n", parser_file);
		      }
		    }
		  }
		  else {
		    /* La borne sup n'est pas encore calculee, on fera ce test + tard */
		    /* Ce test ne doit etre fait qu'une fois pour tous les args
		       identiques */
		    SXBA_1_bit (check_non_nul_range, rhs_pos);
		  }
		}
	      }
	      /* else
		 taille non vide, aucun echec possible */
	    }
	  }
	}
	/* else
	   Le test sur last est inutile, il serait toujours vrai */
      }
      /* else
	 Le test de last est inutile si l'arg se termine par un terminal */
    }		   
  }
}

#if 0
/* Le 19/12/03 : nelle version */
/* Peut peut-etre servir de modele!! */
static void
gen_min_max_test (clause, rhs_pos, exit_stmt)
    SXINT 	clause, rhs_pos;
    char	*exit_stmt;
{
  SXINT Ak, max;

  if (!SXBA_bit_is_set (rhs_pos2nc_max_set, rhs_pos)) {
    param = rhs_pos2param [rhs_pos];

    pos = -1;

    while ((pos = sxba_scan (rhs_pos2_min_max_test, pos)) >= 0) {
      if (rhs_pos2param [pos] == param)
	/* Deja traite' */
	break;
    }

    if (pos < 0) {
      /* Nouvel arg */
      if (param > 0) {
	cur3 = XH_X (rcg_parameters, param);
	top3 = XH_X (rcg_parameters, param+1);
	gmin = gmax = 0;

	while (cur3 < top3) {
	  symb = XH_list_elem (rcg_parameters, cur3);

	  if (symb < 0) {
	    gmin++;
	    gmax++;
	    cur3++;
	  }
	  else {
	    atom = var2atom [symb];

	    if ((max = atom2max [atom]) == -1)
	      /* longueur inconnue */
	      break;

	    gmax += max;
	    gmin += atom2min [atom];
	    cur3 += atom2var_nb [atom];
	  }
	}

	if (cur3 >= top3) {
	  /* OK */
	  fprintf (parser_file, "if (oub[%ld]-olb[%ld] > %ld) %s;\n",
		   rhs_pos-lhs_arity,
		   rhs_pos-lhs_arity,
		   gmax,
		   exit_stmt);
	}
      }
    }

    SXBA_1_bit (rhs_pos2_min_max_test, rhs_pos);
  }
}
#endif /* 0 */

static void
gen_min_max_test (/* clause, */ SXINT rhs_pos, char *exit_stmt)
{
  SXINT Ak, max;

  /* A FAIRE : si le meme arg combinatoire est utilise plusieurs fois en rhs, le test ci-dessous est
     genere plusieurs fois !! */
  if (!SXBA_bit_is_set (rhs_pos2neg_call_set, rhs_pos) && !SXBA_bit_is_set (rhs_pos2nc_max_set, rhs_pos)) {
    Ak = rhs_pos2Ak [rhs_pos];
    max = Ak2max [Ak];

    if (max >= 0) {
      fprintf (parser_file, "if (oub[%ld]-olb[%ld] > %ld) %s;\n",
	       (SXINT) rhs_pos-lhs_arity,
	       (SXINT) rhs_pos-lhs_arity,
	       (SXINT) max/* -Ak2min [Ak] le 19/12/03 */,
	       exit_stmt);
    }
  }
}

static void
gen_rfsa_guide (SXINT nt, SXINT rhs_pos, char *exit_stmt)
{
  fprintf (parser_file, "if (!fsa_nt2lb (%ld, olb[%ld])) %s;\n", (SXINT) nt, (SXINT) rhs_pos-lhs_arity, exit_stmt);
}


static void
gen_lfsa_guide (SXINT nt, SXINT rhs_pos, char *exit_stmt)
{
  fprintf (parser_file, "if (!fsa_nt2ub (%ld, oub[%ld])) %s;\n", (SXINT) nt, (SXINT) rhs_pos-lhs_arity, exit_stmt);
}


static void
gen_karg_guide (/* clause, */ SXINT rhs_pos, char *exit_stmt)
{
  /* Les olb et oub de l'arg rhs_pos viennent d'etre positionnes */
  /* C'est l'argument k de l'appel de A */
  /* Si le parser est guide'/couple', on genere le test pour savoir si Ak ([olb..oub]) est dans le guide */

  SXINT Ak = rhs_pos2Ak [rhs_pos] /* , A = Ak2A (Ak), k = Ak_A2k (Ak, A) */ ;

  fputs ("#if is_guided2\n", parser_file);

#if 0
  /* On genere un truc comme ca */
#if is_robust
  if (is_robust_run)
    ;
  else
#else /* !is_robust */
    {
#if EBUG2
      iarg_searched_in_guide++;
#endif
#if !is_robust
      if (!coupled_prdct (0, 3)) continue;
#endif
#if EBUG2
      iarg_found_in_guide++;
#endif
    }
#endif /* !is_robust */
#endif /* #if 0 */

  fputs ("#if is_robust\n\
if (is_robust_run)\n\
;\n\
else\n\
#else /* !is_robust */\n\
{\n", parser_file);

  fputs ("#if EBUG2\n\
iarg_searched_in_guide++;\n\
#endif\n", parser_file);

  fprintf (parser_file, "if (%scoupled_prdct (%ld, %ld)) %s;\n",
	   SXBA_bit_is_set (rhs_pos2neg_call_set, rhs_pos) ? "" :"!",
	   (SXINT) rhs_pos-lhs_arity,
	   (SXINT) Ak2guide_nt [Ak] /* code interne ds le guide du nt A_k */,
	   exit_stmt);

  fputs ("#if EBUG2\n\
iarg_found_in_guide++;\n\
#endif\n", parser_file);

  fputs ("}\n\
#endif /* !is_robust */\n", parser_file);

  fputs ("#endif /* is_guided2 */\n", parser_file);
}


static bool
is_atom_in_arg (SXINT atom, SXINT pos)
{
    SXINT		ladr, radr, symb, cur_atom;

    ladr = lhs_pos2ladr [pos];
    radr = lhs_pos2radr [pos];

    while (ladr < radr) {
	symb = XH_list_elem (rcg_parameters, adr2cur [ladr]);

	if (symb > 0) {
	    if ((cur_atom = var2atom [symb]) == atom)
		return true;

	    ladr += atom2var_nb [cur_atom];
	}
	else
	    ladr++;
    }

    return false;
}

static VARSTR
unknown_arg_size (SXINT pos)
{
  /* On calcule ds wvstr la somme cummulee des tailles inconnues (apres) pos */
  SXINT		tnb, ladr, radr, symb, atom;
  char	        string [23];
  bool	is_first = true;

  wvstr = varstr_raz (wvstr);
  wvstr = varstr_catenate (wvstr, varstr_tostr (lhs_pos2rvstr [pos]));
  wvstr = varstr_catenate (wvstr, " - (");
  wvstr = varstr_catenate (wvstr, varstr_tostr (lhs_pos2lvstr [pos]));
  wvstr = varstr_catenate (wvstr, ")");

  tnb = 0;
  ladr = lhs_pos2ladr [pos];
  symb = XH_list_elem (rcg_parameters, adr2cur [ladr]);

#if EBUG
  /* pos doit referencer une variable */
  if (symb <= 0)
    sxtrap (ME, "unknown_arg_size");
#endif

  ladr += atom2var_nb [var2atom [symb]];
  radr = lhs_pos2radr [pos];

  while (ladr < radr) {
    symb = XH_list_elem (rcg_parameters, adr2cur [ladr]);

    if (symb > 0) {
      atom = var2atom [symb];

      if (SXBA_bit_is_set (Asize_set, atom)) {
	if (is_first) {
	  is_first = false;
	  wvstr = varstr_catenate (wvstr, " - (");
	}
	else
	  wvstr = varstr_catenate (wvstr, "+");

	wvstr = varstr_catenate (wvstr, varstr_tostr (atom2rsize [atom]));
      }
      else {
	/* ajoute le Ven  3 Mar 2000 13:43:32 */
	tnb += atom2min [atom];
      }

      ladr += atom2var_nb [atom];
    }
    else {
      tnb++;
      ladr++;
    }
  }

  if (!is_first)
    wvstr = varstr_catenate (wvstr, ")");

  if (tnb > 0) {
    sprintf (string, " - %ld", (SXINT) tnb);

    wvstr = varstr_catenate (wvstr, string);
  }

  return wvstr;
}


static SXINT
adr2Index (SXINT pos, SXINT adr)
{
    /* adr est une adresse ds le LHS arg pos. retourne -1 si adr est une ladr ou une radr ou l'ordre de
       l'intervalle (0 pour le + a gauche, 1 pour le suivant, etc...) */
    if (lhs_pos2ladr [pos] == adr || lhs_pos2radr[pos] == adr)
	return -1;

    return adr-lhs_pos2ladr [pos]-1;
}

static bool
lhs_arg_processing (SXINT clause, SXINT pos, char *exit_stmt, bool first_call)
{
  SXINT		  adr, ladr, radr, madr, symb, atom, rhs_pos, cur_rhs_pos, Index, rmadr, lmadr, nt;
  VARSTR	  lvstr, rvstr;
#if 0
  SXINT           Ak;
  bool       has_counter, has_range;
#endif
  char	          string [22];

  ladr = lhs_pos2ladr [pos];
  radr = lhs_pos2radr [pos];

  if (ladr < radr) {
    /* Normalement, si ladr == radr => first_call == true && arg == \varepsilon */
    lvstr = lhs_pos2lvstr [pos]; /* Contient une chaine qui repere le source-index de la partie basse
				    du lhs-arg pos, traite jusqu'ici Ex "ilb[pos]+2" */

    do {
      symb = XH_list_elem (rcg_parameters, adr2cur [ladr]);
      madr = adr2madr [ladr];
	
#if EBUG
      if (madr <= 0)
	sxtrap (ME, "lhs_arg_processing");
#endif

      if (SXBA_bit_is_reset_set (already_seen_madr_set, madr)) {
	if (SXBA_bit_is_set (is_madr_an_ob, madr)) {
	  /* Cette adresse principale correspond a au moins une borne d'un arg de la RHS */
	  /* On genere l'affectation a des ranges */
	  /* attention, avec les compteurs, la meme variable (de range) en lhs peut etre utilisee en rhs
	     ds un range ou ds un compteur. */
#if 0
	  has_counter = false;

	  for (cur_rhs_pos = madr2olb [madr]; cur_rhs_pos > 0; cur_rhs_pos = olbs [cur_rhs_pos]) {
	    Ak = rhs_pos2Ak [cur_rhs_pos];

	    if (SXBA_bit_is_set (counter_Ak_set, Ak)) {
	      has_counter = true;
	      fprintf (parser_file, "olb[%ld]=", cur_rhs_pos-lhs_arity);
	    }
	  }

	  if (has_counter) {
	    fputs ("=counter_lb;\n", parser_file);
	  }

	  has_range = false;

	  for (cur_rhs_pos = madr2olb [madr]; cur_rhs_pos > 0; cur_rhs_pos = olbs [cur_rhs_pos]) {
	    Ak = rhs_pos2Ak [cur_rhs_pos];

	    if (!SXBA_bit_is_set (counter_Ak_set, Ak)) {
	      has_range = true;
	      fprintf (parser_file, "olb[%ld]=", cur_rhs_pos-lhs_arity);

	      /* Tests first/last*/
	      nt = rhs_pos2nt [cur_rhs_pos];

	      if (rfsa_file && nt > 0)
		gen_rfsa_guide (nt, cur_rhs_pos, exit_stmt);

	      if (nt == FIRST_ic || (nt > 0 && SXBA_bit_is_set (is_nt_internal, nt)))
		/* Pas de test de first/last sur les predicats externes (predefinis ou non) */
		gen_first_test (clause, cur_rhs_pos, exit_stmt);

	      if ((rmadr = rhs_pos2rmadr [cur_rhs_pos]) == 0 || SXBA_bit_is_set (assigned_madr_set, rmadr)) {
		/* La borne sup a deja ete calculee */
		if (is_1_RCG && nt > 0)  {
		  gen_karg_guide (/* clause, */cur_rhs_pos, exit_stmt);
		}

		if (nt > 0 && SXBA_bit_is_set (is_nt_internal, nt)) {
		  gen_min_max_test (/* clause, */cur_rhs_pos, exit_stmt);
		}
	      }
	    }
	  }

	  if (has_range) {
	    fprintf (parser_file, "=%s;\n", varstr_tostr (lvstr));
	  }

	  if (rhs_pos > 0 && (Ak = rhs_pos2Ak [rhs_pos]) > 0 && SXBA_bit_is_set (counter_Ak_set, Ak)) {
	    /* On force les lb a etre counter_lb */
	    fprintf (parser_file, "%s = counter_lb;\n",
		   varstr_tostr (madr2lcode [madr]));
	    
	  }
	  else {
	    fprintf (parser_file, "%s = %s;\n",
		     varstr_tostr (madr2lcode [madr]), varstr_tostr (lvstr));

	    /* On regarde s'il est possible de generer le test first sur cet arg */
	    /* madr2olb [madr] == 0 si rhs_pos est un arg d'un predicat externe (predefini ou non) */
	    /* Ou si madr correspond a une borne interne d'un composant :
	       Ex borne inf de Y ds ...Y */
	    if (rhs_pos > 0) {
	      cur_rhs_pos = rhs_pos;

	      do {
		nt = rhs_pos2nt [cur_rhs_pos];

		if (rfsa_file && nt > 0)
		  gen_rfsa_guide (nt, cur_rhs_pos, exit_stmt);

		if (nt == FIRST_ic || (nt > 0 && SXBA_bit_is_set (is_nt_internal, nt)))
		  /* Pas de test de first/last sur les predicats externes (predefinis ou non) */
		  gen_first_test (clause, cur_rhs_pos, exit_stmt);

		if ((rmadr = rhs_pos2rmadr [cur_rhs_pos]) == 0 || SXBA_bit_is_set (assigned_madr_set, rmadr)) {
		  /* La borne sup a deja ete calculee */
		  if (is_1_RCG && nt > 0)  {
		    gen_karg_guide (/* clause, */cur_rhs_pos, exit_stmt);
		  }

		  if (nt > 0 && SXBA_bit_is_set (is_nt_internal, nt)) {
		    gen_min_max_test (/* clause, */cur_rhs_pos, exit_stmt);
		  }
		}		
	      } while ((cur_rhs_pos = olbs [cur_rhs_pos]) > 0);
	    }
	  }  
#endif /* 0 */	 
	    
	  fprintf (parser_file, "%s = %s;\n",
		   varstr_tostr (madr2lcode [madr]), varstr_tostr (lvstr));

	  /* On regarde s'il est possible de generer le test first sur cet arg */
	  /* madr2olb [madr] == 0 si rhs_pos est un arg d'un predicat externe (predefini ou non) */
	  /* Ou si madr correspond a une borne interne d'un composant :
	     Ex borne inf de Y ds ...Y */
	  rhs_pos = madr2olb [madr];

	  if (rhs_pos > 0) {
	    cur_rhs_pos = rhs_pos;

	    do {
	      nt = rhs_pos2nt [cur_rhs_pos];

	      if (rfsa_file && nt > 0)
		gen_rfsa_guide (nt, cur_rhs_pos, exit_stmt);

	      if (nt == FIRST_ic || (nt > 0 && SXBA_bit_is_set (is_nt_internal, nt)))
		/* Pas de test de first/last sur les predicats externes (predefinis ou non) */
		gen_first_test (clause, cur_rhs_pos, exit_stmt);

	      if ((rmadr = rhs_pos2rmadr [cur_rhs_pos]) == 0 || SXBA_bit_is_set (assigned_madr_set, rmadr)) {
		/* La borne sup a deja ete calculee */
		if (is_1_RCG && nt > 0)  {
		  gen_karg_guide (/* clause, */cur_rhs_pos, exit_stmt);
		}

		if (nt > 0 && SXBA_bit_is_set (is_nt_internal, nt)) {
		  gen_min_max_test (/* clause, */cur_rhs_pos, exit_stmt);
		}
	      }		
	    } while ((cur_rhs_pos = olbs [cur_rhs_pos]) > 0);
	  }
 
	  SXBA_1_bit (assigned_madr_set, madr);
	  /* On en profite pour changer le code de l'index courant */
	  lvstr = varstr_catenate (varstr_raz (lvstr), varstr_tostr (madr2rcode [madr]));

	  /* madr peut aussi correspondre a une upper_bound */
	  /* madr2oub [madr] == 0 si rhs_pos est un arg d'un predicat externe (predefini ou non) */
	  if ((rhs_pos = madr2oub [madr]) > 0) {
	    cur_rhs_pos = rhs_pos;

	    do {
	      nt = rhs_pos2nt [cur_rhs_pos];

	      if (lfsa_file && nt > 0)
		gen_lfsa_guide (nt, cur_rhs_pos, exit_stmt);

	      if (nt == LAST_ic || (nt > 0 && SXBA_bit_is_set (is_nt_internal, nt)))
		/* Pas de test de first/last sur les predicats externes (predefinis ou non) */
		gen_last_test (clause, cur_rhs_pos, exit_stmt);

	      if ((lmadr = rhs_pos2lmadr [cur_rhs_pos]) == 0 || SXBA_bit_is_set (assigned_madr_set, lmadr)) {
		/* La borne inf a deja ete calculee */
		if (nt > 0 && is_1_RCG)  {
		  gen_karg_guide (/* clause, */cur_rhs_pos, exit_stmt);
		}

		if (nt > 0 && SXBA_bit_is_set (is_nt_internal, nt)) {
		  gen_min_max_test (/* clause, */cur_rhs_pos, exit_stmt);
		}
	      }		
	    } while ((cur_rhs_pos = oubs [cur_rhs_pos]) > 0);
	  }
	}

	if (SXBA_bit_is_set (synchro_set, madr)) {
	  /* C'est un point de synchronisation */
	  /* C'est la 1ere fois qu'on tombe dessus */
	  /* On note le source index */
	  if (!SXBA_bit_is_set (is_madr_an_ob, madr)) {
	    /* Cet index ne correspond pas a une borne d'un arg de la RHS, mais il va etre utilise */
	    if ((adr = synchro2adr [madr]) != 0) {
	      /* Ce point de synchronisation est une input bound, on utilisera donc "ilb" ou "iub" */
	      /* Evidemment, l'input bound choisie est verifiee a priori */
	      if (adr > 0) {
		/* C'est une iub */
		sprintf (string, "iub[%ld]", (SXINT) adr2pos [adr]);
	      }
	      else {
		adr = -adr;
		sprintf (string, "ilb[%ld]", (SXINT) adr2pos [adr]);
	      }

	      SXBA_1_bit (nyp_synchro_test, adr);
	    }
	    else {
	      /* on genere une affectation ds "Sindex" et on le note */
#if EBUG
	      if (Sindex >= ASindex_nb)
		sxtrap (ME, "lhs_arg_processing");
#endif
	      sprintf (string, "Sindex[%ld]", (SXINT) Sindex);
	      Sindex++;
	      fprintf (parser_file, "%s = %s;\n",
		       string, varstr_tostr (lvstr));
	    }

	    lvstr = varstr_catenate (varstr_raz (lvstr), string);
	  }

	  synchro2vstr [madr] = varstr_catenate (varstr_raz (synchro2vstr [madr]), varstr_tostr (lvstr));
	}

	if (symb < 0) {
	  if (symb >= -max_ste) {
	    /* Vrai terminal (pas constante) */
	    is_source_checked = true;
	    
	    if (terminal_file)
	      fprintf (parser_file, "\
if ((rho [%ld] = p_T_q2Tpq (%s, SXT_%s, iub[%ld])) == 0) %s;\n",
		       (SXINT) (max_son_nb+2+1+adr2tpos[ladr]), varstr_tostr (lvstr), sxstrget (t2Cid_ste [-symb]), pos, exit_stmt);
	    else
	      fprintf (parser_file, "\
if ((rho [%ld]= p_T_q2Tpq (%s, %ld, iub[%ld])) == 0) %s;\n",
		       (SXINT) (max_son_nb+2+1+adr2tpos[ladr]), varstr_tostr (lvstr), (SXINT) -symb, pos, exit_stmt);

	    t_indexes_vstr = varstr_catenate (t_indexes_vstr, "rho[");
	    t_indexes_vstr = i2varstr (t_indexes_vstr, max_son_nb+2+adr2tpos[ladr]);
	    t_indexes_vstr = varstr_catenate (t_indexes_vstr, "] = ");
	    t_indexes_vstr = varstr_catenate (t_indexes_vstr, varstr_tostr (lvstr));
	    t_indexes_vstr = varstr_catenate (t_indexes_vstr, "+1;");

	    if ((Index = adr2Index (pos, ladr)) >= 0) {
	      /* Ce n'est pas la borne d'un input range */
	      /* Il se trouve a l'index relatif ds pos : Index */
	      Index += lhs_pos2ladr [pos] - 2*pos -1;
	      fputs ("#if is_generator\n", parser_file);
	      fprintf (parser_file, "Indexes[%ld] = %s;\n",
		       (SXINT) Index, varstr_tostr (lvstr));
	      fputs ("#endif\n", parser_file);
	    }

	    SXBA_1_bit (tmadr_set, madr);
	    lvstr = varstr_catenate (lvstr, "+1");
	  }
	  else {
	    /* constante */
	    sprintf (string, "+%ld", -symb - max_ste -1);
	    lvstr = varstr_catenate (lvstr, string);
	  }
	    
	  ladr++;
	}
	else {
	  atom = var2atom [symb];

	  if ((Index = adr2Index (pos, ladr)) >= 0) {
	    /* Ce n'est pas la borne d'un input range */
	    /* Il se trouve a l'index relatif ds pos : Index */
	    Index += lhs_pos2ladr [pos] - 2*pos -1;
	    fputs ("#if is_generator\n", parser_file);
	    fprintf (parser_file, "Indexes[%ld] = %s;\n",
		     (SXINT) Index, varstr_tostr (lvstr));
	    fputs ("#endif\n", parser_file);
	  }

	  if (SXBA_bit_is_set (Asize_set, atom)) {
	    /* On connait la taille de cet atome */
	    lvstr = varstr_catenate (varstr_catenate (lvstr, "+"), varstr_tostr (atom2rsize [atom]));
	    ladr += atom2var_nb [atom];
	    /* C'est forcement un atome central dont la taille est calculee */
	    /* On l'enleve, (il n'est plus central) */
	    /* Attention, il peut deja etre a zero, uniquement ds le cas ou cet atome
	       a recu une taille statique. */
	  }
	  else
	    break;
	}
      }
      else {
	/* Cette adresse principale a deja ete examinee */
	if (SXBA_bit_is_set (synchro_set, madr) &&
	    SXBA_bit_is_reset_set (nyp_synchro_test, ladr)) {
	  /* C'est un point de synchronisation */
	  /* son source index a deja ete note */
	  /* et sa verification n'a pas ete faite */
	  /* on la fait */
	  /*
	    Les points de synchro sont traites par des equations, la verif est donc
	    toujours faite
	    fprintf (parser_file, "if (%s != %s) %s;\n",
	    varstr_tostr (lvstr), varstr_tostr (synchro2vstr [madr]), exit_stmt);
	  */
	  /* on reaffecte */
	  lvstr = varstr_catenate (varstr_raz (lvstr), varstr_tostr (synchro2vstr [madr]));
	}

	if (symb < 0) {
	  /* Je ne sais plus pourquoi on peut passer par la car un terminal
	     a toujours du etre traite' pre'ce'demment !! */
	  if (symb >= -max_ste) {
	    /* Vrai terminal (pas constante) */
	    /* Les terminaux (ladr, radr) qui se trouvent "autour" de points de synchronisations
	       doivent etre verifies une et une seule fois.  Attention, ils n'occurrent pas
	       forcement autour de madr mais peut etre autour des ladr, radr ! */
	    if (SXBA_bit_is_reset_set (tmadr_set, madr)) {
	      is_source_checked = true;

	      if (terminal_file)
		fprintf (parser_file, "\
if ((rho [%ld] = p_T_q2Tpq (%s, SXT_%s, iub[%ld])) == 0) %s;\n",
		       (SXINT) (max_son_nb+2+1+adr2tpos[ladr]), varstr_tostr (lvstr), sxstrget (t2Cid_ste [-symb]), pos, exit_stmt);
	      else
		fprintf (parser_file, "\
if ((rho [%ld] = p_T_q2Tpq (%s, %ld, iub[%ld])) == 0) %s;\n",
		       (SXINT) (max_son_nb+2+1+adr2tpos[ladr]), varstr_tostr (lvstr), (SXINT) -symb, pos, exit_stmt);

	      t_indexes_vstr = varstr_catenate (t_indexes_vstr, "rho[");
	      t_indexes_vstr = i2varstr (t_indexes_vstr, max_son_nb+2+adr2tpos[ladr]);
	      t_indexes_vstr = varstr_catenate (t_indexes_vstr, "] = ");
	      t_indexes_vstr = varstr_catenate (t_indexes_vstr, varstr_tostr (lvstr));
	      t_indexes_vstr = varstr_catenate (t_indexes_vstr, "+1;");
	    }

	    if ((Index = adr2Index (pos, ladr)) >= 0) {
	      /* Ce n'est pas la borne d'un input range */
	      /* Il se trouve a l'index relatif ds pos : Index */
	      Index += lhs_pos2ladr [pos] - 2*pos -1;
	      fputs ("#if is_generator\n", parser_file);
	      fprintf (parser_file, "Indexes[%ld] = %s;\n",
		       (SXINT) Index, varstr_tostr (lvstr));
	      fputs ("#endif\n", parser_file);
	    }

	    lvstr = varstr_catenate (lvstr, "+1");
	  }
	  else {
	    /* constante */
	    sprintf (string, "+%ld", -symb - max_ste -1);
	    lvstr = varstr_catenate (lvstr, string);
	  }

	  ladr++;
	}
	else {
	  atom = var2atom [symb];

	  if ((Index = adr2Index (pos, ladr)) >= 0) {
	    /* Ce n'est pas la borne d'un input range */
	    /* Il se trouve a l'index relatif ds pos : Index */
	    Index += lhs_pos2ladr [pos] - 2*pos -1;
	    fputs ("#if is_generator\n", parser_file);
	    fprintf (parser_file, "Indexes[%ld] = %s;\n",
		     (SXINT) Index, varstr_tostr (lvstr));
	    fputs ("#endif\n", parser_file);
	  }

	  if (SXBA_bit_is_set (Asize_set, atom)) {
	    /* On connait la taille de cet atome */
	    lvstr = varstr_catenate (varstr_catenate (lvstr, "+"), varstr_tostr (atom2rsize [atom]));
	    ladr += atom2var_nb [atom];
	    /* C'est forcement un atome central dont la taille est calculee */
	    /* On l'enleve, (il n'est plus central) */
	    /* Attention, il peut deja etre a zero, uniquement ds le cas ou cet atome
	       a recu une taille statique. */
	  }
	  else
	    break;
	}
      }
    } while (ladr < radr);

    lhs_pos2lvstr [pos] = lvstr;
    lhs_pos2ladr [pos] = ladr;

    if (ladr < radr || first_call) {
      /* first_call est positionne lors du 1er appel sur cet arg */
      /* Ca permet de traiter le cas ou la iub de cet arg sert par ailleurs */
      /* Exemple A(X) --> &StrLen(1,X) B(X) . */
      rvstr = lhs_pos2rvstr [pos]; /* Contient une chaine qui repere le source-index de la partie haute
				      du lhs-arg pos, traite jusqu'ici Ex "iub[pos]-2" */
    
      do {
	symb = XH_list_elem (rcg_parameters, adr2cur [radr]-1);
	madr = adr2madr [radr];
	
#if EBUG
	if (madr <= 0)
	  sxtrap (ME, "lhs_arg_processing");
#endif

	if (SXBA_bit_is_reset_set (already_seen_madr_set, madr)) {
	  if (SXBA_bit_is_set (is_madr_an_ob, madr)) {
	    /* Cette adresse principale correspond a au moins une borne d'un arg de la RHS */
	    rhs_pos = madr2oub [madr];

#if 0
	    if (rhs_pos > 0 && (Ak = rhs_pos2Ak [rhs_pos]) > 0 && SXBA_bit_is_set (counter_Ak_set, Ak)) {
	      SXINT   counter_val, atom;
	      bool local_generation;

	      local_generation = false;

	      if (symb > 0) {
		atom = var2atom [symb];

		if (SXBA_bit_is_set (Asize_set, atom)) {
		  counter_val = pos2counter_val [rhs_pos];

		  if (counter_val == -1)
		    counter_val = 0;
	      
		  fprintf (parser_file, "%s = counter_lb + %s + %ld;\n",
			   varstr_tostr (madr2lcode [madr]),
			   varstr_tostr (atom2rsize [atom]),
			   counter_val);
		  local_generation = true;
	      
		  SXBA_1_bit (assigned_madr_set, madr);
		  /* On en profite pour changer le code de l'index courant */
		  rvstr = varstr_catenate (varstr_raz (rvstr), varstr_tostr (madr2rcode [madr]));	      
		}
	      }

	      if (!local_generation)
		SXBA_0_bit (already_seen_madr_set, madr);
	    }
	    else {
	      /* On genere l'affectation */
	      fprintf (parser_file, "%s = %s;\n",
		       varstr_tostr (madr2lcode [madr]), varstr_tostr (rvstr));
	      /* madr2oub [madr] == 0 si rhs_pos est un arg d'un predicat externe (predefini ou non) */
	      SXBA_1_bit (assigned_madr_set, madr);
	      /* On en profite pour changer le code de l'index courant */
	      rvstr = varstr_catenate (varstr_raz (rvstr), varstr_tostr (madr2rcode [madr]));
	      /* Ou si madr correspond a une borne interne d'un composant :
		 Ex borne sup de Y ds Y... */
	      if (rhs_pos > 0) {
		/* On regarde s'il est possible de generer le test last sur cet arg */
		cur_rhs_pos = rhs_pos;

		do {
		  nt = rhs_pos2nt [cur_rhs_pos];

		  if (lfsa_file && nt > 0)
		    gen_lfsa_guide (nt, cur_rhs_pos, exit_stmt);

		  if (nt == LAST_ic || (nt > 0 && SXBA_bit_is_set (is_nt_internal, nt)))
		    /* Pas de test de first/last sur les predicats externes (predefinis ou non) */
		    gen_last_test (clause, cur_rhs_pos, exit_stmt);

		  if ((lmadr = rhs_pos2lmadr [cur_rhs_pos]) == 0 || SXBA_bit_is_set (assigned_madr_set, lmadr)) {
		    /* La borne inf a deja ete calculee */
		    if (nt > 0 && is_1_RCG)  {
		      gen_karg_guide (/* clause, */cur_rhs_pos, exit_stmt);
		    }

		    if (nt > 0 && SXBA_bit_is_set (is_nt_internal, nt)) {
		      gen_min_max_test (/* clause, */cur_rhs_pos, exit_stmt);
		    }
		  }		
		} while ((cur_rhs_pos = oubs [cur_rhs_pos]) > 0);
	      }
	    }
#endif /* 0 */

	    /* On genere l'affectation */
	    fprintf (parser_file, "%s = %s;\n",
		     varstr_tostr (madr2lcode [madr]), varstr_tostr (rvstr));
	    /* madr2oub [madr] == 0 si rhs_pos est un arg d'un predicat externe (predefini ou non) */
	    SXBA_1_bit (assigned_madr_set, madr);
	    /* On en profite pour changer le code de l'index courant */
	    rvstr = varstr_catenate (varstr_raz (rvstr), varstr_tostr (madr2rcode [madr]));
	    /* Ou si madr correspond a une borne interne d'un composant :
	       Ex borne sup de Y ds Y... */

	    if (rhs_pos > 0) {
	      /* On regarde s'il est possible de generer le test last sur cet arg */
	      cur_rhs_pos = rhs_pos;

	      do {
		nt = rhs_pos2nt [cur_rhs_pos];

		if (lfsa_file && nt > 0)
		  gen_lfsa_guide (nt, cur_rhs_pos, exit_stmt);

		if (nt == LAST_ic || (nt > 0 && SXBA_bit_is_set (is_nt_internal, nt)))
		  /* Pas de test de first/last sur les predicats externes (predefinis ou non) */
		  gen_last_test (clause, cur_rhs_pos, exit_stmt);

		if ((lmadr = rhs_pos2lmadr [cur_rhs_pos]) == 0 || SXBA_bit_is_set (assigned_madr_set, lmadr)) {
		  /* La borne inf a deja ete calculee */
		  if (nt > 0 && is_1_RCG)  {
		    gen_karg_guide (/* clause, */cur_rhs_pos, exit_stmt);
		  }

		  if (nt > 0 && SXBA_bit_is_set (is_nt_internal, nt)) {
		    gen_min_max_test (/* clause, */cur_rhs_pos, exit_stmt);
		  }
		}		
	      } while ((cur_rhs_pos = oubs [cur_rhs_pos]) > 0);
	    }

	    /* madr peut aussi correspondre a une lower_bound */
	    /* madr2olb [madr] == 0 si rhs_pos est un arg d'un predicat externe (predefini ou non) */
	    if ((rhs_pos = madr2olb [madr]) > 0) {
	      cur_rhs_pos = rhs_pos;

	      do {
		nt = rhs_pos2nt [cur_rhs_pos];

		if (rfsa_file && nt > 0)
		  gen_rfsa_guide (nt, cur_rhs_pos, exit_stmt);

		if (nt == FIRST_ic || (nt > 0 && SXBA_bit_is_set (is_nt_internal, nt)))
		  /* Pas de test de first/last sur les predicats externes (predefinis ou non) */
		  gen_first_test (clause, cur_rhs_pos, exit_stmt);

		if ((rmadr = rhs_pos2rmadr [cur_rhs_pos]) == 0 || SXBA_bit_is_set (assigned_madr_set, rmadr)) {
		  /* La borne sup a deja ete calculee */
		  if (nt > 0 && is_1_RCG)  {
		    gen_karg_guide (/* clause, */cur_rhs_pos, exit_stmt);
		  }

		  if (nt > 0 && SXBA_bit_is_set (is_nt_internal, nt)) {
		    gen_min_max_test (/* clause, */cur_rhs_pos, exit_stmt);
		  }
		}		
	      } while ((cur_rhs_pos = olbs [cur_rhs_pos]) > 0);
	    }
	  }

	  if (SXBA_bit_is_set (synchro_set, madr)) {
	    /* C'est un point de synchronisation */
	    /* C'est la 1ere fois qu'on tombe dessus */
	    /* On note le source index */
	    if (!SXBA_bit_is_set (is_madr_an_ob, madr)) {
	      /* Cet index ne correspond pas a une borne d'un arg de la RHS, mais il va etre utilise */
	      if ((adr = synchro2adr [madr]) != 0) {
				/* Ce point de synchronisation est une input bound, on utilisera donc "ilb" ou "iub" */
				/* Evidemment, l'input bound choisie est verifiee a priori */
		if (adr > 0) {
		  /* C'est une iub */
		  sprintf (string, "iub[%ld]", (SXINT) adr2pos [adr]);
		}
		else {
		  adr = -adr;
		  sprintf (string, "ilb[%ld]", (SXINT) adr2pos [adr]);
		}

		SXBA_1_bit (nyp_synchro_test, adr);
	      }
	      else {
		/* on genere une affectation ds "Sindex" et on le note */
#if EBUG
		if (Sindex >= ASindex_nb)
		  sxtrap (ME, "lhs_arg_processing");
#endif
		sprintf (string, "Sindex[%ld]", (SXINT) Sindex);
		Sindex++;
		fprintf (parser_file, "%s = %s;\n",
			 string, varstr_tostr (rvstr));
	      }

	      rvstr = varstr_catenate (varstr_raz (rvstr), string);
	    }

	    synchro2vstr [madr] = varstr_catenate (varstr_raz (synchro2vstr [madr]), varstr_tostr (rvstr));
	  }

	  if (ladr < radr) {
	    if (symb < 0) {
	      radr--;

	      if (symb < -max_ste) {
		/* constante */
		sprintf (string, "-%ld", -symb - max_ste -1);
		rvstr = varstr_catenate (rvstr, string);
	      }
	      else {
		/* Vrai terminal (pas constante) */
		madr = adr2madr [radr];
		SXBA_1_bit (tmadr_set, madr);
		is_source_checked = true;

		if (terminal_file)
		  fprintf (parser_file, "if ((q=%s)==ilb[%ld]/* empty range */\
 || (rho [%ld] = p_T_q2Tpq (q-1, SXT_%s, q) == 0) %s;\n",
			     varstr_tostr (rvstr), pos, (SXINT) (max_son_nb+2+1+adr2tpos[radr]), sxstrget (t2Cid_ste [-symb]), exit_stmt);
		else
		  fprintf (parser_file, "if ((q=%s)==ilb[%ld]/* empty range */\
 || (rho [%ld] = p_T_q2Tpq (q-1, %ld, q) == 0) %s;\n",
			     varstr_tostr (rvstr), pos, (SXINT) (max_son_nb+2+1+adr2tpos[radr]), (SXINT) -symb, exit_stmt);

		t_indexes_vstr = varstr_catenate (t_indexes_vstr, "rho[");
		t_indexes_vstr = i2varstr (t_indexes_vstr, max_son_nb+2+adr2tpos[radr]);
		t_indexes_vstr = varstr_catenate (t_indexes_vstr, "] = ");
		t_indexes_vstr = varstr_catenate (t_indexes_vstr, varstr_tostr (rvstr));
		t_indexes_vstr = varstr_catenate (t_indexes_vstr, ";");

		if ((Index = adr2Index (pos, radr)) >= 0) {
		  /* Ce n'est pas la borne d'un input range */
		  /* Il se trouve a l'index relatif ds pos : Index */
		  Index += lhs_pos2ladr [pos] - 2*pos -1;
		  fputs ("#if is_generator\n", parser_file);
		  fprintf (parser_file, "Indexes[%ld] = %s;\n",
			   (SXINT) Index, varstr_tostr (rvstr));
		  fputs ("#endif\n", parser_file);
		}

		rvstr = varstr_catenate (rvstr, "-1");
	      }
	    }
	    else {
	      atom = var2atom [symb];

	      if (SXBA_bit_is_set (Asize_set, atom)) {
				/* On connait la taille de cet atome */
		rvstr = varstr_catenate (varstr_catenate (rvstr, "-"), varstr_tostr (atom2rsize [atom]));
		radr -= atom2var_nb [atom];
				/* C'est forcement un atome central dont la taille est calculee */
				/* On l'enleve, (il n'est plus central) */
				/* Attention, il peut deja etre a zero, uniquement ds le cas ou cet atome
				   a recu une taille statique. */

		if ((Index = adr2Index (pos, radr)) >= 0) {
		  /* Ce n'est pas la borne d'un input range */
		  /* Il se trouve a l'index relatif ds pos : Index */
		  Index += lhs_pos2ladr [pos] - 2*pos -1;
		  fputs ("#if is_generator\n", parser_file);
		  fprintf (parser_file, "Indexes[%ld] = %s;\n",
			   (SXINT) Index, varstr_tostr (rvstr));
		  fputs ("#endif\n", parser_file);
		}
	      }
	      else
		break;
	    }
	  }
	}
	else {
	  /* Cette adresse principale a deja ete examinee */
	  if (SXBA_bit_is_set (synchro_set, madr) &&
	      SXBA_bit_is_reset_set (nyp_synchro_test, radr)) {
	    /* C'est un point de synchronisation */
	    /* son source index a deja ete note */
	    /* et sa verification n'a pas ete faite */
	    /* on la fait */
	    /*
	      Les points de synchro sont traites par des equations, la verif est donc
	      toujours faite
	      fprintf (parser_file, "if (%s != %s) %s;\n",
	      varstr_tostr (rvstr), varstr_tostr (synchro2vstr [madr]), exit_stmt);
	    */
	    /* on reaffecte */
	    rvstr = varstr_catenate (varstr_raz (rvstr), varstr_tostr (synchro2vstr [madr]));
	  }

	  if (ladr < radr) {
	    if (symb < 0) {
	      radr--;

	      if (symb < -max_ste) {
		/* constante */
		sprintf (string, "-%ld", -symb - max_ste -1);
		rvstr = varstr_catenate (rvstr, string);
	      }
	      else {
		/* Vrai terminal (pas constante) */
		madr = adr2madr [radr];

		if (SXBA_bit_is_reset_set (tmadr_set, madr)) {
		  is_source_checked = true;

		  if (terminal_file)
		    fprintf (parser_file, "if ((q=%s)==ilb[%ld]/* empty range */\
 || (rho [%ld] = p_T_q2Tpq (q-1, SXT_%s, q) == 0) %s;\n",
			     varstr_tostr (rvstr), pos, (SXINT) (max_son_nb+2+1+adr2tpos[radr]), sxstrget (t2Cid_ste [-symb]), exit_stmt);
		  else
		    fprintf (parser_file, "if ((q=%s)==ilb[%ld]/* empty range */\
 || (rho [%ld] = p_T_q2Tpq (q-1, %ld, q) == 0) %s;\n",
			     varstr_tostr (rvstr), pos, (SXINT) (max_son_nb+2+1+adr2tpos[radr]), (SXINT) -symb, exit_stmt);

		  t_indexes_vstr = varstr_catenate (t_indexes_vstr, "rho[");
		  t_indexes_vstr = i2varstr (t_indexes_vstr, max_son_nb+2+adr2tpos[radr]);
		  t_indexes_vstr = varstr_catenate (t_indexes_vstr, "] = ");
		  t_indexes_vstr = varstr_catenate (t_indexes_vstr, varstr_tostr (rvstr));
		  t_indexes_vstr = varstr_catenate (t_indexes_vstr, ";");
		}

		if ((Index = adr2Index (pos, radr)) >= 0) {
		  /* Ce n'est pas la borne d'un input range */
		  /* Il se trouve a l'index relatif ds pos : Index */
		  Index += lhs_pos2ladr [pos] - 2*pos -1;
		  fputs ("#if is_generator\n", parser_file);
		  fprintf (parser_file, "Indexes[%ld] = %s;\n",
			   (SXINT) Index, varstr_tostr (rvstr));
		  fputs ("#endif\n", parser_file);
		}

		rvstr = varstr_catenate (rvstr, "-1");
	      }
	    }
	    else {
	      atom = var2atom [symb];

	      if (SXBA_bit_is_set (Asize_set, atom)) {
				/* On connait la taille de cet atome */
		rvstr = varstr_catenate (varstr_catenate (rvstr, "-"), varstr_tostr (atom2rsize [atom]));
		radr -= atom2var_nb [atom];
				/* C'est forcement un atome central dont la taille est calculee */
				/* On l'enleve, (il n'est plus central) */
				/* Attention, il peut deja etre a zero, uniquement ds le cas ou cet atome
				   a recu une taille statique. */

		if ((Index = adr2Index (pos, radr)) >= 0) {
		  /* Ce n'est pas la borne d'un input range */
		  /* Il se trouve a l'index relatif ds pos : Index */
		  Index += lhs_pos2ladr [pos] - 2*pos -1;
		  fputs ("#if is_generator\n", parser_file);
		  fprintf (parser_file, "Indexes[%ld] = %s;\n",
			   (SXINT) Index, varstr_tostr (rvstr));
		  fputs ("#endif\n", parser_file);
		}
	      }
	      else
		break;
	    }
	  }
	}
      } while (ladr < radr);

      lhs_pos2rvstr [pos] = rvstr;
      lhs_pos2radr [pos] = radr;
    }
  }

  if (ladr >= radr) {
    lhs_arg_tbp--;
    SXBA_0_bit (lhs_arg_tbp_set, pos);
    return true;
  }

  return false;
}



static void
process_single_atom (char *exit_stmt)
{
  /* xeq est une (pseudo-)equation dans laquelle il reste un unique atome inconnu
     on peut donc calculer sa taille */
  SXINT		xeq, atom, Xnb, min, max, p, tnb, kind, pos, ladr, radr, symb;
  SXINT		*eq_ptr;
  char	*wstr, string [32];
  bool	is_assign, Asize_is_useful;

  sxinitialise(Asize_is_useful); /* pour faire taire gcc -Wuninitialized */

  while (!IS_EMPTY (equation_stack)) {
    /* On est sur que xeq n'a jamais ete examine' */
    xeq = POP (equation_stack);

    sxinitialise (Xnb);
    eq_ptr = equation [xeq];

    for (atom = 1; atom <= max_atom; atom++) {
      if (atom2var_nb [atom] != 0 && (Xnb = eq_ptr [atom]) != 0)
	break;
    }

#if EBUG
    if (atom > max_atom || equation2Xnb [xeq] != 1)
      sxtrap (ME, "process_single_atom");
#endif

    /* Si Asize[atom] est inconnu, on le calcule, sinon on verifie */
    min = atom2min [atom];
    max = atom2max [atom];
    is_assign = SXBA_bit_is_reset_set (Asize_set, atom);

    equation2Xnb [xeq] = 0;
    eq_ptr [atom] = 0;
			
    kind = equation2vstr_kind [xeq];
    /* kind \in {UNKNOWN_k, GEQ0_k, NULL_k, LEQ0_k} */
    tnb = eq_ptr [0];
    wvstr = varstr_raz (wvstr);

    if (Xnb < 0) {
      Xnb = -Xnb;
      wvstr = varstr_catenate (wvstr, "-(");
      wvstr = varstr_catenate (wvstr, varstr_tostr (equation2vstr [xeq]));
      wvstr = varstr_catenate (wvstr, ")");
      tnb = -tnb;

      switch (kind) {
      case UNKNOWN_k:
      case NULL_k:
	break;

      case GEQ0_k:
	kind = LEQ0_k;
	break;

      case LEQ0_k:
	kind = GEQ0_k;
	break;
      default:
#if EBUG
	sxtrap(ME,"unknown switch case #7");
#endif
	break;
      }

    }
    else {
      wvstr = varstr_catenate (wvstr, varstr_tostr (equation2vstr [xeq]));
    }

    if (tnb != 0) {
      /* A FAIRE utiliser min et tnb */
      if (tnb < 0) {
	sprintf (string, "+%ld", (SXINT) -tnb);
      }
      else {
	sprintf (string, "-%ld", (SXINT) tnb);
      }

      switch (kind) {
      case UNKNOWN_k:
	break;

      case GEQ0_k:
	if (tnb > 0)
	  kind = UNKNOWN_k;
	break;

      case NULL_k:
	kind = (tnb < 0) ? GEQ0_k : LEQ0_k; 
	break;

      case LEQ0_k:
	if (tnb < 0)
	  kind = UNKNOWN_k;
	break;
      default:
#if EBUG
	sxtrap(ME,"unknown switch case #8");
#endif
	break;
      }

      wvstr = varstr_catenate (wvstr, string);
    }

    wstr = varstr_tostr (wvstr);

    /* Affectation */
    /* On prepare les futures references */

    if (is_assign) {
      Asize_is_useful = false;

      for (p = 0; p < max_equation; p++) {
	if (p != xeq && equation [p] [atom] != 0) {
	  /* On aura besoin de "Asize[atom]", on l'affecte */
	  Asize_is_useful = true;
	  break;
	}
      }

      if (!Asize_is_useful) {
	/* On parcourt l'interieur des args de la LHS non encore traites */
	pos = -1;

	while (!Asize_is_useful && (pos = sxba_scan (lhs_arg_tbp_set, pos)) >= 0) {
	  ladr = lhs_pos2ladr [pos];
	  radr = lhs_pos2radr [pos];

	  while (ladr < radr) {
	    symb = XH_list_elem (rcg_parameters, adr2cur [ladr]);

	    if (symb > 0) {
	      if (var2atom [symb] == atom) {
		if (pos != xeq || radr - lhs_pos2ladr [pos] > atom2var_nb [atom])
		  /* S'il reste au moins 2 atomes inconnus ds l'equation courante
		     on suppose qu'on aura besoin de Asize */
		  /* On aura besoin de "Asize[atom]", on l'affecte */
		  /* Tout ca est un peu grossier */
		  Asize_is_useful = true;

		break;
	      }

	      ladr += atom2var_nb [atom];
	    }
	    else
	      ladr++;
	  }
	}
      }
    }

    if (Xnb == 1) {
      if (is_assign) {
	if (Asize_is_useful) {
	  fprintf (parser_file, "if ((Asize[%ld] = %s) < %ld) %s;\n",
		   (SXINT) atom-1,
		   wstr,
		   (SXINT) min,
		   exit_stmt);

	  if (max >= 0)
	    fprintf (parser_file, "if (Asize[%ld] > %ld) %s;\n",
		     (SXINT) atom-1,
		     (SXINT) max,
		     exit_stmt);
	}
	else {
	  if (min > 0 || kind == UNKNOWN_k) {
	    fprintf (parser_file, "if (%s < %ld) %s;\n",
		     wstr,
		     (SXINT) min,
		     exit_stmt);

	    if (max >= 0)
	      fprintf (parser_file, "if (%s > %ld) %s;\n",
		       wstr,
		       (SXINT) max,
		       exit_stmt);
	  }
	  else {
	    if (kind == LEQ0_k)
	      fprintf (parser_file, "/* (%s < 0) */ %s;\n",
		       wstr,
		       exit_stmt);
	  }
	}
      }
      else {
	fprintf (parser_file, "if (Asize[%ld] != %s) %s;\n",
		 (SXINT) atom-1,
		 wstr,
		 exit_stmt);
      }
    }
    else {
      if (Xnb == 2) {
	fprintf (parser_file,
		 "if ((local_val = %s) < 0 || (local_val & 1)) %s;\n",
		 wstr,
		 exit_stmt);
					
	if (is_assign) {
	  if (Asize_is_useful) {
	    fprintf (parser_file, "if ((Asize[%ld] = (local_val>>1)) < %ld) %s;\n",
		     (SXINT) atom-1,
		     (SXINT) min,
		     exit_stmt);

	    if (max >= 0)
	      fprintf (parser_file, "if (Asize[%ld] > %ld) %s;\n",
		       (SXINT) atom-1,
		       (SXINT) max,
		       exit_stmt);
	  }
	  else {
	    if (min > 0 || kind == UNKNOWN_k) {
	      fprintf (parser_file, "if ((local_val>>1) < %ld) %s;\n",
		       (SXINT) min,
		       exit_stmt);

	      if (max >= 0)
		fprintf (parser_file, "if ((local_val>>1) > %ld) %s;\n",
			 (SXINT) max,
			 exit_stmt);
	    }
	    else {
	      if (kind == LEQ0_k)
		fprintf (parser_file, "/* ((local_val>>1) < 0) */ %s;\n",
			 exit_stmt);
	    }
	  }
	}
	else {
	  fprintf (parser_file, "if (Asize[%ld] != (local_val>>1)) %s;\n",
		   (SXINT) atom-1,
		   exit_stmt);
	}
      }
      else {
	fprintf (parser_file,
		 "if ((local_val = %s) < 0 || (local_val %% %ld)) %s;\n",
		 wstr,
		 (SXINT) Xnb,
		 exit_stmt);

	if (is_assign) {
	  if (Asize_is_useful) {
	    fprintf (parser_file, "if ((Asize[%ld] = (local_val/%ld)) < %ld) %s;\n",
		     (SXINT) atom-1,
		     (SXINT) Xnb,
		     (SXINT) min,
		     exit_stmt);

	    if (max >= 0)
	      fprintf (parser_file, "if (Asize[%ld] > %ld) %s;\n",
		       (SXINT) atom-1,
		       (SXINT) max,
		       exit_stmt);
	  }
	  else {
	    if (min > 0 || kind == UNKNOWN_k) {
	      fprintf (parser_file, "if ((local_val/%ld) < %ld) %s;\n",
		       (SXINT) Xnb,
		       (SXINT) min,
		       exit_stmt);

	      if (max >= 0)
		fprintf (parser_file, "if ((local_val/%ld) < %ld) %s;\n",
			 (SXINT) Xnb,
			 (SXINT) max,
			 exit_stmt);
	    }
	    else {
	      if (kind == LEQ0_k)
		fprintf (parser_file, "/* ((local_val/%ld) < 0) */ %s;\n",
			 (SXINT) Xnb,
			 exit_stmt);
	    }
	  }
	}
	else {
	  fprintf (parser_file, "if (Asize[%ld] != (local_val/%ld)) %s;\n",
		   (SXINT) atom-1,
		   (SXINT) Xnb,
		   exit_stmt);
	}
      }
    }

    /* C'est une nouvelle Asize */
    varstr_raz (atom2rsize [atom]);

    if (Asize_is_useful) {
      sprintf (string, "Asize[%ld]", (SXINT) atom-1);
      atom2rsize [atom] = varstr_catenate (atom2rsize [atom], string);
    }
    else {
      /* Le 10/04/08 */
      atom2rsize [atom] = varstr_catenate (atom2rsize [atom], wstr);
    }

    equation2vstr [xeq] = varstr_raz (equation2vstr [xeq]); /* Toutes les tailles sont connues */

    if (is_assign) {
      PUSH (atom_stack, atom);
      set_Asize_set (exit_stmt);
    }
  }
}
   

static void
gen_level (SXINT level, SXINT clause, char *exit_stmt)
{
  SXINT		pos, xeq, mpos, atom, Xnb, xeq2, ladr, radr, lmadr, rmadr, symb, max;
  SXINT		*eq_ptr, *eq2_ptr;
  char	string [28];

  while (!IS_EMPTY (equation_stack)) {
    while (!IS_EMPTY (equation_stack))
      process_single_atom (exit_stmt);

    for (Xnb = 2; Xnb <= max_atom; Xnb++) {
      xeq = -1;

      while ((xeq = sxba_scan (nyp_eq_set, xeq)) >= 0) {
	if (Xnb == equation2Xnb [xeq]) {
	  /* Cette equation a Xnb variables inconnues */
	  /* On recherche les autres equations qui ont au moins ces Xnb inconnues =>
	     par substitution, le nombre des leurs inconnues va diminuer de 1 */
	  eq_ptr = equation [xeq];
	  equation_stack2 [0] = 0;

	  xeq2 = -1;

	  while ((xeq2 = sxba_scan (nyp_eq_set, xeq2)) >= 0) {
	    if (xeq2 != xeq && !SXBA_bit_is_set (equation_already_substituted_set, xeq2)) {
	      if (Xnb <= equation2Xnb [xeq2]) {
		eq2_ptr = equation [xeq2];

		for (atom = 1; atom <= max_atom; atom++) {
		  if (atom2var_nb [atom] != 0 && eq_ptr [atom] != 0 && eq2_ptr [atom] == 0)
		    break;
		}

		if (atom > max_atom)
		  PUSH (equation_stack2, xeq2);
	      }
	    }
	  }

	  if (equation_stack2 [0] >= 1) {
	    PUSH (equation_stack2, xeq);
	    substitute_atom_in_eq (equation_stack2, exit_stmt);

	    if (!IS_EMPTY (equation_stack))
	      break;
	  }
	}
      }

      if (xeq >= 0)
	break;
    }
  }

  /* On exploite ce qui vient d'etre calcule */
  pos = -1;

  while ((pos = sxba_scan (lhs_arg_tbp_set, pos)) >= 0) {
    lhs_arg_processing (clause, pos, exit_stmt, false);
  }

  /* Les args de certains predicats de la RHS peuvent s'evaluer, meme si tout n'est pas connu */
  rhs_processing (level, clause, exit_stmt);

  if (lhs_arg_tbp > 0) {
    /* On genere une boucle for */
    mpos = sxba_scan (lhs_arg_tbp_set, -1);

    /* On prend le 1er atome inconnu du 1er arg non completement traite */
    /* Attention, a` cause des substitutions, atom peut ne pas etre un element inconnu de equation [mpos] */
    ladr = lhs_pos2ladr [mpos];
    radr = lhs_pos2radr [mpos];
    lmadr = adr2madr [ladr];
    rmadr = adr2madr [radr];

    symb = XH_list_elem (rcg_parameters, adr2cur [ladr]);
    atom = var2atom [symb];

    /* On cherche les autres arg ds lequel intervient atom */
    pos = mpos;

    while ((pos = sxba_scan (lhs_arg_tbp_set, pos)) >= 0) {
      if (is_atom_in_arg (atom, pos)) {
	/* Si les bornes sont identiques, on ne fait rien ?? */
	if (adr2madr [lhs_pos2ladr [pos]] != lmadr || adr2madr [lhs_pos2radr [pos]] != rmadr)
	  PUSH (equation_stack, pos);
      }
    }

    sprintf (string, "Asize[%ld]", (SXINT) atom-1);

    /* attention, on peut connaitre une somme sans les connaitre individuellement (A FAIRE) */
    wvstr = unknown_arg_size (mpos);
    max = atom2max [atom];

    if (IS_EMPTY (equation_stack)) {
      if (max == -1) {
	if (decrease_order)
	  fprintf (parser_file, "for (Asize[%ld] = %s; Asize[%ld] >= %ld%s; Asize[%ld]--) {\n",
		   (SXINT) atom-1, varstr_tostr (wvstr),
		   (SXINT) atom-1, (SXINT) atom2min [atom], clause_has_cut ? " && !cut" : "",
		   (SXINT) atom-1);
	else {
	  fprintf (parser_file, "for (top [%ld] = %s, Asize[%ld] = %ld; Asize[%ld] <= top [%ld]%s; Asize[%ld]++) {\n",
		   (SXINT) atom-1, varstr_tostr (wvstr), (SXINT) atom-1, (SXINT) atom2min [atom],
		   (SXINT) atom-1, (SXINT) atom-1, clause_has_cut ? " && !cut" : "",
		   (SXINT) atom-1);
	}
      }
      else {
	/* Si min==max, len est positionne et on n'est pas ici ... */
	fprintf (parser_file, "if ((min_max = %s) > %ld) min_max = %ld;\n",
		 varstr_tostr (wvstr),
		 (SXINT) max,
		 (SXINT) max);

	if (decrease_order)
	  fprintf (parser_file, "for (Asize[%ld] = min_max; Asize[%ld] >= %ld%s; Asize[%ld]--) {\n",
		   (SXINT) atom-1,
		   (SXINT) atom-1, (SXINT) atom2min [atom], clause_has_cut ? " && !cut" : "",
		   (SXINT) atom-1);
	else {
	  fprintf (parser_file, "for (top [%ld] = min_max, Asize[%ld] = %ld; Asize[%ld] <= top [%ld]%s; Asize[%ld]++) {\n",
		   (SXINT) atom-1, (SXINT) atom-1, (SXINT) atom2min [atom],
		   (SXINT) atom-1, (SXINT) atom-1, clause_has_cut ? " && !cut" : "",
		   (SXINT) atom-1);
	}
      }
    }
    else {
      if (max == -1)
	fprintf (parser_file, "min_max = %s;\n", varstr_tostr (wvstr));
      else
	fprintf (parser_file, "if ((min_max = %s) > %ld) min_max = %ld;\n",
		 varstr_tostr (wvstr),
		 (SXINT) max,
		 (SXINT) max);

      do {
	pos = POP (equation_stack);
	wvstr = unknown_arg_size (pos);

	fprintf (parser_file, "if ((local_val = %s) < min_max) min_max = local_val;\n",
		 varstr_tostr (wvstr));

      } while (!IS_EMPTY (equation_stack));

      if (decrease_order)
	fprintf (parser_file, "for (Asize[%ld] = min_max; Asize[%ld] >= %ld%s; Asize[%ld]--) {\n",
		 (SXINT) atom-1,
		 (SXINT) atom-1, (SXINT) atom2min [atom], clause_has_cut ? " && !cut" : "",
		 (SXINT) atom-1);
      else {
	fprintf (parser_file, "for (top [%ld] = min_max, Asize[%ld] = %ld; Asize[%ld] <= top [%ld]%s; Asize[%ld]++) {\n",
		 (SXINT) atom-1, (SXINT) atom-1, (SXINT) atom2min [atom],
		 (SXINT) atom-1, (SXINT) atom-1, clause_has_cut ? " && !cut" : "",
		 (SXINT) atom-1);
      }
    }

    for_loops_nb [var2lhs_pos [atom]]++; /* Le Lun  4 Dec 2000 15:06:38 */

    atom2rsize [atom] = varstr_catenate (varstr_raz (atom2rsize [atom]), string);

    if (SXBA_bit_is_reset_set (Asize_set, atom)) {
      PUSH (atom_stack, atom);
      set_Asize_set ("continue" /* et pas exit_stmt le 07/05/04 */);
    }

    gen_level (level+1, clause, "continue");

    fputs ("}\n", parser_file);

    if (level > 1) {
      fputs ("#if is_recognizer\n\
if (ret_val > 0) break;\n\
#endif\n", parser_file);
#if 0
      fprintf (parser_file, "#if is_recognizer\n\
if (%sret_val > 0) break;\n\
#endif\n",
	       A_clause_nb > 1 ? "local_" : "");
#endif /* 0 */
    }

    /* Ici, on retourne de gen_level */
  }
}

/* UTILISE' si 1-RCG */
static SXINT		size;
static SXINT		*lhs_arg2pos;
static SXINT		*pos2symb, *pos2lhs_arg, *pos2bound, **posxpos2lgth, *posxpos_stack, *pos2main;
static SXBA		*pos2pos_set, var_set, pos_hd_set, k_guide_set;


static SXINT
pos2lgth (SXINT pos1, SXINT pos2)
{
    /* calcule, si elle est connue, la "longueur" entre pos1 et pos2 */
    SXINT symb, dist, pos3, *p, lgth;

    if (pos1 == pos2)
	return 0;

    symb = pos2symb [pos1];

    if (symb < 0 && (dist = pos2lgth (pos1+1, pos2)) >= 0)
	return 1+dist;

    if (!S1_is_empty (posxpos_stack)) {
	p = posxpos2lgth [pos1];

	for (pos3 = pos1+1; pos3 <= pos2; pos3++) {
	    if ((lgth = p [pos3]) >= 0 && (dist = pos2lgth (pos3, pos2)) >= 0) {
		return lgth+dist;
	    }
	}
    }

    return -1;
}

static bool
are_pos1_pos2_equiv (SXINT pos1, SXINT pos2)
{
    SXINT symb1, symb2, cur_pos1, cur_pos2;

    symb1 = pos2symb [pos1];
    symb2 = pos2symb [pos2];

    cur_pos1 = pos1;
    cur_pos2 = pos2;

    while (symb1 == symb2) {
	if (symb1 > 0)
	    return true;

	if (symb1 == 0)
	    break;

	symb1 = pos2symb [++cur_pos1];
	symb2 = pos2symb [++cur_pos2];
    }

    cur_pos1 = pos1;
    cur_pos2 = pos2;
    symb1 = pos2symb [--cur_pos1];
    symb2 = pos2symb [--cur_pos2];

    while (symb1 == symb2) {
	if (symb1 > 0)
	    return true;

	if (symb1 == 0)
	    break;

	symb1 = pos2symb [--cur_pos1];
	symb2 = pos2symb [--cur_pos2];
    }

    return false;
    
}



static bool
left_linear (SXINT clause)
{
    SXINT		lhs_prdct, left_linear_lhs_bot2, left_linear_lhs_top2, lhs_arg, pos, cur2, param, cur3, bot3, top3, symb;
    SXINT		ipos, opos;
    SXINT		rhs, bot, top, cur, prdct, bot2, top2, rhs_pos, rhs_param_pos, k, nt;
    SXINT		Ak, len;
    SXINT		*p, *b1, *d1, *t1;
    SXINT		lhs_arg1, lhs_arg2, top1, pos1, pos2, main_pos, main_pos1, main_pos2;
    bool	ret_val;
    SXBA	pos_set;

    lhs_prdct = XxY_X (rcg_clauses, clause) & lhs_prdct_filter;
    left_linear_lhs_bot2 = XH_X (rcg_predicates, lhs_prdct);
    lhs_arity = nt2arity [XH_list_elem (rcg_predicates, left_linear_lhs_bot2+1)];

    if (lhs_arity <= 1) return true;

    left_linear_lhs_top2 = XH_X (rcg_predicates, lhs_prdct+1);
    ret_val = false;
    pos = 0;
    pos2symb [pos] = 0;
    pos2bound [pos] = 0;

    for (lhs_arg = 0, cur2 = left_linear_lhs_bot2+2; cur2 < left_linear_lhs_top2; lhs_arg++, cur2++) {
	lhs_arg2pos [lhs_arg] = pos+1;
	param = XH_list_elem (rcg_predicates, cur2);

	if (param > 0) {
	    bot3 = XH_X (rcg_parameters, param);
	    top3 = XH_X (rcg_parameters, param+1);
	    pos++;

	    for (cur3 = bot3; cur3 < top3; cur3++) {
		symb = XH_list_elem (rcg_parameters, cur3);
		pos2symb [pos] = symb;
		pos2bound [pos] = 0;
		pos2lhs_arg [pos] = lhs_arg;

		if (symb > 0 && !SXBA_bit_is_reset_set (var_set, symb)) {
		    /* Plusieurs occur de la meme variable en LHS */
		    ret_val = true;
		}

		pos++;
	    }

	    pos2symb [pos] = 0;
	    pos2bound [pos] = 0;
	    pos2lhs_arg [pos] = lhs_arg;
	}
    }

    sxba_empty (var_set);

    if (!ret_val)
	return true;

    lhs_arg2pos [lhs_arg] /* = last_lhs_pos */ = pos+1;

    /* On remplit pos2pos_set, qui donne les equivalences entre les pos */
    /* ATTENTION: toutes les positions ne permettent pas toutes de detecter toutes les equivalences */
    for (lhs_arg1 = 0; lhs_arg1 < lhs_arity-1; lhs_arg1++) {
	top1 = lhs_arg2pos [lhs_arg1+1];

	for (lhs_arg2 = lhs_arg1+1; lhs_arg2 < lhs_arity; lhs_arg2++) {
	    top2 = lhs_arg2pos [lhs_arg2+1];

	    for (pos1 = lhs_arg2pos [lhs_arg1]; pos1 < top1; pos1++) {
		for (pos2 = lhs_arg2pos [lhs_arg2]; pos2 < top2; pos2++) {
		    if (are_pos1_pos2_equiv (pos1, pos2)) {
			main_pos1 = pos2main [pos1];
			main_pos2 = pos2main [pos2];

			if (main_pos1 == 0 && main_pos2 == 0) {
			    main_pos = pos1;
			    pos2main [pos2] = pos2main [pos1] = main_pos;
			    SXBA_1_bit (pos2pos_set [main_pos], pos1);
			    SXBA_1_bit (pos2pos_set [main_pos], pos2);
			    SXBA_1_bit (pos_hd_set, main_pos);
			}
			else {
			    if (main_pos1 == 0) {
				main_pos = main_pos2;
				pos2main [pos1] = main_pos;
				SXBA_1_bit (pos2pos_set [main_pos], pos1);
			    }
			    else {
				if (main_pos2 == 0) {
				    main_pos = main_pos1;
				    pos2main [pos2] = main_pos;
				    SXBA_1_bit (pos2pos_set [main_pos], pos2);
				}
				else {
				    if (main_pos1 != main_pos2) {
					if (main_pos1 < main_pos2) {
					    main_pos = main_pos1;
					    pos_set = pos2pos_set [main_pos2];
					    pos = main_pos2;
					}
					else {
					    main_pos = main_pos2;
					    pos_set = pos2pos_set [main_pos1];
					    pos = main_pos1;
					}

					sxba_or (pos2pos_set [main_pos], pos_set);
					SXBA_0_bit (pos_hd_set, pos);

					do {
					    pos2main [pos] = main_pos;
					} while ((pos = sxba_scan_reset (pos_set, pos)) > 0);
				    }
				}
			    }
			}
			
			break;
		    }
		}
	    }
	}
    }

    /* On calcule le graphe des longueurs connues (&StrLen et Ak2len) */
    rhs = XxY_Y (rcg_clauses, clause);
    top = XH_X (rcg_rhs, rhs+1);
    bot = XH_X (rcg_rhs, rhs);

    rhs_pos = 0;
    rhs_param_pos = lhs_arity;
    p = rhs_pos2disp+clause2rhs_pos [clause];
    
    for (cur = bot; cur < top; cur++) {
	prdct = XH_list_elem (rcg_rhs, cur);
	bot2 = XH_X (rcg_predicates, prdct);
	top2 = XH_X (rcg_predicates, prdct+1);

	for (k = 0, cur2 = bot2+2; cur2 < top2; k++, cur2++) {
	    if (SXBA_bit_is_set (pos_is_a_range, rhs_param_pos)) {
		param = XH_list_elem (rcg_predicates, cur2);
		bot3 = XH_X (rcg_parameters, param);
		top3 = XH_X (rcg_parameters, param+1);

		if (XH_list_elem (rcg_predicates, bot2) == 0) {
		    /* appel positif */
		    nt = XH_list_elem (rcg_predicates, bot2+1);

		    if (nt > 0 || nt == STRLEN_ic) {
			if (nt == STRLEN_ic) {
			    len = XH_list_elem (rcg_parameters,
						XH_X (rcg_parameters, XH_list_elem (rcg_predicates, bot2+2)));
			}
			else {
			    Ak = A_k2Ak (nt, k);
			    len = Ak2len [Ak];
			}

			if (len >= 0) {
			    b1 = rhs_pos2lhs_arg_no + p [rhs_pos];
			    d1 = rhs_pos2lhs_pos + p [rhs_pos];
			    t1 = rhs_pos2lhs_arg_no + p [rhs_pos + 1];

			    do {
				ipos = lhs_arg2pos [*b1] + *d1;
				opos = ipos+top3-bot3;

				posxpos2lgth [ipos][opos] = len;
				S1_push (posxpos_stack, ipos);
				S1_push (posxpos_stack, opos);
				d1++, b1++;
			    } while (b1 < t1);
			}
		    }
		}

		rhs_pos++, rhs_param_pos++;
	    }
	}
    }

    return false;
}




static void
bound2str (char *pstr, SXINT lb)
{
    bool is_l, is_i;

    if (lb < 0) {
	lb = -lb;
	is_l = true;
    }
    else {
	is_l = false;
    }

    if (lb <= lhs_arity) {
	lb--;
	is_i = true;
    }
    else {
	lb -= (lhs_arity+1);
	is_i = false;
    }

    sprintf (pstr, "%s%sb[%ld]", is_i ? "i" : "o", is_l ? "l" : "u", (SXINT) lb);
}

static void
propagate_equiv_pos (SXINT pos1, SXINT pos2)
{
    /* pos1 et pos2 sont equivalents, on en tire les consequences */
    SXINT	main_pos;

    if ((main_pos = pos2main [pos1]) > 0 &&
	pos2main [pos2] == main_pos &&
	SXBA_bit_is_set_reset (pos2pos_set [main_pos], pos2)) {

	if (pos2symb [pos1] < 0 && pos2symb [pos2] < 0) {
	    propagate_equiv_pos (pos1+1, pos2+1);
	}

	if (pos2symb [pos1-1] < 0 && pos2symb [pos2-1] < 0) {
	    propagate_equiv_pos (pos1-1, pos2-1);
	}
    }
}

static bool
gen_pos1_pos2 (SXINT pos1, SXINT pos2)
{
    SXINT lhs_arg1, bot1, top1, llgth1, rlgth1, llgth2, rlgth2, lhs_arg2, bot2, top2;

    lhs_arg1 = pos2lhs_arg [pos1];
    bot1 = lhs_arg2pos [lhs_arg1];
    top1 = lhs_arg2pos [lhs_arg1+1]-1;
    llgth1 = pos2lgth (bot1, pos1);
    rlgth1 = pos2lgth (pos1, top1);

    if ((llgth1 >= 0) || (rlgth1 >= 0)) {
	lhs_arg2 = pos2lhs_arg [pos2];
	bot2 = lhs_arg2pos [lhs_arg2];
	top2 = lhs_arg2pos [lhs_arg2+1]-1;
	llgth2 = pos2lgth (bot2, pos2);
	rlgth2 = pos2lgth (pos2, top2);

	if (((llgth2 >= 0) || (rlgth2 >= 0))) {
	    if (llgth1 >= 0)
		fprintf (parser_file,
			 "if (ilb[%ld]+%ld!=", (SXINT) lhs_arg1, (SXINT) llgth1);
	    else
		fprintf (parser_file,
			 "if (iub[%ld]-%ld!=", (SXINT) lhs_arg1, (SXINT) rlgth1);

	    if (llgth2 >= 0)
		fprintf (parser_file,
			 "ilb[%ld]+%ld) continue;\n", (SXINT) lhs_arg2, (SXINT) llgth2);
	    else
		fprintf (parser_file,
			 "iub[%ld]-%ld) continue;\n", (SXINT) lhs_arg2, (SXINT) rlgth2);

	    /* cette verif est effectuee */
	    propagate_equiv_pos (pos1, pos2);
	    return true;
	}
    }

    return false;
}


static void
gen_guided_bounds (SXINT clause)
{
    /* On commence par tester les predicats predefinis dont les args sont des sous-chaines
       d'args differents de la LHS */
    /* On exploite les Ak2len, Ak2min et nt2IeqJ */
    SXINT 	rhs, top, bot, cur, rhs_param_pos, prdct, bot2, nt, rhs_pos, param, bot3, top3, arity, Ak, k, len, xbot, xtop;
    SXINT 	IJ, I, J, rhs_prdct_pos, tooth, generator_pos, top2, cur2;
    SXINT		*p, *b1, *t1, *c1, *c2, *b2, *t2, *d1;
    SXINT		lhs_arg1, pos, top_pos, next_pos, lhs_arg2;
    SXINT		ipos, main_ipos, opos, main_opos;
    SXINT		cur_pos, bot_pos, bot1_tnb, top1_tnb, bot2_tnb, top2_tnb, lb;
    char	bot1_str [32], top1_str [32], bot2_str [32], top2_str [32];
    bool 	gen_guided_bounds_is_left_linear, is_i, is_o, is_pos2bound, is_next_pos2bound;
    SXBA	pos_set, ipos_set, opos_set;
    SXINT		ipos2, opos2;

    gen_guided_bounds_is_left_linear = left_linear (clause);

    if (!gen_guided_bounds_is_left_linear) {
	/* On commence les verifs sur les i[lu]b (si possible), avant l'appel de rhs_guide qui va positionner les o[lu]b */
	for (lhs_arg1 = 0; lhs_arg1 < lhs_arity; lhs_arg1++) {
	    pos = lhs_arg2pos [lhs_arg1];
	    pos2bound [pos] = -lhs_arg1-1;
	    top_pos = lhs_arg2pos [lhs_arg1+1]-1;
	    pos2bound [top_pos] = lhs_arg1+1;
	}

	pos = 0;

	while ((pos = sxba_scan (pos_hd_set, pos)) > 0) {
	    pos_set = pos2pos_set [pos];
	    next_pos = pos;

	    while ((next_pos = sxba_scan (pos_set, next_pos)) > 0) {
		gen_pos1_pos2 (pos, next_pos);
	    }
	}
    }

#if 0
    if (Arhs_arg_nb)
	fprintf (parser_file, "rhs_guide (%ld, &local_guide, guide_map_%ld, olb, oub);\n",
		 clause2rhs_pos [clause+1]-clause2rhs_pos [clause], clause);
    /* Ici, olb et oub sont remplis */
#endif

    rhs = XxY_Y (rcg_clauses, clause);
    top = XH_X (rcg_rhs, rhs+1);
    bot = XH_X (rcg_rhs, rhs);

    p = rhs_pos2disp+clause2rhs_pos [clause];

    rhs_param_pos = lhs_arity;

    for (rhs_prdct_pos = 1, cur = bot; cur < top; rhs_prdct_pos++, cur++) {
	prdct = XH_list_elem (rcg_rhs, cur);
	bot2 = XH_X (rcg_predicates, prdct);
	top2 = XH_X (rcg_predicates, prdct+1);
	nt = XH_list_elem (rcg_predicates, bot2+1);

	if (!gen_guided_bounds_is_left_linear) {
	    rhs_pos = rhs_param_pos - lhs_arity;

	    for (cur2 = bot2+2; cur2 < top2; rhs_pos++, cur2++) {
		if (SXBA_bit_is_set (pos_is_a_range, rhs_pos+lhs_arity)) {
		    param = XH_list_elem (rcg_predicates, cur2);
		    bot3 = XH_X (rcg_parameters, param);
		    top3 = XH_X (rcg_parameters, param+1);

		    b1 = rhs_pos2lhs_arg_no + p [rhs_pos];
		    d1 = rhs_pos2lhs_pos + p [rhs_pos];
		    t1 = rhs_pos2lhs_arg_no + p [rhs_pos + 1];

		    ipos = lhs_arg2pos [*b1] + *d1;
		    opos = ipos+top3-bot3;

		    if (pos2bound [ipos] == 0)
			pos2bound [ipos] = -(rhs_pos+lhs_arity+1);

		    if (pos2bound [opos] == 0)
			pos2bound [opos] = (rhs_pos+lhs_arity+1);

		    if (b1 < t1-1) {
			/* L'arg en position rhs_pos a plusieurs occurrences en LHS */
			/* Il fait donc parti de plusieurs 1-rcg clauses */
			/* Il doit designer le meme intervalle ds ces differentes clauses */
			/* On genere les tests correspondant */
			/* Attention, la verif a deja pu etre faite */

			/* L'arg rhs_pos occurre ds le LHS arg *b1 en pos pos1 */
			main_ipos = pos2main [ipos];
			ipos_set = pos2pos_set [main_ipos];
			main_opos = pos2main [opos];
			opos_set = pos2pos_set [main_opos];

			for (c2 = rhs_pos2rhs_pos_1rcg + p [rhs_pos] + 1, c1 = b1+1; c1 < t1; c2++, c1++) {
			    d1++;
			    ipos2 = lhs_arg2pos [*c1] + *d1;

			    if (pos2bound [ipos2] == 0)
				pos2bound [ipos2] = -(rhs_pos+lhs_arity+1);

			    if ((is_i = SXBA_bit_is_set (ipos_set, ipos2)))
				propagate_equiv_pos (ipos, ipos2);

			    opos2 = ipos2 + opos-ipos;

			    if (pos2bound [opos2] == 0)
				pos2bound [opos2] = (rhs_pos+lhs_arity+1);

			    if ((is_o = SXBA_bit_is_set (opos_set, opos2)))
				propagate_equiv_pos (opos, opos2);
			    
			    if (is_i + is_o == 1) {
				/* Si == 2, deja teste */
				fprintf (parser_file,
					 "range = pnode%ld->rho[%ld];\n",
					 (SXINT) *c1, (SXINT) *c2);

				if (is_i) {
				    fprintf (parser_file,
					     "if (olb[%ld]!=RANGE2i(range)) continue;\n",
					     (SXINT) rhs_pos);
				}
				else {
				    fprintf (parser_file,
					     "if (oub[%ld]!=RANGE2j(range)) continue;\n",
					     (SXINT) rhs_pos);
				}
			    }
			}
		    }
		}
	    }
	}

	rhs_pos = rhs_param_pos - lhs_arity;

	if (nt < 0 && nt != CUT_ic) {
	    if ((nt == STREQ_ic || nt == STREQLEN_ic) &&
		SXBA_bit_is_set (pos_is_a_range, rhs_param_pos) &&
		SXBA_bit_is_set (pos_is_a_range, rhs_param_pos+1)) {
		b1 = rhs_pos2lhs_arg_no + p [rhs_pos];
		t1 = rhs_pos2lhs_arg_no + p [rhs_pos + 1];
		b2 = t1;
		t2 = rhs_pos2lhs_arg_no + p [rhs_pos + 2];

		while (b1 < t1 && b2 < t2) {
		    if (*b1 == *b2)
			/* Il suffit que l'egalite soit verifiee ds une seule 1-rcg clause */
			break;

		    if (*b1 < *b2)
			b1++;
		    else
			b2++;
		}

		if (b1 == t1 || b2 == t2) {
		    /* Ce n'est pas le cas, test dynamique */
		    if (XH_list_elem (rcg_predicates, bot2) == 0) {
			/* Appel positif */
			fprintf (parser_file,
				 "if (oub[%ld]-olb[%ld]!=oub[%ld]-olb[%ld]) continue;\n",
				 (SXINT) rhs_pos, (SXINT) rhs_pos,
				 (SXINT) rhs_pos+1, (SXINT) rhs_pos+1);

			if (nt == STREQ_ic) {
#if 0
			    fprintf (parser_file,
				     "if (_StrEq (&(rho[%ld]), &(olb[%ld]), &(oub[%ld]))==-1) continue;\n",
				     rhs_prdct_pos2tooth [rhs_prdct_pos]+1, rhs_pos, rhs_pos);
#endif
			    fprintf (parser_file,
				     "if (_StrEq (&(olb[%ld]), &(oub[%ld]))==-1) continue;\n",
				     (SXINT) rhs_pos, (SXINT) rhs_pos);
			}
		    }
		    else {
			/* Appel negatif */
			if (nt == STREQ_ic) {
#if 0
			    fprintf (parser_file,
				     "if (oub[%ld]-olb[%ld]==oub[%ld]-olb[%ld] && _StrEq (&(rho[%ld]), &(olb[%ld]), &(oub[%ld]))==1) continue;\n",
				     rhs_pos, rhs_pos,
				     rhs_pos+1, rhs_pos+1,
				     rhs_prdct_pos2tooth [rhs_prdct_pos]+1, 
				     rhs_pos, rhs_pos);
#endif
			    fprintf (parser_file,
				     "if (oub[%ld]-olb[%ld]==oub[%ld]-olb[%ld] && _StrEq (&(olb[%ld]), &(oub[%ld]))==1) continue;\n",
				     (SXINT) rhs_pos, (SXINT) rhs_pos,
				     (SXINT) rhs_pos+1, (SXINT) rhs_pos+1, 
				     (SXINT) rhs_pos, (SXINT) rhs_pos);
			}
			else {
			    fprintf (parser_file,
				     "if (oub[%ld]-olb[%ld]==oub[%ld]-olb[%ld]) continue;\n",
				     (SXINT) rhs_pos, (SXINT) rhs_pos,
				     (SXINT) rhs_pos+1, (SXINT) rhs_pos+1);
			}
		    }
		}
	    }

	    rhs_param_pos += top2-bot2-2;
	}
	else {
	    if (nt < 0)
		rhs_param_pos += top2-bot2-2;
	    else {
		arity = nt2arity [nt];

		if (XH_list_elem (rcg_predicates, bot2) == 0) {
		    /* Appel positif */
		    for (Ak = A_k2Ak (nt, 0), k = 0; k < arity; Ak++, k++) {
			if ((len = Ak2len [Ak]) >= 0) {
			    /* On a deja tenu compte de Ak2len a la generation de la 1-rcg => on ne fait rien
			       
			       fprintf (parser_file,
			       "if (oub[%ld]-olb[%ld]!=%ld) continue;\n",
			       rhs_pos+k, rhs_pos+k,
			       len);
			       */
			}
			else {
			    /* Priorite a la longueur */
			    if ((len = Ak2min [Ak]) > 0) {
				/* On n'a pas tenu compte de Ak2min a la generation de la 1-rcg (il n'y a pas
				   de predicat predefini &StrLess).  Il est peut etre possible que len ait une
				   valeur + significative ds la rcg complete que ds la 1-rcg ?? */
				fprintf (parser_file,
					 "if (oub[%ld]-olb[%ld]<%ld) continue;\n",
					 (SXINT) rhs_pos+k, (SXINT) rhs_pos+k,
					 (SXINT) len);
			    }

			    if ((len = Ak2max [Ak]) >= 0) {
				fprintf (parser_file,
					 "if (oub[%ld]-olb[%ld]>%ld) continue;\n",
					 (SXINT) rhs_pos+k, (SXINT) rhs_pos+k,
					 (SXINT) len);
			    }
			}
		    }

		    if (arity > 1 && (xbot = nt2IeqJ_disp [nt]) < (xtop = nt2IeqJ_disp [nt+1])) {
			/* On s'occupe des egalites sur les arguments */
			do {
			    IJ = nt2IeqJ [xbot];
			    J = IJ % max_garity;
			    I = IJ / max_garity;

			    /* Bien sur, on ne genere qqchose que si le test n'a pas deja ete produit
			       ds la 1-rcg */
			    b1 = rhs_pos2lhs_arg_no + p [rhs_pos+I];
			    t1 = rhs_pos2lhs_arg_no + p [rhs_pos+I+1];
			    b2 = rhs_pos2lhs_arg_no + p [rhs_pos+J];
			    t2 = rhs_pos2lhs_arg_no + p [rhs_pos+J+1];

			    while (b1 < t1 && b2 < t2) {
				if (*b1 == *b2)
				    /* Il suffit que l'egalite soit verifiee ds une seule 1-rcg clause */
				    break;

				if (*b1 < *b2)
				    b1++;
				else
				    b2++;
			    }

			    if (b1 == t1 || b2 == t2) {
				fprintf (parser_file,
					 "if (oub[%ld]-olb[%ld]!=oub[%ld]-olb[%ld]) continue;\n",
					 (SXINT) rhs_pos+I, (SXINT) rhs_pos+I,
					 (SXINT) rhs_pos+J, (SXINT) rhs_pos+J);
			    }
			} while (++xbot < xtop);
		    }
		}

		rhs_param_pos += arity;
	    }
	}
    }

    if (!gen_guided_bounds_is_left_linear) {
	/* On traite les equivalences restantes */
	/* NON OPTIMAL, on peut generer plusieurs fois le meme test */
	pos = 0;

	while ((pos = sxba_scan (pos_hd_set, pos)) > 0) {
	    pos2main [pos] = 0;
	    pos_set = pos2pos_set [pos];
	    next_pos = sxba_scan (pos_set, pos);

	    if (next_pos > 0) {
		bot1_tnb = top1_tnb = 0;

		/* On cherche les positions les + proches de pos dont on connait les bornes */
		if ((is_pos2bound = ((lb = pos2bound [pos]) != 0))) {
		    bound2str (bot1_str, lb);
		}
		else {
		    lhs_arg1 = pos2lhs_arg [pos];
		    bot_pos = lhs_arg2pos [lhs_arg1];
		    top_pos = lhs_arg2pos [lhs_arg1+1];

		    for (cur_pos = pos-1; bot_pos <= cur_pos; cur_pos--) {
			if (pos2symb [cur_pos] < 0)
			    bot1_tnb++;

			if ((lb = pos2bound [cur_pos]) != 0) {
			    bound2str (bot1_str, lb);
			    break;
			}
		    }

		    for (cur_pos = pos+1; cur_pos < top_pos; cur_pos++) {
			if (pos2symb [cur_pos] < 0)
			    top1_tnb++;

			if ((lb = pos2bound [cur_pos]) != 0) {
			    bound2str (top1_str, lb);
			    break;
			}
		    }
		}

		do {
		    bot2_tnb = top2_tnb = 0;

		    if ((is_next_pos2bound = ((lb = pos2bound [next_pos]) != 0))) {
			bound2str (bot2_str, lb);
		    }
		    else {
			lhs_arg2 = pos2lhs_arg [next_pos];
			bot_pos = lhs_arg2pos [lhs_arg2];
			top_pos = lhs_arg2pos [lhs_arg2+1];

			for (cur_pos = next_pos-1; bot_pos <= cur_pos; cur_pos--) {
			    if (pos2symb [cur_pos] < 0)
				bot2_tnb++;

			    if ((lb = pos2bound [cur_pos]) != 0) {
				bound2str (bot2_str, lb);
				break;
			    }
			}

			for (cur_pos = next_pos+1; cur_pos < top_pos; cur_pos++) {
			    if (pos2symb [cur_pos] < 0)
				top2_tnb++;

			    if ((lb = pos2bound [cur_pos]) != 0) {
				bound2str (top2_str, lb);
				break;
			    }
			}
		    }

		    if (is_pos2bound) {
			if (is_next_pos2bound) {
			    fprintf (parser_file,
				     "if (%s != %s) continue;\n",
				     bot1_str, bot2_str);
			}
			else {
			    fprintf (parser_file,
				     "if (%s > %s-%ld || %s+%ld > %s) continue;\n",
				     bot1_str, top2_str, (SXINT) top2_tnb,
				     bot2_str, (SXINT) bot2_tnb, bot1_str);
			}
		    }
		    else {
			if (is_next_pos2bound) {
			    fprintf (parser_file,
				     "if (%s+%ld > %s || %s > %s-%ld) continue;\n",
				     bot1_str, (SXINT) bot1_tnb, bot2_str,
				     bot2_str, top1_str, (SXINT) top1_tnb);
			}
			else {
			    fprintf (parser_file,
				     "if (%s+%ld > %s-%ld || %s+%ld > %s-%ld) continue;\n",
				     bot1_str, (SXINT) bot1_tnb, top2_str, (SXINT) top2_tnb,
				     bot2_str, (SXINT) bot2_tnb, top1_str, (SXINT) top1_tnb);
			}
		    }

		    propagate_equiv_pos (pos, next_pos);
		} while ((next_pos = sxba_scan (pos_set, next_pos)) > 0);
	    }

	    sxba_empty (pos_set);
	}

	if (!S1_is_empty (posxpos_stack)) {
	    do {
		opos = S1_pop (posxpos_stack);
		ipos = S1_pop (posxpos_stack);
		posxpos2lgth [ipos][opos] = -1;
	    } while (!S1_is_empty (posxpos_stack));
	}
    }

    /* Puis on appelle les predicats de l'utilisateur */
    rhs_pos = 0;

    for (rhs_prdct_pos = 1, cur = bot; cur < top; rhs_prdct_pos++, cur++) {
	prdct = XH_list_elem (rcg_rhs, cur);
	bot2 = XH_X (rcg_predicates, prdct);
	nt = XH_list_elem (rcg_predicates, bot2+1);

	if (nt > 0) {
	    arity = nt2arity [nt];
	    tooth = rhs_prdct_pos2tooth [rhs_prdct_pos];
	    generator_pos = rhs_prdct_pos2generator_pos [rhs_prdct_pos];

	    if (XH_list_elem (rcg_predicates, bot2) == 0) {
		if (arity)
		    fprintf (parser_file,
			     "if ((ret_vals[%ld] = _%s(&(rho[%ld]), &(olb[%ld]), &(oub[%ld])))==-1) continue;\n",
			     (SXINT) generator_pos, sxstrget (nt2ste [nt]), (SXINT) tooth+1, (SXINT) rhs_pos, (SXINT) rhs_pos);
		else
		    fprintf (parser_file,
			     "if ((ret_vals[%ld] = _%s(&(rho[%ld])))==-1) continue;\n",
			     (SXINT) generator_pos, sxstrget (nt2ste [nt]), (SXINT) tooth+1);
	    }
	    else {
		/* Le 09/02/04, on ne se moque plus de la valeur de rho.  De plus on le passe en negatif
		   afin de reconnaitre facilement les appels negatifs... */
		/* negative call, on se moque de la valeur de rho */
		/* mais doit conserver la meme valeur a l'interieur de l'appel */
		if (arity)
		    fprintf (parser_file,
			     "if ((ret_vals[%ld] = -_%s(&(rho[%ld]), &(olb[%ld]), &(oub[%ld])))==-1) continue;rho[%ld] = -rho[%ld];\n",
			     (SXINT) generator_pos, sxstrget (nt2ste [nt]), (SXINT) tooth+1, (SXINT) rhs_pos, (SXINT) rhs_pos, (SXINT) tooth+1, (SXINT) tooth+1);
		else
		    fprintf (parser_file,
			     "if ((ret_vals[%ld] = -_%s(&(rho[%ld])))==-1) continue;rho[%ld] = -rho[%ld];\n",
			     (SXINT) generator_pos, sxstrget (nt2ste [nt]), (SXINT) tooth+1, (SXINT) tooth+1, (SXINT) tooth+1);
	    }

	    rhs_pos += arity;
	}
	else
	    rhs_pos += XH_X (rcg_predicates, prdct+1)-bot2-2;
    }

    /* ... et la semantique */
    /* A FAIRE: traiter le cas de is_generator.  Il faut remplir Indexes */
    
#if 0
    if (A_is_cyclic) {
      fprintf (parser_file, "#if is_cyclic\n\
ret_val = 1;\n\
#else /* !is_cyclic */\n\
%sret_val = 1;\n\
#endif /* !is_cyclic */\n",
	       A_clause_nb > 1 ? "local_ret_val = " : "");
    }
    else
      fprintf (parser_file, "%sret_val = 1;\n", A_clause_nb > 1 ? "local_ret_val = " : "");
#endif /* 0 */
    
#if 0
    /* Le 07/08/08 */
    fprintf (parser_file, "%sret_val = 1;\n",
	     A_clause_nb > 1 ? "local_" : "");
#endif /* 0 */

    if (is_cyclic_grammar) {
	  fputs ("#if is_cyclic\n\
if (!is_glbl_cycle) {\n\
#endif\n", parser_file);
    }

#if is_semantics==1
    fputs ("#if is_semantics\n", parser_file);
#endif /* is_semantics==1 */

    /* Ajout du 28/05/2003 */
    /* On donne la position ds le source des terminaux */
    if (lhs_arity) {
      SXINT lhs_prdct, gen_guided_bounds_lhs_bot2, gen_guided_bounds_lhs_top2, lhs_arg_nb, t_nb, gen_guided_bounds_cur2, gen_guided_bounds_param, gen_guided_bounds_bot3, gen_guided_bounds_top3, cur3, pos_1rcg, gen_guided_bounds_bot, gen_guided_bounds_top, gen_guided_bounds_cur, gen_guided_bounds_xtop, x, local_t_nb; 

      sxinitialise(pos_1rcg); /* pour faire taire gcc -Wuninitialized */
      sxinitialise(gen_guided_bounds_bot); /* pour faire taire gcc -Wuninitialized */
      sxinitialise(gen_guided_bounds_top); /* pour faire taire gcc -Wuninitialized */
					  
      lhs_prdct = XxY_X (rcg_clauses, clause) & lhs_prdct_filter;
      gen_guided_bounds_lhs_bot2 = XH_X (rcg_predicates, lhs_prdct);
      gen_guided_bounds_lhs_top2 = XH_X (rcg_predicates, lhs_prdct+1);
      lhs_arg_nb = 0;
      t_nb = 0;
      varstr_raz (t_indexes_vstr);
    
      for (gen_guided_bounds_cur2 = gen_guided_bounds_lhs_bot2+2; gen_guided_bounds_cur2 < gen_guided_bounds_lhs_top2; lhs_arg_nb++, gen_guided_bounds_cur2++) {
	gen_guided_bounds_param = XH_list_elem (rcg_predicates, gen_guided_bounds_cur2);

	if (gen_guided_bounds_param > 0) {
	  gen_guided_bounds_bot3 = XH_X (rcg_parameters, gen_guided_bounds_param);
	  gen_guided_bounds_top3 = XH_X (rcg_parameters, gen_guided_bounds_param+1);
	  local_t_nb = 0;

	  for (cur3 = gen_guided_bounds_bot3; cur3 < gen_guided_bounds_top3; cur3++) {
	    if (XH_list_elem (rcg_parameters, cur3) < 0) {
	      if (t_nb == 0) {
		fputs ("#if is_sdag && (is_parse_forest || is_2var_form || is_shallow)\n", parser_file);
		gen_guided_bounds_bot = clause2rhs_pos [clause]; /* clause lexicalisee... */
		gen_guided_bounds_top = clause2rhs_pos [clause+1]; /* ... on va en avoir besoin */
	      }

	      if (local_t_nb == 0) {
		/* Calcul de pos_1rcg initial */
		pos_1rcg = 0;

		for (gen_guided_bounds_cur = gen_guided_bounds_bot; gen_guided_bounds_cur < gen_guided_bounds_top; gen_guided_bounds_cur++) {
		  gen_guided_bounds_xtop = rhs_pos2disp [gen_guided_bounds_cur+1]-1;

		  if (rhs_pos2lhs_arg_no [gen_guided_bounds_xtop] == lhs_arg_nb) {
		    if ((x = rhs_pos2rhs_pos_1rcg [gen_guided_bounds_xtop]) > pos_1rcg)
		      pos_1rcg = x;
		  }
		}
	      }

	      fprintf (parser_file, "rho[%ld] = pnode%ld->rho[%ld];\n", (SXINT) 2+max_son_nb+t_nb, (SXINT) lhs_arg_nb, (SXINT) ++pos_1rcg);
	      t_nb++;
	      local_t_nb++;
	    }
	  }
	}
      }

      if (t_nb)
	fputs ("#endif\n", parser_file);
    }

    fputs ("if (*rho0 == 0) rho[1] = *rho0 = local_Aij2global_Aij_pid [Aij] = MAKE_Aij_pid (Aij, G.pid);\n", parser_file); /* Le 07/08/08 */

    /* Modif du Mar  7 Nov 2000 14:07:00 : pour chaque clause le nombre de fils et chacun de leur pid sont
       stockes ds clause2son_nb et clause2son_pid */
    fputs ("if ((*G.semact.first_pass)(&G, rho", parser_file);

#if 0
    fputs ("\n#if is_generator\n\
,ret_vals\n\
#endif\n", parser_file);
#endif

    fputs (")==-1) continue;\n", parser_file);

#if is_semantics==1
    fputs ("#endif /* is_semantics */\n", parser_file);
#endif /* is_semantics==1 */

    if (is_cyclic_grammar) {
      fputs ("#if is_cyclic\n\
}\n", parser_file);

      /* si l'ip est vrai, ca ne sert a rien de continuer */
      fputs ("else break;\n", parser_file);

      fputs ("#endif /* is_cyclic */\n", parser_file);
    }

    /* Le 07/08/08 */
    fprintf (parser_file, "%sret_val = 1;\n",
	     A_clause_nb > 1 ? "local_" : "");

    /* Le 15/11/2001, je prends le meme genere que le cas non fully coupled (sauf generator a faire) */

    fputs ("#if is_recognizer\n\
break;\n\
#endif\n", parser_file);
    
}

       
       
static void
gen_t_bounds (SXINT clause)
{
  /* Premiere tentative sur les arguments de la LHS */
  SXINT		xeq, nb, rhs_pos, pos, param, cur3, nt;
#if 0
  SXINT         counter_nb, counter_val, bot3, top3;
#endif
  bool	is_olb, is_oub;
  static char exit_stmt [] = "break";
    
  /* A FAIRE
     si A(X,Y,Z) -> B(X,Y,Z).
     l'appel de B peut se faire sans aucune verif!
  */

  is_static_phase = false;
  Sindex = 0;
  next_ii = 0;
  not_substituted_nb = 0;

  if (max_component_nb) {
    /* On commence par affecter les o[l|u]b qui correspondent a des "..." */
    /* Ils correspondent au madr "0" */
    SXBA_1_bit (assigned_madr_set, 0);

    for (component_nb = 0; component_nb < max_component_nb; component_nb++) {
      rhs_pos = component2pos [component_nb];
      param = rhs_pos2param [rhs_pos];

      if ((cur3 = component2bot [component_nb]) == XH_X (rcg_parameters, param) &&
	  XH_list_elem (rcg_parameters, cur3) == 0) {
	/* C'est le composant prefixe de l'arg pos = ...\alpha */
	is_olb = true;
	/* L'olb de \alpha est sans importance */
	/* Output "olb[rhs_pos-lhs_arity]=0;" */
	fprintf (parser_file, "olb[%ld]=0;\n", (SXINT) rhs_pos-lhs_arity);

	pos = rhs_pos;

	do {
	  nt = rhs_pos2nt [pos];

	  if (rfsa_file && nt > 0)
	    gen_rfsa_guide (nt, pos, exit_stmt);

	  if (nt == FIRST_ic || (nt > 0 && SXBA_bit_is_set (is_nt_internal, nt)))
	    /* Pas de test de first/last sur les predicats externes (predefinis ou non) */
	    gen_first_test (clause, pos, exit_stmt);		
	} while ((pos = olbs [pos]) > 0);
      }
      else
	is_olb = false;

      if ((cur3 = component2top [component_nb]) == XH_X (rcg_parameters, param+1) &&
	  XH_list_elem (rcg_parameters, cur3-1) == 0) {
	/* C'est le composant suffixe de l'arg rhs_pos = ...\alpha */
	is_oub = true;
	/* L'olb de \alpha est sans importance */
	/* Output "oub[rhs_pos-lhs_arity]=n;" */
	fprintf (parser_file, "oub[%ld]=n;\n", (SXINT) rhs_pos-lhs_arity);

	pos = rhs_pos;

	do {
	  nt = rhs_pos2nt [pos];

	  if (lfsa_file && nt > 0)
	    gen_lfsa_guide (nt, pos, exit_stmt);

	  if (nt == LAST_ic || (nt > 0 && SXBA_bit_is_set (is_nt_internal, nt)))
	    /* Pas de test de first/last sur les predicats externes (predefinis ou non) */
	    gen_last_test (clause, pos, exit_stmt);		
	} while ((pos = oubs [pos]) > 0);
      }
      else
	is_oub = false;

      if (is_olb && is_oub) {
	pos = rhs_pos;

	do {
	  nt = rhs_pos2nt [pos];

	  if (is_1_RCG && nt > 0)
	    gen_karg_guide (/* clause, */pos, exit_stmt);		

	  if (nt > 0 && SXBA_bit_is_set (is_nt_internal, nt))
	    gen_min_max_test (/* clause, */pos, exit_stmt);
	} while ((pos = oubs [pos]) > 0);
      }
    }
  }

  if (max_counter_nb) {
    /* La clause a des args qui sont des constantes */
    /* On genere le code des args de la rhs qui ne contiennent que des constantes */
    /* Ils correspondent au madr "0" */
    SXBA_1_bit (assigned_madr_set, 0);

#if 0
    for (counter_nb = 0; counter_nb < max_counter_nb; counter_nb++) {
      rhs_pos = counter2pos [counter_nb];
      param = rhs_pos2param [rhs_pos];
	
      bot3 = XH_X (rcg_parameters, param);
      top3 = XH_X (rcg_parameters, param+1);

      for (cur3 = bot3; cur3 < top3; cur3++) {
	if (XH_list_elem (rcg_parameters, cur3) >= -max_ste)
	  /* Pas cste */
	  break;
      }

      if (cur3 == top3) {
	/* Que des cstes */
	counter_val = pos2counter_val [rhs_pos];
	rhs_pos -= lhs_arity;
	fprintf (parser_file, "oub[%ld] = (olb[%ld] = counter_lb)+%ld;\n", rhs_pos, rhs_pos, counter_val);
      }
    }
#endif /* 0 */
  }

  for (xeq = 0; xeq < lhs_equation; xeq++) {
    if (xeq < lhs_arity)
      /* Le traitement des bornes des args de la LHS doit etre fait en premier */
      /* les affectations eventuelles des o[l|u]b pourraient etre differees car elles
	 sont inconditionnelles et sont inutiles si un test ulterieur echoue! */
      lhs_arg_processing (clause, xeq, exit_stmt, true);

    if ((nb = equation2Xnb [xeq]) == 0) {
      process_0_atom (xeq, exit_stmt);
      /* L'arg xeq n'est peut etre pas encore verifie :
	 ..., XaY, ...  et StrLen (4, XaY) */
    }
    else {
      if (nb == 1) {
	if (xeq < lhs_arity && SXBA_bit_is_set_reset (useless_lhs_arg, xeq)) {
	  /* Si cet arg == 1 seul atome et cet atome n'a pas d'autres occurrences
	     (Ex: S(X) --> .), aucun calcul ni verif n'est necessaire */
	  lhs_arg_tbp--;
	  SXBA_0_bit (lhs_arg_tbp_set, xeq);
	}
	else
	  PUSH (equation_stack, xeq);
      }
      else {
	not_substituted_nb++;
	SXBA_1_bit (nyp_eq_set, xeq);
      }
    }
  }

  gen_level ((SXINT)1, clause, exit_stmt);

#if EBUG
  if (rhs_prdct_nyp_nb || sxba_cardinal (rhs_prdct_nyp) != 0)
    sxtrap (ME, "gen_t_bounds");
#endif
}




static void
gen_header (void)
{
    fprintf (parser_file,
"\n\
/* ********************************************************************\n\
\tThis parser for the language \"%s\" has been generated\n\
\tby the SYNTAX(*) Range Concatenation Grammar processor\n\
   ********************************************************************\n\
\t(*) SYNTAX is a trademark of INRIA.\n\
   ******************************************************************** */\n\n",
	     PRGENTNAME(true));
	
    fprintf (parser_file,"static char ME [] = \"%s\";\n\n\n", PRGENTNAME(true));
}


static void
gen_includes (void)
{
    fputs ("\n/************** I N C L U D E S **********************/\n", parser_file);
    fputs ("#include \"sxba.h\"\n", parser_file);
#if 0
    {/* mis ds rcg_glbl.h le Lun 4 Dec 2000 17:01:26 */
      SXINT 	arity = 1;

      while ((arity = sxba_scan (arity_set, arity)) > 0) {
	switch (arity) {
	case 1:
	      /* fputs ("#include \"X.h\"\n", parser_file); deja fait! */
	  break;

	case 2:
	  fputs ("#include \"XxY.h\"\n", parser_file);
	  break;

	case 3:
	  fputs ("#include \"XxYxZ.h\"\n", parser_file);
	  break;

	default:
	      /* >3 */
	  fputs ("#include \"XH.h\"\n", parser_file);
	  break;
	}
      }
    }
#endif /* 0 */

    fputs ("#include \"rcg_glbl.h\"\n", parser_file);

    if (terminal_file) {
      fputs ("#ifdef define_tlist_h\n", parser_file);
      fputs ("#include define_tlist_h\n", parser_file);
      fputs ("#else /* define_tlist_h */\n", parser_file);
      fprintf (parser_file, "#error You must compile this module with the \"-Ddefine_tlist_h=\\\"%s_define_tlist.h\\\"\" option\n", (prgentname == NULL) ? "your" : prgentname);
      fputs ("#endif /* define_tlist_h */\n", parser_file);
    }

    fputs ("\n", parser_file);
}


static struct word_tree_struct dico_word_tree;


static void
dico_word_tree_oflw (SXINT old_size, SXINT new_size)
{
  sxuse (old_size);
  word_tree_oflw (&dico_word_tree, old_size, new_size);
}

static void
gen_local_variables (void)
{
  SXINT	A, clause, rhs, cur, bot, top, son_nb, prdct, bot2, nt, t, lgth;
  SXINT	*gen_local_variables_clause2son_nb;
  bool	is_first, done, has_ext_prdct_call;
  char        *str;

  fputs ("\n/************** L O C A L   V A R I A B L E S **********************/\n", parser_file);

  fputs ("#if 0\n\
/* Adaptation a udag_scanner */\n\
#if AG\n\
static struct idag	idag;\n\
static bag_header	bag_hd;\n\
#else\n\
#if is_sdag\n\
static SXBA\t*source, *lsource;\n\
#else\n\
static SXINT\t*source, *lsource;\n\
#endif\n\
#endif\n\
#endif /* 0 */\n\
", parser_file);

  fputs ("\n#if 0\n\
/* Obsolete : maintenant les codes des terminaux sont globaux aux modules, l'acces aux chaines\n\
   terminales se fait par un appel a char **idag_get_t2string (void) qui retourne le pointeur vers t2string */\n", parser_file);
  fputs ("\nstatic char *t2str [] = {\n\
/* 0 */ \"\",\n", parser_file);
  for (t = 1; t <= max_t; t++) {
    str = sxstrget (t2ste [t]);

    if (*str == '"')
      str++;

    fprintf (parser_file, "/* %ld */ \"%s\",\n", (SXINT) t, str);
  }

  fputs ("};\n", parser_file);

  {
    static sxword_header      t_names;
    SXINT ste, id;

    sxword_alloc (&t_names, "t_names", max_t+1, 1, 16,
		  sxcont_malloc, sxcont_alloc, sxcont_realloc, NULL, sxcont_free, NULL, NULL);

    for (t = 1; t <= max_t; t++) {
      ste = t2ste [t];
      str = sxstrget (ste);
      lgth = sxstrlen (ste);

      if (*str == '"') {
	str++;
	lgth--;
      }

      id = sxword_2save (&t_names, str, lgth); /* id == t+1 */
      (void) id;
    }

    sxword_to_c (&t_names, parser_file, "t_names", true);

    sxword_free (&t_names);
  }

  fputs ("#endif /* 0 */\n", parser_file);

  if (is_a_complete_terminal_grammar) {
    SXINT                   ste;
    struct sxdfa_struct     dico_sxdfa;
    struct sxdfa_comb       dico;
    
    word_tree_alloc (&dico_word_tree, "dico_word_tree", max_t+1, 16
		     , 1 /* Xforeach */, 0 /* Yforeach */, true /* from_left_to_right */
		     , true /* with_id */, dico_word_tree_oflw,
#if EBUG
		     stdout
#else
		     NULL
#endif /* EBUG */
		     );
	
    
    for (t = 1; t <= max_t; t++) {
      ste = t2ste [t];
      str = sxstrget (ste);
      lgth = sxstrlen (ste);

      if (*str == '"') {
	str++;
	lgth--;
      }

      word_tree_add_a_string (&dico_word_tree, str, lgth, t);
    }

    word_tree2sxdfa (&dico_word_tree, &dico_sxdfa, "dico_sxdfa",
#if EBUG
		     stdout
#else
		     NULL
#endif /* EBUG */
		     , true /* make_a_min_dag */);

    sxdfa2comb_vector (&dico_sxdfa, 0 /* comb_kind */, 10000 /* comb_vector_threshold */, &dico);
    sxdfa_free (&dico_sxdfa);

    /* Recherche par 
       t = (SXINT) sxdfa_comb_seek_a_string (&t_names_dico, string, &lgth);

       if (t > 0 && lgth == 0) {
          OK
       }
    */


    fprintf (parser_file, "\n\n#if %ld < SXT_End_Of_File\n", max_t+1);
    fputs ("/* Il y a des terminaux qui sont definis dans un autre module */\n", parser_file);

    sxdfa_comb2c (&dico, stdout, "t_names_dico" /* dico_name */, true /* is_static */);

    fprintf (parser_file, "#endif /* %ld < SXT_End_Of_File */\n\n\n", max_t+1);
	
    sxdfa_comb_free (&dico);
  }

  if (is_a_complete_terminal_grammar) {
    /* On va sortir prdct2t_set, t2clause et t2clause_stack */
    SXINT lhs_prdct, param, lnk_nb, x, nb, ste;
    SXINT *t2clause, *clause_stack, *clause_stack_lnk;
    SXBA  *prdct2t_set, t_set;

    prdct2t_set = sxbm_calloc (max_nt+1, max_t+1);

    t2clause = (SXINT *) sxcalloc (max_t+1, sizeof (SXINT));

    DALLOC_STACK (clause_stack, last_clause+1);
    DALLOC_STACK (clause_stack_lnk, last_clause+1);

    top = 0;
    lnk_nb = 0;

    for (A = 1; A <= max_nt; A++) {
      A_clause_set = lhsnt2clause [A];
      t_set = prdct2t_set [A];

      clause = 0;

      while ((clause = sxba_scan (A_clause_set, clause)) > 0) {
	lhs_prdct = XxY_X (rcg_clauses, clause) & lhs_prdct_filter;
	bot = XH_X (rcg_predicates, lhs_prdct);
	param = XH_list_elem (rcg_predicates, bot+2);
	t = -XH_list_elem (rcg_parameters, XH_X (rcg_parameters, param));
	SXBA_1_bit (t_set, t);

	if ((clause_id = t2clause [t]) == 0)
	  t2clause [t] = clause;
	else {
	  DCHECK (clause_stack, 2);
	  DCHECK (clause_stack_lnk, 2);

	  if (clause_id > 0) {
	    /* vraie clause */
	    lnk_nb++;
	    DSPUSH (clause_stack, clause_id);
	    DSPUSH (clause_stack, clause);
	    top = DTOP (clause_stack);
	    DSPUSH (clause_stack_lnk, 0);
	    DSPUSH (clause_stack_lnk, top-1);
	  }
	  else {
	    DSPUSH (clause_stack, clause);
	    DSPUSH (clause_stack_lnk, -clause_id);
	    top = DTOP (clause_stack);
	  }
	      
	  t2clause [t] = -top;
	}
      }
    }
    
    sxbm3_to_c (parser_file, prdct2t_set, max_nt+1, "prdct2t_set", "", true /* is_static */);

    fprintf (parser_file, "\nstatic SXINT t2clause_stack [%ld] = {0,\n", (SXINT)  top+lnk_nb+1);

    if (top) {
      top = 0;

      for (t = 1; t <= max_t; t++) {
	if ((clause_id = t2clause [t]) < 0) {
	  for (x = -clause_id, nb = 0; x != 0; x = clause_stack_lnk [x], nb++);

	  t2clause [t] = -(++top);

	  fprintf (parser_file, "/* %ld */ %ld", (SXINT) top, (SXINT) nb);

	  for (x = -clause_id; x != 0; x = clause_stack_lnk [x]) {
	    top++;
	    fprintf (parser_file, ", %ld", (SXINT) clause_stack [x]);
	  }

	  fputs (",\n", parser_file);
	}
      }
    }

    fputs ("};\n\n", parser_file);

    fprintf (parser_file, "static SXINT t2clause [%ld] = {0,\n", (SXINT)  max_t+1);

    for (t = 1; t <= max_t; t++) {
      ste = t2ste [t];
      str = sxstrget (ste);

      if (*str == '"') {
	str++;
      }

      fprintf (parser_file, "/* %s == %ld */ %ld,\n", str, (SXINT) t, (SXINT) t2clause [t]);
    }

    fputs ("};\n\n", parser_file);

    sxbm_free (prdct2t_set);
    DFREE_STACK (clause_stack);
    DFREE_STACK (clause_stack_lnk);
  }

  fputs ("\nstatic SXINT clause2lhs_nt[clmax+1]={0,", parser_file);

  for (clause = 1; clause <= last_clause; clause++) {
    if (clause%16 == 1)
      fprintf (parser_file, "\n/* %ld */ ", (SXINT) clause);

    fprintf (parser_file, "%ld, ", (SXINT) clause2lhs_nt (clause));
  }

  fputs ("};\n\n", parser_file);

  
  if (!is_a_complete_terminal_grammar)
    fputs ("static SXINT\tdum_range;\n\n", parser_file);

  if (has_external) {
    if (sxba_2op (ext_decl_set /* utilisation avant l'init reelle */, SXBA_OP_COPY, is_nt_external, SXBA_OP_AND, is_lhs_nt)) {
      /* Ce module a des definitions de predicats externes */
      fputs ("SXINT\t\t", parser_file);
      has_ext_def = true;
      A = 0;
      is_first = true;

      while ((A = sxba_scan (ext_decl_set, A)) > 0) {
	if (is_first)
	  is_first = false;
	else
	  fputs (", ", parser_file);

	fprintf (parser_file, "%s_pid", sxstrget (nt2ste [A]));
      }

      fputs (";\n", parser_file);
    }

    if (sxba_2op (ext_decl_set, SXBA_OP_COPY, is_nt_external, SXBA_OP_MINUS, is_lhs_nt)) {
      /* Ce module a des declarations de predicats externes */
      fputs ("extern SXINT\t\t", parser_file);
      has_ext_decl = true;
      A = 0;
      is_first = true;

      while ((A = sxba_scan (ext_decl_set, A)) > 0) {
	if (is_first)
	  is_first = false;
	else
	  fputs (", ", parser_file);

	fprintf (parser_file, "%s_pid", sxstrget (nt2ste [A]));
      }

      fputs (";\n", parser_file);
    }

    /* On verifie la coherence du nombre des arg des predicats externes entre leur def et
       leurs decl. */
    if (has_ext_decl) {
      /* La def est ds un autre module */
      fputs ("extern SXINT\t\t", parser_file);
      A = 0;
      is_first = true;

      while ((A = sxba_scan (ext_decl_set, A)) > 0) {
	if (is_first)
	  is_first = false;
	else
	  fputs (", ", parser_file);

	fprintf (parser_file, "%s_arity", sxstrget (nt2ste [A]));
      }

      fputs (";\n", parser_file);

      /* On cherche toutes les clauses ou il y a des predicats externes definis ds d'autres modules */
      fputs ("#if 0\n\
static bool\t", parser_file);
      is_first = true;
      clause = 0;

      while ((clause = sxba_scan (clause_set, clause)) > 0) {
	rhs = XxY_Y (rcg_clauses, clause);
	top = XH_X (rcg_rhs, rhs+1);
	bot = XH_X (rcg_rhs, rhs);
	done = false;

	for (cur = bot; cur < top; cur++) {
	  A = XH_list_elem (rcg_predicates, XH_X (rcg_predicates, XH_list_elem (rcg_rhs, cur))+1);

	  if (A > 0 && SXBA_bit_is_set (ext_decl_set, A)) {
	    ext_decl2clause [A] = clause;
	    SXBA_1_bit (ext_decl_clause_set, clause);

	    if (!done) {
	      done = true;
			    
	      if (is_first)
		is_first = false;
	      else
		fputs (", ", parser_file);

	      fprintf (parser_file, "is_useful_%ld = true", (SXINT) clause);
	    }
	  }
	}
      }

      fputs (";\n\
#endif /* 0 */\n", parser_file);
    }

    if (has_ext_def) {
      /* La def est ds ce module */
      fputs ("SXINT\t\t", parser_file);
      A = 0;
      is_first = true;

      while ((A = sxba_scan (is_nt_external, A)) > 0) {
	if (SXBA_bit_is_set (is_lhs_nt, A)) {
	  if (is_first)
	    is_first = false;
	  else
	    fputs (", ", parser_file);

	  fprintf (parser_file, "%s_arity = %ld", sxstrget (nt2ste [A]), (SXINT) nt2arity [A]);
	}
      }

      fputs (";\n", parser_file);
    }

#if 0
    /* Le 19/05/04 ... voir + loin ... */
    if (has_ext_def) {
      /* Le Ven 17 Dec 1999 10:37:13 */
      /* Sur les definitions des points d'entree externes, on verifie que pour chaque range, aucun
	 token du source ne vaut 0.  On ne fait pas cette verification dans le module principal car
	 aucun token ne peut valoir 0. */
      /* Cette verif correspond a un scanner local et est valide meme si la grammaire est top-down erasing */
      if (SXBA_bit_is_set (is_nt_external, 1)) {
	fputs ("static SXINT\tlarg_nb, *plb, *pub, lower_bound, upper_bound;\n", parser_file);
      }
    }
#endif /* 0 */
  }

  if (!is_a_complete_terminal_grammar) {
#if is_semantics==1
    fputs ("#if is_semantics\n", parser_file);
#endif /* is_semantics==1 */

    /* Les clauses qui ne font pas appel a des predicats externes declares a l'exterieur, utiliseront sons_0 */
    fprintf (parser_file, "#if 0\n\
/* Inutile, supprime' le 31/07/08 */\n\
static SXINT\tsons_0 [%ld];\n\
#endif /* 0 */\n", (SXINT) max_rhs_prdct + 1);

    gen_local_variables_clause2son_nb = (SXINT*) sxcalloc (last_clause+1, sizeof (SXINT));
    
    is_first = true;
    clause = 0;

    while ((clause = sxba_scan (clause_set, clause)) > 0) {
      rhs = XxY_Y (rcg_clauses, clause);
      top = XH_X (rcg_rhs, rhs+1);
      bot = XH_X (rcg_rhs, rhs);
      son_nb = 0;
      has_ext_prdct_call = false;

      for (cur = bot; cur < top; cur++) {
	prdct = XH_list_elem (rcg_rhs, cur);
	bot2 = XH_X (rcg_predicates, prdct);
	nt = XH_list_elem (rcg_predicates, bot2+1);

	if (XH_list_elem (rcg_predicates, bot2) == 0) {
	  /* Positive call */
	  if (nt > 0 || nt == TRUE_ic || (is_drcg && nt == STREQ_ic && is_streq_spcl (bot2+2)))
	    /* Le Ven 22 Dec 2000 11:56:10, place pour &True */
	    son_nb++;

	  if ((nt > 0 && SXBA_bit_is_set (ext_decl_set, nt)) || nt == TRUE_ic)
	    has_ext_prdct_call = true;
	}
	else {
	  /* Le 09/02/04 on compte les negatifs... */
	  if (nt > 0)
	    son_nb++;
	}
      }

      gen_local_variables_clause2son_nb [clause] = son_nb;

      if (has_ext_prdct_call) {
	SXBA_1_bit (clause_has_ext_decl, clause);

	if (is_first) {
	  fputs ("static SXINT", parser_file);
	  is_first = false;
	}
	else
	  fputs (", ", parser_file);

	fprintf (parser_file, "\n\tsons_%ld [%ld] = {0, ", (SXINT) clause, (SXINT) son_nb+1);

	/* On pre-remplit les "fils" correspondant a des &True */
	for (cur = bot; cur < top; cur++) {
	  prdct = XH_list_elem (rcg_rhs, cur);
	  bot2 = XH_X (rcg_predicates, prdct);

	  if (XH_list_elem (rcg_predicates, bot2) == 0) {
	    /* Positive call */
	    nt = XH_list_elem (rcg_predicates, bot2+1);

	    if (nt > 0 || nt == TRUE_ic) {
	      fprintf (parser_file, "%ld, ", (SXINT) ((nt > 0) ? 0 : nt));
	    }
	  }
	}

	fputs ("}", parser_file);
      }
    }

    if (!is_first)
      fputs (";\n", parser_file);

    fputs ("static SXINT clause2son_nb [] = {0,", parser_file);
#if 0
    /* Le 21/06/07 */
    fputs ("static SXINT gen_local_variables_clause2son_nb [] = {0,", parser_file);
#endif /* 0 */

    for (clause = 1; clause <= last_clause; clause++) {
      if ((clause % 8) == 1)
	fprintf (parser_file, "\n/* %ld */ ", (SXINT) clause);

      fprintf (parser_file, "%ld, ", (SXINT) gen_local_variables_clause2son_nb [clause]);
    }

    fputs ("\n};\n", parser_file);

    fputs ("\n#if 0\n\
/* Inutile, supprime' le 31/07/08 */\n\
static SXINT *clause2son_pid [] = {sons_0,", parser_file);

    for (clause = 1; clause <= last_clause; clause++) {
      if ((clause % 8) == 1)
	fprintf (parser_file, "\n/* %ld */ ", (SXINT) clause);

      if (SXBA_bit_is_set (clause_has_ext_decl, clause))
	fprintf (parser_file, "sons_%ld, ", (SXINT) clause);
      else
	fputs ("sons_0, ", parser_file);
    }

    fputs ("\n};\n\
#endif /* 0 */\n", parser_file);

    sxfree (gen_local_variables_clause2son_nb);

#if is_semantics==1
    fputs ("#endif /* is_semantics */\n", parser_file);
#endif /* is_semantics==1 */

#if 0
    fputs ("#if is_semantics\n", parser_file);
    fputs ("static struct for_semact for_semact = {sxvoid, sxvoid, sxvoid, sxvoid, sxvoid, sxivoid, sxivoid, sxivoid};\n",
	   parser_file);
    fputs ("#endif /* is_semantics */\n", parser_file);
#endif
  }
}


static void
fill_Lex_t2clause_set (SXBA **L, SXBA **t2, SXBA **c2, bool with_First_Last, SXINT t_nb_lim)
{    
  SXINT		clause, prdct, bot2, top2, arity, cur2, param, bot3, cur3, top3, symb;
  SXINT		rhs, top, cur, bot, nt;
  SXINT		t_nb, nt_nb;
  SXBA	        *local_Lex, *fill_Lex_t2clause_set_t2clause_set, line, *clause2rhs_nt_set, rhs_nt_set;
  bool       has_first_last;
#if 0
  bool       is_loop_clause_set_empty;
#endif /* 0 */

  local_Lex = sxbm_calloc (last_clause+1, max_t+1);
  /* Le 21/02/2002 j'ai mis ds la derniere ligne de fill_Lex_t2clause_set_t2clause_set l'ensemble [fsa_]loop_clause_set
     Il est utilise ds lexicalization () ds le cas d'un guide lrfsa */
  /* Le 26/11/2002 loop_clause_set devient une structure a part (changement de version => 3) */
#if 0
  is_loop_clause_set_empty = sxba_is_empty (loop_clause_set);
  fill_Lex_t2clause_set_t2clause_set = sxbm_calloc (max_t+1+((is_loop_clause_set_empty) ? 0 : 1), last_clause+1);

  if (!is_loop_clause_set_empty)
    sxba_copy (fill_Lex_t2clause_set_t2clause_set [max_t+1], loop_clause_set);
#endif /* 0 */

  fill_Lex_t2clause_set_t2clause_set = sxbm_calloc (max_t+1, last_clause+1);

  clause2rhs_nt_set = sxbm_calloc (last_clause+1, max_nt+1);

  for (clause = 1; clause <= last_clause; clause++) {
    nt_nb = t_nb = 0;

    if (!SXBA_bit_is_set (false_clause_set, clause)
	&& clause2identical [clause] == clause) {
      line = local_Lex [clause];
      prdct = XxY_X (rcg_clauses, clause) & lhs_prdct_filter;
      bot2 = XH_X (rcg_predicates, prdct);
      top2 = XH_X (rcg_predicates, prdct+1);
      arity = nt2arity [XH_list_elem (rcg_predicates, bot2+1)];

      if (arity) {
	/* En lhs si un predicat est d'arite 0, il a un param qui vaut 0 */
	for (cur2 = bot2+2; cur2 < top2; cur2++) {
	  param = XH_list_elem (rcg_predicates, cur2);

	  if (param > 0) {
	    bot3 = XH_X (rcg_parameters, param);
	    top3 = XH_X (rcg_parameters, param+1);

	    for (cur3 = bot3; cur3 < top3; cur3++) {
	      symb = -XH_list_elem (rcg_parameters, cur3);

	      if (symb > 0 && symb <= max_ste) {
		/* terminal */
		if (SXBA_bit_is_reset_set (line, symb))
		  t_nb++;

		SXBA_1_bit (fill_Lex_t2clause_set_t2clause_set [symb], clause);
	      }
	    }
	  }
	}
      }

      /* On prend aussi tous les terminaux de la RHS */
      /* Ca permet de traiter les cas &StrEq(\a\b\c, X Y), ... */
      /* ... et &Lex (\e\f\g) */
      /* ATTENTION, cas particulier pour &First et &Last */

      if ((rhs = XxY_Y (rcg_clauses, clause))) {
	rhs_nt_set = clause2rhs_nt_set [clause];
	top = XH_X (rcg_rhs, rhs+1);
	bot = XH_X (rcg_rhs, rhs);
	has_first_last = false;
		    
	for (cur = bot; cur < top; cur++) {
	  prdct = XH_list_elem (rcg_rhs, cur);
	  bot2 = XH_X (rcg_predicates, prdct);
	  nt = XH_list_elem (rcg_predicates, bot2+1);

	  if (nt > 0) {
	    if (SXBA_bit_is_reset_set (rhs_nt_set, nt))
	      nt_nb++;
	  }

	  if (XH_list_elem (rcg_predicates, bot2) == 0) {
	    /* On ignore les negations!! */
	    if (nt != FIRST_ic && nt != LAST_ic) {
	      top2 = XH_X (rcg_predicates, prdct+1);

	      for (cur2 = bot2+2; cur2 < top2; cur2++) {
		param = XH_list_elem (rcg_predicates, cur2);

		if (param > 0 && !(nt == STRLEN_ic && cur2 == bot2+2)) {
		  bot3 = XH_X (rcg_parameters, param);
		  top3 = XH_X (rcg_parameters, param+1);

		  for (cur3 = bot3; cur3 < top3; cur3++) {
		    symb = -XH_list_elem (rcg_parameters, cur3);

		    if (symb > 0 && symb <= max_ste) {
		      /* terminal */
		      if (SXBA_bit_is_reset_set (line, symb))
			t_nb++;

		      SXBA_1_bit (fill_Lex_t2clause_set_t2clause_set [symb], clause);
		    }
		  }
		}
	      }
	    }
	    else
	      has_first_last = true;
	  }
	}

	/* Le 22/02/2002 l'appelant choisit le nb de terminaux max (0 ou 1) qui produit un Lex [clause] == NULL */
	if (t_nb <= t_nb_lim)
	  /* Modif du Jeu 19 Avr 2001 13:41:57 */
	  /* ATTENTION : surtout ne pas toucher a Lex [0] pour le sxbm_free ... */
	  local_Lex [clause] = NULL;

	if (with_First_Last && has_first_last && t_nb == 0) {
	  /* On met les terminaux de &First et &Last ds fill_Lex_t2clause_set_t2clause_set */
	  /* Ca permet, si aucun de ces terminaux n'est ds le source de ne pas selectionner cette clause */
	  for (cur = bot; cur < top; cur++) {
	    prdct = XH_list_elem (rcg_rhs, cur);
	    bot2 = XH_X (rcg_predicates, prdct);
	    nt = XH_list_elem (rcg_predicates, bot2+1);

	    if (XH_list_elem (rcg_predicates, bot2) == 0 && (nt == FIRST_ic || nt == LAST_ic)) {
	      /* On ignore les negations!! */
	      param = XH_list_elem (rcg_predicates, bot2+2);

	      if (param > 0) {
		bot3 = XH_X (rcg_parameters, param);
		top3 = XH_X (rcg_parameters, param+1);

		for (cur3 = bot3; cur3 < top3; cur3++) {
		  symb = -XH_list_elem (rcg_parameters, cur3);

		  if (symb >= 0) {
		    /* terminal ou eof */
		    /* Si eof, clause sera selectionne independamment du source (qui a toujours eof) */
		    t_nb++;
		    SXBA_1_bit (fill_Lex_t2clause_set_t2clause_set [symb], clause);
		  }
		}
	      }
	    }
	  }
	}
      }

      if (t_nb == 0)
	/* Les clauses non-lexicalisees sont mises ds l'element 0 */
	SXBA_1_bit (fill_Lex_t2clause_set_t2clause_set [0], clause);
    }
    else
      local_Lex [clause] = NULL;

    if (nt_nb == 0)
      clause2rhs_nt_set [clause] = NULL;
  }

  *L = local_Lex;
  *t2 = fill_Lex_t2clause_set_t2clause_set;
  *c2 = clause2rhs_nt_set;
}


static void
gen_lex (void)
{
  SXINT     A;
  /* SXINT     *A2max_Aclause, Aclause, max_Aclause; */
  SXBA	    *clause2rhs_nt_set;
  bool is_Lex_empty = true;

  fill_Lex_t2clause_set (&Lex, &t2clause_set, &clause2rhs_nt_set, true, (SXINT)(lfsa_file ? 0 : 1));

  fputs ("/************** L E X **********************/\n", parser_file);

  if (max_t) {
    fputs ("\n#if is_lex && !is_guided\n", parser_file);
    fprintf (parser_file, "#if %ld == SXT_End_Of_File\n", max_t+1);
    sxbm3_to_c (parser_file, Lex, last_clause+1, "Lex", "", true /* is_static */);
    fprintf (parser_file, "#else /* %ld == SXT_End_Of_File */\n\
    /* Initialisation dynamique avec les terminaux globaux ... */\n", max_t+1);
    {
      SXINT     clause;
      SXBA      line;

      for (clause = 0; clause <= last_clause; clause++) {
	if ((line = Lex [clause]) && !sxba_is_empty (line)) {
	  is_Lex_empty = false;
	  fprintf (parser_file, "static SXBA_CST_ALLOC (Lex_%ld, SXT_End_Of_File+1);\n", clause);
	}
      }

      fprintf (parser_file, "static SXBA Lex [%ld]", last_clause+1);

      if (!is_Lex_empty) {
	fputs (" = {", parser_file);

	for (clause = 0; clause <= last_clause; clause++) {
	  if (clause % 16 == 0)
	    fprintf (parser_file, "\n/* %ld */ ", (SXINT) clause);

	  if ((line = Lex [clause]) && !sxba_is_empty (line)) {
	    fprintf (parser_file, "Lex_%ld, ", clause);
	  }
	  else
	    fputs ("NULL, ", parser_file);
	}

	fputs ("}", parser_file);
      }

      fputs (";\n", parser_file);
    }
    fprintf (parser_file, "#endif /* %ld == SXT_End_Of_File */\n", max_t+1);
    fputs ("#endif /* is_lex && !is_guided */\n", parser_file);
  }

  fputs ("\n#if is_lex3\n", parser_file);
  sxbm3_to_c (parser_file, clause2rhs_nt_set, last_clause+1, "clause2rhs_nt_set", "", true /* is_static */);
  fputs ("\n#endif /* is_lex3 */\n", parser_file);

  fputs ("\n#if is_lex && !is_guided || is_sdag && is_parse_forest\n", parser_file);
  /* Le 11/02/04 la vraie sortie est differee et faite ds output_t2clause_set () */
  fputs ("static SXBA t2clause_set [];\n", parser_file);
  fputs ("#endif /* is_lex && !is_guided || is_sdag && is_parse_forest */\n", parser_file);

  fputs ("\n#if is_lex\n", parser_file);
  /* fprintf (parser_file, "static char is_Lex [%ld];\n", last_clause+1); */
  if (is_loop_grammar)
    /* loop_lex = sxba_calloc (last_clause+1); */
    fputs ("static SXBA_ELT loop_lex [1+SXNBLONGS(clmax+1)] = {clmax+1,};\n", parser_file);

  {
    SXINT gen_lex_A_clause_nb;
#if 0
    char str [12];
#endif /* 0 */
    SXBA clause_id_set;

    clause_id_set = sxba_calloc (max_clause_id+1);

    for (A = 1; A <= max_nt; A++) {
      /* On compte tout */
      gen_lex_A_clause_nb = sxba_cardinal (lhsnt2clause [A]);
      fprintf (parser_file,
	       "static SXBA_CST_ALLOC (nt2clause_id_set_%ld, %ld);\n",
	       (SXINT) A,
	       gen_lex_A_clause_nb+1);

#if 0
      clause_id_set [0] = gen_lex_A_clause_nb+1;
      sprintf (str, "%ld", (SXINT) A);
      sxba_to_c (clause_id_set, parser_file, "nt2clause_id_set_", str, true /* static */);
#endif /* 0 */
    }
  
    fputs ("static SXBA nt2clause_id_set [ntmax+1] = {NULL,\n", parser_file);

    for (A = 1; A <= max_nt; A++) {
      fprintf (parser_file, "/* %ld */ nt2clause_id_set_%ld,\n", (SXINT) A, (SXINT) A);
    }

    fputs ("};\n", parser_file);

    sxfree (clause_id_set);
  }



#if 0
  {
    SXINT lexcode = 1;
    
    fputs ("static SXINT nt2lex_code[ntmax+1]={0,", parser_file);

    for (A = 1; A <= max_nt; A++) {
      if (A%16 == 1)
	fprintf (parser_file, "\n/* %ld */ ", A);

      fprintf (parser_file, "%ld, ", lexcode);
	  /* On reserve la place pour les lexcode */
      lexcode += sxba_cardinal (lhsnt2clause [A])+1;
    }

    fputs ("};\n", parser_file);

    fputs ("static SXINT lex_code[clmax+ntmax+1];\n", parser_file);
  }
  
#endif

  fputs ("#endif /* is_lex */\n", parser_file);

#if 0
  {
    SXINT clause;

    fputs ("#if is_lex || is_multiple_pass\n", parser_file);
    fputs ("static SXINT clause2Aclause[clmax+1]={0,", parser_file);
    
    A2max_Aclause = (SXINT*) sxcalloc (max_nt+1, sizeof (SXINT));
    max_Aclause = 0;

    for (clause = 1; clause <= last_clause; clause++) {
      if (clause%16 == 1)
	fprintf (parser_file, "\n/* %ld */ ", clause);

      Aclause = clause2Aclause [clause];

      fprintf (parser_file, "%ld, ", Aclause); /* 0 marque une fin de liste */

      A = clause2lhs_nt (clause);

      if (Aclause > A2max_Aclause [A]) {
	A2max_Aclause [A] = Aclause;

	if (Aclause > max_Aclause)
	  max_Aclause = Aclause;
      }
    }

    fputs ("};\n", parser_file);

    fprintf (parser_file, "#define Gmax_Aclause %ld\n", max_Aclause);

    if (Lex_file) {
      fputs ("#if is_earley_guide\n", parser_file);

      fputs ("static SXBA_ELT A2max_Aclause [ntmax+1]={0,", parser_file);

      for (A = 1; A <= max_nt; A++) {
	if (A%10 == 1)
	  fprintf (parser_file, "\n/* %ld */ ", A);

	fprintf (parser_file, "%ld, ", A2max_Aclause [A]);
      }

      fputs ("};\n", parser_file);
      fputs ("#endif /* is_earley_guide */\n", parser_file);
    }

    sxfree (A2max_Aclause);

    fputs ("#endif /* is_lex || is_multiple_pass */\n", parser_file);
  }
#endif /* 0 */

  if (is_Lex_empty)
    sxbm_free (Lex), Lex = NULL;

  sxbm_free (clause2rhs_nt_set);
}

static void
gen_shallow (void)
{
    SXINT	clause, main_clause, prev_main_clause, i;
    
    fputs ("static SXINT from2var2clause [] = {0,", parser_file);

    prev_main_clause = 0; /* prudence */

    for (clause = 1; clause <= new_clause_nb; clause++) {
	if ((clause % 8) == 1)
	    fprintf (parser_file, "\n/* %ld */ ", (SXINT) clause);

	main_clause = from2var2clause [clause];

	if (!is_factorize) {
	  /* Si on n'est pas ds le cas factorize, chaque clause secondaire correspond a une clause
	     principale unique qui est notee en negatif */
	  if (main_clause == 0)
	    main_clause = -prev_main_clause;
	  else
	    prev_main_clause = main_clause;
	}

	fprintf (parser_file, "%ld, ", (SXINT) main_clause);
    }

    fputs ("\n};\n", parser_file);
    
    fputs ("static SXINT from2var2clause_disp [] = {0,", parser_file);

    for (clause = 1; clause <= new_clause_nb+1; clause++) {
	if ((clause % 8) == 1)
	    fprintf (parser_file, "\n/* %ld */ ", (SXINT) clause);

	fprintf (parser_file, "%ld, ", (SXINT) from2var2clause_disp [clause]);
    }

    fputs ("\n};\n", parser_file);
    
    fputs ("static SXINT from2var2clause_list [] = {0,", parser_file);

    for (i = 1; i <= from2var2clause_list_top; i++) {
	if ((i % 8) == 1)
	    fprintf (parser_file, "\n/* %ld */ ", (SXINT) i);

	fprintf (parser_file, "%ld, ", (SXINT) from2var2clause_list [i]);
    }

    fputs ("\n};\n", parser_file);
}

static void
gen_clause_id (void)
{
    /* Ds le cas is_rav2, on a besoin de connaitre l'index des memoP associe a chaque
       clause ds le cas ou le parser est un guide */
  /* ATTENTION, Le 29/5/2002, on fait disparaitre clause2Aclause */
    /* C'est different de clause2Aclause car la, on ne tient pas compte des clauses identiques */
    /* Il faut "sauter" les clauses false */
    SXINT clause, gen_clause_id_clause_id, A;
    SXBA main_clause_set;

    main_clause_set = sxba_calloc (last_clause+1);
    max_clause_id = 0;

#if 0
    if (!is_loop_grammar)
	fputs ("#if is_rav2 || is_build_coupled_guide\n", parser_file);

    fputs ("#if is_memop || is_earley_guide\n", parser_file);
#endif /* 0 */

    /* Le 11/02/04 la vraie sortie est differee et effectuee par output_clause2clause_id () */
    fputs ("static SXINT clause2clause_id [];\n", parser_file);

    for (clause = 1; clause <= last_clause; clause++) {
	if (!SXBA_bit_is_set (false_clause_set, clause)) {
	  /* Le 29/5/2002 le min de gen_clause_id_clause_id est 1
	     gen_clause_id_clause_id = A2max_clause_id [clause2lhs_nt (clause)]++; */
	  if (clause2identical [clause] == clause && !SXBA_bit_is_set (loop_clause_set, clause))
	    SXBA_1_bit (main_clause_set, clause);

	  gen_clause_id_clause_id = ++A2max_clause_id [clause2lhs_nt (clause)];
	  clause2clause_id [clause] = gen_clause_id_clause_id;

	  if (gen_clause_id_clause_id > max_clause_id)
	    max_clause_id = gen_clause_id_clause_id;
	}
    }

    sxba_to_c (main_clause_set, parser_file, "main_clause_set", "", true /* static */);

    fputs ("\nstatic SXBA_ELT A2max_clause_id [ntmax+1] = {0,", parser_file);

    for (A = 1; A <= max_nt; A++) {
	if ((A % 10) == 1)
	    fprintf (parser_file, "\n/* %ld */ ", (SXINT) A);
	    
	fprintf (parser_file, "%ld, ", (SXINT) A2max_clause_id [A]);
    }

    fputs ("};\n", parser_file);

#if 0
    fputs ("#endif /* is_memop || is_earley_guide */\n", parser_file);

    if (!is_loop_grammar)
	fputs ("#endif /* is_rav2 || is_build_coupled_guide */\n", parser_file);
#endif /* 0 */

    sxfree (main_clause_set);
}

static void
gen_for_rav2 (void)
{
    SXINT clause, x, y;

    if (is_2var_form) {
	/* On sort la structure qui permet de faire le mapping entre la rho_stack obtenue
	   par depliage (recursif) de l'analyse en forme 2var et la rho_stack necessite
	   par la grammaire d'origine */
	fputs ("#if is_rav2\n", parser_file);

	fputs ("static SXINT clause2rhs_stack_map [] = {0,", parser_file);

	for (clause = 1; clause <= last_clause; clause++) {
	    if ((clause % 8) == 1)
		fprintf (parser_file, "\n/* %ld */ ", (SXINT) clause);

	    fprintf (parser_file, "%ld, ", (SXINT) clause2rhs_stack_map [clause]);
	}

	fputs ("};\n\n", parser_file);

	fputs ("static SXINT rhs_stack_map [] = {0,", parser_file);
	y = 0;

	for (x = 1; x < rhs_stack_map_top; x++) {
	    if (y == 0)
		fprintf (parser_file, "\n/* %ld */ ", (SXINT) x);

	    y = rhs_stack_map [x];
	    fprintf (parser_file, "%ld, ", (SXINT) y);
	}

	fputs ("};\n", parser_file);

	fputs ("#endif /* is_rav2 */\n", parser_file);
    }
}

static void
gen_dynam_lex (SXINT clause/* , arity */)
{
    SXINT		lhs_prdct, gen_dynam_lex_lhs_bot2, gen_dynam_lex_lhs_top2, pos, cur2, param, bot3, top3, cur3, x, nb;
    bool	is_first = true;

    if (is_1_RCG) {
#if 0
      /* Le 31/5/2002, le cas is_guided4 est traite par le while (quand il existe) */

      SXINT	k;

      fputs ("#if is_guided4\n", parser_file);

#if 0
      /* On genere qqchose comme ca */
#if EBUG2
      if (!is_robust_run) iclause_searched_in_guide++;
#endif /* EBUG2 */
      if (
#if is_robust
	  is_robust_run ||
#endif /* is_robust */
	  coupled_clause (0, 0)) {
#if EBUG2
	if (!is_robust_run) iclause_found_in_guide++;
#endif /* EBUG2 */
#endif /* #if 0 */


	fputs ("#if EBUG2\n\
if (!is_robust_run) iclause_searched_in_guide++;\n\
#endif /* EBUG2 */\n", parser_file);

	fputs ("if (\n", parser_file);

	fputs ("#if is_robust\n\
is_robust_run ||\n\
#endif /* is_robust */\n", parser_file);

	for (k = 0; k < arity; k++) {
	  if (k > 0)
	    fputs ("\n&& ", parser_file);

	  /* Numero ds le guide de la Ak_clause produite par le kieme arg de clause  */
	  fprintf (parser_file, "coupled_clause (%ld, %ld)", k, clausek2guide_Aclause [clause_k2clausek (clause, k)]);
	}

	fputs (") {\n", parser_file);
	fputs ("#if EBUG2\n\
if (!is_robust_run) iclause_found_in_guide++;\n\
#endif/* EBUG2 */\n", parser_file);
	fputs ("#endif /* is_guided4 */\n", parser_file);
#endif /* 0 */
      }
      else {
	if (is_lex_clause) {
	  fputs ("#if is_dynam_lex\n\
if (", parser_file);

	  lhs_prdct = XxY_X (rcg_clauses, clause) & lhs_prdct_filter;
	  gen_dynam_lex_lhs_bot2 = XH_X (rcg_predicates, lhs_prdct);
	  gen_dynam_lex_lhs_top2 = XH_X (rcg_predicates, lhs_prdct+1);
	  x = clause2lhst_disp [clause];

	  for (pos = 0, cur2 = gen_dynam_lex_lhs_bot2+2; cur2 < gen_dynam_lex_lhs_top2; pos++, cur2++) {
	    param = XH_list_elem (rcg_predicates, cur2);

	    if (param > 0) {
	      nb = 0;
	      bot3 = XH_X (rcg_parameters, param);
	      top3 = XH_X (rcg_parameters, param+1);

	      for (cur3 = bot3; cur3 < top3; cur3++) {
		if (XH_list_elem (rcg_parameters, cur3) < 0) {
		  nb++;
		}
	      }

	      if (nb > 0) {
		if (is_first)
		  is_first = false;
		else
		  fputs (" && ", parser_file);

		fprintf (parser_file, "dynam_lex (ilb[%ld], iub[%ld], %ld, lhst_list+%ld)", (SXINT) pos, (SXINT) pos, (SXINT) nb, (SXINT) x);

		x += nb;
	      }
	    }
	  }

	  fputs (") {\n\
#endif\n", parser_file);
	}
      }
#if 0
    }
#endif
}


static void
gen_first_last_pp (void)
{
    /* Pour chaque clause k qui a un predicat predefini &first ou &last en position i on
       genere le SXBA first|last_k_i */
    SXINT		clause, rhs, top, bot, rhs_prdct_pos, cur, prdct, bot2, nt, param, bot3, cur3, top3, symb;
    char	istr [40];
    SXBA	t_set;

    fputs ("\n/************** U S E R ' S   F I R S T   &   L A S T **********************/\n", parser_file);

    t_set = sxba_calloc (max_t+1);

    clause = 0;

    while ((clause = sxba_scan (clause_set, clause)) > 0) {
	rhs = XxY_Y (rcg_clauses, clause);
	top = XH_X (rcg_rhs, rhs+1);
	bot = XH_X (rcg_rhs, rhs);
		    
	for (rhs_prdct_pos = 1, cur = bot; cur < top; rhs_prdct_pos++, cur++) {
	    prdct = XH_list_elem (rcg_rhs, cur);
	    bot2 = XH_X (rcg_predicates, prdct);
	    nt = XH_list_elem (rcg_predicates, bot2+1);

	    if (nt == FIRST_ic || nt == LAST_ic) {
		sprintf (istr, "%ld_%ld", (SXINT) clause, (SXINT) rhs_prdct_pos);

		param = XH_list_elem (rcg_predicates, bot2+2);

		if (param) {
		    bot3 = XH_X (rcg_parameters, param);
		    top3 = XH_X (rcg_parameters, param+1);

		    for (cur3 = bot3; cur3 < top3; cur3++) {
			symb = -XH_list_elem (rcg_parameters, cur3);
			SXBA_1_bit (t_set, symb);
		    }
		}

		if (nt == FIRST_ic)
		    sxba_to_c (t_set, parser_file, "first_", istr, true /* static */);
		else
		    sxba_to_c (t_set, parser_file, "last_", istr, true /* static */);

		if (param)
		    sxba_empty (t_set);
	    }
	}
    }

    sxfree (t_set);
}


static void
gen_first_last (void)
{
    SXINT 	Ak, A, k;
    SXBA	t_set;
    char	istr [20];

    fputs ("\n#if is_first_last\n", parser_file);

    fputs ("/************** F I R S T   &   L A S T **********************/", parser_file);

    for (Ak = 1; Ak <= max_Ak; Ak++) {
	A = Ak2A (Ak);

	if (SXBA_bit_is_set (is_nt_internal, A)) {
	    /* On ne s'occupe que des vrais internes ... */
	    k = Ak_A2k (Ak, A);
	    sprintf (istr, "%ld", (SXINT) Ak);
	    fputs ("\n/* ", parser_file);
	    fprintf (parser_file, "%s [%ld] */", sxstrget (nt2ste [A]), (SXINT) k+1);

	    if (nt2identical [A] == A) {
		/* ... principaux */
		t_set = first [Ak];

		if (!(SXBA_bit_is_set (is_first_complete, Ak) /* T ou T U {\epsilon} */ ||
		      (sxba_cardinal (t_set) == 1 && SXBA_bit_is_set (t_set, 0)) /* {\epsilon} */))
		    sxba_to_c (t_set, parser_file, "first_", istr, true /* static */);
	
		t_set = last [Ak];

		if (!(SXBA_bit_is_set (is_last_complete, Ak) /* T ou T U {\epsilon} */ ||
		      (sxba_cardinal (t_set) == 1 && SXBA_bit_is_set (t_set, 0)) /* {\epsilon} */))
		    sxba_to_c (t_set, parser_file, "last_", istr, true /* static */);
	    }
	}
    }

    fputs ("\n#endif\n", parser_file);
}


#if is_memop
static SXINT	gmax;
#endif /* is_memop */

static SXINT	gcmax;
#if 0
static bool	has_XxYxZ;
#endif /* 0 */


static void
gen_dynamic_lexicalization_decl (void)
{
    fputs ("\n/************** D Y N A M I C   L E X I C A L I Z A T I O N   S T R U C T U R E S **********************/\n", parser_file);
      
    fprintf (parser_file, "#if is_dynamic_lexicalization\n\
static SXBA *Ai2inside_t_set;\n\
static SXBA_CST_ALLOC (dynamic_lexicalization_Ai_set, %ld);\n\
#endif /* is_dynamic_lexicalization */\n", (SXINT) (A_k2Ak(max_nt, nt2arity [max_nt]-1)+1));
}


static void
gen_nl_decl (void)
{
    SXINT A;

    fputs ("\n/************** N O N   L I N E A R   S T R U C T U R E S **********************/\n", parser_file);
      
    fprintf (parser_file, "#if is_dag && !is_a_linear_module\n\
static SXBA_CST_ALLOC (nl_Ai_set, %ld);\n\
static void nl_Ai_processing (void);\n\
static SXBA_CST_ALLOC (basic_lexicalization_clause_set, %ld);\n",
	     (SXINT) (A_k2Ak(max_nt, nt2arity [max_nt]-1)+1), (SXINT) (last_clause+1));

    sxba_to_c (non_right_linear_clause_set, parser_file, "non_right_linear_clause_set", "", true /* static */);


    fputs ("#endif /* is_dag && !is_a_linear_module */\n\n", parser_file);

    fputs ("static SXINT A2A0 [ntmax+1] = {0,\n", parser_file);
    
    for (A = 1; A <= max_nt; A++) {
	fprintf (parser_file, "/* %s = %ld */ ", sxstrget (nt2ste [A]), (SXINT)  A);

	if (SXBA_bit_is_set (is_lhs_nt, A) && nt2identical [A] == A) {
	    fprintf (parser_file, "%ld,\n", A_k2Ak (A, 0));
	}
	else {
	    fputs ("0 /* external predicate */,\n", parser_file);
	}
    }

    fputs ("} /* end A2A0 */;\n", parser_file);
}


static void
gen_terminal_memo_decl (void)
{
    fputs ("\n/************** T E R M I N A L   M E M O   S T R U C T U R E S **********************\n\
 ************** (This module is a \"terminal\" grammar) **********************/\n", parser_file);

#if 0
    fputs ("static SXBA\t*memoN, memoN0, memoN1;\n", parser_file);
#endif /* 0 */
    fputs ("static SXBA\t*memoN;\n", parser_file);
    fputs ("static SXINT\t*local_Aij2global_Aij_pid;\n", parser_file);
    fputs ("static X_header\t\trhoA_hd;\n", parser_file);

    fprintf (parser_file, "#if %ld < SXT_End_Of_File\n", max_t+1);
    fputs ("/* Il y a des terminaux qui sont definis dans un autre module */\n", parser_file);
    fputs ("static SXINT global_t2local_t [SXT_End_Of_File+1];\n", parser_file);
    fprintf (parser_file, "#endif /* %ld < SXT_End_Of_File */\n", max_t+1);
}


static void
gen_memo_decl (void)
{
#if 0
    static char *header [5] = {"", "X", "XxY", "XxYxZ", "XH"};
    static char *nb [5] = {"", "1", "2", "3", "n"};

    SXINT 	arity, class;
#endif /* 0 */

    fputs ("\n/************** M E M O   S T R U C T U R E S **********************/\n", parser_file);

    fputs ("static SXBA\t*memoN;\n", parser_file);
    fputs ("static SXINT\t*local_Aij2global_Aij_pid;\n", parser_file);
#if is_memop
    fputs ("static SXBA\t*memoP;\n", parser_file);
#endif /* is_memop */

    fputs ("#if is_guided\n", parser_file);
    fputs ("static SXBA\ttcmemoN1, *ptcmemoP;\n", parser_file);
    fputs ("static X_header\t*ptcrange_1;\n", parser_file);
    fputs ("static X_header\t*ptcrhoA_hd;\n", parser_file);
    fputs ("#endif\n", parser_file);

    fputs ("#if AG\n\
static SXINT	*memoret_val;\n\
#endif\n\
", parser_file);

    fputs ("\n", parser_file);

#if 0
    has_XxYxZ = false;
    arity = class = 0;

    while ((arity = sxba_scan (arity_set, arity)) > 0 && class != 4) {
	if (arity >= 4) {
	    class = 4;
	}
	else
	    class = arity;

	if (arity == 3)
	    has_XxYxZ = true;

	if (arity == 1)
	    fputs ("static X_header\t\trange_1;\n", parser_file);
	else
	    fprintf (parser_file, "static %s_header\trange_%s;\n", header [class], nb [class]);
    }
#endif /* 0 */

    fputs ("static X_header\t\trhoA_hd;\n", parser_file);
}

static void
gen_memo_assign (void)
{

  fputs ("\nstatic void\n\
memo_assign ()\n\
{\n", /* " */parser_file);

 if (!is_a_complete_terminal_grammar) { 
   fputs ("memoN0 = memoN [0];\n\
  memoN1 = memoN [1];\n", parser_file);
 }

 fputs ("  G.pmemoN = memoN;\n\
  G.local_Aij2global_Aij_pid = local_Aij2global_Aij_pid;\n", parser_file);

#if is_memop
 fputs ("\
#if is_memop\n\
G.pmemoP = memoP;\n\
#endif /* is_memop */\n", parser_file);
#endif /* is_memop */

 if (is_cyclic_grammar) {
#if is_memon==0
   fputs ("\
#if is_cyclic\n\
memoC = memoN+2;\n\
#endif /* is_cyclic */\n", parser_file);
#else/* is_memon==0 */
   fputs ("\
#if is_memon && is_cyclic\n\
memoC = memoN+2;\n\
#endif /* is_memon && is_cyclic */\n", parser_file);
#endif /* is_memon==0 */
 }

 fputs ("}\n", parser_file);
}

static void
gen_memo_oflw (void)
{
  fputs ("\n\
static void\n\
memo_oflw (SXINT old_size, SXINT new_size)\n\
{\n", parser_file);

#if is_memop
  fputs ("\
#if is_memop\n\
  if (memoP) memoP = sxbm_resize (memoP, old_size+1, new_size+1, G.memoP_size);\n\
#endif /* is_memop */\n", parser_file);
#endif /* is_memop */

#if is_memon==1
  fputs ("#if is_memon\n\n", parser_file);
#endif /* is_memon==1 */

 fputs ("  memoN = sxbm_resize (memoN, G.memoN_size, G.memoN_size, new_size+1);\n", parser_file);
 
 fputs ("  local_Aij2global_Aij_pid = (SXINT*) sxrealloc (local_Aij2global_Aij_pid, new_size+1, sizeof (SXINT));\n", parser_file);

 if (!is_a_complete_terminal_grammar)
  fputs ("#if AG\n\
  memoret_val = (SXINT*) sxrealloc (memoret_val, new_size+1, sizeof (SXINT));\n\
#endif /* AG */\n", parser_file);

#if is_memon==1
  fputs ("#endif /* is_memon */\n", parser_file);
#endif /* is_memon==1 */

#if is_memop
  fputs ("\
#if is_memon || is_memop\n\
  memo_assign ();\n\
#endif /* is_memon || is_memop */\n", parser_file);
#endif /* is_memop */

#if is_memon==1
  fputs ("\
#if is_memon\n\
  memo_assign ();\n\
#endif /* is_memon */\n", parser_file);
#else
  fputs ("  memo_assign ();\n", parser_file);
#endif /* is_memon==1 */


#if is_semantics==1
  fputs ("#if is_semantics\n", parser_file);
#endif /* is_semantics==1 */

  fputs ("  if (*G.semact.oflw) (*G.semact.oflw) (&G, old_size, new_size);\n", parser_file);

#if is_semantics==1
  fputs ("#endif /* is_semantics */\n", parser_file);
#endif /* is_semantics==1 */
    
  fputs ("}\n\n", parser_file);
}


static void
gen_memo_fun (void)
{
    SXINT		A, clause, arity, class, cur, rhs, top, bot, son_nb, prdct, bot2, nt, max;
    bool	has_ext;
    SXBA	gen_memo_fun_A_clause_set;

    fputs ("\n#if !is_grammar_alone\n", parser_file);

#if is_memon==1
    fputs ("#if is_memon\n", parser_file);
#endif /* is_memon==1 */

    fputs ("static SXBA\tmemoN0, memoN1;\n", parser_file);

    gcmax = 0;

#if is_memop
    /* Le 29/5/2002 */
    gmax = max_clause_id; 
#endif /* is_memop */

    for (A = 1; A <= max_nt; A++) {
	if (SXBA_bit_is_set (is_lhs_nt, A) && nt2identical [A] == A) {
	    /* Le Ven 17 Nov 2000 17:30:30 : on revient au cardinal initial car les index des memoP
	       peuvent servir ds un parseur guide' (qui ignore les identites detectees ds le guide) */
	    gen_memo_fun_A_clause_set = lhsnt2clause [A];
	    /*
	      Le 29/5/2002
	      max = sxba_cardinal (gen_memo_fun_A_clause_set);

	      if (max > gmax)
		gmax = max;
	    */

	    max = clause = 0;

	    while ((clause = sxba_scan (gen_memo_fun_A_clause_set, clause)) > 0) {
	      if ((is_lex_clause = is_lexicalized (clause)))
		    SXBA_1_bit (lexicalized_clause_set, clause);

		if (SXBA_bit_is_set (recursive_clause_set, clause)) {
		    if (is_combinatorial || (!is_lex_clause && !SXBA_bit_is_set (loop_clause_set, clause) && is_cyclic_clause_fun (clause))) {
			SXBA_1_bit (cyclic_clause_set, clause);
			SXBA_1_bit (cyclic_nt_set, A);
			is_cyclic_grammar = true;
			max++;
		    }
		}
	    }

	    if (max > gcmax)
		gcmax = max;
	}
    }

    if (is_cyclic_grammar || is_loop_grammar) {
      /* Bidon ds le cas is_loop_grammar */
	fputs ("#if is_cyclic\n", parser_file);
	fputs ("static bool is_glbl_cycle;\n", parser_file);
	fputs ("static SXBA\t*memoC;\n", parser_file);
	fputs ("#endif /* is_cyclic */\n", parser_file);
    }

#if is_memon==1
    fputs ("#endif /* is_memon */\n", parser_file);
#endif /* is_memon==1 */

    gen_memo_assign ();

    if (is_2var_form) {
	fputs ("\n#if is_rav2\n", parser_file);

	fprintf (parser_file, "static void\n\
%s_first_pass_init (struct G *pG)\n\
{\n\
SXINT l;\n", PRGENTNAME(false));
	
	fputs ("G.prhoA_hd = pG->prhoA_hd;\n\
l = X_size (*G.prhoA_hd);\n", parser_file);
    
	arity = class = 0;

	while ((arity = sxba_scan (arity_set, arity)) > 0 && class != 4) {
	    if (arity >= 4) {
		class = 4;
	    }
	    else
		class = arity;

	    switch (class) {
	    case 1:
		/* X */
		fputs ("#if is_large\n", parser_file);
		fputs ("G.prange_1 = pG->prange_1;\n", parser_file);
		fputs ("#endif\n", parser_file);

		break;

	    case 2:
		/* XxY */
		fputs ("G.prange_2 = pG->prange_2;\n", parser_file);
		break;

	    case 3:
		/* XxYxZ */
		fputs ("G.prange_3 = pG->prange_3;\n",
		       parser_file);
		break;

	    case 4:
		/* XH */
		fputs ("G.prange_n = pG->prange_n;\n", parser_file);
		break;
	    default:
#if EBUG
	      sxtrap(ME,"unknown switch case #9");
#endif
	      break;
	    }
	}

#if is_memop
	fputs ("#if is_memop\n", parser_file);
	fprintf (parser_file, "memoP = sxbm_calloc (l+1, G.memoP_size=%ld);\n", gmax+1);
	fputs ("G.pmemoP = memoP;\n", parser_file);
	fputs ("#endif /* is_memop */\n", parser_file);   
#endif /* is_memop */

#if is_memon==1
	fputs ("#if is_memon\n", parser_file);
#endif /* is_memon==1 */
	fputs ("memoN = sxbm_calloc (2, l+1);\n", parser_file);
	fputs ("G.pmemoN = memoN;\n", parser_file);
#if is_memon==1
	fputs ("#endif\n", parser_file);
#endif /* is_memon==1 */

#if is_semantics==1
	fputs ("#if is_semantics\n", parser_file);
#endif /* is_semantics==1 */

	fputs ("(*G.semact.first_pass_init) (&G, 0, l);\n", parser_file);

#if is_semantics==1
	fputs ("#endif /* is_semantics */\n", parser_file);
#endif /* is_semantics==1 */

	fputs ("}\n\n", parser_file);


	fprintf (parser_file, "static void\n\
%s_first_pass_final ()\n\
{\n", PRGENTNAME(false));

#if is_semantics==1
	fputs ("#if is_semantics\n", parser_file);
#endif /* is_semantics==1 */

	fputs ("/* L'appel suivant de semact.first_pass_final sera fait + tard directement depuis RCG_parser\n\
(*G.semact.first_pass_final) (&G, 1, 0); */\n", parser_file);

#if is_semantics==1
	fputs ("#endif /* is_semantics */\n", parser_file);
#endif /* is_semantics==1 */

#if is_memon==1
	fputs ("#if is_memon\n", parser_file);
#endif /* is_memon==1 */
	/* Ds le cas is_tight_coupling, memo, rhoA_hd et range_1 ne sont pas liberes par ici, finesse! */
	fputs ("if (G.pmemoN) sxbm_free (memoN), G.pmemoN = memoN = NULL;\n", parser_file);
#if is_memon==1
	fputs ("#endif\n", parser_file);
#endif /* is_memon==1 */

#if is_memop
	fputs ("#if is_memop\n", parser_file);
	fputs ("if (G.pmemoP) sxbm_free (memoP), G.pmemoP = memoP = NULL;\n", parser_file);
	fputs ("#endif /* is_memop */\n", parser_file);  
#endif /* is_memop */

#if is_memon==1
	fputs ("#if is_memon || is_semantics\n", parser_file);
#endif /* is_memon==1 */

        fputs ("/* La liberation suivante de G.prhoA_hd sera faite + tard directement depuis RCG_parser \n\
if (G.prhoA_hd && X_is_allocated (*G.prhoA_hd)) X_free (G.prhoA_hd); G.prhoA_hd = NULL; */\n", parser_file);
  
	arity = class = 0;

	while ((arity = sxba_scan (arity_set, arity)) > 0 && class != 4) {
	    if (arity >= 4) {
		class = 4;
	    }
	    else
		class = arity;

	    switch (class) {
	    case 1:
		/* X */
		fputs ("#if is_large\n", parser_file);
	        fputs ("if (G.prange_1 && X_is_allocated (*G.prange_1)) X_free (G.prange_1); G.prange_1 = NULL;\n", parser_file);
		fputs ("#endif\n", parser_file);

		break;

	    case 2:
		/* XxY */
	        fputs ("if (G.prange_2 && XxY_is_allocated (*G.prange_2)) XxY_free (G.prange_2); G.prange_2 = NULL;\n", parser_file);
		break;

	    case 3:
		/* XxYxZ */
	        fputs ("if (G.prange_3 && XxYxZ_is_allocated (*G.prange_3)) XxYxZ_free (G.prange_3); G.prange_3 = NULL;\n", parser_file);
		break;

	    case 4:
		/* XH */
	        fputs ("if (G.prange_n && XH_is_allocated (*G.prange_n)) XH_free (G.prange_n); G.prange_n = NULL;\n", parser_file);
		break;
           default:
#if EBUG
                sxtrap(ME,"unknown switch case #10");
#endif
                break;
	    }
	}

#if is_memon==1
	fputs ("#endif /* is_memon || is_semantics */\n", parser_file);
#endif /* is_memon==1 */

	fputs ("}\n", parser_file);
	fputs ("#else /* !is_rav2 */\n", parser_file);
    }	

 gen_memo_oflw ();



#if 0
    fputs ("#if is_multiple_pass\n", parser_file);
    fprintf (parser_file, "static void\n\
%s_second_pass_init ()\n\
{\n", PRGENTNAME(false));
    fputs ("G.pass_no = 2;\n", parser_file);
    /* Il faut remettre a zero memoN1 et les memoP */
#if is_memon==1
    fputs ("#if is_memon\n", parser_file);
#endif /* is_memon==1 */

    fputs ("if (G.pmemoN) sxbm_free (memoN);\n", parser_file);

    if (is_cyclic_grammar) {
       fputs ("memoN = sxbm_calloc (G.memoN_size =\n\
#if is_cyclic\n\
6,\n\
#else /* !is_cyclic */\n\
2,\n\
#endif /* !is_cyclic */\n\
X_top (rhoA_hd)+1);\n", parser_file);
    }
    else {
      fputs ("memoN = sxbm_calloc (2, X_top (rhoA_hd)+1);\n", parser_file);
    }

    fputs ("if (G.local_Aij2global_Aij_pid) /* inutile en seconde passe */sxfree (local_Aij2global_Aij_pid), local_Aij2global_Aij_pid = NULL;\n", parser_file);

#if is_memon==1
    fputs ("#endif /* is_memon */\n", parser_file);
#endif /* is_memon==1 */

#if is_memop
    fputs ("#if is_memop\n", parser_file);
    fprintf (parser_file, "memoP = sxbm_calloc (X_top (rhoA_hd)+1, %ld);\n", gmax+1);
    fputs ("#endif /* is_memop */\n", parser_file);
#endif /* is_memop */

#if is_memop
    fputs ("\
#if is_memon || is_memop\n\
memo_assign ();\n\
#endif /* is_memon || is_memop */\n", parser_file);
#endif /* is_memop */

#if is_memon==1
    fputs ("\
#if is_memon\n\
memo_assign ();\n\
#endif /* is_memon */\n", parser_file);
#else
    fputs ("memo_assign ();\n", parser_file);
#endif /* is_memon==1 */

#if is_semantics==1
    fputs ("#if is_semantics\n", parser_file);
#endif /* is_semantics==1 */

    fputs ("if (*G.semact.second_pass_init) (*G.semact.second_pass_init) (&G, 0, X_top (*G.prhoA_hd)+1);\n", parser_file);

#if is_semantics==1
    fputs ("#endif /* is_semantics */\n", parser_file);
#endif /* is_semantics==1 */

    fputs ("}\n", parser_file);
    
    fprintf (parser_file, "\nstatic void\n\
%s_second_pass_final ()\n\
{\n", PRGENTNAME(false));

#if is_semantics==1
    fputs ("#if is_semantics\n", parser_file);
#endif /* is_semantics==1 */

    fputs ("(*G.semact.second_pass_final) (*G.semact.second_pass_final) (&G, 1, 0);\n", parser_file);

#if is_semantics==1
    fputs ("#endif\n", parser_file);
#endif /* is_semantics==1 */

#if is_memon==1
    fputs ("#if is_memon\n", parser_file);
#endif /* is_memon==1 */

    fputs ("if (G.pmemoN) sxbm_free (memoN), G.pmemoN = NULL, memoN = NULL;\n", parser_file);
    fputs ("if (G.local_Aij2global_Aij_pid) /* inutile en seconde passe */sxfree (local_Aij2global_Aij_pid), G.local_Aij2global_Aij_pid = local_Aij2global_Aij_pid = NULL;\n", parser_file);

#if is_memon==1
    fputs ("#endif /* is_memon */\n", parser_file);
#endif /* is_memon==1 */

#if is_memop
    fputs ("#if is_memop\n", parser_file);
    fputs ("if (G.pmemoP) sxbm_free (memoP), G.pmemoP = NULL, memoP = NULL;\n", parser_file);
    fputs ("#endif /* is_memop */\n", parser_file);
#endif /* is_memop */

    fputs ("}\n", parser_file);
    fputs ("#endif /* is_multiple_pass */\n", parser_file);
#endif /* 0 */
    
    fprintf (parser_file, "\nstatic void\n\
%s_first_pass_init ()\n\
{\n\
SXINT l;\n\
#if EBUG\n\
bool arity_check = true;\n\
#endif /* EBUG */\n", PRGENTNAME(false));

    fputs ("#if is_dag\n\
SS_stack = SS_alloc (50);\n\
#endif /* is_dag */\n", parser_file);
    

#if 0
    if (has_XxYxZ)
	fputs ("static SXINT memo3_assoc [6];\n\n", parser_file);
#endif /* 0 */

    fputs ("#if 0\n\
/* Adaptation a udag_scanner */\n\
#if AG\n\
    fill_idag (G.pid, &idag);\n\
    bag_alloc (&bag_hd, \"bag_hd\", n+1);\n\
#else\n\
    if (G.pid == 0 /* main module */) {\n\
	source = glbl_source;\n\
	lsource = glbl_source+1;\n\
    }\n\
    else {\n\
#if is_sdag\n\
        source = (SXBA*) sxbm_calloc (n+2, tmax+1);\n\
#else /* !is_dag */\n\
        source = (SXINT*) sxalloc (n+2, sizeof (SXINT));\n\
#endif /* !is_dag */\n\
        lsource = source+1;\n\
        fill_buf (G.pid, source, t2str, tmax);\n\
    }\n\
", parser_file);

    if (is_source_checked) {
	/* Si on n'accede jamais au source, on n'en a pas besoin */
	fputs ("else {\n\
#if is_sdag\n\
source = (SXBA*) sxbm_calloc (n+2, tmax+1);\n\
#else\n\
source = (SXINT*) sxalloc (n+2, sizeof (SXINT));\n\
#endif\n\
lsource = source+1;\n\
fill_buf (G.pid, source, t2str, tmax);\n\
}\n", parser_file);
    }

    fputs ("#endif /* AG */\n", parser_file);

    /* Modif le Mer 28 Jun 2000 10:45:46 */
    fputs ("#if is_lex && !is_guided && !is_earley_guide\n\
lexicalization (&G);\n\
#endif /* is_lex && !is_guided && !is_earley_guide */\n\
#endif /* 0 */\n", parser_file);

    if (has_ext_decl) {
	A = 0;

	fputs ("#if EBUG\n", parser_file);

	while ((A = sxba_scan (ext_decl_set, A)) > 0) {
	    clause = ext_decl2clause [A];
	    arity = nt2arity [A];
	    fprintf (parser_file, "if (%s_arity != %ld) {\n", sxstrget (nt2ste [A]), (SXINT) arity);
	    /* fprintf (parser_file, "is_useful_%ld = false;\n", (SXINT) clause); */
	    fprintf (parser_file, "arity_check = false;\n\
arg_nb_put_error (\"&%s\", %s_arity, %ld);\n",
		     sxstrget (nt2ste [A]), sxstrget (nt2ste [A]), (SXINT) arity);
	    fputs ("}\n", parser_file);
	}

	fputs ("if (!arity_check)\n\
sxtrap (ME, \"RCG_first_pass_init\");\n", parser_file);
        fputs ("#endif /* EBUG */\n", parser_file);
    }

    fputs ("#if is_guided\n", parser_file);

    fputs ("#if is_guided2 || is_guided3 || is_guided4\n", parser_file);
    fputs ("if (G.gpmemoN == NULL && G.gpmemoP == NULL || X_size (*G.gprhoA_hd) == 0\n", parser_file);
    fputs ("#if is_large\n", parser_file);
    fputs ("||  X_size (*G.gprange_1) == 0\n", parser_file);
    fputs ("#endif\n", parser_file);
    fputs ("    ) {\n", parser_file);
    fputs ("    extern FILE *sxtty;\n", parser_file);
    fputs ("    fprintf (sxtty, \"The compilation options of the guided parser #%ld are incompatible with the current guide.\\n\", G.pid);\n", parser_file);
    fputs ("    sxexit(1);\n", parser_file);
    fputs ("}\n", parser_file);
    fputs ("#endif /* is_guided2 || is_guided3 || is_guided4 */\n", parser_file);

    fputs ("if (G.gpmemoN) tcmemoN1 = (G.gpmemoN)[1]; /* Acces aux predicats instancies du guide (s'il y en a) */\n", parser_file);
    /* Le 30/5/2002, clause_id min == 1 memoP separes de memoN */
    fputs ("ptcmemoP = G.gpmemoP; /* Acces aux clauses instanciees du guide */\n", parser_file);
    fputs ("ptcrange_1 = (G.gprange_1); /* Acces aux rho du guide */\n", parser_file);
    fputs ("ptcrhoA_hd = (G.gprhoA_hd); /* Acces aux ntXrho du guide */\n", parser_file);
    fputs ("l = X_top (*ptcrhoA_hd);\n", parser_file);
    fputs ("#endif /* is_guided */\n", parser_file);

#if is_memon==1
    fputs ("#if is_memon || is_semantics\n", parser_file);
#endif /* is_memon==1 */

    /*  Le 22/5/2002 */
    if (Lex_file) {
        fputs ("#if is_earley_guide\n\
	  rhoA_hd = *(G.gprhoA_hd) /* for X_free */, G.prhoA_hd = &rhoA_hd, l = X_top (rhoA_hd);\n\
rhoA_hd.user_oflw = (ifun) memo_oflw /* prudence */;\n\
#if is_large\n\
range_1 = *(G.gprange_1), G.prange_1 = &range_1;\n\
#endif\n\
#else /* !is_earley_guide */\n", parser_file);
    }
    
    fputs ("#if !is_guided\n", parser_file);

    fputs ("l = idag.final_state*idag.final_state; if (l > 4096) l = 4096;\n", parser_file);
    fputs ("#endif\n", parser_file);

    fputs ("X_alloc (&rhoA_hd, \"rhoA_hd\", l, 1, memo_oflw, statistics), G.prhoA_hd = &rhoA_hd;\n", parser_file);
    fputs ("l = X_size (rhoA_hd);\n", parser_file);
    
    arity = class = 0;

    while ((arity = sxba_scan (arity_set, arity)) > 0 && class != 4) {
	if (arity >= 4) {
	    class = 4;
	}
	else
	    class = arity;

	switch (class) {
	case 1:
	    /* X */
	    fputs ("#if is_large\n", parser_file);
	    fputs ("arity_eq_1_is_used = true, G.prange_1 = &arity_eq_1;\n", parser_file);
#if 0
	    fputs ("X_alloc (&range_1, \"range_1\", l/2, 1, NULL, statistics), G.prange_1 = &range_1;\n", parser_file);
#endif /* 0 */
	    fputs ("#endif\n", parser_file);

	    break;

	case 2:
	    /* XxY */
	    fputs ("arity_eq_2_is_used = true, G.prange_2 = &arity_eq_2;\n", parser_file);
#if 0
	    fputs ("XxY_alloc (&range_2, \"range_2\", l, 1, 0, 0, NULL, statistics), G.prange_2 = &range_2;\n", parser_file);
#endif /* 0 */
	    break;

	case 3:
	    /* XxYxZ */
	    fputs ("arity_eq_3_is_used = true, G.prange_3 = &arity_eq_3;\n", parser_file);
#if 0
	    fputs ("XxYxZ_alloc (&range_3, \"range_3\", (3*l)/2, 1, memo3_assoc, NULL, statistics), G.prange_3 = &range_3;\n",
		   parser_file);
#endif /* 0 */
	    break;

	case 4:
	    /* XH */
	    fputs ("arity_geq_4_is_used = true, G.prange_n = &arity_geq_4;\n", parser_file);
#if 0
	    fputs ("XH_alloc (&range_n, \"range_n\", 2*l, 1, 4, NULL, statistics), G.prange_n = &range_n;\n", parser_file);
#endif /* 0 */
	    break;
        default:
#if EBUG
            sxtrap(ME,"unknown switch case #11");
#endif
             break;
	}
    }

    /*  Le 22/5/2002 */
    if (Lex_file) {
        fputs ("#endif /* !is_earley_guide */\n", parser_file);
    }

#if is_memon==1
    fputs ("#endif /* is_memon || is_semantics */\n", parser_file);
#endif /* is_memon==1 */

#if is_memon==1
    fputs ("#if is_memon\n", parser_file);
#endif /* is_memon==1 */

    if (is_cyclic_grammar) {
       fputs ("memoN = sxbm_calloc (G.memoN_size =\n\
#if is_cyclic\n\
6,\n\
#else /* !is_cyclic */\n\
2,\n\
#endif /* !is_cyclic */\n\
l+1);\n", parser_file);
}
    else {
       fputs ("memoN = sxbm_calloc (G.memoN_size = 2, l+1);\n", parser_file);
    }

    fputs ("local_Aij2global_Aij_pid = (SXINT*) sxalloc (l+1, sizeof (SXINT)), local_Aij2global_Aij_pid [0] = 0;\n", parser_file);

    fprintf (parser_file, "#if is_dynamic_lexicalization\n\
G.Ai2inside_t_set = Ai2inside_t_set = sxbm_calloc (%ld, idag.source_set_cardinal+1);\n\
#endif /* is_dynamic_lexicalization */\n", (SXINT) (A_k2Ak(max_nt, nt2arity [max_nt]-1)+1));


    fputs ("#if AG\n\
memoret_val = (SXINT*) sxalloc (l+1, sizeof (SXINT));\n\
#endif\n", parser_file);

#if is_memon==1
    fputs ("#endif /* is_memon */\n", parser_file);
#endif /* is_memon==1 */

#if is_memop
    fputs ("#if is_memop && is_single_pass\n", parser_file);
    fprintf (parser_file, "memoP = sxbm_calloc (l+1, G.memoP_size = %ld);\n", gmax+1);
    fputs ("#endif /* is_memop && is_single_pass */\n", parser_file);

    fputs ("#if is_memon || is_memop && is_single_pass\n", parser_file);
    fputs ("memo_assign ();\n", parser_file);
    fputs ("#endif /* is_memon || is_memop && is_single_pass */\n", parser_file);
#endif /* is_memop */

#if is_memon==1
    fputs ("#if is_memon\n", parser_file);
#endif /* is_memon==1 */

    fputs ("memo_assign ();\n", parser_file);

#if is_memon==1
    fputs ("#endif /* is_memon */\n", parser_file);
#endif /* is_memon==1 */

    fputs ("#if 0\n\
/* Inutile, supprime' le 31/07/08 */\n\
#if is_semantics\n", parser_file);
    fputs ("if (G.pid) {\n", parser_file);

    for (cur = 0; cur <= max_rhs_prdct; cur++)
	fprintf (parser_file, "sons_0 [%ld] = ", (SXINT) cur);

    fputs ("G.pid;\n", parser_file);

    has_ext = false;
    clause = 0;

    while ((clause = sxba_scan (clause_has_ext_decl, clause)) > 0) {
	fprintf (parser_file, "sons_%ld [0]", (SXINT) clause);

	rhs = XxY_Y (rcg_clauses, clause);
	top = XH_X (rcg_rhs, rhs+1);
	bot = XH_X (rcg_rhs, rhs);
	son_nb = 0;

	for (cur = bot; cur < top; cur++) {
	    prdct = XH_list_elem (rcg_rhs, cur);
	    bot2 = XH_X (rcg_predicates, prdct);

	    if (XH_list_elem (rcg_predicates, bot2) == 0) {
		/* Positive call */
		nt = XH_list_elem (rcg_predicates, bot2+1);

		if (nt > 0) {
		    son_nb++;

		    if (SXBA_bit_is_set (is_lhs_nt, nt))
			fprintf (parser_file, " = sons_%ld [%ld]", (SXINT) clause, (SXINT) son_nb);
		    else
			has_ext = true;
		}
		else {
		    if ((is_drcg && nt == STREQ_ic && is_streq_spcl (bot2+2)) || nt == TRUE_ic) {
			/* Le rho correspondant est occupe */
			/* mais la valeur du local-pid est sans object ou a deja ete positionnee a` -1 */
			son_nb++;
		    }
		}
	    }
	}

	fputs (" = G.pid;\n", parser_file);
	
    }

    fputs ("}\n", parser_file);

    if (has_ext) {
	fputs ("\n", parser_file);

	clause = 0;

	while ((clause = sxba_scan (clause_has_ext_decl, clause)) > 0) {
	    rhs = XxY_Y (rcg_clauses, clause);
	    top = XH_X (rcg_rhs, rhs+1);
	    bot = XH_X (rcg_rhs, rhs);
	    son_nb = 0;

	    for (cur = bot; cur < top; cur++) {
		prdct = XH_list_elem (rcg_rhs, cur);
		bot2 = XH_X (rcg_predicates, prdct);

#if 0
                /* Le 24/05/04 Comme les appels negatifs figurent ds "rho", les sons correspondant doivent
                   recevoir les pid. */
		if (XH_list_elem (rcg_predicates, bot2) == 0) {
		    /* Positive call */
#endif /* 0 */
		    nt = XH_list_elem (rcg_predicates, bot2+1);

		    if (nt > 0) {
			son_nb++;
		
			if (!SXBA_bit_is_set (is_lhs_nt, nt))
			    fprintf (parser_file, "sons_%ld [%ld] = %s_pid;\n", (SXINT) clause, (SXINT) son_nb, sxstrget (nt2ste [nt]));
		    }
		    else {
			if ((is_drcg && nt == STREQ_ic && is_streq_spcl (bot2+2)) || nt == TRUE_ic) {
			    /* Le rho correspondant est occupe */
			    /* mais la valeur du local-pid est sans object ou a deja ete positionnee a` -1 */
			    son_nb++;
			}
		    }
#if 0
		}
#endif /* 0 */
	    }
	}
    }

    fputs ("#endif /* is_semantics */\n\
#endif /* 0 */\n", parser_file);

#if is_semantics==1
    fputs ("#if is_semantics\n", parser_file);
#endif /* is_semantics==1 */

    fputs ("(*G.semact.first_pass_init) (&G, 0, l);\n", parser_file);

#if is_semantics==1
    fputs ("#endif\n", parser_file);
#endif /* is_semantics==1 */

    fputs ("}\n\n", parser_file);

    fprintf (parser_file, "static void\n\
%s_first_pass_final ()\n\
{\n", PRGENTNAME(false));

#if is_semantics==1
    fputs ("#if is_semantics\n", parser_file);
#endif /* is_semantics==1 */

    fputs ("/* L'appel suivant de semact.first_pass_final sera fait + tard directement depuis RCG_parser\n\
(*G.semact.first_pass_final) (&G, 1, 0); */\n", parser_file);

#if is_semantics==1
    fputs ("#endif\n", parser_file);
#endif /* is_semantics==1 */

    fputs ("#if is_dag\n\
SS_free (SS_stack), SS_stack = NULL;\n\
#endif /* is_dag */\n", parser_file);

fputs ("#if 0\n\
/* Adaptation a udag_scanner */\n\
#if AG\n\
    free_idag (G.pid, &idag);\n\
    bag_free (&bag_hd);\n\
#else\n\
    if (G.pid != 0 /* secondary module */ && source != NULL)\n\
#if is_sdag\n\
        sxbm_free (source)\n\
#else\n\
        sxfree (source)\n\
#endif\n\
 , source = lsource = NULL;\n\
#endif\n\
#endif /* 0 */\n\
", parser_file);

#if is_memon==1
    fputs ("#if is_memon\n", parser_file);
#endif /* is_memon==1 */

    /* Ds le cas is_tight_coupling, memo, rhoA_hd et range_1 ne sont pas liberes par ici, finesse! */
    fputs ("if (G.pmemoN) sxbm_free (memoN), G.pmemoN = NULL, memoN = NULL;\n\
sxfree (local_Aij2global_Aij_pid), local_Aij2global_Aij_pid = NULL;\n", parser_file);

    fputs ("#if is_dynamic_lexicalization\n\
sxbm_free (Ai2inside_t_set), G.Ai2inside_t_set = Ai2inside_t_set = NULL;\n\
#endif /* is_dynamic_lexicalization */\n", parser_file);

    fputs ("#if AG\n\
sxfree (memoret_val);\n\
#endif\n", parser_file);

#if is_memon==1
    fputs ("#endif /* is_memon */\n", parser_file);
#endif /* is_memon==1 */

#if is_memop
    fputs ("#if is_memop\n", parser_file);
    /* Ds le cas is_tight_coupling, memo, rhoA_hd et range_1 ne sont pas liberes par ici, finesse! */
    fputs ("if (G.pmemoP) sxbm_free (memoP), G.pmemoP = NULL, memoP = NULL;\n", parser_file);
    fputs ("#endif /* is_memop */\n", parser_file);
#endif /* is_memop */

#if is_memon==1
    fputs ("#if is_memon || is_semantics\n", parser_file);
#endif /* is_memon==1 */

    fputs ("/* La liberation suivante de G.prhoA_hd sera faite + tard directement depuis RCG_parser \n\
if (G.prhoA_hd && X_is_allocated (*G.prhoA_hd)) X_free (G.prhoA_hd); G.prhoA_hd = NULL; */\n", parser_file);

#if 0
    arity = class = 0;

    while ((arity = sxba_scan (arity_set, arity)) > 0 && class != 4) {
	if (arity >= 4) {
	    class = 4;
	}
	else
	    class = arity;

	switch (class) {
	case 1:
	    /* X */
	    fputs ("#if is_large\n", parser_file);
	    fputs ("if (G.prange_1 && X_is_allocated (*G.prange_1)) X_free (G.prange_1); G.prange_1 = NULL;\n", parser_file);
	    fputs ("#endif\n", parser_file);
	    break;

	case 2:
	    /* XxY */
	    fputs ("if (G.prange_2 && XxY_is_allocated (*G.prange_2)) XxY_free (G.prange_2); G.prange_2 = NULL;\n", parser_file);
	    break;

	case 3:
	    /* XxYxZ */
	    fputs ("if (G.prange_3 && XxYxZ_is_allocated (*G.prange_3)) XxYxZ_free (G.prange_3); G.prange_3 = NULL;\n", parser_file);
	    break;

	case 4:
	    /* XH */
	    fputs ("if (G.prange_n && XH_is_allocated (*G.prange_n)) XH_free (G.prange_n); G.prange_n = NULL;\n", parser_file);
	    break;
        default:
#if EBUG
            sxtrap(ME,"unknown switch case #12");
#endif
            break;
	}
    }
#endif /* 0 */

#if is_memon==1
    fputs ("#endif /* is_memon || is_semantics */\n", parser_file);
#endif /* is_memon==1 */

    fputs ("#if is_guided\n", parser_file);
    fputs ("if (G.gpmemoN) sxbm_free (G.gpmemoN), G.gpmemoN = NULL;\n", parser_file);
    fputs ("if (G.gpmemoP) sxbm_free (G.gpmemoP), G.gpmemoP = NULL;\n", parser_file);
    fputs ("#if is_large\n", parser_file);
    fputs ("if (G.gprange_1 && X_is_allocated (*G.gprange_1)) X_free (G.gprange_1); G.gprange_1 = NULL;\n", parser_file);
    fputs ("#endif\n", parser_file);
    fputs ("if (G.gprhoA_hd && X_is_allocated (*G.gprhoA_hd)) X_free (G.gprhoA_hd); G.gprhoA_hd = NULL;\n", parser_file);
    fputs ("#endif\n", parser_file);
    
    fputs ("}\n", parser_file);

    if (is_2var_form)
	fputs ("#endif /* is_rav2 */\n", parser_file);
}


static void
gen_macros (void)
{
    fputs ("\n/************** M A C R O S **********************/\n", parser_file);

    fprintf (parser_file, "#define tmax\t%ld\n", (SXINT) max_t);
    fprintf (parser_file, "#define ntmax\t%ld\n", (SXINT) max_nt);
    fprintf (parser_file, "#define clmax\t%ld\n", (SXINT) last_clause);

    if (is_2var_form) {
	/* Les nt du parseur initial ont le meme code ds la forme 2var */
	/* La forme 2var en a des supplementaires (new_max_nt >= max_nt) */
	/* Ont prend pour lognt la valeur de la forme 2var, ca permet de reutiliser les Arho */
	fputs ("#if is_rav2\n", parser_file);
	fputs ("/* The value lognt comes from the 2var-form grammar, not the current one. */\n", parser_file);
	fprintf (parser_file, "#define lognt\t%ld\n", (SXINT) sxlast_bit (new_max_nt));
	fputs ("#else\n", parser_file);
    }

    fprintf (parser_file, "#define lognt\t%ld\n", (SXINT) sxlast_bit (max_nt));

    if (is_2var_form) {
	fputs ("#endif /* is_rav2 */\n", parser_file);
    }
    
    fprintf (parser_file, "#define Garity\t%ld\n", (SXINT) max_garity);
    fprintf (parser_file, "#define rhs_arg_nb\t%ld\n", (SXINT) max_rhs_arg_nb);

    if (is_1_RCG) {
	fprintf (parser_file, "#define gclause_nb\t%ld\n", (SXINT) guide_clause);
	fprintf (parser_file, "#define glognt\t%ld\n", (SXINT) guide_lognt);
	fprintf (parser_file, "#define gmax_clause_id\t%ld\n", (SXINT) guide_max_clause_id);
    }
    else {
	fputs ("#define gclause_nb\t0\n", parser_file);
	fputs ("#define glognt\t0\n", parser_file);
	fputs ("#define gmax_clause_id\t0\n", parser_file);
    }

    fprintf (parser_file, "#define is_a_complete_terminal_grammar SX%s\n", is_a_complete_terminal_grammar ? "TRUE" : "FALSE");

}

#if 0
static void
gen_sem_fun (void)
{
    /* Pour l'instant pas de semantique associee a la RCG */
    /* Ici on mettra a la place de sxbvoid le nom de la semantique associee au module */

    fputs ("\n\
#if is_parse_forest\n", parser_file);
    fputs ("#define SEM_FUN\tparse_forest\n", parser_file);
    fputs ("#else\n", parser_file);
    fputs ("#if is_build_coupled_guide\n", parser_file);
    fputs ("#define SEM_FUN\tbuild_coupled_guide\n", parser_file);
    fputs ("#else\n", parser_file);
    fputs ("#if is_2var_form\n", parser_file);
    fputs ("#define SEM_FUN\trav2from2var\n", parser_file);
    fputs ("#else\n", parser_file);
    fputs ("#if is_shallow\n", parser_file);
    fputs ("#define SEM_FUN\tshallow_set\n", parser_file);
    fputs ("#else\n", parser_file);

    if (is_drcg)
	fprintf (parser_file, "#define SEM_FUN\tSXDRCG_%s_sem_fun\n", PRGENTNAME(false));
    else {
	if (has_sem_act)
	    fprintf (parser_file, "#define SEM_FUN\t_%s_semact_prologue\n", PRGENTNAME(false));
	else
	    fputs ("#define SEM_FUN\tsxbvoid\n", parser_file);
    }

    fputs ("#endif\n", parser_file);
    fputs ("#endif\n", parser_file);
    fputs ("#endif\n", parser_file);
    fputs ("#endif\n", parser_file);
}
#endif /* 0 */


static void
gen_Aij (void)
{
    SXINT nt;
    
    fputs ("\n/************** Aij **********************/\n", parser_file);

#if 0
    fputs ("#if is_semantics\n", parser_file);
#endif /* 0 */

    fputs ("static char	*nt2str[] = {/* 0 */ \"\",\n", parser_file);
    
    for (nt = 1; nt <= max_nt; nt++) {
	fprintf (parser_file, "/* %ld */ \"%s\",\n", (SXINT) nt, sxstrget (nt2ste [nt]));
    }

    fputs ("};\n", parser_file);

#if 0
    fputs ("#endif /* is_semantics */\n", parser_file);
#endif /* 0 */
    
    if (!is_a_complete_terminal_grammar) {
#if 0
      fputs ("#if is_semantics || is_guided\n", parser_file);
#endif /* 0 */
      /* sortie de nt2arity */
      /* PB: L'arite de CUT est quelconque (meme nulle), ses args servent uniquement a
	 determiner le moment de son execution (quand ils sont tous calcules et de
	 gauche a droite */
      fputs ("static SXINT nt2arity [] = {\n\
0 /* CUT */, 1 /* LEX */, 1 /* TRUE */, 2 /* LAST */, 2 /* FIRST */, 0 /* FALSE */, 2 /* STRLEN */, 2 /* STREQLEN */, 2 /* STREQ */, 0 /* ORIGIN */,",
	     parser_file);

      for (nt = 1; nt <= max_nt; nt++) {
	if (nt % 8 == 1)
	  fputs ("\n", parser_file);

	fprintf (parser_file, "/* %ld */ %ld,\n", (SXINT) nt, (SXINT) nt2arity [nt]);
      }

      fputs ("\n};\n", parser_file);

#if 0
      fputs ("#endif /* is_semantics || is_guided */\n", parser_file);
#endif /* 0 */
    }
}

static void
gen_loops (void)
{
    SXINT		xlc, A, clause;
    SXINT		*looped_nt, *looped_clause;
    SXBA	wset;

    if (is_loop_grammar) {
      /* Le 26/11/2002 loop_clause_set est genere a part et enleve' de t2clause_set[tmax+1] */
        sxba_to_c (loop_clause_set, parser_file, "loop_clause_set", "", true /* static */);

	looped_nt = (SXINT*) sxcalloc (max_nt+1, sizeof (SXINT));
	looped_clause = (SXINT*) sxalloc (max_nt+sxba_cardinal (loop_clause_set)+1, sizeof (SXINT));
	looped_clause [0] = 0;
	wset = sxba_calloc (last_clause+1);

	xlc = 1;
	A = 0;

	while ((A = sxba_scan (loop_nt_set, A)) > 0) {
	    looped_nt [A] = xlc;
	    sxba_copy (wset, lhsnt2clause [A]);
	    sxba_and (wset, loop_clause_set);

	    if (has_identical_clauses)
		sxba_minus (wset, has_an_identical_clause_set);

	    clause = 0;

	    while ((clause = sxba_scan (wset, clause)) > 0)
		looped_clause [xlc++] = clause;

	    looped_clause [xlc++] = 0;
	}


	fputs ("static SXINT looped_nt[] = {0,\n", parser_file);
    
	for (A = 1; A <= max_nt; A++) {
	    fprintf (parser_file, "/* %ld */ %ld,\n", (SXINT) A, (SXINT) looped_nt [A]);
	}

	fputs ("};\n", parser_file);

	fputs ("static SXINT looped_clause[] = {0,\n", parser_file);
    
	for (A = 1; A <= max_nt; A++) {
	    xlc = looped_nt [A];

	    if (xlc) {
		fprintf (parser_file, "/* %ld */ ", (SXINT) A);

		while ((clause = looped_clause [xlc++]) > 0)
		    fprintf (parser_file, "%ld, ", (SXINT) clause);
	    
		fputs ("0,\n", parser_file);
	    }
	}

	fputs ("};\n", parser_file);

	sxfree (looped_nt);
	sxfree (looped_clause);
	sxfree (wset);
    }
}


static void
gen_instantiated_prdct2struct (void)
{
    SXINT 	arity, profile_nb, profile;
    bool	has_XH;

    has_XH = false;
    arity = profile_nb = 0;
    sxinitialise (profile);

    while((arity = sxba_scan (arity_set, arity)) > 0) {
	profile_nb++;
	profile = arity;

	if (arity >= 4) {
	    has_XH = true;
	}
    }

#if is_semantics==1
    fputs ("#if is_semantics\n", parser_file);
#endif /* is_semantics==1 */

    fprintf (parser_file, "static SXINT	lower_bounds [%ld], upper_bounds [%ld];\n", (SXINT) max_garity, (SXINT) max_garity);

    if (profile_nb > 1)
	fputs ("static struct Aij_struct Aij_struct = {(char*)NULL, 0, 0, lower_bounds, upper_bounds};\n", parser_file);
    else
	fprintf (parser_file,
		 "static struct Aij_struct Aij_struct = {(char*)NULL, 0, %ld, lower_bounds, upper_bounds};\n", (SXINT) profile);

    fputs ("#define ij2b(ij,l)	lower_bounds [l] = RANGE2i(ij), upper_bounds [l] = RANGE2j(ij)\n",
	   parser_file);

    fputs ("\nstatic struct Aij_struct*\n", parser_file);
    fputs ("instantiated_prdct2struct (SXINT Aij)\n", parser_file);
    fputs ("{\n", parser_file);
    fputs ("SXINT couple, profile, rho, ij, bot, param;\n\n", parser_file);
    fputs ("if (Aij < 0) Aij = -Aij;\n", parser_file); /* Le 11/02/04 */
    fputs ("couple = X_X (*(G.prhoA_hd), Aij);\n", parser_file);
    fputs ("Aij_struct.A = couple & ((1<<lognt)-1);\n", parser_file);
    fputs ("Aij_struct.Astr = nt2str [Aij_struct.A];\n", parser_file);
    fputs ("rho = couple >> lognt;\n", parser_file);

    if (profile_nb > 1)
	/* CUT_ic est le + petit code des predefinis */
	fprintf (parser_file, "Aij_struct.arity = (nt2arity+%ld) [Aij_struct.A];\n", (-1*CUT_ic));

    if (profile_nb > 1) {
	if (has_XH) {
	    fputs ("profile = Aij_struct.arity >= 4 ? 4 : Aij_struct.arity;\n", parser_file);
	    fputs ("\nswitch (profile) {\n", parser_file);
	}
	else
	    fputs ("\nswitch (Aij_struct.arity) {\n", parser_file);

	arity = 0;

	while((arity = sxba_scan (arity_set, arity)) > 0) {
	    if (arity >= 4) {
		arity = 4;
	    }

	    switch (arity) {
	    case 1:
		fputs ("case 1:\n", parser_file);
#if is_large
		fputs ("ij = X_X (*(G.prange_1), rho);\n", parser_file);
		fputs ("ij2b(ij, 0);\n", parser_file);
#else
		fputs ("ij2b(rho, 0);\n", parser_file);
#endif
		fputs ("break;\n", parser_file);
		break;

	    case 2:
		fputs ("case 2:\n", parser_file);
		fputs ("ij = XxY_X (*(G.prange_2), rho);\n", parser_file);
		fputs ("ij2b(ij, 0);\n", parser_file);
		fputs ("ij = XxY_Y (*(G.prange_2), rho);\n", parser_file);
		fputs ("ij2b(ij, 1);\n", parser_file);
		fputs ("break;\n", parser_file);
		break;

	    case 3:
		fputs ("case 3:\n", parser_file);
		fputs ("ij = XxYxZ_X (*(G.prange_3), rho);\n", parser_file);
		fputs ("ij2b(ij, 0);\n", parser_file);
		fputs ("ij = XxYxZ_Y (*(G.prange_3), rho);\n", parser_file);
		fputs ("ij2b(ij, 1);\n", parser_file);
		fputs ("ij = XxYxZ_Z (*(G.prange_3), rho);\n", parser_file);
		fputs ("ij2b(ij, 2);\n", parser_file);
		fputs ("break;\n", parser_file);
		break;

	    case 4:
		fputs ("case 4:\n", parser_file);
		fputs ("bot = XH_X (*(G.prange_n), rho);\n", parser_file);
		fputs ("for (param = 0; param < Aij_struct.arity; param++) {\n", parser_file);
		fputs ("ij = XH_list_elem (*(G.prange_n), bot+param);\n", parser_file);
		fputs ("ij2b(ij, param);\n", parser_file);
		fputs ("}\n", parser_file);
		fputs ("break;\n", parser_file);
		break;
           default:
#if EBUG
                sxtrap(ME,"unknown switch case #13");
#endif
                break;
	    }

	    if (arity == 4) {
		break;
	    }
	}

	fputs ("}\n", parser_file);
    }
    else {
	if (has_XH) {
	    fputs ("bot = XH_X (*(G.prange_n), rho);\n", parser_file);
	    fprintf (parser_file, "for (param = 0; param < %ld; param++) {\n", (SXINT) profile);
	    fputs ("ij = XH_list_elem (*(G.prange_n), bot+param);\n", parser_file);
	    fputs ("ij2b(ij, param);\n", parser_file);
	    fputs ("}\n", parser_file);
	}
	else {
	    switch (profile) {
	    case 1:
#if is_large
		fputs ("ij = X_X (*(G.range_1), rho);\n", parser_file);
		fputs ("ij2b(ij, 0);\n", parser_file);
#else
		fputs ("ij2b(rho, 0);\n", parser_file);
#endif
		break;

	    case 2:
		fputs ("ij = XxY_X (*(G.range_2), rho);\n", parser_file);
		fputs ("ij2b(ij, 0);\n", parser_file);
		fputs ("ij = XxY_Y (*(G.range_2), rho);\n", parser_file);
		fputs ("ij2b(ij, 1);\n", parser_file);
		break;

	    case 3:
		fputs ("ij = XxYxZ_X (*(G.range_3), rho);\n", parser_file);
		fputs ("ij2b(ij, 0);\n", parser_file);
		fputs ("ij = XxYxZ_Y (*(G.range_3), rho);\n", parser_file);
		fputs ("ij2b(ij, 1);\n", parser_file);
		fputs ("ij = XxYxZ_Z (*(G.range_3), rho);\n", parser_file);
		fputs ("ij2b(ij, 2);\n", parser_file);
		break;
            default:
#if EBUG
                sxtrap(ME,"unknown switch case #14");
#endif
                break;
	    }
	}
    }

    fputs ("\nreturn &Aij_struct;\n", parser_file);
    fputs ("}\n", parser_file);


#if is_semantics==1
    fputs ("#endif /* is_semantics */\n", parser_file);
#endif /* is_semantics==1 */
}

static void
gen_misc (void)
{
    SXINT i, x;
    SXINT lhs_prdct, gen_G_lhs_bot2, gen_G_lhs_top2, lhs_cur2, lhs_param, lhs_bot3, lhs_top3, lhs_cur3, symb;
	
    fputs ("\n/************** LOCAL GRAMMAR **********************/\n", parser_file);

    /* A FAIRE: tracer les index des terminaux ds le cas fully_coupled */
    fputs ("#if is_sdag && !is_fully_coupled && (is_parse_forest && !is_rav2 || is_2var_form || is_shallow)\n", parser_file);
    /* sortie de clause2lhst_disp */
    /* On en a besoin aussi pour generer les appels a dynam_lex */
    clause2lhst_disp = (SXINT*) sxalloc (last_clause+1, sizeof (SXINT));

    fputs ("static SXINT clause2lhst_disp [] = {0,", parser_file);
    x = 1;

    for (i = 1; i <= last_clause; i++) {
	if (i % 10 == 1)
	    fprintf (parser_file, "\n/* %ld */ ", (SXINT) i);

	fprintf (parser_file, "%ld, ", (SXINT) x);
	clause2lhst_disp [i] = x;

	if (!SXBA_bit_is_set (false_clause_set, i) && clause2identical [i] == i) {
	  /* Le Jeu  2 Nov 2000 11:15:36 */
	    lhs_prdct = XxY_X (rcg_clauses, i) & lhs_prdct_filter;
	    gen_G_lhs_bot2 = XH_X (rcg_predicates, lhs_prdct);
	    gen_G_lhs_top2 = XH_X (rcg_predicates, lhs_prdct+1);

	    for (lhs_cur2 = gen_G_lhs_bot2+2; lhs_cur2 < gen_G_lhs_top2; lhs_cur2++) {
		lhs_param = XH_list_elem (rcg_predicates, lhs_cur2);

		if (lhs_param > 0) {
		    lhs_bot3 = XH_X (rcg_parameters, lhs_param);
		    lhs_top3 = XH_X (rcg_parameters, lhs_param+1);

		    for (lhs_cur3 = lhs_bot3; lhs_cur3 < lhs_top3; lhs_cur3++) {
			if (XH_list_elem (rcg_parameters, lhs_cur3) < 0)
			    x++;
		    }
		}
	    }
	}
    }

    fprintf (parser_file, "%ld, ", (SXINT) x);

    fputs ("\n};\n", parser_file);

    fputs ("#endif /* is_sdag && !is_fully_coupled && (is_parse_forest && !is_rav2 || is_2var_form || is_shallow) */\n", parser_file);

    /* sortie de lhst_list */
    fputs ("#if is_dynam_lex || is_sdag && (is_parse_forest && !is_rav2 || is_2var_form || is_shallow)\n", parser_file);

    fputs ("static SXINT lhst_list [] = {0,", parser_file);

    for (i = 1; i <= last_clause; i++) {
	fprintf (parser_file, "\n/* %ld */ ", (SXINT) i);

	if (!SXBA_bit_is_set (false_clause_set, i) && clause2identical [i] == i) { 
	  /* Le Jeu  2 Nov 2000 11:15:36 */
	    lhs_prdct = XxY_X (rcg_clauses, i) & lhs_prdct_filter;
	    gen_G_lhs_bot2 = XH_X (rcg_predicates, lhs_prdct);
	    gen_G_lhs_top2 = XH_X (rcg_predicates, lhs_prdct+1);

	    for (lhs_cur2 = gen_G_lhs_bot2+2; lhs_cur2 < gen_G_lhs_top2; lhs_cur2++) {
		lhs_param = XH_list_elem (rcg_predicates, lhs_cur2);

		if (lhs_param > 0) {
		    lhs_bot3 = XH_X (rcg_parameters, lhs_param);
		    lhs_top3 = XH_X (rcg_parameters, lhs_param+1);

		    for (lhs_cur3 = lhs_bot3; lhs_cur3 < lhs_top3; lhs_cur3++) {
			symb = XH_list_elem (rcg_parameters, lhs_cur3);

			if (symb < 0) {
			  if (terminal_file)
			    fprintf (parser_file, "SXT_%s, ", sxstrget (t2Cid_ste [-symb]));
			  else
			    fprintf (parser_file, "%ld, ", (SXINT) -symb);
			}
		    }
		}
	    }
	}
    }

    fputs ("\n};\n", parser_file);
    fputs ("#endif /* is_dynam_lex || is_sdag && (is_parse_forest && !is_rav2 || is_2var_form || is_shallow) */\n", parser_file);

    fputs ("#if is_generator || is_parse_forest\n", parser_file);
    
    /* sortie de clause2lhs */
    fputs ("static SXINT clause2lhs [] = {0,", parser_file);

    for (i = 1; i <= last_clause; i++) {
	if (i % 8 == 1)
	    fprintf (parser_file, "\n/* %ld */ ", (SXINT) i);

	x = XxY_X (rcg_clauses, i) & lhs_prdct_filter;
	fprintf (parser_file, "%ld, ", (SXINT) x);
    }

    fputs ("\n};\n", parser_file);
    
    /* sortie de clause2rhs */
    fputs ("static SXINT clause2rhs [] = {0,", parser_file);

    for (i = 1; i <= last_clause; i++) {
	if (i % 8 == 1)
	    fprintf (parser_file, "\n/* %ld */ ", (SXINT) i);

	x = XxY_Y (rcg_clauses, i);
	fprintf (parser_file, "%ld, ", (SXINT) x);
    }

    fputs ("\n};\n", parser_file);

    /* sortie de rhs2prdcts */
    fputs ("static SXINT rhs2prdcts_disp [] = {0,", parser_file);

    for (i = 1; i <= XH_top (rcg_rhs); i++) {
	if (i % 8 == 1)
	    fprintf (parser_file, "\n/* %ld */ ", (SXINT) i);

	x = XH_X (rcg_rhs, i);
	fprintf (parser_file, "%ld, ", (SXINT) x);
    }

    fputs ("\n};\n", parser_file);

    fputs ("static SXINT rhs2prdcts [] = {0,", parser_file);

    for (i = 1; i < XH_list_top (rcg_rhs); i++) {
	if (i % 8 == 1)
	    fprintf (parser_file, "\n/* %ld */ ", (SXINT) i);

	x = XH_list_elem (rcg_rhs, i);
	fprintf (parser_file, "%ld, ", (SXINT) x);
    }

    fputs ("\n};\n", parser_file);

    /* sortie de prdct2prdcts */
    fputs ("static SXINT prdct2args_disp [] = {0,", parser_file);

    for (i = 1; i <= XH_top (rcg_predicates); i++) {
	if (i % 8 == 1)
	    fprintf (parser_file, "\n/* %ld */ ", (SXINT) i);

	x = XH_X (rcg_predicates, i);
	fprintf (parser_file, "%ld, ", (SXINT) x);
    }

    fputs ("\n};\n", parser_file);

    fputs ("static SXINT prdct2args [] = {0,", parser_file);

    for (i = 1; i < XH_list_top (rcg_predicates); i++) {
	if (i % 8 == 1)
	    fprintf (parser_file, "\n/* %ld */ ", (SXINT) i);

	x = XH_list_elem (rcg_predicates, i);
	fprintf (parser_file, "%ld, ", (SXINT) x);
    }

    fputs ("\n};\n", parser_file);

    fputs ("#if is_generator\n", parser_file);

    /* sortie de args2symbs */
    fputs ("static SXINT args2symbs_disp [] = {0,", parser_file);

    for (i = 1; i <= XH_top (rcg_parameters); i++) {
	if (i % 8 == 1)
	    fprintf (parser_file, "\n/* %ld */ ", (SXINT) i);

	x = XH_X (rcg_parameters, i);
	fprintf (parser_file, "%ld, ", (SXINT) x);
    }

    fputs ("\n};\n", parser_file);

    fputs ("static SXINT args2symbs [] = {0,", parser_file);

    for (i = 1; i < XH_list_top (rcg_parameters); i++) {
	if (i % 8 == 1)
	    fprintf (parser_file, "\n/* %ld */ ", (SXINT) i);

	x = XH_list_elem (rcg_parameters, i);

	if (x > 0 || terminal_file == NULL)
	  fprintf (parser_file, "%ld, ", (SXINT) x);
	else
	  fprintf (parser_file, "-%s, ", sxstrget (t2Cid_ste [-x]));
    }

    fputs ("\n};\n", parser_file);

    /* sortie de Xoccur */
    gen_Xoccur ();
    
    fputs ("#endif /* is_generator */\n", parser_file);
    
    fputs ("#endif /*is_generator || is_parse_forest */ \n", parser_file);

    if (is_1_RCG) {
      /* On a guide_clause2clause_id != clause2clause_id [guide_clause2clause] 
         car on ne fait pas subir l'operation 1rcg aux clauses identiques de la grammaire d'origine 
         (qui sont sautees) */
        SXINT logk;

	fputs ("static SXINT guide_clause2clause [] = {0,", parser_file);

	logk = sxlast_bit (last_clause);

	for (i = 1; i <= guide_clause; i++) {
	    if (i % 8 == 1)
		fprintf (parser_file, "\n/* %ld */ ", (SXINT) i);
	    x = guide_clause2clause [i];
	    fprintf (parser_file, "%ld, ", (SXINT) (guide_clause2arg_pos [i]<<logk) + x);
	}

	fputs ("\n};\n", parser_file);

#if 0
	fputs ("static SXINT guide_clause2arg_pos [] = {0,", parser_file);

	for (i = 1; i <= guide_clause; i++) {
	    if (i % 8 == 1)
		fprintf (parser_file, "\n/* %ld */ ", i);
	    fprintf (parser_file, "%ld, ", guide_clause2arg_pos [i]);
	}

	fputs ("\n};\n", parser_file);

	fputs ("static SXINT guide_clause2clause_id [] = {0,", parser_file);

	for (i = 1; i <= guide_clause; i++) {
	    if (i % 8 == 1)
		fprintf (parser_file, "\n/* %ld */ ", i);
	    fprintf (parser_file, "%ld, ", guide_clause2clause_id [i]);
	}

	fputs ("\n};\n", parser_file);
#endif /* 0 */
    }

    if (has_identical_clauses) {
	SXINT	clause, clause2identical_top, nb;
	SXINT	*clause2identical_disp = (SXINT*) sxcalloc (last_clause+1, sizeof (SXINT));

	for (clause = 1; clause <= last_clause; clause++)
	    if ((i = clause2identical [clause]) != clause)
		clause2identical_disp [i]++;

	fputs ("static SXINT clause2identical_disp [] = {0,", parser_file);

	clause2identical_top = 1;

	for (clause = 1; clause <= last_clause; clause++) {
	    if (clause % 8 == 1)
		fprintf (parser_file, "\n/* %ld */ ", (SXINT) clause);

	    fprintf (parser_file, "%ld, ", (SXINT) clause2identical_top);

	    nb = clause2identical_disp [clause];
	    clause2identical_disp [clause] = clause2identical_top;
	    clause2identical_top += nb;
	}
	
	fprintf (parser_file, "%ld\n};\n", (SXINT) clause2identical_top);

	fputs ("static SXINT clause2identical [] = {0,\n", parser_file);

	for (clause = 1; clause <= last_clause; clause++) {
	    if (clause2identical_disp [clause+1] > clause2identical_disp [clause]) {
		fprintf (parser_file, "/* %ld */ ", (SXINT) clause);

		for (i = clause+1; i <= last_clause; i++) {
		    if (clause2identical [i] == clause) {
			fprintf (parser_file, "%ld, ", (SXINT) i);
		    }
		}

		fputs ("\n", parser_file);
	    }
	}

	fputs ("};\n", parser_file);

	sxfree (clause2identical_disp);
    }

#if 0
    if (has_identical_nts) {
	SXINT	nt, nt2identical_top, nb;
	SXINT	*nt2identical_disp = (SXINT*) sxcalloc (max_nt+1, sizeof (SXINT));

	for (nt = 1; nt <= max_nt; nt++)
	    if ((i = nt2identical [nt]) != nt)
		nt2identical_disp [i]++;

	fputs ("static SXINT nt2identical_disp [] = {0,", parser_file);

	nt2identical_top = 1;

	for (nt = 1; nt <= max_nt; nt++) {
	    if (nt % 8 == 1)
		fprintf (parser_file, "\n/* %ld */ ", nt);

	    fprintf (parser_file, "%ld, ", nt2identical_top);

	    nb = nt2identical_disp [nt];
	    nt2identical_disp [nt] = nt2identical_top;
	    nt2identical_top += nb;
	}
	
	fprintf (parser_file, "%ld\n};\n", nt2identical_top);

	fputs ("static SXINT nt2identical [] = {0,\n", parser_file);

	for (nt = 1; nt <= max_nt; nt++) {
	    if (nt2identical_disp [nt+1] > nt2identical_disp [nt]) {
		fprintf (parser_file, "/* %ld */ ", nt);

		for (i = nt+1; i <= max_nt; i++) {
		    if (nt2identical [i] == nt) {
			fprintf (parser_file, "%ld, ", i);
		    }
		}

		fputs ("\n", parser_file);
	    }
	}

	fputs ("};\n", parser_file);

	sxfree (nt2identical_disp);
    }
#endif /* 0 */

    /* if (!(is_left_linear_grammar && is_right_linear_grammar)) non, ds tous les cas */
    gen_nl_stmt_list ();
}

static void
gen_G (void)
{
    /* Sortie de G */	
    fputs ("\n/************** LOCAL GRAMMAR **********************/\n", parser_file);

    fputs ("\nstatic struct G G = {\n", parser_file);
    fprintf (parser_file, "%ld,\n", CURRENT_RCG_PARSER_RELEASE);
    fprintf (parser_file, "\"%s.%s\",\n", PRGENTNAME(true), suffixname);/* char *name; */
    fputs ("clmax /* clause_nb */, Garity, rhs_arg_nb, tmax, ntmax, lognt, 0, 0, 0, /* memoN_size, pid, degree */",
           parser_file);
    fprintf (parser_file, " %ld,\n", (SXINT) max_clause_id);

#if 0
    fputs ("#if is_lex || is_multiple_pass\n\
Gmax_Aclause,\n\
#else\n\
0,\n\
#endif\n", parser_file);
#endif /* 0 */

    fputs ("gclause_nb, glognt, gmax_clause_id, 1 /* pass_no */,\n", parser_file);
    fputs ("#if 0\n", parser_file);
    fputs ("t2str,\n", parser_file);
    fputs ("&(t_names),\n", parser_file);
    fputs ("#endif /* 0 */\n", parser_file);

    if (is_a_complete_terminal_grammar) {
      fputs ("nt2str,\n", parser_file);
      fputs ("NULL /* clause2son_nb */,\n", parser_file);
      fputs ("NULL /* clause2son_pid */,\n", parser_file);
      fputs ("NULL /* nt2arity */,\n", parser_file);
      fputs ("NULL /* clause2lhst_disp */,\n", parser_file);
      fputs ("NULL /* lhst_list */,\n", parser_file);
      fputs ("#if is_parse_forest\n", parser_file);
      fputs ("clause2lhs,\n", parser_file);
      fputs ("#else\n", parser_file);
      fputs ("NULL,\n", parser_file);
      fputs ("#endif\n", parser_file);
      fputs ("NULL /* clause2rhs */,\n", parser_file);
      fputs ("NULL /* rhs2prdcts_disp */,\n", parser_file);
      fputs ("NULL /* rhs2prdcts */,\n", parser_file);
      fputs ("NULL /* prdct2args_disp */,\n", parser_file);
      fputs ("NULL /* prdct2args */,\n", parser_file);

      fputs ("NULL /* args2symbs_disp */,\n", parser_file);
      fputs ("NULL /* args2symbs */,\n", parser_file);
      fputs ("NULL /* Xoccur_disp */,\n", parser_file);
      fputs ("NULL /* Xoccur */,\n", parser_file);
    }
    else {
      fputs ("nt2str,\n", parser_file);
#if is_semantics==1
      fputs ("#if is_semantics\n", parser_file);
#if 0
      fputs ("clause2son_nb, clause2son_pid,\n", parser_file);
#endif /* 0 */
      fputs ("clause2son_nb, NULL,\n", parser_file);
      fputs ("#else /* !is_semantics */\n", parser_file);
      fputs ("NULL, NULL,\n", parser_file);
      fputs ("#endif /* !is_semantics */\n", parser_file);
#else
      fputs ("clause2son_nb, NULL,\n", parser_file);
#endif /* is_semantics==1 */
#if 0
      fputs ("#if is_semantics || is_guided\n", parser_file);
#endif /* 0 */
      fprintf (parser_file, "nt2arity+%ld,\n", (-1*CUT_ic)); /* CUT_ic a le + petit code des predefinis */
#if 0
      fputs ("#else\n", parser_file);
      fputs ("NULL,\n", parser_file);
      fputs ("#endif\n", parser_file);
#endif /* 0 */
      fputs ("#if is_sdag && !is_fully_coupled && (is_parse_forest && !is_rav2 || is_2var_form || is_shallow)\n", parser_file);
      fputs ("clause2lhst_disp,\n", parser_file);
      fputs ("#else\n", parser_file);
      fputs ("NULL,\n", parser_file);
      fputs ("#endif\n", parser_file);
      fputs ("#if is_dynam_lex || is_sdag && (is_parse_forest && !is_rav2 || is_2var_form || is_shallow)\n", parser_file);
      fputs ("lhst_list,\n", parser_file);
      fputs ("#else\n", parser_file);
      fputs ("NULL,\n", parser_file);
      fputs ("#endif\n", parser_file);
      fputs ("#if is_generator || is_parse_forest\n", parser_file);
      fputs ("clause2lhs, clause2rhs,\n", parser_file);
      fputs ("rhs2prdcts_disp, rhs2prdcts,\n", parser_file);
      fputs ("prdct2args_disp, prdct2args,\n", parser_file);
      fputs ("#if is_generator\n", parser_file);
      fputs ("args2symbs_disp, args2symbs,\n", parser_file);
      fputs ("Xoccur_disp, Xoccur,\n", parser_file);
      fputs ("#else\n", parser_file);
      fputs ("NULL, NULL, NULL, NULL,\n", parser_file);
      fputs ("#endif\n", parser_file);
      fputs ("#else\n", parser_file);
      fputs ("NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,\n", parser_file);
      fputs ("#endif\n", parser_file);
    }

    if (is_1_RCG) {
	fputs ("guide_clause2clause,\n", parser_file);
    }
    else
	fputs ("NULL,\n", parser_file);

    if (has_identical_clauses)
	fputs ("clause2identical_disp, clause2identical,\n", parser_file);
    else
	fputs ("NULL, NULL,\n", parser_file);

#if 0
    if (has_identical_nts)
	fputs ("nt2identical_disp, nt2identical,\n", parser_file);
    else
	fputs ("NULL, NULL,\n", parser_file);
#endif

    if (is_loop_grammar)
      fputs ("#if is_lex\nloop_lex,\n#else\nNULL,\n#endif\n", parser_file);
    else
      fputs ("NULL,\n", parser_file);

    if (is_a_complete_terminal_grammar) {
      fputs ("NULL /* nt2clause_id_set */,\n", parser_file);
      fputs ("prdct2t_set,\n", parser_file);
    }
    else {
      fputs ("#if is_lex\nnt2clause_id_set,\n#else\nNULL,\n#endif\n", parser_file);
      fputs ("NULL /* prdct2t_set */,\n", parser_file);
    }

    fputs ("#if is_lex3\nclause2rhs_nt_set,\n#else\nNULL,\n#endif\n",
	   parser_file);

    fputs ("clause2lhs_nt,\n", parser_file);

#if 0
    fputs ("#if is_lex || is_multiple_pass\nclause2Aclause,\n#else\nNULL,\n#endif\n",
	   parser_file);
#endif

    if (is_a_complete_terminal_grammar)
      fputs ("NULL /* clause2clause_id */,\n\
NULL /* A2max_clause_id */,\n\
NULL /* main_clause_set */,\n", parser_file);
    else
      fputs ("clause2clause_id,\n\
A2max_clause_id,\n\
main_clause_set,\n", parser_file);

#if 0
    /*  Le 22/5/2002 */
    if (Lex_file) {
      fputs ("#if is_earley_guide\n\
A2max_Aclause,\n\
#else\n\
NULL /* *A2max_Aclause */,\n\
#endif\n", parser_file);
    }
    else
      fputs ("NULL /* *A2max_Aclause */,\n", parser_file);
#endif /* 0 */

    fputs ("NULL, NULL, NULL, NULL, /* *clause2lbs, *clause2ubs, *nt2lbs, *nt2ubs */\n", parser_file);

    if (is_a_complete_terminal_grammar) {
      fputs ("NULL /* t2clause_set */,\n", parser_file);

      fprintf (parser_file, "#if %ld < SXT_End_Of_File\n", max_t+1);
      fputs ("/* Il y a des terminaux qui sont definis dans un autre module */\n", parser_file);
      fputs ("global_t2local_t,\n", parser_file);
      fputs ("#else\n", parser_file);
      fputs ("NULL /* global_t2local_t */,\n", parser_file);
      fprintf (parser_file, "#endif /* %ld < SXT_End_Of_File */\n", max_t+1);

      fputs ("t2clause,\n", parser_file);
      fputs ("t2clause_stack,\n", parser_file);
    }
    else {
      fputs ("#if is_lex && !is_guided || is_sdag && is_parse_forest\n\
t2clause_set,\n\
#else\n\
NULL /* t2clause_set */,\n\
#endif\n", parser_file);
      fputs ("NULL /* global_t2local_t */,\n", parser_file);
      fputs ("NULL /* t2clause */,\n", parser_file);
      fputs ("NULL /* t2clause_stack */,\n", parser_file);
    }

  if (Lex) {
    fputs ("#if is_lex && !is_guided\n\
Lex,\n\
#else\n\
NULL /* Lex */,\n\
#endif\n", parser_file);
  }
  else {
    fputs ("NULL /* Lex */,\n", parser_file);
  }
    
    /* Pour :
       SXBA		*pmemoN, *pmemoP;
       SXINT            *local_Aij2global_Aij_pid;
       X_header		*prange_1, *prhoA_hd;
       XxY_header	*prange_2;
       XxYxZ_header	*prange_3;
       XH_header	*prange_n;
       SXBA		*gpmemoN, *gpmemoP;
       X_header		*gprange_1, *gprhoA_hd; */
    fputs ("NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,\n", parser_file);

    
    /* SXINT	*from2var2clause, *from2var2clause_disp, *from2var2clause_list, *clause2rhs_stack_map, *rhs_stack_map; */
    if (is_2var_form) {
	fputs ("from2var2clause, from2var2clause_disp, from2var2clause_list,\n", parser_file);
	fputs ("#if is_rav2\n\
clause2rhs_stack_map, rhs_stack_map,\n\
#else\n\
NULL, NULL,\n\
#endif\n", parser_file);
    }
    else
	fputs ("NULL, NULL, NULL, NULL, NULL,\n", parser_file);

    if (is_loop_grammar)
	fputs ("loop_clause_set, looped_nt, looped_clause,\n", parser_file);
    else
	fputs ("NULL, NULL, NULL,\n", parser_file);

/*
   struct fun_calls {
    vfun	first_pass_init, second_pass_init, first_pass_final, second_pass_final, first_pass_loop, second_pass_loop, oflw;
    ifun	first_pass, second_pass, last_pass;
    };
*/
    fputs ("{sxvoid /* first_pass_init */, sxvoid /* second_pass_init */, sxvoid /* first_pass_final */, sxvoid /* second_pass_final */, sxvoid /* first_pass_loop */, sxvoid /* second_pass_loop */, sxvoid /* oflw */, sxvoid /* nl_Ai_processing */, sxivoid /* first_pass */, sxivoid /* second_pass */, sxivoid /* last_pass */, sxbvoid /* lexicalizer */, NULL /* bool   (*dynamic_lexicalizer)(SXINT, SXBA) */},\n", parser_file); /* semact */

    fputs ("{sxvoid /* first_pass_init */, sxvoid /* second_pass_init */, sxvoid /* first_pass_final */, sxvoid /* second_pass_final */, sxvoid /* first_pass_loop */, sxvoid /* second_pass_loop */, sxvoid /* oflw */, sxvoid /* nl_Ai_processing */, sxivoid /* first_pass */, sxivoid /* second_pass */, sxivoid /* last_pass */, sxbvoid /* lexicalizer */, NULL /* bool   (*dynamic_lexicalizer)(SXINT, SXBA) */},\n", parser_file); /* parse */

    fputs ("NULL,\n", parser_file); /* pointeur vers la fonction Aij2struct */
      
    fputs ("#if is_dag\n\
clause2nl_stmt_stack,\n\
nl_stmt_stack,\n\
#else\n\
NULL /* clause2nl_stmt_stack */,\n\
NULL /* nl_stmt_stack */,\n\
#endif /* is_dag */\n", parser_file);
      
    fputs ("NULL /* Ai2inside_t_set */,\n", parser_file);

    fputs ("#if is_dag && !is_a_linear_module\n\
non_right_linear_clause_set,\n\
nl_Ai_set,\n\
#else\n\
NULL /* non_right_linear_clause_set */,\n\
NULL /* nl_Ai_set */,\n\
#endif /* is_dag && !is_a_linear_module */\n", parser_file);

    fputs ("A2A0,\n", parser_file);

    fputs ("#if 0\n\
/* Actually unused */\n", parser_file);
    fprintf (parser_file, "%s /* nl_clause2rhs_pos_disp */,\n", clause2rhs_pos_disp ? "nl_clause2rhs_pos_disp" : "NULL");
    fprintf (parser_file, "%s /* nl_rhs_pos2pos_stack_disp */,\n", clause2rhs_pos_disp ? "nl_rhs_pos2pos_stack_disp" : "NULL");
    fprintf (parser_file, "%s /* nl_pos_stack */,\n", clause2rhs_pos_disp ? "nl_pos_stack" : "NULL");
    fputs ("#endif /* 0 */\n", parser_file);  

    fputs ("NULL,\n", parser_file); /* struct G		**Gs;  Pour acceder aux autres modules */

    fprintf (parser_file, "%s /* is_combinatorial */,\n", is_combinatorial ? "true" : "false"); 
    fprintf (parser_file, "%s /* is_linear */,\n", is_left_linear_grammar && is_right_linear_grammar ? "true" : "false"); 
    fprintf (parser_file, "%s /* is_overlapping */,\n", is_overlapping_grammar ? "true" : "false"); 

    fputs ("#if is_robust\n\
true,\n\
#else\n\
false,\n\
#endif\n", parser_file); /* RCG_parser peut savoir si le module est compile avec l'option -DROBUST */

    if (is_2var_form && is_factorize)
      fputs ("true,\n", parser_file);
    else
      fputs ("false,\n", parser_file);

    fputs ("false /* is_GET_QSTACK */,\n", parser_file);
    fputs ("false /* is_GET_PSTACK */,\n", parser_file);

    fputs ("is_a_complete_terminal_grammar,\n", parser_file);

    fputs ("};\n", parser_file);
}

static void
gen_fun_decl (void)
{
  SXINT 	A;

  fputs ("\n/************** PREDICATE DECLARATIONS **********************/\n", parser_file);


  fputs ("#if is_lexicalization\n", parser_file);

  /* Les predicats externes non definis ds ce module */
  A = 0;
    
  while ((A = sxba_scan (is_nt_external, A)) > 0) {
    if (!SXBA_bit_is_set (is_lhs_nt, A))
      break;
  }

  if (A > 0) {
    fputs ("extern bool\t", parser_file);

    while (A > 0) {
      if ((A & 7) == 0)
	fputs ("\n\t\t", parser_file);

      fprintf (parser_file, "_terminal_%s (void)", sxstrget (nt2ste [A]));
    
      while ((A = sxba_scan (is_nt_external, A)) > 0) {
	if (!SXBA_bit_is_set (is_lhs_nt, A))
	  break;
      }

      if (A < 0)
	fputs (";\n", parser_file);
      else
	fputs (", ", parser_file);
    }
  }

  /* Les predicats externes definis ds ce module */
  A = 0;
    
  while ((A = sxba_scan (is_nt_external, A)) > 0) {
    if (SXBA_bit_is_set (is_lhs_nt, A))
      break;
  }

  if (A > 0) {
    fputs ("bool\t", parser_file);

    while (A > 0) {
      if ((A & 7) == 0)
	fputs ("\n\t\t", parser_file);

      fprintf (parser_file, "_terminal_%s (void)", sxstrget (nt2ste [A]));
    
      while ((A = sxba_scan (is_nt_external, A)) > 0) {
	if (SXBA_bit_is_set (is_lhs_nt, A))
	  break;
      }

      if (A < 0)
	fputs (";\n", parser_file);
      else
	fputs (", ", parser_file);
    }
  }

  /* Les predicats internes (definis et utilises que) ds ce module */
  A = 0;
    
  while ((A = sxba_scan (is_nt_internal, A)) > 0) {
    if (nt2identical [A] == A)
      break;
  }

  if (A > 0) {
    fputs ("static bool\t", parser_file);

    while (A > 0) {
      if ((A & 7) == 0)
	fputs ("\n\t\t", parser_file);

      fprintf (parser_file, "_terminal_%s (void)", sxstrget (nt2ste [A]));
    
      while ((A = sxba_scan (is_nt_internal, A)) > 0) {
	if (nt2identical [A] == A)
	  break;
      }

      if (A < 0)
	fputs (";\n", parser_file);
      else
	fputs (", ", parser_file);
    }
  }


  fputs ("#if is_dynamic_lexicalization\n", parser_file);

  /* Les predicats externes non definis ds ce module */
  A = 0;
    
  while ((A = sxba_scan (is_nt_external, A)) > 0) {
    if (!SXBA_bit_is_set (is_lhs_nt, A))
      break;
  }

  if (A > 0) {
    fputs ("extern bool\t", parser_file);

    while (A > 0) {
      if ((A & 7) == 0)
	fputs ("\n\t\t", parser_file);

      fprintf (parser_file, "_%s_dynamic_lexicalization (SXINT, SXBA)", sxstrget (nt2ste [A]));
    
      while ((A = sxba_scan (is_nt_external, A)) > 0) {
	if (!SXBA_bit_is_set (is_lhs_nt, A))
	  break;
      }

      if (A < 0)
	fputs (";\n", parser_file);
      else
	fputs (", ", parser_file);
    }
  }

  /* Les predicats externes definis ds ce module */
  A = 0;
    
  while ((A = sxba_scan (is_nt_external, A)) > 0) {
    if (SXBA_bit_is_set (is_lhs_nt, A))
      break;
  }

  if (A > 0) {
    fputs ("bool\t", parser_file);

    while (A > 0) {
      if ((A & 7) == 0)
	fputs ("\n\t\t", parser_file);

      fprintf (parser_file, "_%s_dynamic_lexicalization (SXINT, SXBA)", sxstrget (nt2ste [A]));
    
      while ((A = sxba_scan (is_nt_external, A)) > 0) {
	if (SXBA_bit_is_set (is_lhs_nt, A))
	  break;
      }

      if (A < 0)
	fputs (";\n", parser_file);
      else
	fputs (", ", parser_file);
    }
  }

  /* Les predicats internes (definis et utilises que) ds ce module */
  A = 0;
    
  while ((A = sxba_scan (is_nt_internal, A)) > 0) {
    if (nt2identical [A] == A)
      break;
  }

  if (A > 0) {
    fputs ("static bool\t", parser_file);

    while (A > 0) {
      if ((A & 7) == 0)
	fputs ("\n\t\t", parser_file);

      fprintf (parser_file, "_%s_dynamic_lexicalization (SXINT, SXBA)", sxstrget (nt2ste [A]));
    
      while ((A = sxba_scan (is_nt_internal, A)) > 0) {
	if (nt2identical [A] == A)
	  break;
      }

      if (A < 0)
	fputs (";\n", parser_file);
      else
	fputs (", ", parser_file);
    }
  }

  fputs ("#endif /* is_dynamic_lexicalization */\n", parser_file);

  fputs ("#endif /* is_lexicalization */\n", parser_file);


  fputs ("#if is_dag && !is_a_linear_module\n", parser_file);
  /* Les predicats externes non definis ds ce module */
  A = 0;
    
  while ((A = sxba_scan (is_nt_external, A)) > 0) {
    if (!SXBA_bit_is_set (is_lhs_nt, A))
      break;
  }

  if (A > 0) {
    fputs ("extern void\t", parser_file);

    while (A > 0) {
      if ((A & 7) == 0)
	fputs ("\n\t\t", parser_file);

      fprintf (parser_file, "_%s_fill_nl_Ai_set (SXINT)", sxstrget (nt2ste [A]));
    
      while ((A = sxba_scan (is_nt_external, A)) > 0) {
	if (!SXBA_bit_is_set (is_lhs_nt, A))
	  break;
      }

      if (A < 0)
	fputs (";\n", parser_file);
      else
	fputs (", ", parser_file);
    }
  }

  /* Les predicats externes definis ds ce module */
  A = 0;
    
  while ((A = sxba_scan (is_nt_external, A)) > 0) {
    if (SXBA_bit_is_set (is_lhs_nt, A))
      break;
  }

  if (A > 0) {
    fputs ("void\t", parser_file);

    while (A > 0) {
      if ((A & 7) == 0)
	fputs ("\n\t\t", parser_file);

      fprintf (parser_file, "_%s_fill_nl_Ai_set (SXINT)", sxstrget (nt2ste [A]));
    
      while ((A = sxba_scan (is_nt_external, A)) > 0) {
	if (SXBA_bit_is_set (is_lhs_nt, A))
	  break;
      }

      if (A < 0)
	fputs (";\n", parser_file);
      else
	fputs (", ", parser_file);
    }
  }

  /* Les predicats internes (definis et utilises que) ds ce module */
  A = 0;
    
  while ((A = sxba_scan (is_nt_internal, A)) > 0) {
    if (nt2identical [A] == A)
      break;
  }

  if (A > 0) {
    fputs ("static void\t", parser_file);

    while (A > 0) {
      if ((A & 7) == 0)
	fputs ("\n\t\t", parser_file);

      fprintf (parser_file, "_%s_fill_nl_Ai_set (SXINT)", sxstrget (nt2ste [A]));
    
      while ((A = sxba_scan (is_nt_internal, A)) > 0) {
	if (nt2identical [A] == A)
	  break;
      }

      if (A < 0)
	fputs (";\n", parser_file);
      else
	fputs (", ", parser_file);
    }
  }

  fputs ("#endif /* is_dag && !is_a_linear_module */\n", parser_file);


  /* Les predicats externes non definis ds ce module */
  A = 0;
    
  while ((A = sxba_scan (is_nt_external, A)) > 0) {
    if (!SXBA_bit_is_set (is_lhs_nt, A))
      break;
  }

  if (A > 0) {
    fputs ("extern SXINT\t", parser_file);

    while (A > 0) {
      if ((A & 7) == 0)
	fputs ("\n\t\t", parser_file);

      fprintf (parser_file, "_%s (SXINT*, SXINT*, SXINT*)", sxstrget (nt2ste [A]));
    
      while ((A = sxba_scan (is_nt_external, A)) > 0) {
	if (!SXBA_bit_is_set (is_lhs_nt, A))
	  break;
      }

      if (A < 0)
	fputs (";\n", parser_file);
      else
	fputs (", ", parser_file);
    }
  }

  /* Les predicats externes definis ds ce module */
  A = 0;
    
  while ((A = sxba_scan (is_nt_external, A)) > 0) {
    if (SXBA_bit_is_set (is_lhs_nt, A))
      break;
  }

  if (A > 0) {
    fputs ("SXINT\t", parser_file);

    while (A > 0) {
      if ((A & 7) == 0)
	fputs ("\n\t\t", parser_file);

      fprintf (parser_file, "_%s (SXINT*, SXINT*, SXINT*)", sxstrget (nt2ste [A]));
    
      while ((A = sxba_scan (is_nt_external, A)) > 0) {
	if (SXBA_bit_is_set (is_lhs_nt, A))
	  break;
      }

      if (A < 0)
	fputs (";\n", parser_file);
      else
	fputs (", ", parser_file);
    }
  }

  /* Les predicats internes (definis et utilises que) ds ce module */
  A = 0;
    
  while ((A = sxba_scan (is_nt_internal, A)) > 0) {
    if (nt2identical [A] == A)
      break;
  }

  if (A > 0) {
    fputs ("static SXINT\t", parser_file);

    while (A > 0) {
      if ((A & 7) == 0)
	fputs ("\n\t\t", parser_file);

      fprintf (parser_file, "_%s (SXINT*, SXINT*, SXINT*)", sxstrget (nt2ste [A]));
    
      while ((A = sxba_scan (is_nt_internal, A)) > 0) {
	if (nt2identical [A] == A)
	  break;
      }

      if (A < 0)
	fputs (";\n", parser_file);
      else
	fputs (", ", parser_file);
    }
  }


  /* Le point d'entree d'un ensemble de module est determine dynamiquement (c'est l'axiome du
     module dont le local_pid est 0).  Ca permet au meme module d'etre utilise tout seul ou
     pour ses points d'entree */
    
#if 0
  if (has_semantics) {
    /* Ici un traitement semantique est associe a la RCG */
    /* On genere les decl correspondant a la seconde passe au cas ou */
#if is_semantics==1
    fputs ("\n#if is_semantics\n", parser_file);
#endif /* is_semantics==1 */

    fputs ("#if is_multiple_pass\n", parser_file);

    A = sxba_scan (is_nt_external, 0);

    if (A > 0) {
      fputs ("extern SXINT\t", parser_file);

      while (A > 0) {
	if ((A & 7) == 0)
	  fputs ("\n\t\t", parser_file);

	fprintf (parser_file, "_walk_%s (SXINT rho0)", sxstrget (nt2ste [A]));

	A = sxba_scan (is_nt_external, A);

	if (A < 0)
	  fputs (";\n", parser_file);
	else
	  fputs (", ", parser_file);
      }
    }

    A = 0;
    
    while ((A = sxba_scan (is_nt_internal, A)) > 0) {
      if (nt2identical [A] == A)
	break;
    }

    if (A > 0) {
      fputs ("static SXINT\t", parser_file);

      while (A > 0) {
	if ((A & 7) == 0)
	  fputs ("\n\t\t", parser_file);

	fprintf (parser_file, "_walk_%s (SXINT rho0)", sxstrget (nt2ste [A]));
    
	while ((A = sxba_scan (is_nt_internal, A)) > 0) {
	  if (nt2identical [A] == A)
	    break;
	}

	if (A < 0)
	  fputs (";\n", parser_file);
	else
	  fputs (", ", parser_file);
      }
    }


    /* Le point d'entree d'un ensemble de module est determine dynamiquement (c'est l'axiome du
       module dont le local_pid est 0).  Ca permet au meme module d'etre utilise tout seul ou
       pour ses points d'entree */

    fputs ("#endif /* is_multiple_pass */\n", parser_file);

#if is_semantics==1
    fputs ("#endif /* is_semantics */\n", parser_file);
#endif /* is_semantics==1 */
  }
#endif /* 0 */

}

void
gen_clause_comment (FILE *gen_clause_comment_parser_file, SXINT clause)
{
    SXINT rhs, bot, cur, top, ste;

    fprintf (gen_clause_comment_parser_file, "\n/* %ld: ", (SXINT) clause);

    gen_prdct_text (gen_clause_comment_parser_file, XxY_X (rcg_clauses, clause) & lhs_prdct_filter);

    fputs (" --> ", gen_clause_comment_parser_file);

    rhs = XxY_Y (rcg_clauses, clause);
    top = XH_X (rcg_rhs, rhs+1);
    bot = XH_X (rcg_rhs, rhs);
    cur = bot;

    while (cur < top) {
	gen_prdct_text (gen_clause_comment_parser_file, XH_list_elem (rcg_rhs, cur));
	
	cur++;

	if (cur < top)
	    fputs (" ", gen_clause_comment_parser_file);
    }

    ste = clause2action [clause];

    if (ste == SXEMPTY_STE)
	fputs (" . */\n", gen_clause_comment_parser_file);
    else
	fprintf (gen_clause_comment_parser_file, " . @%s */\n", sxstrget (ste));
}

static void
nt2cst (SXBA nt2cst_A_clause_set)
{
    SXINT		clause, nt2cst_lhs_bot2, nt2cst_lhs_top2, isize_nb, cur2, param, cur3, bot3, top3, var_nb, symb, symb_nb;
    SXINT		Sindex_nb, var, rhs, top, bot, nt2cst_rhs_arg_nb, cur, prdct, bot2, top2, rhs_prdct_nb, etc_nb, nt2cst_lhs_t_nb, lhs_adr_nb;

    Aisize_nb = ASindex_nb = Avar_nb = Arhs_arg_nb = Arhs_prdct_nb = Aetc_nb = Alhs_t_nb = Alhs_adr_nb = 0;
    Acut = false;
    Anon_range = false;

    clause = 0;

    while ((clause = sxba_scan (nt2cst_A_clause_set, clause)) > 0) {
	if (SXBA_bit_is_set (clause2non_range_arg, clause)) Anon_range = true;

	prdct = XxY_X (rcg_clauses, clause) & lhs_prdct_filter;
	nt2cst_lhs_bot2 = XH_X (rcg_predicates, prdct);
	nt2cst_lhs_top2 = XH_X (rcg_predicates, prdct+1);
	isize_nb = nt2cst_lhs_t_nb = lhs_adr_nb = 0;

	for (cur2 = nt2cst_lhs_bot2+2; cur2 < nt2cst_lhs_top2; cur2++) {
	    param = XH_list_elem (rcg_predicates, cur2);

	    if (param > 0) {
		bot3 = XH_X (rcg_parameters, param);
		top3 = XH_X (rcg_parameters, param+1);
		lhs_adr_nb += top3-bot3+1;
		var_nb = 0;

		for (cur3 = bot3; cur3 < top3; cur3++) {
		    symb = XH_list_elem (rcg_parameters, cur3);

		    if (symb > 0) {
			var2nb [symb]++;
			var_nb++;
		    }
		    else {
		      if (symb >= -max_ste)
			nt2cst_lhs_t_nb++;
		    }
		}

		if (var_nb > 2)
		    isize_nb += var_nb - 2;
	    }
	}

	if (lhs_adr_nb > Alhs_adr_nb)
	  Alhs_adr_nb = lhs_adr_nb;

	if (nt2cst_lhs_t_nb > Alhs_t_nb)
	    Alhs_t_nb = nt2cst_lhs_t_nb;

	Sindex_nb = 0;

	for (var = 1; var <= max_gvar; var++) {
	    if (var2nb [var] > 1)
		Sindex_nb++;
	}

	rhs = XxY_Y (rcg_clauses, clause);
	top = XH_X (rcg_rhs, rhs+1);
	bot = XH_X (rcg_rhs, rhs);
	nt2cst_rhs_arg_nb = 0;
	etc_nb = 0;

	for (cur = bot; cur < top; cur++) {
	    prdct = XH_list_elem (rcg_rhs, cur);
	    bot2 = XH_X (rcg_predicates, prdct);
	    top2 = XH_X (rcg_predicates, prdct+1);

	    if (XH_list_elem (rcg_predicates, bot2+1) == CUT_ic)
		Acut = true;

	    for (cur2 = bot2+2; cur2 < top2; cur2++) {
		param = XH_list_elem (rcg_predicates, cur2);

		if (param > 0) {
		    nt2cst_rhs_arg_nb++;
		    bot3 = XH_X (rcg_parameters, param);
		    top3 = XH_X (rcg_parameters, param+1);
		    symb_nb = etc_nb = 0;

		    for (cur3 = bot3; cur3 < top3; cur3++) {
			symb = XH_list_elem (rcg_parameters, cur3);

			if (symb == 0) {
			    if (symb_nb != 0) {
				symb_nb = 0;
				/* etc_nb est le nb d'etc qui sont precedes et suivis d'une vraie chaine non vide */
				/* C'est le nb d'oi[l|u]b necessaire */
				etc_nb++;
			    }
			}
			else {
			    symb_nb++;

			    if (symb > 0 &&
				!(XH_list_elem (rcg_predicates, bot2+1) == STRLEN_ic && cur2 == bot2+2)) {
				var2nb [symb]++;
			    }
			}
		    }
		}
	    }
	}

	var_nb = 0;

	for (var = 1; var <= max_gvar; var++) {
	    if (var2nb [var] > 0) {
		var2nb [var] = 0;
		var_nb++;
	    }
	}

	rhs_prdct_nb = top-bot;

	if (isize_nb > Aisize_nb) Aisize_nb = isize_nb;
	if (Sindex_nb > ASindex_nb) ASindex_nb = Sindex_nb;
	if (var_nb > Avar_nb) Avar_nb = var_nb;
	if (nt2cst_rhs_arg_nb > Arhs_arg_nb) Arhs_arg_nb = nt2cst_rhs_arg_nb;
	if (rhs_prdct_nb > Arhs_prdct_nb) Arhs_prdct_nb = rhs_prdct_nb;
	if (etc_nb > Aetc_nb) Aetc_nb = etc_nb;
    }
}


static void
gen_function_local_variables (SXINT A, 
			      SXINT gen_function_local_variables_A_clause_nb, 
			      SXINT A_arity, 
			      /* gen_function_local_variables_A_is_loop, */ 
			      bool gen_function_local_variables_A_is_cyclic)
{
    SXINT		ts, total_size, k;

    if (gen_function_local_variables_A_clause_nb > 1) {
      fputs ("#if is_lex\n", parser_file);

      if (is_1_RCG && A_arity > 1) {
	fputs ("#if is_guided4\n", parser_file);
	k = A2max_clause_id [A]+1;
	/* Le 04/06/2003 */
        fprintf (parser_file, "SXBA_ELT clause_id_set [1+SXNBLONGS(%ld)] = {%ld};\n", (SXINT) k, (SXINT) k);
#if 0
        fprintf (parser_file, "SXBA_ELT clause_id_set [%ld] = {%ld};\n", SXNBLONGS (k)+1, k);
#endif
	fputs ("#else /* !is_guided4 */\n", parser_file);
	fputs ("SXBA clause_id_set;\n", parser_file);
	fputs ("#endif /* !is_guided4 */\n", parser_file);
      }
      else {
	fputs ("SXBA clause_id_set;\n", parser_file);
      }

      fputs ("#endif /* is_lex */\n", parser_file);
    }

    if (is_1_RCG) {
        fprintf (parser_file, "#if is_guided3 || is_guided4 && !is_guided2 || is_fully_guided\n\
SXINT tcrho[%ld];\n\
#endif\n", (SXINT) A_arity);

        if (Arhs_arg_nb) {
	    fputs ("#if is_guided2\n", parser_file);
	    fprintf (parser_file, "SXINT tc2rho[%ld];\n", (SXINT) Arhs_arg_nb);
	    fputs ("#endif\n", parser_file);
	}

        if (A_arity) {
	    fputs ("#if is_fully_guided\n\
struct spf_node ", parser_file);

	    for (k = 0; k < A_arity; k++) {
		if (k > 0)
		    fputs (", ", parser_file);

		if (k == 0)
		    fprintf (parser_file, "*pnode%ld", (SXINT) k);
		else
		    fprintf (parser_file, "*pnodei%ld, *pnode%ld", (SXINT) k, (SXINT) k);
	    }

	    fputs (";\n\
SXINT range;\n\
#endif /* is_fully_guided */\n", parser_file);
	}
    }

     /* rho [0] = clause, rho [1] = lhs, rho [2] = son [1], ..., rho [1+p] = son [p], 
	rho [1+p+1] = t_nb, rho [1+p+1+1] =  Tpq rho [1+p+1+t_nb] = Tp'q' */
    fprintf (parser_file, "SXINT\t\trho[%ld];\n", (SXINT) 2+Arhs_prdct_nb+1+Alhs_t_nb);

    ts = total_size = Aisize_nb + ASindex_nb + Avar_nb + Arhs_arg_nb + Aetc_nb;

    if (total_size)
	fputs ("SXINT\t\t", parser_file);

    if (Arhs_arg_nb) {
        fprintf (parser_file, "olb[%ld], oub[%ld]", (SXINT) Arhs_arg_nb, (SXINT) Arhs_arg_nb);

	if (total_size -= Arhs_arg_nb)
	    fputs (", ", parser_file);
    }

    if (Avar_nb) {
	fprintf (parser_file, "Asize[%ld]", (SXINT) Avar_nb);

	if (total_size -= Avar_nb)
	    fputs (", ", parser_file);
    }

    if (ASindex_nb) {
	fprintf (parser_file, "Sindex[%ld]", (SXINT) ASindex_nb);

	if (total_size -= ASindex_nb)
	    fputs (", ", parser_file);
    }

    if (Aisize_nb) {
	fprintf (parser_file, "isize[%ld]", (SXINT) Aisize_nb);

	if (total_size -= Aisize_nb)
	    fputs (", ", parser_file);
    }

    if (Aetc_nb) {
	fprintf (parser_file, "odddb[%ld]", (SXINT) 2*Aetc_nb);

	if (total_size -= Aetc_nb)
	    fputs (", ", parser_file);
    }

    if (ts) {
        fputs (";\n", parser_file);
    }

    if (!decrease_order && Avar_nb)
	fprintf (parser_file, "SXINT\t\ttop[%ld];\n", (SXINT) Avar_nb);

    if (Avar_nb && Anon_range) {
	fprintf (parser_file, "SXINT\t\tXlb [%ld], Xub [%ld];\n", (SXINT) Avar_nb, (SXINT) Avar_nb);
    }

    fputs ("SXINT\t\tAij, ret_val, clause_id, p, q;\n", parser_file);

    fputs ("#if is_dag\n", parser_file);

    if (Alhs_t_nb) {
      fputs ("SXINT\t\t*qstack_1, *top_qstack_1", parser_file);

      for (k = 2; k <= Alhs_t_nb; k++)
	fprintf (parser_file, ", *qstack_%ld, *top_qstack_%ld", k, k);
	
      fputs (";\n", parser_file);
    }

#if 0
    fputs ("SXINT\t\tpt, tq, pmin, qmax;\n", parser_file);

    if (Alhs_t_nb) {
      fputs ("SXINT\t\tx_1", parser_file);

      for (k = 2; k <= Alhs_t_nb; k++)
	fprintf (parser_file, ", x_%ld", k);
	
      fputs (";\n", parser_file);
    }

    fputs ("SXINT\t\tadr_1", parser_file);

    for (k = 2; k <= Alhs_adr_nb; k++)
      fprintf (parser_file, ", adr_%ld", k);

    fputs (";\n", parser_file);
#endif /* 0 */

    fprintf (parser_file, "SXINT\t\tadr2bound_values [%ld];\n", (SXINT) (Alhs_adr_nb+1));

    /* Large !! */
    fputs ("SXBA\t\tpath_1", parser_file);

    for (k = 2; k <= Alhs_adr_nb; k++)
      fprintf (parser_file, ", path_%ld", k);

    fputs (";\n", parser_file);

    fputs ("#endif /* is_dag */\n", parser_file);

    if (Acut)
	fputs ("bool\tcut;\n", parser_file);

    /* Pour chaque clause instanciee la sem doit etre appelee une fois et une seule.
       Si cette clause instanciee n'est pas impliquee ds un cycle, tous les predicats
       instancies de sa partie droite sont completement calcules.  Si elle est impliquee ds un cycle
       les predicats de sa partie droite sont au moins partiellement calcules */
    if (gen_function_local_variables_A_is_cyclic) {
	fputs ("#if is_cyclic\n", parser_file);

	fputs ("bool\tis_local_cycle, is_cyclic_ip;\n", parser_file);

	fputs ("#endif\n", parser_file);
    }

    fputs ("\n", parser_file);
}


#if 0
static void
gen_extern_first_last (A)
    SXINT A;
{
    /* Genere en debut de procedure les test pour first et last dans le
       cas ou A est un predicat externe */
    /* Pour l'instant on ne fait rien A FAIRE */
    fputs ("#if is_first_last\n", parser_file);
    A_arity = nt2arity [A];

    for (k = 0; k < A_arity; k++) {
	Ak = A_k2Ak(A, k);
	
	ff = SXBA_bit_is_set (is_first_complete, Ak);
	fv = SXBA_bit_is_set (is_first_vide, Ak);
	fe = SXBA_bit_is_set (first [Ak], 0);
    }

    fputs ("#endif\n", parser_file);
}
#endif /* 0 */

static void
gen_sem_act (void)
{
    SXINT clause, ste;

#if is_semantics==1
    fputs ("\n#if is_semantics\n", parser_file);
#endif /* is_semantics==1 */

    fputs ("/* **************************** SEMANTIC ACTIONS **************************** */\n", parser_file);
    fputs ("/* User's defined boolean functions implementing the post-clause @actions */\n", parser_file);
    fprintf (parser_file, "#include \"%s_semact.h\"\n", PRGENTNAME(false));

    fputs ("static bfun semact[]={NULL,\n", parser_file);

    clause = 0;

    while ((clause = sxba_scan (clause_set, clause)) > 0) {
	ste = clause2action [clause];

	if (ste == SXEMPTY_STE)
	    fputs ("sem_void,", parser_file);
	else
	    fprintf (parser_file, "%s,", sxstrget (ste));
    }
	    
    fputs ("};\n", parser_file);

    fputs ("\nstatic bool\n", parser_file);
    fprintf (parser_file, "_%s_semact (SXINT *rho, SXINT son_nb, SXINT sons[], bool is_first)\n", PRGENTNAME(false));
    fputs ("{\n", parser_file);
    fputs ("bfun call;\n", parser_file);
    fputs ("if ((call = semact [rho[0]]) == NULL) return true;\n", parser_file);
    fputs ("return (*call)(rho, son_nb, sons, is_first);\n", parser_file);
    fputs ("}\n", parser_file);
    
    fputs ("\nstatic SXINT\n", parser_file);
    fprintf (parser_file, "_%s_semact_prologue (struct G *G, SXINT local_pid)\n", PRGENTNAME(false));
    fputs ("{\n", parser_file);
    fputs ("#if is_multiple_pass\n", parser_file);
    fprintf (parser_file, "G->semact.second_pass_init = _%s_semact_init;\n", PRGENTNAME(false));
    fprintf (parser_file, "G->semact.second_pass = _%s_semact;\n", PRGENTNAME(false));
    fprintf (parser_file, "G->semact.second_pass_final = _%s_semact_final;\n", PRGENTNAME(false));
    fputs ("#else\n", parser_file);
    fprintf (parser_file, "G->semact.first_pass_init = _%s_semact_init;\n", PRGENTNAME(false));
    fprintf (parser_file, "G->semact.first_pass = _%s_semact;\n", PRGENTNAME(false));
    fprintf (parser_file, "G->semact.first_pass_final = _%s_semact_final;\n", PRGENTNAME(false));
    fputs ("#endif\n", parser_file);
    fprintf (parser_file, "G->semact.last_pass = _%s_semact_last_pass;\n", PRGENTNAME(false));
    fputs ("return 0;\n", parser_file);
    fputs ("}\n", parser_file);

#if is_semantics==1
    fputs ("#endif /* is_semantics */\n", parser_file);
#endif /* is_semantics==1 */
}

#if 0
static void
gen_robust_parser ()
{
    gen_header ();
    gen_includes ();
    gen_macros ();

/* Sortie tres partielle de G */
    fputs ("\nstatic struct G G = {\n", parser_file);
    fprintf (parser_file, "\"%s.%s\",\n", PRGENTNAME(true), suffixname);/* char *name; */
    fputs ("clmax /* clause_nb */, Garity, rhs_arg_nb, tmax, ntmax, lognt, 0, 0, 0,\n", parser_file);
    fputs ("};\n", parser_file);

    /* On genere une ft externe qui remplit les interfaces */
    fputs ("\nvoid _if () {\n", parser_file);
#if 0
    fprintf (parser_file,
	     "\nvoid _%s_if () {\n",
	     PRGENTNAME(false));
#endif

    fputs ("G.pid = global_pid++;\n", parser_file);
    fputs ("Gs [G.pid] = &G;\n", parser_file);
    fputs ("}\n", parser_file);
}
#endif


static void
output_clause2clause_id (void)
{
  SXINT clause;

  fprintf (parser_file, "static SXINT clause2clause_id [%ld] = {0,", (SXINT) last_clause+1);

  for (clause = 1; clause <= last_clause; clause++) {
    if ((clause % 10) == 1)
      fprintf (parser_file, "\n/* %ld */ ", (SXINT) clause);

    fprintf (parser_file, "%ld, ", (SXINT) clause2clause_id [clause]);
  }

  fputs ("};\n", parser_file);

}


static void
output_t2clause_set (SXBA_INDEX	*repr)
{
  fputs ("\n#if is_lex && !is_guided || is_sdag && is_parse_forest\n", parser_file);
  sxbm3header_to_c (repr, parser_file, t2clause_set, max_t+1, "t2clause_set", "");

  fprintf (parser_file, "#if %ld == SXT_End_Of_File\n", max_t+1);

  sxbm3core_to_c (repr, parser_file, max_t+1, "t2clause_set", "", true /* is_static */);

  fprintf (parser_file, "#else /* %ld == SXT_End_Of_File */\n", max_t+1);

  fputs ("/* Initialisation dynamique */\n", parser_file);

  fputs ("static SXBA t2clause_set [SXT_End_Of_File] = {", parser_file);

  if (t2clause_set [0] && !sxba_is_empty (t2clause_set [0]))
    fputs ("t2clause_set_0, };\n", parser_file);
  else
    fputs ("NULL, };\n", parser_file);

  /* Ds ce cas les index doivent etre les codes globaux des terminaux */
  fprintf (parser_file, "#endif /* %ld == SXT_End_Of_File */\n", max_t+1);

  fputs ("#endif /* is_lex && !is_guided || is_sdag && is_parse_forest */\n", parser_file);
}

static void
gen_terminal_instantiated_prdct2struct (void)
{
  fputs (
	 "static SXINT	lower_bounds [1], upper_bounds [1];\n\
static struct Aij_struct Aij_struct = {(char*)NULL, 0, 0, lower_bounds, upper_bounds};\n\
\n\
static struct Aij_struct*\n\
instantiated_prdct2struct (SXINT AijXid)\n\
{\n\
  SXINT Aij, pq;\n\
\n\
  if (AijXid < 0) AijXid = -AijXid;\n\
  Aij = X_X (rhoA_hd, AijXid);\n\
  Aij_struct.A = Aij & ((1<<lognt)-1);\n\
  Aij_struct.Astr = nt2str [Aij_struct.A];\n\
  Aij_struct.arity = 1;\n\
  pq = Aij >> lognt;\n\
  lower_bounds [0] = idag.pq2p [pq];\n\
  upper_bounds [0] = idag.pq2q [pq];\n\
\n\
  return &Aij_struct;\n\
}\n", /* " */parser_file);

}

static void
gen_terminal_parser_body (void)
{
  SXINT A, main_A;
  char *Astr;

#if EBUG
  SXINT A_arity;
  bool A_is_external, A_is_internal, A_is_loop, A_is_cyclic;
#endif /* EBUG */

  for (A = 1; A <= max_nt; A++) {
    if (SXBA_bit_is_set (is_lhs_nt, A)) {
      /* On ne s'occupe que des predicats qui sont definis localement */
      fputs ("\n\n", parser_file);

      if ((main_A = nt2identical [A]) != A) {
	fprintf (parser_file, "/* Predicate %s is identical to %s */\n", sxstrget (nt2ste [A]), sxstrget (nt2ste [main_A]));
      }
      else {
#if EBUG
	A_is_external = SXBA_bit_is_set (is_nt_external, A);
	A_is_internal = SXBA_bit_is_set (is_nt_internal, A);
	A_arity = nt2arity [A];
	A_is_loop = SXBA_bit_is_set (loop_nt_set, A);
	A_is_cyclic = SXBA_bit_is_set (cyclic_nt_set, A);

	if (!A_is_external || A_is_internal || A_is_loop || A_is_cyclic || A_arity != 1 || is_loop_grammar)
	  sxtrap (ME, "gen_terminal_parser_body");
#endif /* EBUG */

	Astr = sxstrget (nt2ste [A]);

	fprintf (parser_file,
"SXINT\n\
_%s (SXINT *rho0, SXINT *ilb, SXINT *iub)\n\
{\n\
  return t_clause_processing (&G, %ld, rho0, ilb [0], iub [0]);\n\
} /* _%s () */\n"
		 , /* " bidon pour le beautifier emacs */Astr, (SXINT) A, Astr);
      }
    }
  }
}


static void
gen_terminal_first_pass_init (void)
{

  fprintf (parser_file,
"#if is_lexicalization\n\
  static SXBA_CST_ALLOC (terminal_prdct_set, %ld);\n\
#endif /* is_lexicalization */\n\n\
",  max_nt+1);


  fprintf (parser_file,
"static void\n\
%s_first_pass_init (void)\n\
{\n\
  SXINT l, global_t, local_t, lgth;\n\
  char  *string;\n\
\n\
  l = idag.ptq_nb;\n\
  X_alloc (&rhoA_hd, \"rhoA_hd\", l+1, 1, memo_oflw, statistics), G.prhoA_hd = &rhoA_hd;\n\
  l = X_size (rhoA_hd);\n\
  memoN = sxbm_calloc (G.memoN_size = 2, l+1);\n\
  local_Aij2global_Aij_pid = (SXINT*) sxalloc (l+1, sizeof (SXINT)), local_Aij2global_Aij_pid [0] = 0;\n\
  memo_assign ();\n\
\n\
#if tmax+1 < SXT_End_Of_File\n\
/* Il y a des terminaux qui sont definis dans un autre module */\n\
  global_t = -1;\n\
\n\
  while ((global_t = sxba_scan (idag.source_set, global_t)) >= 0) {\n\
    string = glbl_t2string [global_t];\n\
    lgth = strlen (string);\n\
    local_t = (SXINT) sxdfa_comb_seek_a_string (&t_names_dico, string, &lgth);\n\
\n\
#if EBUG\n\
    if (local_t <= 0 || lgth != 0)\n\
      sxtrap (ME, \"%s_first_pass_init\");\n\
#endif /* EBUG */\n\
\n\
    /* ce terminal est defini dans ce module (et se trouve dans le source) */\n\
    global_t2local_t [global_t] = local_t;\n\
  }\n\
#endif /* tmax+1 < SXT_End_Of_File */\n\
\n\
#if is_lexicalization\n\
#if is_dynamic_lexicalization\n\
  G.Ai2inside_t_set = Ai2inside_t_set = sxbm_calloc (%ld, idag.source_set_cardinal+1);\n\
#endif /* is_dynamic_lexicalization */\n\
  terminal_lexicalization (&G, terminal_prdct_set);\n\
#endif /* is_lexicalization */\n\
  (*G.semact.first_pass_init) (&G, 0, l);\n\
}\n", /* " */PRGENTNAME(false), PRGENTNAME(false), (SXINT) (A_k2Ak(max_nt, nt2arity [max_nt]-1)+1));
}


static void
gen_terminal_first_pass_final (void)
{
  fprintf (parser_file,
"static void\n\
%s_first_pass_final (void)\n\
{\n\
  /* L'appel suivant de semact.first_pass_final sera fait + tard directement depuis RCG_parser\n\
  (*G.semact.first_pass_final) (&G, 1, 0); */\n\
  if (G.pmemoN) sxbm_free (memoN), G.pmemoN = NULL, memoN = NULL;\n\
  sxfree (local_Aij2global_Aij_pid), local_Aij2global_Aij_pid = NULL;\n\
#if is_dynamic_lexicalization\n\
  sxbm_free (Ai2inside_t_set), G.Ai2inside_t_set = Ai2inside_t_set = NULL;\n\
#endif /* is_dynamic_lexicalization */\n\
  /* La liberation suivante de G.prhoA_hd sera faite + tard directement depuis RCG_parser\n\
  if (G.prhoA_hd && X_is_allocated (*G.prhoA_hd)) X_free (G.prhoA_hd); G.prhoA_hd = NULL; */\n\
}\n", /* " */PRGENTNAME(false));
}


static void
gen_if (SXBA_INDEX *repr)
{
SXINT A, clause;

  /* On genere une ft externe qui remplit les interfaces */
  fputs ("\nvoid _if () {\n", parser_file);
#if 0
fprintf (parser_file,
  "\nvoid _%s_if () {\n",
  PRGENTNAME(false));
#endif

fputs ("#if is_dag && is_a_linear_module\n\
  if (!G.is_linear) {\n\
    printf (\"The current module %s must be recompiled with the \\\"-Dis_a_linear_module=0\\\" option.\\n\", G.name);\n\
    sxexit (1);\n\
  }\n\
#endif /* is_dag && is_a_linear_module */\n", parser_file);

fputs ("if (G.release != CURRENT_RCG_PARSER_RELEASE) {\n\
printf (\"The parser for %s is not up to date, it must be rebuilt.\\n\", G.name);\n\
sxexit (1);\n\
}\n", parser_file);

fputs ("#if !is_large\nlarge_needed (lognt);\n#endif\n", parser_file);

fputs ("G.pid = global_pid++;\n", parser_file);
fprintf (parser_file, "G.degree = %ld;\n", (SXINT) max_degree);
fputs ("Gs [G.pid] = &G;\n", parser_file);
fputs ("G.Gs = &(Gs [0]); /* for son's ref */\n", parser_file);

fputs ("#if 0\n", parser_file);
/* Le 26/05/04 */
fputs ("sxword_reuse (&t_names, \"t_names\", sxcont_malloc, sxcont_alloc, sxcont_realloc, NULL, sxcont_free, NULL, NULL);\n",
  parser_file);
fputs ("#endif /* 0 */\n", parser_file);

     if (is_GET_QSTACK)
        /* Le parseur utilise GET_QSTACK */
        fputs ("G.is_GET_QSTACK = true;\n", parser_file);

     if (is_GET_PSTACK)
        /* Le parseur utilise GET_PSTACK */
        fputs ("G.is_GET_PSTACK = true;\n", parser_file);

     A = 0;

     while ((A = sxba_scan (is_nt_external, A)) > 0) {
        if (SXBA_bit_is_set (is_lhs_nt, A))
           /* A est externe et defini ici */
           fprintf (parser_file, "%s_pid = G.pid;\n", sxstrget (nt2ste [A]));
     }

fputs ("#if !is_grammar_alone\n", parser_file);
fputs ("#if !is_rav2\n", parser_file);
fprintf (parser_file, "if (G.pid == 0 /* main module */) G.parse.first_pass = _%s;\n", sxstrget (nt2ste [1]));
fputs ("#endif /* !is_rav2 */\n", parser_file);
fprintf (parser_file,
  "G.parse.first_pass_init = %s_first_pass_init;\n",
  PRGENTNAME(false));
fprintf (parser_file,
  "G.parse.first_pass_final = %s_first_pass_final;\n",
  PRGENTNAME(false));

#if 0
fputs ("#if is_multiple_pass && !is_rav2\n", parser_file);
fprintf (parser_file, "if (G.pid == 0 /* main module */) G.parse.second_pass = _walk_%s;\n", sxstrget (nt2ste [1]));
fprintf (parser_file,
  "G.parse.second_pass_init = %s_second_pass_init;\n",
  PRGENTNAME(false));

fprintf (parser_file,
  "G.parse.second_pass_final = %s_second_pass_final;\n",
  PRGENTNAME(false));
fputs ("#endif /* is_multiple_pass && !is_rav2 */\n", parser_file);
#endif /* 0 */
fputs ("#endif /* !is_grammar_alone */\n", parser_file);

/* Le Jeu 30 Nov 2000 15:28:55. swap de cette partie en vue du traitement de 2var => rav2 */

#if is_semantics==1
fputs ("#if is_semantics\n", parser_file);
#endif /* is_semantics==1 */

fputs ("G.Aij2struct = instantiated_prdct2struct;\n", parser_file);

fprintf (parser_file, "#if is_lexicalization\n\
  if (G.pid == 0 /* main module */) {\n\
    G.parse.lexicalizer = _terminal_%s;\n\
#if is_dynamic_lexicalization\n\
    G.parse.dynamic_lexicalizer = _%s_dynamic_lexicalization;\n\
#endif /* is_dynamic_lexicalization */\n\
  }\n\
#endif /* is_lexicalization */\n", /* " */sxstrget (nt2ste [1]), sxstrget (nt2ste [1]));

  if (!(is_left_linear_grammar && is_right_linear_grammar)) {
    fputs ("#if is_dag && !is_a_linear_module\n\
  G.parse.nl_Ai_processing = nl_Ai_processing;\n\
#endif /* is_dag && !is_a_linear_module */\n", parser_file);
}


#if 0
fputs ("#if is_single_pass\n", parser_file);
fputs ("if (is_no_semantics) NULL;\n", parser_file);
fputs ("#if !is_build_coupled_guide && !is_2var_form\n", parser_file);
fputs ("else if (is_parse_tree_number) rcg_spf_tree_count_begins (&G);\n", parser_file);
fputs ("#endif /* !is_build_coupled_guide && !is_2var_form */\n", parser_file);
fputs ("else if (is_default_semantics) SEM_FUN (&G);\n", parser_file);
fputs ("#else\n", parser_file);
fputs ("if (is_no_semantics) NULL;\n", parser_file);
fputs ("#if !is_build_coupled_guide && !is_2var_form\n", parser_file);
fputs ("else if (is_parse_tree_number) rcg_spf_tree_count_begins (&G), rcg_spf_begins (&G);\n",
  parser_file);
fputs ("#endif /* !is_build_coupled_guide && !is_2var_form */\n", parser_file);
fputs ("else if (is_default_semantics) SEM_FUN (&G), rcg_spf_begins (&G);\n",
  parser_file);
fputs ("#endif /* is_multiple_pass */\n", parser_file);
#endif /* 0 */
    
#if 0    
if (SXBA_bit_is_set (is_nt_internal, 1)) {
/* On est ds la grammaire principale */
/* last_pass est une variable globale qui est appelee depuis le parser principal
   apres la fin de la derniere passe.  Ce point d'entree permet
   donc de poursuivre/terminer la semantique, independamment du parser qui a termine
   son boulot. */
fputs ("last_pass = G.semact.last_pass;\n", parser_file);
}
#endif /* 0 */

#if is_semantics==1
fputs ("#endif /* is_semantics */\n", parser_file);
#endif /* is_semantics==1 */

if (repr) {
if (max_t && Lex) {
/* Ajout du 18/09/08 */
/* Non vide */
SXINT t;
SXBA  line;

/* Initialisation dynamique avec les terminaux globaux */
fputs ("\n#if is_lex && !is_guided\n", parser_file);
fprintf (parser_file, "#if %ld < SXT_End_Of_File\n", max_t+1);
for (clause = 0; clause <= last_clause; clause++) {
if ((line = Lex [clause]) && !sxba_is_empty (line)) {
t = 0;

while ((t = sxba_scan (line, t)) > 0) {
fprintf (parser_file, "SXBA_1_bit (Lex_%ld, SXT_%s);\n", clause, sxstrget (t2Cid_ste [t]));
}
}
}

fprintf (parser_file, "#endif /* %ld < SXT_End_Of_File */\n", max_t+1);
fputs ("#endif /* is_lex && !is_guided */\n", parser_file);
}

if (max_t) {
/* L'element 0 a deja ete sorti */
SXINT t;

fputs ("\n#if is_lex && !is_guided || is_sdag && is_parse_forest\n", parser_file);
fprintf (parser_file, "#if %ld < SXT_End_Of_File\n", max_t+1);

for (t = 1; t <= max_t; t++) {
if (t2clause_set [t] && !sxba_is_empty (t2clause_set [t]))
     fprintf (parser_file, "t2clause_set [SXT_%s] = t2clause_set_%ld;\n", sxstrget (t2Cid_ste [t]), (SXINT) (repr [t]));
     }

fprintf (parser_file, "#endif /* %ld < SXT_End_Of_File */\n", max_t+1);
fputs ("#endif /* is_lex && !is_guided || is_sdag && is_parse_forest */\n", parser_file);
}
}

fputs ("}\n", parser_file);
}

static void
gen_terminal_lexicalizer (void)
{
  SXINT A, main_A;

  fputs ("#if is_lexicalization\n", parser_file);

  for (A = 1; A <= max_nt; A++) {
    fprintf (parser_file, "\n\
bool\n\
_terminal_%s (void)\n\
{\n\
  return SXBA_bit_is_set (terminal_prdct_set, %ld);\n\
}\n\
", /* " */sxstrget (nt2ste [A]), (SXINT) A);
  }
  fputs ("\n\n#if is_dynamic_lexicalization\n", parser_file);

  for (A = 1; A <= max_nt; A++) {
    if (SXBA_bit_is_set (is_lhs_nt, A)) {
      /* defini ds ce module */
      if ((main_A = nt2identical [A]) != A) {
	fprintf (parser_file, "/* Predicate %s is identical to %s */\n", sxstrget (nt2ste [A]), sxstrget (nt2ste [main_A]));
      }
      else {
	fprintf (parser_file, "\n\
%sbool\n\
_%s_dynamic_lexicalization (SXINT i, SXBA inside_t_set)\n\
{\n\
  sxba_or (inside_t_set, Ai2inside_t_set [%ld]);\n\
}\n\n", /* " */SXBA_bit_is_set (is_nt_external, A) ? "" : "static ",
      sxstrget (nt2ste [A]),
      (SXINT) (A)); /* Pour une grammaire terminale on a A_k2Ak (A, 0) == A */
      }
    }
  }

  fputs ("#endif /* is_dynamic_lexicalization */\n", parser_file);

  fputs ("#endif /* is_lexicalization */\n", parser_file);

  fputs ("\n\n#if is_dag && !is_a_linear_module\n", parser_file);


  for (A = 1; A <= max_nt; A++) {
    if (SXBA_bit_is_set (is_lhs_nt, A)) {
      /* defini ds ce module */
      if ((main_A = nt2identical [A]) != A) {
	fprintf (parser_file, "/* Predicate %s is identical to %s */\n", sxstrget (nt2ste [A]), sxstrget (nt2ste [main_A]));
      }
      else {
	fprintf (parser_file, "\n\
%svoid\n\
_%s_fill_nl_Ai_set (SXINT i)\n\
{\n\
  SXBA_1_bit (nl_Ai_set, %ld);\n\
}\n\n", /* " */SXBA_bit_is_set (is_nt_external, A) ? "" : "static ",
      sxstrget (nt2ste [A]),
      (SXINT) (A)); /* Pour une grammaire terminale on a A_k2Ak (A, 0) == A */
      }
    }
  }

  fputs ("#endif /* is_dag && !is_a_linear_module */\n", parser_file);
}

static void
gen_terminal_parser (void)
{
  gen_header ();
  gen_includes ();
  gen_macros ();
  gen_local_variables ();
  gen_dynamic_lexicalization_decl ();
  gen_nl_decl ();
  gen_terminal_memo_decl ();
  gen_Aij ();
  gen_nl_stmt_list ();
  gen_G ();
  gen_terminal_instantiated_prdct2struct ();
  gen_memo_assign ();
  gen_memo_oflw ();
  gen_terminal_first_pass_init ();
  gen_terminal_first_pass_final ();
  gen_fun_decl ();

  gen_terminal_parser_body ();
    
  gen_if (NULL);

  gen_terminal_lexicalizer ();
}

/* On genere la construction des t reconnus par l'arg i du predicat A */
static void	
gen_dynamic_lexicalization_A_i_clauses (SXINT A, SXINT i)
{
    SXINT clause, lhs_prdct, lhs_bot, lhs_top, rhs, top, bot, cur, lhs_cur, param, symb, prdct, bot3, top3, cur3;
    SXINT nt, bot2, top2, cur2, bot4, top4, cur4;

    fprintf (parser_file, "    while ((clause_id = sxba_scan (nt2clause_id_set_%ld, clause_id)) > 0) {\n\
      switch (clause_id) {\n", (SXINT) A);


    A_clause_set = lhsnt2clause [A];
    clause = 0;

    while ((clause = sxba_scan (A_clause_set, clause)) > 0) {
	/* On sort le source de la clause en commentaire */
	gen_clause_comment (parser_file, clause);

	fprintf (parser_file, "      case %ld:\n", (SXINT) clause2clause_id [clause]);	

	lhs_prdct = XxY_X (rcg_clauses, clause) & lhs_prdct_filter;
	lhs_bot = XH_X (rcg_predicates, lhs_prdct);
	lhs_top = XH_X (rcg_predicates, lhs_prdct+1); (void) lhs_top;

	rhs = XxY_Y (rcg_clauses, clause);	
	top = XH_X (rcg_rhs, rhs+1);
	bot = XH_X (rcg_rhs, rhs);

	lhs_cur = lhs_bot+2+i;
	param = XH_list_elem (rcg_predicates, lhs_cur);

	if (param > 0)  {
	    bot3 = XH_X (rcg_parameters, param);
	    top3 = XH_X (rcg_parameters, param+1);

	    for (cur3 = bot3; cur3 < top3; cur3++) {
		symb = XH_list_elem (rcg_parameters, cur3);
	  
		if (symb < 0) {
		    if (symb >= -max_ste) {
			/* vrai terminal */
			fprintf (parser_file, "          inside_t = X_is_set (&(idag.global_t2inside_t), SXT_%s);\n\
          SXBA_1_bit (Ai_inside_t_set, inside_t);\n", sxstrget (t2Cid_ste [-symb]));
		    }
		}
		else {
		    /* On cherche dans quels predicats de la rhs apparait ce symbole */
		    for (cur = bot; cur < top; cur++) {
			prdct = XH_list_elem (rcg_rhs, cur);
			bot2 = XH_X (rcg_predicates, prdct);

			if (XH_list_elem (rcg_predicates, bot2) == 0) {
			    /* Appel positif */
			    /* on fait quoi sur les appels negatifs ? */
			    nt = XH_list_elem (rcg_predicates, bot2+1);

			    if (nt > 0) {
				/* Predicat utilisateur */
				/* On fait quoi sur les predefinis ? */
				top2 = XH_X (rcg_predicates, prdct+1);

				for (cur2 = bot2+2; cur2 < top2; cur2++) {
				    param = XH_list_elem (rcg_predicates, cur2);

				    if (param > 0) {
					bot4 = XH_X (rcg_parameters, param);
					top4 = XH_X (rcg_parameters, param+1);

					for (cur4 = bot4; cur4 < top4; cur4++) {
					    if (symb == XH_list_elem (rcg_parameters, cur4)) {
						fprintf (parser_file, "        _%s_dynamic_lexicalization (%ld, Ai_inside_t_set);\n",
							 sxstrget (nt2ste [nt]), (SXINT)  (cur2-(bot2+2)));
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

	fputs ("      break;\n", parser_file);
    }

    fputs ("      }\n\
    }\n", parser_file);
}



/* On genere la construction des t reconnus par l'arg i du predicat A */
static void	
gen_nl_A_i_clauses (SXINT A, SXINT i)
{
    SXINT clause, lhs_prdct, lhs_bot, lhs_top, rhs, top, bot, cur, lhs_cur, param, symb, prdct, bot3, top3, cur3;
    SXINT nt, bot2, top2, cur2, bot4, top4, cur4;

    fprintf (parser_file, "#if is_lexicalization\n\
    while ((clause_id = sxba_scan (nt2clause_id_set_%ld, clause_id)) > 0) {\n\
      switch (clause_id) {\n", (SXINT) A);


    A_clause_set = lhsnt2clause [A];
    clause = 0;

    while ((clause = sxba_scan (A_clause_set, clause)) > 0) {
	/* On sort le source de la clause en commentaire */
	gen_clause_comment (parser_file, clause);

	fprintf (parser_file, "      case %ld:\n\
#endif /* is_lexicalization */\n\
        SXBA_1_bit (non_right_linear_clause_set, %ld);\n", (SXINT) clause2clause_id [clause], (SXINT) clause);	

	lhs_prdct = XxY_X (rcg_clauses, clause) & lhs_prdct_filter;
	lhs_bot = XH_X (rcg_predicates, lhs_prdct);
	lhs_top = XH_X (rcg_predicates, lhs_prdct+1); (void) lhs_top;

	rhs = XxY_Y (rcg_clauses, clause);	
	top = XH_X (rcg_rhs, rhs+1);
	bot = XH_X (rcg_rhs, rhs);

	lhs_cur = lhs_bot+2+i;
	param = XH_list_elem (rcg_predicates, lhs_cur);

	if (param > 0)  {
	    bot3 = XH_X (rcg_parameters, param);
	    top3 = XH_X (rcg_parameters, param+1);

	    for (cur3 = bot3; cur3 < top3; cur3++) {
		symb = XH_list_elem (rcg_parameters, cur3);
	  
		if (symb > 0) {
		    /* On cherche dans quels predicats de la rhs apparait ce symbole */
		    for (cur = bot; cur < top; cur++) {
			prdct = XH_list_elem (rcg_rhs, cur);
			bot2 = XH_X (rcg_predicates, prdct);

			if (XH_list_elem (rcg_predicates, bot2) == 0) {
			    /* Appel positif */
			    /* on fait quoi sur les appels negatifs ? */
			    nt = XH_list_elem (rcg_predicates, bot2+1);

			    if (nt > 0) {
				/* Predicat utilisateur */
				/* On fait quoi sur les predefinis ? */
				top2 = XH_X (rcg_predicates, prdct+1);

				for (cur2 = bot2+2; cur2 < top2; cur2++) {
				    param = XH_list_elem (rcg_predicates, cur2);

				    if (param > 0) {
					bot4 = XH_X (rcg_parameters, param);
					top4 = XH_X (rcg_parameters, param+1);

					for (cur4 = bot4; cur4 < top4; cur4++) {
					    if (symb == XH_list_elem (rcg_parameters, cur4)) {
						fprintf (parser_file, "        _%s_fill_nl_Ai_set (%ld);\n",
							 sxstrget (nt2ste [nt]), (SXINT)  (cur2-(bot2+2)));
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

	fputs ("#if is_lexicalization\n\
      break;\n", parser_file);
    }

    fputs ("      }\n\
    }\n\
#endif /* is_lexicalization */\n", parser_file);
}

static void
gen_lexicalizer (void)
{
  SXINT     A, clause, lhs_prdct, lhs_bot, lhs_top, lhs_nt, lhs_cur, param, bot3, cur3, top3, symb;
  SXINT     rhs, top, bot, cur, prdct, bot2, nt, main_A, A_arity, i, Ak, k;
  bool is_first_t, is_first;
  SXBA      non_right_linear_Ak_set;

  fputs ("\n\n#if is_lexicalization\n", parser_file);

  fprintf (parser_file, "static SXBA_CST_ALLOC (terminal_prdct_set0, %ld);\n\n",  max_nt+1);
  fprintf (parser_file, "static SXBA_CST_ALLOC (terminal_prdct_set1, %ld);\n\n",  max_nt+1);


  for (A = 1; A <= max_nt; A++) {
    if (SXBA_bit_is_set (is_lhs_nt, A)) {
      /* defini ds ce module */
    fprintf (parser_file, "\n\
%sbool\n\
_terminal_%s (void)\n\
{\n\
  bool ret_val;\n\
\n\
  if (!SXBA_bit_is_reset_set (terminal_prdct_set0, %ld)) {\n\
    /* Appel recursif retourne vrai */\n\
#if EBUG\n\
    printf (\"basic lexicalization: prdct %s has %%sbeen validated\\n\", SXBA_bit_is_set (terminal_prdct_set1, %ld) ? \"not \" : \"\");\n\
#endif /* EBUG */\n\
    return !SXBA_bit_is_set (terminal_prdct_set1, %ld);\n\
  }\n\
\n\
  ret_val = false;\n\
\n\
  /* chaque t des lhs-args doit etre ds source_set */\n\
  /* et chaque predicat de la rhs doit pouvoir atteindre ces terminaux */\n\
", /* " */SXBA_bit_is_set (is_nt_external, A) ? "" : "static ", sxstrget (nt2ste [A]), (SXINT) A, sxstrget (nt2ste [A]), (SXINT) A, (SXINT) A);
	  
  A_clause_set = lhsnt2clause [A];
  clause = 0;

  while ((clause = sxba_scan (A_clause_set, clause)) > 0) {
    /* On sort le source de la clause en commentaire */
    gen_clause_comment (parser_file, clause);

    lhs_prdct = XxY_X (rcg_clauses, clause) & lhs_prdct_filter;
    lhs_bot = XH_X (rcg_predicates, lhs_prdct);
    lhs_top = XH_X (rcg_predicates, lhs_prdct+1);
    lhs_nt = XH_list_elem (rcg_predicates, lhs_bot+1); (void) lhs_nt;

    is_first_t = true;

    for (lhs_cur = lhs_bot+2; lhs_cur < lhs_top; lhs_cur++) {
      param = XH_list_elem (rcg_predicates, lhs_cur);

      if (param > 0)  {
	bot3 = XH_X (rcg_parameters, param);
	top3 = XH_X (rcg_parameters, param+1);

	for (cur3 = bot3; cur3 < top3; cur3++) {
	  symb = XH_list_elem (rcg_parameters, cur3);
	  
	  if (symb < 0 && symb >= -max_ste) {
	    /* vrai terminal */
	    fprintf (parser_file, "%sSXBA_bit_is_set (idag.source_set, SXT_%s)",
		     is_first_t ? "  if (" : "\n      && ", sxstrget (t2Cid_ste [-symb]));
	    is_first_t = false;
	  }
	}
      }
    }

    rhs = XxY_Y (rcg_clauses, clause);
    top = XH_X (rcg_rhs, rhs+1);
    bot = XH_X (rcg_rhs, rhs);

    for (cur = bot; cur < top; cur++) {
      prdct = XH_list_elem (rcg_rhs, cur);
      bot2 = XH_X (rcg_predicates, prdct);
      nt = XH_list_elem (rcg_predicates, bot2+1);

      /* on fait quoi sur les appels negatifs ? */
      if (nt > 0) {
	fprintf (parser_file, "%s_terminal_%s ()",
		 is_first_t ? "  if (" : "\n      && ", sxstrget (nt2ste [nt]));
	is_first_t = false;
      }
    }

    if (!is_first_t)
      fputs (")", parser_file);

    wvstr = clause2varstr (varstr_raz (wvstr), clause);

    fprintf (parser_file, "  {\n\
#if is_dag && !is_a_linear_module\n\
    SXBA_1_bit (basic_lexicalization_clause_set, %ld);\n\
#endif /* is_dag && !is_a_linear_module */\n\
    SXBA_1_bit (nt2clause_id_set_%ld, %ld /* clause_id */);\n\
    ret_val = true;\n\
    basic_lexicalization_non_instantiated_valid_clause_nb++;\n\
#if EBUG\n\
    printf (\"basic lexicalization (pid=%%ld): clause %s is validated\\n\", G.pid);\n\
#endif /* EBUG */\n\
  }\n\
", /* " */(SXINT) clause, (SXINT) A, (SXINT) clause2clause_id [clause], varstr_tostr (wvstr));
  }
  
  fprintf (parser_file, "\n\
  if (!ret_val)\n\
    /* Echec */\n\
    SXBA_1_bit (terminal_prdct_set1, %ld);\n\
#if EBUG\n\
    printf (\"basic lexicalization: prdct %s is %%svalidated\\n\", ret_val ? \"\" : \"not \");\n\
#endif /* EBUG */\n\
  return ret_val;\n\
} /* _terminal_%s */\n", /* " */(SXINT) A, sxstrget (nt2ste [A]), sxstrget (nt2ste [A]));
  }
}

  fputs ("\n\n#if is_dynamic_lexicalization\n", parser_file);

  for (A = 1; A <= max_nt; A++) {
    if (SXBA_bit_is_set (is_lhs_nt, A)) {
      /* defini ds ce module */
      if ((main_A = nt2identical [A]) != A) {
	fprintf (parser_file, "/* Predicate %s is identical to %s */\n", sxstrget (nt2ste [A]), sxstrget (nt2ste [main_A]));
      }
      else {
	A_arity = nt2arity [A];
	  
	fprintf (parser_file, "\n\
%sbool\n\
_%s_dynamic_lexicalization (SXINT i, SXBA inside_t_set)\n\
{\n\
  SXINT     Ai, clause_id, inside_t;\n\
  SXBA      Ai_inside_t_set;\n\
  bool ret_val;\n\
\n\
  Ai = %ld /* %s */+i;\n\
  Ai_inside_t_set = Ai2inside_t_set [Ai];\n\
\n\
  if (SXBA_bit_is_reset_set (dynamic_lexicalization_Ai_set, Ai)) {\n\
    /* 1ere fois, on le fait */\n\
    clause_id = 0;\n\
\n\
",
/* " */
     SXBA_bit_is_set (is_nt_external, A) ? "" : "static ",
     sxstrget (nt2ste [A]),
     (SXINT) (A_k2Ak(A,0)),
     sxstrget (nt2ste [A]));

        if (A_arity > 2) {
	    fputs ("    switch (i) {\n", parser_file);	

	    for (i = 0; i < A_arity; i++) {
		fprintf (parser_file, "    case %ld:\n", (SXINT) i);

		gen_dynamic_lexicalization_A_i_clauses (A, i);

		fputs ("    break;\n", parser_file);
	    }

	    fputs ("    }\n", parser_file);	
	}
	else {
	    if (A_arity == 2) {
		fputs ("    if (i==0) {\n", parser_file);	
		gen_dynamic_lexicalization_A_i_clauses (A, 0);
		fputs ("    }\n\
    else {\n", parser_file);
		gen_dynamic_lexicalization_A_i_clauses (A, 1);	
		fputs ("    }\n", parser_file);
	    }
	    else {
		gen_dynamic_lexicalization_A_i_clauses (A, 0);
	    }
	}
	  
	fprintf (parser_file, "  }\n\
\n\
  sxba_or (inside_t_set, Ai_inside_t_set);\n\
} /* _%s_dynamic_lexicalization */\n", sxstrget (nt2ste [A]));
      }
    }
  }

  fputs ("#endif /* is_dynamic_lexicalization */\n", parser_file);

  fputs ("#endif /* is_lexicalization */\n", parser_file);

  fputs ("\n\n#if is_dag && !is_a_linear_module\n", parser_file);

  if (!(is_left_linear_grammar && is_right_linear_grammar)) {
      /* On genere  nl_Ai_processing () qui sont les points de depart des non-linearites */
      fputs ("static void\n\
nl_Ai_processing (void)\n\
{\n\
  /* Pour chaque clause non lineaire A (...) --> ... B (..., X, ...) ... C (..., X, ...) .\n\
     on genere _B (i); _C(j); si X se trouve en position i ds B et j ds C */\n\
  /* Marche que is_lexicalization soit positionne' ou pas */\n\
\n\
  SXINT clause;\n\
\n\
#if is_lexicalization\n\
  sxba_and (basic_lexicalization_clause_set, non_right_linear_clause_set);\n\
#else /* is_lexicalization */\n\
  sxba_copy (basic_lexicalization_clause_set, non_right_linear_clause_set);\n\
#endif /* is_lexicalization */\n\
\n\
  clause = 0;\n\
\n\
  while ((clause = sxba_scan (basic_lexicalization_clause_set, clause)) > 0) {\n\
    switch (clause) {\n\
    default:\n\
      sxtrap (ME, \"nl_Ai_processing (unexpected clause)\");\n", /* " */parser_file);

      is_first = true;									      

      for (A = 1; A <= max_nt; A++) {	
	  if (SXBA_bit_is_set (is_lhs_nt, A)) {
	      /* defini ds ce module */	
	      if ((main_A = nt2identical [A]) != A) {
		  fprintf (parser_file, "/* Predicate %s is identical to %s */\n", sxstrget (nt2ste [A]), sxstrget (nt2ste [main_A]));
	      }	
	      else {
		  A_clause_set = lhsnt2clause [A];

		  clause = 0;

		  while ((clause = sxba_scan (A_clause_set, clause)) > 0) {
		      if (SXBA_bit_is_set (non_right_linear_clause_set, clause)) {
			  if (!is_first) {
			      fputs ("      break;\n", parser_file);
			  }
			  else
			      is_first = false;

			  gen_clause_comment (parser_file, clause);
			  fprintf (parser_file, "    case %ld:\n", (SXINT) clause);

			  non_right_linear_Ak_set = clause2non_right_linear_Ak_set [clause];

			  Ak = 0;

			  while ((Ak = sxba_scan (non_right_linear_Ak_set, Ak)) > 0) {
			      nt = Ak2A (Ak);
			      k = Ak_A2k (Ak, nt);

			      fprintf (parser_file, "     _%s_fill_nl_Ai_set (%ld);\n", sxstrget (nt2ste [nt]), (SXINT) k);
			  }
		      }
		  }
	      }
	  }
      }

      fputs ("  break;\n\
    }\n\
  }\n\
}\n\n", parser_file);
  }

  for (A = 1; A <= max_nt; A++) {
    if (SXBA_bit_is_set (is_lhs_nt, A)) {
      /* defini ds ce module */
      if ((main_A = nt2identical [A]) != A) {
	fprintf (parser_file, "/* Predicate %s is identical to %s */\n", sxstrget (nt2ste [A]), sxstrget (nt2ste [main_A]));
      }
      else {
	A_arity = nt2arity [A];
	  
	fprintf (parser_file, "\n\
%svoid\n\
_%s_fill_nl_Ai_set (SXINT i)\n\
{\n\
  SXINT     Ai, clause_id;\n\
  bool ret_val;\n\
\n\
  Ai = %ld /* %s */+i;\n\
\n\
  if (SXBA_bit_is_reset_set (nl_Ai_set, Ai)) {\n\
    /* 1ere fois, on le fait */\n\
#if is_lexicalization\n\
    clause_id = 0;\n\
#endif /* is_lexicalization */\n\
\n\
",
/* " */
     SXBA_bit_is_set (is_nt_external, A) ? "" : "static ",
     sxstrget (nt2ste [A]),
     (SXINT) (A_k2Ak(A,0)),
     sxstrget (nt2ste [A]));

        if (A_arity > 2) {
	    fputs ("    switch (i) {\n", parser_file);	

	    for (i = 0; i < A_arity; i++) {
		fprintf (parser_file, "    case %ld:\n", (SXINT) i);

		gen_nl_A_i_clauses (A, i);

		fputs ("    break;\n", parser_file);
	    }

	    fputs ("    }\n", parser_file);	
	}
	else {
	    if (A_arity == 2) {
		fputs ("    if (i==0) {\n", parser_file);	
		gen_nl_A_i_clauses (A, 0);
		fputs ("    }\n\
    else {\n", parser_file);
		gen_nl_A_i_clauses (A, 1);	
		fputs ("    }\n", parser_file);
	    }
	    else {
		gen_nl_A_i_clauses (A, 0);
	    }
	}
	  
	fprintf (parser_file, "  }\n\
} /* _%s_fill_nl_Ai_set */\n", sxstrget (nt2ste [A]));
      }
    }
  }

  fputs ("#endif /* is_dag && !is_a_linear_module */\n", parser_file);
}

static void
gen_parser (void)
{
  SXINT 	A, main_A, A_arity, clause, main_clause, /* clause_nb, */pos, i;
  SXINT		bot2, top2, Ak, k;
  SXINT		d, degree;
  bool	A_is_internal, A_is_external;
  char	        *Astr;

  gen_header ();
  gen_includes ();
  gen_macros ();
#if 0
  gen_sem_fun ();
#endif /* 0 */
  gen_local_variables ();
  gen_clause_id ();
  gen_lex ();

  if (is_2var_form)
    gen_shallow ();

  gen_for_rav2 ();

  if (has_first_last_pp)
    gen_first_last_pp ();

  if (first)
    gen_first_last ();

  gen_dynamic_lexicalization_decl ();
  gen_nl_decl ();
  gen_memo_decl ();
  gen_Aij ();
  gen_loops ();
  gen_misc ();
  gen_G ();
  gen_instantiated_prdct2struct ();
  gen_memo_fun ();

  /* Le Lun  4 Dec 2000 11:32:08, le corps du parseur genere ne sera active que si l'on
     n'est pas
     - sur un parseur qui se contente de calculer le mapping inverse d'une forme
     initiale vers une forme equivalente 2var
     - sur un shallow_parseur ou un super lexicaliseur qui se contente d'exprimer
     un calcul precedent en termes de la grammaire courante */

  fputs ("\n#if !is_rav2\n", parser_file);

  gen_fun_decl ();

  for (A = 1; A <= max_nt; A++) {
    if (SXBA_bit_is_set (is_lhs_nt, A)) {
      /* On ne s'occupe que des predicats qui sont definis localement */
      fputs ("\n\n", parser_file);

      if ((main_A = nt2identical [A]) != A) {
	fprintf (parser_file, "/* Predicate %s is identical to %s */\n", sxstrget (nt2ste [A]), sxstrget (nt2ste [main_A]));
      }
      else {
	A_arity = nt2arity [A];

	A_is_internal = SXBA_bit_is_set (is_nt_internal, A);
	A_is_external = SXBA_bit_is_set (is_nt_external, A); (void) A_is_external;
	A_is_loop = SXBA_bit_is_set (loop_nt_set, A);
	A_is_cyclic = SXBA_bit_is_set (cyclic_nt_set, A);

	A_clause_set = lhsnt2clause [A];
	sxba_copy (non_false_A_clause_set, A_clause_set);
	sxba_minus (non_false_A_clause_set, false_clause_set);

	if (has_identical_clauses) {
	  sxba_minus (non_false_A_clause_set, has_an_identical_clause_set);
	}

	if (is_loop_grammar)
	  sxba_minus (non_false_A_clause_set, loop_clause_set);

	A_clause_nb = sxba_cardinal (non_false_A_clause_set);

	if (A_is_internal)
	  fputs ("static ", parser_file);

	Astr = sxstrget (nt2ste [A]);

	if (A_arity) {
	  if (is_1_RCG) {
	    fprintf (parser_file,
		     "SXINT\n\
#if is_guided2\n\
_%s (SXINT *rho0, SXINT *ilb, SXINT *iub, SXINT *tcrho)\n\
#else\n\
_%s (SXINT *rho0, SXINT *ilb, SXINT *iub)\n\
#endif\n\
{\n",
		     Astr, Astr);
	  }
	  else {
	    fprintf (parser_file,
		     "SXINT\n_%s (SXINT *rho0, SXINT *ilb, SXINT *iub)\n{\n", Astr);
	  }
	}
	else
	  fprintf (parser_file,
		   "SXINT\n_%s (SXINT *rho0)\n{\n", Astr);

	if (A_clause_nb == 0) {
	  /* Le 09/02/04 je genere le code qui suit (il faut remplir rho0) */
	  fputs ("#if EBUG\n", parser_file);

	  if (A_arity) {
	    fprintf (parser_file,
		     "printf (\"(%%s)in_out(%%ld): %s (", sxstrget (nt2ste [A]));

	    for (pos = 0; pos < A_arity; pos++) {
	      fputs ("%ld..%ld", parser_file);

	      if (pos+1 < A_arity)
		fputs (", ", parser_file);
	    }

	    fputs (") FALSE\\n\", G.name, call_level+1, ", parser_file);

	    for (pos = 0; pos < A_arity; pos++) {
	      fprintf (parser_file, "ilb[%ld], iub[%ld]", (SXINT) pos, (SXINT) pos);

	      if (pos+1 < A_arity)
		fputs (", ", parser_file);
	    }

	    fputs (");\n", parser_file);
	  }
	  else
	    fprintf (parser_file,
		     "printf (\"(%%s)in_out: %s ()\\n\", G.name);\n", sxstrget (nt2ste [A]));

	  fputs ("#endif /* EBUG */\n", parser_file);

	  fputs ("#if EBUG2\nnt_calls++;\n#endif /* EBUG2 */\n", parser_file);

	  fputs ("*rho0 = 0;\n", parser_file); /* Le 07/08/08 */

	  if (!Lex_file) {
	    /* Ds le cas is_earley_guide, *rho0 est calcule par l'appelant (arg in) */
#if is_memon==1
	    fputs ("#if is_memon\n", parser_file);
#endif /* is_memon==1 */

	    switch (A_arity) {
	    case 0:
	      break;

	    case 1:
	      fputs ("#if is_large\n", parser_file);
	      fputs ("X_set(&arity_eq_1, MAKE_A_RANGE(ilb[0], iub[0]), &local_val);\n", parser_file);
#if 0
	      fputs ("X_set(&range_1, MAKE_A_RANGE(ilb[0], iub[0]), &local_val);\n", parser_file);
#endif /* 0 */
	      fputs ("#else\n", parser_file);
	      fputs ("local_val = MAKE_A_RANGE(ilb[0], iub[0]);\n", parser_file);
	      fputs ("#endif\n", parser_file);
	      break;

	    case 2:
	      fputs ("XxY_set(&arity_eq_2, MAKE_A_RANGE(ilb[0], iub[0]), MAKE_A_RANGE(ilb[1], iub[1]), &local_val);\n", parser_file); 

#if 0
	      fputs ("XxY_set(&range_2, MAKE_A_RANGE(ilb[0], iub[0]), MAKE_A_RANGE(ilb[1], iub[1]), &local_val);\n", parser_file); 
#endif /* 0 */
	      break;

	    case 3:
	      fputs ("XxYxZ_set(&arity_eq_3, MAKE_A_RANGE(ilb[0], iub[0]), MAKE_A_RANGE(ilb[1], iub[1]), MAKE_A_RANGE(ilb[2], iub[2]), &local_val);\n", parser_file); 

#if 0
	      fputs ("XxYxZ_set(&range_3, MAKE_A_RANGE(ilb[0], iub[0]), MAKE_A_RANGE(ilb[1], iub[1]), MAKE_A_RANGE(ilb[2], iub[2]), &local_val);\n", parser_file); 
#endif /* 0 */
	      break;

	    default:
	      for (i = 0; i < A_arity; i++) {
		fprintf (parser_file, "XH_push(arity_gte_4, MAKE_A_RANGE(ilb[%ld], iub[%ld]));\n",
			 (SXINT) i, (SXINT) i);
#if 0
		fprintf (parser_file, "XH_push(range_n, MAKE_A_RANGE(ilb[%ld], iub[%ld]));\n",
			 (SXINT) i, (SXINT) i);
#endif /* 0 */
	      }

	      fputs ("XH_set(&arity_gte_4, &local_val);\n", parser_file);
#if 0
	      fputs ("XH_set(&range_n, &local_val);\n", parser_file);

#endif /* 0 */
	      break;
	    }

	    if (A_arity)
	      fprintf (parser_file, "X_set (&rhoA_hd, (local_val<<lognt)+%ld, &Aij);\n", (SXINT) A);
	    else
	      fprintf (parser_file, "X_set (&rhoA_hd, %ld, &Aij));\n", (SXINT) A);

	    fputs ("*rho0 = MAKE_Aij_pid (Aij, G.pid);\n", parser_file);

#if is_memon==1
	    fputs ("#else /* !is_memon */\n", parser_file);

	    fputs ("*rho0 = 0;\n", parser_file);

	    fputs ("#endif /* !is_memon */\n", parser_file);
#endif /* is_memon==1 */
	  }

	  clause = 0;

	  while ((clause = sxba_scan (A_clause_set, clause)) > 0) {
	    gen_clause_comment (parser_file, clause);
	    fputs ("/* False clause */\n", parser_file);
	    SXBA_0_bit (lhsnt2clause [A], clause); /* Pour la seconde passe */
	  }

	  fputs ("return -1;\n", parser_file);
	}
	else {
	  /* Variables locales */
	  nt2cst (non_false_A_clause_set);
	  gen_ret_vals (non_false_A_clause_set);

	  gen_function_local_variables (A, A_clause_nb, A_arity, /* A_is_loop, */A_is_cyclic);

	  fputs ("#if EBUG\n", parser_file);

	  fputs ("call_level++;\n", parser_file);

	  if (A_arity) {
	    fprintf (parser_file,
		     "printf (\"(%%s)in(%%ld): %s (", sxstrget (nt2ste [A]));

	    for (pos = 0; pos < A_arity; pos++) {
	      fputs ("%ld..%ld", parser_file);

	      if (pos+1 < A_arity)
		fputs (", ", parser_file);
	    }

	    fputs (")\\n\", G.name, call_level, ", parser_file);

	    for (pos = 0; pos < A_arity; pos++) {
	      fprintf (parser_file, "ilb[%ld], iub[%ld]", (SXINT) pos, (SXINT) pos);

	      if (pos+1 < A_arity)
		fputs (", ", parser_file);
	    }

	    fputs (");\n", parser_file);
	  }
	  else
	    fprintf (parser_file,
		     "printf (\"(%%s)in: %s ()\\n\", G.name);\n", sxstrget (nt2ste [A]));

	  fputs ("#endif /* EBUG */\n", parser_file);

	  fputs ("#if EBUG2\nnt_calls++;\n#endif /* EBUG2 */\n", parser_file);

	  fputs ("*rho0 = 0;\n", parser_file); /* Le 07/08/08 */

#if 0
	  /* Le 19/05/04 */
	  /* ... finalement je vire c,a car c,a ne permet de traiter des trucs du style
	     &A(X) --> &B(X) .
	     ou` B est non local (et les terminaux de X sont definis ds le module ou` est defini B) */
	  if (A_is_external && SXBA_bit_is_set (is_nt_external, 1)) {
	    /* On verifie qu'il n'y a pas de 0 ds les tokens des defs externes des modules secondaires */
	    /* Cette verif correspond a un scanner local et est valide meme si la grammaire est top-down erasing */
	    fputs ("#if CHECK_LOCAL_T\n", parser_file);

	    if (A_arity > 1) {
	      fprintf (parser_file, "for (larg_nb = 0; larg_nb < %ld; larg_nb++)\n", A_arity);
	    }
	      
	    fputs ("#if is_sdag\n", parser_file);
	    fprintf (parser_file,
		     "for (upper_bound = iub[%s], lower_bound = ilb[%s]; lower_bound < upper_bound; lower_bound++)\n",
		     (A_arity > 1) ? "larg_nb" : "0",
		     (A_arity > 1) ? "larg_nb" : "0");
	    fputs ("if (sxba_is_empty(LSOURCE(lower_bound))) return -1;\n", parser_file);
	    fputs ("#else /* !is_sdag */\n", parser_file);
	    fprintf (parser_file,
		     "for (pub = lsource+iub[%s], plb = lsource+ilb[%s]; plb < pub; plb++)\n",
		     (A_arity > 1) ? "larg_nb" : "0",
		     (A_arity > 1) ? "larg_nb" : "0");
	    fputs ("if (*plb == 0) return -1;\n", parser_file);
	    fputs ("#endif /* !is_sdag */\n", parser_file);
	    
	    fputs ("#endif /* CHECK_LOCAL_T */\n", parser_file);
	  }
#endif /* 0 */

	  /* Le 22/5/2002 */
	  if (Lex_file) {
	    /* Ds le cas is_earley_guide, *rho0 est calcule par l'appelant (arg in) */
	    fputs ("#if is_earley_guide\n\
rho[1] = *rho0;\n\
#else /* !is_earley_guide */\n", parser_file);
	  }

#if is_memon==1
	  fputs ("#if is_memon || is_semantics\n", parser_file);
#endif /* is_memon==1 */

	  switch (A_arity) {
	  case 0:
	    break;

	  case 1:
	    fputs ("#if is_large\n", parser_file);
	    fputs ("X_set(&arity_eq_1, MAKE_A_RANGE(ilb[0], iub[0]), &local_val);\n", parser_file);
#if 0
	    fputs ("X_set(&range_1, MAKE_A_RANGE(ilb[0], iub[0]), &local_val);\n", parser_file);
#endif /* 0 */
	    fputs ("#else\n", parser_file);
	    fputs ("local_val = MAKE_A_RANGE(ilb[0], iub[0]);\n", parser_file);
	    fputs ("#endif\n", parser_file);
	    break;

	  case 2:
	    fputs ("XxY_set(&arity_eq_2, MAKE_A_RANGE(ilb[0], iub[0]), MAKE_A_RANGE(ilb[1], iub[1]), &local_val);\n", parser_file); 

#if 0
	    fputs ("XxY_set(&range_2, MAKE_A_RANGE(ilb[0], iub[0]), MAKE_A_RANGE(ilb[1], iub[1]), &local_val);\n", parser_file); 
#endif /* 0 */
	    break;

	  case 3:
	    fputs ("XxYxZ_set(&arity_eq_3, MAKE_A_RANGE(ilb[0], iub[0]), MAKE_A_RANGE(ilb[1], iub[1]), MAKE_A_RANGE(ilb[2], iub[2]), &local_val);\n", parser_file); 

#if 0
	    fputs ("XxYxZ_set(&range_3, MAKE_A_RANGE(ilb[0], iub[0]), MAKE_A_RANGE(ilb[1], iub[1]), MAKE_A_RANGE(ilb[2], iub[2]), &local_val);\n", parser_file); 
#endif /* 0 */
	    break;

	  default:
	    for (i = 0; i < A_arity; i++) {
	      fprintf (parser_file, "XH_push(arity_gte_4, MAKE_A_RANGE(ilb[%ld], iub[%ld]));\n",
		       (SXINT) i, (SXINT) i);

#if 0
	      fprintf (parser_file, "XH_push(range_n, MAKE_A_RANGE(ilb[%ld], iub[%ld]));\n",
		       (SXINT) i, (SXINT) i);
#endif /* 0 */
	    }

	    fputs ("XH_set(&arity_gte_4, &local_val);\n", parser_file);

#if 0
	    fputs ("XH_set(&range_n, &local_val);\n", parser_file);

#endif /* 0 */
	    break;
	  }

	  if (A_arity)
	    fprintf (parser_file, "X_set (&rhoA_hd, (local_val<<lognt)+%ld, &Aij);\n", (SXINT) A);
	  else
	    fprintf (parser_file, "X_set (&rhoA_hd, %ld, &Aij));\n", (SXINT) A);

#if is_memop
	  /* On change la taille si le parseur courant construit un guide */
	  fprintf (parser_file, "#if is_memop && is_single_pass && is_build_coupled_guide\n\
memoP [*rho] [0] = %ld\n\
#endif /* is_memop && is_single_pass && is_build_coupled_guide */\n\
;\n", A2max_clause_id [A]+1);
#endif /* is_memop */

#if 0
	  /* Le 07/08/08 */
	  fputs ("rho[1] = *rho0 = MAKE_Aij_pid (Aij, G.pid);\n", parser_file);

	  fputs ("#else /* !(is_memon || is_semantics) */\n", parser_file);

	  fputs ("*rho0 = 0;\n", parser_file);
#endif /* 0 */
	  
	  if (A == 1 /* Le predicat courant est l'axiome de la rcg si c'est la principale il doit avoir pour code 1
			    on le fait donc en premier */) {
	    fputs ("if (G.pid == 0) MAKE_Aij_pid (Aij, G.pid) /* L'affectation se fera + tard, normalement */;\n", parser_file);
	  }

#if is_memon==1
	  fputs ("#endif /* is_memon || is_semantics */\n", parser_file);
#endif /* is_memon==1 */

	  /* Le 22/5/2002 */
	  if (Lex_file) {
	    /* Ds le cas is_earley_guide, *rho0 est calcule par l'appelant (arg in) */
	    fputs ("#endif /* !is_earley_guide */\n", parser_file);
	  }

	  if (A_arity) {
	    /* Si le nombre d'args du predicat est nul, j'ai choisi de ne pas le memoiser.
	       L'idee est que sa valeur (vraie ou fausse) provient de la semantique et
	       non pas de la syntaxe (qui rend vrai).  Cette semantique, inconnue au niveau
	       de rcg va donc etre reevaluee a chaque appel.  Il faut neanmoins calculer le
	       couple (A, rho) de la LHS qui sert de pt de repere a la semantique */
#if is_memon==1
	    fputs ("#if is_memon\n", parser_file);
#endif /* is_memon==1 */
	
	    fputs ("if (\n", parser_file);

	    if (is_cyclic_grammar) {
	      fputs ("#if is_cyclic\n", parser_file);
	      fputs ("SXBA_bit_is_set (memoN0, Aij)\n", parser_file);
	      fputs ("#else\n", parser_file);
	      fputs ("!SXBA_bit_is_reset_set (memoN0, Aij)\n", parser_file);
	      fputs ("#endif\n", parser_file);
	    }
	    else
	      fputs ("!SXBA_bit_is_reset_set (memoN0, Aij)\n", parser_file);


	    fputs (") {\n#if EBUG2\n\
memo_prdct_hit++;\n\
#endif\n", parser_file);

	    fprintf (parser_file, "#if EBUG\n\
printf (\"(%%s)out_memo(%%ld): %s (", sxstrget (nt2ste [A]));
 
	    for (pos = 0; pos < A_arity; pos++) {
	      fputs ("%ld..%ld", parser_file);

	      if (pos+1 < A_arity)
		fputs (", ", parser_file);
	    }

	    fputs (") %s\\n\", G.name, call_level, ", parser_file);

	    for (pos = 0; pos < A_arity; pos++) {
	      fprintf (parser_file, "ilb[%ld], iub[%ld], ", (SXINT) pos, (SXINT) pos);
	    }

	    fputs ("SXBA_bit_is_set (memoN1, Aij) ? \"TRUE\" : \"FALSE\");\ncall_level--;\n#endif /* EBUG */\n", parser_file);

#if 0
	    fputs ("return SXBA_bit_is_set (memoN1, Aij) ? 1 : -1;\n}\n", parser_file);
#endif /* 0 */

	    /* Le 07/08/08 */
	    fputs ("if (SXBA_bit_is_set (memoN1, Aij)) {\n\
*rho0 = local_Aij2global_Aij_pid [Aij];\n\
return 1;\n\
}\n\
\n\
return -1;\n\
}\n", parser_file);

#if is_memon==1
	    fputs ("#endif /* is_memon */\n", parser_file);
#endif /* is_memon==1 */
	  }

	    
	  if (A_is_cyclic) {
	    fputs ("#if is_cyclic\n", parser_file);
	    fputs ("if (SXBA_bit_is_set (memoC [1], Aij)) {\n", parser_file);

	    fprintf (parser_file, "#if EBUG\nprintf (\"(%%s)out_cycle(%%ld): %s (", sxstrget (nt2ste [A]));
 
	    for (pos = 0; pos < A_arity; pos++) {
	      fputs ("%ld..%ld", parser_file);

	      if (pos+1 < A_arity)
		fputs (", ", parser_file);
	    }

	    fputs (") FALSE\\n\", G.name, call_level", parser_file);

	    for (pos = 0; pos < A_arity; pos++) {
	      fprintf (parser_file, ", ilb[%ld], iub[%ld]", (SXINT) pos, (SXINT) pos);
	    }

	    fputs (");\ncall_level--;\n#endif /* EBUG */\n", parser_file);
	    fputs ("return -1;\n}\n", parser_file);
	    fputs ("#endif /* is_cyclic */\n", parser_file);
	  }

	  if (is_1_RCG) {
	    fputs ("#if is_guided3 || is_guided4 && !is_guided2 || is_fully_guided\n", parser_file);

#if 0
	    /* On genere un truc comme ca */
#if is_robust
	    if (is_robust_run) {
#if is_memon
	      SXBA_1_bit (memoN1, Aij);
#endif /* is_memon */
	      return 1;
	    } 
	    else
#else /* !is_robust */
	      {
#if EBUG2
		iprdct_searched_in_guide++;
#endif
		if (!coupled_prdct (0, 2)) {
#if EBUG
		  printf ("(%s)out_guide(%ld): S1 (%ld..%ld) false\n", G.name, call_level, ilb[0], iub[0], );
		  call_level--;
#endif /* EBUG */
		  return -1;
		}
#if EBUG2
		iprdct_found_in_guide++;
#endif
	      }
#endif /* !is_robust */
#endif /* if 0 */

#if is_memon==1
	    fputs ("#if is_robust\nif (is_robust_run) {\n#if is_memon\nSXBA_1_bit (memoN1, Aij);\n#endif /* is_memon */\nreturn 1;\n}\nelse\n#else /* !is_robust */\n{\n", parser_file);
#else
	    fputs ("#if is_robust\nif (is_robust_run) {\nSXBA_1_bit (memoN1, Aij);\nreturn 1;\n}\nelse\n#else /* !is_robust */\n{\n", parser_file);
#endif /* is_memon==1 */

	    fputs ("#if EBUG2\n\
iprdct_searched_in_guide++;\n\
#endif\n", parser_file);

	    fputs ("if (", parser_file);

	    for (Ak = A_k2Ak (A, 0), k = 0; k < A_arity; Ak++, k++) {
	      if (k > 0)
		fputs ("\n|| ", parser_file);

	      fprintf (parser_file, "!coupled_prdct (%ld, %ld)", (SXINT) k, (SXINT) Ak2guide_nt [Ak] /* code interne ds le guide du nt A_k */);
	    }

	    fputs (") {\n", parser_file);


	    fprintf (parser_file, "#if EBUG\n\
printf (\"(%%s)out_guide(%%ld): %s (", sxstrget (nt2ste [A]));
 
	    for (pos = 0; pos < A_arity; pos++) {
	      fputs ("%ld..%ld", parser_file);

	      if (pos+1 < A_arity)
		fputs (", ", parser_file);
	    }

	    fputs (") false\\n\", G.name, call_level", parser_file);

	    for (pos = 0; pos < A_arity; pos++) {
	      fprintf (parser_file, ", ilb[%ld], iub[%ld]", (SXINT) pos, (SXINT) pos);
	    }

	    fputs (");\n\
call_level--;\n\
#endif /* EBUG */\n", parser_file);


	    fputs ("return -1;\n", parser_file);

	    fputs ("}\n", parser_file);

	    fputs ("#if EBUG2\n\
iprdct_found_in_guide++;\n\
#endif\n", parser_file);

	    fputs ("}\n\
#endif /* !is_robust */\n", parser_file);

	    fputs ("#endif /* is_guided3 || is_guided4 && !is_guided2 || is_fully_guided */\n", parser_file);
	  }
	    
	  if (A_is_cyclic) {
	    fputs ("#if is_cyclic\n", parser_file);
	    fputs ("is_cyclic_ip = SXBA_bit_is_set (memoC [0], Aij);\n", parser_file);

	    fputs ("if (!((is_glbl_cycle || is_cyclic_ip) ? ((is_glbl_cycle) ? ((is_cyclic_ip) ? (SXBA_bit_is_reset_set (memoC [3], Aij)) : (SXBA_bit_is_reset_set (memoC [2], Aij))) : (SXBA_bit_is_reset_set (memoC [3], Aij))) : true)) {\n", parser_file);
		    
	    fprintf (parser_file, "#if EBUG\nprintf (\"(%%s)out_memo_cycle(%%ld): %s (", sxstrget (nt2ste [A]));
 
	    for (pos = 0; pos < A_arity; pos++) {
	      fputs ("%ld..%ld", parser_file);

	      if (pos+1 < A_arity)
		fputs (", ", parser_file);
	    }

	    fputs (") %s\\n\", G.name, call_level", parser_file);

	    for (pos = 0; pos < A_arity; pos++) {
	      fprintf (parser_file, ", ilb[%ld], iub[%ld]", (SXINT) pos, (SXINT) pos);
	    }

	    fputs (", SXBA_bit_is_set (memoN1, Aij) ? \"true\" : \"false\");\ncall_level--;\n#endif /* EBUG */\n", parser_file);

#if 0
	    fputs ("return SXBA_bit_is_set (memoN1, Aij) ? 1 : -1;\n", parser_file);
#endif /* 0 */

	    /* Le 07/08/08 */
	    fputs ("if (SXBA_bit_is_set (memoN1, Aij)) {\n\
*rho0 = local_Aij2global_Aij_pid [Aij];\n\
return 1;\n\
}\n\
\n\
return -1;\n", parser_file);

	    fputs ("}\n", parser_file);

	    fputs ("if (is_cyclic_ip) SXBA_1_bit (memoC [1], Aij); else SXBA_1_bit (memoC [0], Aij);\n", parser_file);
	    fputs ("is_local_cycle = is_glbl_cycle;\n", parser_file);
	    fputs ("is_glbl_cycle |= is_cyclic_ip;\n", parser_file);

	    fputs ("#endif /* is_cyclic */\n", parser_file);
	  }


          fputs ("\n#if is_dynamic_lexicalization\n", parser_file);
	  
#if 0
	  fprintf (parser_file, "  if (SXBA_bit_is_reset_set (dynamic_lexicalization_A_set, %ld /* %s */)) {\n",
                   (SXINT) A, sxstrget (nt2ste [A]));

	  for (i = 0; i < A_arity; i++) {
             fprintf (parser_file, "    _%s_dynamic_lexicalization (%ld, Ai2inside_t_set [%ld]);\n",
                      sxstrget (nt2ste [A]), (SXINT) i, (SXINT) (A_k2Ak(A,i)));
	  }


          fputs ("  }\n", parser_file);
#endif /* 0 */

          fprintf (parser_file, "  if (!sxba_2op (NULL, SXBA_OP_IS, Ai2inside_t_set [%ld], SXBA_OP_AND, idag.range2inside_t_set [MAKE_AN_IDAG_RANGE (ilb [0], iub [0])])",
                   (SXINT) (A_k2Ak(A,0)));

	  for (i = 1; i < A_arity; i++) {
             fprintf (parser_file, "\n\
      || !sxba_2op (NULL, SXBA_OP_IS, Ai2inside_t_set [%ld], SXBA_OP_AND, idag.range2inside_t_set [MAKE_AN_IDAG_RANGE (ilb [%ld], iub [%ld])])",
                      (SXINT) (A_k2Ak(A,i)), (SXINT) i, (SXINT) i);
	  }

          fprintf (parser_file, ") {\n\
#if EBUG\n\
    printf (\"(%%s)out_dynamic_lexicalization(%%ld): %s (%%ld..%%ld", sxstrget (nt2ste [A]));

	  for (i = 1; i < A_arity; i++) {
             fputs (", %ld..%ld", parser_file);
	  }

          fputs (") FALSE\\n\", G.name, call_level, ilb[0], iub[0]", parser_file);

	  for (i = 1; i < A_arity; i++) {
             fprintf (parser_file, ", ilb[%ld], iub[%ld]", (SXINT) i, (SXINT) i);
	  }

          fputs (");\n\
    call_level--;\n\
#endif /* EBUG */\n\
    return -1;\n\
  }\n", parser_file);

          fputs ("#endif /* is_dynamic_lexicalization */\n\n", parser_file);

	  fputs ("#if is_generator\n\
ret_vals [0] = -1;\n\
#endif\n", parser_file);


	  fputs ("\n#if is_robust\n\
if (is_robust_run) ret_val = 1; else\n\
#endif\n\
ret_val = -1;\n", parser_file);

	  if (A_clause_nb > 1) {
	    fputs ("#if is_lex\n", parser_file);

	    /*  Le 22/5/2002 */
	    if (Lex_file) {
	      /* Ds le cas is_earley_guide, *rho0 est calcule par l'appelant (arg in) */
	      /* ON suppose que si is_earley_guide => is_lex et is_dynam_lex=0 */
	      fprintf (parser_file, "#if is_earley_guide\n\
clause_id_set = ptcmemoP [Aij_pid2Aij (*rho0)];\n\
#else /* !is_earley_guide */\n\
clause_id_set = nt2clause_id_set_%ld;\n\
#endif /* !is_earley_guide */\n", (SXINT) A);
	    }
	    else {
	      if (is_1_RCG) {
		fputs ("#if is_guided4\n", parser_file);

		if (A_arity == 1) {
		  fputs ("clause_id_set = ptcmemoP [tcrho [0]];\n", parser_file);
		}
		else {
		  for (pos = 0; pos < A_arity; pos++) {
		    if (pos == 0)
		      fputs ("sxba_copy (clause_id_set, ptcmemoP [tcrho [0]]);\n", parser_file);
		    else
		      fprintf (parser_file, "sxba_and (clause_id_set, ptcmemoP [tcrho [%ld]]);\n", (SXINT) pos);
		  }
		}

		fputs ("#else /* !is_guided4 */\n", parser_file);
		fprintf (parser_file, "clause_id_set = nt2clause_id_set_%ld;\n", (SXINT) A);
		fputs ("#endif /* !is_guided4 */\n", parser_file);
	      }
	      else {
		fprintf (parser_file, "clause_id_set = nt2clause_id_set_%ld;\n", (SXINT) A);
	      }
	    }

	    fputs ("clause_id = 0;\n\
while ((clause_id = sxba_scan (clause_id_set, clause_id)) > 0) {\n", parser_file);

#if 0
	    fprintf (parser_file, "for (xlex_code = nt2lex_code [%ld]; (clause_id = lex_code [xlex_code]) != 0; xlex_code++)\n", A);
#endif /* 0 */

	    fputs ("switch (clause_id) {\n\
#else /* !is_lex */\n\
do { /* Bidon, pour permettre des breaks */\n\
#endif /* !is_lex */\n", parser_file);
	  }


	  /* Le Ven 15 Dec 2000 17:46:57, clause_id est obtenu par clause2clause_id */
	  /* Le 29/5/2002 Suppression de clause2Aclause : Les valeurs des case sont des clause_id
	     => les clauses identiques ont des clause_id differents */
	  /* clause_nb = */clause = 0; /* clause_id = 0;*/
	  /* Jeu 17 Mai 2001 09:39:59 : l'index 0 est utilise pour le calcul de prdct_level memoC [0]
	     et le stockage de Arho cyclique */

	  while ((clause = sxba_scan (A_clause_set, clause)) > 0) {
#if EBUG
	    if (sxba_cardinal (lhs_arg_tbp_set))
	      sxtrap (ME, "gen_parser");
#endif /* EBUG */

	    /* On sort le source de la clause en commentaire */
	    clause_id = clause2clause_id [clause];

	    if (SXBA_bit_is_set (non_false_A_clause_set, clause) && fill_param_pos (clause)) {
	      /* clause_nb++; Le 05/02/04 chaque case a son break !! nb de clauses generees */

	      /* is_cyclic_clause = SXBA_bit_is_set (cyclic_clause_set, clause); unused */
	      is_lex_clause = SXBA_bit_is_set (lexicalized_clause_set, clause);

	      if (A_clause_nb > 1) {
		fputs ("#if is_lex\n", parser_file);

#if 0
		/* Le 05/02/04 chaque case a son break !! */
		if (clause_nb > 1)
		  fputs ("break;\n", parser_file);
#endif /* 0 */

		gen_clause_comment (parser_file, clause);

#if 0
		fprintf (parser_file, "case %ld:\n#endif /* is_lex */\n", (SXINT) clause2Aclause [clause]);
#endif
		fprintf (parser_file, "case %ld:\n\
#endif /* is_lex */\n", (SXINT) clause_id);
#if is_memop
					 fputs ("#if !is_lex && is_memop && is_single_pass\n", parser_file);
					 fprintf (parser_file, "clause_id = %ld;\n", clause_id);
					 fputs ("#endif /* !is_lex && is_memop && is_single_pass */\n", parser_file);
#endif /* is_memop */
	      }
	      else
		gen_clause_comment (parser_file, clause);

	      if (lfsa_file && !SXBA_bit_is_set (loop_clause_set, clause)) {
		/* Les clauses de loop_clause_set sont non guidees */
		fprintf (parser_file, "if (fsa_clause2ub (%ld, iub[0])", (SXINT) clause);

		if (rfsa_file)
		  fprintf (parser_file, " && fsa_clause2lb (%ld, ilb[0])", (SXINT) clause);

		fputs (") {\n", parser_file);
	      }

	      if (is_lex_clause || is_1_RCG) {
		/* la clause courante a des terminaux ds ses args en LHS ou utilisation du guide */

		if (A_clause_nb == 1) {

#if EBUG
		  if (clause_id != 1)
		    sxtrap (ME, "gen_parser");
#endif

		  fputs ("#if is_lex\n", parser_file);
#if 0
		  fprintf (parser_file, "if (lex_code [nt2lex_code [%ld]] == 1)\n", A);
#endif /* 0 */


		  /*  Le 22/5/2002 */
		  if (Lex_file) {
		    /* Ds le cas is_earley_guide, *rho0 est calcule par l'appelant (arg in) */
		    /* ON suppose que si is_earley_guide => is_lex et is_dynam_lex=0 */
		    fprintf (parser_file, "#if is_earley_guide\n\
if (SXBA_bit_is_set (ptcmemoP [Aij_pid2Aij (*rho0)], 1))\n\
#else /* !is_earley_guide */\n\
if (SXBA_bit_is_set (nt2clause_id_set_%ld, 1))\n\
#endif /* !is_earley_guide */\n", (SXINT) A);
		  }
		  else {
		    if (is_1_RCG) {
		      fputs ("#if is_guided4\n", parser_file);

		      if (A_arity == 1) {
			fputs ("if (SXBA_bit_is_set (ptcmemoP [tcrho [0]], 1))\n", parser_file);
		      }
		      else {
			for (pos = 0; pos < A_arity; pos++) {
			  if (pos == 0)
			    fputs ("if (SXBA_bit_is_set (ptcmemoP [tcrho [0]], 1)", parser_file);
			  else
			    fprintf (parser_file, "\n&& SXBA_bit_is_set (ptcmemoP [tcrho [%ld]], 1)", (SXINT) pos);
			}

			fputs (")\n", parser_file);
		      }

		      fputs ("#else /* !is_guided4 */\n", parser_file);
		      fprintf (parser_file, "if (SXBA_bit_is_set (nt2clause_id_set_%ld, 1))\n", (SXINT) A);
		      fputs ("#endif /* !is_guided4 */\n", parser_file);
		    }
		    else {
		      fprintf (parser_file, "if (SXBA_bit_is_set (nt2clause_id_set_%ld, 1))\n", (SXINT) A);
		    }
		  }

		  fputs ("{\n", parser_file);

		  fputs ("#endif /* is_lex */\n", parser_file);
		}
	      }
		    
	      if (is_lex_clause || is_1_RCG) {
		/* la clause courante a des terminaux ds ses args en LHS ou version guidee */
		gen_dynam_lex (clause/* , A_arity */);
	      }
		    
	      mprdct_pos2generated [0] = 0; /* rho[1] est affecte */

	      if (A_arity) {
		wvstr = clause2varstr (varstr_raz (wvstr), clause);
		fprintf (parser_file, "#if EBUG\nprintf (\"(%%s)%s\\n\", G.name);\n#endif\n",
			 varstr_tostr (wvstr));

		fputs ("#if EBUG2\n", parser_file);
		fputs ("clause_calls++;\n", parser_file);
		fputs ("#endif\n", parser_file);

		fprintf (parser_file, "rho [0] = %ld;\n", (SXINT) clause);
		fprintf (parser_file, "rho [%ld] = %ld /* #terminals in clause #%ld */;\n", (SXINT)  cur_rho_size, (SXINT) lhs_t_nb, (SXINT) clause);

		if (SXBA_bit_is_set (ext_decl_clause_set, clause))
		  fprintf (parser_file, "#if 0\n\
if (is_useful_%ld)\n\
#endif /* 0 */\n", (SXINT) clause);

		/* Le Ven 19 Jan 2001 13:44:51 */
		if (is_1_RCG) {
		  SXINT rhs_pos, rhs_nt, cur3, bot3, top3, arg_no;

		  fputs ("#if is_fully_guided\n", parser_file);

		  /* SUPPRIMER k_guide_set */

		  fputs ("if (", parser_file);

		  for (k = 0; k < A_arity; k++) {
		    if (k == 0)
		      fputs ("GET_FULL_GUIDE (pnode", parser_file);
		    else
		      fputs (" &&\nGET_FULL_GUIDE (pnodei", parser_file);

		    fprintf (parser_file, "%ld, G.pid, tcrho [%ld], %ld)",
			     (SXINT) k, (SXINT) k, (SXINT) clausek2guide_clause [clause_k2clausek (clause, k)]);
		  }

		  fputs (") {\n", parser_file);

		  for (k = 0; k < A_arity; k++) {
		    if (k > 0)
		      fprintf (parser_file, "pnode%ld = pnodei%ld;\n", (SXINT) k, (SXINT) k);

		    fputs ("do {\n", parser_file);

		    bot2 = clause2rhs_pos [clause];
		    top2 = clause2rhs_pos [clause+1];
		    rhs_pos = 0;

		    while (bot2 < top2) {
		      rhs_nt = rhs_pos2rhs_nt [bot2];

		      if (rhs_nt > 0 || rhs_nt == TRUE_ic) {
			/* DS "print_instantiated predicate" */
			/* ON NE GARDE QUE LES NT>0 ET LES &True ... */
			/* ... et les autres appels ds le cas 1rcg sont en fin de rhs */
			top3 = rhs_pos2disp [bot2+1];
			bot3 = rhs_pos2disp [bot2];

			for (cur3 = bot3; cur3 < top3; cur3++) {
			  arg_no = rhs_pos2lhs_arg_no [cur3];

			  if (arg_no == k) {
			    pos = rhs_pos2rhs_pos_1rcg [cur3];
			    fprintf (parser_file,
				     "range = pnode%ld->rho[%ld];\n",
				     (SXINT) k, (SXINT) pos);

			    if (cur3 == bot3) {
			      fprintf (parser_file,
				       "olb[%ld] = RANGE2i(range), oub[%ld] = RANGE2j(range);\n",
				       (SXINT) rhs_pos, (SXINT) rhs_pos);
			    }
			    else {
			      fprintf (parser_file,
				       "if (olb[%ld] != RANGE2i(range) || oub[%ld] != RANGE2j(range)) continue;\n",
				       (SXINT) rhs_pos, (SXINT) rhs_pos);
			    }
			  }
			}
		      }

		      bot2++;
		      rhs_pos++;
		    }
		  }

		  if (clause_has_cut)
		    fputs ("cut = false;\n", parser_file);

		  gen_guided_bounds (clause);

		  /* En ordre inverse */
		  for (k = A_arity-1; 0 <= k; k--) {
		    fprintf (parser_file, "} while (pnode%ld ? pnode%ld = pnode%ld->next : NULL);\n", (SXINT) k, (SXINT) k, (SXINT) k);
		  }

		  fputs ("}\n", parser_file);

		  fputs ("#else /* !is_fully_guided */\n", parser_file);
		}

		fputs ("do {\n", parser_file);

		if (clause_has_cut)
		  fputs ("cut = false;\n", parser_file);

		/* Le 22/04/08 on distingue les cas is_dag et !is_dag pour la generation */
		
		fputs ("#if is_dag\n\
if (input_is_a_dag) {\n", parser_file);

		gen_dag_parser (clause);

		fputs ("} /* input_is_a_dag */\n\
else {\n\
#endif /* is_dag */\n", parser_file);

#if 0
		process_lhs_constant_args (clause); /* Essai */
		process_rhs_constant_args (clause); /* Essai */
#endif /* 0 */

		gen_t_bounds (clause);
		
		fputs ("#if is_dag\n\
} /* !input_is_a_dag */\n\
#endif /* is_dag */\n", parser_file);

#if EBUG
		if (sxba_cardinal (lhs_arg_tbp_set))
		  sxtrap (ME, "gen_parser");
#endif /* EBUG */
		degree = 0;

		for (k = 0; k < A_arity; k++) {
		  if ((d = for_loops_nb [k]) > 0) {
		    for_loops_nb [k] = 0;
		    degree += d + 2;
		  }
		}

		if (degree > max_degree) {
		  max_degree = degree;
		  max_degree_clause = clause;
		}

		fputs ("} while (0);\n", parser_file);
		    
		if (is_1_RCG) {
		  fputs ("#endif /* !is_fully_guided */\n", parser_file);
		}

		if (is_1_RCG) {
#if 0
		  fputs ("#if is_guided4\n}\n#endif /* if (coupled_clause () ...) */\n", parser_file);
#endif /* 0 */
		}
		else {
		  if (is_lex_clause) {
		    fputs ("#if is_dynam_lex\n}\n#endif /* if (dynam_lex () ...) */\n", parser_file);
		  }
		}
			
		if (lfsa_file && !SXBA_bit_is_set (loop_clause_set, clause))
		  fputs ("}\n", parser_file);

		if (is_lex_clause || is_1_RCG) {
		  if (A_clause_nb == 1) {
		    fputs ("#if is_lex\n} /* End if (lex_code [nt2lex_code [...]] == 1) */\n#endif\n", parser_file);
		  }
		}
	      }

	      if (A_clause_nb > 1) {
		/* Le 05/02/04 chaque case a son break !! */
		fputs ("#if is_lex\n\
break;\n\
#else /* !is_lex */\n\
#if is_recognizer\n\
if (ret_val > 0) break;\n\
#endif /* is_recognizer */\n\
/* On continue en sequence */\n\
#endif /* !is_lex */\n", parser_file);
	      }
	    }
	    else {
	      /* Sur les clauses identiques ou les loop, on genere un case qui ne fait rien au cas ou ... */
	      if ((main_clause = clause2identical [clause]) != clause) {
		gen_clause_comment (parser_file, clause);
		fprintf (parser_file, "/* %ld: Useless clause, identical to #%ld (id = %ld) */\n",
			 (SXINT) clause, (SXINT) main_clause, (SXINT) clause_id);
#if 0
		/* Si on veut quand meme faire qqchose sur les identiques... */
		if (A_clause_nb > 1) {
		  fprintf (parser_file, "#if is_lex\n\
case %ld:\n\
break;\n\
#endif /* is_lex */\n", clause_id);
		}
#endif /* 0 */
	      }
	      else if (SXBA_bit_is_set (loop_clause_set, clause)) {
		gen_clause_comment (parser_file, clause);
		fprintf (parser_file, "/* Loop (id = %ld) */\n", (SXINT) clause_id);
#if 0
		/* Si on veut quand meme faire qqchose sur les loops... */
		if (A_clause_nb > 1) {
		  fprintf (parser_file, "#if is_lex\n\
case %ld:\n\
break;\n\
#endif /* is_lex */\n", clause_id);
		}
#endif /* 0 */
	      }
	      else {
		gen_clause_comment (parser_file, clause);
		fputs ("/* False clause */\n", parser_file);

#if 0
		/* Enleve' le 09/02/04 */
		/* ATTENTION, ds le cas False, clause_id == clause2clause_id [clause] == 0 */
		/* Ajoute' le 05/02/04 */
		if (A_clause_nb > 1) {
		  fprintf (parser_file, "#if EBUG4\n\
#if is_lex\n\
case %ld:\n\
break;\n\
#endif /* is_lex */\n\
#endif /* EBUG4 */\n", clause_id);
		}
#endif /* 0 */
		/* SXBA_0_bit (A_clause_set, clause);
		   A_clause_nb--; Enleve le Ven 23 Jun 2000 09:47:54 */
	      }

	      sxba_empty (lhs_arg_tbp_set); /* Ajoute le Mer 4 Fev 2004 10:55 */
	      SXBA_0_bit (lhsnt2clause [A], clause); /* Ajoute le Ven 23 Jun 2000 10:05:57 */
	      clause2clause_id [clause] = 0; /* Ajoute le mer 11 fev 2004 */
	      SXBA_0_bit (t2clause_set [0], clause); /* Ajoute le mer 11 fev 2004 */
	    }

	    /* Pour l'acces aux memoP, il faut compter les clauses identiques */
	    /* Ca permet un decompte identique ds un parseur guide' si celui qu'on est en train
	       de construire sert de guide */
	    /* ... mais pas les false */
	    /* clause_id++; */
	    clear_struct ();
	  }

	  /* Pour la seconde passe !!! */
#if 0
	  /* Enleve' le Jeu 16 Nov 2000 11:12:33 */
	  sxba_and (A_clause_set, non_false_A_clause_set); /* Ajoute' Le Jeu  2 Nov 2000 13:42:51 */
#endif


	  if (A_clause_nb > 1) {
	    fprintf (parser_file, "#if is_lex\n\
default:;\n\
#if EBUG4\n\
sxtrap (G.name, \"%s\");\n\
#endif /* EBUG4 */\n\
}\n\
#endif /* is_lex */\n", sxstrget (nt2ste [A]));
	  }

#if is_memop
	  fputs ("#if is_memop && is_single_pass\n", parser_file);

	  if (A_clause_nb > 1) {
	    if (is_cyclic_grammar) {
	      fputs ("if (local_ret_val > 0)\n\
#if is_cyclic\n\
if (!is_glbl_cycle)\n\
#endif\n\
SXBA_1_bit (memoP [Aij], clause_id);\n", parser_file);
	    }
	    else {
	      fputs ("if (local_ret_val > 0) SXBA_1_bit (memoP [Aij], clause_id);\n", parser_file);
	    }
	  }
	  else {
	    fputs ("if (ret_val > 0) SXBA_1_bit (memoP [Aij], 1);\n", parser_file);
	  }

	  fputs ("#endif /* is_memop && is_single_pass */\n", parser_file);
#endif /* is_memop */

	  if (A_clause_nb > 1) {
	    /* Enleve' le 28/02/08 */
#if 0	    
	    fputs ("#if is_recognizer\n\
if (ret_val > 0) break;\n\
#endif\n", parser_file);
#endif /* 0 */

	    if (is_cyclic_grammar) {
	      /* Si on n'est pas au niveau 1 et si l'ip est vrai, on peut quitter */
	      fputs ("#if is_cyclic\n\
if (ret_val > 0 && is_glbl_cycle) break;\n\
#endif\n", parser_file);
	    }

	    fputs ("#if is_lex\n\
}\n\
#else /* !is_lex */\n\
} while (0); /* Fin du do bidon */\n\
#endif /* !is_lex */\n", parser_file);
	  }

	  if (A_arity) {
#if is_memon==1
	    fputs ("#if is_memon\n", parser_file);
#endif /* is_memon==1 */

	    fputs ("if (ret_val > 0) SXBA_1_bit (memoN1, Aij);\n", parser_file);
		    
	    /* Dans les autres cas memoN0 est positionne de`s l'entree */
	    if (is_cyclic_grammar) {
	      fputs ("#if is_cyclic\n", parser_file);
	      fputs ("if (!is_glbl_cycle) SXBA_1_bit (memoN0, Aij);\n", parser_file);
	      fputs ("#endif /* is_cyclic */\n", parser_file);
	    }
#if 0
	    if (is_cyclic_grammar) {
	      fputs ("#if is_cyclic\n", parser_file);
	      fputs ("if (!is_glbl_cycle) SXBA_1_bit (memoN0, Aij);\n", parser_file);
	      fputs ("#else /* !is_cyclic */\n", parser_file);
	      fputs ("SXBA_1_bit (memoN0, Aij);\n", parser_file);
	      fputs ("#endif /* !is_cyclic */\n", parser_file);
	    }
	    else
	      fputs ("SXBA_1_bit (memoN0, Aij);\n", parser_file);
#endif /* 0 */
	  }

#if is_memon==1
	  fputs ("#endif /* is_memon */\n", parser_file);
#endif /* is_memon==1 */

	  if (A_is_loop || A_is_cyclic) {
	    fputs ("#if is_cyclic\n", parser_file);
		      
	    if (A_is_loop && A_is_cyclic)
	      fputs ("if (ret_val > 0 && !is_glbl_cycle) (*G.semact.first_pass_loop)(&G, rho, LOOP+(SXBA_bit_is_set (memoC [3], Aij) ? CYCLE : 0));\n", parser_file);
	    else {
	      if (A_is_cyclic)
		fputs ("if (ret_val > 0 && !is_glbl_cycle && SXBA_bit_is_set (memoC [3], Aij)) (*G.semact.first_pass_loop)(&G, rho, CYCLE);\n", parser_file);
	      else
		fputs ("if (ret_val > 0) (*G.semact.first_pass_loop)(&G, rho, LOOP);\n", parser_file);
	    }

	    if (A_is_cyclic) {
	      fputs ("is_glbl_cycle = is_local_cycle;\n", parser_file);
	      fputs ("if (is_cyclic_ip) SXBA_0_bit (memoC [1], Aij);\n", parser_file);
	      fputs ("else SXBA_0_bit (memoC [0], Aij);\n", parser_file);
	    }

	    fputs ("#endif /* is_cyclic */\n", parser_file);
	  }

	  fprintf (parser_file, "#if EBUG\nprintf (\"(%%s)out(%%ld): %s (", sxstrget (nt2ste [A]));
 
	  for (pos = 0; pos < A_arity; pos++) {
	    fputs ("%ld..%ld", parser_file);

	    if (pos+1 < A_arity)
	      fputs (", ", parser_file);
	  }

	  fputs (") %s\\n\", G.name, call_level, ", parser_file);

	  for (pos = 0; pos < A_arity; pos++) {
	    fprintf (parser_file, "ilb[%ld], iub[%ld], ", (SXINT) pos, (SXINT) pos);
	  }

	  fputs ("ret_val==-1 ? \"FALSE\": \"TRUE\" );\ncall_level--;\n#endif\n", parser_file);

	  fputs ("#if is_generator\nreturn ret_vals[0];\n#else\nreturn ret_val;\n#endif\n", parser_file);
	}

	fprintf (parser_file, "} /* _%s () */\n", Astr);
      }
    }
  }

#if 0
  if (has_semantics) {
    /* On genere les fonctions qui assurent la seconde passe */
#if is_semantics==1
    fputs ("\n#if is_semantics && is_multiple_pass\n", parser_file);
#else
    fputs ("\n#if is_multiple_pass\n", parser_file);
#endif /* is_semantics==1 */

    for (A = 1; A <= max_nt; A++) {
      if (SXBA_bit_is_set (is_lhs_nt, A)) {
	/* On ne s'occupe que des predicats qui sont definis localement */
	if ((main_A = nt2identical [A]) != A) {
	  fprintf (parser_file, "/* Predicate %s is identical to %s */\n", sxstrget (nt2ste [A]), sxstrget (nt2ste [main_A]));
	}
	else {
	  A_arity = nt2arity [A];
	  A_is_internal = SXBA_bit_is_set (is_nt_internal, A);
	  A_is_loop = SXBA_bit_is_set (loop_nt_set, A);
	  A_is_cyclic = SXBA_bit_is_set (cyclic_nt_set, A);
	  A_clause_set = lhsnt2clause [A];
	  A_clause_nb = sxba_cardinal (A_clause_set);
	  A_has_neg_call = is_negative && SXBA_bit_is_set (negative_nt_set, A);
		
	  if (A_is_internal)
	    fputs ("static ", parser_file);

	  fprintf (parser_file,
		   "SXINT\n_walk_%s (SXINT rho0)\n{\n",
		   sxstrget (nt2ste [A]));

	  if (A_clause_nb) {
	    /* Le 19/6/2002 La propagation des false peut supprimer des evaluations de predicats */

	    fputs ("SXINT\t*p, clause, clause_id;\n", parser_file);
	    fputs ("bool\tret_val;\n", parser_file);
	    fputs ("struct spf_node\t**prev_icp;\n", parser_file);

	    if (A_is_cyclic) {
	      fputs ("#if is_cyclic\n", parser_file);
	      fputs ("bool\tis_cyclic_ip, local_ret_val;\n", parser_file);
	      fputs ("struct spf_node\t**hd_icp;\n", parser_file);
	      fputs ("#endif\n", parser_file);
	    }

	    /* Le Lun 22 Jan 2001 10:25:51, memoN1 et memoP peuvent etre utilises ds le cas guide */
	    /* Le cas (ds xtag) A -> A A .  et A -> . doit pouvoir etre traite */
	    /* Correct, meme si la grammaire est cyclique */
	    fputs ("\n#if EBUG\ninstantiated_prdct2struct (rho0);\n", parser_file);

	    if (A_has_neg_call
		/* Ajout le 19/05/04 */
		|| SXBA_bit_is_set (is_nt_external, A) /* Il peut y avoir un appel negatif depuis un autre module */
		) {
	      fprintf (parser_file, "if (rho0 < 0)\nprintf (\"(%%s)in_out(%%ld): %s (",
		       sxstrget (nt2ste [A]));
 
	      for (pos = 0; pos < A_arity; pos++) {
		fputs ("%ld..%ld", parser_file);

		if (pos+1 < A_arity)
		  fputs (", ", parser_file);
	      }

	      fputs (") FALSE\\n\", G.name, call_level", parser_file);

	      for (pos = 0; pos < A_arity; pos++) {
		fprintf (parser_file, ", lower_bounds[%ld], upper_bounds[%ld]", (SXINT) pos, (SXINT) pos);
	      }

	      fputs (");\nelse {\n", parser_file);
	    }
	      
	    fprintf (parser_file, "call_level++;\nprintf (\"(%%s)in(%%ld): %s (",
		     sxstrget (nt2ste [A]));
 
	    for (pos = 0; pos < A_arity; pos++) {
	      fputs ("%ld..%ld", parser_file);

	      if (pos+1 < A_arity)
		fputs (", ", parser_file);
	    }

	    fputs (")\\n\", G.name, call_level", parser_file);

	    for (pos = 0; pos < A_arity; pos++) {
	      fprintf (parser_file, ", lower_bounds[%ld], upper_bounds[%ld]", (SXINT) pos, (SXINT) pos);
	    }

	    fputs (");\n", parser_file);

	    if (A_has_neg_call
		/* Ajout le 19/05/04 */
		|| SXBA_bit_is_set (is_nt_external, A) /* Il peut y avoir un appel negatif depuis un autre module */
		)
	      fputs ("}\n", parser_file);

	    fputs ("#endif /* EBUG */\n\n", parser_file);

	    /* Le 11/02/04 */
	    if (A_has_neg_call
		/* Ajout le 19/05/04 */
		|| SXBA_bit_is_set (is_nt_external, A) /* Il peut y avoir un appel negatif depuis un autre module */
		) {
	      /* Il y a qq part des appels negatifs sur le predicat courant */
	      /* Appel negatif */
	      fputs ("if (rho0 < 0) return -1;\n\n", parser_file);
	    }

	    fputs ("if (!(ret_val = SXBA_bit_is_set (memoN1, rho0))\n", parser_file);

	    if (A_is_cyclic)
	      fputs ("#if is_cyclic\n\
&& (SXBA_bit_is_set (memoC [1], rho0) ? (ret_val = SXBA_bit_is_set (memoN0, rho0), false) : true)\n\
#endif\n", parser_file);

	    fputs (") {\n", parser_file);

	    if (A_is_cyclic)
	      fputs ("\n#if is_cyclic\n\
is_cyclic_ip = SXBA_bit_is_reset_set (memoC [0], rho0) ? false : (SXBA_1_bit (memoC [1], rho0), true);\n\
\n\
if (*(hd_icp = prev_icp = &GET_SEM_NODE_PTR (rho0)))\n\
#else\n\
if (*(prev_icp = &GET_SEM_NODE_PTR (rho0)))\n\
#endif\n", parser_file);
	    else
	      fputs ("if (*(prev_icp = &GET_SEM_NODE_PTR (rho0)))\n", parser_file);

	    fputs ("{\n", parser_file);
	    fputs ("do {\n", parser_file);
	    fputs ("do {\n", parser_file);

	    fputs ("p = &((*prev_icp)->rho [0]);\n", parser_file);

	    if (A_is_cyclic) {
	      fputs ("#if is_cyclic\n\
local_ret_val = false;\n\
#endif\n", parser_file);
	    }

	    if (A_clause_nb > 1) {
	      fputs ("clause = *p;\n", parser_file);

	      /* LE Jeu 16 Nov 2000 11:02:39 */
#if 0
	      fputs ("switch (clause2Aclause [clause]) {\n", parser_file);
#endif /* 0 */
	      fputs ("switch (clause_id = clause2clause_id [clause]) {\n", parser_file);
	      /* On traite les clauses en ordre inverse!! */
	      /* LE Jeu 16 Nov 2000 11:02:39 : on reprend l'ordre normal */
	      clause = 0;

	      while ((clause = sxba_scan (A_clause_set, clause)) > 0) {
		/* On sort le source de la clause en commentaire */
		gen_clause_comment (parser_file, clause);

#if 0
		fprintf (parser_file, "case %ld:\n", clause2Aclause [clause]);
#endif /* 0 */
		fprintf (parser_file, "case %ld:\n", (SXINT) clause2clause_id [clause]);

					 rhs = XxY_Y (rcg_clauses, clause);
					 top = XH_X (rcg_rhs, rhs+1);
					 bot = XH_X (rcg_rhs, rhs);
					 son_nb = 0;
					 is_first_call = true;

					 for (cur = bot; cur < top; cur++) {
					   prdct = XH_list_elem (rcg_rhs, cur);
					   bot2 = XH_X (rcg_predicates, prdct);
					   nt = XH_list_elem (rcg_predicates, bot2+1);

					   if (XH_list_elem (rcg_predicates, bot2) == 0) {
					     /* Positive call */
					     if (nt > 0) {
					       son_nb++;

					       if (is_first_call) {
						 is_first_call = false;
						 fputs ("if (", parser_file);
					       }
					       else
						 fputs ("\n|| ", parser_file);

					       fprintf (parser_file,
							"_walk_%s (p [%ld]) == -1",
							sxstrget (nt2ste [nt]), (SXINT) son_nb+1);
					     }
					     else {
					       if ((is_drcg && nt == STREQ_ic && is_streq_spcl (bot2+2)) || nt == TRUE_ic)
						 /* Le rho correspondant est occupe */
						 son_nb++;
					     }
					   }
					   else {
					     /* Le 11/02/04 on traite les neg calls */
					     if (nt > 0) {
					       son_nb++;

					       if (is_first_call) {
						 is_first_call = false;
						 fputs ("if (", parser_file);
					       }
					       else
						 fputs ("\n|| ", parser_file);

					       fprintf (parser_file,
							"-_walk_%s (p [%ld]) == -1",
							sxstrget (nt2ste [nt]), (SXINT) son_nb+1);
					     }
					   }
					 }


					 if (!is_first_call)
					   fputs (") continue;\n", parser_file);

					 /* Modif du Mar  7 Nov 2000 14:07:00 : pour chaque clause le nombre de fils et chacun de leur pid sont
					    stockes ds clause2son_nb et clause2son_pid */
#if 0
					 fprintf (parser_file, "clause_id = %ld;\n", clause2clause_id [clause]);
#endif
					 fputs ("break;\n", parser_file);
	      }

	      fputs ("#if EBUG4\n", parser_file);
	      fputs ("default:\n", parser_file);
	      fprintf (parser_file, "sxtrap (G.name, \"_walk_%s\");\n", sxstrget (nt2ste [A]));
	      fputs ("#endif\n", parser_file);
	      fputs ("}\n", parser_file);
	    }
	    else {
	      clause = sxba_scan (A_clause_set, 0);


	      /* On sort le source de la clause en commentaire */
	      gen_clause_comment (parser_file, clause);

	      rhs = XxY_Y (rcg_clauses, clause);
	      top = XH_X (rcg_rhs, rhs+1);
	      bot = XH_X (rcg_rhs, rhs);
	      son_nb = 0;
	      is_first_call = true;

	      for (cur = bot; cur < top; cur++) {
		prdct = XH_list_elem (rcg_rhs, cur);
		bot2 = XH_X (rcg_predicates, prdct);
		nt = XH_list_elem (rcg_predicates, bot2+1);

		if (XH_list_elem (rcg_predicates, bot2) == 0) {
		  /* Positive call */
		  if (nt > 0) {
		    son_nb++;

		    if (is_first_call) {
		      is_first_call = false;
		      fputs ("if (", parser_file);
		    }
		    else
		      fputs ("\n|| ", parser_file);

		    fprintf (parser_file,
			     "_walk_%s (p [%ld]) == -1",
			     sxstrget (nt2ste [nt]), (SXINT) son_nb+1);
		  }
		  else {
		    if ((is_drcg && nt == STREQ_ic && is_streq_spcl (bot2+2)) || nt == TRUE_ic)
		      /* Le rho correspondant est occupe */
		      son_nb++;
		  }
		}
		else {
		  /* Le 11/02/04 on traite les neg calls */
		  if (nt > 0) {
		    son_nb++;

		    if (is_first_call) {
		      is_first_call = false;
		      fputs ("if (", parser_file);
		    }
		    else
		      fputs ("\n|| ", parser_file);

		    fprintf (parser_file,
			     "-_walk_%s (p [%ld]) == -1",
			     sxstrget (nt2ste [nt]), (SXINT) son_nb+1);
		  }
		}
	      }

	      if (!is_first_call) {
		fputs (") continue;\n", parser_file);
	      }
	    }

	    if (A_is_cyclic) {
	      fputs ("#if is_cyclic\n\
SXBA_1_bit (memoN0, rho0);\n\
local_ret_val =\n\
#endif\n", parser_file);
	    }
		      
	    fputs ("ret_val = true;\n", parser_file);

	    if (A_is_cyclic) {
	      fputs ("#if is_cyclic\nif (p [1]) {\n#endif\n", parser_file);
	    }

#if is_memop
	    fputs ("if ((*G.semact.second_pass) && (*G.semact.second_pass)(&G, p) != -1) {\n", parser_file);
	    fputs ("#if is_memop\n", parser_file);

	    /* On change la taille si le parseur courant construit un guide */
	    fprintf (parser_file, "#if is_build_coupled_guide\n\
memoP [rho0] [0] = %ld;\n\
#endif /* is_build_coupled_guide */\n", A2max_clause_id [A]+1);

	    if (A_clause_nb == 1/* && is_first_call */)
	      fprintf (parser_file, "SXBA_1_bit (memoP [rho0], %ld);\n", clause2clause_id [clause]);
	    else
	      fputs ("SXBA_1_bit (memoP [rho0], clause_id);\n", parser_file);

	    fputs ("#endif /* is_memop */\n", parser_file);

	    fputs ("}\n", parser_file);
#endif /* is_memop */

	    fputs ("if (*G.semact.second_pass) (*G.semact.second_pass)(&G, p);\n", parser_file);

	    if (A_is_cyclic)
	      fputs ("#if is_cyclic\n\
if (is_cyclic_ip) p [1] = 0;\n\
}\n\
if (!is_cyclic_ip)\n\
#endif\n", parser_file);

	    fputs ("*prev_icp = (*prev_icp)->next;\n", parser_file);
		      
	    fputs ("} while (false);\n", parser_file);

	    if (A_is_cyclic) {
	      fputs ("#if is_cyclic\nif (!local_ret_val || is_cyclic_ip) prev_icp = &((*prev_icp)->next);\n#endif\n", parser_file);
	    }

	    /* Le 26 octobre 2001 J'y comprends plus rien, je reviens au + simple! */
	    fputs ("} while (*prev_icp);\n", parser_file);    

	    fputs ("} \n", parser_file);

	    if (A_is_cyclic) {
	      fputs ("\n#if is_cyclic\n", parser_file);
	      fputs ("if (is_cyclic_ip) SXBA_0_bit (memoC [1], rho0); else SXBA_0_bit (memoC [0], rho0);\n", parser_file);
	      fputs ("if (*hd_icp == NULL) {\n", parser_file);

	      /* ATTENTION, p designe une A-clause instanciee quelconque */
	      if (A_is_loop) {
		fputs ("p [1] = rho0;\n", parser_file);
		fputs ("if (*G.semact.second_pass_loop) (*G.semact.second_pass_loop)(&G, p, LOOP);\n", parser_file);
	      }

	      fputs ("SXBA_1_bit (memoN1, rho0);\n\
}\n", parser_file);
	      fputs ("#else\n\
SXBA_1_bit (memoN1, rho0);\n\
#endif /* !is_cyclic */\n", parser_file);
	    }
	    else {
	      if (A_is_loop) {
		fputs ("\n#if is_cyclic\n", parser_file);
		fputs ("p [1] = rho0;\n", parser_file);
		fputs ("if (*G.semact.second_pass_loop) (*G.semact.second_pass_loop)(&G, p, LOOP);\n", parser_file);
		fputs ("#endif /* is_cyclic */\n", parser_file);
	      }

	      fputs ("SXBA_1_bit (memoN1, rho0);\n", parser_file);
	    }

	    fputs ("}\n", parser_file);

	    fprintf (parser_file, "\n#if EBUG\ninstantiated_prdct2struct (rho0);\nprintf (\"(%%s)out(%%ld): %s (", sxstrget (nt2ste [A]));
 
	    for (pos = 0; pos < A_arity; pos++) {
	      fputs ("%ld..%ld", parser_file);

	      if (pos+1 < A_arity)
		fputs (", ", parser_file);
	    }

	    fputs (") %s\\n\", G.name, call_level", parser_file);

	    for (pos = 0; pos < A_arity; pos++) {
	      fprintf (parser_file, ", lower_bounds[%ld], upper_bounds[%ld]", (SXINT) pos, (SXINT) pos);
	    }

	    fputs (", ret_val ? \"TRUE\" : \"FALSE\");\ncall_level--;\n#endif\n", parser_file);

	    fputs ("return ret_val ? 1 : -1;\n", parser_file);

	  } /* if (A_clause_nb) */
	  else {
	    /* Le 11/02/04 A voir !! */
	     fputs ("return -1;\n", parser_file);
	  }

	  fprintf (parser_file, "} /* _walk_%s () */\n\n", sxstrget (nt2ste [A]));
	}
      }
    }

#if is_semantics==1
    fputs ("#endif /* is_semantics && is_multiple_pass */\n", parser_file);
#else
    fputs ("#endif /* is_multiple_pass */\n", parser_file);
#endif /* is_semantics==1 */
  }
#endif /* 0 */

  fputs ("\n#endif /* !is_rav2 */\n", parser_file);
  fputs ("#endif /* !is_grammar_alone */\n", parser_file);

  /* Le 11/02/04 j'ai deplace la sortie de clause2clause_id et t2clause_set car la generation
     fill_param_pos () a pu detecter des clauses false, la nouvelle version modifie clause2clause_id et t2clause_set
   */
  output_clause2clause_id ();

  {
    SXBA_INDEX	*repr;
    repr = (SXBA_INDEX*) sxalloc (max_t+1, sizeof (SXBA_INDEX));

    output_t2clause_set (repr);

    gen_if (repr);

    sxfree (repr);
  }

  if (Lex) sxbm_free (Lex), Lex = NULL;
  sxbm_free (t2clause_set), t2clause_set = NULL;

  if (has_sem_act)
    gen_sem_act ();

  gen_lexicalizer ();
}


static void
escape_fprintf (FILE *out_file, char *str)
{
  while (*str != SXNUL) {
    if (*str == '\\' || *str == '"')
      fprintf (out_file, "\\%c",*str);
    else
      fprintf (out_file, "%c",*str);

    str++;
  }
}


static void
rcg_terminal (void)
{
  SXINT cur_time, t;
  char  car, *str, *Cstr;
  VARSTR vstr;

  vstr = varstr_alloc (128);

  cur_time = time (0);  /* now */

  fprintf (terminal_file,
	   "\
/* ***************************************************************************************\n\
\tThe following terminal symbol list part has been extracted on %s\
\tby the SYNTAX(*) processor for Range Concatenation Grammars running with the \"-terminal %s%s\" option\n\
\tfrom the RCG \"%s\"\n\
******************************************************************************************\n\
\t(*) SYNTAX is a trademark of INRIA.\n\
****************************************************************************************** */\n\n\n",
	   ctime (&cur_time),
	   terminal_file_name,
	   is_col1 ? " -col1" : "",
	   (prgentname == NULL) ? "stdin" : prgentname);

  for (t =1; t <= max_t; t++) {
    str = sxstrget (t2ste [t]);

    if (*str == '"')
      str++; /* !! */

    if (!is_col1)
      /* On ne genere pas le terminal en colonne 1 */
      fputs ("\t", terminal_file);

    fputs ("\"", terminal_file);
    escape_fprintf (terminal_file, str);
    fputs ("\"", terminal_file);

    varstr_raz (vstr);

    while ((car = *str++)) {
      Cstr = SXCHAR2CSTR (car);
      vstr = varstr_catenate (vstr, Cstr);
    }

    Cstr = varstr_tostr (vstr);

    t2Cid_ste [t] = sxstr2save (Cstr, varstr_length (vstr));

    /* On commente la sortie */
    fprintf (terminal_file, "\t/* %ld SXT_%s */\n", t, Cstr);
  }

  varstr_free (vstr);
}

void
rcg_gen_parser (void)
{
  SXINT	x, nt, arity, pos, *ptr, Ak, clause;

  if (sxverbosep) {
    if (is_a_complete_terminal_grammar)
      fputs ("\tgen terminal_parser .... ", sxtty);
    else
      fputs ("\tgen parser .... ", sxtty);

    sxttycol1p = false;
  }

  /* On passe certains ensembles a leur taille min */
  /* ... car on les utilise comme operande de sxba_* avec des sxba de taille max_nt+1 */
  SXBASIZE (is_lhs_nt) = max_nt+1; /* initialement max_ste+1 */
  SXBASIZE (is_nt_external) = max_nt+1; /* initialement max_ste+1 */ 
  SXBASIZE (is_nt_internal) = max_nt+1; /* initialement max_ste+1 */

  if (terminal_file) {
    /* On sort la liste des terminaux apres transcodage de chaque char vers qqchose qui
       peut etre (une partie d') identifieur C */
    t2Cid_ste = (SXINT*) sxalloc (max_t+1, sizeof (SXINT));

    rcg_terminal ();
  }

  parser_file = stdout; /* initialisation statique refuse'e sous windowsNT */

  has_external = sxba_scan (is_nt_external, 0) > 0;
  ext_decl_set = sxba_calloc (max_nt+1);
  ext_decl2clause = (SXINT*) sxcalloc (max_nt+1, sizeof (SXINT));
  ext_decl_clause_set = sxba_calloc (last_clause+1);

  if (is_a_complete_terminal_grammar) {
    /* Toutes les clauses sont de la forme
       A (t) --> .
    */
    gen_terminal_parser ();

    if (sxverbosep) {
      fputs ("done\n", sxtty);
      sxttycol1p = true;
    }
  }
  else
#if 0
    if (is_robust) {
      gen_robust_parser ();

      if (sxverbosep) {
	fputs ("done\n", sxtty);
	sxttycol1p = true;
      }
    }
    else 
#endif /* #if 0 */
      {
	wvstr = varstr_alloc (128);
	vstr_cond = varstr_alloc (32);
	vstr_body = varstr_alloc (32);
	t_indexes_vstr = varstr_alloc (32);

	varstrings_top = 2*(max_garity)+3*(max_lhs_adr)+1*(max_gvar);
	varstrings = (VARSTR*) sxalloc (varstrings_top+1, sizeof (VARSTR));

	for (x = varstrings_top; x >= 0; x--) {
	  varstrings [x] = varstr_alloc (32);
	}

	lhs_pos2lvstr = varstrings;
	lhs_pos2rvstr = lhs_pos2lvstr + max_garity;
	madr2lcode = lhs_pos2rvstr + max_garity;
	madr2rcode = madr2lcode + max_lhs_adr;
	synchro2vstr = madr2rcode + max_lhs_adr;
	atom2rsize = synchro2vstr + max_lhs_adr;

	lhs_pos2adr = (SXINT*) sxalloc (max_garity+1, sizeof (SXINT));
	lhs_pos2ladr = (SXINT*) sxalloc (max_garity+1, sizeof (SXINT));
	lhs_pos2radr = (SXINT*) sxalloc (max_garity+1, sizeof (SXINT));
	lhs_arg_pos2prev_args_t_nb = (SXINT*) sxalloc (max_garity+1, sizeof (SXINT));
	lhs_arg_tbp_set = sxba_calloc (max_garity+1);
	useless_lhs_arg = sxba_calloc (max_garity+1);

	for_loops_nb = (SXINT*) sxcalloc (max_garity+1, sizeof (SXINT));

	rhs_pos2static_size_kind = (char*) sxalloc (max_clause_arg_nb+1, sizeof (char));
	rhs_pos2Ak = (SXINT*) sxalloc (max_clause_arg_nb+1, sizeof (SXINT));
	rhs_pos2nt = (SXINT*) sxalloc (max_clause_arg_nb+1, sizeof (SXINT));
	rhs_pos2param = (SXINT*) sxalloc (max_clause_arg_nb+1, sizeof (SXINT));
	rhs_pos2lmadr = (SXINT*) sxalloc (max_clause_arg_nb+1, sizeof (SXINT));
	rhs_pos2rmadr = (SXINT*) sxalloc (max_clause_arg_nb+1, sizeof (SXINT));
	rhs_pos2rhs_mpos = (SXINT*) sxalloc (max_clause_arg_nb+1, sizeof (SXINT));
	rhs_pos2neg_call_set = sxba_calloc (max_clause_arg_nb+1);
	rhs_pos2nc_max_set = sxba_calloc (max_clause_arg_nb+1);

	if (first) {
	  check_nul_range = sxba_calloc (max_clause_arg_nb+1);
	  check_nul_range_done = sxba_calloc (max_clause_arg_nb+1);
	  check_olb = sxba_calloc (max_clause_arg_nb+1);
	  check_oub = sxba_calloc (max_clause_arg_nb+1);
	  check_non_nul_range = sxba_calloc (max_clause_arg_nb+1);
	  check_non_nul_range_done = sxba_calloc (max_clause_arg_nb+1);
	}

	pos_is_a_range  = sxba_calloc (max_clause_arg_nb+1);
	ij_already_set = sxba_calloc (max_clause_arg_nb+1);
	olbs = (SXINT*) sxalloc (max_clause_arg_nb+1, sizeof (SXINT));
	oubs = (SXINT*) sxalloc (max_clause_arg_nb+1, sizeof (SXINT));

	D = max_arg_size+1;		/* compte large */
	adr2cur = (SXINT*) sxalloc (max_lhs_adr+1, sizeof (SXINT));
	adr2lA = (SXINT*) sxcalloc (max_lhs_adr+1, sizeof (SXINT));
	adr2uA = (SXINT*) sxcalloc (max_lhs_adr+1, sizeof (SXINT));
	adr2pos = (SXINT*) sxalloc (max_lhs_adr+1, sizeof (SXINT));
	adr_stack = (SXINT*) sxalloc (max_lhs_adr+1, sizeof (SXINT)), adr_stack [0] = 0;
	adr2madr = (SXINT*) sxcalloc (max_lhs_adr+1, sizeof (SXINT));
	synchro2adr = (SXINT*) sxcalloc (max_lhs_adr+1, sizeof (SXINT));
	madr2olb = (SXINT*) sxcalloc (max_lhs_adr+1, sizeof (SXINT));
	madr2oub = (SXINT*) sxcalloc (max_lhs_adr+1, sizeof (SXINT));
	lmadr_set = sxba_calloc (max_lhs_adr+1);
	rmadr_set = sxba_calloc (max_lhs_adr+1);
	tmadr_set = sxba_calloc (max_lhs_adr+1);
	madr_set = sxba_calloc (max_lhs_adr+1);
	synchro_set = sxba_calloc (max_lhs_adr+1);
	is_madr_an_ob = sxba_calloc (max_lhs_adr+1);
	assigned_madr_set = sxba_calloc (max_lhs_adr+1);
	already_seen_madr_set = sxba_calloc (max_lhs_adr+1);
	nyp_synchro_test = sxba_calloc (max_lhs_adr+1);
	adr2equiv_adr_set = sxbm_calloc (max_lhs_adr+1, max_lhs_adr+1);
	wmadr_sets = sxbm_calloc ((wtop = max_garity)+1, max_lhs_adr+1);
	adr2tpos = (SXINT*) sxalloc (max_lhs_adr+1, sizeof (SXINT));

	atom2min = (SXINT*) sxcalloc (max_gvar+1, sizeof (SXINT));
	atom2max = (SXINT*) sxalloc (max_gvar+1, sizeof (SXINT));
	for (x = 0; x <= max_gvar; x++)
	  atom2max [x] = -1; /* inconnu */

	gatom2nb = (SXINT*) sxcalloc (max_gvar+1, sizeof (SXINT));
	atom2var_nb = (SXINT*) sxalloc (max_gvar+1, sizeof (SXINT));
	atom2static_size = (SXINT*) sxalloc (max_gvar+1, sizeof (SXINT));
	atom_stack = (SXINT*) sxalloc (max_gvar+1, sizeof (SXINT)), atom_stack [0] = 0;
	Asize_set = sxba_calloc (max_gvar+1);
	XxY_alloc (&VxV, "VxV", 2*(max_gvar) + 1, 1, 1, 1, NULL, NULL);

	recursive_clause_set = sxba_calloc (last_clause+1);
	lexicalized_clause_set = sxba_calloc (last_clause+1);
	cyclic_clause_set = sxba_calloc (last_clause+1);
	cyclic_nt_set = sxba_calloc (max_nt+1);
	clause_has_ext_decl = sxba_calloc (last_clause+1);

	var2atom = (SXINT*) sxcalloc (max_gvar+1, sizeof (SXINT));
	var2nb = (SXINT*) sxcalloc (max_gvar+1, sizeof (SXINT));
	var2lhs_pos = (SXINT*) sxcalloc (max_gvar+1, sizeof (SXINT));

	XH_alloc (&XH_prdct_args, "XH_prdct_args", max_rhs_prdct + 1, 1, max_garity, XH_prdct_args_oflw, NULL);
	XH_prdct_args2mprdct_pos = (SXINT*) sxalloc (XH_size (XH_prdct_args)+1, sizeof (SXINT));

	prdct_pos2mprdct_pos = (SXINT*) sxalloc (max_rhs_prdct+2, sizeof (SXINT));
	mprdct_pos2generated = (SXINT*) sxalloc (max_rhs_prdct+2, sizeof (SXINT));

	for (x = max_rhs_prdct+1; x >= 0; x--) {
	  mprdct_pos2generated [x] = -1;
	}
    
	rhs_prdct_pos2tooth = (SXINT*) sxalloc (max_rhs_prdct+2, sizeof (SXINT));
	rhs_prdct_pos2generator_pos = (SXINT*) sxalloc (max_rhs_prdct+2, sizeof (SXINT));
	rhs_prdct_nyp = sxba_calloc (max_rhs_prdct+2);

	component2pos = (SXINT*) sxalloc (max_gcomponent_nb+1, sizeof (SXINT));
	component2bot = (SXINT*) sxalloc (max_gcomponent_nb+1, sizeof (SXINT));
	component2top = (SXINT*) sxalloc (max_gcomponent_nb+1, sizeof (SXINT));
	component2lmadr = (SXINT*) sxalloc (max_gcomponent_nb+1, sizeof (SXINT));
	component2rmadr = (SXINT*) sxalloc (max_gcomponent_nb+1, sizeof (SXINT));

	eq_top = max_clause_arg_nb;
	equation = (SXINT**) sxalloc (eq_top+1, sizeof (SXINT*));
	ptr = eq_list = (SXINT*) sxcalloc ((eq_top+1)*(max_gvar+1)+1, sizeof (SXINT));
	equation2Xnb = (SXINT*) sxalloc (eq_top+1, sizeof (SXINT));
	equation_stack = (SXINT*) sxalloc (eq_top+1, sizeof (SXINT)), equation_stack [0] = 0;
	equation_stack2 = (SXINT*) sxalloc (eq_top+1, sizeof (SXINT)), equation_stack2 [0] = 0;

	equation_already_substituted_set = sxba_calloc (eq_top+1);
	nyp_eq_set = sxba_calloc (eq_top+1);
	equation2vstr_kind = (SXINT*) sxalloc (eq_top+1, sizeof (SXINT));

	pos2counter_val = (SXINT*) sxalloc (max_clause_arg_nb+1, sizeof (SXINT));
	counter2pos = (SXINT*) sxalloc (max_clause_arg_nb+1, sizeof (SXINT));

	for (pos = 0; pos <= eq_top; pos++) {
	  equation [pos] = ptr;
	  ptr += max_gvar+1;
	}

	equation2vstr = (VARSTR*) sxalloc (eq_top+1, sizeof (VARSTR));

	for (pos = 0; pos <= eq_top; pos++) {
	  equation2vstr [pos] = varstr_alloc (32);
	}

	arity_set = sxba_calloc (max_garity+1);

	for (nt = 1; nt <= max_nt; nt++) {
	  if (SXBA_bit_is_set (is_lhs_nt, nt)) {
	    arity = nt2arity [nt];
	    SXBA_1_bit (arity_set, arity);
	  }
	}

	if (first) {
	  is_first_complete = sxba_calloc (max_Ak+1);
	  is_first_vide = sxba_calloc (max_Ak+1);
	  is_last_complete = sxba_calloc (max_Ak+1);
	  is_last_vide = sxba_calloc (max_Ak+1);

	  for (Ak = 1; Ak <= max_Ak; Ak++) {
	    x = sxba_cardinal (first [Ak])-((SXBA_bit_is_set (first [Ak], 0)) ? 1 : 0);

	    if (x == 0)
	      SXBA_1_bit (is_first_vide, Ak);

	    if (x == max_t)
	      SXBA_1_bit (is_first_complete, Ak);

	    x = sxba_cardinal (last [Ak])-((SXBA_bit_is_set (last [Ak], 0)) ? 1 : 0);

	    if (x == 0)
	      SXBA_1_bit (is_last_vide, Ak);

	    if (x == max_t)
	      SXBA_1_bit (is_last_complete, Ak);
	  }
	}


	clause_set = sxba_calloc (last_clause+1);

	XxY_foreach (rcg_clauses, clause)
	  SXBA_1_bit (clause_set, clause);

	non_false_A_clause_set = sxba_calloc (last_clause+1);
	recursive_nt_set = sxba_calloc (max_nt+1);

	rec_clauses ();

	if (is_1_RCG) {
	  SXINT 	i, j;
	  SXINT	*p;

	  lhs_arg2pos = (SXINT*) sxalloc (max_garity+2, sizeof (SXINT));

	  size = 1+max_lhs_adr;
	  pos2symb = (SXINT*) sxalloc (size, sizeof (SXINT));
	  pos2main = (SXINT*) sxcalloc (size, sizeof (SXINT));
	  pos2lhs_arg = (SXINT*) sxalloc (size, sizeof (SXINT));
	  pos2bound = (SXINT*) sxalloc (size, sizeof (SXINT));
	  pos2pos_set = sxbm_calloc (size, size);
	  pos_hd_set = sxba_calloc (size);
	  posxpos2lgth = (SXINT**) sxalloc (size, sizeof (SXINT*));
	  p = (SXINT*) sxalloc (size*size, sizeof (SXINT));

	  for (i = 1; i < size; i++) {
	    posxpos2lgth [i] = p;

	    for (j = i+1, p += j; j < size; j++)
	      *p++ = -1;
	  }

	  S1_alloc (posxpos_stack, size);

	  var_set = sxba_calloc (max_gvar+1);
	  k_guide_set = sxba_calloc (max_garity+1);
	}

    
	clause2clause_id = (SXINT*) sxcalloc (last_clause+1, sizeof (SXINT));

	A2max_clause_id = (SXINT*) sxcalloc (max_nt+1, sizeof (SXINT));

	alloc_gen_dag_parser ();

	gen_parser ();

	if (t2Cid_ste) sxfree (t2Cid_ste), t2Cid_ste = NULL;

	free_gen_dag_parser ();

#if 0
	if (clause2nl_stmt_stack) sxfree (clause2nl_stmt_stack), clause2nl_stmt_stack = NULL;
	if (clause2symb_id_stack) sxfree (clause2symb_id_stack), clause2symb_id_stack = NULL;
#endif /* 0 */
	if (clause2rhs_pos_disp) sxfree (clause2rhs_pos_disp), clause2rhs_pos_disp = NULL;

	sxfree (A2max_clause_id), A2max_clause_id = NULL;

	sxfree (recursive_nt_set), recursive_nt_set = NULL;

	sxfree (clause2clause_id), clause2clause_id = NULL;

	if (is_1_RCG) {
	  sxfree (lhs_arg2pos), lhs_arg2pos = NULL;
	  sxfree (pos2symb), pos2symb = NULL;
	  sxfree (pos2main), pos2main = NULL;
	  sxfree (pos2lhs_arg), pos2lhs_arg = NULL;
	  sxfree (pos2bound), pos2bound = NULL;
	  sxbm_free (pos2pos_set), pos2pos_set = NULL;
	  sxfree (pos_hd_set), pos_hd_set = NULL;
	  sxfree (posxpos2lgth [1]), sxfree (posxpos2lgth), posxpos2lgth = NULL;
	  S1_free (posxpos_stack), posxpos_stack = NULL;

	  sxfree (var_set), var_set = NULL;
	  sxfree (k_guide_set), k_guide_set = NULL;
	}

	sxfree (clause_set), clause_set = NULL;

	varstr_free (wvstr), wvstr = NULL;
	varstr_free (vstr_cond), vstr_cond = NULL;
	varstr_free (vstr_body), vstr_body = NULL;
	varstr_free (t_indexes_vstr), t_indexes_vstr = NULL;
	
	for (x = eq_top; x >= 0; x--) {
	  varstr_free (equation2vstr [x]), equation2vstr [x] = NULL;
	}

	sxfree (equation2vstr), equation2vstr = NULL;

	for (x = varstrings_top; x >= 0; x--) {
	  varstr_free (varstrings [x]);
	}

	sxfree (varstrings), varstrings = NULL;
	
	lhs_pos2lvstr = NULL;
	lhs_pos2rvstr = NULL;
	madr2lcode = NULL;
	madr2rcode = NULL;
	synchro2vstr = NULL;
	atom2rsize = NULL;

	sxfree (lhs_pos2adr), lhs_pos2adr = NULL;
	sxfree (lhs_pos2ladr), lhs_pos2ladr = NULL;
	sxfree (lhs_pos2radr), lhs_pos2radr = NULL;
	sxfree (lhs_arg_pos2prev_args_t_nb), lhs_arg_pos2prev_args_t_nb = NULL;
	sxfree (equation_already_substituted_set), equation_already_substituted_set = NULL;
	sxfree (nyp_eq_set), nyp_eq_set = NULL;
	sxfree (equation2vstr_kind), equation2vstr_kind = NULL;
	sxfree (lhs_arg_tbp_set), lhs_arg_tbp_set = NULL;
	sxfree (useless_lhs_arg), useless_lhs_arg = NULL;

	sxfree (for_loops_nb), for_loops_nb = NULL;

	sxfree (rhs_pos2static_size_kind), rhs_pos2static_size_kind = NULL;
	sxfree (rhs_pos2Ak), rhs_pos2Ak = NULL;
	sxfree (rhs_pos2nt), rhs_pos2nt = NULL;
	sxfree (rhs_pos2param), rhs_pos2param = NULL;
	sxfree (rhs_pos2lmadr), rhs_pos2lmadr = NULL;
	sxfree (rhs_pos2rmadr), rhs_pos2rmadr = NULL;
	sxfree (rhs_pos2rhs_mpos), rhs_pos2rhs_mpos = NULL;
	sxfree (rhs_pos2neg_call_set), rhs_pos2neg_call_set = NULL;
	sxfree (rhs_pos2nc_max_set), rhs_pos2nc_max_set = NULL;

	if (first) {
	  sxfree (check_nul_range), check_nul_range = NULL;
	  sxfree (check_nul_range_done), check_nul_range_done = NULL;
	  sxfree (check_olb), check_olb = NULL;
	  sxfree (check_oub), check_oub = NULL;
	  sxfree (check_non_nul_range), check_non_nul_range = NULL;
	  sxfree (check_non_nul_range_done), check_non_nul_range_done = NULL;
	}

	sxfree (pos_is_a_range), pos_is_a_range = NULL;
	sxfree (ij_already_set), ij_already_set = NULL;
	sxfree (adr2cur), adr2cur = NULL;
	sxfree (adr2lA), adr2lA = NULL;
	sxfree (adr2uA), adr2uA = NULL;
	sxfree (adr2pos), adr2pos = NULL;
	sxfree (adr_stack), adr_stack = NULL;
	sxfree (adr2madr), adr2madr = NULL;
	sxfree (synchro2adr), synchro2adr = NULL;
	sxfree (madr2olb), madr2olb = NULL;
	sxfree (madr2oub), madr2oub = NULL;
	sxfree (madr_set), madr_set = NULL;
	sxfree (lmadr_set), lmadr_set = NULL;
	sxfree (rmadr_set), rmadr_set = NULL;
	sxfree (tmadr_set), tmadr_set = NULL;
	sxfree (synchro_set), synchro_set = NULL;
	sxfree (is_madr_an_ob), is_madr_an_ob = NULL;
	sxfree (assigned_madr_set), assigned_madr_set = NULL;
	sxfree (already_seen_madr_set), already_seen_madr_set = NULL;
	sxfree (nyp_synchro_test), nyp_synchro_test = NULL;
	sxbm_free (adr2equiv_adr_set), adr2equiv_adr_set = NULL;
	sxbm_free (wmadr_sets), wmadr_sets = NULL;
	sxfree (adr2tpos), adr2tpos = NULL;
	sxfree (olbs), olbs = NULL;
	sxfree (oubs), oubs = NULL;

	sxfree (atom2min), atom2min = NULL;
	sxfree (atom2max), atom2max = NULL;
	sxfree (gatom2nb), gatom2nb = NULL;
	sxfree (atom2var_nb), atom2var_nb = NULL;
	sxfree (atom2static_size), atom2static_size = NULL;
	sxfree (atom_stack), atom_stack = NULL;
	sxfree (Asize_set), Asize_set = NULL;
	XxY_free (&VxV);

	sxfree (recursive_clause_set), recursive_clause_set = NULL;
	sxfree (lexicalized_clause_set), lexicalized_clause_set = NULL;
	sxfree (cyclic_clause_set), cyclic_clause_set = NULL;
	sxfree (cyclic_nt_set), cyclic_nt_set = NULL;
	sxfree (clause_has_ext_decl), clause_has_ext_decl = NULL; 

	sxfree (var2atom), var2atom = NULL;
	sxfree (var2nb), var2nb = NULL;
	sxfree (var2lhs_pos), var2lhs_pos = NULL;

	XH_free (&XH_prdct_args);
	sxfree (XH_prdct_args2mprdct_pos), XH_prdct_args2mprdct_pos = NULL;

	sxfree (prdct_pos2mprdct_pos), prdct_pos2mprdct_pos = NULL;
	sxfree (mprdct_pos2generated), mprdct_pos2generated = NULL;
	sxfree (rhs_prdct_pos2tooth), rhs_prdct_pos2tooth = NULL;
	sxfree (rhs_prdct_pos2generator_pos), rhs_prdct_pos2generator_pos = NULL;
	sxfree (rhs_prdct_nyp), rhs_prdct_nyp = NULL;

	sxfree (component2pos), component2pos = NULL;
	sxfree (component2bot), component2bot = NULL;
	sxfree (component2top), component2top = NULL;
	sxfree (component2lmadr), component2lmadr = NULL;
	sxfree (component2rmadr), component2rmadr = NULL;

	sxfree (equation), equation = NULL;
	sxfree (eq_list), eq_list = NULL;
	sxfree (equation2Xnb), equation2Xnb = NULL;
	sxfree (equation_stack), equation_stack = NULL;
	sxfree (equation_stack2), equation_stack2 = NULL;

	sxfree (arity_set);

	if (first) {
	  sxfree (is_first_complete), is_first_complete = NULL;
	  sxfree (is_first_vide), is_first_vide = NULL;
	  sxfree (is_last_complete), is_last_complete = NULL;
	  sxfree (is_last_vide), is_last_vide = NULL;
	}

	sxfree (Indexes), Indexes = NULL;
	sxfree (clause2lhst_disp), clause2lhst_disp = NULL;

	sxfree (non_false_A_clause_set),  non_false_A_clause_set = NULL;

	sxfree (pos2counter_val), pos2counter_val = NULL;
	sxfree (counter2pos), counter2pos = NULL;

	if (sxverbosep) {
	  fprintf (sxtty, "(arity=%ld, clause_nb=%ld, degree=%ld for clause #%ld, size=%ld) done\n",
		   (SXINT) max_garity, (SXINT) last_clause, (SXINT) max_degree, (SXINT) max_degree_clause, (SXINT) gsize); /* Temps d'analyse polynomial, on imprime le degree maximal */
	  /* Si max_degree == 0 => temps d'analyse constant! (le langage est fini ?) */
	  sxttycol1p = true;
	}
      }

  sxfree (ext_decl_set), ext_decl_set = NULL;
  sxfree (ext_decl_clause_set), ext_decl_clause_set = NULL;
  sxfree (ext_decl2clause), ext_decl2clause = NULL;
}

#if 0
/* ATTENTION : on ne sait pas detecter si une grammaire est cyclique
   definition G est cyclique ssi \exists A, w et \rho \in R_w^{arity(A)} t.q.
   A(\rho) =>+ A(\rho)
   1) On analyse pour un w donne
   2) Quand on atteint un appel de A(\rho), cet appel est accessible depuis l'ip initial (i.e.,
      on n'a pas tous les \rho de R_w^{arity(A)})
   3) On peut etre ds un cas A(\rho) =>+ A(\rho) B(\rho'),
      si A(\rho) est faux, on n'executera pas B(\rho'), on ne saura donc pas s'il derive ds le vide
*/

/* Il serait possible de changer la definition d'une grammaire cyclique par \exists A(\rho) ds une
   derivation issue de A(\rho)
   Ds ce cas la detection d'un cycle (sur A(\rho)) au cours d'une analyse => grammaire cyclique
   Inversement, la non detection d'un cycle (sur \forall A(\rho)), ne dit rien sur la grammaire
   et ne dit meme rien sur la non existance de cycle sur A(\rho) : A(\rho) a pu ne jamais etre appele
   bien qu'il figure ds une derivation
*/

/* Un predicat non-cyclique peut appeler des predicats cycliques et etre appeles par des cycliques.  Donc,
   si la grammaire est cyclique, il ne faut faire la generation de ce predicat (et le marquer comme calcule)
   qu'au niveau 1 */


static bool is_glbl_cycle; /* memorise l'etat courant (vis-a-vis des cycles) du ip A\rho courant */
/* Vrai ssi la pile des ip appelants contient 2 occur de B\rho */
/* Le moment de la generation et le positionnement de memoN0 dependent de is_glbl_cycle */

#if is_cyclic
if (SXBA_bit_is_set (memoN0, Aij)) {
  /* A\rho est calcule et completement genere (eventuellement)*/
  return SXBA_bit_is_set (memoN1, Aij) ? 1 : -1;
}

if (SXBA_bit_is_set (memoC [1], Aij)) {
  /* Detection d'un cycle sur A\rho (niveau 3)
     Tout [nouvel] appel de A\rho retournera FAUX (il y a 2 appels de A\rho en pile) */
  return -1;
}

is_cyclic_ip = SXBA_bit_is_set (memoC [0], Aij);

#if 0
if (is_glbl_cycle || is_cyclic_ip) {
  if (is_glbl_cycle) {
    /* On est ds un cycle, A\rho n'est pas l'ip principal */
    if (is_cyclic_ip) {
      /* Detection d'un cycle sur A\rho */
      execute = SXBA_bit_is_reset_set (memoC [3], Aij);
      /* On execute si c'est la 1ere fois */
    }
    else {
      /* Pas (encore) de cycle sur A\rho */
      execute = SXBA_bit_is_reset_set (memoC [2], Aij);
      /* On execute si c'est la 1ere fois */
    }
  }
  else {
    /* A\rho est l'ip principal, detection d'un cycle */
    execute = SXBA_bit_is_reset_set (memoC [3], Aij);
    /* On execute si c'est la 1ere fois */
  }
}
else
/* pas de cycle detecte, on execute */
execute = true;

if (!(execute)) return SXBA_bit_is_set (memoN1, Aij) ? 1 : -1;

/* On "condense tout ca en */
#endif /* 0 */

if (!((is_glbl_cycle || is_cyclic_ip) ? ((is_glbl_cycle) ? ((is_cyclic_ip) ? (SXBA_bit_is_reset_set (memoC [3], Aij)) : (SXBA_bit_is_reset_set (memoC [2], Aij))) : (SXBA_bit_is_reset_set (memoC [3], Aij))) : true)) return SXBA_bit_is_set (memoN1, Aij) ? 1 : -1;

if (is_cyclic_ip) SXBA_1_bit (memoC [1], Aij); else SXBA_1_bit (memoC [0], Aij);
is_local_cycle = is_glbl_cycle;
is_glbl_cycle |= is_cyclic_ip;
ret_val = -1;

for (/* each A_clause */) {
  switch (A_clause) {
    /* ... */

  case A_clause:
    clause_id = ...;

    do {
      /* ... */
      /* Cette instanciation de la A-clause est vraie */
      /* donc A\rho est vrai, mais pas forcement genere */
      ret_val = 1; 

      if (!is_glbl_cycle) {
	/* On ne genere qu'au niveau 1 */
	/* SXBA_bit_is_set (memoP [Aij], %ld) == false */
        (*G.semact.first_pass) ();
      }
      /* ... */
    } while (0);
    /* } */


    break;

    /* ... */
  }
}

if (ret_val > 0) {
  SXBA_1_bit (memoP [Aij], clause_id);
  /* Ds tous les cas on stocke la valeur vraie : prudence */
  SXBA_1_bit (memoN1, Aij);
  /* Si A_is_loop */
  if (!is_glbl_cycle) (*G.semact.first_pass_loop)(&G, rho, LOOP);
}

if (!is_glbl_cycle) {
  SXBA_1_bit (memoN0, Aij); /* A\rho est calcule et completement genere (eventuellement)*/
  /* Detection de cycles */
  /* On peut ne pas detecter de cycle alors qu'il y en a Ex :
     1er cycle S => A => S detecte et resolu (S est vrai), on est donc revenu au niveau 1 de S.  La,
     on a un appel de B qui appelle S.  S retourne vrai et le cycle sur B n'est pas note' */
  if (ret_val > 0 && (is_check_cycle || EBUG3) && SXBA_bit_is_set (memoC [3], Aij)) (*G.semact.first_pass_loop)(&G, rho, CYCLE);
}

#if 0
if (!is_local_cycle || is_cyclic_ip)
     /* Le ip "responsable" du cycle doit etre marque' comme calcule' */
     /* C'est l'appelant en pile qui fera les generations mais lui ne doit
	pas etre rappele' */
     /* On marque les ip de niveau 2 comme calcule's (vrai ou faux).  C,a evite de les rappeler au niveau 2.
	La valeur fausse pouvant etre modifie'e par l'appelant de niveau 1.
	En revanche, si on est au niveau 1 ds un cycle, il ne faut pas marquer l'ip comme calcule'.
	Ce sera le boulot de l'appel (non encore effectue') de cet ip en sortie de cycle. */
     SXBA_1_bit (memoN0, Aij);

     /* Au niveau 1, si ds un cycle, la generation n'a pas ete faite, on repasse donc l'ip a non calcule' */
     else if (is_glbl_cycle && !is_cyclic_ip)
     SXBA_0_bit (memoN0, Aij);
#endif

is_glbl_cycle = is_local_cycle;

if (is_cyclic_ip) SXBA_0_bit (memoC [1], Aij);
else SXBA_0_bit (memoC [0], Aij);

return ret_val;

#else /* !is_cyclic */
if (!SXBA_bit_is_reset_set (memoN0, Aij)) {
    /* A\rho est calcule */
    /* On ne boucle pas sur les grammaires cycliques */
    return (SXBA_bit_is_set (memoN1, Aij)) ? 1 : -1;
}

ret_val = -1;

for (/* each A_clause */) {
  local_ret_val = -1;

  switch (A_clause) {
    /* ... */

  case A_clause:
    clause_id = ...;

    do {
      /* ... */
      if ((*G.semact.first_pass) ())
	ret_val = local_ret_val = 1;
      /* ... */
    } while (0);

    break;

    /* ... */
  }

  if (local_ret_val > 0)
    SXBA_1_bit (memoP [Aij], clause_id);
}

if (ret_val > 0)
     SXBA_1_bit (memoN1, Aij); /* ... vrai */

     return ret_val;
#endif /* !is_cyclic */

     /* 2eme passe */
#if EBUG
  instantiated_prdct2struct (rho0);
  call_level++;
  printf ("(%s)in(%ld): Q (%ld..%ld)\n", G.name, call_level, lower_bounds[0], upper_bounds[0]);
#endif
     
     /* De`s qu'une ic est vraie, on la sort (si elle est vraie, c'est que les ip de sa RHS ont
	ete (partiellement) calcule's).  */

     /* Attention, une ip qui a retourne vrai peut ne pas etre completement generee.  L'ic
	appelante ne peut donc pas etre supprimee.  Si c'est le seul appel de l'ip, il faut
	laisser l'ic pour terminer la generation +tard.
     */

     /* Quelque soit le niveau de cycle, on ne genere une ic que si elle est non marquee
	Dans tous les cas on supprime une ic de niveau 1 (marquee ou non) qui a marche
	Si on genere une ic (non marquee) au niveau 2, on la marque (sans la supprimer, elle
	le sera au niveau 1, sans etre regeneree).
	Si on ne supprime pas une ic, on la saute.
     */

     /* Attention, a la fin d'un niveau 1, la liste peut ne pas etre vide : cas de C ds cyclic3
	A(X) --> B(X) .
	A(X) --> C(X) .
	B(X) --> A(X) .
	C(X) --> .
	C(X) --> A(X) .
     */

  if (!(ret_val = SXBA_bit_is_set (memoN1, rho0))
      && (SXBA_bit_is_set (memoC [1], rho0) ? (ret_val = SXBA_bit_is_set (memoN0, rho0), false) : true)
      /* Pas de cycle sur A\rho */
      ) {
    
    is_cyclic_ip = SXBA_bit_is_reset_set (memoC [0], rho0) ? false : (SXBA_1_bit (memoC [1], rho0), true);

    if (*(hd_icp = prev_icp = &GET_SEM_NODE_PTR (G.pid, rho0))) {
      /* Si on est au niveau 2, l'appelant au niveau 1 est ds l'appel de *prev_icp */
      do {
	do {
	  p = &((*prev_icp)->rho [0]);
	  clause = *p;
	  local_ret_val = false;
	  switch (clause2clause_id [clause]) {

	    /* 3: Q(X0) --> R(X0) . */
	  case 1:
	    if (_walk_R (p [2]) == -1) continue;
	    clause_id = 0;
	    break;

	    /* 4: Q(\a) -->  . */
	  case 2:
	    clause_id = 1;
	    break;
#if EBUG4
	  default:
	    sxtrap (G.name, "_walk_Q");
#endif
	  }

	  /* La generation de la 1ere ic (ou des suivantes) declenche la generation de toutes
	     les cycliques, il faut donc le noter */
	  SXBA_1_bit (memoN0, rho0);
	  local_ret_val = ret_val = true;

	  /* IC est vraie, on l'efface et on la sort */
	  /* ATTENTION : si on est au niveau 1, cette ic a deja pu etre sortie au niveau 2, il ne faut
	     pas recommencer */
	  if (p [1]) {
	    /* ic non marquee */
	    if ((*G.semact.second_pass)(&G, p) != -1)
	      SXBA_1_bit (memoP [rho0], clause_id);

	    if (is_cyclic_ip) p [1] = 0; /* ic marquee */
	  }

	  if (!is_cyclic_ip) *prev_icp = (*prev_icp)->next;
	  
	} while (false); /* Pour les continue ... */

	if (!local_ret_val || is_cyclic_ip) prev_icp = &((*prev_icp)->next);
	  /* Elle sera evaluee + tard, elle reste en liste, on va chercher la suivante */

      } while (*prev_icp);
    }

    /* ATTENTION, un appel de niveau qqc peut rendre faux Ex :
       l'appel de F ds la sequence S -> S -> F -> [F ->] S au cours de la detection d'un
       cycle sur S.
       Il faut pouvoir recommencer cet appel +tard.
       Un ip est fini SSI sa liste est vide (meme si on est au niveau 2, => plus d'appel au niveau 2 et
       l'appelant de niveau 1 ne fera rien)
    */

    if (is_cyclic_ip) SXBA_0_bit (memoC [1], rho0);
    else SXBA_0_bit (memoC [0], rho0);

    if (*hd_icp == NULL) {
      (*G.semact.second_pass_loop)(&G, p, LOOP);
      SXBA_1_bit (memoN1, rho0);
    }
  }
  /* else ici, ret_val peut etre vrai ou faux */

#if EBUG
    instantiated_prdct2struct (rho0);
    printf ("(%s)out(%ld): Q (%ld..%ld) %s\n", G.name, call_level, lower_bounds[0], upper_bounds[0], ret_val ? "TRUE" : "FALSE");
    call_level--;
#endif

  return ret_val ? 1 : -1;

#endif /* 0 */
