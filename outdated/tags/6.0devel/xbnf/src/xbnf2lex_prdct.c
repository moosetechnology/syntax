/******************************************************************************
 *                                S Y N T A X
 *-----------------------------------------------------------------------------
 *   Copyright (C) 1972-2018 INRIA (Institut National de Recherche en
 *   Informatique et Automatique)
 *-----------------------------------------------------------------------------
 *   URL: http://syntax.gforge.inria.fr
 *-----------------------------------------------------------------------------
 *   The source code of SYNTAX is distributed with two different licenses,
 *   depending on the files:
 *   - The recursive content of src/ and incl/ and the non-recursive content
 *     of SYNTAX's root directory are distributed under the CeCILL-C license
 *   - The recursive content of all other repertories is distributed under
 *     the CeCILL license
 *   All code produced by SYNTAX must be considered as being under the
 *   CeCILL-C license. Information about the CeCILL and CeCILL-C licenses
 *   can be found at, e.g., http://www.cecill.fr
 *****************************************************************************/







/* Ce module permet la conversion d'une bnf sans predicats en une bnf
   avec des predicats qui permet de la lexicalization.
   Une prod A -> X ... t1 ... t2 ... est transformee en
   A -> X&i ... t1 ... t2 ..
   ou le predicat &i depend de la liste des terminaux t1 t2 ...
   De plus elle sort sur sa sortie standard le code C qui permet
   l'execution des predicats &i
*/

/* Si l'entree est une specif paradis, on ne la restitue pas!! */
/* A FAIRE entree at ou actions */



static char	ME [] = "xbnf2lex_prdct.c";

#include "sxunix.h"
#include "XH.h"
#include "varstr.h"
char WHAT_XBNF2LEXPRDCT[] = "@(#)SYNTAX - $Id: xbnf2lex_prdct.c 2227 2018-09-26 16:22:59Z garavel $" WHAT_DEBUG;


/*
N O D E   N A M E S
*/
#define ERROR_n 1
#define ACTION_n 2
#define BNF_n 3
#define GENERIC_TERMINAL_n 4
#define LHS_NON_TERMINAL_n 5
#define NON_TERMINAL_n 6
#define PREDICATE_n 7
#define RULE_S_n 8
#define TERMINAL_n 9
#define VOCABULARY_S_n 10
#define X_LHS_n 11
#define X_NON_TERMINAL_n 12
#define X_TERMINAL_n 13
/*
E N D   N O D E   N A M E S
*/


/*------------------*/
/* extern variables */
/*------------------*/

extern struct sxtables *sxtab_ptr;
extern char	*prgentname, *processorname;
extern SXBOOLEAN is_predicate;

/*-----------------*/
/* local variables */
/*-----------------*/

static FILE *outfile;
static SXINT prod, top;
static XH_header rhs_tlist;
static SXINT *prdct_disp;
static SXINT *ste2t, t;

static SXVOID
put_prdct (SXNODE *visited)
{
  SXNODE	*brother;
  SXINT tnb, ste, prdct;

  if (visited->position == 2) {
    /* On parcourt tous les freres */
    brother = visited;
    tnb = 0;

    while ((brother = brother->brother)) {
      if (brother->name == TERMINAL_n || brother->name == GENERIC_TERMINAL_n) {
	tnb++;
	ste = brother->token.string_table_entry;
      
	if (ste2t [ste] == 0)
	  ste2t [ste] = ++t; /* code du terminal */

	if (brother->position == 3)
	  XH_push (rhs_tlist, -ste2t [ste]);
	else
	  XH_push (rhs_tlist, ste2t [ste]);
      }
    }

    if (tnb > 0) {
      if (!XH_set (&rhs_tlist, &prdct)) {
	/* nouveau */
	prdct_disp [prdct] = top;
	top += tnb+1;
      }

      printf ("&%ld ", (long)prdct);
      fprintf (outfile, "&%ld ", (long)prdct);
    }
  }
}


