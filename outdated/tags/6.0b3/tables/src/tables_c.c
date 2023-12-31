/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1987 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *  Produit de l'equipe Langages et Traducteurs.        *
   *                                                      *
   ******************************************************** */





/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030523 11:58 (phd):	Ajout de SX_DFN_EXT_VAR			*/
/************************************************************************/
/* 20030522 17:06 (phd):	Ajout de l'�l�ment -1 de SXS_local_mess	*/
/*	S_transition_matrix SXS_action_or_prdct_code S_global_mess	*/
/*	S_lregle reductions t_bases nt_bases SXPBM_trans P_lregle	*/
/*	S_adrp P_right_ctxt_head SXP_local_mess S_syno PP_tables	*/
/************************************************************************/
/* 17 Oct 1996 14:07 (pb):	is_lig ds le cas EARLEY/LC		*/
/************************************************************************/
/* 17-08-94 11:58 (pb):		Sortie des actions et predicats	des LIGs*/
/************************************************************************/
/* 01-06-94 15:20 (pb):		Sortie de "dynamic_scanner"             */
/************************************************************************/
/* 02-03-94 10:34 (pb):		Sortie de "dynamic_parser"             	*/
/************************************************************************/
/* 12-05-93 17:10 (pb):		Sortie de S_last_state_no		*/
/************************************************************************/
/* 06-04-93 11:00 (pb):		Sortie de xnbpro			*/
/************************************************************************/
/* 31-03-93 13:58 (pb):		Sortie de prdct_list			*/
/************************************************************************/
/* 01-03-93 14:13 (pb):		Test de nd_degree (si >= 0 => nd)	*/
/************************************************************************/
/* 06-04-89 11:00 (pb):		Suppression de gotos			*/
/*				Ajout de final_state			*/
/*				Ajout de PER_trans_sets			*/
/************************************************************************/
/* 06-04-89 11:00 (pb):		Sortie de germe_to_gr_act		*/
/************************************************************************/
/* 02-06-88 15:24 (pb):		BITSTR => SXBA				*/
/************************************************************************/
/* 02-05-88 13:55 (pb):		Nouveau check_keyword			*/
/************************************************************************/
/* 14-03-88 08:43 (pb):		Suppression de #include <sys/timeb.h>	*/
/************************************************************************/
/* 11-03-88 14:40 (pb):		#include <sys/timeb.h>			*/
/************************************************************************/
/* 02-03-88 18:10 (pb):		Adaptation aux XNT			*/
/************************************************************************/
/* 21-09-87 09:14 (pb):		Suppression de <time.h> pour VAX	*/
/************************************************************************/
/* 31-07-87 16:22 (phd):	Utilisation de "sxopentty"		*/
/************************************************************************/
/* 31-07-87 16:22 (phd):	Ajout de cette rubrique et de "what"	*/
/************************************************************************/

static char	ME [] = "tables_c";

#define SX_DFN_EXT_VAR
#define SX_DFN_EXT_VAR_TABLES

#include "sxunix.h"
#include "release.h"
#include "tables.h"
#include "sem_by.h"
#include "out.h"
#include "sxba.h"
char WHAT_TABLESC[] = "@(#)SYNTAX - $Id: tables_c.c 1209 2008-03-12 15:58:48Z rlacroix $" WHAT_DEBUG;

#ifndef SX_GLOBAL_VAR
/* declarations indispensables pour Syntax V3.8 et interdites pour Syntax V6 */ 
FILE *sxstdout, *sxstderr;
#endif

char	*language_name;

BOOLEAN		is_lig;

static BOOLEAN is_dynamic_parser,
               is_dynamic_scanner;

