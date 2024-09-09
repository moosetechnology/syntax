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

/* Le 24/01/2003
   Pour avoir les bonnes macros pour STMT, KEEP, SCAN, CODOP, NEXT ... */
#define SXS_MAX (SHRT_MAX+1)

#include "sxversion.h"
#include "sxunix.h"
#include "tables.h"
#include "out.h"
#include "sxsstmt.h"

char WHAT_TABLESOUTSTBL[] = "@(#)SYNTAX - $Id: out_S_tbl.c 4257 2024-09-06 12:10:40Z garavel $" WHAT_DEBUG;


static SXINT	max; /* Positionne par l'appel a max_S_tables() */

#define TROUVE_MAX(x)	((x>max)?max=x:0)


/* uint est un "stmt" SXUINT qui tient sur un unsigned short */
static unsigned short
uint2ushort (SXUINT stmt)
{
  unsigned short uint2ushort_ushort;

  uint2ushort_ushort = (stmt & 0x3ff) | (stmt >> 16);

  return uint2ushort_ushort;
}

static void	out_S_char_to_simple_class (void)
{
    SXINT	j, i;
    SXINT	d, nbt;
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

	putchar (SXNEWLINE);
    }

    for (j = d; (SXUINT)j < (sizeof (sctsc) / sizeof (unsigned char)); j++) {
	printf ("%d,", sctsc [j]);
    }

    out_end_struct ();
}



static void	out_transition (void)
{
  SXINT	j, i, k;
  SXINT	d, nbt;
  SXUINT stmt;

  puts ("static SXSTMI SXS_transition[]={0,");
  nbt = SC.S_last_simple_class_no / slice_length2;

  for (k = 1; k <= SC.S_last_state_no; k++) {
    d = 1;

    for (i = 1; i <= nbt; i++) {
      for (j = d, d += slice_length2; j < d; j++) {
	stmt = SM [k] [j];

	if (max <= 0x3ff)
	  stmt = uint2ushort (stmt);

	printf ("0x%lx,", stmt);
      }

      putchar (SXNEWLINE);
    }

    for (j = d; j <= SC.S_last_simple_class_no; j++) {
      stmt = SM [k] [j];

      if (max <= 0x3ff)
	stmt = uint2ushort (stmt);

	  printf ("0x%lx,", stmt);
    }

    putchar (SXNEWLINE);
  }

  out_end_struct ();
  printf ("static SXSTMI *S_transition_matrix[]={%s\n", P0);

  for (i = 0; i < SC.S_last_state_no; i++) {
    printf ("&SXS_transition[%ld],\n", i * SC.S_last_simple_class_no);
  }

  out_end_struct ();
}



static void	out_S_action_or_prdct_code (void)
{
  SXINT	i;
  SXUINT stmt;

  out_struct ("SXS_action_or_prdct_code", "{0,0,0,0,0},");

  for (i = 1; i <= SC.S_xprod; i++) {
    stmt = SP [i].stmt;

    if (max <= 0x3ff)
      stmt = uint2ushort (stmt);

	printf ("{0x%lx,%ld,%ld,%d,%d},\n", stmt, SP [i].action_or_prdct_no, SP [i].param,
		SP [i].is_system, SP [i].kind);
  }

  out_end_struct ();
}



static void	out_S_adrp (void)
{
    char	*string;
    SXINT	i, j, d, nbt;

    is_syno = false;
    out_char ("*S_adrp", "0,");

    for (i = 1; i <= SC.S_last_static_ste; i++) {
	if (SA [i].syno_lnk != 0)
	    is_syno = true;

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
		printf ("%ld,", SA [j].syno_lnk);
	    }

	    putchar (SXNEWLINE);
	}

	for (j = d; j <= SC.S_last_static_ste; j++) {
	    printf ("%ld,", SA [j].syno_lnk);
	}

	out_end_struct ();
    }
}



static void	out_S_lregle (void)
{
    SXINT	j, i;

    out_int ("S_lrgl", "");

    for (i = 1; i <= RC.S_nbcart; i++) {
	for (j = 0; j <= RC.S_maxlregle; j++) {
	    printf ("%ld,", R.S_lregle [i] [j]);
	}

	putchar (SXNEWLINE);
    }

    out_end_struct ();
    out_int ("*S_lregle", P0);

    for (i = 0; i < RC.S_nbcart; i++) {
	printf ("&S_lrgl[%ld],\n", i * (RC.S_maxlregle + 1));
    }

    out_end_struct ();
}


static void	out_S_global_mess (void)
{
    SXINT	i, lg;
    char	*m;

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
	    /* LINTED this cast form long to int is needed by printf() */
	    printf ("\"%s%.*s\",\n", m, (int)lg, R.S_string_mess + SG [i].index);
	}
	else
	    printf ("\"%s\",\n", m);
    }

    out_end_struct ();
}



static void	out_S_local_mess (void)
{
    SXINT	j, i, param, lg;

    out_int ("S_param_ref", "");

    for (i = 1; i <= RC.S_nbcart; i++) {
	for (j = 1; j <= SL [i].param_no; j++) {
	    printf ("%ld,", SL [i].string_ref [j].param);
	}

	putchar (SXNEWLINE);
    }

    out_end_struct ();
    out_struct ("SXS_local_mess", "{0,NULL,NULL},");

    for (param = 0, i = 1; i <= RC.S_nbcart; param += SL [i++].param_no) {
	printf ("{%ld,\"", SL [i].param_no);

	for (j = 0; j <= SL [i].param_no; j++) {
	    if ((lg = SL [i].string_ref [j].length) != 0) {
                /* LINTED this cast form long to int is needed by printf() */
		printf ("%%s%.*s", (int)lg, R.S_string_mess + SL [i].string_ref [j].index);
	    }
	    else
		fputs ("%s", stdout);
	}

	printf ("\",&S_param_ref[%ld]},\n", param);
    }

    out_end_struct ();
}



bool		out_S_tables (void)
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

    if (SC.S_is_user_action_or_prdct != 0) {
      puts ("#ifdef SCANACT");
      puts ("#warning \"compiling with -DSCANACT is deprecated; define sxscanner_action() or invoke sxset_scanner_action()\"");
      puts ("#endif");
    }

    if (SC.S_is_non_deterministic) {
	puts ("extern SXSCANIT_FUNCTION sxndscan_it;");
        puts ("extern SXSRECOVERY_FUNCTION sxndsrecovery;");
    } else {
	/* rien : sxscan_it() est declaree dans sxunix.h */
        /* rien : sxsrecovery() est declaree dans sxunix.h */
    }

    if (SC.S_check_kw_lgth != 0)
	puts ("static SXCHECK_KEYWORD_FUNCTION sxcheck_keyword;");

    return true;
}

SXINT max_S_tables (void)
{
  /* retourne le max des valeurs utilisees ds les tables du scanner */
  SXINT k, i;
  SXINT next;

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
