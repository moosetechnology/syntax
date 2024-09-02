
include $(SX)/etc/make/gccoptions

RECOR=$(SX)/recor
LIB=$(RECOR)/lib.$(ARCH)
BOOT=$(RECOR)/boot

CC=$(COMPILER) -I$(RECOR) -I$(BOOT) -I$(SXINCL) -I$(SXINCL)/util $(CFLAGS)

all: $(SXBIN)/recor.out	\
     $(SXAUX)/print_rt.out

NEEDS=	$(LIB)/recor_t.o		\
	$(LIB)/recor_main.o		\
	$(LIB)/recor_sact.o		\
	$(LIB)/recor_smp.o

PRINT=	$(LIB)/print_rt.o

$(SXBIN)/recor.out:	$(NEEDS)	$(SXBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@;exit 1)

$(LIB)/recor_t.o: 	$(SXINCL)/sxunix.h		\
			$(BOOT)/recor_t.c
	$(CC) -o $@ -c $(BOOT)/recor_t.c				\
			-DSCANACT=recor_scan_act	\
			-DSEMPASS=recor_smp

$(LIB)/recor_main.o:	$(SXINCL)/sxunix.h		\
			recor_main.c
	$(CC) -o $@ -c recor_main.c

$(LIB)/recor_sact.o: 	$(SXINCL)/sxunix.h		\
			recor_sact.c
	$(CC) -o $@ -c recor_sact.c

$(LIB)/recor_smp.o:	$(SXINCL)/sxunix.h		\
			$(SXINCL)/put_edit.h		\
			$(RECOR)/recor_node.h		\
			$(SXINCL)/util/R_tables.h	\
			$(SXINCL)/util/B_tables.h	\
			$(SXINCL)/util/S_tables.h	\
			recor_smp.c
	$(CC) -o $@ -c recor_smp.c

$(SXAUX)/print_rt.out:	$(PRINT)	$(SXBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(PRINT) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@;exit 1)

$(LIB)/print_rt.o:	$(SXINCL)/sxunix.h		\
			$(SXINCL)/util/R_tables.h	\
			print_rt.c
	$(CC) -o $@ -c print_rt.c

# debut de code produit par "make-makefile recor"

bootstrap: recor_t.c recor.c.skeleton 

recor_t.c: recor.bt recor.st recor.pt recor.rt recor.att
	tables_c recor > $@

recor.bt recor.att recor.c.skeleton: recor.at
	semat recor.at  > recor.c.skeleton

recor.st: recor.bt recor.lecl
	lecl recor.lecl || [ $$? = 1 ]

recor.pt: recor.bt
	csynt recor || [ $$? = 1 ]

recor.rt: recor.bt recor.st recor.recor
	recor recor.recor

# fin de code produit par "make-makefile recor"

clean:
	/bin/rm -f $(LIB)/*.o $(SXAUX)/print_rt.out
	/bin/rm -f recor.att recor.bt recor.pt recor.rt recor.st
	/bin/rm -f recor.bn.l recor.lc.l recor.rc.l
	/bin/rm -f recor.c.skeleton recor_t.c


