/* ********************************************************
   *                                                      *
   * Copyright (c) 1985 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   ******************************************************** */



/* ********************************************************
   *     Produit de l'equipe Langages et Traducteurs.     *
   ******************************************************** */




/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 14-10-99 16:40 (phd) :	Utilisation de O_BINARY (pour Windows)	*/
/************************************************************************/
/* 14-10-99 16:39 (phd):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/


#include "varstr.h"
#include "sxunix.h"
#include "B_tables.h"
#include "sxword.h"
#include "sxstack.h"
char WHAT_BNFHUGE[] = "@(#)SYNTAX - $Id: bnf_huge.c 588 2007-05-25 12:32:11Z rlacroix $" WHAT_DEBUG;

static void
output_header (long int bnf_modif_time, char *langname)
{
  long int cur_time;

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

static int  *gen_t_list, gen_t_list_top;

static SXBA *nt2gen_t_set;
static int  *nt2gen_t_list;

/* nt2gen_t_set [A] = {t | A =>+ x1 t x2} */
static void
fill_nt2gen_t_set (struct bnf_ag_item *B)
{
  int  prod, item, A, X, bot, top, card, t;
  int  *nt_stack;
  SXBA nt_set, gen_t_set, is_an_int_list;

  nt_set = sxba_calloc (B->WS_TBL_SIZE.ntmax+1);
  nt_stack = (int*) sxalloc (B->WS_TBL_SIZE.ntmax+1, sizeof (int)), nt_stack [0] = 0;
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
	/* ... forme int * */
	gen_t_list_top += card+1;
	SXBA_1_bit (is_an_int_list, A);
      }
    }
  }

  if (gen_t_list_top) {
    gen_t_list = (int*) sxalloc (gen_t_list_top+1, sizeof (int)), gen_t_list [0] = 0;
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

extern VARSTR  cat_t_string (VARSTR, int);


#include "XH.h"

static int *segment_hd2nb;

static void
segment_hd_oflw (int old_size, int new_size)
{
  segment_hd2nb = (int*) sxrealloc (segment_hd2nb, new_size+1, sizeof (int));
}


VOID
bnf_huge (struct bnf_ag_item *B, char *langname)
{
  int    i, j, k, item, lgth, rhs_lgth, nbt, X;
  int    *item2nbt;
  VARSTR vstr;

  if (sxverbosep)
    fputs ("   Huge Bnf Output\n", sxtty);

  vstr = varstr_alloc (128);

  output_header (B->bnf_modif_time, langname);

  printf ("#define ntmax    %i\n", B->WS_TBL_SIZE.ntmax);
  printf ("#define tmax    (%i)\n", B->WS_TBL_SIZE.tmax);
  printf ("#define itemmax %i\n", B->WS_TBL_SIZE.indpro);
  printf ("#define prodmax %i\n", B->WS_TBL_SIZE.nbpro);
  printf ("#define is_epsilon      %i\n", B->BVIDE != NULL && !sxba_is_empty (B->BVIDE));

  fputs ("\n#ifdef def_lispro\n", stdout);
  printf ("static int lispro [%i] = {\n", B->WS_TBL_SIZE.indpro+1);
  printf ("/* 0 */ %i,\n", B->WS_TBL_SIZE.tmax); /* Pas bon ds B->WS_INDPRO [0].lispro !! */
  for (i = 1; i <= B->WS_TBL_SIZE.indpro; i++) {
    printf ("/* %i */ %i,\n", i, B->WS_INDPRO [i].lispro);
  }
  fputs ("};\n", stdout);
  fputs ("#endif /* def_lispro */\n", stdout);

  fputs ("\n#ifdef def_prolis\n", stdout);
  printf ("static int prolis [%i] = {\n", B->WS_TBL_SIZE.indpro+1);
  for (i = 0; i <= B->WS_TBL_SIZE.indpro; i++) {
    printf ("/* %i */ %i,\n", i, B->WS_INDPRO [i].prolis);
  }
  fputs ("};\n", stdout);
  fputs ("#endif /* def_prolis */\n", stdout);

  fputs ("\n#ifdef def_prolon\n", stdout);
  printf ("static int prolon [%i] = {\n", B->WS_TBL_SIZE.nbpro+2);
  rhs_lgth = 0;
  for (i = 0; i <= B->WS_TBL_SIZE.nbpro+1; i++) {
    item = B->WS_NBPRO [i].prolon;
    lgth = B->WS_NBPRO [i+1].prolon - item - 1;

    if (lgth > rhs_lgth)
      rhs_lgth = lgth;

    printf ("/* %i */ %i,\n", i, item);
  }
  fputs ("};\n", stdout);
  fputs ("#endif /* def_prolon */\n", stdout);

  printf ("#define rhs_lgth    %i\n", rhs_lgth);
  fputs ("#define rhs_maxnt    rhs_lgth /* ATTENTION : valeur non defaut !! */\n", stdout);

  /* On en profite pour remplir item2nbt */
  item2nbt = (int*) sxcalloc (B->WS_TBL_SIZE.indpro+1, sizeof (int));

  fputs ("\n#ifdef def_lhs\n", stdout);
  printf ("static int lhs [%i] = {\n", B->WS_TBL_SIZE.nbpro+1);
  for (i = 0; i <= B->WS_TBL_SIZE.nbpro; i++) {
    printf ("/* %i */ %i,\n", i, B->WS_NBPRO [i].reduc);

    item = B->WS_NBPRO [i].prolon;
    nbt = 0;

    while ((X = B->WS_INDPRO [item++].lispro) != 0) {
      if (X < 0)
	nbt++;

      item2nbt [item] = nbt; /* nb de t a gauche */
    }
  }
  fputs ("};\n", stdout);
  fputs ("#endif /* def_lhs */\n", stdout);

  fputs ("\n#ifdef def_item2nbt\n", stdout);
  puts ("static int item2nbt [] = {");

  for (i = 0; i <= B->WS_TBL_SIZE.nbpro; i++) {
    printf ("/* %i */\t", i);

    item = B->WS_NBPRO [i].prolon;

    while ((X = B->WS_INDPRO [item].lispro) != 0) {
      printf ("%i, ", item2nbt [item]);
      item++;
    }

    printf ("%i,\n", item2nbt [item]);
  }

  puts ("};");
  fputs ("#endif /* def_item2nbt */\n", stdout);

  sxfree (item2nbt);

  fputs ("\n#ifdef def_BVIDE\n", stdout);
  sxba2c (B->BVIDE, stdout, "BVIDE", "", TRUE, vstr);
  fputs ("#endif /* def_BVIDE */\n", stdout);

  fputs ("\n#ifdef def_tstring\n", stdout);
  printf ("static char *tstring [%i] = {\n\
/* 0 */ \"\",\n", -B->WS_TBL_SIZE.tmax+1);
  for (i = -1; i >=  B->WS_TBL_SIZE.tmax; i--) {
    varstr_raz (vstr);
    vstr = (VARSTR) cat_t_string (vstr, i);

    printf ("/* %i */ %s,\n", i, varstr_tostr (vstr));
  }
  fputs ("};\n", stdout);
  fputs ("#endif /* def_tstring */\n", stdout);

  fputs ("\n#ifdef def_inflected_form_names\n", stdout);
  fputs ("#include \"sxword.h\"\n", stdout);
  fputs ("#define USE_A_DICO 0\n", stdout);
  {
    sxword_header      inflected_form_names;

    sxword_alloc (&inflected_form_names, "inflected_form_names", -B->WS_TBL_SIZE.tmax, 1, (B->WS_TBL_SIZE.t_string_length/(-B->WS_TBL_SIZE.tmax))+1,
		  sxcont_malloc, sxcont_alloc, sxcont_realloc, NULL, sxcont_free, NULL, NULL);

    for (i = -1; i >  B->WS_TBL_SIZE.tmax; i--) {
      sxword_save (&inflected_form_names, B->T_STRING+B->ADR [i]);
    }

    sxword_to_c (&inflected_form_names, stdout, "inflected_form_names", TRUE);

    sxword_free (&inflected_form_names);
  }
  fputs ("#endif /* def_inflected_form_names */\n", stdout);

  fputs ("\n#ifdef def_non_terminal_names\n", stdout);
  fputs ("#include \"sxword.h\"\n", stdout);
  {
    sxword_header      non_terminal_names;

    sxword_alloc (&non_terminal_names, "non_terminal_names", B->WS_TBL_SIZE.ntmax, 1, (B->WS_TBL_SIZE.nt_string_length/(B->WS_TBL_SIZE.ntmax))+1,
		  sxcont_malloc, sxcont_alloc, sxcont_realloc, NULL, sxcont_free, NULL, NULL);

    for (i = 1; i <  B->WS_TBL_SIZE.ntmax; i++) {
      sxword_save (&non_terminal_names, B->NT_STRING+B->ADR [i]);
    }

    sxword_to_c (&non_terminal_names, stdout, "non_terminal_names", TRUE);

    sxword_free (&non_terminal_names);
  }
  fputs ("#endif /* def_non_terminal_names */\n", stdout);

  fputs ("\n#ifdef def_ntstring\n", stdout);
  {
    int  bnf_huge_lgth;
    char *str;
    printf ("static char *ntstring [%i] = {\n\
/* 0 */ \"\",\n", B->WS_TBL_SIZE.ntmax+1);
    for (i = 1; i <=  B->WS_TBL_SIZE.ntmax; i++) {
      str = B->NT_STRING+B->ADR [i];
      bnf_huge_lgth = strlen (str);
      str [bnf_huge_lgth-1] = NUL;
      printf ("/* %i */ \"%s\",\n", i, str+1);
      str [bnf_huge_lgth-1] = '>';
    }
    fputs ("};\n", stdout);
  }
  fputs ("#endif /* def_ntstring */\n", stdout);

  {
    /* multiple_t_prod_set = { p | p = A -> \alpha t1 \beta t2 \gamma} */
    /* t2prod_list [t2prod_hd [0]] = {A -> \alpha \beta | \alpha \beta \in NT*} */
    /* t2prod_list [t2prod_hd [t]] = {A -> \alpha \beta | \alpha \beta == \gamma t \delta} */
    int   A, t, bot, top, cur, prod, bnf_huge_item, prod_list_top, prev_prod, nb;
    int   *t2prod_hd, *t2prod_list;
    SXBA  multiple_t_prod_set, prod_set, unlexicalized_prod_set;

    multiple_t_prod_set = NULL;
    prod_set = sxba_calloc (B->WS_TBL_SIZE.nbpro+1);
    t2prod_hd = (int*) sxalloc ((-B->WS_TBL_SIZE.tmax)+2, sizeof (int));
    t2prod_list = (int*) sxalloc (B->WS_TBL_SIZE.indpro+1, sizeof (int));
    
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

    fputs ("\n#ifdef def_multiple_t_prod_set\n", stdout);
    fputs ("#define multiple_anchor_prod_nb ", stdout);
    if (multiple_t_prod_set) {
      printf ("%i\n", sxba_cardinal (multiple_t_prod_set));
      sxba2c (multiple_t_prod_set, stdout, "multiple_t_prod_set", "", TRUE, vstr);
      sxfree (multiple_t_prod_set);
    }
    else {
      fputs ("0\n", stdout);
      fputs ("static SXBA multiple_t_prod_set;\n", stdout);
    }
    fputs ("#endif /* def_multiple_t_prod_set */\n", stdout);

    
    fputs ("\n#ifdef def_t2prod_hd\n", stdout);
    if (unlexicalized_prod_set) {
      sxba2c (unlexicalized_prod_set, stdout, "unlexicalized_prod_set", "", TRUE, vstr);
      sxfree (unlexicalized_prod_set);
    }
    else
      fputs ("static SXBA unlexicalized_prod_set;\n", stdout);

    printf ("static int t2prod_hd [%i] = {\n", (-B->WS_TBL_SIZE.tmax)+2);
    for (i = 0; i <= (-B->WS_TBL_SIZE.tmax)+1; i++) {
      printf ("/* %i */ %i,\n", i, t2prod_hd [i]);
    }
    fputs ("};\n", stdout);
    printf ("static int t2prod_list [%i] = {\n", prod_list_top+1);
    
    for (i = 0; i <= (-B->WS_TBL_SIZE.tmax); i++) {
      bot = t2prod_hd [i];
      top = t2prod_hd [i+1];

      if (i == 0)
	fputs ("/* unlexicalized prods */ ", stdout);
      else
	printf ("/* %i \"%s\" */ ", i, B->T_STRING+B->ADR [-i]);

      for (cur = bot; cur < top; cur++) {
	printf ("%i, ", t2prod_list [cur]);
      }
	
      fputs ("\n", stdout);
    }
    fputs ("};\n", stdout);
    fputs ("#endif /* def_t2prod_hd */\n", stdout);

    sxfree (t2prod_hd);
    sxfree (t2prod_list);

    
    nt2gen_t_set = sxbm_calloc (B->WS_TBL_SIZE.ntmax+1, -B->WS_TBL_SIZE.tmax+1);
    nt2gen_t_list = (int*) sxcalloc (B->WS_TBL_SIZE.ntmax+1, sizeof (int));
    fill_nt2gen_t_set (B);
    fputs ("\n#ifdef def_nt2gen_t_set\n", stdout);
    
    /* nt2gen_t_set n'est jamais vide car on a nt2gen_t_set [1] = T */
    sxbm3_to_c (stdout, nt2gen_t_set, B->WS_TBL_SIZE.ntmax+1, "nt2gen_t_set", "", TRUE /* is_static */);
    
    if (gen_t_list) {
      printf ("static int nt2gen_t_list [%i] = {\n\
/* 0 */ 0,\n", B->WS_TBL_SIZE.ntmax+1);

      for (A = 1; A <= B->WS_TBL_SIZE.ntmax; A++) {
	printf ("/* %i=%s */ %i,\n", A, B->NT_STRING+B->ADR [A], nt2gen_t_list [A]);
      }
    
      fputs ("};\n", stdout);

      printf ("static int gen_t_list [%i] = {\n\
/* 0 */ 0,", gen_t_list_top);

      for (A = 1; A <= B->WS_TBL_SIZE.ntmax; A++) {
	if ((i = nt2gen_t_list [A])) {
	  printf ("\n/* %i=%s */ ", A, B->NT_STRING+B->ADR [A]);

	  nb = gen_t_list [i++];
	  printf ("%i /* nb */, ", nb);

	  do {
	    j = gen_t_list [i++];
	    printf ("%i, ", j);
	  } while (--nb > 0);
	}
      }

      fputs ("\n};\n", stdout);

      sxfree (gen_t_list), gen_t_list = NULL;
    }
    else {
      fputs ("static int *nt2gen_t_list;\n", stdout);
      fputs ("static int *gen_t_list;\n", stdout);
    }

    fputs ("\n#endif /* def_nt2gen_t_set */\n", stdout);
    sxfree (nt2gen_t_set), nt2gen_t_set = NULL;
  }

  fputs ("\n#ifdef def_npg\n", stdout);
  printf ("static int npg [%i] = {\n\
/* 0 */ 0,\n", B->WS_TBL_SIZE.ntmax+2);
  for (i = 1; i <=  B->WS_TBL_SIZE.ntmax+1; i++) {
    printf ("/* %i */ %i,\n", i, B->WS_NTMAX [i].npg);
  }
  fputs ("};\n", stdout);
  fputs ("#endif /* def_npg */\n", stdout);

  fputs ("\n#ifdef def_numpg\n", stdout);
  printf ("static int numpg [%i] = {\n", B->WS_TBL_SIZE.nbpro+2);
  for (i = 0; i <= B->WS_TBL_SIZE.nbpro+1; i++) {
    printf ("/* %i */ %i,\n", i, B->WS_NBPRO [i].numpg);
  }
  fputs ("};\n", stdout);
  fputs ("#endif /* def_numpg */\n", stdout);

  fputs ("\n#ifdef def_npd\n", stdout);
  printf ("static int npd [%i] = {\n", B->WS_TBL_SIZE.ntmax+2);
  for (i = 0; i <=  B->WS_TBL_SIZE.ntmax+1; i++) {
    printf ("/* %i */ %i,\n", i, B->WS_NTMAX [i].npd + ((i>1) ? i-1 : 0));
  }
  fputs ("};\n", stdout);
  fputs ("#endif /* def_npd */\n", stdout);

  fputs ("\n#ifdef def_numpd\n", stdout);
  printf ("static int numpd [%i] = {\n", B->WS_TBL_SIZE.maxnpd+1+B->WS_TBL_SIZE.ntmax);
  k = 0;
  j = 1;
  for (i = 0; i <=  B->WS_TBL_SIZE.maxnpd; i++) {
    printf ("/* %i */ %i,\n", k, B->NUMPD [i]);
    k++;

    if (i > 0 && i+1 == B->WS_NTMAX [j+1].npd) {
      /* frontiere, on ajoute = 0 */
      printf ("/* %i (frontiere) */ 0,\n", k);
      k++;
      j++;
    }
  }
  fputs ("};\n", stdout);
  fputs ("#endif /* def_numpd */\n", stdout);

  fputs ("\n#ifdef def_tpd\n", stdout);
  printf ("static int tpd [%i] = {\n\
/* 0 */ 0,\n", (-B->WS_TBL_SIZE.tmax)+2);
  for (i = -1; i >=  B->WS_TBL_SIZE.tmax-1; i--) {
    printf ("/* %i */ %i,\n", i, B->TPD [i]);
  }
  fputs ("};\n", stdout);
  fputs ("#endif /* def_tpd */\n", stdout);

  fputs ("\n#ifdef def_tnumpd\n", stdout);
  printf ("static int tnumpd [%i] = {\n", B->WS_TBL_SIZE.maxtpd+1);
  for (i = 0; i <=  B->WS_TBL_SIZE.maxtpd; i++) {
    printf ("/* %i */ %i,\n", i, B->TNUMPD [i]);
  }
  fputs ("};\n", stdout);
  fputs ("#endif /* def_tnumpd */\n", stdout);

  {
    int       prod, bnf_huge_item_2, head_item, bnf_huge_X, seg_id, top, bot, segment_hd_top, nb;
    int       *ptr;
    int       *segment, *segment_list, *segment_hd2id;
    XH_header segment_hd;

    XH_alloc (&segment_hd, "segment_hd", B->WS_TBL_SIZE.indpro-2*B->WS_TBL_SIZE.nbpro+1, 1, 5, segment_hd_oflw, NULL);
    segment_hd2nb = (int*) sxalloc (XH_size (segment_hd)+1, sizeof (int));
    segment_hd2nb [0] = 0;

    segment = (int*) sxcalloc (B->WS_TBL_SIZE.indpro+1, sizeof (int));

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

    segment_list = (int*) sxalloc (segment_hd2nb [0]+1, sizeof (int));
    segment_list [0] = 0;

    segment_hd_top = XH_top (segment_hd);

    segment_hd2id = (int*) sxalloc (segment_hd_top+1, sizeof (int));
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

    fputs ("\n#ifdef def_segment\n", stdout);
    printf ("static int segment [%i] = {\n", B->WS_TBL_SIZE.indpro+1);
    for (prod = 0; prod <= B->WS_TBL_SIZE.nbpro; prod++) {
      printf ("/* prod=%i */", prod);

      if (prod == 0)
	bnf_huge_item_2 = 0;
      else
	bnf_huge_item_2 = B->WS_NBPRO [prod].prolon;

      while (bnf_huge_item_2 == 0 || B->WS_INDPRO [bnf_huge_item_2].lispro != 0) {
	printf (" %i,", segment [bnf_huge_item_2]);
	bnf_huge_item_2++;
      }

      fputs (" 0,\n", stdout);
    }
    fputs ("};\n", stdout);

    
    printf ("static int segment_list [%i] = {0,\n", segment_hd2nb [0]+1);

    for (seg_id = 1; seg_id < segment_hd_top; seg_id++) {
      bot = segment_hd2id [seg_id];
      ptr = segment_list+bot;
      nb = *ptr;
      printf ("/* seg_id=%i */ %i,", seg_id, nb);

      while (nb-- > 0) {
	printf (" %i,", *++ptr);
      }

      fputs ("\n", stdout);
    }
    fputs ("};\n", stdout);

    fputs ("#endif /* def_segment */\n", stdout);

    XH_free (&segment_hd);
    sxfree (segment_hd2nb), segment_hd2nb = NULL;
    sxfree (segment);
    sxfree (segment_list);
    sxfree (segment_hd2id);
  }

  varstr_free (vstr);
}
