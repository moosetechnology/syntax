/* ********************************************************
   *							  *
   *							  *
   * Copyright (c) 2007 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *							  *
   *							  *
   ******************************************************** */




/* ********************************************************
   *							  *
   *  Produit de l'equipe ALPAGE.                    (pb) *
   *							  *
   ******************************************************** */





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


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 16-11-07 13:30 (pb):	Ajout de cette rubrique "modifications"	        */
/************************************************************************/


#define SX_GLOBAL_VAR
#define SX_DFN_EXT_VAR

#include "sxunix.h"

char WHAT_SXMAIN[] = "@(#)SYNTAX - $Id: make_a_dico.c 527 2007-05-09 14:02:47Z boullier $" WHAT_DEBUG;

static char ME [] = "make_a_dico.c";

#include "sxstack.h"
#include "XH.h"
#include "XxY.h"
#include "fsa.h"
#include "sxword.h"
#include <ctype.h>

#ifndef SX_GLOBAL_VAR
/* declarations indispensables pour Syntax V3.8 et interdites pour Syntax V6 */ 
/* On lit a priori sur stdin, et cetera */
FILE	*sxstdout, *sxstderr
BOOLEAN sxverbosep;
#endif

extern struct sxtables  sxtables;

static SXINT      word_nb, word_lgth;
static BOOLEAN    make_a_min_dag, with_id, with_word_tree, seek, with_sxword, from_right_to_left, with_comb, is_packed, output_in_C, is_static;
static SXINT      comb_kind, comb_vector_threshold;



/*---------------*/
/*    options    */
/*---------------*/

static char	Usage [] = "\
Usage:\t%s [options] [files]\n\
options=\n\
\t\t-wn #nb, -word_nb #nb,\n\
\t\t-wl #lgth, -word_lgth #lgth,\n\
\t\t-md, -min_dag, -nmd, -no_min_dag,\n\
\t\t-wi, -with_id,\n\
\t\t-wt, -word_tree,\n\
\t\t-s, -seek,\n\
\t\t-sxw, -sxword,\n\
\t\t-dn name, -dico_name name,\n\
\t\t-rl, -from_right_to_left,\n\
\t\t-c #kind, -comb #kind,\n\
\t\t-cvt #val, -comb_vector_threshold #val,\n\
\t\t-p, -packed,\n\
\t\t-C, -output_in_C, -nC,-no_output_in_C\n\
\t\t-e, -extern,\n\
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
#define HELP 		        ((SXINT)15)

static char	*option_tbl [] = {"", "wn", "word_nb", "wl", "word_lgth", "md", "min_dag", "nmd", "no_min_dag", "wi", "with_id", "nwi", "no_with_id", "wt", "word_tree", "s", "seek", "sxw", "sxword", "dn", "dico_name",
				  "rl", "from_right_to_left", "c", "comb", "cvt", "comb_vector_threshold", "p", "packed", "C", "output_in_C", "nC", "no_output_in_C", "e", "extern", "-h", "-help"};
static SXINT	option_kind [] = {UNKNOWN_ARG, WORD_NB, WORD_NB, WORD_LGTH, WORD_LGTH, MIN_DAG, MIN_DAG, -MIN_DAG, -MIN_DAG, WITH_ID, WITH_ID, -WITH_ID, -WITH_ID, WORD_TREE, WORD_TREE, SEEK, SEEK, SXWORD, SXWORD, DICO_NAME, DICO_NAME,
				  FROM_RIGHT_TO_LEFT, FROM_RIGHT_TO_LEFT, COMB_VECTOR, COMB_VECTOR, COMB_VECTOR_THRESHOLD, COMB_VECTOR_THRESHOLD, PACKED, PACKED, IN_C, IN_C, -IN_C, -IN_C, EXTERN, EXTERN, HELP, HELP};



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

static BOOLEAN                 case4, case5, case52, case53;
static FILE	               *infile;
static struct word_tree_struct dico_word_tree;
static struct sxdfa_struct     dico_sxdfa;
static struct sxdfa_packed_struct     dico_sxdfa_packed;
static sxword_header           names;
static SXINT                   *ste2id;
static struct sxdfa_comb       dico;
static char                    *source_file_pathname;
static struct next_struct {
  SXINT id, next;
} *next_struct_list;

