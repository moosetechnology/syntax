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

/*** THIS FILE IS NOT PART OF THE SYNTAX LIBRARY libsx.a ***/ 

/* ********************************************************
   *                                                      *
   *     Produit de l'equipe ATOLL.			  *
   *                                                      *
   ******************************************************** */

/* Le couple dag_scanner/read_a_dag simule un scanner non-deterministe pour un analyseur Earley.
   Ce scanner non-deterministe est en fait un analyseur syntax dont la semantique est traitee par arbre abstrait.
   Techniquement, read_a_dag est la passe semantique (parcours d'un arbre abstrait) associe a un scanner/parser
   qui prend en entree un DAG.  Les tables de ce scanner/parser sont ds l'include dag_t.h qui sont une copie
   de dag_t.c produite par l'autogenese de $SX/DAG.  read_a_dag est appele depuis dag_scanner.c qui est
   ds la librairie SYNTAX en $SX/src, qui il doit etre recompilé par l'utilisateur, qui doit l'instancier
   en lui fournissant un certain nombre de donnees additionnelles (dictionnaire par exemple).
   C'est le main associe a dag_scanner qui se charge du src_mngr qui lit le DAG d'entree.
   L'argument de read_a_dag est la procedure store_dag qui se charge (sur chaque token reconnu par dag_scanner)
   de stocker sous forme interne le DAG reconnu par read_a_dag.
   Cela etant fait, on peut faire l'appel a Earley */


/* Prend en entree la ligne source d'un fichier representant un DAG de formes flechies (operateurs concat, | et []).
   et realise le scanning de cette ligne.  Le resultat est principalement une structure XxYxZ ou chaque triplet
   (lpos, X, rpos) est tel que 0 <= lpos < rpos <= final_pos et X designe des terminaux.
   Si X < 0, -X est le code d'un terminal
   Si X > 0, X designe un ensemble de codes de terminaux (comme pour un sdag)
   Si X == 0, chaine vide ou mot inconnu.
   La correspondance forme-flechie <--> ensemble de terminaux est decrite par dico.h.
   Si une forme-flechie n'est pas ds le dico, on peut appliquer des regles par defaut.
   On s'est inspire de lexer_semact.c.
*/

/* 17/01/08
   Lecture a partir d'un udag 
   pas d'option UNFOLD
   Cette nouvelle version utilise des dag construits par fsa_mngr
   Les dico eventuels sont des sxdfa_comb et il peut y en avoir plusieurs */


/* Il est inde'pendant du parseur associe' */


static char	ME [] = "udag_scanner.c";

#include "sxversion.h"
#include "sxunix.h"
#include "XxYxZ.h"
#include "udag_scanner.h"
#include "varstr.h"
#include <math.h>

VARSTR cur_input_vstr; /* inutilisée, sauf si on se recompile le sxsrc_mngr avec -Dsxgetchar pour
			  redéfinir sxgetchar de façon à remplir cette variable (c'est le cas
			  dans sxpipe/dag2dag (patterns_semact.c)) */

char WHAT_UDAG_SCANNER[] = "@(#)SYNTAX - $Id: udag_scanner.c 3633 2023-12-20 18:41:19Z garavel $" WHAT_DEBUG;

/* Ds le cas dag_kind==DAG_KIND */
#include "fsa.h" /* Pour read_a_re () */
/* read_a_re est declare ds fsa.h */

/* Ds le cas dag_kind==UDAG_KIND ou dag_kind==STRING_KIND */
extern SXINT read_a_udag (void (*store)() /* store_dag */, void (*pre_fill)(), void (*fill)(),
			   SXINT  (*check)(), void (*raz)() /* raz_dag */, SXINT (*parser)());

/* Ds le cas dag_kind==SDAG_KIND */
extern SXINT read_a_sdag (void (*store)(SXINT, SXINT, SXINT) /* store_sdag */,
			  void (*pre_fill)(void), 
			  void (*fill)(void),
			  SXINT  (*check)(void), 
			  void (*raz)(void) /* raz_sdag */, 
			  SXINT (*parser)(SXINT what_to_do, struct sxtables *arg));
extern SXINT *sdag_get_t_tok_no_stack (SXINT id);


#define TOK_NB ((sizeof (tok_str)/sizeof (tok_str [0])))

#ifdef dummy_tables
extern struct sxtables dummy_tables;
#endif /* dummy_tables */
static struct sxtables *tables;
 
/* Pour rendre udag_scanner independant de earley.h */
extern SXUINT          maximum_input_size; /* ds earley_parser par exemple */
extern SXINT	       n /* nb de token du source !! */;
extern bool       is_print_time;


#ifdef tdef_by_dico
#undef tdef_by_dico
#endif /* tdef_by_dico */

#ifdef no_dico_lecl
#undef no_dico_lecl /* prudence */
#endif /* no_dico_lecl */

#ifdef def_sxdfa_comb_inflected_form
#undef def_sxdfa_comb_inflected_form /* prudence */
#endif /* def_sxdfa_comb_inflected_form */

#ifdef keywords_h
   /* Les tables keywords_h proviennent directement de l'execution de bnf avec l'option -huge
      La grammaire n'a presque rec,ue aucun traitement.  Les terminaux de la grammaire sont les formes flechies */
   /* Cet include contient 
      #define HUGE_CFG
   */
#  define tdef_by_dico
#  define def_sxdfa_comb_inflected_form
#  include keywords_h
   /* Ici on a un dico terminal --> code (du terminal) et EOF_CODE est defini */
#  define dico_contains_terminals 0
   static struct sxdfa_comb *dico_des_terminaux = &sxdfa_comb_inflected_form;
   /* Dans ce cas t_code_stack n'a pas ete produit, pour que c,a compile ... */
   static SXINT *t_code_stack;
#else
#  ifdef lex_lfg_h
   /* Les tables lex_lfg_h proviennent de l'execution de lfg_lex_smp, elles permettent de faire la correspondance
      entre une ff et une liste de terminaux.
   */
#    define tdef_by_dico
#    define def_sxdfa_comb_inflected_form
#    define def_sxdfa_comb_terminal2code

#    define def_if_id2tok_list_id
#    define def_tok_disp
#    define def_tok_list
#    define def_tok_str

#    include lex_lfg_h
     /* Ici on a un dico formes_flechies --> code (permettant de recuperer entre autre la liste des terminaux associes) */
     /* Ici on a un dico nom du terminal --> code (du terminal) et EOF_CODE est defini */
#    define dico_contains_terminals 1
     static struct sxdfa_comb *dico_des_terminaux = &sxdfa_comb_terminal2code;
#  else /* lex_lfg_h */
#    ifdef sxdfa_comb_dico_h
       /* Les tables sxdfa_comb_dico_h proviennent de make_a_dico (ou assimile') ; EOF_CODE est défini, ainsi qu'un dico formes_flechies --> id */
#      define tdef_by_dico
#      define def_t2string
#      define def_ift_code2terminal
#      define def_ift_code2proba
#      undef EOF_CODE
#      undef MAX_IFT_CODE
#      define def_sxdfa_comb_terminal2code
#      define def_sxdfa_comb_inflected_form
#      include sxdfa_comb_dico_h
#      if (EOF_CODE == 0)
#        error EOF_CODE is 0 in the included dico
#      endif /* (EOF_CODE == 0) */
#      ifdef has_t2string
          /* Ici on a un dico terminal --> code du terminal
	  ... et un tableau code de t --> chaine
          Si id <= EOF_CODE, c'est le code du (seul) terminal associe' a la ff
	  Sinon t_code_stack + (id-EOF_CODE) est une stack qui contient la liste des terminaux associes a ff */
#        define dico_contains_terminals 1
         static struct sxdfa_comb *dico_des_terminaux = &sxdfa_comb_terminal2code;
#      else
#        define dico_contains_terminals 0
         static struct sxdfa_comb *dico_des_terminaux = NULL;
#      endif /* (EOF_CODE > 0) */
#    else /* sxdfa_comb_dico_h */
     /* on n'a pas de dico (anciennement -Dno_dico)*/
#      define no_dico_lecl
#      define EOF_CODE sxeof_code (tables)
#      define dico_contains_terminals 0
       static struct sxdfa_comb *dico_des_terminaux = NULL;
#    endif /* sxdfa_comb_dico_h */
#  endif /* lex_lfg_h */
#endif /* keywords_h */




/* Si #ifdef tdef_by_dico alors la structure qui definit le dico est un sxdfa_comb de nom sxdfa_comb_inflected_form */

#ifdef tdef_by_dico
#  ifdef sxdfa_comb_dico2_h
     /* les ff peuvent aussi se chercher ds un 2eme dico fait par make_a_dico (ou assimile') */
#    define MAX_DICO_NB 2
#    include sxdfa_comb_dico2_h
     /* la structure qui definit ce dico secondaire est un sxdfa_comb de nom sxdfa_comb_inflected_form2 */
#  else
#    define MAX_DICO_NB 1
#  endif /* sxdfa_comb_dico2_h */

static struct sxdfa_comb *dicos [MAX_DICO_NB] = {
  &sxdfa_comb_inflected_form
#  ifdef sxdfa_comb_dico2_h
  , &sxdfa_comb_inflected_form2
#  endif /* sxdfa_comb_dico2_h */
};

static SXINT ciu_name_ids [MAX_DICO_NB], lciu_name_ids [MAX_DICO_NB], digits_name_ids [MAX_DICO_NB];
#else
#  define MAX_DICO_NB 0
#endif /* tdef_by_dico */


static SXINT      End_Of_Sentence_ste /* ste de la transition bidon vide qui peut conduire d'un etat qcq vers l'etat final */;
static char       *wstr;
static SXINT      wstr_lgth;
static bool  input_is_a_dag;
static VARSTR     vstr;

/* Toujours visible car sxspf_mngr.c en a besoin !! (pas terrible, A VOIR) */
SXINT  SEMLEX_lahead = -1;

SXINT
get_SEMLEX_lahead ()
{
  return SEMLEX_lahead;
}


#ifdef tdef_by_dico
/* On fait une recherche ds sxdfa_comb_inflected_form */

#ifdef mp_h
#  define def_mp_t_rev_table
#  define def_mp_t_table
#  include mp_h
#  define TOK_LIST(t)       mp_t_rev_table [tok_list [t]]
/* tok_list rend un vieux terminal */
#else /* mp_h */
#  define TOK_LIST(t)       tok_list [t]
#endif /* mp_h */



static char       *terminal_component_string;
static SXINT      terminal_component_string_lgth;

static SXINT
comb_seek (struct sxdfa_comb *sxdfa_comb_ptr, char *str, SXINT lgth, SXINT *tok)
{  
  SXINT   cur_lgth, id, t_id;
  char    c, *cur_str, *ret_str, *cur_wstr;
  bool has_changed;
  
  cur_str = str;
  cur_lgth = lgth;
  id = sxdfa_comb_seek_a_string (sxdfa_comb_ptr, str, &cur_lgth);

  if (cur_lgth != 0)
    id = 0;
  
  /* On applique les defauts */
  if (id == 0 && (default_set & CHECK_LOWER)) {
    /* On regarde si le mot en minuscule existe */
    cur_str = str + lgth;

    if (lgth > wstr_lgth) {
      wstr = (char*) sxrealloc (wstr, (wstr_lgth = lgth)+1, sizeof (char));
    }

    cur_wstr = wstr + lgth;
    has_changed = false;

    while (--cur_str >= str) {
      *--cur_wstr = c = sxtolower (*cur_str);

      if (c != *cur_str)
	has_changed = true;
    }

    if (has_changed) {
      cur_str = wstr;
      cur_lgth = lgth;
      id = sxdfa_comb_seek_a_string (sxdfa_comb_ptr, cur_str, &cur_lgth);

      if (cur_lgth != 0)
	id = 0;
    }
  }

  if (id == 0 && (default_set & CHECK_UPPER)) {
    /* On regarde si le mot en majuscule existe */
    cur_str = str + lgth;

    if (lgth > wstr_lgth) {
      wstr = (char*) sxrealloc (wstr, (wstr_lgth = lgth)+1, sizeof (char));
    }

    cur_wstr = wstr + lgth;
    has_changed = false;

    while (--cur_str >= str) {
      *--cur_wstr = c = sxtoupper (*cur_str);

      if (c != *cur_str)
	has_changed = true;
    }

    if (has_changed) {
      cur_str = wstr;
      cur_lgth = lgth;
      id = sxdfa_comb_seek_a_string (sxdfa_comb_ptr, cur_str, &cur_lgth);

      if (cur_lgth != 0)
	id = 0;
    }
  }

  if (id == 0 && dico_des_terminaux) {
    /* On regarde si la ff est de la forme pref__suff */
    /* ... uniquement si on dispose d'un dico des terminaux */
    if (cur_str = strstr (str, "__")) {
      /* On prend le + a droite pour permettre des "le__det__det" */
      /* Attention, on peut avoir anti-___nc */
      while (ret_str = strstr (cur_str+1, "__")) {
	cur_str = ret_str;
      }
	
      /* Ici cur_str pointe vers (le dernier) "__suff" */
      cur_lgth = cur_str-str;
      
      id = sxdfa_comb_seek_a_string (sxdfa_comb_ptr, str, &cur_lgth);

      if (cur_lgth != 0)
	id = 0;

      if (id) {
	/* Ici pref est une forme flechie */
	cur_str += 2;
	cur_lgth = lgth-(cur_str-str);
	  
	if (cur_str [cur_lgth - 1] != '*') {
	  t_id = sxdfa_comb_seek_a_string (dico_des_terminaux, cur_str, &cur_lgth);

	  if (cur_lgth != 0)
	    t_id = 0;
	      
#ifdef mp_h
	  t_id = mp_t_rev_table [t_id];
#endif /* mp_h */
	      
	  if (t_id == 0)
	    id = 0;
	  else
	    *tok = t_id; /* Code du terminal */
	}
	else {
	  terminal_component_string = cur_str;
	  terminal_component_string_lgth = cur_lgth;
	  *tok = -1; // cas mot_terminalPrefix* : on met -1 dans tok, et on reporte à plus tard le test
	}
      }
    }
  }

  return id;
}


