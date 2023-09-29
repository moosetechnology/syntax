/* ********************************************************
   *							  *
   *							  *
   * Copyright (c) 1987 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *							  *
   *							  *
   ******************************************************** */




/* ********************************************************
   *							  *
   * Produit de l'équipe ATOLL (Langages et Traducteurs). *
   *							  *
   ******************************************************** */



/* Simulation des entrées-sorties de PL/I */




/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030527 13:31 (phd):	Utilisation de SX{ante,post}.h		*/
/************************************************************************/



#ifndef put_edit_h
#define put_edit_h
#include "SXante.h"

SX_GLOBAL_VAR	FILE	*_SXPE_file	SX_INIT_VAL(NULL);
SX_GLOBAL_VAR	size_t	_SXPE_col;
SX_GLOBAL_VAR	char	_SXPE_str [16];
SX_GLOBAL_VAR	char	*_SXPE_string;

extern VOID	_SXPE_pos (size_t col);

extern VOID	put_edit_fnb (SXINT col, SXINT f, SXINT number);
extern VOID	put_edit_nl (SXINT n);
extern VOID	put_edit_cut_point (char *string, char *cut_chars, size_t max_cut_point, size_t *xcut, BOOLEAN *is_ok);

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
