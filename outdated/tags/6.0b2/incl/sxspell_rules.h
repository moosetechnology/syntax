#ifdef latin1
static char *MAJ_string = "ABCDEFGHIJKLMNOPQRSTUVWXYZÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏĞÑÒÓÔÕÖØÙÚÛÜİŞß";
static char *vowell_string = "AaEeIiOoUuYyÀàÁáÂâÃãÄäÅåÆæÈèÉéÊêËëÌìÍíÎîÏïÒòÓóÔôÕõÖöØøÙùÚúÛûÜüİı";
static char *mpb_string = "mpb";
#else /* latin1 */
#ifdef latin2
static char *MAJ_string = "ABCDEFGHIJKLMNOPQRSTUVWXYZ¡£¥¦©ª«¬®¯ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏĞÑÒÓÔÕÖØÙÚÛÜİŞ";
static char *vowell_string = "AaEeIiOoUuYy¡±ÁáÂâÃãÄäÉéÊêËëÌìÍíÎîÓóÕõÔôÖöÙùÚúÛûÜüİı";
static char *mpb_string = "mpb";
#endif /* latin2 */
#endif /* latin1 */

#ifdef latin1
static char *insert_string = "abcdefghijklmnopqrstuvwxyzâçèéêëîïôöûü";
static char *change_string = "abcdefghijklmnopqrstuvwxyzâçèéêëîïôöûü";
#else /* latin1 */
#ifdef latin2
static char *insert_string = "abcdefghijklmnopqrstuvwxyz±³µ¶¹º»¼¾¿àáâãäåæçèéêëìíîïğñòóôõöøùúûüış";
static char *change_string = "abcdefghijklmnopqrstuvwxyz±³µ¶¹º»¼¾¿àáâãäåæçèéêëìíîïğñòóôõöøùúûüış";
#endif /* latin2 */
#endif /* latin1 */


/* Poids de chaque type de correction */
/* Si le poids est négatif ou nul le coût de la composition est nul */
// utilisées dans sxspell pour le français
// 1. étaient dans sxspeller.c
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
// 2. étaient dans sxspell.h
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
  "À", "à",
  "Á", "á",
  "Â", "â",
  "Ã", "ã",
  "Ä", "ä",
  "Å", "å",
  "Æ", "æ",
  "Ç", "ç",
  "È", "è",
  "É", "é",
  "Ê", "ê",
  "Ë", "ë",
  "Ì", "ì",
  "Í", "í",
  "Î", "î",
  "Ï", "ï",
  "Ğ", "ğ",
  "Ñ", "ñ",
  "Ò", "ò",
  "Ó", "ó",
  "Ô", "ô",
  "Õ", "õ",
  "Ö", "ö",
  "Ø", "ø",
  "Ù", "ù",
  "Ú", "ú",
  "Û", "û",
  "Ü", "ü",
  "İ", "ı",
  "Ş", "ş",
#else /* latin1 */
#ifdef latin2
  "¡", "±",
  "£", "³",
  "¥", "µ",
  "¦", "¶",
  "©", "¹",
  "ª", "º",
  "«", "»",
  "¬", "¼",
  "®", "¾",
  "¯", "¿",
  "À", "à",
  "Á", "á",
  "Â", "â",
  "Ã", "ã",
  "Ä", "ä",
  "Å", "å",
  "Æ", "æ",
  "Ç", "ç",
  "È", "è",
  "É", "é",
  "Ê", "ê",
  "Ë", "ë",
  "Ì", "ì",
  "Í", "í",
  "Î", "î",
  "Ï", "ï",
  "Ğ", "ğ",
  "Ñ", "ñ",
  "Ò", "ò",
  "Ó", "ó",
  "Ô", "ô",
  "Õ", "õ",
  "Ö", "ö",
  "Ø", "ø",
  "Ù", "ù",
  "Ú", "ú",
  "Û", "û",
  "Ü", "ü",
  "İ", "ı",
  "Ş", "ş",
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
  "à", "À",
  "á", "Á",
  "â", "Â",
  "ã", "Ã",
  "ä", "Ä",
  "å", "Å",
  "æ", "Æ",
  "ç", "Ç",
  "è", "È",
  "é", "É",
  "ê", "Ê",
  "ë", "Ë",
  "ì", "Ì",
  "í", "Í",
  "î", "Î",
  "ï", "Ï",
  "ğ", "Ğ",
  "ñ", "Ñ",
  "ò", "Ò",
  "ó", "Ó",
  "ô", "Ô",
  "õ", "Õ",
  "ö", "Ö",
  "ø", "Ø",
  "ù", "Ù",
  "ú", "Ú",
  "û", "Û",
  "ü", "Ü",
  "ı", "İ",
  "ş", "Ş",
  "ÿ", "ß",
