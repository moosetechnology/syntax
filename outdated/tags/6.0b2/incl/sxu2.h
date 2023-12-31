/* ********************************************************
   *							  *
   *							  *
   * Copyright (c) 1986 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *							  *
   *							  *
   ******************************************************** */




/* ********************************************************
   *							  *
   * Produit de l'�quipe ATOLL (Langages et Traducteurs). *
   *							  *
   ******************************************************** */



/* D�claration de toutes les variables et fonctions de SYNTAX */

/* RAPPEL : (voir SXante.h)
   Chaque d�claration de variable doit aussi �tre une d�finition de cette
   variable, selon la valeur donn�e � SX_GLOBAL_VAR : "extern" ou rien.

   Typiquement, cela donne, pour la variable "variable" de type "type"
   et de valeur initiale "valeur" :

   SX_GLOBAL_VAR  type  variable  SX_INIT_VAL(valeur);
*/

#ifndef sxu2_h
#define sxu2_h
#include "SXante.h"
#include "sxcommon.h"

/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20051013 11:05 (pb):	        sxvirtual_timeout			*/
/************************************************************************/
/* 20041220 14:08 (pb):	        sxtime et sxtimeout			*/
/************************************************************************/
/* 20040220 09:54 (phd):	#if WINNT pour sxtime_t			*/
/************************************************************************/
/* 20040213 14:51 (phd):	#if WINNT pour TTY&CON			*/
/************************************************************************/
/* 20040113 13:19 (phd):	#if defined(__APPLE_CC__) pour TTY&CON	*/
/************************************************************************/
/* 20030527 13:31 (phd):	Utilisation de SX{ante,post}.h		*/
/************************************************************************/
/* 20030521 18:13 (phd):	Ajout de SX_DFN_EXT_VAR			*/
/************************************************************************/
/* 20030505 15:31 (phd):	Am�lioration de SOURCE_COORD (merci SGI)*/
/************************************************************************/
/* 20030423 15:26 (phd):	Plein de #define xxx xxx32		*/
/************************************************************************/
/* 20030328 13:04 (phd):	Ajout de "sxfile_copy"			*/
/************************************************************************/
/* 20030318 14:03 (phd):	D�finition de "sxtime_v" et ses copains	*/
/************************************************************************/
/* 20030124 10:30 (pb):	        SXSTMI est lie' a` SXS_MAX      	*/
/************************************************************************/
/* 20021220 14:25 (pb&phd):	"new" est un mot-clef r�serv� en C++	*/
/************************************************************************/
/* 20020402 16:43 (pb&phd):	Modif de la lecture dans une chaine	*/
/************************************************************************/
/* 20010328 15:28 (pb):		Fusion 3.9 + 4.0 + 5.0			*/
/************************************************************************/
/* 20020320 17:03 (phd):	D�finition de SX_DEV_{TTY,NUL}		*/
/************************************************************************/
/* 20020319 10:00 (phd):	"sxnewfile" et "sxfdopen"		*/
/************************************************************************/
/* 20001002 15:08 (phd):	Modifications diverses suite � la	*/
/*				demande d'utilisateurs (surtout VASY)	*/
/************************************************************************/
/* 20000622 14:47 (phd):	D�finition de "register" => rien...	*/
/************************************************************************/
/* 20000620 17:32 (phd):	SXHASH_LENGTH: 1013=>8209		*/
/************************************************************************/
/* 20000607 14:18 (phd):	<strings.h> => <string.h>		*/
/************************************************************************/
/* 20000605 17:44 (pb&phd):	Fin de la modif des SX[PS]_SHORT, �	*/
/*				l'exception de sxparse_in_la ()		*/
/************************************************************************/
/* 19991201 13:37 (phd):	Ajout des divers sx[ps]s?recovery32	*/
/************************************************************************/
/* 19991126 17:45 (phd):	D�finition de SXP_SHORT et SXS_SHORT	*/
/************************************************************************/
/* 19991020 11:40 (phd):	D�finition de O_TEXT (pour Windows)	*/
/************************************************************************/
/* 14-10-99 14:10 (phd):	Ajout de "sxf(re)open" pour portabilit�	*/
/************************************************************************/
/* 14-10-99 16:40 (phd) :	D�finition de O_BINARY (pour Windows)	*/
/************************************************************************/
/* 17-09-99 18:50 (phd) :	SXCHAR_TO_STRING(), sxtolower() et	*/
/*				sxtoupper()				*/
/************************************************************************/
/* 05-04-94 10:30 (pb) :	Le SXBA de "sxindex_mngr" devient int*	*/
/************************************************************************/
/* 20-01-95 13:20 (pb) :	Modif de "struct sxligparsact"		*/
/************************************************************************/
/* 17-08-94 11:48 (pb) :	Ajout de "struct sxligparsact" pour	*/
/*				traiter les Linear Indexed Grammars	*/
/*				Magic number = ?????			*/
/************************************************************************/
/* 27-05-94 14:10 (pb) :	Le source peut etre une chaine C	*/
/************************************************************************/
/* 01-04-94 10:30 (pb) :	Ajout de sxstr2retrieve			*/
/************************************************************************/
/* 24-02-94 14:06 (pb):		Utilisation de sxalloc.h                */
/************************************************************************/
/* 15-02-94 18:05 (pb):		Utilisation de sxba.h et sxindex.h	*/
/************************************************************************/
/* 11-02-94 16:25 (pb):		Ajout de "is_static" dans sxindex_header*/
/*				Ajout de sxindex_(write | read |	*/
/*				array_to_c | header_to_c | to_c | reuse)*/
/************************************************************************/
/* 24-01-94 14:30 (pb):		Ajout de sxindex_is_released et de	*/
/*				sxindex_foreach				*/
/************************************************************************/
/* 18-10-93 13:40 (pb):		Ajout de SXDO	SXUNDO			*/
/************************************************************************/
/* 30-08-93 11:18 (pb):		Ajout de SXDESAMBIG			*/
/************************************************************************/
/* 12-05-93 17:10 (pb):		Ajout de S_last_state_no		*/
/************************************************************************/
/* 06-05-93 11:55 (pb) :	Ajout de S_is_non_deterministic		*/
/************************************************************************/
/* 06-04-93 11:05 (pb) :	Ajout de P_xnbpro			*/
/************************************************************************/
/* 31-03-93 14:50 (pb) :	Ajout de la table SXP_prdct_list qui	*/
/*				permet, a l'analyse, de retrouver un	*/
/*				prdct a partir du couple (red_no, pos)	*/
/************************************************************************/
/* 01-03-93 15:10 (pb) :	Ajout de l'appel de "desambig()" dans	*/
/*				les SXP_tables (ou NULL) si non-deter	*/
/************************************************************************/
/* 25-02-93 11:25 (pb) :	Les variables locales sont mises dans	*/
/*				sxplocals.rcvr				*/
/************************************************************************/
/* 22-02-93 09:58 (pb) :	Suppression de "gotos"			*/
/*				Ajout de la SXBA* PER_trans_sets	*/
/*				Ajout de "final_state"			*/
/*				sxgetbit retourne un int		*/
/*				Magic Number = 52113			*/
/************************************************************************/
/* 05-02-93 09:15 (pb) :	La modif du 06-01-93 est annulee...	*/
/************************************************************************/
/* 27-01-93 11:15 (pb) :	Ajout du typedef 			*/
/*				struct {...} sxindex_header 		*/
/************************************************************************/
/* 06-01-93 15:30 (pb) :	Le champ token de la parse_stack devient*/
/*				un numero de token (toknb) => modif de	*/
/*				macro SXSTACKtoken et du parser		*/
/************************************************************************/
/* 06-01-93 13:57 (pb) :	Modif de sxput_token()			*/
/************************************************************************/
/* 24-09-92 10:20 (pb) :	Modif du token_mngr.			*/
/************************************************************************/
/* 18-05-92 17:30 (phd) :	#include <strings.h>			*/
/************************************************************************/
/* 21-04-92 14:10 (pb) :	Modif de sxput_token() et SXGET_TOKEN()	*/
/*				pour les memoires segmentees (HP).	*/
/************************************************************************/
/* 15-04-92 13:40 (pb) :	Ajout des modifs de la 4.0 sur les sxba	*/
/************************************************************************/
/* 30-03-92 13:10 (pb) :	Ajout des "modes" des parser et scanner	*/
/*				et qq petites choses.			*/
/************************************************************************/
/* 20-02-92 13:10 (pb) :	Definition de sxnextchar conditionnee	*/
/************************************************************************/
/* 16-08-90 12:30 (phd) :	Ajout de "sxtmpfile" pour portabilite	*/
/*				Introduction de "SXERR_NO_TABS"		*/
/************************************************************************/
/* 16-06-90 09:32 (pb) :	Modif ds sxerrmngr:			*/
/*				"char scratch_path[32]"	devient		*/
/*				"FILE *scratch_file"			*/
/************************************************************************/
/* 06-04-90 12:30 (phd) :	Introduction de "SXSTRDBG" pour la mise	*/
/*				au point (EBUG_ALLOC)			*/
/************************************************************************/
/* 06-04-89 11:10 (pb) :	Adaptation au RLR			*/
/*				Magic Number = 56431012			*/
/************************************************************************/
/* 22-06-88 14:45 (pb) :	Changement des definitions des sxalloc	*/
/*				pour lint				*/
/************************************************************************/
/* 31-05-88 13:05 (pb) :	sxBitsArray (complement)		*/
/************************************************************************/
/* 18-05-88 10:07 (pb) :	sxBitsMatrix				*/
/************************************************************************/
/* 02-05-88 12:15 (pb) :	Adaptation nouveau check_keyword	*/
/*				Magic Number = 45010			*/
/************************************************************************/
/* 07-04-88 10:05 (pb) :	Suppression des etiquettes apres #endif	*/
/************************************************************************/
/* 29-03-88 16:25 (phd) :	L'inclusion de sxbitstr.h n'est plus	*/
/*				faite ici, car les BITSTRs ne sont plus	*/
/*				distribues: ils sont (avantageusement)	*/
/*				remplaces par les bits arrays.		*/
/************************************************************************/
/* 28-03-88 16:10 (phd) :	Ajout des definitions de sxindex et	*/
/*				sxrindex, pour portages plus simples	*/
/************************************************************************/
/* 11-03-88 09:35 (pb) :	Adaptation a la V3.1a des tables du     */
/*				parser : Magic Number = 69832		*/
/************************************************************************/
/* 11-01-88 14:12 (phd&pb) :	Tentative de compatibilite VMS uMEGA	*/
/************************************************************************/
/* 28-12-87 15:00 (phd) :	Toujours plus de fonctions sur les SXBA	*/
/************************************************************************/
/* 22-12-87 11:01 (phd) :	SXBA doit etre un typedef!		*/
/*				Ajout de SXBA_ELT			*/
/************************************************************************/
/* 21-12-87 15:56 (phd) :	Ajout des declarations de sxbitsarray	*/
/************************************************************************/
/* 21-12-87 11:16 (phd) :	Ajout de SXBA et SXBITS_PER_LONG	*/
/************************************************************************/
/* 11-12-87 14:25 (pb&phd) :	Adaptation a la V3 des tables du parser	*/
/*				Magic Number = 44573			*/
/*				Au passage, modification de sxtt_state	*/
/************************************************************************/
/* 03-12-87 14:55 (phd) :	Ajout de macros pour plaire a lint	*/
/************************************************************************/
/* 30-11-87 14:05 (phd) :	Suppression de "post_source_index" dans	*/
/*				les noeuds du paragrapheur		*/
/************************************************************************/
/* 26-11-87 18:32 (phd) :	#ifndef SXCONT_ALLOC => #if 0		*/
/************************************************************************/
/* 24-11-87 17:12 (phd) :	Modification de SXEXIT: exit=>sxexit	*/
/************************************************************************/
/* 20-11-87 14:12 (phd) :	Ajout de la macro SXRESIZE		*/
/************************************************************************/
/* 20-11-87 13:50 (dp)	:	Ajout de F2ATC_TABLES			*/
/************************************************************************/
/* 19-11-87 17:13 (phd) :	Nouvelle structure pour			*/
/*				"sxatc_local_variables", avec notamment	*/
/*				suppression de la variable "father"	*/
/************************************************************************/
/* 13-11-87 15:02 (phd):	Mise dans le tas de sxstrmngr.head	*/
/************************************************************************/
/* 13-11-87 09:50 (phd):	SXHASH_LENGTH: 257=>1013		*/
/************************************************************************/
/* 09-11-87 12:34 (phd):	Plein de #ifndef			*/
/************************************************************************/
/* 21-09-87 11:13 (pb):		Introduction de SXSWAPBYTE		*/
/************************************************************************/
/* 03-07-87 09:10 (pb):		Modifications des sxtables (abstract)	*/
/* 				(magic = 56165)			 	*/
/************************************************************************/
/* 11-06-87 15:06 (pb):		Modifications des SXP (magic = 5446) 	*/
/************************************************************************/
/* 11-05-87 14:04 (pb&phd):	Les procedures de rattrapage d'erreurs 	*/
/*				rendent SXBOOLEAN, pas int.		*/
/************************************************************************/
/* 05-05-87 16:20 (pb&phd):	Changements radicaux et adequats 	*/
/************************************************************************/
/* 30-04-87 15:20 (pb):		Modifications des SXP (magic = 561023) 	*/
/************************************************************************/
/* 27-04-87 17:17 (phd):	Ajout de la declaration de sxnumarg  	*/
/************************************************************************/
/* 15-04-87 14:17 (pb):		Ajout du typedef SXSTMI	        	*/
/************************************************************************/
/* 13-04-87 12:00 (pb&phd):	Renommages de ... en SX... ou sx...  	*/
/************************************************************************/
/* 20-03-87 14:59 (pb):		Nouvelle version (magic = 293)	  	*/
/*				Pointeur vers le bon scramble, vers	*/
/*				l'error recovery du parser		*/
/************************************************************************/
/* 20-03-87 14:48 (mj):		Ajout de T_node_name dans T_tables	*/
/************************************************************************/
/* 12-03-87 13:48 (pb):		S_seed_value passee en unsigned long	*/
/************************************************************************/
/* 13-02-87 12:23 (phd):	"sxnextchar" n'est plus verbeuse	*/
/************************************************************************/
/* 12-02-87 11:19 (phd&pb):	Ajout de la macro "sxnextchar"		*/
/************************************************************************/
/* 10-02-87 17:57 (phd):	Ajout de "severity" dans "sxerr_info"	*/
/************************************************************************/
/* 30-01-87 14:16 (phd):	Ajout de EMPTY_STE et ERROR_STE		*/
/*				Renommage de SXSTRGET en sxstrget	*/
/************************************************************************/
/* 20-01-87 13:45 (phd&pb):	Modification des variables locales du	*/
/*				scanner pour Mark			*/
/************************************************************************/
/* 16-01-87 11:10 (phd):	Nouveau sxsrcmngr pour les coords	*/
/************************************************************************/
/* 16-01-87 11:07 (phd):	SCAN_CURRENT_CHAR=>CURRENT_SCANNED_CHAR	*/
/************************************************************************/
/* 05-01-87 14:52 (phd):	Creation de la structure analysers dans	*/
/*				les tables (et modification du nombre	*/
/*				magique...)				*/
/************************************************************************/
/* 31-12-86 15:39 (pb):		Les structures du tree traverser sont	*/
/*			        nommees (appel recursif de sxsmp)	*/
/************************************************************************/
/* 19-12-86 16:17 (phd):	Redefinition de sxalloc -> cont_malloc	*/
/************************************************************************/
/* 17-12-86 11:28 (phd):	Definition de la macro "sxat_snv"	*/
/************************************************************************/
/* 05-12-86 14:15 (phd):	Error manager plus complexe		*/
/************************************************************************/
/* 01-12-86 10:55 (phd&pb):	Ajout de MAGIC_NUMBER			*/
/************************************************************************/
/* 31-10-86 18:34 (phd):	Ajout de SHORT		  		*/
/************************************************************************/
/* 23-10-86 15:26 (phd):	Ajout de CHAR_TO_STRING			*/
/************************************************************************/
/* 20-10-86 15:58 (phd):	Moralisation de l'utilisation de VOID	*/
/************************************************************************/
/* 20-10-86 15:23 (phd):	Modification des sxppvariables		*/
/************************************************************************/
/* 17-10-86 15:32 (phd):	Ajout de "sxtty" pour le mode verbeux	*/
/************************************************************************/
/* 15-10-86 18:30 (phd & bl):	Ajout de la semantique du paragrapheur	*/
/************************************************************************/
/* 14-10-86 12:35 (phd):	Ajout des macros du scanner "sxccc",	*/
/*				"sxttext", "sxeof_code", "sxnext_syno",	*/
/*				"sxkeywordp" et "sxgenericp"		*/
/************************************************************************/
/* 04-06-86 15:05 (phd):	Ajout de "NO_FILE_NAME"			*/
/************************************************************************/
/* 27-05-86 17:20 (phd):	Ajout de la macro SXSTRGET		*/
/************************************************************************/
/* 26-05-86 16:20 (mm):		Modifications dans les variables de atc	*/
/************************************************************************/
/* 22-05-86 11:15 (phd):	Suppression de "ALPHA_STRING_LENGTH",	*/
/*				qui est remplacee par un argument	*/
/*				supplementaire du string manager.	*/
/*				Soumission de la definition de		*/
/*				"HASH_LENGTH" a sa non-definition	*/
/*				prealable				*/
/************************************************************************/
/* 20-05-86 15:00 (phd):	Modification du profil de "cont_free"	*/
/************************************************************************/
/* 20-05-86 11:15 (dp) :	Source_class dans sxsvar		*/
/************************************************************************/
/* 15-05-86 16:15 (phd):	Remplacement de "BUGALLOC" par		*/
/*				"CONT_ALLOC"				*/
/************************************************************************/
/* 14-05-86 14:10 (phd):	Ajout de "sxstrretrieve"		*/
/************************************************************************/
/* 14-05-86 14:00 (mm):		Ajout des procedures du tree-traverser  */
/************************************************************************/
/* 14-05-86 11:41 (phd):	Ajout de "SEVERITIES", "sxerrmngr",	*/
/*				"SXEXIT" et "sxverbosep".		*/
/************************************************************************/
/* 14-05-86 11:40 (phd):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/




#ifdef sun
/*
 * How to reallocate a zone with a smaller size
 * WITHOUT CHANGING ITS BASE ADDRESS.
 */
