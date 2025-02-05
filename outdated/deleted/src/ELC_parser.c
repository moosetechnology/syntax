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



/* ABANDON le 3/5/2002 17:00
   Les temps d'analyse (compares au RCG) sont trop importants...
*/

/* Parseur a la Earley qui utilise un automate sous-jacent Left_corner */
/* Fonde' sur une ide'e de Mustapha Tounsi */
/* Il y a 2 versions */
/* Une version de reconnaisseur en temps cubique qui utilise "p_dispatch" et ... */
/* ... une version exponentielle qui utilise "e_dispatch" */
/* Le nombre "node_hd_calls" (compiler avec l'option -DEBUG2), montre une complexite exactement cubique
   sur la grammaire S -> S S, S -> a */


/*
-DEBUG4 : Qq verifications de coherence
-DEBUG2 : Qq tailles
-DEBUG3 : Les temps d'analyses intermediaires tous les 50 tokens
*/

#include "sxversion.h"
#include "sxunix.h"
#include "sxelc.h"
#include "sxmatrix2vector.h"

char WHAT_ELC_PARSER[] = "@(#)SYNTAX - $Id: ELC_parser.c 3929 2024-04-29 10:12:58Z garavel $" WHAT_DEBUG;

/* BAG_mngr */
typedef struct {
  struct bag_disp_hd {
    SXINT		size;
    SXBA_ELT	*set;
  }		*hd;
  SXBA_ELT	*pool_top;
  SXINT		hd_size, hd_top, hd_high, pool_size, room;
  char	*name;
  SXINT		used_size, prev_used_size, total_size; /* #if statistics */
  FILE        *file_name;
} bag_header;

void
bag_alloc (pbag, name, size, file_name)
    bag_header	*pbag;
    char	*name;
    SXINT 	size;
    FILE        *file_name;
{
  SXBA_ELT	*ptr;

  pbag->name = name;
  pbag->hd_top = 0;
  pbag->hd_high = 0;
  pbag->hd_size = 1;
  pbag->pool_size = size;
  pbag->hd = (struct bag_disp_hd*) sxalloc (pbag->hd_size, sizeof (struct bag_disp_hd));
  pbag->pool_top = pbag->hd [0].set = (SXBA_ELT*) sxcalloc (pbag->pool_size + 2, sizeof (SXBA_ELT));
  pbag->hd [0].size = pbag->pool_size;
  pbag->room = pbag->pool_size;

  if (pbag->file_name = file_name) {
    pbag->total_size = pbag->pool_size + 2;
    pbag->used_size = pbag->prev_used_size = 0;
  }
}



SXBA
bag_get (pbag, size)
    bag_header	*pbag;
    SXINT		size;
{
  SXINT 	slice_nb = SXNBLONGS (size) + 1;
  SXBA	set;

  if (slice_nb > pbag->room)
    {
      if (++pbag->hd_top >= pbag->hd_size)
	pbag->hd = (struct bag_disp_hd*) sxrealloc (pbag->hd,
						    pbag->hd_size *= 2,
						    sizeof (struct bag_disp_hd));

      if (pbag->hd_top <= pbag->hd_high) {
	pbag->pool_size = pbag->hd [pbag->hd_top].size;

	while (slice_nb > (pbag->room = pbag->pool_size)) {
	  if (++pbag->hd_top >  pbag->hd_high)
	    break;

	  pbag->pool_size = pbag->hd [pbag->hd_top].size;
	}
      }

      if (pbag->hd_top > pbag->hd_high) {	    
	while (slice_nb > (pbag->room = pbag->pool_size))
	  pbag->pool_size *= 2;

	if (pbag->file_name) {
	  fprintf (pbag->file_name, "Bag %s: New bag of size %i is created.\n", pbag->name, pbag->pool_size *= 2);
	  pbag->total_size += pbag->pool_size + 2;
	}

	pbag->hd_high = pbag->hd_top;
	pbag->hd [pbag->hd_top].set = (SXBA) sxcalloc (pbag->pool_size + 2, sizeof (SXBA_ELT));
	pbag->hd [pbag->hd_top].size = pbag->pool_size;
      }

      pbag->pool_top = pbag->hd [pbag->hd_top].set;
    }

  *(set = pbag->pool_top) = size;
  pbag->pool_top += slice_nb;
  pbag->room -= slice_nb;

  if (pbag->file_name) {
    pbag->used_size += slice_nb;
  }

  return set;
}


void
bag_reuse (pbag, set)
    bag_header	*pbag;
    SXBA	set;
{
  /* On recupere la place allouee depuis set. */
  /* Dangereux, c'est la responsabilite de l'utilisateur */
  SXINT	slice_nb;

  if (set < pbag->hd [pbag->hd_top].set || set > pbag->pool_top) {
    /* set pointe ne pointe plus ds le courant, on recupere tout le courant */
    set = pbag->hd [pbag->hd_top].set;
  }

  if ((slice_nb = pbag->pool_top - set) > 0) {
    pbag->room += slice_nb;

    if (pbag->file_name) {
      pbag->used_size -= slice_nb;
    }

    /* On nettoie */
    do {
      *--pbag->pool_top = 0;
    } while (pbag->pool_top > set);
  }
}
    

void
bag_free (pbag)
    bag_header	*pbag;
{
  if (pbag->file_name) {
    fprintf (pbag->file_name, "Bag %s: used_size = %i bytes, total_size = %i bytes\n",
	     pbag->name,
	     (pbag->used_size > pbag->prev_used_size ? pbag->used_size : pbag->prev_used_size) *
	     sizeof (SXBA_ELT) + (pbag->hd_high + 1) * sizeof (struct bag_disp_hd),
	     pbag->total_size * sizeof (SXBA_ELT) + pbag->hd_size * sizeof (struct bag_disp_hd));
  }

  do {
    sxfree (pbag->hd [pbag->hd_high].set);
  } while (--pbag->hd_high >= 0);

  sxfree (pbag->hd);
}
    

static void
bag_clear (pbag)
    bag_header	*pbag;
{
  /* On suppose que les SXBA sont empty. */
  pbag->hd_top = 0;
  pbag->pool_top = pbag->hd [0].set;
  pbag->pool_size = pbag->hd [0].size;
  pbag->room = pbag->pool_size;

  if (pbag->file_name) {
    if (pbag->prev_used_size < pbag->used_size)
      pbag->prev_used_size = pbag->used_size;

    pbag->used_size = 0;
  }
}
/* END BAG_mngr */

