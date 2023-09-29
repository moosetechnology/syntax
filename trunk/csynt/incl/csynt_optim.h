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

#include "sxunix.h"
#include "put_edit.h"
#include "varstr.h"
#include "B_tables.h"
#include "XxY.h"

/* Chaque déclaration de variable doit aussi être une définition de cette
   variable, selon la valeur donnée à SX_GLOBAL_VAR_CSYNT : "extern" ou rien.

   Typiquement, cela donne, pour la variable "variable" de type "type"
   et de valeur initiale "valeur" :

   SX_GLOBAL_VAR_CSYNT  type  variable  SX_INIT_VAL(valeur);
*/

#ifndef SX_GLOBAL_VAR_CSYNT /* Inutile de le faire plusieurs fois */
#ifdef SX_DFN_EXT_VAR_CSYNT
/* Cas particulier : Il faut DÉFINIR les variables globales. */
#define SX_GLOBAL_VAR_CSYNT	/*rien*/
#define SX_INIT_VAL_CSYNT(v)	= v
#else
/* Cas général : Ce qu'on veut, c'est juste DÉCLARER les variables globales, pas les DÉFINIR. */
#define SX_GLOBAL_VAR_CSYNT	extern
#define SX_INIT_VAL_CSYNT(v)	/*rien*/
#endif /* #ifndef SX_DFN_EXT_VAR_CSYNT */
#endif /* #ifndef SX_GLOBAL_VAR_CSYNT */

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

#define max(x,y)	((x)>=(y) ? (x) : (y))
#define min(x,y)	((x)<=(y) ? (x) : (y))

#define	LGPROD(r)	(bnf_ag.WS_NBPRO [r + 1].prolon - bnf_ag.WS_NBPRO [r].prolon - 1)
#define PROD_TO_RED(x)	(x > bnf_ag.WS_TBL_SIZE.actpro ? bnf_ag.RULE_TO_REDUCE [x - bnf_ag.WS_TBL_SIZE.actpro] : x)

#define PUSH(T,x)	T [x] = T [0], T [0] = x
#define POP(T,x)	x = T [0], T [0] = T [x]

#define HASH_SIZE 257


#define XT_TO_T_CODE(xt) ((xt+bnf_ag.WS_TBL_SIZE.tmax<=0) ? xt : bnf_ag.XT_TO_T_PRDCT[xt+bnf_ag.WS_TBL_SIZE.tmax].v_code)
#define XT_TO_PRDCT_CODE(xt) ((xt+bnf_ag.WS_TBL_SIZE.tmax<=0) ? -1 : bnf_ag.XT_TO_T_PRDCT[xt+bnf_ag.WS_TBL_SIZE.tmax].prdct_no)
#define XNT_TO_NT_CODE(xnt) ((xnt-bnf_ag.WS_TBL_SIZE.ntmax<=0) ? xnt : bnf_ag.XNT_TO_NT_PRDCT[xnt-bnf_ag.WS_TBL_SIZE.ntmax].v_code)
#define XNT_TO_PRDCT_CODE(xnt) ((xnt-bnf_ag.WS_TBL_SIZE.ntmax<=0) ? -1 : bnf_ag.XNT_TO_NT_PRDCT[xnt-bnf_ag.WS_TBL_SIZE.ntmax].prdct_no)

#define get_t_string(t)  (bnf_ag.T_STRING + bnf_ag.ADR [t])
#define get_nt_string(nt)  (bnf_ag.NT_STRING + bnf_ag.ADR [nt])
#define get_nt_length(nt) (bnf_ag.ADR [nt+1] - bnf_ag.ADR [nt] - 1)

/* #define grand 100000 */
/* Le 04/02/2003 */
#define grand 10000000

SX_GLOBAL_VAR_CSYNT struct bnf_ag_item	bnf_ag;

SX_GLOBAL_VAR_CSYNT SXBOOLEAN is_automaton,
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
        is_path,
        is_lr1_constructor,
        is_listing_opened,
        has_xprod,
	sxttycol1p;

SX_GLOBAL_VAR_CSYNT SXINT	xac2,
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
        xxor_StNt,
        reduce_item_no,
        xincludes,
        NTMAX;

SX_GLOBAL_VAR_CSYNT SXINT	xtt2,
        xtnt2,
        xtat2,
        xtant2,
        xptant,
        xptnt,
        xptat,
        xptt,
        xnucl2;

SX_GLOBAL_VAR_CSYNT VARSTR  vstr;


SX_GLOBAL_VAR_CSYNT SXBA	single_prod /* xnbpro */,
	t_check_set,
        xnt_state_set,
        xt_state_set;


SX_GLOBAL_VAR_CSYNT FILE	*listing;

SX_GLOBAL_VAR_CSYNT XxY_header		ref_to_ee;


struct S1 {
	   SXINT	ptr, ptat, ptt, ptant, ptnt;
	   SXINT	tsuct, tsucnt;
	   SXINT	pprd, pred;
	   SXINT	shftt, shftnt;
	   SXINT	etat, StNt;
       };

struct xor_StNt {
	   SXINT	lnk,
	        Stlnk,
	        state,
	        xptant,
	        X1_head;
       };

struct include {
	   SXINT	lnk,
	        X2;
       };

