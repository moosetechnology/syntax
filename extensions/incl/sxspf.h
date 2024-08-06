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
#ifndef sxspf_h
#define sxspf_h
#include "SXante.h"


#include "XH.h"
#include "XxYxZ.h"

#define MAKE_Aij(Aij,A,i,j)	(XxYxZ_set (&spf.outputG.Axixj2Aij_hd, A, i, j, &(Aij)))
#define GET_Aij(A,i,j)	        (XxYxZ_is_set (&spf.outputG.Axixj2Aij_hd, A, i, j))
#define Aij2A(Aij)               XxYxZ_X (spf.outputG.Axixj2Aij_hd, Aij)
#define Aij2i(Aij)               XxYxZ_Y (spf.outputG.Axixj2Aij_hd, Aij)
#define Aij2j(Aij)               XxYxZ_Z (spf.outputG.Axixj2Aij_hd, Aij)

#define MAKE_Tij(Tij,T,i,j)	(XxYxZ_set (&spf.outputG.Txixj2Tij_hd, T, i, j, &(Tij)))
#define GET_Tij(T,i,j)	        (XxYxZ_is_set (&spf.outputG.Txixj2Tij_hd, T, i, j))
#define Tij2T(Tij)               XxYxZ_X (spf.outputG.Txixj2Tij_hd, Tij)
#define Tij2i(Tij)               XxYxZ_Y (spf.outputG.Txixj2Tij_hd, Tij)
#define Tij2j(Tij)               XxYxZ_Z (spf.outputG.Txixj2Tij_hd, Tij)
#define Tij_iforeach(i,Tij)      XxYxZ_Yforeach (spf.outputG.Txixj2Tij_hd, i, Tij)

/* les lispro [item] sont de la forme Aij (nonterminaux) ou -Tpq (terminaux)
   Les Aij sont stockes ds Axixj2Aij_hd (A, i, j) et manipules par les macros "Aij"
   si 1 <= Aij <= maxnt c'est un nt ordinaire 1 <= i <= j <= n+1
   si maxnt < Aij <= maxxnt c'est un nt d'erreur (l'un au moins des i, j vaut -1 (i.e., inconnu) et
   il n'y a aucune Aij_regle mais Aij2rhs_nb est positionne'.
   Les Tpq sont stockes ds Txixj2Tij_hd (-t, p, q) et manipules par les macros "Tij"
   On a 1 <= Tpq <= -maxt Les p, q peuvent valoir -1 si le terminal est produit par le traitement d'erreur */
   
struct spf /* shared_parse_forest */ {
  struct inputG {
    SXUINT modif_time /* bnf_modif_time */;
    SXINT		      maxprod, maxt, maxnt, maxitem, maxrhsnt, maxrhs;
    SXINT               *lispro, *prolis, *prolon, *lhs, *npd, *numpd, *semact, *constraints, *prdct, *prod2nbnt, *npg, *numpg;
    char              **ntstring, **tstring;
    SXBA              BVIDE;
    bool	      has_cycles;
  } inputG;
  
  struct outputG {
    SXINT	maxprod, maxxprod, maxt, maxnt, maxxnt, maxitem, start_symbol, Tpq_repair_nb, repair_mlstn_top;
    bool     is_proper, has_repair, is_error_detected;

    struct rhs {
      SXINT	lispro, prolis;
    } *rhs;

    struct lhs {
      SXINT	prolon, lhs, init_prod;
    } *lhs;

    SXINT        *Aij2rhs_nb;

    XxYxZ_header Axixj2Aij_hd, Txixj2Tij_hd;

    SXBA        Tpq_rcvr_set; /* Ensemble des Tpq de la recup (ou de la reparation) */
    SXBA        non_productive_Aij_rcvr_set; /* Aij de la recup qui ne sont pas definies par des xprod */

