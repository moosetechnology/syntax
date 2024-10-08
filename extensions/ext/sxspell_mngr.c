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


/* Premisses d'un correcteur orthographique */
/* idee un mot (sequences de lettres) est transforme en FSA non deterministe (avec transitions epsilon possibles
   mais sans cycle) qui represente toutes les "variations" possibles de ce mot.
   A chaque transition est associee un poids.
   A chaque chemin de l'etat initial a l'etat final est associe le poids de la transformation
   qui est une fonction des poids des transitions utilisees.
   Ce poids represente la distance qui existe entre le mot d'origine et le mot corrige'.
   L'utilisateur peut utiliser sxspell de 3 fac,ons :
     - Option 0 (ALL) : calcul de toutes les corrections
     - Option 1 (BEST) : calcul des seules corrections de poids minimal.
     - Option n, n > 1 (NBEST) : calcul des corrections dont les poids sont les n + petits.

  Voir le code de sxspell_model pour un exemple d'utilisation.

  L'utilisateur (au cours de l'appel a sxspell_init)  doit donner le poids maximal qu'il ne veut pas voir depasser
  par aucune des corrections proposees.  Si cette valeur est UNBOUNDED, toutes les corrections peuvent etre examinees.
  Le poids des transitions (elementaires) est un parametre fourni par l'utilisateur aux differentes procedures
  qui construisent le FSA.  Ce poids est un entier relatif.  S'il est negatif ou nul, la composition d'erreurs
  multiples (le faisant intervenir) se fait a cout nul; sinon le cout d'une composition est de compose_weight,
  argument utilisateur fournit lors de l'appel a sxspell_init.
  Ds l'exemple propose' le changement de casse, la correction de diacritics ou l'ajout de tiret se fait
  a un cout de composition nul.  De plus, le changement de casse se fait a un cout total nul.

  ATTENTION : l'ordre ds lequel les appels des procedures qui construisent le FSA peut avoir de l'importance ...
  Chaque procedure augmente le FSA tel qu'il etait avant son appel et retourne un automate qui contient les specifs
  
  - sxspell_init_fsa pour la chaine mot part d'un FSA vide et construit un automate (lineaire) initial dont le nb d'etats (d'origine)
    est |mot|+1, dont chaque transition a un poids vide et qui ne reconnait que ce mot.
  - sxspell_replace (changement d'une lettre par une autre) se contente d'ajouter des transitions ponderees entre les etats d'origine.
  - sxspell_suppress_i supprime i lettres consecutives entre les etats d'origines.  Elle cree de nouveaux etats dits supplementaires.
  - sxspell_suppress_all specifie la suppression d'un nb qcq de lettres qcq entre les etats d'origine.  Elle cree des transitions
    epsilon.
  - sxspell_swap echange 2 lettres entre 3 etats d'origine successifs. Elle cree des etats supplementaires.
  - sxspell_replace_n_p remplace n lettres entre n+1 etats d'origine successifs par p lettres..
    Elle peut creer des etats supplementaires.
    Elle a une notion de contexte gauche et droit.
  - sxspell_add Ajoute depuis l'etat pos (position ds le mot d'origine, on part de 1) une transition sur les lettres specifiees.
    Elle cree des etats supplementaires.
*/


static char	ME [] = "sxspell_mngr";

#include "sxversion.h"
#include "sxunix.h"
#include "fsa.h"
#include "sxspell.h"
#include "sxstack.h"
#include "varstr.h"
#include "X.h"

char WHAT_SXSPELL_MNGR[] = "@(#)SYNTAX - $Id: sxspell_mngr.c 3976 2024-05-30 10:26:48Z garavel $" WHAT_DEBUG;

static struct FSA          *cur_FSA;
static struct spell_result *cur_spell_result;
static struct sxdfa_comb   *cur_dico;

/* Structures de travail */
static SXINT                 *spell_result_id_stack;
static XH_header           *cur_multiple_word_ids_ptr;
static struct weight       *spell_result_weight_stack;
static SXINT                 cross_id_size, *cross_id;
static struct weight       *cross_weight;
static struct weight       *ij_weights; 
static SXINT                 sorted_size, *sorted, *ij_total_weights, *ij_ids;
static SXINT                 cur_spell_result_stack_bot;
/* Ca permet de savoir s'il existe un chemin ds le DAG d'entree entre les etats p et q */
/* L'automate support du transducteur specifiant l'ensemble des corrections possibles etant un DAG, on doit etre capable de
   savoir s'il existe ds ce dag un chemin de p a q sans faire de fermeture transitive.  C'est le role des "local_path".
   Ce sont ces chemins qui partent et aboutissent a des etats du spine.  Chaque chemin a son numero le "local_path_nb"
   Il existe un chemin de p a q ssi
   - p == q
   - ou si p et q appartiennent au meme local_path_nb p < q
   - ou sinon si l'etat fin du local_path de p est <= a l'etat debut du local_path de q
*/
#define FSA_has_path(p,q)  (p==q ||\
                            ((fsa_state2local_path [p] == fsa_state2local_path [q]) && (p < q)) ||\
                            fsa_state2requiv [p] <= fsa_state2lequiv [q])

static SXINT                 *fsa_state2local_path, local_path_nb;
static SXINT                 *fsa_state2lequiv, *fsa_state2requiv, fsa_state2equiv_size;

#define UNDEF_WEIGHT       UNBOUNDED

/* PB avec NBEST : il semble quue l'on soit oblige' de traiter NBEST comme ALL et qu'il soit
   donc impossible d'eliminer a priori une correction, meme si son poids est au_dessus du max (temporaire) */
/* Ca vient du fait que le meme resultat peut etre obtenu par plusieurs compositions de corrections differentes */
/* Supposons qu'a un moment une correction c de poids p ne soit pas retenue parce que la table courante est pleine (elle contient
   des corrections dont l'ensemble des poids est de cardinal NBEST) et qu'une correction d de poids q y figure et que
   cette correction est la seule de poids q.  On essaie de mettre une correction d' identique a d mais de poids q' < q.  
   Supposons de plus qu'il existe deja ds la table courante d'autres corrections de poids q'.  On va donc supprimer d
   (et donc gagner une place ds la table : cardinal(TABLE)==NBEST-1) et inserer d' (avec toujours  cardinal(TABLE)==NBEST-1).
   Donc en fin, on se retrouve avec une table non pleine dans laquelle la correction c aurait tres bien pu se mettre. */
/* Idee : on calcule sur une longueur de disons 2*NBEST en se souvenant du poids min qui n'a pas ete memorise'
   Si en fin le + mauvais des NBEST est < a ce poids min => OK
   sinon, on relance le tout comme si c'etait ALL */
static SXINT                 underflow_nb;

static bool             FSA_HAS_EPS_TRANS /* ssi sxspell_suppress_all a ete appele */;
static SXINT                 MEMO_BASE /* Nb d'etats initiaux+1 */;
static bool             WORD_HAS_SPACES /* Le mot a corrige a-t-il des espaces */;

static SXINT                 *memo_hd;
static SXINT                 *memo_id_stack, *memo_weight_limit;
static struct weight       *memo_weight_stack;
static X_header            key_hd;
static SXINT                 *key2index;
static SXINT                 KEY_SHIFT;
#define MAKE_KEY(n,p,q)    (((((n)<<KEY_SHIFT)+(p))<<KEY_SHIFT)+(q))

static void
key_hd_oflw (SXINT old_size, SXINT new_size)
{
  sxuse(old_size); /* pour faire taire gcc -Wunused */
  key2index = (SXINT*) sxrealloc (key2index, new_size+1, sizeof (SXINT));
}

/* Petit package de gestion des char sets et des char weights */
/* Taille d'un SXBA pour stocker 256 bits (en nb de SXBA_ELT) */
#define CHAR_SET_SIZE        (SXNBLONGS (256)+1)

/* On alloue par blocs de 128!! */
#define AREA_SHIFT    7
#define AREA_AND      (~(((unsigned int) ~0)<<AREA_SHIFT))
#define AREA_SIZE     (1<<AREA_SHIFT) 
#define AREA_PAGE(n)  ((n)>>AREA_SHIFT)
#define AREA_DELTA(n) ((n)&AREA_AND)

#define AREA2TRANS(fsa,id)   fsa->area [AREA_PAGE (id)] [AREA_DELTA (id)]
#define AREA_RAZ(fsa)        fsa->area_id = 0

static void
area_allocate2 (struct FSA* FSA, SXINT i)
{
  SXINT             x; 
  struct trans    *p;
  SXBA            elt_ptr;
  struct weight   *weight_ptr;

  p = FSA->area [i] = (struct trans*) sxalloc (AREA_SIZE, sizeof (struct trans));

  p [0].char_set = elt_ptr = (SXBA) sxalloc (AREA_SIZE*CHAR_SET_SIZE, sizeof (SXBA_ELT));
  *elt_ptr = 256;

  p [0].char_weight = weight_ptr = (struct weight*) sxalloc (AREA_SIZE*256, sizeof (struct weight));

  for (x = 1; x < AREA_SIZE; x++) {
    p [x].char_set = (elt_ptr += CHAR_SET_SIZE);
    *elt_ptr = 256;

    p [x].char_weight = (weight_ptr += 256);
  }
}

static void
area_allocate (struct FSA *FSA, SXINT size)
{
  SXINT i;

  AREA_RAZ (FSA);
  FSA->area_page_nb = AREA_PAGE (size)+1;

  FSA->area = (struct trans**) sxalloc (FSA->area_page_nb, sizeof (struct trans*));

  for (i = 0; i < FSA->area_page_nb; i++)
    area_allocate2 (FSA, i);
}


static void
area_oflw (SXINT old_size, SXINT new_size)
{
  SXINT new_page_nb, i;

  sxuse(old_size); /* pour faire taire gcc -Wunused */
  new_page_nb = AREA_PAGE (new_size)+1;

  if (new_page_nb > cur_FSA->area_page_nb) {
    cur_FSA->area = (struct trans**) sxrealloc (cur_FSA->area, new_page_nb, sizeof (struct trans*));

    for (i = cur_FSA->area_page_nb; i < new_page_nb; i++)
      area_allocate2 (cur_FSA, i);

    cur_FSA->area_page_nb = new_page_nb;
  }
}


static void
area_free (struct FSA *FSA)
{
  SXINT             x;
  struct trans    *p;

  for (x = FSA->area_page_nb-1; x >= 0; x--) {
    p = FSA->area [x];

    sxfree (p->char_set);
    sxfree (p->char_weight);
    sxfree (p);
  }

  sxfree (FSA->area), FSA->area = NULL;
}

/* stocke les corrections (partielles) */
static VARSTR              lvstr, rvstr;

static struct weight       NUL_WEIGHT, total_weight;


//static SXUINT        suffix_mask;
static bool              is_suffix;
static SXUINT              base, tooth, base_shift, class_mask, stop_mask, last_final_id;
static SXUINT              *comb_vector;
static unsigned char	   *char2class;



/* Debordement de cur_spell_result->words */
static void
words_oflw (SXINT old_size, SXINT new_size)
{
    sxuse(old_size); /* pour faire taire gcc -Wunused */
    cur_spell_result->codes = (SXINT *) sxrealloc (cur_spell_result->codes, new_size+1, sizeof (SXINT));
}

void
sxspell_init (struct sxdfa_comb *dico,
	      struct FSA *FSA,
	      unsigned char kind,
	      SXINT max_weight, 
	      SXINT cmp_weight,
	      bool seek_sub_words,
	      SXINT sub_word_weight)
{

  FSA->dico = dico;
  is_suffix = !(dico->private.from_left_to_right);

  FSA->spell_result.kind = (SXINT)kind; /* ALL, BEST ou NBEST */
  FSA->spell_result.max_weight = max_weight; /* On ne s'occupe pas des corrections dont le poids est superieur a max_weight */
  FSA->spell_result.cmp_weight = cmp_weight; /* Poids pour chaque nelle erreur multiple */
  FSA->spell_result.seek_sub_words = (SXINT)seek_sub_words;

  if (seek_sub_words) {
    FSA->spell_result.sub_word_weight = NUL_WEIGHT;

    if (sub_word_weight < 0)
      FSA->spell_result.sub_word_weight.neg = -sub_word_weight;
    else
      FSA->spell_result.sub_word_weight.pos = sub_word_weight;
  }
}

void
sxspell_final (struct FSA *FSA)
{
  
  if (FSA->spell_result.words.table != NULL) {
    sxword_free (&(FSA->spell_result.words));
    sxfree (FSA->spell_result.codes), FSA->spell_result.codes = NULL;
    XH_free (&(FSA->spell_result.multiple_word_ids));
  }

  if (spell_result_weight_stack) {
    DFREE_STACK (spell_result_id_stack);
    sxfree (spell_result_weight_stack), spell_result_weight_stack = NULL;
    varstr_free (lvstr);
    varstr_free (rvstr);
  }

  if (cross_id) {
    sxfree (cross_id), cross_id = NULL;
    sxfree (cross_weight), cross_weight = NULL;
  }

  if (sorted) {
    sxfree (sorted), sorted = NULL;
    sxfree (ij_total_weights), ij_total_weights = NULL;
    sxfree (ij_weights), ij_weights = NULL;
    sxfree (ij_ids), ij_ids = NULL;
  }
}

