
include $(SX)/etc/make/gccoptions

CSYNT=$(SX)/csynt
LIB=$(CSYNT)/lib

CC=$(COMPILER) -I$(CSYNT) -I$(SXINCL) -I$(SXINCL)/util $(CFLAGS)

all: $(SXBIN)/csynt.out

$(SXBIN)/csynt.out:	$(CSYNT)/csynt.out
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

$(LIB)/csynt.o:		csynt_optim.h		\
			$(SXINCL)/util/B_tables.h		\
			$(SXINCL)/util/bstr.h		\
			$(SXINCL)/sxunix.h		\
			csynt.c
	$(CC) -o $@ -c csynt.c

$(LIB)/LALR1.o:		csynt_optim.h		\
			$(SXINCL)/util/B_tables.h		\
			$(SXINCL)/util/D_tables.h		\
			$(SXINCL)/varstr.h		\
			$(SXINCL)/sxunix.h		\
			LALR1.c
	$(CC) -o $@ -c LALR1.c

$(LIB)/path.o:		csynt_optim.h		\
			$(SXINCL)/util/B_tables.h		\
			$(SXINCL)/varstr.h		\
			$(SXINCL)/sxunix.h		\
			path.c
	$(CC) -o $@ -c path.c

$(LIB)/optim_lo.o:	csynt_optim.h		\
			$(SXINCL)/util/B_tables.h		\
			$(SXINCL)/util/P_tables.h		\
			$(SXINCL)/util/bstr.h		\
			$(SXINCL)/sxunix.h		\
			optim_lo.c
	$(CC) -o $@ -c optim_lo.c

$(LIB)/xprdct_mngr.o:	$(SXINCL)/sxunix.h		\
			xprdct_mngr.c
	$(CC) -o $@ -c xprdct_mngr.c

$(LIB)/single.o:	csynt_optim.h		\
			$(SXINCL)/util/B_tables.h		\
			$(SXINCL)/util/P_tables.h		\
			$(SXINCL)/sxunix.h		\
			single.c
	$(CC) -o $@ -c single.c

$(LIB)/algoV.o:		csynt_optim.h		\
			$(SXINCL)/util/B_tables.h		\
			$(SXINCL)/util/P_tables.h		\
			$(SXINCL)/sxunix.h		\
			algoV.c
	$(CC) -o $@ -c algoV.c

$(LIB)/OPTIM.o:		csynt_optim.h		\
			$(SXINCL)/util/B_tables.h		\
			$(SXINCL)/util/P_tables.h		\
			$(SXINCL)/sxunix.h		\
			OPTIM.c
	$(CC) -o $@ -c OPTIM.c

clean:
	/bin/rm -f $(LIB)/*.o $(CSYNT)/csynt.out
