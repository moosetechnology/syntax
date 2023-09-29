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

#ifndef __SXCOMMON_H__
#define __SXCOMMON_H__

#include <unistd.h> /* for ssize_t */
#include <limits.h> /* for LONG_MAX and INT_MAX */

/* -------------------------------------------------------------------------- */

#ifndef SXVOID
#define SXVOID void
#endif

#ifndef SXSHORT
#define SXSHORT short
#endif

#ifndef SXBOOLEAN
#define SXBOOLEAN       char
#define SXFALSE         (SXBOOLEAN) 0
#define SXTRUE          (SXBOOLEAN) 1   /* (!SXFALSE) */
#endif

/* -------------------------------------------------------------------------- */

#ifdef SX_32BIT_ORIGINAL

#define SXINT int
#define SXUINT unsigned int
#define SXINT_SIZE 32
#define SXINT_MAX INT_MAX
#define SXILD "%i"

#else

#define SXINT long
#define SXUINT unsigned long

#if LONG_MAX == 2147483647
#define SXINT_SIZE 32
#define SXINT_MAX INT_MAX
#else
#define SXINT_SIZE 64
#define SXINT_MAX LONG_MAX
#endif /* LONG_MAX == 2147483647 */

#define SXILD "%ld"

#endif /* ifdef SX_32BIT_ORIGINAL */


/* -------------------------------------------------------------------------- */
/* Pour la manipulation de nombres flottants de taille appropriée */

#define SXFLOAT_IS_FLOAT

#ifdef SXFLOAT_IS_FLOAT
#if defined(SXFLOAT_IS_LONG_DOUBLE) || defined(SXFLOAT_IS_DOUBLE)
#error Incompatible compilation options (SXFLOAT_IS_FLOAT, SXFLOAT_IS_LONG_DOUBLE and SXFLOAT_IS_DOUBLE are mutually exclusive)
#endif /* def SXFLOAT_IS_LONG_DOUBLE || SXFLOAT_IS_DOUBLE */
# define SXFLOAT float
# define SXFLOAT_TYPE_ID 1
# define sxlog10(f) log10f(f)
# define sxexp10(f) exp10f(f)
# define sxfabs(f) fabsf(f)
# define sxatof(f) strtof(f,NULL)
# define SXFLOAT_MAX FLT_MAX
# define SXFDLD "%g"
#endif /* SXFLOAT_IS_FLOAT */

#ifdef SXFLOAT_IS_LONG_DOUBLE
#if defined(SXFLOAT_IS_FLOAT) || defined(SXFLOAT_IS_DOUBLE)
#error Incompatible compilation options (SXFLOAT_IS_FLOAT, SXFLOAT_IS_LONG_DOUBLE and SXFLOAT_IS_DOUBLE are mutually exclusive)
#endif /* def SXFLOAT_IS_LONG_DOUBLE || SXFLOAT_IS_DOUBLE */
# define SXFLOAT long double
# define SXFLOAT_TYPE_ID 3
# define sxlog10(f) log10l(f)
# define sxexp10(f) exp10l(f)
# define sxfabs(f) fabsl(f)
# define sxatof(f) strtold(f,NULL)
# define SXFLOAT_MAX LDBL_MAX
# define SXFDLD "%Lg"
#endif /* SXFLOAT_IS_LONG_DOUBLE */

#ifdef SXFLOAT_IS_DOUBLE
#if defined(SXFLOAT_IS_LONG_DOUBLE) || defined(SXFLOAT_IS_FLOAT)
#error Incompatible compilation options (SXFLOAT_IS_FLOAT, SXFLOAT_IS_LONG_DOUBLE and SXFLOAT_IS_DOUBLE are mutually exclusive)
#endif /* def SXFLOAT_IS_LONG_DOUBLE || SXFLOAT_IS_DOUBLE */
# define SXFLOAT double
# define SXFLOAT_TYPE_ID 2
# define sxlog10(f) log10(f)
# define sxexp10(f) exp10(f)
# define sxfabs(f) fabs(f)
# define sxatof(f) strtod(f,NULL)
# define SXFLOAT_MAX DBL_MAX
# define SXFDLD "%g"
#endif /* SXFLOAT_IS_DOUBLE */

#ifndef SXFLOAT
# define SXFLOAT double
# define SXFLOAT_TYPE_ID 2
# define SXFLOAT_IS_DOUBLE
# define sxlog10(f) log10(f)
# define sxexp10(f) exp10(f)
# define sxfabs(f) fabs(f)
# define sxatof(f) strtod(f,NULL)
# define SXFLOAT_MAX DBL_MAX
# define SXFDLD "%g"
#endif /* ndef SXFLOAT */

#define SXFLOAT_TYPE_CHECK(f) if (f()!=SXFLOAT_TYPE_ID) sxtrap ("sxcommon.h", "Type inconsistency detected for SXFLOAT. Please recompile your tables from scratch (parser proba tables, probabilised dico) to ensure consistency.");

/* -------------------------------------------------------------------------- */

#ifndef SXLOGPROB_FACTOR
# define SXLOGPROB_FACTOR 1000000
#endif

//#define SXLOGPROB_IS_SXFLOAT

