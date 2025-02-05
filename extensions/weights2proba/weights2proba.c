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

/*** THIS FILE IS NOT PART OF THE SYNTAX LIBRARY libsx.a ***/ 

/* Transforme un fichier de de'compte de l'utilisation de regles en
   un fichier c utilisable comme une PCFG */

/* Doit etre compile' avec l'option
   -DLC_TABLES_H=\"easy_lc.h\"	\           Pour inclure (et compiler) les tables lc
*/

static char	ME [] = "weights2proba";

#include "sxversion.h"
#include "sxunix.h"

char WHAT_WEIGHTS2PROBA[] = "@(#)SYNTAX - $Id: weights2proba.c 4364 2024-10-02 12:59:56Z garavel $" WHAT_DEBUG;

#ifdef LC_TABLES_H
/* On compile les tables "left_corner" ... */
/* ... uniquement celles dont on peut avoir besoin */
#define def_lispro
#define def_prolon
#define def_lhs
#define def_ntstring
#define def_tstring

#include LC_TABLES_H
#endif /* LC_TABLES_H */


#include "earley.h"
#include "X.h"
#include "XxY.h"
#include "XH.h"
#include <stdio.h>
#include <math.h>
#include <float.h>

static SXINT max_signature_depth;/* defaut 0 => unbounded */

static void
gen_header ()
{
  printf ("\n\
/************************************************************************************************************\n\
\tThis file contains, for each tuple (father_prod, father_val, son_pos, son_prod, son_val), its logprob\n\
\tit has been generated\
\tby the SYNTAX(*) processor weights2proba(*)\n\
     **************************************************************************************************\n\
\t(*) SYNTAX and weights2proba are trademarks of INRIA.\n\
     **************************************************************************************************/\n\n");
}

static void
print_X (SXINT X)
{
  if (X < 0)
    printf ("\"%s\" ", tstring [-X]);
  else
    printf ("<%s> ", ntstring [X]);
}

static void
print_prod (SXINT prod)
{
  SXINT item, X;

  print_X (lhs [prod]);

  fputs (" = ", stdout);

  item = prolon [prod];

  while ((X = lispro [item++]) != 0) {
    print_X (X);
  }

  putchar (';');
}

/*
NAME
       log10, log10f, log10l -  base-10 logarithmic function

SYNOPSIS
       #include <math.h>

       double log10(double x);
       float log10f(float x);
       long double log10l(long double x);

DESCRIPTION
       The log10() function returns the base 10 logarithm of x.
*/


static SXINT          *eprod_father2nb, *eprod_son2nb, *full_ctxtXeprod2nb, *eprod_ctxtXeprod2nb, *prod_ctxtXeprod2nb, *A_ctxtXeprod2nb;
static SXINT          *A2nb, *B2nb, *prod_father2nb, *prod_son2nb, *full_ctxt2nb;
static SXINT          *ff_id2nb, *full_ctxtXlex_id2nb, *eprod_ctxtXlex_id2nb, *prod_ctxtXlex_id2nb, *A_ctxtXlex_id2nb, *lex_id2nb;
static SXINT          *B2sign_nb, *ctxtXsign2nb;

static void
prodXval2eprod_oflw (SXINT old_size, SXINT new_size)
{
  eprod_father2nb = (SXINT*) sxrecalloc (eprod_father2nb, old_size+1, new_size+1, sizeof (SXINT));
  eprod_son2nb = (SXINT*) sxrecalloc (eprod_son2nb, old_size+1, new_size+1, sizeof (SXINT));
}


static void
full_ctxt_oflw (SXINT old_size, SXINT new_size)
{
  full_ctxt2nb = (SXINT*) sxrecalloc (full_ctxt2nb, old_size+1, new_size+1, sizeof (SXINT));
}


static void
full_ctxtXeprod_oflw (SXINT old_size, SXINT new_size)
{
  full_ctxtXeprod2nb = (SXINT*) sxrecalloc (full_ctxtXeprod2nb, old_size+1, new_size+1, sizeof (SXINT));
}


static void
eprod_ctxtXeprod_oflw (SXINT old_size, SXINT new_size)
{
  eprod_ctxtXeprod2nb = (SXINT*) sxrecalloc (eprod_ctxtXeprod2nb, old_size+1, new_size+1, sizeof (SXINT));
}


static void
prod_ctxtXeprod_oflw (SXINT old_size, SXINT new_size)
{
  prod_ctxtXeprod2nb = (SXINT*) sxrecalloc (prod_ctxtXeprod2nb, old_size+1, new_size+1, sizeof (SXINT));
}


static void
A_ctxtXeprod_oflw (SXINT old_size, SXINT new_size)
{
  A_ctxtXeprod2nb = (SXINT*) sxrecalloc (A_ctxtXeprod2nb, old_size+1, new_size+1, sizeof (SXINT));
}


static void
ff_id_oflw (SXINT old_size, SXINT new_size)
{
  ff_id2nb = (SXINT*) sxrecalloc (ff_id2nb, old_size+1, new_size+1, sizeof (SXINT));
}


static void
full_ctxtXlex_id_oflw (SXINT old_size, SXINT new_size)
{
  full_ctxtXlex_id2nb = (SXINT*) sxrecalloc (full_ctxtXlex_id2nb, old_size+1, new_size+1, sizeof (SXINT));
}


static void
eprod_ctxtXlex_id_oflw (SXINT old_size, SXINT new_size)
{
  eprod_ctxtXlex_id2nb = (SXINT*) sxrecalloc (eprod_ctxtXlex_id2nb, old_size+1, new_size+1, sizeof (SXINT));
}


