/*** THIS FILE IS NOT PART OF THE SYNTAX LIBRARY libsx.a ***/ 
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

/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* Mar 4 Mar 2003 13:10(PB)	Ajout de cette rubrique "modifications"	*/
/************************************************************************/


#ifdef is_dag
#undef is_dag
#endif /* is_dag */

/* On est ds dag_scanner !! De plus certains includes peuvent avoir des "#if is_dag" */
#define is_dag 1

static char	ME [] = "dag_scanner.c";

#include "sxunix.h"
#include "dag_scanner.h"
#include "sxnd.h"

char WHAT_DAG_SCANNER[] = "@(#)SYNTAX - $Id: dag_scanner.c 3678 2024-02-06 08:38:24Z garavel $" WHAT_DEBUG;

#if 0
/* !! ce n'est pas vrai ds le cas HUGE ou le code des mots du source est recherche directement ds un dico */
#ifndef  no_dico
/* Si on utilise un dico, on doit avoir
SXBA            *glbl_source, *glbl_out_source;
qui est obtenu par... */
#define is_sdag 1
#endif /* no_dico */
#endif /* 0 */

/* de #include "semact.h" */

#include "earley.h"

#ifndef UNFOLD
/* Le 27/09/06 */
#include "sxspf.h"

#endif /* UNFOLD */

#if 0
/* Le 19/09/06 */
SXBA            source_set, *glbl_source;
#endif /* 0 */

#define TOK_NB ((sizeof (tok_str)/sizeof (tok_str [0])))
static char *wstr;

static struct sxtables *tables;

static SXINT eof;

#ifndef no_dico
/* On peut faire une recherche ds un dico plutot que rechercher les mots-cles de la grammaire */

#ifdef keywords_h
/* Les tables keywords_h proviennent directement de l'execution de bnf avec l'option -huge
   La grammaire n'a presque rec,ue aucun traitement.  Les terminaux de la grammaire sont les formes flechies */
/* Cet include contient 
   #define HUGE_CFG
   #define USE_A_DICO 0
*/

#define def_inflected_form_names
#include keywords_h

#endif /* keywords_h */


#ifdef lex_lfg_h

#define def_dico_if
#define def_dico_t

#define def_if_id2tok_list_id
#define def_inflected_form_names

#define def_t_names
#define def_tok_disp
#define def_tok_list
#define def_tok_str

#include lex_lfg_h
#endif /* lex_lfg_h */


#ifdef mp_h
#  define def_mp_t_rev_table
#  define def_mp_t_table
#  include mp_h
#  define TOK_LIST(t)       mp_t_rev_table [tok_list [t]]
/* tok_list rend un vieux terminal */
#else /* mp_h */
#  define TOK_LIST(t)       tok_list [t]
#endif /* mp_h */


static SXINT ciu_name_id, lciu_name_id, digits_name_id;
static SXINT wstr_lgth;

static SXINT
string2dico_id (str, lgth)
     char *str;
     SXINT  lgth;
{
  SXINT id, cur_lgth;
  char c, first_char, first_char_lc, first_char_uc;
  char *cur_str, *cur_wstr, *where;
  bool has_changed, is_capitalized_initial;

#if USE_A_DICO
  cur_str = str;
  cur_lgth = lgth;

  /* On regarde si c'est une forme flechie */
  id = sxdico_get (&dico_if, &cur_str, &cur_lgth);
#else /* !USE_A_DICO */
  id = sxword_2retrieve (&inflected_form_names, str, lgth);
#endif /* !USE_A_DICO */
  
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
#if USE_A_DICO
      cur_str = wstr;
      cur_lgth = lgth;
      id = sxdico_get (&dico_if, &cur_str, &cur_lgth);
#else /* !USE_A_DICO */
      id = sxword_2retrieve (&inflected_form_names, wstr, lgth);
#endif /* !USE_A_DICO */
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
#if USE_A_DICO
      cur_str = wstr;
      cur_lgth = lgth;
      id = sxdico_get (&dico_if, &cur_str, &cur_lgth);
#else /* !USE_A_DICO */
      id = sxword_2retrieve (&inflected_form_names, wstr, lgth);
#endif /* !USE_A_DICO */
    }
  }

  if (id == 0 && digits_name_ptr) {
    /* On regarde si le mot ne contient que des chiffres */
    if (strtol (str, &where, 10) && where == str+lgth) {
      /* C'est bien un entier */
      id = digits_name_id;
    }
  }

  if (id == 0 && (ciu_name_ptr || lciu_name_ptr)) {
    first_char = *str;
    first_char_lc = sxtolower (first_char);
    first_char_uc = sxtoupper (first_char);
    is_capitalized_initial = first_char_lc != first_char;

    if (is_capitalized_initial && ciu_name_ptr) {
      /* Le mot inconnu commence par une majuscule et il y a un defaut pour les mots en majuscule */
      id = ciu_name_id;
    }
    else {
      if (!is_capitalized_initial && lciu_name_ptr) {
	/* Le mot inconnu commence par une minuscule et il y a un defaut pour les mots en minuscule */
	id = lciu_name_id;
      }
    }
  }
    
  return id;
}
#endif /* no_dico */

struct sxtoken *tok_no2tok (tok_no)
     SXINT tok_no;
{
  return &toks_buf [tok_no];
}

/* Cette procedure modifie les commentaires des tokens de la fac,on suivante :
   si t est un terminal de la transition (i, tok_no, j) alors le commentaire comment de tok_no est
   post_concatene aux tokens tok_no' differents de tok_no tels que (k, tok_no', j) et
   pre_concatene aux tokens tok_no' differents de tok_no tels que (i, tok_no', h).
   Cette procedure permet de resoudre le pb des commentaires associes aux _EPSILON facultatifs (i.e. [_EPSILON])
*/
/* Xforeach et Zforeach */
#include "varstr.h"

