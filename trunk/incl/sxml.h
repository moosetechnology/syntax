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

char WHAT_XMLBUILD[] = "@(#)SYNTAX - $Id: sxml.h 3718 2024-02-10 08:23:15Z garavel $";

#include <stdio.h>
#include <assert.h>
#include <math.h> /* for log10l() ; add -lm option on Linux */

/* ------------------------------------------------------------------------- */

// #define SXML_OOM_ERROR(OOM_MESSAGE) return (OOM_MESSAGE)

#define SXML_OOM_ERROR(OOM_MESSAGE) { \
	fprintf (sxstderr, "%s\n", OOM_MESSAGE); \
	sxexit (sxerr_max_severity ()); \
	return NULL; \
	}

/* ------------------------------------------------------------------------- */

typedef char *SXML_TYPE_TEXT;

/* ------------------------------------------------------------------------- */

SXML_TYPE_TEXT SXML_UINT (SXUINT N)
{
	/* convert N to a character string */
	int LENGTH;
	char *STRING;

	LENGTH = (N == 0) ? 1 : floor (log10l (N)) + 1;
	STRING = calloc (sizeof (char), LENGTH + 1); // +1 for '\0'
	if (STRING == NULL) {
		SXML_OOM_ERROR ("out of memory in SXML_UINT()");
	} else {
		snprintf (STRING, LENGTH + 1, "%lu", N);
    		return STRING;
	}
}

/* ------------------------------------------------------------------------- */

SXML_TYPE_TEXT SXML_INT (SXINT N)
{
	/* implementation of itoa(), which does not exist on Linux */
	/* convert N to a character string */
	int LENGTH;
	char *STRING;

	LENGTH = (N == 0) ? 1 : floor (log10l (labs (N))) + 1;
	if (N < 0) ++ LENGTH; // +1 for negative sign '-'
	STRING = calloc (sizeof (char), LENGTH + 1); // +1 for '\0'
	if (STRING == NULL) {
		SXML_OOM_ERROR ("out of memory in SXML_INT()");
	} else {
		snprintf (STRING, LENGTH + 1, "%ld", N);
    		return STRING;
	}
}

/* ------------------------------------------------------------------------- */

SXML_TYPE_TEXT SXML_QUOTED (char C1, char *S, char C2)
{
	/* returns a copy of S enclosed between characters C1 on the left
	 * and C2 on the right; for instance C1 and C2 may be single quotes,
	 * double quotes, parentheses, brackets, braces, etc.
	 * Note: the user is reponsible for freeing S where applicable
	 */
	int LENGTH;
	char *STRING;

	assert (S != NULL);
	LENGTH = strlen (S) + 2; // +2 for C1 and C2
	STRING = calloc (sizeof (char), LENGTH + 1); // +1 for '\0'
	if (STRING == NULL) {
		SXML_OOM_ERROR ("out of memory in SXML_QUOTED()");
	} else {
		snprintf (STRING, LENGTH + 1, "%c%s%c", C1, S, C2);
    		return STRING;
	}
}

/* ------------------------------------------------------------------------- */

#define SXML_Q(T) ((T) == NULL ? NULL : SXML_QUOTED ('\"', (T), '\"'))

/* ------------------------------------------------------------------------- */

typedef struct SXML_STRUCT_LIST {
   SXML_TYPE_TEXT TEXT;
   struct SXML_STRUCT_LIST *SUCC;
} SXML_BODY_LIST, *SXML_TYPE_LIST;

/* non-empty list: the list contains always at least one element */
/* circular list: the list is referred by a pointer to its last element, and
 * the SUCC pointer of the last element refers to the first element */

/* ------------------------------------------------------------------------- */

#define SXML_HEAD(L) ((L)->SUCC)
/* pointer to the actual first element of the circular list */
/* undefined result if L == NULL */

#define SXML_TAIL(L) (L)
/* pointer to the last element of the circular list */

#define SXML_SUCC(L,LIST) ((L) == (LIST) ? NULL : (L)->SUCC)
/* pointer to the next element of L in the circular list pointed to by LIST */
/* undefined result if L == NULL and LIST != NULL */

/* ------------------------------------------------------------------------- */

void SXML_PRINT (FILE *OUTPUT, SXML_TYPE_LIST LIST)
{
	SXML_TYPE_LIST L;

	/* prints LIST to OUTPUT file */
	assert (LIST != NULL);
	for (L = SXML_HEAD (LIST); L != NULL; L = SXML_SUCC (L, LIST)) {
		fprintf (OUTPUT, "%s", L->TEXT);
	}
	fprintf (OUTPUT, "\n");
}

