#include "sxunix.h"

extern struct sxtables sxtables;

int main (void)
{
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

     return 0;
}
