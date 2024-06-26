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

/* Fait qq verifs sur la sortie de BNF :
   - cycles
   - recursivite gauche et droite
*/

#include "sxversion.h"
#include "sxunix.h"
#include "B_tables.h"
#include "XH.h"
#include "XxY.h"

char WHAT_CHECKBNF[] = "@(#)SYNTAX - $Id: check_bnf.c 3633 2023-12-20 18:41:19Z garavel $" WHAT_DEBUG;

#if 0
struct bnf_ag_item {
    SXINT          bnf_modif_time;
    struct ws_tbl_size_s WS_TBL_SIZE;
    struct ws_nbpro_s	*WS_NBPRO;			/* 0:xnbpro+1		 */
    struct ws_indpro_s	*WS_INDPRO;			/* 0:indpro		 */
    struct xv_to_v_prdct_s	*XT_TO_T_PRDCT;		/* 0:xtmax+tmax	 	 */
    struct xv_to_v_prdct_s	*XNT_TO_NT_PRDCT;	/* 0:xntmax-ntmax	 */
    struct ws_ntmax_s	*WS_NTMAX;			/* 0:xntmax+1		 */
    SXINT          *ADR;					/* -xtmax-1:xntmax+1	 */
    SXINT          *TPD;					/* -xtmax-1:-1		 */
    SXINT          *NUMPD;				/* 0:maxnpd		 */
    SXINT          *TNUMPD;				/* 0:maxtpd		 */
    SXINT          *RULE_TO_REDUCE;			/* 0:xnbpro-actpro	 */
    SXBA          IS_A_GENERIC_TERMINAL;		/* -tmax		 */
    SXBA          BVIDE;				/* xntmax		 */
    SXBA          NULLABLE;				/* indpro		 */
    SXBA         *FIRST;				/* 1:ntmax,1:xtmax	 */
    SXBA         *FOLLOW;				/* 1:-tmax,1:-tmax	 */
    char         *T_STRING;				/* t_string_length	 */
    char         *NT_STRING;				/* nt_string_length	 */
};
#endif /* 0 */

#define TMAX      bnf_ag.WS_TBL_SIZE.tmax
#define NTMAX     bnf_ag.WS_TBL_SIZE.ntmax
#define NBPRO     bnf_ag.WS_TBL_SIZE.nbpro
#define INDPRO    bnf_ag.WS_TBL_SIZE.indpro
#define NPG(n)    bnf_ag.WS_NTMAX [n].npg

#define XNTMAX           (NTMAX+XxY_top(xnt_hd)) 
#define XINDPRO          XH_list_top (prod_hd)

#define PROLON(t,r)      (XH_X (t, r)+1)
#define LISPRO(t,i)      XH_list_elem (t, i)
#define TOP_ITEM(t,r)    (XH_X (t, r+1))
#define REDUC(t,r)       LISPRO (t, XH_X (t, r))
#define PROLIS(i)        item2prod [i]
#define foreach_NPG(A,x) XxY_Xforeach (lhsntXprod_hd, A, x)
#define NUMPG(x)         XxY_Y (lhsntXprod_hd, x)
/* utilisation de foreach_NPG et NUMPG pour parcourir toutes les A-prod :
   foreach_NPG (A, x) {
      prod = NUMPG (x); 
      ...
   }
*/


#define PUSH(t,x)   t[++t[0]]=x
#define POP(t)      (t[t[0]--])
#define IS_EMPTY(t) (t[0]==0)

static struct bnf_ag_item bnf_ag;

/*---------------*/
/*    options    */
/*---------------*/

static bool  is_help, is_debug1, is_debug2, is_stdout, is_lexicon;
static bool  is_cycle_free, is_lr_recursive_free, is_full_eps_rule_free, is_eps_rule_free;
static char	ME [] = "check_bnf";
static char	Usage [] = "\
Usage:\t%s [options] language_name\n\
options=\t--help,\n\
\t\t-d1, -debug1\n\
\t\t-d2, -debug2\n\
\t\t-l, -lexico\n\
\t\t-ferf, -full_eps_rule_free\n\
\t\t-erf, -eps_rule_free\n\
\t\t-cf, -cycle_free\n\
\t\t-lrrf, -left_right_recursive_free\n";

#define OPTION(opt)	(1 << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB ((sizeof (option_kind)/sizeof (option_kind[0]))-1)

#define UNKNOWN_ARG 		0
#define HELP	                1
#define DEBUG1	                2
#define DEBUG2	                3
#define LEXICON	                4
#define FERF	                5
#define ERF	                6
#define CF	                7
#define LRRF	                8
#define STDOUT	                9
#define LAST_OPTION		STDOUT

static char	*option_tbl [] =
	{
	  "",
	  "-help",
	  "d1", "debug1",
	  "d2", "debug2",
	  "l", "lexicon",
	  "ferf", "full_eps_rule_free",
	  "erf", "eps_rule_free",
	  "cf", "cycle_free",
	  "lrrf", "left_right_recursive_free",
	};
static SXINT	option_kind [] =
	{
	  UNKNOWN_ARG,
	  HELP,
	  DEBUG1, DEBUG1,
	  DEBUG2, DEBUG2,
	  LEXICON, LEXICON,
	  FERF, FERF,
	  ERF, ERF,
	  CF, CF,
	  LRRF, LRRF,
	};



static SXINT
option_get_kind (char *arg)
{
    char	**opt;

    if (*arg++ != '-')
	return STDOUT;

    for (opt = &(option_tbl [OPT_NB]); opt > option_tbl; opt--) {
	if (strcmp (*opt, arg) == 0 /* egalite */ )
	    break;
    }

    return option_kind [opt - option_tbl];
}

/* *********************************************************************************** */
static XH_header  prod_hd;
static SXINT        *item2prod;
static XxY_header lhsntXprod_hd;
static XxY_header xnt_hd;
static SXBA       full_eps_set, NULLABLE, VIDE;
static bool    is_epsilon_free, bnf_has_changed, is_new_axiom_needed;

static char bnf_header [] = "\n\
*   ********************************************************************\n\
*\tThis grammar in BNF form has been generated:\n\
*\tfrom \"%s.bt\"\n\
*\t%s\n\
*\tby the SYNTAX(*) check_bnf processor\n\
*   ********************************************************************\n\
*\t(*) SYNTAX is a trademark of INRIA.\n\
*   ********************************************************************\n\n\n";

static void
out_header (char *format, char *out_header_language_name, char *kind_str)
{
    printf (format, out_header_language_name, kind_str);
}


/* *********************************************************************************** */


static char*
get_nt_string (SXINT A)
{
  return &bnf_ag.NT_STRING [bnf_ag.ADR [A]];
}

static char*
get_t_string (SXINT t)
{
  return &bnf_ag.T_STRING [bnf_ag.ADR [t]];
}

static void
print_old_prod (SXINT prod, SXINT A)
{
  SXINT item, X;

  printf ("%ld: %s = ", (SXINT) prod, get_nt_string (A));

  item = bnf_ag.WS_NBPRO [prod].prolon;
  
  while ((X = bnf_ag.WS_INDPRO [item++].lispro) != 0) {
    if (X > 0)
      printf ("%s ", get_nt_string (X));
    else
      printf ("%s ", get_t_string (X));
  }
      
  fputs (";\n", stdout);
}

static void
print_xnt (char *prefix, char *suffix, SXINT xnt)
{
  SXINT  X, Y, Z;
  char *p, *ptop;

  printf ("%s", prefix);

  xnt -= NTMAX;
  X = XxY_X (xnt_hd, xnt);
  Y = XxY_Y (xnt_hd, xnt);

  if (X > NTMAX) {
    /* ESSAI !! */
    fputs ("<", stdout);

    switch (Y) {
    case -1: /* <+A> */
      fputs ("+", stdout);
      Y = 0;
      break;

    case 0: /* <-A> */
      fputs ("-", stdout);
      break;

    default: /* <A'> */
      break;
    }

    do {
      X -= NTMAX;
      Z = XxY_Y (xnt_hd, X);
      X = XxY_X (xnt_hd, X);

      switch (Z) {
      case -1: /* <+A> */
	fputs ("+", stdout);
	break;

      case 0: /* <-A> */
	fputs ("-", stdout);
	break;

      default: /* <A'> */
	Y += Z;
	break;
      }
    } while (X > NTMAX);

    p = get_nt_string (X); /* Le < */
    ptop = p + strlen (p) - 1; /* Le > */
    *ptop = SXNUL;
    printf ("%s", p+1);
    *ptop = '>';

    while (Y-- > 0) {
      fputs ("'", stdout);
    }

    fputs (">", stdout);
  }
  else {
    p = get_nt_string (X); /* Le < */

    switch (Y) {
    case -1: /* <+A> */
      printf ("<+%s", p+1);
      break;

    case 0: /* <-A> */
      printf ("<-%s", p+1);
      break;

    default: /* <A'> */
      ptop = p + strlen (p) - 1; /* Le > */
      *ptop = SXNUL;
      printf ("%s", p);
      *ptop = '>';

      while (Y-- > 0) {
	fputs ("'", stdout);
      }

      fputs (">", stdout);
      break;
    }
  }
    
  printf ("%s", suffix);
}