/* On concatene les 2 commentaires en virant les "} {" interieurs */
static char*
catenate_comment (vstr, comment1, comment2)
     VARSTR vstr;
     char   *comment1, *comment2;
{
  varstr_raz (vstr);

  vstr = varstr_lcatenate (vstr, comment1, strlen (comment1)-1);
  vstr = varstr_catchar (vstr, ' ');
  vstr = varstr_lcatenate (vstr, comment2+1, strlen (comment2)-1);

  return sxstrget (sxstr2save (varstr_tostr (vstr), varstr_length (vstr)));
}

/* Le make_new_dag () eventuel a deja ete appele' */
/* On "repartit" les commentaires associes au terminal t (Exemple epsilon) dans les tokens de ses "voisins" */
/* Typiquement appele' depuis la semantique associee a un parseur qui prend un dag en entree */

/* Ces nouveaux commentaires sont mis ds la table dag2comment qui est indexe'e par un triplet (i, tok_no, j) */
/* Il n'est pas possible de les mettre ds le champ comment d'un tok_no car le meme tok_no peut etre
   partage' par plusieurs triplets */
void distribute_comment (t)
     SXINT t;
{
  SXINT    pos, pos2, tok_no, tok_no2, triple, triple2;
  char   *comment, *comment2;
  VARSTR vstr;

  if (dag2comment == NULL)
    dag2comment = (char **) sxcalloc (XxYxZ_top (dag_hd)+1, sizeof (char *));
  vstr = varstr_alloc (128);

  for (pos = 1; pos < final_pos; pos++) {
    XxYxZ_Xforeach (dag_hd, pos, triple) {
      tok_no = XxYxZ_Y (dag_hd, triple);

      if (
#if is_sdag
	  SXBA_bit_is_set (glbl_source [tok_no], t)
#else /* !is_sdag */
	  glbl_source [tok_no] == t
#endif /* !is_sdag */
	  ) {
	comment = toks_buf [tok_no].comment;

	if (comment != NULL) {
	  XxYxZ_Xforeach (dag_hd, pos, triple2) {
	    if (triple2 != triple) {
	      tok_no2 = XxYxZ_Y (dag_hd, triple2);
	      comment2 = toks_buf [tok_no2].comment;
	      /* On fabrique comment2 = comment || comment2 */
	      dag2comment [triple2] = (comment2 == NULL) ? comment : catenate_comment (vstr, comment, comment2);
	    }
	  }
	  
	  pos2 = XxYxZ_Z (dag_hd, triple);

	  XxYxZ_Zforeach (dag_hd, pos2, triple2) {
	    if (triple2 != triple) {
	      tok_no2 = XxYxZ_Y (dag_hd, triple2);
	      comment2 = toks_buf [tok_no2].comment;
	      /* On fabrique comment2 = comment2 || comment */
	      dag2comment [triple2] = (comment2 == NULL) ? comment : catenate_comment (vstr, comment2, comment);
	    }
	  }
	}
      }
    }
  }

  varstr_free (vstr);
}


static SXINT toks_buf_size, toks_buf_top; /* init : toks_buf_size = last_tok_no+2; toks_buf_top = last_tok_no+1; */

SXINT
get_toks_buf_size ()
{
  return toks_buf_size;
}

/* Cette fonction, appelee depuis l'error repair de earley met ds toks_buf un token
   "genere" par l'error repair, comme s'il provenait du source */
SXINT
put_repair_tok (SXINT t, char *t_str, char *comment_str, struct sxsource_coord *psource_index)
{
  SXINT                   lgth, id;
  struct sxtoken        *ptok_buf;
  char                  str [8+128];

  if (++toks_buf_top >= toks_buf_size) {
    toks_buf_size *= 2;
    toks_buf = (struct sxtoken*) sxrealloc (toks_buf, toks_buf_size, sizeof (struct sxtoken));
  }
    
  ptok_buf = toks_buf + toks_buf_top;

  /* Pas d'acces a glbl_source depuis toks_buf_top si > last_tok_no+1 */
  ptok_buf->comment = comment_str;

#ifndef no_dico
  /* On fabrique _error__t */
  snprintf (str, 8+128, "_error__%s", t_str);
  lgth = strlen (str);
  ptok_buf->string_table_entry = sxstr2save (str, lgth);

  /* On recherche _error ds le dico */
  id = string2dico_id ("_error", 6);

#if 0
  /* t_str peut tres bien etre une categorie style "prep", "nc", ... qui ne se trouve pas ds le dico */
#if EBUG
  lgth = strlen (t_str);

  if (id == 0 || t !=
#if USE_A_DICO
      sxdico_get (&dico_t, &t_str, &lgth) 
#else /* !USE_A_DICO */
      sxword_2retrieve (&t_names, t_str, lgth) - 1
#endif /* !USE_A_DICO */
      )
    sxtrap (str, "unknown inflected form during put_repair_tok");
#endif /* EBUG */
#endif /* 0 */

#ifdef HUGE_CFG
  /* decalage de 1 entre les id retournes par un sxword et les codes terminaux */
  id--;
#endif /* HUGE_CFG */

#else /* ?no_dico */
  ptok_buf->string_table_entry = sxstr2save (t_str, strlen (t_str));
  id = t;
#endif /* no_dico */

  ptok_buf->lahead = id;
  ptok_buf->source_index = *psource_index;

  return toks_buf_top;
}

#if EBUG || UNFOLD
print_trans (dag_hd, triple)
     XxYxZ_header *dag_hd;
     SXINT triple;
{
  SXINT            lpos, rpos, tok_no, t;
  SXBA           glbl_source_i;
  char           *comment;
  struct sxtoken *ptok_buf;

  lpos = XxYxZ_X (*dag_hd, triple);
  tok_no = XxYxZ_Y (*dag_hd, triple);
  rpos = XxYxZ_Z (*dag_hd, triple);

  ptok_buf = toks_buf + tok_no;
  comment = ptok_buf->comment;

  /* Cas UNFOLD */
  printf ("%i\t%s\t%s\t%i", lpos, comment == NULL ? "" : comment, sxstrget (ptok_buf->string_table_entry), rpos);

#if EBUG
  printf (" [%i, %i]", ptok_buf->source_index.line, ptok_buf->source_index.column);


#if is_sdag  
  glbl_source_i = glbl_source [tok_no];
  t = sxba_scan (glbl_source_i, 0);

  printf (" {%s", t2string (t));

  while ((t = sxba_scan (glbl_source_i, t)) > 0) {
    printf (", %s", t2string (t));
  }
	
  putchar ('}');
#else /* !is_sdag */
  t = glbl_source [tok_no];
  printf (" {%s}", t2string (t));
#endif /* is_sdag */
#endif /* EBUG */

  putchar ('\n');
}
#endif /* EBUG || UNFOLD */

