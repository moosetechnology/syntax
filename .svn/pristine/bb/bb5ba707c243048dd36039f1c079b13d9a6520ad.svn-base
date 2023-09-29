/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 2003 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *     Produit de l'equipe ATOLL.			  *
   *                                                      *
   ******************************************************** */

/* Le couple dag_scanner/read_a_dag simule un scanner non-deterministe pour un analyseur Earley.
   Ce scanner non-deterministe est en fait un analyseur syntax dont la semantique est traitee par arbre abstrait.
   Techniquement, read_a_dag est la passe semantique (parcours d'un arbre abstrait) associe a un scanner/parser
   qui prend en entree un DAG.  Les tables de ce scanner/parser sont ds l'include dag_t.h qui sont une copie
   de dag_t.c produite par l'autogenese de $sx/DAG.  read_a_dag est appele depuis dag_scanner.c qui est
   ds la librairie SYNTAX en $sx/src, qui il doit etre recompilé par l'utilisateur, qui doit l'instancier
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

/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* Mar 4 Mar 2003 13:10(PB)	Ajout de cette rubrique "modifications"	*/
/************************************************************************/

static char	ME [] = "udag_scanner.c";

#include "sxunix.h"
#include "udag_scanner.h"
#include "varstr.h"
#include "earley.h"

char WHAT_UDAG_SCANNER[] = "@(#)SYNTAX - $Id: udag_scanner.c 912 2008-01-09 11:23:50Z syntax $" WHAT_DEBUG;

#include "fsa.h" /* Pour read_a_re () */

extern SXINT  read_a_udag (void (*store)() /* store_dag */, void (*pre_fill)(), void (*fill)(),
			   SXINT  (*check)(), void (*raz)() /* raz_dag */, SXINT (*parser)());
extern SXINT *sdag_get_t_tok_no_stack (SXINT id);

#define TOK_NB ((sizeof (tok_str)/sizeof (tok_str [0])))

static struct sxtables *tables;

#ifdef tdef_by_dico
#undef tdef_by_dico
#endif /* tdef_by_dico */

#ifdef no_dico_lecl
#undef no_dico_lecl /* prudence */
#endif /* no_dico_lecl */

#ifdef keywords_h
   /* Les tables keywords_h proviennent directement de l'execution de bnf avec l'option -huge
      La grammaire n'a presque rec,ue aucun traitement.  Les terminaux de la grammaire sont les formes flechies */
   /* Cet include contient 
      #define HUGE_CFG
   */
#  define tdef_by_dico
#  define def_sxdfa_comb_inflected_form
#  include keywords_h
   /* Ici on a un dico formes_flechies --> code (du terminal) et EOF_CODE est defini */
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
     static struct sxdfa_comb *dico_des_terminaux = &sxdfa_comb_terminal2code;
#  else
#    ifdef sxdfa_comb_dico_h
       /* Les tables sxdfa_comb_dico_h proviennent de make_a_dico (ou assimile') */
#      define tdef_by_dico

#      undef EOF_CODE
#      define def_sxdfa_comb_terminal2code
#      define def_sxdfa_comb_inflected_form
#      include sxdfa_comb_dico_h
       /* Ici on a un dico terminal --> code du terminal
	  ...  et EOF_CODE est defini
          ... et un dico   formes_flechies --> id
          Si id <= EOF_CODE, c'est le code du (seul) terminal associe' a la ff
	  Sinon t_code_stack + (id-EOF_CODE) est une stack qui contient la liste des terminaux associes a ff */
       static struct sxdfa_comb *dico_des_terminaux = &sxdfa_comb_terminal2code;
#    else /* sxdfa_comb_dico_h */
     /* on n'a pas de dico (anciennement -Dno_dico)*/
#      define no_dico_lecl
#      define EOF_CODE sxeof_code (tables)
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
static VARSTR     vstr;

