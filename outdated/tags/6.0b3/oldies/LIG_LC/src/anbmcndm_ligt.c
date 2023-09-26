#define WHAT	"@(#)Linear Derivation Grammar Generator for the LIG \"anbmcndm\" - SYNTAX [unix] - Thu Dec 19 15:36:37 1996"
static struct what {
  struct what	*whatp;
  char		what [sizeof (WHAT)];
} what = {&what, WHAT};

static char	ME [] = "anbmcndm_ligt.c";

#include "sxunix.h"

#define SXLIGis_normal_form	1
#define SXLIGis_post_act	0
#define SXLIGis_post_prdct	0
#define SXLIGis_leveln_complete	0
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
/* 3 */	2,
/* 4 */	2,
/* 5 */	3,
/* 6 */	1,
};

#ifdef is_sxndparser
/* Allows the access to SXLIGprdct when the parser is sxndparser */

static int SXLIGlhs [] = {
/* 0 */	0,
/* 1 */	1,
/* 2 */	1,
/* 3 */	2,
/* 4 */	2,
/* 5 */	3,
/* 6 */	3,
};
static int SXLIGprolon [] = {
/* 0 */	0,
/* 1 */	4,
/* 2 */	7,
/* 3 */	9,
/* 4 */	13,
/* 5 */	15,
/* 6 */	18,
/* 7 */	19,
};

static int SXLIGprod2nbnt [] = {
/* 0 */	1,
/* 1 */	1,
/* 2 */	1,
/* 3 */	1,
/* 4 */	1,
/* 5 */	1,
/* 6 */	0,
};

static char *SXLIGntstring [] = {
/* 0 */	"",
/* 1 */	"S", 
/* 2 */	"T", 
/* 3 */	"B", 
};
#endif


static int SXLIGprdct [] = {
/* 0 S' -> $ S $ 0 */	-1, -1, -1, -1,
/* 1 */	3, -1, -1, 
/* 2 */	2, -1, 
/* 3 */	-1, 2, -1, -1, 
/* 4 */	2, -1, 
/* 5 */	-1, 2, -1, 
/* 6 */	-1, 
};

static char *SXLIGssymbstring [] = {
/* 0 */	"",
/* 1 */	"d", 
};

#if SXLIGuse_reduced==1

static  SXBA_ELT SXLIGEQUALn_set [4]
#if SXBITS_PER_LONG==32
 [2] = {
/* 0 */ {4, 0X00000000, },
/* 1 */ {4, 0X00000008, },
/* 2 */ {4, 0X00000008, },
/* 3 */ {4, 0X00000000, },
#else
 [2] = {
/* 0 */ {4, 0X0000000000000000, },
/* 1 */ {4, 0X0000000000000008, },
/* 2 */ {4, 0X0000000000000008, },
/* 3 */ {4, 0X0000000000000000, },
#endif
} /* End SXLIGEQUALn_set */;


static SXBA *SXLIGPUSHPOPn_set;


static int SXLIGAxs [4] [2] = {
/* 0 */	{0, 0, },
/* 1 */	{0, 1, },
/* 2 */	{0, 0, },
/* 3 */	{0, 2, },
};
static  SXBA_ELT SXLIGPOPn_set [4]
#if SXBITS_PER_LONG==32
 [2] = {
/* 0 */ {3, 0X00000000, },
/* 1 */ {3, 0X00000004, },
/* 2 */ {3, 0X00000000, },
/* 3 */ {3, 0X00000000, },
#else
 [2] = {
/* 0 */ {3, 0X0000000000000000, },
/* 1 */ {3, 0X0000000000000004, },
/* 2 */ {3, 0X0000000000000000, },
/* 3 */ {3, 0X0000000000000000, },
#endif
} /* End SXLIGPOPn_set */;

#endif


#include "SXLIGparsact.h"