#if 0
/* Pour l'instant, non utilise' !! */
/* Il y a eu de la reparation et en particulier sont apparus de nouveaux terminaux */
/* On change le dag en entree pour faire comme si ses terminaux provenaient du source */
/* put_repair_tok () a ete appele' */
/* Appelle f (old_i, old_j, new_i, new_j) sur chaque element du nouveau dag */
void make_new_dag (f)
     void (*f)();
{
  SXINT          delta_new_dag, i, j, k, Tij, Tpq, tok_no, new_triple, left_j, right_j, new_j, nb; 
  SXINT          *i2new_lb;
  bool      is_loop;
  XxYxZ_header new_dag_hd;

  XxYxZ_alloc (new_dag_hd, "new_dag_hd", XxYxZ_top (dag_hd)+1, 1, dag_doreach, 0, statistics);
  i2new_lb = (SXINT*) sxalloc (final_pos+1, sizeof (SXINT));

  i2new_lb [0] = 0;

  delta_new_dag = 0;

  for (j = 0; j <= final_pos; j++) {
    /* On cherche tous ceux qui aboutissent en j */
    XxYxZ_Zforeach (dag_hd, j, triple) {
      i = XxYxZ_X (dag_hd, triple);
      tok_no = XxYxZ_Y (dag_hd, triple);

      new_i = i2new_lb [i];
      new_j = j+delta_new_dag;

      XxYxZ_set (&new_dag_hd, new_i, tok_no, new_j, &new_triple);

#if EBUG
      print_trans (&new_dag_hd, new_triple);
#endif /* EBUG */

      if (f) f (i, j, new_i, new_j);
    }

    /* On regarde s'il y a des ajouts qui partent de j */
    left_j = j+delta_new_dag;
    nb = 0;
    is_loop = false;

    Tij_iforeach (j, Tij) {
      k = Tij2j (Tij);

      if (k == j || k > final_pos) {
	/* Oui */
	if (k == j) {
	  /* loop */
	  is_loop = true;
	}
	else {
	  do {
	    nb++;

	    Tij_iforeach (k, Tpq) {
	      k = Tij2j (Tpq);
	      break; /* 1 chemin */
	    }
	  } while (k > final_pos);
	}
      }
    }

    if (is_loop)
      nb++;

    if (nb) {
      delta_new_dag += nb;
      right_j = j+delta_new_dag;
      new_j = left_j;

      Tij_iforeach (j, Tij) {
	k = Tij2j (Tij);

	if (k == j || k > final_pos) {
	  /* Oui */
	  if (k == j) {
	    /* loop */
	    XxYxZ_set (&new_dag_hd, left_j, spf.outputG.Tij2tok_no [Tij], right_j, &new_triple);
#if EBUG
	    print_trans (&new_dag_hd, new_triple);
#endif /* EBUG */
	    if (f) f (j, k, left_j, right_j);
	  }
	  else {
	    XxYxZ_set (&new_dag_hd, left_j, spf.outputG.Tij2tok_no [Tij], ++new_j, &new_triple);
#if EBUG
	    print_trans (&new_dag_hd, new_triple);
#endif /* EBUG */
	    if (f) f (j, k, left_j, new_j);

	    do {
	      Tij_iforeach (k, Tpq) {
		old_k = k;
		k = Tij2j (Tpq);

		if (k <= final_pos)
		  new_k = right_j;
		else
		  new_k = ++new_j;

		XxYxZ_set (&new_dag_hd, new_j, spf.outputG.Tij2tok_no [Tij], new_k, &new_triple);
#if EBUG
		print_trans (&new_dag_hd, new_triple);
#endif /* EBUG */
		if (f) f (old_k, k, new_j, new_k);

		break; /* 1 chemin */
	      }
	    } while (k > final_pos);
	  }
	}
      }
    }

    i2new_lb [j] = j+delta_new_dag;
  }

  final_pos += delta_new_dag;

  XxYxZ_free (&dag_hd);
  sxfree (i2new_lb);

  dag_hd = new_dag_hd;
}
#endif /* 0 */


/* Uniquement ds le cas udag */
static void
dag_oflw (old_size, new_size)
     SXINT      old_size, new_size;
{
  toks_buf_size = new_size+2;
  toks_buf = (struct sxtoken*) sxrealloc (toks_buf, toks_buf_size, sizeof (struct sxtoken));
  toks_buf_size = new_size+2;  
}