static void
print_prod (SXINT prod, SXINT *pleft, SXINT *pright, SXINT *pdot)
{
  SXINT A, X;

  A = *pleft++;
  
  if (prod)
    printf ("%ld: ", (SXINT) prod);
      
  if (A <= NTMAX)
    printf ("%s = ", get_nt_string (A));
  else
    print_xnt ("", " = ", A);

  while (pleft < pright) {
    if (pleft == pdot)
      fputs (".", stdout);

    X = *pleft++;

    if (X > 0) {
      if (X <= NTMAX)
	printf ("%s ", get_nt_string (X));
      else
	print_xnt ("", " ", X);
    }
    else
      printf ("%s ", get_t_string (X));
  }
    
  if (pright == pdot)
    fputs (".", stdout);
      
  fputs (";\n", stdout);
}

/* ************************************************************************************************* */  
/* La grammaire n'est pas epsilon-free, on regarde s'il existe des nt qui ne derivent que ds le vide */
static bool
fill_full_eps_set (void)
{
  SXINT A, bot, top, X, prod, item;
  bool done;

  /* ... ils doivent deja deriver ds le vide */
  sxba_copy (full_eps_set, bnf_ag.BVIDE);

  /* On commence par chercher les prod qui contiennnent des terminaux */
  A = 0;

  while ((A = sxba_scan (full_eps_set, A)) > 0) {
    bot = NPG (A);
    top = NPG (A+1)-1;

    while (bot <= top) {
      prod = bnf_ag.WS_NBPRO [bot].numpg;
      item = bnf_ag.WS_NBPRO [prod].prolon;

      while ((X = bnf_ag.WS_INDPRO [item++].lispro) > 0);

      if (X < 0) {
	/* prod contient un terminal, A ne derive donc pas que ds le vide */
	SXBA_0_bit (full_eps_set, A);
	break;
      }

      bot++;
    }
  }

  done = false;

  while (!done) {
    done = true;

    A = 0;

    while ((A = sxba_scan (full_eps_set, A)) > 0) {
      bot = NPG (A);
      top = NPG (A+1)-1;

      while (bot <= top) {
	prod = bnf_ag.WS_NBPRO [bot].numpg; /* prod ne contient que des nt */
	item = bnf_ag.WS_NBPRO [prod].prolon;

	while ((X = bnf_ag.WS_INDPRO [item++].lispro) > 0) {
	  if (!SXBA_bit_is_set (full_eps_set, X))
	    /* A ne derive pas que dans le vide */
	    break;
	}

	if (X > 0) {
	  SXBA_0_bit (full_eps_set, A);
	  done = false; /* ... faudra boucler */
	  break;
	}

	bot++;
      }
    }
  }
    
  A = sxba_scan (full_eps_set, 0);

  if (is_debug1) {
    if (A > 0) {
      printf ("*The following non-terminal symbol%s only defined the empty string\n", sxba_scan (full_eps_set, A) > 0 ? "s" : "");
      printf ("*%s", get_nt_string (A));

      while ((A = sxba_scan (full_eps_set, A)) > 0) {
	printf (" %s", get_nt_string (A));
      }

      fputs ("\n", stdout);
      A = 1;
    }
  }

  return A > 0;
}

void
prod_hd_free (void)
{
  XH_free (&prod_hd);
  if (item2prod) sxfree (item2prod), item2prod = NULL;
  if (NULLABLE) sxfree (NULLABLE), NULLABLE = NULL;
  if (VIDE) sxfree (VIDE), VIDE = NULL;
  if (XxY_is_allocated (lhsntXprod_hd)) XxY_free (&lhsntXprod_hd);
  if (XxY_is_allocated (xnt_hd)) XxY_free (&xnt_hd);
}


/* Cette procedure remplit les structures associees a prod_hd qui permettent d'acceder a la grammaire */
static void
complete_prod_hd (void)
{
  SXINT A, prod, bot_item, top_item, X, item, ntXprod, top_prod;
  bool is_nullable;

  if (item2prod == NULL) {
    item2prod = (SXINT*) sxalloc (XH_list_size (prod_hd)+1, sizeof (SXINT));
    NULLABLE = sxba_calloc (XH_list_size (prod_hd)+1);
    XxY_alloc (&lhsntXprod_hd, "lhsntXprod_hd", 2*NBPRO+1, 1, 1, 0, (sxoflw0_t) NULL, NULL);
    XxY_alloc (&xnt_hd, "xnt_hd", 2*NTMAX+1, 1, 0, 0, (sxoflw0_t) NULL, NULL);
  }
  else {
    item2prod = (SXINT*) sxrealloc (item2prod, XH_list_size (prod_hd)+1, sizeof (SXINT));
    NULLABLE = sxba_resize (NULLABLE, XH_list_size (prod_hd)+1);
    XxY_clear (&lhsntXprod_hd);
  }
  
  top_prod = XH_top (prod_hd);
  prod = 0;

  while (++prod < top_prod) {
    A = REDUC (prod_hd, prod);
    bot_item = PROLON (prod_hd, prod);
    item = top_item = TOP_ITEM (prod_hd, prod);	

    is_nullable = true;

    while (--item >= bot_item) {
      item2prod [item] = prod;

      if (is_nullable) {
	X = LISPRO (prod_hd, item);

	if (X > 0 && SXBA_bit_is_set (VIDE, X))
	  SXBA_1_bit (NULLABLE, item);
	else
	  is_nullable = false;
      }
    }

    item2prod [item] = prod; /* La lhs !! */
    XxY_set (&lhsntXprod_hd, A, prod, &ntXprod);
  }
}


/* Cette procedure passe de la forme lispro a la forme XH ... */
/* ... en tenant compte de full_eps_set */
static void
fill_prod_hd (void)
{
  SXINT     A, bot, top, prod, item, X, prod_no;
  bool has_changed;

  if (is_debug2)
    fputs ("\n*Initialisation\n", stdout);

  XH_alloc (&prod_hd, "prod_hd", 2*NBPRO+1, 1, (INDPRO/NBPRO)+1, (sxoflw0_t) NULL, NULL);

  for (A = 1; A <= NTMAX; A++) {
    if (!SXBA_bit_is_set (full_eps_set, A)) {
      bot = NPG (A);
      top = NPG (A+1)-1;

      while (bot <= top) {
	prod = bnf_ag.WS_NBPRO [bot].numpg;
	XH_push (prod_hd, A);
	item = bnf_ag.WS_NBPRO [prod].prolon;
	has_changed = false;

	while ((X = bnf_ag.WS_INDPRO [item++].lispro) != 0) {
	  if (X < 0 || !SXBA_bit_is_set (full_eps_set, X))
	    XH_push (prod_hd, X);
	  else
	    has_changed = true;
	}
	  
	if (XH_set (&prod_hd, &prod_no)) {
	  /* bnf_ag est un ensemble ... */
	  /* ...mais du fait de full_eps_set, certaines prod deviennent egales */
	  if (is_debug2) {
	    fputs ("*Changed ", stdout);
	    print_old_prod (prod, A);
	    fputs ("*into duplicate of: ", stdout);
	    print_prod (prod_no, &(REDUC (prod_hd, prod_no)), &(REDUC (prod_hd, prod_no+1)), (SXINT*)NULL);
	  }
	}
	else {
	  if (has_changed && is_debug2) {
	    fputs ("*Changed ", stdout);
	    print_old_prod (prod, A);
	    printf ("*into %ld: ",  (SXINT) prod_no);
	    print_prod (prod_no, &(REDUC (prod_hd, prod_no)), &(REDUC (prod_hd, prod_no+1)), (SXINT*)NULL);
	  }
	}

	bot++;
      }
    }
    else {
      if (is_full_eps_rule_free)
	bnf_has_changed = true;

      if (is_debug2) {
	bot = NPG (A);
	top = NPG (A+1);

	while (bot < top) {
	  prod = bnf_ag.WS_NBPRO [bot].numpg;
	  fputs ("*Erased ", stdout);
	  print_old_prod (prod, A);
	  bot++;
	}
      }
      else {
	if (is_debug1) {
	  printf ("*The %s-language only contains the empty string.\n", get_nt_string (A));
	}
      }
    }
  }

  VIDE = sxba_calloc (NTMAX+1);
  sxba_copy (VIDE, bnf_ag.BVIDE);
  sxba_minus (VIDE, full_eps_set);
  is_epsilon_free = (sxba_scan (VIDE, 0) == -1);

  complete_prod_hd ();

  if (is_debug2)
    fputs ("*Initialisation (end)\n", stdout);
}

/* ************************************************************************************************* */


/* *************************************************************************************************
                         E P S I L O N   R U L E S   E L I M I N A T I O N
   ************************************************************************************************* */

