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


/* Construction de dictionnaires a partir a partir de lignes de la forme
   mot [\t id \t any*] \n
   Dans mot, on peut echapper les caracteres par un '\'

   Options :
   -word_nb #nb      Donne une taille initiale au "dico" (estimation du nb de mots)
   -word_lgth #lgth  Donne une estimation de la longueur moyenne des mots
   -with_id          Indique qu'a chaque mot est associe' un entier positif "id" identifiant
                     le mot et qui sera retourne' lors des recherches. "id" est soit dans le fichier
		     source, soit c'est l'ordre de lecture des mots qui donne cet id.
		     Attentions, toutes les methodes ne permettent pas d'avoir des id.
   -seek             Option qui permet de verifier que la construction a marche' (chaque mot est
                     recherche' ds le dico)
   -word_tree        Forme I.  Produit un dico sous forme d'arbre dont les prefixes sont factorises (c'est un XxY)
                     La recherche de mots se fait lettre par lettre par un XxY_is_set.
		     Permet d'associer des id.
		     Il est possible de retrouver un mot a partir de son etat final.
   default           Forme II.  La forme I est simplement transformee en un sxdfa qui represente le meme arbre
		     La recherche d'un mot se fait lettre par lettre par dichotomie.
		     permet d'associer un id.
		     Il n'est pas possible de retrouver un mot a partir de son etat final.
   -min_dag          Forme III.  La forme I est transformee en un sxdfa qui represente un DAG (les suffixes de l'arbre de la forme I
                     sont factorises, le fsa ainsi obtenu est minimise' et normalise').
		     La recherche d'un mot se fait lettre par lettre par dichotomie.
		     Ne permet pas d'associer un id.
		     Il n'est pas possible de retrouver un mot a partir de son etat final.
   -sxword           C'est sxword.h qui est utilise'
                     La recherche d'un mot est global.
		     permet d'associer un id.
		     Il est possible de retrouver un mot a partir de son ste.
		     Ne peut pas etre utilise ds le spelleur.
   -comb 0           Utilise une representation en comb_vecteur simple
   -comb 1           Utilise une representation en comb_vecteur avec partie commune et partie propre
   -comb 2
   -packed           Utilise un sxdfa_packed
		     

   Les dicos peuvent avoir plusieurs formes :

   I  : construit par ./make_a_dico.out [-word_nb #nb] [-word_lgth #lgth] -word_tree [-with_id] [-seek] [-dico_name mon_dico] file
   II : construit par ./make_a_dico.out [-word_nb #nb] [-word_lgth #lgth] [-with_id] [-seek] [-dico_name mon_dico] file
   III: construit par ./make_a_dico.out [-word_nb #nb] [-word_lgth #lgth] -min_dag [-with_id] [-seek] [-dico_name mon_dico] file
   IV : construit par ./make_a_dico.out [-word_nb #nb] [-word_lgth #lgth] -sxword [-with_id] [-seek] [-dico_name mon_dico] file
   V  : construit par ./make_a_dico.out [-word_nb #nb] [-word_lgth #lgth] -min_dag -comb 0 [-with_id] [-seek] [-dico_name mon_dico] file
   VI : construit par ./make_a_dico.out [-word_nb #nb] [-word_lgth #lgth] -min_dag -comb 1 [-with_id] [-seek] [-dico_name mon_dico] file

   ESSAIS avec 407 864 mots contenant au total 4 222 480 lettres (dico.txt)

       |    CONSTRUCTION            |    RECHERCHE
   ----------------------------------------------------
       | temps (s) | taille (bytes) |    temps (s)
       |           | automate       |
   ----------------------------------------------------
    I  |  0.79     |  32 156 556    |    0.72            ./make_a_dico.out -wn 410000 -wl 10 -nC -word_tree [-seek] dico.txt
   ----------------------------------------------------
       |           |                |  
   ----------------------------------------------------
   II  |  1.72     |  12 621 296    |    0.61            ./make_a_dico.out -wn 410000 -wl 10 -nC [-seek] dico.txt
   ----------------------------------------------------
   II-1|  1.74     |  13 118 296    |    0.64            ./make_a_dico.out -wn 410000 -wl 10 -nC -wi [-seek] dico.txt
   ----------------------------------------------------
   II-2|  1.74     |   6 773 200    |    0.54            ./make_a_dico.out -wn 410000 -wl 10 -nC -packed [-seek] dico.txt
   ----------------------------------------------------
   II-3|  1.73     |   7 270 200    |    0.60            ./make_a_dico.out -wn 410000 -wl 10 -nC -wi -packed [-seek] dico.txt
   ----------------------------------------------------
       |           |                |  
   ----------------------------------------------------
  III  |  2.28     |   2 412 884    |    0.60            ./make_a_dico.out -wn 410000 -wl 10 -nC -md [-seek] dico.txt
   ----------------------------------------------------
  III-1|  2.50     |  10 372 276    |    0.63            ./make_a_dico.out -wn 410000 -wl 10 -nC -md -wi [-seek] dico.txt
   ----------------------------------------------------
  III-2|  2.28     |   1 301 868    |    0.50            ./make_a_dico.out -wn 410000 -wl 10 -nC -md -packed [-seek] dico.txt
   ----------------------------------------------------
  III-3|  2.58     |   6 265 684    |    0.54            ./make_a_dico.out -wn 410000 -wl 10 -nC -md -wi -packed [-seek] dico.txt
   ----------------------------------------------------
       |           |                |  
   ----------------------------------------------------
   IV  |  0.45     |  11 214 192    |    0.33            ./make_a_dico.out -wn 410000 -wl 10 -nC -sxword [-seek] dico.txt
   ----------------------------------------------------
       |           |                |  
   ----------------------------------------------------
    V  |  4.72     |   4 463 424    |    0.34            ./make_a_dico.out -wn 410000 -wl 10 -nC -comb 0 -cvt 10000 [-seek] dico.txt
   ----------------------------------------------------
    V-1|  4.78     |   4 463 424    |    0.34            ./make_a_dico.out -wn 410000 -wl 10 -nC -comb 0 -cvt 10000 -wi [-seek] dico.txt
   ----------------------------------------------------
    V-2|  2.88     |     894 296    |    0.32            ./make_a_dico.out -wn 410000 -wl 10 -nC -comb 0 -cvt 10000 -md [-seek] dico.txt
   ----------------------------------------------------
    V-3|  5.00     |   4 417 164    |    0.34            ./make_a_dico.out -wn 410000 -wl 10 -nC -comb 0 -cvt 10000 -md -wi [-seek] dico.txt
   ----------------------------------------------------
       |           |                |  
   ----------------------------------------------------
   VI  |159.34     |   4 455 448    |    0.44            ./make_a_dico.out -wn 410000 -wl 10 -nC -comb 1 -cvt 10000 [-seek] dico.txt
   ----------------------------------------------------
   VI-1|160.57     |   4 553 792    |    0.62            ./make_a_dico.out -wn 410000 -wl 10 -nC -wi -comb 1 -cvt 10000 [-seek] dico.txt
   ----------------------------------------------------
   VI-2|  7.84     |     864 360    |    0.34            ./make_a_dico.out -wn 410000 -wl 10 -nC -comb 1 -cvt 10000 -md [-seek] dico.txt
   ----------------------------------------------------
   VI-3|282.44     |   4 511 016    |    0.24            ./make_a_dico.out -wn 410000 -wl 10 -nC -comb 1 -cvt 10000 -md -wi [-seek] dico.txt
   ----------------------------------------------------
*/