static void
init_dag_reader()
{
  SXINT        cur_lgth;
  char       *cur_str;

#if 0
  /* deja fait par l'appelant */
  eof = get_eof_val();
#endif /* 0 */

#if 0
  /* Deplace le 13/06/06 au debut de store_dag car la on connait last_tok_no */
  XxYxZ_alloc (&dag_hd, "dag_hd", 100/*predicted_last_tok_no supprimé*/+1, 1, dag_foreach, dag_oflw, NULL); 
  toks_buf_size=XxYxZ_size(dag_hd)+2/* les 2 eof */;
  toks_buf = (struct sxtoken*) sxalloc (toks_buf_size, sizeof (struct sxtoken));
  /* ... sauf si on a du "repair" ds earley.  Auquel cas les terminaux generes seront range's
     (apres reallocation) au-dela de l'indice last_tok_no+1 */
  /* Voir put_repair_tok */
  toks_buf_top = 0;
#endif /* 0 */

#ifndef no_dico
  if (default_set & (CHECK_UPPER+CHECK_LOWER)) {
    wstr = (char*) sxalloc ((wstr_lgth = 128)+1, sizeof (char));
  }
  
  if (digits_name_ptr) {
    /* On cherche le code par defaut des nombres ecrits en chiffres */
    cur_str = digits_name_ptr, cur_lgth = strlen (digits_name_ptr);
      digits_name_id = 
#if USE_A_DICO
	sxdico_get (&dico_if, &cur_str, &cur_lgth)
#else /* !USE_A_DICO */
	sxword_2retrieve (&inflected_form_names, cur_str, cur_lgth)
#endif /* !USE_A_DICO */
	;
  }
  
  if (ciu_name_ptr) {
    /* On cherche le code par defaut des noms propres inconnus */
    cur_str = ciu_name_ptr, cur_lgth = strlen (ciu_name_ptr);
    ciu_name_id = 
#if USE_A_DICO
      sxdico_get (&dico_if, &cur_str, &cur_lgth)
#else /* !USE_A_DICO */
      sxword_2retrieve (&inflected_form_names, cur_str, cur_lgth)
#endif /* !USE_A_DICO */
      ;
    
    if (ciu_name_id == 0) {
      fprintf (sxstderr, "%s: Invalid option for unknown words, \"%s\" must be a valid capitalized initial inflected form.\n", ME, ciu_name_ptr);
      sxexit (6);
      }
  }
  
  if (lciu_name_ptr) {
    /* On cherche le code par defaut des mots minuscules inconnus */
    cur_str = lciu_name_ptr, cur_lgth = strlen (lciu_name_ptr);
    lciu_name_id = 
#if USE_A_DICO
      sxdico_get (&dico_if, &cur_str, &cur_lgth)
#else /* !USE_A_DICO */
      sxword_2retrieve (&inflected_form_names, cur_str, cur_lgth)
#endif /* !USE_A_DICO */
      ;
    
    if (lciu_name_id == 0) {
      fprintf (sxstderr, "%s: Invalid option for unknown words, \"%s\" must be a valid lower case initial inflected form.\n", ME, lciu_name_ptr);
      sxexit (6);
    }
  }
#endif /* no_dico */
  
}

