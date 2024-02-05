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

#ifndef sxdynam_scanner_h
#define sxdynam_scanner_h

#include "sxunix.h"
#include "sxword.h"
#include "XxY.h"

#include "sxreg_exp_mngr.h"


extern SXINT	sxdynam_scanner (SXINT what_to_do );
extern void	sxds_putit (void);
extern void	sxds_scanit (void);

extern struct configs*
                sxds_string_to_token (char *string, bool is_set);


struct sxdslv {
    SXINT	prev_lvno /*			c'est un clone de		*/ ;
    SXINT	state /*			cree sur cet etat		*/ ;
    SXINT	char_no /* 			sur ce nieme char		*/ ;
    SXINT	*counters /* 			taille = S_counters_size	*/ ;
    char	*token_string /*		buffer du token courant		*/ ;
    SXINT	ts_size /*			taille de ts			*/ ;
    SXINT	ts_lgth /*			longueur courante de ts		*/ ;
    struct mark mark /* 			pour l'action @mark		*/ ;
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

    bool		is_char_sets_static,
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

    bool		(*scanact) (SXINT, SXINT, SXINT) /* scanner level user's actions & predicates*/ ;
    void		(*recovery) (SXINT) /* scanner level error recovery		*/ ;
    void		(*desambig) (struct configs *, struct configs *) /* scanner level user's desambiguation	*/ ;

    SXINT			current_char_no /* No de current_char depuis le debut du token*/ ;

    short		current_char,
                        previous_char;

    bool		begin_parser;	

    struct sx_variables	sx;		/* syntax variables */
    struct sxscan_mode	mode /* 	en global pour l'instant		*/ ;
};

#ifndef SX_GLOBAL_VAR_SXDS
#define SX_GLOBAL_VAR_SXDS extern
#endif

SX_GLOBAL_VAR_SXDS struct sxdynam_scanner	*SXDS;

#endif