/* On represente un mot sous forme de FSA */
void
sxspell_init_fsa (char *mot, SXINT lgth, struct FSA *FSA)
{
  SXINT           i, q, pair;
  unsigned char car;
  SXBA          char_set;
  struct weight *char_weight;
  struct trans  trans;

#if 0
  if (lgth == 0) return;
#endif /* 0 */

  cur_FSA = FSA;
  cur_spell_result = &(FSA->spell_result);
  cur_dico = FSA->dico;

  FSA->word = mot;
  FSA->q0 = q = 1; /* etat initial */
  FSA->Q_init = FSA->Q_nb = lgth+1; /* nb d'etats */

  if (FSA->orig_char_set == NULL) {
    /* 1er appel */
    FSA->orig_char_set = sxba_calloc (256); /* ensemble des caracteres initiaux du mot */
  
    FSA->final_set = sxba_calloc (FSA->Q_nb+1);

    XxY_alloc (&(FSA->Q_hd), mot, 10*lgth+1, 1, FSA->dico->private.from_left_to_right /* Xforeach */, !(FSA->dico->private.from_left_to_right) /* Yforeach */, area_oflw, NULL);

    area_allocate (FSA, XxY_size (FSA->Q_hd)+1);
  }
  else {
    sxba_empty (FSA->orig_char_set);
    sxba_empty (FSA->final_set);

    if (SXBASIZE (FSA->final_set) <= (SXUINT)FSA->Q_nb)
      FSA->final_set = sxba_resize (FSA->final_set, FSA->Q_nb+1);

    XxY_clear (&(FSA->Q_hd));
    FSA->Q_hd.name = mot;  
  
    AREA_RAZ (FSA);
  }

  /* Structures allouees 1 seule fois, qq soit le nb de "FSA" actifs */
  if (fsa_state2lequiv == NULL) {
    fsa_state2equiv_size = 5*(lgth+1); /* ne doit pas etre nul si mot vide!! */
    fsa_state2lequiv = (SXINT*) sxalloc (fsa_state2equiv_size+1, sizeof (SXINT));
    fsa_state2requiv = (SXINT*) sxalloc (fsa_state2equiv_size+1, sizeof (SXINT));
    fsa_state2local_path = (SXINT*) sxalloc (fsa_state2equiv_size+1, sizeof (SXINT));
  }
  else {
    if (fsa_state2equiv_size < 5*lgth) {
      fsa_state2equiv_size = 5*lgth;
      fsa_state2lequiv = (SXINT*) sxrealloc (fsa_state2lequiv, fsa_state2equiv_size+1, sizeof (SXINT));
      fsa_state2requiv = (SXINT*) sxrealloc (fsa_state2requiv, fsa_state2equiv_size+1, sizeof (SXINT));
      fsa_state2local_path = (SXINT*) sxrealloc (fsa_state2local_path, fsa_state2equiv_size+1, sizeof (SXINT));
    }
  }
  
  SXBA_1_bit (FSA->final_set, FSA->Q_nb); /* Le seul etat final */

  for (i = 0; i < lgth; q++, i++) {
    fsa_state2lequiv [q] = fsa_state2requiv [q] = q;
    fsa_state2local_path [q] = 0;
    XxY_set (&(FSA->Q_hd), q,  q+1, &pair); /* transition de q a q+1 ... */
    trans = AREA2TRANS (FSA, pair);
    char_set = trans.char_set;
    char_weight = trans.char_weight;
    sxba_empty (char_set);
    car = (unsigned char) mot [i];
    SXBA_1_bit (char_set, car);
    SXBA_1_bit (FSA->orig_char_set, car);
    /* poids nul sur la chaine origine */
    char_weight [car] = NUL_WEIGHT;
  }

  fsa_state2lequiv [FSA->Q_nb] = fsa_state2requiv [FSA->Q_nb] = FSA->Q_nb;
  fsa_state2local_path [FSA->Q_nb] = 0;

  FSA_HAS_EPS_TRANS = false;
}


/* FSA Freeing */
void
sxspell_final_fsa (struct FSA *FSA)
{
  if (FSA->orig_char_set /* Prudence */) {
    sxfree (FSA->orig_char_set), FSA->orig_char_set = NULL;
    sxfree (FSA->final_set), FSA->final_set = NULL;

    XxY_free (&(FSA->Q_hd));

    area_free (FSA);
  }

  if (fsa_state2lequiv) {
    sxfree (fsa_state2lequiv), fsa_state2lequiv = NULL;
    sxfree (fsa_state2requiv), fsa_state2requiv = NULL;
    sxfree (fsa_state2local_path), fsa_state2local_path = NULL;
  }

  if (memo_hd) {
    sxfree (memo_hd), memo_hd = NULL;
    DFREE_STACK (memo_id_stack);
    sxfree (memo_weight_limit), memo_weight_limit = NULL;
    sxfree (memo_weight_stack), memo_weight_stack = NULL;
    X_free (&key_hd);
    sxfree (key2index), key2index = NULL;
  }
}


#define SXBA_fdcl(n,l)	SXBA_ELT n [SXNBLONGS (l) + 1] = {l,}
static SXBA_fdcl (working_char_set, 256);

static unsigned char *replace_strings [256];
static SXBA_fdcl (replace_set, 256);

#define unicode ((SXINT) (unsigned char) '\201')


#define get_total_weight(weight) ((weight).pos+(weight).neg)


static struct weight
get_new_compose_weight (struct weight old_weight, struct weight oper_weight, SXINT oper_nb)
{
  SXUINT weight;
    
  old_weight.neg += oper_nb*oper_weight.neg;
  
  if ((weight = oper_weight.pos) > 0) {
    if (old_weight.pos == 0)
      old_weight.pos = oper_nb*weight + (oper_nb-1)*cur_spell_result->cmp_weight;
    else
      old_weight.pos += oper_nb*(weight+cur_spell_result->cmp_weight);
  }

  return old_weight;
}


static struct weight
get_new_min_weight (struct weight old_weight, struct weight new_weight)
{
  return (old_weight.neg+old_weight.pos < new_weight.neg+new_weight.pos) ? old_weight : new_weight;
}


/* On remplace chaque sequence de p lettres identiques par une seule lettre */
void
sxspell_no_repeat (struct FSA *FSA, SXINT p, SXINT nr_weight)
{
  SXINT           pair, pair2, car, n, couple;
  struct weight cur_weight, weight, *new_char_weight;
  struct trans  trans, new_trans;
  SXBA          new_char_set; 
  bool       is_an_old_pair;

  if (p <= 1)
    return;
  
  if (nr_weight >= 0)
    cur_weight.pos = nr_weight, cur_weight.neg = 0;
  else
    cur_weight.neg = -nr_weight, cur_weight.pos = 0;

  /* FSA->Q_init == nb de transitions initiales+1 */
  for (pair = 1; pair <= FSA->Q_init-p; pair = pair2) {
    car = (SXINT) (unsigned char) FSA->word [pair-1];

    n = 1;

    for (pair2 = pair+1; pair2 < FSA->Q_init; pair2++) {
      if (car != (SXINT) (unsigned char) FSA->word [pair2-1])
	break;

      n++;
    }

    if (n >= p) {
      /* oui */
      /* On fabrique une transition (pair, pair2) */
      is_an_old_pair = XxY_set (&(FSA->Q_hd), pair,  pair2, &couple);
    
      new_trans = AREA2TRANS (FSA, couple);
      new_char_set = new_trans.char_set;
      new_char_weight = new_trans.char_weight;

      if (!is_an_old_pair)
	sxba_empty (new_char_set);

      trans = AREA2TRANS (FSA, pair+1);

      weight = get_new_compose_weight (trans.char_weight [car], cur_weight, (SXINT)1);

      if (SXBA_bit_is_reset_set (new_char_set, car)) {
	/* nouveau => new_char_weight [car] == 0 */
	new_char_weight [car] = weight;
      }
      else {
	new_char_weight [car] = get_new_min_weight (new_char_weight [car], weight);
      }
    }
  }
}

/* R est un vecteur de pointeurs vers des chaines de caracteres.  Sa taille est de R_size == 2k
   pour tout 0 <= i < k on a
   R[2i] = "A" caractere a remplacer
   R[2i+1] = "���a���" les remplac,ant (cout replace_weight)
   On ne remplace que les transitions de la chaine d'origine
*/
void
sxspell_replace (struct FSA *FSA, unsigned char *R[], SXINT R_size, SXINT replace_weight)
{
  SXINT           i, car, pair;
  SXBA          char_set;
  unsigned char new_car, *line;
  struct weight cur_weight, *char_weight;
  struct trans  trans;

  if (R_size <= 0) return;

  if (replace_weight >= 0)
    cur_weight.pos = replace_weight, cur_weight.neg = 0;
  else
    cur_weight.neg = -replace_weight, cur_weight.pos = 0;

  for (i = 0; i < R_size; i += 2) {
    car = (SXINT) R[i][0];

    if (car == unicode)
      /* On suppose que c'est suivi du bon codage de char !! */
      car = (SXINT) R[i][1];

    if (SXBA_bit_is_set (FSA->orig_char_set, car)) {
      replace_strings [car] = R[i+1];
      SXBA_1_bit (replace_set, car);
    }
  }

  /* FSA->Q_init == nb de transitions initiales+1 */
  for (pair = 1; pair < FSA->Q_init; pair++) {
    car = (SXINT) (unsigned char) FSA->word [pair-1];

    if (SXBA_bit_is_set (replace_set, car)) {
      /* car est un caractere de la chaine d'origine qui a des remplacants */
      /* Le meme car peut avoir plusieurs occur ds lmot d'origine */
      trans = AREA2TRANS (FSA, pair);
      char_set = trans.char_set;
      char_weight = trans.char_weight;
      line = replace_strings [car];

      while ((new_car = *line++)) {
	/* On ajoute new_car a char_set */
	if (new_car != unicode) {
	  if (SXBA_bit_is_reset_set (char_set, new_car))
	    /* nouveau */
	    char_weight [new_car] = cur_weight;
	  else {
	    /* On prend le min ... */
	    char_weight [new_car] = get_new_min_weight (char_weight [new_car], cur_weight);
	  }
	}
      }
    }
  }

  sxba_empty (replace_set);
}

/* generalisation de sxspell_replace : on remplace des sequences de n caracteres par des sequences de p caracteres */
/* Les tailles n et p sont donnees par les longueurs de R[2*i] et R(2*i+1] */
/* Le 13/10/04 on permet les meta-caracteres '^' et '$' qui designent le debut et la fin */
/* Le 20/12/04 on permet des contextes gauches et droits (+ longs prefixes et suffixes) */
void
sxspell_replace_n_p (struct FSA *FSA, unsigned char *R[], SXINT R_size, SXINT replace_weight)
{
  SXINT           n, p, i, j, top_j, k, car, pair, new_j, prefix_lgth, suffix_lgth, min_n_p, true_n;
  SXBA          new_char_set, char_set;
  char          *mot;
  unsigned char true_car, true_rcar, *line, *left_line, *right_line, *replace_line, *r_line;
  bool       is_an_old_pair, is_first, is_last, is_ps_done;
  struct weight cur_weight, *char_weight, *new_char_weight;
  struct trans  trans, trans1;

  sxinitialise(prefix_lgth); /* pour faire taire "gcc -Wuninitialized" */

  if (R_size <= 0) return;

  if (replace_weight >= 0)
    cur_weight.pos = replace_weight, cur_weight.neg = 0;
  else
    cur_weight.neg = -replace_weight, cur_weight.pos = 0;

  for (i = 0; i < R_size; i += 2) {
    line = R [i];
    right_line = line + strlen ((char*) line);

    if ((SXINT) *line == '^') {
      is_first = true;
      line++;
    }
    else
      is_first = false;

    left_line = line;

    if ((SXINT) right_line [-1] == '$') {
      right_line--;
      is_last = true;
    }
    else
      is_last = false;

    n = 0;

    while (left_line < right_line) {
      car = (SXINT) *left_line;

      if (car != unicode) {
	if (!SXBA_bit_is_set (FSA->orig_char_set, car))
	  break;

	n++;
      }

      left_line++;
    }

    top_j = FSA->Q_init-n-1;

    if (left_line == right_line && top_j >= 0) {
      /* Tous les caracteres du modele sont ds le source et le modele ne depasse pas la taille du source */
      /* Le modele est de taille n */
      left_line = replace_line = R [i+1];
	    
      /* calcul de p, la longueur du remplac,ant */
      p = 0;

      while ((car = (SXINT) *left_line++) != 0) {
	if (car != unicode) {
	  p++;
	}
      }
	
      if (n > 0) {
	while ((SXINT) (true_car = *line) == unicode)
	  line++;

	mot = FSA->word;
	j = 0;
	is_ps_done = false;
	true_n = n; /* vraie longueur du modele (y compris les contextes eventuels) */

	for (;;) {
	  if ((!is_first && !is_last) || (is_first && j == 0) || (is_last && j == top_j)) {
	    /* Le cas is_first et is_last est traite' par is_first !! */
	    if ((unsigned char) mot [j] == true_car) {
	      /* debut_possible */
	      left_line = line;
	      /* line, left_line et right_line pointent ds le modele */
	  
	      for (k = j+1; k < j+true_n; k++) {
		while ((SXINT)(true_car = *++left_line) == unicode);

		if ((unsigned char) mot [k] != true_car)
		  break;
	      }

	      if (k == j+true_n) {
		/* La sequence de n caracteres se trouve ds mot et commence en position j+1 */

		/* On regarde si le modele et son remplacant on des prefixes et des suffixes en commun */
		if (!is_ps_done) {
		  /* On cherche les prefixes et les suffixes communs entre R[i] et R[i+1] */
		  prefix_lgth = suffix_lgth = 0;

		  if (p > 0) {
		    left_line = line;
		    r_line = replace_line;
		    min_n_p = (n < p) ? n : p;
			
		    do {
		      while ((SXINT)(true_car = *left_line++) == unicode);
		      while ((SXINT)(true_rcar = *r_line++) == unicode);

		      if (true_car != true_rcar)
			break;

		      prefix_lgth++;
		    } while (prefix_lgth <= min_n_p);

		    if (min_n_p -= prefix_lgth) {
		      left_line = right_line;
		      r_line = replace_line + strlen ((char*) replace_line);
			
		      do {
			while ((SXINT)(true_car = *--left_line) == unicode);
			while ((SXINT)(true_rcar = *--r_line) == unicode);

			if (true_car != true_rcar)
			  break;

			suffix_lgth++;
		      } while (suffix_lgth <= min_n_p);
		    }
		  }
		
		  n -= prefix_lgth+suffix_lgth;

		  if (n == 0) {
		    /* On est ds le cas par exemple "m" -> "mm" (prefix_lgth==1 && suffix_lgth==0) */
		    /* On ne laisse pas un modele vide ... */
		    /* ... j'enleve un caractere du ctxt gauche ou droit */
		    n = 1;

		    if (prefix_lgth)
		      prefix_lgth--;
		    else
		      suffix_lgth--;
		  }

		  p -= prefix_lgth+suffix_lgth;

		  /* ici n et p ont pour valeur les longueurs du modele et du remplacant, sans compter les longueurs
		     des prefixes et suffixes */
		  is_ps_done = true;
		  /* L'extraction des prefixe et suffixe du modele et du remplacant sont faits */
		}

		new_j = j + prefix_lgth;

		if (p > 0) {
		  /* On va donc creer p-1 intermediaires */
		  if (FSA->Q_nb+p >= fsa_state2equiv_size) {
		    fsa_state2equiv_size *= 2;
		    fsa_state2equiv_size += p; /* prudence !! */
		    fsa_state2lequiv = (SXINT*) sxrealloc (fsa_state2lequiv, fsa_state2equiv_size+1, sizeof (SXINT));
		    fsa_state2requiv = (SXINT*) sxrealloc (fsa_state2requiv, fsa_state2equiv_size+1, sizeof (SXINT));
		    fsa_state2local_path = (SXINT*) sxrealloc (fsa_state2local_path, fsa_state2equiv_size+1, sizeof (SXINT));
		  }

		  /* On saute le prefixe commun */
		  r_line = replace_line + prefix_lgth;

		  for (k = 1; k <= p; k++) {
		    is_an_old_pair = XxY_set (&(FSA->Q_hd),
					      (k == 1) ? new_j+1 : FSA->Q_nb, (k == p) ? new_j+1+n : FSA->Q_nb+1,
					      &pair);

		    if (k != p) {
		      FSA->Q_nb++;
		      fsa_state2lequiv [FSA->Q_nb] = new_j+1;
		      fsa_state2requiv [FSA->Q_nb] = new_j+1+n;

		      if (k == 1)
			local_path_nb++;

		      fsa_state2local_path [FSA->Q_nb] = local_path_nb;
		    }


		    while ((car = (SXINT) *r_line++) == unicode);

		    trans = AREA2TRANS (FSA, pair);
		    new_char_set = trans.char_set;
		    new_char_weight = trans.char_weight;

		    if (!is_an_old_pair)
		      sxba_empty (new_char_set);

		    if (k == 1) {
		      /* on ne change le poids que de la 1ere transition */

		      if (SXBA_bit_is_reset_set (new_char_set, car))
			/* nelle trans */
			new_char_weight [car] = cur_weight;
		      else {
			/* On prend le min (new_j+1 et new_j+1+n sont des etats d'origine) */
			new_char_weight [car] = get_new_min_weight (new_char_weight [car], cur_weight);
		      }
		    }
		    else {
		      SXBA_1_bit (new_char_set, car);
		      new_char_weight [car] = NUL_WEIGHT;
		    }
		  }
		}
		else {
		  /* Suppression de n caracteres */
		  /* On cree une transition epsilon entre new_j+1 et new_j+1+n */
		  FSA_HAS_EPS_TRANS = true;
		
		  is_an_old_pair = XxY_set (&(FSA->Q_hd), new_j+1, new_j+1+n, &pair);

		  trans = AREA2TRANS (FSA, pair);
		  new_char_set = trans.char_set;
		  new_char_weight = trans.char_weight;

		  if (!is_an_old_pair)
		    sxba_empty (new_char_set);

		  if (SXBA_bit_is_reset_set (new_char_set, 0))
		    new_char_weight [0] = cur_weight;
		  else
		    new_char_weight [0] = get_new_min_weight (new_char_weight [0], cur_weight);
		}
	      }

	      /* On remet en place pour la suite */
	      true_car = *line;
	    }
	  }

	  if (j == 0 && is_first)
	    /* On quitte aussi si ...
	       ... le modele est de la forme "^a1a2...an$" et le mot source est de longueur n */
	    break;

	  j++;

	  if (is_last && j <= top_j)
	    j = top_j;
	  else {
	    if (j > top_j)
	      break;
	  }
	}
      }
      else {
	/* le modele est vide */
	if (p > 0) {
	  /* pas ds le cas
	     "^$", "",
	  */
	  if (is_first || is_last) {
	    /* insertion en debut ou en fin */
	    if (is_first)
	      /* ... et is_last !! */
	      j = 1;
	    else
	      j = FSA->Q_init;

	    /* insertion apres j  */
	    /* On va donc creer p intermediaires a partir de l'etat 1 */
	    if (FSA->Q_nb+p >= fsa_state2equiv_size) {
	      fsa_state2equiv_size *= 2;
	      fsa_state2equiv_size += p; /* prudence !! */
	      fsa_state2lequiv = (SXINT*) sxrealloc (fsa_state2lequiv, fsa_state2equiv_size+1, sizeof (SXINT));
	      fsa_state2requiv = (SXINT*) sxrealloc (fsa_state2requiv, fsa_state2equiv_size+1, sizeof (SXINT));
	      fsa_state2local_path = (SXINT*) sxrealloc (fsa_state2local_path, fsa_state2equiv_size+1, sizeof (SXINT));
	    }

	    for (k = 1; k <= p; k++) {
	      XxY_set (&(FSA->Q_hd), (k == 1) ? j : FSA->Q_nb, FSA->Q_nb+1, &pair);

	      FSA->Q_nb++;
	      fsa_state2lequiv [FSA->Q_nb] = j;
	      fsa_state2requiv [FSA->Q_nb] = ((j+1) <= FSA->Q_init) ? (j+1) : j; /* !! */

	      if (k == 1)
		local_path_nb++;

	      fsa_state2local_path [FSA->Q_nb] = local_path_nb;

	      while ((car = (SXINT) *replace_line++) == unicode);

	      /* transition sur car */
	      trans = AREA2TRANS (FSA, pair);
	      new_char_set = trans.char_set;
	      new_char_weight = trans.char_weight;

	      sxba_empty (new_char_set);
		  
	      SXBA_1_bit (new_char_set, car);
	      /* on ne met le poids que sur la 1ere transition */
	      new_char_weight [car] = (k == 1) ? cur_weight : NUL_WEIGHT;
	    }
		  
	    /* On remet en place */
	    replace_line = R [i+1];

	    if (SXBA_bit_is_set (FSA->final_set, j)) {
	      /* mot vide */
	      FSA->final_set = sxba_resize (FSA->final_set, FSA->Q_nb+1);
	      SXBA_1_bit (FSA->final_set, FSA->Q_nb);
	    }
	    else {
	      /* On duplique la transition de (j, j+1) sur (FSA->Q_nb, j+1) */
	      XxY_set (&(FSA->Q_hd), FSA->Q_nb, j+1, &pair);
	      /* pair est nouveau */
	      trans = AREA2TRANS (FSA, pair);
	      new_char_set = trans.char_set;
	      new_char_weight = trans.char_weight;

	      trans1 = AREA2TRANS (FSA, j);
	      char_set = trans1.char_set;
	      char_weight = trans1.char_weight;

	      sxba_copy (new_char_set, char_set);
	      SXBA_0_bit (new_char_set, 0); /* On ne copie pas l'epsilon eventuel */
      
	      car = 0; /* prudence avec l'epsilon */
 
	      while ((car = sxba_scan (char_set, car)) >= 0) {
		new_char_weight [car] = char_weight [car];
	      }
	    }
	  }
	}
      }
    }
  }

  FSA->final_set = sxba_resize (FSA->final_set, FSA->Q_nb+1);
}