    /* Le 14/02/08 */
    SXINT       *Tij2tok_no_stack; /* Associe a chaque Tij (meme un repair) un index ds *Tij2tok_no_stack qui repere une pile de tokens */
    SXINT       *tok_no_stack; /* pile de piles de tok_no (voir ci_dessus) */
    SXINT       *tok_no2some_Tij; /* associe à un tok_no l'un des Tij qui a ce tok_no parmi ses tok_no */
    char        **Tij2comment; /* Acces aux commentaires associes aux Tij, si Tij2comment==NULL ou Tij2comment[Tij]==NULL,
				  on va prendre le commentaire de d'un des tok de Tij */
#if 0
    /* Le 14/02/08 */
    SXINT         *Tij2tok_no; /* Associe a chaque Tij (meme un repair) un seul tok_no ou un pointeur vers une liste dans le XH ci-dessous */
    XH_header     Tij2XH_tok_no; /* Associe a chaque Tij (meme un repair) plusieurs tok_no */
    SXINT         *tok_no2some_Tij; /* associe à un tok_no l'un des Tij qui a ce tok_no parmi ses tok_no */
    char          **Tij2comment; /* Acces aux commentaires associes aux Tij, si Tij2comment==NULL ou Tij2comment[Tij]==NULL,
				  on va prendre le commentaire de d'un des tok de Tij */
#endif /* 0 */
    SXINT         *xPij2lgth; /* pour les xprod, indexe' par Pij-spf.outputG.maxprod, donne la longueur du genere' */
    SXINT         *Pij2eval; /* fait correspondre a Pij l'entier eval, 0 <= eval < 2^k, si k est le nb de nt =>+ eps en rhs */
  } outputG;

  struct tree_count {
    double *prod2nb, *nt2nb;
  } tree_count;

  /* Variables de spf_topological_walk () */
  struct walk {
    SXINT         prev_Aij /* la lhs de la derniere prod instanciee visitee */
    , prev_td_Aij /* variable locale */
      , prev_bu_Aij /* variable locale */
      , cur_Aij /* la lhs de la prod instanciee courante visitee */
      , cur_Pij /* la prod instanciee courante visitee */
      , Aij_top /* maxxnt ou maxnt suivant l'existence des xprod */
      ;
    SXINT         *Aij2nyp_nb /* variable locale */
    , *Aij_stack /* variable locale */
      ;
    SXBA        Aij_set /* variable locale */
    , cyclic_Aij_set /* variable locale */
      , invalid_Aij_set /* Contient les Aij invalides */
      , status /* Contient le resultat de spf_save_status */
      ; 
    SXINT 	(*pass_inherited) (SXINT) /* variable locale */
	 , (*pass_derived) (SXINT) /* variable locale */
	 , (*pre_pass) (SXINT) /* variable locale */
	 , (*post_pass) (SXINT, bool) /* variable locale */
	 , (*user_filtering_function) (SXINT Pij) /* filtre supplementaire utilisateur sur les Pij */
	 ;
  } walk;
	 