# define SXRESIZE(ptr, nb, size) (SXVOID) sxrealloc (ptr, nb, size)
#endif /* SXRESIZE */

#ifndef SXVOID
# define SXVOID		void	/* May be changed (to int?) for the C compiler */
#endif
#ifndef SXSHORT
# define SXSHORT	short	/* May be changed (to int?) for the C compiler */
#endif
#ifndef SXTAB_INTERVAL
# define SXTAB_INTERVAL 8	/* May be changed for the system */
#endif

#ifndef SXERR_NO_TABS
/*
 * Some users on Apollo systems report difficulties in setting TAB positions
 * on their Display Manager output pads.  We like to have satisfied customers,
 * hence this trick.
 */
# ifdef apollo
#  ifndef SXERR_NO_NO_TABS	/* #define it to override -- unused anywhere else */
#   define SXERR_NO_TABS	/* sxerr_mngr won't output TABs */
#  endif
# endif
#endif

#ifndef sxindex
# define sxindex	strchr	/* was index for UCB, strchr for USV */
#endif
#ifndef sxrindex
# define sxrindex	strrchr	/* was rindex for UCB, strrchr for USV */
#endif
#if 0 /* Let us hope that the system's include files do the right thing */
extern char	*sxindex (), *sxrindex ();
#endif

