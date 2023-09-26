#ifdef latin1
static char *MAJ_string = "ABCDEFGHIJKLMNOPQRSTUVWXYZ�������������������������������";
static char *vowell_string = "AaEeIiOoUuYy����������������������������������������������������";
static char *mpb_string = "mpb";
#else /* latin1 */
#ifdef latin2
static char *MAJ_string = "ABCDEFGHIJKLMNOPQRSTUVWXYZ����������������������������������������";
static char *vowell_string = "AaEeIiOoUuYy����������������������������������������";
static char *mpb_string = "mpb";
#endif /* latin2 */
#endif /* latin1 */

#ifdef latin1
static char *insert_string = "abcdefghijklmnopqrstuvwxyz������������";
static char *change_string = "abcdefghijklmnopqrstuvwxyz������������";
#else /* latin1 */
#ifdef latin2
static char *insert_string = "abcdefghijklmnopqrstuvwxyz����������������������������������������";
static char *change_string = "abcdefghijklmnopqrstuvwxyz����������������������������������������";
#endif /* latin2 */
#endif /* latin1 */


/* Poids de chaque type de correction */
/* Si le poids est n�gatif ou nul le co�t de la composition est nul */
// utilis�es dans sxspell pour le fran�ais
// 1. �taient dans sxspeller.c
#define maj_min_weight	           (-1)
#define min_maj_weight	(-8)
#define diacritics_weight	           (-5)
#define twochars_to_diacritics_weight   (-5)
#define no_repeat_weight              (-5)
#define maj2diacritic_weight               (-3)
#define final_als_weight              (-10)
#define iI_en_l_weight                (-15)
#define final_h_weight                (-20)
#define eau_en_au_weight              (-20)
#define final_dot_weight              5
#define t_il_weight                   15
#define blanc_weight                  15
#define mm_en_m_weight                20
#define m_en_mm_weight                20
#define add_h_weight                  24
#define final_e_weight                25
#define gu_ge_weight                  30
#define dusse_je_weight               40
#define add_apos_weight               45
#define qq_trucs_weight    50
#define light_compose_weight               100
#define super_light_compose_weight               100
// 2. �taient dans sxspell.h
#define ortho_weight		 20
#define ortho2_weight		 (ortho_weight+2)
#define add_hyphen_weight	(-10)
#define typos_weight		 30
#define mauvaise_voyelle_weight	 40
#define qwerty_azerty_weight	 20
#define insert_weight            50
#define change_weight		 55
#define suppress_weight		 60
#define swap_weight		 40
#define compose_weight		100
// pour le dagger
/* pour traiter le cas des pre'fixes/suffixes laisse's ds cet etat par le spelleur ou le corpus :*/
#define final_underscore_weight              4

// autres trucs de sxspell.h
#define diacritics_and_case_weight	(-10)
#define space_hyphen_weight	(-10)
#define space_suppress_weight	(-10)
#define apos_hyphen_weight	(-10)
#define azerty_qwerty_weight	 20
#define ocr_weight		 25
#define add_weight		 50

/* Changement de casse */
static char *maj_min [] = {
  "A", "a",
  "B", "b",
  "C", "c",
  "D", "d",
  "E", "e",
  "F", "f",
  "G", "g",
  "H", "h",
  "I", "i",
  "J", "j",
  "K", "k",
  "L", "l",
  "M", "m",
  "N", "n",
  "O", "o",
  "P", "p",
  "Q", "q",
  "R", "r",
  "S", "s",
  "T", "t",
  "U", "u",
  "V", "v",
  "W", "w",
  "X", "x",
  "Y", "y",
  "Z", "z",
#ifdef latin1
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
#else /* latin1 */
#ifdef latin2
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
#endif /* latin2 */
#endif /* latin1 */
};


/* maj to min */
static char *min_maj [] = {
  "a", "A",
  "b", "B",
  "c", "C",
  "d", "D",
  "e", "E",
  "f", "F",
  "g", "G",
  "h", "H",
  "i", "I",
  "j", "J",
  "k", "K",
  "l", "L",
  "m", "M",
  "n", "N",
  "o", "O",
  "p", "P",
  "q", "Q",
  "r", "R",
  "s", "S",
  "t", "T",
  "u", "U",
  "v", "V",
  "w", "W",
  "x", "X",
  "y", "Y",
  "z", "Z",
#ifdef latin1
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
  "�", "�",
#endif /* latin1 */
};