static SXVOID
tree_walk (SXNODE *visited)
{
  SXINT ste, prdct, x, y;
  char *p, *t_string;

  /* calcul recursif de indpro_size */

  for (;;) {
    ste = visited->token.string_table_entry;

    switch (visited->name) {
    case ACTION_n:
      return;

    case BNF_n:
      fputs ("/*\n", stdout);
      break;

    case X_LHS_n:
      ste = visited->son->token.string_table_entry;
      printf ("%ld : %s ", (long)++prod, sxstrget (ste));
      fprintf (outfile, "%s ", sxstrget (ste));
      ste = visited->son->brother->token.string_table_entry;
      printf ("%s = ", sxstrget (ste));
      fprintf (outfile, "%s = ", sxstrget (ste));
      return;

    case LHS_NON_TERMINAL_n:
      printf ("%ld : %s = ", (long)++prod, sxstrget (ste));
      fprintf (outfile, "%s = ", sxstrget (ste));
      return;

    case X_NON_TERMINAL_n:
    case X_TERMINAL_n:
      return;

    case TERMINAL_n:
    case GENERIC_TERMINAL_n:
      if (ste2t [ste] == 0)
	ste2t [ste] = ++t; /* code du terminal */

      t_string = sxstrget (ste);
      fputs ("\"", stdout);
      fputs ("\"", outfile);

      while ((p = sxindex (t_string, '"')) != NULL) {
	*p = SXNUL;
	printf ("%s", t_string);
	fprintf (outfile, "%s", t_string);
	fputs ("\\\"", stdout);
	fputs ("\\\"", outfile);
	*p = '"';
	t_string = p+1;
      }

      printf ("%s\" ", t_string);
      fprintf (outfile, "%s\" ", t_string);

      put_prdct (visited);

      return;

    case NON_TERMINAL_n:
      printf ("%s ", sxstrget (ste));
      fprintf (outfile, "%s ", sxstrget (ste));

      put_prdct (visited);

      return;

    case VOCABULARY_S_n:
      for (visited = visited->son; visited != NULL; visited = visited->brother)
	tree_walk (visited);

      /* On a fini une prod */
      fputs (";\n", stdout);
      fputs (";\n", outfile);
      return;

    case RULE_S_n:
      XH_alloc (&rhs_tlist, "rhs_tlist", visited->degree, 1, 3, NULL, NULL);
      prdct_disp = (SXINT*) sxalloc (visited->degree + 1, sizeof (SXINT));
      top = 1;

      for (visited = visited->son; visited != NULL; visited = visited->brother)
	tree_walk (visited);

      /* On a fini toutes les prod */
      
      fputs ("*/\n", stdout);

      /* On sort les tables... */
      fputs ("static SXINT prdct_disp [] = {0,\n", stdout);
      
      for (prdct = 1; prdct < XH_top (rhs_tlist); prdct++) {
	printf ("/* %ld */ %ld,\n", (long)prdct, (long)prdct_disp [prdct]);
      }

      fputs ("}; /* end prdct_disp [] */\n", stdout);

      fputs ("static SXINT prdct_code [] = {0,\n", stdout);

      x = XH_X (rhs_tlist, 1);

      for (prdct = 1; prdct < XH_top (rhs_tlist); prdct++) {
	y = XH_X (rhs_tlist, prdct+1);

	printf ("/* %ld */ ", (long)prdct);

	for (x = XH_X (rhs_tlist, prdct); x < y; x++) {
	  printf ("%ld, ", (long)XH_list_elem (rhs_tlist, x));
	}

	fputs ("0,\n", stdout);
      }

      fputs ("}; /* end prdct_code [] */\n", stdout);


      XH_free (&rhs_tlist);
      sxfree (prdct_disp);

    default:
      return;
    }

    visited = visited->son;
  }
}



/*----------------*/
/* xbnf2lex_prdct */
/*----------------*/

SXVOID	xbnf2lex_prdct (SXNODE *adam)
{

/*-----------------------------------------------*/
/*  A T T R I B U T E S    E V A L U A T I O N   */
/*-----------------------------------------------*/
  VARSTR vstr;

  if (is_predicate /* Le scanner a trouve des predicats */ ) {
    sxtmsg (prgentname, "Only predicate-free grammars can be transformed.", sxtab_ptr->err_titles [2] /* error */);
    return;
  }

  vstr = varstr_alloc (64);

  vstr = varstr_catenate (varstr_catenate (varstr_catenate (vstr, prgentname), "_lex."), processorname);

  if ((outfile = sxfopen (varstr_tostr (vstr), "w")) == NULL) {
    fprintf (sxstderr, "%s: Cannot open (read) ", ME);
    sxperror (varstr_tostr (vstr));
    sxerrmngr.nbmess [2]++;
    varstr_free (vstr);
    return;
  }

  ste2t = (SXINT*) sxcalloc (SXSTRtop ()+1, sizeof (SXINT));

  tree_walk (adam);

  varstr_free (vstr);
  sxfree (ste2t);
}
