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

/* Transcodage de caract�res */


#include "sxversion.h"
#include "sxunix.h"

char WHAT_SXC_TO_STR[] = "@(#)SYNTAX - $Id: sxc_to_str.c 2428 2023-01-18 12:54:10Z garavel $" WHAT_DEBUG;



/******************************************************************/
/*								  */
/* Ces tableaux sont pr�vus pour �tre indic�s � partir de leur	  */
/* �l�ment n�128 consid�r� comme repr�sentant le caract�re \0.	  */
/* Les indices effectifs peuvent donc �tre des caract�res sign�s  */
/* ou non sign�s, entre -128 et +255.				  */
/*								  */
/******************************************************************/



char	*sxc_to_str [384] =
{
  "\\200", "\\201", "\\202", "\\203", "\\204", "\\205", "\\206", "\\207",
  "\\210", "\\211", "\\212", "\\213", "\\214", "\\215", "\\216", "\\217",
  "\\220", "\\221", "\\222", "\\223", "\\224", "\\225", "\\226", "\\227",
  "\\230", "\\231", "\\232", "\\233", "\\234", "\\235", "\\236", "\\237",
  "\\240", "\\241", "\\242", "\\243", "\\244", "\\245", "\\246", "\\247", 
  "\\250", "\\251", "\\252", "\\253", "\\254", "\\255", "\\256", "\\257",
  "\\260", "\\261", "\\262", "\\263", "\\264", "\\265", "\\266", "\\267",
  "\\270", "\\271", "\\272", "\\273", "\\274", "\\275", "\\276", "\\277",
  "\\300", "\\301", "\\302", "\\303", "\\304", "\\305", "\\306", "\\307",
  "\\310", "\\311", "\\312", "\\313", "\\314", "\\315", "\\316", "\\317",
  "\\320", "\\321", "\\322", "\\323", "\\324", "\\325", "\\326", "\\327",
  "\\330", "\\331", "\\332", "\\333", "\\334", "\\335", "\\336", "\\337",
  "\\340", "\\341", "\\342", "\\343", "\\344", "\\345", "\\346", "\\347",
  "\\350", "\\351", "\\352", "\\353", "\\354", "\\355", "\\356", "\\357",
  "\\360", "\\361", "\\362", "\\363", "\\364", "\\365", "\\366", "\\367",
  "\\370", "\\371", "\\372", "\\373", "\\374", "\\375", "\\376", "\\377",
  "\\000", "\\001", "\\002", "\\003", "\\004", "\\005", "\\006", "\\007",
  "\\b", "\\t", "\\n", "\\013", "\\f", "\\r", "\\016", "\\017",
  "\\020", "\\021", "\\022", "\\023", "\\024", "\\025", "\\026", "\\027",
  "\\030", "\\031", "\\032", "\\033", "\\034", "\\035", "\\036", "\\037",
  " ", "!", "\\\"", "#", "$", "%", "&", "'",
  "(", ")", "*", "+", ",", "-", ".", "/",
  "0", "1", "2", "3", "4", "5", "6", "7",
  "8", "9", ":", ";", "<", "=", ">", "?",
  "@", "A", "B", "C", "D", "E", "F", "G",
  "H", "I", "J", "K", "L", "M", "N", "O",
  "P", "Q", "R", "S", "T", "U", "V", "W",
  "X", "Y", "Z", "[", "\\\\", "]", "^", "_",
  "`", "a", "b", "c", "d", "e", "f", "g",
  "h", "i", "j", "k", "l" , "m", "n", "o",
  "p", "q", "r", "s", "t", "u", "v", "w",
  "x", "y", "z", "{", "|", "}", "~", "\\177",
  "\\200", "\\201", "\\202", "\\203", "\\204", "\\205", "\\206", "\\207",
  "\\210", "\\211", "\\212", "\\213", "\\214", "\\215", "\\216", "\\217",
  "\\220", "\\221", "\\222", "\\223", "\\224", "\\225", "\\226", "\\227",
  "\\230", "\\231", "\\232", "\\233", "\\234", "\\235", "\\236", "\\237",
  "\\240", "\\241", "\\242", "\\243", "\\244", "\\245", "\\246", "\\247",
  "\\250", "\\251", "\\252", "\\253", "\\254", "\\255", "\\256", "\\257",
  "\\260", "\\261", "\\262", "\\263", "\\264", "\\265", "\\266", "\\267",
  "\\270", "\\271", "\\272", "\\273", "\\274", "\\275", "\\276", "\\277",
  "\\300", "\\301", "\\302", "\\303", "\\304", "\\305", "\\306", "\\307",
  "\\310", "\\311", "\\312", "\\313", "\\314", "\\315", "\\316", "\\317",
  "\\320", "\\321", "\\322", "\\323", "\\324", "\\325", "\\326", "\\327",
  "\\330", "\\331", "\\332", "\\333", "\\334", "\\335", "\\336", "\\337",
  "\\340", "\\341", "\\342", "\\343", "\\344", "\\345", "\\346", "\\347",
  "\\350", "\\351", "\\352", "\\353", "\\354", "\\355", "\\356", "\\357",
  "\\360", "\\361", "\\362", "\\363", "\\364", "\\365", "\\366", "\\367",
  "\\370", "\\371", "\\372", "\\373", "\\374", "\\375", "\\376", "\\377",
};



