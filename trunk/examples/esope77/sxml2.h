
char WHAT_SXML2[] = "@(#)SYNTAX - $Id: sxml2.h 3718 2024-09-10 08:23:15Z garavel $";

/* first we include the official version of sxml.h */
#include "sxml.h"

/* then, we add a few extensions, which Hubert does not want to put in sxml.h */

#define SXML_Q(T) ((T) == NULL ? NULL : SXML_QUOTED ('\"', (T), '\"'))
	/* Hubert warns about returning NULL */
	/* It seems that SXML_Q() is called only once in esope77 */

SXML_TYPE_LIST SXML_QUOTED_LIST (SXML_TYPE_LIST L)
{
	return SXML_TLT ("\"", L, "\"");
}
	/* Hubert: may be that this function will enter sxml.h some day */
	/* after renaming SXML_QUOTED() to SXML_QUOTED_TEXT() */

/* here, Larisa can add new functions */
/* ... */

