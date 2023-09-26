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

/* Ecriture d'un message d'erreur. */

#include "sxversion.h"
#include "sxunix.h"

#include <stdarg.h>
#include <errno.h>

char WHAT_SXERR_MNGR[] = "@(#)SYNTAX - $Id: sxerr_mngr.c 3269 2023-05-17 15:18:36Z garavel $" WHAT_DEBUG;

/*---------------------------------------------------------------------------*/

static char *sxdelete_suffix (char *NAME)
{
  char *dot;

  dot = strrchr (NAME, '.');
  if (dot != NULL)
    *dot = '\0';
  return dot;
}

/*---------------------------------------------------------------------------*/

/* return the maximal severity of the emitted messages */

int sxerr_max_severity ()
{
  int severity;

  for (severity = SXSEVERITIES - 1;
       severity > 0 && sxerrmngr.nbmess [severity] == 0;
       severity--)
    ;
  return (severity);
}

/*---------------------------------------------------------------------------*/

#ifdef SIMPLE

/* SIMPLE version of sxerror() */

SXVOID sxerror (struct sxsource_coord source_index,
		 SXINT severity,
		 char *format,
#ifdef VFPRINTF_IS_BUGGED
		 char* arg1, char* arg2, char* arg3, char* arg4, char* arg5
#else /* VFPRINTF_IS_BUGGED */
		 ...
#endif /* VFPRINTF_IS_BUGGED */
		 )
{
  char *file_name;
#ifndef VFPRINTF_IS_BUGGED
  va_list ap;
  
  va_start (ap, format);
#endif /* !VFPRINTF_IS_BUGGED */

  if (sxerrmngr.sxerr_format == SXERR_FORMAT_EMPTY) {
    /* no error or warning message should be printed */
    return;
  }

  sxerrmngr.nbmess [severity]++;

  if (severity != 0) {
    switch (sxerrmngr.sxerr_format) {
    case SXERR_FORMAT_DEFAULT:
      /* traditional error format */
      fprintf (sxstderr, SXERROR_COORD, sxfile (source_index), sxline (source_index), sxcolumn (source_index));
      break;
    case SXERR_FORMAT_LIGHTWEIGHT:
      /* lightweight error format */
      file_name = sxfile (source_index);
      if (file_name[0] == '\0')
	fprintf (sxstderr, "[%lu] ", sxline (source_index));
      else {
	char *dot = sxdelete_suffix (file_name);
	fprintf (sxstderr, "[%s:%lu] ", file_name, sxline (source_index));
	if (dot != NULL) *dot = '.';
      }
      break;
    case SXERR_FORMAT_BANNER:
      /* user-specified banner format, e.g., "in the command-line options: " */
      fprintf (sxstderr, "%s", sxerrmngr.sxerr_banner);
      break;
    case SXERR_FORMAT_CUSTOM:
      /* user-specified custom format, e.g., "%s:%u:%u: " */
      fprintf (sxstderr, sxerrmngr.sxerr_banner, sxfile (source_index), sxline (source_index), sxcolumn (source_index));
      break;
    case SXERR_FORMAT_EMPTY:
      /* no warning or error message is printed */
      /* NOTREACHED */
      break;
    default: /* pour faire taire gcc -Wswitch-default */
#if EBUG
      sxtrap("sxerr_mngr","unknown switch case #2");
#endif
      break;
    }	
#ifdef VFPRINTF_IS_BUGGED
    fprintf (sxstderr, format, arg1, arg2, arg3, arg4, arg5);
#else /* VFPRINTF_IS_BUGGED */
    vfprintf (sxstderr, format, ap);
#endif /* VFPRINTF_IS_BUGGED */

    fputc (SXNEWLINE, sxstderr);
  }

#ifndef VFPRINTF_IS_BUGGED
  va_end (ap);
#endif /* !VFPRINTF_IS_BUGGED */
}

/*---------------------------------------------------------------------------*/

/* SIMPLE version of sxhmsg() */

/*VARARGS2*/

SXVOID sxhmsg (char *file_name, char *format, ...)
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
    fputc (SXNEWLINE, sxstderr);
  }
}

/*---------------------------------------------------------------------------*/

/* SIMPLE version of sxtmsg() */

/*VARARGS2*/

SXVOID sxtmsg (char *file_name, char *format, ...)
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
    fputc (SXNEWLINE, sxstderr);
  }
}

