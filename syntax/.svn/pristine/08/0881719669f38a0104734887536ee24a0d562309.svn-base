typedef struct varstr_hd {
   char *first, *current, *last;
   } *VARSTR;

extern VARSTR varstr_alloc	 	();
extern VARSTR varstr_realloc	 	();
extern void   varstr_free	 	();
extern VARSTR varstr_raz	 	();
extern VARSTR varstr_catenate	 	();
extern VARSTR varstr_lcatenate	 	();
extern VARSTR varstr_quote	 	();
extern VARSTR varstr_cat_cstring 	();
extern VARSTR varstr_lcat_cstring	();
extern VARSTR varstr_modify		();
extern void   vncpy			();

#define varstr_catchar(v,c)	(*((v)->current++) = c),\
                                ((v)->current == (v)->last) ? varstr_realloc (v) : (v)
#define varstr_complete(v)	(*((v)->current) = NUL, v)
#define varstr_maxlength(v)	((v)->last - (v)->first)
#define varstr_length(v)	((v)->current - (v)->first)
#define varstr_tostr(v)		((v)->first)
