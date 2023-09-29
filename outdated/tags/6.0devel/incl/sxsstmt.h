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
#include "sxunix.h"

#if (defined(SXS_MAX) && SXS_MAX >= SHRT_MAX)
/* STMT tient sur un SXUINT */
#define KEEP(stmt)  ((stmt) & 0x08000000)
#define SCAN(stmt)  ((stmt) & 0x04000000)
#define CODOP(stmt) ((stmt) >> 28)
#define NEXT(stmt)  ((stmt) & 0x3FFFFFF)
#define KSC(stmt)   ((stmt) & 0xFC000000)
#define NC(stmt)    ((stmt) & 0xF3FFFFFF)

#define STMT(K,S,C,N) ((K)<<27|(S)<<26|(C)<<28|(N))
#else
/* STMT tient sur un unsigned short */
#define KEEP(stmt)  ((stmt) & 0x0800)
#define SCAN(stmt)  ((stmt) & 0x0400)
#define CODOP(stmt) ((stmt) >> 12)
#define NEXT(stmt)  ((stmt) & 0x3FF)
#define KSC(stmt)   ((stmt) & 0xFC00)
#define NC(stmt)    ((stmt) & 0xF3FF)

#define STMT(K,S,C,N) ((K)<<11|(S)<<10|(C)<<12|(N))
#endif

/* Values of action_or_prdct_code.kind */
#define IsAction	'\000'
#define IsPredicate	'\001'
#define IsNonDeter	'\002'

/* scanner statement operation code */
/* Le 15/05/2003 ajout du suffixe U (merci Mr Solaris...) */
#define Error		0U
#define SameState	1U
#define State		2U
#define HashReduce	3U
#define Reduce		4U
#define HashReducePost	5U
#define ReducePost	6U
#define FirstLookAhead	7U
#define NextLookAhead	8U
#define ActPrdct	9U

/* system action code */
#define LowerToUpper	1
#define UpperToLower	2
#define Set		3
#define Reset		4
#define Erase		5
#define Include		6
#define UpperCase	7
#define LowerCase	8
#define Put		9
#define Mark		10
#define Release		11
#define Incr		12
#define Decr		13

/* system predicate code */
#define IsTrue		0
#define IsFirstCol	1
#define	IsSet		2
#define	IsReset		3
#define	NotIsFirstCol	4
#define IsLastCol	5
#define NotIsLastCol	6