#define TOP(t)		(*t)
#define POP(t)		t[TOP(t)--]
#define PUSH(t,x)	(((++TOP(t) >= t[-1]) ? (t=(SXINT*) sxrealloc (t-1, (t[-1] *= 2)+2, sizeof (SXINT))+1) : (0)), t[TOP(t)]=x)
/* Secure PUSH */
#define SPUSH(t,x)	(t[++TOP(t)]=x)
/* before SPUSHes */
#define CHECK(t,n)	(((TOP(t)+n >= t[-1]) ? (t=(SXINT*) sxrealloc (t-1, (t[-1] *= 2)+2, sizeof (SXINT))+1) : (0)))
#define RAZ(t)		*t=0
#define ALLOC(t,s)      t = (SXINT*) sxalloc (s+2, sizeof (SXINT))+1, t [-1] = s;
#define FREE(t)         sxfree (t-1), t = NULL;
#define IS_EMPTY(t)     (TOP(t)==0)

#if is_sdag
#define SCAN(i,t)	(SXBA_bit_is_set (glbl_source [i], t))
#else
#define SCAN(i,t)	(glbl_source [i] == t)
#endif

#if is_rfsa 
#define RFSA_CHECKED(r,i)    (prod2lbs == NULL || SXBA_bit_is_set (prod2lbs [prolis [state2item [r]]], i)) 
#else
#define RFSA_CHECKED(r,i)    true
#endif

FILE	*sxstdout, *sxstderr;
FILE	*sxtty;

#include ELC_tables /* A la compilation, une option -DELC_tables = ... doit activer les bonnes tables produites
			 par une execution de csynt_lc -elc L */

static SXINT          max_node_set_size /* Estimation de la taille des ensembles node2node_set pour la tranche j */;
static bool      membership;

#if EBUG2
static SXINT  node_hd_calls;
#endif

#include "XxY.h"
static XxY_header   node_hd;
static SXBA         *node2node_set;
static SXBA         node2node_set_kind;
static bag_header   bag;
static SXINT          *stack1, *stack2, *cur_scan_stack, *scan_stack;


#if is_parse_forest
static SXBA         *node2i_set;
#if is_epsilon
static SXBA         *eps_prod2i_set;
#endif /* is_epsilon */
static void
output (prod, i, j) 
     SXINT prod, i, j;
{
  SXINT clause;

  /* Attention : la bnf ne contient ni les clauses multiples, ni les loops */
  /* Donc clause (qui est le representant de prod) peut ne pas se retrouver ds une analyse RCG
     car elle a pu ne pas etre validee par des &Lex eventuels associes */
  clause = (semact == NULL) ? prod : semact [prod];

  printf ("%i: %s(%i..%i)\n", clause, ntstring [lhs [prod]], i, j);
}
#endif /* is_parse_forest */

#if is_1LA && is_sdag
static bool
IS_AND (lhs_bits_array, rhs_bits_array)
    SXBA	lhs_bits_array, rhs_bits_array;
{
    SXBA	lhs_bits_ptr, rhs_bits_ptr;
    SXINT	        lhs_slices_number = SXNBLONGS (SXBASIZE (lhs_bits_array));

    lhs_bits_ptr = lhs_bits_array + lhs_slices_number, rhs_bits_ptr = rhs_bits_array + lhs_slices_number;

    while (lhs_slices_number-- > 0)
    {
	if (*lhs_bits_ptr-- & *rhs_bits_ptr--)
	    return true;
    }

    return false;
}
#endif /* is_1LA && is_sdag */


static void p_dispatch ();

static void
node_hd_oflw (old_size, new_size)
     SXINT old_size, new_size;
{
  node2node_set = (SXBA *) sxrealloc (node2node_set, new_size+1, sizeof (SXBA));
  node2node_set_kind = sxba_resize (node2node_set_kind, new_size+1);
#if is_parse_forest
  node2i_set = sxbm_resize (node2i_set, old_size+1, new_size+1, n+1);
#endif /* is_parse_forest */
}

#if 0
/* NOUVELLE VERSION EN FIN */
/* On vient de faire un reduce A -> \alpha */
/* p ->Aij r */
/* node_ip  */
static void
A_shift (j, p, r, node_ip)
     SXINT j, p, r, node_ip;
{
  SXINT  node_jr, node, size;
  SXBA node_jr2node_set, node_ip2node_set;
		
  if (XxY_set (&node_hd, j, r, &node_jr)) {
    /* old */
    node_jr2node_set = node2node_set [node_jr];

    if (p+1 == r) {
      if (node_jr2node_set [0] < (size = node2node_set [node_ip] [0]))
	node_jr2node_set [0] = size; /* On change la taille!! */
    }
    else {
      if (node_ip >= node_jr2node_set [0])
	node_jr2node_set [0] = node_ip+1; /* On change la taille!! */
    }
  }
  else {
    /* new */
    node_jr2node_set = node2node_set [node_jr] = bag_get (&bag, max_node_set_size);
    node_jr2node_set [0] = node_jr+1; /* On change la taille!! */
  }

  if (p+1 == r) {
    /* Transition non-terminale ds le kernel */
    node_ip2node_set = node2node_set [node_ip];

    node = 0;

    while ((node = sxba_scan (node_ip2node_set, node)) > 0) {
      /* La complexite en temps cubique provient d'ici, une re'duction peut provoquer O(n) calculs */
#if EBUG2
      node_hd_calls++;
#endif

      if (SXBA_bit_is_reset_set (node_jr2node_set, node)) {
	p_dispatch (j, r, node_jr, node);
      }
    }
  }
  else {
    /* Transition non-terminale non-kernel */
#if EBUG2
    node_hd_calls++;
#endif

    /* On ajoute node_ip a` node_jr2node_set */
    if (SXBA_bit_is_reset_set (node_jr2node_set, node_ip)) {
      p_dispatch (j, r, node_jr, node_ip);
    }
  }
}


