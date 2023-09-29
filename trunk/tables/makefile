
include $(SX)/etc/make/gccoptions

TABLES=$(SX)/tables
SRC=$(TABLES)/src
LIB=$(TABLES)/lib
INCL=$(TABLES)/incl

CC=$(COMPILER) -I$(SXINCL) -I$(INCL) -I$(SXINCL)/util $(CFLAGS)

all: $(SX)/bin/tables_c.out

$(SX)/bin/tables_c.out:	$(TABLES)/tables_c.out
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?


NEEDS=	$(LIB)/init_tbl.o	\
	$(LIB)/out_P_tbl.o	\
	$(LIB)/out_RCVR_tbl.o	\
	$(LIB)/out_S_tbl.o	\
	$(LIB)/out_PP_tbl.o	\
	$(LIB)/out_T_tbl.o	\
	$(LIB)/out_DP_tbl.o	\
	$(LIB)/out_DS_tbl.o	\
	$(LIB)/out_lig_tbl.o	\
	$(LIB)/out.o		\
	$(LIB)/tables_c.o

$(TABLES)/tables_c.out:	$(NEEDS)	$(SXBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@;exit 1)

$(LIB)/tables_c.o:	$(SXINCL)/sxunix.h	\
			$(INCL)/tables.h	\
			$(INCL)/out.h		\
			$(SXINCL)/sem_by.h	\
			$(SXINCL)/sxba.h	\
			$(TABLES)/src/tables_c.c
	$(CC) -o $@ -c $(TABLES)/src/tables_c.c

$(LIB)/init_tbl.o:	$(SXINCL)/sxunix.h	\
			$(INCL)/tables.h	\
			$(SXINCL)/util/P_tables.h	\
			$(SXINCL)/util/S_tables.h	\
			$(SXINCL)/util/PP_tables.h	\
			$(SXINCL)/util/T_tables.h	\
			$(SRC)/init_tbl.c
	$(CC) -o $@ -c $(SRC)/init_tbl.c

$(LIB)/out_S_tbl.o:	$(SXINCL)/sxunix.h	\
			$(INCL)/tables.h	\
			$(INCL)/out.h		\
			$(SXINCL)/util/S_tables.h	\
			$(TABLES)/src/out_S_tbl.c
	$(CC) -o $@ -c $(TABLES)/src/out_S_tbl.c

$(LIB)/out_PP_tbl.o:	$(SXINCL)/sxunix.h	\
			$(INCL)/tables.h	\
			$(INCL)/out.h		\
			$(SXINCL)/util/PP_tables.h	\
			$(SRC)/out_PP_tbl.c
	$(CC) -o $@ -c $(SRC)/out_PP_tbl.c

$(LIB)/out_T_tbl.o:	$(SXINCL)/sxunix.h	\
			$(INCL)/tables.h	\
			$(INCL)/out.h		\
			$(SXINCL)/util/T_tables.h	\
			$(SRC)/out_T_tbl.c
	$(CC) -o $@ -c $(SRC)/out_T_tbl.c

$(LIB)/out_P_tbl.o:	$(SXINCL)/sxunix.h	\
			$(INCL)/tables.h	\
			$(INCL)/out.h		\
			$(SXINCL)/util/P_tables.h	\
			$(TABLES)/src/out_P_tbl.c
	$(CC) -o $@ -c $(TABLES)/src/out_P_tbl.c

$(LIB)/out_RCVR_tbl.o:	$(SXINCL)/sxunix.h	\
			$(INCL)/tables.h	\
			$(INCL)/out.h		\
			$(SXINCL)/util/P_tables.h	\
			$(SXINCL)/util/R_tables.h	\
			$(SXINCL)/sxba.h	\
			$(TABLES)/src/out_RCVR_tbl.c
	$(CC) -o $@ -c $(TABLES)/src/out_RCVR_tbl.c

# simplified version of out_DP_tbl
$(LIB)/out_DP_tbl.o:	$(SXINCL)/sxunix.h	\
			$(TABLES)/src/out_DP_tbl.c
	$(CC) -o $@ -c $(TABLES)/src/out_DP_tbl.c

$(LIB)/out_DS_tbl.o:	$(SXINCL)/sxunix.h	\
			$(TABLES)/src/out_DS_tbl.c
	$(CC) -o $@ -c $(TABLES)/src/out_DS_tbl.c

# simplified version of out_lig_tbl
$(LIB)/out_lig_tbl.o:	$(SXINCL)/sxunix.h	\
			$(INCL)/tables.h	\
			$(INCL)/out.h		\
			$(TABLES)/src/out_lig_tbl.c
	$(CC) -o $@ -c $(TABLES)/src/out_lig_tbl.c

$(LIB)/out.o:		$(SXINCL)/sxunix.h	\
			$(SXINCL)/sem_by.h	\
			$(INCL)/tables.h	\
			$(SXINCL)/util/P_tables.h	\
			$(SXINCL)/util/S_tables.h	\
			$(SRC)/out.c
	$(CC) -o $@ -c $(SRC)/out.c

clean:
	/bin/rm -f $(LIB)/*.o $(TABLES)/tables_c.out