/* extern long	time (); */
extern BOOLEAN	lecl_read (struct lecl_tables_s *lecl_tables_ptr, char *langname);
extern VOID	lecl_free (struct lecl_tables_s *lecl_tables_ptr);
extern BOOLEAN	parser_read (struct parser_ag_item *parser_ag_ptr, char *langname);
extern VOID	parser_free (struct parser_ag_item *parser_ag_ptr);
extern BOOLEAN	recor_read (struct R_tables_s *R_tables, char *langname);
extern VOID	recor_free (struct R_tables_s *R_tables);
extern BOOLEAN	paradis_read (struct PP_ag_item *PP_ag, char *langname);
/* extern VOID	paradis_free () already declared in etc/incl/PP_tables.h (struct PP_ag_item *paradis_ag_ptr) ; */ 
extern BOOLEAN	semat_read (struct T_ag_item *T_ag, char *langname);
extern VOID	semat_free (struct T_ag_item *T_ag);
extern BOOLEAN	init_tables (char *name);
extern SXINT   	max_P_tables (void);
extern SXINT    max_S_tables (void);
extern VOID     sxdp_free (void);
extern BOOLEAN	out_lig_tables (char *);
extern VOID     out_T_tables (void);
extern VOID     out_PP_tables (void);
extern VOID     out_P_tables (void);
extern BOOLEAN  out_S_tables (void);
extern BOOLEAN  ds_read (char *ds_read_language_name);
extern BOOLEAN  dp_read (char *dp_read_language_name);
extern VOID     sxre_free (void);



#define out_header(nom)		out_name_date (nom, "Beginning")
#define out_trailer(nom)	out_name_date (nom, "End")


static VOID	out_name_date (char *nom, char *lib)
{
    long	lt;

    lt = time (0);
    /* the 25th character is a "\n" */
    printf ("/* %s of sxtables for %s [%.*s] */\n", lib, nom, 24, ctime (&lt));
}

static void
out_SXP_MAX (SXINT max)
{
    printf ("#define SXP_MAX %ld\n", (long)max);
}

static void
out_SXS_MAX (void)
{
    puts ("#define SXS_MAX (SHRT_MAX+1)\n");
}





#define out_sxunix()	puts ("#include \"sxunix.h\"\n#include \"sxba.h\"")


