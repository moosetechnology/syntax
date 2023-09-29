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
static unsigned char *MAJ_string = (unsigned char *) "ABCDEFGHIJKLMNOPQRSTUVWXYZÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏĞÑÒÓÔÕÖØÙÚÛÜİŞ";
static unsigned char *MIN_string = (unsigned char *) "abcdefghijklmnopqrstuvwxyzàáâãäåæçèéêëìíîïğñòóôõöøùúûüışß";
static unsigned char *vowell_string = (unsigned char*) "AaEeIiOoUuYyÀàÁáÂâÃãÄäÅåÆæÈèÉéÊêËëÌìÍíÎîÏïÒòÓóÔôÕõÖöØøÙùÚúÛûÜüİı";
static unsigned char *mpb_string = (unsigned char*) "mpb";
#endif /* latin1 */
#ifdef latin2
static unsigned char *MAJ_string = (unsigned char*) "ABCDEFGHIJKLMNOPQRSTUVWXYZ¡£¥¦©ª«¬®¯ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏĞÑÒÓÔÕÖØÙÚÛÜİŞ";
static unsigned char *MIN_string = (unsigned char*) "abcdefghijklmnopqrstuvwxyz±³µ¶¹º»¼¾¿àáâãäåæçèéêëìíîïğñòóôõöøùúûüış";
static unsigned char *vowell_string = (unsigned char*) "AaEeIiOoUuYy¡±ÁáÂâÃãÄäÉéÊêËëÌìÍíÎîÓóÕõÔôÖöÙùÚúÛûÜüİı";
static unsigned char *mpb_string = (unsigned char*) "mpb";
#endif /* latin2 */
#ifdef koi8r
static unsigned char *MAJ_string = (unsigned char*) "áâ÷çäåöúéêëìíîïğòóôõæèãşûıüàñùø³";
static unsigned char *MIN_string = (unsigned char*) "ÁÂ×ÇÄÅÖÚÉÊËÌÍÎÏĞÒÓÔÕÆÈÃŞÛİÜÀÑÙØ£";
static unsigned char *vowell_string = (unsigned char*) "áåéêïõüàñÁÅÉÊÏÕÙÜÀÑ£";
static unsigned char *mpb_string = (unsigned char*) "mpb";
#endif /* koi8r */

#ifdef latin1
static unsigned char *insert_string = (unsigned char*) "abcdefghijklmnopqrstuvwxyzâçèéêëîïôöûü-";
static unsigned char *change_string = (unsigned char*) "abcdefghijklmnopqrstuvwxyzâçèéêëîïôöûü";
#endif /* latin1 */
#ifdef latin2
static unsigned char *insert_string = (unsigned char*) "abcdefghijklmnopqrstuvwxyz±³µ¶¹º»¼¾¿àáâãäåæçèéêëìíîïğñòóôõöøùúûüış-";
static unsigned char *change_string = (unsigned char*) "abcdefghijklmnopqrstuvwxyz±³µ¶¹º»¼¾¿àáâãäåæçèéêëìíîïğñòóôõöøùúûüış";
#endif /* latin2 */
#ifdef koi8r
static unsigned char *insert_string = (unsigned char*) "ÁÂ×ÇÄÅÖÚÉÊËÌÍÎÏĞÒÓÔÕÆÈÃŞÛİßÙØÜÀÑ£-";
static unsigned char *change_string = (unsigned char*) "ÁÂ×ÇÄÅÖÚÉÊËÌÍÎÏĞÒÓÔÕÆÈÃŞÛİßÙØÜÀÑ£";
#endif /* koi8r */

#define light_compose_weight            50
#define super_light_compose_weight      50
#define compose_weight	                50

#define detach_weight                   40

/* Poids de chaque type de correction */
/* Si le poids est négatif ou nul le coût de la composition est nul */
// utilisées dans sxspell/text2dag
// 1. étaient dans sxspeller.c
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
// 2. étaient dans sxspell.h
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
  (unsigned char*) "À", (unsigned char*) "à",
  (unsigned char*) "Á", (unsigned char*) "á",
  (unsigned char*) "Â", (unsigned char*) "â",
  (unsigned char*) "Ã", (unsigned char*) "ã",
  (unsigned char*) "Ä", (unsigned char*) "ä",
  (unsigned char*) "Æ", (unsigned char*) "æ",
  (unsigned char*) "Å", (unsigned char*) "å",
  (unsigned char*) "Ç", (unsigned char*) "ç",
  (unsigned char*) "È", (unsigned char*) "è",
  (unsigned char*) "É", (unsigned char*) "é",
  (unsigned char*) "Ê", (unsigned char*) "ê",
  (unsigned char*) "Ë", (unsigned char*) "ë",
  (unsigned char*) "Ì", (unsigned char*) "ì",
  (unsigned char*) "Í", (unsigned char*) "í",
  (unsigned char*) "Î", (unsigned char*) "î",
  (unsigned char*) "Ï", (unsigned char*) "ï",
  (unsigned char*) "Ğ", (unsigned char*) "ğ",
  (unsigned char*) "Ñ", (unsigned char*) "ñ",
  (unsigned char*) "Ò", (unsigned char*) "ò",
  (unsigned char*) "Ó", (unsigned char*) "ó",
  (unsigned char*) "Ô", (unsigned char*) "ô",
  (unsigned char*) "Õ", (unsigned char*) "õ",
  (unsigned char*) "Ö", (unsigned char*) "ö",
  (unsigned char*) "Ø", (unsigned char*) "ø",
  (unsigned char*) "Ù", (unsigned char*) "ù",
  (unsigned char*) "Ú", (unsigned char*) "ú",
  (unsigned char*) "Û", (unsigned char*) "û",
  (unsigned char*) "Ü", (unsigned char*) "ü",
  (unsigned char*) "İ", (unsigned char*) "ı",
  (unsigned char*) "Ş", (unsigned char*) "ş",
#endif /* latin1 */
#ifdef latin2
  (unsigned char*) "¡", (unsigned char*) "±",
  (unsigned char*) "£", (unsigned char*) "³",
  (unsigned char*) "¥", (unsigned char*) "µ",
  (unsigned char*) "¦", (unsigned char*) "¶",
  (unsigned char*) "©", (unsigned char*) "¹",
  (unsigned char*) "ª", (unsigned char*) "º",
  (unsigned char*) "«", (unsigned char*) "»",
  (unsigned char*) "¬", (unsigned char*) "¼",
  (unsigned char*) "®", (unsigned char*) "¾",
  (unsigned char*) "¯", (unsigned char*) "¿",
  (unsigned char*) "À", (unsigned char*) "à",
  (unsigned char*) "Á", (unsigned char*) "á",
  (unsigned char*) "Â", (unsigned char*) "â",
  (unsigned char*) "Ã", (unsigned char*) "ã",
  (unsigned char*) "Ä", (unsigned char*) "ä",
  (unsigned char*) "Å", (unsigned char*) "å",
  (unsigned char*) "Æ", (unsigned char*) "æ",
  (unsigned char*) "Ç", (unsigned char*) "ç",
  (unsigned char*) "È", (unsigned char*) "è",
  (unsigned char*) "É", (unsigned char*) "é",
  (unsigned char*) "Ê", (unsigned char*) "ê",
  (unsigned char*) "Ë", (unsigned char*) "ë",
  (unsigned char*) "Ì", (unsigned char*) "ì",
  (unsigned char*) "Í", (unsigned char*) "í",
  (unsigned char*) "Î", (unsigned char*) "î",
  (unsigned char*) "Ï", (unsigned char*) "ï",
  (unsigned char*) "Ğ", (unsigned char*) "ğ",
  (unsigned char*) "Ñ", (unsigned char*) "ñ",
  (unsigned char*) "Ò", (unsigned char*) "ò",
  (unsigned char*) "Ó", (unsigned char*) "ó",
  (unsigned char*) "Ô", (unsigned char*) "ô",
  (unsigned char*) "Õ", (unsigned char*) "õ",
  (unsigned char*) "Ö", (unsigned char*) "ö",
  (unsigned char*) "Ø", (unsigned char*) "ø",
  (unsigned char*) "Ù", (unsigned char*) "ù",
  (unsigned char*) "Ú", (unsigned char*) "ú",
  (unsigned char*) "Û", (unsigned char*) "û",
  (unsigned char*) "Ü", (unsigned char*) "ü",
  (unsigned char*) "İ", (unsigned char*) "ı",
  (unsigned char*) "Ş", (unsigned char*) "ş",
#endif /* latin2 */
#ifdef koi8r
  (unsigned char*) "á", (unsigned char*) "Á",
  (unsigned char*) "â", (unsigned char*) "Â",
  (unsigned char*) "÷", (unsigned char*) "×",
  (unsigned char*) "ç", (unsigned char*) "Ç",
  (unsigned char*) "ä", (unsigned char*) "Ä",
  (unsigned char*) "å", (unsigned char*) "Å",
  (unsigned char*) "ö", (unsigned char*) "Ö",
  (unsigned char*) "ú", (unsigned char*) "Ú",
  (unsigned char*) "é", (unsigned char*) "É",
  (unsigned char*) "ê", (unsigned char*) "Ê",
  (unsigned char*) "ë", (unsigned char*) "Ë",
  (unsigned char*) "ì", (unsigned char*) "Ì",
  (unsigned char*) "í", (unsigned char*) "Í",
  (unsigned char*) "î", (unsigned char*) "Î",
  (unsigned char*) "ï", (unsigned char*) "Ï",
  (unsigned char*) "ğ", (unsigned char*) "Ğ",
  (unsigned char*) "ò", (unsigned char*) "Ò",
  (unsigned char*) "ó", (unsigned char*) "Ó",
  (unsigned char*) "ô", (unsigned char*) "Ô",
  (unsigned char*) "õ", (unsigned char*) "Õ",
  (unsigned char*) "æ", (unsigned char*) "Æ",
  (unsigned char*) "è", (unsigned char*) "È",
  (unsigned char*) "ã", (unsigned char*) "Ã",
  (unsigned char*) "ş", (unsigned char*) "Ş",
  (unsigned char*) "û", (unsigned char*) "Û",
  (unsigned char*) "ı", (unsigned char*) "İ",
  (unsigned char*) "ü", (unsigned char*) "Ü",
  (unsigned char*) "à", (unsigned char*) "À",
  (unsigned char*) "ñ", (unsigned char*) "Ñ",
  (unsigned char*) "ù", (unsigned char*) "Ù",
  (unsigned char*) "ø", (unsigned char*) "Ø",
  (unsigned char*) "³", (unsigned char*) "£",
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
  (unsigned char*) "à", (unsigned char*) "À",
  (unsigned char*) "á", (unsigned char*) "Á",
  (unsigned char*) "â", (unsigned char*) "Â",
  (unsigned char*) "ã", (unsigned char*) "Ã",
  (unsigned char*) "ä", (unsigned char*) "Ä",
  (unsigned char*) "å", (unsigned char*) "Å",
  (unsigned char*) "æ", (unsigned char*) "Æ",
  (unsigned char*) "ç", (unsigned char*) "Ç",
  (unsigned char*) "è", (unsigned char*) "È",
  (unsigned char*) "é", (unsigned char*) "É",
  (unsigned char*) "ê", (unsigned char*) "Ê",
  (unsigned char*) "ë", (unsigned char*) "Ë",
  (unsigned char*) "ì", (unsigned char*) "Ì",
  (unsigned char*) "í", (unsigned char*) "Í",
  (unsigned char*) "î", (unsigned char*) "Î",
  (unsigned char*) "ï", (unsigned char*) "Ï",
  (unsigned char*) "ğ", (unsigned char*) "Ğ",
  (unsigned char*) "ñ", (unsigned char*) "Ñ",
  (unsigned char*) "ò", (unsigned char*) "Ò",
  (unsigned char*) "ó", (unsigned char*) "Ó",
  (unsigned char*) "ô", (unsigned char*) "Ô",
  (unsigned char*) "õ", (unsigned char*) "Õ",
  (unsigned char*) "ö", (unsigned char*) "Ö",
  (unsigned char*) "ø", (unsigned char*) "Ø",
  (unsigned char*) "ù", (unsigned char*) "Ù",
  (unsigned char*) "ú", (unsigned char*) "Ú",
  (unsigned char*) "û", (unsigned char*) "Û",
  (unsigned char*) "ü", (unsigned char*) "Ü",
  (unsigned char*) "ı", (unsigned char*) "İ",
  (unsigned char*) "ş", (unsigned char*) "Ş",