/******************************************************************/
/*								  */
/* Manipulation de la casse des caract�res, pour l'impl�mentation */
/* des actions pr�d�finies de l'analyseur lexicographique.	  */
/*								  */
/* La d�finition est faite � partir de la table de l'ISO-8859-1,  */
/* � charge � l'utilisateur de modifier les tableaux si cela ne   */
/* lui convient pas.						  */
/*								  */
/******************************************************************/

/* SXL : pour passer en minuscules CHAR+128 */

/** ce tableau convertit les caracteres
 *     ���������������� ������� ��������
 *  en
 *     ���������������� ������� ��������
 *  et
 *     ABCDEFGHIJKLMNOPQRSTUVWXYZ
 *  en
       abcdefghijklmnopqrstuvwxyz
 * (tous les autres caracteres restant inchanges)
 **/

char SXL [] = "\
\200\201\202\203\204\205\206\207\210\211\212\213\214\215\216\217\
\220\221\222\223\224\225\226\227\230\231\232\233\234\235\236\237\
\240\241\242\243\244\245\246\247\250\251\252\253\254\255\256\257\
\260\261\262\263\264\265\266\267\270\271\272\273\274\275\276\277\
\340\341\342\343\344\345\346\347\350\351\352\353\354\355\356\357\
\360\361\362\363\364\365\366\327\370\371\372\373\374\375\376\337\
\340\341\342\343\344\345\346\347\350\351\352\353\354\355\356\357\
\360\361\362\363\364\365\366\367\370\371\372\373\374\375\376\377\
\000\001\002\003\004\005\006\007\b\t\n\013\f\r\016\017\
\020\021\022\023\024\025\026\027\030\031\032\033\034\035\036\037\
 !\"#$%&'()*+,-./0123456789:;<=>?\
@abcdefghijklmnopqrstuvwxyz[\\]^_\
`abcdefghijklmnopqrstuvwxyz{|}~\177\
\200\201\202\203\204\205\206\207\210\211\212\213\214\215\216\217\
\220\221\222\223\224\225\226\227\230\231\232\233\234\235\236\237\
\240\241\242\243\244\245\246\247\250\251\252\253\254\255\256\257\
\260\261\262\263\264\265\266\267\270\271\272\273\274\275\276\277\
\340\341\342\343\344\345\346\347\350\351\352\353\354\355\356\357\
\360\361\362\363\364\365\366\327\370\371\372\373\374\375\376\337\
\340\341\342\343\344\345\346\347\350\351\352\353\354\355\356\357\
\360\361\362\363\364\365\366\367\370\371\372\373\374\375\376\377";


/* SXU : pour passer en majuscules CHAR+128 */

/** ce tableau convertit les caracteres
 *     ���������������� ������� ��������
 *  en
 *     ���������������� ������� ��������
 *  et
 *     abcdefghijklmnopqrstuvwxyz
 *  en
 *     ABCDEFGHIJKLMNOPQRSTUVWXYZ
 * (tous les autres caracteres restant inchanges�)
 **/

