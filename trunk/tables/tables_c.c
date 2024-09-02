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

static char	ME [] = "tables_c";

#define SX_DFN_EXT_VAR_TABLES

#include "sxversion.h"
#include "sxunix.h"
#include "tables.h"
#include "sem_by.h"
#include "out.h"
#include "sxba.h"

char WHAT_TABLESC[] = "@(#)SYNTAX - $Id: tables_c.c 4210 2024-09-02 07:56:38Z garavel $" WHAT_DEBUG;

bool		is_lig;

static bool is_dynamic_parser,
               is_dynamic_scanner;

static char *tables_name = NULL; /* "sxtables" par defaut, sauf option -name */ 

extern bool	lecl_read (struct lecl_tables_s *lecl_tables_ptr, char *langname);
extern void	lecl_free (struct lecl_tables_s *lecl_tables_ptr);
extern bool	parser_read (struct parser_ag_item *parser_ag_ptr, char *langname);
extern void	parser_free (struct parser_ag_item *parser_ag_ptr);
extern bool	recor_read (struct R_tables_s *R_tables, char *langname);
extern void	recor_free (struct R_tables_s *R_tables);
extern bool	paradis_read (struct PP_ag_item *PP_ag, char *langname);
/* extern void	paradis_free () already declared in incl/util/PP_tables.h (struct PP_ag_item *paradis_ag_ptr) ; */ 
extern bool	semat_read (struct T_ag_item *T_ag, char *langname);
extern void	semat_free (struct T_ag_item *T_ag);
extern bool	init_tables (char *name);
extern SXINT   	max_P_tables (void);
extern SXINT    max_S_tables (void);
extern void     sxdp_free (void);
extern bool	out_lig_tables (char *);
extern void     out_T_tables (void);
extern void     out_PP_tables (void);
extern void     out_P_tables (void);
extern bool  out_S_tables (void);
extern bool  ds_read (char *ds_read_language_name);
extern bool  dp_read (char *dp_read_language_name);
extern void     sxre_free (void);



#define out_header(nom)		out_name_date (nom, "Beginning")
#define out_trailer(nom)	out_name_date (nom, "End")


static void	out_name_date (char *nom, char *lib)
{
    printf ("/* %s of sxtables for %s */\n", lib, nom);
}

static void   out_licence (void)
{
    printf ("%s\n", SXCOPYRIGHT);
}

static void
out_SXP_MAX (SXINT max)
{
    printf ("#define SXP_MAX %ld\n", (SXINT) max);
}

static void
out_SXS_MAX (void)
{
    puts ("#define SXS_MAX (SHRT_MAX+1)\n");
}





#define out_sxunix()	puts ("#include \"sxunix.h\"\n#include \"sxba.h\"")


