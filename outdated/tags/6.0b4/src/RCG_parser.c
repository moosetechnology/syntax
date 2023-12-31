/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1997 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *     Produit de l'equipe ATOLL.			  *
   *                                                      *
   ******************************************************** */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* Ven 10 Dec 1999 11:22(PB)	Adaptation a ANY (_StrEq, _StrReverse et*/
/*				_Permute				*/
/************************************************************************/
/* 6 Nov 1997 10:23:41 (PB)	Ajout de cette rubrique "modifications"	*/
/************************************************************************/



#include "sxunix.h"

char WHAT_RCG_PARSER[] = "@(#)SYNTAX - $Id: RCG_parser.c 1146 2008-03-06 09:10:54Z syntax $" WHAT_DEBUG;

static char	ME [] = "RCG_parser.c";

#include "rcg_glbl.h"

/* Ce programme doit etre compile avec l'option
       -DPID=p S'il y a p sous-parser */

/* Le source est partage uniquement avec le module principal (celui dont le local_pid est nul)
   et qui doit contenir la definition de TOUS les tokens (l'ordre n'a pas d'importance).
   Pour les autres modules "source" est local, remplit avec les codes locaux (ou 0 si inconnu) par
   l'appel de la proc fill_buf si necessaire. */

#define TOP(t)		(*t)
#define POP(t)		t[TOP(t)--]
#define PUSH(t,x)	(((++TOP(t) >= t[-1]) ? (t=(SXINT*) sxrealloc (t-1, (t[-1] *= 2)+2, sizeof (SXINT))+1) : (0)), t[TOP(t)]=x)
/* Secure PUSH */
#define SPUSH(t,x)	(t[++TOP(t)]=x)
/* before SPUSHes */
#define CHECK(t,n)	(((TOP(t)+n >= t[-1]) ? (t=(SXINT*) sxrealloc (t-1, (t[-1] *= 2)+2, sizeof (SXINT))+1) : (0)))
#define RAZ(t)		*t=0

FILE	*sxtty;

#if !is_large
SXINT
large_needed (SXINT lognt)
{
    if ((lognt+2*logn)>SXBITS_PER_LONG) {
	fputs("All modules must be recompiled with the -DLARGE option.\n", sxtty);
	SXEXIT(1);
    }

    return 0;
}
#endif

#if PID>1
static SXINT
check_combinatorial_options (struct G *G)
{
    SXINT pid, kind = 0;

    for (pid = 0; pid < PID; pid++) {
	kind |= (G->Gs [pid]->is_combinatorial) ? 1 : 2;
    }

    if (kind == 3) {
	fputs("All parsers must be generated with consistent \"combinatorial\" options.\n", sxtty);
	SXEXIT(1);
    }
}
#endif


struct area_block {
    SXINT			size;
    struct area_block	*next;
    struct spf_node	node [1];
};

struct area_hd {
    struct area_block	*empty, *bot;
    struct spf_node	*top;
    SXINT			room, size;
};

static VOID
allocate_area (struct area_hd *area_hd)
{
  struct area_block	*new_block, **prev;

  if (new_block = area_hd->empty) {
      /* Reutilisation de blocks vides */
      prev = &(area_hd->empty);

      do {
	  if (new_block->size >= area_hd->size)
	      break;
      } while (prev = &(new_block->next), new_block = new_block->next);

      if (new_block) {
	  *prev = new_block->next;
	  area_hd->size = new_block->size;
      }
  }
      
  if (new_block == NULL) {
      new_block = (struct area_block*) sxalloc (1, sizeof(SXINT)+sizeof(struct area_block*)+area_hd->size);
      new_block->size = area_hd->size;
  }

  new_block->next = area_hd->bot;
  area_hd->bot = new_block;
  area_hd->top = &(new_block->node [0]);
  area_hd->room = area_hd->size;
}

static struct spf_node *
book_area (struct area_hd *area_hd, SXINT int_nb)
{
    struct spf_node	*node_ptr;
    SXINT			char_size;

    char_size = sizeof (struct spf_node*) + sizeof (SXINT)*int_nb;

    if (char_size > area_hd->room) {
	while (char_size > area_hd->size)
	    area_hd->size *= 2 ;

	allocate_area (area_hd);
    }

    area_hd->room -= char_size;
#if EBUG4
  if (area_hd->room < 0)
      sxtrap (ME, "book_area");
#endif

    node_ptr = area_hd->top;

    area_hd->top = (struct spf_node *) ((char *) area_hd->top + char_size);

    return node_ptr;
}

static void
free_area (struct area_hd *area_hd)
{
  struct area_block	*block_ptr, *next_block_ptr;

  for (block_ptr = area_hd->bot; block_ptr != NULL; block_ptr = next_block_ptr) {
    next_block_ptr = block_ptr->next;
    sxfree (block_ptr);
  }

  for (block_ptr = area_hd->empty; block_ptr != NULL; block_ptr = next_block_ptr) {
    next_block_ptr = block_ptr->next;
    sxfree (block_ptr);
  }

  area_hd->bot = area_hd->empty = NULL;
}


static void
clear_area (struct area_hd *area_hd)
{
  struct area_block	*block_ptr, *next_block_ptr, **prev, *empty_ptr;

  block_ptr = area_hd->bot;
  next_block_ptr = block_ptr->next;
  block_ptr->next = NULL;

  if (empty_ptr = area_hd->empty) {
      while (prev = &(empty_ptr->next), empty_ptr = *prev);

      *prev = next_block_ptr;
  }
  else
      area_hd->empty = next_block_ptr;

  area_hd->top = &(block_ptr->node [0]);
  area_hd->room = block_ptr->size;
  /* bot et size sont bons */
}

/* Codage d'un sxbm_resize ds lequel le nb de colonnes ne change pas, les SXBA du debordement sont
   traitees ds des *SXBA annexes.  On suppose que bm a ete realloue, il reste a rempir la deuxieme moitie */
static void
sxbm_line_resize (SXBA *bm, SXINT old_size, SXINT new_size, SXBA **oflw_disp, SXINT *oflw_disp_size, SXINT col_nb)
{
  SXINT slices_number, line;
  SXBA bits_array;

  if (*oflw_disp == NULL) {
    *oflw_disp_size = 1;
    *oflw_disp = (SXBA*) sxalloc (*oflw_disp_size, sizeof (SXBA));
  }
  else {
    *oflw_disp = (SXBA*) sxrealloc (*oflw_disp, *oflw_disp_size += 1, sizeof (SXBA));
  }
	
  slices_number = NBLONGS (col_nb);
  (*oflw_disp) [*oflw_disp_size-1] = bits_array = (SXBA) sxcalloc ((new_size - old_size) * (slices_number + 1), sizeof (SXBA_ELT));

  for (line = old_size+1; line <= new_size; line++, bits_array += slices_number + 1) {
    BASIZE (bits_array) = col_nb;
    bm [line] = bits_array;
  }
}

static void
sxbm_line_free (SXBA *oflw_disp, SXINT oflw_disp_size)
{
  while (--oflw_disp_size >= 0)
    sxfree (oflw_disp [oflw_disp_size]);

  sxfree (oflw_disp);
}


#if is_sdag
static SXBA *glbl_source_area;

#ifdef clause2tree_include_file
/* Les clauses selectionnees sont exprimees en terme des arbres de la TAG d'origine */
#include clause2tree_include_file
#else /* !clause2tree_include_file */
static char **clause2tree_name;
#endif /* !clause2tree_include_file */

#else /* !is_sdag */
static SXINT *glbl_source_area;
#endif /* !is_sdag */

SXBA	        *out_ff2clause_set [PID];


SEM_TYPE	*sem_disp [PID];

static SXINT      icnb; /* decompte le nombre de clauses instanciees */


static void
print_instantiated_clause (struct G *G, SXINT *rho, char *prefix, BOOLEAN is_clause_nb, BOOLEAN is_lhs, BOOLEAN is_rhs)
  /* rho est un tableau de taille son_nb+2
     rho[0] contient le numero p de la clause courante.  Si p est <0 la clause est -p et
     l'appel est cyclique
     rho[1] contient l'identifiant du vecteur de range de la LHS
     rho[2], ..., rho[son_nb+1] contient les identifiants des vecteurs de range de la RHS */
  /* rho[son_nb+2] contient l'identifiant ds rhs_ranges des olb,oub */
  /* Si G->clause2lhst_disp est non NULL, rho[son_nb+3], ... contient la liste des indexes
     ds le source des terminaux de la LHS */
  /* sons est un tableau de taille sons_nb+1, il matche clause.
       Si clause = A0 --> A1 ... Ap, sons[i] est l'identifiant du module ds lequel est defini Ai */
{
    SXINT			clause, rho0, son_nb, bot, top, cur, item, *sons, son, rhoi, son_pid, arity, pos, identical_clause;	
    struct Aij_struct	*Aij_struct_ptr;
    BOOLEAN		is_set, is_neg_call;
    SXINT			pid = G->pid;
    struct G		*sonG;
    
    clause = *rho++;

    if (prefix != NULL)
	printf ("%s ", prefix);

    if (is_clause_nb) {
	if (pid != 0)
	    printf ("%i.", pid);

	is_set = G->clause2identical_disp != NULL &&
	    ((bot = G->clause2identical_disp [clause]) < (top = G->clause2identical_disp [clause+1]));

	if (is_set)
	    fputs ("{", stdout);

#if is_sdag
	if (clause2tree_name)
	  printf ("[%s]/", clause2tree_name [clause]);
#endif /* is_sdag */
	printf ("%i", clause);

	if (is_set) {
	    while (bot < top) {
	      identical_clause = G->clause2identical [bot];
	      fputs (" ", stdout);

#if is_sdag
	      if (clause2tree_name)
		printf ("[%s]/", clause2tree_name [identical_clause]);
#endif /* is_sdag */

	      printf ("%i", identical_clause);
	      bot++;
	    }

	    fputs ("}", stdout);
	}

	fputs (": ", stdout);
    }

    rho0 = *rho++;

    if (is_lhs) {
	Aij_struct_ptr = (*(G->Aij2struct))(rho0);
	printf ("%s (", Aij_struct_ptr->Astr);
	arity = Aij_struct_ptr->arity;

	for (pos = 0; pos < arity; pos++) {
	    if (pos > 0)
		fputs (", ", stdout);

	    printf ("%i..%i", Aij_struct_ptr->lb [pos], Aij_struct_ptr->ub [pos]);
	}

	fputs (")", stdout);
    }

    if (is_rhs) {
	fputs (" --> ", stdout);

	son_nb = G->clause2son_nb [clause];

	if (son_nb > 0) {
	    sons = G->clause2son_pid [clause]+1;

	    for (son = 0; son < son_nb; son++) {
		if (son > 0)
		    fputs (", ", stdout);

		rhoi = *rho++;
		son_pid = *sons++;

		if (son_pid < 0) {
		    if (son_pid == TRUE_ic) {
			printf ("&True (%i..%i)", ij2i(rhoi), ij2j(rhoi));
		    }
		}
		else {
		    sonG = G->Gs [son_pid];

		    if (rhoi < 0) {
		      /* le 09/02/04 */
		      /* Appel negatif */
		      rhoi = -rhoi;
		      is_neg_call = TRUE;
		    }
		    else
		      is_neg_call = FALSE;

		    Aij_struct_ptr = (*(sonG->Aij2struct))(rhoi);
		    /* Le 09/02/04 comment noter les appels negatif ?? */
		    printf ("%s%s (", is_neg_call ? "!" : "", Aij_struct_ptr->Astr);
		    arity = Aij_struct_ptr->arity;

		    for (pos = 0; pos < arity; pos++) {
			if (pos > 0)
			    fputs (", ", stdout);

			printf ("%i..%i", Aij_struct_ptr->lb [pos], Aij_struct_ptr->ub [pos]);
		    }

		    fputs (")", stdout);
		}
	    }

	    fputs (" .", stdout);
	}
	else
	    fputs ("true .", stdout);
    }

    fputs ("\n", stdout);
}


static SXBA 		used_clause [PID];

#if is_parse_forest
/* Pour ressortir le sdag reduit ... ff {nb} ... ou ... ff{c1 c2 ... cp}[n1/n2] ...
 ou n1 est le nb de clauses selectionnees par ff apres analyse
 ou n2 est le nb de clauses selectionnees par ff avant analyse
 et {c1 c2 ... cp} est l'ensemble de ces clauses */
static XxY_header	AijXclause;
static SXINT		AijXclause_Xassoc, AijXclause_Yassoc;
static SXBA		pid2lhs_prdct_set [PID];
#if PID>1
static SXINT		pidXAijXclause_assoc [6];
static XxYxZ_header	pidXAijXclause;
#endif
#endif /* is_parse_forest */

#if is_guided || is_coupled || is_fully_coupled
static SXINT		gpid;
/* Si FULLY GUIDED/COUPLED */
/* Accessible depuis les rcgp par l'intermediaire de la macro GET_FULL_GUIDE() */
XxY_header	XxY_lhs_guide [PID];
struct spf_node	**XxY_lhs_guide2rhs [PID];

static struct area_hd	rhs_guide_area_hd;
static struct spf_node	*node_ptr;

static void
XxY_lhs_guide_oflw (SXINT old_size, SXINT new_size)
{
    /* gpid est static */
    XxY_lhs_guide2rhs [gpid] = (struct spf_node**) sxrealloc (XxY_lhs_guide2rhs [gpid], new_size+1, sizeof (struct spf_node*));
}


static void
fill_guide (SXINT gA_ij, SXINT gclause)
{
    /* gpid et node_ptr sont static */
    SXINT 	lhs;
    BOOLEAN	old_lhs;

    old_lhs = XxY_set (&(XxY_lhs_guide [gpid]), gA_ij, gclause, &lhs);

    if (node_ptr != NULL)
	node_ptr->next = old_lhs ? XxY_lhs_guide2rhs [gpid] [lhs] : NULL;

    XxY_lhs_guide2rhs [gpid] [lhs] = node_ptr;
}
#endif /* is_guided || is_coupled || is_fully_coupled */

#if is_semantics
static void
sem_mem_mngr (struct G *G, SXINT old_size, SXINT new_size)
{
    if (old_size == 0) {
	/* ALLOCATION */
	sem_disp [G->pid] = (SEM_TYPE*) sxcalloc (new_size+1, sizeof (SEM_TYPE));
    }
    else {
	if (new_size == 0) {
	    /* FREE */
	    sxfree (sem_disp [G->pid]), sem_disp [G->pid] = NULL;
	}
	else {
	    /* REALLOCATION */
	    sem_disp [G->pid] = (SEM_TYPE*) sxrecalloc (sem_disp [G->pid], old_size+1, new_size+1, sizeof (SEM_TYPE));
	}
    }
}


static BOOLEAN
semact_gen_loop (struct G *G, SXINT *rho, ifun semact_action)
{
  /* Simulation des cycles triviaux A --> A */
  SXINT 	        A, x, *p, bot, top, clause, clause_id, lrho [3];
  BOOLEAN	ret_val = FALSE;

  A = G->clause2lhs_nt [rho [0]];

  if (x = G->looped_nt [A]) {
    /* ... sur A */
    p = G->looped_clause+x;
    lrho [2] = lrho [1] = rho [1];

    while (clause = *p++) {
      /* ... sur A(...) --> A(...) &Lex(...) */
      /* if (G->is_Lex [clause]) { */
      /* Le 10/6/2002
	 Si invalid_lex_loop_clause est positionne, il contient toutes les loop clauses qui ont ete invalidees
	 par les verif sur les &Lex */
      if (G->invalid_lex_loop_clause == NULL || !SXBA_bit_is_set (G->invalid_lex_loop_clause, clause))
#if 0
      if (SXBA_bit_is_set (G->loop_lex, clause))
#endif /* 0 */
	{
	/* ... et le is_Lex est bon */
	/* On simule un vrai appel de la semantique */
	/* C'est elle qui traite tout */
	lrho [0] = clause;

#if EBUG3	
	if (semact_action == NULL)
	  print_instantiated_clause (G, lrho, G->name, TRUE, TRUE, TRUE);
#endif

	if ((semact_action == NULL) || (*semact_action)(G, lrho))
	  ret_val = TRUE;
      }
    }
  }

  return ret_val;
}


#if is_parse_forest
static SXINT parse_forest_action (struct G *G, SXINT *rho);
static SXINT print_source_text (struct G **Gs, SXINT S0n, SXINT ret_val);

#ifdef SYNCHRONOUS
/* Essai le 03/03/08 */
/* Contient les grammaires synchronisees */
static	struct G    *synchro_Gs [PID];
static XxYxZ_header synchro_fsa;
static SXINT        cur_synchro_fsa_state;
static struct synchro_pq {
  SXINT p, q;
}                   *Aij_arg2synchro_pq, empty_synchro_pq, **symb2synchro_pq_ptr;
static SXINT        synchro_Aij2pq_top = 1;
static SXINT        *synchro_Aij2pq_disp [PID];
#endif /* SYNCHRONOUS */

static void
parse_forest_mem_mngr (struct G *G, SXINT old_size, SXINT new_size)
{
    SXINT		top, bot, clause, nt;
    SXINT		pid = G->pid;

    if (old_size == 0) {
	/* ALLOCATION */
	if ((forest_level & FL_lhs_prdct) != 0 && ((forest_level & FL_lhs_clause) == 0 || (forest_level & FL_rhs) != FL_rhs))
	    pid2lhs_prdct_set [pid] = sxba_calloc (new_size+1);

	if (forest_level & FL_clause) {
	    /* Pour la sortie des no de clauses utilisees */
	    used_clause [pid] = sxba_calloc (G->clause_nb+1);
	}

	if (forest_level & FL_supertagger)
	    out_ff2clause_set [pid] = sxbm_calloc (n+2, G->clause_nb+1);

	if (pid == 0) {
	    /* Commun a tous les pid */
	    if ((forest_level & FL_lhs_clause) && (forest_level & FL_lhs_prdct) && (forest_level & FL_rhs) != FL_rhs) {
		XxY_alloc (&AijXclause, "AijXclause", new_size+1, 1, AijXclause_Xassoc, AijXclause_Yassoc, NULL, statistics);
#if PID>1
		XxYxZ_alloc (&pidXAijXclause, "pidXAijXclause", PID*new_size+1, 1, pidXAijXclause_assoc, NULL, statistics);
#endif
	    }

	    if (forest_level & FL_n)
	      /* La 1ere ligne de la parse-forest est la taille du source et, en commentaire
		 le fichier source et le nom de la grammaire */
	      /* "%" "[" {ANY} "%" "]"&True est un commentaire pour la lecture du guide */
	      printf ("%%%i %%[ %s %s %%]\n", n, SXGET_TOKEN (1).source_index.file_name, G->name);
	}

#ifdef SYNCHRONOUS
	if (pid == 0) {
	  SXINT                 size, cur_pid, cur_size;
	  static SXINT		synchro_fsa_foreach [6] = {0, 0, 0, 0, 0, 0};

	  XxYxZ_alloc (&synchro_fsa, "synchro_fsa", PID*new_size+1, 1, synchro_fsa_foreach, NULL, statistics);

	  size = 0;

	  for (cur_pid = 0; cur_pid < PID; cur_pid++) {
	    cur_size = X_top (*(Gs [cur_pid]->prhoA_hd));
	    synchro_Aij2pq_disp [cur_pid] = (SXINT *) sxcalloc (cur_size+1, sizeof (SXINT));
	    size += cur_size;
	  }
	  
	  Aij_arg2synchro_pq = (struct synchro_pq*) sxalloc (G->arity*size+1, sizeof (struct synchro_pq));
	  symb2synchro_pq_ptr = (struct synchro_pq**) sxalloc (synchro_Gs [G->pid]->rhs_args_nb+1 /* car non-combinatoire ?? */, sizeof (struct synchro_pq*));
	}
#endif /* SYNCHRONOUS */
    }
    else {
	if (new_size == 0) {
	    /* FREE */
	    if (pid2lhs_prdct_set [pid])
		sxfree (pid2lhs_prdct_set [pid]), pid2lhs_prdct_set [pid] = NULL;

	    if (used_clause [pid]) {
		sxfree (used_clause [pid]), used_clause [pid] = NULL;
	    }

	    if (out_ff2clause_set [pid])
		sxbm_free (out_ff2clause_set [pid]), out_ff2clause_set [pid] = NULL;

	    if (pid == 0) {
		/* Commun a tous les pid */
		if (XxY_is_allocated (AijXclause) > 0)
		    XxY_free (&AijXclause);

#if PID>1
		if (XxY_is_allocated (pidXAijXclause) > 0)
		    XxY_free (&pidXAijXclause);
#endif
	    }

#ifdef SYNCHRONOUS
	    if (pid == 0) {
	      SXINT cur_pid;

	      XxYxZ_free (&synchro_fsa);

	      for (cur_pid = 0; cur_pid < PID; cur_pid++) {
		sxfree (synchro_Aij2pq_disp [cur_pid]), synchro_Aij2pq_disp [cur_pid] = NULL;
	      }
	  
	      sxfree (Aij_arg2synchro_pq), Aij_arg2synchro_pq = NULL;
	      sxfree (symb2synchro_pq_ptr), symb2synchro_pq_ptr = NULL;
	    }
#endif /* SYNCHRONOUS */
	}
	else {
	    /* REALLOCATION */
	    if (pid2lhs_prdct_set [pid])
		pid2lhs_prdct_set [pid] = sxba_resize (pid2lhs_prdct_set [pid], new_size+1);
	}
    }
}

static void
process_loop (struct G *G, SXINT *rho, ifun semact_action, SXINT cycle_kind)

{
  if (cycle_kind & LOOP) {
    /* Attention, rho [0] designe une A-clause qcq */
    if (!semact_gen_loop (G, rho, semact_action))
      /* Boucle non confirmee par la lexicalisation */
      cycle_kind -= LOOP;
  }

  /* Le 31/10/2001 abandon de la detection de cycles */
#if is_check_cycle || EBUG3
  if (cycle_kind) {
    char string [40];

    sprintf (string, "WARNING: %s detected around", cycle_kind == LOOP+CYCLE ? "loop & cycle" : (cycle_kind == CYCLE ? "cycle" : "loop"));

    print_instantiated_clause (G, rho, string, FALSE, TRUE, FALSE);
  }
#endif
}

static void
parse_forest_loop (struct G *G, SXINT *rho, SXINT cycle_kind)
{
    /* Arho = rho [1]
       Le calcul de toutes les instantiations des Arho-clauses est termine, au moins une
       clause valide est impliquee ds un cycle (ou une boucle) */
    /* rho [0] est une A-clause */

    /* G->pass_no == 1 */
    process_loop (G, rho, parse_forest_action, cycle_kind);
}



#endif /* is_parse_forest */

#if is_coupled
/* Contient les grammaires des parsers guide's */
static	struct G 	*Guided_Gs [PID];
#if 0
static SXBA 		coupled_used_clause [PID];
#endif /* 0 */
#endif /* is_coupled */
#endif /* is_semantics */


struct G 	*Gs [PID];

/* pfun		Aij2struct [PID]; */

static sxword_header	*t_names_ptr;

#if PID>1
static SXINT 		**g2l, **l2g, *gl_area, gtmax;
#endif /* PID>1 */

#if is_generator || AG
/* On "genere" le texte source */

static XH_header paths;
static struct path_refs {
    SXINT	next, ref;
} *path_refs;
static SXINT	path_ref_top, path_ref_size;

static struct rhs {
    SXINT bot, son;
} *rhs; 

static SXINT	*stack, *eq_ids;
static SXINT 	*si2xs;
static struct G	*G;
static SXINT 	clause, lhs_arg_nb, son_nb, *lhs_args, *rhs_prdcts, *ret_vals, *Xoccur, *Xoccur_lim, *indexes,
                *ilb, *iub, *olb, *oub, local_pid;

#if is_generator
#define BPART	(((SXUINT)~0)>>1)
#define CPART	(~BPART)
#define NOT	CPART
#define EQ	CPART
#define NEG	(EQ>>1)

static SXINT	*freres, *w1stack, *w2stack, *w3stack, *args_id, *neg_calls;
static SXINT	*stack0, *stack1, *stack2, *cstack0, *cstack1;
#endif /* is_generator */

#if AG
static void
AG_alloc (SXINT init_size)
{
}
static void
AG_free ()
{
}
#endif /* AG */

static void
insert_source (SXINT *prev, SXINT ref)
{
    SXINT			next;
    struct path_refs	*prefs;

    if ((next = *prev) == -1)
	next = 0;

    while (next > 0) {
	prefs = &(path_refs [next]);

	if (prefs->ref <= ref) {
	    if (prefs->ref == ref)
		return;

	    break;
	}

	prev = &(prefs->next);
	next = *prev;
    }

    /* A faire avant la reallocation eventuelle */
    *prev = ++path_ref_top;

    if (path_ref_top >= path_ref_size)
	path_refs = (struct path_refs*) sxrealloc (path_refs, (path_ref_size *= 2)+1, sizeof (struct path_refs));

    prefs = &(path_refs [path_ref_top]);
    prefs->ref = ref;
    prefs->next = next;

    return;
}

static void
path_alloc (SXINT init_size)
{
    /* On predefinit l'ensemble qui ne contient que la chaine vide (il vaut 1) */
    SXINT list1, no_arg, rhs_args_nb, id, nb;

    XH_alloc (&paths, "paths", init_size+1, 1, init_size+1, NULL, statistics);
    path_refs = (struct path_refs*) sxalloc ((path_ref_size = init_size)+1, sizeof (struct path_refs));
    path_ref_top = 0;
    XH_set (&paths, &list1); /* list1 = 1 */
    no_arg = 0, insert_source (&no_arg, list1); /* no_arg = list1 = 1 */

    rhs_args_nb = 0;

    for (id = 0; id < PID; id++) {
	if ((nb = Gs [id]->rhs_args_nb) > rhs_args_nb)
	    rhs_args_nb = nb;
    }

    rhs = (struct rhs*) sxalloc (rhs_args_nb+1, sizeof (struct rhs));
}
static void
path_free ()
{
    XH_free (&paths);
    sxfree (path_refs), path_refs = NULL;
    sxfree (rhs), rhs = NULL;
}


static void
gen_or_DAG_alloc (SXINT init_size)
{
    stack = (SXINT*) sxalloc (init_size+2, sizeof (SXINT))+1, stack [-1] = init_size;
    eq_ids = (SXINT*) sxalloc (init_size+2, sizeof (SXINT))+1, stack [-1] = init_size;
    si2xs = (SXINT*) sxalloc (n+1, sizeof (SXINT));
#if AG
    AG_alloc (init_size);
#endif /* AG */
    path_alloc (init_size+1);
#if is_generator
    freres = (SXINT*) sxalloc (init_size+2, sizeof (SXINT))+1, freres [-1] = init_size;
    w1stack = (SXINT*) sxalloc (init_size+2, sizeof (SXINT))+1, w1stack [-1] = init_size;
    w2stack = (SXINT*) sxalloc (init_size+2, sizeof (SXINT))+1, w2stack [-1] = init_size;
    w3stack = (SXINT*) sxalloc (init_size+2, sizeof (SXINT))+1, w3stack [-1] = init_size;
    args_id = (SXINT*) sxalloc (init_size+2, sizeof (SXINT))+1, args_id [-1] = init_size;
    stack0 = (SXINT*) sxalloc (init_size+2, sizeof (SXINT))+1, stack0 [-1] = init_size;
    stack1 = (SXINT*) sxalloc (init_size+2, sizeof (SXINT))+1, stack1 [-1] = init_size;
    stack2 = (SXINT*) sxalloc (init_size+2, sizeof (SXINT))+1, stack2 [-1] = init_size;
    cstack0 = (SXINT*) sxalloc (init_size+2, sizeof (SXINT))+1, cstack0 [-1] = init_size;
    cstack1 = (SXINT*) sxalloc (init_size+2, sizeof (SXINT))+1, cstack1 [-1] = init_size;
    neg_calls = (SXINT*) sxalloc (init_size+2, sizeof (SXINT))+1, neg_calls [-1] = init_size;
#endif /* is_generator */
}

static void
gen_or_DAG_free ()
{
    sxfree (stack-1), stack = NULL;
    sxfree (eq_ids-1), eq_ids = NULL;
    sxfree (si2xs), si2xs = NULL;
#if AG
    AG_free ();
#endif/* AG */
    path_free ();
#if is_generator
    sxfree (freres-1), freres = NULL;
    sxfree (w1stack-1), w1stack = NULL;
    sxfree (w2stack-1), w2stack = NULL;
    sxfree (w3stack-1), w3stack = NULL;
    sxfree (args_id-1), args_id = NULL;
    sxfree (stack0-1), stack0 = NULL;
    sxfree (stack1-1), stack1 = NULL;
    sxfree (stack2-1), stack2 = NULL;
    sxfree (cstack0-1), cstack0 = NULL;
    sxfree (cstack1-1), cstack1 = NULL;
    sxfree (neg_calls-1), neg_calls = NULL;
#endif /* is_generator */
}

#if !AG
static SXINT	ANY_nb, *si2any_nb, *any_nb2si;
static SXBA	ws_set;

static void
source_generator_init (SXINT init_size)
{
    SXINT i, nb;

    if (ANY_nb == 0) return;

    if (ws_set == NULL) {
	gen_or_DAG_alloc (init_size);
	ws_set = sxba_calloc (n+1);
	si2any_nb = (SXINT*) sxalloc (n+1, sizeof (SXINT));
	any_nb2si = (SXINT*) sxalloc (ANY_nb+1, sizeof (SXINT));
	any_nb2si [0] = si2any_nb [0] = nb = 0;

	for (i = 1; i <= n; i++) {
	    if (glbl_source [i] == ANY) {
		nb++;
		any_nb2si [nb] = i;
	    }

	    si2any_nb [i] = nb;
	}
    }
}

static void
source_generator_free ()
{
    if (ws_set != NULL) {
	gen_or_DAG_free ();
	sxfree (ws_set), ws_set = NULL;
	sxfree (si2any_nb), si2any_nb = NULL;
	sxfree (any_nb2si), any_nb2si = NULL;
    }
}

#if EBUG2
static void
source_generator_post (SXINT ret_val)
{
    /* Si ret_val est >0 l'analyse a marche' et c'est l'identifiant de l'ensemble des chaines */
    SXINT next, list_id, eq_id, prdct_id, orig, top, xeq, xlist, i, symb, cstrnt, x1, x2, neg_id, bot;

    if (ANY_nb == 0) return;

    if (ret_val != -1) {
	if (ret_val < 0) {
	    fputs ("Negative generations are not yet implemented\n", stdout);
	    return;
	}

	fputs ("The following sentences have been generated:\n", stdout);

	for (next = ret_val; next != 0; next = path_refs [next].next) {
	    prdct_id = path_refs [next].ref;
	    top = XH_X (paths, prdct_id+1)-1;

	    eq_id = XH_list_elem (paths, top);

	    if (eq_id & NEG) {
		/* Il y a du complementaire */
		neg_id = eq_id ^ NEG;
		top--;
		eq_id = XH_list_elem (paths, top);
	    }
	    else
		neg_id = 0;

	    if (eq_id & EQ) {
		/* Il y a une specif d'egalite sur les args */
		eq_id ^= EQ;
	    }
	    else
		eq_id = 0;

	    list_id = XH_list_elem (paths, XH_X (paths, prdct_id));
	    xlist = XH_X (paths, list_id);

	    for (i = 1; i <= n; i++) {
		symb = glbl_source [i];

		if (symb == ANY) {
		    symb = XH_list_elem (paths, xlist++);
		}

		cstrnt = symb & CPART;
		symb &= BPART;

		fprintf (stdout, "%s%s", cstrnt ? "~" : "", sxttext (sxplocals.sxtables, symb));
	    }

	    if (eq_id) {
		fputs (" [", stdout);

		top = XH_X (paths, eq_id+1);
		xeq = XH_X (paths, eq_id);

		while (xeq < top) {
		    x1 = XH_list_elem (paths, xeq++);
		    cstrnt = x1 & CPART;
		    x1 &= BPART;
		    x2 = XH_list_elem (paths, xeq++)&BPART;
		    
		    fprintf (stdout, "%i%s=%i", x1, cstrnt ? "!" : "=", x2);

		    if (xeq < top)
			fputs (", ", stdout);
		}

		fputs ("]", stdout);
	    }

	    if (neg_id) {
		fputs (" -{", stdout);

		top = XH_X (paths, neg_id+1);

		for (bot = XH_X (paths, neg_id); bot < top; bot++) {
		    list_id = XH_list_elem (paths, bot);
		    /* On est sur l'axiome, list_id designe un singleton */
		    xlist = XH_X (paths, list_id);
		    list_id = XH_list_elem (paths, xlist);
		    xlist = XH_X (paths, list_id);

		    for (i = 1; i <= n; i++) {
			symb = glbl_source [i];

			if (symb == ANY) {
			    symb = XH_list_elem (paths, xlist++);
			}

			fprintf (stdout, "%s", sxttext (sxplocals.sxtables, symb));
		    }

		    if (bot+1 < top)
			fputs (", ", stdout);
		}

		fputs ("}", stdout);
	    }

	    fputs ("\n", stdout);
	}
    }
    else
	fputs ("This source text does not match any sentence.\n", stdout);
}
#endif /* EBUG2 */
#endif /* !AG */
#endif /* is_generator || AG */

#if is_semantics
/* Si is_parse_tree_number */
/* Utilisable soit en parallele avec l'analyse syntaxique, soit au cours
   de la seconde passe (la 1ere passe a utilise store_elem_tree) */
/* calcule le nombre d'arbres de la foret partagee */
/* Peut s'activer, qq soit la semantique associee aux RCG sources par l'option "-ptn" */
#define __NO_MATH_INLINES
#include <math.h>
/*   HUGE_VAL : Produces IEEE Infinity. */

/* Le Jeu 14 Dec 2000 09:46:31, pid est remplace par G pour toutes les fonctions semantiques */
/* Le Mer  5 Jul 2000 11:36:23, suppression du param first_call */
/* Le Mar  7 Nov 2000 14:07:00, pour chaque clause le nombre de fils et chacun de leur pid sont
   stockes ds clause2son_nb et clause2son_pid */
double	*tree_nb_disp [PID];

static void
tree_nb_mem_mngr (struct G *G, SXINT old_size, SXINT new_size)
{
    if (old_size == 0) {
	/* ALLOCATION */
	tree_nb_disp [G->pid] = (double*) sxcalloc (new_size+1, sizeof (double));
    }
    else {
	if (new_size == 0) {
	    /* FREE */
	    sxfree (tree_nb_disp [G->pid]), tree_nb_disp [G->pid] = NULL;
	}
	else {
	    /* REALLOCATION */
	    tree_nb_disp [G->pid] = (double*) sxrecalloc (tree_nb_disp [G->pid], old_size+1, new_size+1, sizeof (double));
	}
    }
}


static void
tree_nb_loop (struct G *G, SXINT *rho, SXINT cycle_kind)
{
    /* Le calcul de toutes les instantiations des Arho-clauses est termine, au moins une
       clause valide est impliquee ds un cycle (ou une boucle) */
    /* Le nombre d'arbres (valides) issus de Arho est donc infini */
    tree_nb_disp [G->pid] [rho [1]] = HUGE_VAL;

    process_loop (G, rho, NULL, cycle_kind);
}

static SXINT
tree_nb_action (struct G *G, SXINT *rho/*, son_nb, sons[], first_call */)
    /* rho est un tableau de taille son_nb+2
       rho[0] contient le numero p de la clause courante.  Si p est <0 la clause est -p et
       l'appel est cyclique
       rho[1] contient l'identifiant du vecteur de range de la LHS
       rho[2], ..., rho[son_nb+1] contient les identifiants des vecteurs de range de la RHS */
    /* sem_disp est un tableau global de taille PID dont chaque element est un pointeur
       vers un tableau local a chaque module ds lequel est stocke la semantique (element
       de type SEM_TYPE, c'est une union) */

    /* sons est un tableau de taille sons_nb+1, il matche clause.
       Si clause = A0 --> A1 ... Ap, sons[i] est l'identifiant du module ds lequel est defini Ai */

    /* Si first_call==-1, c'est la premiere fois qu'on fait une reduc vers (A, rho[0]), clause est une A_clause */
{
    SXINT			rhoi, son, clause, son_nb, *sons, son_pid, bot, top;
    double		nb;

    icnb++;

#if EBUG2
    instantiated_clause_nb++;
#endif
    
    clause = *rho++;

    son_nb = G->clause2son_nb [clause];
    sons = G->clause2son_pid [clause];
    nb = 1;

    for (son = son_nb; son > 0; son--) {
      if ((son_pid = sons [son]) >= 0 && (rhoi = rho [son]) > 0) {
	/* si son_pid < 0 => appel de &True */
	/* Si rhoi < 0 => appel de &StrEq ("a", X) et -rhoi est l'indice de "a"
	   ds le source
	   ou (le 09/02/04) appel negatif qui a marche, on peut donc sauter */
	/* A VOIR : comment differencier les appels negatifs des appels de &StrEq ("a", X) !! */
#if EAL_WITH_GUIDE
	{

#include <string.h>

	  struct Aij_struct	*Aij_struct_ptr;
	  struct G		*sonG;
	  SXINT               length;

	  sonG = G->Gs [son_pid];
	  Aij_struct_ptr = (*(sonG->Aij2struct))(rhoi);
	  length=strlen(Aij_struct_ptr->Astr);
	  if (length > 4 && strncmp(Aij_struct_ptr->Astr + length - 4, "__NF", 4) == 0) {
#if EBUG
	    printf("Sub-forest deriving from %i-th rhs predicate (%s) of an instance of clause #%i is ignored\n", son, Aij_struct_ptr->Astr , clause);
#endif /* EBUG */
	  } else {
#endif /* EAL_WITH_GUIDE */
	    nb *= tree_nb_disp [son_pid] [rhoi];
#if EAL_WITH_GUIDE
	  }
	}
#endif /* EAL_WITH_GUIDE */
      }
    }

    if (G->clause2identical_disp != NULL &&
	((bot = G->clause2identical_disp [clause]) < (top = G->clause2identical_disp [clause+1]))) {
	nb *= top-bot+1;
	icnb += top-bot;
#if EBUG2
	instantiated_clause_nb += top-bot;
#endif
    }

    tree_nb_disp [G->pid] [rho [0]] += nb;

    return 1;
}


static SXINT
tree_nb_post (struct G **Gs, SXINT S0n, SXINT ret_val)
{
    /* ret_val different de -1 ssi l'analyse a marche' */
    /* S0n est l'identifiant du couple axiome, (vecteur de) range [0..n] */
    double		nb;

    if (ret_val != -1) {
	nb = tree_nb_disp [0] [S0n];

	if (nb == 1)
	    fprintf (stdout, "% This %i word sentence is non ambiguous.\n", n);
	else {
	    if (nb < 1.0E9)
		fprintf (stdout, "% There are %.f trees in the shared parse forest of this %i word sentence.\n", nb, n);
	    else {
		if (nb == HUGE_VAL)
		    fprintf (stdout, "% The shared parse forest of this %i word sentence is cyclic.\n", n);
		else
		    fprintf (stdout, "% There are %e trees in the shared parse forest of this %i word sentence.\n", nb, n);
	    }
	}
    }
    else
	fprintf (stdout, "This %i word source text is not a sentence.\n", n);

    return ret_val;
}


/* Procedure qui permet de stocker les arbres elementaires au fur et a mesure de leur
   reconnaissance et de fabriquer ainsi la foret partagee d'analyse qui sera utilisee
   au cours de la seconde passe */

static struct area_hd	spf_area_hd;
static vfun		semact_oflw_call [PID]; /* Pour restaurer le traitement des debordements de la seconde passe */
static vfun		*semact_oflw_ptr [PID];

static void
store_elem_tree_init (struct G *G, SXINT old_size, SXINT new_size)
{
  if (spf_area_hd.bot == NULL) {
      spf_area_hd.size = new_size*sizeof(struct spf_node);/* Pourquoi pas */
      allocate_area (&spf_area_hd);
  }

  sem_mem_mngr (G, old_size, new_size);
}

static void
store_elem_tree_final (struct G *G, SXINT old_size, SXINT new_size)
{
    /* (*(G->semact.second_pass_final)) (G, old_size, new_size); */

    if (spf_area_hd.bot)
	free_area (&spf_area_hd);

    sem_mem_mngr (G, old_size, new_size);
}

static void
store_elem_tree_loop (struct G *G, SXINT *rho, SXINT cycle_kind)
{
    /* Arho = rho [1]
       Le calcul de toutes les instantiations des Arho-clauses est termine, au moins une
       clause valide est impliquee ds un cycle (ou une boucle) */
    /* rho [0] est une A-clause */

    /* G->pass_no == 1 */
    /* GET_SEM_HAS_LOOP (G->pid, rho [1]) = 1; */
    process_loop (G, rho, NULL, cycle_kind);
}


/* Le Mer  5 Jul 2000 11:36:23, suppression du param first_call */
/* Le Mar  7 Nov 2000 14:07:00, pour chaque clause le nombre de fils et chacun de leur pid sont
   stockes ds clause2son_nb et clause2son_pid */
static SXINT
store_elem_tree_action (struct G *G, SXINT *rho /*, son_nb, sons, first_call */)
  /* rho est un tableau de taille son_nb+2
     rho[0] contient le numero p de la clause courante.  Si p est <0 la clause est -p et
     l'appel est cyclique
     rho[1] contient l'identifiant du vecteur de range de la LHS
     rho[2], ..., rho[son_nb+1] contient les identifiants des vecteurs de range de la RHS */
  /* Si G->clause2lhst_disp est non NULL, rho[son_nb+2], ... contient la liste des indexes
     ds le source des terminaux de la LHS */
  /* sem_disp est un tableau global de taille PID dont chaque element est un pointeur
     vers un tableau local a chaque module ds lequel est stocke la semantique (element
     de type SEM_TYPE, c'est une union) */
  /* sons est un tableau de taille sons_nb+1, il matche clause.
     Si clause = A0 --> A1 ... Ap, sons[i] est l'identifiant du module ds lequel est defini Ai */
  /* Stockes en FIFO */
{
    SXINT			*p, i, nb, clause, son_nb, bot, top;
    struct spf_node	*node_ptr, **disp_ptr;

#if EBUG3
    print_instantiated_clause (G, rho, G->name, TRUE, TRUE, TRUE);
#endif

    clause = rho [0];


#if EAL_WITH_GUIDE
    {

#include <string.h>

      struct Aij_struct	*Aij_struct_ptr;
      SXINT               length;

      Aij_struct_ptr = (*(G->Aij2struct))(rho [1]);
      length=strlen(Aij_struct_ptr->Astr);
      if (length > 4 && strncmp(Aij_struct_ptr->Astr + length - 4, "__NF", 4) == 0) {
#if EBUG
	printf("%s-instantiated clause (instance of clause #%i) not stored in the parse forest\n", Aij_struct_ptr->Astr , clause);
#endif /* EBUG */
	return 1 ;
      }
    }
#endif /* EAL_WITH_GUIDE */

    icnb++;

    if (G->clause2identical_disp != NULL &&
	((bot = G->clause2identical_disp [clause]) < (top = G->clause2identical_disp [clause+1]))) {
	icnb += top-bot;
    }

    son_nb = G->clause2son_nb [clause];

    nb = son_nb+2;

    if (G->clause2lhst_disp)
	nb += G->clause2lhst_disp [clause+1]-G->clause2lhst_disp [clause];

    node_ptr = book_area (&spf_area_hd, nb);

    if (*(disp_ptr = &GET_SEM_TOP_NODE_PTR (G->pid, rho [1])) == NULL)
	GET_SEM_NODE_PTR (G->pid, rho [1]) = node_ptr;
    else
	(*disp_ptr)->next = node_ptr;

    node_ptr->next = NULL;
    *disp_ptr = node_ptr;

    p = &(node_ptr->rho [0]);

    for (i = 0; i < nb; i++)
	*p++ = *rho++;
    
    return 1;
}



void
tree_nb (struct G *G)
{
    /* Le Mar 8 Aou 2000 16:02:29 */
    /* RCG_parser devient independant de is_single_pass/is_multiple_pass */
    /* Ca permet d'appeler 2 rcgp compiles l'un avec is_single_pass et l'autre avec is_multiple_pass */

    /* first et second pass sont tous les 2 initialises avec tree_nb_action :
       Si 1 seule passe, semact->second_pass sera inutilise
       Si 2 passes semact->first_pass sera ecrase par la vraie 1ere passe */

    /* ATTENTION, si tree_nb est evalue lors de la seconde passe, le calcul se fait ds le
       sem_disp de store_elem_tree */

    /* Le Lun 29 Jan 2001 11:59:59, creation de tree_nb_mem_mngr */
    G->semact.first_pass_final =  G->semact.second_pass_final = G->semact.first_pass_init = G->semact.second_pass_init =
	G->semact.oflw = tree_nb_mem_mngr;
    G->semact.first_pass = G->semact.second_pass = tree_nb_action;
    G->semact.first_pass_loop = G->semact.second_pass_loop = tree_nb_loop;
    G->semact.last_pass = tree_nb_post;
}


static void
store_elem_tree_first_pass_final (struct G *G, SXINT old_size, SXINT new_size)
{
    *(semact_oflw_ptr [G->pid]) = semact_oflw_call [G->pid];
}

void
store_elem_tree (struct G *G)
{
    G->semact.first_pass_init = store_elem_tree_init;
    G->semact.first_pass = store_elem_tree_action;
    G->semact.first_pass_loop = store_elem_tree_loop;
    G->semact.first_pass_final = store_elem_tree_first_pass_final; /* sem_disp est conserve entre les 2 passes */
    semact_oflw_ptr [G->pid] = &(G->semact.oflw);
    semact_oflw_call [G->pid] = G->semact.oflw; /* sera reactive ds store_elem_tree_first_pass_final pour la 2eme passe */
    G->semact.oflw = sem_mem_mngr; /* pour la 1ere passe */
    G->semact.second_pass_final = store_elem_tree_final;
    /* G->semact.second_pass_loop : on n'y touche pas */
    /* G->semact.last_pass : on n'y touche pas */
}

#if is_parse_forest
void
parse_forest (struct G *G)
{
    G->semact.second_pass_init = parse_forest_mem_mngr;
    G->semact.second_pass = parse_forest_action;
    G->semact.second_pass_final = parse_forest_mem_mngr;
    G->semact.first_pass_init = parse_forest_mem_mngr;
    G->semact.first_pass = parse_forest_action;
    G->semact.first_pass_final =  parse_forest_mem_mngr;
    G->semact.last_pass = print_source_text;
    G->semact.oflw = parse_forest_mem_mngr;
    G->semact.first_pass_loop = G->semact.second_pass_loop = parse_forest_loop;
}
#endif /* is_parse_forest */

#if is_coupled
static void coupled_mem_mngr ();
static SXINT coupled_action ();
static SXINT coupled_last_pass ();
static void coupled_loop ();

void
build_coupled_guide (struct G *G)
{
    G->semact.first_pass_init = G->semact.second_pass_init = coupled_mem_mngr;
    G->semact.first_pass = G->semact.second_pass = coupled_action;
    G->semact.first_pass_final = G->semact.second_pass_final = coupled_mem_mngr; /* Le free sera fait "a la main"!! */

    /* Fait "passer" les structures du guide au parser guide' */
    G->semact.last_pass = coupled_last_pass;
#if 0
    G->semact.oflw = sxvoid;	/* Le debordement des rho ne touche pas aux structures coupled */
#endif /* 0 */
    G->semact.oflw = coupled_mem_mngr; /* Le 7/6/2002 il faut gerer cmemoP */
    G->semact.first_pass_loop = G->semact.second_pass_loop = coupled_loop;
}
#endif /* is_coupled */

#if is_rav2 || is_rav22
static BOOLEAN		is_rav2_coupled_and_not_FL_rhs, main_clause_nb;

static SXINT from2var_mngr ();
static void from2var_alloc_mngr ();
static void from2var_free_mngr ();
static void from2var_mem_mngr ();
static void from2var_loop ();

static struct G	*Rav2_Gs [PID]; /* Grammaire en forme 2var */

static SXINT from2var_last_pass (struct G **Gs, SXINT S0n, SXINT ret_val)
{
    return (*(Rav2_Gs [0]->semact.last_pass)) (&(Rav2_Gs [0]), S0n, ret_val);
}


void
rav2from2var (struct G *G)
{
    /* Assure le mapping inverse permettant le passage d'une forme 2var a la forme originelle */
    G->semact.first_pass_init = G->semact.second_pass_init = from2var_alloc_mngr;
    G->semact.first_pass = G->semact.second_pass = from2var_mngr;
    G->semact.first_pass_final = G->semact.second_pass_final = from2var_free_mngr;

    G->semact.last_pass = from2var_last_pass;
    G->semact.oflw = from2var_mem_mngr;
    G->semact.first_pass_loop = G->semact.second_pass_loop = from2var_loop;
}
#endif /* is_rav2 || is_rav22 */

#if is_shallow
static void shallow_mem_mngr ();
static SXINT shallow_action ();
static SXINT shallow_last_pass ();
static void shallow_loop ();

void
shallow_set (struct G *G)
{
    /* La semantique du parseur courant (en forme 1rcg) est un shallow parseur qui va
       recomposer grossierement et filtrer la foret partagee */
    G->semact.first_pass_init = G->semact.second_pass_init = shallow_mem_mngr;
    G->semact.first_pass = G->semact.second_pass = shallow_action;
    G->semact.first_pass_final = G->semact.second_pass_final = shallow_mem_mngr;

    G->semact.last_pass = shallow_last_pass;
    G->semact.oflw = sxvoid;	/* Le debordement des rho ne touche pas aux structures shallow */
    G->semact.first_pass_loop = G->semact.second_pass_loop = shallow_loop;
}
#endif /* is_shallow */
#endif /* is_semantics */

#if AG
static void
dag_generator_post (SXINT ret_val)
{
    /* Si ret_val est >0 l'analyse a marche' et c'est l'identifiant de l'ensemble des chaines */
    /* S0n est l'identifiant du couple axiome, (vecteur de) range [0..n] */
    SXINT next, list_id, nb, xlist, x;

    if (ret_val != -1) {
	fputs ("The following sentences have been generated:\n", stdout);

	for (next = ret_val; next != 0; next = path_refs [next].next) {
	    list_id = path_refs [next].ref;
	    nb = XH_list_lgth(paths, list_id);
	    xlist = XH_X (paths, list_id)+1;

	    for (x = 2; x < nb; x+=2) {
		fprintf (stdout, "%s", sxttext (sxplocals.sxtables, XH_list_elem (paths, xlist)));
		xlist += 2;
	    }

	    fputs ("\n", stdout);
	}
    }
    else
	fputs ("This source text does not match any sentence.\n", stdout);
}

static void
set_path (SXINT *result)
{
    /* On a un chemin complet ds stack, on le range */
    SXINT x, top, list_id, symb, xs1, xs2;
    
#if is_generator
    for (top = TOP (freres), x = 1; x <= top; x+=3) {
	xs1 = freres [x+1];
	xs2 = freres [x+2];

	if (stack [xs1] == ANY) {
	    stack [xs1] = stack [xs2];
	}

	if (stack [xs2] == ANY) {
	    stack [xs2] = stack [xs1];
	}

	if (stack [xs1] != stack [xs2]) return;
    }
#endif

    x = 1;
    top = TOP (stack);
    XH_push (paths, stack [x++]);

    while (x <= top) {
	symb = stack [x++];
#if is_generator
	if (symb == ANY)
	    /* A FAIRE, expansion obligatoire. PB des freres ds le cas StrEq */
	    sxtrap (ME, "set_path");
#endif
	XH_push (paths, symb);
	XH_push (paths, stack [x++]);
    }

    XH_set (&paths, &list_id);
    insert_source (result, list_id);
}

/* A FAIRE: si is_memo, memoization des streq et strlen ?? */

static void
gstreq (SXINT *result, SXINT local_id, SXINT lb0, SXINT ub0, SXINT lb1, SXINT ub1)
{
    SXINT x, top, list_id, triple0, q0, tok0, tok1, triple1, q1;

    if (lb0 == ub0 || lb1 == ub1) {
	if (lb0 == ub0 && lb1 == ub1) {
	    for (top = TOP(stack0), x = 1; x <= top; x++) {
		XH_push (paths, stack0 [x]);
	    }

	    XH_push (paths, ub0);
	    XH_push (paths, ARG_SEP_EQ);

	    for (top = TOP(stack1), x = 1; x <= top; x++) {
		XH_push (paths, stack1 [x]);
	    }

	    XH_push (paths, ub1);
	    XH_set (&paths, &list_id);
	    insert_source (result, list_id);
	}
    }
    else {
	XxYxZ_Xforeach (glbl_idag.idag_hd, lb0, triple0) {
	    q0 = XxYxZ_Z (glbl_idag.idag_hd, triple0);

	    if (SXBA_bit_is_set(glbl_idag.delta[q0], ub0)) {
		tok0 = XxYxZ_Y (glbl_idag.idag_hd, triple0);
#if is_generator
		if (tok0 == ANY) {
		    XxYxZ_Xforeach (glbl_idag.idag_hd, lb1, triple1) {
			q1 = XxYxZ_Z (glbl_idag.idag_hd, triple1);
				
			if (SXBA_bit_is_set(glbl_idag.delta[q1], ub1)) {
			    tok1 = XxYxZ_Y (glbl_idag.idag_hd, triple1);
			    PUSH (stack0, lb0);
			    PUSH (stack1, lb1);
			    PUSH (stack0, tok0);
			    PUSH (stack1, tok1);
			    gstreq (result, local_id, q0, ub0, q1, ub1);
			    stack0 [0] -= 2;
			    stack1 [0] -= 2;
			}
		    }
		}
		else {
		    XxYxZ_XYforeach (glbl_idag.idag_hd, lb1, ANY, triple1) {
			q1 = XxYxZ_Z (glbl_idag.idag_hd, triple1);
				
			if (SXBA_bit_is_set(glbl_idag.delta[q1], ub1)) {
			    PUSH (stack0, lb0);
			    PUSH (stack1, lb1);
			    PUSH (stack0, tok0);
			    PUSH (stack1, ANY);
			    gstreq (result, local_id, q0, ub0, q1, ub1);
			    stack0 [0] -= 2;
			    stack1 [0] -= 2;
			}
		    }

#endif /* is_generator */
		    XxYxZ_XYforeach (glbl_idag.idag_hd, lb1, tok0, triple1) {
			q1 = XxYxZ_Z (glbl_idag.idag_hd, triple1);
				
			if (SXBA_bit_is_set(glbl_idag.delta[q1], ub1)) {
			    PUSH (stack0, lb0);
			    PUSH (stack1, lb1);
			    PUSH (stack0, tok0);
			    PUSH (stack1, tok0);
			    gstreq (result, local_id, q0, ub0, q1, ub1);
			    stack0 [0] -= 2;
			    stack1 [0] -= 2;
			}
		    }
#if is_generator
		}
#endif
	    }
	}
    }
}



static BOOLEAN
streq (SXINT lb0, SXINT ub0, SXINT lb1, SXINT ub1)
{
    SXINT triple0, q0, tok0, triple1, q1;

    if (lb0 == ub0 || lb1 == ub1) {
	return lb0 == ub0 && lb1 == ub1;
    }

    XxYxZ_Xforeach (glbl_idag.idag_hd, lb0, triple0) {
	q0 = XxYxZ_Z (glbl_idag.idag_hd, triple0);

	if (SXBA_bit_is_set(glbl_idag.delta[q0], ub0)) {
	    tok0 = XxYxZ_Y (glbl_idag.idag_hd, triple0);
#if is_generator
	    /* Ici, le token source ANY matches n'importe quel token */
	    if (tok0 == ANY) {
		XxYxZ_Xforeach (glbl_idag.idag_hd, lb1, triple1) {
		    q1 = XxYxZ_Z (glbl_idag.idag_hd, triple1);

		    if (SXBA_bit_is_set(glbl_idag.delta[q1], ub1)) {
			if (streq (q0, ub0, q1, ub1))
			    return TRUE;
		    }
		}
	    }
	    else {
#endif
		XxYxZ_XYforeach (glbl_idag.idag_hd, lb1, tok0, triple1) {
		    q1 = XxYxZ_Z (glbl_idag.idag_hd, triple1);

		    if (SXBA_bit_is_set(glbl_idag.delta[q1], ub1)) {
			if (streq (q0, ub0, q1, ub1))
			    return TRUE;
		    }
		}
#if is_generator
		XxYxZ_XYforeach (glbl_idag.idag_hd, lb1, ANY, triple1) {
		    q1 = XxYxZ_Z (glbl_idag.idag_hd, triple1);

		    if (SXBA_bit_is_set(glbl_idag.delta[q1], ub1)) {
			if (streq (q0, ub0, q1, ub1))
			    return TRUE;
		    }
		}
	    }
#endif
	}
    }

    return FALSE;
}


SXINT
_GStrEq (SXINT local_id, SXINT *lb, SXINT *ub)
{
  /* Predicat predefini qui retourne l'ensemble des chaines egales entre les etats
     lb[0] et ub[0] et lb[1] et ub[1] de glbl_idag.
  */
    SXINT	result;

    RAZ (stack0);
    RAZ (stack1);
    result = 0;
    gstreq (&result, local_id, lb[0], ub[0], lb[1], ub[1]);

    return result;
}

SXINT
_StrEq (SXINT *lb, SXINT *ub)
{
  /* Predicat predefini qui verifie l'existence de chaines egales entre les etats
     lb[0] et ub[0] et lb[1] et ub[1] de glbl_idag.
  */
    return streq (lb[0], ub[0], lb[1], ub[1]);
}

static void
gstreqlen (SXINT *result, SXINT local_id, SXINT lb0, SXINT ub0, SXINT lb1, SXINT ub1)
{
    SXINT x, top, list_id, triple0, q0, triple1, q1, tok0, tok1;

    if (lb0 == ub0 || lb1 == ub1) {
	if (lb0 == ub0 && lb1 == ub1) {
	    for (top = TOP(stack0), x = ; x <= top; x++) {
		XH_push (paths, stack0 [x]);
	    }

	    XH_push (paths, ub0);
	    XH_push (paths, ARG_SEP);

	    for (top = TOP(stack1), x = 1; x <= top; x++) {
		XH_push (paths, stack1 [x]);
	    }

	    XH_push (paths, ub1);
	    XH_set (&paths, &list_id);
	    insert_source (result, list_id);
	}
    }
    else {
	XxYxZ_Xforeach (glbl_idag.idag_hd, lb0, triple0) {
	    q0 = XxYxZ_Z (glbl_idag.idag_hd, triple0);

	    if (SXBA_bit_is_set(glbl_idag.delta[q0], ub0)) {
		tok0 = XxYxZ_Y (glbl_idag.idag_hd, triple0);

		XxYxZ_Xforeach (glbl_idag.idag_hd, lb1, triple1) {
		    q1 = XxYxZ_Z (glbl_idag.idag_hd, triple1);

		    if (SXBA_bit_is_set(glbl_idag.delta[q1], ub1)) {
			tok1 = XxYxZ_Y (glbl_idag.idag_hd, triple1);
			/* Les ANY seront traites ds generator */
			PUSH (stack0, lb0);
			PUSH (stack1, lb1);
			PUSH (stack0, tok0);
			PUSH (stack1, tok1);
			gstreqlen (result, local_id, q0, ub0, q1, ub1);
			stack0 [0] -= 2;
			stack1 [0] -= 2;
		    }
		}
	    }
	}
    }
}


static BOOLEAN
streqlen (SXINT lb0, SXINT ub0, SXINT lb1, SXINT ub1)
{
    SXINT triple0, q0, triple1, q1;

    if (lb0 == ub0 || lb1 == ub1) {
	return lb0 == ub0 && lb1 == ub1;
    }

    XxYxZ_Xforeach (glbl_idag.idag_hd, lb0, triple0) {
	q0 = XxYxZ_Z (glbl_idag.idag_hd, triple0);

	if (SXBA_bit_is_set(glbl_idag.delta[q0], ub0)) {

	    XxYxZ_Xforeach (glbl_idag.idag_hd, lb1, triple1) {
		q1 = XxYxZ_Z (glbl_idag.idag_hd, triple1);

		if (SXBA_bit_is_set(glbl_idag.delta[q1], ub1)) {
		    if (streqlen (q0, ub0, q1, ub1))
			return TRUE;
		}
	    }
	}
    }

    return FALSE;
}


SXINT
_GStrEqLen (SXINT local_id, SXINT *lb, SXINT *ub)
{
  /* Predicat predefini qui retourne l'ensemble des chaines de longueur egales entre les etats
     lb[0] et ub[0] et lb[1] et ub[1] de glbl_idag.
  */
    SXINT	result;

    RAZ (stack0);
    RAZ (stack1);
    result = 0;
    gstreqlen (&result, local_id, lb[0], ub[0], lb[1], ub[1]);

    return result;
}

SXINT
_StrEqLen (SXINT *lb, SXINT *ub)
{
  /* Predicat predefini qui verifie l'existence de chaines de longueurs egales entre les etats
     lb[0] et ub[0] et lb[1] et ub[1] de glbl_idag.
  */
    return streqlen (lb[0], ub[0], lb[1], ub[1]);
}

static void
gstrlen (SXINT *result, SXINT local_id, SXINT l, SXINT lb, SXINT ub)
{
    /* Predicat predefini qui stocke ds result les chaines de longueur l entre les etats
     lb[0] et ub[0] ds glbl_idag.
     */
    SXINT	triple, q, tok;

    if (l == 0) {
	if (lb == ub) {
	    PUSH (stack, ub);
	    set_path (result);
	    stack [0]--;
	}
    }
    else {
 	XxYxZ_Xforeach (glbl_idag.idag_hd, lb, triple) {
	    q = XxYxZ_Z (glbl_idag.idag_hd, triple);

	    if (SXBA_bit_is_set(glbl_idag.delta[q], ub)) {
		tok = XxYxZ_Y (glbl_idag.idag_hd, triple);
		PUSH (stack, lb);
		PUSH (stack, tok);
		gstrlen (result, l-1, local_id, q, ub);
		stack [0] -= 2;
	    }
	}
    }
}

static BOOLEAN
strlen (SXINT l, SXINT lb, SXINT ub)
{
    /* Predicat predefini qui verifie l'existence de chaines de longueur l entre les etats
     lb[0] et ub[0] ds glbl_idag.
     */
    SXINT	triple, q;

    if (l == 0) return lb == ub;
 
    XxYxZ_Xforeach (glbl_idag.idag_hd, lb, triple) {
	q = XxYxZ_Z (glbl_idag.idag_hd, triple);

	if (SXBA_bit_is_set(glbl_idag.delta[q], ub)) {
	    if (strlen (l-1, q, ub))
		return TRUE;
	}
    }

    return FALSE;
}

SXINT
_GStrLen (SXINT l, SXINT local_id, SXINT *lb, SXINT *ub)
{
    /* Predicat predefini qui retourne l'ensemble des chaines de longueur l entre les etats
     lb[0] et ub[0] ds glbl_idag.
     */
    SXINT result;

    RAZ (stack);
    result = 0;
    gstrlen (&result, local_id, l, lb[0], ub[0]);

    return result;
}


SXINT
_StrLen (SXINT l, SXINT *lb, SXINT *ub)
{
    /* Predicat predefini qui verifie l'existence de chaines de longueur l entre les etats
     lb[0] et ub[0] ds glbl_idag.
     */
    return strlen (l, lb[0], ub[0]);
}

#if is_generator
static void
store_frere (SXINT xlist, SXINT xstack, SXINT xlist_frere)
{
    SXINT xl, x, top;

    if (xlist < xlist_frere)
	xl = xlist;
    else
	xl = xlist_frere;

    for (top = TOP (freres), x = 1; x <= top; x+=3) {
	if (freres [x] == xl) {
	    /* existe deja, on complete */
	    if (xlist == xl) {
		freres [x+1] = xstack;
	    }
	    else {
		freres [x+2] = xstack;
	    }

	    return;
	}
    }

    PUSH (freres, xl);

    if (xlist == xl) {
	PUSH (freres, xstack);
	PUSH (freres, 0); /* sera complete + tard */
    }
    else {
	PUSH (freres, 0); /* sera complete + tard */
	PUSH (freres, xstack);
    }
}
#endif /* is_generator */


static BOOLEAN
non_linearity (SXINT bot, SXINT lb, SXINT ub, SXINT fils, SXINT arg_pos)
{
    SXINT xlist, arg, elem, top, x, symb1, symb2, sep, i, orig1, orig2, xlist_frere;

    /* On cherche (fils, arg_pos) */
    orig1 = xlist = XH_X (paths, ids [fils]);
    arg = 1;

    while (arg < arg_pos) {
	if ((symb1 = XH_list_elem (paths, xlist++)) <= ARG_SEP_EQ) {
	    sep = symb1;
	    orig2 = xlist;
	    arg++;
	}
    }

    for (;;) {
	if (XH_list_elem (paths, xlist++) == lb) {
	    break;
	}

	xlist++;
    }

    for (top = TOP(stack), x = bot; x < top; x++) {
	symb1 = stack[x];
	symb2 = XH_list_elem (paths, xlist++);
#if is_generator
	if (symb2 == ANY) {
	    /* On est ds StrEq ou StrEqLen */
	    if (arg_pos == 1) {
		/* on n'a pas rencontre' le separateur */
		orig2 = (XH_X (paths, ids [fils]+1)+orig1-1)/2; /* il se trouve au milieu */
		sep = XH_list_elem (paths, orig2++);
	    }

	    if (sep == ARG_SEP_EQ) {
		/* On est ds StrEq */
		/* On regarde si le frere de symb a deja ete traite */
		xlist_frere = xlist-1 + ((arg_pos == 1) ? orig2-orig1 : orig1-orig2);
		store_frere (xlist, x, xlist_frere);
	    }
	}
#endif
	if (symb1 != symb2) {
#if is_generator
	    if (symb1 == ANY) {
		stack[x] = symb2;
	    }
	    else if (symb2 != ANY)
#endif
		return fils;
	}
    }

    return (XH_list_elem (paths, xlist) == ub) ? 0 : fils;
}


static SXINT
Tinstance (SXINT symb, SXINT fils, SXINT lb, SXINT ub)
{
    /* Traitement d'un LHS terminal symb entre lb et ub */
    /* S'il a des occur en RHS, on verifie qu'elles figurent bien ds les
       chaines correspondantes */
    SXINT arg_pos, xlist, arg, elem, lim;

    do {
	arg_pos = *Xoccur++;
	/* symb doit se trouver en RHS dans ids[fils][arg_pos] */

	xlist = XH_X (paths, ids [fils]);
	lim = XH_X (paths, ids [fils]+1);
	arg = 1;

	while (arg < arg_pos) {
	    if (XH_list_elem (paths, xlist++) <= ARG_SEP_EQ)
		arg++;
	}

	for (;;) {
	    if (xlist >= lim) return fils;
	    elem = XH_list_elem (paths, xlist++);
	    if (elem <= ARG_SEP_EQ) return fils;
	    if (elem == lb) break;
	}

	if (XH_list_elem (paths, xlist++) != symb || XH_list_elem (paths, xlist) != ub) return fils;

    } while ((fils = *Xoccur++) != 0);

    return 0;
}


static SXINT
Xinstance (SXINT fils, SXINT lb, SXINT ub)
{
    /* Traitement d'une variable instanciee entre lb et ub */
    /* Elle est top-down non-erasing */
    /* En RHS, elle ne contient plus que des triplets du idag */
    SXINT cur_top, arg_pos, xlist, arg, b, nfils, narg_pos, echec, symb, sep, i, orig1, orig2, xlist_frere;

    cur_top = TOP(stack);

    arg_pos = *Xoccur++;
    /* X se trouve en RHS dans ids[fils][arg_pos] */

    orig1 = xlist = XH_X (paths, ids [fils]);
    arg = 1;

    while (arg < arg_pos) {
	if ((symb = XH_list_elem (paths, xlist++)) <= ARG_SEP_EQ) {
	    sep = symb;
	    orig2 = xlist;
	    arg++;
	}
    }

    for (;;) {
	if (XH_list_elem (paths, xlist++) == lb) {
	    break;
	}

	xlist++;
    }

    /* lb est deja copie'e, on ne copie pas ub */
    for (;;) {
	symb = XH_list_elem (paths, xlist++);
#if is_generator
	if (symb == ANY) {
	    /* On est ds StrEq ou StrEqLen */
	    if (arg_pos == 1) {
		/* on n'a pas rencontre' le separateur */
		orig2 = (XH_X (paths, ids [fils]+1)+orig1-1)/2; /* il se trouve au milieu */
		sep = XH_list_elem (paths, orig2++);
	    }

	    if (sep == ARG_SEP_EQ) {
		/* On est ds StrEq */
		/* On regarde si le frere de symb a deja ete traite */
		xlist_frere = xlist-1 + ((arg_pos == 1) ? orig2-orig1 : orig1-orig2);
		store_frere (xlist-1, TOP (stack), xlist_frere);
	    }
	}
#endif

	PUSH (stack, symb);

	if ((b = XH_list_elem (paths, xlist++)) == ub)
	    break;

	PUSH (stack, b);
    }

    while ((nfils = *Xoccur++) != 0) {
	/* cas de la non-linearite */
	narg_pos = *Xoccur++;
	/* On verifie que la chaine entre cur_top et stack[top] est identique a celle
	   definie par nfils, narg_pos */
	if ((echec = non_linearity (cur_top, lb, ub, nfils, narg_pos)) > 0) return echec;
    }

    return 0;
}


static SXINT from_Xoccur (SXINT lb);

static SXINT
all_paths (SXINT lb, SXINT ub)
{
    SXINT		triple, q, symb, echec;

    /* Comme X est top-down erasing, un echec, detecte ds from_Xoccur est independant de
       all_paths, il est donc inutile d'essayer les autres combinaisons qui produiront
       elles aussi un echec */
    if (lb == ub) return from_Xoccur (ub);

    /* on construit tous les chemins entre lb et ub */
    XxYxZ_Xforeach (glbl_idag.idag_hd, lb, triple) {
	q = XxYxZ_Z (glbl_idag.idag_hd, triple);

	if (SXBA_bit_is_set(glbl_idag.delta[q], ub)) {
	    symb = XxYxZ_Y (glbl_idag.idag_hd, triple);
	    PUSH (stack, symb);
	    PUSH (stack, q);

	    if ((echec = all_paths (q, ub)) > 0) return echec;

	    stack [0] -= 2;
	}
    }

    return 0;
}


static SXINT
from_Xoccur (SXINT lb)
{
    SXINT		symb, *ptr, ub, fils, echec;
    BOOLEAN 	ret_val;

    while (Xoccur < Xoccur_lim) {
	PUSH (stack, lb);

	if ((symb = *Xoccur++) == ARG_SEP) {
	    PUSH (stack, ARG_SEP);
	    lb = *ilb++;
	}
	else {
	    /* On cherche l'ub */
	    for (ptr = Xoccur; *ptr; ptr++);

	    ub = (ptr == Xoccur_lim || ptr[1] == ARG_SEP) ? *iub++ : *indexes++;

	    if (symb > 0) {
		/* C'est un terminal */
		if ((fils = *Xoccur++) != 0) {
		    /* Au moins une occur en RHS */
		    if ((echec = Tinstance (symb, fils, lb, ub)) > 0) return echec;
		}

		PUSH (stack, symb);
	    }
	    else {
		/* C'est une variable X */
		if ((fils = *Xoccur++) == 0) {
		    /* X est top-down erasing, on etend entre -lb et -ub */
		    /* A FAIRE plusieurs occurrences du meme X en LHS => meme chaine */
		    return all_paths (lb, ub);
		}

		if ((echec = Xinstance (fils, lb, ub)) > 0) return echec;
	    }

	    lb = ub;
	}
    }
    
    PUSH (stack, lb);
    set_path (ret_vals);

    return 0;
}

static  SXINT
bu_next_son (SXINT son, SXINT *XOCCUR, SXINT *INDEXES, SXINT *ILB, SXINT *IUB)
{
    /* La valeur retournee est nulle si ca a marche'.  Sinon, c'est le numero du fils qui a fait
       echoue' la verification. */
    SXINT next, echec;

    if (son <= son_nb) {
	for (next = ret_vals [son]; next != 0; next = path_refs [next].next) {
	    ids [son] = path_refs [next].ref;
	    echec = bu_next_son (son+1, XOCCUR, INDEXES, ILB, IUB);

	    if (0 < echec && echec < son)
		return echec;
	}

	
	return 0;
    }

    RAZ (stack);
#if is_generator
    RAZ (freres);
#endif
    Xoccur = XOCCUR;
    indexes = INDEXES;
    ilb = ILB;
    iub = IUB;
    Xoccur_lim = Xoccur+*Xoccur++;
    return from_Xoccur (*ilb++);

}


SXINT
generator (SXINT SON_NB, SXINT *RET_VALS, SXINT *XOCCUR, SXINT *INDEXES, SXINT *ILB, SXINT *IUB)
    /* ret_vals est un tableau de taille sons_nb+1.
       ret_vals[i], i>0 est l'identifiant de l'ensemble des chaines associees au ieme fils
       ret_vals[0] est pour la LHS, si ret_vals[0]==0 c'est la premiere fois qu'on fait une reduc
       vers (A, rho[0]), clause est une A_clause */
    /* Composition de clause_args :
       clause_args_size, lhs_arg_nb, son1_xolb, son1_arg_nb, ..., sonson_nb_xolb, sonson_nb_arg_nb,
       streq1_xolb, ..., streqh_xolb */
    /* Composition de XOCCUR:
       top, arg, ARG_SEP, arg, ...,ARG_SEP, arg
       ou arg est la sequence symb 0 symb 0 ... symb 0
       ou symb est la sequence code , {prdct, arg} ou code > 0 est un terminal et code <0 est une variable
       chaque couple prdct,arg indique le prdct et un argument en RHS ou code apparait.
       Si le nombre de couples prdct,arg est >1 => la rcg est non-lineaire */
    /* Si la LHS a la forme (aXbY, cZ), INDEXES[0] est l'etat entre a et X, INDEXES[1] est l'etat entre X et b,
       INDEXES[2] est l'etat entre b et Y et  INDEXES[3] est l'etat entre c et Z.  Les limites des ranges sont
       ds ILB, IUB. */
{
    son_nb = SON_NB;
    ret_vals = RET_VALS;
    bu_next_son (1, XOCCUR, INDEXES, ILB, IUB);

    return ret_vals [0];
}
#else /* !AG */
#if is_generator
static BOOLEAN
check_neg_calls ()
{
    /* Il y a eu des appels negatifs en RHS dont on n'en a pas encore tenu compte, on les regarde */
    /* On suppose que si 2 xstack differents designent le meme source index, ils reperent
       des symb identiques */
    SXINT lb, x, i, next, prdct_id, arg_pos, bot, top, arg_id, bot2, top2, symb1, symb2, kind, cstrnt1, cstrnt2;
    SXINT neg_calls_top, nb;

    neg_calls_top = TOP (neg_calls);
    
    for (i = 1; i <= neg_calls_top; i += 2) {
	prdct_id = neg_calls [i];
	arg_pos = neg_calls [i+1];

	if (prdct_id > 1) {
	    bot = XH_X (paths, prdct_id);
	    top = XH_X (paths, prdct_id+1);

	    while (bot < top) {
		arg_id = XH_list_elem (paths, bot);

		if (arg_id > 1) {
		    top2 = XH_X (paths, arg_id+1);
		    lb = olb [arg_pos-1];
		    nb = si2any_nb [lb];

		    for (bot2 = XH_X (paths, arg_id); bot2 < top2; bot2++) {
			nb++;
			lb = any_nb2si [nb];
			symb1 = stack [si2xs [lb]];
			symb2 = XH_list_elem (paths, bot2);

			/* verifier la compatibilite negative entre symb1 et symb2 */
			/* Si symb1 et symb2 sont incompatibles, on peut passer au "next" suivant (il sera
			   inutile de conserver ce "next" */
			/* Si symb1 est strictement + general que symb2, on peut passer au "next" suivant */
			if (symb1 != symb2) {
			    if (symb1 == ANY)
				kind = 1; /* symb1 est strictement + general que symb2 */
			    else {
				if (symb2 == ANY)
				    kind = -1; /* symb1 est strictement - general que symb2 */
				else {
				    cstrnt1 = symb1 & CPART;
				    symb1 &= BPART;
				    cstrnt2 = symb2 & CPART;
				    symb2 &= BPART;

				    if (cstrnt1 && cstrnt2) {
					/* cas !a et !b */
					/* On dit que symb1 est +general car !a-!b contient b */
					kind = 1; /* symb1 est strictement + general que symb2 */
				    }
				    else {
					if (cstrnt1) {
					    if (symb1 == symb2)
						/* !a et a */
						kind = 2; /* symb1 et symb2 sont incompatibles */
					    else
						/* !a et b */
						kind = 1; /* symb1 est strictement + general que symb2 */
					}
					else {
					    if (cstrnt2) {
						if (symb1 == symb2)
						    /* a et !a */
						    kind = 2; /* symb1 et symb2 sont incompatibles */
						else
						    /* a et !b */
						    kind = -1; /* symb1 est strictement - general que symb2 */
					    }
					    else {
						/* a et b */
						kind = 2; /* symb1 et symb2 sont incompatibles */
					    }
					}
				    }
				}
			    }
			}
			else
			    kind = 0; /* symb1 == symb2 */

			if (kind > 0) {
			    /* symb1 et symb2 sont incompatibles ou symb1 est strictement + general que symb2 */
			    /* on passe au "next" suivant */
			    break;
			}
		    }

		    if (bot2 < top2)
			break;
		}
		    
		bot++;
		arg_pos++;
	    }

	    if (bot > top) {
		/* Ici, chaque symb1 est -general (ou egal) au symb2 correspondant, le resultat
		   de la difference est donc vide */
		return FALSE;
	    }
	}
    }

    return TRUE;
}

static void
put_in_args_id (SXINT *stack, SXINT cstrnt)
{
    SXINT top, x, symb, arg_id;

    if ((top = TOP (stack)) > 0) {
	for (x = 1; x <= top; x++) {
	    symb = stack [x];

	    if (symb == ARG_SEP) {
		XH_set (&paths, &arg_id);
		PUSH (args_id, arg_id|cstrnt);
	    }
	    else {
		XH_push (paths, symb);
	    }
	}

	XH_set (&paths, &arg_id);
	PUSH (args_id, arg_id|cstrnt);
    }
}

static void
set_neg_calls ()
{
    SXINT top_stack, arg, lb, x, i, next, prdct_id, arg_pos, bot, top, arg_id, bot2, top2, symb1, symb2, kind, cstrnt1, cstrnt2;
    SXINT neg_calls_top, nb;

    RAZ (w3stack);
    top_stack = TOP (stack);

    neg_calls_top = TOP (neg_calls);
    
    for (i = 1; i <= neg_calls_top; i += 2) {
	prdct_id = neg_calls [i];
	arg_pos = neg_calls [i+1];

	if (prdct_id > 1) {
	    RAZ (w1stack);

	    for (x = 1; x <= top_stack; x++) {
		if (stack [x] == ARG_SEP) {
		    PUSH (w1stack, ARG_SEP);
		}
		else {
		    PUSH (w1stack, ANY);
		}
	    }
		
	    bot = XH_X (paths, prdct_id);
	    top = XH_X (paths, prdct_id+1);

	    while (bot < top) {
		arg_id = XH_list_elem (paths, bot);

		if (arg_id > 1) {
		    top2 = XH_X (paths, arg_id+1);
		    lb = olb [arg_pos-1];
		    nb = si2any_nb [lb];

		    for (bot2 = XH_X (paths, arg_id); bot2 < top2; bot2++) {
			nb++;
			lb = any_nb2si [nb];
			symb1 = stack [si2xs [lb]];
			symb2 = XH_list_elem (paths, bot2);

			/* verifier la compatibilite negative entre symb1 et symb2 */
			/* Si symb1 et symb2 sont incompatibles, on peut passer au "next" suivant (il sera
			   inutile de conserver ce "next" */
			/* Si symb1 est strictement + general que symb2, on peut passer au "next" suivant */
			if (symb1 != symb2) {
			    if (symb1 == ANY)
				kind = 1; /* symb1 est strictement + general que symb2 */
			    else {
				if (symb2 == ANY)
				    kind = -1; /* symb1 est strictement - general que symb2 */
				else {
				    cstrnt1 = symb1 & CPART;
				    symb1 &= BPART;
				    cstrnt2 = symb2 & CPART;
				    symb2 &= BPART;

				    if (cstrnt1 && cstrnt2) {
					/* cas !a et !b */
					/* On dit que symb1 est +general car !a-!b contient b */
					kind = 1; /* symb1 est strictement + general que symb2 */
				    }
				    else {
					if (cstrnt1) {
					    if (symb1 == symb2)
						/* !a et a */
						kind = 2; /* symb1 et symb2 sont incompatibles */
					    else
						/* !a et b */
						kind = 1; /* symb1 est strictement + general que symb2 */
					}
					else {
					    if (cstrnt2) {
						if (symb1 == symb2)
						    /* a et !a */
						    kind = 2; /* symb1 et symb2 sont incompatibles */
						else
						    /* a et !b */
						    kind = -1; /* symb1 est strictement - general que symb2 */
					    }
					    else {
						/* a et b */
						kind = 2; /* symb1 et symb2 sont incompatibles */
					    }
					}
				    }
				}
			    }
			}
			else
			    kind = 0; /* symb1 == symb2 */

			if (kind == 2) {
			    /* symb1 et symb2 sont incompatibles next est inutile */
			    /* on passe au "next" suivant */
			    break;
			}

			w1stack [si2xs [lb]] = symb2;
		    }

		    if (bot2 < top2)
			break;
		}
		    
		bot++;
		arg_pos++;
	    }

	    if (bot >= top) {
		/* On stocke w1stack */
		RAZ (w2stack);

		for (x = 1; x <= top_stack; x++) {
		    symb1 = w1stack [x];

		    if (symb1 == ARG_SEP) {
			XH_set (&paths, &arg_id);
			PUSH (w2stack, arg_id);
		    }
		    else {
			XH_push (paths, symb1);
		    }
		}

		XH_set (&paths, &arg_id);
		PUSH (w2stack, arg_id);

		for (x = 1; x <= TOP (w2stack); x++)
		    XH_push (paths, w2stack [x]);

		XH_set (&paths, &arg_id);
		PUSH (w3stack, arg_id);
	    }
	}
    }

    if (top = TOP (w3stack)) {
	put_in_args_id (w3stack, NEG);
    }
}

static void
set_path (SXINT *result)
{
    /* On a un chemin complet ds stack, on le range */
    /* freres contient les doublets des egalites */
    SXINT		x, top_freres, top_args_id, top_stack, list_id, symb1, symb2, xs1, xs2, d, arg_pos, elem, arg_id, eq_id;
    SXINT		cstrnt, cstrnt1, cstrnt2, neg_calls_top, top_eq_ids, top, bot, si1, si2, bsi1, bsi2;
    BOOLEAN	has_changed;
    
    RAZ (freres);

    if ((top_eq_ids = TOP (eq_ids)) > 0) {
	/* On commence par verifier la compatibilite des "egalites" */

	for (x = 1; x <= top_eq_ids; x++) {
	    eq_id = eq_ids [x];
	    top = XH_X (paths, eq_id+1);

	    for (bot = XH_X (paths, eq_id); bot < top; bot++) {
		PUSH (freres, XH_list_elem (paths, bot));
	    }
	}

	has_changed = TRUE;
	
	while ((top_freres = TOP (freres)) > 0 && has_changed) {
	    has_changed = FALSE;
	    RAZ (freres);

	    for (x = 1; x <= top_freres; x += 2) {
		si1 = freres [x];
		cstrnt = si1 & CPART;
		bsi1 = si1&BPART;
		xs1 = si2xs [bsi1];
		symb1 = stack [xs1];

		si2 = freres [x+1];
		bsi2 = si2&BPART;
		xs2 = si2xs [bsi2];
		symb2 = stack [xs2];

		if (symb1 == ANY && symb2 == ANY) {
		    /* Il faudra (peut etre) ajouter une partie ARG_SEP_EQ */
		    PUSH (freres, si1);
		    PUSH (freres, si2);
		}
		else {
		    cstrnt1 = symb1&CPART;
		    cstrnt2 = symb2&CPART;

		    if (cstrnt == 0) {
			/* Compatibilite entre symb1 et symb2 vis-a-vis de l'EGALITE */
			if (symb1 != symb2) {
			    if (symb1 == ANY) {
				stack [xs1] = symb2;
				has_changed = TRUE;

				if (cstrnt2) {
				    /* On maintient la contrainte d'egalite */
				    PUSH (freres, si1);
				    PUSH (freres, si2);
				}
			    }
			    else {
				if (symb2 == ANY) {
				    stack [xs2] = symb1;
				    has_changed = TRUE;

				    if (cstrnt1) {
					/* On maintient la contrainte d'egalite */
					PUSH (freres, si1);
					PUSH (freres, si2);
				    }
				}
				else {
				    if (cstrnt1 && cstrnt2) {
					/* On laisse, on a : !a et !b avec a != b */
					PUSH (freres, si1);
					PUSH (freres, si2);
				    }
				    else {
					if (cstrnt1 || cstrnt2) {
					    if (symb1&BPART == symb2&BPART)
						/* a et !a ou b et !b */
						return;

					    if (cstrnt2 == 0) {
						/* !b et a => on met a */
						stack [xs1] = symb2;
						has_changed = TRUE;
					    }
					}
					else
					    /* a et b => faux */
					    return;
				    }
				}
			    }
			}
			else {
			    /* egalite entre symb1 et symb2, on maintient la contrainte d'egalite si ce sont des NOT */
			    if (cstrnt2) {
				PUSH (freres, si1);
				PUSH (freres, si2);
			    }
			}
		    }
		    else {
			/* Compatibilite entre symb1 et symb2 vis-a-vis de l'INEGALITE */
			if (symb1 == ANY) {
			    /* On met !symb2 */
			    stack [xs1] = symb2^NOT;
			    has_changed = TRUE;
			}
			else {
			    if (symb2 == ANY) {
				stack [xs2] = symb1^NOT;
				has_changed = TRUE;
			    }
			    else {
				if (cstrnt1 && cstrnt2) {
				    /* On laisse, on a : !a et !b avec a et b qcq */
				    PUSH (freres, si1);
				    PUSH (freres, si2);
				}
				else {
				    if (cstrnt1 || cstrnt2) {
					if (symb1&BPART == symb2&BPART) {
					    /* a et !a sont inegaux */
					}
					else {
					    /* !a et b ou a et !b, on laisse la specif d'inegalite */
					    PUSH (freres, si1);
					    PUSH (freres, si2);
					}
				    }
				    else {
					/* a et b */
					if (symb1 == symb2)
					    /* a == b => erreur */
					    return;
				    }
				}
			    }
			}
		    }
		}
	    }
	}

    }

    top_freres = TOP (freres);

    if ((neg_calls_top = TOP (neg_calls)) && !check_neg_calls ())
	return;

    RAZ (args_id);
    put_in_args_id (stack, 0);

    if (top_freres)
	put_in_args_id (freres, EQ);
    
    if (neg_calls_top)
	set_neg_calls ();

    for (top_args_id = TOP (args_id), x = 1; x <= top_args_id; x++) {
	XH_push (paths, args_id [x]);
    }

    XH_set (&paths, &list_id);
    insert_source (result, list_id);
}


static BOOLEAN
is_compatible (SXINT x, SXINT xlist)
{
    SXINT symb1, symb2, cstrnt1, cstrnt2;

    symb1 = stack [x];
    symb2 = XH_list_elem (paths, xlist);

    if (symb1 != symb2) {
	if (symb1 == ANY) {
	    stack[x] = symb2;
	}
	else {
	    if (symb2 == ANY)
		stack [x] = symb1;
	    else {
		cstrnt1 = symb1&CPART;
		cstrnt2 = symb2&CPART;

		if (cstrnt1 && cstrnt2)
		    sxtrap (ME, "is_compatible: case not yet implemented"); /* !a et !b */

		if (cstrnt1 || cstrnt2) {
		    if (symb1&BPART == symb2&BPART)
			/* a et !a ou b et !b */
			return FALSE;

		    if (cstrnt2 == 0)
			stack [x] = symb2;
		}
		else
		    /* a et b */
		    return FALSE;
	    }
	}
    }

    return TRUE;
}

static BOOLEAN
non_linearity (SXINT bot, (SXINT lb_any_nb, (SXINT ub_any_nb, (SXINT arg_pos)
{
    SXINT xlist, x, symb1, symb2, i, xlist_frere, cstrnt1, cstrnt2, nb;

    while ((arg_pos = *Xoccur++) != 0) {
	if ((xlist = rhs [arg_pos].bot) > 0) {
	    /* La lb du range instanciation de arg est i */
	    xlist += lb_any_nb - si2any_nb [olb [arg_pos-1]];
	    nb = lb_any_nb;
	    x = bot; /* adresse ds stack ou a ete rangee la premiere instance de X */

	    while (++nb <= ub_any_nb) {
		if (!is_compatible (x, xlist))
		    return rhs [arg_pos].son;

		x++;
		xlist++;
	    }
	}
    }

    return 0;
}


static SXINT
Xinstance (SXINT arg_pos, SXINT lb_any_nb, SXINT ub_any_nb)
{
    /* Traitement d'une variable instanciee entre lb et ub */
    /* Elle est top-down non-erasing */
    SXINT cur_top, xlist, b, echec, symb, i, nb;

    while (arg_pos != 0) {
	if ((xlist = rhs [arg_pos].bot) == 0) {
	    /* Cet un arg d'un predicat dont les chaines generees sont sans importance Ex StrLen, StrEqLen */
	    arg_pos = *Xoccur++; /* Occurrence suivante */
	}
	else {
	    cur_top = TOP (stack);
	    /* La lb du range instanciation de arg est i */
	    xlist += lb_any_nb - si2any_nb [olb [arg_pos-1]];
	    nb = lb_any_nb;

	    while (++nb <= ub_any_nb) {
		symb = XH_list_elem (paths, xlist);
		PUSH (stack, symb);
		i = any_nb2si [nb];
		si2xs [i] = TOP (stack);

		xlist++;
	    }

	    break;
	}
    }

    if (arg_pos == 0) {
	/* C'est comme si X etait top-down erasing, on stocke les ANY entre lb et ub */
	while (++lb_any_nb <= ub_any_nb) {
	    PUSH (stack, ANY);
	    i = any_nb2si [lb_any_nb];
	    si2xs [i] = TOP (stack);
	} 
    }
    else {
	/* cas de la non-linearite */
	/* On verifie que la chaine entre cur_top et stack[top] est identique a celle
	   definie par nfils, narg_pos */
	if ((echec = non_linearity (cur_top+1, lb_any_nb, ub_any_nb, arg_pos)) > 0) return echec;
    }

    return 0;
}


static SXINT
Tinstance (SXINT arg_pos, SXINT ub_any_nb)
{
    /* Traitement d'un LHS terminal range en TOP (stack) a l'index ub du source (C'est ANY) */
    /* il a des occur en RHS, on verifie qu'elles figurent bien ds les chaines correspondantes */
    SXINT top_stack, xlist, symb2, i;

    top_stack = TOP (stack);

    do {
	if ((xlist = rhs [arg_pos].bot) != 0) {
	    /* La lb du range instanciation de arg est i */
	    /* On cherche ou se trouve ub */
	    xlist += ub_any_nb - si2any_nb [olb [arg_pos-1]+1];

	    if (!is_compatible (top_stack, xlist))
		return rhs [arg_pos].son;
	}
    } while ((arg_pos = *Xoccur++) != 0);

    return 0;
}

static SXINT
put_in_paths (SXINT *stack, SXINT *cstack)
{
    /* stack, cstack contiennent respectivement une chaine et ses contraintes */
    /* On la "range" ds paths */
    SXINT x, top_args_id, list_id, arg_id;

    RAZ (args_id);
    put_in_args_id (stack, 0);

    if (TOP (cstack))
	put_in_args_id (cstack, EQ);

    for (top_args_id = TOP (args_id), x = 1; x <= top_args_id; x++) {
	XH_push (paths, args_id [x]);
    }
    
    XH_set (&paths, &list_id);

    return list_id;
}


static  SXINT merge_sons ();
static  SXINT
neg_merge (SXINT next, SXINT nt, SXINT rhs_arg_nb, SXINT son, SXINT arg_pos)
{
    SXINT		prdct_id, bot, top, eq_id, arg_id, bot1, top1, xlist, symb, x, bot2, top2, elem1, elem2, cstrnt, xlist_frere;
    SXINT		i, top0, x1, blck_nb, x0, blck_id, xs1, xs2, symb1, symb2, y, pos, xs, nb;
    struct rhs	*rhs_ptr;

    if (next == 0) {
	/* On a ds negstack,cnegstack un element de la forme disjonctive du complement qu'on est en train
	   de calculer, on le range ds paths et ds rhs et on continue le calcul */
	prdct_id = put_in_paths (cstack0, cstack1);
	/* prdct_id = put_in_paths (negstack, cnegstack); */
	rhs_ptr = rhs + arg_pos;
		
	if (prdct_id == 1) {
	    /* Le contenu de *rhs_ptr est non utilise' */
	    rhs_ptr += rhs_arg_nb;
	}
	else {
	    bot = XH_X (paths, prdct_id);
	    top = XH_X (paths, prdct_id+1);
	    
	    /* Il n'y a pas de NEG */
	    eq_id = XH_list_elem (paths, top-1);

	    if (eq_id & EQ) {
		/* Il y a une specif d'egalite sur les args */
		top--;
		eq_id ^= EQ;
		PUSH (eq_ids, eq_id);
	    }

	    while (bot < top) {
		rhs_ptr->son = son;
		arg_id = XH_list_elem (paths, bot);
		rhs_ptr->bot = XH_X (paths, arg_id);
		rhs_ptr++;
		bot++;
	    }
	}

	return  merge_sons (son+1, arg_pos+rhs_arg_nb);
    }

    prdct_id = path_refs [next].ref;
    bot = XH_X (paths, prdct_id);
    top = XH_X (paths, prdct_id+1);
	    
    if ((eq_id = XH_list_elem (paths, top-1)) & EQ) {
	/* Il y a une specif d'egalite sur les args */
	top--;
	eq_id ^= EQ;
    }
    else
	eq_id = 0;

    /* On calcule le nb de blocs independants de prdct_id */
    RAZ (stack0);
    RAZ (stack2);

    pos = arg_pos-1;
    
    while (bot < top) {
	arg_id = XH_list_elem (paths, bot);
	bot1 = XH_X (paths, arg_id);
	top1 = XH_X (paths, arg_id+1);
	i = olb [pos];
	nb = si2any_nb [i];

	for (xlist = bot1; xlist < top1; xlist++) {
	    if (eq_id)
		PUSH (stack2, 0);

	    PUSH (stack0, XH_list_elem (paths, xlist));
	    nb++;
	    i = any_nb2si [nb];
	    si2xs [i] = TOP (stack0);
	}

	bot++;
	pos++;

	if (bot < top) {
	    if (eq_id)
		PUSH (stack2, 0);

	    PUSH (stack0, ARG_SEP);
	}
    }

    top = TOP (stack0);
    RAZ (stack1);

    if (eq_id > 0) {
	bot2 = XH_X (paths, eq_id);
	top2 = XH_X (paths, eq_id+1);

	while (bot2 < top2) {
	    xs = XH_list_elem (paths, bot2);
	    PUSH (stack1, xs);
	    /* On marque les elements contraints de stack0 */
	    stack2 [si2xs [xs&BPART]] = -1;
	    bot2++;
	}

	top1 = TOP (stack1);

	for (x = 1; x <= top1; x++) {
	    /* On marque les elements contraints de stack0 */
	    stack2 [stack1 [x]] = -1;
	}
    }
    else
	top1 = 0;

    /* Ici, stack0 et stack2 sont // a negstack et stack1 contient les couples de contraintes */
    /* Si stack2 est negatif, l'element correspondant fait parti d'au moins un couple de contraintes */
    /* Pour chaque bloc independant, on calcule son complementaire */

    top0 = TOP (stack0);

    for (x1 = -1; x1 <= top1; x1 += 2) {
	if (x1 < 0) {
	    /* on traite les elements non contraints */
	    blck_nb = 1;
	    RAZ (cstack1);

	    while (blck_nb >= 1) {
		blck_id = 0;
		RAZ (cstack0);
	    
		for (x0 = 1; x0 <= top0; x0++) {
		    if (top1 > 0 && stack2 [x0] < 0) {
			/* C'est un element d'un couple contraint */
			PUSH (cstack0, ANY);
		    }
		    else {
			blck_id++;

			if (blck_id == blck_nb) {
			    symb = stack0 [x0];

			    if (symb == ANY)
				/* Complementaire vide */
				break;

			    PUSH (cstack0, symb ^ NOT);
			}
			else
			    PUSH (cstack0, ANY);
		    }
		}

		if (x0 > top0 && blck_id >= blck_nb) {
		    /* On fusionne avec negstack */
		    neg_merge (0, nt, rhs_arg_nb, son, arg_pos);
		    /* merge_in_negstack (next, nt, rhs_arg_nb, son, arg_pos); */
		    blck_nb++;
		}
		else
		    /* C'est fini pour cette partie */
		    blck_nb = 0;
	    }
	}
	else {
	    /* cas avec contraintes */
	    RAZ (cstack0);
	    RAZ (cstack1);
	    xs1 = stack1 [x1];
	    PUSH (cstack1, xs1 ^ NOT);
	    cstrnt = xs1 & CPART;
	    xs1 = si2xs [xs1 & BPART];
	    xs2 = stack1 [x1+1];
	    PUSH (cstack1, xs2 ^ NOT);
	    xs2 = si2xs [stack1 [x1+1] & BPART];

	    for (x0 = 1; x0 <= top0; x0++) {
		if (stack2 [x0] < 0) {
		    /* C'est un element d'un couple contraint */
		    if (x0 == xs1 || x0 == xs2) {
			/* de celui-la */
			PUSH (cstack0, stack0 [x0]);
		    }
		    else {
			PUSH (cstack0, ANY);
		    }
		}
		else {
		    if (stack0 [x0] == ARG_SEP)
			PUSH (cstack0, ARG_SEP);
		    else
			PUSH (cstack0, ANY);    
		}
	    }

	    neg_merge (0, nt, rhs_arg_nb, son, arg_pos);
	    /* merge_in_negstack (next, nt, rhs_arg_nb, son, arg_pos);  */
	}
    }
}

static BOOLEAN
is_pure (SXINT next)
{
    /* On verifie si chaque chaine de l'ensemble multiple commencant en next est pur */
    /* Une chaine est pure si elle ne contient ni contrainte ni negation */
    /* On pourrait peut etre conserver les contraintes */
    SXINT prdct_id, top;

    do {
	prdct_id = path_refs [next].ref;
	top = XH_X (paths, prdct_id+1)-1;
	    
	if (XH_list_elem (paths, top) & (NEG|EQ))
	    return FALSE;

    } while ((next = path_refs [next].next) > 0);

    return TRUE;
}


static BOOLEAN
is_NEG (SXINT next)
{
    SXINT prdct_id, top;

    prdct_id = path_refs [next].ref;
    top = XH_X (paths, prdct_id+1)-1;
    return (XH_list_elem (paths, top) & NEG) != 0;
}


static  SXINT
merge_sons (SXINT son, SXINT arg_pos)
{
    /* La valeur retournee est nulle si ca a marche'.  Sinon, c'est le numero du fils qui a fait
       echoue' la verification. */
    SXINT		next, echec, prdct_id, bot, top, eq_id, arg_id, rhs_arg_nb, nt, i;
    SXINT		*rhs_prdct, is_neg_call, bot2, top2, top_eq_ids, top_neg_calls, lb_any_nb, ub_any_nb;
    SXINT		symb, *ptr, lb, ub, x, pos, lhs_arg, *symbs, *symbs_lim;
    SXINT		*IUB, *INDEXES;
    struct rhs	*rhs_ptr;

    if (son <= son_nb) {
	bot = G->prdct2args_disp [rhs_prdcts [son]];
	rhs_prdct = G->prdct2args + bot;
	is_neg_call = *rhs_prdct++; /* A FAIRE : traitement des appels negatifs */
	nt = *rhs_prdct;
	rhs_arg_nb = G->nt2arity [nt];
	next = ret_vals [son];
	top_eq_ids = TOP (eq_ids);
	top_neg_calls = TOP (neg_calls);

	if (nt > 0 || nt == STREQ_ic) {
	    if (next < 0) {
		/* En Chantier */
		next = -next;

		if ((i = path_refs [next].next) != 0 && !is_pure (next) || i == 0 && is_NEG (next))
		    sxtrap (ME, "merge_sons : this negative case is not yet implemented");

		if (i == 0) {
		    /* singleton sans specif de negations */
		    echec = neg_merge (next, nt, rhs_arg_nb, son, arg_pos);
		}
		else {
		    rhs_ptr = rhs + arg_pos;

		    for (i = 0; i < rhs_arg_nb; i++) {
			rhs_ptr->son = son;
			rhs_ptr->bot = 0;
			rhs_ptr++;
		    }

		    for (; next != 0; next = path_refs [next].next) {
			prdct_id = path_refs [next].ref;
			PUSH (neg_calls, prdct_id);
			PUSH (neg_calls, arg_pos);
		    }
		
		    echec = merge_sons (son+1, arg_pos+rhs_arg_nb);
		}

		TOP (eq_ids) = top_eq_ids;
		TOP (neg_calls) = top_neg_calls;

		if (0 < echec && echec < son)
		    return echec;

		return 0;
	    }

	    for (; next != 0; next = path_refs [next].next) {
		prdct_id = path_refs [next].ref;
		rhs_ptr = rhs + arg_pos;
		
		if (prdct_id == 1) {
		    /* Le contenu de *rhs_ptr est non utilise' */
		    rhs_ptr += rhs_arg_nb;
		}
		else {
		    bot = XH_X (paths, prdct_id);
		    top = XH_X (paths, prdct_id+1);

		    eq_id = XH_list_elem (paths, top-1);

		    if (eq_id & NEG) {
			/* Il y a une specif de negation sur les args */
			/* On stocke ds neg_calls pour traitement final */
			eq_id ^= NEG;
			top2 = XH_X (paths, eq_id+1);

			for (bot2 = XH_X (paths, eq_id); bot2 < top2; bot2++) {
			    PUSH (neg_calls, XH_list_elem (paths, bot2));
			    PUSH (neg_calls, arg_pos);
			}

			top--;
			eq_id = XH_list_elem (paths, top-1);
		    }

		    if (eq_id & EQ) {
			/* Il y a une specif d'egalite sur les args */
			top--;
			eq_id ^= EQ;
			PUSH (eq_ids, eq_id);
		    }

		    while (bot < top) {
			rhs_ptr->son = son;
			arg_id = XH_list_elem (paths, bot);
			rhs_ptr->bot = XH_X (paths, arg_id);
			rhs_ptr++;
			bot++;
		    }
		}

		echec = merge_sons (son+1, arg_pos+rhs_arg_nb);

		TOP (eq_ids) = top_eq_ids;
		TOP (neg_calls) = top_neg_calls;

		if (0 < echec && echec < son)
		    return echec;
	    }

	    return 0;
	}

	/* Ces predefinis peuvent contenir des ANY ds leurs args, mais ils seront ignores */
	if (nt == CUT_ic) {
	    /* rhs_arg_nb est variable, il doit etre calcule autrement */
	    rhs_arg_nb = G->prdct2args_disp [rhs_prdcts [son]+1]-bot-2;
	}

	rhs_ptr = rhs + arg_pos;

	for (i = 0; i < rhs_arg_nb; i++) {
	    rhs_ptr->son = son;
	    rhs_ptr->bot = 0;
	    rhs_ptr++;
	}

	echec = merge_sons (son+1, arg_pos+rhs_arg_nb);

	return (0 < echec && echec < son) ? echec : 0;
    }

    RAZ (stack);

    if (son_nb)
	Xoccur = G->Xoccur+G->Xoccur_disp [clause];

    IUB = iub;
    INDEXES = indexes;
    lhs_arg = 0;

    while (lhs_arg < lhs_arg_nb) {
	lb = ilb [lhs_arg];
	lb_any_nb = si2any_nb [lb];
	symbs = G->args2symbs + G->args2symbs_disp [lhs_args [lhs_arg]];
	symbs_lim = G->args2symbs + G->args2symbs_disp [lhs_args [lhs_arg]+1];

	while (symbs < symbs_lim) {
	    symb = *symbs++;
	    /* On cherche l'ub */
	    ub = (symbs == symbs_lim) ? *IUB++ : *INDEXES++;
	    ub_any_nb = si2any_nb [ub];
	    
	    if (symb < 0) {
		/* C'est un terminal */
		if (lb_any_nb < ub_any_nb) {
		    /* qui correspond au source ANY */
		    /* Il a peut etre deja ete genere par les fils, on le verifie */
		    symb = -symb;

#if PID>1
		    if (local_pid != 0)
			symb = l2g [local_pid] [symb];
#endif

		    PUSH (stack, symb);
		    si2xs [ub] = TOP (stack);

		    if (son_nb && (pos = *Xoccur++) != 0) {
			/* Au moins une occur en RHS */
			if ((echec = Tinstance (pos, ub_any_nb)) > 0) return echec;
		    }
		}
		else {
		    if (son_nb)
			while (*Xoccur++ != 0);
		}
	    }
	    else {
		/* C'est une variable X */
		if (son_nb == 0 || (pos = *Xoccur++) == 0) {
		    /* X est top-down erasing, on stocke les ANY entre lb et ub */
		    /* A FAIRE plusieurs occurrences du meme X en LHS => meme chaine */
		    while (++lb_any_nb <= ub_any_nb) {
			PUSH (stack, ANY);
			lb = any_nb2si [lb_any_nb];
			si2xs [lb] = TOP (stack);
		    }
		}
		else {
		    if (lb_any_nb < ub_any_nb) {
			/* Y-a du ANY */
			if ((echec = Xinstance (pos, lb_any_nb, ub_any_nb)) > 0) return echec;
		    }
		    else {
			/* On la saute */
			while (*Xoccur++ != 0);
		    }
		}
	    }

	    lb = ub;
	    lb_any_nb = ub_any_nb;
	}

	if (++lhs_arg < lhs_arg_nb)
	    PUSH (stack, ARG_SEP);
    }

    set_path (ret_vals);

    return 0;
}

SXINT
generator (struct G *pG, SXINT CLAUSE, SXINT *RET_VALS, SXINT *INDEXES, SXINT *ILB, SXINT *IUB, SXINT *OLB, SXINT *OUB)
    /* ret_vals est un tableau de taille sons_nb+1.
       ret_vals[i], i>0 est l'identifiant de l'ensemble des chaines associees au ieme fils
       ret_vals[0] est pour la LHS, si ret_vals[0]==0 c'est la premiere fois qu'on fait une reduc
       vers (A, rho[0]), clause est une A_clause */
    /* Composition de clause_args :
       clause_args_size, lhs_arg_nb, son1_xolb, son1_arg_nb, ..., sonson_nb_xolb, sonson_nb_arg_nb,
       streq1_xolb, ..., streqh_xolb */
    /* Composition de XOCCUR:
       top, arg, ARG_SEP, arg, ...,ARG_SEP, arg
       ou arg est la sequence symb 0 symb 0 ... symb 0
       ou symb est la sequence code , {prdct, arg} ou code > 0 est un terminal et code <0 est une variable
       chaque couple prdct,arg indique le prdct et un argument en RHS ou code apparait.
       Si le nombre de couples prdct,arg est >1 => la rcg est non-lineaire */
    /* Si la LHS a la forme (aXbY, cZ), INDEXES[0] est l'etat entre a et X, INDEXES[1] est l'etat entre X et b,
       INDEXES[2] est l'etat entre b et Y et  INDEXES[3] est l'etat entre c et Z.  Les limites des ranges sont
       ds ILB, IUB. */
{
    SXINT 	lb, ub, arg, bot, top, lhs, rhs;
    BOOLEAN	has_any;
	
    if (ANY_nb == 0) return 1;

    G = pG;
    local_pid = pG->pid;

    lhs = G->clause2lhs [CLAUSE];
    lhs_args = G->prdct2args + G->prdct2args_disp [lhs] + 1;
    lhs_arg_nb = G->nt2arity [*lhs_args++];

    ilb = ILB;
    iub = IUB;
    has_any = FALSE;
    sxba_empty (ws_set);

    for (arg = 1; arg <= lhs_arg_nb; arg++) {
	lb = *ILB++;
	ub = *IUB++;

	if ((bot = si2any_nb [lb]) < (top = si2any_nb [ub])) {
	    /* Un ANY ds ce range */
	    has_any = TRUE;

	    while (++bot <= top) {
		/* C,a permet la verification qu'on ne traite pas les grammaires dont les arg en LHS
		   se recouvrent */
		if (!SXBA_bit_is_reset_set (ws_set, bot))
		    sxtrap (ME, "generator: LHS non-linear case not yet implemented");
	    }
	}
    }

    if (!has_any) return 1;

    rhs = G->clause2rhs [clause = CLAUSE];

    if (rhs) {
	rhs_prdcts = G->rhs2prdcts + (bot = G->rhs2prdcts_disp [rhs]) - 1;
	son_nb = G->rhs2prdcts_disp [rhs+1]-bot;
    }
    else
	son_nb = 0;

    ret_vals = RET_VALS;
    indexes = INDEXES;
    olb = OLB;
    /* oub = OUB; unused */

    RAZ (neg_calls);
    RAZ (eq_ids);

    merge_sons (1, 1);

    return ret_vals [0];
}


SXINT
_GStrEq (SXINT *lb, SXINT *ub)
{
  /* Predicat predefini qui retourne l'ensemble des chaines egales entre les index source
     lb[0]+1..ub[0] et lb[1]+1..ub[1] de glbl_source.
  */
  SXINT		result, x, x1, x2, orig0, orig1, symb, arg1, arg2, list_id, top, eq_id, temp;
  SXINT		bot1, top1, bot2, top2;
  BOOLEAN	arg1_has_any, arg2_has_any;

  if (ANY_nb == 0) return 1;

  arg1_has_any = (bot1 = si2any_nb [lb[0]]) < (top1 = si2any_nb [ub[0]]);
  arg2_has_any = (bot2 = si2any_nb [lb[1]]) < (top2 = si2any_nb [ub[1]]);

  if (!arg1_has_any && !arg2_has_any) return 1; /* pas de ANY */

  RAZ (stack);

  if (arg1_has_any) {
      orig0 = lb [1]-lb [0];

      while (++bot1 <= top1) {
	  x1 = any_nb2si [bot1];
	  x = orig0+x1;
	  symb = glbl_source [x];

	  if (symb == ANY) {
	      /* Les 2 symboles freres sont des ANY */
	      /* On note les sources index */
	      if (x1 > x) {
		  temp = x1;
		  x1 = x;
		  x = temp;
	      }

	      PUSH (stack, x1);
	      PUSH (stack, x);
	  }

	  XH_push (paths, symb);
      }

      XH_set (&paths, &arg1);
  }
  else
      arg1 = 1;

  if (arg2_has_any) {
      orig1 = lb [0]-lb [1];

      while (++bot2 <= top2) {
	  x2 = any_nb2si [bot2];
	  x1 = orig1+x2;
	  symb = glbl_source [x1];
	  XH_push (paths, symb);
      }

      XH_set (&paths, &arg2);
  }
  else
      arg2 = 1;

  if ((top = TOP (stack)) > 0) {
      for (x = 1; x < top; x += 2) {
	  XH_push (paths, stack [x]);
	  XH_push (paths, stack [x+1]);
      }

      XH_set (&paths, &eq_id);
  }

  XH_push (paths, arg1);
  XH_push (paths, arg2);

  if (top > 0)
      XH_push (paths, eq_id|EQ);

  XH_set (&paths, &list_id);

  result = 0;
  insert_source (&result, list_id);

  return result;
}
#endif /* is_generator */

#if !is_sdag
/* Si &StrEq () est utilise ds le cas -DSDAG, il y a une erreur a l'edition de liens.
   On ne sait pas faire StrEq ds le cas is_sdag */
SXINT
_StrEq (SXINT *lb, SXINT *ub)
{
  /* Predicat predefini qui verifie l'egalite des chaines source comprises entre
     lb[0]+1..ub[0] et lb[1]+1..ub[1]
     On sait deja que les tailles sont identiques
  */
  SXINT	*src1 = glbl_source + lb [0], *src2 = glbl_source + lb [1], *lim1 = glbl_source + ub [0];

  while (++src2, ++src1 <= lim1) {
    if (
#if is_generator
/* Ici, le token source ANY matches n'importe quel token */
      *src1 != ANY && *src2 != ANY &&
#endif
      *src1 != *src2)
      return -1;
  }

  return 1;
}
#endif /* !is_sdag */
#endif /* !AG */

/* C'est un predicat externe systeme */
/* Son execution est definie par le systeme mais les proprietes statiques de ses arguments
   sont inconnues dy systeme. */
/* Il est bon que ds la pratique son utilisation se fasse avec &StrEqLen (X, Y) */
/* C'est donc un candidat a devenir un (externe) predefini */
SXINT	StrReverse_arity = 2;

#if !is_sdag
SXINT
_StrReverse (SXINT *rho0, SXINT *lb, SXINT *ub)
{
/* Predicat externe qui verifie que les chaines du source comprises entre
   lb[0]+1..ub[0] et lb[1]+1..ub[1] sont inverses l'une de l'autre.
  */
  SXINT	*src1 = glbl_source + lb [0], *src2 = glbl_source + lb [1], *lim1 = glbl_source + ub [0], *lim2 = glbl_source + ub [1];

  if (lim1-src1 != lim2-src2) return FALSE;

  *rho0 = 0;
    
  while (++src1 <= lim1) {
    if (
#if is_generator
/* Ici, le token source ANY matches n'importe quel token */
      *src1 != ANY && *lim2 != ANY &&
#endif
      *src1 != *lim2)
      return -1;

    lim2--;
  }

  return 1;
}


/* C'est un predicat externe systeme */
/* Son execution est definie par le systeme mais les proprietes statiques de ses arguments
   sont inconnues du systeme. */
/* Il est bon que ds la pratique son utilisation se fasse avec &StrEqLen (X, Y) */
/* C'est donc un candidat a devenir un (externe) predefini */
SXINT	Permute_arity = 2;

static BOOLEAN
less_equal (SXINT i, SXINT j)
{
#if is_generator
  if (i == ANY) return TRUE;
  if (j == ANY) return FALSE;
#endif
  return i <= j;
}

BOOLEAN
_Permute (SXINT *rho0, SXINT *lb, SXINT *ub)
{
  /* Just for fun ! Je ne sais pas le faire avec des RCG */
  /* Predicat externe qui verifie que les chaines du source comprises entre
     lb[0]+1..ub[0] et lb[1]+1..ub[1] sont une permutation l'une de l'autre.
     Complexite : n log n
  */
  SXINT	*src1 = glbl_source + lb [0], *src2 = glbl_source + lb [1], *lim1 = glbl_source + ub [0], *lim2 = glbl_source + ub [1];
  SXINT	size, *psort0, *psort1, *sort0, *sort1;

  if ((size = lim1-src1) != lim2-src2) return -1;

  *rho0 = 0;
    
  if (size == 0) return 1;

  sort0 = (SXINT*) sxalloc (2*size, sizeof (SXINT));

  psort0 = sort0;

  while (++src1 <= lim1) {
    /* On peut dans tous les cas utiliser le source "global" */
    *psort0++ = *src1;
  }

  psort1 = sort1 = psort0;

  while (++src2 <= lim2) {
    /* On peut dans tous les cas utiliser le source "global" */
    *psort1++ = *src2;
  }

  sort_by_tree (sort0, 0, size-1, less_equal);
  sort_by_tree (sort1, 0, size-1, less_equal);

  while (psort0 > sort0) {
    if (*--psort0 != *--psort1) break;
  }

  sxfree (sort0);

  return
#if is_generator
    *psort0 == ANY || *psort1 == ANY ||
#endif
    psort0 == sort0 ? 1 : -1;
}
#endif



#if is_earley_guide
/* "partage" avec earley */
SXBA	source_set, source_top;
#else /* !is_earley_guide */
static SXBA	source_set, source_top;
#endif /* !is_earley_guide */

static BOOLEAN
is_source_subset (SXBA t_set)
{
    /* t_set est l'ensemble des terminaux des args de la LHS d'une clause */
    /* On regarde si cet ensemble est un sous-ensemble des tokens du source */
    /* Dans le cas generateur, le nb de any du source doit etre >= au nb de terminaux de t_set non reconnus */
    SXBA_ELT	elt;  
    SXBA	t_slice = t_set, source_slice = source_set;
#if is_generator
    SXINT nb = ANY_nb;
#endif

    while (++source_slice <= source_top) {
	elt = *++t_slice;

	if ((elt &= ~(*source_slice)) != 0) {
	    /* elt = t_set - src */
#if is_generator
	    do {
		static SXINT	NBBITS [256] = {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3, 3, 4, 2
		     , 3, 3, 4, 3, 4, 4, 5, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4,
		     4, 5, 4, 5, 5, 6, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5
		     , 4, 5, 5, 6, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5,
		     6, 6, 7, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5
		     , 6, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
		     2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 3, 4
		     , 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8};

		if ((elt & 0xFFFF) == 0) {
		    elt >>= 16;
		}

		nb -= NBBITS [elt & 0xFF];
	    } while ((elt >>= 8) != 0);

	    if (nb < 0)
#endif
	    return FALSE;  

	}
    }

    return TRUE;
}

#if PID>1
static BOOLEAN
is_local_source_subset (SXBA local_t_set, SXINT local_pid)
{
  SXINT local_t, t, *l2g_ptr;

  local_t = 0;
  l2g_ptr = l2g [local_pid];

  while ((local_t = sxba_scan (local_t_set, local_t)) > 0) {
    t = l2g_ptr [local_t];

    if (!SXBA_bit_is_set (source_set, t))
      return FALSE;
    }
  
  return TRUE;
}
#endif /* PID>1 */


#if is_lfsa
struct fsaG        *lfsaGs [PID];
static struct fsaG store_lfsaGs [PID];
static struct fsaG store_rfsaGs [PID]; /* Ca facilite la vie que ca ne soit pas conditionne par is_rfsa */

static SXBA        *clause02lbs [PID], *clause02ubs [PID], *A02lbs [PID], *A02ubs [PID], valid_nt_sets [PID];
static SXINT         *clause2clause0s [PID], *A2A0s [PID];

#if is_rfsa
struct fsaG        *rfsaGs [PID];
#endif


#if is_coupled
/* on est ds le cas ou la grammaire guidee est accessible, on va l'utiliser pour reduire valid_clause_set */
/* Une p-ary A-clause de la grammaire guidee n'est validee que si les p Ai-clauses de la 1rcg existent */
static void
filtering_by_1rcg (struct G *G, SXBA valid_clause_set)
{
  SXINT           pid, clause, eq_clause, main_clause, gclause, andk, bot, top, A, arity, x;
  SXINT           *kclauses_nb, *p, *new_clause2main;
  SXBA          new_valid_clause_set, valid_nt_set, nt_set, clause_set, t_set, unvalid_clause_set, reduced_clause_set;
  struct G	*gG, *pG;

  /* G est une 1rcg ... */
  pid = G->pid;

  unvalid_clause_set = sxba_calloc (BASIZE (valid_clause_set));

  /* Si non NULL, G->invalid_lex_loop_clause contient l'ensemble des boucles, cet ensemble peut etre reduit par filtering_by_1rcg */
  /* On met les boucles ... */
  if (G->invalid_lex_loop_clause) {
    clause = 0;

    while ((clause = sxba_scan (G->invalid_lex_loop_clause, clause)) > 0) {
      SXBA_1_bit (valid_clause_set, clause);
    }
  }

  /* Attention, si G est en forme 2var, elle ne contient pas de clauses identiques */

#if is_rav2 
  /* ... en forme 2var */
  /* La grammaire est ds Rav2_Gs [pid] */
  gG = Rav2_Gs [pid];
  new_valid_clause_set = sxba_calloc (gG->clause_nb+1);
  new_clause2main = (SXINT*) sxalloc (gG->clause_nb+1, sizeof (SXINT));
  reduced_clause_set = sxba_calloc (gG->clause_nb+1);

  clause = 0;

  while ((clause = sxba_scan (valid_clause_set, clause)) > 0) {
    if ((main_clause = gG->from2var2clause [clause]) > 0) {
      SXBA_1_bit (new_valid_clause_set, main_clause);
      new_clause2main [main_clause] = main_clause;
    }
  }
#else /* !is_rav2 */
  new_valid_clause_set = valid_clause_set;
  reduced_clause_set = sxba_calloc (BASIZE (valid_clause_set));
  gG = G;
  new_clause2main = (SXINT*) sxalloc (BASIZE (valid_clause_set), sizeof (SXINT));

  clause = 0;

  while ((clause = sxba_scan (valid_clause_set, clause)) > 0) {
    new_clause2main [clause] = clause;
  }
#endif /* !is_rav2 */

  /* ... et les clauses identiques */
  if (gG->clause2identical_disp != NULL) {
    clause = gG->clause_nb+1;

    /* On suppose que la clause representant un ensemble de clauses identiques est la + petite ... */
    while ((clause = sxba_1_rlscan (new_valid_clause_set, clause)) > 0) {
      main_clause = new_clause2main [clause];

      if (((bot = gG->clause2identical_disp [clause]) < (top = gG->clause2identical_disp [clause+1]))) {
	do {
	  eq_clause = gG->clause2identical [bot];
	  new_clause2main [eq_clause] = main_clause;
	  SXBA_1_bit (new_valid_clause_set, eq_clause); /* ... sans effet sur le sxba_1_rlscan */
	} while (++bot < top);
      }
    }
  }

  pG = Guided_Gs [pid]; /* grammaire guidee */

  clause_set = sxba_calloc (pG->clause_nb+1);
  kclauses_nb = (SXINT*) sxcalloc (pG->clause_nb+1, sizeof (SXINT));	

  andk = (1<<(sxlast_bit (pG->clause_nb)))-1;

  gclause = 0;

  while ((gclause = sxba_scan (new_valid_clause_set, gclause)) > 0) {
    /* valid_clause_set contient aussi les clauses "identiques" et les "boucles" */
    clause = (pG->guide_clause2clause [gclause]) & andk;
    arity = pG->nt2arity [pG->clause2lhs_nt [clause]];

    /* On valide une clause (et un prdct) ssi toutes ses sous-clauses sont ds le guide */
    if (++kclauses_nb [clause] == arity) {
      SXBA_1_bit (clause_set, clause);
    }
  }

  gclause = 0;

  while ((gclause = sxba_scan (new_valid_clause_set, gclause)) > 0) {
    clause = (pG->guide_clause2clause [gclause]) & andk;

    if (SXBA_bit_is_set (clause_set, clause)) {
      /* On doit conserver le main de gclause */
      main_clause = new_clause2main [gclause];
      SXBA_1_bit (reduced_clause_set, main_clause);
    }
  }

  sxfree (kclauses_nb);
  sxfree (clause_set);
  sxfree (new_clause2main);

  /* Les clauses valides sont ds reduced_clause_set (meme les boucles) */

#if is_rav2
  sxfree (new_valid_clause_set);

  clause = 0;

  while ((clause = sxba_scan (valid_clause_set, clause)) > 0) {
    if ((main_clause = gG->from2var2clause [clause]) > 0) {
      if (!SXBA_bit_is_set (reduced_clause_set, main_clause))
	SXBA_1_bit (unvalid_clause_set, clause);
    }
    else {
      /* clause est secondaire, on la valide ssi elle est une clause secondaire d'au moins
	 une clause principale valide */
      top = gG->from2var2clause_disp [clause+1];
      bot = gG->from2var2clause_disp [clause];

      do {
	main_clause = gG->from2var2clause_list [bot];

	if (SXBA_bit_is_set (reduced_clause_set, main_clause))
	  break; /* au moins un de bon */
      } while (++bot < top);

      if (bot == top)
	/* tous mauvais */
	SXBA_1_bit (unvalid_clause_set, clause);
    }
  }
#else /* !is_rav2 */
  sxba_copy (unvalid_clause_set, valid_clause_set);
  sxba_minus (unvalid_clause_set, reduced_clause_set);
  sxba_copy (valid_clause_set, reduced_clause_set);
#endif /* !is_rav2 */

  sxfree (reduced_clause_set);

  if (G->invalid_lex_loop_clause) sxba_minus (valid_clause_set, G->invalid_lex_loop_clause);

  if (sxba_scan (unvalid_clause_set, 0) > 0) {
    /* Ici unvalid_clause_set contient l'ensemble des clauses (dont les loop) qui finalement se sont revelees invalides */
    if (G->invalid_lex_loop_clause) {
      SXBA unvalid_loop_set;

      unvalid_loop_set = sxba_calloc (G->clause_nb+1);

      sxba_copy (unvalid_loop_set, unvalid_clause_set);
      sxba_and (unvalid_loop_set, G->invalid_lex_loop_clause);
      sxba_minus (G->invalid_lex_loop_clause, unvalid_clause_set);
      sxba_minus (unvalid_clause_set, unvalid_loop_set);

      sxfree (unvalid_loop_set);
    }

    sxba_minus (valid_clause_set, unvalid_clause_set);
    /* valid_clause_set contient ne contient pas les boucles */
    /* unvalid_clause_set ne contient pas les boucles */
    /* G->invalid_lex_loop_clause contient les "bonnes" boucles */
    
    /* On supprime les lub associes aux unvalid_clause_set */ 
    /* ... et on recalcule ceux des nt */
    nt_set = NULL;

    if (G->clause2ubs) {
      if (G->nt2ubs)
	nt_set = sxba_calloc (G->ntmax+1);

      clause = 0;

      while ((clause = sxba_scan (unvalid_clause_set, clause)) > 0) {
	/* clause n'est pas une boucle */
	sxba_empty (G->clause2ubs [clause]);
	if (G->clause2lbs) sxba_empty (G->clause2lbs [clause]);

	if (G->nt2ubs) {
	  A = G->clause2lhs_nt [clause];
	
	  if (SXBA_bit_is_reset_set (nt_set, A)) {
	    sxba_empty (G->nt2ubs [A]);
	    if (G->nt2lbs) sxba_empty (G->nt2lbs [A]);
	  }
	}
      }
    }

    if (nt_set) {
      /* nt_set contient l'ensemble des nt qui ont change */
      clause = 0;

      while ((clause = sxba_scan (valid_clause_set, clause)) > 0) {
	/* clause n'est pas une boucle */
	A = G->clause2lhs_nt [clause];

	if (SXBA_bit_is_set (nt_set, A)) {
	  sxba_or (G->nt2ubs [A], G->clause2ubs [clause]);
	  if (G->nt2lbs) sxba_or (G->nt2lbs [A], G->clause2lbs [clause]);
	}
      }

      sxfree (nt_set);

      /* On peut calculer l'ensemble des nt qui ont disparu */
      /* Il se peut que ces nt produisent des boucles qui doivent donc disparaitre */
      /* De plus ces boucles qui disparaissent peuvent avoir servi a valider une clause
	 qui doit elle aussi disparaitre => point fixe A FAIRE */
    }
  }

  sxfree (unvalid_clause_set);
}
#endif /* is_coupled */
#endif /* is_lfsa */

#if is_lex && !is_earley_guide
/* Cette procedure positionne des structures pour le traitement de la "lexicalisation" */
/* 3 cas :
      - le cas simple is_lex et !is_lex2
      - le cas complet !is_lex et is_lex2
      - la derniere nouveaute is_lex3 => is_lex2
      */

/* CAS SIMPLE
   remplit is_Lex */

/* CAS COMPLET
   Termine le remplissage de lex_code.
   C'est un vecteur d'entiers, decoupe en tranche par nt2lex_code
   Chaque tranche concerne les A-clauses et se termine par 0
   les elements successifs de chaque tranche sont des entiers qui reperent les clauses d'un ensemble de A-clauses
   Par exemple on a 2 5 6 0 qui signifie que les A-clauses numero 2, 5 et 6 sont des candidates possibles a essayer
   (il n'est pas utile d'essayer les A-clauses numero 1, 3, 4 et 7, s'il y a 7 A-clauses).
   C,a permet d'organiser les appels de la fac,on suivante pour un predicat donne

   foreach numero in A-clause
      switch (numero)
      ...
      case i:
      Traitement de la ieme A_clause
      ...
      break;
      ...
      end switch
   end foreach
*/
void
lexicalization (struct G *G)
{
  SXBA t_set, fsa_lex, set, wset;
  SXINT  tok, clause, clause_id, local_tok;
  SXINT  nt, x;

#if is_lex3
  SXINT		x, lclause, *p;
  SXINT           *nt2nb = (SXINT*) sxcalloc (G->ntmax+1, sizeof (SXINT));
  SXBA 	        lhs_nt_set, fsa_lex;
  BOOLEAN	done;
#endif

#if is_lfsa
#if EBUG4
  wset = sxba_calloc (G->clause_nb+1);

  sxba_copy (wset, (G->t2clause_set) [0]);

  tok = 0;

  while ((tok = sxba_scan (source_set, tok)) >= 0) {
    sxba_or (wset, (G->t2clause_set) [tok]);
  }
#endif /* EBUG4 */
  /* Ici il y a deja eu une passe (l|r)fsa, on utilise le resultat pour remplir "is_Lex" */
  if (G->invalid_lex_loop_clause) {
    sxba_copy (G->invalid_lex_loop_clause, G->loop_clause_set); /* contient toutes les boucles de la grammaire */

    if (fsa_lex = store_lfsaGs [G->pid].Lex)
      /* Ici, fsa_lex a ete calcule avec le vieux source et contient les loop */
      /* store_rfsaGs [G->pid].Lex, s'il existe ne contient pas les boucles */
      sxba_and (G->invalid_lex_loop_clause, fsa_lex);
  }

#if is_rfsa
  set = store_rfsaGs [G->pid].fsa_valid_clause_set;
#else /* !is_rfsa */
  set = store_lfsaGs [G->pid].fsa_valid_clause_set;
#endif /* !is_rfsa */

#if EBUG4
  if (set == NULL)
    sxtrap (ME, "lexicalization");

  if (!sxba_is_subset (set, wset))
    sxtrap (ME, "lexicalization");
#endif

  /* ici, si set est non null, il contient les clauses valides et pas de boucles */
  if (G->invalid_lex_loop_clause) {
    /* G->invalid_lex_loop_clause est utilise par semact_gen_loop pour verifier que le lex des loops est bons */
    /* Il sera aussi utilise et refiltre ds filtering_by_1rcg */
    /* On refiltre par le nouveau source */
    clause = -1;

    while ((clause = sxba_scan (G->invalid_lex_loop_clause, clause)) >= 0) {
      if ((t_set = G->Lex [clause]) != NULL && !is_source_subset (t_set))
	SXBA_0_bit (G->invalid_lex_loop_clause, clause);
    }
  }

#if is_coupled
  /* on est ds le cas ou la grammaire guidee est accessible, on va l'utiliser pour reduire set */
  /* Une p-ary A-clause de la grammaire guidee n'est validee que si les p Ai-clauses de la 1rcg existent */
  filtering_by_1rcg (G, set);
  /* A FAIRE On pourrait aussi appeler filtering_by_1rcg ds le cas normal (pas de guide lrfsa) */
#endif /* is_coupled */

#if EBUG4
  if (!sxba_is_subset (set, wset))
    sxtrap (ME, "lexicalization");

  sxfree (wset);
#endif

#if EBUG2
  total_clause_nb += G->clause_nb;

  if (G->invalid_lex_loop_clause) total_clause1_nb += sxba_cardinal (G->invalid_lex_loop_clause);
#endif

  /* Le 10/6/2002 passage au complementaire */
  sxba_not (G->invalid_lex_loop_clause);
  sxba_minus (G->invalid_lex_loop_clause, G->loop_clause_set); /* contient toutes les boucles de la grammaire */

  clause = -1;

  sxba_and (set, G->main_clause_set);

  while ((clause = sxba_scan (set, clause)) >= 0) {
    /* La reconnaissance de "clause" est generee */
#if EBUG2
    total_clause1_nb++;
#endif
    nt = G->clause2lhs_nt [clause];
#if 0
    G->lex_code [G->nt2lex_code [nt] + nt2nb [nt]++] = G->clause2clause_id [clause];
#endif
    clause_id = G->clause2clause_id [clause];
    SXBA_1_bit (G->nt2clause_id_set [nt], clause_id);
  }
#else /* !is_lfsa */
  set = sxba_calloc (G->clause_nb+1);

#if is_lex3
  lhs_nt_set = sxba_calloc (G->ntmax+1), rhs_nt_set;
#endif

  sxba_copy (set, (G->t2clause_set) [0]);

  tok = 0;

  while ((tok = sxba_scan (source_set, tok)) >= 0) {
#if PID==1
    sxba_or (set, (G->t2clause_set) [tok]);
#else /* PID!=1 */
    if (G->pid == 0) {
      /* main */
      sxba_or (set, (G->t2clause_set) [tok]);
    }
    else {
      if (local_tok = g2l [G->pid] [tok])
	sxba_or (set, (G->t2clause_set) [local_tok]);
    }
#endif /* PID!=1 */
  }

#if EBUG2
  total_clause_nb += G->clause_nb;
#endif

  /* Le 10/6/2002 */
  if (G->invalid_lex_loop_clause)
    /* A priori toutes les loop clauses sont invalides */
    sxba_copy (G->invalid_lex_loop_clause, G->loop_clause_set);

  clause = -1;

  while ((clause = sxba_scan (set, clause)) >= 0) {
    /* Pour des stats */
    /* Nb de clauses selectionnees par la "lexicalisation" :
       nb de clauses ds lesquelles apparait au moins un terminal du source
       chaque clause est comptee au plus une fois */
    if ((t_set = G->Lex [clause]) == NULL ||
#if PID==1 
	is_source_subset (t_set)
#else /* PID!=1 */
	((G->pid == 0) ? is_source_subset (t_set) : is_local_source_subset (t_set, G->pid))
#endif /* PID!=1 */
	) {
      /* Ici, t_set == NULL <==> |t_set| <= 1 */
      /* sauf si on a atteint ce point ds le cas is_lfsa, auquel cas on a |t_set| == 0 */
#if EBUG2
      total_clause1_nb++;
#endif

      if (G->invalid_lex_loop_clause)
	/* clause est valide, on l'enleve des invalides */
	SXBA_0_bit (G->invalid_lex_loop_clause, clause);

      /* if (G->loop_lex) SXBA_1_bit (G->loop_lex, clause); */
      /* G->is_Lex [clause] = TRUE; */

      if (SXBA_bit_is_set (G->main_clause_set, clause)) {
	/* La reconnaissance de "clause" est generee */
	nt = G->clause2lhs_nt [clause];
#if is_lex3
	SXBA_1_bit (lhs_nt_set, nt);
	nt2nb [nt]++;
#else  /* !is_lex3 */
#if 0
	G->lex_code [G->nt2lex_code [nt] + nt2nb [nt]++] = G->clause2clause_id [clause];
#endif /* 0 */
	clause_id = G->clause2clause_id [clause];
	SXBA_1_bit (G->nt2clause_id_set [nt], clause_id);
#endif /* !is_lex3 */
      }
#if is_lex3
      else {
	/* Inaccessible par lex_code */
	SXBA_0_bit (set, clause);
      }
#endif /* is_lex3 */
    }
#if is_lex3
    else
      SXBA_0_bit (set, clause);
#endif /* is_lex3 */
  }

#if is_lex3
  done = FALSE;

  while (!done) {
    done = TRUE;
    clause = -1;

    while ((clause = sxba_scan (set, clause)) >= 0) {
      if ((rhs_nt_set = G->clause2rhs_nt_set [clause]) != NULL &&
	  !sxba_is_subset (rhs_nt_set, lhs_nt_set)) {
	/* Il y a en partie droite de clause des predicats A qui retourneront toujours faux
	   Les A-clauses sont non selectionnables */
	SXBA_0_bit (set, clause);
	
	if (G->invalid_lex_loop_clause)
	  /* clause est invalide, on la remet ds les invalides */
	  SXBA_1_bit (G->invalid_lex_loop_clause, clause);

	/* if (G->loop_lex) SXBA_0_bit (G->loop_lex, clause); */
	/* G->is_Lex [clause] = FALSE; */
	nt = G->clause2lhs_nt [clause];

	if (--nt2nb [nt] == 0) {
	  SXBA_0_bit (lhs_nt_set, nt);
	  done = FALSE;

#if 0
	  /* Il est inutile d'y remettre aussi ses identiques car seules les main loop de invalid_lex_loop_clause
	     seront testees */
	  if (G->looped_nt != NULL && (x = G->looped_nt [nt])) {
	    /* Il faut enlever les is_lex des loops */
	    p = G->looped_clause+x;

	    while (lclause = *p++) {
	      /* clause est invalide, on la remet ds les invalides */
	      SXBA_1_bit (G->invalid_lex_loop_clause, lclause);
	      /* SXBA_0_bit (G->loop_lex, lclause); */
	      /* G->is_Lex [lclause] = FALSE; */
	    }
	  }
#endif /* 0 */
	}
      }
    }
  }

  clause = 0;

  while ((clause = sxba_scan (set, clause)) >= 0) {
    nt = G->clause2lhs_nt [clause];

    if (SXBA_bit_is_set_reset (lhs_nt_set, nt))
      nt2nb [nt] = 0;

#if 0
    G->lex_code [G->nt2lex_code [nt] + nt2nb [nt]++] = G->clause2clause_id [clause];
#endif /* 0 */
    clause_id = G->clause2clause_id [clause];
    SXBA_1_bit (G->nt2clause_id_set [nt], clause_id);
  }

#if EBUG2
  total_clause2_nb += sxba_cardinal (set);
#endif
#else
#if EBUG2
  total_clause2_nb += total_clause1_nb;
#endif
#endif /* is_lex3 */

  sxfree (set);
#if is_lex3
  sxfree (lhs_nt_set);
  sxfree (nt2nb);
#endif
#endif /* !is_lfsa */
}
#endif /* is_lex && !is_earley_guide */


BOOLEAN
dynam_lex (SXINT lb, SXINT ub, SXINT nb, SXINT *t_list)
{
    /* nb > 0.  Verifie dynamiquement que la liste des terminaux t_list [0], ..., t_list [nb-1] se trouve
       entre les bornes lb..ub dans cet ordre */
    SXINT		t;

#if EBUG2
    dynam_lex_call_nb++;
#endif

    if (nb <= ub-lb) {
	t = *t_list++;

	while (nb <= ub-lb++) {
	    if (
#if is_sdag
		SXBA_bit_is_set (glbl_source [lb], t)
#else
		glbl_source [lb] == t
#endif
		) {
		if (--nb == 0) return TRUE;
	  
		t = *t_list++;
	    }
	}
    }

#if EBUG2
    dynam_lex_false_call_nb++;
#endif

    return FALSE;
}




#if is_sdag
BOOLEAN
is_AND (SXBA lhs_bits_array, SXBA rhs_bits_array)
/*
 * "is_AND" returns TRUE iff the result of the bitwise
 * "AND" of its two arguments is not nul.
 */
{
    SXINT	slices_number = NBLONGS (BASIZE (lhs_bits_array));
    SXBA	lhs_bits_ptr = lhs_bits_array + slices_number, rhs_bits_ptr = rhs_bits_array + slices_number;

    while (slices_number-- > 0) {
	if ((*lhs_bits_ptr-- & *rhs_bits_ptr--) != 0)
	    return TRUE;
    }

    return FALSE;
}
#endif /* is_sdag */



#if is_semantics
void _walk_StrReverse ()
{
}
#endif

#if is_rcvr
#include "dcg_cst.h"
#include "drcg_glbl.h"

static void
store_list (SXINT S0n, SXINT *next_root, SXINT *list)
{
  /* Sans doute provisoire */
  /* Utilisee par le traitement d'erreur */
  /* Suppose que l'axiome a un seul param qui est une liste. Cette liste est transferee ds list */
  SXINT 			root, prod, t, kind, t_kind, t_atom, root_atom;
  struct drcg_struct		*pdrcg;
  struct drcg_tree2attr	*ptree;

  if ((root = *next_root) == 0)
    root = Aij2tree [S0n];

  ptree = drcg_tree2attr+root;
  *next_root = ptree->next;
  root_set = ptree->tree_set;
  prod = ptree->prod;
  pdrcg = drcg+ptree->pid;

#if EBUG4
  if (pdrcg->param_nb [prod][0] != 1)
    sxtrap (ME, "store_list");
#endif

  t = pdrcg->term [prod] [0] [0];

  for (;;) {
    t_kind = REF2KIND (t);

    if (t_kind == VARIABLE) {
      /* C'est une variable */
      DRCGsubstitute (&root, &t);
      t_kind = REF2KIND (t);
    }

#if EBUG4
    if (t_kind != DYNAM_LIST && t_kind != STATIC_LIST)
      sxtrap (ME, "store_list");
#endif

    t = REF2VAL (t);

    if (t == 0) break;
    
    if (t_kind == STATIC_LIST) {
      pdrcg = drcg+drcg_tree2attr [root].pid;
      t_atom = pdrcg->list_list [t];
    }
    else
      t_atom = XxY_X (dynam_list, t);

    root_atom = root;
    kind = REF2KIND (t_atom);

    if (kind == VARIABLE) {
      /* C'est une variable */
      if (t_atom != DUM_VAR) {
	DRCGsubstitute (&root_atom, &t_atom);
	kind = REF2KIND (t_atom);
      }
    }

#if EBUG4
    if (kind != ATOM)
      sxtrap (ME, "store_list");
#endif

    *list++ = REF2VAL (t_atom);
    /* Attention, t_atom est le code interne de l'atome et non le code du token! */
    /* On suppose que les 2 sont identiques et que la 1ere clause est de la forme
       S(\a\b\c\d\e\f)([a,b,c,d,e,f])
       --> &False() . % sert uniquement a definir l'ensemble T et les atomes avec des codes
       % internes identiques
    */

    if (t_kind == STATIC_LIST)
      t = pdrcg->list_list [t+1];
    else
      t = XxY_Y (dynam_list, t);

  }

  *list = sxsvar.SXS_tables.S_termax;
}

#endif

static char *messages []=
{"",
 "%s\"%s\" is replaced by \"%s\".\n",
 "%s\"%s\" is inserted before \"%s\".\n",
 "%s\"%s\" is deleted.\n",
 "%s\"%s\" and \"%s\" are exchanged.\n",
};
			     
/* On ne met pas "rcg_rcvr_mess" ds le scope de "is_rcvr", ca permet de tester "distance" en stand alone et par
   exemple d'avoir un message de swap sur le source "a ab ba" :

   a ab ba
     ^
     , line 1: column 3: Error:      "a" and "b" are exchanged.
   */

/* sxplocals, ... sont connus ds RCG_parser */
void
call_put_error (mark, format, arg1, arg2, arg3, arg4, arg5)
     SXINT mark;
     char *format;
     char *arg1;
     char *arg2;
     char *arg3;
     char *arg4;
     char *arg5;
{
  sxput_error (SXGET_TOKEN (mark).source_index,
	       format,
	       sxplocals.sxtables->err_titles [2],
	       arg1, arg2, arg3, arg4, arg5);
}



#if is_sdag
void
rcg_rcvr_mess (SXINT kind, SXINT mark, SXINT index1, SXINT index2)
{
    /* A FAIRE */
}
#else
void
rcg_rcvr_mess (SXINT kind, SXINT mark, SXINT index1, SXINT index2)
{
  char		*str1, *str2;
  struct sxtoken	*ptok;

  if (index1 <= rcvr_n) {
    ptok = &(SXGET_TOKEN (index1));

    if (ptok->string_table_entry == EMPTY_STE)
      str1 = sxttext (sxplocals.sxtables, ptok->lahead);
    else
      str1 = sxstrget (ptok->string_table_entry);
  }
  else
    str1 = sxttext (sxplocals.sxtables, glbl_source [index1]);

  if (index2 <= rcvr_n) {
    ptok = &(SXGET_TOKEN (index2));

    if (ptok->string_table_entry == EMPTY_STE)
      str2 = sxttext (sxplocals.sxtables, ptok->lahead);
    else
      str2 = sxstrget (ptok->string_table_entry);
  }
  else
    str2 = sxttext (sxplocals.sxtables, glbl_source [index2]);

  call_put_error (mark,
		  messages [kind],
		  sxplocals.sxtables->err_titles [2],
		  str1,
		  str2);

}
#endif


static SXINT	S0n, zero;


#if is_rcvr
static BOOLEAN
sxrcg_error_recovery (SXINT top)
{
  /* Traitement d'erreur */
  /* Pour l'instant test grossier */
  /* On suppose l'existance du "generateur" associe a l'analyseur */
  /* Pour l'instant, c'est une drcg construite a la main a partir de la rcg */
  /* On genere toutes les chaines de longueur a peu pres n et on cherche celles qui
     sont a la distance minimale de la chaine source (on utilise le predicat externe
     non-predefini &distance) */
  SXINT			*rcvr_source, *store_glbl_source, i, next_tree, dist, last, rcvr_rho0, pid;
  SXINT			insert, again, delta;
  BOOLEAN		ret_val = FALSE;

  static BOOLEAN	is_in_error_recovery;
  static SXINT		rcvr_ilb [3], rcvr_iub [3];
  static SXINT		w_global_pid;
  static bfun		w_first_pass, w_second_pass, w_last_pass;
  static vfun		w_first_pass_init_fun [PID], w_second_pass_init_fun [PID],
    w_first_pass_final_fun [PID], w_second_pass_final_fun [PID];
  static SEM_TYPE	*w_sem_disp [PID];
  static pfun		w_Aij2struct [PID];
  static vfun		distance_first_pass_init, distance_first_pass_final;
  static SEM_TYPE	*gsem_disp;
  static pfun		gAij2struct, distance_Aij2struct;

  if (is_in_error_recovery) return FALSE; /* Appel recursif */

  if (is_print_time)
    fputs ("\t\tError recovery (start)\n", sxtty);

  is_in_error_recovery = TRUE;
    

  w_global_pid = global_pid;

  w_first_pass = first_pass;
  w_second_pass = second_pass;
  w_last_pass = last_pass;

  for (i = 0; i < PID; i++) {
    w_first_pass_init_fun [i] = first_pass_init_fun [i];
    w_second_pass_init_fun [i] = second_pass_init_fun [i];
    w_first_pass_final_fun [i] = first_pass_final_fun [i];
    w_second_pass_final_fun [i] = second_pass_final_fun [i];
    w_sem_disp [i] = sem_disp [i];
    w_Aij2struct [i] = Gs [i]->Aij2struct;
  }

  n = 2 * (top+1) + MAX_DIST;
  logn = sxlast_bit (n);
  ANDj = (1<<logn)-1;

  rcvr_source = (SXINT *) sxalloc (n+1, sizeof (SXINT));

  for (i = rcvr_n; i >= 0; i--)
    rcvr_source [i] = glbl_source [i];

  store_glbl_source = glbl_source;
  glbl_source = rcvr_source;

  /* On initialise "distance" */
  /* C'est un parser 2 passes */
  global_pid = 0;
  _distance_if ();
  distance_first_pass_init = first_pass_init_fun [0];
  distance_first_pass_final = first_pass_final_fun [0];
  distance_Aij2struct = Gs [0]->Aij2struct;
  
  /* On initialise la phase "generation" du langage courant */
  global_pid = 0;
    
  GINTERFACE;

#if PID>1
  for (pid = 0; pid < PID; pid++) {
      Gs [pid]->Gs = &(Gs [0]);
      (*first_pass_init_fun [pid]) ();
  }

  check_combinatorial_options (Gs [0]);
#else
  (*first_pass_init_fun [0]) ();
  Gs [0]->Gs = &(Gs [0]);
#endif

  gAij2struct = Gs [0]->Aij2struct;

  /* L */
  rcvr_ilb [0] = 0;
  /* X */
  rcvr_ilb [1] = 0;
  rcvr_iub [1] = top;
  /* Y */
  rcvr_ilb [2] = rcvr_n;
  next_tree = 0;

  /* Recherche d'une correction a la distance minimale */
  for (dist = 1; dist <= MAX_DIST; dist++) {
    /* La longueur du nouveau source est la + proche possible de l'ancienne */
    for (delta = 0; delta <= dist; delta++) {
      /* On favorise en premier l'insertion ou la suppression */
      insert = again = INSERTION_WGHT >= DELETION_WGHT ? 1 : -1;

      do {
	last = top + insert*delta;

	if ((*first_pass) (&rcvr_rho0, &zero, &last)) {
	  /* Il y a au moins une chaine de taille last qui marche */
	  gsem_disp = sem_disp [0]; /* a pu etre resized */

	  /* Aij2struct [0] = distance_Aij2struct; */

	  do {
	    /* On fabrique un source bidon */
	    store_list (rcvr_rho0, &next_tree, rcvr_source+rcvr_n+1);
	    /* On appelle &distance */
	    /* L */
	    rcvr_iub [0] = dist;
	    /* X */
	    /* Y */
	    rcvr_iub [2] = rcvr_n+last;

	    /* is_second_pass == FALSE, on ne sort pas les messages d'erreur */

	    (*distance_first_pass_init) ();
	    ret_val = _distance (&rcvr_rho0, rcvr_ilb, rcvr_iub); /* On ne rentre pas par l'axiome */

	    if (ret_val) {
	      is_second_pass = TRUE; /* On sort les messages d'erreur */
	      _walk_distance (rcvr_rho0);	/* On ne rentre pas par l'axiome */
	      is_second_pass = FALSE;
	    }

	    (*distance_first_pass_final) (); /* On termine distance */

	    if (ret_val) break;
	  } while (next_tree);

	  /* On remet le "generateur" */
	  sem_disp [0] = gsem_disp;
	  /* Aij2struct [0] = gAij2struct; */
	}

	if (ret_val) break;

	insert = -insert;
      } while (delta > 0 && insert != again);

      if (ret_val) break;
    }

    if (ret_val) break;
  }

  /* On termine la generation (1 passe) */
#if PID>1
  for (pid = 0; pid < PID; pid++)
    (*first_pass_final_fun [pid]) ();
#else
    (*first_pass_final_fun [0]) ();
#endif

   if (ret_val) {
    /* OK */
    /* On reappelle distance avec sortie des messages d'erreur */
    /* Reanalyse avec le nouveau source */
    /* On refabrique glbl_source */
    /* RAZ des memos!! */
    /* ret_val = (*first_pass) (&rho0, &zero, &last); */
  }
  else {
    /* Correction impossible */
    /* Message */
  }

  sxfree (rcvr_source), rcvr_source = NULL;
  glbl_source = store_glbl_source;

  global_pid = w_global_pid;
	
  first_pass = w_first_pass;
  second_pass = w_second_pass;
  last_pass = w_last_pass;

  for (i = 0; i < PID; i++) {
    first_pass_init_fun [i] = w_first_pass_init_fun [i];
    second_pass_init_fun [i] = w_second_pass_init_fun [i];
    first_pass_final_fun [i] = w_first_pass_final_fun [i];
    second_pass_final_fun [i] = w_second_pass_final_fun [i];
    sem_disp [i] = w_sem_disp [i];
    /* Aij2struct [i] = w_Aij2struct [i]; */
  }

  is_in_error_recovery = FALSE;

  n = top; /* provisoire */
  logn = sxlast_bit (n);
  ANDj = (1<<logn)-1;

  if (is_print_time)
    sxtime (TIME_FINAL, "\t\tError recovery (end)");

  return ret_val;
}
#endif

static SXINT axiom_0_n;

static char which_str [132];
static char which2_str [64];
static char icnb_str [32];

static char	*semact_messages [] = {
/* 0 */    "no sem",
/* 1 */    "#parse trees",
/* 2 */    "store parse forest",
/* 3 */    "instantiated clauses",
/* 4 */    "coupled guide construction",
/* 5 */    "full inverse mapping",
/* 6 */    "shallow mapping",
/* 7 */    "guide construction",
/* 8 */    "full guide construction",
/* 9 */    "fully coupled guide construction",
/* 10 */   "instantiated lhs clauses",
};
    

static SXINT
get_semact_no (SXINT pass_no, struct G *G)
{
    /* Associe a chaque semact.[first | second]_pass un numero (voir ci-dessus semact_messages) :
       0 	sxivoid
       1 	tree_nb_action
       2 	store_elem_tree_action
       3|7|8|10	parse_forest_action
       4|9	coupled_action
       5 	from2var_mngr
       6 	shallow_action
       */
    ifun	semact;

    if (pass_no == 1)
	semact = G->semact.first_pass;
    else
	semact = G->semact.second_pass;

    if (semact == sxivoid) return 0;
    if (semact == tree_nb_action) return 1;
    if (semact == store_elem_tree_action) return 2;
#if is_parse_forest
    if (semact == parse_forest_action) {
	if (is_guiding || is_full_guiding)
	    return is_guiding ? 7 : 8;

#if is_rav2 || is_rav22
	if (is_rav2_coupled_and_not_FL_rhs)
	  return 10;
#endif
	return 3;
    }
#endif
#if is_coupled
    if (semact == coupled_action)
#if is_fully_coupled
	return 9;
#else
	return 4;
#endif
#endif
#if is_rav2 || is_rav22
    if (semact == from2var_mngr) return 5;
#endif
#if is_shallow
    if (semact == shallow_action) return 6;
#endif
}

static void
fill_which_str (SXINT kind)
{
    SXINT no;

    switch (kind) {
    case 1:
	no = get_semact_no (1, Gs [0]);

#if is_rav2 || is_rav22
	if (no == 5) {
	    /* inverse mapping */
	  /* Si is_rav2_coupled_and_not_FL_rhs on ne fait pas le mapping inverse */
	    sprintf (which2_str,
		     "%s%s to %s (degree == %i) ",
		     is_rav2_coupled_and_not_FL_rhs ? "no " : "",
		     semact_messages [no],
		     Rav2_Gs [0]->name,
		     Rav2_Gs[0]->degree
		     );

	    no = get_semact_no (1, Rav2_Gs [0]);
	}
	else
#endif /* is_rav2 || is_rav22 */
	    which2_str [0] = NUL;

	if (no != 0 && no != 5)
	  sprintf (icnb_str, "[%i]", icnb);
	else
	  icnb_str [0] = NUL;

	sprintf (which_str, "\t%s (degree == %i) with %s\n\
\t\tFirst Pass (%s%s%s)",
#if is_coupled
		 "Guiding parser",
#else
#if is_guided
		 is_robust_run ? "Robust Parser" : "Guided parser", /* Le parser robuste n'est pas guided!! */
#else
#if is_parser
		 is_robust_run ? "Robust Parser" : "Parser",
#else
		 is_robust_run ? "Robust Recognizer" : "Recognizer",
#endif /* is_parser */
#endif /* is_guided */
#endif /* is coupled */
		 Gs[0]->degree,
		 Gs [0]->name,
		 which2_str,
		 semact_messages [no],
		 icnb_str
		 );
	break;

    case 2:
	no = get_semact_no (2, Gs [0]);

#if is_rav2 || is_rav22
	if (no == 5) {
	    /* inverse mapping */
	    sprintf (which2_str,
		     ", %s%s to %s (degree == %i)",
		     is_rav2_coupled_and_not_FL_rhs ? "no " : "",
		     semact_messages [no],
		     Rav2_Gs [0]->name,
		     Rav2_Gs[0]->degree
		     );

	    no = get_semact_no (1, Rav2_Gs [0]);
	}
	else
#endif /* is_rav2 || is_rav22 */
	    which2_str [0] = NUL;

	if (no != 0 && no != 5)
	  sprintf (icnb_str, "[%i]", icnb);
	else
	  icnb_str [0] = NUL;

	sprintf (which_str, "\t\tSecond pass (pruning%s and %s%s)",
		 which2_str,
		 semact_messages [no],
		 icnb_str
		 );
	break;

    case 3:
	no = get_semact_no (1, Gs [0]);

#if is_rav2 || is_rav22
	if (no == 5) {
	    /* inverse mapping */
	    sprintf (which2_str,
		     "%s%s to %s (degree == %i) ",
		     is_rav2_coupled_and_not_FL_rhs ? "no " : "",
		     semact_messages [no],
		     Rav2_Gs [0]->name,
		     Rav2_Gs[0]->degree
		     );

	    no = get_semact_no (1, Rav2_Gs [0]);
	}
	else
#endif /* is_rav2 || is_rav22 */
	    which2_str [0] = NUL;

	if (no != 0 && no != 5)
	  sprintf (icnb_str, "[%i]", icnb);
	else
	  icnb_str [0] = NUL;

	sprintf (which_str, "\t%s (degree == %i) with %s\n\
\t\tFirst pass (%s%s%s)",
		 is_robust_run ? "Robust Parser" : "Guided parser", /* Le parser robuste n'est pas guided!! */
		 Gs[0]->degree,
		 Gs [0]->name,
		 which2_str,
		 semact_messages [no],
		 icnb_str
		 );
	break;
    }

    icnb = 0;
}

static BOOLEAN
perform_robust_pass (SXINT kind)
{
  BOOLEAN ret_val = FALSE;

  switch (kind) {
  case 1:
#if is_coupled
    ret_val = FALSE;
#else
#if is_guided
    ret_val = TRUE;
#else
#if is_parser
    ret_val = TRUE;
#else
    ret_val = TRUE;
#endif /* is_parser */
#endif /* is_guided */
#endif /* is coupled */
    break;

  case 2:
    break;

  case 3:
#if is_fully_coupled
    ret_val = FALSE;
#else
    ret_val = TRUE;
#endif
    break;
  }

  return ret_val;
}


static BOOLEAN
rcgparse_it (SXINT top, SXINT kind)
{
    SXINT		ret_val;
    SXINT		pid;
    BOOLEAN	is_second_pass;
    char	str [164];

#if PID>1
    for (pid = 0; pid < PID; pid++) {
        (*(Gs [pid]->parse.first_pass_init)) ();
    }
#else
    (*(Gs [0]->parse.first_pass_init)) ();
#endif

    /* On appelle l'axiome avec axiom_0_n, arg supplementaire au cas ou` le parseur est is_guided2 */
    ret_val = (*(Gs [0]->parse.first_pass)) (&S0n, &zero, &top, &axiom_0_n);

    if (is_print_time) {
	fill_which_str (kind);
	sprintf (str, "%s (%s)", which_str, (ret_val != -1) ? "TRUE" : "FALSE");
	sxtime (TIME_FINAL, str);
    }

    if (ret_val == -1 && (Gs [0])->robust && perform_robust_pass (kind)) {
      SXBA *pmemoN;
      /* L'analyseur RCG a du etre compile avec l'option -DROBUST */
      /* Si on n'est pas ds une construction de guide, on relance la passe robuste */
      /* Sauf si on est ds le cas fully_(coupled|guided), qui ne peut pas actuellement
	 s'executer en parseur robuste */
      /* On reutilise les structures partiellement remplies par la 1ere passe qui vient d'echouer */
      /* On dit que les ip vrais sont deja calcules (et seulement ceux la`) */

      if (pmemoN = Gs [0]->pmemoN) {
	/* Y-a de la memoization */
	for (pid = 0; pid < PID; pid++) {
	  sxba_copy (pmemoN [0], pmemoN [1]); /* memoN0 = memoN1 */
	}
      }

      /* Laisse-t-on "Lex" tel quel pour la passe robuste ? */

      is_robust_run = TRUE;

      ret_val = (*(Gs [0]->parse.first_pass)) (&S0n, &zero, &top, &axiom_0_n);

      if (is_print_time) {
	fill_which_str (kind);
	sprintf (str, "%s (%s)", which_str, (ret_val != -1) ? "TRUE" : "FALSE");
	sxtime (TIME_FINAL, str);
      }
    }

#if is_rcvr
    if (ret_val == -1) {
	ret_val = sxrcg_error_recovery (top);
    }
#endif


#if is_semantics
    if (is_second_pass = (ret_val != -1 && Gs [0]->parse.second_pass != sxivoid)) {
#if PID>1
	for (pid = 0; pid < PID; pid++) {
	    (*(Gs [pid]->parse.second_pass_init)) ();
	}
#else
	(*(Gs [0]->parse.second_pass_init)) ();
#endif

	ret_val = (*(Gs [0]->parse.second_pass)) (S0n);

	if (is_print_time) {
	    fill_which_str (2);
	    sprintf (str, "%s (%s)", which_str, (ret_val != -1) ? "TRUE" : "FALSE");
	    sxtime (TIME_FINAL, str);
	}
    }

#if EBUG2 && is_generator
    /* On imprime l'ensemble des chaines reconnues par l'axiome */
#if AG
    dag_generator_post (ret_val);
#else
    source_generator_post (ret_val);
#endif
#endif

    /* On appelle la derniere passe, meme si les passes precedentes ont echouees */
    /* C'est la derniere passe qui decide quoi faire */
    ret_val = (*(Gs [0]->semact.last_pass)) (&(Gs [0]), S0n, ret_val);

#if is_semantics
    if (is_second_pass) {
#if PID>1
	for (pid = 0; pid < PID; pid++)
	    (*(Gs [pid]->parse.second_pass_final)) ();
#else
	(*(Gs [0]->parse.second_pass_final)) ();
#endif
    }
#endif

#if PID>1
    for (pid = 0; pid < PID; pid++) {
#if is_rav2
	Gs [pid]->prhoA_hd = NULL; /* C'est pas lui qui doit le liberer */
#endif /* is_rav2 */
	(*(Gs [pid]->parse.first_pass_final)) ();
    }
#else
#if is_rav2
    Gs [0]->prhoA_hd = NULL;	/* C'est pas lui qui doit le liberer */
#endif /* is_rav2 */
    (*(Gs [0]->parse.first_pass_final)) ();
#endif /* PID>1 */

    if (is_print_time) {
	if (ret_val != -1)
	    sxtime (TIME_FINAL, "\t\tLast pass (TRUE)");
	else
	    sxtime (TIME_FINAL, "\t\tLast pass (FALSE)");
    }
#endif

    return ret_val==-1 ? FALSE : TRUE;
}


#if is_sdag
/* Pour ressortir le sdag reduit */
static SXINT	*ff_ste;


static void
out_sdag (struct G **Gs, SXBA *source, SXBA **ff2clause_set, SXINT init_pos /* 0 si on veut sortir les clauses non lexicalisees, sinon 1 */)
{
  SXINT      pos, ste, t, pid, clause, nb, bot, top, eq_clause;
  SXBA     t_set, line, clause_set;
  char     *str;
  BOOLEAN  is_first, unknown_word;
  struct G *G;
  SXINT      *lex_pidXindex2clause_nb [PID];
  SXINT      nbs, old_nb, old_nbs, uw_nb;

  
  if (forest_level & FL_source) {
    for (pos = 1; pos <= n; pos++) {
      if ((ste = ff_ste [pos]) < 0)
	str = sxttext (sxplocals.sxtables, -ste);
      else
	str = sxstrget (ste);

      printf ("%s {", str);

      is_first = TRUE;
      t_set = source [pos];
      t = sxba_0_lrscan (t_set, 0);

      if (t > 0) {
	/* Ce n'est pas un mot inconnu */
	t = 0;
      
	while ((t = sxba_scan (t_set, t)) > 0) {
	  if (is_first)
	    is_first = FALSE;
	  else
	    fputs (" ", stdout);

	  printf ("%s", sxttext (sxplocals.sxtables, t));
	}
      }
	    
      fputs ("} ", stdout);       
    }

    fputs ("\n", stdout);
  }

  if (ff2clause_set [0]) {
#if MEASURES
    /* Pour chaque index du source de chaque pid on cumule ds old_nbs l'ensemble des clauses qui etaient
       initialement selectionnees par la lexicalisation */
    uw_nb = nbs = old_nbs = 0;

    for (pid = 0; pid < PID; pid++) {
      G = Gs [pid];

      clause_set = sxba_calloc (G->clause_nb+1);

      for (pos = init_pos; pos <= n; pos++) {
	if (pos == 0) {
	  sxba_copy (clause_set, G->t2clause_set [0]);
	}
	else {
	  line = glbl_source [pos];

	  t = 0;

	  while ((t = sxba_scan (line, t)) > 0) {
	    sxba_or (clause_set, G->t2clause_set [t]);
	  }
	}

	clause = 0;

	while ((clause = sxba_scan_reset (clause_set, clause)) > 0) {
	  old_nbs++;

	  if (G->clause2identical_disp != NULL) {
	    old_nbs += G->clause2identical_disp [clause+1] - G->clause2identical_disp [clause];
	  }
	}
      }

      sxfree (clause_set);
    }

    /* init_pos == 0 correspond a la sortie des clauses non lexicalisees */
    for (pos = init_pos; pos <= n; pos++) {
      /* Modif du 29/07/2002.  Les clauses equivalentes sont ajoutees au moment de la sortie */
      /* printf ("\n%i ", pos); */
      unknown_word = SXBA_bit_is_set (unknown_word_set, pos);

      for (pid = 0; pid < PID; pid++) {
	line = ff2clause_set [pid][pos];
      
	G = Gs [pid];

	if (G->clause2identical_disp != NULL) {
	  clause = 0;

	  while ((clause = sxba_scan (line, clause)) > 0) {
	    /* printf ("%i ", clause); */

	    bot = G->clause2identical_disp [clause];
	    top = G->clause2identical_disp [clause+1];
	    nbs += nb = 1 + top - bot;

	    if (unknown_word)
	      uw_nb += nb;
	  }
	}
	else {
	  nbs += nb = sxba_cardinal (line);

	  if (unknown_word)
	    uw_nb += nb;
	}
      }    
    }

    printf ("%i %i %i ", nbs, old_nbs, uw_nb);

#else /* !MEASURES */
#if EBUG2
    /* Pour chaque index du source de chaque pid on calcule l'ensemble des clauses qui etaient
       initialement selectionnees par la lexicalisation */
    for (pid = 0; pid < PID; pid++) {
      G = Gs [pid];

      lex_pidXindex2clause_nb [pid] = (SXINT*) sxalloc (n+1, sizeof (SXINT));
      lex_pidXindex2clause_nb [pid][0] = sxba_cardinal (G->t2clause_set [0]);

      clause_set = sxba_calloc (G->clause_nb+1);

      for (pos = init_pos; pos <= n; pos++) {
	if (pos == 0) {
	  sxba_copy (clause_set, G->t2clause_set [0]);
	}
	else {
	  line = glbl_source [pos];

	  t = 0;

	  while ((t = sxba_scan (line, t)) > 0) {
	    sxba_or (clause_set, G->t2clause_set [t]);
	  }
	}

	clause = 0;
	nb = 0;

	while ((clause = sxba_scan_reset (clause_set, clause)) > 0) {
	  nb++;

	  if (G->clause2identical_disp != NULL) {
	    nb += G->clause2identical_disp [clause+1] - G->clause2identical_disp [clause];
	  }
	}

	lex_pidXindex2clause_nb [pid][pos] = nb;
      }

      sxfree (clause_set);
    }

    nbs = old_nbs = 0;
#endif /* EBUG2 */

    fputs ("%[ ", stdout);

    /* init_pos == 0 correspond a la sortie des clauses non lexicalisees */
    for (pos = init_pos; pos <= n; pos++) {
      if (pos == 0) {
	fputs ("t_free {", stdout);
      }
      else {
	if ((ste = ff_ste [pos]) < 0)
	  str = sxttext (sxplocals.sxtables, -ste);
	else
	  str = sxstrget (ste);

	printf ("%s {", str);
      }

      /* Modif du 29/07/2002.  Les clauses equivalentes sont ajoutees au moment de la sortie */
      for (pid = 0; pid < PID; pid++) {
	line = ff2clause_set [pid][pos];
      
	G = Gs [pid];

	if (G->clause2identical_disp != NULL) {
	  clause = G->clause_nb+1;

	  while ((clause = sxba_1_rlscan (line, clause)) > 0) {
	    if (((bot = G->clause2identical_disp [clause]) < (top = G->clause2identical_disp [clause+1]))) {
	      do {
		eq_clause = G->clause2identical [bot];
		SXBA_1_bit (line, eq_clause); /* ... sans effet sur le sxba_1_rlscan */
	      } while (++bot < top);
	    }
	  }
	}

	if (pid != 0)
	  printf ("%i.", pid);

	fputs ("{", stdout);
	is_first = TRUE;
	nb = 0;
	clause = 0;

	while ((clause = sxba_scan (line, clause)) > 0) {
	  if (is_first)
	    is_first = FALSE;
	  else
	    fputs (" ", stdout);

	  if (clause2tree_name)
	    printf ("%s", clause2tree_name [clause]);
	  else
	    printf ("%i", clause);

	  nb++;
	}
    
	fputs ("}", stdout);

#if EBUG2
	nbs += nb;
	old_nbs += old_nb = lex_pidXindex2clause_nb [pid][pos];
	printf ("[%i/%i]", nb, old_nb);
#endif /* EBUG2 */
      }
	    
      fputs ("} ", stdout);    
    }

#if EBUG2
	printf ("[%i/%i] ", nbs, old_nbs);
#endif /* EBUG2 */  

    fputs ("%]\n", stdout);

#if EBUG2
    for (pid = 0; pid < PID; pid++)
      sxfree (lex_pidXindex2clause_nb [pid]);
#endif /* EBUG2 */
#endif /* !MEASURES */
  }
}
#endif /* is_sdag */


static void
print_used_clause (struct G **Gs)
{
    SXINT		clause, pid, nb;
    SXBA	clause_set;
    BOOLEAN	is_first;

    /* On sort les clauses utilisees */
    fputs ("%% ", stdout);

    for (pid = 0; pid < PID; pid++) {
	clause_set = used_clause [pid];
    
	if (pid != 0)
	    printf (" %i.", pid);

	fputs ("{", stdout);
		
	clause = 0;
	is_first = TRUE;
	nb = 0;

	while ((clause = sxba_scan (clause_set, clause)) > 0) {
	    if (is_first)
		is_first = FALSE;
	    else
		fputs (" ", stdout);

	    printf ("%i", clause);
	    nb++;
	}

	fputs ("}", stdout);

#if EBUG2
	/* Pour la grammaire courante, (lexicalization () a pu ne pas etre executee), on
	   calcule le nb de clauses selectionne par le source */
        {
	    SXINT		tok;
	    struct G	*G;
	    SXBA 	set;

	    /* On "simule" une execution directe par le parseur guide' */
	    G = Gs [pid];

	    set = sxba_calloc (G->clause_nb+1);
	    sxba_copy (set, (G->t2clause_set) [0]);

	    tok = 0;

	    while ((tok = sxba_scan (source_set, tok)) >= 0) {
		sxba_or (set, (G->t2clause_set) [tok]);
	    }

	    printf ("%[%i/%i/%i%]", nb, sxba_cardinal (set), G->clause_nb);

	    sxfree (set);
	}
#endif
    }

    fputs ("\n", stdout);
}

#if is_parse_forest

#if is_sdag
/* Le source est une se'quence L ; L ; ... L ou chaque L a la forme
   ff {t1, t2, ... tq}

   ou` ff est une forme fle'chie et les ti l'ensemble des terminaux de la grammaire qui
   lui sont associes.  Le vrai source aurait la forme "ff1 ff2 ... ffn". */

/* Ici on est ds les semact associees a la bnf (generee et modifiee a la main) produite par rcg.out
   sur la rcg associee a la TAG */

/* On fabrique le source sous forme d'ONDEs et on lance la RCG */

static SXINT
print_source_text (struct G **Gs, SXINT S0n, SXINT ret_val)
{
    /* ret_val different de -1 ssi l'analyse a marche' */
    /* S0n est l'identifiant du couple axiome, (vecteur de) range [0..n] */
    SXINT		tok, ste, t, pid, clause, nb, i;
    char	*str;
    BOOLEAN	is_first;
    SXBA	line, set;
    struct G	*G;

    if (ret_val != -1) {
	if (!is_parse_tree_number && !is_no_semantics) {
	    if (forest_level & FL_clause) {
		print_used_clause (Gs);
	    }

	    if (forest_level & FL_source || out_ff2clause_set [0]) {
		/* On ressort le sdag reduit */
	        /* fputs ("% ", stdout); Le 26/2/2002 */
	        out_sdag (Gs, glbl_out_source, out_ff2clause_set, 1 /* On ne sort que les clauses lexicalisees */);
	    }
	    else {
		if (is_guiding || is_full_guiding) {
		    /* On est ds le cas ou la parse-forest va servir de guide */
		    /* On ressort le source tel quel :
		       le filtrage (lexicalization) sera faite par un autre moyen.
		       Le cout du calcul du nouveau source est loin d'etre negligeable (en particulier
		       si on est ds le cas rav2, il faut deplier toutes les parties droites) */
		    
		    fputs ("% ", stdout);

		    for (tok = 1; tok <= n; tok++) {
			if ((ste = ff_ste [tok]) < 0)
			    str = sxttext (sxplocals.sxtables, -ste);
			else
			    str = sxstrget (ste);

			printf ("%s {", str);
			line = glbl_source [tok];
			t = 0;
			is_first = TRUE;

			while ((t = sxba_scan (line, t)) > 0) {
			    if (is_first)
				is_first = FALSE;
			    else
				fputs (" ", stdout);

			    printf ("%s", sxttext (sxplocals.sxtables, t));
			}
	    
			fputs ("} ", stdout);       
		    }

		    fputs ("\n", stdout);
		}
	    }
	}
    }

    return ret_val;
}
#else/* !is_sdag */
static SXINT
print_source_text (struct G **Gs, SXINT S0n, SXINT ret_val)
{
    SXINT tok;

    if (ret_val != -1) {
	if (!is_parse_tree_number && !is_no_semantics) {
	    if (forest_level & FL_clause) {
		print_used_clause (Gs);
	    }

	    if (forest_level & FL_clause) {
		/* On ressort le source */
		puts ("% ");

		for (tok = 1; tok <= n; tok++) {
		    printf ("%s ", sxstrget (SXGET_TOKEN (tok).string_table_entry));
		}

		fputs ("\n", stdout);
	    }

	    /* A FAIRE : comme ds le cas sdag ... */
	}
    }

    return ret_val;
}
#endif /* !is_sdag */
#endif /* is_parse_forest */

#if EBUG2
static void
out_ebug2 ()
{

  if (nt_calls) {
    fprintf (stdout, "\n%[ There are %u predicate calls, %u selected clauses and %u valid instantiated clauses for this execution. %]\n",
	     nt_calls, clause_calls, instantiated_clause_nb);
    nt_calls = clause_calls = instantiated_clause_nb = 0;
  }
  else {
    if (instantiated_clause_nb) {
      fprintf (stdout, "\n%[ There are %u valid instantiated clauses for this execution. %]\n", instantiated_clause_nb);
      instantiated_clause_nb = 0;
    }
  }

    if (memo_prdct_hit) {
	fprintf (stdout, "%[ The instantiated predicate memoization cells have been hit #%u times. %]\n",
		 memo_prdct_hit);
	memo_prdct_hit = 0;
    }

    if (memo_clause_hit) {
	fprintf (stdout, "%[ The selected clause memoization cells have been hit #%u times. %]\n",
		 memo_clause_hit);
	memo_clause_hit = 0;
    }

    if (total_clause_nb) {
	fprintf (stdout, "%[ Lexicalization clause filtering: (lex3/lex/total) = %u/%u/%u. %]\n",
		 total_clause2_nb, total_clause1_nb, total_clause_nb);
	total_clause2_nb = total_clause1_nb = total_clause_nb = 0;
    }

    if (dynam_lex_call_nb) {
	fprintf (stdout, "%[ Dynamic lexicalization instantiated clause filtering: %u/%u. %]\n",
		 dynam_lex_call_nb-dynam_lex_false_call_nb, dynam_lex_call_nb);
	dynam_lex_call_nb = dynam_lex_false_call_nb = 0;
    }

#if is_guided || is_coupled
    if (iarg_searched_in_guide) {
	fprintf (stdout, "%[ The guide has been successfully searched for %u instantiated args out of %u attempts. %]\n",
		 iarg_found_in_guide, iarg_searched_in_guide);
	iarg_searched_in_guide = iarg_found_in_guide = 0;
    }
    
    if (iprdct_searched_in_guide) {
	fprintf (stdout, "%[ The guide has been successfully searched for %u instantiated predicates out of %u attempts. %]\n",
		 iprdct_found_in_guide, iprdct_searched_in_guide);
	iprdct_searched_in_guide = iprdct_found_in_guide = 0;
    }
    
    if (iclause_searched_in_guide) {
	fprintf (stdout, "%[ The guide has been successfully searched for %u instantiated clauses out of %u attempts. %]\n",
		 iclause_found_in_guide, iclause_searched_in_guide);
	iclause_searched_in_guide = iclause_found_in_guide = 0;
    }
#endif /* is_guided || is_coupled */
}
#endif /* EBUG2 */


#if !is_sdag
/* Cas des &StrEq dont les args ne sont pas des ranges */
static SXINT	*concat_area1, concat_area_size1, *concat_area2, concat_area_size2;


BOOLEAN
call_rcgparse_it ()
{
    BOOLEAN	ret_val;
    SXINT		tok, pid, top, size;

#ifdef NO_SX
    extern BOOLEAN rcgscan_it ();
#endif /* !NO_SX */
    
    /* Ce programme doit etre compile avec l'option
       -DPID=p -DINTERFACE="_g1_if(),...,_gp_if()"
       S'il y a p sous-parser de nom g1, ..., gp (les grammaires s'appellent g1.rcg, ..., gp.rcg
       */

#if is_coupled
    /* interface du parser guide' */
    global_pid = 0;
    INTERFACE2;
#if PID>1
    for (pid = 0; pid < PID; pid++) {
	Guided_Gs [pid] = Gs [pid]; /* Pour coupled_last_pass */
	Guided_Gs [pid]->Gs = &(Guided_Gs [0]);
    }

    check_combinatorial_options (Guided_Gs [0]);
#else
    Guided_Gs [0] = Gs [0];
    Guided_Gs [0]->Gs = &(Guided_Gs [0]);
#endif /* PID>1 */
#endif /* is_coupled */

#ifdef SYNCHRONOUS
    /* interface de la partie synchrone */
    global_pid = 0;
    INTERFACE_SYNCHRO;
#if PID>1
    for (pid = 0; pid < PID; pid++) {
      synchro_Gs [pid] = Gs [pid]; /* Pour coupled_last_pass */
      synchro_Gs [pid]->Gs = &(synchro_Gs [0]);
    }

    check_combinatorial_options (synchro_Gs [0]);
#else
    synchro_Gs [0] = Gs [0];
    synchro_Gs [0]->Gs = &(synchro_Gs [0]);
#endif /* PID>1 */
#endif /* SYNCHRONOUS */

#if !is_guided
	/* Sinon, deja fait ds guide_semact */
#if is_rav2
    /* interface des parsers d'origine */
    global_pid = 0;
    INTERFACE_RAV2;
#if PID>1
    for (pid = 0; pid < PID; pid++) {
	Rav2_Gs [pid] = Gs [pid]; /* Pour coupled_last_pass */
	Rav2_Gs [pid]->Gs = &(Rav2_Gs [0]);
    }
#else
    Rav2_Gs [0] = Gs [0];
    Rav2_Gs [0]->Gs = &(Rav2_Gs [0]);
#endif /* PID>1 */
#endif /* is_rav2 */

    global_pid = 0;
    INTERFACE;

#if PID>1
    for (pid = 0; pid < PID; pid++) {
	Gs [pid]->Gs = &(Gs [0]);
    }

    check_combinatorial_options (Gs [0]);
#else
    Gs [0]->Gs = &(Gs [0]);
#endif
#endif /* !is_guided */

#if is_generator
    source_generator_init (2*n);
#endif

    /* Le 26/05/04 */
    /* ptr vers le sxword_header ou sont ranges les terminaux du module principal */
    t_names_ptr = Gs [0]->t_names;

#ifdef NO_SX
    rcgscan_it (t_names_ptr);
    /* Positionne (entre autres) sxsvar.SXS_tables.S_termax */
#endif /* !NO_SX */

#if PID>1
    {
      SXINT	pid;

      gtmax = sxsvar.SXS_tables.S_termax;
#if 0
      /* Le 26/05/04 str_tbl est remplace par t_names_ptr qui (va) pointer vers le sxword_header
	 du module principal qui contient tous les terminaux */
      SXUINT	ste;
      SXINT		glahead;
      extern char	*malloc (), *calloc (), *realloc ();
      extern void free ();

      sxword_alloc (&str_tbl, "str_tbl", gtmax+1, 1, 10,
		  malloc, calloc, realloc, NULL /* resize */, free, NULL, NULL);
#endif /* 0 */
      g2l = (SXINT **) sxalloc (PID+1, sizeof (SXINT*));
      l2g = (SXINT **) sxalloc (PID+1, sizeof (SXINT*));
      gl_area = (SXINT *) sxcalloc (2*PID*gtmax, sizeof (SXINT));

      g2l [0] = l2g [0] = NULL;
      g2l [1] = gl_area;
      l2g [1] = gl_area + PID*gtmax;

      for (pid = 2; pid <= PID; pid++) {
	g2l [pid] = g2l [pid-1] + gtmax;
	l2g [pid] = l2g [pid-1] + gtmax;
      }

#if 0
      /* Le 26/05/04 voir commentaire ci_dessous (t_names est initialise' statiquement) */
      for (glahead = 1; glahead < gtmax; glahead++) {
	ste = sxword_save (&str_tbl, sxttext (sxplocals.sxtables, glahead));
#if EBUG4
	if (ste != glahead+1)
	  sxtrap (ME, "call_rcgparse_it");
#endif /* EBUG4 */
      }
#endif /* 0 */
    }
#endif /* PID>1 */

#if !is_parse_forest
    forest_level = 0;
#endif

    glbl_source_area = glbl_source = (SXINT*) sxalloc (sxplocals.Mtok_no+1, sizeof (SXINT));
    glbl_source [0] = sxsvar.SXS_tables.S_termax;

    source_set = sxba_calloc (sxsvar.SXS_tables.S_termax);
    source_top = source_set + NBLONGS (BASIZE (source_set)); /* pointe vers la derniere "tranche" */

    for (n = 1; n <= sxplocals.Mtok_no; n++) {
	tok = SXGET_TOKEN (n).lahead;
#if is_generator==0 && ANY
	/* Aucun source ne peut comporter de ANY, on met 0! */
	if (tok == ANY)
	    tok = 0;
#endif
	glbl_source [n] = tok;
#if is_semantics && is_generator && ANY
	if (tok == ANY)
	    ANY_nb++;
#endif

	if (n < sxplocals.Mtok_no)
	    SXBA_1_bit (source_set, tok);
    }

    rcvr_n = sxplocals.Mtok_no; /* pointe sur eof */
    n = sxplocals.Mtok_no-1;
    logn = sxlast_bit (n);
    ANDj = (1<<logn)-1;

    is_robust_run = FALSE; /* Le 1er essai est non robuste */

    ret_val = rcgparse_it (n, 1);

#if 0
    /* Le 26/05/04 */
#if PID>1
    sxword_free (&str_tbl);
#endif
#endif /* 0 */

#if is_coupled
#if EBUG2
    out_ebug2 ();
#endif

    if (ret_val) {
	/* DEBUT de la PASSE GUIDEE/COUPLEE */

	/* Si la construction du guide s'est faite par une forme 2var, il faut changer la valeur
	   du lognt des grammaires guidees car le rhoA_hd utilise' est celui de la forme 2var
	   du constructeur de guide */
#if is_rav2
#if PID>1
	for (pid = 0; pid < PID; pid++) {
	    Guided_Gs [pid]->guide_lognt = Gs [pid]->lognt;
	}
#else
	Guided_Gs [0]->guide_lognt = Gs [0]->lognt;
#endif
#endif /* is_rav2 */

#if is_rav22
	/* On commence par appeler l'interface des parsers d'origine dont on va memoriser les grammaires */
	global_pid = 0;
	INTERFACE2_RAV2;
#if PID>1
	for (pid = 0; pid < PID; pid++) {
	    Rav2_Gs [pid] = Gs [pid]; /* Pour coupled_last_pass */
	    Rav2_Gs [pid]->Gs = &(Rav2_Gs [0]);
	}
#else
	Rav2_Gs [0] = Gs [0];
	Rav2_Gs [0]->Gs = &(Rav2_Gs [0]);
#endif /* PID>1 */
#endif /* is_rav22 */

	/* On reinitialise et on lance la passe guidee */
#if PID>1
	for (pid = 0; pid < PID; pid++) {
	    Gs [pid] = Guided_Gs [pid];
	    Gs [pid]->Gs = &(Gs [0]);
	}
#else
	Gs [0] = Guided_Gs [0];
	Gs [0]->Gs = &(Gs [0]);
#endif /* PID>1 */
	/* Ici on a Gs == Guided_Gs */
 	
	is_robust_run = FALSE;
	ret_val = rcgparse_it (n, 3);
    }
#endif /* is_coupled */

    sxfree (glbl_source_area), glbl_source_area = glbl_source = NULL;
    sxfree (source_set), source_set = NULL;

#if is_generator
    source_generator_free ();
#endif

#if PID>1
    sxfree (g2l), g2l = NULL;
    sxfree (l2g), l2g = NULL;
    sxfree (gl_area), gl_area = NULL;
#endif

    if (concat_area1) sxfree (concat_area1), concat_area1 = NULL;
    if (concat_area2) sxfree (concat_area2), concat_area2 = NULL;

    if (is_print_time)
	sxtime (TIME_FINAL, "\tDone");

    return ret_val;
}



BOOLEAN
sxrcg_parser (SXINT what_to_do, struct sxtables *arg)
{
  char mess [32];

  switch (what_to_do) {
  case BEGIN:
    return TRUE;

  case OPEN:

    sxcheck_magic_number (arg->magic, SXMAGIC_NUMBER, "sxrcg_parser");

    sxplocals.sxtables = arg;
    sxplocals.SXP_tables = arg->SXP_tables;
    sxtkn_mngr (OPEN, 2);

#if is_rcvr
    (*sxplocals.SXP_tables.recovery) (OPEN);
#endif

    return TRUE;

  case INIT:
    sxtkn_mngr (INIT, 0);
    
#if 0
    /* analyse normale */
    /* valeurs par defaut qui peuvent etre changees ds les scan_act ou pars_act. */
    sxplocals.mode.look_back = 0; /* All tokens are kept. */
    sxplocals.mode.mode = SXPARSER;
    sxplocals.mode.kind = SXWITH_RECOVERY | SXWITH_CORRECTION;
    sxplocals.mode.local_errors_nb = 0;
    sxplocals.mode.global_errors_nb = 0;
    sxplocals.mode.is_prefixe = FALSE;
    sxplocals.mode.is_silent = FALSE;
    sxplocals.mode.with_semact = TRUE;
    sxplocals.mode.with_parsact = TRUE;
    sxplocals.mode.with_parsprdct = TRUE;
    sxplocals.mode.with_do_undo = FALSE;
#endif

    return TRUE;

  case ACTION:
  {
    SXINT		lahead;
    BOOLEAN	ret_val, store_print_time;

#if is_generator
#ifndef ANY
    fprintf (sxstderr, "%s: The -DGENERATOR compilation option implies -DANY=n.\n", ME);
    return FALSE;
#endif
#endif

    if (is_print_time)
      sxtime (INIT, NULL);

#if TCUT
/* L'option de compilation -DTCUT=t_code permet de couper l'analyse d'un source
   en morceaux separes par t_code */
    ret_val = TRUE;
    store_print_time = is_print_time;
    is_print_time = FALSE;

    do {

      do {
	(*(sxplocals.SXP_tables.scanit)) ();
	lahead = SXGET_TOKEN (sxplocals.Mtok_no).lahead;
      } while (lahead != sxsvar.SXS_tables.S_termax && lahead != TCUT);

      n = sxplocals.Mtok_no - 1;

      if (lahead == TCUT)
	SXGET_TOKEN (sxplocals.Mtok_no).lahead = sxsvar.SXS_tables.S_termax;

      ret_val &= call_rcgparse_it ();

      if (lahead == TCUT)
	sxplocals.Mtok_no = sxplocals.atok_no = sxplocals.ptok_no = 0;
    } while (lahead != sxsvar.SXS_tables.S_termax);

    if (store_print_time) {
      is_print_time = TRUE;
      sxtime (TIME_FINAL, "\tTotal parse time");
    }
#else
    do {
      (*(sxplocals.SXP_tables.scanit)) ();
    } while (SXGET_TOKEN (sxplocals.Mtok_no).lahead != sxsvar.SXS_tables.S_termax);

    if (is_print_time) {
      sprintf (mess, "\tScanner[%i]", sxplocals.Mtok_no-1);
      sxtime (TIME_FINAL, mess);
    }

    ret_val = call_rcgparse_it ();
#endif

#if EBUG2
    out_ebug2 ();
#endif

    return ret_val;
  }

  case FINAL:
    sxtkn_mngr (FINAL, 0);

    return TRUE;

  case CLOSE:
    /* end of language: free the local arrays */
    sxtkn_mngr (CLOSE, 0);
#if is_rcvr
    (*sxplocals.SXP_tables.recovery) (CLOSE);
#endif

    return TRUE;

  case END:
    /* free everything */
    return TRUE;

  default:
    fprintf (sxstderr, "The function \"sxrcg_parser\" is out of date with respect to its specification.\n");
    sxexit(1);
  }
}
#endif /* #if !is_sdag */

/* - Le scanner est celui associe au module principal (local_pid == 0), il doit donc connaitre tous les
   tokens utilises ds tous les modules.
   - Pour un module secondaire (local_pid > 0), fill_buf remplit un source (buf) local a ce module avec
   une traduction de glbl_source en terme de codes de tokens de ce module.
   - Les tokens du module principal qui n'existent pas ds ce module secondaire prennent le code 0.
   - Les predicats externes, points d'entree des modules secondaires, commencent par verifier que leur
   input ranges ne selectionnent pas de codes nuls.  Ca permet de conserver des clauses de la forme
   A(X) --> . ou A(X T) --> &StrLen(1,T) ... . sans avoir a regarder que X ou T ne contiennent pas de
   tokens inconnus. */
#if PID == 1
void fill_buf ()
{
}
#else /* PID != 1 */
void
fill_buf (SXINT local_pid,
#if is_sdag
	  SXBA  *buf,
#else /* !is_sdag */
	  SXINT	*buf,
#endif /* !is_sdag */
	  char **t2str,
	  SXINT tmax
	  )
{
  /* Cette procedure effectue dynamiquement la traduction des codes internes des tokens du module principal
     en le code interne du module appelant */
  SXUINT	ste;
  SXINT			lahead, *g2l_ptr, *l2g_ptr, i;
#if is_sdag
  SXINT t, buf_t;
  SXBA  glbl_level_set, buf_level_set;
#endif /* is_sdag */

  g2l_ptr = g2l [local_pid];
  l2g_ptr = l2g [local_pid];

  for (lahead = 1; lahead <= tmax; lahead++) {
    ste = sxword_retrieve (t_names_ptr/* ex &str_tbl */, t2str [lahead]);

    if (ste != ERROR_STE) {
      g2l_ptr [ste-1] = lahead;
      l2g_ptr [lahead] = ste-1;
    }
    /* else le 19/05/04
       on est sur un terminal non defini ds le module principal, on ne detecte pas d'erreur
       on laisse le mecanisme normal ([L]SCAN) s'en charger.  C,a permet d'avoir des &Lex("et")
       alors que "et" n'est pas un terminal (du module principal) et donc, par la "lexicalisation"
       d'exclure la clause contenant cet &Lex("et")  */
  }

#if is_sdag
  sxba_copy (buf [0], glbl_source [0]);
  sxba_copy (buf [n+1], glbl_source [n+1]);

  for (i = 1; i <= n; i++) {
    glbl_level_set = glbl_source [i];
    buf_level_set = buf [i];
    
    t = -1;

    while ((t = sxba_scan (glbl_level_set, t)) >= 0) {
      buf_t = g2l_ptr [t];
      SXBA_1_bit (buf_level_set, buf_t);
    }
  }
#else /* !is_sdag */
  buf [0] = glbl_source [0];
  buf [n+1] = glbl_source [n+1];

  for (i = 1; i <= n; i++)
    buf [i] = g2l_ptr [glbl_source [i]];
#endif /* !is_sdag */
}
#endif /* PID != 1 */


void
arg_nb_put_error (char *prdct_name, SXINT def_nb, SXINT decl_nb)
{
#ifdef NO_SX
  fprintf (sxstderr,
	  "The numbers of arguments in the definition and declaration of the external predicate \"%s\" (resp. %i and %i) are incompatible.\n",
	  prdct_name, def_nb, decl_nb);
#else /* !NO_SX */
  sxhmsg (sxsrcmngr.source_coord.file_name,
	  "%sThe numbers of arguments in the definition and declaration of the external predicate \"%s\" (resp. %i and %i) are incompatible.",
	  sxplocals.sxtables->err_titles [1], prdct_name, def_nb, decl_nb);
#endif  /* !NO_SX */
}

#if is_sdag
SXINT *
StrConcat (BOOLEAN is_first_arg, SXINT *arg, SXINT *Xlb, SXINT *Xub)
{
    /* A FAIRE */
}
#else
SXINT *
StrConcat (BOOLEAN is_first_arg, SXINT *arg, SXINT *Xlb, SXINT *Xub)
{
  /* On a au + 2 args a _StrEq2 => implantes ds des structures separees */
  /* On concatene arg_size = arg[0] chaines reperees par le contenu de arg[1]...arg[arg_size] */
  /* Si v = arg[i] est > 0 c'est le token v */
  /*  			<= 0 alors Xlb[-v]..Xub[-v] est un range */
  /* On met le resultat ds result */
  SXINT arg_size, size, i, v, *pz, *p, *ptop, *result;

  arg_size = arg [0];
  size = 0;

  for (i = 1; i <= arg_size; i++) {
    v = arg [i];

    if (v > 0)
      size++;
    else {
      v = -v;
      size += Xub[v]-Xlb[v];
    }
  }

  /* Le resultat tient ds size+1 */

  if (is_first_arg) {
    if (concat_area1 == NULL) {
      concat_area1 = (SXINT*) sxalloc ((concat_area_size1 = 2*(n+1))+1, sizeof (SXINT));
    }

    if (size > concat_area_size1) {
      while ((concat_area_size1 *= 2) < size);

      concat_area1 = (SXINT*) sxrealloc (concat_area1, concat_area_size1+1, sizeof (SXINT));
    }

    pz = concat_area1;
  }
  else {
    if (concat_area2 == NULL) {
      concat_area2 = (SXINT*) sxalloc ((concat_area_size2 = 2*(n+1))+1, sizeof (SXINT));
    }

    if (size > concat_area_size2) {
      while ((concat_area_size2 *= 2) < size);

      concat_area2 = (SXINT*) sxrealloc (concat_area2, concat_area_size2+1, sizeof (SXINT));
    }

    pz = concat_area2;
  }

  result = pz;
  *pz++ = size;
  /* resultat ds result */

  for (i = 1; i <= arg_size; i++) {
    v = arg [i];

    if (v > 0)
      *pz++ = v;
    else {
      v = -v;
      p = glbl_source+Xlb [v];
      ptop = glbl_source+Xub [v];

      while (++p <= ptop) {
	*pz++ = *p;
      }
    }
  }

  return result;
}


SXINT
_StrEq2 (SXINT *rho0, SXINT size1, SXINT *arg1, SXINT size2, SXINT *arg2)
{
  /* Predicat analogue a _StrEq, excepte que les chaines sont ds arg1 et arg2, (arg1[0] et arg2[0]
     sont les longueurs) */
  /* Utilises lorsqu'un arg de &StrEq n'est pas un range */
  /* Il y a eu un appel de StrConcat */

  SXINT		*ptop1;

  if (size1 != size2) return -1;

  ptop1 = arg1 + size1;

  while (arg1 < ptop1) {
    if (*arg1++ != *arg2++) return -1;
  }
    
  *rho0 = 0; /* A FAIRE */

  return 1;
}

SXINT
_StrLen2 (SXINT *arg, SXINT *Xlb, SXINT *Xub)
{
  /* calcule la taille de l'arg */
  /* On concatene arg_size = arg[0] chaines reperees par le contenu de arg[1]...arg[arg_size] */
  /* Si v = arg[i] est > 0 c'est le token v */
  /*  			<= 0 alors Xlb[-v]..Xub[-v] est un range */
  SXINT *arg_top, size, v;

  arg_top = arg + *arg;
  size = 0;

  while (++arg <= arg_top) {
    v = *arg;

    if (v > 0)
      size++;
    else {
      v = -v;
      size += Xub[v]-Xlb[v];
    }
  }

  return size;
}
#endif

#if AG
void
fill_idag (SXINT local_pid, struct idag	*pidag)
{
    /* Foreach sur X, XY et YZ */
    static SXINT 		average_XYZ_assoc [6] = {1,0,0,1,0,1};

    SXINT 		i, j, triple, X, Z, tok, next_tok, nb, x, y, prev_state, cur_state, top_state, next_state, bot_state;
    SXINT			*stack;
    SXBA		line;

    if (local_pid == 0) {
	XxYxZ_alloc (&(pidag->idag_hd), "main_idag", n+1, 1, average_XYZ_assoc, NULL, statistics);
	/* glbl_source contient une expression reguliere de la forme \a \( \_ \5 \b \| \c \) \d ou
	   \_ \5 est une repetition de 1 a 5 \_ et les sequences \( ... \| ... \| ... \) designent
	   des alternatives */
	stack = (SXINT*) sxalloc (n+1, sizeof (SXINT));
	stack [0] = 0;
	prev_state = cur_state = top_state = 0;

	for (i = 1; i <= n ; i++) {
	    tok = glbl_source [i];
#if is_generator && ANY
	    if (tok == ANY) {
		tok = glbl_source [i+1];
		nb = sxttext (sxplocals.sxtables, tok)[0]-'0';

		if (nb < 0 || nb > 9) {
		    nb = 1;
		}
		else
		    i++;

		if (nb > 0) {
		    top_state += (nb*(nb-1)/2) + 1;
		    cur_state = bot_state = prev_state;

		    for (x = 1; x <= nb; x++) {
			prev_state = bot_state;

			for (y = 1; y <= x; y++) {
			    if (y == x)
				next_state = top_state;
			    else
				next_state = ++cur_state;

			    XxYxZ_set (&(pidag->idag_hd), prev_state, ANY, next_state, &triple);
			    prev_state = next_state;
			}
		    }

		    prev_state = top_state;
		}
	    }
	    else
#endif
		if (sxttext (sxplocals.sxtables, tok)[0] == '(') {
		    stack [++stack [0]] = prev_state;
		}
		else if (sxttext (sxplocals.sxtables, tok)[0] == ')') {
		    prev_state = stack [stack [0]];

		    if (stack [0] <= 0 || prev_state > 0) {
			call_put_error (i, "%sOnly alternatives must be parenthesized.\n", sxplocals.sxtables->err_titles [2]);
			sxexit (3);
		    }

		    prev_state = -prev_state;
		    stack [0]--;
		}
		else if (sxttext (sxplocals.sxtables, tok)[0] == '|') {
		    prev_state = stack [stack [0]-1];
		}
		else {
		    next_tok = glbl_source [i+1];

		    if (sxttext (sxplocals.sxtables, next_tok)[0] == ')')
			next_state = -stack [stack [0]];
		    else if (sxttext (sxplocals.sxtables, next_tok)[0] == '|') {
			if (stack [0] <= 0) {
			    call_put_error (i, "%sAlternatives must be parenthesized.\n", sxplocals.sxtables->err_titles [2]);
			    sxexit (3);
			}

			if ((next_state = stack [stack [0]]) < 0)
			    /* Ce n'est pas le premier ou */
			    next_state = -next_state;
			else {
			    next_state = ++top_state;
			    stack [++stack [0]] = -next_state;
			}
		    }
		    else
			next_state = ++top_state;
			
		    
		    XxYxZ_set (&(pidag->idag_hd), prev_state, tok, next_state, &triple);
		    prev_state = next_state;
		}

	}

	n = top_state;

	pidag->delta = sxbm_calloc (n+1, n+1);
	
	XxYxZ_foreach(pidag->idag_hd, triple) {
	    X = XxYxZ_X (pidag->idag_hd, triple);
	    Z = XxYxZ_Z (pidag->idag_hd, triple);
	    SXBA_1_bit (pidag->delta [X], Z);
	}

	fermer (pidag->delta, n+1);

	for (i = 0; i <= n; i++)
	    SXBA_1_bit (pidag->delta [i], i);

	pidag->delta_1 = sxbm_calloc (n+1, n+1);

	for (j = 0; j <= n; j++) {
	    line = pidag->delta_1 [j];
	    for (i = 0; i <= n; i++) {
		if (SXBA_bit_is_set (pidag->delta [i], j))
		    SXBA_1_bit (line, i);
	    }
	}

	glbl_idag = *pidag;

	sxfree (stack);

	AG_alloc ();
    }
    else {
	/* A FAIRE recopie de  glbl_idag en changeant les codes des transitions */
	XxYxZ_alloc (&(pidag->idag_hd), "secondary_idag", XxYxZ_top (glbl_idag.idag_hd)+1, 1, average_XYZ_assoc, NULL, statistics);

	/* memes transitions qq soit le module */
	pidag->delta = glbl_idag.delta;
	pidag->delta_1 = glbl_idag.delta_1;
    }
}

void
free_idag (SXINT local_pid, struct idag	*pidag)
{
    XxYxZ_free (&(pidag->idag_hd));

    if (local_pid == 0) {
	sxbm_free (pidag->delta), pidag->delta = NULL;
	sxbm_free (pidag->delta_1), pidag->delta_1 = NULL;
	AG_free ();
    }
}
#endif


#if is_sdag

#if is_lfsa

static void
add_loop_clauses (struct G *G, BOOLEAN is_full)
{
  SXINT pid, clause, A, x, arity, pos, d, cur, *clause2clause0, *p;
  SXBA t_set, valid_nt_set, valid_clause_set, *clause02lb, *clause02ub, *ub_lines, *ul_lines;

  pid = G->pid;

  valid_clause_set = used_clause [pid];

  if (is_full) {
    valid_nt_set = valid_nt_sets [pid];
    clause2clause0 = clause2clause0s [pid];
    clause02lb = clause02lbs [pid];
    clause02ub = clause02ubs [pid];
  }
  else {
    valid_nt_set = sxba_calloc (G->ntmax+1);

    clause = 0;

    while ((clause = sxba_scan (valid_clause_set, clause)) > 0) {
      A = G->clause2lhs_nt [clause];

      if (G->looped_nt [A])
	SXBA_1_bit (valid_nt_set, A);
    }
  }
  
  A = 0;  

  while ((A = sxba_scan (valid_nt_set, A)) > 0) {
    if (x = G->looped_nt [A]) {
      /* loop sur A */
      if (is_full) {
	arity = G->nt2arity [A];
	d = A2A0s [pid] [A];
	ub_lines = A02ubs [pid]+d;
	ul_lines = A02lbs [pid]+d;
      }

      p = G->looped_clause+x; 

      while (clause = *p++) {
	/* ... sur A(...) --> A(...) &Lex(...) */
	if ((t_set = G->Lex [clause]) == NULL || is_source_subset (t_set)) {
	  /* Ici, t_set == NULL <==> |t_set| <= 1 */
	  /* On peut donc laisser passer des loop clauses non verifiees par la lexicalisation */
	  /* On peut etre + precis en generant t_set == NULL <==> |t_set| <= 0.  mais ds ce cas
	     lexicalisation () sera un peu + long a s'executer!! */
	  SXBA_1_bit (valid_clause_set, clause);

	  if (is_full) {
	    cur = clause2clause0 [clause] = *clause2clause0+1;

	    for (pos = 0; pos < arity; pos++) {
	      clause02ub [cur] = ub_lines [pos];
	      clause02lb [cur] = ul_lines [pos];
	      *clause2clause0 = cur;
	      cur++;
	    }
	  }
	}
      }
    }
  }

  if (!is_full)
    sxfree (valid_nt_set);
}


static void
add_identical_clauses (struct G *G, BOOLEAN is_full)
{
  SXINT pid, clause, bot, top, eq_clause, arity, d, cur, pos, *clause2clause0;
  SXBA eq_clause_set, valid_clause_set, *clause02lb, *clause02ub, *ub_lines, *ul_lines;

  pid = G->pid;

  valid_clause_set = used_clause [pid];
	  
  if (is_full) {
    clause2clause0 = clause2clause0s [pid];
    clause02lb = clause02lbs [pid];
    clause02ub = clause02ubs [pid];
  }

  eq_clause_set = sxba_calloc (BASIZE (valid_clause_set));
  clause = 0;

  while ((clause = sxba_scan (valid_clause_set, clause)) > 0) {
    if (((bot = G->clause2identical_disp [clause]) < (top = G->clause2identical_disp [clause+1]))) {
      if (is_full) {
	arity = G->nt2arity [G->clause2lhs_nt [clause]];
	d = clause2clause0s [pid] [clause];
	ub_lines = clause02ubs [pid]+d;
	ul_lines = clause02lbs [pid]+d;
      }

      do {
	eq_clause = G->clause2identical [bot];
	SXBA_1_bit (eq_clause_set, eq_clause);
	  
	if (is_full) {
	  cur = clause2clause0 [eq_clause] = *clause2clause0+1;

	  for (pos = 0; pos < arity; pos++) {
	    clause02ub [cur] = ub_lines [pos];
	    clause02lb [cur] = ul_lines [pos];
	    *clause2clause0 = cur;
	    cur++;
	  }
	}
      } while (++bot < top);
    }
  }
	    
  sxba_or (valid_clause_set, eq_clause_set);
  sxfree (eq_clause_set);
}

void
lfsa_lexicalize (struct fsaG *G)
{
  SXBA 	set, t_set;
  SXINT	tok, clause;

  if (!G->shallow) G->fsa_valid_clause_set = sxba_calloc (G->clause_nb+1);

  G->Lex = sxba_calloc (G->clause_nb+1);

  set = sxba_calloc (G->clause_nb+1);
  sxba_copy (set, G->t2clause_set [0]);

  tok = 0;

  while ((tok = sxba_scan (source_set, tok)) >= 0) {
    sxba_or (set, (G->t2clause_set) [tok]);
  }

#if EBUG2
  total_clause_nb += G->clause_nb;
#endif

  clause = 0;

  while ((clause = sxba_scan (set, clause)) > 0) {
    if ((t_set = G->clause2t_set [clause]) == NULL || is_source_subset (t_set)) {
      /* Ici, t_set == NULL <==> |t_set| <= 1 */
#if EBUG2
      total_clause1_nb++;
#endif

      SXBA_1_bit (G->Lex, clause);
    }
  }

  SXBA_1_bit (G->Lex, 0); /* Valide les transitions vides */
  sxfree (set);
}


static void
out_shallow_lhs (struct G *G, SXINT clause, SXINT A)
{
  SXINT		arity, pos, pid, i, j, len, cur;
  SXBA          *lbs, *ubs, lb, ub;
  static char blank_str [] = "                                                                                                                                ";
  pid = G->pid;

  if (clause) {
    if (pid != 0)
      printf ("%i.", pid);

    /* Les clauses multiples font l'objet d'appels differents */

    printf ("%i: ", clause);

    A = G->clause2lhs_nt [clause];

    /* juste pour rigoler!! */
    len = 3;
    
    if (clause > 10) {
      len++;
    
      if (clause > 100) {
	len++;
    
	if (clause > 1000) {
	  len++;
	}
      }
    }
  }
  else
    len = 0;

  arity = G->nt2arity [A];

  printf ("%s (", G->nt2str [A]);
  len += strlen (G->nt2str [A])+2;
  blank_str [len] = NUL;

  if (clause) {
    cur = clause2clause0s [pid] [clause];
    lbs = clause02lbs [pid]+cur;
    ubs = clause02ubs [pid]+cur;
  }
  else {
    cur = A2A0s [pid] [A];
    lbs = A02lbs [pid]+cur;
    ubs = A02ubs [pid]+cur;
  }

  pos = 0;

  for (;;) {
    lb = lbs [pos];
    ub = ubs [pos];

    i = sxba_scan (lb, -1);
    j = sxba_scan (lb, i);

    if (j == -1)
      printf ("%i", i);
    else {
      printf ("{%i %i", i, j);
      i = j;

      while ((i = sxba_scan (lb, i)) >= 0)
	printf (" %i", i);

      fputs ("}", stdout);
    }

    fputs ("..", stdout);

    i = sxba_scan (ub, -1);
    j = sxba_scan (ub, i);

    if (j == -1)
      printf ("%i", i);
    else {
      printf ("{%i %i", i, j);
      i = j;

      while ((i = sxba_scan (ub, i)) >= 0)
	printf (" %i", i);

      fputs ("}", stdout);
    }

    if (++pos == arity) break;
	
    fputs (",\n", stdout);
    printf("%s", blank_str);
  }

  blank_str [len] = ' ';

  fputs (")\n", stdout);
}


static void
out_shallow_parse_forest (struct G **Gs)
{
  SXINT           pid, clause, A;
  SXBA          valid_clause_set, valid_nt_set;
  struct G 	*G;

  for (pid = 0; pid < PID; pid++) {
    G = Gs [pid];

    if (forest_level & FL_lhs_clause) {
      valid_clause_set = used_clause [pid];
      clause = 0;

      while ((clause = sxba_scan (valid_clause_set, clause)) > 0)
	out_shallow_lhs (G, clause, 0);
    }
    else {
      valid_nt_set = valid_nt_sets [pid];
      A = 0;

      while ((A = sxba_scan (valid_nt_set, A)) > 0)
	out_shallow_lhs (G, 0, A);
    }
  }
}


static void
copy_out_source2source (struct fsaG **fsaGs)
{
  /* Une passe vient de re'ussir et a construit un nouveau source ds glbl_out_source */
  /* On fabrique le nouveau source pour la passe suivante par recopie */
  SXINT  pid, tok;
  SXBA out_line;
  SXBA set, fsa_valid_clause_set;

  sxba_empty (source_set); /* On refabrique source_set */

  for (tok = 1; tok <= n; tok++) {
    out_line = glbl_out_source [tok];
    sxba_copy (glbl_source [tok], out_line);
    sxba_or (source_set, out_line);
    sxba_empty (out_line);
  }
    
  /* SXBA_1_bit (glbl_source [0], 0), SXBA_1_bit (glbl_source [n+1], 0); deja fait */
	
  /* Il se peut qu'une clause A --> \alpha &Lex(t) de lfsa_valid_clause_set
     ne soit plus valide car t a disparu du source... */
  for (pid = 0; pid < PID; pid++) {
    if ((fsa_valid_clause_set = fsaGs [pid]->fsa_valid_clause_set) != NULL) {
      set = sxba_calloc (fsaGs [pid]->clause_nb+1);
      sxba_copy (set, fsaGs [pid]->t2clause_set [0]);

      tok = 0;

      while ((tok = sxba_scan (source_set, tok)) >= 0) {
	sxba_or (set, fsaGs [pid]->t2clause_set [tok]);
      }
    
      sxba_and (fsa_valid_clause_set, set);
      sxfree (set);
    }
  }
}

#if MEASURES
static void
MEASURE_time (SXINT what)
{
#include <sys/time.h>
#include <sys/resource.h>

#define TIME_INIT	0
#define TIME_FINAL	1

  static struct rusage prev_usage, next_usage;

  if (what == TIME_INIT) {
    getrusage (RUSAGE_SELF, &prev_usage);
  }
  else {
    getrusage (RUSAGE_SELF, &next_usage);

    printf ("%i.%.3d ", (next_usage.ru_utime.tv_sec - prev_usage.ru_utime.tv_sec), (next_usage.ru_utime.tv_usec  - prev_usage.ru_utime.tv_usec) / 1000);

    prev_usage.ru_utime.tv_sec = next_usage.ru_utime.tv_sec;
    prev_usage.ru_utime.tv_usec = next_usage.ru_utime.tv_usec;
  }
}
#endif /* MEASURES */

static BOOLEAN
call_lfsa ()
{
  SXINT         i, tnb, tok;
  BOOLEAN     ret_val, pid;
  char	      str [164];
  struct fsaG *fsaG;

#if MEASURES
  MEASURE_time (TIME_INIT);
#endif /* MEASURES */

  lfsa_INTERFACE; /* _lfsa_if (), ... */

  ret_val = (*(lfsaGs [0]->process_fsa)) (lfsaGs [0]->init_state, 0);

  if (ret_val) {
    /* La passe lfsa a "reduit" le source, c'est celui la` qu'on va utiliser ds la suite */
    copy_out_source2source (lfsaGs);
  }

  if (is_print_time) {
    tnb = 0;

    for (i = 1; i <= n; i++)
      tnb += sxba_cardinal (glbl_source [i]);

#if EBUG2
    sprintf (str, "\tLFSA[t=%i/c=%i/ic=%i] (%s)",
	     tnb,
	     lfsaGs [0]->fsa_valid_clause_set ? sxba_cardinal (lfsaGs [0]->fsa_valid_clause_set) : 0,
	     instantiated_clause_nb,
	     ret_val ? "TRUE" : "FALSE");
#else
    sprintf (str, "\tLFSA[t=%i] (%s)",
	     tnb,
	     ret_val ? "TRUE" : "FALSE");
#endif

    sxtime (TIME_FINAL, str);
  }

  /* On stocke les structures construites par lfsa qui peuvent servir + tard */
  for (pid = 0; pid < PID; pid++) {
    fsaG = lfsaGs [pid];
    store_lfsaGs [pid] = *fsaG;
    /* On inhibe le free (eventuel) de ces structures */
    fsaG->Lex = NULL;
    fsaG->fsa_valid_clause_set = NULL;
    fsaG-> nt2lub = NULL;
    fsaG->clause2lub = NULL;
    /* On free le reste */
    (*(fsaG->final_fsa)) ();
  }

  instantiated_clause_nb = 0;

#if EBUG2
  total_clause_nb = total_clause1_nb = 0;
#endif

#if MEASURES
  MEASURE_time (TIME_FINAL);
#endif /* MEASURES */

  return ret_val;
}

#if is_rfsa
/* La passe lfsa est terminee avec succes */
void
rfsa_lexicalize (struct fsaG *G)
{
  SXBA        lfsa_valid_clause_set;

  if (!G->shallow) G->fsa_valid_clause_set = sxba_calloc (G->clause_nb+1);

  /* On utilise le fsa_valid_clause_set de lfsa ou son Lex a la rigueur */
  lfsa_valid_clause_set = store_lfsaGs [G->pid].fsa_valid_clause_set;

  if (lfsa_valid_clause_set == NULL) {
    lfsa_valid_clause_set = store_lfsaGs [G->pid].Lex;
    store_lfsaGs [G->pid].Lex = NULL; /* Pour le free */
  }
  else {
    store_lfsaGs [G->pid].fsa_valid_clause_set = NULL; /* Pour le free */
  }

#if EBUG4
  if (lfsa_valid_clause_set == NULL)
    sxtrap (ME, "rfsa_lexicalize");
#endif

  SXBA_1_bit (lfsa_valid_clause_set, 0); /* Valide les transitions vides */
  G->Lex = lfsa_valid_clause_set;
  /* Attention G->Lex du rfsa ne contient pas les loop_clauses alors que le
     G->Lex du lfsa les contenait!! */
}


static BOOLEAN
call_rfsa ()
{
  SXINT     i, tnb, pid;
  BOOLEAN ret_val;
  char	  str [164];
  SXBA    set;
  struct fsaG *fsaG;

  rfsa_INTERFACE; /* _rfsa_if (), ... */

  ret_val = (*(rfsaGs [0]->process_fsa)) (rfsaGs [0]->init_state, n);

  if (ret_val) {
    /* La passe rfsa a "reduit" le source, c'est celui la` qu'on va utiliser ds la suite */
    copy_out_source2source (rfsaGs);
  }

  if (is_print_time) {
    tnb = 0;

    for (i = 1; i <= n; i++)
      tnb += sxba_cardinal (glbl_source [i]);

#if EBUG2
    sprintf (str, "\tRFSA[t=%i/c=%i/ic=%i] (%s)",
	     tnb,
	     rfsaGs [0]->fsa_valid_clause_set ? sxba_cardinal (rfsaGs [0]->fsa_valid_clause_set) : 0,
	     instantiated_clause_nb,
	     ret_val ? "TRUE" : "FALSE");
#else
    sprintf (str, "\tRFSA[t=%i] (%s)",
	     tnb,
	     ret_val ? "TRUE" : "FALSE");
#endif

    sxtime (TIME_FINAL, str);
  }

  /* On stocke les structures construites par lfsa qui peuvent servir + tard */
  for (pid = 0; pid < PID; pid++) {
    fsaG = rfsaGs [pid];
    store_rfsaGs [pid] = *fsaG;
    /* On inhibe le free (eventuel) de ces structures */
    fsaG->Lex = NULL;
    fsaG->fsa_valid_clause_set = NULL;
    fsaG-> nt2lub = NULL;
    fsaG->clause2lub = NULL;
    /* On free le reste */
    (*(fsaG->final_fsa)) ();
  }

  instantiated_clause_nb = 0;

#if EBUG2
  total_clause_nb = total_clause1_nb = 0;
#endif

#if MEASURES
  MEASURE_time (TIME_FINAL);
#endif /* MEASURES */

  return ret_val;
}


/* On force la propriete lb <= ub */
static void
lub_washer ()
{
  SXINT           pid, clause, lb, ub;
  SXBA		valid_clause_set, *clause2lb, *clause2ub, lb_set, ub_set;

  for (pid = 0; pid < PID; pid++) {
    valid_clause_set = store_rfsaGs [pid].fsa_valid_clause_set;
    clause2lb = store_rfsaGs [pid].clause2lub;
    clause2ub = store_lfsaGs [pid].clause2lub;

    clause = 0;

    while ((clause = sxba_scan (valid_clause_set, clause)) > 0) {
      lb_set = clause2lb [clause];
      ub_set = clause2ub [clause];

      lb = sxba_scan (lb_set, -1);
      ub = sxba_1_rlscan (ub_set, n+1);
      /* Tous les index de
	 lb_set doivent etre inferieurs ou egaux a ub
	 ub_set doivent etre superieurs ou egaux a lb */

      while ((ub = sxba_scan_reset (lb_set, ub)) >= 0);

      while ((lb = sxba_1_rlscan (ub_set, lb)) >= 0)
	SXBA_0_bit (ub_set, lb);
    }
  }
}
#endif /* is_rfsa */
#endif /* is_lfsa */



#if is_earley_guide
/* is_supertagging, is_earley_shallow et is_coupled sont incompatibles */
/* ATTENTION : pour l'instant, traitement d'une grammaire unique !! (pas de modularite) */

/* 
   Principe de la generation 1RCG --> BNF
   Ni les clauses identiques ni les falses clauses de la 1RCG ne sont examinees.
   Les loop clauses ne sont pas examinees et ne produisent donc pas de prod.  Cependant, l'arg
      de earley2init_supertagger ou de earley2init_guide contient l'ensemble des loop clauses
      qui ont ete exclues par la verif des &Lex.  Donc une A_loop_clause l de la 1RCG (et ses clauses identique)
      est reconnue ssi une A_clause est reconnue et si l n'est pas ds l'ensemble des loop_clauses exclue.
   Si plusieurs clauses de la 1RCG ne different que par des predicats predefinis, elles ne produisent
      qu'une seule prod.  En d'autre termes, la clause argument de earley2guide ou de supertagger peut
      etre la tete d'une liste de clauses identiques (qui ne seront jamais fournies par Earley).  La clause
      argument est valide vis-a-vis des &Lex.
*/


extern BOOLEAN sxearley_parse_it ();

static SXINT Apq; /* Ds le cas fully_coupled, l'appelant earley2full_guide doit recuperer cette valeur... */
static BOOLEAN is_new_Apq; /* ... et il doit savoir si c'est la 1ere fois */

static X_header earley_rhoA_hd;
static SXBA *Aij2clause_id_set;
/* Pour le traitement particulier du debordement de Aij2clause_id_set */
static SXBA *earley_rhoA_hd_oflw_disp;
static SXINT earley_rhoA_hd_oflw_disp_size;
#if is_large
static X_header earley_range_1;
#endif

static SXINT earley_max_clause_id;
static SXINT *earley_clause2lhs_nt, *earley_clause2clause_id;
static SXINT lognt, logn_p_lognt;
static SXBA earley_invalid_lex_loop_clause;
static SXBA_ELT *earley_A2max_clause_id;

#ifdef INTERFACE2
static SXINT andk, logk, andn;
static struct G *Guided_Gs [PID];
static SXBA earley_valid_nt_set, valid_loop_nt_set, *nt2loop_clause_id_set;
#endif /* INTERFACE2 */

#if is_earley_shallow
/* CAS DU SHALLOW PARSER GUIDE' PAR DU EARLEY */
static SXINT shallow_guided_clausek_nb, shallow_Ak_nb;
static SXINT *shallow_kclauses_nb;
static SXBA shallow_guiding_loop_clause_set; /* shallow_guiding_loop_clause_set = Gs [0]->loop_clause_set; */
static SXBA shallow_invalid_lex_guiding_loop_clause, shallow_guiding_clause_set, shallow_guided_A_set;
static SXBA *guiding_clause2range_set;
static X_header shallow_Aij, shallow_ranges;
#endif /* is_earley_shallow */

#if is_supertagging
static SXBA supertagger_guiding_clause_set, supertagger_valid_guiding_nt_set, lexicalized_guided_clause_set;
static SXBA supertagger_valid_guided_nt_set, supertagger_invalid_lex_guiding_loop_clause, *supertagger_guided_clause2i_set;
static SXINT *supertagger_kclauses_nb;
static SXBA *supertagger2clause_set [PID];


/* PB : Soit une 2arg clause p (ayant donne ds la 1rcg 2 clauses p1 et p2) avec t1 ds le 1er arg et t2 ds le second. 
   t1 est reconnu par p1 a l'indice i et t2 par p2 a l'indice j (j > i).
   Doit-on associer p aux indices i et j ou seulement a l'un des 2 (j en l'occurrence) ? */

/* Si une guided_A_loop_clause est detectee sur l'indice i (on suppose qu'elle est non lexicalisee), on
   l'affecte a l'indice 0. */

/* Normalement, une RCG initiale ne devrait pas comporter de loop !! */

/* ATTENTION : ds le cas -1RCG, ni les boucles ni les clauses identiques de la grammaire d'origine ne
   sont traduites vers des clauses de la grammaire 1rcg.  Ca signifie qu'il faut les "reconstituer"
   a partir des guided_clauses, elles ne peuvent pas se deduire de leurs constituants qui n'ont pas ete generes */
/* 2 guiding_clause identiques ne peuvent pas permettre de reconstituer 2 guided_clause identiques. */
/* appele' depuis le reconnaisseur/reducer earley : la reconnaissance d'un token de l'indice i a ete fait
   par clause */
/* Si i==0 => guiding_clause est non lexicalisee */
/* Attention, supertagger est appele' depuis earley sur toutes les guiding_clause (valides (wrt &Lex) qui ont 
   permis de construire une prod de la bnf. */
void
supertagger (SXINT guiding_clause, SXINT i)
{
  SXINT guided_clause, arity, *p, bot, top, x, j, k, guided_idem_clause, guided_A, *gp, guided_loop_clause, guided_idem_loop_clause;
  SXINT guiding_A, guiding_loop_clause;
  BOOLEAN is_new_guiding_clause;
  SXBA i_set;

  is_new_guiding_clause = SXBA_bit_is_reset_set (supertagger_guiding_clause_set, guiding_clause);
  /* C'est la 1ere fois qu'on trouve guiding_clause */
  guided_clause = Guided_Gs [0]->guide_clause2clause [guiding_clause] & andk;

  if (i > 0)
    SXBA_1_bit (lexicalized_guided_clause_set, guided_clause);

  p = supertagger_kclauses_nb+guided_clause;
	  
  if (is_new_guiding_clause) {
    if (*p == 0) {
      /* C'est la 1ere fois qu'on trouve guided_clause */
      arity = Guided_Gs [0]->nt2arity [Guided_Gs [0]->clause2lhs_nt [guided_clause]];
      *p = arity-1; /* On a trouve le 1er arg */
    }
    else
      --*p; /* On a trouve un autre arg */

    if (*p == 0) {
      /* Si guided_clause est une A_clause, cette 1ere reconnaissance de guided_clause peut
	 permettre de valider les A_loop_clauses */
      if (Guided_Gs [0]->invalid_lex_loop_clause) {
	/* Il y a des loop ds la grammaire d'origine !!... */
	guided_A = Guided_Gs [0]->clause2lhs_nt [guided_clause];

	if (x = Guided_Gs [0]->looped_nt [guided_A]) {
	  /* ... sur guided_A... */
	  if (SXBA_bit_is_reset_set (supertagger_valid_guided_nt_set, guided_A)) {
	    /* ...et c'est la 1ere fois qu'on tombe sur guided_A */
	    gp = Guided_Gs [0]->looped_clause+x;
	
	    while (guided_loop_clause = *gp++) {
	      /* guided_loop_clause est une Aloop... */
	      if (!SXBA_bit_is_set (Guided_Gs [0]->invalid_lex_loop_clause, guided_loop_clause)) {
		/* ...valide vis-a-vis des &Lex */
		/* On la note a l'indice 0... */
		SXBA_1_bit (supertagger2clause_set [0] [0], guided_loop_clause);
		SXBA_1_bit (used_clause [0], guided_loop_clause);

		/* ...ainsi que ses boucles identiques */
		if (Guided_Gs [0]->clause2identical_disp != NULL &&
		    ((bot = Guided_Gs [0]->clause2identical_disp [guided_loop_clause]) <
		     (top = Guided_Gs [0]->clause2identical_disp [guided_loop_clause+1]))) {
		  /* Boucles identiques */
		  do {
		    guided_idem_loop_clause = Guided_Gs [0]->clause2identical [bot];
		    /* SXBA_1_bit (supertagger2clause_set [0] [0], guided_idem_loop_clause);
		       Les clauses identiques sont traitees a l'impression */
		    SXBA_1_bit (used_clause [0], guided_idem_loop_clause);
		  } while (++bot < top);
		}
	      }
	    }
	  }
	}
      }
    }
  }

  if (*p == 0) {
    /* Tout est calcule' */
    if (i > 0 || !SXBA_bit_is_set (lexicalized_guided_clause_set, guided_clause))
      /* On decide de ne pas associer a l'indice 0 une clause dont l'un des args produit une 1RCG non lexicalisee */
      SXBA_1_bit (supertagger2clause_set [0] [i], guided_clause);

    if (SXBA_bit_is_reset_set (used_clause [0], guided_clause)) {
      /* C'est la 1ere fois qu'on reconnait completement guided_clause */
      i_set = supertagger_guided_clause2i_set [guided_clause];
    
      if ((j = sxba_scan (i_set, 0)) > 0) {
	k = j;

	do {
	  SXBA_1_bit (supertagger2clause_set [0] [k], guided_clause);
	} while ((k = sxba_scan (i_set, k)) > 0);
      }
    }
    else
      j = 0;

    if (Guided_Gs [0]->clause2identical_disp != NULL &&
	((bot = Guided_Gs [0]->clause2identical_disp [guided_clause]) <
	 (top = Guided_Gs [0]->clause2identical_disp [guided_clause+1]))) {
      /* On traite les clauses identiques a guided_clause */
      do {
	guided_idem_clause = Guided_Gs [0]->clause2identical [bot];
	SXBA_1_bit (used_clause [0], guided_idem_clause);
    
#if 0
	/* Le 29/07/2002.  Les clauses identiques sont traitees a l'impression */
	if (i > 0 || !SXBA_bit_is_set (lexicalized_guided_clause_set, guided_clause))
	  /* On decide de ne pas associer a l'indice 0 une clause dont l'un des args produit une 1RCG non lexicalisee */
	  SXBA_1_bit (supertagger2clause_set [0] [i], guided_idem_clause);

	SXBA_1_bit (used_clause [0], guided_idem_clause);

	if (j > 0) {
	  k = j;

	  do {
	    SXBA_1_bit (supertagger2clause_set [0] [k], guided_idem_clause);
	  } while ((k = sxba_scan (i_set, k)) > 0);
	}
#endif /* 0 */
      } while (++bot < top);
    }
  }
  else {
    /* guided_clause n'est pas reconnu en entier */
    if (i > 0) {
      /* guiding_clause est lexicalisee */
      /* On note qu'a la 1ere reconnaissance complete de guided_clause, il faudra venir completer
	 le niveau i */
      SXBA_1_bit (supertagger_guided_clause2i_set [guided_clause], i);
    }
  }

  /* On traite ses clauses identiques a guiding_clause */
  if (Gs [0]->clause2identical_disp != NULL &&
      ((bot = Gs [0]->clause2identical_disp [guiding_clause]) <
       (top = Gs [0]->clause2identical_disp [guiding_clause+1]))) {
    /* On traite les clauses identiques a guiding_clause */
    do {
      supertagger (Gs [0]->clause2identical [bot], i);
    } while (++bot < top);
  }

  /* On traite aussi les loop eventuelles produites par la reconnaissance de guiding_clause car elles
     peuvent induire la reconnaissance de guided_clauses */
  if (supertagger_invalid_lex_guiding_loop_clause
      && !SXBA_bit_is_set (Gs [0]->loop_clause_set, guiding_clause)) {
    /* La bnf a des boucles... */
    /* ...et guiding_clause n'est pas une loop (on n'est pas ds un appel recursif) */
    guiding_A = Gs [0]->clause2lhs_nt [guiding_clause];

    if (SXBA_bit_is_reset_set (supertagger_valid_guiding_nt_set, guiding_A)) {
      /* ...et c'est la 1ere fois qu'on traite une A-clause */
      if (x = Gs [0]->looped_nt [guiding_A]) {
	/* A a des A_loop ... */
	gp = Gs [0]->looped_clause+x;
	
	while (guiding_loop_clause = *gp++) {
	  /* ... que l'on parcourt */
	  if (!SXBA_bit_is_set (supertagger_invalid_lex_guiding_loop_clause, guiding_loop_clause)) {
	    /* guiding_loop_clause est une A_loop dont les &Lex ont ete verifies */
	    /* On suppose que les loop sont non-lexicalisees */
	    supertagger (guiding_loop_clause, 0);

	    if (Gs [0]->clause2identical_disp != NULL &&
		((bot = Gs [0]->clause2identical_disp [guiding_loop_clause]) <
		 (top = Gs [0]->clause2identical_disp [guiding_loop_clause+1]))) {
	      /* Boucles identiques */
	      do {
		supertagger (Gs [0]->clause2identical [bot], 0);
	      } while (++bot < top);
	    }
	  }
	}
      }
    }
  }
}


/* Debut de l'analyse earley */
/* On remplit invalid_lex_loop_clause de la grammaire du guide */
void
earley2init_supertagger (SXBA invalid_lex_loop_clause)
{
  SXINT gclause, clause;

  if (invalid_lex_loop_clause) {
    supertagger_invalid_lex_guiding_loop_clause = invalid_lex_loop_clause;

    if (Guided_Gs [0]->invalid_lex_loop_clause != NULL) {
      gclause = 0;

      while ((gclause = sxba_scan (invalid_lex_loop_clause, gclause)) > 0) {
	clause = Guided_Gs [0]->guide_clause2clause [gclause] & andk;
	/* guided_invalid_lex_loop_clause est un sur-ensemble, il doit contenir toutes les boucles mais il peut
	   contenir autre chose... Il est donc inutile d'etre trop precis. */
	SXBA_1_bit (Guided_Gs [0]->invalid_lex_loop_clause, clause);
      }
    }
  }
}
#endif /* is_supertagging */



#if is_coupled || is_fully_coupled || is_earley_shallow
/* Debut de l'analyse earley */
void
earley2init_guide (SXBA invalid_lex_loop_clause)
{
  SXINT gclause, clause;
  SXBA guided_invalid_lex_loop_clause;

  if (invalid_lex_loop_clause) {
#if is_earley_shallow
    shallow_invalid_lex_guiding_loop_clause = invalid_lex_loop_clause;
#endif /* is_earley_shallow */

#ifdef INTERFACE2
    earley_valid_nt_set = sxba_calloc (Gs [0]->ntmax+1);
    valid_loop_nt_set = sxba_calloc (Gs [0]->ntmax+1);
    nt2loop_clause_id_set = sxbm_calloc (Gs [0]->ntmax+1, Gs [0]->max_clause_id+1);
    guided_invalid_lex_loop_clause = Guided_Gs [0]->invalid_lex_loop_clause;
#endif /* INTERFACE2 */

#ifdef INTERFACE_RAV2
    /* Ds ce cas il faut transformer l'ensemble des boucles de la version 2var en l'ensemble des
       boucles de la version "rav2", meme si la grammaire guidee n'a pas de boucle */
    earley_invalid_lex_loop_clause = sxba_calloc (Gs [0]->clause_nb+1);
    gclause = 0;

    while ((gclause = sxba_scan (invalid_lex_loop_clause, gclause)) > 0) {
      if ((clause = Gs [0]->from2var2clause [gclause]) > 0) {
	SXBA_1_bit (earley_invalid_lex_loop_clause, clause);

#ifdef INTERFACE2
	if (guided_invalid_lex_loop_clause != NULL) {
	  clause = Guided_Gs [0]->guide_clause2clause [clause] & andk;
	  SXBA_1_bit (guided_invalid_lex_loop_clause, clause);
	}
#endif /* INTERFACE2 */
      }
    }
#else /* !INTERFACE_RAV2 */
    earley_invalid_lex_loop_clause = invalid_lex_loop_clause;
#ifdef INTERFACE2
    if (guided_invalid_lex_loop_clause != NULL) {
      gclause = 0;

      while ((gclause = sxba_scan (invalid_lex_loop_clause, gclause)) > 0) {
	clause = Guided_Gs [0]->guide_clause2clause [gclause] & andk;
	/* guided_invalid_lex_loop_clause est un sur-ensemble, il doit contenir toutes les boucles mais il peut
	   contenir autre chose... Il est donc inutile d'etre trop precis. */
	SXBA_1_bit (guided_invalid_lex_loop_clause, clause);
      }
    }
#else /* !INTERFACE2 */
    sxba_copy (Gs [0]->invalid_lex_loop_clause, invalid_lex_loop_clause);
#endif /* !INTERFACE2 */
#endif  /* !INTERFACE_RAV2 */
  }
  else
    earley_invalid_lex_loop_clause = NULL;
}
#endif /* is_coupled || is_fully_coupled || is_earley_shallow */

#if is_earley_shallow
/* CAS DU SHALLOW PARSER GUIDE' PAR DU EARLEY */
/* Ds un premier temps, on suppose que la grammaire initiale n'a pas de boucle */
static void
out_shallow_clause (SXINT clause, SXINT A, SXBA *range_sets)
{
  SXINT bot, top, arity, arg, qp, p, q, range, idem_clause, len;
  SXBA range_set;
  static char blank_str [] = "                                                                                                                                ";

  len = 0;

  if (A == 0) {
    /* On sort le numero de clause */
    A = Guided_Gs [0]->clause2lhs_nt [clause];
    arity = Guided_Gs [0]->nt2arity [A];

    if (Guided_Gs [0]->clause2identical_disp != NULL) {
      bot = Guided_Gs [0]->clause2identical_disp [clause];
      top = Guided_Gs [0]->clause2identical_disp [clause+1];

      if (bot < top) {
	printf ("{%i", clause);

	do {
	  idem_clause = Guided_Gs [0]->clause2identical [bot];
	  printf (", %i", idem_clause);

	  if (arity > 1) {
	    len += 3;

	    if (idem_clause > 10) {
	      len++;
    
	      if (idem_clause > 100) {
		len++;
    
		if (idem_clause > 1000) {
		  len++;

		  if (idem_clause > 10000) {
		    len++;
		  }
		}
	      }
	    }
	  }
	} while (++bot < top);

	fputs ("}: ", stdout);
	len += 4; /* "{}: " */
      }
      else {
	printf ("%i: ", clause);
	len += 2;
      }
    }
    else {
      printf ("%i: ", clause);
      len += 2;
    }

    if (arity > 1) {
      len++;

      if (clause > 10) {
	len++;
    
	if (clause > 100) {
	  len++;
    
	  if (clause > 1000) {
	    len++;

	    if (clause > 10000) {
	      len++;
	    }
	  }
	}
      }
    }
  }
  else
    arity = Guided_Gs [0]->nt2arity [A];

  printf ("%s (", Guided_Gs [0]->nt2str [A]);

  
  if (arity > 1) {
    len += strlen (Guided_Gs [0]->nt2str [A])+1;
    blank_str [len] = NUL;
  }

  for (arg = 0; arg < arity; arg++) {
    if (arg > 0)
      printf ("\n%s,", blank_str);
      
    fputs ("{", stdout);

    range_set = range_sets [arg];

    qp = -1;
    range = X_top (shallow_ranges)+1;

    while ((range = sxba_1_rlscan (range_set, range)) > 0) {
      if (qp >= 0)
	fputs (", ", stdout);

      qp = X_X (shallow_ranges, range);
      p = qp & andn;
      q = qp >> logn;

      printf ("[%i..%i]", p, q);
    }
      
    fputs ("}", stdout);
  }

  fputs (")\n", stdout);

  if (arity > 1) {
    blank_str [len] = ' ';
  }
}

static void
out_shallow ()
{
  SXINT clause2ranges_top, Ak2ranges_top, guided_clause, A, arity, guiding_clause, guided_clausek;
  SXINT idem_clause, k, clausek, Ak, bot, top;
  SXINT *clause2clause0, *A2A0;
  SXBA *clausek2ranges, *Ak2ranges;

  if (forest_level & FL_lhs_clause) {
    /* shallow_guided_clausek_nb est le nb d'arg des guided_clauses valides */
    clausek2ranges = (SXBA*) sxcalloc (shallow_guided_clausek_nb+1, sizeof (SXBA));
    clause2clause0 = (SXINT*) sxalloc (Guided_Gs [0]->clause_nb+1, sizeof (SXINT));
    clause2ranges_top = 1;
  }
  else {
    /* shallow_Ak_nb est le nb d'arg des guided_Aclauses valides */
    Ak2ranges = (SXBA*) sxcalloc (shallow_Ak_nb+1, sizeof (SXBA));
    A2A0 = (SXINT*) sxcalloc (Guided_Gs [0]->ntmax+1, sizeof (SXINT));
    Ak2ranges_top = 1;
  }

  guided_clause = 0;

  while ((guided_clause = sxba_scan (used_clause [0], guided_clause)) > 0) {
    A = Guided_Gs [0]->clause2lhs_nt [guided_clause];
    arity = Guided_Gs [0]->nt2arity [A];

    if (forest_level & FL_lhs_clause) {
      clause2clause0 [guided_clause] = clause2ranges_top;
      clause2ranges_top += arity;
    }
    else {
      if (A2A0 [A] == 0) {
	A2A0 [A] = Ak2ranges_top;
	Ak2ranges_top += arity;
      }
    }
  }

  guiding_clause = 0;

  while ((guiding_clause = sxba_scan (shallow_guiding_clause_set, guiding_clause)) > 0) {
    guided_clausek = Guided_Gs [0]->guide_clause2clause [guiding_clause];
    guided_clause = guided_clausek & andk;

    if (SXBA_bit_is_set (used_clause [0], guided_clause)) {
      k = guided_clausek>>logk;

      if (forest_level & FL_lhs_clause) {
	clausek = clause2clause0 [guided_clause]+k;

	if (clausek2ranges [clausek] == NULL)
	  clausek2ranges [clausek] = guiding_clause2range_set [guiding_clause];
      }
      else {
	A = Guided_Gs [0]->clause2lhs_nt [guided_clause];
	Ak = A2A0 [A]+k;

	if (Ak2ranges [Ak] == NULL)
	  Ak2ranges [Ak] = guiding_clause2range_set [guiding_clause];
	else
	  sxba_or (Ak2ranges [Ak], guiding_clause2range_set [guiding_clause]);
      }
    }
  }

  if ((forest_level & FL_clause) && Guided_Gs [0]->clause2identical_disp != NULL)  {
    /* On complete used_clause avec les "idem" */
    guided_clause = 0;

    while ((guided_clause = sxba_scan (used_clause [0], guided_clause)) > 0) {
      if (SXBA_bit_is_set (Guided_Gs [0]->main_clause_set, guided_clause)) {
	bot = Guided_Gs [0]->clause2identical_disp [guided_clause];
	top = Guided_Gs [0]->clause2identical_disp [guided_clause+1];

	if (bot < top) {
	  do {
	    idem_clause = Guided_Gs [0]->clause2identical [bot];
	    SXBA_1_bit (used_clause [0], idem_clause);
	  } while (++bot < top);
	}
      }
    }
  }

  guided_clause = 0;

  while ((guided_clause = sxba_scan (used_clause [0], guided_clause)) > 0) {
    if (SXBA_bit_is_set (Guided_Gs [0]->main_clause_set, guided_clause)) {
      if (forest_level & FL_lhs_clause) {
	out_shallow_clause (guided_clause, 0, clausek2ranges+clause2clause0 [guided_clause]);
      }
      else {
	A = Guided_Gs [0]->clause2lhs_nt [guided_clause];

	if (A2A0 [A]) {
	  out_shallow_clause (0, A, Ak2ranges+A2A0 [A]);
	  A2A0 [A] = 0;
	}
      }
    }
  }
  
  if (forest_level & FL_lhs_clause) {
    /* shallow_guided_clausek_nb est le nb d'arg des guided_clauses valides */
    sxfree (clausek2ranges);
    sxfree (clause2clause0);
  }
  else {
    /* shallow_Ak_nb est le nb d'arg des guided_Aclauses valides */
    sxfree (Ak2ranges);
    sxfree (A2A0);
  }
}

void earley2guide (SXINT guiding_clause, SXBA lb_set, SXINT i, SXINT j);

static void
process_shallow_loop (SXINT x, SXINT range, SXINT i, SXINT j, SXINT guiding_A)
{
  SXINT Aij, *gp, guiding_loop_clause, bot, top;

  if (!X_set (&shallow_Aij, (range<<logn)+guiding_A, &Aij)) {
    /* C'est la premiere fois qu'on trouve Aij */
    gp = Gs [0]->looped_clause+x;
	
    while (guiding_loop_clause = *gp++) {
      /* ... que l'on parcourt */
      if (!SXBA_bit_is_set (shallow_invalid_lex_guiding_loop_clause, guiding_loop_clause)) {
	/* guiding_loop_clause est une A_loop dont les &Lex ont ete verifies */
	earley2guide (guiding_loop_clause, NULL, i, j);

	if (Gs [0]->clause2identical_disp != NULL &&
	    ((bot = Gs [0]->clause2identical_disp [guiding_loop_clause]) <
	     (top = Gs [0]->clause2identical_disp [guiding_loop_clause+1]))) {
	  /* Boucles identiques */
	  do {
	    earley2guide (Gs [0]->clause2identical [bot], NULL, i, j);
	  } while (++bot < top);
	}
      }
    }
  }
}


/* A chaque guiding clause, on associe l'ensemble de ses ranges valides.  
   On en profite pour calculer l'ensemble des guided_clauses valide ds used_clause [0] */
/* ATTENTION un range [p+1..q+1] du earley correspond au range [p..q] d'une RCG */
/* Cas SHALLOW */
void
earley2guide (SXINT guiding_clause, SXBA lb_set, SXINT i, SXINT j)
{
  SXINT q, x, k, range, guiding_A, bot, top, guided_clause, *p, arity, guided_A;
  SXBA range_set;

  range_set = guiding_clause2range_set [guiding_clause];

  /* Attention, on range le +grand index ds les forts poids */
  q = (j-1)<<logn;

  if (shallow_invalid_lex_guiding_loop_clause
      /* La bnf a des boucles... */
      && !SXBA_bit_is_set (shallow_guiding_loop_clause_set, guiding_clause)
      /* ...et guiding_clause n'est pas une loop (on n'est pas ds un appel recursif) */
      ) {
    x = Gs [0]->looped_nt [guiding_A = Gs [0]->clause2lhs_nt [guiding_clause]];
  }
  else
    x = 0;
  /* Si x != 0, guiding_A a des A_loop */

  if (lb_set) {
    k = 0;

    while ((k = sxba_scan (lb_set, k)) >= 0) {
      /* ranges par k croissants */
      X_set (&shallow_ranges, q+k-1, &range);
      SXBA_1_bit (range_set, range);

      if (x)
	process_shallow_loop (x, range, k, j, guiding_A);
    }
  }
  else {
    X_set (&shallow_ranges, q+i-1, &range);
    SXBA_1_bit (range_set, range);

    if (x)
      process_shallow_loop (x, range, i, j, guiding_A);
  }

  /* On traite ses clauses identiques a guiding_clause comme si elles provenaient de l'analyse Earley */
  if (Gs [0]->clause2identical_disp != NULL &&
      ((bot = Gs [0]->clause2identical_disp [guiding_clause]) <
       (top = Gs [0]->clause2identical_disp [guiding_clause+1]))) {
    do {
      earley2guide (Gs [0]->clause2identical [bot], lb_set, i, j);
    } while (++bot < top);
  }

  /* On remplit [guided_]used_clause */
  guided_clause = Guided_Gs [0]->guide_clause2clause [guiding_clause] & andk;

  if (SXBA_bit_is_set (Guided_Gs [0]->main_clause_set, guided_clause)) {
    /* mais on s'occupe seulement des "main" */
    p = shallow_kclauses_nb+guided_clause;

    if (SXBA_bit_is_reset_set (shallow_guiding_clause_set, guiding_clause)) {
      /* C'est la 1ere fois qu'on trouve guiding_clause */
      guided_A = Guided_Gs [0]->clause2lhs_nt [guided_clause];
      arity = Guided_Gs [0]->nt2arity [guided_A];

      if (*p == 0) {
	/* C'est la 1ere fois qu'on trouve guided_clause */
	*p = arity-1; /* On a trouve le 1er arg */
      }
      else
	--*p; /* On a trouve un autre arg */

      if (*p == 0) {
	/* guided_clause est valide */
	/* Les clauses identiques a guided_clause seront traitees lors de l'output du shallow parser */
	/* Les boucles eventuelles (normalement y-en a pas) induites par guided_clause seront aussi traitees
	   lors de l'output */
	SXBA_1_bit (used_clause [0], guided_clause);
	shallow_guided_clausek_nb += arity;

	if (SXBA_bit_is_reset_set (shallow_guided_A_set, guided_A))
	  shallow_Ak_nb += arity;
      }
    }
  }
}
#endif /* is_earley_shallow */



#if is_coupled || is_fully_coupled
static void
earley_rhoA_hd_oflw (SXINT old_size, SXINT new_size)
{
  /* On ne peut pas faire
     Aij2clause_id_set = sxbm_resize (Aij2clause_id_set, old_size+1, new_size+1, earley_max_clause_id);
     car les tailles des SXBA sont changees ... */
  /* Apres debordement, les SXBA de Aij2clause_id_set peuvent pointer ds des zones differentes (attention au free!!) */
  Aij2clause_id_set = (SXBA*) sxrealloc (Aij2clause_id_set, new_size+1, sizeof (SXBA));

  sxbm_line_resize (Aij2clause_id_set,
		    old_size, new_size, &earley_rhoA_hd_oflw_disp, &earley_rhoA_hd_oflw_disp_size, earley_max_clause_id);
}


/* Debut de l'analyse earley */
/* Cas COUPLED */

void
earley2guide (SXINT clause, SXBA lb_set, SXINT i, SXINT j)
{
  SXINT q, A, clause_id, pq, Aq;
  SXINT x, *p, cur, bot, top, loop_clause, guided_clause, guided_loop_clause, identical_clause_id, identical_guided_clause, guided_nt;
  SXBA_ELT max_clause_id;
  SXBA clause_id_set, loop_clause_id_set;

#ifdef INTERFACE_RAV2
  if ((clause = Gs [0]->from2var2clause [clause]) <= 0)
    return;
#endif /* INTERFACE_RAV2 */
  
  A = earley_clause2lhs_nt [clause];

#ifdef INTERFACE2
  guided_clause = Guided_Gs [0]->guide_clause2clause [clause] & andk;
  max_clause_id = Guided_Gs [0]->A2max_clause_id [guided_nt = Guided_Gs [0]->clause2lhs_nt [guided_clause]] + 1L;

  if (earley_invalid_lex_loop_clause) {
    /* Il y a des boucles ds la grammaire guidante et ... */
    /* ... earley_invalid_lex_loop_clause contient eventuellement les loop de la grammaire guidante
       qui ne peuvent pas etre valides a cause des "lex" */
    /* Cet ensemble peut etre utilise directement ds le cas d'une analyse 1rcg complete (il n'est pas
       necessaire de valider les Aloop clause par la reconnaissance d'une Aclause, ca sera fait
       naturellement par l'analyse 1rcg) */
    /* En revanche, si le reconnaisseur earley sert de guide pour une analyse kRCG (k > 1), les loop clauses
       doivent etre connues explicitement car elles doivent figurer ds les memoP */
    if (SXBA_bit_is_reset_set (earley_valid_nt_set, A)) {
      /* C'est la 1ere fois qu'on trouve une A-clause guidante valide */
      /* On calcule l'ensemble des id des Aloop (y compris les identiques) */
      /* Cet ensemble sera utilise sur chaque A-clause instanciee */
      if (x = Gs [0]->looped_nt [A]) {
	/* A a des Aloop ... */
	p = Gs [0]->looped_clause+x;
	
	while (loop_clause = *p++) {
	  /* ... que l'on parcourt */
	  if (!SXBA_bit_is_set (earley_invalid_lex_loop_clause, loop_clause)) {
	    /* loop_clause est une Aloop dont les &Lex ont ete verifies */
	    guided_loop_clause = Guided_Gs [0]->guide_clause2clause [loop_clause] & andk;

	    if (SXBA_bit_is_set (Guided_Gs [0]->main_clause_set, guided_loop_clause)) {
	      /* guided_loop_clause est une main clause, son clause_id doit etre memorise' */

	      if (SXBA_bit_is_reset_set (valid_loop_nt_set, A)) {
		/* On change la taille de loop_clause_id_set pour les unions futures !! */
		/* Attention, il faut prendre la taille ds le parseur guide' */
		/* nt2loop_clause_id_set [A] contiendra l'ensemble des clause_id des main clauses
		   de la grammaires guidee dont l'une des clauses composantes est une A-loop (compatible avec
		   les &Lex) de la grammaire guidante.  A est un nt de la grammaire guidante. */
		loop_clause_id_set = nt2loop_clause_id_set [A];
		loop_clause_id_set [0] = max_clause_id;
	      }

	      clause_id = Guided_Gs [0]->clause2clause_id [guided_loop_clause];
	      SXBA_1_bit (loop_clause_id_set, clause_id);

	      if (Gs [0]->clause2identical_disp != NULL &&
		  ((bot = Gs [0]->clause2identical_disp [loop_clause]) < (top = Gs [0]->clause2identical_disp [loop_clause+1]))) {
		/* Boucles identiques */
		do {
		  guided_loop_clause = Guided_Gs [0]->guide_clause2clause [Gs [0]->clause2identical [bot]] & andk;

		  if (SXBA_bit_is_set (Guided_Gs [0]->main_clause_set, guided_loop_clause)) {
		    clause_id = Guided_Gs [0]->clause2clause_id [guided_loop_clause];
		    SXBA_1_bit (loop_clause_id_set, clause_id);
		  }
		} while (++bot < top);
	      }
	    }
	  }
	}
      }
    }
  }

  /* On regarde si clause est le representant d'une classe d'equivalence de clauses identiques */
  if (Gs [0]->clause2identical_disp != NULL)
    bot = Gs [0]->clause2identical_disp [clause], top = Gs [0]->clause2identical_disp [clause+1];
  else
    bot = top = 0; 

  clause_id = Guided_Gs [0]->clause2clause_id [guided_clause];
  SXBA_1_bit (Guided_Gs [0]->nt2clause_id_set [guided_nt], clause_id); /* Au cas ou GUIDED4 n'est pas positionne */
#else /* ! INTERFACE2 */
  max_clause_id = earley_A2max_clause_id [A] + 1L;
  clause_id = earley_clause2clause_id [clause];
  SXBA_1_bit (Gs [0]->nt2clause_id_set [A], clause_id);
#endif /* ! INTERFACE2 */

  /* ATTENTION un range [p+1..q+1] du earley correspond au range [p..q] d'une RCG */
#if is_large
  q = j-1;
#else
  Aq = ((j-1)<<lognt)+A;
#endif

  if (lb_set) {
    i = j+1;

    while ((i = sxba_1_rlscan (lb_set, i)) >= 0) {
#if is_large
      X_set (&earley_range_1, MAKE_ij (i-1, q), &pq);
      is_new_Apq = !X_set (&earley_rhoA_hd, (pq<<lognt)+A, &Apq);
#else /* !is_large */
      is_new_Apq = !X_set (&earley_rhoA_hd, ((i-1)<<logn_p_lognt)+Aq, &Apq);
#endif /* !is_large */

      clause_id_set = Aij2clause_id_set [Apq];

      if (is_new_Apq) {
	/* Apq est nouveau */
	/* On change la taille de Aij2clause_id_set pour le sxba_scan !! */
	clause_id_set [0] = max_clause_id;
#ifdef INTERFACE2
	/* Pour chaque Aij, il faut faire l'union entre Aij2clause_id_set [Aij] et nt2loop_clause_id_set [A] */
	if (SXBA_bit_is_set (valid_loop_nt_set, A))
	  sxba_or (clause_id_set, nt2loop_clause_id_set [A]);
#endif /* INTERFACE2 */
      }

#ifdef INTERFACE2
      if (SXBA_bit_is_set (Guided_Gs [0]->main_clause_set, guided_clause))
	/* Cette clause provient d'une main clause, il faut noter son clause_id */
	SXBA_1_bit (clause_id_set, clause_id);

      if (bot < top) {
	/* On ajoute les clauses identiques */
	cur = bot;

	do {
	  identical_guided_clause = Guided_Gs [0]->guide_clause2clause [Gs [0]->clause2identical [cur]] & andk;

	  if (SXBA_bit_is_set (Guided_Gs [0]->main_clause_set, identical_guided_clause)) {
	    /* Cette clause provient d'une main clause, il faut noter son clause_id */
	    identical_clause_id = Guided_Gs [0]->clause2clause_id [identical_guided_clause];
	    SXBA_1_bit (clause_id_set, identical_clause_id);
	  }
	} while (++cur < top);
      }
#else /* !INTERFACE2 */
      SXBA_1_bit (clause_id_set, clause_id);
#endif /* !INTERFACE2 */
    }
  }
  else {
#if is_large
    X_set (&earley_range_1, MAKE_ij (i-1, q), &pq);
    is_new_Apq = !X_set (&earley_rhoA_hd, (pq<<lognt)+A, &Apq);
#else /* !is_large */
    is_new_Apq = !X_set (&earley_rhoA_hd, ((i-1)<<logn_p_lognt)+Aq, &Apq);
#endif /* !is_large */

    clause_id_set = Aij2clause_id_set [Apq];

    if (is_new_Apq) {
      /* Apq est nouveau */
      /* On change la taille de Aij2clause_id_set pour le sxba_scan !! */
      clause_id_set [0] = max_clause_id;
#ifdef INTERFACE2
      /* Pour chaque Aij, il faut faire l'union entre Aij2clause_id_set [Aij] et nt2loop_clause_id_set [A] */
      if (SXBA_bit_is_set (valid_loop_nt_set, A))
	sxba_or (clause_id_set, nt2loop_clause_id_set [A]);
#endif /* INTERFACE2 */
    }

#ifdef INTERFACE2	
    if (SXBA_bit_is_set (Guided_Gs [0]->main_clause_set, guided_clause))
      /* Cette clause provient d'une main clause, il faut noter son clause_id */
      SXBA_1_bit (clause_id_set, clause_id);

    if (bot < top) {
      /* On ajoute les clauses identiques */
      cur = bot;

      do {
	  identical_guided_clause = Guided_Gs [0]->guide_clause2clause [Gs [0]->clause2identical [cur]] & andk;

	  if (SXBA_bit_is_set (Guided_Gs [0]->main_clause_set, identical_guided_clause)) {
	    /* Cette clause provient d'une main clause, il faut noter son clause_id */
	    identical_clause_id = Guided_Gs [0]->clause2clause_id [identical_guided_clause];
	    SXBA_1_bit (clause_id_set, identical_clause_id);
	  }
      } while (++cur < top);
    }
#else /* !INTERFACE2 */
      SXBA_1_bit (clause_id_set, clause_id);
#endif /* !INTERFACE2 */
  }
}
#endif /* is_coupled || is_fully_coupled */

#if is_fully_coupled
void
earley2full_guide (SXINT clause, SXINT son_nb, SXINT *lb, SXINT *ub, SXINT t_nb)
{
  SXINT             *pto, son, lhs, A, x, *p, pq, loop_clause, bot, top, node_nb, identical_clause, identical_lhs;
  BOOLEAN	  old_lhs;
  struct spf_node *node_ptr;

  /* L'arg t_nb a ete ajoute' le 28/05/2003.  Pour chaque terminal de la production (le plus a gauche
   est repere par i=1, le + a droite par i=t_nb) ub[son_nb+i] donne sa position ds le source */

  /* On commence par calculer/stocker les Aij, memoP comme pour un guidage earley normal */
  earley2guide (clause, NULL, lb [0], ub [0]);
  /* Il a calcule Apq (static) */

  if ((node_nb = son_nb + t_nb) > 0) {
    pto = &((node_ptr = book_area (&rhs_guide_area_hd, node_nb))->rho [0]);

    /* l'ordre est correct ds les lb/olb */
    for (son = 1; son <= son_nb; son++) {
      *pto++ = MAKE_ij (lb [son]-1, ub [son]-1);
    }

    for (; son <= node_nb; son++) {
      /* On met les indices des terminaux */
      *pto++ = ub [son]-1;
    }
  }
  else {
    node_ptr = NULL;
  }

  old_lhs = XxY_set (&(XxY_lhs_guide [0]), Apq, clause, &lhs);

  if (node_ptr != NULL)
    node_ptr->next = old_lhs ? XxY_lhs_guide2rhs [0] [lhs] : NULL;

  XxY_lhs_guide2rhs [0] [lhs] = node_ptr;

  /* Ajoute' le 03/06/2003 */
  /* On regarde si clause est le representant d'une classe d'equivalence de clauses identiques */
  if (Gs [0]->clause2identical_disp != NULL &&
      (bot = Gs [0]->clause2identical_disp [clause]) < (top = Gs [0]->clause2identical_disp [clause+1])) {
    do {
      identical_clause = Gs [0]->clause2identical [bot];
      XxY_set (&(XxY_lhs_guide [0]), Apq, identical_clause, &identical_lhs);
      /* On partage les rhs instanciees entre clauses identiques */
      XxY_lhs_guide2rhs [0] [identical_lhs] = node_ptr;
    } while (++bot < top);
  }

  if (earley_invalid_lex_loop_clause && is_new_Apq) {
    /* Il y a des boucles ds la grammaire guidante et ... */
    /* ... c'est la 1ere fois qu'on trouve Apq */
    A = earley_clause2lhs_nt [clause];

    if (x = Gs [0]->looped_nt [A]) {
      /* A a des A-loop ... */
      pq = MAKE_ij (lb [0]-1, ub [0]-1);
      p = Gs [0]->looped_clause+x;
	
      while (loop_clause = *p++) {
	/* ... que l'on parcourt */
	if (!SXBA_bit_is_set (earley_invalid_lex_loop_clause, loop_clause)) {
	  /* loop_clause est une A_loop dont les &Lex ont ete verifies */
	  (node_ptr = book_area (&rhs_guide_area_hd, 1))->rho [0] = pq;
	  node_ptr->next = NULL;
	  XxY_set (&(XxY_lhs_guide [0]), Apq, loop_clause, &lhs);
	  XxY_lhs_guide2rhs [0] [lhs] = node_ptr;

	  if (Gs [0]->clause2identical_disp != NULL &&
	      ((bot = Gs [0]->clause2identical_disp [loop_clause]) < (top = Gs [0]->clause2identical_disp [loop_clause+1]))) {
	    /* Boucles identiques */
	    do {
	      loop_clause = Gs [0]->clause2identical [bot];
	      (node_ptr = book_area (&rhs_guide_area_hd, 1))->rho [0] = pq;
	      node_ptr->next = NULL;
	      XxY_set (&(XxY_lhs_guide [0]), Apq, loop_clause, &lhs);
	      XxY_lhs_guide2rhs [0] [lhs] = node_ptr;
	    } while (++bot < top);
	  }
	}
      }
    }
  }
}
#endif /* is_fully_coupled */



static BOOLEAN
call_earley_guide ()
{
  SXINT clause, A, ij, l;
  BOOLEAN ret_val;

#if PID>1
  sxtrap (ME, "call_earley_guide");
#endif

#ifdef INTERFACE2
  global_pid = 0;
  INTERFACE2;
  Guided_Gs [0] = Gs [0];
  earley_clause2clause_id = Guided_Gs [0]->clause2clause_id;
  logk = sxlast_bit (Guided_Gs [0]->clause_nb);
  andk = (1<<logk)-1;
  andn = (1<<logn)-1;
#endif /* INTERFACE2 */


#if is_supertagging || is_earley_shallow
#ifndef INTERFACE
  sxtrap (ME, "supertagger");
#endif /* INTERFACE */

#ifndef INTERFACE2
  sxtrap (ME, "supertagger");
#endif /* INTERFACE2 */

#ifdef INTERFACE_RAV2
  /* Pour l'instant */
  sxtrap (ME, "supertagger");
#endif /* INTERFACE_RAV2 */

#ifdef INTERFACE2_RAV2
  /* Pour l'instant */
  sxtrap (ME, "supertagger");
#endif /* INTERFACE2_RAV2 */

  global_pid = 0;
  INTERFACE;
#endif /* is_supertagging || is_earley_shallow */

#if is_supertagging
  supertagger_kclauses_nb = (SXINT*) sxcalloc (Guided_Gs [0]->clause_nb+1, sizeof (SXINT));
  supertagger_guiding_clause_set = sxba_calloc (Guided_Gs [0]->guide_clause_nb+1);
  supertagger2clause_set [0] = sxbm_calloc (n+1, Guided_Gs [0]->clause_nb+1);
  supertagger_valid_guiding_nt_set = sxba_calloc (Gs [0]->ntmax+1);
  lexicalized_guided_clause_set = sxba_calloc (Guided_Gs [0]->clause_nb+1);
  used_clause [0] = sxba_calloc (Guided_Gs [0]->clause_nb+1);
  supertagger_valid_guided_nt_set = sxba_calloc (Guided_Gs [0]->ntmax+1);
  supertagger_guided_clause2i_set = sxbm_calloc (Guided_Gs [0]->clause_nb+1, n+1);
#endif /* is_supertagging */

#if is_earley_shallow
  shallow_kclauses_nb = (SXINT*) sxcalloc (Guided_Gs [0]->clause_nb+1, sizeof (SXINT));
  shallow_guiding_loop_clause_set = Gs [0]->loop_clause_set;
  shallow_guiding_clause_set = sxba_calloc (Gs [0]->clause_nb+1);
  shallow_guided_A_set = sxba_calloc (Guided_Gs [0]->ntmax+1);
  used_clause [0] = sxba_calloc (Guided_Gs [0]->clause_nb+1);

  /* On commence avec une taille de (n+1)*(n+1), ca evite les oflw pour guiding_clause2range_set
     Si trop gros on peut le changer... */
  X_alloc (&shallow_ranges, "shallow_ranges", (n+1)*(n+1), 1, NULL, statistics);
  guiding_clause2range_set = sxbm_calloc (Gs [0]->clause_nb+1, X_size (shallow_ranges)+1);
  X_alloc (&shallow_Aij, "shallow_Aij", (n+1)*(n+1), 1, NULL, statistics);
#endif /* is_earley_shallow */

#if is_coupled || is_fully_coupled
  /* Le reconnaisseur earley pour G sert a guider :
     - soit G (cas du parseur CF complet)
     - soit une kRCG (k > 1) */
  global_pid = 0;
#ifdef INTERFACE_RAV2
  /* Ds le cas earley_guide, si INTERFACE_RAV2, la grammaire 1RCG en forme 2var ne sert a rien!! */
  INTERFACE_RAV2;
#else /* !INTERFACE_RAV2 */
  /* On suppose qu'on a -DINTERFACE=... */
  INTERFACE;
#endif /* !INTERFACE_RAV2 */
  /* Gs [0]->Gs = &(Gs [0]); */

  l = (n+1)*(n+2); if (l > 4096) l = 4096;
  X_alloc (&earley_rhoA_hd, "earley_rhoA_hd", l, 1, earley_rhoA_hd_oflw, statistics);
  l = X_size (earley_rhoA_hd);

#if is_large
  X_alloc (&earley_range_1, "earley_range_1", l/2, 1, NULL, statistics);
#endif

#ifdef INTERFACE2
  earley_max_clause_id = Guided_Gs [0]->max_clause_id+1;
#else /* !INTERFACE2 */
  earley_max_clause_id = Gs [0]->max_clause_id+1;
#endif /* !INTERFACE2 */

  Aij2clause_id_set = sxbm_calloc (l+1, earley_max_clause_id);

  lognt = Gs [0]->lognt;
  logn_p_lognt = logn+lognt;
  earley_clause2lhs_nt = Gs [0]->clause2lhs_nt;
  earley_A2max_clause_id = Gs [0]->A2max_clause_id;
#ifndef INTERFACE2
  earley_clause2clause_id = Gs [0]->clause2clause_id;
#endif /* !INTERFACE2 */
#endif /* is_coupled || is_fully_coupled */

#if is_fully_coupled
  rhs_guide_area_hd.size = n*n*sizeof (struct spf_node); /* Pourquoi pas */
  allocate_area (&rhs_guide_area_hd);
  XxY_alloc (&(XxY_lhs_guide [0]), "XxY_lhs_guide", l, 1, 0, 0, XxY_lhs_guide_oflw, statistics);
  XxY_lhs_guide2rhs [0] = (struct spf_node**) sxalloc (XxY_size (XxY_lhs_guide [0])+1, sizeof (struct spf_node*));
  XxY_lhs_guide2rhs [0] [0] = NULL;
#endif /* is_fully_coupled */

  ret_val = sxearley_parse_it ();

#if is_coupled || is_fully_coupled
#ifdef INTERFACE_RAV2
  if (earley_invalid_lex_loop_clause)
    sxfree (earley_invalid_lex_loop_clause), earley_invalid_lex_loop_clause = NULL;
#endif  /* INTERFACE_RAV2 */

  if (ret_val) {
#if is_rav22
	/* On commence par appeler l'interface des parsers d'origine dont on va memoriser les grammaires */
	global_pid = 0;
	INTERFACE2_RAV2;
	Rav2_Gs [0] = Gs [0];
	Rav2_Gs [0]->Gs = &(Rav2_Gs [0]);
#endif /* is_rav22 */

    /* On rend les structures du guide earley accessibles depuis le parseur guide' */
#ifdef INTERFACE2
    /* C'est lui qui va etre lance */
    Gs [0] = Guided_Gs [0];
#endif /* INTERFACE2 */

    Gs [0]->gpmemoP = Aij2clause_id_set;
#if is_large
    Gs [0]->gprange_1 = &earley_range_1;
#endif
    Gs [0]->gprhoA_hd = &earley_rhoA_hd;

    /* On lance les allocations/initialisations */
    (*(Gs [0]->parse.first_pass_init)) ();
    /* On inhibe les appels ulterieurs */
    Gs [0]->parse.first_pass_init = sxvoid;

    /* l'analyse earley a marche, on lance donc une analyse non robuste */
    is_robust_run = FALSE;

    ij = MAKE_ij (0, n);
#if is_large
    ij = X_is_set (&earley_range_1, ij);
#endif /* is_large */

    axiom_0_n = S0n = X_is_set (&earley_rhoA_hd, (ij<<lognt)+1);

    /* C'est parti ... */
#ifdef INTERFACE2    
    ret_val = rcgparse_it (n, 3); /* VERIFIER 3 ?? */
#else   
    ret_val = rcgparse_it (n, 1);
#endif

    if (!ret_val) {
#ifdef INTERFACE2
      /* A FAIRE : appeler une analyse robuste */
#else /* !INTERFACE2 */
      /* Si earley a marche, l'analyse 1RCG doit marcher aussi !! */
      sxtrap (ME, "call_earley_guide");
#endif /* !INTERFACE2 */
    }
  }
  else {
    /* Ici, le source est obligatoirement errone', on lance directement une analyse robuste (si elle existe) */
    if ((Gs [0])->robust && perform_robust_pass (1)) {
      /* Cette analyse est non guidee */
      /* On fait la lexicalization normale */
      is_robust_run = TRUE;
      ret_val = rcgparse_it (n, 1);
    }
  }
      
#ifdef INTERFACE2
  if (earley_valid_nt_set) {
    sxfree (earley_valid_nt_set), earley_valid_nt_set = NULL;
    sxfree (valid_loop_nt_set), valid_loop_nt_set = NULL;
    sxbm_free (nt2loop_clause_id_set), nt2loop_clause_id_set = NULL;
  }
#endif /* INTERFACE2 */

  /* Les free des Aij2clause_id_set, earley_range_1 et earley_rhoA_hd sont fait ds le parseur */
  if (earley_rhoA_hd_oflw_disp) {
    /* On libere les SXBA alloues sur oflow ... */
    sxbm_line_free (earley_rhoA_hd_oflw_disp, earley_rhoA_hd_oflw_disp_size), earley_rhoA_hd_oflw_disp = NULL;
  }

  Aij2clause_id_set = NULL; /* Pour ici */
#endif /* is_coupled || is_fully_coupled */

#if is_supertagging
  sxfree (supertagger_kclauses_nb), supertagger_kclauses_nb = NULL;
  sxfree (supertagger_guiding_clause_set), supertagger_guiding_clause_set = NULL;
  sxfree (supertagger_valid_guiding_nt_set), supertagger_valid_guiding_nt_set = NULL;
  sxfree (lexicalized_guided_clause_set), lexicalized_guided_clause_set = NULL;
  sxfree (supertagger_valid_guided_nt_set), supertagger_valid_guided_nt_set = NULL;
  sxbm_free (supertagger_guided_clause2i_set), supertagger_guided_clause2i_set = NULL;

  if (is_print_time) {
    SXINT i, t_nb = 0;
    char mess [32];

    for (i = 1; i <= n; i++) {
      t_nb += sxba_cardinal (glbl_out_source [i]);
    }
    
    sprintf (mess, "\tSupertagger[%i/%i]", n, t_nb);
    sxtime (TIME_FINAL, mess);
  }

  /* On sort le supertag... */
  out_sdag (Guided_Gs, glbl_out_source, supertagger2clause_set, 1);
  /* ... et l'ensemble des clauses (non lexicalisees) */
      
  if (forest_level & FL_clause)
    print_used_clause (Guided_Gs);

  sxfree (used_clause [0]), used_clause [0] = NULL;
  sxbm_free (supertagger2clause_set [0]), supertagger2clause_set [0] = NULL;
#endif /* is_supertagging */

#if is_earley_shallow
  sxfree (shallow_kclauses_nb), shallow_kclauses_nb = NULL;
  shallow_guiding_loop_clause_set = NULL;
  X_free (&shallow_Aij);
  sxfree (shallow_guided_A_set), shallow_guided_A_set = NULL;

  out_shallow ();
      
  if (forest_level & FL_clause)
    print_used_clause (Guided_Gs);

  sxfree (shallow_guiding_clause_set), shallow_guiding_clause_set = NULL;
  X_free (&shallow_ranges);
  sxbm_free (guiding_clause2range_set), guiding_clause2range_set = NULL;
  sxfree (used_clause [0]), used_clause [0] = NULL;
#endif /* is_earley_shallow */

#if is_fully_coupled
  free_area (&rhs_guide_area_hd);
  sxfree (XxY_lhs_guide2rhs [0]), XxY_lhs_guide2rhs [0] = NULL;
  XxY_free (&(XxY_lhs_guide [0]));
#endif /* is_fully_coupled */
    
  return ret_val;
}
#endif /* is_earley_guide */

static VOID	gripe ()
{
    fputs ("\nA function of \"sdag\" is out of date with respect to its specification.\n", sxstderr);
    sxexit(1);
}

SXINT sdag_semact (SXINT code, SXINT numact)
{
  static SXINT	tok_no, tmax, t_nb;
  SXINT		lahead, tok, pid, ste, t, size;
  SXINT           *p, i, top;
  BOOLEAN	ret_val, needs_lub;
  char		*str, mess [32];
  SXBA		line;
  SXBA		valid_clause_set, new_valid_clause_set;
  SXBA          *clause2lb, *clause2ub;
  struct G      *pG, *gG;
  SXINT           gclause, clause, arity, main_clause, cur, d;
  SXINT           A, logk, andk, pos, clausek, gA, Ak, clause0, Ad, gd, Acur; 
  SXINT           *clause2clause0, *new_clause2clause0, *A2A0, *new_A2A0;
  SXBA          *clause02ub, *new_clause02ub, *clause02lb, *new_clause02lb;
  SXBA          *A02ub, *new_A02ub, *A02lb, *new_A02lb;
  SXBA          *nt2lb, *nt2ub, *new_clause2lb, *new_clause2ub, *new_nt2lb, *new_nt2ub, valid_nt_set, new_valid_nt_set; 

  static struct G *gGs [PID];

  switch (code) {
  case OPEN:
  case CLOSE:
  case SEMPASS:
  case ERROR:
    return 0;
  case INIT:
    if (is_print_time)
      sxtime (INIT, NULL);

    t_nb = 0;

#if !is_parse_forest
    forest_level = 0;
#endif
	
    n = 0;
    tok_no = 1;

    do {
      (*(sxplocals.SXP_tables.scanit)) ();
      lahead = SXGET_TOKEN (sxplocals.Mtok_no).lahead;

      if (lahead == sxsvar.SXS_tables.S_termax-1) {
	/* C'est le ")" qui marque la fin la fin de l'association forme flechie -> terminaux */
	n++;
      }
	    
    } while (lahead != sxsvar.SXS_tables.S_termax);

    tmax = sxsvar.SXS_tables.S_termax - 4; /* %MOT, "(", ")", EOF */

#if is_sdag
    /* Pour ressortir le sdag reduit */
    ff_ste = (SXINT *) sxalloc (n+2, sizeof (SXINT));
#endif

    /* Le 23/5/02 ESSAI.  Le code de eof est (aussi) passe a 0. 
       Ca permet de faire marcher les &First et &Last car eof est code' par 0 ds les first_%i_%i */
    top = (n+2)*2;

    glbl_source_area = glbl_source = sxbm_calloc (top, tmax+2);
    SXBA_1_bit (glbl_source [0], 0);
    SXBA_1_bit (glbl_source [0], tmax+1);
    SXBA_1_bit (glbl_source [n+1], 0);
    SXBA_1_bit (glbl_source [n+1], tmax+1);

#if MEASURES
    unknown_word_set = sxba_calloc (n+1);
#endif /* MEASURES */

#if !is_fully_coupled || is_earley_guide
    /* A faire ds le cas is_fully_coupled.  Pour l'instant les index des terminaux ne sont
       pas mis ds la rho stack */
    glbl_out_source = glbl_source+n+2;
#endif /* !is_fully_coupled || is_earley_guide */
	
    source_set = sxba_calloc (tmax+1);
    source_top = source_set + NBLONGS (BASIZE (source_set)); /* pointe vers la derniere "tranche" */

    rcvr_n = n+1;		/* pointe sur eof */

    logn = sxlast_bit (n);
    ANDj = (1<<logn)-1;

    return 0;
  case FINAL:
    /* glbl_source est rempli, on peut lancer les RCG!! */

    /* Ce programme doit etre compile avec l'option
       -DPID=p -DINTERFACE="_g1_if(),...,_gp_if()"
       S'il y a p sous-parser de nom g1, ..., gp (les grammaires s'appellent g1.rcg, ..., gp.rcg
    */
#if PID>1
    {
      SXINT	pid;

      gtmax = tmax+1;
#if 0
      /* Le 26/05/04 str_tbl est remplace par t_names_ptr qui (va) pointer vers le sxword_header
	 du module principal qui contient tous les terminaux */
      SXUINT	ste;
      SXINT		glahead;
      extern char	*malloc (), *calloc (), *realloc ();
      extern void free ();

      sxword_alloc (&str_tbl, "str_tbl", gtmax+1, 1, 10,
		  malloc, calloc, realloc, NULL /* resize */, free, NULL, NULL);
#endif /* 0 */
      g2l = (SXINT **) sxalloc (PID+1, sizeof (SXINT*));
      l2g = (SXINT **) sxalloc (PID+1, sizeof (SXINT*));
      gl_area = (SXINT *) sxcalloc (2*PID*gtmax, sizeof (SXINT));

      g2l [0] = l2g [0] = NULL;
      g2l [1] = gl_area;
      l2g [1] = gl_area + PID*gtmax;

      for (pid = 2; pid <= PID; pid++) {
	g2l [pid] = g2l [pid-1] + gtmax;
	l2g [pid] = l2g [pid-1] + gtmax;
      }

#if 0
      /* Le 26/05/04 voir commentaire ci_dessous (t_names est initialise' statiquement) */
      for (glahead = 1; glahead < gtmax; glahead++) {
	ste = sxword_save (&str_tbl, sxttext (sxplocals.sxtables, glahead));
#if EBUG4
	if (ste != glahead+1)
	  sxtrap (ME, "sdag_semact$final");
#endif
      }
#endif /* 0 */
    }
#endif /* PID>1 */

    if (is_print_time) {
      sprintf (mess, "\tScanner[%i/%i]", n, t_nb);
      sxtime (TIME_FINAL, mess);
    }

#if MEASURES
    /* On est ds une campagne de mesures ... */
    printf ("%i %i ", n, t_nb);
#endif /* MEASURES */

#if is_earley_guide
    ret_val = call_earley_guide ();
#else                                                               /* !IS_EARLEY_GUIDE */

#if is_lfsa
    ret_val = call_lfsa ();

#if is_rfsa
    if (ret_val) {
      ret_val = call_rfsa ();

      if (ret_val && (forest_level & (~FL_source)) && store_rfsaGs [0].clause2lub)
	lub_washer ();
    }
#endif /* is_rfsa */
#endif /* !is_lfsa */

#if is_lfsa
#ifndef INTERFACE
#if is_parse_forest
#ifdef INTERFACE_1_RCG
    if (ret_val && (forest_level & (~FL_source)) && store_rfsaGs [0].clause2lub) {
      /* Les sorties autres que le source necessite la connaissance des lb et ub (is_rfsa) et bien sur
	 que les LRFSA_parseurs se soient executes avec -DSEMANTICS */
      /* Ds ce cas les lub des nt ont aussi ete construits */
      /* On va mapper les clauses de fsa_valid_clause_set vers la grammaire initiale */
      /* Il faut commencer par completer les fsa_valid_clause_set par les "loop" et les clauses identiques */
      /* Il faut donc avoir acces a la grammaire 1_RCG par l'interface -DINTERFACE_1_RCG */

      needs_lub = forest_level & ~(FL_clause+FL_source); /* On a besoin des lub */
      global_pid = 0;
      INTERFACE_1_RCG;

      for (pid = 0; pid < PID; pid++) {
	pG = Gs [pid];

	used_clause [pid] = store_rfsaGs [pid].fsa_valid_clause_set;
	store_rfsaGs [pid].fsa_valid_clause_set = NULL;

	if (needs_lub) {
	  new_valid_clause_set = used_clause [pid];

	  clause2lb = store_rfsaGs [pid].clause2lub;
	  clause2ub = store_lfsaGs [pid].clause2lub;

	  clause2clause0 = clause2clause0s [pid] = (SXINT*) sxalloc (pG->clause_nb+1, sizeof (SXINT)), clause2clause0 [0] = 0;
	  clause02ub = clause02ubs [pid] = (SXBA*) sxalloc (pG->clause_nb+1, sizeof (SXBA));
	  clause02lb = clause02lbs [pid] = (SXBA*) sxalloc (pG->clause_nb+1, sizeof (SXBA));

	  nt2ub = store_lfsaGs [pid].nt2lub;
	  nt2lb = store_rfsaGs [pid].nt2lub;

	  new_valid_nt_set = valid_nt_sets [pid] = sxba_calloc (pG->ntmax+1);

	  A2A0 = A2A0s [pid] = (SXINT*) sxalloc (pG->ntmax+1, sizeof (SXINT)), A2A0 [0] = 0;
	  A02ub = A02ubs [pid] = (SXBA*) sxalloc (pG->ntmax+1, sizeof (SXBA));
	  A02lb = A02lbs [pid] = (SXBA*) sxalloc (pG->ntmax+1, sizeof (SXBA));

	  clause = 0;

	  while ((clause = sxba_scan (new_valid_clause_set, clause)) > 0) {
	    cur = clause2clause0 [clause] = ++*clause2clause0;
	    clause02ub [cur] = clause2ub [clause];
	    clause02lb [cur] = clause2lb [clause];
	    A = pG->clause2lhs_nt [clause]; /* arity (A) == 1 */

	    if (SXBA_bit_is_reset_set (new_valid_nt_set, A)) {
	      cur = A2A0 [A] = ++*A2A0;
	      A02ub [cur] = nt2ub [A];
	      A02lb [cur] = nt2lb [A];
	    }
	  }
	}
      }

#if is_rav2
#if 0
      /* Non, les loop de l'INTERFACE_RAV2 ne doivent pas (aussi) etre trouvees par l'analyse de la
	 grammaire 2var */
      for (pid = 0; pid < PID; pid++) {
	pG = Gs [pid];

	/* Il faut completer les valid_clause_set par les "loop" ... */
	if (pG->looped_nt)
	  add_loop_clauses (pG, needs_lub);

	/* ... et les clauses identiques */
	if (pG->clause2identical_disp != NULL)
	  add_identical_clauses (pG);
      }
#endif

      /* On appelle l'interface */
      global_pid = 0;
      INTERFACE_RAV2;

      for (pid = 0; pid < PID; pid++) {
	pG = Gs [pid];

	valid_clause_set = used_clause [pid];
	new_valid_clause_set = sxba_calloc (pG->clause_nb+1);

	if (needs_lub) {
	  clause2clause0 = clause2clause0s [pid];
	  new_clause2clause0 = (SXINT*) sxalloc (pG->clause_nb+1, sizeof (SXINT)), new_clause2clause0 [0] = clause2clause0 [0];

	  new_valid_nt_set = sxba_calloc (pG->ntmax+1);
	}

	clause = 0;

	while ((clause = sxba_scan (valid_clause_set, clause)) > 0) {
	  if ((main_clause = pG->from2var2clause [clause]) > 0) {
	    SXBA_1_bit (new_valid_clause_set, main_clause);

	    if (needs_lub) {
	      new_clause2clause0 [main_clause] = clause2clause0 [clause];

	      /* Les codes des lhs sont identiques sur la forme origine et la forme 2var, on se contente donc
		 de refabriquer un new_valid_nt_set */
	      A = pG->clause2lhs_nt [main_clause]; 
	      SXBA_1_bit (new_valid_nt_set, A);
	    }
	  }
	}

	sxfree (valid_clause_set), used_clause [pid] = new_valid_clause_set;

	if (needs_lub) {
	  sxfree (clause2clause0), clause2clause0s [pid] = new_clause2clause0;
	  sxfree (valid_nt_sets [pid]), valid_nt_sets [pid] = new_valid_nt_set;
	}
      }
#endif /* is_rav2 */

#if is_coupled
      for (pid = 0; pid < PID; pid++) {
	gGs [pid] = pG = Gs [pid];

	/* Il faut completer les valid_clause_set par les "loop" ... */
	if (pG->looped_nt)
	  add_loop_clauses (pG, needs_lub);

	/* ... et les clauses identiques */
	if (pG->clause2identical_disp != NULL)
	  add_identical_clauses (pG, needs_lub);
      }

      /* interface du parser guide' */
      global_pid = 0;
      INTERFACE2;

      for (pid = 0; pid < PID; pid++) {
	pG = Gs [pid];
	gG = gGs [pid]; /* grammaire du guide */
	valid_clause_set = used_clause [pid];
	new_valid_clause_set = sxba_calloc (pG->clause_nb+1);

	if (needs_lub) {
	  clause2clause0 = clause2clause0s [pid];
	  new_clause2clause0 = (SXINT*) sxalloc (pG->clause_nb+1, sizeof (SXINT)), new_clause2clause0 [0] = 0;
	  clause02ub = clause02ubs [pid];
	  new_clause02ub = (SXBA*) sxalloc (*clause2clause0+1, sizeof (SXBA));
	  clause02lb = clause02lbs [pid];
	  new_clause02lb = (SXBA*) sxalloc (*clause2clause0+1, sizeof (SXBA));

	  valid_nt_set = valid_nt_sets [pid];
	  new_valid_nt_set = sxba_calloc (pG->ntmax+1);
	  A2A0 = A2A0s [pid];
	  new_A2A0 = (SXINT*) sxcalloc (pG->ntmax+1, sizeof (SXINT));
	  A02ub = A02ubs [pid];
	  new_A02ub = (SXBA*) sxalloc (*A2A0+1, sizeof (SXBA));
	  A02lb = A02lbs [pid];
	  new_A02lb = (SXBA*) sxalloc (*A2A0+1, sizeof (SXBA));

	  gA = 0;

	  while ((gA = sxba_scan (valid_nt_set, gA)) > 0) {
	    Ad = A2A0 [gA]; /* A est unaire */
	    sxba_empty (A02lb [Ad]);
	    sxba_empty (A02ub [Ad]);
	  }

	  sxba_empty (valid_nt_set);
	}

	kclauses_nb = (SXINT*) sxcalloc (pG->clause_nb+1, sizeof (SXINT));	

	logk = sxlast_bit (pG->clause_nb);
	andk = (1<<logk)-1;

	gclause = 0;

	while ((gclause = sxba_scan (valid_clause_set, gclause)) > 0) {
	  /* valid_clause_set contient aussi les clauses "identiques" et les "boucles" */
	  clause = (pG->guide_clause2clause [gclause]) & andk;
	  arity = pG->nt2arity [pG->clause2lhs_nt [clause]];

	  /* On valide une clause (et un prdct) ssi toutes ses sous-clauses sont ds le guide */
	  if (++kclauses_nb [clause] == arity) {
	    /* Il se peut que des &Lex ne soient + valides */
	    /* Essai sur xtag3.47.sdag invalide une seule clause */
	    if ((line = pG->Lex [clause]) == NULL || sxba_is_subset (line, source_set)) {
	      SXBA_1_bit (new_valid_clause_set, clause);
	      kclauses_nb [clause] = 0;
	    }
	  }
	}

	if (needs_lub) {
	  gclause = 0;

	  while ((gclause = sxba_scan (valid_clause_set, gclause)) > 0) {
	    /* valid_clause_set contient aussi les clauses "identiques" et les "boucles" */
	    gd = clause2clause0 [gclause]; /* gclause est d'arite 1 */
	    clause = pG->guide_clause2clause [gclause];
	    pos = clause>>logk;
	    clause &= andk;

	    if (SXBA_bit_is_set (new_valid_clause_set, clause)) {
	      A = pG->clause2lhs_nt [clause];
	      arity = pG->nt2arity [A];
	  
	      if (kclauses_nb [clause] == 0) {
		/* 1er coup */
		kclauses_nb [clause] = 1;
		new_clause2clause0 [clause] = *new_clause2clause0+1;
		*new_clause2clause0 += arity; /* on reserve la place pour tous les args */
	    
		if (new_A2A0 [A] == 0) {
		  SXBA_1_bit (new_valid_nt_set, A);
		  new_A2A0 [A] = *new_A2A0+1;
		  *new_A2A0 += arity;
		}
	      }

	      cur = new_clause2clause0 [clause] + pos;

	      gA = gG->clause2lhs_nt [gclause];
	      Acur = new_A2A0 [A] + pos;
	      Ad = A2A0 [gA]; /* gA est unaire (1-RCG) */
	      SXBA_1_bit (valid_nt_set, gA);

	      sxba_or (new_A02lb [Acur] = A02lb [Ad], new_clause02lb [cur] = clause02lb [gd]);
	      sxba_or (new_A02ub [Acur] = A02ub [Ad], new_clause02ub [cur] = clause02ub [gd]);
	    }
	  }
	}

#if 0
	/* Si valid_clause_set est non vide, on peut l'utiliser pour "refiltrer" la sortie (dont le source)
	   de LRFSA (est-ce qu'une rexecution de LRFSA pourrait ameliorer les choses ? */
	t_set = sxba_calloc (pG->tmax+1);
	clause = 0;

	while ((clause = sxba_scan (new_valid_clause_set, clause)) > 0) {
	  if (line = pG->Lex [clause])
	    sxba_or (t_set, line);
	}

	if (sxba_is_subset (t_set, source_set))
	  clause = sxba_cardinal (t_set);

	sxfree (t_set);
	/* Aucune amelioration (t_set \subset source_set) lors de l'essai effectue */
#endif

#if 0
	/* Il se peut que des clauses ne sont pas valides car des rhs_nt ne sont pas valides */
	if (pG->clause2rhs_nt_set) {
	  clause = 0;

	  while ((clause = sxba_scan (new_valid_clause_set, clause)) > 0) {
	    if ((rhs_nt_set = pG->clause2rhs_nt_set [clause]) != NULL &&
		!sxba_is_subset (rhs_nt_set, new_valid_nt_set)) {
	      SXBA_0_bit (new_valid_clause_set, clause);
	    }
	  }

	  /* Il y a peut-etre eu "disparition" de nt, on cherche un point fixe */
	}

	/* aucune clause n'a disparu lors de l'essai effectue */
#endif

	sxfree (valid_clause_set), used_clause [pid] = new_valid_clause_set;

	if (needs_lub) {
	  sxfree (clause2clause0), clause2clause0s [pid] = new_clause2clause0;
	  sxfree (clause02ub), clause02ubs [pid] = new_clause02ub;
	  sxfree (clause02lb), clause02lbs [pid] = new_clause02lb;

	  sxfree (valid_nt_set), valid_nt_sets [pid] = new_valid_nt_set;
	  sxfree (A2A0), A2A0s [pid] = new_A2A0;
	  sxfree (A02ub), A02ubs [pid] = new_A02ub;
	  sxfree (A02lb), A02lbs [pid] = new_A02lb;
	}

	sxfree (kclauses_nb);
      }
#endif /* is_coupled */

#if is_rav22
      for (pid = 0; pid < PID; pid++) {
	gGs [pid] = Gs [pid];
      }

      global_pid = 0;
      INTERFACE2_RAV2;

      for (pid = 0; pid < PID; pid++) {
	valid_clause_set = used_clause [pid];
	pG = Gs [pid];
	gG = gGs [pid]; /* grammaire en forme 2var */

	valid_clause_set = used_clause [pid];
	new_valid_clause_set = sxba_calloc (pG->clause_nb+1);

	if (needs_lub) {
	  /* Le mapping en forme 2var conserve le code des non-terminaux, on ne fait donc presque rien sur les nt */
	  clause2clause0 = clause2clause0s [pid];
	  new_clause2clause0 = (SXINT*) sxalloc (pG->clause_nb+1, sizeof (SXINT)), new_clause2clause0 [0] = clause2clause0 [0];
	  new_valid_nt_set = sxba_calloc (pG->ntmax+1);
	}

	clause = 0;

	while ((clause = sxba_scan (valid_clause_set, clause)) > 0) {
	  if ((main_clause = pG->from2var2clause [clause]) > 0) {
	    SXBA_1_bit (new_valid_clause_set, main_clause);
	
	    if (needs_lub) {
	      new_clause2clause0 [main_clause] = clause2clause0 [clause]; 

	      /* Les codes des lhs sont identiques sur la forme origine et la forme 2var, on se contente donc
		 de refabriquer un new_valid_nt_set */
	      A = pG->clause2lhs_nt [main_clause];
	      SXBA_1_bit (new_valid_nt_set, A);
	    }
	  }
	}

	sxfree (valid_clause_set), used_clause [pid] = new_valid_clause_set;

	if (needs_lub) {
	  sxfree (clause2clause0), clause2clause0s [pid] = new_clause2clause0;
	  sxfree (valid_nt_sets [pid]), valid_nt_sets [pid] = new_valid_nt_set;
	}
      }
#endif /* is_rav22 */

      /* On est sur la grammaire d'origine, on reconstitue donc toutes les clauses */
      for (pid = 0; pid < PID; pid++) {
	pG = Gs [pid];

	/* Il faut completer les valid_clause_set par les "loop" ... */
	if (pG->looped_nt)
	  add_loop_clauses (pG, needs_lub);

	/* ... et les clauses identiques */
	if (pG->clause2identical_disp != NULL)
	  add_identical_clauses (pG, needs_lub);
      }

      /* Ouf, on est pret a sortir les clauses */
      if (forest_level & (FL_lhs_clause | FL_lhs_prdct))
	out_shallow_parse_forest (Gs);

      if (forest_level & FL_clause)
	print_used_clause (Gs);

      for (pid = 0; pid < PID; pid++) {
	sxfree (used_clause [pid]), used_clause [pid] = NULL;

	if (needs_lub) {
	  sxfree (clause2clause0s [pid]), clause2clause0s [pid] = NULL;
	  sxfree (clause02lbs [pid]), clause02lbs [pid] = NULL;
	  sxfree (clause02ubs [pid]), clause02ubs [pid] = NULL;

	  sxfree (valid_nt_sets [pid]), valid_nt_sets [pid] = NULL;
	  sxfree (A2A0s [pid]), A2A0s [pid] = NULL;
	  sxfree (A02lbs [pid]), A02lbs [pid] = NULL;
	  sxfree (A02ubs [pid]), A02ubs [pid] = NULL;
	}
      }
    } /* if (ret_val && (forest_level & (~FL_source)) && store_rfsaGs [0].clause2lub) { */
#endif /* #ifdef INTERFACE_1_RCG */
    if (forest_level & FL_source || out_ff2clause_set [0]) {
    /* On ressort le sdag reduit */
      out_sdag (Gs, glbl_source, out_ff2clause_set, 1 /* On ne sort que les clauses lexicalisees */);
    }
#endif /* #if is_parse_forest */
#endif /* #ifndef INTERFACE */
#endif /* is_lfsa */

#ifdef INTERFACE
#if is_coupled
    /* interface du parser guide' */
    global_pid = 0;
    INTERFACE2;
#if PID>1
    for (pid = 0; pid < PID; pid++) {
      Guided_Gs [pid] = Gs [pid]; /* Pour coupled_last_pass */
      Guided_Gs [pid]->Gs = &(Guided_Gs [0]);
    }

    check_combinatorial_options (Guided_Gs [0]);
#else
    Guided_Gs [0] = Gs [0];
    Guided_Gs [0]->Gs = &(Guided_Gs [0]);
#endif /* PID>1 */
#endif /* is_coupled */

#if !is_guided
    /* Sinon, deja fait ds guide_semact */
#if is_rav2
    /* interface des parsers d'origine */
    global_pid = 0;
    INTERFACE_RAV2;
#if PID>1
    for (pid = 0; pid < PID; pid++) {
      Rav2_Gs [pid] = Gs [pid]; /* Pour coupled_last_pass */
      Rav2_Gs [pid]->Gs = &(Rav2_Gs [0]);
    }
#else
    Rav2_Gs [0] = Gs [0];
    Rav2_Gs [0]->Gs = &(Rav2_Gs [0]);
#endif /* PID>1 */
#endif /* is_rav2 */

    global_pid = 0;
    INTERFACE;

#if PID>1
    for (pid = 0; pid < PID; pid++) {
      Gs [pid]->Gs = &(Gs [0]);
    }

    check_combinatorial_options (Gs [0]);
#else
    Gs [0]->Gs = &(Gs [0]);
#endif
#endif /* !is_guided */

#if 0
#if PID>1
    sxword_free (&str_tbl);
#endif
#endif /* 0 */

#if is_generator
    source_generator_init (2*n);
#endif
 	
#if is_lfsa
    /* On prepare l'exploitation des structures construites par (l|r)fsa */
    if (ret_val) {
      /* Le source elague a deja ete construit */
      /* La fabrication de "is_Lex"  par lexicalization () peut se faire directement */
      /* On prepare l'utilisation des (l|u)b calculees par (l|r)fsa */
      /* lexicalization () pourra y retoucher */
      for (pid = 0; pid < PID; pid++) {
#if is_rfsa
	Gs [pid]->clause2lbs = store_rfsaGs [pid].clause2lub;
	Gs [pid]->nt2lbs = store_rfsaGs [pid].nt2lub;
#endif /* is_rfsa */
	Gs [pid]->clause2ubs = store_lfsaGs [pid].clause2lub;
	Gs [pid]->nt2ubs = store_lfsaGs [pid].nt2lub;
      }

      /* l'analyse (l|r)fsa a marche, on lance donc une analyse non robuste */
      is_robust_run = FALSE;
      ret_val = rcgparse_it (n, 1);
    }
    else {
      /* Ici, le source est obligatoirement errone', on lance directement une analyse robuste (si elle existe) */
      if ((Gs [0])->robust && perform_robust_pass (1)) {
	is_robust_run = TRUE;
	ret_val = rcgparse_it (n, 1);
      }
    }
#else /* !is_lfsa */
    is_robust_run = FALSE;
    ret_val = rcgparse_it (n, 1);
#endif /* !is_lfsa */


#if is_coupled
#if EBUG2
    out_ebug2 ();
#endif

    if (ret_val) {
      /* DEBUT de la PASSE GUIDEE/COUPLEE */

      /* Si la construction du guide s'est faite par une forme 2var, il faut changer la valeur
	 du lognt des grammaires guidees car le rhoA_hd utilise' est celui de la forme 2var
	 du constructeur de guide */
#if is_rav2
#if PID>1
      for (pid = 0; pid < PID; pid++) {
	Guided_Gs [pid]->guide_lognt = Gs [pid]->lognt;
      }
#else
      Guided_Gs [0]->guide_lognt = Gs [0]->lognt;
#endif
#endif /* is_rav2 */

#if is_rav22
      /* On commence par appeler l'interface des parsers d'origine dont on va memoriser les grammaires */
      global_pid = 0;
      INTERFACE2_RAV2;
#if PID>1
      for (pid = 0; pid < PID; pid++) {
	Rav2_Gs [pid] = Gs [pid]; /* Pour coupled_last_pass */
	Rav2_Gs [pid]->Gs = &(Rav2_Gs [0]);
      }
#else
      Rav2_Gs [0] = Gs [0];
      Rav2_Gs [0]->Gs = &(Rav2_Gs [0]);
#endif /* PID>1 */
#endif /* is_rav22 */

      /* On reinitialise et on lance la passe guidee */
#if PID>1
      for (pid = 0; pid < PID; pid++) {
	Gs [pid] = Guided_Gs [pid];
      }
#else
      Gs [0] = Guided_Gs [0];
#endif /* PID>1 */
      /* Ici on a Gs == Guided_Gs */

      is_robust_run = FALSE;
      ret_val = rcgparse_it (n, 3);
    }
#endif /* is_coupled */

#endif /* #ifdef INTERFACE */

#if is_lfsa
    /* FREE ds structures (l|r)fsa */
    for (pid = 0; pid < PID; pid++) {
      *lfsaGs [pid] = store_lfsaGs [pid];
      /* On free le reste */
      (*(store_lfsaGs [pid].final_fsa)) ();
#if is_rfsa
      if (rfsaGs [pid] != NULL) {
	/* Si ret_val de lfsa est faux, rfsa n'a pas ete appele */
	*rfsaGs [pid] = store_rfsaGs [pid];
	/* On free le reste */
	(*(store_rfsaGs [pid].final_fsa)) ();
      }
#endif /* is_rfsa */
    }
#endif /* is_lfsa */

#endif                                                              /* !IS_EARLEY_GUIDE */


#if is_sdag
    /* Pour ressortir le sdag reduit */
    if (ff_ste) {
      sxfree (ff_ste), ff_ste = NULL;
    }
#endif

    sxbm_free (glbl_source_area), glbl_source_area = glbl_source = NULL;
    sxfree (source_set), source_set = NULL;

#if MEASURES
    sxfree (unknown_word_set), unknown_word_set = NULL;
#endif /* MEASURES */

#if is_generator
    source_generator_free ();
#endif


#if PID>1
    sxfree (g2l), g2l = NULL;
    sxfree (l2g), l2g = NULL;
    sxfree (gl_area), gl_area = NULL;
#endif

    if (is_print_time)
      sxtime (TIME_FINAL, "\tDone");


#if EBUG2
    out_ebug2 ();
#endif

    return 0;

  case ACTION:
    switch (numact) {
    case 1:			/* <TL> = <TL> <T> ; 1 */
      /* Le code de <T> est SXGET_TOKEN (sxplocals.atok_no-1).lahead */
      /* Il est associe a l'index tok_no */
      tok = SXGET_TOKEN (sxplocals.atok_no-1).lahead;
      SXBA_1_bit (glbl_source [tok_no], tok), t_nb++;

#if is_semantics && is_generator && ANY
      if (tok == ANY)
	ANY_nb++;
#endif

      if (tok <= tmax)
	SXBA_1_bit (source_set, tok);

      if (SXGET_TOKEN (sxplocals.atok_no).lahead == sxsvar.SXS_tables.S_termax-1)
	tok_no++;
	    
      return 0;

    case 2: /* <FF> = %MOT ; 2 */ 
    case 3: /* <FF> = <T> ; 3 */
    case 4: /* <FF> = ; 4 */
#if is_parse_forest
      switch (numact) {
      case 2: /* <FF> = %MOT ; 2 */ 
	if (ff_ste != NULL)
	  ff_ste [tok_no] = SXGET_TOKEN (sxplocals.atok_no-1).string_table_entry;
	break;
      case 3: /* <FF> = <T> ; 3 */
	if (ff_ste != NULL)
	  ff_ste [tok_no] = -SXGET_TOKEN (sxplocals.atok_no-1).lahead;
	break;
      case 4: /* <FF> = ; 4 */
	if (ff_ste != NULL)
	  ff_ste [tok_no] = EMPTY_STE;
	break;
      }
#endif
      if (SXGET_TOKEN (sxplocals.atok_no+1).lahead == sxsvar.SXS_tables.S_termax-1) {
	/* On est ds le cas <FF> { } */
	/* Mot inconnu, on met T */
#if MEASURES
	SXBA_1_bit (unknown_word_set, tok_no);
#endif /* MEASURES */

	for (tok = 1; tok <= tmax; tok++) {
	  SXBA_1_bit (glbl_source [tok_no], tok), t_nb++;
	  SXBA_1_bit (source_set, tok);
	}
	  
	tok_no++;
      }

      return 0;

    case 0:
      return 0;

    default:
      break;
    }

  default:
    gripe ();
  }
}

#endif /* is_sdag */

#if is_coupled
static SXBA *cmemoP [PID];
/* Pour le traitement particulier du debordement de cmemoP */
static SXBA *cmemoP_oflw_disp [PID];
static SXINT cmemoP_oflw_disp_size [PID];
static SXINT andk_filters [PID];

static void
coupled_mem_mngr (struct G *G, SXINT old_size, SXINT new_size)
{
  SXINT l, pid = G->pid;

  if (old_size == 0) {
    /* ALLOCATION */
    andk_filters [pid] = (1<<sxlast_bit (Guided_Gs [pid]->clause_nb))-1;

#if 0
    if (Guided_Gs [pid]->invalid_lex_loop_clause)
      /* if (Guided_Gs [pid]->loop_lex) */
      coupled_used_clause [pid] = sxba_calloc (G->clause_nb+1); 
#endif /* 0 */

    cmemoP [pid] = sxbm_calloc (new_size+1, Guided_Gs [pid]->max_clause_id+1);

#if is_fully_coupled
    l = (n+1)*(n+2); if (l > 4096) l = 4096;
    /* Cas FULLY COUPLED */
    /* Pour le stockage des couples (predicat instancie X clause) */
    XxY_alloc (&(XxY_lhs_guide [pid]), "XxY_lhs_guide", l, 1, 0, 0, XxY_lhs_guide_oflw, statistics);
    XxY_lhs_guide2rhs [pid] = (struct spf_node**) sxalloc (XxY_size (XxY_lhs_guide [pid])+1, sizeof (struct spf_node*));
    XxY_lhs_guide2rhs [pid] [0] = NULL;

    if (rhs_guide_area_hd.size == 0) {
      rhs_guide_area_hd.size = n*n*sizeof(struct spf_node); /* Pourquoi pas */
      allocate_area (&rhs_guide_area_hd);
    }
#endif /* is_fully_coupled */
  }
  else {
    if (new_size == 0) {
      /* FREE */ 
#if 0
      if (coupled_used_clause [pid])
	sxfree (coupled_used_clause [pid]), coupled_used_clause [pid] = NULL; 
#endif /* 0 */

      sxbm_free (cmemoP [pid]), cmemoP [pid] = NULL;

      if (cmemoP_oflw_disp [pid])
	sxbm_line_free (cmemoP_oflw_disp [pid], cmemoP_oflw_disp_size [pid]), cmemoP_oflw_disp [pid] = NULL;

#if is_fully_coupled
      sxfree (XxY_lhs_guide2rhs [pid]), XxY_lhs_guide2rhs [pid] = NULL;
      XxY_free (&(XxY_lhs_guide [pid]));

      if (rhs_guide_area_hd.size != 0) {
	free_area (&rhs_guide_area_hd), rhs_guide_area_hd.size = 0;
      }
#endif /* is_fully_coupled */
    }
    else {
      /* REALLOCATION */
      /* Il faut reallouer memoP */
      cmemoP [pid] = (SXBA*) sxrealloc (cmemoP [pid], new_size+1, sizeof (SXBA));
      sxbm_line_resize (cmemoP [pid], old_size, new_size,
			&(cmemoP_oflw_disp [pid]), &(cmemoP_oflw_disp_size [pid]), Guided_Gs [pid]->max_clause_id+1);
    }
  }
}


static SXINT
fill_coupled_lex (struct G **gGs /* Grammaires du guide */, SXINT S0n, SXINT ret_val)
{
  SXINT		pid, gclause, clause, clause_id, nt, arity, x, andk_filter;
  struct G 	*G, *gG;
  SXBA	        set, loop_clause_set;
  SXINT	        *kclauses_nb;
#if 0
  SXINT	        *nt2nbs;
#endif /* 0 */

  if (ret_val == -1)
    return -1;

  /* Le traitement des loop doit etre independant de is_lex */
  /* nouvelle version du 10/6/2002 */
  /* On remplit G->invalid_lex_loop_clause */
  for (pid = 0; pid < PID; pid++) {
    /* Les pid des guides ou des parsers guides sont identiques */
    G = Guided_Gs [pid]; /* Grammaire guide'e */

    if (G->invalid_lex_loop_clause) {
      /* A priori toutes les loop clauses sont invalides */
      gG = gGs [pid]; /* Grammaire du guide */
      loop_clause_set = G->loop_clause_set; /* loop_clause_set de la grammaire guidee */

      andk_filter = (1<<sxlast_bit (G->clause_nb))-1;

      gclause = 0;

      while ((gclause = sxba_scan (gG->invalid_lex_loop_clause, gclause)) > 0) {
	/* gclause est invalide ds le guide... */
	clause = G->guide_clause2clause [gclause] & andk_filter;
	/* ...clause est donc aussi invalide ds la grammaire guidee... */

	/* ...si de plus c'est une boucle, on l'exclut */
	if (SXBA_bit_is_set (loop_clause_set, clause))
	  SXBA_1_bit (G->invalid_lex_loop_clause, clause);
      }
    }
  }

#if 0
#if is_lex	
  /* On ne conserve ca que pour remplir G->loop_lex */
  for (pid = 0; pid < PID; pid++) {
    /* Les pid des guides ou des parsers guides sont identiques */

    if (set = coupled_used_clause [pid]) {
      G = Guided_Gs [pid]; /* Grammaire guide'e */
      gG = gGs [pid]; /* Grammaire du guide */  
#if 0  
      nt2nbs = (SXINT*) sxcalloc (G->ntmax+1, sizeof (SXINT));
#endif /* 0 */
      kclauses_nb = (SXINT*) sxcalloc (G->clause_nb+1, sizeof (SXINT));

      andk_filter = (1<<sxlast_bit (G->clause_nb))-1;

      gclause = 0;

      while ((gclause = sxba_scan (set, gclause)) > 0) {
	/* set contient aussi les clauses "identiques" et les "boucles" */
	clause = G->guide_clause2clause [gclause] & andk_filter;
	nt = G->clause2lhs_nt [clause];
	arity = G->nt2arity [nt];

	/* On valide une clause ssi toutes ses sous-clauses sont ds le guide */
	if (++kclauses_nb [clause] == arity) {
#if 0
	  if (G->loop_lex) SXBA_1_bit (G->loop_lex, clause);
	  SXBA_1_bit (G->loop_lex, clause);
#endif /* 0 */
	  SXBA_0_bit (G->invalid_lex_loop_clause, clause);

#if 0
	  if (SXBA_bit_is_set (G->main_clause_set, clause)) {
	    /* La reconnaissance de "clause" est generee */
#if 0
	    G->lex_code [G->nt2lex_code [nt] + nt2nbs [nt]++] = G->clause2clause_id [clause];
#endif /* 0 */
	    clause_id = G->clause2clause_id [clause];
	    SXBA_1_bit (G->nt2clause_id_set [nt], clause_id);
	  }
#endif /* 0 */
	}
      }

#if 0
      sxfree (nt2nbs);
#endif /* 0 */
      sxfree (kclauses_nb);
    }
  }
#endif /* is_lex */
#endif /* 0 */
  return 1;
}

static SXINT
coupled_last_pass (struct G **gGs /* Grammaires du guide */, SXINT S0n, SXINT ret_val)
{
    SXINT		pid;
    struct G 	*G, *gG;

    /* On commence par stocker axiom_0_n */
    axiom_0_n = S0n;

    /* On commence par remplir les "lex" */
    if (fill_coupled_lex (gGs, S0n, ret_val) == -1)
      return -1;

    /* Puis, on "passe" les structures du guide au parseur guide' */
    /* ATTENTION : les structures du guide ne doivent pas etre detruite par la passe finale du
       constructeur de guide mais par la passe finale du parser guide' */
    for (pid = 0; pid < PID; pid++) {
	G = Guided_Gs [pid]; /* Grammaire guide'e */
	gG = gGs [pid]; /* Grammaire du guide */

	G->gpmemoN = gG->pmemoN;
	G->gpmemoP = cmemoP [pid];
	/* G->gpmemoP = gG->pmemoP; */
	G->gprange_1 = gG->prange_1;
	G->gprhoA_hd = gG->prhoA_hd;

	/* Pour que la passe finale du constructeur de guide ne fasse rien */
	(gG->pmemoN) = NULL;
	(gG->pmemoP) = NULL;
	(gG->prange_1) = NULL;
	(gG->prhoA_hd) = NULL;
    }

    return 1;
}

static void
coupled_loop (struct G *G, SXINT *rho, SXINT cycle_kind)
{
    /* Arho = rho [1]
       Le calcul de toutes les instantiations des Arho-clauses est termine, au moins une
       clause valide est impliquee ds un cycle (ou une boucle) */
    /* rho [0] est une A-clause */

    /* G->pass_no == 1 */
    process_loop (G, rho, coupled_action, cycle_kind);
}



static SXINT
coupled_action (struct G *gG, SXINT rho)
{
    SXINT			gclause, gA_ij, son_nb, *sons, nb, son, *pto, son_pid, rhoi, bot2, top2, cur2;
    SXINT                 clause, clause_id, andk;
    SXBA		set;
    struct Aij_struct	*Aij_struct_son_ptr;
    SXINT			pid = gG->pid;
    struct G		*G, *sonG;
    BOOLEAN		has_identical;

    icnb++;
#if EBUG2
    instantiated_clause_nb++;
#endif

#if EBUG3
    print_instantiated_clause (gG, rho, gG->name, TRUE, TRUE, TRUE);
#endif

    gclause = *rho;

    if (has_identical = (gG->clause2identical_disp != NULL &&
			 ((bot2 = gG->clause2identical_disp [gclause]) < (top2 = gG->clause2identical_disp [gclause+1])))) {
      /* Il y a des clauses identiques */
      icnb += top2-bot2;
#if EBUG2
      instantiated_clause_nb += top2-bot2;
#endif
    }

#if 0
    if (set = coupled_used_clause [pid] && SXBA_bit_is_reset_set (set, gclause)) {
      if (has_identical) {
	cur2 = bot2;

	while (cur2 < top2) {
	  clause = gG->clause2identical [cur2];
	  SXBA_1_bit (set, clause);
	  cur2++;
	}
      }
    }
#endif /* 0 */

    G = Guided_Gs [pid]; /* Grammaire guide'e */

    set = cmemoP [pid] [rho [1]];
    andk = andk_filters [pid];
    clause = G->guide_clause2clause [gclause] & andk;

    if (SXBA_bit_is_set (G->main_clause_set, clause)) {
      /* clause est une main */
      /* Si clause n'est pas une main, on suppose qu'aucune des clauses identiques a gclause ne peut donner de main */
      /* On change la taille */
      set [0] = G->A2max_clause_id [G->clause2lhs_nt [clause]] +1L;
      clause_id = G->clause2clause_id [clause];
      SXBA_1_bit (set, clause_id);

      if (has_identical) {
	cur2 = bot2;

	while (cur2 < top2) {
	  clause = G->guide_clause2clause [gG->clause2identical [cur2]] & andk;
	  clause_id = G->clause2clause_id [clause];
	  SXBA_1_bit (set, clause_id);
	  cur2++;
	}
      }
    }

#if is_fully_coupled
    /* On recupere ilb [0] et iub [0] */
    rho++;
    gA_ij = *rho++;

    son_nb = gG->clause2son_nb [gclause];

    if (son_nb > 0) {
	node_ptr = book_area (&rhs_guide_area_hd, son_nb);
	sons = gG->clause2son_pid [gclause]+1;
	pto = &(node_ptr->rho [0]);

	for (son = 1; son <= son_nb; son++) {
	    son_pid = *sons++;
	    rhoi = *rho++;

	    if (son_pid < 0) {
		if (son_pid == TRUE_ic) {
		    *pto++ = rhoi;
		}
	    }
	    else {
		sonG = gG->Gs [son_pid];
		Aij_struct_son_ptr = (*(sonG->Aij2struct))(rhoi);

		if (rhoi < 0)
		  /* Appel negatif */
		  rhoi = -rhoi;

		*pto++ = MAKE_ij (Aij_struct_son_ptr->lb [0], Aij_struct_son_ptr->ub [0]);
	    }
	}
    }
    else {
	node_ptr = NULL;
    }

    gpid = pid;
    fill_guide (gA_ij, gclause);
    
    if (has_identical) {
	/* Il y a des clauses identiques */
	while (bot2 < top2) {
	    fill_guide (gA_ij, gG->clause2identical [bot2]);
	    bot2++;
	}
    }
#endif /* is_fully_coupled */

    return 1;
}
#endif /* is_coupled */



#if is_parse_forest
#ifdef SYNCHRONOUS
/* Essai le 03/03/08 */
    static void process_synchronous (struct G *G, SXINT *rho);
#endif /* SYNCHRONOUS */

/* Le Mar  7 Nov 2000 14:07:00, pour chaque clause le nombre de fils et chacun de leur pid sont
   stockes ds clause2son_nb et clause2son_pid */

static SXINT
parse_forest_action (struct G *G, SXINT *rho)
  /* rho est un tableau de taille son_nb+2
     rho[0] contient le numero p de la clause courante.  Si p est <0 la clause est -p et
     l'appel est cyclique
     rho[1] contient l'identifiant du vecteur de range de la LHS
     rho[2], ..., rho[son_nb+1] contient les identifiants des vecteurs de range de la RHS */
  /* rho[son_nb+2] contient l'identifiant ds rhs_ranges des olb,oub */
  /* Si G->clause2lhst_disp est non NULL, rho[son_nb+3], ... contient la liste des indexes
     ds le source des terminaux de la LHS */
  /* sons est un tableau de taille sons_nb+1, il matche clause.
       Si clause = A0 --> A1 ... Ap, sons[i] est l'identifiant du module ds lequel est defini Ai */
{
    SXINT			clause, rho0, son_nb, bot, top, cur, item, eq_clause;	
    BOOLEAN		is_set, is_lhs_clause, already_seen, is_lhs, is_rhs;
    SXBA		*bm;
    SXINT			pid = G->pid;

#if is_sdag
    SXINT			*tbot, *ttop, symb;
#endif

    clause = *rho;

    /* Dans le cas rav2[2], s'il n'y a pas eu de'pliage des RHS, main_clause_nb contient le nb total de clauses
       principales qu'aurait produit le de'pliage (s'il y a eu depliage, main_clause_nb==1 */
#if is_rav2 || is_rav22
    icnb += main_clause_nb;
#if EBUG2
    instantiated_clause_nb += main_clause_nb;
#endif
#else /* !(is_rav2 || is_rav22) */
    icnb++;
#if EBUG2
    instantiated_clause_nb++;
#endif
#endif /* !(is_rav2 || is_rav22) */

    is_set = G->clause2identical_disp != NULL &&
	    ((bot = G->clause2identical_disp [clause]) < (top = G->clause2identical_disp [clause+1]));

    if (is_set) {
#if is_rav2 || is_rav22
      icnb += (top-bot)*main_clause_nb;
#if EBUG2
      instantiated_clause_nb += (top-bot)*main_clause_nb;
#endif
#else /* !(is_rav2 || is_rav22) */
      icnb += top-bot;
#if EBUG2
      instantiated_clause_nb += top-bot;
#endif
#endif /* !(is_rav2 || is_rav22) */
    }

    rho0 = rho [1];

    if (forest_level & FL_clause) {
	SXBA_1_bit (used_clause [pid], clause);

	if (is_set) {
	    cur = bot;

	    while (cur < top) {
		eq_clause = G->clause2identical [cur];
		SXBA_1_bit (used_clause [pid], eq_clause);
		cur++;
	    }
	}
    }

    is_lhs_clause = FALSE;

    if ((forest_level & FL_rhs) == FL_rhs) {
	is_lhs_clause = (forest_level & FL_lhs_prdct) ? TRUE : FALSE;
	is_lhs = is_rhs = TRUE;
    }
    else {
	is_rhs = FALSE;

	if ((forest_level & FL_lhs_clause) && (forest_level & FL_lhs_prdct)) {
#if PID==1
	    already_seen = XxY_set (&AijXclause, rho0, clause, &item);

#else
	    if (pid == 0) {
		already_seen = XxY_set (&AijXclause, rho0, clause, &item);
	    }
	    else {
		already_seen = XxYxZ_set (&pidXAijXclause, pid, rho0, clause, &item);
	    }
#endif

	    if (!already_seen)
		/* On doit sortir le/les numeros des clauses */
		is_lhs_clause = is_lhs = TRUE;
	    else
		is_lhs_clause = is_lhs = FALSE;
	}
	else {
	    if (forest_level & FL_lhs_prdct) {
		is_lhs = SXBA_bit_is_reset_set (pid2lhs_prdct_set [pid], rho0);
	    }
	    else
		is_lhs = FALSE;

	    if (forest_level & FL_lhs_clause) {
		is_lhs_clause = TRUE; /* Trace, on le sort tout le temps!! */
	    }
	    else
		is_lhs_clause = FALSE;
	}
    }

#ifdef SYNCHRONOUS
    /* essai de -DSYNCHRONOUS pour savoir ce dont on a besoin, ce qui peut etre genere ou calcule dynamiquement. */
    /* Pour l'instant */
    print_instantiated_clause (G, rho, NULL, is_lhs_clause, is_lhs, is_rhs);

    process_synchronous (G, rho);
#else /* SYNCHRONOUS */
    if (is_lhs_clause || is_lhs || is_rhs)
	print_instantiated_clause (G, rho, NULL, is_lhs_clause, is_lhs, is_rhs);

#if is_sdag
    /* On recherche les source index de tous les terminaux de la clause pour reconstituer
       un sdag reduit */

    /* Ds le cas rav2[2], la memorisation ds bm et glbl_out_source a deja ete faite par le passage
       2var -> rav2 (par fill_out_source) , il ne faut pas la refaire ici (rho ne contient pas les bonnes info.
       Pour eviter ca on a clause2lhst_disp==NULL ds le cas rav2 */
    if (G->clause2lhst_disp) {
      bm = out_ff2clause_set [pid];

      if (bm || glbl_out_source) {
        son_nb = G->clause2son_nb [clause];
	rho += son_nb + 2;
	tbot = G->lhst_list+G->clause2lhst_disp [clause];
	ttop = G->lhst_list+G->clause2lhst_disp [clause+1];

	while (tbot < ttop) {
	  if (glbl_out_source) {
	    symb = *tbot;
	    SXBA_1_bit (glbl_out_source [*rho], symb);
	  }

	  if (bm)
	    SXBA_1_bit (bm [*rho], clause);

	  rho++, tbot++;
	}
      }
    }
#endif /* is_sdag */
#endif /* SYNCHRONOUS */

    return 1;
}


#endif /* is_parse_forest */


#if is_guided && !is_coupled
/* NOUVELLE VERSION */
/* C'est le contenu du guide qui determine le type de guidage */
#if 0
static SXINT	*nt2nbs;
#endif /* 0 */
static SXINT	*kclauses_nb;
static SXBA	guide_used_clause [PID];
static SXBA	*gmemoN [PID], gmemoN1 [PID];
static X_header	grhoA_hd [PID], grange_1 [PID];
static SXBA	*gmemoP [PID];
static SXBA     *gmemoP_oflw_disp [PID];
static SXINT      gmemoP_oflw_disp_size [PID];
static SXINT      andk_filters [PID];

static VOID	gripe_guide ()
{
    fputs ("\nA function of \"guide\" is out of date with respect to its specification.\n", sxstderr);
    sxexit(1);
}

static void
gmemo_oflw (SXINT old_size, SXINT new_size)
{
    gmemoN [gpid] = sxbm_resize (gmemoN [gpid], 2, 2, new_size+1);
    gmemoN1 [gpid] = gmemoN [gpid][1];

#if 0
    gmemoP [gpid] = sxbm_resize (gmemoP [gpid], old_size+1, new_size+1, Gs [gpid]->guide_max_clause_id+1);
#endif /* 0 */
    gmemoP [gpid] = (SXBA*) sxrealloc (gmemoP [gpid], new_size+1, sizeof (SXBA));
    sxbm_line_resize (gmemoP [gpid],
		      old_size, new_size, &(gmemoP_oflw_disp [gpid]), &(gmemoP_oflw_disp_size [gpid]),
		      Gs [gpid]->guide_max_clause_id+1);
}


static void
allocate_guide (SXINT kind)
{
    SXINT		pid, l;

    l = (n+1)*(n+2); if (l > 4096) l = 4096;

    for (pid = 0; pid < PID; pid++) {
#if is_large
	X_alloc (&(grange_1 [pid]), "grange_1", l/2, 1, NULL, statistics);
#endif

	X_alloc (&(grhoA_hd [pid]), "grhoA_hd", l, 1, gmemo_oflw, statistics);
	gmemoN [pid] = sxbm_calloc (2, X_size (grhoA_hd [pid])+1);
	gmemoN1 [pid] = gmemoN [pid][1];
	gmemoP [pid] = sxbm_calloc (X_size (grhoA_hd [pid])+1, Gs [pid]->guide_max_clause_id+1);
	/* gmemo [pid][0] est inutilise ds le cas guide', il est alloue' par compatibilite' avec le cas couple' */

	if (kind != 0) {
	    /* Cas FULLY GUIDED */
	    /* Pour le stockage des couples (predicat instancie X clause) */
	    XxY_alloc (&(XxY_lhs_guide [pid]), "XxY_lhs_guide", l, 1, 0, 0, XxY_lhs_guide_oflw, statistics);
	    XxY_lhs_guide2rhs [pid] = (struct spf_node**) sxalloc (XxY_size (XxY_lhs_guide [pid])+1, sizeof (struct spf_node*));
	    XxY_lhs_guide2rhs [pid] [0] = NULL;
	}
    }
}

static void
free_guide ()
{
    SXINT		pid;

    for (pid = 0; pid < PID; pid++) {
	sxfree (XxY_lhs_guide2rhs [pid]), XxY_lhs_guide2rhs [pid] = NULL;
	XxY_free (&(XxY_lhs_guide [pid]));

	if (gmemoP_oflw_disp [pid]) {
	  sxbm_line_free (gmemoP_oflw_disp [pid], gmemoP_oflw_disp_size [pid]), gmemoP_oflw_disp [pid] = NULL;
	}
    }
}


static void
guide_mem_mngr (SXINT pid, SXINT old_size, SXINT new_size)
{
    if (old_size == 0) {
	/* ALLOCATION */
	guide_used_clause [pid] = sxba_calloc (Gs [pid]->guide_clause_nb+1); 
    }
    else {
	if (new_size == 0) {
	    /* FREE */
	    sxfree (guide_used_clause [pid]), guide_used_clause [pid] = NULL;
	}
	else {
	    /* REALLOCATION */
	}
    }
}


SXINT
guide_semact (SXINT code, SXINT numact)
{
static SXINT		*guide_ranges, gA_ij, glb, gub, max_rhs_arg_nb;
static BOOLEAN		is_first_clause;
    SXINT			size, xstack, ste, clause, clause_id, gclause, gclause_id, nt, range_nb, i, *rho, pair, lhs_nt, arity, pid;
    SXINT			len, A, poids, ij, x, andk_filter;
    SXBA		set;
    char		*str, car;
    struct G		*G;

    switch (code) {
    case OPEN:
#if is_rav2
	/* interface des parsers d'origine */
	global_pid = 0;
	INTERFACE_RAV2;
#if PID>1
	for (pid = 0; pid < PID; pid++) {
	    Rav2_Gs [pid] = Gs [pid]; /* Pour coupled_last_pass */
	    Rav2_Gs [pid]->Gs = &(Rav2_Gs [0]);
	}
#else
	Rav2_Gs [0] = Gs [0];
	Rav2_Gs [0]->Gs = &(Rav2_Gs [0]);
#endif /* PID>1 */
#endif /* is_rav2 */

	global_pid = 0;
	INTERFACE;

#if PID>1
	for (pid = 0; pid < PID; pid++) {
	    G = Gs [pid];

	    G->Gs = &(Gs [0]);
	    guide_mem_mngr (pid, 0, 1);

	    if (G->rhs_args_nb > max_rhs_arg_nb)
		max_rhs_arg_nb = G->rhs_args_nb;

	    andk_filters [pid] = (1<<sxlast_bit (G->clause_nb))-1;

	    if (G->invalid_lex_loop_clause)
	      sxba_copy (G->invalid_lex_loop_clause, G->loop_clause_set);
	}

	check_combinatorial_options (Gs [0]);
#else
	G = Gs [0];G->Gs = &(Gs [0]);
	guide_mem_mngr (0, 0, 1);
	max_rhs_arg_nb = G->rhs_args_nb;
	andk_filters [0] = (1<<sxlast_bit (G->clause_nb))-1;

	if (G->invalid_lex_loop_clause)
	  sxba_copy (G->invalid_lex_loop_clause, G->loop_clause_set);
#endif

	is_first_clause = TRUE;

	return 0;


    case INIT:
#if 0
/* Actuellement, la possibilite ci-dessous n'est pas exploitee */
#if !is_fully_coupled
	/* On inhibe l'analyse de la rhs des clauses instanciees (s'il y en a ... )*/
	sxsvar.lv_s.counters [0] = 1;
#endif /* !is_fully_coupled */
#endif

	return 0;

    case CLOSE:
	if (guide_ranges) {
	    sxfree (guide_ranges), guide_ranges = NULL;
	    free_area (&rhs_guide_area_hd);
	    free_guide ();
	}

	return 0;

    case SEMPASS:
    case ERROR:
    case FINAL:
	return 0;

    case ACTION:
	switch (numact) {
	case 0:
	    return 0;

	case 1:			/* <> = ; 1 */
	    gpid = 0;
	    return 0;

	case 2:			/* <numero de process>	= %SXINT ; 2 */
	    gpid = atoi (sxstrget (STACKtoken (STACKtop()).string_table_entry));
	    return 0;

	case 3:			/* <clause> = "(" %SXINT ".." %SXINT ")" "-->" <rhs> "." ; 3 */

	    /* On est ds le cas fully guided.  */
	    if (is_first_clause) {
		/* complet */
		is_first_clause = FALSE;

		if (guide_ranges == NULL) {
		    /* <rhs> == true */
		    guide_ranges = (SXINT*) sxalloc (max_rhs_arg_nb+1, sizeof (SXINT));
		    guide_ranges [0] = 0;
		    rhs_guide_area_hd.size = n*n*sizeof(struct spf_node); /* Pourquoi pas */
		    allocate_area (&rhs_guide_area_hd);
		}

		allocate_guide (1);
	    }

	    if (range_nb = guide_ranges [0]) {
		node_ptr = book_area (&rhs_guide_area_hd, range_nb);
		rho = &(node_ptr->rho [0]);

		for (i = 1; i <= range_nb; i++)
		    *rho++ = guide_ranges [i];

		guide_ranges [0] = 0;
	    }
	    else
		node_ptr = NULL;

	    xstack = STACKnewtop()+1;
	    glb = atoi (sxstrget (STACKtoken (xstack).string_table_entry));
	    gub = atoi (sxstrget (STACKtoken (xstack+2).string_table_entry));

	    return 0;

	case 4:			/* <numero clause> = %SXINT ":" <nom clause> ; 4
				   <1> = %SXINT <1> ; 4
				   <1> = %SXINT "}" ":" <nom clause> ; 4 */

	    gclause = atoi (sxstrget (STACKtoken (STACKnewtop()).string_table_entry));
	    SXBA_1_bit (guide_used_clause [gpid], gclause);

	    G = Gs [gpid];	/* Grammaire guide'e */
	    clause = G->guide_clause2clause [gclause] & andk_filters [gpid];

	    if (SXBA_bit_is_set (G->main_clause_set, clause)) {
	      /* clause est une main */
	      set = gmemoP [gpid] [gA_ij];
	      /* On change la taille */
	      set [0] = G->A2max_clause_id [G->clause2lhs_nt [clause]]+1L;
	      clause_id = G->clause2clause_id [clause];
	      SXBA_1_bit (set, clause_id);
	    }

	    if (guide_ranges)
		fill_guide (gA_ij, gclause);

	    return 0;

	case 5:			/* <taille du source>	= %n ; 5 */
	    /* ATTENTION : il faut la taille du source pour calculer logn pour MAKE_ij */
	    n = atoi (sxstrget (STACKtoken (STACKtop()).string_table_entry));
	    logn = sxlast_bit (n);

#if !is_large
	    for (pid = 0; pid < PID; pid++) {
		large_needed (Gs [pid]->guide_lognt);
	    }
#endif
	    
	    ANDj = (1<<logn)-1;

	    return 0;

	case 6:			/* <intervalle>	= %SXINT ".." %SXINT ; 6 */
	    /* Cas fully guided */
	    /* On est en RHS */
	    if (guide_ranges == NULL) {
		guide_ranges = (SXINT*) sxalloc (max_rhs_arg_nb+1, sizeof (SXINT));
		guide_ranges [0] = 0;
		rhs_guide_area_hd.size = n*n*sizeof(struct spf_node); /* Pourquoi pas */
		allocate_area (&rhs_guide_area_hd);
	    }

	    xstack = STACKnewtop();
	    glb = atoi (sxstrget (STACKtoken (xstack).string_table_entry));
	    gub = atoi (sxstrget (STACKtoken (xstack+2).string_table_entry));
	    guide_ranges [++*guide_ranges] = MAKE_ij (glb, gub);

	    return 0;

	case 7:			/* <guide> = <taille du source> <clauses instanciees> <numero des clauses> ; 7
				   <guide> = <taille du source> <numero des clauses> ; 7 */
	  /* EOF */
	  for (pid = 0; pid < PID; pid++) {
	    /* Les pid des guides ou des parsers guides sont identiques */
	    G = Gs [pid];	/* Grammaire guide'e */

	    if (G->invalid_lex_loop_clause)
	      /* A priori toutes les loop clauses sont invalides */
	      sxba_copy (G->invalid_lex_loop_clause, G->loop_clause_set);

	    /* On remplit les lex Avec used_clauses */  
#if 0  
	    nt2nbs = (SXINT*) sxcalloc (G->ntmax+1, sizeof (SXINT));
#endif /* 0 */
	    kclauses_nb = (SXINT*) sxcalloc (G->clause_nb+1, sizeof (SXINT));	
	    set = guide_used_clause [pid];

	    andk_filter = andk_filters [pid];

	    gclause = 0;

	    while ((gclause = sxba_scan (set, gclause)) > 0) {
	      clause = G->guide_clause2clause [gclause] & andk_filter;
	      nt = G->clause2lhs_nt [clause];
	      arity = G->nt2arity [nt];

	      /* On valide une clause ssi toutes ses sous-clauses sont ds le guide */
	      if (++kclauses_nb [clause] == arity) {
		if (G->invalid_lex_loop_clause)
		  SXBA_0_bit (G->invalid_lex_loop_clause, clause);

		/* if (G->loop_lex) SXBA_1_bit (G->loop_lex, clause); */
		/* G->is_Lex [clause] = TRUE; */

#if is_lex	
		if (SXBA_bit_is_set (G->main_clause_set, clause)) {
		  /* La reconnaissance de "clause" est generee */
#if 0
		  G->lex_code [G->nt2lex_code [nt] + nt2nbs [nt]++] = G->clause2clause_id [clause];
#endif /* 0 */
		  /* Ca peut faire double emploi avec les gmemoP, tant pis !! */
		  clause_id = G->clause2clause_id [clause];
		  SXBA_1_bit (G->nt2clause_id_set [nt], clause_id);
		}

#endif /* is_lex */
	      }
	    }

#if 0
	    sxfree (nt2nbs);
#endif /* 0 */
	    sxfree (kclauses_nb);

	    guide_mem_mngr (pid, 1, 0);
		
	    /* On "passe" les memo aux parseurs guide's */
	    G->gpmemoN = gmemoN [pid];
	    G->gpmemoP = gmemoP [pid];
	    G->gprange_1 = &(grange_1 [pid]);
	    G->gprhoA_hd = &(grhoA_hd [pid]);
	  }

	    return 0;

	case 9:			/* <nombre> = %SXINT ; 9  */
	    /* %SXINT est un numero de gclause de la liste des gclauses pour le parser gpid */
	    gclause = atoi (sxstrget (STACKtoken (STACKtop()).string_table_entry));
	    SXBA_1_bit (guide_used_clause [gpid], gclause);

	    return 0;

	case 11:		/* <clause> = "(" %SXINT ".." %SXINT ")" "--> ... ." ; 11
				   <clause> = "(" %SXINT ".." %SXINT ")" ; 11 */
	    if (is_first_clause) {
		/* Pas de rhs */
		is_first_clause = FALSE;
		allocate_guide (0);
	    }

	    xstack = STACKnewtop()+1;
	    glb = atoi (sxstrget (STACKtoken (xstack).string_table_entry));
	    gub = atoi (sxstrget (STACKtoken (xstack+2).string_table_entry));

	    return 0;

	case 12:		/* <nom clause> = %nom <clause> ; 12 */
	    /* %nom est le nom du predicat ds la RCG guidee de la forme bla_nnn
	       ou nnn est le code interne ds le guide du predicat bla_nnn */
	    ste = STACKtoken (STACKnewtop()).string_table_entry;
	    str = sxstrget (ste);
	    len = sxstrlen (ste);
	    
	    str += sxstrlen (ste);

	    A = 0;
	    poids = 1;

	    while ((car = *--str) != '_') {
		A += (car-'0')*poids;
		poids *= 10;
	    }

	    ij = MAKE_ij (glb, gub);

#if is_large
	    X_set(&(grange_1 [gpid]), ij, &local_val);
#else
	    local_val = ij;
#endif	    

	    if (!X_set (&(grhoA_hd [gpid]), (local_val<<Gs [gpid]->guide_lognt)+A, &gA_ij)) {
		SXBA_1_bit (gmemoN1 [gpid], gA_ij);

		if (A == 1 && glb == 0 && gub == n)
		    /* gA_ij == axiom(<0..n>) */
		    axiom_0_n = gA_ij;
	    }
    
	    return 0;

	default:
	    break;
	}

    default:
	gripe_guide ();
    }
}
#endif /* is_guided && !is_coupled */


#if is_rav2 || is_rav22
/* Ces fonctions permettent, a partir d'une analyse effectuee avec la forme "2var" d'une RCG
   d'appeler la semantique comme si l'analyse avait eu lieu avec la RCG initiale */

static SXINT 		*main_rho, *aux_rho, *rrho_stack, *rpid_stack, main_pid, main_clause;
static struct area_hd	rav2_area_hd;
static struct spf_node	**rav2_disp [PID];
static SXINT *rav2_Arho_nb [PID];
#if 0
/* Ca semble inutile, c'est coupled_mngr qui doit s'occuper de ca !! */
static SXBA     *rav2_gmemoP_oflw_disp [PID];
static SXINT      rav2_gmemoP_oflw_disp_size [PID];
#endif /* 0 */

static void
from2var_mem_mngr (struct G *G, SXINT old_size, SXINT new_size)
{
  SXINT 	pid = G->pid;
  struct G	*gG = Rav2_Gs [pid];

  if (old_size == 0) {
    /* ALLOCATION */
    /* A priori, on deplie toutes les parties droites... */
    is_rav2_coupled_and_not_FL_rhs = FALSE;

    /* Le 06/08/2002 : pour ressortir le source (forest_level & FL_source), on n'a pas besoin de de'plier et
       le calcul exact du nombre de clauses instanciees ne necessite plus le depliage
       de toutes les parties droites */
    if ((gG->semact.first_pass == parse_forest_action || gG->semact.second_pass == parse_forest_action) &&
	((forest_level & FL_rhs) != FL_rhs) &&
	!((forest_level & FL_supertagger) && gG->is_factorize))
      /* On ne deplie pas */
      is_rav2_coupled_and_not_FL_rhs = TRUE;
#if is_coupled && !is_fully_coupled
    else
      if (gG->semact.first_pass == coupled_action || gG->semact.second_pass == coupled_action)
	/* On ne deplie pas */
	is_rav2_coupled_and_not_FL_rhs = TRUE;
#endif /* is_coupled && !is_fully_coupled */

    (*(gG->parse.first_pass_init))(G); /* Attention, ds ce cas, on passe la forme 2var en param */

    if (!is_rav2_coupled_and_not_FL_rhs)
      rav2_disp [pid] = (struct spf_node**) sxcalloc (new_size+1, sizeof (struct spf_node*));
    else
      rav2_Arho_nb [pid] = (SXINT*) sxcalloc (new_size+1, sizeof (SXINT));

    if (pid == 0) {
      main_rho = (SXINT*) sxalloc (32+2, sizeof (SXINT))+1, main_rho [-1] = 32;

      if (!is_rav2_coupled_and_not_FL_rhs) {
	aux_rho = (SXINT*) sxalloc (32+2, sizeof (SXINT))+1, aux_rho [-1] = 32;
	aux_rho [0] = 0;
	rrho_stack = (SXINT*) sxalloc (64+2, sizeof (SXINT))+1, rrho_stack [-1] = 64;
	rpid_stack = (SXINT*) sxalloc (64+2, sizeof (SXINT))+1, rpid_stack [-1] = 64;
      }
    }
  }
  else {
    if (new_size == 0) {
      /* FREE */
      if (!is_rav2_coupled_and_not_FL_rhs)
	sxfree (rav2_disp [pid]), rav2_disp [pid] = NULL;
      else
	sxfree (rav2_Arho_nb [pid]), rav2_Arho_nb [pid] = NULL;

#if 0
      if (rav2_gmemoP_oflw_disp [pid]) {
	sxbm_line_free (rav2_gmemoP_oflw_disp [pid], rav2_gmemoP_oflw_disp_size [pid]), rav2_gmemoP_oflw_disp [pid] = NULL;
      }
#endif /* 0 */

      if (pid == 0) {
	sxfree (main_rho-1), main_rho = NULL;

	if (!is_rav2_coupled_and_not_FL_rhs) {
	  sxfree (aux_rho-1), aux_rho = NULL;
	  sxfree (rrho_stack-1), rrho_stack = NULL;
	  sxfree (rpid_stack-1), rpid_stack = NULL;
	}
      }

      (*(gG->parse.first_pass_final))();
    }
    else {
      /* REALLOCATION */
      if (!is_rav2_coupled_and_not_FL_rhs)
	rav2_disp [pid] = (struct spf_node**) sxrecalloc (rav2_disp [pid], old_size+1, new_size+1, sizeof (struct spf_node*));
      else
	rav2_Arho_nb [pid] = (SXINT*) sxrecalloc (rav2_Arho_nb [pid], old_size+1, new_size+1, sizeof (SXINT));

      /* Il faut aussi reallouer les "memo" de la passe rav2 */
      if (gG->pmemoN)
	gG->pmemoN = sxbm_resize (gG->pmemoN, gG->memoN_size, gG->memoN_size, new_size+1);

#if 0
      if (gG->pmemoP) {
	gG->pmemoP = (SXBA*) sxrealloc (gG->pmemoP, new_size+1, sizeof (SXBA));
	sxbm_line_resize (gG->pmemoP,
			  old_size, new_size, &(rav2_gmemoP_oflw_disp [pid]), &(rav2_gmemoP_oflw_disp_size [pid]),
			  gG->guide_max_clause_id+1);
      }
#endif /* 0 */

      (*(gG->parse.oflw))();
    }
  }
}

static void
from2var_alloc_mngr (struct G *G, SXINT old_size, SXINT new_size)
{
    if (rav2_area_hd.bot == NULL) {
	rav2_area_hd.size = new_size*sizeof(struct spf_node); /* Pourquoi pas */
	allocate_area (&rav2_area_hd);
    }

    from2var_mem_mngr (G, old_size, new_size);
}


static void
from2var_free_mngr (struct G *G, SXINT old_size, SXINT new_size)
{
    if (rav2_area_hd.bot) {
	free_area (&rav2_area_hd);
    }

    from2var_mem_mngr (G, old_size, new_size);
}


#if is_sdag
static void
fill_out_source (struct G *G /* G est de la forme 2var */, SXINT gclause, SXINT *rho)
{
    SXINT 	x, y, *tbot, *ttop, index, t;
    SXBA	*bm;

    if (G->clause2lhst_disp && glbl_out_source) {
	if ((y=G->clause2lhst_disp [gclause+1])-(x=G->clause2lhst_disp [gclause])) {
	    bm = out_ff2clause_set [main_pid];

	    tbot = G->lhst_list+x;
	    ttop = G->lhst_list+y;

	    while (tbot < ttop) {
		index = *rho++;
		t = *tbot++;

		SXBA_1_bit (glbl_out_source [index], t);

		if (bm)
		    SXBA_1_bit (bm [index], main_clause);
	    }
	}
    }
}
#endif


static void
fill_rrho_stack (struct G *G /* G est de la forme 2var */, SXINT gclause, SXINT *rho)
{
    SXINT 	son_nb, nb, x, y, *tbot, *ttop;
    SXINT 	*prho, *ppid;

    son_nb = G->clause2son_nb [gclause];
    prho = rho+son_nb;

#if is_sdag
    fill_out_source (G, gclause, prho);
#endif

    ppid = G->clause2son_pid [gclause]+son_nb;

    CHECK (rrho_stack, son_nb);
    CHECK (rpid_stack, son_nb);

    while (--prho >= rho) {
	SPUSH (rrho_stack, *prho);
	SPUSH (rpid_stack, *--ppid);
    }
}


static void
unfold_clause_nb (SXINT pid, SXINT *botrho, SXINT *toprho)
{
  SXINT *Arho_nb = rav2_Arho_nb [pid];

  while (--toprho >= botrho) {
    main_clause_nb *= Arho_nb [*toprho];
  }
}


static SXINT from2var_walk (void);

static SXINT
from2var_expand (struct G *G, SXINT Arho)
{
    SXINT			*p, *prho, *ppid, son_nb, x, gclause;
    struct spf_node	*node_ptr;
    SXINT			ret_val = 0, pid = G->pid;

    for ((node_ptr = rav2_disp [pid] [Arho]);
	 node_ptr != NULL;
	 node_ptr = node_ptr->next) {
	p = &(node_ptr->rho [0]);
	gclause = *p++;
	x = TOP (rrho_stack);

	fill_rrho_stack (G, gclause, p);

	if (from2var_walk ())
	    ret_val = TRUE;
	
	TOP (rrho_stack) = TOP (rpid_stack) = x;
    }

    return ret_val;
}


static SXINT
from2var_walk ()
{
  SXINT		Arho, pid, A, couple, index, symb, ret_val;
  SXINT		main_clause, x, y, *p, *q, *r;
  struct G	*G;

  if (TOP (rrho_stack)) {
    Arho = POP (rrho_stack);
    pid = POP (rpid_stack);
    G = Gs [pid];
    couple = X_X (*(G->prhoA_hd), Arho);
    A = couple & ((1<<G->lognt)-1);

    if (A <= Rav2_Gs [pid]->ntmax) {
      /* A est un predicat de la RCG initiale */
      PUSH (main_rho, Arho);

      ret_val = from2var_walk ();

      TOP (main_rho)--;
    }
    else {
      /* A a ete fabrique par la factorisation ? */
      ret_val = from2var_expand (G, Arho);
    }

    PUSH (rrho_stack, Arho);
    PUSH (rpid_stack, pid);
  }
  else {
    /* On appelle la semantique "normale" */
    /* La memorisation des (source_index/terminal) a deja ete faite */
    main_clause = main_rho [1];

    if (!is_rav2_coupled_and_not_FL_rhs && (x = Rav2_Gs [main_pid]->clause2rhs_stack_map [main_clause]) > 0) {
      /* main_rho contient des Arho et son ordre doit etre change' */
      /* l'ordre ds main_rho doit etre change' */
      p = Rav2_Gs [main_pid]->rhs_stack_map+x;
      aux_rho [1] = main_clause;
      *(r = aux_rho+2) = *(q = main_rho+2);

      CHECK (aux_rho, TOP (main_rho));

      while (y = *p++) {
	r [y] = *++q;
      }

      ret_val = (*(Rav2_Gs [main_pid]->semact.first_pass)) (Rav2_Gs [main_pid], aux_rho+1);
    }
    else
      /* l'ordre ds main_rho est correct */
      ret_val = (*(Rav2_Gs [main_pid]->semact.first_pass)) (Rav2_Gs [main_pid], main_rho+1);
  }

  return ret_val;
}

static void
from2var_loop (struct G *G, SXINT *rho, SXINT cycle_kind)
{
    /* Arho = rho [1]
       Le calcul de toutes les instantiations des Arho-clauses est termine, au moins une
       clause valide est impliquee ds un cycle (ou une boucle) */
    /* rho [0] est une A-clause */

    /* G->pass_no == 1 */
    process_loop (G, rho, from2var_mngr, cycle_kind);
}



static SXINT
from2var_mngr (struct G *G, SXINT *rho)
{
  /* On vient ds la forme 2var de reconnaitre une clause instanciee. */
  /* On la "transforme" en clause instanciee pour la rcg originelle et on appelle la semantique */
  SXINT			*p, i, nb, gclause, son_nb, Arho, main_clause_id, ret_val;
  SXBA                memoP;
  struct spf_node	*node_ptr;
  struct G 		*gG;

  main_pid = G->pid;
  gG = Rav2_Gs [main_pid];		/* grammaire de la forme originelle */

  gclause = *rho;

  if ((main_clause = gG->from2var2clause [gclause]) > 0) {
    /* c'est une clause principale */
    RAZ (main_rho); 
    SPUSH (main_rho, main_clause);
    rho++;
    Arho = *rho++;
    SPUSH (main_rho, Arho);
    main_clause_nb = 1;

    if (!is_rav2_coupled_and_not_FL_rhs) {
      /* On remplit local_rho */
      RAZ (rrho_stack), RAZ (rpid_stack); 
      fill_rrho_stack (G, gclause, rho);
      ret_val = from2var_walk ();
    }
    else {
      /* calcul du nb de main_clauses qu'aurait donne le depliage */
      unfold_clause_nb (main_pid, rho, rho+G->clause2son_nb [gclause]);

#if is_sdag
      if (forest_level & FL_supertagger || forest_level & FL_source) {
	/* main_clause est global */
	fill_out_source (G, gclause, rho+G->clause2son_nb [gclause]);
      }
#endif /* is_sdag */

      ret_val = TRUE;
    }

    /* On remplit gG->pmemoN */
    if (gG->pmemoN) {
      SXBA_1_bit (gG->pmemoN [1], Arho); /* memoN1 */
    }

    /* On remplit gG->pmemoP */
    if (gG->pmemoP) {
      memoP = gG->pmemoP [Arho];
      memoP [0] = gG->A2max_clause_id [gG->clause2lhs_nt [main_clause]] +1L; /* On change la longueur */
      main_clause_id = gG->clause2clause_id [main_clause];
      SXBA_1_bit (memoP, main_clause_id);
    }

    /* Le Mer  2 Mai 2001 16:10:14 suppression de l'appel "clear_area (&rav2_area_hd);"
       car des clauses secondaires peuvent etre partagees par des clauses principales */

    return ret_val;
  }

  if (!is_rav2_coupled_and_not_FL_rhs) {
    /* Clause secondaire, on stocke rho */
    nb = G->clause2son_nb [gclause];

#if is_sdag
    if (G->clause2lhst_disp && glbl_out_source)
      /* Pour les couples (source_index, terminal) */
      nb += G->clause2lhst_disp [gclause+1]-G->clause2lhst_disp [gclause];
#endif

    /* Pb il faut pouvoir recuperer la place sur les clauses principales */
    node_ptr = book_area (&rav2_area_hd, nb+1);

    /* rav2_disp doit etre realloue ds le oflw de rhoA_hd de G */
    /* NON, car des clauses secondaires peuvent etre partagees par des clauses principales */
    node_ptr->next = rav2_disp [main_pid] [rho [1]];
    rav2_disp [main_pid] [rho [1]] = node_ptr;
    p = &(node_ptr->rho [0]);
    rho += 2;

    *p++ = gclause;

    for (i = 0; i < nb; i++)
      *p++ = *rho++;
  }
  else {
    /* Le 06/08/2002 : pour permettre le calcul du nombre de clauses instanciees sans depliage */
    rav2_Arho_nb [main_pid] [rho [1]]++;

#if is_sdag
    if (forest_level & FL_supertagger || forest_level & FL_source) {
      /* main_clause est global */
      /* Ici, on a main_clause < 0 */
      /* Le cas main_clause == 0 correspond a une compilation avec l'option -factorize de la rcg qui necessite
	 un depliage complet.  Ce cas est traite par "!is_rav2_coupled_and_not_FL_rhs" */
      main_clause = -main_clause;
      fill_out_source (G, gclause, rho+2+G->clause2son_nb [gclause]);
    }
#endif /* is_sdag */
  }

  return 1;
}
#endif /* is_rav2 || is_rav22 */

#if is_shallow
/* Contient les grammaires des parsers guide's */
static	struct G 	*Guided_Gs [PID];
static struct G		*Guide_Gs [PID], *Parse_Gs [PID];
static SXBA 		coupled_used_clause [PID];
static SXBA 		shallow_used_clause [PID];

static SXINT		lhs_nt, *ij_stack, *gclause2next;
static XH_header	XHip_hd;

static XxY_header	AijXclause_shlw;
static SXINT		AijXclause_shlw_Xassoc, AijXclause_shlw_Yassoc;
#if PID>1
static SXINT		pidXAijXclause_shlw_assoc [6];
static XxYxZ_header	pidXAijXclause_shlw;
#endif

static struct G		*G /* Shallow Grammaire guide'e */,
                        *gG /* grammaire 1rcg du guide */,
                        *pG /* grammaire ayant realise' l'analyse */,
                        *rG /* grammaire d'origine */ ;

static XxYxZ_header	shallow_ff [PID]; /* (index, clause, t) */
static SXINT		shallow_ff_assoc [6] = {0, 1, 0, 0, 0, 0}; /* XxYxZ_Yforeach */

static void shallow_mem_mngr (struct G *gG /* grammaire 1rcg du guide */, SXINT old_size, SXINT new_size)
{
    SXINT 	pid = gG->pid;

    if (old_size == 0) {
	/* ALLOCATION */
	coupled_used_clause [pid] = sxba_calloc (gG->clause_nb+1);

	if (forest_level & FL_source) {
	    XxYxZ_alloc (&(shallow_ff [pid]), "shallow_ff", gG->clause_nb+1, 1, shallow_ff_assoc, NULL, statistics);
	}

	if ((forest_level & FL_lhs_prdct) && pid == 0) {
	    /* Commun a tous les pid */
#if PID==1
	    AijXclause_shlw_Yassoc = 1; /* XxY_Yforeach sur AijXclause_shlw */
#else
	    AijXclause_shlw_Yassoc = 1; /* XxY_Yforeach sur AijXclause_shlw */
	    pidXAijXclause_shlw_assoc [4] = 1; /* XxYxZ_XZforeach sur pidXAijXclause_shlw */
#endif

	    XxY_alloc (&AijXclause_shlw, "AijXclause_shlw", new_size+1, 1, AijXclause_shlw_Xassoc, AijXclause_shlw_Yassoc,
		       NULL, statistics);
#if PID>1
	    XxYxZ_alloc (&pidXAijXclause_shlw, "pidXAijXclause_shlw", PID*new_size+1, 1, pidXAijXclause_shlw_assoc, NULL, statistics);
#endif
	}
    }
    else {
	if (new_size == 0) {
	    /* FREE */
	    sxfree (coupled_used_clause [pid]), coupled_used_clause [pid] = NULL;

	    if (XxYxZ_is_allocated (shallow_ff [pid]))
		XxYxZ_free (&(shallow_ff [pid]));

	    if (pid == 0) {
		/* Commun a tous les pid */
		if (XxY_is_allocated (AijXclause_shlw))
		    XxY_free (&AijXclause_shlw);

#if PID>1
		if (XxY_is_allocated (pidXAijXclause_shlw))
		    XxY_free (&pidXAijXclause_shlw);
#endif
	    }
	}
	else {
	    /* REALLOCATION */
	}
    }
}


static void
shallow_loop (struct G *G, SXINT *rho, SXINT cycle_kind)
{
    /* Arho = rho [1]
       Le calcul de toutes les instantiations des Arho-clauses est termine, au moins une
       clause valide est impliquee ds un cycle (ou une boucle) */
    /* rho [0] est une A-clause */

    /* G->pass_no == 1 */
    process_loop (G, rho, shallow_action, cycle_kind);
}



static SXINT
shallow_action (struct G *gG /* grammaire 1rcg du guide */, SXINT *rho)
{
    /* On vient d'effectuer une reduction ds la forme 1rcg */
    SXINT		clause, item, pid = gG->pid, eq_clause, cur, bot, top;
    SXBA	set;
    BOOLEAN	has_identical;

#if is_sdag
    SXINT		*tbot, *ttop, x, y, index, t, triple;
#endif

    clause = *rho;

#if EBUG3
    print_instantiated_clause (gG, rho, gG->name, TRUE, TRUE, TRUE);
#endif

    *rho++;

    has_identical = (gG->clause2identical_disp != NULL &&
	((bot = gG->clause2identical_disp [clause]) < (top = gG->clause2identical_disp [clause+1])));

    icnb++;

    if (has_identical)
	icnb += top-bot;

#if EBUG2
    instantiated_clause_nb++;

    if (has_identical)
	instantiated_clause_nb += top-bot;
#endif

    if (set = coupled_used_clause [pid])
	SXBA_1_bit (set, clause);

    if (XxY_is_allocated (AijXclause_shlw) > 0) {
	eq_clause = clause;
	cur = bot;

	do {
#if PID==1
	    XxY_set (&AijXclause_shlw, *rho, eq_clause, &item);
#else
	    if (pid == 0) {
		XxY_set (&AijXclause_shlw, *rho, eq_clause, &item);
	    }
	    else {
		XxYxZ_set (&pidXAijXclause_shlw, pid, *rho, eq_clause, &item);
	    }
#endif

	    eq_clause = (has_identical) ? ((cur < top) ? gG->clause2identical [cur++] : 0) : 0;
	} while (eq_clause);
    }

#if is_sdag
    /* On recherche les source index de tous les terminaux de la clause pour reconstituer
       un sdag reduit */
    if (gG->clause2lhst_disp && XxYxZ_is_allocated (shallow_ff [pid])) {
	if ((y=gG->clause2lhst_disp [clause+1])-(x=gG->clause2lhst_disp [clause])) {
	    rho += gG->clause2son_nb [clause]+1;

	    tbot = gG->lhst_list+x;
	    ttop = gG->lhst_list+y;

	    while (tbot < ttop) {
		index = *rho++;
		t = *tbot++;
		XxYxZ_set (&(shallow_ff [pid]), index, clause, t, &triple);
		
		if (has_identical) {
		    cur = bot;

		    while (cur < top) {
			eq_clause = gG->clause2identical [cur++];
			XxYxZ_set (&(shallow_ff [pid]), index, eq_clause, t, &triple);
		    }
		}
	    }
	}
    }
#endif /* is_sdag */

    return 1;
}


static void
shallow_Aij (SXINT gclause)
{
    SXINT 	x, Aij, ij, i, j, pid, index, item;
    BOOLEAN	is_empty;
#if is_sequential
    BOOLEAN	sequence;
    SXINT		prevj;
#endif

    pid = G->pid;

    if (gclause == 0) {
	/* ij_stack contient tous les ranges */
	/* Ds le cas shallow (ouarf ouarf), on ne sort pas les clauses => les predicats instancies
	   des lhs peuvent donc etre identiques pour differentes Aclauses */

	XH_push (XHip_hd, lhs_nt);

	is_empty = TRUE;
#if is_sequential
	sequence = TRUE;
	prevj = 0;
#endif

	for (x = ij_stack [0]; x > 0; x--) {
	    ij = ij_stack [x];
	    i = ij2i (ij), j = ij2j (ij);

	    if (i != j)
		is_empty = FALSE;
#if is_sequential
	    /* Les ranges des args ne peuvent etre decroissants (cas des TAG) */
	    if (i < prevj)
		sequence = FALSE;

	    prevj = j;
#endif
	    XH_push (XHip_hd, ij);
	}

	if (is_empty
#if is_sequential
	    || !sequence
#endif
	    )
	    XH_raz (XHip_hd);
	else
	    XH_set (&XHip_hd, &index);
    }
    else {
	if (pid == 0) {
	    XxY_Yforeach (AijXclause_shlw, gclause, item) {
		Aij = XxY_X (AijXclause_shlw, item);
		ij = (X_X (*(pG->prhoA_hd), Aij))>>pG->lognt;
		/* On obtient les args ds l'ordre arity-1, ..., 0 */
		ij_stack [++*ij_stack] = ij;
		shallow_Aij (gclause2next [gclause]);
		--*ij_stack;
	    }
	}
#if PID>1
	else {
	    XxYxZ_XZforeach (pidXAijXclause_shlw, pid, gclause, item) {
		Aij = XxYxZ_Y (pidXAijXclause_shlw, item);
		ij = (X_X (*(pG->prhoA_hd), Aij))>>pG->lognt;
		/* On obtient les args ds l'ordre arity-1, ..., 0 */
		ij_stack [++*ij_stack] = ij;
		shallow_Aij (gclause2next [gclause]);
		--*ij_stack;
	    }
	}
#endif /* PID>1 */
    }
}


static void
fill_out_source_from_shallow (SXINT pid, SXINT gclause, SXINT clause)
{
    SXINT triple, index, t;
    
    XxYxZ_Yforeach (shallow_ff [pid], gclause, triple) {
	index = XxYxZ_X (shallow_ff [pid], triple);
	t = XxYxZ_Z (shallow_ff [pid], triple);
	SXBA_1_bit (glbl_out_source [index], t);
	/* Pour out_ff2clause_set, ds le cas mapping inverse, il faut transformer clause
	   en la liste des clauses de la grammaire d'origine (a clause peut correspondre
	   plusieurs clauses de la grammaire d'origine si clause est une clause secondaire
	   construite par l'optimiseur et si on a utilise l'option -factorize) */
	SXBA_1_bit (out_ff2clause_set [pid] [index], clause);
    }
}


static void
shallow_mngr (SXINT pid)
{
    /* Effectue la correspondance entre la forme
       2var et la grammaire superficielle */
    SXINT 	clause, gclause, pGclause, gGclause, Gclause, rGclause, rclause, eq_clause, arity, bot, top, bot1, top1, x, son, ij;
    SXINT		gGbot, gGtop, Gbot, Gtop, pGbot, pGtop, andk_filter;
    SXINT		*kclauses_nb, *clause2gclause;
    SXBA	gclause_set, clause_set, shallow_clause_set;
    BOOLEAN	hasgG_identical, hasG_identical, haspG_identical;

    /* Les pid des guides ou des parsers guides sont identiques */
    /* G = Shallow Grammaire guide'e */
    /* gG = grammaire 1rcg du guide */
    /* rG = grammaire d'origine si is_srav22 sinon == G */
    /* pG = grammaire d'analyse */

    kclauses_nb = (SXINT*) sxcalloc (rG->clause_nb+1, sizeof (SXINT));	
    clause2gclause = (SXINT*) sxcalloc (rG->clause_nb+1, sizeof (SXINT));	
    gclause2next = (SXINT*) sxalloc (pG->clause_nb+1, sizeof (SXINT));	
	
    gclause_set = coupled_used_clause [pid];
    clause_set = used_clause [pid];
    shallow_clause_set = shallow_used_clause [pid];

    ij_stack = (SXINT*) sxalloc (rG->arity+1, sizeof (SXINT));
    ij_stack [0] = 0;

    andk_filter = (1<<sxlast_bit (G->clause_nb))-1;

    gclause = 0;

    while ((gclause = sxba_scan (gclause_set, gclause)) > 0) {
	/* On suppose que l'ordre des gclauses ds gclause_set correspondant a une meme clause
	   correspond aux arguments 0, 1, 2, ... de clause */
	pGclause = gclause;
	haspG_identical = (pG->clause2identical_disp != NULL &&
			   ((pGbot = pG->clause2identical_disp [pGclause]) < (pGtop = pG->clause2identical_disp [pGclause+1])));
	
	do {
#if is_srav2
	    gGclause = gG->from2var2clause [pGclause];

	    if (gGclause > 0)
	      hasgG_identical = (gG->clause2identical_disp != NULL &&
				 ((gGbot = gG->clause2identical_disp [gGclause]) < (gGtop = gG->clause2identical_disp [gGclause+1])));
	    else
	      gGclause = 0;

	    while (gGclause > 0) {
	
#else
		gGclause = pGclause;
#endif

		rGclause = Gclause = G->guide_clause2clause [gGclause] & andk_filter;
#if is_srav22
		hasG_identical = (G->clause2identical_disp != NULL &&
				  ((Gbot = G->clause2identical_disp [Gclause]) < (Gtop = G->clause2identical_disp [Gclause+1])));

		    while (Gclause > 0) {
			rGclause = rG->from2var2clause [Gclause];

			if (rGclause > 0) {
#endif
			    gclause2next [pGclause] = clause2gclause [rGclause];

			    lhs_nt = rG->clause2lhs_nt [rGclause];
			    arity = rG->nt2arity [lhs_nt];

			    /* On valide une clause ssi toutes ses sous-clauses sont ds le guide */
			    if (++kclauses_nb [rGclause] == arity) {
				/* clause est un candidat */
				if (SXBA_bit_is_reset_set (shallow_clause_set, rGclause)) {
				    /* Les clauses identiques, n'ont pas d'influence sur les predicats instancies en LHS */
				    /* Ds le cas shallow, on ne sort pas les no des clauses */
				    /* On se contente donc de completer clause_set */
				    if (clause_set)
				        SXBA_1_bit (clause_set, rGclause);

				    if (rG->clause2identical_disp != NULL &&
					((bot = rG->clause2identical_disp [rGclause]) < (top = rG->clause2identical_disp [rGclause+1]))) {
					while (bot < top) {
					    eq_clause = rG->clause2identical [bot];
					    SXBA_1_bit (shallow_clause_set, eq_clause);

					    if (clause_set)
					        SXBA_1_bit (clause_set, eq_clause);

					    bot++;
					}
				    }
				}

				if (XxY_is_allocated (AijXclause_shlw)) {
#if is_TAG
				    if (rGclause != 1 && arity == 1)
#endif
					shallow_Aij (pGclause);
				}
			    }
			    else
				clause2gclause [rGclause] = pGclause;
#if is_srav22
			}

			Gclause = hasG_identical ? (Gbot < Gtop ? G->clause2identical [Gbot++] : 0) : 0;
		    }
#endif

#if is_srav2
		gGclause = hasgG_identical ? (gGbot < gGtop ? gG->clause2identical [gGbot++] : 0) : 0;
	    }
#endif

	    pGclause = haspG_identical ? (pGbot < pGtop ? pG->clause2identical [pGbot++] : 0) : 0;
	} while (pGclause > 0);
    }

    if (XxYxZ_is_allocated (shallow_ff [pid])) {
	gclause = 0;

	while ((gclause = sxba_scan (gclause_set, gclause)) > 0) {
	    pGclause = gclause;
	    haspG_identical = (pG->clause2identical_disp != NULL &&
			       ((pGbot = pG->clause2identical_disp [pGclause]) < (pGtop = pG->clause2identical_disp [pGclause+1])));
	
	    do {

#if is_srav2
		bot1 = gG->from2var2clause_disp [pGclause];
		top1 = gG->from2var2clause_disp [pGclause+1];

		while (bot1 < top1) {
		    clause = gG->from2var2clause_list [bot1];

		    clause = G->guide_clause2clause [clause] & andk_filter;
#if is_srav22
		    bot = rG->from2var2clause_disp [clause];
		    top = rG->from2var2clause_disp [clause+1];

		    while (bot < top) {
			clause = rG->from2var2clause_list [bot];

			if (SXBA_bit_is_set (shallow_clause_set, clause)) {
			    /* pGclause "participe" a clause */
			    fill_out_source_from_shallow (pid, pGclause, clause);
			}

			bot++;
		    }
#else
		    if (SXBA_bit_is_set (shallow_clause_set, clause)) {
			/* pGclause "participe" a clause */
			fill_out_source_from_shallow (pid, pGclause, clause);
		    }
#endif /* is_srav22 */

		    bot1++;
		}
#else /* !is_rav2 */
		clause = G->guide_clause2clause [pGclause] & andk_filter;

#if is_srav22
		bot = rG->from2var2clause_disp [clause];
		top = rG->from2var2clause_disp [clause+1];

		while (bot < top) {
		    rclause = rG->from2var2clause_list [bot];

		    if (SXBA_bit_is_set (shallow_clause_set, rclause)) {
			/* pGclause "participe" a rclause */
			fill_out_source_from_shallow (pid, pGclause, rclause);
		    }

		    bot++;
		}
#else
		if (SXBA_bit_is_set (shallow_clause_set, clause)) {
		    /* pGclause "participe" a clause */
		    fill_out_source_from_shallow (pid, pGclause, clause);
		}
#endif /* is_srav22 */
#endif /* !is_srav2 */

		pGclause = haspG_identical ? (pGbot < pGtop ? pG->clause2identical [pGbot++] : 0) : 0;
	    } while (pGclause > 0);
	}
    }

    if (XH_is_allocated (XHip_hd)) {
	/* On sort le contenu de XHip_hd */
	top = XH_top (XHip_hd);

	if (top > 1) {
	    /* Il peut y avoir un mapping inverse ... */
	    /* ... mais il a conserve les codes des predicats */
	    for (bot = 1; bot < top; bot++) {
		x = XH_X (XHip_hd, bot);
		lhs_nt = XH_list_elem (XHip_hd, x++);
		arity = rG->nt2arity [lhs_nt];
		printf ("%s(", rG->nt2str [lhs_nt]);

		for (son = 1; son <= arity; son++) {
		    if (son > 1)
			fputs (", ", stdout);

		    ij = XH_list_elem (XHip_hd, x++);
		    printf ("%i..%i", ij2i (ij), ij2j (ij));
		}
	    
		fputs (")\n", stdout);
	    }

	    XH_clear (&XHip_hd);
	}
    }

    sxfree (kclauses_nb);
    sxfree (gclause2next);
    sxfree (clause2gclause);
    sxfree (ij_stack);
}

static SXINT
shallow_last_pass (struct G **Gs, SXINT S0n, SXINT ret_val)
{
    SXINT size, pid;

    if (ret_val) {
	for (pid = 0; pid < PID; pid++) {
	    Parse_Gs [pid] = Guide_Gs [pid] = Gs [pid]; /* grammaire d'analyse et grammaire 1rcg du guide */
	}

#if is_srav2
	/* Il peut y avoir un mapping inverse ... */
	global_pid = 0;
	INTERFACE_RAV2;

	for (pid = 0; pid < PID; pid++) {
	    Guide_Gs [pid] = Gs [pid]; /* grammaire 1rcg du guide */
	}
#endif /* is_srav22 */

	global_pid = 0;
	INTERFACE2;

#if PID>1
	for (pid = 0; pid < PID; pid++) {
	    Guided_Gs [pid] = Gs [pid]; /* grammaire guidee */
	    Guided_Gs [pid]->Gs = &(Guided_Gs [0]);
	}

	check_combinatorial_options (Guided_Gs [0]);
#else
	Guided_Gs [0] = Gs [0]; /* grammaire guidee */
	Guided_Gs [0]->Gs = &(Guided_Gs [0]);
#endif

#if is_srav22
	global_pid = 0;
	INTERFACE2_RAV2;
#endif /* is_srav22 */

	size = X_size (*(Parse_Gs [0]->prhoA_hd));

	if ((forest_level & FL_lhs_prdct) && !XH_is_allocated (XHip_hd)) {
	    XH_alloc (&XHip_hd, "XHip_hd", size+1, 1, Guided_Gs [0]->arity+1, NULL, statistics);
	}

	for (pid = 0; pid < PID; pid++) {
	    /* Les pid des guides ou des parsers guides sont identiques */
	    G = Guided_Gs [pid]; /* Shallow Grammaire guide'e */
	    pG = Parse_Gs [pid]; /* grammaire d'analyse */
#if is_srav2
	    gG = Guide_Gs [pid]; /* grammaire 1rcg du guide */
#else
	    gG = pG;	/* grammaire 1rcg du guide */
#endif

	    
#if is_srav22
	    rG = Gs [pid];	/* grammaire d'origine */
#else
	    rG = G;	/* grammaire d'origine */
#endif

	    (*(rG->semact.first_pass_init))(rG, 0, size);

	    /* Ici, on simule les reductions, comme si c'etait le shallow parser guide' qui les avait faites... */
	    shallow_used_clause [pid] = sxba_calloc (rG->clause_nb+1);

	    shallow_mngr (pid);

	    sxfree (shallow_used_clause [pid]);
	}


	if (XH_is_allocated (XHip_hd)) {
	    XH_free (&XHip_hd);
	}

	rG = Gs [0];	/* grammaire d'origine */
	/* ... parse_forest */
	ret_val = (*(rG->semact.last_pass)) (rG->Gs, S0n, ret_val);
							
	for (pid = 0; pid < PID; pid++) {
	    rG = Gs [pid];	/* grammaire d'origine */
	    (*(rG->semact.first_pass_final))(rG, 1, 0);
	}
    }

    return ret_val;
}
#endif /* is_shallow */

#if 0
void print_ic (struct G *G, SXINT *rho)
{
    print_instantiated_clause (G, rho, G->name, TRUE, TRUE, TRUE);
}
#endif

#if is_parse_forest
#ifdef SYNCHRONOUS
/* Essai le 03/03/08 */
static void
synchro_create_sub_fsa (SXINT t_code, struct synchro_pq *pq)
{
  SXINT transition;

  if (pq->p == 0)
    pq->p = ++cur_synchro_fsa_state;

  if (pq->q == 0)
    pq->q = ++cur_synchro_fsa_state;

  XxYxZ_set (&synchro_fsa, pq->p, t_code, pq->q, &transition);
}

static void
process_synchronous (struct G *G, SXINT *rho)
{
  SXINT                 clause, son_nb, A, Aij, rhoi, lhs_pid, son_pid, arity, son, bot, top, lhs_arity, lhs_arg, arg, lhs, pq_disp, lhs_arg_nb, arg_pos, symb_nb, symb;
  SXINT                 rhs, synchro_son_nb, son_Aij, prdct_name, left_arg_pos;
  SXINT                 *sons, *symbs, *symbs_lim, *lhs_args,*rhs_prdct_ptr;
  BOOLEAN               is_first_synchro_Aij;
  struct Aij_struct	*Aij_struct_ptr;
  struct G              *synchro_G;
  struct synchro_pq     synchro_pq, *synchro_pq_ptr, *pq_ptr;

  clause = *rho++;

  if (clause <= 0)
    sxtrap (ME, "process_synchronous");

  A = G->clause2lhs_nt [clause];

  son_nb = G->clause2son_nb [clause];
  Aij = *rho++; /* On va associer le "resultat" de la synchronisation a Aij */

  lhs_pid = G->pid; /* On suppose que les 2 grammaires ont meme pid */
  synchro_G = synchro_Gs [lhs_pid];
  lhs_arity = synchro_G->nt2arity [A];

  rhs = G->clause2rhs [clause];

  if (rhs) {
    bot = synchro_G->rhs2prdcts_disp [rhs];
    top = synchro_G->rhs2prdcts_disp [rhs+1];
    synchro_son_nb = top-bot;
  }
  else
    synchro_son_nb = 0;

  /* Il se peut que son_nb et synchro_son_nb soient differents a cause des appels de predicats systeme */
  /* Pour l'instant ds l'essai, on a son_nb == synchro_son_nb */
  if (son_nb != synchro_son_nb)
    sxtrap (ME, "process_synchronous");

  if (son_nb > 0) {
    sons = G->clause2son_pid [clause]+1;

    for (son = 0; son < son_nb; son++, bot++) {
      son_Aij = *rho++; /* Pour ce fils, le "resultat" de la synchronisation est associe' a` rhoi */

      if (rhoi < 0)
	/* neg call !! */
	sxtrap (ME, "process_synchronous");

      son_pid = *sons++;

      if (son_pid < 0)
	sxtrap (ME, "process_synchronous");

      synchro_G = synchro_Gs [son_pid];

      if (son_pid != synchro_G->pid)
	/* pid differents !! */
	sxtrap (ME, "process_synchronous");
		    
      Aij_struct_ptr = (*(G->Gs [son_pid]->Aij2struct))(son_Aij);
      
      prdct_name = Aij_struct_ptr->A;
      arity = synchro_G->nt2arity [prdct_name];
      pq_disp = synchro_Aij2pq_disp [son_pid][son_Aij];

      if (pq_disp == 0)
	sxtrap (ME, "process_synchronous");

      synchro_pq_ptr = Aij_arg2synchro_pq + pq_disp;

      rhs_prdct_ptr = synchro_G->prdct2args + synchro_G->rhs2prdcts [bot];

      if (*rhs_prdct_ptr++ != 0)
	/* Appel negatif */
	sxtrap (ME, "process_synchronous");

      if (prdct_name != *rhs_prdct_ptr++)
	/* Les noms des predicats ds la grammaire G et synchro_G different */
	sxtrap (ME, "process_synchronous");
	
      for (arg_pos = 0; arg_pos < arity; arg_pos++) {
	/* A quelle variable correspond arg_pos (la RCG est non combinatoire) */
	arg = synchro_G->prdct2args [*rhs_prdct_ptr++];

	left_arg_pos = synchro_G->args2symbs_disp [arg];

	if (left_arg_pos+1 != synchro_G->args2symbs_disp [arg+1])
	  /* L'arg n'est pas de taille 1 */
	  sxtrap (ME, "process_synchronous");
	    
	symb = synchro_G->args2symbs [left_arg_pos];

	if (symb < 0)
	  /* Un terminal */
	  sxtrap (ME, "process_synchronous");

	if ((pq_ptr = symb2synchro_pq_ptr [symb]) == NULL) {
	  /* 1ere fois qu'on tombe sur X en RHS */
	  symb2synchro_pq_ptr [symb] = synchro_pq_ptr;
	}
	else {
	  /* La syncho_G est non lineaire, il faut faire l'intersection */
	  /* On met le resultat ds pq_ptr */
	  synchro_and_fsa (synchro_pq_ptr, pq_ptr);
	}
	  
	synchro_pq_ptr++;
      }
    }
  }

  is_first_synchro_Aij = (pq_disp = synchro_Aij2pq_disp [lhs_pid][Aij]) == 0;

  if (is_first_synchro_Aij) {
    pq_disp = synchro_Aij2pq_disp [lhs_pid] [Aij] = synchro_Aij2pq_top;
    synchro_Aij2pq_top += lhs_arity;
  }

  synchro_pq_ptr = Aij_arg2synchro_pq + pq_disp;

  lhs = synchro_G->clause2lhs [clause];
  lhs_args = synchro_G->prdct2args + synchro_G->prdct2args_disp [lhs] + 1;
  lhs_arg_nb = synchro_G->nt2arity [*lhs_args++];

  if (lhs_arity != lhs_arg_nb)
    sxtrap (ME, "process_synchronous");

  for (arg_pos = 0; arg_pos < lhs_arg_nb; arg_pos++) {
    lhs_arg = *lhs_args++;
    symbs = synchro_G->args2symbs + synchro_G->args2symbs_disp [lhs_arg];
    symbs_lim = synchro_G->args2symbs + synchro_G->args2symbs_disp [lhs_arg+1];

    if ((symb_nb = symbs_lim - symbs) > 0) {
      symb = *symbs++;

      if (symb < 0) {
	symb = -symb;
	synchro_pq = empty_synchro_pq;
	synchro_create_sub_fsa (symb, &synchro_pq);
      }
      else {
	synchro_pq_ptr = symb2synchro_pq_ptr [symb];
	  
	if (synchro_pq_ptr == NULL)
	  sxtrap (ME, "process_synchronous");
	    
	synchro_pq = *synchro_pq_ptr;
      }

       while (--symb_nb > 0) {
	 /* Le resultat precedent est ds synchro_pq */
	symb = *symbs++;

	if (symb < 0) {
	  /* C'est un terminal */
	  symb = -symb;

	  synchro_catenate_symb (&synchro_pq, symb);
	}
	else {
	  /* C'est une variable */
	  synchro_pq_ptr = symb2synchro_pq_ptr [symb];

	  synchro_catenate_sub_fsa (&synchro_pq, synchro_pq_ptr);
	}
      }
    }
    else {
      /* argument vide */
      synchro_create_sub_fsa (0, &synchro_pq);
    }

    if (is_first_synchro_Aij)
      synchro_pq_ptr [arg_pos] = synchro_pq;
    else {
      synchro_or_sub_fsa (&(synchro_pq_ptr [arg_pos]), synchro_pq);
    }
  }
}
#endif /* SYNCHRONOUS */
#endif /* is_parse_forest */