/* Tous les nt de empty_set derivent ds le vide */
/* On transforme la grammaire de fac,on telle que ces nt ne derivent plus ds le vide */
static void
eps_rule_elimination (SXBA empty_set)
{
  SXINT       A, x, prod, bot_item, item, top_item, X, nb, val, pos, prev_top, prod_no;
  XH_header new_prod_hd;

  if (is_debug2) {
    fputs ("*Epsilon Rules Elimination for:\n* ", stdout);

    A = 0;

    while ((A = sxba_scan (empty_set, A)) > 0)
      printf ("%s ", get_nt_string (A));

    fputs ("\n", stdout);
  }

  XH_alloc (&new_prod_hd, "prod_hd", 2*XH_top (prod_hd)+1, 1, (INDPRO/NBPRO)+1, (sxoflw0_t) NULL, NULL);

  for (A = 1; A <= XNTMAX; A++) {
    foreach_NPG (A, x) {
      prod = NUMPG (x);
      bot_item = PROLON (prod_hd, prod);
      top_item = TOP_ITEM (prod_hd, prod);

      if (bot_item == top_item) {
	/* A -> \var_epsilon */
	if (!SXBA_bit_is_set (empty_set, A)) {
	  /* on recopie */
	  if (is_debug2) {
	    fputs ("*Copy ", stdout);
	    print_prod (prod, &(REDUC (prod_hd, prod)), &(REDUC (prod_hd, prod+1)), (SXINT*)NULL);
	  }

	  XH_push (new_prod_hd, A);
	    
	  if (XH_set (&new_prod_hd, &prod_no)) {
	    /* Production deja generee */
	    if (is_debug2) {
	      fputs ("*Into duplicate of ", stdout);
	      print_prod (prod_no, &(REDUC (new_prod_hd, prod_no)), &(REDUC (new_prod_hd, prod_no+1)), (SXINT*)NULL);
	    }
	  }
	  else {
	    if (is_debug2) {
	      fputs ("*Into ", stdout);
	      print_prod (prod_no, &(REDUC (new_prod_hd, prod_no)), &(REDUC (new_prod_hd, prod_no+1)), (SXINT*)NULL);
	    }
	  }
	}
	else {
	  if (is_debug2) {
	    fputs ("*Erased ", stdout);
	    print_prod (prod, &(REDUC (prod_hd, prod)), &(REDUC (prod_hd, prod+1)), (SXINT*)NULL);
	  }
	}
      }
      else {
	/* On cherche le nb de nt de la rhs qui appartiennent a empty_set */
	nb = 0;
	item = bot_item;

	while (item < top_item) {
	  X = LISPRO (prod_hd, item++);

	  if (X > 0 && SXBA_bit_is_set (empty_set, X))
	    nb++;
	}

	val = 1 << nb;

	if (is_debug2) {
	  printf ("*%s ", val == 1 ? "Copy" : "Change");
	  print_prod (prod, &(REDUC (prod_hd, prod)), &(REDUC (prod_hd, prod+1)), (SXINT*)NULL);
	}

	while (--val >= 0) {
	  pos = 1;
	  item = bot_item;

	  XH_push (new_prod_hd, A);
	  prev_top = XH_list_top (new_prod_hd);

	  while (item < top_item) {
	    X = LISPRO (prod_hd, item++);

	    if (X < 0) {
	      XH_push (new_prod_hd, X);
	    }
	    else {
	      if (SXBA_bit_is_set (empty_set, X)) {
		if (val & pos) {
		  XH_push (new_prod_hd, X);
		}

		pos <<= 1;
	      }
	      else {
		XH_push (new_prod_hd, X);
	      }
	    }
	  }
	    
	  if (XH_list_top (new_prod_hd) == prev_top /* recopie en une prod vide ... */
	      && SXBA_bit_is_set (empty_set, A) /* ... qui doit disparaitre */) {
	    XH_pop (new_prod_hd, X); /* le A */
	  }
	  else {
	    if (XH_set (&new_prod_hd, &prod_no)) {
	      /* Production deja generee */
	      if (is_debug2) {
		fputs ("*Into duplicate of ", stdout);
		print_prod (prod_no, &(REDUC (new_prod_hd, prod_no)), &(REDUC (new_prod_hd, prod_no+1)), (SXINT*)NULL);
	      }
	    }
	    else {
	      if (is_debug2) {
		fputs ("*Into ", stdout);
		print_prod (prod_no, &(REDUC (new_prod_hd, prod_no)), &(REDUC (new_prod_hd, prod_no+1)), (SXINT*)NULL);
	      }
	    }
	  }
	}
      }
    }
  }
  
  XH_free (&prod_hd);
  prod_hd = new_prod_hd;

  sxba_minus (VIDE, empty_set);
  is_epsilon_free = (sxba_scan (VIDE, 0) == -1);

  complete_prod_hd ();

  if (is_debug2)
    fputs ("*Epsilon Rules Elimination (end)\n", stdout);
}
 
/* *************************************************************************************************
                         E P S I L O N   R U L E S   E L I M I N A T I O N (end)
   ************************************************************************************************* */


/* *************************************************************************************************
                                  C Y C L E   E L I M I N A T I O N
   ************************************************************************************************* */

/* Cette procedure transforme la grammaire cyclique prod_hd en une grammaire equivalente sans cycle */
/* repr_set contient les representants (nt) des classes d'equivalence de la relation cycle */
/* Si A est un representant, la classe d'equivalence de representant A est ds cyclic_sets [A] */
static bool            is_cyclic;

static void
cycle_elimination (SXBA repr_set, SXBA *cyclic_sets)
{
  SXINT           A, B, prod, item, X, mark, prod_no, x, bot_item, top_item, eqA; 
  SXINT           *nt2equiv, *empty_stack;
  SXBA          line, empty_set;
  XH_header     new_prod_hd;

  nt2equiv = (SXINT*) sxcalloc (NTMAX+1, sizeof (SXINT));

  A = 0;

  while ((A = sxba_scan (repr_set, A)) > 0) {
    nt2equiv [A] = A;

    line = cyclic_sets [A];

    B = A;

    while ((B = sxba_scan (line, B)) > 0) {
      nt2equiv [B] = A;
    }
  }

  for (A = 1; A <= NTMAX; A++) {
    if (nt2equiv [A] == 0)
      nt2equiv [A] = A;
  }

  empty_set = sxba_calloc (NTMAX+1);

  if (!is_epsilon_free) {
    /* Il y a des productions vides ... */
    /* Traitement des regles de la forme A -> \alpha A \beta, \alpha \beta =>+ \var_epsilon
       ou A est le representant d'un cycle */
    empty_stack = (SXINT*) sxalloc (NTMAX+1, sizeof (SXINT));
    empty_stack [0] = 0;

    A = 0;

    while ((A = sxba_scan (repr_set, A)) > 0) {
      line = cyclic_sets [A];

      B = A-1;

      while ((B = sxba_scan (line, B)) > 0) {
	foreach_NPG (B, x) {
	  prod = NUMPG (x); /* prod est une B-prod */
	  bot_item = item = PROLON (prod_hd, prod);
	  top_item = TOP_ITEM (prod_hd, prod);

	  while (item < top_item && (X = LISPRO (prod_hd, item++)) > 0) {
	    if (SXBA_bit_is_set (line, X)) {
	      if (item == top_item || SXBA_bit_is_set (NULLABLE, item)) {
		/* C'est bon */
		mark = item-1;

		while (item < top_item) {
		  X = LISPRO (prod_hd, item++);

		  if (SXBA_bit_is_reset_set (empty_set, X))
		    PUSH (empty_stack, X);
		}
		
		item = mark;

		while (--item >= bot_item) {
		  X = LISPRO (prod_hd, item);

		  if (SXBA_bit_is_reset_set (empty_set, X))
		    PUSH (empty_stack, X);
		}
	      }

	      break;
	    }
	    else {
	      if (!SXBA_bit_is_set (VIDE, X))
		break;
	    }
	  }
	}
      }
    }

    /* Ici, empty_set contient tous les nt qui derivent ds le vide impliques directement
       ds les \alpha \beta de A -> \alpha A \beta, \alpha \beta =>+ \var_epsilon
       ou A est le representant d'un cycle */

    /* On complete cet ensemble en ajoutant iterativement tous les Yi t.q.
       X -> Y1 ... Yp, X \in empty_set et Y1 ... Yp =>+ \var_epsilon */

    while (!IS_EMPTY (empty_stack)) {
      A = POP (empty_stack);

      foreach_NPG (A, x) {
	prod = NUMPG (x);
	item = PROLON (prod_hd, prod);
	top_item = TOP_ITEM (prod_hd, prod);

	while (item < top_item && (X = LISPRO (prod_hd, item++)) > 0) {
	  if (item == top_item || SXBA_bit_is_set (NULLABLE, item)) {
	    /* C'est un bon X */
	    do {
	      if (SXBA_bit_is_reset_set (empty_set, X))
		PUSH (empty_stack, X);

	      X = LISPRO (prod_hd, item++);
	    } while (item < top_item);
	  }
	}
      }
    }

    sxfree (empty_stack);
    /* Ici, empty_set contient tous les nt qui derivent ds le vide dont il faut s'occuper */

    if (!sxba_is_empty (empty_set))
      eps_rule_elimination (empty_set);
  }

  /* On sort la nouvelle grammaire ... */
  /* ... en tenant compte des resultats precedents */

  if (is_debug2) {
    fputs ("\n*Cyclic Rules Elimination for:\n", stdout);

    A = 0;

    while ((A = sxba_scan (repr_set, A)) > 0) {
      fputs ("* ", stdout);
      line = cyclic_sets [A];
      B = 0;

      while ((B = sxba_scan (repr_set, B)) > 0)
	printf ("%s ", get_nt_string (B));

      fputs ("\n", stdout);
    }
  }

  /* On ne les met pas ds new_prod_hd !! */
  if (SXBA_bit_is_set (empty_set, 1)) {
    /* L'axiome ne derive plus ds le vide ... */
    /* Le langage contient la chaine vide et autre chose ... */
    /* On genere :
       <new_axiom> = <axiom> ;
       <new_axiom> = ;
    */
    is_new_axiom_needed = true;
    printf ("<new_%s = %s ;\n", get_nt_string ((SXINT)1)+1, get_nt_string ((SXINT)1));
    printf ("<new_%s = ;\n", get_nt_string ((SXINT)1)+1);
  }

  XH_alloc (&new_prod_hd, "prod_hd", 2*XH_top (prod_hd)+1, 1, XH_list_top (prod_hd)/XH_top (prod_hd)+1, (sxoflw0_t) NULL, NULL);

  for (A = 1; A <= NTMAX; A++) { 
    foreach_NPG (A, x) {
      prod = NUMPG (x);
      bot_item = item = PROLON (prod_hd, prod);
      top_item = TOP_ITEM (prod_hd, prod);
      eqA = nt2equiv [A];

      if (top_item-bot_item == 1
	  && (X = LISPRO (prod_hd, bot_item)) > 0
	  && nt2equiv [X] == eqA) {
	/* <A> = <A> ; */
	/* supprimee */
	if (is_debug2) {
	  fputs ("*Erased ", stdout);
	  print_prod (prod, &(REDUC (prod_hd, prod)), &(REDUC (prod_hd, prod+1)), (SXINT*)NULL);
	}
      }
      else {
	XH_push (new_prod_hd, eqA);
	item = bot_item;
	
	while (item < top_item) {
	  X = LISPRO (prod_hd, item++);

	  if (X < 0)
	    XH_push (new_prod_hd, X);
	  else {
	    XH_push (new_prod_hd, nt2equiv [X]);
	  }
	}

	if (is_debug2) {
	  fputs ("*Change ", stdout);
	  print_prod (prod, &(REDUC (prod_hd, prod)), &(REDUC (prod_hd, prod+1)), (SXINT*)NULL);
	}

	if (XH_set (&new_prod_hd, &prod_no)) {
	  /* Production deja generee */
	  if (is_debug2) {
	    fputs ("*Into a duplicate of ", stdout);
	    print_prod (prod_no, &(REDUC (new_prod_hd, prod_no)), &(REDUC (new_prod_hd, prod_no+1)), (SXINT*)NULL);
	  }
	}
	else {
	  if (is_debug2) {
	    fputs ("*Into ", stdout);
	    print_prod (prod_no, &(REDUC (new_prod_hd, prod_no)), &(REDUC (new_prod_hd, prod_no+1)), (SXINT*)NULL);
	  }
	}
      }
    }
  }

  sxfree (empty_set);
  sxfree (full_eps_set);
  sxfree (nt2equiv);
  
  XH_free (&prod_hd);
  prod_hd = new_prod_hd;

  complete_prod_hd ();

  if (is_debug2)
    fputs ("*Cyclic Rules Elimination (end)\n", stdout);
}