/* On vient de faire une transition vers l'etat q a l'index j */
/* l'item kernel de q est de la forme A -> \alpha X . \beta */
/* Le noeud characteristique de A -> . \alpha X \beta est node_ip */
static void
p_dispatch (j, q, node_jq, node_ip)
     SXINT j, q, node_jq, node_ip;
{
  SXINT item, Y, prod, t, i, p, p1, q1, base, A, r, bot, top, k, node_kr, node_jr; 
  bool new, ntk_trans;
  SXBA node_kr2node_set, node_jr2node_set, glbl_source_line;

  item = state2item [q];
  Y = lispro [item];
  /* \beta = Y \beta'  */
    
  if (Y < 0) {
    /* (unique) transition terminale ds le kernel */
    /* Y est un t ... */
    if (SCAN (j+1, -Y)) {
      if (q == 2)
	/* j == n */
	membership = true;
      else {
	/* On cree le noeud <j+1, q+1> on lui associe {node_ip} a la 1ere creation de node = <j+1, q+1>, on PUSH node_ip */
	k = j+1;
	r = q+1;
#if EBUG2
	node_hd_calls++;
#endif

	if (XxY_set (&node_hd, k, r, &node_kr)) {
	  /* old */
	  node_kr2node_set = node2node_set [node_kr];

	  if (node_ip >= node_kr2node_set [0])
	    node_kr2node_set [0] = node_ip+1; /* On change la taille!! */
	}
	else {
	  /* new */
	  node_kr2node_set = node2node_set [node_kr] = bag_get (&bag, max_node_set_size);
	  node_kr2node_set [0] = ((node_ip > node_kr) ? node_ip : node_kr)+1; /* On change la taille!! */
	  SPUSH (scan_stack, node_kr);
	}

	SXBA_1_bit (node_kr2node_set, node_ip);
      }
    }
  }
  else {
    if (Y == 0) {
      /* Le kernel de q est un reduce (unique) */
      prod = prolis [item];
      /* item est de la forme prod: A -> \alpha . */
      /* Le look-ahead est ai+1 */

#if is_1LA
#if is_sdag
      if (!IS_AND (glbl_source [j+1], prod2la_set [prod]))
	/* Le test du look-ahead a echoue */
	return;
#else /* !is_sdag */
      t = source [j+1];

      if (!SXBA_bit_is_set (prod2la_set [prod], t))
	/* Le test du look-ahead a echoue */
	return;
#endif /* !is_sdag */
#endif /* is_1LA */

      i = XxY_X (node_hd, node_ip); /* On a Aij */

#if is_parse_forest
      if (SXBA_bit_is_reset_set (node2i_set [node_jq], i))
	output (prod, i, j);
#endif /* is_parse_forest */

      p =  XxY_Y (node_hd, node_ip);

      A = lhs [prod];

      /* On traite d'abord la transition kernel si elle existe */
      if ((base = vector_base (knt_shift, p)) != 0 &&
	  (r = knt_shift_vector_get (p, A, base)) != knt_shift_ERROR) {
#if EBUG4
	if (r > max_state)
	  sxtrap (ME, "p_dispatch");
#endif
	/* On ne guide pas (par RFSA_CHECKED (r, i)) la transition kernel
	   La selection de la prod courante a deja ete effectuee */
	A_shift (j, p, r, node_ip);
      }

      /* ... puis les transitions non-kernel */
      if ((p1 = state2ntstate ? state2ntstate [p] : p) > 0 /* On prend le representant pour les nknt-transitions */
	  && (base = vector_base (nknt_shift, p1)) != 0
	  && (r = nknt_shift_vector_get (p1, A, base)) != nknt_shift_ERROR) {
	if (r <= max_state) {
	  /* transition nk deterministe p -> r sur A */
	  if (RFSA_CHECKED (r, i))
	    /* Utilisation du guide rfsa */
	    /* Transition non-terminale non-kernel */
	    /* validee par l'analyse rfsa */
	    A_shift (j, p, r, node_ip);
	}
	else {
	  r -= max_state;
	  bot = trans_disp [r];
	  top = trans_disp [r+1];

	  do {
	    r = trans_list [bot];
	    /* transition nk non deterministe p -> r sur A */
	    if (RFSA_CHECKED (r, i))
	      A_shift (j, p, r, node_ip);
	  } while (++bot < top);
	}
      }
    }
    else {
      /* Y est un nt */
      /* q est de la forme A -> \alpha X . Y \beta' et Y est un nt */
      /* On traite les transitions terminales non kernel */
      k = j+1;

      if ((q1 = state2tstate ? state2tstate [q] : q) > 0 /* On prend le representant pour les t-transitions */
	  && (base = vector_base (t_shift, q1)) != 0) {
#if is_sdag
	t = -1;
	glbl_source_line = glbl_source [k];

	while ((t = sxba_scan (glbl_source_line, t)) >= 0) {
#else /* !is_sdag */
	  t = glbl_source [k];
#endif /* !is_sdag */

	  if ((r = t_shift_vector_get (q1, t, base)) != t_shift_ERROR) {
	    /* L'etat q a des transitions terminales q ->aj+1 r */
	    if (r <= max_state) {
	      /* transition deterministe */
	      if (RFSA_CHECKED (r, k)) {
		/* Utilisation eventuelle du guide rfsa */
		/* Transition terminale non-kernel validee par l'analyse rfsa :
		   on est au debute une bonne clause au bon indice */
#if EBUG2
		node_hd_calls++;
#endif

		if (XxY_set (&node_hd, k, r, &node_kr)) {
		  /* old */
		  node_kr2node_set = node2node_set [node_kr];

		  if (node_jq >= node_kr2node_set [0])
		    node_kr2node_set [0] = node_jq+1; /* On change la taille!! */
		}
		else {
		  /* new */
		  node_kr2node_set = node2node_set [node_kr] = bag_get (&bag, max_node_set_size);
		  node_kr2node_set [0] = ((node_jq > node_kr) ? node_jq : node_kr)+1; /* On change la taille!! */
		  SPUSH (scan_stack, node_kr);
		}

		SXBA_1_bit (node_kr2node_set, node_jq); 
	      } 
	    }
	    else {
	      /* transition non deterministe */
	      r -= max_state;
	      bot = trans_disp [r];
	      top = trans_disp [r+1];

	      do {
		r = trans_list [bot];
		if (RFSA_CHECKED (r, k)) {
		  /* Utilisation eventuelle du guide rfsa */
		  /* Transition terminale non-kernel validee par l'analyse rfsa */
	  
#if EBUG2
		  node_hd_calls++;
#endif

		  if (XxY_set (&node_hd, k, r, &node_kr)) {
		    /* old */
		    node_kr2node_set = node2node_set [node_kr];

		    if (node_jq >= node_kr2node_set [0])
		      node_kr2node_set [0] = node_jq+1; /* On change la taille!! */
		  }
		  else {
		    /* new */
		    node_kr2node_set = node2node_set [node_kr] = bag_get (&bag, max_node_set_size);
		    node_kr2node_set [0] = ((node_jq > node_kr) ? node_jq : node_kr)+1; /* On change la taille!! */
		    SPUSH (scan_stack, node_kr);
		  }

		  SXBA_1_bit (node_kr2node_set, node_jq); 
		}
	      } while (++bot < top);
	    }
	  }
#if is_sdag
	}
#endif /* is_sdag */
      }

#if is_epsilon
      /* On traite les reduce non kernel i.e., les epsilon_reduce
	 prod: B -> \beta . avec \beta =>* \epsilon */
      /* On sort "prod: Bii", la transition sur B est traitee statiquement, sauf si B==Y
         cad une des reductions vides que l'on vient de faire a son lhs nt qui est la transition kernel */
      ntk_trans = false;
      bot = eps_red_disp [q];
      top = eps_red_disp [q+1];

      if (bot < top) {
	do {
	  prod = eps_red_list [bot];
#if is_1LA
#if is_sdag
	  if (IS_AND (glbl_source [j+1], prod2la_set [prod]))
#else /* !is_sdag */
	    t = source [j+1];

	  if (SXBA_bit_is_set (prod2la_set [prod], t))
#endif /* !is_sdag */
	    /* Le test du look-ahead a marche' */
#endif /* is_1LA */
	    {
#if is_parse_forest
	      if (SXBA_bit_is_reset_set (eps_prod2i_set [prod], j))
		output (prod, j, j);
#endif /* is_parse_forest */
	      if (lhs [prod] == Y) {
		ntk_trans = true;
	      }
	    }
	} while (++bot < top);
      }

      if (ntk_trans) {
	/* Attention, on ne peut pas appeler
	   A_shift (j, q, q+1, node_ip);
	   pour incompatibilite transition kernel/node_ip (ici node_ip est correct alors que A_shift attendrait
	   un node_jq.
	   On simule donc ici directement la transition sur le Y vide */
	r = q+1; /* Transition sur Y ds le kernel */

	if (XxY_set (&node_hd, j, r, &node_jr)) {
	  /* old */
	  node_jr2node_set = node2node_set [node_jr];

	  if (node_ip >= node_jr2node_set [0])
	    node_jr2node_set [0] = node_ip+1; /* On change la taille!! */
	}
	else {
	  /* new */
	  node_jr2node_set = node2node_set [node_jr] = bag_get (&bag, max_node_set_size);
	  node_jr2node_set [0] = node_jr+1; /* On change la taille!! */
	}

#if EBUG2
	node_hd_calls++;
#endif

	/* On ajoute node_ip a` node_jr2node_set */
	if (SXBA_bit_is_reset_set (node_jr2node_set, node_ip)) {
	  p_dispatch (j, r, node_jr, node_ip);
	}
      }
#endif /* is_epsilon */

    }
  }
}