char SXU [] = "\
\200\201\202\203\204\205\206\207\210\211\212\213\214\215\216\217\
\220\221\222\223\224\225\226\227\230\231\232\233\234\235\236\237\
\240\241\242\243\244\245\246\247\250\251\252\253\254\255\256\257\
\260\261\262\263\264\265\266\267\270\271\272\273\274\275\276\277\
\300\301\302\303\304\305\306\307\310\311\312\313\314\315\316\317\
\320\321\322\323\324\325\326\327\330\331\332\333\334\335\336\337\
\300\301\302\303\304\305\306\307\310\311\312\313\314\315\316\317\
\320\321\322\323\324\325\326\367\330\331\332\333\334\335\336\377\
\000\001\002\003\004\005\006\007\b\t\n\013\f\r\016\017\
\020\021\022\023\024\025\026\027\030\031\032\033\034\035\036\037\
 !\"#$%&'()*+,-./0123456789:;<=>?\
@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_\
`ABCDEFGHIJKLMNOPQRSTUVWXYZ{|}~\177\
\200\201\202\203\204\205\206\207\210\211\212\213\214\215\216\217\
\220\221\222\223\224\225\226\227\230\231\232\233\234\235\236\237\
\240\241\242\243\244\245\246\247\250\251\252\253\254\255\256\257\
\260\261\262\263\264\265\266\267\270\271\272\273\274\275\276\277\
\300\301\302\303\304\305\306\307\310\311\312\313\314\315\316\317\
\320\321\322\323\324\325\326\327\330\331\332\333\334\335\336\337\
\300\301\302\303\304\305\306\307\310\311\312\313\314\315\316\317\
\320\321\322\323\324\325\326\367\330\331\332\333\334\335\336\377";
	  

/* SXD : pour supprimer les diacritics CHAR+128 */

/** ce tableau convertit les caracteres
 *     ���������������� ������� ��������
 *  en
 *     AAAAAAACEEEEIIII DNOOOOO OUUUUY��
 *  et
 *     ���������������� ������� ��������
 *  en
 *     aaaaaaaceeeeiiii dnooooo ouuuuy�y
 * (tous les autres caracteres restant inchanges)
 **/

char SXD [] = "\
\200\201\202\203\204\205\206\207\210\211\212\213\214\215\216\217\
\220\221\222\223\224\225\226\227\230\231\232\233\234\235\236\237\
\240\241\242\243\244\245\246\247\250\251\252\253\254\255\256\257\
\260\261\262\263\264\265\266\267\270\271\272\273\274\275\276\277\
AAAAAAACEEEEIIII\
DNOOOOO\327OUUUUY\336\337\
aaaaaaaceeeeiiii\
dnooooo\367ouuuuy\376y\
\000\001\002\003\004\005\006\007\b\t\n\013\f\r\016\017\
\020\021\022\023\024\025\026\027\030\031\032\033\034\035\036\037\
 !\"#$%&'()*+,-./0123456789:;<=>?\
@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_\
`abcdefghijklmnopqrstuvwxyz{|}~\177\
\200\201\202\203\204\205\206\207\210\211\212\213\214\215\216\217\
\220\221\222\223\224\225\226\227\230\231\232\233\234\235\236\237\
\240\241\242\243\244\245\246\247\250\251\252\253\254\255\256\257\
\260\261\262\263\264\265\266\267\270\271\272\273\274\275\276\277\
AAAAAAACEEEEIIII\
DNOOOOO\327OUUUUY\336\337\
aaaaaaaceeeeiiii\
dnooooo\367ouuuuy\376y";