#endif /* latin1 */
#ifdef koi8r
  (unsigned char*) "Á", (unsigned char*) "á",
  (unsigned char*) "Â", (unsigned char*) "â",
  (unsigned char*) "×", (unsigned char*) "÷",
  (unsigned char*) "Ç", (unsigned char*) "ç",
  (unsigned char*) "Ä", (unsigned char*) "ä",
  (unsigned char*) "Å", (unsigned char*) "å",
  (unsigned char*) "Ö", (unsigned char*) "ö",
  (unsigned char*) "Ú", (unsigned char*) "ú",
  (unsigned char*) "É", (unsigned char*) "é",
  (unsigned char*) "Ê", (unsigned char*) "ê",
  (unsigned char*) "Ë", (unsigned char*) "ë",
  (unsigned char*) "Ì", (unsigned char*) "ì",
  (unsigned char*) "Í", (unsigned char*) "í",
  (unsigned char*) "Î", (unsigned char*) "î",
  (unsigned char*) "Ï", (unsigned char*) "ï",
  (unsigned char*) "Ğ", (unsigned char*) "ğ",
  (unsigned char*) "Ò", (unsigned char*) "ò",
  (unsigned char*) "Ó", (unsigned char*) "ó",
  (unsigned char*) "Ô", (unsigned char*) "ô",
  (unsigned char*) "Õ", (unsigned char*) "õ",
  (unsigned char*) "Æ", (unsigned char*) "æ",
  (unsigned char*) "È", (unsigned char*) "è",
  (unsigned char*) "Ã", (unsigned char*) "ã",
  (unsigned char*) "Ş", (unsigned char*) "ş",
  (unsigned char*) "Û", (unsigned char*) "û",
  (unsigned char*) "İ", (unsigned char*) "ı",
  (unsigned char*) "Ü", (unsigned char*) "ü",
  (unsigned char*) "À", (unsigned char*) "à",
  (unsigned char*) "Ñ", (unsigned char*) "ñ",
  (unsigned char*) "Ù", (unsigned char*) "ù",
  (unsigned char*) "Ø", (unsigned char*) "ø",
  (unsigned char*) "£", (unsigned char*) "³",
#endif /* koi8r */
};


/* remplacement des diacritics avec changement de casse possible */
static unsigned char *diacritics_and_case [] = {
#ifdef latin1
  (unsigned char*) "A", (unsigned char*) "ÀÂÁÅaàâáå",
  (unsigned char*) "À", (unsigned char*) "AÂÁÅaàâáå",
  (unsigned char*) "Á", (unsigned char*) "AÀÂÅaàâáå",
  (unsigned char*) "Â", (unsigned char*) "AÀÁÅaàâáå",
  (unsigned char*) "Å", (unsigned char*) "AÀÂÁaàâáå",
  (unsigned char*) "a", (unsigned char*) "AÀÂÁÅàâáå",
  (unsigned char*) "à", (unsigned char*) "AÀÂÁÅaâáå",
  (unsigned char*) "á", (unsigned char*) "AÀÂÁÅaàâå",
  (unsigned char*) "â", (unsigned char*) "AÀÂÁÅaàáå",
  (unsigned char*) "å", (unsigned char*) "AÀÂÁÅaàâá",

  (unsigned char*) "E", (unsigned char*) "ÈÉÊËeèéêë",
  (unsigned char*) "È", (unsigned char*) "EÉÊËeèéêë",
  (unsigned char*) "É", (unsigned char*) "EÈÊËeèéêë",
  (unsigned char*) "Ê", (unsigned char*) "EÈÉËeèéêë",
  (unsigned char*) "Ë", (unsigned char*) "EÈÉÊeèéêë",
  (unsigned char*) "e", (unsigned char*) "EÈÉÊËèéêë",
  (unsigned char*) "è", (unsigned char*) "EÈÉÊËeéêë",
  (unsigned char*) "é", (unsigned char*) "EÈÉÊËeèêë",
  (unsigned char*) "ê", (unsigned char*) "EÈÉÊËeèéë",
  (unsigned char*) "ë", (unsigned char*) "EÈÉÊËeèéê",

  (unsigned char*) "I", (unsigned char*) "ÎÏÍÌiîïíì",
  (unsigned char*) "Î", (unsigned char*) "IÏÍÌiîïíì",
  (unsigned char*) "Ï", (unsigned char*) "IÎÍÌiîïíì",
  (unsigned char*) "Í", (unsigned char*) "IÎÏÌiîïíì",
  (unsigned char*) "Ì", (unsigned char*) "IÎÏÍiîïíì",
  (unsigned char*) "i", (unsigned char*) "IÎÏÍÌîïíì",
  (unsigned char*) "î", (unsigned char*) "IÎÏÍÌiïíì",
  (unsigned char*) "ï", (unsigned char*) "IÎÏÍÌiîíì",
  (unsigned char*) "í", (unsigned char*) "IÎÏÍÌiîïì",
  (unsigned char*) "ì", (unsigned char*) "IÎÏÍÌiîïí",

  (unsigned char*) "O", (unsigned char*) "ÔÓÒoôóò",
  (unsigned char*) "Ô", (unsigned char*) "OÓÒoôóò",
  (unsigned char*) "Ó", (unsigned char*) "OÔÒoôóò",
  (unsigned char*) "Ò", (unsigned char*) "OÔÓoôóò",
  (unsigned char*) "o", (unsigned char*) "OÔÓÒôóò",
  (unsigned char*) "ô", (unsigned char*) "OÔÓÒoóò",
  (unsigned char*) "ó", (unsigned char*) "OÔÓÒoôò",
  (unsigned char*) "ò", (unsigned char*) "OÔÓÒoôó",

  (unsigned char*) "U", (unsigned char*) "ÙÛÜÚuùûüú",
  (unsigned char*) "Ù", (unsigned char*) "UÛÜÚuùûüú",
  (unsigned char*) "Û", (unsigned char*) "UÙÜÚuùûüú",
  (unsigned char*) "Ü", (unsigned char*) "UÙÛÚuùûüú",
  (unsigned char*) "Ú", (unsigned char*) "UÙÛÜuùûüú",
  (unsigned char*) "u", (unsigned char*) "UÙÛÜÚùûüú",
  (unsigned char*) "ù", (unsigned char*) "UÙÛÜÚuûüú",
  (unsigned char*) "û", (unsigned char*) "UÙÛÜÚuùüú",
  (unsigned char*) "ü", (unsigned char*) "UÙÛÜÚuùûú",
  (unsigned char*) "ú", (unsigned char*) "UÙÛÜÚuùûü",

  (unsigned char*) "Y", (unsigned char*) "yÿ",
  (unsigned char*) "y", (unsigned char*) "Yÿ",
  (unsigned char*) "ÿ", (unsigned char*) "Yy",

  (unsigned char*) "N", (unsigned char*) "Ñnñ",
  (unsigned char*) "Ñ", (unsigned char*) "Nnñ",
  (unsigned char*) "n", (unsigned char*) "NÑñ",
  (unsigned char*) "ñ", (unsigned char*) "NÑn",

  (unsigned char*) "C", (unsigned char*) "Çcç",
  (unsigned char*) "Ç", (unsigned char*) "Ccç",
  (unsigned char*) "c", (unsigned char*) "CÇç",
  (unsigned char*) "ç", (unsigned char*) "CÇc",
#endif /* latin1 */
#ifdef koi8r
  (unsigned char*) "Á", (unsigned char*) "á",
  (unsigned char*) "Â", (unsigned char*) "â",
  (unsigned char*) "×", (unsigned char*) "÷",
  (unsigned char*) "Ç", (unsigned char*) "ç",
  (unsigned char*) "Ä", (unsigned char*) "ä",
  (unsigned char*) "Å", (unsigned char*) "å³£",
  (unsigned char*) "Ö", (unsigned char*) "ö",
  (unsigned char*) "Ú", (unsigned char*) "ú",
  (unsigned char*) "É", (unsigned char*) "é",
  (unsigned char*) "Ê", (unsigned char*) "ê",
  (unsigned char*) "Ë", (unsigned char*) "ë",
  (unsigned char*) "Ì", (unsigned char*) "ì",
  (unsigned char*) "Í", (unsigned char*) "í",
  (unsigned char*) "Î", (unsigned char*) "î",
  (unsigned char*) "Ï", (unsigned char*) "ï",
  (unsigned char*) "Ğ", (unsigned char*) "ğ",
  (unsigned char*) "Ò", (unsigned char*) "ò",
  (unsigned char*) "Ó", (unsigned char*) "ó",
  (unsigned char*) "Ô", (unsigned char*) "ô",
  (unsigned char*) "Õ", (unsigned char*) "õ",
  (unsigned char*) "Æ", (unsigned char*) "æ",
  (unsigned char*) "È", (unsigned char*) "è",
  (unsigned char*) "Ã", (unsigned char*) "ã",
  (unsigned char*) "Ş", (unsigned char*) "ş",
  (unsigned char*) "Û", (unsigned char*) "û",
  (unsigned char*) "İ", (unsigned char*) "ı",
  (unsigned char*) "Ü", (unsigned char*) "ü",
  (unsigned char*) "À", (unsigned char*) "à",
  (unsigned char*) "Ñ", (unsigned char*) "ñ",
  (unsigned char*) "Ù", (unsigned char*) "ù",
  (unsigned char*) "Ø", (unsigned char*) "ø",
  (unsigned char*) "£", (unsigned char*) "³åÅ",
  (unsigned char*) "á", (unsigned char*) "Á",
  (unsigned char*) "â", (unsigned char*) "Â",
  (unsigned char*) "÷", (unsigned char*) "×",
  (unsigned char*) "ç", (unsigned char*) "Ç",
  (unsigned char*) "ä", (unsigned char*) "Ä",
  (unsigned char*) "å", (unsigned char*) "Å³£",
  (unsigned char*) "ö", (unsigned char*) "Ö",
  (unsigned char*) "ú", (unsigned char*) "Ú",
  (unsigned char*) "é", (unsigned char*) "É",
  (unsigned char*) "ê", (unsigned char*) "Ê",
  (unsigned char*) "ë", (unsigned char*) "Ë",
  (unsigned char*) "ì", (unsigned char*) "Ì",
  (unsigned char*) "í", (unsigned char*) "Í",
  (unsigned char*) "î", (unsigned char*) "Î",
  (unsigned char*) "ï", (unsigned char*) "Ï",
  (unsigned char*) "ğ", (unsigned char*) "Ğ",
  (unsigned char*) "ò", (unsigned char*) "Ò",
  (unsigned char*) "ó", (unsigned char*) "Ó",
  (unsigned char*) "ô", (unsigned char*) "Ô",
  (unsigned char*) "õ", (unsigned char*) "Õ",
  (unsigned char*) "æ", (unsigned char*) "Æ",
  (unsigned char*) "è", (unsigned char*) "È",
  (unsigned char*) "ã", (unsigned char*) "Ã",
  (unsigned char*) "ş", (unsigned char*) "Ş",
  (unsigned char*) "û", (unsigned char*) "Û",
  (unsigned char*) "ı", (unsigned char*) "İ",
  (unsigned char*) "ü", (unsigned char*) "Ü",
  (unsigned char*) "à", (unsigned char*) "À",
  (unsigned char*) "ñ", (unsigned char*) "Ñ",
  (unsigned char*) "ù", (unsigned char*) "Ù",
  (unsigned char*) "ø", (unsigned char*) "Ø",
  (unsigned char*) "³", (unsigned char*) "£åÅ",
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
  (unsigned char*) "a", (unsigned char*) "àâáåeèéêëiîïíìoôóòuùûüyÿ",
  (unsigned char*) "à", (unsigned char*) "aâáåeèéêëiîïíìoôóòuùûüyÿ",
  (unsigned char*) "á", (unsigned char*) "aâàåeèéêëiîïíìoôóòuùûüyÿ",
  (unsigned char*) "â", (unsigned char*) "aàáåeèéêëiîïíìoôóòuùûüyÿ",
  (unsigned char*) "å", (unsigned char*) "aàâáeèéêëiîïíìoôóòuùûüyÿ",
  (unsigned char*) "e", (unsigned char*) "aàáâåèéêëiîïíìoôóòuùûüyÿ",
  (unsigned char*) "é", (unsigned char*) "aàáâåeèêëiîïíìoôóòuùûüyÿ",
  (unsigned char*) "è", (unsigned char*) "aàáâåeêéëiîïíìoôóòuùûüyÿ",
  (unsigned char*) "ê", (unsigned char*) "aàáâåeèéëiîïíìoôóòuùûüyÿ",
  (unsigned char*) "ë", (unsigned char*) "aàáâåeèéêiîïíìoôóòuùûüyÿ",
  (unsigned char*) "i", (unsigned char*) "aàáâåeèéêëîïíìoôóòuùûüyÿ",
  (unsigned char*) "î", (unsigned char*) "aàáâåeèéêëiïíìoôóòuùûüyÿ",
  (unsigned char*) "ï", (unsigned char*) "aàáâåeèéêëiîíìoôóòuùûüyÿ",
  (unsigned char*) "í", (unsigned char*) "aàáâåeèéêëiîïìoôóòuùûüyÿ",
  (unsigned char*) "ì", (unsigned char*) "aàáâåeèéêëiîïíoôóòuùûüyÿ",
  (unsigned char*) "o", (unsigned char*) "aàáâåeèéêëiîïíìôóòuùûüyÿ",
  (unsigned char*) "ô", (unsigned char*) "aàáâåeèéêëiîïíìoóòuùûüyÿ",
  (unsigned char*) "ó", (unsigned char*) "aàáâåeèéêëiîïíìoôòuùûüyÿ",
  (unsigned char*) "ò", (unsigned char*) "aàáâåeèéêëiîïíìoôóuùûüyÿ",
  (unsigned char*) "u", (unsigned char*) "aàáâåeèéêëiîïíìoôóòùûüyÿ",
  (unsigned char*) "ù", (unsigned char*) "aàáâåeèéêëiîïíìoôóòuûüyÿ",
  (unsigned char*) "û", (unsigned char*) "aàáâåeèéêëiîïíìoôóòuùüyÿ",
  (unsigned char*) "ü", (unsigned char*) "aàáâåeèéêëiîïíìoôóòuùûyÿ",
  (unsigned char*) "y", (unsigned char*) "aàáâåeèéêëiîïíìoôóòuùûüÿ",
  (unsigned char*) "ÿ", (unsigned char*) "aàáâåeèéêëiîïíìoôóòuùûüy",
#endif /* latin1 */
};

