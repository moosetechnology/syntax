#include	<stdio.h>
#include "sxcommon.h"
char WHAT_DETAB[] = "@(#)SYNTAX - $Id: detab.c 529 2007-05-09 14:26:33Z rlacroix $" WHAT_DEBUG;

#define	tab_pos(col)	(((col)&07) == 0)
#ifndef LL
# define LL 132
#endif
#define LAST_TAB ((LL/8)*8)

main ()
{
    register int  c, col;

    for (col = 0;;) {
	switch (c = getchar ()) {
	case EOF:
	    return 0;
	case '\b':
	    putchar ('\b');
	    if (col-- == 0) {
		col = 0;
	    }
	    continue;
	case '\t':
	    if (col < LAST_TAB)
		do
		    putchar (' ');
		while (!tab_pos (++col));
	    else {
		putchar ('\n');
		col = 0;
	    }
	    continue;
	case '\f':
	case '\n':
	case '\r':
	    putchar (c);
	    col = 0;
	    continue;
	case '\v':
	    putchar ('\v');
	    continue;
	default:
	    if (col++ == LL) {
		col = 1;
		putchar ('\n');
	    }
	    putchar (c);
	    continue;
	}
    }
}
