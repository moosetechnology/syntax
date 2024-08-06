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

/*	N O N - D E T E R M I N I S T I C    P A R S I N G	*/

#if 0
/* S I N G L E - E L E M E N T - S T A C K  */

struct sxses {
    SXBA	set;
    SXINT		*stack;
};

#define sxses_set(ses)		ses.set

#define sxses_alloc(ses,size)	ses.set = sxba_calloc (size + 1),			\
                                ses.stack = (SXINT *) sxalloc (size + 1, sizeof (SXINT)),	\
                                ses.stack [0] = 0

#define sxses_resize(ses,size)	ses.set = sxba_resize (ses.set, size + 1),		\
                                ses.stack = (SXINT *) sxrealloc (ses.stack, size + 1, sizeof (SXINT))

#define sxses_is_free(ses)	(ses.set == NULL)

#define sxses_clean(ses)	ses.set = NULL,						\
                                ses.stack = NULL

#define sxses_free(ses)		sxfree (ses.set),					\
                                sxfree (ses.stack),					\
                                sxses_clean (ses)

#define sxses_size(ses)		(SXBASIZE(ses.set) - 1)

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
    SXINT			base, size;
};

struct XxY_layer	*_LAYER_HEADER_PTR;


static void XxY_layer_oflw (old_size, new_size)
    SXINT old_size, new_size;
{
    _LAYER_HEADER_PTR->size += new_size - old_size;
    _LAYER_HEADER_PTR->global =
	(struct XxY_elem *) sxrealloc (_LAYER_HEADER_PTR->global,
				       _LAYER_HEADER_PTR->size + 1,
				       sizeof (struct XxY_elem));
}

static void XxY_layer_alloc (header, size)
    struct XxY_layer	*header;
    SXINT			size;
{
    XxY_alloc (&(header->local), "local", size, 1, 0, 0, XxY_layer_oflw, NULL);
    header->size = XxY_size (header->local);
    header->global = (struct XxY_elem *) sxalloc (header->size + 1, sizeof (struct XxY_elem));
    header->base = 0;
}