/* fautes de frappe pour un clavier QWERTY ... */
static unsigned char *typos_qwerty [] = {
#if defined(latin1) || defined(latin2)
  (unsigned char*) "1", (unsigned char*) "q",
  (unsigned char*) "2", (unsigned char*) "qw",
  (unsigned char*) "3", (unsigned char*) "weèéêë",
  (unsigned char*) "4", (unsigned char*) "eèéêër",
  (unsigned char*) "5", (unsigned char*) "rt",
  (unsigned char*) "6", (unsigned char*) "ty",
  (unsigned char*) "7", (unsigned char*) "yuùûü",
  (unsigned char*) "8", (unsigned char*) "uùûüiîï",
  (unsigned char*) "9", (unsigned char*) "iîïoô",
  (unsigned char*) "0", (unsigned char*) "oôp",
  (unsigned char*) "-", (unsigned char*) "p",
  (unsigned char*) "=", (unsigned char*) "-",
  (unsigned char*) "[", (unsigned char*) "p",
  (unsigned char*) ";", (unsigned char*) "pl",
  (unsigned char*) ",", (unsigned char*) "klm",

  (unsigned char*) "q", (unsigned char*) "waàâ",
  (unsigned char*) "w", (unsigned char*) "qaàâseèéêë",
  (unsigned char*) "e", (unsigned char*) "wsdr",
  (unsigned char*) "r", (unsigned char*) "eèéêëdft",
  (unsigned char*) "t", (unsigned char*) "rfgyÿ",
  (unsigned char*) "y", (unsigned char*) "tghuùûü",
  (unsigned char*) "u", (unsigned char*) "yÿhjkiîï",
  (unsigned char*) "i", (unsigned char*) "uùûüjkoô",
  (unsigned char*) "o", (unsigned char*) "iîïklp",
  (unsigned char*) "p", (unsigned char*) "oôl-",
  (unsigned char*) "a", (unsigned char*) "zswq",
  (unsigned char*) "s", (unsigned char*) "qaàâzxdeèéêëw",
  (unsigned char*) "d", (unsigned char*) "eèéêësxcçfr",
  (unsigned char*) "f", (unsigned char*) "rdcçvgt",
  (unsigned char*) "g", (unsigned char*) "tfvbhyÿ",
  (unsigned char*) "h", (unsigned char*) "yÿgbnjuùûü",
  (unsigned char*) "j", (unsigned char*) "uùûühnmkiîï",
  (unsigned char*) "k", (unsigned char*) "iîïjmloô",
  (unsigned char*) "l", (unsigned char*) "oôkp",
  (unsigned char*) "z", (unsigned char*) "aàâsx",
  (unsigned char*) "x", (unsigned char*) "zsdcç",
  (unsigned char*) "c", (unsigned char*) "xdfv",
  (unsigned char*) "v", (unsigned char*) "cçfgb",
  (unsigned char*) "b", (unsigned char*) "vghn",
  (unsigned char*) "n", (unsigned char*) "bhjm",
  (unsigned char*) "m", (unsigned char*) "njk",

  (unsigned char*) "!", (unsigned char*) "Q",
  (unsigned char*) "@", (unsigned char*) "QW",
  (unsigned char*) "#", (unsigned char*) "WEÈÉÊË",
  (unsigned char*) "$", (unsigned char*) "EÈÉÊËR",
  (unsigned char*) "%", (unsigned char*) "RT",
  (unsigned char*) "^", (unsigned char*) "TY",
  (unsigned char*) "&", (unsigned char*) "YUÙÛÜ",
  (unsigned char*) "*", (unsigned char*) "UÙÛÜIÎÏ",
  (unsigned char*) "(", (unsigned char*) "IÎÏOÔ",
  (unsigned char*) ")", (unsigned char*) "OÔP",
  (unsigned char*) "_", (unsigned char*) "P",
  (unsigned char*) "+", (unsigned char*) "_",
  (unsigned char*) "{", (unsigned char*) "P",
  (unsigned char*) ":", (unsigned char*) "PL",
  (unsigned char*) "<", (unsigned char*) "KLM",

  (unsigned char*) "Q", (unsigned char*) "WAÀÂ",
  (unsigned char*) "W", (unsigned char*) "QAÀÂSEÈÉÊË",
  (unsigned char*) "E", (unsigned char*) "WSDR",
  (unsigned char*) "R", (unsigned char*) "EÈÉÊËDFT",
  (unsigned char*) "T", (unsigned char*) "RFGY",
  (unsigned char*) "Y", (unsigned char*) "TGHUÙÛÜ",
  (unsigned char*) "U", (unsigned char*) "YHJIÎÏ",
  (unsigned char*) "I", (unsigned char*) "UÙÛÜJKOÔ",
  (unsigned char*) "O", (unsigned char*) "IÎÏKLP",
  (unsigned char*) "P", (unsigned char*) "OÔL_",
  (unsigned char*) "A", (unsigned char*) "ZSWQ",
  (unsigned char*) "S", (unsigned char*) "QAÀÂZXDEÈÉÊËW",
  (unsigned char*) "D", (unsigned char*) "EÈÉÊËSXCÇFR",
  (unsigned char*) "F", (unsigned char*) "RDCÇVGT",
  (unsigned char*) "G", (unsigned char*) "TFVBHY",
  (unsigned char*) "H", (unsigned char*) "YGBNJUÙÛÜ",
  (unsigned char*) "J", (unsigned char*) "ÙÛÜUHNMKIÎÏ",
  (unsigned char*) "K", (unsigned char*) "IÎÏJMLOÔ",
  (unsigned char*) "L", (unsigned char*) "OÔKP",
  (unsigned char*) "Z", (unsigned char*) "AÀÂSX",
  (unsigned char*) "X", (unsigned char*) "ZSDCÇ",
  (unsigned char*) "C", (unsigned char*) "XDFV",
  (unsigned char*) "V", (unsigned char*) "CÇFGB",
  (unsigned char*) "B", (unsigned char*) "VGHN",
  (unsigned char*) "N", (unsigned char*) "BHJM",
  (unsigned char*) "M", (unsigned char*) "NJK",
#endif /* latin1 || latin2 */
};