static void
fill_dag_infos ()
{
  SXINT cur_trans, last_trans, lpos, rpos, tok_no, lgth, cur_lgth, id, prev_id, top, cur, tok, cur_tok, ste;
  char *str, *cur_str, *ret_str, *comment;
  struct sxtoken *ptok;
  SXBA glbl_source_i;
  struct sxsrcmngr save_sxsrcmngr;

#if UNFOLD
  puts("## DAG BEGIN");
#endif /* UNFOLD */

  if (tables) {
    /* On peut utiliser "tables" pour re_scanner les chaines traitees par dag_scanner et recuperer ainsi leur
       vrais codes internes donnes par les dummy_tables */
    /* On "ferme" le source mngr qui a lu le DAG */
    save_sxsrcmngr = sxsrcmngr;
    sxsrc_mngr (SXFINAL); /* Celui du DAG */
    sxtkn_mngr (SXOPEN, 1); /* Gestion des nouveaux tokens */
    (*(tables->analyzers.scanner)) (SXOPEN, tables); /* On prepare le scanneur de "tables" */
  }

  last_trans = XxYxZ_top (dag_hd);

  for (cur_trans = 1; cur_trans <= last_trans; cur_trans++) {
    lpos = XxYxZ_X (dag_hd, cur_trans);
    tok_no = XxYxZ_Y (dag_hd, cur_trans);
    rpos = XxYxZ_Z (dag_hd, cur_trans);

    if (tok_no == 0) /* le source est la chaîne vide -- toléré uniquement si dag_kind == DAG */
      return;

    ptok = toks_buf + tok_no;

    ste = ptok->string_table_entry;
    str = sxstrget (ste);
#if EBUG
    comment = ptok->comment;
#endif /* EBUG */

    if (ste != SXERROR_STE) {
      lgth = sxstrlen (ste);
      
#ifndef UNFOLD
#if is_sdag      
      glbl_source_i = glbl_source [tok_no];
#endif /* is_sdag */
#endif /* !UNFOLD */
    
#ifndef no_dico
      /* On suppose que si on a un dico, chaque transition du DAG est un sdag!! */
      /* Faux, au moins ds le cas HUGE */
      id = string2dico_id (str, lgth);
      tok = 0;

      if (id == 0 || (ciu_name_ptr && id == ciu_name_id) || (lciu_name_ptr && id == lciu_name_id)) {
	prev_id = id;

	/* Le 22/12/05 */
	/* On regarde si la ff est de la forme pref__suff */

	if (cur_str = strstr (str, "__")) {
	  /* On prend le + a droite pour permettre des "le__det__det" */
	  /* Attention, on peut avoir anti-___nc */
	  while (ret_str = strstr (cur_str+1, "__")) {
	    cur_str = ret_str;
	  }
	
	  cur_lgth = lgth;
	  lgth = cur_str-str;
	  id = string2dico_id (str, lgth);
	
	  if (id) {
	    cur_str += 2;
	    cur_lgth -= lgth+2;
	  
	    if (cur_str[cur_lgth - 1] != '*') {
	      tok =
#if USE_A_DICO
		sxdico_get (&dico_t, &cur_str, &cur_lgth)
#else /* !USE_A_DICO */
#ifdef keywords_h
		/* Les tables proviennent de l'include keywords_h ou terminaux et formes_flechies sont identiques !! */
		sxword_2retrieve (&inflected_form_names, cur_str, cur_lgth) - 1
#else /* ?keywords_h */
		/* ATTENTION : ce "-1" permet de compenser le décalage dû au fait que t_names_string.chars commence par SXNUL
		   (cf. sa generation dans sxword_to_c) */
		sxword_2retrieve (&t_names, cur_str, cur_lgth) - 1
#endif /* !keywords_h */
#endif /* !USE_A_DICO */
		;
	      
#ifdef mp_h
	      tok = mp_t_rev_table [tok];
#endif /* mp_h */
	      
	      if (tok == 0)
		id = 0;
	    } else
	      tok = -1; // cas mot_terminalPrefix* : on met -1 dans tok, et on reporte à plus tard le test
	  } else {
	    id = prev_id;
	  }
	}
      }

      if (id) {
	ptok->lahead = id; /* on stoke dans un lahead inutile un truc tout différent: le id de la ff */
	
#ifdef is_LFG
	/* Ds le cas ou le dico a ete fabrique a partir d'une specif LFG, il y a un intermediaire ... */
	id = if_id2tok_list_id [id];
#endif /* is_LFG */
      

#ifdef HUGE_CFG
	/* decalage de 1 entre les id retournes par un sxword et les codes terminaux */
	id--;

#ifndef UNFOLD  
#if is_sdag 
	SXBA_1_bit (glbl_source_i, id);
#else /* !is_sdag */
	glbl_source [tok_no] = id;
#endif /* !is_sdag */
#endif /* !UNFOLD */
	
	  if (source_set)
	    SXBA_1_bit (source_set, id);

	  t_occur_nb++;
#else /* !HUGE_CFG */
	for (top = tok_disp [id+1], cur = tok_disp [id]; cur < top; cur++) {
	  cur_tok = TOK_LIST (cur);
	  if (tok == 0 // cas normal
	      || cur_tok == tok // cas forme__terminal
	      || (tok == -1 && strncmp (cur_str,
#ifdef mp_h
					tok_str [mp_t_table [cur_tok]],
#else /* !mp_h */
					tok_str [cur_tok],
#endif /* ?mp_h */
					cur_lgth - 1) == 0 ) // cas forme__terminalPrefix*
	      ) {
#ifndef UNFOLD   
#if is_sdag 
	    SXBA_1_bit (glbl_source_i, cur_tok);
#else /* !is_sdag */
	    glbl_source [tok_no] = cur_tok;
#endif /* !is_sdag */
#endif /* !UNFOLD */
	
	    if (source_set)
	      SXBA_1_bit (source_set, cur_tok);

	    t_occur_nb++;
	  }
	}
#endif /* !HUGE_CFG */
      
	id = -id; /* !! */
      }
      else {
	if (tables == NULL) {
	  sxput_error (ptok->source_index,
		       "%sUnknown word \"%s\".",
		       sxsvar.sxtables->err_titles [1],
		       str);
	  /* Le 03/05/04 Erreur est change' en Warning, les mots inconnus ne bloquent plus l'analyse */
	  /* Le 21/03/07 On sort le mot inconnu (le marqueur ds le source ne suffit pas ds le cas ou le source tient sur une ligne) */
	  
	  /* Essai le 22/12/05 */
	  /* On met le terminal de code 0 sur les mots inconnus */
#ifndef UNFOLD
#if is_sdag 
	  SXBA_1_bit (glbl_source_i, 0);
#else /* !is_sdag */
	  glbl_source [tok_no] = 0;
#endif /* !is_sdag */
#endif /* !UNFOLD */
	  
	  if (source_set)
	    SXBA_1_bit (source_set, 0);	  
	}
	ptok->lahead = -1; /* non initialise' !! */
      }
#endif /* !no_dico */

      /* Désormais (14/12/07), si on a des tables (i.e. si on a fait un _t.c par un lecl), on
	 demande son avis au lecl, même si on a un lexique, et même si ledit lexique a répondu
	 positivement */
      if (tables) {
	if (tables->SXS_tables.check_keyword)
	  id = (*tables->SXS_tables.check_keyword) (str, lgth);
	else
	  /* Les tables qui permettent de recuperer les vrais codes des terminaux n'ont pas de mots-clefs */
	  id = 0;

	if (id == 0) {
	  /* On scanne grace aux tables */
	  sxsrc_mngr (SXINIT, NULL, str); /* On va scanner str */
	  sxnext_char (); /* initialisation de la lecture */
	  sxtkn_mngr (SXINIT, 0); /* Resultat a l'indice 1 */
	  (*(tables->SXP_tables.scanit)) (); /* On le scanne */
	  id = SXGET_TOKEN (sxplocals.Mtok_no).lahead; /* ... et voila le resultat */

	  if (id) {
	    /* Il se peut qu'on vienne de ne reconnaitre qu'un prefixe de str */
	    /* En fait il faudrait fabriquer des ids (et les transitions ds le dag)
	       tant que str n'est pas epuise' !! */
	    /* Ici on fait simple ... */
	    (*(tables->SXP_tables.scanit)) (); /* On scanne le suivant ... */
	    
	    /* ... qui doit etre eof */
	    if (SXGET_TOKEN (sxplocals.Mtok_no).lahead != eof)
	      id = 0;
	  }
	}
	    
#ifndef UNFOLD
#if is_sdag  
	SXBA_1_bit (glbl_source_i, id);
#else /* !is_sdag */
	glbl_source [tok_no] = id;
#endif /* !is_sdag */
#endif /* !UNFOLD */
      
	if (source_set/* && id > 0 Essai */)
	  SXBA_1_bit (source_set, id);
      
	if (id == 0) {
	  sxput_error (ptok->source_index,
		       "%sUnknown word.",
		       sxsvar.sxtables->err_titles [1]);
	  /* Le 03/05/04 Erreur est change' en Warning, les mots inconnus ne bloquent plus l'analyse */
	}
      }
#ifdef no_dico
      ptok->lahead = id;
#endif /* no_dico */
    }
    else {
      ptok->lahead = id = 0;
    }

#if EBUG || UNFOLD
    print_trans (&dag_hd, cur_trans);
#endif /* EBUG || UNFOLD */
  }

  if (tables) {
    (*(tables->analyzers.scanner)) (SXCLOSE, tables); /* Fin du scanneur de "tables" */
    sxtkn_mngr (SXCLOSE, 0); /* Soyons propres */
    sxsrc_mngr (SXFINAL); /* Celui de "tables" */
    sxsrcmngr = save_sxsrcmngr; /* C,a permet a sxput_error de ressortir le source!! */
  }

  top_dag_trans = XxYxZ_top (dag_hd);
  new_init_pos = init_pos;

#if UNFOLD
  puts("## DAG END");
#endif /* UNFOLD */

}