#include "sxversion.h"
#include "sxunix.h"

char WHAT_SXMAIN[] = "@(#)SYNTAX - $Id: make_a_dico.c 2227 2018-09-26 16:22:59Z garavel $" WHAT_DEBUG;

static char ME [] = "make_a_dico.c";

#include "sxstack.h"
#include "XH.h"
#include "XxY.h"
#include "fsa.h"
#include "sxword.h"
#include "varstr.h"
#include <ctype.h>

extern struct sxtables  sxtables;

static SXINT      word_nb, word_lgth;
static SXBOOLEAN    make_a_min_dag, with_id, with_word_tree, seek, with_sxword, from_right_to_left, with_comb, is_packed, output_in_C, is_static, is_probabilized, is_probabilized_forced, is_silent;
static SXINT      comb_kind, comb_vector_threshold;
static char       *define_terminals_file_name;
static FILE       *define_terminals_file;



/*---------------*/
/*    options    */
/*---------------*/

static char	Usage [] = "\
Usage:\t%s [options] [files]\n\
options=\n\
\t\t-wn #nb, -word_nb #nb,/* Default==100 */\n\
\t\t-wl #lgth, -word_lgth #lgth,/* Default==16 */\n\
\t\t-md, -min_dag, -nmd, -no_min_dag,/* Default md */\n\
\t\t-wi, -with_id,/* Default */\n\
\t\t-wt, -word_tree,\n\
\t\t-s, -seek,\n\
\t\t-sxw, -sxword,\n\
\t\t-dn name, -dico_name name,/* Default sxdfa_comb_inflected_form */\n\
\t\t-rl, -from_right_to_left,\n\
\t\t-c #kind, -comb #kind,/* Default kind==0 */\n\
\t\t-cvt #val, -comb_vector_threshold #val,/* Default 10000 */\n\
\t\t-p, -packed,\n\
\t\t-C, -output_in_C, -nC,-no_output_in_C/* Default -C */\n\
\t\t-e, -extern,/* Default static */\n\
\t\t-dt file_name, -define_terminals file_name,\n\
\t\t-plex, -probabilized_lexicon, /* Default, not specified proba = 1.0 */ \n	\
\t\t-noplex, -not-probabilized_lexicon, /* Ignore proba if there is any */ \n \
\t\t-s, -silent, /* Do not display warnings */\n\
\t\t--h, --help,\n\
";

#define OPTION(opt)	(1l << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB ((sizeof(option_tbl)/sizeof(*option_tbl))-1)

#define UNKNOWN_ARG 		((SXINT)0)
#define WORD_NB 		((SXINT)1)
#define WORD_LGTH 		((SXINT)2)
#define MIN_DAG 		((SXINT)3)
#define WITH_ID 		((SXINT)4)
#define WORD_TREE 		((SXINT)5)
#define SEEK 		        ((SXINT)6)
#define SXWORD 		        ((SXINT)7)
#define DICO_NAME	        ((SXINT)8)
#define FROM_RIGHT_TO_LEFT      ((SXINT)9)
#define COMB_VECTOR 		((SXINT)10)
#define COMB_VECTOR_THRESHOLD   ((SXINT)11)
#define PACKED 		        ((SXINT)12)
#define IN_C 		        ((SXINT)13)
#define EXTERN 		        ((SXINT)14)
#define DEFINE_TERMINALS        ((SXINT)15)
#define IS_PROBA                ((SXINT)16)
#define IS_SILENT                ((SXINT)17)
#define HELP 		        ((SXINT)18)

static char	*option_tbl [] = {
  "",
  "wn", "word_nb",
  "wl", "word_lgth",
  "md", "min_dag",
  "nmd", "no_min_dag",
  "wi", "with_id",
  "nwi", "no_with_id",
  "wt", "word_tree",
  "s", "seek",
  "sxw", "sxword",
  "dn", "dico_name",
  "rl", "from_right_to_left",
  "c", "comb",
  "cvt", "comb_vector_threshold",
  "p", "packed",
  "C", "output_in_C",
  "nC", "no_output_in_C",
  "e", "extern",
  "dt", "define_terminals",
  "plex", "probabilized_lexicon",
  "noplex", "not_probabilized_lexicon",
  "s", "silent"
  "-h", "-help"
};
static SXINT	option_kind [] = {
  UNKNOWN_ARG, 
  WORD_NB, WORD_NB, 
  WORD_LGTH, WORD_LGTH, 
  MIN_DAG, MIN_DAG, 
  -MIN_DAG, -MIN_DAG, 
  WITH_ID, WITH_ID, 
  -WITH_ID, -WITH_ID, 
  WORD_TREE, WORD_TREE, 
  SEEK, SEEK, 
  SXWORD, SXWORD, 
  DICO_NAME, DICO_NAME,
  FROM_RIGHT_TO_LEFT, FROM_RIGHT_TO_LEFT, 
  COMB_VECTOR, COMB_VECTOR, 
  COMB_VECTOR_THRESHOLD, COMB_VECTOR_THRESHOLD, 
  PACKED, PACKED, 
  IN_C, IN_C, 
  -IN_C, -IN_C, 
  EXTERN, EXTERN,
  DEFINE_TERMINALS, DEFINE_TERMINALS, 
  IS_PROBA, IS_PROBA,
  -IS_PROBA, -IS_PROBA,
  IS_SILENT, IS_SILENT,
  HELP, HELP
};



static SXINT	option_get_kind (char *arg)
{
    char	**opt;

    if (*arg++ != '-')
	return UNKNOWN_ARG;

    for (opt = &(option_tbl [OPT_NB]); opt > option_tbl; opt--) {
	if (strcmp (*opt, arg) == 0 /* egalite */ )
	    break;
    }

    return option_kind [opt - option_tbl];
}



static char	*option_get_text (SXINT kind)
{
    SXINT	i;

    for (i = OPT_NB; i > 0; i--) {
	if (option_kind [i] == kind)
	    break;
    }

    return option_tbl [i];
}


static SXINT cur_word_nb, max_word_lgth, prev_state, inflected_form_nb, inflected_form_total_char_nb;
static char  *dico_name;

#define TO_BE_NORMALIZED 1

static SXBOOLEAN                 case1, case4, case5, case52, case53;
static FILE	               *infile;
static struct word_tree_struct dico_word_tree, *cur_word_tree_ptr;
static struct sxdfa_struct     dico_sxdfa;
static struct sxdfa_packed_struct     dico_sxdfa_packed;
static sxword_header           names;
static SXINT                   *ste2id;
static struct sxdfa_comb       dico;
static char                    *source_file_pathname;
static struct next_struct {
  SXINT id, next;
} *next_struct_list;

/* ift = (inflected_form + terminal) joint id */
static SXINT                   t_code, terminal_ste2code_size, inflected_form_ste_set_size, max_ste_t_code, max_id, max_ift_code, ift_ste2code_size;
static SXINT                   ift_code2proba_size;
static SXINT                   *terminal_ste2code, *ift_ste2code, *ift_code2terminal;
static SXFLOAT                 *ift_code2proba;
static SXBOOLEAN               is_error;
static SXBA                    inflected_form_ste_set;

static VARSTR                  vstr, wvstr;

