#ifndef sxdynam_scanner_h
#define sxdynam_scanner_h

#include "sxunix.h"
#include "sxword.h"
#include "XxY.h"

extern void	sxre_store (void);
extern void	sxre_restore (void);
extern void	sxre_alloc (void);
extern SXINT	sxre_process_eof (SXINT eof_code);
extern void	sxre_doit (FILE *infile, char *file_name_or_source_string);
extern void	sxre_free (void);
extern SXINT	sxre_string_to_re (char *string,
				   SXINT t_code,
				   BOOLEAN is_a_generic_terminal);
extern BOOLEAN	sxre_write (filedesc_t file_descr /* file descriptor */);
extern BOOLEAN	sxre_read (filedesc_t file_descr /* file descriptor */);
extern void	sxre_array_to_c (FILE *file, char *name);
extern void	sxre_header_to_c (FILE *file, char *name);
extern void	sxre_to_c (FILE *file, char *name, BOOLEAN is_static);
extern void	sxre_reuse (void);

extern SXINT	sxdynam_scanner (SXINT what_to_do );
extern void	sxds_putit (void);
extern void	sxds_scanit (void);

extern struct configs*
                sxds_string_to_token (char *string, BOOLEAN is_set);

/* system action code */
#define LowerToUpper	(-1)
#define UpperToLower	(-2)
#define Set		(-3)
#define Reset		(-4)
#define Erase		(-5)
#define Include		(-6)
#define UpperCase	(-7)
#define LowerCase	(-8)
#define Put		(-9)
#define Mark		(-10)
#define Release		(-11)
#define Incr		(-12)
#define Decr		(-13)

/* system predicate code */
#define IsFalse		(-1)
#define IsTrue		(-2)
#define IsFirstCol	(-3)
#define	IsSet		(-4)
#define	IsReset		(-5)
#define	NotIsFirstCol	(-6)
#define IsLastCol	(-7)
#define NotIsLastCol	(-8)



#define Context			'\000'
#define ContextAll		'\001'
#define ContextAllBut		'\002'
#define UnboundedContext	'\003'
#define UnboundedContextAll	'\004'
#define UnboundedContextAllBut	'\005'


#define KEMPTY			0
#define KCLASS			1
#define KREDUCE			2
#define KACTION			4
#define KPREDICATE		8

#define NoPriority		0
#define ShiftReduce		1
#define ReduceShift		2
#define ReduceReduce		4

#define NoConflict		0
#define Shift			1
#define FirstReduce		2
#define NextReduce		4
#define Ambiguous		8

#define EOF_char		0

#define KW_NO			1
#define KW_PROHIB		2
#define KW_YES			4

#define ForeachConfig(c,p)	for (p=(c)->t+(c)->top;p>(c)->t;p--)	\
                                   if (p->init.state >= 0)

#define EraseConfig(p)		p->init.state = -1


#ifdef SCAN_LA_P
#undef SCAN_LA_P
#endif
#define SCAN_LA_P	(SXDS->mode.mode == SXLA_SCAN)

#ifdef sxttext
#undef sxttext
#endif
#define sxttext(la)	(SXWORD_get (SXDS->terminals, SXDS->token_no_to_terminal [la]))

#ifdef sxeof_code
#undef sxeof_code
#endif
#define sxeof_code()	(SXDS->eof_code)

#ifdef sxkeywordp
#undef sxkeywordp
#endif
#define sxkeywordp(la)	SXDS->terminal_to_attr[SXDS->token_no_to_terminal[la]].is_kw

#ifdef sxgenericp
#undef sxgenericp
#endif
#define sxgenericp(la)	SXDS->terminal_to_attr[SXDS->token_no_to_terminal[la]].is_generic



struct sx_variables {
    struct sxsrcmngr	sxsrcmngr;
    struct sxstrmngr	sxstrmngr;
    struct sxerrmngr	sxerrmngr;
    struct sxsvar	sxsvar;
    struct sxplocals	sxplocals;
    struct sxatcvar	sxatcvar;
};

struct terminal_to_attr {
    SXINT		token_no,
                next_syno;

    BOOLEAN	is_kw,
                is_generic;
};


struct abbrev_to_attr {
    SXINT		init_item,
                final_item;

    BOOLEAN	is_empty;
};


