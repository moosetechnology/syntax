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

#ifndef read_a_re_h
#define read_a_re_h

extern SXINT re_reader (char *pathname_or_string, bool from_file, SXINT (*re_process)(void));

extern SXINT read_a_re (void (*prelude)(bool, SXINT, SXINT, SXINT, SXINT), 
			void (*store)(SXINT, struct sxtoken **, struct sxtoken **, SXINT, SXINT), 
			SXINT (*postlude)(SXINT), 
			SXINT what_to_do);

// extern void re_smp (SXINT what, SXTABLES *sxtables_ptr)

// extern void sxparser_re_tcut  (SXINT what, SXTABLES *arg)

#endif

