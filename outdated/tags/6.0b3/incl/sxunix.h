#ifndef sxunix_h
#define sxunix_h

/* This include file to avoid recompilations and provide a much */
/* agreeable interface to SYNTAX */


#define ACTION SXACTION
#define BEGIN SXBEGIN
#define BOOLEAN SXBOOLEAN
#define CAPITALISED_INITIAL SXCAPITALISED_INITIAL
#define CASE SXCASE
#define CHAR_TO_STRING SXCHAR_TO_STRING
#define CLONE SXCLONE
#define CLOSE SXCLOSE
#define CURRENT_SCANNED_CHAR SXCURRENT_SCANNED_CHAR
#define DERIVED SXDERIVED
#define DO SXDO
#define EMPTY_STE SXEMPTY_STE
#define END SXEND
#define ERROR SXERROR
#define ERROR_STE SXERROR_STE
#undef	FALSE
#define FALSE SXFALSE
#define FINAL SXFINAL
#define INHERITED SXINHERITED
#define INIT SXINIT
#define LAST_ELEM SXLAST_ELEM
#define LEFT SXLEFT
#define LOWER_CASE SXLOWER_CASE
#define NEWLINE SXNEWLINE
#define NODE_HEADER_S SXNODE_HEADER_S
#define NO_SPECIAL_CASE SXNO_SPECIAL_CASE
#define NUL SXNUL
#define OPEN SXOPEN
#define PREDICATE SXPREDICATE
#define SCAN_LA_P SXSCAN_LA_P
#define SEMPASS SXSEMPASS
#define SEVERITIES SXSEVERITIES
#define SHORT SXSHORT
#define STACKnewtop SXSTACKnewtop
#define STACKreduce SXSTACKreduce
#define STACKtoken SXSTACKtoken
#define STACKtop SXSTACKtop
#define TAB_INTERVAL SXTAB_INTERVAL
#undef	TRUE
#define TRUE SXTRUE
#define UNDO SXUNDO
#define UPPER_CASE SXUPPER_CASE
#define VISITED SXVISITED
#define VISIT_KIND SXVISIT_KIND
#define VOID SXVOID
#define VOID_NAME SXVOID_NAME
#define atc_local_variables sxatc_local_variables
#define lv sxlv
#define lv_s sxlv_s
#define node_header_s sxnode_header_s
#define syntax sxsyntax

#ifdef NODE
#  define SXNODE NODE
#endif

#include    "sxu2.h"

#ifndef NODE
# define NODE SXNODE
#endif

#endif
