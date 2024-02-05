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

/* Acces au source */
#ifdef sxgetchar_is_redefined
#error you really mess up everything, please do not define sxgetchar_is_redefined
#endif /* sxgetchar_is_redefined */

#ifdef sxgetchar
#define sxgetchar_is_redefined
#endif /* sxgetchar */

#include "sxversion.h"
#include "sxunix.h"
#include <stdarg.h>

#ifdef sxgetchar_is_redefined
/* on a donc demande' une compilation speciale de sxsrc_mngr.c avec -Dsxgetchar, car on veut se le
   redefinir differement de la macro definie dans sxunix.h */
#undef sxgetchar
extern short sxgetchar (void);
#define sxgetchar sxgetchar
#endif /* sxgetchar_is_redefined */

char WHAT_SXSRC_MNGR[] = "@(#)SYNTAX - $Id: sxsrc_mngr.c 3633 2023-12-20 18:41:19Z garavel $" WHAT_DEBUG;


#if BUG

static void	srcdebug (char *format, char *arg)
{
    SXUINT	srcdebug_index;

    fprintf (sxstdout, format, arg);
    fprintf (sxstdout, ": {\n");
    fprintf (sxstdout, "\tinfile = 0x%lx\n", (SXINT) sxsrcmngr.infile);
    fprintf (sxstdout, "\tsource_coord = {\"%s\"; %lu; %lu;}\n",
	     sxsrcmngr.source_coord.file_name,
	     sxsrcmngr.source_coord.line,
	     sxsrcmngr.source_coord.column);
    fprintf (sxstdout, "\tprevious_char = '%s' (%03o)\n",
	     SXCHAR_TO_STRING (sxsrcmngr.previous_char),
	     sxsrcmngr.previous_char);
    fprintf (sxstdout, "\tcurrent_char = '%s' (%03o)\n",
	     SXCHAR_TO_STRING (sxsrcmngr.current_char),
	     sxsrcmngr.current_char);
    fprintf (sxstdout, "\tbuffer = 0x%lx [%ld]", (SXINT) sxsrcmngr.buffer, sxsrcmngr.buflength);

    if (sxsrcmngr.buflength > 0) {
	fprintf (sxstdout, " [0..%ld] = \"", sxsrcmngr.bufused);

	for (srcdebug_index = 0; srcdebug_index <= (unsigned)sxsrcmngr.bufused; srcdebug_index++)
	    fputs (SXCHAR_TO_STRING (sxsrcmngr.buffer [srcdebug_index]), sxstdout);

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


#ifndef LOCALBUFLENGTH
#define	LOCALBUFLENGTH	8	/* Smallest buffer allocated */
#endif


#ifndef SIMPLE

/* Local buffer, to avoid repetitive small allocations */

static struct localbuf {
	   short	buffer [LOCALBUFLENGTH];
       }	localbuf;
static struct localcoords {
	   struct sxsource_coord	bufcoords [LOCALBUFLENGTH];
       }	localcoords;
static bool	usinglocalbuf = {false};




#endif


/* Acces au caractere suivant */

short	sxnext_char (void)
{

#if BUG

    srcdebug (BEGIN_FMT, "sxnext_char");

#endif

    sxsrcmngr.previous_char = sxsrcmngr.current_char;

    if (sxsrcmngr.has_buffer) {
	if (sxsrcmngr.bufindex < sxsrcmngr.bufused) {
	    sxsrcmngr.source_coord = sxsrcmngr.bufcoords [++sxsrcmngr.bufindex];

#if BUG

	    sxsrcmngr.current_char = sxsrcmngr.buffer [sxsrcmngr.bufindex];
	    goto end;


#else

	    return sxsrcmngr.current_char = sxsrcmngr.buffer [sxsrcmngr.bufindex];

#endif

	}

	sxsrcmngr.has_buffer = false;
    }

    if (sxsrcmngr.current_char != SXNEWLINE)
	sxsrcmngr.source_coord.column++;
    else {
	sxsrcmngr.source_coord.line++, sxsrcmngr.source_coord.column = 1;


#if BUG

	if (sxverbosep)
	    putc ('.', sxtty);

#endif

    }


#if BUG

    sxsrcmngr.current_char = sxgetchar ();
end:
    srcdebug (END_FMT, "sxnext_char");
    return sxsrcmngr.current_char;


#else

    return sxsrcmngr.current_char = sxgetchar ();

#endif

}




/* Acces au caractere en avance suivant */

short	sxlanext_char (void)
{

#if BUG

    srcdebug (BEGIN_FMT, "sxlanext_char");

    if (sxverbosep)
	fputc (':', sxtty);


#endif

    if (sxsrcmngr.labufindex == sxsrcmngr.bufused) {
      if (++sxsrcmngr.bufused == sxsrcmngr.buflength) {

#ifndef SIMPLE

	    if (sxsrcmngr.buffer == localbuf.buffer) {
		usinglocalbuf = false;
		*(struct localbuf*) (sxsrcmngr.buffer = (short*) sxalloc (sxsrcmngr.buflength *= 2, sizeof (short))) =
		     localbuf, *(struct localcoords*) (sxsrcmngr.bufcoords = (struct sxsource_coord*) sxalloc (sxsrcmngr.
		     buflength, sizeof (struct sxsource_coord))) = localcoords;
	    }
	    else

#endif

		sxsrcmngr.buffer = (short*) sxrealloc (sxsrcmngr.buffer, sxsrcmngr.buflength *= 2, sizeof (short)),
		     sxsrcmngr.bufcoords = (struct sxsource_coord*) sxrealloc (sxsrcmngr.bufcoords, sxsrcmngr.buflength,
		     sizeof (struct sxsource_coord));
	}
	sxsrcmngr.buffer [sxsrcmngr.bufused] = sxgetchar (), sxsrcmngr.bufcoords [sxsrcmngr.bufused] = sxsrcmngr.
	     bufcoords [sxsrcmngr.labufindex];

	if (sxsrcmngr.buffer [sxsrcmngr.labufindex] != SXNEWLINE)
	    sxsrcmngr.bufcoords [sxsrcmngr.bufused].column++;
	else
	    sxsrcmngr.bufcoords [sxsrcmngr.bufused].line++, sxsrcmngr.bufcoords [sxsrcmngr.bufused].column = 1;
    }

    sxsrcmngr.labufindex++;

#if BUG

    srcdebug (END_FMT, "sxlanext_char");

#endif

    return sxsrcmngr.buffer [sxsrcmngr.labufindex];
}




/* Acces au premier caractere en avance */

short	sxlafirst_char (void)
{

#if BUG

    srcdebug (BEGIN_FMT, "sxlafirst_char");


#endif

    if (!sxsrcmngr.has_buffer) {
      if (sxsrcmngr.buffer == NULL) {

#ifndef SIMPLE

	    if (!usinglocalbuf) {
		sxsrcmngr.buffer = localbuf.buffer, sxsrcmngr.bufcoords = localcoords.bufcoords;
		sxsrcmngr.buflength = LOCALBUFLENGTH;
		usinglocalbuf = true;
	    }
	    else

#endif

		sxsrcmngr.buffer = (short*) sxalloc (sxsrcmngr.buflength = LOCALBUFLENGTH, sizeof (short)), sxsrcmngr.
		     bufcoords = (struct sxsource_coord*) sxalloc (sxsrcmngr.buflength, sizeof (struct sxsource_coord));
	}
	sxsrcmngr.buffer [0] = sxsrcmngr.current_char, sxsrcmngr.bufcoords [0] = sxsrcmngr.source_coord;
	sxsrcmngr.bufindex = sxsrcmngr.labufindex = sxsrcmngr.bufused = 0;
	sxsrcmngr.has_buffer = true;
    }
    else
	sxsrcmngr.labufindex = sxsrcmngr.bufindex;


#if BUG

    fprintf (sxstdout, "sxlafirst_char () calls sxlanext_char ()...\n");

#endif

    return sxlanext_char ();
}




/* Recul d'un caractere: le scanner veut inserer un caractere devant */

void	sxX (short inserted)
                    
/* On est forcement dans le cas ou le buffer est valide, puisqu'une	*/
/* correction d'erreur impose un look-ahead prealable.  Par ailleurs,	*/
/* cette procedure n'est jamais appelee deux fois de suite, puisqu'une	*/
/* correction d'erreur est censee etre correcte...			*/

{

#if BUG

    srcdebug ("sxX (%s) decrements \"bufindex\" from ", SXCHAR_TO_STRING (inserted));

#endif

    sxsrcmngr.bufindex--;
    sxsrcmngr.current_char = inserted;
}




/* Retour en arriere dans la lecture en avance */

void	sxlaback (SXINT backward_number)
                           
/* Une erreur a ete detectee et corrigee en look-ahead.  Celui-ci doit	*/
/* reprendre plus haut.  "backward_number" est cense etre valide	*/
/* vis-a-vis de la situation dans le "buffer".				*/

{

#if BUG

    char	arg [32];

    sprintf (arg, "%ld", backward_number);
    srcdebug ("sxlaback (%s) decrements \"labufindex\" from ", arg);

#endif

    sxsrcmngr.labufindex -= backward_number;
}




/* Push a string ahead of the current character */

void	sxsrcpush (short previous_char, char *chars, struct sxsource_coord coord)
/* On traite une action "@Release".  Le "source manager" est dans un */
/* etat quelconque.  En particulier "buffer" peut exister ou non...  */

{
    SXINT	length = strlen (chars);

#if BUG

    char	header [202];

    sprintf (header, "sxsrcpush ('%s', \"%.110s\", {\"%.50s\"; %lu; %lu;}) %%ss with ", SXCHAR_TO_STRING (previous_char),
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

#ifndef SIMPLE

		if (sxsrcmngr.buffer == localbuf.buffer) {
		    usinglocalbuf = false;
		    *(struct localbuf*) (sxsrcmngr.buffer = (short*) sxalloc (sxsrcmngr.buflength = minlength, sizeof (
			 short))) = localbuf, *(struct localcoords*) (sxsrcmngr.bufcoords = (struct sxsource_coord*)
			 sxalloc (sxsrcmngr.buflength, sizeof (struct sxsource_coord))) = localcoords;
		}
		else

#endif

		    sxsrcmngr.buffer = (short*) sxrealloc (sxsrcmngr.buffer, sxsrcmngr.buflength = minlength, sizeof (short)), sxsrcmngr.bufcoords = (struct sxsource_coord*) sxrealloc (sxsrcmngr.bufcoords, sxsrcmngr.buflength, sizeof (struct sxsource_coord));
	    }
	}


/* If there is not room enough, push the contents to the right */

	if (sxsrcmngr.bufindex < length) {
	    short	*buf1, *buf2;
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

#ifndef SIMPLE

		if (sxsrcmngr.buffer == localbuf.buffer)
		    usinglocalbuf = false;
		else

#endif

		    sxfree (sxsrcmngr.buffer), sxfree (sxsrcmngr.bufcoords);
	    }

#ifndef SIMPLE

	    if (!usinglocalbuf && sxsrcmngr.buflength <= LOCALBUFLENGTH) {
		sxsrcmngr.buffer = localbuf.buffer, sxsrcmngr.bufcoords = localcoords.bufcoords;
		sxsrcmngr.buflength = LOCALBUFLENGTH;
		usinglocalbuf = true;
	    }
	    else

#endif

		sxsrcmngr.buffer = (short*) sxalloc (sxsrcmngr.buflength, sizeof (short)), sxsrcmngr.bufcoords = (struct
		     sxsource_coord*) sxalloc (sxsrcmngr.buflength, sizeof (struct sxsource_coord));
	}

	sxsrcmngr.has_buffer = true;
    }