/* Cette procedure est appelee depuis read_a_dag sur chaque token */
/* lpos_set est l'ensemble des positions gauches
   rpos_set est l'ensemble des positions droites
   tok_no est le numero du token
   et ptok est le pointeur vers le token courant (dans le noeud de l'arbre abstrait)
*/
static void
store_dag (lpos_set, tok_no, ptok, rpos_set)
     SXBA           lpos_set, rpos_set;
     SXINT            tok_no;
     struct sxtoken *ptok;
{
  SXINT            triple, lpos, rpos;

  if (toks_buf == NULL) {
    toks_buf_size = last_tok_no+2/* les 2 eof */;
    toks_buf = (struct sxtoken*) sxalloc (toks_buf_size, sizeof (struct sxtoken));
    /* la taille de dag_hd doit au moins etre elle de toks_buf, mais elle peut etre + grande Ex :
       1 a 2 [b] 3 => last_tok_no = 2 mais 3 transitions : (1, a, 2), (2, b, 3) et (1, a, 3) */
    XxYxZ_alloc (&dag_hd, "dag_hd", toks_buf_size+1, 1, dag_foreach, NULL /* pas de dag_oflw */, NULL); 

    /* ... sauf si on a du "repair" ds earley.  Auquel cas les terminaux generes seront range's
       (apres reallocation) au-dela de l'indice last_tok_no+1 */
    /* Voir put_repair_tok */
    toks_buf_top = last_tok_no+1;

    toks_buf [0 /* indice du eof de debut */] = SXGET_TOKEN (0);
    toks_buf [toks_buf_top /* indice du eof de fin */] = SXGET_TOKEN (sxplocals.Mtok_no);
    toks_buf [0].lahead = toks_buf [toks_buf_top].lahead = eof;
  }


  if (ptok == NULL) {
    /* La chaine vide appartient au langage DAG */
    /* On met ce triplet */
    XxYxZ_set (&dag_hd, init_pos, 0, final_pos, &triple);
#if EBUG
    printf ("%i\t{}\t\t%i\n", init_pos, final_pos);
#endif
  }
  else {
    toks_buf [tok_no] = *ptok;

    lpos = -1;

    while ((lpos = sxba_scan (lpos_set, lpos)) >= 0) {
      rpos = -1;

      while ((rpos = sxba_scan (rpos_set, rpos)) >= 0) {
	XxYxZ_set (&dag_hd, lpos, tok_no, rpos, &triple);
      }
    }
  }
}



static void
store_udag (lpos, ptok, rpos)
     SXINT               lpos, rpos;
     struct sxtoken    *ptok;
{
  SXINT            triple;

  if (toks_buf == NULL) {
    /* Attention, ici, contrairement a store_dag, on ne connait pas last_tok_no qui est calcule'
       au fur et a mesure */
    /* Ici identite' entre le nombre de transitions et le nombre de tokens */
    XxYxZ_alloc (&dag_hd, "dag_hd", 100+1, 1, dag_foreach, dag_oflw, NULL); 
    toks_buf_size = XxYxZ_size (dag_hd)+2/* les 2 eof */;
    toks_buf = (struct sxtoken*) sxalloc (toks_buf_size, sizeof (struct sxtoken));

    /* ... sauf si on a du "repair" ds earley.  Auquel cas les terminaux generes seront range's
       (apres reallocation) au-dela de l'indice last_tok_no+1 */
    /* Voir put_repair_tok */
    toks_buf_top = 0;
  }

  if (init_pos == -1) { // si on est sur le premier token
    toks_buf [0] = SXGET_TOKEN (0);
    toks_buf [0].lahead = eof;
  }

  if (init_pos == -1 || lpos < init_pos) // l'état initial d'un udag est celui de valeur minimale
    // A FAIRE: pour le moment, on a interet à avoir init_pos=1... sinon mystère
    init_pos = lpos;

  if (final_pos == -1 || rpos > final_pos) // l'état final d'un udag est celui de valeur maximale
    final_pos = rpos;

  XxYxZ_set (&dag_hd, lpos, ++toks_buf_top, rpos, &triple);
  last_tok_no = toks_buf_top;
  toks_buf [toks_buf_top] = *ptok;

  if (SXGET_TOKEN (sxplocals.atok_no).lahead == get_eodag_code()) { // si on est sur le dernier token
    toks_buf [last_tok_no+1] = SXGET_TOKEN (sxplocals.Mtok_no);
    toks_buf [last_tok_no+1].lahead = eof;
  }
}

static void
dag2ndtkn ()
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

#if 0 /* integré à raz_udag */
/* Rapatrie' depuis sxparser_earley le 20/09/06 */
static void
before_first_udag_trans ()
{
  init_pos = -1;
  final_pos = -1;
}
#endif /* 0 */


/* Cette fonction est appelee depuis dag_scanner.c après les appels à store_[u]dag().
   Elle permet de faire certains traitements, les valeurs init_pos, final_pos et last_tok_no etant
   (désormais) connues */
