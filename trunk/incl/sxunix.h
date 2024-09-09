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

#ifndef sxunix_h

#define sxunix_h
#define sxu2_h    /* a conserver pour compatibilite ascendante (2023) */

/* Déclaration de toutes les variables et fonctions de SYNTAX */

/* RAPPEL : (voir SXante.h)
   Chaque déclaration de variable doit aussi être une définition de cette
   variable, selon la valeur donnée à SX_GLOBAL_VAR : "extern" ou rien.

   Typiquement, cela donne, pour la variable "variable" de type "type"
   et de valeur initiale "valeur" :

   SX_GLOBAL_VAR  type  variable  SX_INIT_VAL(valeur);
*/

#include "SXante.h"

#include "sxcommon.h"
/* includes stdio.h, stdlib.h, unistd.h, etc. */

#ifndef SXTAB_INTERVAL
#define SXTAB_INTERVAL 8	/* May be changed for the system */
#endif

#ifndef sxindex
#define sxindex	strchr	/* we should use sxindex() instead of the
				   index() function from <strings.h> */
#endif

#ifndef sxrindex
#define sxrindex	strrchr	/* we should use sxrindex() instead of the
				   rindex() function from <strings.h> */
#endif

#ifndef SXINLINE
#ifdef __GNUC__
#define SXINLINE	__inline__
#else
#define SXINLINE	/* inline */
#endif
#endif

#ifndef register
#define register	/* Modern C compilers do a better job without hints... :-) */
#endif

#define SXMAGIC_NUMBER	((SXUINT) 52113)
extern void sxcheck_magic_number (SXUINT m1, SXUINT m2, char *who);

#include "sxba.h"
/*
 * Unless you redefine SXBITS_PER_LONG in "sxba.h",
 * the default value will be used
 */

#include <sys/fcntl.h> /* pour O_BINARY et O_TEXT */
#include <string.h>
#include <sys/time.h>
#include <sys/types.h> /* pour open() */
#include <sys/stat.h> /* pour open() */
#include <fcntl.h> /* pour open() */
#include <time.h>  /* pour time() et ctime() */

#ifdef __CYGWIN__
#include <io.h> /* pour setmode() */
#endif

#ifndef O_BINARY
#define O_BINARY 0
#endif

#ifndef O_TEXT
#define O_TEXT 0
#endif

/*****************************************************************************/

/* Quelques petites choses bien utiles */

#include "sxalloc.h"

/*****************************************************************************/

/* --------------------------------- INDEX --------------------------------- */

#include "sxindex.h"

/*****************************************************************************/

/* Constantes servant aux initialisations et terminaisons */

#define SXBEGIN		((SXINT)(-2))
#define SXOPEN		((SXINT)(-3))
#define SXINIT		((SXINT)(-4))
#define SXACTION	((SXINT)(-5))
#define SXPREDICATE	((SXINT)(-6))
#define SXERROR		((SXINT)(-7))
#define SXFINAL		((SXINT)(-8))
#define SXSEMPASS	((SXINT)(-9))
#define SXCLOSE		((SXINT)(-10))
#define SXEND		((SXINT)(-11))
#define SXOFLW		((SXINT)(-12))
#define SXCLONE		((SXINT)(-13))
#define SXDO		((SXINT)(-14))
#define SXUNDO		((SXINT)(-15))
#define SXFORMAT        ((SXINT)(-16))
#define SXCLEAR		((SXINT)(-17))
#define SXSEPARATE      ((SXINT)(-18))
#define SXRELATIVE      ((SXINT)(-19))
#define SXABSOLUTE      ((SXINT)(-20))

#define SXRAZ		SXCLEAR /* deprecated: use SXCLEAR */

/* Constantes "dummy" */

#define SXANY_BOOL	false
#define SXANY_INT	-123456789

/* Constantes de la table des chaines */

#ifndef SXHASH_LENGTH
#define	SXHASH_LENGTH		8209
#endif	/* SXHASH_LENGTH */

#define SXERROR_STE	0	/* Chaine vide pour les erreurs */
#define SXEMPTY_STE	1	/* Veritable chaine vide */

/* Constantes du paragrapheur */

#define SXCASE			char
#define SXNO_SPECIAL_CASE	(SXCASE) 00
#define SXUPPER_CASE		(SXCASE) 01
#define SXLOWER_CASE		(SXCASE) 02
#define SXCAPITALISED_INITIAL	(SXCASE) 03


/* Constantes du tree-traverser */

#define SXINHERITED	0
#define SXDERIVED	1

/* Constante servant a separer les actions semantiques et syntaxiques */

#define SXFIRST_SYNTACTIC_ACTION 10000
#define sxis_syntactic_action(a) ((a) >= SXFIRST_SYNTACTIC_ACTION)
#define sxis_semantic_action(a)  ((a) < SXFIRST_SYNTACTIC_ACTION)

/* note : bien faire, il faudrait s'assurer que toutes les actions semantiques
 * restent strictement inferieures a SXFIRST_SYNTACTIC_ACTION ; cela pourrait
 * se faire en consultant les tables (par exemple, dans tables_c) ; aussi,
 * il faudrait s'assurer que SXP_MAX tient compte de SXFIRST_SYNTACTIC_ACTION
 * + le plus grand numero d'action syntaxique  */

/*****************************************************************************/

/* Ecritures */

SX_GLOBAL_VAR FILE *sxstdout SX_INIT_VAL (NULL);
SX_GLOBAL_VAR FILE *sxstderr SX_INIT_VAL (NULL);
SX_GLOBAL_VAR FILE *sxtty SX_INIT_VAL (NULL);

SX_GLOBAL_VAR bool sxverbosep SX_INIT_VAL (false) /* Should we animate the user's screen? Default value is false */ ;

#if (defined(SXVERSION) && ( EBUG || EBUG_ALLOC || BUG ))
#define sxbell ((char) 7)
#else /* EBUG || EBUG_ALLOC || BUG */
#define sxbell (sxverbosep ? (char) 7 : ' ')
#endif /* EBUG || EBUG_ALLOC || BUG */


/*****************************************************************************/

#if defined (__APPLE__) && defined (isascii)
/*
   Correction d'un bug sous MacOS (Nicolas Descoubes Mon May 10 15:19:05 2004)
   Dans /usr/include de ctype.h sur MacOS 10.2, isascii est defini par :
       #define isascii(c)      ((c & ~0x7F) == 0)
   Il manque des parentheses autour de "c", ce qui pose des problemes car il
   y a des lignes dans Syntax du type :
       if (isascii (sxchar = ...)).
   On redefinit donc isascii proprement.
*/
#undef isascii
#define isascii(c)      (((c) & ~0x7F) == 0)
#endif

