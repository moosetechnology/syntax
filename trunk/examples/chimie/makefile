
include $(SX)/etc/make/gccoptions
TOOL	= chimie
CFLAGS += 
# -g

all : $(TOOL)

$(TOOL)	: main.o $(TOOL)_t.o $(TOOL).o
	$(COMPILER) $(CFLAGS) -I$(SXINCL)   *.o -L$(SXLIB)  -lsx -o $(TOOL)

main.o	: main.c
	$(COMPILER) $(CFLAGS) -c -I$(SXINCL)    main.c -o main.o

$(TOOL).bt	: semat

$(TOOL)_t.o	: $(TOOL)_t.c 
	$(COMPILER) $(CFLAGS) -c -DSEMACT=$(TOOL)_act -DSEMPASS=$(TOOL)_smp \
		-I$(SXINCL)    $(TOOL)_t.c -o $(TOOL)_t.o

$(TOOL)_t.c	: $(TOOL).pt $(TOOL).rt $(TOOL).st $(TOOL).bt
	$(SXCOM)/tables_c $(TOOL) > $(TOOL)_t.c

$(TOOL).pt	: $(TOOL).dt $(TOOL).bt
	$(SXCOM)/csynt $(TOOL)
	rm -f $(TOOL).la.l

$(TOOL).rt	: $(TOOL).bt $(TOOL).st $(TOOL).recor
	$(SXCOM)/recor $(TOOL).recor
	rm -f $(TOOL).rc.l

$(TOOL).st	: $(TOOL).bt $(TOOL).lecl
	$(SXCOM)/lecl $(TOOL).lecl
	rm -f $(TOOL).lc.l

$(TOOL).dt	: $(TOOL).bt $(TOOL).prio
	$(SXCOM)/prio $(TOOL).prio
	rm -f $(TOOL).pr.l

$(TOOL).o : $(TOOL).c
	$(COMPILER) $(CFLAGS) -c -I$(SXINCL)    $(TOOL).c -o $(TOOL).o

$(TOOL).c :
	@echo "faites make semat. Attention $(TOOL).c sera ecrase"

semat	: $(TOOL).at
	$(SXCOM)/semat $(TOOL).at > $(TOOL).c
	rm -f $(TOOL).bn.l

clean	:
	rm -f main.o $(TOOL).att $(TOOL).bt $(TOOL).dt $(TOOL).o \
		$(TOOL).c $(TOOL).pt $(TOOL).rt $(TOOL).st $(TOOL)_t* $(TOOL)