#ifndef SXINLINE
# ifdef __GNUC__
#  define SXINLINE	__inline__
# else
#  define SXINLINE	/* inline */
# endif
#endif

#ifndef register
# define register	/* Modern C compilers do a better job without hints... :-) */
#endif

#define SXMAGIC_NUMBER	((unsigned long) 52113)
extern SXVOID sxcheck_magic_number ();


/* "sxba.h" includes <stdio.h> */

#include "sxba.h"
/*
 * Unless you redefine SXBITS_PER_LONG in "sxba.h",
 * the default value will be used
 */

#include	<sys/fcntl.h> /* pour O_BINARY et O_TEXT */
#include	<string.h>
#include	<sys/time.h>
#include        <stdlib.h>
#include        <unistd.h>
#include        <sys/types.h> /* pour open() */
#include        <sys/stat.h> /* pour open() */
#include        <fcntl.h> /* pour open() */
#include        <time.h>  /* pour time() et ctime() */

#ifndef O_BINARY
# define O_BINARY 0
#endif
#ifndef O_TEXT
# define O_TEXT 0
#endif

/*****************************************************************************/

/* Quelques petites choses bien utiles */

#include	"sxalloc.h"

/*****************************************************************************/


/* --------------------------------- INDEX --------------------------------- */

#include "sxindex.h"