/* Suppression des transitions elementaires ds FSA (on ne doit pas obtenir de chaine vide) */
/* Le poids d'une suppression est de suppress_weight */
/* Attention, les remplacements elementaires deja effectues sont pris en compte (l'ordre des appels
   n'est donc pas indifferent)
   Ex si un 'e' a deja ete remplace' par '�' et si le mot est "te", la suppression du 't' conduira aux mots "e" et "�" */

/* Tous les caracteres du mot peuvent etre supprimes */
/* on met des transitions epsilon (ca peut etre tres cher a l'execution !!) */
void
sxspell_suppress_all (struct FSA *FSA, SXINT sup_weight)
{
  SXINT           pair, top_pair;
  struct weight cur_weight, *char_weight;
  SXBA          char_set;
  struct trans  trans;

  FSA_HAS_EPS_TRANS = true;

  top_pair = FSA->Q_init-1; /* nb de transitions initiales */

  if (sup_weight >= 0)
    cur_weight.pos = sup_weight, cur_weight.neg = 0;
  else
    cur_weight.neg = -sup_weight, cur_weight.pos = 0;

  for (pair = 1; pair <= top_pair; pair++) {
    /* on ajoute epsilon a la transition */
    trans = AREA2TRANS (FSA, pair);
    char_set = trans.char_set;
    char_weight = trans.char_weight;

    if (SXBA_bit_is_reset_set (char_set, 0))
      char_weight [0] = cur_weight;
    else
      char_weight [0] = get_new_min_weight (char_weight [0], cur_weight);
  }
}

/* Au + i>0 caracteres consecutifs peuvent etre supprimes */
void
sxspell_suppress_i (struct FSA *FSA, SXINT sup_weight, SXINT i)
{
  SXINT           pair, pair1, car, top_state, n, p;
  struct weight cur_weight, *new_char_weight, *char_weight, weight;
  SXBA          char_set, new_char_set;
  bool       is_an_old_pair; 
  struct trans  trans, trans1;

  top_state = FSA->Q_init-2;

  if (sup_weight >= 0)
    cur_weight.pos = sup_weight, cur_weight.neg = 0;
  else
    cur_weight.neg = -sup_weight, cur_weight.pos = 0;

  for (pair = 1; pair <= top_state; pair++) {
    /* on supprime la transition pair = (p, p+1) avec p == pair ... */
    /* .... et on la remplace par (p, p+2) avec les labels (actualises) de (p+1, p+2) */
    is_an_old_pair = XxY_set (&(FSA->Q_hd), pair,  pair+2, &pair1);

    trans1 = AREA2TRANS (FSA, pair1);
    new_char_set = trans1.char_set;
    new_char_weight = trans1.char_weight;

    if (!is_an_old_pair)
      sxba_empty (new_char_set);

    trans = AREA2TRANS (FSA, pair+1);
    char_set = trans.char_set;
    char_weight = trans.char_weight;

    car = 0; /* je saute l'epsilon !! */

    while ((car = sxba_scan (char_set, car)) >= 0) {
      weight = get_new_compose_weight (char_weight [car], cur_weight, (SXINT)1);

      if (SXBA_bit_is_reset_set (new_char_set, car)) {
	/* nouveau => new_char_weight [car] == 0 */
	new_char_weight [car] = weight;
      }
      else {
	new_char_weight [car] = get_new_min_weight (new_char_weight [car], weight);
      }
    }

    if (pair == top_state-1) {
      /* Le suffixe est de la forme p ->a p+1 ->b p+2 ->c p+3 et p+3 est un etat final */
      /* On cree p ->b p+3 (suppressions de a et c qui ne sont pas obtenus autrement) */
      is_an_old_pair = XxY_set (&(FSA->Q_hd), pair,  pair+3, &pair1);

      trans1 = AREA2TRANS (FSA, pair1);
      new_char_set = trans1.char_set;
      new_char_weight = trans1.char_weight;

      if (!is_an_old_pair)
	sxba_empty (new_char_set);

      trans = AREA2TRANS (FSA, pair+1);
      char_set = trans.char_set;
      char_weight = trans.char_weight;
 
      car = 0; /* On vire l'epsilon !! */

      while ((car = sxba_scan (char_set, car)) >= 0) {
	/* On compose char_weight [car] avec 2 suppressions ... */
	weight = get_new_compose_weight (char_weight [car], cur_weight, (SXINT)2);

	/* ... et on prend le min */
	if (SXBA_bit_is_reset_set (new_char_set, car)) {
	  new_char_weight [car] = weight;
	}
	else {
	  new_char_weight [car] = get_new_min_weight (new_char_weight [car], weight);
	}
      }
    }
    else {
      if (pair == top_state) {
	/* ... et on ajoute aussi les labels de (p, p+1) */
	trans = AREA2TRANS (FSA, pair);
	char_set = trans.char_set;
	char_weight = trans.char_weight;
 
	car = 0; /* On vire l'epsilon !! */

	while ((car = sxba_scan (char_set, car)) >= 0) {
	  /* On compose char_weight [car] avec 1 suppression ... */
	  weight = get_new_compose_weight (char_weight [car], cur_weight, (SXINT)1);

	  /* ... et on prend le min */
	  if (SXBA_bit_is_reset_set (new_char_set, car)) {
	    new_char_weight [car] = weight;
	  }
	  else {
	    new_char_weight [car] = get_new_min_weight (new_char_weight [car], weight);
	  }
	}
      }
    }
  }

  if (i > 1) {
    /* On ajoute des transitions vides sur tous les n-uplets consecutifs pour n = 2, 3, ..., i */
    for (n = 2; n <= i; n++) {
      for (p = top_state+2-n; p > 0; p--) {
	/* On cree p ->eps p+n */
	is_an_old_pair = XxY_set (&(FSA->Q_hd), p, p+n, &pair);
		
	trans = AREA2TRANS (FSA, pair);
	new_char_set = trans.char_set;
	new_char_weight = trans.char_weight;

	if (!is_an_old_pair)
	  sxba_empty (new_char_set);

	/* Cout de n suppressions */
	weight = get_new_compose_weight (NUL_WEIGHT, cur_weight, n);
	  
	if (SXBA_bit_is_reset_set (new_char_set, 0)) {
	  new_char_weight [0] = weight;
	}
	else {
	  /* On prend le min */
	  new_char_weight [0] = get_new_min_weight (new_char_weight [0], weight);
	}
      }
    }
  }

  FSA->final_set = sxba_resize (FSA->final_set, FSA->Q_nb+1);
}


/* Le swap de 2 caracteres consecutifs coute swap_weight */
void
sxspell_swap (struct FSA *FSA, SXINT swp_weight)
{
  SXINT           p, pair, car, top_state;
  struct weight cur_weight, *new_char_weight, *char_weight;
  SXBA          char_set, new_char_set;
  struct trans  trans, trans1;

  top_state = FSA->Q_init-2;

  if (swp_weight >= 0)
    cur_weight.pos = swp_weight, cur_weight.neg = 0;
  else
    cur_weight.neg = -swp_weight, cur_weight.pos = 0;

  if (FSA->Q_nb+top_state >= fsa_state2equiv_size) {
    fsa_state2equiv_size *= 2;
    fsa_state2equiv_size += top_state;
    fsa_state2lequiv = (SXINT*) sxrealloc (fsa_state2lequiv, fsa_state2equiv_size+1, sizeof (SXINT));
    fsa_state2requiv = (SXINT*) sxrealloc (fsa_state2requiv, fsa_state2equiv_size+1, sizeof (SXINT));
    fsa_state2local_path = (SXINT*) sxrealloc (fsa_state2local_path, fsa_state2equiv_size+1, sizeof (SXINT));
  }

  for (p = 1; p <= top_state; p++) {
    /* On remplace les transitions (p, p+1) et (p+1, p+2) 
       par les transitions (p, q) et (q, p+2) ou q est un nouvel etat.  Je ne reutilise pas p+1
       pour ne pas confondre ces (nouvelles) transitions avec les transitions du mot initial.
       On affecte a (p,q) les labels de (p+1, p+2) et a (q, p+2) ceux de (p, q) */
    FSA->Q_nb++;
    /* FSA->Q_nb == etat intermediaire q */
    fsa_state2lequiv [FSA->Q_nb] = p;
    fsa_state2requiv [FSA->Q_nb] = p+2;
    local_path_nb++;
    fsa_state2local_path [FSA->Q_nb] = local_path_nb;

    XxY_set (&(FSA->Q_hd), p, FSA->Q_nb, &pair); /* On ajoute la transition (p, etat intermediaire) ... */
    /* pair est nouveau */
    trans = AREA2TRANS (FSA, pair);
    new_char_set = trans.char_set;
    new_char_weight = trans.char_weight;

    trans1 = AREA2TRANS (FSA, p+1);
    char_set = trans1.char_set;
    char_weight = trans1.char_weight;

    sxba_copy (new_char_set, char_set);
    SXBA_0_bit (new_char_set, 0); /* On ne copie pas l'epsilon eventuel */
      
    car = 0; /* prudence avec l'epsilon */
 
    /* Le poids du swap est mis uniquement sur la 1ere transition ... */
    while ((car = sxba_scan (char_set, car)) >= 0) {
      new_char_weight [car] = get_new_compose_weight (char_weight [car], cur_weight, (SXINT)1);
    }

    XxY_set (&(FSA->Q_hd), FSA->Q_nb, p+2, &pair); /* ... et  (etat intermediaire, p+2) ... */
    /* pair est nouveau */
    trans = AREA2TRANS (FSA, pair);
    new_char_set = trans.char_set;
    new_char_weight = trans.char_weight;

    trans1 = AREA2TRANS (FSA, p);
    char_set = trans1.char_set;
    char_weight = trans1.char_weight;

    sxba_copy (new_char_set, char_set);
    SXBA_0_bit (new_char_set, 0); /* On ne copie pas l'epsilon eventuel */

    car = 0; /* prudence avec l'epsilon */
 
    while ((car = sxba_scan (char_set, car)) >= 0) {
      new_char_weight [car] = char_weight [car];
    }
  }

  FSA->final_set = sxba_resize (FSA->final_set, FSA->Q_nb+1);
}

