/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 2000 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   * Produit de l'équipe ATOLL (Langages et Traducteurs). *
   *                                                      *
   ******************************************************** */




/* Ecriture d'un message d'erreur. */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030505 14:18 (phd):	Modif pour calmer le compilo SGI	*/
/************************************************************************/
/* 20001002 17:42 (phd):	Ajout du fseek(...,0,1) qui va bien	*/
/*				(Merci à VASY <Marc.Herbert@INRIA.Fr>)	*/
/************************************************************************/
/* 16-08-90 12:30 (phd) :	Reprise de la modif du 15-06-90, ni	*/
/*				satisfaisante ni portable ni reentrante */
/*				Prise en compte de SXERR_NO_TABS	*/
/************************************************************************/
/* 18-06-90 11:43 (pb):		La marque d'erreur est positionnee en	*/
/*				utilisant des espaces et non plus des	*/
/*				TABs (Pbs sur apollo)			*/
/************************************************************************/
/* 15-06-90 17:00 (pb):		Utilisation de tmpfile() pour stocker	*/
/*				les messages d'erreur. Le fichier cree	*/
/*				est automatiquement detruit a la fin du	*/
/*				process.				*/
/************************************************************************/
/* 03-08-87 15:53 (phd):	Test explicite du nom du fichier	*/
/*				d'entree vide pour contourner une bogue */
/*				de l'OS Apollo...			*/
/************************************************************************/
/* 07-04-87 15:20 (phd):	Utilisation de "isatty"			*/
/************************************************************************/
/* 11-02-87 17:25 (phd):	La gravite 0 est traitee specialement	*/
/************************************************************************/
/* 10-02-87 17:52 (phd):	On garde la gravite des messages	*/
/************************************************************************/
/* 17-12-86 12:00 (phd):	Les caracteres \ et " ne sont pas	*/
/*				transcodes dans les lignes erronees	*/
/************************************************************************/
/* 17-12-86 11:55 (phd):	La version standard n'est pas SIMPLE	*/
/************************************************************************/
/* 09-12-86 12:45 (phd):	Ajout de sxerrsort a la version simple	*/
/************************************************************************/
/* 08-12-86 16:15 (phd):	Ajout de sxerrsort			*/
/************************************************************************/
/* 05-12-86 14:26 (phd):	Mise au point de la sauvegarde des	*/
/*				messages				*/
/************************************************************************/
/* 03-12-86 16:16 (phd):	Ajout de sxerr_mngr			*/
/************************************************************************/
/* 28-11-86 14:25 (phd):	Ajout de la version ELUXE		*/
/************************************************************************/
/* 04-06-86 14:53 (phd):	Possibilite d'un "file_name" NULL	*/
/************************************************************************/
/* 15-05-86 12:57 (phd):	Ajout du "copyright"			*/
/************************************************************************/
/* 15-05-86 12:56 (phd):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/

#include "sxunix.h"
#include <stdarg.h>

char WHAT_SXERR_MNGR[] = "@(#)SYNTAX - $Id: sxerr_mngr.c 1122 2008-02-28 15:55:03Z rlacroix $" WHAT_DEBUG;


#ifdef	SIMPLE


/*VARARGS2*/

VOID	sxput_error (struct sxsource_coord	source_index, char *format, ...)
{
    SXINT	severity;

    char *arg1;
    va_list ap;


    va_start (ap, format);
    arg1 = va_arg (ap, char*);
    sxerrmngr.nbmess [severity = *arg1++]++;
    va_end (ap);

    if (severity != 0) {
	fprintf (sxstderr, SXERROR_COORD, source_index.file_name, source_index.line, source_index.column);
	
	va_start (ap, format); /* Initialize the va_list */
	vfprintf ( sxstderr, format, ap );
	va_end (ap);           /* Close the va_list */
	fputc (NEWLINE, sxstderr);
    }
}




/*VARARGS2*/

VOID	sxhmsg (char *file_name, char *format, ...)
{
    SXINT	severity;
    char *arg1;
    va_list ap;

    va_start (ap, format);
    arg1 = va_arg (ap, char*);
    sxerrmngr.nbmess [severity = *arg1++]++;
    va_end (ap);

    if (severity != 0) {
	fprintf (sxstderr, SXHEAD_COORD, file_name == NULL ? SXNO_FILE_NAME : file_name);
	va_start (ap, format);
	vfprintf (sxstderr, format, ap);
	va_end (ap);
	fputc (NEWLINE, sxstderr);
    }
}