/* remplacement des diacritics avec changement de casse possible */
static char *diacritics_and_case [] = {
#ifdef latin1
  "A", "��a��",
  "�", "A�a��",
  "�", "A��a��",
  "�", "A�a��",
  "a", "A����",
  "�", "A��a�",
  "�", "A��a��",
  "�", "A��a�",

  "E", "����e����",
  "�", "E���e����",
  "�", "E���e����",
  "�", "E���e����",
  "�", "E���e����",
  "e", "E��������",
  "�", "E����e���",
  "�", "E����e���",
  "�", "E����e���",
  "�", "E����e���",

  "I", "��i��",
  "�", "I�i��",
  "�", "I�i��",
  "i", "I����",
  "�", "I��i�",
  "�", "I��i�",

  "O", "�o�",
  "�", "Oo�",
  "o", "O��",
  "�", "O�o",

  "U", "���u���",
  "�", "U��u���",
  "�", "U��u���",
  "�", "U��u���",
  "u", "U������",
  "�", "U���u��",
  "�", "U���u��",
  "�", "U���u��",

  "Y", "y�",
  "y", "Y�",
  "�", "Yy",

  "C", "�c�",
  "�", "Cc�",
  "c", "C��",
  "�", "C�c",
#endif /* latin1 */
};

/* Suppression des blancs */
static char *space_suppress [] = {
  " ", "",
};

/* changement blanc/tiret */
static char *space_hyphen [] = {
  " ", "-",
  "-", " ",
};

/* changement apostrophe/tiret */
static char *apos_hyphen [] = {
  "'", "-",
  "-", "'",
};

/* On s'est trompe' de voyelle ... */
static char *mauvaise_voyelle [] = {
#ifdef latin1
  "a", "��e���i��o�u���y�",
  "�", "a�e���i��o�u���y�",
  "�", "a�e���i��o�u���y�",
  "e", "a�����i��o�u���y�",
  "�", "a��e��i��o�u���y�",
  "�", "a��e��i��o�u���y�",
  "�", "a��e��i��o�u���y�",
  "i", "a��e�����o�u���y�",
  "�", "a��e���i�o�u���y�",
  "�", "a��e���i�o�u���y�",
  "o", "a��e���i���u���y�",
  "�", "a��e���i��ou���y�",
  "u", "a��e���i��o����y�",
  "�", "a��e���i��o�u��y�",
  "�", "a��e���i��o�u��y�",
  "�", "a��e���i��o�u��y�",
  "y", "a��e���i��o�u����",
  "�", "a��e���i��o�u���y",
#endif /* latin1 */
};

/* fautes de frappe pour un clavier QWERTY ... */
static char *typos_qwerty [] = {
  "1", "q",
  "2", "qw",
  "3", "we����",
  "4", "e����r",
  "5", "rt",
  "6", "ty",
  "7", "yu���",
  "8", "u���i��",
  "9", "i��o�",
  "0", "o�p",
  "-", "p",
  "=", "-",
  "[", "p",
  ";", "pl",
  ",", "klm",

  "q", "wa��",
  "w", "qa��se����",
  "e", "wsdr",
  "r", "e����dft",
  "t", "rfgy�",
  "y", "tghu���",
  "u", "y�hjki��",
  "i", "u���jko�",
  "o", "i��klp",
  "p", "o�l-",
  "a", "zswq",
  "s", "qa��zxde����w",
  "d", "e����sxc�fr",
  "f", "rdc�vgt",
  "g", "tfvbhy�",
  "h", "y�gbnju���",
  "j", "u���hnmki��",
  "k", "i��jmlo�",
  "l", "o�kp",
  "z", "a��sx",
  "x", "zsdc�",
  "c", "xdfv",
  "v", "c�fgb",
  "b", "vghn",
  "n", "bhjm",
  "m", "njk",

  "!", "Q",
  "@", "QW",
  "#", "WE����",
  "$", "E����R",
  "%", "RT",
  "^", "TY",
  "&", "YU���",
  "*", "U���I��",
  "(", "I��O�",
  ")", "O�P",
  "_", "P",
  "+", "_",
  "{", "P",
  ":", "PL",
  "<", "KLM",

  "Q", "WA��",
  "W", "QA��SE����",
  "E", "WSDR",
  "R", "E����DFT",
  "T", "RFGY",
  "Y", "TGHU���",
  "U", "YHJI��",
  "I", "U���JKO�",
  "O", "I��KLP",
  "P", "O�L_",
  "A", "ZSWQ",
  "S", "QA��ZXDE����W",
  "D", "E����SXC�FR",
  "F", "RDC�VGT",
  "G", "TFVBHY",
  "H", "YGBNJU���",
  "J", "���UHNMKI��",
  "K", "I��JMLO�",
  "L", "O�KP",
  "Z", "A��SX",
  "X", "ZSDC�",
  "C", "XDFV",
  "V", "C�FGB",
  "B", "VGHN",
  "N", "BHJM",
  "M", "NJK",
};

