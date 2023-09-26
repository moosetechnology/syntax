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
#ifdef latin1
static unsigned char *MAJ_string = (unsigned char *) "ABCDEFGHIJKLMNOPQRSTUVWXYZ������������������������������";
static unsigned char *MIN_string = (unsigned char *) "abcdefghijklmnopqrstuvwxyz�������������������������������";
static unsigned char *vowell_string = (unsigned char*) "AaEeIiOoUuYy����������������������������������������������������";
static unsigned char *mpb_string = (unsigned char*) "mpb";
#endif /* latin1 */
#ifdef latin2
static unsigned char *MAJ_string = (unsigned char*) "ABCDEFGHIJKLMNOPQRSTUVWXYZ����������������������������������������";
static unsigned char *MIN_string = (unsigned char*) "abcdefghijklmnopqrstuvwxyz����������������������������������������";
static unsigned char *vowell_string = (unsigned char*) "AaEeIiOoUuYy����������������������������������������";
static unsigned char *mpb_string = (unsigned char*) "mpb";
#endif /* latin2 */
#ifdef koi8r
static unsigned char *MAJ_string = (unsigned char*) "��������������������������������";
static unsigned char *MIN_string = (unsigned char*) "������������������������������أ";
static unsigned char *vowell_string = (unsigned char*) "������������������ѣ";
static unsigned char *mpb_string = (unsigned char*) "mpb";
#endif /* koi8r */

#ifdef latin1
static unsigned char *insert_string = (unsigned char*) "abcdefghijklmnopqrstuvwxyz������������-";
static unsigned char *change_string = (unsigned char*) "abcdefghijklmnopqrstuvwxyz������������";
#endif /* latin1 */
#ifdef latin2
static unsigned char *insert_string = (unsigned char*) "abcdefghijklmnopqrstuvwxyz����������������������������������������-";
static unsigned char *change_string = (unsigned char*) "abcdefghijklmnopqrstuvwxyz����������������������������������������";
#endif /* latin2 */
#ifdef koi8r
static unsigned char *insert_string = (unsigned char*) "�������������������������������ѣ-";
static unsigned char *change_string = (unsigned char*) "�������������������������������ѣ";
#endif /* koi8r */

#define light_compose_weight            50
#define super_light_compose_weight      50
#define compose_weight	                50

#define detach_weight                   40

/* Poids de chaque type de correction */
/* Si le poids est n�gatif ou nul le co�t de la composition est nul */
// utilis�es dans sxspell/text2dag
// 1. �taient dans sxspeller.c
#define maj_min_weight	           (-10)
#define min_maj_weight	8
#define add_diacritics_weight	           (-5)
#define remove_diacritics_weight	           (-30)
#define change_diacritics_weight	           (-25)
#define twochars_to_diacritics_weight   (-5)
#define no_repeat_weight              (-60)
#define maj2diacritic_weight               (-5)
#define final_als_weight              (-80)
#define iI_en_l_weight                (-15)
#define final_h_weight                (-20)
#define eau_en_au_weight              (-20)
#define final_dot_weight              5
#define t_il_weight                   15
#define blanc_weight                  detach_weight+5
#define abbrev_weight                  15
#define mm_en_m_weight                (-20)
#define m_en_mm_weight                (-20)
#define add_h_weight                  24
#define final_e_weight                75
#define gu_ge_weight                  30
#define dusse_je_weight               (-40)
#define add_apos_weight               45
#define qq_trucs_weight    50
// 2. �taient dans sxspell.h
#define ortho_weight		 76
#define ortho2_weight		 (ortho_weight+2)
#define add_hyphen_weight	(-10)
#define typos_weight		 60
#define mauvaise_voyelle_weight	 70
#define qwerty_azerty_weight	 60
#define insert_weight            80
#define change_weight		 78
#define suppress_weight		 79
#define swap_weight		 60
// pour le dagger
/* pour traiter le cas des pre'fixes/suffixes laisse's ds cet etat par le spelleur ou le corpus :*/
#define final_underscore_weight              4

// autres trucs de sxspell.h
#define diacritics_and_case_weight	(-10)
#define space_suppress_weight	(-10)
#define apos_hyphen_weight	(-10)
#define azerty_qwerty_weight	 60
#define ocr_weight		 65
#define add_weight		 40

/* Changement de casse */
static unsigned char *maj_min [] = {
#if defined(latin1) || defined(latin2)
  (unsigned char*) "A", (unsigned char*) "a",
  (unsigned char*) "B", (unsigned char*) "b",
  (unsigned char*) "C", (unsigned char*) "c",
  (unsigned char*) "D", (unsigned char*) "d",
  (unsigned char*) "E", (unsigned char*) "e",
  (unsigned char*) "F", (unsigned char*) "f",
  (unsigned char*) "G", (unsigned char*) "g",
  (unsigned char*) "H", (unsigned char*) "h",
  (unsigned char*) "I", (unsigned char*) "i",
  (unsigned char*) "J", (unsigned char*) "j",
  (unsigned char*) "K", (unsigned char*) "k",
  (unsigned char*) "L", (unsigned char*) "l",
  (unsigned char*) "M", (unsigned char*) "m",
  (unsigned char*) "N", (unsigned char*) "n",
  (unsigned char*) "O", (unsigned char*) "o",
  (unsigned char*) "P", (unsigned char*) "p",
  (unsigned char*) "Q", (unsigned char*) "q",
  (unsigned char*) "R", (unsigned char*) "r",
  (unsigned char*) "S", (unsigned char*) "s",
  (unsigned char*) "T", (unsigned char*) "t",
  (unsigned char*) "U", (unsigned char*) "u",
  (unsigned char*) "V", (unsigned char*) "v",
  (unsigned char*) "W", (unsigned char*) "w",
  (unsigned char*) "X", (unsigned char*) "x",
  (unsigned char*) "Y", (unsigned char*) "y",
  (unsigned char*) "Z", (unsigned char*) "z",
#endif /* latin1 || latin2 */
#ifdef latin1
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
#endif /* latin1 */
#ifdef latin2
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
#endif /* latin2 */
#ifdef koi8r
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
#endif /* koi8r */
};


/* maj to min */
static unsigned char *min_maj [] = {
#if defined(latin1) || defined(latin2)
  (unsigned char*) "a", (unsigned char*) "A",
  (unsigned char*) "b", (unsigned char*) "B",
  (unsigned char*) "c", (unsigned char*) "C",
  (unsigned char*) "d", (unsigned char*) "D",
  (unsigned char*) "e", (unsigned char*) "E",
  (unsigned char*) "f", (unsigned char*) "F",
  (unsigned char*) "g", (unsigned char*) "G",
  (unsigned char*) "h", (unsigned char*) "H",
  (unsigned char*) "i", (unsigned char*) "I",
  (unsigned char*) "j", (unsigned char*) "J",
  (unsigned char*) "k", (unsigned char*) "K",
  (unsigned char*) "l", (unsigned char*) "L",
  (unsigned char*) "m", (unsigned char*) "M",
  (unsigned char*) "n", (unsigned char*) "N",
  (unsigned char*) "o", (unsigned char*) "O",
  (unsigned char*) "p", (unsigned char*) "P",
  (unsigned char*) "q", (unsigned char*) "Q",
  (unsigned char*) "r", (unsigned char*) "R",
  (unsigned char*) "s", (unsigned char*) "S",
  (unsigned char*) "t", (unsigned char*) "T",
  (unsigned char*) "u", (unsigned char*) "U",
  (unsigned char*) "v", (unsigned char*) "V",
  (unsigned char*) "w", (unsigned char*) "W",
  (unsigned char*) "x", (unsigned char*) "X",
  (unsigned char*) "y", (unsigned char*) "Y",
  (unsigned char*) "z", (unsigned char*) "Z",
#endif /* latin1 || latin2 */
#ifdef latin1
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
#endif /* latin1 */
#ifdef koi8r
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
#endif /* koi8r */
};


/* remplacement des diacritics avec changement de casse possible */
static unsigned char *diacritics_and_case [] = {
#ifdef latin1
  (unsigned char*) "A", (unsigned char*) "����a����",
  (unsigned char*) "�", (unsigned char*) "A���a����",
  (unsigned char*) "�", (unsigned char*) "A���a����",
  (unsigned char*) "�", (unsigned char*) "A���a����",
  (unsigned char*) "�", (unsigned char*) "A���a����",
  (unsigned char*) "a", (unsigned char*) "A��������",
  (unsigned char*) "�", (unsigned char*) "A����a���",
  (unsigned char*) "�", (unsigned char*) "A����a���",
  (unsigned char*) "�", (unsigned char*) "A����a���",
  (unsigned char*) "�", (unsigned char*) "A����a���",

  (unsigned char*) "E", (unsigned char*) "����e����",
  (unsigned char*) "�", (unsigned char*) "E���e����",
  (unsigned char*) "�", (unsigned char*) "E���e����",
  (unsigned char*) "�", (unsigned char*) "E���e����",
  (unsigned char*) "�", (unsigned char*) "E���e����",
  (unsigned char*) "e", (unsigned char*) "E��������",
  (unsigned char*) "�", (unsigned char*) "E����e���",
  (unsigned char*) "�", (unsigned char*) "E����e���",
  (unsigned char*) "�", (unsigned char*) "E����e���",
  (unsigned char*) "�", (unsigned char*) "E����e���",

  (unsigned char*) "I", (unsigned char*) "����i����",
  (unsigned char*) "�", (unsigned char*) "I���i����",
  (unsigned char*) "�", (unsigned char*) "I���i����",
  (unsigned char*) "�", (unsigned char*) "I���i����",
  (unsigned char*) "�", (unsigned char*) "I���i����",
  (unsigned char*) "i", (unsigned char*) "I��������",
  (unsigned char*) "�", (unsigned char*) "I����i���",
  (unsigned char*) "�", (unsigned char*) "I����i���",
  (unsigned char*) "�", (unsigned char*) "I����i���",
  (unsigned char*) "�", (unsigned char*) "I����i���",

  (unsigned char*) "O", (unsigned char*) "���o���",
  (unsigned char*) "�", (unsigned char*) "O��o���",
  (unsigned char*) "�", (unsigned char*) "O��o���",
  (unsigned char*) "�", (unsigned char*) "O��o���",
  (unsigned char*) "o", (unsigned char*) "O������",
  (unsigned char*) "�", (unsigned char*) "O���o��",
  (unsigned char*) "�", (unsigned char*) "O���o��",
  (unsigned char*) "�", (unsigned char*) "O���o��",

  (unsigned char*) "U", (unsigned char*) "����u����",
  (unsigned char*) "�", (unsigned char*) "U���u����",
  (unsigned char*) "�", (unsigned char*) "U���u����",
  (unsigned char*) "�", (unsigned char*) "U���u����",
  (unsigned char*) "�", (unsigned char*) "U���u����",
  (unsigned char*) "u", (unsigned char*) "U��������",
  (unsigned char*) "�", (unsigned char*) "U����u���",
  (unsigned char*) "�", (unsigned char*) "U����u���",
  (unsigned char*) "�", (unsigned char*) "U����u���",
  (unsigned char*) "�", (unsigned char*) "U����u���",

  (unsigned char*) "Y", (unsigned char*) "y�",
  (unsigned char*) "y", (unsigned char*) "Y�",
  (unsigned char*) "�", (unsigned char*) "Yy",

  (unsigned char*) "N", (unsigned char*) "�n�",
  (unsigned char*) "�", (unsigned char*) "Nn�",
  (unsigned char*) "n", (unsigned char*) "N��",
  (unsigned char*) "�", (unsigned char*) "N�n",

  (unsigned char*) "C", (unsigned char*) "�c�",
  (unsigned char*) "�", (unsigned char*) "Cc�",
  (unsigned char*) "c", (unsigned char*) "C��",
  (unsigned char*) "�", (unsigned char*) "C�c",
#endif /* latin1 */
#ifdef koi8r
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "峣",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "���",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "ų�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "���",
#endif /* koi8r */
};