/*****************************************************************************/

/* Definition de toutes les structures utilisees par SYNTAX */

#ifndef SHRT_MAX
#include <limits.h>
#ifndef SHRT_MAX
#define SHRT_MAX 0x7FFF
#endif
#endif

#if (defined(SXP_MAX) && SXP_MAX >= SHRT_MAX)
#define SXP_SHORT SXINT

#ifndef sxP_access
#define sxP_access sxP_access32
#endif

#ifndef sxparser
#define sxparser sxparser32
#endif

#ifndef sxprecovery
#define sxprecovery sxprecovery32
#endif

#ifndef set_first_trans
#define set_first_trans set_first_trans32
#endif

#ifndef set_next_trans
#define set_next_trans set_next_trans32
#endif

#ifndef predicate_processing
#define predicate_processing predicate_processing32
#endif

#ifndef sxparse_in_la
#define sxparse_in_la sxparse_in_la32
#endif

#ifndef ARC_traversal
#define ARC_traversal ARC_traversal32
#endif

#else
#define SXP_SHORT short
#endif

#if (defined(SXS_MAX) && SXS_MAX >= SHRT_MAX)
#define SXSTMI SXUINT

#ifndef sxscanner
#define sxscanner sxscanner32
#endif

#ifndef sxscan_it
#define sxscan_it sxscan_it32
#endif

#ifndef sxsrecovery
#define sxsrecovery sxsrecovery32
#endif

#else
#define SXSTMI unsigned short
#endif

/*****************************************************************************/

/* Structures des tables elles-memes */

typedef struct sxtables SXTABLES; /* forward declaration */

typedef bool (SXSCANACT_FUNCTION) (SXINT what, SXINT action_no);
/* the type SXSCANACT_FUNCTION denotes a scanning function, which can be
   stored in SXS_tables.S_scanact */

typedef bool (SXSRECOVERY_FUNCTION) (SXINT what, SXINT state_no, unsigned char *class);
/* the type SXSRECOVERY_FUNCTION denotes a recovery function, which can be
   stored in SXS_tables.S_recovery */

typedef SXINT (SXCHECK_KEYWORD_FUNCTION) (char *token_string, SXINT token_lgth);
/* the type SXCHECK_KEYWORD_FUNCTION denotes a keyword checking function,
   which can be stored in SXS_tables.S_check_keyword */

struct SXS_tables {
    SXINT		S_last_char_code, S_termax, S_constructor_kind, S_nbcart, S_nmax,
                S_last_simple_class_no, S_counters_size, S_last_state_no;
    bool	S_is_user_action_or_prdct, S_are_comments_erased, S_is_non_deterministic;
    SXINT	        *S_is_a_keyword;
    SXINT	        *S_is_a_generic_terminal;
    SXSTMI	**S_transition_matrix;
    struct SXS_action_or_prdct_code	*SXS_action_or_prdct_code;
    char	**S_adrp;
    SXINT	        *S_syno;
    struct SXS_local_mess	*SXS_local_mess;
    unsigned char	*S_char_to_simple_class;
    SXINT	        *S_no_delete;
    SXINT	        *S_no_insert;
    char	**S_global_mess;
    SXINT	        **S_lregle;
    SXSCANACT_FUNCTION       *S_scanact;
    SXSRECOVERY_FUNCTION     *S_recovery;
    SXCHECK_KEYWORD_FUNCTION *S_check_keyword;
};

#define sxset_scanner_action(tables,function) (tables).SXS_tables.S_scanact = &(function)

typedef void (SXSCANIT_FUNCTION) (void);
/* the type SXSCANIT_FUNCTION denotes a scan function, which can be stored
   in SXP_tables.P_scan_it */

typedef bool (SXPRECOVERY_FUNCTION) (SXINT what, SXINT *at_state, SXINT latok_no);
/* the type SXPRECOVERY_FUNCTION denotes a recovery function, which can be
   stored in SXP_tables.P_recovery */

typedef bool (SXPARSACT_FUNCTION) (SXINT what, SXINT action_no);
/* the type SXPARSACT_FUNCTION denotes a parse function which can be stored
   in SXP_tables.P_parsact */

typedef void (SXDESAMBIG_FUNCTION) (SXINT what);
/* the type SXDESAMBIG_FUNCTION denotes a desambiguation function, which can
   be stored in SXP_tables.P_desambig */

typedef void (SXSEMACT_FUNCTION) (SXINT what, SXINT action_no, SXTABLES *arg);
/* the type SXSEMACT_FUNCTION denotes a semantic function which can be stored
   in SXP_tables.P_semact */

struct SXP_tables {
    SXINT	        Mrednt, Mred, Mprdct, Mfe, M0ref, Mref, mxvec, Mxvec,
                P_tmax, P_ntmax, P_nbpro, P_xnbpro;
    SXINT /* SXP_SHORT */   init_state, final_state;
    SXINT       Mprdct_list;
    SXINT		P_nbcart, P_nmax, P_maxlregle, P_validation_length,
                P_followers_number, P_sizofpts, P_max_prio_X, P_min_prio_0; /* P_followers_numbers >= 0 */
    struct SXP_reductions	*reductions;
    struct SXP_bases		*t_bases;
    struct SXP_bases		*nt_bases;
    struct SXP_item		*vector;
    SXBA			*PER_trans_sets;
/*  struct SXP_goto		*gotos; */
    struct SXP_prdct		*prdcts;
    SXP_SHORT	                *germe_to_gr_act;
    struct SXP_prdct_list	*prdct_list;
    SXINT		        **P_lregle;
    SXINT			*P_right_ctxt_head;
    struct SXP_local_mess	*SXP_local_mess;
    SXINT		        *P_no_delete;
    SXINT		        *P_no_insert;
    char			**P_global_mess;
    SXINT		        *PER_tset;
    SXSCANIT_FUNCTION		*P_scan_it;
    SXPRECOVERY_FUNCTION	*P_recovery;
    SXPARSACT_FUNCTION		*P_parsact;
    SXDESAMBIG_FUNCTION		*P_desambig;
    SXSEMACT_FUNCTION		*P_semact;
};

typedef void (SXSEMPASS_FUNCTION) (SXINT what, SXTABLES *sxtables_ptr);
/* the type SXSEMPASS_FUNCTION denotes a semantic-pass function, which can
   be stored in SXT_tables.T_sempass */