static VOID	out_sxtables (void)
{
    out_ext_int_newstyle (SC.S_is_non_deterministic ? "sxndscanner(SXINT what_to_do, struct sxtables *arg)" : "sxscanner(SXINT what_to_do, struct sxtables *arg)");
    out_ext_int_newstyle (PC.nd_degree >= 0 ? "sxndparser (SXINT what_to_do, struct sxtables *arg)" : "sxparser(SXINT what_to_do, struct sxtables *arg)");

    if (PC.nd_degree >= 0)
	out_ext_int_newstyle ("ESAMBIG(SXINT what)");

    switch (PC.sem_kind) {
    case sem_by_abstract_tree:
	out_ext_int_newstyle ("sxatc(SXINT what, ...)");
	break;

    case sem_by_paradis:
	out_ext_int_newstyle ("sxatcpp(SXINT what, ...)");
	break;

    default:
	out_ext_int_newstyle ("SEMACT(SXINT what, struct sxtables *arg)");
    }

    puts ("\nstruct sxtables sxtables={");
    printf ("%lu, /* magic */\n", SXMAGIC_NUMBER);
    /* for the next line, the macro 'PARSER_T' denotes the type for the function sxparser() */
    printf ("{sx%sscanner,(PARSER_T) sx%sparser}, ", SC.S_is_non_deterministic ? "nd" : "", PC.nd_degree >= 0 ? "nd" : "");
    printf ("{%ld, %ld, %ld, ", (long)SC.S_last_char_code < 255 ? SC.S_last_char_code : 255, (long)SC.S_termax, (long)SC.S_constructor_kind);
    printf ("%ld, %ld, ", (long)RC.S_nbcart, (long)RC.S_nmax);
    printf ("%ld, %ld, %ld, ", (long)SC.S_last_simple_class_no, (long)SC.S_counters_size, (long)SC.S_last_state_no);
    printf ("%d, %d, %d, ", SC.S_is_user_action_or_prdct, SC.S_are_comments_erased, SC.S_is_non_deterministic);
    puts ("\nS_is_a_keyword,S_is_a_generic_terminal,S_transition_matrix,");
    puts (SC.S_xprod != 0 ? "SXS_action_or_prdct_code," : P0);
    puts ("S_adrp,");
    puts (is_syno ? "S_syno," : P0);
    puts (RC.S_nbcart == 0 ? P0 : "SXS_local_mess,");
    puts ("S_char_to_simple_class+128,");
    puts (RC.S_nbcart == 0 ? P0 : "S_no_delete,");
    puts (RC.S_nbcart == 0 ? P0 : "S_no_insert,");
    puts ("S_global_mess,");
    puts (RC.S_nbcart == 0 ? P0 : "S_lregle,");
    puts (SC.S_is_user_action_or_prdct == 0 ? P0 : "(SCANACT_T) SCANACT,");
    /* for the next line, the macro 'RECOVERY_T' denotes the type for the function recovery() */
    printf ("(RECOVERY_T) sx%ss%srecovery,\n", SC.S_is_non_deterministic ? "nd" : "", RC.S_nbcart == 0 ? "s" : "");
    puts (SC.S_check_kw_lgth == 0 ? P0 : "check_keyword,");
    puts ("},");
    printf ("{%ld, %ld, %ld, %ld, ", (long)-PC.mMrednt, (long)-PC.mMred, (long)-PC.mMprdct, (long)PC.Mfe);
    printf ("%ld, %ld, %ld, %ld, ", (long)PC.M0ref, (long)PC.Mref, (long)PC.deltavec + 1, (long)PC.deltavec + PC.Mvec);
    printf ("%ld, %ld, %ld, %ld, ", (long)PC.tmax, (long)PC.ntmax, (long)PC.nbpro, (long)PC.xnbpro);
    printf ("%ld, %ld, %ld, ", (long)PC.init_state, (long)PC.final_state, (long)PC.Mprdct_list);
    printf ("%ld, %ld, %ld, %ld, %ld, ", (long)RC.P_nbcart, (long)RC.P_nmax, (long)RC.P_maxlregle, (long)RC.P_validation_length, (long)RC.P_followers_number);
    printf ("%ld, %ld, %ld,\n", (long)RC.P_sizeofpts, (long)RC.P_max_prio_X, (long)RC.P_min_prio_0);
    puts (PR == NULL ? P0 : "reductions,");
    puts (PT == NULL ? P0 : "t_bases,");
    puts (PNT == NULL ? P0 : "nt_bases,");

    if (PV != NULL)
	printf ("vector-%ld,\n", (long)PC.deltavec + 1);
    else
	puts (P0);

#if 0
    if (PGO != NULL)
	printf ("gotos-%d,\n", -PC.mMprdct + 1);
    else
	puts (P0);
#endif

#if 0
    printf ("#if SXBITS_PER_LONG == %d\n", SXBITS_PER_LONG);
#endif

    puts ((R.PER_tset == NULL || sxba_scan (R.PER_tset, 0) <= 0) ? P0 : "SXPBM_trans,");

#if 0
    puts ("#else");
    puts (P0);
    puts ("#endif");
#endif

    if (PPR != NULL)
	printf ("prdcts-%ld,\n", (long)-PC.mMred + 1);
    else
	puts (P0);

    if (PGA != NULL)
	printf ("germe_to_gr_act-%ld,\n", (long)PC.mgerme + 1);
    else
	puts (P0);

    puts ((PC.Mprdct_list <= 0) ? P0 : "prdct_list-1,");
    puts (RC.P_nbcart == 0 ? P0 : "P_lregle,");
    puts (RC.P_nbcart == 0 ? P0 : "P_right_ctxt_head,");
    puts ("SXP_local_mess,");
    puts (RC.P_nbcart == 0 ? P0 : "P_no_delete,");
    puts (RC.P_nbcart == 0 ? P0 : "P_no_insert,");
    printf ("P_global_mess,PER_tset,sx%sscan_it,", SC.S_is_non_deterministic ? "nd" : "");
    /* for the next line, the macro 'RECOVERY_T' denotes the type for the function recovery() */
    printf ("(RECOVERY_T) sx%sp%srecovery,\n", PC.nd_degree >= 0 ? "nd" : "", RC.P_nbcart == 0 ? "s" : "");

    /* for the next line, the macro 'PARSER_T' denotes the type for the function PARSACT() */
    puts (PPR != NULL || PC.xnbpro > PC.nbpro /* P_is_user_action_or_predicate */  ? "(PARSER_T) PARSACT," : P0);

    puts (PC.nd_degree >= 0 ? "ESAMBIG," : P0);

    {
	char	*act;

	switch (PC.sem_kind) {
	case sem_by_abstract_tree:
	    act = "sxatc";
	    break;

	case sem_by_paradis:
	    act = "sxatcpp";
	    break;

	default:
	    act = "SEMACT";
	}

	printf ("(SEMACT_T) %s},\n", act);
    }

    puts ("err_titles,");
    puts (RC.E_labstract == 0 ? P0 : "abstract,");

    {
	char	*sem;

	switch (PC.sem_kind) {
	case sem_by_abstract_tree:
	    sem = "&SXT_tables,";
	    break;

	case sem_by_paradis:
	    sem = "PP_tables,";
	    break;

	default:
	    sem = P0;
	}

	printf ("(sxsem_tables*)%s\n", sem);
    }

    puts (is_lig ? "&sxligparsact," : P0);

    out_end_struct ();
}



