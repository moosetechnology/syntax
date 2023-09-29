/******************************************************************************
 *                                S Y N T A X
 *-----------------------------------------------------------------------------
 *   Copyright (C) 1972-2023 INRIA (Institut National de Recherche en
 *   Informatique et Automatique)
 *-----------------------------------------------------------------------------
 *   URL: http://sourcesup.renater.fr/projects/syntax
 *-----------------------------------------------------------------------------
 *   The source code of SYNTAX is distributed with two different licenses,
 *   depending on the files:
 *   - The recursive content of src/ and incl/ and the non-recursive content
 *     of SYNTAX's root directory are distributed under the CeCILL-C license
 *   - The recursive content of all other repertories is distributed under
 *     the CeCILL license
 *   All code produced by SYNTAX must be considered as being under the
 *   CeCILL-C license. Information about the CeCILL and CeCILL-C licenses
 *   can be found at, e.g., http://www.cecill.info
 *****************************************************************************/
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
extern SXVOID		bstr_free (struct bstr *bstr);
extern SXVOID		bstr_get_attr (struct bstr *bstr, SXINT *lgth, SXINT *col);
extern struct bstr	*bstr_shrink (struct bstr *bstr, SXINT new_lgth, SXINT new_col);
extern struct bstr	*bstr_cat_str (struct bstr *bstr, char *str);
extern struct bstr	*bstr_cat_strsp (struct bstr *bstr, char *str);
