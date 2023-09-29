#ifndef sxnd_h
#define sxnd_h

/*	N O N - D E T E R M I N I S T I C    P A R S I N G	*/


# include 	"sxunix.h"
# include 	"X.h"
# include 	"XxY.h"
# include 	"XxYxZ.h"
# include 	"SS.h"

extern BOOLEAN	sxndparse_in_la ();
extern BOOLEAN	sxnd2parser ();
extern BOOLEAN	sxndparser ();
extern BOOLEAN	sxndparse_clean_forest ();
extern void	sxndparse_pack_forest ();
extern void	sxndparse_erase_hook_item ();
extern void     sxndtkn_put (SXBA set);
extern VOID     sxndtkn_mngr (int sxndtkn_mngr_what, int size);


/*      C T R L - S T A C K   M A N A G E R      	*/


struct ctrl_stack {
    SXBA	set;
    int		*stack;
};

#define CTRL_TOP(c)		(*(c).stack)
#define CTRL_ALLOC(c,l)		((c).set=sxba_calloc(l),			\
				 (c).stack=(int*)sxalloc((l)+1,sizeof(int)),	\
				 CTRL_TOP(c)=0)
#define CTRL_FREE(c)		(sxfree((c).set),sxfree((c).stack))
#define CTRL_PUSH(c,x)		(SXBA_bit_is_reset_set((c).set,x) ? (c).stack[++CTRL_TOP(c)]=x : 0)
#define CTRL_POP(c)		((c).stack[CTRL_TOP(c)--])
#define CTRL_POP2(c)		(SXBA_0_bit((c).set,CTRL_TOP(c)),CTRL_POP(c))
#define CTRL_IS_EMPTY(c)	(CTRL_TOP(c)==0)
#define CTRL_FOREACH(c,x)	while(CTRL_IS_EMPTY(c)?0:(x=CTRL_POP(c))>=0)
#define CTRL_FOREACH2(c,x)	while(CTRL_IS_EMPTY(c)?0:(x=CTRL_POP2(c))>=0)
#define CTRL2SXBA(c)		((c).set)
#define CTRL_CLEAR(c)		(sxba_empty((c).set),CTRL_TOP(c)=0)
#define CTRL_RESIZE(c,l)	(sxba_resize((c).set,l),			\
				 (c).stack=(int*)sxrealloc((c).stack,(l)+1,sizeof(int)))

/*      E N D   C T R L - S T A C K   M A N A G E R	*/



#if 0
/* S I N G L E - E L E M E N T - S T A C K  */

struct sxses {
    SXBA	set;
    int		*stack;
};

#define sxses_set(ses)		ses.set

#define sxses_alloc(ses,size)	ses.set = sxba_calloc (size + 1),			\
                                ses.stack = (int *) sxalloc (size + 1, sizeof (int)),	\
                                ses.stack [0] = 0

#define sxses_resize(ses,size)	ses.set = sxba_resize (ses.set, size + 1),		\
                                ses.stack = (int *) sxrealloc (ses.stack, size + 1, sizeof (int))

#define sxses_is_free(ses)	(ses.set == NULL)

#define sxses_clean(ses)	ses.set = NULL,						\
                                ses.stack = NULL

#define sxses_free(ses)		sxfree (ses.set),					\
                                sxfree (ses.stack),					\
                                sxses_clean (ses)

#define sxses_size(ses)		(BASIZE(ses.set) - 1)

#define sxses_top(ses)		ses.stack [0]

#define sxses_is_empty(ses)	(sxses_top(ses) == 0)

#define sxses_raz(ses)		sxba_empty (ses.set), ses.stack [0] = 0

#define sxses_get(ses,i)	ses.stack [i]

#define sxses_spush(ses,x)	ses.stack [++*ses.stack] = x

#define sxses_push(ses,x)	(SXBA_bit_is_reset_set (ses.set, x)			\
                                    ? ses.stack [++*ses.stack] = x			\
                                    : 0)

#define sxses_fpush(ses,x)	SXBA_1_bit (ses.set, x),				\
                                ses.stack [++*ses.stack] = x

#define sxses_pop(ses)		ses.stack [(*ses.stack)--]