/* ------------------------------------------------------------------------- */

SXML_TYPE_LIST SXML_L (SXML_TYPE_LIST L)
{
	return L;
}

/* ------------------------------------------------------------------------- */

SXML_TYPE_LIST SXML_T (SXML_TYPE_TEXT T)
{
	SXML_TYPE_LIST L;

	/* allocates and returns a list of length one */
	assert (T != NULL);
	L = malloc (sizeof (SXML_BODY_LIST));
	if (L == NULL) {
		fprintf (sxstderr, "out of memory in sxml.h\n");
		sxexit (sxerr_max_severity ());
		return NULL; /* never reached */
	}
	L->TEXT = T;
	L->SUCC = L; /* circular link */
	return L;
}

/* ------------------------------------------------------------------------- */

SXML_TYPE_LIST SXML_LL (
		SXML_TYPE_LIST L1,
		SXML_TYPE_LIST L2)
{
	/* returns the in-place concatenation of L1 and L2 */
	SXML_TYPE_LIST L;

	assert (L1 != NULL);
	assert (L2 != NULL);
	L = SXML_HEAD (L1);
	SXML_TAIL (L1)->SUCC = SXML_HEAD (L2);
	SXML_TAIL (L2)->SUCC = L;
	return L2;
}

/* ------------------------------------------------------------------------- */

SXML_TYPE_LIST SXML_LT (
		SXML_TYPE_LIST L1,
		SXML_TYPE_TEXT T2)
{
	return SXML_LL (L1, SXML_T (T2));
}

/* ------------------------------------------------------------------------- */

SXML_TYPE_LIST SXML_TL (
		SXML_TYPE_TEXT T1,
		SXML_TYPE_LIST L2)
{
	return SXML_LL (SXML_T (T1), L2);
}

/* ------------------------------------------------------------------------- */

SXML_TYPE_LIST SXML_TT (
		SXML_TYPE_TEXT T1,
		SXML_TYPE_TEXT T2)
{
	return SXML_LL (SXML_T (T1), SXML_T (T2));
}

/* ------------------------------------------------------------------------- */

SXML_TYPE_LIST SXML_LLL (
		SXML_TYPE_LIST L1,
		SXML_TYPE_LIST L2,
		SXML_TYPE_LIST L3)
{
	return SXML_LL (SXML_LL (L1, L2), L3);
}

/* ------------------------------------------------------------------------- */

SXML_TYPE_LIST SXML_LLT (
		SXML_TYPE_LIST L1,
		SXML_TYPE_LIST L2,
		SXML_TYPE_TEXT T3)
{
	return SXML_LLL (L1, L2, SXML_T (T3));
}

/* ------------------------------------------------------------------------- */

SXML_TYPE_LIST SXML_LTL (
		SXML_TYPE_LIST L1,
		SXML_TYPE_TEXT T2,
		SXML_TYPE_LIST L3)
{
	return SXML_LLL (L1, SXML_T (T2), L3);
}

/* ------------------------------------------------------------------------- */

SXML_TYPE_LIST SXML_LTT (
		SXML_TYPE_LIST L1,
		SXML_TYPE_TEXT T2,
		SXML_TYPE_TEXT T3)
{
	return SXML_LLL (L1, SXML_T (T2), SXML_T (T3));
}

/* ------------------------------------------------------------------------- */

SXML_TYPE_LIST SXML_TLL (
		SXML_TYPE_TEXT T1,
		SXML_TYPE_LIST L2,
		SXML_TYPE_LIST L3)
{
	return SXML_LLL (SXML_T (T1), L2, L3);
}

/* ------------------------------------------------------------------------- */

SXML_TYPE_LIST SXML_TLT (
		SXML_TYPE_TEXT T1,
		SXML_TYPE_LIST L2,
		SXML_TYPE_TEXT T3)
{
	return SXML_LLL (SXML_T (T1), L2, SXML_T (T3));
}

/* ------------------------------------------------------------------------- */

SXML_TYPE_LIST SXML_TTL (
		SXML_TYPE_TEXT T1,
		SXML_TYPE_TEXT T2,
		SXML_TYPE_LIST L3)
{
	return SXML_LLL (SXML_T (T1), SXML_T (T2), L3);
}

/* ------------------------------------------------------------------------- */

SXML_TYPE_LIST SXML_TTT (
		SXML_TYPE_TEXT T1,
		SXML_TYPE_TEXT T2,
	 	SXML_TYPE_TEXT T3)
{
	return SXML_LLL (SXML_T (T1), SXML_T (T2), SXML_T (T3));
}

