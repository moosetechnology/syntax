/*------------------------------*/
/*          tag.h          	*/
/*------------------------------*/
/*				*/
/*  VARIABLES for tag2bnf	*/
/*				*/
/*------------------------------*/

long		options_set;
bool		sxverbosep, is_normal_form, is_test_empty_stack;
char		prgentname [32];
struct sxtables	*sxtab_ptr;

#define IS_ERROR (sxerrmngr.nbmess[2]>0) /* nombre de messages d'erreurs */