  /* Grammaire propre construite dynamiquement par lexicalizer_mngr apres filtrage de la grammaire inputG par le source */
  /* Cas de l'option "-h" de bnf ou compilation de lexicalizer_mngr.c avec l'option -DMAKE_INSIDEG */
  struct insideG {
    SXUINT        modif_time;
    SXINT         maxprod, maxt, maxnt, maxitem, maxrhsnt, maxrhs, sizeofnumpd, sizeoftnumpd;
    SXINT         *lispro, *prolis, *prolon, *lhs, *t2init_t, *nt2init_nt, *prod2init_prod, *npg, *numpg, *npd, *numpd, *tpd, *tnumpd;
    SXINT         *rhs_nt2where, *lhs_nt2where;
    SXINT         *nt2min_gen_lgth /* nt2min_gen_lgth [A] = l, l=min(x), A =>+ x et l=|x| */ /* Rempli par fill_nt2min_gen_lgth () si on en a besoin !! */;
    SXBA              
      bvide /* bvide = {A | A =>+ \epsilon} */,
      titem_set /* titem_set = { A -> \alpha . t \beta } */,
      rc_titem_set /* rc_titem_set = { A -> \alpha t1 . \beta } */,
      empty_prod_item_set /* empty_prod_item_set = {A -> \alpha . \beta | \alpha \beta =>* \epsilon} */,
      *left_corner /* left_corner = {(A, B) | A =>* \alpha B \beta =>* B \beta} */,
      *nt2item_set /* nt2item_set [A] = {B -> \alpha . \beta | B left_corner A et \alpha =>* \epsilon} */,
      *right_corner /* right_corner = {(A, B) | A =>* \alpha B \beta =>* \alpha B} */,
      *nt2rc_item_set /* nt2rc_item_set [A] = {B -> \alpha . \beta | B right_corner A et \beta =>* \epsilon} */,
      *valid_prefixes /* valid_prefixes = {A -> \alpha . \beta | \alpha =>* \epsilon} */,
      *valid_suffixes /* valid_suffixes = {A -> \alpha . \beta | \beta =>* \epsilon} */,
      *t2prod_item_set /* t2prod_item_set [t] = { A -> \alpha . \beta | \alpha \beta == \gamma t \delta} */,
      *t2item_set /* t2item_set [t] = {A -> \alpha . \beta | t \in First1(\beta Follow1(A))} */ /* Rempli par fill_t2item_set () si on en a besoin !! */,
      left_recursive_set /* {A | A =>+ \alpha A \beta =>* A \beta} */,
      right_recursive_set /* {A | A =>+ \alpha A \beta =>* \alpha A} */
      ;
#if 0
    SXBA              
      inputG_source_set /* Le source_set d'origine */,
      *sdag_inputG_glbl_source /* ... et le glbl_source s le cas sdag */
      ;
    SXINT         *no_sdag_inputG_glbl_source /* ... et le glbl_source s le cas no_sdag */;
#endif /* 0 */
    
    bool           is_allocated, is_eps, is_cycle;
  } insideG;
};

#ifndef SX_DFN_EXT_VAR2
extern struct spf spf;
#else /* SX_DFN_EXT_VAR2 */
/* Cas de la definition et de l'initialisation de la structure externe spf
 a priori ds (earley|CYK)_parser.c */
struct spf spf;
struct inputG spf_inputG = {
#ifdef bnf_modif_time
    bnf_modif_time,
#else /* !bnf_modif_time */
    0,
#endif /* bnf_modif_time */
    prodmax,
    tmax,
    ntmax,
    itemmax,
    rhs_maxnt,
    rhs_lgth,

#ifdef def_lispro
    lispro,
#else /* !def_lispro */
    NULL,
#endif /* !def_lispro */

#ifdef def_prolis
    prolis,
#else /* !def_prolis */
    NULL,
#endif /* !def_prolis */

#ifdef def_prolon
    prolon,
#else /* !def_prolon */
    NULL,
#endif /* !def_prolon */

#ifdef def_lhs
    lhs,
#else /* !def_lhs */
    NULL,
#endif /* !def_lhs */

#ifdef def_npd
    npd,
#else /* !def_npd */
    NULL,
#endif /* !def_npd */

#ifdef def_numpd
    numpd,
#else /* !def_numpd */
    NULL,
#endif /* !def_numpd */

#ifdef def_semact
    semact,
#else /* !def_semact */
    NULL,
#endif /* !def_semact */

#ifdef is_constraints
    constraints,
#else /* !is_constraints */
    NULL,
#endif /* !is_constraints */ 

#ifdef is_prdct
    prdct,
#else /* !is_prdct */
    NULL,
#endif /* !is_prdct */ 

#ifdef def_prod2nbnt
    prod2nbnt,
#else /* !def_prod2nbnt */
    NULL,
#endif /* !def_prod2nbnt */

#ifdef def_npg
    npg,
#else /* !def_npg */
    NULL,
#endif /* !def_npg */

#ifdef def_numpg
    numpg,
#else /* !def_numpg */
    NULL,
#endif /* !def_numpg */

#ifdef def_ntstring
    ntstring,
#else /* !def_ntstring */
    NULL,
#endif /* !def_ntstring */

#ifdef def_tstring
    tstring,
#else /* !def_tstring */
    NULL,
#endif /* !def_tstring */

#ifdef is_epsilon
    BVIDE,
#else /* !is_epsilon */
    NULL,
#endif /* !is_epsilon */

#ifdef is_cyclic
    true,
#else /* !is_cyclic */
    false,
#endif /* !is_cyclic */
};