/*VARARGS2*/

VOID	sxtmsg (char *file_name, char *format, ...)
{
    SXINT	severity;
    char *arg1;
    va_list ap;

    va_start (ap, format);
    arg1 = va_arg (ap, char*);
    sxerrmngr.nbmess [severity = *arg1++]++;
    va_end (ap);

    if (severity != 0) {
	fprintf (sxstderr, SXTAIL_COORD, file_name == NULL ? SXNO_FILE_NAME : file_name);
	va_start (ap, format);
	vfprintf (sxstderr, format, ap);
	va_end (ap);
	fputc (NEWLINE, sxstderr);
    }
}



VOID	sxerrsort (void)
{
}



VOID	sxerr_mngr (SXINT what)
{
}




#else					/* <==> #ifndef SIMPLE */


static BOOLEAN	find_line (FILE *infile, SXUINT line_no)
{
    if (isatty (fileno (infile)) || fseek (infile, 0L, 0 /* SEEK_SET */) == -1) {
	return FALSE;
    }

    while (--line_no > 0) {
	for (;;) {
	    switch (getc (infile)) {
	    case EOF:
		return FALSE;

	    case NEWLINE:
		break;

	    default:
		continue;
	    }

	    break;
	}
    }

    return TRUE;
}



static BOOLEAN	compute_lines (FILE *infile, struct sxsource_coord source_index)
{
#define blanks_len	(sizeof (blanks) - 1) /* MUST be >= TAB_INTERVAL */
    static char		blanks [] = "               ";
    SXINT	char_count /* number of characters read */ ;
    SXINT	line_len /* size of output line */ ;
    SXINT	mark_pos /* whitespace length before the marker */ ;

    if (!find_line (infile, source_index.line)) {
	return FALSE;
    }

    if (sxerrmngr.scratch_file == NULL) {
	return FALSE;
    }

    mark_pos = -1, char_count = 0, line_len = 0;

    for (;;) {
	int	ch;

	if ((SXUINT)(++char_count) == source_index.column) {
	    mark_pos = line_len;
	}

	switch (ch = getc (infile)) {
	case EOF:
	case NEWLINE:
	    fputc (NEWLINE, sxerrmngr.scratch_file);
	    break;

	case '\t':
#ifndef SXERR_NO_TABS
	    fputc ('\t', sxerrmngr.scratch_file);
#else
	    fputs (blanks + blanks_len - (TAB_INTERVAL - (line_len % TAB_INTERVAL)), sxerrmngr.scratch_file);
#endif
	    line_len += -(line_len % TAB_INTERVAL) + TAB_INTERVAL;
	    continue;

	case '\\':
	case '\"':
	    fputc (ch, sxerrmngr.scratch_file);
	    line_len++;
	    continue;

	default:
	    {
		char	*str;

		fputs (str = SXCHAR_TO_STRING (ch), sxerrmngr.scratch_file);
		line_len += strlen (str);
	    }

	    continue;
	}

	break;
    }

    if (mark_pos == -1) {
	/* marker is beyond last character of line */
	mark_pos = line_len + 1;
    }

#ifndef SXERR_NO_TABS
    while (mark_pos > TAB_INTERVAL) {
	mark_pos -= TAB_INTERVAL;
	fputc ('\t', sxerrmngr.scratch_file);
    }
#else
    while (mark_pos > blanks_len) {
	mark_pos -= blanks_len;
	fputs (blanks, sxerrmngr.scratch_file);
    }
#endif

    fputs (blanks + blanks_len - mark_pos, sxerrmngr.scratch_file);
    fputs ("|\b^\n", sxerrmngr.scratch_file);
    return TRUE;
}



static BOOLEAN	output_marker (void)
{
    int 	ch;

    if (sxerrmngr.scratch_file == NULL) {
	return FALSE;
    }

    if (fseek (sxerrmngr.scratch_file, sxerrmngr.mark, 0 /* SEEK_SET */) == -1) {
	return FALSE;
    }

    ch = NEWLINE;

    do {
	fputc (ch, sxstderr);
    } while ((ch = getc (sxerrmngr.scratch_file)) != EOF);

    fseek (sxerrmngr.scratch_file, 0L, 1 /* SEEK_CUR */);

    return TRUE;
}