static SXINT
string2dico_id (SXINT which_dico, char *str, SXINT lgth, SXINT *tok)
{
  SXINT   id;
  char    first_char, first_char_lc, first_char_uc;
  char    *where;
  bool is_capitalized_initial;

  id = comb_seek (dicos [which_dico], str, lgth, tok);

  if (id == 0 && digits_name_ptr) {
    /* On regarde si le mot ne contient que des chiffres */
    if (strtol (str, &where, 10) && where == str+lgth) {
      /* C'est bien un entier */
      id = digits_name_ids [which_dico];
    }
  }

  if (id == 0 && (ciu_name_ptr || lciu_name_ptr)) {
    first_char = *str;
    first_char_lc = sxtolower (first_char);
    first_char_uc = sxtoupper (first_char);
    is_capitalized_initial = first_char_lc != first_char;

    if (is_capitalized_initial && ciu_name_ptr) {
      /* Le mot inconnu commence par une majuscule et il y a un defaut pour les mots en majuscule */
      id = ciu_name_ids [which_dico];
    }
    else {
      if (!is_capitalized_initial && lciu_name_ptr) {
	/* Le mot inconnu commence par une minuscule et il y a un defaut pour les mots en minuscule */
	id = lciu_name_ids [which_dico];
      }
    }
  }
    
  return id;
}
#endif /* tdef_by_dico */

struct sxtoken *
tok_no2tok (SXINT tok_no)
{
  return &(SXGET_TOKEN (tok_no));
}


#if 0
SXINT
get_toks_buf_size ()
{
  return idag.repair_toks_buf_top; /* !! */
}
#endif /* 0 */

/* Cette fonction, appelee depuis l'error repair de earley met ds toks_buf un token
   "genere" par l'error repair, comme s'il provenait du source */
SXINT
put_repair_tok (SXINT t, char *t_str, char *comment_str, struct sxsource_coord *psource_index)
{
  SXINT                 lgth, id, kind;
  struct sxtoken        tok;
  char                  str [8+128];

  tok.comment = comment_str;

#ifdef tdef_by_dico
  /* On fabrique _error__t */
  snprintf (str, 8+128, "_error__%s", t_str);
  lgth = strlen (str);
  tok.string_table_entry = sxstr2save (str, lgth);

  /* On recherche _error ds le dico principal*/
  id = string2dico_id (0, "_error", 6, &kind);
#else /* !tdef_by_dico */
  tok.string_table_entry = sxstr2save (t_str, strlen (t_str));
  id = t;
#endif /* tdef_by_dico */

  tok.lahead = id;
  tok.source_index = *psource_index;

  sxput_token (tok);

  return idag.repair_toks_buf_top = sxplocals.Mtok_no;
}



#if EBUG
static void
print_idag_trans (SXINT from_state, char *comment, char *ff, char* semlex, SXINT t, SXINT to_state)
{
  double logprob = 0; /* valeur impossible */

  if (idag.pqt2proba)
    logprob = idag.pqt2proba [p_q_t2pqt (from_state, to_state, t)];

#if dico_contains_terminals &&  defined(def_t2string)
  if (semlex)
    printf ("%ld\t%s %s/%s(%ld) [|%s|]\t%ld\t(logprob=%g)\n", from_state, comment ? comment : "", ff, t2string [t], t, semlex, to_state, logprob);
  else
    printf ("%ld\t%s %s/%s(%ld)\t%ld\t(logprob=%g)\n", from_state, comment ? comment : "", ff, t2string [t], t, to_state, logprob);
#else /* def_t2string */
  if (semlex)
  printf ("%ld\t%s %s/(%ld) [|%s|]\t%ld\t(logprob=%g)\n", from_state, comment ? comment : "", ff, t, semlex, to_state, logprob);
  printf ("%ld\t%s %s/(%ld)\t%ld\t(logprob=%g)\n", from_state, comment ? comment : "", ff, t, to_state, logprob);
#endif /* def_t2string */
}
#endif /* EBUG */


#ifdef tdef_by_dico
/* Point d'entree (unique) appele' avant la lecture du udag, on ne connait donc aucune taille !! */
static void
init_udag_reader ()
{
  SXINT      cur_lgth, lgth, which_dico, id, id2;
  
  if (digits_name_ptr) {
    /* On cherche le code par defaut des nombres ecrits en chiffres dans tous les dicos  */
    lgth = strlen (digits_name_ptr);

    for (which_dico = 0; which_dico < MAX_DICO_NB; which_dico++) {
      cur_lgth = lgth;
      id = sxdfa_comb_seek_a_string (dicos [which_dico], digits_name_ptr, &cur_lgth);

      if (cur_lgth != 0)
	id = 0;

      digits_name_ids [which_dico] = id;
    }
  }
  
  if (ciu_name_ptr) {
    /* On cherche le code par defaut des noms propres inconnus dans tous les dicos  */
    lgth = strlen (ciu_name_ptr);
    id2 = 0;

    for (which_dico = 0; which_dico < MAX_DICO_NB; which_dico++) {
      cur_lgth = lgth;
      id = sxdfa_comb_seek_a_string (dicos [which_dico], ciu_name_ptr, &cur_lgth);

      if (cur_lgth != 0)
	id = 0;

      ciu_name_ids [which_dico] = id;
      id2 += id;
    }
    
    if (id2 == 0)
      ciu_name_ptr = NULL;
  }
  
  if (lciu_name_ptr) {
    /* On cherche le code par defaut des mots minuscules inconnus dans tous les dicos */
    lgth = strlen (lciu_name_ptr);
    id2 = 0;

    for (which_dico = 0; which_dico < MAX_DICO_NB; which_dico++) {
      cur_lgth = lgth;
      id = sxdfa_comb_seek_a_string (dicos [which_dico], lciu_name_ptr, &cur_lgth);

      if (cur_lgth != 0)
	id = 0;

      lciu_name_ids [which_dico] = id;
      id2 += id;
    }
    
    if (id2 == 0)
      lciu_name_ptr = NULL;
  }
}
#endif /* tdef_by_dico */




/* Utilise' par la lexicalisation ou les parseurs */
/* ca ne peut pas etre fait avec idag_infos car :
     - ce qu'on y met depend du parseur
     - et le lexicaliseur a pu changer les codes des terminaux */
/* Si non NULLs, remplit t2suffix_t_set, mlstn2la_tset et t2la_t_set et mlstn2suffix_source_set */
/* Au moins un des t2suffix_t_set et mlstn2la_tset est non NULL */
/* Remplit t2suffix_t_set en utilisant mlstn2suffix_source_set */
/* l'ordre des terminaux est donne' par t2suffix_t_set */
/* t2suffix_t_set [t1] = {t | existe un chemin du source de la forme "eof ... t1 ... t ... eof" */
/* autrement dit, t peut suivre t1 */
/* mlstn2suffix_source_set [i]= { t | a_i=t ou x=a_1 ... a_i ... t ... a_n} */
/* Si mlstn2la_tset est non NULL, remplit mlstn2la_tset [i]= { t | i ->t ?} */
/* Si t2la_t_set est non NULL, remplit t2la_t_set [t1]= { t2 | i ->t1 i+1 ->t2 ?} (utilise mlstn2la_tset qui doit etre aussi alloue') */
/* Nouvelle version sans arg, ds le cas insideG, glbl_source and co ont deja ete modifies */
void
idag_source_processing (SXINT SXEOF, SXBA *t2suffix_t_set, SXBA *mlstn2suffix_source_set, SXBA *mlstn2la_tset, SXBA *t2la_t_set)
{
  SXINT  from_state, to_state, bot_pq, top_pq, lgth, t;
  SXINT  *t_stack, *base_t_stack;
  SXBA   suffix_source_set, next_suffix_source_set, la_t_set, next_la_t_set, next_lb_t_set;

  if (t2suffix_t_set) {
#if EBUG
    if (mlstn2suffix_source_set == NULL)
      sxtrap (ME, "dag_or_nodag_source_processing");
#endif /* EBUG */
    SXBA_1_bit (mlstn2suffix_source_set [idag.final_state], SXEOF);
  }
#if EBUG
  else {
    if (mlstn2la_tset == NULL)
      sxtrap (ME, "dag_or_nodag_source_processing");
  }
#endif /* EBUG */

  if (mlstn2la_tset) {
    SXBA_1_bit (mlstn2la_tset [idag.final_state], SXEOF);
    SXBA_1_bit (mlstn2la_tset [0], SXEOF);
  }
#if EBUG
  else {
    if (t2la_t_set)
      sxtrap (ME, "dag_or_nodag_source_processing");
  }
#endif /* EBUG */

  for (from_state = idag.final_state-1; from_state >= idag.init_state; from_state--) {
    if (t2suffix_t_set) suffix_source_set = mlstn2suffix_source_set [from_state];
    bot_pq = idag.p2pq_hd [from_state];
    top_pq = idag.p2pq_hd [from_state+1];

    while (bot_pq < top_pq) {
      to_state = idag.pq2q [bot_pq];
      if (t2suffix_t_set) next_suffix_source_set = mlstn2suffix_source_set [to_state];
      base_t_stack = idag.t_stack + idag.pq2t_stack_hd [bot_pq];

      if (mlstn2la_tset) {
	la_t_set = mlstn2la_tset [from_state];
	if (t2la_t_set) next_la_t_set = mlstn2la_tset [to_state];
      }

      if (t2suffix_t_set) {
	t_stack = base_t_stack;
	lgth = *t_stack;

	while (lgth-- > 0) {
	  t = *++t_stack;
	  sxba_or (t2suffix_t_set [t], next_suffix_source_set);
	  SXBA_1_bit (suffix_source_set, t);
	}
	
	sxba_or (suffix_source_set, next_suffix_source_set);
      }

      if (mlstn2la_tset) {
	t_stack = base_t_stack;
	lgth = *t_stack;

	if (t2la_t_set) {
	  while (lgth-- > 0) {
	    t = *++t_stack;
	    sxba_or (t2la_t_set [t], next_la_t_set);
	    SXBA_1_bit (la_t_set, t);
	  }
	}
	else {
	  while (lgth-- > 0) {
	    t = *++t_stack;
	    SXBA_1_bit (la_t_set, t);
	  }
	}
      }

      bot_pq++;
    }
  }
}

#if 0
/* Si on a besoin de pq2q ... */
void
fill_idag_pq2p (void)
{
  SXINT nb, p, bot_pq, top_pq;

  if (idag.pq2p == NULL) {
    nb = idag.pq2q [0];
    idag.pq2p = (SXINT *) sxcalloc (nb+1, sizeof (SXINT)), idag.pq2p [0] = nb;

    for (p = idag.init_state; p < idag.final_state; p++) {
      bot_pq = idag.p2pq_hd [p];
      top_pq = idag.p2pq_hd [p+1];

      while (bot_pq < top_pq) {
	idag.pq2p [bot_pq] = p;

	bot_pq++;
      }
    }
  }
}
#endif /* 0 */


/* Si on a besoin de recuperer les transitions backward */
void
fill_idag_q2pq_stack (void)
{
  SXINT p, q, bot_pq, top_pq, nb, top;
  SXINT *q2p_nb, *pq_stack;

  if (idag.q2pq_stack_hd == NULL) {
    q2p_nb = (SXINT *) sxcalloc (idag.final_state+1, sizeof (SXINT));

#if 0
    if (idag.pq2p == NULL)
      fill_idag_pq2p ();
#endif /* 0 */

    nb = idag.last_pq;

    for (p = idag.init_state; p < idag.final_state; p++) {
      bot_pq = idag.p2pq_hd [p];
      top_pq = idag.p2pq_hd [p+1];

      while (bot_pq < top_pq) {
	q = idag.pq2q [bot_pq];
	q2p_nb [q]++;

	bot_pq++;
      }
    }

    idag.q2pq_stack_hd = (SXINT *) sxalloc (idag.final_state+1, sizeof (SXINT)), idag.q2pq_stack_hd [0] = 0, idag.q2pq_stack_hd [1] = 0;
    idag.q2pq_stack = (SXINT *) sxalloc (nb+idag.final_state+1, sizeof (SXINT)), idag.q2pq_stack [0] = 0;

    top = 0;

    for (q = idag.init_state+1; q <= idag.final_state; q++) {
      idag.q2pq_stack_hd [q] = ++top;
      idag.q2pq_stack [top] = 0; /* le compte */
      top += q2p_nb [q];
    }

    for (p = idag.init_state; p < idag.final_state; p++) {
      bot_pq = idag.p2pq_hd [p];
      top_pq = idag.p2pq_hd [p+1];

      while (bot_pq < top_pq) {
	q = idag.pq2q [bot_pq];
	pq_stack = idag.q2pq_stack + idag.q2pq_stack_hd [q];
	PUSH (pq_stack, bot_pq);

	bot_pq++;
      }
    }

    sxfree (q2p_nb);
  }
}

#if EBUG
void
ranges_oflw (SXINT old_size, SXINT new_size)
{
  /* Normalement ca ne deborde pas */
  sxtrap (ME, "ranges_oflw");

  /*
    idag.range2inside_t_set = sxbm_resize (idag.range2inside_t_set, old_size+idag.last_pq+1, new_size+idag.last_pq+1, idag.source_set_cardinal+1);
  */
}
#endif /* EBUG */