# define SXLOGPROB_MIN  (-1073741823)
#ifdef SXLOGPROB_IS_SXFLOAT
# define SXLOGPROB	SXFLOAT
# define SXLOGPROB_display(p) (p)
# define proba2SXLOGPROB(p) (sxlog10 (p))
#else
# define SXLOGPROB	SXINT
# define SXLOGPROB_display(p) (((SXFLOAT)(p))/((SXFLOAT)SXLOGPROB_FACTOR))
# define proba2SXLOGPROB(p) ((SXLOGPROB)(SXLOGPROB_FACTOR * sxlog10 (p)))
#endif
# define SXLPD SXFDLD
#define SXLOGPROB2proba(p) (sxexp10(SXLOGPROB_display(p)))







#ifndef __GNUC__
#define __attribute__(x) /* rien */
#endif

#define sxfiledesc_t       int
/* this type is needed to store file descriptors as returned by create(), open(),
   and used by read() and write()
   We can not use SXINT for file descriptors since it may not be 'int' */

#ifndef SXBA_ELT

#define SXBA_ELT	SXUINT
typedef SXBA_ELT	*SXBA;

#endif

/* ------------------------------- OVERFLOW -------------------------------- */

typedef void (*sxoflw0_t) (SXINT, SXINT);

typedef SXINT (*sxoflw1_t) (SXINT, SXINT);

typedef void * (*sxresize_t) (void*, size_t);

/* ------------------------------------------------------------------------- */

#ifdef SXVERSION
/* the following definitions are done only if SXVERSION is defined
   so that they are not exported to programs using libsx and its headers */
#ifndef FORCE_NO_DEBUG
#define FORCE_NO_DEBUG 0
#endif /* ndef FORCE_NO_DEBUG */

#if FORCE_NO_DEBUG
#undef BUG
#undef EBUG
#undef EBUG_ALLOC
#undef LOG
#undef LLOG
#undef LLLOG
#endif /* FORCE_NO_DEBUG */

#ifndef BUG
#define BUG 0
#endif

#ifndef EBUG
#define EBUG 0
#endif

#ifndef EBUG_ALLOC
#define EBUG_ALLOC 0
#endif

#ifndef LOG
#define LOG 0
#endif

#ifndef LLOG
#define LLOG 0
#endif

#ifndef LLLOG
#define LLLOG 0
#endif

#ifndef MEASURES
#define MEASURES 0
#endif

#ifndef is_dag
#define is_dag 0
#endif

#ifndef is_sdag
#define is_sdag 0
#endif

#ifndef isallow_input_dags
#define isallow_input_dags 0
#endif

#ifndef STATS
#define STATS 0
#endif

#ifndef EBUG_PRDCT
#define EBUG_PRDCT 0
#endif

#ifndef is_large
#define is_large 0
#endif

/* -------------------------------------------------------------------------- */

#if BUG
#define WHAT_BUG "BUG "
#else
#define WHAT_BUG /* rien */
#endif 

#if EBUG
#define WHAT_EBUG "EBUG "
#else
#define WHAT_EBUG /* rien */
#endif 

#if EBUG_ALLOC
#define WHAT_EBUG_ALLOC "EBUG_ALLOC "
#else
#define WHAT_EBUG_ALLOC /* rien */
#endif 

#if LOG
#define WHAT_LOG "LOG "
#else
#define WHAT_LOG /* rien */
#endif 

#if LLOG
#define WHAT_LLOG "LLOG "
#else
#define WHAT_LLOG /* rien */
#endif 

#if LLLOG
#define WHAT_LLLOG "LLLOG "
#else
#define WHAT_LLLOG /* rien */
#endif 

#if MEASURES
#define WHAT_MEASURES "MEASURES "
#else
#define WHAT_MEASURES /* rien */
#endif 

#if is_dag
#define WHAT_is_dag "is_dag "
#else
#define WHAT_is_dag /* rien */
#endif 

#if is_sdag
#define WHAT_is_sdag "is_sdag "
#else
#define WHAT_is_sdag /* rien */
#endif 

#if STATS
#define WHAT_STATS "STATS "
#else
#define WHAT_STATS /* rien */
#endif 

#if EBUG_PRDCT
#define WHAT_EBUG_PRDCT "EBUG_PRDCT "
#else
#define WHAT_EBUG_PRDCT /* rien */
#endif 

#if is_large
#define WHAT_is_large "is_large "
#else
#define WHAT_is_large /* rien */
#endif 

#define WHAT_DEBUG " [ "WHAT_BUG WHAT_EBUG WHAT_EBUG_ALLOC WHAT_LOG WHAT_LLOG WHAT_LLLOG WHAT_MEASURES WHAT_is_dag WHAT_is_sdag WHAT_STATS WHAT_EBUG_PRDCT WHAT_is_large"]"

#else
/* comme ca WHAT_DEBUG est connu ds tous les cas */
#define WHAT_DEBUG /* rien */

#endif /* defined(SXVERSION) */

/* -------------------------------------------------------------------------- */

#define sxinitialise(x) (x) = 0
#define sxinitialise_struct(address,size) (memset ((address),0,(size)))

#define sxuse(x) (void)(x)

/* -------------------------------------------------------------------------- */

#if defined (_WIN32) && defined (VASY_LIB)
/* specifique a VASY */
#define ARCHITECTURE_PC_WIN32
#include "caesar_system.h"
#endif

/* -------------------------------------------------------------------------- */
/* Including definitions of 'fseeko' and 'ftello' under some architectures. */

#ifndef __USE_LARGEFILE
#define __USE_LARGEFILE /* tres provisoire */
#endif
#include <stdio.h>

#endif /* __SXCOMMON_H__ */
