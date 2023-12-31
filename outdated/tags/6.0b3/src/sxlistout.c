/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1986 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *  Produit de l'equipe Langages et Traducteurs.  (phd) *
   *                                                      *
   ******************************************************** */




/* Sortie du listing avec messages d'erreur de SYNTAX */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20040209 16:29 (phd):	Modif pour calmer le compilo HP		*/
/************************************************************************/
/* 20030506 10:21 (phd):	Modif pour calmer le compilo SGI	*/
/************************************************************************/
/* 15-06-90 17:37 (pb):		Utilisation de sxerrmngr.scratch_file	*/
/************************************************************************/
/* 03-07-87 12:11 (pb):		Sortie de l'"abstract"			*/
/************************************************************************/
/* 11-02-87 08:54 (pb):		Sortie des "commentaires"		*/
/************************************************************************/
/* 22-12-86 10:20 (phd):	Ajout du "copyright"			*/
/************************************************************************/
/* 22-12-86 10:20 (phd):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/


#include "sxunix.h"

char WHAT_SXLISTOUT[] = "@(#)SYNTAX - $Id: sxlistout.c 1021 2008-02-13 08:39:05Z rlacroix $" WHAT_DEBUG;

static FILE	*listing;
static FILE	*error_file;
/* Where error messages have been kept */
static long	error_pos;
/* Current position in error_file */
static char	*err_string = {NULL};
static SXINT	max_err_size = {128};



static VOID	output_error (SXINT err_no)
{
    SXINT	err_size;
    long	filepos;

    if ((filepos = sxerrmngr.err_infos [err_no].beginning) != error_pos)
	fseek (error_file, filepos - error_pos, 1);

    error_pos = sxerrmngr.err_infos [err_no].end;

    if ((err_size = error_pos - filepos) > max_err_size) {
	err_string = (char*) sxrealloc (err_string, (max_err_size = err_size) + 1, sizeof (char));
    }

    fread (err_string + 1, sizeof (char), (size_t) err_size, error_file);
    fwrite (err_string, sizeof (char), (size_t) err_size + 1, listing);
}



VOID	sxlisting_output (FILE *LISTING)
{
    int 	c;
    SXINT	line_no;
    SXINT		err_no, i, n;
    SXUINT	errline_no;
    SXUINT	this_file_ste;

    sxinitialise(this_file_ste); /* pour faire taire "gcc -Wuninitialized" */ 
    listing = LISTING;
    err_no = 0, errline_no = ~((SXUINT) 0);

    if (sxerrmngr.err_kept > 0) {
	extern VOID	sxerrsort (void);

	sxerrsort ();

	if ((error_file = sxerrmngr.scratch_file) != NULL) {
	    rewind (error_file);
	}

	error_pos = 0;
	this_file_ste = sxstrsave (sxsrcmngr.source_coord.file_name);
	err_string = sxalloc (max_err_size + 1, sizeof (char)), err_string [0] = NEWLINE;

	for (err_no = 0; err_no < sxerrmngr.err_kept; err_no++) {
	    if (sxerrmngr.err_infos [err_no].file_ste == this_file_ste) {
		if (sxerrmngr.err_infos [err_no].line != 0) {
		    errline_no = sxerrmngr.err_infos [err_no].line;
		    break;
		}

		if (error_file != NULL) {
		    output_error (err_no);
		    putc ('\n', listing);
		}
	    }
	}
    }

    rewind (sxsrcmngr.infile);
    line_no = 0;

    while ((c = getc (sxsrcmngr.infile)) != EOF) {
	fprintf (listing, "\n%5ld :\t", (long)++line_no);

	while (c != NEWLINE) {
	    putc (c, listing);

	    if ((c = getc (sxsrcmngr.infile)) == EOF)
		break;
	}

	if (errline_no == (SXUINT)line_no) {
	    static char		STARS [] = "\n\n******";

	    if (sxerrmngr.err_infos [err_no].severity != 0) {
		fputs (STARS, listing);
	    }

	    do {
		if (error_file != NULL)
		    output_error (err_no);

		if (sxerrmngr.err_infos [err_no].severity != 0) {
		    fputs (STARS + 2, listing);
		}

		errline_no = (++err_no < sxerrmngr.err_kept && sxerrmngr.err_infos [err_no].file_ste == this_file_ste ?
		     sxerrmngr.err_infos [err_no].line : ~((SXUINT)0));
	    } while (errline_no == (SXUINT)line_no);

	    putc ('\n', listing);
	}
    }

    putc ('\n', listing);

    while (error_file != NULL && err_no < sxerrmngr.err_kept && sxerrmngr.err_infos [err_no].file_ste == this_file_ste) {
	putc ('\n', listing);
	output_error (err_no++);
    }
    
    {
      SXINT	msgnb [SXSEVERITIES+3];

      for (n = 0, i = 1; i < SXSEVERITIES+3; i++)
	n += msgnb [i] = (i < SXSEVERITIES) ? sxerrmngr.nbmess [i] : 0 ;
	
      if (sxplocals.sxtables /* pas bien */ ->abstract != NULL && n != 0)
	fprintf (listing, sxplocals.sxtables->abstract, msgnb [1], msgnb [2], msgnb [3], msgnb [4], msgnb [5]);
    }

    putc ('\n', listing);

    if (err_string != NULL) {
	sxfree (err_string), err_string = NULL;
    }
}
