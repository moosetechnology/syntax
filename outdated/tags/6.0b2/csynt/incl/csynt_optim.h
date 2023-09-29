/*   C S Y N T _ O P T I M . H  */

#include "sxunix.h"
#include "put_edit.h"
#include "varstr.h"
#include "B_tables.h"
#include "XxY.h"

#define NonTerminal	0
#define Terminal	1
#define Predicate	2

#define Error		0
#define ScanShift	1
#define Shift		2
#define ScanReduce	3
#define Reduce		4
#define Prdct		5
#define ForceError	6
#define LaScanShift	7
#define NonDeter	8

#define max(x,y)	((x)>=(y) ? (x) : (y))
#define min(x,y)	((x)<=(y) ? (x) : (y))

#define	LGPROD(r)	(bnf_ag.WS_NBPRO [r + 1].prolon - bnf_ag.WS_NBPRO [r].prolon - 1)
#define PROD_TO_RED(x)	(x > bnf_ag.WS_TBL_SIZE.actpro ? bnf_ag.RULE_TO_REDUCE [x - bnf_ag.WS_TBL_SIZE.actpro] : x)

#define PUSH(T,x)	T [x] = T [0], T [0] = x
#define POP(T,x)	x = T [0], T [0] = T [x]

#define HASH_SIZE 257

extern char* get_xnt_string();

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

extern char release_mess [];

struct bnf_ag_item	bnf_ag;

BOOLEAN is_automaton,
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
	sxverbosep,
        has_xprod,
	sxttycol1p;

int	xac2,
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

int	xtt2,
        xtnt2,
        xtat2,
        xtant2,
        xptant,
        xptnt,
        xptat,
        xptt,
        xnucl2;

VARSTR  vstr;


SXBA	single_prod /* xnbpro */,
	t_check_set,
        xnt_state_set,
        xt_state_set;


FILE	*listing;

XxY_header		ref_to_ee;


struct S1 {
	   int	ptr, ptat, ptt, ptant, ptnt;
	   int	tsuct, tsucnt;
	   int	pprd, pred;
	   int	shftt, shftnt;
	   int	etat, StNt;
       };

struct xor_StNt {
	   int	lnk,
	        Stlnk,
	        state,
	        xptant,
	        X1_head;
       };

struct include {
	   int	lnk,
	        X2;
       };

struct lstprd {
	   int	f1,
	        f2;
       };

struct state {
    int		lgth,
                start;
    };

struct floyd_evans {
    int		codop,
                reduce,
                pspl,
                next;
    };

struct stat_vars {
    int		lines,
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

struct S1	*S1 /* 0:lgt (1) */ ;
struct xor_StNt	*xor_StNts /* lgt (2) */ ;
struct include	*includes /* lgt (5) */ ;
struct lstprd	*lstprd /* 0:lgt (6) */ ;
int	*tt /* 1:lgt (8) */ ;
int	*tat /* 0:lgt (9) */ ;
int	*tant /* 1:lgt (10) */ ;
int	*tnt /* 1:lgt (11) */ ;
int	*nucl /* 1:lgt (12) */ ;

int	*nt_trans_set, *nt_trans_stack;
/* SXBA	nullable_set; *NV* */

struct statistics	stats;

struct state	*t_states,
                *nt_states,
                *predicates;

struct P_item	*t_items,
                *nt_items;

struct floyd_evans	*t_fe,
	                *nt_fe,
	                *tnt_fe;

struct P_reduction	*reductions;

struct P_base	*t_bases;

struct P_base	*nt_bases;

struct P_item	*vector;

/* struct P_goto	*gotos; */

struct P_prdct	*prdcts;

struct P_prdct_list	*prdct_list;

int	Mtstate,
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


int	*t_state_equiv_class,
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

int	t_hash [HASH_SIZE],
        nt_hash [HASH_SIZE],
        tnt_hash [HASH_SIZE],
        prdct_hash [HASH_SIZE];


extern BOOLEAN  prdct_equal ();
extern int	LALR1 (), OPTIM (), get_xprdct (), put_in_fe (), gotostar (), gen_tree ();
extern VOID	sat_mess (), soritem (), path_reallocation (), path_free (), build_fsa (), out_reduce_derivation (), 
  out_shift_derivation (), erase_fsa (), output_fe (), CLEAR (), MAJ (), filprd (), cut (), total_spe (), partial_spe (), build_branch (), algoV (), 
  optim_lo (), optim_ll (), optim_sizes (), fe_to_equiv (), xprdct_mngr ();

#include "XxY.h"

XxY_header 	STATExNT_hd;
int		*STATExNT2tnt, *tnt_next;

/*   E N D    C S Y N T _ O P T I M . H  */