/* Changement de clavier ... */
static unsigned char *qwerty_azerty [] = {
#if defined(latin1) || defined(latin2)
  (unsigned char*) "2", (unsigned char*) "é",
  (unsigned char*) "7", (unsigned char*) "è",
  (unsigned char*) "9", (unsigned char*) "ç",
  (unsigned char*) "0", (unsigned char*) "à",
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
  (unsigned char*) "é", (unsigned char*) "2",
  (unsigned char*) "è", (unsigned char*) "7",
  (unsigned char*) "ç", (unsigned char*) "9",
  (unsigned char*) "à", (unsigned char*) "0",
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

  (unsigned char*) "æ", (unsigned char*) "ae",
  (unsigned char*) "ae", (unsigned char*) "æ",
  (unsigned char*) "½", (unsigned char*) "oe",
  (unsigned char*) "e", (unsigned char*) "oe",
  (unsigned char*) "oe", (unsigned char*) "½",
  (unsigned char*) "oe", (unsigned char*) "e",

  /* Essai */
  /* Les autres combi sont faites par ailleurs */
  (unsigned char*) "c", (unsigned char*) "ss",
  (unsigned char*) "c", (unsigned char*) "s",
  (unsigned char*) "c", (unsigned char*) "sc",
  (unsigned char*) "ç", (unsigned char*) "ss",
  (unsigned char*) "ç", (unsigned char*) "s",
  (unsigned char*) "ss", (unsigned char*) "c",
  (unsigned char*) "ss", (unsigned char*) "ç",
  (unsigned char*) "s", (unsigned char*) "c",
  (unsigned char*) "sc", (unsigned char*) "c",
  (unsigned char*) "s", (unsigned char*) "ç",

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
  (unsigned char*) "aiss", (unsigned char*) "èss",
  (unsigned char*) "aiss", (unsigned char*) "êss",
  (unsigned char*) "aîss", (unsigned char*) "éss",
  (unsigned char*) "aîss", (unsigned char*) "èss",
  (unsigned char*) "aîss", (unsigned char*) "êss",

  (unsigned char*) "ess", (unsigned char*) "aiss",
  (unsigned char*) "èss", (unsigned char*) "aiss",
  (unsigned char*) "êss", (unsigned char*) "aiss",
  (unsigned char*) "éss", (unsigned char*) "aîss",
  (unsigned char*) "èss", (unsigned char*) "aîss",
  (unsigned char*) "êss", (unsigned char*) "aîss",

/* À quoi sert-ce ?
  (unsigned char*) "est", (unsigned char*) "é",
  (unsigned char*) "est", (unsigned char*) "è",
  (unsigned char*) "est", (unsigned char*) "ê",
*/

/* Hibou, chou, ... ; clou, sou, ... */
  (unsigned char*) "ous$", (unsigned char*) "oux",
  (unsigned char*) "oux$", (unsigned char*) "ous",

  /* à voir... en principe ça fait passer d'un mot connu à un autre... donc c pas sûr que ça doit rester là */
  (unsigned char*) "er$", (unsigned char*) "é",
  (unsigned char*) "é$", (unsigned char*) "er",

/* Finales de verbes
  (unsigned char*) "$", (unsigned char*) "ent",
*/

/*  (unsigned char*) "^l", (unsigned char*) "l'",*/


#endif /* french */
#ifdef polish
  (unsigned char*) "cz", (unsigned char*) "æ",
  (unsigned char*) "cz", (unsigned char*) "c",
  (unsigned char*) "c", (unsigned char*) "cz",
  (unsigned char*) "æ", (unsigned char*) "cz",
  (unsigned char*) "sz", (unsigned char*) "¶",
  (unsigned char*) "sz", (unsigned char*) "s",
  (unsigned char*) "s", (unsigned char*) "sz",
  (unsigned char*) "¶", (unsigned char*) "sz",
  (unsigned char*) "rz", (unsigned char*) "¼",
  (unsigned char*) "rz", (unsigned char*) "¿",
  (unsigned char*) "¿", (unsigned char*) "rz",
  (unsigned char*) "¼", (unsigned char*) "rz",
  (unsigned char*) "ch", (unsigned char*) "h",
  (unsigned char*) "h", (unsigned char*) "ch",
#endif /* polish */
#ifdef spanish
  (unsigned char*) "y", (unsigned char*) "ll",
  (unsigned char*) "ll", (unsigned char*) "y",
#endif /* spanish */
#ifdef italian
  (unsigned char*) "E'", (unsigned char*) "È",
  (unsigned char*) "E'", (unsigned char*) "É",
  (unsigned char*) "U'", (unsigned char*) "Ù",
  (unsigned char*) "A'", (unsigned char*) "À",
  (unsigned char*) "O'", (unsigned char*) "Ò",
  (unsigned char*) "I'", (unsigned char*) "ì",
  (unsigned char*) "e'", (unsigned char*) "è",
  (unsigned char*) "e'", (unsigned char*) "é",
  (unsigned char*) "u'", (unsigned char*) "ù",
  (unsigned char*) "a'", (unsigned char*) "à",
  (unsigned char*) "o'", (unsigned char*) "ò",
  (unsigned char*) "i'", (unsigned char*) "ì",
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
  (unsigned char*) "fiE", (unsigned char*) "É",
  (unsigned char*) "G", (unsigned char*) "É",
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
  (unsigned char*) "A", (unsigned char*) "ÀÁÂÅaáàâå",
  (unsigned char*) "À", (unsigned char*) "AÁÂÅaáàâå",
  (unsigned char*) "Á", (unsigned char*) "AÀÂÅaáàâå",
  (unsigned char*) "Â", (unsigned char*) "AÀÁÅaáàâå",
  (unsigned char*) "Å", (unsigned char*) "AÀÁÂaáàâå",
  (unsigned char*) "Æ", (unsigned char*) "æ",
  (unsigned char*) "B", (unsigned char*) "b",
  (unsigned char*) "C", (unsigned char*) "Çcç",
  (unsigned char*) "Ç", (unsigned char*) "Ccç",
  (unsigned char*) "D", (unsigned char*) "d",
  (unsigned char*) "E", (unsigned char*) "ÈÉÊËeèéêë",
  (unsigned char*) "È", (unsigned char*) "EÉÊËeèéêë",
  (unsigned char*) "É", (unsigned char*) "EÈÊËeèéêë",
  (unsigned char*) "Ê", (unsigned char*) "EÈÉËeèéêë",
  (unsigned char*) "Ë", (unsigned char*) "EÈÉÊeèéêë",
  (unsigned char*) "F", (unsigned char*) "f",
  (unsigned char*) "G", (unsigned char*) "g",
  (unsigned char*) "H", (unsigned char*) "h",
  (unsigned char*) "I", (unsigned char*) "ÎÏÌÍiîïìí",
  (unsigned char*) "Î", (unsigned char*) "IÏÌÍiîïìí",
  (unsigned char*) "Ï", (unsigned char*) "IÎÌÍiîïìí",
  (unsigned char*) "Ì", (unsigned char*) "IÎÏÍiîïìí",
  (unsigned char*) "Í", (unsigned char*) "IÎÏÌiîïìí",
  (unsigned char*) "J", (unsigned char*) "j",
  (unsigned char*) "K", (unsigned char*) "k",
  (unsigned char*) "L", (unsigned char*) "l",
  (unsigned char*) "M", (unsigned char*) "m",
  (unsigned char*) "N", (unsigned char*) "Ñn",
  (unsigned char*) "Ñ", (unsigned char*) "Nñ",
  (unsigned char*) "O", (unsigned char*) "ÔÒÓÕÖoôòóõö",
  (unsigned char*) "Ô", (unsigned char*) "OÒÓÕÖoôòóõö",
  (unsigned char*) "Ò", (unsigned char*) "OÔÓÕÖoôòóõö",
  (unsigned char*) "Ó", (unsigned char*) "OÔÒÕÖoôòóõö",
  (unsigned char*) "Õ", (unsigned char*) "OÔÒÓÖoôòóõö",
  (unsigned char*) "Ö", (unsigned char*) "OÔÒÓÕoôòóõö",
  (unsigned char*) "P", (unsigned char*) "p",
  (unsigned char*) "Q", (unsigned char*) "q",
  (unsigned char*) "R", (unsigned char*) "r",
  (unsigned char*) "S", (unsigned char*) "s",
  (unsigned char*) "T", (unsigned char*) "t",
  (unsigned char*) "U", (unsigned char*) "ÙÛÜÚuùûü",
  (unsigned char*) "Ù", (unsigned char*) "UÛÜÚuùûü",
  (unsigned char*) "Û", (unsigned char*) "UÙÜÚuùûü",
  (unsigned char*) "Ü", (unsigned char*) "UÙÛÚuùûü",
  (unsigned char*) "Ú", (unsigned char*) "UÙÛÜuùûü",
  (unsigned char*) "V", (unsigned char*) "v",
  (unsigned char*) "W", (unsigned char*) "w",
  (unsigned char*) "X", (unsigned char*) "x",
  (unsigned char*) "Y", (unsigned char*) "İyÿ",
  (unsigned char*) "İ", (unsigned char*) "Yyÿ",
  (unsigned char*) "Z", (unsigned char*) "z",
#endif /* latin1 */
#ifdef latin2
  (unsigned char*) "A", (unsigned char*) "¡ÁÂÃÄa±áâãä",
  (unsigned char*) "¡", (unsigned char*) "AÁÂÃÄa±áâãä",
  (unsigned char*) "Á", (unsigned char*) "A¡ÂÃÄa±áâãä",
  (unsigned char*) "Â", (unsigned char*) "A¡ÁÃÄa±áâãä",
  (unsigned char*) "Ã", (unsigned char*) "A¡ÁÂÄa±áâãä",
  (unsigned char*) "Ä", (unsigned char*) "A¡ÁÂÃa±áâãä",
  (unsigned char*) "B", (unsigned char*) "b",
  (unsigned char*) "C", (unsigned char*) "ÆÇÈcæçè",
  (unsigned char*) "Æ", (unsigned char*) "CÇÈcæçè",
  (unsigned char*) "Ç", (unsigned char*) "CÆÈcæçè",
  (unsigned char*) "È", (unsigned char*) "CÆÇcæçè",
  (unsigned char*) "D", (unsigned char*) "ÏĞdïğ",
  (unsigned char*) "Ï", (unsigned char*) "DĞdïğ",
  (unsigned char*) "Ğ", (unsigned char*) "DÏdïğ",
  (unsigned char*) "E", (unsigned char*) "ÉÊËÌeéêëì",
  (unsigned char*) "É", (unsigned char*) "EÊËÌeéêëì",
  (unsigned char*) "Ê", (unsigned char*) "EÉËÌeéêëì",
  (unsigned char*) "Ë", (unsigned char*) "EÉÊÌeéêëì",
  (unsigned char*) "Ì", (unsigned char*) "EÉÊËeéêëì",
  (unsigned char*) "F", (unsigned char*) "f",
  (unsigned char*) "G", (unsigned char*) "g",
  (unsigned char*) "H", (unsigned char*) "h",
  (unsigned char*) "I", (unsigned char*) "ÍÎiíî",
  (unsigned char*) "Í", (unsigned char*) "IÎiíî",
  (unsigned char*) "Î", (unsigned char*) "IÍiíî",
  (unsigned char*) "J", (unsigned char*) "j",
  (unsigned char*) "K", (unsigned char*) "k",
  (unsigned char*) "L", (unsigned char*) "£¥Ål³µå",
  (unsigned char*) "£", (unsigned char*) "L¥Ål³µå",
  (unsigned char*) "¥", (unsigned char*) "L£Ål³µå",
  (unsigned char*) "Å", (unsigned char*) "L£¥l³µå",
  (unsigned char*) "M", (unsigned char*) "m",
  (unsigned char*) "N", (unsigned char*) "ÒÑnòñ",
  (unsigned char*) "Ò", (unsigned char*) "NÑnòñ",
  (unsigned char*) "Ñ", (unsigned char*) "NÒnòñ",
  (unsigned char*) "O", (unsigned char*) "ÓÔÕÖoóôõö",
  (unsigned char*) "Ó", (unsigned char*) "OÔÕÖoóôõö",
  (unsigned char*) "Ô", (unsigned char*) "OÓÕÖoóôõö",
  (unsigned char*) "Õ", (unsigned char*) "OÓÔÖoóôõö",
  (unsigned char*) "Õ", (unsigned char*) "OÓÔÕoóôõö",
  (unsigned char*) "P", (unsigned char*) "p",
  (unsigned char*) "Q", (unsigned char*) "q",
  (unsigned char*) "R", (unsigned char*) "ÀØràø",
  (unsigned char*) "À", (unsigned char*) "RØràø",
  (unsigned char*) "Ø", (unsigned char*) "RÀràø",
  (unsigned char*) "S", (unsigned char*) "¦©ªs¶¹º",
  (unsigned char*) "¦", (unsigned char*) "S©ªs¶¹º",
  (unsigned char*) "©", (unsigned char*) "S¦ªs¶¹º",
  (unsigned char*) "ª", (unsigned char*) "S¦©s¶¹º",
  (unsigned char*) "T", (unsigned char*) "«Şt»ş",
  (unsigned char*) "«", (unsigned char*) "TŞtş»",
  (unsigned char*) "Ş", (unsigned char*) "T«tş»",
  (unsigned char*) "U", (unsigned char*) "ÙÚÛÜuùúûü",
  (unsigned char*) "Ù", (unsigned char*) "UÚÛÜuùúûü",
  (unsigned char*) "Ú", (unsigned char*) "UÙÛÜuùúûü",
  (unsigned char*) "Û", (unsigned char*) "UÙÚÜuùúûü",
  (unsigned char*) "Ü", (unsigned char*) "UÙÚÛuùúûü",
  (unsigned char*) "V", (unsigned char*) "v",
  (unsigned char*) "W", (unsigned char*) "w",
  (unsigned char*) "X", (unsigned char*) "x",
  (unsigned char*) "Y", (unsigned char*) "İyı",
  (unsigned char*) "İ", (unsigned char*) "Yyı",
  (unsigned char*) "Z", (unsigned char*) "¬®¯z¼¾¿",
  (unsigned char*) "¬", (unsigned char*) "Z®¯z¼¾¿",
  (unsigned char*) "®", (unsigned char*) "Z¬¯z¼¾¿",
  (unsigned char*) "¯", (unsigned char*) "Z¬®z¼¾¿",
#endif /* latin2 */
};