/*****************************************************************************/

/* Constantes servant aux initialisations et terminaisons */

#define SXBEGIN		(-2)
#define SXOPEN		(-3)
#define SXINIT		(-4)
#define SXACTION	(-5)
#define SXPREDICATE	(-6)
#define SXERROR		(-7)
#define SXFINAL		(-8)
#define SXSEMPASS	(-9)
#define SXCLOSE		(-10)
#define SXEND		(-11)
#define SXOFLW		(-12)
#define SXCLONE		(-13)
#define SXDO		(-14)
#define SXUNDO		(-15)


/* Constantes de la table des chaines */

#ifndef SXHASH_LENGTH
#	define	SXHASH_LENGTH		8209
#endif	/* SXHASH_LENGTH */

#define SXERROR_STE	0	/* Chaine vide pour les erreurs */
#define SXEMPTY_STE	1	/* Veritable chaine vide */


/* Constante de l'error manager */

#ifndef SXSEVERITIES
# define SXSEVERITIES	3	/* 0: comment, 1: warning, 2: error */
#endif


/* Constantes du paragrapheur */

#define SXCASE			char
#define SXNO_SPECIAL_CASE	(SXCASE) 00
#define SXUPPER_CASE		(SXCASE) 01
#define SXLOWER_CASE		(SXCASE) 02
#define SXCAPITALISED_INITIAL	(SXCASE) 03


/* Constantes du tree-traverser */

#define SXINHERITED	0
#define SXDERIVED	1


/*****************************************************************************/

/* Ecritures */

SX_GLOBAL_VAR FILE *sxstdout, *sxstderr, *sxtty;


SX_GLOBAL_VAR SXBOOLEAN sxverbosep /* Should we animate the user's screen? */ ;



/*****************************************************************************/

/* Definition de toutes les structures utilisees par SYNTAX */

#ifndef SHRT_MAX
#  include <limits.h>
#  ifndef SHRT_MAX
#    define SHRT_MAX 0x7FFF
#  endif
#endif

#ifndef SXP_MAX
#  define SXP_MAX 0
#endif
#ifndef SXS_MAX
#  define SXS_MAX 0
#endif

#if SXP_MAX >= SHRT_MAX
#  define SXP_SHORT int
# ifndef sxP_access
#  define sxP_access sxP_access32
# endif
# ifndef sxparser
#  define sxparser sxparser32
# endif
# ifndef sxprecovery
#  define sxprecovery sxprecovery32
# endif
# ifndef sxpsrecovery
#  define sxpsrecovery sxpsrecovery32
# endif
# ifndef sxndparser
#  define sxndparser sxndparser32
# endif
# ifndef sxndparse_clean_forest
#  define sxndparse_clean_forest sxndparse_clean_forest32
# endif
# ifndef sxndparse_pack_forest
#  define sxndparse_pack_forest sxndparse_pack_forest32
# endif
# ifndef sxndparse_erase_hook_item
#  define sxndparse_erase_hook_item sxndparse_erase_hook_item32
# endif
# ifndef sxndparser_GC
#  define sxndparser_GC sxndparser_GC32
# endif
# ifndef sxnd2parser_GC
#  define sxnd2parser_GC sxnd2parser_GC32
# endif
# ifndef set_first_trans
#  define set_first_trans set_first_trans32
# endif
# ifndef set2_first_trans
#  define set2_first_trans set2_first_trans32
# endif
# ifndef set_next_trans
#  define set_next_trans set_next_trans32
# endif
# ifndef set2_next_trans
#  define set2_next_trans set2_next_trans32
# endif
# ifndef sxndprecovery
#  define sxndprecovery sxndprecovery32
# endif
# ifndef sxndparse_in_la
#  define sxndparse_in_la sxndparse_in_la32
# endif
# ifndef sxnd2parser
#  define sxnd2parser sxnd2parser32
# endif
# ifndef NDP_access
#  define NDP_access NDP_access32
# endif
# ifndef new_symbol
#  define new_symbol new_symbol32
# endif
# ifndef set_rule
#  define set_rule set_rule32
# endif
# ifndef set_start_symbol
#  define set_start_symbol set_start_symbol32
# endif
# ifndef mreds_push
#  define mreds_push mreds_push32
# endif
# ifndef new_parser
#  define new_parser new_parser32
# endif
# ifndef sxndsubparse_a_token
#  define sxndsubparse_a_token sxndsubparse_a_token32
# endif
# ifndef reducer
#  define reducer reducer32
# endif
# ifndef output_repair_string
#  define output_repair_string output_repair_string32
# endif
# ifndef output_repair_list
#  define output_repair_list output_repair_list32
# endif
# ifndef sxndpallcorrections
#  define sxndpallcorrections sxndpallcorrections32
# endif
# ifndef sxndpsrecovery
#  define sxndpsrecovery sxndpsrecovery32
# endif
# ifndef ndlv_clear
#  define ndlv_clear ndlv_clear32
# endif
# ifndef clone_active_scanner
#  define clone_active_scanner clone_active_scanner32
# endif
# ifndef predicate_processing
#  define predicate_processing predicate_processing32
# endif
# ifndef action_processing
#  define action_processing action_processing32
# endif
# ifndef sxparse_in_la
#  define sxparse_in_la sxparse_in_la32
# endif
# ifndef ARC_traversal
#  define ARC_traversal ARC_traversal32
# endif
#else
#  define SXP_SHORT short
#endif

#if SXS_MAX >= SHRT_MAX
#  define SXS_SHORT int
#  define SXSTMI unsigned int
# ifndef sxscanner
#  define sxscanner sxscanner32
# endif
# ifndef sxscan_it
#  define sxscan_it sxscan_it32
# endif
# ifndef sxsrecovery
#  define sxsrecovery sxsrecovery32
# endif
# ifndef sxssrecovery
#  define sxssrecovery sxssrecovery32
# endif
# ifndef sxndscanner
#  define sxndscanner sxndscanner32
# endif
# ifndef sxndscan_it
#  define sxndscan_it sxndscan_it32
# endif
# ifndef sxndsrecovery
#  define sxndsrecovery sxndsrecovery32
# endif
#else
#  define SXS_SHORT short
#  define SXSTMI unsigned short
#endif



/* Structures des tables elles-memes */