/* Changement de clavier ... */
static char *qwerty_azerty [] = {
  "2", "�",
  "7", "�",
  "9", "�",
  "0", "�",
  "q", "a",
  "w", "z",
  "a", "q",
  ";", "m",
  "z", "w",
  "m", ",",
};

static char *azerty_qwerty [] = {
  "�", "2",
  "�", "7",
  "�", "9",
  "�", "0",
  "a", "q",
  "z", "w",
  "q", "a",
  "z", ";",
  "w", "m",
  ",", "m",
};

/* qq fautes */
static char *ortho [] = {
#ifdef french
  /* pluriels des mots composes */
  "s-", "-",
  "-", "s-",

  "o", "au",
  "o", "eau",
  "au", "o",
  "au", "eau",
  "eau", "o",
  "eau", "au",

  "�", "ae",
  "ae", "�",
  "�", "oe",
  "e", "oe",
  "oe", "�",
  "oe", "e",

  /* Essai */
  /* Les autres combi sont faites par ailleurs */
  "c", "ss",
  "c", "s",
  "c", "sc",
  "�", "ss",
  "�", "s",
  "ss", "c",
  "ss", "�",
  "s", "c",
  "sc", "c",
  "s", "�",

#if 0
  "ci", "ssi",
  "ci", "si",
  "ssi", "ci",
  "ssi", "si",
  "si", "ssi",
  "si", "ci",
  "ce", "sse",
  "ce", "se",
  "sse", "ce",
  "sse", "se",
  "se", "sse",
  "se", "ce",
  "c�", "ss�",
  "c�", "s�",
  "ss�", "c�",
  "ss�", "s�",
  "s�", "ss�",
  "s�", "c�",

  "�a", "ssa",
  "�a", "sa",
  "ssa", "�a",
  "ssa", "sa",
  "sa", "ssa",
  "sa", "�a",
  "�o", "sso",
  "�o", "so",
  "sso", "�o",
  "sso", "so",
  "so", "sso",
  "so", "�o",
  "�u", "ssu",
  "�u", "su",
  "ssu", "�u",
  "ssu", "su",
  "su", "ssu",
  "su", "�u",
#endif /* 0 */

  "em", "am",
  "en", "an",
  "am", "em",
  "an", "en",

  "nb", "mb",
  "np", "mp",

  "rh", "r",
  "th", "t",
  "r", "rh",
  "t", "th",

  "ph", "f",
  "f", "ph",

  "g", "j",
  "j", "g",
  "ge", "j",
  "j", "ge",

  "k", "c",
  "k", "qu",
  "c", "k",
  "c", "qu",
  "qu", "c",
  "qu", "k",

  "ch", "sch",
  "ch", "sh",
  "sch", "ch",
  "sch", "sh",
  "sh", "ch",
  "sh", "sch",

  "ain", "in",
  "ain", "ein",
  "in", "ain",
  "in", "ein",
  "ein", "ain",
  "ein", "ain",

  "aiss", "ess", /* si suivi par exemple de ss */
  "aiss", "�ss",
  "aiss", "�ss",
  "a�ss", "�ss",
  "a�ss", "�ss",
  "a�ss", "�ss",

  "ess", "aiss",
  "�ss", "aiss",
  "�ss", "aiss",
  "�ss", "a�ss",
  "�ss", "a�ss",
  "�ss", "a�ss",

/* � quoi sert-ce ?
  "est", "�",
  "est", "�",
  "est", "�",
*/

/* Hibou, chou, ... ; clou, sou, ... */
  "ous$", "oux",
  "oux$", "ous",

/* Finales de verbes
  "$", "ent",
*/
#endif /* french */
#ifdef polish
  "cz", "�",
  "cz", "c",
  "c", "cz",
  "�", "cz",
  "sz", "�",
  "sz", "s",
  "s", "sz",
  "�", "sz",
  "rz", "�",
  "rz", "�",
  "�", "rz",
  "�", "rz",
  "ch", "h",
  "h", "ch",
#endif /* polish */
};

