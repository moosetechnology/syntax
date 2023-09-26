#ifndef varstr_h
#define varstr_h

typedef struct varstr_hd {
   char *first, *current, *last;
   } *VARSTR;

extern VARSTR varstr_alloc	 	(/* int size */);
extern VARSTR varstr_realloc	 	(/* VARSTR varstr_ptr */);
extern void   varstr_free	 	(/* VARSTR varstr_ptr */);
extern VARSTR varstr_raz	 	(/* VARSTR varstr_ptr */);
extern VARSTR varstr_rcatenate	 	(/* VARSTR varstr_ptr; char *s; int l */);
extern VARSTR varstr_lcatenate	 	(/* VARSTR varstr_ptr; char *s; int l */);
extern VARSTR varstr_quote	 	(/* VARSTR varstr_ptr; char *s */);
extern VARSTR varstr_cat_cstring 	(/* VARSTR varstr_ptr; char *s */);
extern VARSTR varstr_lcat_cstring	(/* VARSTR varstr_ptr; char *s; int l */);
extern VARSTR varstr_modify		(/* VARSTR varstr_ptr; int i, l, sl; char *s */);
extern void   vncpy			(/* char *s1, *s2; int	l2 */);

#define varstr_catenate(v,c)    (varstr_lcatenate (v, c, strlen (c)))
#define varstr_catchar(v,c)	((*((v)->current++) = c),\
                                ((v)->current == (v)->last) ? varstr_realloc (v) : (v))
#define varstr_complete(v)	(*((v)->current) = '\0', v) /* '\0' et pas NUL pour pouvoir
							       l'inclure avant (ou sans) sxunix.h,
							       par exemple pour profiter du
							       mécanisme de contrôle sur sxba2c */
#define varstr_maxlength(v)	((v)->last - (v)->first)
#define varstr_length(v)	((v)->current - (v)->first)
#define varstr_tostr(v)		((v)->first)
#define varstr_pop(v,n)		((v)->current -= n)

#endif /* varstr_h */
