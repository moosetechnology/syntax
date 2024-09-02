/* ********************************************************************
   *  This program has been generated from xtag.at                    *
   *  on Fri Dec 30 13:04:24 1994                                     *
   *  by the SYNTAX (*) abstract tree constructor SEMAT               *
   ********************************************************************
   *  (*) SYNTAX is a trademark of INRIA.                             *
   ******************************************************************** */



/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1994 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *     Produit de l'equipe ATOLL.			  *
   *                                                      *
   ******************************************************** */

/* Genere a partir d'une XTAG
   - la LIG]BNF correspondante
*/

/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 30-12-94 13:12 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/

char WHAT[] = "@(#)xtag_smp.c	- SYNTAX [unix] - Mer 18 Jan 1995 10:30:38";

static char	ME [] = "xtag_smp";




/*   I N C L U D E S   */
#define SXNODE struct xtag_node
#include "sxunix.h"
#include "xtag.h"
#include "SS.h"
#include "varstr.h"
#include "XxY.h"
#include "sxba.h"


struct xtag_node {
    SXNODE_HEADER_S SXVOID_NAME;
};
/*
N O D E   N A M E S
*/
#define ERROR_n 1
#define ATTRIBUT_S_n 2
#define FOOTP_n 3
#define HEADP_n 4
#define NA_n 5
#define NIL_n 6
#define NODE_NAME_n 7
#define NODE_S_n 8
#define OA_n 9
#define SA_n 10
#define SUBSTP_n 11
#define TAG_n 12
#define TREE_n 13
#define TREE_S_n 14
#define TRUE_n 15
#define VOID_n 16
#define string_n 17
/*
E N D   N O D E   N A M E S
*/

#define NA_c     01
#define OA_c     02
#define SA_c     04
#define FOOT_c   010
#define SUBST_c  020
#define KNEE_c   040


static int           tree_nb;
static int           current_tree_index;
static unsigned int  current_root_name;
static int	     current_root_address;
static unsigned int  current_node_name;
static int           current_node_address;

static unsigned int  NA_ste;
static unsigned int  axiome_ste;

static XxY_header    node_address;

static struct node_address2attr 
{
  unsigned int          node_name;
  
  int                   sons_nb;

  unsigned int          kind;

  struct sxsource_coord source_index;

  bool               is_on_spine;
} *node_address2attr;


static struct tree2attr 
{
  bool      is_aux;
  
}                   *tree2attr;

static SXBA	    aux_set;

static int          *axiomes;

static VARSTR       local_address;

  
static void
node_address_oflw (old_size, new_size)
  int old_size, new_size;
{
  node_address2attr = (struct node_address2attr*) sxrealloc (node_address2attr,
							     new_size + 1,
							     sizeof (struct node_address2attr));
  
}