#endif /* SX_DFN_EXT_VAR2 */

#ifndef no_sxspf_function_declarations
/* Construction de la spf */
SX_GLOBAL_VAR void             spf_allocate_Aij (SXINT sxmaxnt, SXINT sxeof);
SX_GLOBAL_VAR void             spf_allocate_spf (SXINT sxmaxprod);
SX_GLOBAL_VAR void             spf_reallocate_lhs_rhs (SXINT lhs_size, SXINT rhs_size);
SX_GLOBAL_VAR void             spf_allocate_insideG (struct insideG *insideG_ptr, bool has_lex2, bool has_set_automaton);
SX_GLOBAL_VAR void             spf_finalize (void);
SX_GLOBAL_VAR void             spf_free (void);
SX_GLOBAL_VAR void             spf_free_insideG (struct insideG *insideG_ptr);
SX_GLOBAL_VAR void             spf_put_an_iprod (SXINT *stack);

/* Ecritures */
/* Ecriture d'une production instanciee */
SX_GLOBAL_VAR bool          spf_print_iprod (FILE *out_file, SXINT prod, char *prefix, char *suffix);
/* Ecriture d'un symbole instancie */
SX_GLOBAL_VAR void             spf_print_Xpq (FILE *out_file, SXINT Xpq);
/* Ecriture en top_down de la sous-foret ancree sur Aij */
SX_GLOBAL_VAR void             spf_print_td_forest (FILE *out_file, SXINT Aij); /* ... de Aij vers les feuilles */
/* Ecriture en bottom-up de la sous-foret ancree sur Aij */
SX_GLOBAL_VAR void             spf_print_bu_forest (FILE *out_file, SXINT Aij); /* ... des feuilles vers Aij */
/* Ecriture de la "taile" courante de la spf (dont le nombre d'arbres) */
SX_GLOBAL_VAR double           spf_print_signature (FILE *out_file, char *prefix_str); /* sort la taille actuelle de la foret et retourne le nb d'arbres */
/* Appelle spf_print_signature mais peut etre utilise' comme valeur de for_semact.sem_pass */
SX_GLOBAL_VAR SXINT              spf_print_tree_count (void);
/* Etablit la correspondance (spf.outputG.Tij2tok_no) entre chaque Tij (meme ceux
   de la reparation) et ses tok_no ; remplit aussi spf.outputG.Tij2comment
   si distribute_comment a ete appele */
/* Le parseur utilise' pour construire la spf doit fournir la fonction
   struct sxtoken *parser_Tpq2tok (SXINT Tpq) */
SX_GLOBAL_VAR void             spf_fill_Tij2tok_no (void);
/* Retourne une "stack" qui contient la liste des tok_no correspondant a Tij */
SX_GLOBAL_VAR SXINT            *spf_get_Tij2tok_no_stack (SXINT Tij);
/* concatène à la fin de wvstr la chaîne (DAG) représentant Tij */
#ifdef varstr_h
SX_GLOBAL_VAR VARSTR           spf_Tpq2varstr_catenate (VARSTR wvstr, SXINT Tij);
#endif /* varstr_h */
/* etablit la correspondance entre Pij et un entier qui indique si les nt en rhs
   qui peuvent deriver ds le vide, y derive effectivement ds Pij */
