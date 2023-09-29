
include $(SX)/etc/make/gccoptions
TOOL	= chimie
CFLAGS += 
# -g

all : $(TOOL)

$(TOOL)	: main.o $(TOOL)_t.o $(TOOL).o
	$(COMPILER) $(CFLAGS) -I$(SX)/incl   *.o -L$(SX)/lib  -lsx -o $(TOOL)

main.o	: main.c
	$(COMPILER) $(CFLAGS) -c -I$(SX)/incl    main.c -o main.o

$(TOOL).bt	: semat

$(TOOL)_t.o	: $(TOOL)_t.c 
	$(COMPILER) $(CFLAGS) -c -DSEMACT=$(TOOL)_act -DSCANACT=scanner_act -Dsempass=$(TOOL)_smp \
		-I$(SX)/incl    $(TOOL)_t.c -o $(TOOL)_t.o

$(TOOL)_t.c	: $(TOOL).pt $(TOOL).rt $(TOOL).st $(TOOL).bt
	$(SX)/bin/tables_c $(TOOL) > $(TOOL)_t.c

$(TOOL).pt	: $(TOOL).dt $(TOOL).bt
	$(SX)/bin/csynt $(TOOL)
	rm -f $(TOOL).la.l

$(TOOL).rt	: $(TOOL).bt $(TOOL).st $(TOOL).recor
	$(SX)/bin/recor $(TOOL).recor
	rm -f $(TOOL).rc.l

$(TOOL).st	: $(TOOL).bt $(TOOL).lecl
	$(SX)/bin/lecl $(TOOL).lecl
	rm -f $(TOOL).lc.l

$(TOOL).dt	: $(TOOL).bt $(TOOL).prio
	$(SX)/bin/prio $(TOOL).prio
	rm -f $(TOOL).pr.l

$(TOOL).o : $(TOOL).c
	$(COMPILER) $(CFLAGS) -c -I$(SX)/incl    $(TOOL).c -o $(TOOL).o

$(TOOL).c :
	@echo "faites make semat. Attention $(TOOL).c sera ecrase"

semat	: $(TOOL).at
	$(SX)/bin/semat $(TOOL).at > $(TOOL).c
	rm -f $(TOOL).bn.l

clean	:
	rm -f main.o $(TOOL).att $(TOOL).bt $(TOOL).dt $(TOOL).o \
		$(TOOL).c $(TOOL).pt $(TOOL).rt $(TOOL).st $(TOOL)_t* $(TOOL) *~
