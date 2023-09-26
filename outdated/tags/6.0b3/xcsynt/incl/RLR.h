#ifndef __RLR_H__
#define __RLR_H__

extern char	*xt_to_str (SXINT code);
extern VARSTR	fsa_to_re (VARSTR varstr_ptr, SXBA *R, SXBA *R_plus, SXINT size, SXINT initial_s, SXINT final_s, SXINT (*get_name_refs) (SXBA_INDEX, SXBA_INDEX, char **));
extern BOOLEAN	is_t_trans_function (SXINT item);

#define L_SUCC(i)	(((i) < 0) ? -1 : (i) + 1)
#define L_MAX(i,j)	(((i) < 0 || (j) < 0) ? -1 : max (i, j))

struct ARC_ATTR {
    SXINT		from, conflict_kind, lgth, primes_nb;
    BOOLEAN	is_tree_orig, is_pure_tree, is_marked;
};
struct whd {
    SXINT		lnk, type_nb;
};
struct wl {
    SXINT		lnk, ss, type;
};

struct ARC_struct {
    XH_header		Q_hd;
    SXBA		Q_set;
    XxYxZ_header	QxTxQ_hd;
    SXINT			*Q_stack;
    struct ARC_ATTR	*attr;
    struct wl		*wl;
    struct whd		*whd;
    SXBA		t_set, conflict_t_set;
    struct ARC_struct	*ARC;
    SXINT			pss_kind, h_value, k_value, kind, conflict_kind;
    BOOLEAN		is_allocated, is_initiated, is_XARC, is_clonable, is_ARP;
};

SX_GLOBAL_VAR_XCSYNT struct ARC_struct	ARCs [4];
#define ARC_UNBOUNDED		((SXINT)2)
#define ARC_BOUNDED_or_FSA	((SXINT)0)
#define XARC_UNBOUNDED		((SXINT)3)
#define XARC_FSA		((SXINT)1)

struct kas {
    SXBA	pss_set;
    SXINT		type, xpts, m, M;
};

SX_GLOBAL_VAR_XCSYNT SXINT		lgt [13];
SX_GLOBAL_VAR_XCSYNT struct kas	*kas, *work_kas;
SX_GLOBAL_VAR_XCSYNT SXINT		XTMAX, XE1, PATHS_size, germe, state_nb_used, work_set_no;
SX_GLOBAL_VAR_XCSYNT SXINT		MAX_RED_PER_STATE, RED_NO;
SX_GLOBAL_VAR_XCSYNT SXINT		ORIG_K_VALUE, ORIG_H_VALUE, ORIG_PSS_KIND;

struct back {
    SXINT		lnk, X2, ibfsa;
};
SX_GLOBAL_VAR_XCSYNT struct back	*back;

struct PSSxT_to_xpts {
    SXINT		xpts, back_head;
};
SX_GLOBAL_VAR_XCSYNT XxY_header		PSSxT_hd;
SX_GLOBAL_VAR_XCSYNT struct PSSxT_to_xpts	*PSSxT_to_xpts;

SX_GLOBAL_VAR_XCSYNT SXBA		LR0_states_set;
SX_GLOBAL_VAR_XCSYNT XH_header	pss_hd /* Parse-Stack-Suffixes */ ;
SX_GLOBAL_VAR_XCSYNT SXINT		*back_head, *pss_check_stack;
SX_GLOBAL_VAR_XCSYNT SXBA		*pss_sets, work_set, and_set, pss_work_set;
SX_GLOBAL_VAR_XCSYNT XH_header	ibfsa_hd /* IncrementalBackwardFSA */ ;
SX_GLOBAL_VAR_XCSYNT SXINT		*ibfsa_stack;
SX_GLOBAL_VAR_XCSYNT SXBA		couples_set;
SX_GLOBAL_VAR_XCSYNT XH_header	bfsa_hd /* BackwardFSA */ ;
SX_GLOBAL_VAR_XCSYNT XH_header	fks_hd /* ForKs_Set */ ;
SX_GLOBAL_VAR_XCSYNT XH_header	pis_hd /* lrPI_States */ ;
SX_GLOBAL_VAR_XCSYNT XH_header	fknot_hd;
SX_GLOBAL_VAR_XCSYNT SXINT		*bfsa_to_ate, *fknot_to_ate, *ibfsa_to_ate;
SX_GLOBAL_VAR_XCSYNT SXINT		*Q0_to_occur_nb;

SX_GLOBAL_VAR_XCSYNT XxY_header	Q0xN_hd, XQ0xXQ0_hd;

