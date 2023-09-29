#ifndef bag_h
#define bag_h
/* Gestionnaire de SXBA */

#include "sxba.h"

typedef struct {
    struct bag_disp_hd {
	int		size;
	SXBA_ELT	*set;
    }		*hd;
    SXBA_ELT	*pool_top;
    int		hd_size, hd_top, hd_high, pool_size, room;
    char	*name;
    int		used_size, prev_used_size, total_size; /* #if EBUG */
} bag_header;

extern void bag_alloc (/* bag_header *pbag;char *name;int size; */); /* size = nb initial de SXBA_ELT */
extern SXBA bag_get (/* bag_header *pbag;int size; */); /* size = nb de bits */
extern void bag_free (/* bag_header *pbag; */);
extern void bag_clear (/* bag_header *pbag; */); /* Ne remet pas a zero les zones restituees */
#endif