static void
gen_header (char *file_name)
{
  long	        date_time;
  char          threshold_string [12];
    
  date_time = time (0);

  if (with_comb) {
    sprintf (&(threshold_string [0]), "%ld", comb_vector_threshold);
  }

  printf (
	  "\n\
/* ********************************************************************\n\
\tThis dictionary has been generated by\n\
\t./make_a_dico.out %s-word_nb #%ld -word_lgth #%ld%s%s%s%s%s%s -output_in_C -dico_name %s%s%s %s\n%s\
\ton %s\
\tby the SYNTAX(*) make_a_dico processor\n\
   ********************************************************************\n\
\t(*) SYNTAX is a trademark of INRIA.\n\
   ******************************************************************** */\n\n\n",
	  is_probabilized_forced ? (is_probabilized ? "-plex " : "-noplex") : "",
	  word_nb,
	  word_lgth,
	  with_word_tree ? " -word_tree" : (with_sxword ? " -sxword" : (with_comb ? " -comb" : (is_packed ? " -packed" : ""))),
	  with_comb ? (comb_kind==0 ? " 0" : (comb_kind==1 ? " 1" : " 2")) : "",
	  with_comb ? " -comb_vector_threshold " : "",
	  with_comb ? threshold_string : "",
	  with_id ? " -with_id" : "",
	  seek ? " -seek" : "",
	  dico_name,
	  define_terminals_file ? " -define_terminals " : "",
	  define_terminals_file ? define_terminals_file_name : "",
	  file_name ? file_name : "[stdin]",
	  is_probabilized_forced ? "" : (is_probabilized ? "\t(probabilized lexicon guessed)\n" : "\t(not probabilized lexicon guessed)\n"),
	  ctime (&date_time));

  /* On sort la date actuelle  */
  printf ("#define MAKE_A_DICO_TIME %ld\n", date_time);

  if (with_sxword)
    fputs ("\n#include \"sxword.h\"\n",stdout);
  else
    fputs ("\n#include \"fsa.h\"\n",stdout);
}



static void
oflw (SXINT old_size, SXINT new_size)
{
  sxuse (old_size);
  word_tree_oflw (cur_word_tree_ptr, old_size, new_size);
}



static void
make_a_dico_run (char *pathname)
{
  infile = NULL;

  source_file_pathname = pathname;

  if (pathname == NULL) {
    int	c;

    if (sxverbosep) {
      fputs ("\"stdin\":\n", sxtty);
    }

    if ((infile = sxtmpfile ()) == NULL) {
      fprintf (sxstderr, "%s: Unable to create ", ME);
      sxperror ("temp file");
      sxerrmngr.nbmess [2]++;
      return;
    }

    int nb_tab;
    nb_tab = 0;
    SXBOOLEAN begin_count_tab, tabulation_met;
    begin_count_tab = SXFALSE;
    tabulation_met = SXFALSE;

    while ((c = getchar ()) != EOF) {
      if (!is_probabilized_forced && !is_probabilized) {
	if (c == '\n') {
	  if (nb_tab == 2) is_probabilized = SXTRUE;
	  nb_tab = 0;
	  begin_count_tab = SXFALSE;
	  tabulation_met = SXFALSE;
	} 
	else {
	  if (c == '\t') {
	    if (begin_count_tab && !tabulation_met)
	      tabulation_met = SXTRUE;
	  } 
	  else {
	    if (!begin_count_tab) begin_count_tab = SXTRUE;
	    if (tabulation_met) {
	      nb_tab++;
	      tabulation_met = SXFALSE;
	    }
	  }
	}
      }
      putc (c, infile);
    }
    rewind (infile);
    syntax (SXBEGIN, &sxtables, infile, "");
  }
  else {
    if ((infile = sxfopen (pathname, "r")) == NULL) {
      fprintf (sxstderr, "%s: Cannot open (read) ", ME);
      sxperror (pathname);
      sxerrmngr.nbmess [2]++;
      return;
    }
    else {
      if (sxverbosep) {
	fprintf (sxtty, "%s:\n", pathname);
      }

      syntax (SXBEGIN, &sxtables, infile, pathname);
    }
  }

  syntax (SXACTION, &sxtables);
  syntax (SXEND, &sxtables);
  fclose (infile);
}



int
main (int argc, char *argv[])
{
    sxopentty ();

    sxuse (prev_state);

    /* Valeurs par defaut */
    word_nb = 100;
    word_lgth = 16;
    with_word_tree = seek = with_sxword = from_right_to_left = is_packed = SXFALSE;
    with_id = with_comb = make_a_min_dag = is_static = output_in_C = SXTRUE;
    comb_kind = 0;
    comb_vector_threshold = 10000;
    dico_name = "sxdfa_comb_inflected_form";

    define_terminals_file = NULL;
    define_terminals_file_name = NULL;
    is_probabilized = SXFALSE;
    is_probabilized_forced = SXFALSE;
    is_silent = SXFALSE;

    syntax (SXINIT, &sxtables, SXTRUE /* has includes */ );

    {
      /* Tous les arguments sont des noms de fichiers, "-" signifiant stdin */
      int	argnum;
      SXBOOLEAN   has_unknown_arg = SXFALSE;

      for (argnum = 1; argnum < argc; argnum++) {
	switch (option_get_kind (argv [argnum])) {
	case WORD_NB:
	  if (++argnum >= argc || !sxnumarg (argv [argnum], &word_nb))
	    fprintf (sxstderr, "%s: a numeric argument must follow the \"%s\" option;\n\tvalue %ld is retained.\n", ME
		     , option_get_text (WORD_NB), (SXINT) word_nb);

	  break;
	  

	case WORD_LGTH:
	  if (++argnum >= argc || !sxnumarg (argv [argnum], &word_lgth))
	    fprintf (sxstderr, "%s: a numeric argument must follow the \"%s\" option;\n\tvalue %ld is retained.\n", ME
		     , option_get_text (WORD_LGTH), (SXINT) word_lgth);

	  break;

	case MIN_DAG:
	  make_a_min_dag = SXTRUE;
	  
	  break;

	case -MIN_DAG:
	  make_a_min_dag = SXFALSE;
	  
	  break;

	case WITH_ID:
	  with_id = SXTRUE;

	  break;

	case -WITH_ID:
	  with_id = SXFALSE;

	  break;

	case WORD_TREE:
	  with_word_tree = SXTRUE;
	  with_sxword = with_comb = SXFALSE;
	  break;

	case SEEK:
	  seek = SXTRUE;
	  break;

	case SXWORD :
	  with_sxword = SXTRUE;
	  with_word_tree = with_comb = SXFALSE;
	  break;
	  
	case DICO_NAME:
	  if (++argnum >= argc)
	    fprintf (sxstderr, "%s: a dictionary name must follow the \"%s\" option;\n\tvalue %s is retained.\n", ME
		     , option_get_text (DICO_NAME), dico_name);

	  dico_name = argv [argnum];
	  break;

	case FROM_RIGHT_TO_LEFT:
	  from_right_to_left = SXTRUE;
	  break;

	case COMB_VECTOR:
	  if (++argnum >= argc || !sxnumarg (argv [argnum], &comb_kind) || comb_kind > 2 || comb_kind < 0) {
	    comb_kind = 0;
	    fprintf (sxstderr, "%s: an optimization kind number 0, 1 or 2 must follow the \"%s\" option;\n\tvalue %ld is retained.\n", ME
		     , option_get_text (COMB_VECTOR), comb_kind);
	  }
	  with_comb = SXTRUE;
	  with_word_tree = with_sxword = SXFALSE;
	  break;

	case COMB_VECTOR_THRESHOLD:
	  if (++argnum >= argc || !sxnumarg (argv [argnum], &comb_vector_threshold))
	    fprintf (sxstderr, "%s: a number must follow the \"%s\" option;\n\tvalue %ld is retained.\n", ME
		     , option_get_text (COMB_VECTOR_THRESHOLD), comb_vector_threshold);
	  break;

	case PACKED:
	  is_packed = SXTRUE;
	  break;

	case IN_C:
	  output_in_C = SXTRUE;
	  break;

	case -IN_C:
	  output_in_C = SXFALSE;
	  break;

	case EXTERN:
	  is_static = SXFALSE;
	  break;

	case DEFINE_TERMINALS:
	  if (++argnum >= argc) {
	    fprintf (sxstderr, "%s: a pathname must follow the \"%s\" option;\n", ME, option_get_text (DEFINE_TERMINALS));
	    SXEXIT (3);
	  }
	      
	  if ((define_terminals_file = sxfopen (define_terminals_file_name = argv [argnum], "w")) == NULL) {
	    fprintf (sxstderr, "%s: Cannot open (create) file in write mode ", ME);
	    sxperror (argv [argnum]);
	    SXEXIT (3);
	  }

	  break;

	case IS_PROBA:
	  is_probabilized = SXTRUE;
	  is_probabilized_forced = SXTRUE;
	  break;

	case -IS_PROBA:
	  is_probabilized = SXFALSE;
	  is_probabilized_forced = SXTRUE;
	  break;

	case IS_SILENT:
	  is_silent = SXTRUE;
	  break;

	case HELP:
	  fprintf (sxstderr, "%s", Usage);
	  return EXIT_SUCCESS;	  

	case UNKNOWN_ARG:
	  has_unknown_arg = SXTRUE;

	  if (argnum+1 == argc && argv [argnum] [0] == '-' && argv [argnum] [1] == '\0') {
	    make_a_dico_run (NULL);
	  }
	  else {
	    if (argv [argnum] [0] == '-')
	      fprintf (sxstderr, "A file name cannot start with an \"-\": ignored.\n");
	    else
	      make_a_dico_run (argv [argnum]);
	  }

	  break;

	default:
	  sxtrap (ME, "main");
	  break;
	}
      }

      if (!has_unknown_arg)
	make_a_dico_run (NULL);
    }

    syntax (SXFINAL, &sxtables, SXTRUE);

    if (define_terminals_file)
      fclose (define_terminals_file);

    return EXIT_SUCCESS;
}