/* permet l'ajout en position pos de l'ensemble de char V de poids add_weight */
void
sxspell_add (struct FSA *FSA,
	     SXINT pos,
	     unsigned char *V,
	     SXINT iw)
{
  SXINT           pair, car;
  SXBA          new_char_set, char_set;
  struct weight cur_weight, *new_char_weight, *char_weight;
  struct trans  trans, trans1;

  if (pos > FSA->Q_init) return;

  if (iw >= 0)
    cur_weight.pos = iw, cur_weight.neg = 0;
  else
    cur_weight.neg = -iw, cur_weight.pos = 0;

  while ((car = *V++))
    SXBA_1_bit (working_char_set, car);
  
  if (++FSA->Q_nb >= fsa_state2equiv_size) {
    fsa_state2equiv_size *= 2;
    fsa_state2lequiv = (SXINT*) sxrealloc (fsa_state2lequiv, fsa_state2equiv_size+1, sizeof (SXINT));
    fsa_state2requiv = (SXINT*) sxrealloc (fsa_state2requiv, fsa_state2equiv_size+1, sizeof (SXINT));
    fsa_state2local_path = (SXINT*) sxrealloc (fsa_state2local_path, fsa_state2equiv_size+1, sizeof (SXINT));
  }

  /* FSA->Q_nb == etat intermediaire q */
  /* On cree pos -> etat intermediaire -> q */
  fsa_state2lequiv [FSA->Q_nb] = pos;

  if (pos < FSA->Q_init)
    fsa_state2requiv [FSA->Q_nb] = pos+1;
  else
    /* FSA->Q_nb est final */
    fsa_state2requiv [FSA->Q_nb] = FSA->Q_nb;

  local_path_nb++;
  fsa_state2local_path [FSA->Q_nb] = local_path_nb;

  XxY_set (&(FSA->Q_hd), pos, FSA->Q_nb, &pair); /* On ajoute la transition (pos, etat intermediaire) ... */
  /* pair est nouveau */
  trans = AREA2TRANS (FSA, pair);
  new_char_set = trans.char_set;
  new_char_weight = trans.char_weight;

  sxba_copy (new_char_set, working_char_set);

  if (iw != 0) {
    car = 0;
 
    while ((car = sxba_scan_reset (working_char_set, car)) >= 0) {
      new_char_weight [car] = cur_weight;
    }
  }

  FSA->final_set = sxba_resize (FSA->final_set, FSA->Q_nb+1);

  if (pos < FSA->Q_init) {
    /* On recopie (pos, pos+1) ds (FSA->Q_nb, pos+1) */
    XxY_set (&(FSA->Q_hd), FSA->Q_nb, pos+1, &pair); /* On ajoute la transition (FSA->Q_nb, pos+1) ... */
    trans = AREA2TRANS (FSA, pair);
    new_char_set = trans.char_set;
    new_char_weight = trans.char_weight;

    trans1 = AREA2TRANS (FSA, pos);
    char_set = trans1.char_set;
    char_weight = trans1.char_weight;

    sxba_copy (new_char_set, char_set);
    SXBA_0_bit (new_char_set, 0); /* On ne copie pas l'epsilon eventuel */

    car = 0;
 
    while ((car = sxba_scan (new_char_set, car)) >= 0) {
      new_char_weight [car] = char_weight [car];
    }
  }
  else {
    SXBA_1_bit (FSA->final_set, FSA->Q_nb);
  }

  FSA->final_set = sxba_resize (FSA->final_set, FSA->Q_nb+1);
}
 

/* permet le remplacement du caracte`re en position pos par l'ensemble des char V de poids cw */
void
sxspell_change (struct FSA *FSA,
		SXINT pos,
		unsigned char *V,
		SXINT cw)
{
  SXINT           car;
  SXBA          char_set;
  struct weight cur_weight, *char_weight;
  struct trans  trans;

  if (pos < 1 || pos >= FSA->Q_init || strlen ((char *) V) == 0) return;

  if (cw >= 0)
    cur_weight.pos = cw, cur_weight.neg = 0;
  else
    cur_weight.neg = -cw, cur_weight.pos = 0;

  while ((car = *V++))
    SXBA_1_bit (working_char_set, car);

  /* On ne remplace pas le caractere en position pos par lui-meme */
  car = (SXINT) (unsigned char) FSA->word [pos-1];
  SXBA_0_bit (working_char_set, car);
      
  trans = AREA2TRANS (FSA, pos);
  char_set = trans.char_set;
  char_weight = trans.char_weight;

  car = -1;

  while ((car = sxba_scan_reset (working_char_set, car)) >= 0) {
    if (SXBA_bit_is_reset_set (char_set, car))
      /* nouveau */
      char_weight [car] = cur_weight;
    else
      /* On prend le min ... */
      char_weight [car] = get_new_min_weight (char_weight [car], cur_weight);
  }
}
     


static SXINT
spell_result_stack_push (SXINT id)
{
  SXINT old_size, new_size;

  old_size = DSIZE (spell_result_id_stack);
  DPUSH (spell_result_id_stack, id);

  if ((new_size = DSIZE (spell_result_id_stack)) > old_size)
    spell_result_weight_stack = (struct weight*) sxrealloc (spell_result_weight_stack, new_size+1, sizeof (struct weight));

  spell_result_id_stack [cur_spell_result_stack_bot]++;

  return DTOP (spell_result_id_stack);
}


#define WEIGHT_NB_SHIFT 24
#define ID_NB_AND       0XFFFFFF

#define spell_result_erase_bloc(x)   (DTOP (spell_result_id_stack) = (x)-1)

/* Le sous mot id de poids weight est stocke' ds spell_result_[id|word]_stack */
/* On stocke par poids non decroissants */
static void
store_sub_word (SXINT id, struct weight weight, bool is_an_old_id)
{
  SXINT           new_total_weight, old_total_weight, prev_total_weight, cur_total_weight, total_weighti;
  SXINT           id_nb, weight_nb, nb, i, j, insert_point, store_id;
  SXINT           *id_stack;
  struct weight *weight_stack, store_weight;
  bool       is_single, unique_insert;

  sxinitialise(weight_nb); /* pour faire taire "gcc -Wuninitialized" */
  /* Si is_an_old_id, on est ds le bloc principal et id a deja ete range'. Il ne doit en rester qu'un seul */

  new_total_weight = get_total_weight (weight);
  
  if (new_total_weight > cur_spell_result->weight_limit)
    sxtrap (ME, "store_sub_word");

  /* Attention id_stack et weight_stack ne peuvent pas etre utilises apres spell_result_stack_push */
  id_stack = spell_result_id_stack + cur_spell_result_stack_bot;
  id_nb = TOP (id_stack);
    
  if (id_nb == 0) {
    /* On stocke ... */
    if (cur_spell_result->kind == 1)
      /* BEST */
      cur_spell_result->weight_limit = new_total_weight;
    else {
      if (cur_spell_result->kind)
	TOP (id_stack) = 1 << WEIGHT_NB_SHIFT;
    }

    spell_result_weight_stack [spell_result_stack_push (id)] = weight;

    return;
  }
  
  weight_stack = spell_result_weight_stack + cur_spell_result_stack_bot;

  if (cur_spell_result->kind == 1) {
    /* BEST et id_nb > 0 */
    /* Les id_nb ont tous le meme poids */
    old_total_weight = get_total_weight (weight_stack [1]);

    if (old_total_weight >= new_total_weight) {
      /* Le nouveau est pas pire ... */
      if (old_total_weight > new_total_weight) {
	/* ... il est meme meilleur */
	cur_spell_result->weight_limit = new_total_weight;

	/* On vire tout et on met le nouveau ... */
	DTOP (spell_result_id_stack) -= id_nb;
	TOP (id_stack) = 0;
      }
      else {
	/* meme poids */
	if (is_an_old_id) {
	  /* On regarde s'il s'y trouve deja (avec le meme poids)
	     pour ne pas l'ajouter */
	  for (i = 1; i <= id_nb; i++) {
	    if (id_stack [i] == id)
	      break;
	  }

	  if (i <= id_nb)
	    return;
	}
      }

      /* On stocke ... */
      spell_result_weight_stack [spell_result_stack_push (id)] = weight;
    }

    return;
  }
 
  /* ALL ou NBEST */
  is_single = false;

  if (cur_spell_result->kind) {
    /* NBEST */
    weight_nb = ((SXUINT)id_nb) >> WEIGHT_NB_SHIFT;
    id_nb &= ID_NB_AND;
  }

  if (is_an_old_id) {
    /* On regarde si id s'y trouve deja avec un poids inferieur ou egal pour ne pas l'ajouter */
    for (i = 1; i <= id_nb; i++) {
      if (id_stack [i] == id) {
	/* Il s'y trouve deja */
	break;
      }
    }

    if (i <= id_nb) {
      if ((total_weighti = get_total_weight (weight_stack [i])) <= new_total_weight)
	/* ... avec un poids + faible ou egal, bye */
	return;

      if (cur_spell_result->kind) {
	/* cas NBEST */
	/* On regarde si c'est le seul de ce poids */
	is_single = !(((i > 1) && ((SXINT)get_total_weight (weight_stack [i-1]) == total_weighti))
		      || ((i < id_nb) && ((SXINT)get_total_weight (weight_stack [i+1]) == total_weighti)));

	if (is_single) {
	  if (weight_nb < cur_spell_result->kind)
	    /* pas plein */
	    is_single = false;

	  weight_nb--;
	  TOP (id_stack) -= 1 << WEIGHT_NB_SHIFT;
	}
      }

      /* ... bon ben on vire celui la */
      while (++i <= id_nb) {
	weight_stack [i-1] = weight_stack [i];
	id_stack [i-1] = id_stack [i];
      }

      id_nb--;

      /* ... et on recupere le sommet */
      DTOP (spell_result_id_stack) -= 1;
      TOP (id_stack) -= 1;

      if (id_nb == 0) {
	/* On stocke ... */
	if (cur_spell_result->kind == 1)
	  /* BEST */
	  cur_spell_result->weight_limit = new_total_weight;
	else {
	  if (cur_spell_result->kind)
	    TOP (id_stack) = 1 << WEIGHT_NB_SHIFT;
	}

	spell_result_weight_stack [spell_result_stack_push (id)] = weight;

	return;
      }
    }
  }

  /* Si is_single => NBEST && bloc plein && on vient de virer un "fantome" de poids p > weight qui etait la seule
     correction du bloc de ce poids p */

  /* Ici tout est correctement positionne', qu'on ait vire' un fantome ou pas ... */

  old_total_weight = get_total_weight (weight_stack [id_nb]);

  if (old_total_weight <= new_total_weight) {
    /* s'il y a insertion, c'est au sommet */
    if (cur_spell_result->kind == 0 || weight_nb < cur_spell_result->kind || old_total_weight == new_total_weight) {
      /* insertion */
      /* ALL || NBEST et pas plein || NBEST et plein et meme poids que le sommet */
      spell_result_weight_stack [spell_result_stack_push (id)] = weight;

      if (cur_spell_result->kind && (old_total_weight < new_total_weight)) {
	/* Un poids de plus */
	TOP (id_stack) += 1 << WEIGHT_NB_SHIFT;
	weight_nb++;

	if (weight_nb == cur_spell_result->kind)
	  cur_spell_result->weight_limit = new_total_weight;
      }

      if (is_single && (old_total_weight == new_total_weight)) {
	/* On est ds le cas NBEST problematique :
	   le bloc etait plein, on a vire' un fantome "unique" et son incarnation a des synonymes de meme poids */
	/* En fait on vient de creer une place vide ds le bloc qui ne sera pas utilisee par l'insertion courante et
	   qui aurait donc pu etre utilisee par des corrections precedentes !! */
	underflow_nb++;
      }

      return;
    }
  }

  /* ici, vraie insertion */
  /* on ne conserve que les cur_spell_result->kind meilleurs */
  prev_total_weight = -1;

  /* Recherche dichotomique */
  {
    SXINT bot, top, mid;

    bot = 1;
    top = id_nb;

    while (bot < top) {
      mid = (bot+top)>>1;

      if ((cur_total_weight = get_total_weight (weight_stack [mid])) <= new_total_weight) {
	bot = mid+1;
	prev_total_weight = cur_total_weight;
      }
      else
	top = mid;
    }

    insert_point = bot;
  }

  if (insert_point == 0 ||
      prev_total_weight > new_total_weight || new_total_weight >= old_total_weight)
    sxtrap (ME, "store_sub_word");

  unique_insert = (prev_total_weight < new_total_weight);

  if (is_single && !unique_insert) {
    /* On est ds le cas NBEST problematique :
       le bloc etait plein, on a vire' un fantome "unique" et son incarnation a des synonymes de meme poids */
    /* En fait on vient de creer une place vide ds le bloc qui ne sera pas utilisee par l'insertion courante et
       qui aurait donc pu etre utilisee par des corrections precedentes !! */
    underflow_nb++;
  }

  if (cur_spell_result->kind && weight_nb == cur_spell_result->kind && unique_insert) {
    /* Plein et l'insertion est un nouveau poids */
    /* On vire le[s] sommet[s] */
    /* weight_nb est correct */
    nb = 0;

    do {
      id_nb--;
      nb++;
    } while ((SXINT)get_total_weight (weight_stack [id_nb]) == old_total_weight);

    if (id_nb >= insert_point) {
      /* On deplace le suffixe */
      for (j = id_nb; j >= insert_point; j--) {
	id_stack [j+1] = id_stack [j];
	weight_stack [j+1] = weight_stack [j];
      }

      id_nb++;
    }
    else
      id_nb = insert_point;

    /* ... et on insere en insert_point */
    id_stack [insert_point] = id;
    weight_stack [insert_point] = weight;
    nb--; /* bilan des suppression/insertion */

    if (nb) {
      TOP (id_stack) -= nb;
      DTOP (spell_result_id_stack) -= nb;
    }

    /* Le bloc est plein */
    /* Nouveau poids du sommet */
    cur_spell_result->weight_limit = get_total_weight (weight_stack [id_nb]);
  }
  else {
    /* On deplace le suffixe de 1 */
    /* On se souvient du sommet (on ne peut pas utiliser spell_result_stack_push) */
    store_id = id_stack [id_nb];
    store_weight = weight_stack [id_nb];

    for (j = id_nb-1; j >= insert_point; j--) {
      id_stack [j+1] = id_stack [j];
      weight_stack [j+1] = weight_stack [j];
    }

    /* ... et on insere en insert_point */
    id_stack [insert_point] = id;
    weight_stack [insert_point] = weight;

    if (cur_spell_result->kind) {
      if (unique_insert) {
	TOP (id_stack) += 1 << WEIGHT_NB_SHIFT;
	weight_nb++;
      }

      if (weight_nb == cur_spell_result->kind)
	cur_spell_result->weight_limit = old_total_weight;
    }

    /* ... et on remet le sommet */
    spell_result_weight_stack [spell_result_stack_push (store_id)] = store_weight;
  }
}