static void
prod_ctxtXlex_id_oflw (SXINT old_size, SXINT new_size)
{
  prod_ctxtXlex_id2nb = (SXINT*) sxrecalloc (prod_ctxtXlex_id2nb, old_size+1, new_size+1, sizeof (SXINT));
}


static void
A_ctxtXlex_id_oflw (SXINT old_size, SXINT new_size)
{
  A_ctxtXlex_id2nb = (SXINT*) sxrecalloc (A_ctxtXlex_id2nb, old_size+1, new_size+1, sizeof (SXINT));
}


static void
ff_idXt2lex_id_oflw (SXINT old_size, SXINT new_size)
{
  lex_id2nb = (SXINT*) sxrecalloc (lex_id2nb, old_size+1, new_size+1, sizeof (SXINT));
}


static void
ctxtXsign_oflw (SXINT old_size, SXINT new_size)
{
  ctxtXsign2nb =  (SXINT*) sxrecalloc (ctxtXsign2nb, old_size+1, new_size+1, sizeof (SXINT));
}


/* Supposons que prod soit une B-prod.  La proba d'avoir (A, prod v)
   est n/m avec
   n = nb de fois ou on a vu (A, prod v)
   m = nb de fois ou on a vu (A, B)
*/

static void
process_weights ()
{
  SXINT          A, B, nb, total_nb, sentence_nb, single, couple, lex_id, top, eprod_father, eprod_son, ff_id, t;
  SXINT          prod_father, val_father, prod_son, val_son, pos, full_ctxt, fs_signature_size, max_fs_signature_size, full_couple, x, elem, signature;
  XxY_header   full_ctxt_hd, prodXval2eprod_hd, eprod_ctxtXeprod_hd, prod_ctxtXeprod_hd, A_ctxtXeprod_hd, full_ctxtXeprod_hd;
  XxY_header   full_ctxtXlex_id_hd, eprod_ctxtXlex_id_hd, prod_ctxtXlex_id_hd, A_ctxtXlex_id_hd, ff_idXt2lex_id_hd;
  X_header     ff_id_hd;
  XxY_header   ctxtXsign_hd;
  XH_header    fs_signatures_hd;
  char         *number_str, *str, *str1;
  double       min_logprob, logprob, min_logprob_fs;
  SXINT          LINE_LGTH = 256;
  SXINT          cur_level_nb, prev_level_nb, depth;

  /* on lit le fichier*/
#if LOG
  printf ("Reading %s\n", weights_file_name);
#endif /* LOG */
  
  min_logprob = -log10 (DBL_MAX);

  max_fs_signature_size = sentence_nb = 0;
	
  number_str = (char *) sxalloc (LINE_LGTH, sizeof (char));
  number_str [LINE_LGTH-1] = '\1'; /* Pour le debordement */

  /*
    char *fgets(char *s, SXINT size, FILE *stream);

    fgets() reads in at most one less than size characters from stream  and
    stores  them  into  the buffer pointed to by s.  Reading stops after an
    EOF or a newline.  If a newline is read, it is stored into the  buffer.
    A '\0' is stored after the last character in the buffer.
  */

  /* lit "sentence_nb 0 0 0 0 0" */
  if (fgets (number_str, LINE_LGTH, stdin)) {
    sentence_nb = strtol (number_str, &str1, 10);

    XxY_alloc (&prodXval2eprod_hd, "prodXval2eprod_hd", prodmax+ntmax+1, 1, 0, 0, prodXval2eprod_oflw, NULL);
    eprod_father2nb = (SXINT *) sxcalloc (XxY_size (prodXval2eprod_hd)+1, sizeof (SXINT));
    eprod_son2nb = (SXINT *) sxcalloc (XxY_size (prodXval2eprod_hd)+1, sizeof (SXINT));
    A2nb = (SXINT*) sxcalloc (ntmax+1, sizeof (SXINT));
    B2nb = (SXINT*) sxcalloc (ntmax+1, sizeof (SXINT));
    B2sign_nb = (SXINT*) sxcalloc (ntmax+1, sizeof (SXINT));
    prod_father2nb = (SXINT *) sxcalloc (prodmax+1, sizeof (SXINT));
    prod_son2nb = (SXINT *) sxcalloc (prodmax+1, sizeof (SXINT));

    XxY_alloc (&full_ctxt_hd, "full_ctxt_hd", prodmax+ntmax+1, 1, 0, 0, full_ctxt_oflw, NULL);
    full_ctxt2nb = (SXINT *) sxcalloc (XxY_size (full_ctxt_hd)+1, sizeof (SXINT));

    XxY_alloc (&full_ctxtXeprod_hd, "full_ctxtXeprod_hd", prodmax+ntmax+1, 1, 0, 0, full_ctxtXeprod_oflw, NULL);
    full_ctxtXeprod2nb = (SXINT*) sxcalloc (XxY_size (full_ctxtXeprod_hd)+1, sizeof (SXINT));

    XxY_alloc (&eprod_ctxtXeprod_hd, "eprod_ctxtXeprod_hd", prodmax+ntmax+1, 1, 0, 0, eprod_ctxtXeprod_oflw, NULL);
    eprod_ctxtXeprod2nb = (SXINT*) sxcalloc (XxY_size (eprod_ctxtXeprod_hd)+1, sizeof (SXINT));

    XxY_alloc (&prod_ctxtXeprod_hd, "prod_ctxtXeprod_hd", prodmax+ntmax+1, 1, 0, 0, prod_ctxtXeprod_oflw, NULL);
    prod_ctxtXeprod2nb = (SXINT*) sxcalloc (XxY_size (prod_ctxtXeprod_hd)+1, sizeof (SXINT));

    XxY_alloc (&A_ctxtXeprod_hd, "A_ctxtXeprod_hd", prodmax+ntmax+1, 1, 0, 0, A_ctxtXeprod_oflw, NULL);
    A_ctxtXeprod2nb = (SXINT*) sxcalloc (XxY_size (A_ctxtXeprod_hd)+1, sizeof (SXINT));

    X_alloc (&ff_id_hd, "ff_id_hd", prodmax+ntmax+1, 1, ff_id_oflw, NULL);
    ff_id2nb = (SXINT*) sxcalloc (X_size (ff_id_hd)+1, sizeof (SXINT));

    XxY_alloc (&full_ctxtXlex_id_hd, "full_ctxtXlex_id_hd", prodmax+ntmax+1, 1, 0, 0, full_ctxtXlex_id_oflw, NULL);
    full_ctxtXlex_id2nb = (SXINT*) sxcalloc (XxY_size (full_ctxtXlex_id_hd)+1, sizeof (SXINT));

    XxY_alloc (&eprod_ctxtXlex_id_hd, "eprod_ctxtXlex_id_hd", prodmax+ntmax+1, 1, 0, 0, eprod_ctxtXlex_id_oflw, NULL);
    eprod_ctxtXlex_id2nb = (SXINT*) sxcalloc (XxY_size (eprod_ctxtXlex_id_hd)+1, sizeof (SXINT));

    XxY_alloc (&prod_ctxtXlex_id_hd, "prod_ctxtXlex_id_hd", prodmax+ntmax+1, 1, 0, 0, prod_ctxtXlex_id_oflw, NULL);
    prod_ctxtXlex_id2nb = (SXINT*) sxcalloc (XxY_size (prod_ctxtXlex_id_hd)+1, sizeof (SXINT));

    XxY_alloc (&A_ctxtXlex_id_hd, "A_ctxtXlex_id_hd", prodmax+ntmax+1, 1, 0, 0, A_ctxtXlex_id_oflw, NULL);
    A_ctxtXlex_id2nb = (SXINT*) sxcalloc (XxY_size (A_ctxtXlex_id_hd)+1, sizeof (SXINT));

    XxY_alloc (&ff_idXt2lex_id_hd, "ff_idXt2lex_id_hd", prodmax+ntmax+1, 1, 0, 0, ff_idXt2lex_id_oflw, NULL);
    lex_id2nb = (SXINT*) sxcalloc (XxY_size (ff_idXt2lex_id_hd)+1, sizeof (SXINT));

    XH_alloc (&fs_signatures_hd, "fs_signatures_hd", prodmax+ntmax+1, 1, 4, NULL, NULL);
    XxY_alloc (&ctxtXsign_hd, "ctxtXsign_hd", prodmax+ntmax+1, 1, 0, 0, ctxtXsign_oflw, NULL);
    ctxtXsign2nb =  (SXINT*) sxcalloc (XxY_size (ctxtXsign_hd)+1, sizeof (SXINT));

    /* On note 6 possibilites avec des contextes + ou - larges entre un parent et ses enfants */

    while (fgets (number_str, LINE_LGTH, stdin)) {
      while (number_str [LINE_LGTH-1] == SXNUL) {
	/* trop petit */
	number_str = (char *) sxrealloc (number_str, 2*LINE_LGTH, sizeof (char));
	number_str [2*LINE_LGTH-1] = '\1'; /* Pour le debordement */

	if (number_str [LINE_LGTH-2] != '\n') {
	  /* la ligne n'est pas lue en entier */
	  str = number_str+LINE_LGTH-1; /* pointe sur le SXNUL */
	  fgets (str, LINE_LGTH+1, stdin);
	}

	LINE_LGTH *= 2;
      }

      str = number_str;
      nb = strtol (str, &str1, 10);
      str = str1;
      prod_father =  strtol (str, &str1, 10);
      A = lhs [prod_father];
      str = str1;
      val_father =  strtol (str, &str1, 10);
      str = str1;
      pos =  strtol (str, &str1, 10);
      str = str1;
      prod_son =  strtol (str, &str1, 10);
      B = lhs [prod_son];
      str = str1;

      if (prod_son <= prodmax) {
	val_son =  strtol (str, &str1, 10);
	str = str1;
	fs_signature_size =  strtol (str, &str1, 10);

	XxY_set (&prodXval2eprod_hd, prod_son, val_son, &eprod_son);

	if (fs_signature_size) {
	  /* C'est une specif de signature */
	  if (fs_signature_size > max_fs_signature_size)
	    max_fs_signature_size = fs_signature_size;

	  if (max_signature_depth) {
	    cur_level_nb = 0;
	    prev_level_nb = 0;
	    depth = max_signature_depth;
	  } else {
            /* pour faire taire gcc */
	    cur_level_nb = 0; /* dummy value */
	    prev_level_nb = 0; /* dummy value */
	    depth = 0; /* dummy value */
	  }

	  for (x = 1; x <= fs_signature_size; x++) {
	    str = str1;
	    elem = strtol (str, &str1, 10);
	    XH_push (fs_signatures_hd, elem);

	    if (max_signature_depth) {
	      if (elem != 0)
		/* f_structure non vide, elle peut donc avoir des fils */
		cur_level_nb++;

	      if (elem <= 0) {
		/* fin d'une f_structure */
		if (--prev_level_nb <= 0) {
		  /* fin d'un niveau */
		  if (--depth == 0)
		    /* fin */
		    break;

		  prev_level_nb = cur_level_nb;
		  cur_level_nb = 0;
		}
	      }
	    }
	  }

	  XH_set (&fs_signatures_hd, &signature);

	  /* A un quintuplet (prod_father, val_father, pos, prod_son, val_son) on va associer une
	     liste de signatures et a chacune de ces signatures on va lui associer une proba */
	  /* En lfg, le ranking consistera a ne garder que les f_structures dont les signatures
	     ont la proba max.  Si aucune des f_structures n'a de signature, le ranking est inoperant */
	  /* On peut aussi calculer pour chaque fs une proba de la facon suivante :
	     - Si on suppose que pour chaque fs associee au nt instancie Xpq et calcule par la prod
	       instanciee Ppq on a associe' une proba p.
	     - On est sur la prod instanciee Pij et on vient de calculer une f-structure fs' a partir de ses fils
	       ... (Xpq, Ppq, fs, p) ...
	       a Pij est associe' (init_prod, eval), a Xpq est associe' (pos, init_prod', eval'). init_prod' est l'init-prod
	       de Ppq et eval' son eval. La proba associee a fs' est la somme de 2 composantes, les fils et une composante
	       contextuelle (des fils) :
	       a) composante des fils : \sigma p
	       b) composante contextuelle \sigma proba_statique (init_prod, eval, pos, init_prod', eval', signature (fs'))
	     Ds ce cas le ranking consistera a ne garder que la/les f_structures de proba max
	  */
	  B2sign_nb [B] += nb;
	  /* Modif le 27/06/06 on n'utilise que du ctxt local (donc eprod_son)
	  XxY_set (&ctxtXsign_hd, full_couple, signature, &couple);
	  */
	  XxY_set (&ctxtXsign_hd, eprod_son, signature, &couple);
	  ctxtXsign2nb [couple] += nb;
	}
	else {
	  XxY_set (&prodXval2eprod_hd, prod_father, val_father, &eprod_father);
	  XxY_set (&full_ctxt_hd, eprod_father, pos, &full_ctxt);
	  full_ctxt2nb [full_ctxt] += nb; /* total pour full ctxt */
	  XxY_set (&full_ctxtXeprod_hd, full_ctxt, eprod_son, &full_couple);

	  eprod_father2nb [eprod_father] += nb; /* total pour eprod ctxt */
	  prod_father2nb [prod_father] += nb; /* total pour prod ctxt */
	  A2nb [A] += nb; /* total pour A ctxt */
	  prod_son2nb [prod_son] += nb; /* local pour pas de ctxt prod et total pour pas de ctxt eprod */
	  B2nb [B] += nb; /* total pour pas de ctxt prod */
	  eprod_son2nb [eprod_son] += nb; /* local pour pas de ctxt eprod */
      
	  full_ctxtXeprod2nb [full_couple] += nb; /* local pour full ctxt */

	  XxY_set (&eprod_ctxtXeprod_hd, eprod_father, eprod_son, &couple);
	  eprod_ctxtXeprod2nb [couple] += nb; /* local pour eprod ctxt */

	  XxY_set (&prod_ctxtXeprod_hd, prod_father, eprod_son, &couple);
	  prod_ctxtXeprod2nb [couple] += nb; /* local pour prod ctxt */

	  XxY_set (&A_ctxtXeprod_hd, A, eprod_son, &couple);
	  A_ctxtXeprod2nb [couple] += nb; /* local pour A ctxt */
	}
      }
      else {
	/* On est sur des Tij */
	XxY_set (&prodXval2eprod_hd, prod_father, val_father, &eprod_father);
	XxY_set (&full_ctxt_hd, eprod_father, pos, &full_ctxt);
	full_ctxt2nb [full_ctxt] += nb; /* total pour full ctxt */
	ff_id =  strtol (str, &str1, 10);
	str = str1;
	t =  strtol (str, &str1, 10);

	X_set (&ff_id_hd, ff_id, &single);
	ff_id2nb [single] += nb;

	XxY_set (&ff_idXt2lex_id_hd, ff_id, t, &lex_id);
	lex_id2nb [lex_id] += nb;

	XxY_set (&full_ctxtXlex_id_hd, full_ctxt, lex_id, &couple);
	full_ctxtXlex_id2nb [couple] += nb;

	XxY_set (&eprod_ctxtXlex_id_hd, eprod_father, lex_id, &couple);
	eprod_ctxtXlex_id2nb [couple] += nb;

	XxY_set (&prod_ctxtXlex_id_hd, prod_father, lex_id, &couple);
	prod_ctxtXlex_id2nb [couple] += nb;

	XxY_set (&A_ctxtXlex_id_hd, A, lex_id, &couple);
	A_ctxtXlex_id2nb [couple] += nb;
      }
    }
  }

  /* Le 11/04/06 : toutes les probas sont rapportees au nb total d'occurrences des B-prods
     - Ca ne change pas les valeurs relatives pour un pere donne'
     - mais, si le pere est different, ca va donner des valeurs qui pourront etre exploitees
       +tard (sur leur ancetre commun) simultanement
  */

  sxfree (number_str);

  gen_header ();

  printf ("/* These statistics have been generated by the ATOLL's SxLfg parser (option -w) running\n\
   on the single consistent C- and F-structures of %ld sentences. */\n\n", sentence_nb);

  fputs ("#include \"XxY.h\"\n", stdout);
  fputs ("#include \"XH.h\"\n\n", stdout);

  printf ("#define PROD_NB %ld\n", prodmax);
  printf ("#define NT_NB %ld\n", ntmax);
  fputs ("\nint logprobs_prod_nb = PROD_NB;\n", stdout);
  fputs ("SXINT logprobs_nt_nb = NT_NB;\n", stdout);

  XxY_lock (&prodXval2eprod_hd);
  XxY_to_c (&prodXval2eprod_hd, stdout, "prodXval2eprod_hd", false /* external */);

  XxY_lock (&full_ctxt_hd);
  XxY_to_c (&full_ctxt_hd, stdout, "full_ctxt_hd", false /* external */);

  /* Utilisation maximale du contexte */
  XxY_lock (&full_ctxtXeprod_hd);
  top = XxY_top (full_ctxtXeprod_hd);
  fputs ("\n\n/* Full context case */", stdout);
  XxY_to_c (&full_ctxtXeprod_hd, stdout, "full_ctxtXeprod_hd", false /* external */);

  printf ("\ndouble full_ctxtXeprod2logprobs [%ld] = {%f,\n", top+1, min_logprob);

  for (couple = 1; couple <= top; couple++) {
    full_ctxt = XxY_X (full_ctxtXeprod_hd, couple);
    eprod_father = XxY_X (full_ctxt_hd, full_ctxt);
    pos = XxY_Y (full_ctxt_hd, full_ctxt);
    eprod_son = XxY_Y (full_ctxtXeprod_hd, couple);
    nb = full_ctxtXeprod2nb [couple]; /* local pour full ctxt */
    total_nb = full_ctxt2nb [full_ctxt]; /* total pour full ctxt */

    prod_father = XxY_X (prodXval2eprod_hd, eprod_father);
    val_father = XxY_Y (prodXval2eprod_hd, eprod_father);

    prod_son = XxY_X (prodXval2eprod_hd, eprod_son);
    val_son = XxY_Y (prodXval2eprod_hd, eprod_son);

    B = lhs [prod_son];
    total_nb = B2nb [B]; /* nouveau denominateur commun */
    
    printf ("/* %ld:\tfather %ld=", couple, prod_father);
    print_prod (prod_father);
    printf (" [val=%ld]\n", val_father);
    
    printf ("\tson %ld=", prod_son);
    print_prod (prod_son);
    printf (" [val=%ld] at pos %ld, log10 (%ld/%ld) = */ %f,\n", val_son, pos, nb, total_nb, log10 ((double)nb / (double)total_nb));
  }

  fputs ("};\n\n", stdout);

  /* On n'utilise pas la position du fils ds le pere */
  XxY_lock (&eprod_ctxtXeprod_hd);
  top = XxY_top (eprod_ctxtXeprod_hd);
  fputs ("\n\n/* Full context case, without pos */\n", stdout);
  XxY_to_c (&eprod_ctxtXeprod_hd, stdout, "eprod_ctxtXeprod_hd", false /* external */);

  printf ("\ndouble eprod_ctxtXeprod2logprobs [%ld] = {%f,\n", top+1, min_logprob);

  for (couple = 1; couple <= top; couple++) {
    eprod_father = XxY_X (eprod_ctxtXeprod_hd, couple);
    eprod_son = XxY_Y (eprod_ctxtXeprod_hd, couple);
    nb = eprod_ctxtXeprod2nb [couple]; /* local pour eprod ctxt */
    total_nb = eprod_father2nb [eprod_father]; /* total pour eprod ctxt */

    prod_father = XxY_X (prodXval2eprod_hd, eprod_father);
    val_father = XxY_Y (prodXval2eprod_hd, eprod_father);

    prod_son = XxY_X (prodXval2eprod_hd, eprod_son);
    val_son = XxY_Y (prodXval2eprod_hd, eprod_son);

    B = lhs [prod_son];
    total_nb = B2nb [B]; /* nouveau denominateur commun */
    
    printf ("/* %ld:\tfather %ld=", couple, prod_father);
    print_prod (prod_father);
    printf (" [val=%ld]\n", val_father);
    
    printf ("\tson %ld=", prod_son);
    print_prod (prod_son);
    printf (" [val=%ld], log10 (%ld/%ld) = */ %f,\n", val_son, nb, total_nb, log10 ((double)nb / (double)total_nb));
  }

  fputs ("};\n\n", stdout);

  /* On n'utilise que la prod du pere comme ctxt (et non son vecteur de derivation ds le vide) */
  XxY_lock (&prod_ctxtXeprod_hd);
  top = XxY_top (prod_ctxtXeprod_hd);
  fputs ("\n\n/* Partial context case, with father production */\n", stdout);
  XxY_to_c (&prod_ctxtXeprod_hd, stdout, "prod_ctxtXeprod_hd", false /* external */);

  printf ("\ndouble prod_ctxtXeprod2logprobs [%ld] = {%f,\n", top+1, min_logprob);

  for (couple = 1; couple <= top; couple++) {
    prod_father = XxY_X (prod_ctxtXeprod_hd, couple);
    eprod_son = XxY_Y (prod_ctxtXeprod_hd, couple);
    nb = prod_ctxtXeprod2nb [couple]; /* local pour prod ctxt */
    total_nb = prod_father2nb [prod_father]; /* total pour prod ctxt */

    prod_son = XxY_X (prodXval2eprod_hd, eprod_son);
    val_son = XxY_Y (prodXval2eprod_hd, eprod_son);

    B = lhs [prod_son];
    total_nb = B2nb [B]; /* nouveau denominateur commun */
    
    printf ("/* %ld:\tfather %ld=", couple, prod_father);
    print_prod (prod_father);
    
    printf ("\tson %ld=", prod_son);
    print_prod (prod_son);
    printf (" [val=%ld], log10 (%ld/%ld) = */ %f,\n", val_son, nb, total_nb, log10 ((double)nb / (double)total_nb));
  }

  fputs ("};\n\n", stdout);


  /* On n'utilise que le nt en lhs du pere comme ctxt */
  XxY_lock (&A_ctxtXeprod_hd);
  top = XxY_top (A_ctxtXeprod_hd);
  fputs ("\n\n/* Partial context case, with the lhs father production */\n", stdout);
  XxY_to_c (&A_ctxtXeprod_hd, stdout, "A_ctxtXeprod_hd", false /* external */);

  printf ("\n\ndouble A_ctxtXeprod2logprobs [%ld] = {%f,\n", top+1, min_logprob);

  for (couple = 1; couple <= top; couple++) {
    A = XxY_X (A_ctxtXeprod_hd, couple);
    eprod_son = XxY_Y (A_ctxtXeprod_hd, couple);
    nb =  A_ctxtXeprod2nb [couple]; /* local pour A ctxt */
    total_nb = A2nb [A]; /* total pour A ctxt */

    prod_son = XxY_X (prodXval2eprod_hd, eprod_son);
    val_son = XxY_Y (prodXval2eprod_hd, eprod_son);

    B = lhs [prod_son];
    total_nb = B2nb [B]; /* nouveau denominateur commun */
    
    printf ("/* %ld : %ld=", couple, A);
    print_X (A);
    
    printf (", son %ld=", prod_son);
    print_prod (prod_son);
    printf (" [val=%ld], log10 (%ld/%ld) = */ %f,\n", val_son, nb, total_nb, log10 ((double)nb / (double)total_nb));
  }

  fputs ("};\n\n", stdout);

  /* On n'utilise que le nt en lhs du pere comme ctxt */
  top = XxY_top (prodXval2eprod_hd);
  fputs ("\n\n/* No context case, with val */\n", stdout);

  printf ("\ndouble eprod2logprobs [%ld] = {%f,\n", top+1, min_logprob);

  for (eprod_son = 1; eprod_son <= top; eprod_son++) {
    prod_son = XxY_X (prodXval2eprod_hd, eprod_son);
    val_son = XxY_Y (prodXval2eprod_hd, eprod_son);
    nb =  eprod_son2nb [eprod_son]; /* local pour pas de ctxt eprod */
    total_nb = prod_son2nb [prod_son]; /* local pour pas de ctxt prod et total pour pas de ctxt eprod */

    B = lhs [prod_son];
    total_nb = B2nb [B]; /* nouveau denominateur commun */
    
    printf ("/* %ld: son %ld=", eprod_son, prod_son);
    print_prod (prod_son);

    logprob = (nb == 0) ? min_logprob : log10 ((double)nb / (double)total_nb);

    printf (" [val=%ld], log10 (%ld/%ld) = */ %f,\n", val_son, nb, total_nb, logprob);
  }

  fputs ("};\n\n", stdout);

  fputs ("\n\n/* No context case, without val */\n", stdout);
  fputs ("\ndouble prod2logprobs [PROD_NB+1] = {0,\n", stdout);

  for (prod_son = 1; prod_son <= prodmax; prod_son++) {
    B = lhs [prod_son];
    nb =  prod_son2nb [prod_son]; /* local pour pas de ctxt prod et total pour pas de ctxt eprod */
    total_nb = B2nb [B]; /* total pour pas de ctxt prod */

    printf ("/* %ld: ", prod_son);
    print_prod (prod_son);

    logprob = (nb == 0) ? min_logprob : log10 ((double)nb / (double)total_nb);

    printf (" log10 (%ld/%ld) = */ %f,\n", nb, total_nb, logprob);
  }

  fputs ("};\n\n", stdout);


  /* Sortie des structures pour la desambiguation lexicale */

  XxY_lock (&full_ctxtXlex_id_hd);
  top = XxY_top (full_ctxtXlex_id_hd);
  fputs ("\n\n/* Full contextual lexical desambiguation probabilities */\n", stdout);
  XxY_to_c (&full_ctxtXlex_id_hd, stdout, "full_ctxtXlex_id_hd", false /* external */);

  printf ("\n\ndouble full_ctxtXlex_id2logprobs [%ld] = {0,\n", top+1);

  for (couple = 1; couple <= top; couple++) {
    nb = full_ctxtXlex_id2nb [couple];

    full_ctxt = XxY_X (full_ctxtXlex_id_hd, couple);
    lex_id = XxY_Y (full_ctxtXlex_id_hd, couple);

    ff_id = XxY_X (ff_idXt2lex_id_hd, lex_id);
    t = XxY_Y (ff_idXt2lex_id_hd, lex_id);

    single = X_is_set (&ff_id_hd, ff_id);
    total_nb = ff_id2nb [single];

    eprod_father = XxY_X (full_ctxt_hd, full_ctxt);
    pos = XxY_Y (full_ctxt_hd, full_ctxt);

    prod_father = XxY_X (prodXval2eprod_hd, eprod_father);
    val_father = XxY_Y (prodXval2eprod_hd, eprod_father);
    
    printf ("/* %ld:\tfather %ld=", couple, prod_father);
    print_prod (prod_father);
    printf (" [val=%ld]\n", val_father);
    
    printf ("\tson %ld=\"%s\" at pos %ld, log10 (%ld/%ld) = */ %f,\n", t, tstring [t], pos, nb, total_nb, log10 ((double)nb / (double)total_nb));
  }

  fputs ("};\n\n", stdout);


  XxY_lock (&eprod_ctxtXlex_id_hd);
  top = XxY_top (eprod_ctxtXlex_id_hd);
  fputs ("\n\n/* Eprod contextual lexical desambiguation probabilities */\n", stdout);
  XxY_to_c (&eprod_ctxtXlex_id_hd, stdout, "eprod_ctxtXlex_id_hd", false /* external */);

  printf ("\n\ndouble eprod_ctxtXlex_id2logprobs [%ld] = {0,\n", top+1);

  for (couple = 1; couple <= top; couple++) {
    nb = eprod_ctxtXlex_id2nb [couple];

    eprod_father = XxY_X (eprod_ctxtXlex_id_hd, couple);
    lex_id = XxY_Y (eprod_ctxtXlex_id_hd, couple);

    ff_id = XxY_X (ff_idXt2lex_id_hd, lex_id);
    t = XxY_Y (ff_idXt2lex_id_hd, lex_id);

    single = X_is_set (&ff_id_hd, ff_id);
    total_nb = ff_id2nb [single];

    prod_father = XxY_X (prodXval2eprod_hd, eprod_father);
    val_father = XxY_Y (prodXval2eprod_hd, eprod_father);
    
    printf ("/* %ld:\tfather %ld=", couple, prod_father);
    print_prod (prod_father);
    printf (" [val=%ld]\n", val_father);
    
    printf ("\tson %ld=\"%s\", log10 (%ld/%ld) = */ %f,\n", t, tstring [t], nb, total_nb, log10 ((double)nb / (double)total_nb));
  }

  fputs ("};\n\n", stdout);


  XxY_lock (&prod_ctxtXlex_id_hd);
  top = XxY_top (prod_ctxtXlex_id_hd);
  fputs ("\n\n/* Prod contextual lexical desambiguation probabilities */\n", stdout);
  XxY_to_c (&prod_ctxtXlex_id_hd, stdout, "prod_ctxtXlex_id_hd", false /* external */);

  printf ("\n\ndouble prod_ctxtXlex_id2logprobs [%ld] = {0,\n", top+1);

  for (couple = 1; couple <= top; couple++) {
    nb = prod_ctxtXlex_id2nb [couple];

    prod_father = XxY_X (prod_ctxtXlex_id_hd, couple);
    lex_id = XxY_Y (prod_ctxtXlex_id_hd, couple);

    ff_id = XxY_X (ff_idXt2lex_id_hd, lex_id);
    t = XxY_Y (ff_idXt2lex_id_hd, lex_id);

    single = X_is_set (&ff_id_hd, ff_id);
    total_nb = ff_id2nb [single];
    
    printf ("/* %ld:\tfather %ld=", couple, prod_father);
    print_prod (prod_father);
    
    printf ("\n\tson %ld=\"%s\", log10 (%ld/%ld) = */ %f,\n", t, tstring [t], nb, total_nb, log10 ((double)nb / (double)total_nb));
  }

  fputs ("};\n\n", stdout);


  XxY_lock (&A_ctxtXlex_id_hd);
  top = XxY_top (A_ctxtXlex_id_hd);
  fputs ("\n\n/* Lhs contextual lexical desambiguation probabilities */\n", stdout);
  XxY_to_c (&A_ctxtXlex_id_hd, stdout, "A_ctxtXlex_id_hd", false /* external */);

  printf ("\n\ndouble A_ctxtXlex_id2logprobs [%ld] = {0,\n", top+1);

  for (couple = 1; couple <= top; couple++) {
    nb = A_ctxtXlex_id2nb [couple];

    A = XxY_X (A_ctxtXlex_id_hd, couple);
    lex_id = XxY_Y (A_ctxtXlex_id_hd, couple);

    ff_id = XxY_X (ff_idXt2lex_id_hd, lex_id);
    t = XxY_Y (ff_idXt2lex_id_hd, lex_id);

    single = X_is_set (&ff_id_hd, ff_id);
    total_nb = ff_id2nb [single];
    
    printf ("/* %ld: father's lhs %ld=", couple, A);
    print_X (A);
    
    printf (", son %ld=\"%s\", log10 (%ld/%ld) = */ %f,\n", t, tstring [t], nb, total_nb, log10 ((double)nb / (double)total_nb));
  }

  fputs ("};\n\n", stdout);

  XxY_lock (&ff_idXt2lex_id_hd);
  top = XxY_top (ff_idXt2lex_id_hd);
  fputs ("\n\n/* Context-free lexical desambiguation probabilities */\n", stdout);
  XxY_to_c (&ff_idXt2lex_id_hd, stdout, "ff_idXt2lex_id_hd", false /* external */);

  printf ("\n\ndouble lex_id2logprobs [%ld] = {0,\n", top+1);

  for (lex_id = 1; lex_id <= top; lex_id++) {
    nb = lex_id2nb [lex_id];

    ff_id = XxY_X (ff_idXt2lex_id_hd, lex_id);
    t = XxY_Y (ff_idXt2lex_id_hd, lex_id);

    single = X_is_set (&ff_id_hd, ff_id);
    total_nb = ff_id2nb [single];
    
    printf ("/* %ld: %ld=\"%s\", log10 (%ld/%ld) = */ %f,\n", lex_id, t, tstring [t], nb, total_nb, log10 ((double)nb / (double)total_nb));
  }

  fputs ("};\n\n", stdout);


  printf ("\nint  max_signature_depth = %ld;\n\n", max_signature_depth);

  /* XH_lock (&fs_signatures_hd); non !! */
  /* On reajuste plutot list_size */
  XH_list_size (fs_signatures_hd) = XH_list_top (fs_signatures_hd) + max_fs_signature_size+1 /* pour permettre les XH_is_set sans reallocation */;
  fputs ("\n\n/* Full contextual f-structures desambiguation probabilities */\n", stdout);
  XH2c (&fs_signatures_hd, stdout, "fs_signatures_hd", false /* external */);
  
  XxY_lock (&ctxtXsign_hd);
  top = XxY_top (ctxtXsign_hd);
  XxY_to_c (&ctxtXsign_hd, stdout, "ctxtXsign_hd", false /* external */);

  printf ("\n\ndouble ctxtXsign2logprobs [%ld] = {0,\n", top+1);

  min_logprob_fs = 0;

  for (couple = 1; couple <= top; couple++) {
    nb = ctxtXsign2nb [couple];

    /*
    full_couple = XxY_X (ctxtXsign_hd, couple);
    signature = XxY_Y (ctxtXsign_hd, couple);

    full_ctxt = XxY_X (full_ctxtXeprod_hd, full_couple);
    eprod_son = XxY_Y (full_ctxtXeprod_hd, full_couple);

    eprod_father = XxY_X (full_ctxt_hd, full_ctxt); 
    pos = XxY_Y (full_ctxt_hd, full_ctxt);

    prod_father = XxY_X (prodXval2eprod_hd, eprod_father);
    val_father = XxY_Y (prodXval2eprod_hd, eprod_father);
    */

    eprod_son = XxY_X (ctxtXsign_hd, couple);
    signature = XxY_Y (ctxtXsign_hd, couple);

    prod_son = XxY_X (prodXval2eprod_hd, eprod_son);
    val_son = XxY_Y (prodXval2eprod_hd, eprod_son);

    B = lhs [prod_son];
    total_nb = B2sign_nb [B];
    
    printf ("/* %ld:\tprod %ld=", couple, prod_son);
    print_prod (prod_son);
    logprob = log10 ((double)nb / (double)total_nb);
    printf (" [val=%ld] with signature %ld, log10 (%ld/%ld) = */ %f,\n",
	    val_son, signature, nb, total_nb, logprob);

    if (logprob < min_logprob_fs)
      min_logprob_fs = logprob;
  }

  fputs ("};\n\n", stdout);

  printf ("double min_logprob_fs = %f;\n\n", min_logprob_fs);

  XxY_free (&prodXval2eprod_hd);
  sxfree (eprod_father2nb), eprod_father2nb = NULL;
  sxfree (eprod_son2nb), eprod_son2nb = NULL;
  sxfree (A2nb), A2nb = NULL;
  sxfree (B2nb), B2nb = NULL;
  sxfree (B2sign_nb), B2sign_nb = NULL;
  sxfree (prod_father2nb), prod_father2nb = NULL;
  sxfree (prod_son2nb), prod_son2nb = NULL;
  XxY_free (&full_ctxt_hd);
  sxfree (full_ctxt2nb), full_ctxt2nb = NULL;
  XxY_free (&full_ctxtXeprod_hd);
  sxfree (full_ctxtXeprod2nb), full_ctxtXeprod2nb = NULL;
  XxY_free (&eprod_ctxtXeprod_hd);
  sxfree (eprod_ctxtXeprod2nb), eprod_ctxtXeprod2nb = NULL;
  XxY_free (&prod_ctxtXeprod_hd);
  sxfree (prod_ctxtXeprod2nb), prod_ctxtXeprod2nb = NULL;
  XxY_free (&A_ctxtXeprod_hd);
  sxfree (A_ctxtXeprod2nb), A_ctxtXeprod2nb = NULL;
  X_free (&ff_id_hd);
  sxfree (ff_id2nb), ff_id2nb = NULL;
  XxY_free (&full_ctxtXlex_id_hd);
  sxfree (full_ctxtXlex_id2nb), full_ctxtXlex_id2nb = NULL;
  XxY_free (&eprod_ctxtXlex_id_hd);
  sxfree (eprod_ctxtXlex_id2nb), eprod_ctxtXlex_id2nb = NULL;
  XxY_free (&prod_ctxtXlex_id_hd);
  sxfree (prod_ctxtXlex_id2nb), prod_ctxtXlex_id2nb = NULL;
  XxY_free (&A_ctxtXlex_id_hd);
  sxfree (A_ctxtXlex_id2nb), A_ctxtXlex_id2nb = NULL;
  XxY_free (&ff_idXt2lex_id_hd);
  sxfree (lex_id2nb), lex_id2nb = NULL;
  XH_free (&fs_signatures_hd);
  XxY_free (&ctxtXsign_hd);
  sxfree (ctxtXsign2nb), ctxtXsign2nb = NULL;
}


