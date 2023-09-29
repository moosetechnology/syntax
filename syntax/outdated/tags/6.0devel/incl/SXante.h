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







/* Prélude pour tout fichier ``include'' de SYNTAX */







#ifdef	__cplusplus
extern "C" {
#endif


/* Chaque déclaration de variable doit aussi être une définition de cette
   variable, selon la valeur donnée à SX_GLOBAL_VAR : "extern" ou rien.

   Typiquement, cela donne, pour la variable "variable" de type "type"
   et de valeur initiale "valeur" :

   SX_GLOBAL_VAR  type  variable  SX_INIT_VAL(valeur);
*/

#ifndef SX_GLOBAL_VAR /* Inutile de le faire plusieurs fois */
#  ifdef SX_DFN_EXT_VAR
/* Cas particulier : Il faut DÉFINIR les variables globales. */
#    define SX_GLOBAL_VAR	/*rien*/
#    define SX_INIT_VAL(v)	= v
#  else
/* Cas général : Ce qu'on veut, c'est juste DÉCLARER les variables globales, pas les DÉFINIR. */
#    define SX_GLOBAL_VAR	extern
#    define SX_INIT_VAL(v)	/*rien*/
#  endif /* #ifndef SX_DFN_EXT_VAR */
#endif /* #ifndef SX_GLOBAL_VAR */

/*
 * Local Variables:
 * mode: C
 * version-control: yes
 * End:
 */