/* un mot corrige' se trouve ds lvstr, rvstr : on le stocke ds spell_result */
void store_word (SXINT code)
{
  SXINT  id, rsize;
  SXINT top;
  SXINT  cur_total_weight;

  cur_total_weight = get_total_weight (total_weight);

  if (cur_total_weight > cur_spell_result->weight_limit) {
    /* trop couteux */
    return;
  }

  top = SXWORD_top (cur_spell_result->words);

  /* On concatene rvstr ds lvstr mais en sens inverse */
  rsize = varstr_length (rvstr);

  lvstr = varstr_rcatenate (lvstr, varstr_tostr (rvstr), rsize);

  id = sxword_2save (&(cur_spell_result->words), varstr_tostr (lvstr), (SXUINT)varstr_length (lvstr));

  /* On remet lvstr comme avant */
  varstr_pop (lvstr, rsize);

  cur_spell_result->codes [id] = code;

  store_sub_word (id, total_weight, (bool) (SXWORD_top (cur_spell_result->words) <= top));
}


static struct weight
get_weight (struct weight weight1, struct weight weight2)
{
    struct weight weight;

    weight.neg = weight1.neg + weight2.neg;
    weight.pos = weight1.pos + weight2.pos;

    if (weight1.pos > 0 && weight2.pos > 0)
	weight.pos += cur_spell_result->cmp_weight;

    return weight;
}

/* Les chemins gauche-droit et droit-gauche se sont-ils rejoints ? */
static bool
FSA_walk_done (SXINT p, SXINT q)
{
#if 0
  return (q == 0 /* Jusqu'a present, recherche ds le sens gauche-droit */ && SXBA_bit_is_set (FSA->final_set, p) /* etat final ... */
	  || p == q);
#endif /* 0 */
  return (p == q);
}


/* Y a t'il un chemin vide entre p et q */
static void
FSA_walk_eps_trans (SXINT p, SXINT q, SXINT code)
{
  SXINT           pair;
  struct weight old_total_weight;
  struct trans  trans;

#if EBUG
  if (q == 0)
    sxtrap (ME, "FSA_walk_eps_trans");
#endif /* EBUG */

  if (FSA_walk_done (p, q)) {
    store_word (code);
    return;
  }

  if (is_suffix) {
    XxY_Yforeach (cur_FSA->Q_hd, q, pair) {
      trans = AREA2TRANS (cur_FSA, pair);
	  
      if (SXBA_bit_is_set (trans.char_set, 0)) {
	old_total_weight = total_weight;
	total_weight = get_weight (total_weight, trans.char_weight [0]);

	if ((SXINT)get_total_weight (total_weight) <= cur_spell_result->weight_limit)
	  FSA_walk_eps_trans (p, XxY_X (cur_FSA->Q_hd, pair), code);

	total_weight = old_total_weight;
      }
    }
  }
  else {
    XxY_Xforeach (cur_FSA->Q_hd, p, pair) {
      trans = AREA2TRANS (cur_FSA, pair);
	  
      if (SXBA_bit_is_set (trans.char_set, 0)) {
	old_total_weight = total_weight;
	total_weight = get_weight (total_weight, trans.char_weight [0]);

	if ((SXINT)get_total_weight (total_weight) <= cur_spell_result->weight_limit)
	  FSA_walk_eps_trans (XxY_Y (cur_FSA->Q_hd, pair), q, code);

	total_weight = old_total_weight;
      }
    }
  }
}


static void dico_walk (unsigned char cur_char,
		       struct weight cur_weight,
		       SXINT p, 
		       SXINT q);

/* Parcourt le FSA ds l'ordre direct */

static SXINT dico_process_base (SXINT p, SXINT q)
{
  /* on est sur une base */
  SXUINT *comb_vector_ptr = comb_vector + base;
  tooth = *comb_vector_ptr;

  if (FSA_walk_done (p, q)) {
    if ((tooth & class_mask) == 0 && (tooth & stop_mask)) {
      /* ... et le mot est ds le dico */
      if (last_final_id)
	store_word (last_final_id);
      else
	store_word (((SXINT) tooth) >> base_shift);
    }
    
    return 1;
  }
  

  /* ici, !FSA_walk_done (p, q) */
  if (tooth /* non vide */ && (tooth & class_mask) == 0 /* il est bien � nous ;) */) {
    if (tooth & stop_mask) {
      /* Ici, un prefixe strict de l'entree est un mot du dico */
      /* On traite la trans suivante qui peut etre l'id du mot unique reconnu par le suffixe */
      tooth = comb_vector_ptr [1];
      
      if ((tooth & class_mask) == 1) {
	last_final_id = tooth >> base_shift;
      }
    }
    else {
      /* on ne devrait pas arriver ici, car �a voudrait dire que la transition-id n'est pas
	 remont�e au maximum... on laisse just in case ;) */
      last_final_id = tooth >> base_shift;
    }
  }

  return 0;
}


static void
FSA_walk (SXINT p, SXINT q)
{
  SXINT           cur_char, pair, r;
  //  SXUINT  new_tooth;
  struct weight *char_weight;
  SXBA          char_set;
  struct trans  trans;
  unsigned char source_char, char_stack_size, *char_stack_ptr, offset;
  bool       p_or_q_in_word;

#if EBUG
  if (q == 0)
    sxtrap (ME, "FSA_walk");
#endif /* EBUG */

#if 0
  if (FSA_walk_done (p, q)) {
    /* On a reconnu un mot complet */
    new_tooth = comb_vector [base];

    if ((new_tooth & class_mask) == 0 && (new_tooth & stop_mask)) {
      /* ... et le mot est ds le dico */
      store_word (((SXINT) new_tooth) >> base_shift);
    }
	
    return;
  }
#endif /* 0 ... remplac� par: */
  if (dico_process_base (p, q)) /* si on est arriv� au bout du FSA (au cours de ce test, on store le
				   word s'il est dans le dico, on stocke le last_final_id le cas
				   �ch�ant) */
    return;
    
  /* On commence par regarder les transitions immediates "sur le mot" */

  if (is_suffix) {
    if ((p_or_q_in_word = (q <= cur_FSA->Q_init && q > 1))) {
      /* "q" est ds le mot initial */
      r = pair = q-1;

      if (FSA_has_path (p, r)) {
	trans = AREA2TRANS (cur_FSA, pair);
	char_set = trans.char_set;
	char_weight = trans.char_weight;

	/* Le caractere du source, on essaie en priorite une transition sur lui (cout nul) */
	source_char = (unsigned char) cur_FSA->word [r-1];
	dico_walk (source_char, NUL_WEIGHT, p, r);
	char_stack_ptr = dico_base2char_stack_ptr (cur_dico, base);

	if (char_stack_ptr == NULL) { /* si on ne dispose pas des tables char_stack_ptr associees au
					 dico courant cur_dico... */
	  cur_char = -1;
	  
	  while ((cur_char = sxba_scan (char_set, cur_char)) >= 0) {
	    if ((unsigned char) cur_char != source_char)
	      dico_walk ((unsigned char) cur_char, char_weight [cur_char], p, r);
	  }
	} 
	else { /* on a les tables char_stack_ptr, on les exploite (rappel: (unsigned char *)1 veut dire
		  qu'aucune transition (non-epsilon) partant d'ici n'existe dans le dico) */
	  /* essayer la transition epsilon */
	  if (SXBA_bit_is_set (char_set, '\0'))
	    dico_walk ((unsigned char) '\0', char_weight [0], p, r);
	  
	  /* transitions non-epsilon ? */
	  if (char_stack_ptr > (unsigned char *)1) {
	    char_stack_size = char_stack_ptr [0];
	    for (offset = 1; offset <= char_stack_size; offset++) {
	      cur_char = char_stack_ptr [offset];
	      if (cur_char != source_char && SXBA_bit_is_set (char_set, cur_char))
		dico_walk ((unsigned char) cur_char, char_weight [cur_char], p, r);
	    }
	  }
	}
      }
    }

    XxY_Yforeach (cur_FSA->Q_hd, q, pair) {
      r = XxY_X (cur_FSA->Q_hd, pair);

      if (FSA_has_path (p, r) && (!p_or_q_in_word || r != q-1)) {
	trans = AREA2TRANS (cur_FSA, pair);
	char_set = trans.char_set;
	char_weight = trans.char_weight;
	char_stack_ptr = dico_base2char_stack_ptr (cur_dico, base);

	if (char_stack_ptr == NULL) { /* si on ne dispose pas des tables char_stack_ptr associees au
					 dico courant cur_dico... */
	  cur_char = -1;
	  
	  while ((cur_char = sxba_scan (char_set, cur_char)) >= 0) {
	    dico_walk ((unsigned char) cur_char, char_weight [cur_char], p, r);
	  }
	}
	else { /* on a les tables char_stack_ptr, on les exploite (rappel: (unsigned char *)1 veut dire
		  qu'aucune transition (non-epsilon) partant d'ici n'existe dans le dico) */
	  /* essayer la transition epsilon */
	  if (SXBA_bit_is_set (char_set, '\0'))
	    dico_walk ((unsigned char) '\0', char_weight [0], p, r);
	  
	  /* transitions non-epsilon ? */
	  if (char_stack_ptr > (unsigned char *)1) {
	    char_stack_size = char_stack_ptr [0];
	    for (offset = 1; offset <= char_stack_size; offset++) {
	      cur_char = char_stack_ptr [offset];
	      if (SXBA_bit_is_set (char_set, cur_char))
		dico_walk ((unsigned char) cur_char, char_weight [cur_char], p, r);
	    }
	  }
	}
      }
    }
  }
  else {
    if ((p_or_q_in_word = (p < cur_FSA->Q_init))) {
      r = p+1;

      if (FSA_has_path (r, q)) {
	pair = p;
	trans = AREA2TRANS (cur_FSA, pair);
	char_set = trans.char_set;
	char_weight = trans.char_weight;

	/* Le caractere du source, on essaie en priorite une transition sur lui (cout nul) */
	source_char = (unsigned char) cur_FSA->word [p-1];
	dico_walk (source_char, NUL_WEIGHT, r, q);
	char_stack_ptr = dico_base2char_stack_ptr (cur_dico, base); // BS: ne marche pas si on n'a pas donn� -comb 2 � make_a_dico pour construire cur_dico

	if (char_stack_ptr == NULL) { /* si on ne dispose pas des tables char_stack_ptr associees au
					 dico courant cur_dico... */
	  cur_char = -1;
	  
	  while ((cur_char = sxba_scan (char_set, cur_char)) >= 0) {
	    if ((unsigned char) cur_char != source_char)
	      dico_walk ((unsigned char) cur_char, char_weight [cur_char], r, q);
	  }
	}
	else { /* on a les tables char_stack_ptr, on les exploite (rappel: (unsigned char *)1 veut dire
		  qu'aucune transition (non-epsilon) partant d'ici n'existe dans le dico) */
	  /* essayer la transition epsilon */
	  if (SXBA_bit_is_set (char_set, '\0'))
	    dico_walk ((unsigned char) '\0', char_weight [0], r, q);
	  
	  /* transitions non-epsilon ? */
	  if (char_stack_ptr > (unsigned char *)1) {
	    char_stack_size = char_stack_ptr [0];
	    for (offset = 1; offset <= char_stack_size; offset++) {
	      cur_char = char_stack_ptr [offset];
	      if (cur_char != source_char && SXBA_bit_is_set (char_set, cur_char))
		dico_walk ((unsigned char) cur_char, char_weight [cur_char], r, q);
	    }
	  }
	}
      }
    }

    XxY_Xforeach (cur_FSA->Q_hd, p, pair) {
      r = XxY_Y (cur_FSA->Q_hd, pair);

      if (FSA_has_path (r, q) && (!p_or_q_in_word || r != p+1)) {
	trans = AREA2TRANS (cur_FSA, pair);
	char_set = trans.char_set;
	char_weight = trans.char_weight;
	char_stack_ptr = dico_base2char_stack_ptr (cur_dico, base);

	if (char_stack_ptr == NULL) { /* si on ne dispose pas des tables char_stack_ptr associees au
					 dico courant cur_dico... */
	  cur_char = -1;
	  
	  while ((cur_char = sxba_scan (char_set, cur_char)) >= 0) {
	    dico_walk ((unsigned char) cur_char, char_weight [cur_char], r, q);
	  }
	}
	else { /* on a les tables char_stack_ptr, on les exploite (rappel: (unsigned char *)1 veut dire
		  qu'aucune transition (non-epsilon) partant d'ici n'existe dans le dico) */
	  /* essayer la transition epsilon */
	  if (SXBA_bit_is_set (char_set, '\0'))
	    dico_walk ((unsigned char) '\0', char_weight [0], r, q);
	  
	  /* transitions non-epsilon ? */
	  if (char_stack_ptr > (unsigned char *)1) {
	    char_stack_size = char_stack_ptr [0];
	    for (offset = 1; offset <= char_stack_size; offset++) {
	      cur_char = char_stack_ptr [offset];
	      if (SXBA_bit_is_set (char_set, cur_char))
		dico_walk ((unsigned char) cur_char, char_weight [cur_char], r, q);
	    }
	  }
	}
      }
    }
  }
}



static void
call_FSA_walk (SXINT p, SXINT q)
{
  if (q != 0)
    FSA_walk (p, q);

  while ((q = sxba_scan (cur_FSA->final_set, q)) > 0)
    FSA_walk (p, q);
}



