/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1985 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *  Produit de l'equipe Langages et Traducteurs.  (phd) *
   *                                                      *
   ******************************************************** */




/* Acces au source */

/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030512 13:07 (phd):	Adaptation � SGI (64 bits)		*/
/************************************************************************/
/* 20030311 17:09 (phd) :	Muselage du compilateur sur ce coup-l�	*/
/************************************************************************/
/* 27-05-94 14:09 (pb) :	Le source peut etre une chaine C	*/
/************************************************************************/
/* 13-02-87 12:20 (phd):	Seule la version BUG est en verbose	*/
/************************************************************************/
/* 20-01-87 15:43 (phd):	Ajout de previous_char dans sxsrcpush	*/
/************************************************************************/
/* 16-01-87 11:37 (phd):	Modifications en profondeur pour	*/
/*				ajouter "previous_char" et maintenir	*/
/*				des "source_coords" exacts.		*/
/************************************************************************/
/* 22-05-86 14:50 (phd):	Ajout de l'option "SIMPLE", qui permet	*/
/*				de diminuer la taille du source manager	*/
/*				au detriment du temps d'execution	*/
/************************************************************************/
/* 21-05-86 14:50 (phd):	Correction d'erreur dans "sxsrcpush"	*/
/************************************************************************/
/* 16-05-86 17:10 (phd):	Modification du profil de "sxsrc_mngr"	*/
/************************************************************************/
/* 15-05-86 12:10 (phd):	Ajout du "copyright"			*/
/************************************************************************/
/* 15-05-86 11:45 (phd):	Ajout de "localbuf" et "usinglocalbuf"	*/
/************************************************************************/
/* 15-05-86 10:55 (phd):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/

#include "sxunix.h"
#include <stdarg.h>

char WHAT_SXSRC_MNGR[] = "@(#)SYNTAX - $Id: sxsrc_mngr.c 1214 2008-03-13 12:49:51Z rlacroix $" WHAT_DEBUG;


#if	BUG

static SXVOID	srcdebug (char *format, char *arg)
{
    SXUINT	srcdebug_index;

    fprintf (sxstdout, format, arg);
    fprintf (sxstdout, ": {\n");
    fprintf (sxstdout, "\tinfile = 0x%lx\n", (long)sxsrcmngr.infile);
    fprintf (sxstdout, "\tsource_coord = {\"%s\"; %lu; %lu;}\n",
	     sxsrcmngr.source_coord.file_name,
	     sxsrcmngr.source_coord.line,
	     sxsrcmngr.source_coord.column);
    fprintf (sxstdout, "\tprevious_char = '%s' (%03o)\n",
	     CHAR_TO_STRING (sxsrcmngr.previous_char),
	     sxsrcmngr.previous_char);
    fprintf (sxstdout, "\tcurrent_char = '%s' (%03o)\n",
	     CHAR_TO_STRING (sxsrcmngr.current_char),
	     sxsrcmngr.current_char);
    fprintf (sxstdout, "\tbuffer = 0x%lx [%ld]", (long)sxsrcmngr.buffer, sxsrcmngr.buflength);

    if (sxsrcmngr.buflength > 0) {
	fprintf (sxstdout, " [0..%ld] = \"", sxsrcmngr.bufused);

	for (srcdebug_index = 0; srcdebug_index <= (unsigned)sxsrcmngr.bufused; srcdebug_index++)
	    fputs (CHAR_TO_STRING (sxsrcmngr.buffer [srcdebug_index]), sxstdout);

	fputc ('"', sxstdout);
    }

    fprintf (sxstdout, " (%svalid)\n", sxsrcmngr.has_buffer ? "" : "in");

    if (sxsrcmngr.has_buffer) {
	fprintf (sxstdout, "\tbufindex = %ld\n", sxsrcmngr.bufindex);
	fprintf (sxstdout, "\tlabufindex = %ld\n", sxsrcmngr.labufindex);
    }

    fprintf (sxstdout, "\t}\n");
}



char	BEGIN_FMT [] = "%s () begins with", END_FMT [] = "%s () ends with";

#endif


#ifndef	LOCALBUFLENGTH
#	define	LOCALBUFLENGTH	8	/* Smallest buffer allocated */
#endif


#ifndef	SIMPLE

/* Local buffer, to avoid repetitive small allocations */

static struct localbuf {
	   SHORT	buffer [LOCALBUFLENGTH];
       }	localbuf;
static struct localcoords {
	   struct sxsource_coord	bufcoords [LOCALBUFLENGTH];
       }	localcoords;
static BOOLEAN	usinglocalbuf = {FALSE};




#endif