struct SXS_tables {
    int		S_last_char_code, S_termax, S_constructor_kind, S_nbcart, S_nmax,
                S_last_simple_class_no, S_counters_size, S_last_state_no;
    SXBOOLEAN	S_is_user_action_or_prdct, S_are_comments_erased, S_is_non_deterministic;
    int	        *S_is_a_keyword;
    int	        *S_is_a_generic_terminal;
    SXSTMI	**S_transition_matrix;
    struct SXS_action_or_prdct_code	*SXS_action_or_prdct_code;
    char	**S_adrp;
    int	        *S_syno;
    struct SXS_local_mess	*SXS_local_mess;
    unsigned char	*S_char_to_simple_class;
    int	        *S_no_delete;
    int	        *S_no_insert;
    char	**S_global_mess;
    int	        **S_lregle;
    int		(*scanact) ();
    SXBOOLEAN	(*recovery) ();
    int		(*check_keyword) ();
};


struct SXP_tables {
    int	        Mrednt, Mred, Mprdct, Mfe, M0ref, Mref, mxvec, Mxvec,
                P_tmax, P_ntmax, P_nbpro, P_xnbpro, init_state, final_state, Mprdct_list;
    int		P_nbcart, P_nmax, P_maxlregle, P_validation_length,
                P_followers_number, P_sizofpts, P_max_prio_X, P_min_prio_0; /* P_followers_numbers >= 0 */
    struct SXP_reductions	*reductions;
    struct SXP_bases		*t_bases;
    struct SXP_bases		*nt_bases;
    struct SXP_item		*vector;
    SXBA			*PER_trans_sets;
/*  struct SXP_goto		*gotos; */
    struct SXP_prdct		*prdcts;
    SXP_SHORT			*germe_to_gr_act;
    struct SXP_prdct_list	*prdct_list;
    int			        **P_lregle;
    int				*P_right_ctxt_head;
    struct SXP_local_mess	*SXP_local_mess;
    int			        *P_no_delete;
    int			        *P_no_insert;
    char			**P_global_mess;
    int			        *PER_tset;
    int				(*scanit) ();
    SXBOOLEAN			(*recovery) ();
    int				(*parsact) ();
    int				(*desambig) ();
    int				(*semact) ();
};


struct SXT_tables {
    struct SXT_node_info	*SXT_node_info;
    int         *T_ter_to_node_name;
    char	*T_stack_schema;
    int		(*sempass) ();
    char	**T_node_name;
};


struct SXPP_schema {
    short int	PP_codop, PP_act;
};



#ifndef F2ATC_TABLES
#  define F2ATC_TABLES	double	/* anything */
#endif


/* To bypass the stupid bug in the type-checking on pointers towards union : */

typedef union {
	    struct SXT_tables	SXT_tables;
	    struct SXPP_schema	*PP_tables;
	    F2ATC_TABLES	F2T_tables;
	}	sxsem_tables;



/********************************************/
/* Actions & Predicates specified from a    */
/* Linear Indexed Grammar (see lig2bnf)	    */
/********************************************/

struct sxligparsact {
    int	*prdct_or_act_disp,
        *prdct_or_act_code,
        *map_disp,
        *map_code;
    int	(*parsact) (); /* Possible user's action or predicates */
};

/* ------------------------------ THE TABLES ------------------------------ */

struct sxtables {
    unsigned long	magic;
    struct {
	int	(*scanner) (), (*parser) ();
    }	analyzers;
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
    unsigned long	line /*		le numero de ligne	*/ ;
    unsigned int	column /*	le numero de colonne	*/ ;
};



/* Symbole rendu par le scanner */

struct sxtoken {
    int		lahead;
    unsigned int	string_table_entry;
    struct sxsource_coord	source_index;
    char	*comment;
};



/* Un element de la table de sauvegarde des chaines */

struct sxstrtable {
    char	*pointer;
    unsigned long int	length;
    unsigned int	collision_link;
};



/* Un tampon pour la sauvegarde des chaines */

struct sxstrstring {
    struct sxstrstring	*next;
    char	chars [sizeof (char*)];
};



/* Variables du string_manager */

SX_GLOBAL_VAR struct sxstrmngr {
    struct sxstrtable	*table;
    struct sxstrstring	*string;
    unsigned int	*head;
    unsigned int	tablength;
    unsigned int	top;
    unsigned long int	strlength;
    unsigned long int	strnext;
}	sxstrmngr;



/* Variables du source_manager */

#define SXSRCsource_coord()	(sxsrcmngr.source_coord)
#define SXSRCprevious_char()	(sxsrcmngr.previous_char)
#define SXSRCcurrent_char()	(sxsrcmngr.current_char)

SX_GLOBAL_VAR struct sxsrcmngr {
    FILE	*infile /*			Fichier en cours de lecture		*/ ;
    struct sxsource_coord	source_coord /*	Reperes du caractere courant		*/ ;
    SXSHORT	previous_char, current_char /*	Caracteres precedent et courant		*/ ;
    SXSHORT	*buffer /*			chaine des caracteres lus en avance	*/ ;
    struct sxsource_coord	*bufcoords /*	Reperes associes			*/ ;
    int		buflength /*			taille de "buffer"			*/ ;
    int		bufused /*			indice du dernier caractere		*/ ;
    int		bufindex /*			indice du caractere courant		*/ ;
    int		labufindex /*			indice en avance			*/ ;
    SXBOOLEAN	has_buffer /*			buffer valide?				*/ ;

    unsigned char	*instring /*		Chaine de car. en cours de lecture	*/ ;
    int		strindex /*			index du caractere courant		*/ ;

}	sxsrcmngr;



/* Variables de l'error manager */

struct sxerr_info {
    unsigned int	file_ste /*	string table entry of source name	*/ ;
    unsigned long	line /*		source coordinates			*/ ;
    unsigned int	column;
    long	beginning, end /*	offsets in scratch file			*/ ;
    int		severity /*		severity of this message		*/ ;
};


SX_GLOBAL_VAR struct sxerrmngr {
    int		nbmess [SXSEVERITIES] /*	counts messages				*/ ;
    long	mark /*				private...				*/ ;
    int		err_kept /*			nb of messages successfully kept	*/ ;
    int		err_size /*			size of err_infos			*/ ;
    FILE	*scratch_file /*		scratch file				*/ ;
    struct sxerr_info	*err_infos /*		to retrieve messages			*/ ;
}	sxerrmngr;


/* Variables du scanner */

#define SXNORMAL_SCAN	0
#define SXLA_SCAN	1
#define SXRCVR_SCAN	2

struct sxscan_mode {
    unsigned int	errors_nb;
    unsigned int	mode /* SXNORMAL_SCAN, SXLA_SCAN, SXRCVR_SCAN */;
    SXBOOLEAN		is_silent; /* no error message */
    SXBOOLEAN		with_system_act; /* scanning system actions should be performed */
    SXBOOLEAN		with_user_act; /* scanning user actions should be performed */
    SXBOOLEAN		with_system_prdct; /* scanning system predicates should be performed */
    SXBOOLEAN		with_user_prdct; /* scanning user predicates should be performed */
};

