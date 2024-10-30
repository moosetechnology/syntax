char WHAT[] = "@(#)Linear Derivation Grammar Generator for the LIG \"nnf\" - SYNTAX [unix] - Thu Dec 19 15:34:26 1996";

static char	ME [] = "nnf_ligt.c";

#include "sxunix.h"

#define SXLIGis_normal_form	0
#define SXLIGis_post_act	0
#define SXLIGis_post_prdct	0
#define SXLIGis_leveln_complete	1
#define SXLIGis_reduced	1
#define is_initial_LIG	0
#define SXLIGmaxssymb	5
#define SXLIGmaxrhsnt	3
#define RDGis_left_part_empty	0


static int SXLIGap_disp [] = {
/* 0 */	1,
/* 1 */	1,
/* 2 */	3,
/* 3 */	5,
/* 4 */	9,
/* 5 */	13,
/* 6 */	16,
/* 7 */	20,
/* 8 */	24,
};

static int SXLIGap_list [] = {
/* 0 */	0,
/* 1 */	0, 0, 
/* 2 */	1, 0, 
/* 3 */	1, 2, 1, 2, 
/* 4 */	1, 2, 2, 1, 
/* 5 */	0, 1, 3, 
/* 6 */	1, 2, 4, 5, 
/* 7 */	0, 2, 5, 4, 
};

static int SXLIGaction [] = {
/* 0 */	-1,
/* 1 */	2,
/* 2 */	4,
/* 3 */	5,
/* 4 */	7,
/* 5 */	-1,
};

static int SXLIGprod2left_secnb [] = {
/* 0 */	0,
/* 1 */	0,
/* 2 */	1,
/* 3 */	0,
/* 4 */	0,
/* 5 */	1,
};

static int SXLIGprod2dum_nb [] = {
/* 0 */	0,
/* 1 */	1,
/* 2 */	4,
/* 3 */	0,
/* 4 */	1,
/* 5 */	0,
};

#ifdef is_sxndparser
/* Allows the access to SXLIGprdct when the parser is sxndparser */

static int SXLIGlhs [] = {
/* 0 */	0,
/* 1 */	1,
/* 2 */	2,
/* 3 */	3,
/* 4 */	4,
/* 5 */	5,
};
static int SXLIGprolon [] = {
/* 0 */	0,
/* 1 */	4,
/* 2 */	6,
/* 3 */	10,
/* 4 */	12,
/* 5 */	14,
/* 6 */	16,
};

static int SXLIGprod2nbnt [] = {
/* 0 */	1,
/* 1 */	1,
/* 2 */	3,
/* 3 */	0,
/* 4 */	0,
/* 5 */	1,
};

static char *SXLIGntstring [] = {
/* 0 */	"",
/* 1 */	"S", 
/* 2 */	"A", 
/* 3 */	"X", 
/* 4 */	"B", 
/* 5 */	"Y", 
};
#endif


static int SXLIGprdct [] = {
/* 0 S' -> $ S $ 0 */	-1, -1, -1, -1,
/* 1 */	3, -1, 
/* 2 */	5, 6, -1, -1, 
/* 3 */	-1, -1, 
/* 4 */	-1, -1, 
/* 5 */	5, -1, 
};

static char *SXLIGssymbstring [] = {
/* 0 */	"",
/* 1 */	"a", 
/* 2 */	"b", 
/* 3 */	"c", 
/* 4 */	"d", 
/* 5 */	"e", 
};

#include "SXLIGparsact.h"