static void
xtag_first_pass (visited)
  SXNODE *visited;
{
  SXNODE         *son = visited->son;
  unsigned int *pkind;
  int          father_node_address, son_node_address, indice;

  switch (visited->name) {

  case ERROR_n :
    return;

  case ATTRIBUT_S_n :
    
    node_address2attr [current_node_address].kind = 0;
    node_address2attr [current_node_address].is_on_spine = false;
    
    while (son != NULL) 
      {
	xtag_first_pass (son);
	son = son->brother;
      }

    pkind = &(node_address2attr [current_node_address].kind);

    if (*pkind & FOOT_c) 
      {
	node_address2attr [current_node_address].is_on_spine = true;

	if ((*pkind & NA_c) == 0)
	  {
	    sxput_error (visited->token.source_index,
			 "%sA No Adjoining Constraint must be specified.",
			 sxtab_ptr->err_titles [2]);
	  }

	if ((node_address2attr [current_root_address].kind & NA_c) == 0)
	  {
	    sxput_error (node_address2attr [current_root_address].source_index,
			 "%sA No Adjoining Constraint must be specified on the root.",
			 sxtab_ptr->err_titles [2]);
	  }
      }
    
    if (*pkind & (SA_c | NA_c))
      {
	if (((*pkind & SA_c) && (*pkind & NA_c)) == 0)
	  sxput_error (visited->token.source_index,
		       "%sConstraints and Constraints-type are not compatible.",
		       sxtab_ptr->err_titles [2]);
      }

    return;

  case FOOTP_n :
    if (visited->son->name == TRUE_n) 
      {
	    /* Ce doit etre une feuille. */
	if (visited->father->brother != NULL) 
	  {
	    sxput_error (visited->son->token.source_index,
			 "%sShould be a leaf.",
			 sxtab_ptr->err_titles [2]);
	  }

	    /* Meme nom que la racine ? */
	if (current_node_name != current_root_name)
	  {
	    sxput_error (visited->son->token.source_index,
			 "%sNot a foot node.",
			 sxtab_ptr->err_titles [2]);
	  }
	else
	  {
	    if (tree2attr [current_tree_index].is_aux) 
	      {
		sxput_error (visited->father->token.source_index,
			 "%sMultiple foot node.",
			 sxtab_ptr->err_titles [2]);
	      }
	    else 
	      {
		node_address2attr [current_node_address].kind |= FOOT_c;
		tree2attr [current_tree_index].is_aux = true;
		SXBA_1_bit (aux_set, current_root_name);
	      }
	  }
	
	    /* On verifiera la contrainte NA sur le pere */

      }
   
    return;

  case HEADP_n :
    return;

  case NA_n :
    pkind = &(node_address2attr [current_node_address].kind);
  
    if ((*pkind & NA_c) ||
	(*pkind & OA_c))
      sxput_error (visited->token.source_index,
		   "%sThis Constraint is not compatible with a previous one.",
		   sxtab_ptr->err_titles [1]);
    else
      *pkind |= NA_c;

    return;

  case NIL_n :
    return;

  case NODE_NAME_n :
    current_node_name = visited->son->token.string_table_entry;
    node_address2attr [current_node_address].node_name = current_node_name;
    node_address2attr [current_node_address].source_index = visited->son->token.source_index;

    if (current_root_name == SXEMPTY_STE) 
      {
	    /* C'est la racine */
	current_root_name = current_node_name;
	current_root_address = current_node_address;
      }
    
    return;

  case NODE_S_n :
    father_node_address = current_node_address;
    node_address2attr [father_node_address].sons_nb = visited->degree - 1;

    while (son != NULL) 
      {
	if (son->position != 1)
	  XxY_set (&node_address, father_node_address, son->position - 1, &current_node_address);
	
	son_node_address = current_node_address;

	xtag_first_pass (son);

	if (son->position != 1 && node_address2attr [son_node_address].is_on_spine)
	  {
	    node_address2attr [father_node_address].is_on_spine = true;

	    if (node_address2attr [son_node_address].kind & FOOT_c)
	      node_address2attr [father_node_address].kind |= KNEE_c;
	  }

	son = son->brother;
      }

    return;

  case OA_n :
    pkind = &(node_address2attr [current_node_address].kind);

    if ((*pkind & NA_c) ||
	(*pkind & OA_c) ||
	(*pkind & SA_c))
      sxput_error (visited->son->token.source_index,
		   "%sThis Constraint is not compatible with a previous one.",
		   sxtab_ptr->err_titles [1]);
    else
      *pkind |= OA_c;

    return;

  case SA_n :
    if (visited->token.string_table_entry != SXEMPTY_STE) 
      {
	    /* Selective Adjoining Constraint */
	if (visited->token.string_table_entry != NA_ste) 
	  {
	    sxput_error (visited->token.source_index,
			 "%sOnly No Adjoining Constraint are processed.",
			 sxtab_ptr->err_titles [2]);
	  }
	else 
	  {
	    pkind = &(node_address2attr [current_node_address].kind);

	    if ((*pkind & SA_c) ||
		(*pkind & OA_c))
	      sxput_error (visited->token.source_index,
			   "%sThis Constraint is not compatible with a previous one.",
			   sxtab_ptr->err_titles [1]);
	    else
	      *pkind |= SA_c;
	  }
      }
    
    return;

  case SUBSTP_n :
    if (visited->son->name == TRUE_n) 
      {
	    /* Ce doit etre une feuille. */
	if (visited->father->brother != NULL) 
	  {
	    sxput_error (visited->son->token.source_index,
			 "%sShould be a leaf.",
			 sxtab_ptr->err_titles [2]);
	  }
	else
	  node_address2attr [current_node_address].kind |= SUBST_c;
      }
    
    return;

  case TAG_n :
    break;

  case TREE_n :
    current_root_name = SXEMPTY_STE;
    current_tree_index++;
    XxY_set (&node_address, current_tree_index, 0, &current_node_address);
    tree2attr [current_tree_index].is_aux = false; /* A priori */
    
    xtag_first_pass (son);

    if (current_root_name == axiome_ste && !tree2attr [current_tree_index].is_aux)
      SS_push (axiomes, current_tree_index);

    return;

  case TREE_S_n :
    while (son != NULL) 
      {
	xtag_first_pass (son);
	son = son->brother;
      }
    
    return;

  case TRUE_n :
    return;

  case VOID_n :
    return;

  case string_n :
    return;

  default:
    return;
  }
   
  xtag_first_pass (son);
}