/* On regarde si la grammaire est cyclique */
static bool
cycle_check (void)
{
  SXINT                prod, A, item, X, B, x, bot_item, top_item; 
  SXBA               line;
  SXBA               *cyclic_set, repr_set, cyclic_nt_set;

  cyclic_set = sxbm_calloc (NTMAX+1, NTMAX+1);
  repr_set = sxba_calloc (NTMAX+1);

  is_cyclic = false;

  for (A = 1; A <= NTMAX; A++) {
    line = cyclic_set [A];

    foreach_NPG (A, x) {
      prod = NUMPG (x); /* prod est une A-prod */
      /* On regarde si prod a la forme:
	 A -> \alpha X \beta, \alpha\beta =>* \varepsilon, X \in N */
      item = PROLON (prod_hd, prod);
      top_item = TOP_ITEM (prod_hd, prod);

      while (item <  top_item && (X = LISPRO (prod_hd, item++)) > 0) {
	if (item == top_item || SXBA_bit_is_set (NULLABLE, item) /* \beta =>* \varepsilon */) {
	  /* ...oui */
	  SXBA_1_bit (line, X);
	}

	if (!SXBA_bit_is_set (bnf_ag.BVIDE, X)) {
	  /* \alpha \not=>* varepsilon */
	  break;
	}
      }
    }
  }

  fermer (cyclic_set, NTMAX+1);
  cyclic_nt_set = cyclic_set [0];
    
  A = 0;

  while ((A = sxba_0_lrscan (cyclic_nt_set, A)) > 0) {
    line = cyclic_set [A];

    if (SXBA_bit_is_set (line, A)) {
      /* cycle autour de A ... */
      /* ...et c'est le representant du cycle */
      SXBA_1_bit (repr_set, A);
      is_cyclic = true;

      if (is_debug1)
	fputs ("\n* Cycles around: ", stdout);

      B = 0;

      while ((B = sxba_scan (line, B)) > 0) {
	/* A =>+ B ... */
	if (SXBA_bit_is_set (cyclic_set [B], A)) {
	  /* ... B =>+ A */
	  if (is_debug1)
	    printf ("%s ", get_nt_string (B));

	  SXBA_1_bit (cyclic_nt_set, B);
	}
	else
	  /* pas ds le cycle */
	  SXBA_0_bit (line, B);
      }

      if (is_debug1)
	fputs ("\n", stdout);

      B = 0;

      while ((B = sxba_scan (line, B)) > 0) {
	foreach_NPG (B, x) {
	  prod = NUMPG (x); /* prod est une B-prod */
	  /* On regarde si prod a la forme:
	     B -> \alpha X \beta, \alpha\beta =>* \varepsilon, X \in LRR */
	  item = bot_item = PROLON (prod_hd, prod);
	  top_item = TOP_ITEM (prod_hd, prod);

	  while (item < top_item && (X = LISPRO (prod_hd, item++)) > 0) {
	    if (SXBA_bit_is_set (line, X) /* X est ds le cycle */
		&& (item == top_item || SXBA_bit_is_set (NULLABLE, item) /* \beta =>* \varepsilon */)
		) {
	      if (is_debug1) {
		fputs ("* ", stdout);
		print_prod (prod, &(LISPRO (prod_hd, bot_item-1)), &(LISPRO (prod_hd, top_item)), &(LISPRO (prod_hd, item-1)));
	      }
	    }

	    if (!SXBA_bit_is_set (bnf_ag.BVIDE, X)) {
	      /* \alpha \not=>* varepsilon */
	      break;
	    }
	  }
	}
      }
    }
  } 

  if (is_cyclic) {
    if (is_debug1) {
      fputs ("\n", stdout);
    }

    if (is_cycle_free) {
      bnf_has_changed = true;
      cycle_elimination (repr_set, cyclic_set);
    }
  }

  sxbm_free (cyclic_set), cyclic_set = NULL;
  sxfree (repr_set), repr_set = NULL;

  return is_cyclic;
}

/* *************************************************************************************************
                                  C Y C L E   E L I M I N A T I O N  (end)
   ************************************************************************************************* */



/* *************************************************************************************************
                          L E F T   R E C U S I O N   E L I M I N A T I O N
   ************************************************************************************************* */
static XH_header  lr_prod_hd;
static XxY_header lrA2prod_hd;
static SXBA       ilr_nt_set, non_lr_nt_set;
static SXINT        *Aprimes;

/* lr_nt_set contient les nt mutuellement recursifs gauche et
   rr_nt_set contient les nt mutuellement recursifs droit */
/* ATTENTION, on n'ajoute pas de A' a droite d'un nt B qui appartient a lr_nt_set U rr_nt_set!!
   il semble que c,a n'ajoute rien au langage et que bien su^r c,a augmente les ambiguite's */