/* E N D   of  S I N G L E - E L E M E N T - S T A C K  */


/* XxY  by  L A Y E R */

struct XxY_layer {
    XxY_header		local;
    struct XxY_elem	*global;
    int			base, size;
};

struct XxY_layer	*_LAYER_HEADER_PTR;


static void XxY_layer_oflw (old_size, new_size)
    int old_size, new_size;
{
    _LAYER_HEADER_PTR->size += new_size - old_size;
    _LAYER_HEADER_PTR->global =
	(struct XxY_elem *) sxrealloc (_LAYER_HEADER_PTR->global,
				       _LAYER_HEADER_PTR->size + 1,
				       sizeof (struct XxY_elem));
}

static void XxY_layer_alloc (header, size)
    struct XxY_layer	*header;
    int			size;
{
    XxY_alloc (&(header->local), "local", size, 1, 0, 0, XxY_layer_oflw, NULL);
    header->size = XxY_size (header->local);
    header->global = (struct XxY_elem *) sxalloc (header->size + 1, sizeof (struct XxY_elem));
    header->base = 0;
}

static int XxY_layer_put (header, X, Y)
    struct XxY_layer	*header;
    int			X, Y;
{
    struct XxY_elem	*eptr;
    int			index;
    BOOLEAN		is_new;

    _LAYER_HEADER_PTR = header;
    is_new = !XxY_set (&(header->local), X, Y, &index);
    index += header->base;
    
    if (is_new) {
	eptr = header->global + index;
	eptr->X = X;
	eptr->Y = Y;
    }

    return index;
}

static void XxY_new_layer (header)
    struct XxY_layer	*header;
{
    int top;

    if ((top = XxY_top (header->local)) > 0) {
	header->base += top;
	XxY_clear (&(header->local));
	_LAYER_HEADER_PTR = header;
	XxY_layer_oflw (header->size, header->size + XxY_size (header->local));
    }
}

static void XxY_layer_free (header)
    struct XxY_layer	*header;
{
    XxY_free (&(header->local));
    sxfree (header->global);
}

#define	XxY_layer_get(header,x,i,j)	i=header.global[x].X,j=header.global[x].Y 

/* END XxY  by  L A Y E R */

#endif

/* INCLUDE	sxmilstn.h	*/

struct sxmilstn_elem {
    int				my_index,
                                my_milestone,
                                next_for_scanner,
                                kind;

    struct sxmilstn_elem	*prev_milstn,
                                *next_milstn;
};

typedef struct {
    sxindex_header		indexes;
    struct sxmilstn_elem	**buf;
    SXBA			set;
} sxmilstn_header;

    
#define sxmilstn_clear(h)	sxindex_clear ((h).indexes)
#define sxmilstn_release(h,x)	sxindex_release((h).indexes,x)
#define sxmilstn_size(h)	sxindex_size((h).indexes)
#define sxmilstn_line_nb(h)	sxindex_line_nb((h).indexes)
#define sxmilstn_column_nb(h)	sxindex_column_nb((h).indexes)
#define sxmilstn_seek(h)	sxindex_seek(&((h).indexes))
#define sxmilstn_access(h,x)	(h).buf [sxindex_i((h).indexes,x)][sxindex_j((h).indexes,x)]

extern VOID sxmilstn_oflw (/* sxindex_header* */);
extern VOID sxmilstn_alloc (/* sxmilstn_header*, int */);
extern VOID sxmilstn_free (/* sxmilstn_header* */);

/* fin INCLUDE	sxmilstn.h	*/


struct G {
    int		N, T, P, G;
};


#define for_scanner_mask	03

#define FS_INCR(x)	((x+1) & for_scanner_mask)
#define FS_DECR(x)	((x+for_scanner_mask) & for_scanner_mask)

struct for_scanner {
    int		previouses [for_scanner_mask + 1];
    int		top, *current_hd, *next_hd;
};

struct rule {
    int		hd 	/* Index ds "grammar" du LHS symbol */;
    int		lgth 	/* Longueur de la regle |LHS| + |RHS| */;
    int		item_hd /* Index ds "grammar" de la 1ere occur de la regle courante en RHS */;
};

