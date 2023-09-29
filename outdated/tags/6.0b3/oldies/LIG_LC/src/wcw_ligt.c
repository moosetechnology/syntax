#define WHAT	"@(#)Linear Derivation Grammar Generator for the LIG \"wcw\" - SYNTAX [unix] - Thu Dec 19 15:32:51 1996"
static struct what {
  struct what	*whatp;
  char		what [sizeof (WHAT)];
} what = {&what, WHAT};

static char	ME [] = "wcw_ligt.c";

#include "sxunix.h"

#define SXLIGis_normal_form	1
#define SXLIGis_post_act	0
#define SXLIGis_post_prdct	0
#define SXLIGis_leveln_complete	1
#define SXLIGis_reduced	1
#define is_initial_LIG	0
#define SXLIGmaxssymb	3
#define SXLIGmaxrhsnt	1
#define RDGis_left_part_empty	1


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

#ifdef is_sxndparser
/* Allows the access to SXLIGprdct when the parser is sxndparser */

static int SXLIGlhs [] = {
/* 0 */	0,
/* 1 */	1,
/* 2 */	1,
/* 3 */	1,
/* 4 */	1,
/* 5 */	2,
/* 6 */	2,
/* 7 */	2,
/* 8 */	2,
};
static int SXLIGprolon [] = {
/* 0 */	0,
/* 1 */	4,
/* 2 */	7,
/* 3 */	10,
/* 4 */	13,
/* 5 */	15,
/* 6 */	18,
/* 7 */	21,
/* 8 */	24,
/* 9 */	26,
};

static int SXLIGprod2nbnt [] = {
/* 0 */	1,
/* 1 */	1,
/* 2 */	1,
/* 3 */	1,
/* 4 */	1,
/* 5 */	1,
/* 6 */	1,
/* 7 */	1,
/* 8 */	0,
};

static char *SXLIGntstring [] = {
/* 0 */	"",
/* 1 */	"S", 
/* 2 */	"T", 
};
#endif


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
