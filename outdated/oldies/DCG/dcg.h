/*------------------------------*/
/*          dcg.h          	*/
/*------------------------------*/
/*				*/
/*  VARIABLES for dcg2bnf	*/
/*				*/
/*------------------------------*/

extern long		options_set;
extern bool		is_normal_form;
extern char		prgentname[];
extern SXTABLES	*sxtab_ptr;

#define IS_ERROR (sxerrmngr.nbmess[2]>0) /* nombre de messages d'erreurs */

#define current_dcg_version 2
