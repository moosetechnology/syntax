
include $(SX)/etc/make/gccoptions

YSX=$(SX)/../extensions/ysx
BIN=$(YSX)/bin.$(ARCH)
LIB=$(YSX)/lib.$(ARCH)

CC=$(COMPILER) -I$(SXINCL) -I$(SXINCL)/util $(CFLAGS)

all: $(BIN)/ysx.out

NEEDS=	$(LIB)/ysx_main.o	\
	$(LIB)/ysx_smp.o	\
	$(LIB)/ysx_t.o		\
	$(LIB)/yaction_t.o

$(BIN)/ysx.out:		$(NEEDS)	$(SXBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@; exit 1)

$(LIB)/ysx_main.o:	$(SXINCL)/sxunix.h	\
			ysx_vars.h	\
			$(SXINCL)/put_edit.h	\
			ysx_main.c
	$(CC) -o $@ -c ysx_main.c

$(LIB)/ysx_smp.o:	$(SXINCL)/sxunix.h	\
			ysx_vars.h	\
			$(SXINCL)/put_edit.h	\
			$(SXINCL)/varstr.h	\
			ysx_smp.c
	$(CC) -o $@ -c ysx_smp.c

$(LIB)/ysx_t.o:		$(SXINCL)/sxunix.h	\
			ysx_t.c
	$(CC) -o $@ -c ysx_t.c -DSEMPASS=ysx_smp

$(LIB)/yaction_t.o:	$(SXINCL)/sxunix.h	\
			yaction_t.c
	$(CC) -o $@ -c yaction_t.c -DSCANACT=yaction_scanact -DSEMACT=yaction_semact

# debut de code produit par "make-makefile ysx"

ysx_t.c: ysx.bt ysx.st ysx.pt ysx.rt ysx.att
	tables_c -name ysx_tables ysx > $@

ysx.bt ysx.att ysx_mix.c: ysx.at
	semat ysx.at > ysx_mix.c

ysx.st: ysx.bt ysx.lecl
	lecl ysx.lecl || [ $$? = 1 ]

ysx.pt: ysx.bt
	csynt ysx || [ $$? = 1 ]

ysx.rt: ysx.bt ysx.st ysx.recor
	recor ysx.recor

# fin de code produit par "make-makefile ysx"

# debut de code produit par "make-makefile yaction"

yaction_t.c: yaction.bt yaction.st yaction.pt yaction.rt 
	tables_c -name yaction_tables yaction > $@

yaction.bt  : yaction.bnf
	semact yaction.bnf 

yaction.st: yaction.bt yaction.lecl
	lecl yaction.lecl || [ $$? = 1 ]

yaction.pt: yaction.bt
	csynt yaction || [ $$? = 1 ]

yaction.rt: yaction.bt yaction.st yaction.recor
	recor yaction.recor

# fin de code produit par "make-makefile yaction"

clean:
	/bin/rm -f $(LIB)/*.o $(BIN)/ysx.out $(YSX)/ysx.*.l $(YSX)/yaction.*.l
	/bin/rm -f $(YSX)/ysx.att $(YSX)/ysx.bt $(YSX)/ysx.pt $(YSX)/ysx.rt
	/bin/rm -f $(YSX)/ysx.st $(YSX)/ysx_mix.c $(YSX)/ysx_t.c
	/bin/rm -f $(YSX)/yaction.bt $(YSX)/yaction.pt $(YSX)/yaction.rt
	/bin/rm -f $(YSX)/yaction.st $(YSX)/yaction_t.c


