/* ********************************************************
   *							  *
   *							  *
   * Copyright (c) 1986 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *							  *
   *							  *
   ******************************************************** */




/* ********************************************************
   *							  *
   *  Produit de l'équipe Langages et Traducteurs.        *
   *							  *
   ******************************************************** */





/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030523 11:58 (phd):	Ajout de P0 (init. au pointeur NULL)	*/
/************************************************************************/


extern VOID out_bit(/* nom, chb */);
/* char* nom;SXBA chb; */

extern VOID out_struct(/* nom */);
/* char* nom; */

extern VOID out_tab_int(/* nom, tab, deb, fin */);
/* char* nom; int tab[]; int deb, fin; */

extern VOID out_short(/* nom */);
/* char* nom; */

extern VOID out_int(/* nom */);
/* char* nom; */

extern VOID out_ext_int();

extern VOID out_char(/* nom */);
/* char* nom; */

extern VOID out_end_struct (void);
extern VOID out_rcvr_trans (void);
extern VOID out_ext_BOOLEAN (char *nom);
extern VOID out_bitc (char *nom, SXBA chb);

SX_GLOBAL_VAR char P0 [] SX_INIT_VAL("NULL,");
