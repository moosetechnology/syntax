#define WHAT	"@(#)Linear Derivation Grammar Generator for the LIG \"nocyc\" - SYNTAX [unix] - Tue Dec  3 13:47:54 1996"
static struct what {
  struct what	*whatp;
  char		what [sizeof (WHAT)];
} what = {&what, WHAT};

static char	ME [] = "nocyc_ligt.c";

#include "sxunix.h"

#define SXLIGis_normal_form	1
#define SXLIGis_post_act	0
#define SXLIGis_post_prdct	0
#define SXLIGis_leveln_complete	0
#define SXLIGis_reduced	1
#define is_initial_LIG	0
#define SXLIGmaxssymb	1
#define SXLIGhas_cycles	0
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
/* 4 */	3,
/* 5 */	3,
/* 6 */	3,
/* 7 */	1,
};

static int SXLIGprdct [] = {
/* 0 S' -> $ S $ 0 */	-1, -1, -1, -1,
/* 1 */	2, -1, 
/* 2 */	3, -1, 
/* 3 */	2, -1, 
/* 4 */	2, -1, 
/* 5 */	2, -1, 
/* 6 */	2, -1, 
/* 7 */	-1, -1, 
};

static char *SXLIGssymbstring [] = {
/* 0 */	"",
/* 1 */	"a", 
};

#if SXLIGuse_reduced==1

static  SXBA_ELT SXLIGEQUALn_set [7]
#if SXBITS_PER_LONG==32
 [2] = {
/* 0 */ {7, 0X00000000, },
/* 1 */ {7, 0X00000040, },
/* 2 */ {7, 0X00000000, },
/* 3 */ {7, 0X00000000, },
/* 4 */ {7, 0X00000000, },
/* 5 */ {7, 0X00000000, },
/* 6 */ {7, 0X00000000, },
#else
 [2] = {
/* 0 */ {7, 0X0000000000000000, },
/* 1 */ {7, 0X0000000000000040, },
/* 2 */ {7, 0X0000000000000000, },
/* 3 */ {7, 0X0000000000000000, },
/* 4 */ {7, 0X0000000000000000, },
/* 5 */ {7, 0X0000000000000000, },
/* 6 */ {7, 0X0000000000000000, },
#endif
} /* End SXLIGEQUALn_set */;


static SXBA *SXLIGPUSHPOPn_set;


static int SXLIGAxs [7] [2] = {
/* 0 */	{0, 0, },
/* 1 */	{0, 0, },
/* 2 */	{0, 1, },
/* 3 */	{0, 0, },
/* 4 */	{0, 2, },
/* 5 */	{0, 3, },
/* 6 */	{0, 4, },
};
static  SXBA_ELT SXLIGPOPn_set [7]
#if SXBITS_PER_LONG==32
 [2] = {
/* 0 */ {5, 0X00000000, },
/* 1 */ {5, 0X00000000, },
/* 2 */ {5, 0X0000001C, },
/* 3 */ {5, 0X00000000, },
/* 4 */ {5, 0X00000000, },
/* 5 */ {5, 0X00000000, },
/* 6 */ {5, 0X00000000, },
#else
 [2] = {
/* 0 */ {5, 0X0000000000000000, },
/* 1 */ {5, 0X0000000000000000, },
/* 2 */ {5, 0X000000000000001C, },
/* 3 */ {5, 0X0000000000000000, },
/* 4 */ {5, 0X0000000000000000, },
/* 5 */ {5, 0X0000000000000000, },
/* 6 */ {5, 0X0000000000000000, },
#endif
} /* End SXLIGPOPn_set */;

#endif


#include "SXLIGparsact.h"
