#include "sxunix.h"
/* this is a sample, old-style C program for testing PPC */
void sxopentty ()
{
sxtty = fopen ("/dev/tty", "a");
if (sxtty != NULL && isatty (fileno (sxtty)))
setbuf (sxtty, NULL);
else {
sxtty = fopen ("/dev/null", "w");
if (sxtty == NULL) {
/* ceci peut arriver sous Windows si /dev/null n'existe pas */
sxtty = stderr;
}
}
}
