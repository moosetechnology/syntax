#define bstr_tostr(bstr)	varstr_tostr (bstr->vstr)

struct bstr {
    VARSTR	vstr;
    int		col, lmargin, rmargin;
};
/* col est la colonne du dernier caractere de vstr.
   Les caracteres significatifs de vstr se trouvent entre les colonnes
   lmargin et rmargin. */

extern struct bstr	*bstr_raz ();
extern struct bstr	*bstr_init ();
extern struct bstr	*bstr_alloc ();
extern VOID		bstr_free ();
extern VOID		bstr_get_attr ();
extern struct bstr	*bstr_shrink ();
extern struct bstr	*bstr_cat_str ();
extern struct bstr	*bstr_cat_strsp ();
