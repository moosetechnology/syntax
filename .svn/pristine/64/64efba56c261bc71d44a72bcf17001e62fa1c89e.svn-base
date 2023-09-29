
# warning : the $SX variable must be defined ->
# for instance
#  SX    = /usr/local/syntax

include $(SX)/etc/make/gccoptions

TOOL	= lolo
INCLFLAGS = -I. -I$(SX)/incl
SRCNAME = sem
MORECFLAGS =  -DSEMACT=lolo_act -DPARSACT=parser_act -DSCANACT=scanner_act

all : $(TOOL)

$(TOOL) : \
	$(TOOL).c \
	$(TOOL).o \
	$(TOOL).st \
	$(TOOL).rt \
	$(TOOL).pt \
	$(TOOL)_t.c \
	$(TOOL)_t.o \
	$(SRCNAME).o 
	$(COMPILER) $(CFLAGS) *.o -L$(SX)/lib -lsx -lm -o $(TOOL)

$(TOOL).c :	$(TOOL).semc
	$(SX)/bin/semc  $(TOOL).semc > $@

$(TOOL).o : $(TOOL).c
	@echo CFLAGS ARE $(CFLAGS)
	$(COMPILER) -c $(CFLAGS) $(INCLFLAGS) $< -o $@

$(TOOL).st : $(TOOL).lecl
	$(SX)/bin/lecl $(TOOL).lecl

$(TOOL).rt : $(TOOL).recor
	$(SX)/bin/recor $(TOOL).recor

$(TOOL).pt : $(TOOL).bt 
	$(SX)/bin/csynt $(TOOL)

$(TOOL)_t.c : $(TOOL).rt $(TOOL).st $(TOOL).pt 
	$(SX)/bin/tables_c $(TOOL) > $@

$(TOOL)_t.o : $(TOOL)_t.c	
	$(COMPILER) -c $(CFLAGS) $(INCLFLAGS) $(MORECFLAGS) $< -o $@ 

$(SRCNAME).o : $(SRCNAME).c $(SRCNAME).h
	$(COMPILER) -c $(CFLAGS) $(INCLFLAGS) -o $@ $(SRCNAME).c

clean :
	rm -f ./*~ $(TOOL).bt $(TOOL).pt $(TOOL).st $(TOOL).rt $(TOOL).c $(TOOL).o $(TOOL)_t.c $(TOOL).bn.l $(TOOL).lc.l $(TOOL).rc.l $(TOOL)_t.o $(SRCNAME).o $(TOOL)