/* Now copy the last char returned, preceded by the "chars" */

    sxsrcmngr.buffer [sxsrcmngr.bufindex] = sxsrcmngr.current_char, sxsrcmngr.bufcoords [sxsrcmngr.bufindex] = sxsrcmngr.
	 source_coord;

    {
	short	*buf;
	struct sxsource_coord	*coords;

	buf = &sxsrcmngr.buffer [sxsrcmngr.bufindex -= length], coords = &sxsrcmngr.bufcoords [sxsrcmngr.bufindex];

	while (length-- != 0) {
	    *coords++ = coord;

	    if ((*buf++ = *chars++) != SXNEWLINE)
		coord.column++;
	    else
		coord.line++, coord.column = 1;
	}
    }


/* And lastly update what is seen from the outside */

    sxsrcmngr.previous_char = previous_char;
    sxsrcmngr.current_char = sxsrcmngr.buffer [sxsrcmngr.bufindex], sxsrcmngr.source_coord = sxsrcmngr.bufcoords [
	 sxsrcmngr.bufindex];

#if BUG

    srcdebug (header, "end");

#endif

}

/*---------------------------------------------------------------------------*/

/* Handle the relocation list */

static sxrelocation_list sxcons_relocation_list (SXUINT current_line, char *relocated_file, SXINT line_increment, sxrelocation_list next)
{
    sxrelocation_list sxresult;

    sxresult = (sxrelocation_list) malloc (sizeof (struct sxrelocation));
    if (sxresult != NULL) {
	sxresult->sxcurrent_line = current_line;
	sxresult->sxrelocated_file = strdup (relocated_file);
	if (sxresult->sxrelocated_file == NULL)
	    sxresult->sxrelocated_file = sxsrcmngr.source_coord.file_name;
	sxresult->sxline_increment = line_increment;
	sxresult->sxnext = next;
    }
    return sxresult;
}