struct grammar {
  int		rhs_symb 	/* Si <= 0 => terminal sinon index ds "rule" */;
  int		lhs_rule 	/* No de la regle courante */;
  int		next_item	/* Si >0 Index ds "grammar" de la l'occur suivante de rhs_symb */;
};

struct triple {
    int		parser, ref, tok_no, link, symbol, re_do;
    BOOLEAN	is_shift_reduce, is_valid;
};

struct for_reducer {
    struct triple	*triples;
    int			top, size, re_do;
    int			*refs, *triples_hd;
};

struct for_parsact {
    int		(*action_top) (),
    		(*action_pop) (),
    		(*action_bot) (),
    		(*action_new_top) (),
    		(*action_final) (),
    		(*GC) (),
      		(*post) (),
                (*third_field) (),
                (*parsers_oflw) (),
                (*sons_oflw) (),
                (*symbols_oflw) (),
        	(*post_td_init) (),
        	(*post_td_action) (),
        	(*post_td_final) ();
};

struct repair {
    int		repair_tok_no,
                error_tok_no,
                model_no,
                kind;

    int		params [5];
};

struct ndrcvr {
    int			**prefixes,
                        **mm;

    int			*TOK,
                        *MS,
                        *to_be_sorted,
                        *preds;

    SXBA		prefixes_set,
                        halt_hit_set;

    struct sxsource_coord
	                source_index;

    struct sxmilstn_elem
	                *prefixes_to_milestones,
	                *milstn_head,
	                *milstn_elems,
	                *milestones;

    int			cur_top,
                        preds_size,
                        milstn_nb,
                        Mtok_no,
                        MMrch,
                        Mrch,
                        Mrch_model_no,
                        Mrch_pos,
                        mrch,
                        prev_top;

    SXBA		trans,
	                *back_bone;

    XxY_header		seeds,
                        correction_strings;

    XxYxZ_header	shifts;

    int			index_0X;

    BOOLEAN		is_up,
	                with_parsprdct,
	                is_warning,
	                all_corrections,
	                dont_delete;

    struct best	{
	int		*correction,
	                *TOK,
	                *MS;
	SXBA		vt_set;
	int		model_no,
	                single_la;
	BOOLEAN		a_la_rigueur;
    } best;

    struct repair	*repair_list;

    char		*repair_string;

    int			repair_list_size,
                        repair_list_top,
                        repair_string_size,
                        repair_string_top;
};


/* sizeof (struct  parsers_attr) == 2**3 * sizeof (int) */
struct  parsers_attr {
    int		son, milestone, symbol, next_for_scanner, scan_ref;
    int		rhs_i [2];
    BOOLEAN	is_already_processed;
};



struct parse_stack {
    /* XxY_header		parsers; */
    XxYxZ_header		parsers;

    struct parsers_attr		*parser_to_attr;

    SXBA			*parsers_set;

    int				*GC_area,
                                GC_area_size;

    XxY_header			sons,
                                mscans,
                                suffixes,
                                ranges,
                                symbols,
                                sf_rule,
                                /* unit_rules, */
                                sf_rhs;

    struct rule			*rule;

    struct grammar		*grammar;

    struct ctrl_stack		reached,
                                erased;

    SXBA			current_links,
                                used_links,
                                free_parsers_set,
                                erasable_sons;

    struct for_scanner		for_scanner;

    struct sxmilstn_elem	*milstn_tail;

    int				start_symbol,
                                hole_nb,
                                local_link,
                                root,
	                        free_parser,
	                        hook_rule,
                                current_token,
                                current_milestone,
                                current_lahead,
                                delta_generator,
	                        shift_trans_nb,
	                        red_trans_nb,
	                        rule_size,
	                        rule_top,
	                        grammar_size,
	                        grammar_top;

    int				*transitions;

    struct for_reducer		for_reducer;

    struct for_parsact		for_parsact;

    int				*red2act;

    struct SXP_reductions	*ared;

    struct G			G;

    SXBA			useful_ntset,
                                useful_tset;

    BOOLEAN			is_new_links,
                                is_token_dag,
                                parsact_seen,
                                parsprdct_seen,
                                is_ambiguous,
                                /* is_unit_rules, */
                                halt_hit;