static BOOLEAN	sem_read (char *name)
{
    switch (PC.sem_kind) {
    case no_sem:
    case sem_by_action:
	return TRUE;

    case sem_by_abstract_tree:
	return semat_read (&T, name);

    case sem_by_paradis:
	return paradis_read (&PP, name);

    default:
	return FALSE;
    }
}



static VOID	out_sem_tables (void)
{
    switch (PC.sem_kind) {
    case no_sem:
    case sem_by_action:
	break;

    case sem_by_abstract_tree:
	out_T_tables ();
	break;

    case sem_by_paradis:
	out_PP_tables ();
	break;

    default:
	break;
    }
}



static VOID	sem_free (void)
{
    switch (PC.sem_kind) {
    case no_sem:
    case sem_by_action:
	break;

    case sem_by_abstract_tree:
	semat_free (&T);
	break;

    case sem_by_paradis:
	paradis_free (&PP);
	break;

    default:
	break;
    }
}



enum ERROR_IN {SCANNER, RECOR, PARSER, SEMANTICS, OTHER, DP, DS};



static int	gripe (enum ERROR_IN num)
{
    static char		*name [] = {"scanner", "recor", "parser", "semantic", "dynamic_parser",
					"dynamic_scanner"};

    switch (num) {
    case DP:

    default:
	sxre_free ();

    case DS:
	sem_free ();

    case SEMANTICS:
	parser_free (&P);

    case PARSER:
	recor_free (&R);

    case RECOR:
	lecl_free (&S);

    case SCANNER:
    case OTHER:
	break;
    }

    if (num != OTHER) {
	fprintf (sxstderr, "%s: cannot access %s tables.\n", ME, name [(SXINT) num]);
    }

    return (SEVERITIES - 1);
}



/*---------------*/
/*    options    */
/*---------------*/

static char	Usage [] = "\
Usage:\t%s [options] language_name\n\
options=\t-v, -verbose, -nv, -noverbose,\n\
\t\t-dp, -dynamic_parser\n\
\t\t-ds, -dynamic_scanner\n";

#define OPTION(opt)	(1 << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB ((sizeof(option_tbl)/sizeof(*option_tbl))-1)

#define UNKNOWN_ARG 		0
#define VERBOSE 		1
#define DYNAMIC_PARSER		2
#define DYNAMIC_SCANNER		3
#define LAST_OPTION		DYNAMIC_SCANNER
#define LANGUAGE_NAME 		LAST_OPTION+1


static char	*option_tbl [] = {"", "v", "verbose", "nv", "noverbose",
				      "dp", "dynamic_parser",
				      "ds", "dynamic_scanner",};
static SXINT	option_kind [] = {UNKNOWN_ARG, VERBOSE, VERBOSE, -VERBOSE, -VERBOSE,
				      DYNAMIC_PARSER, DYNAMIC_PARSER,
				      DYNAMIC_SCANNER, DYNAMIC_SCANNER,};



