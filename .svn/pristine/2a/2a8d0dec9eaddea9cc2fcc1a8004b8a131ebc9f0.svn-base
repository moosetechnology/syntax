bin_PROGRAMS = cx

LNorCP = ln -s

CX =	$(top_srcdir)
SRC =	$(CX)/src
INCL =	$(CX)/incl
TMP=$(CX)/tmp
BIBS = $(syntaxlib)
SXINCL = $(syntaxincl)

AM_CPPFLAGS =	-I$(INCL) -I$(SXINCL) $(CFLAGS)
CC_FLAGS = $(CC) $(AM_CPPFLAGS) -c
COMPILER=cc

cx_SOURCES =	$(SRC)/cx_t.c			\
		$(SRC)/cx_main.c		\
		$(SRC)/cx_act.c			\
		$(SRC)/cx_scnr.c
cx_LDADD =	$(BIBS)

cx_t.o: 	$(SXINCL)/sxunix.h		\
		$(SRC)/cx_t.c
	$(CC_FLAGS) $(SRC)/cx_t.c			\
			-Dsxtables=cx_tables	\
			-Dsxscanner=cx_scanner	\
			-Dsxscan_it=cx_scan_it	\
			-DSEMACT=cx_act		\
			-Dsempass=sxvoid

cx_main.o:	$(SXINCL)/sxunix.h	\
			$(SRC)/cx_main.c
	$(CC_FLAGS) $(SRC)/cx_main.c

cx_scnr.o:	$(SXINCL)/sxunix.h	\
			$(SRC)/cx_scnr.c
	$(CC_FLAGS) $(SRC)/cx_scnr.c

cx_act.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/put_edit.h	\
			$(SRC)/cx_act.c
	$(CC_FLAGS) $(SRC)/cx_act.c