/* Suppression des blancs */
static unsigned char *space_suppress [] = {
  (unsigned char*) " ", (unsigned char*) "",
};


/* changement apostrophe/tiret */
static unsigned char *apos_hyphen [] = {
  (unsigned char*) "'", (unsigned char*) "-",
  (unsigned char*) "-", (unsigned char*) "'",
};

/* On s'est trompe' de voyelle ... */
static unsigned char *mauvaise_voyelle [] = {
#ifdef latin1
  (unsigned char*) "a", (unsigned char*) "����e����i����o���u���y�",
  (unsigned char*) "�", (unsigned char*) "a���e����i����o���u���y�",
  (unsigned char*) "�", (unsigned char*) "a���e����i����o���u���y�",
  (unsigned char*) "�", (unsigned char*) "a���e����i����o���u���y�",
  (unsigned char*) "�", (unsigned char*) "a���e����i����o���u���y�",
  (unsigned char*) "e", (unsigned char*) "a��������i����o���u���y�",
  (unsigned char*) "�", (unsigned char*) "a����e���i����o���u���y�",
  (unsigned char*) "�", (unsigned char*) "a����e���i����o���u���y�",
  (unsigned char*) "�", (unsigned char*) "a����e���i����o���u���y�",
  (unsigned char*) "�", (unsigned char*) "a����e���i����o���u���y�",
  (unsigned char*) "i", (unsigned char*) "a����e��������o���u���y�",
  (unsigned char*) "�", (unsigned char*) "a����e����i���o���u���y�",
  (unsigned char*) "�", (unsigned char*) "a����e����i���o���u���y�",
  (unsigned char*) "�", (unsigned char*) "a����e����i���o���u���y�",
  (unsigned char*) "�", (unsigned char*) "a����e����i���o���u���y�",
  (unsigned char*) "o", (unsigned char*) "a����e����i�������u���y�",
  (unsigned char*) "�", (unsigned char*) "a����e����i����o��u���y�",
  (unsigned char*) "�", (unsigned char*) "a����e����i����o��u���y�",
  (unsigned char*) "�", (unsigned char*) "a����e����i����o��u���y�",
  (unsigned char*) "u", (unsigned char*) "a����e����i����o������y�",
  (unsigned char*) "�", (unsigned char*) "a����e����i����o���u��y�",
  (unsigned char*) "�", (unsigned char*) "a����e����i����o���u��y�",
  (unsigned char*) "�", (unsigned char*) "a����e����i����o���u��y�",
  (unsigned char*) "y", (unsigned char*) "a����e����i����o���u����",
  (unsigned char*) "�", (unsigned char*) "a����e����i����o���u���y",
#endif /* latin1 */
};

/* fautes de frappe pour un clavier QWERTY ... */
static unsigned char *typos_qwerty [] = {
#if defined(latin1) || defined(latin2)
  (unsigned char*) "1", (unsigned char*) "q",
  (unsigned char*) "2", (unsigned char*) "qw",
  (unsigned char*) "3", (unsigned char*) "we����",
  (unsigned char*) "4", (unsigned char*) "e����r",
  (unsigned char*) "5", (unsigned char*) "rt",
  (unsigned char*) "6", (unsigned char*) "ty",
  (unsigned char*) "7", (unsigned char*) "yu���",
  (unsigned char*) "8", (unsigned char*) "u���i��",
  (unsigned char*) "9", (unsigned char*) "i��o�",
  (unsigned char*) "0", (unsigned char*) "o�p",
  (unsigned char*) "-", (unsigned char*) "p",
  (unsigned char*) "=", (unsigned char*) "-",
  (unsigned char*) "[", (unsigned char*) "p",
  (unsigned char*) ";", (unsigned char*) "pl",
  (unsigned char*) ",", (unsigned char*) "klm",

  (unsigned char*) "q", (unsigned char*) "wa��",
  (unsigned char*) "w", (unsigned char*) "qa��se����",
  (unsigned char*) "e", (unsigned char*) "wsdr",
  (unsigned char*) "r", (unsigned char*) "e����dft",
  (unsigned char*) "t", (unsigned char*) "rfgy�",
  (unsigned char*) "y", (unsigned char*) "tghu���",
  (unsigned char*) "u", (unsigned char*) "y�hjki��",
  (unsigned char*) "i", (unsigned char*) "u���jko�",
  (unsigned char*) "o", (unsigned char*) "i��klp",
  (unsigned char*) "p", (unsigned char*) "o�l-",
  (unsigned char*) "a", (unsigned char*) "zswq",
  (unsigned char*) "s", (unsigned char*) "qa��zxde����w",
  (unsigned char*) "d", (unsigned char*) "e����sxc�fr",
  (unsigned char*) "f", (unsigned char*) "rdc�vgt",
  (unsigned char*) "g", (unsigned char*) "tfvbhy�",
  (unsigned char*) "h", (unsigned char*) "y�gbnju���",
  (unsigned char*) "j", (unsigned char*) "u���hnmki��",
  (unsigned char*) "k", (unsigned char*) "i��jmlo�",
  (unsigned char*) "l", (unsigned char*) "o�kp",
  (unsigned char*) "z", (unsigned char*) "a��sx",
  (unsigned char*) "x", (unsigned char*) "zsdc�",
  (unsigned char*) "c", (unsigned char*) "xdfv",
  (unsigned char*) "v", (unsigned char*) "c�fgb",
  (unsigned char*) "b", (unsigned char*) "vghn",
  (unsigned char*) "n", (unsigned char*) "bhjm",
  (unsigned char*) "m", (unsigned char*) "njk",

  (unsigned char*) "!", (unsigned char*) "Q",
  (unsigned char*) "@", (unsigned char*) "QW",
  (unsigned char*) "#", (unsigned char*) "WE����",
  (unsigned char*) "$", (unsigned char*) "E����R",
  (unsigned char*) "%", (unsigned char*) "RT",
  (unsigned char*) "^", (unsigned char*) "TY",
  (unsigned char*) "&", (unsigned char*) "YU���",
  (unsigned char*) "*", (unsigned char*) "U���I��",
  (unsigned char*) "(", (unsigned char*) "I��O�",
  (unsigned char*) ")", (unsigned char*) "O�P",
  (unsigned char*) "_", (unsigned char*) "P",
  (unsigned char*) "+", (unsigned char*) "_",
  (unsigned char*) "{", (unsigned char*) "P",
  (unsigned char*) ":", (unsigned char*) "PL",
  (unsigned char*) "<", (unsigned char*) "KLM",

  (unsigned char*) "Q", (unsigned char*) "WA��",
  (unsigned char*) "W", (unsigned char*) "QA��SE����",
  (unsigned char*) "E", (unsigned char*) "WSDR",
  (unsigned char*) "R", (unsigned char*) "E����DFT",
  (unsigned char*) "T", (unsigned char*) "RFGY",
  (unsigned char*) "Y", (unsigned char*) "TGHU���",
  (unsigned char*) "U", (unsigned char*) "YHJI��",
  (unsigned char*) "I", (unsigned char*) "U���JKO�",
  (unsigned char*) "O", (unsigned char*) "I��KLP",
  (unsigned char*) "P", (unsigned char*) "O�L_",
  (unsigned char*) "A", (unsigned char*) "ZSWQ",
  (unsigned char*) "S", (unsigned char*) "QA��ZXDE����W",
  (unsigned char*) "D", (unsigned char*) "E����SXC�FR",
  (unsigned char*) "F", (unsigned char*) "RDC�VGT",
  (unsigned char*) "G", (unsigned char*) "TFVBHY",
  (unsigned char*) "H", (unsigned char*) "YGBNJU���",
  (unsigned char*) "J", (unsigned char*) "���UHNMKI��",
  (unsigned char*) "K", (unsigned char*) "I��JMLO�",
  (unsigned char*) "L", (unsigned char*) "O�KP",
  (unsigned char*) "Z", (unsigned char*) "A��SX",
  (unsigned char*) "X", (unsigned char*) "ZSDC�",
  (unsigned char*) "C", (unsigned char*) "XDFV",
  (unsigned char*) "V", (unsigned char*) "C�FGB",
  (unsigned char*) "B", (unsigned char*) "VGHN",
  (unsigned char*) "N", (unsigned char*) "BHJM",
  (unsigned char*) "M", (unsigned char*) "NJK",
#endif /* latin1 || latin2 */
};