static SXINT                   t_code, terminal_ste2code_size, inflected_form_ste_set_size, max_ste_t_code, max_id;
static SXINT                   *terminal_ste2code;
static BOOLEAN                 is_error;
static SXBA                    inflected_form_ste_set;


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
\t./make_a_dico.out -word_nb #%ld -word_lgth #%ld%s%s%s%s%s%s -output_in_C -dico_name %s %s\n\
\ton %s\
\tby the SYNTAX(*) make_a_dico processor\n\
   ********************************************************************\n\
\t(*) SYNTAX is a trademark of INRIA.\n\
   ******************************************************************** */\n\n\n",
	  word_nb,
	  word_lgth,
	  with_word_tree ? " -word_tree" : (with_sxword ? " -sxword" : (with_comb ? " -comb" : (is_packed ? " -packed" : ""))),
	  with_comb ? (comb_kind==0 ? " 0" : " 1") : "",
	  with_comb ? " -comb_vector_threshold " : "",
	  with_comb ? threshold_string : "",
	  with_id ? " -with_id" : "",
	  seek ? " -seek" : "",
	  dico_name,
	  file_name ? file_name : "[stdin]",
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
  word_tree_oflw (&dico_word_tree, old_size, new_size);
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

    while ((c = getchar ()) != EOF) {
      putc (c, infile);
    }

    rewind (infile);
    sxsrc_mngr (INIT, infile, "");
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

      sxsrc_mngr (INIT, infile, pathname);
    }
  }

  sxerr_mngr (BEGIN);
  syntax (ACTION, &sxtables);
  sxerr_mngr (END);
  fclose (infile);
  sxsrc_mngr (FINAL);
}



