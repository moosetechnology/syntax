/*------------------------------*/
/*          tag.h          	*/
/*------------------------------*/
/*				*/
/*  VARIABLES for tag2bnf	*/
/*				*/
/*------------------------------*/

extern long		options_set;
extern bool		is_normal_form, is_test_empty_stack;
extern char		prgentname [];
extern SXTABLES	*sxtab_ptr;

#define IS_ERROR (sxerrmngr.nbmess[2]>0) /* nombre de messages d'erreurs */