/*---------------------------------------------------------------------------*/

/* SIMPLE version of sxerrsort() */

SXVOID sxerrsort (void)
{
}

/*---------------------------------------------------------------------------*/

/* SIMPLE version of sxerr_mngr() */

SXVOID sxerr_mngr (SXINT what, ...)
{
  sxuse (what);
}

/*---------------------------------------------------------------------------*/

#else					/* <==> #ifndef SIMPLE */

static SXBOOLEAN find_line (FILE *infile, SXUINT line_num)
{
  if (isatty (fileno (infile)) || fseeko (infile, (off_t) 0, 0 /* SEEK_SET */) == -1) {
    return SXFALSE;
  }

  while (--line_num > 0) {
    for (;;) {
      switch (getc (infile)) {
      case EOF:
	return SXFALSE;

      case SXNEWLINE:
	break;

      default:
	continue;
      }

      break;
    }
  }

  return SXTRUE;
}

/*---------------------------------------------------------------------------*/

static SXBOOLEAN compute_lines (FILE *infile, SXUINT line_num, SXUINT column_num)
{
#define blanks_len	(sizeof (blanks) - 1) /* MUST be >= SXTAB_INTERVAL */
  static char		blanks [] = "               ";
  SXINT	char_count /* number of characters read */ ;
  SXINT	line_len /* size of output line */ ;
  SXINT	mark_pos /* whitespace length before the marker */ ;

  if (!find_line (infile, line_num)) {
    return SXFALSE;
  }

  if (sxerrmngr.scratch_file == NULL) {
    return SXFALSE;
  }

  if (sxerrmngr.sxerr_format == SXERR_FORMAT_EMPTY) {
    /* no error or warning message should be printed */
    return SXTRUE;
  }

  mark_pos = -1, char_count = 0, line_len = 0;

  for (;;) {
    int	ch;

    if ((SXUINT)(++char_count) == column_num) {
      mark_pos = line_len;
    }

    switch (ch = getc (infile)) {
    case EOF:
    case SXNEWLINE:
      fputc (SXNEWLINE, sxerrmngr.scratch_file);
      break;

    case '\t':
#ifndef SXERR_NO_TABS
      fputc ('\t', sxerrmngr.scratch_file);
#else
      fputs (blanks + blanks_len - (SXTAB_INTERVAL - (line_len % SXTAB_INTERVAL)), sxerrmngr.scratch_file);
#endif
      line_len += -(line_len % SXTAB_INTERVAL) + SXTAB_INTERVAL;
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
  while (mark_pos > SXTAB_INTERVAL) {
    mark_pos -= SXTAB_INTERVAL;
    fputc ('\t', sxerrmngr.scratch_file);
  }
#else
  while (mark_pos > blanks_len) {
    mark_pos -= blanks_len;
    fputs (blanks, sxerrmngr.scratch_file);
  }
#endif

  fputs (blanks + blanks_len - mark_pos, sxerrmngr.scratch_file);
  switch (sxerrmngr.sxerr_format) {
  case SXERR_FORMAT_DEFAULT:
    /* traditional error format */
    fputs ("|\b^\n", sxerrmngr.scratch_file);      
    break;
  case SXERR_FORMAT_LIGHTWEIGHT:
    /* lightweight error format */
    fputs ("^\n", sxerrmngr.scratch_file);      
    break;
  case SXERR_FORMAT_BANNER:
    /* user-specified banner format */
    fputs ("^\n", sxerrmngr.scratch_file);
    break;
  case SXERR_FORMAT_CUSTOM:
    /* user-specified custom format */
    fputs ("^\n", sxerrmngr.scratch_file);
    break;
  case SXERR_FORMAT_EMPTY:
    /* no warning or error message is printed */
    /* NOTREACHED */
    break;
  default: /* pour faire taire gcc -Wswitch-default */
#if EBUG
    sxtrap("sxerr_mngr","unknown switch case #3");
#endif
    break;
  }

  return SXTRUE;
}

/*---------------------------------------------------------------------------*/

static SXBOOLEAN output_marker (void)
{
  int 	ch;

  if (sxerrmngr.scratch_file == NULL) {
    return SXFALSE;
  }

  if (fseeko (sxerrmngr.scratch_file, sxerrmngr.mark, 0 /* SEEK_SET */) == -1) {
    return SXFALSE;
  }

  ch = SXNEWLINE;

  do {
    fputc (ch, sxstderr);
  } while ((ch = getc (sxerrmngr.scratch_file)) != EOF);

  fseeko (sxerrmngr.scratch_file, (off_t) 0, 1 /* SEEK_CUR */);

  return SXTRUE;
}

/*---------------------------------------------------------------------------*/

static SXBOOLEAN find_source (struct sxsource_coord source_index)
{
  char *file_name;
  FILE *infile;
  SXBOOLEAN code;
  off_t here;
  
  SYNTAX_is_in_critical_zone = SXTRUE;

  /* First, try opening the relocated file (if relocation is in effect) */

  if (sxsrcmngr.sxrelocations != NULL) {
    file_name = sxfile (source_index);
    if (file_name [0] != SXNUL && (infile = sxfopen (file_name, "r")) != NULL) {
      code = compute_lines (infile, sxline (source_index), sxcolumn (source_index));
      fclose (infile);
      SYNTAX_is_in_critical_zone = SXFALSE;
      return code;
    }
  }

  /* Second, try opening the file actually being parsed */

  file_name = source_index.file_name;
  if (file_name [0] != SXNUL && (infile = sxfopen (file_name, "r")) != NULL) {
    code = compute_lines (infile, source_index.line, source_index.column);
    fclose (infile);
    SYNTAX_is_in_critical_zone = SXFALSE;
    return code;
  }

  /* Third: it didn't work, try using that of the source manager */

  if (file_name == sxsrcmngr.source_coord.file_name && (infile = sxsrcmngr.infile) != NULL && (here = ftello (infile)) != -1) {
    code = compute_lines (infile, source_index.line, source_index.column);
    fseeko (infile, here, 0 /* SEEK_SET */);
    SYNTAX_is_in_critical_zone = SXFALSE;
    return code;
  }

  /* No more ideas? */

  SYNTAX_is_in_critical_zone = SXFALSE;
  return SXFALSE;
}

/*---------------------------------------------------------------------------*/

static SXVOID keep_message (struct sxsource_coord source_index, SXINT severity)
{
  struct sxerr_info	err_info;

  err_info.file_ste = source_index.file_name == NULL ? 0 : sxstrsave (source_index.file_name);
  err_info.line = source_index.line;
  err_info.column = source_index.column;
  err_info.severity = severity;
  err_info.beginning = sxerrmngr.mark;
  fputc (SXNEWLINE, sxerrmngr.scratch_file);
  fflush (sxerrmngr.scratch_file);
  sxerrmngr.mark = /* prepare for next message */
    err_info.end = ftello (sxerrmngr.scratch_file);

  if (sxerrmngr.err_kept == sxerrmngr.err_size) {
    sxprepare_for_possible_reset (sxerrmngr.err_infos);
    sxprepare_for_possible_reset (sxerrmngr.err_size);
    sxerrmngr.err_infos = (struct sxerr_info*) (sxerrmngr.err_size == 0 ? 
						sxalloc (sxerrmngr.err_size = 2, sizeof (struct sxerr_info)) : 
						sxrealloc (sxerrmngr.err_infos, sxerrmngr.err_size *= 2, sizeof (struct sxerr_info)));
  }

  sxerrmngr.err_infos [sxerrmngr.err_kept++] = err_info;
}

/*---------------------------------------------------------------------------*/

SXVOID sxerror (struct sxsource_coord source_index,
		 SXINT severity,
		 char *format,
#ifdef VFPRINTF_IS_BUGGED
		 char* arg1, char* arg2, char* arg3, char* arg4, char* arg5, char* arg6
#else /* VFPRINTF_IS_BUGGED */
		 ...
#endif /* VFPRINTF_IS_BUGGED */
		 )
{
  char *file_name;
#ifndef VFPRINTF_IS_BUGGED
  va_list ap;
  
  va_start (ap, format);
#endif /* !VFPRINTF_IS_BUGGED */

  if (sxerrmngr.sxerr_format == SXERR_FORMAT_EMPTY) {
    /* no error or warning message should be printed */
    return;
  }

  sxerrmngr.nbmess [severity]++;

  if ((severity == 0 || (find_source (source_index) && output_marker ())) && sxerrmngr.scratch_file != NULL) {

#ifdef VFPRINTF_IS_BUGGED
    fprintf (sxerrmngr.scratch_file, format, arg1, arg2, arg3, arg4, arg5, arg6);
#else /* VFPRINTF_IS_BUGGED */
    vfprintf (sxerrmngr.scratch_file, format, ap);
#endif /* VFPRINTF_IS_BUGGED */
	
    keep_message (source_index, severity);
  }

  if (severity != 0) {
    switch (sxerrmngr.sxerr_format) {
    case SXERR_FORMAT_DEFAULT:
      /* traditional error format */
      fprintf (sxstderr, SXERROR_COORD, sxfile (source_index), sxline (source_index), sxcolumn (source_index));
      break;
    case SXERR_FORMAT_LIGHTWEIGHT:
      /* lightweight error format */
      file_name = sxfile (source_index);
      if (file_name[0] == '\0')
	fprintf (sxstderr, "[%lu] ", sxline (source_index));
      else {
	char *dot = sxdelete_suffix (file_name);
	fprintf (sxstderr, "[%s:%lu] ", file_name, sxline (source_index));
	if (dot != NULL) *dot = '.';
      }
      break;
    case SXERR_FORMAT_BANNER:
      /* user-specified banner format, e.g., "in the command-line options: " */
      fprintf (sxstderr, "%s", sxerrmngr.sxerr_banner);
      break;
    case SXERR_FORMAT_CUSTOM:
      /* user-specified custom format, e.g., "%s:%u:%u: " */
      fprintf (sxstderr, sxerrmngr.sxerr_banner, sxfile (source_index), sxline (source_index), sxcolumn (source_index));
      break;
    case SXERR_FORMAT_EMPTY:
      /* no warning or error message is printed */
      /* NOTREACHED */
      break;
    default: /* pour faire taire gcc -Wswitch-default */
#if EBUG
      sxtrap("sxerr_mngr","unknown switch case #5");
#endif
      break;
    }

#ifdef VFPRINTF_IS_BUGGED
    fprintf (sxstderr, format, arg1, arg2, arg3, arg4, arg5, arg6);
#else /* VFPRINTF_IS_BUGGED */
    vfprintf (sxstderr, format, ap);
#endif /* VFPRINTF_IS_BUGGED */

    fputc (SXNEWLINE, sxstderr);
  }
    
#ifndef VFPRINTF_IS_BUGGED
  va_end (ap);
#endif /* !VFPRINTF_IS_BUGGED */
}

/*---------------------------------------------------------------------------*/

/*VARARGS2*/

SXVOID sxhmsg (char *file_name, char *format, ...)
{
  SXINT	severity;
  char *arg1;
  va_list ap;
    
  va_start (ap, format);
  arg1 = va_arg (ap, char*);
  sxerrmngr.nbmess [severity = *arg1++]++;
  va_end (ap);

  if (sxerrmngr.scratch_file != NULL) {
    struct sxsource_coord source_index;

    source_index.file_name = file_name;
    source_index.line = 0;
    source_index.column = 0;
    va_start (ap, format);
    vfprintf (sxerrmngr.scratch_file, format, ap);
    va_end (ap);
    keep_message (source_index, severity);
  }

  if (severity != 0) {
    fprintf (sxstderr, SXHEAD_COORD, file_name == NULL ? SXNO_FILE_NAME : file_name);
    va_start (ap, format);
    vfprintf (sxstderr, format, ap);
    va_end (ap);
    fputc (SXNEWLINE, sxstderr);
  }
}

/*---------------------------------------------------------------------------*/

/*VARARGS2*/

SXVOID sxtmsg (char *file_name, char *format, ...)
{
  SXINT	severity;
  char *arg1;
  va_list ap;

  va_start (ap, format);
  arg1 = va_arg (ap, char*);
  sxerrmngr.nbmess [severity = *arg1++]++;
  va_end (ap);


  if (sxerrmngr.scratch_file != NULL) {
    struct sxsource_coord source_index;

    source_index.file_name = file_name;
    source_index.line = ~((SXUINT) 0);
    source_index.column = 0;
    va_start (ap, format);
    vfprintf (sxerrmngr.scratch_file, format, ap);
    va_end (ap);
    keep_message (source_index, severity);
  }

  if (severity != 0) {
    fprintf (sxstderr, SXTAIL_COORD, file_name == NULL ? SXNO_FILE_NAME : file_name);
    va_start (ap, format);
    vfprintf (sxstderr, format, ap);
    va_end (ap);
    fputc (SXNEWLINE, sxstderr);
  }
}

/*---------------------------------------------------------------------------*/

/* Function to compare two positions of error messages */

static SXBOOLEAN errlessp (SXINT i1, SXINT i2)
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

/*---------------------------------------------------------------------------*/

/* Procedure which sorts in place all error messages by position */

SXVOID sxerrsort (void)
{
  SXINT	*sorted /* temporary array */ ;

  if (sxerrmngr.err_infos == NULL) {
    /* No message kept */
    return;
  }

  sxprepare_for_possible_reset (sxerrmngr.err_infos);
  sxprepare_for_possible_reset (sxerrmngr.err_size);
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

/*---------------------------------------------------------------------------*/

SXVOID sxerr_mngr (SXINT sxerr_mngr_what, ...)
{
  SXUINT sxerr_format, severity;
  va_list ap;

  switch (sxerr_mngr_what) {
  case SXBEGIN:
    if ((sxerrmngr.scratch_file = sxtmpfile ()) != NULL) {
      sxerrmngr.mark = ftello (sxerrmngr.scratch_file);
    }
    else
      sxerrmngr.mark = 0;
    sxerrmngr.sxerr_format = SXERR_FORMAT_DEFAULT;
    break;

  case SXINIT:
    /* Allow for multiple executions in the same process */
    if (sxerrmngr.scratch_file != NULL) {
      rewind (sxerrmngr.scratch_file);
      sxerrmngr.mark = ftello (sxerrmngr.scratch_file);
    }
    else
      sxerrmngr.mark = 0;
    sxerrmngr.err_kept = 0;
    sxerrmngr.sxerr_format = SXERR_FORMAT_DEFAULT;
    break;

  case SXCLEAR:
    for (severity = 0; severity < SXSEVERITIES; severity++)
      sxerrmngr.nbmess [severity] = 0;
    break;

  case SXFORMAT:
    va_start (ap, sxerr_mngr_what);
    sxerr_format = va_arg (ap, SXUINT);
    sxerrmngr.sxerr_format = sxerr_format;
    switch (sxerr_format) {
    case SXERR_FORMAT_DEFAULT:
      /* traditional error format */
      /* no further argument expected */
      break;
    case SXERR_FORMAT_LIGHTWEIGHT:
      /* lightweight error format */
      /* no further argument expected */
      break;
    case SXERR_FORMAT_BANNER:
    case SXERR_FORMAT_CUSTOM:
      /* user-specified banner or custom formats */
      /* one argument expected: the banner string */
      sxerrmngr.sxerr_banner = va_arg (ap, char *);
      break;
    case SXERR_FORMAT_EMPTY:
      /* no warning or error message is printed */
      break;
    default: /* pour faire taire gcc -Wswitch-default */
#if EBUG
    sxtrap("sxerr_mngr","unknown switch case #7");
#endif
    break;
    }
    va_end (ap);
    break;

  case SXEND:
    if (sxerrmngr.scratch_file != NULL) {
      fclose (sxerrmngr.scratch_file) /* deletes it */ , sxerrmngr.scratch_file = NULL;
    }
    if (sxerrmngr.err_infos != NULL) {
      sxerrmngr.err_kept = sxerrmngr.err_size = 0;
      sxfree (sxerrmngr.err_infos), sxerrmngr.err_infos = NULL;
    }
    break;

  default: /* pour faire taire gcc -Wswitch-default */
#if EBUG
    sxtrap("sxerr_mngr","unknown switch case #6");
#endif
    break;
  }
}

#endif

/*---------------------------------------------------------------------------*/

SXVOID	sxperror (char *string)
{
    fprintf (sxstderr, "%s: %s.\n", string, strerror (errno));
}

/*---------------------------------------------------------------------------*/

SXVOID	sxtrap (caller, message)
    char	*caller, *message;
{
  fprintf (sxstderr, "\nInternal system ERROR in \"%s\" during \"%s\".%c\n", caller, message, sxbell);

    if (sxtrap_recovery && mem_signature_mode)
      sxtrap_recovery ();
    else
      sxexit (SXSEVERITIES);
}

/*---------------------------------------------------------------------------*/