SX_GLOBAL_VAR_XCSYNT XxY_header	forks;
SX_GLOBAL_VAR_XCSYNT XxY_header	FORKxT_hd;
SX_GLOBAL_VAR_XCSYNT SXINT		*FORKxT_to_fks;

SX_GLOBAL_VAR_XCSYNT SXINT		XX_ARC_kind, QTQ /* Global pour install_a_quad */;
SX_GLOBAL_VAR_XCSYNT BOOLEAN		XX_is_XARC, XX_is_pss_check;

SX_GLOBAL_VAR_XCSYNT SXBA		*reads_cycle_sets;
SX_GLOBAL_VAR_XCSYNT SXINT		reads_cycle_sets_line_nb;


struct init_state_attr {
    SXINT		back_head, type, pss_list, fork_list, h_value;
};
SX_GLOBAL_VAR_XCSYNT struct init_state_attr	*init_state_attr;

SX_GLOBAL_VAR_XCSYNT SXINT	shift_item, shift_terminal;

SX_GLOBAL_VAR_XCSYNT BOOLEAN		mandatory_ff, is_shift_type, ibfsa_needed, has_several_equiv_reductions, is_clonable;

SX_GLOBAL_VAR_XCSYNT SXINT		NEW_K_VALUE;

SX_GLOBAL_VAR_XCSYNT XxY_header	ARP_items;
SX_GLOBAL_VAR_XCSYNT XxY_header	ARPxTRANS_hd;
SX_GLOBAL_VAR_XCSYNT SXINT		*ARPxTRANS_to_next;
SX_GLOBAL_VAR_XCSYNT XxY_header	ARP_ItemxItem_hd;
SX_GLOBAL_VAR_XCSYNT XH_header	ARP_items_hd;
SX_GLOBAL_VAR_XCSYNT XH_header	ARP_states_hd;

SX_GLOBAL_VAR_XCSYNT SXINT		*ws, *lpss;

SX_GLOBAL_VAR_XCSYNT X_header	pss_to_Next_hd;
SX_GLOBAL_VAR_XCSYNT SXINT		*pss_to_Next;
SX_GLOBAL_VAR_XCSYNT XH_header	Next_hd;

SX_GLOBAL_VAR_XCSYNT BOOLEAN		conflict_message;
#define is_all_conflicts	TRUE

#define maxint		(((SXUINT)(-1))>>1)
#define TopState(s)	(pss_hd.list [pss_hd.display [(s)+1].X-2])
#define BotState(s)	(pss_hd.list [pss_hd.display [s].X])


#define TOP_STATE(h,x)	((h).list [(h).display [(x)+1].X - 1])
#define BOT_STATE(h,x)	((h).list [(h).display [x].X])


#define BFSA_BOT(x)		(bfsa_hd.list + bfsa_hd.display [x].X)
#define BFSA_TOP(x)		(bfsa_hd.list + bfsa_hd.display [(x)+1].X)
#define IBFSA_BOT(x)		(ibfsa_hd.list + ibfsa_hd.display [x].X)
#define IBFSA_TOP(x)		(ibfsa_hd.list + ibfsa_hd.display [(x)+1].X)
#define PSS_BOT(x)		(pss_hd.list + pss_hd.display [x].X)
#define PSS_TOP(x)		(pss_hd.list + pss_hd.display [(x)+1].X - 1)
#define BACK_PATH_BOT(x)	(back_path_hd.list + back_path_hd.display [x].X)
#define BACK_PATH_TOP(x)	(back_path_hd.list + back_path_hd.display [(x)+1].X)
#define STACK_BOT(x)		(stack_hd.list + stack_hd.display [x].X)
#define STACK_TOP(x)		(stack_hd.list + stack_hd.display [(x)+1].X)
#define QUAD_BOT(x)		(quad_hd.list + quad_hd.display [x].X)
#define QUAD_TOP(x)		(quad_hd.list + quad_hd.display [(x)+1].X)
#define ARP_STATES_BOT(x)	(ARP_states_hd.list + ARP_states_hd.display [x].X)
#define ARP_STATES_TOP(x)	(ARP_states_hd.list + ARP_states_hd.display [(x)+1].X)
#define ARP_ITEMS_BOT(x)	(ARP_items_hd.list + ARP_items_hd.display [x].X)
#define ARP_ITEMS_TOP(x)	(ARP_items_hd.list + ARP_items_hd.display [(x)+1].X)

SX_GLOBAL_VAR_XCSYNT SXINT	pss_kind;


#endif /* __RLR_H__ */
