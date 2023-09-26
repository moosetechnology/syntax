#ifndef sxba_bag_h
#define sxba_bag_h

#include "sxba.h"

typedef struct {
    struct bag_disp_hd {
	SXBA_INDEX	size;
	SXBA_ELT	*set;
    }		*hd;
    SXBA_ELT	*pool_top;
    SXBA_INDEX   hd_size, hd_top, hd_high, pool_size, room;
    char	*name;
    SXBA_INDEX	used_size, prev_used_size, total_size;
    FILE        *file_name; /* #if statistics */
} bag_header;

/* size est la taille initiale (en SXBA_ELT) du reservoir */
extern void  sxba_bag_alloc (bag_header *pbag, char *name, SXBA_INDEX size, FILE *file_name);
/* Meme effet qu'un sxba_calloc (bits_number) */
extern SXBA  sxba_bag_get (bag_header *pbag, SXBA_INDEX bits_number);
extern void  sxba_bag_clear (bag_header *pbag);
extern void  sxba_bag_free (bag_header *pbag);

#endif /* sxba_bag_h */
