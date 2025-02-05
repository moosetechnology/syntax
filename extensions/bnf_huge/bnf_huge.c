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
#include "varstr.h" /* doit etre inclus avant "sxunix.h" pour avoir sxba2c() */
#include "sxunix.h"
#include "B_tables.h"
#include "fsa.h"

#if 0
#include "sxword.h"
#endif /* 0 */

#include "sxstack.h"

char WHAT_BNFHUGE[] = "@(#)SYNTAX - $Id: bnf_huge.c 3631 2023-12-20 17:16:41Z garavel $" WHAT_DEBUG;

static void
output_header (time_t bnf_modif_time, char *langname)
{
  time_t cur_time;

  cur_time = time (0);  /* now */

  printf ("\
/* ***************************************************************************************\n\
\tThis (partial) C tables has been extracted on %s\
\tby the SYNTAX(*) BNF processor running with the \"-huge\" option\n\
\tfrom the huge CFG \"%s\" in BNF form last modified on %s\
******************************************************************************************\n\
\t(*) SYNTAX is a trademark of INRIA.\n\
****************************************************************************************** */\n\n\n",
	  ctime (&cur_time),
	  (langname == NULL) ? "stdin" : langname,
	  ctime (&bnf_modif_time));

  printf ("\n#define LANGUAGE_NAME \"%s\"\n", (langname == NULL) ? "stdin" : langname);

  fputs ("\n#define HUGE_CFG\n", stdout);

  printf ("\n#define bnf_modif_time\t%ld\n\n", bnf_modif_time);
}

static SXINT  *gen_t_list, gen_t_list_top;

static SXBA *nt2gen_t_set;
static SXINT  *nt2gen_t_list;

/* nt2gen_t_set [A] = {t | A =>+ x1 t x2} */
static void
fill_nt2gen_t_set (struct bnf_ag_item *B)
{
  SXINT  prod, item, A, X, bot, top, card, t;
  SXINT  *nt_stack;
  SXBA nt_set, gen_t_set, is_an_int_list;

  nt_set = sxba_calloc (B->WS_TBL_SIZE.ntmax+1);
  nt_stack = (SXINT*) sxalloc (B->WS_TBL_SIZE.ntmax+1, sizeof (SXINT)), nt_stack [0] = 0;
  is_an_int_list = sxba_calloc (B->WS_TBL_SIZE.ntmax+1);

  for (prod = 1; prod <= B->WS_TBL_SIZE.nbpro; prod++) {
    item = B->WS_NBPRO [prod].prolon;
    A = B->WS_NBPRO [prod].reduc;
    gen_t_set = nt2gen_t_set [A];

    if (gen_t_set && B->BVIDE != NULL && SXBA_bit_is_set (B->BVIDE, A)) {
      /* A =>+ \varepsilon */
      gen_t_set = nt2gen_t_set [A] = NULL; /* On ne calcule pas le gen_t_set des nt derivant ds le vide */
    }

    if (gen_t_set) {
      while ((X = B->WS_INDPRO [item++].lispro) != 0) {
	if (X < 0) {
	  X = -X;
	  SXBA_1_bit (gen_t_set, X);

	  if (SXBA_bit_is_reset_set (nt_set, A))
	    PUSH (nt_stack, A);
	}
      }
    }
  }

  while (!IS_EMPTY (nt_stack)) {
    A = POP (nt_stack);
    SXBA_0_bit (nt_set, A);

    if (A > 0) {
      bot = B->WS_NTMAX [A].npd;
      top = B->WS_NTMAX [A+1].npd;
      gen_t_set = nt2gen_t_set [A];

      while (bot < top) {
	item = B->NUMPD [bot];
	A = B->WS_NBPRO [B->WS_INDPRO [item].prolis].reduc; /* On est su^r que prod n'est pas la super-re`gle */

	if (nt2gen_t_set [A] && sxba_or_with_test (nt2gen_t_set [A], gen_t_set)) {
	  /* B a change' ... */
	  if (SXBA_bit_is_reset_set (nt_set, A)) {
	    /* ... et n'est pas/plus ds la pile */
	    PUSH (nt_stack, A);
	  }
	}

	bot++;
      }
    }
  }

  sxfree (nt_set);
  sxfree (nt_stack);

  gen_t_list_top = 0;

  for (A = 1; A <= B->WS_TBL_SIZE.ntmax; A++) {
    if ((gen_t_set = nt2gen_t_set [A])) {
      card = sxba_cardinal (gen_t_set);

      if (card < (-B->WS_TBL_SIZE.tmax)>>SXSHIFT_AMOUNT) {
	/* ... forme SXINT * */
	gen_t_list_top += card+1;
	SXBA_1_bit (is_an_int_list, A);
      }
    }
  }

  if (gen_t_list_top) {
    gen_t_list = (SXINT*) sxalloc (gen_t_list_top+1, sizeof (SXINT)), gen_t_list [0] = 0;
    gen_t_list_top = 1;

    A = 0;

    while ((A = sxba_scan (is_an_int_list, A)) > 0) {
      nt2gen_t_list [A] = gen_t_list_top;
      gen_t_set = nt2gen_t_set [A];

      gen_t_list [gen_t_list_top++] = sxba_cardinal (gen_t_set); /* marqueur de fin */

      t = 0;

      while ((t = sxba_scan (gen_t_set, t)) > 0) {
	gen_t_list [gen_t_list_top++] = t;
      }

      nt2gen_t_set [A] = NULL;
    }
  }

  sxfree (is_an_int_list);
}