/* Les chaines ci-dessus sont issues de celle-ci, qui est cens�e �tre la chaine identit� :
\200\201\202\203\204\205\206\207\210\211\212\213\214\215\216\217
\220\221\222\223\224\225\226\227\230\231\232\233\234\235\236\237\
\240���������������\
����������������\
����������������\
����������������\
����������������\
���������������\377\
\000\001\002\003\004\005\006\007\b\t\n\013\f\r\016\017\
\020\021\022\023\024\025\026\027\030\031\032\033\034\035\036\037\
 !\"#$%&'()*+,-./0123456789:;<=>?\
@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_\
`abcdefghijklmnopqrstuvwxyz{|}~\177\
\200\201\202\203\204\205\206\207\210\211\212\213\214\215\216\217\
\220\221\222\223\224\225\226\227\230\231\232\233\234\235\236\237\
\240���������������\
����������������\
����������������\
����������������\
����������������\
���������������\377";
*/




/******************************************************************/
/*								  */
/* Ces tableaux sont pr�vus pour �tre indic�s � partir de leur	  */
/* �l�ment n�128 consid�r� comme repr�sentant le caract�re \0.	  */
/* Les indices effectifs peuvent donc �tre des caract�res sign�s  */
/* ou non sign�s, entre -128 et +255.				  */
/*								  */
/* A chaque caractere de l'ISOlatin1 on fait correspondre une     */
/* chaine de caracteres qui peut etre un (composant d'un) ident C */
/* A toute chaine il est donc possible de lui faire correspondre  */
/* un ident C unique.                                             */
/******************************************************************/