static void
dico_walk (unsigned char cur_char,
	   struct weight cur_weight,
	   SXINT p, 
	   SXINT q)
{
  struct weight old_total_weight;
  SXINT           cur_total_weight;
  SXUINT	old_tooth, new_tooth, old_base;
  // old_is_suffix
  unsigned char	class;
  bool       FSA_walk_complete, stop_mask_hit;

  if (cur_char == SXNUL) {
    /* transition sur epsilon */
    old_total_weight = total_weight;
    total_weight = get_weight (total_weight, cur_weight);
    cur_total_weight = get_total_weight (total_weight);

    if (cur_total_weight <= cur_spell_result->weight_limit)
      FSA_walk (p, q);

    total_weight = old_total_weight;

    return;
  }

  if ((class = char2class [cur_char])) { /* class est non nul */
    new_tooth = comb_vector [base+class];

    if ((new_tooth & class_mask) == class) { /* et il est bon */
      old_total_weight = total_weight;
      total_weight = get_weight (total_weight, cur_weight);
      cur_total_weight = get_total_weight (total_weight);

      if (cur_total_weight <= cur_spell_result->weight_limit) {
	if (is_suffix)
	  varstr_catchar (rvstr, (char) cur_char);
	else
	  varstr_catchar (lvstr, (char) cur_char);

	FSA_walk_complete = FSA_walk_done (p, q);
	stop_mask_hit = ((new_tooth & stop_mask) != 0);

	if (stop_mask_hit || FSA_walk_complete) {
	  if (stop_mask_hit) {
	    /* ... et le mot est l� ds le dico, et n'est le pr�fixe d'aucun autre mot du dico */
	    if (FSA_walk_complete) {
	      if (last_final_id)
		store_word (last_final_id);
	      else
		store_word ((((SXINT) new_tooth) >> base_shift) | 1);
	    } else {
	      /* Il est possible qu'il ne reste que des transitions vides entre p et q */
	      if (FSA_HAS_EPS_TRANS) {
		if (last_final_id)
		  FSA_walk_eps_trans (p, q, last_final_id);
		else
		  FSA_walk_eps_trans (p, q, (((SXINT) new_tooth) >> base_shift) | 1);
	      }
	    }
	  }
	  else {
	    /* ... mais ca peut etre aussi un prefixe d'autres mots */
	    new_tooth = comb_vector [new_tooth >> base_shift];

	    if ((new_tooth & class_mask) == 0 && (new_tooth & stop_mask) != 0)
	      /* ... oui */
	      store_word (((SXINT) new_tooth) >> base_shift);
	  }
	}
	else {
	  old_tooth = tooth;
	  // old_is_suffix = is_suffix;
	  old_base = base;

	  tooth = new_tooth;
	  // is_suffix = tooth & suffix_mask;
	  base = tooth >> base_shift;

	  FSA_walk (p, q);

	  tooth = old_tooth;
	  // is_suffix = old_is_suffix;
	  base = old_base;
	}

	if (is_suffix)
	  varstr_pop (rvstr, 1);
	else
	  varstr_pop (lvstr, 1);
      }

      total_weight = old_total_weight;
    }
  }
}

/* for qsort */
/* void qsort(void *base, size_t nmemb, size_t size, int(*compar)(const void *, const void *)); */
/* The  qsort()  function  sorts an array with nmemb elements of size size.
   The base argument points to the start of the array.

   The contents of the array are sorted in ascending order according to a comparison function pointed to  by  compar,
   which is called with two arguments that point to the objects being compared.

   The  comparison function must return an integer less than, equal to, or greater than zero if the first argument is
   considered to be respectively less than, equal to, or greater than the second.  If two members compare  as  equal,
   their order in the sorted array is undefined. */

static int
merge_compare (const SXINT *p1, const SXINT *p2)
{
  return (ij_total_weights [*p1]) - (ij_total_weights [*p2]);
}

/* Ds spell_result_stack, on fusionne les sous piles d'indices x et y ds top.
   Les sous-piles top, x et y sont consecutives et forment le suffixe
   De plus chacune est ordonnee par poids non decroissants*/
static void
merge_spell_result_stack (SXINT top, SXINT x, SXINT y, SXINT additional_cost)
{
  SXINT           top_size, x_size, y_size, size, i, top_nb, nb, greatest_weight; 
  SXINT           ij_total_weight, top_total_weight, min_total_weight, x_id, y_id, xy_top, cur_bot, cur_top, cur_id; 
  SXINT           *int_ptr;
  struct weight *x_weights, *y_weights, *top_weights, ij_weight, weighti, weightj, top_weight;
  bool       done, from_top;
  SXINT           sorted_top, sorted_nb, merge_spell_result_stack_base, j, ij, top_id, cross_size, old_size, new_size, xij, min_cross_weight;
  SXINT           *x_ids, *y_ids, *top_ids;

  sxinitialise_struct ( &top_weight, sizeof(struct weight) ); /* pour faire taire "gcc -Wuninitialized" */
  x_size = spell_result_id_stack [x] & ID_NB_AND;
  y_size = spell_result_id_stack [y] & ID_NB_AND;

  if (x_size == 0 || y_size == 0 ||
      (min_cross_weight = get_total_weight (get_weight (spell_result_weight_stack [x+1],
							spell_result_weight_stack [y+1]))) > cur_spell_result->weight_limit) {
    /* Pas de croisement */
    /* On recupere la place ds spell_result_id_stack ... */
    DTOP (spell_result_id_stack) -= x_size+y_size+2;
    
    return;
  }

  top_size = spell_result_id_stack [top] & ID_NB_AND; /* Lui peut etre nul ... */

#if EBUG
  if (x != top+top_size+1 || y != x+x_size+1)
    sxtrap (ME, "merge_spell_result_stack");
#endif /* EBUG */

  if (top_size > 0 && cur_spell_result->kind > 0 && (((SXUINT)spell_result_id_stack [top]) >> WEIGHT_NB_SHIFT) == (SXUINT)(cur_spell_result->kind)) {
    /* Y'a deja des resultats pour top ... */
    /* ... et BEST ou NBEST ...*/
    /* ... et plein */
    if ((SXINT)get_total_weight (spell_result_weight_stack [top+top_size]) < min_cross_weight) {
      /* Le croisement ne sera pas productif */
      /* On recupere la place ds spell_result_id_stack ... */
      DTOP (spell_result_id_stack) -= x_size+y_size+2;
    
      return;
    }
  }

  /* majorant de la taille du croisement */
  size = top_size + x_size*y_size;

  if (cross_id == NULL) {
    cross_id = (SXINT*) sxalloc ((cross_id_size = 128+size)+1, sizeof (SXINT));
    cross_weight = (struct weight*) sxalloc (cross_id_size+1, sizeof (struct weight));
  }
  else {
    if (size > cross_id_size) {
      cross_id = (SXINT*) sxrealloc (cross_id, (cross_id_size += size)+1, sizeof (SXINT));
      cross_weight = (struct weight*) sxrealloc (cross_weight, cross_id_size+1, sizeof (struct weight));
    }
  }

  RAZ (cross_id);

  /* On peut croiser */
  merge_spell_result_stack_base = (x_size >= y_size) ? x_size : y_size;

  x_weights = spell_result_weight_stack + x + 1;
  y_weights = spell_result_weight_stack + y + 1;

  sorted_top = 0;

  if (sorted == NULL) {
    sorted_size = x_size*y_size;
    sorted = (SXINT*) sxalloc (sorted_size, sizeof (SXINT));
    ij_weights = (struct weight*) sxalloc (sorted_size, sizeof (struct weight));
    ij_total_weights = (SXINT*) sxalloc (sorted_size, sizeof (SXINT));
    ij_ids = (SXINT*) sxalloc (sorted_size, sizeof (SXINT));
  }
  else {
    if ((i = x_size*y_size) > sorted_size) {
      sorted_size = i;
      sorted = (SXINT*) sxrealloc (sorted, sorted_size, sizeof (SXINT));
      ij_weights = (struct weight*) sxrealloc (ij_weights, sorted_size, sizeof (struct weight));
      ij_total_weights = (SXINT*) sxrealloc (ij_total_weights, sorted_size, sizeof (SXINT));
      ij_ids = (SXINT*) sxrealloc (ij_ids, sorted_size, sizeof (SXINT));
    }
  }

  for (i = 0; i < x_size; i++) {
    weighti = x_weights [i];

    for (j = 0; j < y_size; j++) {
      weightj = y_weights [j];
      ij_weight = get_weight (weighti, weightj);
      //      ij_weight = get_weight (get_weight (weighti, weightj), cur_spell_result->sub_word_weight);
      ij_total_weight = get_total_weight (ij_weight);

      if (ij_total_weight > cur_spell_result->weight_limit)
	break;

      ij_total_weights [sorted_top] = ij_total_weight + additional_cost;
      ij_weights [sorted_top] = ij_weight;
      ij_weights [sorted_top].pos += additional_cost;
      ij_ids [sorted_top] = i*merge_spell_result_stack_base + j;
      sorted [sorted_top] = sorted_top;
      sorted_top++;
    }

    if (j == 0)
      break;
  }

  if (sorted_top > 1)
    qsort (sorted, (size_t)sorted_top, sizeof (SXINT), (int (*) (const void *, const void *)) merge_compare);

  top_weights = spell_result_weight_stack + top + 1;

  x_ids = spell_result_id_stack + x + 1;
  y_ids = spell_result_id_stack + y + 1;
  top_ids = spell_result_id_stack + top + 1;
  
  done = false;
  top_nb = sorted_nb = 0;
  nb = 0; /* nb d'items de poids differents ds cross_weight.  On doit avoir nb <= cur_spell_result->kind */
  greatest_weight = -1;
  sxinitialise (xij); /* pour faire taire "gcc -Wuninitialized" */

  while (!done) {
    if (sorted_nb < sorted_top) {
      xij = sorted [sorted_nb];
      ij_total_weight = ij_total_weights [xij];
    }
    else
      ij_total_weight = -1;

    if (top_nb < top_size) {
      top_weight = top_weights [top_nb];
      top_total_weight = get_total_weight (top_weight);
      min_total_weight = (ij_total_weight == -1)
	? (from_top = true, top_total_weight) :
	((ij_total_weight < top_total_weight) ? (from_top = false, ij_total_weight) : (from_top = true, top_total_weight));
    }
    else {
      min_total_weight = ij_total_weight;
      from_top = false;
    }

    if (min_total_weight >= 0) {
      /* Doit-on le ranger ? */
      if (cur_spell_result->kind == 0 /* All */
	   || greatest_weight == min_total_weight /* egal au precedent */
	   || nb < cur_spell_result->kind /* pas plein */) {
	if (greatest_weight < min_total_weight) {
	  /* Nouveau poids */
	  nb++;
	  greatest_weight = min_total_weight;
	}

	if (from_top) {
	  /* Le min_total_weight est top_nb */
	  top_id = top_ids [top_nb];
	  PUSH (cross_id, top_id);
	  cross_weight [TOP (cross_id)] = top_weight;

	  top_nb++;
	}
	else {
	  ij = ij_ids [xij];
	  i = ij/merge_spell_result_stack_base;
	  j = ij%merge_spell_result_stack_base;
	  x_id = x_ids [i];
	  y_id = y_ids [j];

	  /* Le couple (id, weight) est stockable */
	  /* on fusionne x_id et y_id */
	  if (x_id > 0) {
	    XH_push (*cur_multiple_word_ids_ptr, x_id);
	  }
	  else {
	    cur_top = XH_X (*cur_multiple_word_ids_ptr, -x_id+1);
	    cur_bot = XH_X (*cur_multiple_word_ids_ptr, -x_id);

	    while (cur_bot < cur_top) {
	      cur_id = XH_list_elem (*cur_multiple_word_ids_ptr, cur_bot++);
	      XH_push (*cur_multiple_word_ids_ptr, cur_id);
	    }
	  }

	  if (y_id > 0) {
	    XH_push (*cur_multiple_word_ids_ptr, y_id);
	  }
	  else {
	    cur_top = XH_X (*cur_multiple_word_ids_ptr, -y_id+1);
	    cur_bot = XH_X (*cur_multiple_word_ids_ptr, -y_id);

	    while (cur_bot < cur_top) {
	      cur_id = XH_list_elem (*cur_multiple_word_ids_ptr, cur_bot++);
	      XH_push (*cur_multiple_word_ids_ptr, cur_id);
	    }
	  }

	  if (XH_set (cur_multiple_word_ids_ptr, &xy_top)) {
	    /* On verifie qu'il n'existe pas deja ds cross_id ... */
	    int_ptr = cross_id+cross_id[0];

	    while (int_ptr > cross_id) {
	      if (*int_ptr == -xy_top)
		break;

	      int_ptr--;
	    }

	    if (int_ptr == cross_id) {
	      /* ... non */
	      PUSH (cross_id, -xy_top);
	      cross_weight [TOP (cross_id)] = ij_weights [xij];
	    }
	  }
	  else {
	    PUSH (cross_id, -xy_top);
	    cross_weight [TOP (cross_id)] = ij_weights [xij];
	  }

	  sorted_nb++;
	}
      }
      else
	done = true;
    }
    else
      done = true;
  }

  /* ... et on recopie de cross ds spell_result_[id|weight]_stack */

  /* On recupere la place ds spell_result_id_stack ... */
  DTOP (spell_result_id_stack) -= x_size+y_size+top_size+2;
  /* sous-pile vide */

#if EBUG
  if (top != spell_result_id_stack [0])
    sxtrap (ME, "merge_spell_result_stack");
#endif /* EBUG */

  spell_result_id_stack [top] = cross_size = TOP (cross_id);
  
  /* et on reserve ... */
  old_size = DSIZE (spell_result_id_stack);
  DCHECK (spell_result_id_stack, cross_size);

  if ((new_size = DSIZE (spell_result_id_stack)) > old_size)
    spell_result_weight_stack = (struct weight*) sxrealloc (spell_result_weight_stack, new_size+1, sizeof (struct weight));

  for (i = 1; i <= cross_size; i++) {
    DSPUSH (spell_result_id_stack, cross_id [i]);
    spell_result_weight_stack [DTOP (spell_result_id_stack)] = cross_weight [i];
  }

  if (cur_spell_result->kind > 0 && nb >= cur_spell_result->kind)
    /* BEST ou NBEST et plein */
    cur_spell_result->weight_limit = greatest_weight;

  if (cur_spell_result->kind > 1)
    /* NBEST */
    spell_result_id_stack [top] += nb << WEIGHT_NB_SHIFT;

#if EBUG
  if ((top + (spell_result_id_stack [top] & ID_NB_AND)) != (spell_result_id_stack [0] & ID_NB_AND))
    sxtrap (ME, "merge_spell_result_stack");
#endif /* EBUG */
}


/* On regarde si ce qui a ete memoise' en x est valide vis-a-vis du cur_spell_result->weight_limit courant */
static bool
memo_is_valid (SXINT x)
{
  SXINT           weight_nb;

  weight_nb = ((SXUINT)memo_id_stack [x]) >> WEIGHT_NB_SHIFT;

  if (cur_spell_result->kind == 1 /* best */ || ((cur_spell_result->kind > 1) && (cur_spell_result->kind == weight_nb)) /* nbest */)
    /* Plein => OK */
    return true;

  return memo_weight_limit [x] >= cur_spell_result->weight_limit;
}