    struct ARC {
	int			current_token,
	                        level,
	                        init_t_state;
    } ARC;

    struct ndrcvr		rcvr;
};


struct parse_stack	parse_stack;

struct sxndtw {
    int		root;

    XxY_header	paths;

    SXBA	is_erased,
                reached_symbol,
                already_traversed;

    int		(*pass_inherited) (),
                (*pass_derived) ();

    BOOLEAN     (*cycle_processing) ();

    VOID	(*open_hook) (),
                (*close_hook) (),
                (*nodes_oflw) ();

    struct pv {
	int	node;
    } pv;

    struct cv {
	int	visited,
	        visited_position,
	        visited_symbol,
	        visited_arity,
	        father,
	        father_symbol,
	        father_redno;
    } cv;

    struct nv {
	int	kind,
	        node;
    } nv;
};

struct sxndtw	sxndtw;


#define NDFATHER		sxndtw.cv.father
#define NDFATHER_SYMBOL		sxndtw.cv.father_symbol
#define NDFATHER_REDNO		sxndtw.cv.father_redno

#define NDVISITED		sxndtw.cv.visited
#define NDVISITED_POSITION	sxndtw.cv.visited_position
#define NDVISITED_SYMBOL	sxndtw.cv.visited_symbol
#define NDVISITED_ARITY		sxndtw.cv.visited_arity

#define NDPREV_NODE		sxndtw.pv.node


int	SXNDTW_;

#define sxndtw_node_to_item(n)		XxY_Y(sxndtw.paths,n)

#define sxndtw_symb_to_lhsitem(s)	parse_stack.rule[s].hd

#define sxndtw_node_to_father(n)	((n)==0?0:XxY_X(sxndtw.paths,n))

#define sxndtw_item_to_symb(i)		parse_stack.grammar[i].rhs_symb

#define sxndtw_node_to_symb(n)		sxndtw_item_to_symb(sxndtw_node_to_item(n))

#define	sxndtw_item_to_pos(i)		((i)-parse_stack.rule[parse_stack.grammar[i].lhs_rule].hd)

#define	sxndtw_node_to_pos(n)		sxndtw_item_to_pos(sxndtw_node_to_item(n))

#define sxndtw_symb_to_arity(s)							\
    (((SXNDTW_=(s))<=0)?0:parse_stack.rule [SXNDTW_].lgth-1)

#define	sxndtw_node_to_arity(n)		sxndtw_symb_to_arity (sxndtw_node_to_symb(n))

#define	sxndtw_node_to_son(n,i,s)	(XxY_set(&sxndtw.paths, (n), (i), &(s)), s)

#define	sxndtw_symb_to_ref(s)		XxY_X (parse_stack.symbols,			\
					       sxndtw_item_to_symb (sxndtw_symb_to_lhsitem (s)))

#define	sxndtw_symb_to_red_no(s)						\
    (sxplocals.SXP_tables.reductions [sxndtw_symb_to_ref(s)].reduce)

#define	sxndtw_symb_to_action(s)						\
    (sxplocals.SXP_tables.reductions [sxndtw_symb_to_ref(s)].action)

#define	sxndtw_node_to_red_no(n)	sxndtw_symb_to_red_no (sxndtw_node_to_symb (n))

#define	sxndtw_symb_to_range(s)		XxY_Y (parse_stack.symbols,			\
					       sxndtw_item_to_symb (sxndtw_symb_to_lhsitem (s)))

#define	sxndtw_node_to_range(n)		sxndtw_symb_to_range (sxndtw_node_to_symb (n)) 

#define sxndtw_inf(range)		(((range)<0)?-(range):(XxY_X(parse_stack.ranges,range)))

#define sxndtw_sup(range)		(((range)<0)?-(range):(XxY_Y(parse_stack.ranges,range)))

#define	sxndtw_is_node_erased(n)	SXBA_bit_is_set(sxndtw.is_erased,n)

#define	sxndtw_is_item_erased(i)	(parse_stack.grammar[i].rhs_symb==0)

#define	sxndtw_item_erase(i)		sxndtw_item_to_symb(i)=0