static BOOLEAN	find_source (struct sxsource_coord source_index)
{
    FILE	*infile;
    BOOLEAN	code;
    long	here;


/* First try opening a new file */

    if (source_index.file_name [0] != NUL && (infile = sxfopen (source_index.file_name, "r")) != NULL) {
	code = compute_lines (infile, source_index);
	fclose (infile);
	return code;
    }


/* It didn't work: try using that of the source manager */

    if (source_index.file_name == sxsrcmngr.source_coord.file_name && (infile = sxsrcmngr.infile) != NULL && (here =
	 ftell (infile)) != -1) {
	code = compute_lines (infile, source_index);
	fseek (infile, here, 0 /* SEEK_SET */);
	return code;
    }


/* No more ideas? */

    return FALSE;
}



static VOID	keep_message (struct sxsource_coord source_index, SXINT severity)
{
    struct sxerr_info	err_info;

    err_info.file_ste = source_index.file_name == NULL ? 0 : sxstrsave (source_index.file_name);
    err_info.line = source_index.line;
    err_info.column = source_index.column;
    err_info.severity = severity;
    err_info.beginning = sxerrmngr.mark;
    fputc (NEWLINE, sxerrmngr.scratch_file);
    fflush (sxerrmngr.scratch_file);
    sxerrmngr.mark = /* prepare for next message */
		     err_info.end = ftell (sxerrmngr.scratch_file);

    if (sxerrmngr.err_kept == sxerrmngr.err_size) {
	sxerrmngr.err_infos = (struct sxerr_info*) (sxerrmngr.err_size == 0 ? sxalloc (sxerrmngr.err_size = 2, sizeof (
	     struct sxerr_info)) : sxrealloc (sxerrmngr.err_infos, sxerrmngr.err_size *= 2, sizeof (struct sxerr_info)));
    }

    sxerrmngr.err_infos [sxerrmngr.err_kept++] = err_info;
}




/*VARARGS2*/

VOID	sxput_error (struct sxsource_coord source_index, char *format, ... )
{
    SXINT	severity;
    char *arg1;
    va_list ap;

    va_start (ap, format);
    arg1 = va_arg (ap, char*);
    sxerrmngr.nbmess [severity = *arg1++]++;
    va_end (ap);

    va_start (ap, format);

    if ((severity == 0 || (find_source (source_index) && output_marker ())) && sxerrmngr.scratch_file != NULL) {
	vfprintf (sxerrmngr.scratch_file, format, ap);
	keep_message (source_index, severity);
    }

    if (severity != 0) {
	fprintf (sxstderr, SXERROR_COORD, source_index.file_name, source_index.line, source_index.column);
        va_end(ap);
        va_start(ap, format);
	vfprintf (sxstderr, format, ap);
	fputc (NEWLINE, sxstderr);
    }
    va_end (ap);
}





/*VARARGS2*/

VOID	sxhmsg (char *file_name, char *format, ...)
{
    SXINT	severity;
    char *arg1;
    va_list ap;
    
    va_start (ap, format);
    arg1 = va_arg (ap, char*);
    sxerrmngr.nbmess [severity = *arg1++]++;
    va_end (ap);

    va_start (ap, format);

    if (sxerrmngr.scratch_file != NULL) {
	struct sxsource_coord	source_index;

	source_index.file_name = file_name;
	source_index.line = 0;
	source_index.column = 0;
	vfprintf (sxerrmngr.scratch_file, format, ap);
	keep_message (source_index, severity);
    }

    if (severity != 0) {
	fprintf (sxstderr, SXHEAD_COORD, file_name == NULL ? SXNO_FILE_NAME : file_name);
	vfprintf (sxstderr, format, ap);
	fputc (NEWLINE, sxstderr);
    }
    va_end (ap);
}




/*VARARGS2*/

VOID	sxtmsg (char *file_name, char *format, ... )
{
    SXINT	severity;
    char *arg1;
    va_list ap;

    va_start (ap, format);
    arg1 = va_arg (ap, char*);
    sxerrmngr.nbmess [severity = *arg1++]++;
    va_end (ap);

    va_start (ap, format);

    if (sxerrmngr.scratch_file != NULL) {
	struct sxsource_coord	source_index;

	source_index.file_name = file_name;
	source_index.line = ~((SXUINT) 0);
	source_index.column = 0;
	vfprintf (sxerrmngr.scratch_file, format, ap);
	keep_message (source_index, severity);
    }

    if (severity != 0) {
	fprintf (sxstderr, SXTAIL_COORD, file_name == NULL ? SXNO_FILE_NAME : file_name);
	vfprintf (sxstderr, format, ap);
	fputc (NEWLINE, sxstderr);
    }
    va_end (ap);
}