/* Acces au caractere suivant */

SHORT	sxnext_char (void)
{

#if	BUG

    srcdebug (BEGIN_FMT, "sxnext_char");

#endif

    sxsrcmngr.previous_char = sxsrcmngr.current_char;

    if (sxsrcmngr.has_buffer) {
	if (sxsrcmngr.bufindex < sxsrcmngr.bufused) {
	    sxsrcmngr.source_coord = sxsrcmngr.bufcoords [++sxsrcmngr.bufindex];

#if	BUG

	    sxsrcmngr.current_char = sxsrcmngr.buffer [sxsrcmngr.bufindex];
	    goto end;


#else

	    return sxsrcmngr.current_char = sxsrcmngr.buffer [sxsrcmngr.bufindex];

#endif

	}

	sxsrcmngr.has_buffer = FALSE;
    }

    if (sxsrcmngr.current_char != NEWLINE)
	sxsrcmngr.source_coord.column++;
    else {
	sxsrcmngr.source_coord.line++, sxsrcmngr.source_coord.column = 1;


#if	BUG

	if (sxverbosep)
	    putc ('.', sxtty);

#endif

    }


#if	BUG

    sxsrcmngr.current_char = sxgetchar ();
end:
    srcdebug (END_FMT, "sxnext_char");
    return sxsrcmngr.current_char;


#else

    return sxsrcmngr.current_char = sxgetchar ();

#endif

}




/* Acces au caractere en avance suivant */

SHORT	sxlanext_char (void)
{

#if	BUG

    srcdebug (BEGIN_FMT, "sxlanext_char");

    if (sxverbosep)
	fputc (':', sxtty);


#endif

    if (sxsrcmngr.labufindex == sxsrcmngr.bufused) {
      if (++sxsrcmngr.bufused == sxsrcmngr.buflength) {

#ifndef	SIMPLE

	    if (sxsrcmngr.buffer == localbuf.buffer) {
		usinglocalbuf = FALSE;
		*(struct localbuf*) (sxsrcmngr.buffer = (SHORT*) sxalloc (sxsrcmngr.buflength *= 2, sizeof (SHORT))) =
		     localbuf, *(struct localcoords*) (sxsrcmngr.bufcoords = (struct sxsource_coord*) sxalloc (sxsrcmngr.
		     buflength, sizeof (struct sxsource_coord))) = localcoords;
	    }
	    else

#endif

		sxsrcmngr.buffer = (SHORT*) sxrealloc (sxsrcmngr.buffer, sxsrcmngr.buflength *= 2, sizeof (SHORT)),
		     sxsrcmngr.bufcoords = (struct sxsource_coord*) sxrealloc (sxsrcmngr.bufcoords, sxsrcmngr.buflength,
		     sizeof (struct sxsource_coord));
	}
	sxsrcmngr.buffer [sxsrcmngr.bufused] = sxgetchar (), sxsrcmngr.bufcoords [sxsrcmngr.bufused] = sxsrcmngr.
	     bufcoords [sxsrcmngr.labufindex];

	if (sxsrcmngr.buffer [sxsrcmngr.labufindex] != NEWLINE)
	    sxsrcmngr.bufcoords [sxsrcmngr.bufused].column++;
	else
	    sxsrcmngr.bufcoords [sxsrcmngr.bufused].line++, sxsrcmngr.bufcoords [sxsrcmngr.bufused].column = 1;
    }

    sxsrcmngr.labufindex++;

#if	BUG

    srcdebug (END_FMT, "sxlanext_char");

#endif

    return sxsrcmngr.buffer [sxsrcmngr.labufindex];
}




/* Acces au premier caractere en avance */

SHORT	sxlafirst_char (void)
{

#if	BUG

    srcdebug (BEGIN_FMT, "sxlafirst_char");


#endif

    if (!sxsrcmngr.has_buffer) {
      if (sxsrcmngr.buffer == NULL) {

#ifndef	SIMPLE

	    if (!usinglocalbuf) {
		sxsrcmngr.buffer = localbuf.buffer, sxsrcmngr.bufcoords = localcoords.bufcoords;
		sxsrcmngr.buflength = LOCALBUFLENGTH;
		usinglocalbuf = TRUE;
	    }
	    else

#endif

		sxsrcmngr.buffer = (SHORT*) sxalloc (sxsrcmngr.buflength = LOCALBUFLENGTH, sizeof (SHORT)), sxsrcmngr.
		     bufcoords = (struct sxsource_coord*) sxalloc (sxsrcmngr.buflength, sizeof (struct sxsource_coord));
	}
	sxsrcmngr.buffer [0] = sxsrcmngr.current_char, sxsrcmngr.bufcoords [0] = sxsrcmngr.source_coord;
	sxsrcmngr.bufindex = sxsrcmngr.labufindex = sxsrcmngr.bufused = 0;
	sxsrcmngr.has_buffer = TRUE;
    }
    else
	sxsrcmngr.labufindex = sxsrcmngr.bufindex;


#if	BUG

    fprintf (sxstdout, "sxlafirst_char () calls sxlanext_char ()...\n");

#endif

    return sxlanext_char ();
}




