extern char	*xt_to_str ();
extern VARSTR	fsa_to_re ();
extern BOOLEAN	is_t_trans ();

#define L_SUCC(i)	(((i) < 0) ? -1 : (i) + 1)
#define L_MAX(i,j)	(((i) < 0 || (j) < 0) ? -1 : max (i, j))

struct ARC_ATTR {
    int		from, conflict_kind, lgth, primes_nb;
    BOOLEAN	is_tree_orig, is_pure_tree, is_marked;
};
struct whd {
    int		lnk, type_nb;
};
struct wl {
    int		lnk, ss, type;
};

struct ARC_struct {
    XH_header		Q_hd;
    SXBA		Q_set;
    XxYxZ_header	QxTxQ_hd;
    int			*Q_stack;
    struct ARC_ATTR	*attr;
    struct wl		*wl;
    struct whd		*whd;
    SXBA		t_set, conflict_t_set;
    struct ARC_struct	*ARC;
    int			pss_kind, h_value, k_value, kind, conflict_kind;
    BOOLEAN		is_allocated, is_initiated, is_XARC, is_clonable, is_ARP;
};

struct ARC_struct	ARCs [4];
#define ARC_UNBOUNDED		2
#define ARC_BOUNDED_or_FSA	0
#define XARC_UNBOUNDED		3
#define XARC_FSA		1

struct kas {
    SXBA	pss_set;
    int		type, xpts, m, M;
};

int		lgt [13];
struct kas	*kas, *work_kas;
int		XTMAX, XE1, PATHS_size, germe, state_nb_used, work_set_no;
int		MAX_RED_PER_STATE, RED_NO;
int		ORIG_K_VALUE, ORIG_H_VALUE, ORIG_PSS_KIND;

struct back {
    int		lnk, X2, ibfsa;
};
struct back	*back;

struct PSSxT_to_xpts {
    int		xpts, back_head;
};
XxY_header		PSSxT_hd;
struct PSSxT_to_xpts	*PSSxT_to_xpts;

SXBA		LR0_states_set;
XH_header	pss_hd /* Parse-Stack-Suffixes */ ;
int		*back_head, *pss_check_stack;
SXBA		*pss_sets, work_set, and_set, pss_work_set;
XH_header	ibfsa_hd /* IncrementalBackwardFSA */ ;
int		*ibfsa_stack;
SXBA		couples_set;
XH_header	bfsa_hd /* BackwardFSA */ ;
XH_header	fks_hd /* ForKs_Set */ ;
XH_header	pis_hd /* lrPI_States */ ;
XH_header	fknot_hd;
int		*bfsa_to_ate, *fknot_to_ate, *ibfsa_to_ate;
int		*Q0_to_occur_nb;

XxY_header	Q0xN_hd, XQ0xXQ0_hd;

XxY_header	forks;
XxY_header	FORKxT_hd;
int		*FORKxT_to_fks;

int		XX_ARC_kind, QTQ /* Global pour install_a_quad */;
BOOLEAN		XX_is_XARC, XX_is_pss_check;

SXBA		*reads_cycle_sets;
int		reads_cycle_sets_line_nb;


struct init_state_attr {
    int		back_head, type, pss_list, fork_list, h_value;
};
struct init_state_attr	*init_state_attr;

int	shift_item, shift_terminal;

BOOLEAN		mandatory_ff, is_shift_type, ibfsa_needed, has_several_equiv_reductions, is_clonable;

int		NEW_K_VALUE;

XxY_header	ARP_items;
XxY_header	ARPxTRANS_hd;
int		*ARPxTRANS_to_next;
XxY_header	ARP_ItemxItem_hd;
XH_header	ARP_items_hd;
XH_header	ARP_states_hd;

int		*ws, *lpss;

X_header	pss_to_Next_hd;
int		*pss_to_Next;
XH_header	Next_hd;

BOOLEAN		conflict_message;
#define is_all_conflicts	TRUE

#define maxint		(((unsigned int)(-1))>>1)
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

int	pss_kind;