/* ------------------------------------------------------------------------- */

SXML_TYPE_LIST SXML_LLLL (
		SXML_TYPE_LIST L1,
		SXML_TYPE_LIST L2,
		SXML_TYPE_LIST L3,
		SXML_TYPE_LIST L4)
{
	return SXML_LL (SXML_LLL (L1, L2, L3), L4);
}

/* ------------------------------------------------------------------------- */

SXML_TYPE_LIST SXML_LLLT (
                SXML_TYPE_LIST L1,
                SXML_TYPE_LIST L2,
                SXML_TYPE_LIST L3,
                SXML_TYPE_TEXT T4)
{
        return SXML_LL (SXML_LLL (L1, L2, L3), SXML_T (T4));
}

/* ------------------------------------------------------------------------- */

SXML_TYPE_LIST SXML_LLTL (
		SXML_TYPE_LIST L1,
		SXML_TYPE_LIST L2,
		SXML_TYPE_TEXT T3,
		SXML_TYPE_LIST L4)
{
	return SXML_LLLL (L1, L2, SXML_T (T3), L4);
}

/* ------------------------------------------------------------------------- */

SXML_TYPE_LIST SXML_LLTT (
		SXML_TYPE_LIST L1,
		SXML_TYPE_LIST L2,
		SXML_TYPE_TEXT T3,
		SXML_TYPE_TEXT T4)
{
	return SXML_LLLL (L1, L2, SXML_T (T3), SXML_T (T4));
}

/* ------------------------------------------------------------------------- */

SXML_TYPE_LIST SXML_LTLL (
		SXML_TYPE_LIST L1,
		SXML_TYPE_TEXT T2,
		SXML_TYPE_LIST L3,
		SXML_TYPE_LIST L4)
{
	return SXML_LLLL (L1, SXML_T (T2), L3, L4);
}

/* ------------------------------------------------------------------------- */

SXML_TYPE_LIST SXML_LTLT (
		SXML_TYPE_LIST L1,
		SXML_TYPE_TEXT T2,
		SXML_TYPE_LIST L3,
		SXML_TYPE_TEXT T4)
{
 	return SXML_LLLL (L1, SXML_T (T2), L3, SXML_T (T4));
}

/* ------------------------------------------------------------------------- */

SXML_TYPE_LIST SXML_LTTL (
		SXML_TYPE_LIST L1,
		SXML_TYPE_TEXT T2,
		SXML_TYPE_TEXT T3,
		SXML_TYPE_LIST L4)
{
	return SXML_LLL (L1, SXML_TT (T2, T3), L4);
}

/* ------------------------------------------------------------------------- */

SXML_TYPE_LIST SXML_LTTT (
		SXML_TYPE_LIST L1,
		SXML_TYPE_TEXT T2,
		SXML_TYPE_TEXT T3,
		SXML_TYPE_TEXT T4)
{
	return SXML_LL (L1, SXML_TTT (T2, T3, T4));
}

/* ------------------------------------------------------------------------- */

SXML_TYPE_LIST SXML_TLLT (
		SXML_TYPE_TEXT T1,
		SXML_TYPE_LIST L2,
		SXML_TYPE_LIST L3,
		SXML_TYPE_TEXT T4)
{
	return SXML_LLLL (SXML_T (T1), L2, L3, SXML_T (T4));
}

/* ------------------------------------------------------------------------- */

SXML_TYPE_LIST SXML_TTLL (
		SXML_TYPE_TEXT T1,
		SXML_TYPE_TEXT T2,
		SXML_TYPE_LIST L3,
		SXML_TYPE_LIST L4)
{
	return SXML_LLLL (SXML_T (T1), SXML_T (T2), L3, L4);
}

/* ------------------------------------------------------------------------- */

SXML_TYPE_LIST SXML_TTTL (
		SXML_TYPE_TEXT T1,
		SXML_TYPE_TEXT T2,
		SXML_TYPE_TEXT T3,
		SXML_TYPE_LIST L4)
{
	return SXML_LL (SXML_TTT (T1, T2, T3), L4);
}

/* ------------------------------------------------------------------------- */

SXML_TYPE_LIST SXML_TTTT (
		SXML_TYPE_TEXT T1,
		SXML_TYPE_TEXT T2,
	 	SXML_TYPE_TEXT T3,
		SXML_TYPE_TEXT T4)
{
	return SXML_LLLL (SXML_T (T1), SXML_T (T2), SXML_T (T3), SXML_T (T4));
}

/* ------------------------------------------------------------------------- */