#endif /* latin1 */
};


/* remplacement des diacritics avec changement de casse possible */
static char *diacritics_and_case [] = {
#ifdef latin1
  "A", "ÀÂaàâ",
  "À", "AÂaàâ",
  "Á", "AÀÂaàâ",
  "Â", "AÀaàâ",
  "a", "AÀÂàâ",
  "à", "AÀÂaâ",
  "á", "AÀÂaàâ",
  "â", "AÀÂaà",

  "E", "ÈÉÊËeèéêë",
  "È", "EÉÊËeèéêë",
  "É", "EÈÊËeèéêë",
  "Ê", "EÈÉËeèéêë",
  "Ë", "EÈÉÊeèéêë",
  "e", "EÈÉÊËèéêë",
  "è", "EÈÉÊËeéêë",
  "é", "EÈÉÊËeèêë",
  "ê", "EÈÉÊËeèéë",
  "ë", "EÈÉÊËeèéê",

  "I", "ÎÏiîï",
  "Î", "IÏiîï",
  "Ï", "IÎiîï",
  "i", "IÎÏîï",
  "î", "IÎÏiï",
  "ï", "IÎÏiî",

  "O", "Ôoô",
  "Ô", "Ooô",
  "o", "OÔô",
  "ô", "OÔo",

  "U", "ÙÛÜuùûü",
  "Ù", "UÛÜuùûü",
  "Û", "UÙÜuùûü",
  "Ü", "UÙÛuùûü",
  "u", "UÙÛÜùûü",
  "ù", "UÙÛÜuûü",
  "û", "UÙÛÜuùü",
  "ü", "UÙÛÜuùû",

  "Y", "yÿ",
  "y", "Yÿ",
  "ÿ", "Yy",

  "C", "Çcç",
  "Ç", "Ccç",
  "c", "CÇç",
  "ç", "CÇc",
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
  "a", "àâeèêëiîïoôuùûüyÿ",
  "à", "aâeèêëiîïoôuùûüyÿ",
  "â", "aàeèêëiîïoôuùûüyÿ",
  "e", "aàâèêëiîïoôuùûüyÿ",
  "è", "aàâeêëiîïoôuùûüyÿ",
  "ê", "aàâeèëiîïoôuùûüyÿ",
  "ë", "aàâeèêiîïoôuùûüyÿ",
  "i", "aàâeèêëîïoôuùûüyÿ",
  "î", "aàâeèêëiïoôuùûüyÿ",
  "ï", "aàâeèêëiîoôuùûüyÿ",
  "o", "aàâeèêëiîïôuùûüyÿ",
  "ô", "aàâeèêëiîïouùûüyÿ",
  "u", "aàâeèêëiîïoôùûüyÿ",
  "ù", "aàâeèêëiîïoôuûüyÿ",
  "û", "aàâeèêëiîïoôuùüyÿ",
  "ü", "aàâeèêëiîïoôuùûyÿ",
  "y", "aàâeèêëiîïoôuùûüÿ",
  "ÿ", "aàâeèêëiîïoôuùûüy",
#endif /* latin1 */
};

