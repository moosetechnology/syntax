
include $(SX)/etc/make/gccoptions

SXEXTBIBS=$(SXLIB)/libsxext.a $(SXBIBS)

ND=$(SX)/../outdated/oldies

C=$(ND)/c
LIB=$(C)/lib.$(ARCH)

CC=cc -I$(SXINCL) -I$(SXEXTINCL) $(CFLAGS)

ALL=	$(C)/c.out			\
	$(C)/dc.out			\
	$(C)/ndc.out

all: $(ALL)

C_NEEDS=$(LIB)/sxmain.o			\
	$(LIB)/c_desambig.o		\
	$(LIB)/c_t.o

DC_NEEDS=$(LIB)/sxmain.o		\
	 $(LIB)/dc_parsact.o		\
	 $(LIB)/dc_smp.o		\
	 $(LIB)/dc_t.o

NDC_NEEDS=$(LIB)/sxmain.o		\
	 $(LIB)/ndc_parsact.o		\
	 $(LIB)/ndc_t.o

$(C)/c.out:	$(C_NEEDS)	$(SXBIBS)
	cc -o $@ $(C_NEEDS) $(CFLAGS) $(LDFLAGS) $(SXEXTBIBS) || (rm -f $@;exit 1)

$(LIB)/sxmain.o:	$(SXINCL)/sxunix.h	\
			sxmain.c
	$(CC) -o $@ -c sxmain.c


$(LIB)/c_t.o:		$(SXINCL)/sxunix.h 	\
			$(SXINCL)/sxba.h 	\
			c_t.c
	$(CC) -o $@ -c c_t.c		\
		-DESAMBIG=c_desambig		\
		-DPARSACT=sxjvoid		\
		-DSEMACT=sxwvoid


$(LIB)/c_desambig.o:	$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXINCL)/XxY.h		\
			$(SXINCL)/XH.h			\
			$(SXINCL)/SS.h			\
			$(SXEXTINCL)/sxnd.h		\
			$(SXEXTINCL)/sxsymbol_table.h	\
			c_desambig.c
	$(CC) -o $@ -c c_desambig.c

# debut de code produit par "make-makefile c"

c_t.c: c.bt c.st c.pt c.rt
	tables_c c > $@

c.bt: c.bnf
	bnf c.bnf

c.st: c.bt c.lecl
	lecl c.lecl || [ $$? = 1 ]

c.pt: c.bt
	csynt c || [ $$? = 1 ]

c.rt: c.bt c.st c.recor
	recor c.recor

# fin de code produit par "make-makefile c"

$(C)/dc.out:	$(DC_NEEDS)	$(SXBIBS)
	cc -o $@ $(DC_NEEDS) $(CFLAGS) $(LDFLAGS) $(SXEXTBIBS) || (rm -f $@;exit 1)

$(LIB)/dc_t.o:		$(SXINCL)/sxunix.h 	\
			$(SXINCL)/sxba.h 	\
			dc_t.c
	$(CC) -o $@ -c dc_t.c		\
		-DPARSACT=dc_parsact		\
		-DSEMPASS=dc_smp

$(LIB)/dc_parsact.o:	$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXINCL)/XxY.h		\
			$(SXEXTINCL)/sxsymbol_table.h	\
			dc_parsact.c
	$(CC) -o $@ -c dc_parsact.c


$(LIB)/dc_smp.o:	$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXINCL)/XxY.h		\
			dc_tdef.h		\
			$(SXEXTINCL)/sxsymbol_table.h	\
			dc_smp.c
	$(CC) -o $@ -c dc_smp.c

# debut de code produit par "make-makefile dc"

dc_t.c: dc.bt dc.st dc.pt dc.rt dc.att
	tables_c dc > $@

dc.bt dc.att dc_mix.c: dc.at
	semat dc.at  > dc_mix.c

dc.st: dc.bt dc.lecl
	lecl dc.lecl || [ $$? = 1 ]

dc.pt: dc.bt
	csynt dc || [ $$? = 1 ]

dc.rt: dc.bt dc.st dc.recor
	recor dc.recor

dc_tdef.h: dc.bt dc.tdef
	tdef dc.tdef > $@

# fin de code produit par "make-makefile dc"

$(C)/ndc.out:	$(NDC_NEEDS)	$(SXBIBS)
	cc -o $@ $(NDC_NEEDS) $(CFLAGS) $(LDFLAGS) $(SXEXTBIBS) || (rm -f $@;exit 1)

$(LIB)/ndc_t.o:		$(SXINCL)/sxunix.h 	\
			$(SXINCL)/sxba.h 	\
			ndc_t.c
	$(CC) -o $@ -c ndc_t.c		\
		-Dsxndprecovery=sxndpallcorrections    \
		-DESAMBIG=sxvoid_int		\
		-DPARSACT=ndc_parsact		\
		-DSEMPASS=sxsvoid

$(LIB)/ndc_parsact.o:	$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXINCL)/XxY.h		\
			$(SXEXTINCL)/sxsymbol_table.h	\
			$(SXEXTINCL)/sxnd.h		\
			ndc_parsact.c
	$(CC) -o $@ -c ndc_parsact.c

# debut de code produit par "make-makefile ndc"

ndc_t.c: ndc.bt ndc.st ndc.pt ndc.rt ndc.att
	tables_c ndc > $@

ndc.bt ndc.att ndc_mix.c: ndc.at
	semat ndc.at  > ndc_mix.c

ndc.st: ndc.bt ndc.lecl
	lecl ndc.lecl || [ $$? = 1 ]

ndc.pt: ndc.bt
	csynt ndc || [ $$? = 1 ]

ndc.rt: ndc.bt ndc.st ndc.recor
	recor ndc.recor

ndc_tdef.h: ndc.bt ndc.tdef
	tdef ndc.tdef > $@

# fin de code produit par "make-makefile ndc"

clean:
	rm -f $(ALL)
	rm -f $(C_NEEDS)
	/bin/rm -f c.*.l
	/bin/rm -f c.att c.bt c.pt c.rt c.st
	/bin/rm -f c_mix.c c_t.c
	rm -f $(DC_NEEDS)
	/bin/rm -f dc.*.l
	/bin/rm -f dc.att dc.bt dc.pt dc.rt dc.st
	/bin/rm -f dc_mix.c dc_t.c
	rm -f $(NDC_NEEDS)
	/bin/rm -f ndc.*.l
	/bin/rm -f ndc.att ndc.bt ndc.pt ndc.rt ndc.st
	/bin/rm -f ndc_mix.c ndc_t.c