bool
sxelc_parse_it ()
{
  SXINT j, q, node_00, node_01, node_jq, node_ip, *stack;
  SXBA node_ip_set;
#if EBUG3
  char str [12];
#endif /* EBUG3 */

  XxY_alloc (&node_hd, "node_hd", n*n+1, 1, 0, 0, node_hd_oflw, statistics);
  bag_alloc (&bag, "bag", XxY_size (node_hd)+1, statistics);
  node2node_set = (SXBA *) sxalloc (XxY_size (node_hd)+1, sizeof (SXBA));

#if is_parse_forest
  node2i_set = sxbm_calloc (XxY_size (node_hd)+1, n+1);
#if is_epsilon
  eps_prod2i_set = sxbm_calloc (nbpro+1, n+1);;
#endif /* is_epsilon */

#endif /* is_parse_forest */

  ALLOC (stack1, max_state);
  ALLOC (stack2, max_state);

  cur_scan_stack = stack1;
  scan_stack = stack2;

#if EBUG2
  node_hd_calls = 2;
#endif
  XxY_set (&node_hd, 0, 0, &node_00);
  XxY_set (&node_hd, 0, 1, &node_01);
  node2node_set [node_01] = bag_get (&bag, node_01+1);
  SXBA_1_bit (node2node_set [node_01], node_00);

  /* On simule une transition depuis l'etat 0 sur eof vers l'etat (initial) 1 */
  PUSH (scan_stack, node_01);

  for (j = 0; j <= n; j++) {
    stack = cur_scan_stack;
    cur_scan_stack = scan_stack;
    scan_stack = stack;

    if (IS_EMPTY (cur_scan_stack))
      break;

#if EBUG3
    /* Pour mesurer la complexite' en temps */
    if (j % 50 == 0) {
      sprintf (str, "%i", j);
      sxtime (TIME_FINAL, str);
    }
#endif /* EBUG3 */

    /* Estimation (genereuse) de la taille des ensembles node2node_set pour la tranche j */
    /* On pourrait, pour la tranche j-1 "compacter les node2node_set */
    max_node_set_size = XxY_top (node_hd) + max_state + 1;

    do {
      node_jq = POP (cur_scan_stack);
      q = XxY_Y (node_hd, node_jq);
      node_ip_set = node2node_set [node_jq];
      
      node_ip = 0;

      while ((node_ip = sxba_scan (node_ip_set, node_ip)) > 0) {
	p_dispatch (j, q, node_jq, node_ip);
      }
    } while (!IS_EMPTY (cur_scan_stack));
  }

#if EBUG2
  printf ("Polynomial run (%s): node_calls = %i, nodes = %i\n", membership ? "true" : "false", node_hd_calls, XxY_top (node_hd));
#endif

  XxY_free (&node_hd);
  bag_free (&bag);
  sxfree (node2node_set), node2node_set = NULL;
  FREE (stack1);
  FREE (stack2);

#if is_parse_forest
  sxbm_free (node2i_set), node2i_set = NULL;
#if is_epsilon
  sxbm_free (eps_prod2i_set), eps_prod2i_set = NULL;
#endif /* is_epsilon */
#endif

  return membership;
}
#endif /* 0 */



#if 0
/* Ici, analyse en temps exponentiel par un parseur backtrack */
/* Exponential run data */
#include "XxYxZ.h"
static XxYxZ_header Prodij_hd;



static SXINT	    null_assoc [6];
static XxYxZ_header chart_hd;

#if EBUG2
static SXINT    memo_hit, *i2chart_nb;
#endif