/* fautes de frappe pour un clavier QWERTY ... */
static char *typos_qwerty [] = {
  "1", "q",
  "2", "qw",
  "3", "weèéêë",
  "4", "eèéêër",
  "5", "rt",
  "6", "ty",
  "7", "yuùûü",
  "8", "uùûüiîï",
  "9", "iîïoô",
  "0", "oôp",
  "-", "p",
  "=", "-",
  "[", "p",
  ";", "pl",
  ",", "klm",

  "q", "waàâ",
  "w", "qaàâseèéêë",
  "e", "wsdr",
  "r", "eèéêëdft",
  "t", "rfgyÿ",
  "y", "tghuùûü",
  "u", "yÿhjkiîï",
  "i", "uùûüjkoô",
  "o", "iîïklp",
  "p", "oôl-",
  "a", "zswq",
  "s", "qaàâzxdeèéêëw",
  "d", "eèéêësxcçfr",
  "f", "rdcçvgt",
  "g", "tfvbhyÿ",
  "h", "yÿgbnjuùûü",
  "j", "uùûühnmkiîï",
  "k", "iîïjmloô",
  "l", "oôkp",
  "z", "aàâsx",
  "x", "zsdcç",
  "c", "xdfv",
  "v", "cçfgb",
  "b", "vghn",
  "n", "bhjm",
  "m", "njk",

  "!", "Q",
  "@", "QW",
  "#", "WEÈÉÊË",
  "$", "EÈÉÊËR",
  "%", "RT",
  "^", "TY",
  "&", "YUÙÛÜ",
  "*", "UÙÛÜIÎÏ",
  "(", "IÎÏOÔ",
  ")", "OÔP",
  "_", "P",
  "+", "_",
  "{", "P",
  ":", "PL",
  "<", "KLM",

  "Q", "WAÀÂ",
  "W", "QAÀÂSEÈÉÊË",
  "E", "WSDR",
  "R", "EÈÉÊËDFT",
  "T", "RFGY",
  "Y", "TGHUÙÛÜ",
  "U", "YHJIÎÏ",
  "I", "UÙÛÜJKOÔ",
  "O", "IÎÏKLP",
  "P", "OÔL_",
  "A", "ZSWQ",
  "S", "QAÀÂZXDEÈÉÊËW",
  "D", "EÈÉÊËSXCÇFR",
  "F", "RDCÇVGT",
  "G", "TFVBHY",
  "H", "YGBNJUÙÛÜ",
  "J", "ÙÛÜUHNMKIÎÏ",
  "K", "IÎÏJMLOÔ",
  "L", "OÔKP",
  "Z", "AÀÂSX",
  "X", "ZSDCÇ",
  "C", "XDFV",
  "V", "CÇFGB",
  "B", "VGHN",
  "N", "BHJM",
  "M", "NJK",
};

/* Changement de clavier ... */
static char *qwerty_azerty [] = {
  "2", "é",
  "7", "è",
  "9", "ç",
  "0", "à",
  "q", "a",
  "w", "z",
  "a", "q",
  ";", "m",
  "z", "w",
  "m", ",",
};

