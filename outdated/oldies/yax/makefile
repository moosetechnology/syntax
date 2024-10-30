
include $(SX)/etc/make/gccoptions

YAX=$(SX)/../outdated/oldies/yax

LIB=$(YAX)/lib.$(ARCH)

CC=$(COMPILER) -I$(SXINCL) -I$(SXINCL)/util $(CFLAGS)

all: $(YAX)/yax.out

NEEDS=	$(LIB)/yax_put.o	\
	$(LIB)/yax.o		\
	$(LIB)/yax_main.o	\
	$(LIB)/ydec_act.o	\
	$(LIB)/ydec_t.o		\
	$(LIB)/ydef_act.o	\
	$(LIB)/ydef_t.o

$(YAX)/yax.out:		$(NEEDS) $(SXBIBS) $(SXLIB)/libsxbnf.a
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXLIB)/libsxbnf.a $(SXBIBS) || (rm -f $@; exit 1)

$(LIB)/yax_main.o:	$(SXINCL)/sxunix.h		\
	 		$(SXINCL)/util/B_tables.h	\
			yax_vars.h	\
			$(SXINCL)/util/bnf_vars.h	\
			yax_main.c
	$(CC) -o $@ -c yax_main.c

$(LIB)/yax.o: 		$(SXINCL)/sxunix.h		\
	 		$(SXINCL)/util/B_tables.h	\
	 		$(SXINCL)/put_edit.h	\
			yax_vars.h	\
			$(SXINCL)/util/bnf_vars.h	\
			yax.c
	$(CC) -o $@ -c yax.c

$(LIB)/yax_put.o:	$(SXINCL)/sxunix.h		\
	 		$(SXINCL)/util/B_tables.h	\
			yax_vars.h	\
			$(SXINCL)/util/bnf_vars.h	\
			yax_put.c
	$(CC) -o $@ -c yax_put.c

$(LIB)/ydec_act.o:	$(SXINCL)/sxunix.h		\
	 		$(SXINCL)/util/B_tables.h	\
			yax_vars.h	\
			$(SXINCL)/util/bnf_vars.h	\
			ydec_act.c
	$(CC) -o $@ -c ydec_act.c

$(LIB)/ydef_act.o:	$(SXINCL)/sxunix.h		\
			$(SXINCL)/util/B_tables.h	\
			yax_vars.h	\
			$(SXINCL)/util/bnf_vars.h	\
			ydef_act.c
	$(CC) -o $@ -c ydef_act.c

$(LIB)/ydec_t.o:		$(SXINCL)/sxunix.h		\
			ydec_t.c
	$(CC) -o $@ -c ydec_t.c \
		-DSEMACT=dec_semact

$(LIB)/ydef_t.o:		$(SXINCL)/sxunix.h		\
			ydef_t.c
	$(CC) -o $@ -c ydef_t.c \
		-DSEMACT=def_semact

# debut de code produit par "make-makefile ydec"

ydec_t.c: ydec.bt ydec.st ydec.pt ydec.rt 
	tables_c -name dec_tables ydec > $@

ydec.bt  : ydec.bnf
	semact ydec.bnf 

ydec.st: ydec.bt ydec.lecl
	lecl ydec.lecl || [ $$? = 1 ]

ydec.pt: ydec.bt
	csynt ydec || [ $$? = 1 ]

ydec.rt: ydec.bt ydec.st ydec.recor
	recor ydec.recor

# fin de code produit par "make-makefile ydec"

# debut de code produit par "make-makefile ydef"

ydef_t.c: ydef.bt ydef.st ydef.pt ydef.rt 
	tables_c -name def_tables ydef > $@

ydef.bt  : ydef.bnf
	semact ydef.bnf 

ydef.st: ydef.bt ydef.lecl
	lecl ydef.lecl || [ $$? = 1 ]

ydef.pt: ydef.bt
	csynt ydef || [ $$? = 1 ]

ydef.rt: ydef.bt ydef.st ydef.recor
	recor ydef.recor

# fin de code produit par "make-makefile ydef"

clean:
	/bin/rm -f $(LIB)/*.o $(YAX)/yax.out
	/bin/rm -f $(YAX)/ydec.*.l $(YAX)/ydef.*.l
	/bin/rm -f $(YAX)/ydec.bt $(YAX)/ydec.pt $(YAX)/ydec.rt $(YAX)/ydec.st
	/bin/rm -f $(YAX)/ydec_t.c
	/bin/rm -f $(YAX)/ydef.bt $(YAX)/ydef.pt $(YAX)/ydef.rt $(YAX)/ydef.st
	/bin/rm -f $(YAX)/ydef_t.c