/* dispatch a une complexite en temps (et place ds chart_hd) exponentielle */
/* L'item kernel de p est de la forme A -> \alpha . \beta */
/* Le look-ahead est ai+1 */
static void
e_dispatch (i, p, chart)
     SXINT i, p, chart;
{
  SXINT X, item, cur_chart, base, q, bot, top, prod, t, A, j, prodji, r, bot_chart;
  SXBA glbl_source_line;

  /* p est de la forme A -> \alpha . \beta */
  item = state2item [p];
  X = lispro [item];
  /* item est de la forme A -> \alpha . X \beta'  */
    
  if (X < 0) {
    /* (unique) transition terminale ds le kernel */
    /* X est un t ... */
    if (SCAN (i+1, -X)) {
      /* ... et c'est le t du source */
      if (X == -tmax)
	/* X == eof et p est le seul etat depuis lequel il existe une transition sur eof */
	membership = true;
      else
	e_dispatch (i+1, p+1, chart);
    }
  }
  else {
    if (XxYxZ_set (&chart_hd, i, p, chart, &cur_chart)) {
      /* existe deja */
#if EBUG2
      memo_hit++;
#endif
      return;
    }

#if EBUG2
    i2chart_nb [i]++;
#endif

    if (X > 0) {
      /* p est de la forme A -> \alpha . X \beta' et X est un nt */
      /* On traite les transitions terminales non kernel */
      if ((base = vector_base (t_shift, p)) != 0) { 
#if is_sdag
	t = -1;
	glbl_source_line = glbl_source [k];

	while ((t = sxba_scan (glbl_source_line, t)) >= 0) {
#else /* !is_sdag */
	  t = glbl_source [i+1];
#endif /* !is_sdag */

	  if ((q = t_shift_vector_get (p, t, base)) != t_shift_ERROR) {
	    /* L'etat p a des transitions terminales p ->ai+1 q */
	    if (q <= max_state)
	      /* transition deterministe */
	      e_dispatch (i+1, q, cur_chart);
	    else {
	      /* transition non deterministe */
	      q -= max_state;
	      bot = trans_disp [q];
	      top = trans_disp [q+1];

	      do {
		e_dispatch (i+1, trans_list [bot], cur_chart);
	      } while (++bot < top);
	    }
	  }
#if is_sdag
	}
#endif /* is_sdag */
      }

#if is_parse_forest && is_epsilon
      /* On traite les reduce non kernel i.e., les epsilon_reduce
	 prod: B -> \beta . avec \beta =>* \epsilon */
      /* On se contente de sortir "prod: Bii", la transition sur B est traitee statiquement */
      bot = eps_red_disp [p];
      top = eps_red_disp [p+1];

      if (bot < top) {
	do {
	  prod = eps_red_list [bot];
#if is_1LA
#if is_sdag
	  if (IS_AND (glbl_source [i+1], prod2la_set [prod]))
#else /* !is_sdag */
	    t = source [i+1];

	  if (SXBA_bit_is_set (prod2la_set [prod], t))
#endif /* !is_sdag */
	    /* Le test du look-ahead a marche' */
#endif /* is_1LA */
	    output (prod, i, i);
	} while (++bot < top);
      }
#endif /* is_parse_forest && is_epsilon */
    }
    else {
      /* Le kernel est un reduce (unique) */
      /* p est de la forme A -> \alpha .   (\beta == \epsilon) */
      /* Le look-ahead est ai+1 */
      prod = prolis [item];

#if is_1LA
#if is_sdag
      if (!IS_AND (glbl_source [i+1], prod2la_set [prod]))
	/* Le test du look-ahead a echoue */
	return;
#else /* !is_sdag */
      t = source [i+1];

      if (!SXBA_bit_is_set (prod2la_set [prod], t))
	/* Le test du look-ahead a echoue */
	return;
#endif /* !is_sdag */
#endif /* is_1LA */

#if is_parse_forest
      j = XxYxZ_X (chart_hd, chart); /* On a Aji */

      if (!XxYxZ_set (&Prodij_hd, prod, j, i, &prodji))
	output (prod, j, i);
#endif /* is_parse_forest */

      q = XxYxZ_Y (chart_hd, chart);

      base = vector_base (nknt_shift, q);

#if EBUG4
      if (base == 0)
	sxtrap (ME, "e_dispatch");
#endif

      r = nknt_shift_vector_get (q, lhs [prod], base); /* q ->A r et q ->\alpha p */

#if EBUG4
      if (r == nknt_shift_ERROR)
	sxtrap (ME, "e_dispatch");
#endif

      bot_chart = XxYxZ_Z (chart_hd, chart);

      if (r <= max_state)
	/* transition deterministe p -> r sur A */
	/* Si q+1==r <=> transition kernel */
	e_dispatch (i, r, (q+1 == r) ? bot_chart : chart);
      else {
	r -= max_state;
	bot = trans_disp [r];
	top = trans_disp [r+1];

	do {
	  r = trans_list [bot];
	  /* transition non deterministe p -> r sur A */
	  /* Si q+1==r <=> transition kernel */
	  e_dispatch (i, r, (q+1 == r) ? bot_chart : chart);
	} while (++bot < top);
      }
    }
  }
}

bool
sxelc_parse_it ()
{
  SXINT j, q, node_00, node_01, node_jq, node_ip, *stack;
  SXBA node_ip_set;

  /* Exponential run */
#if is_parse_forest
  XxYxZ_alloc (&Prodij_hd, "Prodij_hd",  n*n*ntmax+1, 1, null_assoc, NULL, statistics);
#endif /* is_parse_forest */

  XxYxZ_alloc (&chart_hd, "chart_hd", (1<<(n+1))+1, 1, null_assoc, NULL, statistics);

#if EBUG2
  i2chart_nb = (SXINT*) sxcalloc (n+2, sizeof (SXINT));
#endif

  e_dispatch (0, 1, 1); /* finesse : chart autoreference */

#if EBUG2
  printf ("Exponential run (%s): charts = %i, top_charts = %i, memo_hit = %i\n",
	  membership ? "true" : "false", XxYxZ_top (chart_hd), i2chart_nb [n], memo_hit);

  sxfree (i2chart_nb), i2chart_nb = NULL;
#endif

  XxYxZ_free (&chart_hd);

#if is_parse_forest
  XxYxZ_free (&Prodij_hd);
#endif

  return membership;
}
#endif /* 0 */

static void
SXBA_OR (lhs_bits_array, rhs_bits_array)
    SXBA	lhs_bits_array, rhs_bits_array;
/*
 * If the size of rhs_bits_array is > lhs_bits_array, the size of lhs_bits_array is modified
 * (we assume that there is enough room in lhs_bits_array)
 * It modified its first argument.
 */
{
  SXBA	lhs_bits_ptr, rhs_bits_ptr;
  SXINT         rhs_size = SXBASIZE (rhs_bits_array);
  SXINT	        slices_number;

  if (rhs_size > SXBASIZE (lhs_bits_array)) {
    SXBASIZE (lhs_bits_array) = rhs_size;
  }

  slices_number = SXNBLONGS (rhs_size);
    

  lhs_bits_ptr = lhs_bits_array + slices_number, rhs_bits_ptr = rhs_bits_array + slices_number;

  while (slices_number-- > 0) {
    *lhs_bits_ptr-- |= *rhs_bits_ptr--;
  }
}