/* Changement de clavier ... */
static unsigned char *qwerty_azerty [] = {
#if defined(latin1) || defined(latin2)
  (unsigned char*) "2", (unsigned char*) "�",
  (unsigned char*) "7", (unsigned char*) "�",
  (unsigned char*) "9", (unsigned char*) "�",
  (unsigned char*) "0", (unsigned char*) "�",
  (unsigned char*) "q", (unsigned char*) "a",
  (unsigned char*) "w", (unsigned char*) "z",
  (unsigned char*) "a", (unsigned char*) "q",
  (unsigned char*) ";", (unsigned char*) "m",
  (unsigned char*) "z", (unsigned char*) "w",
  (unsigned char*) "m", (unsigned char*) ",",
#endif /* latin1 || latin2 */
};

static unsigned char *azerty_qwerty [] = {
#if defined(latin1) || defined(latin2)
  (unsigned char*) "�", (unsigned char*) "2",
  (unsigned char*) "�", (unsigned char*) "7",
  (unsigned char*) "�", (unsigned char*) "9",
  (unsigned char*) "�", (unsigned char*) "0",
  (unsigned char*) "a", (unsigned char*) "q",
  (unsigned char*) "z", (unsigned char*) "w",
  (unsigned char*) "q", (unsigned char*) "a",
  (unsigned char*) "z", (unsigned char*) ";",
  (unsigned char*) "w", (unsigned char*) "m",
  (unsigned char*) ",", (unsigned char*) "m",
#endif /* latin1 || latin2 */
};

/* qq fautes */
static unsigned char *ortho [] = {
#ifdef french
  /* pluriels des mots composes */
  (unsigned char*) "s-", (unsigned char*) "-",
  (unsigned char*) "-", (unsigned char*) "s-",

  (unsigned char*) "o", (unsigned char*) "au",
  (unsigned char*) "o", (unsigned char*) "eau",
  (unsigned char*) "au", (unsigned char*) "o",
  (unsigned char*) "au", (unsigned char*) "eau",
  (unsigned char*) "eau", (unsigned char*) "o",
  (unsigned char*) "eau", (unsigned char*) "au",

  (unsigned char*) "�", (unsigned char*) "ae",
  (unsigned char*) "ae", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "oe",
  (unsigned char*) "e", (unsigned char*) "oe",
  (unsigned char*) "oe", (unsigned char*) "�",
  (unsigned char*) "oe", (unsigned char*) "e",

  /* Essai */
  /* Les autres combi sont faites par ailleurs */
  (unsigned char*) "c", (unsigned char*) "ss",
  (unsigned char*) "c", (unsigned char*) "s",
  (unsigned char*) "c", (unsigned char*) "sc",
  (unsigned char*) "�", (unsigned char*) "ss",
  (unsigned char*) "�", (unsigned char*) "s",
  (unsigned char*) "ss", (unsigned char*) "c",
  (unsigned char*) "ss", (unsigned char*) "�",
  (unsigned char*) "s", (unsigned char*) "c",
  (unsigned char*) "sc", (unsigned char*) "c",
  (unsigned char*) "s", (unsigned char*) "�",

  (unsigned char*) "em", (unsigned char*) "am",
  (unsigned char*) "en", (unsigned char*) "an",
  (unsigned char*) "am", (unsigned char*) "em",
  (unsigned char*) "an", (unsigned char*) "en",

  (unsigned char*) "nb", (unsigned char*) "mb",
  (unsigned char*) "np", (unsigned char*) "mp",

  (unsigned char*) "rh", (unsigned char*) "r",
  (unsigned char*) "th", (unsigned char*) "t",
  (unsigned char*) "r", (unsigned char*) "rh",
  (unsigned char*) "t", (unsigned char*) "th",

  (unsigned char*) "ph", (unsigned char*) "f",
  (unsigned char*) "f", (unsigned char*) "ph",

  (unsigned char*) "g", (unsigned char*) "j",
  (unsigned char*) "j", (unsigned char*) "g",
  (unsigned char*) "ge", (unsigned char*) "j",
  (unsigned char*) "j", (unsigned char*) "ge",

  (unsigned char*) "ch", (unsigned char*) "sch",
  (unsigned char*) "ch", (unsigned char*) "sh",
  (unsigned char*) "sch", (unsigned char*) "ch",
  (unsigned char*) "sch", (unsigned char*) "sh",
  (unsigned char*) "sh", (unsigned char*) "ch",
  (unsigned char*) "sh", (unsigned char*) "sch",

  (unsigned char*) "ain", (unsigned char*) "in",
  (unsigned char*) "ain", (unsigned char*) "ein",
  (unsigned char*) "in", (unsigned char*) "ain",
  (unsigned char*) "in", (unsigned char*) "ein",
  (unsigned char*) "ein", (unsigned char*) "ain",
  (unsigned char*) "ein", (unsigned char*) "ain",

  (unsigned char*) "aiss", (unsigned char*) "ess", /* si suivi par exemple de ss */
  (unsigned char*) "aiss", (unsigned char*) "�ss",
  (unsigned char*) "aiss", (unsigned char*) "�ss",
  (unsigned char*) "a�ss", (unsigned char*) "�ss",
  (unsigned char*) "a�ss", (unsigned char*) "�ss",
  (unsigned char*) "a�ss", (unsigned char*) "�ss",

  (unsigned char*) "ess", (unsigned char*) "aiss",
  (unsigned char*) "�ss", (unsigned char*) "aiss",
  (unsigned char*) "�ss", (unsigned char*) "aiss",
  (unsigned char*) "�ss", (unsigned char*) "a�ss",
  (unsigned char*) "�ss", (unsigned char*) "a�ss",
  (unsigned char*) "�ss", (unsigned char*) "a�ss",

/* � quoi sert-ce ?
  (unsigned char*) "est", (unsigned char*) "�",
  (unsigned char*) "est", (unsigned char*) "�",
  (unsigned char*) "est", (unsigned char*) "�",
*/

/* Hibou, chou, ... ; clou, sou, ... */
  (unsigned char*) "ous$", (unsigned char*) "oux",
  (unsigned char*) "oux$", (unsigned char*) "ous",

  /* � voir... en principe �a fait passer d'un mot connu � un autre... donc c pas s�r que �a doit rester l� */
  (unsigned char*) "er$", (unsigned char*) "�",
  (unsigned char*) "�$", (unsigned char*) "er",

/* Finales de verbes
  (unsigned char*) "$", (unsigned char*) "ent",
*/

/*  (unsigned char*) "^l", (unsigned char*) "l'",*/


#endif /* french */
#ifdef polish
  (unsigned char*) "cz", (unsigned char*) "�",
  (unsigned char*) "cz", (unsigned char*) "c",
  (unsigned char*) "c", (unsigned char*) "cz",
  (unsigned char*) "�", (unsigned char*) "cz",
  (unsigned char*) "sz", (unsigned char*) "�",
  (unsigned char*) "sz", (unsigned char*) "s",
  (unsigned char*) "s", (unsigned char*) "sz",
  (unsigned char*) "�", (unsigned char*) "sz",
  (unsigned char*) "rz", (unsigned char*) "�",
  (unsigned char*) "rz", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "rz",
  (unsigned char*) "�", (unsigned char*) "rz",
  (unsigned char*) "ch", (unsigned char*) "h",
  (unsigned char*) "h", (unsigned char*) "ch",
#endif /* polish */
#ifdef spanish
  (unsigned char*) "y", (unsigned char*) "ll",
  (unsigned char*) "ll", (unsigned char*) "y",
#endif /* spanish */
#ifdef italian
  (unsigned char*) "E'", (unsigned char*) "�",
  (unsigned char*) "E'", (unsigned char*) "�",
  (unsigned char*) "U'", (unsigned char*) "�",
  (unsigned char*) "A'", (unsigned char*) "�",
  (unsigned char*) "O'", (unsigned char*) "�",
  (unsigned char*) "I'", (unsigned char*) "�",
  (unsigned char*) "e'", (unsigned char*) "�",
  (unsigned char*) "e'", (unsigned char*) "�",
  (unsigned char*) "u'", (unsigned char*) "�",
  (unsigned char*) "a'", (unsigned char*) "�",
  (unsigned char*) "o'", (unsigned char*) "�",
  (unsigned char*) "i'", (unsigned char*) "�",
#endif /* italian */
};

static unsigned char *ortho2 [] = {
#ifdef french
  (unsigned char*) "ci", (unsigned char*) "ti",
  (unsigned char*) "ti", (unsigned char*) "ci",
  (unsigned char*) "si", (unsigned char*) "ti",
  (unsigned char*) "ti", (unsigned char*) "si",
  (unsigned char*) "ssi", (unsigned char*) "ti",
  (unsigned char*) "ti", (unsigned char*) "ssi",

  (unsigned char*) "k", (unsigned char*) "ch",
  (unsigned char*) "c", (unsigned char*) "ch",
  (unsigned char*) "ch", (unsigned char*) "c",
  (unsigned char*) "ch", (unsigned char*) "k",
  (unsigned char*) "ch", (unsigned char*) "qu",
  (unsigned char*) "qu", (unsigned char*) "ch",

  (unsigned char*) "ch", (unsigned char*) "tch",
  (unsigned char*) "tch", (unsigned char*) "ch",

  (unsigned char*) "ou$", (unsigned char*) "oux",
  (unsigned char*) "o$", (unsigned char*) "ot",

  (unsigned char*) "k", (unsigned char*) "c",
  (unsigned char*) "k", (unsigned char*) "qu",
  (unsigned char*) "c", (unsigned char*) "k",
  (unsigned char*) "c", (unsigned char*) "qu",
  (unsigned char*) "qu", (unsigned char*) "c",
  (unsigned char*) "qu", (unsigned char*) "k",
#endif /* french */
};


