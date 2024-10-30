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

#ifndef sxnd_h
#define sxnd_h

/*	N O N - D E T E R M I N I S T I C    P A R S I N G	*/

#ifndef SX_GLOBAL_VAR_ND /* Inutile de le faire plusieurs fois */
#ifdef SX_DFN_EXT_VAR_ND
/* Cas particulier : Il faut DÉFINIR les variables globales. */
#define SX_GLOBAL_VAR_ND	/*rien*/
#else
/* Cas général : Ce qu'on veut, c'est juste DÉCLARER les variables globales, pas les DÉFINIR. */
#define SX_GLOBAL_VAR_ND	extern
#endif /* #ifndef SX_DFN_EXT_VAR_ND */
#endif /* #ifndef SX_GLOBAL_VAR_ND */



#include 	"sxunix.h"
#include 	"X.h"
#include 	"XxY.h"
#include 	"XxYxZ.h"
#include 	"SS.h"

#if (defined(SXS_MAX) && SXS_MAX >= SHRT_MAX)

#ifndef sxndparser
#define sxndparser sxndparser32
#endif

#ifndef sxndparse_clean_forest
#define sxndparse_clean_forest sxndparse_clean_forest32
#endif

#ifndef sxndparse_pack_forest
#define sxndparse_pack_forest sxndparse_pack_forest32
#endif

#ifndef sxndparse_erase_hook_item
#define sxndparse_erase_hook_item sxndparse_erase_hook_item32
#endif

#ifndef sxndparser_GC
#define sxndparser_GC sxndparser_GC32
#endif

#ifndef sxnd2parser_GC
#define sxnd2parser_GC sxnd2parser_GC32
#endif

#ifndef sxndprecovery
#define sxndprecovery sxndprecovery32
#endif

#ifndef sxndparse_in_la
#define sxndparse_in_la sxndparse_in_la32
#endif

#ifndef sxnd2parser
#define sxnd2parser sxnd2parser32
#endif

#ifndef NDP_access
#define NDP_access NDP_access32
#endif

#ifndef new_symbol
#define new_symbol new_symbol32
#endif

#ifndef set_rule
#define set_rule set_rule32
#endif

#ifndef set_start_symbol
#define set_start_symbol set_start_symbol32
#endif

#ifndef mreds_push
#define mreds_push mreds_push32
#endif

#ifndef new_parser
#define new_parser new_parser32
#endif

#ifndef sxndsubparse_a_token
#define sxndsubparse_a_token sxndsubparse_a_token32
#endif

#ifndef reducer
#define reducer reducer32
#endif

#ifndef output_repair_string
#define output_repair_string output_repair_string32
#endif

#ifndef output_repair_list
#define output_repair_list output_repair_list32
#endif

#ifndef sxndpallcorrections
#define sxndpallcorrections sxndpallcorrections32
#endif

#ifndef ndlv_clear
#define ndlv_clear ndlv_clear32
#endif

#ifndef clone_active_scanner
#define clone_active_scanner clone_active_scanner32
#endif

#ifndef action_processing
#define action_processing action_processing32
#endif

#ifndef sxndscanner
#define sxndscanner sxndscanner32
#endif

#ifndef sxndscan_it
#define sxndscan_it sxndscan_it32
#endif

#ifndef sxndsrecovery
#define sxndsrecovery sxndsrecovery32
#endif

#endif

typedef SXINT (*sxaction_new_top_t) (SXINT, SXINT, SXINT);

extern bool	sxndparse_in_la (SXINT ep_la, SXINT Ttok_no, SXINT *Htok_no, struct sxparse_mode *mode_ptr);
extern bool	sxnd2parser (SXINT what_to_do, SXTABLES *arg);
extern bool	sxndparser (SXINT what_to_do, SXTABLES *arg);
extern bool	sxndparse_clean_forest (void);
extern void	sxndparse_pack_forest (void);
extern void	sxndparse_erase_hook_item (SXINT item);
extern void     sxndtkn_put (SXBA set);
extern void     sxndtkn_mngr (SXINT sxndtkn_mngr_what, SXINT size);