struct sxlv_s {
    long	*counters /* 		taille = S_counters_size	*/ ;
    int		include_no /*		nombre d'includes		*/ ;
    int		ts_lgth_use /*		longueur utilisee dans ts	*/ ;
    char	*token_string /*	buffer du token courant		*/ ;
};


struct sxlv {
    int		ts_lgth /*			longueur courante de ts		*/ ;
    int		current_state_no /*		etat courant du scanner		*/ ;
    struct sxtoken	terminal_token /*	structure passee au parser	*/ ;
    unsigned char	source_class /*		classe du caractere		*/ ;
    int		include_action /*		post-action pour les includes	*/ ;
    SXSHORT	previous_char /*		precedant le token courant	*/ ;
    struct mark {
	struct sxsource_coord	source_coord;
	int	index /*			indice dans ts du mark		*/ ;
	SXSHORT		previous_char /*	precedant la marque		*/ ;
    }	mark;
    struct sxscan_mode	mode;
};


SX_GLOBAL_VAR struct sxsvar {
    struct SXS_tables	SXS_tables /*	les tables					*/ ;
    struct sxlv_s	sxlv_s /*	variables locales				*/ ;
    struct sxlv		sxlv /*		variables locales a sauver pour include		*/ ;
    struct sxtables	*sxtables;
}	sxsvar;



/* Variables du parser */

struct sxparstack {
    struct sxtoken	token;
    int			state;
};


SX_GLOBAL_VAR struct sxpglobals {
    struct sxparstack	*parse_stack /* lgt1 */ ;
    int			reduce, xps, pspl, stack_bot;
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
    unsigned int	mode; /* SXPARSER, SXRECOGNIZER, SXGENERATOR */
    unsigned int	kind; /* SXWITH_RECOVERY, SXWITH_CORRECTION, SXWITHOUT_ERROR */
    unsigned int	local_errors_nb;
    unsigned int	global_errors_nb;
    unsigned int	look_back; /* 0 => infinity, k>0 => look_back of k tokens. */
    SXBOOLEAN		is_prefixe; /* parse a prefixe */
    SXBOOLEAN		is_silent; /* no error message */
    SXBOOLEAN		with_semact; /* semantic actions should be performed */
    SXBOOLEAN		with_parsact; /* parsing actions should be performed */
    SXBOOLEAN		with_parsprdct; /* parsing predicates should be performed */
    SXBOOLEAN		with_do_undo; /* do (and undo) parsing actions in look_ahead */
};

struct sxp_a_la_rigueur {
    int		*correction;
    int		modele_no;
};

struct sxp_ARC_mod {
    int		modele_no, ref, lgth, checked_lgth;
};

struct sxp_lrcvr {
    int				min_xs, nomod, TOK_0, TOK_i; 
    BOOLEAN			is_correction_on_error_token, truncate_context, has_prdct;
    char			**com;
    int				*ARC_ster, *ster, *lter;
    SXP_SHORT			*stack, *st_stack;
    struct sxtoken		*toks;
    SXBA			modified_set, vt_set, trans_set, to_be_checked;
    struct sxp_a_la_rigueur	a_la_rigueur;
    struct sxp_ARC_mod		ARC_best, ARC_a_la_rigueur;
    struct SXP_item		*mvector, *Mvector;
    int				*undo_stack;
};


SX_GLOBAL_VAR struct sxplocals {
    struct SXP_tables	SXP_tables;
    struct sxtables	*sxtables;
    int			state;
    /* token manager */
    struct sxtoken	**toks_buf	/* Support Physique */ ;
    int			atok_no		/* Pour les actions  */ ,
    			ptok_no		/* Pour les predicats */ ,
    			Mtok_no		/* Numero du dernier token emmagasine */ ,
    			min		/* Page de debut du buffer */ ,
    			max		/* Page de fin du buffer */ ,
                        left_border	/* Numero du premier token accessible */ ;
    struct sxparse_mode mode;
    struct sxp_lrcvr	rcvr;
}	sxplocals;


/*****************************************************************************/


SX_GLOBAL_VAR struct sxliglocals {
    struct sxligparsact	code;
    struct sxtables	*sxtables;
    /* Pile // a la pile d'analyse, contient des numeros de pile ou -1 */
    int			*DO_stack;	/* Geree par DO */
    int			DO_stack_size;
    sxindex_header	tank;		/* Reserve des numeros de pile */
    int			**stacks;	/* // a tank, contient les piles allouees ou NULL. */
    struct store {
	struct {
	    int		*ss,
	                cur_bot;
	}		act,
	                pop;
    } 			*where [2],
                        store1,
                        store2;
} sxliglocals;



/*****************************************************************************/

/* Acces a la pile d'analyse du parser */
#define SXSTACKtop()		(sxpglobals.xps)
#define SXSTACKnewtop()		(sxpglobals.xps-sxpglobals.pspl)
#define SXSTACKreduce()		(sxpglobals.reduce)
#define SXSTACKtoken(x_stack)	(sxpglobals.parse_stack [x_stack].token)


/*****************************************************************************/

/* Variables du paragrapheur */

#ifndef SXPP_STRING_INFO
#define SXPP_STRING_INFO double /* anything */
#endif	/* SXPP_STRING_INFO */

#ifndef SXPP_SAVED_INFO
#define SXPP_SAVED_INFO double /* anything */
#endif	/* SXPP_SAVED_INFO */


SX_GLOBAL_VAR struct sxppvariables {
    struct sxtables	*sxtables;
    struct SXPP_schema	**PP_tables;
    SXPP_STRING_INFO	*terminal_string_info;
    SXPP_SAVED_INFO	*save_stack;
    int		tsi_top;
    int		save_stack_length;
    short int	tmax;
    SXCASE	kw_case /*		How should keywords be written				*/ ;
    SXCASE	*terminal_case /*	Same as kw_case, but for each type of terminal		*/ ;
    SXBOOLEAN	kw_dark /*		Should keywords be artificially darkened		*/ ;
    SXBOOLEAN	*terminal_dark /*	Same as kw_dark, but for each type of terminal		*/ ;
    SXBOOLEAN	no_tabs /*		Do not optimize spaces into tabs			*/ ;
    SXBOOLEAN	block_margin /*		Do not preserve structure when deeply nested		*/ ;
    short int	line_length /*		What it says						*/ ;
    short int	max_margin /*		Do not indent lines further than that			*/ ;
    short int	tabs_interval /*	number of columns between two tab positions		*/ ;
    SXBOOLEAN	is_error /*		SXTRUE if the pretty-printer detected an error anywhere	*/ ;
    long int	char_count /*		number of chars output by the pretty-printer		*/ ;
}	sxppvariables;



/*****************************************************************************/

/* Structures utilisees pour la definition des tables */

/* -------------------------------- SCANNER -------------------------------- */