static bool
SXBA_OR_MINUS (lhs_bits_array, lhs2_bits_array, rhs_bits_array)
    SXBA	lhs_bits_array, lhs2_bits_array, rhs_bits_array;
/*
 * SXBASIZE (lhs2_bits_array) == SXBASIZE (rhs_bits_array)
 * If the size of rhs_bits_array is > lhs_bits_array, the size of lhs_bits_array is modified
 * (we assume that there is enough room in lhs_bits_array)
 * It modified its first and second argument.
 * lhs2_bits_array = rhs_bits_array - lhs_bits_array
 * lhs_bits_array = lhs_bits_array | rhs_bits_array
 */
{
  SXBA	lhs_bits_ptr, lhs2_bits_ptr, rhs_bits_ptr;
  SXINT         rhs_size = SXBASIZE (rhs_bits_array);
  SXINT	        slices_number;
  bool ret_val = false;

  if (rhs_size > SXBASIZE (lhs_bits_array)) {
    SXBASIZE (lhs_bits_array) = rhs_size;
  }

  slices_number = SXNBLONGS (rhs_size);

  lhs_bits_ptr = lhs_bits_array + slices_number;
  lhs2_bits_ptr = lhs2_bits_array + slices_number;
  rhs_bits_ptr = rhs_bits_array + slices_number;

  while (slices_number-- > 0) {
    if (*lhs2_bits_ptr = *rhs_bits_ptr-- & ~*lhs_bits_ptr) {
      ret_val = true;
      *lhs_bits_ptr-- |= *lhs2_bits_ptr--;
    }
    else {
      lhs_bits_ptr--;
    }
  }

  return ret_val;
}

/* On vient de faire un reduce A -> \alpha */
/* p ->Aij r */
/* node_ip  */
static void
A_shift (j, p, r, node_ip)
     SXINT j, p, r, node_ip;
{
  SXINT  node_jr, node, size;
  SXBA node_jr2node_set, node_ip2node_set, wset;
		
#if EBUG2
  node_hd_calls++;
#endif

  if (XxY_set (&node_hd, j, r, &node_jr)) {
    /* old */
    node_jr2node_set = node2node_set [node_jr];

    if (p+1 == r) {
      /* Transition non-terminale ds le kernel */
      if (SXBA_bit_is_set_reset (node2node_set_kind, node_jr)) {
	/* allocation et recopie */
	wset = node2node_set [node_jr];
	node_jr2node_set = node2node_set [node_jr] = bag_get (&bag, max_node_set_size);
	node_jr2node_set [0] = wset [0];
	sxba_copy (node_jr2node_set, wset);
      }

      node_ip2node_set = node2node_set [node_ip];
      wset = bag_get (&bag, node_ip2node_set [0]);
      
      if (SXBA_OR_MINUS (node2node_set [node_jr], wset, node_ip2node_set))
	/* wset = node_ip2node_set - node2node_set [node_jr]; cad les les nouveaux nodes
	   node2node_set [node_jr] = node2node_set [node_jr] | node_ip2node_set;
	*/
	p_dispatch (j, r, node_jr, 0, wset);
    }
    else {
      /* Transition non-terminale non-kernel */
      node_jr2node_set = node2node_set [node_jr];

      if (node_ip >= node_jr2node_set [0])
	node_jr2node_set [0] = node_ip+1; /* On change la taille!! */

      if (SXBA_bit_is_reset_set (node_jr2node_set, node_ip)) {
	p_dispatch (j, r, node_jr, node_ip, NULL);
      }
    }
  }
  else {
    /* new */
    if (p+1 == r) {
      /* Transition non-terminale ds le kernel */
      /* A la 1ere creation, on met node2node_set [node_ip] */
      node_jr2node_set = node2node_set [node_jr] = node2node_set [node_ip];
      SXBA_1_bit (node2node_set_kind, node_jr);
      p_dispatch (j, r, node_jr, 0, node_jr2node_set);
    }
    else {
      /* Transition non-terminale non-kernel */
      node_jr2node_set = node2node_set [node_jr] = bag_get (&bag, max_node_set_size);
      node_jr2node_set [0] = node_jr+1; /* On change la taille!! */

      /* On initialise node_jr2node_set avec node_ip */
      SXBA_1_bit (node_jr2node_set, node_ip);
      p_dispatch (j, r, node_jr, node_ip, NULL);
    }
  }
}