struct lstprd {
	   SXINT	f1,
	        f2;
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

SX_GLOBAL_VAR_CSYNT struct S1	*S1 /* 0:lgt (1) */ ;
SX_GLOBAL_VAR_CSYNT struct xor_StNt	*xor_StNts /* lgt (2) */ ;
SX_GLOBAL_VAR_CSYNT struct include	*includes /* lgt (5) */ ;
SX_GLOBAL_VAR_CSYNT struct lstprd	*lstprd /* 0:lgt (6) */ ;
SX_GLOBAL_VAR_CSYNT SXINT	*tt /* 1:lgt (8) */ ;
SX_GLOBAL_VAR_CSYNT SXINT	*tat /* 0:lgt (9) */ ;
SX_GLOBAL_VAR_CSYNT SXINT	*tant /* 1:lgt (10) */ ;
SX_GLOBAL_VAR_CSYNT SXINT	*tnt /* 1:lgt (11) */ ;
SX_GLOBAL_VAR_CSYNT SXINT	*nucl /* 1:lgt (12) */ ;

SX_GLOBAL_VAR_CSYNT SXINT	*nt_trans_set, *nt_trans_stack;
/* SXBA	nullable_set; *NV* */

SX_GLOBAL_VAR_CSYNT struct statistics	stats;

SX_GLOBAL_VAR_CSYNT struct state	*t_states,
                *nt_states,
                *predicates;

SX_GLOBAL_VAR_CSYNT struct P_item	*t_items,
                *nt_items;

SX_GLOBAL_VAR_CSYNT struct floyd_evans	*t_fe,
	                *nt_fe,
	                *tnt_fe;

SX_GLOBAL_VAR_CSYNT struct P_reduction	*reductions;

SX_GLOBAL_VAR_CSYNT struct P_base	*t_bases;

SX_GLOBAL_VAR_CSYNT struct P_base	*nt_bases;

SX_GLOBAL_VAR_CSYNT struct P_item	*vector;

/* struct P_goto	*gotos; */

SX_GLOBAL_VAR_CSYNT struct P_prdct	*prdcts;

SX_GLOBAL_VAR_CSYNT struct P_prdct_list	*prdct_list;

SX_GLOBAL_VAR_CSYNT SXINT	Mtstate,
    	Mntstate,
    	Mvec,
    	/* Mgotos, */
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


SX_GLOBAL_VAR_CSYNT SXINT	*t_state_equiv_class,
        *nt_state_equiv_class,
	*t_lnks,
	*nt_lnks,
	*tnt_lnks,
	*prdct_lnks,
	*nt_to_ad,
        *nt_to_nt,
	*new_t_state_to_old,
	*new_nt_state_to_old,
        *germe_to_gr_act /* Fait correspondre a chaque ARC (germe) une instruction reduce
			   du conflit */;

SX_GLOBAL_VAR_CSYNT SXINT	t_hash [HASH_SIZE],
        nt_hash [HASH_SIZE],
        tnt_hash [HASH_SIZE],
        prdct_hash [HASH_SIZE];

#define item P_item

struct base {
    SXINT		defaut, commun, propre;
};


extern SXBOOLEAN  prdct_equal (SXINT xprdct, SXINT prdct1, SXINT prdct2);
extern SXINT	LALR1 (char *ln), 
                OPTIM (char *ln), 
                get_xprdct (SXINT prdct1, SXINT prdct2), 
                put_in_fe (struct floyd_evans *fe, SXINT *hashs, SXINT *lnks, struct floyd_evans *afe), 
                gotostar (SXINT state, SXINT start, SXINT goal), 
                gen_tree (SXINT father, struct floyd_evans *fe);
extern SXVOID	sat_mess (char *name, SXINT no, SXINT old_size, SXINT new_size), 
                soritem (SXINT x, SXINT d, SXINT f, char *c1, char *s2), 
                path_reallocation (SXBA *first), 
                path_free (void), 
                build_fsa (SXINT *build_fsa_nucl, SXBA *first), 
                out_reduce_derivation (SXINT t, SXINT item, SXINT nt_trans), 
                out_shift_derivation (SXINT item), 
                erase_fsa (void), 
                output_fe (SXINT kind, SXINT xa, SXINT test, struct floyd_evans *afe), 
                CLEAR (SXINT *T), 
                MAJ (SXINT *x, SXINT n), 
                filprd (SXINT pred, SXINT suc), 
                cut (SXINT St, SXINT q, SXINT q1), 
                total_spe (SXBA total_spe_single_prod), 
                partial_spe (SXBA partial_spe_single_prod), 
                build_branch (SXINT xprdct, SXINT action, SXINT father), 
                algoV (SXBOOLEAN is_nt, 
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
		       SXINT *DELTA), 
                optim_lo (char *ln, char *ME_optim_lo), 
                optim_ll (void), 
                optim_sizes (void), 
                fe_to_equiv (struct floyd_evans *afe1, struct floyd_evans *afe2), 
                xprdct_mngr (SXINT entry);

#include "XxY.h"

SX_GLOBAL_VAR_CSYNT XxY_header 	STATExNT_hd;
SX_GLOBAL_VAR_CSYNT SXINT		*STATExNT2tnt, *tnt_next;

/*   E N D    C S Y N T _ O P T I M . H  */
