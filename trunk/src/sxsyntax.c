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

/* Module generique d'appel a SYNTAX */

#include <stdarg.h>
#include "sxversion.h"
#include "sxunix.h"

char WHAT_SXSYNTAX[] = "@(#)SYNTAX - $Id: sxsyntax.c 3633 2023-12-20 18:41:19Z garavel $" WHAT_DEBUG;

/*---------------------------------------------------------------------------*/

static bool sxuses_incl_mngr = false;

/*---------------------------------------------------------------------------*/

void	syntax (SXINT syntax_what, struct sxtables *tables, ...)
{
    va_list ap;
    FILE *file;
    char *filename;
    bool sxdelete_str_mngr;

    switch (syntax_what) {
    case SXOPEN: /* arguments: syntax (SXOPEN, tables) */

	/* CONVECS: syntax (SXOPEN...) is reserved to advanced users */
	(*(tables->analyzers.scanner)) (SXOPEN, tables);
	(*(tables->analyzers.parser)) (SXOPEN, tables);

	if (tables->SXS_tables.S_is_user_action_or_prdct)
	    (*(tables->SXS_tables.scanact)) (SXOPEN, tables);

	if (tables->SXP_tables.parsact != NULL)
	    (*(tables->SXP_tables.parsact)) (SXOPEN, tables);

	if (tables->SXP_tables.semact != NULL)
	  (*(tables->SXP_tables.semact)) (SXOPEN, tables);
	break;

    case SXCLOSE: /* arguments: syntax (SXCLOSE, tables) */

	/* CONVECS: syntax (SXCLOSE...) is reserved to advanced users */
	if (tables->SXP_tables.semact != NULL)
	  (*(tables->SXP_tables.semact)) (SXCLOSE, tables);

	if (tables->SXP_tables.parsact != NULL)
	    (*(tables->SXP_tables.parsact)) (SXCLOSE, tables);

	if (tables->SXS_tables.S_is_user_action_or_prdct)
	    (*(tables->SXS_tables.scanact)) (SXCLOSE, tables);

	(*(tables->analyzers.parser)) (SXCLOSE, tables);
	(*(tables->analyzers.scanner)) (SXCLOSE, tables);
	break;

    case SXINIT: /* arguments: syntax (SXINIT, tables, sxuses_incl_mngr) */

	/* added by INRIA/CONVECS */
	va_start (ap, tables);
        sxuses_incl_mngr = va_arg (ap, SXINT /* bool */);
	sxopentty ();
	sxstr_mngr (SXBEGIN);
	(*(tables->analyzers.parser)) (SXBEGIN, tables); /* it is unclear if the 'tables' parameter is really needed after SXBEGIN */
	syntax (SXOPEN, tables);
	if (sxuses_incl_mngr)
	    sxincl_mngr (SXOPEN);
	va_end (ap);
	break;

    case SXBEGIN: /* arguments: syntax (SXBEGIN, tables, file, filename) */

	/* added by INRIA/CONVECS */
	/* parameter 'tables' is unused except for the call to va_arg() */
	va_start (ap, tables);
	file = va_arg (ap, FILE *);
	filename = va_arg (ap, char *);
	sxsrc_mngr (SXINIT, file, filename);
	if (sxuses_incl_mngr)
	    sxincl_mngr (SXINIT);
	sxerr_mngr (SXBEGIN);
	va_end (ap);
	break;

    case SXACTION: /* arguments: syntax (SXACTION, tables) */

	(*(tables->analyzers.scanner)) (SXINIT, tables);
	(*(tables->analyzers.parser)) (SXINIT, tables);

	if (tables->SXS_tables.S_is_user_action_or_prdct)
	    (*(tables->SXS_tables.scanact)) (SXINIT, tables);

	if (tables->SXP_tables.parsact != NULL)
	    (*(tables->SXP_tables.parsact)) (SXINIT, tables);

	if (tables->SXP_tables.semact != NULL)
	  (*(tables->SXP_tables.semact)) (SXINIT, tables);

	(*(tables->analyzers.parser)) (SXACTION, tables);
	
	if (tables->SXP_tables.semact != NULL)
	  (*(tables->SXP_tables.semact)) (SXFINAL, tables);

	if (tables->SXP_tables.parsact != NULL)
	    (*(tables->SXP_tables.parsact)) (SXFINAL, tables);

	if (tables->SXS_tables.S_is_user_action_or_prdct)
	    (*(tables->SXS_tables.scanact)) (SXFINAL, tables);

	(*(tables->analyzers.parser)) (SXFINAL, tables);
	(*(tables->analyzers.scanner)) (SXFINAL, tables);

	if (tables->SXP_tables.semact != NULL)
	  (*(tables->SXP_tables.semact)) (SXSEMPASS, tables);
	break;

    case SXEND: /* arguments: syntax (SXEND, tables) */

	/* added by INRIA/CONVECS */
	(void) tables; /* parameter 'tables' is unused */
	sxsrc_mngr (SXFINAL);
	if (sxuses_incl_mngr)
	    sxincl_mngr (SXFINAL);
	sxerr_mngr (SXEND);
	break;

    case SXFINAL: /* arguments: syntax (SXFINAL, tables, sxdelete_str_mngr) */

	/* added by INRIA/CONVECS */
	va_start (ap, tables);
	sxdelete_str_mngr = va_arg (ap, SXINT /* bool */);
	syntax (SXCLOSE, tables);
	(*(tables->analyzers.parser)) (SXEND, tables);  /* it is unclear if the 'tables' parameter is really needed after SXEND */
	if (sxuses_incl_mngr)
	    sxincl_mngr (SXCLOSE);
	if (sxdelete_str_mngr)
	    sxstr_mngr (SXEND);
	break;

    default:
	fprintf (sxstderr, "The function \"syntax\" is out of date with respect to its specification.\n");
	sxexit(1);
    }
}