extern VARSTR  cat_t_string (VARSTR, SXINT);


#include "XH.h"

static SXINT *segment_hd2nb;

static void
segment_hd_oflw (SXINT old_size, SXINT new_size)
{
  sxuse(old_size); /* pour faire taire gcc -Wunused */
  segment_hd2nb = (SXINT*) sxrealloc (segment_hd2nb, new_size+1, sizeof (SXINT));
}


static void
out_ifdef (char *name)
{
  printf ("\n\n#ifdef def_%s\n", name);
}

static void

out_endif (char *name)
{
  printf ("#endif /* def_%s */\n", name);
}

void
bnf_huge (struct bnf_ag_item *B, char *langname)
{
  SXINT    i, j, k, item, lgth, rhs_lgth, nbt, X;
  SXINT    *item2nbt;
  VARSTR vstr;

  if (sxverbosep)
    fputs ("   Huge Bnf Output\n", sxtty);

  vstr = varstr_alloc (128);

  output_header (B->bnf_modif_time, langname);

  printf ("#define ntmax    %ld\n", (SXINT) B->WS_TBL_SIZE.ntmax);
  printf ("#define tmax    (%ld)\n", (SXINT) B->WS_TBL_SIZE.tmax);
  printf ("#define EOF_CODE %ld\n", (SXINT) (-B->WS_TBL_SIZE.tmax));
  printf ("#define itemmax %ld\n", (SXINT) B->WS_TBL_SIZE.indpro);
  printf ("#define prodmax %ld\n", (SXINT) B->WS_TBL_SIZE.nbpro);
  printf ("#define is_epsilon      %i\n", B->BVIDE != NULL && !sxba_is_empty (B->BVIDE));

  out_ifdef ("lispro");
  printf ("static SXINT lispro [%ld] = {\n", (SXINT) B->WS_TBL_SIZE.indpro+1);
  printf ("/* 0 */ %ld,\n", (SXINT) B->WS_TBL_SIZE.tmax); /* Pas bon ds B->WS_INDPRO [0].lispro !! */
  for (i = 1; i <= B->WS_TBL_SIZE.indpro; i++) {
    printf ("/* %ld */ %ld,\n", (SXINT) i, (SXINT) B->WS_INDPRO [i].lispro);
  }
  fputs ("};\n", stdout);
  out_endif ("lispro");

  out_ifdef ("prolis");
  printf ("static SXINT prolis [%ld] = {\n", (SXINT) B->WS_TBL_SIZE.indpro+1);
  for (i = 0; i <= B->WS_TBL_SIZE.indpro; i++) {
    printf ("/* %ld */ %ld,\n", (SXINT) i, (SXINT) B->WS_INDPRO [i].prolis);
  }
  fputs ("};\n", stdout);
  out_endif ("prolis");

  out_ifdef ("prolon");
  printf ("static SXINT prolon [%ld] = {\n", (SXINT) B->WS_TBL_SIZE.nbpro+2);
  rhs_lgth = 0;
  for (i = 0; i <= B->WS_TBL_SIZE.nbpro+1; i++) {
    item = B->WS_NBPRO [i].prolon;
    if (i <= B->WS_TBL_SIZE.nbpro) {
      lgth = B->WS_NBPRO [i+1].prolon - item - 1;
      if (lgth > rhs_lgth)
	rhs_lgth = lgth;
    }

    printf ("/* %ld */ %ld,\n", (SXINT) i, (SXINT) item);
  }
  fputs ("};\n", stdout);
  out_endif ("prolon");

  printf ("#define rhs_lgth    %ld\n", (SXINT) rhs_lgth);
  fputs ("#define rhs_maxnt    rhs_lgth /* ATTENTION : valeur non defaut !! */\n", stdout);

  /* On en profite pour remplir item2nbt */
  item2nbt = (SXINT*) sxcalloc (B->WS_TBL_SIZE.indpro+1, sizeof (SXINT));

  out_ifdef ("lhs");
  printf ("static SXINT lhs [%ld] = {\n", (SXINT) B->WS_TBL_SIZE.nbpro+1);
  for (i = 0; i <= B->WS_TBL_SIZE.nbpro; i++) {
    printf ("/* %ld */ %ld,\n", (SXINT) i, (SXINT) B->WS_NBPRO [i].reduc);

    item = B->WS_NBPRO [i].prolon;
    nbt = 0;

    while ((X = B->WS_INDPRO [item++].lispro) != 0) {
      if (X < 0)
	nbt++;

      item2nbt [item] = nbt; /* nb de t a gauche */
    }
  }
  fputs ("};\n", stdout);
  out_endif ("lhs");

  out_ifdef ("item2nbt");
  puts ("static SXINT item2nbt [] = {");

  for (i = 0; i <= B->WS_TBL_SIZE.nbpro; i++) {
    printf ("/* %ld */\t", (SXINT) i);

    item = B->WS_NBPRO [i].prolon;

    while ((X = B->WS_INDPRO [item].lispro) != 0) {
      printf ("%ld, ", (SXINT) item2nbt [item]);
      item++;
    }

    printf ("%ld,\n", (SXINT) item2nbt [item]);
  }

  puts ("};");
  out_endif ("item2nbt");

  sxfree (item2nbt);

  out_ifdef ("BVIDE");
  sxba2c (B->BVIDE, stdout, "BVIDE", "", true, vstr);
  out_endif ("BVIDE");

  out_ifdef ("tstring");
  printf ("static char *tstring [%ld] = {\n\
/* 0 */ \"\",\n", (SXINT) -B->WS_TBL_SIZE.tmax+1);
  for (i = -1; i >=  B->WS_TBL_SIZE.tmax; i--) {
    varstr_raz (vstr);
    vstr = (VARSTR) cat_t_string (vstr, i);

    printf ("/* %ld */ %s,\n", (SXINT) i, varstr_tostr (vstr));
  }
  fputs ("};\n", stdout);
  out_endif ("tstring");

  out_ifdef ("sxdfa_comb_inflected_form");
  fputs ("#include \"fsa.h\"\n", stdout);
#if 0
  fputs ("#define USE_A_DICO 0\n", stdout);
#endif /* 0 */
  {
    char                    *t_string;
    struct word_tree_struct word_tree_struct_inflected_form;
    struct sxdfa_struct     sxdfa_struct_inflected_form;
    struct sxdfa_comb       sxdfa_comb_inflected_form;

    word_tree_alloc (&word_tree_struct_inflected_form, "word_tree_struct_inflected_form", -B->WS_TBL_SIZE.tmax /* word_nb */, (B->WS_TBL_SIZE.t_string_length/(-B->WS_TBL_SIZE.tmax))+1 /* word_lgth */,
		     1 /* Xforeach */, 0 /* Yforeach */, true /* from_left_to_right */, true /* with_path2id */, (sxoflw0_t) NULL /* void (*oflw) () */, NULL /* FILE *stats */);
	  
    for (i = -1; i >  B->WS_TBL_SIZE.tmax; i--) {
      t_string = B->T_STRING+B->ADR [i];
      word_tree_add_a_string (&word_tree_struct_inflected_form, t_string, strlen (t_string), -i);
    }

    word_tree2sxdfa (&word_tree_struct_inflected_form, &sxdfa_struct_inflected_form, "sxdfa_struct_inflected_form", NULL /* FILE *stats */, true /* to_be_minimized */);
    sxdfa2comb_vector (&sxdfa_struct_inflected_form, 0 /* optim_kind (comb simple) */, 10000 /* comb_vector_threshold */, &sxdfa_comb_inflected_form);
    sxdfa_comb2c (&sxdfa_comb_inflected_form, stdout, "sxdfa_comb_inflected_form", true /* is_static */);
    sxdfa_comb_free (&sxdfa_comb_inflected_form);

#if 0
    sxword_header      inflected_form_names;

    sxword_alloc (&inflected_form_names, "inflected_form_names", -B->WS_TBL_SIZE.tmax, 1, (B->WS_TBL_SIZE.t_string_length/(-B->WS_TBL_SIZE.tmax))+1,
		  sxcont_malloc, sxcont_alloc, sxcont_realloc, NULL, sxcont_free, NULL, NULL);

    for (i = -1; i >  B->WS_TBL_SIZE.tmax; i--) {
      sxword_save (&inflected_form_names, B->T_STRING+B->ADR [i]);
    }

    sxword_to_c (&inflected_form_names, stdout, "inflected_form_names", true);

    sxword_free (&inflected_form_names);
#endif /* 0 */
  }
  out_endif ("sxdfa_comb_inflected_form");

  out_ifdef ("sxdfa_comb_non_terminal_names");
  fputs ("#include \"fsa.h\"\n", stdout);
  {
    char                    *nt_string;
    struct word_tree_struct word_tree_struct_non_terminal_names;
    struct sxdfa_struct     sxdfa_struct_non_terminal_names;
    struct sxdfa_comb       sxdfa_comb_non_terminal_names;

    word_tree_alloc (&word_tree_struct_non_terminal_names, "word_tree_struct_non_terminal_names", B->WS_TBL_SIZE.ntmax /* word_nb */, (B->WS_TBL_SIZE.nt_string_length/(B->WS_TBL_SIZE.ntmax))+1 /* word_lgth */,
		     1 /* Xforeach */, 0 /* Yforeach */, true /* from_left_to_right */, true /* with_path2id */, (sxoflw0_t) NULL /* void (*oflw) () */, NULL /* FILE *stats */);
	  
    for (i = 1; i <  B->WS_TBL_SIZE.ntmax; i++) {
      nt_string = B->NT_STRING+B->ADR [i];
      word_tree_add_a_string (&word_tree_struct_non_terminal_names, nt_string, strlen (nt_string), i);
    }

    word_tree2sxdfa (&word_tree_struct_non_terminal_names, &sxdfa_struct_non_terminal_names, "sxdfa_struct_non_terminal_names", NULL /* FILE *stats */, true /* to_be_minimized */);
    sxdfa2comb_vector (&sxdfa_struct_non_terminal_names, 0 /* optim_kind (comb simple) */, 10000 /* comb_vector_threshold */, &sxdfa_comb_non_terminal_names);
    out_ifdef ("sxdfa_comb_non_terminal_names");
    sxdfa_comb2c (&sxdfa_comb_non_terminal_names, stdout, "sxdfa_comb_non_terminal_names", true /* is_static */);
    out_endif ("sxdfa_comb_non_terminal_names");
    sxdfa_comb_free (&sxdfa_comb_non_terminal_names);

#if 0
    sxword_header      non_terminal_names;

    sxword_alloc (&non_terminal_names, "non_terminal_names", B->WS_TBL_SIZE.ntmax, 1, (B->WS_TBL_SIZE.nt_string_length/(B->WS_TBL_SIZE.ntmax))+1,
		  sxcont_malloc, sxcont_alloc, sxcont_realloc, NULL, sxcont_free, NULL, NULL);

    for (i = 1; i <  B->WS_TBL_SIZE.ntmax; i++) {
      sxword_save (&non_terminal_names, B->NT_STRING+B->ADR [i]);
    }

    sxword_to_c (&non_terminal_names, stdout, "non_terminal_names", true);

    sxword_free (&non_terminal_names);
#endif /* 0 */
  }
  out_endif ("sxdfa_comb_non_terminal_names");

  out_ifdef ("ntstring");
  {
    SXINT  bnf_huge_lgth;
    char *str;
    printf ("static char *ntstring [%ld] = {\n\
/* 0 */ \"\",\n", (SXINT) B->WS_TBL_SIZE.ntmax+1);
    for (i = 1; i <=  B->WS_TBL_SIZE.ntmax; i++) {
      str = B->NT_STRING+B->ADR [i];
      bnf_huge_lgth = strlen (str);
      str [bnf_huge_lgth-1] = SXNUL;
      printf ("/* %ld */ \"%s\",\n", (SXINT) i, str+1);
      str [bnf_huge_lgth-1] = '>';
    }
    fputs ("};\n", stdout);
  }
  out_endif ("ntstring");

  {
    /* multiple_t_prod_set = { p | p = A -> \alpha t1 \beta t2 \gamma} */
    /* t2prod_list [t2prod_hd [0]] = {A -> \alpha \beta | \alpha \beta \in NT*} */
    /* t2prod_list [t2prod_hd [t]] = {A -> \alpha \beta | \alpha \beta == \gamma t \delta} */
    SXINT   A, t, bot, top, cur, prod, bnf_huge_item, prod_list_top, prev_prod, nb;
    SXINT   *t2prod_hd, *t2prod_list;
    SXBA  multiple_t_prod_set, prod_set, unlexicalized_prod_set;

    multiple_t_prod_set = NULL;
    prod_set = sxba_calloc (B->WS_TBL_SIZE.nbpro+1);
    t2prod_hd = (SXINT*) sxalloc ((-B->WS_TBL_SIZE.tmax)+2, sizeof (SXINT));
    t2prod_list = (SXINT*) sxalloc (B->WS_TBL_SIZE.indpro+1, sizeof (SXINT));
    
    for (t = -1; t > B->WS_TBL_SIZE.tmax; t--) {
      bot = B->TPD [t];
      top = B->TPD [t-1];

      for (cur = bot; cur < top; cur++) {
	bnf_huge_item = B->TNUMPD [cur];
	prod = B->WS_INDPRO [bnf_huge_item].prolis;

	if (!SXBA_bit_is_reset_set (prod_set, prod)) {
	  /* prod est a ancres multiples */
	  if (multiple_t_prod_set == NULL)
	    multiple_t_prod_set = sxba_calloc (B->WS_TBL_SIZE.nbpro+1);
	    
	  SXBA_1_bit (multiple_t_prod_set, prod);
	}
      }
      
    } 

    t2prod_hd [0] = prod_list_top = 0;

    prod = sxba_0_lrscan (prod_set, 0);

    if (prod > 0) {
      /* Y'a des prod non lexicalisees */
      unlexicalized_prod_set = sxba_calloc (B->WS_TBL_SIZE.nbpro+1);

      do {
	/* prod n'est pas lexicalisee */
	SXBA_1_bit (unlexicalized_prod_set, prod);
#if 0
	/* On les met ds un SXBA */
	t2prod_list [prod_list_top] = prod;
	prod_list_top++;
#endif /* 0 */
      } while ((prod = sxba_0_lrscan (prod_set, prod)) > 0);
    }
    else
      unlexicalized_prod_set = NULL;

    t2prod_hd [1] = prod_list_top;

    for (t = -1; t > B->WS_TBL_SIZE.tmax; t--) {
      bot = B->TPD [t];
      top = B->TPD [t-1];
      prev_prod = 0;

      for (cur = bot; cur < top; cur++) {
	bnf_huge_item = B->TNUMPD [cur];
	prod = B->WS_INDPRO [bnf_huge_item].prolis;

	if (prod != prev_prod) {
	  prev_prod = prod;
	  t2prod_list [prod_list_top] = prod;
	  prod_list_top++;
	}
      }
      
      t2prod_hd [-t+1] = prod_list_top;
    } 
    
    t2prod_list [prod_list_top] = 0; /* super production */
    prod_list_top++;

    t2prod_hd [-(B->WS_TBL_SIZE.tmax)+1] = prod_list_top;
    t2prod_list [prod_list_top] = 0; /* pour etre propre */

    sxfree (prod_set);

    out_ifdef ("multiple_t_prod_set");
    fputs ("#define multiple_anchor_prod_nb ", stdout);
    if (multiple_t_prod_set) {
      printf ("%li\n", (SXINT) sxba_cardinal (multiple_t_prod_set));
      sxba2c (multiple_t_prod_set, stdout, "multiple_t_prod_set", "", true, vstr);
      sxfree (multiple_t_prod_set);
    }
    else {
      fputs ("0\n", stdout);
      fputs ("static SXBA multiple_t_prod_set;\n", stdout);
    }
    out_endif ("multiple_t_prod_set");

    
    out_ifdef ("t2prod_hd");
    if (unlexicalized_prod_set) {
      sxba2c (unlexicalized_prod_set, stdout, "unlexicalized_prod_set", "", true, vstr);
      sxfree (unlexicalized_prod_set);
    }
    else
      fputs ("static SXBA unlexicalized_prod_set;\n", stdout);

    printf ("static SXINT t2prod_hd [%ld] = {\n", (SXINT) (-B->WS_TBL_SIZE.tmax)+2);
    for (i = 0; i <= (-B->WS_TBL_SIZE.tmax)+1; i++) {
      printf ("/* %ld */ %ld,\n", (SXINT) i, (SXINT) t2prod_hd [i]);
    }
    fputs ("};\n", stdout);
    printf ("static SXINT t2prod_list [%ld] = {\n", (SXINT) prod_list_top+1);
    
    for (i = 0; i <= (-B->WS_TBL_SIZE.tmax); i++) {
      bot = t2prod_hd [i];
      top = t2prod_hd [i+1];

      if (i == 0)
	fputs ("/* unlexicalized prods */ ", stdout);
      else
	printf ("/* %ld \"%s\" */ ", (SXINT) i, B->T_STRING+B->ADR [-i]);

      for (cur = bot; cur < top; cur++) {
	printf ("%ld, ", (SXINT) t2prod_list [cur]);
      }
	
      fputs ("\n", stdout);
    }
    fputs ("};\n", stdout);
    out_endif ("t2prod_hd");

    sxfree (t2prod_hd);
    sxfree (t2prod_list);

    
    nt2gen_t_set = sxbm_calloc (B->WS_TBL_SIZE.ntmax+1, -B->WS_TBL_SIZE.tmax+1);
    nt2gen_t_list = (SXINT*) sxcalloc (B->WS_TBL_SIZE.ntmax+1, sizeof (SXINT));
    fill_nt2gen_t_set (B);

    out_ifdef ("nt2gen_t_set");
    /* nt2gen_t_set n'est jamais vide car on a nt2gen_t_set [1] = T */
    sxbm3_to_c (stdout, nt2gen_t_set, B->WS_TBL_SIZE.ntmax+1, "nt2gen_t_set", "", true /* is_static */);
    
    if (gen_t_list) {
      printf ("static SXINT nt2gen_t_list [%ld] = {\n\
/* 0 */ 0,\n", (SXINT) B->WS_TBL_SIZE.ntmax+1);

      for (A = 1; A <= B->WS_TBL_SIZE.ntmax; A++) {
	printf ("/* %ld=%s */ %ld,\n", (SXINT) A, B->NT_STRING+B->ADR [A], (SXINT) nt2gen_t_list [A]);
      }
    
      fputs ("};\n", stdout);

      printf ("static SXINT gen_t_list [%ld] = {\n\
/* 0 */ 0,", (SXINT) gen_t_list_top);

      for (A = 1; A <= B->WS_TBL_SIZE.ntmax; A++) {
	if ((i = nt2gen_t_list [A])) {
	  printf ("\n/* %ld=%s */ ", (SXINT) A, B->NT_STRING+B->ADR [A]);

	  nb = gen_t_list [i++];
	  printf ("%ld /* nb */, ", (SXINT) nb);

	  do {
	    j = gen_t_list [i++];
	    printf ("%ld, ", (SXINT) j);
	  } while (--nb > 0);
	}
      }

      fputs ("\n};\n", stdout);

      sxfree (gen_t_list), gen_t_list = NULL;
    }
    else {
      fputs ("static SXINT *nt2gen_t_list;\n", stdout);
      fputs ("static SXINT *gen_t_list;\n", stdout);
    }

    out_endif ("nt2gen_t_set");

    sxfree (nt2gen_t_set), nt2gen_t_set = NULL;
  }

  out_ifdef ("npg");
  printf ("static SXINT npg [%ld] = {\n\
/* 0 */ 0,\n", (SXINT) B->WS_TBL_SIZE.ntmax+2);
  for (i = 1; i <=  B->WS_TBL_SIZE.ntmax+1; i++) {
    printf ("/* %ld */ %ld,\n", (SXINT) i, (SXINT) B->WS_NTMAX [i].npg);
  }
  fputs ("};\n", stdout);
  out_endif ("npg");

  out_ifdef ("numpg");
  printf ("static SXINT numpg [%ld] = {\n", (SXINT) B->WS_TBL_SIZE.nbpro+2);
  for (i = 0; i <= B->WS_TBL_SIZE.nbpro+1; i++) {
    printf ("/* %ld */ %ld,\n", (SXINT) i, (SXINT) B->WS_NBPRO [i].numpg);
  }
  fputs ("};\n", stdout);
  out_endif ("numpg");

  out_ifdef ("npd");
  printf ("static SXINT npd [%ld] = {\n", (SXINT) B->WS_TBL_SIZE.ntmax+2);
  for (i = 0; i <=  B->WS_TBL_SIZE.ntmax+1; i++) {
    printf ("/* %ld */ %ld,\n", (SXINT) i, (SXINT) B->WS_NTMAX [i].npd + ((i>1) ? i-1 : 0));
  }
  fputs ("};\n", stdout);
  out_endif ("npd");

  out_ifdef ("numpd");
  printf ("static SXINT numpd [%ld] = {\n", (SXINT) B->WS_TBL_SIZE.maxnpd+1+B->WS_TBL_SIZE.ntmax);
  k = 0;
  j = 1;
  for (i = 0; i <=  B->WS_TBL_SIZE.maxnpd; i++) {
    printf ("/* %ld */ %ld,\n", (SXINT) k, (SXINT) B->NUMPD [i]);
    k++;

    if (i > 0 && i+1 == B->WS_NTMAX [j+1].npd) {
      /* frontiere, on ajoute = 0 */
      printf ("/* %ld (frontiere) */ 0,\n", (SXINT) k);
      k++;
      j++;
    }
  }
  fputs ("};\n", stdout);
  out_endif ("numpd");

  out_ifdef ("tpd");
  printf ("static SXINT tpd [%ld] = {\n\
/* 0 */ 0,\n", (SXINT) (-B->WS_TBL_SIZE.tmax)+2);
  for (i = -1; i >=  B->WS_TBL_SIZE.tmax-1; i--) {
    printf ("/* %ld */ %ld,\n", (SXINT) i, (SXINT) B->TPD [i]);
  }
  fputs ("};\n", stdout);
  out_endif ("tpd");

  out_ifdef ("tnumpd");
  printf ("static SXINT tnumpd [%ld] = {\n", (SXINT) B->WS_TBL_SIZE.maxtpd+1);
  for (i = 0; i <=  B->WS_TBL_SIZE.maxtpd; i++) {
    printf ("/* %ld */ %ld,\n", (SXINT) i, (SXINT) B->TNUMPD [i]);
  }
  fputs ("};\n", stdout);
  out_endif ("tnumpd");

  {
    SXINT       prod, bnf_huge_item_2, head_item, bnf_huge_X, seg_id, top, bot, segment_hd_top, nb;
    SXINT       *ptr;
    SXINT       *segment, *segment_list, *segment_hd2id;
    XH_header segment_hd;

    XH_alloc (&segment_hd, "segment_hd", B->WS_TBL_SIZE.indpro-2*B->WS_TBL_SIZE.nbpro+1, 1, 5, segment_hd_oflw, NULL);
    segment_hd2nb = (SXINT*) sxalloc (XH_size (segment_hd)+1, sizeof (SXINT));
    segment_hd2nb [0] = 0;

    segment = (SXINT*) sxcalloc (B->WS_TBL_SIZE.indpro+1, sizeof (SXINT));

    /* On isole les segments */
    /* traitement manuel de la super-regle S' -> $ S $ */
    if (SXBA_bit_is_set (B->BVIDE, 1)) {
      /* eps est ds L */
      XH_push (segment_hd, B->WS_TBL_SIZE.tmax);
      XH_push (segment_hd, 1);
      XH_push (segment_hd, B->WS_TBL_SIZE.tmax);
      XH_set (&segment_hd, &seg_id);
      segment_hd2nb [seg_id] = 2; /* le 0 de fin */
      segment_hd2nb [0] += 2; /* le 0 de fin */
      segment [0] = seg_id;
      segment [2] = -1; /* peut etre ecrase' + tard */
      
    }
    else {
      XH_push (segment_hd, B->WS_TBL_SIZE.tmax);
      XH_push (segment_hd, 1);
      XH_set (&segment_hd, &seg_id);
      segment_hd2nb [seg_id] = 2; /* le 0 de fin */
      segment_hd2nb [0] += 2; /* le 0 de fin */
      segment [0] = seg_id;

      XH_push (segment_hd, 1);
      XH_push (segment_hd, B->WS_TBL_SIZE.tmax);
      XH_set (&segment_hd, &seg_id);
      segment_hd2nb [seg_id] = 2; /* le 0 de fin */
      segment_hd2nb [0] += 2; /* le 0 de fin */
      segment [1] = seg_id;
      segment [2] = -1; /* peut etre ecrase' + tard */
    }

    for (prod = 1; prod <= B->WS_TBL_SIZE.nbpro; prod++) {
      bnf_huge_item_2 = B->WS_NBPRO [prod].prolon;
      head_item = 0;

      while ((bnf_huge_X = B->WS_INDPRO [bnf_huge_item_2].lispro) != 0) {
	if (bnf_huge_X < 0 || !SXBA_bit_is_set (B->BVIDE, bnf_huge_X)) {
	  if (head_item) {
	    /* On vient d'isoler un segment */
	    /* On calcule son identifiant */
	    for (i = head_item; i <= bnf_huge_item_2; i++) {
	      XH_push (segment_hd, B->WS_INDPRO [i].lispro);
	    }

	    if (XH_set (&segment_hd, &seg_id)) {
	      segment_hd2nb [seg_id]++;
	      segment_hd2nb [0]++;
	    }
	    else {
	      segment_hd2nb [seg_id] = 2; /* le 0 de fin */
	      segment_hd2nb [0] += 2; /* le 0 de fin */
	    }

	    segment [head_item] = seg_id;
	    segment [bnf_huge_item_2] = -1; /* peut etre ecrase' + tard */
	  }
	  
	  head_item = bnf_huge_item_2;
	}

	bnf_huge_item_2++;
      }
    }

    segment_list = (SXINT*) sxalloc (segment_hd2nb [0]+1, sizeof (SXINT));
    segment_list [0] = 0;

    segment_hd_top = XH_top (segment_hd);

    segment_hd2id = (SXINT*) sxalloc (segment_hd_top+1, sizeof (SXINT));
    segment_hd2id [0] = 0;
	      
    top = 1;

    for (seg_id = 1; seg_id < segment_hd_top; seg_id++) {
      segment_hd2id [seg_id] = top;
      segment_list [top] = 0; /* nb d'elem ds la pile */
      top += segment_hd2nb [seg_id];
    }
    
    for (prod = 0; prod <= B->WS_TBL_SIZE.nbpro; prod++) {
      if (prod == 0)
	bnf_huge_item_2 = 0;
      else
	bnf_huge_item_2 = B->WS_NBPRO [prod].prolon;

      head_item = 0;

      while (bnf_huge_item_2 == 0 || B->WS_INDPRO [bnf_huge_item_2].lispro != 0) {
	seg_id = segment [bnf_huge_item_2];

	if (seg_id > 0) {
	  bot = segment [bnf_huge_item_2] = segment_hd2id [seg_id]; /* indice vers le bas de la pile */
	  segment_list [bot + ++segment_list [bot]] = bnf_huge_item_2; /* On push bnf_huge_item_2 ds cette "pile" */
	}

	bnf_huge_item_2++;
      }
    }

    out_ifdef ("segment");
    printf ("static SXINT segment [%ld] = {\n", (SXINT) B->WS_TBL_SIZE.indpro+1);
    for (prod = 0; prod <= B->WS_TBL_SIZE.nbpro; prod++) {
      printf ("/* prod=%ld */", (SXINT) prod);

      if (prod == 0)
	bnf_huge_item_2 = 0;
      else
	bnf_huge_item_2 = B->WS_NBPRO [prod].prolon;

      while (bnf_huge_item_2 == 0 || B->WS_INDPRO [bnf_huge_item_2].lispro != 0) {
	printf (" %ld,", (SXINT) segment [bnf_huge_item_2]);
	bnf_huge_item_2++;
      }

      fputs (" 0,\n", stdout);
    }
    fputs ("};\n", stdout);

    
    printf ("static SXINT segment_list [%ld] = {0,\n", (SXINT) segment_hd2nb [0]+1);

    for (seg_id = 1; seg_id < segment_hd_top; seg_id++) {
      bot = segment_hd2id [seg_id];
      ptr = segment_list+bot;
      nb = *ptr;
      printf ("/* seg_id=%ld */ %ld,", (SXINT) seg_id, (SXINT) nb);

      while (nb-- > 0) {
	printf (" %ld,", (SXINT) *++ptr);
      }

      fputs ("\n", stdout);
    }
    fputs ("};\n", stdout);

    out_endif ("segment");

    XH_free (&segment_hd);
    sxfree (segment_hd2nb), segment_hd2nb = NULL;
    sxfree (segment);
    sxfree (segment_list);
    sxfree (segment_hd2id);
  }

  varstr_free (vstr);
}