void
fill_inside_t_set (SXINT p, SXINT q, SXBA inside_t_set)
{
  SXINT bot, top, pr, r, rq;

  /* On cherche tous les r tq p ->t r ->+ q */
  bot = idag.p2pq_hd [p];
  top = idag.p2pq_hd [p+1];

  for (pr = bot; pr < top; pr++) {
    r = idag.pq2q [pr];

    if (r == q || SXBA_bit_is_set (idag.path [r], q)) {
      sxba_or (inside_t_set, idag.range2inside_t_set [pr]);

      if (r != q) {
	if ((rq = p_q2pq (r, q)) == 0) {
	  /* On n'a pas r ->t q */
	  if (!X_set (&(idag.ranges), (r<<idag.logn)+q, &rq)) {
	    /* nouveau */
	    rq += idag.last_pq;
	    fill_inside_t_set (r, q, idag.range2inside_t_set [rq]);
	  }
	}

	sxba_or (inside_t_set, idag.range2inside_t_set [rq]);
      }
    }
  }
}


/* Permet de completer idag avec certaines tables :
     - Si is_qstack_needed construit p_t2q_disp qui permet connaissant p et t
           a) de savoir s'il existe une trans p ->t et
	   b) de recuperer une pile qui contient la liste des q tel que p ->t q
     - Si is_pstack_needed construit t_q2p_disp qui permet connaissant t et q
           a) de savoir s'il existe une trans ->t q et
	   b) de recuperer une pile qui contient la liste des p tel que p ->t q
     - Si is_inside_t_needed construit le X_header qui pour les terminaux du source
           assure la correspondance global_t <--> inside_t
 */
void
fill_more_idag_infos (bool is_qstack_needed, bool is_pstack_needed, bool is_inside_t_needed)
{
  SXINT p, pq, top_pq, t, q, q_nb, pt2q_stack_top;
  SXINT *tstack, *cur_t_ptr, **cur_pt2q_area_ptr, *pt2q_stack;
  SXBA  pt_set, q_set, *t2q_set;

  if (is_qstack_needed) {
    idag.pt2q_disp = (SXINT ***) sxalloc (idag.final_state+1, sizeof (SXINT**)),
      idag.pt2q_disp [idag.final_state] = NULL;

    cur_pt2q_area_ptr =
      (idag.pt2q_disp [0] = (SXINT **) sxcalloc ((idag.final_state-1)*(idag.eof_code+1)+1,
						 sizeof (SXINT*))) + 1;

    *(idag.pt2q_disp [0]) =
      pt2q_stack = (SXINT *) sxalloc (2*idag.ptq_nb+1 /* Normalement ne deborde pas !! */, sizeof (SXINT)),
      TOP (pt2q_stack) = 0;

    t2q_set = sxbm_calloc (idag.eof_code+1, idag.final_state+1);
    pt_set = sxba_calloc (idag.eof_code+1);

    for (p = idag.init_state; p < idag.final_state; p++) {
      pq = idag.p2pq_hd [p];
      top_pq = idag.p2pq_hd [p+1];

      while (pq < top_pq) {
	q = idag.pq2q [pq];
	tstack = idag.t_stack + idag.pq2t_stack_hd [pq];
	
	for (cur_t_ptr = tstack + *tstack; cur_t_ptr > tstack; cur_t_ptr--) {
	  t = *cur_t_ptr; /* code global */

	  SXBA_1_bit (t2q_set [t], q);
	  SXBA_1_bit (pt_set, t);
	}

	pq++;
      }

      t = -1;

      while ((t = sxba_scan_reset (pt_set, t)) >= 0) {
	q_set = t2q_set [t];
	q_nb = 0;

	PUSH (pt2q_stack, 0); /* Le compte */
	pt2q_stack_top = TOP (pt2q_stack);
	cur_pt2q_area_ptr [t] = pt2q_stack+pt2q_stack_top;

	q = 0;

	while ((q = sxba_scan_reset (q_set, q)) >= 0) {
	  PUSH (pt2q_stack, q);
	  q_nb++;
	}

	pt2q_stack [pt2q_stack_top] = q_nb;
      }
      
      idag.pt2q_disp [p] = cur_pt2q_area_ptr;
      cur_pt2q_area_ptr += idag.eof_code+1;
    }

    sxfree (pt_set);
    sxbm_free (t2q_set);
  }

  if (is_pstack_needed) {
    SXINT p_nb, tq2p_stack_top;
    SXINT **cur_tq2p_area_ptr, *tq2p_stack, *cur_pq_ptr, *pq_stack;
    SXBA  tq_set, *t2p_set, p_set;

    if (idag.q2pq_stack == NULL)
      fill_idag_q2pq_stack ();

    idag.tq2p_disp = (SXINT ***) sxalloc (idag.final_state+1, sizeof (SXINT**)),
      idag.tq2p_disp [idag.final_state] = NULL;

    cur_tq2p_area_ptr =
      (idag.tq2p_disp [0] = (SXINT **) sxcalloc ((idag.final_state-1)*(idag.eof_code+1)+1,
					      sizeof (SXINT*))) + 1;

    *(idag.tq2p_disp [0]) =
      tq2p_stack = (SXINT *) sxalloc (2*idag.ptq_nb+1 /* Normalement ne deborde pas !! */, sizeof (SXINT)),
      TOP (tq2p_stack) = 0;

    t2p_set = sxbm_calloc (idag.eof_code+1, idag.final_state+1);
    tq_set = sxba_calloc (idag.eof_code+1);

    for (q = idag.init_state+1; q <= idag.final_state; q++) {
      pq_stack = idag.q2pq_stack + idag.q2pq_stack_hd [q];

      for (cur_pq_ptr = pq_stack + *pq_stack; cur_pq_ptr > pq_stack; cur_pq_ptr--) {
	pq = *cur_pq_ptr;
	p = range2p (pq);
	tstack = idag.t_stack + idag.pq2t_stack_hd [pq];

	for (cur_t_ptr = tstack +*tstack; cur_t_ptr > tstack; cur_t_ptr--) {
	  t = *cur_t_ptr; /* code global */
	  SXBA_1_bit (t2p_set [t], p);
	  SXBA_1_bit (tq_set, t);
	}
      }

      t = -1;

      while ((t = sxba_scan_reset (tq_set, t)) >= 0) {
	p_set = t2p_set [t];
	p_nb = 0;

	PUSH (tq2p_stack, 0); /* Le compte */
	tq2p_stack_top = TOP (tq2p_stack);
	cur_tq2p_area_ptr [t] = tq2p_stack+tq2p_stack_top;

	q = 0;

	while ((q = sxba_scan_reset (q_set, q)) >= 0) {
	  PUSH (tq2p_stack, q);
	  p_nb++;
	}

	tq2p_stack [tq2p_stack_top] = p_nb;
      }
      
      idag.tq2p_disp [p] = cur_tq2p_area_ptr;
      cur_tq2p_area_ptr += idag.eof_code+1;
    }

    sxfree (tq_set);
    sxbm_free (t2p_set);
  }

  if (is_inside_t_needed) {
    SXINT global_t, inside_t, range_nb, p, q, pq;
    SXINT *t_stack, *t_stack_top;
    SXBA inside_t_set, path;

    X_alloc (&(idag.global_t2inside_t), "global_t2inside_t", idag.source_set_cardinal+1, 1, NULL, NULL);

    global_t = -1;

    while ((global_t = sxba_scan (idag.source_set, global_t)) >= 0)
      X_set (&(idag.global_t2inside_t), global_t, &inside_t);

    if (idag.path == NULL)
      fill_idag_path ();

    range_nb = 0;

    for (p = idag.init_state; p < idag.final_state; p++)
      range_nb += sxba_cardinal (idag.path [p]); /* Ensemble des q accessibles depuis p */

    X_alloc (&(idag.ranges), "ranges", range_nb-idag.last_pq+1, 1, 
#if EBUG
	     ranges_oflw
#else
	     NULL
#endif /* EBUG */
	     , NULL);
    idag.range2inside_t_set = sxbm_calloc (X_size (idag.ranges)+idag.last_pq+1, idag.source_set_cardinal+1);

    /* On commence par remplir les pq <= idag.last_pq */
    for (pq = 1; pq <= idag.last_pq; pq++) {
      inside_t_set = idag.range2inside_t_set [pq];
      t_stack = idag.t_stack + idag.pq2t_stack_hd [pq];
      t_stack_top = t_stack + *t_stack;

      while (++t_stack <= t_stack_top) {
	inside_t = X_is_set (&(idag.global_t2inside_t), *t_stack);
	SXBA_1_bit (inside_t_set, inside_t);
      }
    }

    /* Normalement on couvre tous les ranges */
    p = idag.init_state;
    path = idag.path [p]; /* Ensemble des q accessibles depuis init_state */

    q = p;

    while ((q = sxba_scan (path, q)) > 0) {
      if ((pq = p_q2pq (p, q)) == 0) {
	/* on n'a pas p ->t q */
	if (!X_set (&(idag.ranges), (p<<idag.logn)+q, &pq)) {
	  pq += idag.last_pq;
	  fill_inside_t_set (p, q, idag.range2inside_t_set [pq]);
	}
      }
    }
  }
}


static SXINT dummy_check_keyword (char *s, SXINT i)
{
  sxuse (s);
  sxuse (i);
  return 0;
}

#define SXGET_SAVED_TOKEN(n)		(save_sxplocals.toks_buf [SXTOKEN_PAGE(n)][SXTOKEN_INDX(n)])