static char *ortho2 [] = {
  "ci", "ti",
  "ti", "ci",
  "si", "ti",
  "ti", "si",
  "ssi", "ti",
  "ti", "ssi",

  "k", "ch",
  "c", "ch",
  "ch", "c",
  "ch", "k",
  "ch", "qu",
  "qu", "ch",

  "ch", "tch",
  "tch", "ch",
};


/* qq fautes specifiques a` l'OCR */
static char *ocr [] = {
  "fiE", "�",
  "G", "�",
  "I3", "B",
  "8", "B",
  "B", "O",
  "1", "I",
  "1", "l",
  "I", "l",
  "l", "I",
  "h", "tu",
  "s", "x",
  "ri", "n",
  "d", "a",
};


/* Chaque majuscule est change'e en toutes ses variations    (-5) */
static char *maj2diacritic [] = {
#ifdef latin1
  "A", "����a����",
  "�", "A���a����",
  "�", "A���a����",
  "�", "A���a����",
  "�", "A���a����",
  "�", "�",
  "B", "b",
  "C", "�c�",
  "�", "Cc�",
  "D", "d",
  "E", "����e����",
  "�", "E���e����",
  "�", "E���e����",
  "�", "E���e����",
  "�", "E���e����",
  "F", "f",
  "G", "g",
  "H", "h",
  "I", "����i����",
  "�", "I���i����",
  "�", "I���i����",
  "�", "I���i����",
  "�", "I���i����",
  "J", "j",
  "K", "k",
  "L", "l",
  "M", "m",
  "N", "�n",
  "�", "N�",
  "O", "�����o�����",
  "�", "O����o�����",
  "�", "O����o�����",
  "�", "O����o�����",
  "�", "O����o�����",
  "�", "O����o�����",
  "P", "p",
  "Q", "q",
  "R", "r",
  "S", "s",
  "T", "t",
  "U", "���u���",
  "�", "U��u���",
  "�", "U��u���",
  "�", "U��u���",
  "V", "v",
  "W", "w",
  "X", "x",
  "Y", "�y�",
  "�", "Yy�",
  "Z", "z",
#else
#ifdef latin2
  "A", "�����a�����",
  "�", "A����a�����",
  "�", "A����a�����",
  "�", "A����a�����",
  "�", "A����a�����",
  "�", "A����a�����",
  "B", "b",
  "C", "���c���",
  "�", "C��c���",
  "�", "C��c���",
  "�", "C��c���",
  "D", "��d��",
  "�", "D�d��",
  "�", "D�d��",
  "E", "����e����",
  "�", "E���e����",
  "�", "E���e����",
  "�", "E���e����",
  "�", "E���e����",
  "F", "f",
  "G", "g",
  "H", "h",
  "I", "��i��",
  "�", "I�i��",
  "�", "I�i��",
  "J", "j",
  "K", "k",
  "L", "���l���",
  "�", "L��l���",
  "�", "L��l���",
  "�", "L��l���",
  "M", "m",
  "N", "��n��",
  "�", "N�n��",
  "�", "N�n��",
  "O", "����o����",
  "�", "O���o����",
  "�", "O���o����",
  "�", "O���o����",
  "�", "O���o����",
  "P", "p",
  "Q", "q",
  "R", "��r��",
  "�", "R�r��",
  "�", "R�r��",
  "S", "���s���",
  "�", "S��s���",
  "�", "S��s���",
  "�", "S��s���",
  "T", "��t��",
  "�", "T�t��",
  "�", "T�t��",
  "U", "����u����",
  "�", "U���u����",
  "�", "U���u����",
  "�", "U���u����",
  "�", "U���u����",
  "V", "v",
  "W", "w",
  "X", "x",
  "Y", "�y�",
  "�", "Yy�",
  "Z", "���z���",
  "�", "Z��z���",
  "�", "Z��z���",
  "�", "Z��z���",
#endif /* latin2 */
#endif /* latin1 */
};


