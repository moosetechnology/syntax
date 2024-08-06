
include $(SX)/etc/make/gccoptions

TABLES=$(SX)/../extensions/tables
BIN=$(TABLES)/bin.$(ARCH)
LIB=$(TABLES)/lib.$(ARCH)

SXEXTBIBS=$(SXLIB)/libsxext.a $(SXBIBS)

CC=$(COMPILER) -I$(SXINCL) -I$(TABLES) -I$(SXINCL)/util -I$(SX)/../extensions/incl -I$(SX)/../outdated/oldies/LIG/incl $(CFLAGS)

all: $(BIN)/tables_c.out

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

$(BIN)/tables_c.out:	$(NEEDS)	$(SXEXTBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXEXTBIBS) || (rm $@;exit 1)

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

$(LIB)/out_DP_tbl.o:	$(SXINCL)/sxunix.h	\
                        $(SX)/../extensions/incl/sxdynam_parser.h	\
			out_DP_tbl.c
	$(CC) -o $@ -c out_DP_tbl.c

$(LIB)/out_DS_tbl.o:	$(SXINCL)/sxunix.h	\
                        $(SX)/../extensions/incl/sxdynam_scanner.h	\
			out_DS_tbl.c
	$(CC) -o $@ -c out_DS_tbl.c

$(LIB)/out_lig_tbl.o:	$(SXINCL)/sxunix.h	\
			tables.h	\
			out.h		\
			$(SX)/../outdated/oldies/LIG/incl/lig.h	\
			out_lig_tbl.c
	$(CC) -o $@ -c -I$(SXINCL)/util out_lig_tbl.c

$(LIB)/out.o:		$(SXINCL)/sxunix.h	\
			$(SXINCL)/sem_by.h	\
			tables.h	\
			$(SXINCL)/util/P_tables.h	\
			$(SXINCL)/util/S_tables.h	\
			out.c
	$(CC) -o $@ -c out.c

clean:
	/bin/rm -f $(LIB)/*.o $(BIN)/tables_c.out

