#ifndef snt_semact_h
#define snt_semact_h

#include "SXante.h"

SX_GLOBAL_VAR int               Xpq_top;

SX_GLOBAL_VAR int               *Aij2sntid;
SX_GLOBAL_VAR struct snt {
  int lb,ub,Aij;
  int id;
  VARSTR comm;
  char *cfirst;
  char *clast;
  int bool;
  int pruning_reason;
} *snts;
SX_GLOBAL_VAR int               maxsnt;

#if 0
#ifndef CHUNK_PREFIX
#define CHUNK_PREFIX "Easy_"
#define CHUNK_PREFIX_LENGTH = 5;
#endif /* CHUNK_PREFIX */

#define GN 0
#define GA 1
#define GR 2
#define GP 3
#define NV 4
#define PV 5
#define ZZ 6

#define MAX_CHUNK_ID 6
#endif /* 0 */

#define sntfilter_h
#include ntf_h
#undef sntfilter_h

#endif /* snt_semact_h */
