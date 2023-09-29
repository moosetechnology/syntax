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

/* Ecriture d'un message d'erreur. */

#include "sxversion.h"
#include "sxunix.h"
#include <stdarg.h>

char WHAT_SXERR_MNGR[] = "@(#)SYNTAX - $Id: sxerr_mngr.c 2330 2021-12-08 10:57:46Z garavel $" WHAT_DEBUG;

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

/* sxmake_special_string_and_severity() :

   purpose of the function :
   - create a string that can be displayed from the parameters 'format' and 'ap'
   - do not include the first byte of the first element of the va_list into this string
   - store this first byte of the first element of the va_list into *severity

   the parameters :
   - SXINT *severity : a pointer where to store the severity value
   - char *format, va_list ap : a printf-style list of parameters

   return :
   - a string that can be displayed (i.e. not including the non-displayable character located in the first byte)
*/

static char *sxmake_special_string_and_severity (SXINT *severity, char *format, va_list ap)
{
  char *arg1, *result, *format_1st_part;
  size_t strlen_total;
  SXINT i;
  char dummy[1];

  strlen_total = (size_t) vsnprintf ( dummy, strlen(dummy), format, ap);

  result = sxcalloc (strlen_total, sizeof(char));

  /* get the first arg */
  arg1 = va_arg (ap, char*);

  /* search for the first %s and move after it */
  i=0;
  while (format[i] && (format[i] != '%')) i++;
  i += 2;

  /* duplicate the begin of the format string */
  format_1st_part = sxcalloc ( i+1, sizeof(char));
  strncpy (format_1st_part, format, (size_t) i);

  /* store the severity coded in the first byte of arg1 */
  *severity = *arg1;

  /* build the 1st part of the result 
     the 1st byte of arg1 must not be included */
  sprintf ( result, format_1st_part, arg1+1);

  /* build the end of the result, using va_list */
  vsnprintf ( result+strlen(result), strlen_total-strlen(result), format + i, ap);
  
  sxfree (format_1st_part);

  return result;

}

/*---------------------------------------------------------------------------*/

static SXINT format2arg_nb (char *format)
{
  SXINT arg_nb = 0;

  while ((format = strchr (format, '%'))) {
    if (*(++format) == '%')
      format++;
    else
      arg_nb++;
  }

  return arg_nb;
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

#ifdef	SIMPLE

/*VARARGS2*/

SXVOID sxput_error (struct sxsource_coord source_index, char *format, ...)
{
  va_list ap;
  SXINT	severity;
  char *full_string;
  char *file_name;

  if (sxerrmngr.sxerr_format == SXERR_FORMAT_EMPTY) {
    /* no error or warning message should be printed */
    return;
  }

  severity = 0;

  /* we are calling sxmake_special_string_and_severity(), because the parameters can
     not be printed 'as is' : the first element of the list of parameters contains a
     non-displayable character: it is an integer coding the severity */
  
  va_start (ap, format);
  full_string = sxmake_special_string_and_severity ( &severity, format, ap);
  va_end (ap);

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
      sxtrap("sxerr_mngr","unknown switch case #1");
#endif
      break;
    }	
    fprintf (sxstderr, full_string);
    fputc (SXNEWLINE, sxstderr);
  }

  sxfree (full_string);
}

/*---------------------------------------------------------------------------*/

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
  char* msg_strings [5];
  SXINT i;
  SXINT arg_nb = format2arg_nb (format);

  va_start (ap, format);

  for (i=0; i < arg_nb; i++)
    msg_strings [i] = va_arg(ap, char *);
  for (; i<5; i++)
    msg_strings [i] = NULL;

  va_end (ap);
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
    fprintf (sxstderr, format, msg_strings [0], msg_strings [1], msg_strings [2], msg_strings [3], msg_strings [4], msg_strings [5]);
#endif /* VFPRINTF_IS_BUGGED */

    fputc (SXNEWLINE, sxstderr);
  }
}

/*---------------------------------------------------------------------------*/

/*VARARGS2*/

SXVOID sxhmsg (char *file_name, char *format, char *arg1, ...)
{
  SXINT	severity;
  va_list ap;

  sxerrmngr.nbmess [severity = (SXINT) *arg1]++;

  if (severity != 0) {
    fprintf (sxstderr, SXHEAD_COORD, file_name == NULL ? SXNO_FILE_NAME : file_name);
    va_start (ap, format);
    fprintf (sxstderr, "%s", ++arg1);
    vfprintf (sxstderr, format, ap);
    va_end (ap);
    fputc (SXNEWLINE, sxstderr);
  }
}

/*---------------------------------------------------------------------------*/

/*VARARGS2*/

