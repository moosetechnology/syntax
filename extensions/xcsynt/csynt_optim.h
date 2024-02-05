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
/*   C S Y N T _ O P T I M . H  */

/* Chaque déclaration de variable doit aussi être une définition de cette
   variable, selon la valeur donnée à SX_GLOBAL_VAR_XCSYNT : "extern" ou rien.

   Typiquement, cela donne, pour la variable "variable" de type "type"
   et de valeur initiale "valeur" :

   SX_GLOBAL_VAR_XCSYNT  type  variable  SX_INIT_VAL(valeur);
*/

#ifndef SX_GLOBAL_VAR_XCSYNT /* Inutile de le faire plusieurs fois */
#  ifdef SX_DFN_EXT_VAR_XCSYNT
/* Cas particulier : Il faut DÉFINIR les variables globales. */
#    define SX_GLOBAL_VAR_XCSYNT	/*rien*/
#    define SX_INIT_VAL_XCSYNT(v)	= v
#  else
/* Cas général : Ce qu'on veut, c'est juste DÉCLARER les variables globales, pas les DÉFINIR. */
#    define SX_GLOBAL_VAR_XCSYNT	extern
#    define SX_INIT_VAL_XCSYNT(v)	/*rien*/
#  endif /* #ifndef SX_DFN_EXT_VAR_XCSYNT */
#endif /* #ifndef SX_GLOBAL_VAR_XCSYNT */

#include "sxunix.h"
#include "put_edit.h"
#include "varstr.h"
#include "B_tables.h"
#include "sxba.h"
#include "XxYxZ.h"
#include "XH.h"
#include "SS.h"

#include "RLR.h"

#define min(x,y) (((x)<=(y))?(x):(y))
#define max(x,y) (((x)>=(y))?(x):(y))

#define NonTerminal	((SXINT)0)
#define Terminal	((SXINT)1)
#define Predicate	((SXINT)2)

#define Error		((SXINT)0)
#define ScanShift	((SXINT)1)
#define Shift		((SXINT)2)
#define ScanReduce	((SXINT)3)
#define Reduce		((SXINT)4)
#define Prdct		((SXINT)5)
#define ForceError	((SXINT)6)
#define LaScanShift	((SXINT)7)
#define NonDeter	((SXINT)8)

#define NO_CONFLICT_	((SXINT)000000)
#define NOT_LALRk_	((SXINT)000001)
#define NOT_LALR_	((SXINT)000003)
#define NOT_LRk_	((SXINT)000005)
#define NOT_LR_		((SXINT)000017)
#define NOT_RhLALRk_	((SXINT)000020)
#define NOT_RoxoLALRk_	((SXINT)000040)
#define NOT_RhLALR0_	((SXINT)000100)
#define NOT_RoxoLALR0_	((SXINT)000200)
#define NOT_RLALR_	((SXINT)000763)
#define NOT_RoxoLRk_	((SXINT)001040)
#define NOT_RoxoLR0_	((SXINT)002200)
#define NOT_LRPI_	((SXINT)007777)
#define AMBIGUOUS_	((SXINT)017777)
#define ALL_CONFLICT_	((SXINT)037777)

#define FSA_		((SXINT)0)
#define UNBOUNDED_	((SXINT)1)
#define BOUNDED_	((SXINT)2)

#define PUSH(T,x)	T [x] = T [0], T [0] = x
#define POP(T,x)	x = T [0], T [0] = T [x], T [x] = 0

#define	LGPROD(r)	(bnf_ag.WS_NBPRO [r + 1].prolon - bnf_ag.WS_NBPRO [r].prolon - 1)
#define PROD_TO_RED(x)	(x > bnf_ag.WS_TBL_SIZE.actpro ? bnf_ag.RULE_TO_REDUCE [x - bnf_ag.WS_TBL_SIZE.actpro] : x)

#define HASH_SIZE 257


/* le parametre xt est POSITIF */
#define XT_TO_T_CODE(xt) ((xt+bnf_ag.WS_TBL_SIZE.tmax<=0) ? xt : bnf_ag.XT_TO_T_PRDCT[xt+bnf_ag.WS_TBL_SIZE.tmax].v_code)
#define XT_TO_PRDCT_CODE(xt) ((xt+bnf_ag.WS_TBL_SIZE.tmax<=0) ? -1 : bnf_ag.XT_TO_T_PRDCT[xt+bnf_ag.WS_TBL_SIZE.tmax].prdct_no)