SXML_TYPE_LIST SXML_LLLLL (
		SXML_TYPE_LIST L1,
		SXML_TYPE_LIST L2,
		SXML_TYPE_LIST L3,
		SXML_TYPE_LIST L4,
		SXML_TYPE_LIST L5)
{
	return SXML_LL (SXML_LLLL (L1, L2, L3, L4), L5);
}

/* ------------------------------------------------------------------------- */

SXML_TYPE_LIST SXML_LLLTL (
		SXML_TYPE_LIST L1,
		SXML_TYPE_LIST L2,
		SXML_TYPE_LIST L3,
		SXML_TYPE_TEXT T4,
		SXML_TYPE_LIST L5)
{
	return SXML_LLLLL (L1, L2, L3, SXML_T (T4), L5);
}

/* ------------------------------------------------------------------------- */

SXML_TYPE_LIST SXML_LLTLL (
		SXML_TYPE_LIST L1,
		SXML_TYPE_LIST L2,
		SXML_TYPE_TEXT T3,
		SXML_TYPE_LIST L4,
		SXML_TYPE_LIST L5)
{
	return SXML_LLLLL (L1, L2, SXML_T (T3), L4, L5);
}

/* ------------------------------------------------------------------------- */

SXML_TYPE_LIST SXML_LTLLL (
		SXML_TYPE_LIST L1,
		SXML_TYPE_TEXT T2,
		SXML_TYPE_LIST L3,
		SXML_TYPE_LIST L4,
		SXML_TYPE_LIST L5)
{
	return SXML_LLLLL (L1, SXML_T (T2), L3, L4, L5);
}

/* ------------------------------------------------------------------------- */

SXML_TYPE_LIST SXML_LTLTL (
		SXML_TYPE_LIST L1,
		SXML_TYPE_TEXT T2,
		SXML_TYPE_LIST L3,
		SXML_TYPE_TEXT T4,
		SXML_TYPE_LIST L5)
{
	return SXML_LLLLL (L1, SXML_T (T2), L3, SXML_T (T4), L5);
}

/* ------------------------------------------------------------------------- */

SXML_TYPE_LIST SXML_TLLLT (
		SXML_TYPE_TEXT T1,
		SXML_TYPE_LIST L2,
		SXML_TYPE_LIST L3,
		SXML_TYPE_LIST L4,
		SXML_TYPE_TEXT T5)
{
	return SXML_LLLLL (SXML_T (T1), L2, L3, L4, SXML_T (T5));
}

/* ------------------------------------------------------------------------- */

SXML_TYPE_LIST SXML_TTTLT (
		SXML_TYPE_TEXT T1,
		SXML_TYPE_TEXT T2,
		SXML_TYPE_TEXT T3,
		SXML_TYPE_LIST L4,
		SXML_TYPE_TEXT T5)
{
	return SXML_LLL (SXML_TTT (T1, T2, T3), L4, SXML_T (T5));
}

/* ------------------------------------------------------------------------- */

SXML_TYPE_LIST SXML_TTTTL (
		SXML_TYPE_TEXT T1,
		SXML_TYPE_TEXT T2,
		SXML_TYPE_TEXT T3,
		SXML_TYPE_TEXT T4,
		SXML_TYPE_LIST L5)
{
	return SXML_LL (SXML_TTTT (T1, T2, T3, T4), L5);
}

/* ------------------------------------------------------------------------- */

SXML_TYPE_LIST SXML_TTTTT (
		SXML_TYPE_TEXT T1,
		SXML_TYPE_TEXT T2,
		SXML_TYPE_TEXT T3,
		SXML_TYPE_TEXT T4,
		SXML_TYPE_TEXT T5)
{
	return SXML_LL (SXML_TTT (T1, T2, T3), SXML_TT (T4, T5));
}

/* ------------------------------------------------------------------------- */

SXML_TYPE_LIST SXML_LLTLTL (
		SXML_TYPE_LIST L1,
		SXML_TYPE_LIST L2,
		SXML_TYPE_TEXT T3,
		SXML_TYPE_LIST L4,
		SXML_TYPE_TEXT T5,
		SXML_TYPE_LIST L6)
{
	return SXML_LL (SXML_LLT (L1, L2, T3), SXML_LTL (L4, T5, L6));
}

/* ------------------------------------------------------------------------- */

SXML_TYPE_LIST SXML_LTLLLL (
		SXML_TYPE_LIST L1,
		SXML_TYPE_TEXT T2,
		SXML_TYPE_LIST L3,
		SXML_TYPE_LIST L4,
		SXML_TYPE_LIST L5,
		SXML_TYPE_LIST L6)
{
	return SXML_LL (SXML_LTL (L1, T2, L3), SXML_LLL (L4, L5, L6));
}