struct SXT_tables {
    struct SXT_node_info  *SXT_node_info;
    SXINT                 *T_ter_to_node_name;
    char	          *T_stack_schema;
    SXSEMPASS_FUNCTION    *T_sempass;
    char	          **T_node_name;
};

struct SXPP_schema {
    SXINT	PP_codop, PP_act;
};

/* To bypass the stupid bug in the type-checking on pointers towards union : */

typedef union {
	    struct SXT_tables	SXT_tables;
	    struct SXPP_schema	*PP_tables;
}	sxsem_tables;

/********************************************/
/* Actions & Predicates specified from a    */
/* Linear Indexed Grammar (see lig2bnf)	    */
/********************************************/

struct sxligparsact {
    SXINT	*prdct_or_act_disp;
    SXINT       *prdct_or_act_code;
    SXINT       *map_disp;
    SXINT       *map_code;
    SXPARSACT_FUNCTION *parsact; /* Possible user's action or predicates */
};

/* ------------------------------ THE TABLES ------------------------------ */

typedef void (SXSCANNER_FUNCTION) (SXINT what, SXTABLES *arg);
/* the type SXSCANNER_FUNCTION denotes a scanning function, which can be stored
   in sxtables.SX_scanner */

typedef void (SXPARSER_FUNCTION) (SXINT what, SXTABLES *arg);
/* the type SXPARSER_FUNCTION denotes a parsing function, which can be stored
   in sxtables.SX_parser */

struct sxtables {
    SXUINT	magic;
    SXSCANNER_FUNCTION *SX_scanner;
    SXPARSER_FUNCTION *SX_parser;
    struct SXS_tables	SXS_tables;
    struct SXP_tables	SXP_tables;
    char	**err_titles /* 0 = comment, 1 = warning, 2 = error, ... */ ;
    char	*abstract;
    sxsem_tables	*sem_tables;
    struct sxligparsact	*sxligparsact; /* Ajout le 17/08/94 */
};

/*****************************************************************************/

/* Reperes vers le source analyse */

struct sxsource_coord {
    char	*file_name /*		le nom du fichier	*/ ;
    SXUINT	line /*		le numero de ligne	*/ ;
    SXUINT	column /*	le numero de colonne	*/ ;
};



/* Symbole rendu par le scanner */

struct sxtoken {
    SXINT		lahead;
    SXINT	string_table_entry;
    struct sxsource_coord	source_index;
    char	*comment;
};



/* Un element de la table de sauvegarde des chaines */

typedef struct sxstrtable {
    char	*pointer;
    SXUINT	length;
    SXINT	collision_link;
} sxstrtable_t;



/* Un tampon pour la sauvegarde des chaines */

typedef struct sxstrstring {
    struct sxstrstring	*next;
    char	chars [sizeof (char*)];
} sxstrstring_t;



/* Variables du string_manager */

typedef struct sxstrmngr {
    sxstrtable_t	*table;
    sxstrstring_t	*string;
    SXINT	*head;
    SXUINT	tablength;
    SXINT	top;
    SXUINT	strlength;
    SXUINT	strnext;
}	sxstrmngr_t;

SX_GLOBAL_VAR sxstrmngr_t sxstrmngr;


/* Variables du source_manager */

typedef struct sxrelocation *sxrelocation_list;

struct sxrelocation {
    SXUINT sxcurrent_line;    /* equal to the current line number in the source
                                 file when sxsrc_mngr (SXRELATIVE, F, L) or
                                 sxsrc_mngr (SXABSOLUTE, F, L) has been
                                 called */
    char *sxrelocated_file;   /* equal to strdup (F) if either
                                 sxsrc_mngr (SXRELATIVE, F, L) or
                                 sxsrc_mngr (SXABSOLUTE, F, L) has been called;
                                 F is expected to be a filename that replaces,
                                 in error messages, the name of the main file
                                 being parsed (but not the names of included
                                 files) */
    SXINT sxline_increment;   /* 0 by default, but equal to a signed integer if
                                 either sxsrc_mngr (SXRELATIVE, F, L) or
                                 sxsrc_mngr (SXABSOLUTE, F, L) has been called;
                                 L is expected to be either an increment that,
                                 in error messages, will be added to the
                                 current line number of the main file being
                                 parsed (but not to line numbers of included
                                 files) */
    sxrelocation_list sxnext; /* next relocation entry */
};

SX_GLOBAL_VAR struct sxsrcmngr {
    FILE	*infile /*			Fichier en cours de lecture		*/ ;
    struct sxsource_coord	source_coord /*	Reperes du caractere courant		*/ ;
    short	previous_char, current_char /*	Caracteres precedent et courant		*/ ;
    short	*buffer /*			chaine des caracteres lus en avance	*/ ;
    struct sxsource_coord	*bufcoords /*	Reperes associes			*/ ;
    SXINT		buflength /*			taille de "buffer"			*/ ;
    SXINT		bufused /*			indice du dernier caractere		*/ ;
    SXINT		bufindex /*			indice du caractere courant		*/ ;
    SXINT		labufindex /*			indice en avance			*/ ;
    bool	has_buffer /*			buffer valide?				*/ ;

    unsigned char	*instring /*		Chaine de car. en cours de lecture	*/ ;
    SXINT		strindex /*			index du caractere courant		*/ ;
    sxrelocation_list sxrelocations; /* List of relocation entries encountered
                                        so far during the scan of the source
                                        file. NULL by default, but non empty if
                                        sxsrc_mngr (SXRELATIVE, F, L) or
                                        sxsrc_mngr (SXABSOLUTE, F, L) has been
                                        called */
}	sxsrcmngr;



/* Definitions de l'error manager */

#include "sxerr_mngr.h"

/* Variables du scanner */

#define SXNORMAL_SCAN	0
#define SXLA_SCAN	1
#define SXRCVR_SCAN	2

struct sxscan_mode {
    SXUINT	errors_nb;
    SXUINT	mode /* SXNORMAL_SCAN, SXLA_SCAN, SXRCVR_SCAN */;
    bool		is_silent; /* no error message */
    bool		with_system_act; /* scanning system actions should be performed */
    bool		with_user_act; /* scanning user actions should be performed */
    bool		with_system_prdct; /* scanning system predicates should be performed */
    bool		with_user_prdct; /* scanning user predicates should be performed */
};