static char	Usage [] = "\
\t-msd #value, -max_signature_depth #value (default -msd 0)\n\
";

#define OPTION(opt)	(1 << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB ((sizeof(option_tbl)/sizeof(*option_tbl))-1)

#define UNKNOWN_ARG 	    0
#define MAX_SIGNATURE_DEPTH 1

static char	*option_tbl [] = {
    "",
    "msd", "max_signature_depth",
};

static SXINT	option_kind [] = {
    UNKNOWN_ARG,
    MAX_SIGNATURE_DEPTH, MAX_SIGNATURE_DEPTH,
};

static SXINT	option_get_kind (char *arg)
{
  char	**opt;

  if (*arg++ != '-')
    return UNKNOWN_ARG;

  if (*arg == SXNUL)
    return UNKNOWN_ARG;

  for (opt = &(option_tbl [OPT_NB]); opt > option_tbl; opt--) {
    if (strcmp (*opt, arg) == 0 /* egalite */ )
      break;
  }

  return option_kind [opt - option_tbl];
}



static char	*option_get_text (SXINT	kind)
{
  SXINT	i;

  for (i = OPT_NB; i > 0; i--) {
    if (option_kind [i] == kind)
      break;
  }

  return option_tbl [i];
}


int main (int argc, char *argv [])
{
  SXINT	argnum;

  sxuse (Usage);
  sxopentty ();

  if (argc > 1) {
    argnum = 0;

    while (++argnum < argc) {
      switch (option_get_kind (argv [argnum])) {

      case MAX_SIGNATURE_DEPTH:
	if (++argnum >= argc) {
	  fprintf (sxstderr, "%s: an integer number must follow the \"%s\" option;\n", ME, option_get_text (MAX_SIGNATURE_DEPTH));
	  return 1;
	}

	max_signature_depth = atoi (argv [argnum]);
	break;

      case UNKNOWN_ARG:
      default:
	return 1;
      }
    }
  }


#if BUG
  /* Suppress bufferisation, in order to have proper	 */
  /* messages when something goes wrong...		 */
  setbuf (stdout, NULL);
#endif

  process_weights ();

  sxexit (sxerr_max_severity ());
}