static void
fill_idag_infos (void)
{
  SXINT            from_state, to_state, bot_pq, top_pq, tok_no, lgth, cur_lgth, id, top, cur, tok, cur_tok, ste, semlex_ste = 0;
  SXINT            which_dico, nb, cur_t_stack_bot, cur_local_t_stack_bot, prev_tok_no;
  SXINT            *stack, *tok_no_stack, *top_tok_no_stack, *sdag_t_tok_no_stack, *top_sdag_t_tok_no_stack;
  SXINT            ptq_nb;
  char             *str, *cur_str, *ret_str, *comment, *semlex_str = NULL;
  bool          success;
  struct sxtoken   *ptok, *semlex_ptok;
  SXBA             glbl_source_i, current_trans_t_set;
  struct sxsrcmngr save_sxsrcmngr;
  struct sxsvar    save_sxsvar;
  struct sxplocals save_sxplocals;

  End_Of_Sentence_ste = (SXINT) sxstr2save ("_End_Of_Sentence_", 17);

  ptq_nb = 0;

#ifndef is_LFG
  if (tables) {
    /* On peut utiliser "tables" pour re_scanner les chaines traitees par dag_scanner et recuperer ainsi leur
       vrais codes internes donnes par les dummy_tables */
    /* On "ferme" le source mngr qui a lu le DAG */
    save_sxplocals = sxplocals;
    save_sxsrcmngr = sxsrcmngr;
    save_sxsvar = sxsvar;
    sxsrc_mngr (SXFINAL); /* Celui du DAG */

    sxtkn_mngr (SXOPEN, 1); /* Gestion des nouveaux tokens */
    (*(tables->analyzers.scanner)) (SXOPEN, tables); /* On prepare le scanneur de "tables" */
#ifdef def_sxdfa_comb_inflected_form
    // on a un lexique, donc on empêche la reconnaissance de keywords par le scanner (i.e. de formes en tant que terminaux de la grammaire)
    sxsvar.SXS_tables.check_keyword = dummy_check_keyword;
#endif /* def_sxdfa_comb_inflected_form */
  }
#endif /* is_LFG */

  current_trans_t_set = sxba_calloc (EOF_CODE+1);
  idag.source_set = sxba_calloc (EOF_CODE+1);
  idag.eof_code = EOF_CODE;
  idag.dag_kind = input_is_a_dag ? dag_kind : STRING_KIND /* un peu grossier, ca peut etre un SDAG */;

  /* A VOIR le cas ou le source est la chaine vide (a tester) */

  for (from_state = idag.init_state; from_state < idag.final_state; from_state++) {
    bot_pq = idag.p2pq_hd [from_state];
    top_pq = idag.p2pq_hd [from_state+1];

    while (bot_pq < top_pq) {
      to_state = idag.pq2q [bot_pq];
      DPUSH (idag.t_stack, 0);
      cur_t_stack_bot = TOP (idag.t_stack);
      idag.pq2t_stack_hd [bot_pq] = cur_t_stack_bot;

      tok_no = idag.pq2tok_no [bot_pq];

      if (dag_kind == SDAG_KIND) {
	prev_tok_no = 0; /* rappel: on n'a en SDAG qu'un seul pq pour chaque p */

	/* Ds ce cas tok_no designe la pile ... */
	sdag_t_tok_no_stack = sdag_get_t_tok_no_stack (tok_no);

	/* On exploite les terminaux associes a la ff */
	top_sdag_t_tok_no_stack = sdag_t_tok_no_stack + *sdag_t_tok_no_stack;
	sdag_t_tok_no_stack++; /* on saute le compte */

	/* le token contenant le mot d'origine */
	tok_no = *sdag_t_tok_no_stack;
	ptok = tables ? &(SXGET_SAVED_TOKEN (tok_no)) : &(SXGET_TOKEN (tok_no));
	idag.p2ff_ste [from_state] = ptok->string_table_entry;

	/* les tokens contenant les terminaux (et les probas) */
	while (++sdag_t_tok_no_stack <= top_sdag_t_tok_no_stack) {
	  tok_no = *sdag_t_tok_no_stack;
	  DPUSH (idag.local_t_stack, 0); /* Les t associes a chaque tok_no */
	  cur_local_t_stack_bot = TOP (idag.local_t_stack);

	  ptok = tables ? &(SXGET_SAVED_TOKEN (tok_no)) : &(SXGET_TOKEN (tok_no));
	  ptok->lahead = 0; /* a priori */

	  ste = ptok->string_table_entry;
	  str = sxstrget (ste);
#if EBUG
	  comment = ptok->comment;
#endif /* EBUG */
	  success = false;

	  if (ste != SXERROR_STE) {
	    lgth = sxstrlen (ste);

#ifdef tdef_by_dico
	    for (which_dico = 0; which_dico < MAX_DICO_NB; which_dico++) {
	      /* On suppose que si on a un dico, chaque transition du DAG est un sdag!! */
	      /* Faux, au moins ds le cas HUGE */
	      tok = 0;
	      id = string2dico_id (which_dico, str, lgth, &tok);

	      if (which_dico == 0)
		ptok->lahead = id; /* on stoke dans un lahead inutile un truc tout différent: la
				      catégorie ou le ift si unique, un offset pour récupérer la
				      liste des catégories ou des ift si non unique */

	      if (id) {
		success = true;

#ifdef is_LFG
		/* Cas ou le dico a ete fabrique a partir d'une specif LFG ... */
		if (which_dico == 0) {
		  /* ... pour l'instant, seul le dico principal a les structures qui permettent d'acceder aux codes des terminaux */
		  id = if_id2tok_list_id [id];

		  for (top = tok_disp [id+1], cur = tok_disp [id]; cur < top; cur++) {
		    cur_tok = TOK_LIST (cur);

		    if (tok == 0 // cas normal
			|| cur_tok == tok // cas forme__terminal
			|| (tok == -1 && strncmp (terminal_component_string,
#ifdef mp_h
						  tok_str [mp_t_table [cur_tok]],
#else /* !mp_h */
						  tok_str [cur_tok],
#endif /* ?mp_h */
						  terminal_component_string_lgth - 1) == 0 ) // cas forme__terminalPrefix*
			) {
		      if (!SXBA_bit_is_set (current_trans_t_set, cur_tok))
			DPUSH (idag.t_stack, cur_tok), ptq_nb++;

		      DPUSH (idag.local_t_stack, cur_tok);

		      if (SXBA_bit_is_reset_set (idag.source_set, cur_tok))
			idag.source_set_cardinal++;

#if EBUG
		      print_idag_trans (from_state, comment, str, NULL, cur_tok, to_state);
#endif /* EBUG */
		    }
		  }
		}
#else  /* !is_LFG */

		/* Cas "normal" */
		if (tok == -1)
		  /* Pour l'instant pas de ff__term* */
		  sxtrap (ME, "fill_idag_infos");

		if (tok > 0)
		  /* cas ff__terminal */
		  /* priorite' a` tok */
		  id = tok;

#ifdef is_probabilized_lexicon
#define MAX_ID MAX_IFT_CODE
#define CODE_STACK ift_code_stack
#else /* is_probabilized_lexicon */
#define MAX_ID EOF_CODE
#define CODE_STACK t_code_stack
#endif /* is_probabilized_lexicon */
		if (id > MAX_ID) {
		  /* a une forme flechie correspond plusieurs terminaux */
		  stack = CODE_STACK + id - MAX_ID;
		  nb = *stack;

		  while (nb-- > 0) {
		    id = *++stack; /* un terminal dans le cas normal, un ift dans le cas is_probabilized_lexicon */

#ifdef is_probabilized_lexicon
		    id = ift_code2terminal [id]; /* on rétablit le terminal à partir du ift_code */
#endif /* is_probabilized_lexicon */

		    if (SXBA_bit_is_reset_set (current_trans_t_set, id))
		      DPUSH (idag.t_stack, id), ptq_nb++;

		    DPUSH (idag.local_t_stack, id);

		    if (SXBA_bit_is_reset_set (idag.source_set, id))
		      idag.source_set_cardinal++;

#if EBUG
		    print_idag_trans (from_state, comment, str, NULL, id, to_state);
#endif /* EBUG */
		  }
		}
		else {
#ifdef is_probabilized_lexicon
		  id = ift_code2terminal [id]; /* on rétablit le terminal à partir du ift_code */
#endif /* is_probabilized_lexicon */

		  if (SXBA_bit_is_reset_set (current_trans_t_set, id))
		    DPUSH (idag.t_stack, id), ptq_nb++;

		  DPUSH (idag.local_t_stack, id);

		  if (SXBA_bit_is_reset_set (idag.source_set, id))
		    idag.source_set_cardinal++;

#if EBUG
		  print_idag_trans (from_state, comment, str, semlex_str, id, to_state);
#endif /* EBUG */
		}

#endif /* !is_LFG */

		if (prev_tok_no == 0)
		  idag.pq2tok_no [bot_pq] = tok_no;
		else {
		  if (prev_tok_no > 0) {
		    /* 2eme trans */
		    PUSH (idag.tok_no_stack, 2);
		    idag.pq2tok_no [bot_pq] = -TOP (idag.tok_no_stack); /* en negatif c'est l'index vers la sous-pile */
		    PUSH (idag.tok_no_stack, prev_tok_no);
		  }
		  else {
		    idag.tok_no_stack [-prev_tok_no]++; /* On va pusher un nouvel elem */
		  }
		    
		  PUSH (idag.tok_no_stack, tok_no);
		}
		prev_tok_no = idag.pq2tok_no [bot_pq];
		  
	      }
	      /* else La recherche ds "which_dico" a echoue'e ... */
	    }
#endif /* tdef_by_dico */



#ifndef is_LFG
	    /* On exploite les tables pour identifier str (calculer son id) --> */
	    if (tables) {
	      /* On scanne grace aux tables */
	      sxsrc_mngr (SXINIT, NULL, str); /* On va scanner str */
	      sxnext_char (); /* initialisation de la lecture */
	      sxplocals.Mtok_no = 0; /* Resultat a l'indice "Mtok_no + 1" */
	      
	      (*(tables->SXP_tables.scanit)) (); /* On le scanne */
	      id = SXGET_TOKEN (sxplocals.Mtok_no).lahead; /* ... et voila le resultat */
	      
	      if (id) {
		/* Il faut pusher le tok_no de ce terminal */
		if (prev_tok_no == 0)
		  idag.pq2tok_no [bot_pq] = tok_no;
		else {
		  if (prev_tok_no > 0) {
		    /* 2eme trans */
		    PUSH (idag.tok_no_stack, 2);
		    idag.pq2tok_no [bot_pq] = -TOP (idag.tok_no_stack); /* en negatif c'est l'index vers la sous-pile */
		    PUSH (idag.tok_no_stack, prev_tok_no);
		  }
		  else {
		    idag.tok_no_stack [-prev_tok_no]++; /* On va pusher un nouvel elem */
		  }
		  
		  PUSH (idag.tok_no_stack, tok_no);
		}
		prev_tok_no = idag.pq2tok_no [bot_pq];

		ptok->lahead = id;

		if (SXBA_bit_is_reset_set (current_trans_t_set, id))
		  DPUSH (idag.t_stack, id), ptq_nb++;
		
		DPUSH (idag.local_t_stack, id);
		
		if (SXBA_bit_is_reset_set (idag.source_set, id))
		  idag.source_set_cardinal++;

#if EBUG
		print_idag_trans (from_state, comment, str, semlex_str, id, to_state);
#endif /* EBUG */
	      }
	    }
	    /* On exploite les tables pour identifier str (calculer son id) <-- */
#endif /* ndef is_LFG */
	  }

	  top = TOP (idag.local_t_stack);
	  idag.local_t_stack [cur_local_t_stack_bot] = top - cur_local_t_stack_bot;
	  idag.tok_no2t_stack_hd [tok_no] = cur_local_t_stack_bot;

	    /* Le semlex A voir */
	  tok_no = *++sdag_t_tok_no_stack;
	}      
      }


      else /* ici, dag_kind != SDAG */ {
	if (tok_no < 0) {
	  tok_no_stack = idag.tok_no_stack - tok_no;
	  top_tok_no_stack = tok_no_stack + *tok_no_stack;
	  tok_no_stack++;
	}
	else {
	  /* truc */
	  top_tok_no_stack = tok_no_stack = &tok_no;
	}

	while (tok_no_stack <= top_tok_no_stack) {
	  tok_no = *tok_no_stack++;
	  DPUSH (idag.local_t_stack, 0); /* Les t associes a chaque tok_no */
	  cur_local_t_stack_bot = TOP (idag.local_t_stack);

	  ptok = tables ? &(SXGET_SAVED_TOKEN (tok_no)) : &(SXGET_TOKEN (tok_no));
	  ptok->lahead = 0; /* a priori */

	  ste = ptok->string_table_entry;
	  str = sxstrget (ste);
	  
	  semlex_ptok = tables ? &(SXGET_SAVED_TOKEN (tok_no+1)) : &(SXGET_TOKEN (tok_no+1));
	  if (semlex_ptok->lahead == get_SEMLEX_lahead ()) {
	    semlex_ste = semlex_ptok->string_table_entry;
	    semlex_str = sxstrget (semlex_ste);
	  } else {
	    semlex_ste = 0;
	    semlex_str = NULL;
	    semlex_ptok = NULL;
	  }
#if EBUG
	  comment = ptok->comment;
#endif /* EBUG */
    
	  if (ste != End_Of_Sentence_ste) {
	    success = false;

	    if (ste != SXERROR_STE) {
	      lgth = sxstrlen (ste);

#ifdef tdef_by_dico
	      for (which_dico = 0; which_dico < MAX_DICO_NB; which_dico++) {
		/* On suppose que si on a un dico, chaque transition du DAG est un sdag!! */
		/* Faux, au moins ds le cas HUGE */
		tok = 0;
		id = string2dico_id (which_dico, str, lgth, &tok);

		if (which_dico == 0)
		  ptok->lahead = id; /* on stoke dans un lahead inutile un truc tout différent: le id ds la ff principale (il peut donc etre nul) */

		if (id) {
		  success = true;

#ifdef is_LFG
		  /* Cas ou le dico a ete fabrique a partir d'une specif LFG ... */
		  if (which_dico == 0) {
		    /* ... pour l'instant, seul le dico principal a les structures qui permettent d'acceder aux codes des terminaux */
		    id = if_id2tok_list_id [id];

		    for (top = tok_disp [id+1], cur = tok_disp [id]; cur < top; cur++) {
		      cur_tok = TOK_LIST (cur);

		      if (tok == 0 // cas normal
			  || cur_tok == tok // cas forme__terminal
			  || (tok == -1 && strncmp (terminal_component_string,
#ifdef mp_h
						    tok_str [mp_t_table [cur_tok]],
#else /* !mp_h */
						    tok_str [cur_tok],
#endif /* ?mp_h */
						    terminal_component_string_lgth - 1) == 0 ) // cas forme__terminalPrefix*
			  ) {
			if (!SXBA_bit_is_set (current_trans_t_set, cur_tok))
			  DPUSH (idag.t_stack, cur_tok), ptq_nb++;

			DPUSH (idag.local_t_stack, cur_tok);

			if (SXBA_bit_is_reset_set (idag.source_set, cur_tok))
			  idag.source_set_cardinal++;

#if EBUG
			print_idag_trans (from_state, comment, str, NULL, cur_tok, to_state);
#endif /* EBUG */
		      }
		    }
		  }
#else  /* !is_LFG */
		  /* Cas "normal" */
		  if (tok == -1)
		    /* Pour l'instant pas de ff__term* */
		    sxtrap (ME, "fill_idag_infos");

		  if (tok > 0)
		    /* cas ff__terminal */
		    /* priorite' a` tok */
		    id = tok;

#ifdef is_probabilized_lexicon
#define MAX_ID MAX_IFT_CODE
#define CODE_STACK ift_code_stack
#else /* is_probabilized_lexicon */
#define MAX_ID EOF_CODE
#define CODE_STACK t_code_stack
#endif /* is_probabilized_lexicon */
		  if (id > MAX_ID) {
		    /* a une forme flechie correspond plusieurs terminaux */
		    stack = CODE_STACK + id - MAX_ID;
		    nb = *stack;

		    while (nb-- > 0) {
		      id = *++stack;

#ifdef is_probabilized_lexicon
		      id = ift_code2terminal [id]; /* on rétablit le terminal à partir du ift_code */
#endif /* is_probabilized_lexicon */

		      if (SXBA_bit_is_reset_set (current_trans_t_set, id))
			DPUSH (idag.t_stack, id), ptq_nb++;

		      DPUSH (idag.local_t_stack, id);

		      if (SXBA_bit_is_reset_set (idag.source_set, id))
			idag.source_set_cardinal++;

#if EBUG
		      print_idag_trans (from_state, comment, str, NULL, id, to_state);
#endif /* EBUG */
		    }
		  }
		  else {
#ifdef is_probabilized_lexicon
		    id = ift_code2terminal [id]; /* on rétablit le terminal à partir du ift_code */
#endif /* is_probabilized_lexicon */

		    if (SXBA_bit_is_reset_set (current_trans_t_set, id))
		      DPUSH (idag.t_stack, id), ptq_nb++;

		    DPUSH (idag.local_t_stack, id);

		    if (SXBA_bit_is_reset_set (idag.source_set, id))
		      idag.source_set_cardinal++;

#if EBUG
		    print_idag_trans (from_state, comment, str, semlex_str, id, to_state);
#endif /* EBUG */
		  }

#endif /* !is_LFG */
		}
		/* else La recherche ds "which_dico" a echoue'e ... */
	      }
#endif /* tdef_by_dico */

#ifndef is_LFG
	      /* Désormais (14/12/07), si on a des tables (i.e. si on a fait un _t.c par un lecl), on
		 demande son avis au lecl, même si on a un lexique, et même si le dit lexique a répondu
		 positivement */
	      /* Pas ds le cas is_LFG */
	      if (tables) {
		/* On scanne grace aux tables */
		sxsrc_mngr (SXINIT, NULL, str); /* On va scanner str */
		sxnext_char (); /* initialisation de la lecture */
		sxplocals.Mtok_no = 0; /* Resultat a l'indice "Mtok_no + 1" */
		(*(tables->SXP_tables.scanit)) (); /* On le scanne */
		id = SXGET_TOKEN (sxplocals.Mtok_no).lahead; /* ... et voila le resultat */

		if (id) {
		  success = true;
		  ptok->lahead = id;

		  if (SXBA_bit_is_reset_set (current_trans_t_set, id))
		    DPUSH (idag.t_stack, id), ptq_nb++;

		  DPUSH (idag.local_t_stack, id);

		  if (SXBA_bit_is_reset_set (idag.source_set, id))
		    idag.source_set_cardinal++;
#if EBUG
		  print_idag_trans (from_state, comment, str, semlex_str, id, to_state);
#endif /* EBUG */
		}
	      }
#endif /* ifndef is_LFG */
	    }

	    if (!success) {
#ifndef ont_warn_for_unknown_words
	      sxerror (ptok->source_index,
		       sxsvar.sxtables->err_titles [1][0],
		       "%sUnknown word \"%s\".",
		       sxsvar.sxtables->err_titles [1]+1,
		       str);
	      /* Le 03/05/04 Erreur est change' en Warning, les mots inconnus ne bloquent plus l'analyse */
	      /* Le 21/03/07 On sort le mot inconnu (le marqueur ds le source ne suffit pas ds le cas ou le source tient sur une ligne) */
#endif /* ont_warn_for_unknown_words */

	      ptok->lahead = 0;

	      /* Essai le 22/12/05 */
	      /* On met le terminal de code 0 sur les mots inconnus */
	      if (SXBA_bit_is_reset_set (current_trans_t_set, 0))
		DPUSH (idag.t_stack, 0);

	      DPUSH (idag.local_t_stack, 0);

	      if (SXBA_bit_is_reset_set (idag.source_set, 0))
		idag.source_set_cardinal++;
	    }
	  }
	  else {
	    /* On a une transition vide vers l'etat final f */
	    ptok->lahead = -1; /* marque de la transition vide (L'etat courant et l'etat final sont equiv. pour le parseur) */

	    /* Il faut dire que EOF est un look_ahead */
	    if (SXBA_bit_is_reset_set (current_trans_t_set, EOF_CODE))
	      DPUSH (idag.t_stack, EOF_CODE);

	    DPUSH (idag.local_t_stack, EOF_CODE);
	    /* SXBA_1_bit (idag.source_set, EOF_CODE); inutile, sera fait de toutes les facons */
#if EBUG
	    print_idag_trans (from_state, comment, str, NULL, EOF_CODE, to_state);
#endif /* EBUG */
	  }

	  top = TOP (idag.local_t_stack);
	  idag.local_t_stack [cur_local_t_stack_bot] = top - cur_local_t_stack_bot;
	  idag.tok_no2t_stack_hd [tok_no] = cur_local_t_stack_bot;
	  idag.tok_no2semlex_ste [tok_no] = semlex_ste;
	}
      }

      /* On exploite current_trans_t_set */
      top = TOP (idag.t_stack);
      idag.t_stack [cur_t_stack_bot] = top - cur_t_stack_bot;

      while (top > cur_t_stack_bot) {
	id = idag.t_stack [top];
	SXBA_0_bit (current_trans_t_set, id);
	top--;
      }

      bot_pq++;
    }
  }

  sxfree (current_trans_t_set);

#ifndef is_LFG
  if (tables) {
    (*(tables->analyzers.scanner)) (SXCLOSE, tables); /* Fin du scanneur de "tables" */
    sxtkn_mngr (SXCLOSE, 0); /* Soyons propres */
    sxsrc_mngr (SXFINAL); /* Celui de "tables" */

    sxsrcmngr = save_sxsrcmngr;
    sxsvar = save_sxsvar;
    sxplocals = save_sxplocals;
  }
#endif /* is_LFG */

  idag.repair_toks_buf_top = idag.toks_buf_top; /* vers le eof de fin */
  SXBA_1_bit (idag.source_set, EOF_CODE); /* L'eof de fin */
  idag.source_set_cardinal++;
  idag.ptq_nb = ptq_nb;
  idag.logn = sxlast_bit (idag.final_state);
  if (idag.pq2q [TOP (idag.pq2q)] == 0)
    idag.last_pq = TOP (idag.pq2q) - 1 /* à cause du 0 PUSHé en plus */;
  else
    idag.last_pq = TOP (idag.pq2q);

  {
    SXINT p, q, pq, range;
    SXINT *int_ptr;

    idag.p_q2range = (SXINT**) sxalloc (idag.final_state+1, sizeof (SXINT*));
    int_ptr = idag.p_q2range [0] /* was ...range_store, but can conveniently be stored in ...range [0] */ 
      = (SXINT*) sxcalloc ((idag.final_state+1)*(idag.final_state+2)/2+1, sizeof (SXINT));

    /* transitions */
    for (p = idag.init_state; p <= idag.final_state; p++) {
      idag.p_q2range [p] = int_ptr;

      for (pq = idag.p2pq_hd [p]; pq <= idag.last_pq && pq < idag.p2pq_hd [p+1]; pq++) {

	q = idag.pq2q [pq];
	idag.p_q2range [p] [q] = pq;
      }

      int_ptr += idag.final_state - p;
    }

    /* ranges that are not transitions */
    idag.max_range_value = idag.last_pq;

    for (p = idag.init_state; p <= idag.final_state; p++) {
      int_ptr = idag.p_q2range [p] + p;

      for (q = p; q <= idag.final_state; q++) {
	if (*int_ptr == 0)
	  *int_ptr = ++idag.max_range_value;

	int_ptr++;
      }
    }

    idag.range2p = (SXINT *) sxalloc (idag.max_range_value+1, sizeof (SXINT)), idag.range2p [0] = idag.range2p [0] = idag.max_range_value;
    idag.range2q = (SXINT *) sxalloc (idag.max_range_value+1, sizeof (SXINT)), idag.range2p [0] = idag.range2p [0] = idag.max_range_value;

    for (p = idag.init_state; p <= idag.final_state; p++) {
      int_ptr = idag.p_q2range [p];

      for (q = p; q <= idag.final_state; q++) {
	range = int_ptr [q];
	idag.range2p [range] = p;
	idag.range2q [range] = q;
      }
    }
  }
}


