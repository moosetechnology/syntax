/*------------------------------*/
/*          lig.h          	*/
/*------------------------------*/
/*				*/
/*  VARIABLES for lig2bnf	*/
/*				*/
/*------------------------------*/

long		options_set;
BOOLEAN		is_normal_form;
char		prgentname [32];
struct sxtables	*sxtab_ptr;

#define IS_ERROR (sxerrmngr.nbmess[2]>0) /* nombre de messages d'erreurs */

#define current_lig_version 2
