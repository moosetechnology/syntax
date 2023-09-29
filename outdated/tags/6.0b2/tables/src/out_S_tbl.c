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
/* 20040211 16:42 (phd):	Modif pour calmer le compilo HPUX	*/
/************************************************************************/
/* 20030522 17:06 (phd):	Ajout de l'élément -1 de SXS_transition	*/
/*	S_transition_matrix SXS_action_or_prdct_code S_global_mess	*/
/*	S_adrp SXS_local_mess S_lregle S_syno				*/
/************************************************************************/
/* 20030520 14:25 (phd):	Correction d'un format. Merci SGI	*/
/************************************************************************/

/* Le 24/01/2003
   Pour avoir les bonnes macros pour STMT, KEEP, SCAN, CODOP, NEXT ... */
#define SXS_MAX (SHRT_MAX+1)
#include "sxunix.h"
#include "tables.h"
#include "out.h"
#include "sxsstmt.h"
char WHAT_TABLESOUTSTBL[] = "@(#)SYNTAX - $Id: out_S_tbl.c 625 2007-06-12 08:35:56Z syntax $" WHAT_DEBUG;



static int	max; /* Positionne par l'appel a max_S_tables() */

#define TROUVE_MAX(x)	((x>max)?max=x:0)


/* uint est un "stmt" unsigned int qui tient sur un unsigned short */
static unsigned short
uint2ushort (stmt)
     unsigned int stmt;
{
  unsigned short uint2ushort_ushort;

  uint2ushort_ushort = (stmt & 0x3ff) | (stmt >> 16);

  return uint2ushort_ushort;
}

static VOID	out_S_char_to_simple_class ()
{
    register int	j, i;
    register int	d, nbt;
    unsigned char	sctsc [128 + 1 + 255] /* -128:255 */ ;

    if (SC.S_last_char_code > 255) {
	SC.S_last_char_code = 255;
    }

    for (i = SC.S_last_char_code; i >= 0; i--)
	(sctsc + 128) [i] = S.S_char_to_simple_class [i];

    for (i = SC.S_last_char_code + 1; i <= 255; i++)
	(sctsc + 128) [i] = 1 /* Class of Illegal Char */ ;

    for (i = 128; i <= 255; i++)
	(sctsc - 128) [i] = (sctsc + 128) [i];

    puts ("\nstatic unsigned char S_char_to_simple_class[]={");
    nbt = (sizeof (sctsc) / sizeof (unsigned char)) / slice_length;
    d = 0;

    for (i = 1; i <= nbt; i++) {
	for (j = d, d += slice_length; j < d; j++) {
	    printf ("%d,", sctsc [j]);
	}

	putchar (NEWLINE);
    }

    for (j = d; (unsigned)j < (sizeof (sctsc) / sizeof (unsigned char)); j++) {
	printf ("%d,", sctsc [j]);
    }

    out_end_struct ();
}



static VOID	out_transition ()
{
  register int	j, i, k;
  register int	d, nbt;
  unsigned int stmt;

  puts ("static SXSTMI SXS_transition[]={0,");
  nbt = SC.S_last_simple_class_no / slice_length2;

  for (k = 1; k <= SC.S_last_state_no; k++) {
    d = 1;

    for (i = 1; i <= nbt; i++) {
      for (j = d, d += slice_length2; j < d; j++) {
	stmt = SM [k] [j];

	if (max <= 0x3ff)
	  stmt = uint2ushort (stmt);

	printf ("0x%x,", stmt);
      }

      putchar (NEWLINE);
    }

    for (j = d; j <= SC.S_last_simple_class_no; j++) {
      stmt = SM [k] [j];

      if (max <= 0x3ff)
	stmt = uint2ushort (stmt);

	  printf ("0x%x,", stmt);
    }

    putchar (NEWLINE);
  }

  out_end_struct ();
  printf ("static SXSTMI *S_transition_matrix[]={%s\n", P0);

  for (i = 0; i < SC.S_last_state_no; i++) {
    printf ("&SXS_transition[%d],\n", i * SC.S_last_simple_class_no);
  }

  out_end_struct ();
}



static VOID	out_S_action_or_prdct_code ()
{
  register int	i;
  unsigned int stmt;

  out_struct ("SXS_action_or_prdct_code", "{0,0,0,0,0},");

  for (i = 1; i <= SC.S_xprod; i++) {
    stmt = SP [i].stmt;

    if (max <= 0x3ff)
      stmt = uint2ushort (stmt);

	printf ("{0x%x,%d,%d,%d,%d},\n", stmt, SP [i].action_or_prdct_no, SP [i].param,
		SP [i].is_system, SP [i].kind);
  }

  out_end_struct ();
}



