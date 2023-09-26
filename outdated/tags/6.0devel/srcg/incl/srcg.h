#ifndef srcg_h
#define srcg_h

/*------------------------------*/
/*          srcg.h          	*/
/*------------------------------*/
/*				*/
/*  Global VARIABLES for srcg	*/
/*				*/
/*------------------------------*/



/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 19991209 18:47 (phd):	"extern" pour "by_mess"			*/
/************************************************************************/
/* 19991209 18:46 (phd):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/

/* Chaque déclaration de variable doit aussi être une définition de cette
   variable, selon la valeur donnée à SX_GLOBAL_VAR_SRCG : "extern" ou rien.

   Typiquement, cela donne, pour la variable "variable" de type "type"
   et de valeur initiale "valeur" :

   SX_GLOBAL_VAR_SRCG  type  variable  SX_INIT_VAL(valeur);
*/

#ifndef SX_GLOBAL_VAR_SRCG /* Inutile de le faire plusieurs fois */
#  ifdef SX_DFN_EXT_VAR_SRCG
/* Cas particulier : Il faut DÉFINIR les variables globales. */
#    define SX_GLOBAL_VAR_SRCG	/*rien*/
#    define SX_INIT_VAL_SRCG(v)	= v
#  else
/* Cas général : Ce qu'on veut, c'est juste DÉCLARER les variables globales, pas les DÉFINIR. */
#    define SX_GLOBAL_VAR_SRCG	extern
#    define SX_INIT_VAL_SRCG(v)	/*rien*/
#  endif /* #ifndef SX_DFN_EXT_VAR_SRCG */
#endif /* #ifndef SX_GLOBAL_VAR_SRCG */


#include "srcg_td.h"
#include "XH.h"
#include "XxY.h"

#if STATS
#define statistics	stdout
#else
#define statistics	NULL
#endif

#define DIRECT  1
#define REVERSE 2

SX_GLOBAL_VAR_SRCG FILE  *parse_file, *synchro_file;
SX_GLOBAL_VAR_SRCG char  *parse_file_name, *synchro_file_name;
SX_GLOBAL_VAR_SRCG SXINT srcg_kind;


extern SXBOOLEAN	IS_AND (SXBA lhs_bits_array, SXBA rhs_bits_array);

SX_GLOBAL_VAR_SRCG long		options_set;
SX_GLOBAL_VAR_SRCG SXBOOLEAN	is_listing_output, check_self_definition, is_CF_parser, check_set, is_OLTAG, is_1_SRCG, is_gen_bnf;
SX_GLOBAL_VAR_SRCG SXBOOLEAN	decrease_order, is_simple, is_proper, is_2var_form, is_factorize, is_combinatorial, is_full_lex;
SX_GLOBAL_VAR_SRCG SXBOOLEAN	is_left_corner, used_nfndfsa, is_first_last, is_look_ahead, is_tagger, is_shift_reduce, is_shallow;
SX_GLOBAL_VAR_SRCG SXBOOLEAN    is_keep_lex, is_keep_epsilon_rules, check_instantiable_clause;
SX_GLOBAL_VAR_SRCG SXINT        h_value;
SX_GLOBAL_VAR_SRCG char		*prgentname, *suffixname, *lfsa_file_name, *rfsa_file_name, *shallow_file_name, *lbnf_file_name, *rbnf_file_name, *Lex_file_name, *lrprod_file_name, *tig_file_name;
SX_GLOBAL_VAR_SRCG char	        by_mess [];
SX_GLOBAL_VAR_SRCG FILE		*listing, *lfsa_file, *rfsa_file, *useless_clause_file, *shallow_file, *lbnf_file, *rbnf_file, *Lex_file, *lrprod_file, *tig_file;
SX_GLOBAL_VAR_SRCG struct sxtables	*sxtab_ptr;

#define IS_ERROR (sxerrmngr.nbmess[2]>0) /* nombre de messages d'erreurs */

#define SXTOP	((((SXUINT)(~0))>>1))	/* + grand entier positif */

struct srcg_node {
    SXNODE_HEADER_S SXVOID_NAME;

/*
your attribute declarations...
*/
  SXINT ste;
};


/* ATTENTION, voir ds srcg_gen_parser la generation de "static SXINT nt2arity [] = {..." */
#define	STREQ_ic	((SXINT)-1)
#define	STREQLEN_ic	((SXINT)-2)
#define	STRLEN_ic	((SXINT)-3)
#define	FALSE_ic	((SXINT)-4)
#define	FIRST_ic	((SXINT)-5)
#define	LAST_ic		((SXINT)-6)
#define	TRUE_ic		((SXINT)-7)
#define	LEX_ic		((SXINT)-8)
/* ATTENTION, CUT_ic doit avoir le +petit code des predefinis !! */
#define	CUT_ic		((SXINT)-9)

/*
#define CURRENT_SRCG_PARSER_RELEASE p
*/

#endif /* srcg_h */
