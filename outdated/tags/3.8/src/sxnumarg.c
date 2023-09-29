#include "sxunix.h"


BOOLEAN	sxnumarg (arg, nb)
    register char	*arg;
    int		*nb;
{
    register int	val = 0;
    register char	c;

    switch (c = *arg) {
    case NUL:
	return FALSE;

    case '0':
	switch (c = *++arg) {
	case NUL:
	    return FALSE;

	case 'x':
	case 'X':
	    /* hexa */
	    if (c = *++arg)
		do {
		    val <<= 4;

		    if (c >= '0' && c <= '9')
			val += c - '0';
		    else if (c >= 'A' && c <= 'F')
			val += c - 'A' + 10;
		    else if (c >= 'a' && c <= 'f')
			val += c - 'a' + 10;
		    else
			return FALSE;
		} while (c = *++arg);
	    else
		return FALSE;

	    break;

	default:
	    /* octal */
	    do {
		val <<= 3;

		if (c >= '0' && c <= '7')
		    val += c - '0';
		else
		    return FALSE;
	    } while (c = *++arg);

	    break;
	}

	break;

    default:
	/* decimal */
	do {
	    val *= 10;

	    if (c >= '0' && c <= '9')
		val += c - '0';
	    else
		return FALSE;
	} while (c = *++arg);

	break;
    }

    *nb = val;
    return TRUE;
}
