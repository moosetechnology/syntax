TOOL	= chimie
CFLAGS += 
# -g
COMPILER=cc

$(TOOL)	: main.o $(TOOL)_t.o $(TOOL).o
	$(COMPILER) $(CFLAGS) -I$(sx)/incl   *.o -L$(sx)/lib  -lsx$(sxV) -o $(TOOL)

main.o	: main.c
	$(COMPILER) $(CFLAGS) -c -I$(sx)/incl    main.c -o main.o

$(TOOL).bt	: semat

$(TOOL)_t.o	: $(TOOL)_t.c 
	$(COMPILER) $(CFLAGS) -c -DSEMACT=$(TOOL)_act -DSCANACT=scanner_act -Dsempass=$(TOOL)_smp \
		-I$(sx)/incl    $(TOOL)_t.c -o $(TOOL)_t.o

$(TOOL)_t.c	: $(TOOL).pt $(TOOL).rt $(TOOL).st $(TOOL).bt
	$(sx)/bin/tables_c $(TOOL) > $(TOOL)_t.c

$(TOOL).pt	: $(TOOL).dt $(TOOL).bt
	$(sx)/bin/csynt $(TOOL)
	rm -f $(TOOL).la.l

$(TOOL).rt	: $(TOOL).bt $(TOOL).st $(TOOL).recor
	$(sx)/bin/recor $(TOOL).recor
	rm -f $(TOOL).rc.l

$(TOOL).st	: $(TOOL).bt $(TOOL).lecl
	$(sx)/bin/lecl $(TOOL).lecl
	rm -f $(TOOL).lc.l

$(TOOL).dt	: $(TOOL).bt $(TOOL).prio
	$(sx)/bin/prio $(TOOL).prio
	rm -f $(TOOL).pr.l

$(TOOL).o : $(TOOL).c
	$(COMPILER) $(CFLAGS) -c -I$(sx)/incl    $(TOOL).c -o $(TOOL).o

$(TOOL).c :
	@echo "faites make semat. Attention $(TOOL).c sera ecrase"

semat	: $(TOOL).at
	$(sx)/bin/semat $(TOOL).at > $(TOOL).c
	rm -f $(TOOL).bn.l

clean	:
	rm -f main.o $(TOOL).att $(TOOL).bt $(TOOL).dt $(TOOL).o \
		$(TOOL).c $(TOOL).pt $(TOOL).rt $(TOOL).st $(TOOL)_t* $(TOOL) *~