SXVOID sxtmsg (char *file_name, char *format, char *arg1, ...)
{
  SXINT	severity;
  char *arg1;
  va_list ap;

  sxerrmngr.nbmess [severity = (SXINT) *arg1]++;

  if (severity != 0) {
    fprintf (sxstderr, SXTAIL_COORD, file_name == NULL ? SXNO_FILE_NAME : file_name);
    va_start (ap, format);
    fprintf (sxstderr, "%s", ++arg1);
    vfprintf (sxstderr, format, ap);
    va_end (ap);
    fputc (SXNEWLINE, sxstderr);
  }
}

/*---------------------------------------------------------------------------*/

SXVOID sxerrsort (void)
{
}

/*---------------------------------------------------------------------------*/

SXVOID sxerr_mngr (SXINT what, ...)
{
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

/*VARARGS2*/

SXVOID sxput_error (struct sxsource_coord source_index, char *format, ...)
{
  va_list ap;
  SXINT severity;
  char *full_string;
  char *file_name;

  if (sxerrmngr.sxerr_format == SXERR_FORMAT_EMPTY) {
    /* no error or warning message should be printed */
    return;
  }

  severity = 0;
  
  /* we are calling sxmake_special_string_and_severity(), because the parameters can
     not be printed 'as is' : the first element of the list of parameters contains a
     non-displayable character: it is an integer coding the severity */
  
  va_start (ap, format);
  full_string = sxmake_special_string_and_severity ( &severity, format, ap);
  va_end (ap);

  sxerrmngr.nbmess [severity]++;


  if ((severity == 0 || (find_source (source_index) && output_marker ())) && sxerrmngr.scratch_file != NULL) {

    fputs (full_string, sxerrmngr.scratch_file);
	
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
      sxtrap("sxerr_mngr","unknown switch case #4");
#endif
      break;
    }

    fputs (full_string, sxstderr);

    fputc (SXNEWLINE, sxstderr);
  }
    
  sxfree(full_string);
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
  char* msg_strings [6];
  SXINT i;
  SXINT arg_nb = format2arg_nb (format);
  
  va_start (ap, format);

  for (i = 0; i < arg_nb; i++)
    msg_strings [i] = va_arg(ap, char *);
  for (; i < 6; i++)
    msg_strings [i] = NULL;

  va_end (ap);
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
    //    vfprintf (sxerrmngr.scratch_file, format, ap);
    fprintf (sxerrmngr.scratch_file, format, msg_strings [0], msg_strings [1], msg_strings [2], msg_strings [3], msg_strings [4], msg_strings [5]);
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
    fprintf (sxstderr, format, msg_strings [0], msg_strings [1], msg_strings [2], msg_strings [3], msg_strings [4], msg_strings [5]);
#endif /* VFPRINTF_IS_BUGGED */

    fputc (SXNEWLINE, sxstderr);
  }
}

/*---------------------------------------------------------------------------*/

/*VARARGS2*/

SXVOID sxhmsg (char *file_name, char *format, char *arg1, ...)
{
  SXINT	severity;
  va_list ap;
    
  sxerrmngr.nbmess [severity = *arg1]++;
  arg1++;

  va_start (ap, format);

  if (sxerrmngr.scratch_file != NULL) {
    struct sxsource_coord source_index;

    source_index.file_name = file_name;
    source_index.line = 0;
    source_index.column = 0;
    fprintf (sxerrmngr.scratch_file, "%s", arg1);
    vfprintf (sxerrmngr.scratch_file, format, ap);
    keep_message (source_index, severity);
  }

  if (severity != 0) {
    fprintf (sxstderr, SXHEAD_COORD, file_name == NULL ? SXNO_FILE_NAME : file_name);
    fprintf (sxstderr, "%s", arg1);
    vfprintf (sxstderr, format, ap);
    fputc (SXNEWLINE, sxstderr);
  }
  va_end (ap);
}

/*---------------------------------------------------------------------------*/

/*VARARGS2*/

SXVOID sxtmsg (char *file_name, char *format, char *arg1, ...)
{
  SXINT	severity;
  va_list ap;

  sxerrmngr.nbmess [severity = *arg1]++;
  arg1++;

  va_start (ap, format);

  if (sxerrmngr.scratch_file != NULL) {
    struct sxsource_coord source_index;

    source_index.file_name = file_name;
    source_index.line = ~((SXUINT) 0);
    source_index.column = 0;
    fprintf (sxerrmngr.scratch_file, "%s", arg1);
    vfprintf (sxerrmngr.scratch_file, format, ap);
    keep_message (source_index, severity);
  }

  if (severity != 0) {
    fprintf (sxstderr, SXTAIL_COORD, file_name == NULL ? SXNO_FILE_NAME : file_name);
    fprintf (sxstderr, "%s", arg1);
    vfprintf (sxstderr, format, ap);
    fputc (SXNEWLINE, sxstderr);
  }
  va_end (ap);
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