/* ajout d'une apostrophe dans certains contextes - poids assez élevé : add_apos_weight */
static unsigned char *add_apos  [] = {
#if defined(latin1) || defined(latin2)
  (unsigned char*) "a", (unsigned char*) "a'",
  (unsigned char*) "â", (unsigned char*) "â'",
  (unsigned char*) "e", (unsigned char*) "e'",
  (unsigned char*) "é", (unsigned char*) "é'",
  (unsigned char*) "è", (unsigned char*) "è'",
  (unsigned char*) "ê", (unsigned char*) "ê'",
  (unsigned char*) "i", (unsigned char*) "i'",
  (unsigned char*) "î", (unsigned char*) "î'",
  (unsigned char*) "o", (unsigned char*) "o'",
  (unsigned char*) "ô", (unsigned char*) "ô'",
  (unsigned char*) "u", (unsigned char*) "u'",
  (unsigned char*) "û", (unsigned char*) "û'",
  (unsigned char*) "a'", (unsigned char*) "a",
  (unsigned char*) "â'", (unsigned char*) "â",
  (unsigned char*) "e'", (unsigned char*) "e",
  (unsigned char*) "é'", (unsigned char*) "é",
  (unsigned char*) "è'", (unsigned char*) "è",
  (unsigned char*) "ê'", (unsigned char*) "ê",
  (unsigned char*) "i'", (unsigned char*) "i",
  (unsigned char*) "î'", (unsigned char*) "î",
  (unsigned char*) "o'", (unsigned char*) "o",
  (unsigned char*) "ô'", (unsigned char*) "ô",
  (unsigned char*) "u'", (unsigned char*) "u",
  (unsigned char*) "û'", (unsigned char*) "û",
  (unsigned char*) ".'", (unsigned char*) "_",
#endif /* latin1 || latin2 */
};

/* abréviations sans leur point - poids très faible final_dot_weight */ 
static unsigned char *final_dot  [] = {
  (unsigned char*) "$", (unsigned char*) ".",
};


/* abréviations sans leur point - poids très faible final_dot_weight */ 
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

/* ajout final d'un e - poids assez élevé final_e_weight */
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

/* finales abrégées - poids moyen abbrev_weight */
static unsigned char *abbrev  [] = {
#ifdef french
  (unsigned char*) "t°$", (unsigned char*) "tion",
  (unsigned char*) "mt$", (unsigned char*) "ment",
#endif /* french */
};

/* flexions mal fixées - poids faible  final_als_weight */
static unsigned char *final_als [] = {
#ifdef french
  (unsigned char*) "als$", (unsigned char*) "aux",
  (unsigned char*) "aux$", (unsigned char*) "als",
  (unsigned char*) "ère$", (unsigned char*) "euse",
  (unsigned char*) "euse$", (unsigned char*) "ère",
  (unsigned char*) "ères$", (unsigned char*) "euses",
  (unsigned char*) "euses$", (unsigned char*) "ères",
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
  (unsigned char*) "ssè-je$", (unsigned char*) "s-je",
  (unsigned char*) "ssé-je$", (unsigned char*) "s-je",
  (unsigned char*) "é-je$", (unsigned char*) "-je",
  (unsigned char*) "è-je$", (unsigned char*) "-je",
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
  (unsigned char*) "é", (unsigned char*) "ù",
  (unsigned char*) "ù", (unsigned char*) "é",
  (unsigned char*) "É", (unsigned char*) "Ù",
  (unsigned char*) "Ù", (unsigned char*) "É",
#endif /* russian */
};

/* manque un u ou un e après un g pour en changer le son - poids moyen gu_ge_weight */
static unsigned char *gu_ge [] = {
#ifdef french
  (unsigned char*) "ge", (unsigned char*) "gue",
  (unsigned char*) "gé", (unsigned char*) "gué",
  (unsigned char*) "gè", (unsigned char*) "guè",
  (unsigned char*) "gê", (unsigned char*) "guê",
  (unsigned char*) "gi", (unsigned char*) "gui",
  (unsigned char*) "gî", (unsigned char*) "guî",
  (unsigned char*) "ga", (unsigned char*) "gea",
  (unsigned char*) "gâ", (unsigned char*) "geâ",
  (unsigned char*) "go", (unsigned char*) "geo",
  (unsigned char*) "gô", (unsigned char*) "geô",
  (unsigned char*) "gu", (unsigned char*) "geu",
  (unsigned char*) "gû", (unsigned char*) "geû",
#endif /* french */
};


/* ss <-> ß */
static unsigned char *esszet [] = {
#ifdef german
  (unsigned char*) "ss", (unsigned char*) "ß",
  (unsigned char*) "ß", (unsigned char*) "ss",
#endif /* german */
};