void
make_a_dico_scanact (SXINT entry, SXINT act_no)
{
  switch (entry) {
    case SXOPEN:
    case SXCLOSE:
    case SXINIT:
    case SXFINAL:
      return;

    case SXACTION:
      switch (act_no) {
      case 1:
	/* The pathname of the include file is in token_string */
	if (sxpush_incl (sxsvar.sxlv_s.token_string))
	  /* Now, the source text comes from the include file ... */
	  return;

	/* something failed (unable to open, recursive call, ...) */
	/* Error message */
	sxerror (sxsvar.sxlv.terminal_token.source_index /* Source coordinates of the include command */,
		 sxsvar.sxtables->err_titles [2][0],
		 "%sUnable to process the include file \"%s\".",
		 sxsvar.sxtables->err_titles [2]+1 /* Severity level: Error */,
		 sxsvar.sxlv_s.token_string /* Include file name */
		 );
	/* However, scanning of the current files goes on ... */
	return;

      case 2:
	/* End of include processing */
	if (sxpop_incl ())
	  return;

	/* Something really goes wrong ... */
	/* Error message */
	fputs ("Sorry, the include processing garbled, nevertheless hope to see you again soon.\n", sxstderr);
	sxexit (3) /* panic */;

      default:
	break;
      }

    default :
      fputs ("The function \"make_a_dico_scanact\" is out of date w.r.t. its specification.\n", sxstderr);
      sxexit (3) /* panic */;
  }
}

static SXVOID
init (void)
{
  is_error = SXFALSE;
  t_code = 0;

  max_ste_t_code = 0;
  terminal_ste2code_size = 256;
  terminal_ste2code = (SXINT *) sxcalloc (terminal_ste2code_size+1, sizeof (SXINT));
  if (is_probabilized) {
    ift_ste2code_size = 256;
    ift_ste2code = (SXINT *) sxcalloc (terminal_ste2code_size+1, sizeof (SXINT));
    ift_code2proba_size = 256;
    ift_code2proba = (SXFLOAT *) sxcalloc (ift_code2proba_size+1, sizeof (SXFLOAT));
    ift_code2terminal = (SXINT *) sxcalloc (ift_code2proba_size+1, sizeof (SXINT));
  }

  inflected_form_ste_set_size = word_nb+256;
  inflected_form_ste_set = sxba_calloc (inflected_form_ste_set_size+1);

  if (with_id) {
    ste2id = (SXINT*) sxalloc (inflected_form_ste_set_size+1, sizeof (SXINT)), ste2id [0] = inflected_form_ste_set_size;
    next_struct_list = (struct next_struct*) sxalloc (inflected_form_ste_set_size+1, sizeof (struct next_struct));
    next_struct_list [0].id = 0;
    next_struct_list [0].next = inflected_form_ste_set_size;
  }
  else
    ste2id = NULL;

  max_id = inflected_form_nb = inflected_form_total_char_nb = cur_word_nb = max_word_lgth = 0;

  if (is_probabilized)
    wvstr = varstr_alloc (128);

  if (define_terminals_file) {
    SXINT cur_time;

    vstr = varstr_alloc (128);
    cur_time = time (0);  /* now */

    fprintf (define_terminals_file,
	     "\
/* ***************************************************************************************\n\
\tThis #define'd terminal symbol list has been extracted on %s\
\tby the SYNTAX(*) processor make_a_dico with the \"-define_terminals %s\" option\n\
\tfrom the source file \"%s\"\n\
******************************************************************************************\n\
\t(*) SYNTAX is a trademark of INRIA.\n\
****************************************************************************************** */\n\n\n",
	     ctime (&cur_time),
	     define_terminals_file_name,
	     (source_file_pathname == NULL) ? "stdin" : source_file_pathname);
  }
}


