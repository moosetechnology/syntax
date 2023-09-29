#define bstr_tostr(bstr)	varstr_tostr (bstr->vstr)

struct bstr {
    VARSTR	vstr;
    SXINT		col, lmargin, rmargin;
};
/* col est la colonne du dernier caractere de vstr.
   Les caracteres significatifs de vstr se trouvent entre les colonnes
   lmargin et rmargin. */

extern struct bstr	*bstr_raz (struct bstr *bstr);
extern struct bstr	*bstr_init (struct bstr *bstr, SXINT lmargin, SXINT rmargin);
extern struct bstr	*bstr_alloc (SXINT size, SXINT lmargin, SXINT rmargin);
extern VOID		bstr_free (struct bstr *bstr);
extern VOID		bstr_get_attr (struct bstr *bstr, SXINT *lgth, SXINT *col);
extern struct bstr	*bstr_shrink (struct bstr *bstr, SXINT new_lgth, SXINT new_col);
extern struct bstr	*bstr_cat_str (struct bstr *bstr, char *str);
extern struct bstr	*bstr_cat_strsp (struct bstr *bstr, char *str);