static void	out_sxtables (void)
{
    if (SC.S_is_non_deterministic) {
       puts ("extern SXSCANNER_FUNCTION sxndscanner;");
    } else {
       /* rien : sxscanner() est declaree dans sxunix.h */
    }

    if (PC.nd_degree >= 0) {
       puts ("extern SXPARSER_FUNCTION sxndparser;");
    } else {
       /* rien, car sxparser() est declare dans "sxunix.h" */
    }

    if (PC.nd_degree >= 0) {
      puts ("#ifdef ESAMBIG");
      out_extern_int ("ESAMBIG(SXINT what)");
      puts ("#endif /* ESAMBIG */");
    }

    switch (PC.sem_kind) {
    case sem_by_abstract_tree:
	puts ("extern SXSEMACT_FUNCTION sxatc;");
	break;

    case sem_by_paradis:
	puts ("extern SXSEMACT_FUNCTION sxatcpp;");
	break;

    default:
      /* CAS SPECIAL 'SEMACT' */
      puts ("#ifdef SEMACT");
      /* pour supprimer un warning eventuel de GCC */
      puts ("#pragma GCC diagnostic push");
      puts ("#pragma GCC diagnostic ignored \"-Wredundant-decls\"");
      puts ("extern SXSEMACT_FUNCTION SEMACT;");
      puts ("#pragma GCC diagnostic pop");
      puts ("#endif");
    }

    printf ("\nSXTABLES sxtables={\n");
    printf ("%lu, /* magic */\n", SXMAGIC_NUMBER);
    printf ("sx%sscanner,\n", SC.S_is_non_deterministic ? "nd" : "");
    printf ("sx%sparser,\n", PC.nd_degree >= 0 ? "nd" : "");
    printf ("{%ld, %ld, %ld, ", (SXINT) SC.S_last_char_code < 255 ? SC.S_last_char_code : 255, (SXINT) SC.S_termax, (SXINT) SC.S_constructor_kind);
    printf ("%ld, %ld, ", (SXINT) RC.S_nbcart, (SXINT) RC.S_nmax);
    printf ("%ld, %ld, %ld, ", (SXINT) SC.S_last_simple_class_no, (SXINT) SC.S_counters_size, (SXINT) SC.S_last_state_no);
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
    if (SC.S_is_user_action_or_prdct) {
      puts ("#ifdef SCANACT");
      puts ("SCANACT,");
      puts ("#else");
      printf ("(SXSCANACT_FUNCTION *) ");
      puts (P0);
      puts ("#endif");
    }
    else {
      printf ("(SXSCANACT_FUNCTION *) ");
      puts (P0);
    }
    printf ("sx%ss%srecovery,\n", SC.S_is_non_deterministic ? "nd" : "", RC.S_nbcart == 0 ? "s" : "");
    if (SC.S_check_kw_lgth == 0) {
      printf ("(SXCHECK_KEYWORD_FUNCTION *) ");
      puts (P0_WITHOUT_COMMA);
    } else {
      puts ("sxcheck_keyword");
    }
    puts ("},");
    printf ("{%ld, %ld, %ld, %ld, ", (SXINT) -PC.mMrednt, (SXINT) -PC.mMred, (SXINT) -PC.mMprdct, (SXINT) PC.Mfe);
    printf ("%ld, %ld, %ld, %ld, ", (SXINT) PC.M0ref, (SXINT) PC.Mref, (SXINT) PC.deltavec + 1, (SXINT) PC.deltavec + PC.Mvec);
    printf ("%ld, %ld, %ld, %ld, ", (SXINT) PC.tmax, (SXINT) PC.ntmax, (SXINT) PC.nbpro, (SXINT) PC.xnbpro);
    printf ("%ld, %ld, %ld, ", (SXINT) PC.init_state, (SXINT) PC.final_state, (SXINT) PC.Mprdct_list);
    printf ("%ld, %ld, %ld, %ld, %ld, ", (SXINT) RC.P_nbcart, (SXINT) RC.P_nmax, (SXINT) RC.P_maxlregle, (SXINT) RC.P_validation_length, (SXINT) RC.P_followers_number);
    printf ("%ld, %ld, %ld,\n", (SXINT) RC.P_sizeofpts, (SXINT) RC.P_max_prio_X, (SXINT) RC.P_min_prio_0);
    puts (PR == NULL ? P0 : "reductions,");
    puts (PT == NULL ? P0 : "t_bases,");
    puts (PNT == NULL ? P0 : "nt_bases,");

    if (PV != NULL)
	printf ("#ifdef __INTEL_COMPILER\n#pragma warning(push ; disable:170)\n#endif\nvector-%ld,\n#ifdef __INTEL_COMPILER\n#pragma warning(pop)\n#endif \n", (SXINT) PC.deltavec + 1);
    else
	puts (P0);



    puts ((R.PER_tset == NULL || sxba_scan (R.PER_tset, 0) <= 0) ? P0 : "SXPBM_trans,");


    if (PPR != NULL)
	printf ("#ifdef __INTEL_COMPILER\n#pragma warning(push ; disable:170)\n#endif\nprdcts-%ld,\n#ifdef __INTEL_COMPILER\n#pragma warning(pop)\n#endif \n", (SXINT) -PC.mMred + 1);
    else
	puts (P0);

    if (PGA != NULL)
	printf ("#ifdef __INTEL_COMPILER\n#pragma warning(push ; disable:170)\n#endif\ngerme_to_gr_act-%ld,\n#ifdef __INTEL_COMPILER\n#pragma warning(pop)\n#endif \n", (SXINT) PC.mgerme + 1);
    else
	puts (P0);

    puts ((PC.Mprdct_list <= 0) ? P0 : "prdct_list-1,");
    puts (RC.P_nbcart == 0 ? P0 : "P_lregle,");
    puts (RC.P_nbcart == 0 ? P0 : "P_right_ctxt_head,");
    puts ("SXP_local_mess,");
    puts (RC.P_nbcart == 0 ? P0 : "P_no_delete,");
    puts (RC.P_nbcart == 0 ? P0 : "P_no_insert,");
    printf ("P_global_mess,PER_tset,\n");
    printf ("sx%sscan_it,\n", SC.S_is_non_deterministic ? "nd" : "");
    printf ("sx%sp%srecovery,\n", PC.nd_degree >= 0 ? "nd" : "", RC.P_nbcart == 0 ? "s" : "");

    /* for the next line, the macro 'SXPARSACT_FUNCTION' denotes the type of the function PARSACT() */
    if (PPR != NULL || PC.xnbpro > PC.nbpro /* P_is_user_action_or_predicate */) {
      puts ("#ifdef PARSACT");
      puts ("PARSACT,");
      puts ("#else");
      printf ("(SXPARSACT_FUNCTION *) ");
      puts (P0);
      puts ("#endif");
    }
    else {
      printf ("(SXPARSACT_FUNCTION *) ");
      puts (P0);    
    }

    if (PC.nd_degree >= 0) {
      puts ("#ifdef ESAMBIG");
      puts ("ESAMBIG,");
      puts ("#else /* ESAMBIG */");
      printf ("(SXDESAMBIG_FUNCTION *) ");
      puts (P0);
      puts ("#endif /* ESAMBIG */");
    }
    else {
      printf ("(SXDESAMBIG_FUNCTION *) ");
      puts (P0);    
    }

    switch (PC.sem_kind) {
    case sem_by_abstract_tree:
      puts ("sxatc");
      break;
      
    case sem_by_paradis:
      puts ("sxatcpp");
      break;
      
    default:
      puts ("#ifdef SEMACT");
      puts ("SEMACT");
      puts ("#else");
      printf ("(SXSEMACT_FUNCTION *) ");
      puts (P0_WITHOUT_COMMA);
      puts ("#endif");
      break;
    }
    puts ("},");

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

	printf ("(sxsem_tables *) %s\n", sem);
    }

    puts (is_lig ? "&sxligparsact," : P0_WITHOUT_COMMA);

    out_end_struct ();
}