/* qq fautes specifiques a` l'OCR */
static unsigned char *ocr [] = {
#if defined(latin1) || defined(latin2)
  (unsigned char*) "fiE", (unsigned char*) "�",
  (unsigned char*) "G", (unsigned char*) "�",
  (unsigned char*) "I3", (unsigned char*) "B",
  (unsigned char*) "8", (unsigned char*) "B",
  (unsigned char*) "B", (unsigned char*) "O",
  (unsigned char*) "1", (unsigned char*) "I",
  (unsigned char*) "1", (unsigned char*) "l",
  (unsigned char*) "I", (unsigned char*) "l",
  (unsigned char*) "l", (unsigned char*) "I",
  (unsigned char*) "h", (unsigned char*) "tu",
  (unsigned char*) "s", (unsigned char*) "x",
  (unsigned char*) "ri", (unsigned char*) "n",
  (unsigned char*) "d", (unsigned char*) "a",
#endif /* latin1 || latin2 */
};


/* Chaque majuscule est change'e en toutes ses variations    (-5) */
static unsigned char *maj2diacritic [] = {
#ifdef latin1
  (unsigned char*) "A", (unsigned char*) "����a����",
  (unsigned char*) "�", (unsigned char*) "A���a����",
  (unsigned char*) "�", (unsigned char*) "A���a����",
  (unsigned char*) "�", (unsigned char*) "A���a����",
  (unsigned char*) "�", (unsigned char*) "A���a����",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "B", (unsigned char*) "b",
  (unsigned char*) "C", (unsigned char*) "�c�",
  (unsigned char*) "�", (unsigned char*) "Cc�",
  (unsigned char*) "D", (unsigned char*) "d",
  (unsigned char*) "E", (unsigned char*) "����e����",
  (unsigned char*) "�", (unsigned char*) "E���e����",
  (unsigned char*) "�", (unsigned char*) "E���e����",
  (unsigned char*) "�", (unsigned char*) "E���e����",
  (unsigned char*) "�", (unsigned char*) "E���e����",
  (unsigned char*) "F", (unsigned char*) "f",
  (unsigned char*) "G", (unsigned char*) "g",
  (unsigned char*) "H", (unsigned char*) "h",
  (unsigned char*) "I", (unsigned char*) "����i����",
  (unsigned char*) "�", (unsigned char*) "I���i����",
  (unsigned char*) "�", (unsigned char*) "I���i����",
  (unsigned char*) "�", (unsigned char*) "I���i����",
  (unsigned char*) "�", (unsigned char*) "I���i����",
  (unsigned char*) "J", (unsigned char*) "j",
  (unsigned char*) "K", (unsigned char*) "k",
  (unsigned char*) "L", (unsigned char*) "l",
  (unsigned char*) "M", (unsigned char*) "m",
  (unsigned char*) "N", (unsigned char*) "�n",
  (unsigned char*) "�", (unsigned char*) "N�",
  (unsigned char*) "O", (unsigned char*) "�����o�����",
  (unsigned char*) "�", (unsigned char*) "O����o�����",
  (unsigned char*) "�", (unsigned char*) "O����o�����",
  (unsigned char*) "�", (unsigned char*) "O����o�����",
  (unsigned char*) "�", (unsigned char*) "O����o�����",
  (unsigned char*) "�", (unsigned char*) "O����o�����",
  (unsigned char*) "P", (unsigned char*) "p",
  (unsigned char*) "Q", (unsigned char*) "q",
  (unsigned char*) "R", (unsigned char*) "r",
  (unsigned char*) "S", (unsigned char*) "s",
  (unsigned char*) "T", (unsigned char*) "t",
  (unsigned char*) "U", (unsigned char*) "����u���",
  (unsigned char*) "�", (unsigned char*) "U���u���",
  (unsigned char*) "�", (unsigned char*) "U���u���",
  (unsigned char*) "�", (unsigned char*) "U���u���",
  (unsigned char*) "�", (unsigned char*) "U���u���",
  (unsigned char*) "V", (unsigned char*) "v",
  (unsigned char*) "W", (unsigned char*) "w",
  (unsigned char*) "X", (unsigned char*) "x",
  (unsigned char*) "Y", (unsigned char*) "�y�",
  (unsigned char*) "�", (unsigned char*) "Yy�",
  (unsigned char*) "Z", (unsigned char*) "z",
#endif /* latin1 */
#ifdef latin2
  (unsigned char*) "A", (unsigned char*) "�����a�����",
  (unsigned char*) "�", (unsigned char*) "A����a�����",
  (unsigned char*) "�", (unsigned char*) "A����a�����",
  (unsigned char*) "�", (unsigned char*) "A����a�����",
  (unsigned char*) "�", (unsigned char*) "A����a�����",
  (unsigned char*) "�", (unsigned char*) "A����a�����",
  (unsigned char*) "B", (unsigned char*) "b",
  (unsigned char*) "C", (unsigned char*) "���c���",
  (unsigned char*) "�", (unsigned char*) "C��c���",
  (unsigned char*) "�", (unsigned char*) "C��c���",
  (unsigned char*) "�", (unsigned char*) "C��c���",
  (unsigned char*) "D", (unsigned char*) "��d��",
  (unsigned char*) "�", (unsigned char*) "D�d��",
  (unsigned char*) "�", (unsigned char*) "D�d��",
  (unsigned char*) "E", (unsigned char*) "����e����",
  (unsigned char*) "�", (unsigned char*) "E���e����",
  (unsigned char*) "�", (unsigned char*) "E���e����",
  (unsigned char*) "�", (unsigned char*) "E���e����",
  (unsigned char*) "�", (unsigned char*) "E���e����",
  (unsigned char*) "F", (unsigned char*) "f",
  (unsigned char*) "G", (unsigned char*) "g",
  (unsigned char*) "H", (unsigned char*) "h",
  (unsigned char*) "I", (unsigned char*) "��i��",
  (unsigned char*) "�", (unsigned char*) "I�i��",
  (unsigned char*) "�", (unsigned char*) "I�i��",
  (unsigned char*) "J", (unsigned char*) "j",
  (unsigned char*) "K", (unsigned char*) "k",
  (unsigned char*) "L", (unsigned char*) "���l���",
  (unsigned char*) "�", (unsigned char*) "L��l���",
  (unsigned char*) "�", (unsigned char*) "L��l���",
  (unsigned char*) "�", (unsigned char*) "L��l���",
  (unsigned char*) "M", (unsigned char*) "m",
  (unsigned char*) "N", (unsigned char*) "��n��",
  (unsigned char*) "�", (unsigned char*) "N�n��",
  (unsigned char*) "�", (unsigned char*) "N�n��",
  (unsigned char*) "O", (unsigned char*) "����o����",
  (unsigned char*) "�", (unsigned char*) "O���o����",
  (unsigned char*) "�", (unsigned char*) "O���o����",
  (unsigned char*) "�", (unsigned char*) "O���o����",
  (unsigned char*) "�", (unsigned char*) "O���o����",
  (unsigned char*) "P", (unsigned char*) "p",
  (unsigned char*) "Q", (unsigned char*) "q",
  (unsigned char*) "R", (unsigned char*) "��r��",
  (unsigned char*) "�", (unsigned char*) "R�r��",
  (unsigned char*) "�", (unsigned char*) "R�r��",
  (unsigned char*) "S", (unsigned char*) "���s���",
  (unsigned char*) "�", (unsigned char*) "S��s���",
  (unsigned char*) "�", (unsigned char*) "S��s���",
  (unsigned char*) "�", (unsigned char*) "S��s���",
  (unsigned char*) "T", (unsigned char*) "��t��",
  (unsigned char*) "�", (unsigned char*) "T�t��",
  (unsigned char*) "�", (unsigned char*) "T�t��",
  (unsigned char*) "U", (unsigned char*) "����u����",
  (unsigned char*) "�", (unsigned char*) "U���u����",
  (unsigned char*) "�", (unsigned char*) "U���u����",
  (unsigned char*) "�", (unsigned char*) "U���u����",
  (unsigned char*) "�", (unsigned char*) "U���u����",
  (unsigned char*) "V", (unsigned char*) "v",
  (unsigned char*) "W", (unsigned char*) "w",
  (unsigned char*) "X", (unsigned char*) "x",
  (unsigned char*) "Y", (unsigned char*) "�y�",
  (unsigned char*) "�", (unsigned char*) "Yy�",
  (unsigned char*) "Z", (unsigned char*) "���z���",
  (unsigned char*) "�", (unsigned char*) "Z��z���",
  (unsigned char*) "�", (unsigned char*) "Z��z���",
  (unsigned char*) "�", (unsigned char*) "Z��z���",
#endif /* latin2 */
};


/* ajout d'une apostrophe dans certains contextes - poids assez �lev� : add_apos_weight */
static unsigned char *add_apos  [] = {
#if defined(latin1) || defined(latin2)
  (unsigned char*) "a", (unsigned char*) "a'",
  (unsigned char*) "�", (unsigned char*) "�'",
  (unsigned char*) "e", (unsigned char*) "e'",
  (unsigned char*) "�", (unsigned char*) "�'",
  (unsigned char*) "�", (unsigned char*) "�'",
  (unsigned char*) "�", (unsigned char*) "�'",
  (unsigned char*) "i", (unsigned char*) "i'",
  (unsigned char*) "�", (unsigned char*) "�'",
  (unsigned char*) "o", (unsigned char*) "o'",
  (unsigned char*) "�", (unsigned char*) "�'",
  (unsigned char*) "u", (unsigned char*) "u'",
  (unsigned char*) "�", (unsigned char*) "�'",
  (unsigned char*) "a'", (unsigned char*) "a",
  (unsigned char*) "�'", (unsigned char*) "�",
  (unsigned char*) "e'", (unsigned char*) "e",
  (unsigned char*) "�'", (unsigned char*) "�",
  (unsigned char*) "�'", (unsigned char*) "�",
  (unsigned char*) "�'", (unsigned char*) "�",
  (unsigned char*) "i'", (unsigned char*) "i",
  (unsigned char*) "�'", (unsigned char*) "�",
  (unsigned char*) "o'", (unsigned char*) "o",
  (unsigned char*) "�'", (unsigned char*) "�",
  (unsigned char*) "u'", (unsigned char*) "u",
  (unsigned char*) "�'", (unsigned char*) "�",
  (unsigned char*) ".'", (unsigned char*) "_",
#endif /* latin1 || latin2 */
};