/* ajout d'un h dans certains contextes - poids moyen add_h_weight */
static unsigned char *add_h [] = {
#ifdef french
  (unsigned char*) "a", (unsigned char*) "ah",
  (unsigned char*) "â", (unsigned char*) "âh",
  (unsigned char*) "e", (unsigned char*) "eh",
  (unsigned char*) "é", (unsigned char*) "éh",
  (unsigned char*) "è", (unsigned char*) "èh",
  (unsigned char*) "ê", (unsigned char*) "êh",
  (unsigned char*) "i", (unsigned char*) "ih",
  (unsigned char*) "î", (unsigned char*) "îh",
  (unsigned char*) "o", (unsigned char*) "oh",
  (unsigned char*) "ô", (unsigned char*) "ôh",
  (unsigned char*) "u", (unsigned char*) "uh",
  (unsigned char*) "û", (unsigned char*) "ûh",
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
  (unsigned char*) "A^", (unsigned char*) "Â",
  (unsigned char*) "A'", (unsigned char*) "Á",
  (unsigned char*) "A`", (unsigned char*) "À",
  (unsigned char*) "a^", (unsigned char*) "â",
  (unsigned char*) "a'", (unsigned char*) "á",
  (unsigned char*) "a`", (unsigned char*) "à",

  (unsigned char*) "E^", (unsigned char*) "Ê",
  (unsigned char*) "E'", (unsigned char*) "É",
  (unsigned char*) "E`", (unsigned char*) "È",
  (unsigned char*) "E\"", (unsigned char*) "Ë",
  (unsigned char*) "e^", (unsigned char*) "ê",
  (unsigned char*) "e'", (unsigned char*) "é",
  (unsigned char*) "e`", (unsigned char*) "è",
  (unsigned char*) "e\"", (unsigned char*) "ë",

  (unsigned char*) "I^", (unsigned char*) "Î",
  (unsigned char*) "I\"", (unsigned char*) "Ï",
  (unsigned char*) "i^", (unsigned char*) "î",
  (unsigned char*) "i\"", (unsigned char*) "ï",

  (unsigned char*) "O^", (unsigned char*) "Ô",
  (unsigned char*) "o^", (unsigned char*) "ô",

  (unsigned char*) "U^", (unsigned char*) "Û",
  (unsigned char*) "U`", (unsigned char*) "Ù",
  (unsigned char*) "U\"", (unsigned char*) "Ü",
  (unsigned char*) "u^", (unsigned char*) "û",
  (unsigned char*) "u`", (unsigned char*) "ù",
  (unsigned char*) "u\"", (unsigned char*) "ü",

  (unsigned char*) "y\"", (unsigned char*) "ÿ",

  (unsigned char*) "C,", (unsigned char*) "Ç",
  (unsigned char*) "c,", (unsigned char*) "ç",
#endif /* latin1 */
#ifdef latin2
  (unsigned char*) "A,", (unsigned char*) "¡",
  (unsigned char*) "a,", (unsigned char*) "±",
  (unsigned char*) "L·", (unsigned char*) "¥",
  (unsigned char*) "l·", (unsigned char*) "µ",
  (unsigned char*) "S'", (unsigned char*) "¦",
  (unsigned char*) "s'", (unsigned char*) "¶",
  (unsigned char*) "S·", (unsigned char*) "©",
  (unsigned char*) "s·", (unsigned char*) "¹",
  (unsigned char*) "S,", (unsigned char*) "ª",
  (unsigned char*) "s,", (unsigned char*) "º",
  (unsigned char*) "T·", (unsigned char*) "«",
  (unsigned char*) "t·", (unsigned char*) "»",
  (unsigned char*) "Z'", (unsigned char*) "¬",
  (unsigned char*) "z'", (unsigned char*) "¼",
  (unsigned char*) "Z·", (unsigned char*) "®¯",
  (unsigned char*) "z·", (unsigned char*) "¾¿",
  (unsigned char*) "R'", (unsigned char*) "À",
  (unsigned char*) "r'", (unsigned char*) "à",
  (unsigned char*) "A'", (unsigned char*) "Á",
  (unsigned char*) "a'", (unsigned char*) "á",
  (unsigned char*) "A^", (unsigned char*) "ÂÃ",
  (unsigned char*) "a^", (unsigned char*) "âã",
  (unsigned char*) "A¨", (unsigned char*) "Ä",
  (unsigned char*) "a¨", (unsigned char*) "ä",
  (unsigned char*) "L'", (unsigned char*) "Å£",
  (unsigned char*) "l'", (unsigned char*) "å³",
  (unsigned char*) "C'", (unsigned char*) "Æ",
  (unsigned char*) "c'", (unsigned char*) "æ",
  (unsigned char*) "C,", (unsigned char*) "Ç",
  (unsigned char*) "c,", (unsigned char*) "ç",
  (unsigned char*) "C·", (unsigned char*) "È",
  (unsigned char*) "c·", (unsigned char*) "è",
  (unsigned char*) "E'", (unsigned char*) "É",
  (unsigned char*) "e'", (unsigned char*) "é",
  (unsigned char*) "E,", (unsigned char*) "Ê",
  (unsigned char*) "e,", (unsigned char*) "ê",
  (unsigned char*) "E¨", (unsigned char*) "Ë",
  (unsigned char*) "e¨", (unsigned char*) "ë",
  (unsigned char*) "E·", (unsigned char*) "Ì",
  (unsigned char*) "e·", (unsigned char*) "ì",
  (unsigned char*) "I'", (unsigned char*) "Í",
  (unsigned char*) "i'", (unsigned char*) "í",
  (unsigned char*) "I^", (unsigned char*) "Î",
  (unsigned char*) "i^", (unsigned char*) "î",
  (unsigned char*) "D·", (unsigned char*) "ÏĞ",
  (unsigned char*) "d·", (unsigned char*) "ïğ",
  (unsigned char*) "N'", (unsigned char*) "Ñ",
  (unsigned char*) "n'", (unsigned char*) "ñ",
  (unsigned char*) "N·", (unsigned char*) "Ò",
  (unsigned char*) "n·", (unsigned char*) "ò",
  (unsigned char*) "O'", (unsigned char*) "Ó",
  (unsigned char*) "o'", (unsigned char*) "ó",
  (unsigned char*) "O^", (unsigned char*) "Ô",
  (unsigned char*) "o^", (unsigned char*) "ô",
  (unsigned char*) "O\"", (unsigned char*) "Õ",
  (unsigned char*) "o\"", (unsigned char*) "õ",
  (unsigned char*) "O¨", (unsigned char*) "Ö",
  (unsigned char*) "o¨", (unsigned char*) "ö",
  (unsigned char*) "R·", (unsigned char*) "Ø",
  (unsigned char*) "r·", (unsigned char*) "ø",
  (unsigned char*) "U·", (unsigned char*) "Ù",
  (unsigned char*) "u·", (unsigned char*) "ù",
  (unsigned char*) "U'", (unsigned char*) "Ú",
  (unsigned char*) "u'", (unsigned char*) "ú",
  (unsigned char*) "U\"", (unsigned char*) "Û",
  (unsigned char*) "u\"", (unsigned char*) "û",
  (unsigned char*) "U¨", (unsigned char*) "Ü",
  (unsigned char*) "u¨", (unsigned char*) "ü",
  (unsigned char*) "Y'", (unsigned char*) "İ",
  (unsigned char*) "y'", (unsigned char*) "ı",
  (unsigned char*) "T,", (unsigned char*) "Ş",
  (unsigned char*) "t,", (unsigned char*) "ş",
#endif /* latin2 */
};


/* remplacement des diacritics */
static unsigned char *add_diacritics [] = {
#ifdef latin1
  (unsigned char*) "A", (unsigned char*) "áàâäå",
  (unsigned char*) "a", (unsigned char*) "áàâäå",

  (unsigned char*) "E", (unsigned char*) "èéêë",
  (unsigned char*) "e", (unsigned char*) "èéêë",

  (unsigned char*) "I", (unsigned char*) "îïíì",
  (unsigned char*) "i", (unsigned char*) "îïíì",

  (unsigned char*) "O", (unsigned char*) "ôóò",
  (unsigned char*) "o", (unsigned char*) "ôóò",

  (unsigned char*) "U", (unsigned char*) "ùûü",
  (unsigned char*) "u", (unsigned char*) "ùûü",

  (unsigned char*) "Y", (unsigned char*) "ÿı",
  (unsigned char*) "y", (unsigned char*) "ÿı",

  (unsigned char*) "C", (unsigned char*) "ç",
  (unsigned char*) "c", (unsigned char*) "ç",
#endif /* latin1 */
#ifdef latin2
  (unsigned char*) "A", (unsigned char*) "±áâãä",
  (unsigned char*) "a", (unsigned char*) "±áâãä",

  (unsigned char*) "C", (unsigned char*) "æçè",
  (unsigned char*) "c", (unsigned char*) "æçè",

  (unsigned char*) "D", (unsigned char*) "ïğ",
  (unsigned char*) "d", (unsigned char*) "ïğ",

  (unsigned char*) "E", (unsigned char*) "éêëì",
  (unsigned char*) "e", (unsigned char*) "éêëì",

  (unsigned char*) "I", (unsigned char*) "íî",
  (unsigned char*) "i", (unsigned char*) "íî",

  (unsigned char*) "L", (unsigned char*) "³µå",
  (unsigned char*) "l", (unsigned char*) "³µå",

  (unsigned char*) "N", (unsigned char*) "òñ",
  (unsigned char*) "n", (unsigned char*) "òñ",

  (unsigned char*) "O", (unsigned char*) "óôõö",
  (unsigned char*) "o", (unsigned char*) "óôõö",

  (unsigned char*) "R", (unsigned char*) "àø",
  (unsigned char*) "r", (unsigned char*) "àø",

  (unsigned char*) "S", (unsigned char*) "¶¹º",
  (unsigned char*) "s", (unsigned char*) "¶¹º",

  (unsigned char*) "T", (unsigned char*) "»ş",
  (unsigned char*) "t", (unsigned char*) "»ş",

  (unsigned char*) "U", (unsigned char*) "ùúûü",
  (unsigned char*) "u", (unsigned char*) "ùúûü",

  (unsigned char*) "Y", (unsigned char*) "yı",
  (unsigned char*) "y", (unsigned char*) "ı",

  (unsigned char*) "Z", (unsigned char*) "¼¾¿",
  (unsigned char*) "z", (unsigned char*) "¼¾¿",
#endif /* latin2 */
#ifdef koi8r
  (unsigned char*) "Å", (unsigned char*) "£",
  (unsigned char*) "É", (unsigned char*) "Ê",
  (unsigned char*) "é", (unsigned char*) "ê",
#endif /* koi8r */
};