static bool	sem_read (char *name)
{
    switch (PC.sem_kind) {
    case no_sem:
    case sem_by_action:
	return true;

    case sem_by_abstract_tree:
	return semat_read (&T, name);

    case sem_by_paradis:
	return paradis_read (&PP, name);

    default:
	return false;
    }
}



static void	out_sem_tables (void)
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



static void	sem_free (void)
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
#if defined (__GNUC__) && (__GNUC__ - 0 >= 7)
	__attribute__ ((fallthrough));
#endif

    case DS:
	sem_free ();
#if defined (__GNUC__) && (__GNUC__ - 0 >= 7)
	__attribute__ ((fallthrough));
#endif

    case SEMANTICS:
	parser_free (&P);
#if defined (__GNUC__) && (__GNUC__ - 0 >= 7)
	__attribute__ ((fallthrough));
#endif

    case PARSER:
	recor_free (&R);
#if defined (__GNUC__) && (__GNUC__ - 0 >= 7)
	__attribute__ ((fallthrough));
#endif

    case RECOR:
	lecl_free (&S);
#if defined (__GNUC__) && (__GNUC__ - 0 >= 7)
	__attribute__ ((fallthrough));
#endif

    case SCANNER:
    case OTHER:
	break;
    }

    if (num != OTHER) {
	fprintf (sxstderr, "%s: cannot access %s tables.\n", ME, name [(SXINT) num]);
    }

    return (SXSEVERITIES - 1);
}