/* abr�viations sans leur point - poids tr�s faible final_dot_weight */ 
static unsigned char *final_dot  [] = {
  (unsigned char*) "$", (unsigned char*) ".",
};


/* abr�viations sans leur point - poids tr�s faible final_dot_weight */ 
static unsigned char *final_underscore  [] = {
  (unsigned char*) "$", (unsigned char*) "_",
  (unsigned char*) "^", (unsigned char*) "_",
};

/* ajout d'un h initial ou final - poids moyen final_h_weight */
static unsigned char *final_h  [] = {
#ifdef french
  (unsigned char*) "$", (unsigned char*) "h",
  (unsigned char*) "^", (unsigned char*) "h",
  /*  (unsigned char*) "^z", (unsigned char*) "j",*/
  (unsigned char*) "^l", (unsigned char*) "l'",
#endif /* french */
};

/* ajout final d'un e - poids assez �lev� final_e_weight */
static unsigned char *final_e  [] = {
#ifdef french
  (unsigned char*) "$", (unsigned char*) "'",
  (unsigned char*) "$", (unsigned char*) "e",
  (unsigned char*) "ts$", (unsigned char*) "s",
  (unsigned char*) "e$", (unsigned char*) "",
  (unsigned char*) "s$", (unsigned char*) "",
  (unsigned char*) "^z", (unsigned char*) "",
  (unsigned char*) "euh$", (unsigned char*) "e",
  (unsigned char*) "^x", (unsigned char*) "ex",
  (unsigned char*) "'$", (unsigned char*) "",
#endif /* french */
};

/* finales abr�g�es - poids moyen abbrev_weight */
static unsigned char *abbrev  [] = {
#ifdef french
  (unsigned char*) "t�$", (unsigned char*) "tion",
  (unsigned char*) "mt$", (unsigned char*) "ment",
#endif /* french */
};

/* flexions mal fix�es - poids faible  final_als_weight */
static unsigned char *final_als [] = {
#ifdef french
  (unsigned char*) "als$", (unsigned char*) "aux",
  (unsigned char*) "aux$", (unsigned char*) "als",
  (unsigned char*) "�re$", (unsigned char*) "euse",
  (unsigned char*) "euse$", (unsigned char*) "�re",
  (unsigned char*) "�res$", (unsigned char*) "euses",
  (unsigned char*) "euses$", (unsigned char*) "�res",
  (unsigned char*) "oye$", (unsigned char*) "oie",
  (unsigned char*) "oyes$", (unsigned char*) "oies",
  (unsigned char*) "oyent$", (unsigned char*) "oient",
  (unsigned char*) "uye$", (unsigned char*) "uie",
  (unsigned char*) "uyes$", (unsigned char*) "uies",
  (unsigned char*) "uyent$", (unsigned char*) "uient",
#endif /* french */
};

/* i' ou I'- poids faible iI_en_l_weight */
static unsigned char *iI_en_l [] = {
#ifdef latin1
  (unsigned char*) "^i'$", (unsigned char*) "l'",
  (unsigned char*) "^I'$", (unsigned char*) "l'",
#endif /* latin1 */
};

/* poids faible dusse_je_weight */
static unsigned char *dusse_je [] = {
#ifdef french
  (unsigned char*) "ss�-je$", (unsigned char*) "s-je",
  (unsigned char*) "ss�-je$", (unsigned char*) "s-je",
  (unsigned char*) "�-je$", (unsigned char*) "-je",
  (unsigned char*) "�-je$", (unsigned char*) "-je",
#endif /* french */
};

/* -t-il et similaires incomplets - poids faible t_il_weight */
static unsigned char *t_il [] = {
  (unsigned char*) "^-t", (unsigned char*) "-t-",
  (unsigned char*) "t$", (unsigned char*) "",
};

/* Suppression de l'espace ou remplacement par un tiret - poids blanc_weight = detach_weight + 5*/
static unsigned char *blanc [] = {
  (unsigned char*) " ", (unsigned char*) "",
  (unsigned char*) " ", (unsigned char*) "-",
  (unsigned char*) "-", (unsigned char*) "",
  (unsigned char*) " ", (unsigned char*) "_",
  (unsigned char*) "-", (unsigned char*) "_",
};

/* eau <-> au - poids assez faible eau_en_au_weight */
static unsigned char *eau_en_au [] = {
#ifdef french
  (unsigned char*) "eau", (unsigned char*) "au",
  (unsigned char*) "au", (unsigned char*) "eau",
#endif /* french */
#ifdef russian
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
#endif /* russian */
};

/* manque un u ou un e apr�s un g pour en changer le son - poids moyen gu_ge_weight */
static unsigned char *gu_ge [] = {
#ifdef french
  (unsigned char*) "ge", (unsigned char*) "gue",
  (unsigned char*) "g�", (unsigned char*) "gu�",
  (unsigned char*) "g�", (unsigned char*) "gu�",
  (unsigned char*) "g�", (unsigned char*) "gu�",
  (unsigned char*) "gi", (unsigned char*) "gui",
  (unsigned char*) "g�", (unsigned char*) "gu�",
  (unsigned char*) "ga", (unsigned char*) "gea",
  (unsigned char*) "g�", (unsigned char*) "ge�",
  (unsigned char*) "go", (unsigned char*) "geo",
  (unsigned char*) "g�", (unsigned char*) "ge�",
  (unsigned char*) "gu", (unsigned char*) "geu",
  (unsigned char*) "g�", (unsigned char*) "ge�",
#endif /* french */
};


/* ss <-> � */
static unsigned char *esszet [] = {
#ifdef german
  (unsigned char*) "ss", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "ss",
#endif /* german */
};

/* ajout d'un h dans certains contextes - poids moyen add_h_weight */
static unsigned char *add_h [] = {
#ifdef french
  (unsigned char*) "a", (unsigned char*) "ah",
  (unsigned char*) "�", (unsigned char*) "�h",
  (unsigned char*) "e", (unsigned char*) "eh",
  (unsigned char*) "�", (unsigned char*) "�h",
  (unsigned char*) "�", (unsigned char*) "�h",
  (unsigned char*) "�", (unsigned char*) "�h",
  (unsigned char*) "i", (unsigned char*) "ih",
  (unsigned char*) "�", (unsigned char*) "�h",
  (unsigned char*) "o", (unsigned char*) "oh",
  (unsigned char*) "�", (unsigned char*) "�h",
  (unsigned char*) "u", (unsigned char*) "uh",
  (unsigned char*) "�", (unsigned char*) "�h",
  (unsigned char*) "k", (unsigned char*) "kh",
  (unsigned char*) "t", (unsigned char*) "th",
  (unsigned char*) "p", (unsigned char*) "ph",
  (unsigned char*) "d", (unsigned char*) "dh",
  (unsigned char*) "g", (unsigned char*) "gh",
  (unsigned char*) "r", (unsigned char*) "rh",
  (unsigned char*) "b", (unsigned char*) "bh",
  (unsigned char*) "z", (unsigned char*) "zh",
  (unsigned char*) "s", (unsigned char*) "sh",
  (unsigned char*) "c", (unsigned char*) "ch",
  (unsigned char*) "j", (unsigned char*) "dj",
  (unsigned char*) "dj", (unsigned char*) "j",
#endif /* french */
};



