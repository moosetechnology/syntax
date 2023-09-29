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

#ifndef __SXDEBUG_H__
#define __SXDEBUG_H__

/* -------------------------------------------------------------------------- */

#ifdef SXVERSION

/* the following definitions are done only if SXVERSION is defined
   so that they are not exported to programs using libsx and its headers */

/* -------------------------------------------------------------------------- */

#ifndef FORCE_NO_DEBUG
#define FORCE_NO_DEBUG 0
#endif

#if FORCE_NO_DEBUG
#undef BUG
#undef EBUG
#undef EBUG_ALLOC
#undef LOG
#undef LLOG
#undef LLLOG
#undef MEASURES
#undef is_dag
#undef is_sdag
#undef STATS
#undef EBUG_PRDCT
#undef is_large
#endif

/* -------------------------------------------------------------------------- */

#ifndef BUG
#define BUG 0
#endif

#if BUG
#define WHAT_BUG "BUG "
#else
#define WHAT_BUG /* rien */
#endif 

/* -------------------------------------------------------------------------- */

#ifndef EBUG
#define EBUG 0
#endif

#if EBUG
#define WHAT_EBUG "EBUG "
#else
#define WHAT_EBUG /* rien */
#endif 

/* -------------------------------------------------------------------------- */

#ifndef EBUG_ALLOC
#define EBUG_ALLOC 0
#endif

#if EBUG_ALLOC
#define WHAT_EBUG_ALLOC "EBUG_ALLOC "
#else
#define WHAT_EBUG_ALLOC /* rien */
#endif 

/* -------------------------------------------------------------------------- */

#ifndef LOG
#define LOG 0
#endif

#if LOG
#define WHAT_LOG "LOG "
#else
#define WHAT_LOG /* rien */
#endif 

/* -------------------------------------------------------------------------- */

#ifndef LLOG
#define LLOG 0
#endif

#if LLOG
#define WHAT_LLOG "LLOG "
#else
#define WHAT_LLOG /* rien */
#endif 

/* -------------------------------------------------------------------------- */

#ifndef LLLOG
#define LLLOG 0
#endif

#if LLLOG
#define WHAT_LLLOG "LLLOG "
#else
#define WHAT_LLLOG /* rien */
#endif 

/* -------------------------------------------------------------------------- */

#ifndef MEASURES
#define MEASURES 0
#endif

#if MEASURES
#define WHAT_MEASURES "MEASURES "
#else
#define WHAT_MEASURES /* rien */
#endif

/* -------------------------------------------------------------------------- */

#ifndef is_dag
#define is_dag 0
#endif

#if is_dag
#define WHAT_is_dag "is_dag "
#else
#define WHAT_is_dag /* rien */
#endif

/* -------------------------------------------------------------------------- */

#ifndef is_sdag
#define is_sdag 0
#endif

#if is_sdag
#define WHAT_is_sdag "is_sdag "
#else
#define WHAT_is_sdag /* rien */
#endif 

/* -------------------------------------------------------------------------- */

#ifndef STATS
#define STATS 0
#endif

#if STATS
#define WHAT_STATS "STATS "
#else
#define WHAT_STATS /* rien */
#endif 

/* -------------------------------------------------------------------------- */

#ifndef EBUG_PRDCT
#define EBUG_PRDCT 0
#endif

#if EBUG_PRDCT
#define WHAT_EBUG_PRDCT "EBUG_PRDCT "
#else
#define WHAT_EBUG_PRDCT /* rien */
#endif 

/* -------------------------------------------------------------------------- */

#ifndef is_large
#define is_large 0
#endif

#if is_large
#define WHAT_is_large "is_large "
#else
#define WHAT_is_large /* rien */
#endif 

/* -------------------------------------------------------------------------- */

#define WHAT_DEBUG " [ "WHAT_BUG WHAT_EBUG WHAT_EBUG_ALLOC WHAT_LOG WHAT_LLOG WHAT_LLLOG WHAT_MEASURES WHAT_is_dag WHAT_is_sdag WHAT_STATS WHAT_EBUG_PRDCT WHAT_is_large"]"

#else

#define WHAT_DEBUG /* rien */
/* comme ca WHAT_DEBUG est connu dans tous les cas */

#endif /* defined(SXVERSION) */

#endif /* __SXCOMMON_H__ */