/*---------------*/
/*    options    */
/*---------------*/

static char	Usage [] = "\
Usage:\t%s [options] language_name\n\
options=\t-v, -verbose, -nv, -noverbose\n\
\t\t-name tables_name\n\
\t\t-dp, -dynamic_parser\n\
\t\t-ds, -dynamic_scanner\n";

#define OPTION(opt)	(1 << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB ((sizeof(option_tbl)/sizeof(*option_tbl))-1)

#define UNKNOWN_ARG 		0
#define VERBOSE 		1
#define DYNAMIC_PARSER		2
#define DYNAMIC_SCANNER		3
#define NAME		        4
#define LAST_OPTION		NAME
#define LANGUAGE_NAME 		(LAST_OPTION+1)


static char	*option_tbl [] = {"", "v", "verbose", "nv", "noverbose",
				      "dp", "dynamic_parser",
				      "ds", "dynamic_scanner",
                                      "name"};
static SXINT	option_kind [] = {UNKNOWN_ARG, VERBOSE, VERBOSE, -VERBOSE, -VERBOSE,
				      DYNAMIC_PARSER, DYNAMIC_PARSER,
				      DYNAMIC_SCANNER, DYNAMIC_SCANNER,
                                      NAME};



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

  sxopentty ();

  is_dynamic_parser = false;
  is_dynamic_scanner = false;
  is_lig = false;

  /* Decodage des options */

  for (argnum = 1; argnum < argc; argnum++) {
    switch (option_get_kind (name = argv [argnum])) {
    case VERBOSE:
      sxverbosep = true;
      break;

    case -VERBOSE:
      sxverbosep = false;
      break;

    case DYNAMIC_PARSER:
      is_dynamic_parser = true;
      break;

    case -DYNAMIC_PARSER:
      is_dynamic_parser = false;
      break;

    case DYNAMIC_SCANNER:
      is_dynamic_scanner = true;
      break;

    case -DYNAMIC_SCANNER:
      is_dynamic_scanner = false;
      break;

    case NAME:
	 ++ argnum;
	 name = argv [argnum];
	 if (option_get_kind (name) != LANGUAGE_NAME) {
	      fprintf (sxstderr, "%s: a variable name is mandatory after the \"-name\" option\n", ME);
	      sxexit (SXSEVERITIES);
         }
	 tables_name = name; /* ... = strdup (name) would be safer */
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
  sxexit (SXSEVERITIES);

 run:
  language_name = name;

  if (sxverbosep) {
    fprintf (sxtty, "%s\n", release_mess);
  }

  if (!lecl_read (&S, language_name))
    sxexit (gripe (SCANNER));

  if (!recor_read (&R, language_name))
    sxexit (gripe (RECOR));

  if (!parser_read (&P, language_name))
    sxexit (gripe (PARSER));

  if (!sem_read (language_name))
    sxexit (gripe (SEMANTICS));

  if (!init_tables (language_name))
    sxexit (gripe (OTHER));

  out_licence ();
  out_header (language_name);

  if (tables_name != NULL) {
    printf ("#define sxtables %s /* due to option \"-name\" of %s */\n", tables_name, ME);
  }

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
    sxexit (gripe (OTHER));
  }

  out_sem_tables ();

  if (is_lig)
    {
      if (!out_lig_tables (language_name))
	sxexit (SXSEVERITIES - 1);
    }

  out_sxtables ();

  if (SC.S_check_kw_lgth != 0)
    puts (SK);

  if (is_dynamic_scanner && !ds_read (language_name))
    sxexit (gripe (DS));

  if (is_dynamic_parser && !dp_read (language_name))
    sxexit (gripe (DP));

  out_trailer (language_name);

  if (is_dynamic_scanner)
    sxre_free ();

  if (is_dynamic_parser)
    sxdp_free ();

  sem_free ();
  parser_free (&P);
  recor_free (&R);
  lecl_free (&S);
  sxexit (0);			/* fin normale */

  return EXIT_SUCCESS; /* Jamais atteint !! pour les compilo susceptibles ... */
}