/* On vient de faire une transition vers l'etat Q a l'index j */
/* Q peut etre le resultat d'une transition non_deterministe */
/* l'item kernel de q est de la forme A -> \alpha X . \beta */
/* Le noeud characteristique de A -> . \alpha X \beta est node_ip */
static void
p_dispatch (j, Q, node_jQ, node_ip, node_ip_set)
     SXINT j, Q, node_jQ, node_ip;
     SXBA node_ip_set;
{
  SXINT item, Y, prod, t, i, p, p1, q, q1, base, A, r, bot, top, k, node_kr, node_jr; 
  bool new, ntk_trans;
  SXBA node_kr2node_set, node_jr2node_set, glbl_source_line, wset;

  if (Q <= max_state) {
    q = Q;
    bot = top = 0;
  }
  else {
    Q -= max_state;
    bot = trans_disp [Q];
    top = trans_disp [Q+1];
    q = trans_list [bot];
  }

  do {
    item = state2item [q];
    Y = lispro [item];
    /* \beta = Y \beta'  */
    
    if (Y < 0) {
      /* (unique) transition terminale ds le kernel */
      /* Y est un t ... */
      if (SCAN (j+1, -Y)) {
	if (q == 2)
	  /* j == n */
	  membership = true;
	else {
	  /* On cree le noeud <j+1, q+1> on lui associe {node_ip} a la 1ere creation de node = <j+1, q+1>, on PUSH node_ip */
	  k = j+1;
	  r = q+1;
#if EBUG2
	  node_hd_calls++;
#endif

	  if (XxY_set (&node_hd, k, r, &node_kr)) {
	    /* old */
	    node_kr2node_set = node2node_set [node_kr];

	    if (SXBA_bit_is_set_reset (node2node_set_kind, node_kr)) {
	      /* allocation et recopie */
	      wset = node_kr2node_set;
	      node_kr2node_set = node2node_set [node_kr] = bag_get (&bag, max_node_set_size);
	      node_kr2node_set [0] = wset [0];
	      sxba_copy (node_kr2node_set, wset);
	    }

	    /* union avec node_ip_set */
	    if (node_ip_set)
	      SXBA_OR (node2node_set [node_kr], node_ip_set);
	    else
	      SXBA_1_bit (node2node_set [node_kr], node_ip);
	  }
	  else {
	    /* new */
	    /* A la 1ere creation, on met node_ip_set */
	    if (node_ip_set) {
	      node2node_set [node_kr] = node_ip_set;
	      SXBA_1_bit (node2node_set_kind, node_kr);
	    }
	    else {
	      node_kr2node_set = node2node_set [node_kr] = bag_get (&bag, max_node_set_size);
	      node_kr2node_set [0] = node_kr+1;
	      SXBA_1_bit (node_kr2node_set, node_ip);
	    }

	    SPUSH (scan_stack, node_kr); /* node_kr > 0 => pas de RFSA_CHECKED */
	  }
	}
      }
    }
    else {
      if (Y == 0) {
	/* Le kernel de q est un reduce (unique) */
	prod = prolis [item];
	/* item est de la forme prod: A -> \alpha . */
	/* Le look-ahead est ai+1 */

#if is_1LA
#if is_sdag
	if (!IS_AND (glbl_source [j+1], prod2la_set [prod]))
	  /* Le test du look-ahead a echoue */
	  return;
#else /* !is_sdag */
	t = source [j+1];

	if (!SXBA_bit_is_set (prod2la_set [prod], t))
	  /* Le test du look-ahead a echoue */
	  return;
#endif /* !is_sdag */
#endif /* is_1LA */

	if (node_ip_set)
	  node_ip = sxba_scan (node_ip_set, 0);

	do {
#if is_parse_forest
	  i = XxY_X (node_hd, node_ip); /* On a Aij */

	  if (SXBA_bit_is_reset_set (node2i_set [node_jQ], i))
	    output (prod, i, j);
#endif /* is_parse_forest */

	  p =  XxY_Y (node_hd, node_ip);
	  A = lhs [prod];

	  /* On traite d'abord la transition kernel si elle existe */
	  if ((base = vector_base (knt_shift, p)) != 0 &&
	      (r = knt_shift_vector_get (p, A, base)) != knt_shift_ERROR) {
#if EBUG4
	    if (r > max_state)
	      sxtrap (ME, "p_dispatch");
#endif
	    /* On ne guide pas (par RFSA_CHECKED (r, i)) la transition kernel
	       La selection de la prod courante a deja ete effectuee */
	    A_shift (j, p, r, node_ip);
	  }

	  /* ... puis les transitions non-kernel */
	  if ((p1 = state2ntstate ? state2ntstate [p] : p) > 0 /* On prend le representant pour les nknt-transitions */
	      && (base = vector_base (nknt_shift, p1)) != 0
	      && (r = nknt_shift_vector_get (p1, A, base)) != nknt_shift_ERROR) {
	    if (r <= max_state) {
	      /* transition nk deterministe p -> r sur A */
	      if (RFSA_CHECKED (r, i))
		/* Utilisation du guide rfsa */
		/* Transition non-terminale non-kernel */
		/* validee par l'analyse rfsa */
		A_shift (j, p, r, node_ip);
	    }
	    else {
	      r -= max_state;
	      bot = trans_disp [r];
	      top = trans_disp [r+1];

	      do {
		r = trans_list [bot];
		/* transition nk non deterministe p -> r sur A */
		if (RFSA_CHECKED (r, i))
		  A_shift (j, p, r, node_ip);
	      } while (++bot < top);
	    }
	  }
	} while (node_ip_set ? ((node_ip = sxba_scan (node_ip_set, node_ip)) > 0) : false);
      }
      else {
	/* Y est un nt */
	/* q est de la forme A -> \alpha X . Y \beta' et Y est un nt */
	/* On traite les transitions terminales non kernel */
	k = j+1;

	if ((q1 = state2tstate ? state2tstate [q] : q) > 0 /* On prend le representant pour les t-transitions */
	    && (base = vector_base (t_shift, q1)) != 0) {
#if is_sdag
	  t = -1;
	  glbl_source_line = glbl_source [k];

	  while ((t = sxba_scan (glbl_source_line, t)) >= 0)
#else /* !is_sdag */
	    t = glbl_source [k];
#endif /* !is_sdag */
	  {

	    if ((r = t_shift_vector_get (q1, t, base)) != t_shift_ERROR) {
	      /* L'etat q a des transitions terminales q ->aj+1 r */
#if EBUG2
	      node_hd_calls++;
#endif

	      if (XxY_set (&node_hd, k, r, &node_kr)) {
		/* old */
		node_kr2node_set = node2node_set [node_kr];

		if (node_jQ >= node_kr2node_set [0])
		  node_kr2node_set [0] = node_jQ+1;

		SXBA_1_bit (node_kr2node_set, node_jQ);
	      }
	      else {
		/* new */
		node_kr2node_set = node2node_set [node_kr] = bag_get (&bag, max_node_set_size);
		node_kr2node_set [0] = node_kr+1;
		SXBA_1_bit (node_kr2node_set, node_jQ);
		SPUSH (scan_stack, -node_kr); /* node_kr < 0 => il faudra faire RFSA_CHECKED */
	      }
	    }
	  }
	}

#if is_epsilon
	/* On traite les reduce non kernel i.e., les epsilon_reduce
	   prod: B -> \beta . avec \beta =>* \epsilon */
	/* On sort "prod: Bii", la transition sur B est traitee statiquement, sauf si B==Y
	   cad une des reductions vides que l'on vient de faire a son lhs nt qui est la transition kernel */
	ntk_trans = false;
	bot = eps_red_disp [q];
	top = eps_red_disp [q+1];

	if (bot < top) {
	  do {
	    prod = eps_red_list [bot];
#if is_1LA
#if is_sdag
	    if (IS_AND (glbl_source [j+1], prod2la_set [prod]))
#else /* !is_sdag */
	      t = source [j+1];

	    if (SXBA_bit_is_set (prod2la_set [prod], t))
#endif /* !is_sdag */
	      /* Le test du look-ahead a marche' */
#endif /* is_1LA */
	      {
#if is_parse_forest
		if (SXBA_bit_is_reset_set (eps_prod2i_set [prod], j))
		  output (prod, j, j);
#endif /* is_parse_forest */
		if (lhs [prod] == Y) {
		  ntk_trans = true;
		}
	      }
	  } while (++bot < top);
	}

	if (ntk_trans) {
	  /* Attention, on ne peut pas appeler
	     A_shift (j, q, q+1, node_ip);
	     pour incompatibilite transition kernel/node_ip (ici node_ip est correct alors que A_shift attendrait
	     un node_jq.
	     On simule donc ici directement la transition sur le Y vide */
	  r = q+1; /* Transition sur Y ds le kernel */
		
#if EBUG2
	  node_hd_calls++;
#endif

	  if (XxY_set (&node_hd, j, r, &node_jr)) {
	    /* old */
	    if (SXBA_bit_is_set_reset (node2node_set_kind, node_jr)) {
	      /* allocation et recopie */
	      wset = node2node_set [node_jr];
	      node_jr2node_set = node2node_set [node_jr] = bag_get (&bag, max_node_set_size);
	      node_jr2node_set [0] = wset [0];
	      sxba_copy (node_jr2node_set, wset);
	    }
	    else
	      node_jr2node_set = node2node_set [node_jr];

	    if (node_ip_set) {
	      wset = bag_get (&bag, node_ip_set [0]);
      
	      if (SXBA_OR_MINUS (node_jr2node_set, wset, node_ip_set))
		/* wset = node_ip_set - node2node_set [node_jr]; cad les les nouveaux nodes
		   node2node_set [node_jr] = node2node_set [node_jr] | node_ip_set;
		*/
		p_dispatch (j, r, node_jr, 0, wset);
	    }
	    else {
	      if (node_ip >= node_jr2node_set [0])
		node_jr2node_set [0] = node_ip+1; /* On change la taille!! */

	      if (SXBA_bit_is_reset_set (node_jr2node_set, node_ip))
		p_dispatch (j, r, node_jr, node_ip, NULL);
	    }
	  }
	  else {
	    /* new */
	    if (node_ip_set) {
	      node2node_set [node_jr] = node_ip_set;
	      SXBA_1_bit (node2node_set_kind, node_jr);
	      p_dispatch (j, r, node_jr, 0, node_ip_set);
	    }
	    else {
	      node_jr2node_set = node2node_set [node_jr] = bag_get (&bag, max_node_set_size);
	      node_jr2node_set [0] = node_jr+1; /* On change la taille!! */
	      SXBA_1_bit (node_jr2node_set, node_ip);
	      p_dispatch (j, r, node_jr, node_ip, NULL);
	    }
	  }
	}
#endif /* is_epsilon */

      }
    }
  } while ((++bot < top) && (q = trans_list [bot]));
}