static void
after_last_trans ()
{
  SXINT pos;

  /* Pour permettre des modifs dynamiques du dag, et pour conserver la propriete si (lpos, tok_no, rpos)
     on a lpos < rpos (par exemple traitement des erreurs), on maintient 2 tableaux dag_state2mlstn et mlstn2dag_state
     tels que la propriete precedente s'applique sur dag_state2mlstn[lpos] < dag_state2mlstn[rpos].
     Ce sont donc les milestones qui sont strictement croissants.
  */

#if 0
  rcvr_tok_no = 0;
  max_rcvr_tok_no = delta_n;
  /* On peut "creer" de nouveaux tokens pour rcvr_tok_no = [1..max_rcvr_tok_no] */
#endif /* 0 */

  final_mlstn = final_pos ;
  init_mlstn = init_pos;
#if 0
  /* Le 04/10/06 suppression de ces correspondances */
  dag_state2mlstn = (SXINT*) sxalloc (final_pos+1, sizeof (SXINT));
  mlstn2dag_state = (SXINT*) sxalloc (final_pos+1, sizeof (SXINT));

  for (pos = 0; pos < init_pos; pos++) {
    /* prudence */
    dag_state2mlstn [pos] = mlstn2dag_state [pos] = 0;
  }

  while (pos <= final_pos) {
    /* initialement, pos == milestone */
    dag_state2mlstn [pos] = mlstn2dag_state [pos] = pos;
    pos++;
  }
#endif /* 0 */
	
  n = final_mlstn ; /* !! */


  if (source_set) // on n'est pas sur le premier DAG
    sxba_empty(source_set);
  else
    source_set = sxba_calloc (eof+1);

  SXBA_1_bit (source_set, eof);

  source_top = source_set + NBLONGS (BASIZE (source_set)); /* pointe vers la derniere "tranche" */

  /* On alloue glbl_source, meme si on n'est pas ds le cas is_sdag, ca permet de faire du traitement
     d'erreur + facilement.  PB le cas !is_sdag doit-il etre traite comme is_sdag ? */
#if is_sdag
  if (glbl_source_area)
    sxbm_free (glbl_source_area);
  
  glbl_source_area = glbl_source = sxbm_calloc (2*(last_tok_no+2+max_rcvr_tok_no), eof+1);
  SXBA_1_bit (glbl_source [0], eof);
  SXBA_1_bit (glbl_source [last_tok_no+1], eof);
#else /* !is_sdag */
  if (glbl_source_area) {
    glbl_source_area = sxrealloc (glbl_source_area, 2*(last_tok_no+2+max_rcvr_tok_no), sizeof (SXINT));
  } else {
    glbl_source_area = glbl_source = (SXINT*) sxalloc (2*(last_tok_no+2+max_rcvr_tok_no), sizeof (SXINT));
    glbl_source [0] = eof;
  }
  
  glbl_source [last_tok_no+1] = eof; // last_tok_no change d'un DAG au suivant
#endif /* !is_sdag */
    
  glbl_out_source = glbl_source+last_tok_no+2+max_rcvr_tok_no;

}

#if 0
/* Appel depuis un parseur non-deterministe */
SXINT dag_scan_it ()
{
  struct sxplocals store_sxplocals = sxplocals;
  int severity;

  severity = read_a_dag (store_dag);
  sxplocals = store_sxplocals;
      
  if (sxatcvar.atc_lv.abstract_tree_is_error_node)
    /* L'expression reguliere du source est un peu trop bugee, on abandonne!! */
    sxexit (SXSEVERITIES - 1);

  /* Pas d'ERROR ds le source */
  /* Pour faire croire a la recuperation d'erreur qu'on est ds la cas d'un (vrai) scanner
     non-deterministe... */
  sxplocals.sxtables->SXS_tables.S_is_non_deterministic = true;
  sxplocals.sxtables->SXS_tables.S_are_comments_erased = true;
  
  dag2ndtkn ();

  return severity;
}
#endif /* 0 */

#ifndef UNFOLD
/* static */ SXINT
check_dag ()
{
  bool oversized;
  char    mess [32];
  int     severity = 0;

  /* Pas utilisation en stand alone */
  /* On ajoute eof comme terminal rencontre' */  
  oversized = ((SXUINT) t_occur_nb > maximum_input_size);
  
  if (is_print_time) {
    sprintf (mess, "\t%s Scanner[%i, %i, %i%s]", dag_kind == DAG ? "Dag" : "UDag",final_pos - 1 /* pb de poteaux et d'intervalles*/, last_tok_no, t_occur_nb, oversized ? "(oversized)" : "");
    sxtime (SXACTION, mess);
  }
  
  if (oversized)
    severity = 7;

  return severity;
}
#endif /* UNFOLD */

static void
raz_dag () {
  if (toks_buf)
    sxfree (toks_buf), toks_buf = NULL;

  if (XxYxZ_is_allocated (dag_hd))
    XxYxZ_free (&dag_hd);
  
  if (dag2comment)
    sxfree (dag2comment), dag2comment = NULL;

#if 0
  if (dag_state2mlstn) sxfree (dag_state2mlstn), dag_state2mlstn = NULL;
  if (mlstn2dag_state) sxfree (mlstn2dag_state), mlstn2dag_state = NULL;
#endif /* 0 */
 
  /* les allocations sont faites sur le premier appel à store_dag pour le DAG courant */
}

static void
raz_udag () {
  raz_dag();
  init_pos = -1;
  final_pos = -1;
}


SXINT dag_scanner (what, arg)
    SXINT			what;
    struct sxtables	*arg;
{
  int severity = 0;

#if 0
  eof = get_eof_val();
#endif /* 0 */

  switch (what) {
  case SXOPEN:
#ifndef UNFOLD
    eof = -spf.inputG.maxt;
#endif /* UNFOLD */
    tables = arg;

#ifndef no_dico
#if !USE_A_DICO
#ifndef HUGE_CFG
      sxword_reuse (&t_names, "t_names", sxcont_malloc, sxcont_alloc, sxcont_realloc, NULL, sxcont_free, NULL, NULL);
#endif /* HUGE_CFG */
      sxword_reuse (&inflected_form_names, "inflected_form_names", sxcont_malloc, sxcont_alloc, sxcont_realloc, NULL, sxcont_free, NULL, NULL);
#endif /* !USE_A_DICO */
#endif /* no_dico */

    break;
  case SXINIT:
    if (tables != NULL) {
      if (0 /* A FAIRE : identifier le cas non-deterministe */) {
	/* Pour sxndtkn_mngr (SXINIT) */
	sxndsvar.ndlv = (struct sxndlv*) sxalloc (sxndsvar.ndlv_size = 2, sizeof (struct sxndlv));
	sxndsvar.current_ndlv = sxndsvar.ndlv + 1;
	sxndsvar.current_ndlv->my_index = 1;
      }
    }
    break;
  case SXACTION:
    /* Appel depuis un "programme normal" (pas un parseur SYNTAX!!) */
    init_dag_reader ();

    if (dag_kind == DAG) {
      severity = read_a_dag (store_dag, 
#ifdef UNFOLD
			     NULL,
#else /* UNFOLD */
			     after_last_trans, 
#endif /* UNFOLD */
			     fill_dag_infos,
#ifdef UNFOLD
			     NULL,
#else /* UNFOLD */
			     check_dag,
#endif /* UNFOLD */
			     raz_dag);
    } else if (dag_kind == UDAG) {
      severity = read_a_udag (store_udag, 
#ifdef UNFOLD
			     NULL,
#else /* UNFOLD */
			     after_last_trans, 
#endif /* UNFOLD */
			     fill_dag_infos,
#ifdef UNFOLD
			     NULL,
#else /* UNFOLD */
			     check_dag,
#endif /* UNFOLD */
			     raz_udag);
    } else {
      sxtrap(ME,"dag_scanner (unknown DAG kind)");
    }
    break;
  case SXCLOSE:
    if (tables != NULL) {
      if (0 /* A FAIRE : identifier le cas non-deterministe */)
	sxfree (sxndsvar.ndlv);
    }

    if (wstr)
      sxfree (wstr), wstr = NULL;

    break;
  default:
    break;
  }