/*---------------------------------------------------------------------------*/

static void sxdelete_relocation_list (sxrelocation_list relocation_list)
{
    sxrelocation_list relocation_cell;

    while (relocation_list != NULL) {
	relocation_cell = relocation_list->sxnext;
	(void) free (relocation_list);
	relocation_list = relocation_cell;
    }
}

/*---------------------------------------------------------------------------*/

#ifdef DEBUG_RELOCATION

static void sxprint_relocation_list (FILE *file, sxrelocation_list relocation_list)
{
    (void) fprintf (file, "Relocation list:\n");
    for (; relocation_list != NULL; relocation_list = relocation_list->sxnext) {
	(void) fprintf (file, "[%lu, \"%s\", %ld]\n", relocation_list->sxcurrent_line, relocation_list->sxrelocated_file, relocation_list->sxline_increment);
    }
}

#endif

/*---------------------------------------------------------------------------*/

static sxrelocation_list sxget_closest_relocation_entry (struct sxsource_coord coord)
{
    sxrelocation_list sxrelocation_entry;

    sxrelocation_entry = sxsrcmngr.sxrelocations;
    while ((sxrelocation_entry != NULL) && (sxrelocation_entry->sxcurrent_line > coord.line)) {
	sxrelocation_entry = sxrelocation_entry->sxnext;
    }
    return sxrelocation_entry;
}