/* On met en spell_result_[id|weight]_stack [top] ce qui a etre memoize en x */
/* on tient compte de cur_spell_result->weight_limit */
static void
fill_spell_result_stacks (SXINT top, SXINT x)
{
  SXINT           i, nb, old_size, new_size, weight_nb, top_total_weight, prev_top_total_weight;
  SXINT           *mid_stack, *id_stack;
  struct weight *mweight_stack, *weight_stack, *top_mweight_stack;

  mid_stack = memo_id_stack + x;
  mweight_stack = memo_weight_stack + x;
  nb = mid_stack [0] & ID_NB_AND;

#if 0
  /* Voir commentaire en tete de memo_cut_in_subwords */
  if (cur_spell_result->weight_limit > mid_stack [nb+1])
    sxtrap (ME, "fill_spell_result_stacks");
#endif /* 0 */

  top_mweight_stack = mweight_stack+nb;

  weight_nb = ((SXUINT)mid_stack [0]) >> WEIGHT_NB_SHIFT; /* Si cur_spell_result->kind==1 => == 0 */
  prev_top_total_weight = -1;

  while (top_mweight_stack > mweight_stack) {
    if ((top_total_weight = get_total_weight (*top_mweight_stack)) <= cur_spell_result->weight_limit)
      break;

    /* pas bon */
    nb--;
    top_mweight_stack--;

    if (cur_spell_result->kind != 1 && top_total_weight != prev_top_total_weight) {
      weight_nb--;
      prev_top_total_weight = top_total_weight;
    }
  }

  if (nb > 0) {
    old_size = DSIZE (spell_result_id_stack);
    DCHECK (spell_result_id_stack, nb);

    if ((new_size = DSIZE (spell_result_id_stack)) > old_size)
      spell_result_weight_stack = (struct weight*) sxrealloc (spell_result_weight_stack, new_size+1, sizeof (struct weight));
  }

  DTOP (spell_result_id_stack) = top+nb;
  id_stack = spell_result_id_stack+top;

  id_stack [0] = (weight_nb<<WEIGHT_NB_SHIFT) + nb;

  weight_stack = spell_result_weight_stack+top;

  for (i = 1; i <= nb; i++) {
    *++id_stack = *++mid_stack;
    *++weight_stack = *++mweight_stack;
  }

  if ((i = memo_weight_limit [x]) < cur_spell_result->weight_limit)
    cur_spell_result->weight_limit = i;
}

/* Ajoute ds memoize les contenus de spell_result_[id|weight]_stack [top] et retourne l'indice */
static SXINT
fill_memo_stacks (SXINT top)
{
  SXINT           i, x, nb, old_size, new_size;
  SXINT           *mid_stack, *id_stack;
  struct weight *mweight_stack, *weight_stack;

  /* On range spell_result_id_stack et spell_result_weight_stack */
  id_stack = spell_result_id_stack+top;
  weight_stack = spell_result_weight_stack+top;

  nb = id_stack [0] & ID_NB_AND;

  old_size = DSIZE (memo_id_stack);
  DCHECK (memo_id_stack, nb+1);

  if ((new_size = DSIZE (memo_id_stack)) > old_size) {
    memo_weight_limit = (SXINT*) sxrealloc (memo_weight_limit, new_size+1, sizeof (SXINT));
    memo_weight_stack = (struct weight*) sxrealloc (memo_weight_stack, new_size+1, sizeof (struct weight));
  }

  x = DTOP (memo_id_stack)+1;
  DTOP (memo_id_stack) += nb+1;
  mid_stack = memo_id_stack + x;
  mweight_stack = memo_weight_stack + x;
  mid_stack [0] = id_stack [0];
  /* Pour connaitre les conditions de la construction de memo */
  memo_weight_limit [x] = cur_spell_result->weight_limit;

  for (i = 1; i <= nb; i++) {
    *++mid_stack = *++id_stack;
    *++mweight_stack = *++weight_stack;
  }

  return x;
}

/* calcule le nb d'espaces internes entre p et q */
/* p < q et char[p] != ' ' */
static SXINT
get_space_nb (SXINT p, SXINT q)
{
  SXINT nb = 0;

  /* On saute les blancs du debut ... */
  while (p < q && cur_FSA->word [p-1] == ' ')
    p++;

  /* ... et de fin */
  while (p < q && cur_FSA->word [q-2] == ' ')
    q--;
  
  while (++p < q) {
    if (cur_FSA->word [p-1] == ' ') {
      while (++p < q && cur_FSA->word [p-1] == ' ');

      nb++;
    }
  }

  return nb;
}


/* On decoupe le sous-mot [p .. q] en cut_nb+1 morceaux */
/* version memoizee */
/* On ne finasse pas avec les weight_limit */
/* Le 10/11/04, j'essaie de finasser avec memo_is_valid () */
static SXINT
memo_cut_in_subwords (SXINT p, SXINT q, SXINT cut_nb)
{
  SXINT           old_size, new_size, top, i=0, j, x, y, k, key_id, costly_cut_nb, space_nb;
  SXINT           local_old_weight_limit, ultra_local_old_weight_limit, x_cut_total_weight, total_cut_weight=0;
  bool       is_a_subword;
  struct weight n_cut_weight, store_SUB_WORD_WEIGHT;

  if ((is_a_subword = (p > cur_FSA->q0 || q < cur_FSA->Q_init))) {
    /* Ce n'est pas l'appel sur tout le mot initial */
    old_size = DSIZE (spell_result_id_stack);
    DPUSH (spell_result_id_stack, 0);

    if ((new_size = DSIZE (spell_result_id_stack)) > old_size)
      spell_result_weight_stack = (struct weight*) sxrealloc (spell_result_weight_stack, new_size+1, sizeof (struct weight));

    top = DTOP (spell_result_id_stack);
  }
  else
    top = 1; /* bloc principal */

  if (cut_nb == 0) {
    do {
      if ((x = memo_hd [k = p*MEMO_BASE+q]) == 0 ||
	  !memo_is_valid (x)) {
	/* 1ere fois, ou ce qui a ete calcule l'a ete pour un weight_limit +petit (on a pu en oublier),
	   on [re]fait le calcul ... */
	tooth = cur_dico->init_base;
	is_suffix = !(cur_dico->private.from_left_to_right);
	last_final_id = 0;
	base = tooth >> base_shift;
	cur_spell_result_stack_bot = top;

	FSA_walk (p, q);

	/* ... et on memoize */
	x = memo_hd [k] = fill_memo_stacks (top);
      }
      else
	/* On utilise la memoization */
	fill_spell_result_stacks (top, x);

      if (q >= cur_FSA->Q_init)
	q = sxba_scan (cur_FSA->final_set, q);
      else
	q = -1;
    } while (q > 0);
  }
  else {
    if (top == 1 || !X_set (&key_hd, MAKE_KEY(cut_nb, p, q), &key_id)) {
      /* Le calcul n'est pas memoize' */
      /* On le lance  */
      local_old_weight_limit = cur_spell_result->weight_limit;

      if ((WORD_HAS_SPACES ? get_space_nb (p, q) : 0) == cut_nb) {
	/* On essaie en premier un decoupage sur les blancs, qui sont ici en nombre exactement cut_nb */
	i = p+1;

	while (cur_FSA->word [i-1] != ' ')
	  i++;

	x = memo_cut_in_subwords (p, i, (SXINT)0);

	if (x) {
	  /* Si la coupure se fait sur un espace, elle ne coute rien ... */
	  /* On saute les blancs */
	  j = i;

	  do {
	    j++;
	  } while (cur_FSA->word [j-1] == ' ');

	  /* Coupure de cout nul */
	  /* x_cut_total_weight est le cout minimal de la correction du sous-mot [p..i] */
	  x_cut_total_weight = get_total_weight (spell_result_weight_stack [x+1]);

	  if (x_cut_total_weight <= local_old_weight_limit) {
	    /* ... on a suffisamment de provisions pour tenter ce decoupage ... */
	    /* ... et on dispose au max de ... */
	    cur_spell_result->weight_limit = local_old_weight_limit-x_cut_total_weight;

	    y = memo_cut_in_subwords (j, q, cut_nb-1);

	    if (y) {
	      /* On remet le cout max pour le merge */
	      cur_spell_result->weight_limit = local_old_weight_limit;

	      /* Coupure sur un blanc : cout nul */
	      store_SUB_WORD_WEIGHT = cur_spell_result->sub_word_weight;
	      cur_spell_result->sub_word_weight = NUL_WEIGHT;

	      merge_spell_result_stack (top, x, y,
					is_a_subword ? 0 : total_cut_weight); /* on ne prend en compte
										 le co�t des
										 d�coupages qu'� la
										 racine, puisque c'est
										 l� qu'il est
										 pr�-calcul� */

	      cur_spell_result->sub_word_weight = store_SUB_WORD_WEIGHT;

	      /* weight_limit a pu s'ameliorer ... */
	      local_old_weight_limit = cur_spell_result->weight_limit;
	    }
	    else {
	      /* Il faut recuperer x */
	      spell_result_erase_bloc (x);
	    }
	  }
	  else {
	    /* Il faut recuperer x */
	    spell_result_erase_bloc (x);
	  }
	}
      }

      if (!is_a_subword) {
	/* tenir compte du cout obligatoire des cut futurs pour restreindre cur_spell_result->weight_limit */
	space_nb = WORD_HAS_SPACES ? get_space_nb (i, q) : 0;
	costly_cut_nb = cut_nb-space_nb;
	total_cut_weight = costly_cut_nb == 0 ? 0 :
	  get_total_weight (get_new_compose_weight (NUL_WEIGHT, cur_spell_result->sub_word_weight, costly_cut_nb));
	
	/* On dispose de ce poids pour corriger [p..i] */
	local_old_weight_limit -= total_cut_weight;	
      }

      for (i = p+1; i <= q-cut_nb; i++) {
	cur_spell_result->weight_limit = ultra_local_old_weight_limit = local_old_weight_limit;

	x = memo_cut_in_subwords (p, i, (SXINT)0);

	if (x) {
	  /* cout min du x + 1 cut */
	  /* Si la coupure se fait sur un espace, on en fait une qui ne coute rien ... */
	  if (WORD_HAS_SPACES && (cur_FSA->word [i-1] == ' ')) {
	    /* On les saute */
	    j = i;

	    do {
	      j++;
	    } while (cur_FSA->word [j-1] == ' ');

	    if (j <= q-cut_nb) {
	      /* Cout min de la correction [p..i] */
	      n_cut_weight = spell_result_weight_stack [x+1];

	      if (cut_nb > 1) {
		costly_cut_nb = cut_nb - get_space_nb (j, q) - 1;

		if (costly_cut_nb > 0)
		  n_cut_weight = get_new_compose_weight (n_cut_weight, cur_spell_result->sub_word_weight, costly_cut_nb);
	      }
	      
	      /* Cout de la correction [p..j] + cout des cuts previsiuonnels [j..q] */
	      total_cut_weight = get_total_weight (n_cut_weight);

	      if (total_cut_weight <= ultra_local_old_weight_limit) {
		/* ... on a suffisamment de provisions pour tenter ce decoupage ... */
		/* ... au cout max */
		cur_spell_result->weight_limit = ultra_local_old_weight_limit - total_cut_weight;

		y = memo_cut_in_subwords (j, q, cut_nb-1);

		if (y) {
		  /* On dispose de cet argent pour faire le merge */
		  cur_spell_result->weight_limit = ultra_local_old_weight_limit;

		  /* Coupure sur un blanc : cout nul */
		  store_SUB_WORD_WEIGHT = cur_spell_result->sub_word_weight;
		  cur_spell_result->sub_word_weight = NUL_WEIGHT;

		  merge_spell_result_stack (top, x, y,
					    is_a_subword ? 0 : total_cut_weight); /* on ne prend en compte
										     le co�t des
										     d�coupages qu'� la
										     racine, puisque c'est
										     l� qu'il est
										     pr�-calcul� */
		  cur_spell_result->sub_word_weight = store_SUB_WORD_WEIGHT;

		  /* weight_limit a pu s'ameliorer ... */
		  ultra_local_old_weight_limit = cur_spell_result->weight_limit;
		}
		else {
		  /* Il faut recuperer x */
		  spell_result_erase_bloc (x);
		}
	      }
	      else {
		/* Il faut recuperer x */
		spell_result_erase_bloc (x);
	      }

	      /* Il faut "reactiver" x qui a ete memoize' ... */
	      /* ... ici */
	      k = memo_hd [p*MEMO_BASE+i];
	      /* On calcule le nouvel x ... */
	      old_size = DSIZE (spell_result_id_stack);
	      DPUSH (spell_result_id_stack, 0);

	      if ((new_size = DSIZE (spell_result_id_stack)) > old_size)
		spell_result_weight_stack = (struct weight*) sxrealloc (spell_result_weight_stack, new_size+1, sizeof (struct weight));

	      x = DTOP (spell_result_id_stack);
	      /* ... et on utilise la memoization */
	      fill_spell_result_stacks (x, k);
	    }
	  }

#if 0
	  /* Cout de la correction [p..i] et de la coupure en i */
	  n_cut_weight = get_new_compose_weight (spell_result_weight_stack [x+1],
						 cur_spell_result->sub_word_weight,
						 (SXINT)1);

	  if ((costly_cut_nb = cut_nb - 1) > 0) {
	    if (WORD_HAS_SPACES)
	      costly_cut_nb -= get_space_nb (i, q);

	    if (costly_cut_nb > 0)
	      n_cut_weight = get_new_compose_weight (n_cut_weight, cur_spell_result->sub_word_weight, costly_cut_nb);
	  }

	  /* ... + cout previsionnel des decoupages suivants */
	  total_cut_weight = get_total_weight (n_cut_weight);

	  if (total_cut_weight <= ultra_local_old_weight_limit) {
	    /* ... on a suffisamment de provisions pour tenter ce decoupage ... */
	    /* ... au cout max */
	    cur_spell_result->weight_limit = ultra_local_old_weight_limit - total_cut_weight;
#endif /* 0 */
	    ultra_local_old_weight_limit -= get_total_weight (spell_result_weight_stack [x+1]);
	    cur_spell_result->weight_limit = ultra_local_old_weight_limit;

	    y = memo_cut_in_subwords (i, q, cut_nb-1);

	    if (y) {
	      /* On dispose de cet argent pour faire le merge */
	      cur_spell_result->weight_limit = local_old_weight_limit;

	      merge_spell_result_stack (top, x, y, 
					is_a_subword ? 0 : total_cut_weight /* on ne prend en compte
									       le co�t des
									       d�coupages qu'� la
									       racine, puisque c'est
									       l� qu'il est
									       pr�-calcul� */);
	    }
	    else {
	      /* Il faut recuperer x */
	      spell_result_erase_bloc (x);
	    }
#if 0
	  }
	  else {
	    /* Il faut recuperer x */
	    spell_result_erase_bloc (x);
	  }
#endif /* 0 */
	}
      }

      /* ... et on memoize */
      if (top > 1)
	key2index [key_id] = fill_memo_stacks (top);
    }
    else {
      /* Le calcul est deja memoize' */
      x = key2index [key_id];
      fill_spell_result_stacks (top, x);
    }
  }

  if (is_a_subword) {
    if (spell_result_id_stack [top] == 0) {
      /* echec de la correction */
      DTOP (spell_result_id_stack) -= 1;
      top = 0;
    }
  }
  /* else le (nouveau) cur_spell_result->weight_limit calcule' devient "global" */

#if EBUG
  if ((spell_result_id_stack [0] & ID_NB_AND) != top+(spell_result_id_stack [top] & ID_NB_AND))
    sxtrap (ME, "memo_cut_in_subwords");
#endif /* EBUG */

  return top;
}

