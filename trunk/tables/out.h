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

#define P0 "NULL,"
#define P0_WITHOUT_COMMA "NULL"

extern void out_bit(char *nom, SXBA chb);

extern void out_struct(char *nom, char *elt_m1);

extern void out_tab_int(char *nom, SXINT tab[], SXINT deb, SXINT fin, char *elt_m1);

extern void out_short(char *nom);

extern void out_int(char *nom, char *elt_m1);

extern void out_extern_int(char *nom);

extern void out_char(char *nom, char *elt_m1);

extern void out_end_struct (void);
extern void out_rcvr_trans (void);
extern void out_extern_bool (char *nom);
extern void out_bitc (char *nom, SXBA chb);