#define XNT_TO_NT_CODE(xnt) ((xnt-bnf_ag.WS_TBL_SIZE.ntmax<=0) ? xnt : bnf_ag.XNT_TO_NT_PRDCT[xnt-bnf_ag.WS_TBL_SIZE.ntmax].v_code)
#define XNT_TO_PRDCT_CODE(xnt) ((xnt-bnf_ag.WS_TBL_SIZE.ntmax<=0) ? -1 : bnf_ag.XNT_TO_NT_PRDCT[xnt-bnf_ag.WS_TBL_SIZE.ntmax].prdct_no)

#define get_t_string(t)  (bnf_ag.T_STRING + bnf_ag.ADR [t])
#define get_nt_string(nt)  (bnf_ag.NT_STRING + bnf_ag.ADR [nt])
#define get_nt_length(nt) (bnf_ag.ADR [nt+1] - bnf_ag.ADR [nt] - 1)

/* #define grand 100000 */
/* Le 25/02/2003 */
#define grand 100000000

#define Q0_to_tnt_trans(s)	(Q0 [s].in)

#define XH_BOT(h,x)		((h).list + (h).display [x].X)
#define XH_TOP(h,x)		((h).list + (h).display [(x)+1].X)

#define XQ0_TO_Q0(s)		((clone_to_lr0_state == NULL) || (s <= lr0_automaton_state_nb) ? s : clone_to_lr0_state [s - lr0_automaton_state_nb])
SX_GLOBAL_VAR_XCSYNT SXINT		*clone_to_lr0_state, lr0_automaton_state_nb;

SX_GLOBAL_VAR_XCSYNT char	constructor_name [24];

SX_GLOBAL_VAR_XCSYNT struct bnf_ag_item	bnf_ag;

SX_GLOBAL_VAR_XCSYNT bool is_automaton,
	is_list_user_conflicts,
	is_list_system_conflicts,
	is_list_conflicts,
	is_abstract,
	is_floyd_evans,
	is_long_listing,
	is_force,
        is_pspe,
        is_tspe,
        is_nspe,
        is_unique_conflict_per_state,
        is_unique_derivation_per_conflict,
        is_lr_constructor,
        is_rlr_constructor,
        is_ambiguity_check,
        is_statistics,
        is_left_corner,
        is_listing_opened,
        is_ambiguous,
        is_super_arc,
        is_not_LRpi,
        is_non_deterministic_automaton,
        has_xprod,
        print_time,
	sxttycol1p;

SX_GLOBAL_VAR_XCSYNT SXINT	conflict_derivation_kind,
        reduce_item_no,
        max_red_per_state,
        xac2,
        xe1,
        xpredicates,
	xt_items,
        xnt_items,
        xprdct_items,
	any,
        t_error,
        force_error,
	t_vector_size,
        nt_vector_size,
        nt_fe_size,
	MIN,
        MAX,
        delta,
        t_MIN,
        t_delta,
        init_state,
        final_state,
        NTMAX,
        MAX_LA,
        h_value,
        k_value,
        k_value_max,
        limnttrans;

SX_GLOBAL_VAR_XCSYNT VARSTR  vstr;


SX_GLOBAL_VAR_XCSYNT SXBA	single_prod /* xnbpro */,
	s_red_set /* xnbpro */,
        not_used_red_set /* xnbpro */,
	t_check_set,
        xnt_state_set,
        xt_state_set,
        nd_state_set,
        *LR0_sets,
        Next_states_set,
	*FIRST;

SX_GLOBAL_VAR_XCSYNT FILE	*listing, *statistics_file;


SX_GLOBAL_VAR_XCSYNT XH_header		nucleus_hd;
SX_GLOBAL_VAR_XCSYNT XxY_header		Q0xV_hd;
SX_GLOBAL_VAR_XCSYNT SXINT		 	*Q0xV_to_Q0;
SX_GLOBAL_VAR_XCSYNT SXINT		 	*Q0xV_to_Q0xXNT;
SX_GLOBAL_VAR_XCSYNT XxY_header		Q0xQ0_hd;
SX_GLOBAL_VAR_XCSYNT XxY_header		includes_hd;
SX_GLOBAL_VAR_XCSYNT X_header		StNt_reads_cycle_hd;
SX_GLOBAL_VAR_XCSYNT SXINT			*StNt_to_reads_cycle_nb, Q0xQ0_top;
SX_GLOBAL_VAR_XCSYNT XxY_header		ref_to_ee;

struct Q0 {
    SXINT		in, xla, bot, t_trans_nb, nt_trans_nb, red_trans_nb;
};


struct state {
    SXINT		lgth,
                start;
    };