/* Toujours visible car sxspf_mngr.c en a besoin !! (pas terrible, A VOIR) */
SXINT  SEMLEX_lahead;

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
  BOOLEAN has_changed;
  
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
    has_changed = FALSE;

    while (--cur_str >= str) {
      *--cur_wstr = c = sxtolower (*cur_str);

      if (c != *cur_str)
	has_changed = TRUE;
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
    has_changed = FALSE;

    while (--cur_str >= str) {
      *--cur_wstr = c = sxtoupper (*cur_str);

      if (c != *cur_str)
	has_changed = TRUE;
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
  BOOLEAN is_capitalized_initial;

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
print_idag_trans (SXINT from_state, char *comment, char *ff, SXINT t, SXINT to_state)
{
  printf ("%i\t%s %s/%s\t%ld\n", from_state, comment ? comment : "", ff, t2string (t), to_state);
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
    
    if (id2 == 0) {
      fprintf (sxstderr, "%s: Invalid option for unknown words, \"%s\" must be a valid capitalized initial inflected form.\n", ME, ciu_name_ptr);
      SXEXIT (6);
    }
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
    
    if (id2 == 0) {
      fprintf (sxstderr, "%s: Invalid option for unknown words, \"%s\" must be a valid lower case initial inflected form.\n", ME, lciu_name_ptr);
      SXEXIT (6);
    }
  }
}
#endif /* tdef_by_dico */




/* Utilise' par la lexicalisation ou les parseurs */
/* ca ne peut pas etre fait avec fill_idag_infos car :
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
  SXINT  from_state, to_state, bot_pq2q, top_pq2q, lgth, t;
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
    bot_pq2q = idag.p2pq_hd [from_state];
    top_pq2q = idag.p2pq_hd [from_state+1];

    while (bot_pq2q < top_pq2q) {
      to_state = idag.pq2q [bot_pq2q];
      if (t2suffix_t_set) next_suffix_source_set = mlstn2suffix_source_set [to_state];
      base_t_stack = idag.t_stack + idag.pq2t_stack_hd [bot_pq2q];

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

      bot_pq2q++;
    }
  }
}

/* Si on a besoin de recuperer les transitions backward */
void
fill_idag_q2pq_stack (void)
{
  SXINT p, q, bot_pq, top_pq, nb, top;
  SXINT *q2p_nb, *pq_stack;

  if (idag.q2pq_stack_hd == NULL) {
    q2p_nb = (SXINT *) sxcalloc (idag.final_state+1, sizeof (SXINT)), nb = 0;
    idag.pq2p = (SXINT *) sxcalloc (idag.final_state+1, sizeof (SXINT)), idag.pq2p [0] = 0;

    for (p = idag.init_state; p < idag.final_state; p++) {
      bot_pq = idag.p2pq_hd [p];
      top_pq = idag.p2pq_hd [p+1];

      while (bot_pq < top_pq) {
	q = idag.pq2q [bot_pq];
	idag.pq2p [bot_pq] = p;
	q2p_nb [q]++;
	nb++;

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

/* Ds le cas dag_kind==SDAG_KIND */
extern SXINT *sdag_get_t_tok_no_stack (SXINT id);

static void
fill_idag_infos (void)
{
  SXINT            from_state, to_state, bot_pq, top_pq, tok_no, lgth, cur_lgth, id, top, cur, tok, cur_tok, ste;
  SXINT            which_dico, nb, cur_t_stack_bot, cur_local_t_stack_bot, prev_tok_no;
  SXINT            *stack, *tok_no_stack, *top_tok_no_stack, *sdag_t_tok_no_stack, *top_sdag_t_tok_no_stack;
  SXINT            save_Mtok_no;
  char             *str, *cur_str, *ret_str, *comment;
  BOOLEAN          success;
  struct sxtoken   *ptok;
  SXBA             glbl_source_i, current_trans_t_set;
  struct sxsrcmngr save_sxsrcmngr;
  struct sxsvar    save_sxsvar;
#if EBUG
  char             *ff_str;
#endif /* EBUG */

  End_Of_Sentence_ste = (SXINT) sxstr2save ("_End_Of_Sentence_", 17);

#ifndef is_LFG
  if (tables) {
    save_Mtok_no = sxplocals.Mtok_no;
    /* On peut utiliser "tables" pour re_scanner les chaines traitees par dag_scanner et recuperer ainsi leur
       vrais codes internes donnes par les dummy_tables */
    /* On "ferme" le source mngr qui a lu le DAG */
    save_sxsrcmngr = sxsrcmngr;
    save_sxsvar = sxsvar;
    sxsrc_mngr (FINAL); /* Celui du DAG */
    //    sxtkn_mngr (OPEN, 1); /* Gestion des nouveaux tokens */
    (*(tables->analyzers.scanner)) (OPEN, tables); /* On prepare le scanneur de "tables" */
  }
#endif /* is_LFG */

  current_trans_t_set = sxba_calloc (EOF_CODE+1);
  idag.source_set = sxba_calloc (EOF_CODE+1);

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
	/* Ds ce cas tok_no designe la pile ... */
	sdag_t_tok_no_stack = sdag_get_t_tok_no_stack (tok_no);
#if EBUG
	ff_str = tok_no ? sxstrget (SXGET_TOKEN (tok_no).string_table_entry) : "<no_ff>";
#endif /* EBUG */

#if 0
	/* Pour l'instant, on ne fait rien de la ff eventuelle */
	/* Ci-dessous si on considere la liste des terminaux comme etant des terminaux "supplementaires" 
	 Auquel cas il faut passer ds la boucle "while (tok_no_stack <= top_tok_no_stack) {" */
	tok_no = sdag_t_tok_no_stack [1];
	idag.pq2tok_no [bot_pq] = tok_no; /* Attention, peut etre nul */

	if (tok_no) {
	  /* Y'a une ff ds le coup, c'est elle qu'on met ds bot_pq */
	  /* truc */
	  top_tok_no_stack = tok_no_stack = &tok_no;
	}
	else {
	  /* Sinon pas de ff ds le coup */
	  /* ... et on ne fait aucune recherche sur la ff */
	  /* truc */
	  top_tok_no_stack = &tok_no;
	  tok_no_stack = top_tok_no_stack+1;
	}
#endif /* 0 */
      }
      else {
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

	  ptok = &(SXGET_TOKEN (tok_no));
	  ptok->lahead = 0; /* a priori */

	  ste = ptok->string_table_entry;
	  str = sxstrget (ste);
#if EBUG
	  comment = ptok->comment;
#endif /* EBUG */
    
	  if (ste != End_Of_Sentence_ste) {
	    success = FALSE;

	    if (ste != ERROR_STE) {
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
		  success = TRUE;

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
			t_occur_nb++;

			if (!SXBA_bit_is_set (current_trans_t_set, cur_tok))
			  DPUSH (idag.t_stack, cur_tok);

			DPUSH (idag.local_t_stack, cur_tok);

			SXBA_1_bit (idag.source_set, cur_tok);

#if EBUG
			print_idag_trans (from_state, comment, str, cur_tok, to_state);
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

		  if (id > EOF_CODE) {
		    /* a une forme flechie correspond plusieurs terminaux */
		    stack = t_code_stack + id - EOF_CODE;
		    nb = *stack;
		    t_occur_nb += nb;

		    while (nb-- > 0) {
		      id = *++stack;

		      if (SXBA_bit_is_reset_set (current_trans_t_set, id))
			DPUSH (idag.t_stack, id);

		      DPUSH (idag.local_t_stack, id);
		      SXBA_1_bit (idag.source_set, id);

#if EBUG
		      print_idag_trans (from_state, comment, str, id, to_state);
#endif /* EBUG */
		    }
		  }
		  else {
		    t_occur_nb++;

		    if (SXBA_bit_is_reset_set (current_trans_t_set, id))
		      DPUSH (idag.t_stack, id);

		    DPUSH (idag.local_t_stack, id);
		    SXBA_1_bit (idag.source_set, id);

#if EBUG
		    print_idag_trans (from_state, comment, str, id, to_state);
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
		if (tables->SXS_tables.check_keyword)
		  id = (*tables->SXS_tables.check_keyword) (str, lgth);
		else
		  /* Les tables qui permettent de recuperer les vrais codes des terminaux n'ont pas de mots-clefs */
		  id = 0;

		if (id == 0) {
		  /* On scanne grace aux tables */
		  sxsrc_mngr (INIT, NULL, str); /* On va scanner str */
		  sxnext_char (); /* initialisation de la lecture */
		  //		  sxtkn_mngr (INIT, 0); /* Resultat a l'indice 1 */
		  (*(tables->SXP_tables.scanit)) (); /* On le scanne */
		  id = SXGET_TOKEN (sxplocals.Mtok_no).lahead; /* ... et voila le resultat */

		  if (id) {
		    /* Il se peut qu'on vienne de ne reconnaitre qu'un prefixe de str */
		    /* En fait il faudrait fabriquer des ids (et les transitions ds le dag)
		       tant que str n'est pas epuise' !! */
		    /* Ici on fait simple ... */
		    (*(tables->SXP_tables.scanit)) (); /* On scanne le suivant ... */
	    
		    /* ... qui doit etre eof */
		    if (SXGET_TOKEN (sxplocals.Mtok_no).lahead != sxeof_code (tables))
		      id = 0;
		  }
		}
	    
		if (id) {
		  success = TRUE;
		  ptok->lahead = id;

		  if (SXBA_bit_is_reset_set (current_trans_t_set, id))
		    DPUSH (idag.t_stack, id);

		  DPUSH (idag.local_t_stack, id);
		  SXBA_1_bit (idag.source_set, id);
#if EBUG
		  print_idag_trans (from_state, comment, str, id, to_state);
#endif /* EBUG */
		}
	      }
#endif /* ifndef is_LFG */
	    }

	    if (!success) {
	      sxput_error (ptok->source_index,
			   "%sUnknown word \"%s\".",
			   sxsvar.sxtables->err_titles [1],
			   str);
	      /* Le 03/05/04 Erreur est change' en Warning, les mots inconnus ne bloquent plus l'analyse */
	      /* Le 21/03/07 On sort le mot inconnu (le marqueur ds le source ne suffit pas ds le cas ou le source tient sur une ligne) */
	  
	      ptok->lahead = 0;

	      /* Essai le 22/12/05 */
	      /* On met le terminal de code 0 sur les mots inconnus */
	      if (SXBA_bit_is_reset_set (current_trans_t_set, 0))
		DPUSH (idag.t_stack, 0);

	      DPUSH (idag.local_t_stack, 0);
	      SXBA_1_bit (idag.source_set, 0);
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
	    print_idag_trans (from_state, comment, str, EOF_CODE, to_state);
#endif /* EBUG */
	  }

	  top = TOP (idag.local_t_stack);
	  idag.local_t_stack [cur_local_t_stack_bot] = top - cur_local_t_stack_bot;
	  idag.tok_no2t_stack_hd [tok_no] = cur_local_t_stack_bot;
	}
      }

      if (dag_kind == SDAG_KIND && dico_des_terminaux) {
	/* On exploite les terminaux associes a la ff */
	top_sdag_t_tok_no_stack = sdag_t_tok_no_stack++ /* On saute la ff */ + *sdag_t_tok_no_stack;

	while (++sdag_t_tok_no_stack < top_sdag_t_tok_no_stack) {
	  tok_no = *sdag_t_tok_no_stack;

	  ptok = &(SXGET_TOKEN (tok_no));
	  ptok->lahead = 0; /* a priori */

	  ste = ptok->string_table_entry;
	  str = sxstrget (ste);
#if EBUG
	  comment = ptok->comment;
#endif /* EBUG */
	  if (ste != ERROR_STE) {
	    lgth = sxstrlen (ste);

	    id = sxdfa_comb_seek_a_string (dico_des_terminaux, str, &lgth);

	    if (lgth != 0)
	      id = 0;

	    if (id) {
	      /* Il faut pusher le tok_no de ce terminal */
	      prev_tok_no = idag.pq2tok_no [bot_pq];

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
#if 0
		if (prev_tok_no > 0) {
		  /* 2eme trans */
		  PUSH (idag.tok_no2t_stack_hd, 2);
		  idag.pq2tok_no [bot_pq] = -TOP (idag.tok_no2t_stack_hd); /* en negatif c'est l'index vers la sous-pile */
		  PUSH (idag.tok_no2t_stack_hd, prev_tok_no);
		}
		else {
		  idag.tok_no2t_stack_hd [-prev_tok_no]++; /* On va pusher un nouvel elem */
		}
	
		PUSH (idag.tok_no2t_stack_hd, tok_no);
#endif /* 0 */
	      }

	      ptok->lahead = id;
	      t_occur_nb++;

	      if (SXBA_bit_is_reset_set (current_trans_t_set, id))
		DPUSH (idag.t_stack, id);

	      DPUSH (idag.local_t_stack, 1); /* Les t associes a chaque tok_no */
	      idag.tok_no2t_stack_hd [tok_no] = TOP (idag.local_t_stack);
	      DPUSH (idag.local_t_stack, id);

	      SXBA_1_bit (idag.source_set, id);

#if EBUG
	      print_idag_trans (from_state, comment, ff_str, id, to_state);
#endif /* EBUG */
	    }
	  }

	  /* Le semlex A voir */
	  tok_no = *++sdag_t_tok_no_stack;
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
    (*(tables->analyzers.scanner)) (CLOSE, tables); /* Fin du scanneur de "tables" */
    //    sxtkn_mngr (CLOSE, 0); /* Soyons propres */
    sxsrc_mngr (FINAL); /* Celui de "tables" */
    sxsrcmngr = save_sxsrcmngr; /* C,a permet a sxput_error de ressortir le source!! */
    sxsvar = save_sxsvar;
    sxplocals.Mtok_no = save_Mtok_no;
  }
#endif /* is_LFG */

  idag.repair_toks_buf_top = idag.toks_buf_top; /* vers le eof de fin */
  SXBA_1_bit (idag.source_set, EOF_CODE); /* L'eof de fin */
}


/* Cas DAG_KIND */
static struct fa {
  SXINT          fsa_kind, init_state, final_state, sigma_card, eof_ste, transition_nb;
  XxYxZ_header fsa_hd;
  BOOLEAN      has_epsilon_trans;
} dfa, *cur_fsa;


/* appele' depuis les "actions" du parseur de udag sur chaque transition */
static void
store_idag (SXINT from_state, SXINT tok_no, SXINT to_state)
{
  SXINT            pq, prev_tok_no;

  /* Ne rien faire sur les tokens, ils sont deja ds sxplocals.toks_buf */

  /* Pour un udag on a from_state < to_state
     Si le udag est normalise', pour 2 appels consecutifs (p, t, q), (p', t', q') on a :
     Soit p' == p soit p' == p+1
     Si p' == p on a q' >= q
     Si p' == p et q' == q on a t' > t */
  if (dag_kind == DAG_KIND) {
    /* store_idag est appele' depuis dfa_minimize de postlude_re */
    if (to_state == cur_fsa->final_state && from_state+1 == to_state)
      /* On supprime la "derniere" trans sur eof */
      return;
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
  }
  else {
    pq = TOP (idag.pq2q);

    if (idag.pq2q [pq] < to_state) {
      /* transition depuis from_state vers un etat different du coup precedent */
      /* On empile */
      PUSH (idag.pq2q, to_state);
      idag.pq2tok_no [pq+1] = tok_no;
    }
    else {
      /* transition multiple de p vers q (avec un tok_no different) */
#if EBUG
      if (idag.pq2q [pq] != to_state)
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

#if 0
      if (prev_tok_no > 0) {
	/* 2eme trans */
	PUSH (idag.tok_no2t_stack_hd, 2);
	idag.pq2tok_no [pq] = -TOP (idag.tok_no2t_stack_hd); /* en negatif c'est l'index vers la sous-pile */
	PUSH (idag.tok_no2t_stack_hd, prev_tok_no);
      }
      else {
	idag.tok_no2t_stack_hd [-prev_tok_no]++; /* On va pusher un nouvel elem */
      }
	
      PUSH (idag.tok_no2t_stack_hd, tok_no);
#endif /* 0 */
    }
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
  idag.final_state = idag.pq2q [last_pq];
  PUSH (idag.pq2q, 0);

#if EBUG
  if (TOP (idag.p2pq_hd) != idag.final_state-1)
    sxtrap (ME, "after_last_idag_trans");
#endif /* EBUG */

  idag.p2pq_hd [idag.final_state] = last_pq+1;

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

  if (what == ACTION) {
    for (tok_no = 1; tok_no < sxplocals.Mtok_no; tok_no++) {
      store_idag (tok_no, tok_no, tok_no+1);
    }
  }
  else
    /* prudence */
    sxparser (what, arg);
    
}

static char *dag_kind_strings [] = {"??", "Dag", "Udag", "Sdag", "String"};
/* Ca permet de prevenir si le nombre de terminaux depasse une valeur limite (source trop gros pour etre traite') */
/* Pas utilisation en stand alone */
static SXINT
check_udag ()
{
  BOOLEAN oversized;
  char    mess [128];
  int   severity = 0;

  oversized = ((SXUINT) t_occur_nb > maximum_input_size);
  
  if (is_print_time) {
    sprintf (mess, "\t%s Scanner[final_state=%ld, transition_nb=%ld, terminal_nb=%ld%s]", dag_kind_strings [dag_kind], idag.final_state /* Etat final */, idag.toks_buf_top-1 /* nb de transitions */, t_occur_nb, oversized ? "(oversized)" : "");
    sxtime (ACTION, mess);
  }
  
  if (oversized)
    severity = 7;

  return severity;
}

/* Le scanneur de udag vient de se terminer, on a donc une idee du nb de transitions */
static void
idag_free ()
{
  if (idag.p2pq_hd) {
    sxfree (idag.p2pq_hd), idag.p2pq_hd = NULL;
    sxfree (idag.pq2q), idag.pq2q = NULL;
    sxfree (idag.pq2tok_no), idag.pq2tok_no = NULL;

    if (idag.tok_no_stack) sxfree (idag.tok_no_stack), idag.tok_no_stack = NULL;

    sxfree (idag.tok_no2t_stack_hd), idag.tok_no2t_stack_hd = NULL;

    sxfree (idag.pq2t_stack_hd), idag.pq2t_stack_hd = NULL;
    DFREE_STACK (idag.t_stack);
    DFREE_STACK (idag.local_t_stack);
    sxfree (idag.source_set), idag.source_set = NULL;

    if (idag.q2pq_stack_hd) {
      sxfree (idag.q2pq_stack_hd), idag.q2pq_stack_hd = NULL;
      sxfree (idag.q2pq_stack), idag.q2pq_stack = NULL;
      sxfree (idag.pq2p), idag.pq2p = NULL;
    }

    if (idag.path)
      sxbm_free (idag.path), idag.path = NULL;
  }
}

static void
idag_alloc (SXINT size)
{
  idag.p2pq_hd = (SXINT *) sxalloc (size+2, sizeof (SXINT)), idag.p2pq_hd [0] = 0;
  idag.pq2q = (SXINT *) sxalloc (size+2, sizeof (SXINT)), RAZ (idag.pq2q);
  idag.pq2tok_no = (SXINT *) sxalloc (size+2, sizeof (SXINT)), RAZ (idag.pq2tok_no);
  idag.tok_no_stack = (SXINT *) sxalloc (2*size+2, sizeof (SXINT)), RAZ (idag.tok_no_stack);
  idag.tok_no2t_stack_hd = (SXINT *) sxalloc (size+1, sizeof (SXINT)), idag.tok_no2t_stack_hd [0] = 0;
}


static void
raz_idag ()
{
  idag_free ();
  /* Prudence */
  idag_alloc (sxplocals.Mtok_no);
}



/* Cas SDAG_KIND */
/* Appele' depuis read_a_sdag */
static void
store_sdag_trans (SXINT p, SXINT id, SXINT q)
{
  SXINT top;

  /* id est un identifiant qui designe la transition de p -> q
     On pourra ds fill_idag_infos tout recupe'rer a` partir de id */
  /* Que faut-il faire pour que le futur appel a fill_idag_infos marche ? */
  /* Les couples de tok_no de t_tok_no_stack repe`re des couples (t, semlex)
     t doit etre recherche' ds dico_des_terminaux */
  /* Si ff_tok_no est non nul, ff peut etre recherchee ds les dicos */
  PUSH (idag.pq2q, q);
  top = TOP (idag.pq2q);
  PUSH (idag.p2pq_hd, top);
  idag.pq2tok_no [top] = id; /* On sait que ce n'est pas un tok_no car dag_kind==SDAG_KIND */
}



/* Cette procedure est appelee depuis read_a_re a` la fin de la 1ere passe sur l'arbre abstrait */
/* Les arguments sont 
   - TRUE => OK
   - ste des transitions vers l'etat final
   - le nb de noeuds de cet arbre et 
   - le nb d'operande (dont eof + 1 bidon de fin) */
/* Peut servir a faire des alloc + ciblees */
static void
prelude_re (BOOLEAN is_OK, SXINT eof_ste, SXINT node_nb, SXINT operand_nb, SXINT fsa_kind)
{
  SXINT *fsa_foreach;
#if EBUG
  char *name;
#endif
  SXINT foreach [6] = {1, 0, 0, 0, 0, 0};

  if (is_OK) {
#if LOG
    sxtime (INIT, NULL);
#endif /* LOG */

#if EBUG
    name = "RE2DFA";
#endif
    fsa_foreach = foreach;
    cur_fsa = &dfa;

    cur_fsa->fsa_kind = fsa_kind;
    cur_fsa->eof_ste = eof_ste;
    cur_fsa->init_state = 1;
    XxYxZ_alloc (&(cur_fsa->fsa_hd), "nfsa_hd", node_nb+1, 1, fsa_foreach, NULL /* pas de re_oflw */, NULL); 

#if EBUG
    printf ("\n%s\n", name);
#endif
  }
  else
    /* PB ds la lecture de l'ER (pas un dag, ...) */
    SXEXIT (2);

  /* On remet a zero sxplocals.toks_buf pour store_re */
  sxtkn_mngr (INIT, 0);
}

/* Cette procedure est appelee depuis read_a_re sur chaque transition de l'automate produit */
/* L'etat initial est 1 */
/* l'etat final unique est atteint par transition sur le terminal bidon "<EOF>" de ste eof_ste et vaut -f */
/* Tous les etats de 1 a f sont utilises */
/* ptok_ptr contient {NULL, ptok1, ...ptokm, NULL} */
/* ste est commun a tous les ptok de ptok_ptr */
/* ptoki est un pointeur vers un token courant (dans le noeud de l'arbre abstrait) */
static void
store_re (SXINT state, struct sxtoken **ptok_ptr, SXINT ste, SXINT next_state)
{
  SXINT            triple, lgth;
  struct sxtoken   tok, *ptok;

  /* Normalement les transitions epsilon ont ete supprimees (read_a_re a ete appele' avec l'option RE2DFA) */
#if EBUG
  if (ptok_ptr == NULL && ste == 0)
    /* Transition epsilon */
    sxtrap (ME, "store_re");

#if 0
  /* Sinon, il faudrait faire un truc du genre ... */
  if (ptok_ptr == NULL && ste == 0) {
    /* Transition epsilon */
    cur_fsa->has_epsilon_trans = TRUE;
    XxYxZ_set (&(cur_fsa->fsa_hd), state, 0, next_state, &triple);
#if EBUG
    printf ("%i\t\"<EPSILON>\"\t\t%i\n", state, next_state);
#endif /* EBUG */
  }
#endif /* 0 */
#endif /* EBUG */

  /* Ici on exploite ptok_ptr qui contient {NULL, ptok1, ...ptokm, NULL}
     ou chaque ptoki repere un sxtoken dans l'arbre abstrait dont le string_table_entry est ste */
  if (ste == cur_fsa->eof_ste)
    cur_fsa->final_state = next_state;

  /* On cumule les comments de tous les tok de ptptok */
  varstr_raz (vstr);
  tok = *(ptok = *++ptok_ptr);

  if (tok.comment)
    vstr = varstr_catenate (vstr, tok.comment);

  while ((ptok = *++ptok_ptr) != NULL) {
    if (ptok->comment) {
      varstr_pop (vstr, 1); /* On enleve le "}" final */
      vstr = varstr_catchar (vstr, ' ');
      vstr = varstr_catenate (vstr, ptok->comment+1); /* On enleve le "{" initial du suffixe */
    }
  }

  if ((lgth = varstr_length (vstr)) > 0) {
    tok.comment = sxstrget (sxstr2save (varstr_tostr (vstr), lgth));
  }

  tok.string_table_entry = ste; /* prudence, c'est pas fait sur eof */

  sxput_token (tok); /* mis en sxplocals.Mtok_no */
  XxYxZ_set (&(cur_fsa->fsa_hd), state, sxplocals.Mtok_no, next_state, &triple);
#if EBUG
  printf ("%i\t%s %s\t\t%i\n", state, tok.comment ? tok.comment : "", sxstrget (ste), next_state);
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
static void
postlude_re (SXINT fsa_kind)
{
#if LOG
  static char           mess [128];
#endif /* LOG */

  /* Attention, il faut changer cur_fsa->eof_ste car les transitions ne sont pas des ste mais des tok_no */
  cur_fsa->eof_ste = sxplocals.Mtok_no; /* Le tok_no de "eof" */
  cur_fsa->transition_nb = XxYxZ_top (cur_fsa->fsa_hd);

#if LOG
  sprintf (mess, "RE2DFA: state_nb = %i", cur_fsa->final_state);
  sxtime (ACTION, mess);
#endif /* LOG */

#if EBUG
  fputs ("DFA2min_DFA\n", stdout);
#endif /* EBUG */

  idag_free (); /* Prudence */
  idag_alloc (cur_fsa->transition_nb);
  
  dfa_minimize (1, cur_fsa->final_state, cur_fsa->eof_ste, DFA_extract_trans, store_idag, TRUE /* to_be_normalized */);

  after_last_idag_trans ();

#if LOG
  sprintf (mess, "DFA2min_DFA: state_nb = %i, total_trans_nb = %i", idag.final_state, idag.toks_buf_top-1);
  sxtime (ACTION, mess);
#endif /* LOG */

  fill_idag_infos ();
  check_udag ();

  raz_re ();
}

/* FIN du cas DAG_KIND */



/* Le point d'entree principal de udag_scanner.c s'appelle dag_scanner (et non udag_scanner)
   c,a permet au main d'appeler "dag_scanner ()", c'est le makefile de l'utilisateur qui a choisi
   entre dag_scanner.c ou udag_scanner.c */
SXINT 
dag_scanner (SXINT what, struct sxtables *arg)
{
  int severity = 0;

  switch (what) {
  case BEGIN :
#ifdef no_dico_lecl
    if (arg == NULL)
      sxtrap (ME, "dag_scanner (we detected a configuration with LECL but without lexicon ; however, we found no LECL table)");
#endif /* no_dico_lecl */
    tables = arg;

    sxerr_mngr (BEGIN);
    sxstr_mngr (BEGIN) /* initialisation ou remise a vide de la string table */ ;

    if (dag_kind == SDAG_KIND && tables)
      /* On est ds le cas d'un vrai parseur SYNTAX construit par l'utilisateur (la grammaire doit cependant avoir une forme predefinie) */
      (*(tables->analyzers.parser)) (BEGIN, tables) /* Allocation des variables globales du parser */ ;

    break;

  case OPEN:
    switch (dag_kind) {
    case UDAG_KIND :
      /* On continue en sequence */
    case DAG_KIND :
    default : /* Par defaut on dit qu'on a un DAG en entree !! */
#ifndef EOF_CODE
      sxtrap(ME,"udag_scanner (EOF_CODE is undef)");
#endif /* EOF_CODE */

#if EBUG
      if (EOF_CODE==0)
	sxtrap(ME,"udag_scanner (EOF_CODE is 0)");
#endif /* EBUG */

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

  case INIT:
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

  case ACTION:
    if (is_print_time)
      sxtime (INIT, NULL);
    
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
  
    idag_free ();

    sxfree (wstr), wstr = NULL;
    varstr_free (vstr);

    break;

  case FINAL:
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

  case CLOSE:
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

  case END :
    sxerr_mngr (END);
    sxstr_mngr (END);

    if (dag_kind == SDAG_KIND && tables)
      /* On est ds le cas d'un vrai parseur SYNTAX construit par l'utilisateur (la grammaire doit cependant avoir une forme predefinie) */
      (*(tables->analyzers.parser)) (END, tables);

    break;

  default:
    break;
  }

  return severity;
}

/* On a une transition sur t entre les mlstn p et q
   On retourne le tok_no tq toks_buf[tok_no] soit le token
   de cette transition */
/* Cette fonction est appelee depuis spf_fill_Tij2tok_no du sxspf_mngr */
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

  sxtrap (ME, "idag_p_q2pq");
}


/* Remplit la stack passee en parametre avec tous les tok_no qui correspondent a p t q */
void
idag_p_t_q2tok_no_stack (SXINT p, SXINT t, SXINT q)
{
  SXINT pq, tok_no;
  SXINT *tok_no_stack, *top_tok_no_stack, *local_t_stack, *top_local_t_stack;

  pq = idag_p_q2pq (p, q);

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

    top_local_t_stack = local_t_stack +*local_t_stack;

    while (++local_t_stack <= top_local_t_stack) {
      if (*local_t_stack == t) {
	/* La transition tok_no entre p et q contient bien le terminal t */
	DPUSH (spf.outputG.tok_no_stack, tok_no);
	break;
      }
    }
  }
}


#if 0
void
fill_Tij2tok_no (SXINT Tpq, SXINT maxTpq)
{
  SXINT          t, p, q, pq, tok_no, top_tok_no, first_tok_no, result, bot, top;
  SXINT          *tok_no_stack, *top_tok_no_stack;
  SXINT          *local_t_stack, *top_local_t_stack;
  BOOLEAN        is_first_time;
  char           *comment;

  t = -Tij2T (Tpq);

  /* Si t == 0 => mot inconnu */

#if EBUG
  if (t < 0)
    sxtrap (ME, "fill_Tij2tok_no");
#endif /* EBUG */

  p = Tij2i (Tpq);
  q = Tij2j (Tpq);

  pq = idag_p_q2pq (p, q);

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

    top_local_t_stack = local_t_stack +*local_t_stack;

    while (++local_t_stack <= top_local_t_stack) {
      if (*local_t_stack == t) {
	/* La transition tok_no entre p et q contient bien le terminal t */
	spf.outputG.tok_no2some_Tij [tok_no] = Tpq;

	if (first_tok_no = spf.outputG.Tij2tok_no [Tpq]) {
	  if (!XH_is_allocated (spf.outputG.Tij2XH_tok_no))
	    XH_alloc (&spf.outputG.Tij2XH_tok_no, "spf.outputG.Tij2XH_tok_no", -spf.outputG.maxt, 1, 2, NULL, NULL);

	  if (first_tok_no > 0) {
	    XH_push (spf.outputG.Tij2XH_tok_no, first_tok_no);
	    spf.outputG.Tij2tok_no [Tpq] = -1;
	  }

	  XH_push (spf.outputG.Tij2XH_tok_no, tok_no);
	} 
	else {
	  spf.outputG.Tij2tok_no [Tpq] = tok_no;
	}
      }
    }
  }

  tok_no = spf.outputG.Tij2tok_no [Tpq];

  if (tok_no == -1) {
    XH_set (&spf.outputG.Tij2XH_tok_no, &result);
    spf.outputG.Tij2tok_no [Tpq] = -result;	  
  }

  /* On s'occupe des commentaires */
  if (spf.outputG.Tij2comment == NULL)
    spf.outputG.Tij2comment = (char **) sxcalloc (maxTpq+1, sizeof (char *));

  if (tok_no > 0) {
    /* Unique */
    spf.outputG.Tij2comment [Tpq] = SXGET_TOKEN (tok_no).comment;
  }
  else {
    bot = XH_X (spf.outputG.Tij2XH_tok_no, -tok_no);
    top = XH_X (spf.outputG.Tij2XH_tok_no, -tok_no+1);
    varstr_raz (vstr);
    is_first_time = TRUE;
    
    while (bot < top) {
      tok_no = XH_list_elem (spf.outputG.Tij2XH_tok_no, bot);
      comment = SXGET_TOKEN (tok_no).comment;

      if (comment ) {
	if (is_first_time) {
	  is_first_time = FALSE;
	  vstr = varstr_catenate (vstr, comment);
	}
	else {
	  varstr_pop (vstr, 1); /* On enleve le "}" final */
	  vstr = varstr_catchar (vstr, ' ');
	  vstr = varstr_lcatenate (vstr, comment+1, strlen (comment)-1); /* On enleve le "{" initial du suffixe */
	}
      }

      bot++;
    }

    spf.outputG.Tij2comment [Tpq] = sxstrget (sxstr2save (varstr_tostr (vstr), varstr_length (vstr)));
  }
}
#endif /* 0 */

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
  BOOLEAN is_first_time;

  if (idag.path == NULL)
    fill_idag_path ();

  if (!SXBA_bit_is_set (idag.path [p], q))
    return vstr; /* pas de chemin, on ne fait rien */

  state_set = idag.path [0];
  is_first_time = TRUE;
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
	      is_first_time = FALSE;
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


#if 0
/* A VOIR si necessaire */
/* Appel depuis un parseur non-deterministe */
static void
dag2ndtkn (void)
{
  /* On simule un scanner non-deterministe a partir de dag_hd */
  /* Il faut allouer sxndsvar.ndlv */
  SXINT X, Y, Z, triple, XYZ;
  struct sxmilstn_elem	*milestone_new (/* prev_milstn_ptr, my_milestone, kind */);

  /* On simule sxndtkn_put () */
  /* Les transitions depuis le eof initial sont toutes calculees */
  sxndtkn.milstn_current->kind |= MS_COMPLETE;

  for (Y = 1; Y <= last_tok_no; Y++)
    /* Les tokens sont ranges ds leur ordre naturel... */
    sxput_token (toks_buf [Y]);

  sxput_token (SXGET_TOKEN (0)); /* eof ... */
  SXGET_TOKEN(sxplocals.Mtok_no).source_index = sxsrcmngr.source_coord; /* ... de fin */

  for (Z = 2; Z <= final_pos; Z++) {
    sxndtkn.milstn_head = milestone_new (sxndtkn.milstn_head,
					 ++sxndtkn.last_milestone_no,
					 MS_BEGIN+MS_COMPLETE);

    XxYxZ_Zforeach (dag_hd, Z, triple) {
      XxYxZ_set (&sxndtkn.dag,
		 XxYxZ_X (dag_hd, triple),
		 XxYxZ_Y (dag_hd, triple),
		 Z,
		 &XYZ);
    }
  }

  sxndtkn.milstn_head = milestone_new (sxndtkn.milstn_head,
					 ++sxndtkn.last_milestone_no,
					 MS_BEGIN+MS_COMPLETE);
  XxYxZ_set (&sxndtkn.dag,
	     final_pos,
	     sxplocals.Mtok_no,
	     sxndtkn.milstn_head->my_index /* should be final_pos+1 */,
	     &XYZ);
}

SXINT
dag_scan_it ()
{
  struct sxplocals store_sxplocals = sxplocals;
  int severity;

  severity = read_a_dag (store_dag);
  sxplocals = store_sxplocals;
      
  if (sxatcvar.atc_lv.abstract_tree_is_error_node)
    /* L'expression reguliere du source est un peu trop bugee, on abandonne!! */
    SXEXIT (SEVERITIES - 1);

  /* Pas d'ERROR ds le source */
  /* Pour faire croire a la recuperation d'erreur qu'on est ds la cas d'un (vrai) scanner
     non-deterministe... */
  sxplocals.sxtables->SXS_tables.S_is_non_deterministic = TRUE;
  sxplocals.sxtables->SXS_tables.S_are_comments_erased = TRUE;
  
  dag2ndtkn ();

  return severity;
}
#endif /* 0 */