static SXINT XxY_layer_put (header, X, Y)
    struct XxY_layer	*header;
    SXINT			X, Y;
{
    struct XxY_elem	*eptr;
    SXINT			index;
    bool		is_new;

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
    SXINT top;

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
    SXINT				my_index, next_for_scanner;
    struct sxmilstn_elem	*prev_milstn, *next_milstn;
    SXINT				kind;
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

extern void sxmilstn_oflw (sxindex_header *index_header_ptr, SXINT old_line_nb, SXINT old_size);
extern void sxmilstn_alloc (sxmilstn_header *header, SXINT size);
extern void sxmilstn_free (sxmilstn_header *header);

/* fin INCLUDE	sxmilstn.h	*/


struct G {
    SXINT		N, T, P, G;
};


#define for_scanner_mask	03

#define FS_INCR(x)	((x+1) & for_scanner_mask)
#define FS_DECR(x)	((x+for_scanner_mask) & for_scanner_mask)

struct for_scanner {
    SXINT		previouses [for_scanner_mask + 1];
    SXINT		top, *current_hd, *next_hd;
};

struct rule {
    SXINT		hd, lgth;
};

struct triple {
    SXINT		parser, link;
};

struct for_reducer {
    struct triple	*triples;
    SXINT			top, size;
    SXINT			*refs, *triples_hd, *shift_reduce_symb;
};

struct ndrcvr {
    SXINT			**prefixes;

    SXINT			*TOK,
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

    SXINT			cur_top,
                        preds_size,
                        prev_top;

    SXBA		trans;

    XxY_header		seeds;

    XxYxZ_header	shifts;

    SXINT			index_0X;

    bool		is_up,
	                with_parsprdct,
	                is_warning,
	                dont_delete;

    struct best	{
	SXINT		*correction,
	                *TOK,
	                *MS;
	SXBA		vt_set;
	SXINT		model_no,
	                single_la;
	bool		a_la_rigueur;
    } best;

    struct nd2 {
	SXINT	prev_ms_nb;
    } nd2;
};


struct  parsers_attr {
    SXINT		son, level, symbol, milestone, next_for_scanner;
    SXINT		rhs_i [2];
    bool	is_already_processed, for_do_limited, dum1, dum2;
};



struct parse_stack {
    XxYxZ_header		shift_reduce,
                                parsers;

    struct parsers_attr		*parser_to_attr;

    SXBA			*parsers_set;

    SXINT				*GC_area,
                                GC_area_size;

    XxY_header			sons,
                                suffixes,
                                ranges,
                                symbols,
                                sf_rule,
                                sf_rhs;

    struct rule			*rule;

    SXINT				*grammar;

    SXBA			current_links,
                                used_links,
                                free_parsers_set,
                                erasable_sons;

    struct for_scanner		for_scanner;

    struct sxmilstn_elem	*milstn_head;

    SXINT				start_symbol,
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

    SXINT			        *transitions,
	                        *semact,
	                        *positions;

    struct for_reducer		for_reducer;

    struct SXP_reductions	*ared;

    struct G			G;

    bool			is_new_links,
                                parsact_seen,
                                parsprdct_seen,
                                is_ambiguous,
                                halt_hit;

    struct ndrcvr		rcvr;
};


struct parse_stack	parse_stack;

struct sxndtw {
    SXINT		root;

    XxY_header	paths;

    SXBA	is_erased,
                reached_symbol;

    SXINT		(*pass_inherited) (),
                (*pass_derived) ();

    bool     (*cycle_processing) ();

    void	(*open_hook) (),
                (*close_hook) (),
                (*nodes_oflw) ();

    struct pv {
	SXINT	node;
    } pv;

    struct cv {
	SXINT	visited,
	        visited_position,
	        visited_symbol,
	        visited_arity,
	        father,
	        father_symbol,
	        father_redno;
    } cv;

    struct nv {
	SXINT	kind,
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


SXINT	SXNDTW_;

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


SXINT	sxndtw_is_cycle (/* node */);
void	sxndtw_open (/* SXINT (*pass_inherited) (), SXINT (*pass_derived) (),
			bool (*cycle_processing) (),
			void (*open_hook) (), void (*close_hook) (), void (*nodes_oflw) () */);
void	sxndtw_init ();
void	sxndtw_walk ();
bool sxndrtw_walk ();
void	sxndtw_final ();
void	sxndtw_close ();
void	sxndtw_node_erase (/* node */);
void	sxndtw_check_grammar ();
bool sxndtw_sem_calls ();



/*  	N O N - D E T E R M I N I S T I C    S C A N N I N G	*/

struct ndincl_elem {
    struct ndincl_elem	*prev;
    SXINT			current_size, include_action;
    SXBA		ndlsets [3];
};

struct ndsrcvr_probe {
    SXINT 	*model;
    SXINT		model_no;
    short	*string;
};

struct ndsrcvr {
    SXBA		ndlv_set,
                        state_set;
    X_header		stmt_set;
    XxYxZ_header	paths;
    short		*source_char,
	                *class_to_insert;
    SXINT			*source_classes;
    struct ndsrcvr_probe
	                current,
	                may_be;
    bool		is_done,
                        is_may_be;
};

struct sxndsvar {
    struct SXS_tables	SXS_tables /*	les tables du scanner en local			*/ ;
    struct sxtables	*sxtables /*	pointeur vers l'ensemble des tables		*/ ;
    struct sxscan_mode	mode /* 	en global pour l'instant			*/ ;
    sxindex_header	index_header /* index dans ndlv					*/ ;
    SXINT			ndlv_size /* 	taille de ndlv					*/ ;
    struct sxndlv	*ndlv /*	ensemble des variables locales disponibles	*/ ;
    struct sxndlv	*current_ndlv /*ptr des variables locales en cours de traitement*/ ;
    SXBA		active_ndlv_set /*ensemble des variables locales actives	*/ ;
    SXBA		reduce_ndlv_set /*ensemble reduce des variables locales actives	*/ ;
    SXBA		read_char_ndlv_set /*ensemble read des variables locales actives*/ ;
    struct ndincl_elem	*top_incl /*	for includes processing				*/ ;
    struct ndsrcvr	rcvr /*		for the error recovery				*/ ;
} sxndsvar;

struct sxndlv {
    SXINT		my_index /*			current index in ndlv			*/ ;
    SXINT	*counters /* 			taille = S_counters_size		*/ ;
    SXINT		ts_lgth_use /*			longueur utilisee dans ts		*/ ;
    char	*token_string /*		buffer du token courant			*/ ;
    SXINT		ts_lgth /*			longueur courante de ts			*/ ;
    SXINT		current_state_no /*		etat courant du scanner			*/ ;
    SXINT		milestone /*			noeud du tokens_dag			*/ ;
    struct sxtoken	terminal_token /*	structure passee au parser		*/ ;
    short	previous_char /*		precedent le token courant		*/ ;
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
    SXINT				eof_milestone, eof_tok_no;
} sxndtkn;


#define MS_BEGIN	0
#define MS_EMPTY	1
#define MS_WAIT		2
#define MS_SLEEP	4
#define MS_DEAD		8
#define MS_COMPLETE	16
#define MS_EXHAUSTED	32

extern void 	ndlv_clear ();
extern SXSCANIT_FUNCTION	sxndscan_it;