struct sxlv_s {
    SXINT	*counters /* 		taille = S_counters_size	*/ ;
    SXINT		include_no /*		nombre d'includes		*/ ;
    SXINT		ts_lgth_use /*		longueur utilisee dans ts	*/ ;
    char	*token_string /*	buffer du token courant		*/ ;
};


struct sxlv {
    SXINT		ts_lgth /*			longueur courante de ts		*/ ;
    SXINT		current_state_no /*		etat courant du scanner		*/ ;
    struct sxtoken	terminal_token /*	structure passee au parser	*/ ;
    unsigned char	source_class /*		classe du caractere		*/ ;
    SXINT		include_action /*		post-action pour les includes	*/ ;
    short	previous_char /*		precedant le token courant	*/ ;
    struct mark {
	struct sxsource_coord	source_coord;
	SXINT	index /*			indice dans ts du mark		*/ ;
	short		previous_char /*	precedant la marque		*/ ;
    }	mark;
    struct sxscan_mode	mode;
};


SX_GLOBAL_VAR struct sxsvar {
    struct SXS_tables	SXS_tables /*	les tables					*/ ;
    struct sxlv_s	sxlv_s /*	variables locales				*/ ;
    struct sxlv		sxlv /*		variables locales a sauver pour include		*/ ;
    SXTABLES		*sxtables;
}	sxsvar;



/* Variables du parser */

struct sxparstack {
    struct sxtoken	token;
    SXINT			state;
};


SX_GLOBAL_VAR struct sxpglobals {
    struct sxparstack	*parse_stack /* lgt1 */ ;
    SXINT			reduce, xps, pspl, stack_bot;
} sxpglobals;


/* For parser mode */
#define SXPARSER		0
#define SXRECOGNIZER		1
#define SXGENERATOR		2
#define SXDESAMBIG		3

#define SXWITHOUT_ERROR		0
#define SXWITH_RECOVERY		1
#define SXWITH_CORRECTION	2

struct sxparse_mode {
    SXUINT	mode; /* SXPARSER, SXRECOGNIZER, SXGENERATOR */
    SXUINT	kind; /* SXWITH_RECOVERY, SXWITH_CORRECTION, SXWITHOUT_ERROR */
    SXUINT	local_errors_nb;
    SXUINT	global_errors_nb;
    SXUINT	look_back; /* 0 => infinity, k>0 => look_back of k tokens. */
    bool		is_prefixe; /* parse a prefixe */
    bool		is_silent; /* no error message */
    bool		with_semact; /* semantic actions should be performed */
    bool		with_parsact; /* parsing actions should be performed */
    bool		with_parsprdct; /* parsing predicates should be performed */
    bool		with_do_undo; /* do (and undo) parsing actions in look_ahead */
};

struct sxp_a_la_rigueur {
    SXINT		*correction;
    SXINT		modele_no;
};

struct sxp_ARC_mod {
    SXINT		modele_no;
    /* SXINT */ SXP_SHORT           ref;
    SXINT               lgth, checked_lgth;
};

struct sxp_lrcvr {
    SXINT			min_xs, nomod, TOK_0, TOK_i; 
    bool			is_correction_on_error_token, truncate_context, has_prdct;
    char			**com;
    SXINT			*ARC_ster, *ster, *lter;
    SXP_SHORT	                *stack, *st_stack;
    struct sxtoken		*toks;
    SXBA			modified_set, vt_set, trans_set, to_be_checked;
    struct sxp_a_la_rigueur	a_la_rigueur;
    struct sxp_ARC_mod		ARC_best, ARC_a_la_rigueur;
    struct SXP_item		*mvector, *Mvector;
    SXINT			*undo_stack;
    SXBA                        *PER_trans_sets;
    SXINT                       glbl_best_mod;
};

#include "varstr.h"

struct sxcomment {
  SXINT  vstr_heads_top, vstr_heads_size;
  VARSTR *vstr_heads;
};


SX_GLOBAL_VAR struct sxplocals {
    struct SXP_tables	SXP_tables;
    SXTABLES		*sxtables;
    SXINT /* SXP_SHORT */   state;
    /* token manager */
    struct sxtoken	**toks_buf	/* Support Physique */ ;
    SXINT			atok_no		/* Pour les actions  */ ,
    			ptok_no		/* Pour les predicats */ ,
    			Mtok_no		/* Numero du dernier token emmagasine */ ,
    			min		/* Page de debut du buffer */ ,
    			max		/* Page de fin du buffer */ ,
                        left_border	/* Numero du premier token accessible */ ;
    struct sxparse_mode mode;
    struct sxp_lrcvr	rcvr;
    struct sxcomment    sxcomment;
}	sxplocals;


/*****************************************************************************/


SX_GLOBAL_VAR struct sxliglocals {
    struct sxligparsact	code;
    SXTABLES	*sxtables;
    /* Pile // a la pile d'analyse, contient des numeros de pile ou -1 */
    SXINT			*DO_stack;	/* Geree par SXDO */
    SXINT			DO_stack_size;
    sxindex_header	tank;		/* Reserve des numeros de pile */
    SXINT			**stacks;	/* // a tank, contient les piles allouees ou NULL. */
    struct store {
	struct {
	    SXINT		*ss,
	                cur_bot;
	}		act,
	                pop;
    } 			*where [2],
                        store1,
                        store2;
} sxliglocals;



/*****************************************************************************/

/* Variables du paragrapheur */

#ifndef SXPP_STRING_INFO
#define SXPP_STRING_INFO double /* anything */
#endif	/* SXPP_STRING_INFO */

#ifndef SXPP_SAVED_INFO
#define SXPP_SAVED_INFO double /* anything */
#endif	/* SXPP_SAVED_INFO */


SX_GLOBAL_VAR struct sxppvariables {
    SXTABLES	*sxtables;
    struct SXPP_schema	**PP_tables;
    SXPP_STRING_INFO	*terminal_string_info;
    SXPP_SAVED_INFO	*save_stack;
    SXINT		tsi_top;
    SXINT		save_stack_length;
    short int	tmax;
    SXCASE	kw_case /*		How should keywords be written				*/ ;
    SXCASE	*terminal_case /*	Same as kw_case, but for each type of terminal		*/ ;
    bool	kw_dark /*		Should keywords be artificially darkened		*/ ;
    bool	*terminal_dark /*	Same as kw_dark, but for each type of terminal		*/ ;
    bool	no_tabs /*		Do not optimize spaces into tabs			*/ ;
    bool	block_margin /*		Do not preserve structure when deeply nested		*/ ;
    SXINT 	line_length /*		What it says						*/ ;
    short int	max_margin /*		Do not indent lines further than that			*/ ;
    SXINT 	tabs_interval /*	number of columns between two tab positions		*/ ;
    bool	is_error /*		true if the pretty-printer detected an error anywhere	*/ ;
    SXINT	char_count /*		number of chars output by the pretty-printer		*/ ;
    char	logical_file_name [20] /* name of the temporary file containing the logical pretty-printer */ ;
}	sxppvariables;