struct floyd_evans {
    SXINT		codop,
                reduce,
                pspl,
                next;
    };

struct stat_vars {
    SXINT		lines,
                cols;
    };

struct stat_tbl {
    struct stat_vars	i,
                        r,
                        c;
    };

struct statistics {
    struct stat_tbl	t,
                        nt;
    };

#define item P_item

struct base {
    SXINT		defaut, commun, propre;
};

SX_GLOBAL_VAR_XCSYNT struct Q0	*Q0;

SX_GLOBAL_VAR_XCSYNT SXBA	nullable_set;

SX_GLOBAL_VAR_XCSYNT struct statistics	stats;

SX_GLOBAL_VAR_XCSYNT struct state	*t_states,
                *nt_states,
                *predicates;

SX_GLOBAL_VAR_XCSYNT struct P_item	*t_items,
                *nt_items;

SX_GLOBAL_VAR_XCSYNT struct floyd_evans	*t_fe,
	                *nt_fe,
	                *tnt_fe;

SX_GLOBAL_VAR_XCSYNT struct P_reduction	*reductions;

SX_GLOBAL_VAR_XCSYNT struct P_base	*t_bases;

SX_GLOBAL_VAR_XCSYNT struct P_base	*nt_bases;

SX_GLOBAL_VAR_XCSYNT struct P_item	*vector;

SX_GLOBAL_VAR_XCSYNT struct P_prdct	*prdcts;

SX_GLOBAL_VAR_XCSYNT struct P_prdct_list	*prdct_list;

SX_GLOBAL_VAR_XCSYNT SXINT	Mtstate,
    	Mntstate,
    	Mvec,
    	deltavec,
    	mMprdct,
    	mMred,
    	mMrednt,
    	Mprdct,
    	Mfe,
    	M0ref,
    	Mref,
    	Mred,
    	mMfe,
        mgerme,
        Mgerme,
        nd_degree,
        prdct_list_top;


SX_GLOBAL_VAR_XCSYNT SXINT	*t_state_equiv_class,
        *nt_state_equiv_class,
	*t_lnks,
	*nt_lnks,
	*tnt_lnks,
	*prdct_lnks,
	*nt_to_ad,
        *nt_to_nt,
        *germe_to_gr_act /* Fait correspondre a chaque ARC (germe) une instruction reduce
			   du conflit */;

SX_GLOBAL_VAR_XCSYNT SXINT	t_hash [HASH_SIZE],
        nt_hash [HASH_SIZE],
        tnt_hash [HASH_SIZE],
        prdct_hash [HASH_SIZE];



extern SXINT	LALR1 (void), 
                OPTIM (void), 
                new_clone (SXINT state), 
                nucl_i (SXINT state, SXINT i), 
                gotostar (SXINT state, SXINT start, SXINT goal), 
                ARC_get_red_no (SXINT arc_no, SXINT t), 
                ARC_final (void), 
                Q0_V_to_next (SXINT state, SXINT v), 
                make_new_pss (SXINT pss, SXINT next_state), 
                put_in_fe (struct floyd_evans *fe, 	   
			   SXINT *put_in_fe_hashs, 
			   SXINT *put_in_fe_lnks, 
			   struct floyd_evans *afe), 
                ARC_building (struct ARC_struct *ARC,
			      bool (*F_trans)(struct ARC_struct *, SXINT, SXINT, SXINT*),
			      SXINT (*F_get_conflict_kind) (struct ARC_struct *, SXINT, bool, SXINT) ),
                is_LRpi (struct ARC_struct *XARC, SXINT qtq), 
                call_XARC_building (struct ARC_struct *ARC, SXINT qtq), 
                get_quad_nb (void), 
                super_FSA (X_header *X_hd, 		   
			   XxYxZ_header	*XxYxZ_hd,
			   XxYxZ_header *AxOxV_hd,
			   SXINT Tcard,
			   bool dont_reuse_initial_state), 
                orig_item_i (SXINT red_no, SXINT i), 
                pss_set_to_pss_list (SXBA pss_set), 
                get_xprdct (SXINT prdct1, SXINT prdct2), 
                gen_tree (SXINT codop, SXINT father);