extern bool new_symbol (SXINT reduce, SXINT k, SXINT i, SXINT j, SXINT *symbol);
extern void set_rule (SXINT lhs, SXINT rhs);
extern void set_start_symbol (SXINT symbol);

/*      C T R L - S T A C K   M A N A G E R      	*/


struct ctrl_stack {
    SXBA	set;
    SXINT		*stack;
};

#define CTRL_TOP(c)		(*(c).stack)
#define CTRL_ALLOC(c,l)		((c).set=sxba_calloc(l),			\
				 (c).stack=(SXINT*)sxalloc((l)+1,sizeof(SXINT)),	\
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
				 (c).stack=(SXINT*)sxrealloc((c).stack,(l)+1,sizeof(SXINT)))

/*      E N D   C T R L - S T A C K   M A N A G E R	*/


/* INCLUDE	sxmilstn.h	*/

struct sxmilstn_elem {
    SXINT				my_index,
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
    SXINT		hd 	/* Index ds "grammar" du LHS symbol */;
    SXINT		lgth 	/* Longueur de la regle |LHS| + |RHS| */;
    SXINT		item_hd /* Index ds "grammar" de la 1ere occur de la regle courante en RHS */;
};

struct grammar {
  SXINT		rhs_symb 	/* Si <= 0 => terminal sinon index ds "rule" */;
  SXINT		lhs_rule 	/* No de la regle courante */;
  SXINT		next_item	/* Si >0 Index ds "grammar" de la l'occur suivante de rhs_symb */;
};

struct triple {
    SXINT		parser, ref, tok_no, link, symbol, re_do;
    bool	is_shift_reduce, is_valid;
};

struct for_reducer {
    struct triple	*triples;
    SXINT			top, size, re_do;
    SXINT			*refs, *triples_hd;
};

struct for_parsact {
    SXINT	(*action_top) (SXINT);
    SXINT	(*action_pop) (SXINT, SXINT, SXINT);
    SXINT	(*action_bot) (SXINT);
    sxaction_new_top_t	action_new_top;
    SXINT	(*action_final) (SXINT);
    SXINT	(*GC) (void);
    SXINT	(*post) (void);
    SXINT	(*third_field) (SXINT*, char**);
    SXINT	(*parsers_oflw) (SXINT, SXINT);
    SXINT	(*sons_oflw) (SXINT, SXINT);
    SXINT	(*symbols_oflw) (SXINT, SXINT);
    SXINT	(*post_td_init) (void);
    SXINT	(*post_td_action) (SXINT*);
    SXINT	(*post_td_final) (SXINT);
};

struct repair {
    SXINT		repair_tok_no,
                error_tok_no,
                model_no,
                kind;

    SXINT		params [5];
};

struct ndrcvr {
    SXINT			**prefixes,
                        **mm;

    SXINT			*TOK,
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

    SXINT			cur_top,
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

    SXINT			index_0X;

    bool		is_up,
	                with_parsprdct,
	                is_warning,
	                all_corrections,
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

    struct repair	*repair_list;

    char		*repair_string;

    SXINT			repair_list_size,
                        repair_list_top,
                        repair_string_size,
                        repair_string_top;
};


/* sizeof (struct  parsers_attr) == 2**3 * sizeof (SXINT) */
struct  parsers_attr {
    SXINT		son, milestone, symbol, next_for_scanner, scan_ref;
    SXINT		rhs_i [2];
    bool	is_already_processed;
};



struct parse_stack {
    /* XxY_header		parsers; */
    XxYxZ_header		parsers;

    struct parsers_attr		*parser_to_attr;

    SXBA			*parsers_set;

    SXINT				*GC_area,
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

    SXINT				start_symbol,
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

    SXINT				*transitions;

