
include $(SX)/etc/make/gccoptions

TABLES=$(SX)/tables
LIB=$(TABLES)/lib

CC=$(COMPILER) -I$(SXINCL) -I$(TABLES) -I$(SXINCL)/util $(CFLAGS)

all: $(SXBIN)/tables_c.out

$(SXBIN)/tables_c.out:	$(TABLES)/tables_c.out
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
			tables.h	\
			out.h		\
			$(SXINCL)/sem_by.h	\
			$(SXINCL)/sxba.h	\
			tables_c.c
	$(CC) -o $@ -c tables_c.c

$(LIB)/init_tbl.o:	$(SXINCL)/sxunix.h	\
			tables.h	\
			$(SXINCL)/util/P_tables.h	\
			$(SXINCL)/util/S_tables.h	\
			$(SXINCL)/util/PP_tables.h	\
			$(SXINCL)/util/T_tables.h	\
			init_tbl.c
	$(CC) -o $@ -c init_tbl.c

$(LIB)/out_S_tbl.o:	$(SXINCL)/sxunix.h	\
			tables.h	\
			out.h		\
			$(SXINCL)/util/S_tables.h	\
			out_S_tbl.c
	$(CC) -o $@ -c out_S_tbl.c

$(LIB)/out_PP_tbl.o:	$(SXINCL)/sxunix.h	\
			tables.h	\
			out.h		\
			$(SXINCL)/util/PP_tables.h	\
			out_PP_tbl.c
	$(CC) -o $@ -c out_PP_tbl.c

$(LIB)/out_T_tbl.o:	$(SXINCL)/sxunix.h	\
			tables.h	\
			out.h		\
			$(SXINCL)/util/T_tables.h	\
			out_T_tbl.c
	$(CC) -o $@ -c out_T_tbl.c

$(LIB)/out_P_tbl.o:	$(SXINCL)/sxunix.h	\
			tables.h	\
			out.h		\
			$(SXINCL)/util/P_tables.h	\
			out_P_tbl.c
	$(CC) -o $@ -c out_P_tbl.c

$(LIB)/out_RCVR_tbl.o:	$(SXINCL)/sxunix.h	\
			tables.h	\
			out.h		\
			$(SXINCL)/util/P_tables.h	\
			$(SXINCL)/util/R_tables.h	\
			$(SXINCL)/sxba.h	\
			out_RCVR_tbl.c
	$(CC) -o $@ -c out_RCVR_tbl.c

# simplified version of out_DP_tbl
$(LIB)/out_DP_tbl.o:	$(SXINCL)/sxunix.h	\
			out_DP_tbl.c
	$(CC) -o $@ -c out_DP_tbl.c

$(LIB)/out_DS_tbl.o:	$(SXINCL)/sxunix.h	\
			out_DS_tbl.c
	$(CC) -o $@ -c out_DS_tbl.c

# simplified version of out_lig_tbl
$(LIB)/out_lig_tbl.o:	$(SXINCL)/sxunix.h	\
			tables.h	\
			out.h		\
			out_lig_tbl.c
	$(CC) -o $@ -c out_lig_tbl.c

$(LIB)/out.o:		$(SXINCL)/sxunix.h	\
			$(SXINCL)/sem_by.h	\
			tables.h	\
			$(SXINCL)/util/P_tables.h	\
			$(SXINCL)/util/S_tables.h	\
			out.c
	$(CC) -o $@ -c out.c

clean:
	/bin/rm -f $(LIB)/*.o $(TABLES)/tables_c.out