/* Recul d'un caractere: le scanner veut inserer un caractere devant */

VOID	sxX (SHORT inserted)
                    
/* On est forcement dans le cas ou le buffer est valide, puisqu'une	*/
/* correction d'erreur impose un look-ahead prealable.  Par ailleurs,	*/
/* cette procedure n'est jamais appelee deux fois de suite, puisqu'une	*/
/* correction d'erreur est censee etre correcte...			*/

{

#if	BUG

    srcdebug ("sxX (%s) decrements \"bufindex\" from ", CHAR_TO_STRING (inserted));

#endif

    sxsrcmngr.bufindex--;
    sxsrcmngr.current_char = inserted;
}




/* Retour en arriere dans la lecture en avance */

VOID	sxlaback (SXINT backward_number)
                           
/* Une erreur a ete detectee et corrigee en look-ahead.  Celui-ci doit	*/
/* reprendre plus haut.  "backward_number" est cense etre valide	*/
/* vis-a-vis de la situation dans le "buffer".				*/

{

#if	BUG

    char	arg [32];

    sprintf (arg, "%ld", backward_number);
    srcdebug ("sxlaback (%s) decrements \"labufindex\" from ", arg);

#endif

    sxsrcmngr.labufindex -= backward_number;
}




/* Push a string ahead of the current character */

VOID	sxsrcpush (SHORT previous_char, char *chars, struct sxsource_coord coord)
/* On traite une action "@Release".  Le "source manager" est dans un */
/* etat quelconque.  En particulier "buffer" peut exister ou non...  */

{
    SXINT	length = strlen (chars);

#if	BUG

    char	header [200];

    sprintf (header, "sxsrcpush ('%s', \"%.110s\", {\"%.50s\"; %lu; %lu;}) %%ss with ", CHAR_TO_STRING (previous_char),
	 chars, coord.file_name, coord.line, coord.column);
    srcdebug (header, "begin");

    if (sxverbosep)
	fputc ('@', sxtty);


#endif

    if (sxsrcmngr.has_buffer) {
	/* Must keep its contents in the right place */

	/* First see if it is big enough */
	{
	    SXINT	minlength;

	    minlength = (sxsrcmngr.bufused - sxsrcmngr.bufindex + 1) + length;

	    if (sxsrcmngr.buflength < minlength) {

#ifndef	SIMPLE

		if (sxsrcmngr.buffer == localbuf.buffer) {
		    usinglocalbuf = FALSE;
		    *(struct localbuf*) (sxsrcmngr.buffer = (SHORT*) sxalloc (sxsrcmngr.buflength = minlength, sizeof (
			 SHORT))) = localbuf, *(struct localcoords*) (sxsrcmngr.bufcoords = (struct sxsource_coord*)
			 sxalloc (sxsrcmngr.buflength, sizeof (struct sxsource_coord))) = localcoords;
		}
		else

#endif

		    sxsrcmngr.buffer = (SHORT*) sxrealloc (sxsrcmngr.buffer, sxsrcmngr.buflength = minlength, sizeof (SHORT)), sxsrcmngr.bufcoords = (struct sxsource_coord*) sxrealloc (sxsrcmngr.bufcoords, sxsrcmngr.buflength, sizeof (struct sxsource_coord));
	    }
	}


/* If there is not room enough, push the contents to the right */

	if (sxsrcmngr.bufindex < length) {
	    SHORT	*buf1, *buf2;
	    struct sxsource_coord	*coords1, *coords2;
	    SXINT	count;

	    count = sxsrcmngr.bufused - sxsrcmngr.bufindex;
	    buf1 = &sxsrcmngr.buffer [sxsrcmngr.bufused], coords1 = &sxsrcmngr.bufcoords [sxsrcmngr.bufused];
	    buf2 = &sxsrcmngr.buffer [sxsrcmngr.bufused = sxsrcmngr.buflength - 1], coords2 = &sxsrcmngr.bufcoords [
		 sxsrcmngr.bufused];
	    sxsrcmngr.bufindex = sxsrcmngr.bufused - count;

	    while (count-- != 0)
		*buf2-- = *buf1--, *coords2-- = *coords1--;
	}
    }
    else {
	/* No need to keep the contents.  Just make sure it is big enough */
	sxsrcmngr.bufindex = sxsrcmngr.bufused = length;

	if (sxsrcmngr.buflength <= length) {
	    sxsrcmngr.buflength = length + 1;

	    if (sxsrcmngr.buffer != NULL) {

#ifndef	SIMPLE

		if (sxsrcmngr.buffer == localbuf.buffer)
		    usinglocalbuf = FALSE;
		else

#endif

		    sxfree (sxsrcmngr.buffer), sxfree (sxsrcmngr.bufcoords);
	    }

#ifndef	SIMPLE

	    if (!usinglocalbuf && sxsrcmngr.buflength <= LOCALBUFLENGTH) {
		sxsrcmngr.buffer = localbuf.buffer, sxsrcmngr.bufcoords = localcoords.bufcoords;
		sxsrcmngr.buflength = LOCALBUFLENGTH;
		usinglocalbuf = TRUE;
	    }
	    else

#endif

		sxsrcmngr.buffer = (SHORT*) sxalloc (sxsrcmngr.buflength, sizeof (SHORT)), sxsrcmngr.bufcoords = (struct
		     sxsource_coord*) sxalloc (sxsrcmngr.buflength, sizeof (struct sxsource_coord));
	}

	sxsrcmngr.has_buffer = TRUE;
    }


/* Now copy the last char returned, preceded by the "chars" */

    sxsrcmngr.buffer [sxsrcmngr.bufindex] = sxsrcmngr.current_char, sxsrcmngr.bufcoords [sxsrcmngr.bufindex] = sxsrcmngr.
	 source_coord;

    {
	SHORT	*buf;
	struct sxsource_coord	*coords;

	buf = &sxsrcmngr.buffer [sxsrcmngr.bufindex -= length], coords = &sxsrcmngr.bufcoords [sxsrcmngr.bufindex];

	while (length-- != 0) {
	    *coords++ = coord;

	    if ((*buf++ = *chars++) != NEWLINE)
		coord.column++;
	    else
		coord.line++, coord.column = 1;
	}
    }


/* And lastly update what is seen from the outside */

    sxsrcmngr.previous_char = previous_char;
    sxsrcmngr.current_char = sxsrcmngr.buffer [sxsrcmngr.bufindex], sxsrcmngr.source_coord = sxsrcmngr.bufcoords [
	 sxsrcmngr.bufindex];

#if	BUG

    srcdebug (header, "end");

#endif

}




