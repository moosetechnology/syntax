
include $(SX)/etc/make/gccoptions

TDEF=$(SX)/tdef
LIB=$(TDEF)/lib.$(ARCH)
BOOT=$(TDEF)/boot

CC=$(COMPILER) -I$(SXINCL) -I$(SXINCL)/util $(CFLAGS)

all: $(SXBIN)/tdef.out

NEEDS=	$(LIB)/tdef_t.o			\
	$(LIB)/tdef_main.o		\
	$(LIB)/tdef_sact.o		\
	$(LIB)/tdef_action.o

$(SXBIN)/tdef.out:	$(NEEDS)	$(SXBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@;exit 1)

$(LIB)/tdef_t.o: 	$(SXINCL)/sxunix.h		\
			$(BOOT)/tdef_t.c
	$(CC) -o $@ -c $(BOOT)/tdef_t.c		\
			-Dsxtables=tdef_tables	\
			-DSCANACT=tdef_scan_act	\
			-DSEMACT=tdef_action

$(LIB)/tdef_main.o:	$(SXINCL)/sxunix.h	\
			tdef_main.c
	$(CC) -o $@ -c tdef_main.c

$(LIB)/tdef_sact.o:	$(SXINCL)/sxunix.h		\
			tdef_sact.c
	$(CC) -o $@ -c tdef_sact.c

$(LIB)/tdef_action.o:	$(SXINCL)/sxunix.h		\
			$(SXINCL)/put_edit.h		\
			$(SXINCL)/util/B_tables.h	\
			tdef_action.c
	$(CC) -o $@ -c tdef_action.c

# debut de code produit par "make-makefile tdef"

bootstrap: tdef_t.c  

tdef_t.c: tdef.bt tdef.st tdef.pt tdef.rt 
	tables_c tdef > $@

tdef.bt  : tdef.bnf
	semact tdef.bnf 

tdef.st: tdef.bt tdef.lecl
	lecl tdef.lecl || [ $$? = 1 ]

tdef.pt: tdef.bt
	csynt tdef || [ $$? = 1 ]

tdef.rt: tdef.bt tdef.st tdef.recor
	recor tdef.recor

# fin de code produit par "make-makefile tdef"

clean:
	/bin/rm -f $(LIB)/*.o
	/bin/rm -f tdef_t.c
# pas de tdef_td.h
	/bin/rm -f tdef.bt tdef.pt tdef.rt tdef.st 
# pas de prio.dt 
	/bin/rm -f tdef.bn.l tdef.lc.l tdef.pr.l tdef.rc.l 
# pas de prio.la.l prio.td.l 