/* remplacement des diacritics */
static unsigned char *change_diacritics [] = {
#ifdef latin1
  (unsigned char*) "À", (unsigned char*) "áàâäå",
  (unsigned char*) "Á", (unsigned char*) "áàâäå",
  (unsigned char*) "Â", (unsigned char*) "áàâäå",
  (unsigned char*) "Å", (unsigned char*) "áàâäå",
  (unsigned char*) "à", (unsigned char*) "áâäå",
  (unsigned char*) "á", (unsigned char*) "àâäå",
  (unsigned char*) "â", (unsigned char*) "áàäå",
  (unsigned char*) "å", (unsigned char*) "áàäâ",

  (unsigned char*) "È", (unsigned char*) "èéêë",
  (unsigned char*) "É", (unsigned char*) "èéêë",
  (unsigned char*) "Ê", (unsigned char*) "èéêë",
  (unsigned char*) "Ë", (unsigned char*) "èéêë",
  (unsigned char*) "è", (unsigned char*) "éêë",
  (unsigned char*) "é", (unsigned char*) "èêë",
  (unsigned char*) "ê", (unsigned char*) "èéë",
  (unsigned char*) "ë", (unsigned char*) "èéê",
  //  (unsigned char*) "ì", (unsigned char*) "é",

  (unsigned char*) "Î", (unsigned char*) "îïíì",
  (unsigned char*) "Ï", (unsigned char*) "îïíì",
  (unsigned char*) "Í", (unsigned char*) "îïíì",
  (unsigned char*) "Ì", (unsigned char*) "îïíì",
  (unsigned char*) "î", (unsigned char*) "ïíì",
  (unsigned char*) "ï", (unsigned char*) "îíì",
  (unsigned char*) "í", (unsigned char*) "îïì",
  (unsigned char*) "ì", (unsigned char*) "îïí",

  (unsigned char*) "Ó", (unsigned char*) "ôóò",
  (unsigned char*) "ó", (unsigned char*) "ôò",
  (unsigned char*) "ò", (unsigned char*) "ôó",
  (unsigned char*) "ô", (unsigned char*) "óò",

  (unsigned char*) "Ú", (unsigned char*) "úùûü",
  (unsigned char*) "Ù", (unsigned char*) "úùûü",
  (unsigned char*) "Û", (unsigned char*) "úùûü",
  (unsigned char*) "Ü", (unsigned char*) "úùûü",
  (unsigned char*) "ú", (unsigned char*) "ùûü",
  (unsigned char*) "ù", (unsigned char*) "úûü",
  (unsigned char*) "û", (unsigned char*) "úùü",
  (unsigned char*) "ü", (unsigned char*) "úùû",

  (unsigned char*) "İ", (unsigned char*) "ÿı",
  (unsigned char*) "ÿ", (unsigned char*) "ı",
  (unsigned char*) "ı", (unsigned char*) "ÿ",

#endif /* latin1 */
#ifdef latin2
  (unsigned char*) "¡", (unsigned char*) "±áâãä",
  (unsigned char*) "Á", (unsigned char*) "±áâãä",
  (unsigned char*) "Â", (unsigned char*) "±áâãä",
  (unsigned char*) "Ã", (unsigned char*) "±áâãä",
  (unsigned char*) "Ä", (unsigned char*) "±áâãä",
  (unsigned char*) "±", (unsigned char*) "áâãä",
  (unsigned char*) "á", (unsigned char*) "±âãä",
  (unsigned char*) "â", (unsigned char*) "±áãä",
  (unsigned char*) "ã", (unsigned char*) "±áâä",
  (unsigned char*) "ä", (unsigned char*) "±áâã",

  (unsigned char*) "Æ", (unsigned char*) "æçè",
  (unsigned char*) "Ç", (unsigned char*) "æçè",
  (unsigned char*) "È", (unsigned char*) "æçè",
  (unsigned char*) "æ", (unsigned char*) "çè",
  (unsigned char*) "ç", (unsigned char*) "æè",
  (unsigned char*) "è", (unsigned char*) "æç",

  (unsigned char*) "Ï", (unsigned char*) "ïğ",
  (unsigned char*) "Ğ", (unsigned char*) "ïğ",
  (unsigned char*) "ï", (unsigned char*) "ğ",
  (unsigned char*) "ğ", (unsigned char*) "ï",

  (unsigned char*) "É", (unsigned char*) "éêëì",
  (unsigned char*) "Ê", (unsigned char*) "éêëì",
  (unsigned char*) "Ë", (unsigned char*) "éêëì",
  (unsigned char*) "Ì", (unsigned char*) "éêëì",
  (unsigned char*) "é", (unsigned char*) "êëì",
  (unsigned char*) "ê", (unsigned char*) "éëì",
  (unsigned char*) "ë", (unsigned char*) "éêì",
  (unsigned char*) "ì", (unsigned char*) "éêë",

  (unsigned char*) "Í", (unsigned char*) "íî",
  (unsigned char*) "Î", (unsigned char*) "íî",
  (unsigned char*) "í", (unsigned char*) "î",
  (unsigned char*) "î", (unsigned char*) "í",

  (unsigned char*) "£", (unsigned char*) "³µå",
  (unsigned char*) "¥", (unsigned char*) "³µå",
  (unsigned char*) "Å", (unsigned char*) "³µå",
  (unsigned char*) "³", (unsigned char*) "µå",
  (unsigned char*) "µ", (unsigned char*) "³å",
  (unsigned char*) "å", (unsigned char*) "³µ",

  (unsigned char*) "Ò", (unsigned char*) "òñ",
  (unsigned char*) "Ñ", (unsigned char*) "òñ",
  (unsigned char*) "ò", (unsigned char*) "ñ",
  (unsigned char*) "ñ", (unsigned char*) "ò",

  (unsigned char*) "Ó", (unsigned char*) "óôõö",
  (unsigned char*) "Ô", (unsigned char*) "óôõö",
  (unsigned char*) "Õ", (unsigned char*) "óôõö",
  (unsigned char*) "Õ", (unsigned char*) "óôõö",
  (unsigned char*) "ó", (unsigned char*) "ôõö",
  (unsigned char*) "ô", (unsigned char*) "óõö",
  (unsigned char*) "õ", (unsigned char*) "óôö",
  (unsigned char*) "ö", (unsigned char*) "óôõ",

  (unsigned char*) "À", (unsigned char*) "àø",
  (unsigned char*) "Ø", (unsigned char*) "àø",
  (unsigned char*) "à", (unsigned char*) "ø",
  (unsigned char*) "ø", (unsigned char*) "à",

  (unsigned char*) "¦", (unsigned char*) "¶¹º",
  (unsigned char*) "©", (unsigned char*) "¶¹º",
  (unsigned char*) "ª", (unsigned char*) "¶¹º",
  (unsigned char*) "¶", (unsigned char*) "¹º",
  (unsigned char*) "¹", (unsigned char*) "¶º",
  (unsigned char*) "º", (unsigned char*) "¶¹",

  (unsigned char*) "«", (unsigned char*) "ş»",
  (unsigned char*) "Ş", (unsigned char*) "ş»",
  (unsigned char*) "»", (unsigned char*) "ş",
  (unsigned char*) "ş", (unsigned char*) "»",

  (unsigned char*) "Ù", (unsigned char*) "ùúûü",
  (unsigned char*) "Ú", (unsigned char*) "ùúûü",
  (unsigned char*) "Û", (unsigned char*) "ùúûü",
  (unsigned char*) "Ü", (unsigned char*) "ùúûü",
  (unsigned char*) "ù", (unsigned char*) "úûü",
  (unsigned char*) "ú", (unsigned char*) "ùûü",
  (unsigned char*) "û", (unsigned char*) "ùúü",
  (unsigned char*) "ü", (unsigned char*) "ùúû",

  (unsigned char*) "¬", (unsigned char*) "¼¾¿",
  (unsigned char*) "®", (unsigned char*) "¼¾¿",
  (unsigned char*) "¯", (unsigned char*) "¼¾¿",
  (unsigned char*) "¼", (unsigned char*) "¾¿",
  (unsigned char*) "¾", (unsigned char*) "¼¿",
  (unsigned char*) "¿", (unsigned char*) "¼¾",
#endif /* latin2 */
};

