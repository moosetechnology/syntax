# To avoid problems with other shells:
SHELL=/bin/sh

TABCL=your language name
SX=$(sx)
SXI=$(SX)/incl
SXL=$(SX)/lib

BIBS= /usr/local/lib/libsxetc$(sxV).a /usr/local/lib/libsx$(sxV).a

.PRECIOUS:	$(TABCL).out

$(TABCL).out:	$(TABCL)_t.o $(TABCL).o $(SXL)/sxmain.o $(BIBS) 
	cc -o $@ $(TABCL)_t.o $(TABCL).o $(SXL)/sxmain.o \
		 $(BIBS) -lsxetc$(sxV) -lsx$(sxV)

$(TABCL)_t.o:	$(TABCL)_t.c	\
		$(SXI)/sxunix.h
	cc -I$(SXI) -DSEMACT=$(TABCL)_act -c $(TABCL)_t.c 

$(TABCL).o:	$(TABCL).c	\
		$(SXI)/sxunix.h
	cc -I$(SXI) -c $(TABCL).c

$(TABCL)_t.c:	$(TABCL).bt $(TABCL).st $(TABCL).pt $(TABCL).rt
	tables_c $(SXFLAGS) $(TABCL) >$@

$(TABCL).bt:	$(TABCL).tabc
	tabc $(SXFLAGS) $(TABCL).tabc

$(TABCL).st:	$(TABCL).bt $(TABCL).lecl
	lecl $(SXFLAGS) $(TABCL).lecl

$(TABCL).pt:	$(TABCL).bt
	if [ -r $(TABCL).prio ]; then prio $(SXFLAGS) $(TABCL).prio; else exit 0; fi
	csynt -force $(SXFLAGS) $(TABCL)

$(TABCL).rt:	$(TABCL).bt $(TABCL).st $(TABCL).recor
	recor $(SXFLAGS) $(TABCL).recor