/*****************************************************************************/

/* Structures utilisees pour la definition des tables */

/* -------------------------------- SCANNER -------------------------------- */

/* structure pour la correction d'erreurs */

/* attention: SXSTMI n'est correctement défini que dans les tables et dans les fichiers qui ont
un petit frère *32.c; les déclarations suivantes ne sont utilisées que dans de tels fichiers */

struct SXS_correction_item {
    SXINT	*model;
    SXINT		model_no;
    unsigned char	simple_class;
    short		rank_of_char;
};


struct SXS_action_or_prdct_code {
    SXSTMI	stmt;
    SXINT	action_or_prdct_no, param;
    bool	is_system;
    char	kind;
};


struct SXS_local_mess {
    SXINT	S_param_no;
    char	*S_string;
    SXINT	*S_param_ref;
};



/* --------------------------------- PARSER -------------------------------- */

/* attention: SXP_SHORT n'est correctement défini que dans les tables et dans les fichiers qui ont
un petit frère *32.c; les déclarations suivantes ne sont utilisées que dans de tels fichiers */

struct SXP_reductions {
    SXP_SHORT	action, reduce, lgth, lhs;
};


struct SXP_bases {
    SXP_SHORT	defaut, commun, propre;
};


struct SXP_item {
    SXP_SHORT	check, action;
};


/*
struct SXP_goto {
    SXP_SHORT	state, next;
};
*/

struct SXP_prdct {
    SXP_SHORT	prdct, action;
};


struct SXP_prdct_list {
    SXP_SHORT	red_no, pos, prdct;
};


struct SXP_local_mess {
    SXINT	P_param_no;
    char	*P_string;
    SXINT	*P_param_ref;
};



/* ---------------------------- ABSTRACT TREE ---------------------------- */

struct SXT_node_info {
    SXINT 	T_node_name;
    SXINT 	T_ss_indx;
};



/* --------------------------------- ATC --------------------------------- */

#ifndef SXNODE
#define SXNODE	struct sxnode_header_s
#endif

#ifndef SXATC_AREA
#define SXATC_AREA	double	/* anything */
#endif

/* to have the definition of sxnode_header_s look like normal C */
#define SXVOID_NAME

#define SXNODE_HEADER_S \
   SXNODE *father, *brother, *son;				\
   SXINT name, degree, position, index;				\
   bool is_list, first_list_element, last_list_element;	\
   struct sxtoken token

struct sxnode_header_s {
   SXNODE_HEADER_S SXVOID_NAME;
   };


/* atc_pp tree pointers */

struct sxnode_pp {
     struct sxnode_pp *next;	   /* abstract tree pointers		    */
     struct sxtoken terminal_token;/* lexical attributes		    */
     SXINT name;			   /* node identification		    */
				   /* >0 => internal node,		    */
				   /*  0 => error node,			    */
				   /* <0 => generic terminal (code = -name) */
     char *post_comment;	   /* lexical attributes continued	    */
   };


struct sxatc_local_variables {
    SXNODE	*abstract_tree_root;
    SXBA	early_visit_set;
    SXINT		node_size;
    union {
	struct sxatc_new {
	    sxindex_header	index_hd;
	    SXINT			line_nb;
	    SXNODE		**nodes;
	} New;
	struct sxatc_old {
	    SXATC_AREA	*area;
            void	*area_end, *current_node;
	    size_t	area_size;
	} Old;
    } u;
    bool	abstract_tree_is_error_node;
    bool	are_comments_erased;
};


SX_GLOBAL_VAR struct sxatcvar {
   struct SXT_tables SXT_tables;
   struct sxatc_local_variables atc_lv;
   } sxatcvar;



/*****************************************************************************/

/* Variables du tree-traverser */
extern void sxsmp (SXNODE *root, void (*pass_inherited) (void), void (*pass_derived) (void));

SX_GLOBAL_VAR struct sxtt_state {
    SXNODE	*visited;
    bool	visit_kind;
    SXNODE	*last_elem_or_left;
}	sxtt_state;


SX_GLOBAL_VAR struct sxnext_visit {
    SXNODE	*visited;
    bool	visit_kind, normal;
}	sxnext_visit;


#define SXVISIT_KIND	sxtt_state.visit_kind
#define SXVISITED	sxtt_state.visited
#define SXLEFT		SXLAST_ELEM
#define SXLAST_ELEM	sxtt_state.last_elem_or_left

/*****************************************************************************/
 
#define sxtime_t_initval	/* rien */
#ifndef WINNT
#define sxtime_t	struct timeval
#else
#define sxtime_t	clock_t
#ifdef SX_DFN_EXT_VAR
#undef sxtime_t_initval
#define sxtime_t_initval	= 0
#endif
#endif

/* Il n'est pas interdit de jongler avec plusieurs variables de ce type : */

SX_GLOBAL_VAR sxtime_t sxtime_v sxtime_t_initval;

extern SXINT sxtimediff (SXINT INIT_ou_ACTION);
extern void sxtimestamp (SXINT INIT_ou_ACTION, char *format);
extern void sxtime (SXINT INIT_ou_ACTION, char *str);
extern void sxtimeout (int delay); /* Abandon sxexit(4) apres delay secondes */
extern void sxvirtual_timeout (SXINT seconds, SXINT useconds); /* Abandon sxexit(4) apres seconds secondes et useconds microsecondes */
extern void sxcaught_virtual_timeout (SXINT seconds, SXINT useconds, void (*sxvirtual_timeout_handler) (int));
extern void sxcaught_timeout (int delay, void (*sxtimeout_handler) (int));
extern void sxexit_timeout (int signal_val);

/*****************************************************************************/

/* Caractere servant a marquer la fin d'une ligne */

#define SXNEWLINE	'\n'


/* Caractere servant a marquer la fin d'une chaine */

#define SXNUL	'\0'


/* Représentation externe d'un caractère */
/* On accepte les caractères signés ou non :  -129 < c < 256	  */

