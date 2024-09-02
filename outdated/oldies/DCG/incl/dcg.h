/*------------------------------*/
/*          dcg.h          	*/
/*------------------------------*/
/*				*/
/*  VARIABLES for dcg2bnf	*/
/*				*/
/*------------------------------*/

long		options_set;
bool		sxverbosep, is_normal_form;
char		prgentname [32];
SXTABLES	*sxtab_ptr;

#define IS_ERROR (sxerrmngr.nbmess[2]>0) /* nombre de messages d'erreurs */

#define current_dcg_version 2