/* Cas DAG_KIND */
static struct fa {
  SXINT          fsa_kind, init_state, final_state, sigma_card, eof_ste, transition_nb;
  XxYxZ_header fsa_hd;
  bool      has_epsilon_trans;
} dfa, *cur_fsa;

static SXBA pseudofinal_trans_to_state_set; /* liste des transitions à dupliquer car elles arrivent
					       vers un état pseudo-final (relié par un EOF à l'état
					       final) */
static SXINT eof_tok_no;


/* appele' depuis les "actions" du parseur de udag sur chaque transition */
static void
store_idag (SXINT from_state, SXINT tok_no, SXINT to_state)
{
  SXINT            pq, q, prev_tok_no;

  /* Ne rien faire sur les tokens, ils sont deja ds sxplocals.toks_buf */

  /* Pour un udag on a from_state < to_state
     Si le udag est normalise', pour 2 appels consecutifs (p, t, q), (p', t', q') on a :
     Soit p' == p soit p' == p+1
     Si p' == p on a q' >= q
     Si p' == p et q' == q on a t' > t */
  if (dag_kind == DAG_KIND) {
    /* store_idag est appele' depuis dfa_minimize de postlude_re */
    if (/* to_state == cur_fsa->final_state
	   PB Le 14/08/08 ce test me semble hautement suspect car
	   cur_fsa->final_state est l'etat final du DAG avant minimisation !! remplace' par */
        SXGET_TOKEN (tok_no).string_table_entry == cur_fsa->eof_ste /* == sxplocals.Mtok_no (voir debut de postlude_re) */) {
      if (from_state+1 == to_state)
      /* On supprime la "derniere" trans sur eof */
	return;

      /* on garde les autres mais on redirige to_state */
      to_state--;

      if (pseudofinal_trans_to_state_set == NULL) {
	sxprepare_for_possible_reset (pseudofinal_trans_to_state_set);
	pseudofinal_trans_to_state_set = sxba_calloc (cur_fsa->final_state + 1);
	eof_tok_no = tok_no;
      }

      SXBA_1_bit (pseudofinal_trans_to_state_set, from_state);
    }
  }

  if (TOP (idag.p2pq_hd) < from_state) {
    /* Debut des transitions depuis from_state */
#if EBUG
    if (TOP (idag.p2pq_hd) != from_state-1)
      sxtrap (ME, "store_idag");
#endif /* EBUG */

    PUSH (idag.pq2q, to_state);
    pq = TOP (idag.pq2q);
    PUSH (idag.p2pq_hd, pq);
    idag.pq2tok_no [pq] = tok_no;

    DCHECK (idag.pq_base2pq, to_state-from_state+1);

    DSPUSH (idag.pq_base2pq, to_state);
    /* On a idag.pq_base2pq [idag.p2pq_base [p]] == max (to_state) */

    idag.p2pq_base [from_state] = TOP (idag.pq_base2pq);

    for (q = from_state+1; q < to_state; q++)
      DSPUSH (idag.pq_base2pq, 0);

    DSPUSH (idag.pq_base2pq, pq);
    /* On a pq = idag.pq_base2pq [idag.p2pq_base [p]+q-p]; */
  }
  else {
    pq = TOP (idag.pq2q);
    q = idag.pq2q [pq];

    if (q < to_state) {
      /* transition depuis from_state vers un etat different du coup precedent */
      /* On empile */
      input_is_a_dag = true; /* vrai DAG */
      PUSH (idag.pq2q, to_state);
      pq = TOP (idag.pq2q);
      idag.pq2tok_no [pq] = tok_no;

      idag.pq_base2pq [idag.p2pq_base [from_state]] = to_state;
      /* On a idag.pq_base2pq [idag.p2pq_base [p]] == max (to_state) */

      DCHECK (idag.pq_base2pq, to_state-q);

      while (++q < to_state)
	DSPUSH (idag.pq_base2pq, 0);

      DSPUSH (idag.pq_base2pq, pq);
      /* On a pq = idag.p2pq_base [p] [q-p]; */
    }
    else {
      /* transition multiple de p vers q (avec un tok_no different) */
#if EBUG
      if (q != to_state)
	sxtrap (ME, "store_idag");
#endif /* EBUG */

      prev_tok_no = idag.pq2tok_no [pq];

      if (prev_tok_no > 0) {
	/* 2eme trans */
	PUSH (idag.tok_no_stack, 2);
	idag.pq2tok_no [pq] = -TOP (idag.tok_no_stack); /* en negatif c'est l'index vers la sous-pile */
	PUSH (idag.tok_no_stack, prev_tok_no);
      }
      else {
	idag.tok_no_stack [-prev_tok_no]++; /* On va pusher un nouvel elem */
      }
	
      PUSH (idag.tok_no_stack, tok_no);
    }
  }
}


static void
idag_alloc (SXINT size)
{
  sxprepare_for_possible_reset (idag);
  memset (&idag, 0, sizeof(struct idag));
  idag.p2pq_hd = (SXINT *) sxalloc (size+2, sizeof (SXINT)), idag.p2pq_hd [0] = 0;
  idag.pq2q = (SXINT *) sxalloc (size+2, sizeof (SXINT)), RAZ (idag.pq2q);
  idag.pq2tok_no = (SXINT *) sxalloc (size+2, sizeof (SXINT)), RAZ (idag.pq2tok_no);
  idag.tok_no_stack = (SXINT *) sxalloc (2*size+2, sizeof (SXINT)), RAZ (idag.tok_no_stack);
  idag.tok_no2t_stack_hd = (SXINT *) sxalloc (2*size+1, sizeof (SXINT)), idag.tok_no2t_stack_hd [0] = 0;
  idag.tok_no2semlex_ste = (SXINT *) sxcalloc (2*size+1, sizeof (SXINT));
  idag.p2pq_base = (SXINT *) sxcalloc (size+2, sizeof (SXINT)), idag.p2pq_base [0] = 0;
  DALLOC_STACK (idag.pq_base2pq, size+2);

  if (dag_kind == SDAG_KIND)
    idag.p2ff_ste = (SXINT *) sxcalloc (size+2, sizeof (SXINT)), idag.p2pq_hd [0] = 0;
}

