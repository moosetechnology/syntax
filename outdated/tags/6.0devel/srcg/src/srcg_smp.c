/* ********************************************************************
   *  This program has been generated from srcg.at                    *
   *  on Thu Mar 13 14:44:30 2008                                     *
   *  by the SYNTAX (*) abstract tree constructor SEMAT               *
   ********************************************************************
   *  (*) SYNTAX is a trademark of INRIA.                             *
   ******************************************************************** */


static char	ME [] = "srcg_smp";

/*   I N C L U D E S   */
#define NODE struct srcg_node
#include "sxunix.h"
#include "srcg.h"
/* struct srcg_node est de'clare' ds "srcg.h" */

/*
  N O D E   N A M E S
*/
#define ERROR_n 1
#define ARGUMENT_S_n 2
#define CLAUSE_n 3
#define CLAUSE_S_n 4
#define PREDICATE_n 5
#define PREDICATE_S_n 6
#define STRING_S_n 7
#define VOID_n 8
#define external_predicate_n 9
#define internal_predicate_n 10
#define profile_n 11
#define terminal_n 12
#define variable_n 13
/*
  E N D   N O D E   N A M E S
*/

#include "varstr.h"

static VARSTR varstr_profile;
static SXINT  clause_nb;


static void
srcg_pi (void)
{

  /*
    I N H E R I T E D
  */

  switch (SXVISITED->father->name) {

  case ERROR_n :
    break;

  case ARGUMENT_S_n :/* SXVISITED->name = STRING_S_n */
    if (SXVISITED->position > 1)
      varstr_profile = varstr_catenate (varstr_profile, ", ");
    break;

  case CLAUSE_n :
    switch (SXVISITED->position) {
    case 1 :/* SXVISITED->name = PREDICATE_n */
      break;

    case 2 :/* SXVISITED->name = PREDICATE_S_n */
      break;

    }

    break;

  case CLAUSE_S_n :/* SXVISITED->name = CLAUSE_n */
    break;

  case PREDICATE_n :
    switch (SXVISITED->position) {
    case 1 :/* SXVISITED->name = {VOID_n, external_predicate_n, internal_predicate_n} */
      break;

    case 2 :/* SXVISITED->name = {ARGUMENT_S_n, profile_n} */
      varstr_raz (varstr_profile); /* Pour le cas ARGUMENT_S_n */
      break;

    }

    break;

  case PREDICATE_S_n :/* SXVISITED->name = PREDICATE_n */
    break;

  case STRING_S_n :/* SXVISITED->name = {terminal_n, variable_n} */
    if (SXVISITED->position > 1)
      varstr_profile = varstr_catenate (varstr_profile, " ");
    break;

  case profile_n :
    switch (SXVISITED->position) {
    case 1 :/* SXVISITED->name = ARGUMENT_S_n */
      break;

    case 2 :/* SXVISITED->name = ARGUMENT_S_n */
      break;

    case 3 :/* SXVISITED->name = {ARGUMENT_S_n, VOID_n} */
      break;

    }

    varstr_raz (varstr_profile);
    break;



    /*
      Z Z Z Z
    */

  default:
    break;
  }
  /* end srcg_pi */
}


static SXBOOLEAN
str2int (char *str, SXINT *val)
{
    char 	*str2 = (char *) NULL;

    *val = (SXINT) strtol (str, &str2, 10);

    return (*str2 == SXNUL);
}