static SXVOID
action (SXINT action_no)
{
  SXINT	         path, ste, len, top, ift_ste, ift_code;
  char           car, *str;
  struct sxtoken *token_ptr;
  static SXINT   id, terminal_ste;
  SXFLOAT        proba;

  sxinitialise (top);
  sxinitialise(path), sxuse (path);
  if (is_error)
    return;

  switch (action_no) {
  case 0:
    return;

  case 1:
    /* <terminal_+>		= <terminal_+> <terminal>			; 1 */
    /* <terminal_+>		= <terminal>					; 1 */
    /* Definition d'un terminal */
    /* Ds tous les cas on va faire une tstring */
    token_ptr = &(SXSTACKtoken (SXSTACKtop ()));
    ste = token_ptr->string_table_entry; // r�cup�re le symbole le plus � droite (ici, le terminal %MOT)

    /* Testing if ste accesses over the boundary terminal_ste2code_size
     * in that case we sxrealloc largelly so that ste is now the
     * middle between old and new boundaries */ 
    if (ste > terminal_ste2code_size) {
      terminal_ste2code = (SXINT *) sxrecalloc (terminal_ste2code, terminal_ste2code_size+1, 2*ste-terminal_ste2code_size+1, sizeof (SXINT));
      terminal_ste2code_size = 2*ste-terminal_ste2code_size;
    }

    if (terminal_ste2code [ste] == 0) {
      case1 = SXTRUE;
      terminal_ste2code [ste] = ++t_code;
      max_ste_t_code = ste;

      if (define_terminals_file) {
	str = sxstrget (ste);
	varstr_raz (vstr);

	while ((car = *str++)) {
	  vstr = varstr_catenate (vstr, SXCHAR2CSTR (car));
	}

	fprintf (define_terminals_file, "#define SXT_%s\t%ld\n", varstr_tostr (vstr), (long)t_code);
      }
    }
    else {
      if (define_terminals_file == NULL) {
	/* On accepte les duplicata ds le cas define_terminals_file */
	sxerror (token_ptr->source_index,
		 sxsvar.sxtables->err_titles [1][0],
		 "%sMultiple occurrence of the terminal symbol \"%s\".",
		 sxsvar.sxtables->err_titles [1]+1 /* Severity level: Warning */,
		 sxstrget (ste)
		 );
      }
    }

    return;

  case 2:
    /* <inflected_forme_code>	= <terminal>					; 2 */
    /* Utilisation d'un terminal */
    if (ste2id) {
      case52 = SXTRUE;
      token_ptr = &(SXSTACKtoken (SXSTACKtop ()));
      terminal_ste = ste = token_ptr->string_table_entry;

      if (ste > max_ste_t_code) {
	if (!is_silent)
	  sxerror (token_ptr->source_index,
		   sxsvar.sxtables->err_titles [1][0],
		   "%sUnknown terminal symbol \"%s\", line is ignored.",
		   sxsvar.sxtables->err_titles [1]+1 /* Severity level: Warning */,
		   sxstrget (ste)
		   );	
      } else {
	id = terminal_ste2code [ste];
	
	if (id == 0 || case53) {
	  if (id == 0) {
	    if (!is_silent)
	      sxerror (token_ptr->source_index,
		       sxsvar.sxtables->err_titles [1][0],
		       "%sUnknown terminal symbol \"%s\", line is ignored.",
		       sxsvar.sxtables->err_titles [1]+1 /* Severity level: Warning */,
		       sxstrget (ste)
		       );
	  }
	  else {
	    if (!is_silent)
	      sxerror (token_ptr->source_index,
		       sxsvar.sxtables->err_titles [1][0],
		       "%sTerminal symbol \"%s\" and integer values cannot be mixed, line is ignored.",
		       sxsvar.sxtables->err_titles [1]+1 /* Severity level: Warning */,
		       sxstrget (ste)
		       );
	    id = 0;
	  }
	}
	else {
	  if (id > max_id)
	    max_id = id;
	}
      }
    }

    break;

  case 3:
    /* <inflected_forme_code>	= %ENTIER					; 3 */
    if (ste2id) {
      case53 = SXTRUE;
      token_ptr = &(SXSTACKtoken (SXSTACKtop ()));
      ste = token_ptr->string_table_entry;
      id = atoi (sxstrget (ste));

      if (id > max_id)
	max_id = id;

      if (case52) {
	if (!is_silent)
	  sxerror (token_ptr->source_index,
		   sxsvar.sxtables->err_titles [1][0],
		   "%sInteger value \"%i\" and terminal symbols cannot be mixed, line is ignored.",
		   sxsvar.sxtables->err_titles [1]+1 /* Severity level: Warning */,
		   id
		   );
	id = 0;
      }
    }

    break;

  case 4:
    /* <inflected_form_def>    = <inflected_form>				; 4 */
    /* Ici, si with_id, chaque ff recoit un id different */
    token_ptr = &(SXSTACKtoken (SXSTACKtop ()));
    ste = token_ptr->string_table_entry;

    if (case5) {
      if (!is_silent)
	sxerror (token_ptr->source_index,
		 sxsvar.sxtables->err_titles [1][0],
		 "%sThis inflected form \"%s\" must possess an id spec, it is ignored.",
		 sxsvar.sxtables->err_titles [1]+1 /* Severity level: Warning */,
		 sxstrget (ste)
		 );
    }
    else {
      case4 = SXTRUE;

      while (ste > inflected_form_ste_set_size) {
	inflected_form_ste_set_size *= 2;
	inflected_form_ste_set = sxba_resize (inflected_form_ste_set, inflected_form_ste_set_size+1);

	if (ste2id)
	  ste2id = (SXINT *) sxrealloc (ste2id, inflected_form_ste_set_size+1, sizeof (SXINT));
      }

      /* Dans ce cas on suppose que le mot courant est un "terminal", on sortira une table code_t => chaine terminale */
      if (ste > terminal_ste2code_size) {
	terminal_ste2code = (SXINT *) sxrecalloc (terminal_ste2code, terminal_ste2code_size+1, 2*ste-terminal_ste2code_size+1, sizeof (SXINT));
	terminal_ste2code_size = 2*ste-terminal_ste2code_size;
      }

      if (SXBA_bit_is_reset_set (inflected_form_ste_set, ste)) {
	inflected_form_nb++;
	len = sxstrlen (ste);
	inflected_form_total_char_nb += len;
    
	if (len > max_word_lgth)
	  max_word_lgth = len;

	if (ste2id) {
	  id = ++cur_word_nb;

	  if (id > max_id)
	    max_id = id;

	  ste2id [ste] = id;
	}
      
	terminal_ste2code [ste] = ++t_code;
	max_ste_t_code = ste;

	if (!case1 && define_terminals_file) {
	  /* Il n'y a pas de liste de terminaux et
	     il n'y a pas de code associe' a chaque ff
	     et l'utilisateur a demande' a sortir un fichier des #define
	     On suppose donc que la liste des ff (en colonne 1) est en fait la
	     liste des terminaux */
	  str = sxstrget (ste);
	  varstr_raz (vstr);

	  while ((car = *str++)) {
	    vstr = varstr_catenate (vstr, SXCHAR2CSTR (car));
	  }

	  fprintf (define_terminals_file, "#define SXT_%s\t%ld\n", varstr_tostr (vstr), (long)t_code);
	}
      }
      else {
	if (case1 || define_terminals_file == NULL) {
	  sxerror (token_ptr->source_index,
		   sxsvar.sxtables->err_titles [1][0],
		   "%sMultiple occurrence of the inflected_form \"%s\".",
		   sxsvar.sxtables->err_titles [1]+1 /* Severity level: Warning */,
		   sxstrget (ste)
		   );
	}
      }
    }

    break;

  case 6:
    /* <inflected_form_def>    = <inflected_form> <inflected_forme_code> <proba>	; 6 */
    token_ptr = &(SXSTACKtoken (SXSTACKtop ())); /* etait initialise ds le cas sxerror, merci Hubert ! */

    if (!is_probabilized && is_probabilized_forced) {/* no proba forced */
      if (!is_silent) {
	sxerror (token_ptr->source_index,
		 sxsvar.sxtables->err_titles [1][0],
		 "%sThis entry has a probability with the option -noplex. Probability information ignored.",
		 sxsvar.sxtables->err_titles [1]+1 /* Severity level: Warning */
		 );
      }
    }
    else {      
      ste = token_ptr->string_table_entry;
      proba = sxatof (sxstrget (ste));
    }
    /* pas de break */

  case 5:
    /* <inflected_form_def>    = <inflected_form> <inflected_forme_code>	; 5 */
    token_ptr = &(SXSTACKtoken (SXSTACKtop () - (action_no == 5 ? 1 : 2)));
    ste = token_ptr->string_table_entry;

    if (is_probabilized) {
      varstr_raz (wvstr);
      wvstr = varstr_catenate (wvstr, sxstrget (ste));
      wvstr = varstr_catchar (wvstr, '_');
      wvstr = varstr_catchar (wvstr, '_');
      wvstr = varstr_catenate (wvstr, sxstrget (terminal_ste));
      ift_ste = sxstr2save (varstr_tostr (wvstr), varstr_length (wvstr));

      /* Testing if ift_ste accesses over the boundary ift_ste2code_size 
       * in that case we sxrealloc largelly so that ift_ste is now 
       * the middle between old and new boundaries */ 
      if (ift_ste > ift_ste2code_size) {
	ift_ste2code = (SXINT *) sxrecalloc (ift_ste2code, ift_ste2code_size+1, 2*ift_ste-ift_ste2code_size+1, sizeof (SXINT));
	ift_ste2code_size = 2*ift_ste - ift_ste2code_size;
      }      

      if (ift_ste2code [ift_ste] == 0) {
	ift_code = ift_ste2code [ift_ste] = ++max_ift_code;
      }
      else
	ift_code = ift_ste2code [ift_ste];

      if (action_no == 5)
	proba = 1.0;

      /* Testing if ift_code accesses over the boundary ift_code2proba_size 
       * in that case we sxrealloc largelly so that ift_code is now 
       * the middle between old and new boundaries */ 
      if (ift_code > ift_code2proba_size) {
	ift_code2proba = (SXFLOAT *) sxrecalloc (ift_code2proba, ift_code2proba_size+1, 2*ift_code-ift_code2proba_size+1, sizeof (SXFLOAT));
	ift_code2terminal = (SXINT *) sxrecalloc (ift_code2terminal, ift_code2proba_size+1, 2*ift_code-ift_code2proba_size+1, sizeof (SXINT));
	ift_code2proba_size = 2*ift_code - ift_code2proba_size;
      }

      if (ift_code2terminal [ift_code]) {
	if (!is_silent) {
	  sxerror (token_ptr->source_index,
		   sxsvar.sxtables->err_titles [1][0],
		   "%sThis inflected form \"%s\" has more than one entry with the same terminal \"%s\". Ignored.",
		   sxsvar.sxtables->err_titles [1]+1 /* Severity level: Warning */,
		   sxstrget (ste),
		   sxstrget (terminal_ste)
		   );
	}
      }
      else {
	if (proba < 0.0 || proba > 1.0) {
	  if (!is_silent)
	    sxerror (token_ptr->source_index,
		     sxsvar.sxtables->err_titles [1][0],
		     "%sThis entry has a probability higher than 1 or lower than 0 (proba = " SXFDLD "). Proba set to 0.",
		     sxsvar.sxtables->err_titles [1]+1 /* Severity level: Warning */,
		     proba
		     );
	  proba = 0.0;
	}
	ift_code2proba [ift_code] = proba;
	ift_code2terminal [ift_code] = id;
      }
    }

    if (case4) {
      if (!is_silent)
	sxerror (token_ptr->source_index,
		 sxsvar.sxtables->err_titles [1][0],
		 "%sThis inflected form \"%s\" must not possess an id spec, it is ignored.",
		 sxsvar.sxtables->err_titles [1]+1 /* Severity level: Warning */,
		 sxstrget (ste)
		 );
    }
    else {
      case5 = SXTRUE;

      while (ste > inflected_form_ste_set_size) {
	inflected_form_ste_set_size *= 2;
	inflected_form_ste_set = sxba_resize (inflected_form_ste_set, inflected_form_ste_set_size+1);

	if (ste2id)
	  ste2id = (SXINT *) sxrealloc (ste2id, inflected_form_ste_set_size+1, sizeof (SXINT));

      }
	
      if (ste2id) {
	top = ++next_struct_list [0].id;

	if (top >= next_struct_list [0].next) {
	  next_struct_list [0].next *= 2;
	  next_struct_list = (struct next_struct*) sxrealloc (next_struct_list, next_struct_list [0].next+1, sizeof (struct next_struct));
	}
      }

      if (SXBA_bit_is_reset_set (inflected_form_ste_set, ste)) {
	inflected_form_nb++;
	len = sxstrlen (ste);
	inflected_form_total_char_nb += len;
    
	if (len > max_word_lgth)
	  max_word_lgth = len;

	/* id est positionne' par les cas 2 ou 3 */
	if (ste2id) {
	  ste2id [ste] = top;
	  if (is_probabilized)
	    next_struct_list [top].id = ift_code;
	  else
	    next_struct_list [top].id = id;
	  if (max_id < next_struct_list [top].id) max_id = next_struct_list [top].id;
	  next_struct_list [top].next = 0;
	}
      }
      else {
	if (ste2id) {
	  if (is_probabilized)
	    next_struct_list [top].id = ift_code;
	  else
	    next_struct_list [top].id = id;
	  if (max_id < next_struct_list [top].id) max_id = next_struct_list [top].id;
	  next_struct_list [top].next = ste2id [ste];
	  ste2id [ste] = top;
	}
	else
	  sxerror (token_ptr->source_index,
		   sxsvar.sxtables->err_titles [1][0],
		   "%sMultiple occurrence of the inflected_form \"%s\".",
		   sxsvar.sxtables->err_titles [1]+1 /* Severity level: Warning */,
		   sxstrget (ste)
		   );
      }
    }

      break;

    default:
      fputs ("The function \"action\" is out of date with respect to its specification.\n", sxstderr);
      sxexit (SXSEVERITIES);
    }
}