/* diacritics ecrits comme 2 caracteres successifs ... */
static unsigned char *twochars_to_diacritics [] = {
#ifdef latin1
  (unsigned char*) "A^", (unsigned char*) "�",
  (unsigned char*) "A'", (unsigned char*) "�",
  (unsigned char*) "A`", (unsigned char*) "�",
  (unsigned char*) "a^", (unsigned char*) "�",
  (unsigned char*) "a'", (unsigned char*) "�",
  (unsigned char*) "a`", (unsigned char*) "�",

  (unsigned char*) "E^", (unsigned char*) "�",
  (unsigned char*) "E'", (unsigned char*) "�",
  (unsigned char*) "E`", (unsigned char*) "�",
  (unsigned char*) "E\"", (unsigned char*) "�",
  (unsigned char*) "e^", (unsigned char*) "�",
  (unsigned char*) "e'", (unsigned char*) "�",
  (unsigned char*) "e`", (unsigned char*) "�",
  (unsigned char*) "e\"", (unsigned char*) "�",

  (unsigned char*) "I^", (unsigned char*) "�",
  (unsigned char*) "I\"", (unsigned char*) "�",
  (unsigned char*) "i^", (unsigned char*) "�",
  (unsigned char*) "i\"", (unsigned char*) "�",

  (unsigned char*) "O^", (unsigned char*) "�",
  (unsigned char*) "o^", (unsigned char*) "�",

  (unsigned char*) "U^", (unsigned char*) "�",
  (unsigned char*) "U`", (unsigned char*) "�",
  (unsigned char*) "U\"", (unsigned char*) "�",
  (unsigned char*) "u^", (unsigned char*) "�",
  (unsigned char*) "u`", (unsigned char*) "�",
  (unsigned char*) "u\"", (unsigned char*) "�",

  (unsigned char*) "y\"", (unsigned char*) "�",

  (unsigned char*) "C,", (unsigned char*) "�",
  (unsigned char*) "c,", (unsigned char*) "�",
#endif /* latin1 */
#ifdef latin2
  (unsigned char*) "A,", (unsigned char*) "�",
  (unsigned char*) "a,", (unsigned char*) "�",
  (unsigned char*) "L�", (unsigned char*) "�",
  (unsigned char*) "l�", (unsigned char*) "�",
  (unsigned char*) "S'", (unsigned char*) "�",
  (unsigned char*) "s'", (unsigned char*) "�",
  (unsigned char*) "S�", (unsigned char*) "�",
  (unsigned char*) "s�", (unsigned char*) "�",
  (unsigned char*) "S,", (unsigned char*) "�",
  (unsigned char*) "s,", (unsigned char*) "�",
  (unsigned char*) "T�", (unsigned char*) "�",
  (unsigned char*) "t�", (unsigned char*) "�",
  (unsigned char*) "Z'", (unsigned char*) "�",
  (unsigned char*) "z'", (unsigned char*) "�",
  (unsigned char*) "Z�", (unsigned char*) "��",
  (unsigned char*) "z�", (unsigned char*) "��",
  (unsigned char*) "R'", (unsigned char*) "�",
  (unsigned char*) "r'", (unsigned char*) "�",
  (unsigned char*) "A'", (unsigned char*) "�",
  (unsigned char*) "a'", (unsigned char*) "�",
  (unsigned char*) "A^", (unsigned char*) "��",
  (unsigned char*) "a^", (unsigned char*) "��",
  (unsigned char*) "A�", (unsigned char*) "�",
  (unsigned char*) "a�", (unsigned char*) "�",
  (unsigned char*) "L'", (unsigned char*) "ţ",
  (unsigned char*) "l'", (unsigned char*) "�",
  (unsigned char*) "C'", (unsigned char*) "�",
  (unsigned char*) "c'", (unsigned char*) "�",
  (unsigned char*) "C,", (unsigned char*) "�",
  (unsigned char*) "c,", (unsigned char*) "�",
  (unsigned char*) "C�", (unsigned char*) "�",
  (unsigned char*) "c�", (unsigned char*) "�",
  (unsigned char*) "E'", (unsigned char*) "�",
  (unsigned char*) "e'", (unsigned char*) "�",
  (unsigned char*) "E,", (unsigned char*) "�",
  (unsigned char*) "e,", (unsigned char*) "�",
  (unsigned char*) "E�", (unsigned char*) "�",
  (unsigned char*) "e�", (unsigned char*) "�",
  (unsigned char*) "E�", (unsigned char*) "�",
  (unsigned char*) "e�", (unsigned char*) "�",
  (unsigned char*) "I'", (unsigned char*) "�",
  (unsigned char*) "i'", (unsigned char*) "�",
  (unsigned char*) "I^", (unsigned char*) "�",
  (unsigned char*) "i^", (unsigned char*) "�",
  (unsigned char*) "D�", (unsigned char*) "��",
  (unsigned char*) "d�", (unsigned char*) "��",
  (unsigned char*) "N'", (unsigned char*) "�",
  (unsigned char*) "n'", (unsigned char*) "�",
  (unsigned char*) "N�", (unsigned char*) "�",
  (unsigned char*) "n�", (unsigned char*) "�",
  (unsigned char*) "O'", (unsigned char*) "�",
  (unsigned char*) "o'", (unsigned char*) "�",
  (unsigned char*) "O^", (unsigned char*) "�",
  (unsigned char*) "o^", (unsigned char*) "�",
  (unsigned char*) "O\"", (unsigned char*) "�",
  (unsigned char*) "o\"", (unsigned char*) "�",
  (unsigned char*) "O�", (unsigned char*) "�",
  (unsigned char*) "o�", (unsigned char*) "�",
  (unsigned char*) "R�", (unsigned char*) "�",
  (unsigned char*) "r�", (unsigned char*) "�",
  (unsigned char*) "U�", (unsigned char*) "�",
  (unsigned char*) "u�", (unsigned char*) "�",
  (unsigned char*) "U'", (unsigned char*) "�",
  (unsigned char*) "u'", (unsigned char*) "�",
  (unsigned char*) "U\"", (unsigned char*) "�",
  (unsigned char*) "u\"", (unsigned char*) "�",
  (unsigned char*) "U�", (unsigned char*) "�",
  (unsigned char*) "u�", (unsigned char*) "�",
  (unsigned char*) "Y'", (unsigned char*) "�",
  (unsigned char*) "y'", (unsigned char*) "�",
  (unsigned char*) "T,", (unsigned char*) "�",
  (unsigned char*) "t,", (unsigned char*) "�",
#endif /* latin2 */
};


/* remplacement des diacritics */
static unsigned char *add_diacritics [] = {
#ifdef latin1
  (unsigned char*) "A", (unsigned char*) "�����",
  (unsigned char*) "a", (unsigned char*) "�����",

  (unsigned char*) "E", (unsigned char*) "����",
  (unsigned char*) "e", (unsigned char*) "����",

  (unsigned char*) "I", (unsigned char*) "����",
  (unsigned char*) "i", (unsigned char*) "����",

  (unsigned char*) "O", (unsigned char*) "���",
  (unsigned char*) "o", (unsigned char*) "���",

  (unsigned char*) "U", (unsigned char*) "���",
  (unsigned char*) "u", (unsigned char*) "���",

  (unsigned char*) "Y", (unsigned char*) "��",
  (unsigned char*) "y", (unsigned char*) "��",

  (unsigned char*) "C", (unsigned char*) "�",
  (unsigned char*) "c", (unsigned char*) "�",
#endif /* latin1 */
#ifdef latin2
  (unsigned char*) "A", (unsigned char*) "�����",
  (unsigned char*) "a", (unsigned char*) "�����",

  (unsigned char*) "C", (unsigned char*) "���",
  (unsigned char*) "c", (unsigned char*) "���",

  (unsigned char*) "D", (unsigned char*) "��",
  (unsigned char*) "d", (unsigned char*) "��",

  (unsigned char*) "E", (unsigned char*) "����",
  (unsigned char*) "e", (unsigned char*) "����",

  (unsigned char*) "I", (unsigned char*) "��",
  (unsigned char*) "i", (unsigned char*) "��",

  (unsigned char*) "L", (unsigned char*) "���",
  (unsigned char*) "l", (unsigned char*) "���",

  (unsigned char*) "N", (unsigned char*) "��",
  (unsigned char*) "n", (unsigned char*) "��",

  (unsigned char*) "O", (unsigned char*) "����",
  (unsigned char*) "o", (unsigned char*) "����",

  (unsigned char*) "R", (unsigned char*) "��",
  (unsigned char*) "r", (unsigned char*) "��",

  (unsigned char*) "S", (unsigned char*) "���",
  (unsigned char*) "s", (unsigned char*) "���",

  (unsigned char*) "T", (unsigned char*) "��",
  (unsigned char*) "t", (unsigned char*) "��",

  (unsigned char*) "U", (unsigned char*) "����",
  (unsigned char*) "u", (unsigned char*) "����",

  (unsigned char*) "Y", (unsigned char*) "y�",
  (unsigned char*) "y", (unsigned char*) "�",

  (unsigned char*) "Z", (unsigned char*) "���",
  (unsigned char*) "z", (unsigned char*) "���",
#endif /* latin2 */
#ifdef koi8r
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
#endif /* koi8r */
};

/* remplacement des diacritics */
static unsigned char *change_diacritics [] = {
#ifdef latin1
  (unsigned char*) "�", (unsigned char*) "�����",
  (unsigned char*) "�", (unsigned char*) "�����",
  (unsigned char*) "�", (unsigned char*) "�����",
  (unsigned char*) "�", (unsigned char*) "�����",
  (unsigned char*) "�", (unsigned char*) "����",
  (unsigned char*) "�", (unsigned char*) "����",
  (unsigned char*) "�", (unsigned char*) "����",
  (unsigned char*) "�", (unsigned char*) "����",

  (unsigned char*) "�", (unsigned char*) "����",
  (unsigned char*) "�", (unsigned char*) "����",
  (unsigned char*) "�", (unsigned char*) "����",
  (unsigned char*) "�", (unsigned char*) "����",
  (unsigned char*) "�", (unsigned char*) "���",
  (unsigned char*) "�", (unsigned char*) "���",
  (unsigned char*) "�", (unsigned char*) "���",
  (unsigned char*) "�", (unsigned char*) "���",
  //  (unsigned char*) "�", (unsigned char*) "�",

  (unsigned char*) "�", (unsigned char*) "����",
  (unsigned char*) "�", (unsigned char*) "����",
  (unsigned char*) "�", (unsigned char*) "����",
  (unsigned char*) "�", (unsigned char*) "����",
  (unsigned char*) "�", (unsigned char*) "���",
  (unsigned char*) "�", (unsigned char*) "���",
  (unsigned char*) "�", (unsigned char*) "���",
  (unsigned char*) "�", (unsigned char*) "���",

  (unsigned char*) "�", (unsigned char*) "���",
  (unsigned char*) "�", (unsigned char*) "��",
  (unsigned char*) "�", (unsigned char*) "��",
  (unsigned char*) "�", (unsigned char*) "��",

  (unsigned char*) "�", (unsigned char*) "����",
  (unsigned char*) "�", (unsigned char*) "����",
  (unsigned char*) "�", (unsigned char*) "����",
  (unsigned char*) "�", (unsigned char*) "����",
  (unsigned char*) "�", (unsigned char*) "���",
  (unsigned char*) "�", (unsigned char*) "���",
  (unsigned char*) "�", (unsigned char*) "���",
  (unsigned char*) "�", (unsigned char*) "���",

  (unsigned char*) "�", (unsigned char*) "��",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",

#endif /* latin1 */
#ifdef latin2
  (unsigned char*) "�", (unsigned char*) "�����",
  (unsigned char*) "�", (unsigned char*) "�����",
  (unsigned char*) "�", (unsigned char*) "�����",
  (unsigned char*) "�", (unsigned char*) "�����",
  (unsigned char*) "�", (unsigned char*) "�����",
  (unsigned char*) "�", (unsigned char*) "����",
  (unsigned char*) "�", (unsigned char*) "����",
  (unsigned char*) "�", (unsigned char*) "����",
  (unsigned char*) "�", (unsigned char*) "����",
  (unsigned char*) "�", (unsigned char*) "����",

  (unsigned char*) "�", (unsigned char*) "���",
  (unsigned char*) "�", (unsigned char*) "���",
  (unsigned char*) "�", (unsigned char*) "���",
  (unsigned char*) "�", (unsigned char*) "��",
  (unsigned char*) "�", (unsigned char*) "��",
  (unsigned char*) "�", (unsigned char*) "��",

  (unsigned char*) "�", (unsigned char*) "��",
  (unsigned char*) "�", (unsigned char*) "��",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",

  (unsigned char*) "�", (unsigned char*) "����",
  (unsigned char*) "�", (unsigned char*) "����",
  (unsigned char*) "�", (unsigned char*) "����",
  (unsigned char*) "�", (unsigned char*) "����",
  (unsigned char*) "�", (unsigned char*) "���",
  (unsigned char*) "�", (unsigned char*) "���",
  (unsigned char*) "�", (unsigned char*) "���",
  (unsigned char*) "�", (unsigned char*) "���",

  (unsigned char*) "�", (unsigned char*) "��",
  (unsigned char*) "�", (unsigned char*) "��",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",

  (unsigned char*) "�", (unsigned char*) "���",
  (unsigned char*) "�", (unsigned char*) "���",
  (unsigned char*) "�", (unsigned char*) "���",
  (unsigned char*) "�", (unsigned char*) "��",
  (unsigned char*) "�", (unsigned char*) "��",
  (unsigned char*) "�", (unsigned char*) "��",

  (unsigned char*) "�", (unsigned char*) "��",
  (unsigned char*) "�", (unsigned char*) "��",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",

  (unsigned char*) "�", (unsigned char*) "����",
  (unsigned char*) "�", (unsigned char*) "����",
  (unsigned char*) "�", (unsigned char*) "����",
  (unsigned char*) "�", (unsigned char*) "����",
  (unsigned char*) "�", (unsigned char*) "���",
  (unsigned char*) "�", (unsigned char*) "���",
  (unsigned char*) "�", (unsigned char*) "���",
  (unsigned char*) "�", (unsigned char*) "���",

  (unsigned char*) "�", (unsigned char*) "��",
  (unsigned char*) "�", (unsigned char*) "��",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",

  (unsigned char*) "�", (unsigned char*) "���",
  (unsigned char*) "�", (unsigned char*) "���",
  (unsigned char*) "�", (unsigned char*) "���",
  (unsigned char*) "�", (unsigned char*) "��",
  (unsigned char*) "�", (unsigned char*) "��",
  (unsigned char*) "�", (unsigned char*) "��",

  (unsigned char*) "�", (unsigned char*) "��",
  (unsigned char*) "�", (unsigned char*) "��",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",

  (unsigned char*) "�", (unsigned char*) "����",
  (unsigned char*) "�", (unsigned char*) "����",
  (unsigned char*) "�", (unsigned char*) "����",
  (unsigned char*) "�", (unsigned char*) "����",
  (unsigned char*) "�", (unsigned char*) "���",
  (unsigned char*) "�", (unsigned char*) "���",
  (unsigned char*) "�", (unsigned char*) "���",
  (unsigned char*) "�", (unsigned char*) "���",

  (unsigned char*) "�", (unsigned char*) "���",
  (unsigned char*) "�", (unsigned char*) "���",
  (unsigned char*) "�", (unsigned char*) "���",
  (unsigned char*) "�", (unsigned char*) "��",
  (unsigned char*) "�", (unsigned char*) "��",
  (unsigned char*) "�", (unsigned char*) "��",
#endif /* latin2 */
};

