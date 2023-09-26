#ifndef earley_h
#define earley_h
#include "SXante.h"

/* Le handleur positionne cette variable s'il est active' */
SX_GLOBAL_VAR BOOLEAN is_virtual_timeout_signal_raised;

/* EARLEY.h */
/* Le 11/08/2003, changement de nom de semact.h en earley.h */
/* Interface entre les analyseurs a la Earley et les actions semantiques. */

extern VOID		sxvoid ();
extern BOOLEAN		sxbvoid ();
extern int		sxivoid ();
extern void             sxearley_init_for_semact ();
extern BOOLEAN          sxearley_parse_it ();
/* typedef unsigned short	ushort; */
extern void             print_symb (FILE *out_file, int symb, int i, int j);
/* permet ds tous les cas a la semantique d'acceder au token associe' a un Tpq */
extern struct sxtoken   *parser_Tpq2tok (int Tpq);

/* Pour faire une allocation de sxba quand on connait la taille statiquement */
#define SXBA_CST_ALLOC(T,l)  SXBA_ELT T[NBLONGS(l)+1] = {l}

#include        "rcg_sglbl.h"

SX_GLOBAL_VAR int	      n /* nb de token du source */;
SX_GLOBAL_VAR SXBA            source_set, source_top;
/* Ds tous les cas ... */
SX_GLOBAL_VAR int             init_mlstn, final_mlstn;

#if is_sdag
SX_GLOBAL_VAR SXBA            *glbl_source_area, *glbl_source, *glbl_out_source;
#if MEASURES
SX_GLOBAL_VAR SXBA            unknown_word_set;
#endif /* MEASURES */
#else /* !is_sdag */
SX_GLOBAL_VAR int             *glbl_source_area, *glbl_source, *glbl_out_source;
#endif /* !is_sdag */

#if is_dag
SX_GLOBAL_VAR int             top_dag_trans, new_init_pos;
#else /* !is_dag */
#if is_sdag
#include                      "X.h"
SX_GLOBAL_VAR X_header        tok_noXtok;
SX_GLOBAL_VAR int             *tok_noXtok2xbuf; /* (tok_no, t) --> xbuf  (pour SXGET_TOKEN) */
#define SDAG_PACK(i,j)     ((int)((i)<<16)+(int)(j))
/* Codes des formes flechie "(" et ")" ds les SDAG PACK et UNPACK */
#define SDAG_FF               0
#define SDAG_LP               0X0000FFFF
#define SDAG_RP               0X0000FFFE
#endif /* is_sdag */
#endif /* !is_dag */

SX_GLOBAL_VAR BOOLEAN	      is_print_prod, is_parse_forest, is_tagged_dag, is_tagged_sdag, IS_DYNAMIC_SET_AUTOMATON;
SX_GLOBAL_VAR int	      IS_CHECK_RHS_SEQUENCES;
SX_GLOBAL_VAR unsigned int    maximum_input_size;


#if EBUG || LOG || LLOG
#define statistics	(stdout)
#else /* !(EBUG || LOG || LLOG) */
/* Ne pas utiliser NULL!! */
#define statistics	(0)
#endif /* !(EBUG || LOG || LLOG) */

/* Le 04/10/06, changement */
#define rcvr_tok_no       0
#define max_rcvr_tok_no   0
#define delta_n           0

#if 0
SX_GLOBAL_VAR int             rcvr_tok_no, max_rcvr_tok_no;
#endif /* 0 */

/* delta_n est un #define donne a la compilation qui donne le nombre maximal
   d'accroissement possible (par correction d'erreurs) de la chaine source. */
/* Meme ds le cas is_dag */
/* Ds les cas is_[dag_]rcvr, delta_n==0 => rattrapage global uniquement */
#ifndef delta_n
# define delta_n 0
#endif /* delta_n */

/* permet de selectionner une strategie systeme de parcours des couples de milestones sur lesquels
   la rcvr est essayee successivement.  L'utilisateur doit remplir ds l'appel de SEMANTICS() le champ
   rcvr_spec.range_walk_kind avec l'une de ces valeurs.
   Si ca ne lui convient pas il peut faire son propre parcours en emplissant for_semact.rcvr_range_walk
   avec sa propre fonction
*/
#define FORWARD_RANGE_WALK_KIND        1
#define BACKWARD_RANGE_WALK_KIND       2
#define MIXED_FORWARD_RANGE_WALK_KIND  3
#define MIXED_BACKWARD_RANGE_WALK_KIND 4

#define GEN_ALL_REPAIR_STRING          0

#define GEN_ALL_REPAIR_PARSE           0

#define TRY_MIN                        0
#define TRY_LOW                        1
#define TRY_MEDIUM                     2
#define TRY_HIGH                       3
#define TRY_MAX                        4

/* Cette structure est remplie par l'appel de SEMANTICS () */
SX_GLOBAL_VAR struct rcvr_spec {
  int insertion; /* -1 => pas d'insertion */
  int suppression;
  int changement;
  int deplacement;
  int range_walk_kind;
  int repair_kind; /* GEN_ALL_REPAIR_STRING ou nb */
  int repair_parse_kind; /* GEN_ALL_REPAIR_PARSE ou nb */
  int try_kind; /* [TRY_MIN..TRY_MAX] */
  BOOLEAN perform_repair, perform_repair_parse;
} rcvr_spec;

SX_GLOBAL_VAR struct for_semact {
    void	(*sem_init) (),
                (*sem_final) (),
                (*scanact) (), /* Le 17/11/05 pour traiter les [|...|] */
                (*oflw) (),
                (*timeout_mngr) (), /* BS 26/04/06 */
                (*vtimeout_mngr) ();

    int  	(*prdct) (),
                (*parsact) (),
                (*semact) (),
                (*constraint) (),
                (*sem_pass) (),
                (*rcvr) (),
                (*rcvr_range_walk) (); /* c'est l'utilisateur qui calcule les range de la rcvr */
    
    char        *(*string_args) (), /* Le 18/01/06 retourne l'utilisation des args de la semantique */
                *(*ME) (); /* nom de la semantique */

    BOOLEAN     (*process_args) (); /* Le 18/01/06 pour traiter les args de la semantique */

    int         pass_nb;
} for_semact;

#if 0
/* obsolete */
SX_GLOBAL_VAR int                            logn;
#define MAKE_A_i_j(A,i,j)      ((((A<<logn)+i)<<logn)+j)
#define HI_BIT                 (1<<31)
#endif /* 0 */

/* La dcl/def de struct spf a ete deplacee ds sxspf.h le 27/09/06 */

#if 0
/* ds dag_scanner.h */
/* fait "ressembler" un terminal de la correction a un vrai terminal du source */
extern int put_repair_tok (int                   t,
		    char                  *t_str,
		    char                  *comment_str,
		    struct sxsource_coord source_index);
#endif /* 0 */

#if is_dag
extern char *get_dag_sentence_str (BOOLEAN is_unique_parse) /* Retourne la string de la phrase d'entree, filtree par l'analyse, sous forme d'un dag */;
#else
#if is_sdag
extern char *get_sdag_sentence_str () /* Retourne la string de la phrase d'entree, filtree par l'analyse, sous forme d'un sdag */;
#endif /* !is_sdag */
#endif /* !is_dag */

extern void dag_or_nodag_source_processing () /* se trouve ds lexicalizer_mngr */;

#include "sxspf.h"

#endif /* earley_h */