static void
left_recursion_elimination (SXBA lr_nt_set /*, rr_nt_set */)
{
  SXINT A, x, prod, bot_item, top_item, item, X, prod_no, A2prod, Aprime, Aprod, B, Bprod, B2prod, repr, minusA, plusA;
  SXINT Abot_item, Atop_item, Aitem, dum, leftmostA; 

  /* On commence par remplir lr_prod_hd, lrA2prod_hd et ilr_nt_set */
  /* De plus on regarde s'il y a des A-rules non recursives gauches (de la forme A -> X \alpha avec X \in lr_nt_set) */
  if (is_debug2)
    fputs ("\n*left recursion elimination: copy\n", stdout);

  repr = sxba_scan (lr_nt_set, 0);

  sxba_empty (ilr_nt_set);
  sxba_empty (non_lr_nt_set);
  A = 0;

  while ((A = sxba_scan (lr_nt_set, A)) > 0) {
    foreach_NPG (A, x) {
      XH_push (lr_prod_hd, A);
      prod = NUMPG (x); /* prod est une A_prod ... */

      if (is_debug2) {
	fputs ("*Copy ", stdout);
	print_prod (prod, &(REDUC (prod_hd, prod)), &(REDUC (prod_hd, prod+1)), (SXINT*)NULL);
      }

      bot_item = item = PROLON (prod_hd, prod);
      top_item = TOP_ITEM (prod_hd, prod);

      if (bot_item == top_item /* Normalement tj faux, A ne derivant plus/pas ds le vide!! */
	  || ((X = LISPRO (prod_hd, bot_item)) > 0 && !SXBA_bit_is_set (lr_nt_set, X))) {
	/* prod est non recursive gauche */
	SXBA_1_bit (non_lr_nt_set, A);
      }

      while (item < top_item) {
	X = LISPRO (prod_hd, item);
	XH_push (lr_prod_hd, X);

	if (X == A && item == bot_item)
	  SXBA_1_bit (ilr_nt_set, A);

	item++;
      }

      if (!XH_set (&lr_prod_hd, &prod_no)) {
	if (is_debug2) {
	  fputs ("*Into ", stdout);
	  print_prod (prod_no, &(REDUC (lr_prod_hd, prod_no)), &(REDUC (lr_prod_hd, prod_no+1)), (SXINT*)NULL);
	}
	
	XxY_set (&lrA2prod_hd, A, prod_no, &A2prod);
      }
      else {
	if (is_debug2) {
	  printf ("*Into a duplicate of prod #%ld:  ", (SXINT) prod_no);
	  print_prod (prod_no, &(REDUC (lr_prod_hd, prod_no)), &(REDUC (lr_prod_hd, prod_no+1)), (SXINT*)NULL);
	}
      }
    }
  }

  if (is_debug2)
    fputs ("*left recursion elimination: end copy\n", stdout);

  A = XNTMAX+1;

  while ((A = sxba_1_rlscan (lr_nt_set, A)) > 0) {
    /* Ici A est recursif gauche */
    if (A != repr /* ce n'est pas le representant de la classe d'equivalence */
	&& SXBA_bit_is_set (non_lr_nt_set, A) /* ...et il y a des A_regles non recursives gauches */) {
      /* On va donc partitionner les A_regles en 2, les recursives gauches (+A) et les non-recursives gauches (-A) */
      /* On commence par generer <A> -> <-A> ; et <A> -> <+A> ; */
      XH_push (lr_prod_hd, A);
      XxY_set (&xnt_hd, A, 0, &minusA);
      minusA += NTMAX;
      XH_push (lr_prod_hd, minusA);
      XH_set (&lr_prod_hd, &prod_no);
	  
      if (is_debug2) {
	fputs ("*Partition into ... ", stdout);
	print_prod (prod_no, &(REDUC (lr_prod_hd, prod_no)), &(REDUC (lr_prod_hd, prod_no+1)), (SXINT*)NULL);
      }

      XH_push (lr_prod_hd, A);
      XxY_set (&xnt_hd, A, -1, &plusA);
      plusA += NTMAX;
      XH_push (lr_prod_hd, plusA);
      XH_set (&lr_prod_hd, &prod_no);
	  
      if (is_debug2) {
	fputs ("*...and ", stdout);
	print_prod (prod_no, &(REDUC (lr_prod_hd, prod_no)), &(REDUC (lr_prod_hd, prod_no+1)), (SXINT*)NULL);
      }
    }
    else {
      minusA = plusA = 0;
    }

    if (plusA != 0 || SXBA_bit_is_set (ilr_nt_set, A)) {
      /* A est partitionne' ... */
      /* ...ou A est recursif gauche immediat */
      if (SXBA_bit_is_set (ilr_nt_set, A)) {
	/* On genere <A'> -> ; */
	if (is_debug2)
	  fputs ("*Immediate left recursion elimination\n", stdout);

	XxY_set (&xnt_hd, A, ++Aprimes [A], &Aprime);
	Aprime += NTMAX;

	SXBA_1_bit (VIDE, Aprime); /* la taille de VIDE est correcte */

	XH_push (lr_prod_hd, Aprime);
	XH_set (&lr_prod_hd, &prod_no);
	/* Il faudra la retrouver ... */
	XxY_set (&lrA2prod_hd, Aprime, prod_no, &dum);
	  
	if (is_debug2) {
	  fputs ("*Create epsilon rule ", stdout);
	  print_prod (prod_no, &(REDUC (lr_prod_hd, prod_no)), &(REDUC (lr_prod_hd, prod_no+1)), (SXINT*)NULL);
	}
      }
      else
	Aprime = 0;

      XxY_Xforeach (lrA2prod_hd, A, A2prod) {
	Aprod = XxY_Y (lrA2prod_hd, A2prod);

	if (is_debug2) {
	  fputs ("*Change ", stdout);
	  print_prod (Aprod, &(REDUC (lr_prod_hd, Aprod)), &(REDUC (lr_prod_hd, Aprod+1)), (SXINT*)NULL);
	}

	bot_item = XH_X (lr_prod_hd, Aprod)+1;
	top_item = XH_X (lr_prod_hd, Aprod+1);

	if (bot_item < top_item
	    && (X = LISPRO (lr_prod_hd, bot_item)) == A) {
	  /* ... recursion gauche immediate */
	  /* prod : <A> -> <A> \alpha */
	  /* On genere <A'> -> \alpha <A'> */
	  if (Aprime == 0)
	    sxtrap (ME, "left_recursion_elimination");

	  XH_push (lr_prod_hd, Aprime);
	  X = 0;

	  while (++bot_item < top_item) {
	    X = LISPRO (lr_prod_hd, bot_item);
	    XH_push (lr_prod_hd, X);
	  }

	  /* if ((X <= 0) || !(SXBA_bit_is_set (lr_nt_set, X) || SXBA_bit_is_set (rr_nt_set, X))) FAUX */
	  if ((X <= 0) || (X != A))
	      /* car L(A A') == L(A) */
	    XH_push (lr_prod_hd, Aprime);

	  if (!XH_set (&lr_prod_hd, &prod_no)) {
	    /* Il faudra les retrouver ... */
	    XxY_set (&lrA2prod_hd, Aprime, prod_no, &dum);
	  }
	}
	else {
	  /* prod : <A> -> X \alpha */
	  if (plusA != 0) {
	    if (bot_item == top_item /* Normalement tj faux, A ne derivant plus/pas ds le vide!! */
		|| ((X = LISPRO (prod_hd, bot_item)) > 0 && !SXBA_bit_is_set (lr_nt_set, X))) {
	      /* prod est non recursive gauche ... */
	      /* ...On genere <-A> -> X \alpha <A'> */
	      XH_push (lr_prod_hd, minusA);
	    }
	    else {
	      /* prod est recursive gauche ... */
	      /* ...On genere <+A> -> X \alpha <A'> */
	      XH_push (lr_prod_hd, plusA);
	    }
	  }
	  else {
	    /* On genere <A> -> X \alpha <A'> */
	    /* et on met a jour lrA2prod_hd */
	    XH_push (lr_prod_hd, A);
	  }

	  X = 0;

	  while (bot_item < top_item) {
	    X = LISPRO (lr_prod_hd, bot_item++);
	    XH_push (lr_prod_hd, X);
	  }

	  /* if ((X <= 0) || !(SXBA_bit_is_set (lr_nt_set, X) || SXBA_bit_is_set (rr_nt_set, X))) */
	  if ((X <= 0) || (X != A)) {
	    /* car L(A A') == L(A) */
	    if (Aprime == 0) {
	      /* ESSAI le 16/01/04 */
	      XxY_set (&xnt_hd, A, ++Aprimes [A], &Aprime);
	      Aprime += NTMAX;
	      XH_push (lr_prod_hd, Aprime);
	      Aprime = -Aprime;
	    }
	    else
	      XH_push (lr_prod_hd, Aprime);
	  }

	  if (!XH_set (&lr_prod_hd, &prod_no)) {
	    XxY_set (&lrA2prod_hd, A, prod_no, &dum);
	  }
	}
	
	if (is_debug2) {
	  fputs ("*Into ", stdout);
	  print_prod (prod_no, &(REDUC (lr_prod_hd, prod_no)), &(REDUC (lr_prod_hd, prod_no+1)), (SXINT*)NULL);
	}

	if (Aprod != prod_no) {
	  /* Aucun changement!! */
	  XH_erase (lr_prod_hd, Aprod);
	  XxY_erase (lrA2prod_hd, A2prod);
	}

	if (Aprime < 0) {
	  /* ESSAI le 16/01/04 */
	  /* On cree une prod vide!! */
	  Aprime = -Aprime;

	  SXBA_1_bit (VIDE, Aprime); /* la taille de VIDE est correcte */

	  XH_push (lr_prod_hd, Aprime);
	  XH_set (&lr_prod_hd, &prod_no);
	  /* Il faudra la retrouver ... */
	  XxY_set (&lrA2prod_hd, Aprime, prod_no, &dum);
	  
	  if (is_debug2) {
	    fputs ("*Create epsilon rule ", stdout);
	    print_prod (prod_no, &(REDUC (lr_prod_hd, prod_no)), &(REDUC (lr_prod_hd, prod_no+1)), (SXINT*)NULL);
	  }
	}
      }
    }

    /* ... et on etend les A des B_prod (B < A) comme suit : */
    /* Si B -> A \alpha et
       - plusA != 0, on genere B -> -A \alpha et B -> \beta \alpha pour tout +A -> \beta
       - plusA == 0, on genere B -> \beta \alpha pour tout A -> \beta
    */
       
    if (is_debug2) {
      if (sxba_1_rlscan (lr_nt_set, A) > 0)
	fputs ("*Expansion\n", stdout);
    }

    B = A;

    while ((B = sxba_1_rlscan (lr_nt_set, B)) > 0) {
      /* On commence par regarder si une B_prod commence par un A ... */
      XxY_Xforeach (lrA2prod_hd, B, B2prod) {
	Bprod = XxY_Y (lrA2prod_hd, B2prod);
	bot_item = XH_X (lr_prod_hd, Bprod)+1;
	top_item = XH_X (lr_prod_hd, Bprod+1);

	if (bot_item < top_item
	    && (X = LISPRO (lr_prod_hd, bot_item)) == A) {
	  /* ...oui */
	  break;
	}
      }

      if (B2prod > 0) {
	/* ...oui */
	/* On recopie donc les B_prod en etendant les A */
	XxY_Xforeach (lrA2prod_hd, B, B2prod) {
	  Bprod = XxY_Y (lrA2prod_hd, B2prod);
	  bot_item = XH_X (lr_prod_hd, Bprod)+1;
	  top_item = XH_X (lr_prod_hd, Bprod+1);

	  if (bot_item < top_item
	      && (X = LISPRO (lr_prod_hd, bot_item)) == A) {
	    /* Bprod commence par un A */
	    if (plusA != 0) {
	      /* on genere B -> -A \alpha */
	      if (is_debug2) {
		fputs ("*Expand ", stdout);
		print_prod (Bprod, &(REDUC (lr_prod_hd, Bprod)), &(REDUC (lr_prod_hd, Bprod+1)), (SXINT*)NULL);
	      }

	      XH_push (lr_prod_hd, B); /* lhs */
	      XH_push (lr_prod_hd, minusA);

	      for (item = bot_item+1; item < top_item; item++)
		XH_push (lr_prod_hd, LISPRO (lr_prod_hd, item));

	      if (!XH_set (&lr_prod_hd, &prod_no)) {
		XxY_set (&lrA2prod_hd, B, prod_no, &dum);

		if (is_debug2) {
		  fputs ("*Into ", stdout);
		  print_prod (prod_no, &(REDUC (lr_prod_hd, prod_no)), &(REDUC (lr_prod_hd, prod_no+1)), (SXINT*)NULL);
		}
	      }

	      leftmostA = plusA;
	    }
	    else
	      leftmostA = A;

	    XxY_Xforeach (lrA2prod_hd, leftmostA, A2prod) {
	      if (is_debug2) {
		fputs ("*Expand ", stdout);
		print_prod (Bprod, &(REDUC (lr_prod_hd, Bprod)), &(REDUC (lr_prod_hd, Bprod+1)), (SXINT*)NULL);
	      }
	  
	      Aprod = XxY_Y (lrA2prod_hd, A2prod);
	      Abot_item = Aitem = XH_X (lr_prod_hd, Aprod)+1;
	      Atop_item = XH_X (lr_prod_hd, Aprod+1);

	      XH_push (lr_prod_hd, B); /* lhs */

	      /* expansion de A */
	      while (Aitem < Atop_item) {
		X = LISPRO (lr_prod_hd, Aitem);

		if (X == B && Aitem == Abot_item)
		  /* B devient recursif gauche immediat */
		  SXBA_1_bit (ilr_nt_set, B);

		XH_push (lr_prod_hd, X);

		Aitem++;
	      }

	      /* suffixe */
	      item = bot_item;

	      while (++item < top_item) {
		X = LISPRO (lr_prod_hd, item);
		XH_push (lr_prod_hd, X);
	      }

	      if (!XH_set (&lr_prod_hd, &prod_no)) {
		XxY_set (&lrA2prod_hd, B, prod_no, &dum);

		if (is_debug2) {
		  fputs ("*Into ", stdout);
		  print_prod (prod_no, &(REDUC (lr_prod_hd, prod_no)), &(REDUC (lr_prod_hd, prod_no+1)), (SXINT*)NULL);
		}
	      }
	    }

	    XH_erase (lr_prod_hd, Bprod);
	    XxY_erase (lrA2prod_hd, B2prod);
	  }
	  /* else inchangee ... */
	}
      }
    }
  }

  if (is_debug2)
    fputs ("*left recursion elimination: done\n", stdout);
}