SX_GLOBAL_VAR void             spf_fill_Pij2eval (void);

/* Positionne ds count (et pour chaque Aij ds spf.tree_count.nt2nb [Aij]) le
   nombre d'arbres de la [sous-]foret partagee [de racine Aij] */
SX_GLOBAL_VAR bool          spf_tree_count (double *count);

/* Les parcours de la spf */
/* Ds le cas de la visite heritee (*pi)(Pij), si Pij est une Aij-iprod,
   on est su^r que :
      toutes les productions qui contiennent Aij en rhs ont deja ete appelees
      toutes les Aij-iprods sont appelees en sequence : si l'appel suivant est de
      la forme (*pi)(P'ij), alors soit P'ij est une Aij-iprod ou, si ce n'est
      pas le cas plus aucun des appels ulterieurs ne se fera avec une Aij-iprod */
/* Ds le cas de la visite synthetisee (*pd)(Pij), si Pij est une Aij-iprod,
   on est su^r que si Xpq est en rhs:
      toutes les Xpq-iprods ont deja ete appelees
      toutes les Aij-iprods sont appelees en sequence */
/* Attention soit pi, soit pd est non NULL (on ne peut par parcourir la foret haut-bas
   gauche-droit avec les contraintes precedentes) */
SX_GLOBAL_VAR bool          spf_topological_walk (SXINT root,
						     SXINT (*pi)(SXINT),
						     SXINT (*pd)(SXINT));
/* Permettent des pre et/ou des post visites sur les noeuds Aij des Aij-iprods */
SX_GLOBAL_VAR bool          spf_topological_top_down_walk (SXINT root,
							      SXINT (*pi)(SXINT),
							      SXINT (*pre_pass)(SXINT),
							      SXINT (*post_pass)(SXINT, bool));
SX_GLOBAL_VAR bool          spf_topological_bottom_up_walk (SXINT root,
							       SXINT (*pd)(SXINT),
							       SXINT (*pre_pass)(SXINT),
							       SXINT (*post_pass)(SXINT, bool));
/* Les appels des ft arg sont de la forme pi(Pij), pd(Pij), pre_p(Aij),
   post_p(Aij, kind) ou kind==false <=> toutes les Aij-prods ont ete supprimees */
/* Les valeurs entieres retournees par ces ft arg de l'utilisateur ont 3 formes :
   1  => rien (cas standard)
   0  => la production Pij est saute'e
   -1 => la production Pij est supprimee de la spf
*/

/* Elimine les iprods inaccessibles dues aux valeurs -1 retournees */
SX_GLOBAL_VAR bool          spf_make_proper (SXINT root_Aij);
/* Elimine de la foret les Pij de Pij_set */
SX_GLOBAL_VAR bool          spf_erase (SXBA Pij_set);

/* Permet de revenir a un etat precedant de la foret */
SX_GLOBAL_VAR void             spf_push_status (void); /* calcule l'etat courant de la spf et le sauve ds une SXBA locale (spf.walk.status) */
SX_GLOBAL_VAR void             spf_pop_status (void); /* Utilise spf.walk.status pour remettre la spf dans cet etat */
SX_GLOBAL_VAR void             spf_restore_status (SXBA save_set); /* copie save_set ds spf.walk.status */
SX_GLOBAL_VAR SXBA             spf_save_status (SXBA save_set) /* copie spf.walk.status dans le SXBA utilisateur save_set (qui peut etre NULL) */;

/* Deplie la foret */
/* Transforme la frontiere de la foret en un DAG minimal */
SX_GLOBAL_VAR void             spf_yield2dfa (bool which_form /* true => UDAG */);
SX_GLOBAL_VAR SXINT            spf_unfold (void);
SX_GLOBAL_VAR SXINT            spf_dag2tree (SXUINT p, SXINT Aij, SXINT (*f)(SXINT));
#endif /* no_sxspf_function_declarations */
#endif /* sxspf_h */