/* ajout d'une apostrophe dans certains contextes - poids assez �lev� : add_apos_weight */
static char *add_apos  [] = {
  "a", "a'",
  "�", "�'",
  "e", "e'",
  "�", "�'",
  "�", "�'",
  "�", "�'",
  "i", "i'",
  "�", "�'",
  "o", "o'",
  "�", "�'",
  "u", "u'",
  "�", "�'",
  "a'", "a",
  "�'", "�",
  "e'", "e",
  "�'", "�",
  "�'", "�",
  "�'", "�",
  "i'", "i",
  "�'", "�",
  "o'", "o",
  "�'", "�",
  "u'", "u",
  "�'", "�",
  ".'", "_",
};

/* abr�viations sans leur point - poids tr�s faible final_dot_weight */ 
static char *final_dot  [] = {
  "$", ".",
};


/* abr�viations sans leur point - poids tr�s faible final_dot_weight */ 
static char *final_underscore  [] = {
  "$", "_",
  "^", "_",
};

/* ajout d'un h initial ou final - poids moyen final_h_weight */
static char *final_h  [] = {
  "$", "h",
  "^", "h",
  "^z", "j",
};

/* ajout final d'un e - poids assez �lev� final_e_weight */
static char *final_e  [] = {
  "$", "e",
  "ts$", "s",
  "e$", "",
  "s$", "",
  "^z", "",
  "euh$", "e",
  "^x", "ex",
  "'$", "",
};

/* flexions mal fix�es - poids faible  final_als_weight */
static char *final_als [] = {
  "als$", "aux",
  "aux$", "als",
  "�re$", "euse",
  "euse$", "�re",
  "�res$", "euses",
  "euses$", "�res",
  "oye$", "oie",
  "oyes$", "oies",
  "oyent$", "oient",
  "uye$", "uie",
  "uyes$", "uies",
  "uyent$", "uient",
};

/* i' ou I'- poids faible iI_en_l_weight */
static char *iI_en_l [] = {
  "^i'$", "l'",
  "^I'$", "l'",
};

/* i' ou I'- poids faible dusse_je_weight */
static char *dusse_je [] = {
  "ss�$", "s",
};

/* -t-il et similaires incomplets - poids faible t_il_weight */
static char *t_il [] = {
  "^-t", "-t-",
  "t$", "",
};

/* Suppression de l'espace ou remplacement par un tiret - poids faible blanc_weight */
static char *blanc [] = {
  " ", "",
  " ", "-",
  "-", "",
};

/* eau <-> au - poids assez faible eau_en_au_weight */
static char *eau_en_au [] = {
  "eau", "au",
  "au", "eau",
};

/* manque un u ou un e apr�s un g pour en changer le son - poids moyen gu_ge_weight */
static char *gu_ge [] = {
  "ge", "gue",
  "g�", "gu�",
  "g�", "gu�",
  "g�", "gu�",
  "gi", "gui",
  "g�", "gu�",
  "ga", "gea",
  "g�", "ge�",
  "go", "geo",
  "g�", "ge�",
  "gu", "geu",
  "g�", "ge�",
};

/* ajout d'un h dans certains contextes - poids moyen add_h_weight */
static char *add_h [] = {
  "a", "ah",
  "�", "�h",
  "e", "eh",
  "�", "�h",
  "�", "�h",
  "�", "�h",
  "i", "ih",
  "�", "�h",
  "o", "oh",
  "�", "�h",
  "u", "uh",
  "�", "�h",
  "k", "kh",
  "t", "th",
  "p", "ph",
  "d", "dh",
  "g", "gh",
  "r", "rh",
  "b", "bh",
  "z", "zh",
  "s", "sh",
  "c", "ch",
  "j", "dj",
  "dj", "j",
};



