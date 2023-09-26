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

#ifndef sem_by
#define sem_by

#define no_sem			0
#define sem_by_action		1	/* used by SEMC, SEMACT, YAX, etc. */
#define sem_by_abstract_tree	2	/* used by SEMAT */ 
#define sem_by_tabdef		3	/* no longer used in 2023 */
#define sem_by_tabact		4	/* no longer used in 2023 */
#define sem_by_FNC		5	/* no longer used in 2023 */
#define sem_by_paradis	  	6	/* used by PARADIS */

#endif

