struct rhs {
    SXINT		symbol, next;
};

struct  parse_stack_elem {
    SXINT		state, red_ref, shift_ref, son, level, symbol;
};

struct G {
    SXINT		N, T, P, G;
};

struct red_parsers {
    SXINT parser, lnk;
};


struct parse_stack {
    struct parse_stack_elem	*graph;

    struct red_parsers		*red_parsers_list;

    XxY_header			sons,
                                suffixes,
                                ranges,
                                symbols;

    XH_header			output_grammar;

    SXINT				*lhs_hd,
                                *lhs_to_prod_list;

    SXBA			*parsers_set;

    SXINT	                        *parsers_rhs0,
    	                        *parsers_rhs1;

    SXBA			current_links,
                                used_links,
                                free_parsers_set,
                                erasable_sons,
                                active_sons,
                                for_do_limited,
                                is_already_processed;

    SXINT				*active_states,
                                *active_parsers,
                                *GC_area,
                                *red_stack,
                                *red_parsers_hd,
                                *for_shifter,
                                *for_scanner,
                                *active_reduces;

    SXINT				start_symbol,
                                root,
                                graph_size,
	                        graph_top,
	                        rhs_size,
	                        rhs_top,
	                        red_parsers_list_top,
	                        red_parsers_list_size,
	                        free_parser,
	                        hook_rule,
	                        hook_item,
	                        current_level,
                                current_token,
	                        current_symbol;

    SXINT			        *transitions,
	                        *semact,
	                        *positions;

    struct rhs			*rhs;

    struct SXP_reductions	*ared;

    struct G			G;

    BOOLEAN			is_new_links,
                                is_ambiguous;
} parse_stack;


struct parse_stack	parse_stack;

struct sxndtw {
    SXINT		root;

    XxY_header	paths;

    SXBA	is_erased;

    VOID	(*pass_inherited) (),
                (*pass_derived) (),
                (*nodes_oflw) ();

    struct pv {
	SXINT	node;
    } pv;

    struct cv {
	SXINT	visited,
	        visited_position,
	        visited_symbol,
	        father;
    } cv;

    struct nv {
	SXINT	kind,
	        node;
    } nv;
};

struct sxndtw	sxndtw;


#define NDVISITED		sxndtw.cv.visited
#define NDVISITED_POSITION	sxndtw.cv.visited_position
#define NDVISITED_SYMBOL	sxndtw.cv.visited_symbol
#define NDFATHER		sxndtw.cv.father
#define NDPREV_NODE		sxndtw.pv.node


SXINT	SXNDTW_;

#define sxndtw_item(n)		XxY_Y(sxndtw.paths,n)

#define sxndtw_father(n)	((n)==0?0:XxY_X(sxndtw.paths,n))

#define sxndtw_symbol(n)	XH_list_elem(parse_stack.output_grammar,sxndtw_item(n))

#define	sxndtw_position(n)	parse_stack.positions[sxndtw_item(n)]

#define	sxndtw_arity(n)		((sxndtw_is_erased(n)||(SXNDTW_=sxndtw_symbol(n))<0)		\
				 ?0								\
				 :parse_stack.positions[XH_X(parse_stack.output_grammar,	\
							     SXNDTW_+1)-1])

#define	sxndtw_son(n, l, s)	(XxY_set(&sxndtw.paths, 					\
					n, 							\
					XH_X(parse_stack.output_grammar,sxndtw_symbol(n))+(l),	\
					&(s)), s)

#define	sxndtw_reduce_nb(n)								\
             XxY_X (parse_stack.symbols,						\
		    XH_list_elem(parse_stack.output_grammar,				\
				 XH_X(parse_stack.output_grammar,sxndtw_symbol(n))))

#define	sxndtw_range(n)									\
             XxY_Y (parse_stack.symbols,						\
		    XH_list_elem(parse_stack.output_grammar,				\
				 XH_X(parse_stack.output_grammar,sxndtw_symbol(n))))

#define sxndtw_inf(range)	(XxY_X(parse_stack.ranges,range)+1)

#define sxndtw_sup(range)	XxY_Y(parse_stack.ranges,range)

#define	sxndtw_is_erased(n)	SXBA_bit_is_set(sxndtw.is_erased,n)


SXINT	sxndtw_is_cycle (/* node */);
SXVOID	sxndtw_open (/* void (*pass_inherited)(), void (*pass_derived)(), void (*nodes_oflw)() */);
SXVOID	sxndtw_walk ();
SXVOID	sxndtw_close ();
SXVOID	sxndtw_erase (/* node */);
BOOLEAN sxndtw_check_tree (/* node */);
BOOLEAN sxndtw_sem_calls ();
