char WHAT[] = "@(#)Linear Derivation Grammar Generator for the LIG \"ndwcw3\" - SYNTAX [unix] - Tue Nov 19 13:50:11 1996";

static char	ME [] = \"ndwcw3_ligt.c\";

#include "sxunix.h"

#define SXLIGis_normal_form	1
#define SXLIGis_post_act	0
#define SXLIGis_post_prdct	0
#define SXLIGis_leveln_complete	1
#define SXLIGis_reduced	1
#define is_initial_LIG	0
#define SXLIGmaxssymb	3


static int SXLIGap_disp [] = {
/* 0 */	1,
/* 1 */	1,
/* 2 */	3,
/* 3 */	5,
/* 4 */	8,
/* 5 */	11,
/* 6 */	14,
};

static int SXLIGap_list [] = {
/* 0 */	0,
/* 1 */	0, 0, 
/* 2 */	1, 0, 
/* 3 */	1, 1, 1, 
/* 4 */	1, 1, 2, 
/* 5 */	1, 1, 3, 
};

static int SXLIGaction [] = {
/* 0 */	-1,
/* 1 */	2,
/* 2 */	2,
/* 3 */	2,
/* 4 */	2,
/* 5 */	3,
/* 6 */	4,
/* 7 */	5,
/* 8 */	1,
};

static int SXLIGprdct [] = {
/* 0 S' -> $ S $ 0 */	-1, -1, -1, -1,
/* 1 */	3, -1, -1, 
/* 2 */	4, -1, -1, 
/* 3 */	5, -1, -1, 
/* 4 */	2, -1, 
/* 5 */	-1, 2, -1, 
/* 6 */	-1, 2, -1, 
/* 7 */	-1, 2, -1, 
/* 8 */	-1, -1, 
};

static char *SXLIGssymbstring [] = {
/* 0 */	"",
/* 1 */	"a", 
/* 2 */	"b", 
/* 3 */	"c", 
};

#include "SXLIGparsact.h"