static bool
lr_recursive_check (void)
{
  SXINT                prod, A, X, nb, B, item, x, bot_item, top_item, Aprod, prod_no, prev_xnt, Aprime; 
  SXBA               line_LR, line_RR;
  bool            is_ambiguous = false; 
  SXBA               *LR, *RR, LRR, LRR2, item_set, tbp_empty_nt_set, line;
  SXINT                *tbp;
  XH_header          new_prod_hd;

  /* On commence par construire les relations LR et RR (Left_Recursive et Right-Recursive) */
  LR = sxbm_calloc (XNTMAX+1, XNTMAX+1);
  RR = sxbm_calloc (XNTMAX+1, XNTMAX+1);

  for (A = 1; A <= XNTMAX; A++) {
    line_LR = LR [A];
    line_RR = RR [A];

    foreach_NPG (A, x) {
      prod = NUMPG (x); /* prod est une A-prod */
      item = bot_item = PROLON (prod_hd, prod);
      top_item = TOP_ITEM (prod_hd, prod);

      while (item < top_item && (X = LISPRO (prod_hd, item++)) > 0) {
	SXBA_1_bit (line_LR, X);

	if (!SXBA_bit_is_set (VIDE, X))
	  break;
      }

      item = top_item;

      while (--item >= bot_item && (X = LISPRO (prod_hd, item)) > 0) {
	SXBA_1_bit (line_RR, X);

	if (!SXBA_bit_is_set (VIDE, X))
	  break;
      }
    }
  }

  fermer (LR, XNTMAX+1);
  fermer (RR, XNTMAX+1);

  LRR = sxba_calloc (XNTMAX+1);

  for (A = 1; A <= XNTMAX; A++) {
    if (SXBA_bit_is_set (LR [A], A) && SXBA_bit_is_set (RR [A], A)) {
      /* A est a la fois recursif gauche et droit */
      SXBA_1_bit (LRR, A);
      is_ambiguous = true;
    }
  }

  if (is_ambiguous) {
    nb = sxba_cardinal (LRR);

    if (is_debug1) {
      printf ("\n* This grammar is ambiguous, %ld nonterminal symbol%s both left and right recursive:\n* ",
	      (SXINT) nb, nb == 1 ? " is" : "s are");

      A = 0;

      while ((A = sxba_scan (LRR, A)) > 0) {
	printf (" %s", get_nt_string (A));
      }

      fputs ("\n", stdout);
    }

    item_set = sxba_calloc (XINDPRO+1);
    tbp = (SXINT*) sxalloc (XNTMAX+1, sizeof (SXINT));
    tbp [0] = 0;
    
    tbp_empty_nt_set = sxba_calloc (XNTMAX+1);

    /* On sort les recursivites droites ... */
    LRR2 = sxba_calloc (XNTMAX+1);
    sxba_copy (LRR2, LRR);

    if (is_debug1)
      fputs ("* Right recursive rules around:\n", stdout);

    A = 0;

    while ((A = sxba_scan (LRR2, A)) > 0) {
      line = RR [A];
      sxba_empty (line);
      SXBA_1_bit (line, A);
      PUSH (tbp, A);

      while (!IS_EMPTY (tbp)) {
	B = POP (tbp);
    
	foreach_NPG (B, x) {
	  prod = NUMPG (x); /* prod est une B-prod */

	  /* On regarde si prod est impliquee ds la recursivite gauche ... */
	  bot_item = PROLON (prod_hd, prod);
	  item = TOP_ITEM (prod_hd, prod);

	  while (bot_item <= --item && (X = LISPRO (prod_hd, item)) > 0) {
	    if (SXBA_bit_is_set (RR [X], A)) {
	      /* ...oui */
	      SXBA_1_bit (item_set, item);

	      if (SXBA_bit_is_reset_set (line, X))
		PUSH (tbp, X);
	    }

	    if (!SXBA_bit_is_set (VIDE, X))
	      break;
	  }
	}
      }

      if (is_debug1) {
	fputs ("* ", stdout);
	B = 0;

	while ((B = sxba_scan (line, B)) > 0) {
	  printf (" %s", get_nt_string (B));
	}

	fputs ("\n", stdout);

	item = 0;

	while ((item = sxba_scan (item_set, item)) > 0) {
	  fputs ("* ", stdout);
	  prod = PROLIS (item);
	  print_prod (prod, &(REDUC (prod_hd, prod)), &(REDUC (prod_hd, prod+1)), &(LISPRO (prod_hd, item+1)));
	}
      }

      B = 0;

      while ((B = sxba_scan (line, B)) > 0) {
	if (!SXBA_bit_is_set (RR [B], A))
	  /* B n'est pas ds le cycle de A */
	  SXBA_0_bit (line, B);
      }

      sxba_minus (LRR2, line);
      sxba_or (RR [0], line); /* cumul */
      SXBA_1_bit (LRR2, A); /* representant d'une classe de recursivites gauches */
	
      if (!is_epsilon_free) {
	/* On remplit tbp_empty_nt_set avec les nt derivant ds le vide qu'il va falloir traiter... */
	item = 0;

	while ((item = sxba_scan_reset (item_set, item)) > 0) {
	  prod = PROLIS (item);
	  top_item = TOP_ITEM (prod_hd, prod);

	  while (--top_item > item) {
	    X = LISPRO (prod_hd, top_item);
	    SXBA_1_bit (tbp_empty_nt_set, X);
	  }
	}
      }
      else
	sxba_empty (item_set);
    }

    if (!is_epsilon_free) {
      /* Prudence, on ajoute a tbp_empty_nt_set les nt recursifs droits */
      line = RR [0];
      A = 0;

      while ((A = sxba_scan (line, A)) > 0) {
	if (SXBA_bit_is_set (VIDE, A))
	  SXBA_1_bit (tbp_empty_nt_set, A);
      }
    }

    sxfree (LRR2), LRR2 = NULL;

    if (is_debug1)
      fputs ("* Left recursive rules around:\n", stdout);
    
    A = 0;

    while ((A = sxba_scan (LRR, A)) > 0) {
      line = LR [A];
      sxba_empty (line);
      SXBA_1_bit (line, A);
      PUSH (tbp, A);

      while (!IS_EMPTY (tbp)) {
	B = POP (tbp);
    
	foreach_NPG (B, x) {
	  prod = NUMPG (x); /* prod est une B-prod */

	  /* On regarde si prod est impliquee ds la recursivite gauche ... */
	  item = PROLON (prod_hd, prod);
	  top_item = TOP_ITEM (prod_hd, prod);

	  while (item < top_item && (X = LISPRO (prod_hd, item)) > 0) {
	    if (SXBA_bit_is_set (LR [X], A)) {
	      /* ...oui */
	      SXBA_1_bit (item_set, item);

	      if (SXBA_bit_is_reset_set (line, X))
		PUSH (tbp, X);
	    }

	    if (!SXBA_bit_is_set (VIDE, X))
	      break;

	    item++;
	  }
	}
      }

      if (is_debug1) {
	fputs ("* ", stdout);
	B = 0;

	while ((B = sxba_scan (line, B)) > 0) {
	  printf (" %s", get_nt_string (B));
	}

	fputs ("\n", stdout);

	item = 0;

	while ((item = sxba_scan (item_set, item)) > 0) {
	  fputs ("* ", stdout);
	  prod = PROLIS (item);
	  print_prod (prod, &(REDUC (prod_hd, prod)), &(REDUC (prod_hd, prod+1)), &(LISPRO (prod_hd, item)));
	}
      }

      B = 0;

      while ((B = sxba_scan (line, B)) > 0) {
	if (!SXBA_bit_is_set (LR [B], A))
	  /* B n'est pas ds le cycle de A */
	  SXBA_0_bit (line, B);
      }

      sxba_minus (LRR, line);
      sxba_or (LR [0], line); /* cumul */
      SXBA_1_bit (LRR, A); /* representant d'une classe de recursivites gauches */
	
      if (!is_epsilon_free) {
	/* On remplit tbp_empty_nt_set avec les nt derivant ds le vide qu'il va falloir traiter... */
	item = 0;

	while ((item = sxba_scan_reset (item_set, item)) > 0) {
	  prod = PROLIS (item);
	  bot_item = PROLON (prod_hd, prod);
	  top_item = item;

	  while (--top_item >= bot_item) {
	    X = LISPRO (prod_hd, top_item);
	    SXBA_1_bit (tbp_empty_nt_set, X);
	  }
	}
      }
      else
	sxba_empty (item_set);
    }

    if (!is_epsilon_free) {
      /* Prudence, on ajoute a tbp_empty_nt_set les nt recursifs gauches */
      line = LR [0];
      A = 0;

      while ((A = sxba_scan (line, A)) > 0) {
	if (SXBA_bit_is_set (VIDE, A))
	  SXBA_1_bit (tbp_empty_nt_set, A);
      }
    }

    if (!is_epsilon_free && !sxba_is_empty (tbp_empty_nt_set)) {
      eps_rule_elimination (tbp_empty_nt_set);
    }

    sxfree (tbp_empty_nt_set);
    sxfree (tbp), tbp = NULL;
    sxfree (item_set), item_set = NULL;
    
    if (is_lr_recursive_free) {
      /* Ici on genere une nouvelle bnf... */
      /* On peut supprimer les recursivites gauches ... */
      /*  Ici LRR contient tous les representants des classes d'equivalences des recursivites gauches ... */
      /* ...et LR[A] la classe d'equivalence de A \in LRR */
      /* Les items recursifs sont des items initiaux */
      XH_alloc (&lr_prod_hd, "lr_prod_hd", XH_top (prod_hd)+1, 1, XH_list_top (prod_hd)/XH_top (prod_hd)+1, (sxoflw0_t) NULL, NULL);
      XxY_alloc (&lrA2prod_hd, "lrA2prod_hd", XH_top (prod_hd)+1, 1, 1, 0, (sxoflw0_t) NULL, NULL);
      ilr_nt_set = sxba_calloc (XNTMAX+1);
      non_lr_nt_set = sxba_calloc (XNTMAX+1);
      Aprimes = (SXINT*) sxcalloc (NTMAX+1, sizeof (SXINT));

      prev_xnt = XxY_top (xnt_hd);

      /* On va creer sxba_cardinal(LRR) nouveaux non terminaux qui vont deriver ds le vide */
      VIDE = sxba_resize (VIDE, NTMAX+sxba_cardinal(LRR)+1); 

      A = 0;

      while ((A = sxba_scan (LRR, A)) > 0) {
	left_recursion_elimination (LR [A]/*, RR [A] */);
      }

      XH_alloc (&new_prod_hd, "prod_hd", XH_top (prod_hd)+XH_top (lr_prod_hd)+1, 1, 
		XH_list_top (lr_prod_hd)/XH_top (lr_prod_hd)+1, (sxoflw0_t) NULL, NULL);

      /* On fait l'union de prod_hd et lr_prod_hd ds new_prod_hd */
      if (is_debug2) {
	fputs ("\n*Simple copy\n", stdout);
      }

      A = 0;

      for (A = 1; A <= XNTMAX; A++) {
	if (SXBA_bit_is_set (LR [0], A)) {
	  XxY_Xforeach (lrA2prod_hd, A, Aprod) {
	    prod = XxY_Y (lrA2prod_hd, Aprod);
	    item = XH_X (lr_prod_hd, prod);
	    top_item = XH_X (lr_prod_hd, prod+1);

	    if (is_debug2) {
	      fputs ("*From ", stdout);
	      print_prod (prod, &(REDUC (lr_prod_hd, prod)), &(REDUC (lr_prod_hd, prod+1)), (SXINT*)NULL);
	    }

	    while (item < top_item) {
	      X = LISPRO (lr_prod_hd, item++);
	      XH_push (new_prod_hd, X);
	    }

	    XH_set (&new_prod_hd, &prod_no);

	    if (is_debug2) {
	      fputs ("*To ", stdout);
	      print_prod (prod_no, &(REDUC (new_prod_hd, prod_no)), &(REDUC (new_prod_hd, prod_no+1)), (SXINT*)NULL);
	    }
	  }
	}
	else {
	  /* Les A_prod sont ds prod_hd */
	  foreach_NPG (A, x) {
	    XH_push (new_prod_hd, A);
	    prod = NUMPG (x); /* prod est une A_prod ... */

	    if (is_debug2) {
	      fputs ("*From ", stdout);
	      print_prod (prod, &(REDUC (prod_hd, prod)), &(REDUC (prod_hd, prod+1)), (SXINT*)NULL);
	    }

	    item = PROLON (prod_hd, prod);
	    top_item = TOP_ITEM (prod_hd, prod);

	    while (item < top_item) {
	      X = LISPRO (prod_hd, item++);
	      XH_push (new_prod_hd, X);
	    }

	    XH_set (&new_prod_hd, &prod_no);

	    if (is_debug2) {
	      fputs ("*To ", stdout);
	      print_prod (prod_no, &(REDUC (new_prod_hd, prod_no)), &(REDUC (new_prod_hd, prod_no+1)), (SXINT*)NULL);
	    }
	  }
	}
      }

      /* Maintenant on stocke les A' */
      for (A = XxY_top (xnt_hd); A > prev_xnt; A--) {
	Aprime = A+NTMAX;

	XxY_Xforeach (lrA2prod_hd, Aprime, Aprod) {
	  prod = XxY_Y (lrA2prod_hd, Aprod);
	  item = XH_X (lr_prod_hd, prod);
	  top_item = XH_X (lr_prod_hd, prod+1);

	  if (is_debug2) {
	    fputs ("*From ", stdout);
	    print_prod (prod, &(REDUC (lr_prod_hd, prod)), &(REDUC (lr_prod_hd, prod+1)), (SXINT*)NULL);
	  }

	  while (item < top_item) {
	    X = LISPRO (lr_prod_hd, item++);
	    XH_push (new_prod_hd, X);
	  }

	  XH_set (&new_prod_hd, &prod_no);

	  if (is_debug2) {
	    fputs ("*To ", stdout);
	    print_prod (prod_no, &(REDUC (new_prod_hd, prod_no)), &(REDUC (new_prod_hd, prod_no+1)), (SXINT*)NULL);
	  }
	}
      }

      if (is_debug2) {
	fputs ("*Simple copy (end)\n\n", stdout);
      }

      XH_free (&prod_hd);
      prod_hd = new_prod_hd;
      complete_prod_hd (); /* pour le rappel de lr_recursive_check () */

      XH_free (&lr_prod_hd);
      XxY_free (&lrA2prod_hd);
      sxfree (ilr_nt_set), ilr_nt_set = NULL;
      sxfree (non_lr_nt_set), non_lr_nt_set = NULL;
      sxfree (Aprimes), Aprimes = NULL;

      bnf_has_changed = true;
    }
  }

  sxbm_free (RR), RR = NULL;
  sxbm_free (LR), LR = NULL;
  sxfree (LRR), LRR = NULL;

  return is_ambiguous;
}