/* structure pour la correction d'erreurs */

struct SXS_correction_item {
    int	*model;
    int		model_no;
    int		simple_class;
    int		rank_of_char;
};


struct SXS_action_or_prdct_code {
    SXSTMI	stmt;
    int	action_or_prdct_no, param;
    SXBOOLEAN	is_system;
    char	kind;
};


struct SXS_local_mess {
    int	S_param_no;
    char	*S_string;
    int	*S_param_ref;
};



/* --------------------------------- PARSER -------------------------------- */

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
    int	P_param_no;
    char	*P_string;
    int	*P_param_ref;
};



/* ---------------------------- ABSTRACT TREE ---------------------------- */

struct SXT_node_info {
    int 	T_node_name;
    int 	T_ss_indx;
};



/* --------------------------------- ATC --------------------------------- */

#ifndef SXNODE
#  define SXNODE	struct sxnode_header_s
#endif

#ifndef SXATC_AREA
#  define SXATC_AREA	double	/* anything */
#endif

/* to have the definition of sxnode_header_s look like normal C */
#define SXVOID_NAME

#define SXNODE_HEADER_S \
   SXNODE *father, *brother, *son;				\
   int name, degree, position, index;				\
   SXBOOLEAN is_list, first_list_element, last_list_element;	\
   struct sxtoken token

struct sxnode_header_s {
   SXNODE_HEADER_S SXVOID_NAME;
   };


/* atc_pp tree pointers */

struct sxnode_pp {
     struct sxnode_pp *next;	   /* abstract tree pointers		    */
     struct sxtoken terminal_token;/* lexical attributes		    */
     int name;			   /* node identification		    */
				   /* >0 => internal node,		    */
				   /*  0 => error node,			    */
				   /* <0 => generic terminal (code = -name) */
     char *post_comment;	   /* lexical attributes continued	    */
   };


struct sxatc_local_variables {
    SXNODE	*abstract_tree_root;
    SXBA	early_visit_set;
    int		node_size;
    union {
	struct sxatc_new {
	    sxindex_header	index_hd;
	    int			line_nb;
	    SXNODE		**nodes;
	} New;
	struct sxatc_old {
	    SXATC_AREA	*area;
	    char	*area_end, *current_node;
	    int		area_size;
	} Old;
    } u;
    SXBOOLEAN	abstract_tree_is_error_node;
    SXBOOLEAN	are_comments_erased;
};


SX_GLOBAL_VAR struct sxatcvar {
   struct SXT_tables SXT_tables;
   struct sxatc_local_variables atc_lv;
   } sxatcvar;



/*****************************************************************************/

/* Variables du tree-traverser */
extern SXVOID sxsmp ();

SX_GLOBAL_VAR struct sxtt_state {
    SXNODE	*visited;
    SXBOOLEAN	visit_kind;
    SXNODE	*last_elem_or_left;
}	sxtt_state;


SX_GLOBAL_VAR struct sxnext_visit {
    SXNODE	*visited;
    SXBOOLEAN	visit_kind, normal;
}	sxnext_visit;


#define SXVISIT_KIND	sxtt_state.visit_kind
#define SXVISITED	sxtt_state.visited
#define SXLEFT		SXLAST_ELEM
#define SXLAST_ELEM	sxtt_state.last_elem_or_left

/*****************************************************************************/
 
# ifndef WINNT
#  define sxtime_t	struct timeval
# else
#  define sxtime_t	clock_t
# endif

/* Il n'est pas interdit de jongler avec plusieurs variables de ce type : */

SX_GLOBAL_VAR sxtime_t sxtime_v;

extern long sxtimediff (int INIT_ou_ACTION);
extern VOID sxtimestamp (int INIT_ou_ACTION, char *format);
extern VOID sxtime (int INIT_ou_ACTION, char *str);
extern VOID sxtimeout (int delay); /* Abandon SXEXIT(4) apres delay secondes */
extern VOID sxvirtual_timeout (long seconds, long useconds); /* Abandon SXEXIT(4) apres seconds secondes et useconds microsecondes */

/*****************************************************************************/

/* Caractere servant a marquer la fin d'une ligne */

#define SXNEWLINE	'\n'


/* Caractere servant a marquer la fin d'une chaine */

#define SXNUL	'\0'


/* Repr�sentation externe d'un caract�re */
/* On accepte les caract�res sign�s ou non :  -129 < c < 256	  */

#ifndef SXCHAR_TO_STRING
# define SXCHAR_TO_STRING(c)	((sxc_to_str+128)[c])
    /* "extern" et non "SX_GLOBAL_VAR" car d�finie � part */
  extern char *sxc_to_str [384];
#endif


/* Manipulation de la casse des caract�res, pour l'impl�mentation */
/* des actions pr�d�finies de l'analyseur lexicographique.	  */
/* On accepte les caract�res sign�s ou non :  -129 < c < 256	  */

# define sxtolower(c) ((SXL+128)[c])
# define sxtoupper(c) ((SXU+128)[c])
    /* "extern" et non "SX_GLOBAL_VAR" car d�finies � part */
extern char SXL [384], SXU [384];


/*****************************************************************************/

/* Divers parametres reglables */

#ifndef SXEXIT
# define SXEXIT(sevlev)	sxexit (sevlev)	/* should never return */
extern SXVOID sxexit ();
#endif
#ifndef SXERROR_COORD
# define SXERROR_COORD	"%s, line %lu: column %u: "	/* args are filename, line #, col # */
#endif
#ifndef SXHEAD_COORD
# define SXHEAD_COORD	"%s: "		/* arg is filename */
#endif
#ifndef SXTAIL_COORD
# define SXTAIL_COORD	"%s: "		/* arg is filename */
#endif
#ifndef SXNO_FILE_NAME
# define SXNO_FILE_NAME	"\"\""		/* used when file name is unknown */
#endif
#ifndef SXSRC_STRING_NAME
# define SXSRC_STRING_NAME	"<string>"	/* used when reading in a string */
#endif


/*****************************************************************************/

/* Fonctions redefinissables */

#ifndef sxgetchar
# define sxgetchar()									\
	((sxsrcmngr.infile != NULL)							\
	  ? getc (sxsrcmngr.infile)							\
	  : (sxsrcmngr.instring [++sxsrcmngr.strindex] == NUL				\
	    ? (--sxsrcmngr.strindex, EOF)						\
	    : sxsrcmngr.instring [sxsrcmngr.strindex]))
#endif



#ifndef sxtmpfile
#  ifdef P_tmpdir
   /* "tmpfile" exists -- no need to compile our own */
#    define sxtmpfile	tmpfile
#  else
     extern FILE	*sxtmpfile ();
#  endif
#endif


#ifndef sxnewfile
/* Cr�ation d'un fichier temporaire, qui pourra �tre conserv� si n�cessaire */
/* extern FILE	*sxnewfile (char *name) : "name" sera pass� � "mktemp" (qv) */
     extern FILE	*sxnewfile ();