/* Le scanneur de udag vient de se terminer, on a donc une idee du nb de transitions */
static void
idag_free (struct idag *idag_ptr)
{
  if (idag_ptr->range2p) {
    sxfree (idag_ptr->range2p), idag_ptr->range2p = NULL;
    sxfree (idag_ptr->range2q), idag_ptr->range2q = NULL;
  }
  if (idag_ptr->p2pq_hd) {
    sxfree (idag_ptr->p2pq_hd), idag_ptr->p2pq_hd = NULL;
    sxfree (idag_ptr->pq2q), idag_ptr->pq2q = NULL;
    sxfree (idag_ptr->pq2tok_no), idag_ptr->pq2tok_no = NULL;

    sxfree (idag_ptr->p2pq_base), idag_ptr->p2pq_base = NULL;
    DFREE_STACK (idag_ptr->pq_base2pq);

    if (idag.p_q2range) {
      if (idag.p_q2range [0])
	sxfree (idag.p_q2range [0]);
      sxfree (idag.p_q2range), idag.p_q2range = NULL;
    }

    if (idag_ptr->tok_no_stack) sxfree (idag_ptr->tok_no_stack), idag_ptr->tok_no_stack = NULL;

    sxfree (idag_ptr->tok_no2t_stack_hd), idag_ptr->tok_no2t_stack_hd = NULL;
    sxfree (idag_ptr->tok_no2semlex_ste), idag_ptr->tok_no2semlex_ste = NULL;

    if (idag_ptr->pq2t_stack_hd) {
      sxfree (idag_ptr->pq2t_stack_hd), idag_ptr->pq2t_stack_hd = NULL;
      if ((idag_ptr->orig_t_stack) && (idag_ptr->t_stack != idag_ptr->orig_t_stack))
	DFREE_STACK (idag_ptr->orig_t_stack);
      else
	idag_ptr->orig_t_stack = NULL;
      DFREE_STACK (idag_ptr->t_stack);
      DFREE_STACK (idag_ptr->local_t_stack);
      if ((idag_ptr->orig_source_set) && (idag_ptr->source_set != idag_ptr->orig_source_set))
	sxfree (idag_ptr->orig_source_set);
      idag_ptr->orig_source_set = NULL;
      sxfree (idag_ptr->source_set), idag_ptr->source_set = NULL;
      if (idag_ptr->pqt2proba)
	sxfree (idag_ptr->pqt2proba), idag_ptr->pqt2proba = NULL;
    }

    if (idag_ptr->q2pq_stack_hd) {
      sxfree (idag_ptr->q2pq_stack_hd), idag_ptr->q2pq_stack_hd = NULL;
      sxfree (idag_ptr->q2pq_stack), idag_ptr->q2pq_stack = NULL;
#if 0
      sxfree (idag_ptr->pq2p), idag_ptr->pq2p = NULL;
#endif /* 0 */
    }

    if (idag_ptr->path)
      sxbm_free (idag_ptr->path), idag_ptr->path = NULL;

    if (idag_ptr->pqt2pq)
      sxfree (idag_ptr->pqt2pq), idag_ptr->pqt2pq = NULL;

    if (idag_ptr->pt2q_disp) {
      sxfree (**idag_ptr->pt2q_disp);
      sxfree (*idag_ptr->pt2q_disp);
      sxfree (idag_ptr->pt2q_disp), idag_ptr->pt2q_disp = NULL;
    }

    if (idag_ptr->tq2p_disp) {
      sxfree (**idag_ptr->tq2p_disp);
      sxfree (*idag_ptr->tq2p_disp);
      sxfree (idag_ptr->tq2p_disp), idag_ptr->tq2p_disp = NULL;
    }

    if (X_is_allocated (idag_ptr->global_t2inside_t))
      X_free (&(idag_ptr->global_t2inside_t));

    if (X_is_allocated (idag_ptr->ranges)) {
      X_free (&(idag_ptr->ranges));
      sxbm_free (idag_ptr->range2inside_t_set), idag_ptr->range2inside_t_set = NULL;
    }

    if (idag_ptr->p2ff_ste)
      sxfree (idag_ptr->p2ff_ste), idag_ptr->p2ff_ste = NULL;

    if (idag_ptr->source_set)
      sxfree (idag_ptr->source_set), idag_ptr->source_set = NULL;
  }
}

/* Cette fonction est appelee depuis read_a_udag après tous les appels à store_dag ().
   (le parseur de udag est donc fini)
   Precede l'appel a fill_udag_infos ()
   Elle permet de faire certains traitements, les valeurs init_pos, final_pos et last_tok_no etant
   (désormais) connues */
static void
after_last_idag_trans ()
{
  /* Pour permettre des modifs dynamiques du dag, et pour conserver la propriete si (from_state, tok_no, to_state)
     on a from_state < to_state (par exemple traitement des erreurs), on maintient 2 tableaux dag_state2mlstn et mlstn2dag_state
     tels que la propriete precedente s'applique sur dag_state2mlstn[from_state] < dag_state2mlstn[to_state].
     Ce sont donc les milestones qui sont strictement croissants.
  */
  SXINT last_pq;

  /* On "termine" idag */
  idag.init_state = 1;
  last_pq = TOP (idag.pq2q);

  if ((idag.final_state = idag.pq2q [last_pq]) == 0)
    sxtrap (ME, "after_last_idag_trans (the input DAG is empty)");

  PUSH (idag.pq2q, 0);

#if EBUG
  if (TOP (idag.p2pq_hd) != idag.final_state-1)
    sxtrap (ME, "after_last_idag_trans");
#endif /* EBUG */

  idag.p2pq_hd [idag.final_state] = last_pq+1;

  if (pseudofinal_trans_to_state_set) {
    struct idag saved_idag;
    SXINT p, q, tok_no, *tok_no_stack, tok_no_number, bot, top, pq, offset;

    saved_idag = idag;
    idag_alloc (sxplocals.Mtok_no);

    /* travail */
    for (p = saved_idag.init_state; p < saved_idag.final_state; p++) {
      bot = saved_idag.p2pq_hd [p];
      top = saved_idag.p2pq_hd [p+1];

      for (pq = bot; pq < top; pq++) {
	q = saved_idag.pq2q [pq];
	tok_no = saved_idag.pq2tok_no [pq];

	if (tok_no > 0) {
	  if (tok_no != eof_tok_no)
	    store_idag (p, tok_no, q);
	  else if (p == saved_idag.init_state)
	    sxtrap (ME, "after_last_idag_trans (the input DAG includes the empty string)");
	}
	else {
#if 0
	  /* semble faux, tok_no2t_stack_hd ne semble rempli que bien plus tard par fill_idag_infos */
	  tok_no_stack = saved_idag.tok_no_stack + saved_idag.tok_no2t_stack_hd [-tok_no] + 1;
#endif /* 0 */
	  tok_no_stack = saved_idag.tok_no_stack - tok_no; /* au sens de saved_idag.tok_no_stack + (-tok_no) */
	  tok_no_number = tok_no_stack [0];

	  for (offset = 1; offset <= tok_no_number; offset++) {
	    tok_no = tok_no_stack [offset];

	    if (tok_no != eof_tok_no)
	      store_idag (p, tok_no, q);
	    else if (p == saved_idag.init_state)
	      sxtrap (ME, "after_last_idag_trans (the input DAG includes the empty string)");
	  }
	}

	if (SXBA_bit_is_set (pseudofinal_trans_to_state_set, q)) {
	  /* pas de test sur eof_tok_no car on ne peut pas arriver par EOF sur un etat pseudofinal */
	  if (tok_no > 0) {
	    store_idag (p, tok_no, saved_idag.final_state);
	  }
	  else {
	    /* tok_no_stack déjà calculé */
	    /* tok_no_number idem */
	    for (offset = 1; offset <= tok_no_number; offset++)
	      store_idag (p, tok_no_stack [offset], saved_idag.final_state);
	  }
	}
      }
    }

    idag_free (&saved_idag);
    sxfree (pseudofinal_trans_to_state_set), pseudofinal_trans_to_state_set = NULL;    

    /* On "termine" idag */
    idag.init_state = 1;
    /*    last_pq = TOP (idag.pq2q); fait plus haut, et en plus faux à cause du PUSH d'un 0 en plus (il faudrait dire TOP - 1)*/
    /* BS: ben si, rétabli suite à débug: contrairement à ce que dit le commentaire précédent, le
       PUSH concernait l'ancien idag (celui mis dans saved_idag puis libéré), pas le nouvel idag
       qu'on vient de construire, et dans lequel le 0 n'est pas encore pushé à ce stade (cf. 2
       lignes plus bas). Du coup je rétablis (car le last_pq du nouvel idag n'est pas forcément
       celui de l'ancien, si jamais des trafics sur transition epsilon vers eof ont eu lieu */
    last_pq = TOP (idag.pq2q);
    idag.final_state = idag.pq2q [last_pq];
    PUSH (idag.pq2q, 0);
#if EBUG
    if (TOP (idag.p2pq_hd) != idag.final_state-1)
      sxtrap (ME, "after_last_idag_trans");
#endif /* EBUG */
    
    idag.p2pq_hd [idag.final_state] = last_pq+1;
  }
  
  SXGET_TOKEN (0).lahead = EOF_CODE;
  SXGET_TOKEN (sxplocals.Mtok_no).lahead = EOF_CODE;

  idag.toks_buf_top = sxplocals.Mtok_no; /* Le tok_no du eof final */

  DALLOC_STACK (idag.t_stack, 2*idag.toks_buf_top); /* taille min pour contenir un t par tok */
  idag.pq2t_stack_hd = (SXINT *) sxalloc (last_pq+1, sizeof (SXINT)), idag.pq2t_stack_hd [0] = 0;

  DALLOC_STACK (idag.local_t_stack, 2*idag.toks_buf_top); /* taille min pour contenir un t par tok */

  n = idag.final_state ; /* !! */
}

extern SXINT sxparser (); // celui de SYNTAX

/* Appele' depuis read_a_udag */
/* Le scanner est termine' sxplocals.Mtok_no reference le eof final */
/* Parseur "manuel" des sources sous forme de chaine lineaire */
static SXINT
string_parser (SXINT what, struct sxtables *arg)
{
  SXINT tok_no;

  if (what == SXACTION) {
    for (tok_no = 1; tok_no < sxplocals.Mtok_no; tok_no++) {
      store_idag (tok_no, tok_no, tok_no+1);
    }
  }
  else
    /* prudence */
    sxparser (what, arg);
    
}

static SXINT sentence_nb;

SXINT
get_sentence_nb (void)
{
  return sentence_nb;
}

static char *dag_kind_strings [] = {"??", "Dag", "Udag","??", "Sdag","??","??","??", "String"};
/* Ca permet de prevenir si le nombre de terminaux depasse une valeur limite (source trop gros pour etre traite') */
/* Pas utilisation en stand alone */
static SXINT
check_udag ()
{
  bool oversized;
  char    mess [128];
  int   severity = 0;

  oversized = ((SXUINT) idag.ptq_nb > maximum_input_size);
  
  if (is_print_time) {
    sentence_nb++;
    sprintf (mess, "%s\t%s Scanner[sentence #%ld, final_state=%ld, terminal_transition_nb=%ld%s]",
	     sentence_nb == 1 ? "" : "\n",
	     dag_kind_strings [dag_kind], sentence_nb, idag.final_state /* Etat final */, idag.ptq_nb /* nb de transitions */, oversized ? "(oversized)" : "");
    sxtime (SXACTION, mess);
  }
  
  if (oversized)
    severity = 7;

  return severity;
}




static void
raz_idag ()
{
  idag_free (&idag);
  /* Prudence */
  idag_alloc (sxplocals.Mtok_no);
}



/* Cas SDAG_KIND */
/* Appele' depuis read_a_sdag */
static void
store_sdag_trans (SXINT p, SXINT id, SXINT q)
{
  SXINT pq;

  /* id est un identifiant qui designe la transition de p -> q
     On pourra ds fill_idag_infos tout recupe'rer a` partir de id */
  /* Que faut-il faire pour que le futur appel a fill_idag_infos marche ? */
  /* Les couples de tok_no de t_tok_no_stack repe`re des couples (t, semlex)
     t doit etre recherche' ds dico_des_terminaux */
  /* Si ff_tok_no est non nul, ff peut etre recherchee ds les dicos */
  PUSH (idag.pq2q, q);
  pq = TOP (idag.pq2q);
  PUSH (idag.p2pq_hd, pq);
  idag.pq2tok_no [pq] = id; /* On sait que ce n'est pas un tok_no car dag_kind==SDAG_KIND */

  if (idag.p2pq_base [p] == 0) {
    DPUSH (idag.pq_base2pq, q);
    /* On a idag.pq_base2pq [idag.p2pq_base [p]] == max (to_state) */

    idag.p2pq_base [p] = TOP (idag.pq_base2pq);

    DPUSH (idag.pq_base2pq, pq);
  }
}



/* Cette procedure est appelee depuis read_a_re a` la fin de la 1ere passe sur l'arbre abstrait */
/* Les arguments sont 
   - true => OK
   - ste des transitions vers l'etat final
   - le nb de noeuds de cet arbre et 
   - le nb d'operande (dont eof + 1 bidon de fin) */
/* Peut servir a faire des alloc + ciblees */
static void
prelude_re (bool is_OK, SXINT eof_ste, SXINT node_nb, SXINT operand_nb, SXINT fsa_kind)
{
  SXINT *fsa_foreach;
#if EBUG
  char *name;
#endif
  SXINT foreach [6] = {1, 0, 0, 0, 0, 0};

  sxuse (operand_nb);
  if (is_OK) {
#if LOG
    sxtime (SXINIT, NULL);
#endif /* LOG */

#if EBUG
    name = "RE2DFA";
#endif
    fsa_foreach = foreach;
    cur_fsa = &dfa;

    cur_fsa->fsa_kind = fsa_kind;
    cur_fsa->eof_ste = eof_ste;
    cur_fsa->init_state = 1;
    sxprepare_for_possible_reset (cur_fsa->fsa_hd.is_allocated); /* car *cur_fsa vaut dfa qui est static ... */
    XxYxZ_alloc (&(cur_fsa->fsa_hd), "nfsa_hd", node_nb+1, 1, fsa_foreach, NULL /* pas de re_oflw */, NULL); 

#if EBUG
    printf ("\n%s\n", name);
#endif
  }
  else
    /* PB ds la lecture de l'ER (pas un dag, ...) */
    sxexit (2);

  /* On remet a zero sxplocals.toks_buf pour store_re */
  sxtkn_mngr (SXINIT, 0);
}