int	sxndtw_is_cycle (/* node */);
SXVOID	sxndtw_open (/* int (*pass_inherited) (), int (*pass_derived) (),
			BOOLEAN (*cycle_processing) (),
			VOID (*open_hook) (), VOID (*close_hook) (), VOID (*nodes_oflw) () */);
SXVOID	sxndtw_init ();
SXVOID	sxndtw_walk ();
BOOLEAN sxndrtw_walk ();
SXVOID	sxndtw_final ();
SXVOID	sxndtw_close ();
SXVOID	sxndtw_node_erase (/* node */);
SXVOID	sxndtw_check_grammar ();
BOOLEAN sxndtw_sem_calls ();



/*  	N O N - D E T E R M I N I S T I C    S C A N N I N G	*/

struct ndincl_elem {
    struct ndincl_elem	*prev;
    int			include_action;
    SXBA		ndlsets [3];
};

struct ndsrcvr_probe {
    int 	*model;
    int		model_no;
    SXSHORT	*string;
};

struct ndsrcvr {
    SXBA		ndlv_set,
                        state_set;
    X_header		stmt_set;
    XxYxZ_header	paths;
    SXSHORT		*source_char,
	                *class_to_insert;
    int			*source_classes;
    struct ndsrcvr_probe
	                current,
	                may_be;
    BOOLEAN		is_done,
                        is_may_be;
};

struct sxndsvar {
    struct SXS_tables	SXS_tables /*	les tables du scanner en local			*/ ;
    struct sxtables	*sxtables /*	pointeur vers l'ensemble des tables		*/ ;
    struct sxscan_mode	mode /* 	en global pour l'instant			*/ ;
    sxindex_header	index_header /* index dans ndlv					*/ ;
    int			ndlv_size /* 	taille de ndlv					*/ ;
    struct sxndlv	*ndlv /*	ensemble des variables locales disponibles	*/ ;
    struct sxndlv	*current_ndlv /*ptr des variables locales en cours de traitement*/ ;
    SXBA		active_ndlv_set /*ensemble des variables locales actives	*/ ;
    SXBA		reduce_ndlv_set /*ensemble reduce des variables locales actives	*/ ;
    SXBA		read_char_ndlv_set /*ensemble read des variables locales actives*/ ;
    struct ndincl_elem	*top_incl /*	for includes processing				*/ ;
    struct ndsrcvr	rcvr /*		for the error recovery				*/ ;
} sxndsvar;

struct sxndlv {
    int		my_index /*			current index in ndlv			*/ ;
    long	*counters /* 			taille = S_counters_size		*/ ;
    int		ts_lgth_use /*			longueur utilisee dans ts		*/ ;
    char	*token_string /*		buffer du token courant			*/ ;
    int		ts_lgth /*			longueur courante de ts			*/ ;
    int		current_state_no /*		etat courant du scanner			*/ ;
    int		milestone /*			noeud du tokens_dag			*/ ;
    struct sxtoken	terminal_token /*	structure passee au parser		*/ ;
    SXSHORT	previous_char /*		precedent le token courant		*/ ;
    struct mark mark /* 			pour l'action @mark			*/ ;
    SXSTMI	stmt /*				0 ou next stmt				*/ ;
};


#define SXNDNORMAL_SCAN_P	(sxndsvar.mode.mode == SXNORMAL_SCAN)
#define SXNDLA_SCAN_P		(sxndsvar.mode.mode == SXLA_SCAN)
#define SXNDRCVR_SCAN_P		(sxndsvar.mode.mode == SXRCVR_SCAN)

#define SXCURRENT_NDSCANNED_CHAR						\
   (SXNDNORMAL_SCAN_P ? sxsrcmngr.current_char : sxsrcmngr.buffer[sxsrcmngr.labufindex])


struct sxndtkn {
    sxmilstn_header		milestones;

    XxYxZ_header		dag;

    struct sxmilstn_elem	*milstn_current,
                                *milstn_head;

    int				last_milestone_no;
} sxndtkn;


#define MS_BEGIN	0
#define MS_EMPTY	1
#define MS_WAIT		2
#define MS_SLEEP	4
#define MS_DEAD		8
#define MS_COMPLETE	16
#define MS_EXHAUSTED	32

extern VOID 	ndlv_clear ();
extern VOID	sxndscan_it ();

#endif
