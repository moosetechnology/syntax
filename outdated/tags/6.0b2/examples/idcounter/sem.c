
#include <stdio.h>
#include <stdlib.h>

/* #ifndef SXVOID */
#include "sxunix.h"
/* #endif */
char WHAT_SEM[] = "@(#)SYNTAX - $Id: sem.c 535 2007-05-10 13:35:37Z rlacroix $" WHAT_DEBUG;


FILE *sxstdout;
FILE *sxstderr;
FILE *sxtty;
BOOLEAN sxverbosep = {FALSE};

extern struct sxtables sxtables;

typedef char *ptchar;

BOOLEAN IS_VAR;

BOOLEAN SPECIAL_ACCEPTED;

struct sxstrmngr SYNTAX_TABLE;

struct sxstrmngr VAR_TABLE;

struct sxstrmngr TYPE_TABLE;

#define new_with_size(O,N,T) O = (T) malloc(N)

#define old(O) free ((char *) (O))

void INIT_SYNTACTIC_ANALYSIS ()

{
SYNTAX_TABLE = sxstrmngr;
sxstrmngr = VAR_TABLE;
sxstr_mngr(BEGIN);
VAR_TABLE = sxstrmngr;
sxstrmngr = TYPE_TABLE;
sxstr_mngr(BEGIN);
TYPE_TABLE = sxstrmngr;
sxstrmngr = SYNTAX_TABLE;
}

void END_SYNTACTIC_ANALYSIS ()
{
SYNTAX_TABLE = sxstrmngr;
sxstrmngr = VAR_TABLE;
sxstr_mngr(END);
sxstrmngr = TYPE_TABLE;
sxstr_mngr(END);
sxstrmngr = SYNTAX_TABLE;
}

void INIT_VAR ()
{
IS_VAR = TRUE;
}

void INIT_TYPE ()
{
IS_VAR = FALSE;
}

void VERIF_UNICITE (STRING_TABLE_ENTRY, CURRENT_NAME)
int STRING_TABLE_ENTRY;
ptchar *CURRENT_NAME;

{

new_with_size(*CURRENT_NAME, sizeof(char) * (32), ptchar);
strncpy(*CURRENT_NAME,(sxstrget(STRING_TABLE_ENTRY)), 31);
(*CURRENT_NAME)[31] = '\000';
SYNTAX_TABLE = sxstrmngr;
if (IS_VAR == TRUE)
     sxstrmngr = VAR_TABLE;
else
     sxstrmngr = TYPE_TABLE;
if (sxstrretrieve(*CURRENT_NAME) == ERROR_STE)
     sxstrsave(*CURRENT_NAME);
else
     printf("%s already declared\n", *CURRENT_NAME);
if (IS_VAR == TRUE)
     VAR_TABLE = sxstrmngr;
else
     TYPE_TABLE = sxstrmngr;
sxstrmngr = SYNTAX_TABLE;
}

void PRINT_SPECIAL (CURRENT_NAME)
char *CURRENT_NAME;

{
printf("identifier found: \"%s\"\n", CURRENT_NAME);
}

void scanner_act (ENTRY, ACTION_NUMBER)
int  ENTRY, ACTION_NUMBER;	/* numero de l'action lexicale cf manuel
				 * SYNTAX */


{
     char *NAME;

     switch (ENTRY) {
     case OPEN:
     case CLOSE:
     case INIT:
     case FINAL:
	  return;

     case ACTION:
	  switch (ACTION_NUMBER) {
	  case 1:
	       if (SPECIAL_ACCEPTED == TRUE){
	       new_with_size(NAME, sizeof(char) * (sxsvar.sxlv.ts_lgth + 1),
	       ptchar);
	       strncpy (NAME, sxsvar.sxlv_s.token_string, sxsvar.sxlv.ts_lgth);
	       NAME[sxsvar.sxlv.ts_lgth] = NULL ;
	       printf("special comment: %s\n", NAME);
	       old(NAME);
	       SPECIAL_ACCEPTED = FALSE;
	       }
	       return;

	  case 2:
	       SPECIAL_ACCEPTED = TRUE;
	       return;

	  case 3:
	       SPECIAL_ACCEPTED = FALSE;
	       return;

	  default:
	       return;
	  }

     default:
	  return;
     }
}

void DELETE_SUFFIX () {
}

int main ()
{
     sxstdout = stdout;
     sxstderr = stderr;

     sxstr_mngr (BEGIN);
     /* Creation de la table des chaines */

     (*(sxtables.analyzers.parser)) (BEGIN);
     /* Allocation des variables globales du parser */

     syntax (OPEN, &sxtables);
     /* Initialisation de SYNTAX (mono-langage) */

     sxsrc_mngr (INIT, stdin, "");
     sxerr_mngr (BEGIN);
     syntax (ACTION, &sxtables);
     sxsrc_mngr (FINAL);
     sxerr_mngr (END);
     syntax (CLOSE, &sxtables);
     (*(sxtables.analyzers.parser)) (END);
     sxstr_mngr (END);


}

