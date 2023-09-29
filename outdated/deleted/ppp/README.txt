
Hubert Garavel - Tue Jul 11 16:15:11 CEST 2023

According to Pierre Boullier, ppp was a Pascal pretty-printer.

This tool no longer compiles in 2023, because of a missing file ppp_main.c
that is not present in the SYNTAX distribution.

Perhaps, the ppp_main.c file could be rewritten by taking inspiration from
the *_main.c files of other pretty-printers (ppada, ppc, pplecl, pprecor...)
but Pascal is no longer used, and there are already enough pretty-printers. 