/* Cette procedure est appelee depuis read_a_re sur chaque transition de l'automate produit */
/* L'etat initial est 1 */
/* l'etat final unique est atteint par transition sur le terminal bidon "<EOF>" de ste eof_ste et vaut -f */
/* Tous les etats de 1 a f sont utilises */
/* ptok_ptr contient {NULL, ptok1, ...ptokm, NULL} */
/* ste est commun a tous les ptok de ptok_ptr */
/* ptoki est un pointeur vers un token courant (dans le noeud de l'arbre abstrait) */
static void
store_re (SXINT state, struct sxtoken **ptok_ptr, struct sxtoken **semlex_ptok_ptr, SXINT ste, SXINT next_state)
{
  SXINT            triple, lgth, tmp_ste;
  struct sxtoken   tok, *ptok, *semlex_tok_ptr;

  /* Normalement les transitions epsilon ont ete supprimees (read_a_re a ete appele' avec l'option RE2DFA) */
#if EBUG
  if (ptok_ptr == NULL && ste == 0)
    /* Transition epsilon */
    sxtrap (ME, "store_re");

#if 0
  /* Sinon, il faudrait faire un truc du genre ... */
  if (ptok_ptr == NULL && ste == 0) {
    /* Transition epsilon */
    cur_fsa->has_epsilon_trans = true;
    XxYxZ_set (&(cur_fsa->fsa_hd), state, 0, next_state, &triple);
#if EBUG
    printf ("%ld\t\"<EPSILON>\"\t\t%ld\n", state, next_state);
#endif /* EBUG */
  }
#endif /* 0 */
#endif /* EBUG */

  /* Ici on exploite ptok_ptr qui contient {NULL, ptok1, ...ptokm, NULL}
     ou chaque ptoki repere un sxtoken dans l'arbre abstrait dont le string_table_entry est ste
     en parallèle, semlex_ptok_ptr contient (s'il n'est pas nul) le semlex associé à chaque token, ou NULL */
  if (ste == cur_fsa->eof_ste)
    cur_fsa->final_state = next_state;

  /* On cumule les comments de tous les tok de ptok_ptr */
  tok = *(ptok = *++ptok_ptr);
  if (semlex_ptok_ptr)
    semlex_tok_ptr = *++semlex_ptok_ptr;
  else
    semlex_tok_ptr = NULL;

  if ((ptok = *++ptok_ptr) != NULL) {
    varstr_raz (vstr);
  
    if (tok.comment)
      vstr = varstr_catenate (vstr, tok.comment);
    
    do {
      if (ptok->comment) {
	if (strcmp (varstr_tostr (vstr) + 1, ptok->comment + 1) != 0) {
	  //      if (strstr (varstr_tostr (vstr) + 1, ptok->comment + 1) == NULL) {
	  varstr_pop (vstr, 1); /* On enleve le "}" final */
	  vstr = varstr_catchar (vstr, ' ');
	  vstr = varstr_catenate (vstr, ptok->comment+1); /* On enleve le "{" initial du suffixe */
	}
      }
    } while ((ptok = *++ptok_ptr) != NULL);
    
    if ((lgth = varstr_length (vstr)) > 0) {
      tmp_ste = sxstr2save (varstr_tostr (vstr), lgth);
      tok.comment = sxstrget (tmp_ste);
    }

    if (semlex_ptok_ptr /* la pile (pê null) de semlex tokens recue en entrée */ != NULL
	&& semlex_tok_ptr == NULL /* le futur (unique) semlex token */)
      semlex_tok_ptr = *++semlex_ptok_ptr;

  }

  tok.string_table_entry = ste; /* prudence, c'est pas fait sur eof */

  sxput_token (tok); /* mis en sxplocals.Mtok_no */

  XxYxZ_set (&(cur_fsa->fsa_hd), state, sxplocals.Mtok_no, next_state, &triple);

  if (semlex_tok_ptr)
    sxput_token (*semlex_tok_ptr); /* mis en sxplocals.Mtok_no */

#if EBUG
  printf ("%ld\t%s %s\t\t%ld\n", state, tok.comment ? tok.comment : "", sxstrget (ste), next_state);
#endif /* EBUG */
}


static void
DFA_extract_trans (SXINT dfa_state, void (*output_trans) ())
{
  SXINT triple;

  XxYxZ_Xforeach (cur_fsa->fsa_hd, dfa_state, triple)
    (*output_trans) (dfa_state, XxYxZ_Y (cur_fsa->fsa_hd, triple), XxYxZ_Z (cur_fsa->fsa_hd, triple));
}


static void
raz_re ()
{
  if (XxYxZ_is_allocated (cur_fsa->fsa_hd)) {
    XxYxZ_free (&(cur_fsa->fsa_hd));
  }
}


/* Cette fonction est appelee après les appels à store_[u]re(). */
/* Elle utilise l'automate lu */
static SXINT
postlude_re (SXINT fsa_kind)
{
  SXINT severity;

#if LOG
  static char           mess [128];
#endif /* LOG */

  cur_fsa->transition_nb = XxYxZ_top (cur_fsa->fsa_hd);

#if LOG
  sprintf (mess, "RE2DFA: state_nb = %ld", cur_fsa->final_state);
  sxtime (SXACTION, mess);
#endif /* LOG */

#if EBUG
  fputs ("DFA2min_DFA\n", stdout);
#endif /* EBUG */

  idag_free (&idag); /* Prudence */
  idag_alloc (cur_fsa->transition_nb);
  
  dfa_minimize (1, cur_fsa->final_state, sxplocals.Mtok_no/*cur_fsa->eof_ste*/, DFA_extract_trans, store_idag, true /* to_be_normalized */
#ifdef ESSAI_INVERSE_MAPPING
		, NULL
#endif /* ESSAI_INVERSE_MAPPING */
		);

  after_last_idag_trans ();

#if LOG
  sprintf (mess, "DFA2min_DFA: state_nb = %ld, total_trans_nb = %ld", idag.final_state, idag.toks_buf_top-1);
  sxtime (SXACTION, mess);
#endif /* LOG */

  fill_idag_infos ();
  severity = check_udag ();

  raz_re ();

  return severity;
}

/* FIN du cas DAG_KIND */



/* Le point d'entree principal de udag_scanner.c s'appelle dag_scanner (et non udag_scanner)
   c,a permet au main d'appeler "dag_scanner ()", c'est le makefile de l'utilisateur qui a choisi
   entre dag_scanner.c ou udag_scanner.c */
SXINT 
dag_scanner (SXINT what, struct sxtables *arg /* reçoit toujours NULL*/)
{
  SXINT severity = 0;

  switch (what) {
  case SXBEGIN :
#ifdef dummy_tables
    tables = &dummy_tables;
#endif /* dummy_tables */
#ifdef no_dico_lecl
    if (tables == NULL)
      sxtrap (ME, "dag_scanner (we detected a configuration with LECL but without lexicon ; however, we found no LECL table)");
#endif /* no_dico_lecl */

    sxerr_mngr (SXBEGIN);
    sxstr_mngr (SXBEGIN) /* initialisation ou remise a vide de la string table */ ;

#if 0
    if (dag_kind == SDAG_KIND && tables)
      /* On est ds le cas d'un vrai parseur SYNTAX construit par l'utilisateur (la grammaire doit cependant avoir une forme predefinie) */
      (*(tables->analyzers.parser)) (SXBEGIN, tables) /* Allocation des variables globales du parser */ ;
#endif /* 0 */

    break;

  case SXOPEN:
    /* Il se peut que par exemple une chaine soit lue par read_a_re, dans ce cas on veut quand meme lui donner le type STRING_KIND, etc. */
    input_is_a_dag = false;

    switch (dag_kind) {
    case UDAG_KIND :
      /* On continue en sequence */
    case DAG_KIND :
    default : /* Par defaut on dit qu'on a un DAG en entree !! */
#ifndef EOF_CODE
      sxtrap(ME,"udag_scanner (EOF_CODE is undef)");
#endif /* EOF_CODE */

#if 0
      if (EOF_CODE==0)
	sxtrap(ME,"udag_scanner (The 'dico' included in this program is not suitable for use with udag_scanner (EOF_CODE == 0))");
#endif /* 0 */

      break;

    case SDAG_KIND :
      /* Le source a la forme :
	 ( [ff] "(" ( t [%SEMLEX] )+ ")" )+
      */
#if EBUG
#ifndef no_dico_lecl
      if (dico_des_terminaux == NULL)
	sxtrap (ME, "dag_scanner (dico_des_terminaux == NULL)");
#endif  /* no_dico_lecl */
#endif /* EBUG */
  
      break;

    case STRING_KIND :
      /* C'est le "scanner" des udag_t.c qui va etre utilise' pour scanner les
	 ff du source.
	 Si tables est non null, elles seront utilisees uniquement pour faire des
	 reconnaissances supplementaires */
      break;
    }

    break;

  case SXINIT:
    switch (dag_kind) {
    case UDAG_KIND :
    /* On continue en sequence */
    case DAG_KIND :
    default : /* Par defaut on dit qu'on a un DAG en entree !! */
    break;

    case SDAG_KIND :
      break;

    case STRING_KIND :
      break;
    }
    break;

  case SXACTION:
    if (is_print_time)
      sxtime (SXINIT, NULL);
    
    wstr = (char*) sxalloc ((wstr_lgth = 128)+1, sizeof (char));
    vstr = varstr_alloc (128);

#ifdef tdef_by_dico
    init_udag_reader ();
#endif /* tdef_by_dico */
    
    switch (dag_kind) {
    case UDAG_KIND :
      severity = read_a_udag (store_idag, 
			    after_last_idag_trans, 
			    fill_idag_infos,
			    check_udag,
			    raz_idag,
			    sxparser /* celui de syntax */
			    );

    break;

    case DAG_KIND :
    default : /* Par defaut on dit qu'on a un DAG en entree !! */
      severity = read_a_re (prelude_re,
			    store_re, 
			    postlude_re,
			    RE2DFA|RE_IS_A_DAG|RE_INCREASE_ORDER /* what_to_do */); /* On ne met pas "|RE_USE_COMMENTS" car on va utiliser les tokens ds store_re */
    break;

    case SDAG_KIND :
      severity = read_a_sdag (store_sdag_trans, 
			    after_last_idag_trans, 
			    fill_idag_infos,
			    check_udag,
			    raz_idag,
			    sxparser /* celui de syntax */
			    );
    break;

    case STRING_KIND :
      severity = read_a_udag (store_idag, 
			    after_last_idag_trans, 
			    fill_idag_infos,
			    check_udag,
			    raz_idag,
			    string_parser /* local */
			    );
      break;
    }
  
    idag_free (&idag);

    sxfree (wstr), wstr = NULL;
    varstr_free (vstr);

    break;

  case SXFINAL:
    switch (dag_kind) {
    case UDAG_KIND :
    /* On continue en sequence */
    case DAG_KIND :
    default : /* Par defaut on dit qu'on a un DAG en entree !! */
    break;

    case SDAG_KIND :
      break;

    case STRING_KIND :
      break;
    }
    break;

  case SXCLOSE:
    switch (dag_kind) {
    case UDAG_KIND :
    /* On continue en sequence */
    case DAG_KIND :
    default : /* Par defaut on dit qu'on a un DAG en entree !! */
    break;

    case SDAG_KIND :
      break;

    case STRING_KIND :
      break;
    }

    break;

  case SXEND :
    sxerr_mngr (SXEND);
    sxstr_mngr (SXEND);

    if (dag_kind == SDAG_KIND && tables)
      /* On est ds le cas d'un vrai parseur SYNTAX construit par l'utilisateur (la grammaire doit cependant avoir une forme predefinie) */
      (*(tables->analyzers.parser)) (SXEND, tables);

    break;

  default:
    break;
  }

  return severity;
}

void
fill_idag_pqt2pq (void)
{
  SXINT pq, x;
  SXINT top = DTOP (idag.t_stack);
  SXINT *pqt2pq, *top_pqt2pq;

  if (idag.pqt2pq == NULL) {
    idag.pqt2pq = (SXINT *) sxalloc (top+1, sizeof (SXINT)), TOP (idag.pqt2pq) = top;

#if 0
    if (idag.pq2p == NULL)
      /* On en a besoin pour pqt2p() */
      fill_idag_pq2p ();
#endif /* 0 */

    for (pq = idag.pq2q [0]; pq > 0; pq--) {
      if (idag.pq2q [pq] > 0 /* à cause du 0 rajouté par sécurité au début de after_last_idag_trans */
	  && idag.range2p [pq] != idag.range2q [pq] /* BS: sécurité car pour l'instant les trans epsilon ne sont pas gérées!!! A FAIRE!!! */
	  ) {
	x = idag.pq2t_stack_hd [pq];
	pqt2pq = idag.pqt2pq + x;
	top_pqt2pq = pqt2pq+idag.t_stack [x];
	
	while (top_pqt2pq > pqt2pq /* on ne met pas les comptes, car le compte global est en 0 */)
	  *top_pqt2pq-- = pq;

	*top_pqt2pq = 0; /* place du compte */
      }
    }
  }
}

// !!! A FAIRE: à passer en global, pour que nbest y mette fudge_factor si on se compile avec nbest_semact.c
//#define fudge_factor 0.01
#define fudge_factor 1