/* diacritics ecrits comme 2 caracteres successifs ... */
static char *twochars_to_diacritics [] = {
#ifdef latin1
  "A^", "�",
  "A'", "�",
  "A`", "�",
  "a^", "�",
  "a'", "�",
  "a`", "�",

  "E^", "�",
  "E'", "�",
  "E`", "�",
  "E\"", "�",
  "e^", "�",
  "e'", "�",
  "e`", "�",
  "e\"", "�",

  "I^", "�",
  "I\"", "�",
  "i^", "�",
  "i\"", "�",

  "O^", "�",
  "o^", "�",

  "U^", "�",
  "U`", "�",
  "U\"", "�",
  "u^", "�",
  "u`", "�",
  "u\"", "�",

  "y\"", "�",

  "C,", "�",
  "c,", "�",
#else /* latin1 */
#ifdef latin2
  "A,", "�",
  "a,", "�",
  "L�", "�",
  "l�", "�",
  "S'", "�",
  "s'", "�",
  "S�", "�",
  "s�", "�",
  "S,", "�",
  "s,", "�",
  "T�", "�",
  "t�", "�",
  "Z'", "�",
  "z'", "�",
  "Z�", "��",
  "z�", "��",
  "R'", "�",
  "r'", "�",
  "A'", "�",
  "a'", "�",
  "A^", "��",
  "a^", "��",
  "A�", "�",
  "a�", "�",
  "L'", "ţ",
  "l'", "�",
  "C'", "�",
  "c'", "�",
  "C,", "�",
  "c,", "�",
  "C�", "�",
  "c�", "�",
  "E'", "�",
  "e'", "�",
  "E,", "�",
  "e,", "�",
  "E�", "�",
  "e�", "�",
  "E�", "�",
  "e�", "�",
  "I'", "�",
  "i'", "�",
  "I^", "�",
  "i^", "�",
  "D�", "��",
  "d�", "��",
  "N'", "�",
  "n'", "�",
  "N�", "�",
  "n�", "�",
  "O'", "�",
  "o'", "�",
  "O^", "�",
  "o^", "�",
  "O\"", "�",
  "o\"", "�",
  "O�", "�",
  "o�", "�",
  "R�", "�",
  "r�", "�",
  "U�", "�",
  "u�", "�",
  "U'", "�",
  "u'", "�",
  "U\"", "�",
  "u\"", "�",
  "U�", "�",
  "u�", "�",
  "Y'", "�",
  "y'", "�",
  "T,", "�",
  "t,", "�",
#endif /* latin2 */
#endif /* latin1 */
};


/* remplacement des diacritics */
static char *diacritics [] = {
#ifdef latin1
  "A", "a����",
  "�", "a����",
  "�", "a����",
  "�", "a����",
  "a", "����",
  "�", "a���",
  "�", "a���",
  "�", "a���",

  "E", "e����",
  "�", "e����",
  "�", "e����",
  "�", "e����",
  "�", "e����",
  "e", "����",
  "�", "e���",
  "�", "e���",
  "�", "e���",
  "�", "e���",
  "�", "e�",

  "I", "i���",
  "�", "i��",
  "�", "i��",
  "i", "���",
  "�", "i�",
  "�", "i�",
  "�", "i",

  "O", "o�",
  "�", "o�",
  "o", "�",
  "�", "o",

  "U", "u���",
  "�", "u���",
  "�", "u���",
  "�", "u���",
  "u", "���",
  "�", "u��",
  "�", "u��",
  "�", "u��",

  "Y", "y��",
  "y", "��",
  "�", "y",
  "�", "y",

  "C", "c�",
  "�", "c�",
  "c", "�",
  "�", "c",
#else /* latin1 */
#ifdef latin2
  "A", "a�����",
  "�", "a�����",
  "�", "a�����",
  "�", "a�����",
  "�", "a�����",
  "�", "a�����",
  "a", "�����",
  "�", "a����",
  "�", "a����",
  "�", "a����",
  "�", "a����",
  "�", "a����",

  "C", "c���",
  "�", "c���",
  "�", "c���",
  "�", "c���",
  "c", "���",
  "�", "c��",
  "�", "c��",
  "�", "c��",

  "D", "d��",
  "�", "d��",
  "�", "d��",
  "d", "��",
  "�", "d�",
  "�", "d�",

  "E", "e����",
  "�", "e����",
  "�", "e����",
  "�", "e����",
  "�", "e����",
  "e", "����",
  "�", "e���",
  "�", "e���",
  "�", "e���",
  "�", "e���",

  "I", "i��",
  "�", "i��",
  "�", "i��",
  "i", "��",
  "�", "i�",
  "�", "i�",

  "L", "l���",
  "�", "l���",
  "�", "l���",
  "�", "l���",
  "l", "���",
  "�", "l��",
  "�", "l��",
  "�", "l��",

  "N", "n��",
  "�", "n��",
  "�", "n��",
  "n", "��",
  "�", "n�",
  "�", "n�",

  "O", "o����",
  "�", "o����",
  "�", "o����",
  "�", "o����",
  "�", "o����",
  "o", "����",
  "�", "o���",
  "�", "o���",
  "�", "o���",
  "�", "o���",

  "R", "r��",
  "�", "r��",
  "�", "r��",
  "r", "��",
  "�", "r�",
  "�", "r�",

  "S", "s���",
  "�", "s���",
  "�", "s���",
  "�", "s���",
  "s", "���",
  "�", "s��",
  "�", "s��",
  "�", "s��",

  "T", "t��",
  "�", "t��",
  "�", "t��",
  "t", "��",
  "�", "t�",
  "�", "t�",

  "U", "u����",
  "�", "u����",
  "�", "u����",
  "�", "u����",
  "�", "u����",
  "u", "����",
  "�", "u���",
  "�", "u���",
  "�", "u���",
  "�", "u���",

  "Y", "y�",
  "�", "y�",
  "y", "�",
  "�", "y",

  "Z", "z���",
  "�", "z���",
  "�", "z���",
  "�", "z���",
  "z", "���",
  "�", "z��",
  "�", "z��",
  "�", "z��",
#endif /* latin2 */
#endif /* latin1 */
};