static void
process_adjunction (tree_nb, lhs_address, string_address)
  int tree_nb, lhs_address;
  char *string_address;
{
  /* On est sur le noeud d'adresse lhs_address dans l'arbre tree_nb,
     on traite les adjonctions eventuelles a ce noeud. */
  char *A;
  struct node_address2attr *pattr = node_address2attr + lhs_address;

  if (pattr->kind & (NA_c + SUBST_c))
    return;
  
  /* On regarde s'il existe (au moins) un arbre auxiliaire dont la racine a le nom pattr->node_name */
  
  if (!SXBA_bit_is_set (aux_set, pattr->node_name))
    {
      /* Il n'y en a pas */
      if (pattr->kind & OA_c)
	sxput_error (pattr->source_index,
		     "%sThe Obligatory Adjoining Constraint cannot be satisfied.",
		     sxtab_ptr->err_titles [2]);

      return;
    }

  /* En XTAG, aucune adjonction sur une feuille */

  /* On ne sait pas generer les adjonctions sur les racines !!! (grammaire circulaire). */

  /* Racine,  initial             : <A.n.0.t> = <A.t> ( ) ;           
                                  : <A.b> ( ) = <A.n.0.b> ;
     interne, initial             : <A.address.t> = <A.t> ( ) ;   
                                  : <A.b> ( ) = <A.address.b> ;

     Racine,  aux                 : erreur
                                  : erreur
     interne, aux, !is_on_spine   : <A.address.t> = <A.t> ( ) ;   
                                  : <A.b> ( ) = <A.address.b> ;
     interne, aux, is_on_spine    : <A.address.t> ( .. tree_nb ) = <A.t> ( .. tree_nb ) ;
                                  : <A.b> ( .. ) = <A.address.b> ( .. ) ;
  */

  A = sxstrget (pattr->node_name);
  
  /* On genere en premier le raccord entre le "top" de l'arbre adjoint et le noeud d'adjonction. */

  if (tree2attr [tree_nb].is_aux)
    {
      /* adjonction dans un arbre auxiliaire */
      if (*string_address != SXNUL)
	{
	  /* Noeud interne */
	  printf ("<%s.%i%s.t>", A, tree_nb, string_address);

	  if (pattr->is_on_spine)
	    printf (" ( .. %i )\t= <%s.t> ( .. %i ) ;\n", tree_nb, A, tree_nb);
	  else
	    printf ("\t= <%s.t> ( ) ;\n", A);
	}
      /* else adjonction sur la racine, erreur (deja signalee) */
    }
  else
    {
      /* adjonction dans un arbre initial */
      printf ("<%s.%i", A, tree_nb);

      if (*string_address != SXNUL)
	/* Noeud interne */
	printf ("%s", string_address);
      else
	fputs (".0", stdout);

      printf (".t>\t= <%s.t> ( ) ;\n", A);
    }
  
  if (tree2attr [tree_nb].is_aux)
    {
      /* adjonction dans un arbre auxiliaire */
      if (*string_address != SXNUL)
	{
	  /* Noeud interne */
	  if (pattr->is_on_spine)
	    printf ("<%s.b> ( .. )\t= <%s.%i%s.b> ( .. ) ;\n", A, A, tree_nb, string_address);
	  else
	    printf ("<%s.b> ( )\t= <%s.%i%s.b> ( ) ;\n", A, A, tree_nb, string_address);
	}
	/* else Racine (erreur) */
    }
  else
    {
      /* adjonction dans un arbre initial */
      printf ("<%s.b> ( )\t= <%s.%i", A, A, tree_nb);

      if (*string_address != SXNUL)
	/* Noeud interne */
	printf ("%s", string_address);
      else
	fputs (".0", stdout);

      fputs (".b> ;\n", stdout);
    }
}



