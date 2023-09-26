TOOL	= lustre
CFLAGS  +=
COMPILER=cc
# -g


$(TOOL)	: main.o $(TOOL)_t.o $(TOOL).o
	$(COMPILER) $(CFLAGS) -I$(sx)/incl  -g  *.o -L$(sxlib)  -lsx$(sxV) -o $(TOOL)

main.o	: main.c
	$(COMPILER) $(CFLAGS) -c -I$(sx)/incl  -g  main.c -o main.o

$(TOOL).bt	: semat

$(TOOL)_t.o	: $(TOOL)_t.c 
	$(COMPILER) $(CFLAGS) -c -DSEMACT=$(TOOL)_act -DSCANACT=scanner_act -Dsempass=$(TOOL)_smp \
		-I$(sx)/incl  -g  $(TOOL)_t.c -o $(TOOL)_t.o

$(TOOL)_t.c	: $(TOOL).pt $(TOOL).rt $(TOOL).st $(TOOL).bt
	$(sxbin)/tables_c $(TOOL) > $(TOOL)_t.c

$(TOOL).pt	: $(TOOL).dt $(TOOL).bt
	$(sxbin)/csynt $(TOOL)
	rm -f $(TOOL).la.l

$(TOOL).rt	: $(TOOL).bt $(TOOL).st $(TOOL).recor
	$(sxbin)/recor $(TOOL).recor
	rm -f $(TOOL).rc.l

$(TOOL).st	: $(TOOL).bt $(TOOL).lecl
	$(sxbin)/lecl $(TOOL).lecl
	rm -f $(TOOL).lc.l

$(TOOL).dt	: $(TOOL).bt $(TOOL).prio
	$(sxbin)/prio $(TOOL).prio
	rm -f $(TOOL).pr.l

$(TOOL).o : $(TOOL).c
	$(COMPILER) $(CFLAGS) -c -I$(sx)/incl  -g  $(TOOL).c -o $(TOOL).o

$(TOOL).c :
	@echo "faites make semat. Attention $(TOOL).c sera ecrase"

semat	: $(TOOL).at
	$(sxbin)/semat $(TOOL).at > $(TOOL).c
	rm -f $(TOOL).bn.l

clean	:
	rm -f main.o $(TOOL).att $(TOOL).bt $(TOOL).dt $(TOOL).o \
		$(TOOL).c $(TOOL).pt $(TOOL).rt $(TOOL).st $(TOOL)_t* $(TOOL) *~
