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

#ifndef __SXCOMMON_H__
#define __SXCOMMON_H__

#include <stdbool.h> /* for bool, false, true */
#include <stdint.h> /* for INTPTR_MAX on macOS, etc. */
#include <limits.h> /* for LONG_MAX, INT_MAX, etc. */
#include <unistd.h> /* for ssize_t */
#include <time.h> /* for time_t */

/* -------------------------------------------------------------------------- */

/* Including definitions of 'fseeko' and 'ftello' under some architectures. */

#ifndef __USE_LARGEFILE
#define __USE_LARGEFILE /* tres provisoire */
#endif

#include <stdio.h>

/* -------------------------------------------------------------------------- */

/* Including various definitions that are useful when debugging SYNTAX */
#include "sxdebug.h"

/* -------------------------------------------------------------------------- */

#if defined (_WIN32) && defined (VASY_LIB)
/* case of Windows (32- or 64-bits), specific to the VASY/CONVECS team */

#include "caesar_system.h"

#endif

/* -------------------------------------------------------------------------- */

/*
 * note : in the early 32-bit-only versions of SYNTAX, SXINT was defined as
 * int; SXUINT was defined as unsigned int; SXINT_SIZE was equal to 32; and
 * SXINT_MAX was equal to INT_MAX
 */

#if INTPTR_MAX == INT32_MAX     /* 32-bit machine */

#define SXINT long
#define SXUINT unsigned long
#define SXINT_SIZE 32
#define SXINT_MAX INT_MAX

#elif INTPTR_MAX == INT64_MAX   /* 64-bit machine */

#if defined (_WIN64) && defined (VASY_LIB)

/* case of Windows (32- or 64-bits), specific to the VASY/CONVECS team */
/* note: __GNUC__ and__MINGW64__ are likely to be #defined at this point */
/* special case: compiling for Win64 using MinGW: the 'long' type is 32-bit */

#define SXINT long long /* 64 bits */
#define SXUINT unsigned long long /* 64 bits */
#define SXINT_SIZE 64
#define SXINT_MAX LONG_LONG_MAX

#else

/* normal case */
#define SXINT long
#define SXUINT unsigned long
#define SXINT_SIZE 64
#define SXINT_MAX LONG_MAX

#endif

#else
...
#endif

/* -------------------------------------------------------------------------- */

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


#define sxinitialise(x) (x) = 0
#define sxinitialise_struct(address,size) (memset ((address),0,(size)))

#define sxuse(x) (void)(x)

/* -------------------------------------------------------------------------- */

#ifndef __GNUC__
#define __attribute__(x) /* rien */
#endif

/* -------------------------------------------------------------------------- */

#endif /* __SXCOMMON_H__ */

