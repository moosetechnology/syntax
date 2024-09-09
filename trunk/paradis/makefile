
include $(SX)/etc/make/gccoptions

PARADIS=$(SX)/paradis
LIB=$(PARADIS)/lib.$(ARCH)
INCL=$(PARADIS)/incl
BOOT=$(PARADIS)/boot

CC=$(COMPILER) -I$(BOOT) -I$(SXINCL) -I$(SXINCL)/util $(CFLAGS)

all: $(SXBIN)/paradis.out $(SXAUX)/print_ppt.out

NEEDS=		$(LIB)/paradis_t.o	\
		$(LIB)/paradis_main.o	\
		$(LIB)/paradis_smp.o

PRINT=		$(LIB)/print_ppt.o

$(SXBIN)/paradis.out:	$(NEEDS) $(SXBIBS) $(SXLIB)/libsxbnf.a
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXLIB)/libsxbnf.a $(SXBIBS) || (rm $@;exit 1)

$(SXAUX)/print_ppt.out: print_ppt.c $(SXBIBS) $(SXLIB)/libsxbnf.a
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXLIB)/libsxbnf.a $(SXBIBS) || (rm $@;exit 1)

$(LIB)/paradis_t.o: 	$(SXINCL)/sxunix.h $(BOOT)/paradis_t.c
	$(CC) -o $@ -c $(BOOT)/paradis_t.c		\
		-DSEMPASS=paradis_smp

$(LIB)/paradis_main.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/util/B_tables.h 	\
			$(SXINCL)/util/bnf_vars.h 	\
			paradis_main.c
	$(CC) -o $@ -c paradis_main.c

$(LIB)/paradis_smp.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/sem_by.h	\
			$(SXINCL)/util/PP_tables.h	\
			$(SXINCL)/util/B_tables.h	\
			$(SXINCL)/util/bnf_vars.h	\
			$(SXINCL)/put_edit.h	\
			$(SXINCL)/varstr.h	\
			paradis_smp.c
	$(CC) -o $@ -c paradis_smp.c

$(LIB)/print_ppt.o:	$(SXINCL)/util/PP_tables.h	\
			$(SXINCL)/sxunix.h	\
			print_ppt.c
	$(CC) -o $@ -c print_ppt.c

# debut de code produit par "make-makefile paradis"

bootstrap: paradis_t.c paradis.c.skeleton 

paradis_t.c: paradis.bt paradis.st paradis.pt paradis.rt paradis.att
	tables_c paradis > $@

paradis.bt paradis.att paradis.c.skeleton: paradis.at
	semat paradis.at  > paradis.c.skeleton

paradis.st: paradis.bt paradis.lecl
	lecl paradis.lecl || [ $$? = 1 ]

paradis.pt: paradis.bt
	csynt paradis || [ $$? = 1 ]

paradis.rt: paradis.bt paradis.st paradis.recor
	recor paradis.recor

# fin de code produit par "make-makefile paradis"

clean:
	/bin/rm -f $(LIB)/*.o $(SXAUX)/print_ppt.out
	/bin/rm -f paradis.att paradis.bt paradis.pt paradis.rt paradis.st
	/bin/rm -f paradis.bn.l paradis.lc.l paradis.rc.l
	/bin/rm -f paradis.c.skeleton paradis_t.c

