
# This is a sample makefile for using SYNTAX (with SEMC)

# To avoid problems with other shells:
SHELL=/bin/sh

LANGUAGE=your language name
# let $SX refer to the "trunk" directory of the SYNTAX distribution

SXINCL=$(SX)/incl
SXLIB=$(SX)/lib
ARCH=`$(SX)/etc/bin/arch`

BIBS= $(SX)/lib/libsxutil.a $(SX)/lib.$(ARCH)/libsx.a

.PRECIOUS: $(LANGUAGE).out

$(LANGUAGE).out: $(LANGUAGE)_t.o $(LANGUAGE).o $(SXLIB)/sxmain.o $(BIBS) 
	cc -o $@ $(LANGUAGE)_t.o $(LANGUAGE).o $(SXLIB)/sxmain.o $(BIBS)

$(LANGUAGE)_t.o: $(LANGUAGE)_t.c $(SXINCL)/sxunix.h
	cc -I$(SXINCL) -DSEMACT=$(LANGUAGE)_act -c $(LANGUAGE)_t.c 

$(LANGUAGE).o: $(LANGUAGE).c $(SXINCL)/sxunix.h
	cc -I$(SXINCL) -c $(LANGUAGE).c

$(LANGUAGE)_t.c: $(LANGUAGE).bt $(LANGUAGE).st $(LANGUAGE).pt $(LANGUAGE).rt
	tables_c $(SXFLAGS) $(LANGUAGE) >$@

$(LANGUAGE).bt:	$(LANGUAGE).semc
	semc $(SXFLAGS) $(LANGUAGE).semc

$(LANGUAGE).st:	$(LANGUAGE).bt $(LANGUAGE).lecl
	lecl $(SXFLAGS) $(LANGUAGE).lecl

$(LANGUAGE).pt:	$(LANGUAGE).bt
	if [ -r $(LANGUAGE).prio ]; then prio $(SXFLAGS) $(LANGUAGE).prio; else exit 0; fi
	csynt -force $(SXFLAGS) $(LANGUAGE)

$(LANGUAGE).rt:	$(LANGUAGE).bt $(LANGUAGE).st $(LANGUAGE).recor
	recor $(SXFLAGS) $(LANGUAGE).recor

