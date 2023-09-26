
include $(SX)/etc/make/gccoptions

TOOL	= lustre

all     : $(TOOL)

$(TOOL)	: main.o $(TOOL)_s.o $(TOOL)_t.o
	$(COMPILER) $(CFLAGS) -I$(SX)/incl *.o -L$(SX)/lib  -lsx -lm -o $(TOOL)

main.o	: main.c
	$(COMPILER) $(CFLAGS) -c -I$(SX)/incl main.c -o main.o

$(TOOL)_s.o : $(TOOL)_s.c 
	$(COMPILER) $(CFLAGS) -c \
		-I$(SX)/incl $(TOOL)_s.c -o $(TOOL)_s.o

$(TOOL)_s.c :	$(TOOL).semc
	$(SX)/bin/semc  $(TOOL).semc > $@

$(TOOL)_t.o	: $(TOOL)_t.c 
	$(COMPILER) $(CFLAGS) -c -DSEMACT=$(TOOL)_act \
		-I$(SX)/incl $(TOOL)_t.c -o $(TOOL)_t.o

$(TOOL)_t.c	: $(TOOL).bt $(TOOL).dt  $(TOOL).pt $(TOOL).rt $(TOOL).st 
	$(SX)/bin/tables_c $(TOOL) > $(TOOL)_t.c

$(TOOL).bt	: $(TOOL)_s.o

$(TOOL).dt	: $(TOOL).bt $(TOOL).prio
	$(SX)/bin/prio $(TOOL).prio
	rm -f $(TOOL).pr.l

$(TOOL).pt	: $(TOOL).dt $(TOOL).bt
	$(SX)/bin/csynt $(TOOL)
	rm -f $(TOOL).la.l

$(TOOL).rt	: $(TOOL).bt $(TOOL).st $(TOOL).recor
	$(SX)/bin/recor $(TOOL).recor
	rm -f $(TOOL).rc.l

$(TOOL).st	: $(TOOL).bt $(TOOL).lecl
	$(SX)/bin/lecl $(TOOL).lecl
	rm -f $(TOOL).lc.l

clean	:
	rm -f $(TOOL).bt $(TOOL).dt $(TOOL).pt $(TOOL).rt $(TOOL).st
	rm -f $(TOOL).bn.l $(TOOL).la.l
	rm -f main.o $(TOOL)_s.c $(TOOL)_s.o $(TOOL)_t.c $(TOOL)_t.o $(TOOL)