/* Function to compare two positions of error messages */

static BOOLEAN	errlessp (SXINT i1, SXINT i2)
{
    struct sxerr_info	*r1, *r2;

    r1 = &(sxerrmngr.err_infos [i1]);
    r2 = &(sxerrmngr.err_infos [i2]);

    if (r1->file_ste != r2->file_ste)
	return r1->file_ste < r2->file_ste;

    if (r1->line != r2->line)
	return r1->line < r2->line;

    if (r1->column != r2->column)
	return r1->column < r2->column;

    return r1->beginning < r2->beginning;
}




/* Procedure which sorts in place all error messages by position */

VOID	sxerrsort (void)
{
    SXINT	*sorted /* temporary array */ ;

    if (sxerrmngr.err_infos == NULL) {
	/* No message kept */
	return;
    }

    sxerrmngr.err_infos = (struct sxerr_info*) /* save space */
					       sxrealloc (sxerrmngr.err_infos, sxerrmngr.err_size = sxerrmngr.err_kept,
	 sizeof (struct sxerr_info));
    sorted = (SXINT*) sxalloc (sxerrmngr.err_kept, sizeof (SXINT));

    {
	SXINT	i;

	for (i = sxerrmngr.err_kept - 1; i >= 0; i--) {
	    sorted [i] = i;
	}
    }

    sort_by_tree (sorted, 0, sxerrmngr.err_kept - 1, errlessp);


/*
 * "sorted" now contains indexes into "err_infos", ie "sorted[0]" is the
 * index of the first error message to output when listing the source
 * program.  We now use it to properly sort "err_infos" in place,
 * minimizing the number of tranfers.
 */

    {
	struct sxerr_info	temp /* Value of "err_infos[index]" */ ;
	SXINT	hole, next;
	SXINT	sxerrsort_index /* looks at each message in turn */ ;

	for (sxerrsort_index = sxerrmngr.err_kept - 1; sxerrsort_index >= 0; sxerrsort_index--) {
	    if (sorted [sxerrsort_index] == sxerrsort_index) {
		/* Already in its final place */
		continue;
	    }

	    temp = sxerrmngr.err_infos [hole = sxerrsort_index] /* Create a hole */ ;

	    do {
		/* Fill the hole with what goes there */
		sxerrmngr.err_infos [hole] = sxerrmngr.err_infos [next = sorted [hole]];
		sorted [hole] = hole /* and keep it in mind */ ;
		/* until the hole is at the right place */
	    } while (sorted [hole = next] != sxerrsort_index);

	    sxerrmngr.err_infos [hole] = temp /* Fill the gap */ ;
	    sorted [hole] = hole /* Don't come here again */ ;
	}
    }

    sxfree (sorted);
}



VOID	sxerr_mngr (SXINT sxerr_mngr_what)
{
    switch (sxerr_mngr_what) {
    case BEGIN:
	if ((sxerrmngr.scratch_file = sxtmpfile ()) != NULL) {
	    sxerrmngr.mark = ftell (sxerrmngr.scratch_file);
	}
	else
	    sxerrmngr.mark = 0;

	break;

    case INIT:
	/* Allow for multiple executions in the same process */
	if (sxerrmngr.scratch_file != NULL) {
	    rewind (sxerrmngr.scratch_file);
	    sxerrmngr.mark = ftell (sxerrmngr.scratch_file);
	}
	else
	    sxerrmngr.mark = 0;

	sxerrmngr.err_kept = 0;
	break;

    case END:
	if (sxerrmngr.scratch_file != NULL) {
	    fclose (sxerrmngr.scratch_file) /* deletes it */ , sxerrmngr.scratch_file = NULL;
	}

	if (sxerrmngr.err_infos != NULL) {
	    sxerrmngr.err_kept = sxerrmngr.err_size = 0;
	    sxfree (sxerrmngr.err_infos), sxerrmngr.err_infos = NULL;
	}

	break;
    default: /* pour faire taire gcc -Switch-default */
#if EBUG
      sxtrap("sxerr_mngr","unknown switch case #1");
#endif
      break;
    }
}

#endif