char	*sxcISOlatin12Cidstr [384] =
{
/* &#128;- &#159;      Unused */ "_128_", "_129_",
/*  */ "_130_", "_131_", "_132_", "_133_", "_134_", "_135_", "_136_", "_137_", "_138_", "_139_", 
/*  */ "_140_", "_141_", "_142_", "_143_", "_144_", "_145_", "_146_", "_147_", "_148_", "_149_",
/*  */ "_150_", "_151_", "_152_", "_153_", "_154_", "_155_", "_156_", "_157_", "_158_", "_159_",
/* &160;                */ "_ESPACE_INSECABLE_",
/* &161;               Inverted exclamation */ "_POINT_D_EXCLAMATION_RENVERSE_",
/* &162;               Cent sign */ "_CENTIME_",
/* &163;               Pound sterling */ "_LIVRE_",
/* &164;               General currency sign */ "_SYMBOLE_MONETAIRE_",
/* &165;               Yen sign */ "_YEN_",
/* &166;               Broken vertical bar */ "_BARRE_VERTICALE_DISCONTINUE_",
/* &167;               Section sign */ "_PARAGRAPHE_",
/* &168;               Umlaut (dieresis) */ "_TREMA_",
/* &169;               Copyright */ "_COPYRIGHT_",
/* &170;               Feminine ordinal */ "_ORDINAL_FEMININ_",
/* &171;               Left angle quote, guillemotleft */ "_CHEVRON_OUVRANT_",
/* &172;               Not sign */ "_NEGATION_",
/* &173;               Soft hyphen */ "_TRAIT_D_UNION_CONDITIONNEL_",
/* &174;               Registered trademark */ "_MARQUE_DEPOSEE_",
/* &175;               Macron accent */ "_MACRON_",
/* &176;               Degree sign */ "_DEGRE_",
/* &177;               Plus or minus */ "_PLUS_OU_MOINS_",
/* &178;               Superscript two */ "_EXPOSANT_DEUX_",
/* &179;               Superscript three */ "_EXPOSANT_TROIS_",
/* &180;               Acute accent */ "_ACCENT_AIGU_",
/* &181;               Micro sign */ "_MICRO_",
/* &182;               Paragraph sign */ "_FIN_DE_PARAGRAPHE_",
/* &183;               Middle dot */ "_POINT_MEDIAN_",
/* &184;               Cedilla */ "_CEDILLE_",
/* &185;               Superscript one */ "_EXPOSANT_UN_",
/* &186;               Masculine ordinal */ "_ORDINAL_MASCULIN_",
/* &187;               Right angle quote, guillemotright */ "_CHEVRON_FERMANT_",
/* &188;               Fraction one-fourth */ "_UN_QUART",
/* &189;               Fraction one-half */ "_UN_DEMI_",
/* &190;               Fraction three-fourths */ "_TROIS_QUART_",
/* &191;               Inverted question mark */ "_POINT_D_INTERROGATION_RENVERSE_",
/*  */
/* &192;               Capital A, acute accent */ "_A_AIGU_",
/* &193;               Capital A, grave accent */ "_A_GRAVE_",
/* &194;               Capital A, circumflex accent */ "_A_CIRCONFLEXE_",
/* &195;               Capital A, tilde */ "_A_TILDE_",
/* &196;               Capital A, ring */ "_A_TREMA_",
/* &197;               Capital A, dieresis or umlaut mark */ "_A_ROND_",
/* &198;               Capital AE dipthong (ligature) */ "_AE_",
/* &199;               Capital C, cedilla */ "_C_CEDILLE_",
/* &200;               Capital E, acute accent */ "_E_AIGU_",
/* &201;               Capital E, grave accent */ "_E_GRAVE_",
/* &202;               Capital E, circumflex accent */ "_E_CIRCONFLEXE_",
/* &203;               Capital E, dieresis or umlaut mark */ "_E_ROND_",
/* &204;               Capital I, acute accent */ "_I_AIGU_",
/* &205;               Capital I, grave accent */ "_I_GRAVE_",
/* &206;               Capital I, circumflex accent */ "_I_CIRCONFLEXE_",
/* &207;               Capital I, dieresis or umlaut mark */ "_I_ROND_",
/* &208;               Capital Eth, Icelandic */ "_ETH_",
/* &209;               Capital N, tilde */ "_N_TILDE_",
/* &210;               Capital O, acute accent */ "_O_AIGU_",
/* &211;               Capital O, grave accent */ "_O_GRAVE_",
/* &212;               Capital O, circumflex accent */ "_O_CIRCONFLEXE_",
/* &213;               Capital O, tilde */  "_A_TILDE_",
/* &214;               Capital O, dieresis or umlaut mark */ "_O_TREMA_",
/*  */
/* &215;               Multiply sign */ "_MULTIPLICATION_",
/*  */
/* &216;               Capital O, slash */ "_O_BARRE_OBLIQUEMENT_",
/* &217;               Capital U, acute accent */ "_U_AIGU_",
/* &218;               Capital U, grave accent */ "_U_GRAVE_",
/* &219;               Capital U, circumflex accent */ "_U_CIRCONFLEXE_",
/* &220;               Capital U, dieresis or umlaut mark */ "_U_ROND_",
/* &221;               Capital Y, acute accent */ "_Y_AIGU_",
/*  */
/* &222;               Capital THORN, Icelandic */ "_THORN_",
/* &223;               Small sharp s, German (sz ligature) */ "_sz_",
/*  */
/* &224;               Small a, acute accent */ "_a_AIGU_",
/* &225;               Small a, grave accent */ "_a_GRAVE_",
/* &226;               Small a, circumflex accent */ "_a_CIRCONFLEXE_",
/* &227;               Small a, tilde */ "_a_TILDE_",
/* &228;               Small a, ring */ "_a_TREMA_",
/* &229;               Small a, dieresis or umlaut mark */ "_a_ROND_",
/* &230;               Small ae dipthong (ligature) */ "_ae_",
/* &231;               Small c, cedilla */ "_c_CEDILLE_",
/* &232;               Small e, acute accent */ "_e_AIGU_",
/* &233;               Small e, grave accent */ "_e_GRAVE_",
/* &234;               Small e, circumflex accent */ "_e_CIRCONFLEXE_",
/* &235;               Small e, dieresis or umlaut mark */ "_e_ROND_",
/* &236;               Small i, acute accent */ "_i_AIGU_",
/* &237;               Small i, grave accent */ "_i_GRAVE_",
/* &238;               Small i, circumflex accent */ "_i_CIRCONFLEXE_",
/* &239;               Small i, dieresis or umlaut mark */ "_i_ROND_",
/* &240;               Small eth, Icelandic */ "_eth_",
/* &241;               Small n, tilde */ "_n_TILDE_",
/* &242;               Small o, acute accent */ "_o_AIGU_",
/* &243;               Small o, grave accent */ "_o_GRAVE_",
/* &244;               Small o, circumflex accent */ "_o_CIRCONFLEXE_",
/* &245;               Small o, tilde */ "_o_TILDE_",
/* &246;               Small o, dieresis or umlaut mark */ "_o_ROND_",
/*  */
/* &247;               Division sign */ "_DIVISION_",
/*  */
/* &248;               Small o, slash */ "_o_O_BARRE_OBLIQUEMENT_",
/* &249;               Small u, acute accent */ "_u_AIGU_",
/* &250;               Small u, grave accent */ "_u_GRAVE_",
/* &251;               Small u, circumflex accent */ "_u_CIRCONFLEXE_",
/* &252;               Small u, dieresis or umlaut mark */ "_u_ROND_",
/* &253;               Small y, acute accent */ "_y_AIGU_",
/* &254;               Small thorn, Icelandic */ "_thorn_",
/* &255;               Small y, dieresis or umlaut mark */ "_y_ROND_",
/*  */

/* &#00;-&#08;         Unused */ "_000_", "_001_", "_002_", "_003_", "_004_", "_005_", "_006_", "_007_", "_008_",
/* &#09;               Horizontal tab */ "_HT_",
/* &#10;               Line feed */ "_LF_",
/* &#11;-&#31;         Unused */ "_011_", "_012_", "_013_", "_014_", "_015_", "_016_", "_017_", "_018_", "_019_", "_020_",
/*  */ "_021_", "_022_", "_023_", "_024_", "_025_", "_026_", "_027_", "_028_", "_029_", "_030_",
/*  */ "_031_",
/* &32;                Space */ "_ESPACE_",
/* &33;                Exclamation mark */ "_POINT_D_EXCLAMATION_",
/* &34;                Double quote */ "_GUILLEMET_ANGLAIS_",
/* &35;                Number sign */ "_CROISILLON_",
/* &36;                Dollar sign */ "_SYMBOLE_DOLLAR_",
/* &37;                Percent sign */ "_SYMBOLE_POUR_CENT_",
/* &38;                Ampersand */ "_PERLUETTE_", 
/* &39;                Apostrophe */ "_APOSTROPHE_",
/* &40;                Left parenthesis */ "_PARENTHESE_GAUCHE_",
/* &41;                Right parenthesis */"_PARENTHESE_DROITE_",
/* &42;                Asterisk */ "_ASTERISQUE_",
/* &43;                Plus sign */ "_SIGNE_PLUS_",
/* &44;                Comma */ "_VIRGULE_",
/* &45;                Hyphen */ "_TRAIT_D_UNION_SIGNE_MOINS_",
/* &46;                Period (fullstop) */ "_POINT_",
/* &47;                Solidus (slash) */ "_BARRE_OBLIQUE_",
/*  */
/* &#48; - &#57;       Digits 0-9 */  "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
/*  */
/* &58;                Colon */ "_DEUX_POINTS_",
/* &59;                Semi-colon */ "_POINT_VIRGULE_",
/* &60;                Less than */ "_SIGNE_INFERIEUR_A_",
/* &61;                Equals aign */ "_SIGNE_EGAL_",
/* &62;                Greater than */ "_SIGNE_SUPERIEUR_A_",
/* &63;                Question mark */ "_POINT_D_INTERROGATION_",
/* &64;                Commercial at */ "_ARROBE_",
/*  */
/* &#65;-&#90;         Letters A-Z */ "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L",
/*  */ "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
/* &91;                Left square bracket */ "_CROCHET_GAUCHE_",
/* &92;                Reverse solidus (backslash) */ "_BARRE_OBLIQUE_INVERSEE_",
/* &93;                Right square bracket */ "_CROCHET_DROIT_",
/* &94;                */ "_ACCENT_CICONFLEXE_",
/* &95;                Horizontal bar */ "_TIRET_BAS_",
/* &96;                Acute accent */ "_ACCENT_GRAVE_",
/*  */
/* &#97;-&#122;        Letters a-z */ "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l",
/*  */"m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z",
/* &123;               Left curly brace */ "_ACCOLADE_GAUCHE_",
/* &124;               Vertical bar */ "_BARRE_VERTICALE_",
/* &125;               Right curly brace */ "_ACCOLADE_DROITE_",
/* &126;               Tilde */ "_TILDE_",
/*  */
/* &#127;- &#159;      Unused */ "_127_", "_128_", "_129_",
/*  */ "_130_", "_131_", "_132_", "_133_", "_134_", "_135_", "_136_", "_137_", "_138_", "_139_", 
/*  */ "_140_", "_141_", "_142_", "_143_", "_144_", "_145_", "_146_", "_147_", "_148_", "_149_",
/*  */ "_150_", "_151_", "_152_", "_153_", "_154_", "_155_", "_156_", "_157_", "_158_", "_159_",
/* &160;                */ "_ESPACE_INSECABLE_",
/* &161;               Inverted exclamation */ "_POINT_D_EXCLAMATION_RENVERSE_",
/* &162;               Cent sign */ "_SYMBOLE_CENTIME_",
/* &163;               Pound sterling */ "_SYMBOLE_LIVRE_",
/* &164;               General currency sign */ "_SYMBOLE_MONETAIRE_",
/* &165;               Yen sign */ "_SYMBOLE_YEN_",
/* &166;               Broken vertical bar */ "_BARRE_VERTICALE_DISCONTINUE_",
/* &167;               Section sign */ "_PARAGRAPHE_",
/* &168;               Umlaut (dieresis) */ "_TREMA_",
/* &169;               Copyright */ "_SYMBOLE_COPYRIGHT_",
/* &170;               Feminine ordinal */ "_INDICATEUR_ORDINAL_FEMININ_",
/* &171;               Left angle quote, guillemotleft */ "_GUILLEMET_GAUCHE_",
/* &172;               Not sign */ "_SIGNE_NEGATION_",
/* &173;               Soft hyphen */ "_TRAIT_D_UNION_CONDITIONNEL_",
/* &174;               Registered trademark */ "_SYMBOLE_MARQUE_DEPOSEE_",
/* &175;               Macron accent */ "_MACRON_",
/* &176;               Degree sign */ "_SYMBOLE_DEGRE_",
/* &177;               Plus or minus */ "_SYMBOLE_PLUS_OU_MOINS_",
/* &178;               Superscript two */ "_EXPOSANT_DEUX_",
/* &179;               Superscript three */ "_EXPOSANT_TROIS_",
/* &180;               Acute accent */ "_ACCENT_AIGU_",
/* &181;               Micro sign */ "_SYMBOLE_MICRO_",
/* &182;               Paragraph sign */ "_PIED_DE_MOUCHE_",
/* &183;               Middle dot */ "_POINT_MEDIAN_",
/* &184;               Cedilla */ "_CEDILLE_",
/* &185;               Superscript one */ "_EXPOSANT_UN_",
/* &186;               Masculine ordinal */ "_INDICATEUR_ORDINAL_MASCULIN_",
/* &187;               Right angle quote, guillemotright */ "_GUILLEMET_DROIT_",
/* &188;               Fraction one-fourth */ "_FRACTION_UN_QUART_",
/* &189;               Fraction one-half */ "_FRACTION_UN_DEMI_",
/* &190;               Fraction three-fourths */ "_FRACTION_TROIS_QUART_",
/* &191;               Inverted question mark */ "_POINT_D_INTERROGATION_RENVERSE_",
/*  */
/* &192;               Capital A, grave accent */ "_A_GRAVE_",
/* &193;               Capital A, acute accent */ "_A_AIGU_",
/* &194;               Capital A, circumflex accent */ "_A_CIRCONFLEXE_",
/* &195;               Capital A, tilde */ "_A_TILDE_",
/* &196;               Capital A, ring */ "_A_TREMA_",
/* &197;               Capital A, dieresis or umlaut mark */ "_A_ROND_",
/* &198;               Capital AE dipthong (ligature) */ "_AE_",
/* &199;               Capital C, cedilla */ "_C_CEDILLE_",
/* &200;               Capital E, grave accent */ "_E_GRAVE_",
/* &201;               Capital E, acute accent */ "_E_AIGU_",
/* &202;               Capital E, circumflex accent */ "_E_CIRCONFLEXE_",
/* &203;               Capital E, dieresis or umlaut mark */ "_E_ROND_",
/* &204;               Capital I, grave accent */ "_I_GRAVE_",
/* &205;               Capital I, acute accent */ "_I_AIGU_",
/* &206;               Capital I, circumflex accent */ "_I_CIRCONFLEXE_",
/* &207;               Capital I, dieresis or umlaut mark */ "_I_ROND_",
/* &208;               Capital Eth, Icelandic */ "_ETH_",
/* &209;               Capital N, tilde */ "_N_TILDE_",
/* &210;               Capital O, grave accent */ "_O_GRAVE_",
/* &211;               Capital O, acute accent */ "_O_AIGU_",
/* &212;               Capital O, circumflex accent */ "_O_CIRCONFLEXE_",
/* &213;               Capital O, tilde */  "_A_TILDE_",
/* &214;               Capital O, dieresis or umlaut mark */ "_O_TREMA_",
/*  */
/* &215;               Multiply sign */ "_MULTIPLICATION_",
/*  */
/* &216;               Capital O, slash */ "_O_BARRE_OBLIQUEMENT_",
/* &217;               Capital U, grave accent */ "_U_GRAVE_",
/* &218;               Capital U, acute accent */ "_U_AIGU_",
/* &219;               Capital U, circumflex accent */ "_U_CIRCONFLEXE_",
/* &220;               Capital U, dieresis or umlaut mark */ "_U_ROND_",
/* &221;               Capital Y, acute accent */ "_Y_AIGU_",
/*  */
/* &222;               Capital THORN, Icelandic */ "_THORN_",
/* &223;               Small sharp s, German (sz ligature) */ "_sz_",
/*  */
/* &224;               Small a, grave accent */ "_a_GRAVE_",
/* &225;               Small a, acute accent */ "_a_AIGU_",
/* &226;               Small a, circumflex accent */ "_a_CIRCONFLEXE_",
/* &227;               Small a, tilde */ "_a_TILDE_",
/* &228;               Small a, ring */ "_a_TREMA_",
/* &229;               Small a, dieresis or umlaut mark */ "_a_ROND_",
/* &230;               Small ae dipthong (ligature) */ "_ae_",
/* &231;               Small c, cedilla */ "_c_CEDILLE_",
/* &232;               Small e, grave accent */ "_e_GRAVE_",
/* &233;               Small e, acute accent */ "_e_AIGU_",
/* &234;               Small e, circumflex accent */ "_e_CIRCONFLEXE_",
/* &235;               Small e, dieresis or umlaut mark */ "_e_ROND_",
/* &236;               Small i, grave accent */ "_i_GRAVE_",
/* &237;               Small i, acute accent */ "_i_AIGU_",
/* &238;               Small i, circumflex accent */ "_i_CIRCONFLEXE_",
/* &239;               Small i, dieresis or umlaut mark */ "_i_ROND_",
/* &240;               Small eth, Icelandic */ "_eth_",
/* &241;               Small n, tilde */ "_n_TILDE_",
/* &242;               Small o, grave accent */ "_o_GRAVE_",
/* &243;               Small o, acute accent */ "_o_AIGU_",
/* &244;               Small o, circumflex accent */ "_o_CIRCONFLEXE_",
/* &245;               Small o, tilde */ "_o_TILDE_",
/* &246;               Small o, dieresis or umlaut mark */ "_o_ROND_",
/*  */
/* &247;               Division sign */ "_DIVISION_",
/*  */
/* &248;               Small o, slash */ "_o_BARRE_OBLIQUEMENT_",
/* &249;               Small u, grave accent */ "_u_GRAVE_",
/* &250;               Small u, acute accent */ "_u_AIGU_",
/* &251;               Small u, circumflex accent */ "_u_CIRCONFLEXE_",
/* &252;               Small u, dieresis or umlaut mark */ "_u_ROND_",
/* &253;               Small y, acute accent */ "_y_AIGU_",
/* &254;               Small thorn, Icelandic */ "_thorn_",
/* &255;               Small y, dieresis or umlaut mark */ "_y_ROND_",
/*  */
};