static VOID	out_S_adrp ()
{
    register char	*string;
    register int	i, j, d, nbt;

    is_syno = FALSE;
    out_char ("*S_adrp", "0,");

    for (i = 1; i <= SC.S_last_static_ste; i++) {
	if (SA [i].syno_lnk != 0)
	    is_syno = TRUE;

	fputs ("\"", stdout);

	for (j = SA [i].lgth, string = S.S_terlis + SA [i].xterlis; j != 0; j--) {
	    fputs (SXCHAR_TO_STRING (*string++), stdout);
	}

	puts ("\",");
    }

    out_end_struct ();

    if (is_syno) {
	out_int ("S_syno", "0,");
	nbt = SC.S_last_static_ste / slice_length;
	d = 1;

	for (i = 1; i <= nbt; i++) {
	    for (j = d, d += slice_length; j < d; j++) {
		printf ("%d,", SA [j].syno_lnk);
	    }

	    putchar (NEWLINE);
	}

	for (j = d; j <= SC.S_last_static_ste; j++) {
	    printf ("%d,", SA [j].syno_lnk);
	}

	out_end_struct ();
    }
}



static VOID	out_S_lregle ()
{
    register int	j, i;

    out_int ("S_lrgl", "");

    for (i = 1; i <= RC.S_nbcart; i++) {
	for (j = 0; j <= RC.S_maxlregle; j++) {
	    printf ("%d,", R.S_lregle [i] [j]);
	}

	putchar (NEWLINE);
    }

    out_end_struct ();
    out_int ("*S_lregle", P0);

    for (i = 0; i < RC.S_nbcart; i++) {
	printf ("&S_lrgl[%d],\n", i * (RC.S_maxlregle + 1));
    }

    out_end_struct ();
}



static VOID	out_S_global_mess ()
{
    register int	i, lg;
    register char	*m;

    out_char ("*S_global_mess", "0,");

    for (i = 1; i <= 5; i++) {
	switch (i) {
	default:
	    m = "";
	    break;

	case 1:
	case 2:
	case 5:
	    m = "%s";
	}

	if ((lg = SG [i].length) != 0) {
	    printf ("\"%s%.*s\",\n", m, lg, R.S_string_mess + SG [i].index);
	}
	else
	    printf ("\"%s\",\n", m);
    }

    out_end_struct ();
}



static VOID	out_S_local_mess ()
{
    register int	j, i, param, lg;

    out_int ("S_param_ref", "");

    for (i = 1; i <= RC.S_nbcart; i++) {
	for (j = 1; j <= SL [i].param_no; j++) {
	    printf ("%d,", SL [i].string_ref [j].param);
	}

	putchar (NEWLINE);
    }

    out_end_struct ();
    out_struct ("SXS_local_mess", "{0, NULL, 0},");

    for (param = 0, i = 1; i <= RC.S_nbcart; param += SL [i++].param_no) {
	printf ("{%d,\"", SL [i].param_no);

	for (j = 0; j <= SL [i].param_no; j++) {
	    if ((lg = SL [i].string_ref [j].length) != 0) {
		printf ("%%s%.*s", lg, R.S_string_mess + SL [i].string_ref [j].index);
	    }
	    else
		fputs ("%s", stdout);
	}

	printf ("\",&S_param_ref[%d]},\n", param);
    }

    out_end_struct ();
}



BOOLEAN		out_S_tables ()
{
    if (sxverbosep)
	fputs ("\tScanner Tables\n", sxtty);

    out_S_char_to_simple_class ();
    out_transition ();

    if (SC.S_xprod != 0)
	out_S_action_or_prdct_code ();

    out_S_adrp ();
    out_bit ("S_is_a_keyword", S.S_is_a_keyword);
    out_bit ("S_is_a_generic_terminal", S.S_is_a_generic_terminal);

    if (RC.S_nbcart != 0) {
	out_S_lregle ();
	out_S_local_mess ();
	out_bitc ("S_no_delete", R.S_no_delete);
	out_bitc ("S_no_insert", R.S_no_insert);
    }

    out_S_global_mess ();

    if (SC.S_is_user_action_or_prdct != 0)
	out_ext_int ("SCANACT");

    if (SC.S_is_non_deterministic) {
	out_ext_int ("sxndscan_it");
	out_ext_BOOLEAN (RC.S_nbcart == 0 ? "sxndssrecovery" : "sxndsrecovery");
    }
    else {
	out_ext_int ("sxscan_it");
	out_ext_BOOLEAN (RC.S_nbcart == 0 ? "sxssrecovery" : "sxsrecovery");
    }

    if (SC.S_check_kw_lgth != 0)
	puts ("static int check_keyword();");

    return TRUE;
}

int max_S_tables ()
{
  /* retourne le max des valeurs utilisees ds les tables du scanner */
  int k, i;
  int next;

  for (k = 1; k <= SC.S_last_state_no; k++) {
    for (i = 1; i <= SC.S_last_simple_class_no; i++) {
      next = NEXT (SM [k] [i]);
      TROUVE_MAX(next);
    }
  }

  for (i = 1; i <= SC.S_xprod; i++) {
    next = NEXT (SP [i].stmt);
    TROUVE_MAX(next);
  }

  return max;
}