#if 0
static	void
make_a_dico (char *pathname)
{
  int	 c; /* vrai int */
  SXINT  path, id, trans, lgth, Xforeach, Yforeach, size, max_id;
  FILE	 *infile = NULL;
  VARSTR vstr;

  if (pathname == NULL) {
    if (sxverbosep) {
      fputs ("\"stdin\":\n", sxtty);
    }
  }
  else {
    if ((infile = sxfopen (pathname, "r")) == NULL) {
      fprintf (sxstderr, "%s: Cannot open (read) ", ME);
      sxperror (pathname);
      sxerrmngr.nbmess [2]++;
      return;
    }
    if (sxverbosep) {
      fprintf (sxtty, "%s:\n", pathname);
    }
  }

  vstr = varstr_alloc (4*word_lgth+1);

  if (with_word_tree) {
    /* On n'utilise que dico_word_tree pour stocker le dico */
    Xforeach = Yforeach = 0;
  }
  else {
    Xforeach = 1;
    Yforeach = 0;
  }

  if (pathname && seek && with_id)
    name2id = (SXINT*) sxalloc (word_nb+1, sizeof (SXINT)), name2id [0] = word_nb;
  else
    name2id = NULL;

  if (with_sxword) {
    sxword_alloc (&names, "names", word_nb, 1, word_lgth, sxcont_malloc, sxcont_alloc, sxcont_realloc, NULL, sxcont_free, NULL,
#if EBUG
		   stdout
#else
		   NULL
#endif /* EBUG */
		   );
  }
  else {
    word_tree_alloc (&dico_word_tree, "dico_word_tree", word_nb, word_lgth, Xforeach, Yforeach, !from_right_to_left /* from_left_to_right */, with_id, oflw,
#if EBUG
		     stdout
#else
		     NULL
#endif /* EBUG */
		     );
  }

  DALLOC_STACK (word_stack, 32);

  max_id = max_trans = cur_word_nb = max_word_lgth = char_nb = trans_nb = state_nb = final_state_nb = prev_state = 0;

  while ((c = read_a_char ()) != EOF) {
    DRAZ (word_stack);
    if (with_sxword) vstr = varstr_raz (vstr);

    while (c != '\n' && c != '\t') {
      if (c != '"') {
	/* Les quotes doivent etre echappes */
	if (c == '\\') {
	  /* Le '\' est le caractere d'echappement */
	  c = read_a_char ();
	}

	char_nb++;
	trans = (SXINT) c;
	DPUSH (word_stack, trans);
	if (with_sxword) vstr = varstr_catchar (vstr, (char) c);

	if (trans > max_trans)
	  max_trans = trans;
      }

      c = read_a_char ();
    }

    if (DTOP (word_stack) > max_word_lgth)
      max_word_lgth = DTOP (word_stack);

    cur_word_nb++;

    if (c == '\t') {
      id = 0;

      while (isdigit (c = read_a_char ())) {
	id *= 10;
	id += c-(int)'0';
      }

      while (c != '\n')
	/* suffixe eventuel pris comme des commentaires */
	c = read_a_char ();

      if (!with_id)
	id = 0;
    }
    else {
      if (with_id)
	id = cur_word_nb;
      else
	id = 0;
    }

    if (id) {
      if (id > max_id)
	max_id = id;
    }

    if (with_sxword) {
      vstr = varstr_complete (vstr);
      path = sxword_2save (&names, varstr_tostr (vstr), varstr_length (vstr));
      if (name2id) name2id [path] = id;
    }
    else {
      path = word_tree_add_a_word (&dico_word_tree, word_stack, id);
    }
      
    if (name2id) {
      if (cur_word_nb > name2id [0]) {
	size = name2id [0] *= 2;
	name2id = (SXINT*) sxrealloc (name2id, size+1, sizeof (SXINT));
      }

      name2id [cur_word_nb] = id;
    }
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
  printf ("\n// *** word_nb = %ld, char_nb = %ld ***\n", cur_word_nb, char_nb);
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
  if (pathname && seek) {
    /* Tant pis si on a lu sur stdin */
    rewind (infile);
    cur_word_nb = 0;

    while ((c = read_a_char ()) != EOF) {
      vstr = varstr_raz (vstr);

      while (c != '\n' && c != '\t') {
	if (c == '\\') {
	  /* Le '\' est le caractere d'echappement */
	  c = read_a_char ();
	}

	char_nb++;

	vstr = varstr_catchar (vstr, (char) c);

	c = read_a_char ();
      }

      vstr = varstr_complete (vstr);

      lgth = varstr_length (vstr);

      cur_word_nb++;

      if (with_word_tree)
	id = word_tree_seek_a_string (&dico_word_tree, varstr_tostr (vstr), &lgth);
      else {
	if (with_sxword) {
	  id = sxword_2retrieve (&names, varstr_tostr (vstr), lgth);
	}
	else {
	  if (with_comb) {
	    char *string = varstr_tostr (vstr);
	    id = (SXINT) sxdfa_comb_seek_a_string (&dico, string, &lgth);
	  }
	  else {
	    if (is_packed)
	      id = sxdfa_packed_seek_a_string (&dico_sxdfa_packed, varstr_tostr (vstr), &lgth);
	    else
	      id = sxdfa_seek_a_string (&dico_sxdfa, varstr_tostr (vstr), &lgth);
	  }

	  if (id < 0 || lgth > 0)
	    /* chaine non reconnue en entier */
	    id = 0;
	}
      }

      if (id == 0)
	sxtrap (ME, "make_a_dico");

      if (name2id && name2id [cur_word_nb] != id) 
	sxtrap (ME, "make_a_dico");

      if (c == '\t') {
	while ((c = read_a_char ()) != '\n');
      }
    }
  }

  if (name2id) sxfree (name2id), name2id = NULL;

  if (output_in_C) {
    gen_header (pathname);

    /* EOF_VAL est soit le +grand id + 1, soit 0 */
    printf ("\n#define EOF_VAL %ld\n\n", with_id ? max_id+1 : 0);

    if (with_comb) {
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
  }

  if (with_word_tree)
    word_tree_free (&dico_word_tree);
  else {
    if (with_sxword) {
      if (names.stat_file) {
	SXUINT  ste;
	SXINT	i, strings_lgth = 0;

	for (i = 0; (SXUINT)i < names.hashsiz; i++) {
	  if ((ste = names.head [i]) != SXERROR_STE) {
	    do {
	      strings_lgth += SXWORD_len (names, ste);
	    } while ((ste = names.table [ste].collision_link) != SXERROR_STE);
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
		 + sizeof (SXINT) * (dico.max+1));

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
  
  if (pathname)
    fclose (infile);

  DFREE_STACK (word_stack);
  varstr_free (vstr);
}
#endif /* 0 */


int main (int argc, char *argv[])
{
    sxstdout = stdout;
    sxstderr = stderr;
    sxverbosep = TRUE;

    sxuse (prev_state);

#if BUG
    /* Suppress bufferisation, in order to have proper	 */
    /* messages when something goes wrong...		 */
    setbuf (stdout, NULL);
#endif

    sxopentty ();

    /* Valeurs par defaut */
    word_nb = 100;
    word_lgth = 16;
    with_word_tree = seek = with_sxword = from_right_to_left = is_packed = FALSE;
    with_id = with_comb = make_a_min_dag = is_static = output_in_C = TRUE;
    comb_kind = 0;
    comb_vector_threshold = 10000;
    dico_name = "sxdfa_comb_inflected_form";

    sxopentty ();
    sxstr_mngr (BEGIN) /* Creation de la table des chaines */ ;
    (*(sxtables.analyzers.parser)) (BEGIN) /* Allocation des variables globales du parser */ ;
    syntax (OPEN, &sxtables) /* Initialisation de SYNTAX (mono-langage) */ ;

    {
      /* Tous les arguments sont des noms de fichiers, "-" signifiant stdin */
      int	argnum;
      BOOLEAN   has_unknown_arg = FALSE;

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
	  make_a_min_dag = TRUE;
	  
	  break;

	case -MIN_DAG:
	  make_a_min_dag = FALSE;
	  
	  break;

	case WITH_ID:
	  with_id = TRUE;

	  break;

	case -WITH_ID:
	  with_id = FALSE;

	  break;

	case WORD_TREE:
	  with_word_tree = TRUE;
	  with_sxword = with_comb = FALSE;
	  break;

	case SEEK:
	  seek = TRUE;
	  break;

	case SXWORD :
	  with_sxword = TRUE;
	  with_word_tree = with_comb = FALSE;
	  break;
	  
	case DICO_NAME:
	  if (++argnum >= argc)
	    fprintf (sxstderr, "%s: a dictionary name must follow the \"%s\" option;\n\tvalue %s is retained.\n", ME
		     , option_get_text (DICO_NAME), dico_name);

	  dico_name = argv [argnum];
	  break;

	case FROM_RIGHT_TO_LEFT:
	  from_right_to_left = TRUE;
	  break;

	case COMB_VECTOR:
	  if (++argnum >= argc || !sxnumarg (argv [argnum], &comb_kind))
	    fprintf (sxstderr, "%s: an optimization kind number must follow the \"%s\" option;\n\tvalue %ld is retained.\n", ME
		     , option_get_text (COMB_VECTOR), comb_kind);

	  with_comb = TRUE;
	  with_word_tree = with_sxword = FALSE;
	  break;

	case COMB_VECTOR_THRESHOLD:
	  if (++argnum >= argc || !sxnumarg (argv [argnum], &comb_vector_threshold))
	    fprintf (sxstderr, "%s: a number must follow the \"%s\" option;\n\tvalue %ld is retained.\n", ME
		     , option_get_text (COMB_VECTOR_THRESHOLD), comb_vector_threshold);
	  break;

	case PACKED:
	  is_packed = TRUE;
	  break;

	case IN_C:
	  output_in_C = TRUE;
	  break;

	case -IN_C:
	  output_in_C = FALSE;
	  break;

	case EXTERN:
	  is_static = FALSE;
	  break;

	case HELP:
	  fprintf (sxstderr, "%s", Usage);
	  return EXIT_SUCCESS;	  

	case UNKNOWN_ARG:
	  has_unknown_arg = TRUE;

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

    syntax (CLOSE, &sxtables);
    (*(sxtables.analyzers.parser)) (END) /* Inutile puisque le process va etre termine */ ;
    sxstr_mngr (END) /* Inutile puisque le process va etre termine */ ;

    return EXIT_SUCCESS;
}



VOID	sxvoid (void)
/* procedure ne faisant rien */
{
}

BOOLEAN	sxbvoid (void)
/* procedure ne faisant rien */
{
    return 0;
}



void
make_a_dico_scanact (SXINT entry, SXINT act_no)
{
  switch (entry) {
    case OPEN:
    case CLOSE:
    case INIT:
    case FINAL:
      sxincl_mngr (entry);
      return;

    case ACTION:
      switch (act_no) {
      case 1:
	/* The pathname of the include file is in token_string */
	if (sxpush_incl (sxsvar.sxlv_s.token_string))
	  /* Now, the source text comes from the include file ... */
	  return;

	/* something failed (unable to open, recursive call, ...) */
	/* Error message */
	sxput_error (sxsvar.sxlv.terminal_token.source_index /* Source coordinates of the include command */,
		     "%sUnable to process the include file \"%s\".",
		     sxsvar.sxtables->err_titles [2] /* Severity level: Error */,
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



static VOID
init (void)
{
  is_error = FALSE;
  t_code = 0;

  if (with_id) {
    max_ste_t_code = 0;
    terminal_ste2code_size = 256;
    terminal_ste2code = (SXINT *) sxcalloc (terminal_ste2code_size+1, sizeof (SXINT));
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
}


static VOID
action (SXINT action_no)
{
  SXINT	         path, ste, len, top;
  struct sxtoken *token_ptr;
  static SXINT   id;

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
    if (with_id) {
      token_ptr = &(SXSTACKtoken (SXSTACKtop ()));
      ste = token_ptr->string_table_entry; // récupère le symbole le plus à droite (ici, le terminal %MOT)

      if (ste > terminal_ste2code_size) {
	terminal_ste2code = (SXINT *) sxrecalloc (terminal_ste2code, terminal_ste2code_size+1, 2*terminal_ste2code_size+1, sizeof (SXINT));
	terminal_ste2code_size *= 2;
      }

      if (terminal_ste2code [ste] == 0) {
	terminal_ste2code [ste] = ++t_code;
	max_ste_t_code = ste;
      }
      else {
	sxput_error (token_ptr->source_index,
		     "%sMultiple occurrence of the terminal symbol \"%s\".",
		     sxsvar.sxtables->err_titles [1] /* Severity level: Warning */,
		     sxstrget (ste)
		     );
      }
    }

    return;

  case 2:
    /* <inflected_forme_code>	= <terminal>					; 2 */
    /* Utilisation d'un terminal */
    if (ste2id) {
      case52 = TRUE;
      token_ptr = &(SXSTACKtoken (SXSTACKtop ()));
      ste = token_ptr->string_table_entry;
      id = terminal_ste2code [ste];

      if (id == 0 || case53) {
	if (id == 0)
	  sxput_error (token_ptr->source_index,
		       "%sUnknown terminal symbol \"%s\", line is ignored.",
		       sxsvar.sxtables->err_titles [1] /* Severity level: Warning */,
		       sxstrget (ste)
		       );
	else {
	  sxput_error (token_ptr->source_index,
		       "%sTerminal symbol \"%s\" and integer values cannot be mixed, line is ignored.",
		       sxsvar.sxtables->err_titles [1] /* Severity level: Warning */,
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

    break;

  case 3:
    /* <inflected_forme_code>	= %ENTIER					; 3 */
    if (ste2id) {
      case53 = TRUE;
      token_ptr = &(SXSTACKtoken (SXSTACKtop ()));
      ste = token_ptr->string_table_entry;
      id = atoi (sxstrget (ste));

      if (case52) {
	  sxput_error (token_ptr->source_index,
		       "%sInteger value \"%i\" and terminal symbols cannot be mixed, line is ignored.",
		       sxsvar.sxtables->err_titles [1] /* Severity level: Warning */,
		       id
		       );
	  id = 0;
      }
    }

    break;

  case 4:
    /* <inflected_form_def>    = <inflected_form>				; 4 */
    /* Ici, si with_id, chaque ff recoit un id different */
    case4 = TRUE;
    token_ptr = &(SXSTACKtoken (SXSTACKtop ()));
    ste = token_ptr->string_table_entry;


    if (case5) {
	sxput_error (token_ptr->source_index,
		     "%sThis inflected form \"%s\" must possess an id spec, it is ignored.",
		     sxsvar.sxtables->err_titles [1] /* Severity level: Warning */,
		     sxstrget (ste)
		     );
    }
    else {
      if (ste > inflected_form_ste_set_size) {
	inflected_form_ste_set_size *= 2;
	inflected_form_ste_set = sxba_resize (inflected_form_ste_set, inflected_form_ste_set_size+1);

	if (ste2id)
	  ste2id = (SXINT *) sxrealloc (ste2id, inflected_form_ste_set_size+1, sizeof (SXINT));
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
      }
      else
	sxput_error (token_ptr->source_index,
		     "%sMultiple occurrence of the inflected_form \"%s\".",
		     sxsvar.sxtables->err_titles [1] /* Severity level: Warning */,
		     sxstrget (ste)
		     );
    }

    break;

  case 5:
    /* <inflected_form_def>    = <inflected_form> <inflected_forme_code>	; 5 */
    case5 = TRUE;
    token_ptr = &(SXSTACKtoken (SXSTACKtop () - 1));
    ste = token_ptr->string_table_entry;

    if (case4) {
	sxput_error (token_ptr->source_index,
		     "%sThis inflected form \"%s\" must not possess an id spec, it is ignored.",
		     sxsvar.sxtables->err_titles [1] /* Severity level: Warning */,
		     sxstrget (ste)
		     );
    }
    else {
      if (ste > inflected_form_ste_set_size) {
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
	  next_struct_list [top].id = id;
	  next_struct_list [top].next = 0;
	}
      }
      else {
	if (ste2id) {
	  next_struct_list [top].id = id;
	  next_struct_list [top].next = ste2id [ste];
	  ste2id [ste] = top;
	}
	else
	  sxput_error (token_ptr->source_index,
		       "%sMultiple occurrence of the inflected_form \"%s\".",
		       sxsvar.sxtables->err_titles [1] /* Severity level: Warning */,
		       sxstrget (ste)
		       );
      }
    }

    break;

  default:
    fputs ("The function \"action\" is out of date with respect to its specification.\n", sxstderr);
    sxexit (SEVERITIES);
  }
}


static VOID
final (void)
{
  SXINT     lgth, ste, id, top, bot;
  SXINT     Xforeach, Yforeach;
  char      *ff;
  XH_header XH_id_stack;

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
    word_tree_alloc (&dico_word_tree, "dico_word_tree", inflected_form_nb, inflected_form_total_char_nb/inflected_form_nb + 2, Xforeach, Yforeach, !from_right_to_left /* from_left_to_right */, with_id, oflw,
#if EBUG
		     stdout
#else
		     NULL
#endif /* EBUG */
		     );
  }

  if (ste2id) {
    /* A chaque forme flechie est associe' une liste d'id */
    /* Si cette liste contient un seul id, il est associe' avec la ff
       Sinon, la liste est mise ds un XH (precedee du nb) et c'est l'identifiant de cet XH (decale de EOF_VAL)
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
	    lgth++;
	    id = next_struct_list [next].id;
	    SXBA_1_bit (id_set, id);
	  } while ((next = next_struct_list [next].next) != 0);

	  XH_push (XH_id_stack, lgth);

	  id = 0;

	  while ((id = sxba_scan_reset (id_set, id)) > 0)
	    XH_push (XH_id_stack, id);
	    
	  XH_set (&XH_id_stack, &id);
	  id = XH_X (XH_id_stack, id)+t_code+1; /* Index ds t_code_stack, delta */
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

    /* EOF_VAL est soit t_code+1, soit 0 */
    printf ("\n#define EOF_CODE %ld\n\n", t_code ? t_code+1 : 0);

    if (t_code) {
      SXINT                   t;
      struct word_tree_struct word_tree_struct_terminal2code;
      struct sxdfa_struct     sxdfa_struct_terminal2code;
      struct sxdfa_comb       sxdfa_comb_terminal2code;

      /* Les terminaux ont ete mis ds un sxword car on a eu besoin, a partir du code de retrouver la chaine */
      word_tree_alloc (&word_tree_struct_terminal2code, "word_tree_struct_terminal2code", t_code /* word_nb */, 16 /* word_lgth */, 1 /* Xforeach */, 0 /* Yforeach */,
		       TRUE /* from_left_to_right */, TRUE /* with_path2id */, NULL /* void (*oflw) () */, NULL /* FILE *stats */);

      /* On sort un dico des terminaux */
      for (ste = EMPTY_STE; ste <= max_ste_t_code; ste++) {
	t = terminal_ste2code [ste];

	if (t) {
	  word_tree_add_a_string (&word_tree_struct_terminal2code, sxstrget (ste), sxstrlen (ste), t);
	}
      }

      word_tree2sxdfa (&word_tree_struct_terminal2code, &sxdfa_struct_terminal2code, "sxdfa_struct_terminal2code", NULL /* FILE *stats */, TRUE /* to_be_minimized */);
      sxdfa2comb_vector (&sxdfa_struct_terminal2code, 0 /* optim_kind (comb simple) */, 10000 /* comb_vector_threshold */, &sxdfa_comb_terminal2code);

      fputs ("\n\n#ifdef def_sxdfa_comb_terminal2code", stdout);
      sxdfa_comb2c (&sxdfa_comb_terminal2code, stdout, "sxdfa_comb_terminal2code", TRUE /* is_static */);
      fputs ("#endif /* def_sxdfa_comb_terminal2code */\n", stdout);

      sxdfa_comb_free (&sxdfa_comb_terminal2code);
    }
  
    if (terminal_ste2code) sxfree (terminal_ste2code), terminal_ste2code = NULL;

    fputs ("\n\n#ifdef def_sxdfa_comb_inflected_form", stdout);

    if (with_comb) {
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
  }

  if (ste2id) {
    if (case5 && (top = XH_list_top (XH_id_stack)) > 1) {
      printf ("\n\nstatic SXINT t_code_stack [%ld] = {/* 0 top */ %ld,\n", top, top-1);

      for (id = 1; id < XH_top (XH_id_stack); id++) {
	bot = XH_X (XH_id_stack, id);
	top = XH_X (XH_id_stack, id+1);

	printf ("/* id = %ld */ %ld", bot, XH_list_elem (XH_id_stack, bot) /* le nombre */);

	while (++bot < top) {
	  printf (", %ld", XH_list_elem (XH_id_stack, bot));
	}
      
	fputs (",\n", stdout);
      }
	
      fputs ("} /* End of t_code_stack */;\n\n", stdout);
    }
    else
      /* cas ou toutes les listes sont de taille 1 */
      fputs ("\n\nstatic SXINT *t_code_stack;\n\n", stdout);

    if (case5)
      XH_free (&XH_id_stack);
  }

  fputs ("#endif /* def_sxdfa_comb_inflected_form */\n", stdout);

  if (ste2id) {
    sxfree (ste2id), ste2id = NULL;
    sxfree (next_struct_list), next_struct_list = NULL;
  }

  if (with_word_tree)
    word_tree_free (&dico_word_tree);
  else {
    if (with_sxword) {
      if (names.stat_file) {
	SXUINT  ste_local;
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
		 + sizeof (SXINT) * (dico.max+1));

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
  case OPEN:
    break;

  case INIT:
    init ();
    break;

  case ACTION:
    action ((SXINT)arg);
    break;

  case ERROR:
    is_error = TRUE;
    break;

  case FINAL:
    final ();
    break;

  case CLOSE:
  case SEMPASS:
    break;

  default:
    fputs ("The function \"udag_action\" is out of date with respect to its specification.\n", sxstderr);
    sxexit (SEVERITIES);
  }

  return 0;
}