static char *azerty_qwerty [] = {
  "é", "2",
  "è", "7",
  "ç", "9",
  "à", "0",
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

  "æ", "ae",
  "ae", "æ",
  "½", "oe",
  "e", "oe",
  "oe", "½",
  "oe", "e",

  /* Essai */
  /* Les autres combi sont faites par ailleurs */
  "c", "ss",
  "c", "s",
  "c", "sc",
  "ç", "ss",
  "ç", "s",
  "ss", "c",
  "ss", "ç",
  "s", "c",
  "sc", "c",
  "s", "ç",

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
  "cé", "ssé",
  "cé", "sé",
  "ssé", "cé",
  "ssé", "sé",
  "sé", "ssé",
  "sé", "cé",

  "ça", "ssa",
  "ça", "sa",
  "ssa", "ça",
  "ssa", "sa",
  "sa", "ssa",
  "sa", "ça",
  "ço", "sso",
  "ço", "so",
  "sso", "ço",
  "sso", "so",
  "so", "sso",
  "so", "ço",
  "çu", "ssu",
  "çu", "su",
  "ssu", "çu",
  "ssu", "su",
  "su", "ssu",
  "su", "çu",
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
  "aiss", "èss",
  "aiss", "êss",
  "aîss", "éss",
  "aîss", "èss",
  "aîss", "êss",

  "ess", "aiss",
  "èss", "aiss",
  "êss", "aiss",
  "éss", "aîss",
  "èss", "aîss",
  "êss", "aîss",

/* À quoi sert-ce ?
  "est", "é",
  "est", "è",
  "est", "ê",
*/

/* Hibou, chou, ... ; clou, sou, ... */
  "ous$", "oux",
  "oux$", "ous",

/* Finales de verbes
  "$", "ent",
*/
#endif /* french */
#ifdef polish
  "cz", "æ",
  "cz", "c",
  "c", "cz",
  "æ", "cz",
  "sz", "¶",
  "sz", "s",
  "s", "sz",
  "¶", "sz",
  "rz", "¼",
  "rz", "¿",
  "¿", "rz",
  "¼", "rz",
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
  "fiE", "É",
  "G", "É",
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
  "A", "ÀÁÂÅaáàâå",
  "À", "AÁÂÅaáàâå",
  "Á", "AÀÂÅaáàâå",
  "Â", "AÀÁÅaáàâå",
  "Å", "AÀÁÂaáàâå",
  "Æ", "æ",
  "B", "b",
  "C", "Çcç",
  "Ç", "Ccç",
  "D", "d",
  "E", "ÈÉÊËeèéêë",
  "È", "EÉÊËeèéêë",
  "É", "EÈÊËeèéêë",
  "Ê", "EÈÉËeèéêë",
  "Ë", "EÈÉÊeèéêë",
  "F", "f",
  "G", "g",
  "H", "h",
  "I", "ÎÏÌÍiîïìí",
  "Î", "IÏÌÍiîïìí",
  "Ï", "IÎÌÍiîïìí",
  "Ì", "IÎÏÍiîïìí",
  "Í", "IÎÏÌiîïìí",
  "J", "j",
  "K", "k",
  "L", "l",
  "M", "m",
  "N", "Ñn",
  "Ñ", "Nñ",
  "O", "ÔÒÓÕÖoôòóõö",
  "Ô", "OÒÓÕÖoôòóõö",
  "Ò", "OÔÓÕÖoôòóõö",
  "Ó", "OÔÒÕÖoôòóõö",
  "Õ", "OÔÒÓÖoôòóõö",
  "Ö", "OÔÒÓÕoôòóõö",
  "P", "p",
  "Q", "q",
  "R", "r",
  "S", "s",
  "T", "t",
  "U", "ÙÛÜuùûü",
  "Ù", "UÛÜuùûü",
  "Û", "UÙÜuùûü",
  "Ü", "UÙÛuùûü",
  "V", "v",
  "W", "w",
  "X", "x",
  "Y", "İyÿ",
  "İ", "Yyÿ",
  "Z", "z",
#else
#ifdef latin2
  "A", "¡ÁÂÃÄa±áâãä",
  "¡", "AÁÂÃÄa±áâãä",
  "Á", "A¡ÂÃÄa±áâãä",
  "Â", "A¡ÁÃÄa±áâãä",
  "Ã", "A¡ÁÂÄa±áâãä",
  "Ä", "A¡ÁÂÃa±áâãä",
  "B", "b",
  "C", "ÆÇÈcæçè",
  "Æ", "CÇÈcæçè",
  "Ç", "CÆÈcæçè",
  "È", "CÆÇcæçè",
  "D", "ÏĞdïğ",
  "Ï", "DĞdïğ",
  "Ğ", "DÏdïğ",
  "E", "ÉÊËÌeéêëì",
  "É", "EÊËÌeéêëì",
  "Ê", "EÉËÌeéêëì",
  "Ë", "EÉÊÌeéêëì",
  "Ì", "EÉÊËeéêëì",
  "F", "f",
  "G", "g",
  "H", "h",
  "I", "ÍÎiíî",
  "Í", "IÎiíî",
  "Î", "IÍiíî",
  "J", "j",
  "K", "k",
  "L", "£¥Ål³µå",
  "£", "L¥Ål³µå",
  "¥", "L£Ål³µå",
  "Å", "L£¥l³µå",
  "M", "m",
  "N", "ÒÑnòñ",
  "Ò", "NÑnòñ",
  "Ñ", "NÒnòñ",
  "O", "ÓÔÕÖoóôõö",
  "Ó", "OÔÕÖoóôõö",
  "Ô", "OÓÕÖoóôõö",
  "Õ", "OÓÔÖoóôõö",
  "Õ", "OÓÔÕoóôõö",
  "P", "p",
  "Q", "q",
  "R", "ÀØràø",
  "À", "RØràø",
  "Ø", "RÀràø",
  "S", "¦©ªs¶¹º",
  "¦", "S©ªs¶¹º",
  "©", "S¦ªs¶¹º",
  "ª", "S¦©s¶¹º",
  "T", "«Şt»ş",
  "«", "TŞtş»",
  "Ş", "T«tş»",
  "U", "ÙÚÛÜuùúûü",
  "Ù", "UÚÛÜuùúûü",
  "Ú", "UÙÛÜuùúûü",
  "Û", "UÙÚÜuùúûü",
  "Ü", "UÙÚÛuùúûü",
  "V", "v",
  "W", "w",
  "X", "x",
  "Y", "İyı",
  "İ", "Yyı",
  "Z", "¬®¯z¼¾¿",
  "¬", "Z®¯z¼¾¿",
  "®", "Z¬¯z¼¾¿",
  "¯", "Z¬®z¼¾¿",
#endif /* latin2 */
#endif /* latin1 */
};