static void
process_lhs (tree_nb, lhs_address, string_address)
  int tree_nb, lhs_address;
  char *string_address;
{
  char	*A;
  struct node_address2attr *pattr = node_address2attr + lhs_address;

  /* Si OA et !AUX => Erreur */

      /* On genere si :
	 Initial, Racine,                           !AUX 	: "<A> = "
	 Initial, Racine,                !OA         AUX 	: "<A> = <A.n.0.t> ;"
	 							: "<A.n.0.t> = <A.n.0.b> ;"
	 							: "<A.n.0.b> = "
	 Initial, Racine,                 OA         AUX 	: "<A> = <A.n.0.t> ;"
	 							: "<A.n.0.b> = "
	 Initial, Interne                           !AUX 	: "<A.address> = "
	 Initial, Interne                !OA         AUX 	: "<A.address.t> = <A.address.b> ( ) ;"
	 							: "<A.address.b> ( ) = "
	 Initial, Interne                 OA         AUX 	: "<A.address.b> ( ) = "

	 Aux,     Racine,                            NA 	: "<A.t> ( .. ) = "
	 Aux,	  Interne   !is_on_spine,!OA        !AUX 	: "<A.address> = "
	 Aux,     Interne   !is_on_spine,!OA         AUX 	: "<A.address.t> = <A.address.b> ( ) ;"
	 							: "<A.address.b> ( ) = "
	 Aux,     Interne   !is_on_spine, OA         AUX 	: "<A.address.b> ( ) = "

	 Aux,     Interne,  is_on_spine, !OA, !KNEE !AUX	: "<A.address> ( .. ) = "
	 Aux,     Interne,  is_on_spine, !OA  !KNEE  AUX 	: "<A.address.t> ( .. ) = 
	                                                           <A.address.b> ( .. ) ;"
	 							: "<A.address.b> ( .. ) = "
	 Aux,     Interne,  is_on_spine,  OA  !KNEE  AUX 	: "<A.address.b> ( .. ) = "
	 Aux,     Interne,  is_on_spine, !OA,  KNEE !AUX	: "<A.address.b> ( .. tree_nb ) = "
	 Aux,     Interne,  is_on_spine, !OA   KNEE  AUX 	: "<A.address.t> ( .. ) = 
	                                                           <A.address.b> ( .. ) ;"
	 							: "<A.address.b> ( .. tree_nb ) = "
	 Aux,     Interne,  is_on_spine,  OA   KNEE  AUX 	: "<A.address.b> ( .. tree_nb ) = "
      */

  printf ("<%s", A = sxstrget (pattr->node_name));
  
  if (tree2attr [tree_nb].is_aux)
    {
      /* Arbre auxiliaire */
      if (*string_address == SXNUL)
	{
	  /* Noeud racine */
	  fputs (".t> ( .. )", stdout);
	}
      else
	{
	  /* Noeud interne */
	  if (pattr->is_on_spine)
	    {
	      /* Sur l'arete */
	      if ((pattr->kind & NA_c) || !SXBA_bit_is_set (aux_set, pattr->node_name))
		{
		  /* Pas d'adjonction */
		  printf (".%i%s> ( .. ", tree_nb, string_address);

		  if (pattr->kind & KNEE_c)
		    /* Pere du pied */
		    printf ("%i ", tree_nb);

		  fputs (")", stdout);
		}
	      else
		{
		  /* adjonction... */
		  if ((pattr->kind & OA_c) == 0)
		    {
		      /* ... non obligatoire */
		      printf (".%i%s.t> ( .. )\t= <%s.%i%s.b> ( .. ) ;\n<%s", tree_nb,
			  string_address, A, tree_nb, string_address, A);
		    }
		  
		  printf (".%i%s.b> ( .. ", tree_nb, string_address);

		  if (pattr->kind & KNEE_c)
		    {
		      /* Pere du pied */
		      printf ("%i ", tree_nb);
		    }

		  fputs (")", stdout);
		}
	    }
	  else
	    {
	      /* Hors de l'arete */
	      if ((pattr->kind & NA_c) || !SXBA_bit_is_set (aux_set, pattr->node_name))
		{
		  /* Pas d'adjonction */
		  printf (".%i%s>", tree_nb, string_address);
		}
	      else
		{
		  /* adjonction */
		  if ((pattr->kind & OA_c) == 0)
		    {
		      /* non obligatoire */
		      printf (".%i%s.t>\t= <%s.%i%s.b> ( ) ;\n<%s", tree_nb, string_address, A,
		      tree_nb, string_address, A);
		    }

		  printf (".%i%s.b> ( )", tree_nb, string_address);
		}
	    }
	}
    }
  else
    {
	  /* Arbre initial */
      if (*string_address == SXNUL)
	{
	  /* Noeud racine */
	  if ((pattr->kind & NA_c) || !SXBA_bit_is_set (aux_set, pattr->node_name))
	    {
	      /* Pas d'adjonction */
	      fputs (">", stdout);
	    }
	  else 
	    {
	      /* adjonction */
	      printf (">\t= <%s.%i.0.t> ;\n<%s", A, tree_nb, A);

	      if ((pattr->kind & OA_c) == 0)
		{
		  /* non obligatoire */
		  printf (".%i.0.t>\t= <%s.%i.0.b> ;\n<%s", tree_nb, A, tree_nb, A);
		}

	      printf (".%i.0.b>", tree_nb);
	    }
	}
      else 
	{
	  /* Noeud interne */
	  if ((pattr->kind & NA_c) || !SXBA_bit_is_set (aux_set, pattr->node_name))
	    {
	      /* Pas d'adjonction */
	      printf (".%i%s>", tree_nb, string_address);
	    }
	  else 
	    {
	      if ((pattr->kind & OA_c) == 0)
		{
		  printf (".%i%s.t>\t= <%s.%i%s.b> ( ) ;\n<%s", tree_nb, string_address, A,
		  tree_nb, string_address, A);
		}

	      printf (".%i%s.b> ( )", tree_nb, string_address);
	    }
	}
    }

  fputs ("\t= ", stdout);
}