#ifndef SXCHAR_TO_STRING
#define SXCHAR_TO_STRING(c)	((sxc_to_str+128)[c])
  /* "extern" et non "SX_GLOBAL_VAR" car définie à part */
  extern char *sxc_to_str [384];
#endif

#ifndef SXCHAR2CSTR
#define SXCHAR2CSTR(c) ((sxcISOlatin12Cidstr+128)[c])
  /* "extern" et non "SX_GLOBAL_VAR" car définie à part */
  extern char *sxcISOlatin12Cidstr [384];
#endif /* SXCHAR2CSTR */


/* Manipulation de la casse des caractères, pour l'implémentation */
/* des actions prédéfinies de l'analyseur lexicographique.	  */
/* On accepte les caractères signés ou non :  -129 < c < 256	  */

#define sxtolower(c) ((SXL+128)[c])
#define sxtoupper(c) ((SXU+128)[c])
#define sxtonodiacritics(c) ((SXD+128)[c])
    /* "extern" et non "SX_GLOBAL_VAR" car définies à part */
extern char SXL [384], SXU [384], SXD [384];


/*****************************************************************************/

/* Divers parametres reglables */

extern void sxexit (int sevlev)
#ifdef __GNUC__
     __attribute__ ((noreturn))
#endif
     ;

#ifndef SXSRC_STRING_NAME
#define SXSRC_STRING_NAME	"<string>"	/* used when reading in a string */
#endif

/*****************************************************************************/

/* Fonctions redefinissables */

#ifndef sxgetchar
#define sxgetchar()									\
	((sxsrcmngr.infile != NULL)							\
	  ? getc (sxsrcmngr.infile)							\
	  : (sxsrcmngr.instring [++sxsrcmngr.strindex] == SXNUL				\
	    ? (--sxsrcmngr.strindex, EOF)						\
	    : sxsrcmngr.instring [sxsrcmngr.strindex]))
#endif

#ifndef sxtmpfile
   /* "tmpfile" exists -- no need to compile our own */
#define sxtmpfile	tmpfile
#else
     extern FILE	*sxtmpfile (void);
#endif

#ifndef sxnewfile
/* Création d'un fichier temporaire, qui pourra être conservé si nécessaire */
/* extern FILE	*sxnewfile (char *name) : "name" sera passé à "mktemp" ou "mkstemp" (qv) */
     extern FILE	*sxnewfile (char *name);
#endif

#ifndef sxfopen
#ifdef WINNT
     extern FILE	*sxfopen (char *s, char *m);
#else
#define sxfopen(s,m)	fopen (s, m)
#endif
#endif

#ifndef sxfreopen
#ifdef WINNT
     extern FILE	*sxfreopen (char *s, char *m, FILE *f);
#else
#define sxfreopen(s,m,f)	freopen (s, m, f)
#endif
#endif

#ifndef sxfclose
#define sxfclose(f)	fclose (f)
#endif

#ifndef sxfdopen
#ifdef WINNT
     extern FILE	*sxfdopen (SXINT d, char *m);
#else
#define sxfdopen(d,m)	fdopen (d, m)
#endif
#endif

#ifdef WINNT
#define SX_DEV_TTY	"CON"
#define SX_DEV_NUL	"NUL"
#else
#define SX_DEV_TTY	"/dev/tty"
#define SX_DEV_NUL	"/dev/null"
#endif

/************************************************************************/
/*	     Procedures et fonctions definies dans SYNTAX		*/
/************************************************************************/

/*********************************/
/* MISCELLANEOUS (see sxunix(3)) */
/*********************************/

extern void sxsyntax (SXINT syntax_what, SXTABLES *tables, ...);

#define syntax sxsyntax   /* pour compatibilite ascendante */

extern void sxopentty (void);

extern bool sxnumarg (char *arg, SXINT *nb);

extern bool sxfile_copy (char *in, char *out, SXINT char_count, char *ME_sxfile_copy);

extern SXINT sxnext_prime (SXINT germe), sxlast_bit (SXINT nb);

extern void sort_by_tree (SXINT *t, SXINT bi, SXINT bs, bool (*less_equal) (SXINT, SXINT));
   
extern void  fermer (SXBA *R, SXBA_INDEX taille);
extern void  fermer2 (SXBA *R, SXBA_INDEX taille);

extern void		sxvoid (void);
extern void		sxvoid_int (SXINT i);
extern bool		sxbvoid (void);
extern SXINT		sxivoid (void);
extern SXINT		sxivoid_int (SXINT i);
extern SXPARSACT_FUNCTION /* ou SXSCANACT_FUNCTION */ sxjvoid;
extern SXSEMPASS_FUNCTION sxsvoid;
extern SXSEMACT_FUNCTION sxwvoid;

extern SXSCANACT_FUNCTION sxscanner_action;
extern SXPARSACT_FUNCTION sxparser_action;

/**************************************/
/* STRING MANAGER (see sxstr_mngr(3)) */
/**************************************/

extern void sxstr_mngr (SXINT sxstr_mngr_what, ...);

/* General primitives operating on any string table passed as 1st argument */

/* Searches if string exists in strtable and return its unique index, or 0 if the string does not exist */
extern SXINT sxstr_retrievelen (sxstrmngr_t *strtable, char *string, SXUINT strlength);

#ifdef __GNUC__
/* here, one uses the GNUC-specific extension ({ ... }) */
#define sxstr_retrieve(strtable,string) ({ char *strvar = (string); sxstr_retrievelen ((strtable), strvar, strlen (strvar)); })
#else
extern SXINT sxstr_retrieve (sxstrmngr_t *strtable, char *string);
#endif

/* Save a string in strtable and return a unique index for it */
extern SXINT sxstr_savelen (sxstrmngr_t *strtable, char *string, SXUINT strlength);

#ifdef __GNUC__
/* here, one uses the GNUC-specific extension ({ ... }) */
#define sxstr_save(strtable,string) ({ char *strvar = (string); sxstr_savelen ((strtable), strvar, strlen (strvar)); })
#else
extern SXINT sxstr_save (sxstrmngr_t *strtable, char *string);
#endif

/* Get a pointer to a string stored in strtable from its unique index ste */
#define sxstr_get(strtable,ste)	((strtable)->table [(ste)].pointer)

/* Get the length of the string given by its unique index ste in strtable */
#define sxstr_length(strtable,ste) ((strtable)->table [ste].length)

/* Current number of elements in strtable */
#define sxstr_size(strtable) ((strtable)->top)

/* Dump the contents of strtable to file f (e.g., for debugging) */
extern void sxstr_dump (FILE *f, sxstrmngr_t *strtable);

