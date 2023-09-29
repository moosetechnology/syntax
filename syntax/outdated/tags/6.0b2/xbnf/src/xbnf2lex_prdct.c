/* ********************************************************
   *							  *
   *							  *
   * Copyright (c) 1987 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *							  *
   *							  *
   ******************************************************** */




/* ********************************************************
   *							  *
   *  Produit de l'equipe Langages et Traducteurs.  (bl)  *
   *							  *
   ******************************************************** */


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


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030512 13:07 (phd):	Adaptation à SGI (64 bits)		*/
/************************************************************************/
/* 12 Fev 2003 11:0 (pb):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/

static char	ME [] = "xbnf2lex_prdct.c";

#include "sxunix.h"
#include "XH.h"
#include "varstr.h"
char WHAT_XBNF2LEXPRDCT[] = "@(#)SYNTAX - $Id: xbnf2lex_prdct.c 545 2007-05-14 08:34:04Z rlacroix $" WHAT_DEBUG;


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


/*-----------------*/
/* local variables */
/*-----------------*/

struct sxtables *sxtab_ptr;
char	*prgentname, *processorname;
BOOLEAN is_predicate;

static FILE *outfile;
static int prod, top;
static XH_header rhs_tlist;
static int *prdct_disp;
static int *ste2t, t;

static VOID
put_prdct (visited)
    NODE	*visited;
{
  NODE	*brother;
  int tnb, ste, prdct;

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

      printf ("&%i ", prdct);
      fprintf (outfile, "&%i ", prdct);
    }
  }
}


static VOID
tree_walk (visited)
    NODE	*visited;
{
  int ste, prdct, x, y;
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
      printf ("%i : %s ", ++prod, sxstrget (ste));
      fprintf (outfile, "%s ", sxstrget (ste));
      ste = visited->son->brother->token.string_table_entry;
      printf ("%s = ", sxstrget (ste));
      fprintf (outfile, "%s = ", sxstrget (ste));
      return;

    case LHS_NON_TERMINAL_n:
      printf ("%i : %s = ", ++prod, sxstrget (ste));
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
	*p = NUL;
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
      prdct_disp = (int*) sxalloc (visited->degree + 1, sizeof (int));
      top = 1;

      for (visited = visited->son; visited != NULL; visited = visited->brother)
	tree_walk (visited);

      /* On a fini toutes les prod */
      
      fputs ("*/\n", stdout);

      /* On sort les tables... */
      fputs ("static int prdct_disp [] = {0,\n", stdout);
      
      for (prdct = 1; prdct < XH_top (rhs_tlist); prdct++) {
	printf ("/* %i */ %i,\n", prdct, prdct_disp [prdct]);
      }

      fputs ("}; /* end prdct_disp [] */\n", stdout);

      fputs ("static int prdct_code [] = {0,\n", stdout);

      x = XH_X (rhs_tlist, 1);

      for (prdct = 1; prdct < XH_top (rhs_tlist); prdct++) {
	y = XH_X (rhs_tlist, prdct+1);

	printf ("/* %i */ ", prdct);

	for (x = XH_X (rhs_tlist, prdct); x < y; x++) {
	  printf ("%i, ", XH_list_elem (rhs_tlist, x));
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

VOID	xbnf2lex_prdct (adam)
    NODE	*adam;
{

/*-----------------------------------------------*/
/*  A T T R I B U T E S    E V A L U A T I O N   */
/*-----------------------------------------------*/
  VARSTR vstr;

  if (is_predicate /* Le scanner a trouve des predicats */ ) {
    sxtmsg (prgentname, "%sOnly predicate-free grammars can be transformed.", sxtab_ptr->err_titles [2] /* error */);
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

  ste2t = (int*) sxcalloc (SXSTRtop ()+1, sizeof (int));

  tree_walk (adam);

  varstr_free (vstr);
  sxfree (ste2t);
}