static void
call_cut (void)
{
  SXINT           max_cut_nb, cut_nb, space_nb, costly_cut_nb;
  SXINT           saved_weight_limit = cur_spell_result->weight_limit;

  /* On ne decoupe pas au-dela de cur_FSA->Q_init ... */
  max_cut_nb =  cur_FSA->Q_init-2;

  space_nb = WORD_HAS_SPACES ? get_space_nb (cur_FSA->q0, cur_FSA->Q_init) : 0;

  if (space_nb > 0) {
    /* On essaie en 1er un decoupage en exactement space_nb */
    memo_cut_in_subwords (cur_FSA->q0, cur_FSA->Q_init, space_nb);
  }

  for (cut_nb = 1; cut_nb <= max_cut_nb; cut_nb++) {
    if (cut_nb != space_nb) {
      cur_spell_result->weight_limit = saved_weight_limit;
      if ((costly_cut_nb = cut_nb-space_nb) >= 1) {
	if ((SXINT)get_total_weight (get_new_compose_weight (NUL_WEIGHT, cur_spell_result->sub_word_weight, costly_cut_nb)) > cur_spell_result->weight_limit)
	  /* Le cout de ce decoupage (et des suivants) en sous-mots excede le poids limite courant */
	  break;
      }
      memo_cut_in_subwords (cur_FSA->q0, cur_FSA->Q_init, cut_nb);
    }
  }

  cur_spell_result_stack_bot = 1; /* bloc principal */
}

bool
sxspell_do_it (struct FSA *FSA)
{
  SXINT           x, cur_total_weight, prev_total_weight, nb, store_nbest, nmemb;
  SXINT           *id_stack;
  struct weight *weight_stack;

  cur_FSA = FSA;
  cur_spell_result = &(FSA->spell_result);
  cur_dico = FSA->dico;

  tooth = cur_dico->init_base;

  if (/*tooth == 0 || */cur_dico->max == 0)
    return false;

  //  suffix_mask = cur_dico->is_suffix_mask;
  base_shift = cur_dico->base_shift;
  class_mask = cur_dico->class_mask;
  stop_mask = cur_dico->stop_mask;
  comb_vector = cur_dico->comb_vector;
  char2class = cur_dico->char2class;

  total_weight.pos = total_weight.neg = 0;

  cur_spell_result->weight_limit = cur_spell_result->max_weight;
  underflow_nb = 0;

  if (FSA->spell_result.words.table == NULL) {
    sxword_alloc (&(FSA->spell_result.words), "FSA->spell_result.words", 63, 1, 16,
		  sxcont_malloc, sxcont_alloc, sxcont_realloc, (sxresize_t) NULL, sxcont_free, words_oflw, NULL);
    FSA->spell_result.codes = (SXINT *) sxalloc (SXWORD_size (FSA->spell_result.words)+1, sizeof (SXINT));
    XH_alloc (&(FSA->spell_result.multiple_word_ids), "multiple_word_ids", 256, 1, 4, (sxoflw0_t) NULL, NULL);
  }
  else {
    sxword_clear (&(FSA->spell_result.words));
    XH_clear (&(FSA->spell_result.multiple_word_ids));
  }

  if (spell_result_weight_stack == NULL) {
    DALLOC_STACK (spell_result_id_stack, 256);
    spell_result_weight_stack = (struct weight*) sxalloc (DSIZE (spell_result_id_stack)+1, sizeof (struct weight));
    lvstr = varstr_alloc (128);
    rvstr = varstr_alloc (128);
  }
  else {
    DRAZ (spell_result_id_stack);
    varstr_raz (lvstr);
    varstr_raz (rvstr);
  }
  
  cur_multiple_word_ids_ptr = &(cur_spell_result->multiple_word_ids);

  base = tooth >> base_shift;
  //  is_suffix = tooth & suffix_mask;

  local_path_nb = 0;

  /* On suppose que spell_result_id_stack permet au moins d'y mettre la taille du bloc principal */
  DPUSH (spell_result_id_stack, 0);
  cur_spell_result_stack_bot = DTOP (spell_result_id_stack); /* Bloc principal en 1 */
  last_final_id = 0;

  call_FSA_walk (FSA->q0, (SXINT)0);

  if (underflow_nb) {
    /* On est ds le cas NBEST et on a pu rater au plus underflow_nb resultats */
    /* On recommence avec un peu + de place ... */
    store_nbest = cur_spell_result->kind;
    cur_spell_result->kind += underflow_nb;
    underflow_nb = 0;

    tooth = cur_dico->init_base;
    base = tooth >> base_shift;
    //    is_suffix = tooth & suffix_mask;
	
    cur_spell_result->weight_limit = cur_spell_result->max_weight;
    DRAZ (spell_result_id_stack);
    DPUSH (spell_result_id_stack, 0);
    last_final_id = 0;

    call_FSA_walk (FSA->q0, (SXINT)0);

    /* Ici la valeur de underflow_nb n'a pas d'importance ... */
    cur_spell_result->kind = store_nbest;

    /* On ecrete la pile des resultats */
    id_stack = spell_result_id_stack+cur_spell_result_stack_bot;
    weight_stack = spell_result_weight_stack+cur_spell_result_stack_bot;
    nmemb = TOP (id_stack) & ID_NB_AND;
    nb = 0;
    prev_total_weight = -1;

    for (x = 1; x <= nmemb; x++) {
      cur_total_weight = get_total_weight (weight_stack [x]);

      if (prev_total_weight < cur_total_weight) {
	nb++;

	if (nb > cur_spell_result->kind) {
	  /* On ecrete */
	  DTOP (spell_result_id_stack) = x;
	  TOP (id_stack) = (cur_spell_result->kind << WEIGHT_NB_SHIFT) + x-1;

	  break;
	}

	prev_total_weight = cur_total_weight;
      }
    }

    if (nb >= cur_spell_result->kind)
      cur_spell_result->weight_limit = prev_total_weight;
  }
  
  if (cur_spell_result->seek_sub_words) {
    /* On a demande aussi de tenter des decoupages en sous-mots */
    /* dans un premier temps, je neglige le phenomene underflow ds les merge */
    /* Traitement de la memoization ds le cas du traitement des sous-mots */
    /* q == Etat final max */
    SXINT q, size, top, i;
    SXINT *int_ptr;

    q = sxba_1_rlscan (FSA->final_set, FSA->Q_nb+1);

    KEY_SHIFT = sxlast_bit (q);
    MEMO_BASE = q + 1;
    size = MEMO_BASE*MEMO_BASE;
    WORD_HAS_SPACES = SXBA_bit_is_set (FSA->orig_char_set, ((SXINT)' '));

    if (memo_hd == NULL) {
      memo_hd = (SXINT*) sxcalloc ((SXUINT)size, sizeof (SXINT));
      memo_hd [0] = size;

      DALLOC_STACK (memo_id_stack, ((FSA->spell_result.kind == 0) ? 5 : FSA->spell_result.kind)*size);
      memo_weight_limit = (SXINT*) sxalloc (DSIZE (memo_id_stack)+1, sizeof (SXINT));
      memo_weight_stack = (struct weight*) sxalloc (DSIZE (memo_id_stack)+1, sizeof (struct weight));

      X_alloc (&key_hd, "key_hd", size, 1, key_hd_oflw, NULL);
      key2index = (SXINT*) sxalloc (X_size (key_hd)+1, sizeof (SXINT));
    }
    else {
      int_ptr = memo_hd;
      top = memo_hd [0];

      for (i = 1; i < top; i++) {
	*++int_ptr = 0;
      }

      if (size > memo_hd [0]) {
	memo_hd = (SXINT*) sxrecalloc (memo_hd, memo_hd [0], size, sizeof (SXINT));
	memo_hd [0] = size;
      }

      DRAZ (memo_id_stack);
      X_clear (&key_hd);
    }

    call_cut ();
    /* Les resultats se trouvent ds spell_result_id_stack */
  }
  
  cur_spell_result->ids = spell_result_id_stack + cur_spell_result_stack_bot;
  cur_spell_result->spelling_nb = cur_spell_result->ids [0] & ID_NB_AND;
  cur_spell_result->weights = spell_result_weight_stack + cur_spell_result_stack_bot;

  return (SXSPELL_spelling_nb (*FSA) > 0);
}


/* mod�le de correcteur appel� par -DEMO */
#ifdef EMO

#include "sxspell_rules.h"

#define Dac_weight               (-10)
/* Dac (Pierre) */
static unsigned char *Dac [] = {
  (unsigned char*) "es", (unsigned char*) "�",
  (unsigned char*) "s", (unsigned char*) "e",
};

void
sxspell_model (unsigned char kind,
	       SXINT max_weight,
	       bool is_sub_word,
	       SXINT sub_word_weight,
	       char *mot,
	       struct sxdfa_comb *dico)
{
  SXINT                 id, x, y, top, weight;
  SXINT                 *bot_ptr, *top_ptr;
  static  struct FSA  main_FSA;



#if 0
  char *pmot = mot;
  SXINT  pl = strlen (mot);
  printf ("Looking for a spelling error on \"%s\" (code = %i)\n", mot, sxdico_get (dico, &pmot, &pl));
#endif /* 0 */

  /* Appel initial : on recherche toutes les fautes ("ALL") */
  sxspell_init (dico,
		&main_FSA,
		kind,
		max_weight,
		compose_weight,
		is_sub_word,
		sub_word_weight);

  /* On fabrique l'automate initial du mot ... */
  sxspell_init_fsa (mot, strlen (mot), &main_FSA);

  /* On peut ajouter qq lettres en fin */
  sxspell_add (&main_FSA, strlen (mot)+1, (unsigned char*) "cdefhpstxz", add_weight);
  /* et une hache muette au d�but... */
  sxspell_add (&main_FSA, 1, (unsigned char*) "hH", add_weight);

  /* changement de casse */
  sxspell_replace (&main_FSA, maj_min, T_NB (maj_min), maj_min_weight);
  sxspell_replace (&main_FSA, min_maj, T_NB (min_maj), min_maj_weight);
  /* traitement des diacritiques */
  sxspell_replace (&main_FSA, diacritics, T_NB (diacritics), diacritics_weight);
  sxspell_replace (&main_FSA, twochars_to_diacritics, T_NB (twochars_to_diacritics), twochars_to_diacritics_weight);
  /* traitement des fautes de frappe */
  sxspell_replace (&main_FSA, typos_qwerty, T_NB (typos_qwerty), typos_weight);
  /* traitement du changement de claviers */
  sxspell_replace (&main_FSA, qwerty_azerty, T_NB (qwerty_azerty), qwerty_azerty_weight);
  sxspell_replace (&main_FSA, azerty_qwerty, T_NB (azerty_qwerty), azerty_qwerty_weight);
  /* On s'est trompe' de voyelle, meme accent */
  sxspell_replace (&main_FSA, mauvaise_voyelle, T_NB (mauvaise_voyelle), mauvaise_voyelle_weight);
  /* changement blanc/tiret */
  sxspell_replace (&main_FSA, space_hyphen, T_NB (space_hyphen), space_hyphen_weight);
  /* Suppression des blancs */
  sxspell_replace_n_p (&main_FSA, space_suppress, T_NB (space_suppress), space_suppress_weight);
  /* changement apos/tiret */
  sxspell_replace (&main_FSA, apos_hyphen, T_NB (apos_hyphen), apos_hyphen_weight);

#if 0
  /* Essai, attention au temps ... */
  sxspell_suppress_all (&main_FSA, suppress_weight);
#endif /* 0 */
  
#if 0
  /* nooooooooooonnnnnnnnnn => non */
  sxspell_no_repeat (&main_FSA, 3, no_repeat_weight);
#endif /* 0 */

  /* Le temps augmente tres fortement si i > 1 !! */
  /* traitement de la suppression de 1 caractere successif */
  sxspell_suppress_i (&main_FSA, suppress_weight, 1);

  /* traitement de l'intervertion de 2 caracteres */
  sxspell_swap (&main_FSA, swap_weight);
  /* il faut des consonnes double'es */
  sxspell_replace_n_p (&main_FSA, m_en_mm, T_NB (m_en_mm), m_en_mm_weight);
  /* les lettres double'es sont fausses */
  sxspell_replace_n_p (&main_FSA, mm_en_m, T_NB (mm_en_m), mm_en_m_weight);
  /* qq fautes d'orthographe */
  sxspell_replace_n_p (&main_FSA, final_underscore, T_NB (final_underscore), final_underscore_weight);
  sxspell_replace_n_p (&main_FSA, ortho, T_NB (ortho), ortho_weight);
  sxspell_replace_n_p (&main_FSA, ortho2, T_NB (ortho2), ortho2_weight);

  /* insertion de tirets */
  for (x = strlen (mot); x >= 1; x--)
     sxspell_add (&main_FSA, x, (unsigned char*) "-", add_hyphen_weight);

  /* insertion de qq lettres */
  for (x = strlen (mot); x >= 1; x--) {
    sxspell_add (&main_FSA, x, insert_string, insert_weight);
  }

  sxspell_replace_n_p (&main_FSA, ocr, T_NB (ocr), ocr_weight);

  sxspell_replace_n_p (&main_FSA, Dac, T_NB (Dac), Dac_weight);

  /* remplacements */
  // ATTENTION: Comment� par BS � des fins d'efficacit� (05/05)
  //  for (x = strlen (mot); x >= 1; x--)
  //    sxspell_change (&main_FSA, x, change_string, change_weight);

  if (sxspell_do_it (&main_FSA)) {
    for (x = 1; x <= SXSPELL_spelling_nb (main_FSA); x++) {
      id = SXSPELL_x2id (main_FSA, x);

      if (!SXSPELL_has_subwords (id))
	printf ("Weight = %i, Code = %i, Word = \"%s\"\n",
		SXSPELL_x2weight(main_FSA, x),
		SXSPELL_id2code (main_FSA, id),
		SXSPELL_id2str (main_FSA, id)
		);
      else {
	/* cas sous-mots */
	printf ("Weight = %i, {",
		SXSPELL_x2weight(main_FSA, x));

	SXSPELL_subword_foreach (main_FSA, id, bot_ptr, top_ptr) {
	  id = SXSPELL_x2subword_id (bot_ptr);

	  printf ("(Code = %i, Word = \"%s\")",
		  SXSPELL_id2code (main_FSA, id),
		  SXSPELL_id2str (main_FSA, id)
		  );

	  if (!SXSPELL_last_subword (bot_ptr, top_ptr))
	    fputs (", ", stdout);
	}

	fputs ("}\n", stdout);
      }
    }
  }
  else
    fputs ("No spelling correction ...\n", stdout); 

  /* ... et on libere */
  sxspell_final_fsa (&main_FSA);
  /* Appel final */
  sxspell_final (&main_FSA);
}
#endif /* EMO */