    struct for_reducer		for_reducer;

    struct for_parsact		for_parsact;

    SXINT				*red2act;

    struct SXP_reductions	*ared;

    struct G			G;

    SXBA			useful_ntset,
                                useful_tset;

    bool			is_new_links,
                                is_token_dag,
                                parsact_seen,
                                parsprdct_seen,
                                is_ambiguous,
                                /* is_unit_rules, */
                                halt_hit;

    struct ARC {
	SXINT			current_token,
	                        level,
	                        init_t_state;
    } ARC;

    struct ndrcvr		rcvr;
};


SX_GLOBAL_VAR_ND struct parse_stack	parse_stack;

struct sxndtw {
    SXINT		root;

    XxY_header	paths;

    SXBA	is_erased,
                reached_symbol,
                already_traversed;

    SXINT		(*pass_inherited) (void),
                (*pass_derived) (void);

    bool     (*cycle_processing) (void);

    void	(*open_hook) (void),
                (*close_hook) (void),
                (*nodes_oflw) (SXINT, SXINT);

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

SX_GLOBAL_VAR_ND struct sxndtw	sxndtw;


#define NDFATHER		sxndtw.cv.father
#define NDFATHER_SYMBOL		sxndtw.cv.father_symbol
#define NDFATHER_REDNO		sxndtw.cv.father_redno

#define NDVISITED		sxndtw.cv.visited
#define NDVISITED_POSITION	sxndtw.cv.visited_position
#define NDVISITED_SYMBOL	sxndtw.cv.visited_symbol
#define NDVISITED_ARITY		sxndtw.cv.visited_arity

#define NDPREV_NODE		sxndtw.pv.node


SX_GLOBAL_VAR_ND SXINT	SXNDTW_;

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


SX_GLOBAL_VAR_ND SXINT	sxndtw_is_cycle (SXINT node);
SX_GLOBAL_VAR_ND void	sxndtw_open (SXINT    (*pass_inherited) (void),
		     SXINT    (*pass_derived) (void),
		     bool  (*cycle_processing) (void),
		     void     (*open_hook) (void), 
		     void     (*close_hook) (void), 
		     void     (*nodes_oflw) (SXINT, SXINT));
SX_GLOBAL_VAR_ND void	sxndtw_init (void);
SX_GLOBAL_VAR_ND void	sxndtw_walk (void);
SX_GLOBAL_VAR_ND bool sxndrtw_walk (void);
SX_GLOBAL_VAR_ND void	sxndtw_final (void);
SX_GLOBAL_VAR_ND void	sxndtw_close (void);
SX_GLOBAL_VAR_ND void	sxndtw_node_erase (SXINT node);
SX_GLOBAL_VAR_ND void	sxndtw_check_grammar (void);
SX_GLOBAL_VAR_ND bool sxndtw_sem_calls (void);



/*  	N O N - D E T E R M I N I S T I C    S C A N N I N G	*/

struct ndincl_elem {
    struct ndincl_elem	*prev;
    SXINT			include_action;
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

SX_GLOBAL_VAR_ND struct sxndsvar {
    struct SXS_tables	SXS_tables /*	les tables du scanner en local			*/ ;
    SXTABLES	*sxtables /*	pointeur vers l'ensemble des tables		*/ ;
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


SX_GLOBAL_VAR_ND struct sxndtkn {
    sxmilstn_header		milestones;

    XxYxZ_header		dag;

    struct sxmilstn_elem	*milstn_current,
                                *milstn_head;

    SXINT				last_milestone_no;
} sxndtkn;


#define MS_BEGIN	0
#define MS_EMPTY	1
#define MS_WAIT		2
#define MS_SLEEP	4
#define MS_DEAD		8
#define MS_COMPLETE	16
#define MS_EXHAUSTED	32

extern void 	ndlv_clear (struct sxndlv *ndlv, bool keep_comments);
extern SXSCANIT_FUNCTION sxndscan_it;

#endif
