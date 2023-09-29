/* ********************************************************
   *							  *
   *							  *
   * Copyright (c) 2003 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *							  *
   *							  *
   ******************************************************** */




/* ********************************************************
   *							  *
   * Produit de l'�quipe ATOLL (Langages et Traducteurs). *
   *							  *
   ******************************************************** */



/* Pr�lude pour tout fichier ``include'' de SYNTAX */




/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030527 13:31 (phd):	Cr�ation ex nihilo			*/
/************************************************************************/



#ifdef	__cplusplus
extern "C" {
#endif


/* Chaque d�claration de variable doit aussi �tre une d�finition de cette
   variable, selon la valeur donn�e � SX_GLOBAL_VAR : "extern" ou rien.

   Typiquement, cela donne, pour la variable "variable" de type "type"
   et de valeur initiale "valeur" :

   SX_GLOBAL_VAR  type  variable  SX_INIT_VAL(valeur);
*/

#ifndef SX_GLOBAL_VAR /* Inutile de le faire plusieurs fois */
#  ifdef SX_DFN_EXT_VAR
/* Cas particulier : Il faut D�FINIR les variables globales. */
#    define SX_GLOBAL_VAR	/*rien*/
#    define SX_INIT_VAL(v)	= v
#  else
/* Cas g�n�ral : Ce qu'on veut, c'est juste D�CLARER les variables globales, pas les D�FINIR. */
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