static void
srcg_pd (void)
{
  /*
    D E R I V E D
  */
  SXINT            cur_t;
  char             *prdct_name, *string;
  NODE *son, *son_son, *visited;
  

  switch (SXVISITED->name) {

  case ERROR_n :
    break;

  case ARGUMENT_S_n :
    visited = (NODE *) SXVISITED;
    visited->ste = sxstr2save (varstr_tostr (varstr_profile), varstr_length (varstr_profile));
    break;

  case CLAUSE_n :
    /* Tout est calcule', on le sort */
    clause_nb++;
    son = (NODE *) SXVISITED->son->son;
    /* son->name == %(internal|external)_predicate */
    prdct_name = sxstrget (son->ste);
    fprintf (parse_file, "%s (", prdct_name);
    fprintf (synchro_file, "%s (", prdct_name);

    son = (NODE *) son->brother->son;
    /* son->name = ARGUMENT_S */

    if (srcg_kind == DIRECT)
      fprintf (parse_file, "%s", sxstrget (son->ste));
    else
      fprintf (synchro_file, "%s", sxstrget (son->ste));

    son = (NODE *) son->brother;

    if (srcg_kind == DIRECT)
      fprintf (synchro_file, "%s", sxstrget (son->ste));
    else
      fprintf (parse_file, "%s", sxstrget (son->ste));

    son = (NODE *) son->brother;
    
    if (son != NULL && son->name == ARGUMENT_S_n) {
      /* 3eme profil, on le met ds parse */
      fprintf (parse_file, ", %s", sxstrget (son->ste));
    }

    fputs (")  --> ", parse_file);
    fputs (")  --> ", synchro_file);
    
    son = (NODE *) SXVISITED->son->brother;
    /* son->name == PREDICATE_S */

    for (son = (NODE *) son->son; son != NULL; son = (NODE *) son->brother) {
      /* son->name == PREDICATE */
      son_son = (NODE *) son->son;
      prdct_name = sxstrget (son_son->ste);

      if (son_son->name == VOID_n) {
	/* predicat predefini */
	/* ds parse uniquement */
	fprintf (parse_file, "%s (%s) ", prdct_name, sxstrget (((NODE *) (son_son->brother))->ste));
      }
      else {
	fprintf (parse_file, "%s (", prdct_name);
	fprintf (synchro_file, "%s (", prdct_name);

	son_son = (NODE *) son_son->brother->son;    /* son->name = ARGUMENT_S */

	if (srcg_kind == DIRECT)
	  fprintf (parse_file, "%s", sxstrget (son_son->ste));
	else
	  fprintf (synchro_file, "%s", sxstrget (son_son->ste));

	son_son = (NODE *) son_son->brother;

	if (srcg_kind == DIRECT)
	  fprintf (synchro_file, "%s", sxstrget (son_son->ste));
	else
	  fprintf (parse_file, "%s", sxstrget (son_son->ste));

	son_son = (NODE *) son_son->brother;
    
	if (son_son != NULL && son_son->name == ARGUMENT_S_n) {
	  /* 3eme profil on met ds parse */
	  fprintf (parse_file, ", %s", sxstrget (son_son->ste));
	}

	fputs (") ", parse_file);
	fputs (") ", synchro_file);
      }
    }
	
    fprintf (parse_file, ". %%%ld\n", clause_nb);
    fprintf (synchro_file, ". %%%ld\n", clause_nb);
    break;

  case CLAUSE_S_n :
    break;

  case PREDICATE_n :
    break;

  case PREDICATE_S_n :
    break;

  case STRING_S_n :
    break;

  case VOID_n :
    if (SXVISITED->father->name == PREDICATE_n) {
      switch (SXVISITED->token.lahead) {
      case streq_t:
	string = "&StrEq";
	break;

      case streqlen_t:
	string = "&StrEqLen";
	break;

      case strlen_t:
	string = "&StrLen";
	break;

      case false_t:
	string = "&False";
	break;

      case first_t:
	string = "&First";
	break;

      case last_t:
	string = "&Last";
	break;

      case cut_t:
	string = "&Cut";
	break;

      case true_t:
	string = "&True";
	break;

      case lex_t:
	string = "&Lex";
	break;

      default:
	sxtrap (ME, "srcg_pd");
      }

      visited = (NODE *) SXVISITED;
      visited->ste = sxstrsave (string);
    }

    break;

  case external_predicate_n :
    varstr_profile = varstr_catenate (varstr_catchar (varstr_profile, '&'), sxstrget (SXVISITED->token.string_table_entry));
    visited = (NODE *) SXVISITED;
    visited->ste = sxstr2save (varstr_tostr (varstr_profile), varstr_length (varstr_profile));
    varstr_raz (varstr_profile);
    break;

  case internal_predicate_n :
    visited = (NODE *) SXVISITED;
    visited->ste = SXVISITED->token.string_table_entry;
    break;

  case profile_n :
    break;

  case terminal_n :
    string = sxstrget (SXVISITED->token.string_table_entry);

    if (SXVISITED->father->name == STRING_S_n && SXVISITED->father->degree == 1 && str2int (string, &cur_t)) {
      /* Un entier seul comme arg, on le conserve */
      varstr_profile = varstr_catenate (varstr_profile, string);
    }
    else {
      if (*string == '"')
	string++;

      varstr_profile = varstr_quote (varstr_profile, string);
    }

    break;

  case variable_n :
    varstr_profile = varstr_catenate (varstr_profile, sxstrget (SXVISITED->token.string_table_entry));
    break;



    /*
      Z Z Z Z
    */

  default:
    break;
  }
  /* end srcg_pd */
}


static void
smpopen (struct sxtables *sxtables_ptr)
{
  sxatcvar.atc_lv.node_size = sizeof (NODE);
}


static void
smppass (void)
{

  /*   I N I T I A L I S A T I O N S   */
  /* ........... */

  varstr_profile = varstr_alloc (128);
  clause_nb = 0;

  /*   A T T R I B U T E S    E V A L U A T I O N   */
  sxsmp (sxatcvar.atc_lv.abstract_tree_root, srcg_pi, srcg_pd);

  /*   F I N A L I S A T I O N S   */
  /* ........... */

  varstr_free (varstr_profile), varstr_profile = NULL;

}


SXINT
srcg_smp (SXINT what, struct sxtables *sxtables_ptr)
{
  switch (what) {
  case SXOPEN:
    smpopen (sxtables_ptr);
    break;
  case SXSEMPASS: /* For [sub-]tree evaluation during parsing */
    break;
  case SXACTION:
    smppass ();
    break;
  case SXCLOSE:
    break;
  default:
    break;
  }
  return 1;
}