extern void	sat_mess (char *name, SXINT no, SXINT old_size, SXINT new_size), 
                LR0 (void), 
                cut_includes (SXINT q, SXINT q1), 
                soritem (SXINT x, SXINT d, SXINT f, char *c1, char *s2), 
                XARC_resize (SXINT old_xac2, SXINT new_non_lalr1_state_nb), 
                print_ambiguities (/* xac1, */SXBA t_set, SXINT StNt), 
                free_ambiguities (void), 
                print_not_LRpi_conflicts (SXINT kind), 
                free_not_LRpi (void), 
                ARC_init (bool print_conflicts, 
		          SXINT kind, 
		          SXINT ARC_init_max_red_per_state, 
		          SXINT arcs_size), 
                ARC_get_sizes (SXINT *arc_state_nb, SXINT *arc_trans_nb), 
                ARC_free (void), 
                free_cycles (void), 
                sornt (SXINT xac1), 
                sort (SXINT xac1),
                RLR_code_generation (SXINT xac1, SXINT arc_no, SXBA t_set, SXINT plulong), 
                CLEAR (SXINT *T), 
                XARC_free (void), 
                messages_mngr_free (void), 
                earley_free (void), 
                first_free (void), 
                make_inter (SXINT *bot1, 
			    SXINT *top1, 
			    SXINT *bot2, 
			    SXINT *top2, 
			    bool normal, 
			    SXBA states_set), 
                output_fe (SXINT kind,
			   SXINT xa, 
			   SXINT test, 
			   struct floyd_evans *afe), 
                XARC_print (struct ARC_struct *XARC), 
                compute_Next_states_set (void), 
                messages_mngr_alloc (void), 
                cloning (void), 
                conflict_messages_output (SXBA conflict_t_set), 
                ARP_alloc (void), 
                ARP_realloc (void), 
                sambig_conflict_messages (struct ARC_struct *XARC,
					  SXINT	qtq, 
					  SXINT sambig_conflict_messages_fork, 
					  SXINT type1, 
					  SXINT type2), 
                ARP_free (void), 
                fill_kas (struct ARC_struct *ARC,
			  SXINT ARC_state, 
			  struct kas *fill_kas_kas, 
			  SXBA *fill_kas_pss_sets, 
			  SXINT *xkas, 
			  SXINT conflict_kind), 
                ARC_allocate (struct ARC_struct *ARC,
			      SXINT kind,
			      void (*F_oflw1)(SXINT, SXINT) ), 
                alloc_UARP (void), 
                n_to_forks (struct ARC_struct *XARC,
			    SXINT XARC_state, 
			    SXINT n, 
			    SXINT type), 
                RARP_free (void), 
                build_UARP (void), 
                create_clone (SXINT p, SXINT q1), 
                call_ARP_conflict_messages (struct ARC_struct *XARC,
					    SXINT prev_XARC_state, 
					    SXINT t, 
					    SXINT next_XARC_state), 
                print_conclusions (SXINT xac1, SXBA print_conclusions_t_set), 
                fe_to_equiv (struct floyd_evans *afe1,
			     struct floyd_evans	*afe2), 
                build_branch (SXINT xprdct, struct floyd_evans *fe, SXINT father), 
                total_spe (SXBA total_spe_single_prod), 
                partial_spe (SXBA partial_spe_single_prod, SXBA restriction_set), 
                xprdct_mngr (SXINT entry), 
                algoV (bool is_nt,
		       SXINT statemax, 
		       SXINT tmax, 
		       SXINT xrupt1, 
		       struct state *states, 
		       struct item *items, 
		       struct base *BASES, 
		       struct item **VECTOR, 
		       SXBA *BASES_SET, 
		       SXINT *m, 
		       SXINT *M, 
		       SXINT *DELTA, 
		       SXINT *USED_NB), 
                optim_lo (char *optim_lo_language_name, char *ME_local), 
                optim_ll (void), 
                optim_sizes (void), 
                free_alpha_table (void);
extern char     *get_constructor_name (char *string,
				       bool is_lalr, 
				       bool is_rlr,
				       SXINT h, 
				       SXINT k);
extern bool  should_print_conflict (SXINT conflict_kind), 
                ARC_walk_backward (struct ARC_struct *ARC,
				   SXINT qtq,
				   bool (*bh)(struct ARC_struct *, SXINT),
				   bool (*bs)(struct ARC_struct *, SXINT)), 
                earley_parse (SXINT LA_init, SXINT LA_final, XH_header *hd, SXINT item, bool (*F)(SXINT, SXINT, bool)), 
                check_bot_state_conformity (SXINT state, SXINT level), 
                prdct_equal (SXINT xprdct, SXINT prdct1, SXINT prdct2), 
                open_listing (SXINT kind, 
			      char *open_listing_language_name, 
			      char *ME_local, 
			      char *component_name);
extern VARSTR prod_to_str (VARSTR prod_to_str_vstr, SXINT prod);


/*   E N D    C S Y N T _ O P T I M . H  */