static SXINT	option_get_kind (char *arg)
{
    char	**opt;

    if (*arg++ != '-')
	return LANGUAGE_NAME;

    for (opt = &(option_tbl [OPT_NB]); opt > option_tbl; opt--) {
	if (strcmp (*opt, arg) == 0 /* egalite */ )
	    break;
    }

    return option_kind [opt - option_tbl];
}



/************************************************************************/
/* main function */
/************************************************************************/
int main (int argc, char *argv[])
{
  int		argnum;
  char          *name;

  sxinitialise(name); /* pour faire taire gcc -Wuninitialized */
  /* valeurs par defaut */

  sxstdout = stdout;
  sxstderr = stderr;

  sxverbosep = TRUE;
  is_dynamic_parser = FALSE;
  is_dynamic_scanner = FALSE;
  is_lig = FALSE;

  /* Decodage des options */

  for (argnum = 1; argnum < argc; argnum++) {
    switch (option_get_kind (name = argv [argnum])) {
    case VERBOSE:
      sxverbosep = TRUE;
      break;

    case -VERBOSE:
      sxverbosep = FALSE;
      break;

    case DYNAMIC_PARSER:
      is_dynamic_parser = TRUE;
      break;

    case -DYNAMIC_PARSER:
      is_dynamic_parser = FALSE;
      break;

    case DYNAMIC_SCANNER:
      is_dynamic_scanner = TRUE;
      break;

    case -DYNAMIC_SCANNER:
      is_dynamic_scanner = FALSE;
      break;

    case LANGUAGE_NAME:
      /* nom du langage */
      goto run;

    case UNKNOWN_ARG:
      fprintf (sxstderr, "%s: unknown option \"%s\" (ignored).\n", ME, name);
      break;
    default:
#if EBUG
      sxtrap(ME,"unknown switch case #1");
#endif
      break;
    }
  }

  fprintf (sxstderr, Usage, ME);
  SXEXIT (SEVERITIES);

 run:
  language_name = name;
  sxopentty ();

  if (sxverbosep) {
    fprintf (sxtty, "%s\n", release_mess);
  }

  if (!lecl_read (&S, language_name))
    SXEXIT (gripe (SCANNER));

  if (!recor_read (&R, language_name))
    SXEXIT (gripe (RECOR));

  if (!parser_read (&P, language_name))
    SXEXIT (gripe (PARSER));

  if (!sem_read (language_name))
    SXEXIT (gripe (SEMANTICS));

  if (!init_tables (language_name))
    SXEXIT (gripe (OTHER));

  out_header (language_name);
  out_SXP_MAX (max_P_tables ());

  if (max_S_tables () >= 0x3ff)
    out_SXS_MAX ();

  out_sxunix ();

  /* Cas des LIGs traitees avec le constructeur EARLEY/LC ! */
  /* Dans le cas GLR, is_lig sera repositionne par out_P_tables() */
  /* C,a semble tres etrange, de toutes les facons, ds la version 6, y-a pas de lig!!
     is_lig = PC.nd_degree == 1;
  */

  out_P_tables ();

  if (!out_S_tables ()) {
    SXEXIT (gripe (OTHER));
  }

  out_sem_tables ();

  if (is_lig)
    {
      if (!out_lig_tables (language_name))
	SXEXIT (SEVERITIES - 1);
    }

  out_sxtables ();

  if (SC.S_check_kw_lgth != 0)
    puts (SK);

  if (is_dynamic_scanner && !ds_read (language_name))
    SXEXIT (gripe (DS));

  if (is_dynamic_parser && !dp_read (language_name))
    SXEXIT (gripe (DP));

  out_trailer (language_name);

  if (is_dynamic_scanner)
    sxre_free ();

  if (is_dynamic_parser)
    sxdp_free ();

  sem_free ();
  parser_free (&P);
  recor_free (&R);
  lecl_free (&S);
  SXEXIT (0);			/* fin normale */

  return EXIT_SUCCESS; /* Jamais atteint !! pour les compilo susceptibles ... */
}
