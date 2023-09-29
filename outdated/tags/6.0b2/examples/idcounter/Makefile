
# warning : these variables must be defined ->
#               $sx     $sxbin  $sxlib  $sxV
# for instance
#  sx    = /usr/local/syntax
#  sxbin = $sx/bin
#  sxlib = $sx/lib
#  sxV   = 6.0

TOOL	= lolo
CFLAGS +=   
SRCNAME = sem
COMPILER=cc
MORECFLAGS =  -DSEMACT=lolo_act -DPARSACT=parser_act -DSCANACT=scanner_act
CFLAGS = -I. -I$(sx)/incl
LDFLAGS = -L$(sxlib) -lsx$(sxV) -lm

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
	$(sxbin)/tabc  $(TOOL).tabc > $@

$(TOOL).o : $(TOOL).c
	$(COMPILER) -c $(CFLAGS) $< -o $@

$(TOOL).st : $(TOOL).lecl
	$(sxbin)/lecl $(TOOL).lecl

$(TOOL).rt : $(TOOL).recor
	$(sxbin)/recor $(TOOL).recor

$(TOOL).pt : $(TOOL).bt 
	$(sxbin)/csynt $(TOOL)

$(TOOL)_t.c : $(TOOL).rt $(TOOL).st $(TOOL).pt 
	$(sxbin)/tables_c $(TOOL) > $@

$(TOOL)_t.o : $(TOOL)_t.c	
	$(COMPILER) -c $(CFLAGS) $(MORECFLAGS) $< -o $@ 

$(SRCNAME).o : $(SRCNAME).c $(SRCNAME).h
	$(COMPILER) -c $(CFLAGS) -o $@ $(SRCNAME).c

mrproper :
	rm -f ./*~ $(TOOL).bt $(TOOL).pt $(TOOL).st $(TOOL).rt $(TOOL).c $(TOOL).o $(TOOL)_t.c $(TOOL).bn.l $(TOOL).lc.l $(TOOL).rc.l $(TOOL)_t.o $(SRCNAME).o

clean :
	rm -f  ./*~ $(TOOL).bt $(TOOL).pt $(TOOL).st $(TOOL).rt $(TOOL).c $(TOOL).o $(TOOL)_t.c $(TOOL).bn.l $(TOOL).lc.l $(TOOL).rc.l $(TOOL)_t.o $(SRCNAME).o $(TOOL)

