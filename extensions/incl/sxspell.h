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
#ifndef sxspell_h
#define sxspell_h

#include "sxba.h"
#include "fsa.h"
#include "sxword.h"
#include "XxY.h"
#include "XH.h"

#define UNBOUNDED          ((~0U)>>1)
	

/* Automate de travail */
struct weight {
  SXUINT pos, neg;
};  

struct trans {
  SXBA          char_set;
  struct weight *char_weight;
};

/* structure qui contiendra le resultat de la correction */
struct spell_result {
  SXINT              spelling_nb /* SXSPELL_spelling_nb(s) == nb de corrections verifiant la specif */,
                     weight_limit /* Majorant des poids des corrections retenues */,
                     max_weight /* aucune correction ne peut depasser ce poids */,
                     kind /* ALL, BEST ou NBEST */,
                     cmp_weight /* Poids pour chaque nelle erreur multiple */,
                     seek_sub_words /* 1 => on cherche a decouper en sous-mots */;
  struct weight      sub_word_weight;
  /* Pour chaque correction x variant de 1 a spelling_nb ... */
  SXINT              *ids /* ... son identifiant s'obtient par SXSPELL_x2id(s,x) ... */;
  struct weight      *weights /* ... et son poids par SXSPELL_x2weight(s,x) */;
  /* Si SXSPELL_has_subwords(id) cet identifiant repere une liste de sous-mots que l'on peut parcourir par
     SXSPELL_subword_foreach(s,id,y,w).  y et w sont des variables utilisateur, y est un SXINT et w est un SXINT*, positionnees
     par le foreach.  Elles sont utilisees par SXSPELL_x2subword_id(w,y) qui permet d'obtenir l'identifiant de chaque sous-mot */
  XH_header          multiple_word_ids /* contiend la liste des codes des sous-mots */;
  /* Soit id l'identifiant d'une correction (mot ou sous-mot) ... */
  sxword_header      words /* ... sa chaine s'obtient par SXSPELL_id2str(s,id), sa longueur par SXSPELL_id2str_len(s,id) */;
  SXINT              *codes /* ... et son code (provenant du dico) par SXSPELL_id2code(s,id) */;
};

struct FSA {
  struct spell_result spell_result;
  struct sxdfa_comb   *dico;
  SXINT               q0, Q_init, Q_nb, area_id, area_page_nb; /* Q_nb >= 0 */
  char       *word /* Le mot a corriger */;
  SXBA                orig_char_set /* ensemble des caracteres initiaux du mot */,
                      final_set; 
  XxY_header          Q_hd;
  struct trans        **area;
};


/* 1er appel au package "sxspell" */
extern void sxspell_init (  struct sxdfa_comb   *dico,
			    struct FSA          *FSA,
			    unsigned char       kind /* 0 => ALL, 1 => BEST ou [2..255] => NBEST */,
			    SXINT               max_weight /* ne pas depasser cette valeur */,
			    SXINT               compose_weight /* cout de la composition de fautes */,
			    bool             seek_sub_words /* une correction peut etre formee d'une liste de sous-mots */,
			    SXINT               sub_word_weight /* cout d'une telle correction */
			  );

/* dernier appel au package "sxspell" */
extern void sxspell_final (
			   struct FSA          *FSA
			  );

/* On represente un mot sous forme de FSA */
extern void sxspell_init_fsa (
			      char          *mot,
			      SXINT         lgth,
			      struct FSA    *FSA
			      );

/* FSA Freeing */
extern void sxspell_final_fsa (
			       struct FSA   *FSA
			      );

/* R est un vecteur de pointeurs vers des chaines de caracteres.  Sa taille est de R_size == 2k
   pour tout 0 <= i < k on a
   R[2i] = "A" caractere a remplacer
   R[2i+1] = "ÀÁÂaàáâ" les remplac,ant (cout replace_weight)
*/
extern void sxspell_replace (
			     struct FSA *FSA, 
			     unsigned char *R[], 
			     SXINT R_size, 
			     SXINT replace_weight
			    );