/* *************************************************************************************************
                          L E F T   R E C U S I O N   E L I M I N A T I O N (end)
   ************************************************************************************************* */

/* *************************************************************************************************
                                       C H E C K   P R O P E R
   ************************************************************************************************* */
static SXBA reached_set;
static SXINT  *reached_stack;

/* On met ds reached_set l'ensemble des nt accessibles depuis l'axiome */
static void
check_proper (void)
{
  SXINT A, x, prod, item, top_item, X;

  SXBA_1_bit (reached_set, 1);
  PUSH (reached_stack, 1);

  while (!IS_EMPTY (reached_stack)) {
    A = POP (reached_stack);

    foreach_NPG (A, x) {
      prod = NUMPG (x); /* prod est une A-prod */
      item = PROLON (prod_hd, prod);
      top_item = TOP_ITEM (prod_hd, prod);

      while (item < top_item) {
	X = LISPRO (prod_hd, item++);

	if (X > 0 && SXBA_bit_is_reset_set (reached_set, X))
	  PUSH (reached_stack, X);
      }
    }
  }
}
/* *************************************************************************************************
                                       C H E C K   P R O P E R (end)
   ************************************************************************************************* */


static int
run (char *name)
{
  SXINT     prod, A, x;
  bool is_ambiguous;

  if (!bnf_read (&bnf_ag, name))
    return 2;

  language_name = name;

  if (is_lexicon) {
    /* Ici, on se contente de generer la semact qui permet de reconnaitre une sequence
       d'associations forme_flechie ensemble de terminaux de la forme "vous {cla cld cln clr}" */
    SXINT t;

    out_header (bnf_header, language_name, "For the processing of a lexicon");

    fputs ("<axiom> = <S> ;\n", stdout);

    for (t = -1; t > TMAX; t--) {
      printf ("<T> = \"%s\" ;\n", get_t_string (t));
    }

    fputs ("<TL> = ;\n", stdout);
    fputs ("<TL> = <TL> <F> ; 1\n", stdout);
    fputs ("<F> = <T> ;\n", stdout);
    fputs ("<F> = %MOT ;\n", stdout);
    fputs ("<FF> = %MOT ; 2\n", stdout);
    fputs ("<FF> = <T> ; 3\n", stdout);
    fputs ("<FF> = ; 4\n", stdout);
    fputs ("<S> = <S> <L> ;\n", stdout);
    fputs ("<S> = ;\n", stdout);
    fputs ("<L> = <FF> ( <TL> ) ;\n", stdout);
  }
  else {
    /* Contiendra les nt qui ne derivent que dans le vide */
    full_eps_set = sxba_calloc (NTMAX+1);

    is_epsilon_free = (sxba_scan (bnf_ag.BVIDE, 0) == -1);

    if (!is_epsilon_free)
      fill_full_eps_set ();

    if (SXBA_bit_is_set (full_eps_set, 1)) {
      /* Ici le langage ne contient que la chaine vide ... */
      /* ... on genere donc <Axiom> = ; */
      out_header (bnf_header, language_name, "after (some) disambiguation");
      printf ("%s = ;\n", get_nt_string ((SXINT)1));
    }
    else {
      /* La grammaire est mise sous forme XH, on tient compte de full_eps_set */
      fill_prod_hd ();

      if (is_eps_rule_free && !sxba_is_empty (VIDE)) {
	eps_rule_elimination (VIDE);
	bnf_has_changed = true;
      }

      is_ambiguous = cycle_check (); /* is_ambiguous => y'a des cycles */
      /* Si la grammaire est cyclique et si "is_cycle_free", les cycles ont ete elimines */

      if (is_lr_recursive_free) {
	while (lr_recursive_check ());
      }
      else {
	if (lr_recursive_check ())
	  /* Y'a des recursivites gauches et droites simultanees */
	  is_ambiguous = true;
      }

      if (bnf_has_changed) {
	/* On a demande' de generer une nelle grammaire */
	reached_set = sxba_calloc (XNTMAX+1);
	reached_stack = (SXINT*) sxalloc (XNTMAX+1, sizeof (SXINT));
	reached_stack [0] = 0;

	check_proper ();

	out_header (bnf_header, language_name, "after (some) disambiguation");

	if (is_new_axiom_needed) {
	  /* L'axiome ne derive plus ds le vide ... */
	  /* Le langage contient la chaine vide et autre chose ... */
	  /* On genere :
	     <new_axiom> = <axiom> ;
	     <new_axiom> = ;
	  */
	  printf ("<new_%s = %s ;\n", get_nt_string ((SXINT)1)+1, get_nt_string ((SXINT)1));
	  printf ("<new_%s = ;\n", get_nt_string ((SXINT)1)+1);
	}

	for (A = 1; A <= XNTMAX; A++) {
	  if (SXBA_bit_is_set (reached_set, A)) {
	    foreach_NPG (A, x) {
	      prod = NUMPG (x); /* prod est une A-prod */
	      print_prod ((SXINT)0, &(REDUC (prod_hd, prod)), &(REDUC (prod_hd, prod+1)), (SXINT*)NULL);
	    }
	  }
	}

	sxfree (reached_set), reached_set = NULL;
	sxfree (reached_stack), reached_stack = NULL;
      }
      else {
	if (!is_debug1 && !is_debug2) {
	  fputs ("Grammar unchanged!!\n", sxstderr);

	  if (is_ambiguous)
	    fputs ("Ambiguous grammar, rerun it with the option \"-d(1|2)\" to know more...\n", sxstderr);  
	}
      }

      prod_hd_free ();
    }
  }

  bnf_free (&bnf_ag);

  return 0;
}