/* remplacement des diacritics */
static unsigned char *remove_diacritics [] = {
#ifdef latin1
  (unsigned char*) "�", (unsigned char*) "a",
  (unsigned char*) "�", (unsigned char*) "a",
  (unsigned char*) "�", (unsigned char*) "a",
  (unsigned char*) "�", (unsigned char*) "a",
  (unsigned char*) "�", (unsigned char*) "a",
  (unsigned char*) "�", (unsigned char*) "a",
  (unsigned char*) "�", (unsigned char*) "a",
  (unsigned char*) "�", (unsigned char*) "a",

  (unsigned char*) "�", (unsigned char*) "e",
  (unsigned char*) "�", (unsigned char*) "e",
  (unsigned char*) "�", (unsigned char*) "e",
  (unsigned char*) "�", (unsigned char*) "e",
  (unsigned char*) "�", (unsigned char*) "e",
  (unsigned char*) "�", (unsigned char*) "e",
  (unsigned char*) "�", (unsigned char*) "e",
  (unsigned char*) "�", (unsigned char*) "e",
  (unsigned char*) "�", (unsigned char*) "e",

  (unsigned char*) "�", (unsigned char*) "i",
  (unsigned char*) "�", (unsigned char*) "i",
  (unsigned char*) "�", (unsigned char*) "i",
  (unsigned char*) "�", (unsigned char*) "i",
  (unsigned char*) "�", (unsigned char*) "i",
  (unsigned char*) "�", (unsigned char*) "i",
  (unsigned char*) "�", (unsigned char*) "i",
  (unsigned char*) "�", (unsigned char*) "i",

  (unsigned char*) "�", (unsigned char*) "o",
  (unsigned char*) "�", (unsigned char*) "o",
  (unsigned char*) "�", (unsigned char*) "o",
  (unsigned char*) "�", (unsigned char*) "o",
  (unsigned char*) "�", (unsigned char*) "o",
  (unsigned char*) "�", (unsigned char*) "o",

  (unsigned char*) "�", (unsigned char*) "u",
  (unsigned char*) "�", (unsigned char*) "u",
  (unsigned char*) "�", (unsigned char*) "u",
  (unsigned char*) "�", (unsigned char*) "u",
  (unsigned char*) "�", (unsigned char*) "u",
  (unsigned char*) "�", (unsigned char*) "u",
  (unsigned char*) "�", (unsigned char*) "u",
  (unsigned char*) "�", (unsigned char*) "u",

  (unsigned char*) "�", (unsigned char*) "y",
  (unsigned char*) "�", (unsigned char*) "y",

  (unsigned char*) "�", (unsigned char*) "c",
  (unsigned char*) "�", (unsigned char*) "c",
#endif /* latin1 */
#ifdef latin2
  (unsigned char*) "�", (unsigned char*) "a",
  (unsigned char*) "�", (unsigned char*) "a",
  (unsigned char*) "�", (unsigned char*) "a",
  (unsigned char*) "�", (unsigned char*) "a",
  (unsigned char*) "�", (unsigned char*) "a",
  (unsigned char*) "�", (unsigned char*) "a",
  (unsigned char*) "�", (unsigned char*) "a",
  (unsigned char*) "�", (unsigned char*) "a",
  (unsigned char*) "�", (unsigned char*) "a",
  (unsigned char*) "�", (unsigned char*) "a",

  (unsigned char*) "�", (unsigned char*) "c",
  (unsigned char*) "�", (unsigned char*) "c",
  (unsigned char*) "�", (unsigned char*) "c",
  (unsigned char*) "�", (unsigned char*) "c",
  (unsigned char*) "�", (unsigned char*) "c",
  (unsigned char*) "�", (unsigned char*) "c",

  (unsigned char*) "�", (unsigned char*) "d",
  (unsigned char*) "�", (unsigned char*) "d",
  (unsigned char*) "�", (unsigned char*) "d",
  (unsigned char*) "�", (unsigned char*) "d",

  (unsigned char*) "�", (unsigned char*) "e",
  (unsigned char*) "�", (unsigned char*) "e",
  (unsigned char*) "�", (unsigned char*) "e",
  (unsigned char*) "�", (unsigned char*) "e",
  (unsigned char*) "�", (unsigned char*) "e",
  (unsigned char*) "�", (unsigned char*) "e",
  (unsigned char*) "�", (unsigned char*) "e",
  (unsigned char*) "�", (unsigned char*) "e",

  (unsigned char*) "�", (unsigned char*) "i",
  (unsigned char*) "�", (unsigned char*) "i",
  (unsigned char*) "�", (unsigned char*) "i",
  (unsigned char*) "�", (unsigned char*) "i",

  (unsigned char*) "�", (unsigned char*) "l",
  (unsigned char*) "�", (unsigned char*) "l",
  (unsigned char*) "�", (unsigned char*) "l",
  (unsigned char*) "�", (unsigned char*) "l",
  (unsigned char*) "�", (unsigned char*) "l",
  (unsigned char*) "�", (unsigned char*) "l",

  (unsigned char*) "�", (unsigned char*) "n",
  (unsigned char*) "�", (unsigned char*) "n",
  (unsigned char*) "�", (unsigned char*) "n",
  (unsigned char*) "�", (unsigned char*) "n",

  (unsigned char*) "�", (unsigned char*) "o",
  (unsigned char*) "�", (unsigned char*) "o",
  (unsigned char*) "�", (unsigned char*) "o",
  (unsigned char*) "�", (unsigned char*) "o",
  (unsigned char*) "�", (unsigned char*) "o",
  (unsigned char*) "�", (unsigned char*) "o",
  (unsigned char*) "�", (unsigned char*) "o",
  (unsigned char*) "�", (unsigned char*) "o",

  (unsigned char*) "�", (unsigned char*) "r",
  (unsigned char*) "�", (unsigned char*) "r",
  (unsigned char*) "�", (unsigned char*) "r",
  (unsigned char*) "�", (unsigned char*) "r",

  (unsigned char*) "�", (unsigned char*) "s",
  (unsigned char*) "�", (unsigned char*) "s",
  (unsigned char*) "�", (unsigned char*) "s",
  (unsigned char*) "�", (unsigned char*) "s",
  (unsigned char*) "�", (unsigned char*) "s",
  (unsigned char*) "�", (unsigned char*) "s",

  (unsigned char*) "�", (unsigned char*) "t",
  (unsigned char*) "�", (unsigned char*) "t",
  (unsigned char*) "�", (unsigned char*) "t",
  (unsigned char*) "�", (unsigned char*) "t",

  (unsigned char*) "�", (unsigned char*) "u",
  (unsigned char*) "�", (unsigned char*) "u",
  (unsigned char*) "�", (unsigned char*) "u",
  (unsigned char*) "�", (unsigned char*) "u",
  (unsigned char*) "�", (unsigned char*) "u",
  (unsigned char*) "�", (unsigned char*) "u",
  (unsigned char*) "�", (unsigned char*) "u",
  (unsigned char*) "�", (unsigned char*) "u",

  (unsigned char*) "�", (unsigned char*) "y",
  (unsigned char*) "�", (unsigned char*) "y",

  (unsigned char*) "�", (unsigned char*) "z",
  (unsigned char*) "�", (unsigned char*) "z",
  (unsigned char*) "�", (unsigned char*) "z",
  (unsigned char*) "�", (unsigned char*) "z",
  (unsigned char*) "�", (unsigned char*) "z",
  (unsigned char*) "�", (unsigned char*) "z",
#endif /* latin2 */
#ifdef koi8r
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
  (unsigned char*) "�", (unsigned char*) "�",
#endif /* koi8r */
};