/* Doublement de lettres ... */
static char *m_en_mm [] = {
  "b", "bb",
  "c", "cc",
  "d", "dd",
  "�", "��",
  "f", "ff",
  "g", "gg",
  "l", "ll",
  "m", "mm",
  "n", "nn",
  "o", "oo",
  "p", "pp",
  "r", "rr",
  "s", "ss",
  "t", "tt",
  "z", "zz",
};

/* ... et pas de doublement */
static char *mm_en_m [] = {
  "aa", "a",
  "bb", "b",
  "cc", "c",
  "dd", "d",
  "ee", "e",
  "ff", "f",
  "gg", "g",
  "hh", "h",
  "ii", "i",
  "jj", "j",
  "kk", "k",
  "ll", "l",
  "mm", "m",
  "nn", "n",
  "oo", "o",
  "pp", "p",
  "qq", "q",
  "rr", "r",
  "ss", "s",
  "tt", "t",
  "uu", "u",
  "vv", "v",
  "ww", "w",
  "xx", "x",
  "yy", "y",
  "zz", "z",
#ifdef latin1
  "��", "�",
  "��", "�",
  "��", "�",
  "��", "�",
  "��", "�",
  "��", "�",
  "��", "�",
  "��", "�",
  "��", "�",
  "��", "�",
  "��", "�",
  "��", "�",
  "��", "�",
#endif /* latin1 */
};


/* ... et qq_trucs */
static char *qq_trucs [] = {
  "@", "a",
  "@", "",
#ifdef latin1
  "K", "ca",
  "k", "ca",
#endif /* latin1 */
};


/* Corrections issues de text2dag_semact */
#if IS_LIGHT_CORRECTION == 1
/* Capital_initial */
static char *capital_initial2min_diacritic [] = {
  "^A", "�",
  "^A", "�",

  "^E", "�",
  "^E", "�",
  "^E", "�",
  "^E", "�",

  "^I", "�",
  "^I", "�",

  "^O", "�",

  "^U", "�",
  "^U", "�",
  "^U", "�",

  "^Y", "�",
  
  "^C", "�",
};
/* Changement de casse */
static char *capital_initial2min [] = {
  "^A", "a",
  "^B", "b",
  "^C", "c",
  "^D", "d",
  "^E", "e",
  "^F", "f",
  "^G", "g",
  "^H", "h",
  "^I", "i",
  "^J", "j",
  "^K", "k",
  "^L", "l",
  "^M", "m",
  "^N", "n",
  "^O", "o",
  "^P", "p",
  "^Q", "q",
  "^R", "r",
  "^S", "s",
  "^T", "t",
  "^U", "u",
  "^V", "v",
  "^W", "w",
  "^X", "x",
  "^Y", "y",
  "^Z", "z",
  "^�", "�",
  "^�", "�",
  "^�", "�",
  "^�", "�",
  "^�", "�",
  "^�", "�",
  "^�", "�",
  "^�", "�",
  "^�", "�",
  "^�", "�",
  "^�", "�",
  "^�", "�",
  "^�", "�",
  "^�", "�",
  "^�", "�",
  "^�", "�",
  "^�", "�",
  "^�", "�",
  "^�", "�",
  "^�", "�",
  "^�", "�",
  "^�", "�",
  "^�", "�",
  "^�", "�",
  "^�", "�",
  "^�", "�",
  "^�", "�",
  "^�", "�",
  "^�", "�",
  "^�", "�",
  "^�", "�",
};
#endif /* IS_LIGHT_CORRECTION == 1 */
