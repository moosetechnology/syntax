
include $(SX)/etc/make/gccoptions

REX=$(SX)/../outdated/oldies/reg_exp
PPP=$(SX)/../outdated/oldies/ppp
LIB=$(PPP)/lib

CC=cc -I$(SXINCL) -I$(SXINCL)/util -I$(REX)/incl $(CFLAGS)

all: $(SX)/bin/ppp.out

$(SX)/bin/ppp.out:	$(PPP)/ppp.out
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?

PP=$(LIB)/ppp_t.o		\
   $(LIB)/ppp_main.o		\
   $(LIB)/ppp_pact.o		\
   $(LIB)/ppp_sact.o

$(PPP)/ppp.out:	$(PP)	$(SXBIBS)
	rm -f $@; cc -o $@ $(PP) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@;exit 1)

$(LIB)/ppp_main.o:	$(SXINCL)/sxunix.h		\
			ppp_main.c
	$(CC) -o $@ -c ppp_main.c


$(LIB)/ppp_t.o: 	$(SXINCL)/sxunix.h		\
			ppp_t.c
	$(CC) -o $@ -c ppp_t.c				\
			-Dsxtables=pp_ppp_tables	\
			-DPARSACT=ppp_pars_act		\
			-DSCANACT=pp_ppp_scan_act

$(LIB)/ppp_pact.o:	$(SXINCL)/sxunix.h		\
			ppp_pact.c
	$(CC) -o $@ -c ppp_pact.c

$(LIB)/ppp_sact.o: 	$(SXINCL)/sxunix.h		\
			ppp_sact.c
	$(CC) -o $@ -c ppp_sact.c

# debut de code produit par "make-makefile ppp"

ppp_t.c: ppp.bt ppp.st ppp.pt ppp.rt ppp.ppt
	tables_c ppp > $@

ppp.bt ppp.ppt : ppp.paradis
	paradis ppp.paradis 

ppp.st: ppp.bt ppp.lecl
	lecl ppp.lecl || [ $$? = 1 ]

ppp.pt: ppp.bt
	csynt ppp || [ $$? = 1 ]

ppp.rt: ppp.bt ppp.st ppp.recor
	recor ppp.recor

# fin de code produit par "make-makefile ppp"

clean:
	/bin/rm -f $(PPP)/ppp.*.l
	/bin/rm -f $(PPP)/ppp.bt $(PPP)/ppp.ppt $(PPP)/ppp.pt $(PPP)/ppp.rt 
	/bin/rm -f $(PPP)/ppp.st $(PPP)/ppp_t.c