static void
process_rhs (tree_nb, rhs_address, string_address)
  int tree_nb, rhs_address;
  char *string_address;
  {
    int position, lhs_address;
    struct node_address2attr *pattr = node_address2attr + rhs_address;
  
    /* On genere 
       Si c'est un T :                                   	"\"t\" "

       Si initial, feuille, !AUX                         	<A>
       Si initial, interne, !AUX                         	<A.address>
       Si initial, interne,  AUX                         	<A.address.t>

       Si aux, feuille  !is_on_spine                     	<A>
       Si aux, feuille   is_on_spine                     	<A.b> ( .. )
       Si aux, interne, !is_on_spine  !AUX               	<A.address>
       Si aux, interne, !is_on_spine   AUX               	<A.address.t>
       Si aux, interne,  is_on_spine, !AUX, father == ROOT      <A.address> ( .. tree_nb )
       Si aux, interne,  is_on_spine, !AUX, father != ROOT      <A.address> ( .. )
       Si aux, interne,  is_on_spine,  AUX, father == ROOT      <A.address.t> ( .. tree_nb )
       Si aux, interne,  is_on_spine,  AUX, father != ROOT      <A.address.t> ( .. )
       */
  
    if (pattr->sons_nb == 0 && (pattr->kind & (SUBST_c + FOOT_c)) == 0)
      /* terminal */
      printf ("\"%s\" ", sxstrget (pattr->node_name));
    else
      {
	/* Non terminal */
	position = XxY_Y (node_address, rhs_address);
	printf ("<%s", sxstrget (pattr->node_name));
  
	if (tree2attr [tree_nb].is_aux)
	  {
	    /* Arbre auxiliaire */
	    if (pattr->sons_nb == 0) 
	      {
		/* Feuille */
		if (pattr->is_on_spine)
		  fputs (".b> ( .. ) ", stdout);
		else
		  fputs ("> ", stdout);
	      }
	    else
	      {
		/* Noeud interne */
		printf (".%i%s.%i", tree_nb, string_address, position);

		if (SXBA_bit_is_set (aux_set, pattr->node_name))
		  fputs (".t", stdout);

		fputs ("> ", stdout);

		if (pattr->is_on_spine)
		  {
		    lhs_address = XxY_X (node_address, rhs_address);
		  
		    if (XxY_Y (node_address, lhs_address) == 0)
		      /* Le LHS est racine de l'arbre */
		      printf ("( .. %i ) ", tree_nb);
		    else
		      fputs ("( .. ) ", stdout);
		  }
	      }
	  }
	else
	  {
	    /* Arbre initial */
	    if (pattr->sons_nb != 0)
	      {
	      /* interne */
	      printf (".%i%s.%i", tree_nb, string_address, position);

	      if (SXBA_bit_is_set (aux_set, pattr->node_name))
		fputs (".t", stdout);
	      }
	    
	    fputs ("> ", stdout);
	  }
      }
  }




static void
process_rule (tree_nb, lhs_address, lhs_string_address)
  int tree_nb, lhs_address;
  char *lhs_string_address;
{
  int son, nadd;
  struct node_address2attr *pattr = node_address2attr + lhs_address;
  
  process_lhs (tree_nb, lhs_address, lhs_string_address);

  for (son = 1; son <= pattr->sons_nb; son++) 
    {
      nadd = XxY_is_set (&node_address, lhs_address, son);
      process_rhs (tree_nb, nadd, lhs_string_address);
    }

  printf (";\n");

  process_adjunction (tree_nb, lhs_address, lhs_string_address);
}