/* Doublement de lettres ... */
static unsigned char *m_en_mm [] = {
#if defined(latin1) || defined(latin2)
  (unsigned char*) "b", (unsigned char*) "bb",
  (unsigned char*) "c", (unsigned char*) "cc",
  (unsigned char*) "d", (unsigned char*) "dd",
  (unsigned char*) "�", (unsigned char*) "��",
  (unsigned char*) "f", (unsigned char*) "ff",
  (unsigned char*) "g", (unsigned char*) "gg",
  (unsigned char*) "l", (unsigned char*) "ll",
  (unsigned char*) "m", (unsigned char*) "mm",
  (unsigned char*) "n", (unsigned char*) "nn",
  (unsigned char*) "o", (unsigned char*) "oo",
  (unsigned char*) "p", (unsigned char*) "pp",
  (unsigned char*) "r", (unsigned char*) "rr",
  (unsigned char*) "s", (unsigned char*) "ss",
  (unsigned char*) "t", (unsigned char*) "tt",
  (unsigned char*) "z", (unsigned char*) "zz",
#endif /* latin1 || latin2 */
};

/* ... et pas de doublement */
static unsigned char *mm_en_m [] = {
#if defined(latin1) || defined(latin2)
  (unsigned char*) "aa", (unsigned char*) "a",
  (unsigned char*) "bb", (unsigned char*) "b",
  (unsigned char*) "cc", (unsigned char*) "c",
  (unsigned char*) "dd", (unsigned char*) "d",
  (unsigned char*) "ee", (unsigned char*) "e",
  (unsigned char*) "ff", (unsigned char*) "f",
  (unsigned char*) "gg", (unsigned char*) "g",
  (unsigned char*) "hh", (unsigned char*) "h",
  (unsigned char*) "ii", (unsigned char*) "i",
  (unsigned char*) "jj", (unsigned char*) "j",
  (unsigned char*) "kk", (unsigned char*) "k",
  (unsigned char*) "ll", (unsigned char*) "l",
  (unsigned char*) "mm", (unsigned char*) "m",
  (unsigned char*) "nn", (unsigned char*) "n",
  (unsigned char*) "oo", (unsigned char*) "o",
  (unsigned char*) "pp", (unsigned char*) "p",
  (unsigned char*) "qq", (unsigned char*) "q",
  (unsigned char*) "rr", (unsigned char*) "r",
  (unsigned char*) "ss", (unsigned char*) "s",
  (unsigned char*) "tt", (unsigned char*) "t",
  (unsigned char*) "uu", (unsigned char*) "u",
  (unsigned char*) "vv", (unsigned char*) "v",
  (unsigned char*) "ww", (unsigned char*) "w",
  (unsigned char*) "xx", (unsigned char*) "x",
  (unsigned char*) "yy", (unsigned char*) "y",
  (unsigned char*) "zz", (unsigned char*) "z",
#endif /* latin1 || latin2 */
#ifdef latin1
  (unsigned char*) "��", (unsigned char*) "�",
  (unsigned char*) "��", (unsigned char*) "�",
  (unsigned char*) "��", (unsigned char*) "�",
  (unsigned char*) "��", (unsigned char*) "�",
  (unsigned char*) "��", (unsigned char*) "�",
  (unsigned char*) "��", (unsigned char*) "�",
  (unsigned char*) "��", (unsigned char*) "�",
  (unsigned char*) "��", (unsigned char*) "�",
  (unsigned char*) "��", (unsigned char*) "�",
  (unsigned char*) "��", (unsigned char*) "�",
  (unsigned char*) "��", (unsigned char*) "�",
  (unsigned char*) "��", (unsigned char*) "�",
  (unsigned char*) "��", (unsigned char*) "�",
#endif /* latin1 */
};


/* ... et qq_trucs */
static unsigned char *qq_trucs [] = {
#if defined(latin1) || defined(latin2)
  (unsigned char*) "@", (unsigned char*) "a",
  (unsigned char*) "@", (unsigned char*) "�",
  (unsigned char*) "@", (unsigned char*) "",
#endif /* latin1 || latin2 */
#ifdef latin1
  (unsigned char*) "K", (unsigned char*) "ca",
#endif /* latin1 */
};


/* Corrections issues de text2dag_semact */
#if IS_LIGHT_CORRECTION == 1
/* Capital_initial */
static unsigned char *capital_initial2min_diacritic [] = {
#ifdef latin1
  (unsigned char*) "^A", (unsigned char*) "�",
  (unsigned char*) "^A", (unsigned char*) "�",

  (unsigned char*) "^E", (unsigned char*) "�",
  (unsigned char*) "^E", (unsigned char*) "�",
  (unsigned char*) "^E", (unsigned char*) "�",
  (unsigned char*) "^E", (unsigned char*) "�",

  (unsigned char*) "^I", (unsigned char*) "�",
  (unsigned char*) "^I", (unsigned char*) "�",
  (unsigned char*) "^I", (unsigned char*) "�",
  (unsigned char*) "^I", (unsigned char*) "�",

  (unsigned char*) "^O", (unsigned char*) "�",
  (unsigned char*) "^O", (unsigned char*) "�",
  (unsigned char*) "^O", (unsigned char*) "�",

  (unsigned char*) "^U", (unsigned char*) "�",
  (unsigned char*) "^U", (unsigned char*) "�",
  (unsigned char*) "^U", (unsigned char*) "�",
  (unsigned char*) "^U", (unsigned char*) "�",

  (unsigned char*) "^Y", (unsigned char*) "�",
  (unsigned char*) "^Y", (unsigned char*) "�",
  
  (unsigned char*) "^C", (unsigned char*) "�",
#endif /* latin1 */
};
/* Changement de casse */
static unsigned char *capital_initial2min [] = {
#ifdef latin1
  (unsigned char*) "^A", (unsigned char*) "a",
  (unsigned char*) "^B", (unsigned char*) "b",
  (unsigned char*) "^C", (unsigned char*) "c",
  (unsigned char*) "^D", (unsigned char*) "d",
  (unsigned char*) "^E", (unsigned char*) "e",
  (unsigned char*) "^F", (unsigned char*) "f",
  (unsigned char*) "^G", (unsigned char*) "g",
  (unsigned char*) "^H", (unsigned char*) "h",
  (unsigned char*) "^I", (unsigned char*) "i",
  (unsigned char*) "^J", (unsigned char*) "j",
  (unsigned char*) "^K", (unsigned char*) "k",
  (unsigned char*) "^L", (unsigned char*) "l",
  (unsigned char*) "^M", (unsigned char*) "m",
  (unsigned char*) "^N", (unsigned char*) "n",
  (unsigned char*) "^O", (unsigned char*) "o",
  (unsigned char*) "^P", (unsigned char*) "p",
  (unsigned char*) "^Q", (unsigned char*) "q",
  (unsigned char*) "^R", (unsigned char*) "r",
  (unsigned char*) "^S", (unsigned char*) "s",
  (unsigned char*) "^T", (unsigned char*) "t",
  (unsigned char*) "^U", (unsigned char*) "u",
  (unsigned char*) "^V", (unsigned char*) "v",
  (unsigned char*) "^W", (unsigned char*) "w",
  (unsigned char*) "^X", (unsigned char*) "x",
  (unsigned char*) "^Y", (unsigned char*) "y",
  (unsigned char*) "^Z", (unsigned char*) "z",
  (unsigned char*) "^�", (unsigned char*) "�",
  (unsigned char*) "^�", (unsigned char*) "�",
  (unsigned char*) "^�", (unsigned char*) "�",
  (unsigned char*) "^�", (unsigned char*) "�",
  (unsigned char*) "^�", (unsigned char*) "�",
  (unsigned char*) "^�", (unsigned char*) "�",
  (unsigned char*) "^�", (unsigned char*) "�",
  (unsigned char*) "^�", (unsigned char*) "�",
  (unsigned char*) "^�", (unsigned char*) "�",
  (unsigned char*) "^�", (unsigned char*) "�",
  (unsigned char*) "^�", (unsigned char*) "�",
  (unsigned char*) "^�", (unsigned char*) "�",
  (unsigned char*) "^�", (unsigned char*) "�",
  (unsigned char*) "^�", (unsigned char*) "�",
  (unsigned char*) "^�", (unsigned char*) "�",
  (unsigned char*) "^�", (unsigned char*) "�",
  (unsigned char*) "^�", (unsigned char*) "�",
  (unsigned char*) "^�", (unsigned char*) "�",
  (unsigned char*) "^�", (unsigned char*) "�",
  (unsigned char*) "^�", (unsigned char*) "�",
  (unsigned char*) "^�", (unsigned char*) "�",
  (unsigned char*) "^�", (unsigned char*) "�",
  (unsigned char*) "^�", (unsigned char*) "�",
  (unsigned char*) "^�", (unsigned char*) "�",
  (unsigned char*) "^�", (unsigned char*) "�",
  (unsigned char*) "^�", (unsigned char*) "�",
  (unsigned char*) "^�", (unsigned char*) "�",
  (unsigned char*) "^�", (unsigned char*) "�",
  (unsigned char*) "^�", (unsigned char*) "�",
  (unsigned char*) "^�", (unsigned char*) "�",
#endif /* latin1 */
};
#endif /* IS_LIGHT_CORRECTION == 1 */