struct reg_exp_to_attr {
    SXINT		init_item,
                post_action,
                token_no,
                reduce_id;

    char	context_kind,
                priority_kind,
                kw_kind;
};


struct item_to_attr {
    SXINT	reg_exp_no,
                val, /* index ds char_sets, ou reduce ou action ou predicat */
                param; /* Parametre eventuel des actions ou predicats. */

    BOOLEAN	is_erased;
    SXINT        kind /* KCLASS, KREDUCE, KACTION, KPREDICATE */;
};


struct sxdslv {
    SXINT	prev_lvno /*			c'est un clone de		*/ ;
    SXINT	state /*			cree sur cet etat		*/ ;
    SXINT	char_no /* 			sur ce nieme char		*/ ;
    long	*counters /* 			taille = S_counters_size	*/ ;
    char	*token_string /*		buffer du token courant		*/ ;
    SXINT	ts_size /*			taille de ts			*/ ;
    SXINT	ts_lgth /*			longueur courante de ts		*/ ;
    struct mark mark /* 			pour l'action @mark		*/ ;
};



struct config_elem {
    SXINT		state, lvno;
};


struct config {
    struct config_elem	init, prev, current;
};


struct configs {
    SXINT				top, size;
    SXINT				prdct_nb, is_true_nb;
    struct config		*t, *last;
    char			conflict_kind, priority_kind;
};


struct sxdynam_scanner {
    sxword_header	sc_names;	/* simple class names */
    SXINT			*sc_names_to_char_sets;
                                        /* Associated char sets */
    SXBA		*char_sets;/* working area */
    SXINT			char_sets_top,
                        char_sets_size;

    sxword_header	abbrev_names;	/* abbreviation names */
    struct abbrev_to_attr
	                *abbrev_to_attr;

    sxword_header	reg_exp_names /* reg_exp names 				*/ ;
    struct reg_exp_to_attr
	                *reg_exp_to_attr;
    SXINT			current_reg_exp_or_abbrev_no,
                        current_reduce_id,
                        delta;
    SXBA		*context_sets;

    SXBA		*tfirst;
    SXINT			tfirst_line_nb,
                        tfirst_col_nb;

    struct item_to_attr	*item_to_attr;
    SXINT			item_size,
                        item_top;

    XxY_header		nfsa;

    sxword_header	terminals /* 	terminal names 				*/ ;
    struct terminal_to_attr
			*terminal_to_attr;
    SXINT			*token_no_to_terminal,
                        token_no_to_terminal_size;

    char		*ws /*		working_string_area			*/ ;
    SXINT			ws_lgth;

    SXBA		ctxt_set,
                        filter /* 	// reg_exp_names 			*/ ;

    SXINT			counters_size /* nombre de "compteurs" utilises		*/ ;

    SXINT			eof_code,
                        eof_reg_exp_name;

    BOOLEAN		is_char_sets_static,
         		is_item_to_attr_static,
         		is_token_no_to_terminal_static,
         		is_smp_error;

    SXINT			lv_size /* 	taille de dslv				*/ ;
    SXINT			lv_top /* 	sommet courant de dslv			*/ ;
    struct sxdslv	*dslv /* ensemble des variables locales disponibles	*/ ;
    SXBA		lvno_set /*	// dslv 				*/ ;
    SXINT			*lvno_clones /* // dslv 				*/ ;

    struct configs	configs [3],
                        *source_configs,
                        *target_configs,
                        *la_configs;

    SXINT			target;

    struct sxtoken	terminal_token;

    BOOLEAN		(*scanact) (SXINT, SXINT, SXINT) /* scanner level user's actions & predicates*/ ;
    void		(*recovery) (SXINT) /* scanner level error recovery		*/ ;
    void		(*desambig) (struct configs *, struct configs *) /* scanner level user's desambiguation	*/ ;

    SXINT			current_char_no /* No de current_char depuis le debut du token*/ ;

    SHORT		current_char,
                        previous_char;

    BOOLEAN		begin_parser;	

    struct sx_variables	sx;		/* syntax variables */
    struct sxscan_mode	mode /* 	en global pour l'instant		*/ ;
};

#ifndef SX_GLOBAL_VAR_SXDS
#define SX_GLOBAL_VAR_SXDS extern
#endif

SX_GLOBAL_VAR_SXDS struct sxdynam_scanner	*SXDS;

#endif