/* Save the reserved words of language lang in strtable */
extern void sxstr_save_keywords (sxstrmngr_t *strtable, char *lang);

/* Traditional primitives that operate only on the global variable sxstrmngr */

#define sxstr2retrieve(string,strlength) sxstr_retrievelen (&sxstrmngr, (string), (strlength))

#define sxstr2save(string,strlength) sxstr_savelen (&sxstrmngr, (string), (strlength))

#define sxstrretrieve(string) sxstr_retrieve (&sxstrmngr, (string))
#define sxstrsave(string) sxstr_save (&sxstrmngr, (string))

#define sxstrget(ste)	sxstr_get (&sxstrmngr, (ste))
#define sxstrlen(ste)	sxstr_length (&sxstrmngr, (ste))
#define SXSTRtop()	(sxstr_size (&sxstrmngr))

/**************************************/
/* SOURCE MANAGER (see sxsrc_mngr(3)) */
/**************************************/

#define SXSRCsource_coord()     (sxsrcmngr.source_coord)
#define SXSRCprevious_char()    (sxsrcmngr.previous_char)
#define SXSRCcurrent_char()     (sxsrcmngr.current_char)

#ifndef sxnextchar
#define sxnextchar()							\
   (sxsrcmngr.has_buffer						\
    ? sxnext_char ()							\
    : (((sxsrcmngr.previous_char = sxsrcmngr.current_char) != SXNEWLINE	\
	? sxsrcmngr.source_coord.column++				\
	: (sxsrcmngr.source_coord.column = 1,				\
	   sxsrcmngr.source_coord.line++)),				\
	sxsrcmngr.current_char = sxgetchar ()))
#endif

extern short sxnext_char (void);
extern short sxlafirst_char (void);
extern short sxlanext_char (void);

extern void sxsrc_mngr (SXINT sxsrc_mngr_what, ...);
extern void sxsrcpush (short previous_char, char *chars, struct sxsource_coord coord);
extern void sxX (short inserted);
extern void sxlaback (SXINT backward_number);

extern char* sxget_relocated_file (struct sxsource_coord coord);
extern SXUINT sxget_relocated_line (struct sxsource_coord coord);

#define /* (char *) */ sxfile(/* struct sxsource_coord */ source_index) \
  (((sxsrcmngr.sxrelocations != NULL) && (sxincl_depth () == 0))        \
    ? (sxget_relocated_file (source_index))                             \
    : ((source_index).file_name)                                        \
  )

#define /* SXUINT */ sxline(/* struct sxsource_coord */ source_index)   \
  (((sxsrcmngr.sxrelocations != NULL) && (sxincl_depth () == 0))        \
    ? (sxget_relocated_line (source_index))                             \
    : ((source_index).line)                                             \
  )

#define /* SXUINT */ sxcolumn(/* struct sxsource_coord */ source_index) \
   ((source_index).column)

/****************************/
/* PARSER (see sxparser(3)) */
/****************************/

/* Acces a la pile d'analyse du parser */
#define SXSTACKtop()            (sxpglobals.xps)
#define SXSTACKnewtop()         (sxpglobals.xps-sxpglobals.pspl)
#define SXSTACKreduce()         (sxpglobals.reduce)
#define SXSTACKtoken(x_stack)   (sxpglobals.parse_stack [x_stack].token)

/* in the following macros, n is the index of the token in the right hand-side
 * of the production: n = 0 for the leftmost token, n = 1, 2, 3... for the
 * next tokens; notice that SXSTACKnewtop() corresponds to the leftmost token
 * and SXSTACKtop() to the rightmost token
 */

#define sxcurrent_parsed_file_name(n) (sxfile (SXSTACKtoken (SXSTACKnewtop () + (n)).source_index))

#define sxcurrent_parsed_line(n) (sxline (SXSTACKtoken (SXSTACKnewtop () + (n)).source_index))

#define sxcurrent_parsed_column(n) (sxcolumn (SXSTACKtoken (SXSTACKnewtop () + (n)).source_index))

extern bool sxparse_in_la (SXINT ep_la, SXINT Ttok_no, SXINT *Htok_no, struct sxparse_mode *mode_ptr);

extern SXINT sxP_get_parse_stack_size (void); /* permet de recuperer la taille actuelle de la parse_stack */

extern SXPARSER_FUNCTION sxparser;

extern SXPRECOVERY_FUNCTION sxprecovery;

/***************************************/
/* TOKEN_MANAGER (see sxtoken_mngr(3)) */
/***************************************/

#define SXTOKEN_AND		0x3FF
#define SXTOKEN_SHIFT		10
#define SXTOKEN_PAGE(n)		(((n)>>SXTOKEN_SHIFT)-sxplocals.min)
#define SXTOKEN_INDX(n)		((n)&SXTOKEN_AND)

/* When sxplocals.min <= SXTOKEN_PAGE(n) < sxplocals.max */
#define SXGET_TOKEN(n)		(sxplocals.toks_buf [SXTOKEN_PAGE(n)][SXTOKEN_INDX(n)])

extern void			sxtkn_mngr (SXINT sxtkn_mngr_what, SXINT size);
extern bool		sxtknzoom (SXINT token_number);
extern struct sxtoken		*sxget_token (SXINT token_number);
extern SXINT			sxtknmdf (struct sxtoken *new_seq, SXINT lgth1, SXINT old_tok_no, SXINT lgth2);

#define sxput_token(tok)	((((++sxplocals.Mtok_no)>>SXTOKEN_SHIFT) >= sxplocals.max)	\
                                ? sxtknzoom (sxplocals.Mtok_no) : 0), 				\
                                SXGET_TOKEN(sxplocals.Mtok_no) = tok

/********************/
/* COMMENT_MANAGER  */
/********************/

void   sxcomment_alloc (struct sxcomment *sxcomment_ptr, SXINT size);
void   sxcomment_clear (struct sxcomment *sxcomment_ptr);
void   sxcomment_free (struct sxcomment *sxcomment_ptr);
void   sxcomment_mngr (SXINT sxcomment_mngr_what, SXINT size);
char * sxcomment_book (struct sxcomment *sxcomment_ptr, SXUINT string_lgth);

#define sxcomment_save(c,s,l)      strncpy (sxcomment_book (c, l), s, (size_t)l)
#define sxcomment_put(s,l)         sxcomment_save (&(sxplocals.sxcomment), s, l)
#define sxcomment_catenate(ss,s,l) (strncpy (ss, s, (size_t)l)+(l))

