char WHAT[] = "@(#)Linear Derivation Grammar Generator for the LIG \"cyc2\" - SYNTAX [unix] - Thu Dec 19 15:35:16 1996";

static char	ME [] = "cyc2_ligt.c";

#include "sxunix.h"

#define SXLIGis_normal_form	0
#define SXLIGis_post_act	0
#define SXLIGis_post_prdct	0
#define SXLIGis_leveln_complete	1
#define SXLIGis_reduced	1
#define is_initial_LIG	0
#define SXLIGmaxssymb	1
#define SXLIGmaxrhsnt	1
#define RDGis_left_part_empty	1


static int SXLIGap_disp [] = {
/* 0 */	1,
/* 1 */	1,
/* 2 */	3,
/* 3 */	5,
/* 4 */	9,
/* 5 */	12,
};

static int SXLIGap_list [] = {
/* 0 */	0,
/* 1 */	0, 0, 
/* 2 */	1, 0, 
/* 3 */	1, 2, 1, 1, 
/* 4 */	1, 1, 1, 
};

static int SXLIGaction [] = {
/* 0 */	-1,
/* 1 */	2,
/* 2 */	2,
/* 3 */	4,
/* 4 */	1,
};

static int SXLIGprod2dum_nb [] = {
/* 0 */	0,
/* 1 */	1,
/* 2 */	0,
/* 3 */	0,
/* 4 */	0,
};

#ifdef is_sxndparser
/* Allows the access to SXLIGprdct when the parser is sxndparser */

static int SXLIGlhs [] = {
/* 0 */	0,
/* 1 */	1,
/* 2 */	1,
/* 3 */	2,
/* 4 */	2,
};
static int SXLIGprolon [] = {
/* 0 */	0,
/* 1 */	4,
/* 2 */	6,
/* 3 */	8,
/* 4 */	10,
/* 5 */	12,
};

static int SXLIGprod2nbnt [] = {
/* 0 */	1,
/* 1 */	1,
/* 2 */	1,
/* 3 */	1,
/* 4 */	0,
};

static char *SXLIGntstring [] = {
/* 0 */	"",
/* 1 */	"A", 
/* 2 */	"B", 
};
#endif


static int SXLIGprdct [] = {
/* 0 S' -> $ S $ 0 */	-1, -1, -1, -1,
/* 1 */	3, -1, 
/* 2 */	2, -1, 
/* 3 */	2, -1, 
/* 4 */	-1, -1, 
};

static char *SXLIGssymbstring [] = {
/* 0 */	"",
/* 1 */	"a", 
};

#include "SXLIGparsact.h"