bool
sxelc_parse_it ()
{
  SXINT j, q, bot, top, node_00, node_01, node_jq, *stack;
  SXBA node_jq_set;
  bool RFSA_CHECK_done;
#if EBUG3
  char str [12];
#endif /* EBUG3 */

  XxY_alloc (&node_hd, "node_hd", n*n+1, 1, 0, 0, node_hd_oflw, statistics);
  node2node_set_kind = sxba_calloc (XxY_size (node_hd)+1);
  bag_alloc (&bag, "bag", XxY_size (node_hd)+1, statistics);
  node2node_set = (SXBA *) sxalloc (XxY_size (node_hd)+1, sizeof (SXBA));

#if is_parse_forest
  node2i_set = sxbm_calloc (XxY_size (node_hd)+1, n+1);
#if is_epsilon
  eps_prod2i_set = sxbm_calloc (nbpro+1, n+1);;
#endif /* is_epsilon */

#endif /* is_parse_forest */

  ALLOC (stack1, max_state);
  ALLOC (stack2, max_state);

  cur_scan_stack = stack1;
  scan_stack = stack2;

#if EBUG2
  node_hd_calls = 2;
#endif
  XxY_set (&node_hd, 0, 0, &node_00);
  XxY_set (&node_hd, 0, 1, &node_01);
  node2node_set [node_01] = bag_get (&bag, node_01+1);
  SXBA_1_bit (node2node_set [node_01], node_00);

  /* On simule une transition depuis l'etat 0 sur eof vers l'etat (initial) 1 */
  PUSH (scan_stack, node_01);

  for (j = 0; j <= n; j++) {
    stack = cur_scan_stack;
    cur_scan_stack = scan_stack;
    scan_stack = stack;

    if (IS_EMPTY (cur_scan_stack))
      break;

#if EBUG3
    /* Pour mesurer la complexite' en temps */
    if (j % 50 == 0) {
      sprintf (str, "%i", j);
      sxtime (TIME_FINAL, str);
    }
#endif /* EBUG3 */

    /* Estimation (genereuse) de la taille des ensembles node2node_set pour la tranche j */
    /* On pourrait, pour la tranche j-1 "compacter les node2node_set */
    max_node_set_size = XxY_top (node_hd) + max_state + 1;

    do {
      if ((node_jq = POP (cur_scan_stack)) < 0) {
	RFSA_CHECK_done = false;
	node_jq = -node_jq;
      }
      else
	RFSA_CHECK_done = true;

      node_jq_set = node2node_set [node_jq];
      q = XxY_Y (node_hd, node_jq);

      if (q <= max_state) {
	/* transition deterministe */
	if (RFSA_CHECK_done || RFSA_CHECKED (q, j)) {
	  /* Utilisation eventuelle du guide rfsa */
	  /* Transition terminale non-kernel validee par l'analyse rfsa :
	     on est au debute une bonne clause au bon indice */
	  p_dispatch (j, q, node_jq, 0, node_jq_set);
	}
      }
      else {
	/* transition non deterministe */
	q -= max_state;
	bot = trans_disp [q];
	top = trans_disp [q+1];

	do {
	  q = trans_list [bot];

	  if (RFSA_CHECK_done || RFSA_CHECKED (q, j)) {
	    /* Utilisation eventuelle du guide rfsa */
	    /* Transition terminale non-kernel validee par l'analyse rfsa */
	    p_dispatch (j, q, node_jq, 0, node_jq_set);
	  }
	} while (++bot < top);
      }
    } while (!IS_EMPTY (cur_scan_stack));
  }

#if EBUG2
  printf ("Polynomial run (%s): node_calls = %i, nodes = %i\n", membership ? "true" : "false", node_hd_calls, XxY_top (node_hd));
#endif

  XxY_free (&node_hd);
  bag_free (&bag);
  sxfree (node2node_set), node2node_set = NULL;
  sxfree (node2node_set_kind), node2node_set_kind = NULL;
  FREE (stack1);
  FREE (stack2);

#if is_parse_forest
  sxbm_free (node2i_set), node2i_set = NULL;
#if is_epsilon
  sxbm_free (eps_prod2i_set), eps_prod2i_set = NULL;
#endif /* is_epsilon */
#endif

  return membership;
}