/* remplacement des diacritics */
static unsigned char *remove_diacritics [] = {
#ifdef latin1
  (unsigned char*) "À", (unsigned char*) "a",
  (unsigned char*) "Á", (unsigned char*) "a",
  (unsigned char*) "Â", (unsigned char*) "a",
  (unsigned char*) "Å", (unsigned char*) "a",
  (unsigned char*) "à", (unsigned char*) "a",
  (unsigned char*) "á", (unsigned char*) "a",
  (unsigned char*) "â", (unsigned char*) "a",
  (unsigned char*) "å", (unsigned char*) "a",

  (unsigned char*) "È", (unsigned char*) "e",
  (unsigned char*) "É", (unsigned char*) "e",
  (unsigned char*) "Ê", (unsigned char*) "e",
  (unsigned char*) "Ë", (unsigned char*) "e",
  (unsigned char*) "è", (unsigned char*) "e",
  (unsigned char*) "é", (unsigned char*) "e",
  (unsigned char*) "ê", (unsigned char*) "e",
  (unsigned char*) "ë", (unsigned char*) "e",
  (unsigned char*) "ì", (unsigned char*) "e",

  (unsigned char*) "Î", (unsigned char*) "i",
  (unsigned char*) "Ï", (unsigned char*) "i",
  (unsigned char*) "Í", (unsigned char*) "i",
  (unsigned char*) "Ì", (unsigned char*) "i",
  (unsigned char*) "î", (unsigned char*) "i",
  (unsigned char*) "ï", (unsigned char*) "i",
  (unsigned char*) "í", (unsigned char*) "i",
  (unsigned char*) "ì", (unsigned char*) "i",

  (unsigned char*) "Ô", (unsigned char*) "o",
  (unsigned char*) "Ò", (unsigned char*) "o",
  (unsigned char*) "Ó", (unsigned char*) "o",
  (unsigned char*) "ô", (unsigned char*) "o",
  (unsigned char*) "ò", (unsigned char*) "o",
  (unsigned char*) "ó", (unsigned char*) "o",

  (unsigned char*) "Ù", (unsigned char*) "u",
  (unsigned char*) "Ú", (unsigned char*) "u",
  (unsigned char*) "Û", (unsigned char*) "u",
  (unsigned char*) "Ü", (unsigned char*) "u",
  (unsigned char*) "ù", (unsigned char*) "u",
  (unsigned char*) "ú", (unsigned char*) "u",
  (unsigned char*) "û", (unsigned char*) "u",
  (unsigned char*) "ü", (unsigned char*) "u",

  (unsigned char*) "ÿ", (unsigned char*) "y",
  (unsigned char*) "ı", (unsigned char*) "y",

  (unsigned char*) "Ç", (unsigned char*) "c",
  (unsigned char*) "ç", (unsigned char*) "c",
#endif /* latin1 */
#ifdef latin2
  (unsigned char*) "¡", (unsigned char*) "a",
  (unsigned char*) "Á", (unsigned char*) "a",
  (unsigned char*) "Â", (unsigned char*) "a",
  (unsigned char*) "Ã", (unsigned char*) "a",
  (unsigned char*) "Ä", (unsigned char*) "a",
  (unsigned char*) "±", (unsigned char*) "a",
  (unsigned char*) "á", (unsigned char*) "a",
  (unsigned char*) "â", (unsigned char*) "a",
  (unsigned char*) "ã", (unsigned char*) "a",
  (unsigned char*) "ä", (unsigned char*) "a",

  (unsigned char*) "Æ", (unsigned char*) "c",
  (unsigned char*) "Ç", (unsigned char*) "c",
  (unsigned char*) "È", (unsigned char*) "c",
  (unsigned char*) "æ", (unsigned char*) "c",
  (unsigned char*) "ç", (unsigned char*) "c",
  (unsigned char*) "è", (unsigned char*) "c",

  (unsigned char*) "Ï", (unsigned char*) "d",
  (unsigned char*) "Ğ", (unsigned char*) "d",
  (unsigned char*) "ï", (unsigned char*) "d",
  (unsigned char*) "ğ", (unsigned char*) "d",

  (unsigned char*) "É", (unsigned char*) "e",
  (unsigned char*) "Ê", (unsigned char*) "e",
  (unsigned char*) "Ë", (unsigned char*) "e",
  (unsigned char*) "Ì", (unsigned char*) "e",
  (unsigned char*) "é", (unsigned char*) "e",
  (unsigned char*) "ê", (unsigned char*) "e",
  (unsigned char*) "ë", (unsigned char*) "e",
  (unsigned char*) "ì", (unsigned char*) "e",

  (unsigned char*) "Í", (unsigned char*) "i",
  (unsigned char*) "Î", (unsigned char*) "i",
  (unsigned char*) "í", (unsigned char*) "i",
  (unsigned char*) "î", (unsigned char*) "i",

  (unsigned char*) "£", (unsigned char*) "l",
  (unsigned char*) "¥", (unsigned char*) "l",
  (unsigned char*) "Å", (unsigned char*) "l",
  (unsigned char*) "³", (unsigned char*) "l",
  (unsigned char*) "µ", (unsigned char*) "l",
  (unsigned char*) "å", (unsigned char*) "l",

  (unsigned char*) "Ò", (unsigned char*) "n",
  (unsigned char*) "Ñ", (unsigned char*) "n",
  (unsigned char*) "ò", (unsigned char*) "n",
  (unsigned char*) "ñ", (unsigned char*) "n",

  (unsigned char*) "Ó", (unsigned char*) "o",
  (unsigned char*) "Ô", (unsigned char*) "o",
  (unsigned char*) "Õ", (unsigned char*) "o",
  (unsigned char*) "Õ", (unsigned char*) "o",
  (unsigned char*) "ó", (unsigned char*) "o",
  (unsigned char*) "ô", (unsigned char*) "o",
  (unsigned char*) "õ", (unsigned char*) "o",
  (unsigned char*) "ö", (unsigned char*) "o",

  (unsigned char*) "À", (unsigned char*) "r",
  (unsigned char*) "Ø", (unsigned char*) "r",
  (unsigned char*) "à", (unsigned char*) "r",
  (unsigned char*) "ø", (unsigned char*) "r",

  (unsigned char*) "¦", (unsigned char*) "s",
  (unsigned char*) "©", (unsigned char*) "s",
  (unsigned char*) "ª", (unsigned char*) "s",
  (unsigned char*) "¶", (unsigned char*) "s",
  (unsigned char*) "¹", (unsigned char*) "s",
  (unsigned char*) "º", (unsigned char*) "s",

  (unsigned char*) "«", (unsigned char*) "t",
  (unsigned char*) "Ş", (unsigned char*) "t",
  (unsigned char*) "»", (unsigned char*) "t",
  (unsigned char*) "ş", (unsigned char*) "t",

  (unsigned char*) "Ù", (unsigned char*) "u",
  (unsigned char*) "Ú", (unsigned char*) "u",
  (unsigned char*) "Û", (unsigned char*) "u",
  (unsigned char*) "Ü", (unsigned char*) "u",
  (unsigned char*) "ù", (unsigned char*) "u",
  (unsigned char*) "ú", (unsigned char*) "u",
  (unsigned char*) "û", (unsigned char*) "u",
  (unsigned char*) "ü", (unsigned char*) "u",

  (unsigned char*) "İ", (unsigned char*) "y",
  (unsigned char*) "ı", (unsigned char*) "y",

  (unsigned char*) "¬", (unsigned char*) "z",
  (unsigned char*) "®", (unsigned char*) "z",
  (unsigned char*) "¯", (unsigned char*) "z",
  (unsigned char*) "¼", (unsigned char*) "z",
  (unsigned char*) "¾", (unsigned char*) "z",
  (unsigned char*) "¿", (unsigned char*) "z",
#endif /* latin2 */
#ifdef koi8r
  (unsigned char*) "£", (unsigned char*) "Å",
  (unsigned char*) "Ê", (unsigned char*) "É",
  (unsigned char*) "ê", (unsigned char*) "é",
#endif /* koi8r */
};

/* Doublement de lettres ... */
static unsigned char *m_en_mm [] = {
#if defined(latin1) || defined(latin2)
  (unsigned char*) "b", (unsigned char*) "bb",
  (unsigned char*) "c", (unsigned char*) "cc",
  (unsigned char*) "d", (unsigned char*) "dd",
  (unsigned char*) "é", (unsigned char*) "éé",
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
  (unsigned char*) "àà", (unsigned char*) "à",
  (unsigned char*) "ââ", (unsigned char*) "â",
  (unsigned char*) "èè", (unsigned char*) "è",
  (unsigned char*) "éé", (unsigned char*) "é",
  (unsigned char*) "êê", (unsigned char*) "ê",
  (unsigned char*) "ëë", (unsigned char*) "ë",
  (unsigned char*) "îî", (unsigned char*) "î",
  (unsigned char*) "ïï", (unsigned char*) "ï",
  (unsigned char*) "ôô", (unsigned char*) "ô",
  (unsigned char*) "ùù", (unsigned char*) "ù",
  (unsigned char*) "ûû", (unsigned char*) "û",
  (unsigned char*) "üü", (unsigned char*) "ü",
  (unsigned char*) "ÿÿ", (unsigned char*) "ÿ",
#endif /* latin1 */
};


/* ... et qq_trucs */
static unsigned char *qq_trucs [] = {
#if defined(latin1) || defined(latin2)
  (unsigned char*) "@", (unsigned char*) "a",
  (unsigned char*) "@", (unsigned char*) "à",
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
  (unsigned char*) "^A", (unsigned char*) "à",
  (unsigned char*) "^A", (unsigned char*) "â",

  (unsigned char*) "^E", (unsigned char*) "è",
  (unsigned char*) "^E", (unsigned char*) "é",
  (unsigned char*) "^E", (unsigned char*) "ê",
  (unsigned char*) "^E", (unsigned char*) "ë",

  (unsigned char*) "^I", (unsigned char*) "î",
  (unsigned char*) "^I", (unsigned char*) "ï",
  (unsigned char*) "^I", (unsigned char*) "í",
  (unsigned char*) "^I", (unsigned char*) "ì",

  (unsigned char*) "^O", (unsigned char*) "ô",
  (unsigned char*) "^O", (unsigned char*) "ó",
  (unsigned char*) "^O", (unsigned char*) "ò",

  (unsigned char*) "^U", (unsigned char*) "ù",
  (unsigned char*) "^U", (unsigned char*) "ú",
  (unsigned char*) "^U", (unsigned char*) "û",
  (unsigned char*) "^U", (unsigned char*) "ü",

  (unsigned char*) "^Y", (unsigned char*) "ÿ",
  (unsigned char*) "^Y", (unsigned char*) "ı",
  
  (unsigned char*) "^C", (unsigned char*) "ç",
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
  (unsigned char*) "^À", (unsigned char*) "à",
  (unsigned char*) "^Á", (unsigned char*) "á",
  (unsigned char*) "^Â", (unsigned char*) "â",
  (unsigned char*) "^Ã", (unsigned char*) "ã",
  (unsigned char*) "^Ä", (unsigned char*) "ä",
  (unsigned char*) "^Å", (unsigned char*) "å",
  (unsigned char*) "^Æ", (unsigned char*) "æ",
  (unsigned char*) "^Ç", (unsigned char*) "ç",
  (unsigned char*) "^È", (unsigned char*) "è",
  (unsigned char*) "^É", (unsigned char*) "é",
  (unsigned char*) "^Ê", (unsigned char*) "ê",
  (unsigned char*) "^Ë", (unsigned char*) "ë",
  (unsigned char*) "^Ì", (unsigned char*) "ì",
  (unsigned char*) "^Í", (unsigned char*) "í",
  (unsigned char*) "^Î", (unsigned char*) "î",
  (unsigned char*) "^Ï", (unsigned char*) "ï",
  (unsigned char*) "^Ğ", (unsigned char*) "ğ",
  (unsigned char*) "^Ñ", (unsigned char*) "ñ",
  (unsigned char*) "^Ò", (unsigned char*) "ò",
  (unsigned char*) "^Ó", (unsigned char*) "ó",
  (unsigned char*) "^Ô", (unsigned char*) "ô",
  (unsigned char*) "^Õ", (unsigned char*) "õ",
  (unsigned char*) "^Ö", (unsigned char*) "ö",
  (unsigned char*) "^Ø", (unsigned char*) "ø",
  (unsigned char*) "^Ù", (unsigned char*) "ù",
  (unsigned char*) "^Ú", (unsigned char*) "ú",
  (unsigned char*) "^Û", (unsigned char*) "û",
  (unsigned char*) "^Ü", (unsigned char*) "ü",
  (unsigned char*) "^İ", (unsigned char*) "ı",
  (unsigned char*) "^Ş", (unsigned char*) "ş",
#endif /* latin1 */
};
#endif /* IS_LIGHT_CORRECTION == 1 */