/* R est un vecteur de pointeurs vers des chaînes de caracteres.  Sa taille est de R_size == 2k
   pour tout 0 <= i < k on a
   R[2i] = "alpha" chaîne à remplacer
   R[2i+1] = "beta" par beta (coût replace_n_p_weight)
   Si l (resp. r) est le + grand préfixe (resp. suffixe) de alpha et beta (i.e. alpha = l alpha' r et beta = l beta' r)
   alors on remplace alpha' par beta' dans les contextes l et r
*/
extern void sxspell_replace_n_p (
				 struct FSA *FSA, 
				 unsigned char *R[], 
				 SXINT R_size, 
				 SXINT replace_weight
				);

/* Suppression des transitions elementaires ds FSA (on ne doit pas obtenir de chaine vide) */
/* Le poids d'une suppression est de suppress_weight */
/* Tous les caracteres peuvent etre supprimes */
extern void sxspell_suppress_all (
				  struct FSA    *FSA,
				  SXINT  sup_weight
				 );
/* Au + i caracteres consecutifs peuvent etre supprimes */
extern void sxspell_suppress_i (
				struct FSA    *FSA,
				SXINT         sup_weight,
				SXINT         i
			       );

/* Le swap de 2 caracteres consecutifs coute swap_weight */
extern void sxspell_swap (
			  struct FSA    *FSA,
			  SXINT         swp_weight
			 );

/* remplace p caracteres identiques successifs par un seul : nooooooooooonnnnnnnnnn => non */
extern void sxspell_no_repeat (
			       struct FSA    *FSA,
			       SXINT         p,
			       SXINT         new_weight
			      );

/* permet l'ajout en position pos de l'ensemble de char V de poids add_weight */
extern void sxspell_add (
			 struct FSA    *FSA,
			 SXINT         pos,
			 unsigned char *V,
			 SXINT         iw
			);

/* permet le remplacement du caracte`re en position pos par l'ensemble des char V de poids change_weight */
extern void sxspell_change (
			    struct FSA    *FSA,
			    SXINT         pos,
			    unsigned char *V,
			    SXINT         cw
			   );

/* Recherche ds dico les chaines du langage FSA qui verifient les contraintes de sxspell_init */
/* Le resultat est mis ds spell_result */
extern bool sxspell_do_it (
			      struct FSA          *FSA
			     );
	
/* Donne la taille des tables statiques */
/* Ex : T_NB (diacritics) donne le nb d'elements ds diacritics */
#define T_NB(t) ((sizeof(t)/sizeof(*t))-1)

extern void sxspell_model (unsigned char kind,
			   SXINT max_weight,
			   bool is_sub_word,
			   SXINT sub_word_weight,
			   char *mot,
			   struct sxdfa_comb *dico);

#define SXSPELL_spelling_nb(s)               (s).spell_result.spelling_nb
#define SXSPELL_x2id(s,x)                    (s).spell_result.ids[x]
#define SXSPELL_x2weight(s,x)                ((s).spell_result.weights[x].pos+(s).spell_result.weights[x].neg)
#define SXSPELL_id2str(s,id)                 SXWORD_get((s).spell_result.words, id)
#define SXSPELL_id2str_len(s,id)             SXWORD_len((s).spell_result.words, id)
#define SXSPELL_id2code(s,id)                (s).spell_result.codes[id]
#define SXSPELL_has_subwords(id)             ((id)<0)
#define SXSPELL_subword_foreach(s,id,b,t)    for (b=(s).spell_result.multiple_word_ids.list+(s).spell_result.multiple_word_ids.display[-id].X,\
						    t=(s).spell_result.multiple_word_ids.list+((s).spell_result.multiple_word_ids.display[-(id)+1].X-1);\
						  b<=t;(b)++)
#define SXSPELL_x2subword_id(b)              (*b)
#define SXSPELL_last_subword(b,t)            (b==t)
#endif /* sxspell_h */ 
