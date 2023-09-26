/******************************************************************************
 *                                S Y N T A X
 *-----------------------------------------------------------------------------
 *   Copyright (C) 1972-2018 INRIA (Institut National de Recherche en
 *   Informatique et Automatique)
 *-----------------------------------------------------------------------------
 *   URL: http://syntax.gforge.inria.fr
 *-----------------------------------------------------------------------------
 *   The source code of SYNTAX is distributed with two different licenses,
 *   depending on the files:
 *   - The recursive content of src/ and incl/ and the non-recursive content
 *     of SYNTAX's root directory are distributed under the CeCILL-C license
 *   - The recursive content of all other repertories is distributed under
 *     the CeCILL license
 *   All code produced by SYNTAX must be considered as being under the
 *   CeCILL-C license. Information about the CeCILL and CeCILL-C licenses
 *   can be found at, e.g., http://www.cecill.fr
 *****************************************************************************/







/* Simulation des entrées-sorties de PL/I */







#ifndef put_edit_h
#define put_edit_h
#include "SXante.h"

SX_GLOBAL_VAR	FILE	*_SXPE_file	SX_INIT_VAL(NULL);
SX_GLOBAL_VAR	size_t	_SXPE_col;
SX_GLOBAL_VAR	char	_SXPE_str [16];
SX_GLOBAL_VAR	char	*_SXPE_string;

extern SXVOID	_SXPE_pos (size_t col);

extern SXVOID	put_edit_fnb (SXINT col, SXINT f, SXINT number);
extern SXVOID	put_edit_nl (SXINT n);
extern SXVOID	put_edit_cut_point (char *string, char *cut_chars, size_t max_cut_point, size_t *xcut, SXBOOLEAN *is_ok);

#define	put_edit_ap(string)	(fputs(string,_SXPE_file),\
				 putc ('\n',_SXPE_file),\
				 _SXPE_col=1)

#define	put_edit(col,string)	(_SXPE_pos(col),\
				 put_edit_ap(string))

#define	put_edit_apnnl(string)	(fputs(_SXPE_string=string,_SXPE_file),\
				 _SXPE_col+=strlen(_SXPE_string))

#define	put_edit_nnl(col,string)	(_SXPE_pos(col),\
					 put_edit_apnnl(string))

#define	put_edit_apnb(number)	(sprintf(_SXPE_str,"%ld",number),\
				 fputs(_SXPE_str,_SXPE_file),\
				 _SXPE_col+=strlen(_SXPE_str))

#define	put_edit_nb(col,number)	(_SXPE_pos(col),\
				 put_edit_apnb(number))

#define	put_edit_ff()		(putc('\f',_SXPE_file),\
				 _SXPE_col=1)

#define	put_edit_initialize(file)	(_SXPE_file=file,\
					 _SXPE_col=1)

#define	put_edit_finalize()	if(_SXPE_file!=NULL)fclose(_SXPE_file),_SXPE_file=NULL; else ((void)0)

#define	put_edit_get_file()	(_SXPE_file)

#define	put_edit_get_col()	(_SXPE_col)

#include "SXpost.h"
#endif /* put_edit_h */

/*
 * Local Variables:
 * mode: C
 * version-control: yes
 * End:
 */