static void
process_tree (tree_nb, root_address, local_address)
  int tree_nb, root_address;
  VARSTR local_address;
{
  int sons_nb, son, nadd, component, length;
  char string [12];

  sons_nb = node_address2attr [root_address].sons_nb;

  component = XxY_Y (node_address, root_address);
  length = varstr_length (local_address);
      
  if (component != 0)
    {
	  /* inside node */
      sprintf (string, ".%i", component);
      varstr_catenate (local_address, string);
    }

  if (sons_nb > 0)
    {
  
      process_rule (tree_nb, root_address, varstr_tostr (local_address));
  
      for (son = 1; son <= sons_nb; son++) 
	{
	  nadd = XxY_is_set (&node_address, root_address, son);
	  process_tree (tree_nb, nadd, local_address);
	}
    }
  else
    process_adjunction (tree_nb, root_address, varstr_tostr (local_address));   
  
  if (component != 0)
    *(local_address->current = local_address->first + length) = SXNUL;
}


void
xtag_second_pass ()
{
      /* Sortie de la LIG */

  int xtree, nadd;
  
  if (!IS_ERROR) {
	/* On traite en premier les arbres initiaux definissant l'axiome */
    extern FILE	*sxtty;
    
    if (sxverbosep) {
	    fprintf (sxtty, "\tLIG generation from a TAG with %i nodes in %i elementary trees\n",
		     XxY_top (node_address), current_tree_index);
    }

    local_address = varstr_alloc (32);

    if (!SS_is_empty (axiomes)) 
      {
	do 
	  {
	    xtree = SS_pop (axiomes);

	    printf ("************************** %i **************************\n", xtree);
  	    process_tree (xtree, XxY_is_set (&node_address, xtree, 0), local_address);
	  }
	while (!SS_is_empty (axiomes));
      }

    for (xtree = 1; xtree <= current_tree_index; xtree++)
      {
	nadd = XxY_is_set (&node_address, xtree, 0);

	if (tree2attr [xtree].is_aux || node_address2attr [nadd].node_name != axiome_ste)
	  {
	    printf ("************************** %i **************************\n", xtree);
	    process_tree (xtree, nadd, local_address);
	  }
      }

    varstr_free (local_address);
  }
  
  XxY_free (&node_address);
  sxfree (node_address2attr);
  sxfree (tree2attr);
  sxfree (aux_set);
  SS_free (axiomes);
}

void
xtag_smp (what, sxtables_ptr)
    int what;
    SXTABLES *sxtables_ptr;
{
  int local_tree_nb;

  switch (what) {
  case SXOPEN:
    sxtab_ptr = sxtables_ptr;
    sxatcvar.atc_lv.node_size = sizeof (struct xtag_node);
    tree_nb = 200;
    
    XxY_alloc (&node_address, "node_address", tree_nb * 10, 1, 0, 0, node_address_oflw, NULL);
    node_address2attr = (struct node_address2attr*) sxalloc (XxY_size (node_address) + 1,
							     sizeof (struct node_address2attr));

    tree2attr = (struct tree2attr*) sxalloc (tree_nb + 1, sizeof (struct tree2attr));

    aux_set = sxba_calloc (tree_nb * 10);

    axiomes = SS_alloc (20);

    NA_ste = sxstr2save ("NA", 2);
	/* En XTAG, l'axiome est "S"!!! */
    axiome_ste = sxstr2save ("S", 1);

    current_tree_index = 0;
    
    break;

  case SXSEMPASS:		/* For [sub-]tree evaluation during parsing */
    break;

  case SXACTION:
    local_tree_nb = sxatcvar.atc_lv.abstract_tree_root->son->degree;

    if (current_tree_index + local_tree_nb > tree_nb) 
      {
	tree_nb += local_tree_nb;
	tree2attr = (struct tree2attr*) sxrealloc (tree2attr, tree_nb + 1, sizeof (struct tree2attr));

	if (SXSTRtop () > BASIZE (aux_set))
	  aux_set = sxba_resize (aux_set, SXSTRtop () + 1);
      }
    

    xtag_first_pass (sxatcvar.atc_lv.abstract_tree_root->son);
    break;

  case SXCLOSE:
	/* Free est fait ds xtag_second_pass */
    break;

  default:
    break;
  }
}