/*******************/
/* INCLUDE MANAGER */
/*******************/

extern void sxincl_mngr (SXINT sxincl_mngr_what, ...);

extern bool sxpush_incl (char *pathname);
extern bool sxpush_recincl (char *pathname);
extern bool sxpush_uniqincl (char *pathname);

extern bool sxpop_incl (void);

extern SXINT sxincl_depth (void);
extern SXINT sxincl_max_depth (void);

extern SXINT sxincl_size (void);

extern char *sxincl_get (SXINT incl_index);
extern SXINT sxincl_get_depth (SXINT incl_index);

extern SXINT sxincl_retrieve (char *pathname);

#define sxincl_order_reading 0
#define sxincl_order_inverse_reading 1
#define sxincl_order_increasing_depth 2
#define sxincl_order_decreasing_depth 3

extern void sxincl_depend (FILE *f, SXINT order);

extern void sxincl_depend_but (FILE *f, SXINT order, SXINT excluded_index);

/******************************/
/* SCANNER (see sxscanner(3)) */
/******************************/

/* acces a l'etat du scanner et au caractere courant */

/* Warning: the macros defined here: sxcurrent_scanned_token_entry(),
 * sxcurrent_scanned_token_string(), etc. should only be used in the C code
 * written by the user to specify lexical predicates and/or lexical actions
 * invoked by the scanner. They must not be called from elsewhere, including
 * from syntactic predicates invoked by the parser. Indeed, these macros rely
 * on local variables of the scanner ("l" in "sxlv" means "local"), the
 * values of which evolve in time and are only valid at a precise moment
 * of the analysis, but not later. In particular, when a syntactic predicate
 * is invoked by the parser, the scanner might have already looked an unbounded
 * number of tokens in advance ("look-ahead"), so that the contents of the
 * variables sxsvar.sxlv(_s).* are totally undetermined.
 */

#define SXSCAN_LA_P							\
   (sxsvar.SXS_tables.S_transition_matrix [sxsvar.sxlv.current_state_no] [1])

#define SXCURRENT_SCANNED_CHAR						\
   (SXSCAN_LA_P ? sxsrcmngr.buffer[sxsrcmngr.labufindex] : sxsrcmngr.current_char)

#define sxcurrent_scanned_token_entry() (sxsvar.sxlv.terminal_token.string_table_entry)

#define sxcurrent_scanned_token_string() (sxsvar.sxlv_s.token_string)

#define sxcurrent_scanned_token_length() (sxsvar.sxlv.ts_lgth)

/* Hubert: should strcmp (sxstrget (sxcurrent_scanned_token_entry ()), sxcurrent_scanned_token_string()) always be equal to 0? */

#define sxcurrent_scanned_file_name() (sxfile (sxsvar.sxlv.terminal_token.source_index))

#define sxcurrent_scanned_line() (sxline (sxsvar.sxlv.terminal_token.source_index))

#define sxcurrent_scanned_column() (sxcolumn (sxsvar.sxlv.terminal_token.source_index))

#define sxscanner_counter(n) (sxsvar.sxlv_s.counters [(n)])

/* Utilisation de diverses informations contenues dans les tables */

#define sxttext(sxtables, look_ahead)					\
	((sxtables)->SXS_tables.S_adrp [look_ahead])

#define sxeof_code(sxtables)						\
	((sxtables)->SXS_tables.S_termax)

#define sxnextsyno(sxtables, look_ahead)				\
	((sxtables)->SXS_tables.S_syno [look_ahead])

#define sxkeywordp(sxtables, look_ahead)				\
	(sxgetbit ((sxtables)->SXS_tables.S_is_a_keyword, look_ahead) > 0)

#define sxgenericp(sxtables, look_ahead)				\
	(sxgetbit ((sxtables)->SXS_tables.S_is_a_generic_terminal, look_ahead) > 0)

extern SXINT sxgetbit (SXINT *tab, SXINT val);

extern SXSCANIT_FUNCTION sxscan_it;

extern SXSCANNER_FUNCTION sxscanner;

extern SXSRECOVERY_FUNCTION sxsrecovery;

/********************************************/
/* ABSTRACT TREE CONSTRUCTOR (see sxatc(3)) */
/********************************************/

extern SXNODE ***sxatc_stack (void);
extern void sxatc_sub_tree_erase (/* SXNODE *node */ struct sxnode_header_s *node);


/********************************************/
/* ABSTRACT TREE MANAGER (see sxat_mngr(3)) */
/********************************************/

extern SXNODE *sxson (/* struct sxnode_header_s */ void *visited, SXINT n), 
              *sxbrother (/* struct sxnode_header_s */ void *visited, SXINT n);

#define sxat_snv(kind, node_ptr)		\
	(sxnext_visit.normal = false,		\
	 sxnext_visit.visit_kind = kind,	\
	 sxnext_visit.visited = node_ptr)

/************************************************/
/* PRETTY-PRINTER OF PROGRAMS (see sxppp(3))    */
/************************************************/

extern void sxppp (SXINT sxppp_what, SXTABLES *);

/************************************************/
/* OUTPUT SOURCE LISTING (see sxlistout(3))    */
/************************************************/

extern void    sxlisting_output (FILE *LISTING);

/************************************************/
/* MISC (was in sxvars.c)                       */
/************************************************/

SX_GLOBAL_VAR char *language_name;
SX_GLOBAL_VAR char *source_file_name;
SX_GLOBAL_VAR FILE *source_file;

#define SXCOPYRIGHT "/******************************************************************************\n *                                S Y N T A X\n *-----------------------------------------------------------------------------\n *   Copyright (C) 1972-2023 INRIA (Institut National de Recherche en\n *   Informatique et Automatique)\n *-----------------------------------------------------------------------------\n *   URL: http://sourcesup.renater.fr/projects/syntax\n *-----------------------------------------------------------------------------\n *   The source code of SYNTAX is distributed with two different licenses,\n *   depending on the files:\n *   - The recursive content of src/ and incl/ and the non-recursive content\n *     of SYNTAX's root directory are distributed under the CeCILL-C license\n *   - The recursive content of all other repertories is distributed under\n *     the CeCILL license\n *   All code produced by SYNTAX must be considered as being under the\n *   CeCILL-C license. Information about the CeCILL and CeCILL-C licenses\n *   can be found at, e.g., http://www.cecill.info\n *****************************************************************************/\n"

#include "SXpost.h"

#endif /* ndef sxunix_h */

