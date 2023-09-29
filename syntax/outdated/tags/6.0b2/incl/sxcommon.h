
#ifndef __SXCOMMON_H__
#define __SXCOMMON_H__

/* -------------------------------------------------------------------------- */

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

/* -------------------------------------------------------------------------- */

#define sxinitialise(x) (x) = 0
#define sxinitialise_struct(address,size) memset ( (address),0,(size))




#endif /* __SXCOMMON_H__ */
