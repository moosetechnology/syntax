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


extern VOID out_bit(char *nom, SXBA chb);

extern VOID out_struct(char *nom, char *elt_m1);

extern VOID out_tab_int(char *nom, SXINT tab[], SXINT deb, SXINT fin, char *elt_m1);

extern VOID out_short(char *nom);

extern VOID out_int(char *nom, char *elt_m1);

extern VOID out_ext_int(char *nom);
extern VOID out_ext_int_newstyle(char *nom);

extern VOID out_char(char *nom, char *elt_m1);

extern VOID out_end_struct (void);
extern VOID out_rcvr_trans (void);
extern VOID out_ext_BOOLEAN (char *nom);
extern VOID out_ext_BOOLEAN_newstyle (char *nom);
extern VOID out_bitc (char *nom, SXBA chb);

SX_GLOBAL_VAR_TABLES char P0 [] SX_INIT_VAL_TABLES("NULL,");