/*VARARGS1*/

VOID	sxsrc_mngr (SXINT sxsrc_mngr_what, ... /* FILE *infile, char *name_or_string */ )
{

    va_list ap;
    FILE *infile;
    char *name_or_string;
#if	BUG

    char	header [80];

    sprintf (header, "sxsrc_mngr (%ld) %%ss with ", sxsrc_mngr_what);
    srcdebug (header, "begin");


#endif

    switch (sxsrc_mngr_what) {
    case INIT:
        va_start (ap, sxsrc_mngr_what);
	infile = va_arg (ap, FILE*);
	name_or_string = va_arg (ap, char*);

	sxsrcmngr.infile = infile;
	sxsrcmngr.source_coord.line = sxsrcmngr.source_coord.column = 0;
	sxsrcmngr.previous_char = EOF;
	sxsrcmngr.current_char = NEWLINE;
	sxsrcmngr.buffer = NULL;
	sxsrcmngr.buflength = sxsrcmngr.bufused = sxsrcmngr.bufindex = 0;
	sxsrcmngr.has_buffer = FALSE;
	
	if (infile)
	    sxsrcmngr.source_coord.file_name = name_or_string;
	else
	{
	    sxsrcmngr.source_coord.file_name = SXSRC_STRING_NAME;
	    sxsrcmngr.instring = (unsigned char*) name_or_string;
	    sxsrcmngr.strindex = -1;
	}
	va_end (ap);
	break;

    case FINAL:
	if (sxsrcmngr.buffer != NULL) {

#ifndef	SIMPLE

	    if (sxsrcmngr.buffer == localbuf.buffer)
		usinglocalbuf = FALSE;
	    else

#endif

		sxfree (sxsrcmngr.buffer), sxfree (sxsrcmngr.bufcoords);

	    sxsrcmngr.buffer = NULL;
	}

	break;
    default: /* pour faire taire gcc -Wswitch-default */
#if EBUG
      sxtrap("sxsrc_mngr","unknown switch case #1");
#endif
      break;
    }


#if	BUG

    srcdebug (header, "end");

#endif

}
