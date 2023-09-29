
# warning : these variables must be defined ->
#               $sx  $sxV
# for instance
#  sx    = /usr/local/syntax
#  sxV   = 6.0
include $(sx)/etc/gccoptions
TOOL	= lolo
INCLFLAGS = -I. -I$(sx)/incl
SRCNAME = sem
MORECFLAGS =  -DSEMACT=lolo_act -DPARSACT=parser_act -DSCANACT=scanner_act
LDFLAGS = -L$(sx)/lib -lsx$(sxV) -lm

$(TOOL) : \
	$(TOOL).c \
	$(TOOL).o \
	$(TOOL).st \
	$(TOOL).rt \
	$(TOOL).pt \
	$(TOOL)_t.c \
	$(TOOL)_t.o \
	$(SRCNAME).o 
	$(COMPILER) $(CFLAGS) *.o $(LDFLAGS) -o $(TOOL)


$(TOOL).c :	$(TOOL).tabc
	$(sx)/bin/tabc  $(TOOL).tabc > $@

$(TOOL).o : $(TOOL).c
	@echo CFLAGS ARE $(CFLAGS)
	$(COMPILER) -c $(CFLAGS) $(INCLFLAGS) $< -o $@

$(TOOL).st : $(TOOL).lecl
	$(sx)/bin/lecl $(TOOL).lecl

$(TOOL).rt : $(TOOL).recor
	$(sx)/bin/recor $(TOOL).recor

$(TOOL).pt : $(TOOL).bt 
	$(sx)/bin/csynt $(TOOL)

$(TOOL)_t.c : $(TOOL).rt $(TOOL).st $(TOOL).pt 
	$(sx)/bin/tables_c $(TOOL) > $@

$(TOOL)_t.o : $(TOOL)_t.c	
	$(COMPILER) -c $(CFLAGS) $(INCLFLAGS) $(MORECFLAGS) $< -o $@ 

$(SRCNAME).o : $(SRCNAME).c $(SRCNAME).h
	$(COMPILER) -c $(CFLAGS) $(INCLFLAGS) -o $@ $(SRCNAME).c

mrproper :
	rm -f ./*~ $(TOOL).bt $(TOOL).pt $(TOOL).st $(TOOL).rt $(TOOL).c $(TOOL).o $(TOOL)_t.c $(TOOL).bn.l $(TOOL).lc.l $(TOOL).rc.l $(TOOL)_t.o $(SRCNAME).o

clean :
	rm -f  ./*~ $(TOOL).bt $(TOOL).pt $(TOOL).st $(TOOL).rt $(TOOL).c $(TOOL).o $(TOOL)_t.c $(TOOL).bn.l $(TOOL).lc.l $(TOOL).rc.l $(TOOL)_t.o $(SRCNAME).o $(TOOL)