static SXVOID
final (void)
{
  SXINT     lgth, ste, id, top, bot;
  SXINT     Xforeach, Yforeach;
  char      *ff;
  XH_header XH_id_stack;

  if (define_terminals_file) {
    fprintf (define_terminals_file, "#define SXT_End_Of_File %ld\n", (long) (t_code+1));
    varstr_free (vstr);
  }

  if (is_probabilized)
    varstr_free (wvstr);
 

  if (with_word_tree) {
    /* On n'utilise que dico_word_tree pour stocker le dico */
    Xforeach = Yforeach = 0;
  }
  else {
    Xforeach = 1;
    Yforeach = 0;
  }

  if (with_sxword) {
    sxword_alloc (&names, "names", inflected_form_nb, 1, inflected_form_total_char_nb/inflected_form_nb + 2, sxcont_malloc, sxcont_alloc, sxcont_realloc, NULL, sxcont_free, NULL,
#if EBUG
		  stdout
#else
		  NULL
#endif /* EBUG */
		  );
  }
  else {
    word_tree_alloc (&dico_word_tree, "dico_word_tree", inflected_form_nb, inflected_form_total_char_nb/inflected_form_nb + 2, Xforeach, Yforeach, (SXBOOLEAN) (!from_right_to_left) /* from_left_to_right */, with_id, oflw,
#if EBUG
		     stdout
#else
		     NULL
#endif /* EBUG */
		     );
    cur_word_tree_ptr = &dico_word_tree;
  }

  if (ste2id) {
    /* A chaque forme flechie est associe' une liste d'id */
    /* Si cette liste contient un seul id, il est associe' avec la ff
       Sinon, la liste est mise ds un XH (precedee du nb) et c'est l'identifiant de cet XH (decale de EOF_CODE dans le cas non probabilis�, et de max_ift_code dans le cas probabilis�)
       qui est associe' a la ff */
    SXINT     next, path;
    SXBA      id_set = NULL;

    if (case5) {
      XH_alloc (&XH_id_stack, "XH_id_stack", inflected_form_nb+1, 1, (next_struct_list [0].id/inflected_form_nb)+1+1/* le compte */, NULL, NULL);
      id_set = sxba_calloc (max_id+1);
    }

    ste = 0;

    while ((ste = sxba_scan (inflected_form_ste_set, ste)) > 0) {
      next = ste2id [ste];

      if (case4) {
	/* cas ou l'id est le numero d'occur de la ff */
	id = next;
      }
      else {
	if (next_struct_list [next].next == 0) {
	  id = next_struct_list [next].id;
	}
	else {
	  lgth = 0;

	  do {
	    id = next_struct_list [next].id;
	    if (SXBA_bit_is_reset_set (id_set, id))
	      lgth++;
	  } while ((next = next_struct_list [next].next) != 0);

	  if (lgth == 1)
	    /* id est d�j� positionn� */
	    SXBA_0_bit (id_set, id); /* on RAZ id_set qui ne contient que id */
	  else {
	    XH_push (XH_id_stack, lgth);
	    
	    id = 0;
	    
	    while ((id = sxba_scan_reset (id_set, id)) > 0)
	      XH_push (XH_id_stack, id);
	    
	    XH_set (&XH_id_stack, &id);
	    if (is_probabilized)
	      id = XH_X (XH_id_stack, id) + (max_ift_code + 1); /* Index ds ift_code_stack, delta */
	    else
	      id = XH_X (XH_id_stack, id) + (t_code + 1 /* i.e. EOF_CODE */); /* Index ds t_code_stack, delta */
	  }
	}
      }

      if (with_sxword) {
	path = sxword_2save (&names, sxstrget (ste), sxstrlen (ste));
      }
      else {
	path = word_tree_add_a_string (&dico_word_tree, sxstrget (ste), sxstrlen (ste), id);
      }
    }

    if (case5)
      sxfree (id_set);
  }


  if (!with_word_tree && !with_sxword)
    /* On remplit dico_sxdfa */
    word_tree2sxdfa (&dico_word_tree, &dico_sxdfa, "dico_sxdfa",
#if EBUG
		     stdout
#else
		     NULL
#endif /* EBUG */
		     , make_a_min_dag);

#if EBUG
  printf ("\n// *** word_nb = %ld, char_nb = %ld ***\n", inflected_form_nb, inflected_form_total_char_nb);
#endif /* EBUG */

  if (is_packed) {
    sxdfa2sxdfa_packed (&dico_sxdfa, &dico_sxdfa_packed, "dico_sxdfa_packed",
#if EBUG
			stdout
#else
			NULL
#endif /* EBUG */
			);
  }
  else {
    if (with_comb) {
      sxdfa2comb_vector (&dico_sxdfa, comb_kind, comb_vector_threshold, &dico);
      sxdfa_free (&dico_sxdfa);
    }
  }

  /* on analyse les mots pour verifier que ca marche */
  if (seek) {
    cur_word_nb = 0;
    ste = 0;

    while ((ste = sxba_scan (inflected_form_ste_set, ste)) > 0) {
      cur_word_nb++;
      lgth = sxstrlen (ste);
      ff = sxstrget (ste);

      if (with_word_tree)
	id = word_tree_seek_a_string (&dico_word_tree, ff, &lgth);
      else {
	if (with_sxword) {
	  id = sxword_2retrieve (&names, ff, lgth);
	}
	else {
	  if (with_comb) {
	    id = (SXINT) sxdfa_comb_seek_a_string (&dico, ff, &lgth);
	  }
	  else {
	    if (is_packed)
	      id = sxdfa_packed_seek_a_string (&dico_sxdfa_packed, ff, &lgth);
	    else
	      id = sxdfa_seek_a_string (&dico_sxdfa, ff, &lgth);
	  }

	  if (id < 0 || lgth > 0)
	    /* chaine non reconnue en entier */
	    id = 0;
	}
      }

      if (id == 0)
	sxtrap (ME, "make_a_dico");

      if (ste2id && ste2id [ste] != id) 
	sxtrap (ME, "make_a_dico");
    }
  }

  if (output_in_C) {
    gen_header (source_file_pathname);

    /* EOF_CODE est soit t_code+1, soit max_id+1, soit 0 */
    printf ("\n#define EOF_CODE %ld\n\n", t_code ? t_code+1 : (max_id ? max_id + 1 : 0));

    if (is_probabilized) {
      fputs ("\n#define is_probabilized_lexicon\n\n", stdout);
      printf ("\n#define MAX_IFT_CODE %ld\n\n", max_ift_code+1);
    }

    if (t_code) {
      SXINT                   t;
      struct word_tree_struct word_tree_struct_terminal2code;
      struct sxdfa_struct     sxdfa_struct_terminal2code;
      struct sxdfa_comb       sxdfa_comb_terminal2code;

      /* Les terminaux ont ete mis ds un sxword car on a eu besoin, a partir du code de retrouver la chaine */
      word_tree_alloc (&word_tree_struct_terminal2code, "word_tree_struct_terminal2code", t_code /* word_nb */, 16 /* word_lgth */, 1 /* Xforeach */, 0 /* Yforeach */,
		       SXTRUE /* from_left_to_right */, SXTRUE /* with_path2id */, oflw /* BS: �tait "NULL" */ /* void (*oflw) () */, NULL /* FILE *stats */);
      cur_word_tree_ptr = &word_tree_struct_terminal2code;

      /* On sort un dico des terminaux */
      for (ste = SXEMPTY_STE; ste <= max_ste_t_code; ste++) {
	t = terminal_ste2code [ste];

	if (t) {
	  word_tree_add_a_string (&word_tree_struct_terminal2code, sxstrget (ste), sxstrlen (ste), t);
	}
      }

      word_tree2sxdfa (&word_tree_struct_terminal2code, &sxdfa_struct_terminal2code, "sxdfa_struct_terminal2code", NULL /* FILE *stats */, SXTRUE /* to_be_minimized */);
      sxdfa2comb_vector (&sxdfa_struct_terminal2code, 0 /* optim_kind (comb simple) */, 10000 /* comb_vector_threshold */, &sxdfa_comb_terminal2code);
      
      fputs ("\n\n#ifdef def_sxdfa_comb_terminal2code", stdout);
      sxdfa_comb2c (&sxdfa_comb_terminal2code, stdout, "sxdfa_comb_terminal2code", SXTRUE /* is_static */);
      fputs ("#endif /* def_sxdfa_comb_terminal2code */\n", stdout);
      
      sxdfa_comb_free (&sxdfa_comb_terminal2code);

      vstr = varstr_alloc (32);

      fputs ("\n\n#ifdef def_t2string\n", stdout);
      fputs ("\n\n#define has_t2string\n\n", stdout);
      if (is_static) 
	printf ("static ");
      printf ("char *t2string [%ld] = {\n\
/* 0 */ \"\",\n", t_code+1);

      for (ste = SXEMPTY_STE; ste <= max_ste_t_code; ste++) {
	t = terminal_ste2code [ste];

	if (t) {
	  vstr = varstr_quote (varstr_raz (vstr), sxstrget (ste));
	  printf ("/* %ld */ %s,\n", t, varstr_tostr (vstr));
	}
      }

      fputs ("} /* End of t2string [] */;\n\
#endif /* def_t2string */\n", stdout);



      if (is_probabilized) {
	SXINT ift_code;
	
	fputs ("\n\n#ifdef def_ift_code2proba\n", stdout);
	fputs ("\n\n#define has_ift_code2proba\n\n", stdout);
	if (is_static) {
	  fputs ("\n\n#define ift_code2proba_is_static\n\n", stdout);
	  printf ("static ");
	}
	else
	  fputs ("/* Function to be used for checking consistency between the definition of SXFLOAT \
   in the library version and in the compliled version of the tables below.\
   Returns SXTRUE if SXFLOAT is float, SXFALSE if it is double */\
SXINT\
get_ift_code2proba_type (void) {\
  return SXFLOAT_TYPE_ID;\
}\n", stdout);
	printf ("SXFLOAT ift_code2proba [%ld] = {\n\
/* 0 */ 0.0,\n", max_ift_code+1);

	for (ift_code = 1; ift_code <= max_ift_code; ift_code++) {
	  printf ("/* %ld */ " SXFDLD ",\n", ift_code, ift_code2proba [ift_code]);
	}

	fputs ("} /* End of ift_code2proba [] */;\n",stdout);
	fputs ("#endif /* def_ift_code2proba */\n", stdout);

	fputs ("\n\n#ifdef def_ift_code2terminal\n", stdout);
	fputs ("\n\n#define has_ift_code2terminal\n\n", stdout);
	if (is_static) 
	  printf ("static ");
	printf ("SXINT ift_code2terminal [%ld] = {\n\
/* 0 */ 0,\n", max_ift_code+1);

	for (ift_code = 1; ift_code <= max_ift_code; ift_code++) {
	  printf ("/* %ld */ %ld,\n", ift_code, ift_code2terminal [ift_code]);
	}

	fputs ("} /* End of ift_code2terminal [] */;\n",stdout);
	fputs ("#endif /* def_ift_code2terminal */\n", stdout);
      }




      varstr_free (vstr);
    }

    fputs ("\n\n#ifdef def_sxdfa_comb_inflected_form\n", stdout);

    if (with_comb) {
      if (!case1 && case4 && t_code) {
	/* Il semble que ds ce cas on ff et t identiques !! */
	fputs ("\n\n#ifdef def_sxdfa_comb_terminal2code\n", stdout);
	//	printf ("#define sxdfa_comb_terminal2code %s\n", dico_name);
	printf ("#define %s sxdfa_comb_terminal2code\n", dico_name);
	fputs ("#else /* def_sxdfa_comb_terminal2code */\n", stdout);
	sxdfa_comb2c (&dico, stdout, dico_name, is_static);
	fputs ("#endif /* def_sxdfa_comb_terminal2code */\n", stdout);
      }
      else
	sxdfa_comb2c (&dico, stdout, dico_name, is_static);
    }
    else {
      if (!with_word_tree && !with_sxword) {
	/* On sort la structure */
	if (is_packed)
	  sxdfa_packed2c (&dico_sxdfa_packed, stdout, dico_name, is_static);
	else
	  sxdfa2c (&dico_sxdfa, stdout, dico_name, is_static);
      }
    }

    if (ste2id) {
      if (case5 && (top = XH_list_top (XH_id_stack)) > 1) {
	printf ("\nstatic SXINT %s [%ld] = {/* 0 top */ %ld,\n", is_probabilized ? "ift_code_stack" : "t_code_stack", top, top-1);

	for (id = 1; id < XH_top (XH_id_stack); id++) {
	  bot = XH_X (XH_id_stack, id);
	  top = XH_X (XH_id_stack, id+1);

	  printf ("/* id = %ld */ %ld", bot, XH_list_elem (XH_id_stack, bot) /* le nombre */);

	  while (++bot < top) {
	    printf (", %ld", XH_list_elem (XH_id_stack, bot));
	  }
      
	  fputs (",\n", stdout);
	}
	
	printf ("} /* End of %s */;\n\n", is_probabilized ? "ift_code_stack" : "t_code_stack");
      }
      else
	/* cas ou toutes les listes sont de taille 1 */
	printf ("\nstatic SXINT *%s;\n\n", is_probabilized ? "ift_code_stack" : "t_code_stack");
    }

    fputs ("#endif /* def_sxdfa_comb_inflected_form */\n", stdout);
  }

  if (terminal_ste2code) sxfree (terminal_ste2code), terminal_ste2code = NULL;
  if (ste2id && case5) XH_free (&XH_id_stack);

  if (ste2id) {
    sxfree (ste2id), ste2id = NULL;
    sxfree (next_struct_list), next_struct_list = NULL;
  }

  if (with_word_tree)
    word_tree_free (&dico_word_tree);
  else {
    if (with_sxword) {
      if (names.stat_file) {
	SXINT  ste_local;
	SXINT	i, strings_lgth = 0;

	for (i = 0; (SXUINT)i < names.hashsiz; i++) {
	  if ((ste_local = names.head [i]) != SXERROR_STE) {
	    do {
	      strings_lgth += SXWORD_len (names, ste_local);
	    } while ((ste_local = names.table [ste_local].collision_link) != SXERROR_STE);
	  }
	}
	
	fprintf (names.stat_file, "// Statistics for \"%s\": total size (in bytes) = %ld\n", names.area_name,
		 sizeof (sxword_header)
		 + sizeof (names.head [0]) * (names.hashsiz)
		 + sizeof (struct sxword_table) * (names.top + 1)
		 + sizeof (char) * (strings_lgth)
		 );
      }

      sxword_free (&names);
    }
    else {
      if (with_comb) {
	fprintf (stdout, "// Statistics for the comb_vector structure \"%s\": total size (in bytes) = %ld\n", dico_name,
		 sizeof (struct sxdfa_comb)
		 + sizeof (unsigned char) * 256
		 + sizeof (SXINT) * (dico.max + 1)
		 + (dico.char_stack_list_top ? sizeof (unsigned char) * (dico.char_stack_list_top + 1) : 0)
		 + (dico.char_stack_list_top ? sizeof (SXINT) * (dico.max + 1) : 0));

	sxdfa_comb_free (&dico);
      }
      else {
	if (is_packed)
	  sxdfa_packed_free (&dico_sxdfa_packed);
	else
	  sxdfa_free (&dico_sxdfa);
      }
    }
  }

  sxfree (inflected_form_ste_set), inflected_form_ste_set = NULL;
}


SXINT
make_a_dico_action (SXINT what, struct sxtables *arg)
{
  switch (what) {
  case SXOPEN:
    break;

  case SXINIT:
    init ();
    break;

  case SXACTION:
    action ((SXINT)arg);
    break;

  case SXERROR:
    is_error = SXTRUE;
    break;

  case SXFINAL:
    final ();
    break;

  case SXCLOSE:
  case SXSEMPASS:
    break;

  default:
    fputs ("The function \"udag_action\" is out of date with respect to its specification.\n", sxstderr);
    sxexit (SXSEVERITIES);
  }

  return 0;
}
