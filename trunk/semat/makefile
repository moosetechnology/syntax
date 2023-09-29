
include $(SX)/etc/make/gccoptions

SEMAT=$(SX)/semat
LIB=$(SEMAT)/lib
BOOT=$(SEMAT)/boot

BNF=$(SX)/bnf
BNFLIB=$(BNF)/lib

CC=$(COMPILER) -I$(BNF) -I$(BOOT) -I$(SXINCL) -I$(SXINCL)/util $(CFLAGS)

all: $(SX)/bin/semat.out	\
     $(SXAUX)/print_att.out

$(SX)/bin/semat.out:	$(SEMAT)/semat.out
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?

NEEDS=	$(LIB)/genat_c.o		\
	$(LIB)/semat_main.o		\
	$(LIB)/semat_sact.o		\
	$(LIB)/semat_t.o		\
	$(BNFLIB)/bnf_folcon.o		\
	$(BNFLIB)/bnf_lo.o    		\
	$(BNFLIB)/bnf_protest.o		\
	$(BNFLIB)/bnf_sact.o   		\
	$(BNFLIB)/bnf_smp.o    		\
	$(BNFLIB)/bnf_symtab.o		\
	$(BNFLIB)/bnf_t.o

PRINT=	$(LIB)/print_att.o

$(SEMAT)/semat.out:	$(NEEDS)	$(SXBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@;exit 1)

$(LIB)/semat_main.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/util/B_tables.h	\
			$(BNF)/bnf_vars.h	\
			semat_main.c
	$(CC) -o $@ -c semat_main.c

$(LIB)/genat_c.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/util/B_tables.h	\
			$(SXINCL)/util/T_tables.h	\
			$(BNF)/bnf_vars.h	\
			genat_c.c
	$(CC) -o $@ -c genat_c.c

$(LIB)/semat_sact.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/sem_by.h	\
			$(SXINCL)/util/B_tables.h	\
			$(BNF)/bnf_vars.h 	\
			$(SXINCL)/util/T_tables.h	\
			$(SXINCL)/put_edit.h	\
			$(SXINCL)/varstr.h	\
			semat_sact.c
	$(CC) -o $@ -c semat_sact.c

$(LIB)/semat_t.o:	$(SXINCL)/sxunix.h $(BOOT)/semat_t.c
	$(CC) -o $@ -c $(BOOT)/semat_t.c			\
		-Dsxtables=semat_tables		\
		-DSEMACT=semat_semact		\
		-Dsempass=semat_sem

$(SXAUX)/print_att.out:	$(PRINT)	$(SXBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(PRINT) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@;exit 1)

$(LIB)/print_att.o:	$(SXINCL)/util/T_tables.h	\
			print_att.c
	$(CC) -o $@ -c print_att.c

# debut de code produit par "make-makefile semat"

bootstrap: semat_t.c  

semat_t.c: semat.bt semat.st semat.pt semat.rt 
	tables_c semat > $@

semat.bt  : semat.bnf
	semact semat.bnf 

semat.st: semat.bt semat.lecl
	lecl semat.lecl || [ $$? = 1 ]

semat.pt: semat.bt
	csynt semat || [ $$? = 1 ]

semat.rt: semat.bt semat.st semat.recor
	recor semat.recor

# fin de code produit par "make-makefile semat"

clean:
	/bin/rm -f $(LIB)/*.o $(SXAUX)/print_att.out $(SEMAT)/semat.out
	/bin/rm -f semat_t.c
	/bin/rm -f semat.bt semat.pt semat.rt semat.st 
# pas de semat.dt 
	/bin/rm -f semat.bn.l semat.lc.l semat.rc.l 
# pas de semat.la.l semat.td.l semat.pr.l 

