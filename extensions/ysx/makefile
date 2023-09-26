
include $(SX)/etc/make/gccoptions

YSX=$(SX)/../extensions/ysx
LIB=$(YSX)/lib

CC=$(COMPILER) -I$(SXINCL) -I$(SXINCL)/util $(CFLAGS)

all: $(SX)/bin/ysx.out

$(SX)/bin/ysx.out:	$(YSX)/ysx.out
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?

NEEDS=	$(LIB)/ysx_main.o	\
	$(LIB)/ysx_smp.o	\
	$(LIB)/ysx_t.o		\
	$(LIB)/yaction_t.o

$(YSX)/ysx.out:		$(NEEDS)	$(SXBIBS)
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
	$(CC) -o $@ -c ysx_t.c -Dsempass=ysx_smp -Dsxtables=ysx_tables

$(LIB)/yaction_t.o:	$(SXINCL)/sxunix.h	\
			yaction_t.c
	$(CC) -o $@ -c yaction_t.c -DSCANACT=yaction -DSEMACT=yaction -Dsxtables=yaction_tables -DSCANACT_AND_SEMACT_ARE_IDENTICAL
# pour la documentation de cette macro SCANACT_AND_SEMACT_ARE_IDENTICAL,
# consulter le fichier $SX/tables/src/out.c et les commentaires de la
# fonction out_ext_int()

# debut de code produit par "make-makefile ysx"

ysx_t.c: ysx.bt ysx.st ysx.pt ysx.rt ysx.att
	tables_c ysx > $@

ysx.bt ysx.att ysx_mix.c: ysx.at
	semat ysx.at  > ysx_mix.c

ysx.st: ysx.bt ysx.lecl
	lecl ysx.lecl || [ $$? = 1 ]

ysx.pt: ysx.bt
	csynt ysx || [ $$? = 1 ]

ysx.rt: ysx.bt ysx.st ysx.recor
	recor ysx.recor

# fin de code produit par "make-makefile ysx"

# debut de code produit par "make-makefile yaction"

yaction_t.c: yaction.bt yaction.st yaction.pt yaction.rt 
	tables_c yaction > $@

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
	/bin/rm -f $(LIB)/*.o $(YSX)/ysx.out $(YSX)/ysx.*.l $(YSX)/yaction.*.l
	/bin/rm -f $(YSX)/ysx.att $(YSX)/ysx.bt $(YSX)/ysx.pt $(YSX)/ysx.rt
	/bin/rm -f $(YSX)/ysx.st $(YSX)/ysx_mix.c $(YSX)/ysx_t.c
	/bin/rm -f $(YSX)/yaction.bt $(YSX)/yaction.pt $(YSX)/yaction.rt
	/bin/rm -f $(YSX)/yaction.st $(YSX)/yaction_t.c


