char WHAT[] = "@(#)Linear Derivation Grammar Generator for the LIG \"anbncn\" - SYNTAX [unix] - Tue Jan 28 16:16:39 1997";

static char	ME [] = "anbncn_ligt.c";

#include "sxunix.h"

#define SXLIGis_normal_form	1
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
/* 4 */	8,
};

static int SXLIGap_list [] = {
/* 0 */	0,
/* 1 */	0, 0, 
/* 2 */	1, 0, 
/* 3 */	1, 1, 1, 
};

static int SXLIGaction [] = {
/* 0 */	-1,
/* 1 */	2,
/* 2 */	2,
/* 3 */	3,
/* 4 */	1,
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
/* 2 */	7,
/* 3 */	9,
/* 4 */	13,
/* 5 */	14,
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
/* 1 */	"S", 
/* 2 */	"AB", 
};
#endif


static int SXLIGprdct [] = {
/* 0 S' -> $ S $ 0 */	-1, -1, -1, -1,
/* 1 */	3, -1, -1, 
/* 2 */	2, -1, 
/* 3 */	-1, 2, -1, -1, 
/* 4 */	-1, 
};

static char *SXLIGssymbstring [] = {
/* 0 */	"",
/* 1 */	"c", 
};

#include "SXLIGparsact.h"