/************************************************************************/
/* main function */
/************************************************************************/
int main (int argc, char *argv[])
{
  int         argnum, severity;
  SXINT	      kind;

  sxopentty ();

  /* valeurs par defaut */
  /* is_help = is_debug1 = is_debug2 = is_stdout = is_lexicon = false; */
  is_full_eps_rule_free = is_eps_rule_free = is_cycle_free = is_lr_recursive_free = false;

  /* Decodage des options */
  for (argnum = 1; argnum < argc; argnum++) {
    kind = option_get_kind (argv [argnum]);

    switch (kind) {
    case HELP:
      is_help = true;
      break;

    case DEBUG1:
      is_debug1 = true;
      break;

    case DEBUG2:
      is_debug1 = true;
      is_debug2 = true;
      break;

    case LEXICON:
      is_lexicon = true;
      break;

    case FERF:
      is_full_eps_rule_free = true;
      break;

    case ERF:
      is_eps_rule_free = true;
      break;

    case CF:
      is_cycle_free = true;
      break;

    case LRRF:
      is_lr_recursive_free = true;
      break;

    case STDOUT:
      if (argnum+1 != argc) {
	fprintf (sxstderr, Usage, ME);
	sxexit (3);
      }

      is_stdout = true;
      break;

    case UNKNOWN_ARG:
      fprintf (sxstderr, "%s: unknown option \"%s\".\n", ME, argv [argnum]);
      sxexit (3);
    default:
#if EBUG
      sxtrap(ME,"unknown switch case #1");
#endif
      break;
    }

    if (kind == STDOUT)
      break;
  }

  if (!is_stdout || is_help) {
    fprintf (sxstderr, Usage, ME);
    sxexit (3);
  }

  if (is_lr_recursive_free)
    is_full_eps_rule_free = is_eps_rule_free = is_cycle_free = true;
  else
    if (is_cycle_free)
      is_full_eps_rule_free = is_eps_rule_free = true;
    else
      if (is_eps_rule_free)
	is_full_eps_rule_free = true;

  severity = run (argv [argnum]);

  sxexit (severity);

  return EXIT_SUCCESS; /* Jamais atteint !! pour les compilo susceptibles ... */
}