/* ajout d'une apostrophe dans certains contextes - poids assez élevé : add_apos_weight */
static char *add_apos  [] = {
  "a", "a'",
  "â", "â'",
  "e", "e'",
  "é", "é'",
  "è", "è'",
  "ê", "ê'",
  "i", "i'",
  "î", "î'",
  "o", "o'",
  "ô", "ô'",
  "u", "u'",
  "û", "û'",
  "a'", "a",
  "â'", "â",
  "e'", "e",
  "é'", "é",
  "è'", "è",
  "ê'", "ê",
  "i'", "i",
  "î'", "î",
  "o'", "o",
  "ô'", "ô",
  "u'", "u",
  "û'", "û",
  ".'", "_",
};

/* abréviations sans leur point - poids très faible final_dot_weight */ 
static char *final_dot  [] = {
  "$", ".",
};


/* abréviations sans leur point - poids très faible final_dot_weight */ 
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

/* ajout final d'un e - poids assez élevé final_e_weight */
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

/* flexions mal fixées - poids faible  final_als_weight */
static char *final_als [] = {
  "als$", "aux",
  "aux$", "als",
  "ère$", "euse",
  "euse$", "ère",
  "ères$", "euses",
  "euses$", "ères",
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
  "ssè$", "s",
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

/* manque un u ou un e après un g pour en changer le son - poids moyen gu_ge_weight */
static char *gu_ge [] = {
  "ge", "gue",
  "gé", "gué",
  "gè", "guè",
  "gê", "guê",
  "gi", "gui",
  "gî", "guî",
  "ga", "gea",
  "gâ", "geâ",
  "go", "geo",
  "gô", "geô",
  "gu", "geu",
  "gû", "geû",
};

/* ajout d'un h dans certains contextes - poids moyen add_h_weight */
static char *add_h [] = {
  "a", "ah",
  "â", "âh",
  "e", "eh",
  "é", "éh",
  "è", "èh",
  "ê", "êh",
  "i", "ih",
  "î", "îh",
  "o", "oh",
  "ô", "ôh",
  "u", "uh",
  "û", "ûh",
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
  "A^", "Â",
  "A'", "Á",
  "A`", "À",
  "a^", "â",
  "a'", "á",
  "a`", "à",

  "E^", "Ê",
  "E'", "É",
  "E`", "È",
  "E\"", "Ë",
  "e^", "ê",
  "e'", "é",
  "e`", "è",
  "e\"", "ë",

  "I^", "Î",
  "I\"", "Ï",
  "i^", "î",
  "i\"", "ï",

  "O^", "Ô",
  "o^", "ô",

  "U^", "Û",
  "U`", "Ù",
  "U\"", "Ü",
  "u^", "û",
  "u`", "ù",
  "u\"", "ü",

  "y\"", "ÿ",

  "C,", "Ç",
  "c,", "ç",
#else /* latin1 */
#ifdef latin2
  "A,", "¡",
  "a,", "±",
  "L·", "¥",
  "l·", "µ",
  "S'", "¦",
  "s'", "¶",
  "S·", "©",
  "s·", "¹",
  "S,", "ª",
  "s,", "º",
  "T·", "«",
  "t·", "»",
  "Z'", "¬",
  "z'", "¼",
  "Z·", "®¯",
  "z·", "¾¿",
  "R'", "À",
  "r'", "à",
  "A'", "Á",
  "a'", "á",
  "A^", "ÂÃ",
  "a^", "âã",
  "A¨", "Ä",
  "a¨", "ä",
  "L'", "Å£",
  "l'", "å³",
  "C'", "Æ",
  "c'", "æ",
  "C,", "Ç",
  "c,", "ç",
  "C·", "È",
  "c·", "è",
  "E'", "É",
  "e'", "é",
  "E,", "Ê",
  "e,", "ê",
  "E¨", "Ë",
  "e¨", "ë",
  "E·", "Ì",
  "e·", "ì",
  "I'", "Í",
  "i'", "í",
  "I^", "Î",
  "i^", "î",
  "D·", "ÏĞ",
  "d·", "ïğ",
  "N'", "Ñ",
  "n'", "ñ",
  "N·", "Ò",
  "n·", "ò",
  "O'", "Ó",
  "o'", "ó",
  "O^", "Ô",
  "o^", "ô",
  "O\"", "Õ",
  "o\"", "õ",
  "O¨", "Ö",
  "o¨", "ö",
  "R·", "Ø",
  "r·", "ø",
  "U·", "Ù",
  "u·", "ù",
  "U'", "Ú",
  "u'", "ú",
  "U\"", "Û",
  "u\"", "û",
  "U¨", "Ü",
  "u¨", "ü",
  "Y'", "İ",
  "y'", "ı",
  "T,", "Ş",
  "t,", "ş",
#endif /* latin2 */
#endif /* latin1 */
};


/* remplacement des diacritics */
static char *diacritics [] = {
#ifdef latin1
  "A", "aáàâä",
  "À", "aáàâä",
  "Á", "aáàâä",
  "Â", "aáàâä",
  "a", "áàâä",
  "à", "aáâä",
  "á", "aàâä",
  "â", "aáàä",

  "E", "eèéêë",
  "È", "eèéêë",
  "É", "eèéêë",
  "Ê", "eèéêë",
  "Ë", "eèéêë",
  "e", "èéêë",
  "è", "eéêë",
  "é", "eèêë",
  "ê", "eèéë",
  "ë", "eèéê",
  "ì", "eé",

  "I", "iîïí",
  "Î", "iîï",
  "Ï", "iîï",
  "i", "îïí",
  "î", "iï",
  "ï", "iî",
  "í", "i",

  "O", "oô",
  "Ô", "oô",
  "o", "ô",
  "ô", "o",

  "U", "uùûü",
  "Ù", "uùûü",
  "Û", "uùûü",
  "Ü", "uùûü",
  "u", "ùûü",
  "ù", "uûü",
  "û", "uùü",
  "ü", "uùû",

  "Y", "yÿı",
  "y", "ÿı",
  "ÿ", "y",
  "ı", "y",

  "C", "cç",
  "Ç", "cç",
  "c", "ç",
  "ç", "c",
#else /* latin1 */
#ifdef latin2
  "A", "a±áâãä",
  "¡", "a±áâãä",
  "Á", "a±áâãä",
  "Â", "a±áâãä",
  "Ã", "a±áâãä",
  "Ä", "a±áâãä",
  "a", "±áâãä",
  "±", "aáâãä",
  "á", "a±âãä",
  "â", "a±áãä",
  "ã", "a±áâä",
  "ä", "a±áâã",

  "C", "cæçè",
  "Æ", "cæçè",
  "Ç", "cæçè",
  "È", "cæçè",
  "c", "æçè",
  "æ", "cçè",
  "ç", "cæè",
  "è", "cæç",

  "D", "dïğ",
  "Ï", "dïğ",
  "Ğ", "dïğ",
  "d", "ïğ",
  "ï", "dğ",
  "ğ", "dï",

  "E", "eéêëì",
  "É", "eéêëì",
  "Ê", "eéêëì",
  "Ë", "eéêëì",
  "Ì", "eéêëì",
  "e", "éêëì",
  "é", "eêëì",
  "ê", "eéëì",
  "ë", "eéêì",
  "ì", "eéêë",

  "I", "iíî",
  "Í", "iíî",
  "Î", "iíî",
  "i", "íî",
  "í", "iî",
  "î", "ií",

  "L", "l³µå",
  "£", "l³µå",
  "¥", "l³µå",
  "Å", "l³µå",
  "l", "³µå",
  "³", "lµå",
  "µ", "l³å",
  "å", "l³µ",

  "N", "nòñ",
  "Ò", "nòñ",
  "Ñ", "nòñ",
  "n", "òñ",
  "ò", "nñ",
  "ñ", "nò",

  "O", "oóôõö",
  "Ó", "oóôõö",
  "Ô", "oóôõö",
  "Õ", "oóôõö",
  "Õ", "oóôõö",
  "o", "óôõö",
  "ó", "oôõö",
  "ô", "oóõö",
  "õ", "oóôö",
  "ö", "oóôõ",

  "R", "ràø",
  "À", "ràø",
  "Ø", "ràø",
  "r", "àø",
  "à", "rø",
  "ø", "rà",

  "S", "s¶¹º",
  "¦", "s¶¹º",
  "©", "s¶¹º",
  "ª", "s¶¹º",
  "s", "¶¹º",
  "¶", "s¹º",
  "¹", "s¶º",
  "º", "s¶¹",

  "T", "t»ş",
  "«", "tş»",
  "Ş", "tş»",
  "t", "»ş",
  "»", "tş",
  "ş", "t»",

  "U", "uùúûü",
  "Ù", "uùúûü",
  "Ú", "uùúûü",
  "Û", "uùúûü",
  "Ü", "uùúûü",
  "u", "ùúûü",
  "ù", "uúûü",
  "ú", "uùûü",
  "û", "uùúü",
  "ü", "uùúû",

  "Y", "yı",
  "İ", "yı",
  "y", "ı",
  "ı", "y",

  "Z", "z¼¾¿",
  "¬", "z¼¾¿",
  "®", "z¼¾¿",
  "¯", "z¼¾¿",
  "z", "¼¾¿",
  "¼", "z¾¿",
  "¾", "z¼¿",
  "¿", "z¼¾",
#endif /* latin2 */
#endif /* latin1 */
};

/* Doublement de lettres ... */
static char *m_en_mm [] = {
  "b", "bb",
  "c", "cc",
  "d", "dd",
  "é", "éé",
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
  "àà", "à",
  "ââ", "â",
  "èè", "è",
  "éé", "é",
  "êê", "ê",
  "ëë", "ë",
  "îî", "î",
  "ïï", "ï",
  "ôô", "ô",
  "ùù", "ù",
  "ûû", "û",
  "üü", "ü",
  "ÿÿ", "ÿ",
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
  "^A", "à",
  "^A", "â",

  "^E", "è",
  "^E", "é",
  "^E", "ê",
  "^E", "ë",

  "^I", "î",
  "^I", "ï",

  "^O", "ô",

  "^U", "ù",
  "^U", "û",
  "^U", "ü",

  "^Y", "ÿ",
  
  "^C", "ç",
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
  "^À", "à",
  "^Á", "á",
  "^Â", "â",
  "^Ã", "ã",
  "^Ä", "ä",
  "^Å", "å",
  "^Æ", "æ",
  "^Ç", "ç",
  "^È", "è",
  "^É", "é",
  "^Ê", "ê",
  "^Ë", "ë",
  "^Ì", "ì",
  "^Í", "í",
  "^Î", "î",
  "^Ï", "ï",
  "^Ğ", "ğ",
  "^Ñ", "ñ",
  "^Ò", "ò",
  "^Ó", "ó",
  "^Ô", "ô",
  "^Õ", "õ",
  "^Ö", "ö",
  "^Ø", "ø",
  "^Ù", "ù",
  "^Ú", "ú",
  "^Û", "û",
  "^Ü", "ü",
  "^İ", "ı",
  "^Ş", "ş",
  "^ß", "ÿ",
};
#endif /* IS_LIGHT_CORRECTION == 1 */
