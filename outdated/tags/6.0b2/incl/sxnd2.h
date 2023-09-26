/*	N O N - D E T E R M I N I S T I C    P A R S I N G	*/


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
    int				my_index, next_for_scanner;
    struct sxmilstn_elem	*prev_milstn, *next_milstn;
    int				kind;
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
    int		hd, lgth;
};

struct triple {
    int		parser, link;
};

struct for_reducer {
    struct triple	*triples;
    int			top, size;
    int			*refs, *triples_hd, *shift_reduce_symb;
};

struct ndrcvr {
    int			**prefixes;

    int			*TOK,
                        *MS,
                        *to_be_sorted,
                        *preds;

    SXBA		prefixes_set;

    struct sxsource_coord
	                source_index;

    struct sxmilstn_elem
	                *prefixes_to_milestones,
	                *milstn_head,
	                *milestones;

    int			cur_top,
                        preds_size,
                        prev_top;

    SXBA		trans;

    XxY_header		seeds;

    XxYxZ_header	shifts;

    int			index_0X;

    BOOLEAN		is_up,
	                with_parsprdct,
	                is_warning,
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

    struct nd2 {
	int	prev_ms_nb;
    } nd2;
};


struct  parsers_attr {
    int		son, level, symbol, milestone, next_for_scanner;
    int		rhs_i [2];
    BOOLEAN	is_already_processed, for_do_limited, dum1, dum2;
};



struct parse_stack {
    XxYxZ_header		shift_reduce,
                                parsers;

    struct parsers_attr		*parser_to_attr;

    SXBA			*parsers_set;

    int				*GC_area,
                                GC_area_size;

    XxY_header			sons,
                                suffixes,
                                ranges,
                                symbols,
                                sf_rule,
                                sf_rhs;

    struct rule			*rule;

    int				*grammar;

    SXBA			current_links,
                                used_links,
                                free_parsers_set,
                                erasable_sons;

    struct for_scanner		for_scanner;

    struct sxmilstn_elem	*milstn_head;

    int				start_symbol,
                                hole_nb,
                                root,
	                        free_parser,
	                        hook_rule,
	                        current_level,
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

    int			        *transitions,
	                        *semact,
	                        *positions;

    struct for_reducer		for_reducer;

    struct SXP_reductions	*ared;

    struct G			G;

    BOOLEAN			is_new_links,
                                parsact_seen,
                                parsprdct_seen,
                                is_ambiguous,
                                halt_hit;

    struct ndrcvr		rcvr;
};


struct parse_stack	parse_stack;

struct sxndtw {
    int		root;

    XxY_header	paths;

    SXBA	is_erased,
                reached_symbol;

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

#define sxndtw_item_to_symb(i)		parse_stack.grammar[i]

#define sxndtw_node_to_symb(n)		sxndtw_item_to_symb(sxndtw_node_to_item(n))

#define	sxndtw_item_to_pos(i)		parse_stack.positions[i]

#define	sxndtw_node_to_pos(n)		sxndtw_item_to_pos(sxndtw_node_to_item(n))

#define sxndtw_symb_to_arity(s)							\
    (((SXNDTW_=(s))<=0)?0:parse_stack.rule [SXNDTW_].lgth-1)

#define	sxndtw_node_to_arity(n)		sxndtw_symb_to_arity (sxndtw_node_to_symb(n))

#define	sxndtw_node_to_son(n, i, s)	(XxY_set(&sxndtw.paths, (n), (i), &(s)), s)

#define	sxndtw_symb_to_red_no(s)	XxY_X (parse_stack.symbols,			\
					       sxndtw_item_to_symb (sxndtw_symb_to_lhsitem (s)))

#define	sxndtw_node_to_red_no(n)	sxndtw_symb_to_red_no (sxndtw_node_to_symb (n))

#define	sxndtw_symb_to_range(s)		XxY_Y (parse_stack.symbols,			\
					       sxndtw_item_to_symb (sxndtw_symb_to_lhsitem (s)))

#define	sxndtw_node_to_range(n)		sxndtw_symb_to_range (sxndtw_node_to_symb (n)) 

#define sxndtw_inf(range)		(((range)<0)?-(range):(XxY_X(parse_stack.ranges,range)))

#define sxndtw_sup(range)		(((range)<0)?-(range):(XxY_Y(parse_stack.ranges,range)-1))

#define	sxndtw_is_node_erased(n)	SXBA_bit_is_set(sxndtw.is_erased,n)

#define	sxndtw_is_item_erased(i)	(parse_stack.grammar[i]==0)

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
    int			current_size, include_action;
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
    struct sxmilstn_elem	*milstn_current, *milstn_head;
    int				eof_milestone, eof_tok_no;
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

