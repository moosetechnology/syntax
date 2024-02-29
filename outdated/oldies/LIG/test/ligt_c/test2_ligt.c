char WHAT[] = "@(#)Linear Derivation Grammar Generator for the LIG \"test2\" - SYNTAX [unix] - Tue Nov 19 13:43:26 1996";

static char	ME [] = \"test2_ligt.c\";

#include "sxunix.h"

#define SXLIGis_normal_form	1
#define SXLIGis_post_act	0
#define SXLIGis_post_prdct	0
#define SXLIGis_leveln_complete	1
#define SXLIGis_reduced	0
#define is_initial_LIG	0
#define SXLIGmaxssymb	2


static int SXLIGap_disp [] = {
/* 0 */	1,
/* 1 */	1,
/* 2 */	3,
/* 3 */	5,
/* 4 */	8,
/* 5 */	11,
};

static int SXLIGap_list [] = {
/* 0 */	0,
/* 1 */	0, 0, 
/* 2 */	1, 0, 
/* 3 */	1, 1, 1, 
/* 4 */	1, 1, 2, 
};

static int SXLIGaction [] = {
/* 0 */	-1,
/* 1 */	2,
/* 2 */	2,
};

static int SXLIGprdct [] = {
/* 0 S' -> $ S $ 0 */	-1, -1, -1, -1,
/* 1 */	-1, 3, -1, 
/* 2 */	-1, 4, -1, 
};

static char *SXLIGssymbstring [] = {
/* 0 */	"",
/* 1 */	"a", 
/* 2 */	"b", 
};
static  SXBA_ELT SXLIGt_set
#if SXBITS_PER_LONG==32
 [2] = {3, 0X00000000, }
#else
 [2] = {3, 0X0000000000000000, }
#endif
/* End SXLIGt_set */;


#include "SXLIGparsact.h"