#endif


#ifndef sxfopen
#  ifdef WINNT
     extern FILE	*sxfopen ();
#  else
#    define sxfopen(s,m)	fopen (s, m)
#  endif
#endif
#ifndef sxfreopen
#  ifdef WINNT
     extern FILE	*sxfreopen ();
#  else
#    define sxfreopen(s,m,f)	freopen (s, m, f)
#  endif
#endif
#ifndef sxfclose
#    define sxfclose(f)	fclose (f)
#endif
#ifndef sxfdopen
#  ifdef WINNT
     extern FILE	*sxfdopen ();
#  else
#    define sxfdopen(d,m)	fdopen (d, m)
#  endif
#endif


#ifdef WINNT
# define SX_DEV_TTY	"CON"
# define SX_DEV_NUL	"NUL"
#else
# define SX_DEV_TTY	"/dev/tty"
# define SX_DEV_NUL	"/dev/null"
#endif


/************************************************************************/
/*	     Procedures et fonctions definies dans SYNTAX		*/
/************************************************************************/

/*********************************/
/* MISCELLANEOUS (see sxunix(3)) */
/*********************************/

extern SXVOID sxsyntax ();

extern SXVOID sxtrap (char *caller, char *message);

extern SXVOID sxopentty ();

extern SXBOOLEAN sxnumarg ();

extern SXBOOLEAN sxfile_copy ();

extern int sxnext_prime (int germe), sxlast_bit (int nb);

extern SXVOID sort_by_tree (int *, int, int, SXBOOLEAN (*) ());
   
extern SXVOID  fermer (SXBA *R, int taille);
extern SXVOID  fermer2 (SXBA *R, int taille);


/**************************************/
/* STRING MANAGER (see sxstr_mngr(3)) */
/**************************************/

extern unsigned int sxstrretrieve (), sxstr2retrieve (), sxstrsave (), sxstr2save ();
extern SXVOID INCR_SXIN_STRMACRO ();
#if !EBUG_ALLOC
/* Get a pointer to a string from its unique identifier */
#define sxstrget(ste)	(sxstrmngr.table [ste].pointer)

/* Get the length of the string associated with a unique identifier */
#define sxstrlen(ste)	(sxstrmngr.table [ste].length)
#else
/* Catch possible bugs in the use of the strings manager */
extern char	*SXSTRGET ();
extern unsigned long int	SXSTRLEN ();
SX_GLOBAL_VAR int	SXIN_STRMACRO /* SX_INIT_VAL(0) */ ;

#define sxstrget(ste)	(INCR_SXIN_STRMACRO (), SXSTRGET (ste))
#define sxstrlen(ste)	(INCR_SXIN_STRMACRO (), SXSTRLEN (ste))

#endif

/* Logical size of the string_manager data */
#define SXSTRtop()	sxstrmngr.top

extern SXVOID sxstr_mngr ();

/**************************************/
/* SOURCE MANAGER (see sxsrc_mngr(3)) */
/**************************************/

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

extern SXSHORT sxnext_char (), sxlafirst_char (), sxlanext_char ();


extern SXVOID sxsrc_mngr (), sxsrcpush (), sxX (), sxlaback ();


/****************************/
/* PARSER (see sxparser(3)) */
/****************************/

extern BOOLEAN sxparse_in_la ();

/***************************************/
/* TOKEN_MANAGER (see sxtoken_mngr(3)) */
/***************************************/

#define SXTOKEN_AND		0x3FF
#define SXTOKEN_SHIFT		10
#define SXTOKEN_PAGE(n)		(((n)>>SXTOKEN_SHIFT)-sxplocals.min)
#define SXTOKEN_INDX(n)		((n)&SXTOKEN_AND)

/* When sxplocals.min <= SXTOKEN_PAGE(n) < sxplocals.max */
#define SXGET_TOKEN(n)		(sxplocals.toks_buf [SXTOKEN_PAGE(n)][SXTOKEN_INDX(n)])

extern SXVOID			sxtkn_mngr ( /* what, buf number */);
extern SXBOOLEAN		sxtknzoom (/* new (last) token number */);
extern struct sxtoken		*sxget_token (/* n */);
extern int			sxtknmdf (/* new_seq, lgth1, old_tok_no, lgth2 */);

#define sxput_token(tok)	(((++sxplocals.Mtok_no)>>SXTOKEN_SHIFT) >= sxplocals.max)	\
                                ? sxtknzoom (sxplocals.Mtok_no) : 0, 				\
                                SXGET_TOKEN(sxplocals.Mtok_no) = tok


/*******************/
/* INCLUDE MANAGER */
/*******************/

extern SXVOID sxincl_mngr ();


extern SXBOOLEAN sxpush_incl (), sxpop_incl ();


/*************************************/
/* ERROR MANAGER (see sxerr_mngr(3)) */
/*************************************/

extern SXVOID sxput_error (), sxhmsg (), sxtmsg (), sxerr_mngr ();
extern SXVOID sxperror ();
/* PB le 13/09/04 : Sur certains systemes Linux, on a le message
 /syntax/6.0/linux-i386/src/sxperror.c:74: warning: `sys_nerr' is deprecated; use `strerror' or `strerror_r' instead
 ... */

/******************************/
/* SCANNER (see sxscanner(3)) */
/******************************/

/* acces a l'etat du scanner et au caractere courant */

#define SXSCAN_LA_P							\
   (sxsvar.SXS_tables.S_transition_matrix [sxsvar.sxlv.current_state_no] [1])

#define SXCURRENT_SCANNED_CHAR						\
   (SXSCAN_LA_P ? sxsrcmngr.buffer[sxsrcmngr.labufindex] : sxsrcmngr.current_char)

extern int sxcheck_keyword ();


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

extern int sxgetbit ();

/********************************************/
/* ABSTRACT TREE CONSTRUCTOR (see sxatc(3)) */
/********************************************/

extern SXNODE ***sxatc_stack ();
extern SXVOID sxatc_sub_tree_erase (/* NODE* */);


/********************************************/
/* ABSTRACT TREE MANAGER (see sxat_mngr(3)) */
/********************************************/

extern SXNODE *sxson (), *sxbrother ();

#define sxat_snv(kind, node_ptr)		\
	(sxnext_visit.normal = SXFALSE,		\
	 sxnext_visit.visit_kind = kind,	\
	 sxnext_visit.visited = node_ptr)

/************************************************/
/* PRETTY-PRINTER OF PROGRAMS (see sxppp(3))    */
/************************************************/

extern SXVOID sxppp (int sxppp_what, struct sxtables *);

/************************************************/
/* OUTPUT SOURCE LISTING (see sxlistout(3))    */
/************************************************/

extern SXVOID    sxlisting_output (FILE *LISTING);


#include "SXpost.h"
#endif /* sxu2_h */

/*
 * Local Variables:
 * mode: C
 * version-control: yes
 * End:
 */