/* ------------------------------------------------------------------------- */

SXML_TYPE_LIST SXML_TTTTTT (
		SXML_TYPE_TEXT T1,
		SXML_TYPE_TEXT T2,
		SXML_TYPE_TEXT T3,
		SXML_TYPE_TEXT T4,
		SXML_TYPE_TEXT T5,
		SXML_TYPE_TEXT T6)
{
	return SXML_LL (SXML_TTT (T1, T2, T3), SXML_TTT (T4, T5, T6));
}

/* ------------------------------------------------------------------------- */

SXML_TYPE_LIST SXML_LLLTLTL (
		SXML_TYPE_LIST L1,
		SXML_TYPE_LIST L2,
		SXML_TYPE_LIST L3,
		SXML_TYPE_TEXT T4,
		SXML_TYPE_LIST L5,
		SXML_TYPE_TEXT T6,
		SXML_TYPE_LIST L7)
{
	return SXML_LLL (SXML_LLL (L1, L2, L3), SXML_TL (T4, L5), SXML_TL (T6, L7));
}

/* ------------------------------------------------------------------------- */

SXML_TYPE_LIST SXML_TTTLTLT (
		SXML_TYPE_TEXT T1,
		SXML_TYPE_TEXT T2,
		SXML_TYPE_TEXT T3,
		SXML_TYPE_LIST L4,
		SXML_TYPE_TEXT T5,
		SXML_TYPE_LIST L6,
		SXML_TYPE_TEXT T7)
{
	return SXML_LLL (SXML_TTTLT (T1, T2, T3, L4, T5), L6, SXML_T (T7));
}

/* ------------------------------------------------------------------------- */

SXML_TYPE_LIST SXML_TTTTTTT (
		SXML_TYPE_TEXT T1,
		SXML_TYPE_TEXT T2,
		SXML_TYPE_TEXT T3,
		SXML_TYPE_TEXT T4,
		SXML_TYPE_TEXT T5,
		SXML_TYPE_TEXT T6,
		SXML_TYPE_TEXT T7)
{
	return SXML_LL (SXML_TTTT (T1, T2, T3, T4), SXML_TTT (T5, T6, T7));
}

/* ------------------------------------------------------------------------- */

SXML_TYPE_LIST SXML_TTTTTTTT (
		SXML_TYPE_TEXT T1,
		SXML_TYPE_TEXT T2,
		SXML_TYPE_TEXT T3,
		SXML_TYPE_TEXT T4,
		SXML_TYPE_TEXT T5,
		SXML_TYPE_TEXT T6,
		SXML_TYPE_TEXT T7,
		SXML_TYPE_TEXT T8)
{
	return SXML_LL (SXML_TTTT (T1, T2, T3, T4), SXML_TTTT (T5, T6, T7, T8));
}

/* ------------------------------------------------------------------------- */

SXML_TYPE_LIST SXML_TTTTTTTTT (
		SXML_TYPE_TEXT T1,
		SXML_TYPE_TEXT T2,
		SXML_TYPE_TEXT T3,
		SXML_TYPE_TEXT T4,
		SXML_TYPE_TEXT T5,
		SXML_TYPE_TEXT T6,
		SXML_TYPE_TEXT T7,
		SXML_TYPE_TEXT T8,
		SXML_TYPE_TEXT T9)
{
	return SXML_LL (SXML_TTTT (T1, T2, T3, T4), SXML_TTTTT (T5, T6, T7, T8, T9));
}

/* ------------------------------------------------------------------------- */

SXML_TYPE_LIST SXML_TTTTTTTTTTT (
		SXML_TYPE_TEXT T1,
		SXML_TYPE_TEXT T2,
		SXML_TYPE_TEXT T3,
		SXML_TYPE_TEXT T4,
		SXML_TYPE_TEXT T5,
		SXML_TYPE_TEXT T6,
		SXML_TYPE_TEXT T7,
		SXML_TYPE_TEXT T8,
		SXML_TYPE_TEXT T9,
		SXML_TYPE_TEXT T10,
		SXML_TYPE_TEXT T11)
{
	return SXML_LL (SXML_TTTTT (T1, T2, T3, T4, T5), SXML_TTTTTT (T6, T7, T8, T9, T10, T11));
}

/* ------------------------------------------------------------------------- */


SXML_TYPE_LIST SXML_QUOTED_LIST (SXML_TYPE_LIST L)
{
	return SXML_TLT("\"", L, "\"");
}

/* ------------------------------------------------------------------------- */