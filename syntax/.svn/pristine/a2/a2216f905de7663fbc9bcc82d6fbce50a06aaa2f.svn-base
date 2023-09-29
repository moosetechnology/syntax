
include $(SX)/etc/make/gccoptions

CSYNT=$(SX)/csynt
SRC=$(CSYNT)/src
LIB=$(CSYNT)/lib
INCL=$(CSYNT)/incl

CC=$(COMPILER) -I$(INCL) -I$(SXINCL) -I$(SXINCL)/util $(CFLAGS)

all: $(SX)/bin/csynt.out

$(SX)/bin/csynt.out:	$(CSYNT)/csynt.out
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?

NEEDS=	$(LIB)/csynt.o			\
	$(LIB)/LALR1.o			\
	$(LIB)/path.o			\
	$(LIB)/single.o			\
	$(LIB)/algoV.o			\
	$(LIB)/optim_lo.o		\
	$(LIB)/xprdct_mngr.o		\
	$(LIB)/OPTIM.o

$(CSYNT)/csynt.out:	$(NEEDS)	$(SXBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@;exit 1)

$(LIB)/csynt.o:		$(INCL)/csynt_optim.h		\
			$(SXINCL)/util/B_tables.h		\
			$(SXINCL)/util/bstr.h		\
			$(SXINCL)/sxunix.h		\
			$(SRC)/csynt.c
	$(CC) -o $@ -c $(SRC)/csynt.c

$(LIB)/LALR1.o:		$(INCL)/csynt_optim.h		\
			$(SXINCL)/util/B_tables.h		\
			$(SXINCL)/util/D_tables.h		\
			$(SXINCL)/varstr.h		\
			$(SXINCL)/sxunix.h		\
			$(SRC)/LALR1.c
	$(CC) -o $@ -c $(SRC)/LALR1.c

$(LIB)/path.o:		$(INCL)/csynt_optim.h		\
			$(SXINCL)/util/B_tables.h		\
			$(SXINCL)/varstr.h		\
			$(SXINCL)/sxunix.h		\
			$(SRC)/path.c
	$(CC) -o $@ -c $(SRC)/path.c

$(LIB)/optim_lo.o:	$(INCL)/csynt_optim.h		\
			$(SXINCL)/util/B_tables.h		\
			$(SXINCL)/util/P_tables.h		\
			$(SXINCL)/util/bstr.h		\
			$(SXINCL)/sxunix.h		\
			$(SRC)/optim_lo.c
	$(CC) -o $@ -c $(SRC)/optim_lo.c

$(LIB)/xprdct_mngr.o:	$(SXINCL)/sxunix.h		\
			$(SRC)/xprdct_mngr.c
	$(CC) -o $@ -c $(SRC)/xprdct_mngr.c

$(LIB)/single.o:	$(INCL)/csynt_optim.h		\
			$(SXINCL)/util/B_tables.h		\
			$(SXINCL)/util/P_tables.h		\
			$(SXINCL)/sxunix.h		\
			$(SRC)/single.c
	$(CC) -o $@ -c $(SRC)/single.c

$(LIB)/algoV.o:		$(INCL)/csynt_optim.h		\
			$(SXINCL)/util/B_tables.h		\
			$(SXINCL)/util/P_tables.h		\
			$(SXINCL)/sxunix.h		\
			$(SRC)/algoV.c
	$(CC) -o $@ -c $(SRC)/algoV.c

$(LIB)/OPTIM.o:		$(INCL)/csynt_optim.h		\
			$(SXINCL)/util/B_tables.h		\
			$(SXINCL)/util/P_tables.h		\
			$(SXINCL)/sxunix.h		\
			$(SRC)/OPTIM.c
	$(CC) -o $@ -c $(SRC)/OPTIM.c

clean:
	/bin/rm -f $(LIB)/*.o $(CSYNT)/csynt.out