void
fill_idag_pqt2proba (void) {
  SXINT from_state, bot_pq, top_pq, tok_no, t, pqt, pq_tok_nb, id, tok_terminal_nb, nb;
  SXINT *tok_no_stack, *top_tok_no_stack, *t_stack, *top_t_stack, *stack;
  struct sxtoken  *semlex_ptok;
  double proba_tok, proba_terminal_sachant_tok;

  if (idag.pqt2proba == NULL) {
    fill_idag_pqt2pq ();

    idag.pqt2proba = (double *) sxcalloc (MAX_pqt () + 1, sizeof (double));
    
    for (from_state = idag.init_state; from_state < idag.final_state; from_state++) {
      bot_pq = idag.p2pq_hd [from_state];
      top_pq = idag.p2pq_hd [from_state+1];
      
      while (bot_pq < top_pq) {
	tok_no = idag.pq2tok_no [bot_pq];
	
	if (tok_no < 0) {
	  tok_no_stack = idag.tok_no_stack - tok_no;
	  pq_tok_nb = *tok_no_stack;
	  top_tok_no_stack = tok_no_stack + pq_tok_nb;
	  tok_no_stack++;
	}
	else {
	  /* truc */
	  top_tok_no_stack = tok_no_stack = &tok_no;
	  pq_tok_nb = 1;
	}
	
	while (tok_no_stack <= top_tok_no_stack) {
	  tok_no = *tok_no_stack++;

	  /* probabilité de la transition */
	  semlex_ptok = &(SXGET_TOKEN (tok_no + 1));
	  
	  if (semlex_ptok->lahead == get_SEMLEX_lahead ()) {
	    proba_tok = atof (sxstrget (semlex_ptok->string_table_entry));
	    //	    proba_trans = fudge_factor * log10 (dynweight);
	  }
	  else {
	    proba_tok = (double)1 / pq_tok_nb ; // A FAIRE: le cas où certaines trans p->q ont un semlex et d'autres pas
	  }

	  id = SXGET_TOKEN (tok_no).lahead;

#ifdef is_probabilized_lexicon
#define MAX_ID MAX_IFT_CODE
#define CODE_STACK ift_code_stack
#else /* is_probabilized_lexicon */
#define MAX_ID EOF_CODE
#define CODE_STACK t_code_stack
#endif /* is_probabilized_lexicon */
#ifndef no_dico_lecl
#ifndef is_LFG
	  if (id > MAX_ID) {
	    /* a cette transition correspondent plusieurs terminaux */
	    stack = CODE_STACK + id - MAX_ID;
	    tok_terminal_nb = nb = *stack;

	    while (nb-- > 0) {
	      id = *++stack; /* un terminal dans le cas normal, un ift dans le cas is_probabilized_lexicon */

#ifdef is_probabilized_lexicon
	      proba_terminal_sachant_tok = ift_code2proba [id];
	      id = ift_code2terminal [id]; /* on rétablit le terminal à partir du ift_code */
#else /* is_probabilized_lexicon */
	      proba_terminal_sachant_tok = (double)1 / tok_terminal_nb;
#endif /* is_probabilized_lexicon */

	      pqt = pq_t2pqt (bot_pq, id);

	      idag.pqt2proba [pqt] += proba_tok * proba_terminal_sachant_tok;
	    }
	  }
	  else {
	    /* à cette transition correspond un seul terminal */
#ifdef is_probabilized_lexicon
	    proba_terminal_sachant_tok = ift_code2proba [id];
	    id = ift_code2terminal [id]; /* on rétablit le terminal à partir du ift_code */
#else /* is_probabilized_lexicon */
	    proba_terminal_sachant_tok = 1;
#endif /* is_probabilized_lexicon */
	   
	    pqt = pq_t2pqt (bot_pq, id);
	   
	    idag.pqt2proba [pqt] += proba_tok * proba_terminal_sachant_tok;
	  }
#endif /* ! is_LFG */	
#endif /* ! no_dico_lecl */	
	}
	
	if ((t_stack = idag.orig_t_stack) == NULL)
	  t_stack = idag.t_stack;

	t_stack += idag.pq2t_stack_hd [bot_pq];

	for (top_t_stack = t_stack + *t_stack; top_t_stack > t_stack; top_t_stack--) {
	  pqt = pq_t2pqt (bot_pq, *top_t_stack);
	   
	  idag.pqt2proba [pqt] = fudge_factor * log10 (idag.pqt2proba [pqt]);
#if EBUG
	  print_idag_trans (from_state, NULL, sxstrget (SXGET_TOKEN (tok_no).string_table_entry), NULL, *top_t_stack, idag.pq2q [bot_pq]);
#endif /* EBUG */
	}


	bot_pq++;
      }
    }
  }
}

/* On a une transition sur t entre les etats p et q
   retourne l'indexe ds t_stack/orig_t_stack qui est interpre`te' comme un pqt */
/* recherche dicho ds idag.t_stack */
SXINT
idag_pq_t2pqt (SXINT pq, SXINT t)
{
  SXINT bot, mid, top, mid_t;
  SXINT bot_pqt;
  SXINT *t_stack;

  /* BS: mais pourquoi diable suppose-t-on que t_stack est trié??? je n'ai pas trouvé de raison, et
     surtout j'ai un cas où ce n'est pas le cas; je laisse dans le doute, mais je rajoute une
     recherche linéaire au cas où la dichotomie aurait échoué */

  /* PB: elle doit etre triee car on y fait une recherche par dichotomie.  Ds le cas de insideG, 
     durant le parseur, idag.t_stack contient les codes des terminaux de insideG, ils ne sont donc
     pas tries.  Il faut utiliser orig_t_stack.  De plus, ds ce cas t est une valeur ds insideG qu'il faut donc
     reconvertir. */

  /* PB: je ne sais plus ou la t_stack est triee => recherche lineaire en attendant mieux (un XxY) */

  /* BS: ... et PB a if0isé la rech dichotomique ;-) */

  if (pq) {
#if 0
    bot_pqt = idag.pq2t_stack_hd [pq];

    if (idag.orig_t_stack)
      t_stack = idag.orig_t_stack + bot_pqt;
    else
      t_stack = idag.t_stack + bot_pqt;

    bot = 1;
    top = *t_stack+1;

    while (bot < top) {
      mid = (top+bot)/2;
      mid_t = t_stack [mid];

      if (mid_t == t)
	return bot_pqt+mid;

      if (mid_t < t)
	bot = mid+1;
      else
	top = mid;
    }

    bot = 1;
    top = *t_stack+1;

    while (bot < top) {
      if (t_stack [bot] == t)
	return bot_pqt+bot;

      bot++;
    }
#endif /* 0 */

    bot_pqt = idag.pq2t_stack_hd [pq];

    if (idag.orig_t_stack)
      t_stack = idag.orig_t_stack + bot_pqt;
    else
      t_stack = idag.t_stack + bot_pqt;

    top = *t_stack;

    while (top > 0) {
      if (t_stack [top] == t)
	return bot_pqt+top;

      top--;
    }
    
  }

  return 0;
}


#if 0
/* Remplace' le 28/10/08 par une macro qui accede directement a` pq */
/* On a une transition entre les etats p et q
   On retourne le "pq" correspondant a cette transition */
/* recherche dicho ds idag.p2pq_hd */
SXINT
idag_p_q2pq (SXINT p, SXINT q)
{
  SXINT bot, mid, top, mid_q;

  bot = idag.p2pq_hd [p];
  top = idag.p2pq_hd [p+1];

  while (bot < top) {
    mid = (top+bot)/2;
    mid_q = idag.pq2q [mid];

    if (mid_q == q)
      return mid;

    if (mid_q < q)
      bot = mid+1;
    else
      top = mid;
  }

  return 0;
}
#endif /* 0 */


/* Remplit la dstack passee en parametre avec tous les tok_no qui correspondent a p t q */
/* Cette fonction est appelee depuis spf_fill_Tij2tok_no du sxspf_mngr */
void
idag_p_t_q2tok_no_stack (SXINT **tok_no_stack_ptr, SXINT p, SXINT t, SXINT q)
{
  SXINT pq, tok_no;
  SXINT *tok_no_stack, *top_tok_no_stack, *local_t_stack, *top_local_t_stack;

  pq = p_q2pq (p, q);

  tok_no = idag.pq2tok_no [pq];

  if (tok_no < 0) {
    tok_no_stack = idag.tok_no_stack - tok_no;
    top_tok_no_stack = tok_no_stack + *tok_no_stack;
    tok_no_stack++;
  }
  else {
    /* truc */
    top_tok_no_stack = tok_no_stack = &tok_no;
  }

  while (tok_no_stack <= top_tok_no_stack) {
    tok_no = *tok_no_stack++;
    local_t_stack = idag.local_t_stack + idag.tok_no2t_stack_hd [tok_no];

    top_local_t_stack = local_t_stack + *local_t_stack;

    while (++local_t_stack <= top_local_t_stack) {
      if (*local_t_stack == t) {
	/* La transition tok_no entre p et q contient bien le terminal t */
	DPUSH (*tok_no_stack_ptr, tok_no);
	break;
      }
    }
  }
}



void
fill_idag_path (void)
{
  SXINT from_state, to_state, bot_pq2q, top_pq2q;
  SXBA  line;

  idag.path = sxbm_calloc (idag.final_state+1, idag.final_state+1); 

  for (from_state = idag.init_state; from_state < idag.final_state; from_state++) {
    bot_pq2q = idag.p2pq_hd [from_state];
    top_pq2q = idag.p2pq_hd [from_state+1];

    line = idag.path [from_state];

    while (bot_pq2q < top_pq2q) {
      to_state = idag.pq2q [bot_pq2q];
      SXBA_1_bit (line, to_state);
      bot_pq2q++;
    }
  }

  fermer2 (idag.path, idag.final_state+1);
}


/* Concatene les commentaires du sous dag entre les e'tats p et q de dag_hd ds un varstr */
/* PB que faire s'il n'y a pas de chemin entre p et q ? */
/* On va etre brutal */
/* Si un commentaire commence par un '{' ou finit par un '}', on l'enleve (l'appelant retablira eventuellement
   les '{' et '}' englobant) */
VARSTR
sub_dag_to_comment (VARSTR vstr, SXINT p, SXINT q)
{    
  SXINT   from_state, to_state, bot_pq2q, top_pq2q, tok_no, top_tok_no;
  SXINT   *tok_no_stack, *top_tok_no_stack;
  SXBA    line;
  char    *comment;
  SXBA    state_set;
  bool is_first_time;

  if (idag.path == NULL)
    fill_idag_path ();

  if (!SXBA_bit_is_set (idag.path [p], q))
    return vstr; /* pas de chemin, on ne fait rien */

  state_set = idag.path [0];
  is_first_time = true;
  from_state = p;

  do {
    bot_pq2q = idag.p2pq_hd [from_state];
    top_pq2q = idag.p2pq_hd [from_state+1];

    line = idag.path [from_state];

    while (bot_pq2q < top_pq2q) {
      to_state = idag.pq2q [bot_pq2q];

      if (to_state == q || to_state < q && SXBA_bit_is_set (idag.path [to_state], q) /* y'a un chemin */) {
	/* On concatene les commentaires des tokens qui vont de p a q */
	tok_no = idag.pq2tok_no [bot_pq2q];

	if (tok_no < 0) {
	  tok_no_stack = idag.tok_no_stack - tok_no;
	  top_tok_no_stack = tok_no_stack + *tok_no_stack;
	  tok_no_stack++;
	}
	else {
	  /* truc */
	  top_tok_no_stack = tok_no_stack = &tok_no;
	}

	while (tok_no_stack <= top_tok_no_stack) {
	  tok_no = *tok_no_stack++;
	  comment = SXGET_TOKEN (tok_no).comment;

	  if (comment ) {
	    if (is_first_time) {
	      is_first_time = false;
	      vstr = varstr_catenate (vstr, comment);
	    }
	    else {
	      varstr_pop (vstr, 1); /* On enleve le "}" final */
	      vstr = varstr_catchar (vstr, ' ');
	      vstr = varstr_lcatenate (vstr, comment+1, strlen (comment)-1); /* On enleve le "{" initial du suffixe */
	    }
	  }
	}

	if (to_state < q)
	  SXBA_1_bit (state_set, to_state);
      }

      bot_pq2q++;
    }
  } while ((from_state = sxba_scan_reset (state_set, from_state)) > 0);

  return vstr;
}


/* Procedure appelee par dag2re qui permet de recuperer le "texte" associe' aux transitions qui sont ici des tok_no */
/* Marche avec le (vrai) source et non pas le source lexicalise' ds le cas insideG */
static char*
sub_dag_edges (SXINT tok_no)
{
  return sxstrget (SXGET_TOKEN (tok_no).string_table_entry);
}

/* Procedure appelee par dag2re qui permet d'extraire de idag les transitions depuis l'etat p */
void
dag_extract_trans (SXINT p, void (*f) (SXINT p, SXINT t, SXINT q))
{
  SXINT bot_pq, top_pq, q, tok_no;
  SXINT *tok_no_stack, *top_tok_no_stack;

  bot_pq = idag.p2pq_hd [p];
  top_pq = idag.p2pq_hd [p+1];

  while (bot_pq < top_pq) {
    q = idag.pq2q [bot_pq];
    tok_no = idag.pq2tok_no [bot_pq];

    if (tok_no < 0) {
      tok_no_stack = idag.tok_no_stack - tok_no;
      top_tok_no_stack = tok_no_stack + *tok_no_stack;
      tok_no_stack++;
    }
    else {
      /* truc */
      top_tok_no_stack = tok_no_stack = &tok_no;
    }

    while (tok_no_stack <= top_tok_no_stack) {
      tok_no = *tok_no_stack++;
      /* J'ai pris le tok_no comme transition */
      (*f) (p, tok_no, q);
    }

    bot_pq++;
  }
}

/* Concatene l'expression reguliere du sous dag (en terme de terminaux) entre les e'tats p et q de dag_hd ds vstr */
VARSTR
sub_dag_to_re (VARSTR vstr, SXINT p, SXINT q)
{    
  if (idag.path == NULL)
    fill_idag_path ();

  if (!SXBA_bit_is_set (idag.path [p], q))
    return vstr; /* pas de chemin, on ne fait rien */
    
  vstr = dag2re (vstr, p, q, idag.toks_buf_top /* ?? */, dag_extract_trans, sub_dag_edges);

  return vstr;
}



/* Retourne le pointeur vers t2string */
char **
idag_get_t2string (void)
{
#if (dico_contains_terminals) && defined(has_t2string)
  return &(t2string [0]);
#else /* def_t2string */
  return NULL;
#endif /* def_t2string */
}

#if 0
/* obsolete */
#if !(defined (__GNUC__) && !EBUG)
SXINT
idag_p_q2pq (SXINT p, SXINT q) {
  SXINT *pq_base_ptr;

  return ((q <= p || q > *(pq_base_ptr = idag.pq_base2pq+idag.p2pq_base [p]))
	  ? 0
	  : pq_base_ptr [q-p]);
}

SXINT
MAKE_AN_IDAG_RANGE (SXINT p, SXINT q) {
  SXINT pq;

  return ((pq = idag_p_q2pq (p, q)) == 0
	  ? X_is_set (&(idag.ranges), (p<<idag.logn)+q)+idag.last_pq
	  : pq);
}
#endif /* !(defined (__GNUC__) && !EBUG) */
#endif /* 0 */
