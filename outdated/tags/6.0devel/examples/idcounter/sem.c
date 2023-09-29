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


#include <stdio.h>
#include <stdlib.h>

/* #ifndef SXVOID */
#include "sxunix.h"
/* #endif */
char WHAT_SEM[] = "@(#)SYNTAX - $Id: sem.c 2227 2018-09-26 16:22:59Z garavel $";

extern struct sxtables sxtables;

typedef char *ptchar;

SXBOOLEAN IS_VAR;

SXBOOLEAN SPECIAL_ACCEPTED;

sxstrmngr_t VAR_TABLE;

sxstrmngr_t TYPE_TABLE;

#define new_with_size(O,N,T) O = (T) malloc(N)

#define old(O) free ((char *) (O))

void INIT_SYNTACTIC_ANALYSIS (void)
{
sxstr_mngr (SXOPEN, &VAR_TABLE);
sxstr_mngr (SXOPEN, &TYPE_TABLE);
}

void END_SYNTACTIC_ANALYSIS (void)
{
sxstr_mngr (SXCLOSE, &VAR_TABLE);
sxstr_mngr (SXCLOSE, &TYPE_TABLE);
}

void INIT_VAR (void)
{
IS_VAR = SXTRUE;
}

void INIT_TYPE (void)
{
IS_VAR = SXFALSE;
}

void VERIF_UNICITE (SXINT STRING_TABLE_ENTRY, ptchar *CURRENT_NAME)

{
sxstrmngr_t *table;

new_with_size(*CURRENT_NAME, sizeof(char) * (32), ptchar);
strncpy(*CURRENT_NAME,(sxstrget(STRING_TABLE_ENTRY)), 31);
(*CURRENT_NAME)[31] = '\000';
if (IS_VAR == SXTRUE)
     table = &VAR_TABLE;
else
     table = &TYPE_TABLE;
if (sxstr_retrieve(table, *CURRENT_NAME) == SXERROR_STE)
     sxstr_save(table, *CURRENT_NAME);
else
     printf("%s already declared\n", *CURRENT_NAME);
}

void PRINT_SPECIAL (char *CURRENT_NAME)

{
printf("identifier found: \"%s\"\n", CURRENT_NAME);
}

void scanner_act (SXINT ENTRY, SXINT ACTION_NUMBER)
 /* numero de l'action lexicale cf manuel SYNTAX */
{
     char *NAME;

     switch (ENTRY) {
     case SXOPEN:
     case SXCLOSE:
     case SXINIT:
     case SXFINAL:
	  return;

     case SXACTION:
	  switch (ACTION_NUMBER) {
	  case 1:
	       if (SPECIAL_ACCEPTED == SXTRUE){
	       new_with_size(NAME, sizeof(char) * (sxsvar.sxlv.ts_lgth + 1),
	       ptchar);
	       strncpy (NAME, sxsvar.sxlv_s.token_string, sxsvar.sxlv.ts_lgth);
	       NAME[sxsvar.sxlv.ts_lgth] = 0 ;
	       printf("special comment: %s\n", NAME);
	       old(NAME);
	       SPECIAL_ACCEPTED = SXFALSE;
	       }
	       return;

	  case 2:
	       SPECIAL_ACCEPTED = SXTRUE;
	       return;

	  case 3:
	       SPECIAL_ACCEPTED = SXFALSE;
	       return;

	  default:
	       return;
	  }

     default:
	  return;
     }
}

void DELETE_SUFFIX (void) {
}

int main (void)
{
     syntax (SXINIT, &sxtables, SXFALSE);
     syntax (SXBEGIN, &sxtables, stdin, "");
     syntax (SXACTION, &sxtables);
     syntax (SXEND, &sxtables);
     syntax (SXFINAL, &sxtables, SXTRUE);

     return 0;
}