/*---------------------------------------------------------------------------*/

char* sxget_relocated_file (struct sxsource_coord coord)
{
    sxrelocation_list sxrelocation_entry;

    sxrelocation_entry = sxget_closest_relocation_entry (coord);
    if (sxrelocation_entry == NULL)
	return coord.file_name;
    else
	return sxrelocation_entry->sxrelocated_file;
}

/*---------------------------------------------------------------------------*/

SXUINT sxget_relocated_line (struct sxsource_coord coord)
{
    sxrelocation_list sxrelocation_entry;

    sxrelocation_entry = sxget_closest_relocation_entry (coord);
    if (sxrelocation_entry == NULL)
	return coord.line;
    else
	return coord.line + sxrelocation_entry->sxline_increment;
}

/*---------------------------------------------------------------------------*/

/*VARARGS1*/

void	sxsrc_mngr (SXINT sxsrc_mngr_what, ... /* FILE *infile, char *name_or_string */ )
{
    va_list ap;
    FILE *infile;
    char *name_or_string;
    char *file_argument;
    SXUINT line_argument;
    sxrelocation_list relocations;

#if BUG
    char	header [80];

    sprintf (header, "sxsrc_mngr (%ld) %%ss with ", sxsrc_mngr_what);
    srcdebug (header, "begin");
#endif

    switch (sxsrc_mngr_what) {
    case SXINIT:
        va_start (ap, sxsrc_mngr_what);
	infile = va_arg (ap, FILE*);
	name_or_string = va_arg (ap, char*);

	sxsrcmngr.infile = infile;
	sxsrcmngr.source_coord.line = sxsrcmngr.source_coord.column = 0;
	sxsrcmngr.previous_char = EOF;
	sxsrcmngr.current_char = SXNEWLINE;
	sxsrcmngr.buffer = NULL;
	sxsrcmngr.buflength = sxsrcmngr.bufused = sxsrcmngr.bufindex = 0;
	sxsrcmngr.has_buffer = false;
	
	if (infile) {
#if defined _WIN32 && defined __MSVCRT__ && !defined __CYGWIN__

/* Remi Herilier et Hubert Garavel - Wed Jun 17 10:21:33 CEST 2009
 *
 * Patch pour corriger un probleme apparu sous Windows XP et Vista :
 * la bibliotheque standard msvcrt.dll (y compris les versions 7.0.2600.2180
 * sous XP et 7.0.6001.18000 sous Vista - en pratique, toutes les versions de
 * Windows disponibles entre 2006 et 2009) semble comporter un bug.
 * 
 * Ce bug qui semble intervenir a l'intersection de deux traitements : la
 * bufferisation des caracteres lus et la conversion des caracteres CR+LF en 
 * LF (lorsqu'il y a une suite \r\n dans le buffer, alors le \r est supprime
 * ou ignore). Ce bug impacte les fonctions de lecture, entre autres, getc (),
 * getchar () et fread (). Il se manifeste au moment du rattrapage d'erreurs
 * de Syntax lorsqu'un compilateur produit avec Syntax analyse un fichier
 * ne comportant que des \n et pas de \r et comportant une erreur lexicale
 * ou syntaxique. Peut-etre que msvcrt.dll suppose qu'avant le \n, il devait
 * forcement y avoir un \r et introduit automatiquement un \r qui n'existait
 * pas dans le fichier source. En tout le buffer de caracteres est corrompu.
 *
 * Ce probleme ne se produit que si l'on compile Syntax avec gcc+Mingwin.
 * Il ne se produit pas si l'on compile avec gcc+Cygwin, car Cygwin fournit
 * sa propre bibliotheque libc.
 *
 * Ce bug a ete mis en evidence sur les versions Windows de CADP (bugs 0288
 * et 0321 de VASY). Il peut aussi etre reproduit sur l'exemple idcounter
 * de la distribution Syntax V6 en le compilant selon les procedures de VASY. 
 * Il faut ensuite lancer le compilateur "lolo" obtenu sur un programme 
 * incorrect dans lequel les lignes se terminent par \n et pas \r\n". En
 * pistant le resultat renvoye par les appels a getc(), on observe une 
 * difference par rapport au meme compilateur produit sous Linux, Solaris,
 * ou meme Windows/Cygwin.
 *
 * Deux solutions existent a ce probleme : (1) ouvrir le fichier 'infile'
 * par fopen (..., "rb") au lieu de fopen (..., "r") mais ceci necessite de
 * modifier tous les compilateurs eux-memes, ou bien (2) désactiver la 
 * bufferisation du fichier source, ce qui a l'avantage de fonctionner sans
 * exiger de l'utilisateur qu'il fasse attention a remplacer "r" par "rb".
 */
	    setbuf (sxsrcmngr.infile, NULL);
#endif
	    sxsrcmngr.source_coord.file_name = name_or_string;
	} else {
	    sxsrcmngr.source_coord.file_name = SXSRC_STRING_NAME;
	    sxsrcmngr.instring = (unsigned char*) name_or_string;
	    sxsrcmngr.strindex = -1;
	}

        /* initialize the relocation-related variables */
        sxsrcmngr.sxrelocations = NULL;

	va_end (ap);
	break;

    case SXRELATIVE:
    case SXABSOLUTE:
	va_start (ap, sxsrc_mngr_what);
	file_argument = va_arg (ap, char *);
	line_argument = va_arg (ap, SXUINT);
	relocations = sxcons_relocation_list (sxsvar.sxlv.terminal_token.source_index.line, file_argument, line_argument, sxsrcmngr.sxrelocations);
	if (relocations != NULL) {
	    sxsrcmngr.sxrelocations = relocations;
	    switch (sxsrc_mngr_what) {
		case SXRELATIVE:
		    sxsrcmngr.sxrelocations->sxline_increment = line_argument;
		    break;
		case SXABSOLUTE:
		default: /* pour faire taire gcc -Wswitch-default */
		    sxsrcmngr.sxrelocations->sxline_increment = line_argument - (sxsvar.sxlv.terminal_token.source_index.line);
		    break;
	    }
	}
	va_end (ap);
	break;

    case SXFINAL:
	if (sxsrcmngr.buffer != NULL) {
#ifndef SIMPLE
	    if (sxsrcmngr.buffer == localbuf.buffer)
		usinglocalbuf = false;
	    else
#endif
		sxfree (sxsrcmngr.buffer), sxfree (sxsrcmngr.bufcoords);
	    sxsrcmngr.buffer = NULL;
	}
	if (sxsrcmngr.sxrelocations != NULL) {
#ifdef DEBUG_RELOCATION
	    sxprint_relocation_list (stderr, sxsrcmngr.sxrelocations);
#endif
	    sxdelete_relocation_list (sxsrcmngr.sxrelocations);
	}
	break;

    default: /* pour faire taire gcc -Wswitch-default */
#if EBUG
      sxtrap("sxsrc_mngr","unknown switch case #1");
#endif
      break;
    }

#if BUG
    srcdebug (header, "end");
#endif
}