  return severity;
}

#ifndef UNFOLD
/* On a une transition sur t entre les mlstn p et q
   On retourne le tok_no tq toks_buf[tok_no] soit le token
   de cette transition */
/* Cette fonction est appelee depuis spf_fill_Tij2tok_no du sxspf_mngr */
/* Il y en a une autre version ds le cas !is_dag ds sxearley_main.c */
void
fill_Tij2tok_no (SXINT Tpq, SXINT maxTpq)
{
  SXINT            t, p, q, trans, result, tok_no = 0, first_tok_no;
  char           *comment;
  struct sxtoken *ptok_buf;

  t = -Tij2T (Tpq);

  /* Si t == 0 => mot inconnu */

#if EBUG
  if (t < 0)
    sxtrap (ME, "fill_Tij2tok_no");
#endif /* EBUG */

  p = Tij2i (Tpq);
  q = Tij2j (Tpq);

  /* p et q sont des mlstn */
  p = mlstn2dag_state (p);
  q = mlstn2dag_state (q);
  /* En "standard" on dispose de X et Zforeach sur dag_hd (voir sxearley_main.c) */

  XxYxZ_Xforeach (dag_hd, p, trans) {
    if (XxYxZ_Z (dag_hd, trans) == q) {
      tok_no = XxYxZ_Y (dag_hd, trans);

      if (
#if is_sdag
	  SXBA_bit_is_set (glbl_source [tok_no], t)
#else /* !is_sdag */
	  glbl_source [tok_no] == t
#endif /* !is_sdag */
	  ) {
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
	} else {
	  spf.outputG.Tij2tok_no [Tpq] = tok_no;
	}

	if (dag2comment && (comment = dag2comment [trans])) {
	  if (spf.outputG.Tij2comment == NULL)
	    spf.outputG.Tij2comment = (char **) sxcalloc (maxTpq+1, sizeof (char *));

	  spf.outputG.Tij2comment [Tpq] = comment;
#if LOG
	  printf ("composite_comment=%s ", comment);
#endif /* LOG */
	}

#if LOG
	ptok_buf = toks_buf + tok_no;
	printf ("tok_no=%i, (local_comment=%s, ste=%s, source_index=[%i, %i])\n", tok_no,
		ptok_buf->comment == NULL ? "" : ptok_buf->comment, sxstrget (ptok_buf->string_table_entry), 
		ptok_buf->source_index.line, ptok_buf->source_index.column);
#endif /* LOG */

	/*	break; -- commenté car on peut avoir plusieurs tok_no pour un même Tpq */
      }
    }
  }

  if (spf.outputG.Tij2tok_no [Tpq] == -1) {
    XH_set (&spf.outputG.Tij2XH_tok_no, &result);
    spf.outputG.Tij2tok_no [Tpq] = -result;	  
  }
}

/* Le 18/12/06 dag_or_nodag_source_processing () a ete deplace ds lexicalizer_mngr.c */

#if 0
void
dag_source_processing (SXBA *t2suffix_t_set, SXBA *mlstn2suffix_source_set)
{
  SXINT  mlstn, next_mlstn, dag_state, trans, Y, t;
  SXBA suffix_source_set, next_suffix_source_set;

  SXBA_1_bit (mlstn2suffix_source_set [final_mlstn], eof);
  sxba_copy (mlstn2suffix_source_set [0], source_set);
  sxba_copy (mlstn2suffix_source_set [init_mlstn], source_set);

  for (mlstn = final_mlstn-1; mlstn >= init_mlstn; mlstn--) {
    suffix_source_set = mlstn2suffix_source_set [mlstn];
    dag_state = mlstn2dag_state [mlstn];

    XxYxZ_Xforeach (dag_hd, dag_state, trans) {
      Y = XxYxZ_Y (dag_hd, trans);
      next_mlstn = dag_state2mlstn [XxYxZ_Z (dag_hd, trans)];
      next_suffix_source_set = mlstn2suffix_source_set [next_mlstn];

#if is_sdag
      trans_source_set = glbl_source [Y];

      if (t2suffix_t_set) {
	t = -1;

	while ((t = sxba_scan (trans_source_set, t)) >= 0) {
	  sxba_or (t2suffix_t_set [t], next_suffix_source_set);
	}
      }

      sxba_or (suffix_source_set, next_suffix_source_set);
      sxba_or (suffix_source_set, trans_source_set);
#else /* !is_sdag */
      t = toks_buf [Y].lahead;

      if (t2suffix_t_set)
	sxba_or (t2suffix_t_set [t], next_suffix_source_set);

      sxba_or (suffix_source_set, next_suffix_source_set);
      SXBA_1_bit (suffix_source_set, t);
#endif /* !is_sdag */
    }
  }
}
#endif /* 0 */

#endif /* UNFOLD */

